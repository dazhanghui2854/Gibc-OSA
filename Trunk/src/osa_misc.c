/*******************************************************************************
* osa_misc.c
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA�޷�����Ľӿڡ�
*
*       1. Ӳ��˵����
*          �ޡ�

*       2. ����ṹ˵����
*          ��
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


