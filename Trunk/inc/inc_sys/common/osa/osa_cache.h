/*******************************************************************************
* osa_cache.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Li Xianglin <li_xianglin@dahuatech.com>
* Version: V1.0.0  2013-12-18 Create
*
* Description: cache操作接口。与平台相关的接口。
*
*       1. 硬件说明。
*          无。
*
*       2. 程序结构说明。
*          模块化和层次化设计。
*
*       3. 使用说明。
*          无。
*
*       4. 局限性说明。
*          无。
*
*       5. 其他说明。
*          无。
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
/* 头文件区                                                                   */
/* ========================================================================== */


#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/* 宏和类型定义区                                                             */
/* ========================================================================== */


/* Cache类型 */
typedef enum
{
    /* L1 指令Cache */
    OSA_CACHE_TYPE_L1P   = 0x0001,

    /* L1 数据Cache */
    OSA_CACHE_TYPE_L1D   = 0x0002,

    /* L1 Cache */
    OSA_CACHE_TYPE_L1    = (OSA_CACHE_TYPE_L1D | OSA_CACHE_TYPE_L1P),

    /* L2 指令Cache */
    OSA_CACHE_TYPE_L2P   = 0x0004,

    /* L2 数据Cache */
    OSA_CACHE_TYPE_L2D   = 0x0008,

    /* L2 Cache */
    OSA_CACHE_TYPE_L2    = (OSA_CACHE_TYPE_L2D | OSA_CACHE_TYPE_L2P),

    /* 所有指令Cache */
    OSA_CACHE_TYPE_ALL_P = (OSA_CACHE_TYPE_L2P | OSA_CACHE_TYPE_L1P),

    /* 所有数据Cache */
    OSA_CACHE_TYPE_ALL_D = (OSA_CACHE_TYPE_L2D | OSA_CACHE_TYPE_L1D),

     /* 所有 Cache */
    OSA_CACHE_TYPE_ALL   = 0xFFFF,

} OSA_CacheType;


/* ========================================================================== */
/* 函数定义区                                                                 */
/* ========================================================================== */


/*******************************************************************************
* 函数名  : OSA_cacheWb
* 描  述  : Cache回写，将缓存在cache的数据写入到实际的物理内存中
* 输  入  : - pMemAddr: 内存地址
*           - size: 字节数
*           - type: cache类型，参见OSA_CacheType，该参数目前仅在SysBios下有效
*           - isWait: 是否等待cache操作完成，该参数目前仅在SysBios下有效
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_cacheWb(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait);


/*******************************************************************************
* 函数名  : OSA_cacheInv
* 描  述  : Cache无效，将缓存在cache中的数据丢弃掉，不写入到内存中。cache跟内存
*           之间的缓存映射也将自动清除。
* 输  入  : - pMemAddr: 内存地址
*           - size: 字节数
*           - type: cache类型，参见OSA_CacheType，该参数目前仅在SysBios下有效
*           - isWait: 是否等待cache操作完成，该参数目前仅在SysBios下有效
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_cacheInv(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait);


/*******************************************************************************
* 函数名  : OSA_cacheWbInv
* 描  述  : Cache回写并无效，将缓存在cache中的数据写入到内存中，然后将cache对应
*           的数据清除。
* 输  入  : - pMemAddr: 内存地址
*           - size: 字节数
*           - type: cache类型，参见OSA_CacheType，该参数目前仅在SysBios下有效
*           - isWait: 是否等待cache操作完成，该参数目前仅在SysBios下有效
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_cacheWbInv(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait);


#ifdef __cplusplus
}
#endif


#endif /* __OSA_CACHE_H__ */

