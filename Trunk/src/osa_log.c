/*******************************************************************************
* osa_log.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-10-30 Create
*
* Desc: ʵ��OSA����־�ӿ�
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
#include <osa_priv.h>

#if defined(__RTEMS__)
extern void printk(const char *fmt, ...);
extern void vprintk(const char *fmt, va_list ap);
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
#if defined(__KERNEL__)
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,4,24)/* DH5000 */
void vprintk(const Char *pFmt, va_list args)
{
    static char buffer[512];
    vsnprintf(buffer,512, pFmt, args);/* ���̴߳�ӡ�����ܵ��´�ӡ���ݻ������� */
    printk("%s", buffer);
}
#endif
#endif
/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            ����������                                      */
/* ========================================================================== */

static Int32 OSA_logWriteDefault(Uint32 level, Uint32 color, \
                                        const Char *pFmt, va_list args);

/* ========================================================================== */
/*                            ȫ�ֱ���������                                  */
/* ========================================================================== */

/*��־��������*/
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
/*                          ����������                                        */
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
* ������  : OSA_logRegister
* ��  ��  : �ú�����OSAע����־�����ӿڣ�ͨ������־����ģ���ڳ�ʼ��ʱ���á�
*           ��δ��OSAע��֮ǰ�����д�ӡ��ʹ��OSAģ���ڲ��ĺ�����
*           ����־����ģ�������󣬻���ñ��ӿ���OSAע�ᣬ
*           ֮�����е���־��ӡ��ͨ����־����ģ��Ľӿ�ʵ���ˡ�
*
* ��  ��  : - pLogOps:  ��־��������
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
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
* ������  : OSA_logUnRegister
* ��  ��  : �ú�����OSAȡ��ע����־�����ӿڣ�ͨ������־����ģ�����˳�ʱ���á�
*           ����־����ģ��ȡ��ע������е���־��ӡ�ָ�����OSAģ���ڲ��ĺ���
*
* ��  ��  : �ޡ�
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_logUnRegister(void)
{
    gLogOps.OpWrite = OSA_logWriteDefault;

    return OSA_SOK;
}

#if !defined(OS_LINUX)

/* ����־ģ��û��ע��֮ǰ��OSA�ڲ���ӡ���� */
static Int32 OSA_logWriteDefault(Uint32 level, Uint32 color, \
                                        const Char *pFmt, va_list args)
{
    (void)level;

#if defined(__KERNEL__)
    /* �ں�̬Ĭ�ϴ�ӡ��printk */
    OSA_printColor(color);
    vprintk(pFmt, args);
    OSA_printColor(OSA_LOG_CLR_NONE);

#elif defined(___DSPBIOS___)
    /* SYSBIOS Ĭ�ϲ���ӡ */
    (void)color;
    (void)pFmt;
    (void)args;
    
#elif defined(OS21)
    kernel_printf(pFmt, args);

#elif defined(__RTEMS__)
    vprintk(pFmt, args);

#else
    /* �û�̬Ĭ�ϴ�ӡ��printf */
    OSA_printColor(color);
    vprintf(pFmt, args);
    OSA_printColor(OSA_LOG_CLR_NONE);
#endif

    return OSA_SOK;
}
/*******************************************************************************
* ������  : OSA_logWrite
* ��  ��  : �ú����ṩͳһ��ӡ�ӿ�
* ��  ��  : - level:   ��ӡ�ȼ����μ� OSA_LogLevel����
*           - color:   ��ӡ��ɫ���μ� OSA_LogColor����
*           - pFmt:    ��ӡ�ĸ�ʽ�ַ���
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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

#else //����OS_LINUX�꣬������Linux�ں�����/�û�̬
/*���ʱ���ӡ��ÿһ��ͨ����ʱ������ʱ�����*/
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

