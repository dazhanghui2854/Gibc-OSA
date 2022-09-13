/*******************************************************************************
* osa_utInit.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng Wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2013年9月12日 Create
*
* Description: osa初始化相关接口用例
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

#ifndef __KERNEL__

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

/* 正常情况 */
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

/* 输入为NULL */
static void INIT_testInitNull(void)
{
    Int32         status;

    status = OSA_init(NULL);
    CU_ASSERT(OSA_EFAIL == status);
}

/* 重复初始化多次 */
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

