#include<windows.h>	
#include <list>
#include "DataType.h"

using namespace std;

#ifndef _I_IMAGE_CTRL_H_
#define _I_IMAGE_CTRL_H_

#ifndef OUTTYPE_H
#define OUTTYPE_H extern "C" _declspec(dllexport)
#endif OUTTYPE_H

/**********************************************************************
函数      : imageInstance
功能      : 实列化iImageCtrl接口中的TCamera类
输入参数  : camaratype 指定的相机和接口类型
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 初始化iImageCtrl
**********************************************************************/
OUTTYPE_H int imageInstance(int dllnum, string dlllist[]);


/**********************************************************************
函数      : deinit
功能      : 释放图像采集卡资源
输入参数  : N/A
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int deinitImage();	

/**********************************************************************
函数      : getGUIDList
功能      : 获取相机guid列表
输入参数  : N/A
输出参数  : guidlist，相机的guid列表
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int getGUIDList(std::list<std::string> &guidlist,int dllnum,string dlllist[]);

/**********************************************************************
函数      : getParameInfo
功能      : 获取相机参数
输入参数  : name,指定的相机名(guid)
			sepciPara,指定参数项
输出参数  : val,接收指定项的值
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int getParameInfo(string name,CAMARA_PARA_ITEM speciPara,unsigned long &val);

/**********************************************************************
函数      : setParameInfo
功能      : 设置相机参数
输入参数  : name,指定的相机名
			speciPara,指定的参数项
			val,指定参数项的值
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int setParameInfo(string name,CAMARA_PARA_ITEM speciPara,int val);

/**********************************************************************
函数      : open
功能      : 打开相机
输入参数  : name,指定的相机名(guid)
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int openCamera (string name );

/**********************************************************************
函数      : close
功能      : 关闭相机
输入参数  : 指定的相机名(guid)
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int closeCamera (string name);

/**********************************************************************
函数      : getImageSize
功能      : 获取相机图像大小
输入参数  : name,指定的相机名(guid)
输出参数  : w,图像宽
			h,图像高
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int getImageSize(string name ,int &w, int &h);

/**********************************************************************
函数      : getImage
功能      : 获取相机图像
输入参数  : name,指定的相机名(guid)
输出参数  : pImage,接收图像的缓存的指针
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int getImage(string name, void *pImage);

/**********************************************************************
函数      : setGain
功能      : 设置相机增益
输入参数  : name,指定的相机名(guid)
			value,增益值
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int setGain(string name,unsigned int value); 
/**********************************************************************
函数      : getGain
功能      : 获取相机增益
输入参数  : name,指定的相机名(guid)
输出参数  : N/A
返回值    : 增益值
备注      : 
**********************************************************************/
OUTTYPE_H int getGain(string name); 

/**********************************************************************
函数      : setExposureTime
功能      : 设置相机曝光时间
输入参数  : name,指定相机名(guid)
		   value,曝光值
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int setExposureTime(string name,unsigned int value);
/**********************************************************************
函数      : getExposureTime
功能      : 获取相机曝光时间
输入参数  : name,指定相机名(guid)
输出参数  : N/A
返回值    : 曝光值
备注      : 
**********************************************************************/
OUTTYPE_H int getExposureTime(string name);

/**********************************************************************
函数      : startSnap
功能      : 开始实时取像
输入参数  : name,指定相机名(guid)
		   Callback，通知显示的回调函数指针
		   pParent, ？？？？
输出参数  : pImage,接收图像的缓存的指针
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int startSnap(string name,void *pImage,void(*Callback)(void* context),void *pParent);

/**********************************************************************
函数      : stopSnap
功能      : 停止实时取像
输入参数  : name,指定的相机名（guid）
输出参数  : N/A
返回值    : CAMARA_ERRORCODE 错误码
备注      : 
**********************************************************************/
OUTTYPE_H int stopSnap(string name);

#endif 