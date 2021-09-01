/*******************************************************************************
* osa_mem.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSAģ���ڴ���Դ���
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

/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            ȫ�ֱ���������                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

#if defined(__KERNEL__)


Int32 OSA_testMemBegin(void)
{
    Int32   status;
    Int32   size = 2 * OSA_MB;
    Ptr     memTestPtr;
    
    memTestPtr = OSA_memAlloc(size);
    if(OSA_isNull(memTestPtr))
    {
        OSA_ERRORR("OSA_memAlloc failed\n");
        return  OSA_EFAIL;
    }

    OSA_INFO("OSA_memAlloc sucesss %d\n", size);
    
    OSA_msleep(100);

    status = OSA_memFree(memTestPtr);
    if(OSA_isFail(status))
    {
        OSA_ERRORR("OSA_memFree failed\n");
        return  OSA_EFAIL;
    }    

    OSA_INFO("OSA_memAlloc free sucesess %d\n", size);
 
    return OSA_SOK;
}


Int32 OSA_testMemEnd(void)
{
    
    return OSA_SOK;
}

#else

Int32 OSA_testMemBegin(void)
{
    return OSA_SOK;
}
Int32 OSA_testMemEnd(void)
{
    return OSA_SOK;
}

#endif

