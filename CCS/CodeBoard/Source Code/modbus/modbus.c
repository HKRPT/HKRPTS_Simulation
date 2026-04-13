/*
 * modbus.c
 *
 *  Created on: 2025年2月8日
 *      Author: BYRD-003
 */
#include "modbus.h"
#include "mbcrc.h"
#include "my_types.h"
#include "svpwm.h"
#include "powerModule.h"
#include "my_adcContorl.h"

#ifdef _FLASH
    modbusMasterData_t  modbusMasterData[MODBUS_NUM];
    #pragma DATA_SECTION(modbusMasterData, "flash_ramgs");          // map the TX data to memory
#else
    modbusMasterData_t  modbusMasterData[MODBUS_NUM];
    #pragma DATA_SECTION(modbusMasterData, "rwEX_RAMFile");          // map the TX data to memory
#endif

uint16_t (*modbusMasterReturnFrame[MODBUS_NUM])(modbusMasterData_t* ) = {NULL};    // 帧处理函数

void modbusBuff2UINT16(uint8_t *buff,uint16_t * value)
{
    *value = ((buff[0] & 0xFF) << 8) | (buff[1] & 0xFF);
}

void modbusBuff2UINT32(uint8_t *buff,uint32_t * value)
{
    *value = ((uint32_t)(buff[0] & 0xFF) << 24) | ((uint32_t)(buff[1] & 0xFF) << 16) | ((uint32_t)(buff[2] & 0xFF) << 8) | ((uint32_t)(buff[3] & 0xFF) << 0);
}

#define DATA_STORE_FORMAT LITTLE_ENDIAN         // 大小端模式
void modbusBuff2Float(uint8_t *buff,float * value)
{
    uint8_t i;
    union ss
    {
        uint16_t    tmp[2];
        float       tmp_f;
    }DataTrant;

    for(i = 0; i < 2; i++)
    {
        DataTrant.tmp[i] = 0;
    }

#if (DATA_STORE_FORMAT == LITTLE_ENDIAN)
    DataTrant.tmp[1] = ((uint16_t)buff[0] << 8) | buff[1];  // 低16位
    DataTrant.tmp[0] = ((uint16_t)buff[2] << 8) | buff[3];  // 高16位

#else // BIG_ENDIAN
    DataTrant.tmp[1] = ((uint16_t)buff[3] << 8) | buff[2];  // 高16位
    DataTrant.tmp[0] = ((uint16_t)buff[1] << 8) | buff[0];  // 低16位
#endif

    *value = DataTrant.tmp_f;
}

// 浮点转数组
void modbusFloat2Buff(uint8_t *buff,float *value)
{
    uint8_t i = 0;
    union ss
    {
        uint16_t    tmp[2];
        float       tmp_f;
    }DataTrant;

    DataTrant.tmp_f = *value;

    buff[i++] =  DataTrant.tmp[0]         & 0xFF;
    buff[i++] = (DataTrant.tmp[0] >> 8)   & 0xFF;
    buff[i++] =  DataTrant.tmp[1]         & 0xFF;
    buff[i++] = (DataTrant.tmp[1] >> 8)   & 0xFF;
}

uint16_t testmsg[4]  = {0x01,0x02,0x03,0x04};
// 03 主站功能码帧返回
uint16_t modbus03MasterReturnFrame(modbusMasterData_t* data)
{
    uint16_t my_crc = 0x00;
    uint16_t i      = 0;
    uint16_t *buff, *flag, *length;
    uint16_t len    = data->targetlen;  // 目标长度

    if(data->TxStartFlag != 0x01) return ERROR;       // 未使能发送

    buff    = data->Txbuff;         // 数组缓存区
    length  = data->Txlen;          // 数据长度
    flag    = data->exTxFlag;       // 外部发送标志位


    if(*flag != 0x01)return ERROR;  // 尚有数据未发送完成

    // 添加帧数据
    buff[i++]   =   CID;            // 从站地址
    buff[i++]   =   0x03;           // 查询功能
    buff[i++]   =   len * 2;        // 寄存器长度
    while(len--)
    {
        buff[i++] = (*(data->targetReg) & 0xFF00) >> 8; // 目标寄存器值 (高位)
        buff[i++] = (*(data->targetReg) & 0x00FF);      // 目标寄存器值 (低位)
        data->targetReg++;
    }
    my_crc = usMBCRC16(buff, i);    // CRC验证
    buff[i++] = (my_crc & 0x00FF) >> 0;
    buff[i++] = (my_crc & 0xFF00) >> 8;

    *length = i;                    // 长度
    *flag   = 0;                    // 外部发送标志位

    return NO_ERROR;
}

