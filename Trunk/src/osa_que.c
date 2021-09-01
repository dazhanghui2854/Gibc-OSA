/*******************************************************************************
* osa_que.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA队列接口。
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


typedef struct
{
    Uint32 len;
    Uint32 count;

    Uint32 curRd;
    Uint32 curWr;

    Bool32 isAlign;

    Sizet *pQueue;

    pthread_mutex_t     lock;
    pthread_cond_t      condRd;
    pthread_cond_t      condWr;
} OSA_QueObj;


typedef struct
{
    Uint32 len;
    Uint32 count;

    OSA_ListHead queHead;

    pthread_mutex_t     lock;
    pthread_cond_t      condRd;
    pthread_cond_t      condWr;
} OSA_QueListObj;

void OSA_queCleanup(void *arg)
{
    pthread_mutex_unlock((pthread_mutex_t *)arg);
}

Int32 OSA_queCreate(OSA_QueCreate *pCreate,
                    OSA_QueHandle *phQue)
{
    OSA_QueObj *pObj = NULL;

    OSA_assertNotNull(pCreate);
    OSA_assertNotNull(phQue);

    if (OSA_isNull(pCreate)
        || OSA_isNull(phQue))
    {
        OSA_ERROR("Input parms NULL !\n");
        return OSA_EFAIL;
    }

    if (pCreate->maxElems == 0)
    {
        OSA_ERROR("maxElems was zero !\n");
        return OSA_EFAIL;
    }

    if (OSA_isNull(pCreate->pQueue))
    {
        pObj = OSA_memAlloc((sizeof(*pObj->pQueue)
                                * pCreate->maxElems)
                             + sizeof(*pObj));
        if(OSA_isNull(pObj))
        {
            OSA_ERROR("Alloc obj/queue failed\n");
            return OSA_EFAIL;
        }

        OSA_clearSize(pObj,
                      (sizeof(*pObj->pQueue)
                         * pCreate->maxElems)
                       + sizeof(*pObj));

        pObj->pQueue = (Sizet *)(Ptr)((Uint8 *)pObj + sizeof(*pObj));
    }
    else
    {
        pObj = OSA_memAlloc(sizeof(*pObj));
        if(OSA_isNull(pObj))
        {
            OSA_ERROR("Alloc obj failed\n");
            return OSA_EFAIL;
        }

        OSA_clear(pObj);

        pObj->pQueue = pCreate->pQueue;
    }

    pObj->len = pCreate->maxElems;

	/*
	  covertiy号： 323704
	  问题描述： 资源泄露
	  修改描述： close
	  注意事项描述： 
	*/

    /* 检测maxElems的值是否是2的N次方，如果是，则后续put/get中不使用%符号求余数。*/
    if (OSA_checkIs2n(pCreate->maxElems))
    {
        OSA_DEBUG("maxElems %u is 2^n !\n",pCreate->maxElems);
        pObj->isAlign = OSA_TRUE;
    }

    pthread_mutex_init(&pObj->lock, NULL);
    pthread_cond_init(&pObj->condRd, NULL);
    pthread_cond_init(&pObj->condWr, NULL);

    *phQue = (OSA_QueHandle)pObj;

    return OSA_SOK;
}


