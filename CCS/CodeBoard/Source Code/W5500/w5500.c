/*
 * w5500.c
 *
 *  Created on: 2025年1月22日
 *      Author: Yang
 */
#include "w5500.h"
#include "string.h"

typedef enum
{
    W5500_Normal    = 0,        // 正常状态
    W5500_WD_TimeOut   ,        // 发送超时状态
    W5500_RD_TimeOut   ,        // 接收超时状态
    W5500_CRCTxError   ,        // CRC发送错误
    W5500_CRCRxError   ,        // CRC接收错误
    W5500_DisConnect   ,        // 网线被拔出
    W5500_Error        ,        // 错误状态
}W5500_Status_t;

typedef struct
{
    uint8_t     mac[6];         // 硬件地址
    uint8_t     lip[4];         // 本地地址
    uint8_t     sub[4];         // 子网掩码
    uint8_t      gw[4];         // 网关
    uint8_t     dns[4];
}W5500_ConfigMsg_t;


w5500_ETH_msg_t w5500_ETH_msg[ETH_NUM]; // 消息队列
#pragma DATA_SECTION(w5500_ETH_msg, "w5500_ramgs");

W5500_ConfigMsg_t  W5500_ConfigMsg[2];                        // 两个网口硬件配置

uint8_t W5500_txSize[MAX_SOCKET_NUM] = {2,2,2,2,2,2,2,2};     // 8个socket每个socket发送缓存大小
uint8_t W5500_rxSize[MAX_SOCKET_NUM] = {2,2,2,2,2,2,2,2};     // 8个socket每个socket接收缓存大小

#if YHYComputer
// 配置远程服务器IP地址
uint8_t     server_ip[SOCKET_NUM][4]    = {{192,168,2,123},{192,168,2,123},{192,168,2,123},{192,168,2,123}, // ETH1 8个Socket服务器地址分配
                                           {192,168,2,123},{192,168,2,123},{192,168,2,123},{192,168,2,123}};
#else
// 配置远程服务器IP地址
uint8_t     server_ip[SOCKET_NUM][4]    = {{192,168,0,30},{192,168,0,30},{192,168,0,30},{192,168,0,30}, // ETH1 8个Socket服务器地址分配
                                           {192,168,0,30},{192,168,0,30},{192,168,0,30},{192,168,0,30}};
#endif
#if YHYComputer
uint16_t    server_port[SOCKET_NUM]     = {5002,5002,5002,5002,5002,5002,5002,5002};                        // ETH2 8个端口配置远程服务器端口
#else
// 配置远程服务器IP地址
uint16_t    server_port[SOCKET_NUM]     = {5000,5001,5001,5001,5000,5000,5000,5000};
#endif

uint16_t    local_port[SOCKET_NUM]      = {5000,5500,2346,6500,7000,7500,2345,8500};                        // ETH2 定义一个本地端口并初始化

uint16_t    connect_flag[8]             = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };

W5500_Status_t    W5500_Status[ETH_NUM] = { W5500_Normal};                  // ETH 状态
uint32_t    W5500_secondDlyCnt[ETH_NUM] = { 0 };                            // ETH 延迟ms 函数，放在中断内运行
uint32_t    W5500_secondClientDlyCnt[SOCKET_NUM] = { 0 , 0 };               // 客户端延迟函数 延迟ms 函数，放在中断内运行
uint32_t    W5500_secondDlyCntLimit     = 1000;                             // ETH 延迟ms 函数，超时报警值

typedef struct
{
    uint16_t data       :8;
    uint16_t addrbsb    :8;
}W5500_U8;

typedef union
{
    uint16_t Half_Word;
    W5500_U8 byte;
}W5500_UNION;

typedef struct
{
    uint16_t    addrbsb;
    W5500_U8    data;
    W5500_UNION extra_data[80];
}W5500_t;

// 定义W5500的接收和发送结构体
W5500_t w5500[2];
#pragma DATA_SECTION(w5500, "ramgs0");    // map the TX data to memory



extern volatile struct SPI_REGS     *SPI_REG[];
extern volatile struct CH_REGS      *DMAChannel[];

// 中断计时函数
void W5500_contorlTimeCnt(void)
{
    uint16_t i, j = 0;

    for(i = 0; i < ETH_NUM; i++)
    {
        for(j = 0; j < SOCKET_NUM ;j++)
        {
            W5500_secondClientDlyCnt[j + i * 2]++;              // 客户端延迟函数 延迟ms 函数，放在中断内运行
            w5500_ETH_msg[i].socketRx[j].sockettimeOutCnt++;
            w5500_ETH_msg[i].socketTx[j].sockettimeOutCnt++;
            if(W5500_secondClientDlyCnt[j + i * 2] >= W5500_secondDlyCntLimit) W5500_secondClientDlyCnt[j + i * 2] = W5500_secondDlyCntLimit;
            if(w5500_ETH_msg[i].socketRx[j].sockettimeOutCnt >= W5500_secondDlyCntLimit)
                w5500_ETH_msg[i].socketRx[j].sockettimeOutCnt = W5500_secondDlyCntLimit;
            if(w5500_ETH_msg[i].socketTx[j].sockettimeOutCnt >= W5500_secondDlyCntLimit)
                w5500_ETH_msg[i].socketTx[j].sockettimeOutCnt = W5500_secondDlyCntLimit;

        }
        W5500_secondDlyCnt[i]++;                // ETH 延迟ms 函数，放在中断内运行
        if(W5500_secondDlyCnt[i] >= W5500_secondDlyCntLimit)W5500_secondDlyCnt[i] = W5500_secondDlyCntLimit;
    }
}
// 片选信号切换函数
void W5500_CS(uint16_t port ,uint8_t val)
{
    if(port == ETH1)
    {
//        ETH1_CS(val);
    }
    else
    {
//        ETH2_CS(val);
    }
}

