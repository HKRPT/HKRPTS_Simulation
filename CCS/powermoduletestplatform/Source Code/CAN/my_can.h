#ifndef __MY_CAN_H
#define __MY_CAN_H

#include "device.h"
#include "driverlib.h"
#include "powerModule.h"

#define MSG_DATA_LENGTH    4
#define TX_MSG_OBJ_ID      1
#define RX_MSG_OBJ_ID      2


/*************************************************************/
#define USE_CANA  0
#define USE_CANB  1
#define CAN_Id_Standard   0//表示标准帧
#define CAN_Id_Extended   1//表示扩展帧
#define CAN_ID_STD      CAN_Id_Standard
#define CAN_ID_EXT      CAN_Id_Extended
#define TXCOUNT 100

typedef enum _CAN_Num
{
    Null = 0, CANA = 1, CANB = 2,
} CAN_Num;

typedef enum _UPDATE_State
{
    NON_CHANGE = 0, UPDATE = 1,
} UPDATE_State;

typedef union
{
    unsigned long int all;
     struct
    {
        unsigned short int SA :8;
        unsigned short int PS :8;
        unsigned short int PF :8;
        unsigned short int DP :1;
        unsigned short int R :1;
        unsigned short int Priority :3;
        unsigned short int resved :3;
    } bit;
} SAE_ID;

typedef struct _CAN_MSG_byte
{
    unsigned char data[8];
} CAN_MSG_byte;

typedef struct _CAN_MSG_BYTE
{
    unsigned char byte0;
    unsigned char byte1;
    unsigned char byte2;
    unsigned char byte3;
    unsigned char byte4;
    unsigned char byte5;
    unsigned char byte6;
    unsigned char byte7;
} CAN_MSG_BYTE;

// 数据寄存器
typedef union _CAN_MSG_DATA
{
    CAN_MSG_byte msg_byte;
    CAN_MSG_BYTE msg_Byte;
} CAN_MSG_DATA;

// 发送寄存器
typedef struct _CanTxMsg
{
    union
    {
        unsigned short int all;
        struct
        {
            unsigned short int StdId :11;
            unsigned short int resved :5;
        } bit;
    } StdId;
    union
    {
        unsigned long int all;
        struct
        {
            unsigned long int ExtId :29;
            unsigned long int resved :3;
        } bit;
    } ExtId;
    unsigned char SAE_J1939_Flag;
    SAE_ID SAE_J1939_ID;
    unsigned char IDE;
    unsigned char DLC;
    unsigned char MBox_num;
    unsigned short int Tx_timeout_cnt;
    CAN_Num CAN_num;
    UPDATE_State tx_update;
    CAN_MSG_DATA CAN_Tx_msg_data;

} CanTxMsg;


// 接收寄存器
typedef struct _CanRxMsg
{
    union
    {
        unsigned short int all;
        struct
        {
            unsigned short int StdId :11;
            unsigned short int resved :5;
        } bit;
    } StdId;
    union
    {
        unsigned long int all;
        struct
        {
            unsigned long int ExtId :29;
            unsigned long int resved :3;
        } bit;
    } ExtId;
    unsigned char SAE_J1939_Flag;
    SAE_ID SAE_J1939_ID;
    unsigned char IDE;
    unsigned char DLC;
    unsigned char MBox_num;
    unsigned short int Rx_timeout_cnt;
    CAN_MSG_DATA CAN_Rx_msg_data;
    CAN_Num CAN_num;
    UPDATE_State rx_update;
} CanRxMsg;

extern CanTxMsg can_tx_msg;
extern CanRxMsg can_rx_msg;


extern volatile struct CAN_REGS *CAN_R[];

/*************************************************************/
extern uint32_t CAN_TX_CMD_FrameFuncID ;                                        // 发送帧ID
extern uint8_t CAN_TX_CMD_FrameFuncTXBuff[8] ;                                // 发送帧缓冲区
extern uint16_t (*CAN_TX_CMD_FrameFunc)(uint16_t,uint32_t,uint8_t *) ;       // 随机发送指令

/*************************************************************/


// CAN初始化
void My_CAN_Init(uint16_t port);
// 邮箱5 任意ID发送，发送数据8字节 标准帧
uint16_t My_CAN_SendStdCMD(uint16_t port, uint32_t msgID, uint8_t *buff);
// 邮箱6 任意ID发送，发送数据8字节
uint16_t My_CAN_SendCMD(uint16_t port, uint32_t msgID, uint8_t *buff);
// 邮箱7 任意ID发送，发送数据8字节  CANOPEN发送标准帧
uint16_t My_CAN_SendCANopenSingleFrame(uint16_t port, uint32_t msgID, uint8_t len ,uint8_t *buff);
// 邮箱8 任意ID发送，发送数据8字节  CANOPEN发送远程帧
uint16_t My_CAN_SendCANopenRemoteFrame(uint16_t port, uint32_t msgID, uint8_t *buff);
// 邮箱31 接收ID
uint16_t My_CAN_ReqID(uint16_t port, uint32_t msgID);
// 邮箱32 任意ID发送，发送数据8字节
uint16_t My_CAN_SetID(uint16_t port, uint32_t msgID, uint16_t *buff);
// CAN 初始化
void CAN_Init(void);
// CAN 控制函数
void CAN_Control(void);


#endif

