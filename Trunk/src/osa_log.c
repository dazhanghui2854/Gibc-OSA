/*******************************************************************************
* osa_log.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-10-30 Create
*
* Desc: 实现OSA的日志接口
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include <osa.h>
#include <osa_priv.h>

#if defined(__RTEMS__)
extern void printk(const char *fmt, ...);
extern void vprintk(const char *fmt, va_list ap);
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#if defined(__KERNEL__)
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,4,24)/* DH5000 */
void vprintk(const Char *pFmt, va_list args)
{
    static char buffer[512];
    vsnprintf(buffer,512, pFmt, args);/* 多线程打印，可能导致打印内容混乱问题 */
    printk("%s", buffer);
}
#endif
#endif
/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            函数声明区                                      */
/* ========================================================================== */

static Int32 OSA_logWriteDefault(Uint32 level, Uint32 color, \
                                        const Char *pFmt, va_list args);

/* ========================================================================== */
/*                            全局变量定义区                                  */
/* ========================================================================== */

/*日志操作方法*/
/*lint -esym(459, gLogOps) */
static OSA_LogOps gLogOps =
{
    .OpWrite = OSA_logWriteDefault,
};

#ifdef OS_LINUX
static struct Osa_LogDev_s {
	int fd;
	int registered_mods;
} Thiz = {0};

static OSA_LIST_HEAD(OSA_LogManager_list);
static OSA_LogManager *OSA_LogManagerGetbyName(const Char *modName);
#endif
/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/*lint -esym(459, gColor) */

#if (!defined(___DSPBIOS___)) && (!defined(OS21)) && (!defined(__RTEMS__))
static const char *gColor[OSA_LOG_CLR_MAX] = {
    LOG_CLRSTR_NONE,
    LOG_CLRSTR_RED,
    LOG_CLRSTR_GREEN,
    LOG_CLRSTR_BLUE,
    LOG_CLRSTR_DARY_GRAY,
    LOG_CLRSTR_CYAN,
    LOG_CLRSTR_PURPLE,
    LOG_CLRSTR_BROWN,
    LOG_CLRSTR_YELLOW,
    LOG_CLRSTR_WHITE
};

static void  OSA_printColor(Uint32 color)
{
    const char *str;

    if (color >= sizeof(gColor)/sizeof(char *))
    {
        return;
    }

    str = gColor[color];

    /*lint -save -e592 */
    #if defined(__KERNEL__)
    printk(str);
    #else
    printf(str);
    #endif
    /*lint -restore */

    return;
}
#endif

/*******************************************************************************
* 函数名  : OSA_logRegister
* 描  述  : 该函数向OSA注册日志操作接口，通常由日志管理模块在初始化时调用。
*           在未向OSA注册之前，所有打印都使用OSA模块内部的函数。
*           当日志管理模块启动后，会调用本接口向OSA注册，
*           之后，所有的日志打印都通过日志管理模块的接口实现了。
*
* 输  入  : - pLogOps:  日志操作方法
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_logRegister(OSA_LogOps *pLogOps)
{
    if((pLogOps == NULL) || (pLogOps->OpWrite == NULL))
    {
        OSA_ERROR("Invalid params\n");
        return OSA_EFAIL;
    }

    gLogOps.OpWrite = pLogOps->OpWrite;

    return OSA_SOK;
}

/*******************************************************************************
* 函数名  : OSA_logUnRegister
* 描  述  : 该函数向OSA取消注册日志操作接口，通常由日志管理模块在退出时调用。
*           当日志管理模块取消注册后，所有的日志打印恢复到用OSA模块内部的函数
*
* 输  入  : 无。
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_logUnRegister(void)
{
    gLogOps.OpWrite = OSA_logWriteDefault;

    return OSA_SOK;
}

#if !defined(OS_LINUX)

/* 在日志模块没有注册之前的OSA内部打印函数 */
static Int32 OSA_logWriteDefault(Uint32 level, Uint32 color, \
                                        const Char *pFmt, va_list args)
{
    (void)level;

#if defined(__KERNEL__)
    /* 内核态默认打印，printk */
    OSA_printColor(color);
    vprintk(pFmt, args);
    OSA_printColor(OSA_LOG_CLR_NONE);

#elif defined(___DSPBIOS___)
    /* SYSBIOS 默认不打印 */
    (void)color;
    (void)pFmt;
    (void)args;
    
#elif defined(OS21)
    kernel_printf(pFmt, args);

#elif defined(__RTEMS__)
    vprintk(pFmt, args);

#else
    /* 用户态默认打印，printf */
    OSA_printColor(color);
    vprintf(pFmt, args);
    OSA_printColor(OSA_LOG_CLR_NONE);
#endif

    return OSA_SOK;
}
/*******************************************************************************
* 函数名  : OSA_logWrite
* 描  述  : 该函数提供统一打印接口
* 输  入  : - level:   打印等级，参见 OSA_LogLevel定义
*           - color:   打印颜色，参见 OSA_LogColor定义
*           - pFmt:    打印的格式字符串
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_logWrite(Uint32 level, Uint32 color, const Char *pFmt, ...)
{
    va_list args;
    Int32 ret = OSA_SOK;
    
    /*lint -save -e530 */
    va_start(args, pFmt);
    ret = gLogOps.OpWrite(level, color, pFmt, args);
    va_end(args);
    /*lint -restore */

    return ret;
}

Int32 OSA_logManagerRegister(OSA_LogManager *pLogManager)
{
	return OSA_SOK;
}

Int32 OSA_logManagerDeRegister(OSA_LogManager *pLogManager)
{
	return OSA_SOK;
}

Int32 OSA_LogManagerSetEntryPrintLevel(const Char *modName, Int32 level)
{
	return OSA_SOK;
}

