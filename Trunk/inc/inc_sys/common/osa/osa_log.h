/*******************************************************************************
* osa_log.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng Wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-10-30 Create
*
* Description: OSA的日志接口
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


#ifndef _OSA_LOG_H_
#define _OSA_LOG_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#ifdef _WIN32
#include <stdarg.h>
#include <stdio.h>
#include "osa_types.h"
#include "osa_macro.h"
#endif

#include "osa_mutex.h"
#include "osa_list.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/*日志级别定义*/
typedef enum
{
    OSA_LOG_LV_ERR = 0,            /*错误打印*/
    OSA_LOG_LV_WARN,               /*警告打印*/
    OSA_LOG_LV_INFO,               /*通告打印*/
    OSA_LOG_LV_DBG,                /*调试打印*/
    
    OSA_LOG_LV_MAX
}OSA_LogLevel;

/*日志颜色定义*/
typedef enum
{
    OSA_LOG_CLR_NONE = 0,          /*无颜色*/
    OSA_LOG_CLR_RED,
    OSA_LOG_CLR_GREEN,
    OSA_LOG_CLR_BLUE,
    OSA_LOG_CLR_GRAY,
    OSA_LOG_CLR_CYAN,
    OSA_LOG_CLR_PURPLE,
    OSA_LOG_CLR_BROWN,
    OSA_LOG_CLR_YELLOW,
    OSA_LOG_CLR_WHITE,

    OSA_LOG_CLR_MAX
}OSA_LogColor;

/* 定义日志颜色控制字符 */
#define LOG_CLRSTR_NONE          "\033[m"   
#define LOG_CLRSTR_RED           "\033[0;32;31m"   
#define LOG_CLRSTR_GREEN         "\033[0;32;32m"   
#define LOG_CLRSTR_BLUE          "\033[0;32;34m"   
#define LOG_CLRSTR_DARY_GRAY     "\033[1;30m"   
#define LOG_CLRSTR_CYAN          "\033[0;36m"   
#define LOG_CLRSTR_PURPLE        "\033[0;35m"     
#define LOG_CLRSTR_BROWN         "\033[0;33m"   
#define LOG_CLRSTR_YELLOW        "\033[1;33m"   
#define LOG_CLRSTR_WHITE         "\033[1;37m"

typedef enum {
	OSA_LOGMANAGER_DISABLE,     /* 关闭模块日志功能 */
	OSA_LOGMANAGER_ENABLE,      /* 使能模块日志使能 */
	OSA_LOGMANAGER_ENABLE_MAX
}OSA_LogManangerEnable;

#define OSA_LOGENTITY_NAMELEN 48
#define OSA_LOGMANAGER_EXTEND 64

typedef struct OSA_LogManagerCommon_t {	
	Char osa_Name[OSA_LOGENTITY_NAMELEN];/* 模块的名称，取模块OSA_MODULE_NAME定义@字符前部分 */
	Uint8 osa_LogLevel;                /* 模块内部打印等级 */
	Uint8 osa_PrintEnable;             /* 模块打印使能，可设置关闭整个模块打印 */
	Uint8 osa_Stat;                     /* 模块打印量统计功能开关 */
	Int8 osa_Refcnt;                   /* 模块日志管理引用计数 */
}OSA_LogManagerCommon;

typedef struct OSA_LogManager_t {
	OSA_LogManagerCommon info;
	OSA_ListHead  node;
#ifdef __KERNEL__
	atomic_t osa_PrintBytes;      /* 内核模块打印统计计数 */
#else
	Uint64 osa_PrintBytes;      /* 用户态库打印统计计数 */
	OSA_MutexHandle hMutex;
#endif
	Uint8 reserve[OSA_LOGMANAGER_EXTEND];  /* 后期扩展 */
}OSA_LogManager;

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/*日志操作接口*/
typedef struct
{
    Int32 (*OpWrite)(Uint32 level, Uint32 color, const Char *pFmt, va_list args);

    Int32 reserved[4];
}OSA_LogOps;

/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_logRegister
* 描  述  : 该函数向OSA注册日志操作接口，通常由日志管理模块在初始化时调用。
*           在未向OSA注册之前，所有打印都使用OSA模块内部的函数。
*           当日志管理模块启动后，会调用本接口向OSA注册，
*           之后，所有的日志打印都通过日志管理模块的接口实现了。
*
* 输  入  : - pLogOps:  日志操作方法
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_logRegister(OSA_LogOps *pLogOps);

