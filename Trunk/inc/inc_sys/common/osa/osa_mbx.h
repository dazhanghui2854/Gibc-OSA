/*******************************************************************************
* osa_mbx.h
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


#ifndef _OSA_MBX_H_
#define _OSA_MBX_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* 消息的标志，表明消息的属性。*/
typedef enum
{
    /* 无效标志 */
    OSA_MBX_FLAG_NON = 0x0000,

    /* 等待消息回复的标志 */
    OSA_MBX_WAIT_ACK = 0x0002,

    /* 说明消息本身是Malloc的，需要被Free。*/
    OSA_MBX_FREE_MSG = 0x0004,

    /* 说明消息的pPrm是Malloc的，需要被Free。*/
    OSA_MBX_FREE_PRM = 0x0008,
} OSA_MbxMsgFlags;


/* 广播消息时，接收邮箱句柄队列的最大数量。*/
#define OSA_MBX_BROADCAST_MAX 32


typedef void * OSA_MbxHandle;


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_mbxCreate
* 描  述  : 创建邮箱。
* 输  入  : - nMsgNum: 邮箱的消息数量。
*         : - phMsgq : 返回的邮箱句柄，后续所有操作基于此句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxCreate(Uint32 nMbxNum, OSA_MbxHandle *phMbx);


/*******************************************************************************
* 函数名  : OSA_mbxDelete
* 描  述  : 删除邮箱。
* 输  入  - hMbx : 邮箱句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxDelete(OSA_MbxHandle hMbx);


/*******************************************************************************
* 函数名  : OSA_mbxSendMsg
* 描  述  : 发送消息。通过单个形式参数传递消息信息。
* 输  入  : - hMsgqTo  : 接收者的邮箱句柄。
*           - hMsgqFrom: 发送者的邮箱句柄。
*           - cmd      : 命令。
*           - flags    : 标志。其定义见OSA_MbxMsgFlags。
*           - pPrm     : 消息参数。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxSendMsg(OSA_MbxHandle hMbxTo,
                     OSA_MbxHandle hMbxFrom,
                     Uint16        cmd,
                     Uint16        flags,
                     void         *pPrm);


/*******************************************************************************
* 函数名  : OSA_mbxBroadcastMsg
* 描  述  : 广播消息。
* 输  入  : - phMbxToList  : 接收者的邮箱句柄组，包含所有接收消息的邮箱句柄。
*                            其最大数量见OSA_MBX_BROADCAST_MAX定义。
*           - hMsgqFrom    : 发送者的邮箱句柄。
*           - cmd          : 命令。
*           - flags        : 标志。其定义见OSA_MbxMsgFlags。
*           - pPrm         : 消息参数。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxBroadcastMsg(OSA_MbxHandle *phMbxToList,
                          OSA_MbxHandle  hMbxFrom,
                          Uint16         cmd,
                          Uint16         flags,
                          void          *pPrm);


/*******************************************************************************
* 函数名  : OSA_mbxRecvMsg
* 描  述  : 接收消息。
* 输  入  : - hMsgq  : 邮箱句柄。
*           - ppMsg  : 消息指针。注意是指针的指针。
*           - timeout: 超时时间。单位是毫秒。OSA_TIMEOUT_NONE表示不等待，
*                      OSA_TIMEOUT_FOREVER表示无限等待。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxRecvMsg(OSA_MbxHandle hMbx,
                     OSA_MsgqMsg **pMsg,
                     Uint32        timeOut);

/*******************************************************************************
* 函数名  : OSA_mbxRecvMsg
* 描  述  : 检查并接收消息。有无消息都立即返回，不等待。
* 输  入  : - hMsgq  : 邮箱句柄。
*           - ppMsg  : 消息指针。注意是指针的指针。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxCheckMsg(OSA_MbxHandle hMbx,
                      OSA_MsgqMsg **pMsg);


/*******************************************************************************
* 函数名  : OSA_mbxAckOrFreeMsg
* 描  述  : 发送回复并释放消息内存。
* 输  入  : - hMsgq    : 邮箱句柄。
*           - ackRetVal: 返回值。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxAckOrFreeMsg(OSA_MsgqMsg *pMsg, Int32 ackRetVal);


/*******************************************************************************
* 函数名  : OSA_mbxFlush
* 描  述  : 刷新邮箱。清除所有邮箱中的消息。
* 输  入  : - hMsgq    : 邮箱句柄。
*           - ackRetVal: 返回值。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxFlush(OSA_MbxHandle hMbx);


/*******************************************************************************
* 函数名  : OSA_mbxWaitCmd
* 描  述  : 等待指定的命令值。
* 输  入  : - hMsgq  : 邮箱句柄。
*           - pMsg   : 包含命令值的消息。
*           - waitCmd: 命令值。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_mbxWaitCmd(OSA_MbxHandle hMbx,
                     OSA_MsgqMsg **pMsg,
                     Uint16        waitCmd);

#ifdef __cplusplus
}
#endif

#endif /* _OSA_MBX_H_ */


