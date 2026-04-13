/*
 * FRAM.c
 *
 *  Created on: 2025年1月17日
 *      Author: BYRD-003
 */
#include "main.h"
#include "string.h"
#include "my_fram.h"

Fram_t  fram[2];                         // 0为发送数组,1为接收数组
#pragma DATA_SECTION(fram, "ramgs0");    // map the TX data to memory

uint32_t FRAM_SPI_Timeout   = 0.0;      // FRAM SPI  超时计数器 (该计数器应放在中断的1ms计数处)
uint32_t FRAM_SPI_FLAG      = 0.0;      // FRAM SPI  状态位
uint8_t FRAMRecDataFlag     = 1;        // FRAM 接收标志位
uint32_t FRAMSendDataCnt    = 0.0;      // 1ms 中断延迟
// 中断计时函数
void Fram_contorlTimeCnt(void)
{
    FRAM_SPI_Timeout++;                // ETH 延迟ms 函数，放在中断内运行
    FRAMSendDataCnt++;
    if(FRAM_SPI_Timeout >= FRAM_TIMEOUT_LIMIT)FRAM_SPI_Timeout = FRAM_TIMEOUT_LIMIT;
    if(FRAMSendDataCnt >= FRAM_TIMEOUT_LIMIT)FRAMSendDataCnt = FRAM_TIMEOUT_LIMIT;
}
// 读取单字节
void FRAM_WriteOneByte(Fram_t *buff, uint16_t data)
{
    DMA_head_t head;

    FRAM_SPI_Timeout = 0;

//    FRAM_CS(0);

    buff->head[0] = data << 8;          // 单字节数据

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FRAM_SPI_Handle->SPITXBUF;   // 起始地址
    head.receiveBuf     = (uint16_t*)&fram[FRAM_RXBUF_NUM];         // 接收数组
    head.isOneByte      = 1;                                        // 是否为单字节
    head.head_len       = 0;                                        // 包头长度
    head.len            = 1;                                        // 消息大小
    head.dmaTxChannel   = FRAM_DMA_TX_Handle;                       // DMA 发送通道
    head.dmaRxChannel   = FRAM_DMA_RX_Handle;                       // DMA 接收通道
    head.spiChannel     = FRAM_SPI_Handle;                          // SPI通道
    head.timeOutCnt     = &FRAM_SPI_Timeout;                        // 超时计数器
    head.timeOutErrCode = Fram_TimeOut ;                            // 超时错误代码
    head.timeOutFlag    = &FRAM_SPI_FLAG;                           // 超时标志位
    head.timeOutLimit   = FRAM_TIMEOUT_LIMIT;                       // 超时计数限制
    myDMARecevieSendMsg(&head);

//    FRAM_CS(1);

}

// 写使能
void FRAM_Write_Enable(void)
{
    FRAM_WriteOneByte(&fram[FRAM_TXBUF_NUM],0x06);
}

// 写失能
void FRAM_Write_DisEnable(void)
{
    FRAM_WriteOneByte(&fram[FRAM_TXBUF_NUM],0x04);

}

// 发送数据
void Fram_Send_Data(uint16_t addr, Fram_t *buff,uint16_t len)
{
    DMA_head_t head;

    FRAM_SPI_Timeout = 0;

    FRAM_Write_Enable();

    FRAM_CS(0);
    /*              写操作代码               地址高8位           */
    buff->head[0] = ((0x02) << 8) | ((addr >> 8) & 0xFF);
    /*              地址低8位               无效字符           */
    buff->head[1] = ((addr << 8 ) & 0xFF00) | ((0x16) & 0xFF);

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FRAM_SPI_Handle->SPITXBUF;   // 起始地址
    head.receiveBuf     = (uint16_t*)&fram[FRAM_RXBUF_NUM];         // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 2;                                        // 包头长度
    head.len            = len;                                      // 消息大小
    head.dmaTxChannel   = FRAM_DMA_TX_Handle;                       // DMA 发送通道
    head.dmaRxChannel   = FRAM_DMA_RX_Handle;                       // DMA 接收通道
    head.spiChannel     = FRAM_SPI_Handle;                          // SPI通道
    head.timeOutCnt     = &FRAM_SPI_Timeout;                        // 超时计数器
    head.timeOutErrCode = Fram_TimeOut ;                            // 超时错误代码
    head.timeOutFlag    = &FRAM_SPI_FLAG;                           // 超时标志位
    head.timeOutLimit   = FRAM_TIMEOUT_LIMIT;                       // 超时计数限制
    myDMARecevieSendMsg(&head);

    FRAM_CS(1);

    FRAM_Write_DisEnable();

}

