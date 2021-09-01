/*******************************************************************************
* osa_trace_level.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2011-07-25 Create
*          V1.0.1  2011-11-14 1. ���Ӷ�TI SYSBIOSϵͳ��֧�֡�                            
*                             2. ��DEBUG���޸�Ϊ_OSA_TRACE_��
*                             3. �ļ�����osa_debug.h�޸�Ϊosa_trace.h��
*          V1.0.2  2011-11-30 1. ���ӳ������ʱ�ö���OSA_COMPILETIME_ASSERT��
*          V1.0.3  2012-08-10 1. ���ӵȼ����ơ�
*          V1.0.4  2012-12-05 1. ֧����־����ģ�顣
*
* Description: ͳһ�ĵ��Դ�ӡ�ӿڣ�����LinuxӦ�ó������������SYSBIOSϵͳ����
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


#ifndef _OSA_TRACE_H_
#define _OSA_TRACE_H_


/* 
 ģ������ֱ�����Makefile������ͷ�ļ��ж��壬���ֲ���Ҫ����[]��
 ��Makefile����ӵķ����ǣ�
 OSA_MODULE_NAME = XXX
 CFLAGS       += -DOSA_MODULE_NAME='"$(OSA_MODULE_NAME)"'
 EXTRA_CFLAGS += -D'OSA_MODULE_NAME=\"$(OSA_MODULE_NAME)\"'
 Ӧ�ó�����ʹ��CFLAGS������������ʹ��EXTRA_CFLAGS��
*/
#ifndef OSA_MODULE_NAME
#warning Not define OSA_MODULE_NAME !!!
#define OSA_MODULE_NAME "UNKNOWN"
#endif

#define OSA_print OSA_logWrite


/* _OSA_TRACE_���ǵ��Կ��أ���ͨ��Makeife���룬���뷽����-D_OSA_TRACE_��*/
#ifdef _OSA_TRACE_
    /* ���ԣ��������Ϊ��ɫ����ӡ���ļ��������������кš������������ѭ����*/ 
	#define OSA_assert(x)  \
	    if((x) == 0) 	   \
	    	{ 			   \
	    	OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED,  \
	    	        "[" OSA_MODULE_NAME "] " "ASSERT (%s|%d)\r\n", \
	    	                __func__, __LINE__); \
	    	while(1);     \
	    } 
		
	#define OSA_assertSuccess(status)	\
	    OSA_assert(OSA_SOK == (status))
    #define OSA_assertFail(status)  \
	    OSA_assert(OSA_SOK != (status))

    #define OSA_assertNotNull(ptr)  \
	    OSA_assert(NULL != (ptr))
    #define OSA_assertNull(ptr)  \
        OSA_assert(NULL == (ptr)) 

    /* ���Դ�ӡ��������岻����ɫ, ����ʱ��*/
	#define OSA_DEBUG(fmt, args ...) \
		OSA_print(OSA_LOG_LV_DBG, OSA_LOG_CLR_NONE,"[" OSA_MODULE_NAME "] " fmt, ## args)

#else
	#define OSA_assert(status)
	#define OSA_assertSuccess(status)
	#define OSA_assertFail(status)
	#define OSA_assertNotNull(ptr)
	#define OSA_assertNull(ptr)
#ifndef _WIN32
	#define OSA_DEBUG(fmt, args ... ) 
#else
	#define OSA_DEBUG(fmt, ... ) 
	#define __func__ __FUNCTION__
#endif
#endif


/* �жϷ���ֵ */
#define OSA_isSuccess(status)  (OSA_SOK == (status))
#define OSA_isFail(status)     (OSA_SOK != (status))
    
#define OSA_isTrue(status)     ((status))
#define OSA_isFalse(status)    (!(status))   
    
#define OSA_isNull(ptr)        (NULL == (ptr))
#define OSA_isNotNull(ptr)     (NULL != (ptr))


/* ����Release�汾�õĶ��� */
/* ���ԣ��������Ϊ��ɫ����ӡ���ļ��������������кš������������ѭ����*/ 
#define OSA_assertRl(x)  \
    if((x) == 0)         \
    { \
        OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
            "ASSERT (%s|%d)\r\n", __func__, __LINE__); \
        while(1);   \
    } 
    
#define OSA_assertSuccessRl(status)  \
    OSA_assertRl(OSA_SOK == (status))
#define OSA_assertFailRl(status)  \
    OSA_assertRl(OSA_SOK != (status))

#define OSA_assertNotNullRl(ptr)  \
    OSA_assertRl(NULL != (ptr))
#define OSA_assertNullRl(ptr)  \
    OSA_assertRl(NULL == (ptr)) 


/* �������ʱ�õĶ��ԣ��ö��Է����ڳ������ʱ�̡�*/
#define OSA_COMPTIME_ASSERT(condition)  \
    do                                  \
    {                                   \
       typedef char ErrorCheck[((condition) == OSA_TRUE) ? 1 : -1]; \
    } \
    while(0)

#ifndef _WIN32

/* ���´�ӡ�����Ƿ�������ģʽ������ӡ�����*/

/* ͨ�ô�ӡ��������岻����ɫ, ����ʱ�䣬����ģ������ */
#define OSA_printf(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_DBG, OSA_LOG_CLR_NONE, fmt, ## args);\
	} \
	while(0)

/* �����ӡ���������Ϊ��ɫ����ӡ���ļ��������������к�,���������ܼ������С�*/
#define OSA_ERROR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
		              "ERROR  (%s|%d): " \
		              fmt, __func__, __LINE__, ## args); \
	} \
	while(0)
 
/* �����ӡ���������Ϊ��ɫ��*/
#define OSA_ERRORR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
		              "ERROR  : " \
		              fmt, ## args); \
	} \
	while(0)	

