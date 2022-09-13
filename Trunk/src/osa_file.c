/*******************************************************************************
* osa_file.c
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Zheng wei <zheng_wei@dahuatech.com>
* Version: V1.0.0  2012-5-23 Create
*
* Desc: 实现linux用户态OSA模块对外提供的文件操作接口
*
* Modification:
*    Date    :
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/

/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */
#include <osa.h>
#include <osa_priv.h>


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */
/*定义文件对象结构体*/
typedef struct
{
    Uint32              nMgicNum; /*魔数,用于校验句柄有效性。*/
    FILE *              pFile;    /*用户态file对象指针*/
    Uint32              mode;     /*用户态文件操作模式*/
}OSA_FileObject;



/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */


/* ========================================================================== */
/*                          全局变量定义区                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数定义区                                        */
/* ========================================================================== */


/*******************************************************************************
* 函数名  : OSA_fileAccess
* 描  述  : 该接口可用于判断文件是否存在且是否有权限访问,通过mode参数指定访问的权限。
*           若mode为OSA_FILEMODE_RDONLY,检查文件是否有读的权限
*           若mode为OSA_FILEMODE_WRONLY,检查文件是否有写的权限
*           若mode为OSA_FILEMODE_WRAPPEND,检查文件是否有写的权限
*           若mode为OSA_FILEMODE_RDWR,检查文件是否有读和写的权限
*           若mode为OSA_FILEMODE_FEXIST，仅测试文件是否存在
*
* 输  入  : - fileName: 包含文件路径信息的完整文件名
*           - mode:     文件操作模式,参见OSA_FileMode定义
* 输  出  : 无
* 返回值  : OSA_SOK:    文件存在且有权限访问
*           OSA_EFAIL:  文件不存在或者没有权限访问
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
* 函数名  : OSA_fileRemove
* 描  述  : 该接口可用于删除fileName制定路径下的文件或文件夹
*           该函数暂时仅支持Linux用户态
*
* 输  入  : - fileName: 包含文件路径信息的完整文件名
* 输  出  : 无
* 返回值  : OSA_SOK:    文件删除成功
*           OSA_EFAIL:  文件删除失败
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
* 函数名  : OSA_fileOpen
* 描  述  : 该函数负责打开一个文件,支持四种操作模式: 只读、只写、读写创建、追加写
*           若文件打开成功,将返回一个文件句柄,供调用其他文件操作接口时传入
*
* 输  入  : - fileName: 包含文件路径信息的完整文件名
*           - mode:     文件操作模式,参见OSA_FileMode定义
*
* 输  出  : - phFile:   文件句柄指针,当打开成功时输出文件句柄
* 返回值  :  OSA_SOK:   打开成功
*            OSA_EFAIL: 打开失败
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

    /*分配内存*/
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
* 函数名  : OSA_fileClose
* 描  述  : 该函数负责关闭一个文件,将销毁文件句柄指向的对象,
*           因此调用该接口后原有的文件句柄不能再使用。
*
* 输  入  : - hFile: 文件句柄
* 输  出  : 无
* 返回值  : OSA_SOK:   关闭成功
*           OSA_EFAIL: 关闭失败
*******************************************************************************/
Int32 OSA_fileClose(OSA_FileHandle hFile)
{
    OSA_FileObject *pFileObj = (OSA_FileObject *)hFile;

    OSA_handleCheck(pFileObj);

    if(pFileObj->pFile)
    {
        fclose(pFileObj->pFile);
    }

    /*魔数设为-1，避免其他人在释放后继续用*/
    pFileObj->nMgicNum = (Uint32)-1;
    if(OSA_SOK != OSA_memFree(pFileObj))
		OSA_ERROR("memory free failed\n");
    return OSA_SOK;
}

/*******************************************************************************
* 函数名  : OSA_fileRead
* 描  述  : 该函数负责实现文件的读操作,读缓冲必须由调用者申请,且调用者必须保证
*           buffer的长度要大于等于size
*
* 输  入  : - hFile:   文件句柄
*           - buffer:  读缓冲区
*           - size:    要读取的长度
* 输  出  : 无
* 返回值  : OSA_EFAIL: 读取失败
*           >= 0:      真正读取的字节数
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
* 函数名  : OSA_fileWrite
* 描  述  : 该函数负责实现文件的写操作,缓冲区buffer必须由调用者申请,且调用者必须保证
*           buffer的长度要大于等于size
*
* 输  入  : - hFile:   文件句柄
*           - buffer:  写缓冲区
*           - size:    要写入的长度
* 输  出  : 无
* 返回值  : OSA_EFAIL: 写入失败
*           >= 0:      真正写入的字节数
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
* 函数名  : OSA_fileSeek
* 描  述  : 该函数负责实现文件的Seek操作
*
* 输  入  : - hFile:   文件句柄
*           - offset:  seek的偏移
*           - type:    seek的类型,参见OSA_FileSeekType定义
* 输  出  : 无
* 返回值  : OSA_EFAIL: seek失败
*           OSA_SOK:   seek成功
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
* 函数名  : OSA_fileTell
* 描  述  : 该函数获取当前文件的读写位置
*
* 输  入  : - hFile:   文件句柄
* 输  出  : - pOffset: 当前的读写位置
* 返回值  : OSA_EFAIL: 失败
*           OSA_SOK:   成功
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
* 函数名  : OSA_fileSync
* 描  述  : 该函数负责将文件缓冲里的数据刷到硬件，仅在用户态有效，内核态调用后无效果。
*
* 输  入  : - hFile: 文件句柄
* 输  出  : 无
* 返回值  : OSA_SOK:   成功
*           OSA_EFAIL: 失败
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
* 函数名  : OSA_fgets
* 描  述  : 该函数读取当前文件中的一行以换行或者EOF为结尾的数据
*           该函数仅支持Linux用户态
*
* 输  入  : - hFile:   文件句柄
*           - size: 字符串最大长度
* 输  出  : - s: 读取到的字符串
* 返回值  : OSA_EFAIL: 失败
*           OSA_SOK:   成功
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
