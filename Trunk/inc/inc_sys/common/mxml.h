/*******************************************************************************
* mxml.h
*
* Copyright (C) 2014-2016 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Hu Jianguang<hu_jianguang@dahuatech.com>
* Version: V1.0.0  2015-9-10 Create
*
* Desc: ����MXMLģ������ṩ��xml�ļ������ӿ�
*
* Modification: 
*    Date    :  
*    Revision:
*    Author  :
*    Contents:
*******************************************************************************/


#ifndef _MXML_H_
#define _MXML_H_

 
/* ========================================================================== */
/*                             ͷ�ļ���                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           ������Ͷ�����                                   */
/* ========================================================================== */
/*�ļ��������,�ϲ�ģ�鲻��Ҫ�������еľ�������,*/
/*ֻ��Ҫ�ڵ��ø��ļ������ӿ�ʱ�Ѿ�����뼴��*/
typedef Handle MXML_Handle;

 
 /* xml�ļ����ػص�ģʽ*/
typedef enum
{
     MXML_INTEGER_MODE  = 0, /* ���е����ݽڵ�����Կո�ָ������ */
     MXML_TEXT_MODE,         /* ���е����ݽڵ�����Կո�ָ���ı��ַ��� */ 
}MXML_CallbackMode;


typedef enum 
{ 
  MXML_ELEMENT_TYPE = 0,	    /* XML element with attributes */
  MXML_INTEGER_TYPE,		    /* Integer value */
  MXML_OPAQUE_TYPE,				/* Opaque string */
  MXML_REAL_TYPE,				/* Real value */
  MXML_TEXT_TYPE			    /* Text fragment */ 
}MXML_Type;


typedef struct
{
    Char	   *pName;		/* Attribute name */
    Char	   *pValue;		/* Attribute value */
  
}MXML_Attr;

typedef struct 
{
    Char		*pName;		/* Name of element */
    Int32	     numAttrs;	/* Number of attributes */
    MXML_Attr	*pAttrs;    /* Attributes */

}MXML_Element;


typedef struct 
{
    Int32	    whitespace;	  /* Leading whitespace */
    Char	    *pString;	  /* Fragment string */
  
}MXML_Text;


typedef union 
{
    MXML_Element	element;	/* Element */
    Int32			integer;	/* Integer number */
    Char			*pOpaque;	/* Opaque string */
    Float64		    real;		/* Real number */
    MXML_Text		text;		/* Text fragment */ 
    
}MXML_Value;


/* mxmlÿ���ڵ���� */
struct MXML_Node_s
{
    MXML_Type	          type;		    /* Node type */ 
    struct MXML_Node_s	 *pNext;	    /* Next node under same parent */ 
    struct MXML_Node_s	 *pPrev;	    /* Previous node under same parent */
    struct MXML_Node_s	 *pParent;	    /* Parent node */
    struct MXML_Node_s	 *pChild;		/* First child node */
    struct MXML_Node_s	 *pLastChild;	/* Last child node */
    MXML_Value            value;	    /* Node value */  
    
};

typedef struct MXML_Node_s MXML_Node; 

/* �ļ����ز��� 64�ֽ� */
typedef struct
{
    const Char *fileName;       /* �����ļ�·����Ϣ�������ļ���*/
    Uint32      callbackMode;   /* �ļ�����ģʽ,�μ�MXML_CallbackMode���� */
    Uint8       fileEncrypt;    /* �����ļ��Ƿ���ܣ� OSA_TRUE: load���ܹ����ļ� */
    Uint8       reserved[55];
} MXML_InitParams;


/* ========================================================================== */
/*                          ���ݽṹ������                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          ����������                                        */
/* ========================================================================== */
 
