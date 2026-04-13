/*
 * PIDFunMode.h
 *
 *  Created on: 2025年10月15日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_CONTORL_PIDFUNMODE_H_
#define SOURCE_CODE_CONTORL_PIDFUNMODE_H_

#include "main.h"

typedef struct
{
    int     State;          // 积分饱和状态标志：1 = 上限饱和， -1 = 下限饱和，0=正常
    float   Ref;            // 设定值(目标值)
    float   Input[2];       // Input[0]: 当前反馈值；Input[1]:上一时刻反馈值(用于微分)
    float   Err;
    float   Kp;             // PID参数
    float   Ki;
    float   Accumulate;     // 积分项累加值（积分器状态）
    float   Kd;
    float   Max;            // 输出限幅上下限
    float   Min;
    float   Output;         // 控制输出
}PidCtrlReg;

float PidCtrlLoop(PidCtrlReg *ps);
// PID 初始化
void PidCtrlRegInit(PidCtrlReg *ps, float kp, float ki, float kd, float max, float min);
// PID 清除
void PidCtrlRegClear(PidCtrlReg *ps, float input1, float accumulate, int state, float output);
#endif /* SOURCE_CODE_CONTORL_PIDFUNMODE_H_ */
