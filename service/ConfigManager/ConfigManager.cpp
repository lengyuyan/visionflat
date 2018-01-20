// ConfigManager.cpp : 定义 DLL 应用程序的导出函数。
//
#pragma  once

#include "stdafx.h"
#include "ConfigManager.h"
#include "DataType.h"
#include "sysapi.h"
#include "common.h"
//#include "AssistantApply.h"

#include  <io.h>

#define CONFIG_FILE_NAME   "config.ini"


//定义配置文件中的字符意义

#define  WITH_CAMPARAM   "WithCamParam"        //标定或测量是否采用相机参数模式
#define  CALBOARD_THICKNESS "CalBoradThickness"     //标定板的厚度




CConfigManager *CConfigManager::m_pInstance = NULL;

CConfigManager::CConfigManager(void)
{
	 memset(m_SysconfigPath,0,sizeof(m_SysconfigPath));//配置类对应 一个配置文件
	 //场景名字名字也一样
	strcpy(m_SceneconfigName,CONFIG_FILE_NAME);
}


CConfigManager::~CConfigManager(void)
{
}

CConfigManager *CConfigManager::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CConfigManager;
    }
    return m_pInstance;
}

void CConfigManager::ReleaseInstance()
{
    if (NULL != m_pInstance)
    {
        //delete m_pInstance;
        m_pInstance = NULL;
    }
}

