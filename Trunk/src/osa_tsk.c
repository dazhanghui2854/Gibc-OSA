/*******************************************************************************
* osa_task.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA������Taskģ�飬�������񴴽���ע�������Ƶ�һϵ�в����ӿڡ�
*              �ڲ�������һ��������񣬼�����������Ƿ�ʱ����ʱ���ӡ����ʱ
*              ʱ��ֵ�����񴴽����ɹ����к�Ĭ�ϱ����뵽����������С�
*
*       1. Ӳ��˵����
*          �ޡ�
*
*       2. ����ṹ˵����
*          �ޡ�
*
*       3. ʹ��˵����
*          һ����������ʱ��ϣ���ܿ�������������ʱ������ϣ������������������ʼ��
*          ���Ե�ģ�����������ʼ�����������ѭ��ִ�У����Ա�ģ���ṩ�˴���ӿڣ�
*          �Ǵ�������OSA_TskCreate�е�OSA_TskOps��OSA_TskCreateFlag���������ṹ��
*          �еĳ�Ա�ɿ�������Ĵ������̣���ϸ��μ�����Ա˵����һ��������
*          OSA_TskCreateFlag��ĳ����־������������񴴽��󣬵�����Ӧ������ӿڣ�
*          ��ʽ��OSA_tskCmdxxx()���������񲻻�����ִ����ȥ������һֱ�ڵȴ������
*
*          1) ���񴴽�������ʱ�ӿڵ�����������:
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
*          2) �������񣬵��ýӿ�OSA_tskDelete()��
*          3) ���ͻ�㲥��Ϣ���ֱ����OSA_tskSendMsg()��OSA_tskBroadcastMsg()��
*          4) ��ͣ�������������񣬷ֱ����OSA_tskStop()��OSA_tskStart()��
*          5) ��ӡָ�������ȫ������ͳ��ֵ���ֱ����OSA_tskPrintStat()��
*             OSA_tskPrintAllStat()��
*
*       4. ������˵����
*          �û�������cmd���λ������1��Ҳ���ǲ�����0x8XXX�����������ڲ�ʹ�á�
*
*       5. ����˵����
*          �ޡ�
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#include <osa.h>
#include <osa_priv.h>


/* �ڲ�ʹ�õı�׼����û�����ʹ�á�����ʵ�ֿ������������ʱ����*/
typedef enum
{
    OSA_TSK_CMD_FLAG   = 0x8000,
    OSA_TSK_CMD_CREATE = 0x8001,  /* ���񴴽����� */
    OSA_TSK_CMD_DELETE = 0x8002,  /* ����ɾ������ */
    OSA_TSK_CMD_START  = 0x8004,  /* ������������ */

    OSA_TSK_CMD_MASK   = 0x7fff,
} OSA_TskCmdFlags;


/* ͳ����Ϣ����ÿ����������һ��ʵ����*/
typedef struct
{
    Uint32 curRunTime;      /* Task��ǰѭ������ʱ��       */
    Uint32 minRunTime;      /* Task����ѭ����С����ʱ��   */
    Uint32 maxRunTime;      /* Task����ѭ���������ʱ��   */
    Uint32 totalRunTime;    /* Task����ѭ�����ܹ�����ʱ�� */
    Uint32 totalRunCnt;     /* Taskѭ������ */

    Uint16 tskPri;  /* Task���ȼ� */
    Uint16 tskPol;  /* Task���Ȳ��� */
    Int32  tskTid;  /* Task�̺߳� */
    Int32  tskPid;  /* Task���̺� */
} OSA_TskStatObj;


/* ����������ÿ����������һ��ʵ����*/
typedef struct
{
    OSA_ThrHandle  hThr;

    OSA_MbxHandle  hMbx;      /* Mailbox handle */

    OSA_MutexHandle hLock;    /* ֹͣTask�õ��� */

    OSA_TskOps     tskOps;    /* �����Ӧ�Ĵ����������û�ע�ᴫ�롣*/

    /* ��������ʱ�䡣��msΪ��λ�� OSA_TIMEOUT_NONE��ʾ�����ߡ�*/
    Uint32         nTimeSleep;

    Ptr            pUsrArgs;  /* �û��Զ������ */

    /* ���񴴽����Ʊ�־���䶨���OSA_TskCreateFlag��*/
    Uint32         createFlags;

    /* �Ѿ�ִ�е������ǣ��䶨���OSA_TskCmdFlags��*/
    Uint32         cmdFlags;

    OSA_TskStatObj statObj;    /* ͳ����Ϣ */

    Int8           name[32];

    Uint32          isActive;

    Uint32          isStop;
} OSA_TskObj;


