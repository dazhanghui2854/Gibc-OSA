/*******************************************************************************
* osa_mutex.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-1 Create
*
* Desc: ��linux�û�̬��ʵ��OSAģ������ṩ�Ļ������ӿ�
*
*           �ӿڵ�����������:
*           ==========================
*                   |                            
*           OSA_mutexCreate
*                   |
*           OSA_mutexLock/OSA_mutexUnlock
*                   |
*           OSA_mutexDelete
*           ===========================
*           ע��:��������ΪOSA_MUTEX_NORMAL�Ļ�������ͬһ�̲߳��ܼ������
*                ��������ΪOSA_MUTEX_RECURSIVE�Ļ�������ͬһ�߳̿��Լ������
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

/*���廥��������*/
typedef struct
{
    Uint32              nMgicNum;    /*ħ��,����У������Ч�ԡ�*/

    pthread_mutex_t     lock;        /*�û�̬����������*/
}OSA_MutexObject;

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
* ������  : OSA_mutexCreate
* ��  ��  : �ú������𴴽�һ��������
*
* ��  ��  : - type:    ����������,�μ�OSA_MutexType����
*
* ��  ��  : - phMutex: ���������ָ��,�������ɹ�ʱ������������
* ����ֵ  : OSA_SOK:   �����ɹ�
*           OSA_EFAIL: ����ʧ��
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

    /*�����ڴ�*/
    pMutexObj = (OSA_MutexObject *)OSA_memAlloc(sizeof(OSA_MutexObject));
    if(NULL == pMutexObj)
    {
        OSA_ERROR("alloc mutex failed\n");
        return OSA_EFAIL;
    }

    /*��ֵħ��*/
    pMutexObj->nMgicNum = OSA_MAGIC_NUM;

    /*��ʼ����*/
    if(OSA_MUTEX_RECURSIVE == type)
    {
        pthread_mutexattr_init(&mutex_attr);

        /*lint -save -e718 -e746 -e40 */
        /* ������֧��Ƕ�ף���Ҫ��makfile�����_D_GNU_SOURCE��*/
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
* ������  : OSA_mutexLock
* ��  ��  : ����������
* ��  ��  : - hMutex: ���������,
*
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_mutexLock(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;
    Int32 ret;

    /*���handle��Ч��*/    
    OSA_handleCheck(pMutexObj);

    ret = pthread_mutex_lock(&pMutexObj->lock);

    return (0 == ret) ? OSA_SOK : OSA_EFAIL;
}


/*******************************************************************************
* ������  : OSA_mutexTryLock
* ��  ��  : ��ͼ�Ի��������м������ýӿ�Ŀǰֻ֧��Linux�û�����á�
*           �ýӿڲ������ں�̬�ж������ĵ���
* ��  ��  : - hMutex: ���������,
*
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EBUSY: ����ռ��
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_mutexTryLock(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;
    Int32 ret;

    /*���handle��Ч��*/    
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
* ������  : OSA_mutexUnlock
* ��  ��  : ����������
*           �ýӿڲ������ں�̬�ж������ĵ���
* ��  ��  : - hMutex: ���������,
*
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_mutexUnlock(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;
    Int32 ret;

    /*���handle��Ч��*/    
    OSA_handleCheck(pMutexObj);

    ret = pthread_mutex_unlock(&pMutexObj->lock);

    return (0 == ret) ? OSA_SOK : OSA_EFAIL;
}

/*******************************************************************************
* ������  : OSA_mutexDelete
* ��  ��  : �ú�����������һ��������
*           �ýӿڲ������ں�̬�ж������ĵ���
*
* ��  ��  : - hMutex: ���������
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_mutexDelete(OSA_MutexHandle hMutex)
{
    OSA_MutexObject *pMutexObj = (OSA_MutexObject *)hMutex;

    /*���handle��Ч��*/    
    OSA_handleCheck(pMutexObj);

    pthread_mutex_destroy(&pMutexObj->lock);  

    
    /*ħ����Ϊ-1���������������ͷź������*/
    pMutexObj->nMgicNum = (Uint32)-1;

    if(OSA_SOK != OSA_memFree(pMutexObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}


