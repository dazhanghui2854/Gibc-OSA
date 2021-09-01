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
/*                             ͷ�ļ���                                       */
/* ========================================================================== */



#ifdef __cplusplus
extern "C" {
#endif

#if defined(__KERNEL__)

/************************************** Linux kernel ***************************************/

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/*��������ƽ̨���ڴ������ͷŽӿڣ������ں�̬��ʹ��kmalloc GFP_KERNEL��ʽ����*/
#define OSA_memAlloc(size)              OSA_memKAlloc((size), OSA_KMEM_KMA)
#define OSA_memCalloc(size)             OSA_memKCalloc((size), OSA_KMEM_KMA)
#define OSA_memFree(ptr)                OSA_memKFree((ptr))
#define OSA_memCpySize(dst,src,size)    memcpy((dst), (src), (size))
#define OSA_memCpy(dst, src)            memcpy((dst), (src), sizeof(*(src)))


/*������ڴ�����*/
typedef enum
{
    OSA_KMEM_VMA  = 0,        /* ͨ��vmalloc������ڴ�*/
    OSA_KMEM_KMA,             /* ͨ��kmalloc������ڴ�,flagsΪ GFP_KERNEL*/
    OSA_KMEM_KMA_DMA,         /* ͨ��kmalloc������ڴ�,flagsΪ GFP_KERNEL|GFP_DMA*/
    OSA_KMEM_KMA_NOSLEEP,     /* ͨ��kmalloc������ڴ�,flagsΪ GFP_ATOMIC*/
    OSA_KMEM_KMA_NOSLEEP_DMA, /* ͨ��kmalloc������ڴ�,flagsΪ GFP_ATOMIC|GFP_DMA*/
    OSA_KMEM_PAGE,            /* ͨ��__get_free_pages������ڴ�,flagsΪ GFP_KERNEL*/
    OSA_KMEM_PAGE_DMA,        /* ͨ��__get_free_pages������ڴ�,flagsΪ GFP_KERNEL|GFP_DMA*/
    OSA_KMEM_PAGE_NOSLEEP,    /* ͨ��__get_free_pages������ڴ�,flagsΪ GFP_ATOMIC*/
    OSA_KMEM_PAGE_NOSLEEP_DMA /* ͨ��__get_free_pages������ڴ�,flagsΪ GFP_ATOMIC|GFP_DMA*/

} OSA_KMemType;

/*����������ʹ���Զ�����뷽ʽ�����ڴ�ĺ�, �������*/
#define OSA_memKAlloc(size, type) \
    OSA_memKAllocAlign((size), (type), 0)

#define OSA_memKCalloc(size, type) \
    OSA_memKCallocAlign((size), (type), 0)


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
* ������  : OSA_memKAllocAlign
* ��  ��  : �ú������������ڴ�,���ݲ�ͬ������,���ò�ͬ���ں����뺯��
*           �����typeΪ��������֮һ,��ýӿڿ������ں�̬�ж������ĵ���
*           OSA_KMEM_KMA_NOSLEEP
*           OSA_KMEM_KMA_NOSLEEP_DMA
*           OSA_KMEM_PAGE_NOSLEEP
*           OSA_KMEM_PAGE_NOSLEEP_DMA
*           �������,�ýӿڲ������ں�̬�ж������ĵ���
*
* ��  ��  : - size:  Ҫ������ڴ��С
*           - type:  ������ڴ�����,����μ�OSA_KMemType
*           - align: �û��Զ��������ֽ���, ��Ϊ0��ʾ�������Զ������
*                    �ò�������Ϊ4��������, ������������NULL
*
* ��  ��  : �ޡ�
* ����ֵ  : ��NULL: ����ɹ�
*           NULL:   ����ʧ��
*******************************************************************************/
Ptr OSA_memKAllocAlign(Uint32 size, Uint32 type, Uint32 align);


/*******************************************************************************
* ������  : OSA_memKCallocAlign
* ��  ��  : �ú�����OSA_memKAllocAlign�Ļ�����,���Ӷ��ڴ���0�Ķ���
*           �����typeΪ��������֮һ,��ýӿڿ������ں�̬�ж������ĵ���
*           OSA_KMEM_KMA_NOSLEEP
*           OSA_KMEM_KMA_NOSLEEP_DMA
*           OSA_KMEM_PAGE_NOSLEEP
*           OSA_KMEM_PAGE_NOSLEEP_DMA
*           �������,�ýӿڲ������ں�̬�ж������ĵ���
*
* ��  ��  : - size:  Ҫ������ڴ��С
*           - type:  ������ڴ�����,����μ�OSA_KMemType
*           - align: �û��Զ��������ֽ���, ��Ϊ0��ʾ�������Զ������
*                    �ò�������Ϊ4��������, ������������NULL
*
* ��  ��  : �ޡ�
* ����ֵ  : ��NULL:  ����ɹ�
*           NULL:    ����ʧ��
*******************************************************************************/
Ptr OSA_memKCallocAlign(Uint32 size, Uint32 type, Uint32 align);


/*******************************************************************************
* ������  : OSA_memKFree
* ��  ��  : �ú��������ͷ��ڴ�,���ݲ�ͬ������,���ò�ͬ���ں��ͷź���
*           ��������ڴ�ΪOSA_KMEM_VMA, ��ýӿڲ������ں�̬�ж������ĵ��ã�����������ԡ�
*
* ��  ��  : - pPtr:    Ҫ�ͷŵ��ڴ�ָ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�,�ڴ����ͷ�
*           OSA_EFAIL: ʧ��, �ڴ�δ�ͷ�
*******************************************************************************/
Int32 OSA_memKFree(Ptr pPtr);

/*******************************************************************************
* ������  : OSA_memPhyToVirt
* ��  ��  : �ú�������������ַ�������ַ��ת��
*           ��������ַ�Ƿ�, �򷵻�OSA_EFAIL
*           �ýӿڿ������ں�̬�ж������ĵ���
*
* ��  ��  : - phyAddr:   �����ַ
*
* ��  ��  : - pVirtAddr: �����ַ
* ����ֵ  :  OSA_SOK:    �ɹ�
*            OSA_EFAIL:  ʧ��
*******************************************************************************/
Int32 OSA_memPhyToVirt(Uint32L phyAddr, Uint32L *pVirtAddr);

/*******************************************************************************
* ������  : OSA_memVirtToPhy
* ��  ��  : �ú�������������ַ�������ַ��ת��
*           ��������ַ�Ƿ�, �򷵻�OSA_EFAIL
*           �ýӿڿ������ں�̬�ж������ĵ���
*
* ��  ��  : - virtAddr: �����ַ
*
* ��  ��  : - pPhyAddr: �����ַ
* ����ֵ  :  OSA_SOK:   �ɹ�
*            OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_memVirtToPhy(Uint32L virtAddr, Uint32L *pPhyAddr);

/*******************************************************************************
* ������  : OSA_memCopyFromUser
* ��  ��  : �ú���������û��ռ俽�����ݵ��ں˿ռ�
*
* ��  ��  : - pKBuf: �ں˿ռ�buffer
*           - pUBuf: �û��ռ�buffer
*           - size:  ���������ݳ���
*
* ��  ��  : ��
* ����ֵ  :  OSA_SOK:   �ɹ�
*            OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_memCopyFromUser(Ptr pKBuf, Ptr pUBuf, Uint32 size);

/*******************************************************************************
* ������  : OSA_memCopyToUser
* ��  ��  : �ú���������ں˿ռ俽�����ݵ��û��ռ�
*
* ��  ��  : - pUBuf: �û��ռ�buffer
*           - pKBuf: �ں˿ռ�buffer
*           - size:  ���������ݳ���
*
* ��  ��  : ��
* ����ֵ  :  OSA_SOK:   �ɹ�
*            OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_memCopyToUser(Ptr pUBuf, Ptr pKBuf, Uint32 size);

#elif defined (___DSPBIOS___)

/************************************** TI SYSBIOS *************************/

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
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
/*                          ����������                                        */
/* ========================================================================== */
/*******************************************************************************
* ������  : OSA_memTIAllocAlign
* ��  ��  : �ú���������TI SYSBIOS�������ڴ�
*
* ��  ��  : - hHeap:  Heap�������Ϊ-1, ���ʾʹ�ñ�׼��malloc
*           - size:  Ҫ������ڴ��С
*           - align: �û��Զ��������ֽ���, ��Ϊ0��ʾ�������Զ������
*                    �ò�������Ϊ4��������, ������������NULL
*
* ��  ��  : �ޡ�
* ����ֵ  : ��NULL: ����ɹ�
*           NULL:   ����ʧ��
*******************************************************************************/
Ptr OSA_memTIAllocAlign(Handle hHeap, Uint32 size, Uint32 align);

/*******************************************************************************
* ������  : OSA_memTICallocAlign
* ��  ��  : �ú�����OSA_memTIAllocAlign�Ļ�����,���Ӷ��ڴ���0�Ķ���
*
* ��  ��  : - hHeap:  Heap�������Ϊ-1, ���ʾʹ�ñ�׼��malloc
*           - size:  Ҫ������ڴ��С
*           - align: �û��Զ��������ֽ���, ��Ϊ0��ʾ�������Զ������
*                    �ò�������Ϊ4��������, ������������NULL
*
* ��  ��  : �ޡ�
* ����ֵ  : ��NULL:  ����ɹ�
*           NULL:    ����ʧ��
*******************************************************************************/
Ptr OSA_memTICallocAlign(Handle hHeap, Uint32 size, Uint32 align);


/*******************************************************************************
* ������  : OSA_memTIFree
* ��  ��  : �ú���������TI SYSBIOS���ͷ��ڴ�
*
* ��  ��  : - pPtr:    Ҫ�ͷŵ��ڴ�ָ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�,�ڴ����ͷ�
*           OSA_EFAIL: ʧ��, �ڴ�δ�ͷ�
*******************************************************************************/
Int32 OSA_memTIFree(Ptr pPtr);


#else

/************************************** linux user *************************/

/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

#define OSA_memAlloc(size)              OSA_memUAllocAlign((size), 0)
#define OSA_memCalloc(size)             OSA_memUCallocAlign((size), 0)
#define OSA_memFree(ptr)                OSA_memUFree((ptr))
#define OSA_memCpy(dst, src)            OSA_memCpySize((dst), (src), sizeof(*(src)))

/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */
/*******************************************************************************
* ������  : OSA_memUAllocAlign
* ��  ��  : �ú���������linux�û�̬�������ڴ�
*
* ��  ��  : - size:  Ҫ������ڴ��С
*           - align: �û��Զ��������ֽ���, ��Ϊ0��ʾ�������Զ������
*                    �ò�������Ϊ4��������, ������������NULL
*
* ��  ��  : �ޡ�
* ����ֵ  : ��NULL: ����ɹ�
*           NULL:   ����ʧ��
*******************************************************************************/
Ptr OSA_memUAllocAlign(Uint32 size, Uint32 align);

/*******************************************************************************
* ������  : OSA_memUCallocAlign
* ��  ��  : �ú�����OSA_memUAllocAlign�Ļ�����,���Ӷ��ڴ���0�Ķ���
*
* ��  ��  : - size:  Ҫ������ڴ��С
*           - align: �û��Զ��������ֽ���, ��Ϊ0��ʾ�������Զ������
*                    �ò�������Ϊ4��������, ������������NULL
*
* ��  ��  : �ޡ�
* ����ֵ  : ��NULL:  ����ɹ�
*           NULL:    ����ʧ��
*******************************************************************************/
Ptr OSA_memUCallocAlign(Uint32 size, Uint32 align);


/*******************************************************************************
* ������  : OSA_memUFree
* ��  ��  : �ú���������linux�û�̬���ͷ��ڴ�
*
* ��  ��  : - pPtr:    Ҫ�ͷŵ��ڴ�ָ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�,�ڴ����ͷ�
*           OSA_EFAIL: ʧ��, �ڴ�δ�ͷ�
*******************************************************************************/
Int32 OSA_memUFree(Ptr pPtr);

#ifdef __ARM_NEON__
/*******************************************************************************
* ������  : OSA_memCpySize
* ��  ��  : �ú���������linux�û�̬�½������ݿ���
* ��  ��  : - pDst: Ŀ�ĵ�ַ
*         : - pSrc: Դ��ַ
*         : - size: �������ݳ���
* ��  ��  : ��
* ����ֵ  : Ptr : ָ��Ŀ�ĵ�ַ��ָ�� 
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

