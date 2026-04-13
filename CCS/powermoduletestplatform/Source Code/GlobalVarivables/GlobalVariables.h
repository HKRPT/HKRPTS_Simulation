/*
 * GlobalVariables.h
 *
 *  Created on: 2025Äê9ÔÂ23ÈÕ
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_GLOBALVARIVABLES_GLOBALVARIABLES_H_
#define SOURCE_CODE_GLOBALVARIVABLES_GLOBALVARIABLES_H_

#include "main.h"

extern uint16_t rwFPGA_RW;
extern uint16_t roFPGA_Version;
extern int16_t roEXADC_Result[18];
extern float ADC_Ratio;
extern uint16_t roHW_ErrorSignalBitState[3];
extern uint16_t roHW_DIInputState;
extern uint16_t woHW_DIInputStateDly;
extern uint16_t woHW_ErrorStateRst;
extern uint16_t woPWM_OutputCtrl;
extern uint16_t woFAN_CtrlPwm1;
extern uint16_t woFAN_CtrlPwm2;
extern uint16_t woHW_DOOutputState;
extern uint16_t woHW_ErrorSignal;
extern uint16_t woHW_ErrorSignalBitSet[3];
extern uint16_t woHW_IGBT_ErrorSignal;
extern uint16_t woHW_ErrorSignalBitCtrl[3];
extern uint16_t rdNTC_CNT[6];

#endif /* SOURCE_CODE_GLOBALVARIVABLES_GLOBALVARIABLES_H_ */
