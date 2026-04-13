/*
 * my_sci.c
 *
 *  Created on: 2025年2月8日
 *      Author: Yang
 */
#include "my_sci.h"
#include "modbus.h"
#include "bootloader.h"

#if (PORT_NUM == 3)
volatile struct SCI_REGS *SCI_R[PORT_NUM] = {&SciaRegs, &ScibRegs, &ScidRegs};
#else
volatile struct SCI_REGS *SCI_R[PORT_NUM] = {&SciaRegs, &ScibRegs, &ScidRegs, &ScicRegs};
#endif

UART_PaketType UART[PORT_NUM];
#pragma DATA_SECTION(UART, "flash_ramgs");          // map the TX data to memory

#define UART_TX 1
#define UART_RX 0

void (*UART_RX_FrameResolutionFunc[4])(modbusMasterData_t* ) = {NULL,NULL,NULL,NULL};

void UART_ErrSWRESET(uint16_t port)
{
    if (SCI_R[port]->SCIRXST.bit.RXERROR)
    {
        SCI_R[port]->SCICTL1.bit.SWRESET = 0;
    }
    else
    {
        if (!SCI_R[port]->SCIRXST.bit.RXWAKE)
        {
            SCI_R[port]->SCICTL1.bit.SWRESET = 1;
        }
    }
}

void UART_ArrayInit(void)
{
    uint16_t port;
    for (port = 0; port < PORT_NUM; port++)
    {
        UART_RX_ArrayInit(port);
        UART_TX_ArrayInit(port);
    }
}

void UART_RX_ArrayInit(uint16_t port)
{
    UART[port].uartRxOverFlag = 0;
    UART[port].uartRxtimeOutCnt = 0;

    UART[port].rxArray.front = 0;
    UART[port].rxArray.end = 0;
    UART[port].rxArray.lenght = 0;
    UART[port].rxArray.errCode = 0;

    UART[port].rxArray.FirstByte = 0xFF;
    UART[port].rxArray.buffer[0] = 0xFF; // 出问题时, 可以认为是未识别的帧
}

void UART_TX_ArrayInit(uint16_t port)
{
    UART[port].uartTxOverFlag = 1;
    UART[port].uartTxtimeOutCnt = 0;

    UART[port].txArray.front = 0;
    UART[port].txArray.end = 0;
    UART[port].txArray.lenght = 0;
    UART[port].txArray.errCode = 0;
    UART[port].txArray.buffer[0] = 0;

}

void SCI_Init(volatile struct SCI_REGS *rSCI, uint32_t BaudRate)
{
    rSCI->SCIFFTX.bit.SCIRST = 1; // bit 15       1 : SCI FIFO can resume transmit or receive.
    rSCI->SCIFFTX.bit.SCIFFENA = 1; // bit 14     1 : SCI FIFO enhancements are enabled
    rSCI->SCIFFTX.bit.TXFIFORESET = 1; // bit 13  1 : Re-enable transmit FIFO operation
    rSCI->SCIFFTX.bit.TXFFST = 0;// bit 12 - 8    FIFO status
    rSCI->SCIFFTX.bit.TXFFINT = 0; // bit 7       0 : TXFIFO interrupt has not occurred, read-only bit
    rSCI->SCIFFTX.bit.TXFFINTCLR = 1; // bit 6    1 : Write 1 to clear TXFFINT flag in bit 7
    rSCI->SCIFFTX.bit.TXFFIENA = 0; // bit 5      0 : TX FIFO interrupt based on TXFFIL match (less than or equal to) is disabled
    rSCI->SCIFFTX.bit.TXFFIL = 0; // bit 4 - 0

    rSCI->SCIFFRX.bit.RXFFOVF = 0; // bit 15
    rSCI->SCIFFRX.bit.RXFFOVRCLR = 0; // bit 14
    rSCI->SCIFFRX.bit.RXFIFORESET = 1; // bit 13
    rSCI->SCIFFRX.bit.RXFFST = 0; // bit 12 - 8
    rSCI->SCIFFRX.bit.RXFFINT = 0; // bit 7
    rSCI->SCIFFRX.bit.RXFFINTCLR = 1; // bit 6
    rSCI->SCIFFRX.bit.RXFFIENA = 0; // bit 5
    rSCI->SCIFFRX.bit.RXFFIL = 8; // bit 4 - 0

    rSCI->SCIFFCT.all = 0x0;

    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //
    rSCI->SCICCR.all = 0x0007;  // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
    rSCI->SCICTL1.all = 0x0003; // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
    rSCI->SCICTL2.all = 0x0003;
    rSCI->SCICTL2.bit.TXINTENA = 0;
    rSCI->SCICTL2.bit.RXBKINTENA = 0;
    // baud rate B = 115200
    // SCID at 9600 baud
    // @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8A.
    // @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
    // BAUD REG = 50.0e6/(B * 8) - 1
    rSCI->SCIHBAUD.all = ((uint16_t)(50.0e6/(BaudRate*8.0) - 0.5) >> 8) & 0x00FF;
    rSCI->SCILBAUD.all = ((uint16_t)(50.0e6/(BaudRate*8.0) - 0.5) >> 0) & 0x00FF;

    rSCI->SCICTL1.all = 0x0003;     // Relinquish SCI from Reset, and enable RX TX
    rSCI->SCICTL1.bit.SWRESET = 1;
}
void SCI_A_Init(void)
{
    SCI_Init(&SciaRegs, 115200);
}

