/*******************************************************************************
* osa_mem.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc:
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_MEM_H_
#define _OSA_MEM_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */



#ifdef __cplusplus
extern "C" {
#endif

#if defined(__KERNEL__)

/************************************** Linux kernel ***************************************/

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/*兼容所有平台的内存申请释放接口，对于内核态，使用kmalloc GFP_KERNEL方式申请*/
#define OSA_memAlloc(size)              OSA_memKAlloc((size), OSA_KMEM_KMA)
#define OSA_memCalloc(size)             OSA_memKCalloc((size), OSA_KMEM_KMA)
#define OSA_memFree(ptr)                OSA_memKFree((ptr))
#define OSA_memCpySize(dst,src,size)    memcpy((dst), (src), (size))
#define OSA_memCpy(dst, src)            memcpy((dst), (src), sizeof(*(src)))


/*申请的内存类型*/
typedef enum
{
    OSA_KMEM_VMA  = 0,        /* 通过vmalloc申请的内存*/
    OSA_KMEM_KMA,             /* 通过kmalloc申请的内存,flags为 GFP_KERNEL*/
    OSA_KMEM_KMA_DMA,         /* 通过kmalloc申请的内存,flags为 GFP_KERNEL|GFP_DMA*/
    OSA_KMEM_KMA_NOSLEEP,     /* 通过kmalloc申请的内存,flags为 GFP_ATOMIC*/
    OSA_KMEM_KMA_NOSLEEP_DMA, /* 通过kmalloc申请的内存,flags为 GFP_ATOMIC|GFP_DMA*/
    OSA_KMEM_PAGE,            /* 通过__get_free_pages申请的内存,flags为 GFP_KERNEL*/
    OSA_KMEM_PAGE_DMA,        /* 通过__get_free_pages申请的内存,flags为 GFP_KERNEL|GFP_DMA*/
    OSA_KMEM_PAGE_NOSLEEP,    /* 通过__get_free_pages申请的内存,flags为 GFP_ATOMIC*/
    OSA_KMEM_PAGE_NOSLEEP_DMA /* 通过__get_free_pages申请的内存,flags为 GFP_ATOMIC|GFP_DMA*/

} OSA_KMemType;

/*定义两个不使用自定义对齐方式申请内存的宏, 方便操作*/
#define OSA_memKAlloc(size, type) \
    OSA_memKAllocAlign((size), (type), 0)

#define OSA_memKCalloc(size, type) \
    OSA_memKCallocAlign((size), (type), 0)


/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_memKAllocAlign
* 描  述  : 该函数负责申请内存,根据不同的类型,调用不同的内核申请函数
*           若入参type为以下四种之一,则该接口可以在内核态中断上下文调用
*           OSA_KMEM_KMA_NOSLEEP
*           OSA_KMEM_KMA_NOSLEEP_DMA
*           OSA_KMEM_PAGE_NOSLEEP
*           OSA_KMEM_PAGE_NOSLEEP_DMA
*           其他情况,该接口不能在内核态中断上下文调用
*
* 输  入  : - size:  要申请的内存大小
*           - type:  申请的内存类型,定义参见OSA_KMemType
*           - align: 用户自定义对齐的字节数, 若为0表示不进行自定义对齐
*                    该参数必须为4的整数倍, 否则函数将返回NULL
*
* 输  出  : 无。
* 返回值  : 非NULL: 申请成功
*           NULL:   申请失败
*******************************************************************************/
Ptr OSA_memKAllocAlign(Uint32 size, Uint32 type, Uint32 align);


/*******************************************************************************
* 函数名  : OSA_memKCallocAlign
* 描  述  : 该函数在OSA_memKAllocAlign的基础上,增加对内存清0的动作
*           若入参type为以下四种之一,则该接口可以在内核态中断上下文调用
*           OSA_KMEM_KMA_NOSLEEP
*           OSA_KMEM_KMA_NOSLEEP_DMA
*           OSA_KMEM_PAGE_NOSLEEP
*           OSA_KMEM_PAGE_NOSLEEP_DMA
*           其他情况,该接口不能在内核态中断上下文调用
*
* 输  入  : - size:  要申请的内存大小
*           - type:  申请的内存类型,定义参见OSA_KMemType
*           - align: 用户自定义对齐的字节数, 若为0表示不进行自定义对齐
*                    该参数必须为4的整数倍, 否则函数将返回NULL
*
* 输  出  : 无。
* 返回值  : 非NULL:  申请成功
*           NULL:    申请失败
*******************************************************************************/
Ptr OSA_memKCallocAlign(Uint32 size, Uint32 type, Uint32 align);


/*******************************************************************************
* 函数名  : OSA_memKFree
* 描  述  : 该函数负责释放内存,根据不同的类型,调用不同的内核释放函数
*           若申请的内存为OSA_KMEM_VMA, 则该接口不能在内核态中断上下文调用，其他情况可以。
*
* 输  入  : - pPtr:    要释放的内存指针
* 输  出  : 无
* 返回值  : OSA_SOK:   成功,内存已释放
*           OSA_EFAIL: 失败, 内存未释放
*******************************************************************************/
Int32 OSA_memKFree(Ptr pPtr);

/*******************************************************************************
* 函数名  : OSA_memPhyToVirt
* 描  述  : 该函数负责从物理地址到虚拟地址的转换
*           如果物理地址非法, 则返回OSA_EFAIL
*           该接口可以在内核态中断上下文调用
*
* 输  入  : - phyAddr:   物理地址
*
* 输  出  : - pVirtAddr: 虚拟地址
* 返回值  :  OSA_SOK:    成功
*            OSA_EFAIL:  失败
*******************************************************************************/
Int32 OSA_memPhyToVirt(Uint32L phyAddr, Uint32L *pVirtAddr);

/*******************************************************************************
* 函数名  : OSA_memVirtToPhy
* 描  述  : 该函数负责从虚拟地址到物理地址的转换
*           如果虚拟地址非法, 则返回OSA_EFAIL
*           该接口可以在内核态中断上下文调用
*
* 输  入  : - virtAddr: 虚拟地址
*
* 输  出  : - pPhyAddr: 物理地址
* 返回值  :  OSA_SOK:   成功
*            OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_memVirtToPhy(Uint32L virtAddr, Uint32L *pPhyAddr);

/*******************************************************************************
* 函数名  : OSA_memCopyFromUser
* 描  述  : 该函数负责从用户空间拷贝数据到内核空间
*
* 输  入  : - pKBuf: 内核空间buffer
*           - pUBuf: 用户空间buffer
*           - size:  拷贝的数据长度
*
* 输  出  : 无
* 返回值  :  OSA_SOK:   成功
*            OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_memCopyFromUser(Ptr pKBuf, Ptr pUBuf, Uint32 size);

/*******************************************************************************
* 函数名  : OSA_memCopyToUser
* 描  述  : 该函数负责从内核空间拷贝数据到用户空间
*
* 输  入  : - pUBuf: 用户空间buffer
*           - pKBuf: 内核空间buffer
*           - size:  拷贝的数据长度
*
* 输  出  : 无
* 返回值  :  OSA_SOK:   成功
*            OSA_EFAIL: 失败
*******************************************************************************/
Int32 OSA_memCopyToUser(Ptr pUBuf, Ptr pKBuf, Uint32 size);

#elif defined (___DSPBIOS___)

/************************************** TI SYSBIOS *************************/

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

#define OSA_memAlloc(size)   OSA_memTIAllocAlign((Handle)-1, (size), 0)
#define OSA_memCalloc(size)  OSA_memTICallocAlign((Handle)-1, (size), 0)
#define OSA_memFree(ptr)     OSA_memTIFree((ptr))

#define OSA_memCopyFromUser(pKBuf, pUBuf, size) \
                OSA_memCpySize(pKBuf, pUBuf, size)
#define OSA_memCopyToUser(pUBuf, pKBuf, size)   \
                OSA_memCpySize(pUBuf, pKBuf, size)
#define OSA_memCpySize(dst,src,size)    memcpy((dst), (src), (size))
#define OSA_memCpy(dst, src)            memcpy((dst), (src), sizeof(*(src)))

/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */
/*******************************************************************************
* 函数名  : OSA_memTIAllocAlign
* 描  述  : 该函数负责在TI SYSBIOS中申请内存
*
* 输  入  : - hHeap:  Heap句柄，若为-1, 则表示使用标准的malloc
*           - size:  要申请的内存大小
*           - align: 用户自定义对齐的字节数, 若为0表示不进行自定义对齐
*                    该参数必须为4的整数倍, 否则函数将返回NULL
*
* 输  出  : 无。
* 返回值  : 非NULL: 申请成功
*           NULL:   申请失败
*******************************************************************************/
Ptr OSA_memTIAllocAlign(Handle hHeap, Uint32 size, Uint32 align);

/*******************************************************************************
* 函数名  : OSA_memTICallocAlign
* 描  述  : 该函数在OSA_memTIAllocAlign的基础上,增加对内存清0的动作
*
* 输  入  : - hHeap:  Heap句柄，若为-1, 则表示使用标准的malloc
*           - size:  要申请的内存大小
*           - align: 用户自定义对齐的字节数, 若为0表示不进行自定义对齐
*                    该参数必须为4的整数倍, 否则函数将返回NULL
*
* 输  出  : 无。
* 返回值  : 非NULL:  申请成功
*           NULL:    申请失败
*******************************************************************************/
Ptr OSA_memTICallocAlign(Handle hHeap, Uint32 size, Uint32 align);


/*******************************************************************************
* 函数名  : OSA_memTIFree
* 描  述  : 该函数负责在TI SYSBIOS中释放内存
*
* 输  入  : - pPtr:    要释放的内存指针
* 输  出  : 无
* 返回值  : OSA_SOK:   成功,内存已释放
*           OSA_EFAIL: 失败, 内存未释放
*******************************************************************************/
Int32 OSA_memTIFree(Ptr pPtr);


#else

/************************************** linux user *************************/

/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

#define OSA_memAlloc(size)              OSA_memUAllocAlign((size), 0)
#define OSA_memCalloc(size)             OSA_memUCallocAlign((size), 0)
#define OSA_memFree(ptr)                OSA_memUFree((ptr))
#define OSA_memCpy(dst, src)            OSA_memCpySize((dst), (src), sizeof(*(src)))

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
Ptr OSA_memUAllocAlign(Uint32 size, Uint32 align);

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
Ptr OSA_memUCallocAlign(Uint32 size, Uint32 align);


/*******************************************************************************
* 函数名  : OSA_memUFree
* 描  述  : 该函数负责在linux用户态中释放内存
*
* 输  入  : - pPtr:    要释放的内存指针
* 输  出  : 无
* 返回值  : OSA_SOK:   成功,内存已释放
*           OSA_EFAIL: 失败, 内存未释放
*******************************************************************************/
Int32 OSA_memUFree(Ptr pPtr);

#ifdef __ARM_NEON__
/*******************************************************************************
* 函数名  : OSA_memCpySize
* 描  述  : 该函数负责在linux用户态下进行数据拷贝
* 输  入  : - pDst: 目的地址
*         : - pSrc: 源地址
*         : - size: 拷贝数据长度
* 输  出  : 无
* 返回值  : Ptr : 指向目的地址的指针 
*
*******************************************************************************/
Ptr OSA_memCpySize(Ptr pDst,Ptr pSrc,Uint32 size);
#else
#define OSA_memCpySize(dst, src, size)  memcpy((dst), (src), (size))
#endif



#endif



#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_MEM_H_  */

