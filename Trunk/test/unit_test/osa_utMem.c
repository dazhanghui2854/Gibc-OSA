/*******************************************************************************
 * osa_utMem.c
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

/* 测试alloc和free */
static void MEM_testAllocFree(void)
{
    Ptr    p;
    Int32  status;
    Uint32 size = 1024;

    p = OSA_memAlloc(size);
    CU_ASSERT_PTR_NOT_NULL(p);

    status = OSA_memFree(p);
    CU_ASSERT(OSA_SOK == status);
}

/* 测试calloc和free */
static void MEM_testCallocFree(void)
{
    Uint8 *p;
    Uint8 *pStart;
    Int32  status;
    Uint32 size = 1024;
    Uint32 i = 0;

    p = OSA_memCalloc(size);
    CU_ASSERT_PTR_NOT_NULL(p);
    pStart = p;

    for ( i = 0 ; i < size; i++,p++ )
    {
        CU_ASSERT(0 == (*p));
    }

    status = OSA_memFree(pStart);
    CU_ASSERT(OSA_SOK == status);    
}

/* 测试非4字节对齐 */
static void MEM_testAllocNoAlign(void)
{
    Ptr p;

    p = OSA_memUAllocAlign(1023, 3);
    CU_ASSERT_PTR_NULL(p);
}

/* 测试释放空指针 */
static void MEM_testFreeNULL(void)
{
    Int32 status;

    status = OSA_memFree(NULL);
    CU_ASSERT(OSA_EFAIL == status)
}

/* 测试分配0字节的内存 */
static void MEM_testAllocZero(void)
{
    Ptr p;

    p = OSA_memAlloc(0);
    CU_ASSERT_PTR_NULL(p);
}

void OSA_utMemSuit(void)
{
    CU_SuiteHandle pSuite;

    CU_addSuite("OSA_utMem",NULL,NULL,&pSuite);

    CU_ADD_TEST(pSuite, MEM_testAllocFree);
    CU_ADD_TEST(pSuite, MEM_testCallocFree);
    CU_ADD_TEST(pSuite, MEM_testAllocNoAlign);
    CU_ADD_TEST(pSuite, MEM_testFreeNULL);
    CU_ADD_TEST(pSuite, MEM_testAllocZero);
}