void SCI_B_Init(void)
{
    SCI_Init(&ScibRegs, 115200);
}

void SCI_C_Init(void)
{
    SCI_Init(&ScicRegs, 115200);
}

void SCI_D_Init(void)
{
    SCI_Init(&ScidRegs, 115200);
}
void UART_TimerTickMicroSeconds(uint32_t CC)
{
    uint8_t port = 0;

    for (port = 0; port < PORT_NUM; port++)
    {
        if (UART[port].uartRxtimeOutCnt++ > 1000) UART[port].uartRxtimeOutCnt = 1000;
    }
}

void UART_RX_Buff(uint16_t port, uint8_t chr)
{
    UART[port].rxArray.errCode = 0;
    UART[port].rxArray.buffer[UART[port].rxArray.front++] = chr;
    UART[port].rxArray.lenght++;
}

// 串口 modbus 构建函数
void UART_modbusConstruction(uint16_t port)
{
    modbusMasterData[0 + port].type         = MD_UART;                      // 消息类型
    modbusMasterData[0 + port].port         = port;                         // 端口
    modbusMasterData[0 + port].Rxlen        = &UART[port].rxArray.lenght;   // 接收缓存区长度
    modbusMasterData[0 + port].Rxbuff       = UART[port].rxArray.buffer;    // 接收缓存区
    modbusMasterData[0 + port].Txlen        = &UART[port].txArray.lenght;   // 发送缓存区长度
    modbusMasterData[0 + port].Txbuff       = UART[port].txArray.buffer;    // 发送缓存区
    modbusMasterData[0 + port].RX_ArrayInit = UART_RX_ArrayInit;            // 数组清除函数
    modbusMasterData[0 + port].exTxFlag     = &UART[port].uartTxOverFlag;   // 发送标志位
    modbusMasterData[0 + port].exRxFlag     = &UART[port].uartRxOverFlag;   // 接收标志位

    modbusMasterData[0 + port].RxOverFlag   = 0x01;                         // 接收标志位
}

