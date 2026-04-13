/*
 * my_dma.h
 *
 *  Created on: 2025年2月7日
 *      Author: YangHY
 */

#ifndef SOURCE_CODE_INCLUDE_MY_DMA_H_
#define SOURCE_CODE_INCLUDE_MY_DMA_H_

typedef struct
{
    uint16_t    *src_addr;      // 源地址
    uint16_t    *dec_addr;      // 终地址(默认为发送数组)
    uint16_t    len;            // 长度
    uint16_t    head_len;       // 包头长度
    uint32_t    *timeOutCnt;    // 超时计数
    uint32_t    *timeOutFlag;   // 超时标志位
    uint16_t    timeOutErrCode; // 超时错误代码
    uint32_t    timeOutLimit;   // 超时时间限制
    uint16_t    isOneByte;      // 是否为单字节(8位)发送 1则是
    uint16_t    *receiveBuf;    // 接收数组
    volatile struct CH_REGS  *dmaTxChannel;     // dma通道 发送结构体
    volatile struct CH_REGS  *dmaRxChannel;     // dma通道 接收结构体
    volatile struct SPI_REGS *spiChannel;       // spi通道结构体
}DMA_head_t;


// DMA 发送数据
//void myDMASendMsg(DMA_head_t * head);
// DMA收发消息函数
void myDMARecevieSendMsg(DMA_head_t * head);


#endif /* SOURCE_CODE_INCLUDE_MY_DMA_H_ */
