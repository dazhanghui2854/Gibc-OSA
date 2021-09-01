/*******************************************************************************
* video_buffer.h
*
* Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Yang Zhiqiang  <yang_zhiqiang@dahuatech.com>
* Version: V1.0.0  2011-10-26 Create
*
* Desc: ��Ƶ��Ϣ����Ƶ���ݼ�Buffer�Ķ�������������ڶദ��������ģ��乲��
*       ����������Buffer�ĸ��һ��������Buffer���������й��ڸ�֡��Ƶ����Ϣ��
*       �������֮Ϊ��Ƶ֡Buffer����һ���Ǳ���һ֡��Ƶ���ݵ�Buffer���������
*       ֮Ϊ��Ƶ����Buffer��ǰ�߰������ߣ���һ���ַ�������ڴ档��Ƶ֡Buffer
*       ��������Ϣ����ǰ������Ƶ����Buffer����󲿡���ϸ������鿴�����ݽṹ
*       �Ķ����ע�͡�
*
* Modification: 
* 1. Date    : 2012-07-17
*    Revision: V1.01
*    Author  : Yang Zhiqiang
*    Contents: �����˶��壬����˶Ա������ݵ�֧�֣����������������Ϣ��������
               ��Ƶ��������Ľڵ�ͳ�ƺ�ʱ��ͳ����Ϣ�������˸��ڵ��ڴ�ӳ�����
*
* 2. Date    : 
*    Revision: 
*    Author  : 
*    Contents: 
*******************************************************************************/


#ifndef _VIDEO_BUFFER_H_
#define _VIDEO_BUFFER_H_


/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#include <osa.h>
#include <sys_info.h>


