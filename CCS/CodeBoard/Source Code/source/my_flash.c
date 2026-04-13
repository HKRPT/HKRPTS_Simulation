/*
 * flash.c
 *
 *  Created on: 2025年2月5日
 *      Author: BYRD-003
 */
#include "main.h"
#include "my_flash.h"

extern volatile struct SPI_REGS     *SPI_REG[];
extern volatile struct CH_REGS      *DMAChannel[];

FLASH_MSG_t flash_msg[2];                       // 0为发送数组,1为接收数组
FLASH_t     flash_data[2];                      // 0为发送数组,1为接收数组
FLASHU8_t   flash_dataU8[2];                    // 0为发送数组,1为接收数组

uint16_t bootData[128];                         // 写boot数据
uint16_t bootDataRD[128];                       // 读boot数据

#pragma DATA_SECTION(bootData, "flash_ramgs");          // map the TX data to memory
#pragma DATA_SECTION(flash_data, "flash_ramgs");      // map the TX data to memory
#pragma DATA_SECTION(flash_dataU8, "flash_ramgs");      // map the TX data to memory

uint32_t FLASH_SPI_Timeout   = 0.0;         // FRAM SPI  超时计数器 (该计数器应放在中断的1ms计数处)
uint32_t FLASH_SPI_FLAG      = 0.0;         // FRAM SPI  状态位
uint32_t FLASH_SendDataCnt    = 0.0;        // 1ms 中断延迟
// 中断计时函数
void FLASH_contorlTimeCnt(void)
{
    FLASH_SPI_Timeout++;                    // ETH 延迟ms 函数，放在中断内运行
    FLASH_SendDataCnt++;
    if(FLASH_SPI_Timeout >= FLASH_TIMEOUT_LIMIT)FLASH_SPI_Timeout = FLASH_TIMEOUT_LIMIT;
    if(FLASH_SendDataCnt >= FLASH_TIMEOUT_LIMIT)FLASH_SendDataCnt = FLASH_TIMEOUT_LIMIT;
}

// 写单字节
void Flash_WriteOneByte(FLASH_t *buff , uint16_t data)
{
    DMA_head_t head;

    FLASH_SPI_Timeout = 0;

//    FLASH_CS(0);

    buff->head[0] = data << 8;          // 单字节数据

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FLASH_SPI_Handle->SPITXBUF;  // 起始地址
    head.receiveBuf     = (uint16_t*)&flash_data[FLASH_RXBUF_NUM];  // 接收数组
    head.isOneByte      = 1;                                        // 是否为单字节
    head.head_len       = 0;                                        // 包头长度
    head.len            = 1;                                        // 消息大小
    head.dmaTxChannel   = FLASH_DMA_TX_Handle;                      // DMA 发送通道
    head.dmaRxChannel   = FLASH_DMA_RX_Handle;                      // DMA 接收通道
    head.spiChannel     = FLASH_SPI_Handle;                         // SPI通道
    head.timeOutCnt     = &FLASH_SPI_Timeout;                       // 超时计数器
    head.timeOutErrCode = FLASH_TimeOut ;                           // 超时错误代码
    head.timeOutFlag    = &FLASH_SPI_FLAG;                          // 超时标志位
    head.timeOutLimit   = FLASH_TIMEOUT_LIMIT;                      // 超时计数限制
    myDMARecevieSendMsg(&head);

//    FLASH_CS(1);
}

