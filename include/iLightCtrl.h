#include "common.h"
#include "DataType.h"
#ifndef _I_LIGHT_CTRL_H_
#define _I_LIGHT_CTRL_H_

#ifndef OUTTYPE_H
#define OUTTYPE_H extern "C" _declspec(dllexport)
#endif OUTTYPE_H

enum OPER_LIGHT_TYPE
{
    TYPE_OPEN = 0,         //打开光源
    TYPE_CLOSE,        //关闭光源
    TYPE_SET_VAULE,    //设置光源亮度
    TYPE_RESET,    //重置串口
};

/**********************************************************************
函数      : iLightInstance
功能      : 初始化光源控制器
输入参数  : seriaInfo:光源控制器信息表
输出参数  : N/A
返回值    : LIGHTCTL_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int iLightInstance(LightInfo seriaInfo[],int cout, LightTypeInfo *pType = NULL, int iTypeCount = 1) ; 

/**********************************************************************
函数      : destroyLightInstance
功能      : 去初始化光源控制器
输入参数  : N/A
输出参数  : N/A
返回值    : N/A
备注      : 
**********************************************************************/
OUTTYPE_H void destroyLightInstance() ; 

/**********************************************************************
函数      : initLightCtrl
功能      : 释放光源控制器资源
输入参数  : comName:串口名
输出参数  : N/A
返回值    : LIGHTCTL_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int deInitLightCtrl(string comName);

/**********************************************************************
函数      : openLight
功能      : 打开指定频道光源
输入参数  : comName:串口名;index,指定的光源通道1开始;
输出参数  : N/A
返回值    : LIGHTCTL_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int openLight(string comName,int index);

/**********************************************************************
函数      : closeLight
功能      : 关闭指定频道光源
输入参数  : comName:串口名;index,指定的光源通道1开始;
输出参数  : N/A
返回值    : LIGHTCTL_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int closeLight(string comName,int index);

/**********************************************************************
函数      : setLightValue
功能      : 设置指定频道光源亮度
输入参数  : comName:串口名;index,指定的光源通道1开始;value,亮度值;
输出参数  : N/A
返回值    : LIGHTCTL_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int setLightValue(string comName,int index ,unsigned int value);

/**********************************************************************
函数      : getLightValue
功能      : 获取指定频道光源亮度值
输入参数  : comName:串口名;index,指定的光源通道1开始;value,亮度值;
输出参数  : N/A
返回值    : LIGHTCTL_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int getLightValue(string comName,int index ,unsigned int &val);
/**********************************************************************
函数      : JudgeLightConnect
功能      : 判断光源控制器是否连接成功
输入参数  : 串口名称
输出参数  : N/A
返回值    : LIGHTCTL_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int JudgeLightConnect(string comName);

#endif