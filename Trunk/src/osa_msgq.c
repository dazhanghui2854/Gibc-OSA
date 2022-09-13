/*******************************************************************************
* osa_msgq.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2012-7-25 Create
*
* Description: OSA消息队列。
*
*       1. 硬件说明。
*          无。
*
*       2. 程序结构说明。
*          无
*
*       3. 使用说明。
*          无。
*          
*       4. 局限性说明。
*          无。
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


typedef struct 
{
    /* 
      消息。msg必须存放在首位置，因为程序里会使用强制类型转换，
      互用OSA_MsgqMsg和OSA_MsgqMsgObj两种类型的指针。
     */
    OSA_MsgqMsg         msg;
    struct OSA_MsgqObj *pMsgqObjTo;
    struct OSA_MsgqObj *pMsgqObjFrom; 
    OSA_ListHead        msgList;
} OSA_MsgqMsgObj;


typedef struct OSA_MsgqObj
{
    OSA_QueHandle hQueEmpty;   /* 空闲消息队列 */
    OSA_QueHandle hQueRdWr;    /* 消息读写队列 */
   
    Uint32 nMsgNum;

    OSA_MsgqMsgObj *pMsgObj;                                
} OSA_MsgqObj;


Int32 OSA_msgqCreate(Uint32 nMsgNum, OSA_MsgqHandle *phMsgq)
{
    Int32         status   = OSA_SOK;
    Uint32        i;
    OSA_MsgqObj  *pMsgqObj = NULL;
    OSA_ListHead *pMsgList = NULL;
    OSA_QueCreate queCreate;

    OSA_assertNotNull(phMsgq);
    
    if (OSA_isNull(phMsgq))
    {
        OSA_ERROR("phMsgq is NULL !\n");
        return OSA_EFAIL;
    }

    /* 消息队列对象与消息对象的内存一起分配，对应的必须统一释放。*/
    pMsgqObj = OSA_memAlloc(sizeof(*pMsgqObj)
                    + nMsgNum * sizeof(OSA_MsgqMsgObj));
    if (OSA_isNull(pMsgqObj))
    {
        OSA_ERROR("Alloc mem for msgqobj fail !\n");
        return OSA_EFAIL;
    }

    OSA_clearSize(pMsgqObj, (Sizet)(sizeof(*pMsgqObj)
                  + nMsgNum * sizeof(OSA_MsgqMsgObj)));

    pMsgqObj->pMsgObj = \
        (OSA_MsgqMsgObj *)(Ptr)((Uint8 *)pMsgqObj + sizeof(*pMsgqObj));
    pMsgqObj->nMsgNum = nMsgNum;

    OSA_clear(&queCreate);

    queCreate.maxElems = (Uint16)nMsgNum;

    status |= OSA_queListCreate(&queCreate, 
                                &pMsgqObj->hQueEmpty);
    /* 读写队列成员数量是空队列的两倍 */
    queCreate.maxElems += (Uint16)nMsgNum;                          
    status |= OSA_queListCreate(&queCreate, 
                                &pMsgqObj->hQueRdWr);

	if(OSA_isFail(status))
    {
        if(OSA_SOK != OSA_memFree(pMsgqObj))
			OSA_ERROR("memory free failed\n");
        OSA_ERROR("Mag queue Create Fail = %d \r\n", status);
        return OSA_EFAIL;
    }  

    /* 初始化所有msg节点并初始化。*/
    for (i = 0; i < pMsgqObj->nMsgNum; i++)
    {
        pMsgList = &pMsgqObj->pMsgObj[i].msgList;
        OSA_listHeadInit(pMsgList);
        status = OSA_queListPut(pMsgqObj->hQueEmpty, 
                       pMsgList,
                       OSA_TIMEOUT_FOREVER);
		if(OSA_isFail(status))
	    {
	        if(OSA_SOK != OSA_memFree(pMsgqObj))
				OSA_ERROR("memory free failed\n");
	        OSA_ERROR("OSA_queListPut Fail status = %d \r\n", status);
	        return OSA_EFAIL;
	    }  
    }
	
    *phMsgq = (OSA_MsgqHandle)pMsgqObj;

    return status;
}


