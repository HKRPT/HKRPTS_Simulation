/*
 * my_xmif.h
 *
 *  Created on: 2025年9月23日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_INCLUDE_MY_XMIF_H_
#define SOURCE_CODE_INCLUDE_MY_XMIF_H_

#include "main.h"



#define DSP_XMIF_A0_GPIOMUX         GPIO_38_EM1A0
#define DSP_XMIF_A0_GPIO            38

#define DSP_XMIF_A1_GPIOMUX         GPIO_39_EM1A1
#define DSP_XMIF_A1_GPIO            39

#define DSP_XMIF_A2_GPIOMUX         GPIO_40_EM1A2
#define DSP_XMIF_A2_GPIO            40

#define DSP_XMIF_A3_GPIOMUX         GPIO_41_EM1A3
#define DSP_XMIF_A3_GPIO            41

#define DSP_XMIF_A4_GPIOMUX         GPIO_44_EM1A4
#define DSP_XMIF_A4_GPIO            44

#define DSP_XMIF_A5_GPIOMUX         GPIO_45_EM1A5
#define DSP_XMIF_A5_GPIO            45

#define DSP_XMIF_A6_GPIOMUX         GPIO_46_EM1A6
#define DSP_XMIF_A6_GPIO            46

#define DSP_XMIF_A7_GPIOMUX         GPIO_47_EM1A7
#define DSP_XMIF_A7_GPIO            47

#define DSP_XMIF_A8_GPIOMUX         GPIO_48_EM1A8
#define DSP_XMIF_A8_GPIO            48

#define DSP_XMIF_A9_GPIOMUX         GPIO_49_EM1A9
#define DSP_XMIF_A9_GPIO            49

#define DSP_XMIF_A10_GPIOMUX        GPIO_50_EM1A10
#define DSP_XMIF_A10_GPIO           50

#define DSP_XMIF_A11_GPIOMUX        GPIO_51_EM1A11
#define DSP_XMIF_A11_GPIO           51

#define DSP_XMIF_A12_GPIOMUX        GPIO_52_EM1A12
#define DSP_XMIF_A12_GPIO           52

#define DSP_XMIF_A13_GPIOMUX        GPIO_86_EM1A13
#define DSP_XMIF_A13_GPIO           86

#define DSP_XMIF_A14_GPIOMUX        GPIO_87_EM1A14
#define DSP_XMIF_A14_GPIO           87

#define DSP_XMIF_A15_GPIOMUX        GPIO_88_EM1A15
#define DSP_XMIF_A15_GPIO           88

#define DSP_XMIF_A16_GPIOMUX        GPIO_89_EM1A16
#define DSP_XMIF_A16_GPIO           89

#define DSP_XMIF_A17_GPIOMUX        GPIO_90_EM1A17
#define DSP_XMIF_A17_GPIO           90

#define DSP_XMIF_A18_GPIOMUX        GPIO_91_EM1A18
#define DSP_XMIF_A18_GPIO           91

#define DSP_XMIF_EM1BA1_GPIOMUX     GPIO_92_EM1BA1
#define DSP_XMIF_EM1BA1_GPIO        92

#define DSP_XMIF_D0_GPIOMUX         GPIO_85_EM1D0
#define DSP_XMIF_D0_GPIO            85

#define DSP_XMIF_D1_GPIOMUX         GPIO_83_EM1D1
#define DSP_XMIF_D1_GPIO            83

#define DSP_XMIF_D2_GPIOMUX         GPIO_82_EM1D2
#define DSP_XMIF_D2_GPIO            82

#define DSP_XMIF_D3_GPIOMUX         GPIO_81_EM1D3
#define DSP_XMIF_D3_GPIO            81

#define DSP_XMIF_D4_GPIOMUX         GPIO_80_EM1D4
#define DSP_XMIF_D4_GPIO            80

#define DSP_XMIF_D5_GPIOMUX         GPIO_79_EM1D5
#define DSP_XMIF_D5_GPIO            79

#define DSP_XMIF_D6_GPIOMUX         GPIO_78_EM1D6
#define DSP_XMIF_D6_GPIO            78

#define DSP_XMIF_D7_GPIOMUX         GPIO_77_EM1D7
#define DSP_XMIF_D7_GPIO            77

#define DSP_XMIF_D8_GPIOMUX         GPIO_76_EM1D8
#define DSP_XMIF_D8_GPIO            76

#define DSP_XMIF_D9_GPIOMUX         GPIO_75_EM1D9
#define DSP_XMIF_D9_GPIO            75

#define DSP_XMIF_D10_GPIOMUX        GPIO_74_EM1D10
#define DSP_XMIF_D10_GPIO           74

#define DSP_XMIF_D11_GPIOMUX        GPIO_73_EM1D11
#define DSP_XMIF_D11_GPIO           73

#define DSP_XMIF_D12_GPIOMUX        GPIO_72_EM1D12
#define DSP_XMIF_D12_GPIO           72

#define DSP_XMIF_D13_GPIOMUX        GPIO_71_EM1D13
#define DSP_XMIF_D13_GPIO           71

#define DSP_XMIF_D14_GPIOMUX        GPIO_70_EM1D14
#define DSP_XMIF_D14_GPIO           70

#define DSP_XMIF_D15_GPIOMUX        GPIO_69_EM1D15
#define DSP_XMIF_D15_GPIO           69

#define DSP_XMIF_CSn_GPIOMUX        GPIO_34_EM1CS2N
#define DSP_XMIF_CSn_GPIO           34

#define DSP_XMIF_OEn_GPIOMUX        GPIO_37_EM1OEN
#define DSP_XMIF_OEn_GPIO           37

#define DSP_XMIF_WEn_GPIOMUX        GPIO_31_EM1WEN
#define DSP_XMIF_WEn_GPIO           31

// 外部存储器初始化
void my_xintfInit(void);

#endif /* SOURCE_CODE_INCLUDE_MY_XMIF_H_ */