bool CConfigManager::InitConfig(void *pData)
{
	bool nRes = true;
    if (NULL == pData)
    {
        return false;
    }
    char szCurrentPath[512] = {0};
    char isrPath[512] = {0};
    get_module_path((LPSTR)szCurrentPath);  //取得config.ini的地址
    strcpy_s(isrPath,512, szCurrentPath);
    strcat_s(szCurrentPath, CONFIG_FILE_NAME);

    if(-1 == access(szCurrentPath, 0))//ini文件不存在
    {
        return false;
    }
	strcpy(m_SysconfigPath,szCurrentPath);
    char szBuf[CONFIG_VALUE_LENGTH] = {0};
    char szName[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] ={0};
    SYSTEM_PARAM *param = (SYSTEM_PARAM *)pData;
    param->iIsInstall = GetPrivateProfileInt("install", "install", 0, szCurrentPath);
    param->iWindowNum = GetPrivateProfileInt("window", "num", 0, szCurrentPath);
    param->iSceneNum = GetPrivateProfileInt("scene", "num", 0, szCurrentPath);
	if (0 == GetPrivateProfileInt("WorkMode", "Simulate", 0, szCurrentPath))
	{
		param->IsSimulate = false;
	}else
	{
		param->IsSimulate = true;
	}
	//读取要加载的相机dll
	param->iCameraDllNum = GetPrivateProfileInt("cameradll", "num", 0, szCurrentPath);
	if ( 0 < param->iCameraDllNum)
	{
		for (int i = 0; i < param->iCameraDllNum; i++)
		{
			memset(szBuf, 0, CONFIG_VALUE_LENGTH);
			sprintf_s(szBuf, "cameradll%d", i);
			memset(szName, 0, CONFIG_VALUE_LENGTH);
			GetPrivateProfileString("cameradll", szBuf, "cameradll", szName, CONFIG_VALUE_LENGTH, szCurrentPath);
			param->pstrCameraDll[i] = szName;
		}
	}
	///读取场景
    if (0 < param->iSceneNum)
    {
        for (int i = 0; i < param->iSceneNum; i++)
        {
            memset(szBuf, 0, CONFIG_VALUE_LENGTH);
            sprintf_s(szBuf, "scene%d", i);
            memset(szName, 0, CONFIG_VALUE_LENGTH);
            GetPrivateProfileString("scene", szBuf, "sence", szName, CONFIG_VALUE_LENGTH, szCurrentPath);
            param->pstrSceneName[i] = szName;
        }
    }
    //光源个数
    param->iLightNum = GetPrivateProfileInt("light", "lightnum", 0, szCurrentPath);
    param->iLightTypeNum = GetPrivateProfileInt("light", "lighttypenum", 2, szCurrentPath);
    param->iAutoOL = GetPrivateProfileInt("light", "autool", 1, szCurrentPath);
    //加载光源类型及对应参数
    int iValue = 0;
    for (int i = 0; i < param->iLightTypeNum; i++)
    {
        memset(szBuf, 0, CONFIG_VALUE_LENGTH);
        sprintf_s(szBuf, "lighttype%d", i);
        memset(szName, 0, CONFIG_VALUE_LENGTH);
        iValue = GetPrivateProfileInt(szBuf, "baudrate", 9600, szCurrentPath);
        param->strLightType[i].BaudRate = iValue;
        iValue = GetPrivateProfileInt(szBuf, "parity", 0, szCurrentPath);
        param->strLightType[i].Parity = iValue;
        iValue = GetPrivateProfileInt(szBuf, "bytesize", 8, szCurrentPath);
        param->strLightType[i].ByteSize = iValue;
        iValue = GetPrivateProfileInt(szBuf, "stopbits", 0, szCurrentPath);
        param->strLightType[i].StopBits = iValue;
        int iE = GetPrivateProfileString(szBuf, "name", "NDPS", szName, CONFIG_VALUE_LENGTH, szCurrentPath);
        param->strLightType[i].TypeName = szName;
    }
    if (0 < param->iSceneNum)
    {
        iValue = 0;
        for (int i = 0; i < param->iLightNum; i++)
        {
            memset(szBuf, 0, CONFIG_VALUE_LENGTH);
            sprintf_s(szBuf, "light%d", i);
            memset(szName, 0, CONFIG_VALUE_LENGTH);
            iValue = GetPrivateProfileInt(szBuf, "lighttype", 0, szCurrentPath);
            param->strLightInfo[i].strLightType = (LIGHTCTL_TYPE)iValue;
            iValue = GetPrivateProfileInt(szBuf, "lightchanum", 4, szCurrentPath);
            param->strLightInfo[i].Channel = iValue;
            iValue = GetPrivateProfileInt(szBuf, "lightvalue", 200, szCurrentPath);
            param->strLightInfo[i].DefaultValue = iValue;
            int iE = GetPrivateProfileString(szBuf, "commname", "COM1", szName, CONFIG_VALUE_LENGTH, szCurrentPath);
            param->strLightInfo[i].PortName = szName;
        }
    }
	//夹具
	param->iToolNum = GetPrivateProfileInt("Tools", "toolnum", 0, szCurrentPath);
	if (param->iToolNum>TOOL_NUMBER)
	{
		param->iToolNum = TOOL_NUMBER;
	}
	//轴
	param->iAxisNum = GetPrivateProfileInt("Axis","Axisnum" ,0, szCurrentPath);
	if (param->iAxisNum>AXIS_NUMBER)
	{
		param->iAxisNum = AXIS_NUMBER;
	}
   //相机
    param->iCameraNum = GetPrivateProfileInt("camera", "cameranum", 0, szCurrentPath);
	if (param->iCameraNum > CAMERA_NUMBER)
	{
		param->iCameraNum = CAMERA_NUMBER;
	}   
    for (int iCaremaIndex = 0; iCaremaIndex < param->iCameraNum; iCaremaIndex++)
    {
        memset(szBuf, 0, CONFIG_VALUE_LENGTH);
        sprintf_s(szBuf, "camera%d",iCaremaIndex);
        memset(szName, 0, CONFIG_VALUE_LENGTH);
        GetPrivateProfileString("camera", szBuf, "98765", szName, CONFIG_VALUE_LENGTH, szCurrentPath);
		string stTemp = szName;
        param->pstrCamareGUID[iCaremaIndex] = stTemp;
    }
	//读取相机像素大小值和其他
	for (int iCaremaIndex = 0; iCaremaIndex < param->iCameraNum; iCaremaIndex++)
	{
		for (int AxiIndex = 0;AxiIndex < param->iAxisNum ; AxiIndex++ )
		{		
			for (int ToolIndex = 0;ToolIndex<param->iToolNum;ToolIndex++)
			{
				memset(szName,0,sizeof(szName));
				sprintf_s(szName,"CamCal%d_Axe%d_Tool%d",iCaremaIndex,AxiIndex,ToolIndex);
				//读取相机像素大小
				memset(szValue,0,sizeof(szValue));
				GetPrivateProfileString(szName, "RealPerPix", "0.0", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
				if (atof(szValue)>0.0)
				{
					param->CameraPixSize[iCaremaIndex][AxiIndex][ToolIndex] = atof(szValue);
				}
				// 获取标定点数量,这个方法不好，采用下面矩阵的方法
				int CalPointNum = GetPrivateProfileInt(szName, "CalpointCount", 0, szCurrentPath);
				param->CalPointNum[iCaremaIndex][AxiIndex][ToolIndex] = CalPointNum;
				//读取标定的相机在机械系中的旋转矩阵,数据放入到视觉处理库，等待使用,标定相关的在走标定流程时才会保存
				int MecCamDataNum = 0;
				ReadMecAndCamRot(iCaremaIndex, AxiIndex,ToolIndex,param->CamInMecRotMetix[iCaremaIndex][AxiIndex][ToolIndex],MecCamDataNum);
				ReadCamRot(iCaremaIndex, AxiIndex,ToolIndex,param->CamRotMetix[iCaremaIndex][AxiIndex][ToolIndex],MecCamDataNum);
				ReadImageMetrix(iCaremaIndex,AxiIndex,ToolIndex,param->ImageRelative[iCaremaIndex][AxiIndex][ToolIndex],
					       param->ImageAbsolute[iCaremaIndex][AxiIndex][ToolIndex],MecCamDataNum);

				//读取相机标定时移动的相对距离
				for (int j = 0;j<CalPointNum;j++)
				{
					memset(szValue,0,sizeof(szValue));
					memset(szBuf,0,sizeof(szBuf));
					sprintf_s(szBuf,"CalPointX%d",j);
					GetPrivateProfileString(szName, szBuf, "0.0", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
					param->CalMotorX[iCaremaIndex][AxiIndex][ToolIndex][j] = atof(szValue);

					memset(szValue,0,sizeof(szValue));
					memset(szBuf,0,sizeof(szBuf));
					sprintf_s(szBuf,"CalPointY%d",j);
					GetPrivateProfileString(szName,szBuf, "0.0", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
					param->CalMotorY[iCaremaIndex][AxiIndex][ToolIndex][j] = atof(szValue);

					memset(szValue,0,sizeof(szValue));
					memset(szBuf,0,sizeof(szBuf));
					sprintf_s(szBuf,"CalPointPhi%d",j);
					GetPrivateProfileString(szName, szBuf, "0.0", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
					param->CalMotorPhi[iCaremaIndex][AxiIndex][ToolIndex][j] = atof(szValue);
				}
				memset(szValue,0,sizeof(szValue));
				GetPrivateProfileString(szName, WITH_CAMPARAM, "0", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
				if (0 == atoi(szValue))
				{
					param->CalWithCamParam[iCaremaIndex][AxiIndex][ToolIndex] = false;

				}else 
				{
					param->CalWithCamParam[iCaremaIndex][AxiIndex][ToolIndex] = true;
				}
				memset(szValue,0,sizeof(szValue));
				param->CalThickNess[iCaremaIndex][AxiIndex][ToolIndex] = GetPrivateProfileDouble(szName,CALBOARD_THICKNESS,0.0,szCurrentPath);		
			}
		}
	}
	//读取标定设定
	for (int iCaremaIndex = 0; iCaremaIndex < param->iCameraNum; iCaremaIndex++)
	{
		memset(szName, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szName, "Camera%d_Param",iCaremaIndex);
		param->CamFixedType[iCaremaIndex] = GetPrivateProfileInt(szName,"FixWay" ,-1, szCurrentPath);
		for (int AxisIndex = 0;AxisIndex < param->iAxisNum ;AxisIndex++)
		{
			for (int ToolIndex = 0;ToolIndex < param->iToolNum ; ToolIndex++)
			{
				memset(szName, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szName, "CamCal%d_Axe%d_Tool%d",iCaremaIndex,AxisIndex,ToolIndex);
				//轴的旋转方向,默认为1
				int RotDirect = GetPrivateProfileIntA(szName,"RotDirect",1, szCurrentPath);
				if (0 == RotDirect)
				{
					param->AxeRotDirect[iCaremaIndex][AxisIndex][ToolIndex] = false;
				}else
				{
					param->AxeRotDirect[iCaremaIndex][AxisIndex][ToolIndex] = true;
				}			
				//标定时的机械初始坐标
				param->PicCenterPointX[iCaremaIndex][AxisIndex][ToolIndex] =
					GetPrivateProfileDouble(szName,"PicCenterPointX" ,0.0, szCurrentPath);
				param->PicCenterPointY[iCaremaIndex][AxisIndex][ToolIndex]=
					GetPrivateProfileDouble(szName,"PicCenterPointY" ,0.0, szCurrentPath);

				param->ToolCalOriX[iCaremaIndex][AxisIndex][ToolIndex] =
					GetPrivateProfileDouble(szName,"CalToolOriX" ,0.0, szCurrentPath);
				param->ToolCalOriY[iCaremaIndex][AxisIndex][ToolIndex]=
				    GetPrivateProfileDouble(szName,"CalToolOriY" ,0.0, szCurrentPath);
				param->ToolCalOriPhi[iCaremaIndex][AxisIndex][ToolIndex]=
					 GetPrivateProfileDouble(szName,"CalToolOriPhi" ,0.0, szCurrentPath);
				param->CamCalOriX[iCaremaIndex][AxisIndex][ToolIndex] = 
					GetPrivateProfileDouble(szName,"CalCamOriX" ,0.0, szCurrentPath);			
				param->CamCalOriY[iCaremaIndex][AxisIndex][ToolIndex] = 
					GetPrivateProfileDouble(szName,"CalCamOriY" ,0.0, szCurrentPath);
				param->CamCalOriPhi[iCaremaIndex][AxisIndex][ToolIndex] = 
					GetPrivateProfileDouble(szName,"CamCalOriPhi" ,0.0, szCurrentPath);
				param->PosMoveType[iCaremaIndex][AxisIndex][ToolIndex] =
					GetPrivateProfileIntA(szName,"PosMoveType" ,0, szCurrentPath);//移动类型
				//读取相机标定时，旋转轴在相机中的坐标（位置）。
				param->RotpointToCamX[iCaremaIndex][AxisIndex][ToolIndex] = 
					        GetPrivateProfileDouble(szName,"RotpointToCamX",0.0,szCurrentPath);
				param->RotpointToCamY[iCaremaIndex][AxisIndex][ToolIndex] = 
					        GetPrivateProfileDouble(szName,"RotpointToCamY",0.0,szCurrentPath);
				//读取相机内外参数
				for (int CamParamValueIndex =0;CamParamValueIndex<12;CamParamValueIndex++)
				{
					memset(szValue, 0, CONFIG_VALUE_LENGTH);
					sprintf_s(szValue,"CamParam[%d]",CamParamValueIndex);//相机内参
					param->CamParam[iCaremaIndex][AxisIndex][ToolIndex][CamParamValueIndex] =
						  GetPrivateProfileDouble(szName,szValue,0.0,szCurrentPath);

				}
				for (int CamPosValueIndex =0;CamPosValueIndex<6;CamPosValueIndex++)
				{
					memset(szValue, 0, CONFIG_VALUE_LENGTH);
					sprintf_s(szValue,"CamPose[%d]",CamPosValueIndex);//Pose 相机外参
					param->CamPose[iCaremaIndex][AxisIndex][ToolIndex][CamPosValueIndex] =
						GetPrivateProfileDouble(szName,szValue,0.0,szCurrentPath);
				}
			}
		}	
	}
	//转轴和夹具间的关系 暂时没用到 待删除gpzdelete 
	for (int AxisIndex = 0 ;AxisIndex <param->iAxisNum;AxisIndex++)
	{
		for (int ToolIndex = 0;ToolIndex<param->iToolNum;ToolIndex++)
		{
			memset(szBuf, 0, CONFIG_VALUE_LENGTH);
			sprintf_s(szBuf,"Axis%d_Tool%d",AxisIndex,ToolIndex);
			param->RotX[AxisIndex][ToolIndex] = GetPrivateProfileDouble(szBuf,"RotX",0.0,szCurrentPath);
			param->RotY[AxisIndex][ToolIndex] = GetPrivateProfileDouble(szBuf,"RotY",0.0,szCurrentPath);
			param->RotX_Adjust[AxisIndex][ToolIndex] = GetPrivateProfileDouble(szBuf,"RotX_Adjust",0.0,szCurrentPath);
			param->RotY_Adjust[AxisIndex][ToolIndex] = GetPrivateProfileDouble(szBuf,"RotY_Adjust",0.0,szCurrentPath);
		}
	}
	//产品映射:a,b两种产品,但是模板不一样,在切换产品时会建立新的场景,此时控制平台端发送过来的是同样的场景,这边再内部映射好
	ReadProductMap(&(param->ProductMap));
	ReadDefaultProduct(param->CurProductName);
	//网络通信
    memset(szName, 0, CONFIG_VALUE_LENGTH);
    GetPrivateProfileString("communication", "serverip", "127.0.0.1", szName, CONFIG_VALUE_LENGTH, szCurrentPath);
    param->stSocketParam.strServerIPAdd = szName;
    param->stSocketParam.ServerPort = GetPrivateProfileInt("communication", "port", 0, szCurrentPath);
    memset(szName, 0, CONFIG_VALUE_LENGTH);
    GetPrivateProfileString("communication", "clientip", "127.0.0.1", szName, CONFIG_VALUE_LENGTH, szCurrentPath);
    param->stSocketParam.strClientIPAdd = szName;
    param->stSocketParam.ClientPort = GetPrivateProfileInt("communication", "clientport", 41013, szCurrentPath);
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileString("communication", "CalRevHead", "CB", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
	param->stSocketParam.CalRevHead = szValue;
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileString("communication", "CalSendHead", "RGR", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
	param->stSocketParam.CalSendHead = szValue;
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileString("communication", "WorkRevHead", "GR", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
	param->stSocketParam.WorkRevHead = szValue;
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileString("communication", "WorkSendHead", "RGR", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
	param->stSocketParam.WorkSendHead = szValue;
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileString("communication", "DividLabel", ",", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
	param->stSocketParam.DividLabel = szValue;
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileString("communication", "EndLabel", ";", szValue, CONFIG_VALUE_LENGTH, szCurrentPath);
	param->stSocketParam.EndLabel = szValue;
	//其他
    memset(szName, 0, CONFIG_VALUE_LENGTH);
    GetPrivateProfileString("ispath", "ISPath", isrPath, szName, CONFIG_VALUE_LENGTH, szCurrentPath);
    strcpy_s(param->ImageSavePath, 256, szName);
    return true;
}

const char* CConfigManager::GetSysConfigPath(void)
{
	return m_SysconfigPath;
}

bool CConfigManager::SaveConfig(void *pData)
{
	if (NULL == pData)
	{
		return false;
	}
    char szCurrentPath[512] = {0};
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szName[CONFIG_VALUE_LENGTH] = {0};
	char szBuf[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	SYSTEM_PARAM *param = (SYSTEM_PARAM *)pData;
    WritePrivateProfileInt("install", "install", param->iIsInstall, szCurrentPath);
	WritePrivateProfileInt("window", "num", param->iWindowNum, szCurrentPath);
	WritePrivateProfileInt("scene", "num",param->iSceneNum, szCurrentPath);

	if (param->IsSimulate)
	{
		WritePrivateProfileInt("WorkMode", "Simulate", 1, szCurrentPath);
	}else
	{
		WritePrivateProfileInt("WorkMode", "Simulate", 0, szCurrentPath);

	}

	if (0 < param->iSceneNum)
	{
		for (int i = 0; i < param->iSceneNum; i++)
		{
			memset(szBuf, 0, CONFIG_VALUE_LENGTH);
			sprintf_s(szBuf, "scene%d", i);
			WritePrivateProfileString("scene",szBuf ,param->pstrSceneName[i].c_str() , szCurrentPath);
		}
    }
    WritePrivateProfileInt("light","lightnum" ,param->iLightNum, szCurrentPath);
	WritePrivateProfileInt("Axis","Axisnum" ,param->iAxisNum, szCurrentPath);
    WritePrivateProfileInt("light","autool" ,param->iAutoOL, szCurrentPath);
    WritePrivateProfileInt("light","lighttypenum" ,param->iLightTypeNum, szCurrentPath);

    //保存光源类型及对应参数
    for (int i = 0; i < param->iLightTypeNum; i++)
    {
        memset(szBuf, 0, CONFIG_VALUE_LENGTH);
        sprintf_s(szBuf, "lighttype%d", i);
        WritePrivateProfileInt(szBuf, "baudrate", param->strLightType[i].BaudRate, szCurrentPath);
        WritePrivateProfileInt(szBuf, "parity", param->strLightType[i].Parity, szCurrentPath);
        WritePrivateProfileInt(szBuf, "bytesize", param->strLightType[i].ByteSize, szCurrentPath);
        WritePrivateProfileInt(szBuf, "stopbits", param->strLightType[i].StopBits, szCurrentPath);
        WritePrivateProfileString(szBuf,"name" ,param->strLightType[i].TypeName.c_str(), szCurrentPath);
    }

    if (0 < param->iLightNum)
    {
        for (int i = 0; i < param->iLightNum; i++)
        {
            memset(szBuf, 0, CONFIG_VALUE_LENGTH);
            sprintf_s(szBuf, "light%d", i);
            WritePrivateProfileInt(szBuf, "lighttype", param->strLightInfo[i].strLightType, szCurrentPath);
            WritePrivateProfileInt(szBuf, "lightchanum", param->strLightInfo[i].Channel, szCurrentPath);
            WritePrivateProfileString(szBuf,"commname" ,param->strLightInfo[i].PortName.c_str(), szCurrentPath);
        }
    }

	WritePrivateProfileInt("camera","cameranum" ,param->iCameraNum, szCurrentPath);

	for (int iCaremaIndex = 0; iCaremaIndex < param->iCameraNum; iCaremaIndex++)
	{
		memset(szBuf, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf, "camera%d",iCaremaIndex);
        WritePrivateProfileString("camera",szBuf ,param->pstrCamareGUID[iCaremaIndex].c_str(), szCurrentPath);
	}
	//保存标定设定
	for (int iCaremaIndex = 0; iCaremaIndex < param->iCameraNum; iCaremaIndex++)
	{
		memset(szName, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szName, "Camera%d_Param",iCaremaIndex);
		memset(szBuf, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf, "%d",param->CamFixedType[iCaremaIndex]);	
		WritePrivateProfileString(szName,"FixWay" ,szBuf, szCurrentPath);
		for (int AxisIndex = 0;AxisIndex < param->iAxisNum ;AxisIndex++)
		{
			for (int ToolIndex = 0;ToolIndex < param->iToolNum ; ToolIndex++)
			{
				memset(szName, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szName, "CamCal%d_Axe%d_Tool%d",iCaremaIndex,AxisIndex,ToolIndex);
				//轴的旋转方向
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%d",param->AxeRotDirect[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"RotDirect",szBuf,szCurrentPath);

				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->PicCenterPointX[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"PicCenterPointX" ,szBuf, szCurrentPath);
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->PicCenterPointY[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"PicCenterPointY" ,szBuf, szCurrentPath);

				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->ToolCalOriX[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"CalToolOriX" ,szBuf, szCurrentPath);
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->ToolCalOriY[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"CalToolOriY" ,szBuf, szCurrentPath);


				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->ToolCalOriPhi[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"CalToolOriPhi" ,szBuf, szCurrentPath);

				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->CamCalOriX[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"CalCamOriX" ,szBuf, szCurrentPath);
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->CamCalOriY[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"CalCamOriY" ,szBuf, szCurrentPath);
				//CamCalOriPhi
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->CamCalOriPhi[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"CamCalOriPhi" ,szBuf, szCurrentPath);

				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%d",param->PosMoveType[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"PosMoveType" ,szBuf, szCurrentPath);

				//RotpointToCamX
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->RotpointToCamX[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"RotpointToCamX" ,szBuf, szCurrentPath);

				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->RotpointToCamY[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,"RotpointToCamY" ,szBuf, szCurrentPath);

				//保持九点标定运动参数
				int CalPointNum = param->CalPointNum[iCaremaIndex][AxisIndex][ToolIndex];

				for (int MoveDataIndex = 0;MoveDataIndex<CalPointNum;MoveDataIndex++)
				{
					memset(szBuf, 0, CONFIG_VALUE_LENGTH);
					memset(szValue, 0, CONFIG_VALUE_LENGTH);
					sprintf_s(szBuf,"CalPointX%d",MoveDataIndex);			
					sprintf_s(szValue,"%0.12lf",param->CalMotorX[iCaremaIndex][AxisIndex][ToolIndex][MoveDataIndex]);
					WritePrivateProfileString(szName,szBuf,szValue, szCurrentPath);
					memset(szBuf, 0, CONFIG_VALUE_LENGTH);
					memset(szValue, 0, CONFIG_VALUE_LENGTH);
					sprintf_s(szBuf,"CalPointY%d",MoveDataIndex);
					sprintf_s(szValue,"%0.12lf",param->CalMotorY[iCaremaIndex][AxisIndex][ToolIndex][MoveDataIndex]);
					WritePrivateProfileString(szName,szBuf,szValue, szCurrentPath);
					memset(szBuf, 0, CONFIG_VALUE_LENGTH);
					memset(szValue, 0, CONFIG_VALUE_LENGTH);
					sprintf_s(szBuf,"CalPointPhi%d",MoveDataIndex);
					sprintf_s(szValue,"%0.12lf",param->CalMotorPhi[iCaremaIndex][AxisIndex][ToolIndex][MoveDataIndex]);
					WritePrivateProfileString(szName,szBuf ,szValue, szCurrentPath);

					//sprintf_s(szBuf,"CalPointY%d",j);
					//sprintf_s(szBuf,"CalPointPhi%d",j);

				}
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%d",CalPointNum);	
				WritePrivateProfileString(szName,"CalpointCount" ,szBuf, szCurrentPath);




				//标定板标定参数
				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				if (param->CalWithCamParam[iCaremaIndex][AxisIndex][ToolIndex])
				{
					sprintf_s(szBuf, "1");	

				}else
				{
					sprintf_s(szBuf, "0");	
				}
				WritePrivateProfileString(szName,WITH_CAMPARAM,szBuf, szCurrentPath);

				memset(szBuf, 0, CONFIG_VALUE_LENGTH);
				sprintf_s(szBuf, "%0.5lf",param->CalThickNess[iCaremaIndex][AxisIndex][ToolIndex]);	
				WritePrivateProfileString(szName,CALBOARD_THICKNESS,szBuf, szCurrentPath);



				//保存相机内外参数
				for (int CamParamValueIndex =0;CamParamValueIndex<12;CamParamValueIndex++)
				{
					memset(szBuf, 0, CONFIG_VALUE_LENGTH);
					sprintf_s(szBuf,"CamParam[%d]",CamParamValueIndex);
					WritePrivateProfileDouble(szName,szBuf,param->CamParam[iCaremaIndex][AxisIndex][ToolIndex][CamParamValueIndex]
						      ,szCurrentPath);

				}
				for (int CamPosValueIndex =0;CamPosValueIndex<6;CamPosValueIndex++)
				{
					memset(szBuf, 0, CONFIG_VALUE_LENGTH);
					sprintf_s(szBuf,"CamPose[%d]",CamPosValueIndex);
					WritePrivateProfileDouble(szName,szBuf,param->CamPose[iCaremaIndex][AxisIndex][ToolIndex][CamPosValueIndex]
					         ,szCurrentPath);

				}

			}
		}	
	}




	WritePrivateProfileString("communication","serverip",param->stSocketParam.strServerIPAdd.c_str(), szCurrentPath);   
	WritePrivateProfileInt("communication","port",param->stSocketParam.ServerPort, szCurrentPath);
	WritePrivateProfileString("communication","clientip",param->stSocketParam.strClientIPAdd.c_str(), szCurrentPath);   
	WritePrivateProfileInt("communication","clientport",param->stSocketParam.ClientPort, szCurrentPath);
    WritePrivateProfileString("ispath","ISPath",param->ImageSavePath, szCurrentPath); 
	

    return true;
}

bool CConfigManager::SaveCamCalibrate(int CamerIndex,int ToolIndex,int DataCount,double MotorX[],double MotorY[],double PixRow[],double PixCol[])
{
	char szCurrentPath[512] = {0};
	 //取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szBuf1, "CamCal%d_Tool%d",CamerIndex,ToolIndex); 
	sprintf_s(szValue, "%d",DataCount); 
	WritePrivateProfileString(szBuf1,"DataCount",szValue, szCurrentPath);

	for (int i = 0;i<DataCount;i++)
	{
		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "MotorX_%d",i); 
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szValue, "%.6lf",MotorX[i]); 
	    WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);

		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "MotorY_%d",i); 
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szValue, "%.6lf",MotorY[i]); 
		WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);

		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "PixRow_%d",i); 
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szValue, "%.2lf",PixRow[i]); 
		WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);

		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "PixCol_%d",i); 
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szValue, "%.2lf",PixCol[i]); 
		WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);
	}
	return true;
}

