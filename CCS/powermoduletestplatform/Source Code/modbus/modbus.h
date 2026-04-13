/*
 * modbus.h
 *
 *  Created on: 2025年2月8日
 *      Author: Yang
 */

#ifndef SOURCE_CODE_MODBUS_MODBUS_H_
#define SOURCE_CODE_MODBUS_MODBUS_H_

#include "main.h"


// modbus 入口类型
typedef enum
{
    MD_UART     = 0,
    MD_W5500,
    MD_CAN
}MD_interType_t;

typedef struct
{
    MD_interType_t  type;           // 传入的类型
    uint16_t        port;           // 端口
    uint16_t*       targetReg;      // 目标寄存器
    uint16_t        targetlen;      // 目标长度
    uint8_t         *Rxbuff;        // 接收缓存
    uint8_t         *Txbuff;        // 发送缓存
    uint16_t        *Rxlen;         // 接收长度
    uint16_t        *Txlen;         // 发送长度
    uint16_t        RxOverFlag;     // 接收完毕标志位
    uint16_t        TxStartFlag;    // 发送开始标志位
    uint16_t        *exTxFlag;      // 外部发送标志位
    uint16_t        *exRxFlag;      // 外部接收标志位
    void (*RX_ArrayInit)(uint16_t); // 接收初始化数据函数
}modbusMasterData_t;

/*  MODBUS     */
#define MODBUS_NUM      22

extern modbusMasterData_t  modbusMasterData[MODBUS_NUM];
// 03 主站功能码帧解析(查询传感器数据)
extern void modbus03MasterFrameResolution(modbusMasterData_t* data);
// 05 主站功能码帧解析(写单个线圈)
extern void modbus05MasterFrameResolution(modbusMasterData_t* data);
// 10 主站功能码帧解析(写多个寄存器)
extern void modbus10MasterFrameResolution(modbusMasterData_t* data);

// modbus 帧返回函数
extern void modbusMaster(void);
extern void w5500_modbusMaster(void);
#endif /* SOURCE_CODE_MODBUS_MODBUS_H_ */