/*******************************************************************************
* ������  : MXML_LoadFile
* ��  ��  : �ú����������һ��XML�ļ�(����ģʽΪUTF-8��UTF-16)���������ڴ��
*
* ��  ��  : - fileName: �����ļ�·����Ϣ�������ļ���
*           - mode:     �ļ�����ģʽ,�μ�MXML_CallbackMode����
*
* ��  ��  : - phMxml:   �������ָ��,���򿪳ɹ�ʱ����������
* ����ֵ  : ��NULL:  �����ָ�롣
*           NULL  :  ʧ�ܡ�
*******************************************************************************/ 
MXML_Node *MXML_LoadFile(MXML_InitParams *pInitParams, 
                         MXML_Handle *phMxml);


/*******************************************************************************
* ������  : MXML_Delete
* ��  ��  : �ú��������ͷ�xml�ļ�����ʱ,����������Դ�������ٲ������ָ��Ķ���,
*           ��˵��øýӿں�ԭ�еĲ������������ʹ�á�
*           �ú����������ں�̬�ж������ĵ���
*
* ��  ��  : - hMxml: �������
* ��  ��  : ��
* ����ֵ  : OSA_SOK:   �رճɹ�
*           OSA_EFAIL: �ر�ʧ��
*******************************************************************************/
Int32 MXML_Delete(MXML_Handle hMxml);


/*******************************************************************************
* ������  : MXML_FindElement
* ��  ��  : ����ָ��Ԫ�����ڵĽڵ㡣
*
* ��  ��  : - hMxml: �������
*           - pNode: Ԫ�ص��ϲ�ڵ�ָ�롣
*           - pName: Ԫ�ص����ơ�
*           - pAttr: Ԫ�ص�����������ΪNULL��
*           - pValue: Ԫ�ص�����ֵ����ΪNULL��
*
* ��  ��  : ��
* ����ֵ  : ��NULL: Ԫ�ؽڵ�ָ��
*           NULL  : ʧ��
*******************************************************************************/ 
MXML_Node *MXML_FindElement(MXML_Handle hMxml, MXML_Node *pNode, 
                            const Char  *pName, 
                            const Char  *pAttr, 
                            const Char  *pValue);

/*******************************************************************************
* ������  : MXML_GetInteger
* ��  ��  : ��ȡָ��Ԫ�ؽڵ��������ֵ��֧����������16���ơ�
*           ע��xml�ļ�����ģʽΪ����ʱ��Ч��
*
* ��  ��  : - hMxml: �������
*           - pNode: Ԫ�ؽڵ�ָ�롣 
*
* ��  ��  : ��
* ����ֵ  : ����ֵ
*          
*******************************************************************************/ 
Int32 MXML_GetInteger(MXML_Handle hMxml, MXML_Node *pNode);

 
/*******************************************************************************
* ������  : MXML_GetText
* ��  ��  : ��ȡָ��Ԫ�ؽڵ���ı���
*           ע��xml�ļ������ı�ģʽΪʱ��Ч��
*
* ��  ��  : - hMxml: �������
*           - pNode: Ԫ�ؽڵ�ָ�롣 
*
* ��  ��  : ��
* ����ֵ  : ��NULL: �ı��ַ���ָ�롣
*           NULL:   ʧ�ܡ�
*******************************************************************************/ 
const Char *MXML_GetText(MXML_Handle hMxml, MXML_Node *pNode);


/*******************************************************************************
* ������  : MXML_ElementGetAttr
* ��  ��  : ��ȡָ��Ԫ�ؽڵ��������������ֵ��
*
* ��  ��  : - hMxml: �������
*           - pNode: Ԫ�ؽڵ�ָ��
*           - pName: ������
*
* ��  ��  : ��
* ����ֵ  : ��NULL: ����ֵ�ı��ַ���ָ�롣
*           NULL:   ʧ�ܡ�
*******************************************************************************/
const Char *MXML_ElementGetAttr(MXML_Handle hMxml, MXML_Node *pNode,	
                                const Char *pName);

 
#ifdef __cplusplus
} 
#endif

#endif  /*  _MXML_H_  */

