/*
 * my_fram.h
 *
 *  Created on: 2026年2月9日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_INCLUDE_MY_FRAM_H_
#define SOURCE_CODE_INCLUDE_MY_FRAM_H_

#include "main.h"

typedef struct
{
    uint16_t    head[2];        // 包头
    uint16_t    data[36];       // 包数据
    uint16_t    crc[2];         // CRC校验位
}Fram_t;

typedef enum
{
    Fram_Normal      = 0,
    Fram_ERROR       ,
    Fram_TimeOut     ,
    Fram_CRCTxError    ,
    Fram_CRCRxError    ,
}FRAM_Status;

extern volatile struct SPI_REGS     *SPI_REG[];
extern volatile struct CH_REGS      *DMAChannel[];


#define FRAM_TXBUF_NUM          0
#define FRAM_RXBUF_NUM          1
#define FRAM_TIMEOUT_LIMIT      1000


#endif /* SOURCE_CODE_INCLUDE_MY_FRAM_H_ */
