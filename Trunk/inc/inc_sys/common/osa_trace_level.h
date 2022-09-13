/*******************************************************************************
* osa_trace_level.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2011-07-25 Create
*          V1.0.1  2011-11-14 1. 增加对TI SYSBIOS系统的支持。                            
*                             2. 将DEBUG宏修改为_OSA_TRACE_。
*                             3. 文件名由osa_debug.h修改为osa_trace.h。
*          V1.0.2  2011-11-30 1. 增加程序编译时用断言OSA_COMPILETIME_ASSERT。
*          V1.0.3  2012-08-10 1. 增加等级控制。
*          V1.0.4  2012-12-05 1. 支持日志管理模块。
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


/* 
 模块的名字必须在Makefile或其他头文件中定义，名字不需要加上[]。
 在Makefile中添加的方法是：
 OSA_MODULE_NAME = XXX
 CFLAGS       += -DOSA_MODULE_NAME='"$(OSA_MODULE_NAME)"'
 EXTRA_CFLAGS += -D'OSA_MODULE_NAME=\"$(OSA_MODULE_NAME)\"'
 应用程序中使用CFLAGS，驱动程序中使用EXTRA_CFLAGS。
*/
#ifndef OSA_MODULE_NAME
#warning Not define OSA_MODULE_NAME !!!
#define OSA_MODULE_NAME "UNKNOWN"
#endif

#define OSA_print OSA_logWrite


/* _OSA_TRACE_宏是调试开关，可通过Makeife传入，传入方法是-D_OSA_TRACE_。*/
#ifdef _OSA_TRACE_
    /* 断言，输出字体为红色，打印出文件名、函数名和行号。程序进入无限循环。*/ 
	#define OSA_assert(x)  \
	    if((x) == 0) 	   \
	    	{ 			   \
	    	OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED,  \
	    	        "[" OSA_MODULE_NAME "] " "ASSERT (%s|%d)\r\n", \
	    	                __func__, __LINE__); \
	    	while(1);     \
	    } 
		
	#define OSA_assertSuccess(status)	\
	    OSA_assert(OSA_SOK == (status))
    #define OSA_assertFail(status)  \
	    OSA_assert(OSA_SOK != (status))

    #define OSA_assertNotNull(ptr)  \
	    OSA_assert(NULL != (ptr))
    #define OSA_assertNull(ptr)  \
        OSA_assert(NULL == (ptr)) 

    /* 调试打印，输出字体不带颜色, 不带时间*/
	#define OSA_DEBUG(fmt, args ...) \
		OSA_print(OSA_LOG_LV_DBG, OSA_LOG_CLR_NONE,"[" OSA_MODULE_NAME "] " fmt, ## args)

#else
	#define OSA_assert(status)
	#define OSA_assertSuccess(status)
	#define OSA_assertFail(status)
	#define OSA_assertNotNull(ptr)
	#define OSA_assertNull(ptr)
#ifndef _WIN32
	#define OSA_DEBUG(fmt, args ... ) 
#else
	#define OSA_DEBUG(fmt, ... ) 
	#define __func__ __FUNCTION__
#endif
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
    if((x) == 0)         \
    { \
        OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
            "ASSERT (%s|%d)\r\n", __func__, __LINE__); \
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
#define OSA_COMPTIME_ASSERT(condition)  \
    do                                  \
    {                                   \
       typedef char ErrorCheck[((condition) == OSA_TRUE) ? 1 : -1]; \
    } \
    while(0)

#ifndef _WIN32

/* 以下打印不管是否开启调试模式，都打印输出。*/

/* 通用打印，输出字体不带颜色, 不带时间，不带模块名称 */
#define OSA_printf(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_DBG, OSA_LOG_CLR_NONE, fmt, ## args);\
	} \
	while(0)

/* 出错打印，输出字体为红色，打印出文件名、函数名和行号,表明程序不能继续运行。*/
#define OSA_ERROR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
		              "ERROR  (%s|%d): " \
		              fmt, __func__, __LINE__, ## args); \
	} \
	while(0)
 
/* 出错打印，输出字体为红色。*/
#define OSA_ERRORR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
		              "ERROR  : " \
		              fmt, ## args); \
	} \
	while(0)	

