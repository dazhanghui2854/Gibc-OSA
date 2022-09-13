/*******************************************************************************
* osa_task.c
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


#include <osa.h>
#include <osa_priv.h>


/* 内部使用的标准命令，用户不能使用。用于实现控制任务的启动时机。*/
typedef enum
{
    OSA_TSK_CMD_FLAG   = 0x8000,
    OSA_TSK_CMD_CREATE = 0x8001,  /* 任务创建命令 */
    OSA_TSK_CMD_DELETE = 0x8002,  /* 任务删除命令 */
    OSA_TSK_CMD_START  = 0x8004,  /* 任务启动命令 */

    OSA_TSK_CMD_MASK   = 0x7fff,
} OSA_TskCmdFlags;


/* 统计信息对象，每个任务生成一个实例。*/
typedef struct
{
    Uint32 curRunTime;      /* Task当前循环运行时间       */
    Uint32 minRunTime;      /* Task单个循环最小运行时间   */
    Uint32 maxRunTime;      /* Task单个循环最大运行时间   */
    Uint32 totalRunTime;    /* Task所有循环的总共运行时间 */
    Uint32 totalRunCnt;     /* Task循环次数 */

    Uint16 tskPri;  /* Task优先级 */
    Uint16 tskPol;  /* Task调度策略 */
    Int32  tskTid;  /* Task线程号 */
    Int32  tskPid;  /* Task进程号 */
} OSA_TskStatObj;


/* 任务管理对象，每个任务生成一个实例。*/
typedef struct
{
    OSA_ThrHandle  hThr;

    OSA_MbxHandle  hMbx;      /* Mailbox handle */

    OSA_MutexHandle hLock;    /* 停止Task用的锁 */

    OSA_TskOps     tskOps;    /* 任务对应的处理函数，由用户注册传入。*/

    /* 任务休眠时间。以ms为单位， OSA_TIMEOUT_NONE表示不休眠。*/
    Uint32         nTimeSleep;

    Ptr            pUsrArgs;  /* 用户自定义参数 */

    /* 任务创建控制标志，其定义见OSA_TskCreateFlag。*/
    Uint32         createFlags;

    /* 已经执行的命令标记，其定义见OSA_TskCmdFlags。*/
    Uint32         cmdFlags;

    OSA_TskStatObj statObj;    /* 统计信息 */

    Int8           name[32];

    Uint32          isActive;

    Uint32          isStop;
} OSA_TskObj;


/* 任务管理对象的Wrapper，增加了任务监测用的成员，每个任务生成一个实例。*/
typedef struct
{
    /*
      任务对象。tskObj必须存放在首位置，因为程序里会使用强制类型转换，
      互用OSA_TskObj和OSA_TskMonObj两种类型的指针。
      */
    OSA_TskObj tskObj;

    /* 任务超时时间。以ms为单位， OSA_TIMEOUT_FOREVER表示永不超时。*/
    Uint32 nTimeOut;

    /* 任务超时后的响应函数 */
    OSA_TskTimeOutAck OpTimeOutAck;

    /* 任务链表管理成员，用于链接其他任务。*/
    OSA_ListHead tskElem;
} OSA_TskMonObj;


/* 整个osa_tsk模块的管理对象，整个模块只有一个实例。*/
typedef struct
{
    /* 一旦任务创建后，一定存在于以下两个链表中的一个当中。*/

    /* 处于被监测状态的任务链表 */
    OSA_ListHead actvTskList;
    /* 处于未被监测状态的链表 */
    OSA_ListHead idleTskList;

    /* 监测任务的句柄，整个模块只有一个监测任务 */
    OSA_TskHandle hMonTsk;

    /* 多线程访问链表的锁 */
    OSA_MutexHandle hLock;

} OSA_TskModuleObj;


/* osa_tsk模块管理对象的实例 */
static OSA_TskModuleObj gTskModObj;


/* 更新统计时间 */
static inline void TSK_updateStatTime(OSA_TskStatObj *pStatObj,
                                      Uint32 startTime,
                                      Uint32 endTime)
{
    Uint32 runTime;

    OSA_assertNotNull(pStatObj);

    /* 更新运行时间，考虑了溢出情况。*/
    runTime = endTime >= startTime ? endTime - startTime
                  : endTime + ((Uint32)(~0) - startTime);

    if (runTime < pStatObj->minRunTime)
    {
        pStatObj->minRunTime = runTime;
    }

    if (runTime > pStatObj->maxRunTime)
    {
        pStatObj->maxRunTime = runTime;
    }

    pStatObj->curRunTime    = runTime;
    pStatObj->totalRunTime += runTime;
    pStatObj->totalRunCnt++;

    return;
}


/* 将任务放入活动链表中 */
static Int32 OSA_tskPutActv(OSA_TskMonObj *pTskMonObj)
{
    OSA_listAddTail(&pTskMonObj->tskElem, &gTskModObj.actvTskList);
    return OSA_SOK;
}


/* 判断任务是否存在活动链表中 */
static Bool32 OSA_tskIsInActv(OSA_TskMonObj *pTskMonObj)
{

    OSA_TskMonObj *pTskMonObjTmp = NULL;
    OSA_ListHead  *pTskList      = NULL;
    OSA_ListHead  *pTskListUsed  = NULL;

    pTskListUsed = &gTskModObj.actvTskList;

    OSA_listForEach(pTskList, pTskListUsed)
    {
        pTskMonObjTmp = OSA_listEntry(pTskList, OSA_TskMonObj,
                                      tskElem);

        if (pTskMonObj == pTskMonObjTmp)
        {
            return OSA_TRUE;
        }
    }

    return OSA_FALSE;
}


