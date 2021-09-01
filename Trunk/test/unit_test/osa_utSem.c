/*******************************************************************************
 * osa_utSem.c
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

/* ���Դ���ɾ���ź��� */
static void SEM_testCreateDel(void)
{
    Int32   status;
    Uint32  val = 0;
    OSA_SemHandle hSem = NULL;

    status = OSA_semCreate(val, &hSem);
    CU_ASSERT(OSA_SOK == status);
    CU_ASSERT(NULL != hSem);

    status = OSA_semDelete(hSem);
    CU_ASSERT(OSA_SOK == status);
}

/* �����ź���Pend */
static void SEM_testPend(void)
{
    Int32  status;
    OSA_SemHandle hSem = NULL;

    status = OSA_semCreate(0, &hSem);
    CU_ASSERT(OSA_SOK == status);
    CU_ASSERT(NULL != hSem);

    status = OSA_semPend(hSem, 1000);
    CU_ASSERT(OSA_ETIMEOUT == status);

    status = OSA_semDelete(hSem);
    CU_ASSERT(OSA_SOK == status);
}

/* �����ź���post */
static void SEM_testPost(void)
{
    Int32  status;
    OSA_SemHandle hSem = NULL;

    status = OSA_semCreate(0, &hSem);
    CU_ASSERT(OSA_SOK == status);
    CU_ASSERT(NULL != hSem);

    status = OSA_semPost(hSem);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_semPend(hSem, 1000);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_semDelete(hSem);
    CU_ASSERT(OSA_SOK == status);
}

void OSA_utSemSuit(void)
{
    CU_SuiteHandle pSuite;

    CU_addSuite("OSA_utSem",NULL,NULL,&pSuite);
    
    CU_ADD_TEST(pSuite, SEM_testCreateDel);
    CU_ADD_TEST(pSuite, SEM_testPend);
    CU_ADD_TEST(pSuite, SEM_testPost);
}



