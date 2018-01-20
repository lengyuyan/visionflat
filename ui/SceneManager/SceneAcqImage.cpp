#include "StdAfx.h"
#include "SceneAcqImage.h"


SceneAcqImage::SceneAcqImage(void)
{
	//m_ProcTypeId = IMAGE_ACQISITION;
	BuildInstance();
}

//注意这里会初始化基类
SceneAcqImage::SceneAcqImage(CString ConfigPath,int ProcID):VisionProcBase(ConfigPath,ProcID)
{
	//m_ProcTypeId = IMAGE_ACQISITION;
	BuildInstance();
}


SceneAcqImage::~SceneAcqImage(void)
{
	m_SceneAcqImageConfig.CloseSettingDlg();
}

 void SceneAcqImage::BuildInstance(void)   
 {
	 m_ProcTypeId = IMAGE_ACQISITION;
	 m_CamIndex = 0;
	 m_SceneAcqImageConfig.m_ProcProcess = 0;//标识用的新方法,这里默认用新方法0
	 m_SceneAcqImageConfig.GetContainer(this);
	 ReadProcConfig(m_ProcNo);
	// m_SceneAcqImageConfig.m_hWnd = (HWND)-1;
	 
 }

 void SceneAcqImage::InitInstance(CWnd* pParentWnd)
 {
	 CRect rtS;
	 if (NULL == pParentWnd)
	 {
		 return;
	 }
	 m_pParentWnd = pParentWnd;
	 assert(pParentWnd->IsKindOf( RUNTIME_CLASS(CWnd)));
	 m_pParentWnd->GetClientRect(&rtS);
	 m_SceneAcqImageConfig.Create(IDD_DIALOG_SCENE_IMAGE, m_pParentWnd);
	 m_SceneAcqImageConfig.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
	 m_SceneAcqImageConfig.ShowWindow(SW_HIDE);
	
	 m_SceneAcqImageConfig.UpdateUI();
	 m_SceneInitialCount = g_SceneInitialCount;

 }

 void SceneAcqImage::DeleteSelf()
 {
	 delete this;

 }


void SceneAcqImage::HideConfigWnd()
{	
	if(g_SceneInitialCount==m_SceneInitialCount)
		m_SceneAcqImageConfig.ShowWindow(SW_HIDE);
}

void SceneAcqImage::ShowConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
		m_SceneAcqImageConfig.ShowWindow(SW_SHOW);

}

bool SceneAcqImage::ProcGrabImage(bool sendflag)
{
	return m_SceneAcqImageConfig.ConfigGrabImage(sendflag);

}

 void SceneAcqImage::SaveProcConfig(int ProcIndex)
 {
	 VisionProcBase::SaveProcConfig(ProcIndex);
	 //添加子类代码

	 m_SceneAcqImageConfig.SaveProcConfig(m_ConfigPath,ProcIndex);

 }
 //1,采集类本身读取相机序号
 //2,基类读取

 void SceneAcqImage::ReadProcConfig(int ProcIndex)
 {
	
	 CString ProcIndexName;
	 ProcIndexName.Format("%d",ProcIndex);
	 m_CamIndex = GetPrivateProfileInt(ProcIndexName, "CamIndex", -1, m_ConfigPath);
	 m_CurSceneCamIndex = m_CamIndex;
	 //基类
	 VisionProcBase::ReadProcConfig(ProcIndex);
   //添加子类代码
	 m_SceneAcqImageConfig.ReadProcConfig(m_ConfigPath,ProcIndex);
	 m_SceneAcqImageConfig.m_CameraIndex = m_CamIndex;

	  
 }

 BOOL SceneAcqImage::VisionProcRun(CHECK_RESULT& ProcRes)
 {
	 bool res = false;
	if(m_SceneAcqImageConfig.VisionProcRun(ProcRes))
	{ 
		ProcRes.err = 0;
		res = true;
	}else
	{
		ProcRes.err =  IMAGE_ACQISITION ;//错误在抓图这项
		res = false;
	}
	SYSTEM_PARAM *pSysParam = (SYSTEM_PARAM *)GetSysParamPoint();
	if (pSysParam->IsSimulate)//若为模拟测试,则反true
	{
		res = true;
	}
	return res;	
}