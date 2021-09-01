/*******************************************************************************
* osa.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSAģ��ȫ�ֳ�ʼ�������ýӿڡ�
*
*       1. Ӳ��˵����
*          �ޡ�
*
*       2. ����ṹ˵����
*          ��
*
*       3. ʹ��˵����
*          �ޡ�
*
*       4. ������˵����
*          �ޡ�
*
*       5. ����˵����
*          �ޡ�
*
* Modification:
* 1. Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#include <osa.h>
#include <osa_priv.h>


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

typedef struct
{
    Uint32 nInitRefCount;
    Int32  devFd;
} OSA_gblObj;


/* ========================================================================== */
/*                            ȫ�ֱ���������                                  */
/* ========================================================================== */

static OSA_gblObj gOsaObj;


/* ========================================================================== */
/*                            ����������                                      */
/* ========================================================================== */

extern Int32 OSA_tskInit(Bool32 isStartMonTask);
extern Int32 OSA_tskDeinit(void);


/* ========================================================================== */
/*                            ����������                                      */
/* ========================================================================== */

/*******************************************************************************
* ������  : OSA_init
* ��  ��  : ��ʼ��osa��LinuxӦ�ò��SYSBIOS�ĳ�����ʹ��osa�����ӿ�ǰ�������
*           �ýӿڣ�Linux�������ص��ã�Ҳ���ò��ˣ�����û�е�����
* ��  ��  : - pInitParms: ��ʼ��������
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: У��ɹ���
*           OSA_EFAIL: У��ʧ�ܡ�
*******************************************************************************/
Int32 OSA_init(OSA_initParms *pInitParms)
{
    Int32 status = OSA_SOK;

    OSA_assertNotNull(pInitParms);

    if (OSA_isNull(pInitParms))
    {
        OSA_ERROR("pInitParms is NULL!\n");

        return OSA_EFAIL;
    }

    /* ֻ�����ʼ��һ��*/
    if (gOsaObj.nInitRefCount > 0)
    {
        gOsaObj.nInitRefCount++;
        return OSA_SOK;
    }
    gOsaObj.nInitRefCount++;

    OSA_printf("\n\n");
    OSA_INFO("OSA Build on %s \n", __DATE__, __TIME__);
    OSA_INFO("SVN NUM: %d.\n\n", SVN_NUM);

    status = OSA_tskInit(pInitParms->isStartMonTask);
    if (OSA_isFail(status))
    {
        OSA_ERROR("Could not start monitor Task !\n");
        return OSA_EFAIL;
    }
    gOsaObj.devFd = -1;
#if (defined(OS_LINUX) && !defined(__KERNEL__))
    gOsaObj.devFd = open("/dev/"OSA_THREAD_RECODE_DEV, O_SYNC | O_RDWR);
#endif

    return status;
}


/*******************************************************************************
* ������  : OSA_deinit
* ��  ��  : ����osa��������OSA_init()���ʹ�á�
* ��  ��  : �ޡ�
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_deinit(void)
{
#if (defined(OS_LINUX) && !defined(__KERNEL__))
    Int32    devFd;
#endif
    /* ֻ��������һ�Σ����û�г�ʼ�����򷵻ء�*/
    if (gOsaObj.nInitRefCount == 0)
    {
        return OSA_SOK;
    }

    gOsaObj.nInitRefCount--;

    if (gOsaObj.nInitRefCount > 0)
    {
        return OSA_SOK;
    }

#if (defined(OS_LINUX) && !defined(__KERNEL__))
    devFd = gOsaObj.devFd;
    gOsaObj.devFd = -1;
    if (devFd > 0)
    {
        close(devFd);
    }

#endif

    OSA_tskDeinit();

    return OSA_SOK;
}


