/*******************************************************************************
* osa_cmpl.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2011-06-08Create
*
* Description: ��������صķ�װ��
*
*       1. Ӳ��˵����
*          �ޡ�
*  
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
* 1. Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_CMPL_H_
#define _OSA_CMPL_H_


#ifdef __GNUC__  /* GCC������ */
    /* �����������������Ż�����ִ��˳�򣬿������������Ĵ���ִ����ˮ�������Ⱥ�cache�����ʡ�*/
    #define OSA_likely(x)      __builtin_expect(!!(x), 1)  /* ���ܷ����ĸ��ʵ� */
    #define OSA_unlikely(x)    __builtin_expect(!!(x), 0)  /* ���ܷ����ĸ��ʸ� */
    /* �˺������ڷ�ֹ�������Ż�����ִ��˳��*/
    #define OSA_barrier()      __asm__ __volatile__("": : :"memory")
#else
    #define OSA_likely(x)     (x) 
    #define OSA_unlikely(x)   (x)   
    #define OSA_barrier()
#endif


#endif