// W5500 SPI 写函数
void W5500_SPIWrite(uint16_t port , uint32_t addrbsb, uint8_t data)
{
    DMA_head_t head;

    W5500_secondDlyCnt[port] = 0;
    W5500_CS(port, 0);

    /* 向结构体装载数据 */
    w5500[ETHx_TX_BUFNUM].addrbsb        = ((addrbsb & 0xFFFF00) >> 8);
    w5500[ETHx_TX_BUFNUM].data.addrbsb   = (addrbsb & 0xF8) + 4;
    w5500[ETHx_TX_BUFNUM].data.data      = data;

    head.dec_addr       = (uint16_t *)&w5500[ETHx_TX_BUFNUM];       // 终地址
    head.src_addr       = (uint16_t *)&ETHx_SPI_Handle->SPITXBUF;   // 起始地址
    head.receiveBuf     = (uint16_t*)&w5500[ETHx_RX_BUFNUM];        // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 0;                                        // 包头长度
    head.len            = 2;                                        // 消息大小
    head.dmaTxChannel   = ETHx_DMA_TX_Handle;                       // DMA 发送通道
    head.dmaRxChannel   = ETHx_DMA_RX_Handle;                       // DMA 接收通道
    head.spiChannel     = ETHx_SPI_Handle;                          // SPI通道
    head.timeOutCnt     = &W5500_secondDlyCnt[port];                // 超时计数器
    head.timeOutErrCode = W5500_WD_TimeOut ;                        // 超时错误代码
    head.timeOutFlag    = (uint32_t *)&W5500_Status[port];          // 超时标志位
    head.timeOutLimit   = W5500_secondDlyCntLimit;                  // 超时计数限制
    myDMARecevieSendMsg(&head);

    W5500_CS(port, 1);
}

// W5500 SPI 读函数
uint16_t W5500_SPIRead(uint16_t port , uint32_t addrbsb)
{
    DMA_head_t head;

    uint16_t data = 0;
    W5500_secondDlyCnt[port] = 0;
    W5500_CS(port, 0);

    /* 向结构体装载数据 */
    w5500[ETHx_TX_BUFNUM].addrbsb        = ((addrbsb & 0xFFFF00) >> 8);
    w5500[ETHx_TX_BUFNUM].data.addrbsb   = (addrbsb & 0xF8) + 0;
    w5500[ETHx_TX_BUFNUM].data.data      = 0x00;

    head.dec_addr       = (uint16_t *)&w5500[ETHx_TX_BUFNUM];       // 终地址
    head.src_addr       = (uint16_t *)&ETHx_SPI_Handle->SPITXBUF;   // 起始地址
    head.receiveBuf     = (uint16_t*)&w5500[ETHx_RX_BUFNUM];        // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 0;                                        // 包头长度
    head.len            = 2;                                        // 消息大小
    head.dmaTxChannel   = ETHx_DMA_TX_Handle;                       // DMA 发送通道
    head.dmaRxChannel   = ETHx_DMA_RX_Handle;                       // DMA 接收通道
    head.spiChannel     = ETHx_SPI_Handle;                          // SPI通道
    head.timeOutCnt     = &W5500_secondDlyCnt[port];                // 超时计数器
    head.timeOutErrCode = W5500_WD_TimeOut ;                        // 超时错误代码
    head.timeOutFlag    = (uint32_t *)&W5500_Status[port];          // 超时标志位
    head.timeOutLimit   = W5500_secondDlyCntLimit;                  // 超时计数限制
    myDMARecevieSendMsg(&head);

    W5500_CS(port, 1);

    data = w5500[ETHx_RX_BUFNUM].data.data;
    return data;
}
// W5500 SPI 写数组函数
void W5500_SPIWriteBuf(uint16_t port , uint32_t addrbsb, uint8_t *buf, uint16_t len)
{
    DMA_head_t  head;
    uint32_t    i = 0;
    W5500_secondDlyCnt[port] = 0;

    /*  组包  */
    /* 向结构体装载数据 */
    w5500[0].extra_data[i++].Half_Word  = ((addrbsb & 0xFF0000) >> 8);
    w5500[0].extra_data[i++].Half_Word  = ((addrbsb & 0x00FF00) >> 0);
    w5500[0].extra_data[i++].Half_Word  = ((uint32_t)((addrbsb & 0x0000F8) + 4) << 8);

    for(i = 0; i < len; i++)
    {
        w5500[0].extra_data[i+3].Half_Word  = (buf[i] & 0xFF) << 8;
    }

    head.dec_addr       = (uint16_t *)&w5500[ETHx_TX_BUFNUM].extra_data;       // 终地址
    head.src_addr       = (uint16_t *)&ETHx_SPI_Handle->SPITXBUF;   // 起始地址
    head.receiveBuf     = (uint16_t*)&w5500[ETHx_RX_BUFNUM];        // 接收数组
    head.isOneByte      = 1;                                        // 是否为单字节
    head.head_len       = 0;                                        // 包头长度
    head.len            = len+3;                                    // 消息大小
    head.dmaTxChannel   = ETHx_DMA_TX_Handle;                       // DMA 发送通道
    head.dmaRxChannel   = ETHx_DMA_RX_Handle;                       // DMA 接收通道
    head.spiChannel     = ETHx_SPI_Handle;                          // SPI通道
    head.timeOutCnt     = &W5500_secondDlyCnt[port];                // 超时计数器
    head.timeOutErrCode = W5500_WD_TimeOut ;                        // 超时错误代码
    head.timeOutFlag    = (uint32_t *)&W5500_Status[port];          // 超时标志位
    head.timeOutLimit   = W5500_secondDlyCntLimit;                  // 超时计数限制
    myDMARecevieSendMsg(&head);

}