// 05 主站功能码帧返回
uint16_t modbus05MasterReturnFrame(modbusMasterData_t* data)
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
    buff[i++]   =   CID;            // 从站地址
    buff[i++]   =   0x05;           // 写寄存器
    buff[i++] = (*(data->targetReg) & 0xFF00) >> 8; // 起始地址 (高位)
    buff[i++] = (*(data->targetReg) & 0x00FF);      // 起始地址 (低位)
    buff[i++] = ((data->targetlen) & 0xFF00) >> 8; // 线圈值 (高位)
    buff[i++] = ((data->targetlen) & 0x00FF);      // 线圈值 (低位)
    my_crc    = usMBCRC16(buff, i);    // CRC验证
    buff[i++] = (my_crc & 0x00FF) >> 0;
    buff[i++] = (my_crc & 0xFF00) >> 8;

    *length = i;                    // 长度
    *flag   = 0;                    // 外部发送标志位

    return NO_ERROR;
}

// 10 主站功能码帧返回
uint16_t modbus10MasterReturnFrame(modbusMasterData_t* data)
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
    buff[i++]   =   CID;            // 从站地址
    buff[i++]   =   0x10;           // 写多个寄存器
    buff[i++] = (*(data->targetReg) & 0xFF00) >> 8; // 起始地址 (高位)
    buff[i++] = (*(data->targetReg) & 0x00FF);      // 起始地址 (低位)
    buff[i++] = ((data->targetlen) & 0xFF00) >> 8; // 起始数量 (高位)
    buff[i++] = ((data->targetlen) & 0x00FF);      // 起始数量 (低位)
    my_crc    = usMBCRC16(buff, i);    // CRC验证
    buff[i++] = (my_crc & 0x00FF) >> 0;
    buff[i++] = (my_crc & 0xFF00) >> 8;

    *length = i;                    // 长度
    *flag   = 0;                    // 外部发送标志位

    return NO_ERROR;
}

uint16_t MBaddr2 = 0;
void modbus05DebugSendFloat(modbusMasterData_t* data, uint16_t value, uint16_t addr)
{
    MBaddr2 = addr;
    data->TxStartFlag   = 0x01;                 // 发送标志位
    data->targetlen     = value;                  // 目标长度
    data->targetReg     = &MBaddr2;             // 目标寄存器
    modbusMasterReturnFrame[data->port] = modbus05MasterReturnFrame;
}

