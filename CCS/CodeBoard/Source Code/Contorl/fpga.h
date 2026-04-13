/*
 * fpga.h
 *
 *  Created on: 2025年10月31日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_CONTORL_FPGA_H_
#define SOURCE_CODE_CONTORL_FPGA_H_

#include "main.h"
#include "GlobalVariables.h"

void FanStateCtrl(uint8_t s);

// 硬件故障复位
void HW_ERROR_StateRst(void);
// 硬件故障复位
void FPGA_PWM_OutputLock(void);
void FPGA_Init(void);

#endif /* SOURCE_CODE_CONTORL_FPGA_H_ */
