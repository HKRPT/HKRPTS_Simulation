
/*
 * my_epwm.c
 *
 *  Created on: 2024年9月19日
 *      Author: BYRD-yhy
 */
#include "my_epwm.h"


volatile struct EPWM_REGS* EPwmxRegs[8] = {&EPwm1Regs,&EPwm2Regs,&EPwm3Regs,&EPwm4Regs,&EPwm5Regs,&EPwm6Regs,&EPwm7Regs,&EPwm8Regs};

void myEPWM_Init(void)
{

    // 初始化PWM gpio

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    myEPWMx_Init(EPWM1,TPRD, 0, 0 , 150,1,0); // PWM通道初始化
    myEPWMx_Init(EPWM2,TPRD, 0, 0 , 150,1,0); // PWM通道初始化
    myEPWMx_Init(EPWM3,TPRD, 0, 0 , 150,1,0); // PWM通道初始化

    myEPWM_ADC_Init(EPWM1);
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

/**
 * port     PWM索引
 * period   PWM 定时器周期
 * compareX PWM 通道比较值
 * deadband PWM 死区设置
 * PHSEN    PWM移相设置
 * PhsShift PWM移相位大小
 *
 */

void myEPWMx_Init(uint16_t port,uint16_t period, uint16_t compareA, uint16_t compareB,uint16_t deadband,uint16_t PHSEN, uint16_t PhsShift)
{
    EALLOW;
    EPwmxRegs[port]->TBCTL.bit.FREE_SOFT    = EPWM_EMULATION_FREE_RUN;
    EPwmxRegs[port]->TBCTL.bit.CLKDIV       = __CLKDIV;                 // 设置时钟分频值
    EPwmxRegs[port]->TBCTL.bit.HSPCLKDIV    = __HSPCLKDIV;              // TBCLK
    EPwmxRegs[port]->TBCTL.bit.CTRMODE      = EPWM_COUNTER_MODE_UP_DOWN;// 设置时钟计时器模式
    EPwmxRegs[port]->TBCTL.bit.PHSEN        = PHSEN;                     // 关闭相移
    EPwmxRegs[port]->TBCTL.bit.SYNCOSEL     = TB_CTR_ZERO;

    EPwmxRegs[port]->TBPRDHR                = period << 8;              // 设置高分辨率时钟周期
    EPwmxRegs[port]->TBPRD                  = period;                   // 设置时钟周期
    EPwmxRegs[port]->TBCTR                  = 0;                        // 设置时钟计数器值
    EPwmxRegs[port]->TBCTL2.bit.PRDLDSYNC   = EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO;// 设置时钟计时器模式

    EPwmxRegs[port]->TBPHS.bit.TBPHS        = PhsShift;                     // 设置相移

    EPwmxRegs[port]->CMPA.bit.CMPA      = compareA;                     // 设置比较值 CCA
    EPwmxRegs[port]->CMPA.bit.CMPAHR    = compareA << 8;

    EPwmxRegs[port]->CMPB.bit.CMPB      = compareB;                     // 设置比较值 CCB
    EPwmxRegs[port]->CMPB.bit.CMPBHR    = compareB << 8;

    // 设置比较值 CCA/B 影子寄存器模式
    EPwmxRegs[port]->CMPCTL.bit.LOADAMODE   = 0x00;                     //  EPWM_COMP_LOAD_ON_CNTR_ZERO_PERIOD
    EPwmxRegs[port]->CMPCTL.bit.LOADBMODE   = 0x00;                     //  EPWM_COMP_LOAD_ON_CNTR_ZERO_PERIOD
    EPwmxRegs[port]->CMPCTL.bit.SHDWAMODE   = 0x00;                     //  CMPA寄存器操作模式 0x00影子模式
    EPwmxRegs[port]->CMPCTL.bit.SHDWBMODE   = 0x00;                     //  CMPB寄存器操作模式 0x00影子模式
    EPwmxRegs[port]->CMPCTL.bit.LOADASYNC   = 0x00;                     //  影子寄存器加载到活动寄存器事件
    EPwmxRegs[port]->CMPCTL.bit.LOADBSYNC   = 0x00;                     //  影子寄存器加载到活动寄存器事件


    // 设置动作
    EPwmxRegs[port]->AQCTLA.bit.CAU     = AQ_SET;       // 向上到达比较值为1，向下到达时为0
    EPwmxRegs[port]->AQCTLA.bit.CAD     = AQ_CLEAR;

    EPwmxRegs[port]->AQCTLB.bit.CAU     = AQ_CLEAR;     // 向下到达比较值为1，向上到达时为0
    EPwmxRegs[port]->AQCTLB.bit.CAD     = AQ_SET;

    // 设置死区
    EPwmxRegs[port]->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwmxRegs[port]->DBCTL.bit.POLSEL   = DB_ACTV_HIC;
    EPwmxRegs[port]->DBCTL.bit.IN_MODE  = DBA_ALL;
    EPwmxRegs[port]->DBRED.bit.DBRED    = deadband;
    EPwmxRegs[port]->DBFED.bit.DBFED    = deadband;

    // 中断事件
//    EPwmxRegs[port]->ETSEL.bit.INTSEL   = ET_CTR_ZERO;  // 选择中断在0点事件
//    EPwmxRegs[port]->ETSEL.bit.INTEN    = 1;            // 使能中断
//    EPwmxRegs[port]->ETPS.bit.INTPRD    = ET_1ST;       // 生成中断在每次事件发生

    EDIS;
}


void myEPWM_ADC_Init(uint16_t port)
{
    extern __interrupt void ADC_ISR(void);

//    EALLOW;
    EPwmxRegs[port]->ETSEL.bit.SOCAEN       = 1;                        // 使能 EPWM1 SOCA
    EPwmxRegs[port]->ETSEL.bit.SOCASEL      = EPWM_SOC_TBCTR_PERIOD;    // 设置 EPWM1 SOCA 触发源 --- 选择SOC向上事件
    EPwmxRegs[port]->ETPS.bit.SOCAPRD       = 1;                        // 生成中断在第一次事件发生

    // 使能外部ADC0
    SysCtl_enableExtADCSOCSource(0);

    // 关闭温度测量
    ASysCtl_disableTemperatureSensor();
    // 设置外部ADC参考电平
//    ASysCtl_setAnalogReferenceExternal( ASYSCTL_VREFHIA | ASYSCTL_VREFHIB | ASYSCTL_VREFHIC );
    //  设置ADC A 参考电压来源     外部ADC A
//    ADC_setVREF(ADCA_BASE, ADC_REFERENCE_EXTERNAL, ADC_REFERENCE_2_5V);
    //  配置分频值   4分频
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCD_BASE, ADC_CLK_DIV_4_0);
    //  完成转换后产生中断
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCC_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCD_BASE, ADC_PULSE_END_OF_CONV);
    //  启动ADC A
    ADC_enableConverter(ADCA_BASE);
    ADC_enableConverter(ADCB_BASE);
    ADC_enableConverter(ADCC_BASE);
    ADC_enableConverter(ADCD_BASE);
    //  延迟500us 上电
    DEVICE_DELAY_US(500);
    // 关闭ADC Burst 模式
    ADC_disableBurstMode(ADCA_BASE);
    ADC_disableBurstMode(ADCB_BASE);
    ADC_disableBurstMode(ADCC_BASE);
    ADC_disableBurstMode(ADCD_BASE);
    // 设置ADCA SOC优先级
    ADC_setSOCPriority(ADCA_BASE, ADC_PRI_ALL_ROUND_ROBIN);
    ADC_setSOCPriority(ADCB_BASE, ADC_PRI_ALL_ROUND_ROBIN);
    ADC_setSOCPriority(ADCC_BASE, ADC_PRI_ALL_ROUND_ROBIN);
    ADC_setSOCPriority(ADCD_BASE, ADC_PRI_ALL_ROUND_ROBIN);
    // 设置 SOC0 由EPWM1触发 ADC_A1
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN0, 8U);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN1, 8U);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN2, 8U);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN3, 8U);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER4, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN4, 8U);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER5, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN5, 8U);

    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN0, 8U);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN1, 8U);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN2, 8U);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN3, 8U);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER4, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN4, 8U);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER5, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN5, 8U);

    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN0, 8U);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN1, 8U);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN2, 8U);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN3, 8U);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER4, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN4, 8U);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER5, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN5, 8U);

    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN0, 8U);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN1, 8U);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN2, 8U);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN3, 8U);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER4, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN4, 8U);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER5, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN5, 8U);
    // 设置ADC 中断源为 ADC SOC0
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER5);
    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER5);
    ADC_setInterruptSource(ADCC_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER5);
    ADC_setInterruptSource(ADCD_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER5);
    // 清除ADC状态
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1);
    // 使能连续转换模式
    ADC_enableContinuousMode(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableContinuousMode(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_enableContinuousMode(ADCC_BASE, ADC_INT_NUMBER1);
    ADC_enableContinuousMode(ADCD_BASE, ADC_INT_NUMBER1);
    // 使能ADC中断
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCC_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCD_BASE, ADC_INT_NUMBER1);
    // 设置中断寄存器
    Interrupt_register(INT_ADCA1, &ADC_ISR);
    // 使能ADC中断
    Interrupt_enable(INT_ADCA1);

    EDIS;

}