// 03 主站功能码帧解析(查询传感器数据)
void modbus03MasterFrameResolution(modbusMasterData_t* data)
{
    extern my_adcContorl_t my_adcContorl[ADC_NUM + ADC_EX_NUM];
    uint16_t my_crc     = 0x00;
    uint16_t remote_crc = 0x00;
    uint16_t MBaddr     = 0x00;
    uint16_t MBlen      = 0x00;
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

    // 按地址索引
    MBaddr = ((buff[2] & 0xFF)  << 8)| buff[3] & 0xFF;   // modbus 请求地址

    switch(MBaddr)
    {
    case 0x01:
        modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
        if(MBlen <= 0x04)                       // 若长度等于或小于则发送
        {
            data->TxStartFlag   = 0x01;         // 发送标志位
            data->targetlen     = MBlen;        // 目标长度
            data->targetReg     = testmsg;      // 目标寄存器
            modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        }
        break;
    case 0x02:
        // DI 信息查询 01 03 00 02 00 01 25 CA
        modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
        if(MBlen <= 0x10)                       // 若长度等于或小于则发送
        {
            data->TxStartFlag   = 0x01;                 // 发送标志位
            data->targetlen     = MBlen;                // 目标长度
            data->targetReg     = &roHW_DIInputState;   // 目标寄存器
            modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        }
        break;
    case 0x03:
        // FPGA 信息 01 03 00 03 00 01 74 0A
        data->TxStartFlag   = 0x01;                 // 发送标志位
        data->targetlen     = 0x01;                 // 目标长度
        data->targetReg     = &roFPGA_Version;      // 目标寄存器
        modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        break;
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
        // ADC 信息查询 01 03 00 04 00 12 84 06
        modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
        if(MBlen <= (18 -(MBaddr - 0x04)))      // 若长度等于或小于则发送
        {
            data->TxStartFlag   = 0x01;                                     // 发送标志位
            data->targetlen     = MBlen;                                    // 目标长度
            data->targetReg     = (uint16_t*)&roEXADC_Result[MBaddr-0x04]; // 目标寄存器
            modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        }
        break;
    case 0x16:
    case 0x18:
    case 0x1A:
    case 0x1C:
    case 0x1E:
    case 0x20:
        // PT100 信息查询 01 03 00 16 00 0C A4 0B  排序方式：CDAB
        modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
        if(MBlen <= (12 -(MBaddr - 0x16)))      // 若长度等于或小于则发送
        {
            data->TxStartFlag   = 0x01;                                                 // 发送标志位
            data->targetlen     = MBlen;                                                // 目标长度
            data->targetReg     = (uint16_t*)&my_adcContorl[PT100_1 + ((MBaddr-0x16)/2)];   // 目标寄存器
            modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        }
        break;
    case 0x22:
    case 0x24:
    case 0x26:
    case 0x28:
    case 0x2A:
    case 0x2C:
        // NTC 信息查询 01 03 00 22 00 0C E5 C5
        modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
        if(MBlen <= (12 -(MBaddr - 0x22)))      // 若长度等于或小于则发送
        {
            data->TxStartFlag   = 0x01;                                                     // 发送标志位
            data->targetlen     = MBlen;                                                    // 目标长度
            data->targetReg     = (uint16_t*)&my_adcContorl[u_NTC_U + ((MBaddr-0x22)/2)];   // 目标寄存器
            modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        }
        break;
    case 0x2E:
    case 0x30:
    case 0x32:
    case 0x34:
    case 0x36:
    case 0x38:
    case 0x3A:
    case 0x3C:
    case 0x3E:
    case 0x40:
    case 0x42:
    case 0x44:
    case 0x46:
    case 0x48:
    case 0x4A:
    case 0x4C:
       // 通用ADC 信息查询 01 03 00 2E 00 20 24 1B
        modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
        if(MBlen <= (32 -(MBaddr - 0x2E)))      // 若长度等于或小于则发送
        {
            data->TxStartFlag   = 0x01;                                                     // 发送标志位
            data->targetlen     = MBlen;                                                    // 目标长度
            data->targetReg     = (uint16_t*)&my_adcContorl[General_ADC_1 + ((MBaddr-0x2E)/2)];   // 目标寄存器
            modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        }
       break;
    case 0x4E:
    case 0x50:
    case 0x52:
    case 0x54:
    case 0x56:
    case 0x58:
       // 电流ADC 信息查询 01 03 00 4E 00 0C 25 D8
        modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
        if(MBlen <= (12 -(MBaddr - 0x4E)))      // 若长度等于或小于则发送
        {
            data->TxStartFlag   = 0x01;                                                     // 发送标志位
            data->targetlen     = MBlen;                                                    // 目标长度
            data->targetReg     = (uint16_t*)&my_adcContorl[i_ADC_1 + ((MBaddr-0x4E)/2)];   // 目标寄存器
            modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
        }
       break;
    case 0x5A:
    case 0x5C:
    case 0x5E:
    case 0x60:
    case 0x62:
    case 0x64:
       // 功率模块NTC 信息查询 01 03 00 5A 00 0C 65 DC
       modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
       if(MBlen <= (12 -(MBaddr - 0x5A)))      // 若长度等于或小于则发送
       {
           data->TxStartFlag   = 0x01;                                                          // 发送标志位
           data->targetlen     = MBlen;                                                         // 目标长度
           data->targetReg     = (uint16_t*)&my_adcContorl[IGBT_NTC_ADC_U + ((MBaddr-0x5A)/2)]; // 目标寄存器
           modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
       }
       break;
    case 0x66:
    case 0x68:
    case 0x6A:
    case 0x6C:
    case 0x6E:
    case 0x70:
       // 功率模块电压 信息查询 01 03 00 66 00 0C A5 D0
       modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
       if(MBlen <= (12 -(MBaddr - 0x66)))      // 若长度等于或小于则发送
       {
           data->TxStartFlag   = 0x01;                                                              // 发送标志位
           data->targetlen     = MBlen;                                                             // 目标长度
           data->targetReg     = (uint16_t*)&my_adcContorl[IGBT_Voltage_ADC_U + ((MBaddr-0x66)/2)]; // 目标寄存器
           modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
       }
       break;
    case 0x72:
    case 0x74:
    case 0x76:
    case 0x78:
    case 0x7A:
    case 0x7C:
       // 功率模块电流 信息查询 01 03 00 72 00 0C E5 D4
       modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
       if(MBlen <= (12 -(MBaddr - 0x72)))      // 若长度等于或小于则发送
       {
           data->TxStartFlag   = 0x01;                                                              // 发送标志位
           data->targetlen     = MBlen;                                                             // 目标长度
           data->targetReg     = (uint16_t*)&my_adcContorl[IGBT_Current_ADC_U + ((MBaddr-0x72)/2)]; // 目标寄存器
           modbusMasterReturnFrame[data->port] = modbus03MasterReturnFrame;
       }
       break;
    default:
        break;
    }

    data->RX_ArrayInit(data->port);     // 处理完成清除

    return;
}


