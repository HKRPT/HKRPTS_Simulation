/*
 * powerModule.h
 *
 *  Created on: 2025年11月3日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_CONTORL_POWERMODULE_H_
#define SOURCE_CODE_CONTORL_POWERMODULE_H_

#include "main.h"

// 华为电源上下电
void Huawei_Power(uint8_t status);
void Huawei_Control(void);
// 华为电源 设置电压
void Huawei_SetVoltage(uint32_t value);
// 华为电源 设置电流
void Huawei_SetCurrent(uint32_t value);
// 华为电源 读取电流
void Huawei_ReadCurrent(void);
// 华为电源 读取电压
void Huawei_ReadVoltage(void);


#endif /* SOURCE_CODE_CONTORL_POWERMODULE_H_ */