Int32 OSA_msgqDelete(OSA_MsgqHandle hMsgq)
{
    OSA_MsgqObj *pMsgqObj = (OSA_MsgqObj *)hMsgq;

    OSA_assertNotNull(hMsgq);
    
    if (OSA_isNull(hMsgq))
    {
        OSA_ERROR("hMsgq is NULL !\n");
        return OSA_EFAIL;
    }

    OSA_queListDelete(pMsgqObj->hQueEmpty);
    OSA_queListDelete(pMsgqObj->hQueRdWr);

    if(OSA_SOK != OSA_memFree(pMsgqObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}


Int32 OSA_msgqSend(OSA_MsgqHandle hMsgq, 
                   OSA_MsgqMsg    *pMsg, 
                   Uint32         timeout)
{
    Int32 status;
    OSA_MsgqObj    *pMsgqObj = (OSA_MsgqObj *)hMsgq;
    OSA_MsgqMsgObj *pMsgObj  = (OSA_MsgqMsgObj *)pMsg;

    OSA_assertNotNull(hMsgq);

    status = OSA_queListPut(pMsgqObj->hQueRdWr, 
                            &pMsgObj->msgList,
                            timeout);

    return status;
}


Int32 OSA_msgqSendMsg(OSA_MsgqHandle hMsgqTo, 
                      OSA_MsgqHandle hMsgqFrom, 
                      Uint16 cmd,      
                      Uint16 msgFlags, 
                      void  *pPrm,  
                      OSA_MsgqMsg **ppMsg)
{
    Int32 status = OSA_SOK;
    OSA_MsgqMsg    *pMsg;
    OSA_MsgqMsgObj *pMsgObj;
    OSA_MsgqObj    *pObjTo;
    OSA_ListHead   *pQueList;

    OSA_assertNotNull(hMsgqTo);
    OSA_assertNotNull(hMsgqFrom);
    OSA_assertNotNull(ppMsg);

    pObjTo = (OSA_MsgqObj *)hMsgqTo;

    status = OSA_queListGet(pObjTo->hQueEmpty, &pQueList,
                            (Uint32)OSA_TIMEOUT_FOREVER);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Queue Get Fail !\n");
        return OSA_EFAIL;
    }

    pMsgObj = OSA_listEntry(pQueList, OSA_MsgqMsgObj, msgList);

    pMsgObj->pMsgqObjFrom = (OSA_MsgqObj *)hMsgqFrom;
    pMsgObj->pMsgqObjTo   = (OSA_MsgqObj *)hMsgqTo;

    pMsg = &pMsgObj->msg;
    pMsg->status = OSA_SOK;
    pMsg->cmd    = cmd;
    pMsg->flags  = msgFlags;
    pMsg->pPrm   = pPrm;

    *ppMsg = pMsg;  

    status = OSA_queListPut(pObjTo->hQueRdWr, 
                            pQueList,
                            OSA_TIMEOUT_FOREVER);

    return status;
}


Int32 OSA_msgqRecvMsg(OSA_MsgqHandle hMsgq, 
                      OSA_MsgqMsg **ppMsg,
                      Uint32 timeout)
{
    Int32 status = OSA_SOK;
    OSA_MsgqObj    *pMsgqObj = (OSA_MsgqObj *)hMsgq;
    OSA_MsgqMsgObj *pMsgObj;
    OSA_ListHead   *pQueList;

    OSA_assertNotNull(hMsgq);
    OSA_assertNotNull(ppMsg);

    status = OSA_queListGet(pMsgqObj->hQueRdWr, &pQueList,
                            timeout);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Queue Get Fail !\n");
        return status;
    }
    
    pMsgObj = OSA_listEntry(pQueList, OSA_MsgqMsgObj, msgList);

    *ppMsg = &pMsgObj->msg;

    return status;
}


Bool32 OSA_msgqCheckMsg(OSA_MsgqHandle hMsgq)
{
    OSA_MsgqObj *pMsgqObj = (OSA_MsgqObj *)hMsgq;

    OSA_assertNotNull(hMsgq);
    
    return !OSA_queIsEmpty(pMsgqObj->hQueRdWr);
}


Int32 OSA_msgqSendAck(OSA_MsgqMsg *pMsg, Int32 ackRetVal)
{
    Int32 status;
    OSA_MsgqMsgObj *pMsgObj;

    OSA_assertNotNull(pMsg);

    pMsg->status = ackRetVal;

    pMsgObj = (OSA_MsgqMsgObj *)pMsg;

    status = OSA_msgqSend((OSA_MsgqHandle)pMsgObj->pMsgqObjFrom, 
                          pMsg, (Uint32)OSA_TIMEOUT_FOREVER); 
                
    return status;
}


Int32 OSA_msgqAllocMsg(OSA_MsgqMsg **ppMsg)
{
    OSA_assertNotNull(ppMsg);

    /* 实际申请的内存大小是OSA_MsgqMsgObj。*/
    *ppMsg = OSA_memAlloc(sizeof(OSA_MsgqMsgObj));    
    if (OSA_isNull(*ppMsg))
    {
        OSA_ERROR("Msg alloc fail !\n");
        return OSA_EFAIL;
    }
    
    return OSA_SOK;
}


Int32 OSA_msgqFreeMsg(OSA_MsgqMsg *pMsg)
{
    OSA_assertNotNull(pMsg);
               
    if(OSA_SOK != OSA_memFree(pMsg))
		OSA_ERROR("memory free failed\n");    
    
    return OSA_SOK;
}


Int32 OSA_msgqFreeList(OSA_MsgqMsg *pMsg)
{
    OSA_MsgqMsgObj *pMsgObj = (OSA_MsgqMsgObj *)pMsg;
    OSA_MsgqObj *pMsgqObj;

    OSA_assertNotNull(pMsg);

    pMsgqObj = pMsgObj->pMsgqObjTo;
    
    OSA_assertNotNull(pMsgqObj);
           
    return OSA_queListPut(pMsgqObj->hQueEmpty, 
                          &pMsgObj->msgList,
                          OSA_TIMEOUT_FOREVER);
}

#ifdef __KERNEL__
EXPORT_SYMBOL(OSA_msgqCreate);
EXPORT_SYMBOL(OSA_msgqDelete);
EXPORT_SYMBOL(OSA_msgqSend);
EXPORT_SYMBOL(OSA_msgqSendMsg);
EXPORT_SYMBOL(OSA_msgqRecvMsg);
EXPORT_SYMBOL(OSA_msgqCheckMsg);
EXPORT_SYMBOL(OSA_msgqSendAck);
EXPORT_SYMBOL(OSA_msgqAllocMsg);
EXPORT_SYMBOL(OSA_msgqFreeMsg);
EXPORT_SYMBOL(OSA_msgqFreeList);
#endif

