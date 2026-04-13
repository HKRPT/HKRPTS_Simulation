/*
 * my_adcContorl.c
 *
 *  Created on: 2025年9月19日
 *      Author: YangHaoYan
 */
#include "my_adcContorl.h"
#include <math.h> // 需要包含math.h以使用log函数
#include "GlobalVariables.h"
#include "my_adcContorl.h"


my_adcContorl_t my_adcContorl[ADC_NUM + ADC_EX_NUM];
myADCResults_t  ADCRusults;

float BX = 3950;                    // B值
float T25 = 298.15;                 // 25℃ 开尔文系数
float R25 = 10E3;                   // 25℃ 100K
const float Rb = 4.7E5;             // NTC 并联电阻

/*
 * 打拍延迟采集信号
 *
 * sw_value1 ： PT100
 * sw_value2 ： NTC
 * sw_value3 ： General_ADC
 */
void My_adcSWContorl(float sw_value1)
{
    static uint16_t i = 0, j = 0;
    switch(j++){
    case 0 :
        // 控制ADC模拟开关切换
        if(i >= 8) i = 0;
        DSP_ADCAnalog_A0(i & 0x01);
        DSP_ADCAnalog_A1(i & 0x02);
        DSP_ADCAnalog_A2(i & 0x04);
        i++;
        break;
    case 1 :
        // 对模拟开关信号进行采集
        // 先进入缓存区
        switch (i - 1)
        {
        case 0:
            my_adcContorl[PT100_1].adcValue         = ((sw_value1 * 131.75) - 100) /0.385;
            break;
        case 1:
            my_adcContorl[PT100_2].adcValue         = ((sw_value1 * 131.75) - 100) /0.385;
            break;
        case 2:
            my_adcContorl[PT100_3].adcValue         = ((sw_value1 * 131.75) - 100) /0.385;
            break;
        case 3:
            my_adcContorl[PT100_4].adcValue         = ((sw_value1 * 131.75) - 100) /0.385;
            break;
        case 4:
            my_adcContorl[PT100_5].adcValue         = ((sw_value1 * 131.75) - 100) /0.385;
            break;
        case 5:
            my_adcContorl[PT100_6].adcValue         = ((sw_value1 * 131.75) - 100) /0.385;
            break;
        case 6:
            my_adcContorl[Voltage_3V_1].adcValue    = sw_value1;
            break;
        case 7:
            my_adcContorl[Voltage_GND_1].adcValue   = sw_value1;
            break;
        default:
            break;
        }
        break;
    default:
        j = 0;
        break;
    }
}

/*
 * 主ADC 采集
 *
 *  i_ADC_1,
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
 *
 * */
void ADC_SampleVal(float NTC_U, float NTC_V, float NTC_W, float V_U, float V_V, float V_W,
                   float C_U, float C_V, float C_W)
{
    my_adcContorl[IGBT_NTC_ADC_U].adcValue          = NTC_U;
    my_adcContorl[IGBT_NTC_ADC_V].adcValue          = NTC_V;
    my_adcContorl[IGBT_NTC_ADC_W].adcValue          = NTC_W;
    my_adcContorl[IGBT_Voltage_ADC_U].adcValue      = V_U;
    my_adcContorl[IGBT_Voltage_ADC_V].adcValue      = V_V;
    my_adcContorl[IGBT_Voltage_ADC_W].adcValue      = V_W;
    my_adcContorl[IGBT_Current_ADC_U].adcValue      = C_U;
    my_adcContorl[IGBT_Current_ADC_V].adcValue      = C_V;
    my_adcContorl[IGBT_Current_ADC_W].adcValue      = C_W;
}


/**
 * @brief 将NTC热敏电阻分压电路的ADC测得电压值转换为温度值
 *
 * @param voltage_mv ADC转换后的真实电压值（单位：毫伏, mV）
 * @param ntc_nominal_r_ohm NTC在25°C (298.15K)下的标称阻值 (单位：欧姆, Ω)，常见为10000 (10kΩ)
 * @param ntc_b_value NTC的B值 (热敏指数，单位：开尔文K)，常见为3950
 * @param ref_resistor_ohm 分压电路中与NTC串联的参考电阻阻值 (单位：欧姆, Ω)，常见为10000 (10kΩ)
 * @param vcc_mv 分压电路的供电电压 (单位：毫伏, mV)，常见为3300 (3.3V)
 * @param ratio  运放放大倍率
 * @return float 计算得到的温度值 (单位：摄氏度, °C)，如果输入电压无效（如0或等于Vcc），返回-273.15（接近绝对零度，表示错误）
 */