Int32 OSA_LogManagerGetEntryPrintLevel(const Char *modName, Int32 *level)
{
	return OSA_SOK;
}

Int32 OSA_LogManagerSetEntryEnable(const Char *modName, OSA_LogManangerEnable status)
{
	return OSA_SOK;
}

Int32 OSA_LogManagerGetEntryPrintStatics(const Char *modName, Uint64 *printedBytes)
{
	return OSA_SOK;
}

Int32 OSA_LogManagerGetAllEntryInfoSize(void)
{
	return OSA_SOK;
}

Int32 OSA_LogManagerGetAllEntryInfo(OSA_LogManagerCommon *pentry, size_t size)
{
	return OSA_SOK;
}

#else //定义OS_LINUX宏，即对于Linux内核驱动/用户态
/*添加时间打印，每一秒通过定时器更新时间变量*/
#if defined(PRINT_TIME)

struct tm time_now = {0};	
static unsigned char time_flag=0;

void OSA_TimeUpdate(void)
{	 
	time_t current_time = time(NULL);
	localtime_r(&current_time, &time_now);	
}

void OSA_InitTime(void)
{
	struct itimerval value;
	value.it_value.tv_sec=1;
	value.it_value.tv_usec=0;
	value.it_interval=value.it_value;
	setitimer(ITIMER_REAL,&value,NULL);	
	OSA_TimeUpdate();
}

void OSA_TimeThread(void)
{
	int ret;
	sigset_t waitset;
	siginfo_t info;
	
	pthread_t ppid = pthread_self();
	pthread_detach(ppid);
	OSA_InitTime();
	sigemptyset(&waitset);
	sigaddset(&waitset, SIGALRM);

	while(1){	
		ret = sigwaitinfo(&waitset, &info);	
		if (ret != -1) {
			OSA_TimeUpdate();			
		} 		
	}	
	
}

#endif

/* 在日志模块没有注册之前的OSA内部打印函数 */
static Int32 OSA_logWriteDefault(Uint32 level, Uint32 color, \
                                        const Char *pFmt, va_list args)
{
	Int32 length;
    (void)level;
#if defined(__KERNEL__)
    /* 内核态默认打印，printk */
    OSA_printColor(color);
    length = vprintk(pFmt, args);
    OSA_printColor(OSA_LOG_CLR_NONE);

#elif defined(___DSPBIOS___)
    /* SYSBIOS 默认不打印 */
    (void)color;
    (void)pFmt;
    (void)args;

#elif defined(OS21)
    kernel_printf(pFmt, args);

#elif defined(__RTEMS__)
    vprintk(pFmt, args);

#else

    /* 用户态默认打印，printf */
    OSA_printColor(color);

#if defined(PRINT_TIME)
	fprintf(stdout, "[%02d:%02d:%02d]", time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
#endif

	length = vfprintf(stdout, pFmt, args);	
    OSA_printColor(OSA_LOG_CLR_NONE);
#endif

    return length;
}

/*******************************************************************************
* 函数名  : OSA_logWrite
* 描  述  : 该函数提供统一打印接口
* 输  入  : - level:   打印等级，参见 OSA_LogLevel定义
*           - color:   打印颜色，参见 OSA_LogColor定义
*           - pFmt:    打印的格式字符串
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_logWrite(Uint32 level, Uint32 color, const Char *pFmt, ...)
{
    va_list args;
	char *pbuf = NULL;
	Char modName[OSA_LOGENTITY_NAMELEN] = {0};
	
	Int32 modName_len = 0;
	Int32 length = 0;
    Int32 ret = OSA_SOK;

	OSA_LogManager *osa_entry = NULL;
	
	/* 提取出模块/库定义的 OSA_MODULE_NAME */
	pbuf = strchr(pFmt, '@');
	if (pbuf) {
		modName_len = (Int32)((unsigned long)pbuf - (unsigned long)&pFmt[1]);
		if (modName_len > OSA_LOGENTITY_NAMELEN) {
			goto default_print;
		}
		memcpy(modName, &pFmt[1], modName_len);
		modName[OSA_LOGENTITY_NAMELEN - 1] = '\0';
	}
	
	osa_entry = OSA_LogManagerGetbyName(modName);
	if (!osa_entry && modName_len) {
		/*
		 * 为了兼容源接口，对于未调用OSA_logManagerRegister显式注册管理结构，为其注册默认管理模块
		 * 部分打印接口没加OSA_MODULE_NAME，导致modName为NULL，对这些接口无需注册管理结构
		 */
		osa_entry = (OSA_LogManager *)OSA_memAlloc(sizeof(OSA_LogManager));
		if (!osa_entry) {
			OSA_ERROR("Register default entry for %s Failed\n", modName);
			goto default_print;
		}

		memset(osa_entry, 0, sizeof(OSA_LogManager));
		memcpy(osa_entry->info.osa_Name, modName, OSA_LOGENTITY_NAMELEN);
		osa_entry->info.osa_LogLevel = OSA_LOG_LV_INFO;
		osa_entry->info.osa_PrintEnable = OSA_LOGMANAGER_ENABLE;
		osa_entry->info.osa_Refcnt = OSA_LOGMANAGER_REGISTER_INNER;
		
		OSA_logManagerRegister(osa_entry);
#if defined(__KERNEL__)
	pr_info("OSA_logWrite entry %s\n", osa_entry->info.osa_Name);
#else
	printf("%s|%d entry %s osa_entry->osa_PrintBytes %llu \n", __func__, __LINE__, modName, osa_entry->osa_PrintBytes);
#endif
	}

default_print:
	/*
	 * 兼容不带OSA_MODULE_NAME的输出接口，此场景下osa_entry为NULL即默认输出
	 */
	if (!osa_entry || (osa_entry &&  osa_entry->info.osa_Refcnt > 0 &&
			osa_entry->info.osa_PrintEnable && osa_entry->info.osa_LogLevel >= level)) {
		/*lint -save -e530 */
		va_start(args, pFmt);
		length = gLogOps.OpWrite(level, color, pFmt, args);
		va_end(args);

		if (osa_entry && (OSA_LOGMANAGER_ENABLE == osa_entry->info.osa_Stat)) {
#if defined(__KERNEL__)
			atomic_add(length, &osa_entry->osa_PrintBytes);
#else
		OSA_mutexLock(osa_entry->hMutex);
		osa_entry->osa_PrintBytes += length;
		OSA_mutexUnlock(osa_entry->hMutex);
#endif
		}
		/*lint -restore */
	}

    return ret;
}

