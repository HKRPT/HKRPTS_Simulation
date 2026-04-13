/*
 * bootloader.c
 *
 *  Created on: 2026年2月6日
 *      Author: YangHaoYan
 */
#include "bootloader.h"
#include "my_flash.h"
#include "crc16-ccitt.h"

typedef  void (*pFunction)(void);

uint8_t bootSendBuf[8] = {0};

uint8_t bootMode    = 0x00; // boot模式 ： 0为APP, 1为Boot 2 为Boot Eth
uint8_t vaildLen    = 0x00; // 固件有效数据长度
uint8_t ComplateCRCFlag = 0x00; // 整包CRC 标志位
uint8_t ComplateCRC     = 0x00; // 整包CRC
uint8_t ComplateCRCRemote     = 0x00; // 整包CRC(远程发送下来的校验CRC)

bootloaderMsg_t bootloaderMsg;          // 编译信息
bootloaderMsg_t bootloaderRDMsg;        // 读取编译信息

//uint32_t app_flag = 0x010216;
//#pragma DATA_SECTION(app_flag, "app_flagfile");


extern uint16_t (*modbusMasterReturnFrame[MODBUS_NUM])(modbusMasterData_t* );
// BootLoader帧返回
uint16_t BootLoaderReturnFrame(modbusMasterData_t* data)
{
    uint16_t my_crc = 0x00;
    uint16_t i      = 0;
    uint16_t *buff, *flag, *length;
    uint16_t len    = data->targetlen;  // 目标长度

    if(data->TxStartFlag != 0x01) return ERROR;       // 未使能发送

    buff    = data->Txbuff;         // 数组缓存区
    length  = data->Txlen;          // 数据长度
    flag    = data->exTxFlag;       // 外部发送标志位


    if(*flag != 0x01)return ERROR;  // 尚有数据未发送完成

    if(data->type == MD_UART || data->type == MD_W5500)
    {
        buff[i++] = 0x5A;
    }

    // 添加帧数据
    while(len--)
    {
        buff[i++] = *(data->targetReg) & 0x00FF;      // 目标寄存器值 (低位)
        data->targetReg++;
    }

    if(data->type == MD_UART || data->type == MD_W5500)
    {

        my_crc = usMBCRC16(buff, i);    // CRC验证
        buff[i++] = (my_crc & 0x00FF) >> 0;
        buff[i++] = (my_crc & 0xFF00) >> 8;
    }


    *length = i;                    // 长度
    *flag   = 0;                    // 外部发送标志位

    return NO_ERROR;
}

uint32_t bootAddr   = 0x00;             // 缓存区地址
uint16_t bootLen    = 0x00;             // 缓存区长度
uint32_t bootLenAll = 0x00;             // 缓存区总长度
uint8_t bootTemp[256] = {0};             // 缓存区
uint8_t  packageLen = 0x00;             // 包计数
uint32_t dataAddr   = 0x00;             // 固件数据偏移地址
bootStatus_t bootStatus;                // boot传输状态

// bootloader数据存储(单个字节存放到缓存区)
void bootLoaderDataStoreSingleByte(uint8_t data)
{
    if(bootLen >= (vaildLen - 2))
    {
        // 超范围报错
//        bootStatus = BOOTExcessLenROM;
    }
    else
    {
        bootTemp[bootLen++]  = data;
    }
}

// 存储器存入数据组包
uint8_t bootLoaderDataCombine(void)
{
    uint8_t i = 0;
    uint16_t *buff = flash_dataU8[FLASH_TXBUF_NUM].data;
    for(i = 0; i < bootLen; i++)
    {
        *(buff++) = bootTemp[i];       // 将数据放在原来的位置
    }
    return bootLen;
}

// CRC
uint8_t bootLoaderDataStoreCRC(uint16_t remote_crc)
{
    uint16_t my_crc     = 0x00;

//    my_crc = usMBCRC16(bootTemp, bootLen);
    my_crc = (uint16_t)crc16_ccitt((unsigned char *)bootTemp, bootLen, 0x00);
    ComplateCRCRemote = (uint16_t)crc16_ccitt((unsigned char *)bootTemp, bootLen, ComplateCRCRemote);
//    my_crc = usMBCRC16(bootTemp, bootLen);
    if(my_crc != remote_crc)
    {
        bootStatus = BOOTErrorCRCData;
        return 0x00;
    }
    else
    {
        return 0x01;    // CRC通过
    }
}

