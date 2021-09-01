/*******************************************************************************
* osa_test.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSAģ�鹫�����Դ������
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include <osa.h>
#include <osa_test.h>

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            ȫ�ֱ���������                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

Int32 OSA_testBegin(Int32 type)
{
    switch(type)
    {
        case OSA_TEST_THR:
            OSA_testThrBegin();
            break;

        case OSA_TEST_SEM:
            OSA_testSemBegin();
            break;

        case OSA_TEST_TIMER:
            OSA_testTimerBegin();
            break;

        case OSA_TEST_MUTEX:
            OSA_testMutexBegin();
            break;

        case OSA_TEST_KPROC:
            OSA_testKProcBegin();
            break;

        case OSA_TEST_KCDEV:
            OSA_testKCdevBegin();
            break;
        
        case OSA_TEST_MEM:
            OSA_testMemBegin();
            break;
        
        default:
            OSA_ERROR("Invalid test type:%d\n", type);
            return OSA_EFAIL;
    }

    return OSA_SOK;
}

Int32 OSA_testEnd(Int32 type)
{
    switch(type)
    {
        case OSA_TEST_THR:
            OSA_testThrEnd();
            break;
            
        case OSA_TEST_SEM:
            OSA_testSemEnd();
            break;
            
        case OSA_TEST_TIMER:
            OSA_testTimerEnd();
            break;

        case OSA_TEST_MUTEX:
            OSA_testMutexEnd();
            break;

        case OSA_TEST_KPROC:
            OSA_testKProcEnd();
            break;
            
        case OSA_TEST_KCDEV:
            OSA_testKCdevEnd();
            break;
            
        case OSA_TEST_MEM:
            OSA_testMemEnd();
            break;
        
        default:
            OSA_ERROR("Invalid test type:%d\n", type);
            return OSA_EFAIL;
    }

    return OSA_SOK;
}



