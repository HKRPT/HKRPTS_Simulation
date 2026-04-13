/*
 * bootloader.h
 *
 *  Created on: 2026年2月6日
 *      Author: YangHaoYan
 */

#ifndef SOURCE_CODE_BOOTLOADER_BOOTLOADER_H_
#define SOURCE_CODE_BOOTLOADER_BOOTLOADER_H_

#include "main.h"

typedef enum
{
    BOOTSuceessWriteROM = 0,
    BOOTModeDontWrite   = 4,
    BOOTErrorWriteROM   = 5,
    BOOTExcessLenROM    ,
    BOOTErrorCRCData    ,
    BOOTErrorCRCROM     ,
}bootStatus_t;

typedef struct
{
    uint16_t    ComplateCRC;            // 完整包CRC
    uint32_t    ComplateLen;            // 完整包长度
    uint32_t    ComplateTopAddr;        // 完整包首地址
    uint16_t    ComplateTime;           // 编译时间
    uint16_t    ComplateVersion;        // 编译版本
    uint16_t    ComplateUpdateFlag;     // 编译更新标志
    uint16_t    CRC;                    // 结构体CRC
}bootloaderMsg_t;

#define BootLoaderFlashBootArea1        0x7F0000
#define BootLoaderFlashBootArea2        0x800000
#define BootLoaderFlashBootArea3        0x810000
#define BootLoaderFlashMsgArea3         0x820000

#define BootLoaderFlashBootBackupArea   0x050000

// 首地址
#define bootLoaderAddr              BootLoaderFlashBootArea1
// 长度
#define bootLoaderLen               0x030000
// 最大地址
#define bootLoaderTopAddr           bootLoaderAddr + bootLoaderLen


// bootLoader在线解析函数
void BootLoaderFrameFunc(modbusMasterData_t* data);
void BootLoaderReadFlashToComplateCRC(void);
uint16_t BootLoaderReadFlashToComplateCRCBoot(uint32_t boot_addr);
uint16_t BootLoaderReadFlashToComplateMsg(uint32_t boot_addr, uint32_t msg_addr);
void bootloaderupdateApp(void);
void bootloaderTest(void);
uint16_t APP_BackupToFLASH(void);

#endif /* SOURCE_CODE_BOOTLOADER_BOOTLOADER_H_ */
