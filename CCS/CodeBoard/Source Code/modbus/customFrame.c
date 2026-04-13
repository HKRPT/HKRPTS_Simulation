/*
 * customFrame.c
 *
 *  Created on: 2025年12月18日
 *      Author: YangHaoYan
 */
#include "customFrame.h"

extern SYSTEM_STATE ControlState;
extern SYSTEM_RECORD ControlRecord;

extern uint16_t (*modbusMasterReturnFrame[MODBUS_NUM])(modbusMasterData_t* ) ;

// 自定义帧返回
uint16_t customFrameReturnFrame(modbusMasterData_t* data)
{
    uint16_t my_crc = 0x00;
    uint16_t i      = 0;
    uint16_t *buff, *flag, *length;

    if(data->TxStartFlag != 0x01) return ERROR;       // 未使能发送

    buff    = data->Txbuff;         // 数组缓存区
    length  = data->Txlen;          // 数据长度
    flag    = data->exTxFlag;       // 外部发送标志位


    if(*flag != 0x01)return ERROR;  // 尚有数据未发送完成

    // 添加帧数据
    buff[i++] =   CID;                          // 从站地址
    buff[i++] = (*(data->targetReg) & 0xFF);    // 寄存器地址
    buff[i++] = (*(data->targetReg) & 0xFF);    // 寄存器地址
    buff[i++] = (*(data->targetReg) & 0xFF);    // 寄存器地址
    buff[i++] = (*(data->targetReg) & 0xFF);    // 寄存器地址
    my_crc    = usMBCRC16(buff, i);             // CRC验证
    buff[i++] = (my_crc & 0x00FF) >> 0;
    buff[i++] = (my_crc & 0xFF00) >> 8;

    *length = i;                    // 长度
    *flag   = 0;                    // 外部发送标志位

    return NO_ERROR;
}

uint16_t MBbuf[4]    = {0,0,0,0};
void customFrameSendFloat(modbusMasterData_t* data, float *value)
{
    // 浮点转数组
    extern void modbusFloat2Buff(uint8_t *buff,float *value);

    modbusFloat2Buff(MBbuf, value);

    data->TxStartFlag   = 0x01;                 // 发送标志位
    data->targetReg     = MBbuf;                // 目标寄存器
    modbusMasterReturnFrame[data->port] = customFrameReturnFrame;
}

