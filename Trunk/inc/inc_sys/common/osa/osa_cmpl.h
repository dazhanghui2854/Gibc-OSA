/*******************************************************************************
* osa_cmpl.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2011-06-08Create
*
* Description: 编译器相关的封装。
*
*       1. 硬件说明。
*          无。
*  
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
* 1. Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_CMPL_H_
#define _OSA_CMPL_H_


#ifdef __GNUC__  /* GCC编译器 */
    /* 以下两个函数用于优化代码执行顺序，可提升处理器的代码执行流水线流畅度和cache命中率。*/
    #define OSA_likely(x)      __builtin_expect(!!(x), 1)  /* 可能发生的概率低 */
    #define OSA_unlikely(x)    __builtin_expect(!!(x), 0)  /* 可能发生的概率高 */
    /* 此函数用于防止编译器优化代码执行顺序。*/
    #define OSA_barrier()      __asm__ __volatile__("": : :"memory")
#else
    #define OSA_likely(x)     (x) 
    #define OSA_unlikely(x)   (x)   
    #define OSA_barrier()
#endif


#endif