bool CConfigManager::SaveMecAndCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pCamRot),
	double (*pMecAndCam),double (*pImageRela),double (*pImagAbs),int DataNum)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	char szValue1[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szBuf1, "CamCal%d_Axe%d_Tool%d",CamIndex,AxeIndex,ToolIndex); 
	for (int i = 0;i<DataNum;i++)
	{
		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		memset(szValue1, 0, CONFIG_VALUE_LENGTH);

		sprintf_s(szValue1, "%d",i+1);
	    WritePrivateProfileString(szBuf1,"CamRotInMecNum",szValue1, szCurrentPath);
		WritePrivateProfileString(szBuf1,"CamRotNum",szValue1, szCurrentPath);
		WritePrivateProfileString(szBuf1,"ImageRelativeNum",szValue1, szCurrentPath);
		WritePrivateProfileString(szBuf1,"ImageAbsoluteNum",szValue1, szCurrentPath);

		//相机坐标系下，计算绝对坐标的矩阵，
		sprintf_s(szBuf2, "CamRotInMec_%d",i);
		sprintf_s(szValue, "%.12lf",pMecAndCam[i]);
		WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);

		//相机坐标系下，计算相对坐标的矩阵
		sprintf_s(szBuf2, "CamRot_%d",i);
		sprintf_s(szValue, "%.12lf",pCamRot[i]);
		WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);

		//图像坐标系下，计算相对坐标的矩阵
		sprintf_s(szBuf2, "ImageRelative_%d",i);
		sprintf_s(szValue, "%.12lf",pImageRela[i]);
		WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);

		//图像坐标系下，计算绝对坐标的矩阵
		sprintf_s(szBuf2, "ImageAbsolute_%d",i);
		sprintf_s(szValue, "%.12lf",pImagAbs[i]);
		WritePrivateProfileString(szBuf1,szBuf2,szValue, szCurrentPath);

	}

	return true;
	                                                                      
}
bool CConfigManager::SavePicCenter(int CamIndex,int AxeIndex,int ToolIndex,double X,double Y)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);

	sprintf_s(szBuf1, "CamCal%d_Axe%d_Tool%d",CamIndex,AxeIndex,ToolIndex); 
	for (int i = 0;i<1;i++)
	{
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szValue, "%.4lf",X);
		WritePrivateProfileString(szBuf1,"PicCenterPointX",szValue, szCurrentPath);
		sprintf_s(szValue, "%.4lf",Y);
		WritePrivateProfileString(szBuf1,"PicCenterPointY",szValue, szCurrentPath);
	}

	return true;

}


