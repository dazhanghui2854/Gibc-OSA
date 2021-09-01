/*******************************************************************************
* osa_time.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-2 Create
*
* Desc: 实现linux用户态OSA模块对外提供的:
*       1. 延时接口
*       2. 获取系统时间接口
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
#include <osa_priv.h>


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */
static OSA_MctsRegisterFun gMctsParam =
{
    .OpGetTimeOfJiffies = NULL,
    .OpGetTimeOfDay     = NULL,
    .OpGetTimeOfDay64   = NULL,
};
/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_getTimeOfDay
* 描  述  : 获取当前时间。
*
* 输  入  : - pTimeVal: 时间表述结构，由秒和微妙组成。
*
* 输  出  : 无
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_getTimeOfDay(OSA_TimeVal *pTimeVal)
{
    Int32          status;

    if (OSA_isNull(gMctsParam.OpGetTimeOfDay))
    {
        struct timeval tv;

        OSA_assertNotNull(pTimeVal);

        status = gettimeofday(&tv, NULL);
        if (OSA_isSuccess(status))
        {
            pTimeVal->tvSec  = (Int32)tv.tv_sec;
            pTimeVal->tvUsec = (Int32)tv.tv_usec;
        }
        else
        {
            OSA_ERROR("Get time failed !\n");
        }
    }
    else
    {
        status = gMctsParam.OpGetTimeOfDay(pTimeVal);
    }
    return status;

}


/*******************************************************************************
* 函数名  : OSA_getTimeOfDay64
* 描  述  : 获取当前时间。
*
* 输  入  : - pTimeVal: 时间表述结构，由秒和微妙组成。
*
* 输  出  : 无
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_getTimeOfDay64(OSA_TimeVal64 *pTimeVal)
{
    Int32          status;

    if (OSA_isNull(gMctsParam.OpGetTimeOfDay64))
    {
        struct timeval tv;

        OSA_assertNotNull(pTimeVal);

        status = gettimeofday(&tv, NULL);
        if (OSA_isSuccess(status))
        {
            pTimeVal->tvSec  = (Int64)tv.tv_sec;
            pTimeVal->tvUsec = (Int64)tv.tv_usec;
        }
        else
        {
            OSA_ERROR("Get time failed !\n");
        }
    }
    else
    {
        status = gMctsParam.OpGetTimeOfDay64(pTimeVal);
    }
    return status;

}



/*******************************************************************************
* 函数名  : OSA_getTimeOfJiffies
* 描  述  : 获取系统单调递增的时间。
* 输  入  : 无。
* 输  出  : 无。
* 返回值  : 时间值，单位是毫秒。
*******************************************************************************/
Uint64 OSA_getTimeOfJiffies(void)
{
    if (OSA_isNull(gMctsParam.OpGetTimeOfJiffies))
    {
        /* 当前tick计数，初始化为一个中间值，处理第一次溢出 */
        static Uint64 curtick = 0x7fffffff;

        /* 先备份一下，避免多线程访问出错 */
        Uint64 tmptick = curtick;

        /* times使用空参数，减少times系统调用cpu耗时 */
        Uint64 tick = (Uint32)times(NULL);
        if (tick == (Uint32)-1)    /* 溢出，取errno的值 */
        {
            /* glibc在处理溢出时，返回的errno可能不准，引起误溢出 */
            /*tick = (Uint32)-errno;*/
            tick = tmptick;     /* 使用上一次合理的值，风险更低 */
        }

        if ((Uint32)tmptick != (Uint32)tick) /* 低32位变化说明tick变化 */
        {
            while(tick < tmptick)       /* 溢出处理 */
            {
                tick += 0xffffffff;
                tick += 1;
            }

            /* 提前处理多线程引起的时间倒流的问题，提高效率 */
            if(curtick < tick)
            {
                curtick = tick;
            }
        }

        return curtick * (1000 / (Uint32L)sysconf(_SC_CLK_TCK));

    }
    else
    {
        return gMctsParam.OpGetTimeOfJiffies();
    }
}

/*******************************************************************************
*  函数名: OSA_registerMcts
*  描   述   :注册时间同步模块的获取时间函数
*  输   入   : 无
*  输   出   :
*  返回值: OSA_SOK: 成功。
*                      OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_registerMcts(OSA_MctsRegisterFun *pFun)
{
	static Uint32 init = 0;

    if (OSA_isNull(pFun))
    {
        return OSA_EFAIL;
    }

    if (!init)
	{
		gMctsParam.OpGetTimeOfDay     = pFun->OpGetTimeOfDay;
		gMctsParam.OpGetTimeOfDay64	  = pFun->OpGetTimeOfDay64;
		gMctsParam.OpGetTimeOfJiffies = pFun->OpGetTimeOfJiffies;
		init++;
	}

    return OSA_SOK;
}
/*******************************************************************************
* 函数名  : OSA_getCurTimeInMsec
* 描  述  : 获取当前时刻的毫秒数, 使用时请注意:
*
*           1. 该函数返回的时间值建议使用OSA_timeAfter、OSA_timeBefore、OSA_timeEqual这三个宏
*              进行比较
*
*           2. 该函数返回的时间值建议使用OSA_timeDelta宏计算两个时间点的差值
*
* 输  入  : 无
*
* 输  出  : 无
* 返回值  : 当前系统毫秒数
*******************************************************************************/
Uint32 OSA_getCurTimeInMsec(void)
{
    return (Uint32)OSA_getTimeOfJiffies();
}


/*******************************************************************************
* 函数名  : OSA_msleep
* 描  述  : 毫秒级延时,延时过程中会休眠
*
* 输  入  : - ms: 毫秒数
*
* 输  出  : 无
* 返回值  : 无
*******************************************************************************/
void   OSA_msleep(Uint32 ms)
{
    struct timespec delayTime, elaspedTime;

    delayTime.tv_sec  = (time_t)(ms / 1000);
    delayTime.tv_nsec = (ms % 1000) * 1000000;

    (void)nanosleep(&delayTime, &elaspedTime);
}


