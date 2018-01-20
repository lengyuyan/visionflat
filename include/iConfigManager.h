#ifndef _I_CONFIG_MANAGER_H_
#define _I_CONFIG_MANAGER_H_

#include "DataType.h"

#ifndef EXPORT_C
#define EXPORT_C extern "C"
#endif
#ifndef DLL_EXPORT
#define DLL_EXPORT _declspec(dllexport)
#endif



/**********************************************************************
函数      : InitConfig
功能      : 初始化处理
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool InitConfig(void *pData);

/**********************************************************************
函数      : ReleaseConfigInstance
功能      : 去初始化处理
输入参数  : N/A
输出参数  : N/A
返回值    : N/A
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT void ReleaseConfigInstance();

/**********************************************************************
函数      : SaveSysConfig
功能      : 保存系统参数
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SaveSysConfig(void *pData);


/**********************************************************************
函数      : SaveSysCamCalibrate
功能      : 保存相机的标定数据(点位标定法)
输入参数  : CamerIndex  相机索引
            DataCount   标定数据的数量
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SaveSysCamCalibrate(int CamerIndex,int ToolIndex,int DataCount,double MotorX[],double MotorY[],double PixRow[],double PixCol[]);

EXPORT_C DLL_EXPORT bool SaveMecAndCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pCamRot),double (*pMecAndCam),
	                       double (*pImageRela),double (*pImagAbs),int DataNum);

EXPORT_C DLL_EXPORT bool SavePicCenter(int CamIndex,int AxeIndex,int ToolIndex,double X,double Y);

EXPORT_C DLL_EXPORT bool ReadMecAndCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pMecAndCam),int& DataNum);

EXPORT_C DLL_EXPORT bool SaveSysCamPixSize(int CamIndex,int AxeIndex,  int ToolIndex,double PixSize);

EXPORT_C DLL_EXPORT bool SaveSysRotAxleToCam(int CamIndex,int AxisIndex, int ToolIndex,double RotX,double RotY);
EXPORT_C DLL_EXPORT bool SaveSysRotAxleToTool(int AxisIndex, int ToolIndex,double RotX,double RotY);

EXPORT_C DLL_EXPORT void ConfigSaveSysRotAxleAdjust(int AxisIndex, int ToolIndex,double RotXAd,double RotYAd);
EXPORT_C DLL_EXPORT void ConfigReadSysRotAxleAdjust(int AxisIndex, int ToolIndex,double& RotXAd,double& RotYAd);

EXPORT_C DLL_EXPORT void SaveProctocolData(const PROCTOCOLDATA& ProctoData);

EXPORT_C DLL_EXPORT void ConfigSaveProductMap(void* pMapData);

EXPORT_C DLL_EXPORT void ConfigReadProductMap(void* pMapData);

EXPORT_C DLL_EXPORT void ConfigSaveDefaultProduct(string productName);

EXPORT_C DLL_EXPORT void ConfigReadDefaultProduct(string& productName);


//EXPORT_C DLL_EXPORT void ConfigSaveProductId(int ProductId,string ProductName);

//EXPORT_C DLL_EXPORT void ConfigReadProductId(void* lpProductMap);


/**********************************************************************
函数      : GetSceneConfig
功能      : 初始化处理
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool GetSceneConfig(const char * pConfigPath, void *pData);


EXPORT_C DLL_EXPORT char* ConfigGetsysPath();

EXPORT_C DLL_EXPORT char* ConfigGetSceneName();

EXPORT_C DLL_EXPORT char* ConfigGetScenePath(const char* SceneFloderName);

/**********************************************************************
函数      : GetSceneProcProcess
功能      : 从配置文件中获取场景处理类型
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool GetSceneProcProcess(const char * pConfigPath, int& ProcProcess);

/**********************************************************************
函数      : SetScenePara
功能      : 更新场景
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SetSceneConfig(const char * pConfigPath, void *pData);





#endif 
