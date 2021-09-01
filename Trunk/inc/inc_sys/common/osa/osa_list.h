/*******************************************************************************
* osa_list.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA���������ӿڡ�
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


#ifndef _OSA_LIST_H_
#define _OSA_LIST_H_


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */


#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/* ���ڳ�ʼ������ڵ� */
#ifndef _WIN32
#define OSA_LIST_POISON1  ((void *) 0xAC11CD76)
#define OSA_LIST_POISON2  ((void *) 0xAC12CD86)
#else
#define OSA_LIST_POISON1  NULL
#define OSA_LIST_POISON2  NULL
#endif


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */

/* ����ͷ */
typedef struct OSA_ListHead
{
	struct OSA_ListHead *next;
	struct OSA_ListHead *prev;
} OSA_ListHead;


/* ����̬����ͳ�ʼ�� */
#define OSA_LIST_HEAD(name) \
	OSA_ListHead name = { &(name), &(name) }


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/* ����̬��ʼ�� */
static inline void OSA_listHeadInit(OSA_ListHead *list)
{
	list->next = list;
	list->prev = list;
}


/* ͨ����ӳ�Ա�Ľӿڣ���ָ�����е�ǰ�ڵ�ͺ�ڵ㡣*/
static inline void __OSA_listAdd(OSA_ListHead *newList,
			                     OSA_ListHead *prev,
			                     OSA_ListHead *next)
{
	next->prev = newList;
	newList->next = next;
	newList->prev = prev;
	prev->next = newList;
}


/* ��ӳ�Ա�Ľӿڣ�head��nextָ���β��head��prevָ����һ��Ԫ�ء�*/
static inline void OSA_listAdd(OSA_ListHead *newList, OSA_ListHead *head)
{
	__OSA_listAdd(newList, head, head->next);
}


/* ��ӳ�Ա�Ľӿڣ�head��nextָ����һ��Ԫ�أ�head��prevָ���β��*/
static inline void OSA_listAddTail(OSA_ListHead *newList, OSA_ListHead *head)
{
	__OSA_listAdd(newList, head->prev, head);
}


/* ��������ɾ����Ա��ͨ�ýӿڣ�ָ���ó�Ա�Ķ�ͷ�Ͷ�β��*/
static inline void __OSA_listDel(OSA_ListHead * prev, OSA_ListHead * next)
{
	next->prev = prev;
	prev->next = next;
}


/* ��������ɾ����Ա��ͨ�ýӿڣ�ָ���ó�Ա��*/
static inline void OSA_listDel(OSA_ListHead *entry)
{
	__OSA_listDel(entry->prev, entry->next);
	entry->next = (OSA_ListHead *)OSA_LIST_POISON1;
	entry->prev = (OSA_ListHead *)OSA_LIST_POISON2;
}


/* �ж������Ƿ�Ϊ�յĽӿڡ�*/
static inline int OSA_listEmpty(const OSA_ListHead *head)
{
	return head->next == head;
}


/* ͨ�������Ա��ȡ�����ṹ����׵�ַ��*/
#define OSA_listEntry(ptr, type, member) \
	OSA_containerOf(ptr, type, member)

/* ��ȡ�����׸�Ԫ�س�Ա, �������豣֤����Ϊ�� */
#define OSA_listFirstEntry(ptr, type, member) \
	OSA_listEntry((ptr)->next, type, member)

/* ��ȡ�����׸�Ԫ�س�Ա, �������Ϊ��, �򷵻�NULL */
#define OSA_listFirstEntryOrNull(ptr, type, member) \
	(!OSA_listEmpty(ptr) ? OSA_listFirstEntry(ptr, type, member) : NULL)

/* ��������������ת���������л�õĽڵ㲻�ɱ�ɾ����*/
#define OSA_listForEachT(pos, head, T) \
    for (pos = (T*)((head)->next); pos != (T*)(head); \
        pos = (T*)(((OSA_ListHead*)pos)->next))

/* �������������л�õĽڵ㲻�ɱ�ɾ����*/
#define OSA_listForEach(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)

/* ����������������л�õĽڵ㲻�ɱ�ɾ����*/
#define OSA_listForEachPrev(pos, head) \
        for (pos = (head)->prev; pos != (head); \
                pos = pos->prev)

/* �������������л�õĽڵ�ɰ�ȫɾ����tmpΪ��ʱ�ڵ�ָ�롣*/
#define OSA_listForEachSafe(pos, tmp, head) \
    for (pos = (head)->next, tmp = pos->next; pos != (head); \
         pos = tmp, tmp = pos->next)

/* ����������������л�õĽڵ�ɰ�ȫɾ����tmpΪ��ʱ�ڵ�ָ�롣*/
#define OSA_listForEachPrevSafe(pos, tmp, head) \
    for (pos = (head)->prev, tmp = pos->prev; pos != (head); \
         pos = tmp, tmp = pos->prev)

#ifdef __cplusplus
    }
#endif


#endif /* _OSA_LIST_H_ */


