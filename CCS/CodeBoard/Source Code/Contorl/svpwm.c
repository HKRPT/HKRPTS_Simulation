/*
 * svpwm.c
 *
 *  Created on: 2025年10月14日
 *      Author: YangHaoYan
 */
#include "svpwm.h"
#include "math.h"
#include "recoder.h"
#include "my_epwm.h"
#include "powerModule.h"

uint16_t    Io_SoftStartFlg = 1;        // 输出电流软启标志
uint16_t    Io__SoftStartFlg = 1;       // 输出电流频率软启标志
uint16_t    SoftStartFlg = 1;           // 软启标志

float Io_Ref[2] = {0 , 0};              // 输出电流参考值
float Io_Slope  = 1.0;                  // 输出电流软启_A/s斜率

float Io_Max    = 400.0;                // 最大输出电流
float Io_Kp     = 0.0001;
float Io_Ki     = 0.000001;
float Io_Kd     = 0;
float Io_bd     = 0;                    // 输出电流波动值

float dx        = 0;
float qx        = 0;
float dq2alpha  = 0;
float dq2beta   = 0;

float Io[3]     = {0, 0, 0};            // 输出电流有效值
float io[3]     = {0, 0, 0};            // 输出电流瞬时值
float iox[3]    = {0, 0, 0};            // 输出电流瞬时累加值
float io_d, io_q, io_z = 0;             // d 分量，q 分量
float io_ratio  = 300;                  // 电流变比 ±1200A， ±4V

float Uo[3]     = {0, 0, 0};            // 输出电压有效值
float uo[3]     = {0, 0, 0};            // 输出电压瞬时值
float uox[3]    = {0, 0, 0};            // 输出电压瞬时累加值
float uo_d, uo_q, uo_z = 0;             // d 分量，q 分量
float uo_ratio  = 218.18;               // 电压变比


float Ui[3]     = {0, 0, 0};            // 输入线电压有效值
float Uiz[3]    = {0, 0, 0};            // 输入线电压有效值z-1
float UiMax     = 0;                    // 输入线电压有效值，三相最大的值
float Uixc      = 0.8;                  // Ui 斜率变化
float UiMaxz    = 0;                    // 输入线电压有效值，三相最大的值z-1
float ui[3]     = {0, 0, 0};            // 输入线电压瞬时值
float uix[3]    = {0, 0, 0};            // 输入线电压瞬时值累加值
float ui_d, ui_q, ui_z = 0;             // d 分量， q分量
float uiac_alphas, uiac_betas = 0;      // alpha分量、beta分量
float ui_ratio  = 218.18;               // 电压变比

float UbusRef   = 0;                    // 直流母线电压有效值参考
float Ubus      = 0;                    // 直流母线电压有效值
float Ubusz     = 0;                    // 直流母线电压有效值z-1
float ubus      = 0;                    // 直流母线电压瞬时值
float ubusx     = 0;                    // 直流母线电压瞬时电压累加值
float ubus_ratio= 300.00;               // 电压变比
uint16_t UbusCtrlTimeCnt    = 0;        // 调节时间
uint16_t UbusCtrlTimeCntx   = 0;        // 调节时间
uint16_t UbusCtrlFlg        = 0;        // 直流母线电压调节
uint16_t UbusCtrlock        = 0;        //

float IbusRef   = 0;                    // 直流母线电流参考
float Ibus      = 0;                    // 直流母线有效电流值
float ibus      = 0;                    // 直流母线瞬时电流值
float ibusx     = 0;                    // 直流母线瞬时电流累加值
float ibus_ratio= 50.00;                // 电流变比， 0 ~ 200A, 0~4V

float I_AC_Leakage  = 0;                // 交流漏电流有效值
float i_AC_Leakage  = 0;                // 交流有效值瞬时值
float i_AC_Leakagex = 0;                // 交流漏电流瞬时累加值
float i_AC_Leakage_ratio = 10.0;        // 交流漏电流变比，±50mA, ±5V(DC) 5V(AC)

