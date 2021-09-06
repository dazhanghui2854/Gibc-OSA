/*******************************************************************************
* osa_priv.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: ����OSAģ���ڲ�ʹ�õ�ͷ�ļ�
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_PRIV_H_
#define _OSA_PRIV_H_


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
#define OSA_MAGIC_NUM   0x4D414749

/* �߳�����󳤶� */
#define OSA_THREAD_NAME_MAX_LEN     (32)

/* �߳���Ϣ��¼�豸���� */
#define OSA_THREAD_RECODE_DEV       "threads"  /* thread recode info */

/* �߳��������룬0��ʾ�û��̣߳�1��ʾ�ں��߳� */
#define OSA_THREAD_MODE_MASK        0x80000000
#define OSA_THREAD_TID_MASK         (~(OSA_THREAD_MODE_MASK))

#define OSA_REQ_MAXLEN 32
#define DH_LOGMANAGE_NAME "dh_logmagle"
#define DH_LOGMANGE_FILENMAE "/dev/dh_logmagle"
#define DH_LOGMANAGER_BASE  'L'

typedef enum {
	OSA_LOGMANAGER_CMD_SETENABLE,   /* ����ģ����־���ʹ�� */
	OSA_LOGMANAGER_CMD_GETENABLE,
	OSA_LOGMANAGER_CMD_SETLEVEL,    /* ����ģ��/���ڲ���ӡ�ȼ� */
	OSA_LOGMANAGER_CMD_GETLEVEL,    /* ��ȡģ��/���ڲ���ӡ�ȼ� */
	OSA_LOGMANAGER_CMD_GETPRINTS,   /* ��ȡģ��/���ӡ������� */
	OSA_LOGMANAGER_CMD_SETSTAT,     /* ����ģ��/���ڲ���ӡͳ�� */
	OSA_LOGMANAGER_CMD_SETENTRY,    /* ����ģ��/���ڲ������� */
	OSA_LOGMANAGER_CMD_GETENTRY,    /* ��ȡģ��/���ڲ������� */
	OSA_LOGMANAGER_CMD_MAX
} OSA_LogManagerCmd;

typedef enum {
	OSA_LOGMANAGER_REGISTER_EXTERN = 1,    /* ģ���ڲ�ע��Ĭ�϶�����Դ��ģ�鸺���ͷ� */
	OSA_LOGMANAGER_REGISTER_INNER,       /* �ⲿģ��ע�ᣬ��Դ���ⲿģ�鸺���ͷ� */
} OSA_LOGMANAGER_REGISTER_TYPE_e;

typedef struct OSA_LogManagerReq_t {
	char osa_Name[OSA_LOGENTITY_NAMELEN];
	OSA_LogManagerCmd osa_cmd;      /* ������������ */
	uint8_t osa_val[OSA_REQ_MAXLEN];   /*��������뼰������������󷵻ؾ���ԭ�� */
} OSA_LogManagerReq;

typedef struct OSA_LogManagerReqInfo_t {
	size_t size;   /* OSA��������ģ��ռ��С */
	uint8_t *buf;  /* ����OSA��������ģ����Ϣ */
} OSA_LogManagerReqInfo;

#define DH_LOGMANAGER_GET  _IOR(DH_LOGMANAGER_BASE, 1, OSA_LogManagerReq)
#define DH_LOGMANAGER_SET   _IOW(DH_LOGMANAGER_BASE, 2, OSA_LogManagerReq)
#define DH_LOGMANAGER_GETINFO   _IOR(DH_LOGMANAGER_BASE, 3, OSA_LogManagerReqInfo)
#define DH_LOGMANAGER_GETENTRYS _IOR(DH_LOGMANAGER_BASE, 4, Uint32)

/* ��¼�������ͣ�����/ɾ�� */
typedef enum
{
    OSA_RECODE_TID_ADD  = 0x80,
    OSA_RECODE_TID_DEL,
    OSA_RECODE_TID_GET,
} OSA_RecodeTidCmd;

