/*******************************************************************************
* osa_types.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2012-5-31 Create
*
* Description: ͳһ���������͡�
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


#ifndef _OSA_TYPES_H_
#define _OSA_TYPES_H_


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */
#include "osa_sys.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

#if !defined(STD_H) && !defined(_TI_STD_TYPES)

/* ���������� */
#ifndef DEFINED_Bool
#define DEFINED_Bool
typedef unsigned char      Bool;        /* ͨ�ò������� */
#endif

/* �з������Ͷ��� */
#ifndef DEFINED_Int32
#define DEFINED_Int32
typedef int                Int32;       /* �з���32λ���������� */   
#endif

#ifndef DEFINED_Int16
#define DEFINED_Int16
typedef short              Int16;       /* �з���16λ���������� */   
#endif

#ifndef DEFINED_Int8
#define DEFINED_Int8
typedef char               Int8;        /* �з���8λ���������� */    
#endif

/* ָ�����Ͷ��� */
#ifndef DEFINED_Ptr
#define DEFINED_Ptr
typedef void *             Ptr;         /* ָ������ */
#endif

/* �ַ����Ͷ��� */
#ifndef DEFINED_String
#define DEFINED_String
typedef char *             String;      /* �ַ������ͣ���NUL��β��*/
#endif

#ifndef DEFINED_Char
#define DEFINED_Char
typedef char               Char;        /* �ַ����� */
#endif

#endif


#if !defined(_TI_STD_TYPES)

/* �޷������Ͷ��� */ 
#ifndef DEFINED_Uint32
#define DEFINED_Uint32
typedef unsigned int       Uint32;      /* �޷���32λ���������� */ 
#endif

#ifndef DEFINED_Uint16
#define DEFINED_Uint16
typedef unsigned short     Uint16;      /* �޷���16λ���������� */ 
#endif

#ifndef DEFINED_Uint8
#define DEFINED_Uint8
typedef unsigned char      Uint8;       /* �޷���8λ���������� */ 
#endif

#ifndef DEFINED_Int64
#define DEFINED_Int64
typedef long long          Int64;       /* �з���64λ���������� */
#endif

#endif

#ifndef DEFINED_Bool16
#define DEFINED_Bool16
typedef unsigned short     Bool16;      /* 16λ�������� */
#endif

#ifndef DEFINED_Bool32
#define DEFINED_Bool32
typedef unsigned int       Bool32;      /* 32λ�������� */
#endif

#ifndef DEFINED_Int32L
#define DEFINED_Int32L
typedef long               Int32L;      /* �з���32λ������������ */ 
#endif

#ifndef DEFINED_Uint32L
#define DEFINED_Uint32L
typedef unsigned long      Uint32L;     /* �޷���32λ������������ */   
#endif

#ifndef DEFINED_Uint64
#define DEFINED_Uint64
typedef unsigned long long Uint64;      /* �޷���64λ���������� */
#endif


/* �������Ͷ��� */
#ifndef DEFINED_Float32
#define DEFINED_Float32
typedef float              Float32;		/* 32λ���������� */
#endif

#ifndef DEFINED_Float64
#define DEFINED_Float64
typedef double             Float64;		/* 64λ���������� */
#endif

/* ������� */
#ifndef DEFINED_Handle
#define DEFINED_Handle
typedef void *             Handle;      /* ͳ�þ������ */
#endif

/* �����Ͷ��� */
#ifndef DEFINED_Empty
#define DEFINED_Empty
typedef void               Empty;        /* ������ */
#endif

/* size_t���Ͷ��� */
#ifndef DEFINED_Sizet
#define DEFINED_Sizet
typedef size_t             Sizet;       /* size_t���� */
#endif

/* 32&64λ�������Ͷ��� */
#define Uint8_Align64 Uint8 __attribute__ ((aligned(8)))

#define _SIZEOF_NEED_CHECKED(dataType) \
	(sizeof(dataType) - OSA_offsetof(dataType, u8_align64))
	
#define _SIZEOF_NEED_CHECKED_EQUAL(dataType, nBytes) \
	((_SIZEOF_NEED_CHECKED(dataType) == (nBytes))?1:-1)
	
#define _SIZEOF_CHECK_EQ(dataType, nBytes) \
	typedef char SC_EQ_##dataType[_SIZEOF_NEED_CHECKED_EQUAL(dataType, nBytes)];

#define _SIZEOF_CHECK_EQ_NEW(dataType, nBytes) \
	typedef char SC_EQ_##dataType[((sizeof(dataType) == (nBytes))?1:-1)];

#if (__SIZEOF_POINTER__ == 4)
#define Ptr_t(type, field) type* field;void * res_##field
#define Fxn_t(type, field, args ...) type (*field)(args);void * res_##field
#elif (__SIZEOF_POINTER__ == 8)
#define Ptr_t(type, field) type* field
#define Fxn_t(type, field, args ...) type (*field)(args)
#else
#error "__SIZEOF_POINTER__ = ?"
#endif

/// ��һ��д��
/// ��֤32λ��64λƽ̨��ָ���������һ�£���ֹ32λƽ̨ͷ�ļ�����ָ�뵼��64λ�½ṹ���С�ı�
/// �÷�����ָ�������ʹ�ô˺꣬N������ֵ��ͬһ���ṹ���ڲ��������ɡ�
/// char* pointer; 
///OSA_UNUSED_FORCE_POINTER_ALIGN8(pointer)
#ifndef OSA_UNUSED_FORCE_POINTER_ALIGN8
#if defined(_WIN64) || (defined(__WORDSIZE) && __WORDSIZE==64)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N)
#elif defined(_WIN32) || (defined(__WORDSIZE) && __WORDSIZE==32)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N) Uint32 unused##N;
#elif defined(__GNUC__)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N) Uint8 unused##N[8-sizeof(void*)];
#elif defined(CCS)
#define OSA_UNUSED_FORCE_POINTER_ALIGN8(N) Uint32 unused##N;
#else
#error "Can't find macro `__WORDSIZE' definition, please specify this macro 32 or 64 base on your platform!"
#endif
#endif //UNUSED_FORCE_POINTER_ALIGN8


#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_TYPES_H_  */