// 清除缓存区
void BootLoaderCleanTempZone(void)
{
//    bootMode    = 0x00;
    bootLen             = 0x00;
    bootLenAll          = 0x00;
    ComplateCRCRemote   = 0x00;
    ComplateCRCFlag     = 0x00;
    bootStatus          = BOOTSuceessWriteROM;
}

// bootLoader在线解析函数
void BootLoaderFrameFunc(modbusMasterData_t* data)
{
    extern void FLASH_Page_ProgramU8(uint32_t addr, uint16_t *buff,uint16_t len);

    uint8_t i = 0x00;
    uint16_t my_crc     = 0x00;
    uint16_t remote_crc = 0x00;
    uint8_t *buff       = data->Rxbuff;
    uint16_t len        = *(data->Rxlen);
    uint32_t datalen    = 0x00;              // 缓冲区数大小或接收数据大小
    uint8_t step        = 0x00;

    if(data->RxOverFlag != 0x01) return ;    // 接收未完成

    if(data->type == MD_UART || data->type == MD_W5500)
    {
        if(len != 11)
        {
            data->RX_ArrayInit(data->port);     // 未知协议弃之
            return;
        }

        my_crc = usMBCRC16(buff, len - 2);
        remote_crc = ((buff[len - 1] & 0xFF)  << 8)| buff[len - 2] & 0xFF;     // 远程CRC
        if(my_crc != remote_crc)
        {
            // CRC 校验不对
            data->RX_ArrayInit(data->port);     // 未知协议弃之
            return;
        }
        // 偏移数组
        buff       = &data->Rxbuff[1];
    }
    else
    {
        // CAN
    }

    // 判断节点ID 0x7F为广播ID
    if(buff[0] == CID || buff[0] == 0x7F)
    {
        step = (buff[1] >> 4) & 0x0F;
        if(step == 0x00)
        {
            // 由SID 进行判断
            switch(buff[2])
            {
            case 0xB2:
                // 获取固件信息
                bootSendBuf[0] = CID;               // NAD
                bootSendBuf[1] = 0x06;              // PCI
                bootSendBuf[2] = 0xF2;              // RSID
                bootSendBuf[3] = (bootMode == 0x00) ?0xAA:0x55;    // D1 当前固件类型
                bootSendBuf[4] = Major;             // D2 固件版本号
                bootSendBuf[5] = Minor;             // D3 固件版本号
                bootSendBuf[6] = Revision;          // D4 固件版本号
                bootSendBuf[7] = Build;             // D5 固件版本号

                data->TxStartFlag   = 0x01;         // 发送标志位
                data->targetlen     = 0x08;         // 目标长度
                data->targetReg     = bootSendBuf;  // 目标寄存器
                modbusMasterReturnFrame[data->port] = BootLoaderReturnFrame;
                break;
            case 0xB3:
                // 进入BOOT模式
                if(data->type == MD_W5500)
                {
                    bootMode = 0x02;
                }
                else
                {
                    bootMode = 0x01;
                }
                BootLoaderCleanTempZone();
                break;
            case 0xB4:
                if(buff[3] == 0x42)
                {
                   // 擦除APP程序区数据
                   FLASH_BlockErase(BootLoaderFlashBootArea1);
                   FLASH_BlockErase(BootLoaderFlashBootArea2);
                   FLASH_BlockErase(BootLoaderFlashBootArea3);

                   // 清除状态
                   BootLoaderCleanTempZone();

                   // 擦除APP程序区数据
                   bootSendBuf[0] = CID;               // NAD
                   bootSendBuf[1] = 0x06;              // PCI
                   bootSendBuf[2] = 0xF4;              // RSID
                   bootSendBuf[3] = 0x00;              // STATUS：固件擦除状态，0-固件擦除成功，1-固件擦除出错，2-当前模式为APP，不能擦除固件
                   bootSendBuf[4] = 0xFF;              // D2 固件版本号
                   bootSendBuf[5] = 0xFF ;             // D3 固件版本号
                   bootSendBuf[6] = 0xFF;              // D4 固件版本号
                   bootSendBuf[7] = 0xFF;              // D5 固件版本号

                   data->TxStartFlag   = 0x01;         // 发送标志位
                   data->targetlen     = 0x08;         // 目标长度
                   data->targetReg     = bootSendBuf;  // 目标寄存器
                   modbusMasterReturnFrame[data->port] = BootLoaderReturnFrame;
                }
                else if(buff[3] == 0x03)
                {
                    // 固件数据偏移地址 bootLoaderAddr
                    dataAddr = ((uint32_t)buff[4] << 24 | (uint32_t)buff[5] << 16 | (uint32_t)buff[6] << 8 | (uint32_t)buff[7] << 0);
                    datalen  = sizeof(flash_dataU8[FLASH_TXBUF_NUM].data) - bootLen;

                    bootSendBuf[0] = CID;               // NAD
                    bootSendBuf[1] = 0x06;              // PCI
                    bootSendBuf[2] = 0xF4;              // RSID
                    bootSendBuf[3] = (ProgramVersion == 0xAA) ?((dataAddr > bootLoaderTopAddr) ? 0x03 : 0x00):0x04;    // D1 STATUS：0-准备好接收数据，3-当前地址超出了正常的地址范围，4-当前模式不能写入固件数据
                    bootSendBuf[4] = (datalen >> 8) & 0XFF; // 接收数据缓冲区大小bit[15..8]位
                    bootSendBuf[5] = (datalen >> 0) & 0XFF; // 接收数据缓冲区大小bit[7..0]位
                    bootSendBuf[6] = 0xFF;              // D4
                    bootSendBuf[7] = 0xFF;              // D5

                    data->TxStartFlag   = 0x01;         // 发送标志位
                    data->targetlen     = 0x08;         // 目标长度
                    data->targetReg     = bootSendBuf;  // 目标寄存器
                    modbusMasterReturnFrame[data->port] = BootLoaderReturnFrame;
                }
                else if (buff[3] == 0xC4)
                {
                    // 数据小于等于4字节
                    vaildLen = buff[1] & 0x0F;                          // 有效数据
                    for(i = 0; i < vaildLen; i++)
                    {
                        bootLoaderDataStoreSingleByte(buff[4 + i]);     // 存入单字节
                    }
                }
                else if (buff[3] == 0x85)
                {
                    // 获取CRC数据进行CRC校验
                    if(bootLoaderDataStoreCRC(((buff[4] & 0xFF)  << 8)| buff[5] & 0xFF))
                    {
                        // CRC 校验通过，进行组包进存储器
                        bootLoaderDataCombine();
                        bootAddr = bootLoaderAddr + bootLenAll;     // 当前要写入的位置

                        FLASH_Page_ProgramU8(bootAddr, bootTemp,bootLen);
                        bootLenAll += bootLen;  // 记录下一次地址位置
                        bootLen = 0;            // 清除缓冲区
                    }
                    // 返回数据
                    bootSendBuf[0] = CID;               // NAD
                    bootSendBuf[1] = 0x06;              // PCI
                    bootSendBuf[2] = 0xF4;              // RSID
                    bootSendBuf[3] = bootStatus;        // STATUS：0-数据成功写入程序存储器，4-当前模式不能写入固件数据，5-数据写入程序存储器出错，6-数据长度超出了程序存储器范围，7-数据传输CRC校验出错，8-数据写入芯片CRC校验出错
                    bootSendBuf[4] = 0xFF;              // D2
                    bootSendBuf[5] = 0xFF;              // D3
                    bootSendBuf[6] = 0xFF;              // D4
                    bootSendBuf[7] = 0xFF;              // D5

                    data->TxStartFlag   = 0x01;         // 发送标志位
                    data->targetlen     = 0x08;         // 目标长度
                    data->targetReg     = bootSendBuf;  // 目标寄存器
                    modbusMasterReturnFrame[data->port] = BootLoaderReturnFrame;
                }
                else if (buff[3] == 0x06)
                {
                    bootMode        = 0x00;
                    ComplateCRCFlag = 0x01;
                    // 执行APP固件
//                    BootLoaderCleanTempZone();
                }
                break;
            default:
                break;
            }
        }
        else if(step == 0x01)
        {
           // 大于4字节
           if(buff[3] == 0xB4 && buff[4] == 0xC4)
           {
               vaildLen = ((buff[1] & 0x0F) << 8) | buff[2] & 0xFF;    // 有效数据
               for(i = 0; i < 3; i++)
               {
                   bootLoaderDataStoreSingleByte(buff[5 + i]);         // 存入单字节
               }
           }
        }
        else if(step == 0x02)
        {
            packageLen = buff[1] & 0x0F;                            // 包数量
            for(i = 0; i < 6; i++)
            {
                bootLoaderDataStoreSingleByte(buff[2 + i]);         // 存入单字节
            }
        }
        else
        {
            data->RX_ArrayInit(data->port);     // 其它节点弃之
        }
    }
    else
    {
        data->RX_ArrayInit(data->port);     // 其它节点弃之
    }

    data->RX_ArrayInit(data->port);

    return ;
}


