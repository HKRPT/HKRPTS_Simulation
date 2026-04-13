#include "my_can.h"
#include "main.h"

volatile struct CAN_REGS *CAN_R[2] = {&CanaRegs, &CanbRegs};


// CAN波特率设置
void CANBitRateSet(uint32_t base, uint32_t clockFreq, uint32_t bitRate,
               uint16_t bitTime)
{
    uint16_t brp;
    uint16_t tPhase;
    uint16_t phaseSeg2;
    uint16_t tSync = 1U;
    uint16_t tProp = 2U;
    uint16_t tSeg1;
    uint16_t tSeg2;
    uint16_t sjw;
    uint16_t prescaler;
    uint16_t prescalerExtension;

    //
    // Check the arguments.
    //
    ASSERT(CAN_isBaseValid(base));
    ASSERT((bitTime > 7U) && (bitTime < 26U));
    ASSERT(bitRate <= 1000000U);

    //
    // Calculate bit timing values
    //
    brp = (uint16_t)(clockFreq / (bitRate * bitTime));
    tPhase = bitTime - (tSync + tProp);
    if((tPhase / 2U) <= 8U)
    {
        phaseSeg2 = tPhase / 2U;
    }
    else
    {
        phaseSeg2 = 8U;
    }
    tSeg1 = ((tPhase - phaseSeg2) + tProp) - 1U;
    tSeg2 = phaseSeg2 - 1U;
    if(phaseSeg2 > 4U)
    {
        sjw = 3U;
    }
    else
    {
        sjw = tSeg2;
    }
    prescalerExtension = ((brp - 1U) / 64U);
    prescaler = ((brp - 1U) % 64U);

    //
    // Set the calculated timing parameters
    //
    CAN_setBitTiming(base, prescaler, prescalerExtension, tSeg1, tSeg2, sjw);
}

typedef struct
{
    uint16_t    port;   // CAN 类型
    uint16_t    msgID;  // 消息ID
    uint16_t    msgMask;// 消息掩码
    uint16_t    msgLen; // 消息长度
    uint16_t    mailID; // 邮箱ID
    uint16_t    extEn;  // 扩展帧
}CAN_Init_t;

