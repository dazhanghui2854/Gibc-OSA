/*lint -save --e{754} */

#ifndef __ANDROID__

#include <osa.h>


//#define OSA_IPC_MBX_MSG_TYPE_ID   (0x0000ACBD)
#define OSA_IPC_SEM_MEMBERS_MAX   (8)


union semun {
    Int32 val;
    struct semid_ds *buf;
    unsigned short  *array;
} sem_arg;


Uint32 OSA_ipcMakeKey(char *path, Int8 id)
{
    key_t key;

    key = ftok(path, id);

    return (Uint32)key;
}


/* shared memory */
Int8 *OSA_ipcShmOpen(OSA_IpcShmHndl *pHndl, Uint32 key, 
                        Uint32 size, Uint32 flags)
{
    pHndl->ptr = NULL;

    if(flags & OSA_IPC_FLAG_CREATE) {
        pHndl->id = shmget((key_t)key, size, IPC_CREAT | 0666);/* IPC_EXCL */
    } else {
        pHndl->id = shmget((key_t)key, size, 0);
    }

    if(pHndl->id < 0)
        return pHndl->ptr;

    pHndl->ptr = shmat(pHndl->id, 0, 0);

    if(pHndl->ptr == (char*)-1)
        pHndl->ptr = NULL;

    return pHndl->ptr;
}


Int32   OSA_ipcShmClose(OSA_IpcShmHndl *pHndl)
{
    return shmctl(pHndl->id, IPC_RMID, 0);
}


/* message box */
Int32 OSA_ipcMbxOpen(OSA_IpcMbxHndl *pHndl, Uint32 key, Uint32 flags)
{
    if(flags & OSA_IPC_FLAG_CREATE) 
    {
        pHndl->id = msgget((key_t)key, IPC_CREAT | 0660); /* IPC_EXCL */
    }
    else
    {
        pHndl->id = msgget((key_t)key, 0);
    }

    if(pHndl->id < 0)
    {   
        return OSA_EFAIL;
    }

    return OSA_SOK;
}


Int32 OSA_ipcMbxSend(OSA_IpcMbxHndl *pHndl, OSA_IpcMsgHndl *msgData,
                        Uint32 msgSize, Int32 msgFlag)
{
    Int32 status = OSA_SOK;


    status = msgsnd(pHndl->id, msgData, msgSize, msgFlag);

    return status;
}


Int32 OSA_ipcMbxRecv(OSA_IpcMbxHndl *pHndl, OSA_IpcMsgHndl *msgData,
                        Uint32 msgSize, Int32 msgFlag)
{
    Int32 status = OSA_SOK;

    //msgData->type = OSA_IPC_MBX_MSG_TYPE_ID;

    status = msgrcv(pHndl->id, msgData, msgSize, msgData->type, msgFlag);

    return status;
}


Int32 OSA_ipcMbxClose(OSA_IpcMbxHndl *pHndl)
{
    return msgctl(pHndl->id, IPC_RMID, 0);
}


/* semaphores */
Int32 OSA_ipcSemOpen(OSA_IpcSemHndl *pHndl, Uint32 key, Uint32 members,
                        Uint32 initVal, Uint32 maxVal, Uint32 flags)
{
    Int32 cntr;
    union semun semopts;

    pHndl->members = (Int32)members;
    pHndl->maxVal = (Int32)maxVal;

    if(initVal > maxVal)
        initVal = maxVal;

    if(flags & OSA_IPC_FLAG_CREATE) {

        if(members > OSA_IPC_SEM_MEMBERS_MAX)
            return OSA_EFAIL;

        pHndl->id = semget((key_t)key, (Int32)members, IPC_CREAT | 0660);/* IPC_EXCL */
        if(pHndl->id < 0 )
            return OSA_EFAIL;

        semopts.val = (Int32)initVal;

        for(cntr = 0; cntr < (Int32)members; cntr++)
            semctl(pHndl->id, cntr, SETVAL, semopts);

    } else {
        pHndl->id = semget((key_t)key, 0, 0660);
    }

    if(pHndl->id < 0 )
        return OSA_EFAIL;

    return OSA_SOK;
}


Int32 OSA_ipcSemLock(OSA_IpcSemHndl *pHndl, Uint32 member, Uint32 timeout)
{
    struct sembuf sem_op={ 0, -1, 0};

    if( (Int32)member > (pHndl->members-1) ) {
        return OSA_EFAIL;
    }
    
    sem_op.sem_flg |= SEM_UNDO;

    sem_op.sem_num = (Uint16)member;

    if(timeout==OSA_TIMEOUT_NONE)
        sem_op.sem_flg |= IPC_NOWAIT;

    if( (semop(pHndl->id, &sem_op, 1)) == -1) {
        return OSA_EFAIL;
    }

  return OSA_SOK;
}


Int32 OSA_ipcSemUnlock(OSA_IpcSemHndl *pHndl, Uint32 member, 
                        Uint32 timeout)
{
    struct sembuf sem_op={ 0, 1, 0};

    if( (Int32)member > (pHndl->members - 1) ) {
        return OSA_EFAIL;
    }

#if 0
    if(OSA_ipcSemGetVal(pHndl, member) >= hndl->maxVal)
        return OSA_SOK;
#endif

    sem_op.sem_flg |= SEM_UNDO;

    sem_op.sem_num = (Uint16)member;

    if(timeout==OSA_TIMEOUT_NONE)
        sem_op.sem_flg |= IPC_NOWAIT;

    if( (semop(pHndl->id, &sem_op, 1)) == -1) {
        return OSA_EFAIL;
    }

  return OSA_SOK;
}


Int32 OSA_ipcSemGetVal(OSA_IpcSemHndl *pHndl, Uint32 member)
{
    return semctl(pHndl->id,(Int32)member, GETVAL, 0);
}

Int32 OSA_ipcSemClose(OSA_IpcSemHndl *pHndl)
{
    return semctl(pHndl->id, 0, IPC_RMID, 0);
}

/**---------------------------------------------------------------------------@n
\brief 创建文件名为pathname的FIFO文件

\param [in] pathname: 包含文件路径信息的完整文件名
\param [in] mode: 件操作模式,参见OSA_FileMode定义
    若OSA_FILEMODE_RDWR，已读写方式创建FIFO文件，若不存在则创建文件，原有数据将被清掉，从文件头开始读写

\return OSA_SOK 成功，OSA_EFAIL 失败 
------------------------------------------------------------------------------*/
Int32 OSA_ipcMkfifo(Char *pathname, OSA_FileMode mode)
{
    Uint32 mask;
    
    if(NULL == pathname)
    {
        OSA_ERROR("NULL == pathname\n");
        return OSA_EFAIL;
    }

    switch(mode)
    {
        case OSA_FILEMODE_RDWR:
            mask = O_RDWR | O_CREAT | O_TRUNC;
            break;
        default:
            OSA_ERROR("mode is unsupported\n");
            return OSA_EFAIL;
    }

    if(mkfifo(pathname, mask) != 0)
    {
        OSA_ERROR("mkfifo failed\n");
        return OSA_EFAIL;
    }

    return OSA_SOK;
}

#endif

/*lint -restore */

