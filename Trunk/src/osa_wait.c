

#include <osa.h>

/* 该接口用OSA_msleep替代 */
#if 0
void OSA_waitMsecs(Uint32 msecs)
{
    struct timespec delayTime, elaspedTime;

    delayTime.tv_sec  = msecs/1000;
    delayTime.tv_nsec = (msecs%1000)*1000000;

    nanosleep(&delayTime, &elaspedTime);
}
#endif


Int32 OSA_select(Int32 fd, Int32 usec)
{
    Int32 status;
    fd_set fds; 
    struct timeval tv;

    FD_ZERO (&fds);
    FD_SET (fd, &fds); 
    
    tv.tv_sec  = 0;
    tv.tv_usec = usec;  
    status = select(fd + 1, &fds, NULL, NULL, &tv);
    if (status <= 0)    
    {
        OSA_WARN("select()\n");
        return OSA_EFAIL;
    }
    
    return OSA_SOK;
}