// 接收邮箱初始化
void myCAN_receiveMailBoxesInit(CAN_Init_t *can)
{
    uint32_t port = can->port ;


    while(CAN_R[port]->CAN_IF1CMD.bit.Busy);    // 等待接口1空闲

    CAN_R[port]->CAN_IF1CMD.bit.MSG_NUM = can->mailID;   // 邮箱0

    CAN_R[port]->CAN_IF1CMD.all         = 0;    // 清空

    CAN_R[port]->CAN_IF1CMD.bit.DIR     = 1;    // 从IF到RAM
    CAN_R[port]->CAN_IF1CMD.bit.Mask    = 1;    // 更新掩码寄存器
    CAN_R[port]->CAN_IF1CMD.bit.Arb     = 1;    // 更新仲裁寄存器
    CAN_R[port]->CAN_IF1CMD.bit.Control = 1;    // 更新控制寄存器
    CAN_R[port]->CAN_IF1CMD.bit.ClrIntPnd = 1;  // 写模式下无效
    CAN_R[port]->CAN_IF1CMD.bit.TXRQST  = 0;
    CAN_R[port]->CAN_IF1CMD.bit.DATA_A  = 0;    // 不更新数据寄存器A
    CAN_R[port]->CAN_IF1CMD.bit.DATA_B  = 0;    // 不更新数据寄存器B

    // 掩码寄存器
    CAN_R[port]->CAN_IF1MSK.all         = 0;
    CAN_R[port]->CAN_IF1MSK.bit.MXtd    = 1;    // 扩展ID掩码  使能
    CAN_R[port]->CAN_IF1MSK.bit.MDir    = 0;    // 邮箱DIR位掩码 禁止
    CAN_R[port]->CAN_IF1MSK.bit.Msk     = can->msgMask; //

    // 仲裁寄存器
    CAN_R[port]->CAN_IF1ARB.all         = 0;
    CAN_R[port]->CAN_IF1ARB.bit.MsgVal  = 1;            // 邮箱使能 修改控制寄存器XTD DIR DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF1ARB.bit.Xtd     = can->extEn;   // 扩展帧模式
    CAN_R[port]->CAN_IF1ARB.bit.Dir     = 0;            // 接收邮箱
    CAN_R[port]->CAN_IF1ARB.bit.ID      = can->mailID;  // 邮箱ID 需补充设备ID

    // 控制寄存器
    CAN_R[port]->CAN_IF1MCTL.all = 0;
    CAN_R[port]->CAN_IF1MCTL.bit.NewDat = 0;    // 未向邮箱RAM写入新数据
    CAN_R[port]->CAN_IF1MCTL.bit.MsgLst = 0;    // 写模式下无效
    CAN_R[port]->CAN_IF1MCTL.bit.IntPnd = 0;    // 写模式下无效
    CAN_R[port]->CAN_IF1MCTL.bit.UMask  = 1;    // 使能掩码
    CAN_R[port]->CAN_IF1MCTL.bit.TxIE   = 0;    // 发送中断 禁止
    CAN_R[port]->CAN_IF1MCTL.bit.RxIE   = 0;    // 接收中断 禁止
    CAN_R[port]->CAN_IF1MCTL.bit.RmtEn  = 1;    // 远程帧将使TxRqst置位
    CAN_R[port]->CAN_IF1MCTL.bit.EoB    = 1;    // 单消息对象
    CAN_R[port]->CAN_IF1MCTL.bit.DLC    = can->msgLen;      // 数据长度为8

    CAN_R[port]->CAN_IF1CMD.bit.MSG_NUM = can->mailID;      // 执行

    CAN_R[port]->CAN_IF3UPD |= 1 << (can->mailID - 1);      // IF3 自动接收邮箱1

    CAN_R[port]->CAN_IF3OBS.all         = can->msgLen;

    CAN_R[port]->CAN_IF3OBS.bit.Data_B  = 1;    // IF3 DMA不必须读 Data_B
    CAN_R[port]->CAN_IF3OBS.bit.Data_A  = 1;    // IF3 DMA不必须读 Data_A
    CAN_R[port]->CAN_IF3OBS.bit.Ctrl    = 1;    // IF3 DMA不必须读 Ctrl
    CAN_R[port]->CAN_IF3OBS.bit.Arb     = 1;    // IF3 DMA不必须读 Arb
    CAN_R[port]->CAN_IF3OBS.bit.Mask    = 1;    // IF3 DMA不必须读 Mask
}

