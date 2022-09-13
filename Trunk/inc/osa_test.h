/*******************************************************************************
* osa_test.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: 定义OSA模块测试代码头文件
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_TEST_H_
#define _OSA_TEST_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/*定义字符设备的命令宏*/
#define OSA_KCDEV_BASE_CMD         'O'

/*
 * 字符设备的命令ID
 */
#define CMD_OSA_KCDEV_TESTPRINT         _IOWR(OSA_KCDEV_BASE_CMD,\
                                        0u,\
                                        Uint32)


/*定义测试类型*/
typedef enum
{
    OSA_TEST_THR = 0,   /*测试osa_thr.h接口*/
    OSA_TEST_SEM,       /*测试osa_sem.h接口*/
    OSA_TEST_TIMER,     /*测试osa_timer.h接口*/
    OSA_TEST_MUTEX,     /*测试osa_mutex.h接口*/    
    OSA_TEST_KPROC,     /*测试osa_kproc.h接口*/
    OSA_TEST_KCDEV,     /*测试osa_kcdev.h接口*/
    OSA_TEST_MEM,       /*测试osa_mem.h接口*/
    OSA_TEST_MAX
}OSA_TestType;


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

Int32 OSA_testBegin(Int32 type);
Int32 OSA_testEnd(Int32 type);

/*测试线程的接口*/
Int32 OSA_testThrBegin(void);
Int32 OSA_testThrEnd(void);

/*测试信号量的接口*/
Int32 OSA_testSemBegin(void);
Int32 OSA_testSemEnd(void);

/*测试定时器的接口*/
Int32 OSA_testTimerBegin(void);
Int32 OSA_testTimerEnd(void);

/*测试互斥锁的接口*/
Int32 OSA_testMutexBegin(void);
Int32 OSA_testMutexEnd(void);

/*测试linux内核proc的接口*/
Int32 OSA_testKProcBegin(void);
Int32 OSA_testKProcEnd(void);

/*测试linux内核字符设备的接口*/
Int32 OSA_testKCdevBegin(void);
Int32 OSA_testKCdevEnd(void);

Int32 OSA_testMemBegin(void);
Int32 OSA_testMemEnd(void);


#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_TEST_H_  */