/* �����������Wrapper���������������õĳ�Ա��ÿ����������һ��ʵ����*/
typedef struct
{
    /*
      �������tskObj����������λ�ã���Ϊ�������ʹ��ǿ������ת����
      ����OSA_TskObj��OSA_TskMonObj�������͵�ָ�롣
      */
    OSA_TskObj tskObj;

    /* ����ʱʱ�䡣��msΪ��λ�� OSA_TIMEOUT_FOREVER��ʾ������ʱ��*/
    Uint32 nTimeOut;

    /* ����ʱ�����Ӧ���� */
    OSA_TskTimeOutAck OpTimeOutAck;

    /* ������������Ա������������������*/
    OSA_ListHead tskElem;
} OSA_TskMonObj;


/* ����osa_tskģ��Ĺ����������ģ��ֻ��һ��ʵ����*/
typedef struct
{
    /* һ�����񴴽���һ���������������������е�һ�����С�*/

    /* ���ڱ����״̬���������� */
    OSA_ListHead actvTskList;
    /* ����δ�����״̬������ */
    OSA_ListHead idleTskList;

    /* �������ľ��������ģ��ֻ��һ��������� */
    OSA_TskHandle hMonTsk;

    /* ���̷߳���������� */
    OSA_MutexHandle hLock;

} OSA_TskModuleObj;


/* osa_tskģ���������ʵ�� */
static OSA_TskModuleObj gTskModObj;


/* ����ͳ��ʱ�� */
static inline void TSK_updateStatTime(OSA_TskStatObj *pStatObj,
                                      Uint32 startTime,
                                      Uint32 endTime)
{
    Uint32 runTime;

    OSA_assertNotNull(pStatObj);

    /* ��������ʱ�䣬��������������*/
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


/* ��������������� */
static Int32 OSA_tskPutActv(OSA_TskMonObj *pTskMonObj)
{
    OSA_listAddTail(&pTskMonObj->tskElem, &gTskModObj.actvTskList);
    return OSA_SOK;
}


/* �ж������Ƿ���ڻ������ */
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


/* ������������������ */
static Int32 OSA_tskPutIdle(OSA_TskMonObj *pTskMonObj)
{
    OSA_listAddTail(&pTskMonObj->tskElem, &gTskModObj.idleTskList);
    return OSA_SOK;
}


/* �ж������Ƿ���ڿ��������� */
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


/* ��������ɾ������ */
static Int32 OSA_tskDelList(OSA_TskMonObj *pTskMonObj)
{
    OSA_listDel(&pTskMonObj->tskElem);
    return OSA_SOK;
}


/* �������������� */
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


/* �ͷ����������� */
static void OSA_tskFreeObj(OSA_TskMonObj *pTskMonObj)
{
    if(OSA_SOK != OSA_memFree(pTskMonObj))
		OSA_ERROR("memory free failed\n");
}


/* ��������ִ�к��� */
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


    /* �������б��������񣬼���Ƿ�ʱ��*/
    OSA_listForEach(pTskList, &pTskModObj->actvTskList)
    {
        pTskMonObj = OSA_listEntry(pTskList, OSA_TskMonObj,
                                   tskElem);
        pTskObj = &pTskMonObj->tskObj;

        /* ��������Ƿ�ʱ */
        if (pTskObj->statObj.curRunTime
                      > pTskMonObj->nTimeOut)
        {
            OSA_WARNR("%s Task Run Time Out %ums > %ums\n",
                            pTskObj->name,
                            pTskObj->statObj.curRunTime,
                            pTskMonObj->nTimeOut);

            /* �����û�ע��ĳ�ʱ����ӿڣ��Ա��û�������쳣�����*/
            if (OSA_isNotNull(pTskMonObj->OpTimeOutAck))
            {
                pTskMonObj->OpTimeOutAck(pUsrArgs);
            }
        }
    }

    return OSA_SOK;
}


