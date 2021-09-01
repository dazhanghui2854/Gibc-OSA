/*******************************************************************************
 * osa_utQue.c
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
#define QUE_TEST_NUM 8


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
typedef struct
{
    OSA_ListHead node;

    Uint32 val;
    
} QUE_ListElem;


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/* 测试队列接口 */
static void QUE_testQue(void)
{
    OSA_QueCreate create;
    OSA_QueHandle hQue;
    Int32         status;
    Uint32        aQueArr[QUE_TEST_NUM];
    Uint32        i = 0;
    Uint32        val;

    OSA_clear(&create);

    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    for ( i = 0 ; i < QUE_TEST_NUM; i++ )
    {
        status = OSA_quePut(hQue, i, OSA_TIMEOUT_FOREVER);
        CU_ASSERT(OSA_SOK == status);
    }

    for ( i = 0 ; i < QUE_TEST_NUM; i++ )
    {
        status = OSA_queGet(hQue, &val, OSA_TIMEOUT_FOREVER);
        CU_ASSERT(OSA_SOK == status);

        CU_ASSERT(val == i)
    }

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);
}

/* 测试轻量级队列读写接口 */
static void QUE_testQueSl(void)
{
    OSA_QueCreate create;
    OSA_QueHandle hQue;
    Int32         status;
    Uint32        aQueArr[QUE_TEST_NUM];
    Uint32        i = 0;
    Uint32        val;

    OSA_clear(&create);

    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    for ( i = 0 ; i < QUE_TEST_NUM; i++ )
    {
        status = OSA_queslPut(hQue, i);
        CU_ASSERT(OSA_SOK == status);
    }

    for ( i = 0 ; i < QUE_TEST_NUM; i++ )
    {
        status = OSA_queslGet(hQue, &val);
        CU_ASSERT(OSA_SOK == status);

        CU_ASSERT(val == i)
    }

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);
}

/* 测试链表队列读写接口 */
static void QUE_testQueList(void)
{
    OSA_QueCreate create;
    OSA_QueHandle hQue;
    Int32         status;
    QUE_ListElem  aQueArr[QUE_TEST_NUM];
    OSA_ListHead  *pQueNode = NULL;
    QUE_ListElem  *pQueElem = NULL;
    Uint32        i;

    for ( i = 0 ; i < QUE_TEST_NUM; i++ )
    {
        OSA_listHeadInit(&aQueArr[i].node);
        aQueArr[i].val = i;
    }

    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    
    status = OSA_queListCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    for ( i = 0 ; i < QUE_TEST_NUM; i++ )
    {
        status = OSA_queListPut(hQue, &aQueArr[i].node, OSA_TIMEOUT_FOREVER);
        CU_ASSERT(OSA_SOK == status);
    }

    for ( i = 0 ; i < QUE_TEST_NUM; i++ )
    {
        status = OSA_queListGet(hQue, &pQueNode, OSA_TIMEOUT_FOREVER);
        CU_ASSERT(OSA_SOK == status);

        pQueElem = OSA_listEntry(pQueNode, QUE_ListElem, node);
        CU_ASSERT(pQueElem->val == i);
    }

    status = OSA_queListDelete(hQue);
    CU_ASSERT(OSA_SOK == status);
}

