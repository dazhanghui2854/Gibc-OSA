/*******************************************************************************
* osa_func.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSAģ��ȫ�ֳ�ʼ�������ýӿڡ�����������OSAϵ��ͷ�ļ���
*
*       1. Ӳ��˵����
*          �ޡ�
*
*       2. ����ṹ˵����
*          ��
*
*       3. ʹ��˵����
*          �ޡ�
*
*       4. ������˵����
*          �ޡ�
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


#ifndef __OSA_FUNC_H__
#define __OSA_FUNC_H__


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */


#include <osa_types.h>
#include <osa_macro.h>
#include <osa_log.h>
#include <osa_trace_level.h>

#include <osa_list.h>
#include <osa_string.h>
#include <osa_mem.h>
#include <osa_sem.h>
#include <osa_mutex.h>
#include <osa_file.h>
#include <osa_cmpl.h>
#include <osa_time.h>
#include <osa_prf.h>
#include <osa_irq.h>
#include <osa_timer.h>
#include <osa_reg.h>
#include <osa_que.h>
#include <osa_msgq.h>
#include <osa_mbx.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_flg.h>
#include <osa_spinlock.h>
#include <osa_kwait.h>
#include <osa_kcdev.h>
#include <osa_kproc.h>
#include <osa_ktasklet.h>
#include <osa_kworkque.h>
#include <osa_uipc.h>
#include <osa_umisc.h>
#include <osa_usignal.h>
#include <osa_uwait.h>
#include <osa_cache.h>
#include <osa_pm.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/* OSA�İ汾�ţ������ڼ������ԡ�*/
#define OSA_VERSION  \
            (OSA_versionSet(1, 0, 0))


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

typedef struct
{
    /* �Ƿ�������������ڼ�����������Ƿ�ʱ�������쳣�����*/
    Bool32 isStartMonTask;

    Uint32 reserved[8];
} OSA_initParms;


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
* ������  : OSA_init
* ��  ��  : ��ʼ��osa��LinuxӦ�ò��SYSBIOS�ĳ�����ʹ��osa�����ӿ�ǰ�������
*           �ýӿڣ�Linux�������ص��ã�Ҳ���ò��ˣ�����û�е�����
* ��  ��  : - pInitParms: ��ʼ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: У��ɹ���
*           OSA_EFAIL: У��ʧ�ܡ�
*******************************************************************************/
Int32 OSA_init(OSA_initParms *pInitParms);


/*******************************************************************************
* ������  : OSA_deinit
* ��  ��  : ����osa��������OSA_init()���ʹ�á�
* ��  ��  : �ޡ�
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: У��ɹ���
*           OSA_EFAIL: У��ʧ�ܡ�
*******************************************************************************/
Int32 OSA_deinit(void);


/*******************************************************************************
* ������  : OSA_validate
* ��  ��  : У��OSA�汾�ļ����ԡ�
*           ͨ������ͷ�ļ�����İ汾��OSA_VERSION���ݵ��ú����ڲ�
*           ���бȽϣ�У���ͷ�ļ�����Ӧ��ļ����ԡ��ڸú����ڲ�������汾��
*           ��ȣ�ʹ����ɫ�����ӡ���汾�ţ��������ȵ��ɼ��ݣ���ʹ�û�ɫ��
*           ���ӡ��������Ϣ����������ݣ���ʹ�ú�ɫ�����ӡ������Ϣ��ǰ����
*           �������������ȷֵ�����һ��������ش���ֵ�����Ե����߱����鷵
*           ��ֵ����ȷ�ϰ汾�ļ����ԡ�
* ��  ��  : - nVersion: ͷ�ļ��ж���İ汾��OSA_VERSION��
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: У��ɹ���
*           OSA_EFAIL: У��ʧ�ܡ�
*******************************************************************************/
Int32 OSA_validate(Uint32 nVersion);


#ifdef __cplusplus
}
#endif


#endif /* __OSA_FUNC_H__ */