// W5500 SPI 读数组函数
uint16_t W5500_SPIReadBuf(uint16_t port , uint32_t addrbsb, uint8_t *buf, uint16_t len)
{
    DMA_head_t head;
    uint16_t data = 0;
    uint8_t     partity;                // 判断奇数还是偶数
    uint16_t length = 0;
    uint16_t i,j    = 0;
    W5500_CS(port, 0);

    W5500_secondDlyCnt[port] = 0;
    partity = len % 2;                  // 0为偶数,1则为奇数
//    if(partity == 1) len = len - 1;   // 若为奇数时则减1位
    length = len / 2;

    /* 向结构体装载数据 */
    w5500[ETHx_TX_BUFNUM].addrbsb        = ((addrbsb & 0xFFFF00) >> 8);
    w5500[ETHx_TX_BUFNUM].data.addrbsb   = (addrbsb & 0xF8) + 0;
    w5500[ETHx_TX_BUFNUM].data.data      = 0x00;

    head.dec_addr       = (uint16_t *)&w5500[ETHx_TX_BUFNUM];       // 终地址
    head.src_addr       = (uint16_t *)&ETHx_SPI_Handle->SPITXBUF;   // 起始地址
    head.receiveBuf     = (uint16_t*)&w5500[ETHx_RX_BUFNUM];        // 接收数组
    head.isOneByte      = 0;                                        // 是否为单字节
    head.head_len       = 2;                                        // 包头长度
    head.len            = length;                                   // 消息大小
    head.dmaTxChannel   = ETHx_DMA_TX_Handle;                       // DMA 发送通道
    head.dmaRxChannel   = ETHx_DMA_RX_Handle;                       // DMA 接收通道
    head.spiChannel     = ETHx_SPI_Handle;                          // SPI通道
    head.timeOutCnt     = &W5500_secondDlyCnt[port];                // 超时计数器
    head.timeOutErrCode = W5500_WD_TimeOut ;                        // 超时错误代码
    head.timeOutFlag    = (uint32_t *)&W5500_Status[port];          // 超时标志位
    head.timeOutLimit   = W5500_secondDlyCntLimit;                  // 超时计数限制
    myDMARecevieSendMsg(&head);

    W5500_CS(port, 1);

    data = w5500[ETHx_RX_BUFNUM].data.data;
    buf[0] = data;
    j = 1;
    for(i=0; i < length; i++)
    {
        buf[j]      = w5500[ETHx_RX_BUFNUM].extra_data[i].byte.addrbsb;
        buf[j + 1]  = w5500[ETHx_RX_BUFNUM].extra_data[i].byte.data;
        j+=2;
    }
    if(partity == 0)        // 若长度为偶数，则再补一个
    {
        buf[j]  = w5500[ETHx_RX_BUFNUM].extra_data[length].byte.addrbsb;
    }
    return len;
}

// W5500 复位
void W5500_Reset(void)
{
    ETHx_RST(0);
    DELAY_US(20 * 1000);
//    ETHx_RST(1);
//    DELAY_US(1600 * 1000);
}
/* PHY配置寄存器 */
void W5500_setPHYCFGR(uint16_t port, uint8_t retry)
{
    W5500_SPIWrite(port, PHYCFGR, retry);
}
// PHY 配置寄存器
uint16_t W5500_getPHYCFGR(uint16_t port)
{
    return W5500_SPIRead(port, PHYCFGR);
}

/* 设置网关 */
void W5500_getGWIP(uint16_t port,uint8_t * addr)
{
    W5500_SPIWriteBuf(port, GAR0, addr, 4);
}

/* 设置子网掩码   */
void W5500_setSUBR(uint16_t port,uint8_t * addr)
{
    W5500_SPIWriteBuf(port, SUBR0, addr, 4);
}

/* 设置MAC地址  */
void W5500_setSHAR(uint16_t port,uint8_t * addr)
{
    W5500_SPIWriteBuf(port, SHAR0, addr, 6);
}

/*  设置源地址   */
void W5500_setSIPR(uint16_t port, uint8_t * addr)
{
    W5500_SPIWriteBuf(port, SIPR0, addr, 4);
}

/*  设置超时时间  */
void W5500_setRTR(uint16_t port, uint16_t timeout)
{
    W5500_SPIWrite(port, RTR0,(uint8_t)((timeout & 0xff00) >> 8));
    W5500_SPIWrite(port, RTR1,(uint8_t)(timeout & 0x00ff));
}

/*  设置重传次数   */
void W5500_setRCR(uint16_t port, uint8_t retry)
{
    W5500_SPIWrite(port, WIZ_RCR, retry);
}

static uint16_t W5500_SSIZE[MAX_SOCKET_NUM];      /*  最大发送数组大小，每个通道   */
static uint16_t W5500_RSIZE[MAX_SOCKET_NUM];      /*  最大接收数组大小，每个通道   */