Int32 OSA_queDelete(OSA_QueHandle hQue)
{
    OSA_QueObj *pObj = (OSA_QueObj *)hQue;

    OSA_assertNotNull(hQue);

    if(OSA_isNull(hQue))
    {
        return OSA_EFAIL;
    }

    pthread_cond_destroy(&pObj->condRd);
    pthread_cond_destroy(&pObj->condWr);
    pthread_mutex_destroy(&pObj->lock);

    OSA_clear(pObj);

    if(OSA_SOK != OSA_memFree(pObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}


Int32 OSA_quePut(OSA_QueHandle hQue, Sizet value, Uint32 timeout)
{
    Int32           status = OSA_SOK;
    OSA_QueObj     *pObj = (OSA_QueObj *)hQue;
    struct timespec waitTime;
    OSA_TimeVal64     timeVal;

    OSA_assertNotNull(hQue);

    pthread_mutex_lock(&pObj->lock);

    do
    {
        if( pObj->count < pObj->len )
        {
            pObj->pQueue[pObj->curWr] = value;

            if (pObj->isAlign)
            {
                pObj->curWr = (pObj->curWr + 1) & (pObj->len - 1);
            }
            else
            {
                pObj->curWr = (pObj->curWr + 1) % pObj->len;
            }

            if (0 == pObj->count)
            {
                pthread_cond_signal(&pObj->condRd);
            }

            pObj->count++;

            break;
        }
        else
        {
            if(timeout==OSA_TIMEOUT_NONE)
            {
                status = OSA_EFAIL;
                break;
            }

            if(timeout == (Uint32)OSA_TIMEOUT_FOREVER)
            {
                status = pthread_cond_wait(&pObj->condWr, &pObj->lock);
                if(OSA_SOK != status)
                {
                    OSA_ERROR("pthread_cond_wait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
            else
            {
                waitTime.tv_sec  = (Int32L)timeout/1000;
                waitTime.tv_nsec = ((Int32L)timeout%1000)*1000000;

                OSA_getTimeOfDay64(&timeVal);
                waitTime.tv_sec  += timeVal.tvSec;
                waitTime.tv_nsec += timeVal.tvUsec * 1000;
                if(waitTime.tv_nsec >= 1000000000)
                {
                    waitTime.tv_sec  += 1;
                    waitTime.tv_nsec %= 1000000000;
                }

                status = pthread_cond_timedwait(&pObj->condWr, &pObj->lock, &waitTime);
                if(ETIMEDOUT == status)
                {
                    status = OSA_ETIMEOUT;
                    break;
                }
                else if(OSA_SOK != status)
                {
                    OSA_ERROR("pthread_cond_timedwait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
        }
    } while(1);

    pthread_mutex_unlock(&pObj->lock);

    return status;
}


Int32 OSA_queGet(OSA_QueHandle hQue, Sizet *pValue, Uint32 timeout)
{
    Int32           status = OSA_SOK;
    OSA_QueObj     *pObj = (OSA_QueObj *)hQue;
    struct timespec waitTime;
    OSA_TimeVal64     timeVal;

    OSA_assertNotNull(hQue);
    OSA_assertNotNull(pValue);

    pthread_mutex_lock(&pObj->lock);

    do
    {
        if(pObj->count > 0)
        {

            if(OSA_isNotNull(pValue))
            {
                *pValue = pObj->pQueue[pObj->curRd];
            }
            else
            {
                status = OSA_EFAIL;
                break;
            }

            if (pObj->isAlign)
            {
                pObj->curRd = (pObj->curRd + 1) & (pObj->len - 1);
            }
            else
            {
                pObj->curRd = (pObj->curRd + 1) % pObj->len;
            }

            if (pObj->count == pObj->len)
            {
                pthread_cond_signal(&pObj->condWr);
            }

            pObj->count--;

            break;
        }
        else
        {
            if(timeout==OSA_TIMEOUT_NONE)
            {
                status = OSA_EFAIL;
                break;
            }

            if(timeout == (Uint32)OSA_TIMEOUT_FOREVER)
            {
                status = pthread_cond_wait(&pObj->condRd, &pObj->lock);
                if(OSA_SOK != status)
                {
                    OSA_ERROR("pthread_cond_wait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
            else
            {
                waitTime.tv_sec   = (Int32L)timeout/1000;
                waitTime.tv_nsec  = ((Int32L)timeout%1000)*1000000;

                OSA_getTimeOfDay64(&timeVal);
                waitTime.tv_sec  += timeVal.tvSec;
                waitTime.tv_nsec += timeVal.tvUsec * 1000;
                if(waitTime.tv_nsec >= 1000000000)
                {
                    waitTime.tv_sec  += 1;
                    waitTime.tv_nsec %= 1000000000;
                }
                /* pthread_cond_timedwait需要使用到的是一个未来特定的时间，所以超时是基于当前时间 */
                status = pthread_cond_timedwait(&pObj->condRd, &pObj->lock, &waitTime);
                if(ETIMEDOUT == status)
                {
                    status = OSA_ETIMEOUT;
                    break;
                }
                else if(OSA_SOK != status)
                {
                    OSA_ERROR("pthread_cond_timedwait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
        }
    } while(1);

    pthread_mutex_unlock(&pObj->lock);

    return status;
}


Int32 OSA_queReset(OSA_QueHandle hQue)
{
    OSA_QueObj *pObj = (OSA_QueObj *)hQue;

    OSA_assertNotNull(hQue);

    pthread_mutex_lock(&pObj->lock);

    pObj->curRd = 0;
    pObj->curWr = 0;
    pObj->count = 0;

    OSA_clearSize(pObj->pQueue,
                 sizeof(*pObj->pQueue) * pObj->len);

    pthread_mutex_unlock(&pObj->lock);

    return OSA_SOK;
}


/* 轻量级的队列写，没有保护，只能用于单线程，无资源抢占的场合。*/
Int32 OSA_queslPut(OSA_QueHandle hQue, Sizet value)
{
    Int32 status = OSA_EFAIL;
    OSA_QueObj *pObj = (OSA_QueObj *)hQue;

    OSA_assertNotNull(hQue);

    if( pObj->count < pObj->len )
    {

        pObj->pQueue[pObj->curWr] = value;

        if (pObj->isAlign)
        {
            pObj->curWr = (pObj->curWr + 1) & (pObj->len - 1);
        }
        else
        {
            pObj->curWr = (pObj->curWr + 1) % pObj->len;
        }

        pObj->count++;
        status = OSA_SOK;
    }

    return status;
}


/* 轻量级的队列读，没有保护，只能用于单线程，无资源抢占的场合。*/
Int32 OSA_queslGet(OSA_QueHandle hQue, Sizet *pValue)
{
    Int32 status = OSA_EFAIL;
    OSA_QueObj *pObj = (OSA_QueObj *)hQue;

    OSA_assertNotNull(hQue);
    OSA_assertNotNull(pValue)

    if(pObj->count > 0)
    {
        *pValue = pObj->pQueue[pObj->curRd];

        if (pObj->isAlign)
        {
            pObj->curRd = (pObj->curRd + 1) & (pObj->len - 1);
        }
        else
        {
            pObj->curRd = (pObj->curRd + 1) % pObj->len;
        }

        pObj->count--;
        status = OSA_SOK;
    }

    return status;
}


Int32 OSA_queListCreate(OSA_QueCreate *pCreate,
                        OSA_QueHandle *phQue)
{
    OSA_QueListObj *pQueObj;

    OSA_assertNotNull(pCreate);
    OSA_assertNotNull(phQue);

    if (OSA_isNull(pCreate)
        || OSA_isNull(phQue))
    {
        OSA_ERROR("Input parms NULL !\n");
        return OSA_EFAIL;
    }

    if (pCreate->maxElems == 0)
    {
        OSA_ERROR("maxElems was zero !\n");
        return OSA_EFAIL;
    }

    pQueObj = OSA_memAlloc(sizeof(*pQueObj));
    if (OSA_isNull(pQueObj))
    {
        OSA_ERROR("Queue Obj Alloc Fail !\n");
        return OSA_EFAIL;
    }

    OSA_clear(pQueObj);

    pQueObj->len = pCreate->maxElems;

    OSA_listHeadInit(&pQueObj->queHead);

    pthread_mutex_init(&pQueObj->lock, NULL);
    pthread_cond_init(&pQueObj->condRd, NULL);
    pthread_cond_init(&pQueObj->condWr, NULL);

    *phQue = (OSA_QueHandle)pQueObj;

    return OSA_SOK;
}


Int32 OSA_queListDelete(OSA_QueHandle hQue)
{
    OSA_QueListObj *pQueObj = (OSA_QueListObj *)hQue;

    OSA_assertNotNull(hQue);

    if(OSA_isNull(hQue))
    {
        return OSA_EFAIL;
    }

    pthread_cond_destroy(&pQueObj->condWr);
    pthread_cond_destroy(&pQueObj->condRd);
    pthread_mutex_destroy(&pQueObj->lock);

    OSA_clear(pQueObj);

    if(OSA_SOK != OSA_memFree(pQueObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}

Int32 OSA_queListRemove(OSA_QueHandle hQue, OSA_ListHead *pQueElem)
{
    Int32               status     = OSA_SOK;
    OSA_QueListObj      *pQueObj   = (OSA_QueListObj *)hQue;

    OSA_assertNotNull(hQue);
    OSA_assertNotNull(pQueElem);

    pthread_mutex_lock(&pQueObj->lock);

    if ((pQueElem->next != pQueElem)         &&
        (pQueElem->prev != pQueElem)         &&
        (pQueElem->next != OSA_NULL)         &&
        (pQueElem->prev != OSA_NULL)         &&
        (pQueElem->next != OSA_LIST_POISON1) &&
        (pQueElem->prev != OSA_LIST_POISON2)  )
    {
        OSA_listDel(pQueElem);

        if (pQueObj->count == pQueObj->len)
        {
           pthread_cond_signal(&pQueObj->condWr);
        }

        pQueObj->count--;
    }
    else
    {
        status = OSA_EFAIL;
    }

    pthread_mutex_unlock(&pQueObj->lock);

    return status;
}

OSA_ListHead * OSA_queListNext(OSA_QueHandle hQue, OSA_ListHead *pQueElem)
{
    OSA_ListHead *      pListElem  = OSA_NULL;
    OSA_QueListObj      *pQueObj   = (OSA_QueListObj *)hQue;

    OSA_assertNotNull(hQue);

    pthread_mutex_lock(&pQueObj->lock);

    if (OSA_NULL == pQueElem)
    {
        if(!OSA_listEmpty(&pQueObj->queHead))
        {
            pListElem = pQueObj->queHead.next;
        }
    }
    else
    {
        if (pQueElem->next != &pQueObj->queHead)
        {
            pListElem = pQueElem->next;
        }
    }

    pthread_mutex_unlock(&pQueObj->lock);

    return pListElem;
}

OSA_ListHead * OSA_queListPrev(OSA_QueHandle hQue, OSA_ListHead *pQueElem)
{
    OSA_ListHead *      pListElem  = OSA_NULL;
    OSA_QueListObj      *pQueObj   = (OSA_QueListObj *)hQue;

    OSA_assertNotNull(hQue);

    pthread_mutex_lock(&pQueObj->lock);

    if (OSA_NULL == pQueElem)
    {
        if(!OSA_listEmpty(&pQueObj->queHead))
        {
            pListElem = pQueObj->queHead.prev;
        }
    }
    else
    {
        if (pQueElem->prev != &pQueObj->queHead)
        {
            pListElem = pQueElem->prev;
        }
    }

    pthread_mutex_unlock(&pQueObj->lock);

    return pListElem;
}


Int32 OSA_queListPut(OSA_QueHandle hQue,
                     OSA_ListHead *pQueElem,
                     Uint32 timeout)
{
    Int32           status = OSA_SOK;
    OSA_QueListObj *pQueObj = (OSA_QueListObj *)hQue;
    struct timespec waitTime;
    OSA_TimeVal64     timeVal;

    OSA_assertNotNull(hQue);
    OSA_assertNotNull(pQueElem);


    /* 注册一个线程清理函数，当线程被取消时，先执行 pthread_cond_wait 中注册的线程清理函数 __condvar_cleanup，将 mutex 锁>上，
 * 再执行线程中注册的线程处理函数pthread_mutex_unlock，将mutex解锁。这样就避免了死锁的发生*/
    pthread_cleanup_push(OSA_queCleanup, (void *) &pQueObj->lock);

    pthread_mutex_lock(&pQueObj->lock);

    do
    {
        if (pQueObj->count < pQueObj->len)
        {
            if (0 == pQueObj->count)
            {
                pthread_cond_signal(&pQueObj->condRd);
            }

            OSA_listAddTail(pQueElem, &pQueObj->queHead);

            pQueObj->count++;

            break;
        }
        else
        {
            if(timeout == OSA_TIMEOUT_NONE)
            {
                status = OSA_EFAIL;
                break;
            }

            if(timeout == (Uint32)OSA_TIMEOUT_FOREVER)
            {
                status = pthread_cond_wait(&pQueObj->condWr, &pQueObj->lock);
                if(0 != status)
                {
                    OSA_ERROR("pthread_cond_wait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
            else
            {
                waitTime.tv_sec  = (Int32L)timeout/1000;
                waitTime.tv_nsec = ((Int32L)timeout%1000)*1000000;

                OSA_getTimeOfDay64(&timeVal);
                waitTime.tv_sec  += timeVal.tvSec;
                waitTime.tv_nsec += timeVal.tvUsec * 1000;
                if(waitTime.tv_nsec >= 1000000000)
                {
                    waitTime.tv_sec  += 1;
                    waitTime.tv_nsec %= 1000000000;
                }

                status = pthread_cond_timedwait(&pQueObj->condWr,
                                        &pQueObj->lock, &waitTime);
                if(ETIMEDOUT == status)
                {
                    status = OSA_ETIMEOUT;
                    break;
                }
                else if(OSA_SOK != status)
                {
                    OSA_ERROR("pthread_cond_timedwait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
        }
    }
    while (1);

    pthread_mutex_unlock(&pQueObj->lock);

    pthread_cleanup_pop(0);

    return status;
}


Int32 OSA_queListGet(OSA_QueHandle  hQue,
                     OSA_ListHead **ppQueElem,
                     Uint32 timeout)
{
    Int32           status = OSA_SOK;
    OSA_QueListObj *pQueObj = (OSA_QueListObj *)hQue;
    struct timespec waitTime;
    OSA_TimeVal64     timeVal;

    OSA_assertNotNull(hQue);
    OSA_assertNotNull(ppQueElem);

    /* 注册一个线程清理函数，当线程被取消时，先执行 pthread_cond_wait 中注册的线程清理函数 __condvar_cleanup，将 mutex 锁上，再执行线程中注册的线程处理函数pthread_mutex_unlock，将mutex解锁。这样就避免了死锁的发生*/
    pthread_cleanup_push(OSA_queCleanup, (void *) &pQueObj->lock);

    pthread_mutex_lock(&pQueObj->lock);

    do
    {
        if(!OSA_listEmpty(&pQueObj->queHead))
        {
            *ppQueElem = pQueObj->queHead.next;

            OSA_listDel(*ppQueElem);

            if (pQueObj->count == pQueObj->len)
            {
               pthread_cond_signal(&pQueObj->condWr);
            }

            pQueObj->count--;

            break;
        }
        else
        {
            if(timeout == OSA_TIMEOUT_NONE)
            {
                status = OSA_EFAIL;
                break;
            }

            if(timeout == (Uint32)OSA_TIMEOUT_FOREVER)
            {
                status = pthread_cond_wait(&pQueObj->condRd, &pQueObj->lock);
                if(0 != status)
                {
                    OSA_ERROR("pthread_cond_wait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
            else
            {
                waitTime.tv_sec  = (Int32L)timeout/1000;
                waitTime.tv_nsec = ((Int32L)timeout%1000)*1000000;

                OSA_getTimeOfDay64(&timeVal);
                waitTime.tv_sec  += timeVal.tvSec;
                waitTime.tv_nsec += timeVal.tvUsec * 1000;
                if(waitTime.tv_nsec >= 1000000000)
                {
                    waitTime.tv_sec  += 1;
                    waitTime.tv_nsec %= 1000000000;
                }

                status = pthread_cond_timedwait(&pQueObj->condRd,
                                        &pQueObj->lock, &waitTime);
                if(ETIMEDOUT == status)
                {
                    status = OSA_ETIMEOUT;
                    break;
                }
                else if(OSA_SOK != status)
                {
                    OSA_ERROR("pthread_cond_timedwait failed, ret:%d\n", status);
                    status = OSA_EFAIL;
                    break;
                }
            }
        }
    } while(1);

    pthread_mutex_unlock(&pQueObj->lock);

    pthread_cleanup_pop(0);

    return status;
}


/* 公共模块，OSA_que和OSA_queList都可以使用。*/

Bool32 OSA_queIsEmpty(OSA_QueHandle hQue)
{
    OSA_QueObj *pQueObj = (OSA_QueObj *)hQue;

    OSA_assertNotNull(hQue);

    return pQueObj->count ? OSA_FALSE : OSA_TRUE;
}


Uint32 OSA_queGetLen(OSA_QueHandle hQue)
{

    OSA_QueObj *pQueObj = (OSA_QueObj *)hQue;

    OSA_assertNotNull(hQue);

    return pQueObj->len;
}


Uint32 OSA_queGetCount(OSA_QueHandle hQue)
{

    OSA_QueObj *pQueObj = (OSA_QueObj *)hQue;

    OSA_assertNotNull(hQue);

    return pQueObj->count;
}



