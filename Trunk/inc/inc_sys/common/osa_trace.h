/*******************************************************************************
* osa_trace.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2011-07-25 Create
*          V1.0.1  2011-11-14 1. 增加对TI SYSBIOS系统的支持。                            
*                             2. 将DEBUG宏修改为_OSA_TRACE_。
*                             3. 文件名由osa_debug.h修改为osa_trace.h。
*          V1.0.2  2011-11-30 1. 增加程序编译时用断言OSA_COMPILETIME_ASSERT。
*
* Description: 统一的调试打印接口，兼容Linux应用程序和驱动程序，SYSBIOS系统程序。
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


#ifndef _OSA_TRACE_H_
#define _OSA_TRACE_H_


/* 打印字体显示的颜色 */
#define NONE          "\033[m"   
#define RED           "\033[0;32;31m"   
#define GREEN         "\033[0;32;32m"   
#define BLUE          "\033[0;32;34m"   
#define DARY_GRAY     "\033[1;30m"   
#define CYAN          "\033[0;36m"   
#define PURPLE        "\033[0;35m"     
#define BROWN         "\033[0;33m"   
#define YELLOW        "\033[1;33m"   
#define WHITE         "\033[1;37m"   


/* 
 模块的名字必须在Makefile或其他头文件中定义，名字不需要加上[]。
 在Makefile中添加的方法是：
 OSA_MODULE_NAME = XXX
 CFLAGS       += -DOSA_MODULE_NAME='"$(OSA_MODULE_NAME)"'
 EXTRA_CFLAGS += -D'OSA_MODULE_NAME=\"$(OSA_MODULE_NAME)\"'
 应用程序中使用CFLAGS，驱动程序中使用EXTRA_CFLAGS。
*/
#ifndef OSA_MODULE_NAME
#error Not define OSA_MODULE_NAME !!!
#endif


#ifdef __KERNEL__    /* Linux kernel */
    #include <linux/kernel.h>

    #define OSA_print  printk
#elif ___DSPBIOS___  /* TI SYSBIOS */
	  extern int Vps_printf(char *format, ...);
    #define OSA_print  Vps_printf /* print info to share memory */
#else                /* Generic Application */
    #include <stdio.h>
    
    #define OSA_print  printf
#endif


