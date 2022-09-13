/*******************************************************************************
* osa.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA模块全局初始化和设置接口。
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
* 1. Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#include <osa.h>
#include <osa_priv.h>


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

typedef struct
{
    Uint32 nInitRefCount;
    Int32  devFd;
} OSA_gblObj;


/* ========================================================================== */
/*                            全局变量定义区                                  */
/* ========================================================================== */

static OSA_gblObj gOsaObj;


/* ========================================================================== */
/*                            函数声明区                                      */
/* ========================================================================== */

extern Int32 OSA_tskInit(Bool32 isStartMonTask);
extern Int32 OSA_tskDeinit(void);


/* ========================================================================== */
/*                            函数定义区                                      */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_init
* 描  述  : 初始化osa。Linux应用层和SYSBIOS的程序在使用osa其他接口前必须调用
*           该接口，Linux驱动不必调用，也调用不了，符号没有导出。
* 输  入  : - pInitParms: 初始化参数。
* 输  出  : 无。
* 返回值  : OSA_SOK: 校验成功。
*           OSA_EFAIL: 校验失败。
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

    /* 只允许初始化一次*/
    if (gOsaObj.nInitRefCount > 0)
    {
        gOsaObj.nInitRefCount++;
        return OSA_SOK;
    }
    gOsaObj.nInitRefCount++;

    OSA_printf("\n\n");
    OSA_INFO("OSA Build on %s \n", __DATE__, __TIME__);
    //OSA_INFO("SVN NUM: %d.\n\n", SVN_NUM);

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
* 函数名  : OSA_deinit
* 描  述  : 销毁osa。必须与OSA_init()配对使用。
* 输  入  : 无。
* 输  出  : 无。
* 返回值  : OSA_SOK: 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_deinit(void)
{
#if (defined(OS_LINUX) && !defined(__KERNEL__))
    Int32    devFd;
#endif
    /* 只允许销毁一次，如果没有初始化过则返回。*/
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
* 函数名  : OSA_validate
* 描  述  : 校验OSA版本的兼容性。
*           通过将此头文件定义的版本号OSA_VERSION传递到该函数内部
*           进行比较，校验此头文件与相应库的兼容性。在该函数内部，如果版本号
*           相等，使用绿色字体打印出版本号，如果不相等但可兼容，则使用黄色字
*           体打印出警告信息，如果不兼容，则使用红色字体打印出错信息。前两种
*           情况函数返回正确值，最后一种情况返回错误值。所以调用者必须检查返
*           回值，以确认版本的兼容性。
* 输  入  : - nVersion: 头文件中定义的版本号OSA_VERSION。
* 输  出  : 无。
* 返回值  : OSA_SOK: 校验成功。
*           OSA_EFAIL: 校验失败。
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

/* 登记线程信息 */
Int32 OSA_addTidInfo(OSA_ThreadInfo* pInfo)
{
    return OSA_drvAddTriActual(pInfo, OSA_TRUE);
}
/* 移除线程信息 */
Int32 OSA_delTidInfo(Uint32 tid)
{
    return OSA_drvDelTriActual(tid, OSA_TRUE);
}
/* 获取线程信息 */
Int32 OSA_getTidInfo(OSA_ThreadInfo* pInfo)
{
    return OSA_drvGetTriActual(pInfo, OSA_TRUE);
}
#else
/* 登记线程信息 */
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
/* 移除线程信息 */
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
/* 获取线程信息 */
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

