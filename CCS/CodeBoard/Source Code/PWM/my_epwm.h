/*
 * my_epwm.h
 *
 *  Created on: 2024年9月19日
 *      Author: BYRD-YHY
 */

#ifndef BSP_PWM_MY_EPWM_H_
#define BSP_PWM_MY_EPWM_H_

#include "main.h"

#define EPWM1       0
#define EPWM2       1
#define EPWM3       2
#define EPWM4       3
#define EPWM5       4
#define EPWM6       5
#define EPWM7       6
#define EPWM8       7

/**
 * port     PWM索引
 * period   PWM 定时器周期
 * compareX PWM 通道比较值
 * deadband PWM 死区设置
 *
 */
void myEPWMx_Init(uint16_t port,uint16_t period, uint16_t compareA, uint16_t compareB,uint16_t deadband,uint16_t PHSEN, uint16_t PhsShift);
void myEPWM_ADC_Init(uint16_t port);
void myEPWM_Init(void);
void DSC_PWM_OuputLock(void);
void DSC_PWM_OutputUnlock(void);
#endif /* BSP_PWM_MY_EPWM_H_ */
