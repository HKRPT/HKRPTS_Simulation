/*
 * w5500.h
 *
 *  Created on: 2025年1月22日
 *      Author: Yang
 */

#ifndef SOURCE_CODE_W5500_W5500_H_
#define SOURCE_CODE_W5500_W5500_H_

#include "main.h"
#include "w5500_Type.h"
#include "modbus.h"

/*  构建发送和接收通用的结构体 */
typedef struct
{
    uint16_t data[Comm_BufLen];
    uint16_t front;             // 帧头
    uint16_t len;
    uint16_t socketOverFlag;    // socket 发送结束标志
    uint16_t sockettimeOutCnt;  // 超时计数, 单位ms
    uint16_t freeFlag;          // 空闲标志位，若此位为1可以触发接收/发送
}socket_msg_t;
typedef struct
{
    socket_msg_t  socketTx[8];  // 发送 socket
    socket_msg_t  socketRx[8];  // 接收 socket
}w5500_ETH_msg_t;

/*  定义socket 数量和网口数量    */
#define ETH_NUM         1
#define SOCKET_NUM      ETH_NUM * MAX_SOCKET_NUM
/*  数组寻址            */
#define ETH_Array(port,socket)     (port * 8 + socket)
/*  定义两个网口的关系   */
#define ETH1            0
#define ETH2            1
/*  网口接收和发送的关系  */
#define ETHx_TX_BUFNUM  0
#define ETHx_RX_BUFNUM  1
/*  最大socket        */
#define MAX_SOCKET_NUM  8
/* W5500 状态 */
#define NORMAL_STATE          0
#define NEW_APP_IN_BACK       1
#define CONFIGTOOL_FW_UP      2
/*  定义socket类型  */
#define TCP_Client            0
#define TCP_Server            1
#define Socket_UDP            2

extern void (*W5500_RX_FrameResolutionFunc[SOCKET_NUM])(modbusMasterData_t* );
// W5500 复位
void W5500_Reset(void);
// 片选信号切换函数
void W5500_CS(uint16_t port ,uint8_t val);
// W5500 SPI 写函数
void W5500_SPIWrite(uint16_t port , uint32_t addrbsb, uint8_t data);
// W5500 SPI 读函数
uint16_t W5500_SPIRead(uint16_t port , uint32_t addrbsb);
// W5500 网络配置
void W5500_NetWorkConfig(void);
/* W5500 测试函数   */
void W5500_Test(void);
/*  W5500控制  */
void W5500_Control(uint16_t port, uint8_t socket_num);

#endif /* SOURCE_CODE_W5500_W5500_H_ */