bool CConfigManager::ReadMecAndCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pMecAndCam),int& DataNum)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	sprintf_s(szBuf1, "CamCal%d_Axe%d_Tool%d",CamIndex,AxeIndex,ToolIndex);                                         
	DataNum = GetPrivateProfileInt(szBuf1, "CamRotInMecNum", 0, szCurrentPath);
	pMecAndCam[0] = DataNum;
	for (int i = 0;i<DataNum;i++)
	{
		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "CamRotInMec_%d",i);
		pMecAndCam[i+1] = GetPrivateProfileDouble(szBuf1, szBuf2, 0.0, szCurrentPath);
	}
	return true;
}

bool CConfigManager::ReadCamRot(int CamIndex,int AxeIndex,int ToolIndex,double (*pCamRot),int& DataNum)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	sprintf_s(szBuf1, "CamCal%d_Axe%d_Tool%d",CamIndex,AxeIndex,ToolIndex);
	DataNum = GetPrivateProfileInt(szBuf1, "CamRotNum", 0, szCurrentPath);
	pCamRot[0] = DataNum;
	for (int i = 0;i<DataNum;i++)
	{
		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "CamRot_%d",i);
		pCamRot[i+1] = GetPrivateProfileDouble(szBuf1, szBuf2, 0.0, szCurrentPath);
	}
	return true;
}