/******************************************************************************
* 函数名  :  OSA_logManagerRegister
* 描  述  : 该函数向OSA注册模块日志管理结构，通常在模块/库在初始化时调用。
*           对于未向OSA_LOGMANAGER注册之前，所有模块打印默认打开。
*           当模块启动调用本接口向OSA注册管理结构后，
*           所有的日志打印都通过日志管理模块的接口进行管理。
*           对于已注册过的模块，再次加载后，直接复用原结构，但为了
*           避免编译报警锁内存内存泄露，注册时重新分配锁内存
* 输  入  : OSA_LogManager:  待注册的日志管理结构对象
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_logManagerRegister(OSA_LogManager *pLogManager)
{
	OSA_LogManager *osa_entry;
	
	OSA_assertNotNull(pLogManager);
	
	osa_entry = OSA_LogManagerGetbyName(pLogManager->info.osa_Name);
	if (osa_entry) { //已被注册过直接复用
		osa_entry->info.osa_Stat = 0; //注册后默认使能该模块的日志输出
		osa_entry->info.osa_PrintEnable = (int8_t)OSA_LOGMANAGER_ENABLE; //注册后默认使能该模块的日志输出
		osa_entry->info.osa_LogLevel = (osa_entry->info.osa_LogLevel >= OSA_LOGMANAGER_CMD_MAX) ?
							OSA_LOG_LV_DBG : osa_entry->info.osa_LogLevel;

		/* 根据注销时的使用计数恢复引用计数 */
		osa_entry->info.osa_Refcnt = 1;
	} else {
		if (pLogManager->info.osa_Refcnt != OSA_LOGMANAGER_REGISTER_INNER) {
			osa_entry = (OSA_LogManager *)OSA_memAlloc(sizeof(OSA_LogManager));
			if (!osa_entry) {
				OSA_ERROR("Register default entry for %s Failed\n", pLogManager->info.osa_Name);
				return OSA_EFAIL;
			}
			memset(osa_entry, 0, sizeof(OSA_LogManager));
			memcpy(osa_entry, pLogManager, sizeof(OSA_LogManager));
		} else {
			osa_entry = pLogManager;
		}
		
		OSA_listHeadInit(&osa_entry->node);
		OSA_listAddTail(&osa_entry->node, &OSA_LogManager_list);
		
		osa_entry->info.osa_Stat = 0;   //注册后默认关闭该模块的日志统计
		osa_entry->info.osa_PrintEnable = OSA_LOGMANAGER_ENABLE; //注册后默认使能该模块的日志输出
		osa_entry->info.osa_LogLevel =
			(osa_entry->info.osa_LogLevel >= OSA_LOG_LV_MAX) ?
				OSA_LOG_LV_DBG : osa_entry->info.osa_LogLevel;
				
		osa_entry->info.osa_Refcnt = 1;
#if defined(PRINT_TIME)	
	if (time_flag == 0) {
		pthread_t timeid;
		sigset_t waitset;
		sigaddset(&waitset, SIGALRM);
		pthread_sigmask(SIG_SETMASK,&waitset,NULL);
		if(pthread_create(&timeid,NULL,(void*)OSA_TimeThread,NULL) !=0 ){
			OSA_ERROR("create OSA_TimeThread failed\n");
		} else {
			time_flag = 1;
		}
	}
	
#endif
#ifndef __KERNEL__
		osa_entry->osa_PrintBytes = 0;
		if (OSA_isFail(OSA_mutexCreate(OSA_MUTEX_NORMAL, &osa_entry->hMutex)))
			OSA_ERROR("create lock failed\n");
#endif
		Thiz.registered_mods++;
	}

	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_logManagerDeRegister
* 描  述  : 该函数向OSA注销模块日志管理结构，只会在模块注销时调用。
*           当模块被注销时，不对管理结构内存进行释放，只是对引用计数进行处理
*           重新加载模块后，可按同名匹配后复用原来分配的内存对象。
*           为了避免编译报警锁内存内存泄露，注销时释放锁内存空间
*
* 输  入  : OSA_LogManager:  待注册的日志管理结构对象
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_logManagerDeRegister(OSA_LogManager *pLogManager)
{
	OSA_assertNotNull(pLogManager);
	
	pLogManager->info.osa_Refcnt = -1; //只是简单设置引用计数
			
	pLogManager->info.osa_PrintEnable = 0; //注销后关闭该模块的日志输出

	//list_del(&pLogManager->node);
	return OSA_SOK;
}

