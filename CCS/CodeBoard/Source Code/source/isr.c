/*
 * isr.c
 *
 *  Created on: 2025年1月26日
 *      Author: BYRD-003
 */
#include "main.h"
#include "powerModule.h"
#include "my_adcContorl.h"


// ADC 中断函数
__interrupt void ADC_ISR(void)
{
    static float isr_time1MiliSecondCnt = 0.0F;
    static float isr_time10MiliSecondCnt = 0.0F;
    static float isr_time1SecondCnt = 0.0F;
    static float isr_time1SecondCnt1 = 0.0F;
    // 中断计时函数
    extern void tmrTask_thread(void);
    extern void W5500_contorlTimeCnt(void);
    extern void Fram_contorlTimeCnt(void);
    extern void FLASH_contorlTimeCnt(void);
    extern void UART_TimerTickMicroSeconds(uint32_t CC);
    extern void Contorl_ISR(void);

    extern void adc_handle(myADCResults_t *result); // ADC 处理函数



    isr_time1MiliSecondCnt  += __FTs;               // ms 自增
    isr_time10MiliSecondCnt += __FTs;               // ms 自增
    isr_time1SecondCnt      += __FTs;               // ms 自增

    if(isr_time1MiliSecondCnt >= 0.001)             // 1ms
    {
        tmrTask_thread();
        readDiValue();                              // 读取IO引脚
        isr_time1MiliSecondCnt = 0;
    }

    if(isr_time10MiliSecondCnt >= 0.01)             // 10ms
    {
        W5500_contorlTimeCnt();
        Fram_contorlTimeCnt();
        FLASH_contorlTimeCnt();
        UART_TimerTickMicroSeconds(0);
        isr_time10MiliSecondCnt = 0;
    }
    if(isr_time1SecondCnt >= 1.0)                     // 1s
    {
        NTC_Flag = !NTC_Flag;           // 1s 读取NTC 一次

        Huawei_ReadCurrent();          // 读取 电流
        Huawei_ReadVoltage();          // 读取 电压

        isr_time1SecondCnt1++;
        isr_time1SecondCnt = 0;
    }

    adc_handle(&ADCRusults);

    Contorl_ISR();

    if(1 == AdcaRegs.ADCINTOVF.bit.ADCINT1)
    {
        AdcaRegs.ADCINTOVFCLR.bit.ADCINT1 = 1;              //clear INT1 overflow flag
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;              //clear INT1 flag
    }

    if(1 == AdcbRegs.ADCINTOVF.bit.ADCINT1)
    {
        AdcbRegs.ADCINTOVFCLR.bit.ADCINT1 = 1;              //clear INT1 overflow flag
        AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;              //clear INT1 flag
    }

    if(1 == AdcbRegs.ADCINTOVF.bit.ADCINT1)
    {
        AdcbRegs.ADCINTOVFCLR.bit.ADCINT1 = 1;              //clear INT1 overflow flag
        AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;              //clear INT1 flag
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

