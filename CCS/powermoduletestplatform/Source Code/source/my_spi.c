/*
 * my_spi.c
 *
 *  Created on: 2025年1月17日
 *      Author: Yang
 */
#include "main.h"

volatile struct SPI_REGS    *SPI_REG[3] = {&SpiaRegs, &SpibRegs, &SpicRegs};

#define SPIx_BRR        ((200E6 / 4) / 50E6) - 1

/* SPI 初始化  */
void SPI_Reg_Init(volatile struct SPI_REGS *SpixRegs)
{
    // SPI控制寄存器
    SpixRegs->SPICCR.bit.SPISWRESET     = 0x00;         // SPI软复位
    SpixRegs->SPICCR.bit.SPICHAR        = 0x0F;         // 16个字节长度
    SpixRegs->SPICCR.bit.SPILBK         = 0x00;         // 关闭回环模式
    SpixRegs->SPICCR.bit.HS_MODE        = 0x01;         // 高速模式使能
    SpixRegs->SPICCR.bit.CLKPOLARITY    = 0x00;         // 时钟极性

    SpixRegs->SPICTL.bit.SPIINTENA      = 0x00;         // SPI 中断使能
    SpixRegs->SPICTL.bit.TALK           = 0x01;         // 发送使能
    SpixRegs->SPICTL.bit.MASTER_SLAVE   = 0x01;         // SPI 网络模式控制   1:主站
    SpixRegs->SPICTL.bit.OVERRUNINTENA  = 0x00;         // 溢出中断使能       0:关闭
    SpixRegs->SPICTL.bit.CLK_PHASE      = 0x01;         // 时钟相位选择

    // SPI状态寄存器
    SpixRegs->SPISTS.bit.BUFFULL_FLAG   = 0x00;         // 发送数组满标志位    0: 发送数组未满 1:发送数组已满
    SpixRegs->SPISTS.bit.INT_FLAG       = 0X00;         // SPI 中断标志位    0:没有完整字节来接收或发送 1:完成发送或者接收最后一位并准备服务
    SpixRegs->SPISTS.bit.OVERRUN_FLAG   = 0x01;         // SPI接收溢出标志位   0:写1清零

    // SPI 波特率寄存器
    SpixRegs->SPIBRR.bit.SPI_BIT_RATE   = SPIx_BRR;    // SPI 波特率  Baud_Rate = (LSPCLK) / (SPIBRR + 1)

    // SPI FIFO 发送寄存器
    SpixRegs->SPIFFTX.bit.TXFFIL        = 0x01;         // 发送FIFO 中断等级位
    SpixRegs->SPIFFTX.bit.TXFFIENA      = 0x00;         // 发送FIFO中断使能
    SpixRegs->SPIFFTX.bit.TXFFINTCLR    = 0x01;         // 发送FIFO 中断清除  写1清除
    SpixRegs->SPIFFTX.bit.TXFFINT       = 0x00;         // 发送FIFO中断标志
    SpixRegs->SPIFFTX.bit.TXFFST        = 0x00;         // 发送FIFO状态     // 0 FIFO为空 10为16字节
    SpixRegs->SPIFFTX.bit.TXFIFO        = 0x00;         // TXFIFO复位 0:FIFO指针复位，并保持复位状态  1：恢复发送FIFO
    SpixRegs->SPIFFTX.bit.SPIFFENA      = 0X01;         // 使能增强型SPI FIFO
    SpixRegs->SPIFFTX.bit.SPIRST        = 0x00;         // SPI复位  0:复位SPI发送和接收通道 1：恢复SPI FIFO 接收和复位

    // SPI RX FIFO 接收寄存器
    SpixRegs->SPIFFRX.bit.RXFFIL        = 0x01;         // 接收FIFO中断等级位
    SpixRegs->SPIFFRX.bit.RXFFIENA      = 0X00;         // 接收FIFO中断使能
    SpixRegs->SPIFFRX.bit.RXFFINTCLR    = 0x01;         // 接收FIFO中断清除位
    SpixRegs->SPIFFRX.bit.RXFFINT       = 0x00;         // 接收FIFO中断标志位
    SpixRegs->SPIFFRX.bit.RXFFST        = 0x00;         // 接收FIFO状态
    SpixRegs->SPIFFRX.bit.RXFIFORESET   = 0x00;         // 接收FIFO复位 0:写0复位FIFO指针并保持复位 1:重新使能接收FIFO操作
    SpixRegs->SPIFFRX.bit.RXFFOVFCLR    = 0x01;         // 接收FIFO溢出清除
    SpixRegs->SPIFFRX.bit.RXFFOVF       = 0x00;         // 接收FIFO溢出位

    // SPI 发送延迟位
    SpixRegs->SPIFFCT.bit.TXDLY         = 0x00;         // 发送FIFO延迟位

    SpixRegs->SPIPRI.bit.STEINV         = 0X00;         // SPI CS 引脚取反
    SpixRegs->SPIPRI.bit.FREE           = 0x01;         // SPI 自由运行
    SpixRegs->SPIPRI.bit.SOFT           = 0x00;

    // 使能FIFO操作
    SpixRegs->SPIFFTX.bit.TXFIFO        = 0x01;         // TXFIFO复位 0:FIFO指针复位，并保持复位状态  1：恢复发送FIFO
    SpixRegs->SPIFFRX.bit.RXFIFORESET   = 0x01;         // 接收FIFO复位 0:写0复位FIFO指针并保持复位 1:重新使能接收FIFO操作
    SpixRegs->SPIFFTX.bit.SPIRST        = 0x01;         // SPI复位  0:复位SPI发送和接收通道 1：恢复SPI FIFO 接收和复位
    SpixRegs->SPICCR.bit.SPISWRESET     = 0x01;         // SPI软复位
}

/* SPI 初始化  */
void SPI_Init(void)
{
    SPI_Reg_Init(SPI_REG[0]);           // spiB
    SPI_Reg_Init(SPI_REG[1]);           // spiB
    SPI_Reg_Init(SPI_REG[2]);           // spiB
}

/*  SPI RX 复位   */
void SPI_RX_ArrayInit(uint16_t port)
{
    SPI_REG[port]->SPIFFRX.bit.RXFIFORESET  = 0x00;         // 接收FIFO复位 0:写0复位FIFO指针并保持复位 1:重新使能接收FIFO操作
    SPI_REG[port]->SPIFFRX.bit.RXFFOVFCLR   = 0x01;         // 清除FIFO溢出标志位
    SPI_REG[port]->SPIFFRX.bit.RXFFOVFCLR   = 0x01;         // 清除FIFO中断
    SPI_REG[port]->SPIFFRX.bit.RXFIFORESET  = 0x01;         // 接收FIFO复位 0:写0复位FIFO指针并保持复位 1:重新使能接收FIFO操作
}

/*  SPI TX 复位   */
void SPI_TX_ArrayInit(uint16_t port)
{
    // SPI FIFO 发送寄存器
    SPI_REG[port]->SPIFFTX.bit.TXFIFO        = 0x00;         // TXFIFO复位 0:FIFO指针复位，并保持复位状态  1：恢复发送FIFO
    SPI_REG[port]->SPISTS.bit.OVERRUN_FLAG   = 0x01;         // SPI接收溢出标志位   0:写1清零
    SPI_REG[port]->SPIFFTX.bit.TXFFINTCLR    = 0x01;         // 发送FIFO 中断清除  写1清除
    SPI_REG[port]->SPIFFTX.bit.TXFIFO        = 0x01;         // TXFIFO复位 0:FIFO指针复位，并保持复位状态  1：恢复发送FIFO
}



