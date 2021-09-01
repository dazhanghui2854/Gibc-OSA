/*******************************************************************************
 * cunit.h
 *
 * Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
 *
 * Author : wang_kefu <wang_kefu@dahuatech.com>
 * Version: V1.0.0  2013年03月22日 Create
 *
 * Description: C 语言单元测试框架
 *       支持控制台输出测试结果
 *       支持XML输出测试报告
 *       在程序异常时，可以输出函数调用栈及PC指针
 *       在当前测试异常时，可继续后面的测试，而不是程序退出
 *
 *       1. 硬件说明。
 *          无。
 *       2. 程序结构说明。
 *          无
 *
 *       3. 使用说明。
 *          函数调用顺序                     测试用例组织结构图
 *          CU_init                                CUnit
 *              |                                /   |    \
 *              |                              /     |      \
 *              |                           /        |        \
 *          CU_addSuite                  suit      suit      suit
 *              |                       /        /   |    \     \
 *              |                     /        /     |      \     \
 *              |                  /        /        |        \     \
 *          CU_addTest           test     test      test     tes   testt
 *              |                                /   |    \
 *              |                              /     |      \
 *              |                           /        |        \
 *          CU_ASSERT                   assert    assert     assert
 *              |
 *              |
 *              |
 *          CU_runAllTests
 *              |
 *              |
 *              |
 *          CU_deInit
 *
 *
 *       4. 局限性说明。
 *          该框架目前只支持Linux的用户态程序。
 *          该框架目前只支持单线程测试
 *
 *       5. 其他说明。
 *          无。
 *
 * Modification:
 *    Date    :
 *    Revision:
 *    Author  :
 *    Contents:
 *******************************************************************************/

#ifndef __CU_HH__
#define __CU_HH__
/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include "osa.h"


