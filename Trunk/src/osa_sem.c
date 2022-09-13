/*******************************************************************************
* osa_sem.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-1 Create
*
* Desc: 实现linux用户态OSA模块对外提供的信号量接口
*
*           接口调用流程如下:
*           ==========================
*                   |
*           OSA_semCreate
*                   |
*           OSA_semPend/OSA_semPost
*                   |
*           OSA_semDelete
*           ===========================
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

/*定义semaphore对象结构体*/
typedef struct
{
    Uint32              nMgicNum;    /*魔数,用于校验句柄有效性。*/

    pthread_mutex_t     lock;        /*互斥锁*/
    pthread_cond_t      cond;        /*条件变量*/
    Uint16              count;        /*信号量计数*/
    Uint16              wait;         /*等待量计数*/
    clockid_t           clk_id;
}OSA_SemObject;


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_semCreate
* 描  述  : 该函数负责创建一个信号量
*
* 输  入  : - val:      信号量的初始值
*
* 输  出  : - phSem:    信号量 句柄指针,当创建成功时输出信号量句柄
* 返回值  :  OSA_SOK:   创建成功
*            OSA_EFAIL: 创建失败
*******************************************************************************/
Int32 OSA_semCreate(Uint32 val, OSA_SemHandle *phSem)
{
    Int32               ret;
    OSA_SemObject *     pSemObj;
    pthread_condattr_t  cattr;

    if(NULL == phSem)
    {
        OSA_ERROR("Null phSem\n");
        return OSA_EFAIL;
    }

    /*分配内存*/
    pSemObj = (OSA_SemObject *)OSA_memAlloc(sizeof(OSA_SemObject));
    if(NULL == pSemObj)
    {
        OSA_ERROR("alloc sem failed\n");
        return OSA_EFAIL;
    }

    /*赋值魔数*/
    pSemObj->nMgicNum = OSA_MAGIC_NUM;

    /*初始化信号量计数*/
    pSemObj->count = val;
    pSemObj->wait  = 0;

    /*初始化互斥锁*/
    pthread_mutex_init(&pSemObj->lock, NULL);

    /* 设置条件属性 */
    pthread_condattr_init(&cattr);
    ret = pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
    if (ret != 0)
    {
        /* 一般情况下，都不会进入该分支，除非是老掉牙的1993年前的版本 */
        pSemObj->clk_id = CLOCK_REALTIME;
        pthread_cond_init(&pSemObj->cond, NULL);
    }
    else
    {
        pSemObj->clk_id = CLOCK_MONOTONIC;
        pthread_cond_init(&pSemObj->cond, &cattr);
    }


    *phSem = (OSA_SemHandle)pSemObj;

    return OSA_SOK;
}

