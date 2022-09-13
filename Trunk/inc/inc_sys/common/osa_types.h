/*******************************************************************************
* osa_types.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2012-5-31 Create
*
* Description: 统一的数据类型。
*
*       1. 硬件说明。
*          无。

*       2. 程序结构说明。
*          无
*
*       3. 使用说明。
*          无。
*          
*       4. 局限性说明。
*          无。
*
*       5. 其他说明。
*          无。
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_TYPES_H_
#define _OSA_TYPES_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include "osa_sys.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

#if !defined(STD_H) && !defined(_TI_STD_TYPES)

/* 布尔量定义 */
#ifndef DEFINED_Bool
#define DEFINED_Bool
typedef unsigned char      Bool;        /* 通用布尔类型 */
#endif

/* 有符号类型定义 */
#ifndef DEFINED_Int32
#define DEFINED_Int32
typedef int                Int32;       /* 有符号32位整形数类型 */   
#endif

#ifndef DEFINED_Int16
#define DEFINED_Int16
typedef short              Int16;       /* 有符号16位整形数类型 */   
#endif

#ifndef DEFINED_Int8
#define DEFINED_Int8
typedef char               Int8;        /* 有符号8位整形数类型 */    
#endif

/* 指针类型定义 */
#ifndef DEFINED_Ptr
#define DEFINED_Ptr
typedef void *             Ptr;         /* 指针类型 */
#endif

/* 字符类型定义 */
#ifndef DEFINED_String
#define DEFINED_String
typedef char *             String;      /* 字符串类型，以NUL结尾。*/
#endif

#ifndef DEFINED_Char
#define DEFINED_Char
typedef char               Char;        /* 字符类型 */
#endif

#endif


#if !defined(_TI_STD_TYPES)

/* 无符号类型定义 */ 
#ifndef DEFINED_Uint32
#define DEFINED_Uint32
typedef unsigned int       Uint32;      /* 无符号32位整形数类型 */ 
#endif

#ifndef DEFINED_Uint16
#define DEFINED_Uint16
typedef unsigned short     Uint16;      /* 无符号16位整形数类型 */ 
#endif

#ifndef DEFINED_Uint8
#define DEFINED_Uint8
typedef unsigned char      Uint8;       /* 无符号8位整形数类型 */ 
#endif

#ifndef DEFINED_Int64
#define DEFINED_Int64
typedef long long          Int64;       /* 有符号64位整形数类型 */
#endif

#endif

#ifndef DEFINED_Bool16
#define DEFINED_Bool16
typedef unsigned short     Bool16;      /* 16位布尔类型 */
#endif

#ifndef DEFINED_Bool32
#define DEFINED_Bool32
typedef unsigned int       Bool32;      /* 32位布尔类型 */
#endif

#ifndef DEFINED_Int32L
#define DEFINED_Int32L
typedef long               Int32L;      /* 有符号32位长整形数类型 */ 
#endif

#ifndef DEFINED_Uint32L
#define DEFINED_Uint32L
typedef unsigned long      Uint32L;     /* 无符号32位长整形数类型 */   
#endif

#ifndef DEFINED_Uint64
#define DEFINED_Uint64
typedef unsigned long long Uint64;      /* 无符号64位整形数类型 */
#endif


/* 浮点类型定义 */
#ifndef DEFINED_Float32
#define DEFINED_Float32
typedef float              Float32;		/* 32位浮点数类型 */
#endif

#ifndef DEFINED_Float64
#define DEFINED_Float64
typedef double             Float64;		/* 64位浮点数类型 */
#endif

/* 句柄类型 */
#ifndef DEFINED_Handle
#define DEFINED_Handle
typedef void *             Handle;      /* 统用句柄类型 */
#endif

/* 空类型定义 */
#ifndef DEFINED_Empty
#define DEFINED_Empty
typedef void               Empty;        /* 空类型 */
#endif

/* size_t类型定义 */
#ifndef DEFINED_Sizet
#define DEFINED_Sizet
typedef size_t             Sizet;       /* size_t类型 */
#endif

/* 32&64位兼容类型定义 */
#define Uint8_Align64 Uint8 __attribute__ ((aligned(8)))

#define _SIZEOF_NEED_CHECKED(dataType) \
	(sizeof(dataType) - OSA_offsetof(dataType, u8_align64))
	
#define _SIZEOF_NEED_CHECKED_EQUAL(dataType, nBytes) \
	((_SIZEOF_NEED_CHECKED(dataType) == (nBytes))?1:-1)
	
#define _SIZEOF_CHECK_EQ(dataType, nBytes) \
	typedef char SC_EQ_##dataType[_SIZEOF_NEED_CHECKED_EQUAL(dataType, nBytes)];

#define _SIZEOF_CHECK_EQ_NEW(dataType, nBytes) \
	typedef char SC_EQ_##dataType[((sizeof(dataType) == (nBytes))?1:-1)];

#if (__SIZEOF_POINTER__ == 4)
#define Ptr_t(type, field) type* field;void * res_##field
#define Fxn_t(type, field, args ...) type (*field)(args);void * res_##field
#elif (__SIZEOF_POINTER__ == 8)
#define Ptr_t(type, field) type* field
#define Fxn_t(type, field, args ...) type (*field)(args)
#else
#error "__SIZEOF_POINTER__ = ?"
#endif

/// 另一种写法
/// 保证32位和64位平台的指针变量长度一致，防止32位平台头文件增加指针导致64位下结构体大小改变
/// 用法：在指针变量后，使用此宏，N填任意值，同一个结构体内不重名即可。
/// char* pointer; 
///OSA_UNUSED_FORCE_POINTER_ALIGN8(pointer)
#ifndef OSA_UNUSED_FORCE_POINTER_ALIGN8
#if defined(_WIN64) || (defined(__WORDSIZE) && __WORDSIZE==64)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N)
#elif defined(_WIN32) || (defined(__WORDSIZE) && __WORDSIZE==32)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N) Uint32 unused##N;
#elif defined(__GNUC__)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N) Uint8 unused##N[8-sizeof(void*)];
#elif defined(CCS)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N) Uint32 unused##N;
#else
#error "Can't find macro `__WORDSIZE' definition, please specify this macro 32 or 64 base on your platform!"
#endif
#endif //UNUSED_FORCE_POINTER_ALIGN8


#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_TYPES_H_  */

