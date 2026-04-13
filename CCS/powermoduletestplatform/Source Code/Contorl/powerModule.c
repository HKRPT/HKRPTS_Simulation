/*
 * powerModule.c
 *
 *  Created on: 2025年11月3日
 *      Author: YangHaoYan
 */
#include "powerModule.h"
#include "my_can.h"

uint8_t sendbuf[8] = {0};
uint32_t    huaweiVoltage       = 0;
uint32_t    huaweiCurrent       = 0;
uint32_t    RDhuaweiVoltage     = 0;
uint32_t    RDhuaweiCurrent     = 0;
uint32_t    huaweiPowerState    = 0;            // 电源状态 1为上电状态
uint32_t    huaweiPowerVoltage  = 200;          // 电压状态 1为上电状态
uint32_t    huaweiPowerCurrent  = 50;           // 电流状态 1为上电状态

// 华为电源上下电
void Huawei_Power(uint8_t status)
{
    extern uint16_t CAN_SetEXFrame(uint16_t port ,uint32_t ID,uint8_t *buff);

    uint16_t voltage = 0;
    uint16_t current = 0;

    huaweiPowerState = status;
    voltage = huaweiPowerVoltage;
    current = huaweiPowerCurrent;

    status = !status;       // 华为电源端 1为关机 0 为开机

    voltage *= 10;          // 电压乘以倍数
    current *= 10;          // 电流乘以倍数

    sendbuf[0] = 0x01;
    sendbuf[1] = 0x08;
    sendbuf[2] = 0x00;
    sendbuf[3] = status & 0x01;
    sendbuf[4] = ((uint32_t)current >> 8)  & 0xFF;
    sendbuf[5] = ((uint32_t)current >> 0)  & 0xFF;
    sendbuf[6] = ((uint32_t)voltage >> 8)  & 0xFF;
    sendbuf[7] = ((uint32_t)voltage >> 0)  & 0xFF;

    CAN_SetEXFrame(USE_CANA, 0x068180FE, sendbuf);
}



// 华为电源 设置电压
void Huawei_SetVoltage(uint32_t value)
{
    extern uint16_t CAN_SetEXFrame(uint16_t port ,uint32_t ID,uint8_t *buff);
    huaweiPowerVoltage = value;
    if(huaweiPowerState == 0) return ; // 未上电，不进行操作

    sendbuf[0] = 0x01;
    sendbuf[1] = 0x32;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = (value >> 24)  & 0xFF;
    sendbuf[5] = (value >> 16)  & 0xFF;
    sendbuf[6] = (value >> 8)   & 0xFF;
    sendbuf[7] = (value >> 0)   & 0xFF;

    CAN_SetEXFrame(USE_CANA, 0x068180FE, sendbuf);
}

// 华为电源 设置电流
void Huawei_SetCurrent(uint32_t value)
{
    extern uint16_t CAN_SetEXFrame(uint16_t port ,uint32_t ID,uint8_t *buff);
    huaweiPowerCurrent = value;

    if(huaweiPowerState == 0) return ;  // 未上电，不进行操作

    value *= 1024;                      // 电压乘以倍数
    sendbuf[0] = 0x01;
    sendbuf[1] = 0x0F;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = (value >> 24)  & 0xFF;
    sendbuf[5] = (value >> 16)  & 0xFF;
    sendbuf[6] = (value >> 8)   & 0xFF;
    sendbuf[7] = (value >> 0)   & 0xFF;

    CAN_SetEXFrame(USE_CANA, 0x068180FE, sendbuf);
}

// 华为电源 读取电流
void Huawei_ReadCurrent(void)
{
    extern uint16_t CAN_SetEXFrame(uint16_t port ,uint32_t ID,uint8_t *buff);

    if(huaweiPowerState == 0) return ; // 未上电，不进行操作
    sendbuf[0] = 0x01;
    sendbuf[1] = 0x82;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = 0x00;
    sendbuf[5] = 0x00;
    sendbuf[6] = 0x00;
    sendbuf[7] = 0x00;

    CAN_SetEXFrame(USE_CANA, 0x068182FE, sendbuf);
}

// 华为电源 读取电压
void Huawei_ReadVoltage(void)
{
    extern uint16_t CAN_SetEXFrame(uint16_t port ,uint32_t ID,uint8_t *buff);

    if(huaweiPowerState == 0) return ; // 未上电，不进行操作
    sendbuf[0] = 0x01;
    sendbuf[1] = 0x75;
    sendbuf[2] = 0x00;
    sendbuf[3] = 0x00;
    sendbuf[4] = 0x00;
    sendbuf[5] = 0x00;
    sendbuf[6] = 0x00;
    sendbuf[7] = 0x00;

    CAN_SetEXFrame(USE_CANA, 0x068182FE, sendbuf);
}
void Huawei_Control(void)
{
    extern uint32_t CAN_CMD_ID;
    extern uint32_t CAN_CMD_DeviceID;
    extern uint8_t CAN_RXBuff[8];
    uint32_t temp = 0;

    switch (CAN_RXBuff[1])
    {
    case 0x82:
        temp = (uint32_t)(CAN_RXBuff[4] & 0xFF) << 24 |(uint32_t) (CAN_RXBuff[5] & 0xFF) << 16 | (uint32_t)(CAN_RXBuff[6] & 0xFF) << 8 | (uint32_t)(CAN_RXBuff[7] & 0xFF) << 0 ;
        RDhuaweiVoltage = temp / 1024;
        break;
    case 0x75:
        temp = (uint32_t)(CAN_RXBuff[4] & 0xFF) << 24 | (uint32_t)(CAN_RXBuff[5] & 0xFF) << 16 | (uint32_t)(CAN_RXBuff[6] & 0xFF) << 8 | (uint32_t)(CAN_RXBuff[7] & 0xFF) << 0 ;
        RDhuaweiCurrent = temp / 1024;
        break;
    default:
        break;
    }
}

