// 读取单字节
uint8_t Flash_ReadOneByte(FLASH_t *buff, uint32_t cmd, uint32_t addr,uint32_t len)
{
    DMA_head_t head;

    FLASH_SPI_Timeout = 0;

//    FLASH_CS(0);
    /*              读操作代码               地址高8位           */
    buff->head[0] = ((cmd) << 8) | (addr >> 16) & 0x00FF;
    buff->head[1] = addr & 0xFF;

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FLASH_SPI_Handle->SPITXBUF;  // 起始地址
    head.receiveBuf     = (uint16_t*)&flash_data[FLASH_RXBUF_NUM];  // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 0;                                        // 包头长度
    head.len            = len;                                      // 消息大小
    head.dmaTxChannel   = FLASH_DMA_TX_Handle;                      // DMA 发送通道
    head.dmaRxChannel   = FLASH_DMA_RX_Handle;                      // DMA 接收通道
    head.spiChannel     = FLASH_SPI_Handle;                         // SPI通道
    head.timeOutCnt     = &FLASH_SPI_Timeout;                       // 超时计数器
    head.timeOutErrCode = FLASH_TimeOut ;                           // 超时错误代码
    head.timeOutFlag    = &FLASH_SPI_FLAG;                          // 超时标志位
    head.timeOutLimit   = FLASH_TIMEOUT_LIMIT;                      // 超时计数限制
    myDMARecevieSendMsg(&head);

//    FLASH_CS(1);

    return flash_data[FLASH_RXBUF_NUM].head[0] & 0xFF;
}

// 写使能
void FLASH_Write_Enable(void)
{
    Flash_WriteOneByte(&flash_data[FLASH_TXBUF_NUM],W25X_WriteEnable);
}

// 写失能
void FLASH_Write_DisEnable(void)
{
    Flash_WriteOneByte(&flash_data[FLASH_TXBUF_NUM], W25X_WriteDisable);
}
// 等待读写完成
void FLASH_WaitForWriteEnd(void)
{
    uint8_t t = 0;

    FLASH_SPI_Timeout = 0;

//    FLASH_CS(0);

    t = Flash_ReadOneByte(&flash_data[FLASH_TXBUF_NUM],W25X_ReadStatusReg,0x00,1);

    while((t & 0x01) != 0)
    {
        t = Flash_ReadOneByte(&flash_data[FLASH_TXBUF_NUM],W25X_ReadStatusReg,0x00,1);
        if(FLASH_SPI_Timeout >= FLASH_TIMEOUT_LIMIT)
        {
            FLASH_SPI_FLAG = FLASH_TimeOut ;
            break;
        }
    }

//    FLASH_CS(1);
}

// 发送数据
void FLASH_Send_Data(uint32_t addr, FLASH_t *buff,uint16_t len)
{
    DMA_head_t head;

    FLASH_SPI_Timeout = 0;

//    FLASH_CS(0);
    /*              写操作代码               地址高16位           */
    buff->head[0] = ((W25X_PageProgram) << 8) | (((uint32_t)addr >> 16) & 0x00FF);
    /*              地址 16位 */
    buff->head[1] = addr & 0xFFFF;

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FLASH_SPI_Handle->SPITXBUF;  // 起始地址
    head.receiveBuf     = (uint16_t*)&flash_data[FLASH_RXBUF_NUM];  // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 2;                                        // 包头长度
    head.len            = len;                                      // 消息大小
    head.dmaTxChannel   = FLASH_DMA_TX_Handle;                      // DMA 发送通道
    head.dmaRxChannel   = FLASH_DMA_RX_Handle;                      // DMA 接收通道
    head.spiChannel     = FLASH_SPI_Handle;                         // SPI通道
    head.timeOutCnt     = &FLASH_SPI_Timeout;                       // 超时计数器
    head.timeOutErrCode = FLASH_TimeOut ;                           // 超时错误代码
    head.timeOutFlag    = &FLASH_SPI_FLAG;                          // 超时标志位
    head.timeOutLimit   = FLASH_TIMEOUT_LIMIT;                      // 超时计数限制
    myDMARecevieSendMsg(&head);

//    FLASH_CS(1);
}

