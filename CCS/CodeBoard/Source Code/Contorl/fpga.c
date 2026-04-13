/*
 * fpga.c
 *
 *  Created on: 2025年10月31日
 *      Author: YangHaoYan
 */
#include "fpga.h"

uint16_t woHW_DOOutputStates = 0;
void woHW_DOOutputSetState(uint8_t bit, uint16_t s) // 按位置'1'代表关闭, 置'0'代表开通
{
    if (s) // 开通, 清零
    {
        woHW_DOOutputStates &= (~(0x0001 << bit)); // 开
    }
    else // 关闭, 置壹
    {
        woHW_DOOutputStates |= (0x0001 << bit); // 关
    }
    woHW_DOOutputState = woHW_DOOutputStates;
}


// 风扇控制
void FanStateCtrl(uint8_t s)
{
    woHW_DOOutputSetState(0, s);
}

// IGBT 电源供电
void IGBT_DrvSuplyPowerCtrl(uint8_t s)    //24v
{
    woHW_DOOutputSetState(1, s);
}

// FPGA 复位
void FPGA_Rst(void)
{
    FPGA_RST(low_level);
    DELAY_US(1000);
    FPGA_RST(high_level);
}

// 硬件故障复位
void HW_ERROR_StateRst(void)
{
    woPWM_OutputCtrl = __DISABLE__;     // 封锁PWM脉冲输出;
    woHW_ErrorSignal = __ENABLE__;      // 是否屏蔽急停、所有的硬件故障(包括IGBT硬件故障)所引起的封锁PWM/DO0/DO1输出, 一般用于测试时

    woHW_ErrorStateRst = __RST__;       // 硬件故障复位
}

// 硬件故障复位
void FPGA_PWM_OutputLock(void)
{
    woPWM_OutputCtrl = __DISABLE__;     // 封锁PWM脉冲输出;
    woHW_ErrorSignal = __ENABLE__;      // 是否屏蔽急停、所有的硬件故障(包括IGBT硬件故障)所引起的封锁PWM/DO0/DO1输出, 一般用于测试时
    woHW_IGBT_ErrorSignal = __ENABLE__; // 是否屏蔽IGBT硬件故障信号, 一般用于测试或风机启停时
}

void FPGA_Init(void)
{
    uint16_t i;
    FPGA_Rst();                                 // FPGA 复位

    // DI输入滤波时间常数， 单位 ms
    woHW_DIInputStateDly = 0;
    // 风扇1/风扇2控制占空比输出值，单位0.1%
    woFAN_CtrlPwm1 = 0;
    woFAN_CtrlPwm2 = 0;

    for (i = 0; i < 16; i++)
    {
        woHW_DOOutputSetState(i, 0);            // DO口输出关闭
    }
    IGBT_DrvSuplyPowerCtrl(1);                  // IGBT功率模块驱动上电

    woHW_ErrorSignalBitSet[0] = 0x0000;         // 硬件故障信号00~15有效电平设置
    woHW_ErrorSignalBitSet[1] = 0x0000;         // 硬件故障信号16~17有效电平设置
    woHW_ErrorSignalBitSet[2] = 0x0000;         // IGBT硬件故障1~6有效电平设置，风扇硬件故障1~3有效电平设置

    for (i = 0; i < 3; i++)
    {
     woHW_ErrorSignalBitCtrl[i] = 0xFFFF;       // 硬件故障寄存器n使能设置
    }
    woHW_IGBT_ErrorSignal = __ENABLE__;         // 是否屏蔽IGBT硬件故障信号, 一般用于测试或风机启停时

    HW_ERROR_StateRst(); // 硬件故障复位
}