float myADCNTCValToTempature(float voltage_mv,
                             float ntc_nominal_r_ohm,
                             float ntc_b_value,
                             float ref_resistor_ohm,
                             float vcc_mv,
                             float ratio)
{
    // 输入电压有效性检查
    if (voltage_mv <= 0.0f || voltage_mv >= vcc_mv)
    {
        // 电压为0或等于Vcc可能意味着NTC开路或短路，返回一个极低的错误值
        return -273.15f;
    }
    // 得出运放前电压
    voltage_mv /= ratio;
    // 1. 根据分压原理计算NTC当前的实际阻值 (R_ntc)
    //  V_ntc = Vcc * (R_ntc / (R_ntc + R_ref))
    //  推导得: R_ntc = (V_ntc * R_ref) / (Vcc - V_ntc)
    float r_ntc = (voltage_mv * ref_resistor_ohm) / (vcc_mv - voltage_mv);

    // 2. 使用NTC的Steinhart-Hart方程（简化版，使用B值）计算温度
    //    1/T = 1/T0 + (1/B) * ln(R/R0)
    //    其中 T0 = 298.15K (25°C), R0 = ntc_nominal_r_ohm
    const float T0 = 298.15f; // 25°C in Kelvin
    float ln_r_ratio = logf(r_ntc / ntc_nominal_r_ohm); // ln(R/R0)
    float inv_temp_k = (1.0f / T0) + (ln_r_ratio / ntc_b_value); // 1/T
    float temp_k = 1.0f / inv_temp_k; // T in Kelvin

    // 3. 将开尔文温度转换为摄氏度
    float temp_c = temp_k - 273.15f;

    return temp_c;
}

// ADC 处理函数
void adc_handle(myADCResults_t *result)
{
    static uint16_t i = 0;
    extern float NTC_ResValue(uint16_t TH);

    switch(i)
    {
    case 0:
        result->A0 += AdcaResultRegs.ADCRESULT0;
        result->A1 += AdcaResultRegs.ADCRESULT1;
        result->A2 += AdcaResultRegs.ADCRESULT2;
        result->A3 += AdcaResultRegs.ADCRESULT3;
        result->A4 += AdcaResultRegs.ADCRESULT3;
        result->A5 += AdcaResultRegs.ADCRESULT3;
        result->B0 += AdcbResultRegs.ADCRESULT0;
        result->B1 += AdcbResultRegs.ADCRESULT1;
        result->B2 += AdcbResultRegs.ADCRESULT2;
        result->B3 += AdcbResultRegs.ADCRESULT3;
        result->B4 += AdcbResultRegs.ADCRESULT3;
        result->B5 += AdcbResultRegs.ADCRESULT3;
        result->C0 += AdccResultRegs.ADCRESULT0;
        result->C1 += AdccResultRegs.ADCRESULT1;
        result->C2 += AdccResultRegs.ADCRESULT2;
        result->C3 += AdccResultRegs.ADCRESULT3;
        result->C4 += AdccResultRegs.ADCRESULT3;
        result->C5 += AdccResultRegs.ADCRESULT3;
        result->D0 += AdcdResultRegs.ADCRESULT0;
        result->D1 += AdcdResultRegs.ADCRESULT1;
        result->D2 += AdcdResultRegs.ADCRESULT2;
        result->D3 += AdcdResultRegs.ADCRESULT3;
        result->D4 += AdcdResultRegs.ADCRESULT3;
        result->D5 += AdcdResultRegs.ADCRESULT3;

        result->sampleCnt += 1.0F;
        if(result->sampleCnt >= 2) i++;        // 采集次数到达20后，数据进行处理
        break;
    case 1:
        // 数据滤波处理
        result->A0 = result->A0 / result->sampleCnt;
        result->A1 = result->A1 / result->sampleCnt;
        result->A2 = result->A2 / result->sampleCnt;
        result->A3 = result->A3 / result->sampleCnt;
        result->A4 = result->A4 / result->sampleCnt;
        result->A5 = result->A5 / result->sampleCnt;
        result->B0 = result->B0 / result->sampleCnt;
        result->B1 = result->B1 / result->sampleCnt;
        result->B2 = result->B2 / result->sampleCnt;
        result->B3 = result->B3 / result->sampleCnt;
        result->B4 = result->B4 / result->sampleCnt;
        result->B5 = result->B5 / result->sampleCnt;
        result->C0 = result->C0 / result->sampleCnt;
        result->C1 = result->C1 / result->sampleCnt;
        result->C2 = result->C2 / result->sampleCnt;
        result->C3 = result->C3 / result->sampleCnt;
        result->C4 = result->C4 / result->sampleCnt;
        result->C5 = result->C5 / result->sampleCnt;
        result->D0 = result->D0 / result->sampleCnt;
        result->D1 = result->D1 / result->sampleCnt;
        result->D2 = result->D2 / result->sampleCnt;
        result->D3 = result->D3 / result->sampleCnt;
        result->D4 = result->D4 / result->sampleCnt;
        result->D5 = result->D5 / result->sampleCnt;
        result->sampleCnt = 0.0F;
        i++;
        break;
    case 2:
        // 数据处理
        result->A0 = result->A0 * 0.0048828125;         // U ADC 电压
        result->A1 = result->A1 * 0.0048828125;         // V ADC 电压
        result->A2 = result->A2 * 0.0048828125;         // W ADC 电压
        result->A3 = result->A3 * 0.0048828125;         // U ADC 电流
        result->A4 = result->A4 * 0.0048828125;         // V ADC 电流
        result->A5 = result->A5 * 0.0048828125;         // W ADC 电流
        result->B0 = result->B0 * 0.0022194602272727;   // U ADC NTC
        result->B1 = result->B1 * 0.0022194602272727;   // V ADC NTC
        result->B2 = result->B2 * 0.0022194602272727;   // W ADC NTC
        result->B3 = result->B3 * 0.000732421875;       // ADC PT100

        i++;
        break;
    case 3:
        // 数据赋值
        ADC_SampleVal(result->B0, result->B1, result->B2, result->A0, result->A1, result->A2,
                      result->A3, result->A4, result->A5);
        My_adcSWContorl(result->B3);
        my_adcContorl[IGBT_NTC_ADC_U].adcValue      = result->B0;
        my_adcContorl[IGBT_NTC_ADC_V].adcValue      = result->B1;
        my_adcContorl[IGBT_NTC_ADC_W].adcValue      = result->B2;
        my_adcContorl[IGBT_Voltage_ADC_U].adcValue  = result->A0;
        my_adcContorl[IGBT_Voltage_ADC_V].adcValue  = result->A1;
        my_adcContorl[IGBT_Voltage_ADC_W].adcValue  = result->A2;
        my_adcContorl[IGBT_Current_ADC_U].adcValue  = result->A3;
        my_adcContorl[IGBT_Current_ADC_V].adcValue  = result->A4;
        my_adcContorl[IGBT_Current_ADC_W].adcValue  = result->A5;
        my_adcContorl[General_ADC_1].adcValue       = result->B4;
        my_adcContorl[General_ADC_2].adcValue       = result->B5;
        my_adcContorl[General_ADC_3].adcValue       = result->D0;
        my_adcContorl[General_ADC_4].adcValue       = result->D1;
        my_adcContorl[General_ADC_5].adcValue       = result->D3;
        my_adcContorl[General_ADC_6].adcValue       = result->C0;
        my_adcContorl[General_ADC_7].adcValue       = result->C1;
        my_adcContorl[General_ADC_8].adcValue       = result->C2;
        my_adcContorl[i_ADC_1].adcValue             = result->C3;

        i++;
        break;
    case 4:
        // 数据清零
        result->A0 = 0.0F;
        result->A1 = 0.0F;
        result->A2 = 0.0F;
        result->A3 = 0.0F;
        result->A4 = 0.0F;
        result->A5 = 0.0F;
        result->B0 = 0.0F;
        result->B1 = 0.0F;
        result->B2 = 0.0F;
        result->B3 = 0.0F;
        result->B4 = 0.0F;
        result->B5 = 0.0F;
        result->C0 = 0.0F;
        result->C1 = 0.0F;
        result->C2 = 0.0F;
        result->C3 = 0.0F;
        result->C4 = 0.0F;
        result->C5 = 0.0F;
        result->D0 = 0.0F;
        result->D1 = 0.0F;
        result->D2 = 0.0F;
        result->D3 = 0.0F;
        result->D4 = 0.0F;
        result->D5 = 0.0F;
        result->sampleCnt = 0.0F;
        i = 0;
    default:
        i = 0;
        break;
    }
}


