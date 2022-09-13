/*******************************************************************************
* osa_prf.h
*
* Description: OSA时间统计模块。可用于统计时间。
*
*       1. 硬件说明。
*          无。
*
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


#ifndef _OSA_PRF_H_
#define _OSA_PRF_H_


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

#define OSA_PRF_PRINT_DEFAULT   \
            (OSA_PRF_PRINT_TIME|OSA_PRF_PRINT_VALUE)

#define OSA_PRF_PRINT_ALL           (0xFFFF)

#define OSA_PRF_PRINT_TIME          (0x0001)
#define OSA_PRF_PRINT_VALUE         (0x0002)
#define OSA_PRF_PRINT_MIN_MAX       (0x0004)
#define OSA_PRF_PRINT_COUNT         (0x0008)
#define OSA_PRF_PRINT_TIME_OUTSIDE  (0x0010)


/* 打印的统计信息格式如下 */
/*
Profile info  : <name>
======================
Iterations    :
Avg Time (ms) :
Max Time (ms) :
Min Time (ms) :
Avg Value     :
Avg Value/sec :
Max Value     :
Min Value     :
*/


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */

typedef struct 
{
	Uint32 totalTime;
	Uint32 maxTime;
	Uint32 minTime;

	Uint32 totalValue;
	Uint32 maxValue;
	Uint32 minValue;

	Uint32 count;
	Uint32 curTime;
	Uint32 curValue;
} OSA_PrfHandle;


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */

/*******************************************************************************
* 函数名  : OSA_prfBegin
* 描  述  : 开始时间统计。          
* 输  入  : - phPrf: 时间统计句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_prfBegin(OSA_PrfHandle *phPrf);


/*******************************************************************************
* 函数名  : OSA_prfEnd
* 描  述  : 结束时间统计。          
* 输  入  : - phPrf: 时间统计句柄。
*           - value: 用户指定需要统计的频度值，一般为1。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_prfEnd(OSA_PrfHandle *phPrf, Uint32 value);


/*******************************************************************************
* 函数名  : OSA_prfBegin
* 描  述  : 复位时间统计。          
* 输  入  : - phPrf: 时间统计句柄。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_prfReset(OSA_PrfHandle *phPrf);


/*******************************************************************************
* 函数名  : OSA_prfBegin
* 描  述  : 复位时间统计。          
* 输  入  : - phPrf: 时间统计句柄。
*           - sName: 统计时间模块的名字。
*           - flags: 其定义请参见OSA_PRF_PRINT_XXX。
* 输  出  : 无。
* 返回值  : OSA_SOK  : 成功。
*           OSA_EFAIL: 失败。
*******************************************************************************/
Int32 OSA_prfPrint(OSA_PrfHandle *phPrf, 
                   String         sName, 
                   Uint32         flags);


#endif /* _OSA_PRF_H_ */



