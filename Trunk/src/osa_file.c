/*******************************************************************************
* osa_file.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: ʵ��linux�û�̬OSAģ������ṩ���ļ������ӿ�
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
/*�����ļ�����ṹ��*/
typedef struct
{
    Uint32              nMgicNum; /*ħ��,����У������Ч�ԡ�*/
    FILE *              pFile;    /*�û�̬file����ָ��*/
    Uint32              mode;     /*�û�̬�ļ�����ģʽ*/
}OSA_FileObject;



/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          ȫ�ֱ���������                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */


/*******************************************************************************
* ������  : OSA_fileAccess
* ��  ��  : �ýӿڿ������ж��ļ��Ƿ�������Ƿ���Ȩ�޷���,ͨ��mode����ָ�����ʵ�Ȩ�ޡ�
*           ��modeΪOSA_FILEMODE_RDONLY,����ļ��Ƿ��ж���Ȩ��
*           ��modeΪOSA_FILEMODE_WRONLY,����ļ��Ƿ���д��Ȩ��
*           ��modeΪOSA_FILEMODE_WRAPPEND,����ļ��Ƿ���д��Ȩ��
*           ��modeΪOSA_FILEMODE_RDWR,����ļ��Ƿ��ж���д��Ȩ��
*           ��modeΪOSA_FILEMODE_FEXIST���������ļ��Ƿ����
*
* ��  ��  : - fileName: �����ļ�·����Ϣ�������ļ���
*           - mode:     �ļ�����ģʽ,�μ�OSA_FileMode����
* ��  ��  : ��
* ����ֵ  : OSA_SOK:    �ļ���������Ȩ�޷���
*           OSA_EFAIL:  �ļ������ڻ���û��Ȩ�޷���
*******************************************************************************/
Int32 OSA_fileAccess(const Char *fileName, OSA_FileMode mode)
{
    Int32 ret = 0;
    Int32 internelMode = 0;

    if(NULL == fileName)
    {
        OSA_ERROR("NULL fileName\n");
        return OSA_EFAIL;
    }

    switch(mode)
    {
        case OSA_FILEMODE_RDONLY:
            internelMode = R_OK;
            break;
        case OSA_FILEMODE_WRONLY:
        case OSA_FILEMODE_WRAPPEND:
            internelMode = W_OK;
            break;
        case OSA_FILEMODE_RDWR:
            internelMode = F_OK;
            break;
        case OSA_FILEMODE_RDWR_NOCLR:
            internelMode = F_OK;
            break;
        case OSA_FILEMODE_FEXIST:
            internelMode = F_OK;
            break;
        default:
            OSA_ERROR("Invalid mode:%d\n", mode);
            return OSA_EFAIL;
    }

    ret = access(fileName, internelMode);

    return (0 == ret) ? OSA_SOK : OSA_EFAIL;
}

/*******************************************************************************
* ������  : OSA_fileRemove
* ��  ��  : �ýӿڿ�����ɾ��fileName�ƶ�·���µ��ļ����ļ���
*           �ú�����ʱ��֧��Linux�û�̬
*
* ��  ��  : - fileName: �����ļ�·����Ϣ�������ļ���
* ��  ��  : ��
* ����ֵ  : OSA_SOK:    �ļ�ɾ���ɹ�
*           OSA_EFAIL:  �ļ�ɾ��ʧ��
*******************************************************************************/
Int32 OSA_fileRemove(const Char *fileName)
{
    if(NULL == fileName)
    {
        OSA_ERROR("NULL fileName\n");
        return OSA_EFAIL;
    }

    if(remove(fileName) != 0)
    {
        OSA_ERROR("remove failed\n");
        return OSA_EFAIL;
    }

    return OSA_SOK;
}