//自定义帧解析
void DebugConfigFrameAnalytic(modbusMasterData_t* data)
{
    extern void modbusBuff2Float(uint8_t *buff,float * value);
    static float zz = 0;
    uint16_t my_crc     = 0x00;
    uint16_t remote_crc = 0x00;
    uint8_t *buff       = data->Rxbuff;
    uint16_t len        = *(data->Rxlen);

    if(data->RxOverFlag != 0x01) return;    // 接收未完成

    if(len != 8)
    {
        data->RX_ArrayInit(data->port);     // 未知协议弃之
        return;
    }

    my_crc = usMBCRC16(buff, len - 2);
    remote_crc = ((buff[len - 1] & 0xFF)  << 8)| buff[len - 2] & 0xFF;     // 远程CRC
    if(my_crc != remote_crc)
    {
        // CRC 校验不对
        data->RX_ArrayInit(data->port);     // 未知协议弃之
        return;
    }

    switch (buff[1])
    {
        case 1: // 电流给定
        {
            extern float Io_Ref[];
            modbusBuff2Float(&buff[2], &zz);

            if (zz > 1500) // 1500A
            {
                zz = 1500;
                customFrameSendFloat(data, &zz);
            }

            Io_Ref[0] = zz * 1.41421356;
            customFrameSendFloat(data, &Io_Ref[0]);
            break;
        }

        case 2: // 电流软启A/S
        {
            extern float Io_Slope;
            modbusBuff2Float(&buff[2], &zz);

            if (zz <= 0)
            {
                zz = 1.0; // 1.0A
            }
            Io_Slope = zz;
            customFrameSendFloat(data, &Io_Slope);
            break;
        }

        case 3: // 输出频率给定
        {
            modbusBuff2Float(&buff[2], &zz);

            if (zz >= 60.0)
            {
                zz = 60.0;
            }
            else if (zz <= 5.0)
            {
                zz = 5.0;
            }
            else
            {
                ;
            }
            customFrameSendFloat(data, &zz);
            break;
        }

        case 4: // 频率瞬变Hz/s
        {
            modbusBuff2Float(&buff[2], &zz);
            customFrameSendFloat(data, &zz);
            break;
        }

        case 5: // 最大输出电流给定
        {
            extern float Io_Max;
            modbusBuff2Float(&buff[2], &zz);
            if (zz > 1100)
            {
                zz = 1100;
                customFrameSendFloat(data, &zz);
            }

            Io_Max = zz;
            customFrameSendFloat(data, &Io_Max);
            break;
        }

        case 6: // 过温保护设定, 默认75℃
        {
            extern float xIGBT_TemperatureMax;
            modbusBuff2Float(&buff[2], &zz);
            if (zz > 100.0)
            {
                zz = 100.0;
                customFrameSendFloat(data, &zz);
            }

            xIGBT_TemperatureMax = zz;
            customFrameSendFloat(data, &xIGBT_TemperatureMax);
            break;
        }

        case 7: // 电流环 Io_Kp 参数给定
        {
            extern float Io_Kp;
            modbusBuff2Float(&buff[2], &zz);
            Io_Kp = zz;
            customFrameSendFloat(data, &zz);
            break;
        }

        case 8: // 过温保护设定, 默认130℃
        {
            extern float iIGBT_TemperatureMax;
            modbusBuff2Float(&buff[2], &zz);
            if (zz > 160.0)
            {
                zz = 160.0;
                customFrameSendFloat(data, &zz);
            }

            iIGBT_TemperatureMax = zz;
            customFrameSendFloat(data, &iIGBT_TemperatureMax);
            break;
        }

        case 9: // 电流环 Io_Ki 参数给定
        {
            extern float Io_Ki;
            modbusBuff2Float(&buff[2], &zz);
            Io_Ki = zz;
            customFrameSendFloat(data, &zz);
            break;
        }

        case 10: // Uixc 参数给定
        {
            extern float Uixc;
            modbusBuff2Float(&buff[2], &zz);
            Uixc = zz;
            customFrameSendFloat(data, &zz);
            break;
        }

        case 11: // 电流环 Io_Kd 参数给定
        {
            extern float Io_Kd;
            modbusBuff2Float(&buff[2], &zz);
            Io_Kd = zz;
            customFrameSendFloat(data, &zz);
            break;
        }

        case 16: // 电流环 Io_bd 参数给定
        {
            extern float Io_bd;
            modbusBuff2Float(&buff[2], &zz);
            Io_bd = zz;
            customFrameSendFloat(data, &zz);
            break;
        }

        case 12: // 直流母线电压给定锁定
        {

            break;
        }

        case 13: // 故障复位命令
        {
            ControlState = STATE_IDLE;
            break;
        }

        case 15: // 运行参数反馈
        {
            extern SYSTEM_RECORD ControlRecord;
            if (ControlRecord != RECORD_STATE_RUNNING)
            {
                ControlRecord = RECORD_STATE_RUNNING;
            }
            else
            {
                ControlRecord = RECORD_DEFAULT;
            }
            break;
        }

        case 17: // 功率模块拆卸操作
        {

            break;
        }

        case 19: // 手动运行命令
        {
//            extern void REDLedCtrl(uint8_t s);
            if (ControlState != STATE_RUNNING)
            {
                if (ControlState == STATE_ERROR)
                {
                    // 发送错误代码
//                    REDLedCtrl(0);
                }
                else if (ControlState == STATE_STOP) // 停机状态下才能够手动启动
                {
                    ControlState = STATE_READY;
//                    REDLedCtrl(1);
                }
            }
            break;
        }

        case 20: // 记录交直输入电压
        {
            extern SYSTEM_RECORD ControlRecord;
            ControlRecord = RECORD_INPUT_VOLTAGE;
            break;
        }

        case 21: // 记录交流输出电流
        {
            extern SYSTEM_RECORD ControlRecord;
            ControlRecord = RECORD_OUPUT_CURRENT;
            break;
        }

        case 22: // 记录交流输出电压
        {
            extern SYSTEM_RECORD ControlRecord;
            ControlRecord = RECORD_OUPUT_VOLTAGE;
            break;
        }

        case 25: // 记录功率模块温度
        {
            extern SYSTEM_RECORD ControlRecord;
            ControlRecord = RECORD_IGBT_TEMPERATURE;
            break;
        }

        case 26: // 风扇控制
        {
            extern void FanStateCtrl(uint8_t s);
            extern uint8_t ManualFanCtrlFlg;
            if (ManualFanCtrlFlg)
            {
                ManualFanCtrlFlg = 0;
                FanStateCtrl(0);
            }
            else
            {
                ManualFanCtrlFlg = 1;
                FanStateCtrl(1);
            }
            break;
        }

        case 27: // 手动停机命令
        {
//            extern void REDLedCtrl(uint8_t s);
            ControlState = STATE_STOP;
//            REDLedCtrl(0);
            break;
        }

        case 28: // 反馈数据
        {

            break;
        }

        case 29: // 中断占比
        {
            extern SYSTEM_RECORD ControlRecord;

            zz = 1/__FT;
            customFrameSendFloat(data, &zz);

            ControlRecord = RECORD_STATE_AVG_RMS;
            break;
        }

        case 30: // 采样时基 ms
        {

            break;
        }

        case 31: // 调节直流母线电压
        {

            break;
        }

        case 32: // 采样时长 ms
        {

            break;
        }

        case 33: // 通讯测试
        {
            zz = 0.0;
            customFrameSendFloat(data, &zz);
            break;
        }

        case 51:  //直流母线合闸
        {

            break;
        }

        case 52:
        {

            break;
        }

        case 0xFF: // 心跳
        {

            break;
        }
        default:
              break;
    }

    data->RX_ArrayInit(data->port);     // 处理完成清除
}
