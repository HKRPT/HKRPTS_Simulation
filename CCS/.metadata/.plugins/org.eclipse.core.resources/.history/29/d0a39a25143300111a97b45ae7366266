#ifndef SOURCE_MAIN_H_
#define SOURCE_MAIN_H_

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "F2837xD_device.h"
#include "F28x_Project.h"
#include  "my_dma.h"
#include "dido.h"
#include "my_xmif.h"
#include "GlobalVariables.h"
#include "recoder.h"
#include "fpga.h"
#include "customFrame.h"
//#include "my_can.h"

#define YHYComputer             0
#define ProgramVersion          0xAA            // 0xAA为APP 0x55为boot
#if(ProgramVersion == 0xAA)
#define APP     1
#elif (ProgramVersion == 0x55)
#define Boot    1
#endif
/*  设备ID  */
#define CID                     0x01
#define Major                   0x01
#define Minor                   0x02
#define Revision                0x04
#define Build                   0x06
/* 引脚定义 */
#define low_level               0
#define high_level              1
#define nop                     asm(" nop")

// 通讯缓冲数组长度
#define Comm_BufLen             280

// PWM 定义
#define __FPLL    200.0e6       //  200.0MHz
#define __FC        3.0e3       //  3.0KHz
#define __FT         __FC       //   中断周期
#define __FTs      1/__FC       //   中断周期
#define __PI        3.1415926536F
#define __CLKDIV    TB_DIV2
#define __HSPCLKDIV TB_DIV1
#define PI          3.1415926536

#define __ET_XRD    ET_1ST

#define TPRD (__FPLL/(2.0 * (TB_DIV1+1.0) * (__CLKDIV+1.0) * __FC))

#ifndef __RST__
#define __RST__             0x5AA5
#endif

#ifndef __ENABLE__
#define __ENABLE__          0x5AA5
#endif

#ifndef __DISABLE__
#define __DISABLE__         0xFFFF
#endif

/*  FPGA  */
#define FPGA_RST_GPIOMUX            GPIO_68_GPIO68
#define FPGA_RST_GPIO               68
#define FPGA_RST(x)                 do{x ? GPIO_writePin(FPGA_RST_GPIO, high_level):GPIO_writePin(FPGA_RST_GPIO, low_level);} while(0)

/*  DMA     */

// DMA 头文件结构体
#define DMA_SPIB_TX         4           // DMA Channel 5 作为 SPIB发送
#define DMA_SPIB_RX         5           // DMA Channel 6 作为 SPIB接收
#define DMA_SPIB_TX_Config  DMA_SPIBTX
#define DMA_SPIB_RX_Config  DMA_SPIBRX
#define DMA_SPIA_TX         0           // DMA Channel 1 作为 SPIA发送
#define DMA_SPIA_RX         1           // DMA Channel 2 作为 SPIA接收
#define DMA_SPIA_TX_Config  DMA_SPIATX
#define DMA_SPIA_RX_Config  DMA_SPIARX
#define DMA_SPIC_TX         2           // DMA Channel 3 作为 SPIC发送
#define DMA_SPIC_RX         3           // DMA Channel 4 作为 SPIC接收
#define DMA_SPIC_TX_Config  DMA_SPICTX
#define DMA_SPIC_RX_Config  DMA_SPICRX

/*  运行灯   */

#define RunLed1GPIOMUX               GPIO_126_GPIO126
#define RunLed1GPIO                  126
#define RunLed1GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO126
#define RunLed1(x)                   do{x ? GPIO_writePin(RunLed1GPIO, high_level):GPIO_writePin(RunLed1GPIO, low_level);} while(0)

#define RunLed2GPIOMUX               GPIO_127_GPIO127
#define RunLed2GPIO                  127
#define RunLed2GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO127
#define RunLed2(x)                   do{x ? GPIO_writePin(RunLed2GPIO, high_level):GPIO_writePin(RunLed2GPIO, low_level);} while(0)

