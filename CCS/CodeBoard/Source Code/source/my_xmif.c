/*
 * my_xmif.c
 *
 *  Created on: 2025年9月23日
 *      Author: YangHaoYan
 */
#include "my_xmif.h"


// 外部内存扩展IO接口
void my_xmifGPIOInit(void)
{
    GPIO_setPinConfig(DSP_XMIF_EM1BA1_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_EM1BA1_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_EM1BA1_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_OEn_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_OEn_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_OEn_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_WEn_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_WEn_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_WEn_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A0_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A0_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A0_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A1_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A1_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A1_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A2_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A2_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A2_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A3_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A3_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A3_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A4_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A4_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A4_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A5_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A5_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A5_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A6_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A6_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A6_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A7_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A7_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A7_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A8_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A8_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A8_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A9_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A9_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A9_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A10_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A10_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A10_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A11_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A11_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A11_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A12_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A12_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A12_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A13_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A13_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A13_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A14_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A14_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A14_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A15_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A15_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A15_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A16_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A16_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A16_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A17_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A17_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A17_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_A18_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_A18_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_A18_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D0_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D0_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D0_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D1_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D1_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D1_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D2_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D2_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D2_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D3_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D3_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D3_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D4_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D4_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D4_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D5_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D5_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D5_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D6_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D6_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D6_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D7_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D7_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D7_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D8_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D8_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D8_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D9_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D9_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D9_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D10_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D10_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D10_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D11_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D11_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D11_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D12_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D12_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D12_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D13_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D13_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D13_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D14_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D14_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D14_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_D15_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_D15_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_D15_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_XMIF_CSn_GPIOMUX);
    GPIO_setPadConfig(DSP_XMIF_CSn_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_XMIF_CSn_GPIO, GPIO_QUAL_ASYNC);

}

// 外部存储器初始化
void my_xintfInit(void)
{
    EALLOW;
    ClkCfgRegs.PERCLKDIVSEL.bit.EMIF1CLKDIV = 0;            // 全速率
    Emif1ConfigRegs.EMIF1MSEL.bit.MSEL_EMIF1 = 0x01;        // EMIF1为CPU1主机
    Emif1ConfigRegs.EMIF1ACCPROT0.all = 0x00;               // EMIF1 DMA_WR/CPU_WR/CPU_FETCH
    DELAY_US(1);
    Emif1ConfigRegs.EMIF1COMMIT.bit.COMMIT_EMIF1 = 0x01;    // 锁存EMIF1ACCPROT0寄存器里的值
    Emif1ConfigRegs.EMIF1LOCK.bit.LOCK_EMIF1     = 0x01;    // 些ACCPROT和MSelect将被阻止

    my_xmifGPIOInit();                                      // 配置EMIF   GPIO

    // SRAM
    Emif1Regs.ASYNC_CS2_CR.bit.SS       = 0x00;             // EMIF_ASYNC_NORMAL_MODE
    Emif1Regs.ASYNC_CS2_CR.bit.EW       = 0x00;             // 关闭外部等待模式
    Emif1Regs.ASYNC_CS2_CR.bit.ASIZE    = 0x01;             // 设置16位数据总线
    Emif1Regs.ASYNC_CS2_CR.bit.R_SETUP  = 0x03;             // 读建立时间
    Emif1Regs.ASYNC_CS2_CR.bit.R_STROBE = 0x07;             // 读选通时间
    Emif1Regs.ASYNC_CS2_CR.bit.R_HOLD   = 0x03;             // 读保持时间
    Emif1Regs.ASYNC_CS2_CR.bit.TA       = 0x01;             // 周转周期
    Emif1Regs.ASYNC_CS2_CR.bit.W_SETUP  = 0x03;             // 写建立时间
    Emif1Regs.ASYNC_CS2_CR.bit.W_STROBE = 0x07;             // 写选通时间
    Emif1Regs.ASYNC_CS2_CR.bit.W_HOLD   = 0x03;             // 写保持时间

    // FPGA
    Emif1Regs.ASYNC_CS3_CR.bit.SS       = 0x00;             // EMIF_ASYNC_NORMAL_MODE
    Emif1Regs.ASYNC_CS3_CR.bit.EW       = 0x00;             // 关闭外部等待模式
    Emif1Regs.ASYNC_CS3_CR.bit.ASIZE    = 0x01;             // 设置16位数据总线
    Emif1Regs.ASYNC_CS3_CR.bit.R_SETUP  = 0x03;             // 读建立时间
    Emif1Regs.ASYNC_CS3_CR.bit.R_STROBE = 0x07;             // 读选通时间
    Emif1Regs.ASYNC_CS3_CR.bit.R_HOLD   = 0x03;             // 读保持时间
    Emif1Regs.ASYNC_CS3_CR.bit.TA       = 0x01;             // 周转周期
    Emif1Regs.ASYNC_CS3_CR.bit.W_SETUP  = 0x03;             // 写建立时间
    Emif1Regs.ASYNC_CS3_CR.bit.W_STROBE = 0x07;             // 写选通时间
    Emif1Regs.ASYNC_CS3_CR.bit.W_HOLD   = 0x03;             // 写保持时间
    EDIS;
}


















