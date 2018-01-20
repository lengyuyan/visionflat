
#pragma  once

#include "StdAfx.h"
#include "SceneMatchModel.h"
#include "SceneAcqImage.h"
#include "io.h"
#include "direct.h"

SceneMatchModel::SceneMatchModel(void)
{
	 //m_ProcTypeId = MODEL_PRO;
	BuildInstance();
}
SceneMatchModel::SceneMatchModel(CString ConfigPath,int ProcID):VisionProcBase(ConfigPath,ProcID)
{
	BuildInstance();
}

//SceneMatchModel::SceneMatchModel(CWnd*pParentWnd):VisionProcBase(pParentWnd)
//{
//
//}

SceneMatchModel::~SceneMatchModel(void)
{
	m_SceneModelSetting.CloseSettingDlg();

}




void SceneMatchModel::BuildInstance(void)
{
		CString szTemp;
		m_CalModRotAng = 0.0;
	    m_ProcTypeId = MODEL_PRO;
		m_SceneModelSetting.m_ProcProcess = 0;
		m_SceneModelSetting.GetContainer(this);
		ReadProcConfig(m_ProcNo);
		
		//得到模板路径名称
		//char ScenePath[256] = {0};
		memset(m_ModelName,0,sizeof(m_ModelName));
		memcpy(m_ModelName, GetScenePath(GetSceneId()),sizeof(m_ModelName));
		if(-1 == access(m_ModelName,0))
			    _mkdir(m_ModelName);
		strcat_s(m_ModelName,"\\");
		szTemp.Format("Scene%d_shapeMod_%d.mod",GetSceneId(),m_ProcNo);
		strcat_s(m_ModelName,szTemp);
		
		
}




void SceneMatchModel::InitInstance(CWnd* pParentWnd)
{	
	CRect rtS;
	if (NULL == pParentWnd)
	{
		return;
	}
	m_pParentWnd = pParentWnd;

	assert(pParentWnd->IsKindOf( RUNTIME_CLASS(CWnd)));
	m_pParentWnd->GetClientRect(&rtS);
	m_SceneModelSetting.Create(IDD_DIALOG_SCENE_MODEL, m_pParentWnd);
	m_SceneModelSetting.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
	m_SceneModelSetting.ShowWindow(SW_SHOW);
	m_SceneModelSetting.UpdateUI();
	//m_SceneModelSetting.GetContainer(this);

	m_SceneInitialCount = g_SceneInitialCount;

}

void SceneMatchModel::DeleteSelf()
{
	delete this;

}

void SceneMatchModel::HideConfigWnd()
{
    if(g_SceneInitialCount==m_SceneInitialCount)
	{
		m_SceneModelSetting.ShowWindow(SW_HIDE);
	}
}

void SceneMatchModel::ShowConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
		m_SceneModelSetting.ShowWindow(SW_SHOW);
}

void SceneMatchModel::SaveProcConfig(int ProcIndex)
{
	VisionProcBase::SaveProcConfig(ProcIndex);
  //添加子类代码
	char szValue[256] = {0};
	CString ProcIndexName;
	ProcIndexName.Format("%d",ProcIndex);
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%.6lf",m_CalModRotAng);
	WritePrivateProfileStringA(ProcIndexName,"CalModRotAng",szValue, m_ConfigPath);	

	 //保存ROI的参数
	m_SceneModelSetting.SaveProcConfig(m_ConfigPath,ProcIndex);

}



