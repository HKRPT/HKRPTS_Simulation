/*
* Copyright (c) 2012
* All rights reserved.
*
* 文件名称：types.h
* 文件标识：
* 摘    要：针对每个种编译器对定点型数据和浮点型数据处理不一样而定义的头文件，
* 提高程序的移植性。
*
* 当前版本：1.0
* 作    者：shawn.xu
* 完成日期：2012年8月2日
*
* 取代版本：无
* 原作者  ：无
* 完成日期：无
*/
#ifndef __TYPES_H__  // 防止被重定义
#define __TYPES_H__

#ifndef STATIC
#define STATIC static
#endif
// 根据目前使用的是CCSv5 C2000 编译器定义以下数据类型
// 备注：C2000是16位DSC, 定义16位数据也是按照16位进行存储
// Table: Integer Data Types
// Type                   Bits      Min      Max
// char, signed char      8        -128      127
// unsigned char          8         0        255
// short, signed short    16       -32768    32767
// unsigned short         16        0        65535
// int, signed int        32       -2^31     2^31 - 1
// unsigned int           32        0        2^32 - 1
// long, signed long      32       -2^31     2^31 - 1
// unsigned long          32        0        2^32 - 1
// long long**,
// signed long long**     64       -2^63     2^63 - 1
// unsigned long long**   64        0        2^64 - 1


// Table: Floating Point Data Types
// Type          Bits  E_Min  E_Max  N_Min    N_Max
// float         32    -126   127    2^-126   2^128
// double*       32    -126   127    2^126    2^128
// long double   64    -1022  1023   2^-1022  2^1024
// E = Exponent
// N = Normalized (approximate)
// * double is equivalent to long double if -fno-short-double is used.

typedef char                 BOOL;

// data types:
typedef signed char          CHAR;
typedef unsigned char        UCHAR;

typedef signed int     SHORT;     // signed   16 bit quantity

// data types:
typedef unsigned char        UBYTE;     // unsigned  8 bit quantity
typedef signed char          BYTE;      // signed    8 bit quantity

typedef unsigned short int   USHORT;    // unsigned 16 bit quantity
typedef unsigned short int   UWORD;     // unsigned 16 bit quantity

typedef signed short int     WORD;      // signed   16 bit quantity
typedef unsigned long        ULONG;     // unsigned 32 bit quantity
typedef signed long          LONG;      // signed   32 bit quantity

// data types
typedef unsigned char        INT8U;     // unsigned  8 bit quantity
typedef signed char          INT8S;     // signed    8 bit quantity
typedef unsigned int   INT16U;    // unsigned 16 bit quantity
typedef signed int     INT16S;    // signed   16 bit quantity
typedef unsigned long int    INT32U;    // Unsigned 32 bit quantity
typedef signed long int      INT32S;    // Signed   32 bit quantity

typedef float          FP32;
typedef long double    FP64;


typedef unsigned char  Uint8;



typedef unsigned char        uint8_T;
typedef signed char          int8_T;
typedef unsigned int   uint16_T;
typedef unsigned long        uint32_T;

typedef signed int q15_t;
typedef signed long q31_t;

#ifndef ERROR
#define ERROR  1u
#endif

#ifndef NO_ERROR
#define NO_ERROR 0u
#endif

#endif
