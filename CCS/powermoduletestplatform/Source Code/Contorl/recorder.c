/*
 * recorder.c
 *
 *  Created on: 2025年10月28日
 *      Author: YangHaoYan
 */
#include "recoder.h"
#include "my_adcContorl.h"

uint8_t     recode_flag             = 0;        // 录波标志位
uint32_t    recode_SampleTime       = 0;        // 录播采样时间
uint32_t    recode_SampleTimeCnt    = 0;        // 录播采样时间
uint32_t    mlen                    = 0;        // 采样长度
uint32_t    mlenTarget              = 0;        // 采样目标长度



RingBuffer_t g_ringBuf = {0};

SYSTEM_RECORD ControlRecord = RECORD_DEFAULT;
// 10 从站功能码帧发送
uint16_t modbus10ClientSendFrame(modbusMasterData_t* data, uint16_t sample_addr, uint16_t sample_times, uint32_t** sample_buf)
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

    buff[i++] = (sample_addr & 0xFF00) >> 8;            // 起始地址 (高位)
    buff[i++] = (sample_addr & 0x00FF);                 // 起始地址 (低位)

    buff[i++] = ((sample_times * 2) & 0xFF00) >> 8;     // 寄存器数量 (高位)
    buff[i++] = ((sample_times * 2) & 0x00FF);          // 寄存器数量 (低位)

    buff[i++] = ((sample_times * 4) & 0xFF00) >> 8;     // 字节数量 (高位)
    buff[i++] = ((sample_times * 4) & 0x00FF);          // 字节数量 (低位)

    while(sample_times)
    {
        sample_times--;
        buff[i++] = (sample_buf[sample_addr][sample_times] & 0xFF000000) >> 24;          // 所需地址 (高位)
        buff[i++] = (sample_buf[sample_addr][sample_times] & 0x00FF0000) >> 16;          // 所需地址 (高位)
        buff[i++] = (sample_buf[sample_addr][sample_times] & 0x0000FF00) >> 8;           // 所需地址 (高位)
        buff[i++] = (sample_buf[sample_addr][sample_times] & 0x000000FF);                // 所需地址 (高位)
    }

    my_crc    = usMBCRC16(buff, i);    // CRC验证
    buff[i++] = (my_crc & 0x00FF) >> 0;
    buff[i++] = (my_crc & 0xFF00) >> 8;

    *length = i;                    // 长度
    *flag   = 0;                    // 外部发送标志位

    return NO_ERROR;
}

// 10 从站功能码帧发送
uint16_t JustFloatClientSendFrame(modbusMasterData_t* data, uint16_t sample_addr, uint16_t sample_times, float (*sample_buf)[RING_BUF_SIZE])
{
    uint16_t i=0,j=0;
    uint16_t *buff, *flag, *length;
    typedef union
    {
        uint32_t    tmp32;
        float       tmpfp;
    }type2u32;

    type2u32 tmp;

    if(data->TxStartFlag != 0x01) return ERROR;       // 未使能发送

    buff    = data->Txbuff;         // 数组缓存区
    length  = data->Txlen;          // 数据长度
    flag    = data->exTxFlag;       // 外部发送标志位


    if(*flag != 0x01)return ERROR;  // 尚有数据未发送完成


    for(j = 0; j < MAX_CHANNEL; j++)
    {
        tmp.tmpfp = sample_buf[j][sample_times];
        buff[i++] = (tmp.tmp32 & 0x000000FF);                // 所需地址 (高位)
        buff[i++] = (tmp.tmp32 & 0x0000FF00) >> 8;           // 所需地址 (高位)
        buff[i++] = (tmp.tmp32 & 0x00FF0000) >> 16;          // 所需地址 (高位)
        buff[i++] = (tmp.tmp32 & 0xFF000000) >> 24;          // 所需地址 (高位)
    }

    buff[i++] = 0x00;
    buff[i++] = 0x00;
    buff[i++] = 0x80;
    buff[i++] = 0x7F;

    *length = i;                    // 长度
    *flag   = 0;                    // 外部发送标志位

    return NO_ERROR;
}

uint16_t ch1_index = 0;
uint16_t ch2_index = 0;
uint16_t ch3_index = 0;
uint16_t ch4_index = 0;
uint16_t ch5_index = 0;
uint16_t ch6_index = 0;
uint16_t ch7_index = 0;


// while循环运行
void Recode_loop(void)
{
    extern modbusMasterData_t modbusMasterData[MODBUS_NUM];
    extern void UART_modbusConstruction(uint16_t port);
    extern void W5500_modbusConstruction(uint16_t port, uint8_t socket);

    if (g_ringBuf.count == 0) {
        return; // 缓冲区空
    }

    uint16_t readIdx = g_ringBuf.tail;

    W5500_modbusConstruction(0,2);
    modbusMasterData[6].TxStartFlag = 0x01;

    // 传入整个 buffer，JustFloatClientSendFrame 会读取 buffer[channel][readIdx]
    if (JustFloatClientSendFrame(&modbusMasterData[6], 0x01, readIdx, g_ringBuf.buffer) == NO_ERROR) {
        // 读取成功，移动 tail
        g_ringBuf.tail = (readIdx + 1) % RING_BUF_SIZE;
        g_ringBuf.count--;
    }
    // 若发送失败，保留数据，下次重试
}