// 发送邮箱初始化
void myCAN_sendMailBoxesInit(CAN_Init_t *can)
{
    uint32_t port = can->port ;

    while(CAN_R[port]->CAN_IF2CMD.bit.Busy);    // 等待接口2空闲

    CAN_R[port]->CAN_IF2CMD.all         = 0;    // 清空
    CAN_R[port]->CAN_IF2DATA.all        = 0;    // 清空
    CAN_R[port]->CAN_IF2DATB.all        = 0;    // 清空

    CAN_R[port]->CAN_IF2CMD.bit.DIR     = 1;    // 从IF到RAM
    CAN_R[port]->CAN_IF2CMD.bit.Mask    = 1;    // 更新掩码寄存器
    CAN_R[port]->CAN_IF2CMD.bit.Arb     = 1;    // 更新仲裁寄存器
    CAN_R[port]->CAN_IF2CMD.bit.Control = 1;    // 更新控制寄存器
    CAN_R[port]->CAN_IF2CMD.bit.ClrIntPnd = 0;  // 写模式下无效
    CAN_R[port]->CAN_IF2CMD.bit.TxRqst  = 0;
    CAN_R[port]->CAN_IF2CMD.bit.DATA_A  = 1;    // 不更新数据寄存器A
    CAN_R[port]->CAN_IF2CMD.bit.DATA_B  = 1;    // 不更新数据寄存器B

    // 数据寄存器
    CAN_R[port]->CAN_IF2DATA.bit.Data_0 = 0;
    CAN_R[port]->CAN_IF2DATA.bit.Data_1 = 0;
    CAN_R[port]->CAN_IF2DATA.bit.Data_2 = 0;
    CAN_R[port]->CAN_IF2DATA.bit.Data_3 = 0;
    CAN_R[port]->CAN_IF2DATB.bit.Data_4 = 0;
    CAN_R[port]->CAN_IF2DATB.bit.Data_5 = 0;
    CAN_R[port]->CAN_IF2DATB.bit.Data_6 = 0;
    CAN_R[port]->CAN_IF2DATB.bit.Data_7 = 0;


    // 掩码寄存器
    CAN_R[port]->CAN_IF2MSK.all         = 0;
    CAN_R[port]->CAN_IF2MSK.bit.MXtd    = 1;            // 扩展ID掩码  使能
    CAN_R[port]->CAN_IF2MSK.bit.MDir    = 0;            // 邮箱DIR位掩码 禁止
    CAN_R[port]->CAN_IF2MSK.bit.Msk     = can->msgMask; // 0 ~ 29bit必须一致

    // 仲裁寄存器
    CAN_R[port]->CAN_IF2ARB.all         = 0;
    CAN_R[port]->CAN_IF2ARB.bit.MsgVal  = 1;            // 邮箱使能 修改控制寄存器XTD DIR DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF2ARB.bit.Xtd     = can->extEn;   // 扩展帧模式
    CAN_R[port]->CAN_IF2ARB.bit.Dir     = 1;            // 设置邮箱为发送模式
    CAN_R[port]->CAN_IF2ARB.bit.ID      = can->mailID;  // 邮箱ID 需补充设备ID

    // 控制寄存器
    CAN_R[port]->CAN_IF2MCTL.all = 0;
    CAN_R[port]->CAN_IF2MCTL.bit.NewDat = 0;    // 未向邮箱RAM写入新数据
    CAN_R[port]->CAN_IF2MCTL.bit.MsgLst = 0;    // 写模式下无效
    CAN_R[port]->CAN_IF2MCTL.bit.IntPnd = 0;    // 写模式下无效
    CAN_R[port]->CAN_IF2MCTL.bit.UMask  = 1;    // 使能掩码
    CAN_R[port]->CAN_IF2MCTL.bit.TxIE   = 0;    // 发送中断 禁止
    CAN_R[port]->CAN_IF2MCTL.bit.RxIE   = 0;    // 接收中断 禁止
    CAN_R[port]->CAN_IF2MCTL.bit.RmtEn  = 0;    // 远程帧将使TxRqst置位
    CAN_R[port]->CAN_IF2MCTL.bit.EoB    = 1;    // 单消息对象

    CAN_R[port]->CAN_IF2MCTL.bit.DLC    = can->msgLen;  // 数据长度为8

    CAN_R[port]->CAN_IF2CMD.bit.MSG_NUM = can->mailID;  // 执行 邮箱6
}

