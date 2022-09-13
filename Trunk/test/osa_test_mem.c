/*******************************************************************************
* osa_mem.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSA模块内存测试代码
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

/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                            全局变量定义区                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                          函数声明区                                        */
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

