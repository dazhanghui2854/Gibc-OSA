/*******************************************************************************
* log.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-10-15 Create
*
* Desc: 定义日志模块对外接口头文件。
*
*       1. 硬件说明。
*          无。
*
*       2. 程序结构说明。
*          日志模块提供了一套框架，配合外部的数据收发模块，
*          实现从机向主机发送日志的功能，
*          程序结构图如下:
*
*               从机端                   主机端
*
*              日志模块                 日志模块
*
*                 ↓                       ↑
*
*            数据发送模块      →      数据接收模块
*
*          1) 在从机端，日志模块通过调用数据发送模块注册的接口，
*             将日志发送出去。
*          2) 从机端数据发送模块负责提供具体的数据发送通道。
*          3) 主机端数据接收模块负责提供具体的数据接收通道，
*             向日志模块注册日志接收的函数
*          4) 主机端日志模块内部会启动一个任务，
*             通过不断调用数据接收模块注册的日志接收函数，获取日志。
*
*       3. 使用说明。
*          从机向主机发送日志的接口使用说明如下:
*
*          1) 从机端数据发送模块调用LOG_drvRegExtSlave向日志模块注册
*             可通过入参pExtParms->logFlag指定哪些日志操作需要通过外部模块完成，
*             比如LOG_LOCATION_UART|LOG_LOCATION_FILE表示,
*             所有从机端需要打印到串口和文件的日志，
*             都将通过数据发送模块发给主机，剩余的打印到内部RAM的日志，
*             则仍然打印到从机端本地的内部RAM中。
*
*          2) 主机端数据接收模块调用LOG_drvRegExtHost向日志模块注册,
*             注册后，日志模块内部将启动一个任务，
*             通过不断调用数据接收模块注册的日志接收函数，获取日志。
*
*          3) 从机端数据发送模块不再需要向主机发送日志时调用
*             LOG_drvUnRegExtSlave取消注册
*          4) 主机端数据接收模块不再需要接收从机的日志时调用
*             LOG_drvUnRegExtHost取消注册
*
*       4. 局限性说明。
*          无。
*
*       5. 其他说明。
*          无。
*
* Modification:
*    Date    :  2013-6-4
*    Revision:
*    Author  :  zheng_wei
*    Contents:  增加从机向主机发送日志功能
*******************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_



/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* 定义日志打印位置 */
#define LOG_LOCATION_NONE   (0)     /* 不打印 */
#define LOG_LOCATION_UART   (1<<0)  /* 串口 */
#define LOG_LOCATION_RAM    (1<<1)  /* 内部RAM */
#define LOG_LOCATION_FILE   (1<<2)  /* 文件 */

/* 从机端注册时打印前缀的大小 */
#define LOG_PREFIX_LEN (32)

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* 从机向主机通过外部模块发送的单条日志信息结构 */
typedef struct
{
    Uint32 level;       /* 日志级别，参见OSA_LogLevel定义 */
    Uint32 len;         /* 日志数据长度 */
    Char  *pBuf;        /* 日志数据buffer指针 */

    Uint32 reserved[5];
}LOG_ExtEntry;

/* 从机端外部模块向Log注册发送日志接口的结构体，
   该结构体中的所有方法由外部模块实现 */
typedef struct
{
    /* 发送接口, 入参pLog中的内容由日志模块负责填充 */
    Int32 (*OpSendLog)(LOG_ExtEntry *pLog);

    Uint32 reserved[7];
}LOG_ExtSlaveOps;

/* 从机端外部模块向Log注册时，需要提供的参数 */
typedef struct
{
    /*
        操作的日志标记，
        可以由LOG_LOCATION_UART、LOG_LOCATION_RAM、LOG_LOCATION_FILE
        中的一个或多个相或组成。
        表示哪些日志操作需要通过外部模块完成
    */
    Uint32          logFlag;

    /* 外部模块注册时，需要添加的打印前缀 */
    Char            logPrefix[LOG_PREFIX_LEN];

    /* 日志操作接口 */
    LOG_ExtSlaveOps logOps;

    Uint32 reserved[4];
}LOG_ExtSlaveParams;

/* 主机端外部模块向Log注册接收日志接口的结构体，
   该结构体中的所有方法由外部模块实现 */
typedef struct
{
    /* 接收接口, 入参pLog中的内容由外部模块负责填充 */
    Int32 (*OpRcvLog)(LOG_ExtEntry *pLog);

    Uint32 reserved[7];
}LOG_ExtHostOps;

/* 主机端外部模块向Log注册时，需要提供的参数 */
typedef struct
{
    /* 日志操作接口 */
    LOG_ExtHostOps logOps;

    Uint32 reserved[4];
}LOG_ExtHostParams;


