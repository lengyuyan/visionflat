#include "StdAfx.h"
#include "HalProcPosition.h"
#include "SceneAcqImage.h"

HalProcPosition::HalProcPosition(void)
{
		BuildInstance();
}

HalProcPosition::HalProcPosition(CString ConfigPath,int ProcID):VisionProcBase(ConfigPath,ProcID)
{
		BuildInstance();
}

HalProcPosition::~HalProcPosition(void)
{
	m_HalCommomPosSet.CloseSettingDlg();
}


void HalProcPosition::BuildInstance(void)
{
	CString szTemp;
	m_ProcTypeId = HALPROC_POSITIONG;  
	m_HalCommomPosSet.m_ProcProcess = 0;
	m_HalCommomPosSet.GetContainer(this);
	ReadProcConfig(m_ProcNo);

}


void HalProcPosition::InitInstance(CWnd* pParentWnd)
{	
	CRect rtS;
	if (NULL == pParentWnd)
	{
		return;
	}
	m_pParentWnd = pParentWnd;

	assert(pParentWnd->IsKindOf( RUNTIME_CLASS(CWnd)));
	m_pParentWnd->GetClientRect(&rtS);
	m_HalCommomPosSet.Create(IDD_HALPROC_LOCATION, m_pParentWnd);
	m_HalCommomPosSet.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
	m_HalCommomPosSet.ShowWindow(SW_SHOW);
	m_HalCommomPosSet.UpdateUI();


	m_SceneInitialCount = g_SceneInitialCount;

}

void HalProcPosition::DeleteSelf()
{
	delete this;

}


void HalProcPosition::HideConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
	{
		m_HalCommomPosSet.ShowWindow(SW_HIDE);
	}
}

void HalProcPosition::ShowConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
		m_HalCommomPosSet.ShowWindow(SW_SHOW);
}


void HalProcPosition::SaveProcConfig(int ProcIndex)
{
	VisionProcBase::SaveProcConfig(ProcIndex);
	//添加子类代码

	m_HalCommomPosSet.SaveProcConfig(m_ConfigPath,ProcIndex);

}

void HalProcPosition::ReadProcConfig(int ProcIndex)
{
	VisionProcBase::ReadProcConfig(ProcIndex);
	//添加子类代码


	m_HalCommomPosSet.ReadProcConfig(m_ConfigPath,ProcIndex);

}

bool HalProcPosition::CommonLocate(CString pathName,double (*inParam)[12],
	double& PosX,double& PosY,double& PosPhi)
{
	int ImageIndex = g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcImageIndex(GetProcId());
	SceneAcqImage* pAcqImage = dynamic_cast<SceneAcqImage*>(g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcByTypeIndex(IMAGE_ACQISITION,ImageIndex));
	int CamIndex = pAcqImage->m_CamIndex; //相机索引
	long winId = -1;

	if(g_SceneInitialCount==m_SceneInitialCount&&g_SceneInitialCount!=0)
	{
		winId = g_SceneDlgHwinId;
	}else
	{
		SYSTEM_PARAM pSysParam;
		GetSysParam(&pSysParam);
		vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(pSysParam.MainImageWndVec));
		if (CamIndex < pImageWnd->size())
		{
			winId = (long)(pImageWnd->at(CamIndex)->m_hWnd);//显示在相机对应的窗口
		}else
		{
			winId = (long)(pImageWnd->at(0)->m_hWnd);//显示在第一窗口
		}
	} 

	return HalconProcedureLocate(winId,GetSceneId(),ImageIndex,pathName.GetBuffer(),inParam,PosX,PosY,PosPhi);
	
}

BOOL HalProcPosition::VisionProcRun(CHECK_RESULT& ProcRes)
{
	bool ResRun = false;
	//double RotPhi,MoveX,MoveY;
	SYSTEM_PARAM sysParam;
	GetSysParam(&sysParam);
	SCENE_PKG config;
	GetSceneParam(GetSceneId(),&config);
	int AxisIndex = config.AxisIndex;
	if (AxisIndex<0)
	{
		AfxMessageBox("轴参数配置错误");
		return false;
	}
	int imageIndex = (g_SceneList.GetSceneBySceneID(GetSceneId()))->GetProcImageIndex(GetProcId());
	SceneAcqImage* pAcqImage = dynamic_cast<SceneAcqImage*>(g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcByTypeIndex(IMAGE_ACQISITION,imageIndex));
	int CamIndex = pAcqImage->m_CamIndex; //相机索引

	//3 ，调用函数处理
	if (m_HalCommomPosSet.VisionProcRun(ProcRes))
	{
		ResRun = true;
	}else
	{
		ProcRes.err = HALPROC_POSITIONG;
		ResRun = false;

	}

	//4，将需要改变的值放入结果数据中。
	//显示结果
	vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(sysParam.MainImageWndVec));
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
		SetVisionColorHeight(HalconWinID,"green");
		ShowVisionString(HalconWinID," Hal定位OK");

	}else 
	{
		SetVisionColorHeight(HalconWinID,"red");
		ShowVisionString(HalconWinID," Hal定位NG");
	}

	//如果只是跑流程，则返回OK
	if (sysParam.IsSimulate)
	{
		//int ResIndex = ProcRes.iCount -3 ;
		ResRun = true;
		
	}

	//int BeginIndex = ProcRes.iCount ;
	//ProcRes.iCount+=3;
	//strcpy(ProcRes.szValue[0],ProcRes.szValue[0]);
	//strcpy(ProcRes.szValue[1],ProcRes.szValue[1]);
	//strcpy(ProcRes.szValue[2],ProcRes.szValue[2]);
	//strcpy(ProcRes.szValue[3],ProcRes.szValue[3]);
	//ProcRes.iCount = 4;


	return ResRun;

}
