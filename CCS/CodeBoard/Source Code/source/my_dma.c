/*
 * my_dma.c
 *
 *  Created on: 2025年1月20日
 *      Author: BYRD-003
 */
#include "main.h"
#include  "my_dma.h"

volatile struct CH_REGS *DMAChannel[6] = {&DmaRegs.CH1,&DmaRegs.CH2,&DmaRegs.CH3,&DmaRegs.CH4,&DmaRegs.CH5,&DmaRegs.CH6};

/*  DMA通道地址配置    */
void My_DMACHxAddrConfig(volatile struct CH_REGS *CHx,volatile uint16_t *DMA_Dest,volatile uint16_t *DMA_Source)
{
    EALLOW;

    CHx->SRC_BEG_ADDR_SHADOW = (uint32_t)DMA_Source;    // 源起始地址影子寄存器

    CHx->SRC_ADDR_SHADOW     = (uint32_t)DMA_Source;    // 源地址影子寄存器

    CHx->DST_BEG_ADDR_SHADOW = (uint32_t)DMA_Dest;      // 终起始地址影子寄存器

    CHx->DST_ADDR_SHADOW     = (uint32_t)DMA_Dest;      // 终地址影子寄存器

    EDIS;
}

/*  单次传输值  */
void MY_DMACHxBurstConfig(volatile struct CH_REGS *CHx, int16_t bsize, int16_t srcbstep, int16_t desbstep)
{
    EALLOW;

    CHx->BURST_SIZE.all             = bsize;

    CHx->SRC_BURST_STEP             = srcbstep;     // 源地址突发步进值

    CHx->DST_BURST_STEP             = desbstep;     // 终地址步进值

    EDIS;
}

/*      传输数量    */
void MY_DMACHxTransferConfig(volatile struct CH_REGS *CHx, uint16_t tsize, int16_t srctstep, int16_t deststep)
{
    EALLOW;

    CHx->TRANSFER_SIZE      = tsize;        // 传输数量大小

    CHx->SRC_TRANSFER_STEP  = srctstep;     // 源传输步进值

    CHx->DST_TRANSFER_STEP  = deststep;     // 终传输地址步进值

    EDIS;
}

/*      传输数量    */
void MY_DMACHxWrapConfig(volatile struct CH_REGS *CHx, uint16_t srcwsize, int16_t srcwstep, uint16_t deswsize,int16 deswstep)
{
    EALLOW;


    CHx->SRC_WRAP_SIZE = srcwsize;      // 真实值 = WRAP_SIZE + 1
    CHx->SRC_WRAP_STEP = srcwstep;      // 传输步进值

    CHx->DST_WRAP_SIZE = deswsize;      // 真实值 = WRAP_SIZE + 1
    CHx->DST_WRAP_STEP = deswstep;      // 传输步进值

    EDIS;
}

/*
 *     Set up MODE Register:
 *     CHx          -   通道结构体
 *     channel      -   通道号
 *     persel       -   通道源选择
 *     PERINTSEL    -   外设事件触发使能
 *     ONESHOT      -   单次模式
 *     CONTINUOUS   -   连续模式
 *     OVRINTE      -   关闭溢出中断使能
 *     DATASIZE     -   16/32位数据传输位
 *     CHINTMODE    -   通道中断产生模式
 *     CHINTE       -   通道中断使能位
 * */
