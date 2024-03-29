/*******************************************************************************
* osa_pm.h
* Desc: 定义linux内核态OSA模块对外提供电源管理中心接口
*
*           接口调用流程如下:
*           ==========================
*                   |
*           OSA_pmRegister
*                   |
*           系统suspend时依次调用OSA_pm_register中注册的suspend方法
*                   |
*           系统resume时依次调用OSA_pm_register中注册的resume方法
*                   |
*           OSA_pmUnregister
*           ===========================
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

#ifndef _OSA_PM_H_
#define _OSA_PM_H_

#ifdef __KERNEL__    /* 仅Linux 内核态支持*/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define OSA_PM_DEV_NAME_LEN 32

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
#if 0
/**
 * struct dev_pm_ops - device PM callbacks
 *
 * Several device power state transitions are externally visible, affecting
 * the state of pending I/O queues and (for drivers that touch hardware)
 * interrupts, wakeups, DMA, and other hardware state.  There may also be
 * internal transitions to various low-power modes which are transparent
 * to the rest of the driver stack (such as a driver that's ON gating off
 * clocks which are not in active use).
 *
 * The externally visible transitions are handled with the help of callbacks
 * included in this structure in such a way that two levels of callbacks are
 * involved.  First, the PM core executes callbacks provided by PM domains,
 * device types, classes and bus types.  They are the subsystem-level callbacks
 * supposed to execute callbacks provided by device drivers, although they may
 * choose not to do that.  If the driver callbacks are executed, they have to
 * collaborate with the subsystem-level callbacks to achieve the goals
 * appropriate for the given system transition, given transition phase and the
 * subsystem the device belongs to.
 *
 * @prepare: The principal role of this callback is to prevent new children of
 *	the device from being registered after it has returned (the driver's
 *	subsystem and generally the rest of the kernel is supposed to prevent
 *	new calls to the probe method from being made too once @prepare() has
 *	succeeded).  If @prepare() detects a situation it cannot handle (e.g.
 *	registration of a child already in progress), it may return -EAGAIN, so
 *	that the PM core can execute it once again (e.g. after a new child has
 *	been registered) to recover from the race condition.
 *	This method is executed for all kinds of suspend transitions and is
 *	followed by one of the suspend callbacks: @suspend(), @freeze(), or
 *	@poweroff().  The PM core executes subsystem-level @prepare() for all
 *	devices before starting to invoke suspend callbacks for any of them, so
 *	generally devices may be assumed to be functional or to respond to
 *	runtime resume requests while @prepare() is being executed.  However,
 *	device drivers may NOT assume anything about the availability of user
 *	space at that time and it is NOT valid to request firmware from within
 *	@prepare() (it's too late to do that).  It also is NOT valid to allocate
 *	substantial amounts of memory from @prepare() in the GFP_KERNEL mode.
 *	[To work around these limitations, drivers may register suspend and
 *	hibernation notifiers to be executed before the freezing of tasks.]
 *
 * @complete: Undo the changes made by @prepare().  This method is executed for
 *	all kinds of resume transitions, following one of the resume callbacks:
 *	@resume(), @thaw(), @restore().  Also called if the state transition
 *	fails before the driver's suspend callback: @suspend(), @freeze() or
 *	@poweroff(), can be executed (e.g. if the suspend callback fails for one
 *	of the other devices that the PM core has unsuccessfully attempted to
 *	suspend earlier).
 *	The PM core executes subsystem-level @complete() after it has executed
 *	the appropriate resume callbacks for all devices.
 *
 * @suspend: Executed before putting the system into a sleep state in which the
 *	contents of main memory are preserved.  The exact action to perform
 *	depends on the device's subsystem (PM domain, device type, class or bus
 *	type), but generally the device must be quiescent after subsystem-level
 *	@suspend() has returned, so that it doesn't do any I/O or DMA.
 *	Subsystem-level @suspend() is executed for all devices after invoking
 *	subsystem-level @prepare() for all of them.
 *
 * @suspend_late: Continue operations started by @suspend().  For a number of
 *	devices @suspend_late() may point to the same callback routine as the
 *	runtime suspend callback.
 *
 * @resume: Executed after waking the system up from a sleep state in which the
 *	contents of main memory were preserved.  The exact action to perform
 *	depends on the device's subsystem, but generally the driver is expected
 *	to start working again, responding to hardware events and software
 *	requests (the device itself may be left in a low-power state, waiting
 *	for a runtime resume to occur).  The state of the device at the time its
 *	driver's @resume() callback is run depends on the platform and subsystem
 *	the device belongs to.  On most platforms, there are no restrictions on
 *	availability of resources like clocks during @resume().
 *	Subsystem-level @resume() is executed for all devices after invoking
 *	subsystem-level @resume_noirq() for all of them.
 *
 * @resume_early: Prepare to execute @resume().  For a number of devices
 *	@resume_early() may point to the same callback routine as the runtime
 *	resume callback.
 *
 * @freeze: Hibernation-specific, executed before creating a hibernation image.
 *	Analogous to @suspend(), but it should not enable the device to signal
 *	wakeup events or change its power state.  The majority of subsystems
 *	(with the notable exception of the PCI bus type) expect the driver-level
 *	@freeze() to save the device settings in memory to be used by @restore()
 *	during the subsequent resume from hibernation.
 *	Subsystem-level @freeze() is executed for all devices after invoking
 *	subsystem-level @prepare() for all of them.
 *
 * @freeze_late: Continue operations started by @freeze().  Analogous to
 *	@suspend_late(), but it should not enable the device to signal wakeup
 *	events or change its power state.
 *
 * @thaw: Hibernation-specific, executed after creating a hibernation image OR
 *	if the creation of an image has failed.  Also executed after a failing
 *	attempt to restore the contents of main memory from such an image.
 *	Undo the changes made by the preceding @freeze(), so the device can be
 *	operated in the same way as immediately before the call to @freeze().
 *	Subsystem-level @thaw() is executed for all devices after invoking
 *	subsystem-level @thaw_noirq() for all of them.  It also may be executed
 *	directly after @freeze() in case of a transition error.
 *
 * @thaw_early: Prepare to execute @thaw().  Undo the changes made by the
 *	preceding @freeze_late().
 *
 * @poweroff: Hibernation-specific, executed after saving a hibernation image.
 *	Analogous to @suspend(), but it need not save the device's settings in
 *	memory.
 *	Subsystem-level @poweroff() is executed for all devices after invoking
 *	subsystem-level @prepare() for all of them.
 *
 * @poweroff_late: Continue operations started by @poweroff().  Analogous to
 *	@suspend_late(), but it need not save the device's settings in memory.
 *
 * @restore: Hibernation-specific, executed after restoring the contents of main
 *	memory from a hibernation image, analogous to @resume().
 *
 * @restore_early: Prepare to execute @restore(), analogous to @resume_early().
 *
 * @suspend_noirq: Complete the actions started by @suspend().  Carry out any
 *	additional operations required for suspending the device that might be
 *	racing with its driver's interrupt handler, which is guaranteed not to
 *	run while @suspend_noirq() is being executed.
 *	It generally is expected that the device will be in a low-power state
 *	(appropriate for the target system sleep state) after subsystem-level
 *	@suspend_noirq() has returned successfully.  If the device can generate
 *	system wakeup signals and is enabled to wake up the system, it should be
 *	configured to do so at that time.  However, depending on the platform
 *	and device's subsystem, @suspend() or @suspend_late() may be allowed to
 *	put the device into the low-power state and configure it to generate
 *	wakeup signals, in which case it generally is not necessary to define
 *	@suspend_noirq().
 *
 * @resume_noirq: Prepare for the execution of @resume() by carrying out any
 *	operations required for resuming the device that might be racing with
 *	its driver's interrupt handler, which is guaranteed not to run while
 *	@resume_noirq() is being executed.
 *
 * @freeze_noirq: Complete the actions started by @freeze().  Carry out any
 *	additional operations required for freezing the device that might be
 *	racing with its driver's interrupt handler, which is guaranteed not to
 *	run while @freeze_noirq() is being executed.
 *	The power state of the device should not be changed by either @freeze(),
 *	or @freeze_late(), or @freeze_noirq() and it should not be configured to
 *	signal system wakeup by any of these callbacks.
 *
 * @thaw_noirq: Prepare for the execution of @thaw() by carrying out any
 *	operations required for thawing the device that might be racing with its
 *	driver's interrupt handler, which is guaranteed not to run while
 *	@thaw_noirq() is being executed.
 *
 * @poweroff_noirq: Complete the actions started by @poweroff().  Analogous to
 *	@suspend_noirq(), but it need not save the device's settings in memory.
 *
 * @restore_noirq: Prepare for the execution of @restore() by carrying out any
 *	operations required for thawing the device that might be racing with its
 *	driver's interrupt handler, which is guaranteed not to run while
 *	@restore_noirq() is being executed.  Analogous to @resume_noirq().
 *
 * All of the above callbacks, except for @complete(), return error codes.
 * However, the error codes returned by the resume operations, @resume(),
 * @thaw(), @restore(), @resume_noirq(), @thaw_noirq(), and @restore_noirq(), do
 * not cause the PM core to abort the resume transition during which they are
 * returned.  The error codes returned in those cases are only printed by the PM
 * core to the system logs for debugging purposes.  Still, it is recommended
 * that drivers only return error codes from their resume methods in case of an
 * unrecoverable failure (i.e. when the device being handled refuses to resume
 * and becomes unusable) to allow us to modify the PM core in the future, so
 * that it can avoid attempting to handle devices that failed to resume and
 * their children.
 *
 * It is allowed to unregister devices while the above callbacks are being
 * executed.  However, a callback routine must NOT try to unregister the device
 * it was called for, although it may unregister children of that device (for
 * example, if it detects that a child was unplugged while the system was
 * asleep).
 *
 * Refer to Documentation/power/devices.txt for more information about the role
 * of the above callbacks in the system suspend process.
 *
 * There also are callbacks related to runtime power management of devices.
 * Again, these callbacks are executed by the PM core only for subsystems
 * (PM domains, device types, classes and bus types) and the subsystem-level
 * callbacks are supposed to invoke the driver callbacks.  Moreover, the exact
 * actions to be performed by a device driver's callbacks generally depend on
 * the platform and subsystem the device belongs to.
 *
 * @runtime_suspend: Prepare the device for a condition in which it won't be
 *	able to communicate with the CPU(s) and RAM due to power management.
 *	This need not mean that the device should be put into a low-power state.
 *	For example, if the device is behind a link which is about to be turned
 *	off, the device may remain at full power.  If the device does go to low
 *	power and is capable of generating runtime wakeup events, remote wakeup
 *	(i.e., a hardware mechanism allowing the device to request a change of
 *	its power state via an interrupt) should be enabled for it.
 *
 * @runtime_resume: Put the device into the fully active state in response to a
 *	wakeup event generated by hardware or at the request of software.  If
 *	necessary, put the device into the full-power state and restore its
 *	registers, so that it is fully operational.
 *
 * @runtime_idle: Device appears to be inactive and it might be put into a
 *	low-power state if all of the necessary conditions are satisfied.  Check
 *	these conditions and handle the device as appropriate, possibly queueing
 *	a suspend request for it.  The return value is ignored by the PM core.
 *
 * Refer to Documentation/power/runtime_pm.txt for more information about the
 * role of the above callbacks in device runtime power management.
 *
 */