float I_DC_Leakage  = 0;                // 直流漏电流有效值
float i_DC_Leakage  = 0;                // 直流有效值瞬时值
float i_DC_Leakagex = 0;                // 直流漏电流瞬时累加值
float i_DC_Leakage_ratio = 10.0;        // 直流漏电流变比，±50mA, ±5V(DC) 5V(AC)

float xIGBT_Temperature = 0;            // 功率模块散热器温度
float xIGBT_TemperatureMax = 75.0;      // 最高温度值℃，触发过温保护
float xIGBT_Temperature_ratio = 20.0;   // 温度变比，0~100℃， 0~5V

float iIGBT_Temperature = 0;            // IGBT温度
float iIGBT_TemperatureMax = 75.0;      // 最高温度值℃，触发过温保护
float iIGBT_Temperature_ratio = 20.0;   // 温度变比，-40~150℃，0~5V 38℃/V t = 输出电压*38℃/V -40℃
uint16_t iIGBT_TemperatureFlg = 0;

float theta = 0.0;                      // 输出电流同步相位值
float sin_angle = 0;
float cos_angle = 0;
float SVPWMDuty[3] = {0 ,0 ,0};         // 三相PWM波输出占空比

uint32_t PowerVoltage   = 24;           // 电源模块输出电压
uint32_t PowerCurrent   = 2;            // 电源模块输出电流

uint8_t ManualFanCtrlFlg;

SYSTEM_STATE ControlState   = STATE_IDLE;


// ADC读取
void ADCSampling(int16_t *data)
{
    //  三相输出电流瞬时值
    io[0]   = data[0] * ADC_Ratio * io_ratio - 1.50;        // A相电流传感器有＋1.50A的偏置电流
    io[1]   = data[1] * ADC_Ratio * io_ratio + 2.50;        // B相电流传感器有 -2.25A的偏置电流
    io[2]   = data[2] * ADC_Ratio * io_ratio + 2.50;        // C相电流传感器有 -2.25A的偏置电流

    // 三相输入电压瞬时值（利用三相平衡求出第三项电压）
    ui[0]   = (data[6])  * ADC_Ratio * ui_ratio - 1.42;             // A相电压传感器有+1.50V的偏置电压
    ui[1]   = (-data[7]) * ADC_Ratio * ui_ratio - 0.25;             // B相电压传感器有+0.25V的偏置电压
    ui[2]   = (data[7] - data[6])    * ADC_Ratio * ui_ratio + 1.67; // C相电压传感器有-1.67V的偏置电压

    // 直流母线电压及电流
    ubus    = data[8] * ADC_Ratio * ubus_ratio - 5.60;              // 有+5.60V偏置电压
    ibus    = data[4] * ADC_Ratio * ibus_ratio;

    // 三相输出电压瞬时值
    uo[0]   = (data[9]) * ADC_Ratio * uo_ratio;
    uo[1]   = (-data[10]) * ADC_Ratio * uo_ratio;
    uo[2]   = (data[10] - data[9]) * ADC_Ratio * uo_ratio;

    // 漏电流
    I_AC_Leakage = data[12] * ADC_Ratio * i_AC_Leakage_ratio;
    I_DC_Leakage = data[13] * ADC_Ratio * i_DC_Leakage_ratio;

    xIGBT_Temperature = data[3] * ADC_Ratio * xIGBT_Temperature_ratio;

    if(!iIGBT_TemperatureFlg) iIGBT_TemperatureFlg = 1;
}

