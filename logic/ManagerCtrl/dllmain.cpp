// dllmain.cpp : 定义 DLL 应用程序的入口点。  //iManagerCtrl.cpp
#include "stdafx.h"
#include "ManagerCtrl.h"
#include "iManagerCtrl.h"
#include "iImageCtrl.h"
#include "iLightCtrl.h"
#include "iCommunication.h"
#include "iConfigManager.h"

bool InitManager()
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->UnInitManager();
		Sleep(500);
		bRet = pInstance->InitManager();
	}
	return true;
}

bool UnInitManager()
{
	bool bRet = false;
	CManagerCtrl::ReleaseInstance();
	return true;
}



void GetSceneProcess(pSceneProcess* pProcess)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->GetSceneProcess(pProcess);
	}

}


/**********************************************************************
函数      : GetSceneNameInfo
功能      : 获取场景信息
输入参数  : N/A
输出参数  : iSceneNum 场景个数
pSceneName 场景别名列表
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool GetSceneNameInfo(int &iSceneNum, char **pSceneName)
{
	return true;
}

bool TriggerVisionProcess(int iSceneID, int iPercent)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->TriggerVisionProcess(iSceneID, iPercent);
	}
	return bRet;
}

/**********************************************************************
函数      : SetSysParam
功能      : 更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool SetSysParam(void *pData)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SetSysParam(pData);
	}
	return bRet;
}

/**********************************************************************
函数      : SaveSysParam
功能      : 更新并保存视觉配置
输入参数  : N/A 
pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool SaveSysParam(void *pData)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SaveSysParam(pData);
	}
	return bRet;
}
/**********************************************************************
函数      : GetSysParam
功能      : 获取系统参数
输入参数  : N/A 
输出参数  : pData    要获取的结构体指针
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool GetSysParam(void *pData)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->GetSysParam(pData);
	}
	return bRet;
}


void* GetSysParamPoint()
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetSysParamPoint();
	}
	return NULL;

}


bool SetSceneParam(void *pData)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SetSceneParam(pData);
	}
	return bRet;
}
/**********************************************************************
函数      : SaveSceneParam
功能      : 保存并更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool SaveSceneParam(int iSceneID, void *pData)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SaveSceneParam(iSceneID, pData);
	}
	return bRet;
}

bool  GetSceneParam(int iSceneID, void *pData)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->GetSceneParam(iSceneID, pData);
	}
	return bRet;
}

void* GetSceneParamHandle(int iSceneID)
{
	return CManagerCtrl::GetSceneHandle(iSceneID);

}



EXPORT_C DLL_EXPORT int GetSceneNum()
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetSceneNum();
	}
	return -1;

}
EXPORT_C DLL_EXPORT char* GetSceneConfigName()
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetSceneName();
	}
	return NULL;
}

EXPORT_C DLL_EXPORT const char* GetSceneConfigPath(int SceneID)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetSceneconfigPath(SceneID);
	}
	return NULL;

}

EXPORT_C DLL_EXPORT const char* GetScenePath(int SceneID)
{

	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetScenePath(SceneID);
	}
	return NULL;


}



EXPORT_C DLL_EXPORT int GetSceneProcType(int SceneID)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return 	pInstance->GetSceneProcType(SceneID);
	}
	return -1;

}

EXPORT_C DLL_EXPORT int GetSceneId(int SceneIndex)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{

		return 	pInstance->GetSceneId(SceneIndex);

	}
	return -1;



}




EXPORT_C DLL_EXPORT char* GetSysConfigPath()
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetSysconfigPath();
	}
	return NULL;
}

void SetCamParam(int CamIndex,int shutterTime,int Gain)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->SetCamParam(CamIndex,shutterTime,Gain);
	}

}




int SceneGrabImage(int iSceneID,int ImageIndex,int CamIndex,int shutterTime,int Gain,int Interval,int LightIndex,int LightValue)
{

	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->SceneGrabImage(iSceneID, ImageIndex,CamIndex,shutterTime,Gain,Interval,LightIndex,LightValue);
	}
	return -1;
}

int SceneGrabFinished(int iSceneID, CHECK_RESULT *res,int iErrorCode)
{

	CManagerCtrl *pInstance = NULL;
	int TelSceneId = iSceneID;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->ManagerGrabFinish(TelSceneId, res,iErrorCode);
		return 0;
	}
	return -1;

}




bool InitGrabSceneimage(int iSceneID, int index)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->InitGrabSceneimage(iSceneID, index);
	}
	return bRet;
}

bool InitGrabSceneimage1(int CamIndex ,int iSceneID,int ImageIndex)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->InitGrabSceneimage1( CamIndex , iSceneID, ImageIndex);
	}
	return bRet;

}


/**********************************************************************
函数      : GetCameraIDList
功能      : 获取相机GUID列表
输入参数  : guidlist， GUID列表
输出参数  : N/A
返回值    : 返回结果
备注      : 
**********************************************************************/
int GetCameraIDList(std::list<std::string> &guidlist,int dllnum,string dlllist[])
{
	return getGUIDList(guidlist,dllnum,dlllist);
}