struct dev_pm_ops {
	int (*prepare)(struct device *dev);
	void (*complete)(struct device *dev);
	int (*suspend)(struct device *dev);
	int (*resume)(struct device *dev);
	int (*freeze)(struct device *dev);
	int (*thaw)(struct device *dev);
	int (*poweroff)(struct device *dev);
	int (*restore)(struct device *dev);
	int (*suspend_late)(struct device *dev);
	int (*resume_early)(struct device *dev);
	int (*freeze_late)(struct device *dev);
	int (*thaw_early)(struct device *dev);
	int (*poweroff_late)(struct device *dev);
	int (*restore_early)(struct device *dev);
	int (*suspend_noirq)(struct device *dev);
	int (*resume_noirq)(struct device *dev);
	int (*freeze_noirq)(struct device *dev);
	int (*thaw_noirq)(struct device *dev);
	int (*poweroff_noirq)(struct device *dev);
	int (*restore_noirq)(struct device *dev);
	int (*runtime_suspend)(struct device *dev);
	int (*runtime_resume)(struct device *dev);
	int (*runtime_idle)(struct device *dev);
};
#endif

typedef struct
{
    /*
     * @prepare: The principal role of this callback is to prevent new children of
     *	the device from being registered after it has returned (the driver's
     *	subsystem and generally the rest of the kernel is supposed to prevent
     *	new calls to the probe method from being made too once @prepare() has
     *	succeeded).  If @prepare() detects a situation it cannot handle (e.g.
     *	registration of a child already in progress), it may return -EAGAIN, so
     *	that the PM core can execute it once again (e.g. after a new child has
     *	been registered) to recover from the race condition.
     *	This method is executed for all kinds of suspend transitions and is
     *	followed by one of the suspend callbacks: @suspend(), @freeze(), or
     *	@poweroff().  The PM core executes subsystem-level @prepare() for all
     *	devices before starting to invoke suspend callbacks for any of them, so
     *	generally devices may be assumed to be functional or to respond to
     *	runtime resume requests while @prepare() is being executed.  However,
     *	device drivers may NOT assume anything about the availability of user
     *	space at that time and it is NOT valid to request firmware from within
     *	@prepare() (it's too late to do that).  It also is NOT valid to allocate
     *	substantial amounts of memory from @prepare() in the GFP_KERNEL mode.
     *	[To work around these limitations, drivers may register suspend and
     *	hibernation notifiers to be executed before the freezing of tasks.]
     */
    Int32 (*prepare)(void);

    /*
     * @complete: Undo the changes made by @prepare().  This method is executed for
     *	all kinds of resume transitions, following one of the resume callbacks:
     *	@resume(), @thaw(), @restore().  Also called if the state transition
     *	fails before the driver's suspend callback: @suspend(), @freeze() or
     *	@poweroff(), can be executed (e.g. if the suspend callback fails for one
     *	of the other devices that the PM core has unsuccessfully attempted to
     *	suspend earlier).
     *	The PM core executes subsystem-level @complete() after it has executed
     *	the appropriate resume callbacks for all devices.
     */
    void  (*complete)(void);

    /*
     * @suspend: Executed before putting the system into a sleep state in which the
     *	contents of main memory are preserved.  The exact action to perform
     *	depends on the device's subsystem (PM domain, device type, class or bus
     *	type), but generally the device must be quiescent after subsystem-level
     *	@suspend() has returned, so that it doesn't do any I/O or DMA.
     *	Subsystem-level @suspend() is executed for all devices after invoking
     *	subsystem-level @prepare() for all of them.
     *
     */
    Int32 (*suspend)(void);

    /*
     * @resume: Executed after waking the system up from a sleep state in which the
     *	contents of main memory were preserved.  The exact action to perform
     *	depends on the device's subsystem, but generally the driver is expected
     *	to start working again, responding to hardware events and software
     *	requests (the device itself may be left in a low-power state, waiting
     *	for a runtime resume to occur).  The state of the device at the time its
     *	driver's @resume() callback is run depends on the platform and subsystem
     *	the device belongs to.  On most platforms, there are no restrictions on
     *	availability of resources like clocks during @resume().
     *	Subsystem-level @resume() is executed for all devices after invoking
     *	subsystem-level @resume_noirq() for all of them.
     */
    Int32 (*resume)(void);

    /*
     * @freeze: Hibernation-specific, executed before creating a hibernation image.
     *	Analogous to @suspend(), but it should not enable the device to signal
     *	wakeup events or change its power state.  The majority of subsystems
     *	(with the notable exception of the PCI bus type) expect the driver-level
     *	@freeze() to save the device settings in memory to be used by @restore()
     *	during the subsequent resume from hibernation.
     *	Subsystem-level @freeze() is executed for all devices after invoking
     *	subsystem-level @prepare() for all of them.
     */
    Int32 (*freeze)(void);

    /*
     * @thaw: Hibernation-specific, executed after creating a hibernation image OR
     *	if the creation of an image has failed.  Also executed after a failing
     *	attempt to restore the contents of main memory from such an image.
     *	Undo the changes made by the preceding @freeze(), so the device can be
     *	operated in the same way as immediately before the call to @freeze().
     *	Subsystem-level @thaw() is executed for all devices after invoking
     *	subsystem-level @thaw_noirq() for all of them.  It also may be executed
     *	directly after @freeze() in case of a transition error.
     */
    Int32 (*thaw)(void);

    /*
     * @poweroff: Hibernation-specific, executed after saving a hibernation image.
     *	Analogous to @suspend(), but it need not save the device's settings in
     *	memory.
     *	Subsystem-level @poweroff() is executed for all devices after invoking
     *	subsystem-level @prepare() for all of them.
     */
    Int32 (*poweroff)(void);

    /*
     * @restore: Hibernation-specific, executed after restoring the contents of main
     *	memory from a hibernation image, analogous to @resume().
     */
    Int32 (*restore)(void);

    /*
     * @suspend_late: Continue operations started by @suspend().  For a number of
     *	devices @suspend_late() may point to the same callback routine as the
     *	runtime suspend callback.
     */
    Int32 (*suspend_late)(void);

    /*
     * @resume_early: Prepare to execute @resume().  For a number of devices
     *	@resume_early() may point to the same callback routine as the runtime
     *	resume callback.
     */
    Int32 (*resume_early)(void);

    /*
     * @freeze_late: Continue operations started by @freeze().  Analogous to
     *	@suspend_late(), but it should not enable the device to signal wakeup
     *	events or change its power state.
     */
    Int32 (*freeze_late)(void);

    /*
     * @thaw_early: Prepare to execute @thaw().  Undo the changes made by the
     *	preceding @freeze_late().
     */
    Int32 (*thaw_early)(void);

    /*
     * @poweroff_late: Continue operations started by @poweroff().  Analogous to
     *	@suspend_late(), but it need not save the device's settings in memory.
     */
    Int32 (*poweroff_late)(void);

    /*
     * @restore_early: Prepare to execute @restore(), analogous to @resume_early().
     */
    Int32 (*restore_early)(void);

    /*
     * @suspend_noirq: Complete the actions started by @suspend().  Carry out any
     *	additional operations required for suspending the device that might be
     *	racing with its driver's interrupt handler, which is guaranteed not to
     *	run while @suspend_noirq() is being executed.
     *	It generally is expected that the device will be in a low-power state
     *	(appropriate for the target system sleep state) after subsystem-level
     *	@suspend_noirq() has returned successfully.  If the device can generate
     *	system wakeup signals and is enabled to wake up the system, it should be
     *	configured to do so at that time.  However, depending on the platform
     *	and device's subsystem, @suspend() or @suspend_late() may be allowed to
     *	put the device into the low-power state and configure it to generate
     *	wakeup signals, in which case it generally is not necessary to define
     *	@suspend_noirq().
     */
    Int32 (*suspend_noirq)(void);

    /*
     * @resume_noirq: Prepare for the execution of @resume() by carrying out any
     *	operations required for resuming the device that might be racing with
     *	its driver's interrupt handler, which is guaranteed not to run while
     *	@resume_noirq() is being executed.
     */
    Int32 (*resume_noirq)(void);

    /*
     * @freeze_noirq: Complete the actions started by @freeze().  Carry out any
     *	additional operations required for freezing the device that might be
     *	racing with its driver's interrupt handler, which is guaranteed not to
     *	run while @freeze_noirq() is being executed.
     *	The power state of the device should not be changed by either @freeze(),
     *	or @freeze_late(), or @freeze_noirq() and it should not be configured to
     *	signal system wakeup by any of these callbacks.
     */
    Int32 (*freeze_noirq)(void);

    /*
     * @thaw_noirq: Prepare for the execution of @thaw() by carrying out any
     *	operations required for thawing the device that might be racing with its
     *	driver's interrupt handler, which is guaranteed not to run while
     *	@thaw_noirq() is being executed.
     */
    Int32 (*thaw_noirq)(void);

    /*
     * @poweroff_noirq: Complete the actions started by @poweroff().  Analogous to
     *  @suspend_noirq(), but it need not save the device's settings in memory.
     */
    Int32 (*poweroff_noirq)(void);

    /*
     * @restore_noirq: Prepare for the execution of @restore() by carrying out any
     *	operations required for thawing the device that might be racing with its
     *	driver's interrupt handler, which is guaranteed not to run while
     *	@restore_noirq() is being executed.  Analogous to @resume_noirq().
     */
    Int32 (*restore_noirq)(void);

    /*
     * @runtime_suspend: Prepare the device for a condition in which it won't be
     *	able to communicate with the CPU(s) and RAM due to power management.
     *	This need not mean that the device should be put into a low-power state.
     *	For example, if the device is behind a link which is about to be turned
     *	off, the device may remain at full power.  If the device does go to low
     *	power and is capable of generating runtime wakeup events, remote wakeup
     *	(i.e., a hardware mechanism allowing the device to request a change of
     *	its power state via an interrupt) should be enabled for it.
     */
    Int32 (*runtime_suspend)(void);

    /*
     * @runtime_resume: Put the device into the fully active state in response to a
     *	wakeup event generated by hardware or at the request of software.  If
     *	necessary, put the device into the full-power state and restore its
     *	registers, so that it is fully operational.
     */
    Int32 (*runtime_resume)(void);

    /*
     * @runtime_idle: Device appears to be inactive and it might be put into a
     *	low-power state if all of the necessary conditions are satisfied.  Check
     *	these conditions and handle the device as appropriate, possibly queueing
     *	a suspend request for it.  The return value is ignored by the PM core.
     */
    Int32 (*runtime_idle)(void);

    Int32 *reserved[9];
}OSA_PM_OPS;