#define RunLed3GPIOMUX               GPIO_128_GPIO128
#define RunLed3GPIO                  128
#define RunLed3GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO128
#define RunLed3(x)                   do{x ? GPIO_writePin(RunLed3GPIO, high_level):GPIO_writePin(RunLed3GPIO, low_level);} while(0)

#define RunLed4GPIOMUX               GPIO_129_GPIO129
#define RunLed4GPIO                  129
#define RunLed4GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO129
#define RunLed4(x)                   do{x ? GPIO_writePin(RunLed4GPIO, high_level):GPIO_writePin(RunLed4GPIO, low_level);} while(0)

#define RunLed5GPIOMUX               GPIO_130_GPIO130
#define RunLed5GPIO                  130
#define RunLed5GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO130
#define RunLed5(x)                   do{x ? GPIO_writePin(RunLed5GPIO, high_level):GPIO_writePin(RunLed5GPIO, low_level);} while(0)

#define RunLed6GPIOMUX               GPIO_131_GPIO131
#define RunLed6GPIO                  131
#define RunLed6GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO131
#define RunLed6(x)                   do{x ? GPIO_writePin(RunLed6GPIO, high_level):GPIO_writePin(RunLed6GPIO, low_level);} while(0)

#define RunLed7GPIOMUX               GPIO_132_GPIO132
#define RunLed7GPIO                  132
#define RunLed7GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO132
#define RunLed7(x)                   do{x ? GPIO_writePin(RunLed7GPIO, high_level):GPIO_writePin(RunLed7GPIO, low_level);} while(0)

#define RunLed8GPIOMUX               GPIO_133_GPIO133
#define RunLed8GPIO                  133
#define RunLed8GPIOTOGGLE            GpioDataRegs.GPDTOGGLE.bit.GPIO133
#define RunLed8(x)                   do{x ? GPIO_writePin(RunLed8GPIO, high_level):GPIO_writePin(RunLed8GPIO, low_level);} while(0)


/*  CAN   */
#define DSP_CANATx_GPIOMUX          GPIO_19_CANTXA
#define DSP_CANATx_GPIO             19
#define DSP_CANARx_GPIOMUX          GPIO_18_CANRXA
#define DSP_CANARx_GPIO             18

#define DSP_CANBTx_GPIOMUX          GPIO_12_CANTXB
#define DSP_CANBTx_GPIO             12
#define DSP_CANBRx_GPIOMUX          GPIO_13_CANRXB
#define DSP_CANBRx_GPIO             13

/*  SPIB    */
#define Board_W5500_SPI             1

#define SPIB_CS_GPIO_PIN            66
#define SPIB_CS_GPIO_PIN_CONFIG     GPIO_66_SPISTEB

#define SPIB_MISO_GPIO_PIN          64
#define SPIB_MISO_GPIO_PIN_CONFIG   GPIO_64_SPISOMIB

#define SPIB_MOSI_GPIO_PIN          63
#define SPIB_MOSI_GPIO_PIN_CONFIG   GPIO_63_SPISIMOB

#define SPIB_MCLK_GPIO_PIN          65
#define SPIB_MCLK_GPIO_PIN_CONFIG   GPIO_65_SPICLKB

/*  SPIB    */
#define Board_SPI                   1                   // 板上SPI选择

// SPI C
#define SPIC_CS_GPIO_PIN            66
#define SPIC_CS_GPIO_PIN_CONFIG     GPIO_125_SPISTEC

#define SPIC_MISO_GPIO_PIN          122
#define SPIC_MISO_GPIO_PIN_CONFIG   GPIO_122_SPISIMOC

#define SPIC_MOSI_GPIO_PIN          123
#define SPIC_MOSI_GPIO_PIN_CONFIG   GPIO_122_SPISIMOC

#define SPIC_MCLK_GPIO_PIN          124
#define SPIC_MCLK_GPIO_PIN_CONFIG   GPIO_124_SPICLKC

