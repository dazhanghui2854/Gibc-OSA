/*******************************************************************************
* osa_mem.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-1 Create
*
* Desc: ����linux�û�̬OSAģ������ṩ��:
*       1.�ڴ������ͷŽӿ�
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


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
/*�ڴ����ݶ�ָ�뵽ͷָ���ת��*/
#define OSA_memDataToHead(pData) \
    ((OSA_MemBlockObject *)(Ptr) \
        ((Uint8 *)(pData) - sizeof(OSA_MemBlockObject)))

/*�ڴ��ħ��, ������Ч��У��*/
#define OSA_MEM_MAGIC 0x12345678UL


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/*����������ڴ�������Ϣ, Ϊ��֤Ч��, �ýṹ�峤�ȱ���Ϊ32�ֽ�������*/
typedef struct
{
    Uint32 nMgicNum;    /*ħ��,����У���ڴ����Ч�ԡ�*/
    Uint32 size;        /*�ڴ���ܵĴ�С, ����Head��Data*/
    Ptr    pBufBase;    /*���뵽�Ļ�����ַ, freeʱʹ��*/

    Uint32 reserved[5];
}OSA_MemBlockObject;

/* ========================================================================== */
/*                            ȫ�ֱ���������                                  */
/* ========================================================================== */


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

    /*���align ����Ϊ4�ı���*/
    if(0 != (align & 3))
    {
        OSA_ERROR("invalid align:%d\n", align);
        return NULL;
    }

    /*����ͷ��С*/
    headSize = sizeof(OSA_MemBlockObject);
    
    /*�ܵ�Ҫ������ڴ��С*/
    allocSize = size + align + headSize;

    /*�����ڴ�*/
    pBufBase = malloc(allocSize);
    if(NULL == pBufBase)
    {
        OSA_ERROR("alloc failed, size:%d, align:%d\n", 
                    size, align);
        return NULL;
    }

    /*�������ݶε�λ��*/
    pBufData = (Ptr)((Uint8 *)pBufBase + headSize);
    if(0 != align)
    {
        pBufData = (Ptr)OSA_align((Uint32L)pBufData, align);
    }

    /*��ȡͷ��λ��*/
    pBufHead = OSA_memDataToHead(pBufData);

    /*���ͷ*/
    pBufHead->nMgicNum = OSA_MEM_MAGIC;
    pBufHead->pBufBase = pBufBase;
    pBufHead->size = allocSize;

    /*�������ݶ�ָ��*/
    return pBufData;
}

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
* ������  : OSA_memUFree
* ��  ��  : �ú���������linux�û�̬���ͷ��ڴ�
*
* ��  ��  : - pPtr:    Ҫ�ͷŵ��ڴ�ָ��
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�,�ڴ����ͷ�
*           OSA_EFAIL: ʧ��, �ڴ�δ�ͷ�
*******************************************************************************/
Int32 OSA_memUFree(Ptr pPtr)
{
    OSA_MemBlockObject *pBufHead;
    
    if(NULL == pPtr)
    {
        return OSA_EFAIL;
    }

    pBufHead = OSA_memDataToHead(pPtr);

    /*У��ħ��*/
    if(OSA_MEM_MAGIC != pBufHead->nMgicNum)
    {
        OSA_ERROR("invalid magic:0x%x\n", pBufHead->nMgicNum);
        return OSA_EFAIL;
    }

    /*�ͷ��ڴ��ʱ��ħ����Ϊ-1*/
    pBufHead->nMgicNum = (Uint32)-1;

    /*�ͷ��ڴ�*/
    free(pBufHead->pBufBase);

    return OSA_SOK;
}

#ifdef __ARM_NEON__
Ptr OSA_memCpySize(Ptr pDst,Ptr pSrc,Uint32 size)
{
    /* С��64BʱЧ�ʲ���memcpy  */
    extern Ptr OSA_memCpyNeon(Ptr pDest,Ptr pSrc,Uint32 size);
    if (size < 64)
    {
        return memcpy(pDst,pSrc,size);
    }
    return OSA_memCpyNeon(pDst,pSrc,size);
}
#endif

