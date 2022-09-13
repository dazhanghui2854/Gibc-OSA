/*******************************************************************************
* osa_test_kcdev.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSA模块内核字符设备测试代码，仅针对linux内核态有效，linux用户态或者sysbios下实现为空函数
*
*       
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include <osa.h>
#include <osa_test.h>

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
#if defined(__KERNEL__)
#define OSA_KCDEV_NAME "osa_test"
#endif

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            全局变量定义区                                  */
/* ========================================================================== */
#if defined(__KERNEL__)

static Int32 OSA_testKCdevIoctl(OSA_KCdevHandle hCdev, 
                          Uint32  cmd, 
                          Uint32L args);


static OSA_KCdevOps gFops = 
{
    .OpIoctl   = OSA_testKCdevIoctl,
};

static OSA_KCdevHandle ghCdev;

#endif

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

#if defined(__KERNEL__)

static Int32 OSA_testKCdevIoctl(OSA_KCdevHandle hCdev, 
                          Uint32  cmd, 
                          Uint32L args)
{
    switch(cmd)
    {
        case CMD_OSA_KCDEV_TESTPRINT:
            OSA_INFOPUP("foot_print, index:%d, current_tid:%d\n", 
                        args, OSA_thrGetTid());
            break;
        
        default:
            OSA_ERROR("Invalid cmd:%d\n", cmd);
            return OSA_EFAIL;
    }
    
    
    return OSA_SOK;
}


Int32 OSA_testKCdevBegin(void)
{
    Int32 status;
    OSA_KCdevInitParams cdevParams;

    OSA_clear(&cdevParams);
    
    cdevParams.pName      = OSA_KCDEV_NAME;
    cdevParams.pFops      = &gFops;

    status = OSA_kCdevCreate(&cdevParams, 
                             &ghCdev);
    if (OSA_isFail(status))
    {
        OSA_ERROR("OSA_kCdevCreate failed !\n");
        return OSA_EFAIL;
    }

    return OSA_SOK;
}

Int32 OSA_testKCdevEnd(void)
{
    
    OSA_kCdevDelete(ghCdev);
    return OSA_SOK;
}

#else
Int32 OSA_testKCdevBegin(void)
{
    return OSA_SOK;
}
Int32 OSA_testKCdevEnd(void)
{
    return OSA_SOK;
}

#endif

