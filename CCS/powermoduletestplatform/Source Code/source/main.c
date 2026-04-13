#include "main.h"
#include "w5500.h"
#include "my_sci.h"
#include "modbus.h"
#include "bootloader.h"
#include "my_adcContorl.h"


extern uint8_t bootMode; // boot模式 ： 0为APP, 1为Boot

void main(void)
{
    Device_init();

    Device_initGPIO();

    Interrupt_initModule();

    Interrupt_initVectorTable();

    Board_init();

    /*  外设初始化   */
    Peripheral_Init();
    /*  FPGA        */
    FPGA_Init();

    EINT;
    ERTM;

    /*  网络配置    */
    W5500_NetWorkConfig();

    while(1)
    {
        if(bootMode == 0)
        {
            // APP 模式下运行
            FLASH_Test();
            FRAM_Contorl();
            My_Control_Loop();
            Recode_loop();      // 录波循环

            BootLoaderReadFlashToComplateCRC();
            APP_BackupToFLASH();                    // APP  备份程序
            W5500_Test();
            UART_CCC();
            modbusMaster();     // modbus主站函数
            w5500_modbusMaster();
            CANOpenLoop();      // CanOpen 函数
            NTC_Loop();
        }
        else if(bootMode == 0x02)
        {
            W5500_Test();         // 以太网更新时只有它运行
            w5500_modbusMaster();
            modbusMaster();     // modbus主站函数
        }
        else
        {
            UART_CCC();
            CANOpenLoop();      // CanOpen 函数
            modbusMaster();     // modbus主站函数
        }
    }
}