void SceneMatchModel::ShowAllShape(void)
{
	OnShowAllRoi(g_SceneDlgHwinId,GetSceneId(),&m_RoiIdList);

}
void SceneMatchModel::CreateModel(int Nevels,double AngleStart,double AngleExtent,int Contrast,double& ModRow,double ModCol,
	double& ModPhi)
{
	//得到处理的图形序号
	 long HalWinId = -1;

	 int ImageIndex = g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcImageIndex(GetProcId());
	 if (FindHalconWinID(g_SceneDlgHwinId,HalWinId))
	 {
	      CreateWay2VisionModel(HalWinId,GetSceneId(),m_RoiID,m_ModelName,Nevels,AngleStart,AngleExtent,Contrast,ImageIndex);
     }

	 //新创建匹配的数据保存到配置文件中。
	 
	double Score = 0.5,Row,Col,phi ;
	bool IsBuild = true ; //借用参数IsBuild来判断是否是新建匹配

	//获取  匹配序号
	int ProcIndex = GetProcTypeIndex();


	// Row = GetProcIndexById(GetProcId());
	MatchModel(m_SceneModelSetting.m_MatchLevel, m_SceneModelSetting.m_MatchNum,Score,IsBuild,Row,Col,phi);
    
	char szName[256] ={0};
	char szValue[256]={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",GetProcIndexById(GetProcId()));
	//存储模板类型
	memset(szName,0,sizeof(szValue));
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szName,"Match%d_OriRow",GetProcIndexById(GetProcId()));	
	sprintf_s(szValue,"%.6lf",Row);
	WritePrivateProfileStringA(ProcIndexName,szName,szValue, m_ConfigPath);

	memset(szName,0,sizeof(szValue));
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szName,"Match%d_OriCol",GetProcIndexById(GetProcId()));	
	sprintf_s(szValue,"%.6lf",Col);
	WritePrivateProfileStringA(ProcIndexName,szName,szValue, m_ConfigPath);

	memset(szName,0,sizeof(szValue));
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szName,"Match%d_OriPhi",GetProcIndexById(GetProcId()));	
	sprintf_s(szValue,"%.6lf",phi);
	WritePrivateProfileStringA(ProcIndexName,szName,szValue, m_ConfigPath);	
}

BOOL  SceneMatchModel::MatchModel(int PyramidLevel,int MatchNum,double& Score,bool IsBulid,double& row,double& col,double& phi)
{
	 int ImageIndex = g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcImageIndex(GetProcId());
	 int ProcIndex  = GetProcTypeIndex();//看他本身是处理表中同类项的第几项
	 SceneAcqImage* pAcqImage = dynamic_cast<SceneAcqImage*>(g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcByTypeIndex(IMAGE_ACQISITION,ImageIndex));
	 int CamIndex = pAcqImage->m_CamIndex; //相机索引
	
	 long HalWinId = -1;
	 if(g_SceneInitialCount==m_SceneInitialCount&&g_SceneInitialCount!=0)
	 {
		 FindHalconWinID(g_SceneDlgHwinId,HalWinId);
	 }else
	 {
		 SYSTEM_PARAM pSysParam;
		 GetSysParam(&pSysParam);
		 vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(pSysParam.MainImageWndVec));
		 if (CamIndex < pImageWnd->size())
		 {
			 FindHalconWinID((long)(pImageWnd->at(CamIndex)->m_hWnd),HalWinId);//显示在相机对应的窗口

		 }else
		 {
			  FindHalconWinID((long)(pImageWnd->at(0)->m_hWnd),HalWinId);//显示在第一窗口
		 }

	}
	 if (-1 == HalWinId )
	 {
		 return false;
	 }


    if(!FindWay2VisionModel(GetSceneId(),HalWinId,ImageIndex,m_ModelName,PyramidLevel,MatchNum,Score,ProcIndex,IsBulid,row,col,phi))
	{
		return false;
	}
	
	return true;

}


void SceneMatchModel::ReadProcConfig(int ProcIndex)
{
	VisionProcBase::ReadProcConfig(ProcIndex);
	//添加子类代码
	char szValue[256] = {0};
	CString ProcIndexName,strTemp;
	int CombShapeNum = 0;
	int ShapeId = -1;
	ST_ROI_INFO RoiInfor;
	ProcIndexName.Format("%d",ProcIndex);
	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA(ProcIndexName, "CalModRotAng", "",szValue,sizeof(szValue), m_ConfigPath);
	m_CalModRotAng = atof(szValue);
	m_SceneModelSetting.ReadProcConfig(m_ConfigPath,ProcIndex);

}





