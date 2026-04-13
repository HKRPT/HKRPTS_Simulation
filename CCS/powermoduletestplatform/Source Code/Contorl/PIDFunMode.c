/*
 * PIDFunMode.c
 *
 *  Created on: 2025年10月15日
 *      Author: YangHaoYan
 */
#include "PIDFunMode.h"

// PID 初始化
void PidCtrlRegInit(PidCtrlReg *ps, float kp, float ki, float kd, float max, float min)
{
    ps->Kp  = kp;
    ps->Ki  = ki;
    ps->Kd  = kd;

    ps->Max = max;
    ps->Min = min;
}

// PID 清除
void PidCtrlRegClear(PidCtrlReg *ps, float input1, float accumulate, int state, float output)
{
    ps->Input[1]    = input1;
    ps->Accumulate  = accumulate;
    ps->State       = state;
    ps->Output      = output;
}

/*
 * 位置式PID控制
 * 比例(P)    积分(I)   微分(D)三项
 * 积分抗饱和：输出达到限幅时，有条件地停止积分累加
 * 微分项使用“微分先行”或误差微分
 * 输出再次限幅，确保最终结果在[min,max]范围内
 *
 * */
// PID 控制循环
float PidCtrlLoop(PidCtrlReg *ps)
{
    float tmp = 0;

    ps->Err = ps->Ref - ps->Input[0];       // 计算当前误差   Err = 设定值 - 当前反馈值

    tmp = (float)ps->Ki * (float)ps->Err;   // 计算积分增量   Ki * error

    if(ps->State == 1)
    {
        if(tmp > 0)
        {
            ;                               // 不累加(防止往正方向积分)
        }
        else
        {
            ps->Accumulate  += tmp;         // 允许反向积分(退出饱和)
        }
    }
    else if(ps->State == -1)
    {
        if(tmp < 0)
        {
            ;                               // 不累加(防止往负方向积分)
        }
        else
        {
            ps->Accumulate += tmp;          // 运行正向积分(退出饱和)
        }
    }
    else
    {
        ps->Accumulate += tmp;              // 正常情况，始终累积
    }

    tmp = (float)ps->Kp * (float)ps->Err + ps->Accumulate;  // 计算PID输出(不含微分)

    if(tmp >= ps->Max)
    {
        tmp = ps->Max;
        ps->State = 1;                      // 标记为上限饱和
    }
    else if(tmp <= ps->Min)
    {
        tmp = ps->Min;
        ps->State = -1;                     // 标记为下限饱和
    }
    else
    {
        ps->State = 0;                      // 正常工作区
    }

    // 对反馈信号积分
    tmp = tmp + ((ps->Input[0] - ps->Input[1]) * ps->Kd);
    ps->Input[1] = ps->Input[0];            // 更新历史值

    if(tmp >= ps->Max){
        tmp = ps->Max;
    }
    else if (tmp <= ps->Min)
    {
        tmp = ps->Min;
    }

    ps->Output = tmp;

    return ps->Output;
}





















