/*******************************************************************************
* osa_test_timer.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSA模块定时器测试代码，仅针对linux内核态或者sysbios有效，linux用户态下实现为空函数
*       测试步骤:
*       1) 创建一个类型为OSA_TIMER_ONCE的定时器，5秒后超时，观察超时时的打印
*       2) 创建一个类型为OSA_TIMER_LOOP的定时器, 5秒后超时，观察超时时的打印, 
*          同时在定时器回调中调用OSA_mdelay测试忙等延时
*       
*       
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include <osa.h>
#include <osa_test.h>

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            全局变量定义区                                  */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

#if defined(__KERNEL__) || defined (___DSPBIOS___)    /* Linux 内核态和TI SYSBIOS支持*/

static OSA_TimerHandle ghTmr1 = NULL;
static OSA_TimerHandle ghTmr2 = NULL;

static Int32 OSA_testTimerProc(OSA_TimerHandle hTimer, Ptr pUserArgs)
{
    if(hTimer == ghTmr1)
    {
        //OSA_INFO("Timer1 expired, args:0x%x, timestamp:%u\n", 
        //   (Uint32)pUserArgs, OSA_getCurTimeInMsec());
        OSA_INFO("Timer1 expired, args:0x%p, timestamp:%u\n", 
             pUserArgs, OSA_getCurTimeInMsec());
    }
    else if(hTimer == ghTmr2)
    {
        OSA_INFO("Timer2 expired, args:0x%p, timestamp:%u\n", 
            pUserArgs, OSA_getCurTimeInMsec());
    }
    else
    {
        OSA_ERROR("Invalid Timer Handle:%p\n", (Ptr)hTimer);
    }

    OSA_mdelay(1);
    OSA_INFO("timestamp after delay:%u\n", OSA_getCurTimeInMsec());

    return OSA_SOK;
}

Int32 OSA_testTimerBegin(void)
{
    OSA_TimerInitParams params;
    Int32 ret = OSA_SOK;
    
    params.type = OSA_TIMER_ONCE;
    params.userFunc = OSA_testTimerProc;
    params.pUserArgs = (Ptr)0x12345678;
    params.expireMs = 5000;

    ret = OSA_timerCreate(&params, &ghTmr1);
    if(OSA_SOK != ret)
    {
        OSA_ERROR("create timer1 failed\n");
        return OSA_EFAIL;
    }

    params.type = OSA_TIMER_LOOP;
    params.userFunc = OSA_testTimerProc;
    params.pUserArgs = (Ptr)0x12345678;
    params.expireMs = 5000;
    
    ret = OSA_timerCreate(&params, &ghTmr2);
    if(OSA_SOK != ret)
    {
        OSA_ERROR("create timer2 failed\n");
        return OSA_EFAIL;
    }

    OSA_timerStart(ghTmr1);
    OSA_timerStart(ghTmr2);

    OSA_INFO("timer1 and timer2 started, timestamp:%u\n",
        OSA_getCurTimeInMsec());

    return OSA_SOK;
}
Int32 OSA_testTimerEnd(void)
{
    OSA_timerDelete(ghTmr1);
    OSA_timerDelete(ghTmr2);

    return OSA_SOK;
}

#else
Int32 OSA_testTimerBegin(void)
{
    return OSA_SOK;
}
Int32 OSA_testTimerEnd(void)
{
    return OSA_SOK;
}

#endif