uint16_t RMSTimeCnt = 0;
// 有效值计算
void RMSCalc(void)
{
    uint8_t i ;
    float tUiMax = 0;

    // 直流母线电压/电流累加
    ubusx += ubus;
    ibusx += ibusx;

    for(i = 0; i < 3; i++)
    {
        uix[i]  += ui[i] * ui[i];
        uox[i]  += uo[i] * uo[i];
        iox[i]  += io[i] * io[i];
    }

    // 20ms 取均值
    if(++RMSTimeCnt >= 0.02 * __FC)
    {

        Ubus = ubusx/RMSTimeCnt;
        Ibus = ibusx/RMSTimeCnt;

        for(i = 0; i < 3; i++)
        {
            uix[i]  = uix[i] / RMSTimeCnt;
            Ui[i]   = sqrt(uix[i]);

            uox[i]  = uox[i] / RMSTimeCnt;
            Uo[i]   = sqrt(uox[i]);

            iox[i]  = iox[i] / RMSTimeCnt;
            Io[i]   = sqrt(iox[i]);
        }


        // 寻找出有效值最大电压
        for(i = 0; i < 3; i++)
        {
            if(tUiMax <= Ui[i])
            {
                tUiMax = Ui[i];
            }
        }

        // 最大有效值
        UiMax = tUiMax;

        // 清除计算数值
        ubusx = 0;
        ibusx = 0;

        for(i = 0; i < 3; i++)
        {
            uix[i] = 0;
            uox[i] = 0;
            iox[i] = 0;
        }

        RMSTimeCnt = 0;
    }
}

uint32_t LEDTickCounter = 0;
uint32_t errorState = 0;    // 错误状态
/*
 * 灯定义：     前五个灯 为功能        1:IGBT ， 2：散热器/温度 3 ：电压 4：电流 5：控制故障
 *            后三个灯 为报错通道     6:A 7:B 8:C
 * */
void LED_Contorl(uint8_t value)
{
    RunLed1((value >> 0) & 0x01);
    RunLed2((value >> 1) & 0x01);
    RunLed3((value >> 2) & 0x01);
    RunLed4((value >> 3) & 0x01);
    RunLed5((value >> 4) & 0x01);
    RunLed6((value >> 5) & 0x01);
    RunLed7((value >> 6) & 0x01);
    RunLed8((value >> 7) & 0x01);
}

// 运行灯控制
void LED_RunState(SYSTEM_STATE state)
{
    static uint8_t i =  0;
    uint8_t errorIn = 0;
    static uint8_t flowLed = 0;

    // 报错提示
    if(state == STATE_ERROR)
    {
        switch (i)
        {
        case 0:
            errorIn = 0;        // 无错误
            LED_Contorl(0x00);
            LEDTickCounter = 0;
        case 1:
            if(errorState & 0x0E)
            {
                errorIn = 1;    // 有错误
                RunLed1(0);                                 // IGBT故障
                if(errorState & 0x02)      RunLed6(0);      // A
                if(errorState & 0x04)      RunLed7(0);      // B
                if(errorState & 0x08)      RunLed8(0);      // C
            }
            else
            {
                errorIn = 0;    // 无错误
            }
            break;
        case 2:
            if(errorState & 0x70)
            {
                errorIn = 1;    // 有错误
                RunLed2(0);    // 散热器/温度故障
                if(errorState & 0x10)      RunLed6(0);      // A
                if(errorState & 0x20)      RunLed7(0);      // B
                if(errorState & 0x40)      RunLed8(0);      // C
            }
            else
            {
                errorIn = 0;    // 无错误
            }
            break;
        case 3:
            if(errorState & 0x1C00)
            {
                errorIn = 1;    // 有错误
                RunLed3(0);    // 电压故障
                if(errorState & 0x80)      RunLed6(0);      // A
                if(errorState & 0x100)     RunLed7(0);      // B
                if(errorState & 0x200)     RunLed8(0);      // C
            }
            else
            {
                errorIn = 0;    // 无错误
            }
            break;
        case 4:
            if(errorState & 0xE000)
            {
                errorIn = 1;    // 有错误
                RunLed4(0);    // 电流故障
                if(errorState & 0x400)     RunLed6(0);      // A
                if(errorState & 0x800)     RunLed7(0);      // B
                if(errorState & 0x1000)    RunLed8(0);      // C
            }
            else
            {
                errorIn = 0;    // 无错误
            }
            break;
        case 5:
            if(errorState & 0x70000)
            {
                errorIn = 1;    // 有错误
                RunLed5(0);    // 控制故障
                if(errorState & 0x2000)     RunLed6(0);      // A
                if(errorState & 0x3000)     RunLed7(0);      // B
                if(errorState & 0x4000)     RunLed8(0);      // C
            }
            {
                errorIn = 0;    // 无错误
            }
            break;
        default:
            errorIn = 0;        // 无错误
            LEDTickCounter = 0;
            i = 0;
            break;
        }

        // 该循环是否存在错误
        if(errorIn == 1)
        {
            // 1s切换显示
            if(LEDTickCounter++ >= __FT)
            {
                i++;
                LEDTickCounter = 0;
            }
        }
        else
        {
            // 迅速切换显示
            i++;
        }
    }
    else
    {
        // 若为正常运行时，则为流水灯
        if(state == STATE_IDLE)     LEDTickCounter += 2;
        if(state == STATE_RUNNING)  LEDTickCounter += 6;
        if(state == STATE_STOP)     LEDTickCounter += 10;
        if(LEDTickCounter >= __FT)
        {
            flowLed += 1;
            LED_Contorl(flowLed);
            LEDTickCounter = 0;
        }
    }

}