// SPI A
#define SPIA_CS_GPIO_PIN            57
#define SPIA_CS_GPIO_PIN_CONFIG     GPIO_57_SPISTEA

#define SPIA_MISO_GPIO_PIN          58
#define SPIA_MISO_GPIO_PIN_CONFIG   GPIO_58_SPISIMOA

#define SPIA_MOSI_GPIO_PIN          59
#define SPIA_MOSI_GPIO_PIN_CONFIG   GPIO_59_SPISOMIA

#define SPIA_MCLK_GPIO_PIN          60
#define SPIA_MCLK_GPIO_PIN_CONFIG   GPIO_60_SPICLKA


/*  FLASH CS */
#define FLASH_CS_GPIOMUX            GPIO_131_GPIO131
#define FLASH_CS_GPIO               131
#define FLASH_CS(x)                 do{x ? GPIO_writePin(FLASH_CS_GPIO, high_level):GPIO_writePin(FLASH_CS_GPIO, low_level);} while(0)

#define FLASH_DMA_TX_Handle         DMAChannel[DMA_SPIA_TX]
#define FLASH_DMA_RX_Handle         DMAChannel[DMA_SPIA_RX]
#define FLASH_SPI_Handle            SPI_REG[0]

/*  FRAM CS */
#define FRAM_CS_GPIOMUX             GPIO_134_GPIO134
#define FRAM_CS_GPIO                134
#define FRAM_CS(x)                  do{x ? GPIO_writePin(FRAM_CS_GPIO, high_level):GPIO_writePin(FRAM_CS_GPIO, low_level);} while(0)

#define FRAM_DMA_TX_Handle          DMAChannel[DMA_SPIC_TX]
#define FRAM_DMA_RX_Handle          DMAChannel[DMA_SPIC_RX]
#define FRAM_SPI_Handle             SPI_REG[2]

/*  ETH1  */
#define ETH1_CS_GPIOMUX             GPIO_133_GPIO133
#define ETH1_CS_GPIO                133
#define ETH1_CS(x)                  do{x ? GPIO_writePin(ETH1_CS_GPIO, high_level):GPIO_writePin(ETH1_CS_GPIO, low_level);} while(0)

#define ETH1_INT_GPIOMUX            GPIO_29_GPIO29
#define ETH1_INT_GPIO               29
#define ETH1_INT(x)                 do{x ? GPIO_writePin(ETH1_INT_GPIO, high_level):GPIO_writePin(ETH1_INT_GPIO, low_level);} while(0)

/*  ETH2 CS */
#define ETH2_CS_GPIOMUX             GPIO_132_GPIO132
#define ETH2_CS_GPIO                132
#define ETH2_CS(x)                  do{x ? GPIO_writePin(ETH2_CS_GPIO, high_level):GPIO_writePin(ETH2_CS_GPIO, low_level);} while(0)

#define ETH2_INT_GPIOMUX            GPIO_30_GPIO30
#define ETH2_INT_GPIO               30
#define ETH2_INT(x)                 do{x ? GPIO_writePin(ETH2_INT_GPIO, high_level):GPIO_writePin(ETH2_INT_GPIO, low_level);} while(0)

#define ETHx_RST_GPIOMUX            GPIO_28_GPIO28
#define ETHx_RST_GPIO               28
#define ETHx_RST(x)                 do{x ? GPIO_writePin(ETHx_RST_GPIO, high_level):GPIO_writePin(ETHx_RST_GPIO, low_level);} while(0)

#define ETHx_DMA_TX_Handle          DMAChannel[DMA_SPIB_TX]
#define ETHx_DMA_RX_Handle          DMAChannel[DMA_SPIB_RX]
#define ETHx_SPI_Handle             SPI_REG[Board_SPI]


#define DSP_PWM1A_GPIOMUX            GPIO_145_EPWM1A
#define DSP_PWM1A_GPIO               145

#define DSP_PWM1B_GPIOMUX            GPIO_146_EPWM1B
#define DSP_PWM1B_GPIO               146

