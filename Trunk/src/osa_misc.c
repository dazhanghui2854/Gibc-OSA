/*******************************************************************************
* osa_misc.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA无法归类的接口。
*
*       1. 硬件说明。
*          无。

*       2. 程序结构说明。
*          无
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
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#include <osa.h>


Int32 OSA_appExit(Int32 exitValue)
{
    _exit(exitValue);

    return OSA_SOK;
}


Uint32 OSA_getPageSize(void)
{
    return (Uint32)getpagesize();
}


