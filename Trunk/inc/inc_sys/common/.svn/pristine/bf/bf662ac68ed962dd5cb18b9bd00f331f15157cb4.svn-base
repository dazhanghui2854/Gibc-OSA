/*******************************************************************************
* osa_list.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2010-8-31 Create
*
* Description: OSA的链表管理接口。
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


#ifndef _OSA_LIST_H_
#define _OSA_LIST_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */


#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* 用于初始化链表节点 */
#ifndef _WIN32
#define OSA_LIST_POISON1  ((void *) 0xAC11CD76)
#define OSA_LIST_POISON2  ((void *) 0xAC12CD86)
#else
#define OSA_LIST_POISON1  NULL
#define OSA_LIST_POISON2  NULL
#endif


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/* 链表头 */
typedef struct OSA_ListHead
{
	struct OSA_ListHead *next;
	struct OSA_ListHead *prev;
} OSA_ListHead;


/* 链表静态定义和初始化 */
#define OSA_LIST_HEAD(name) \
	OSA_ListHead name = { &(name), &(name) }


/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */

/* 链表动态初始化 */
static inline void OSA_listHeadInit(OSA_ListHead *list)
{
	list->next = list;
	list->prev = list;
}


/* 通用添加成员的接口，需指定队列的前节点和后节点。*/
static inline void __OSA_listAdd(OSA_ListHead *newList,
			                     OSA_ListHead *prev,
			                     OSA_ListHead *next)
{
	next->prev = newList;
	newList->next = next;
	newList->prev = prev;
	prev->next = newList;
}


/* 添加成员的接口，head的next指向队尾，head的prev指向下一个元素。*/
static inline void OSA_listAdd(OSA_ListHead *newList, OSA_ListHead *head)
{
	__OSA_listAdd(newList, head, head->next);
}


/* 添加成员的接口，head的next指向下一个元素，head的prev指向队尾。*/
static inline void OSA_listAddTail(OSA_ListHead *newList, OSA_ListHead *head)
{
	__OSA_listAdd(newList, head->prev, head);
}


/* 从链表中删除成员的通用接口，指定该成员的队头和队尾。*/
static inline void __OSA_listDel(OSA_ListHead * prev, OSA_ListHead * next)
{
	next->prev = prev;
	prev->next = next;
}


/* 从链表中删除成员的通用接口，指定该成员。*/
static inline void OSA_listDel(OSA_ListHead *entry)
{
	__OSA_listDel(entry->prev, entry->next);
	entry->next = (OSA_ListHead *)OSA_LIST_POISON1;
	entry->prev = (OSA_ListHead *)OSA_LIST_POISON2;
}


/* 判断链表是否为空的接口。*/
static inline int OSA_listEmpty(const OSA_ListHead *head)
{
	return head->next == head;
}


/* 通过链表成员获取包裹结构体的首地址。*/
#define OSA_listEntry(ptr, type, member) \
	OSA_containerOf(ptr, type, member)

/* 获取链表首个元素成员, 调用者需保证链表不为空 */
#define OSA_listFirstEntry(ptr, type, member) \
	OSA_listEntry((ptr)->next, type, member)

/* 获取链表首个元素成员, 如果链表为空, 则返回NULL */
#define OSA_listFirstEntryOrNull(ptr, type, member) \
	(!OSA_listEmpty(ptr) ? OSA_listFirstEntry(ptr, type, member) : NULL)

/* 遍历链表，带类型转换，过程中获得的节点不可被删除。*/
#define OSA_listForEachT(pos, head, T) \
    for (pos = (T*)((head)->next); pos != (T*)(head); \
        pos = (T*)(((OSA_ListHead*)pos)->next))

/* 遍历链表，过程中获得的节点不可被删除。*/
#define OSA_listForEach(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)

/* 反向遍历链表，过程中获得的节点不可被删除。*/
#define OSA_listForEachPrev(pos, head) \
        for (pos = (head)->prev; pos != (head); \
                pos = pos->prev)

/* 遍历链表，过程中获得的节点可安全删除。tmp为临时节点指针。*/
#define OSA_listForEachSafe(pos, tmp, head) \
    for (pos = (head)->next, tmp = pos->next; pos != (head); \
         pos = tmp, tmp = pos->next)

/* 反向遍历链表，过程中获得的节点可安全删除。tmp为临时节点指针。*/
#define OSA_listForEachPrevSafe(pos, tmp, head) \
    for (pos = (head)->prev, tmp = pos->prev; pos != (head); \
         pos = tmp, tmp = pos->prev)

#ifdef __cplusplus
    }
#endif


#endif /* _OSA_LIST_H_ */