#define DSP_PWM2A_GPIOMUX            GPIO_147_EPWM2A
#define DSP_PWM2A_GPIO               147

#define DSP_PWM2B_GPIOMUX            GPIO_148_EPWM2B
#define DSP_PWM2B_GPIO               148

#define DSP_PWM3A_GPIOMUX            GPIO_149_EPWM3A
#define DSP_PWM3A_GPIO               149

#define DSP_PWM3B_GPIOMUX            GPIO_150_EPWM3B
#define DSP_PWM3B_GPIO               150

#define DSP_PWM4A_GPIOMUX            GPIO_151_EPWM4A
#define DSP_PWM4A_GPIO               151

#define DSP_PWM4B_GPIOMUX            GPIO_152_EPWM4B
#define DSP_PWM4B_GPIO               152

#define DSP_PWM5A_GPIOMUX            GPIO_153_EPWM5A
#define DSP_PWM5A_GPIO               153

#define DSP_PWM5B_GPIOMUX            GPIO_154_EPWM5B
#define DSP_PWM5B_GPIO               154

#define DSP_PWM6A_GPIOMUX            GPIO_155_EPWM6A
#define DSP_PWM6A_GPIO               155

#define DSP_PWM6B_GPIOMUX            GPIO_156_EPWM6B
#define DSP_PWM6B_GPIO               156

#define DSP_PWM7A_GPIOMUX            GPIO_157_EPWM7A
#define DSP_PWM7A_GPIO               157

#define DSP_PWM7B_GPIOMUX            GPIO_158_EPWM7B
#define DSP_PWM7B_GPIO               158

#define DSP_PWM8A_GPIOMUX            GPIO_159_EPWM8A
#define DSP_PWM8A_GPIO               159

#define DSP_PWM8B_GPIOMUX            GPIO_160_EPWM8B
#define DSP_PWM8B_GPIO               160
/*      ADC         */
#define MCU_ADC01_PinMux            GPIO_233_GPIO233
#define MCU_ADC01_Pin               233

/*      串口         */
#define DSP_sciATx_GPIOMUX            GPIO_135_SCITXDA
#define DSP_sciATx_GPIO               135
#define DSP_sciARx_GPIOMUX            GPIO_136_SCIRXDA
#define DSP_sciARx_GPIO               136

#define DSP_sciBTx_GPIOMUX            GPIO_137_SCITXDB
#define DSP_sciBTx_GPIO               137
#define DSP_sciBRx_GPIOMUX            GPIO_138_SCIRXDB
#define DSP_sciBRx_GPIO               138

#define DSP_sciCTx_GPIOMUX            GPIO_140_SCITXDC
#define DSP_sciCTx_GPIO               140
#define DSP_sciCRx_GPIOMUX            GPIO_139_SCIRXDC
#define DSP_sciCRx_GPIO               139

#define DSP_sciDTx_GPIOMUX            GPIO_142_SCITXDD
#define DSP_sciDTx_GPIO               142
#define DSP_sciDRx_GPIOMUX            GPIO_141_SCIRXDD
#define DSP_sciDRx_GPIO               141

// DI配置
#define DSP_DI0_GPIOMUX                 GPIO_102_GPIO102
#define DSP_DI0_GPIO                    102
#define DSP_DI0                         GPIO_readPin(DSP_DI0_GPIO)

#define DSP_DI1_GPIOMUX                 GPIO_103_GPIO103
#define DSP_DI1_GPIO                    103
#define DSP_DI1                         GPIO_readPin(DSP_DI1_GPIO)

#define DSP_DI2_GPIOMUX                 GPIO_104_GPIO104
#define DSP_DI2_GPIO                    104
#define DSP_DI2                         GPIO_readPin(DSP_DI2_GPIO)

#define DSP_DI3_GPIOMUX                 GPIO_105_GPIO105
#define DSP_DI3_GPIO                    105
#define DSP_DI3                         GPIO_readPin(DSP_DI3_GPIO)