bool SetDispWindow(int iSceneID, int iWinID)
{
	return true;
}

bool OpenContinousGrab(int iWindowID, int iSceneID, int index, unsigned int MsgID,int CameraIndex)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->OpenContinousGrab(iWindowID, iSceneID, index, MsgID,CameraIndex);
	}
	return bRet;
}
bool ColseContinousGrab(int iSceneID, int index,int CameraIndex)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->ColseContinousGrab(iSceneID, index,CameraIndex);
	}
	return bRet;
}

/**********************************************************************
函数      : OpenWindow
功能      : 绑定halcon窗口与UI窗口
输入参数  : iWinID 界面窗口ID
x,y,width,hight 界面窗口位置大小
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool BindWindow(long iWinID, int x, int y, int width, int hight, bool bDebug)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->BindWindow(iWinID, x, y, width, hight, bDebug);
	}
	return bRet;
}

bool SetDebugWindowID(int iSceneID, long iWinID)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SetDebugWindowID(iSceneID, iWinID);
	}
	return bRet;
}
/**********************************************************************
函数      : ClearWindow
功能      : 清除对应窗口上信息
输入参数  : iWinID 界面窗口ID
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool ClearWindow(long iWinID)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->ClearWindow(iWinID);
	}
	return bRet;
}

/**********************************************************************
函数      : ReadImage
功能      : 加载本地图片
输入参数  : iSceneID  窗口序号
pFileName 文件名称
index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool ReadImage(const char *pFileName, int iSceneID, int index)
{
	bool bRet;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	bRet = pInstance->ReadImage(iSceneID, pFileName, index);
	return bRet;
}

/**********************************************************************
函数      : DispImage
功能      : 显示指定图片到指定窗口上
输入参数  : iWinID 界面窗口ID
iSceneID  窗口序号
index     图像序号，调试使用
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool DispImage(long iWinID, int iSceneID, int index)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->DispImage(iWinID, iSceneID, index);
	}
	return bRet;
}


bool FindHalconWinID(int iWinID, long& HalconWinID)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->FindHalconWinID( iWinID,HalconWinID);
	}
	return bRet;


}

bool ShowVisionString(long winID,char* StrContent,int topRow,int topCol )
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->ShowVisionString(winID,StrContent,topRow,topCol);
	}
	return bRet;


}
bool SetVisionColorHeight(long hwinID,char* strColor,int FontHeight)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SetVisionColorHeight(hwinID,strColor,FontHeight);
	}
	return bRet;


}


/**********************************************************************
函数      : DispObj
功能      : 显示处理结果
输入参数  : iWinID 界面窗口ID
iSceneID  窗口序号
bIsShowTmp 调试参数，用于显示中间过程
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool DispObj(int iWinID, int iSceneID, bool bIsShowTmp)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->DispObj(iWinID, iSceneID, bIsShowTmp);
	}
	return bRet;
}

bool SaveImage(int iSceneID, int index, string SNstr)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SaveImage(iSceneID,index, SNstr);
	}
	return bRet;
}

EXPORT_C DLL_EXPORT bool SaveImageDump( const long winID, int iSceneID,char* outpath )
{
	bool bRet = false;   
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->SaveImageDump(winID, iSceneID,outpath);
	}
	return bRet;
}

EXPORT_C DLL_EXPORT bool SendImageDump( void *buff, int size,int w,int h,int iSceneID )
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		SendImage(iSceneID,buff,size,w,h);
	}
	return true;
}

EXPORT_C DLL_EXPORT void* GetImageDump( int winID, int sence,double &width,double &heigh )
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetImageDump(winID,sence,width,heigh);
	}
	return NULL;
}
/**********************************************************************
函数      : DispROI
功能      : 置为绘制ROI模式
输入参数  : iWinID 窗口ID
iSceneID  场景ID，根据该ID获取参数
index 当前场景中的第几个ROI
iType, 0,为处理图像区域, 1为模板区域，2为画直线， 3为画圆， 待改为枚举类型
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool DispROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->DispROI(iWinID, iSceneID, index, iType);
	}
	return bRet;
}

/**********************************************************************
函数      : DrawROI
功能      : 置为绘制ROI模式
输入参数  : iSceneID  窗口序号
index 当前场景中的第几个ROI
iType, 0,为处理图像区域, 1为模板区域，2为画直线， 3为画圆， 待改为枚举类型
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool DrawROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->DrawROI(iWinID, iSceneID, index, iType);
	}
	return bRet;
}

/**********************************************************************
函数      : DrawROI
功能      : 直接绘制ROI
输入参数  : iSceneID  窗口序号
iType, 画直线， 画圆， 
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/

bool OnDrawROI(int iWinID, int iSceneID,PROCESS_TYPE iType,RoiCombType CombType,vector<int>* pRoiIdList)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->OnDrawROI(iWinID, iSceneID, iType,CombType,pRoiIdList);
	}
	return bRet;
}

// int  GetLastShapeId(int iSceneID)
//{
//	CManagerCtrl *pInstance = NULL;
//	pInstance = CManagerCtrl::GetInstance();
//	if (NULL != pInstance)
//	{
//		return  pInstance->GetLastShapeId(iSceneID);
//	}
//	
//
//}

void  GenShape(int SceneID,const ST_ROI_INFO& roi,ShapeType shapeflag,int ShapeId,int CamIndex)
{

	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->GenShape(SceneID, roi, shapeflag,ShapeId,CamIndex);
	}

}


int GetCurShapeId(int iSceneID)
{

	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return pInstance->GetCurShapeId(iSceneID);
	}
	return -1;

}

bool   GetRoiParam(int sceneID,int ShapeId,ST_ROI_INFO* pRoi)
{

	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->GetRoiParam(sceneID,ShapeId,pRoi);
	}
	return bRet;


}


void OnShowAllRoi(int iWinID, int iSceneID,const vector<int>* const pRoiDdList)
{

	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->OnShowAllRoi(iWinID, iSceneID,pRoiDdList);
	}

}







/**********************************************************************
函数      : CreateVisionModel
功能      : 创建模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CreateVisionModel(int iWinID,int iSceneID, int index)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->CreateVisionModel(iWinID,iSceneID, index);
	}
	return bRet;
}

///**********************************************************************
//函数      : CreateWay2VisionModel
//功能      : 创建模板
//输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
bool CreateWay2VisionModel(int iWinID,int iSceneID,int RoiID,char* modePath, int Nevels,double AngleStart,double AngleExtent,
	int Contrast,int index)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->CreateVisionModel(iWinID,iSceneID,RoiID, modePath,Nevels,AngleStart,AngleExtent,Contrast,index);
	}
	return bRet;

}





///**********************************************************************
//函数      : UpdateVisionModel
//功能      : 更新模板
//输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//bool UpdateVisionModel(int iSceneID, int index, int iType)
//{
//    bool bRet = false;
//    CManagerCtrl *pInstance = NULL;
//    pInstance = CManagerCtrl::GetInstance();
//    if (NULL != pInstance)
//    {
//        if(pInstance->UpdateVisionModel(iSceneID, index, iType) >= 0)
//        {
//            bRet = true;
//        }
//    }
//    return bRet;
//}

/**********************************************************************
函数      : FindVisionModel
功能      : 查找模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool FindVisionModel(int iSceneID, int index, bool bIsB)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->FindVisionModel(iSceneID, index, bIsB);
	}
	return bRet;
}
/**********************************************************************
函数      : FindVisionModel
功能      : 查找模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool FindWay2VisionModel(int iSceneID, int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum,double& Score,
	int ProcIndex,bool IsBulid,double& row,double& col,double& phi)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->FindWay2VisionModel(iSceneID,WinID, index, szModePath, PyramidLevel, MatchNum, Score,ProcIndex,IsBulid, row, col, phi);
	}
	return bRet;
}
EXPORT_C DLL_EXPORT bool FindModelRealTime(int iSceneID, int WinID, int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
	int ProcIndex,bool IsBulid,double& row,double& col,double& phi)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->FindModelRealTime(iSceneID,WinID, index, szModePath, PyramidLevel, MatchNum, Score,ProcIndex,IsBulid, row, col, phi);
	}
	return bRet;

}

void MoveAndRotInCamForOriMatch(int iSceneID,double PixSize,bool AxeMoveRotDirect,double& MotorRelX,double& MotorRelY,
	int LastSceneId,int NextSceneId,double SceneRotPhi,double PicCentX,double PicCentY,double* pCamParam ,int ParamNum,double* pCamPose,double CalThick,double RotCenX,
	double RotCenY,double MotorRotPhi,double CalRotPhi,int CamIndex,int AxeIndex,int ToolIndex)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->MoveAndRotInCamForOriMatch(iSceneID,PixSize,AxeMoveRotDirect,MotorRelX, MotorRelY,LastSceneId,
			NextSceneId,SceneRotPhi,PicCentX,PicCentY,RotCenX, RotCenY, MotorRotPhi, CalRotPhi,
			pCamParam,ParamNum,pCamPose,CalThick, CamIndex, AxeIndex, ToolIndex);
	}

}

bool AffinePointByCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double& PointX,double& PointY)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->AffinePointByCamInMecRot(CamIndex, AxisIndex,ToolIndex, PointX, PointY);
	}
	return bRet;


}

void AffToRobotWorld(int SceneId,int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double ModRotPhi,
	double& ToolWorldX,double& ToolWorldY,bool WithCamParam,double CalThickness,double* pCamParam,int ParamNum,double* CamPos)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->AffToRobotWorld(SceneId,CamIndex, AxisIndex, ToolIndex, PixSize, ModRotPhi,
			ToolWorldX,ToolWorldY,WithCamParam,CalThickness,pCamParam,ParamNum,CamPos);
	}
}


void AffinePointToWorld(int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double OriCamX,double OriCamY,
	double OriCamPhi,double CurCamX,double CurCamY,double CurCamPhi,double& MoveX,double& MoveY,int ResType,
	bool MoveRotDirect,double CurMatchPhi,double AxeRotX,double AxeRotY)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->AffinePointToWorld(CamIndex,AxisIndex,ToolIndex,PixSize,OriCamX, OriCamY,OriCamPhi,CurCamX, 
			CurCamY, CurCamPhi,MoveX,MoveY, ResType,MoveRotDirect, CurMatchPhi, AxeRotX, AxeRotY);
	}
}




/**********************************************************************
函数      : VisionDebugThreshold
功能      : 调试阈值接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index， 根据该index获取参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool VisionDebugThreshold(int iWinID, int iSceneID, int index)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->VisionDebugThreshold(iWinID, iSceneID,index);
	}
	return bRet;
}

/**********************************************************************
函数      : VisionFindLine
功能      : 找线接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index， 根据该index获取找圆参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool VisionFindLine(int iSceneID, int index, bool bIsFollow)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->VisionFindLine(iSceneID,index, bIsFollow);
	}
	return bRet;
}

/**********************************************************************
函数      : VisionFindCircle
功能      : 找圆接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index， 根据该index获取找圆参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool VisionFindCircle(int iSceneID, int index, bool bIsFollow)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->VisionFindCircle(iSceneID,index, bIsFollow);
	}
	return bRet;
}

bool  VisionMeasureCircle(long WinId, int iSceneID, int Imageindex,int MatchIndex, double& Row,double& Col,double& Rii)
{
	bool bRet = false;
	long HalconWinID;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		FindHalconWinID( WinId,  HalconWinID);
		bRet = pInstance->VisionMeasureCircle(HalconWinID,iSceneID,  Imageindex, MatchIndex, Row, Col, Rii);
	}
	return bRet;
}



/**********************************************************************
函数      : CtrlComm
功能      : 打开or关闭光源
输入参数  : pCommName 串口名称
bOpen     打开操作或关闭操作
iType     光源类型
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CtrlComm(LightInfo *pInfo, bool bOpen, LightTypeInfo *pTypeInfo)
{
	//LightInfo info;
	//info.PortName = pCommName;
	//info.LightType = (LIGHTCTL_TYPE)iType;
	if (NULL == pInfo)
	{
		return false;
	}
	if (bOpen)
	{
		if(LIGHTCTL_SUCCESS != iLightInstance(pInfo, 1, pTypeInfo))
		{
			return false;
		}
	}
	else
	{
		if(LIGHTCTL_SUCCESS != deInitLightCtrl(pInfo->PortName))
		{
			return false;
		}
	}
	return true;
}
void RestoreImagePart(int WinID, int iScene, int width, int height, int index)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->RestoreImagePart(WinID, iScene, width, height, index);
	}
}

void UpdateImagePart(int WinID, int iScene, int iType, double dXPer, double dYPer)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->UpdateImagePart(WinID, iScene, iType, dXPer, dYPer);
	}
}

/**********************************************************************
函数      : VisionProcess
功能      : 整体检测
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool VisionProcess(int iSceneID, int index,CHECK_RESULT &result)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;

	memset(&result,0,sizeof(CHECK_RESULT));
	result.bIsDisp = true;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->VisionGetResult(iSceneID,index,result);
	}
	return bRet;
}

/**********************************************************************
函数      : VisionProcess
功能      : 整体检测
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool VisionCalibProcess(CALIB_DATA *pData)
{
	bool bRet = true;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->VisionCalibProcess(pData);
	}
	return bRet;
}
/**********************************************************************
函数      : OpenCheckStateThread
功能      : 打开检测系统状态线程
输入参数  :   

输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool OpenCheckStateThread()
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		bRet = pInstance->OpenCheckStateThread();
	}
	return bRet;
}



int ResetFinishedSendMsg(int iErrorCode)
{
	return ResetFinished(iErrorCode);
}


int JudgeLightConnectStatus(string comName)
{
	return JudgeLightConnect(comName);
}
//获取对应类型参数在pkg->pBody中的位置
//input 类型， 索引， pkg, 
//返回找到位置
int IFindParamData(SCENE_PKG *pkg, int iType, int index)
{
	int iRet = 0;
	iRet = CManagerCtrl::FindParamData(pkg,iType, index);
	return iRet;
}
int FindSpecTypePos(const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iType)
{
	int iRet = 0;
	iRet = CManagerCtrl::FindSpecTypePos(pkg,pType, iType);
	return iRet;
}

int InsertParamItem(int SceneId, VIRSION_DATA_PKG *pkg, unsigned int *pType, int iSize, int insertPos)
{
	int iRet = 0;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		iRet = pInstance->InsertParamItem(SceneId, pkg, pType, iSize, insertPos);
	}
	return iRet;
}

int UpdateParamItem( int SceneId, const VIRSION_DATA_PKG *pkg, unsigned int *pType, INTERFACE_SET_DATA *dataTmp, int iSize, int insertPos, bool insert /*= false*/ )
{
	int iRet = 0;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		iRet = pInstance->UpdateParamItem(SceneId, pkg, pType, dataTmp, iSize, insertPos, insert);
	}
	return iRet;
}
int SwapParamItem(int SceneId, int iPosF, int iPosN)
{
	int iRet = 0;
	return iRet;
}
int DeleteParamItem(int SceneId, int insertPos)
{
	int iRet = 0;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		iRet = pInstance->DeleteParamItem(SceneId, insertPos);
	}
	return iRet;
}