bool CConfigManager::ReadImageMetrix(int CamIndex,int AxeIndex,int ToolIndex,double (*pImageRela),
	              double (*pImageAbs),int& DataNum)
{

	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	sprintf_s(szBuf1, "CamCal%d_Axe%d_Tool%d",CamIndex,AxeIndex,ToolIndex);
	DataNum = GetPrivateProfileInt(szBuf1, "ImageRelativeNum", 0, szCurrentPath);
	pImageRela[0] = DataNum;
	for (int i = 0;i<DataNum;i++)
	{
		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "ImageRelative_%d",i);
		pImageRela[i+1] = GetPrivateProfileDouble(szBuf1, szBuf2, 0.0, szCurrentPath);
	}

	DataNum = GetPrivateProfileInt(szBuf1, "ImageAbsoluteNum", 0, szCurrentPath);
	pImageAbs[0] = DataNum;
	for (int i = 0;i<DataNum;i++)
	{
		memset(szBuf2, 0, CONFIG_VALUE_LENGTH);
		memset(szValue, 0, CONFIG_VALUE_LENGTH);
		sprintf_s(szBuf2, "ImageAbsolute_%d",i);
		pImageAbs[i+1] = GetPrivateProfileDouble(szBuf1, szBuf2, 0.0, szCurrentPath);
	}
	return true;

}






bool  CConfigManager::SaveSysRotAxleToCam(int CamIndex,int AxisIndex, int ToolIndex,double RotX,double RotY)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szBuf1, "CamCal%d_Axe%d_Tool%d",CamIndex,AxisIndex,ToolIndex); 
	sprintf_s(szValue, "%.6lf",RotX); 
	WritePrivateProfileString(szBuf1,"RotpointToCamX",szValue, szCurrentPath);
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue, "%.6lf",RotY);
    WritePrivateProfileString(szBuf1,"RotpointToCamY",szValue, szCurrentPath);



	return true;
}

bool CConfigManager::SaveSysCamPixSize(int CamIndex,int AxeIndex,int ToolIndex,double PixSize)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szBuf1, "CamCal%d_Axe%d_Tool%d",CamIndex,AxeIndex,ToolIndex); 
	sprintf_s(szValue, "%.8lf",PixSize); 
	WritePrivateProfileString(szBuf1,"RealPerPix",szValue, szCurrentPath);

	return true;

}

bool  CConfigManager::SaveSysRotAxleToTool(int AxisIndex, int ToolIndex,double RotX,double RotY)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	//char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szBuf1, "Axis%d_Tool%d",AxisIndex,ToolIndex); 
	sprintf_s(szValue, "%.8lf",RotX); 
	WritePrivateProfileString(szBuf1,"RotX",szValue, szCurrentPath);

	memset(szValue, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szValue, "%.8lf",RotY); 
	WritePrivateProfileString(szBuf1,"RotY",szValue, szCurrentPath);
	return true;
}

void CConfigManager::SaveSysRotAxleAdjust(int AxisIndex, int ToolIndex,double RotXAdjust,double RotYAdjust)
{
	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	//char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	char szValue[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szBuf1, "Axis%d_Tool%d",AxisIndex,ToolIndex); 
	sprintf_s(szValue, "%.8lf",RotXAdjust); 
	WritePrivateProfileString(szBuf1,"RotX_Adjust",szValue, szCurrentPath);

	memset(szValue, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szValue, "%.8lf",RotYAdjust); 
	WritePrivateProfileString(szBuf1,"RotY_Adjust",szValue, szCurrentPath);


}