//变频
void SetPWMPeriod(uint16_t port , uint16_t value)
{
    EPwmxRegs[port]->TBPRDHR                = value << 8;              // 设置高分辨率时钟周期
    EPwmxRegs[port]->TBPRD                  = value;                   // 设置时钟周期
}

//移相
void SetPWMPhase(uint16_t port,uint16_t PhsShift)
{
    EPwmxRegs[port]->TBPHS.bit.TBPHS = PhsShift;
}

//改占空比
void SetPWMCompared(uint16_t port,uint16_t CMP)
{
    EPwmxRegs[port]->CMPA.bit.CMPA = CMP;
}


void DSC_PWM_OuputLock(void)
{
    // 强制拉低电平
    EALLOW;
    EPwm1Regs.TZSEL.bit.OSHT1 = 1;
    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;

    EPwm2Regs.TZSEL.bit.OSHT1 = 1;
    EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO;

    EPwm3Regs.TZSEL.bit.OSHT1 = 1;
    EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO;
    EDIS;
}

void DSC_PWM_OutputUnlock(void)
{
    // 不做任何事情
    EALLOW;
    EPwm1Regs.TZSEL.bit.OSHT1 = 1;
    EPwm1Regs.TZCTL.bit.TZA = TZ_NO_CHANGE;
    EPwm1Regs.TZCTL.bit.TZB = TZ_NO_CHANGE;

    EPwm2Regs.TZSEL.bit.OSHT1 = 1;
    EPwm2Regs.TZCTL.bit.TZA = TZ_NO_CHANGE;
    EPwm2Regs.TZCTL.bit.TZB = TZ_NO_CHANGE;

    EPwm3Regs.TZSEL.bit.OSHT1 = 1;
    EPwm3Regs.TZCTL.bit.TZA = TZ_NO_CHANGE;
    EPwm3Regs.TZCTL.bit.TZB = TZ_NO_CHANGE;
    EDIS;
}