/* 将任务放入空闲链表中 */
static Int32 OSA_tskPutIdle(OSA_TskMonObj *pTskMonObj)
{
    OSA_listAddTail(&pTskMonObj->tskElem, &gTskModObj.idleTskList);
    return OSA_SOK;
}


/* 判断任务是否存在空闲链表中 */
static Bool32 OSA_tskIsInIdle(OSA_TskMonObj *pTskMonObj)
{

    OSA_TskMonObj *pTskMonObjTmp = NULL;
    OSA_ListHead  *pTskList      = NULL;
    OSA_ListHead  *pTskListUsed  = NULL;

    pTskListUsed = &gTskModObj.idleTskList;

    OSA_listForEach(pTskList, pTskListUsed)
    {
        pTskMonObjTmp = OSA_listEntry(pTskList, OSA_TskMonObj,
                                      tskElem);
        if (pTskMonObj == pTskMonObjTmp)
        {
            return OSA_TRUE;
        }
    }

    return OSA_FALSE;
}


/* 从链表中删除任务 */
static Int32 OSA_tskDelList(OSA_TskMonObj *pTskMonObj)
{
    OSA_listDel(&pTskMonObj->tskElem);
    return OSA_SOK;
}


/* 申请任务管理对象 */
static OSA_TskMonObj *OSA_tskAllocObj(void)
{
    OSA_TskMonObj *pTskMonObj;

    pTskMonObj = OSA_memAlloc(sizeof(OSA_TskMonObj));
    if(OSA_isNull(pTskMonObj))
    {
        OSA_ERROR("Task Obj Malloc mem Fail !\n");
        return NULL;
    }

    OSA_clear(pTskMonObj);

    return pTskMonObj;
}


/* 释放任务管理对象 */
static void OSA_tskFreeObj(OSA_TskMonObj *pTskMonObj)
{
    if(OSA_SOK != OSA_memFree(pTskMonObj))
		OSA_ERROR("memory free failed\n");
}


/* 监测任务的执行函数 */
static Int32 OSA_tskMonRun(Uint32 cmd,
                           Ptr    pCmdPrms,
                           Ptr    pUsrArgs)
{
    OSA_TskModuleObj *pTskModObj = (OSA_TskModuleObj *)pUsrArgs;
    OSA_TskMonObj *pTskMonObj;
    OSA_TskObj    *pTskObj;
    OSA_ListHead  *pTskList;

    (void)cmd;
    (void)pCmdPrms;


    /* 遍历所有被监测的任务，检查是否超时。*/
    OSA_listForEach(pTskList, &pTskModObj->actvTskList)
    {
        pTskMonObj = OSA_listEntry(pTskList, OSA_TskMonObj,
                                   tskElem);
        pTskObj = &pTskMonObj->tskObj;

        /* 检查任务是否超时 */
        if (pTskObj->statObj.curRunTime
                      > pTskMonObj->nTimeOut)
        {
            OSA_WARNR("%s Task Run Time Out %ums > %ums\n",
                            pTskObj->name,
                            pTskObj->statObj.curRunTime,
                            pTskMonObj->nTimeOut);

            /* 调用用户注册的超时处理接口，以便用户处理此异常情况。*/
            if (OSA_isNotNull(pTskMonObj->OpTimeOutAck))
            {
                pTskMonObj->OpTimeOutAck(pUsrArgs);
            }
        }
    }

    return OSA_SOK;
}


/*******************************************************************************
* 函数名  : OSA_tskSendMsg
* 描  述  : 向指定的任务发消息。
* 输  入  : - hTskTo  : 目的任务句柄。
*         : - hTskFrom: 源任务句柄。
*         : - cmd     : 命令值。
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
                     Ptr pPrm)
{
    Int32 status = OSA_SOK;
    OSA_MbxHandle hMbxFrom;
    OSA_TskObj *pTskObjTo;
    OSA_TskObj *pTskObjFrom;

    OSA_assertNotNull(hTskTo);

    pTskObjTo   = (OSA_TskObj *)hTskTo;
    pTskObjFrom = (OSA_TskObj *)hTskFrom;

    if(OSA_isNull(pTskObjFrom))
    {
        hMbxFrom = NULL;
    }
    else
    {
        hMbxFrom = pTskObjFrom->hMbx;
    }

    status = OSA_mbxSendMsg(pTskObjTo->hMbx, hMbxFrom,
                            cmd, flags, pPrm);

    return status;
}


/*******************************************************************************
* 函数名  : OSA_tskBroadcastMsg
* 描  述  : 向指定数量的任务广播消息。
* 输  入  : - phTskToList  : 目的任务句柄数组，以NULL结尾。
*         : - hTskFrom     : 源任务句柄。
*         : - cmd          : 命令值。
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
                          Ptr pPrm)
{
    Int32 status = OSA_SOK;

    OSA_MbxHandle hMbxToList[OSA_MBX_BROADCAST_MAX];
    OSA_MbxHandle hMbxFrom;
    OSA_TskObj *pTskObjTo;
    OSA_TskObj *pTskObjFrom;
    Uint32 i, numMbx;

    OSA_assertNotNull(phTskToList);

    pTskObjFrom = (OSA_TskObj *)hTskFrom;

    cmd &= OSA_TSK_CMD_MASK;

    if(OSA_isNull(pTskObjFrom))
    {
        hMbxFrom = NULL;
    }
    else
    {
        hMbxFrom = pTskObjFrom->hMbx;
    }

    for(i = 0; i < OSA_MBX_BROADCAST_MAX; i++)
    {
        hMbxToList[i] = NULL;
    }

    numMbx = 0;

    while(phTskToList[numMbx] != NULL)
    {
        pTskObjTo = phTskToList[numMbx];
        hMbxToList[numMbx] = pTskObjTo->hMbx;
        numMbx++;

        /* 不能超过最大邮箱数OSA_mbx_BROADCAST_MAX */
        if(numMbx >= OSA_MBX_BROADCAST_MAX)
        {
            OSA_ERROR("Too many tasks !\n");
            return OSA_EFAIL;
        }
    }

    /* 如果用户没有传递有效的句柄，则直接返回。*/
    if(numMbx == 0)
    {
        return OSA_SOK;
    }

    status = OSA_mbxBroadcastMsg(hMbxToList, hMbxFrom, cmd, flags, pPrm);

    return status;
}


