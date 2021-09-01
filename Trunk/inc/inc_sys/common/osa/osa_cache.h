/*******************************************************************************
* osa_cache.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Li Xianglin <li_xianglin@dahuatech.com>
* Version: V1.0.0  2013-12-18 Create
*
* Description: cache�����ӿڡ���ƽ̨��صĽӿڡ�
*
*       1. Ӳ��˵����
*          �ޡ�
*
*       2. ����ṹ˵����
*          ģ�黯�Ͳ�λ���ơ�
*
*       3. ʹ��˵����
*          �ޡ�
*
*       4. ������˵����
*          �ޡ�
*
*       5. ����˵����
*          �ޡ�
*
* Modification:
* 1. Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

#ifndef __OSA_CACHE_H__
#define __OSA_CACHE_H__

/* ========================================================================== */
/* ͷ�ļ���                                                                   */
/* ========================================================================== */


#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/* ������Ͷ�����                                                             */
/* ========================================================================== */


/* Cache���� */
typedef enum
{
    /* L1 ָ��Cache */
    OSA_CACHE_TYPE_L1P   = 0x0001,

    /* L1 ����Cache */
    OSA_CACHE_TYPE_L1D   = 0x0002,

    /* L1 Cache */
    OSA_CACHE_TYPE_L1    = (OSA_CACHE_TYPE_L1D | OSA_CACHE_TYPE_L1P),

    /* L2 ָ��Cache */
    OSA_CACHE_TYPE_L2P   = 0x0004,

    /* L2 ����Cache */
    OSA_CACHE_TYPE_L2D   = 0x0008,

    /* L2 Cache */
    OSA_CACHE_TYPE_L2    = (OSA_CACHE_TYPE_L2D | OSA_CACHE_TYPE_L2P),

    /* ����ָ��Cache */
    OSA_CACHE_TYPE_ALL_P = (OSA_CACHE_TYPE_L2P | OSA_CACHE_TYPE_L1P),

    /* ��������Cache */
    OSA_CACHE_TYPE_ALL_D = (OSA_CACHE_TYPE_L2D | OSA_CACHE_TYPE_L1D),

     /* ���� Cache */
    OSA_CACHE_TYPE_ALL   = 0xFFFF,

} OSA_CacheType;


/* ========================================================================== */
/* ����������                                                                 */
/* ========================================================================== */


/*******************************************************************************
* ������  : OSA_cacheWb
* ��  ��  : Cache��д����������cache������д�뵽ʵ�ʵ������ڴ���
* ��  ��  : - pMemAddr: �ڴ��ַ
*           - size: �ֽ���
*           - type: cache���ͣ��μ�OSA_CacheType���ò���Ŀǰ����SysBios����Ч
*           - isWait: �Ƿ�ȴ�cache������ɣ��ò���Ŀǰ����SysBios����Ч
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_cacheWb(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait);


/*******************************************************************************
* ������  : OSA_cacheInv
* ��  ��  : Cache��Ч����������cache�е����ݶ���������д�뵽�ڴ��С�cache���ڴ�
*           ֮��Ļ���ӳ��Ҳ���Զ������
* ��  ��  : - pMemAddr: �ڴ��ַ
*           - size: �ֽ���
*           - type: cache���ͣ��μ�OSA_CacheType���ò���Ŀǰ����SysBios����Ч
*           - isWait: �Ƿ�ȴ�cache������ɣ��ò���Ŀǰ����SysBios����Ч
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_cacheInv(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait);


/*******************************************************************************
* ������  : OSA_cacheWbInv
* ��  ��  : Cache��д����Ч����������cache�е�����д�뵽�ڴ��У�Ȼ��cache��Ӧ
*           �����������
* ��  ��  : - pMemAddr: �ڴ��ַ
*           - size: �ֽ���
*           - type: cache���ͣ��μ�OSA_CacheType���ò���Ŀǰ����SysBios����Ч
*           - isWait: �Ƿ�ȴ�cache������ɣ��ò���Ŀǰ����SysBios����Ч
* ��  ��  : �ޡ�
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_cacheWbInv(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait);


#ifdef __cplusplus
}
#endif


#endif /* __OSA_CACHE_H__ */