#define DSP_DI4_GPIOMUX                 GPIO_106_GPIO106
#define DSP_DI4_GPIO                    106
#define DSP_DI4                         GPIO_readPin(DSP_DI4_GPIO)

#define DSP_DI5_GPIOMUX                 GPIO_107_GPIO107
#define DSP_DI5_GPIO                    107
#define DSP_DI5                         GPIO_readPin(DSP_DI5_GPIO)

#define DSP_DI6_GPIOMUX                 GPIO_108_GPIO108
#define DSP_DI6_GPIO                    108
#define DSP_DI6                         GPIO_readPin(DSP_DI6_GPIO)

#define DSP_DI7_GPIOMUX                 GPIO_109_GPIO109
#define DSP_DI7_GPIO                    109
#define DSP_DI7                         GPIO_readPin(DSP_DI7_GPIO)

#define DSP_DI8_GPIOMUX                 GPIO_110_GPIO110
#define DSP_DI8_GPIO                    110
#define DSP_DI8                         GPIO_readPin(DSP_DI8_GPIO)

#define DSP_DI9_GPIOMUX                 GPIO_111_GPIO111
#define DSP_DI9_GPIO                    111
#define DSP_DI9                         GPIO_readPin(DSP_DI9_GPIO)

#define DSP_DI10_GPIOMUX                GPIO_112_GPIO112
#define DSP_DI10_GPIO                   110
#define DSP_DI10                        GPIO_readPin(DSP_DI10_GPIO)

#define DSP_DI11_GPIOMUX                GPIO_113_GPIO113
#define DSP_DI11_GPIO                   113
#define DSP_DI11                        GPIO_readPin(DSP_DI11_GPIO)

#define DSP_DI12_GPIOMUX                GPIO_114_GPIO114
#define DSP_DI12_GPIO                   114
#define DSP_DI12                        GPIO_readPin(DSP_DI12_GPIO)

#define DSP_DI13_GPIOMUX                GPIO_115_GPIO115
#define DSP_DI13_GPIO                   115
#define DSP_DI13                        GPIO_readPin(DSP_DI13_GPIO)

#define DSP_DI14_GPIOMUX                GPIO_116_GPIO116
#define DSP_DI14_GPIO                   116
#define DSP_DI14                        GPIO_readPin(DSP_DI14_GPIO)

#define DSP_DI15_GPIOMUX                GPIO_117_GPIO117
#define DSP_DI15_GPIO                   117
#define DSP_DI15                        GPIO_readPin(DSP_DI15_GPIO)

// DO 配置
#define DSP_DO0_GPIOMUX                 GPIO_161_GPIO161
#define DSP_DO0_GPIO                    161
#define DSP_DO0(x)                      {x ? GPIO_writePin(DSP_DO0_GPIO, high_level):GPIO_writePin(DSP_DO0_GPIO, low_level);} while(0)

#define DSP_DO1_GPIOMUX                 GPIO_162_GPIO162
#define DSP_DO1_GPIO                    162
#define DSP_DO1(x)                      {x ? GPIO_writePin(DSP_DO1_GPIO, high_level):GPIO_writePin(DSP_DO1_GPIO, low_level);} while(0)

#define DSP_DO2_GPIOMUX                 GPIO_163_GPIO163
#define DSP_DO2_GPIO                    163
#define DSP_DO2(x)                      {x ? GPIO_writePin(DSP_DO2_GPIO, high_level):GPIO_writePin(DSP_DO2_GPIO, low_level);} while(0)

#define DSP_DO3_GPIOMUX                 GPIO_164_GPIO164
#define DSP_DO3_GPIO                    164
#define DSP_DO3(x)                      {x ? GPIO_writePin(DSP_DO3_GPIO, high_level):GPIO_writePin(DSP_DO3_GPIO, low_level);} while(0)

