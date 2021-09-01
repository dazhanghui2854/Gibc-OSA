/*******************************************************************************
 * osa_utTsk.c
 *
 * Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
 *
 * Author : Zheng Wei <zheng_wei@dahuatech.com>
 * Version: V1.0.0  2013��9��16�� Create
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
static Bool32 gTestRun = OSA_FALSE;
static Bool32 gTestStart = OSA_FALSE;


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/* �������� */
static Int32 TSK_testRun(Uint32 cmd,
                         Ptr    pCmdPrms,
                         Ptr    pUsrArgs)
{
    OSA_INFO("TSK_testRun\n");
    gTestRun = OSA_TRUE;
    return OSA_SOK;
}

/* �������� */
static Int32 TSK_testStart(Ptr pUsrArgs)
{
    OSA_INFO("TSK_testStart\n");
    gTestStart = OSA_TRUE;
    
    return OSA_SOK;
}

/* �������� */
static void TSK_test(void)
{
    OSA_TskCreate tskCreate;
    OSA_TskHandle hTask;
    Int32         status;
    OSA_initParms params;

#ifndef __KERNEL__
    /* ��ʼ��OSA */
    params.isStartMonTask = OSA_TRUE;
    status = OSA_init(&params);
    CU_ASSERT(OSA_SOK == status);
#endif

    /* �������� */
    OSA_clear(&tskCreate);
    tskCreate.createFlags    = OSA_TSK_FLAG_START;
    tskCreate.nMsgNum        = 2;
    tskCreate.nTimeOut       = OSA_TIMEOUT_FOREVER;
    tskCreate.pName          = "utTsk";
    tskCreate.nTimeSleep     = 1000;
    tskCreate.OpTimeOutAck   = NULL;
    tskCreate.stackSize      = 0;
    tskCreate.tskPol         = OSA_SCHED_OTHER;
    tskCreate.tskPri         = OSA_THR_PRI_DEFAULT(OSA_SCHED_OTHER);
    tskCreate.tskOps.OpTskRun   = TSK_testRun;
    tskCreate.tskOps.OpTskStart = TSK_testStart;
    status = OSA_tskCreate(&tskCreate, &hTask);
    CU_ASSERT(OSA_SOK == status);
    CU_ASSERT_PTR_NOT_NULL(hTask);

    /* �������� */
    status = OSA_tskCmdStart(hTask);
    CU_ASSERT(OSA_SOK == status);

    CU_ASSERT(OSA_TRUE == gTestStart);

    /* ����5s */
    OSA_msleep(5000);

    CU_ASSERT(OSA_TRUE == gTestRun);

    /* ɾ������ */
    status =  OSA_tskDelete(hTask);
    CU_ASSERT(OSA_SOK == status);

#ifndef __KERNEL__
    /* ����ʼ��OSA */
    status = OSA_deinit();
    CU_ASSERT(OSA_SOK == status);
#endif    
}

void OSA_utTskSuit(void)
{
    CU_SuiteHandle pSuite;

    CU_addSuite("OSA_utTsk",NULL,NULL,&pSuite);
    
    CU_ADD_TEST(pSuite, TSK_test);
}