/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_pmRegister
* 描  述  : 该函数负责注册一个设备的电源管理方法到OSA电源管理中心
*           该函数仅能在内核态使用
*           先注册的设备后休眠, 后注册的设备先休眠
*           先注册的设备先唤醒, 后注册的设备后唤醒
*
* 输  入  : pName : 电源管理设备的名称
*           pmOps : 电源管理方法集
*
* 输  出  : pHandle: 设备的电源管理方法句柄指针, 当创建成功时输出字符设备句柄
* 返回值  : OSA_SOK:   创建成功
*           OSA_EFAIL: 创建失败
*******************************************************************************/
Int32 OSA_pmRegister(Char *pName, OSA_PM_OPS *pmOps);

/*******************************************************************************
* 函数名  : OSA_pmUnregister
* 描  述  : 该函数负责销毁一个设备的电源管理方法到OSA电源管理中心
*           该函数仅能在内核态使用
*
* 输  入  : pName : 电源管理设备的名称
*
* 输  出  :
* 返回值  : OSA_SOK:   创建成功
*           OSA_EFAIL: 创建失败
*******************************************************************************/
Int32 OSA_pmUnregister(Char *pName);

/*******************************************************************************
* 函数名  : OSA_pmInit
* 描  述  : 该函数负责初始化OSA的电源管理中心, OSA初始化时候调用一次
*           该函数仅能在内核态使用
*
* 输  入  : void
* 输  出  : void
* 返回值  : OSA_SOK:   创建成功
*           OSA_EFAIL: 创建失败
*******************************************************************************/
Int32 OSA_pmInit(void);

/*******************************************************************************
* 函数名  : OSA_pmExit
* 描  述  : 该函数负责去初始化OSA的电源管理中心, OSA去初始化时候调用一次
*           该函数仅能在内核态使用
*
* 输  入  : void
* 输  出  : void
* 返回值  : OSA_SOK:   销毁成功
*           OSA_EFAIL: 销毁失败
*******************************************************************************/
Int32 OSA_pmExit(void);

#ifdef __cplusplus
}
#endif

#endif

#endif  /*  _OSA_PM_H_  */
