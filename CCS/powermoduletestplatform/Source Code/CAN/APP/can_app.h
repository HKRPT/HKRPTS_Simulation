#ifndef __MY_CANAPP_H
#define __MY_CANAPP_H

#include "device.h"
#include "driverlib.h"


#include "stdint.h"
#include "my_can.h"
#include "main.h"

//extern control_t    control;
#define ENCRYPT         0       //固件传输是否加密
#define APP_START_ADDR  ((uint32_t)0x310000)

#define DEVICE_ADDR     1       //设备地址
//固件类型值定义
#define FW_TYPE_BOOT     0x55
#define FW_TYPE_APP      0xAA
//定义当前固件类型为BOOT
#define FW_TYPE         FW_TYPE_APP
//定义数据收发的帧ID，必须跟上位机软件配置一致，否则无法正常工作。
//对于CAN总线，数据收发ID可以定义为一个ID，也可以定义为不同的ID
#define MSG_RECEIVE_ID      0x3C
#define MSG_SEND_ID         0x3D
//定义数据收发帧ID类型,0-标准帧，1-扩展帧
#define MSG_ID_TYPE         0
//定义节点广播地址
#define NAD_BROADCAST       0x7F
//BOOT命令定义
#define CMD_GET_FW_INFO             0x80
#define CMD_ENTER_BOOT              0xC1
#define CMD_ERASE_APP               0x42
#define CMD_SET_ADDR_OFFSET         0x03
#define CMD_TRAN_DATA               0xC4
#define CMD_WRITE_DATA              0x85
#define CMD_ENTER_APP               0x06
//BOOT错误定义
#define CAN_BOOT_ERR_SUCCESS        0       //没有错误
#define CAN_BOOT_ERR_ERASE          1   //固件擦除出错
#define CAN_BOOT_ERR_ERASE_IN_APP   2   //当前模式为APP，不能擦除固件
#define CAN_BOOT_ERR_WRITE_OUTRANGE 3   //当前地址超出了正常的地址范围
#define CAN_BOOT_ERR_WRITE_IN_APP   4   //当前模式不能写入固件数据
#define CAN_BOOT_ERR_WRITE          5   //数据写入程序存储器出错
#define CAN_BOOT_ERR_WRITE_OUT_ADDR 6   //数据长度超出了程序存储器范围
#define CAN_BOOT_ERR_TRAN_CRC       7   //数据传输CRC校验出错
#define CAN_BOOT_ERR_WRITE_CRC      8   //数据写入芯片CRC校验出错

#define GetNAD() DEVICE_ADDR

void __disable_irq(void);
void __enable_irq(void);
void __set_PRIMASK(unsigned char state);

unsigned short int CRCcalc16 (unsigned char *data,unsigned short int len);
void CAN_BOOT_JumpToApplication(uint32_t Addr);
void CAN_BOOT_ExecutiveCommand(CanRxMsg *pRxMessage);


#endif