/* ����־ģ��û��ע��֮ǰ��OSA�ڲ���ӡ���� */
static Int32 OSA_logWriteDefault(Uint32 level, Uint32 color, \
                                        const Char *pFmt, va_list args)
{
	Int32 length;
    (void)level;
#if defined(__KERNEL__)
    /* �ں�̬Ĭ�ϴ�ӡ��printk */
    OSA_printColor(color);
    length = vprintk(pFmt, args);
    OSA_printColor(OSA_LOG_CLR_NONE);

#elif defined(___DSPBIOS___)
    /* SYSBIOS Ĭ�ϲ���ӡ */
    (void)color;
    (void)pFmt;
    (void)args;

#elif defined(OS21)
    kernel_printf(pFmt, args);

#elif defined(__RTEMS__)
    vprintk(pFmt, args);

#else

    /* �û�̬Ĭ�ϴ�ӡ��printf */
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
* ������  : OSA_logWrite
* ��  ��  : �ú����ṩͳһ��ӡ�ӿ�
* ��  ��  : - level:   ��ӡ�ȼ����μ� OSA_LogLevel����
*           - color:   ��ӡ��ɫ���μ� OSA_LogColor����
*           - pFmt:    ��ӡ�ĸ�ʽ�ַ���
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
	
	/* ��ȡ��ģ��/�ⶨ��� OSA_MODULE_NAME */
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
		 * Ϊ�˼���Դ�ӿڣ�����δ����OSA_logManagerRegister��ʽע�����ṹ��Ϊ��ע��Ĭ�Ϲ���ģ��
		 * ���ִ�ӡ�ӿ�û��OSA_MODULE_NAME������modNameΪNULL������Щ�ӿ�����ע�����ṹ
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
	 * ���ݲ���OSA_MODULE_NAME������ӿڣ��˳�����osa_entryΪNULL��Ĭ�����
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
* ������  :  OSA_logManagerRegister
* ��  ��  : �ú�����OSAע��ģ����־����ṹ��ͨ����ģ��/���ڳ�ʼ��ʱ���á�
*           ����δ��OSA_LOGMANAGERע��֮ǰ������ģ���ӡĬ�ϴ򿪡�
*           ��ģ���������ñ��ӿ���OSAע�����ṹ��
*           ���е���־��ӡ��ͨ����־����ģ��Ľӿڽ��й���
*           ������ע�����ģ�飬�ٴμ��غ�ֱ�Ӹ���ԭ�ṹ����Ϊ��
*           ������뱨�����ڴ��ڴ�й¶��ע��ʱ���·������ڴ�
* ��  ��  : OSA_LogManager:  ��ע�����־����ṹ����
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
******************************************************************************/
Int32 OSA_logManagerRegister(OSA_LogManager *pLogManager)
{
	OSA_LogManager *osa_entry;
	
	OSA_assertNotNull(pLogManager);
	
	osa_entry = OSA_LogManagerGetbyName(pLogManager->info.osa_Name);
	if (osa_entry) { //�ѱ�ע���ֱ�Ӹ���
		osa_entry->info.osa_Stat = 0; //ע���Ĭ��ʹ�ܸ�ģ�����־���
		osa_entry->info.osa_PrintEnable = (int8_t)OSA_LOGMANAGER_ENABLE; //ע���Ĭ��ʹ�ܸ�ģ�����־���
		osa_entry->info.osa_LogLevel = (osa_entry->info.osa_LogLevel >= OSA_LOGMANAGER_CMD_MAX) ?
							OSA_LOG_LV_DBG : osa_entry->info.osa_LogLevel;

		/* ����ע��ʱ��ʹ�ü����ָ����ü��� */
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
		
		osa_entry->info.osa_Stat = 0;   //ע���Ĭ�Ϲرո�ģ�����־ͳ��
		osa_entry->info.osa_PrintEnable = OSA_LOGMANAGER_ENABLE; //ע���Ĭ��ʹ�ܸ�ģ�����־���
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
* ������  :  OSA_logManagerDeRegister
* ��  ��  : �ú�����OSAע��ģ����־����ṹ��ֻ����ģ��ע��ʱ���á�
*           ��ģ�鱻ע��ʱ�����Թ���ṹ�ڴ�����ͷţ�ֻ�Ƕ����ü������д���
*           ���¼���ģ��󣬿ɰ�ͬ��ƥ�����ԭ��������ڴ����
*           Ϊ�˱�����뱨�����ڴ��ڴ�й¶��ע��ʱ�ͷ����ڴ�ռ�
*
* ��  ��  : OSA_LogManager:  ��ע�����־����ṹ����
*
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
******************************************************************************/
Int32 OSA_logManagerDeRegister(OSA_LogManager *pLogManager)
{
	OSA_assertNotNull(pLogManager);
	
	pLogManager->info.osa_Refcnt = -1; //ֻ�Ǽ��������ü���
			
	pLogManager->info.osa_PrintEnable = 0; //ע����رո�ģ�����־���

	//list_del(&pLogManager->node);
	return OSA_SOK;
}

static OSA_LogManager *OSA_LogManagerGetbyName(const Char *modName)
{
	Int32 len;
	OSA_LogManager *entry;
	OSA_ListHead  *pOsaList = NULL;
	OSA_ListHead  *pListTmp = NULL;

	/* ��ֹ��ģ��δ���� OSA_MODULE_NAME */
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
* ������  :  OSA_LogManagerSetEntryPrintLevel 
* ��  ��  : �ú�����OSA����Ķ�Ӧģ��/���������ӡ�ȼ� 
*           ������ƣ���������Ҫ��֤��֤���ð�ȫ
*      
* ��  ��  : modName,:    ������ģ������
* ��  ��  : level:   ��ӡ�ȼ�
* ��  ��  : 
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
* ������  :  OSA_LogManagerSetEntryEnable 
* ��  ��  : �ú�������OSA����Ķ�Ӧģ��/���ӡʹ�ܻ�ر�
*           ������ƣ���������Ҫ��֤��֤���ð�ȫ
*      
* ��  ��  : modName,:    ������ģ������
* ��  ��  : status:   ʹ�ܻ�ر�ģ��/��Ĵ�ӡ���
* ��  ��  : 
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
* ������  :  OSA_LogManagerSet_EntryInfo
* ��  ��  : �ú�����ȡOSA������ĵ���ģ��/����Ϣ
* 
* 
* ��  ��  : pentry ������Ҫ��ȡ��Ϣģ��/�������
* ��  ��  : pentry ����ָ��ģ��/�����Ϣ,��������ģ���ӡͳ����
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
* ������  :  OSA_LogManagerGetEntryPrintLevel 
* ��  ��  : �ú�����ȡOSA����Ķ�Ӧģ��/��Ĵ�ӡ�ȼ� 
*           ������ƣ���������Ҫ��֤��֤���ð�ȫ
*      
* ��  ��  : modName,:    ������ģ������
* ��  ��  : level:   ����ģ���ӡ�ȼ�
* ��  ��  : 
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
* ������  :  OSA_LogManagerGetEntryPrintStatics
* ��  ��  : �ú�����ȡ��Ӧģ��/����ش�ӡͳ����
*           �û�̬ͳ�Ƽ��������������ں�̬ʹ��ԭ�ӱ��� 
*           ��ǰ��Ҫ�ɻ�ȡģ��/��Ĵ�ӡͳ�Ƽ���
*      
* ��  ��  : modName,:    ������ģ������
* ��  ��  : printedBytes:   ����ģ��/���ӡ����
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
* ������  :  OSA_LogManagerGetEntryInfo
* ��  ��  : �ú�����ȡOSA������ĵ���ģ��/����Ϣ
* 
* 
* ��  ��  : pentry ������Ҫ��ȡ��Ϣģ��/�������
* ��  ��  : pentry ����ָ��ģ��/�����Ϣ,��������ģ���ӡͳ����
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
* ������  :  OSA_LogManagerGetAllEntryInfoSize
* ��  ��  : �ú�����ȡOSA�����������ģ��/�����ṹռ���ڴ�ռ�
*           Ŀ�����ڱ��ڵ����߻�ȡ����ṹ��Ϣʱ�������㹻�ڴ�ռ�
* 
* ��  ��  : ��  
* ��  ��  : ��
* ����ֵ  : OSA����������ģ��/����Ľṹ��Ϣռ�ÿռ�
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
* ������  :  OSA_LogManagerGetAllEntryInfo
* ��  ��  : �ú�����ȡOSA�����������ģ��/�⵱ǰ���������ӡ��
* 
* 
* ��  ��  : size Ϊbuf�Ĵ�С
* ��  ��  : buf ���ڷ���OSA������ģ��/������ݽṹ����Ϣ��buf�����㹻������
		   : ���򽫻�ȡʧ�ܣ����ػ�ȡ��Ϣ����Ҫ�����С
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
			> 0��ֵ��ʾ����buf���ṩ�Ŀռ䲻�����㷵�ؽṹ����Ϣ����ռ䣬��ʱ
			��������Ŀռ�
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
* ������  :  OSA_LogManagerDisableAllEntryStat
* ��  ��  : �ú����ر�/��OSA�����������ģ��/�������ӡ��ͳ�ƹ���
*           �û�̬��ӡͳ�ƹ��ܻ����������ܿ��������Ӹýӿڹر�ͳ�ƹ���
* 
* ��  ��  :  ��
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
			  covertiy�ţ� 323707
			  ����������osa_entity ��ʽNULL ��������
			  �޸�������osa_entity ������֮�����ÿ�
			  ע������������ 
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
* ������  :  OSA_LogManagerUser_init
* ��  ��  : �ú���ΪOSA�����ĳ�ʼ���ӿ�
* 
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
* ������  :  OSA_LogManagerUser_exit
* ��  ��  : �ú���ΪOSA������ע���ӿ�
*           �ýӿڽ������û�Ӧ�ÿ�����ģ��ע��
* 
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�
*          OSA_EFAIL: ʧ��
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
