/*******************************************************************************
* osa_test_sem.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSA模块信号量测试代码
*       测试信号量对资源的保护
*       创建两个线程，竞争同一个信号量sem，获得信号量后对全局变量gData进行赋值，然后看是否会被修改.
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
static OSA_ThrHandle ghThr1 = NULL;
static OSA_ThrHandle ghThr2 = NULL;
static OSA_SemHandle ghSem = NULL;
static Uint32 gData = 0;

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */







static Int32 OSA_testSemProc1(Ptr data)
{
    OSA_INFO("Thread1 Created, pid:%d, tid:%d\n", OSA_thrGetPid(),OSA_thrGetTid());

    while(!OSA_thrShouldStop())
    {
        OSA_semPend(ghSem, (Uint32)OSA_TIMEOUT_FOREVER);

        gData = 0x50505050;
        OSA_msleep(1);
        if(gData != 0x50505050)
        {
            OSA_WARN("sem test failed\n");
        }    

        OSA_semPost(ghSem);
    
        OSA_msleep(1000);
    }

    return OSA_SOK;
}

static Int32 OSA_testSemProc2(Ptr data)
{
    OSA_INFO("Thread2 Created, pid:%d, tid:%d\n", OSA_thrGetPid(),OSA_thrGetTid());

    while(!OSA_thrShouldStop())
    {
        OSA_semPend(ghSem, (Uint32)OSA_TIMEOUT_FOREVER);

        gData = 0x05050505;
        OSA_msleep(1);
        if(gData != 0x05050505)
        {
            OSA_WARN("sem test failed\n");
        }    

        OSA_semPost(ghSem);
    
        OSA_msleep(1000);
    }

    return OSA_SOK;    
}



Int32 OSA_testSemBegin(void)
{
    OSA_ThrCreate param;
    Int32 status;

    /*创建信号量*/
    status = OSA_semCreate(1, &ghSem);
    if(OSA_SOK != status)
    {
        OSA_ERROR("create sem failed\n");
        return OSA_EFAIL;
    }

    /*创建线程1*/    
    OSA_clear(&param);
    param.OpThrRun = OSA_testSemProc1;
    param.thrPol = OSA_SCHED_FIFO;
    param.thrPri = OSA_THR_PRI_MAX(OSA_SCHED_FIFO);
    param.stackSize = OSA_THR_STACK_SIZE_DEFAULT;
    param.pUsrArgs = 0;

    status = OSA_thrCreate(&param, &ghThr1);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Could not Ctreat Thread!\n");
        return OSA_EFAIL;
    }

    /*创建线程2*/    
    OSA_clear(&param);
    param.OpThrRun = OSA_testSemProc2;
    param.thrPol = OSA_SCHED_FIFO;
    param.thrPri = OSA_THR_PRI_MAX(OSA_SCHED_FIFO);
    param.stackSize = OSA_THR_STACK_SIZE_DEFAULT;
    param.pUsrArgs = 0;
    
    status = OSA_thrCreate(&param, &ghThr2);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Could not Ctreat Thread!\n");
        return OSA_EFAIL;
    }

    return OSA_SOK;
}

Int32 OSA_testSemEnd(void)
{
    OSA_thrDelete(ghThr1);
    OSA_thrDelete(ghThr2);

    OSA_semDelete(ghSem);

    return OSA_SOK;
}