/*******************************************************************************
* 函数名  : OSA_tskFlushMsg
* 描  述  : 清除指定任务已经接收到的所有消息。
* 输  入  : - hTsk   : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskFlushMsg(OSA_TskHandle hTsk)
{
    Int32 status = OSA_SOK;
    OSA_TskObj *pTskObj;

    OSA_assertNotNull(hTsk);

    pTskObj = (OSA_TskObj *)hTsk;

    status = OSA_mbxFlush(pTskObj->hMbx);

    return status;
}


/*******************************************************************************
* 函数名  : OSA_tskStartMon
* 描  述  : 启动监测指定的任务，主要是监测任务是否超时。
* 输  入  : - hTsk   : 将要被监测的任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStartMon(OSA_TskHandle hTsk)
{
    OSA_TskMonObj *pTskMonObj = NULL;
    Bool32 isExit;

    OSA_assertNotNull(hTsk);

    pTskMonObj = (OSA_TskMonObj *)hTsk;

    OSA_mutexLock(gTskModObj.hLock);

    /* 如果已经开始则不处理。*/
    isExit = OSA_tskIsInActv(pTskMonObj);
    if (isExit)
    {
        OSA_mutexUnlock(gTskModObj.hLock);
        return OSA_SOK;
    }

    isExit = OSA_tskIsInIdle(pTskMonObj);
    if (isExit)
    {
        OSA_tskDelList(pTskMonObj);

        /* 只有当任务处于空闲队列时，才将任务加入Active队列 */
        /* 如果该任务不处于任何队列中，则说明该任务正在被删除中，
             此时不能再加入Active队列，否则会导致段错误 */
        OSA_tskPutActv(pTskMonObj);
    }

    OSA_mutexUnlock(gTskModObj.hLock);

    return OSA_SOK;
}


/*******************************************************************************
* 函数名  : OSA_tskStopMon
* 描  述  : 停止监测指定的任务。
* 输  入  : - hTsk   : 将要被监测的任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStopMon(OSA_TskHandle hTsk)
{
    OSA_TskMonObj *pTskMonObj = NULL;
    Bool32 isExit;
	Int32 status = OSA_SOK;

    OSA_assertNotNull(hTsk);

    pTskMonObj = (OSA_TskMonObj *)hTsk;

    if(OSA_isNotNull(gTskModObj.hMonTsk))
    {
        /* 先停止监测任务本身，防止同时访问，出现临界情况。*/
        status = OSA_tskStop(gTskModObj.hMonTsk);
		if(OSA_isFail(status))
	    {
	        OSA_ERROR("OSA_tskStop Fail status = %d \r\n", status);
	        return OSA_EFAIL;
	    }  
    }

    OSA_mutexLock(gTskModObj.hLock);

    /* 如果已经停止则不处理。*/
    isExit = OSA_tskIsInIdle(pTskMonObj);
    if (isExit)
    {
        goto DONE;
    }

    isExit = OSA_tskIsInActv(pTskMonObj);
    if (isExit)
    {
        OSA_tskDelList(pTskMonObj);

        /* 只有当任务处于Active队列时，才将任务加入空闲队列 */
        /* 如果该任务不处于任何队列中，则说明该任务正在被删除中，
             此时不能再加入空闲队列，否则会导致段错误 */
        OSA_tskPutIdle(pTskMonObj);		 
    }

DONE:
    OSA_mutexUnlock(gTskModObj.hLock);

    if(OSA_isNotNull(gTskModObj.hMonTsk))
    {
        status = OSA_tskStart(gTskModObj.hMonTsk);
		if(OSA_isFail(status))
	    {
	        OSA_ERROR("OSA_tskStart Fail status = %d \r\n", status);
	        return OSA_EFAIL;
	    } 
    }

    return OSA_SOK;
}


/* 通过发命令删除指定的任务，内部使用。*/
static Int32 OSA_tskCmdDelete(OSA_TskHandle hTsk)
{
    OSA_assertNotNull(hTsk);

    return OSA_tskSendMsg(hTsk, NULL,
                          OSA_TSK_CMD_DELETE,
                          OSA_MBX_WAIT_ACK,
                          NULL);
}


