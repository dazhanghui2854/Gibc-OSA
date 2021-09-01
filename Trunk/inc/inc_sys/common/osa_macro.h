/*******************************************************************************
* osa_macro.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2012-5-31 Create
*
* Description: ͳһ��ͨ�úꡣ
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


#ifndef _OSA_MACRO_H_
#define _OSA_MACRO_H_


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/* ��������ֵ */
#define OSA_SOK        0        /* �ɹ� */
#define OSA_EFAIL     -1        /* ͨ�ô��� */
#define OSA_ETIMEOUT  -2        /* �ȴ���ʱ */
#define OSA_EINTR     -3        /* ���ź��ж� */
#define OSA_EBUSY     -4        /* ����æ */


/* Bool������� */
#define OSA_TRUE	  (1)       /* �� */
#define OSA_FALSE	  (0)       /* �� */

/* NULLֵ���� */
#define OSA_NULL      (NULL)

/* ��������register�ؼ��� */
#define OSA_register  register

/* ��ʱ���� */
#define OSA_TIMEOUT_NONE         (0)    /* ���ȴ����������ء�*/ 
#define OSA_TIMEOUT_FOREVER      (~0U)  /* һֱ�ȴ�ֱ������ */


/* ���ݴ洢������λ */
#define OSA_KB (1024)
#define OSA_MB (OSA_KB * OSA_KB)
#define OSA_GB (OSA_KB * OSA_MB)
/* 
 * ���ð汾���õĺꡣ��8λ����Major Version����8λ����Minor Version��
 * ��16����Revision��
 */
#define OSA_versionSet(major, minor, revision)  \
        ((((major) & 0x0ff) << 24U) | (((minor) & 0x0ff) << 16U) \
           | ((revision) & 0x0ffff))


/* ��ֵ�Ƚ�*/
#define OSA_max(a, b)    ( (a) > (b) ? (a) : (b) )
#define OSA_min(a, b)    ( (a) < (b) ? (a) : (b) )

/* ��ΧԼ��, ����Ϊ��ֵ */
#define OSA_upperLimit(x, limit)  if ((x) > (limit)) (x) = (limit)
#define OSA_lowerLimit(x, limit)  if ((x) < (limit)) (x) = (limit)
#define OSA_rangeLimit(x, lowerLimit, upperLimit) \
		do \
		{ \
		    OSA_lowerLimit((x), (lowerLimit)); \
		    OSA_upperLimit((x), (upperLimit)); \
		} while (0)

/* �ڴ����*/
#define OSA_clear(ptr)		        memset((ptr), 0, sizeof(*(ptr)))	
#define OSA_clearSize(ptr, size)        memset((ptr), 0, (size))
#define OSA_memCmp(dst, src)            memcmp((dst), (src), sizeof(*(src)))
#define OSA_memCmpSize(dst, src, size)  memcmp((dst), (src), (size))


/* �������*/
#define OSA_align(value, align)   ((( (value) + ( (align) - 1 ) ) \
                                     / (align) ) * (align) )
#define OSA_ceil(value, align)    OSA_align(value, align) 
#define OSA_floor(value, align)   (( (value) / (align) ) * (align) )


/* ������� */
#define OSA_setTrue(val)        ((val) = OSA_TRUE)
#define OSA_setFalse(val)       ((val) = OSA_FALSE)      


/* ��ȡ�����Ա���� */
#define OSA_arraySize(array)    (sizeof(array)/sizeof((array)[0]))


/* ��ȡ�ṹ���Ա�ĵ�ַƫ���� */
#ifdef __compiler_offsetof
#define OSA_offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define OSA_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif


/* ͨ���ṹ���Ա��ȡ�ṹ���׵�ַ */
#define OSA_containerOf(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - OSA_offsetof(type,member) );})


/* ���valueֵ�Ƿ���2��N�η� */
#define OSA_checkIs2n(value)  ( ((value) == 0) ? OSA_FALSE  \
                               : ((value) & ((value) - 1))  \
                                  ? OSA_FALSE : OSA_TRUE )

#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_MACRO_H_  */


