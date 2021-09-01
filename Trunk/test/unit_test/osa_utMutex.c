/*******************************************************************************
 * osa_utMutex.c
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

/* 测试创建删除互斥锁 */
static void MUTEX_testCreateDel(void)
{
    Int32           status;
    OSA_MutexHandle hMutex;

    status = OSA_mutexCreate(OSA_MUTEX_NORMAL,&hMutex);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_mutexDelete(hMutex);
    CU_ASSERT(OSA_SOK == status);
}

/* 测试加解锁 */
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

/* 测试递归锁 */
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