/*******************************************************************************
* 函数名  : OSA_tskCmdCreate
* 描  述  : 向任务发送创建命令，会调用用户注册的OpTskCreate()回调函数。
* 输  入  : - hTsk   : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskCmdCreate(OSA_TskHandle hTsk)
{
    OSA_assertNotNull(hTsk);

    return OSA_tskSendMsg(hTsk, NULL,
                          OSA_TSK_CMD_CREATE,
                          OSA_MBX_WAIT_ACK,
                          NULL);
}


/*******************************************************************************
* 函数名  : OSA_tskCmdStart
* 描  述  : 向任务发送启动命令，会调用用户注册的OpTskStart()回调函数。
* 输  入  : - hTsk   : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskCmdStart(OSA_TskHandle hTsk)
{
    OSA_assertNotNull(hTsk);

    return OSA_tskSendMsg(hTsk, NULL,
                          OSA_TSK_CMD_START,
                          OSA_MBX_WAIT_ACK,
                          NULL);
}


/*******************************************************************************
* 函数名  : OSA_tskStop
* 描  述  : 暂停任务。在等待任务停止期间会休眠。要与OSA_tskStart()接口成对使用。
* 输  入  : - hTsk  : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK:   成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStop(OSA_TskHandle hTsk)
{
    OSA_TskObj *pTskObj = (OSA_TskObj *)hTsk;

    OSA_assertNotNull(hTsk);

    OSA_mutexLock(pTskObj->hLock);

    if (OSA_TRUE == pTskObj->isActive)
    {
        pTskObj->isActive = OSA_FALSE;
        while (OSA_TRUE != pTskObj->isStop)
        {
            OSA_msleep(10);
        }

        pTskObj->isStop = OSA_FALSE;
    }

    OSA_mutexUnlock(pTskObj->hLock);

    return OSA_SOK;
}


/*******************************************************************************
* 函数名  : OSA_tskStart
* 描  述  : 重新启动任务。要与OSA_tskStop()接口成对使用，先调用它，然后调用本接口。
* 输  入  : - hTsk  : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK: 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskStart(OSA_TskHandle hTsk)
{
    OSA_TskObj *pTskObj = (OSA_TskObj *)hTsk;

    OSA_assertNotNull(hTsk);

    OSA_mutexLock(pTskObj->hLock);
    pTskObj->isActive = OSA_TRUE;
    OSA_mutexUnlock(pTskObj->hLock);

    return OSA_SOK;
}


/* 删除任务，释放所有已经分配的资源。*/
static Int32 OSA_tskFree(OSA_TskObj *pTskObj)
{
    OSA_TskMonObj *pTskMonObj    = NULL;

    pTskMonObj = (OSA_TskMonObj *)pTskObj;

    OSA_tskStopMon((OSA_TskHandle)pTskObj);

    OSA_mutexLock(gTskModObj.hLock);

    /* 从监测空闲链表中删除该Obj */
    OSA_tskDelList(pTskMonObj);

    OSA_mutexUnlock(gTskModObj.hLock);

	if (NULL != pTskObj->tskOps.OpTskDelete)
	{
    	OSA_tskCmdDelete(pTskObj);
	}

    OSA_thrDelete(pTskObj->hThr);

    OSA_mbxDelete(pTskObj->hMbx);
    OSA_mutexDelete(pTskObj->hLock);

    OSA_clear(pTskObj);

    OSA_tskFreeObj((OSA_TskMonObj *)pTskObj);

    return OSA_SOK;
}


/* 响应标准命令 */
static Int32 OSA_tskDoCmd(OSA_TskObj *pTskObj, OSA_MsgqMsg *pMsg)
{
    Int32  status = OSA_SOK;
    Uint32 cmd;

    cmd = OSA_msgGetCmd(pMsg);

    OSA_DEBUG("%s Task Recevied Cmd 0x%x !\n", pTskObj->name, cmd);

    switch (cmd)
    {

        case OSA_TSK_CMD_CREATE:

            if (OSA_isNotNull(pTskObj->tskOps.OpTskCreate))
            {
                status = pTskObj->tskOps.OpTskCreate(pTskObj->pUsrArgs);
                if(OSA_isFail(status))
                {
                    OSA_ERROR("%s Task OpTskCreate() !\n", pTskObj->name);
                    break;
                }
            }
            else
            {
                OSA_ERROR("%s Task Not Support Create CB !\n", pTskObj->name);
            }

            pTskObj->cmdFlags |= OSA_TSK_CMD_CREATE;

            OSA_mbxAckOrFreeMsg(pMsg, status);

            OSA_DEBUG("%s Task CMD CREATE !\n", pTskObj->name);

            break;

        case OSA_TSK_CMD_DELETE:

            /* 回调函数必须有效并且执行过CREATE命令。*/
            if (OSA_isNotNull(pTskObj->tskOps.OpTskDelete)
                && (pTskObj->cmdFlags & OSA_TSK_CMD_CREATE))
            {
                status = pTskObj->tskOps.OpTskDelete(pTskObj->pUsrArgs);
                if(OSA_isFail(status))
                {
                    OSA_ERROR("%s Task OpTskCreate() !\n", pTskObj->name);
                    break;
                }

                pTskObj->cmdFlags &= ~OSA_TSK_CMD_CREATE;
                pTskObj->cmdFlags |= OSA_TSK_CMD_DELETE;
            }

            OSA_mbxAckOrFreeMsg(pMsg, OSA_SOK);

            status = OSA_EFAIL;

            OSA_INFO("%s Task CMD Delete !\n", pTskObj->name);

            break;

        case OSA_TSK_CMD_START:

            if (OSA_isNotNull(pTskObj->tskOps.OpTskStart))
            {
                status = pTskObj->tskOps.OpTskStart(pTskObj->pUsrArgs);
                if(OSA_isFail(status))
                {
                    OSA_ERROR("%s Task OpTskStart() !\n", pTskObj->name);
                    break;
                }
            }
            else
            {
                OSA_ERROR("%s Task Not Support Start Cmd !\n", pTskObj->name);
            }

            pTskObj->cmdFlags |= OSA_TSK_CMD_START;

            OSA_mbxAckOrFreeMsg(pMsg, status);

            OSA_DEBUG("%s Task CMD Start !\n", pTskObj->name);

            break;

        default:

            OSA_ERROR("%s Task CMD 0x%x Not Supported !\n",
                                        pTskObj->name, cmd);
            break;
    }

    return status;
}