/* 
* �����ӡ���������Ϊ��ɫ����ӡ���ļ��������������кš����������Կɼ������У�
* ���뾯ʾ��
*/
#define OSA_WARN(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
		                    "[" OSA_MODULE_NAME "] " "WARN  (%s|%d): " \
		                    fmt, __func__, __LINE__, ## args); \
	} \
	while(0)

/* �����ӡ���������Ϊ��ɫ�����������Կ�������ȥ�����뾯ʾ��*/
#define OSA_WARNR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
		                    "[" OSA_MODULE_NAME "] " "WARN  : " \
		                    fmt, ## args); \
	} \
	while(0)

/* ��Ϣͨ���ӡ���������Ϊ��ɫ��*/
#define OSA_INFO(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)


/*  
* ���ϴ�ӡΪ��ģ��ǿ��ʹ�ã����´�ӡ��ģ���ѡ��ʹ�ã�Ҳ��������Ӵ�ӡ��ʽ��
* ����ӡ�����ӡ��ģ�����֣�������Ӧ���������׺������ĸR�ı�ʾ����ӡģ������
*/
#define OSA_INFOR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, fmt, ## args);\
	} \
	while(0)


#define OSA_INFOPUP(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)

#define OSA_INFOPUPR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, fmt, ## args);\
    } \
    while(0)


#define OSA_INFORED(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)
	
#define OSA_INFOREDR(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, fmt, ## args);\
	} \
	while(0)	

	
#define OSA_INFOBLUE(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)

#define OSA_INFOBLUER(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, fmt, ## args);\
    } \
    while(0)


#define OSA_INFODG(fmt, args ...) \
	do \
	{ \
		OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, \
		                    "[" OSA_MODULE_NAME "] " fmt, ## args);\
	} \
	while(0)


#define OSA_INFODGR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, fmt, ## args);\
    } \
    while(0)


#define OSA_INFOCYAN(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)


#define OSA_INFOCYANR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, fmt, ## args);\
    } \
    while(0)
        

#define OSA_INFOBROWN(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)

#define OSA_INFOBROWNR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, fmt, ## args);\
    } \
    while(0)


#define OSA_INFOBYELLOW(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)

#define OSA_INFOBYELLOWR(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, fmt, ## args);\
    } \
    while(0)


#define OSA_INFOBWHITE(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, \
                            "[" OSA_MODULE_NAME "] " fmt, ## args);\
    } \
    while(0)

#define OSA_INFOBWHITER(fmt, args ...) \
    do \
    { \
        OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, fmt, ## args);\
    } \
    while(0)

#else

/* ���´�ӡ�����Ƿ�������ģʽ������ӡ�����*/

/* ͨ�ô�ӡ��������岻����ɫ, ����ʱ�䣬����ģ������ */
#define OSA_printf(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_DBG, OSA_LOG_CLR_NONE, fmt, ##__VA_ARGS__);\
	} \
	while(0)

/* �����ӡ���������Ϊ��ɫ����ӡ���ļ��������������к�,���������ܼ������С�*/
#define OSA_ERROR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
    	"ERROR  (%s|%d): " \
    	fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	} \
	while(0)

/* �����ӡ���������Ϊ��ɫ��*/
#define OSA_ERRORR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_ERR, OSA_LOG_CLR_RED, "[" OSA_MODULE_NAME "] " \
    	"ERROR  : " \
    	fmt, ##__VA_ARGS__); \
	} \
	while(0)	

/* 
* �����ӡ���������Ϊ��ɫ����ӡ���ļ��������������кš����������Կɼ������У�
* ���뾯ʾ��
*/
#define OSA_WARN(fmt,...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
    	"[" OSA_MODULE_NAME "] " "WARN  (%s|%d): " \
    	fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	} \
	while(0)

/* �����ӡ���������Ϊ��ɫ�����������Կ�������ȥ�����뾯ʾ��*/
#define OSA_WARNR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_WARN, OSA_LOG_CLR_YELLOW, \
    	"[" OSA_MODULE_NAME "] " "WARN  : " \
    	fmt, ##__VA_ARGS__); \
	} \
	while(0)

/* ��Ϣͨ���ӡ���������Ϊ��ɫ��*/
#define OSA_INFO(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)


/*  
* ���ϴ�ӡΪ��ģ��ǿ��ʹ�ã����´�ӡ��ģ���ѡ��ʹ�ã�Ҳ��������Ӵ�ӡ��ʽ��
* ����ӡ�����ӡ��ģ�����֣�������Ӧ���������׺������ĸR�ı�ʾ����ӡģ������
*/
#define OSA_INFOR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GREEN, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOPUP(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOPUPR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_PURPLE, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFORED(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOREDR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_RED, fmt, ##__VA_ARGS__);\
	} \
	while(0)	


#define OSA_INFOBLUE(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBLUER(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BLUE, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFODG(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFODGR(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_GRAY, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOCYAN(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOCYANR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_CYAN, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOBROWN(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBROWNR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_BROWN, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOBYELLOW(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBYELLOWR(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_YELLOW, fmt, ##__VA_ARGS__);\
	} \
	while(0)


#define OSA_INFOBWHITE(fmt, ...) \
	do \
	{ \
    	OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, \
    	"[" OSA_MODULE_NAME "] " fmt, ##__VA_ARGS__);\
	} \
	while(0)

#define OSA_INFOBWHITER(fmt, ...) \
	do \
	{ \
	    OSA_print(OSA_LOG_LV_INFO, OSA_LOG_CLR_WHITE, fmt, ##__VA_ARGS__);\
	} \
	while(0)
#endif

#endif  /* _OSA_TRACE_H_ */


