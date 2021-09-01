/*******************************************************************************
* osa_sem.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-1 Create
*
* Desc: ʵ��linux�û�̬OSAģ������ṩ���ź����ӿ�
*
*           �ӿڵ�����������:
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
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include <osa.h>
#include <osa_priv.h>


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/*����semaphore����ṹ��*/
typedef struct
{
    Uint32              nMgicNum;    /*ħ��,����У������Ч�ԡ�*/

    pthread_mutex_t     lock;        /*������*/
    pthread_cond_t      cond;        /*��������*/
    Uint16              count;        /*�ź�������*/
    Uint16              wait;         /*�ȴ�������*/
    clockid_t           clk_id;
}OSA_SemObject;


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          ȫ�ֱ���������                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
* ������  : OSA_semCreate
* ��  ��  : �ú������𴴽�һ���ź���
*
* ��  ��  : - val:      �ź����ĳ�ʼֵ
*
* ��  ��  : - phSem:    �ź��� ���ָ��,�������ɹ�ʱ����ź������
* ����ֵ  :  OSA_SOK:   �����ɹ�
*            OSA_EFAIL: ����ʧ��
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

    /*�����ڴ�*/
    pSemObj = (OSA_SemObject *)OSA_memAlloc(sizeof(OSA_SemObject));
    if(NULL == pSemObj)
    {
        OSA_ERROR("alloc sem failed\n");
        return OSA_EFAIL;
    }

    /*��ֵħ��*/
    pSemObj->nMgicNum = OSA_MAGIC_NUM;

    /*��ʼ���ź�������*/
    pSemObj->count = val;
    pSemObj->wait  = 0;

    /*��ʼ��������*/
    pthread_mutex_init(&pSemObj->lock, NULL);

    /* ������������ */
    pthread_condattr_init(&cattr);
    ret = pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
    if (ret != 0)
    {
        /* һ������£����������÷�֧���������ϵ�����1993��ǰ�İ汾 */
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
* ������  : OSA_semPend
* ��  ��  : �ź�����ȡ����,
*           �ýӿڲ������ж������ĵ���
*
* ��  ��  : - hSem:     �ź������,
*           - timeOut:  �ź����ȴ���ʱʱ��, ��λ���룬���ȴ���ʱ�򷵻�OSA_ETIMEOUT
*                       OSA_TIMEOUT_NONE��ʾ���ȴ��ź����������أ�
*                               ��ʱ���ܻ�ȡ���ź����򷵻�OSA_SOK
*                               �����ܻ�ȡ�ź����򷵻�OSA_EFAIL
*                       OSA_TIMEOUT_FOREVER��ʾ��Զ�ȴ��ź���
*
* ��  ��  : ��
* ����ֵ  :  OSA_SOK:   ��ȡ�ź����ɹ�
*            OSA_EFAIL: ��ȡ�ź���ʧ��
*            OSA_ETIMEOUT: ��ȡ�ź�����ʱ
*******************************************************************************/
Int32 OSA_semPend(OSA_SemHandle hSem, Uint32 timeOut)
{
    OSA_SemObject  *pSemObj = (OSA_SemObject *)hSem;
    Int32           status = OSA_EFAIL;
    struct timespec waitTime;
    struct timespec timeVal;

    /*���handle��Ч��*/
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
                /* ��ʱʱ����õ�������ʱ��CLOCK_MONOTONIC */
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
* ������  : OSA_semPost
* ��  ��  : �ź����ͷŲ���
*
* ��  ��  : - hSem:   �ź������,
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_semPost(OSA_SemHandle hSem)
{
    OSA_SemObject *pSemObj = (OSA_SemObject *)hSem;

    /*���handle��Ч��*/
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

    /*���handle��Ч��*/
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

    /*���handle��Ч��*/
    OSA_handleCheck(pSemObj);

    pthread_mutex_lock(&pSemObj->lock);

    *pCnt = pSemObj->count;

    pthread_mutex_unlock(&pSemObj->lock);

    return OSA_SOK;

}

/*******************************************************************************
* ������  : OSA_semDelete
* ��  ��  : �ú�����������һ���ź���
*
* ��  ��  : - hSem: �ź������,
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_semDelete(OSA_SemHandle hSem)
{
    OSA_SemObject *pSemObj = (OSA_SemObject *)hSem;

    /*���handle��Ч��*/
    OSA_handleCheck(pSemObj);

    /*ħ����Ϊ-1���������������ͷź������*/
    pSemObj->nMgicNum = (Uint32)-1;

    /*������������*/
    pthread_cond_destroy(&pSemObj->cond);

    /*���ٻ�����*/
    pthread_mutex_destroy(&pSemObj->lock);

    /*�ͷ��ź��������ڴ�*/
    if(OSA_SOK != OSA_memFree(pSemObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}
