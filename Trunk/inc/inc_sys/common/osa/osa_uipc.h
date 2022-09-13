
#ifndef _OSA_UIPC_H_
#define _OSA_UIPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "osa_file.h"

/*仅linux用户态支持以下接口*/
#if !defined(__KERNEL__) && !defined(___DSPBIOS___)

#define OSA_IPC_FLAG_OPEN      (0x0)
#define OSA_IPC_FLAG_CREATE    (0x1)

#define OSA_IPC_KEY_INVALID    ((Uint32)-1)

#define OSA_IPC_MBX_MSG_SIZE_MAX    (OSA_KB)

typedef struct {

  Int32 id;
  Int8 *ptr;

} OSA_IpcShmHndl;

typedef struct {

    Int32 id;

} OSA_IpcMbxHndl;

typedef struct {

    Int32 id;
    Int32 members;
    Int32 maxVal;

} OSA_IpcSemHndl;

typedef struct {

    long  type;
    Uint8 data[OSA_IPC_MBX_MSG_SIZE_MAX];

} OSA_IpcMsgHndl;

Uint32 OSA_ipcMakeKey(char *path, Int8 id);

/* shared memory */
Int8 *OSA_ipcShmOpen(OSA_IpcShmHndl *pHndl, Uint32 key, Uint32 size,
                    Uint32 flags);
Int32 OSA_ipcShmClose(OSA_IpcShmHndl *pHndl);

/* message box */
Int32 OSA_ipcMbxOpen(OSA_IpcMbxHndl *pHndl, Uint32 key, Uint32 flags);

Int32 OSA_ipcMbxSend(OSA_IpcMbxHndl *pHndl, OSA_IpcMsgHndl *msgData,
                        Uint32 msgSize, Int32 msgFlag);

Int32 OSA_ipcMbxRecv(OSA_IpcMbxHndl *pHndl, OSA_IpcMsgHndl *msgData,
                        Uint32 msgSize, Int32 msgFlag);

Int32 OSA_ipcMbxClose(OSA_IpcMbxHndl *pHndl);

/* semaphore */
Int32 OSA_ipcSemOpen(OSA_IpcSemHndl *pHndl, Uint32 key, Uint32 members,
                        Uint32 initVal, Uint32 maxVal, Uint32 flags);

Int32 OSA_ipcSemLock(OSA_IpcSemHndl *pHndl, Uint32 member, Uint32 timeout);

Int32 OSA_ipcSemUnlock(OSA_IpcSemHndl *pHndl, Uint32 member,
                        Uint32 timeout);

Int32 OSA_ipcSemGetVal(OSA_IpcSemHndl *pHndl, Uint32 member);

Int32 OSA_ipcSemClose(OSA_IpcSemHndl *pHndl);


/*!< \brief FIFO  */

/**---------------------------------------------------------------------------@n
\brief 创建文件名为pathname的FIFO文件

\param [in] pathname: 包含文件路径信息的完整文件名
\param [in] mode: 件操作模式,参见OSA_FileMode定义
    若OSA_FILEMODE_RDWR，已读写方式创建FIFO文件，若不存在则创建文件，原有数据将被清掉，从文件头开始读写

\return OSA_SOK 成功，OSA_EFAIL 失败
------------------------------------------------------------------------------*/
Int32 OSA_ipcMkfifo(Char *pathname, OSA_FileMode mode);

#endif

#ifdef __cplusplus
}
#endif

#endif  /*  _OSA_IPC_ */