void Fram_Read_Data(uint16_t addr, Fram_t *buff,uint16_t len)
{
    DMA_head_t head;

    FRAM_SPI_Timeout = 0;

    FRAM_CS(0);
    /*              读操作代码               地址高8位           */
    buff->head[0] = ((0x03) << 8) | ((addr >> 8) & 0xFF);
    /*              地址低8位               无效字符           */
    buff->head[1] = ((addr << 8 ) & 0xFF00) | ((0x16) & 0xFF);

    head.dec_addr       = (uint16_t *)buff;                         // 终地址
    head.src_addr       = (uint16_t *)&FRAM_SPI_Handle->SPITXBUF;   // 起始地址
    head.receiveBuf     = (uint16_t*)&fram[FRAM_RXBUF_NUM];         // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 2;                                        // 包头长度
    head.len            = len;                                      // 消息大小
    head.dmaTxChannel   = FRAM_DMA_TX_Handle;                       // DMA 发送通道
    head.dmaRxChannel   = FRAM_DMA_RX_Handle;                       // DMA 接收通道
    head.spiChannel     = FRAM_SPI_Handle;                          // SPI通道
    head.timeOutCnt     = &FRAM_SPI_Timeout;                        // 超时计数器
    head.timeOutErrCode = Fram_TimeOut ;                            // 超时错误代码
    head.timeOutFlag    = &FRAM_SPI_FLAG;                           // 超时标志位
    head.timeOutLimit   = FRAM_TIMEOUT_LIMIT;                       // 超时计数限制
    myDMARecevieSendMsg(&head);

    FRAM_CS(1);
}


void FRAM_Contorl(void)
{
    extern void SPI_RX_ArrayInit(uint16_t port);
    extern void SPI_TX_ArrayInit(uint16_t port);
    extern void DMA_Channel_ReStart(uint16_t port);

    switch(FRAM_SPI_FLAG)
    {
    case Fram_Normal:
        // 第一次上电时,读取FRAM数据。或有需要时，读取数据
        if(FRAMRecDataFlag)
        {
            // CRC 不通过，跳转到CRCError
            FRAM_SPI_FLAG = Fram_CRCRxError;

            FRAMRecDataFlag = 0;
        }
        if(FRAMSendDataCnt >= 1000)     // 10s
        {
            // 组包

            // CRC

            // 1s 发送数据

            FRAMSendDataCnt  = 0;
        }
        break;
    case Fram_CRCTxError:
        // 清除发送数据
        FRAM_ArrayInit(FRAM_TXBUF_NUM);

        FRAM_SPI_FLAG = Fram_Normal;     // 重新进入正常模式

        break;
    case Fram_CRCRxError:
        // 清除接收数据
        FRAM_ArrayInit(FRAM_RXBUF_NUM);

        FRAM_SPI_FLAG = Fram_Normal;     // 重新进入正常模式

        break;
    case Fram_ERROR:
        // 此处处理SPI DMA异常信息
        // 解决SPI DMA异常的方式，重启DMA和SPI
        SPI_RX_ArrayInit(2);
        SPI_TX_ArrayInit(2);
        DMA_Channel_ReStart(DMA_SPIC_RX);
        DMA_Channel_ReStart(DMA_SPIC_TX);

        FRAM_SPI_FLAG = Fram_Normal;     // 重新进入正常模式
        break;
    case Fram_TimeOut:
        // 系统应当报警告，后置为错误
        FRAM_SPI_FLAG = Fram_ERROR;
        break;
    default:
        FRAM_SPI_FLAG = Fram_Normal;      // 未知状态进入
        break;
    }
}

// FRAM 数组初始化
void FRAM_ArrayInit(uint16_t port)
{
    memset((char*)&fram[port],0,sizeof(fram[port]));
}

// FRAM测试函数
void FRAM_Test(void)
{

    Fram_Send_Data(0x0000,&fram[FRAM_TXBUF_NUM],sizeof(fram[FRAM_TXBUF_NUM].data));
    Fram_Read_Data(0x0000,&fram[FRAM_RXBUF_NUM],sizeof(fram[FRAM_RXBUF_NUM].data));

//    DELAY_US(500 * 1000);
}













