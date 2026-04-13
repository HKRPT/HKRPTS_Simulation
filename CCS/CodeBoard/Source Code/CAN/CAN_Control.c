/*
 * CAN_Control.c
 *
 *  Created on: 2024年8月1日
 *      Author: BYRD-003
 */
#include "my_can.h"
#include "f28x_project.h"
#include "can_app.h"


uint32_t CAN_CMD_ID;
uint32_t CAN_CMD_DeviceID;
uint32_t CAN_CMD_ModuleAddr;

uint8_t CAN_RXBuff[8] = {0};
uint8_t CAN_TXBuff[8] = {0};

uint32_t CAN_TX_CMD_FrameFuncID = 0;                                        // 发送帧ID
uint8_t CAN_TX_CMD_FrameFuncTXBuff[8] = {0};                                // 发送帧缓冲区
uint8_t CAN_TX_CMD_FrameLen = 0;                                            // 发送帧长度
uint16_t (*CAN_TX_CMD_FrameFunc)(uint16_t,uint32_t,uint8_t *) = NULL;       // 随机发送指令

#define DEVICE_ID              1

// 发送扩展帧
uint16_t CAN_SetEXFrame(uint16_t port ,uint16_t ID,uint8_t *buff)
{
    CAN_TX_CMD_FrameFunc = NULL;    // 优先级最高操作

    CAN_TX_CMD_FrameFuncID = ID;

    CAN_TX_CMD_FrameFuncTXBuff[0] = buff[0];
    CAN_TX_CMD_FrameFuncTXBuff[1] = buff[1];
    CAN_TX_CMD_FrameFuncTXBuff[2] = buff[2];
    CAN_TX_CMD_FrameFuncTXBuff[3] = buff[3];
    CAN_TX_CMD_FrameFuncTXBuff[4] = buff[4];
    CAN_TX_CMD_FrameFuncTXBuff[5] = buff[5];
    CAN_TX_CMD_FrameFuncTXBuff[6] = buff[6];
    CAN_TX_CMD_FrameFuncTXBuff[7] = buff[7];

    CAN_TX_CMD_FrameFunc = My_CAN_SendCMD;   // 邮箱6 扩展帧

    return 0x00;
}

// 设置标准帧
uint16_t CAN_SetStdFrame(uint16_t port ,uint16_t ID,uint8_t *buff)
{
    CAN_TX_CMD_FrameFunc = NULL;    // 优先级最高操作

    CAN_TX_CMD_FrameFuncID = ID;

    CAN_TX_CMD_FrameFuncTXBuff[0] = buff[0];
    CAN_TX_CMD_FrameFuncTXBuff[1] = buff[1];
    CAN_TX_CMD_FrameFuncTXBuff[2] = buff[2];
    CAN_TX_CMD_FrameFuncTXBuff[3] = buff[3];
    CAN_TX_CMD_FrameFuncTXBuff[4] = buff[4];
    CAN_TX_CMD_FrameFuncTXBuff[5] = buff[5];
    CAN_TX_CMD_FrameFuncTXBuff[6] = buff[6];
    CAN_TX_CMD_FrameFuncTXBuff[7] = buff[7];

    CAN_TX_CMD_FrameFunc = My_CAN_SendStdCMD;   // 邮箱5 标准帧

    return 0x00;
}

