/*******************************************************************************
* osa_test_kcdev.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSAģ���ں��ַ��豸���Դ��룬�����linux�ں�̬��Ч��linux�û�̬����sysbios��ʵ��Ϊ�պ���
*
*       
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include <osa.h>
#include <osa_test.h>

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
#if defined(__KERNEL__)
#define OSA_KCDEV_NAME "osa_test"
#endif

/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            ȫ�ֱ���������                                  */
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
/*                          ����������                                        */
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