int OpratorLight(string comName, int index, int value, int type)
{
	int iRet = 0;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		switch(type)
		{
		case TYPE_OPEN:
			{
				iRet = openLight(comName, index);
			}
			break;
		case TYPE_CLOSE:
			{
				iRet = closeLight(comName, index);
			}
			break;
		case TYPE_SET_VAULE:
			{
				iRet = setLightValue(comName, index, value);
			}
			break;
		default:
			iRet = -1;
			break;
		}
	}
	return iRet;
}


void  SetWebProctocol(const PROCTOCOLDATA& ProctoData)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->SetProctocolData(ProctoData);
	}
	SetCommuniWebProctocol(ProctoData);
}

void  GetWebProctocol(PROCTOCOLDATA& ProctoData)
{
	GetCommuniWebProctocol(ProctoData);
}


void DrawRect(int SenceId)
{
	bool bRet = false;
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		//bRet = pInstance->m_pVisionInstance->DrawRegionRect(SenceId);
	}

}

////产品的设定函数，用于换产品时一键切换
void SetProductMap(void* pMapData)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		((SYSTEM_PARAM*)(pInstance->GetSysParamPoint()))->ProductMap = *(LPPMAPDATA)pMapData;
		pInstance->SetProductMap(pMapData);

	}


}
void GetProductMap(void* pMapData)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		*(LPPMAPDATA)pMapData = ((SYSTEM_PARAM*)(pInstance->GetSysParamPoint()))->ProductMap;
		//pInstance->GetProductMap(pMapData); //不应该实时地从配置文件里读取
	}


}


