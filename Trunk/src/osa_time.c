/*******************************************************************************
* osa_time.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-8-2 Create
*
* Desc: ʵ��linux�û�̬OSAģ������ṩ��:
*       1. ��ʱ�ӿ�
*       2. ��ȡϵͳʱ��ӿ�
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#include <osa.h>
#include <osa_priv.h>


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          ȫ�ֱ���������                                    */
/* ========================================================================== */
static OSA_MctsRegisterFun gMctsParam =
{
    .OpGetTimeOfJiffies = NULL,
    .OpGetTimeOfDay     = NULL,
    .OpGetTimeOfDay64   = NULL,
};
/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */

/*******************************************************************************
* ������  : OSA_getTimeOfDay
* ��  ��  : ��ȡ��ǰʱ�䡣
*
* ��  ��  : - pTimeVal: ʱ������ṹ�������΢����ɡ�
*
* ��  ��  : ��
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_getTimeOfDay(OSA_TimeVal *pTimeVal)
{
    Int32          status;

    if (OSA_isNull(gMctsParam.OpGetTimeOfDay))
    {
        struct timeval tv;

        OSA_assertNotNull(pTimeVal);

        status = gettimeofday(&tv, NULL);
        if (OSA_isSuccess(status))
        {
            pTimeVal->tvSec  = (Int32)tv.tv_sec;
            pTimeVal->tvUsec = (Int32)tv.tv_usec;
        }
        else
        {
            OSA_ERROR("Get time failed !\n");
        }
    }
    else
    {
        status = gMctsParam.OpGetTimeOfDay(pTimeVal);
    }
    return status;

}


/*******************************************************************************
* ������  : OSA_getTimeOfDay64
* ��  ��  : ��ȡ��ǰʱ�䡣
*
* ��  ��  : - pTimeVal: ʱ������ṹ�������΢����ɡ�
*
* ��  ��  : ��
* ����ֵ  : OSA_SOK  : �ɹ���
*           OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_getTimeOfDay64(OSA_TimeVal64 *pTimeVal)
{
    Int32          status;

    if (OSA_isNull(gMctsParam.OpGetTimeOfDay64))
    {
        struct timeval tv;

        OSA_assertNotNull(pTimeVal);

        status = gettimeofday(&tv, NULL);
        if (OSA_isSuccess(status))
        {
            pTimeVal->tvSec  = (Int64)tv.tv_sec;
            pTimeVal->tvUsec = (Int64)tv.tv_usec;
        }
        else
        {
            OSA_ERROR("Get time failed !\n");
        }
    }
    else
    {
        status = gMctsParam.OpGetTimeOfDay64(pTimeVal);
    }
    return status;

}



/*******************************************************************************
* ������  : OSA_getTimeOfJiffies
* ��  ��  : ��ȡϵͳ����������ʱ�䡣
* ��  ��  : �ޡ�
* ��  ��  : �ޡ�
* ����ֵ  : ʱ��ֵ����λ�Ǻ��롣
*******************************************************************************/
Uint64 OSA_getTimeOfJiffies(void)
{
    if (OSA_isNull(gMctsParam.OpGetTimeOfJiffies))
    {
        /* ��ǰtick��������ʼ��Ϊһ���м�ֵ�������һ����� */
        static Uint64 curtick = 0x7fffffff;

        /* �ȱ���һ�£�������̷߳��ʳ��� */
        Uint64 tmptick = curtick;

        /* timesʹ�ÿղ���������timesϵͳ����cpu��ʱ */
        Uint64 tick = (Uint32)times(NULL);
        if (tick == (Uint32)-1)    /* �����ȡerrno��ֵ */
        {
            /* glibc�ڴ������ʱ�����ص�errno���ܲ�׼����������� */
            /*tick = (Uint32)-errno;*/
            tick = tmptick;     /* ʹ����һ�κ����ֵ�����ո��� */
        }

        if ((Uint32)tmptick != (Uint32)tick) /* ��32λ�仯˵��tick�仯 */
        {
            while(tick < tmptick)       /* ������� */
            {
                tick += 0xffffffff;
                tick += 1;
            }

            /* ��ǰ������߳������ʱ�䵹�������⣬���Ч�� */
            if(curtick < tick)
            {
                curtick = tick;
            }
        }

        return curtick * (1000 / (Uint32L)sysconf(_SC_CLK_TCK));

    }
    else
    {
        return gMctsParam.OpGetTimeOfJiffies();
    }
}

/*******************************************************************************
*  ������: OSA_registerMcts
*  ��   ��   :ע��ʱ��ͬ��ģ��Ļ�ȡʱ�亯��
*  ��   ��   : ��
*  ��   ��   :
*  ����ֵ: OSA_SOK: �ɹ���
*                      OSA_EFAIL: ʧ�ܡ�
*******************************************************************************/
Int32 OSA_registerMcts(OSA_MctsRegisterFun *pFun)
{
	static Uint32 init = 0;

    if (OSA_isNull(pFun))
    {
        return OSA_EFAIL;
    }

    if (!init)
	{
		gMctsParam.OpGetTimeOfDay     = pFun->OpGetTimeOfDay;
		gMctsParam.OpGetTimeOfDay64	  = pFun->OpGetTimeOfDay64;
		gMctsParam.OpGetTimeOfJiffies = pFun->OpGetTimeOfJiffies;
		init++;
	}

    return OSA_SOK;
}
/*******************************************************************************
* ������  : OSA_getCurTimeInMsec
* ��  ��  : ��ȡ��ǰʱ�̵ĺ�����, ʹ��ʱ��ע��:
*
*           1. �ú������ص�ʱ��ֵ����ʹ��OSA_timeAfter��OSA_timeBefore��OSA_timeEqual��������
*              ���бȽ�
*
*           2. �ú������ص�ʱ��ֵ����ʹ��OSA_timeDelta���������ʱ���Ĳ�ֵ
*
* ��  ��  : ��
*
* ��  ��  : ��
* ����ֵ  : ��ǰϵͳ������
*******************************************************************************/
Uint32 OSA_getCurTimeInMsec(void)
{
    return (Uint32)OSA_getTimeOfJiffies();
}


/*******************************************************************************
* ������  : OSA_msleep
* ��  ��  : ���뼶��ʱ,��ʱ�����л�����
*
* ��  ��  : - ms: ������
*
* ��  ��  : ��
* ����ֵ  : ��
*******************************************************************************/
void   OSA_msleep(Uint32 ms)
{
    struct timespec delayTime, elaspedTime;

    delayTime.tv_sec  = (time_t)(ms / 1000);
    delayTime.tv_nsec = (ms % 1000) * 1000000;

    (void)nanosleep(&delayTime, &elaspedTime);
}