void My_CAN_RX(uint16_t port)
{
    uint8_t n = 0;
    uint8_t SID=0xFF;
    uint8_t CMD=0xFF;
    uint8_t PCI=0xFF;


    // 判断帧类型
    if(1) // 标准帧
    {
        PCI = CAN_RXBuff[1];
        //根据消息获取SID和CMD
        if((PCI&0xF0)==0x00){
          SID = CAN_RXBuff[2];
          CMD = CAN_RXBuff[3];
        }else if((PCI&0xF0)==0x10){
          SID = CAN_RXBuff[3];
          CMD = CAN_RXBuff[4];
        }
        CAN_TXBuff[0] = GetNAD();//填充NAD

        switch(CMD)  // 判断是什么协议
        {
        case CMD_GET_FW_INFO:

            CAN_TXBuff[1] = 0x06;
            CAN_TXBuff[2] = SID+0x40;
            CAN_TXBuff[3] = FW_TYPE;
            CAN_TXBuff[4] = 0;//固件版本号 Major
            CAN_TXBuff[5] = 0;//固件版本号 Minor
            CAN_TXBuff[6] = 0;//固件版本号 Revision
            CAN_TXBuff[7] = 3;//固件版本号 Build

            CAN_SetStdFrame(0,0x3D,CAN_TXBuff);
            break;
        case CMD_ENTER_BOOT:
            if(FW_TYPE == FW_TYPE_APP){
                DINT;
                CAN_BOOT_JumpToApplication(0x080000);//跳转地址参考boot的cmd文件的BEGIN定义
                EINT;
            }
            break;
        case CMD_ERASE_APP:
            //APP中不能执行擦除APP的操作
            CAN_TXBuff[1] = 0x06;
            CAN_TXBuff[2] = SID+0x40;
            CAN_TXBuff[3] = CAN_BOOT_ERR_ERASE_IN_APP;
            CAN_TXBuff[4] = 0xFF;
            CAN_TXBuff[5] = 0xFF;
            CAN_TXBuff[6] = 0xFF;
            CAN_TXBuff[7] = 0xFF;

            CAN_SetStdFrame(0,0x3D,CAN_TXBuff);
            break;
        case CMD_SET_ADDR_OFFSET:
            CAN_TXBuff[1] = 0x06;
            CAN_TXBuff[2] = SID+0x40;
            CAN_TXBuff[3] = CAN_BOOT_ERR_WRITE_IN_APP;
            CAN_TXBuff[4] = 0xFF;
            CAN_TXBuff[5] = 0xFF;
            CAN_TXBuff[6] = 0xFF;
            CAN_TXBuff[7] = 0xFF;

            CAN_SetStdFrame(0,0x3D,CAN_TXBuff);
            break;
        case CMD_TRAN_DATA:
            break;
        case CMD_WRITE_DATA:
            CAN_TXBuff[1] = 0x06;
            CAN_TXBuff[2] = SID+0x40;
            CAN_TXBuff[3] = CAN_BOOT_ERR_WRITE_IN_APP;
            CAN_TXBuff[4] = 0xFF;
            CAN_TXBuff[5] = 0xFF;
            CAN_TXBuff[6] = 0xFF;
            CAN_TXBuff[7] = 0xFF;

            CAN_SetStdFrame(0,0x3D,CAN_TXBuff);
            break;
        case CMD_ENTER_APP:
            break;
        default:
            break;
        }
        for (n = 0; n < 8; n++) {CAN_RXBuff[n] = 0;CAN_TXBuff[n] = 0;}      // 清除缓冲区
    }
    else                                        // 扩展帧
    {
        CAN_CMD_ID = (CAN_R[port]->CAN_IF3ARB.bit.ID >> 8) & 0xFFFFF;       // 命令ID, 扩展帧
        CAN_CMD_DeviceID = CAN_R[port]->CAN_IF3ARB.bit.ID & 0xFF;           // 设备地址
        CAN_CMD_ModuleAddr = 0;                                             // 模块地址为0

        CAN_RXBuff[0] = CAN_R[port]->CAN_IF3DATA.bit.Data_0;
        CAN_RXBuff[1] = CAN_R[port]->CAN_IF3DATA.bit.Data_1;
        CAN_RXBuff[2] = CAN_R[port]->CAN_IF3DATA.bit.Data_2;
        CAN_RXBuff[3] = CAN_R[port]->CAN_IF3DATA.bit.Data_3;
        CAN_RXBuff[4] = CAN_R[port]->CAN_IF3DATB.bit.Data_4;
        CAN_RXBuff[5] = CAN_R[port]->CAN_IF3DATB.bit.Data_5;
        CAN_RXBuff[6] = CAN_R[port]->CAN_IF3DATB.bit.Data_6;
        CAN_RXBuff[7] = CAN_R[port]->CAN_IF3DATB.bit.Data_7;

        CAN_R[port]->CAN_IF3OBS.all |= 0x0000001F; // 告知接口3可以读下一个邮箱

        CAN_CMD_ID = 0;
    }

}

void CAN_Control(void)
{
//    My_CAN_RX(0); // 接收解析
    // 随机指令
    if(CAN_TX_CMD_FrameFunc != NULL)
    {
        if(CAN_TX_CMD_FrameFunc(0,CAN_TX_CMD_FrameFuncID,CAN_TX_CMD_FrameFuncTXBuff) == 0x00) // 发送成功
        {
            DINT;
            CAN_TX_CMD_FrameFunc = NULL; // 清除命令发送函数
            EINT;
        }
    }
}