/*******************************************************************************
* ������  : OSA_validate
* ��  ��  : У��OSA�汾�ļ����ԡ�
*           ͨ������ͷ�ļ�����İ汾��OSA_VERSION���ݵ��ú����ڲ�
*           ���бȽϣ�У���ͷ�ļ�����Ӧ��ļ����ԡ��ڸú����ڲ�������汾��
*           ��ȣ�ʹ����ɫ�����ӡ���汾�ţ��������ȵ��ɼ��ݣ���ʹ�û�ɫ��
*           ���ӡ��������Ϣ����������ݣ���ʹ�ú�ɫ�����ӡ������Ϣ��ǰ����
*           �������������ȷֵ�����һ��������ش���ֵ�����Ե����߱����鷵
*           ��ֵ����ȷ�ϰ汾�ļ����ԡ�
* ��  ��  : - nVersion: ͷ�ļ��ж���İ汾��OSA_VERSION��
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK: У��ɹ���
*           OSA_EFAIL: У��ʧ�ܡ�
*******************************************************************************/
Int32 OSA_validate(Uint32 nVersion)
{
    Int32 status = 0;

    if (nVersion < OSA_VERSION)
    {
        status = 2;
    }

    switch (status)
    {
        case 0:
            OSA_INFO("Head Version 0x%x Compatible \
                       Libray Version 0x%x.\n",
                       nVersion,
                       OSA_VERSION);
            status = OSA_SOK;
            break;

        case 1:
            OSA_WARNR("Head Version 0x%x Compatible \
                       Libray Version 0x%x, but not equal !\n",
                       nVersion,
                       OSA_VERSION);
            status = OSA_SOK;
            break;

        case 2:
            OSA_ERRORR("Head Version 0x%x not Compatible \
                       Libray Version 0x%x !\n",
                       nVersion,
                       OSA_VERSION);
            status = OSA_EFAIL;
            break;

        default:
            status = OSA_EFAIL;
            break;
    }

    return status;
}

#ifdef OS_LINUX
#ifdef __KERNEL__
EXPORT_SYMBOL(OSA_validate);

/* �Ǽ��߳���Ϣ */
Int32 OSA_addTidInfo(OSA_ThreadInfo* pInfo)
{
    return OSA_drvAddTriActual(pInfo, OSA_TRUE);
}
/* �Ƴ��߳���Ϣ */
Int32 OSA_delTidInfo(Uint32 tid)
{
    return OSA_drvDelTriActual(tid, OSA_TRUE);
}
/* ��ȡ�߳���Ϣ */
Int32 OSA_getTidInfo(OSA_ThreadInfo* pInfo)
{
    return OSA_drvGetTriActual(pInfo, OSA_TRUE);
}
#else
/* �Ǽ��߳���Ϣ */
Int32 OSA_addTidInfo(OSA_ThreadInfo* pInfo)
{
    OSA_RecodeTidCmdArgs   cmdArgs;
    Int32                  status = OSA_SOK;

    if (gOsaObj.devFd > 0)
    {
        OSA_clear(&cmdArgs);
        cmdArgs.tid       = pInfo->tid;
        cmdArgs.policy    = pInfo->policy;
        cmdArgs.priority  = pInfo->priority;
        cmdArgs.tBodyAddr = (Uint32L)(pInfo->tBodyAddr);
        cmdArgs.privData  = pInfo->privData;
        if (pInfo->pName != OSA_NULL)
        {
            OSA_strncpy(cmdArgs.name, pInfo->pName, OSA_THREAD_NAME_MAX_LEN);
        }
        else
        {
            OSA_strncpy(cmdArgs.name, "--", OSA_THREAD_NAME_MAX_LEN);
        }
        cmdArgs.name[OSA_THREAD_NAME_MAX_LEN-1] = '\0';

        status = ioctl(gOsaObj.devFd, OSA_RECODE_TID_ADD, &cmdArgs);
    }
    return status;
}
/* �Ƴ��߳���Ϣ */
Int32 OSA_delTidInfo(Uint32 tid)
{
    OSA_RecodeTidCmdArgs   cmdArgs;
    Int32                  status = OSA_SOK;

    if (gOsaObj.devFd > 0)
    {
        cmdArgs.tid       = tid;

        status = ioctl(gOsaObj.devFd, OSA_RECODE_TID_DEL, &cmdArgs);
    }
    return status;
}
/* ��ȡ�߳���Ϣ */
Int32 OSA_getTidInfo(OSA_ThreadInfo* pInfo)
{
    Int32                  status = OSA_EFAIL;
    OSA_RecodeTidCmdArgs   cmdArgs;

    if (gOsaObj.devFd > 0)
    {
        OSA_clear(&cmdArgs);
        cmdArgs.tid       = pInfo->tid;

        status = ioctl(gOsaObj.devFd, OSA_RECODE_TID_GET, &cmdArgs);
        if (OSA_SOK == status)
        {
            pInfo->policy    = cmdArgs.policy;
            pInfo->priority  = cmdArgs.priority;
            pInfo->tBodyAddr = cmdArgs.tBodyAddr;
            pInfo->privData  = cmdArgs.privData;
            pInfo->pName     = OSA_NULL;
        }
    }
    return status;
}
#endif
#endif