void  CConfigManager::ReadSysRotAxleAdjust(int AxisIndex, int ToolIndex,double& RotXAdjust,double& RotYAdjust)
{

	char szCurrentPath[512] = {0};
	//取得config.ini的地址
	memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	//char szValue[CONFIG_VALUE_LENGTH] = {0};
	//memset(szBuf, 0, CONFIG_VALUE_LENGTH);
	sprintf_s(szBuf1, "Axis%d_Tool%d",AxisIndex,ToolIndex); 

	RotXAdjust = GetPrivateProfileDouble(szBuf1,"RotX_Adjust",0.0,szCurrentPath); 
 
	RotYAdjust = GetPrivateProfileDouble(szBuf1,"RotY_Adjust",0.0, szCurrentPath);


}




 void CConfigManager::SaveProctocolData(const PROCTOCOLDATA& ProctoData)
 {
	 char szCurrentPath[512] = {0};
	 //取得config.ini的地址
	 memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	 char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	 //char szBuf2[CONFIG_VALUE_LENGTH] = {0};
	 char szValue[CONFIG_VALUE_LENGTH] = {0};
	 //memset(szBuf, 0, CONFIG_VALUE_LENGTH);	

	 if(ProctoData.EnableData[0])
	 {
		 sprintf_s(szValue, "%s",ProctoData.CalRevHead); 
		 WritePrivateProfileString("communication","CalRevHead",szValue, szCurrentPath);
	 }
	 if(ProctoData.EnableData[1])
	 {

		 memset(szValue, 0, CONFIG_VALUE_LENGTH);	
		 sprintf_s(szValue, "%s",ProctoData.WorkRevHead); 
		 WritePrivateProfileString("communication","WorkRevHead",szValue, szCurrentPath);
	 }
	 if(ProctoData.EnableData[2])
	 {
		 memset(szValue, 0, CONFIG_VALUE_LENGTH);	
		 sprintf_s(szValue, "%s",ProctoData.CalSendHead); 
		 WritePrivateProfileString("communication","CalSendHead",szValue, szCurrentPath);
	 }
	 if(ProctoData.EnableData[3])
	 {
		 memset(szValue, 0, CONFIG_VALUE_LENGTH);	
		 sprintf_s(szValue, "%s",ProctoData.WorkSendHead); 
		 WritePrivateProfileString("communication","WorkSendHead",szValue, szCurrentPath);
	 }
	 if(ProctoData.EnableData[4])
	 {
		 memset(szValue, 0, CONFIG_VALUE_LENGTH);	
		 sprintf_s(szValue, "%s",ProctoData.IPDivid); 
		 WritePrivateProfileString("communication","DividLabel",szValue, szCurrentPath);
	 }
	 if(ProctoData.EnableData[5])
	 {
		 memset(szValue, 0, CONFIG_VALUE_LENGTH);	
		 sprintf_s(szValue, "%s",ProctoData.IPEnd); 
		 WritePrivateProfileString("communication","EndLabel",szValue, szCurrentPath);
	 }
 }

 void CConfigManager::SaveProductMap(void* pMapData)
 {

	  LPPMAPDATA pProductMap = static_cast<LPPMAPDATA>(pMapData);
	  char szCurrentPath[512] = {0};
	  //取得config.ini的地址
	  memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	  char szBuf1[CONFIG_VALUE_LENGTH] = {0};
	  char szValue[CONFIG_VALUE_LENGTH] = {0};

	  char szSubBuf[CONFIG_VALUE_LENGTH] = {0};
	  char szSubValue[CONFIG_VALUE_LENGTH] = {0};

	  PMAPDATA::iterator it;
	  map<int,int>::iterator subit;
	  int i= 0;
	  
	  for (it = pProductMap->begin();it != pProductMap->end();it++,i++ )
	  {
		  //保存产品数量
		  memset(szBuf1, 0, CONFIG_VALUE_LENGTH);	
		  memset(szValue, 0, CONFIG_VALUE_LENGTH);
		  sprintf_s(szValue, "%d",i+1); 
		  WritePrivateProfileString("SceneMapProduct","ProductCount",szValue, szCurrentPath);
		  //保存产品的场景号映射
		  memset(szBuf1, 0, CONFIG_VALUE_LENGTH);	
		  memset(szValue, 0, CONFIG_VALUE_LENGTH);
		  sprintf_s(szBuf1, "p_%d",i); 
		  sprintf_s(szValue, "%s",it->first.c_str()); 
		  WritePrivateProfileString("SceneMapProduct",szBuf1,szValue, szCurrentPath);
		  subit = (it->second).begin();
		  for ( int j = 1;subit !=(it->second).end();subit++,j++)
		  {
			  //保存每个产品映射的场景号数量
			  memset(szSubBuf, 0, CONFIG_VALUE_LENGTH);	
			  memset(szSubValue, 0, CONFIG_VALUE_LENGTH);
			  sprintf_s(szSubBuf, "p_%d_MapCount",i); 
			  sprintf_s(szSubValue, "%d",j);
			  WritePrivateProfileString("SceneMapProduct",szSubBuf,szSubValue, szCurrentPath);

			  //保存场景号的具体映射
			  memset(szSubBuf, 0, CONFIG_VALUE_LENGTH);	
			  memset(szSubValue, 0, CONFIG_VALUE_LENGTH);
			  sprintf_s(szSubBuf, "p_%d_scene%d",i,subit->first); 
			  sprintf_s(szSubValue, "%d",subit->second);
			  WritePrivateProfileString("SceneMapProduct",szSubBuf,szSubValue, szCurrentPath);
		  }
	  }


 }


 void CConfigManager::ReadProductMap(void* pMapData)
 {
	 LPPMAPDATA pProductMap = static_cast<LPPMAPDATA>(pMapData);
	 SCENEMAP  SceneNoMap;
	 string szName;
	 char szCurrentPath[512] = {0};
	 //取得config.ini的地址
	 memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	 char szBuf[CONFIG_VALUE_LENGTH] = {0};
	 char szValue[CONFIG_VALUE_LENGTH] = {0};

	 char szSubBuf[CONFIG_VALUE_LENGTH] = {0};
	 char szSubValue[CONFIG_VALUE_LENGTH] = {0};
	 int ProductNum = GetPrivateProfileInt("SceneMapProduct", "ProductCount", -1, szCurrentPath);
	 for (int i = 0;i<ProductNum;i++)
	 {
		 //获取产品名
		 memset(szBuf, 0, CONFIG_VALUE_LENGTH);	
		 memset(szValue, 0, CONFIG_VALUE_LENGTH);
		 sprintf_s(szBuf, "p_%d",i); 
		 GetPrivateProfileString("SceneMapProduct",szBuf, "-1", szValue, sizeof(szValue), szCurrentPath);
		 szName = szValue;

		 //获取每个产品的映射数量
		 memset(szSubBuf, 0, CONFIG_VALUE_LENGTH);
		 sprintf_s(szSubBuf, "p_%d_MapCount",i); 
		  int SceneMapNum = GetPrivateProfileInt("SceneMapProduct", szSubBuf, -1, szCurrentPath);
		 for (int j = 0;j<SceneMapNum;j++)
		 {
			  memset(szSubBuf, 0, CONFIG_VALUE_LENGTH);
			  sprintf_s(szSubBuf, "p_%d_scene%d",i,j); 
			  int MapIndex = GetPrivateProfileInt("SceneMapProduct", szSubBuf, -1, szCurrentPath);
			  SceneNoMap.insert(make_pair(j,MapIndex));
		 }
	
		 pProductMap->insert( make_pair(szName,SceneNoMap));
		
		 //清空SceneNoMap，以便下一个产品使用。
		 SceneNoMap.clear();

	 }
 }

 void CConfigManager::SaveDefaultProduct(string productName)
 {
	 char szCurrentPath[512] = {0};
	 //取得config.ini的地址
	 memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	 //char szBuf[CONFIG_VALUE_LENGTH] = {0};
	// char szValue[CONFIG_VALUE_LENGTH] = {0};
	// sprintf_s(szBuf,"")

	 WritePrivateProfileString("SceneMapProduct","DefaultProduct",productName.c_str(), szCurrentPath);

 }

 void  CConfigManager::ReadDefaultProduct(string& productName)
 {
	 char szCurrentPath[512] = {0};
	 //取得config.ini的地址
	 memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	 //char szBuf[CONFIG_VALUE_LENGTH] = {0};
	 char szValue[CONFIG_VALUE_LENGTH] = {0};
	 GetPrivateProfileString("SceneMapProduct","DefaultProduct", "", szValue, sizeof(szValue), szCurrentPath);
	 productName = szValue;

 }


 //void CConfigManager::SaveProductId(int ProductId,string ProductName)
 //{
	// char szCurrentPath[512] = {0};
	// //取得config.ini的地址
	// memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	// char szBuf[CONFIG_VALUE_LENGTH] = {0};
	// char szValue[CONFIG_VALUE_LENGTH] = {0};
	// memset(szBuf, 0, CONFIG_VALUE_LENGTH);	
	// memset(szValue, 0, CONFIG_VALUE_LENGTH);
	// sprintf_s(szBuf, "pId_%d",ProductId); 
	// sprintf_s(szValue, "%s",ProductName.c_str()); 
	// WritePrivateProfileString("ProductIdMap",szBuf,szValue, szCurrentPath);
 //}
 
 //void CConfigManager::ReadProductId(void* lpProductMap)
 //{
	// char szCurrentPath[512] = {0};
	// //取得config.ini的地址
	// memcpy(szCurrentPath, GetSysConfigPath(),sizeof(szCurrentPath));     
	// char szBuf[CONFIG_VALUE_LENGTH] = {0};
	// char szValue[CONFIG_VALUE_LENGTH] = {0};
	// for (int i = 0;i<20;i++)
	// {
	//	 memset(szBuf, 0, CONFIG_VALUE_LENGTH);	
	//	 memset(szValue, 0, CONFIG_VALUE_LENGTH);
	//	 sprintf_s(szBuf, "pId_%d",i); 
	//	 GetPrivateProfileString("ProductIdMap",szBuf, "", szValue, sizeof(szValue), szCurrentPath);
	//	 ((LPPRODUCTIDMAP)lpProductMap)->insert(make_pair(i,szValue));
	// }
 //}


bool CConfigManager::ReadSceneProcProcess(const char *pPath, int& ProcProcess)
{
	 char szCurrentPath[512] = {0};
	get_module_path((LPSTR)szCurrentPath);  //取得config.ini的地址
	//strcat_s(szCurrentPath, "\\");
	strcat_s(szCurrentPath, pPath);
	strcat_s(szCurrentPath, "\\");
	strcat_s(szCurrentPath, CONFIG_FILE_NAME);
	if(-1 == access(szCurrentPath, 0))//ini文件不存在
	{
		return false;
	}
	ProcProcess =  GetPrivateProfileInt("scene", "ProcProcess", -1, szCurrentPath);

	return TRUE;

}

char* CConfigManager::GetSceneconfigPath(const char* SceneFloderName)
{
	char szBuf[64] = {0};
	char szName[64] = {0};
	char szCurrentPath[512] = {0};
	memset(m_CurSceneConfigPath,0,sizeof(m_CurSceneConfigPath));   
	get_module_path((LPSTR)m_CurSceneConfigPath);  //取得config.ini的地址
	//strcat_s(szCurrentPath, "\\");
	strcat_s(m_CurSceneConfigPath, SceneFloderName);
	strcat_s(m_CurSceneConfigPath, "\\");
	strcat_s(m_CurSceneConfigPath, CONFIG_FILE_NAME);

	if(-1 == access(m_CurSceneConfigPath, 0))//ini文件不存在
	{
		return NULL;
	}else
	{
		return m_CurSceneConfigPath;//szCurrentPath
	}
}