/* 所有任务的执行体。*/
static Int32 OSA_tskRun(Ptr pPrm)
{
    Int32  status = OSA_SOK;
    Uint32 cmd = 0;
    OSA_MsgqMsg *pMsg    = NULL;
    OSA_TskObj  *pTskObj = NULL;
    OSA_ThreadInfo info;

    OSA_assertNotNull(pPrm);

    pTskObj = (OSA_TskObj *)pPrm;

    pTskObj->statObj.tskTid = OSA_thrGetTid();
    pTskObj->statObj.tskPid = OSA_thrGetPid();

    /* 这里将记录信息替换成用户注册的回调函数信息 */
    OSA_delTidInfo(pTskObj->statObj.tskTid);

    info.tid       = pTskObj->statObj.tskTid;
    info.policy    = pTskObj->statObj.tskPol;
    info.priority  = pTskObj->statObj.tskPri;
    info.tBodyAddr = (Uint32L)(pTskObj->tskOps.OpTskRun);
	info.privData  = NULL;
    info.pName     = (Char*)(pTskObj->name);
    OSA_addTidInfo(&info);

    OSA_INFOPUP("%-10s Task   pid=%d   tid=%d\n",
                  pTskObj->name,
                  pTskObj->statObj.tskPid,
                  pTskObj->statObj.tskTid);

    /* 所有任务，一旦创建时指定了这些标志位，那么就必须使用对应的命令接口来启动。*/

    if (pTskObj->createFlags & OSA_TSK_FLAG_CREATE)
    {
        status = OSA_mbxRecvMsg(pTskObj->hMbx,
                               &pMsg,
                                OSA_TIMEOUT_FOREVER);
        if(OSA_isFail(status))
        {
            OSA_ERROR("%s Task Wait Msg !\n", pTskObj->name);
            goto LOOP_EXIT;
        }

        if (OSA_TSK_CMD_CREATE != OSA_msgGetCmd(pMsg))
        {
            OSA_WARNR("%s Task: It was't Create Cmd %u !\n",
                          pTskObj->name, OSA_msgGetCmd(pMsg));
        }

        status = OSA_tskDoCmd(pTskObj, pMsg);
        if(OSA_isFail(status))
        {
            // OSA_ERROR("%s Task Do Cmd !\n", pTskObj->name);
            goto LOOP_EXIT;
        }

        OSA_DEBUG("%s Task Create OK !\n", pTskObj->name);
    }

    if (pTskObj->createFlags & OSA_TSK_FLAG_START)
    {
        status = OSA_mbxRecvMsg(pTskObj->hMbx,
                               &pMsg,
                                OSA_TIMEOUT_FOREVER);
        if(OSA_isFail(status))
        {
            OSA_ERROR("%s Task Wait Msg !\n", pTskObj->name);
            goto LOOP_EXIT;
        }

        status = OSA_tskDoCmd(pTskObj, pMsg);
        if(OSA_isFail(status))
        {
           // OSA_ERROR("%s Task Do Cmd !\n", pTskObj->name);
            goto LOOP_EXIT;
        }

        OSA_DEBUG("%s Task Start  OK !\n", pTskObj->name);
    }

    /* 启动监测 */
    OSA_tskStartMon((OSA_TskHandle)pTskObj);

    while(!OSA_thrShouldStop())
    {
        Uint32 startTime, endTime;
        Ptr    pCmdPrms = NULL;

        /* 检查并获取消息 */
        status = OSA_mbxCheckMsg(pTskObj->hMbx, &pMsg);
        if(OSA_isSuccess(status))
        {
            cmd      = OSA_msgGetCmd(pMsg);
            pCmdPrms = pMsg->pPrm;
            OSA_DEBUG("%s Task Recevied Cmd = 0x%04x, pPrm %p\n",
                       pTskObj->name, cmd, pCmdPrms);
        }

        if(OSA_TRUE == pTskObj->isActive)
        {
             /* 获取运行时间 */
            startTime = OSA_getCurTimeInMsec();

            status = pTskObj->tskOps.OpTskRun(cmd,
                                              pCmdPrms,
                                              pTskObj->pUsrArgs);

            if(OSA_isFail(status))
            {
                OSA_ERROR("%s Task OpTskRun() !\n", pTskObj->name);
                break;
            }

            endTime = OSA_getCurTimeInMsec();

            TSK_updateStatTime(&pTskObj->statObj,
                               startTime,
                               endTime);
        }
        else
        {
            pTskObj->isStop = OSA_TRUE;
        }

        /* 检查该命令是外部命令还是内部标准命令，并采取相应的响应方式。*/
        if (cmd)
        {
            if (cmd & OSA_TSK_CMD_FLAG)
            {
                status = OSA_tskDoCmd(pTskObj, pMsg);
                if(OSA_isFail(status))
                {
                    break;
                }
            }
            else
            {
                OSA_mbxAckOrFreeMsg(pMsg, status);
                if(OSA_isFail(status))
                {
                    OSA_ERROR("%s Task Ack Msg !\n", pTskObj->name);
                    break;
                }
            }

            cmd = 0;
        }

        /* 任务创建时，如果指定了休眠时间，则休眠。*/
        if (pTskObj->nTimeSleep != OSA_TIMEOUT_NONE)
        {
            OSA_msleep(pTskObj->nTimeSleep);
        }
        else if(OSA_FALSE == pTskObj->isActive)
        {
            /* 如果被stop了，且未指定休眠时长的情况下 */
            OSA_msleep(40);
        }
    }

LOOP_EXIT:

    OSA_INFO("%s Task loop Exit !\n", pTskObj->name);
/*
 由于OSA_tskFree里面先调用OSA_tskCmdDelete，导致OSA_tskRun从while循环中退出,
 在linux内核态下，如果不进行等待就直接退出，将导致后续调用OSA_thrDelete时出错，
 2.6.18表现为挂起，2.6.37内核直接崩溃，因此在linux内核态下需要以下代码
*/
#ifdef __KERNEL__
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2, 4, 24))/* DH5000的内核是linux2.4.24 */
   while(current->vfork_done != NULL)
