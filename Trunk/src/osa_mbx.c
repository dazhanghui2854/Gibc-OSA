/*******************************************************************************
* osa_mbx.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2012-7-25 Create
*
* Description: OSA消息邮箱。
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
    OSA_MsgqHandle hRcvMsgq;  /* Receive mailbox */
    OSA_MsgqHandle hAckMsgq;  /* ACK mailbox */
} OSA_MbxObj;


Int32 OSA_mbxCreate(Uint32 nMbxNum, OSA_MbxHandle *phMbx)
{
    Int32 status = OSA_SOK;
    OSA_MbxObj *pMbxObj;

    OSA_assertNotNull(phMbx);
    
    if (OSA_isNull(phMbx))
    {
        OSA_ERROR("phMbx is NULL !\n");
        return OSA_EFAIL;
    }

    pMbxObj = OSA_memAlloc(sizeof(*pMbxObj));
    if (OSA_isNull(pMbxObj))
    {
        OSA_ERROR("Alloc mem for msgqobj fail !\n");
        return OSA_EFAIL;
    }
    OSA_clear(pMbxObj);

    status |= OSA_msgqCreate(nMbxNum, &pMbxObj->hRcvMsgq);
    status |= OSA_msgqCreate(nMbxNum, &pMbxObj->hAckMsgq);

    if(OSA_isFail(status))
    {
        OSA_ERROR("MailBox Create Fail = %d \r\n", status);
    }

   *phMbx = (OSA_MbxHandle)pMbxObj;
    
    return status;
}


Int32 OSA_mbxDelete(OSA_MbxHandle hMbx)
{
    OSA_MbxObj *pMbxObj = (OSA_MbxObj *)hMbx;

    OSA_assertNotNull(hMbx);

    OSA_msgqDelete(pMbxObj->hRcvMsgq);
    OSA_msgqDelete(pMbxObj->hAckMsgq);

    return OSA_SOK;
}


Int32 OSA_mbxFreeMsg(OSA_MsgqMsg *pMsg)
{
    Int32 status = OSA_SOK;

    OSA_assertNotNull(pMsg);

    if(pMsg->flags & OSA_MBX_FREE_PRM) 
    {
        if(OSA_isNotNull(pMsg->pPrm)) 
        {
            if(OSA_SOK != OSA_memFree(pMsg->pPrm))
				OSA_ERROR("memory free failed\n");
        }
    }

    if(pMsg->flags & OSA_MBX_FREE_MSG) 
    {
        status = OSA_msgqFreeMsg(pMsg);
    }

    OSA_msgqFreeList(pMsg);     
    
    OSA_assertSuccess(status);

    return status;
}


Int32 OSA_mbxSendMsg(OSA_MbxHandle hMbxTo, 
                     OSA_MbxHandle hMbxFrom,
                     Uint16 cmd, 
                     Uint16 flags,
                     void  *pPrm)
{
    Bool32 waitAck;
    Int32  status = OSA_SOK;
    OSA_MsgqMsg *pSentMsg, *pRcvMsg;
    OSA_MsgqHandle hAckMsgq;
    OSA_MbxObj    *pMbxTo;

    OSA_assertNotNull(hMbxTo);

    pMbxTo = (OSA_MbxObj *)hMbxTo;

    if(OSA_isNotNull(hMbxFrom)) 
    {
        hAckMsgq = ((OSA_MbxObj *)hMbxFrom)->hAckMsgq;
    } 
    else 
    {
        if(flags & OSA_MBX_WAIT_ACK) 
        {
            /* 使用目的邮箱的消息接收队列。*/
              hAckMsgq = pMbxTo->hAckMsgq;
        }
        else
        {
            hAckMsgq = NULL;
        }        
    }

    status = OSA_msgqSendMsg(pMbxTo->hRcvMsgq, hAckMsgq, 
                             cmd, flags, pPrm, &pSentMsg);
    OSA_assertSuccess(status);

    if((flags & OSA_MBX_WAIT_ACK) 
        && OSA_isNotNull(hAckMsgq)) 
    {
        waitAck = OSA_TRUE;

        do {

            /* 等待回复 */
            status = OSA_msgqRecvMsg(hAckMsgq, &pRcvMsg, 
                                     (Uint32)OSA_TIMEOUT_FOREVER);
            if (OSA_isFail(status))
            {
                OSA_ERROR("Msgq recev !\n");
                return status;
            }

            if(pRcvMsg == pSentMsg) 
            {
                /* 得到回复，返回。*/
                waitAck = OSA_FALSE;
                status  = OSA_msgGetAckStatus(pRcvMsg);
            }
            OSA_mbxFreeMsg(pRcvMsg);

        } while(waitAck);
    }

    return status;
}