/* _OSA_TRACE_宏是调试开关，可通过Makeife传入，传入方法是-D_OSA_TRACE_。*/
/* 调试版本用的断言 */
#ifdef _OSA_TRACE_
    /* 断言，输出字体为红色，打印出文件名、函数名和行号。程序进入无限循环。*/ 
	#define OSA_assert(x)  \
	    if((x) == 0) 	   \
	    {                  \
	    	OSA_print(RED "[" OSA_MODULE_NAME "] " "ASSERT (%s|%d)\r\n", \
	    	                __func__, __LINE__); \
	    	while(1);      \
	    } 
		
	#define OSA_assertSuccess(ret)	\
	    OSA_assert(OSA_SOK == (ret))
    #define OSA_assertFail(ret)  \
	    OSA_assert(OSA_SOK != (ret))

    #define OSA_assertNotNull(ptr)  \
	    OSA_assert(NULL != (ptr))
    #define OSA_assertNull(ptr)  \
        OSA_assert(NULL == (ptr)) 

    /* 调试打印，输出字体不带颜色。*/
	#define OSA_DEBUG(fmt, args ...) \
		OSA_print("[" OSA_MODULE_NAME "] " fmt, ## args)

#else
	#define OSA_assert(x)
	#define OSA_assertSuccess(ret)
	#define OSA_assertFail(ret)
	#define OSA_assertNotNull(ptr)
	#define OSA_assertNull(ptr)
	#define OSA_DEBUG(fmt, args ... ) 
#endif


/* 判断返回值 */
#define OSA_isSuccess(status)  (OSA_SOK == (status))
#define OSA_isFail(status)     (OSA_SOK != (status))
    
#define OSA_isTrue(status)     ((status))
#define OSA_isFalse(status)    (!(status))   
    
#define OSA_isNull(ptr)        (NULL == (ptr))
#define OSA_isNotNull(ptr)     (NULL != (ptr))


/* 程序Release版本用的断言 */
/* 断言，输出字体为红色，打印出文件名、函数名和行号。程序进入无限循环。*/ 
#define OSA_assertRl(x)  \
    if((x) == 0) { \
        OSA_print(RED "[" OSA_MODULE_NAME "] " "ASSERT (%s|%d)\r\n", \
                        __func__, __LINE__); \
        while(1);   \
    } 
#define OSA_assertSuccessRl(status)  \
    OSA_assertRl(OSA_SOK == (status))
#define OSA_assertFailRl(status)  \
    OSA_assertRl(OSA_SOK != (status))

#define OSA_assertNotNullRl(ptr)  \
    OSA_assertRl(NULL != (ptr))
#define OSA_assertNullRl(ptr)  \
    OSA_assertRl(NULL == (ptr)) 


/* 程序编译时用的断言，该断言发生在程序编译时刻。*/
#define OSA_assertCompTime(condition)  \
    do {                                   \
       typedef char ErrorCheck[((condition) == OSA_TRUE) ? 1 : -1]; \
    } while(0)


/* 以下打印不管是否开启调试模式，都打印输出。*/

/* 通用打印，输出字体不带颜色。*/
#define OSA_printf(fmt, args ...) \
	do \
	{ \
		OSA_print("[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)

/* 出错打印，输出字体为红色，打印出文件名、函数名和行号,表明程序不能继续运行。*/
#define OSA_ERROR(fmt, args ...) \
	do \
	{ \
		OSA_print(RED "[" OSA_MODULE_NAME "] " "ERROR  (%s|%d): " fmt NONE, \
		                    __func__, __LINE__, ## args); \
	} \
	while(0)
 
/* 出错打印，输出字体为红色。*/
#define OSA_ERRORR(fmt, args ...) \
	do \
	{ \
		OSA_print(RED "[" OSA_MODULE_NAME "] " "ERROR  : " fmt NONE, ## args);\
	} \
	while(0)	

/* 
* 警告打印，输出字体为黄色，打印出文件名、函数名和行号。表明程序仍可继续运行，
* 但须警示。
*/
#define OSA_WARN(fmt, args ...) \
	do \
	{ \
		OSA_print(YELLOW "[" OSA_MODULE_NAME "] " "WARN  (%s|%d): "        \
		                    fmt NONE, __func__, __LINE__, ## args); \
	} \
	while(0)

/* 警告打印，输出字体为黄色。表明程序仍可运行下去，但须警示。*/
#define OSA_WARNR(fmt, args ...) \
	do \
	{ \
		OSA_print(YELLOW "[" OSA_MODULE_NAME "] " "WARN  : "    \
		                    fmt NONE, ## args);                 \
	} \
	while(0)

/* 信息通告打印，输出字体为绿色。*/
#define OSA_INFO(fmt, args ...) \
	do \
	{ \
		OSA_print(GREEN "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
	} \
	while(0)


/*  
* 以上打印为各模块强制使用，以下打印给模块可选择使用，也可自行添加打印方式，
* 但打印必须打印出模块名字，除特殊应用情况。后缀加上字母R的表示不打印模块名。
*/
#define OSA_INFOR(fmt, args ...) \
	do \
	{ \
		OSA_print(GREEN fmt NONE, ## args);\
	} \
	while(0)


#define OSA_INFOPUP(fmt, args ...) \
	do \
	{ \
		OSA_print(PURPLE "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
	} \
	while(0)

#define OSA_INFOPUPR(fmt, args ...) \
    do \
    { \
        OSA_print(PURPLE fmt NONE, ## args);\
    } \
    while(0)


#define OSA_INFORED(fmt, args ...) \
	do \
	{ \
		OSA_print(RED "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
	} \
	while(0)
	
#define OSA_INFOREDR(fmt, args ...) \
	do \
	{ \
		OSA_print(RED fmt NONE, ## args);\
	} \
	while(0)	

	
#define OSA_INFOBLUE(fmt, args ...) \
	do \
	{ \
		OSA_print(BLUE "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
	} \
	while(0)

#define OSA_INFOBLUER(fmt, args ...) \
    do \
    { \
        OSA_print(BLUE fmt NONE, ## args);\
    } \
    while(0)


#define OSA_INFODG(fmt, args ...) \
	do \
	{ \
		OSA_print(DARY_GRAY "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
	} \
	while(0)


#define OSA_INFODGR(fmt, args ...) \
    do \
    { \
        OSA_print(DARY_GRAY fmt NONE, ## args);\
    } \
    while(0)


#define OSA_INFOCYAN(fmt, args ...) \
    do \
    { \
        OSA_print(CYAN "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
    } \
    while(0)


#define OSA_INFOCYANR(fmt, args ...) \
    do \
    { \
        OSA_print(CYAN fmt NONE, ## args);\
    } \
    while(0)
        

#define OSA_INFOBROWN(fmt, args ...) \
    do \
    { \
        OSA_print(BROWN "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
    } \
    while(0)

#define OSA_INFOBROWNR(fmt, args ...) \
    do \
    { \
        OSA_print(BROWN fmt NONE, ## args);\
    } \
    while(0)


#define OSA_INFOBYELLOW(fmt, args ...) \
    do \
    { \
        OSA_print(YELLOW "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
    } \
    while(0)

#define OSA_INFOBYELLOWR(fmt, args ...) \
    do \
    { \
        OSA_print(YELLOW fmt NONE, ## args);\
    } \
    while(0)


#define OSA_INFOBWHITE(fmt, args ...) \
    do \
    { \
        OSA_print(WHITE "[" OSA_MODULE_NAME "] " fmt NONE, ## args);\
    } \
    while(0)

#define OSA_INFOBWHITER(fmt, args ...) \
    do \
    { \
        OSA_print(WHITE fmt NONE, ## args);\
    } \
    while(0)


#endif  /* _OSA_TRACE_H_ */


