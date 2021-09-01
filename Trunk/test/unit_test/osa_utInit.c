/*******************************************************************************
* osa_utInit.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng Wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2013��9��12�� Create
*
* Description: osa��ʼ����ؽӿ�����
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

#ifndef __KERNEL__

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

/* ������� */
static void INIT_testInitNormal(void)
{
    Int32         status;
    OSA_initParms params;
    
    params.isStartMonTask = OSA_TRUE;
    status = OSA_init(&params);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_deinit();
    CU_ASSERT(OSA_SOK == status);

    params.isStartMonTask = OSA_FALSE;
    status = OSA_init(&params);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_deinit();
    CU_ASSERT(OSA_SOK == status);
}

/* ����ΪNULL */
static void INIT_testInitNull(void)
{
    Int32         status;

    status = OSA_init(NULL);
    CU_ASSERT(OSA_EFAIL == status);
}

/* �ظ���ʼ����� */
static void INIT_testInitRepeat(void)
{
    Int32         status;
    OSA_initParms params;
    Uint32        i = 0;

    for ( i = 0 ; i < 3; i++ )
    {
        params.isStartMonTask = OSA_TRUE;
        status = OSA_init(&params);
        CU_ASSERT(OSA_SOK == status);
    }

    for ( i = 0 ; i < 3; i++ )
    {
        status = OSA_deinit();
        CU_ASSERT(OSA_SOK == status);
    }
}

void OSA_utInitSuit(void)
{
    CU_SuiteHandle pSuite;

    CU_addSuite("osa_utInit",NULL,NULL,&pSuite);
     
    CU_ADD_TEST(pSuite, INIT_testInitNormal);
    CU_ADD_TEST(pSuite, INIT_testInitNull);
    CU_ADD_TEST(pSuite, INIT_testInitRepeat);

}

#endif