// 发送数据 8位数据
void FLASH_Send_DataU8(uint32_t addr, uint16_t *buff1,uint16_t len)
{
    uint16_t i = 0;
    DMA_head_t head;
    FLASHU8_t *buff     = &flash_dataU8[FLASH_TXBUF_NUM];
    FLASH_SPI_Timeout = 0;

    buff->Instruction = W25X_PageProgram  << 8;                     // 写指令
    buff->address1    = (addr & 0xFF0000) >> 8;
    buff->address2    = (addr & 0x00FF00) >> 0;
    buff->address3    = (addr & 0x0000FF) << 8;                     // 24位地址

    for(i = 0; i < len; i++)
    {
        buff->data[i] = (buff1[i] << 8) & 0xFF00;                   // 数据搬运
    }

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FLASH_SPI_Handle->SPITXBUF;  // 起始地址
    head.receiveBuf     = (uint16_t*)&flash_data[FLASH_RXBUF_NUM];  // 接收数组
    head.isOneByte      = 1;                                        // 是否为单字节
    head.head_len       = 4;                                        // 包头长度
    head.len            = len;                                      // 消息大小
    head.dmaTxChannel   = FLASH_DMA_TX_Handle;                      // DMA 发送通道
    head.dmaRxChannel   = FLASH_DMA_RX_Handle;                      // DMA 接收通道
    head.spiChannel     = FLASH_SPI_Handle;                         // SPI通道
    head.timeOutCnt     = &FLASH_SPI_Timeout;                       // 超时计数器
    head.timeOutErrCode = FLASH_TimeOut ;                           // 超时错误代码
    head.timeOutFlag    = &FLASH_SPI_FLAG;                          // 超时标志位
    head.timeOutLimit   = FLASH_TIMEOUT_LIMIT;                      // 超时计数限制
    myDMARecevieSendMsg(&head);

}

// 读取数据
void FLASH_Read_Data(uint32_t addr, FLASH_t *buff,uint16_t len)
{
    DMA_head_t head;

    FLASH_SPI_Timeout = 0;

//    FLASH_CS(0);
    /*              读操作代码               地址高8位           */
    buff->head[0] = ((W25X_ReadData) << 8) | (((uint32_t)addr >> 16) & 0xFF);
    /*              地址16位       */
    buff->head[1] = addr & 0xFFFF;

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FLASH_SPI_Handle->SPITXBUF;  // 起始地址
    head.receiveBuf     = (uint16_t*)&flash_data[FLASH_RXBUF_NUM];  // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 2;                                        // 包头长度
    head.len            = len;                                      // 消息大小
    head.dmaTxChannel   = FLASH_DMA_TX_Handle;                      // DMA 发送通道
    head.dmaRxChannel   = FLASH_DMA_RX_Handle;                      // DMA 接收通道
    head.spiChannel     = FLASH_SPI_Handle;                         // SPI通道
    head.timeOutCnt     = &FLASH_SPI_Timeout;                       // 超时计数器
    head.timeOutErrCode = FLASH_TimeOut ;                           // 超时错误代码
    head.timeOutFlag    = &FLASH_SPI_FLAG;                          // 超时标志位
    head.timeOutLimit   = FLASH_TIMEOUT_LIMIT;                      // 超时计数限制
    myDMARecevieSendMsg(&head);

//    FLASH_CS(1);
}
// 读取数据
void FLASH_Read_DataU8(uint32_t addr, FLASHU8_t *buff,uint16_t len)
{
    DMA_head_t head;

    FLASH_SPI_Timeout = 0;

    buff->Instruction = W25X_ReadData  << 8;                        // 写指令
    buff->address1    = (addr & 0xFF0000) >> 8;
    buff->address2    = (addr & 0x00FF00) >> 0;
    buff->address3    = (addr & 0x0000FF) << 8;                     // 24位地址

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FLASH_SPI_Handle->SPITXBUF;  // 起始地址
    head.receiveBuf     = (uint16_t*)&flash_dataU8[FLASH_RXBUF_NUM];// 接收数组
    head.isOneByte      = 1;                                        // 是否为单字节
    head.head_len       = 4;                                        // 包头长度
    head.len            = len;                                      // 消息大小
    head.dmaTxChannel   = FLASH_DMA_TX_Handle;                      // DMA 发送通道
    head.dmaRxChannel   = FLASH_DMA_RX_Handle;                      // DMA 接收通道
    head.spiChannel     = FLASH_SPI_Handle;                         // SPI通道
    head.timeOutCnt     = &FLASH_SPI_Timeout;                       // 超时计数器
    head.timeOutErrCode = FLASH_TimeOut ;                           // 超时错误代码
    head.timeOutFlag    = &FLASH_SPI_FLAG;                          // 超时标志位
    head.timeOutLimit   = FLASH_TIMEOUT_LIMIT;                      // 超时计数限制
    myDMARecevieSendMsg(&head);

}