// 计算角度
float PLL_3Phase(float freq, float val)
{
    val += (2 * PI * freq / __FC);      // 0 ~ 2Π，频率分块

    if(val > 2 * PI)
    {
        val -= 2 * PI;
    }
    else if (val < -2 * PI)
    {
        val += 2 * PI;
    }

    return val;
}

PidCtrlReg Io_Ctrl;

// 电流PID控制
float Io_CtrlLoop(float *p_v, float *p_set)
{
    float d;

    Io_Ctrl.Ref         = *p_set;       // 设定值
    Io_Ctrl.Input[0]    = *p_v;         // 当前值
    PidCtrlRegInit(&Io_Ctrl, Io_Kp, Io_Ki, Io_Kd, 1.0, 0);
    d = PidCtrlLoop(&Io_Ctrl);          // PID 控制

    return d;
}

void Io_CtrlLoopInit(float *p_v, float *p_set)
{
    PidCtrlRegInit(&Io_Ctrl, Io_Kp, Io_Ki, Io_Kd, 1.0, 0);
    PidCtrlRegClear(&Io_Ctrl, *p_set, 0.0, 0, 0);
}


// 使用SVPWM算法计算三相EPWM占空比
// 输入参数为 alpha 分量、beta分量、调制比比例
// 输出参数 为 a 相占空比、b相占空比、c相占空比
void SVPWM_Generate(float alpha, float beta, float modratio, \
                    float *rtb_dgain_a, float *rtb_dgain_b, float *rtb_dgain_c)
{
    float Ualpha,Ubeta;
    float ratio;
    uint16_t sector = 0;
    float Va, Vb, Vc;
    float Ta, Tb, Tc;

    Ualpha  = alpha;
    Ubeta   = beta;
    ratio   = modratio;

    // 确认扇区
    sector = 0;
    // Vref坐标系拆分 Vref1为Vbeta Vref2为Vref1顺时针120°  Vref3为Vref1逆时针120°
    Va = ratio * Ubeta;
    Vb = ratio * (-0.5*Ubeta + 0.8660254*Ualpha);
    Vc = ratio * (-0.5*Ubeta - 0.8660254*Ualpha);

    if(Va > 0) sector = 1;
    if(Vb > 0) sector += 2;
    if(Vc > 0) sector += 4;

    Va = ratio * Ubeta;
    Vb = ratio * (0.5*Ubeta + 0.8660254*Ualpha);    // Y = Vb
    Vc = ratio * (0.5*Ubeta - 0.8660254*Ualpha);    // Z = Vc

    if (sector==0) // Sector 0: this is special case for (Ualpha,Ubeta) = (0,0)
    {
        Ta = 0.5;
        Tb = 0.5;
        Tc = 0.5;
    }
    else if (sector==1) // Sector 1: t1=Z and t2=Y (abc ---> Tb,Ta,Tc)
    {
        Tb = 0.5*(1 - Vc -Vb);
        Ta = Tb + Vc;
        Tc = Ta + Vb;
    }
    else if (sector==2)// Sector 2: t1=Y and t2=-X (abc ---> Ta,Tc,Tb)
    {
        Ta = 0.5 *(1 - Vb +Va);
        Tc = Ta + Vb;
        Tb = Tc - Va;
    }
    else if (sector==3) // Sector 3: t1=-Z and t2=X (abc ---> Ta,Tb,Tc)
    {
        Ta = 0.5 *(1 + Vc -Va);
        Tb = Ta - Vc;
        Tc = Tb + Va;
    }
    else if (sector==4) // Sector 4: t1=-X and t2=Z (abc ---> Tc,Tb,Ta)
    {
        Tc =0.5* (1 + Va -Vc);
        Tb = Tc - Va;
        Ta = Tb + Vc;
    }
    else if (sector==5) // Sector 5: t1=X and t2=-Y (abc ---> Tb,Tc,Ta)
    {
        Tb = 0.5*(1 - Va +Vb);
        Tc = Tb + Va;
        Ta = Tc - Vb;
    }
    else if (sector==6) // Sector 6: t1=-Y and t2=-Z (abc ---> Tc,Ta,Tb)
    {
        Tc = 0.5*(1 + Vb +Vc);
        Ta = Tc - Vb;
        Tb = Ta - Vc;
    }

    *rtb_dgain_a = Ta;
    *rtb_dgain_b = Tb;
    *rtb_dgain_c = Tc;
}

