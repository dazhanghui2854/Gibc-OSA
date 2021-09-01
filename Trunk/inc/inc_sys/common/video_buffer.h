/*******************************************************************************
* video_buffer.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang  <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2011-10-26 Create
*
* Desc: 视频信息、视频数据及Buffer的定义和描述，用于多处理器、多模块间共享。
*       这里有两个Buffer的概念，一个是整个Buffer，包含所有关于该帧视频的信息，
*       在这里称之为视频帧Buffer；另一个是保存一帧视频数据的Buffer，在这里称
*       之为视频数据Buffer。前者包含后者，是一块地址连续的内存。视频帧Buffer
*       的描述信息在其前部，视频数据Buffer在其后部。详细描述请查看各数据结构
*       的定义和注释。
*
* Modification: 
* 1. Date    : 2012-07-17
*    Revision: V1.01
*    Author  : Yang Zhiqiang
*    Contents: 完善了定义，添加了对编码数据的支持，增加了链表管理信息，增加了
               视频数据流向的节点统计和时间统计信息，增加了各节点内存映射管理。
*
* 2. Date    : 
*    Revision: 
*    Author  : 
*    Contents: 
*******************************************************************************/


#ifndef _VIDEO_BUFFER_H_
#define _VIDEO_BUFFER_H_


/* ========================================================================== */
/*                             头文件区                                       */
/* ========================================================================== */

#include <osa.h>
#include <sys_info.h>


