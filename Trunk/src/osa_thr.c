/*******************************************************************************
* osa_thr.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: linux用户态提供的OSA线程接口。
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


#include <osa.h>
#include <osa_priv.h>

typedef struct
{
    Uint16    thrPol;    /* 调度策略，其定义见OSA_thrSchedPolicy */
    Uint16    thrPri;
    pthread_t thread;
    Char      name[OSA_THREAD_NAME_MAX_LEN];
    Int32     (*OpThrRun)(Ptr pUsrArgs); /* 线程运行函数 */
    Ptr       pUsrArgs;                  /* 用户自定义参数 */
} OSA_ThrObj;



static OSA_ThrObj *OSA_thrGetObj(void)
{
    OSA_ThrObj *pThrObj = NULL;

    pThrObj = OSA_memAlloc(sizeof(*pThrObj));
    if(OSA_isNull(pThrObj))
    {
        OSA_ERROR("Thread Obj Malloc mem Fail !\n");
    }

    return pThrObj;
}


static void OSA_thrPutObj(OSA_ThrObj *pThrObj)
{
    if(OSA_SOK != OSA_memFree(pThrObj))
		OSA_ERROR("memory free failed\n");
}


static Int32 OSA_thrGetPolicy(Uint32 thrPol)
{
    switch (thrPol)
    {
        case OSA_SCHED_OTHER:
            return SCHED_OTHER;

        case OSA_SCHED_FIFO:
            return SCHED_FIFO;

        case OSA_SCHED_RR:
            return SCHED_RR;

        case OSA_SCHED_DEFAULT:
            return SCHED_OTHER;

        case OSA_SCHED_INHERIT:
            return SCHED_OTHER;
            
        default:
            OSA_ERROR("The schedule policy %u !\n", thrPol);
            break;
    }

    return OSA_EFAIL;
}


Int32 OSA_thrGetMaxPri(Uint32 thrPol)
{
    return sched_get_priority_max(OSA_thrGetPolicy(thrPol));
}


Int32 OSA_thrGetMinPri(Uint32 thrPol)
{
    return sched_get_priority_min(OSA_thrGetPolicy(thrPol));
}


typedef void *(*OSA_thrFuncType)(void*);


static void* OSA_thrRunBody(void * pArgs)
{
    Ptr               retPtr;
    OSA_ThreadInfo    info;
    OSA_ThrObj        *pThrObj = (OSA_ThrObj*)pArgs;


    info.tid       = OSA_thrGetTid();
    info.policy    = pThrObj->thrPol;
    info.priority  = pThrObj->thrPri;
    info.tBodyAddr = (Uint32L)(pThrObj->OpThrRun);
    info.privData  = (Ptr)pThrObj;
    info.pName     = pThrObj->name;

    OSA_addTidInfo(&info);
    retPtr = (void*)(ssize_t)(pThrObj->OpThrRun(pThrObj->pUsrArgs));
    OSA_delTidInfo(info.tid);

    return retPtr;
}

Int32 OSA_thrCreateEx(OSA_ThrCreate *pCreate, OSA_ThrHandle *phThr)
{
    Int32              i, status = OSA_SOK;
    OSA_ThrObj        *pThrObj = NULL;
    pthread_attr_t     thread_attr;
    struct sched_param schedprm;
    Int32              thrPriMax;
    Int32              thrPriMin;
    Int32              thrPriVal;
    Int32              thrPolicy;

    OSA_assertNotNull(pCreate);
    OSA_assertNotNull(phThr);

    if (OSA_isNull(pCreate) || OSA_isNull(phThr) || OSA_isNull(pCreate->pName))
    {
        OSA_ERROR("Input Parms NULL !\n");
        return OSA_EFAIL;
    }

    for (i = 0; i < (sizeof(pCreate->reserved)/sizeof(Uint32)); i++)
    {
        if (pCreate->reserved[i])
        {
            OSA_ERROR("Param 'OSA_ThrCreate.reserved' field isn't zero !\n");
            return OSA_EFAIL;
        }
    }

    pThrObj = OSA_thrGetObj();

    OSA_assertNotNull(pThrObj);

    if (OSA_isNull(pThrObj))
    {
        OSA_ERROR("Could not Get thread Obj !\n");
        return OSA_EFAIL;
    }

    pThrObj->thrPol = pCreate->thrPol;

    /* Initialize thread attributes structure */
    status = pthread_attr_init(&thread_attr);
    if(OSA_isFail(status))
    {
        OSA_ERROR("Could not initialize thread attributes !\n");
        return OSA_EFAIL;
    }

    if(pCreate->stackSize != OSA_THR_STACK_SIZE_DEFAULT)
    {
        status |= pthread_attr_setstacksize(&thread_attr,
                                            pCreate->stackSize);

#ifndef CONFIG_CKCORE  /* dh5000 gcc-lib和uc-lib库没有pthread_attr_setstack函数接口 */
        /* 指定了栈空间，必须指定栈空间大小 */
        if(pCreate->pStackAddr != OSA_NULL)
        {
            status |= pthread_attr_setstack(&thread_attr,
                                            pCreate->pStackAddr,
                                            pCreate->stackSize);
        }
#endif
    }

#ifndef __ANDROID__   /* android不支持pthread_attr_setinheritsched */

    if (pCreate->thrPol != OSA_SCHED_INHERIT)
    {
        status |= pthread_attr_setinheritsched(&thread_attr,
                                               PTHREAD_EXPLICIT_SCHED);
    }
    else
    {
        status |= pthread_attr_setinheritsched(&thread_attr,
                                               PTHREAD_INHERIT_SCHED);
    }
#endif
    thrPolicy = OSA_thrGetPolicy(pCreate->thrPol);
    if(OSA_EFAIL == thrPolicy)
    {
        OSA_ERROR("Could not get thr policy !\n");
        goto error_exit;
    }

    status |= pthread_attr_setschedpolicy(&thread_attr, thrPolicy);

    thrPriVal = (Int32)pCreate->thrPri;
    thrPriMax = OSA_THR_PRI_MAX(pCreate->thrPol);
    thrPriMin = OSA_THR_PRI_MIN(pCreate->thrPol);

    if(thrPriVal > thrPriMax)
    {
        thrPriVal = thrPriMax;
    }
    else if(thrPriVal < thrPriMin)
    {
        thrPriVal = thrPriMin;
    }

    schedprm.sched_priority = thrPriVal;

    status |= pthread_attr_setschedparam(&thread_attr, &schedprm);
    if(OSA_isFail(status))
    {
        OSA_ERROR("Could not initialize thread attributes !\n");
        goto error_exit;
    }
    pThrObj->thrPol   = pCreate->thrPol;
    pThrObj->thrPri   = thrPriVal;
    OSA_strncpy(pThrObj->name, pCreate->pName, OSA_THREAD_NAME_MAX_LEN);
    pThrObj->name[OSA_THREAD_NAME_MAX_LEN-1] = '\0';
    pThrObj->OpThrRun = pCreate->OpThrRun;
    pThrObj->pUsrArgs = pCreate->pUsrArgs;
    if (pCreate->thrPol != OSA_SCHED_DEFAULT)
    {
        status = pthread_create(&pThrObj->thread,
                                &thread_attr,
                                OSA_thrRunBody,
                                pThrObj);
    }
    else
    {
        status = pthread_create(&pThrObj->thread,
                                OSA_NULL,
                                OSA_thrRunBody,
                                pThrObj);
    }
    if(OSA_isFail(status))
    {
        OSA_thrPutObj(pThrObj);

        pThrObj = NULL;

        OSA_ERROR("Could not create thread [%d] !\n", status);
    }

    *phThr = (OSA_ThrHandle)(pThrObj);

error_exit:

    pthread_attr_destroy(&thread_attr);

    return status;
}

