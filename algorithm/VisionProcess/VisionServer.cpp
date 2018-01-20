#pragma once
#include "StdAfx.h"
#include "VisionServer.h"
#include "VisionProcess.h"
#include <io.h>
#include <direct.h>
#include "sysapi.h" //调用了路径接口




map<int, CVisionProcess *>CVisionServer::m_vecVisionInstance;

CVisionServer::CVisionServer(void)
{
}


CVisionServer::~CVisionServer(void)
{
}

CVisionProcess *CVisionServer::GetInstance(int iSceneID)
{
    CVisionProcess *pVision = NULL;
    map<int, CVisionProcess *>::iterator  it; 
    for(it = m_vecVisionInstance.begin(); it != m_vecVisionInstance.end(); it++)
    {
        if (iSceneID == it->first)
        {
            pVision = it->second;
        }
    }
    if (NULL == pVision)
    {
        pVision = new CVisionProcess;
        m_vecVisionInstance.insert(make_pair(iSceneID, pVision));
    }
    return pVision;
}

void CVisionServer::ReleaseInstance()
{
    CVisionProcess *pVision = NULL;
    map<int, CVisionProcess *>::iterator  it; 
    for(it = m_vecVisionInstance.begin(); it != m_vecVisionInstance.end(); it++)
    {
        if (NULL != it->second)
        {
            pVision = it->second;
            delete pVision;
            pVision = NULL;
        }
    }
    m_vecVisionInstance.clear();
}

bool CVisionServer::InitVision(void)
{
    //确认标定文件夹是否存在， 不存在则创建
    char szPath[MAX_BUFFER] = {0};
    char buf[64] = {0};
    char exePath[MAX_PATH] = {0};
    DWORD dwLen = GetModuleFileName(NULL, exePath, sizeof(exePath));
    std::string strCommonPath(exePath);
    std::string strTuple(exePath);
    size_t lastSpit = strCommonPath.find_last_of("\\");
    strTuple = strCommonPath;
    strcpy_s(buf,"\\calib");
    strTuple.replace(lastSpit, string::npos, buf);    //标定数据目录
    strcpy_s(buf,"\\data"); 
    strCommonPath.replace(lastSpit, string::npos, buf);//检测参数目录
    if (-1 == _access(strTuple.c_str(), 0))
    {
        _mkdir(strTuple.c_str());
    }
    if (-1 == _access(strCommonPath.c_str(), 0))
    {
        _mkdir(strCommonPath.c_str());
    }
    return true;
}

/**********************************************************************
函数      : OpenWindow
功能      : 绑定窗口
输入参数  : winID 窗口ID
            x,y,width,hight, 窗口位置及宽高
输出参数  : halconWinID halcon窗口ID
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CVisionServer::OpenWindow(long winID, int x, int y, int width, int hight, long &halconWinID)
{

	return CVisionProcess::openWindow(winID, x, y, width, hight,halconWinID);

}

/**********************************************************************
函数      : CloseHalconWindow
功能      : 关闭窗口
输入参数  : halconWinID halcon窗口ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CVisionServer::CloseHalconWindow(long halconWinID)
{
   /*//close_window(halconWinID);
	HalconCpp::CloseWindow(halconWinID);
    return true;*/

	return CVisionProcess::closeWindow(halconWinID);
}

//传入相机高宽，获取指向图像内存的指针
char * CVisionServer::GetImageBuf(int iSceneID, int index, int iWidth, int Height)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(iSceneID);
	return pVision->GetImageBuf(index, iWidth, Height);
}
/**********************************************************************
函数      : ReadImage
功能      : 绑定窗口
输入参数  : pFileName 图片路径
            sceneID   场景ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 里面用到halcon
**********************************************************************/
bool CVisionServer::ReadImage(const char *pFileName, int sceneID, int index, bool bIsNew)
{
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    pVision->ReadImage(pFileName, index, bIsNew);
    
    return true;
}