/*******************************************************************************
* 函数名  : OSA_semPend
* 描  述  : 信号量获取操作,
*           该接口不能在中断上下文调用
*
* 输  入  : - hSem:     信号量句柄,
*           - timeOut:  信号量等待超时时间, 单位毫秒，若等待超时则返回OSA_ETIMEOUT
*                       OSA_TIMEOUT_NONE表示不等待信号量立即返回，
*                               此时若能获取到信号量则返回OSA_SOK
*                               若不能获取信号量则返回OSA_EFAIL
*                       OSA_TIMEOUT_FOREVER表示永远等待信号量
*
* 输  出  : 无
* 返回值  :  OSA_SOK:   获取信号量成功
*            OSA_EFAIL: 获取信号量失败
*            OSA_ETIMEOUT: 获取信号量超时
*******************************************************************************/
Int32 OSA_semPend(OSA_SemHandle hSem, Uint32 timeOut)
{
    OSA_SemObject  *pSemObj = (OSA_SemObject *)hSem;
    Int32           status = OSA_EFAIL;
    struct timespec waitTime;
    struct timespec timeVal;

    /*检查handle有效性*/
    OSA_handleCheck(pSemObj);

    pthread_mutex_lock(&pSemObj->lock);

    do
    {
        if(pSemObj->count > 0)
        {
            pSemObj->count--;

            status = OSA_SOK;
            break;
        }
        else
        {
            if(timeOut == OSA_TIMEOUT_NONE)
            {
                break;
            }
            pSemObj->wait++;

            if(timeOut == (Uint32)OSA_TIMEOUT_FOREVER)
            {
                status = pthread_cond_wait(&pSemObj->cond, &pSemObj->lock);
                if(0 != status)
                {
                    OSA_ERROR("pthread_cond_wait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
            else
            {
                waitTime.tv_sec  = (Int32L)timeOut/1000;
                waitTime.tv_nsec = ((Int32L)timeOut%1000)*1000000;
                /* 超时时间采用单调递增时间CLOCK_MONOTONIC */
                status = clock_gettime(pSemObj->clk_id, &timeVal);
                if(0 != status)
                {
                    OSA_ERROR("clock_gettime failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
                waitTime.tv_sec  += timeVal.tv_sec;
                waitTime.tv_nsec += timeVal.tv_nsec;
                if(waitTime.tv_nsec >= 1000000000)
                {
                    waitTime.tv_sec  += 1;
                    waitTime.tv_nsec %= 1000000000;
                }

                status = pthread_cond_timedwait(&pSemObj->cond, &pSemObj->lock, &waitTime);
                if(ETIMEDOUT == status)
                {
                    status = OSA_ETIMEOUT;
                    break;
                }
                else if(0 != status)
                {
                    OSA_ERROR("pthread_cond_timedwait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
        }
    }
    while(1);

    pthread_mutex_unlock(&pSemObj->lock);

    return status;
}

/*******************************************************************************
* 函数名  : OSA_semPost
* 描  述  : 信号量释放操作
*
* 输  入  : - hSem:   信号量句柄,
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_semPost(OSA_SemHandle hSem)
{
    OSA_SemObject *pSemObj = (OSA_SemObject *)hSem;

    /*检查handle有效性*/
    OSA_handleCheck(pSemObj);

    pthread_mutex_lock(&pSemObj->lock);

    if (pSemObj->wait > 0)
    {
        pthread_cond_signal(&pSemObj->cond);
        pSemObj->wait--;
    }

    pSemObj->count++;

    pthread_mutex_unlock(&pSemObj->lock);

    return OSA_SOK;
}


Int32 OSA_semPostWhenEmpty(OSA_SemHandle hSem)
{
    OSA_SemObject *pSemObj = (OSA_SemObject *)hSem;

    /*检查handle有效性*/
    OSA_handleCheck(pSemObj);

    pthread_mutex_lock(&pSemObj->lock);

    if (0 == pSemObj->count)
    {
        if (pSemObj->wait > 0)
        {
            pthread_cond_signal(&pSemObj->cond);
            pSemObj->wait--;
        }
        pSemObj->count++;
    }

    pthread_mutex_unlock(&pSemObj->lock);

    return OSA_SOK;
}


Int32 OSA_semGetCnt(OSA_SemHandle hSem,Uint32 *pCnt)
{
    OSA_SemObject *pSemObj = (OSA_SemObject *)hSem;

    /*检查handle有效性*/
    OSA_handleCheck(pSemObj);

    pthread_mutex_lock(&pSemObj->lock);

    *pCnt = pSemObj->count;

    pthread_mutex_unlock(&pSemObj->lock);

    return OSA_SOK;

}

/*******************************************************************************
* 函数名  : OSA_semDelete
* 描  述  : 该函数负责销毁一个信号量
*
* 输  入  : - hSem: 信号量句柄,
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_semDelete(OSA_SemHandle hSem)
{
    OSA_SemObject *pSemObj = (OSA_SemObject *)hSem;

    /*检查handle有效性*/
    OSA_handleCheck(pSemObj);

    /*魔数设为-1，避免其他人在释放后继续用*/
    pSemObj->nMgicNum = (Uint32)-1;

    /*销毁条件变量*/
    pthread_cond_destroy(&pSemObj->cond);

    /*销毁互斥锁*/
    pthread_mutex_destroy(&pSemObj->lock);

    /*释放信号量对象内存*/
    if(OSA_SOK != OSA_memFree(pSemObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}
