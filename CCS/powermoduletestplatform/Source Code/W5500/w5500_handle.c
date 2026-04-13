/*
 * w5500_handle.c
 *
 *  Created on: 2025年2月10日
 *      Author: BYRD-003
 */
#include "w5500.h"
#include "modbus.h"
#include "my_types.h"

extern w5500_ETH_msg_t w5500_ETH_msg[ETH_NUM];
void (*W5500_RX_FrameResolutionFunc[SOCKET_NUM])(modbusMasterData_t* )= {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
extern uint8_t     server_ip[SOCKET_NUM][4] ;
extern uint16_t    server_port[SOCKET_NUM];
extern uint16_t    local_port[SOCKET_NUM];

void W5500_SocketRxArrayInit(uint16_t port, uint16_t socket)
{
    w5500_ETH_msg[port].socketRx[socket].freeFlag           = 0x01;     // 置位空闲状态
    w5500_ETH_msg[port].socketRx[socket].len                = 0x00;     // 长度清零
    w5500_ETH_msg[port].socketRx[socket].socketOverFlag     = 0x00;     // socet接收完成标志
    w5500_ETH_msg[port].socketRx[socket].sockettimeOutCnt   = 0x00;     // 超时清零
    w5500_ETH_msg[port].socketRx[socket].data[0]            = 0xFF;     // 数组首字节清空
    w5500_ETH_msg[port].socketRx[socket].front              = 0x00;     // 清空首字节
}
void W5500_SocketTxArrayInit(uint16_t port, uint16_t socket)
{
    w5500_ETH_msg[port].socketTx[socket].freeFlag           = 0x01;     // 置位空闲状态
    w5500_ETH_msg[port].socketTx[socket].front              = 0x00;     // 清空首字节
    w5500_ETH_msg[port].socketTx[socket].len                = 0x00;     // 长度清零
    w5500_ETH_msg[port].socketTx[socket].socketOverFlag     = 0x01;     // socet发送完成标志
    w5500_ETH_msg[port].socketTx[socket].sockettimeOutCnt   = 0x00;     // 超时清零
    w5500_ETH_msg[port].socketTx[socket].data[0]            = 0xFF;     // 数组首字节清空
}

// modbus 特供函数
void W5500_MBSocketRxArrayInit(uint16_t port)
{
    W5500_SocketRxArrayInit(port / 8, port % 8);
}

// W5500 modbus 构建函数
void W5500_modbusConstruction(uint16_t port, uint8_t socket)
{
    socket_msg_t *dataRx  = &w5500_ETH_msg[port].socketRx[socket];
    socket_msg_t *dataTx  = &w5500_ETH_msg[port].socketTx[socket];

    modbusMasterData[4 + port * 8 + socket].type         = MD_W5500;                    // 消息类型
    modbusMasterData[4 + port * 8 + socket].port         = 4 + port * 8 + socket;       // 端口
    modbusMasterData[4 + port * 8 + socket].Rxlen        = &dataRx->len;                // 接收缓存区长度
    modbusMasterData[4 + port * 8 + socket].Rxbuff       = &dataRx->data[dataRx->front];// 接收缓存区
    modbusMasterData[4 + port * 8 + socket].Txlen        = &dataTx->len;                // 发送缓存区长度
    modbusMasterData[4 + port * 8 + socket].Txbuff       = dataTx->data;                // 发送缓存区
    modbusMasterData[4 + port * 8 + socket].RX_ArrayInit = W5500_MBSocketRxArrayInit;   // 数组清除函数
    modbusMasterData[4 + port * 8 + socket].exTxFlag     = &dataTx->socketOverFlag;     // 发送标志位
    modbusMasterData[4 + port * 8 + socket].exRxFlag     = &dataRx->socketOverFlag;     // 接收标志位

    modbusMasterData[4 + port * 8 + socket].RxOverFlag   = 0x01;                         // 接收标志位
}

// W5500 异步接收
void W5500_FrameReceive(uint16_t port, uint8_t socket, uint16_t type)
{
    extern uint16_t W5500_recv(uint16_t port, uint16_t socket, uint8_t *buf, uint16_t len);
    extern uint16_t W5500_getSnRxSize(uint16_t port, uint8_t socket);
    extern uint32_t    W5500_secondDlyCntLimit;
    // UDP接收
    extern uint16_t W5500_recvfrom(uint16_t port, uint8_t socket, uint8_t * buf, uint16_t len, uint8_t * ip_addr, uint16_t *ip_port);

    uint16_t len = 0;
    socket_msg_t *data  = &w5500_ETH_msg[port].socketRx[socket];

    len = W5500_getSnRxSize(port, socket);             // 读取W5500空闲接收缓存寄存器的值，表示接收缓存中已接受和保存的数据大小

    if(len > 0)
    {
        if(data->len >= 256)
        {
            W5500_SocketRxArrayInit(port, socket);      // 溢出，舍弃
            return  ;                                   // 没有数据需要解析
        }
        if(type == SOCK_ESTABLISHED)        // TCP接收函数
        {
            // 接收程序
            data->len = W5500_recv(port, socket, data->data, len);    // W5500接收来自客户端的数据，并发送到SPI
        }
        else if(type == SOCK_UDP)              // UDP函数
        {
            data->len = W5500_recvfrom(port, socket, data->data, len,server_ip[ETH_Array(port,socket)],&server_port[ETH_Array(port,socket)]); // W5500接收来自远程上位机的数据,并通过SPI发送给MCU
        }
        data->sockettimeOutCnt  = 0x00;                                 // 超时函数清零
        data->freeFlag          = 0x00;                                 // 空闲标志位
        data->socketOverFlag    = 0x00;                                 // 数据未处理完成
    }

    else
    {
        // 空闲时候
        data->freeFlag = 0x01;          // 空闲标志位

        if(data->len < 8)
        {
            W5500_SocketRxArrayInit(port, socket);      // 未知数据，舍弃
            return  ;                                   // 没有数据需要解析
        }

        if(data->socketOverFlag != 1)   // 数据尚未完成处理
        {
            if(data->sockettimeOutCnt >= W5500_secondDlyCntLimit)
            {
                W5500_SocketRxArrayInit(port, socket);      // 超时舍弃
                return  ;                                   // 没有数据需要解析
            }
        }
        else
        {
            W5500_SocketRxArrayInit(port, socket);      // 超时舍弃
            return; // 上一帧已完成
        }
        // 帧解析/提取
        if(data->data[data->front] == CID)                // 判断从机号是否相同
        {
            if(data->data[data->front + 1] == 0x03)           // 判断是否为modbus 功能码为0x03
            {
                data->sockettimeOutCnt  = 0x00;             // 超时函数清零
                // 提交到接口层
                data->socketOverFlag = 1;
                // 处理modbus报文
                W5500_modbusConstruction(port, socket);     // 串口modbus构建函数

                W5500_RX_FrameResolutionFunc[port * 8 + socket] = modbus03MasterFrameResolution; // 异步帧解析
            }
            else if(data->data[data->front + 1] == 0x05)           // 判断是否为modbus 功能码为0x05
            {
                data->sockettimeOutCnt  = 0x00;             // 超时函数清零
                // 提交到接口层
                data->socketOverFlag = 1;
                // 处理modbus报文
                W5500_modbusConstruction(port, socket);     // 串口modbus构建函数

                W5500_RX_FrameResolutionFunc[port * 8 + socket] = modbus05MasterFrameResolution; // 异步帧解析
            }
            else if(data->data[data->front + 1] == 0x10)           // 判断是否为modbus 功能码为0x10
            {
                data->sockettimeOutCnt  = 0x00;             // 超时函数清零
                // 提交到接口层
                data->socketOverFlag = 1;
                // 处理modbus报文
                W5500_modbusConstruction(port, socket);     // 串口modbus构建函数

                W5500_RX_FrameResolutionFunc[port * 8 + socket] = modbus10MasterFrameResolution; // 异步帧解析
            }
            else
            {
                if(data->len < 8 || 1) // 该方法丢包率太高了，放弃兼容
                {
                    W5500_SocketRxArrayInit(port, socket);      // 未知数据，舍弃
                    return  ;                                   // 没有数据需要解析
                }
                else
                {
                    // 没有对应的功能码，丢掉
                    data->front += 8;
                    data->len -= 8;
                }
            }
        }
        else if(data->data[data->front] == 0x55)                                        // 自定义协议
        {
            extern void DebugConfigFrameAnalytic(modbusMasterData_t* data) ;
            data->sockettimeOutCnt  = 0x00;                                             // 超时函数清零
            // 提交到接口层
            data->socketOverFlag = 1;
            // 处理modbus报文
            W5500_modbusConstruction(port, socket);                                     // 串口modbus构建函数

            W5500_RX_FrameResolutionFunc[port * 8 + socket] = DebugConfigFrameAnalytic; // 异步帧解析
        }
        else if(data->data[data->front] == 0x5A)                                        // 自定义协议
        {
            // bootLoader在线解析函数
            extern void BootLoaderFrameFunc(modbusMasterData_t* data);
            data->sockettimeOutCnt  = 0x00;                                             // 超时函数清零
            // 提交到接口层
            data->socketOverFlag = 1;
            // 处理modbus报文
            W5500_modbusConstruction(port, socket);                                     // 串口modbus构建函数

            W5500_RX_FrameResolutionFunc[port * 8 + socket] = BootLoaderFrameFunc;      // 异步帧解析
        }
        else
        {
            // 不是帧头，丢掉
//            data->front++;
//            data->len--;      // 该方法丢包率太高了
            W5500_SocketRxArrayInit(port, socket);      // 超时舍弃
            return; // 上一帧已完成
        }
        if(data->front >= data->len)
        {
            W5500_SocketRxArrayInit(port, socket);      //  溢出舍弃
            return  ;                                   // 没有数据需要解析
        }
    }
}

// W5500 发送帧函数
void W5500_FrameSend(uint16_t port, uint8_t socket, uint16_t type)
{
    /*  发送数据    */
    extern uint16_t W5500_send(uint16_t port, uint16_t socket,  const uint8_t * buf, uint16_t len);
    extern uint16_t W5500_sendto(uint16_t port, uint16_t socket, const uint8_t * buf, uint16_t len, uint8_t * ip_addr, uint16_t ip_port);
    socket_msg_t *data  = &w5500_ETH_msg[port].socketTx[socket];

    if(data->socketOverFlag == 0x01)       return ;     // 发送未触发

    if (data->len == 0)                                 // 无数据正在发送
    {
        W5500_SocketTxArrayInit(port, socket);          // 清空数组
    }
    else
    {
        if(type == SOCK_ESTABLISHED)        // TCP接收函数
        {
            data->len -= W5500_send(port, socket, data->data, data->len);// 发送数据
        }
        else if(type == SOCK_UDP)
        {
            data->len -= W5500_sendto(port, socket, data->data, data->len,server_ip[ETH_Array(port,socket)],server_port[ETH_Array(port,socket)]);          // 接受到数据后再回给远程上位机，完成数据回环
        }
        if(data->len >512) data->len = 0;
    }
}

// modbus 帧返回函数
void w5500_modbusMaster(void)
{
    extern modbusMasterData_t  modbusMasterData[];
    uint16_t i ,j ;
    for(j = 0; j < SOCKET_NUM; j++)
    {
        i = 4 + j;
        if(W5500_RX_FrameResolutionFunc[j] != NULL)
        {
            W5500_RX_FrameResolutionFunc[j](&modbusMasterData[i]);
            modbusMasterData[i].RxOverFlag  = 0x00;     // 清除接收完成标志位
            W5500_RX_FrameResolutionFunc[j] = NULL;
        }
    }
}

/*
 *
 *
 * */