// 从FLASH 中读取出来，逐个验证CRC(crc16_ccitt)
void BootLoaderReadFlashToComplateCRC(void)
{
    uint32_t times      = 0x00;
    uint32_t times1     = 0x00;
    static uint32_t i   = 0x00;
    static uint8_t end  = 0x00;
    uint32_t addr       = 0x00;
    static uint8_t step = 0x00;
    static uint16_t crc = 0x00;
    static uint32_t curtimes = 0x00;

    if (ComplateCRCFlag == 0x00) return;        // 状态未激活
    if (bootLenAll == 0x00) return;             // 若未接收到数据就退出

    times   = bootLenAll / 256;                 // 得出需要读出多少个section
    times1  = bootLenAll % 256;                 // 得出需要读出多少个section

    addr  = bootLoaderAddr;                     // 首地址

    switch(step)
    {
    case 0:
        // 从flash中读取数据
        if(i < times)
        {
            FLASH_Read_DataU8(addr + i * 256, &flash_dataU8[FLASH_TXBUF_NUM], 256);
            curtimes = 256;
            i++;
            if(times1 == 0) end = 1;
        }
        else
        {
            FLASH_Read_DataU8(addr + times * 256, &flash_dataU8[FLASH_TXBUF_NUM], times1); // 读出剩余字节
            curtimes = times1;
            end = 1;
            i = 0;
        }
        step++; // 步骤
        break;
    case 1:
        // CRC校验
        crc = (uint16_t)crc16_ccitt((unsigned char *)flash_dataU8[FLASH_RXBUF_NUM].data, curtimes, crc);

        if(end == 1)
        {
            step = 0x02;
        }
        else
        {
            step = 0x00;
        }
        break;
    case 2:
        if(crc != ComplateCRCRemote)    step = 0;           // 校验不通过弃掉
        // 外部赋值
        ComplateCRC = crc;
        bootloaderMsg.ComplateCRC       = ComplateCRC;      // 完整包CRC
        bootloaderMsg.ComplateLen       = bootLenAll;       // 完整包长度
        bootloaderMsg.ComplateTime      = 0x010216;         // 完整包时间
        bootloaderMsg.ComplateTopAddr   = bootLoaderAddr;   // 完整包首地址
        bootloaderMsg.ComplateUpdateFlag= 0x01;             // 更新标志位
        bootloaderMsg.ComplateVersion   = 0x010216;         // 完整包版本
        crc = 0;
        end = 0;
        curtimes = 0;
        step++;
        break;
    case 3:
        // 擦除FLASH
        FLASH_SectorErase(BootLoaderFlashMsgArea3);
        bootloaderMsg.CRC = (uint16_t)crc16_ccitt((unsigned char *)&bootloaderMsg, sizeof(bootloaderMsg) - 1, 0x00);    // 除了CRC位都要进行校验
        step++;
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        step++;
        break;
    case 8:
        // 存入FLASH
        memcpy(flash_data[FLASH_TXBUF_NUM].data, &bootloaderMsg, sizeof(bootloaderMsg));
        FLASH_Page_Program(BootLoaderFlashMsgArea3, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderMsg));
        step++;
        break;
    case 9:
    case 10:
    case 11:
        step++;
        break;
    case 12:
        BootLoaderCleanTempZone();
        ComplateCRCFlag = 0x00;
        step = 0;
        break;
    default:
        step = 0;
        break;
    }
    return;
}


