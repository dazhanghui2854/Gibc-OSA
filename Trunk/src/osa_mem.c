/*******************************************************************************
* osa_mem.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-1 Create
*
* Desc: 定义linux用户态OSA模块对外提供的:
*       1.内存申请释放接口
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


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
/*内存数据段指针到头指针的转换*/
#define OSA_memDataToHead(pData) \
    ((OSA_MemBlockObject *)(Ptr) \
        ((Uint8 *)(pData) - sizeof(OSA_MemBlockObject)))

/*内存的魔数, 用于有效性校验*/
#define OSA_MEM_MAGIC 0x12345678UL


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/*定义申请的内存块对象信息, 为保证效率, 该结构体长度必须为32字节整数倍*/
typedef struct
{
    Uint32 nMgicNum;    /*魔数,用于校验内存块有效性。*/
    Uint32 size;        /*内存块总的大小, 包括Head和Data*/
    Ptr    pBufBase;    /*申请到的基础地址, free时使用*/

    Uint32 reserved[5];
}OSA_MemBlockObject;

/* ========================================================================== */
/*                            全局变量定义区                                  */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */



/*******************************************************************************
* 函数名  : OSA_memUAllocAlign
* 描  述  : 该函数负责在linux用户态中申请内存
*
* 输  入  : - size:  要申请的内存大小
*           - align: 用户自定义对齐的字节数, 若为0表示不进行自定义对齐
*                    该参数必须为4的整数倍, 否则函数将返回NULL
*
* 输  出  : 无。
* 返回值  : 非NULL: 申请成功
*           NULL:   申请失败
*******************************************************************************/
Ptr OSA_memUAllocAlign(Uint32 size, Uint32 align)
{
    Uint32 allocSize;
    Uint32 headSize;
    Ptr    pBufBase;
    Ptr    pBufData;
    OSA_MemBlockObject *pBufHead;

    if (0 == size)
    {
        OSA_ERROR("size must > 0\n");
        return NULL;
    }

    /*检查align 必须为4的倍数*/
    if(0 != (align & 3))
    {
        OSA_ERROR("invalid align:%d\n", align);
        return NULL;
    }

    /*计算头大小*/
    headSize = sizeof(OSA_MemBlockObject);
    
    /*总的要申请的内存大小*/
    allocSize = size + align + headSize;

    /*申请内存*/
    pBufBase = malloc(allocSize);
    if(NULL == pBufBase)
    {
        OSA_ERROR("alloc failed, size:%d, align:%d\n", 
                    size, align);
        return NULL;
    }

    /*计算数据段的位置*/
    pBufData = (Ptr)((Uint8 *)pBufBase + headSize);
    if(0 != align)
    {
        pBufData = (Ptr)OSA_align((Uint32L)pBufData, align);
    }

    /*获取头的位置*/
    pBufHead = OSA_memDataToHead(pBufData);

    /*填充头*/
    pBufHead->nMgicNum = OSA_MEM_MAGIC;
    pBufHead->pBufBase = pBufBase;
    pBufHead->size = allocSize;

    /*返回数据段指针*/
    return pBufData;
}

/*******************************************************************************
* 函数名  : OSA_memUCallocAlign
* 描  述  : 该函数在OSA_memUAllocAlign的基础上,增加对内存清0的动作
*
* 输  入  : - size:  要申请的内存大小
*           - align: 用户自定义对齐的字节数, 若为0表示不进行自定义对齐
*                    该参数必须为4的整数倍, 否则函数将返回NULL
*
* 输  出  : 无。
* 返回值  : 非NULL:  申请成功
*           NULL:    申请失败
*******************************************************************************/
Ptr OSA_memUCallocAlign(Uint32 size, Uint32 align)
{
    Ptr pPtr = OSA_memUAllocAlign(size, align);

    if(pPtr)
    {
        OSA_clearSize(pPtr, size);
    }

    return pPtr;
}

/*******************************************************************************
* 函数名  : OSA_memUFree
* 描  述  : 该函数负责在linux用户态中释放内存
*
* 输  入  : - pPtr:    要释放的内存指针
* 输  出  : 无
* 返回值  : OSA_SOK:   成功,内存已释放
*           OSA_EFAIL: 失败, 内存未释放
*******************************************************************************/
Int32 OSA_memUFree(Ptr pPtr)
{
    OSA_MemBlockObject *pBufHead;
    
    if(NULL == pPtr)
    {
        return OSA_EFAIL;
    }

    pBufHead = OSA_memDataToHead(pPtr);

    /*校验魔数*/
    if(OSA_MEM_MAGIC != pBufHead->nMgicNum)
    {
        OSA_ERROR("invalid magic:0x%x\n", pBufHead->nMgicNum);
        return OSA_EFAIL;
    }

    /*释放内存块时把魔数置为-1*/
    pBufHead->nMgicNum = (Uint32)-1;

    /*释放内存*/
    free(pBufHead->pBufBase);

    return OSA_SOK;
}

#ifdef __ARM_NEON__
Ptr OSA_memCpySize(Ptr pDst,Ptr pSrc,Uint32 size)
{
    /* 小于64B时效率不及memcpy  */
    extern Ptr OSA_memCpyNeon(Ptr pDest,Ptr pSrc,Uint32 size);
    if (size < 64)
    {
        return memcpy(pDst,pSrc,size);
    }
    return OSA_memCpyNeon(pDst,pSrc,size);
}
#endif

