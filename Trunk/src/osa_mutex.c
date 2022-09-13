/*******************************************************************************
* osa_mutex.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-1 Create
*
* Desc: 在linux用户态下实现OSA模块对外提供的互斥锁接口
*
*           接口调用流程如下:
*           ==========================
*                   |                            
*           OSA_mutexCreate
*                   |
*           OSA_mutexLock/OSA_mutexUnlock
*                   |
*           OSA_mutexDelete
*           ===========================
*           注意:对于类型为OSA_MUTEX_NORMAL的互斥锁，同一线程不能加锁多次
*                对于类型为OSA_MUTEX_RECURSIVE的互斥锁，同一线程可以加锁多次
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

/*定义互斥锁对象*/
typedef struct
{
    Uint32              nMgicNum;    /*魔数,用于校验句柄有效性。*/

    pthread_mutex_t     lock;        /*用户态互斥锁变量*/
}OSA_MutexObject;

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
* 函数名  : OSA_mutexCreate
* 描  述  : 该函数负责创建一个互斥锁
*
* 输  入  : - type:    互斥锁类型,参见OSA_MutexType定义
*
* 输  出  : - phMutex: 互斥锁句柄指针,当创建成功时输出互斥锁句柄
* 返回值  : OSA_SOK:   创建成功
*           OSA_EFAIL: 创建失败
*******************************************************************************/
Int32 OSA_mutexCreate(Uint32 type, OSA_MutexHandle *phMutex)
{
    OSA_MutexObject *pMutexObj = NULL;
    pthread_mutexattr_t mutex_attr;
    
    if(NULL == phMutex)
    {
        OSA_ERROR("Null phMutex\n");
        return OSA_EFAIL;
    }

    /*分配内存*/
    pMutexObj = (OSA_MutexObject *)OSA_memAlloc(sizeof(OSA_MutexObject));
    if(NULL == pMutexObj)
    {
        OSA_ERROR("alloc mutex failed\n");
        return OSA_EFAIL;
    }

    /*赋值魔数*/
    pMutexObj->nMgicNum = OSA_MAGIC_NUM;

    /*初始化锁*/
    if(OSA_MUTEX_RECURSIVE == type)
    {
        pthread_mutexattr_init(&mutex_attr);

        /*lint -save -e718 -e746 -e40 */
        /* 设置锁支持嵌套，需要在makfile中添加_D_GNU_SOURCE。*/
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
        /*lint -restore */

        pthread_mutex_init(&pMutexObj->lock, &mutex_attr);
        pthread_mutexattr_destroy(&mutex_attr);
    }
    else
    {
        pthread_mutex_init(&pMutexObj->lock, NULL);
    }

    *phMutex = (OSA_MutexHandle)pMutexObj;
    
    return OSA_SOK;
}

/*******************************************************************************
* 函数名  : OSA_mutexLock
* 描  述  : 互斥锁加锁
* 输  入  : - hMutex: 互斥锁句柄,
*
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_mutexLock(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;
    Int32 ret;

    /*检查handle有效性*/    
    OSA_handleCheck(pMutexObj);

    ret = pthread_mutex_lock(&pMutexObj->lock);

    return (0 == ret) ? OSA_SOK : OSA_EFAIL;
}


/*******************************************************************************
* 函数名  : OSA_mutexTryLock
* 描  述  : 试图对互斥锁进行加锁。该接口目前只支持Linux用户层调用。
*           该接口不能在内核态中断上下文调用
* 输  入  : - hMutex: 互斥锁句柄,
*
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*           OSA_EBUSY: 锁被占着
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_mutexTryLock(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;
    Int32 ret;

    /*检查handle有效性*/    
    OSA_handleCheck(pMutexObj);

    ret = pthread_mutex_trylock(&pMutexObj->lock);

    if (0 == ret)
    {
        return OSA_SOK;
    }
    else if (EBUSY == ret)
    {
        return OSA_EBUSY;
    }
    else
    {
        return OSA_EFAIL;
    }
}


/*******************************************************************************
* 函数名  : OSA_mutexUnlock
* 描  述  : 互斥锁解锁
*           该接口不能在内核态中断上下文调用
* 输  入  : - hMutex: 互斥锁句柄,
*
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_mutexUnlock(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;
    Int32 ret;

    /*检查handle有效性*/    
    OSA_handleCheck(pMutexObj);

    ret = pthread_mutex_unlock(&pMutexObj->lock);

    return (0 == ret) ? OSA_SOK : OSA_EFAIL;
}

/*******************************************************************************
* 函数名  : OSA_mutexDelete
* 描  述  : 该函数负责销毁一个互斥锁
*           该接口不能在内核态中断上下文调用
*
* 输  入  : - hMutex: 互斥锁句柄
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_mutexDelete(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;

    /*检查handle有效性*/    
    OSA_handleCheck(pMutexObj);

    pthread_mutex_destroy(&pMutexObj->lock);  

    
    /*魔数设为-1，避免其他人在释放后继续用*/
    pMutexObj->nMgicNum = (Uint32)-1;

    if(OSA_SOK != OSA_memFree(pMutexObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}