/*******************************************************************************
* ������  : OSA_tskSendMsg
* ��  ��  : ��ָ����������Ϣ��
* ��  ��  : - hTskTo  : Ŀ����������
*         : - hTskFrom: Դ��������
*         : - cmd     : ����ֵ��
*         : - flags   : ��Ϣ�ı�־����������Ϣ��һЩ���ԣ�������鿴
*                       OSA_MbxMsgFlags��
*         : - pPrm    : �������Ĳ���
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
* ������  : OSA_tskBroadcastMsg
* ��  ��  : ��ָ������������㲥��Ϣ��
* ��  ��  : - phTskToList  : Ŀ�����������飬��NULL��β��
*         : - hTskFrom     : Դ��������
*         : - cmd          : ����ֵ��
*         : - flags        : ��Ϣ�ı�־����������Ϣ��һЩ���ԣ�������鿴
*                            OSA_MbxMsgFlags��
*         : - pPrm         : �������Ĳ���
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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

        /* ���ܳ������������OSA_mbx_BROADCAST_MAX */
        if(numMbx >= OSA_MBX_BROADCAST_MAX)
        {
            OSA_ERROR("Too many tasks !\n");
            return OSA_EFAIL;
        }
    }

    /* ����û�û�д�����Ч�ľ������ֱ�ӷ��ء�*/
    if(numMbx == 0)
    {
        return OSA_SOK;
    }

    status = OSA_mbxBroadcastMsg(hMbxToList, hMbxFrom, cmd, flags, pPrm);

    return status;
}


/*******************************************************************************
* ������  : OSA_tskFlushMsg
* ��  ��  : ���ָ�������Ѿ����յ���������Ϣ��
* ��  ��  : - hTsk   : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
* ������  : OSA_tskStartMon
* ��  ��  : �������ָ����������Ҫ�Ǽ�������Ƿ�ʱ��
* ��  ��  : - hTsk   : ��Ҫ��������������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_tskStartMon(OSA_TskHandle hTsk)
{
    OSA_TskMonObj *pTskMonObj = NULL;
    Bool32 isExit;

    OSA_assertNotNull(hTsk);

    pTskMonObj = (OSA_TskMonObj *)hTsk;

    OSA_mutexLock(gTskModObj.hLock);

    /* ����Ѿ���ʼ�򲻴���*/
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

        /* ֻ�е������ڿ��ж���ʱ���Ž��������Active���� */
        /* ��������񲻴����κζ����У���˵�����������ڱ�ɾ���У�
             ��ʱ�����ټ���Active���У�����ᵼ�¶δ��� */
        OSA_tskPutActv(pTskMonObj);
    }

    OSA_mutexUnlock(gTskModObj.hLock);

    return OSA_SOK;
}