#else
    while(!kthread_should_stop())
#endif
    {
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();
    }
#endif

    return OSA_SOK;
}


/* 打印指定Task的统计数据 */

/*由于linux内核态不支持浮点运算，因此该函数中使用整型运算*/
#ifdef __KERNEL__
static Int32 OSA_tskPrint(OSA_TskHandle hTsk, Bool32 isTitle)
{
    OSA_TskObj     *pTskObj  = (OSA_TskObj *)hTsk;
    OSA_TskStatObj *pStatObj = &pTskObj->statObj;
    Uint32 fre, avg;

    if (isTitle)
    {
        OSA_INFOCYANR("\nTASK          PID  TID  PRI  FRE   AVG   CUR  MIN  MAX\n");
    }

    fre = (pStatObj->totalRunTime != 0) ?
            (pStatObj->totalRunCnt * 1000) / pStatObj->totalRunTime
            : 0;
    fre = fre > 999 ? 999 : fre; /* 为了显示的美观，截取到999 */

    avg = (pStatObj->totalRunCnt != 0) ?
            pStatObj->totalRunTime / pStatObj->totalRunCnt
            : 0,
    avg = avg > 999 ? 999 : avg; /* 为了显示的美观，截取到999 */


    OSA_INFOCYANR("%-12s: %-4d %-4d %-4d %-5u %-5u %-4u %-4u %-4u\n",
                pTskObj->name,
                pStatObj->tskPid,
                pStatObj->tskTid,
                pStatObj->tskPri,
                fre,
                avg,
                pStatObj->curRunTime,
                pStatObj->minRunTime,
                pStatObj->maxRunTime);

    return OSA_SOK;
}

#else
static Int32 OSA_tskPrint(OSA_TskHandle hTsk, Bool32 isTitle)
{
    OSA_TskObj     *pTskObj  = (OSA_TskObj *)hTsk;
    OSA_TskStatObj *pStatObj = &pTskObj->statObj;
    Float32 fre, avg;

    if (isTitle)
    {
        OSA_INFOCYANR("\nTASK          PID  TID  PRI  FRE   AVG   CUR  MIN  MAX\n");
    }

    fre = (pStatObj->totalRunTime != 0) ?
            (pStatObj->totalRunCnt * 1000.0f) / pStatObj->totalRunTime
            : 0.0f;
    fre = fre > 999.9f ? 999.9f : fre; /* 为了显示的美观，截取到999.9 */

    avg = (pStatObj->totalRunCnt != 0) ?
            (Float32)pStatObj->totalRunTime / pStatObj->totalRunCnt
            : 0.0f,
    avg = avg > 999.9f ? 999.9f : avg; /* 为了显示的美观，截取到999.9 */


    OSA_INFOCYANR("%-12s: %-4d %-4d %-4d %-5.1f %-5.1f %-4u %-4u %-4u\n",
                pTskObj->name,
                pStatObj->tskPid,
                pStatObj->tskTid,
                pStatObj->tskPri,
                fre,
                avg,
                pStatObj->curRunTime,
                pStatObj->minRunTime,
                pStatObj->maxRunTime);

    return OSA_SOK;
}
#endif


