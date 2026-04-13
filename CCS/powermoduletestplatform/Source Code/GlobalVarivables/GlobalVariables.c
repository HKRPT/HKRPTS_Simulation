/*
 * GlobalVariables.c
 *
 *  Created on: 2025年9月23日
 *      Author: YangHaoYan
 */
#include "GlobalVariables.h"


//-----------------------------------------------------------------------------
// Define Global Variables
//-----------------------------------------------------------------------------
// 外部 RAM 测试地址
#ifdef __cplusplus
#pragma DATA_SECTION(rwFPGA_RW_TestFile)
#else
#pragma DATA_SECTION(rwFPGA_RW, "rwFPGA_RW_TestFile");
#endif
uint16_t rwFPGA_RW;


// FPGA 软件版本
#ifdef __cplusplus
#pragma DATA_SECTION(roFPGA_VersionFile)
#else
#pragma DATA_SECTION(roFPGA_Version, "roFPGA_VersionFile");
#endif
uint16_t roFPGA_Version;

// 外置 ADC 采样结果
#ifdef __cplusplus
#pragma DATA_SECTION(roEXADC_ResultFile)
#else
#pragma DATA_SECTION(roEXADC_Result, "roEXADC_ResultFile");
#endif
int16_t roEXADC_Result[18];
float ADC_Ratio = 1.0 * (1/32768.0) * 10.0; // ADC参考电压 -10V~10V(-32768~32768)

// 硬件故障输入
// roHW_ErrorSignalBitState[0].0~roHW_ErrorSignalBitState[0].15, 对应 着ADC通道的 HW_ERROR[0] ~ HW_ERROR[15]
// roHW_ErrorSignalBitState[1].0~roHW_ErrorSignalBitState[1].2,  对应 着ADC通道的 HW_ERROR[16] ~ HW_ERROR[17]
// roHW_ErrorSignalBitState[3].0~roHW_ErrorSignalBitState[3].5,  对应 着ADC通道的 IGBT_ERROR[0] ~ IGBT_ERROR[5]
// roHW_ErrorSignalBitState[3].6~roHW_ErrorSignalBitState[3].8,  对应 着ADC通道的 FAN_ERROR[0] ~ FAN_ERROR[2]
#ifdef __cplusplus
#pragma DATA_SECTION(roHW_ErrorSignalBitStateFile)
#else
#pragma DATA_SECTION(roHW_ErrorSignalBitState, "roHW_ErrorSignalBitStateFile");
#endif
uint16_t roHW_ErrorSignalBitState[3];

// DI输入
#ifdef __cplusplus
#pragma DATA_SECTION(roHW_DIInputStateFile)
#else
#pragma DATA_SECTION(roHW_DIInputState, "roHW_DIInputStateFile");
#endif
uint16_t roHW_DIInputState;

// DI输入滤波时间常数， 单位 ms
#ifdef __cplusplus
#pragma DATA_SECTION(woHW_DIInputStateDlyFlie)
#else
#pragma DATA_SECTION(woHW_DIInputStateDly, "woHW_DIInputStateDlyFlie");
#endif
uint16_t woHW_DIInputStateDly = 0;

// 硬件故障复位，写入0x5AA5复位
#ifdef __cplusplus
#pragma DATA_SECTION(woHW_ErrorStateRstFile)
#else
#pragma DATA_SECTION(woHW_ErrorStateRst, "woHW_ErrorStateRstFile");
#endif
uint16_t woHW_ErrorStateRst;

// PWM输出使能，写入0x5AA5使能
// 故障复位后，PWM使能端维持故障前信号，所以在故障复位前需要先DSP向FPAG发送IGBT不使能信息
#ifdef __cplusplus
#pragma DATA_SECTION(woPWM_OutputCtrlFile)
#else
#pragma DATA_SECTION(woPWM_OutputCtrl, "woPWM_OutputCtrlFile");
#endif
uint16_t woPWM_OutputCtrl;

// 风扇1控制占空比输出值，单位0.1%
#ifdef __cplusplus
#pragma DATA_SECTION(woFAN_CtrlPwm1File)
#else
#pragma DATA_SECTION(woFAN_CtrlPwm1, "woFAN_CtrlPwm1File");
#endif
uint16_t woFAN_CtrlPwm1;

// 风扇2控制占空比输出值，单位0.1%
#ifdef __cplusplus
#pragma DATA_SECTION(woFAN_CtrlPwm2File)
#else
#pragma DATA_SECTION(woFAN_CtrlPwm2, "woFAN_CtrlPwm2File");
#endif
uint16_t woFAN_CtrlPwm2;

// DO输出
#ifdef __cplusplus
#pragma DATA_SECTION(woHW_DOOutputStateFile)
#else
#pragma DATA_SECTION(woHW_DOOutputState, "woHW_DOOutputStateFile");
#endif
uint16_t woHW_DOOutputState;

// 写入0x5AA5使能，写入0xXXXXX不使能；
// 屏蔽急停、所有的硬件故障(包括IGBT硬件故障)所引起的封锁PWM/DO0/DO1输出, 一般用于测试时
#ifdef __cplusplus
#pragma DATA_SECTION(woHW_ErrorSignalFile)
#else
#pragma DATA_SECTION(woHW_ErrorSignal, "woHW_ErrorSignalFile");
#endif
uint16_t woHW_ErrorSignal;

// 硬件故障寄存器有效电平设置，按位分别对应着roHW_ErrorSignalBitState[]
#ifdef __cplusplus
#pragma DATA_SECTION(woHW_ErrorSignalBitSetFile)
#else
#pragma DATA_SECTION(woHW_ErrorSignalBitSet, "woHW_ErrorSignalBitSetFile");
#endif
uint16_t woHW_ErrorSignalBitSet[3];

// 写入0x5AA5使能，写入0xXXXXX不使能；
// 屏蔽IGBT硬件故信号
#ifdef __cplusplus
#pragma DATA_SECTION(woHW_IGBT_ErrorSignalFile)
#else
#pragma DATA_SECTION(woHW_IGBT_ErrorSignal, "woHW_IGBT_ErrorSignalFile");
#endif
uint16_t woHW_IGBT_ErrorSignal;

// 硬件故障寄存器使能设置，按位分别对应着roHW_ErrorState[]
#ifdef __cplusplus
#pragma DATA_SECTION(woHW_ErrorSignalBitCtrlFile)
#else
#pragma DATA_SECTION(woHW_ErrorSignalBitCtrl, "woHW_ErrorSignalBitCtrlFile");
#endif
uint16_t woHW_ErrorSignalBitCtrl[3];

// NTC计数值
#ifdef __cplusplus
#pragma DATA_SECTION(rdNTC_CNTFile)
#else
#pragma DATA_SECTION(rdNTC_CNT, "rdNTC_CNTFile");
#endif
uint16_t rdNTC_CNT[6];