// 通过索引返回需要查询的变量指针
float RecoderFrameReturnPtr(uint16_t i)
{
    extern SYSTEM_STATE ControlState;
    extern float ui[], io[], uo[], Ui[];
    extern float iIGBT_Temperature, xIGBT_Temperature, i_AC_Leakage, i_DC_Leakage;
    extern float ubus, ibus, Ubus, Ibus;
    extern float dx, io_d, io_q, io_z;
    extern PidCtrlReg Io_Ctrl;
    extern float sin_angle, cos_angle;
    extern my_adcContorl_t my_adcContorl[ADC_NUM + ADC_EX_NUM];

    float ptr = sin_angle;

    switch (i)
    {
    case SYS_StatePTR:
            ptr = (float)ControlState;
        break;
    case VOL1_IN_PTR:
            ptr = ui[0];
        break;
    case VOL2_IN_PTR:
            ptr = ui[1];
        break;
    case VOL3_IN_PTR:
            ptr = ui[2];
        break;
    case VOL1_OUT_PTR:
            ptr = uo[0];
        break;
    case VOL2_OUT_PTR:
            ptr = uo[1];
        break;
    case VOL3_OUT_PTR:
            ptr = uo[2];
        break;
    case CUR1_OUT_PTR:
            ptr = io[0];
        break;
    case CUR2_OUT_PTR:
            ptr = io[1];
        break;
    case CUR3_OUT_PTR:
            ptr = io[2];
        break;
    case Park_D_PTR:
            ptr = io_d;
        break;
    case Park_Q_PTR:
            ptr = io_q;
        break;
    case UBUS_PTR:
            ptr = Ubus;
        break;
    case IBUS_PTR:
            ptr = Ibus;
        break;
    case CUR_Leakage:
            ptr = i_AC_Leakage;
        break;
    case NTC_1_PTR:
            ptr = my_adcContorl[u_NTC_U].adcValue;
        break;
    case NTC_2_PTR:
            ptr = my_adcContorl[u_NTC_V].adcValue;
        break;
    case NTC_3_PTR:
            ptr = my_adcContorl[u_NTC_W].adcValue;
        break;
    case NTC_4_PTR:
            ptr = my_adcContorl[u_NTC_U1].adcValue;
        break;
    case NTC_5_PTR:
            ptr = my_adcContorl[u_NTC_V1].adcValue;
        break;
    case NTC_6_PTR:
            ptr = my_adcContorl[u_NTC_W1].adcValue;
        break;
    case PT100_1_PTR:
            ptr = my_adcContorl[PT100_1].adcValue;
        break;
    case PT100_2_PTR:
            ptr = my_adcContorl[PT100_2].adcValue;
        break;
    case PT100_3_PTR:
            ptr = my_adcContorl[PT100_3].adcValue;
        break;
    case PT100_4_PTR:
            ptr = my_adcContorl[PT100_4].adcValue;
        break;
    case PT100_5_PTR:
            ptr = my_adcContorl[PT100_5].adcValue;
        break;
    case PT100_6_PTR:
            ptr = my_adcContorl[PT100_6].adcValue;
        break;
    case PM_NTC_1:
            ptr = my_adcContorl[IGBT_NTC_ADC_U].adcValue;
        break;
    case PM_NTC_2:
            ptr = my_adcContorl[IGBT_NTC_ADC_V].adcValue;
        break;
    case PM_NTC_3:
            ptr = my_adcContorl[IGBT_NTC_ADC_W].adcValue;
        break;
    case PM_CUR_1:
            ptr = my_adcContorl[IGBT_Current_ADC_U].adcValue;
        break;
    case PM_CUR_2:
            ptr = my_adcContorl[IGBT_Current_ADC_V].adcValue;
        break;
    case PM_CUR_3:
            ptr = my_adcContorl[IGBT_Current_ADC_W].adcValue;
        break;
    case PM_VOL_1:
            ptr = my_adcContorl[IGBT_Voltage_ADC_U].adcValue;
        break;
    case PM_VOL_2:
            ptr = my_adcContorl[IGBT_Voltage_ADC_V].adcValue;
        break;
    case PM_VOL_3:
            ptr = my_adcContorl[IGBT_Voltage_ADC_W].adcValue;
        break;
    case NOR_ADC_1:
            ptr = my_adcContorl[i_ADC_1].adcValue;
        break;
    case NOR_ADC_2:
            ptr = my_adcContorl[i_ADC_2].adcValue;
        break;
    case NOR_ADC_3:
            ptr = my_adcContorl[i_ADC_3].adcValue;
        break;
    case NOR_ADC_4:
            ptr = my_adcContorl[i_ADC_4].adcValue;
        break;
    case NOR_ADC_5:
            ptr = my_adcContorl[i_ADC_5].adcValue;
        break;
    case NOR_ADC_6:
            ptr = my_adcContorl[i_ADC_6].adcValue;
        break;
    default:
        ptr = sin_angle;
        break;
    }

    return ptr;
}

