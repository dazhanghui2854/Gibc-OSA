/*******************************************************************************
* mxml.h
*
* Copyright (C) 2014-2016 ZheJiang Dahua Technology CO.,LTD.
*
* Author : Hu Jianguang<hu_jianguang@dahuatech.com>
* Version: V1.0.0  2015-9-10 Create
*
* Desc: 定义MXML模块对外提供的xml文件操作接口
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
/*                             头文件区                                       */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           宏和类型定义区                                   */
/* ========================================================================== */
/*文件句柄类型,上层模块不需要关心其中的具体内容,*/
/*只需要在调用各文件操作接口时把句柄传入即可*/
typedef Handle MXML_Handle;

 
 /* xml文件加载回调模式*/
typedef enum
{
     MXML_INTEGER_MODE  = 0, /* 所有的数据节点包含以空格分割的整数 */
     MXML_TEXT_MODE,         /* 所有的数据节点包含以空格分割的文本字符串 */ 
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


/* mxml每个节点参数 */
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

/* 文件加载参数 64字节 */
typedef struct
{
    const Char *fileName;       /* 包含文件路径信息的完整文件名*/
    Uint32      callbackMode;   /* 文件加载模式,参见MXML_CallbackMode定义 */
    Uint8       fileEncrypt;    /* 加载文件是否加密， OSA_TRUE: load加密过的文件 */
    Uint8       reserved[55];
} MXML_InitParams;


/* ========================================================================== */
/*                          数据结构定义区                                    */
/* ========================================================================== */


/* ========================================================================== */
/*                          函数声明区                                        */
/* ========================================================================== */
 
/*******************************************************************************
* 函数名  : MXML_LoadFile
* 描  述  : 该函数负责加载一个XML文件(编码模式为UTF-8或UTF-16)并解析到内存里。
*
* 输  入  : - fileName: 包含文件路径信息的完整文件名
*           - mode:     文件加载模式,参见MXML_CallbackMode定义
*
* 输  出  : - phMxml:   操作句柄指针,当打开成功时输出操作句柄
* 返回值  : 非NULL:  根结点指针。
*           NULL  :  失败。
*******************************************************************************/ 
MXML_Node *MXML_LoadFile(MXML_InitParams *pInitParams, 
                         MXML_Handle *phMxml);


/*******************************************************************************
* 函数名  : MXML_Delete
* 描  述  : 该函数负责释放xml文件加载时,所创建的资源并将销毁操作句柄指向的对象,
*           因此调用该接口后原有的操作句柄不能再使用。
*           该函数不能在内核态中断上下文调用
*
* 输  入  : - hMxml: 操作句柄
* 输  出  : 无
* 返回值  : OSA_SOK:   关闭成功
*           OSA_EFAIL: 关闭失败
*******************************************************************************/
Int32 MXML_Delete(MXML_Handle hMxml);


/*******************************************************************************
* 函数名  : MXML_FindElement
* 描  述  : 查找指定元素所在的节点。
*
* 输  入  : - hMxml: 操作句柄
*           - pNode: 元素的上层节点指针。
*           - pName: 元素的名称。
*           - pAttr: 元素的属性名。可为NULL。
*           - pValue: 元素的属性值。可为NULL。
*
* 输  出  : 无
* 返回值  : 非NULL: 元素节点指针
*           NULL  : 失败
*******************************************************************************/ 
MXML_Node *MXML_FindElement(MXML_Handle hMxml, MXML_Node *pNode, 
                            const Char  *pName, 
                            const Char  *pAttr, 
                            const Char  *pValue);

/*******************************************************************************
* 函数名  : MXML_GetInteger
* 描  述  : 获取指定元素节点的整型数值。支持正负数及16进制。
*           注意xml文件加载模式为整数时有效。
*
* 输  入  : - hMxml: 操作句柄
*           - pNode: 元素节点指针。 
*
* 输  出  : 无
* 返回值  : 整数值
*          
*******************************************************************************/ 
Int32 MXML_GetInteger(MXML_Handle hMxml, MXML_Node *pNode);

 
/*******************************************************************************
* 函数名  : MXML_GetText
* 描  述  : 获取指定元素节点的文本。
*           注意xml文件加载文本模式为时有效。
*
* 输  入  : - hMxml: 操作句柄
*           - pNode: 元素节点指针。 
*
* 输  出  : 无
* 返回值  : 非NULL: 文本字符串指针。
*           NULL:   失败。
*******************************************************************************/ 
const Char *MXML_GetText(MXML_Handle hMxml, MXML_Node *pNode);


/*******************************************************************************
* 函数名  : MXML_ElementGetAttr
* 描  述  : 获取指定元素节点的属性名的属性值。
*
* 输  入  : - hMxml: 操作句柄
*           - pNode: 元素节点指针
*           - pName: 属性名
*
* 输  出  : 无
* 返回值  : 非NULL: 属性值文本字符串指针。
*           NULL:   失败。
*******************************************************************************/
const Char *MXML_ElementGetAttr(MXML_Handle hMxml, MXML_Node *pNode,	
                                const Char *pName);

 
#ifdef __cplusplus
} 
#endif

#endif  /*  _MXML_H_  */