/* 
* 警告打印，输出字体为黄色，打印出文件名、函数名和行号。表明程序仍可继续运行，
* 但须警示。
*/
#define OSA_WARN(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
		                    "[" OSA_MODULE_NAME "] " "WARN  (%s|%d): " \
		                    fmt, __func__, __LINE__, ## args); \
	} \
	while(0)

/* 警告打印，输出字体为黄色。表明程序仍可运行下去，但须警示。*/
#define OSA_WARNR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
		                    "[" OSA_MODULE_NAME "] " "WARN  : " \
		                    fmt, ## args); \
	} \
	while(0)

/* 信息通告打印，输出字体为绿色。*/
#define OSA_INFO(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)


/*  
* 以上打印为各模块强制使用，以下打印给模块可选择使用，也可自行添加打印方式，
* 但打印必须打印出模块名字，除特殊应用情况。后缀加上字母R的表示不打印模块名。
*/
#define OSA_INFOR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, fmt, ## args);\
	} \
	while(0)


#define OSA_INFOPUP(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)

#define OSA_INFOPUPR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, fmt, ## args);\
    } \
    while(0)


#define OSA_INFORED(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)
	
#define OSA_INFOREDR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, fmt, ## args);\
	} \
	while(0)	

	
#define OSA_INFOBLUE(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)

#define OSA_INFOBLUER(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, fmt, ## args);\
    } \
    while(0)


#define OSA_INFODG(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)


#define OSA_INFODGR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, fmt, ## args);\
    } \
    while(0)


#define OSA_INFOCYAN(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)


#define OSA_INFOCYANR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, fmt, ## args);\
    } \
    while(0)
        

#define OSA_INFOBROWN(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)

#define OSA_INFOBROWNR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, fmt, ## args);\
    } \
    while(0)


#define OSA_INFOBYELLOW(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)

#define OSA_INFOBYELLOWR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, fmt, ## args);\
    } \
    while(0)


#define OSA_INFOBWHITE(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)

#define OSA_INFOBWHITER(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, fmt, ## args);\
    } \
    while(0)

#else

/* 以下打印不管是否开启调试模式，都打印输出。*/

/* 通用打印，输出字体不带颜色, 不带时间，不带模块名称 */
#define OSA_printf(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_DBG, OSA_LOG_CLR_NONE, fmt, ##__VA_ARGS__);\
	} \
	while(0)

/* 出错打印，输出字体为红色，打印出文件名、函数名和行号,表明程序不能继续运行。*/
#define OSA_ERROR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
    	"ERROR  (%s|%d): " \
    	fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	} \
	while(0)

/* 出错打印，输出字体为红色。*/
#define OSA_ERRORR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
    	"ERROR  : " \
    	fmt, ##__VA_ARGS__); \
	} \
	while(0)	

/* 
* 警告打印，输出字体为黄色，打印出文件名、函数名和行号。表明程序仍可继续运行，
* 但须警示。
*/
#define OSA_WARN(fmt,...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
    	"[" OSA_MODULE_NAME "] " "WARN  (%s|%d): " \
    	fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	} \
	while(0)

/* 警告打印，输出字体为黄色。表明程序仍可运行下去，但须警示。*/
#define OSA_WARNR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
    	"[" OSA_MODULE_NAME "] " "WARN  : " \
    	fmt, ##__VA_ARGS__); \
	} \
	while(0)

/* 信息通告打印，输出字体为绿色。*/
#define OSA_INFO(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)


/*  
* 以上打印为各模块强制使用，以下打印给模块可选择使用，也可自行添加打印方式，
* 但打印必须打印出模块名字，除特殊应用情况。后缀加上字母R的表示不打印模块名。
*/
#define OSA_INFOR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOPUP(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOPUPR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFORED(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOREDR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, fmt, ##__VA_ARGS__);\
	} \
	while(0)	


#define OSA_INFOBLUE(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBLUER(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFODG(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFODGR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOCYAN(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOCYANR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOBROWN(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBROWNR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOBYELLOW(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBYELLOWR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOBWHITE(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBWHITER(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, fmt, ##__VA_ARGS__);\
	} \
	while(0)
#endif

#endif  /* _OSA_TRACE_H_ */


