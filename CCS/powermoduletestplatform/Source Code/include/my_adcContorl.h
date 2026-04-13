/*
 * my_adcContorl.h
 *
 *  Created on: 2025年9月19日
 *      Author: YangHaoYan
 */
#ifndef __MY_ADCCONTORL_H
#define __MY_ADCCONTORL_H


#include "main.h"

#define ADCSampleTimes      10
#define ADC_NUM             15
#define ADC_EX_NUM          24
typedef enum{
    PT100_1,
    PT100_2,
    PT100_3,
    PT100_4,
    PT100_5,
    PT100_6,
    Voltage_3V_1,
    Voltage_GND_1,

    u_NTC_U,
    u_NTC_V,
    u_NTC_W,
    u_NTC_U1,
    u_NTC_V1,
    u_NTC_W1,
    Voltage_3V_2,
    Voltage_GND_2,

    General_ADC_1,
    General_ADC_2,
    General_ADC_3,
    General_ADC_4,
    General_ADC_5,
    General_ADC_6,
    General_ADC_7,
    General_ADC_8,

    i_ADC_1,
    i_ADC_2,
    i_ADC_3,
    i_ADC_4,
    i_ADC_5,
    i_ADC_6,

    IGBT_NTC_ADC_U,
    IGBT_NTC_ADC_V,
    IGBT_NTC_ADC_W,

    IGBT_Voltage_ADC_U,
    IGBT_Voltage_ADC_V,
    IGBT_Voltage_ADC_W,

    IGBT_Current_ADC_U,
    IGBT_Current_ADC_V,
    IGBT_Current_ADC_W,

}adc_enum;

typedef struct {
    float       adcValue;                       // ADC 值
}my_adcContorl_t;

typedef struct {
    float NTC_U;
    float NTC_V;
    float NTC_W;
    float NTC_U1;
    float NTC_V1;
    float NTC_W1;
    float Voltage_3V_2;

    float General_ADC_1;
    float General_ADC_2;
    float General_ADC_3;
    float General_ADC_4;
    float General_ADC_5;
    float General_ADC_6;
    float General_ADC_7;
    float General_ADC_8;

    float i_ADC_1;
    float i_ADC_2;
    float i_ADC_3;
    float i_ADC_4;
    float i_ADC_5;
    float i_ADC_6;

    float IGBT_NTC_ADC_U;
    float IGBT_NTC_ADC_V;
    float IGBT_NTC_ADC_W;

    float IGBT_Voltage_ADC_U;
    float IGBT_Voltage_ADC_V;
    float IGBT_Voltage_ADC_W;

    float IGBT_Current_ADC_U;
    float IGBT_Current_ADC_V;
    float IGBT_Current_ADC_W;
}my_adc_t;


typedef struct
{
    float A0;
    float A1;
    float A2;
    float A3;
    float A4;
    float A5;
    float B0;
    float B1;
    float B2;
    float B3;
    float B4;
    float B5;
    float C0;
    float C1;
    float C2;
    float C3;
    float C4;
    float C5;
    float D0;
    float D1;
    float D2;
    float D3;
    float D4;
    float D5;
    float sampleCnt;        // 采样次数
}myADCResults_t;

extern myADCResults_t  ADCRusults;
// NTC 循环读取
extern void NTC_Loop(void);
extern uint8_t NTC_Flag;
#endif