/*
 * 返回值
 * 0：未处理完毕
 * 1：处理完毕
 * 2：app已被刷新但是校验不过
 * 3：未校验过，但app未被刷新
 *
 * */
// Boot 函数读取FLASH并写入程序区
uint16_t BootLoaderReadFlashToComplateCRCBoot(uint32_t boot_addr)
{
    uint32_t times      = 0x00;
    uint32_t times1     = 0x00;
    static uint32_t i   = 0x00;
    static uint8_t end  = 0x00;
    uint32_t addr       = 0x00;
    uint32_t addr1      = 0x00;
    static uint8_t step = 0x00;
    static uint16_t crc = 0x00;
    static uint32_t curtimes = 0x00;
    uint8_t ret         = 0x00;

    times   = bootloaderRDMsg.ComplateLen / 256;                 // 得出需要读出多少个section
    times1  = bootloaderRDMsg.ComplateLen % 256;                 // 得出需要读出多少个section

    addr  = boot_addr;                                          // 首地址

    switch(step)
    {
    case 0:
        // 从flash中读取数据
        if(i < times)
        {
            FLASH_Read_DataU8(addr + i * 256, &flash_dataU8[FLASH_TXBUF_NUM], 256);
            curtimes = 256;
            i++;
            if(times1 == 0) end = 1;
        }
        else
        {
            FLASH_Read_DataU8(addr + times * 256, &flash_dataU8[FLASH_TXBUF_NUM], times1); // 读出剩余字节
            curtimes = times1;
            end = 1;
            i = 0;
        }
        step++; // 步骤
        break;
    case 1:
        // CRC校验
        crc = (uint16_t)crc16_ccitt((unsigned char *)flash_dataU8[FLASH_RXBUF_NUM].data, curtimes, crc);

        if(end == 1)
        {
            end  = 0x00;
            step = 0x02;
        }
        else
        {
            step = 0x00;
        }
        break;
    case 2:
        if(bootloaderRDMsg.ComplateCRC != crc)
        {
            step = 0x00;
            ret  = 0x03;
        }
        else
        {
            // 擦除 dsp flash和 外部flash
            if(addr == BootLoaderFlashBootArea1)
            {
                // 备用才启用
                FLASH_BlockErase(BootLoaderFlashBootArea1 + BootLoaderFlashBootBackupArea);
                FLASH_BlockErase(BootLoaderFlashBootArea2 + BootLoaderFlashBootBackupArea);
                FLASH_BlockErase(BootLoaderFlashBootArea3 + BootLoaderFlashBootBackupArea);
                FLASH_BlockErase(BootLoaderFlashMsgArea3  + BootLoaderFlashBootBackupArea);
            }
            step++;
        }
        crc = 0;
        break;
    case 3:
        // 从flash中读取数据
        if(i < times)
        {
            addr1 = addr + i * 256;
            FLASH_Read_DataU8(addr1, &flash_dataU8[FLASH_TXBUF_NUM], 256);
            curtimes = 256;
            i++;
            if(times1 == 0) end = 1;
        }
        else
        {
            addr1 = addr + times * 256;
            FLASH_Read_DataU8(addr1, &flash_dataU8[FLASH_TXBUF_NUM], times1); // 读出剩余字节
            curtimes = times1;
            end = 1;
            i = 0;
        }
        step++; // 步骤
        break;
    case 4:
        // CRC校验
        crc = (uint16_t)crc16_ccitt((unsigned char *)flash_dataU8[FLASH_RXBUF_NUM].data, curtimes, crc);

        if(addr == BootLoaderFlashBootArea1) addr1 += BootLoaderFlashBootBackupArea;    // 备份时启用
        // 写入 DSP 程序区,同时写入FLASH
        FLASH_Page_ProgramU8(addr1, flash_dataU8[FLASH_RXBUF_NUM].data, curtimes);

        if(end == 1)
        {
            addr1   = 0x00;
            end     = 0x00;
            step    = 0x05;
        }
        else
        {
            step = 0x03;
        }
        break;
    case 5:
        if(bootloaderRDMsg.ComplateCRC != crc)
        {
            // 写入的CRC不同于 校验值，APP可能跑不起来
            ret = 2;    // 按道理启用FLASH中备份程序
        }
        else
        {
            if(addr == BootLoaderFlashBootArea1)
            {
                // 存入FLASH 备份
                memcpy(flash_data[FLASH_TXBUF_NUM].data, &bootloaderRDMsg, sizeof(bootloaderRDMsg));
                FLASH_Page_Program(BootLoaderFlashMsgArea3 + BootLoaderFlashBootBackupArea, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderRDMsg));
            }

            ret = 1;
        }
        crc = 0;
        end = 0;
        curtimes = 0;
        step = 0;
        break;
    default:
        step = 0;
        break;
    }
    return ret;
}

