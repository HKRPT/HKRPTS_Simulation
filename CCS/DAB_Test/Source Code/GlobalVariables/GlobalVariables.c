//###########################################################################
//
// FILE	GlobalVariables.c
//
// TITLE	Global Variables XDATA Section Pragmas.
//
//###########################################################################
#include "GlobalVariables.h"
//#include "eeprom.h"

//-----------------------------------------------------------------------------
// Define Global Variables
//-----------------------------------------------------------------------------
//#pragma DATA_SECTION(roFPGA_Version, "roFPGA_VersionFile");
//INT16U roFPGA_Version;


extern FP32 Iac_Ref[];
extern FP32 Iac_Slope;
extern FP32 Iac_Max;
extern FP32 Iac_Kp;
extern FP32 Iac_Ki;
extern FP32 Iac_Kd;
extern FP32 UBus_H_Ref[];
extern FP32 UBus_H_Slope;
extern FP32 UBus_H_Max;
extern FP32 UBus_H_Min;
extern FP32 UBus_H_Kp;
extern FP32 UBus_H_Ki;
extern FP32 UBus_H_Kd;
extern FP32 uac_AF_K;
extern FP32 Uac_Max;
extern FP32 Uac_Min;
extern FP32 Ot;
extern FP32 ACDC_FullBridge_CtrlMode;
extern FP32 rSampleTimeMS;
extern FP32 rRecordTimeLengthMS;

E2PROM_DP const rDP[] =
{
    4, &Iac_Ref[0],  // 按4字节进行存储，浮点是4字节，整型是4字节，字是2字节，并获取地址
    4, &Iac_Slope,
    4, &Iac_Max,
    4, &Iac_Kp,
    4, &Iac_Ki,
    4, &Iac_Kd,
    4, &UBus_H_Ref,
    4, &UBus_H_Slope,
    4, &UBus_H_Max,
    4, &UBus_H_Min,
    4, &UBus_H_Kp,
    4, &UBus_H_Ki,
    4, &UBus_H_Kd,
    4, &uac_AF_K,
    4, &Uac_Max,
    4, &Uac_Min,
    4, &Ot,
    2, &ACDC_FullBridge_CtrlMode,
    4, &rSampleTimeMS,
    4, &rRecordTimeLengthMS
};

void DataStorage_Read(INT16U s)
{
    INT16U *dp;
    INT16U n;

    INT16U addr = s << 2; // 数据在E2PROM的起始地址，位置X4，因为没个数据以4字节进行存储

    n = rDP[s].n;
    dp = rDP[s].dp;

    do
    {
        *dp = DataStorage_ReadUint16(addr);
        dp++;
        addr = addr + 2;

        n = n - 2;
    } while (n != 0);
}

void DataStorage_Write(INT16U s)
{
    INT16U *dp;
    INT16U n;

    INT16U addr = s << 2; // 数据在E2PROM的起始地址，位置X4，因为没个数据以4字节进行存储

    n = rDP[s].n;
    dp = rDP[s].dp;

    do
    {
        DataStorage_WriteUint16(addr, *dp);
        dp++;
        addr = addr + 2;

        n = n - 2;
    } while (n != 0);
}

void InitGlobalvariables(void)
{

}

//===========================================================================
// End of file.
//===========================================================================
