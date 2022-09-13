/*******************************************************************************
* osa_macro.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2012-5-31 Create
*
* Description: 统一的通用宏。
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


#ifndef _OSA_MACRO_H_
#define _OSA_MACRO_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* 函数返回值 */
#define OSA_SOK        0        /* 成功 */
#define OSA_EFAIL     -1        /* 通用错误 */
#define OSA_ETIMEOUT  -2        /* 等待超时 */
#define OSA_EINTR     -3        /* 被信号中断 */
#define OSA_EBUSY     -4        /* 对象忙 */


/* Bool类型真假 */
#define OSA_TRUE	  (1)       /* 真 */
#define OSA_FALSE	  (0)       /* 假 */

/* NULL值定义 */
#define OSA_NULL      (NULL)

/* 变量属性register关键词 */
#define OSA_register  register

/* 超时参数 */
#define OSA_TIMEOUT_NONE         (0)    /* 不等待，立即返回。*/ 
#define OSA_TIMEOUT_FOREVER      (~0U)  /* 一直等待直到返回 */


/* 数据存储度量单位 */
#define OSA_KB (1024)
#define OSA_MB (OSA_KB * OSA_KB)
#define OSA_GB (OSA_KB * OSA_MB)
/* 
 * 设置版本号用的宏。高8位代表Major Version，中8位代表Minor Version，
 * 低16代表Revision。
 */
#define OSA_versionSet(major, minor, revision)  \
        ((((major) & 0x0ff) << 24U) | (((minor) & 0x0ff) << 16U) \
           | ((revision) & 0x0ffff))


/* 数值比较*/
#define OSA_max(a, b)    ( (a) > (b) ? (a) : (b) )
#define OSA_min(a, b)    ( (a) < (b) ? (a) : (b) )

/* 范围约束, 不作为右值 */
#define OSA_upperLimit(x, limit)  if ((x) > (limit)) (x) = (limit)
#define OSA_lowerLimit(x, limit)  if ((x) < (limit)) (x) = (limit)
#define OSA_rangeLimit(x, lowerLimit, upperLimit) \
		do \
		{ \
		    OSA_lowerLimit((x), (lowerLimit)); \
		    OSA_upperLimit((x), (upperLimit)); \
		} while (0)

/* 内存操作*/
#define OSA_clear(ptr)		        memset((ptr), 0, sizeof(*(ptr)))	
#define OSA_clearSize(ptr, size)        memset((ptr), 0, (size))
#define OSA_memCmp(dst, src)            memcmp((dst), (src), sizeof(*(src)))
#define OSA_memCmpSize(dst, src, size)  memcmp((dst), (src), (size))


/* 对齐操作*/
#define OSA_align(value, align)   ((( (value) + ( (align) - 1 ) ) \
                                     / (align) ) * (align) )
#define OSA_ceil(value, align)    OSA_align(value, align) 
#define OSA_floor(value, align)   (( (value) / (align) ) * (align) )


/* 设置真假 */
#define OSA_setTrue(val)        ((val) = OSA_TRUE)
#define OSA_setFalse(val)       ((val) = OSA_FALSE)      


/* 获取数组成员数量 */
#define OSA_arraySize(array)    (sizeof(array)/sizeof((array)[0]))


/* 获取结构体成员的地址偏移量 */
#ifdef __compiler_offsetof
#define OSA_offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define OSA_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif


/* 通过结构体成员获取结构体首地址 */
#define OSA_containerOf(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - OSA_offsetof(type,member) );})


/* 检测value值是否是2的N次方 */
#define OSA_checkIs2n(value)  ( ((value) == 0) ? OSA_FALSE  \
                               : ((value) & ((value) - 1))  \
                                  ? OSA_FALSE : OSA_TRUE )

#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_MACRO_H_  */