/*******************************************************************************
* 函数名  : OSA_tskPrintStat
* 描  述  : 打印指定Task的统计数据。
* 输  入  : - hTsk  : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK: 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskPrintStat(OSA_TskHandle hTsk)
{
    OSA_assertNotNull(hTsk);

    OSA_tskPrint(hTsk, OSA_TRUE);

    return OSA_SOK;
}


/*******************************************************************************
* 函数名  : OSA_tskPrintAllStat
* 描  述  : 打印所有Task的统计数据。
* 输  入  : - hTsk  : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK: 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskPrintAllStat(void)
{
    OSA_TskMonObj  *pTskMonObj    = NULL;
    OSA_ListHead   *pTskList      = NULL;
    OSA_ListHead   *pTskListUsed  = NULL;
    Bool32 isFirst  = OSA_TRUE;

    OSA_mutexLock(gTskModObj.hLock);

    pTskListUsed = &gTskModObj.actvTskList;

    OSA_listForEach(pTskList, pTskListUsed)
    {
        pTskMonObj = OSA_listEntry(pTskList, OSA_TskMonObj,
                                   tskElem);
        OSA_tskPrint((OSA_TskHandle)&pTskMonObj->tskObj, isFirst);
        isFirst = OSA_FALSE;
    }

    OSA_mutexUnlock(gTskModObj.hLock);

    return OSA_SOK;
}


/*******************************************************************************
* 函数名  : OSA_tskCreate
* 描  述  : 创建任务。
* 输  入  : - pCreate : 创建参数，不能为NULL。
* 输  出  : - phTsk   : 任务句柄指针，输出给调用者使用。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskCreate(OSA_TskCreate *pCreate,
                    OSA_TskHandle *phTsk)
{
    Int32 status = OSA_SOK;
    OSA_TskObj    *pTskObj    = NULL;
    OSA_TskMonObj *pTskMonObj = NULL;
    OSA_ThrCreate thrCtreate;

    OSA_assertNotNull(pCreate);
    OSA_assertNotNull(pCreate->pName);
    OSA_assertNotNull(pCreate->tskOps.OpTskRun);
    OSA_assertNotNull(phTsk);

    if ((NULL == pCreate)
        || (NULL == pCreate->pName)
        || (NULL == pCreate->tskOps.OpTskRun)
        || (NULL == phTsk))
    {
        OSA_ERROR("Input Parms NULL !\n");
        return OSA_EFAIL;
    }

    if ((Uint32)OSA_TIMEOUT_FOREVER == pCreate->nTimeSleep)
    {
        OSA_ERROR("nTimeSleep was TIMEOUT FOREVER !\n");
        return OSA_EFAIL;
    }

    if (OSA_TIMEOUT_NONE == pCreate->nTimeOut)
    {
        OSA_ERROR("nTimeSleep was TIMEOUT NONE !\n");
        return OSA_EFAIL;
    }

    pTskMonObj = OSA_tskAllocObj();

    OSA_assertNotNull(pTskMonObj);

    if (OSA_isNull(pTskMonObj))
    {
        OSA_ERROR("Could not Get Tsk Obj !\n");
        return OSA_EFAIL;
    }

    pTskObj= &pTskMonObj->tskObj;

    pTskObj->tskOps      = pCreate->tskOps;

    pTskObj->nTimeSleep  = pCreate->nTimeSleep;

    pTskObj->pUsrArgs    = pCreate->pUsrArgs;

    pTskObj->createFlags = pCreate->createFlags;

    strncpy((Char *)pTskObj->name,
            (const Char *)pCreate->pName,
             sizeof(pTskObj->name) - 1);

    pTskObj->statObj.tskPri = (Uint16)pCreate->tskPri;

    /* 初始化为一个比较大的值。 */
    pTskObj->statObj.minRunTime = 1000;

    if (pCreate->createFlags & OSA_TSK_FLAG_CREATE)
    {
        OSA_assertNotNull(pTskObj->tskOps.OpTskCreate);

        if (OSA_isNull(pTskObj->tskOps.OpTskCreate))
        {
            OSA_ERROR("%s Task Create NULL !\n", pTskObj->name);
            goto ERR_DONE;
        }
    }

    if (pCreate->createFlags & OSA_TSK_FLAG_START)
    {
        OSA_assertNotNull(pTskObj->tskOps.OpTskStart);

        if (OSA_isNull(pTskObj->tskOps.OpTskStart))
        {
            OSA_ERROR("%s Task Create NULL !\n", pTskObj->name);
            goto ERR_DONE;
        }
    }

    OSA_mbxCreate(pCreate->nMsgNum, &pTskObj->hMbx);

	status = OSA_mutexCreate(OSA_MUTEX_NORMAL, &pTskObj->hLock);
	if (OSA_isFail(status))
	{
		OSA_ERROR("create lock failed\n");
		goto ERR_DONE;
	}

    /* 设置监测方面的参数 */
    pTskMonObj->nTimeOut     = pCreate->nTimeOut;
    pTskMonObj->OpTimeOutAck = pCreate->OpTimeOutAck;

    OSA_listHeadInit(&pTskMonObj->tskElem);

    /* 不启动监测 */
    OSA_tskPutIdle(pTskMonObj);

    /* isStop和isActive默认初始值都设置为True,是为了避免在创建任务时,
    * 还没有正式进入循环体，就调用了stop，导致OSA_tskStop无法正常退出,
    * 大致流程为:某些地方地方调用start，然后么有正式进入while循环，直接,
    * 调用OSA_tskStop，导致OSA_tskStop条件得不到满足，无法退出 */
    pTskObj->isStop         = OSA_TRUE;
    pTskObj->isActive       = OSA_TRUE;

    thrCtreate.OpThrRun   = OSA_tskRun;
    thrCtreate.thrPol     = (Uint16)pCreate->tskPol;
    thrCtreate.thrPri     = (Uint16)pCreate->tskPri;
    thrCtreate.stackSize  = pCreate->stackSize;
    thrCtreate.pStackAddr = pCreate->pStackAddr;
    thrCtreate.pUsrArgs   = pTskObj;

    status = OSA_thrCreate(&thrCtreate, &pTskObj->hThr);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Could not Ctreat Thread %s !\n", pTskObj->name);
        goto ERR_DONE;
    }

    *phTsk = (OSA_TskHandle)pTskMonObj;

    return OSA_SOK;