Int32 OSA_mbxBroadcastMsg(OSA_MbxHandle *phMbxToList, 
                          OSA_MbxHandle  hMbxFrom, 
                          Uint16 cmd, 
                          Uint16 flags, 
                          void  *pPrm)
{
    OSA_MsgqMsg *pSentMsgList[OSA_MBX_BROADCAST_MAX];
    Bool         ackList[OSA_MBX_BROADCAST_MAX];
    OSA_MsgqMsg    *pRcvMsg;
    OSA_MsgqHandle hAckMsgq;
    OSA_MbxObj     *pMbxObj;
    Bool waitAck, unknownAck;
    Uint32 i, numMsg;
    Int32 status = OSA_SOK, ackRetVal = OSA_SOK;

    OSA_assertNotNull(phMbxToList);
    OSA_assertNotNull(hMbxFrom);

    if(OSA_isNotNull(hMbxFrom)) 
    {        
        hAckMsgq = ((OSA_MbxObj *)hMbxFrom)->hAckMsgq;
    } else 
    {
        if(flags & OSA_MBX_WAIT_ACK) 
        {
              /* 消息接收邮箱不能为空，否则不能够接收回复。*/
            return OSA_EFAIL;
        }
        hAckMsgq = NULL;    
    }

    /* 目的邮箱计数 */
    numMsg = 0;
    
    while(NULL != phMbxToList[numMsg]) 
    {
        numMsg++;
        if(numMsg >= OSA_MBX_BROADCAST_MAX) 
        {
            return OSA_EFAIL;
        }
    }

    if(numMsg == 0) 
    {
        /* 无邮箱 */
        return OSA_EFAIL;
    }

    /* Malloc的内存，必须使用ACK方式发送。*/
    if(flags & OSA_MBX_FREE_MSG) 
    {
        if(((flags & OSA_MBX_WAIT_ACK) == 0) 
            && numMsg > 1) 
        {
            return OSA_EFAIL;
        }
    }

    for(i = 0; i < OSA_MBX_BROADCAST_MAX; i++) 
    {
        ackList[i]      = OSA_FALSE;
        pSentMsgList[i] = NULL;
    }

    /* 开始发送消息 */
    for(i = 0; i < numMsg; i++) 
    {
        pMbxObj = (OSA_MbxObj *)phMbxToList[i];
        status = OSA_msgqSendMsg(pMbxObj->hRcvMsgq, 
                                hAckMsgq, cmd, flags, 
                                pPrm, &pSentMsgList[i]);
        if (OSA_isFail(status))
        {
            return status;
        }
    }

    if((flags & OSA_MBX_WAIT_ACK) && hAckMsgq!=NULL ) 
    {
        /* 等待回复 */
        do {
            
            status = OSA_msgqRecvMsg(hAckMsgq, &pRcvMsg, (Uint32)OSA_TIMEOUT_FOREVER);
            if (OSA_isFail(status))
            {
                return status;
            }

            unknownAck = OSA_TRUE;

            /* mark ACK as received for sent MSG */
            for(i = 0; i < numMsg; i++) 
            {
                if(pRcvMsg == pSentMsgList[i]) 
                {
                    ackList[i] = OSA_TRUE;
                    unknownAck = OSA_FALSE;
                    
                    if(ackRetVal == OSA_SOK) 
                    {
                        ackRetVal = OSA_msgGetAckStatus(pRcvMsg);
                    }
                    
                    break;
                }
            }

            /* check if all ACKs received */
            waitAck = OSA_FALSE;
            
            for(i = 0 ; i < numMsg; i++) 
            {
                if(ackList[i] == OSA_FALSE) 
                {
                    waitAck = OSA_TRUE;
                    break;
                }
            }

            if(unknownAck) 
            {
                  /* ACK received is for some old message, hence free MSG and prm */
                OSA_mbxFreeMsg(pRcvMsg);
            } 
            else 
            {
                /* only free MSG now, free prm after all ACKs are received */
                if (flags & OSA_MBX_FREE_MSG)
                {
                    status = OSA_msgqFreeMsg(pRcvMsg);
                    if (OSA_isFail(status))
                    {
                        return status;
                    }
                }
            }

        } while(waitAck);

        if(flags & OSA_MBX_FREE_PRM) 
        {
            if(pPrm != NULL) 
            {
                if(OSA_SOK != OSA_memFree(pPrm))
					OSA_ERROR("memory free failed\n");
            }
        }

        status = ackRetVal;
    }

    return status;
}


