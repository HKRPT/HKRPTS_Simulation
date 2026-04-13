#include "F28x_Project.h" // 包含所有外设寄存器头文件

void main(void)
{
    // 1. 初始化系统控制：PLL，看门狗，启用外设时钟
    InitSysCtrl();

    // 2. 初始化 GPIO：将所有引脚设置为默认状态
    InitGpio();

    // 3. 清除所有中断并初始化 PIE 向量表 (可选，但建议保留)
    DINT; // 禁用 CPU 中断
    InitPieCtrl(); // 初始化 PIE 控制寄存器到默认状态
    IER = 0x0000;  // 禁用 CPU 中断
    IFR = 0x0000;  // 清除所有 CPU 中断标志

    InitPieVectTable(); // 初始化 PIE 向量表

    // 4. 启用全局中断和高优先级实时调试事件
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    // 5. 主循环
    while(1)
    {
        // 你的代码逻辑
    }
}
