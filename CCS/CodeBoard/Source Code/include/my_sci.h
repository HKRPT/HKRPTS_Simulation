/*
 * my_sci.h
 *
 *  Created on: 2025年2月8日
 *      Author: Yang
 */

#ifndef SOURCE_CODE_INCLUDE_MY_SCI_H_
#define SOURCE_CODE_INCLUDE_MY_SCI_H_

#include    "main.h"

#define PORT_NUM  4

#define A 0
#define B 1
#define D 2
#define C 3

#define UART_BUFFER_LEN     Comm_BufLen

#define ERROR_UART_BUFFER_OVER                        0x01
#define ERROR_UART_ARRAY_CONFLICT                     0x02

//extern void (*UART_RX_FrameResolutionFunc[4])(modbusMasterData_t* );

typedef struct
{
    uint16_t front;           // 链头
    uint16_t end;             // 链尾
    uint16_t lenght;          // 链长
    uint8_t FirstByte;        // 第一个字节
    uint8_t buffer[UART_BUFFER_LEN];      // 链缓冲区
    uint16_t errCode;         // 1 : 溢出; 2 : 冲突; ;3 - 255 : 保留
}BUFFER_UNION, *P_BUFFER_UNION;    // 缓冲区参数

typedef struct
{
    BUFFER_UNION txArray;          // 发送队列参数
    BUFFER_UNION rxArray;          // 接收队列参数
    uint16_t uartTxOverFlag;         // UART 发送结束标志
    uint16_t uartRxOverFlag;         // UART 接收完成标志
    uint16_t uartRxtimeOutCnt;       // 接收超时计数, 单位ms
    uint16_t uartTxtimeOutCnt;       // 发送超时计数, 单位ms
}UART_PaketType, *pUART_PaketType;

extern UART_PaketType UART[];
extern void UART_ArrayInit(void);
extern void UART_RX_ArrayInit(uint16_t port);
extern void UART_TX_ArrayInit(uint16_t port);
extern void UART_RX_ISR(uint16_t port);
extern void UART_TX_ISR(uint16_t port);
extern void UART_CCC(void);
extern void SCI_Init(volatile struct SCI_REGS *rSCI, uint32_t BaudRate);
extern void SCI_A_Init(void);
extern void SCI_B_Init(void);
extern void SCI_C_Init(void);
extern void SCI_D_Init(void);



#endif /* SOURCE_CODE_INCLUDE_MY_SCI_H_ */
