/*******************************************************************************
 * osa_utSem.c
 *
 * Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
 *
 * Author : Zheng Wei <zheng_wei@dahuatech.com>
 * Version: V1.0.0  2013年9月12日 Create
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

/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/* 测试创建删除信号量 */
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

/* 测试信号量Pend */
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

/* 测试信号量post */
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