#define DSP_DO4_GPIOMUX                 GPIO_165_GPIO165
#define DSP_DO4_GPIO                    165
#define DSP_DO4(x)                      {x ? GPIO_writePin(DSP_DO4_GPIO, high_level):GPIO_writePin(DSP_DO4_GPIO, low_level);} while(0)

#define DSP_DO5_GPIOMUX                 GPIO_166_GPIO166
#define DSP_DO5_GPIO                    166
#define DSP_DO5(x)                      {x ? GPIO_writePin(DSP_DO5_GPIO, high_level):GPIO_writePin(DSP_DO5_GPIO, low_level);} while(0)

#define DSP_DO6_GPIOMUX                 GPIO_167_GPIO167
#define DSP_DO6_GPIO                    167
#define DSP_DO6(x)                      {x ? GPIO_writePin(DSP_DO6_GPIO, high_level):GPIO_writePin(DSP_DO6_GPIO, low_level);} while(0)

#define DSP_DO7_GPIOMUX                 GPIO_168_GPIO168
#define DSP_DO7_GPIO                    168
#define DSP_DO7(x)                      {x ? GPIO_writePin(DSP_DO7_GPIO, high_level):GPIO_writePin(DSP_DO7_GPIO, low_level);} while(0)

#define DSP_DO8_GPIOMUX                 GPIO_67_GPIO67
#define DSP_DO8_GPIO                    67
#define DSP_DO8(x)                      {x ? GPIO_writePin(DSP_DO8_GPIO, high_level):GPIO_writePin(DSP_DO8_GPIO, low_level);} while(0)

#define DSP_DO9_GPIOMUX                 GPIO_95_GPIO95
#define DSP_DO9_GPIO                    95
#define DSP_DO9(x)                      {x ? GPIO_writePin(DSP_DO9_GPIO, high_level):GPIO_writePin(DSP_DO9_GPIO, low_level);} while(0)

#define DSP_DO10_GPIOMUX                 GPIO_96_GPIO96
#define DSP_DO10_GPIO                    96
#define DSP_DO10(x)                      {x ? GPIO_writePin(DSP_DO10_GPIO, high_level):GPIO_writePin(DSP_DO10_GPIO, low_level);} while(0)

#define DSP_DO11_GPIOMUX                 GPIO_97_GPIO97
#define DSP_DO11_GPIO                    97
#define DSP_DO11(x)                      {x ? GPIO_writePin(DSP_DO11_GPIO, high_level):GPIO_writePin(DSP_DO11_GPIO, low_level);} while(0)

#define DSP_DO12_GPIOMUX                 GPIO_98_GPIO98
#define DSP_DO12_GPIO                    98
#define DSP_DO12(x)                      {x ? GPIO_writePin(DSP_DO12_GPIO, high_level):GPIO_writePin(DSP_DO12_GPIO, low_level);} while(0)

#define DSP_DO13_GPIOMUX                 GPIO_99_GPIO99
#define DSP_DO13_GPIO                    99
#define DSP_DO13(x)                      {x ? GPIO_writePin(DSP_DO13_GPIO, high_level):GPIO_writePin(DSP_DO13_GPIO, low_level);} while(0)

#define DSP_DO14_GPIOMUX                 GPIO_100_GPIO100
#define DSP_DO14_GPIO                    100
#define DSP_DO14(x)                      {x ? GPIO_writePin(DSP_DO14_GPIO, high_level):GPIO_writePin(DSP_DO14_GPIO, low_level);} while(0)

#define DSP_DO15_GPIOMUX                 GPIO_101_GPIO101
#define DSP_DO15_GPIO                    101
#define DSP_DO15(x)                      {x ? GPIO_writePin(DSP_DO15_GPIO, high_level):GPIO_writePin(DSP_DO15_GPIO, low_level);} while(0)

// ADC 模拟开关切换
#define DSP_ADCAnalog_A0_GPIOMUX        GPIO_95_GPIO95
#define DSP_ADCAnalog_A0_GPIO           95
#define DSP_ADCAnalog_A0(x)             {x ? GPIO_writePin(DSP_ADCAnalog_A0_GPIO, high_level):GPIO_writePin(DSP_ADCAnalog_A0_GPIO, low_level);} while(0)