/*******************************************************************************
* ������  : OSA_fileOpen
* ��  ��  : �ú��������һ���ļ�,֧�����ֲ���ģʽ: ֻ����ֻд����д������׷��д
*           ���ļ��򿪳ɹ�,������һ���ļ����,�����������ļ������ӿ�ʱ����
*
* ��  ��  : - fileName: �����ļ�·����Ϣ�������ļ���
*           - mode:     �ļ�����ģʽ,�μ�OSA_FileMode����
*
* ��  ��  : - phFile:   �ļ����ָ��,���򿪳ɹ�ʱ����ļ����
* ����ֵ  :  OSA_SOK:   �򿪳ɹ�
*            OSA_EFAIL: ��ʧ��
*******************************************************************************/
Int32 OSA_fileOpen(const Char *fileName, OSA_FileMode mode,
                    OSA_FileHandle *phFile)
{
    Int32  fd = -1;
    FILE   *pFile = NULL;
    OSA_FileObject *pFileObj = NULL;
    const Char *pFlag = NULL;

    if(NULL == fileName)
    {
        OSA_ERROR("NULL fileName\n");
        return OSA_EFAIL;
    }

    if(NULL == phFile)
    {
        OSA_ERROR("NULL phFile\n");
        return OSA_EFAIL;
    }

    switch(mode)
    {
        case OSA_FILEMODE_RDONLY:
            pFlag = "r";
            break;
        case OSA_FILEMODE_WRONLY:
            pFlag = "w";
            break;
        case OSA_FILEMODE_RDWR:
            pFlag = "w+";
            break;
        case OSA_FILEMODE_WRAPPEND:
            pFlag = "a";
            break;
        case OSA_FILEMODE_RDWR_NOCLR:
            pFlag = "w+";
            break;
        default:
            OSA_ERROR("Invalid mode:%d\n", mode);
            return OSA_EFAIL;
    }

    if(OSA_FILEMODE_RDWR_NOCLR == mode)
    {
#ifdef __ANDROID__

        fd = open(fileName, O_RDWR | O_CREAT, S_IRWXU);
#else
        fd = open(fileName, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
#endif
        if(fd < 0)
        {
            OSA_ERROR("Open %s failed\n", fileName);
            return OSA_EFAIL;
        }

        pFile = fdopen(fd, pFlag);
    }
    else
    {
        pFile = fopen(fileName, pFlag);
    }

    if(NULL == pFile)
    {
        OSA_ERROR("Open %s failed\n", fileName);
        goto closeFd;
    }

    /*�����ڴ�*/
    pFileObj = (OSA_FileObject *)OSA_memAlloc(sizeof(OSA_FileObject));
    if(NULL == pFileObj)
    {
        OSA_ERROR("alloc pFileObj failed\n");
        goto closeFd;
    }

    pFileObj->mode = (Uint32)mode;
    pFileObj->nMgicNum = OSA_MAGIC_NUM;
    pFileObj->pFile = pFile;
    *phFile = (OSA_FileHandle)pFileObj;
    return OSA_SOK;

closeFd:
    if(fd >= 0)
    {
        close(fd);
    }
    if (pFile)
    {
        fclose(pFile);
    }

    return OSA_EFAIL;

}

/*******************************************************************************
* ������  : OSA_fileClose
* ��  ��  : �ú�������ر�һ���ļ�,�������ļ����ָ��Ķ���,
*           ��˵��øýӿں�ԭ�е��ļ����������ʹ�á�
*
* ��  ��  : - hFile: �ļ����
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �رճɹ�
*           OSA_EFAIL: �ر�ʧ��
*******************************************************************************/
Int32 OSA_fileClose(OSA_FileHandle hFile)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;

    OSA_handleCheck(pFileObj);

    if(pFileObj->pFile)
    {
        fclose(pFileObj->pFile);
    }

    /*ħ����Ϊ-1���������������ͷź������*/
    pFileObj->nMgicNum = (Uint32)-1;
    if(OSA_SOK != OSA_memFree(pFileObj))
		OSA_ERROR("memory free failed\n");
    return OSA_SOK;
}

/*******************************************************************************
* ������  : OSA_fileRead
* ��  ��  : �ú�������ʵ���ļ��Ķ�����,����������ɵ���������,�ҵ����߱��뱣֤
*           buffer�ĳ���Ҫ���ڵ���size
*
* ��  ��  : - hFile:   �ļ����
*           - buffer:  ��������
*           - size:    Ҫ��ȡ�ĳ���
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: ��ȡʧ��
*           >= 0:      ������ȡ���ֽ���
*******************************************************************************/
Int32 OSA_fileRead (OSA_FileHandle hFile, Int8 *buffer, Uint32 size)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    Int32 ret = 0;

    OSA_handleCheck(pFileObj);

    if(NULL == buffer)
    {
        OSA_ERROR("NULL buffer\n");
        return OSA_EFAIL;
    }

    ret = (Int32)fread(buffer, 1, size, pFileObj->pFile);

    return ret;
}

