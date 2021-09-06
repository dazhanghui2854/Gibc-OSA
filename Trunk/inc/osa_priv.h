/*******************************************************************************
* osa_priv.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: 定义OSA模块内部使用的头文件
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
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#define OSA_MAGIC_NUM   0x4D414749

/* 线程名最大长度 */
#define OSA_THREAD_NAME_MAX_LEN     (32)

/* 线程信息记录设备名称 */
#define OSA_THREAD_RECODE_DEV       "threads"  /* thread recode info */

/* 线程类型掩码，0表示用户线程，1表示内核线程 */
#define OSA_THREAD_MODE_MASK        0x80000000
#define OSA_THREAD_TID_MASK         (~(OSA_THREAD_MODE_MASK))

#define OSA_REQ_MAXLEN 32
#define DH_LOGMANAGE_NAME "dh_logmagle"
#define DH_LOGMANGE_FILENMAE "/dev/dh_logmagle"
#define DH_LOGMANAGER_BASE  'L'

typedef enum {
	OSA_LOGMANAGER_CMD_SETENABLE,   /* 设置模块日志输出使能 */
	OSA_LOGMANAGER_CMD_GETENABLE,
	OSA_LOGMANAGER_CMD_SETLEVEL,    /* 设置模块/库内部打印等级 */
	OSA_LOGMANAGER_CMD_GETLEVEL,    /* 获取模块/库内部打印等级 */
	OSA_LOGMANAGER_CMD_GETPRINTS,   /* 获取模块/库打印输出计数 */
	OSA_LOGMANAGER_CMD_SETSTAT,     /* 设置模块/库内部打印统计 */
	OSA_LOGMANAGER_CMD_SETENTRY,    /* 设置模块/库内部功能项 */
	OSA_LOGMANAGER_CMD_GETENTRY,    /* 获取模块/库内部功能项 */
	OSA_LOGMANAGER_CMD_MAX
} OSA_LogManagerCmd;

typedef enum {
	OSA_LOGMANAGER_REGISTER_EXTERN = 1,    /* 模块内部注册默认对象，资源由模块负责释放 */
	OSA_LOGMANAGER_REGISTER_INNER,       /* 外部模块注册，资源由外部模块负责释放 */
} OSA_LOGMANAGER_REGISTER_TYPE_e;

typedef struct OSA_LogManagerReq_t {
	char osa_Name[OSA_LOGENTITY_NAMELEN];
	OSA_LogManagerCmd osa_cmd;      /* 配置请求命名 */
	uint8_t osa_val[OSA_REQ_MAXLEN];   /*请求的输入及结果，遇到错误返回具体原因 */
} OSA_LogManagerReq;

typedef struct OSA_LogManagerReqInfo_t {
	size_t size;   /* OSA管理所有模块空间大小 */
	uint8_t *buf;  /* 返回OSA管理所有模块信息 */
} OSA_LogManagerReqInfo;

#define DH_LOGMANAGER_GET  _IOR(DH_LOGMANAGER_BASE, 1, OSA_LogManagerReq)
#define DH_LOGMANAGER_SET   _IOW(DH_LOGMANAGER_BASE, 2, OSA_LogManagerReq)
#define DH_LOGMANAGER_GETINFO   _IOR(DH_LOGMANAGER_BASE, 3, OSA_LogManagerReqInfo)
#define DH_LOGMANAGER_GETENTRYS _IOR(DH_LOGMANAGER_BASE, 4, Uint32)

/* 记录操作类型，增加/删除 */
typedef enum
{
    OSA_RECODE_TID_ADD  = 0x80,
    OSA_RECODE_TID_DEL,
    OSA_RECODE_TID_GET,
} OSA_RecodeTidCmd;

/* 线程记录信息内容 */
typedef struct
{
    Uint32              tid;       /* 线程ID, bit31区分内核线程还是用户线程 */
    Uint16              policy;    /* 调度策略 */
    Uint16              priority;  /* 优先级 */
    Uint32L             tBodyAddr; /* 线程运行函数地址 */
    Ptr                 privData;  /* 线程私有数据 */
    Char                *pName;
} OSA_ThreadInfo;

/* 线程记录信息内容 */
typedef struct
{
    Uint32              tid;       /* 线程ID, bit31区分内核线程还是用户线程 */
    Uint16              policy;    /* 调度策略 */
    Uint16              priority;  /* 优先级 */
    Uint32L             tBodyAddr; /* 线程运行函数地址 */
    Ptr                 privData;  /* 线程私有数据 */
    Char                name[OSA_THREAD_NAME_MAX_LEN];
} OSA_RecodeTidCmdArgs;

#ifndef OS_LINUX
/* 登记线程信息 */
Int32 OSA_addTidInfo(OSA_ThreadInfo* pInfo);
/* 移除线程信息 */
Int32 OSA_delTidInfo(Uint32 tid);
/* 获取线程信息 */
Int32 OSA_getTidInfo(OSA_ThreadInfo* pInfo);
#else
/* 非Linux平台，线程信息不做处理 */
#define OSA_addTidInfo(a) ((void)a)
#define OSA_delTidInfo(a) ((void)a)
#define OSA_getTidInfo(a) ((void)a)
#endif

/* 登记线程信息 */
Int32 OSA_drvAddTriActual(OSA_ThreadInfo* pInfo, Bool32 isKernel);
/* 移除线程信息 */
Int32 OSA_drvDelTriActual(Uint32 tid, Bool32 isKernel);
/* 获取线程信息 */
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
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                          变量声明区                                        */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */
#ifdef __KERNEL__
/*linux内核态私有的接口*/
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


