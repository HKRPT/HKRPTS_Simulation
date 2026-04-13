/*
 * flash.h
 *
 *  Created on: 2025年2月5日
 *      Author: BYRD-003
 */

#ifndef SOURCE_CODE_INCLUDE_FLASH_H_
#define SOURCE_CODE_INCLUDE_FLASH_H_


#define FLASH_TIMEOUT_LIMIT      1000
#define FLASH_TXBUF_NUM          0
#define FLASH_RXBUF_NUM          1


/*  固件更新 帧程序格式  */
/*
 * 帧ID (兼容CAN)           若为0x0216则为更新固件指令
 * data (8位数据格式)        限定一帧传输8位数据(兼容CAN)
 * crc  (modbus 16位校验)   CAN没有，串口、网口需带有
 * */
/*
 * FLASH
 * 限定结构体的data区只放256个数据
 * 到达256数据就通过spi发送到flash存储，或未达256并超过一定时间则存入flash，并长度清零
 * 单数sector存数据，双数sector存crc
 * */
typedef enum
{
    FLASH_Normal     = 0,
    FLASH_ERROR         ,
    FLASH_TimeOut       ,
    FLASH_CRCTxError    ,
    FLASH_CRCRxError    ,
}FLASH_Status;

typedef struct
{
    uint16_t    head[2];            // 包头
    uint16_t    data[36];          // 包数据 128字 = 256字节
}FLASH_t;

typedef struct
{
    uint16_t    Instruction;        // 指令
    uint16_t    address1;           // 地址
    uint16_t    address2;           // 地址
    uint16_t    address3;           // 地址

    uint16_t    data[256];          // 包数据 128字 = 256字节
}FLASHU8_t;

typedef struct
{
    uint16_t    head[2];            // 包头

    uint32_t    dataIsOver;         // 数据是否结束 1为结束
    uint32_t    len;                // 数据长度
    uint16_t    dataCrc[2];         // data校验 累计，最终为整包校验结果
    uint16_t    crc[2];             // 整包CRC校验位 除去结构体的包头
}FLASH_MSG_t;


/************************FLASH Instruction***************************/
#define W25X_WriteEnable              0x06
#define W25X_WriteDisable             0x04
#define W25X_ReadStatusReg            0x05
#define W25X_WriteStatusReg           0x01
#define W25X_ReadData                 0x03
#define W25X_FastReadData             0x0B
#define W25X_FastReadDual             0x3B
#define W25X_PageProgram              0x02
#define W25X_BlockErase               0xD8
#define W25X_SectorErase              0x20
#define W25X_ChipErase                0xC7
#define W25X_PowerDown                0xB9
#define W25X_ReleasePowerDown         0xAB
#define W25X_DeviceID                 0xAB
#define W25X_ManufactDeviceID         0x90
#define W25X_JedecDeviceID            0x9F

#define WIP_Flag                      0x01
#define Dummy_Byte                    0xFF

// Sector | Block | 0x001000     -> BLOCK1
//                  ....
// Sector | Block | 0x00F000     -> BLOCK15
#define FLASH_Addr(Block,Sector)          0x000000 | (Block << 16) | (Sector << 12)

// 页编程  最多256个字节
extern void FLASH_Send_DataU8(uint32_t addr, uint16_t *buff1,uint16_t len);
// 擦除扇区
extern void FLASH_BlockErase(uint32_t addr);
// 读取数据
extern void FLASH_Read_DataU8(uint32_t addr, FLASHU8_t *buff,uint16_t len);
// 页编程  最多256个字节
extern void FLASH_Page_Program(uint32_t addr, FLASH_t *buff,uint16_t len);
// 擦除扇区
extern void FLASH_SectorErase(uint32_t addr);
// 读取数据
extern void FLASH_Read_Data(uint32_t addr, FLASH_t *buff,uint16_t len);
void FLASH_Page_ProgramU8(uint32_t addr, uint16_t *buff,uint16_t len);
extern FLASH_MSG_t flash_msg[2];                       // 0为发送数组,1为接收数组
extern FLASH_t     flash_data[2];                      // 0为发送数组,1为接收数组
extern FLASHU8_t   flash_dataU8[2];                    // 0为发送数组,1为接收数组
#endif /* SOURCE_CODE_INCLUDE_FLASH_H_ */
