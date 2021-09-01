/*******************************************************************************
* osa_task.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA的任务Task模块，包含任务创建、注销、控制等一系列操作接口。
*              内部会启动一个监测任务，监测其他任务是否超时，超时则打印出超时
*              时间值。任务创建并成功运行后，默认被加入到被监测链表中。
*
*       1. 硬件说明。
*          无。
*
*       2. 程序结构说明。
*          无。
*
*       3. 使用说明。
*          一般启动任务时，希望能控制任务启动的时机，如希望所有任务启动并初始化
*          各自的模块后，再真正开始各任务的自行循环执行，所以本模块提供了此类接口，
*          是创建参数OSA_TskCreate中的OSA_TskOps和OSA_TskCreateFlag，这两个结构体
*          中的成员可控制任务的创建过程，详细请参见各成员说明。一旦设置了
*          OSA_TskCreateFlag的某个标志，则必须在任务创建后，调用相应的命令接口，
*          形式是OSA_tskCmdxxx()，否则任务不会真正执行下去，而是一直在等待该命令。
*
*          1) 任务创建和启动时接口调用流程如下:
*           ==========================
*              OSA_tskCreate()
*                    |
*                   \|/
*              OSA_tskCmdCreate()  // createFlags |= OSA_TSK_FLAG_CREATE
*                    |
*                   \|/
*              OSA_tskCmdStart()   // createFlags |= OSA_TSK_FLAG_START
*                    |
*                   \|/
*                   Run
*           ===========================
*          2) 销毁任务，调用接口OSA_tskDelete()。
*          3) 发送或广播消息，分别调用OSA_tskSendMsg()和OSA_tskBroadcastMsg()。
*          4) 暂停和重新启动任务，分别调用OSA_tskStop()和OSA_tskStart()。
*          5) 打印指定任务后全部任务统计值，分别调用OSA_tskPrintStat()和
*             OSA_tskPrintAllStat()。
*
*       4. 局限性说明。
*          用户的命令cmd最高位不能是1，也就是不能是0x8XXX，此类命令内部使用。
*
*       5. 其他说明。
*          无。
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_TSK_H_
#define _OSA_TSK_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* 任务句柄 */
typedef Handle OSA_TskHandle;


typedef enum
{
    OSA_TSK_FLAG_NONE     =  0x00,

    /*
      该标志设置后，一定要注册OpTskCreate()回调函数，并调用
      OSA_tskCmdCreate()函数。没有设置，则无需调用。
     */
    OSA_TSK_FLAG_CREATE   =  0x01,

    /*
      该标志设置后，一定要注册OpTskStart()回调函数，并调用
      OSA_tskCmdStart()函数。没有设置，则无需调用。
     */
    OSA_TSK_FLAG_START    =  0x02,
} OSA_TskCreateFlag;


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* 任务相关的操作函数 */
typedef struct
{
    /*
       以下两个回调函数是在任务创建时被调用，两者同时存在时，
       先调用OpTskCreate再调用OpTskStart。通过OSA_TskCreate.createFlags
       指示是否调用这两个函数，一旦指定了，如果函数指针无效，则会警告。
       这三个回调函数的目的是方便用户控制其模块的初始化、销毁和启动。
     */
    Int32 (*OpTskCreate)(Ptr pUsrArgs);
    Int32 (*OpTskStart)(Ptr pUsrArgs);

    /* 此回调函数在删除Task时被调用。*/
    Int32 (*OpTskDelete)(Ptr pUsrArgs);

    /*
      任务的运行回调函数，请不要在该函数中主动休眠，可在创建运行任务是指定
      休眠时间。pCmdPrms是cmd命令相关的数据，有可能为NULL，使用前一定要检查
      有效性。pUsrArgs是用户注册时提供的私有数据。
    */
    Int32 (*OpTskRun)(Uint32 cmd,
                      Ptr    pCmdPrms,
                      Ptr    pUsrArgs);

    Uint32 reserved[4];
} OSA_TskOps;



/* 出现任务运行超时后的处理函数，由用户提供。*/
typedef Int32 (*OSA_TskTimeOutAck)(Ptr pUsrArgs);


/* Task属性 */
typedef struct
{
    OSA_ThrAttr     thrAttr;     /* 线程属性 */
    Uint64          reserved[8]; /* 保留字段 */
} OSA_TskAttr;