/*******************************************************************************
* 函数名  : OSA_logUnRegister
* 描  述  : 该函数向OSA取消注册日志操作接口，通常由日志管理模块在退出时调用。
*           当日志管理模块取消注册后，所有的日志打印恢复到用OSA模块内部的函数
*
* 输  入  : 无。
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_logUnRegister(void);


/*******************************************************************************
* 函数名  : OSA_logWrite
* 描  述  : 该函数提供统一打印接口 
* 输  入  : - level:   打印等级，参见 OSA_LogLevel定义
*           - color:   打印颜色，参见 OSA_LogColor定义
*           - pFmt:    打印的格式字符串
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_logWrite(Uint32 level, Uint32 color, const Char *pFmt, ...);

/******************************************************************************
* 函数名  :  OSA_logManagerRegister
* 描  述  : 该函数向OSA注册模块日志管理结构，通常在模块/库在初始化时调用。
*           对未向OSA注册之前，所有模块打印默认关闭。
*           当模块启动调用本接口向OSA注册管理结构后，
*           所有的日志打印都通过日志管理模块的接口进行管理。
*
* 输  入  : OSA_LogManager:  待注册的日志管理结构对象
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_logManagerRegister(OSA_LogManager *pLogManager);

/******************************************************************************
* 函数名  :  OSA_logManagerDeRegister
* 描  述  : 该函数向OSA注销模块日志管理结构，只会在模块注销时调用。
*           当模块被注销时，不对管理结构内存进行释放，只是对引用计数进行处理
*           重新加载模块后，可按同名匹配后复用原来分配的内存对象。
*
* 输  入  : OSA_LogManager:  待注册的日志管理结构对象
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_logManagerDeRegister(OSA_LogManager *pLogManager);

/******************************************************************************
* 函数名  :  OSA_LogManagerSetEntryPrintLevel 
* 描  述  : 该函数向OSA管理的对应模块/库设置其打印等级 
*           无锁设计，调用者需要保证保证配置安全
*      
* 输  入  : modName,:    待操作模块名称
* 输  入  : level:   打印等级,等级定义为来自枚举OSA_LogLevel中的定义
* 输  出  : 
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerSetEntryPrintLevel(const Char *modName, Int32 level);

/******************************************************************************
* 函数名  :  OSA_LogManagerGetEntryPrintLevel
* 描  述  : 该函数获取OSA管理的对应模块/库的打印等级 
*           无锁设计，调用者需要保证保证配置安全
*      
* 输  入  : modName,:    待操作模块名称
* 输  入  : level:   返回模块打印等级,等级定义为来自枚举OSA_LogLevel中的定义
* 输  出  : 
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerGetEntryPrintLevel(const Char *modName, Int32 *level);

/******************************************************************************
* 函数名  :  OSA_LogManagerSetEntryEnable
* 描  述  : 该函数设置OSA管理的对应模块/库打印使能或关闭
*           无锁设计，调用者需要保证保证配置安全
*      
* 输  入  : modName,:    待操作模块名称
* 输  入  : status:   使能或关闭模块/库的打印输出
* 输  出  : 
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerSetEntryEnable(const Char *modName, OSA_LogManangerEnable status);

/******************************************************************************
* 函数名  :  OSA_LogManagerGetEntryPrintStatics
* 描  述  : 该函数获取对应模块/库相关打印统计量
*           用户态统计计数加锁保护，内核态使用原子变量 
*           当前主要可获取模块/库的打印统计计数
*      
* 输  入  : modName,:    待操作模块名称
* 输  出  : printedBytes:   返回模块/库打印计数
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerGetEntryPrintStatics(const Char *modName, Uint64 *printedBytes);

/******************************************************************************
* 函数名  :  OSA_LogManagerGetEntryInfo
* 描  述  : 该函数获取OSA所管理的单个模块/库信息
* 
* 
* 输  入  : pentry 填入需要获取信息模块/库的名称
* 输  出  : pentry 返回指定模块/库的信息,不包括该模块打印统计量
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerGetEntryInfo(OSA_LogManagerCommon *pentry);

/******************************************************************************
* 函数名  :  OSA_LogManagerSetEntryInfo
* 描  述  : 该函数设置OSA所管理的单个模块/库管理功能
* 
* 输  入  : pentry 设置OSA管理功能选项
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerSetEntryInfo(OSA_LogManagerCommon *pentry);

/******************************************************************************
* 函数名  :  OSA_LogManagerGetAllEntryInfoSize
* 描  述  : 该函数获取OSA所管理的所有模块/库管理结构占用内存空间
*           目的在于便于调用者获取管理结构信息时，分配足够内存空间
* 
* 输  入  : 无  
* 输  出  : 无
* 返回值  : OSA所管理所有模块/库核心结构信息占用空间
******************************************************************************/
Int32 OSA_LogManagerGetAllEntryInfoSize(void);

/******************************************************************************
* 函数名  :  OSA_LogManagerGetAllEntryInfo
* 描  述  : 该函数获取OSA所管理的所有模块/库当前所总输出打印量
* 
* 
* 输  入  : size 为buf的大小
* 输  出  : pentry 用于返回OSA所管理模块/库的数据结构数组，pentry必须足够可容纳
		   : 否则将获取失败，返回获取信息所需要缓存大小
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
			> 0的值表示参数buf所提供的空间不能满足返回结构化信息所需空间，此时
			返回所需的空间
******************************************************************************/
Int32 OSA_LogManagerGetAllEntryInfo(OSA_LogManagerCommon *pentry, size_t size);

/******************************************************************************
* 函数名  :  OSA_LogManagerDisableAllEntryStat
* 描  述  : 该函数关闭/打开OSA所管理的所有模块/库输出打印量统计功能
*           用户态打印统计功能基于锁的性能开销，增加该接口关闭统计功能
* 
* 输  入  :  无
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerDisableAllEntryStat(OSA_LogManangerEnable status);

/******************************************************************************
* 函数名  :  OSA_LogManagerUserInit
* 描  述  : 该函数为OSA管理库的初始化接口
* 
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerUserInit(void);

/******************************************************************************
* 函数名  :  OSA_LogManagerUserExit
* 描  述  : 该函数为OSA管理功能注销接口
*           该接口将所有用户应用库所有模块注销
* 
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerUserExit(void);


#ifdef __cplusplus
    }
#endif

#endif /* _OSA_LOG_H_ */