BOOL SceneMatchModel::VisionProcRun(CHECK_RESULT& ProcRes)
{
	bool ResRun = false;
	SCENE_PKG *config = NULL;
	double CamRotPhi = 0.0,MoveX = 0.0,MoveY =0.0;
	SYSTEM_PARAM *sysParam = NULL;
	sysParam = (SYSTEM_PARAM *)GetSysParamPoint();
	if (NULL == sysParam)
	{
		AfxMessageBox("系统参数获取失败!");
		return false;
	}
	config = (SCENE_PKG *)GetSceneParamHandle(GetSceneId());
	int AxisIndex = config->AxisIndex;
	if (AxisIndex<0)
	{
		AfxMessageBox("轴参数配置错误");
		return false;
	}
	int imageIndex = (g_SceneList.GetSceneBySceneID(GetSceneId()))->GetProcImageIndex(GetProcId());
	SceneAcqImage* pAcqImage = dynamic_cast<SceneAcqImage*>(g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcByTypeIndex(IMAGE_ACQISITION,imageIndex));
	int CamIndex = pAcqImage->m_CamIndex; //相机索引
	int LastId = -1,NextId = -1;
	double SceneRotPhi = 0.0;

	ProcList* pScene = g_SceneList.GetSceneBySceneID(GetSceneId());
	if (pScene)
	{
		LastId = pScene->GetRelaSceneID();
		NextId = pScene->GetNextSceneID();
		SceneRotPhi = pScene->GetNextRotAngle()*31.1416/180;
	}

	try
	{
		if(m_SceneModelSetting.VisionProcRun(ProcRes))//到设置里面获得了结果,结果正确才会走进if
		{
			if (IMAGE_ABS == m_ResultType || MODLE_DISTENCE == m_ResultType)//返回坐标值是相对值还是绝对值
			{
				ProcRes.resultType = 0;
			}
			else
			{
				ProcRes.resultType = 1;//1为相对坐标
			}//标定场景走不进来,返回的是像素坐标
			if (m_ResultType > 0) //如下的坐标调整计算，如果没有标定相机和夹具，计算使用的各个值是初值，计算将是错误的。
			{		
				int ToolIndex = config->MotorToolIndex; //夹具索引							
				CamRotPhi =  atof(ProcRes.szValue[3]);  //第3个数为角度
				double RotAxeX = sysParam->RotX[AxisIndex][ToolIndex] + sysParam->RotX_Adjust[AxisIndex][ToolIndex];//用不到
				double RotAxeY = sysParam->RotY[AxisIndex][ToolIndex] + sysParam->RotY_Adjust[AxisIndex][ToolIndex];;
				bool AxeMoveRotDirect = sysParam->AxeRotDirect[CamIndex][AxisIndex][ToolIndex];//旋转方向，系统界面上有
				int  ResType = sysParam->PosMoveType[CamIndex][AxisIndex][ToolIndex];//位置类型，绝对坐标还是旋转后绝对坐标,系统界面上有
				double picCentX = sysParam->PicCenterPointX[CamIndex][AxisIndex][ToolIndex]; //中心像素点x
				double picCentY = sysParam->PicCenterPointY[CamIndex][AxisIndex][ToolIndex]; //中心像素点y
				
				//2，需要计算在相机坐标系下位姿还原时，相机坐标系下的相对运动。			
				switch (m_ResultType)
				{
				case REAL_UNIT_RESULT:  //计算与模板对应的相对偏移值，考虑旋转
					{
						MoveAndRotInCamForOriMatch(GetSceneId(),sysParam->CameraPixSize[CamIndex][AxisIndex][ToolIndex],AxeMoveRotDirect,MoveX,MoveY,
							        LastId,NextId,SceneRotPhi,picCentX,picCentY,sysParam->CamParam[CamIndex][AxisIndex][ToolIndex],12,sysParam->CamPose[CamIndex][AxisIndex][ToolIndex],
									sysParam->CalThickNess[CamIndex][AxisIndex][ToolIndex],
									sysParam->RotX[AxisIndex][ToolIndex]+sysParam->RotX_Adjust[AxisIndex][ToolIndex],
									sysParam->RotY[AxisIndex][ToolIndex]+sysParam->RotY_Adjust[AxisIndex][ToolIndex],
									CamRotPhi,m_CalModRotAng*3.1416/180.0);	//
					}
					break;
				case NO_MATCH_ROT: //计算与模板对应的相对偏移值,不考虑旋转
					{
						CamRotPhi = 0.0;
						//参数,场景id,像素大小,旋转方向,模板x,y，上一个场景id，下一个场景id,下一个场景旋转角度,相机内参,相机外参,厚度,旋转中心x，y,z,此次检测角度跟初次角度的差,模板角度(),相机,轴,工件索引
						//MoveAndRotInCamForOriMatch(GetSceneId(),sysParam->CameraPixSize[CamIndex][AxisIndex][ToolIndex],AxeMoveRotDirect,MoveX,MoveY,
						//	LastId,NextId,SceneRotPhi,picCentX,picCentY,sysParam->CamParam[CamIndex][AxisIndex][ToolIndex],12,sysParam->CamPose[CamIndex][AxisIndex][ToolIndex],
						//	sysParam->CalThickNess[CamIndex][AxisIndex][ToolIndex]);
						//模板移动了多少，他就移动多少
						MoveX = atof(ProcRes.szValue[1]) *sysParam->CameraPixSize[CamIndex][AxisIndex][ToolIndex];
						MoveY = atof(ProcRes.szValue[2]) *sysParam->CameraPixSize[CamIndex][AxisIndex][ToolIndex];


					}
					break;
				case TEST_REPOSITION: //计算与模板对应的相对偏移值,不考虑旋转//测试重复定位  IMAGE_RELA   图像坐标标定，相对坐标
					{
		
						MoveAndRotInCamForOriMatch(GetSceneId(),2,AxeMoveRotDirect,MoveX,MoveY,
							LastId,NextId,SceneRotPhi,picCentX,picCentY,sysParam->CamParam[CamIndex][AxisIndex][ToolIndex],12,sysParam->CamPose[CamIndex][AxisIndex][ToolIndex],
							sysParam->CalThickNess[CamIndex][AxisIndex][ToolIndex],
							sysParam->RotX[AxisIndex][ToolIndex]+sysParam->RotX_Adjust[AxisIndex][ToolIndex],
							sysParam->RotY[AxisIndex][ToolIndex]+sysParam->RotY_Adjust[AxisIndex][ToolIndex],
							CamRotPhi,m_CalModRotAng*3.1416/180.0,CamIndex,AxisIndex,ToolIndex);
					}
					break;
				case IMAGE_ABS: //计算与模板对应的相对偏移值,不考虑旋转
					{
						MoveAndRotInCamForOriMatch(GetSceneId(),3,AxeMoveRotDirect,MoveX,MoveY,LastId,NextId,SceneRotPhi,picCentX,picCentY,
							sysParam->CamParam[CamIndex][AxisIndex][ToolIndex],12,sysParam->CamPose[CamIndex][AxisIndex][ToolIndex],
							sysParam->CalThickNess[CamIndex][AxisIndex][ToolIndex],
							sysParam->RotX[AxisIndex][ToolIndex]+sysParam->RotX_Adjust[AxisIndex][ToolIndex],
							sysParam->RotY[AxisIndex][ToolIndex]+sysParam->RotY_Adjust[AxisIndex][ToolIndex],
							CamRotPhi,m_CalModRotAng*3.1416/180.0,CamIndex,AxisIndex,ToolIndex);
						
					}
					break;
				case MODLE_DISTENCE: //针对机械手，计算绝对坐标值。
					{
						//MoveAndRotInCamForOriMatch(GetSceneId(),sysParam->CameraPixSize[CamIndex][ToolIndex],MoveX,MoveY);
						int ResCount = ProcRes.iCount; 
						MoveX = sysParam->CameraPixSize[CamIndex][AxisIndex][ToolIndex]*atof(ProcRes.szValue[1]);
						MoveY = sysParam->CameraPixSize[CamIndex][AxisIndex][ToolIndex]*atof(ProcRes.szValue[2]);
						//如果相机的位置与标定时的初始位有移动变化，需要计算这个变化。 
						double CurCamX = 0.0;
						double CurCamY = 0.0;
						double CurCamPhi = 0.0;

						double OriCamX  = sysParam->CamCalOriX[CamIndex][AxisIndex][ToolIndex];
						double OriCamY  = sysParam->CamCalOriY[CamIndex][AxisIndex][ToolIndex];
						double OriCamPhi  = sysParam->CamCalOriPhi[CamIndex][AxisIndex][ToolIndex];

						if (1) //相机实际拍照位置与建立模板的位置一致。
						{
							CurCamX = OriCamX;
							CurCamY = OriCamY;
							CurCamPhi = OriCamPhi;
						}else
						{
							//相机实际拍照位置与建立模板的位置不一致，需要通信协议给出实际拍照位置，待实现。
						 
						}
						double TcpRot = 0.0;
						if (AxeMoveRotDirect)//true = 相反
						{
							TcpRot =  -CamRotPhi; //取负是为了纠正回到原来模板的角度（与相机平行）。
						}else
						{
							TcpRot = CamRotPhi;
						}

						MoveX = sysParam->ToolCalOriX[CamIndex][AxisIndex][ToolIndex];//TCP Mod 
						MoveY = sysParam->ToolCalOriY[CamIndex][AxisIndex][ToolIndex];
						//参数为:相机轴工件索引,相机像素大小，角度 第一个点 是否用标定 厚度,内参12 外参 
						AffToRobotWorld(GetSceneId(),CamIndex,AxisIndex,ToolIndex,sysParam->CameraPixSize[CamIndex][AxisIndex][ToolIndex],
							TcpRot,MoveX,MoveY,sysParam->CalWithCamParam[CamIndex][AxisIndex][ToolIndex],sysParam->CalThickNess[CamIndex][AxisIndex][ToolIndex],
							sysParam->CamParam[CamIndex][AxisIndex][ToolIndex],12,sysParam->CamPose[CamIndex][AxisIndex][ToolIndex]);

					}
					break;
					

				}
				//3,将相机系中的移动变换到机械系中，这个变换，使用的矩阵只是旋转矩阵，不带移动。
				if (REAL_UNIT_RESULT == m_ResultType)
				{
					AffinePointByCamInMecRot(CamIndex,AxisIndex,ToolIndex,MoveX,MoveY);
					MoveX = -MoveX;
					MoveY = -MoveY;
					
				}

				//4，将需要改变的值放入结果数据中，对于m_SceneModelSetting对象中使用的结果数据参数，此处最好不要覆盖，而应使用其他参数。
				if (AxeMoveRotDirect)//true = 相反
				{
					CamRotPhi = CamRotPhi;
				}else
				{
					CamRotPhi = -CamRotPhi;
				}
				//转换后的结果

				sprintf(ProcRes.szValue[1],"%0.4lf",MoveX);
				sprintf(ProcRes.szValue[2],"%0.4lf",MoveY);
				double angleA = CamRotPhi*180/3.1415926;
				if (angleA>=180.000)
				{
					angleA -= 180.000;
				}
				sprintf(ProcRes.szValue[3],"%0.4lf",angleA);	
	
			}	
			ResRun = true;
		}else
		{
			 ProcRes.err = MODEL_PRO;
			 ResRun = false;
		}
	}
	catch(...)
	{
		ResRun = false;
	}

	vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(sysParam->MainImageWndVec));
	long HalconWinID = -1;
	if(-1 == g_SceneDlgHwinId)
	{
		FindHalconWinID((int)(pImageWnd->at(CamIndex)->m_hWnd),HalconWinID);
	}
	else
	{
		FindHalconWinID(g_SceneDlgHwinId,HalconWinID);
	}

	if (ResRun)
	{
		SetVisionColorHeight(HalconWinID,"green");//设置颜色,默认字体
		ShowVisionString(HalconWinID,"匹配 OK");//默认在0,0位置写内容

	}else 
	{
		SetVisionColorHeight(HalconWinID,"red");
		ShowVisionString(HalconWinID,"匹配 NG");
	}

	//如果只是跑流程，则返回OK
	if (sysParam->IsSimulate)
	{
		ResRun = true;
	}

	return ResRun;
}



