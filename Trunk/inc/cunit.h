/*******************************************************************************
 * cunit.h
 *
 * Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
 *
 * Author : wang_kefu <wang_kefu@dahuatech.com>
 * Version: V1.0.0  2013��03��22�� Create
 *
 * Description: C ���Ե�Ԫ���Կ��
 *       ֧�ֿ���̨������Խ��
 *       ֧��XML������Ա���
 *       �ڳ����쳣ʱ�����������������ջ��PCָ��
 *       �ڵ�ǰ�����쳣ʱ���ɼ�������Ĳ��ԣ������ǳ����˳�
 *
 *       1. Ӳ��˵����
 *          �ޡ�
 *       2. ����ṹ˵����
 *          ��
 *
 *       3. ʹ��˵����
 *          ��������˳��                     ����������֯�ṹͼ
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
 *       4. ������˵����
 *          �ÿ��Ŀǰֻ֧��Linux���û�̬����
 *          �ÿ��Ŀǰֻ֧�ֵ��̲߳���
 *
 *       5. ����˵����
 *          �ޡ�
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
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include "osa.h"


#ifdef __cplusplus
extern "C" {
#endif
/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
#define CU_VERSION "2.1-2"

/* suite ��� */
typedef Handle CU_SuiteHandle;

/* ����̨�����Ϣ������ */
typedef enum
{
    CU_OUTPUT_MODE_NORMAL = 0,          /* ���������Ϣ������ */
    CU_OUTPUT_MODE_SILENT,              /* ��������Ա��� */
    CU_OUTPUT_MODE_VERBOSE,             /* �������Ĳ�����Ϣ */
} CU_OutputMode;

/* ���в��� */
typedef struct
{
    Uint32 outputMode;                  /* �ο� CU_OutputMode */
    Uint32 outXmlReport;                /* �Ƿ���� xml ���� */
    char *xmlFilePrefix;                /* xml �ļ�ǰ׺ */
    char *xslDir;                       /* xsl �ļ���� xml �ļ���·�� */
} CU_RunOpt;

/* suite ��ʼ���ص�������������suiteǰ������ */
typedef Int32(*CU_InitializeFunc)(void);

/* suite ����ص����������˳�suiteʱ������ */
typedef Int32(*CU_CleanupFunc)(void);

/* test ����ԭ�� */
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
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
 * ������  : CU_init
 * ��  ��  : CUnit ��ʼ��
 * ��  ��  : - : ��
 * ��  ��  :
 * ����ֵ  : OSA_SOK:   �ɹ���
 *           OSA_EFAIL: ʧ�ܡ�
 *******************************************************************************/
Int32 CU_init(void);


/*******************************************************************************
 * ������  : CU_deInit
 * ��  ��  : CUnit ����
 * ��  ��  : - : ��
 * ��  ��  :
 * ����ֵ  : OSA_SOK:   �ɹ���
 *           OSA_EFAIL: ʧ�ܡ�
 *******************************************************************************/
Int32 CU_deInit(void);


/*******************************************************************************
 * ������  : CU_addSuite
 * ��  ��  : ����һ�� suite ����ӵ����Կ����
 * ��  ��  : -strName: suite ����
 *           -pInit  : suite ��ʼ���ص�����������Ϊ NULL
 *           -pClean  : suite �˳��ص�����������Ϊ NULL
 * ��  ��  : -phSuite: �´����� suite���
 * ����ֵ  : OSA_SOK:   �ɹ���
 *           OSA_EFAIL: ʧ�ܡ�
 *******************************************************************************/
Int32 CU_addSuite(const char *strName,
                  CU_InitializeFunc pInit,
                  CU_CleanupFunc pClean,
                  CU_SuiteHandle *phSuite);


/*******************************************************************************
 * ������  : CU_addTest
 * ��  ��  : ����һ�� test ����ӵ� suit��
 *           ͨ������£���������ͬ���Ժ�����������ʹ�� CU_ADD_TEST ��򻯲���
 * ��  ��  : -pSuite:ϣ����ӵ���suite
 *           -strName:test������
 *           -pTestFunc:test�Ĳ��Ժ���
 * ��  ��  : ��
 * ����ֵ  : OSA_SOK:   �ɹ���
 *           OSA_EFAIL: ʧ�ܡ�
 *******************************************************************************/
Int32 CU_addTest(CU_SuiteHandle hSuite, const char *strName,
                 CU_TestFunc pTestFunc);


/*******************************************************************************
 * ������  : CU_runAllTests
 * ��  ��  : �������в���
 *           ���й����е���������н����Ĭ��Ϊ����̨
 *           ���ָ�� pOpt->outXmlReport���ɽ����Խ��ͬʱ�����xml�ļ��С�
 *           Ϊ������IE����ʾ��xml�ļ�����ָ��xsl�ļ����xml�ļ���·����Ҳ��ʹ��
 *           ����·����
 * ��  ��  : -pOpt: ����ѡ��
 * ��  ��  : ��
 * ����ֵ  : OSA_SOK:   �ɹ���
 *           OSA_EFAIL: ʧ�ܡ�
 *******************************************************************************/
Int32 CU_runAllTests(CU_RunOpt *pOpt);


/*******************************************************************************
* ������  : CU_runSuite
* ��  ��  : ����ĳ��suite
* ��  ��  : - : hSuite ��Ҫ���е�suite
*           - : pOpt   ����ѡ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 CU_runSuite(CU_SuiteHandle hSuite, CU_RunOpt *pOpt);


/*******************************************************************************
 * ������  : CU_assertImplementation
 * ��  ��  : CU���Ե�ʵ�֣���ͨ�� CU_ASSERT �Ⱥ�����������Ҫֱ�ӵ��á�
 *           ����ֻ�ܳ�����test������
 * ��  ��  : - :
 * ��  ��  :
 * ����ֵ  : OSA_SOK:   �ɹ���
 *           OSA_EFAIL: ʧ�ܡ�
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