void MY_DMACHxModeConfig( volatile struct CH_REGS *CHx, uint16_t channel, uint16_t persel, uint16_t perinte, uint16_t oneshot,
                         uint16_t cont, uint16_t synce, uint16_t syncsel, uint16_t ovrinte,
                         uint16_t datasize, uint16_t chintmode, uint16_t chinte)
{
    EALLOW;
    // 模式寄存器
    CHx->MODE.bit.PERINTSEL         = channel;
    CHx->MODE.bit.OVRINTE           = ovrinte;      // 关闭溢出中断使能
    CHx->MODE.bit.PERINTE           = perinte;      // 外设事件触发使能
    CHx->MODE.bit.CHINTMODE         = chintmode;    // 通道中断产生模式
    CHx->MODE.bit.ONESHOT           = oneshot;      // 单次模式
    CHx->MODE.bit.CONTINUOUS        = cont;         // 连续模式
    CHx->MODE.bit.DATASIZE          = datasize;     // 16位数据传输位
    CHx->MODE.bit.CHINTE            = chinte;       // 通道中断使能位

    // DMA控制寄存器
//    CHx->CONTROL.bit.RUN            = 0x00;     // 运行通道，写1使能DMA通道和设置RUNSTS为1.
//    CHx->CONTROL.bit.HALT           = 0x01;     // 停止通道
//    CHx->CONTROL.bit.SOFTRESET      = 0x01;     // 通道软复位
//    CHx->CONTROL.bit.PERINTFRC      = 0x00;     // 强制外设事件触发
    CHx->CONTROL.bit.PERINTCLR      = 0x01;     // 清除外设事件触发
    CHx->CONTROL.bit.ERRCLR         = 0x01;     // 清除错误
//    CHx->CONTROL.bit.PERINTFLG      = 0x00;     // 外设事件触发标志位
//    CHx->CONTROL.bit.TRANSFERSTS    = 0x00;     // 传输状态标志位
//    CHx->CONTROL.bit.BURSTSTS       = 0x00;     // 突发状态标志位
//    CHx->CONTROL.bit.RUNSTS         = 0x00;     // 运行状态标志位
//    CHx->CONTROL.bit.OVRFLG         = 0x00;     // 溢出标志位

    switch(channel)
    {
    case 1:
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1 = persel;
        PieCtrlRegs.PIEIER7.bit.INTx1 = 1;
        break;
    case 2:
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH2 = persel;
        PieCtrlRegs.PIEIER7.bit.INTx2 = 1;
        break;
    case 3:
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH3 = persel;
        PieCtrlRegs.PIEIER7.bit.INTx3 = 1;
        break;
    case 4:
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH4 = persel;
        PieCtrlRegs.PIEIER7.bit.INTx4 = 1;
        break;
    case 5:
        DmaClaSrcSelRegs.DMACHSRCSEL2.bit.CH5 = persel;
        PieCtrlRegs.PIEIER7.bit.INTx5 = 1;
        break;
    case 6:
        DmaClaSrcSelRegs.DMACHSRCSEL2.bit.CH6 = persel;
        PieCtrlRegs.PIEIER7.bit.INTx6 = 1;
        break;
    default:
        DmaClaSrcSelRegs.DMACHSRCSEL2.bit.CH5 = persel;
        PieCtrlRegs.PIEIER7.bit.INTx1 = 1;
        break;
    }
    EDIS;
}
/* DMA 通道初始化 */
void My_DMA_Chanal_Init(volatile struct CH_REGS *CHx)
{
    EALLOW;

    CHx->BURST_COUNT.bit.BURSTCOUNT = 8;        // 当前突发数量剩余次数

    CHx->TRANSFER_COUNT             = 0x00;     // 传输数量

    CHx->SRC_WRAP_COUNT             = 0x00;     // 传输数量

    CHx->DST_WRAP_COUNT             = 0x00;     // 传输数量

    CHx->SRC_BEG_ADDR_ACTIVE        = 0x00;     // 源起始地址寄存器

    CHx->SRC_ADDR_ACTIVE            = 0x00;     // 源地址寄存器

    CHx->DST_BEG_ADDR_ACTIVE        = 0x00;     // 终起始地址寄存器

    CHx->DST_ADDR_ACTIVE            = 0x00;     // 终地址寄存器

    EDIS;
}

// DMA启动
void StartDMACHx(volatile struct CH_REGS *CHx)
{
    EALLOW;
    CHx->CONTROL.bit.RUN = 1;
    EDIS;
}