bool CConfigManager::ReadSceneConfig(const char *pPath, void *pData)
{
    SCENE_PKG *pkg = NULL;
    if (NULL == pData)
    {
        return false;
    }
    char szBuf[64] = {0};
    char szName[64] = {0};
    char szCurrentPath[512] = {0};

    get_module_path((LPSTR)szCurrentPath);  //取得config.ini的地址
    strcat_s(szCurrentPath, pPath);
    strcat_s(szCurrentPath, "\\");
    strcat_s(szCurrentPath, CONFIG_FILE_NAME);

    if(-1 == access(szCurrentPath, 0))//ini文件不存在
    {
        return false;
    }
    pkg = (SCENE_PKG *)pData;
    pkg->display = 0;
    int iAffineType = 0;
    //读取包头信息
    iAffineType = GetPrivateProfileInt("scene", "affine", 0, szCurrentPath);
    if (0 == iAffineType)
    {
        pkg->bSave = false;
    }
    else
    {
        pkg->bSave = true;
    }
    iAffineType = GetPrivateProfileInt("scene", "simulate", 0, szCurrentPath);
    if (1 != iAffineType)
    {
        pkg->bIsSimulate = false;
    }
    else
    {
        pkg->bIsSimulate = true;
    }

	// 添加机械手视觉定义编号
	pkg->MotorToolIndex =   GetPrivateProfileInt("scene", "MotorToolIndex", -1, szCurrentPath);
	pkg->CalPointNum =  GetPrivateProfileInt("scene", "CalPointNum", 0, szCurrentPath); //这个应该不需要
	pkg->AxisIndex = GetPrivateProfileInt("scene", "AxisIndex", -1, szCurrentPath);
	pkg->stationID = GetPrivateProfileInt("scene", "station", 0, szCurrentPath);
	pkg->imageCount = GetPrivateProfileInt("scene", "imagenum", 0, szCurrentPath);
	pkg->pkgBodySize = GetPrivateProfileInt("scene", "pkgSize", 0, szCurrentPath);
	pkg->pkgCount = GetPrivateProfileInt("scene", "pkgnum", 0, szCurrentPath);
	pkg->ModelCount = GetPrivateProfileInt("scene", "modelnum", 0, szCurrentPath);
	pkg->InterfaceDataNumb = GetPrivateProfileInt("scene", "InterfaceDataNumb", 0, szCurrentPath);

	//读取图像获取时设置参数
	for (int i = 0; i < pkg->imageCount; i++)
	{
		SCENE_DEVICE_INFO info;
		memset(szName, 0, 64);
		sprintf_s(szName, "image%d", i);
		info.CameraIndex = GetPrivateProfileInt(szName, "no", 0, szCurrentPath);
		info.LightIndex = GetPrivateProfileInt(szName, "light", 0, szCurrentPath);
		info.CameraShutter = GetPrivateProfileInt(szName, "shutter", 0, szCurrentPath);
		info.CameraGain = GetPrivateProfileInt(szName, "gain", 8, szCurrentPath);
		pkg->vecInfo.push_back(info);
	}
	if (pkg->imageCount != pkg->vecInfo.size())
	{
		pkg->imageCount = (int)pkg->vecInfo.size();
	}
    pkg->AxisIndex = GetPrivateProfileInt("scene", "AxisIndex", -1, szCurrentPath);
	pkg->ProcProcess = GetPrivateProfileInt("scene", "ProcProcess", -1, szCurrentPath);  //新方法
	pkg->SceneType = GetPrivateProfileInt("scene", "sceneType", -1, szCurrentPath); //标定还是测量
	char szValue[256] = {0};
	pkg->defcamwidth  = 0;  //图像默认尺寸
	pkg->defcamheight = 0;
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA("scene","defcamwidth","2552",szValue,sizeof(szValue),szCurrentPath);
	pkg->defcamwidth = atof(szValue);
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA("scene","defcamheight","1944",szValue,sizeof(szValue),szCurrentPath);
	pkg->defcamheight = atof(szValue);

	 pkg->testcount = GetPrivateProfileInt("scene", "testcount", 0, szCurrentPath);
	
	//如果是采用C++封装的固定方式( ProcProcess=0) ，则需要根据配置文件生成相应的场景对象，完成后续参数的设定，
	if (0 == pkg->ProcProcess)  //新的方法
	{

	} 
	if(1)  //之前的处理方法
	{
		//读取包数据内容
		pkg->vecBody.reserve(pkg->pkgCount);
		for (int i = 0; i < pkg->pkgCount; i++)
		{
			int iType = 0;
			int iSize = 0;
			int index = 0;
			//pkgType = 类型*100 + 索引
			memset(szBuf, 0, 64);
			sprintf_s(szBuf, "param%d", i);
			iType = GetPrivateProfileInt(szBuf, "pkgType", 0, szCurrentPath);
			iSize = GetPrivateProfileInt(szBuf, "pkgSize", 0, szCurrentPath);

			VIRSION_DATA_PKG *pBody = new VIRSION_DATA_PKG[iSize + 2];
			unsigned int *pDataType = new unsigned int[iSize + 2];
			pDataType[index] = 0;
			pBody[index].iValue = iType;
			index++;
			pDataType[index] = 0;
			pBody[index].iValue = iSize;
			index++;
			for (int j = 0; j < iSize; j++)
			{
				memset(szName, 0, 64);
				sprintf_s(szName, "type%d", index);
				pDataType[index] = GetPrivateProfileInt(szBuf, szName, 0, szCurrentPath);
				memset(szName, 0, 64);
				sprintf_s(szName, "data%d", index);
				int iT = pDataType[index]%100;
				if (ID_TYPE == pDataType[index]/100)
				{
					pBody[index].iValue = -1;
				}
				else
				{
					switch(iT)
					{
					case 0:  //int
						{
							pBody[index].iValue = GetPrivateProfileInt(szBuf, szName, 0, szCurrentPath);
						}
						break;
					case 1:  //double
						{
							pBody[index].dValue = GetPrivateProfileDouble(szBuf, szName, 0.0, szCurrentPath);
						}
						break;
					default:  //string
						{
						}
						break;
					}

				}
				index++;
			}
			pkg->vecBody.push_back(pBody);
			pkg->vecDataType.push_back(pDataType);
		}
		if (pkg->vecBody.size() != pkg->pkgCount || pkg->vecDataType.size() != pkg->pkgCount)
		{
			pkg->vecBody.size() > pkg->vecDataType.size() ? pkg->pkgCount = (int)pkg->vecBody.size() : pkg->pkgCount = (int)pkg->vecDataType.size();
		}

		//读取界面参数设置
		for (int i = 0; i < pkg->InterfaceDataNumb; i++)
		{
			INTERFACE_SET_DATA *pInterfaceData = new INTERFACE_SET_DATA;
			char   szValue[20];    
			char   szDefaultValue[20];  

			//pkgType = 类型*100 + 索引
			memset(szBuf, 0, 64);
			sprintf_s(szBuf, "interface%d", i);
			memset(szName, 0, 64);
			sprintf_s(szName, "type%d", i);  
			pInterfaceData->nIndex = GetPrivateProfileInt(szBuf, szName, 0, szCurrentPath);
			pInterfaceData->CurSceneID = GetPrivateProfileInt(szBuf, "CurSceneID", 0, szCurrentPath);
			pInterfaceData->iT = GetPrivateProfileInt(szBuf, "listingNumb", 0, szCurrentPath);
			GetPrivateProfileString(szBuf, "btnType", szDefaultValue, szValue, 20, szCurrentPath);
			pInterfaceData->BtnType = szValue;
			GetPrivateProfileString(szBuf, "btnName", szDefaultValue, szValue, 20, szCurrentPath);
			pInterfaceData->BtnName = szValue;
			pInterfaceData->ItemType = GetPrivateProfileInt(szBuf, "itemType", 0, szCurrentPath);
			pInterfaceData->ItemNumb = GetPrivateProfileInt(szBuf, "itemNumb", 0, szCurrentPath);
			pInterfaceData->InitValue = GetPrivateProfileDouble(szBuf, "initValue", 0.0, szCurrentPath);
			pInterfaceData->ChangeRate = GetPrivateProfileDouble(szBuf, "changeRate", 0.0, szCurrentPath);
			pInterfaceData->OptionNumb = GetPrivateProfileInt(szBuf, "optionNumb", 0, szCurrentPath);
			pInterfaceData->height = GetPrivateProfileInt(szBuf, "height", 0, szCurrentPath);
			for (int j = 0; j < pInterfaceData->OptionNumb; j++)
			{
				memset(szName, 0, 64);
				sprintf_s(szName, "optionName%d", j); 
				GetPrivateProfileString(szBuf, szName, szDefaultValue, szValue, 20, szCurrentPath);
				strcpy_s(pInterfaceData->OptionName[j], 16, szValue);
			}
			pkg->InterfaceSetData.push_back(pInterfaceData);
		}
		if (pkg->InterfaceDataNumb != (unsigned int)pkg->InterfaceSetData.size())
		{
			pkg->InterfaceDataNumb = (int)pkg->InterfaceSetData.size();
		}
	}
    return true;

    //pkg
}

