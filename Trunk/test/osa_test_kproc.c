/*******************************************************************************
* osa_test_kproc.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-6-12 Create
*
* Desc: OSA模块内核proc测试代码，仅针对linux内核态有效，linux用户态或者sysbios下实现为空函数
*       测试步骤:
*       1) 定义一个proc文件节点cmd，
*
*          当往cmd中输入add时，将生成一个新的proc文件节点cmd_res
*          当往cmd中输入del时，将删除已创建的cmd_res节点。
*
*       2) cmd_res文件节点生成时的默认值为Hello World!,用户可以修改其中的内容
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

#define CMD_BUF_LEN 16
#define CMDRES_BUF_LEN 128


static Int32 OSA_testKProcRead(Char *pBuf, Uint32 *pCount, Ptr pUsrArgs);
static Int32 OSA_testKProcWrite(const Char *pBuf, Uint32 count, Ptr pUsrArgs);


static OSA_KProcHandle ghProc = NULL;
static OSA_KProcOps gProcOps = {OSA_testKProcRead, OSA_testKProcWrite};

static const Char *gpCmdName = "cmd";
static Char gCmdBuf[CMD_BUF_LEN] = "no cmd yet";

static const Char *gpCmdResName = "cmd_res";
static Char gCmdResBuf[CMDRES_BUF_LEN] = "Hello World!";
static OSA_KProcFileHandle ghCmdResFile = NULL;


static Int32 OSA_testKProcReadCmdRes(Char *pBuf, Uint32 *pCount)
{
    *pCount = (Uint32)OSA_snprintf(pBuf, CMD_BUF_LEN, "%s", gCmdResBuf);
    
    return OSA_SOK;
}
static Int32 OSA_testKProcWriteCmdRes(const Char *pBuf, Uint32 count)
{
    if(count >= CMD_BUF_LEN)
    {
        OSA_ERROR("Too much count :%d\n", count);
        return OSA_EFAIL;
    }
    
    OSA_snprintf(gCmdResBuf, count, "%s", pBuf);

    return OSA_SOK;
}

static Int32 OSA_testKProcReadCmd(Char *pBuf, Uint32 *pCount)
{
    *pCount = (Uint32)OSA_snprintf(pBuf, CMD_BUF_LEN, "%s", gCmdBuf);

    return OSA_SOK;
}
static Int32 OSA_testKProcWriteCmd(const Char *pBuf, Uint32 count)
{
    Int32 ret = OSA_SOK;

    if(0 == OSA_strncmp(pBuf, "add", 3))
    {
        if(NULL == ghCmdResFile)
        {
            ret = OSA_kProcAddFile(ghProc, gpCmdResName, &gProcOps, 
                    (Ptr)gpCmdResName, &ghCmdResFile);
            if(OSA_SOK != ret)
            {
                OSA_ERROR("OSA_kProcAddFile failed\n");
            }
        }
        else
        {
            OSA_ERROR("cmd_res already exist, can't add\n");
            ret = OSA_EFAIL;
        }
    }
    else if(0 == OSA_strncmp(pBuf, "del", 3))
    {
        if(ghCmdResFile)
        {
            ret = OSA_kProcDelFile(ghCmdResFile);
            if(OSA_SOK != ret)
            {
                OSA_ERROR("OSA_kProcDelFile failed\n");
            }
            else
            {
                ghCmdResFile = NULL;
            }
        }
        else
        {
            OSA_ERROR("cmd_res not exist, can't del\n");
            ret = OSA_EFAIL;
        }
    }
    else
    {
        OSA_ERROR("Invalid input cmd:%s\n", pBuf);
        ret = OSA_EFAIL;
    }

    if(ret == OSA_SOK)
    {
        OSA_snprintf(gCmdBuf, count, "%s", pBuf);
    }

    return ret;
}



static Int32 OSA_testKProcRead(Char *pBuf, Uint32 *pCount, Ptr pUsrArgs)
{
    if( pUsrArgs == (Ptr)gpCmdName )
    {
        return OSA_testKProcReadCmd(pBuf, pCount);
    }

    if ( pUsrArgs == (Ptr)gpCmdResName )
    {
        return OSA_testKProcReadCmdRes(pBuf, pCount);
    }
    
    //OSA_ERROR("Invalid pUsrArgs:0x%x\n", (Uint32)pUsrArgs);
    OSA_ERROR("Invalid pUsrArgs:0x%p\n", pUsrArgs);

    return OSA_EFAIL;
}

static Int32 OSA_testKProcWrite(const Char *pBuf, Uint32 count, Ptr pUsrArgs)
{
    if( pUsrArgs == (Ptr)gpCmdName )
    {
        return OSA_testKProcWriteCmd(pBuf, count);
    }

    if ( pUsrArgs == (Ptr)gpCmdResName )
    {
        return OSA_testKProcWriteCmdRes(pBuf, count);
    }
    
    //OSA_ERROR("Invalid pUsrArgs:0x%x\n", (Uint32)pUsrArgs);
    OSA_ERROR("Invalid pUsrArgs:0x%p\n", pUsrArgs);

    return OSA_EFAIL;
}


                                        


Int32 OSA_testKProcBegin(void)
{
    Int32 ret = OSA_SOK;

    ret = OSA_kProcCreate(OSA_MODULE_NAME, &ghProc);
    if(OSA_SOK != ret)
    {
        OSA_ERROR("Create proc Failed\n");
        return OSA_EFAIL;
    }

    ret = OSA_kProcAddFile(ghProc, gpCmdName, &gProcOps, (Ptr)gpCmdName, NULL);
    if(OSA_SOK != ret)
    {
        OSA_ERROR("OSA_kProcAddFile Failed\n");
        return OSA_EFAIL;
    }
    

    return OSA_SOK;
}


Int32 OSA_testKProcEnd(void)
{
    OSA_kProcDelete(ghProc);

    return OSA_SOK;
}

#else
Int32 OSA_testKProcBegin(void)
{
    return OSA_SOK;
}
Int32 OSA_testKProcEnd(void)
{
    return OSA_SOK;
}

#endif