// DMA初始化
void MyDMA_Init(void)
{
    EALLOW;

    // 优先级复位
    DmaRegs.DMACTRL.bit.PRIORITYRESET           = 0x01;
    // 头指针复位
    DmaRegs.DMACTRL.bit.HARDRESET               = 0x01;

    asm(" nop");

    // 仿真自由运行
    DmaRegs.DEBUGCTRL.bit.FREE                  = 1;

    // 优先级控制
    DmaRegs.PRIORITYCTRL1.bit.CH1PRIORITY       = 0;
    // 优先级状态
    DmaRegs.PRIORITYSTAT.bit.ACTIVESTS_SHADOW   = 0x00;
    DmaRegs.PRIORITYSTAT.bit.ACTIVESTS          = 0x00;

    EDIS;


    My_DMA_Chanal_Init(DMAChannel[DMA_SPIA_TX]);
    My_DMA_Chanal_Init(DMAChannel[DMA_SPIB_TX]);
    My_DMA_Chanal_Init(DMAChannel[DMA_SPIC_TX]);

    // CH1 用作SPIx 发送
    MY_DMACHxModeConfig(DMAChannel[DMA_SPIA_TX], 1 , DMA_SPIA_TX_Config,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_ENABLE);
    // CH3 用作SPIx 发送
    MY_DMACHxModeConfig(DMAChannel[DMA_SPIC_TX], 3 , DMA_SPIC_TX_Config,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_ENABLE);
    // CH5 用作SPIx 发送
    MY_DMACHxModeConfig(DMAChannel[DMA_SPIB_TX], 5 , DMA_SPIB_TX_Config,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_ENABLE);

    // CH3 用作SPIx 接收
    MY_DMACHxModeConfig(DMAChannel[DMA_SPIA_RX], 2 , DMA_SPIA_RX_Config,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_DISABLE);
    // CH5 用作SPIx 接收
    MY_DMACHxModeConfig(DMAChannel[DMA_SPIC_RX], 4 , DMA_SPIC_RX_Config,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_DISABLE);
    // CH6 用作SPIx 接收
    MY_DMACHxModeConfig(DMAChannel[DMA_SPIB_RX], 6 , DMA_SPIB_RX_Config,PERINT_ENABLE,ONESHOT_DISABLE,CONT_DISABLE,
                     SYNC_DISABLE,SYNC_SRC,OVRFLOW_DISABLE,SIXTEEN_BIT,
                     CHINT_END,CHINT_DISABLE);
}

// DMA 恢复
void DMA_Channel_ReStart(uint16_t port)
{
    DMAChannel[port]->CONTROL.bit.PERINTCLR = 0x01;
    DMAChannel[port]->CONTROL.bit.ERRCLR = 0x01;
    DMAChannel[port]->CONTROL.bit.SOFTRESET = 0x01;
}