void UART_RX_ISR(uint16_t port) // 接收中断
{
    uint8_t chr;
    // 主循环接收, 非中断接收

    if (SCI_R[port]->SCIFFRX.bit.RXFFST == 0)
    {
        if (UART[port].rxArray.lenght) // 正在接收
        {
            if (UART[port].uartRxOverFlag) // 已经提交协议层处理
            {
                UART[port].uartRxtimeOutCnt = 0;
            }
            else // 未提交协议层未处理
            {
                if (UART[port].uartRxtimeOutCnt >= 50) // 50ms内未收到新字节, 判定超时
                {
                    UART_RX_ArrayInit(port); // 丢弃之
                    UART[port].uartRxtimeOutCnt = 0;
                }
            }
        }
        else // 无协议层接收
        {
            if (UART[port].uartRxOverFlag)
            {
                UART_RX_ArrayInit(port);
            }
            UART[port].uartRxtimeOutCnt = 0;
        }

        UART_ErrSWRESET(port);
    }
    else
    {
        UART[port].uartRxtimeOutCnt = 0; // 清除RX超时标志

        chr = SCI_R[port]->SCIRXBUF.all;

        if (UART[port].rxArray.front == 0) UART[port].rxArray.FirstByte = chr; // 收到的第1个字节

        if (UART[port].rxArray.lenght >= (UART_BUFFER_LEN - 1))  // 缓冲区满
        {
            UART_RX_ArrayInit(port);
            UART[port].rxArray.errCode = ERROR_UART_BUFFER_OVER; // 溢出
        }

        // 开启处理帧数据， 从站地址必须匹配
        if (UART[port].rxArray.FirstByte == CID)    // 若为本机数据则接收
        {
            UART_RX_Buff(port, chr);
            if(UART[port].rxArray.lenght >= 2)
            {
                // buff[1]为功能码,功能码03(查询传感器地址)
                if (UART[port].rxArray.buffer[1] == 0x03)
                {
                    // 从发送的03码肯定是8个字节
                    if (UART[port].rxArray.lenght >= 8)
                    {
                        UART[port].uartRxOverFlag = 1;      // 请求协议层进行数据处理

                        UART_modbusConstruction(port);      // 串口modbus构建函数

                        UART_RX_FrameResolutionFunc[port] = modbus03MasterFrameResolution;
                    }
                }
                else if (UART[port].rxArray.buffer[1] == 0x05)
                {
                    // 从发送的05码肯定是8个字节
                    if (UART[port].rxArray.lenght >= 8)
                    {
                        UART[port].uartRxOverFlag = 1;      // 请求协议层进行数据处理

                        UART_modbusConstruction(port);      // 串口modbus构建函数

                        UART_RX_FrameResolutionFunc[port] = modbus05MasterFrameResolution;
                    }
                }
                else if (UART[port].rxArray.buffer[1] == 0x10)
                {
                    // 从发送的05码肯定是8个字节
                    if (UART[port].rxArray.lenght >= 8)
                    {
                        UART[port].uartRxOverFlag = 1;      // 请求协议层进行数据处理

                        UART_modbusConstruction(port);      // 串口modbus构建函数

                        UART_RX_FrameResolutionFunc[port] = modbus10MasterFrameResolution;
                    }
                }
                else
                {
                    // 暂不支持的功能码舍弃
                    UART_RX_ArrayInit(port); // 未知协议, 丢弃之;
                }
            }
        }
        else if(UART[port].rxArray.FirstByte == 0x55)                // 自定义协议解析
        {
            extern void DebugConfigFrameAnalytic(modbusMasterData_t* data) ;
            // 从发送的自定义帧肯定是8个字节
            UART_RX_Buff(port, chr);
            if (UART[port].rxArray.lenght >= 8)
            {
                UART[port].uartRxOverFlag = 1;      // 请求协议层进行数据处理

                UART_modbusConstruction(port);      // 串口modbus构建函数

                UART_RX_FrameResolutionFunc[port] = DebugConfigFrameAnalytic;
            }
        }
        else if(UART[port].rxArray.FirstByte == 0x5A)                // bootloader协议解析
        {
            // bootLoader在线解析函数
            UART_RX_Buff(port, chr);
            extern void BootLoaderFrameFunc(modbusMasterData_t* data);
            // 从发送的自定义帧肯定是8个字节
            if (UART[port].rxArray.lenght >= 11)
            {
                UART[port].uartRxOverFlag = 1;      // 请求协议层进行数据处理

                UART_modbusConstruction(port);      // 串口modbus构建函数

                UART_RX_FrameResolutionFunc[port] = BootLoaderFrameFunc;
            }
        }
        else
        {
            UART_RX_ArrayInit(port); // 未知协议, 丢弃之;
        }
    }
}

uint16_t UART_TX_DlyCnt[PORT_NUM] = {0};
void UART_TX_ISR(uint16_t port)  // 发送中断
{
    if (UART[port].uartTxOverFlag) return;

    if (UART[port].txArray.lenght == 0)   // 无数据正在发送
    {
        if (SCI_R[port]->SCIFFTX.bit.TXFFST) // 还有数据在FIFO上未发送完
        {
            UART_TX_DlyCnt[port] = 0;
        }
        else // FIFO 也是空的
        {
            if (SCI_R[port]->SCICTL2.bit.TXEMPTY)
            {
                if (UART_TX_DlyCnt[port]++ >= 1) UART_TX_ArrayInit(port);
            }
        }
    }
    else
    {
        UART_TX_DlyCnt[port] = 0;

        if (SCI_R[port]->SCIFFTX.bit.TXFFST < 16) // FIFO MAX words = 16
        {
            SCI_R[port]->SCITXBUF.all = UART[port].txArray.buffer[UART[port].txArray.end++]; // 往串口发送
            UART[port].txArray.end = UART[port].txArray.end % UART_BUFFER_LEN;
            UART[port].txArray.lenght--;
        }
    }
}


void UART_CCC(void)
{
    uint16_t port;
    for (port = 0; port < PORT_NUM; port++)
    {
        UART_RX_ISR(port);
        UART_RX_ISR(port);

        if (UART_RX_FrameResolutionFunc[port] != NULL)
        {
            UART_RX_FrameResolutionFunc[port](&modbusMasterData[port]);   // 帧解析
            UART_RX_FrameResolutionFunc[port] = NULL;
        }
        UART_TX_ISR(port);
    }
}
