// CAN初始化
void My_CAN_Init(uint16_t port)
{
    CAN_Init_t can;
    EALLOW;
    ClkCfgRegs.CLKSRCCTL2.bit.CANABCLKSEL = 0;  // 时钟源设置 CPUx.SYSCLK = 100M
    EDIS;

    // 重新初始化后CAN控制寄存器之前必须禁止所有邮箱
    CAN_R[port]->CAN_CTL.bit.Init = 1;          // 初始化CAN模块 (屏蔽总线活动)

    EALLOW;
    CAN_R[port]->CAN_CTL.bit.SWR  = 1;          // 软件复位
    EDIS;
    EALLOW;
    CAN_R[port]->CAN_CTL.bit.IE1  = 0;          // 禁用CAN INT1
    CAN_R[port]->CAN_CTL.bit.ABO  = 1;          // 启用总线自动恢复  用户自定义延迟启动周期
    CAN_R[port]->CAN_CTL.bit.IDS  = 1;          // 收到Debug请求 立刻中断接收发送，进入debug模式
    CAN_R[port]->CAN_CTL.bit.Test = 0;          // 禁用Test模式
    CAN_R[port]->CAN_CTL.bit.DAR  = 0;          // 禁止关闭自动重传(发送失败自动重传)
    CAN_R[port]->CAN_CTL.bit.EIE  = 0;          // 禁止 PER BOff EWarn 中断
    CAN_R[port]->CAN_CTL.bit.SIE  = 0;          // 禁止 RxOK TxOK LEC中断
    CAN_R[port]->CAN_CTL.bit.CCE  = 1;          // CAN 配置信息已修改
    CAN_R[port]->CAN_CTL.bit.IE0  = 0;          // 禁用CAN INT0
    EDIS;

    // 100Mhz运行频率 500K设置波特率CAN
    CANBitRateSet((uint32_t)CAN_R[port], 200e6, 500e3, 20);
    CAN_R[port]->CAN_ABOTR = 0;                 // 总线恢复时间

    // 必须将所有邮箱MsgVal值清零
    CAN_R[port]->CAN_RAM_INIT.all = 0x1A;       // 邮箱RAM初始化
    while(CAN_R[port]->CAN_RAM_INIT.bit.RAM_INIT_DONE != 1)
    {
        // 等待邮箱RAM初始化完成
    }

    CAN_R[port]->CAN_GLB_INT_EN.bit.GLBINT0_EN = 0; // 禁止CANINT0 挂到PIE
    CAN_R[port]->CAN_GLB_INT_EN.bit.GLBINT1_EN = 0; // 禁止CANINT1 挂到PIE

    /*      接收邮箱        */
    can.port    = port;         // CAN端口
    can.msgLen  = 8;            // 消息长度
    can.msgMask = 0x00000007;   // 消息掩码
    can.msgID   = 0x00000001;   // 消息ID
    can.mailID  = 1;            // 邮箱ID
    can.extEn   = 1;            // 扩展帧模式
    myCAN_receiveMailBoxesInit(&can);       // 邮箱1

    can.msgID   = 0x00000002;   // 消息ID
    can.mailID  = 2;            // 邮箱ID
    myCAN_receiveMailBoxesInit(&can);       // 邮箱2

    can.msgID   = 0x00000003;   // 消息ID
    can.mailID  = 3;            // 邮箱ID
    myCAN_receiveMailBoxesInit(&can);       // 邮箱3

    can.msgID   = 0x00000004;   // 消息ID
    can.mailID  = 4;            // 邮箱ID
    myCAN_receiveMailBoxesInit(&can);       // 邮箱4

    can.msgID   = 0x00000005;   // 消息ID
    can.mailID  = 5;            // 邮箱ID
    can.extEn   = 0;            // 标准帧模式
    myCAN_receiveMailBoxesInit(&can);       // 邮箱5

    /*      发送邮箱        */
    can.port    = port;             // CAN端口
    can.msgLen  = 8;                // 消息长度
    can.msgMask = 0xFFFFFFFF;       // 消息掩码
    can.msgID   = 0x00000000;       // 消息ID
    can.mailID  = 6;                // 邮箱ID
    can.extEn   = 1;                // 扩展帧模式
    myCAN_sendMailBoxesInit(&can);  // 邮箱6

    can.mailID  = 7;                // 邮箱ID
    can.extEn   = 0;                // 标准帧模式
    myCAN_sendMailBoxesInit(&can);  // 邮箱7

    can.mailID  = 8;                // 邮箱ID
    can.extEn   = 0;                // 标准帧模式
    myCAN_sendMailBoxesInit(&can);  // 邮箱8

    CAN_R[port]->CAN_CTL.bit.Init   = 0;// 确认是否得在mailBox初始化后
}