/* �̼߳�¼��Ϣ���� */
typedef struct
{
    Uint32              tid;       /* �߳�ID, bit31�����ں��̻߳����û��߳� */
    Uint16              policy;    /* ���Ȳ��� */
    Uint16              priority;  /* ���ȼ� */
    Uint32L             tBodyAddr; /* �߳����к�����ַ */
    Ptr                 privData;  /* �߳�˽������ */
    Char                *pName;
} OSA_ThreadInfo;

/* �̼߳�¼��Ϣ���� */
typedef struct
{
    Uint32              tid;       /* �߳�ID, bit31�����ں��̻߳����û��߳� */
    Uint16              policy;    /* ���Ȳ��� */
    Uint16              priority;  /* ���ȼ� */
    Uint32L             tBodyAddr; /* �߳����к�����ַ */
    Ptr                 privData;  /* �߳�˽������ */
    Char                name[OSA_THREAD_NAME_MAX_LEN];
} OSA_RecodeTidCmdArgs;

#ifndef OS_LINUX
/* �Ǽ��߳���Ϣ */
Int32 OSA_addTidInfo(OSA_ThreadInfo* pInfo);
/* �Ƴ��߳���Ϣ */
Int32 OSA_delTidInfo(Uint32 tid);
/* ��ȡ�߳���Ϣ */
Int32 OSA_getTidInfo(OSA_ThreadInfo* pInfo);
#else
/* ��Linuxƽ̨���߳���Ϣ�������� */
#define OSA_addTidInfo(a) ((void)a)
#define OSA_delTidInfo(a) ((void)a)
#define OSA_getTidInfo(a) ((void)a)
#endif

/* �Ǽ��߳���Ϣ */
Int32 OSA_drvAddTriActual(OSA_ThreadInfo* pInfo, Bool32 isKernel);
/* �Ƴ��߳���Ϣ */
Int32 OSA_drvDelTriActual(Uint32 tid, Bool32 isKernel);
/* ��ȡ�߳���Ϣ */
Int32 OSA_drvGetTriActual(OSA_ThreadInfo* pInfo, Bool32 isKernel);

#ifdef _OSA_TRACE_
#define OSA_handleCheck(handle) \
    do \
    { \
        if(NULL == (handle)) \
        { \
            OSA_ERROR("NULL handle\n"); \
            return OSA_EFAIL; \
        } \
        \
        if(OSA_MAGIC_NUM != (handle)->nMgicNum) \
        { \
            OSA_ERROR("Invalid Magic Num : %d\n", (handle)->nMgicNum); \
            return OSA_EFAIL; \
        } \
    } \
    while(0)

    #ifdef __KERNEL__
    #define OSA_noInterruptCheck() \
        do \
        { \
            if(in_interrupt()) \
            { \
                OSA_ERROR("Can not be called from Interrupt Context\n"); \
                return OSA_EFAIL; \
            } \
        } \
        while(0)

    #define OSA_noInterruptCheckRetNULL() \
        do \
        { \
            if(in_interrupt()) \
            { \
                OSA_ERROR("Can not be called from Interrupt Context\n"); \
                return NULL; \
            } \
        } \
        while(0)
    #endif

#else
    #define OSA_handleCheck(handle)
    #define OSA_noInterruptCheck()
    #define OSA_noInterruptCheckRetNULL()
#endif


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */
#ifdef __KERNEL__
/*linux�ں�̬˽�еĽӿ�*/
Int32 OSA_kProcInit(void);
Int32 OSA_kProcExit(void);
Int32 OSA_kWaitGetInternelPtr(OSA_KWaitHandle hWait, Ptr *pPtr);
Int32 OSA_LogManager_init(void);
void OSA_LogManager_exit(void);
#elif defined(OS_LINUX)
Int32 OSA_LogManagerUser_init(void);
Int32 OSA_LogManagerUser_exit(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_PRIV_H_  */