/* 测试队列是否为空 */
static void QUE_testQueIsEmpty(void)
{
    OSA_QueCreate create;
    OSA_QueHandle hQue;
    Int32         status;
    Uint32        aQueArr[QUE_TEST_NUM];
    QUE_ListElem  queElem;
    Bool32        bisEmpty;

    /* 普通队列 */
    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    bisEmpty = OSA_queIsEmpty(hQue);
    CU_ASSERT(OSA_TRUE == bisEmpty);
    
    status = OSA_quePut(hQue, 1, OSA_TIMEOUT_FOREVER);
    CU_ASSERT(OSA_SOK == status);

    bisEmpty = OSA_queIsEmpty(hQue);
    CU_ASSERT(OSA_FALSE == bisEmpty);

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);

    /* 轻量级队列 */
    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    bisEmpty = OSA_queIsEmpty(hQue);
    CU_ASSERT(OSA_TRUE == bisEmpty);
    
    status = OSA_queslPut(hQue, 1);
    CU_ASSERT(OSA_SOK == status);

    bisEmpty = OSA_queIsEmpty(hQue);
    CU_ASSERT(OSA_FALSE == bisEmpty);

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);

    /* 链表队列 */
    OSA_listHeadInit(&queElem.node);

    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    status = OSA_queListCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    bisEmpty = OSA_queIsEmpty(hQue);
    CU_ASSERT(OSA_TRUE == bisEmpty);

    status = OSA_queListPut(hQue, &queElem.node, OSA_TIMEOUT_FOREVER);
    CU_ASSERT(OSA_SOK == status);
    
    bisEmpty = OSA_queIsEmpty(hQue);
    CU_ASSERT(OSA_FALSE == bisEmpty);
    
    status = OSA_queListDelete(hQue);
    CU_ASSERT(OSA_SOK == status);
}

/* 测试获取队列长度 */
static void OSA_testQueGetLen(void)
{
    OSA_QueCreate create;
    OSA_QueHandle hQue;
    Int32         status;
    Uint32        aQueArr[QUE_TEST_NUM];
    Uint32        queLen = 0;

    /* 普通队列 */
    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    queLen = OSA_queGetLen(hQue);
    CU_ASSERT(QUE_TEST_NUM == queLen);

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);

    /* 轻量级队列 */
    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    queLen = OSA_queGetLen(hQue);
    CU_ASSERT(QUE_TEST_NUM == queLen);

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);

    /* 链表队列 */
    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    status = OSA_queListCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    queLen = OSA_queGetLen(hQue);
    CU_ASSERT(QUE_TEST_NUM == queLen);
    
    status = OSA_queListDelete(hQue);
    CU_ASSERT(OSA_SOK == status);
}

/* 测试获取队列中的成员个数 */
static void OSA_testQueGetCount(void)
{
    OSA_QueCreate create;
    OSA_QueHandle hQue;
    Int32         status;
    Uint32        aQueArr[QUE_TEST_NUM];
    QUE_ListElem  queElem;
    Uint32        queCnt;

    /* 普通队列 */
    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);
    
    status = OSA_quePut(hQue, 1, OSA_TIMEOUT_FOREVER);
    CU_ASSERT(OSA_SOK == status);

    queCnt = OSA_queGetCount(hQue);
    CU_ASSERT(1 == queCnt);

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);

    /* 轻量级队列 */
    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    create.pQueue   = aQueArr;
    status = OSA_queCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);
    
    status = OSA_queslPut(hQue, 1);
    CU_ASSERT(OSA_SOK == status);

    queCnt = OSA_queGetCount(hQue);
    CU_ASSERT(1 == queCnt);

    status = OSA_queDelete(hQue);
    CU_ASSERT(OSA_SOK == status);

    /* 链表队列 */
    OSA_listHeadInit(&queElem.node);

    OSA_clear(&create);
    create.maxElems = QUE_TEST_NUM;
    status = OSA_queListCreate(&create, &hQue);
    CU_ASSERT(OSA_SOK == status);

    status = OSA_queListPut(hQue, &queElem.node, OSA_TIMEOUT_FOREVER);
    CU_ASSERT(OSA_SOK == status);
    
    queCnt = OSA_queGetCount(hQue);
    CU_ASSERT(1 == queCnt);

    status = OSA_queListDelete(hQue);
    CU_ASSERT(OSA_SOK == status);
}

void OSA_utQueSuit(void)
{
    CU_SuiteHandle pSuite;

    CU_addSuite("OSA_utQue",NULL,NULL,&pSuite);
    
    CU_ADD_TEST(pSuite, QUE_testQue);
    CU_ADD_TEST(pSuite, QUE_testQueSl);
    CU_ADD_TEST(pSuite, QUE_testQueList);
    CU_ADD_TEST(pSuite, QUE_testQueIsEmpty);
    CU_ADD_TEST(pSuite, OSA_testQueGetLen);
    CU_ADD_TEST(pSuite, OSA_testQueGetCount);
}