// 邮箱5 任意ID发送，发送数据8字节 标准帧
uint16_t My_CAN_SendStdCMD(uint16_t port, uint32_t msgID, uint8_t *buff)
{
    if(CAN_R[port]->CAN_IF2CMD.bit.Busy || (CAN_R[port]->CAN_TXRQ_21 & (1 << (5-1)))) return 0x01; // 等待接口2 空闲, bit4 =5邮箱
//    CAN_R[port]->CAN_IF2CMD.bit.MSG_NUM = 0;    // 重新定向邮箱

    union CAN_IF2CMD_REG IF_CMD;

    IF_CMD.all = 0;
    IF_CMD.bit.DIR = 1;         // 向邮箱写数据
    IF_CMD.bit.DATA_A   = 1;    // 更新数据寄存器A
    IF_CMD.bit.DATA_B   = 1;    // 更新数据寄存器B
    IF_CMD.bit.Arb      = 1;    // 更新ID
    IF_CMD.bit.TxRqst   = 1;    // 请求发送
    IF_CMD.bit.MSG_NUM  = 5;    // 向邮箱5发送数据

    CAN_R[port]->CAN_IF2ARB.all         = 0;
    CAN_R[port]->CAN_IF2ARB.bit.MsgVal  = 1;    // 邮箱使能 修改控制寄存器 Xtd Dir DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF2ARB.bit.Xtd     = 0;    // 扩展帧模式
    CAN_R[port]->CAN_IF2ARB.bit.Dir     = 1;    // 发送邮箱
    CAN_R[port]->CAN_IF2ARB.bit.ID      = (msgID << 18);// 邮箱ID

    // 数据寄存器
    CAN_R[port]->CAN_IF2DATA.bit.Data_0 = buff[0];
    CAN_R[port]->CAN_IF2DATA.bit.Data_1 = buff[1];
    CAN_R[port]->CAN_IF2DATA.bit.Data_2 = buff[2];
    CAN_R[port]->CAN_IF2DATA.bit.Data_3 = buff[3];
    CAN_R[port]->CAN_IF2DATB.bit.Data_4 = buff[4];
    CAN_R[port]->CAN_IF2DATB.bit.Data_5 = buff[5];
    CAN_R[port]->CAN_IF2DATB.bit.Data_6 = buff[6];
    CAN_R[port]->CAN_IF2DATB.bit.Data_7 = buff[7];

    CAN_R[port]->CAN_IF2CMD.all = IF_CMD.all;


    return 0x00;
}

// 邮箱6 任意ID发送，发送数据8字节
uint16_t My_CAN_SendCMD(uint16_t port, uint32_t msgID, uint8_t *buff)
{
    if(CAN_R[port]->CAN_IF2CMD.bit.Busy || (CAN_R[port]->CAN_TXRQ_21 & (1 << (6-1)))) return 0x01; // 等待接口2 空闲, bit5 =6邮箱
//    CAN_R[port]->CAN_IF2CMD.bit.MSG_NUM = 0;    // 重新定向邮箱

    union   CAN_IF2CMD_REG  IF2CMD;

    IF2CMD.all = 0;
    IF2CMD.bit.DIR = 1;         // 向邮箱写数据
    IF2CMD.bit.DATA_A   = 1;    // 更新数据寄存器A
    IF2CMD.bit.DATA_B   = 1;    // 更新数据寄存器B
    IF2CMD.bit.Arb      = 1;    // 更新ID
    IF2CMD.bit.TxRqst   = 1;    // 请求发送
    IF2CMD.bit.MSG_NUM  = 6;    // 向邮箱6发送数据


    CAN_R[port]->CAN_IF2ARB.all         = 0;
    CAN_R[port]->CAN_IF2ARB.bit.MsgVal  = 1;    // 邮箱使能 修改控制寄存器 Xtd Dir DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF2ARB.bit.Xtd     = 1;    // 扩展帧模式
    CAN_R[port]->CAN_IF2ARB.bit.Dir     = 1;    // 发送邮箱
    CAN_R[port]->CAN_IF2ARB.bit.ID      = msgID;// 邮箱ID

    // 数据寄存器
    CAN_R[port]->CAN_IF2DATA.bit.Data_0 = buff[0];
    CAN_R[port]->CAN_IF2DATA.bit.Data_1 = buff[1];
    CAN_R[port]->CAN_IF2DATA.bit.Data_2 = buff[2];
    CAN_R[port]->CAN_IF2DATA.bit.Data_3 = buff[3];
    CAN_R[port]->CAN_IF2DATB.bit.Data_4 = buff[4];
    CAN_R[port]->CAN_IF2DATB.bit.Data_5 = buff[5];
    CAN_R[port]->CAN_IF2DATB.bit.Data_6 = buff[6];
    CAN_R[port]->CAN_IF2DATB.bit.Data_7 = buff[7];

    CAN_R[port]->CAN_IF2CMD.all = IF2CMD.all;

    return 0x00;
}

