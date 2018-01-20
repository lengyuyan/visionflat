#ifndef _I_COMMUNICATION_H_
#define _I_COMMUNICATION_H_
#include "DataType.h"

#ifndef EXPORT_C
#define EXPORT_C extern "C"
#endif
#ifndef DLL_EXPORT
#define DLL_EXPORT _declspec(dllexport)
#endif

typedef  bool (*pFunCallback)(int index, int ires);
typedef  void (*pFunCloseSys)(int iType, void *param);

/**********************************************************************
函数      : InitConnect
功能      : 初始化处理
输入参数  : pCount 初始化配置项
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT int InitConnect(void *pCount);

/**********************************************************************
函数      : InitConnect
功能      : 初始化处理
输入参数  : pCount 初始化配置项
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT int UnInitConnect();

EXPORT_C DLL_EXPORT void ReleaseConnectInstance();
/**********************************************************************
函数      : RegesterCallBack
功能      : 注册触发接口
输入参数  : 回调函数
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT bool RegesterCallBack(pFunCallback pfun);

/**********************************************************************
函数      : RegesterCloseSys
功能      : 通知进程关闭
输入参数  : 回调函数
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT bool RegesterCloseSys(pFunCloseSys pfun);


/**********************************************************************
函数      : 
功能      : 图像采集完成
输入参数  : iSceneID 场景ID
iErrorCode 错误码
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT int GrabFinished(int iSceneID,CHECK_RESULT *pResult, int iErrorCode);

/**********************************************************************
函数      : getImagePathFinished
功能      : 图像采集完成
输入参数  : iSceneID 场景ID
			path  图片路径
            iErrorCode 错误码
输出参数  : N/A
返回值    : 成功或失败
备注      : 
修改记录：
日期		修改人       修改内容
2016-08-16 guopengzhou   获得图片路径
**********************************************************************/
EXPORT_C DLL_EXPORT int getImagePathFinished(int iSceneID, char* path,int iErrorCode = 0);

/**********************************************************************
函数      : ProcessFinished
功能      : 处理完成
输入参数  : pResult 处理结果
            iErrorCode 错误码
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT int CommunicationProcessFinished(int iSceneID, CHECK_RESULT *pResult, int iErrorCode = 0);

/**********************************************************************
函数      : ProcessFinished
功能      : 处理完成
输入参数  : pResult 处理结果
            iErrorCode 错误码
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT int SendImage(int iSceneID, void *pBuffer, int size,int w,int h,int iErrorCode = 0);
/**********************************************************************
函数      : OpenLight
功能      : 光源操作
输入参数  : index 光源索引
            type  开关类型, 1为开， 0为关
输出参数  : N/A
返回值    : 成功或失败
备注      : 光源通过IO口操作， IO口独占
**********************************************************************/
EXPORT_C DLL_EXPORT bool OpenLight(int index, int type);
/**********************************************************************
函数      : ResetFinished
功能      : 重置系统返回消息
输入参数  : iSceneID 场景ID
            iErrorCode 错误码
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT int ResetFinished(int iErrorCode = 0);

/**********************************************************************
函数      : PantCheckSend
功能      : 心跳检测发送数据函数
输入参数  :  
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT int PantCheckSend();


EXPORT_C DLL_EXPORT void SetCommuniWebProctocol(const PROCTOCOLDATA& ProctoData);
EXPORT_C DLL_EXPORT void GetCommuniWebProctocol(PROCTOCOLDATA& ProctoData);


#endif 
