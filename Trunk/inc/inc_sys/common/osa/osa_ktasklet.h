/*******************************************************************************
* osa_ktasklet.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-2 Create
*
* Desc: 定义linux内核态OSA模块对外提供的Tasklet接口
*       
*           接口调用流程如下:
*           ==========================
*                   |                            
*           OSA_kTaskletCreate
*                   |
*           OSA_kTaskletStart / OSA_kTaskletStop 
*                   |
*           OSA_kTaskletDelete
*           ===========================
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _OSA_KTASKLET_H_
#define _OSA_KTASKLET_H_

#ifdef __KERNEL__    /* 仅Linux 内核态支持*/


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/*内核Tasklet句柄类型*/
typedef Handle OSA_KTaskletHandle;

/*定义Tasklet的回调处理函数类型, 
入参hTasklet为Tasklet句柄
入参pUserArgs为Tasklet创建时传入的用户参数
返回值Int32表示函数执行成功或失败
*/
typedef Int32 (*OSA_KTaskletCallBack)(OSA_KTaskletHandle hTasklet, Ptr pUserArgs);


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

/*创建Tasklet的参数结构体*/
typedef struct
{
    OSA_KTaskletCallBack userFunc;          /*Tasklet的回调处理函数*/
    Ptr pUserArgs;                          /*传给回调处理函数的用户参数*/
                                            
    Uint32 reserved[4];                     /*保留*/                                            
}OSA_KTaskletInitParams;

/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_kTaskletCreate
* 描  述  : 该函数负责创建一个内核Tasklet
*           该接口不能在中断上下文调用
*
* 输  入  : - pParams:  创建Tasklet的参数，参见OSA_KTaskletInitParams定义
*
* 输  出  : - phTasklet:  Tasklet句柄指针,当创建成功时输出Tasklet句柄
* 返回值  :  OSA_SOK:   创建成功
*            OSA_EFAIL: 创建失败
*******************************************************************************/
Int32 OSA_kTaskletCreate(OSA_KTaskletInitParams *pParams, 
                            OSA_KTaskletHandle *phTasklet);

/*******************************************************************************
* 函数名  : OSA_kTaskletStart
* 描  述  : 该函数负责启动一个Tasklet,成功调用后Tasklet开始运行，调用时的注意事项:
*           1、该接口可以在中断上下文调用
*           2、可以在回调函数中调用本接口
*
* 输  入  :  - hTasklet:  Tasklet句柄
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   启动成功
*           OSA_EFAIL: 启动失败
*******************************************************************************/
Int32 OSA_kTaskletStart(OSA_KTaskletHandle hTasklet);

/*******************************************************************************
* 函数名  : OSA_kTaskletStop
* 描  述  : 该函数负责停止一个Tasklet,成功调用后Tasklet将不再调用回调函数，注意事项:
*           1、不能在中断上下文调用
*           2、不能在Tasklet回调函数中调用
*
* 输  入  :  - hTasklet:  Tasklet句柄
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   停止成功
*           OSA_EFAIL: 停止失败
*******************************************************************************/
Int32 OSA_kTaskletStop(OSA_KTaskletHandle hTasklet);

/*******************************************************************************
* 函数名  : OSA_kTaskletDelete
* 描  述  : 该函数负责删除一个Tasklet，注意事项:
*           1、不能在中断上下文调用
*           2、不能在Tasklet回调函数中调用
*           3、成功调用后，不能再继续访问Tasklet句柄
*
* 输  入  :  - hTasklet:  Tasklet句柄
*
* 输  出  : 无。
* 返回值  : OSA_SOK:   删除成功
*           OSA_EFAIL: 删除失败
*******************************************************************************/
Int32 OSA_kTaskletDelete(OSA_KTaskletHandle hTasklet);

#ifdef __cplusplus
}
#endif

#endif

#endif  /*  _OSA_KTASKLET_H_  */


