#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_
#pragma once

#include "DataType.h"
#include <map>
using namespace std;



class CConfigManager
{
public:
    CConfigManager(void);
    ~CConfigManager(void);

public:
    bool InitConfig(void *pData);
    const char* GetSysConfigPath(void);
    bool SaveConfig(void *pData);
	bool SaveCamCalibrate(int CamerIndex,int ToolIndex,int DataCount,double MotorX[],double MotorY[],double PixRow[],double PixCol[]);
	bool SaveMecAndCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pCamRot),double (*pMecAndCam),
		                  double (*pImageRela),double (*pImagAbs),int DataNum);
	bool SavePicCenter(int CamIndex,int AxeIndex,int ToolIndex,double X,double Y);
	bool ReadMecAndCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pMecAndCam),int& DataNum);//暂未使用
	bool ReadCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pCamRot),int& DataNum);

	bool ReadImageMetrix(int CamIndex,int AxeIndex,int ToolIndex,double (*pImageRela),double (*pImageAbs),int& DataNum);
	

	bool SaveSysCamPixSize(int CamIndex,int AxeIndex, int ToolIndex,double PixSize);
	bool SaveSysRotAxleToCam(int CamIndex,int AxisIndex, int ToolIndex,double RotX,double RotY);
	bool SaveSysRotAxleToTool(int AxisIndex, int ToolIndex,double RotX,double RotY);

	void SaveSysRotAxleAdjust(int AxisIndex, int ToolIndex,double RotXAdjust,double RotYAdjust);
	void ReadSysRotAxleAdjust(int AxisIndex, int ToolIndex,double& RotXAdjust,double& RotYAdjust);


    void SaveProctocolData(const PROCTOCOLDATA& ProctoData);
	void SaveProductMap(void* pMapData);
	void ReadProductMap(void* pMapData);
	void SaveDefaultProduct(string productName);
	void ReadDefaultProduct(string& productName);

	//void SaveProductId(int ProductId,string ProductName);
	//void ReadProductId(void* lpProductMap);


    bool ReadSceneConfig(const char *pPath, void *pData);
	bool ReadSceneProcProcess(const char *pPath, int& ProcProcess);
	char* GetSceneconfigPath(const char* SceneFloderName);

    bool SaveSceneConfig(const char *pPath, void *pData);

    //封装读写double类型， 及写int类型配置项
    double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName,
        double dDefault, LPCTSTR lpFileName);
    BOOL WritePrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName,
        double dValue, LPCTSTR lpFileName);
    BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT Value, LPCTSTR lpFileName);   

public:
	char m_SysconfigPath[256];
	char m_SceneconfigName[256];
	char m_CurSceneConfigPath[256];

public:
    static CConfigManager *GetInstance();
    static void ReleaseInstance();

private:
    static CConfigManager *m_pInstance;
};

#endif