// 擦除扇区
void FLASH_SectorErase(uint32_t addr)
{

    FLASH_Write_Enable();

    FLASH_WaitForWriteEnd();

    Flash_ReadOneByte(&flash_data[FLASH_TXBUF_NUM],W25X_SectorErase,addr,2);

    FLASH_WaitForWriteEnd();

    FLASH_Write_DisEnable();

}
// 擦除扇区
void FLASH_BlockErase(uint32_t addr)
{

    FLASH_Write_Enable();

    FLASH_WaitForWriteEnd();

    Flash_ReadOneByte(&flash_data[FLASH_TXBUF_NUM],W25X_BlockErase,addr,2);

    FLASH_WaitForWriteEnd();

    FLASH_Write_DisEnable();

}

// 读取制造商ID和设备ID
void FLASH_readManufacturerDeviceID(uint8_t *manufacturerID,uint8_t *deviceID)
{

    Flash_ReadOneByte(&flash_data[FLASH_TXBUF_NUM],W25X_ManufactDeviceID,0x00,3);

    *manufacturerID    = (flash_data[FLASH_RXBUF_NUM].data[0] >> 8) & 0xFF;
    *deviceID          = (flash_data[FLASH_RXBUF_NUM].data[0] >> 0) & 0xFF;
}
// 页编程  最多256个字节
void FLASH_Page_Program(uint32_t addr, FLASH_t *buff,uint16_t len)
{
//    if(len > 3) len -= 3;       // 减去包头
//    if(len > 127) return;
    FLASH_Write_Enable();

    FLASH_WaitForWriteEnd();

    FLASH_Send_Data(addr, buff, len);

    FLASH_WaitForWriteEnd();

    FLASH_Write_DisEnable();
}
// 页编程  最多256个字节
void FLASH_Page_ProgramU8(uint32_t addr, uint16_t *buff,uint16_t len)
{
    int8_t t = 0;
    uint8_t z = 0;
//    if(len > 3) len -= 3;       // 减去包头
//    if(len > 127) return;
    FLASH_Write_Enable();

    FLASH_WaitForWriteEnd();

    // 判断是否为跨页写入
    t = 256 - ((addr % 256) + len);
    if(t < -1)
    {
        // 需要跨页写入
        z = 256 - (addr % 256);
        FLASH_Send_DataU8(addr, buff, z);  // 剩余的写入
        FLASH_WaitForWriteEnd();
        FLASH_Send_DataU8(addr+z, &buff[z], len-z);  // 其余的写入
    }
    else
    {
        FLASH_Send_DataU8(addr, buff, len);
    }


    FLASH_WaitForWriteEnd();

    FLASH_Write_DisEnable();
}

// W25Q128 结构体单次写入 data + msg  addr为指针传入，方便改外面指针值
void FLASH_writeOneShot(uint32_t addr,  FLASH_t *data)
{
    FLASH_Page_Program(addr, data, sizeof(data->data));
}
// W25Q128 结构体单次写入 data + msg  addr为指针传入，方便改外面指针值
void FLASH_writeU8data(uint32_t addr,  FLASH_t *data)
{
    FLASH_Page_ProgramU8(addr, data->data, sizeof(data->data));
}

// W25Q128 结构体单次读取
void FLASH_readOneShot(uint32_t addr,  FLASH_t *data)
{
    FLASH_Read_Data(addr, data, sizeof(data->data));
}

uint32_t addr               = 0x0000;
// W25Q128  测试函数
void FLASH_Test(void)
{
//    FLASH_SectorErase(addr);

//    FLASH_writeU8data(addr,&flash_data[FLASH_TXBUF_NUM]);
//    FLASH_writeOneShot(addr,&flash_data[FLASH_TXBUF_NUM]);
//    FLASH_readOneShot(addr,&flash_data[FLASH_RXBUF_NUM]);
//    FLASH_readOneShot(addr,&flash_data[FLASH_RXBUF_NUM]);
}













