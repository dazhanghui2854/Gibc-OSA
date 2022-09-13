/*******************************************************************************
* osa_cache.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Li Xianglin <li_xianglin@dahuatech.com>
* Version: V1.0.0  2013-7-9 Create
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


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#include <osa.h>




/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */


/*******************************************************************************
* 函数名  : OSA_cacheWb
* 描  述  : Cache回写，将缓存在cache的数据写入到实际的物理内存中
* 输  入  : - pMemAddr: 内存地址
*           - size: 字节数
*           - type: cache类型，参见OSA_CacheType
*           - isWait: 是否等待cache操作完成
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_cacheWb(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait)
{
    (void)pMemAddr;
    (void)size;
    (void)type;
    (void)isWait;

    return OSA_SOK;
}


/*******************************************************************************
* 函数名  : OSA_cacheInv
* 描  述  : Cache无效，将缓存在cache中的数据丢弃掉，不写入到内存中。cache跟内存
*           之间的缓存映射也将自动清除。
* 输  入  : - pMemAddr: 内存地址
*           - size: 字节数
*           - type: cache类型，参见OSA_CacheType
*           - isWait: 是否等待cache操作完成
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_cacheInv(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait)
{
    (void)pMemAddr;
    (void)size;
    (void)type;
    (void)isWait;

    return OSA_SOK;
}


/*******************************************************************************
* 函数名  : OSA_cacheWbInv
* 描  述  : Cache回写并无效，将缓存在cache中的数据写入到内存中，然后将cache对应
*           的数据清除。
* 输  入  : - pMemAddr: 内存地址
*           - size: 字节数
*           - type: cache类型，参见OSA_CacheType
*           - isWait: 是否等待cache操作完成
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_cacheWbInv(Ptr pMemAddr, Uint32 size, Uint32 type, Bool32 isWait)
{
    (void)pMemAddr;
    (void)size;
    (void)type;
    (void)isWait;

    return OSA_SOK;
}