#ifdef __cplusplus
extern "C" {
#endif 


// TODO: Buffer可能被多个单元引用，必须有一个临时队列管理，通refCnt判断，只有refCnt为0的Buffer才可加入到空队列中。

/* ========================================================================== */
/*                             接口约定说明区                                 */
/* ========================================================================== */

/* 
* 以下介绍几种典型的图像数据格式在内存中的存储方式，其他格式依次类推。在下列描
* 述中，"Frame Top Pad"表示的是在视频数据起始部分预留的空间，"Frame Bottom Pad"
* 表示的是在视频数据末尾部分预留的空间。原因是某些算法需要这些空间，大小根据算法
* 的不同而不同，可通过VIDEO_BufferInfo中的nTopPadNum、nBotPadNum和linePitch成员
* 计算得到。在对视频帧Buffer的描述中，"Frame Top Pad"和"Frame Bottom Pad"看作是
* 与视频数据Buffer一体的。
*/

/* 
* 1. 逐行扫描的YUV422/RAW数据格式（或隔行扫描的，但是偶奇场交错保存)
*    视频数据Buffer中数据的排列顺序如下。只有一个平面，一个指针指向它。
*/
/*********************************
--------------------------
      Frame Top Pad
--------------------------
      Video Frame
--------------------------
      Frame Bottom Pad
--------------------------
*********************************/

/* 
* 2. 隔行扫描且偶奇场分开存储的YUV422数据格式，视频数据Buffer中数据的排列
*    顺序如下图。上下(偶奇)两个平面之间内存地址空间不能保证是连续的，可能
*    会存在内存空洞（某些算法需要）。每个平面都有指针指向它，以便快速索引。
*/
/*********************************
--------------------------
      Frame Top Pad
--------------------------
      TOP Field
--------------------------
      BOT Field
--------------------------
      Frame Bottom Pad
--------------------------
*********************************/

/* 
* 3. 逐行扫描的YUV420SP_VU/YUV422SP_VU数据格式，视频数据Buffer中数据的排列
*    顺序如下图。Y和UV两个平面之间内存地址空间不能保证是连续的，可能会存在
*    内存空洞（某些算法需要）。每个平面都有指针指向它，以便快速索引。
*/
/*********************************
--------------------------
      Frame Top Pad
--------------------------
      Y Plane
--------------------------
      VU Plane
--------------------------
      Frame Bottom Pad
--------------------------
*********************************/

/* 
* 4. 隔行扫描的YUV420SP_VU/YUV422SP_VU数据格式，视频数据Buffer中数据的排列
*    顺序如下图。Y和UV两个平面之间内存地址空间不能保证是连续的，可能会存在
*    内存空洞（某些算法需要）。每个平面都有指针指向它，以便快速索引。
*/
/*********************************
--------------------------
      Frame Top Pad
--------------------------
              Y Plane
 TOP Field ---------------
              VU Plane
--------------------------
              Y Plane
 BOT Field ---------------
              VU Plane
--------------------------
      Frame Bottom Pad
--------------------------
*********************************/

/* 
* 5. 逐行扫描的YUV420P/YUV422P数据格式，视频数据Buffer中数据排列顺序如下
*    图。Y、U和V三个平面内存地址空间不能保证是连续的，可能会存在内存空洞
*    （某些算法需要）。每个平面都有指针指向它，以便快速索引。
*/
/*********************************
--------------------------
      Frame Top Pad
--------------------------
      Y Plane
--------------------------
      U Plane
--------------------------
      V Plane
--------------------------
      Frame Bottom Pad
--------------------------
*********************************/

/* 
* 6. 隔行扫描的YUV420P/YUV422P数据格式，视频数据Buffer中数据排列顺序如下
*    图。Y、U和V三个平面内存地址空间不能保证是连续的，可能会存在内存空洞
*    （某些算法需要）。每个平面都有指针指向它，以便快速索引。
*/
/*********************************
--------------------------
      Frame Top Pad
--------------------------
              Y Plane
           ---------------
 TOP Field    U Plane
           ---------------
              V Plane
--------------------------
              Y Plane
           ---------------
 BOT Field    U Plane
           ---------------
              V Plane
--------------------------
      Frame Bottom Pad
--------------------------
*********************************/


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */

/* 视频帧Buffer的魔数 */
#define VIDEO_BUFFER_MAGIC_NUM    (0x12AC34FD)

/* 视频帧Buffer的版本号，可用于检测兼容性。*/
#define VIDEO_BUFFER_VERSION      (OSA_versionSet(1, 0, 0))


/* 视频数据Buffer的场地址平面的数量，可支持上下（偶奇）场两个平面。*/
#define VIDEO_MAX_FIELDS          (2U)

/* 视频数据Buffer帧/场的颜色地址平面的数量，可支持Y/CB/CR三个平面。*/
#define VIDEO_MAX_PLANES          (3U)


/* 表明此Buffer视频数据的保存格式 */
typedef enum
{
    /* YUV 422交错保存格式 - UYVY。*/
    VIDEO_DATFMT_UYVY     =  0x0000,
    /* YUV 422交错保存格式 - YUYV。*/
    VIDEO_DATFMT_YUYV,
    /* YUV 422交错保存格式 - YVYU。*/
    VIDEO_DATFMT_YVYU,
    /* YUV 422交错保存格式 - VYUY。*/
    VIDEO_DATFMT_VYUY,
    /* YUV 422半平面保存格式 - Y在一个平面，UV在另一平面（交错保存）。*/
    VIDEO_DATFMT_YUV422SP_UV,
    /* YUV 422半平面保存格式 - Y是一个平面，VU是另一平面（交错保存）。*/
    VIDEO_DATFMT_YUV422SP_VU,
    /* YUV 422平面保存格式 - Y是一个平面, V是一个平面，U也是一个平面。*/
    VIDEO_DATFMT_YUV422P,
    /* YUV 420半平面保存格式 - Y在一个平面，UV在另一平面（交错保存）。*/
    VIDEO_DATFMT_YUV420SP_UV,
    /* YUV 420半平面保存格式 - Y是一个平面，VU是另一平面（交错保存）。*/
    VIDEO_DATFMT_YUV420SP_VU,
    /* YUV 420平面保存格式 - Y是一个平面, U是一个平面，V也是一个平面。*/
    VIDEO_DATFMT_YUV420P,
    /* RGB565 16-bit保存格式，单平面，其中5bits R, 6bits G, 5bits B。*/
    VIDEO_DATFMT_RGB16_565  = 0x1000,
    /* BITMAP 8bpp保存格式，单平面。*/ 
    VIDEO_DATFMT_BITMAP8    = 0x2000,
    /* RAW Bayer保存格式，单平面，通过VIDEO_BitsPerPixel区分保存方式。*/
    VIDEO_DATFMT_BAYER_RAW  = 0x3000, 
    /* Bit Strema保存格式，表明是编码后的数据。*/
    VIDEO_DATFMT_BIT_STREAM = 0x4000,    
    /* 无效的数据格式，可用于初始化数据格式变量. */
    VIDEO_DATFMT_INVALID    = 0xF000,
    /* 最大数，可用于计数和校验*/
    VIDEO_DATFMT_MAX
} VIDEO_DataFormat;


/* 每个像素占用的位数 */ 
typedef enum
{
    /* 1Bits每个像素*/
    VIDEO_BPP_BITS1 = 0U,
    /* 2Bits每个像素*/
    VIDEO_BPP_BITS2,
    /* 4Bits每个像素*/
    VIDEO_BPP_BITS4,
    /* 8Bits每个像素*/
    VIDEO_BPP_BITS8,
    /* 12Bits每个像素，用于YUV420数据格式。*/
    VIDEO_BPP_BITS12,
    /* 16Bits每个像素 */
    VIDEO_BPP_BITS16,
    /* 24Bits每个像素 */
    VIDEO_BPP_BITS24,
    /* 32Bits每个像素 */
    VIDEO_BPP_BITS32,
    /* 最大数，可用于计数和校验*/
    VIDEO_BPP_MAX
} VIDEO_BitsPerPixel;


/* 视频数据扫描格式 */ 
typedef enum 
{
    /* 逐行扫描格式 */
	VIDEO_SCANFMT_PROGRESSIVE = 0U,
	/* 隔行扫描格式 */
	VIDEO_SCANFMT_INTERLACED,
	/* 最大数，可用于计数和校验*/
	VIDEO_SCANFMT_MAX
} VIDEO_ScanFormat;


/* 隔行扫描图像在内存中的保存方式 */
typedef enum 
{
    /* 交错保存 */
	VIDEO_FLD_BUF_INTERLEAVED = 0U,
	/* 两场分开保存 */
	VIDEO_FLD_BUF_SEPARATED,
    /* 最大数，可用于计数和校验*/
    VIDEO_FLD_BUF_MAX
} VIDEO_FidBufType;


/* 视频帧/场标示 */
typedef enum
{
    /* 第一场（偶） */
    VIDEO_FID_TOP = 0U,
    /* 第二场（奇） */
    VIDEO_FID_BOTTOM,
    /* 逐行扫描时为一帧，隔行扫描时包含两场。 */
    VIDEO_FID_FRAME,
    /* 最大数，可用于计数和校验 */
    VIDEO_FID_MAX
} VIDEO_FidType;


/* 指示该Buffer视频数据将要进行的编码格式或本身的保存格式，前者可组合使用。*/
typedef enum
{
    /* MJPEG编码 */
    VIDEO_ENCFMT_MJPEG = 0x0001,
    /* MPEG4编码 */
    VIDEO_ENCFMT_MPEG4 = 0x0002,    
    /* H264编码 */
    VIDEO_ENCFMT_H264  = 0x0004,
} VIDEO_EncFormat;


/* 编码后数据的帧类型 */
typedef enum
{
	VIDEO_ENC_FRAME_I     = 0U,
	VIDEO_ENC_FRAME_P,
	VIDEO_ENC_FRAME_B,
	VIDEO_ENC_FRAME_JPEG,
	VIDEO_ENC_FRAME_MAX
} VIDEO_EncFrameType;


/* 视频翻转、镜像、90度旋转、270度旋转标志，可组合使用。*/
typedef enum
{
    VIDEO_ROTATE_NONE   = 0x0000,
    VIDEO_ROTATE_VFLIP  = 0x0001,
    VIDEO_ROTATE_HFLIP  = 0x0002,
    VIDEO_ROTATE_90DEG  = 0x0004,
    VIDEO_ROTATE_270DEG = 0x0008
} VIDEO_RotateType;


/* 智能交通应用中识别视频帧和图片帧 */
typedef enum
{
    /* ITC视频帧 */
    VIDEO_ITC_FRAME_VIDEO      = 0U,
    /* ITC图片帧 */
    VIDEO_ITC_FRAME_PICTURE,
    /* ITC图片流帧 */
    VIDEO_ITC_FRAME_PIC_STREAM,
    /* 最大数，可用于计数和校验*/
    VIDEO_ITC_FRAME_MAX
} VIDEO_ItcFrameType;

 
/* 
* 视频数据Buffer的地址索引定义。VIDEO_BufferInfo.bufAddr是一个二维指针数组。
* 根据帧/场和数据格式定义的宏可方便快速地引用地址指针。
*/

/* 隔行扫描视频帧的TOP Field地址索引 */
#define VIDEO_FIELD_TOP_ADDR_IDX        (0u)

/* 隔行扫描视频帧的Bottom Field地址索引 */
#define VIDEO_FIELD_BOTTOM_ADDR_IDX     (1u)

/* 逐行扫描视频帧的帧地址索引 */
#define VIDEO_FRAME_ADDR_IDX            (0u)

/* 隔行扫描视频帧的偶场地址索引 */
#define VIDEO_FIELD_EVEN_ADDR_IDX       (VIDEO_FIELD_TOP_ADDR_IDX)

/* 隔行扫描视频帧的奇场地址索引 */
#define VIDEO_FIELD_ODD_ADDR_IDX        (VIDEO_FIELD_BOTTOM_ADDR_IDX)

/* 隔行扫描的视帧据，以交错方式保存，偶奇场的地址索引相同 */
#define VIDEO_FIELD_MODE_ADDR_IDX       (0u)

/* 逐行扫描视频帧的帧地址索引 */
#define VIDEO_FIELD_NONE_ADDR_IDX       (VIDEO_FRAME_ADDR_IDX)

/* 交错的YUV444/YUV422数据格式地址平面索引 */
#define VIDEO_YUV_INT_ADDR_IDX          (0u)

/* 平面的YUV444/YUV422/YUV420数据格式Y地址平面索引 */
#define VIDEO_YUV_PL_Y_ADDR_IDX         (0u)

/* 平面的YUV444/YUV422/YUV420数据格式CB(U)地址平面索引 */
#define VIDEO_YUV_PL_CB_ADDR_IDX        (1u)

/* 平面的YUV444/YUV422/YUV420数据格式CR(V)地址平面索引 */
#define VIDEO_YUV_PL_CR_ADDR_IDX        (2u)

/* 半平面的YUV422/YUV420数据格式Y地址平面索引 */
#define VIDEO_YUV_SP_Y_ADDR_IDX         (0u)

/* 半平面的YUV422/YUV420数据格式CBCR()UV地址平面索引 */
#define VIDEO_YUV_SP_CBCR_ADDR_IDX      (1u)

/* RGB888/RGB565/ARGB32数据格式地址平面索引 */
#define VIDEO_RGB_ADDR_IDX              (0u)

/* RAW数据格式地址平面索引 */
#define VIDEO_RAW_ADDR_IDX              (0u)

/* 偶场 */
#define VIDEO_FID_EVEN                  (VIDEO_FID_TOP)

/* 奇场 */
#define VIDEO_FID_ODD                   (VIDEO_FID_BOTTOM)

/* 无偶奇场 */
#define VIDEO_FID_NONE                  (VIDEO_FID_FRAME)



/* 视频帧大小的矩形表示 */
typedef struct
{
    /* 视频数据的左偏移 */
    Uint32  nLeft;		
    	 
    /* 视频数据的上偏移 */
    Uint32  nTop;			
    	
    /* 视频数据的宽度 */
    Uint32  nWidth;	
    
    /* 视频数据的高度 */			
    Uint32  nHeight;
} VIDEO_Rect;


/* 视频帧剪裁大小描述 */
typedef VIDEO_Rect VIDEO_Crop;


/* 链表管理。*/
typedef struct                 
{ 
    OSA_ListHead listHead;  /* 链表头。*/
    
    Uint32 reserved[4];     /* 扩展用。*/
} VIDEO_ListType;



/* 节点地址映射管理。*/
typedef struct                 
{    
    /* 映射后的内核地址，如果节点是Linux，则是内核地址空间，RTOS也映射到此。*/
    Ptr bufAddrKernel[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 

    /* 映射后的视频处理程序进程地址，RTOS则无效。*/
    Ptr bufAddrVideo[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 

    /* 映射后的应用程序进程地址，RTOS则无效。*/
    Ptr bufAddrApp[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 
        
    Uint32 reserved[6];     /* 扩展用。*/
} VIDEO_NodeMemMap;

/* 
* 特定Buffer的节点流向记录，一个输入节点，N个输出节点。
* 可制定和追踪Buffer的流向。
*/
typedef struct                 
{ 
   Uint32 nOutNodeNum;          /* 输出节点的有效个数 */
   Uint32 inNode;               /* 一个输入节点 */
   Uint32 outNode[SYS_PROC_NR];  /* SYS_MCU_NR个输出节点 */
   
   Uint32 reserved[4];          /* 扩展用。*/
} VIDEO_NodeDircRecord;


/* 
* 特定Buffer的时间信息记录，一个输入节点，N个输出节点。
* 可制定和追踪Buffer的流向。
*/
typedef struct
{
    Uint32 curRunTime;          /* 该节点当前运行时间 */
    Uint32 minRunTime;          /* 该节点最小运行时间 */
    Uint32 maxRunTime;          /* 该节点最大运行时间 */
    Uint32 totalRunTime;        /* 该节点总共运行时间 */
    Uint32 totalRunCnt;         /* 该节点运行的次数 */
    
    Uint32 reserved[4];          /* 扩展用。*/
} VIDEO_NodeTimeRecord;


/*
* 各节点的统计信息。
*/
typedef struct                 
{ 
   VIDEO_NodeDircRecord direc;  /* 视频数据流向的节点信息。*/
    
   VIDEO_NodeTimeRecord time;   /* 当前Buffer在该节点的运行时间信息记录 */
   
   Uint32 reserved[8];          /* 扩展用。*/
} VIDEO_NodeStat;


/* 
* 各节点信息的标准头，各节点制定的结构必须包含它并将其放置在首位，
* 如capInfo，EncInfo等节点。
*/
typedef struct                 
{   
    /* 整个节点所有存储信息的大小 */
    Uint32 nNodeInfoSize;
 
    /* 保存将要发送的目的节点 */
    VIDEO_NodeDircRecord nextNode;
    
    /* 链表管理信息。如果各个节点使用链表管理这些Buffer，那么使用该list。*/
    VIDEO_ListType   list;
 
    /* 此节点此Buffer的内存映射 */
    VIDEO_NodeMemMap memMap;
    
    /* 此节点的统计信息。*/
    VIDEO_NodeStat   stat;
         
    /* 将来扩展用 */                
    Uint32 reserved[16];
} VIDEO_NodeInfoHead;


/* 
* 视频帧Buffer的描述结构体定义。
* 下列各统一由采集模块填充，运行时是固定的，通过修改采集模块的程
* 序来修改其大小。各成员的Offset都是从整个Buffer的起始地址开始计算，即包含了
* VIDEO_FrameHeadd结构体的大小。
* 1. CapInfo、EncInfo、AlgInfo、AppInfo和IspInfo这些信息的结构体定义，由相
*    关模块间协商和定义。
* 2. 视频帧Buffer中的内容排列顺序如下，视频数据Buffer至少1个，可能有多个，
*    具体数据由VIDEO_FrameInfo.nBufInfoNumPlus决定。
-------------------------- 
      VIDEO_FrameHead
--------------------------
      CapInfo
--------------------------
      EncInfo
--------------------------
      AlgInfo
--------------------------
      AppInfo
--------------------------
      IspInfo
--------------------------
      Video Frame 0 Buffer
--------------------------
      Video Frame 1 Buffer
--------------------------
      ......
--------------------------
*/
typedef struct 
{
    /* 
    *  魔数，检查这块Buffer是否被破坏(如DMA等)，其值定义请查看
    *  VIDEO_BUFFER_MAGIC_NUM。
    */
    Uint32 nMagicNum;
    
    /* 
    * 版本号。高8位代表Major Version，中8位代表Minor Version，
    * 低16代表Revision。其值由采集模块根据VIDEO_BUFFER_VERSION
    * 写入，各模块可使用它与本文件中的VIDEO_BUFFER_VERSION比较，
    * 以检测版本的兼容性及其他事项。
    */				
    Uint32 nVersion;	
    
    /* 整块视频帧Buffer的大小 */
    Uint32 nTotalBufSize;
  
    /* 此Buffer的源节点。*/ 
    Uint32 srcNode;       
     
    /* 
    * 视频数据Buffer的偏移，可快速得到视频数据Buffer的开始地址。该偏移未包
    * 含"Frame Top Pad"的大小，即偏移后得到的地址是从"Frame Top Pad"开
    * 始的。由于视频数据Buffer内部存在空闲，所以正常情况应该通过
    * VIDEO_BufferInfo.bufAddr来得到其物理地址。
    */		
    Uint32 nFrameBufOffset;	
    
    /* 采集模块存放的信息偏移。统一由采集模块填充。*/	
    Uint32 nCapInfoOffset;	
    
    /* 采集模块可存放信息的数量。*/
    Uint32 nCapInfoSize;		
     
    /* 编码模块存放的信息偏移，统一由采集模块填充。*/		
    Uint32 nEncInfoOffset;	
    
    /* 编码模块可存放信息的数量。*/
    Uint32 nEncInfoSize;	
       
    /* 智能算法模块存放的信息偏移，统一由采集模块填充。*/
    Uint32 nAlgInfoOffset;
    
    /* 智能算法模块可存放信息的数量。*/		
    Uint32 nAlgInfoSize;

    /* 应用模块的信息偏移，统一由采集模块填充。*/
    Uint32 nAppInfoOffset;
    
    /* 应用模块可存放信息的数量。*/		
    Uint32 nAppInfoSize;
    
    /* 图像处理模块存放的信息偏移，统一由采集模块填充。*/
    Uint32 nIspInfoOffset;
    
    /* 图像处理模块可存放信息的数量。*/		
    Uint32 nIspInfoSize;	
    
    /* 将来扩展用 */		
    Uint32 reserved[8];			
} VIDEO_BufferDesc;		


/* 视频帧和码流相关的信息 */
typedef struct 
{
    /* 码流ID，与编码和web的码流号定义一致。*/
    Uint32 nStreamId;
    
    /* 通道号，区别不同Senosr输出的码流。*/
    Uint32 nChId;
    
    /* 采集程序内部使用 */
    Uint32 nInterId; 
    
    /* 引用计数，采集程序内部使用。*/
    Uint32 nRefCnt;
   
    /* 帧序号。每路码流都有自己的帧序号。*/
    Uint32 nFrameNum;
    
    /* 
    * VIDEO_BufferInfo的数量等于nBufInfoNumPlus + 1，即视频数据Buffer的数量。
    * 通过任一Buffer都可以得到其他Buffer。
    */
    Uint32 nBufInfoNumPlus;
    
    /* 链接同一码流的所有Buffer。*/
    OSA_ListHead nextBuffer;
    
    /* 将来扩展用 */
    Uint32 reserved[8];
} VIDEO_FrameInfo;


/* 视频数据Buffer的信息定义。Buffer图示如下，实际视频大小在箭头标示的区域内。
    
   <----------------- nWidth -----------------> ---------
 /|\          |                               /|\        |
  |      dataCrop.nTop                         |         |  
  |           |                                |         | 
  |-- nLeft -- --- dataCrop.nWidth ---         |         | 
  |           |                       |        |         | 
  |           |                       |        |         | 
  |           |                       |        |         | 
  |           |           dataCrop.nHeight   nHeight     | 
  |           |                       |        |         | 
  |           |                       |        |         | 
  |           |                       |        |         | 
  |            -----------------------         |         | 
  |                                            |         | 
  |                                            |         | 
  |<----------------- linePitch[x] --------------------->|
  |                                            |         |
 \|/                                          \|/        | 
   <------------------------------------------> ---------
*/
typedef struct  
{
    /* 视频数据Buffer的内部索引号 */
    Uint32 nIndex;
    
    /* 视频数据Buffer的大小 */
    Uint32 nBufSize;
  
    /* 视频数据Buffer实际使用大小，一般保存的是编码数据或其统计数据时使用。*/
    Uint32 nBytesUsed;
    
    /* 视频数据Buffer头部多分配的行数，用于某些算法，如JPEG编码等。*/
    Uint32 nTopPadNum;
    
    /* 视频数据Buffer底部多分配的行数，用于某些算法。*/
    Uint32 nBotPadNum;  
    
    /* 视频数据的实际宽度 */
    Uint32 nWidth;	
    
    /* 视频数据的实际高度 */			
    Uint32 nHeight;
    
    /* 视频数据剪裁的大小。编码器使用此大小进行编码。*/
    VIDEO_Crop encCrop;
    
    /* 
    * 视频数据的格式，先判断是BitStream还是Frame，再判断具体的格式。
    * 其定义见VIDEO_DataFormat。
    */	       
    Uint32 dataFormat;
    
    /* 每个像素占用的位数，其定义见VIDEO_BitsPerPixel。*/
    Uint32 nBpp;
 
    /* 
     * 视频帧/场号。其定义见VIDEO_FidType。扫描格式和存储格式定义在
     * scanFormat和fidBufType成员中。
     */
    Uint32 nFieldId;
    
    /* 视频数据采集的扫描方式，其定义见VIDEO_ScanFormat。*/
    Uint32 scanFormat;
    
    /* 
    * 隔行扫描的视频数据保存方式，对逐行扫描的视频数据无效，
    * 其定义见VIDEO_FidBufType。
    */
    Uint32 fidBufType; 
    
    /* 
    * 表明视频数据做了翻转、镜像、旋转等操作，可以是多项的组合，编码模块和智能算法
    * 读取视频数据时，需要知道此信息。其定义见VIDEO_RotateType。
    */
    Uint32 rotateFlags;
    
    /* 视频数据的目标编码格式或Buffer数据的编码格式，其定义见VIDEO_EncFormat。*/
    Uint32 encFlags;
    
    /* 编码数据的帧类型，YUV数据不用关注。其定义见VIDEO_EncFrameType。*/
    Uint32 encFrmType;

    /* 视频数据Buffer的染色，用于回收和处理，采集程序内部使用。*/
    Uint32 colorMark;
    
    /* 时间戳，是单调递增的时间，以ms为单位。*/			
    Uint32 timeStamp;
    
    /* 操作系统滴答数 */
    Uint32 osTicks;   
     
    /* 
     * 每块平面内的行地址偏移。处理器保存视频时，对视频数据每行的地址偏移都有
     * 对齐要求。如要求32Byte对齐，如果图像的行宽度不是32Byte的对齐，则保存的
     * 图像在地址上并不是连续，而是以32Byte对齐的地址行宽度保存。所以读取视频
     * 数据时以这个值作为行地址偏移。
    */	
    Uint32 linePitch[VIDEO_MAX_PLANES];
    
    /*
    * 视频数据Buffer使用一个二维的指针数组bufAddr管理，第一维代表的是帧/场，
    * 第二维代表的是颜色平面。这些指针有些可能为NULL，要根据数据格式区分
    * 对待(dataFormat)。下面举例介绍几种典型数据格式的情况。
    * 1. 逐行扫描的YUV422/RAW数据格式。
    *    bufAddr[0][0] --> YUV422/RAW
    *    bufAddr[1][x] --> 没有使用
    * 2. 隔行扫描且偶奇场分开存储的YUV422数据格式
    *    bufAddr[0][0] --> YUV422 Top Field
    *    bufAddr[1][0] --> YUV422 Bot Field  
    * 3. 逐行扫描的YUV420SP_VU/YUV422SP_VU的数据格式
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> VU
    *    bufAddr[1][x] --> 没有使用  
    * 4. 隔行扫描的YUV420SP_VU/YUV422SP_VU的数据格式
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> VU
    *    bufAddr[1][0] --> Y
    *    bufAddr[1][1] --> VU  
    * 5. 逐行扫描的YUV420P/YUV422P的数据格式
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> U
    *    bufAddr[0][2] --> V
    *    bufAddr[1][x] --> 没有使用    
    * 6. 隔行扫描的YUV420P/YUV422P的数据格式
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> U
    *    bufAddr[0][2] --> V    
    *    bufAddr[1][0] --> Y
    *    bufAddr[1][1] --> U
    *    bufAddr[1][2] --> V  
    *
    * 可使用VIDEO_YUV_INT_ADDR_IDX, VIDEO_RGB_ADDR_IDX, VIDEO_FID_TOP
    * 等宏来获取这些指针.
    * 由于bufAddr中的指针指向的地址是视频帧Buffer的一部分，所以只需对
    * 视频帧Buffer地址进行处理器间的地址映射，然后根据之前的地址偏移
    * 便可得到各指针值。假设VIDEO_Frame未映射前的地址是pOld，映射后
    * 的地址是pNew，则bufAddr[0][0] = pNew + (bufAddr[0][0] - pOld)。
    * 这些地址在计算的时候，需要强制类型为Uint32或Uint8 *。
    */
    Ptr    bufAddr[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 
    
    /* 将来扩展用 */
    Uint32 reserved[8];

    /* 
    * 占位符，不占用内存。方便程序得到下一个相邻的VIDEO_BufferInfo地址。
    * 其剩余数量保存在VIDEO_FrameInfo.nBufInfoNumPlus中。必须是同一码流
    * 的Buffer，或者是其他非码流的辅助数据，如直方图等。
    */
    Uint32 nextBufInfo[0];
} VIDEO_BufferInfo; 


/* 产品的业务应用标志信息。*/
typedef struct                 
{    
    /* 智能交通中用于识别帧的类型。其定义见VIDEO_ItcFrameType。*/
    Uint8 itcFrameId;
         
    /* 将来扩展用 */                
    Uint8 reserved[127];
} VIDEO_AppInfo;


/*
* 视频帧Buffer的帧头，描述了所有视频帧Buffer的信息。
* 此结构体里所有的信息，除了listInfo和nodeInfo外，只有采集程序可写，
* 其他处理器或模块的程序只可读。结构体的描述如下图。
--------------------------
     VIDEO_BufferDesc
--------------------------
     VIDEO_AppInfo
--------------------------
     VIDEO_FrameInfo
--------------------------
     VIDEO_BufferInfo 0
--------------------------
     VIDEO_BufferInfo 1
--------------------------
         ......
--------------------------
*/
typedef struct
{
    /* 整个视频帧Buffer描述 */
    VIDEO_BufferDesc  bufDesc; 
    
    /* 产品业务应用标志信息 */
    VIDEO_AppInfo     appInfo;    
    
    /* 视频数据和码流信息 */
    VIDEO_FrameInfo   frmInfo;
    
    /* 将来扩展用 */
    Uint32            reserved[64];
    
    /* 视频数据Buffer信息 */
    VIDEO_BufferInfo  bufInfo;

} VIDEO_FrameHead;


/*  
* 多处理器或模块间视频帧交互的数据类型，是一个空类型的指针。各队列间以此指针作为
* 载体传递信息。处理器接收到该信息后，首先需要对该指针进行处理器间内存地址空间
* 映射，然后从其中获取到视频数据相关的信息。假如收到的该类型指针变量是pVideoFrame，
* 则VIDEO_FrameHead *pFrameHead = (VIDEO_FrameHead *)pVideoFrame。
*/
typedef void * VIDEO_Frame;


#ifdef __cplusplus
}
#endif


#endif  /*  _VIDEO_BUFFER_H_  */