// 邮箱7 任意ID发送，任意长度发送  CANOPEN发送标准帧
uint16_t My_CAN_SendCANopenSingleFrame(uint16_t port, uint32_t msgID, uint8_t len ,uint8_t *buff)
{
    if(CAN_R[port]->CAN_IF2CMD.bit.Busy || (CAN_R[port]->CAN_TXRQ_21 & (1 << (7-1)))) return 0x01; // 等待接口2 空闲, bit5 =7邮箱
    if(len > 8) len = 8;

    union   CAN_IF2CMD_REG  IF2CMD;

    IF2CMD.all = 0;
    IF2CMD.bit.DIR = 1;         // 向邮箱写数据
    IF2CMD.bit.DATA_A   = 1;    // 更新数据寄存器A
    IF2CMD.bit.DATA_B   = 1;    // 更新数据寄存器B
    IF2CMD.bit.Arb      = 1;    // 更新ID
    IF2CMD.bit.TxRqst   = 1;    // 请求发送
    IF2CMD.bit.MSG_NUM  = 7;    // 向邮箱8发送数据
    IF2CMD.bit.Control  = 1;    // 更新控制寄存器
    IF2CMD.bit.Arb      = 1;    // 更新仲裁寄存器
    while(CAN_R[port]->CAN_IF2CMD.bit.Busy);    // 等待接口2空闲

    // 仲裁寄存器
    CAN_R[port]->CAN_IF2ARB.all         = 0;
    CAN_R[port]->CAN_IF2ARB.bit.MsgVal  = 1;    // 邮箱使能 修改控制寄存器XTD DIR DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF2ARB.bit.Xtd     = 0;    // 标准帧模式
    CAN_R[port]->CAN_IF2ARB.bit.Dir     = 1;    // 设置邮箱为发送模式
    CAN_R[port]->CAN_IF2ARB.bit.ID      = msgID << 18;// 邮箱ID 需补充设备ID

    // 控制寄存器
    CAN_R[port]->CAN_IF2MCTL.all = 0;
    CAN_R[port]->CAN_IF2MCTL.bit.NewDat = 0;    // 未向邮箱RAM写入新数据
    CAN_R[port]->CAN_IF2MCTL.bit.MsgLst = 0;    // 写模式下无效
    CAN_R[port]->CAN_IF2MCTL.bit.IntPnd = 0;    // 写模式下无效
    CAN_R[port]->CAN_IF2MCTL.bit.UMask  = 1;    // 使能掩码
    CAN_R[port]->CAN_IF2MCTL.bit.TxIE   = 0;    // 发送中断 禁止
    CAN_R[port]->CAN_IF2MCTL.bit.RxIE   = 0;    // 接收中断 禁止
    CAN_R[port]->CAN_IF2MCTL.bit.RmtEn  = 0;    // 远程帧将使TxRqst置位
    CAN_R[port]->CAN_IF2MCTL.bit.EoB    = 1;    // 单消息对象
    CAN_R[port]->CAN_IF2MCTL.bit.DLC    = len;    // 数据长度为8

    // 数据寄存器
    CAN_R[port]->CAN_IF2DATA.bit.Data_0 = buff[0];
    CAN_R[port]->CAN_IF2DATA.bit.Data_1 = buff[1];
    CAN_R[port]->CAN_IF2DATA.bit.Data_2 = buff[2];
    CAN_R[port]->CAN_IF2DATA.bit.Data_3 = buff[3];
    CAN_R[port]->CAN_IF2DATB.bit.Data_4 = buff[4];
    CAN_R[port]->CAN_IF2DATB.bit.Data_5 = buff[5];
    CAN_R[port]->CAN_IF2DATB.bit.Data_6 = buff[6];
    CAN_R[port]->CAN_IF2DATB.bit.Data_7 = buff[7];

    CAN_R[port]->CAN_IF2CMD.all = IF2CMD.all;

    return 0x00;
}
// 邮箱8 任意ID发送，发送数据8字节  CANOPEN发送远程帧
uint16_t My_CAN_SendCANopenRemoteFrame(uint16_t port, uint32_t msgID, uint8_t *buff)
{
    if(CAN_R[port]->CAN_IF2CMD.bit.Busy || (CAN_R[port]->CAN_TXRQ_21 & (1 << (8-1)))) return 0x01; // 等待接口2 空闲, bit5 =8邮箱
//    CAN_R[port]->CAN_IF2CMD.bit.MSG_NUM = 0;    // 重新定向邮箱

    union   CAN_IF2CMD_REG  IF2CMD;

    IF2CMD.all = 0;
    IF2CMD.bit.DIR = 1;         // 向邮箱写数据
    IF2CMD.bit.DATA_A   = 1;    // 更新数据寄存器A
    IF2CMD.bit.DATA_B   = 1;    // 更新数据寄存器B
    IF2CMD.bit.Arb      = 1;    // 更新ID
    IF2CMD.bit.TxRqst   = 1;    // 请求发送
    IF2CMD.bit.MSG_NUM  = 8;    // 向邮箱8发送数据


    CAN_R[port]->CAN_IF2ARB.bit.MsgVal  = 1;    // 邮箱使能 修改控制寄存器 Xtd Dir DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF2ARB.bit.Xtd     = 0;    // 标准帧模式
    CAN_R[port]->CAN_IF2ARB.bit.Dir     = 1;    // 发送邮箱
    CAN_R[port]->CAN_IF2ARB.bit.ID      = msgID;// 邮箱ID

    // 数据寄存器
    CAN_R[port]->CAN_IF2DATA.bit.Data_0 = buff[0];
    CAN_R[port]->CAN_IF2DATA.bit.Data_1 = buff[1];
    CAN_R[port]->CAN_IF2DATA.bit.Data_2 = buff[2];
    CAN_R[port]->CAN_IF2DATA.bit.Data_3 = buff[3];
    CAN_R[port]->CAN_IF2DATB.bit.Data_4 = buff[4];
    CAN_R[port]->CAN_IF2DATB.bit.Data_5 = buff[5];
    CAN_R[port]->CAN_IF2DATB.bit.Data_6 = buff[6];
    CAN_R[port]->CAN_IF2DATB.bit.Data_7 = buff[7];

    CAN_R[port]->CAN_IF2CMD.all = IF2CMD.all;

    return 0x00;
}

