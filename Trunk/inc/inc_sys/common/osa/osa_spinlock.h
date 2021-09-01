/*******************************************************************************
* osa_kspinlock.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: 实现OSA模块对外提供的自旋锁接口
*
*           接口调用流程如下:
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
*    Contents:  精简调用方式，使用回调函数实现。名字中去掉代表kernel的'k'，因为
*               在其他核中也要实现。
*******************************************************************************/


#ifndef _OSA_SPINLOCK_H_
#define _OSA_SPINLOCK_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
/* 调用OSA_kSpinlockCreate返回的自旋锁句柄类型,上层模块不需要关心其中的具体内容,*/
/* 只需要在各自旋锁操作接口中传入该句柄即可*/
typedef Handle OSA_SpinlockHandle;


/* 自旋锁加解锁的类型。SYSBIOS中支持OSA_SPINLOCK_IRQ。*/
typedef enum
{
    OSA_SPINLOCK_NORMAL  = 0,  /* 对应spin_lock、spin_unlock */
    OSA_SPINLOCK_BH,           /* 对应spin_lock_bh、spin_unlock_bh */
    OSA_SPINLOCK_IRQ,          /* 对应spin_lock_irq、spin_unlock_irq */
    OSA_SPINLOCK_IRQSAVE       /* 对应spin_lock_irqsave、spin_unlock_irqrestore*/
} OSA_SpinlockType;

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_spinlockCreate
* 描  述  : 该函数负责创建一个自旋锁
*           该接口不能在中断上下文调用
*
* 输  入  : - spinType  : 加锁类型,具体定义参见OSA_SpinlockType。
*
* 输  出  : - phSpinlock: 自旋锁 句柄指针,当创建成功时输出自旋锁句柄
* 返回值  : OSA_SOK:      创建成功
*           OSA_EFAIL:    创建失败
*******************************************************************************/
Int32 OSA_spinlockCreate(OSA_SpinlockType    spinType, 
                         OSA_SpinlockHandle *phSpinlock);

/*******************************************************************************
* 函数名  : OSA_spinLock
* 描  述  : 自旋锁加锁操作
*           该接口可以在中断上下文调用
*
* 输  入  : - hSpinlock: 自旋锁句柄
*
* 输  出  : 无。
* 返回值  :  OSA_SOK:    成功
*            OSA_EFAIL:  失败
*******************************************************************************/
Int32 OSA_spinLock(OSA_SpinlockHandle hSpinlock);

/*******************************************************************************
* 函数名  : OSA_spinUnlock
* 描  述  : 自旋锁解锁操作
*           该接口可以在中断上下文调用
*
* 输  入  : - hSpinlock: 自旋锁句柄
*           - type:      加锁类型,具体定义参见OSA_KSpinlockType
*
* 输  出  : 无。
* 返回值  :  OSA_SOK:    成功
*            OSA_EFAIL:  失败
*******************************************************************************/
Int32 OSA_spinUnlock(OSA_SpinlockHandle hSpinlock);

/*******************************************************************************
* 函数名  : OSA_spinTrylock
* 描  述  : 自旋锁尝试加锁操作
*           该接口可以在中断上下文调用
*
* 输  入  : - hSpinlock:  自旋锁句柄
*           - type:       加锁类型,具体定义参见OSA_KSpinlockType
*
* 输  出  : 无。
* 返回值  : OSA_SOK:      加锁成功
*           OSA_EFAIL:    获得锁失败
*******************************************************************************/
Int32 OSA_spinTrylock(OSA_SpinlockHandle hSpinlock);

/*******************************************************************************
* 函数名  : OSA_spinlockDelete
* 描  述  : 该函数负责销毁一个自旋锁
*           该接口不能在中断上下文调用
*
* 输  入  : - hSpinlock:  要销毁的自旋锁句柄
*
* 输  出  : 无。
* 返回值  : OSA_SOK:      成功
*           OSA_EFAIL:    失败
*******************************************************************************/
Int32 OSA_spinlockDelete(OSA_SpinlockHandle hSpinlock);


#ifdef __cplusplus
}
#endif


#endif  /*  _OSA_SPINLOCK_H_  */