extern uint8_t bootMode; // boot模式 ： 0为APP, 1为Boot

// 控制函数
void Contorl_ISR(void)
{
    if(bootMode) return;                                    // 仅在APP上运行
    ADCSampling(&roEXADC_Result[0]);                        // 电流采集


    RMSCalc();                                              // 有效值，平均值计算


    {
        // Clask 变换 ABC to DQ
        float rut2;
        float rut3;
        float rut5;
        float rut6;

        // 电流变换
        rut2 = -0.5 * sin_angle - 0.8660254 * cos_angle;    // -sin(angle - 120)
        rut3 = -rut2 - sin_angle;                           // 利用三相之和为零, c = -a - b;
        rut5 = -0.5 * cos_angle + 0.8660254 * sin_angle;    // cos(angle - 120)
        rut6 = -rut5 - cos_angle;                           // 利用三相之和为零, c = -a - b;

        // DQ 变换
        io_d = 6.666666667E-001F * (io[0] * sin_angle + io[1] * rut2 + io[2] * rut3);
        io_q = 6.666666667E-001F * (io[0] * cos_angle + io[1] * rut5 + io[2] * rut6);
        io_z = 3.333333333E-001F * (io[0] + io[1] + io[2]);
    }

    {
        uint8_t i;

        // 过流保护
        for (i = 0; i < 3; i++)
        {
            if((io[i] > (Io_Max * 1.414)) || (io[i] >= 1100.0)) // 过流保护
            {
                ControlState = STATE_ERROR;
                errorState |= ((uint32_t)1 << ERROR_IN_CTL_DSP);
                if      (i == 0) errorState |= ((uint32_t)1 << ERROR_IN_CUR_A);
                else if (i == 1) errorState |= ((uint32_t)1 << ERROR_IN_CUR_B);
                else if (i == 2) errorState |= ((uint32_t)1 << ERROR_IN_CUR_C);
            }
        }

        // 过温保护
        if(iIGBT_Temperature >= iIGBT_TemperatureMax)
        {
            ControlState = STATE_ERROR;
            errorState |= ((uint32_t)1 << ERROR_IN_CTL_DSP);
            errorState |= ((uint32_t)1 << ERROR_IN_FAN_A);
        }
    }


    // 直流母线电压控制(华为模块) (待写)

    // 运行灯控制
    LED_RunState(ControlState);
    // 风扇控制
    {
        if(xIGBT_Temperature >= 50.0)               // 功率模块温度大于50℃，散热风机启动
        {
            FanStateCtrl(1);
        }
        else if(xIGBT_Temperature <= 45.0)          // 功率模块温度小于45℃，散热风机关闭
        {
            FanStateCtrl(0 || ManualFanCtrlFlg);    // 当在低温时, 可以手动操作风扇启停
        }
    }

    theta = PLL_3Phase(50.0, theta);                // 若为并网，则放置锁相环，计算角度

    sin_angle = sin(theta);
    cos_angle = cos(theta);

    if((ControlState == STATE_RUNNING) || (ControlState == STATE_READY_RUN))
    {
        if(SoftStartFlg)    // 软启
        {
            // 输出电流软启
            if(Io_SoftStartFlg)
            {
                Io_Ref[1] += Io_Slope / __FC;
                if(Io_Ref[1] >= Io_Ref[0])
                {
                    Io_SoftStartFlg = 0;        // 关闭软启
                    Io_Ref[1] = Io_Ref[0];
                }
            }
            else
            {
                Io_Ref[1] = Io_Ref[0];
            }

            // 软启标志
            SoftStartFlg = Io_SoftStartFlg;
        }
        else
        {
            Io_Ref[1] = Io_Ref[0];
        }

        dx = Io_CtrlLoop(&io_q, &Io_Ref[1]);    // 电流闭环控制，输出 0 ~ 1.0
        qx = 0;

        if(dx >= 0.95)  dx = 0.95;
        if(dx <= 0.0)   dx = 0.0;

        // dq互换(q轴为主轴)下的反Park变换
        dq2alpha    =  dx * sin_angle + qx * cos_angle;
        dq2beta     = -dx * cos_angle + qx * sin_angle;

        SVPWM_Generate(dq2alpha, dq2beta, 1.0, &SVPWMDuty[0], &SVPWMDuty[1], &SVPWMDuty[2]);

        // 设置比较值
        {
            EPwm1Regs.CMPA.bit.CMPA = (uint16_t)(SVPWMDuty[0] * TPRD);
            EPwm2Regs.CMPA.bit.CMPA = (uint16_t)(SVPWMDuty[1] * TPRD);
            EPwm3Regs.CMPA.bit.CMPA = (uint16_t)(SVPWMDuty[2] * TPRD);
        }

        if(ControlState == STATE_READY_RUN)
        {
            // 允许DSP PWM输出
            ControlState = STATE_RUNNING;
            DSC_PWM_OutputUnlock();
            Huawei_Power(1);                // 华为模块上电
            Huawei_SetVoltage(PowerVoltage);// 设置模块电压
            Huawei_SetCurrent(PowerCurrent);// 设置模块电流
        }
    }
    else
    {
        woPWM_OutputCtrl    = __DISABLE__;  // 封锁PWM脉冲输出
        // 封闭PWM脉冲输出
        DSC_PWM_OuputLock();
        Huawei_Power(0);                    // 华为模块下电
    }

    timeSampleHandle();
}


// 控制循环
void My_Control_Loop(void)
{
    switch(ControlState)
    {
    case STATE_IDLE:
        HW_ERROR_StateRst();                // 尝试进行故障复位
        ControlState    = STATE_STOP;
        break;
    case STATE_READY:
        Io_Ref[1] = 0;                      // 电流给定值清零
        Io_SoftStartFlg = 1;                // 软起标志

        SoftStartFlg = 1;
        Io_CtrlLoopInit(&io_q, &Io_Ref[1]);

        woPWM_OutputCtrl    = __ENABLE__;   // 允许PWM脉冲输出
        ControlState    = STATE_READY_RUN;
        break;
    case STATE_READY_RUN:
            break;
    case STATE_RUNNING:
            break;
    case STATE_STOP:
            break;
    case STATE_ERROR:
            break;
    case STATE_SYSTEM_RST:
               break;
    default :
        break;
    }
}


