#ifdef __cplusplus
extern "C" {
#endif
/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define CU_VERSION "2.1-2"

/* suite 句柄 */
typedef Handle CU_SuiteHandle;

/* 控制台输出信息量控制 */
typedef enum
{
    CU_OUTPUT_MODE_NORMAL = 0,          /* 输出出错信息及报告 */
    CU_OUTPUT_MODE_SILENT,              /* 仅输出测试报告 */
    CU_OUTPUT_MODE_VERBOSE,             /* 输出更多的测试信息 */
} CU_OutputMode;

/* 运行参数 */
typedef struct
{
    Uint32 outputMode;                  /* 参考 CU_OutputMode */
    Uint32 outXmlReport;                /* 是否输出 xml 报告 */
    char *xmlFilePrefix;                /* xml 文件前缀 */
    char *xslDir;                       /* xsl 文件相对 xml 文件的路径 */
} CU_RunOpt;

/* suite 初始化回调函数，在运行suite前被调用 */
typedef Int32(*CU_InitializeFunc)(void);

/* suite 清理回调函数，在退出suite时被调用 */
typedef Int32(*CU_CleanupFunc)(void);

/* test 函数原形 */
typedef void (*CU_TestFunc)(void);

#define CU_ADD_TEST(suite, test) (CU_addTest(suite, #test, (CU_TestFunc)test))

/** Record a pass condition without performing a logical test. */
#define CU_PASS(msg) \
{ CU_assertImplementation(OSA_TRUE, __LINE__, ("CU_PASS(" #msg ")"),\
        __FILE__, "", OSA_FALSE); }

/** Simple assertion.
 *  Reports failure with no other action.
 */
#define CU_ASSERT(value) \
{ CU_assertImplementation((value), __LINE__, #value, __FILE__, "", OSA_FALSE); }

/** Simple assertion.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_FATAL(value) \
{ CU_assertImplementation((value), __LINE__, #value, __FILE__, "", OSA_TRUE); }

/** Record a failure without performing a logical test. */
#define CU_FAIL(msg) \
{ CU_assertImplementation(OSA_FALSE, __LINE__, ("CU_FAIL(" #msg ")"),\
        __FILE__, "", OSA_FALSE); }

/** Record a failure without performing a logical test, and abort test. */
#define CU_FAIL_FATAL(msg) \
{ CU_assertImplementation(OSA_FALSE, __LINE__, ("CU_FAIL_FATAL(" #msg ")"),\
        __FILE__, "", OSA_TRUE); }

/** Asserts that value is OSA_TRUE.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_TRUE(value) \
{ CU_assertImplementation((value), __LINE__, ("CU_ASSERT_TRUE(" #value ")"),\
        __FILE__, "", OSA_FALSE); }

/** Asserts that value is OSA_TRUE.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_TRUE_FATAL(value) \
{ CU_assertImplementation((value), __LINE__, ("CU_ASSERT_TRUE_FATAL(" #value ")"),\
        __FILE__, "", OSA_TRUE); }

/** Asserts that value is OSA_FALSE.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_FALSE(value) \
{ CU_assertImplementation(!(value), __LINE__, ("CU_ASSERT_FALSE(" #value ")"),\
        __FILE__, "", OSA_FALSE); }

/** Asserts that value is OSA_FALSE.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_FALSE_FATAL(value) \
{ CU_assertImplementation(!(value), __LINE__, ("CU_ASSERT_FALSE_FATAL(" #value ")"),\
        __FILE__, "", OSA_TRUE); }

/** Asserts that actual == expected.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_EQUAL(actual, expected) \
{ CU_assertImplementation(((actual) == (expected)), __LINE__,\
        ("CU_ASSERT_EQUAL(" #actual "," #expected ")"), __FILE__, "", OSA_FALSE); }

/** Asserts that actual == expected.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_EQUAL_FATAL(actual, expected) \
{ CU_assertImplementation(((actual) == (expected)), __LINE__,\
        ("CU_ASSERT_EQUAL_FATAL(" #actual "," #expected ")"), __FILE__, "", OSA_TRUE); }

/** Asserts that actual != expected.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_NOT_EQUAL(actual, expected) \
{ CU_assertImplementation(((actual) != (expected)), __LINE__,\
        ("CU_ASSERT_NOT_EQUAL(" #actual "," #expected ")"), __FILE__, "", OSA_FALSE); }

/** Asserts that actual != expected.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_NOT_EQUAL_FATAL(actual, expected) \
{ CU_assertImplementation(((actual) != (expected)), __LINE__,\
        ("CU_ASSERT_NOT_EQUAL_FATAL(" #actual "," #expected ")"), __FILE__, "", OSA_TRUE); }

/** Asserts that pointers actual == expected.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_PTR_EQUAL(actual, expected) \
{ CU_assertImplementation(((void*)(actual) == (void*)(expected)), __LINE__,\
        ("CU_ASSERT_PTR_EQUAL(" #actual "," #expected ")"), __FILE__, "", OSA_FALSE); }

/** Asserts that pointers actual == expected.
 * Reports failure and causes test to abort.
 */
#define CU_ASSERT_PTR_EQUAL_FATAL(actual, expected) \
{ CU_assertImplementation(((void*)(actual) == (void*)(expected)), __LINE__,\
        ("CU_ASSERT_PTR_EQUAL_FATAL(" #actual "," #expected ")"), __FILE__, "", OSA_TRUE); }

/** Asserts that pointers actual != expected.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_PTR_NOT_EQUAL(actual, expected) \
{ CU_assertImplementation(((void*)(actual) != (void*)(expected)), __LINE__,\
        ("CU_ASSERT_PTR_NOT_EQUAL(" #actual "," #expected ")"), __FILE__, "", OSA_FALSE); }

/** Asserts that pointers actual != expected.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_PTR_NOT_EQUAL_FATAL(actual, expected) \
{ CU_assertImplementation(((void*)(actual) != (void*)(expected)),\
        __LINE__, ("CU_ASSERT_PTR_NOT_EQUAL_FATAL(" #actual "," #expected ")"),\
        __FILE__, "", OSA_TRUE); }

/** Asserts that pointer value is NULL.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_PTR_NULL(value) \
{ CU_assertImplementation((NULL == (void*)(value)), __LINE__,\
        ("CU_ASSERT_PTR_NULL(" #value")"), __FILE__, "", OSA_FALSE); }

/** Asserts that pointer value is NULL.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_PTR_NULL_FATAL(value) \
{ CU_assertImplementation((NULL == (void*)(value)), __LINE__,\
        ("CU_ASSERT_PTR_NULL_FATAL(" #value")"), __FILE__, "", OSA_TRUE); }

/** Asserts that pointer value is not NULL.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_PTR_NOT_NULL(value) \
{ CU_assertImplementation((NULL != (void*)(value)), __LINE__,\
        ("CU_ASSERT_PTR_NOT_NULL(" #value")"), __FILE__, "", OSA_FALSE); }

/** Asserts that pointer value is not NULL.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_PTR_NOT_NULL_FATAL(value) \
{ CU_assertImplementation((NULL != (void*)(value)), __LINE__,\
        ("CU_ASSERT_PTR_NOT_NULL_FATAL(" #value")"), __FILE__, "", OSA_TRUE); }

/** Asserts that string actual == expected.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_STRING_EQUAL(actual, expected) \
{ CU_assertImplementation(!(strcmp((const char*)(actual),\
                (const char*)(expected))), __LINE__,\
        ("CU_ASSERT_STRING_EQUAL(" #actual ","  #expected ")"),\
        __FILE__, "", OSA_FALSE); }

/** Asserts that string actual == expected.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_STRING_EQUAL_FATAL(actual, expected) \
{ CU_assertImplementation(!(strcmp((const char*)(actual),\
                (const char*)(expected))), __LINE__,\
        ("CU_ASSERT_STRING_EQUAL_FATAL(" #actual ","  #expected ")"),\
        __FILE__, "", OSA_TRUE); }

/** Asserts that string actual != expected.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_STRING_NOT_EQUAL(actual, expected) \
{ CU_assertImplementation((strcmp((const char*)(actual),\
                (const char*)(expected))), __LINE__,\
        ("CU_ASSERT_STRING_NOT_EQUAL(" #actual ","  #expected ")")\
        , __FILE__, "", OSA_FALSE); }

/** Asserts that string actual != expected.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_STRING_NOT_EQUAL_FATAL(actual, expected) \
{ CU_assertImplementation((strcmp((const char*)(actual),\
                (const char*)(expected))), __LINE__,\
        ("CU_ASSERT_STRING_NOT_EQUAL_FATAL(" #actual ","  #expected ")")\
        , __FILE__, "", OSA_TRUE); }

/** Asserts that string actual == expected with length specified.
 *  The comparison is limited to count characters.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_NSTRING_EQUAL(actual, expected, count) \
{ CU_assertImplementation(!(strncmp((const char*)(actual),\
                (const char*)(expected), (size_t)(count))), __LINE__,\
        ("CU_ASSERT_NSTRING_EQUAL(" #actual ","  #expected "," #count ")"),\
        __FILE__, "", OSA_FALSE); }

/** Asserts that string actual == expected with length specified.
 *  The comparison is limited to count characters.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_NSTRING_EQUAL_FATAL(actual, expected, count) \
{ CU_assertImplementation(!(strncmp((const char*)(actual),\
                (const char*)(expected), (size_t)(count))), __LINE__,\
        ("CU_ASSERT_NSTRING_EQUAL_FATAL(" #actual ","  #expected "," #count ")"),\
        __FILE__, "", OSA_TRUE); }

/** Asserts that string actual != expected with length specified.
 *  The comparison is limited to count characters.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_NSTRING_NOT_EQUAL(actual, expected, count) \
{ CU_assertImplementation((strncmp((const char*)(actual),\
                (const char*)(expected), (size_t)(count))), __LINE__,\
        ("CU_ASSERT_NSTRING_NOT_EQUAL(" #actual ","  #expected "," #count ")"),\
        __FILE__, "", OSA_FALSE); }

/** Asserts that string actual != expected with length specified.
 *  The comparison is limited to count characters.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_NSTRING_NOT_EQUAL_FATAL(actual, expected, count) \
{ CU_assertImplementation((strncmp((const char*)(actual),\
                (const char*)(expected), (size_t)(count))), __LINE__,\
        ("CU_ASSERT_NSTRING_NOT_EQUAL_FATAL(" #actual ","  #expected "," #count ")"),\
        __FILE__, "", OSA_TRUE); }

/** Asserts that double actual == expected within the specified tolerance.
 *  If actual is within granularity of expected, the assertion passes.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_DOUBLE_EQUAL(actual, expected, granularity) \
{ CU_assertImplementation(((fabs((double)(actual) - (expected))\
                <= fabs((double)(granularity)))), __LINE__, \
        ("CU_ASSERT_DOUBLE_EQUAL(" #actual ","  #expected "," #granularity ")"),\
        __FILE__, "", OSA_FALSE); }

/** Asserts that double actual == expected within the specified tolerance.
 *  If actual is within granularity of expected, the assertion passes.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_DOUBLE_EQUAL_FATAL(actual, expected, granularity) \
{ CU_assertImplementation(((fabs((double)(actual) - (expected)) \
                <= fabs((double)(granularity)))), __LINE__,\
        ("CU_ASSERT_DOUBLE_EQUAL_FATAL(" #actual ","  #expected "," #granularity ")")\
        , __FILE__, "", OSA_TRUE); }

/** Asserts that double actual != expected within the specified tolerance.
 *  If actual is within granularity of expected, the assertion fails.
 *  Reports failure with no other action.
 */
#define CU_ASSERT_DOUBLE_NOT_EQUAL(actual, expected, granularity) \
{ CU_assertImplementation(((fabs((double)(actual) - (expected))\
                > fabs((double)(granularity)))), __LINE__,\
        ("CU_ASSERT_DOUBLE_NOT_EQUAL(" #actual ","  #expected "," #granularity ")"),\
        __FILE__, "", OSA_FALSE); }

/** Asserts that double actual != expected within the specified tolerance.
 *  If actual is within granularity of expected, the assertion fails.
 *  Reports failure and causes test to abort.
 */
#define CU_ASSERT_DOUBLE_NOT_EQUAL_FATAL(actual, expected, granularity) \
{ CU_assertImplementation(((fabs((double)(actual) - (expected)) >\
                fabs((double)(granularity)))), __LINE__,\
        ("CU_ASSERT_DOUBLE_NOT_EQUAL_FATAL(" #actual ","  #expected "," #granularity ")"),\
        __FILE__, "", OSA_TRUE); }

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
 * 函数名  : CU_init
 * 描  述  : CUnit 初始化
 * 输  入  : - : 无
 * 输  出  :
 * 返回值  : OSA_SOK:   成功。
 *           OSA_EFAIL: 失败。
 *******************************************************************************/
Int32 CU_init(void);


/*******************************************************************************
 * 函数名  : CU_deInit
 * 描  述  : CUnit 清理
 * 输  入  : - : 无
 * 输  出  :
 * 返回值  : OSA_SOK:   成功。
 *           OSA_EFAIL: 失败。
 *******************************************************************************/
Int32 CU_deInit(void);


/*******************************************************************************
 * 函数名  : CU_addSuite
 * 描  述  : 创建一个 suite 并添加到测试框架中
 * 输  入  : -strName: suite 名称
 *           -pInit  : suite 初始化回调函数，允许为 NULL
 *           -pClean  : suite 退出回调函数，允许为 NULL
 * 输  出  : -phSuite: 新创建的 suite句柄
 * 返回值  : OSA_SOK:   成功。
 *           OSA_EFAIL: 失败。
 *******************************************************************************/
Int32 CU_addSuite(const char *strName,
                  CU_InitializeFunc pInit,
                  CU_CleanupFunc pClean,
                  CU_SuiteHandle *phSuite);


/*******************************************************************************
 * 函数名  : CU_addTest
 * 描  述  : 创建一个 test 并添加到 suit中
 *           通常情况下，测试名称同测试函数名，建议使用 CU_ADD_TEST 宏简化操作
 * 输  入  : -pSuite:希望添加到的suite
 *           -strName:test的名称
 *           -pTestFunc:test的测试函数
 * 输  出  : 无
 * 返回值  : OSA_SOK:   成功。
 *           OSA_EFAIL: 失败。
 *******************************************************************************/
Int32 CU_addTest(CU_SuiteHandle hSuite, const char *strName,
                 CU_TestFunc pTestFunc);


/*******************************************************************************
 * 函数名  : CU_runAllTests
 * 描  述  : 运行所有测试
 *           运行过程中的输出及运行结果，默认为控制台
 *           如果指定 pOpt->outXmlReport，可将测试结果同时输出到xml文件中。
 *           为了能在IE中显示该xml文件，需指定xsl文件相对xml文件的路径，也可使用
 *           绝对路径。
 * 输  入  : -pOpt: 运行选项
 * 输  出  : 无
 * 返回值  : OSA_SOK:   成功。
 *           OSA_EFAIL: 失败。
 *******************************************************************************/
Int32 CU_runAllTests(CU_RunOpt *pOpt);


/*******************************************************************************
* 函数名  : CU_runSuite
* 描  述  : 运行某个suite
* 输  入  : - : hSuite 需要运行的suite
*           - : pOpt   运行选项
* 输  出  : 无
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 CU_runSuite(CU_SuiteHandle hSuite, CU_RunOpt *pOpt);


/*******************************************************************************
 * 函数名  : CU_assertImplementation
 * 描  述  : CU断言的实现，请通过 CU_ASSERT 等宏来操作，不要直接调用。
 *           断言只能出现在test函数中
 * 输  入  : - :
 * 输  出  :
 * 返回值  : OSA_SOK:   成功。
 *           OSA_EFAIL: 失败。
 *******************************************************************************/
Int32 CU_assertImplementation(Bool32 bValue,
                              Uint32 uiLine,
                              const char *strCondition,
                              const char *strFile,
                              const char *strFunction,
                              Bool32 bFatal);

#ifdef __cplusplus
}
#endif
#endif
