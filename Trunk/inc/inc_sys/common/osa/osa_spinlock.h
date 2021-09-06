/*******************************************************************************
* osa_kspinlock.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: ʵ��OSAģ������ṩ���������ӿ�
*
*           �ӿڵ�����������:
*           ==========================
*                   |                            
*           OSA_spinlockCreate
*                   |
*           OSA_spinLock/OSA_spinUnlock/OSA_spinTrylock
*                   |
*           OSA_spinlockDelete
*           ===========================
*
* Modification: 
*    Date    :  2012-10-25
*    Revision:  V1.10
*    Author  :  Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
*    Contents:  ������÷�ʽ��ʹ�ûص�����ʵ�֡�������ȥ������kernel��'k'����Ϊ
*               ����������ҲҪʵ�֡�
*******************************************************************************/


#ifndef _OSA_SPINLOCK_H_
#define _OSA_SPINLOCK_H_


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
/* ����OSA_kSpinlockCreate���ص��������������,�ϲ�ģ�鲻��Ҫ�������еľ�������,*/
/* ֻ��Ҫ�ڸ������������ӿ��д���þ������*/
typedef Handle OSA_SpinlockHandle;


/* �������ӽ��������͡�SYSBIOS��֧��OSA_SPINLOCK_IRQ��*/
typedef enum
{
    OSA_SPINLOCK_NORMAL  = 0,  /* ��Ӧspin_lock��spin_unlock */
    OSA_SPINLOCK_BH,           /* ��Ӧspin_lock_bh��spin_unlock_bh */
    OSA_SPINLOCK_IRQ,          /* ��Ӧspin_lock_irq��spin_unlock_irq */
    OSA_SPINLOCK_IRQSAVE       /* ��Ӧspin_lock_irqsave��spin_unlock_irqrestore*/
} OSA_SpinlockType;

/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
* ������  : OSA_spinlockCreate
* ��  ��  : �ú������𴴽�һ��������
*           �ýӿڲ������ж������ĵ���
*
* ��  ��  : - spinType  : ��������,���嶨��μ�OSA_SpinlockType��
*
* ��  ��  : - phSpinlock: ������ ���ָ��,�������ɹ�ʱ������������
* ����ֵ  : OSA_SOK:      �����ɹ�
*           OSA_EFAIL:    ����ʧ��
*******************************************************************************/
Int32 OSA_spinlockCreate(OSA_SpinlockType    spinType, 
                         OSA_SpinlockHandle *phSpinlock);

/*******************************************************************************
* ������  : OSA_spinLock
* ��  ��  : ��������������
*           �ýӿڿ������ж������ĵ���
*
* ��  ��  : - hSpinlock: ���������
*
* ��  ��  : �ޡ�
* ����ֵ  :  OSA_SOK:    �ɹ�
*            OSA_EFAIL:  ʧ��
*******************************************************************************/
Int32 OSA_spinLock(OSA_SpinlockHandle hSpinlock);

/*******************************************************************************
* ������  : OSA_spinUnlock
* ��  ��  : ��������������
*           �ýӿڿ������ж������ĵ���
*
* ��  ��  : - hSpinlock: ���������
*           - type:      ��������,���嶨��μ�OSA_KSpinlockType
*
* ��  ��  : �ޡ�
* ����ֵ  :  OSA_SOK:    �ɹ�
*            OSA_EFAIL:  ʧ��
*******************************************************************************/
Int32 OSA_spinUnlock(OSA_SpinlockHandle hSpinlock);

/*******************************************************************************
* ������  : OSA_spinTrylock
* ��  ��  : ���������Լ�������
*           �ýӿڿ������ж������ĵ���
*
* ��  ��  : - hSpinlock:  ���������
*           - type:       ��������,���嶨��μ�OSA_KSpinlockType
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:      �����ɹ�
*           OSA_EFAIL:    �����ʧ��
*******************************************************************************/
Int32 OSA_spinTrylock(OSA_SpinlockHandle hSpinlock);

/*******************************************************************************
* ������  : OSA_spinlockDelete
* ��  ��  : �ú�����������һ��������
*           �ýӿڲ������ж������ĵ���
*
* ��  ��  : - hSpinlock:  Ҫ���ٵ����������
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:      �ɹ�
*           OSA_EFAIL:    ʧ��
*******************************************************************************/
Int32 OSA_spinlockDelete(OSA_SpinlockHandle hSpinlock);


#ifdef __cplusplus
}
#endif


#endif  /*  _OSA_SPINLOCK_H_  */