/*  设置各 socket 接收和发送缓冲区大小    */
void W5500_sysInit(uint16_t port, uint8_t * tx_size, uint8_t * rx_size)
{
    uint16_t i;
    uint16_t ssum, rsum;

    ssum = 0;
    rsum = 0;

    for(i = 0; i < MAX_SOCKET_NUM; i++)
    {
        W5500_SPIWrite(port, (Sn_TXMEM_SIZE(i)), tx_size[i]);
        W5500_SPIWrite(port, (Sn_RXMEM_SIZE(i)), rx_size[i]);

        W5500_SSIZE[i] = 0;
        W5500_RSIZE[i] = 0;

        if(ssum <= 16384)                                     // 设置Socket发送缓存区大小
        {
            switch( tx_size[i] )
             {
                 case 1:
                     W5500_SSIZE[i] = (int16)(1024);          // i=1，tx_size=1KB
                 break;
                 case 2:
                     W5500_SSIZE[i] = (int16)(2048);          // i=2，tx_size=2KB
                 break;
                 case 4:
                     W5500_SSIZE[i] = (int16)(4096);          // i=4，tx_size=4KB
                 break;
                 case 8:
                     W5500_SSIZE[i] = (int16)(8192);          // i=8，tx_size=8KB
                 break;
                 case 16:
                     W5500_SSIZE[i] = (int16)(16384);         // i=16，tx_size=16KB
                 break;
                 default :
                     W5500_SSIZE[i] = (int16)(2048);          // 默认i=2，tx_size=2KB
                 break;
             }
        }
        if (rsum <= 16384)                              // 设置Socket接收缓存空间的大小
        {
            switch( rx_size[i] )
            {
                case 1:
                    W5500_RSIZE[i] = (int16)(1024);           // i=1，rx_size=1KB
                break;
                case 2:
                    W5500_RSIZE[i] = (int16)(2048);           // i=2，rx_size=2KB
                break;
                case 4:
                    W5500_RSIZE[i] = (int16)(4096);           // i=4，rx_size=4KB
                break;
                case 8:
                    W5500_RSIZE[i] = (int16)(8192);           // i=8，rx_size=8KB
                break;
                case 16:
                    W5500_RSIZE[i] = (int16)(16384);          // i=16，rx_size=16KB
                break;
                default :
                    W5500_RSIZE[i] = (int16)(2048);           // 默认i=2，rx_size=2K
                break;
            }
        }
        ssum += W5500_SSIZE[i];
        rsum += W5500_RSIZE[i];
    }
}
uint16_t W5500_getTxMAX(uint8_t socket)
{
   return W5500_SSIZE[socket];
}
// UDP 发送
uint16_t W5500_sendto(uint16_t port, uint16_t socket, const uint8_t * buf, uint16_t len, uint8_t * ip_addr, uint16_t ip_port)
{
    uint16_t ret=0;
    void W5500_send_data_processing(uint16_t port, uint16_t socket, uint8_t *data, uint16_t len);
    if (len > W5500_getTxMAX(socket)) ret = W5500_getTxMAX(socket); // 检查大小不超过最大值
    else ret = len;

    if( ((ip_addr[0] == 0x00) && (ip_addr[1] == 0x00) && (ip_addr[2] == 0x00) && (ip_addr[3] == 0x00)) || ((ip_port == 0x00)) )//||(ret == 0) )
    {
      /* added return value */
      ret = 0;
    }
    else
    {
        W5500_SPIWrite(port, Sn_DIPR0(socket), ip_addr[0]);
        W5500_SPIWrite(port, Sn_DIPR1(socket), ip_addr[1]);
        W5500_SPIWrite(port, Sn_DIPR2(socket), ip_addr[2]);
        W5500_SPIWrite(port, Sn_DIPR3(socket), ip_addr[3]);
        W5500_SPIWrite(port, Sn_DPORT0(socket),(uint8_t)((ip_port & 0xff00) >> 8));
        W5500_SPIWrite(port, Sn_DPORT1(socket),(uint8_t)(ip_port & 0x00ff));
        // copy data
        W5500_send_data_processing(port, socket, (uint8_t *)buf, ret);

        W5500_SPIWrite(port,  Sn_CR(socket) ,Sn_CR_SEND);
        /* wait to process the command... */
        W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

        while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
        {
            if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
            {
                W5500_Status[port] = W5500_RD_TimeOut;
                break;
            }
        }
        /* ------- */
        W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

        while( (W5500_SPIRead(port, Sn_IR(socket) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
        {
            if (W5500_SPIRead(port, Sn_IR(socket) ) & Sn_IR_TIMEOUT)                  // 超时
            {
                /* clear interrupt */
                W5500_SPIWrite(port, Sn_IR(socket) , (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
                return 0;
            }
            if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
            {
                W5500_Status[port] = W5500_RD_TimeOut;
                break;
            }
        }
            W5500_SPIWrite(port, Sn_IR(socket) , Sn_IR_SEND_OK);                       // 发送完成
        }
        return ret;
}
// UDP接收
uint16_t W5500_recvfrom(uint16_t port, uint8_t socket, uint8_t * buf, uint16_t len, uint8_t * ip_addr, uint16_t *ip_port)
{
    uint8_t head[8];
    uint16_t data_len=0;
    uint16_t ptr=0;
    uint32_t addrbsb =0;
    if ( len > 0 )
    {
      ptr     = W5500_SPIRead(port, Sn_RX_RD0(socket) );
      ptr     = ((ptr & 0x00ff) << 8) + W5500_SPIRead(port, Sn_RX_RD1(socket));
      addrbsb = ((uint32_t)ptr<<8) +  (socket<<5) + 0x18;

      switch (W5500_SPIRead(port, Sn_MR(socket) ) & 0x07)
      {
      case Sn_MR_UDP :
          W5500_SPIReadBuf(port ,addrbsb, head, 0x08);
          ptr += 8;
          // read peer's IP address, port number.
          ip_addr[0]  = head[0];
          ip_addr[1]  = head[1];
          ip_addr[2]  = head[2];
          ip_addr[3]  = head[3];
          *ip_port    = head[4];
          *ip_port    = (*ip_port << 8) + head[5];
          data_len = head[6];
          data_len = (data_len << 8) + head[7];

          addrbsb = ((uint32_t)ptr<<8) +  (socket<<5) + 0x18;
          W5500_SPIReadBuf(port ,addrbsb, buf, data_len);
          ptr += data_len;

          W5500_SPIWrite(port, Sn_RX_RD0(socket), (uint8_t)((ptr & 0xff00) >> 8));
          W5500_SPIWrite(port, Sn_RX_RD1(socket), (uint8_t)(ptr & 0x00ff));
        break;

      case Sn_MR_IPRAW :
          W5500_SPIReadBuf(port ,addrbsb, head, 0x06);
          ptr += 6;
          ip_addr[0]  = head[0];
          ip_addr[1]  = head[1];
          ip_addr[2]  = head[2];
          ip_addr[3]  = head[3];
          data_len = head[4];
          data_len = (data_len << 8) + head[5];

          addrbsb  = ((uint32_t)ptr<<8) +  (socket<<5) + 0x18;
          W5500_SPIReadBuf(port,addrbsb, buf, data_len);
          ptr += data_len;

          W5500_SPIWrite(port, Sn_RX_RD0(socket), (uint8_t)((ptr & 0xff00) >> 8));
          W5500_SPIWrite(port, Sn_RX_RD1(socket), (uint8_t)(ptr & 0x00ff));
        break;

      case Sn_MR_MACRAW :                               // MAC LAYER RAW SOCK
          W5500_SPIReadBuf(port ,addrbsb, head, 0x02);
          ptr+=2;
          data_len = head[0];
          data_len = (data_len<<8) + head[1] - 2;
          if(data_len > 1514)
          {
            // 超出大小，弃之
              break;
          }

          addrbsb  = ((uint32_t)ptr<<8) +  (socket<<5) + 0x18;
          W5500_SPIReadBuf(port ,addrbsb, buf, data_len);
          ptr += data_len;

          W5500_SPIWrite(port, Sn_RX_RD0(socket), (uint8_t)((ptr & 0xff00) >> 8));
          W5500_SPIWrite(port, Sn_RX_RD1(socket), (uint8_t)(ptr & 0x00ff));
        break;

      default :
            break;
      }
      W5500_SPIWrite(port, Sn_CR(socket) ,Sn_CR_RECV);

      /* wait to process the command... */
      W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

      while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
      {
          if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
          {
              W5500_Status[port] = W5500_RD_TimeOut;
              break;
          }
      }
      /* ------- */
    }
    return data_len;
}

// 获取公共寄存器中断寄存器值
uint16_t W5500_getIR(uint16_t port)
{
    return W5500_SPIRead(port, IR);
}
// 获取Socket寄存器状态寄存器值
uint16_t W5500_getSnSR(uint16_t port, uint8_t socket)
{
    return W5500_SPIRead(port, Sn_SR(socket));
}
// 获取Socket寄存器中断寄存器值
uint16_t W5500_getSnIR(uint16_t port, uint8_t socket)
{
    return W5500_SPIRead(port, Sn_IR(socket));
}
// 设置Socket寄存器中断寄存器值
void W5500_setSnIR(uint16_t port, uint8_t socket, uint16_t val)
{
    W5500_SPIWrite(port, Sn_IR(socket), val);
}
/* W5500心跳检测程序，设置Socket在线时间寄存器Sn_KPALVTR，单位为5s  */
void W5500_setkeepalive(uint16_t port, uint8_t socket)
{
    W5500_SPIWrite(port, Sn_KPALVTR(socket),0x02);
}
// 关闭Socket 通道
void W5500_socketClose(uint16_t port, uint8_t socket)
{
    W5500_SPIWrite(port, Sn_CR(socket), Sn_CR_CLOSE);       // 关闭 socket

    W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

    while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
    {
        if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
        {
            W5500_Status[port] = W5500_RD_TimeOut;
            break;
        }
    }

    W5500_SPIWrite(port, Sn_IR(socket), 0xFF);              // 清除标志位
}

/*  获取socket 发送数据空闲大小   */
uint16_t W5500_getSn_TX_FSR(uint16_t port, uint16_t socket)
{
    uint16_t val=0,val1=0;
    do
    {
        val  = 0;
        val1 = W5500_SPIRead(port, Sn_TX_FSR0(socket));                 // 读取Sn_TX_FSR低8位
        val1 = (val1 << 8) +  W5500_SPIRead(port, Sn_TX_FSR1(socket));  // 读取Sn_TX_FSR高8位
        if(val1 != 0)
        {
          val = W5500_SPIRead(port, Sn_TX_FSR0(socket));                // 读取Sn_TX_FSR低8位
          val = (val << 8) +  W5500_SPIRead(port, Sn_TX_FSR1(socket));  // 读取Sn_TX_FSR高8位
        }
    } while (val != val1);
    return val;
}
// 获取Socket RX 接收数组大小
uint16_t W5500_getSnRxSize(uint16_t port, uint8_t socket)   // 获取空闲接收缓存寄存器的值
{
    uint16_t val, val1 = 0;
    do
    {
        val  = 0;
        val1 = W5500_SPIRead(port, Sn_RX_RSR0(socket));                 // 读取Sn_RX_RSR低8位
        val1 = (val1 << 8) +  W5500_SPIRead(port, Sn_RX_RSR1(socket));  // 读取Sn_RX_RSR高8位
        if(val1 != 0)
        {
            val = W5500_SPIRead(port, Sn_RX_RSR0(socket));                 // 读取Sn_RX_RSR低8位
            val = (val << 8) +  W5500_SPIRead(port, Sn_RX_RSR1(socket));  // 读取Sn_RX_RSR高8位
        }
    }while(val != val1);                                    // 判断val与val1是否相等
    // 目的是判断网口接收停止了没有
    return val;
}

// 建立连接
uint8_t W5500_connect(uint16_t port, uint8_t socket, uint8_t *ip_addr, uint8_t ip_port)
{
    uint8_t ret;
    if (((ip_addr[0] == 0xFF) && (ip_addr[1] == 0xFF) && (ip_addr[2] == 0xFF) && (ip_addr[3] == 0xFF)) ||
        ((ip_addr[0] == 0x00) && (ip_addr[1] == 0x00) && (ip_addr[2] == 0x00) && (ip_addr[3] == 0x00)) ||
        (ip_port == 0x00))
    {
        // 规定IP不能为0.0.0.0或255.255.255.255或 端口为0
        ret = 0;
    }
    else
    {
        ret = 1;

        W5500_SPIWrite(port, Sn_DIPR0(socket), ip_addr[0]);     // 设置服务器地址
        W5500_SPIWrite(port, Sn_DIPR1(socket), ip_addr[1]);
        W5500_SPIWrite(port, Sn_DIPR2(socket), ip_addr[2]);
        W5500_SPIWrite(port, Sn_DIPR3(socket), ip_addr[3]);
                                                                // 设置端口号
        W5500_SPIWrite(port, Sn_DPORT0(socket), (uint8_t)(ip_port & 0xFF00) >> 8);
        W5500_SPIWrite(port, Sn_DPORT1(socket), (uint8_t)(ip_port & 0x00FF) >> 0);

        W5500_SPIWrite(port, Sn_CR(socket), Sn_CR_CONNECT);     // 发送连接请求

        W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

        while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
        {
            if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
            {
                W5500_Status[port] = W5500_RD_TimeOut;
                break;
            }
        }
    }
    return ret;
}
// 接收数据处理
void W5500_recvDataProcessing(uint16_t port, uint16_t socket, uint8_t *data, uint16_t len)
{
    uint16_t ptr = 0;
    uint32_t addrbsb = 0;

    // MCU读取Sn_RX_RD接收写指针寄存器的值，并赋给ptr
    // Sn_RX_RD保存接收缓存中数据的首地址，若有数据接收，则接收完后该寄存器值要更新
    ptr = W5500_SPIRead(port,  Sn_RX_RD0(socket) );
    ptr = ((ptr & 0x00ff) << 8) + W5500_SPIRead(port, Sn_RX_RD1(socket));

    addrbsb = ((uint32_t)ptr<<8) + (socket<<5) + 0x18;  // 获取接收到的数据的绝对地址
    W5500_SPIReadBuf(port, addrbsb, data, len);         // 通过绝对地址，将接收到的数据发给MCU

    // 更新Sn_RX_RD寄存器的值
    ptr += len;
    W5500_SPIWrite(port, Sn_RX_RD0(socket), (uint8_t)((ptr & 0xff00) >> 8));
    W5500_SPIWrite(port, Sn_RX_RD1(socket), (uint8_t)( ptr & 0x00ff));
}

// 发送数据处理
void W5500_send_data_processing(uint16_t port, uint16_t socket, uint8_t *data, uint16_t len)
{
    uint16_t ptr =0;
    uint32_t addrbsb =0;

    // MCU读取Sn_TX_WR接收写指针寄存器的值，并赋给ptr
    // Sn_TX_WR保存发送缓存中数据的首地址，若有数据发送，则发送完后该寄存器值要更新
    ptr = W5500_SPIRead(port,  Sn_TX_WR0(socket) );
    ptr = ((ptr & 0x00ff) << 8) + W5500_SPIRead(port, Sn_TX_WR1(socket));

    addrbsb = ((uint32_t)ptr<<8) + (socket<<5) + 0x10;  // 获取发送到的数据的绝对地址
    W5500_SPIWriteBuf(port, addrbsb, data, len);        // 通过绝对地址，将接收到的数据发给W5500

    // 更新Sn_TX_WR寄存器的值
    ptr += len;
    W5500_SPIWrite(port, Sn_TX_WR0(socket), (uint8_t)((ptr & 0xff00) >> 8));
    W5500_SPIWrite(port, Sn_TX_WR1(socket), (uint8_t)( ptr & 0x00ff));
}

// 设置默认值
void W5500_setDefault(uint16_t port)
{
#if YHYComputer
    uint8_t mac[6] = {0x00,0x08,0xDC,0x11,0x11,0x11 + port};
    uint8_t lip[4] = {192,168,2,210 + port};
    uint8_t sub[4] = {255,255,255,0};
    uint8_t  gw[4] = {192,168,2,1};
    uint8_t dns[4] = {8,8,8,8};
#else
    uint8_t mac[6] = {0x00,0x08,0xDC,0x11,0x11,0x11 + port};
    uint8_t lip[4] = {192,168,0,210 + port};
    uint8_t sub[4] = {255,255,255,0};
    uint8_t  gw[4] = {192,168,0,1};
    uint8_t dns[4] = {8,8,8,8};
#endif
    memcpy(W5500_ConfigMsg[port].lip, lip, 4);
    memcpy(W5500_ConfigMsg[port].sub, sub, 4);
    memcpy(W5500_ConfigMsg[port].gw , gw , 4);
    memcpy(W5500_ConfigMsg[port].dns, dns, 4);
    memcpy(W5500_ConfigMsg[port].mac, mac, 6);

}

/*  设置网络   */
void W5500_setNetWork(uint16_t port)
{
    W5500_setSHAR(port, W5500_ConfigMsg[port].mac);
    W5500_setSUBR(port, W5500_ConfigMsg[port].sub);
    W5500_getGWIP(port, W5500_ConfigMsg[port].gw);
    W5500_setSIPR(port, W5500_ConfigMsg[port].lip);

    /*  设置各 socket 接收和发送缓冲区大小    */
    W5500_sysInit(port, W5500_txSize, W5500_rxSize);

    /*  设置超时时间  */
    W5500_setRTR(port, 4000);
    /*  设置最大重新传输次数  */
    W5500_setRCR(port, 3);

    W5500_SPIReadBuf(port, SIPR0, W5500_ConfigMsg[port].lip, 4);
    W5500_SPIReadBuf(port, SUBR0, W5500_ConfigMsg[port].sub, 4);
    W5500_SPIReadBuf(port, GAR0 , W5500_ConfigMsg[port].gw, 4);

    // 设置PHY寄存器
    W5500_setPHYCFGR(port,  0x78);
    W5500_setPHYCFGR(port,  0xF8);
}

/*  接收数据    */
uint16_t W5500_recv(uint16_t port, uint16_t socket, uint8_t *buf, uint16_t len)
{
    uint16_t ret = 0;
    if(len > 0)
    {
        W5500_recvDataProcessing(port, socket, buf, len);   // 数据接收进程，通过Socket的buf接收长度位len的数据写入指针对应的DSP缓存地址
        W5500_SPIWrite(port, Sn_CR(socket), Sn_CR_RECV);    // DSP配置Sn_CR为RECV

        W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

        while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
        {
            if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
            {
                W5500_Status[port] = W5500_RD_TimeOut;
                break;
            }
        }

        ret = len;
    }
    return ret;     // 返回ret的值。有返回值说明W5500有数据接收，并不断重读接收这进程
}

// 关闭连接
void W5500_close(uint16_t port, uint8_t socket)
{

    W5500_SPIWrite(port, Sn_CR(socket) ,Sn_CR_CLOSE);

   /* wait to process the command... */
    W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

    while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
    {
        if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
        {
            W5500_Status[port] = W5500_RD_TimeOut;
            break;
        }
    }
   /* ------- */
        /* all clear */
   W5500_SPIWrite(port, Sn_IR(socket) , 0xFF);
}
/*  发送数据    */
uint16_t W5500_send(uint16_t port, uint16_t socket,  const uint8_t * buf, uint16_t len)
{
    uint8_t status=0;
    uint16_t ret=0;
    uint16_t freesize=0;

    if (len > W5500_getTxMAX(socket)) ret = W5500_getTxMAX(socket); // 获取发送最大缓存
    else ret = len;

    do
    {
        freesize = W5500_getSn_TX_FSR(port, socket);                // 获取发送空闲缓存大小
        status = W5500_SPIRead(port, Sn_SR(socket));                // 读取 Sn_SR 状态
        if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
        {
            ret = 0;
            break;
        }
    } while (freesize < ret);                                       // 等待足够多的空闲空间

    W5500_send_data_processing(port, socket, (uint8_t *)buf, ret);  // 发送进程

    W5500_SPIWrite(port, Sn_CR(socket) ,Sn_CR_SEND);                // 发送命令

    W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

    while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
    {
        if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
        {
            W5500_Status[port] = W5500_RD_TimeOut;
            break;
        }
    }

    while ( (W5500_SPIRead(port, Sn_IR(socket)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK ) // 等待发送完成
    {
        status = W5500_SPIRead(port, Sn_SR(socket));                // 读取socket状态
        if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
        {
          W5500_close(port, socket);
          return 0;
        }
    }

    W5500_SPIWrite(port, Sn_IR(socket) , Sn_IR_SEND_OK);            // 清除标志位

    return ret;
}

uint8_t W5500_socket(uint16_t port, uint16_t socket, uint8_t protocol, uint16_t ip_port, uint8_t flag)
{
    uint8_t ret;
    uint8_t i = 0;
   if (((protocol&0x0F) == Sn_MR_TCP)     ||
        ((protocol&0x0F) == Sn_MR_UDP)    ||
        ((protocol&0x0F) == Sn_MR_IPRAW)  ||
        ((protocol&0x0F) == Sn_MR_MACRAW) ||
        ((protocol&0x0F) == Sn_MR_PPPOE))
   {
       W5500_close(port, socket);

      if((protocol&0x0F) == Sn_MR_TCP)  // 如果是TCP 模式，需要设置心跳
      {
          W5500_setkeepalive(port, socket);
      }
      W5500_SPIWrite(port, Sn_MR(socket) ,protocol | flag);
      if (ip_port != 0)
      {
        W5500_SPIWrite(port, Sn_PORT0(socket) ,(uint8_t)((ip_port & 0xff00) >> 8));
        W5500_SPIWrite(port, Sn_PORT1(socket) ,(uint8_t)(ip_port & 0x00ff));
      }
      else
      {
        i++;
        W5500_SPIWrite(port, Sn_PORT0(socket) ,(uint8_t)((local_port[ETH_Array(port,socket)] & 0xff00) >> 8));
        W5500_SPIWrite(port, Sn_PORT1(socket) ,(uint8_t)(local_port[ETH_Array(port,socket)] & 0x00ff));
      }
      W5500_SPIWrite(port, Sn_CR(socket) ,Sn_CR_OPEN); // run sockinit Sn_CR

      /* wait to process the command... */
      W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

      while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
      {
          if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
          {
              W5500_Status[port] = W5500_RD_TimeOut;
              break;
          }
      }
      /* ------- */
      ret = 1;
   }
   else
   {
      ret = 0;
   }
   return ret;
}

/*  W5500 断开连接  */
void W5500_disconnect(uint16_t port, uint16_t socket)
{
    W5500_SPIWrite(port, Sn_CR(socket) ,Sn_CR_DISCON);

   /* wait to process the command... */
    W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

    while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
    {
        if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
        {
            W5500_Status[port] = W5500_RD_TimeOut;
            break;
        }
    }
}

/*  W5500监听     */
uint8_t W5500_listen(uint16_t port, uint16_t socket)
{
    uint8_t     ret = 0;            // 定义一个监听标志位，若Sn_CR的LISTEN命令发送成功，其值为1，否则为0
    uint16_t status;
    status = W5500_getSnSR(port, socket);   // 获取 socket 的状态
    if(status == SOCK_INIT)                 // 若Sn_SR处于初始化状态，进入循环
    {
        W5500_SPIWrite(port, Sn_CR(socket) ,Sn_CR_LISTEN);      // MCU配置W5500为监听状态

        W5500_secondDlyCnt[port] = 0;                           // 清零延迟函数

        while(W5500_SPIRead(port, Sn_CR(socket)))               // 等待发送完成
        {
            if(W5500_secondDlyCnt[port] >= W5500_secondDlyCntLimit)
            {
                W5500_Status[port] = W5500_RD_TimeOut;
                break;
            }
        }

        ret = 1;                            // 配置完成，Sn_CR自动清零
    }
    else
    {
        ret = 0;
    }
    return ret;
}

// W5500 网络配置
void W5500_NetWorkConfig(void)
{
    /*   硬重启W5500    */
    W5500_Reset();

    /*   W5500的IP信息初始化    */
    W5500_setDefault(ETH1);
//    W5500_setDefault(ETH2);

    W5500_setNetWork(ETH1);
//    W5500_setNetWork(ETH2);
}

/* W5500 测试函数   */
void W5500_Test(void)
{
    /*   W5500的IP信息初始化    */
//    W5500_setDefault(ETH1);
//    W5500_setDefault(ETH2);
//
//    W5500_setNetWork(ETH1);
//    W5500_setNetWork(ETH2);

    W5500_Control(ETH1,8);
//    W5500_Control(ETH2,8);
}

// W5500 TCP 客户端
void W5500_ETH_Contorl(uint16_t port, uint8_t socket, uint8_t socket_type)
{
    extern void W5500_FrameReceive(uint16_t port, uint8_t socket, uint16_t type);
    // W5500 发送帧函数
    extern void W5500_FrameSend(uint16_t port, uint8_t socket, uint16_t type);
    uint16_t status;
//    uint8_t len;
//    uint16_t w5500_testtouch = 0; // 临时变量

    status = W5500_getSnSR(port, socket);                 // 获取 socket 的状态
    switch(status)
    {
    case SOCK_INIT:                                       // Socket处于初始化完成(打开)状态
        if(socket_type == TCP_Client)
        {
            if(W5500_secondClientDlyCnt[ETH_Array(port,socket)] >= W5500_secondDlyCntLimit)
            {

                W5500_secondClientDlyCnt[ETH_Array(port,socket)] = 0;
                connect_flag[socket] = 0;                // 超时未连接, 超时重新连接
            }
            if(connect_flag[socket] == 0)                 // 若还未发起连接，发起连接
            {
                W5500_connect(port, socket, server_ip[ETH_Array(port,socket)], server_port[ETH_Array(port,socket)]);

                W5500_secondClientDlyCnt[ETH_Array(port,socket)] = 0;

                connect_flag[socket] = 1;
            }
        }
        else if(socket_type == TCP_Server)
        {
            W5500_listen(port, socket);                  // 监听刚刚打开的本地
        }
        break;
    case SOCK_ESTABLISHED:                               // Socket处于连接建立阶段
        if(W5500_getSnIR(port, socket) & Sn_IR_CON)
        {
            W5500_setSnIR(port, socket, Sn_IR_CON);      // Sn_IR的CON位置1,通知W5500连接已建立
        }
        // 接收函数
        W5500_FrameReceive(port, socket, SOCK_ESTABLISHED);
        // 发送函数
        W5500_FrameSend(port, socket, SOCK_ESTABLISHED);
#if 0
        /*  数据回环测试程序：数据从上位机客户端发给W5500，W5500接收到数据后再回到客户端 */
        len = W5500_getSnRxSize(port, socket);           // 读取W5500空闲接收缓存寄存器的值，表示接收缓存中已接受和保存的数据大小

        if(len > 0)
        {
            w5500_ETH_msg[port].socketRx[socket].len    = len;                              // 接收到的数据
            W5500_recv(port, socket, w5500_ETH_msg[port].socketRx[socket].data, len);       // W5500接收来自客户端的数据，并通道SPI发送到SPI


            if(w5500_testtouch == 0)
            {
                memcpy(w5500_ETH_msg[port].socketTx[socket].data, w5500_ETH_msg[port].socketRx[socket].data, sizeof(w5500_ETH_msg[port].socketTx[socket].data));
                w5500_ETH_msg[port].socketTx[socket].len = w5500_ETH_msg[port].socketRx[socket].len;
                W5500_send(port, socket, w5500_ETH_msg[port].socketTx[socket].data, len); // 接收到数据后再回到客户端，完成数据回环
                w5500_testtouch = 0;
            }

        }
#endif
        break;
    case SOCK_CLOSE_WAIT:                                // Socket处于等待关闭状态
        // 此状态仍可以处理收发事务
        W5500_disconnect(port, socket);                  // 发起断开连接命令，满足4次挥手
        break;
    case SOCK_UDP:
        // 接收函数
        W5500_FrameReceive(port, socket, SOCK_UDP);
        // 发送函数
        W5500_FrameSend(port, socket, SOCK_UDP);
#if 0
        // 回环测试程序：数据从远程上位机发给W5500，W5500接收到数据再回到远程上位机
        if((len = W5500_getSnRxSize(port, socket)) > 0)
        {
            w5500_ETH_msg[port].socketRx[socket].len    = len;
            len = W5500_recvfrom(port, socket, w5500_ETH_msg[port].socketRx[socket].data, len,
                                 server_ip[ETH_Array(port,socket)], &server_port[ETH_Array(port,socket)]); // W5500接收来自远程上位机的数据,并通过SPI发送给MCU
                                      // 接收到的数据

            if(w5500_testtouch == 0)
            {
                memcpy(w5500_ETH_msg[port].socketTx[socket].data, w5500_ETH_msg[port].socketRx[socket].data, sizeof(w5500_ETH_msg[port].socketTx[socket].data));
                w5500_ETH_msg[port].socketTx[socket].len = w5500_ETH_msg[port].socketRx[socket].len;
                W5500_sendto(port, socket, w5500_ETH_msg[port].socketTx[socket].data, len,
                             server_ip[ETH_Array(port,socket)], server_port[ETH_Array(port,socket)]);          // 接受到数据后再回给远程上位机，完成数据回环
                w5500_testtouch = 0;
            }

        }
#endif
        break;
    case SOCK_CLOSED:
        if(socket_type == Socket_UDP)
        {
            W5500_socket(port,socket,Sn_MR_UDP,local_port[ETH_Array(port,socket)],0);           // UDP
        }
        else
        {
            connect_flag[socket] = 0;
            W5500_socket(port,socket,Sn_MR_TCP,local_port[ETH_Array(port,socket)],Sn_MR_ND);    // TCP
        }
        break;
    }
}



/*  W5500控制  */
void W5500_Control(uint16_t port, uint8_t socket_num)
{
    uint16_t socket_n = 0;
    uint16_t close_flag[ETH_NUM] = {0};

    extern void SPI_RX_ArrayInit(uint16_t port);
    extern void SPI_TX_ArrayInit(uint16_t port);
    extern void DMA_Channel_ReStart(uint16_t port);

    if((W5500_getPHYCFGR(port) & LINK) != LINK)         // 网线未连接
    {
        if(close_flag[port] == 0)
        {
            for(socket_n = 0; socket_n < socket_num; socket_n++)
            {
                W5500_socketClose(port, socket_n);      // 关闭 socket
                connect_flag[socket_n] = 1;
            }
            W5500_Status[port] = W5500_DisConnect;
            close_flag[port] = 1;
        }
    }
    else
    {
        switch(W5500_Status[port])
        {
        case W5500_Normal:
            // 网线已插入
            close_flag[port] = 0;
            socket_n = 0;
            W5500_ETH_Contorl(port, socket_n,TCP_Client);
            socket_n = 1;
            W5500_ETH_Contorl(port, socket_n,TCP_Client);
            socket_n = 2;
            W5500_ETH_Contorl(port, socket_n,Socket_UDP);
            socket_n = 3;
            W5500_ETH_Contorl(port, socket_n,TCP_Client);
            socket_n = 4;
            W5500_ETH_Contorl(port, socket_n,TCP_Client);
            socket_n = 5;
            W5500_ETH_Contorl(port, socket_n,TCP_Server);
            socket_n = 6;
            W5500_ETH_Contorl(port, socket_n,Socket_UDP);
            break;
        case W5500_CRCTxError:
            // 清除发送数据

            W5500_Status[port] = W5500_Normal;     // 重新进入正常模式

            break;
        case W5500_CRCRxError:
            // 清除接收数据

            W5500_Status[port] = W5500_Normal;     // 重新进入正常模式

            break;
        case W5500_WD_TimeOut:
        case W5500_RD_TimeOut:
        case W5500_Error:
            // 错误清理
            W5500_Reset();
            // 解决SPI DMA异常的方式，重启DMA和SPI
            SPI_RX_ArrayInit(Board_SPI);
            SPI_TX_ArrayInit(Board_SPI);
            DMA_Channel_ReStart(DMA_SPIB_RX);
            DMA_Channel_ReStart(DMA_SPIB_TX);

            W5500_Status[port] = W5500_Normal;  // 重新进入正常模式
            break;
        case W5500_DisConnect:                  // 网线重新插入
            W5500_Status[port] = W5500_Normal;  // 重新进入正常模式
        default:
            W5500_Status[port] = W5500_Error;   // 进入错误模式
            break;
        }
    }
}


