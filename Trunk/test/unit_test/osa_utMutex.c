/*******************************************************************************
 * osa_utMutex.c
 *
 * Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
 *
 * Author : Zheng Wei <zheng_wei@dahuatech.com>
 * Version: V1.0.0  2013��9��12�� Create
 *
 * Description: 
 *
 *       1. Ӳ��˵��
 *          �ޡ�
 *
 *       2. ����ṹ˵����
 *          �ޡ�
 *
 *       3. ʹ��˵����
 *          �ޡ�
 *
 *       4. ������˵����
 *          �ޡ�
 *
 *       5. ����˵����
 *          �ޡ�
 *
 * Modification: 
 *    Date     : 
 *    Revision :
 *    Author   :
 *    Contents :
 *******************************************************************************/

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#include <cunit.h>
#include <osa.h>

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          ȫ�ֱ���������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/* ���Դ���ɾ�������� */
static void MUTEX_testCreateDel(void)
{
    Int32           status;
    OSA_MutexHandle hMutex;

    status = OSA_mutexCreate(OSA_MUTEX_NORMAL,&hMutex);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_mutexDelete(hMutex);
    CU_ASSERT(OSA_SOK == status);
}

/* ���Լӽ��� */
static void MUTEX_testLockUnlock(void)
{
    Int32           status;
    OSA_MutexHandle hMutex;
    
    status = OSA_mutexCreate(OSA_MUTEX_NORMAL,&hMutex);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_mutexLock(hMutex);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_mutexUnlock(hMutex);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_mutexDelete(hMutex);
    CU_ASSERT(OSA_SOK == status);
}

/* ���Եݹ��� */
static void MUTEX_testRecursive(void)
{
    Int32           status;
    OSA_MutexHandle hMutex;
    Uint32          i;
    
    status = OSA_mutexCreate(OSA_MUTEX_RECURSIVE,&hMutex);
    CU_ASSERT(OSA_SOK == status);

    for ( i = 0 ; i < 3; i++ )
    {
        status = OSA_mutexLock(hMutex);
        CU_ASSERT(OSA_SOK == status);    
    }

    for ( i = 0 ; i < 3; i++ )
    {
        status = OSA_mutexUnlock(hMutex);
        CU_ASSERT(OSA_SOK == status);        
    }

    status = OSA_mutexDelete(hMutex);
    CU_ASSERT(OSA_SOK == status);
}

void OSA_utMutexSuit(void)
{
    CU_SuiteHandle pSuite;

    CU_addSuite("OSA_utMutex",NULL,NULL,&pSuite);
    
    CU_ADD_TEST(pSuite, MUTEX_testCreateDel);
    CU_ADD_TEST(pSuite, MUTEX_testLockUnlock);
    CU_ADD_TEST(pSuite, MUTEX_testRecursive);
}