// 05 主站功能码帧解析(写单个线圈)
void modbus05MasterFrameResolution(modbusMasterData_t* data)
{
    extern uint8_t     recode_flag ;
    extern SYSTEM_STATE ControlState;
    extern void woHW_DOOutputSetState(uint8_t bit, uint16_t s);
    extern void DebugSendFloat(modbusMasterData_t* data, uint16_t len, uint16_t addr);

    uint16_t my_crc     = 0x00;
    uint16_t remote_crc = 0x00;
    uint16_t MBaddr     = 0x00;
    uint16_t MBlen      = 0x00;
    uint8_t *buff       = data->Rxbuff;
    uint16_t len        = *(data->Rxlen);
    uint16_t value      = 0x00;

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

    // 按地址索引
    MBaddr = ((buff[2] & 0xFF)  << 8)| buff[3] & 0xFF;   // modbus 请求地址

    switch(MBaddr)
    {
    // DO 控制
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
            // DO 输出控制 01 05 00 0F 00 00 FD C9
            modbusBuff2UINT16(&buff[4],&MBlen);     // 读取线圈值
            value = MBlen == 0x00 ? 0: 1;           // 线圈开与关

            woHW_DOOutputSetState(MBaddr, value);   // IO 控制

            modbus05DebugSendFloat(data,MBlen, MBaddr);     // 返回报文
        break;
    case 0x13:
        {
            modbusBuff2UINT16(&buff[4],&MBlen);     // 读取线圈值

            // 故障复位命令
            // 01 05 00 13 FF 00 7D FF
            ControlState   = STATE_IDLE;

            modbus05DebugSendFloat(data,MBlen, MBaddr);     // 返回报文
            break;
        }
    case 0x19:  // 手动运行命令
        {
            modbusBuff2UINT16(&buff[4],&MBlen);     // 读取线圈值
            if(ControlState != STATE_RUNNING)
            {
                if(ControlState == STATE_ERROR)
                {
                    // 发送错误代码
                }
                else if(ControlState == STATE_STOP) // 停机状态下才能够手动启动
                {
                    ControlState = STATE_READY;
                }
            }
            modbus05DebugSendFloat(data,MBlen, MBaddr);     // 返回报文
            break;
        }
    case 0x27:  // 手动停机命令
        {
            modbusBuff2UINT16(&buff[4],&MBlen);     // 读取线圈值
            ControlState = STATE_STOP;
            modbus05DebugSendFloat(data,MBlen, MBaddr);     // 返回报文
            break;
        }
    case 0x28:  // 反馈录播
        {
            modbusBuff2UINT16(&buff[4],&MBlen);     // 读取线圈值
            value = MBlen == 0x00 ? 0: 1;           // 线圈开与关

            recode_flag = value;
            modbus05DebugSendFloat(data,MBlen, MBaddr);     // 返回报文
            break;
        }
    default:
        break;
    }

    data->RX_ArrayInit(data->port);     // 处理完成清除

    return;
}

uint16_t MBaddr1    = 0x00;
void DebugSendFloat(modbusMasterData_t* data, uint16_t len, uint16_t addr)
{
    MBaddr1 = addr;
    data->TxStartFlag   = 0x01;                 // 发送标志位
    data->targetlen     = len;                  // 目标长度
    data->targetReg     = &MBaddr1;             // 目标寄存器
    modbusMasterReturnFrame[data->port] = modbus10MasterReturnFrame;
}