// 邮箱31 接收ID
uint16_t My_CAN_ReqID(uint16_t port, uint32_t msgID)
{
    if(CAN_R[port]->CAN_IF1CMD.bit.Busy || (CAN_R[port]->CAN_TXRQ_21 & 0x40000000)) return 0x01; // 等待接口1 空闲, bit5 =6邮箱
    CAN_R[port]->CAN_IF1CMD.bit.MSG_NUM = 0;    // 重新定向邮箱

    union CAN_IF1CMD_REG IF_CMD;

    IF_CMD.all = 0;
    IF_CMD.bit.DIR = 1;         // 向邮箱写数据
    IF_CMD.bit.DATA_A   = 1;    // 更新数据寄存器A
    IF_CMD.bit.DATA_B   = 1;    // 更新数据寄存器B
    IF_CMD.bit.Arb      = 1;    // 更新ID
    IF_CMD.bit.TXRQST   = 1;    // 请求发送
    IF_CMD.bit.MSG_NUM  = 31;   // 向邮箱x接收数据

    CAN_R[port]->CAN_IF1ARB.all         = 0;
    CAN_R[port]->CAN_IF1ARB.bit.MsgVal  = 1;    // 邮箱使能 修改控制寄存器 Xtd Dir DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF1ARB.bit.Xtd     = 1;    // 扩展帧模式
    CAN_R[port]->CAN_IF1ARB.bit.Dir     = 0;    // 接收邮箱
    CAN_R[port]->CAN_IF1ARB.bit.ID      = msgID;// 邮箱ID

    CAN_R[port]->CAN_IF1CMD.all = IF_CMD.all;

    return 0x00;
}

