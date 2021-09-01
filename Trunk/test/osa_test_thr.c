/*******************************************************************************
* osa_test_thr.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSA模块线程测试代码
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
static OSA_ThrHandle ghThr = NULL;
/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */
static Int32 OSA_testThrProc(Ptr data)
{
    OSA_INFO("Thread Created, pid:%d, tid:%d\n", OSA_thrGetPid(),OSA_thrGetTid());

    while(!OSA_thrShouldStop())
    {
        //OSA_INFO("Thread Running, data:0x%x\n", (Int32)data);
        OSA_INFO("Thread Running, data:0x%p\n", data);
    
        OSA_msleep(1000);
    }

    return OSA_SOK;
}


Int32 OSA_testThrBegin(void)
{
    OSA_ThrCreate param;
    Int32 status;

    OSA_clear(&param);

    param.OpThrRun = OSA_testThrProc;
    param.thrPol = OSA_SCHED_RR;
    param.thrPri = OSA_THR_PRI_MAX(OSA_SCHED_RR);
    param.stackSize = OSA_THR_STACK_SIZE_DEFAULT;
    param.pUsrArgs = (Ptr)0x12345678;

    status = OSA_thrCreate(&param, &ghThr);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Could not Ctreat Thread!\n");
        return OSA_EFAIL;
    } 

    return OSA_SOK;
}

Int32 OSA_testThrEnd(void)
{
    OSA_thrDelete(ghThr);

    return OSA_SOK;
}




