/*
 * dido.c
 *
 *  Created on: 2025年9月18日
 *      Author: YangHaoYan
 */
#include "dido.h"

dido_contorl_t  dido_contorl;       // DIDO控制函数

// DI 读取函数
void readDiValue(void)
{
    static uint16_t i = 0;

    switch(i++){
    case 0 :
        dido_contorl.DI[0] = DSP_DI0;
        break;
    case 1 :
        dido_contorl.DI[1] = DSP_DI1;
        break;
    case 2 :
        dido_contorl.DI[2] = DSP_DI2;
        break;
    case 3 :
        dido_contorl.DI[3] = DSP_DI3;
        break;
    case 4 :
        dido_contorl.DI[4] = DSP_DI4;
        break;
    case 5 :
        dido_contorl.DI[5] = DSP_DI5;
        break;
    case 6 :
        dido_contorl.DI[6] = DSP_DI6;
        break;
    case 7 :
        dido_contorl.DI[7] = DSP_DI7;
        break;
    case 8 :
        dido_contorl.DI[8] = DSP_DI8;
        break;
    case 9 :
        dido_contorl.DI[9] = DSP_DI9;
        break;
    case 10 :
        dido_contorl.DI[10] = DSP_DI10;
        break;
    case 11 :
        dido_contorl.DI[11] = DSP_DI11;
        break;
    case 12 :
        dido_contorl.DI[12] = DSP_DI12;
        break;
    case 13 :
        dido_contorl.DI[13] = DSP_DI13;
        break;
    case 14 :
        dido_contorl.DI[14] = DSP_DI14;
        break;
    case 15 :
        dido_contorl.DI[15] = DSP_DI15;
        break;
    default:
        i = 0;
        break;
    }
}

// DO控制
void writeDoReg(uint16_t addr, uint16_t data)
{
    switch(addr)
    {
    case 0:
        DSP_DO0(data);
        break;
    case 1:
        DSP_DO1(data);
        break;
    case 2:
        DSP_DO2(data);
        break;
    case 3:
        DSP_DO3(data);
        break;
    case 4:
        DSP_DO4(data);
        break;
    case 5:
        DSP_DO5(data);
        break;
    case 6:
        DSP_DO6(data);
        break;
    case 7:
        DSP_DO7(data);
        break;
    case 8:
        DSP_DO8(data);
        break;
    case 9:
        DSP_DO9(data);
        break;
    case 10:
        DSP_DO10(data);
        break;
    case 11:
        DSP_DO11(data);
        break;
    case 12:
        DSP_DO12(data);
        break;
    case 13:
        DSP_DO13(data);
        break;
    case 14:
        DSP_DO14(data);
        break;
    case 15:
        DSP_DO15(data);
        break;
    default:
        break;
    }
}

// DO 测试
void DO_Test(void)
{
    static uint8_t t = 0;
    static uint16_t i = 0;
    writeDoReg(i++, t);

    if( i >= 16)
    {
        t = !t;
        i = 0;
    }

}


