// DMA收发消息函数
void myDMARecevieSendMsg(DMA_head_t * head)
{
    uint16_t bustNumFirst           = 0;
    uint16_t transferNumFirst       = 0;
    uint16_t bustNumSecond          = 0;
    uint16_t transferNumSecond      = 0;
    uint16_t len = head->len;

    // 计算需要分配的地方
    len += head->head_len; // 加上包头长度
    if(len <= 8)
    {
        bustNumFirst     = len;
        transferNumFirst = 1;
    }
    else
    {
        // 寻找最大公约数
        bustNumFirst        = 1;
        transferNumFirst    = len / bustNumFirst;
    }

    // 若为单字节
    if(head->isOneByte)
    {
        EALLOW;
        head->spiChannel->SPICCR.bit.SPICHAR    = 0x07;         // 8个字节长度
        EDIS;
    }

    // 配置参数
    My_DMACHxAddrConfig(head->dmaTxChannel, &head->spiChannel->SPITXBUF,head->dec_addr);
    MY_DMACHxBurstConfig(head->dmaTxChannel, bustNumFirst - 1, 1, 0);
    MY_DMACHxTransferConfig(head->dmaTxChannel, transferNumFirst - 1 , 1, 0);

    My_DMACHxAddrConfig(head->dmaRxChannel,head->receiveBuf  ,&head->spiChannel->SPIRXBUF);  // 待更改接收数组
    MY_DMACHxBurstConfig(head->dmaRxChannel,  bustNumFirst - 1, 0, 1);
    MY_DMACHxTransferConfig(head->dmaRxChannel, transferNumFirst - 1 , 0, 1);

    // 启动DMA
    EALLOW;
    head->spiChannel->SPIFFTX.bit.TXFFIL        = bustNumFirst ;         // 发送FIFO中断等级位
    head->spiChannel->SPIFFRX.bit.RXFFIL        = bustNumFirst ;         // 接收FIFO中断等级位

    head->spiChannel->SPIFFTX.bit.SPIRST       = 0x00;           // SPI复位  0:复位SPI发送和接收通道 1：恢复SPI FIFO 接收和复位
    head->spiChannel->SPIFFRX.bit.RXFFINTCLR   = 0x01;
    head->spiChannel->SPIFFTX.bit.TXFFINTCLR   = 0x01;
    head->spiChannel->SPIFFTX.bit.SPIRST       = 0x01;           // SPI复位  0:复位SPI发送和接收通道 1：恢复SPI FIFO 接收和复位
    head->dmaRxChannel->CONTROL.bit.SOFTRESET = 0x01;           // DMA软复位
    head->dmaTxChannel->CONTROL.bit.SOFTRESET = 0x01;           // DMA软复位
    head->dmaTxChannel->CONTROL.bit.PERINTCLR = 0x01;           // 清除标志位，遇到问题:不清标志位会导致数据错位
    head->dmaRxChannel->CONTROL.bit.PERINTCLR = 0x01;           // 清除标志位，遇到问题:不清标志位会导致数据错位
    head->dmaRxChannel->CONTROL.bit.RUN       = 0x01;           // 启动DMA
    head->dmaTxChannel->CONTROL.bit.RUN       = 0x01;           // 启动DMA

    head->spiChannel->SPIRXBUF;
    head->dmaTxChannel->CONTROL.bit.PERINTFRC = 0x01;           // 触发事件

    EDIS;
    //
    nop;
    while(head->dmaRxChannel->CONTROL.bit.RUNSTS)               // 等待发送完成
    {
        if(*(head->timeOutCnt) >= head->timeOutLimit)
        {
            *(head->timeOutFlag) = head->timeOutErrCode ;
            break;
        }
    }
    // 若还有剩余，也就是除不尽
    bustNumSecond = len % bustNumFirst;
    if(bustNumSecond != 0)
    {
        transferNumSecond = 1;
        // 配置参数
        My_DMACHxAddrConfig(head->dmaTxChannel, &head->spiChannel->SPITXBUF,&(head->dec_addr[bustNumFirst * transferNumFirst]));
        MY_DMACHxBurstConfig(head->dmaTxChannel, bustNumSecond - 1 , 1, 0);
        MY_DMACHxTransferConfig(head->dmaTxChannel, transferNumSecond - 1, 1, 0);

        My_DMACHxAddrConfig(head->dmaRxChannel,&(head->receiveBuf[bustNumFirst * transferNumFirst]),  &head->spiChannel->SPIRXBUF);
        MY_DMACHxBurstConfig(head->dmaRxChannel,  bustNumSecond - 1, 0, 1);
        MY_DMACHxTransferConfig(head->dmaRxChannel, transferNumSecond - 1, 0, 1);

        // 启动DMA
        EALLOW;
        head->spiChannel->SPIFFTX.bit.TXFFIL        = bustNumSecond ;         // 发送FIFO中断等级位
        head->spiChannel->SPIFFRX.bit.RXFFIL        = bustNumSecond ;         // 接收FIFO中断等级位

        head->spiChannel->SPIFFTX.bit.SPIRST       = 0x00;           // SPI复位  0:复位SPI发送和接收通道 1：恢复SPI FIFO 接收和复位
        head->spiChannel->SPIFFRX.bit.RXFFINTCLR   = 0x01;
        head->spiChannel->SPIFFTX.bit.SPIRST       = 0x01;           // SPI复位  0:复位SPI发送和接收通道 1：恢复SPI FIFO 接收和复位
        head->dmaRxChannel->CONTROL.bit.SOFTRESET = 0x01;           // DMA软复位
        head->dmaTxChannel->CONTROL.bit.SOFTRESET = 0x01;           // DMA软复位
        head->dmaTxChannel->CONTROL.bit.PERINTCLR = 0x01;           // 清除标志位，遇到问题:不清标志位会导致数据错位
        head->dmaRxChannel->CONTROL.bit.PERINTCLR = 0x01;           // 清除标志位，遇到问题:不清标志位会导致数据错位
        head->dmaRxChannel->CONTROL.bit.RUN       = 0x01;           // 启动DMA
        head->dmaTxChannel->CONTROL.bit.RUN       = 0x01;           // 启动DMA

        head->dmaTxChannel->CONTROL.bit.PERINTFRC = 0x01;           // 触发事件

        EDIS;

        nop;
        while(head->dmaRxChannel->CONTROL.bit.RUNSTS)               // 等待发送完成
        {
            if(*(head->timeOutCnt) >= head->timeOutLimit)
            {
                *(head->timeOutFlag) = head->timeOutErrCode ;
                break;
            }
        }
    }

    // 若为单字节
    if(head->isOneByte)
    {
        EALLOW;
        head->spiChannel->SPICCR.bit.SPICHAR    = 0x0F;         // 16个字节长度
        EDIS;
    }
}