/*******************************************************************************
* ������  : OSA_tskStopMon
* ��  ��  : ֹͣ���ָ��������
* ��  ��  : - hTsk   : ��Ҫ��������������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
        /* ��ֹͣ�����������ֹͬʱ���ʣ������ٽ������*/
        status = OSA_tskStop(gTskModObj.hMonTsk);
		if(OSA_isFail(status))
	    {
	        OSA_ERROR("OSA_tskStop Fail status = %d \r\n", status);
	        return OSA_EFAIL;
	    }  
    }

    OSA_mutexLock(gTskModObj.hLock);

    /* ����Ѿ�ֹͣ�򲻴���*/
    isExit = OSA_tskIsInIdle(pTskMonObj);
    if (isExit)
    {
        goto DONE;
    }

    isExit = OSA_tskIsInActv(pTskMonObj);
    if (isExit)
    {
        OSA_tskDelList(pTskMonObj);

        /* ֻ�е�������Active����ʱ���Ž����������ж��� */
        /* ��������񲻴����κζ����У���˵�����������ڱ�ɾ���У�
             ��ʱ�����ټ�����ж��У�����ᵼ�¶δ��� */
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


/* ͨ��������ɾ��ָ���������ڲ�ʹ�á�*/
static Int32 OSA_tskCmdDelete(OSA_TskHandle hTsk)
{
    OSA_assertNotNull(hTsk);

    return OSA_tskSendMsg(hTsk, NULL,
                          OSA_TSK_CMD_DELETE,
                          OSA_MBX_WAIT_ACK,
                          NULL);
}


/*******************************************************************************
* ������  : OSA_tskCmdCreate
* ��  ��  : �������ʹ������������û�ע���OpTskCreate()�ص�������
* ��  ��  : - hTsk   : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
* ������  : OSA_tskCmdStart
* ��  ��  : ���������������������û�ע���OpTskStart()�ص�������
* ��  ��  : - hTsk   : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
* ������  : OSA_tskStop
* ��  ��  : ��ͣ�����ڵȴ�����ֹͣ�ڼ�����ߡ�Ҫ��OSA_tskStart()�ӿڳɶ�ʹ�á�
* ��  ��  : - hTsk  : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK:   �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
* ������  : OSA_tskStart
* ��  ��  : ������������Ҫ��OSA_tskStop()�ӿڳɶ�ʹ�ã��ȵ�������Ȼ����ñ��ӿڡ�
* ��  ��  : - hTsk  : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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


/* ɾ�������ͷ������Ѿ��������Դ��*/
static Int32 OSA_tskFree(OSA_TskObj *pTskObj)
{
    OSA_TskMonObj *pTskMonObj    = NULL;

    pTskMonObj = (OSA_TskMonObj *)pTskObj;

    OSA_tskStopMon((OSA_TskHandle)pTskObj);

    OSA_mutexLock(gTskModObj.hLock);

    /* �Ӽ�����������ɾ����Obj */
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


/* ��Ӧ��׼���� */
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

            /* �ص�����������Ч����ִ�й�CREATE���*/
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


/* ���������ִ���塣*/
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

    /* ���ｫ��¼��Ϣ�滻���û�ע��Ļص�������Ϣ */
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

    /* ��������һ������ʱָ������Щ��־λ����ô�ͱ���ʹ�ö�Ӧ������ӿ���������*/

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

    /* ������� */
    OSA_tskStartMon((OSA_TskHandle)pTskObj);

    while(!OSA_thrShouldStop())
    {
        Uint32 startTime, endTime;
        Ptr    pCmdPrms = NULL;

        /* ��鲢��ȡ��Ϣ */
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
             /* ��ȡ����ʱ�� */
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

        /* �����������ⲿ������ڲ���׼�������ȡ��Ӧ����Ӧ��ʽ��*/
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

        /* ���񴴽�ʱ�����ָ��������ʱ�䣬�����ߡ�*/
        if (pTskObj->nTimeSleep != OSA_TIMEOUT_NONE)
        {
            OSA_msleep(pTskObj->nTimeSleep);
        }
        else if(OSA_FALSE == pTskObj->isActive)
        {
            /* �����stop�ˣ���δָ������ʱ��������� */
            OSA_msleep(40);
        }
    }

LOOP_EXIT:

    OSA_INFO("%s Task loop Exit !\n", pTskObj->name);
/*
 ����OSA_tskFree�����ȵ���OSA_tskCmdDelete������OSA_tskRun��whileѭ�����˳�,
 ��linux�ں�̬�£���������еȴ���ֱ���˳��������º�������OSA_thrDeleteʱ����
 2.6.18����Ϊ����2.6.37�ں�ֱ�ӱ����������linux�ں�̬����Ҫ���´���
*/
#ifdef __KERNEL__
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2, 4, 24))/* DH5000���ں���linux2.4.24 */
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


/* ��ӡָ��Task��ͳ������ */