#define DSP_ADCAnalog_A1_GPIOMUX        GPIO_96_GPIO96
#define DSP_ADCAnalog_A1_GPIO           96
#define DSP_ADCAnalog_A1(x)             {x ? GPIO_writePin(DSP_ADCAnalog_A1_GPIO, high_level):GPIO_writePin(DSP_ADCAnalog_A1_GPIO, low_level);} while(0)

#define DSP_ADCAnalog_A2_GPIOMUX        GPIO_97_GPIO97
#define DSP_ADCAnalog_A2_GPIO           97
#define DSP_ADCAnalog_A2(x)             {x ? GPIO_writePin(DSP_ADCAnalog_A2_GPIO, high_level):GPIO_writePin(DSP_ADCAnalog_A2_GPIO, low_level);} while(0)

// CAN
//#define DSP_CANATx_GPIOMUX            GPIO_19_CANTXA
//#define DSP_CANATx_GPIO               19
//#define DSP_CANARx_GPIOMUX            GPIO_18_CANRXA
//#define DSP_CANARx_GPIO               18
//
//#define DSP_CANBTx_GPIOMUX            GPIO_12_CANTXB
//#define DSP_CANBTx_GPIO               12
//#define DSP_CANBRx_GPIOMUX            GPIO_13_CANRXB
//#define DSP_CANBRx_GPIO               13

// FPGA 控制
#define FPGA_CSn_GPIOMUX                GPIO_35_EM1CS3N
#define FPGA_CSn_GPIO                   35

#define FPGA_RSTn_GPIOMUX               GPIO_68_GPIO68
#define FPGA_RSTn_GPIO                  68
#define FPGA_RSTn(x)                    {x ? GPIO_writePin(FPGA_RSTn_GPIO, high_level):GPIO_writePin(FPGA_RSTn_GPIO, low_level);} while(0)


extern uint8_t Tx_BUFF[512];
extern uint8_t Rx_BUFF[512];
/*  外设初始化   */
extern void Peripheral_Init(void);
// DMA初始化
void MyDMA_Init(void);
/* SPI 初始化  */
void SPI_Init(void);
// FRAM 数组初始化
void FRAM_ArrayInit(uint16_t port);
// PWM初始化函数
void My_EPWM_Init(void);

/*  DMA通道地址配置    */
void My_DMACHxAddrConfig(volatile struct CH_REGS *CHx,volatile uint16_t *DMA_Dest,volatile uint16_t *DMA_Source);
/*  单次传输值  */
void MY_DMACHxBurstConfig(volatile struct CH_REGS *CHx, int16_t bsize, int16_t srcbstep, int16_t desbstep);
/*      传输数量    */
void MY_DMACHxTransferConfig(volatile struct CH_REGS *CHx, uint16_t tsize, int16_t srctstep, int16_t deststep);
/*      传输数量    */
void MY_DMACHxWrapConfig(volatile struct CH_REGS *CHx, uint16_t srcwsize, int16_t srcwstep, uint16_t deswsize,int16 deswstep);
void MY_DMACHxModeConfig( volatile struct CH_REGS *CHx, uint16_t channel, uint16_t persel, uint16_t perinte, uint16_t oneshot,
                         uint16_t cont, uint16_t synce, uint16_t syncsel, uint16_t ovrinte,
                         uint16_t datasize, uint16_t chintmode, uint16_t chinte);
/* DMA 通道初始化 */
void My_DMA_Chanal_Init(volatile struct CH_REGS *CHx);
// DMA启动
void StartDMACHx(volatile struct CH_REGS *CHx);

/*  测试函数    */
void FRAM_Test(void);
/*  控制函数    */
void FRAM_Contorl(void);
/*  测试函数    */
void FLASH_Test(void);

void CANOpenLoop(void);
#endif
