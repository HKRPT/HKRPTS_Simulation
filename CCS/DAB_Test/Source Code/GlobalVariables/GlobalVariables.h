/*
* Copyright (c)
* All rights reserved.
*
* 文件名称：GlobalVariables.h
* 文件标识：
* 摘    要：
*
* 当前版本：1.0
* 作    者：shawn.xu
* 完成日期：2016年10月21日
*
* 取代版本：无
* 原作者  ：无
* 完成日期：无
*/
#ifndef __GLOBAL_VARIABLES_H__  // 防止被重定义
#define __GLOBAL_VARIABLES_H__
#include "types.h"

// 每个序号占用4个字节

#define DICT_Iac_Ref_0                0
#define DICT_Iac_Slope                1
#define DICT_Iac_Max                  2
#define DICT_Iac_Kp                   3
#define DICT_Iac_Ki                   4
#define DICT_Iac_Kd                   5
#define DICT_UBus_H_Ref_0             6
#define DICT_UBus_H_Slope             7
#define DICT_UBus_H_Max               8
#define DICT_UBus_H_Min               9
#define DICT_UBus_H_Kp                10
#define DICT_UBus_H_Ki                11
#define DICT_UBus_H_Kd                12
#define DICT_uac_AF_K                 13
#define DICT_Uac_Max                  14
#define DICT_Uac_Min                  15
#define DICT_Ot                       16
#define DICT_ACDC_FullBridge_CtrlMode 17
#define DICT_rSampleTimeMS            18
#define DICT_rRecordTimeLengthMS      19

typedef struct
{
    INT16U n; // 存储字节数
    void *dp; // 数据指针
}E2PROM_DP;

#endif