void timeSampleHandle(void)
{
    extern SYSTEM_STATE ControlState;
    extern float ui[], io[], uo[], Ui[];
    extern float iIGBT_Temperature, xIGBT_Temperature, i_AC_Leakage, i_DC_Leakage;
    extern float ubus, ibus, Ubus, Ibus;
    extern float dx, io_d, io_q, io_z;
    extern PidCtrlReg Io_Ctrl;
    extern float sin_angle, cos_angle;
    uint16_t ch = 0;

    if (!recode_flag) return;

    static uint16_t sampleCnt = 0;
    if (++sampleCnt < recode_SampleTime) return;
    sampleCnt = 0;

    // 缓冲区满，丢弃本次采样
    if (g_ringBuf.count >= RING_BUF_SIZE) {
        return;
    }

    uint16_t idx = g_ringBuf.head;  // 当前写入位置（时间索引）

    // 先清零所有通道（可选，避免残留旧数据）
    for (ch = 0; ch < MAX_CHANNEL; ch++) {
        g_ringBuf.buffer[ch][idx] = 0.0f;
    }

    switch (ControlRecord) {
        case RECORD_INPUT_VOLTAGE:
            g_ringBuf.buffer[0][idx] = ui[0];
            g_ringBuf.buffer[1][idx] = ui[1];
            g_ringBuf.buffer[2][idx] = ui[2];
            g_ringBuf.buffer[3][idx] = ubus;
            g_ringBuf.buffer[4][idx] = ibus * 10.0f;
            // ch5, ch6 保持为0（已清零）
            break;

        case RECORD_OUPUT_CURRENT:
            g_ringBuf.buffer[0][idx] = io[0];
            g_ringBuf.buffer[1][idx] = io[1];
            g_ringBuf.buffer[2][idx] = io[2];
            g_ringBuf.buffer[3][idx] = io_d;
            g_ringBuf.buffer[4][idx] = io_q;
            g_ringBuf.buffer[5][idx] = Ibus;
            g_ringBuf.buffer[6][idx] = i_DC_Leakage;
            // ch3~6 = 0
            break;

        case RECORD_OUPUT_VOLTAGE:
            g_ringBuf.buffer[0][idx] = uo[0];
            g_ringBuf.buffer[1][idx] = uo[1];
            g_ringBuf.buffer[2][idx] = uo[2];
            break;

        case RECORD_IGBT_TEMPERATURE:
            g_ringBuf.buffer[0][idx] = iIGBT_Temperature;
            g_ringBuf.buffer[1][idx] = xIGBT_Temperature;
            g_ringBuf.buffer[2][idx] = i_AC_Leakage;
            g_ringBuf.buffer[3][idx] = i_DC_Leakage;
            // ch4~6 = 0
            break;

        case RECORD_STATE_RUNNING:
            g_ringBuf.buffer[0][idx] = ubus;
            g_ringBuf.buffer[1][idx] = io[0];
            g_ringBuf.buffer[2][idx] = iIGBT_Temperature;
            g_ringBuf.buffer[3][idx] = xIGBT_Temperature;
            break;

        case RECORD_STATE_AVG_RMS:
            g_ringBuf.buffer[0][idx] = Ui[0];
            g_ringBuf.buffer[1][idx] = Ui[1];
            g_ringBuf.buffer[2][idx] = Ui[2];
            g_ringBuf.buffer[3][idx] = Ubus;
            g_ringBuf.buffer[4][idx] = Ibus;
            // ch5 = 0
            break;

        default:
            g_ringBuf.buffer[0][idx] = RecoderFrameReturnPtr(ch1_index);
            g_ringBuf.buffer[1][idx] = RecoderFrameReturnPtr(ch2_index);
            g_ringBuf.buffer[2][idx] = RecoderFrameReturnPtr(ch3_index);
            g_ringBuf.buffer[3][idx] = RecoderFrameReturnPtr(ch4_index);
            g_ringBuf.buffer[4][idx] = RecoderFrameReturnPtr(ch5_index);
            g_ringBuf.buffer[5][idx] = RecoderFrameReturnPtr(ch6_index);
            g_ringBuf.buffer[6][idx] = RecoderFrameReturnPtr(ch7_index);

            break;
    }

    // 原子性更新：先写数据，再更新 head 和 count
    g_ringBuf.head = (idx + 1) % RING_BUF_SIZE;
    g_ringBuf.count++;
}






















