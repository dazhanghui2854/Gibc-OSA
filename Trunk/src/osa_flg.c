

#include <osa.h>

typedef struct
{    
    Uint32 pattern;
    pthread_mutex_t     lock;        /*互斥锁*/
    pthread_cond_t      cond;        /*条件变量*/
} OSA_FlgObject;





Int32  OSA_flgCreate(OSA_FlgHndl *pHndl, Uint32 initPattern)
{
    OSA_FlgObject *pObj;

    if (OSA_isNull(pHndl))
    {
        OSA_ERROR("hndl is NULL\n");
        return OSA_EFAIL;
    }

    pObj = (OSA_FlgObject *)OSA_memAlloc(sizeof(OSA_FlgObject));
    if(NULL == pObj)
    {
        OSA_ERROR("alloc pObj failed\n");
        return OSA_EFAIL;
    }
    
    pObj->pattern = initPattern;

    pthread_mutex_init(&pObj->lock, NULL);
    pthread_cond_init(&pObj->cond, NULL);

    *pHndl = (OSA_FlgHndl)pObj;

    return OSA_SOK;
}

Int32  OSA_flgWait(OSA_FlgHndl hndl, Uint32 pattern,
                    Uint32 mode, Uint32 timeout)
{
  Int32           status = OSA_EFAIL, found;
  Uint32          setPattern;
  OSA_FlgObject  *pObj = (OSA_FlgObject *)hndl;
  struct timespec waitTime;
  OSA_TimeVal     timeVal;

  OSA_assertNotNull(hndl);

  pthread_mutex_lock(&pObj->lock);

  while(1) {

    found = 0;
    setPattern = pObj->pattern & pattern;

    if(mode & OSA_FLG_MODE_AND) {
        if( setPattern == pattern )
            found = 1;
    } else {
        if( setPattern )
            found = 1;
    }

    if(found) {
        if(mode & OSA_FLG_MODE_CLR)
            pObj->pattern &= ~setPattern;

        status = OSA_SOK;
        break;

    } else {
        if(timeout==OSA_TIMEOUT_NONE)
        {        
            status = OSA_EFAIL;
            break;
        }

        if(timeout == (Uint32)OSA_TIMEOUT_FOREVER)
        {
            status = pthread_cond_wait(&pObj->cond, &pObj->lock);
            if(0 != status)
            {
                OSA_ERROR("pthread_cond_wait failed, ret:%d\n", status);
                status = OSA_EFAIL;
                break;
            }
        }
        else
        {
            waitTime.tv_sec  = (Int32L)timeout/1000;
            waitTime.tv_nsec = ((Int32L)timeout%1000)*1000000;

            OSA_getTimeOfDay(&timeVal);
            waitTime.tv_sec  += timeVal.tvSec;
            waitTime.tv_nsec += timeVal.tvUsec * 1000;
            if(waitTime.tv_nsec >= 1000000000)
            {
                waitTime.tv_sec  += 1;
                waitTime.tv_nsec %= 1000000000;
            }

            status = pthread_cond_timedwait(&pObj->cond, &pObj->lock, &waitTime);
            if(ETIMEDOUT == status)
            {                
                status = OSA_ETIMEOUT;
                break;
            }
            else if(0 != status)
            {
                OSA_ERROR("pthread_cond_timedwait failed, ret:%d\n", status);
                status = OSA_EFAIL;
                break;
            }
        }
    }
  }

  pthread_mutex_unlock(&pObj->lock);

  return status;
}


Int32  OSA_flgSet(OSA_FlgHndl hndl, Uint32 pattern)
{
    Int32 status = OSA_SOK;
    OSA_FlgObject *pObj = (OSA_FlgObject *)hndl;
    
    OSA_assertNotNull(hndl);

    pthread_mutex_lock(&pObj->lock);

    pObj->pattern |= pattern;
    pthread_cond_signal(&pObj->cond);

    pthread_mutex_unlock(&pObj->lock);

    return status;
}


Int32  OSA_flgClear(OSA_FlgHndl hndl, Uint32 pattern)
{
    Int32 status = OSA_SOK;
    OSA_FlgObject *pObj = (OSA_FlgObject *)hndl;

    OSA_assertNotNull(hndl);

    pthread_mutex_lock(&pObj->lock);

    pObj->pattern &= ~pattern;

    pthread_mutex_unlock(&pObj->lock);

    return status;
}


Bool OSA_flgIsSet(OSA_FlgHndl hndl, Uint32 pattern)
{
    Int32 status;

    status = OSA_flgWait(hndl, pattern, OSA_FLG_MODE_AND, OSA_TIMEOUT_NONE);

    if(status!=OSA_SOK)
        return OSA_FALSE;

    return OSA_TRUE;    
}


Int32  OSA_flgDelete(OSA_FlgHndl hndl)
{
    OSA_FlgObject *pObj = (OSA_FlgObject *)hndl;
    
    OSA_assertNotNull(hndl);
    
    pthread_cond_destroy(&pObj->cond);
    pthread_mutex_destroy(&pObj->lock);

    if(OSA_SOK != OSA_memFree(pObj))
		OSA_ERROR("memory free failed\n");

    return OSA_SOK;
}

