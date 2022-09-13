/*******************************************************************************
* dgs_errcode.h
*
* Copyright (C) 2017 ZheJiang Dahua Technology CO.,LTD.
*
* Author : xinjianjian <xin_jianjian@dahuatech.com>
* Version: V1.0.1  2017-08-01 Create
*
* --------------------------------------------------
* Desc: 设备故障诊断系统公共错误码定义头文件。
*   故障码格式定义如下：
*     举例：0x20000000 : 最高1字节代表产品线，次高字节代表模块编码，后2字节为某一个模块中的故障码。
*     0x20******	前端公共
*     0x21******	IPC(即0x21000000 ~ 0x21FFFFFF)
*     0x22******	球机
*     0x23******	智能交通
*     0x24******	存储公共
*     0x25******	DVR
*     0x26******	NVR
*     0x27******	EVS（*）
*     
*     0x**00****	boot
*     0x**01****	kernel //注1
*     0x**02****	prc.ko
*     0x**03****	pdc.ko
*     0x**04****	sensor.ko
*     **********	更多定义待后续添加和完善
*     0x**80****	80及以上是应用模块的错误码
*     
*     0x****0001	1号故障码，具体含义由模块自行决定
* 
* 注1：由于内核源码中无法直接调用故障诊断驱动模块的API进行错误码写入
*      因此内核源码里面目前无法进行错误码打桩，错误码暂时空缺
* -----------------------------------------------
*
* Modification:
*    Date    : 
*    Revision: 
*    Author  : 
*    Contents: 
*******************************************************************************/
#ifndef __DGS_ERRCODE_H
#define __DGS_ERRCODE_H

/* boot 错误码 */
#define DGS_EC_BOOT_ENV_CRC_ERROR		0x20000001

/* PDC 模块错误码 */
#define DGS_EC_PDC_UNKNOWN_DEVICE		0x20030001

/* Sensor 模块错误码 */
#define DGS_EC_SENSOR_INVALID			0x20040001
#define DGS_EC_SENSOR_XFER_ERR			0x20040002

/* 图像2A库 模块错误码*/
#define DGS_EC_LIBAEW_CFG_FAILED        0x20820001
#define DGS_EC_LIBAEW_hwidEx_ERR        0x20820002
#define DGS_EC_LIBAEW_GETH3A_FAILED     0x20820003

/* motor库 模块错误码 */
#define DGS_EC_MOTORDRV_CHIP_FAILED     0x20060001
#define DGS_EC_MOTORDRV_IRIS_FAILED     0x20060002
#define DGS_EC_MOTORDRV_PI_FAILED       0x20060003

#endif