/* pmc向log模块注册的保存内核oops等异常日志pmc接口集 */
typedef struct
{

    /* pmc处理内核紧急信息注册接口 */
    Int32 (*OpKernelEmergLogWrite)(const Char *errInfo, Uint32 errInfoLen);

    Uint32 res[15];

}LOG_PmcOps;


/* 当前设备之前一次重启的异常原因 */
typedef enum
{
    CRASH_NONE        = 0,        /* 没有异常 */
    CRASH_LOSTPOWER   = 1 << 0,   /* 断电 */
    CRASH_WDT         = 1 << 1,   /* 看门狗重启 */
    CRASH_OOPS        = 1 << 2,   /* 内核崩溃 */
    CRASH_VDEXIT      = 1 << 3,   /* VideoDaemon异常退出 */
    CRASH_NOBIT       = 1 << 4,    /* 无码流 */
    BOOT_PROGRAM      = 1 << 5,
} LOG_CrashFlag;

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

#ifndef __KERNEL__

/*******************************************************************************
* 函数名  : LOG_init
* 描  述  : 初始化日志，用户态或者SYSBIOS下调用一次即可，内核态下无需调用
* 输  入  : - pInitParms: 初始化参数, 预留参数，可以为NULL
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_init(Ptr pInitParms);


/*******************************************************************************
* 函数名  : LOG_deInit
* 描  述  : 销毁日志模块，用户态或者SYSBIOS下调用，内核态下无需调用
* 输  入  : 无
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_deInit(void);


#endif


#ifdef __KERNEL__
/*******************************************************************************
* 函数名  : LOG_drvRegExtSlave
* 描  述  : 向日志模块注册从机端外部模块
* 输  入  : - pExtParms: 外部模块注册参数。
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_drvRegExtSlave(LOG_ExtSlaveParams *pExtParms);

/*******************************************************************************
* 函数名  : LOG_drvUnRegExtSlave
* 描  述  : 向日志模块取消已注册的从机端外部模块
* 输  入  : 无。
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_drvUnRegExtSlave(void);

/*******************************************************************************
* 函数名  : LOG_drvRegExtHost
* 描  述  : 向日志模块注册主机端外部模块
* 输  入  : - pExtParms: 外部模块注册参数。
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_drvRegExtHost(LOG_ExtHostParams *pExtParms);

/*******************************************************************************
* 函数名  : LOG_drvUnRegExtHost
* 描  述  : 向日志模块取消已注册的主机端外部模块
* 输  入  : 无。
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_drvUnRegExtHost(void);

/*******************************************************************************
* 函数名  : LOG_drvPmcRegister
* 描  述  : pmc模块接管内核oops等日志时，将接口注册给给log模块，log模块检测到发
*           生oops时，调用pmc注册的接管接口
* 输  入  : pOps:pmc向log模块注册的保存内核oops等异常日志pmc接口集
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_drvPmcRegister(LOG_PmcOps *pOps);

/*******************************************************************************
* 函数名  : LOG_drvPmcUnRegister
* 描  述  : 与LOG_drvPmcRegister对应的撤销注册接口
* 输  入  : 无。
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_drvPmcUnRegister(void);

#endif

/*******************************************************************************
* 函数名  : LOG_getCrashFlag
* 描  述  : 获取异常重启的标志
*
* 输  入  : - 无。
*
* 输  出  : - pFlag: 当前系统异常重启的标志，定义参见LOG_CrashFlag
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 LOG_getCrashFlag(Uint32 *pFlag);

/*******************************************************************************
* 函数名  : LOG_setCrashFlag
* 描  述  : 设置异常重启的标志,
*           设置之前要先获取标志，再 或(|) 上新标志，以防冲掉
*           其他的异常标志；
* 输  入  : - flag: 要设置的系统异常重启的标志，定义参见LOG_CrashFlag
*
* 输  出  : - 无
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 LOG_setCrashFlag(Uint32 flag);

/*******************************************************************************
* 函数名  : LOG_setCacheFlag
* 描  述  :设置cache标志，用于是否刷cache操作
* 输  入  : - bIsCacheEnable: 打印共享内存是否是cache
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 LOG_setCacheFlag(Bool bIsCacheEnable);


/*******************************************************************************
* 函数名  : LOG_setInsteadStdout
* 描  述  : 设置是否接管标准输出
*           如果设置使用标准输出，内核printk、tty输出将采用标准内核的方式
* 输  入  : - enable: true表示由LOG接管，false表示使用内核原始方式
*
* 输  出  : - 无
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 LOG_setInsteadStdout(Bool32 enable);

/*******************************************************************************
* 函数名  : LOG_drvFlushRam
* 描  述  : 把log缓冲区数据全部立即打印出来，缓冲区清空
* 输  入  : - 无
*
* 输  出  : - 无
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
*******************************************************************************/
Int32 LOG_drvFlushRam(void);

#ifdef __cplusplus
}
#endif

#endif  /*_LOG_H_ */