/**********************************************************************
函数      : DispImage
功能      : 绑定窗口
输入参数  : iWinIndex 显示的目标窗口
            sceneID   场景ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CVisionServer::DispImage(int iWinIndex, int sceneID, int index)
{
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    return  pVision->DispImage(iWinIndex, index);

}

bool CVisionServer::DispObj(int iWinIndex, int sceneID, bool bIsShowTmp)
{
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    return pVision->DispObj(iWinIndex, bIsShowTmp);

}

bool CVisionServer::ShowWindString(long winID,char* StrContent,int topRow ,int topCol )
{
	 return  CVisionProcess::ShowWindString(winID,StrContent,topRow,topCol);

}

bool CVisionServer::SetDispColorHeight(long winID,char* strColor,int FontHeight)
{
	return CVisionProcess::SetDispColorHeight(winID,strColor,FontHeight);
}



/**********************************************************************
函数      : VisionProcess
功能      : 初始化处理
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CVisionServer::VisionProcess(void)
{
	
    return true;
}


void  CVisionServer::CalCamMetrix(int CamerIndex,int DataCount,double MotorX[],double MotorY[],double CamRow[],double CamCol[],
	double PixSize,double(*pCamRot),double (*pMecAndCam),double(*pImageRela),double (*pImageAbs),double (*pCamParam),int ParamNum,double (*pCamPose),int ToolIndex,
	int AxeIndex,double CalFirstX,double CalFirstY,double CalFirstAngle,double ToolFirstX,double ToolFirstY,double CamRotAngle,
	double RotToCamX,double RotToCamY,double CalThickNess)
{
	CVisionProcess::CalCamMetrix(CamerIndex,DataCount,MotorX,MotorY,CamRow,CamCol,PixSize,pCamRot,pMecAndCam,
		pImageRela,pImageAbs,pCamParam,
		ParamNum,pCamPose,ToolIndex,AxeIndex,CalFirstX,CalFirstY,CalFirstAngle,ToolFirstX,ToolFirstY,CamRotAngle,
		RotToCamX,RotToCamY);

}



void CVisionServer::AffineCoorPoint(double CoordX,double CoordY,double CoordAng,double& PointX,double& PointY)
{
	CVisionProcess::AffineCoorPoint(CoordX,CoordY,CoordAng,PointX,PointY);
}


bool CVisionServer::Imagebuffer(int cameraindex, int sceneID, long *pbuffer)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	pVision->Imagebuffer(cameraindex,sceneID,pbuffer);
	return true;
}



bool  CVisionServer::drawRoi(int sceneID,long  halconID , const  ST_ROI_INFO  &roi, ST_ROI_INFO &outroi ,bool bflag ,bool bcolor)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	pVision->drawRoi(halconID,roi,outroi,bflag,bcolor);
	
	return true;
}

bool  CVisionServer::OndrawRoi(int sceneID,long  halconID , const  ST_ROI_INFO  &roi,RoiCombType CombType ,PROCESS_TYPE bflag ,vector<int>* pRoiIDList,bool bcolor)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	return pVision->OndrawRoi(halconID,roi,CombType,bflag,pRoiIDList,bcolor);

	//return true;
}

void CVisionServer::GenShape(int SceneID,const ST_ROI_INFO& roi,ShapeType shapeflag,int ShapeId,int Width,int Height)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(SceneID);
	return pVision->GenShape(roi,shapeflag,ShapeId,Width,Height);

}

int CVisionServer::GetCurShapeId(int sceneID)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	return pVision->GetCurShapeId();

}

bool  CVisionServer::GetRoiParam(int sceneID,int ShapeId,ST_ROI_INFO* pRoi)
{

	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	return pVision->GetRoiParam(ShapeId,pRoi);

}

 void CVisionServer::OnShowAllRoi(int sceneID,long  halconID,const vector<int>* const pRoiDdList)
 {
	 CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	  pVision->OnShowAllRoi(halconID,pRoiDdList);

 }


bool  CVisionServer::dispRoi(int sceneID, long  halconID , ST_ROI_INFO  roi,bool bflag,bool bcolor)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	pVision->dispRoi(halconID,roi,bflag,bcolor);

	return true;
}

bool CVisionServer::saveWinImage( const long winID,char *path,char *outpath,string SNstr )
{
	return	CVisionProcess::SaveWinImage(winID,path,outpath,SNstr);
}

void* CVisionServer::getWinImage( const long winID,double &width,double &heigh )
{
	return CVisionProcess::GetWinImage(winID,width,heigh);
}

bool  CVisionServer::saveImage(int  sceneID,int index,char *path,string SNstr)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	return pVision->SaveImage(index,path,SNstr);
}

bool CVisionServer::saveImagePath(int  sceneID,int index,char *path,string SNstr,unsigned int type)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	pVision->saveImagePath(index,path,SNstr);
	return true;
}

char* CVisionServer::getImagePath(int  sceneID,int index,unsigned int type)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	return pVision->getImagePath(index,type);
}



int CVisionServer::buildModel(int iWinID,int sceneID,int index, SCENE_PKG *pkg,char *sceneName,char *path)
{
    int iRet = -1;
    if (NULL == pkg)
    {
        return iRet;
    }
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    //找到当前模板参数
    int iPos = pVision->FindParamData(pkg, MODEL_PRO, index);
    VIRSION_DATA_PKG *paramD = NULL;
    if (iPos < 0 || iPos >= pkg->vecBody.size() || iPos >= pkg->vecDataType.size())
    {
        return iRet;
    }
    paramD = pkg->vecBody[iPos];
    int iLength = paramD[1].iValue;
    unsigned int *pType = pkg->vecDataType[iPos];  
	iRet = pVision->buildModel(iWinID,paramD,iLength, pType,sceneName,path);
    return iRet;
}

bool  CVisionServer::buildModel(int iWinID,int iSceneID,int RoiID,char* modePath, int Nevels,double AngleStart,double AngleExtent,
	                           int Contrast,int index)
{
	  CVisionProcess *pVision = CVisionServer::GetInstance(iSceneID);
	  return pVision->buildModel(iWinID,RoiID,modePath,Nevels,AngleStart,AngleExtent,Contrast,index);

}

int CVisionServer::clearModel(int sceneID, int iModelID, int iType)
{
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    return pVision->clearModel(iModelID, iType);
}

int  CVisionServer::readModel(int sceneID, char *sceneName,char *path, int index, int iType)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	return pVision->readModel(sceneName,path,index, iType);
}

int  CVisionServer::findModel(const int sceneID, int index, SCENE_PKG *pkg, ST_RECTANGLE_DATA &data, unsigned int bDisplay)
{
    int iRet = -1;
    if (NULL == pkg)
    {
        return iRet;
    }
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    //找到当前模板参数
    int iPos = pVision->FindParamData(pkg, MODEL_PRO, index);
    VIRSION_DATA_PKG *paramD = NULL;
    if (iPos < 0 || iPos >= pkg->vecBody.size() || iPos >= pkg->vecDataType.size())
    {
        return iRet;
    }
    paramD = pkg->vecBody[iPos];
    int iLength = paramD[1].iValue;
    unsigned int *pType = pkg->vecDataType[iPos];
    //模板查找
    iRet = pVision->findModel(pkg->szSceneName, paramD,iLength, pType,data, bDisplay);
    return iRet;
}


bool CVisionServer::FindWay2VisionModel(const int iSceneID,int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
	                    int ProcIndex,bool IsBulid,double& row,double& col,double& phi)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(iSceneID);
	return pVision->MatchModel(WinID,index,szModePath,PyramidLevel,MatchNum,Score,ProcIndex,IsBulid,row,col,phi);

}

bool CVisionServer::FindModelRealTime(const int iSceneID,int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
	int ProcIndex,bool IsBulid,double& row,double& col,double& phi)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(iSceneID);
	return pVision->FindModelRealTime(WinID,index,szModePath,PyramidLevel,MatchNum,Score,ProcIndex,IsBulid,row,col,phi);

}

void CVisionServer::MoveAndRotInCamForOriMatch(int iSceneID,double PixSize,bool AxeMoveRotDirect,
	     double& MotorRelX,double& MotorRelY,int LastSceneId,int NextSceneId,double SceneRotPhi,double PicCentX,double PicCentY,double RotCenX,double RotCenY,
		 double MotorRotPhi,double CalRotPhi,double(*pCamParam),int ParamNum,double(*pCamPose),double CalThickness,
		  int CamIndex,int AxeIndex,int ToolIndex)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(iSceneID);
	 pVision->MoveAndRotInCamForOriMatch(PixSize,AxeMoveRotDirect,MotorRelX, MotorRelY,LastSceneId,iSceneID,
		 NextSceneId,SceneRotPhi,PicCentX,PicCentY,RotCenX, RotCenY, MotorRotPhi,CalRotPhi,pCamParam,ParamNum,pCamPose,CalThickness,
		 CamIndex,AxeIndex,ToolIndex);

}


bool CVisionServer::AffinePointByCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double& PointX,double& PointY)
{
	HTuple hv_QX ;
	HTuple hv_QY ;
	//HTuple hv_ComToMotor;

	double Px = PointX;
	double Py = PointY;


	AffineTransPoint2d(CVisionProcess::m_CamRotToMotorHomMat2D[CamIndex][AxisIndex][ToolIndex],Px,Py,&hv_QX,&hv_QY);
	PointX = hv_QX.D();
	PointY = hv_QY.D();

	return true;

}

void CVisionServer::AffToRobotWorld(int SceneId,int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double ModRotPhi,
	double& ToolWorldX,double& ToolWorldY,bool WithCamParam,double CalThickness,double* pCamParam,int ParamNum,double* CamPos)
{
	CVisionProcess *pVision = CVisionServer::GetInstance(SceneId);
	 pVision->AffToRobotWorld(CamIndex,AxisIndex,ToolIndex,PixSize,ModRotPhi,ToolWorldX,ToolWorldY,WithCamParam,CalThickness,pCamParam,ParamNum,
		    CamPos);

}


void CVisionServer::AffinePointToWorld(int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double OriCamX,double OriCamY,
	double OriCamPhi,double CurCamX,double CurCamY,double CurCamPhi,double& PointX,double& PointY,int ResType,
	bool MoveRotDirect,double CurMatchPhi,double AxeRotX,double AxeRotY)
{

	 // int  ResType = 0; //返回值的类别
	  bool RotDirect = true; // true机械和图像中旋转方向一致，false方向相反。

	  //HTuple hv_X1,hv_Y1;
	  HTuple hv_X2,hv_Y2;
	  HTuple hv_Hom2dIdenty;

	  HTuple MatchPhi = 0.0;  //匹配得出的角度
	  //HTuple AxeRotX = 0.0;  //轴在夹具坐标系中的坐标。
	  //HTuple AxeRotY = 0.0;
	  HTuple hv_ReX,hv_ReY;

	  
	 //考虑旋转
	   //1,对当前定位点旋转,旋转中心是标定出来的轴偏移中心RotX,RotY。
	     HTuple hv_Hom2dAxe;
	     HomMat2dIdentity(&hv_Hom2dAxe);
		 HomMat2dRotate(hv_Hom2dAxe,-MatchPhi,AxeRotX,AxeRotY,&hv_Hom2dAxe); //旋转后的点在夹具坐标系中的位置。
		                                     //-MatchPhi取负是因为旋转是校正匹配角度，所以是反方向。
	   //2,计算夹具坐标系到当前相机系中的变换。（就是相机坐标系移动到夹具坐标系需要的动作。
		 HTuple hv_Hom2dTool;
		 HomMat2dIdentity(&hv_Hom2dTool);
		 HomMat2dRotateLocal(hv_Hom2dTool,MatchPhi,&hv_Hom2dTool);
		 HomMat2dTranslate(hv_Hom2dTool,PointX,PointY,&hv_Hom2dTool);

	 //计算旋转后的夹具在机械坐标系中的位置。
		 HomMat2dIdentity(&hv_Hom2dIdenty);
		//1,根据初始相机位和当前相机位的差别,计算出当前相机在初始相机中的位置
		 VectorAngleToRigid(CurCamY,CurCamX,CurCamPhi,OriCamY,OriCamX,OriCamPhi,&hv_Hom2dIdenty); 
		//2,计算出定位点在初始相机中的 位置
		 //AffineTransPoint2d(hv_Hom2dIdenty,PointX,PointY,&hv_X1,&hv_Y1);
		//3,计算定位点在机械坐标系中的位置.
		 // AffineTransPoint2d(CVisionProcess::m_CamerasHomMat2D[CamIndex][AxisIndex][ToolIndex],hv_X1,hv_Y1,&hv_X2,&hv_Y2);
	//获取绝对坐标。
		//2，旋转后的位置就是目标位置，就是机械坐标系的绝对坐标。
    //获取相对坐标。

    //返回值的三种情况，（无论如何旋转角度是一定需要给运动控制的）。
		//第2，3两种情况原理一样，建议运动使用第2种，更通用些。
	HTuple hv_Hom2dNoMove;
	HomMat2dIdentity(&hv_Hom2dNoMove);
	HomMat2dCompose(hv_Hom2dIdenty,CVisionProcess::m_CamerasHomMat2D[CamIndex][AxisIndex][ToolIndex],&hv_Hom2dNoMove);
	switch (ResType)
	{
	case 0:
		{
		//1，不用考虑机械旋转，直接给出实际绝对坐标和旋转角度，运动控制自己考虑旋转的偏心计算旋转后的位置。
			 AffineTransPoint2d(hv_Hom2dNoMove,PointX,PointY,&hv_ReX,&hv_ReY);
		}
		 break;
	case 1:
		{
		//2，考虑旋转，返回旋转后的实际绝对坐标（位移）和旋转角度，运动直接走坐标值和旋转角度，不计算。
			 HTuple hv_Hom2dMove;
			 HomMat2dIdentity(&hv_Hom2dMove);
			 HomMat2dCompose(hv_Hom2dTool,hv_Hom2dNoMove,&hv_Hom2dMove);
			 AffineTransPoint2d(hv_Hom2dMove,0,0,&hv_ReX,&hv_ReY);
		}
		 break;
	case 2:
		{
		//3，相对运动，返回旋转后对应于初始位置的偏移量（位移）和角度，运动直接走偏移量和旋转角度，不计算。
			PointX = 0.0; //暂时未实现，为保险将其置零。
			PointY = 0.0;  //暂时未实现，为保险将其置零。
		}
		break;
	}
	PointX = hv_ReX.D();
	PointY = hv_ReY.D();

}



void CVisionServer::SetCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double(*pMecAndCam),int MetixCount)
{
	for (int i = 0;i<MetixCount;i++)
	{
		//旋转矩阵放到visionprocess 的HTuple矩阵里面来
		CVisionProcess::m_CamerasHomMat2D[CamIndex][AxisIndex][ToolIndex][i] = pMecAndCam[i+1];
	}
}

void CVisionServer::SetCamRot(int CamIndex,int AxisIndex,int ToolIndex,double(*pCamRot),int MetixCount)
{
	for (int i = 0;i<MetixCount;i++)
	{
		CVisionProcess::m_CamRotToMotorHomMat2D[CamIndex][AxisIndex][ToolIndex][i] = pCamRot[i+1];
	}

}
void  CVisionServer::SetImageMetrix(int CamIndex,int AxisIndex,int ToolIndex,double(*pImageRela),double(*pImageAbs),int MetixCount)
{
	for (int i = 0;i<MetixCount;i++)
	{
		CVisionProcess::m_ImageToMotorRelative[CamIndex][AxisIndex][ToolIndex][i] = pImageRela[i+1];
		CVisionProcess::m_ImageToMotorAbsolute[CamIndex][AxisIndex][ToolIndex][i] = pImageAbs[i+1];

	}

}


bool CVisionServer::addAffineData(const int sceneID, SCENE_PKG *param, AF_POINT apoint, AF_POINT bpoint, int index, bool bIsSave)
{
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    int CameraID = 0;
    int iPos = pVision->FindParamData(param, MAIN_PRO, 0); //标定时一个场景中只标定第一张图像对应的相机
    if (-1 == iPos || iPos >= param->vecBody.size())
    {
        return false;
    }
    iPos = (param->vecBody[iPos][0].iValue / 10000);  //找到对应组，再从info中取对应索引的相机index
    if (iPos >= param->vecInfo.size())
    {
        return false;
    }
    CameraID = param->vecInfo[iPos].CameraIndex;
    return pVision->addAffineData(param->szSceneName, apoint,bpoint, index, CameraID, bIsSave);
}


int CVisionServer::affine_apply(int sceneID, int iCameraID,char *sceneName, AF_POINT &apoint, AF_POINT &stOutPoint)
{
    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    return pVision->affine_apply(iCameraID, sceneName, apoint, stOutPoint);
}


bool  CVisionServer::clearWindow(const  long  winID )
{
	//clear_window(winID);
	HalconCpp::ClearWindow(winID);
	return true;
}

bool  CVisionServer::process(int sceneID,SCENE_PKG *param, CHECK_RESULT &result)
{
    HTuple HomMat2D = -1;
    char szBuf[BUF_SIZE] = {0};
	CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
    
	pVision->process(sceneID,param,result);
	return true;
}

  bool CVisionServer::setpart(long winID,int topx, int topy,int lowx,int lowy)
  {
	  return  CVisionProcess::setpart(winID,topx,topy,lowx,lowy);
	 
  }

  bool CVisionServer::getpart(long winID,int &topx, int &topy,int &lowx,int &lowy)
   {
	   CVisionProcess::getpart(winID,topx,topy,lowx,lowy);
	   return true;
   }

 bool  CVisionServer::getimagesize(int sceneID,int &width, int &height)
  {
	  CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
	  return pVision->getimagesize(width,height); 
  }

 //调试阈值分割参数
 bool  CVisionServer::DebugThreshold(long  halconID, int sceneID, int iMinGray, int iMaxGray)
 {
     CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
     return pVision->DebugThreshold(halconID,iMinGray, iMaxGray);
 }

 void DrawRegionRect(int SenceId)
 {




 }

 bool  CVisionServer::findline(int sceneID,SCENE_PKG *param, int index, ST_LINE &result, bool bFollow)
  {
      bool bRet = false;
	  int iTPos = 0;
	  CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);

      //找到当前直线参数
      int iPos = pVision->FindParamData(param, LINE_PRO, index);
      VIRSION_DATA_PKG *paramD = NULL;
      if (iPos >= param->vecBody.size() || iPos >= param->vecDataType.size())
      {
          return false;
      }
      paramD = param->vecBody[iPos];
      int iLength = paramD[1].iValue;
	  VIRSION_DATA_PKG * pNewparamD = new VIRSION_DATA_PKG[iLength + 2];
	  memcpy(&(*pNewparamD), &paramD[0],sizeof(VIRSION_DATA_PKG)*(iLength + 2));

      unsigned int *pType = param->vecDataType[iPos];
	  if (bFollow)
	  {
		int iGroup = paramD[0].iValue/10000;
		iPos = FindParamData(param, MODEL_PRO, -1, iGroup);
		iTPos = FindSpecTypePos(param->vecBody[iPos], param->vecDataType[iPos], ID_TYPE);
        int iTypeP = FindSpecTypePos(param->vecBody[iPos], param->vecDataType[iPos], SPEC_TYPE);
		ST_RECTANGLE_DATA rtData;
		paramD = param->vecBody[iPos];
        int indexM = (paramD[0].iValue%10000)%100;
		if (iTPos >= 0 && -1 == paramD[iTPos].iValue)  //Model ID
		{
			char szBuf[512] = {0};
            get_module_path((LPSTR)szBuf);
            sprintf_s(szBuf, "%s%s\\build_model%d", szBuf,param->szSceneName, indexM);
            int iType = paramD[iTypeP].iValue;
			paramD[iTPos].iValue = readModel(sceneID, param->szSceneName, szBuf, indexM, iType); 
		}

		findModel(sceneID, indexM,param, rtData, 0);

		paramD = param->vecBody[iPos];
		iTPos = FindSpecTypePos(param->vecBody[iPos], param->vecDataType[iPos], ROI_TYPE);
		HTuple HomMat2D, RowTrans, ColTrans;

	    //VectorAngleToRigid(BeginRow,BeginCol,BeginAng,EndRow,EndCol,EndAng,&HomMat2dID);
	//	vector_angle_to_rigid(paramD[iTPos + 5].dValue, paramD[iTPos + 6].dValue, paramD[iTPos + 7].dValue, rtData.row, rtData.col, rtData.angle, &HomMat2D);
		//affine_trans_pixel(HomMat2D,HTuple(pNewparamD[2].dValue),HTuple(pNewparamD[3].dValue),&RowTrans,&ColTrans);
		pNewparamD[2].dValue = RowTrans[0].D();
		pNewparamD[3].dValue = ColTrans[0].D();
		double angleoffset = rtData.angle - paramD[iTPos + 7].dValue;
		pNewparamD[4].dValue = pNewparamD[4].dValue + angleoffset;
		 


	  }
      // 直线查找
      if (-1 != iPos && NULL != pNewparamD)
      {
          bRet = pVision->findline(param->szSceneName, pNewparamD,iLength, pType,result, param->display);
      }
	  delete pNewparamD;
	  return bRet;
  }
  
  bool  CVisionServer::findcircle(int sceneID,SCENE_PKG *param, int index, ST_CIRCLE &result, bool bFollow)
  {
      bool bRet = false;
	  int iTPos = 0;
      CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);

      //找到当前模板参数
      int iPos = pVision->FindParamData(param, CIRCLE_PRO, index);
      VIRSION_DATA_PKG *paramD = NULL;
      if (iPos >= param->vecBody.size() || iPos >= param->vecDataType.size())
      {
          return false;
      }
      paramD = param->vecBody[iPos];
      int iLength = paramD[1].iValue;

	  VIRSION_DATA_PKG * pNewparamD = new VIRSION_DATA_PKG[iLength + 2];
	  memcpy(&(*pNewparamD), &paramD[0],sizeof(VIRSION_DATA_PKG)*(iLength + 2));

      unsigned int *pType = param->vecDataType[iPos];
      //模板查找
	  if (bFollow)
	  {
		  int iGroup = paramD[1].iValue/10000;
		  iPos = FindParamData(param, MODEL_PRO, -1, iGroup);  
		  paramD = param->vecBody[iPos];
          iTPos = FindSpecTypePos(param->vecBody[iPos], param->vecDataType[iPos], ID_TYPE);
          int iTypeP = FindSpecTypePos(param->vecBody[iPos], param->vecDataType[iPos], SPEC_TYPE);
		  ST_RECTANGLE_DATA rtData;
		  int indexM = 0;
		  if (-1 == paramD[iTPos].iValue)  //Model ID
		  {
			  int indexM = (paramD[1].iValue%10000)%100;
              char szBuf[512] = {0};
              get_module_path((LPSTR)szBuf);
              sprintf_s(szBuf, "%s%s\\model%d", szBuf,param->szSceneName, indexM);
              int iType = paramD[iTypeP].iValue;
			  paramD[iTPos].iValue = readModel(sceneID, param->szSceneName, szBuf, index, iType); 
		  }

		  iTPos = FindSpecTypePos(param->vecBody[iPos], param->vecDataType[iPos], ROI_TYPE);
		  findModel(sceneID, indexM,param, rtData, 0);

		  HTuple HomMat2D, RowTrans, ColTrans;
		//  vector_angle_to_rigid(paramD[iTPos + 5].dValue, paramD[iTPos + 6].dValue, paramD[iTPos + 7].dValue, rtData.row, rtData.col, rtData.angle, &HomMat2D);
		//  affine_trans_pixel(HomMat2D,HTuple(pNewparamD[2].dValue),HTuple(pNewparamD[3].dValue),&RowTrans,&ColTrans);
		  pNewparamD[2].dValue = RowTrans[0].D();
		  pNewparamD[3].dValue = ColTrans[0].D();
		  double angleoffset = rtData.angle - paramD[iTPos + 7].dValue;
		  pNewparamD[6].dValue = pNewparamD[6].dValue + angleoffset;
		  pNewparamD[7].dValue = pNewparamD[7].dValue + angleoffset;
	  }
      if (-1 != iPos && NULL != pNewparamD)
      {
          bRet = pVision->findcircle(param->szSceneName, pNewparamD,iLength, pType,result, param->display);
      }
	  delete pNewparamD;
	  return bRet;
  }
  
  bool  CVisionServer::MeasureCircle(long HwinID,int iSceneID, int Imageindex,int MatchIndex, double& Row,double& Col,double& Rii)
  {
	    CVisionProcess *pVision = NULL;
		pVision = CVisionServer::GetInstance(iSceneID);
	    if(pVision)
		{
          return   pVision->MeasureCircle(HwinID,Imageindex,MatchIndex,Row,Col,Rii);
		}
		return false;

  }



//获取对应类型参数在pkg->pBody中的位置
//input 类型， 索引， pkg, 
//返回找到位置
int CVisionServer::FindParamData(SCENE_PKG *pkg, int iType, int index, int iGroup)
{
    return CVisionProcess::FindParamData(pkg, iType, index, iGroup);
}


int CVisionServer::FindSpecTypePos(const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iType)
{
    return CVisionProcess::FindSpecTypePos(pkg, pType, iType);
}



bool CVisionServer::HalconProcedureLocate(long HwinID,int iSceneID, int Imageindex,string ProcedurePath,
	double (*inParam)[HalProcInParamNum], double& PosX,double& PosY,double& PosPhi)
{
	CVisionProcess *pVision = NULL;
	bool res = false;
	pVision = CVisionServer::GetInstance(iSceneID);
	if(pVision)
	{
		res =  pVision->HalconProcedureLocate(HwinID,Imageindex,ProcedurePath,inParam,PosX,PosY,PosPhi);

	}
	return res;
}

bool CVisionServer::HalconProcedureProcess(long HwinID,int iSceneID,int Imageindex,string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage)
{
	CVisionProcess *pVision = NULL;
	bool res = false;
	pVision = CVisionServer::GetInstance(iSceneID);
	if(pVision)
	{
		res =  pVision->HalconProcedureProcess(HwinID,Imageindex,ProcedurePath,inparam,outparam,display,error,hmessage);

	}
	return res;
}
bool CVisionServer::ImageEnhancement(long iWinID,int iSceneID,int Imageindex,double& multinum,double& addnum,int display,int &error,string &hmessage)
{
	CVisionProcess *pVision = NULL;
	bool res = false;
	pVision = CVisionServer::GetInstance(iSceneID);
	if(pVision)
	{
		res =  pVision->ImageEnhancement(iWinID,Imageindex,multinum,addnum,display,error,hmessage);

	}
	return res;
}
