/*******************************************************************************
* osa_prf.h
*
* Copyright (C) 2010-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2012-7-25 Create
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


#include <osa.h>


Int32 OSA_prfReset(OSA_PrfHandle *phPrf)
{
    memset(phPrf, 0, sizeof(*phPrf) );

    return OSA_SOK;
}


Int32 OSA_prfBegin(OSA_PrfHandle *phPrf)
{
    phPrf->curTime = OSA_getCurTimeInMsec();

    return OSA_SOK;
}


Int32 OSA_prfEnd(OSA_PrfHandle *phPrf, Uint32 value)
{
    Uint32 curTime = OSA_getCurTimeInMsec();

    phPrf->curTime = (Uint32)OSA_timeDelta(curTime, phPrf->curTime);

    phPrf->totalTime += phPrf->curTime;
    
    phPrf->count++;
        
    if(phPrf->curTime > phPrf->maxTime)
    {
        phPrf->maxTime = phPrf->curTime;
    }
    
    if(phPrf->curTime < phPrf->minTime)
    {
        phPrf->minTime = phPrf->curTime;
    }
    
    phPrf->totalValue += value;

    if(value > phPrf->maxValue)
    {
        phPrf->maxValue = value;
    }
    
    if(value < phPrf->minTime)
    {
        phPrf->minValue = phPrf->curValue;
    }
    
    if (phPrf->count >= 100) 
    {
        OSA_prfReset(phPrf);
    }

    return OSA_SOK;
}

/*lint -save -e715 */

/*由于linux内核态不支持浮点运算，因此该函数中使用整型运算*/
#ifdef __KERNEL__
Int32 OSA_prfPrint(OSA_PrfHandle *phPrf, 
                   String         sName, 
                   Uint32         flags)
{
    if ((phPrf->totalTime == 0) 
            || (phPrf->count == 0)) 
    {
        return OSA_EFAIL;
    }
    
    OSA_DEBUG(" \n");
    OSA_DEBUG(" Profile Info  : %s \n", sName);
    OSA_DEBUG(" ======================\n");
        
    if(flags == 0)
    {
        flags = OSA_PRF_PRINT_DEFAULT;
    }
    
    if(flags & OSA_PRF_PRINT_COUNT ) 
    {
        OSA_DEBUG(" Iterations    : %d \n", phPrf->count);
    }

    if(flags & OSA_PRF_PRINT_TIME ) 
    {

        OSA_DEBUG(" Avg Time (ms) : %9d \n", 
                            phPrf->totalTime/phPrf->count);

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Min Time (ms) : %d \n", phPrf->minTime);
        }

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Max Time (ms) : %d \n", phPrf->maxTime);
        }
    }

    if(flags & OSA_PRF_PRINT_TIME_OUTSIDE ) 
    {

        OSA_INFOPUP(" Avg Time (ms) : %9d \n", 
                    phPrf->totalTime/phPrf->count);
        
        OSA_INFOPUP(" Avg Value/sec : %9d \n", 
                  (phPrf->totalValue * 1000)/phPrf->totalTime);
     
    }

    if(flags & OSA_PRF_PRINT_VALUE ) 
    {

        //OSA_printf(" Avg Value     : %9d \n", phPrf->totalValue/phPrf->count);
        OSA_DEBUG(" Avg Value/sec : %9d \n", 
                    (phPrf->totalValue * 1000) / phPrf->totalTime);

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Min Value     : %d \n", phPrf->minValue);
        }

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Max Value     : %d \n", phPrf->maxValue);
        }
    }
    
    OSA_DEBUG(" \n");  

    return OSA_SOK;
}

#else
Int32 OSA_prfPrint(OSA_PrfHandle *phPrf, 
                   String         sName, 
                   Uint32         flags)
{
    if ((phPrf->totalTime == 0) 
            || (phPrf->count == 0)) 
    {
        return OSA_EFAIL;
    }
    
    OSA_DEBUG(" \n");
    OSA_DEBUG(" Profile Info  : %s \n", sName);
    OSA_DEBUG(" ======================\n");
        
    if(flags == 0)
    {
        flags = OSA_PRF_PRINT_DEFAULT;
    }
    
    if(flags & OSA_PRF_PRINT_COUNT ) 
    {
        OSA_DEBUG(" Iterations    : %d \n", phPrf->count);
    }

    if(flags & OSA_PRF_PRINT_TIME ) 
    {

        OSA_DEBUG(" Avg Time (ms) : %9.2f \n", 
                    (Float32)phPrf->totalTime/phPrf->count);

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Min Time (ms) : %d \n", phPrf->minTime);
        }

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Max Time (ms) : %d \n", phPrf->maxTime);
        }
    }

    if(flags & OSA_PRF_PRINT_TIME_OUTSIDE ) 
    {

        OSA_INFOPUP(" Avg Time (ms) : %9.2f \n", 
                    (Float32)phPrf->totalTime/phPrf->count);
        
        OSA_INFOPUP(" Avg Value/sec : %9.2f \n", 
                    (Float32)(phPrf->totalValue*1000)/phPrf->totalTime);
     
    }

    if(flags & OSA_PRF_PRINT_VALUE ) 
    {

        OSA_DEBUG(" Avg Value/sec : %9.2f \n", 
                (Float32)(phPrf->totalValue*1000)/phPrf->totalTime);

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Min Value     : %d \n", phPrf->minValue);
        }

        if(flags & OSA_PRF_PRINT_MIN_MAX ) 
        {
            OSA_DEBUG(" Max Value     : %d \n", phPrf->maxValue);
        }
    }
    
    OSA_DEBUG(" \n");  

    return OSA_SOK;
}

#endif

/*lint -restore */


#ifdef __KERNEL__
EXPORT_SYMBOL(OSA_prfBegin);
EXPORT_SYMBOL(OSA_prfEnd);
EXPORT_SYMBOL(OSA_prfReset);
EXPORT_SYMBOL(OSA_prfPrint);
#endif


