
MEMORY
{
    PAGE 0:    /* Program Memory */

    PAGE 1:    /* Data Memory */
    /* 片外RAM IS61WV102416BLL-10TLI 起始地址 1024K X 16 */
    EX_RAM                     : origin = 0x100000, length = 0x100000     /* 片外 RAM */

    FPGA_VERSION               : origin = 0x000400, length = 0x000001     /* FPGA 程序版本 */
    EX_ADC_RESULT              : origin = 0x000401, length = 0x000012     /* 18 路ADC采样结果 */
    HW_ERROR_SIGNAL_BIT_STATE  : origin = 0x000413, length = 0x000003     /* 硬件故障寄存器,共3个 */
    HW_DI_INPUT_STATE          : origin = 0x000416, length = 0x000001     /* DI信号输入 */

    HW_DI_INPUT_STATE_DLY      : origin = 0x000500, length = 0x000001     /* DI信号输入滤波时间 */
    HW_ERROR_STATE_RST         : origin = 0x000501, length = 0x000001     /* 硬件故障复位 */
    PWM_OUTPUT_CTRL            : origin = 0x000502, length = 0x000001     /* PWM输出使能 */
    FAN_CTRL_PWM1              : origin = 0x000503, length = 0x000001     /* 风扇1控制占空比输出 */
    FAN_CTRL_PWM2              : origin = 0x000504, length = 0x000001     /* 风扇2控制占空比输出 */
    HW_DO_OUTPUT_STATE         : origin = 0x000505, length = 0x000001     /* DO信号输出 */
    HW_ERROR_SIGNAL            : origin = 0x000506, length = 0x000001     /* 硬件故障封锁使能 */
    HW_ERROR_SIGNAL_BIT_SET    : origin = 0x000507, length = 0x000003     /* 硬件故障寄存器有效电平设置，共3个*/
    HW_IGBT_ERROR_SIGNAL       : origin = 0x00050A, length = 0x000001     /* IGBT硬件故障保护使能设置 */
    HW_ERROR_SIGNAL_BIT_CTRL   : origin = 0x00050B, length = 0x000003     /* 硬件故障寄存器使能设置，共3个 */
}

SECTIONS
{
    /*** Peripheral Frame 0 Register Structures ***/
    roFPGA_VersionFile             :> FPGA_VERSION,               PAGE = 1
    roEXADC_ResultFile             :> EX_ADC_RESULT,              PAGE = 1
    roHW_ErrorSignalBitStateFile   :> HW_ERROR_SIGNAL_BIT_STATE,  PAGE = 1
    roHW_DIInputStateFile          :> HW_DI_INPUT_STATE,          PAGE = 1

    woHW_DIInputStateDlyFlie       :> HW_DI_INPUT_STATE_DLY,      PAGE = 1
    woHW_ErrorStateRstFile         :> HW_ERROR_STATE_RST,         PAGE = 1
    woPWM_OutputCtrlFile           :> PWM_OUTPUT_CTRL,            PAGE = 1
    woFAN_CtrlPwm1File             :> FAN_CTRL_PWM1,              PAGE = 1
    woFAN_CtrlPwm2File             :> FAN_CTRL_PWM2,              PAGE = 1
    woHW_DOOutputStateFile         :> HW_DO_OUTPUT_STATE,         PAGE = 1
    woHW_ErrorSignalFile           :> HW_ERROR_SIGNAL,            PAGE = 1
    woHW_ErrorSignalBitSetFile     :> HW_ERROR_SIGNAL_BIT_SET,    PAGE = 1
    woHW_IGBT_ErrorSignalFile      :> HW_IGBT_ERROR_SIGNAL,       PAGE = 1
    woHW_ErrorSignalBitCtrlFile    :> HW_ERROR_SIGNAL_BIT_CTRL,   PAGE = 1

    rwEX_RAMFile                   :> EX_RAM,                     PAGE = 1
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
