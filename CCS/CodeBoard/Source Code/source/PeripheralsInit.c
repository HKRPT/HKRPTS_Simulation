#include "main.h"
#include "my_sci.h"
#include "my_epwm.h"
#include "my_can.h"

// GPIO初始化
void Peripheral_GPIO_Init(void)
{
    /*  运行灯  */
    GPIO_setPinConfig(RunLed1GPIOMUX);
	GPIO_setPadConfig(RunLed1GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(RunLed1GPIO, GPIO_QUAL_SYNC);
	GPIO_setDirectionMode(RunLed1GPIO, GPIO_DIR_MODE_OUT);
	GPIO_setControllerCore(RunLed1GPIO, GPIO_CORE_CPU1);

	GPIO_setPinConfig(RunLed2GPIOMUX);
    GPIO_setPadConfig(RunLed2GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(RunLed2GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(RunLed2GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(RunLed2GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(RunLed3GPIOMUX);
    GPIO_setPadConfig(RunLed3GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(RunLed3GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(RunLed3GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(RunLed3GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(RunLed4GPIOMUX);
    GPIO_setPadConfig(RunLed4GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(RunLed4GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(RunLed4GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(RunLed4GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(RunLed5GPIOMUX);
    GPIO_setPadConfig(RunLed5GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(RunLed5GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(RunLed5GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(RunLed5GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(RunLed6GPIOMUX);
    GPIO_setPadConfig(RunLed6GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(RunLed6GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(RunLed6GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(RunLed6GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(RunLed7GPIOMUX);
    GPIO_setPadConfig(RunLed7GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(RunLed7GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(RunLed7GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(RunLed7GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(RunLed8GPIOMUX);
    GPIO_setPadConfig(RunLed8GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(RunLed8GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(RunLed8GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(RunLed8GPIO, GPIO_CORE_CPU1);

	/*  CAN 引脚初始化  */
    GPIO_setPinConfig(DSP_CANATx_GPIOMUX);
    GPIO_setPadConfig(DSP_CANATx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_CANATx_GPIO, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(DSP_CANARx_GPIOMUX);
    GPIO_setPadConfig(DSP_CANARx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_CANARx_GPIO, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DSP_CANBTx_GPIOMUX);
    GPIO_setPadConfig(DSP_CANBTx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_CANBTx_GPIO, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(DSP_CANBRx_GPIOMUX);
    GPIO_setPadConfig(DSP_CANBRx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_CANBRx_GPIO, GPIO_QUAL_ASYNC);

    /*  SPIB 引脚初始化  */
    GPIO_setPinConfig(SPIB_CS_GPIO_PIN_CONFIG);                     // SPIB CS
    GPIO_setPadConfig(SPIB_CS_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIB_CS_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIB_MISO_GPIO_PIN_CONFIG);                   // SPIB MISO
    GPIO_setPadConfig(SPIB_MISO_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIB_MISO_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIB_MOSI_GPIO_PIN_CONFIG);                   // SPIB MOSI
    GPIO_setPadConfig(SPIB_MOSI_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIB_MOSI_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIB_MCLK_GPIO_PIN_CONFIG);                   // SPIB MCLK
    GPIO_setPadConfig(SPIB_MCLK_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIB_MCLK_GPIO_PIN, GPIO_QUAL_ASYNC);

	/*  SPIC 引脚初始化  */
    GPIO_setPinConfig(SPIC_CS_GPIO_PIN_CONFIG);                     // SPIC CS
    GPIO_setPadConfig(SPIC_CS_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIC_CS_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIC_MISO_GPIO_PIN_CONFIG);                   // SPIC MISO
    GPIO_setPadConfig(SPIC_MISO_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIC_MISO_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIC_MOSI_GPIO_PIN_CONFIG);                   // SPIC MOSI
    GPIO_setPadConfig(SPIC_MOSI_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIC_MOSI_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIC_MCLK_GPIO_PIN_CONFIG);                   // SPIC MCLK
    GPIO_setPadConfig(SPIC_MCLK_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIC_MCLK_GPIO_PIN, GPIO_QUAL_ASYNC);

	/*  SPIA 引脚初始化  */
    GPIO_setPinConfig(SPIA_CS_GPIO_PIN_CONFIG);                     // SPIC CS
    GPIO_setPadConfig(SPIA_CS_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_CS_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIA_MISO_GPIO_PIN_CONFIG);                   // SPIC MISO
    GPIO_setPadConfig(SPIA_MISO_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_MISO_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIA_MOSI_GPIO_PIN_CONFIG);                   // SPIC MOSI
    GPIO_setPadConfig(SPIA_MOSI_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_MOSI_GPIO_PIN, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(SPIA_MCLK_GPIO_PIN_CONFIG);                   // SPIC MCLK
    GPIO_setPadConfig(SPIA_MCLK_GPIO_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(SPIA_MCLK_GPIO_PIN, GPIO_QUAL_ASYNC);



    /*  ETH1  */
    GPIO_setPinConfig(ETH1_INT_GPIOMUX);
    GPIO_setPadConfig(ETH1_INT_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(ETH1_INT_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(ETH1_INT_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(ETH1_INT_GPIO, GPIO_CORE_CPU1);

    /*  ETH2  */
    GPIO_setPinConfig(ETH2_INT_GPIOMUX);
    GPIO_setPadConfig(ETH2_INT_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(ETH2_INT_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(ETH2_INT_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(ETH2_INT_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(ETHx_RST_GPIOMUX);
    GPIO_setPadConfig(ETHx_RST_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(ETHx_RST_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(ETHx_RST_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(ETHx_RST_GPIO, GPIO_CORE_CPU1);

    // PWM1 引脚初始化
    GPIO_setPinConfig(DSP_PWM1A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM1A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM1A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM1B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM1B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM1B_GPIO, GPIO_QUAL_SYNC);

    // PWM2 引脚初始化
    GPIO_setPinConfig(DSP_PWM2A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM2A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM2A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM2B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM2B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM2B_GPIO, GPIO_QUAL_SYNC);

    // PWM3 引脚初始化
    GPIO_setPinConfig(DSP_PWM3A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM3A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM3A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM3B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM3B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM3B_GPIO, GPIO_QUAL_SYNC);

    // PWM4 引脚初始化
    GPIO_setPinConfig(DSP_PWM4A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM4A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM4A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM4B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM4B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM4B_GPIO, GPIO_QUAL_SYNC);

    // PWM5 引脚初始化
    GPIO_setPinConfig(DSP_PWM5A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM5A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM5A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM5B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM5B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM5B_GPIO, GPIO_QUAL_SYNC);

    // PWM6 引脚初始化
    GPIO_setPinConfig(DSP_PWM6A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM6A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM6A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM6B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM6B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM6B_GPIO, GPIO_QUAL_SYNC);

    // PWM7 引脚初始化
    GPIO_setPinConfig(DSP_PWM7A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM7A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM7A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM7B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM7B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM7B_GPIO, GPIO_QUAL_SYNC);

    // PWM8 引脚初始化
    GPIO_setPinConfig(DSP_PWM8A_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM8A_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM8A_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_PWM8B_GPIOMUX);
    GPIO_setPadConfig(DSP_PWM8B_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_PWM8B_GPIO, GPIO_QUAL_SYNC);

    // 串口1 引脚初始化
    GPIO_setPinConfig(DSP_sciATx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciATx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciATx_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_sciARx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciARx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciARx_GPIO, GPIO_QUAL_SYNC);

    // 串口2 引脚初始化
    GPIO_setPinConfig(DSP_sciBTx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciBTx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciBTx_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_sciBRx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciBRx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciBRx_GPIO, GPIO_QUAL_SYNC);

    // 串口3 引脚初始化
    GPIO_setPinConfig(DSP_sciCTx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciCTx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciCTx_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_sciCRx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciCRx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciCRx_GPIO, GPIO_QUAL_SYNC);

    // 串口4 引脚初始化
    GPIO_setPinConfig(DSP_sciDTx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciDTx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciDTx_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(DSP_sciDRx_GPIOMUX);
    GPIO_setPadConfig(DSP_sciDRx_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_sciDRx_GPIO, GPIO_QUAL_SYNC);

    // DI 引脚初始化
    GPIO_setPinConfig(DSP_DI0_GPIOMUX);
    GPIO_setPadConfig(DSP_DI0_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI0_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI0_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI0_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI1_GPIOMUX);
    GPIO_setPadConfig(DSP_DI1_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI1_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI1_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI1_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI2_GPIOMUX);
    GPIO_setPadConfig(DSP_DI2_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI2_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI2_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI2_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI3_GPIOMUX);
    GPIO_setPadConfig(DSP_DI3_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI3_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI3_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI3_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI4_GPIOMUX);
    GPIO_setPadConfig(DSP_DI4_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI4_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI4_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI4_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI5_GPIOMUX);
    GPIO_setPadConfig(DSP_DI5_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI5_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI5_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI5_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI6_GPIOMUX);
    GPIO_setPadConfig(DSP_DI6_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI6_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI6_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI6_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI7_GPIOMUX);
    GPIO_setPadConfig(DSP_DI7_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI7_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI7_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI7_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI8_GPIOMUX);
    GPIO_setPadConfig(DSP_DI8_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI8_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI8_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI8_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI9_GPIOMUX);
    GPIO_setPadConfig(DSP_DI9_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI9_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI9_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI9_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI10_GPIOMUX);
    GPIO_setPadConfig(DSP_DI10_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI10_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI10_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI10_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI11_GPIOMUX);
    GPIO_setPadConfig(DSP_DI11_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI11_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI11_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI11_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI12_GPIOMUX);
    GPIO_setPadConfig(DSP_DI12_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI12_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI12_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI12_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI13_GPIOMUX);
    GPIO_setPadConfig(DSP_DI13_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI13_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI13_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI13_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI14_GPIOMUX);
    GPIO_setPadConfig(DSP_DI14_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI14_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI14_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI14_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DI15_GPIOMUX);
    GPIO_setPadConfig(DSP_DI15_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DI15_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DI15_GPIO, GPIO_DIR_MODE_IN);
    GPIO_setControllerCore(DSP_DI15_GPIO, GPIO_CORE_CPU1);

    // DO 引脚初始化
    GPIO_setPinConfig(DSP_DO0_GPIOMUX);
    GPIO_setPadConfig(DSP_DO0_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO0_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO0_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO0_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO1_GPIOMUX);
    GPIO_setPadConfig(DSP_DO1_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO1_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO1_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO1_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO2_GPIOMUX);
    GPIO_setPadConfig(DSP_DO2_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO2_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO2_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO2_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO3_GPIOMUX);
    GPIO_setPadConfig(DSP_DO3_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO3_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO3_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO3_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO4_GPIOMUX);
    GPIO_setPadConfig(DSP_DO4_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO4_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO4_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO4_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO5_GPIOMUX);
    GPIO_setPadConfig(DSP_DO5_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO5_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO5_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO5_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO6_GPIOMUX);
    GPIO_setPadConfig(DSP_DO6_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO6_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO6_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO6_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO7_GPIOMUX);
    GPIO_setPadConfig(DSP_DO7_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO7_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO7_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO7_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO8_GPIOMUX);
    GPIO_setPadConfig(DSP_DO8_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO8_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO8_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO8_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO9_GPIOMUX);
    GPIO_setPadConfig(DSP_DO9_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO9_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO9_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO9_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO10_GPIOMUX);
    GPIO_setPadConfig(DSP_DO10_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO10_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO10_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO10_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO11_GPIOMUX);
    GPIO_setPadConfig(DSP_DO11_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO11_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO11_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO11_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO12_GPIOMUX);
    GPIO_setPadConfig(DSP_DO12_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO12_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO12_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO12_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO13_GPIOMUX);
    GPIO_setPadConfig(DSP_DO13_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO13_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO13_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO13_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO14_GPIOMUX);
    GPIO_setPadConfig(DSP_DO14_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO14_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO14_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO14_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_DO15_GPIOMUX);
    GPIO_setPadConfig(DSP_DO15_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_DO15_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_DO15_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_DO15_GPIO, GPIO_CORE_CPU1);

    // 模拟开关切换IO
    GPIO_setPinConfig(DSP_ADCAnalog_A0_GPIOMUX);
    GPIO_setPadConfig(DSP_ADCAnalog_A0_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_ADCAnalog_A0_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_ADCAnalog_A0_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_ADCAnalog_A0_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_ADCAnalog_A1_GPIOMUX);
    GPIO_setPadConfig(DSP_ADCAnalog_A1_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_ADCAnalog_A1_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_ADCAnalog_A1_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_ADCAnalog_A1_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(DSP_ADCAnalog_A2_GPIOMUX);
    GPIO_setPadConfig(DSP_ADCAnalog_A2_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DSP_ADCAnalog_A2_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(DSP_ADCAnalog_A2_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DSP_ADCAnalog_A2_GPIO, GPIO_CORE_CPU1);

    /*  FPGA    */
    GPIO_setPinConfig(FPGA_CSn_GPIOMUX);
    GPIO_setPadConfig(FPGA_CSn_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(FPGA_CSn_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(FPGA_RSTn_GPIOMUX);
    GPIO_setPadConfig(FPGA_RSTn_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(FPGA_RSTn_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(FPGA_RSTn_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(FPGA_RSTn_GPIO, GPIO_CORE_CPU1);

    FPGA_RSTn(0);           // FPGA复位
    DELAY_US(1);            // 延迟时间
    FLASH_CS(1);
    FRAM_CS(1);

    FPGA_RSTn(1);           // FPGA 正常工作
}


/*  外设初始化   */
void Peripheral_Init(void)
{
    /*  GPIO初始化  */
    Peripheral_GPIO_Init();
    /* 外设初始化 */
    MyDMA_Init();
    /* SPI 初始化 */
    SPI_Init();
    /* SCI 初始化 */
    SCI_A_Init();
    SCI_B_Init();
    SCI_C_Init();
    SCI_D_Init();
    /* CAN 初始化 */
    CAN_Init();
    /*  PWM初始化  */
    myEPWM_Init();
    /* 外部存储器初始化 */
    my_xintfInit();

    EALLOW;
    CpuSysRegs.SECMSEL.bit.PF2SEL = 1;
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);   // 开启PWM
    EDIS;
}



