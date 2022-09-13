/*******************************************************************************
* osa_test_mutex.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSA模块互斥锁测试代码
*       
*       1) 测试互斥锁对资源的保护, 创建两个线程，竞争同一个把锁，获得锁后对全局变量gData进行赋值，
*           然后看是否会被修改.
*
*       2) 测试递归锁是否能够生效，创建一个线程，线程体中对一把递归锁循环加锁，看是否会死锁
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
static OSA_ThrHandle ghThr1 = NULL;
static OSA_ThrHandle ghThr2 = NULL;
static OSA_MutexHandle ghLock12 = NULL;
static Uint32 gData = 0;

static OSA_ThrHandle ghThr3 = NULL;
static OSA_MutexHandle ghLock3 = NULL;



/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */







static Int32 OSA_testMutexProc1(Ptr data)
{
    OSA_INFO("Thread1 Created, pid:%d, tid:%d\n", OSA_thrGetPid(),OSA_thrGetTid());

    while(!OSA_thrShouldStop())
    {
        OSA_mutexLock(ghLock12);

        gData = 0x50505050;
        OSA_msleep(1);
        if(gData != 0x50505050)
        {
            OSA_WARN("mutex test failed\n");
        }    

        OSA_mutexUnlock(ghLock12);
    
        OSA_msleep(1000);
    }

    return OSA_SOK;
}

static Int32 OSA_testMutexProc2(Ptr data)
{
    OSA_INFO("Thread2 Created, pid:%d, tid:%d\n", OSA_thrGetPid(),OSA_thrGetTid());

    while(!OSA_thrShouldStop())
    {
        OSA_mutexLock(ghLock12);

        gData = 0x05050505;
        OSA_msleep(1);
        if(gData != 0x05050505)
        {
            OSA_WARN("mutex test failed\n");
        }    

        OSA_mutexUnlock(ghLock12);
    
        OSA_msleep(1000);
    }

    return OSA_SOK;    
}


static Int32 OSA_testMutexProc3(Ptr data)
{
    Int32 i = 0;
    
    OSA_INFO("Thread3 Created, pid:%d, tid:%d\n", OSA_thrGetPid(),OSA_thrGetTid());

    while(!OSA_thrShouldStop())
    {
        for(i=0; i < 10; i++)
        {
            OSA_mutexLock(ghLock3);            
        }

        OSA_INFO("Thread3 In Lock\n");
    
        for(i=0; i < 10; i++)
        {
            OSA_mutexUnlock(ghLock3);            
        }
    
        OSA_msleep(1000);
    }

    return OSA_SOK;    
}



Int32 OSA_testMutexBegin(void)
{
    OSA_ThrCreate param;
    Int32 status;

    /*创建互斥锁*/
    status = OSA_mutexCreate(OSA_MUTEX_NORMAL, &ghLock12);
    if(OSA_SOK != status)
    {
        OSA_ERROR("create mutex failed\n");
        return OSA_EFAIL;
    }

    /*创建线程1*/
    OSA_clear(&param);
    param.OpThrRun = OSA_testMutexProc1;
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
    param.OpThrRun = OSA_testMutexProc2;
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

    /*创建递归锁*/
    status = OSA_mutexCreate(OSA_MUTEX_RECURSIVE, &ghLock3);
    if(OSA_SOK != status)
    {
        OSA_ERROR("create mutex failed\n");
        return OSA_EFAIL;
    }

    /*创建线程3*/    
    OSA_clear(&param);
    param.OpThrRun = OSA_testMutexProc3;
    param.thrPol = OSA_SCHED_FIFO;
    param.thrPri = OSA_THR_PRI_MAX(OSA_SCHED_FIFO);
    param.stackSize = OSA_THR_STACK_SIZE_DEFAULT;
    param.pUsrArgs = 0;
    
    status = OSA_thrCreate(&param, &ghThr3);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Could not Ctreat Thread!\n");
        return OSA_EFAIL;
    }

    return OSA_SOK;
}

Int32 OSA_testMutexEnd(void)
{
    OSA_thrDelete(ghThr1);
    OSA_thrDelete(ghThr2);
    OSA_thrDelete(ghThr3);

    OSA_mutexDelete(ghLock12);
    OSA_mutexDelete(ghLock3);

    return OSA_SOK;
}