/*******************************************************************************
* ������  : OSA_fileWrite
* ��  ��  : �ú�������ʵ���ļ���д����,������buffer�����ɵ���������,�ҵ����߱��뱣֤
*           buffer�ĳ���Ҫ���ڵ���size
*
* ��  ��  : - hFile:   �ļ����
*           - buffer:  д������
*           - size:    Ҫд��ĳ���
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: д��ʧ��
*           >= 0:      ����д����ֽ���
*******************************************************************************/
Int32 OSA_fileWrite(OSA_FileHandle hFile, Int8 *buffer, Uint32 size)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    Int32 ret = 0;

    OSA_handleCheck(pFileObj);

    if(NULL == buffer)
    {
        OSA_ERROR("NULL buffer\n");
        return OSA_EFAIL;
    }

    ret = (Int32)fwrite(buffer, 1, size, pFileObj->pFile);

    return ret;
}

/*******************************************************************************
* ������  : OSA_fileSeek
* ��  ��  : �ú�������ʵ���ļ���Seek����
*
* ��  ��  : - hFile:   �ļ����
*           - offset:  seek��ƫ��
*           - type:    seek������,�μ�OSA_FileSeekType����
* ��  ��  : ��
* ����ֵ  : OSA_EFAIL: seekʧ��
*           OSA_SOK:   seek�ɹ�
*******************************************************************************/
Int32 OSA_fileSeek (OSA_FileHandle hFile, Int32 offset, Int32 type)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    Int32 ret = 0;

    OSA_handleCheck(pFileObj);

    ret = fseek(pFileObj->pFile, offset, type);
    if(ret < 0)
    {
        OSA_ERROR("seek failed, ret:%d\n", ret);
        return OSA_EFAIL;
    }

    return OSA_SOK;
}

/*******************************************************************************
* ������  : OSA_fileTell
* ��  ��  : �ú�����ȡ��ǰ�ļ��Ķ�дλ��
*
* ��  ��  : - hFile:   �ļ����
* ��  ��  : - pOffset: ��ǰ�Ķ�дλ��
* ����ֵ  : OSA_EFAIL: ʧ��
*           OSA_SOK:   �ɹ�
*******************************************************************************/
Int32 OSA_fileTell (OSA_FileHandle hFile, Uint32 *pOffset)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    Int32 ret = 0;

    OSA_handleCheck(pFileObj);

    if(NULL == pOffset)
    {
        OSA_ERROR("NULL pOffset\n");
        return OSA_EFAIL;
    }

    if(NULL == pFileObj->pFile)
    {
        OSA_ERROR("NULL pFileObj->pFile\n");
        return OSA_EFAIL;
    }

    ret = ftell(pFileObj->pFile);
    if(ret < 0)
    {
        OSA_ERROR("ftell failed\n");
        return OSA_EFAIL;
    }

    *pOffset = (Uint32)ret;

    return OSA_SOK;
}

/*******************************************************************************
* ������  : OSA_fileSync
* ��  ��  : �ú��������ļ������������ˢ��Ӳ���������û�̬��Ч���ں�̬���ú���Ч����
*
* ��  ��  : - hFile: �ļ����
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �ɹ�
*           OSA_EFAIL: ʧ��
*******************************************************************************/
Int32 OSA_fileSync(OSA_FileHandle hFile)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;
    Int32 ret = OSA_EFAIL;

    OSA_handleCheck(pFileObj);

    ret = fflush(pFileObj->pFile);
    if(OSA_isFail(ret))
    {
        OSA_ERROR("fflush failed\n");
        return OSA_EFAIL;
    }

    ret = fsync(fileno(pFileObj->pFile));
    if(OSA_isFail(ret))
    {
        OSA_ERROR("fsync failed\n");
        return OSA_EFAIL;
    }

    return OSA_SOK;
}

/*******************************************************************************
* ������  : OSA_fgets
* ��  ��  : �ú�����ȡ��ǰ�ļ��е�һ���Ի��л���EOFΪ��β������
*           �ú�����֧��Linux�û�̬
*
* ��  ��  : - hFile:   �ļ����
*           - size: �ַ�����󳤶�
* ��  ��  : - s: ��ȡ�����ַ���
* ����ֵ  : OSA_EFAIL: ʧ��
*           OSA_SOK:   �ɹ�
*******************************************************************************/
Int32 OSA_fgets(Char *s, Int32 size, OSA_FileHandle hFile)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;

    OSA_handleCheck(pFileObj);

    if(NULL == fgets(s, size, pFileObj->pFile))
    {
        OSA_ERROR("fgets failed\n");
        return OSA_EFAIL;
    }

    return OSA_SOK;
}
