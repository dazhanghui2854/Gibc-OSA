/*******************************************************************************
* osa_test.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: ����OSAģ����Դ���ͷ�ļ�
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_TEST_H_
#define _OSA_TEST_H_


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/*�����ַ��豸�������*/
#define OSA_KCDEV_BASE_CMD         'O'

/*
 * �ַ��豸������ID
 */
#define CMD_OSA_KCDEV_TESTPRINT         _IOWR(OSA_KCDEV_BASE_CMD,\
                                        0u,\
                                        Uint32)


/*�����������*/
typedef enum
{
    OSA_TEST_THR = 0,   /*����osa_thr.h�ӿ�*/
    OSA_TEST_SEM,       /*����osa_sem.h�ӿ�*/
    OSA_TEST_TIMER,     /*����osa_timer.h�ӿ�*/
    OSA_TEST_MUTEX,     /*����osa_mutex.h�ӿ�*/    
    OSA_TEST_KPROC,     /*����osa_kproc.h�ӿ�*/
    OSA_TEST_KCDEV,     /*����osa_kcdev.h�ӿ�*/
    OSA_TEST_MEM,       /*����osa_mem.h�ӿ�*/
    OSA_TEST_MAX
}OSA_TestType;


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

Int32 OSA_testBegin(Int32 type);
Int32 OSA_testEnd(Int32 type);

/*�����̵߳Ľӿ�*/
Int32 OSA_testThrBegin(void);
Int32 OSA_testThrEnd(void);

/*�����ź����Ľӿ�*/
Int32 OSA_testSemBegin(void);
Int32 OSA_testSemEnd(void);

/*���Զ�ʱ���Ľӿ�*/
Int32 OSA_testTimerBegin(void);
Int32 OSA_testTimerEnd(void);

/*���Ի������Ľӿ�*/
Int32 OSA_testMutexBegin(void);
Int32 OSA_testMutexEnd(void);

/*����linux�ں�proc�Ľӿ�*/
Int32 OSA_testKProcBegin(void);
Int32 OSA_testKProcEnd(void);

/*����linux�ں��ַ��豸�Ľӿ�*/
Int32 OSA_testKCdevBegin(void);
Int32 OSA_testKCdevEnd(void);

Int32 OSA_testMemBegin(void);
Int32 OSA_testMemEnd(void);


#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_TEST_H_  */