float BSP_Get_Temperature(float Rntc)
{
    float N1,N2,N3,N4;

    N1 = (log(Rntc)-log(R25))/BX;
    N2 = 1/T25 + N1;
    N3 = 1/N2;
    N4 = N3 - 273.15;
    return N4;
}



// NTC 阻值
float NTC_ResValue(uint16_t TH)
{
    float Ra, Ra1 = 0.0F;

    Ra = (((float)TH / 100000.0) - 0.00715) / (0.692 * 1E-6);

    Ra1 = (Rb * Ra) / (Rb - Ra);

    return BSP_Get_Temperature(Ra1);
}

// 1s 读取一次
uint8_t NTC_Flag = 0x00;
// NTC 循环读取
void NTC_Loop(void)
{
    if(NTC_Flag == 0x01) return;
    // 板载 NTC
    my_adcContorl[u_NTC_U].adcValue             = NTC_ResValue(rdNTC_CNT[0]);
    my_adcContorl[u_NTC_V].adcValue             = NTC_ResValue(rdNTC_CNT[1]);
    my_adcContorl[u_NTC_W].adcValue             = NTC_ResValue(rdNTC_CNT[2]);
    my_adcContorl[u_NTC_U1].adcValue            = NTC_ResValue(rdNTC_CNT[3]);
    my_adcContorl[u_NTC_V1].adcValue            = NTC_ResValue(rdNTC_CNT[4]);
    my_adcContorl[u_NTC_W1].adcValue            = NTC_ResValue(rdNTC_CNT[5]);
}