uint8_t ComplateMsgFlag = 0x01;

// 从FLASH 中读取出来，MSG信息
uint16_t BootLoaderReadFlashToComplateMsg(uint32_t boot_addr, uint32_t msg_addr)
{
    static uint8_t step = 0x00;
    uint16_t my_crc = 0x00;
    uint16_t ret    = 0x00;
    if (ComplateMsgFlag == 0x00) return 0x00;        // 状态未激活

    switch(step)
    {
    case 0:
        // 从FLASH中提取信息
        FLASH_Read_Data(msg_addr, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderRDMsg));
        step++;
        break;
    case 1:
        // 复制到对应区域
        memcpy(&bootloaderRDMsg,flash_data[FLASH_RXBUF_NUM].data,  sizeof(bootloaderRDMsg));
        step++;
        break;
    case 2:
        // CRC校验
        my_crc = (uint16_t)crc16_ccitt((unsigned char *)&bootloaderRDMsg, sizeof(bootloaderRDMsg) - 1, 0x00);    // 除了CRC位都要进行校验
        if(bootloaderRDMsg.CRC == my_crc && (bootloaderRDMsg.ComplateUpdateFlag == 0x01))
        {
            step++;
        }
        else
        {
            ComplateMsgFlag = 0x00; // 失败校验,和更新标志位
            step    = 0;
            ret     = 0x03;
        }
        break;
    case 3:
        bootloaderRDMsg.ComplateUpdateFlag = 0x00;  // 更新标志清零
        memcpy(flash_data[FLASH_TXBUF_NUM].data, &bootloaderRDMsg, sizeof(bootloaderRDMsg));
        FLASH_Page_Program(msg_addr, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderRDMsg));   // 存回原来的位置
        step++;
        break;
    case 4:
        // 校验通过读取FLASH信息
        ret = BootLoaderReadFlashToComplateCRCBoot(boot_addr);
        if(ret != 0x00)
        {
            step = 0;
            ComplateMsgFlag = 0x0;
        }
        break;
    default:
        step = 0x00;
        break;
    }
    return ret;
}


