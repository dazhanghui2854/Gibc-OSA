/*******************************************************************************
 * osa_utTsk.c
 *
 * Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
 *
 * Author : Zheng Wei <zheng_wei@dahuatech.com>
 * Version: V1.0.0  2013年9月16日 Create
 *
 * Description: 
 *
 *       1. 硬件说明
 *          无。
 *
 *       2. 程序结构说明。
 *          无。
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
 *    Date     : 
 *    Revision :
 *    Author   :
 *    Contents :
 *******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#include <cunit.h>
#include <osa.h>

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */
static Bool32 gTestRun = OSA_FALSE;
static Bool32 gTestStart = OSA_FALSE;


/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/* 任务运行 */
static Int32 TSK_testRun(Uint32 cmd,
                         Ptr    pCmdPrms,
                         Ptr    pUsrArgs)
{
    OSA_INFO("TSK_testRun\n");
    gTestRun = OSA_TRUE;
    return OSA_SOK;
}

/* 任务启动 */
static Int32 TSK_testStart(Ptr pUsrArgs)
{
    OSA_INFO("TSK_testStart\n");
    gTestStart = OSA_TRUE;
    
    return OSA_SOK;
}

/* 测试任务 */
static void TSK_test(void)
{
    OSA_TskCreate tskCreate;
    OSA_TskHandle hTask;
    Int32         status;
    OSA_initParms params;

#ifndef __KERNEL__
    /* 初始化OSA */
    params.isStartMonTask = OSA_TRUE;
    status = OSA_init(&params);
    CU_ASSERT(OSA_SOK == status);
#endif

    /* 创建任务 */
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

    /* 启动任务 */
    status = OSA_tskCmdStart(hTask);
    CU_ASSERT(OSA_SOK == status);

    CU_ASSERT(OSA_TRUE == gTestStart);

    /* 休眠5s */
    OSA_msleep(5000);

    CU_ASSERT(OSA_TRUE == gTestRun);

    /* 删除任务 */
    status =  OSA_tskDelete(hTask);
    CU_ASSERT(OSA_SOK == status);

#ifndef __KERNEL__
    /* 反初始化OSA */
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