void SetSysCurProductName(string ProductName)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->SetCurProductName(ProductName);
	}

}

void SaveSysRotAxleAdjust(int AxisIndex, int ToolIndex,double RotXAd,double RotYAd)
{

	ConfigSaveSysRotAxleAdjust(AxisIndex,ToolIndex,RotXAd,RotYAd);
}

void ReadSysRotAxleAdjust(int AxisIndex, int ToolIndex,double& RotXAd,double& RotYAd)
{
	ConfigReadSysRotAxleAdjust(AxisIndex,ToolIndex,RotXAd,RotYAd);
}


bool HalconProcedureLocate(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,double (*inParam)[HalProcInParamNum],
	double& PosX,double& PosY,double& PosPhi)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return  pInstance->HalconProcedureLocate(iWinID,iSceneID,Imageindex,ProcedurePath,inParam,PosX,PosY,PosPhi);
	}
	return false;

}
bool HalconProcedureProcess(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return  pInstance->HalconProcedureProcess(iWinID,iSceneID,Imageindex,ProcedurePath,inparam,outparam,display,error,hmessage);
	}
	return false;
}

bool IImageEnhancement(long iWinID,int iSceneID,int Imageindex,double& multinum,double& addnum,int display,int &error,string &hmessage)
{

	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return  pInstance->ImageEnhancement(iWinID,iSceneID,Imageindex,multinum,addnum,display,error,hmessage);
	}
	return false;
}


CHECK_RESULT* GetSceneProcResult(string &scneName)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return  pInstance->GetSceneProcResult(scneName);
	}
	return false; 
}
CHECK_RESULT* GetSceneProcResultById(int  iSceneID)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return  pInstance->GetSceneProcResultById(iSceneID);
	}
	return false; 
}

int SetSceneProcResult(string &scneName,CHECK_RESULT& res)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return  pInstance->SetSceneProcResult(scneName,res);
	}
	return false; 
}
int  ClearProcResultByID(int iSceneID)
{
	CManagerCtrl *pInstance = NULL;
	pInstance = CManagerCtrl::GetInstance();
	if (NULL != pInstance)
	{
		return  pInstance->ClearProcResultByID(iSceneID);
	}
	return false; 
}