/* 任务创建时指定的参数。*/
typedef struct
{
    const Char *pName; /* 任务名字 */

    OSA_TskOps  tskOps; /* 任务回调接口 */

    Uint32      tskPol;     /* 任务调度策略，其定义见OSA_thrSchedPolicy */
    Uint32      tskPri;     /* 任务调度优先级 */

    /*
    栈大小，OSA_THR_STACK_SIZE_DEFAULT表示默认栈大小，如果为默认大小，
    则pStackAddr不起作用
    */
    Uint32      stackSize;

    Uint32      nMsgNum;    /* 任务消息队列中消息数量 */

    /* 任务休眠时间。以ms为单位， OSA_TIMEOUT_NONE表示不休眠。*/
    Uint32      nTimeSleep;

    /*
      任务超时时间，表示一个任务多久没有运行就表明异常，一旦超时检测任务将会报警。
      以ms为单位， OSA_TIMEOUT_FOREVER表示永不超时。
    */
    Uint32      nTimeOut;

    /* 任务超时后的响应函数 */
    OSA_TskTimeOutAck OpTimeOutAck;

    /* 控制任务启动过程的标志，其定义见OSA_TskCreateFlag。*/
    Uint32      createFlags;

    Ptr         pUsrArgs;   /* 用户自定义参数 */

    Ptr         pStackAddr; /* 栈起始地址，Linux内核态中无效，不使用时请设为NULL。*/

    Ptr_t(OSA_TskAttr, pAttr);

    Uint32 reserved[1];
} OSA_TskCreate;


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_tskSendMsg
* 描  述  : 向指定的任务发消息。
* 输  入  : - hTskTo  : 目的任务句柄。
*         : - hTskFrom: 源任务句柄。
*         : - cmd     : 命令值。最高1位不能使用，内部使用。
*         : - flags   : 消息的标志，表明该消息的一些属性，定义请查看
*                       OSA_MbxMsgFlags。
*         : - pPrm    : 命令伴随的参数
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskSendMsg(OSA_TskHandle hTskTo,
                      OSA_TskHandle hTskFrom,
                      Uint16 cmd,
                      Uint16 flags,
                      Ptr pPrm);

/*******************************************************************************
* 函数名  : OSA_tskBroadcastMsg
* 描  述  : 向指定数量的任务广播消息。
* 输  入  : - phTskToList  : 目的任务句柄数组，以NULL结尾。
*         : - hTskFrom     : 源任务句柄。
*         : - cmd          : 命令值。最高1位不能使用，内部使用。
*         : - flags        : 消息的标志，表明该消息的一些属性，定义请查看
*                            OSA_MbxMsgFlags。
*         : - pPrm         : 命令伴随的参数
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskBroadcastMsg(OSA_TskHandle *phTskToList,
                           OSA_TskHandle  hTskFrom,
                           Uint16 cmd,
                           Uint16 flags,
                           Ptr pPrm);

/*******************************************************************************
* 函数名  : OSA_tskFlushMsg
* 描  述  : 清除指定任务已经接收到的所有消息。
* 输  入  : - hTsk   : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskFlushMsg(OSA_TskHandle hTsk);

/*******************************************************************************
* 函数名  : OSA_tskStartMon
* 描  述  : 启动监测指定的任务，主要是监测任务是否超时。
* 输  入  : - hTsk   : 将要被监测的任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStartMon(OSA_TskHandle hTsk);

/*******************************************************************************
* 函数名  : OSA_tskStopMon
* 描  述  : 停止监测指定的任务。
* 输  入  : - hTsk   : 将要被监测的任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStopMon(OSA_TskHandle hTsk);


/*******************************************************************************
* 函数名  : OSA_tskCmdCreate
* 描  述  : 向任务发送创建命令，会调用用户注册的OpTskCreate()回调函数。
* 输  入  : - hTsk   : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskCmdCreate(OSA_TskHandle hTsk);

/*******************************************************************************
* 函数名  : OSA_tskCmdStart
* 描  述  : 向任务发送启动命令，会调用用户注册的OpTskStart()回调函数。
* 输  入  : - hTsk   : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskCmdStart(OSA_TskHandle hTsk);


/*******************************************************************************
* 函数名  : OSA_tskStop
* 描  述  : 暂停任务。在等待任务停止期间会休眠。要与OSA_tskStart()接口成对使用。
* 输  入  : - hTsk   : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStop(OSA_TskHandle hTsk);

/*******************************************************************************
* 函数名  : OSA_tskStart
* 描  述  : 重新启动任务。要与OSA_tskStop()接口成对使用，先调用它，然后调用本接口。
* 输  入  : - hTsk  : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStart(OSA_TskHandle hTsk);


/*******************************************************************************
* 函数名  : OSA_tskCreate
* 描  述  : 创建任务。
* 输  入  : - pCreate : 创建参数，不能为NULL。
* 输  出  : - phTsk   : 任务句柄指针，输出给调用者使用。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskCreate(OSA_TskCreate *pCreate,
                     OSA_TskHandle *phTsk);

/*******************************************************************************
* 函数名  : OSA_tskCreate
* 描  述  : 销毁任务。
* 输  入  : - hTsk : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskDelete(OSA_TskHandle hTsk);


/*******************************************************************************
* 函数名  : OSA_tskPrintStat
* 描  述  : 打印指定Task的统计数据。
* 输  入  : - hTsk  : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK: 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskPrintStat(OSA_TskHandle hTsk);

/*******************************************************************************
* 函数名  : OSA_tskPrintAllStat
* 描  述  : 打印所有Task的统计数据。
* 输  入  : - hTsk  : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskPrintAllStat(void);

#ifdef __cplusplus
}
#endif

#endif /* OSA_TSK_H_ */