void BOOT_JumpToApplication(uint32_t Addr)
{
    pFunction jump;
    jump = (pFunction)(Addr);
    jump();
}

void bootloaderupdateApp(void)
{
    static uint8_t ret = 0x01;
    uint8_t ret1 = 0;
    if(ret == 0x01)
    {
        ret1 = BootLoaderReadFlashToComplateMsg(BootLoaderFlashBootArea1, BootLoaderFlashMsgArea3);
        if(ret1 == 0x02 || ret1 == 0x03)
        {
            ComplateMsgFlag = 0x01;
            ret = 0x02;     // 报错启用备份程序
        }
    }
    else if(ret == 0x02)
    {
        // 备份
        BootLoaderReadFlashToComplateMsg(BootLoaderFlashBootArea1 + BootLoaderFlashBootBackupArea, BootLoaderFlashMsgArea3 + BootLoaderFlashBootBackupArea);
    }

    if(ComplateMsgFlag == 0)
    {
        // 跳转APP程序
        DINT;
        DRTM;
        BOOT_JumpToApplication(((uint32_t)0X084000));
        while(1);
    }
}

void bootloaderTest(void)
{
    static uint8_t step = 0x00;
    uint16_t my_crc = 0x00;
    uint32_t msg_addr = 0x00;

    msg_addr = BootLoaderFlashMsgArea3;
    switch(step)
    {
    case 0:
        FLASH_SectorErase(BootLoaderFlashMsgArea3);
        bootloaderMsg.CRC = (uint16_t)crc16_ccitt((unsigned char *)&bootloaderMsg, sizeof(bootloaderMsg) - 1, 0x00);    // 除了CRC位都要进行校验
        step++;
        break;
    case 1:
        // 存入FLASH
        memcpy(flash_data[FLASH_TXBUF_NUM].data, &bootloaderMsg, sizeof(bootloaderMsg));
        FLASH_Page_Program(BootLoaderFlashMsgArea3, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderMsg));
        step++;
        break;
    case 2:
        // 从FLASH中提取信息
        FLASH_Read_Data(msg_addr, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderRDMsg));
        step++;
        break;
    case 3:
        // 复制到对应区域
        memcpy(&bootloaderRDMsg,flash_data[FLASH_TXBUF_NUM].data,  sizeof(bootloaderRDMsg));
        step++;
        break;
    case 4:
        // CRC校验
        my_crc = (uint16_t)crc16_ccitt((unsigned char *)&bootloaderRDMsg, sizeof(bootloaderRDMsg) - 1, 0x00);    // 除了CRC位都要进行校验
        if(bootloaderRDMsg.CRC == my_crc)
        {
            step++;
        }
        else
        {
            ComplateMsgFlag = 0x00; // 失败校验,和更新标志位
            step    = 0;
        }
        break;
    case 5:
        step = 0;
        break;
    default:
        break;
    }
}

