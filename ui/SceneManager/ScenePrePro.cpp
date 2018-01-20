
#pragma  once

#include "StdAfx.h"
#include "ScenePrePro.h"
#include "SceneAcqImage.h"
#include "io.h"
#include "direct.h"

ScenePrePro::ScenePrePro(void)//无参构造函数无用
{
	BuildInstance();
}
ScenePrePro::ScenePrePro(CString ConfigPath,int ProcID):VisionProcBase(ConfigPath,ProcID)
{
	BuildInstance();
}


ScenePrePro::~ScenePrePro(void)
{
	m_ScenePreProSetting.CloseSettingDlg();

}


void ScenePrePro::BuildInstance(void)
{
	CString szTemp;
	m_ProcTypeId = IMAGE_PREPROCESS;
	m_ScenePreProSetting.m_ProcProcess = 0;
	m_ScenePreProSetting.GetContainer(this);
	ReadProcConfig(m_ProcNo);		
}




void ScenePrePro::InitInstance(CWnd* pParentWnd)
{	
	CRect rtS;
	if (NULL == pParentWnd)
	{
		return;
	}
	m_pParentWnd = pParentWnd;

	assert(pParentWnd->IsKindOf( RUNTIME_CLASS(CWnd)));
	m_pParentWnd->GetClientRect(&rtS);
	m_ScenePreProSetting.Create(IDD_PROPRECESS, m_pParentWnd);
	m_ScenePreProSetting.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
	m_ScenePreProSetting.ShowWindow(SW_SHOW);
	m_ScenePreProSetting.UpdateUI();
	m_SceneInitialCount = g_SceneInitialCount;

}

void ScenePrePro::DeleteSelf()
{
	delete this;

}

void ScenePrePro::HideConfigWnd()
{
    if(g_SceneInitialCount==m_SceneInitialCount)
	{
		m_ScenePreProSetting.ShowWindow(SW_HIDE);
	}
}

void ScenePrePro::ShowConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
		m_ScenePreProSetting.ShowWindow(SW_SHOW);
}

void ScenePrePro::SaveProcConfig(int ProcIndex)
{
	VisionProcBase::SaveProcConfig(ProcIndex);
  //添加子类代码
	//char szValue[256] = {0};
	//CString ProcIndexName;
	//ProcIndexName.Format("%d",ProcIndex);
	//memset(szValue,0,sizeof(szValue));
	//sprintf_s(szValue,"%.6lf",m_CalModRotAng);
	//WritePrivateProfileStringA(ProcIndexName,"CalModRotAng",szValue, m_ConfigPath);	

	 //保存ROI的参数
	m_ScenePreProSetting.SaveProcConfig(m_ConfigPath,ProcIndex);

}









void ScenePrePro::ReadProcConfig(int ProcIndex)
{
	/*VisionProcBase::ReadProcConfig(ProcIndex);*/

	m_ScenePreProSetting.ReadProcConfig(m_ConfigPath,ProcIndex);

}





BOOL ScenePrePro::VisionProcRun(CHECK_RESULT& ProcRes)
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
	if (m_ScenePreProSetting.VisionProcRun(ProcRes))
	{
		ResRun = true;
	}else
	{
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
		ShowVisionString(HalconWinID," 图像增强OK");

	}else 
	{
		SetVisionColorHeight(HalconWinID,"red");
		ShowVisionString(HalconWinID," 图像增强NG");
	}

	//如果只是跑流程，则返回OK
	if (sysParam.IsSimulate)
	{
		ResRun = true;

	}

	return ResRun;
}

bool ScenePrePro::ImageEnhancement(double& multinum,double& addnum)
{
	bool ResRun = false;
	int error = 0;
	bool display = true; //是否把处理子项显示在窗口,这个后续把他作为一个基类的成员，每个子类都要有
	string hmessage = "OK";
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

	ResRun = IImageEnhancement(winId,GetSceneId(),ImageIndex,multinum,addnum,display,error,hmessage);


	return ResRun;
}