// 10 主站功能码帧解析(写多个寄存器)
void modbus10MasterFrameResolution(modbusMasterData_t* data)
{
    extern uint16_t ch1_index,ch2_index,ch3_index,ch4_index,ch5_index,ch6_index,ch7_index;
    uint16_t my_crc     = 0x00;
    uint16_t remote_crc = 0x00;
    uint16_t MBaddr     = 0x00;
    uint8_t *buff       = data->Rxbuff;
    uint16_t len        = *(data->Rxlen);
    uint16_t MBlen      = 0x00;
    uint16_t val16      = 0;
    float value      = 0x00;

    if(data->RxOverFlag != 0x01) return;    // 接收未完成

    if(len > ((uint16_t)buff[6] + 9))        // 字节数
    {
        data->RX_ArrayInit(data->port);     // 未知协议弃之
        return;
    }
    else if(len < ((uint16_t)buff[6] + 9))
    {
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

    // 按地址索引
    MBaddr = ((buff[2] & 0xFF)  << 8)| buff[3] & 0xFF;   // modbus 请求地址

    switch(MBaddr)
    {
    case 1:
        {
            extern float Io_Ref[];
            // 数据帧 01 10 00 01 00 02 04 40 06 66 06 6C
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            if(value > 1500.0)    // 1500A
            {
                value = 1500.0;
            }

            Huawei_SetCurrent(value);
            Io_Ref[0] = value * 1.41421356;
            DebugSendFloat(data,MBlen, MBaddr);
            break;
        }
    case 3:
        {
            extern float Io_Slope;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            if(value < 0.0 )
            {
                value = 1.0;                        // 1A
            }

            Io_Slope = value;

            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 5:
        {
            // 最大输出电流给定
            extern float Io_Max;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            if(value > 1100.0 )
            {
                value = 1100.0;                        // 1A
                DebugSendFloat(data, MBlen, MBaddr);
            }

            Io_Max = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 7:
        {
            // 过温保护设定，默认75℃
            extern float xIGBT_TemperatureMax;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            if(value > 100.0 )
            {
                value = 100.0;                        // 1A
                DebugSendFloat(data, MBlen, MBaddr);
            }

            xIGBT_TemperatureMax = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 9:
        {
            // 电流环Io_Kp参数给定
            extern float Io_Kp;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验

            Io_Kp = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 11:
        {
            // 过温保护给定，默认130℃
            extern float iIGBT_TemperatureMax;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验

            if(value > 160.0)
            {
                value = 160.0;
                DebugSendFloat(data, MBlen, MBaddr);
            }

            iIGBT_TemperatureMax = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 13:
        {
            // 电流环 Io_Ki 参数给定
            extern float Io_Ki;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验

            Io_Ki = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 15:
        {
            // Uixc 参数给定
            extern float Uixc;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验

            Uixc = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 17:
        {
            // 电流环 Io_Kd 参数给定
            extern float Io_Kd;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验

            Io_Kd = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 19:
        {
            // 电流环 Io_bd 参数给定
            extern float Io_bd;
            modbusBuff2Float(&buff[7], &value);
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验

            Io_bd = value;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 21:
        {
            // 电源模块输出电压给定
            extern uint32_t PowerVoltage ;           // 电源模块输出电压
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            PowerVoltage = val16;
            Huawei_SetVoltage(PowerVoltage);
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 22:
        {
            // 电源模块输出电流给定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            PowerCurrent = val16;
            Huawei_SetCurrent(PowerCurrent);
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 23:
        {
            // 通道1 映射设定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            ch1_index = val16;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 24:
        {
            // 通道2 映射设定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            ch2_index = val16;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 25:
        {
            // 通道3 映射设定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            ch3_index = val16;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 26:
        {
            // 通道4 映射设定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            ch4_index = val16;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 27:
        {
            // 通道5 映射设定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            ch5_index = val16;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 28:
        {
            // 通道6 映射设定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            ch6_index = val16;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }
    case 29:
        {
            // 通道7 映射设定
            extern uint32_t PowerCurrent ;            // 电源模块输出电流
            modbusBuff2UINT16(&buff[4],&MBlen);     // 对 mosbus 传过来的长度进行校验
            modbusBuff2UINT16(&buff[7],&val16);     // 解析

            ch7_index = val16;
            DebugSendFloat(data, MBlen, MBaddr);
            break;
        }

    default:
        break;
    }

    data->RX_ArrayInit(data->port);     // 处理完成清除

    return;
}


// modbus 帧返回函数
void modbusMaster(void)
{
    uint16_t i = 0;
    for(i = 0; i < MODBUS_NUM; i++)
    {
        if(modbusMasterReturnFrame[i] != NULL)
        {
            if(modbusMasterReturnFrame[i](&modbusMasterData[i]) == NO_ERROR)
            {
                modbusMasterData[i].TxStartFlag = 0x00;     // 清除发送完成标志位
                modbusMasterData[i].RxOverFlag  = 0x00;     // 清除接收完成标志位
                modbusMasterReturnFrame[i] = NULL;
            }
        }
    }
}