// APP 正常运行时自动备份正常运行的程序到FLASH区
uint16_t APP_BackupToFLASH(void)
{
    uint32_t times              = 0x00;
    uint32_t times1             = 0x00;
    static uint32_t i           = 0x00;
    static uint8_t end          = 0x00;
    uint32_t addr               = 0x00;
    static uint32_t addr1       = 0x00;
    static uint8_t step         = 0x00;
    static uint16_t crc         = 0x00;
    static uint32_t curtimes    = 0x00;
    uint8_t ret                 = 0x00;
    uint32_t boot_addr          = BootLoaderFlashBootArea1;
    uint32_t msg_addr           = BootLoaderFlashMsgArea3;
    uint16_t my_crc             = 0x00;

    if (ComplateMsgFlag == 0x00) return 0x00;                    // 状态未激活

    times   = bootloaderRDMsg.ComplateLen / 256;                 // 得出需要读出多少个section
    times1  = bootloaderRDMsg.ComplateLen % 256;                 // 得出需要读出多少个section


    addr = boot_addr;
    switch(step)
    {
        case 0:
            // 从FLASH中提取信息
            FLASH_Read_Data(msg_addr, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderRDMsg));
            step++;
            break;
        case 1:
            // 复制到对应区域
            memcpy(&bootloaderRDMsg,flash_data[FLASH_RXBUF_NUM].data,  sizeof(bootloaderRDMsg));
            step++;
            break;
        case 2:
            // CRC校验
            my_crc = (uint16_t)crc16_ccitt((unsigned char *)&bootloaderRDMsg, sizeof(bootloaderRDMsg) - 1, 0x00);    // 除了CRC位都要进行校验
            if(bootloaderRDMsg.CRC == my_crc && bootloaderRDMsg.ComplateUpdateFlag == 0x02)
            {
                step++;
            }
            else
            {
                step    = 0;
                ComplateMsgFlag = 0x00; // 失败校验,和更新标志位
                if(bootloaderRDMsg.ComplateUpdateFlag == 0x00)
                {
                    ret     = 0x00;
                }
                else
                {
                    // CRC校验不通过
                    ret     = 0x03;
                }
            }
            break;
    case 3:
        // 从flash中读取数据
        if(i < times)
        {
            FLASH_Read_DataU8(addr + i * 256, &flash_dataU8[FLASH_TXBUF_NUM], 256);
            curtimes = 256;
            i++;
            if(times1 == 0) end = 1;
        }
        else
        {
            FLASH_Read_DataU8(addr + times * 256, &flash_dataU8[FLASH_TXBUF_NUM], times1); // 读出剩余字节
            curtimes = times1;
            end = 1;
            i = 0;
        }
        step++; // 步骤
        break;
    case 4:
        // CRC校验
        crc = (uint16_t)crc16_ccitt((unsigned char *)flash_dataU8[FLASH_RXBUF_NUM].data, curtimes, crc);

        if(end == 1)
        {
            end  = 0x00;
            step = 0x05;
        }
        else
        {
            step = 0x03;
        }
        break;
    case 5:
        if(bootloaderRDMsg.ComplateCRC != crc)
        {
            ComplateMsgFlag = 0x00; // 失败校验,和更新标志位
            step = 0x00;
            ret  = 0x03;
        }
        else
        {
            // 擦除 dsp flash和 外部flash
            if(addr == BootLoaderFlashBootArea1)
            {
                // 备用才启用
                FLASH_BlockErase(BootLoaderFlashBootArea1 + BootLoaderFlashBootBackupArea);
                FLASH_BlockErase(BootLoaderFlashBootArea2 + BootLoaderFlashBootBackupArea);
                FLASH_BlockErase(BootLoaderFlashBootArea3 + BootLoaderFlashBootBackupArea);
                FLASH_BlockErase(BootLoaderFlashMsgArea3  + BootLoaderFlashBootBackupArea);
            }
            step++;
        }
        crc = 0;
        break;
    case 6:
        times   = bootloaderRDMsg.ComplateLen / 256;                 // 得出需要读出多少个section
        times1  = bootloaderRDMsg.ComplateLen % 256;                 // 得出需要读出多少个section
        // 从flash中读取数据
        if(i < times)
        {
            addr1 = boot_addr + i * 256;
            FLASH_Read_DataU8(addr1, &flash_dataU8[FLASH_TXBUF_NUM], 256);
            curtimes = 256;
            i++;
            if(times1 == 0 && i==times) end = 1;
        }
        else
        {
            addr1 = boot_addr + times * 256;
            FLASH_Read_DataU8(addr1, &flash_dataU8[FLASH_TXBUF_NUM], times1); // 读出剩余字节

            curtimes = times1;
            end = 1;
            i = 0;
        }
        step++; // 步骤
        break;
    case 7:
        // CRC校验
        crc = (uint16_t)crc16_ccitt((unsigned char *)flash_data[FLASH_RXBUF_NUM].data, curtimes, crc);

        if(addr == BootLoaderFlashBootArea1) addr1 += BootLoaderFlashBootBackupArea;    // 备份时启用
        // 存入备份区域
        FLASH_Page_ProgramU8(addr1, flash_dataU8[FLASH_RXBUF_NUM].data, curtimes);

        if(end == 1)
        {
            end     = 0x00;
            step    = 0x08;
        }
        else
        {
            step = 0x06;
        }
        addr1   = 0x00;

        break;
    case 8:
        if(addr == BootLoaderFlashBootArea1)
        {
            // 存入FLASH 备份
            bootloaderRDMsg.ComplateUpdateFlag = 0x01;  // 更新标志置1
            bootloaderRDMsg.CRC = (uint16_t)crc16_ccitt((unsigned char *)&bootloaderRDMsg, sizeof(bootloaderRDMsg) - 1, 0x00);
            memcpy(flash_data[FLASH_TXBUF_NUM].data, &bootloaderRDMsg, sizeof(bootloaderRDMsg));
            FLASH_Page_Program(BootLoaderFlashMsgArea3 + BootLoaderFlashBootBackupArea, &flash_data[FLASH_TXBUF_NUM], sizeof(bootloaderRDMsg));
        }

        ComplateMsgFlag = 0x00; // 失败校验,和更新标志位
        crc             = 0;
        end             = 0;
        curtimes        = 0;
        step            = 0;
        break;
    default:
        step = 0;
        break;
    }
    return ret;
}







