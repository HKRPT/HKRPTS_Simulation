/*
 * dido.h
 *
 *  Created on: 2025年9月18日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_DIDO_DIDO_H_
#define SOURCE_CODE_DIDO_DIDO_H_

#include "main.h"


// DI 信号轮询
typedef struct
{
    uint16_t    DI[16];
}dido_contorl_t;

extern dido_contorl_t  dido_contorl;
void readDiValue(void);
// DO控制
void writeDoReg(uint16_t addr, uint16_t data);
// DO 测试
void DO_Test(void);
#endif /* SOURCE_CODE_DIDO_DIDO_H_ */