ERR_DONE:

    if (OSA_isNotNull(pTskObj->hMbx))
    {
        OSA_mbxDelete(pTskObj->hMbx);
    }

    if (OSA_isNotNull(pTskObj->hLock))
    {
        OSA_mutexDelete(pTskObj->hLock);
    }

    OSA_tskFreeObj(pTskMonObj);

    return OSA_EFAIL;
}


/*******************************************************************************
* 函数名  : OSA_tskCreate
* 描  述  : 销毁任务。
* 输  入  : - hTsk : 任务句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_tskDelete(OSA_TskHandle hTsk)
{

    OSA_TskObj *pTskObj;

    OSA_assertNotNull(hTsk);

    pTskObj = (OSA_TskObj *)hTsk;

    OSA_tskFree(pTskObj);

    return OSA_SOK;
}


/* 初始化osa_tsk模块，根据isStartMonTask觉得是否创建监测任务。内部使用。*/
Int32 OSA_tskInit(Bool32 isStartMonTask)
{
    Int32 status = OSA_SOK;
    OSA_TskCreate tskCreate;

    OSA_listHeadInit(&gTskModObj.actvTskList);
    OSA_listHeadInit(&gTskModObj.idleTskList);

    status = OSA_mutexCreate(OSA_MUTEX_RECURSIVE, &gTskModObj.hLock);
    if (OSA_isFail(status))
    {
        OSA_ERROR("create lock failed\n");
        return OSA_EFAIL;
    }

    /* 是否开启监测任务，用于监测其他任务是否超时及其他异常情况。*/
    if (isStartMonTask)
    {
        OSA_clear(&tskCreate);

        tskCreate.pName           = "Monitor";
        tskCreate.createFlags     = OSA_TSK_FLAG_NONE;
        tskCreate.tskOps.OpTskRun = OSA_tskMonRun;
        tskCreate.tskPol          = OSA_SCHED_OTHER;
        #ifdef __RTEMS__
        tskCreate.tskPri          = (Uint32)OSA_THR_PRI_DEFAULT(OSA_SCHED_OTHER); // 若优先级太低线程跑不起来
        #else
        tskCreate.tskPri          = (Uint32)OSA_THR_PRI_MIN(OSA_SCHED_OTHER);
        #endif
        tskCreate.stackSize    = OSA_THR_STACK_SIZE_DEFAULT;
        tskCreate.nMsgNum         = 2;
        tskCreate.nTimeSleep      = 2000;
        tskCreate.OpTimeOutAck    = NULL;
        tskCreate.nTimeOut        = (Uint32)OSA_TIMEOUT_FOREVER;
        tskCreate.pUsrArgs        = &gTskModObj;

        status = OSA_tskCreate(&tskCreate, &gTskModObj.hMonTsk);
        if (OSA_isFail(status))
        {
            OSA_ERROR("%s Task Create Fail !\n", tskCreate.pName);
            return OSA_EFAIL;
        }
    }

    return OSA_SOK;
}


/* 销毁osa_tsk模块。*/
Int32 OSA_tskDeinit(void)
{
    OSA_TskMonObj *pTskMonObj    = NULL;
    OSA_ListHead  *pTskList      = NULL;
    OSA_ListHead  *pTskListUsed  = NULL;
    OSA_ListHead  *pListTmp      = NULL;

    /* 先销毁监测任务 */
    if (OSA_isNotNull(gTskModObj.hMonTsk))
    {
        OSA_tskDelete(gTskModObj.hMonTsk);
        gTskModObj.hMonTsk = NULL;
    }

    OSA_mutexLock(gTskModObj.hLock);

    /* 接着遍历两个链表，销毁所有已经创建的任务。*/

    pTskListUsed = &gTskModObj.actvTskList;

    OSA_listForEachSafe(pTskList, pListTmp, pTskListUsed)
    {
        pTskMonObj = OSA_listEntry(pTskList, OSA_TskMonObj,
                                   tskElem);
        OSA_tskFree(&pTskMonObj->tskObj);
    }

    pTskListUsed = &gTskModObj.idleTskList;

    OSA_listForEachSafe(pTskList, pListTmp, pTskListUsed)
    {
        pTskMonObj = OSA_listEntry(pTskList, OSA_TskMonObj,
                                   tskElem);
        OSA_tskFree(&pTskMonObj->tskObj);
    }

    OSA_mutexUnlock(gTskModObj.hLock);

    OSA_mutexDelete(gTskModObj.hLock);
    gTskModObj.hLock = NULL;

    return OSA_SOK;
}

#ifdef __KERNEL__

EXPORT_SYMBOL(OSA_tskSendMsg);
EXPORT_SYMBOL(OSA_tskBroadcastMsg);
EXPORT_SYMBOL(OSA_tskFlushMsg);
EXPORT_SYMBOL(OSA_tskStartMon);
EXPORT_SYMBOL(OSA_tskStopMon);
EXPORT_SYMBOL(OSA_tskCmdCreate);
EXPORT_SYMBOL(OSA_tskCmdStart);
EXPORT_SYMBOL(OSA_tskStop);
EXPORT_SYMBOL(OSA_tskStart);
EXPORT_SYMBOL(OSA_tskCreate);
EXPORT_SYMBOL(OSA_tskDelete);
EXPORT_SYMBOL(OSA_tskPrintStat);
EXPORT_SYMBOL(OSA_tskPrintAllStat);

#endif