Int32 OSA_thrCreate(OSA_ThrCreate *pCreate, OSA_ThrHandle *phThr)
{
    pCreate->pName = "--";
    OSA_clearSize(pCreate->reserved, sizeof(pCreate->reserved));
    return OSA_thrCreateEx(pCreate, phThr);
}


Int32 OSA_thrJoin(OSA_ThrHandle hThr)
{
    Int32 status=OSA_SOK;
    void *returnVal;
    OSA_ThrObj *pThrObj = NULL;

    pThrObj = (OSA_ThrObj *)hThr;

    status = pthread_join(pThrObj->thread, &returnVal);

    return status;
}

#ifndef __ANDROID__  /* android不支持pthread_cancel函数，android下OSA_thrDelete总是返回-1 */
Int32 OSA_thrDelete(OSA_ThrHandle hThr)
{
    Int32 status=OSA_SOK;
    OSA_ThrObj *pThrObj = NULL;

    pThrObj = (OSA_ThrObj *)hThr;

    status |= pthread_cancel(pThrObj->thread);
    status |= OSA_thrJoin(hThr);

    OSA_thrPutObj(pThrObj);

    return status;
}
#else
Int32 OSA_thrDelete(OSA_ThrHandle hThr)
{
    return OSA_EFAIL;
}
#endif




Int32 OSA_thrChangePri(OSA_ThrHandle hThr, Uint32 thrPri)

{
    Int32              status = OSA_SOK;
    struct sched_param schedprm;
    OSA_ThrObj        *pThrObj = NULL;
    Int32              thrPriMax;
    Int32              thrPriMin;
    Int32              thrPriVal;

    pThrObj = (OSA_ThrObj *)hThr;

    thrPriVal = (Int32)thrPri;
    thrPriMax = OSA_THR_PRI_MAX(pThrObj->thrPol);
    thrPriMin = OSA_THR_PRI_MIN(pThrObj->thrPol);

    if(thrPriVal > thrPriMax)
    {
        thrPriVal = thrPriMax;
    }
    else if(thrPriVal < thrPriMin)
    {
        thrPriVal = thrPriMin;
    }

    schedprm.sched_priority = thrPriVal;
    status |= pthread_setschedparam(pThrObj->thread,
                                    SCHED_FIFO, &schedprm);

    return status;
}


Int32 OSA_thrExit(Ptr pRetVal)
{
    pthread_exit(pRetVal);
    return OSA_SOK;
}


/*判断线程是否需要退出, 在线程执行函数中调用, 用户态线程该接口始终返回0*/
Int32 OSA_thrShouldStop(void)
{
    return OSA_FALSE;
}

Int32 OSA_thrGetTid(void)
{

#ifdef __ANDROID__
    return gettid();
#else
    return (Int32)syscall(SYS_gettid);
#endif

}

Int32 OSA_thrGetPid(void)
{
    return (Int32)getpid();
}

Char *OSA_thrGetName(void)
{
    Int32             status;
    OSA_ThreadInfo    info;
    OSA_ThrObj        *pThrObj;

    OSA_clear(&info);
    info.tid = OSA_thrGetTid();

    status = OSA_getTidInfo(&info);
    if ((OSA_SOK == status) && (info.privData != OSA_NULL))
    {
        pThrObj = (OSA_ThrObj*)(info.privData);
        return pThrObj->name;
    }

    return OSA_NULL;
}


