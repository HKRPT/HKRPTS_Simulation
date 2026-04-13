#include "CANopen.h"
#include "main.h"
#include "string.h"

#define TMR_TASK_INTERVAL   (1000)          /* Interval of tmrTask thread in microseconds */
#define INCREMENT_1MS(var)  (var++)         /* Increment 1ms variable in tmrTask */


struct CANbase
{
    uintptr_t baseAddress;
};

uint16_t   CO_timer1ms = 0x00;
CO_NMT_reset_cmd_t reset = CO_RESET_NOT;    // 初始化状态

void CanOpen_Contorl(void)
{
    extern void CO_CAN1InterruptHandler(void);
    /* loop for normal program execution ******************************************/
    static uint16_t timer1msCopy = 0;
    static uint16_t timer1msDiff = 0;
    static uint16_t timer1msPrevious = 0;

    if(reset != CO_RESET_NOT) return;

    timer1msCopy = CO_timer1ms;
    timer1msDiff = timer1msCopy - timer1msPrevious;
    timer1msPrevious = timer1msCopy;

    /* CANopen 处理 */
//    reset = CO_process(CO, timer1msDiff, NULL);

    CO_CAN1InterruptHandler();  // 接收处理
}

// CANOPEN
void CANOpenLoop(void)
{
//    extern control_t    control;
    static uint16_t CANopenInit_flag = 0x00;
//    if(control.CID == 0)       return;

    if(reset == CO_RESET_APP | CANopenInit_flag == 0){
        CANopenInit_flag = 0x01;
        struct CANbase canBase = {.baseAddress = 0u,};  /* CAN module address */
        /* initialize CANopen */
        CO_init(&canBase, 1/* NodeID */, 500 /* bit rate */);
//        CO_init(&canBase, control.CID/* NodeID */, 500 /* bit rate */);
        /* start CAN */
        CO_CANsetNormalMode(CO->CANmodule[0]);
        reset = CO_RESET_NOT;
    }
    // CanOpen 控制函数
    CanOpen_Contorl();
}

// 1ms 信息同步
void tmrTask_thread(void)
{
    INCREMENT_1MS(CO_timer1ms);

    if(CO->CANmodule[0]->CANnormal)
    {
        bool_t syncWas;

        /* Process Sync */
        syncWas = CO_process_SYNC(CO, TMR_TASK_INTERVAL);

        /* Read inputs */
        CO_process_RPDO(CO, syncWas);

        /* Write outputs */
        CO_process_TPDO(CO, syncWas, TMR_TASK_INTERVAL);
    }
}

void CO_CAN1InterruptHandler(void)
{
    CO_CANinterrupt(CO->CANmodule[0]);
}
