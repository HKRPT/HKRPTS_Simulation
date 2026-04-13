/*
 * recoder.h
 *
 *  Created on: 2025年10月28日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_CONTORL_RECODER_H_
#define SOURCE_CODE_CONTORL_RECODER_H_

#include "main.h"
#include "modbus.h"
#include "svpwm.h"
#include "mbcrc.h"
#include "my_types.h"

typedef enum
{
    RECORD_INPUT_VOLTAGE,       // 记录交直输入电压
    RECORD_OUPUT_CURRENT,       // 记录交流输出电流
    RECORD_OUPUT_VOLTAGE,       // 记录交流输出电压
    RECORD_IGBT_TEMPERATURE,    // 记录功率模块温度
    RECORD_STATE_RUNNING,       // 记录运行状态
    RECORD_STATE_AVG_RMS,       // 记录平均值及有效值
    RECORD_DEFAULT
}SYSTEM_RECORD;


#define MAX_CHANNEL         7
#define RING_BUF_SIZE       512

typedef struct
{
    volatile uint16_t head;   // 写入位置（中断更新）
    volatile uint16_t tail;   // 读取位置（主循环更新）
    volatile uint8_t  count;  // 当前有效数据个数（0 ~ RING_BUF_SIZE）
    float buffer[MAX_CHANNEL][RING_BUF_SIZE];  // 注意：[通道][时间索引]
}RingBuffer_t;


typedef enum
{
    SYS_StatePTR,           // 系统状态
    VOL1_IN_PTR,            // 电压输入
    VOL2_IN_PTR,            // 电压输入
    VOL3_IN_PTR,            // 电压输入
    VOL1_OUT_PTR,           // 电压输出
    VOL2_OUT_PTR,           // 电压输出
    VOL3_OUT_PTR,           // 电压输出
    CUR1_OUT_PTR,           // 电流输入
    CUR2_OUT_PTR,           // 电流输入
    CUR3_OUT_PTR,           // 电流输入
    Park_D_PTR,             // Park 变换d轴
    Park_Q_PTR,             // Park 变换q轴
    UBUS_PTR,               // 直流母线电压
    IBUS_PTR,               // 直流母线电流
    CUR_Leakage,            // 直流尖峰电压
    NTC_1_PTR,              // NTC1
    NTC_2_PTR,              // NTC2
    NTC_3_PTR,              // NTC3
    NTC_4_PTR,              // NTC4
    NTC_5_PTR,              // NTC5
    NTC_6_PTR,              // NTC6
    PT100_1_PTR,            // PT100
    PT100_2_PTR,            // PT100
    PT100_3_PTR,            // PT100
    PT100_4_PTR,            // PT100
    PT100_5_PTR,            // PT100
    PT100_6_PTR,            // PT100
    PM_NTC_1,               // 功率模块NTC
    PM_NTC_2,               // 功率模块NTC
    PM_NTC_3,               // 功率模块NTC
    PM_CUR_1,               // 功率模块电流
    PM_CUR_2,               // 功率模块电流
    PM_CUR_3,               // 功率模块电流
    PM_VOL_1,               // 功率模块电压
    PM_VOL_2,               // 功率模块电压
    PM_VOL_3,               // 功率模块电压
    NOR_ADC_1,              // 普通ADC
    NOR_ADC_2,              // 普通ADC
    NOR_ADC_3,              // 普通ADC
    NOR_ADC_4,              // 普通ADC
    NOR_ADC_5,              // 普通ADC
    NOR_ADC_6,              // 普通ADC

}RecoderPtr;


void timeSampleHandle(void);
void Recode_loop(void);


#endif /* SOURCE_CODE_CONTORL_RECODER_H_ */
