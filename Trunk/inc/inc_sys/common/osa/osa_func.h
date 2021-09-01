/*******************************************************************************
* osa_func.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA模块全局初始化和设置接口。包括了所有OSA系列头文件。
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
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef __OSA_FUNC_H__
#define __OSA_FUNC_H__


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */


#include <osa_types.h>
#include <osa_macro.h>
#include <osa_log.h>
#include <osa_trace_level.h>

#include <osa_list.h>
#include <osa_string.h>
#include <osa_mem.h>
#include <osa_sem.h>
#include <osa_mutex.h>
#include <osa_file.h>
#include <osa_cmpl.h>
#include <osa_time.h>
#include <osa_prf.h>
#include <osa_irq.h>
#include <osa_timer.h>
#include <osa_reg.h>
#include <osa_que.h>
#include <osa_msgq.h>
#include <osa_mbx.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_flg.h>
#include <osa_spinlock.h>
#include <osa_kwait.h>
#include <osa_kcdev.h>
#include <osa_kproc.h>
#include <osa_ktasklet.h>
#include <osa_kworkque.h>
#include <osa_uipc.h>
#include <osa_umisc.h>
#include <osa_usignal.h>
#include <osa_uwait.h>
#include <osa_cache.h>
#include <osa_pm.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* OSA的版本号，可用于检测兼容性。*/
#define OSA_VERSION  \
            (OSA_versionSet(1, 0, 0))


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

typedef struct
{
    /* 是否开启监测任务，用于监测其他任务是否超时及其他异常情况。*/
    Bool32 isStartMonTask;

    Uint32 reserved[8];
} OSA_initParms;


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_init
* 描  述  : 初始化osa。Linux应用层和SYSBIOS的程序在使用osa其他接口前必须调用
*           该接口，Linux驱动不必调用，也调用不了，符号没有导出。
* 输  入  : - pInitParms: 初始化参数。
* 输  出  : 无。
* 返回值  : OSA_SOK: 校验成功。
*           OSA_EFAIL: 校验失败。
*******************************************************************************/
Int32 OSA_init(OSA_initParms *pInitParms);


/*******************************************************************************
* 函数名  : OSA_deinit
* 描  述  : 销毁osa。必须与OSA_init()配对使用。
* 输  入  : 无。
* 输  出  : 无。
* 返回值  : OSA_SOK: 校验成功。
*           OSA_EFAIL: 校验失败。
*******************************************************************************/
Int32 OSA_deinit(void);


/*******************************************************************************
* 函数名  : OSA_validate
* 描  述  : 校验OSA版本的兼容性。
*           通过将此头文件定义的版本号OSA_VERSION传递到该函数内部
*           进行比较，校验此头文件与相应库的兼容性。在该函数内部，如果版本号
*           相等，使用绿色字体打印出版本号，如果不相等但可兼容，则使用黄色字
*           体打印出警告信息，如果不兼容，则使用红色字体打印出错信息。前两种
*           情况函数返回正确值，最后一种情况返回错误值。所以调用者必须检查返
*           回值，以确认版本的兼容性。
* 输  入  : - nVersion: 头文件中定义的版本号OSA_VERSION。
* 输  出  : 无。
* 返回值  : OSA_SOK: 校验成功。
*           OSA_EFAIL: 校验失败。
*******************************************************************************/
Int32 OSA_validate(Uint32 nVersion);


#ifdef __cplusplus
}
#endif


#endif /* __OSA_FUNC_H__ */