static OSA_LogManager *OSA_LogManagerGetbyName(const Char *modName)
{
	Int32 len;
	OSA_LogManager *entry;
	OSA_ListHead  *pOsaList = NULL;
	OSA_ListHead  *pListTmp = NULL;

	/* 防止有模块未定义 OSA_MODULE_NAME */
	if (!modName)
		return NULL;

	len = strlen(modName) + 1;
	
	len = len > OSA_LOGENTITY_NAMELEN ? OSA_LOGENTITY_NAMELEN : len;
	OSA_listForEachSafe(pOsaList, pListTmp, &OSA_LogManager_list) {
		entry = OSA_listEntry(pOsaList, OSA_LogManager, node);
		if (0 == memcmp(entry->info.osa_Name, modName, len)) {
			return entry;
		}
	}
	
	return NULL;
}

#if defined(__KERNEL__)
static long OSA_LogManagerIoctl(struct file *filp, uint32_t cmd, unsigned long arg)
{
	Uint64 val;

	Int32 Request_len;
	Uint8 *Kbuf = NULL;
	Uint8 *pKbuf = NULL;
	OSA_LogManager *entry;

	OSA_ListHead *pOsaList = NULL;
	OSA_ListHead *pListTmp = NULL;
	OSA_LogManagerReq LogManagerReq;
	OSA_LogManagerReqInfo LogManagerReqInfo;
	OSA_LogManager *pLogmanager = NULL;
	
	OSA_clearSize(&LogManagerReq, sizeof(OSA_LogManagerReq));
	OSA_clearSize(&LogManagerReqInfo, sizeof(LogManagerReqInfo));

	if (_IOC_NR(cmd) == _IOC_NR(DH_LOGMANAGER_GET) ||
			_IOC_NR(cmd) == _IOC_NR(DH_LOGMANAGER_SET)) {
		if (copy_from_user(&LogManagerReq, (void __user *)arg, sizeof(OSA_LogManagerReq))) {
			OSA_ERROR("copy_from_user OSA_LogManagerReq failed.\n");
			return OSA_EFAIL;
		}

		LogManagerReq.osa_Name[OSA_LOGENTITY_NAMELEN - 1] = 0;
		pLogmanager = OSA_LogManagerGetbyName(LogManagerReq.osa_Name);
		if (NULL == pLogmanager) {
			OSA_ERROR("Not Found module %s registered. cmd %d %d\n", LogManagerReq.osa_Name,
					_IOC_NR(cmd), LogManagerReq.osa_cmd);
			return -EINVAL;
		}
	}
	
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(DH_LOGMANAGER_GET):
		switch (LogManagerReq.osa_cmd) {
		case OSA_LOGMANAGER_CMD_GETENABLE:
			LogManagerReq.osa_val[0] = pLogmanager->info.osa_PrintEnable;
			break;
		case OSA_LOGMANAGER_CMD_GETLEVEL:
			LogManagerReq.osa_val[0] = pLogmanager->info.osa_LogLevel;
			break;
		case OSA_LOGMANAGER_CMD_GETPRINTS:
			val = atomic_read(&pLogmanager->osa_PrintBytes);
			memcpy(LogManagerReq.osa_val, &val, sizeof(val));
			break;
		case OSA_LOGMANAGER_CMD_GETENTRY:
			LogManagerReq.osa_val[0] = pLogmanager->info.osa_LogLevel;
			LogManagerReq.osa_val[1] = pLogmanager->info.osa_PrintEnable;
			LogManagerReq.osa_val[2] = pLogmanager->info.osa_Stat;
			break;
		default:
			return OSA_SOK;
		}
		
		if (copy_to_user((void __user *)arg, &LogManagerReq, sizeof(OSA_LogManagerReq))) {
			OSA_ERROR("copy LogManagerReq to user faild\n");
			return -EFAULT;
		}
		
		break;
	case _IOC_NR(DH_LOGMANAGER_GETENTRYS):
		if (copy_to_user((void __user *)arg, &Thiz.registered_mods, sizeof(Thiz.registered_mods))) {
			OSA_ERROR("copy Registered_mods to user faild\n");
			return -EFAULT;
		}
		break;

	case _IOC_NR(DH_LOGMANAGER_GETINFO):
		if (copy_from_user(&LogManagerReqInfo, (void __user *)arg, sizeof(LogManagerReqInfo))) {
			OSA_ERROR("copy_from_user LogManagerReqInfo failed.\n");
			return OSA_EFAIL;
		}

		Request_len = Thiz.registered_mods * sizeof(OSA_LogManagerCommon);

		if (LogManagerReqInfo.size < Request_len) {
			OSA_ERROR("No Enough Memory Specified %u Request %d \n", LogManagerReqInfo.size, Request_len);
			return -ENOMEM;
		}
		
		Kbuf = (uint8_t *)OSA_memAlloc(Request_len);
		if (!Kbuf) {
			OSA_ERROR("No Enough Memory Request %d \n", Request_len);
			return -ENOMEM;
		}
		memset(Kbuf, 0 , Request_len);
		pKbuf = Kbuf;
		
		OSA_listForEachSafe(pOsaList, pListTmp, &OSA_LogManager_list) {
			entry = OSA_listEntry(pOsaList, OSA_LogManager, node);
			memcpy(pKbuf, &entry->info, sizeof(OSA_LogManagerCommon));
			pKbuf += sizeof(OSA_LogManagerCommon);
		}
		
		if (copy_to_user((void __user *)LogManagerReqInfo.buf, Kbuf, Request_len)) {
			if(OSA_SOK != OSA_memFree(Kbuf))
				OSA_ERROR("memory free failed\n");
			Kbuf = NULL;
			OSA_ERROR("copy Kmod Entrys Info to user faild\n");
			return -EFAULT;
		}
		if(OSA_SOK != OSA_memFree(Kbuf))
			OSA_ERROR("memory free failed\n");
		Kbuf = NULL;
		
		break;
	case _IOC_NR(DH_LOGMANAGER_SET):
		switch (LogManagerReq.osa_cmd) {
		case OSA_LOGMANAGER_CMD_SETENABLE:
			pLogmanager->info.osa_PrintEnable = LogManagerReq.osa_val[0];
			break;
		case OSA_LOGMANAGER_CMD_SETLEVEL:
			pLogmanager->info.osa_LogLevel = LogManagerReq.osa_val[0];
			break;
		case OSA_LOGMANAGER_CMD_SETSTAT:
			pLogmanager->info.osa_Stat = LogManagerReq.osa_val[0];
			break;
		case OSA_LOGMANAGER_CMD_SETENTRY:
			pLogmanager->info.osa_LogLevel = LogManagerReq.osa_val[0];
			pLogmanager->info.osa_PrintEnable = LogManagerReq.osa_val[1];
			pLogmanager->info.osa_Stat = LogManagerReq.osa_val[2];
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	
	return OSA_SOK;
}

static Int32 OSA_LogManagerOpen(struct inode * inode, struct file * file)
{
	return OSA_SOK;
}

static Int32 OSA_LogManagerClose(struct inode * inode, struct file * file)
{
	return OSA_SOK;
}

static struct file_operations OSA_LogManagerFops = {
   .owner      = THIS_MODULE,
   .unlocked_ioctl = OSA_LogManagerIoctl,
   .open       = OSA_LogManagerOpen,
   .release    = OSA_LogManagerClose
};

static struct miscdevice OSA_LogManagerDev = {
   .minor      = MISC_DYNAMIC_MINOR,
   .name       = DH_LOGMANAGE_NAME,
   .fops       = &OSA_LogManagerFops,
};

ssize_t OSA_LogManagerProcWrite(struct file *filp, const char __user *buf, size_t count, loff_t *off)
{
	Int32 val;
	char Buf[128] = {0};
	OSA_LogManager *pLogmanager = NULL;
	char modName[OSA_LOGENTITY_NAMELEN] = {0};
	
	if (copy_from_user((void *)Buf, buf, count))
		return -EFAULT;
		
	Buf[127] = 0;
	if(strstr(Buf, "setstatus")) {
		if (sscanf(Buf, "setstatus %s %d", modName, &val)) {
			pLogmanager = OSA_LogManagerGetbyName(modName);
			if (pLogmanager) {
				pr_info("Module %s Status %d set to %d \n", modName, pLogmanager->info.osa_PrintEnable, val);
				pLogmanager->info.osa_PrintEnable = val;
			} else {
				pr_info("Module %s Not register\n", modName);
			}
		}
	} else if(strstr(Buf, "setlevle")) {
		if (sscanf(Buf, "setlevle %s %d", modName, &val)) {
			pLogmanager = OSA_LogManagerGetbyName(modName);
			if (pLogmanager) {
				pr_info("Module %s Level %d set to %d \n", modName, pLogmanager->info.osa_LogLevel, val);
				pLogmanager->info.osa_LogLevel = val;
			} else {
				pr_info("Module %s Not register\n", modName);
			}
		}
	} else if(strstr(Buf, "list")) {
		OSA_LogManager *entry;
		OSA_ListHead  *pOsaList = NULL;
		OSA_ListHead  *pListTmp = NULL;

		pr_info("Entry Name        status    level    pstat    prints\n");
	
		OSA_listForEachSafe(pOsaList, pListTmp, &OSA_LogManager_list) {
			entry = OSA_listEntry(pOsaList, OSA_LogManager, node);
		
			pr_info("%s \t\t %d \t %d \t %d \t %u \n", entry->info.osa_Name, entry->info.osa_PrintEnable,
				entry->info.osa_LogLevel, entry->info.osa_Stat, atomic_read(&entry->osa_PrintBytes));
		}
	
		pr_info("\n");
	}
	
	return count;
}

static const struct file_operations OSA_LogManagerProcFops =
{
	.open		= NULL,
	.write		= OSA_LogManagerProcWrite,
	.read		= NULL,
	.llseek		= NULL,
	.release	= NULL,
};

Int32 OSA_LogManager_init(void)
{
	Int32 ret;
	ret = misc_register(&OSA_LogManagerDev);
	if (ret != 0) {
		OSA_ERROR("Register LogManager faild\n");
		return -1;
	}
	
	if (!proc_create(DH_LOGMANAGE_NAME, S_IRUGO, NULL, &OSA_LogManagerProcFops))
		return -1;
	
	return 0;
}

void OSA_LogManager_exit(void)
{
	OSA_LogManager *entry = NULL;
	OSA_ListHead  *pOsaList = NULL;
	OSA_ListHead  *pListTmp = NULL;
	
	misc_deregister(&OSA_LogManagerDev);
	remove_proc_entry(DH_LOGMANAGE_NAME, NULL);
	
	OSA_listForEachSafe(pOsaList, pListTmp, &OSA_LogManager_list) {
	entry = OSA_listEntry(pOsaList, OSA_LogManager, node);
		if (entry) {
			OSA_listDel(&entry->node);
			if(OSA_SOK != OSA_memFree(entry))
				OSA_ERROR("memory free failed\n");
		}
	}
}

EXPORT_SYMBOL(OSA_LogManager_init);
EXPORT_SYMBOL(OSA_LogManager_exit);
EXPORT_SYMBOL(OSA_logRegister);
EXPORT_SYMBOL(OSA_logUnRegister);
EXPORT_SYMBOL(OSA_logWrite);
EXPORT_SYMBOL(OSA_logManagerRegister);
EXPORT_SYMBOL(OSA_logManagerDeRegister);

#else   // Not defined __KERNEL__

static Int32 OSA_LogManagerSet(const Char *modName, OSA_LogManagerCmd cmd, void *pBuf, size_t len)
{
	Int32 ret;
	int8_t *buf = (int8_t *)pBuf;
	OSA_LogManagerReq LogManagerReq;
	
	OSA_LogManager *pLogmanager = NULL;
	
	OSA_assertNotNull(modName);
	OSA_assertNotNull(pBuf);

	memset(&LogManagerReq, 0, sizeof(OSA_LogManagerReq));
	
	pLogmanager = OSA_LogManagerGetbyName(modName);
	if (!pLogmanager) {
		LogManagerReq.osa_cmd = cmd;
		
		if (strlen((Int8 *)modName) >= OSA_LOGENTITY_NAMELEN || len > sizeof(LogManagerReq.osa_val)) {
			OSA_ERROR("Invalid Param %s len %zu \n", modName, len);
			return OSA_EFAIL;
		}
		
		memcpy(LogManagerReq.osa_Name, modName, sizeof(LogManagerReq.osa_Name));
		memcpy(LogManagerReq.osa_val, pBuf, len);

		ret = ioctl(Thiz.fd, DH_LOGMANAGER_SET, &LogManagerReq);
		if (ret < 0) {
			OSA_ERROR("%s cmd %d fail: %s \n", modName, cmd, strerror(errno));
			return OSA_EFAIL;
		}
	} else {
		switch (cmd) {
		case OSA_LOGMANAGER_CMD_SETENABLE:
			pLogmanager->info.osa_PrintEnable = *(uint8_t *)pBuf;
			break;
		case OSA_LOGMANAGER_CMD_SETLEVEL:
			pLogmanager->info.osa_LogLevel = *(uint8_t *)pBuf;
			
			break;
		case OSA_LOGMANAGER_CMD_SETSTAT:
			pLogmanager->info.osa_Stat = *(uint8_t *)pBuf;
			break;
		case OSA_LOGMANAGER_CMD_SETENTRY:
			pLogmanager->info.osa_LogLevel = buf[0];
			pLogmanager->info.osa_PrintEnable = buf[1];
			pLogmanager->info.osa_Stat = buf[2];
			break;
		default:
			break;
		}
	}

	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerSetEntryPrintLevel 
* 描  述  : 该函数向OSA管理的对应模块/库设置其打印等级 
*           无锁设计，调用者需要保证保证配置安全
*      
* 输  入  : modName,:    待操作模块名称
* 输  入  : level:   打印等级
* 输  出  : 
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerSetEntryPrintLevel(const Char *modName, Int32 level)
{
	Int32 ret;

	OSA_assertNotNull(modName);

	if (level < OSA_LOGMANAGER_CMD_SETENABLE ||
			level >= OSA_LOGMANAGER_CMD_MAX) {
		OSA_ERROR("%s PRAM level %d Invalid \n", modName, level);
		return OSA_EFAIL;
	}
	
	ret = OSA_LogManagerSet(modName, OSA_LOGMANAGER_CMD_SETLEVEL, (void *)&level, sizeof(Int32));
	if (ret != OSA_SOK) {
		OSA_ERROR("%s SETLEVLE To %d Failed\n", modName, level);
		return OSA_EFAIL;
	}

	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerSetEntryEnable 
* 描  述  : 该函数设置OSA管理的对应模块/库打印使能或关闭
*           无锁设计，调用者需要保证保证配置安全
*      
* 输  入  : modName,:    待操作模块名称
* 输  入  : status:   使能或关闭模块/库的打印输出
* 输  出  : 
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerSetEntryEnable(const Char *modName, OSA_LogManangerEnable status)
{
	Int32 ret;

	OSA_assertNotNull(modName);
	if (status != OSA_LOGMANAGER_ENABLE &&
			status != OSA_LOGMANAGER_DISABLE) {
		OSA_ERROR("%s PRAM status %d Invalid \n", modName, status);
		return OSA_EFAIL;
	}

	ret = OSA_LogManagerSet(modName, OSA_LOGMANAGER_CMD_SETENABLE, (void *)&status, sizeof(status));
	if (ret < 0) {
		OSA_ERROR("%s SETLEVLE To %d Failed\n", modName, status);
		return OSA_EFAIL;
	}

	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerSet_EntryInfo
* 描  述  : 该函数获取OSA所管理的单个模块/库信息
* 
* 
* 输  入  : pentry 填入需要获取信息模块/库的名称
* 输  出  : pentry 返回指定模块/库的信息,不包括该模块打印统计量
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerSetEntryInfo(OSA_LogManagerCommon *pentry)
{
	Int32 ret;
	OSA_assertNotNull(pentry);
	
	ret = OSA_LogManagerSet(pentry->osa_Name, OSA_LOGMANAGER_CMD_SETENTRY, (void *)&pentry->osa_LogLevel,
			sizeof(OSA_LogManagerCommon) - OSA_LOGENTITY_NAMELEN);
	if (ret < 0) {
		OSA_ERROR("%s SETENTRYINFO Failed\n", pentry->osa_Name);
		return OSA_EFAIL;
	}

	return OSA_SOK;
}

static Int32 OSA_LogManagerGet(const Char *modName, OSA_LogManagerCmd cmd, void *pBuf, size_t len)
{
	Int32 ret;
	Int8 *buf = (Int8 *)pBuf;
	OSA_LogManagerReq LogManagerReq;
	
	OSA_LogManager *pLogmanager = NULL;

	OSA_assertNotNull(modName);
	OSA_assertNotNull(pBuf);
	
	memset(&LogManagerReq, 0, sizeof(OSA_LogManagerReq));
	
	pLogmanager = OSA_LogManagerGetbyName(modName);
	if (!pLogmanager) {
		LogManagerReq.osa_cmd = cmd;
		
		if (strlen(modName) >= OSA_LOGENTITY_NAMELEN || len > sizeof(LogManagerReq.osa_val)) {
			OSA_ERROR("Invalid Param %s len %zu \n", modName, len);
			return OSA_EFAIL;
		}
		memcpy(LogManagerReq.osa_Name, modName, sizeof(LogManagerReq.osa_Name));

		ret = ioctl(Thiz.fd, DH_LOGMANAGER_GET, &LogManagerReq);
		if (ret < 0) {
			OSA_ERROR("%s cmd %d fail: %s \n", modName, cmd, strerror(errno));
			return OSA_EFAIL;
		}
		
		memcpy(pBuf, LogManagerReq.osa_val, len);
	} else {
		switch (cmd) {
		case OSA_LOGMANAGER_CMD_GETENABLE:
			*(uint8_t *)pBuf = pLogmanager->info.osa_PrintEnable;
			break;
		case OSA_LOGMANAGER_CMD_GETLEVEL:
			*(uint8_t *)pBuf = pLogmanager->info.osa_LogLevel;
			break;
		case OSA_LOGMANAGER_CMD_GETPRINTS:
			memcpy(pBuf, &pLogmanager->osa_PrintBytes, sizeof(pLogmanager->osa_PrintBytes));
			break;
		case OSA_LOGMANAGER_CMD_GETENTRY:
			buf[0] = pLogmanager->info.osa_LogLevel;
			buf[1] = pLogmanager->info.osa_PrintEnable;
			buf[2] = pLogmanager->info.osa_Stat;
			break;
		default:
			break;
		}
	}

	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerGetEntryPrintLevel 
* 描  述  : 该函数获取OSA管理的对应模块/库的打印等级 
*           无锁设计，调用者需要保证保证配置安全
*      
* 输  入  : modName,:    待操作模块名称
* 输  入  : level:   返回模块打印等级
* 输  出  : 
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerGetEntryPrintLevel(const Char *modName, Int32 *level)
{
	Int32 ret;
	OSA_assertNotNull(modName);

	ret = OSA_LogManagerGet(modName, OSA_LOGMANAGER_CMD_GETLEVEL, (void *)level, sizeof(Int32));
	if (ret != OSA_SOK) {
		OSA_ERROR("%s GETLEVLE Failed\n", modName);
		return OSA_EFAIL;
	}
	
	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerGetEntryPrintStatics
* 描  述  : 该函数获取对应模块/库相关打印统计量
*           用户态统计计数加锁保护，内核态使用原子变量 
*           当前主要可获取模块/库的打印统计计数
*      
* 输  入  : modName,:    待操作模块名称
* 输  出  : printedBytes:   返回模块/库打印计数
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerGetEntryPrintStatics(const Char *modName, Uint64 *printedBytes)
{
	Int32 ret;
	
	OSA_assertNotNull(modName);
	
	ret = OSA_LogManagerGet(modName, OSA_LOGMANAGER_CMD_GETPRINTS, (void *)printedBytes, sizeof(Uint64));
	if (ret < 0) {
		OSA_ERROR("%s GETLEVLE Failed\n", modName);
		return OSA_EFAIL;
	}
		
	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerGetEntryInfo
* 描  述  : 该函数获取OSA所管理的单个模块/库信息
* 
* 
* 输  入  : pentry 填入需要获取信息模块/库的名称
* 输  出  : pentry 返回指定模块/库的信息,不包括该模块打印统计量
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerGetEntryInfo(OSA_LogManagerCommon *pentry)
{
	Int32 ret;
	OSA_assertNotNull(pentry);
	
	ret = OSA_LogManagerGet(pentry->osa_Name, OSA_LOGMANAGER_CMD_GETENTRY, (void *)&pentry->osa_LogLevel,
			sizeof(OSA_LogManagerCommon) - OSA_LOGENTITY_NAMELEN);
	if (ret < 0) {
		OSA_ERROR("%s GETENTRYINFO Failed\n", pentry->osa_Name);
		return OSA_EFAIL;
	}
		
	return OSA_SOK;
}

static Int32 OSA_LogManagerGetKmodEntryCnts (void)
{
	Int32 ret;
	Int32 kmodcnt;

	ret = ioctl(Thiz.fd, DH_LOGMANAGER_GETENTRYS, &kmodcnt);
	if (ret < 0) {
		OSA_ERROR("GET KModEntrys Failed %s \n", strerror(errno));
		return OSA_EFAIL;
	}

	return kmodcnt;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerGetAllEntryInfoSize
* 描  述  : 该函数获取OSA所管理的所有模块/库管理结构占用内存空间
*           目的在于便于调用者获取管理结构信息时，分配足够内存空间
* 
* 输  入  : 无  
* 输  出  : 无
* 返回值  : OSA所管理所有模块/库核心结构信息占用空间
******************************************************************************/
Int32 OSA_LogManagerGetAllEntryInfoSize(void)
{
	int32_t kmodcnt;
	size_t request_len;

	kmodcnt = OSA_LogManagerGetKmodEntryCnts();
	if (kmodcnt < 0) {
		OSA_ERROR("Get KmodEntry Fail \n");
		return OSA_EFAIL;
	}

	request_len = (kmodcnt + Thiz.registered_mods) * sizeof(OSA_LogManagerCommon);

	return request_len;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerGetAllEntryInfo
* 描  述  : 该函数获取OSA所管理的所有模块/库当前所总输出打印量
* 
* 
* 输  入  : size 为buf的大小
* 输  出  : buf 用于返回OSA所管理模块/库的数据结构化信息，buf必须足够可容纳
		   : 否则将获取失败，返回获取信息所需要缓存大小
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
			> 0的值表示参数buf所提供的空间不能满足返回结构化信息所需空间，此时
			返回所需的空间
******************************************************************************/
Int32 OSA_LogManagerGetAllEntryInfo(OSA_LogManagerCommon *pentry, size_t size)
{
	Int32 ret;
	Int32 kmodcnt;	
	size_t request_len;
	char *pbuf = (char *)pentry;

	OSA_LogManager *entry;
	OSA_ListHead  *pOsaList = NULL;
	OSA_ListHead  *pListTmp = NULL;
	OSA_LogManagerReqInfo Info;

	OSA_assertNotNull(pbuf);

	kmodcnt = OSA_LogManagerGetKmodEntryCnts();
	if (kmodcnt < 0) {
		OSA_ERROR("Get KmodEntry Fail \n");
		return OSA_EFAIL;
	}

	request_len = (kmodcnt + Thiz.registered_mods) * sizeof(OSA_LogManagerCommon);
	if (request_len < size) {
		OSA_ERROR("No Enough Memory Rquest to %zu \n", request_len);
		return request_len;
	}

	Info.size = size;
	Info.buf = (uint8_t *)pbuf;

	ret = ioctl(Thiz.fd, DH_LOGMANAGER_GETINFO, &Info);
	if (ret < 0) {
		OSA_ERROR("GETINFO Failed %s \n", strerror(errno));
		return OSA_EFAIL;
	}
		
	pbuf += kmodcnt * sizeof(OSA_LogManagerCommon);

	OSA_listForEachSafe(pOsaList, pListTmp, &OSA_LogManager_list) {
		entry = OSA_listEntry(pOsaList, OSA_LogManager, node);
		memcpy(pbuf, &entry->info, sizeof(OSA_LogManagerCommon));
		pbuf += sizeof(OSA_LogManagerCommon);
	}

	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerDisableAllEntryStat
* 描  述  : 该函数关闭/打开OSA所管理的所有模块/库输出打印量统计功能
*           用户态打印统计功能基于锁的性能开销，增加该接口关闭统计功能
* 
* 输  入  :  无
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerDisableAllEntryStat(OSA_LogManangerEnable status)
{
	int ret;
	int i = 0;
	Int32 len;
	
	int8_t stat = status;
	
	OSA_LogManagerCommon *osa_entity = NULL;
	
	len = OSA_LogManagerGetAllEntryInfoSize();
	if (len < 0) {
		OSA_ERROR("Get_AllEntryInfoSize Failed \n");
		return -1;
	}	
	
	osa_entity = (OSA_LogManagerCommon *)OSA_memAlloc(len);
	if (!osa_entity) {
		OSA_ERROR("No Enough mem \n");
		return -1;
	}

	ret = OSA_LogManagerGetAllEntryInfo(osa_entity, len);
	if (ret != 0) {
		if(OSA_SOK != OSA_memFree(osa_entity))
			OSA_ERROR("memory free failed\n");
		osa_entity = NULL;
		OSA_ERROR("Get_AllEntryInfo Failed \n");
		return -1;
	}

	for (i = 0; i < (len / sizeof(OSA_LogManagerCommon)); i++) {
		ret = OSA_LogManagerSet(osa_entity[i].osa_Name, OSA_LOGMANAGER_CMD_SETSTAT, 
					(void *)&stat, sizeof(int8_t));
		if (ret < 0) {
			if(OSA_SOK != OSA_memFree(osa_entity))
				OSA_ERROR("memory free failed\n");
			/*
			  covertiy号： 323707
			  问题描述：osa_entity 显式NULL 被解引用
			  修改描述：osa_entity 被引用之后再置空
			  注意事项描述： 
			*/			
			OSA_ERROR("%s SETSTAT To %d Failed\n", osa_entity[i].osa_Name, status);
			osa_entity = NULL;
			return OSA_EFAIL;
		}
	}

	if(OSA_SOK != OSA_memFree(osa_entity))
		OSA_ERROR("memory free failed\n");
	osa_entity = NULL;

	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerUser_init
* 描  述  : 该函数为OSA管理库的初始化接口
* 
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerUserInit(void)
{
	Thiz.fd = open(DH_LOGMANGE_FILENMAE, O_RDWR);

	if (Thiz.fd < 0) {
		OSA_ERROR("Open %s Fail.. %s \n", DH_LOGMANGE_FILENMAE, strerror(errno));
		return OSA_EFAIL;
	}
	
	OSA_INFO("OSA USER init \n");
	return OSA_SOK;
}

/******************************************************************************
* 函数名  :  OSA_LogManagerUser_exit
* 描  述  : 该函数为OSA管理功能注销接口
*           该接口将所有用户应用库所有模块注销
* 
* 输  入  : 无
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*          OSA_EFAIL: 失败
******************************************************************************/
Int32 OSA_LogManagerUserExit(void)
{
	OSA_LogManager *entry;
	OSA_ListHead  *pOsaList = NULL;
	OSA_ListHead  *pListTmp = NULL;
	
	if (Thiz.fd > 0) {
		close(Thiz.fd);
		Thiz.fd = -1;
	}
	
	OSA_listForEachSafe(pOsaList, pListTmp, &OSA_LogManager_list) {
		entry = OSA_listEntry(pOsaList, OSA_LogManager, node);
		if (entry) {
			OSA_listDel(&entry->node);
			if (NULL != entry->hMutex) {
				OSA_mutexDelete(entry->hMutex);
				entry->hMutex = NULL;
			}

			if(OSA_SOK != OSA_memFree(entry))
				OSA_ERROR("memory free failed\n");
		}
	}
	
//	OSA_INFO("OSA USER exit \n");
	return OSA_SOK;
}

#endif
#endif