Int32 OSA_mbxRecvMsg(OSA_MbxHandle hMbx, 
                     OSA_MsgqMsg **pMsg,
                     Uint32        timeOut)
{
    Int32       status;
    OSA_MbxObj *pMbxObj = (OSA_MbxObj *)hMbx;

    OSA_assertNotNull(hMbx);
    OSA_assertNotNull(pMsg);

    status = OSA_msgqRecvMsg(pMbxObj->hRcvMsgq, 
                             pMsg, 
                             timeOut);
    return status;
}


Int32 OSA_mbxCheckMsg(OSA_MbxHandle hMbx, OSA_MsgqMsg **pMsg)
{
    Int32 status = OSA_EFAIL;
    OSA_MbxObj *pMbxObj = (OSA_MbxObj *)hMbx;

    OSA_assertNotNull(hMbx);
    OSA_assertNotNull(pMsg);

    /* 先检查是否有消息，有则收取。*/
    if (OSA_msgqCheckMsg(pMbxObj->hRcvMsgq))
    {
        status = OSA_msgqRecvMsg(pMbxObj->hRcvMsgq, 
                                 pMsg, OSA_TIMEOUT_NONE);        
    }
    
    return status;
}

Int32 OSA_mbxAckOrFreeMsg(OSA_MsgqMsg *pMsg, Int32 ackRetVal)
{
    Int32 status = OSA_SOK;

    OSA_assertNotNull(pMsg);

    if(OSA_isNull(pMsg))
    {
        return OSA_EFAIL;
    }
    
    if(pMsg->flags & OSA_MBX_WAIT_ACK) 
    {
        /* ACK message */
        status = OSA_msgqSendAck(pMsg, ackRetVal);
    } else 
    {
        /* FREE message and prm */
        OSA_mbxFreeMsg(pMsg);
    }

    return status;
}


Int32 OSA_mbxFlush(OSA_MbxHandle hMbx)
{
    Int32 status;
    OSA_MsgqMsg *pMsg;
    OSA_MbxObj *pMbxObj = (OSA_MbxObj *)hMbx;

    OSA_assertNotNull(hMbx);
    
    /* flush receive mailbox */
    do 
    {
        status = OSA_mbxCheckMsg(hMbx, &pMsg);
        if(OSA_isSuccess(status)) 
        {
            OSA_mbxAckOrFreeMsg(pMsg, OSA_SOK);
        }
    } while(OSA_isSuccess(status));

    /* flush ACK mailbox */
    do 
    {
        status = OSA_msgqRecvMsg(pMbxObj->hAckMsgq, 
                                &pMsg, OSA_TIMEOUT_NONE);
        if(OSA_isSuccess(status)) 
        {
            OSA_mbxFreeMsg(pMsg);
        }
        
    } while(OSA_isSuccess(status));

    return status;
}


Int32 OSA_mbxWaitCmd(OSA_MbxHandle hMbx, 
                     OSA_MsgqMsg **pMsg, 
                     Uint16        waitCmd)
{
    OSA_MsgqMsg *pRcvMsg;

    OSA_assertNotNull(hMbx);
    OSA_assertNotNull(pMsg);

    while(1) 
    {
        OSA_mbxRecvMsg(hMbx, &pRcvMsg, OSA_TIMEOUT_FOREVER);
        if(OSA_msgGetCmd(pRcvMsg) == waitCmd)
        {
            break;
        }
        OSA_mbxAckOrFreeMsg(pRcvMsg, OSA_SOK);
    }

    if(OSA_isNull(pMsg)) 
    {
        OSA_mbxAckOrFreeMsg(pRcvMsg, OSA_SOK);
    } 
    else 
    {
        *pMsg = pRcvMsg;
    }

    return OSA_SOK;
}


#ifdef __KERNEL__
EXPORT_SYMBOL(OSA_mbxCreate);
EXPORT_SYMBOL(OSA_mbxDelete);
EXPORT_SYMBOL(OSA_mbxSendMsg);
EXPORT_SYMBOL(OSA_mbxBroadcastMsg);
EXPORT_SYMBOL(OSA_mbxRecvMsg);
EXPORT_SYMBOL(OSA_mbxCheckMsg);
EXPORT_SYMBOL(OSA_mbxAckOrFreeMsg);
EXPORT_SYMBOL(OSA_mbxFlush);
EXPORT_SYMBOL(OSA_mbxWaitCmd);

#endif