/*����linux�ں�̬��֧�ָ������㣬��˸ú�����ʹ����������*/
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
    fre = fre > 999 ? 999 : fre; /* Ϊ����ʾ�����ۣ���ȡ��999 */

    avg = (pStatObj->totalRunCnt != 0) ?
            pStatObj->totalRunTime / pStatObj->totalRunCnt
            : 0,
    avg = avg > 999 ? 999 : avg; /* Ϊ����ʾ�����ۣ���ȡ��999 */


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
    fre = fre > 999.9f ? 999.9f : fre; /* Ϊ����ʾ�����ۣ���ȡ��999.9 */

    avg = (pStatObj->totalRunCnt != 0) ?
            (Float32)pStatObj->totalRunTime / pStatObj->totalRunCnt
            : 0.0f,
    avg = avg > 999.9f ? 999.9f : avg; /* Ϊ����ʾ�����ۣ���ȡ��999.9 */


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
* ������  : OSA_tskPrintStat
* ��  ��  : ��ӡָ��Task��ͳ�����ݡ�
* ��  ��  : - hTsk  : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_tskPrintStat(OSA_TskHandle hTsk)
{
    OSA_assertNotNull(hTsk);

    OSA_tskPrint(hTsk, OSA_TRUE);

    return OSA_SOK;
}


/*******************************************************************************
* ������  : OSA_tskPrintAllStat
* ��  ��  : ��ӡ����Task��ͳ�����ݡ�
* ��  ��  : - hTsk  : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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
* ������  : OSA_tskCreate
* ��  ��  : ��������
* ��  ��  : - pCreate : ��������������ΪNULL��
* ��  ��  : - phTsk   : ������ָ�룬�����������ʹ�á�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
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

    /* ��ʼ��Ϊһ���Ƚϴ��ֵ�� */
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

    /* ���ü�ⷽ��Ĳ��� */
    pTskMonObj->nTimeOut     = pCreate->nTimeOut;
    pTskMonObj->OpTimeOutAck = pCreate->OpTimeOutAck;

    OSA_listHeadInit(&pTskMonObj->tskElem);

    /* ��������� */
    OSA_tskPutIdle(pTskMonObj);

    /* isStop��isActiveĬ�ϳ�ʼֵ������ΪTrue,��Ϊ�˱����ڴ�������ʱ,
    * ��û����ʽ����ѭ���壬�͵�����stop������OSA_tskStop�޷������˳�,
    * ��������Ϊ:ĳЩ�ط��ط�����start��Ȼ��ô����ʽ����whileѭ����ֱ��,
    * ����OSA_tskStop������OSA_tskStop�����ò������㣬�޷��˳� */
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
* ������  : OSA_tskCreate
* ��  ��  : ��������
* ��  ��  : - hTsk : ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_tskDelete(OSA_TskHandle hTsk)
{

    OSA_TskObj *pTskObj;

    OSA_assertNotNull(hTsk);

    pTskObj = (OSA_TskObj *)hTsk;

    OSA_tskFree(pTskObj);

    return OSA_SOK;
}


/* ��ʼ��osa_tskģ�飬����isStartMonTask�����Ƿ񴴽���������ڲ�ʹ�á�*/
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

    /* �Ƿ�������������ڼ�����������Ƿ�ʱ�������쳣�����*/
    if (isStartMonTask)
    {
        OSA_clear(&tskCreate);

        tskCreate.pName           = "Monitor";
        tskCreate.createFlags     = OSA_TSK_FLAG_NONE;
        tskCreate.tskOps.OpTskRun = OSA_tskMonRun;
        tskCreate.tskPol          = OSA_SCHED_OTHER;
        #ifdef __RTEMS__
        tskCreate.tskPri          = (Uint32)OSA_THR_PRI_DEFAULT(OSA_SCHED_OTHER); // �����ȼ�̫���߳��ܲ�����
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


/* ����osa_tskģ�顣*/
Int32 OSA_tskDeinit(void)
{
    OSA_TskMonObj *pTskMonObj    = NULL;
    OSA_ListHead  *pTskList      = NULL;
    OSA_ListHead  *pTskListUsed  = NULL;
    OSA_ListHead  *pListTmp      = NULL;

    /* �����ټ������ */
    if (OSA_isNotNull(gTskModObj.hMonTsk))
    {
        OSA_tskDelete(gTskModObj.hMonTsk);
        gTskModObj.hMonTsk = NULL;
    }

    OSA_mutexLock(gTskModObj.hLock);

    /* ���ű��������������������Ѿ�����������*/

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


