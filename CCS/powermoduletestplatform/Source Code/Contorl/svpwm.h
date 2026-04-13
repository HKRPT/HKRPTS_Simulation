/*
 * svpwm.h
 *
 *  Created on: 2025年10月14日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_CONTORL_SVPWM_H_
#define SOURCE_CODE_CONTORL_SVPWM_H_

#include "main.h"
#include "GlobalVariables.h"
#include "PIDFunMode.h"
#include "fpga.h"



typedef enum
{
    STATE_IDLE = 0,         // 空闲状态, 尝试故障复位,复位后进入停机状态
    STATE_READY,            // 待机状态，进入运行前的准备工作
    STATE_READY_RUN,        // 从待机状态切换启动状态的瞬间，此状态只能在中断里切换
    STATE_RUNNING,          // 运行状态
    STATE_STOP,             // 停机状态
    STATE_ERROR,            // 故障状态
    STATE_SYSTEM_RST,       // 系统复位状态
    STATE_SYSTEM_DEFAULT    // 恢复出厂设置状态
}SYSTEM_STATE;

typedef enum
{
    ERROR_IN_None,          // 无报错
    ERROR_IN_IGBT_A,        // A相IGBT报错
    ERROR_IN_IGBT_B,        // B相IGBT报错
    ERROR_IN_IGBT_C,        // C相IGBT报错
    ERROR_IN_FAN_A,         // 散热器A 报错
    ERROR_IN_FAN_B,         // 散热器B 报错
    ERROR_IN_FAN_C,         // 散热器C 报错
    ERROR_IN_Temp_A,        // 温度A  报错
    ERROR_IN_Temp_B,        // 温度B  报错
    ERROR_IN_Temp_C,        // 温度C  报错
    ERROR_IN_VOL_A,         // 电压A  报错
    ERROR_IN_VOL_B,         // 电压B  报错
    ERROR_IN_VOL_C,         // 电压C  报错
    ERROR_IN_CUR_A,         // 电流A  报错
    ERROR_IN_CUR_B,         // 电流B  报错
    ERROR_IN_CUR_C,         // 电流C  报错
    ERROR_IN_COM_FPGA,      // FPGA 通讯报错
    ERROR_IN_CTL_DSP,       // DSP  控制报错
}Error_STATE;
extern uint32_t errorState;

// 控制循环
void My_Control_Loop(void);

#endif /* SOURCE_CODE_CONTORL_SVPWM_H_ */