// 邮箱32 任意ID发送，发送数据8字节
uint16_t My_CAN_SetID(uint16_t port, uint32_t msgID, uint16_t *buff)
{
    if(CAN_R[port]->CAN_IF2CMD.bit.Busy || (CAN_R[port]->CAN_TXRQ_21 & 0x80000000)) return 0x01; // 等待接口2 空闲, bit31 =32邮箱
    CAN_R[port]->CAN_IF2CMD.bit.MSG_NUM = 0;    // 重新定向邮箱

    union CAN_IF2CMD_REG IF_CMD;

    IF_CMD.all = 0;
    IF_CMD.bit.DIR = 1;         // 向邮箱写数据
    IF_CMD.bit.DATA_A   = 1;    // 更新数据寄存器A
    IF_CMD.bit.DATA_B   = 1;    // 更新数据寄存器B
    IF_CMD.bit.Arb      = 1;    // 更新ID
    IF_CMD.bit.TxRqst   = 1;    // 请求发送
    IF_CMD.bit.MSG_NUM  = 32;   // 向邮箱32发送数据

    CAN_R[port]->CAN_IF2ARB.all         = 0;
    CAN_R[port]->CAN_IF2ARB.bit.MsgVal  = 1;    // 邮箱使能 修改控制寄存器 Xtd Dir DLC ID 或不需要邮箱 必须先禁止邮箱
    CAN_R[port]->CAN_IF2ARB.bit.Xtd     = 1;    // 扩展帧模式
    CAN_R[port]->CAN_IF2ARB.bit.Dir     = 1;    // 发送邮箱
    CAN_R[port]->CAN_IF2ARB.bit.ID      = msgID;// 邮箱ID

    // 数据寄存器
    CAN_R[port]->CAN_IF2DATA.bit.Data_0 = buff[0];
    CAN_R[port]->CAN_IF2DATA.bit.Data_1 = buff[1];
    CAN_R[port]->CAN_IF2DATA.bit.Data_2 = buff[2];
    CAN_R[port]->CAN_IF2DATA.bit.Data_3 = buff[3];
    CAN_R[port]->CAN_IF2DATB.bit.Data_4 = buff[4];
    CAN_R[port]->CAN_IF2DATB.bit.Data_5 = buff[5];
    CAN_R[port]->CAN_IF2DATB.bit.Data_6 = buff[6];
    CAN_R[port]->CAN_IF2DATB.bit.Data_7 = buff[7];

    CAN_R[port]->CAN_IF2CMD.all = IF_CMD.all;

    return 0x00;
}

void CAN_Tx_Msg(CanTxMsg *can_tx_msg)  //发送消息
{
    uint32_t CAN_BASE ;
    uint32_t CAN_id;
    uint16_t txMsgDataBuffer[8];
    CAN_MsgFrameType CANtype;
    if(can_tx_msg->CAN_num == CANA)
    {
        CAN_BASE = CANA_BASE;
    }
    else if (can_tx_msg->CAN_num == CANB)
    {
//        CAN_BASE = CANB_BASE;     // F280025 只有一路CAN
        return;
    }
    else return;
    if(can_tx_msg->IDE == CAN_ID_STD)
    {
        CAN_id = can_tx_msg->StdId.bit.StdId; //standard identifier
        CANtype = CAN_MSG_FRAME_STD;
    }
    else if(can_tx_msg->IDE == CAN_ID_EXT)
    {
        if(can_tx_msg->SAE_J1939_Flag == 0)
        {
            CAN_id = can_tx_msg->ExtId.bit.ExtId; //extended identifier.
            CANtype = CAN_MSG_FRAME_EXT;
        }
        else
        {
            CAN_id = can_tx_msg->SAE_J1939_ID.all; //extended identifier.
            CANtype = CAN_MSG_FRAME_EXT;
        }
    }

    CAN_setupMessageObject(CAN_BASE, can_tx_msg->MBox_num, CAN_id, CANtype,
                                   CAN_MSG_OBJ_TYPE_TX, 0, CAN_MSG_OBJ_TX_INT_ENABLE,
                                   can_tx_msg->DLC);
    txMsgDataBuffer[0] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte0;
    txMsgDataBuffer[1] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte1;
    txMsgDataBuffer[2] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte2;
    txMsgDataBuffer[3] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte3;
    txMsgDataBuffer[4] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte4;
    txMsgDataBuffer[5] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte5;
    txMsgDataBuffer[6] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte6;
    txMsgDataBuffer[7] = can_tx_msg->CAN_Tx_msg_data.msg_Byte.byte7;
    CAN_sendMessage(CANA_BASE, can_tx_msg->MBox_num, can_tx_msg->DLC,txMsgDataBuffer);
}

// CAN 初始化
void CAN_Init(void)
{
    My_CAN_Init(0);         // CANA
    My_CAN_Init(1);         // CANB
}