bool CConfigManager::SaveSceneConfig(const char *pPath, void *pData)
{
    SCENE_PKG *pkg = NULL;
    if (NULL == pData)
    {
        return false;
    }
    char szCurrentPath[512] = {0};
    get_module_path((LPSTR)szCurrentPath);  //取得config.ini的地址
    //strcat_s(szCurrentPath, "\\");
    strcat_s(szCurrentPath, pPath);
    strcat_s(szCurrentPath, "\\");
    strcat_s(szCurrentPath, CONFIG_FILE_NAME);

    int index = 0;
    char szBuf[64] = {0};
    char szName[64] = {0};

    pkg = (SCENE_PKG *)pData;
    int iAffineType = 0;
    //读取包头信息
    if (pkg->bSave)
    {
        WritePrivateProfileInt("scene", "affine", 1, szCurrentPath);
    }
    else
    {
        WritePrivateProfileInt("scene", "affine", 0, szCurrentPath);
    }
    if (pkg->bIsSimulate)
    {
        WritePrivateProfileInt("scene", "simulate", 1, szCurrentPath);
    }
    else
    {
        WritePrivateProfileInt("scene", "simulate", 0, szCurrentPath);
    }
	//确定执行场景处理时，使用封装的方法(0)，还是使用Halcon调用过程的方法(1)
	WritePrivateProfileInt("scene", "ProcProcess", 0, szCurrentPath);


    WritePrivateProfileInt("scene", "station", pkg->stationID, szCurrentPath);
    WritePrivateProfileInt("scene", "imagenum", pkg->imageCount, szCurrentPath);
    WritePrivateProfileInt("scene", "pkgSize", pkg->pkgBodySize, szCurrentPath);
    WritePrivateProfileInt("scene", "pkgnum", pkg->pkgCount, szCurrentPath);
    WritePrivateProfileInt("scene", "modelnum", pkg->ModelCount, szCurrentPath);
	WritePrivateProfileInt("scene", "InterfaceDataNumb", pkg->InterfaceDataNumb, szCurrentPath);
	//

    //读取图像获取时设置参数
    for (int i = 0; i < pkg->imageCount; i++)
    {
        memset(szName, 0, 64);
        sprintf_s(szName, "image%d", i);
        WritePrivateProfileInt(szName, "no", pkg->vecInfo[i].CameraIndex, szCurrentPath);
        WritePrivateProfileInt(szName, "light", pkg->vecInfo[i].LightIndex, szCurrentPath);
        WritePrivateProfileInt(szName, "shutter", pkg->vecInfo[i].CameraShutter, szCurrentPath);
        WritePrivateProfileInt(szName, "gain", pkg->vecInfo[i].CameraGain, szCurrentPath);
    }

    //读取包数据内容
    for (int i = 0; i < pkg->pkgCount; i++)
    {
        VIRSION_DATA_PKG *pBody = pkg->vecBody[i];
        unsigned int *pDataType = pkg->vecDataType[i];
        int iSize = 0;
        int index = 0;

        //pkgType = 类型*100 + 索引
        memset(szBuf, 0, 64);
        sprintf_s(szBuf, "param%d", i);
        memset(szName, 0, 64);
        sprintf_s(szName, "type%d", index);  
        WritePrivateProfileInt(szBuf, szName, pDataType[index], szCurrentPath);
        WritePrivateProfileInt(szBuf, "pkgType", pBody[index].iValue, szCurrentPath);
        index++;
        memset(szName, 0, 64);
        sprintf_s(szName, "type%d", index);  
        WritePrivateProfileInt(szBuf, szName, pDataType[index], szCurrentPath);
        iSize = pBody[index].iValue;
        WritePrivateProfileInt(szBuf, "pkgSize", iSize, szCurrentPath);
        index++;
        for (int j = 0; j < iSize; j++)
        {
            memset(szName, 0, 64);
            sprintf_s(szName, "type%d", index);  
            WritePrivateProfileInt(szBuf, szName, pDataType[index], szCurrentPath);
            memset(szName, 0, 64);
            sprintf_s(szName, "data%d", index);
            int iT = (pDataType[index]%100);
            switch(iT)
            {
            case 0:  //int
                {
                    WritePrivateProfileInt(szBuf, szName, pBody[index].iValue, szCurrentPath);
                }
                break;
            case 1:  //double
                {
                    WritePrivateProfileDouble(szBuf, szName, pBody[index].dValue, szCurrentPath);
                }
                break;
            default:  //string
                {
                }
                break;
            }
            index++;
        }
    }

	//读取界面参数设置
	for (int i = 0; i < pkg->InterfaceDataNumb; i++)
	{
		INTERFACE_SET_DATA *pInterfaceData = pkg->InterfaceSetData[i];

		//pkgType = 类型*100 + 索引
		memset(szBuf, 0, 64);
		sprintf_s(szBuf, "interface%d", i);
		memset(szName, 0, 64);
		sprintf_s(szName, "type%d", i);  
		WritePrivateProfileInt(szBuf, szName, pInterfaceData->nIndex, szCurrentPath);
		WritePrivateProfileInt(szBuf, "curSceneID", pInterfaceData->CurSceneID, szCurrentPath);
		WritePrivateProfileInt(szBuf, "listingNumb", pInterfaceData->iT, szCurrentPath);
		WritePrivateProfileString(szBuf, "btnType", pInterfaceData->BtnType.c_str(), szCurrentPath);
		WritePrivateProfileString(szBuf, "btnName", pInterfaceData->BtnName.c_str(), szCurrentPath);
		WritePrivateProfileInt(szBuf, "itemType", pInterfaceData->ItemType, szCurrentPath);
		WritePrivateProfileInt(szBuf, "itemNumb", pInterfaceData->ItemNumb, szCurrentPath);
		WritePrivateProfileDouble(szBuf, "initValue", pInterfaceData->InitValue, szCurrentPath);
		WritePrivateProfileDouble(szBuf, "changeRate", pInterfaceData->ChangeRate, szCurrentPath);
		WritePrivateProfileInt(szBuf, "optionNumb", pInterfaceData->OptionNumb, szCurrentPath);
		WritePrivateProfileInt(szBuf, "height", pInterfaceData->height, szCurrentPath);
		for (int j = 0; j < pInterfaceData->OptionNumb; j++)
		{
			memset(szName, 0, 64);
			sprintf_s(szName, "optionName%d", j); 
			WritePrivateProfileString(szBuf, szName, pInterfaceData->OptionName[j], szCurrentPath);
		}
	}
    return true;
}

double CConfigManager::GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName,
    double dDefault, LPCTSTR lpFileName)
{
    double dValue = 0.0;
    char   szValue[64];    
    char   szDefaultValue[64];    
    GetPrivateProfileString(lpAppName, lpKeyName, szDefaultValue, szValue, 30, lpFileName);
    dValue = atof(szValue);
    return dValue; 
}
BOOL CConfigManager::WritePrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName,
    double dValue, LPCTSTR lpFileName)
{
    char   szValue[64];             
    sprintf_s(szValue, "%0.9lf", dValue);             
    return(   WritePrivateProfileString(lpAppName, lpKeyName, szValue, lpFileName)); 
}

BOOL CConfigManager::WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT Value, LPCTSTR lpFileName)     
{         
    char   szValue[16];             
    sprintf_s(szValue, "%d", Value);             
    return(   WritePrivateProfileString(lpAppName, lpKeyName, szValue, lpFileName)); 
}