#ifdef __cplusplus
extern "C" {
#endif 


// TODO: Buffer���ܱ������Ԫ���ã�������һ����ʱ���й���ͨrefCnt�жϣ�ֻ��refCntΪ0��Buffer�ſɼ��뵽�ն����С�

/* ========================================================================== */
/*                             �ӿ�Լ��˵����                                 */
/* ========================================================================== */

/* 
* ���½��ܼ��ֵ��͵�ͼ�����ݸ�ʽ���ڴ��еĴ洢��ʽ��������ʽ�������ơ���������
* ���У�"Frame Top Pad"��ʾ��������Ƶ������ʼ����Ԥ���Ŀռ䣬"Frame Bottom Pad"
* ��ʾ��������Ƶ����ĩβ����Ԥ���Ŀռ䡣ԭ����ĳЩ�㷨��Ҫ��Щ�ռ䣬��С�����㷨
* �Ĳ�ͬ����ͬ����ͨ��VIDEO_BufferInfo�е�nTopPadNum��nBotPadNum��linePitch��Ա
* ����õ����ڶ���Ƶ֡Buffer�������У�"Frame Top Pad"��"Frame Bottom Pad"������
* ����Ƶ����Bufferһ��ġ�
*/

/* 
* 1. ����ɨ���YUV422/RAW���ݸ�ʽ�������ɨ��ģ�����ż�泡������)
*    ��Ƶ����Buffer�����ݵ�����˳�����¡�ֻ��һ��ƽ�棬һ��ָ��ָ������
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
* 2. ����ɨ����ż�泡�ֿ��洢��YUV422���ݸ�ʽ����Ƶ����Buffer�����ݵ�����
*    ˳������ͼ������(ż��)����ƽ��֮���ڴ��ַ�ռ䲻�ܱ�֤�������ģ�����
*    ������ڴ�ն���ĳЩ�㷨��Ҫ����ÿ��ƽ�涼��ָ��ָ�������Ա����������
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
* 3. ����ɨ���YUV420SP_VU/YUV422SP_VU���ݸ�ʽ����Ƶ����Buffer�����ݵ�����
*    ˳������ͼ��Y��UV����ƽ��֮���ڴ��ַ�ռ䲻�ܱ�֤�������ģ����ܻ����
*    �ڴ�ն���ĳЩ�㷨��Ҫ����ÿ��ƽ�涼��ָ��ָ�������Ա����������
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
* 4. ����ɨ���YUV420SP_VU/YUV422SP_VU���ݸ�ʽ����Ƶ����Buffer�����ݵ�����
*    ˳������ͼ��Y��UV����ƽ��֮���ڴ��ַ�ռ䲻�ܱ�֤�������ģ����ܻ����
*    �ڴ�ն���ĳЩ�㷨��Ҫ����ÿ��ƽ�涼��ָ��ָ�������Ա����������
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
* 5. ����ɨ���YUV420P/YUV422P���ݸ�ʽ����Ƶ����Buffer����������˳������
*    ͼ��Y��U��V����ƽ���ڴ��ַ�ռ䲻�ܱ�֤�������ģ����ܻ�����ڴ�ն�
*    ��ĳЩ�㷨��Ҫ����ÿ��ƽ�涼��ָ��ָ�������Ա����������
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
* 6. ����ɨ���YUV420P/YUV422P���ݸ�ʽ����Ƶ����Buffer����������˳������
*    ͼ��Y��U��V����ƽ���ڴ��ַ�ռ䲻�ܱ�֤�������ģ����ܻ�����ڴ�ն�
*    ��ĳЩ�㷨��Ҫ����ÿ��ƽ�涼��ָ��ָ�������Ա����������
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
/*                           ������Ͷ�����                                   */
/* ========================================================================== */

/* ��Ƶ֡Buffer��ħ�� */
#define VIDEO_BUFFER_MAGIC_NUM    (0x12AC34FD)

/* ��Ƶ֡Buffer�İ汾�ţ������ڼ������ԡ�*/
#define VIDEO_BUFFER_VERSION      (OSA_versionSet(1, 0, 0))


/* ��Ƶ����Buffer�ĳ���ַƽ�����������֧�����£�ż�棩������ƽ�档*/
#define VIDEO_MAX_FIELDS          (2U)

/* ��Ƶ����Buffer֡/������ɫ��ַƽ�����������֧��Y/CB/CR����ƽ�档*/
#define VIDEO_MAX_PLANES          (3U)


/* ������Buffer��Ƶ���ݵı����ʽ */
typedef enum
{
    /* YUV 422�������ʽ - UYVY��*/
    VIDEO_DATFMT_UYVY     =  0x0000,
    /* YUV 422�������ʽ - YUYV��*/
    VIDEO_DATFMT_YUYV,
    /* YUV 422�������ʽ - YVYU��*/
    VIDEO_DATFMT_YVYU,
    /* YUV 422�������ʽ - VYUY��*/
    VIDEO_DATFMT_VYUY,
    /* YUV 422��ƽ�汣���ʽ - Y��һ��ƽ�棬UV����һƽ�棨�����棩��*/
    VIDEO_DATFMT_YUV422SP_UV,
    /* YUV 422��ƽ�汣���ʽ - Y��һ��ƽ�棬VU����һƽ�棨�����棩��*/
    VIDEO_DATFMT_YUV422SP_VU,
    /* YUV 422ƽ�汣���ʽ - Y��һ��ƽ��, V��һ��ƽ�棬UҲ��һ��ƽ�档*/
    VIDEO_DATFMT_YUV422P,
    /* YUV 420��ƽ�汣���ʽ - Y��һ��ƽ�棬UV����һƽ�棨�����棩��*/
    VIDEO_DATFMT_YUV420SP_UV,
    /* YUV 420��ƽ�汣���ʽ - Y��һ��ƽ�棬VU����һƽ�棨�����棩��*/
    VIDEO_DATFMT_YUV420SP_VU,
    /* YUV 420ƽ�汣���ʽ - Y��һ��ƽ��, U��һ��ƽ�棬VҲ��һ��ƽ�档*/
    VIDEO_DATFMT_YUV420P,
    /* RGB565 16-bit�����ʽ����ƽ�棬����5bits R, 6bits G, 5bits B��*/
    VIDEO_DATFMT_RGB16_565  = 0x1000,
    /* BITMAP 8bpp�����ʽ����ƽ�档*/ 
    VIDEO_DATFMT_BITMAP8    = 0x2000,
    /* RAW Bayer�����ʽ����ƽ�棬ͨ��VIDEO_BitsPerPixel���ֱ��淽ʽ��*/
    VIDEO_DATFMT_BAYER_RAW  = 0x3000, 
    /* Bit Strema�����ʽ�������Ǳ��������ݡ�*/
    VIDEO_DATFMT_BIT_STREAM = 0x4000,    
    /* ��Ч�����ݸ�ʽ�������ڳ�ʼ�����ݸ�ʽ����. */
    VIDEO_DATFMT_INVALID    = 0xF000,
    /* ������������ڼ�����У��*/
    VIDEO_DATFMT_MAX
} VIDEO_DataFormat;


/* ÿ������ռ�õ�λ�� */ 
typedef enum
{
    /* 1Bitsÿ������*/
    VIDEO_BPP_BITS1 = 0U,
    /* 2Bitsÿ������*/
    VIDEO_BPP_BITS2,
    /* 4Bitsÿ������*/
    VIDEO_BPP_BITS4,
    /* 8Bitsÿ������*/
    VIDEO_BPP_BITS8,
    /* 12Bitsÿ�����أ�����YUV420���ݸ�ʽ��*/
    VIDEO_BPP_BITS12,
    /* 16Bitsÿ������ */
    VIDEO_BPP_BITS16,
    /* 24Bitsÿ������ */
    VIDEO_BPP_BITS24,
    /* 32Bitsÿ������ */
    VIDEO_BPP_BITS32,
    /* ������������ڼ�����У��*/
    VIDEO_BPP_MAX
} VIDEO_BitsPerPixel;


/* ��Ƶ����ɨ���ʽ */ 
typedef enum 
{
    /* ����ɨ���ʽ */
	VIDEO_SCANFMT_PROGRESSIVE = 0U,
	/* ����ɨ���ʽ */
	VIDEO_SCANFMT_INTERLACED,
	/* ������������ڼ�����У��*/
	VIDEO_SCANFMT_MAX
} VIDEO_ScanFormat;


/* ����ɨ��ͼ�����ڴ��еı��淽ʽ */
typedef enum 
{
    /* ������ */
	VIDEO_FLD_BUF_INTERLEAVED = 0U,
	/* �����ֿ����� */
	VIDEO_FLD_BUF_SEPARATED,
    /* ������������ڼ�����У��*/
    VIDEO_FLD_BUF_MAX
} VIDEO_FidBufType;


/* ��Ƶ֡/����ʾ */
typedef enum
{
    /* ��һ����ż�� */
    VIDEO_FID_TOP = 0U,
    /* �ڶ������棩 */
    VIDEO_FID_BOTTOM,
    /* ����ɨ��ʱΪһ֡������ɨ��ʱ���������� */
    VIDEO_FID_FRAME,
    /* ������������ڼ�����У�� */
    VIDEO_FID_MAX
} VIDEO_FidType;


/* ָʾ��Buffer��Ƶ���ݽ�Ҫ���еı����ʽ����ı����ʽ��ǰ�߿����ʹ�á�*/
typedef enum
{
    /* MJPEG���� */
    VIDEO_ENCFMT_MJPEG = 0x0001,
    /* MPEG4���� */
    VIDEO_ENCFMT_MPEG4 = 0x0002,    
    /* H264���� */
    VIDEO_ENCFMT_H264  = 0x0004,
} VIDEO_EncFormat;


/* ��������ݵ�֡���� */
typedef enum
{
	VIDEO_ENC_FRAME_I     = 0U,
	VIDEO_ENC_FRAME_P,
	VIDEO_ENC_FRAME_B,
	VIDEO_ENC_FRAME_JPEG,
	VIDEO_ENC_FRAME_MAX
} VIDEO_EncFrameType;


/* ��Ƶ��ת������90����ת��270����ת��־�������ʹ�á�*/
typedef enum
{
    VIDEO_ROTATE_NONE   = 0x0000,
    VIDEO_ROTATE_VFLIP  = 0x0001,
    VIDEO_ROTATE_HFLIP  = 0x0002,
    VIDEO_ROTATE_90DEG  = 0x0004,
    VIDEO_ROTATE_270DEG = 0x0008
} VIDEO_RotateType;


/* ���ܽ�ͨӦ����ʶ����Ƶ֡��ͼƬ֡ */
typedef enum
{
    /* ITC��Ƶ֡ */
    VIDEO_ITC_FRAME_VIDEO      = 0U,
    /* ITCͼƬ֡ */
    VIDEO_ITC_FRAME_PICTURE,
    /* ITCͼƬ��֡ */
    VIDEO_ITC_FRAME_PIC_STREAM,
    /* ������������ڼ�����У��*/
    VIDEO_ITC_FRAME_MAX
} VIDEO_ItcFrameType;

 
/* 
* ��Ƶ����Buffer�ĵ�ַ�������塣VIDEO_BufferInfo.bufAddr��һ����άָ�����顣
* ����֡/�������ݸ�ʽ����ĺ�ɷ�����ٵ����õ�ַָ�롣
*/

/* ����ɨ����Ƶ֡��TOP Field��ַ���� */
#define VIDEO_FIELD_TOP_ADDR_IDX        (0u)

/* ����ɨ����Ƶ֡��Bottom Field��ַ���� */
#define VIDEO_FIELD_BOTTOM_ADDR_IDX     (1u)

/* ����ɨ����Ƶ֡��֡��ַ���� */
#define VIDEO_FRAME_ADDR_IDX            (0u)

/* ����ɨ����Ƶ֡��ż����ַ���� */
#define VIDEO_FIELD_EVEN_ADDR_IDX       (VIDEO_FIELD_TOP_ADDR_IDX)

/* ����ɨ����Ƶ֡���泡��ַ���� */
#define VIDEO_FIELD_ODD_ADDR_IDX        (VIDEO_FIELD_BOTTOM_ADDR_IDX)

/* ����ɨ�����֡�ݣ��Խ���ʽ���棬ż�泡�ĵ�ַ������ͬ */
#define VIDEO_FIELD_MODE_ADDR_IDX       (0u)

/* ����ɨ����Ƶ֡��֡��ַ���� */
#define VIDEO_FIELD_NONE_ADDR_IDX       (VIDEO_FRAME_ADDR_IDX)

/* �����YUV444/YUV422���ݸ�ʽ��ַƽ������ */
#define VIDEO_YUV_INT_ADDR_IDX          (0u)

/* ƽ���YUV444/YUV422/YUV420���ݸ�ʽY��ַƽ������ */
#define VIDEO_YUV_PL_Y_ADDR_IDX         (0u)

/* ƽ���YUV444/YUV422/YUV420���ݸ�ʽCB(U)��ַƽ������ */
#define VIDEO_YUV_PL_CB_ADDR_IDX        (1u)

/* ƽ���YUV444/YUV422/YUV420���ݸ�ʽCR(V)��ַƽ������ */
#define VIDEO_YUV_PL_CR_ADDR_IDX        (2u)

/* ��ƽ���YUV422/YUV420���ݸ�ʽY��ַƽ������ */
#define VIDEO_YUV_SP_Y_ADDR_IDX         (0u)

/* ��ƽ���YUV422/YUV420���ݸ�ʽCBCR()UV��ַƽ������ */
#define VIDEO_YUV_SP_CBCR_ADDR_IDX      (1u)

/* RGB888/RGB565/ARGB32���ݸ�ʽ��ַƽ������ */
#define VIDEO_RGB_ADDR_IDX              (0u)

/* RAW���ݸ�ʽ��ַƽ������ */
#define VIDEO_RAW_ADDR_IDX              (0u)

/* ż�� */
#define VIDEO_FID_EVEN                  (VIDEO_FID_TOP)

/* �泡 */
#define VIDEO_FID_ODD                   (VIDEO_FID_BOTTOM)

/* ��ż�泡 */
#define VIDEO_FID_NONE                  (VIDEO_FID_FRAME)



/* ��Ƶ֡��С�ľ��α�ʾ */
typedef struct
{
    /* ��Ƶ���ݵ���ƫ�� */
    Uint32  nLeft;		
    	 
    /* ��Ƶ���ݵ���ƫ�� */
    Uint32  nTop;			
    	
    /* ��Ƶ���ݵĿ�� */
    Uint32  nWidth;	
    
    /* ��Ƶ���ݵĸ߶� */			
    Uint32  nHeight;
} VIDEO_Rect;


/* ��Ƶ֡���ô�С���� */
typedef VIDEO_Rect VIDEO_Crop;


/* �������*/
typedef struct                 
{ 
    OSA_ListHead listHead;  /* ����ͷ��*/
    
    Uint32 reserved[4];     /* ��չ�á�*/
} VIDEO_ListType;



/* �ڵ��ַӳ�����*/
typedef struct                 
{    
    /* ӳ�����ں˵�ַ������ڵ���Linux�������ں˵�ַ�ռ䣬RTOSҲӳ�䵽�ˡ�*/
    Ptr bufAddrKernel[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 

    /* ӳ������Ƶ���������̵�ַ��RTOS����Ч��*/
    Ptr bufAddrVideo[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 

    /* ӳ����Ӧ�ó�����̵�ַ��RTOS����Ч��*/
    Ptr bufAddrApp[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 
        
    Uint32 reserved[6];     /* ��չ�á�*/
} VIDEO_NodeMemMap;

/* 
* �ض�Buffer�Ľڵ������¼��һ������ڵ㣬N������ڵ㡣
* ���ƶ���׷��Buffer������
*/
typedef struct                 
{ 
   Uint32 nOutNodeNum;          /* ����ڵ����Ч���� */
   Uint32 inNode;               /* һ������ڵ� */
   Uint32 outNode[SYS_PROC_NR];  /* SYS_MCU_NR������ڵ� */
   
   Uint32 reserved[4];          /* ��չ�á�*/
} VIDEO_NodeDircRecord;


/* 
* �ض�Buffer��ʱ����Ϣ��¼��һ������ڵ㣬N������ڵ㡣
* ���ƶ���׷��Buffer������
*/
typedef struct
{
    Uint32 curRunTime;          /* �ýڵ㵱ǰ����ʱ�� */
    Uint32 minRunTime;          /* �ýڵ���С����ʱ�� */
    Uint32 maxRunTime;          /* �ýڵ��������ʱ�� */
    Uint32 totalRunTime;        /* �ýڵ��ܹ�����ʱ�� */
    Uint32 totalRunCnt;         /* �ýڵ����еĴ��� */
    
    Uint32 reserved[4];          /* ��չ�á�*/
} VIDEO_NodeTimeRecord;


/*
* ���ڵ��ͳ����Ϣ��
*/
typedef struct                 
{ 
   VIDEO_NodeDircRecord direc;  /* ��Ƶ��������Ľڵ���Ϣ��*/
    
   VIDEO_NodeTimeRecord time;   /* ��ǰBuffer�ڸýڵ������ʱ����Ϣ��¼ */
   
   Uint32 reserved[8];          /* ��չ�á�*/
} VIDEO_NodeStat;


/* 
* ���ڵ���Ϣ�ı�׼ͷ�����ڵ��ƶ��Ľṹ����������������������λ��
* ��capInfo��EncInfo�Ƚڵ㡣
*/
typedef struct                 
{   
    /* �����ڵ����д洢��Ϣ�Ĵ�С */
    Uint32 nNodeInfoSize;
 
    /* ���潫Ҫ���͵�Ŀ�Ľڵ� */
    VIDEO_NodeDircRecord nextNode;
    
    /* ���������Ϣ����������ڵ�ʹ�����������ЩBuffer����ôʹ�ø�list��*/
    VIDEO_ListType   list;
 
    /* �˽ڵ��Buffer���ڴ�ӳ�� */
    VIDEO_NodeMemMap memMap;
    
    /* �˽ڵ��ͳ����Ϣ��*/
    VIDEO_NodeStat   stat;
         
    /* ������չ�� */                
    Uint32 reserved[16];
} VIDEO_NodeInfoHead;


/* 
* ��Ƶ֡Buffer�������ṹ�嶨�塣
* ���и�ͳһ�ɲɼ�ģ����䣬����ʱ�ǹ̶��ģ�ͨ���޸Ĳɼ�ģ��ĳ�
* �����޸����С������Ա��Offset���Ǵ�����Buffer����ʼ��ַ��ʼ���㣬��������
* VIDEO_FrameHeadd�ṹ��Ĵ�С��
* 1. CapInfo��EncInfo��AlgInfo��AppInfo��IspInfo��Щ��Ϣ�Ľṹ�嶨�壬����
*    ��ģ���Э�̺Ͷ��塣
* 2. ��Ƶ֡Buffer�е���������˳�����£���Ƶ����Buffer����1���������ж����
*    ����������VIDEO_FrameInfo.nBufInfoNumPlus������
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
    *  ħ����������Buffer�Ƿ��ƻ�(��DMA��)����ֵ������鿴
    *  VIDEO_BUFFER_MAGIC_NUM��
    */
    Uint32 nMagicNum;
    
    /* 
    * �汾�š���8λ����Major Version����8λ����Minor Version��
    * ��16����Revision����ֵ�ɲɼ�ģ�����VIDEO_BUFFER_VERSION
    * д�룬��ģ���ʹ�����뱾�ļ��е�VIDEO_BUFFER_VERSION�Ƚϣ�
    * �Լ��汾�ļ����Լ��������
    */				
    Uint32 nVersion;	
    
    /* ������Ƶ֡Buffer�Ĵ�С */
    Uint32 nTotalBufSize;
  
    /* ��Buffer��Դ�ڵ㡣*/ 
    Uint32 srcNode;       
     
    /* 
    * ��Ƶ����Buffer��ƫ�ƣ��ɿ��ٵõ���Ƶ����Buffer�Ŀ�ʼ��ַ����ƫ��δ��
    * ��"Frame Top Pad"�Ĵ�С����ƫ�ƺ�õ��ĵ�ַ�Ǵ�"Frame Top Pad"��
    * ʼ�ġ�������Ƶ����Buffer�ڲ����ڿ��У������������Ӧ��ͨ��
    * VIDEO_BufferInfo.bufAddr���õ��������ַ��
    */		
    Uint32 nFrameBufOffset;	
    
    /* �ɼ�ģ���ŵ���Ϣƫ�ơ�ͳһ�ɲɼ�ģ����䡣*/	
    Uint32 nCapInfoOffset;	
    
    /* �ɼ�ģ��ɴ����Ϣ��������*/
    Uint32 nCapInfoSize;		
     
    /* ����ģ���ŵ���Ϣƫ�ƣ�ͳһ�ɲɼ�ģ����䡣*/		
    Uint32 nEncInfoOffset;	
    
    /* ����ģ��ɴ����Ϣ��������*/
    Uint32 nEncInfoSize;	
       
    /* �����㷨ģ���ŵ���Ϣƫ�ƣ�ͳһ�ɲɼ�ģ����䡣*/
    Uint32 nAlgInfoOffset;
    
    /* �����㷨ģ��ɴ����Ϣ��������*/		
    Uint32 nAlgInfoSize;

    /* Ӧ��ģ�����Ϣƫ�ƣ�ͳһ�ɲɼ�ģ����䡣*/
    Uint32 nAppInfoOffset;
    
    /* Ӧ��ģ��ɴ����Ϣ��������*/		
    Uint32 nAppInfoSize;
    
    /* ͼ����ģ���ŵ���Ϣƫ�ƣ�ͳһ�ɲɼ�ģ����䡣*/
    Uint32 nIspInfoOffset;
    
    /* ͼ����ģ��ɴ����Ϣ��������*/		
    Uint32 nIspInfoSize;	
    
    /* ������չ�� */		
    Uint32 reserved[8];			
} VIDEO_BufferDesc;		


/* ��Ƶ֡��������ص���Ϣ */
typedef struct 
{
    /* ����ID��������web�������Ŷ���һ�¡�*/
    Uint32 nStreamId;
    
    /* ͨ���ţ�����ͬSenosr�����������*/
    Uint32 nChId;
    
    /* �ɼ������ڲ�ʹ�� */
    Uint32 nInterId; 
    
    /* ���ü������ɼ������ڲ�ʹ�á�*/
    Uint32 nRefCnt;
   
    /* ֡��š�ÿ·���������Լ���֡��š�*/
    Uint32 nFrameNum;
    
    /* 
    * VIDEO_BufferInfo����������nBufInfoNumPlus + 1������Ƶ����Buffer��������
    * ͨ����һBuffer�����Եõ�����Buffer��
    */
    Uint32 nBufInfoNumPlus;
    
    /* ����ͬһ����������Buffer��*/
    OSA_ListHead nextBuffer;
    
    /* ������չ�� */
    Uint32 reserved[8];
} VIDEO_FrameInfo;


/* ��Ƶ����Buffer����Ϣ���塣Bufferͼʾ���£�ʵ����Ƶ��С�ڼ�ͷ��ʾ�������ڡ�
    
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
    /* ��Ƶ����Buffer���ڲ������� */
    Uint32 nIndex;
    
    /* ��Ƶ����Buffer�Ĵ�С */
    Uint32 nBufSize;
  
    /* ��Ƶ����Bufferʵ��ʹ�ô�С��һ�㱣����Ǳ������ݻ���ͳ������ʱʹ�á�*/
    Uint32 nBytesUsed;
    
    /* ��Ƶ����Bufferͷ������������������ĳЩ�㷨����JPEG����ȡ�*/
    Uint32 nTopPadNum;
    
    /* ��Ƶ����Buffer�ײ�����������������ĳЩ�㷨��*/
    Uint32 nBotPadNum;  
    
    /* ��Ƶ���ݵ�ʵ�ʿ�� */
    Uint32 nWidth;	
    
    /* ��Ƶ���ݵ�ʵ�ʸ߶� */			
    Uint32 nHeight;
    
    /* ��Ƶ���ݼ��õĴ�С��������ʹ�ô˴�С���б��롣*/
    VIDEO_Crop encCrop;
    
    /* 
    * ��Ƶ���ݵĸ�ʽ�����ж���BitStream����Frame�����жϾ���ĸ�ʽ��
    * �䶨���VIDEO_DataFormat��
    */	       
    Uint32 dataFormat;
    
    /* ÿ������ռ�õ�λ�����䶨���VIDEO_BitsPerPixel��*/
    Uint32 nBpp;
 
    /* 
     * ��Ƶ֡/���š��䶨���VIDEO_FidType��ɨ���ʽ�ʹ洢��ʽ������
     * scanFormat��fidBufType��Ա�С�
     */
    Uint32 nFieldId;
    
    /* ��Ƶ���ݲɼ���ɨ�跽ʽ���䶨���VIDEO_ScanFormat��*/
    Uint32 scanFormat;
    
    /* 
    * ����ɨ�����Ƶ���ݱ��淽ʽ��������ɨ�����Ƶ������Ч��
    * �䶨���VIDEO_FidBufType��
    */
    Uint32 fidBufType; 
    
    /* 
    * ������Ƶ�������˷�ת��������ת�Ȳ����������Ƕ������ϣ�����ģ��������㷨
    * ��ȡ��Ƶ����ʱ����Ҫ֪������Ϣ���䶨���VIDEO_RotateType��
    */
    Uint32 rotateFlags;
    
    /* ��Ƶ���ݵ�Ŀ������ʽ��Buffer���ݵı����ʽ���䶨���VIDEO_EncFormat��*/
    Uint32 encFlags;
    
    /* �������ݵ�֡���ͣ�YUV���ݲ��ù�ע���䶨���VIDEO_EncFrameType��*/
    Uint32 encFrmType;

    /* ��Ƶ����Buffer��Ⱦɫ�����ڻ��պʹ����ɼ������ڲ�ʹ�á�*/
    Uint32 colorMark;
    
    /* ʱ������ǵ���������ʱ�䣬��msΪ��λ��*/			
    Uint32 timeStamp;
    
    /* ����ϵͳ�δ��� */
    Uint32 osTicks;   
     
    /* 
     * ÿ��ƽ���ڵ��е�ַƫ�ơ�������������Ƶʱ������Ƶ����ÿ�еĵ�ַƫ�ƶ���
     * ����Ҫ����Ҫ��32Byte���룬���ͼ����п�Ȳ���32Byte�Ķ��룬�򱣴��
     * ͼ���ڵ�ַ�ϲ�����������������32Byte����ĵ�ַ�п�ȱ��档���Զ�ȡ��Ƶ
     * ����ʱ�����ֵ��Ϊ�е�ַƫ�ơ�
    */	
    Uint32 linePitch[VIDEO_MAX_PLANES];
    
    /*
    * ��Ƶ����Bufferʹ��һ����ά��ָ������bufAddr������һά�������֡/����
    * �ڶ�ά���������ɫƽ�档��Щָ����Щ����ΪNULL��Ҫ�������ݸ�ʽ����
    * �Դ�(dataFormat)������������ܼ��ֵ������ݸ�ʽ�������
    * 1. ����ɨ���YUV422/RAW���ݸ�ʽ��
    *    bufAddr[0][0] --> YUV422/RAW
    *    bufAddr[1][x] --> û��ʹ��
    * 2. ����ɨ����ż�泡�ֿ��洢��YUV422���ݸ�ʽ
    *    bufAddr[0][0] --> YUV422 Top Field
    *    bufAddr[1][0] --> YUV422 Bot Field  
    * 3. ����ɨ���YUV420SP_VU/YUV422SP_VU�����ݸ�ʽ
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> VU
    *    bufAddr[1][x] --> û��ʹ��  
    * 4. ����ɨ���YUV420SP_VU/YUV422SP_VU�����ݸ�ʽ
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> VU
    *    bufAddr[1][0] --> Y
    *    bufAddr[1][1] --> VU  
    * 5. ����ɨ���YUV420P/YUV422P�����ݸ�ʽ
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> U
    *    bufAddr[0][2] --> V
    *    bufAddr[1][x] --> û��ʹ��    
    * 6. ����ɨ���YUV420P/YUV422P�����ݸ�ʽ
    *    bufAddr[0][0] --> Y
    *    bufAddr[0][1] --> U
    *    bufAddr[0][2] --> V    
    *    bufAddr[1][0] --> Y
    *    bufAddr[1][1] --> U
    *    bufAddr[1][2] --> V  
    *
    * ��ʹ��VIDEO_YUV_INT_ADDR_IDX, VIDEO_RGB_ADDR_IDX, VIDEO_FID_TOP
    * �Ⱥ�����ȡ��Щָ��.
    * ����bufAddr�е�ָ��ָ��ĵ�ַ����Ƶ֡Buffer��һ���֣�����ֻ���
    * ��Ƶ֡Buffer��ַ���д�������ĵ�ַӳ�䣬Ȼ�����֮ǰ�ĵ�ַƫ��
    * ��ɵõ���ָ��ֵ������VIDEO_Frameδӳ��ǰ�ĵ�ַ��pOld��ӳ���
    * �ĵ�ַ��pNew����bufAddr[0][0] = pNew + (bufAddr[0][0] - pOld)��
    * ��Щ��ַ�ڼ����ʱ����Ҫǿ������ΪUint32��Uint8 *��
    */
    Ptr    bufAddr[VIDEO_MAX_FIELDS][VIDEO_MAX_PLANES]; 
    
    /* ������չ�� */
    Uint32 reserved[8];

    /* 
    * ռλ������ռ���ڴ档�������õ���һ�����ڵ�VIDEO_BufferInfo��ַ��
    * ��ʣ������������VIDEO_FrameInfo.nBufInfoNumPlus�С�������ͬһ����
    * ��Buffer�������������������ĸ������ݣ���ֱ��ͼ�ȡ�
    */
    Uint32 nextBufInfo[0];
} VIDEO_BufferInfo; 


/* ��Ʒ��ҵ��Ӧ�ñ�־��Ϣ��*/
typedef struct                 
{    
    /* ���ܽ�ͨ������ʶ��֡�����͡��䶨���VIDEO_ItcFrameType��*/
    Uint8 itcFrameId;
         
    /* ������չ�� */                
    Uint8 reserved[127];
} VIDEO_AppInfo;


/*
* ��Ƶ֡Buffer��֡ͷ��������������Ƶ֡Buffer����Ϣ��
* �˽ṹ�������е���Ϣ������listInfo��nodeInfo�⣬ֻ�вɼ������д��
* ������������ģ��ĳ���ֻ�ɶ����ṹ�����������ͼ��
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
    /* ������Ƶ֡Buffer���� */
    VIDEO_BufferDesc  bufDesc; 
    
    /* ��Ʒҵ��Ӧ�ñ�־��Ϣ */
    VIDEO_AppInfo     appInfo;    
    
    /* ��Ƶ���ݺ�������Ϣ */
    VIDEO_FrameInfo   frmInfo;
    
    /* ������չ�� */
    Uint32            reserved[64];
    
    /* ��Ƶ����Buffer��Ϣ */
    VIDEO_BufferInfo  bufInfo;

} VIDEO_FrameHead;


/*  
* �ദ������ģ�����Ƶ֡�������������ͣ���һ�������͵�ָ�롣�����м��Դ�ָ����Ϊ
* ���崫����Ϣ�����������յ�����Ϣ��������Ҫ�Ը�ָ����д��������ڴ��ַ�ռ�
* ӳ�䣬Ȼ������л�ȡ����Ƶ������ص���Ϣ�������յ��ĸ�����ָ�������pVideoFrame��
* ��VIDEO_FrameHead *pFrameHead = (VIDEO_FrameHead *)pVideoFrame��
*/
typedef void * VIDEO_Frame;


#ifdef __cplusplus
}
#endif


#endif  /*  _VIDEO_BUFFER_H_  */

