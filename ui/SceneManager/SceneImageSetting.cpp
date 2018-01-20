// SceneImageSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneManager.h"
#include "SceneImageSetting.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"
#include "SceneAcqImage.h"
// CSceneImageSetting 对话框

IMPLEMENT_DYNAMIC(CSceneImageSetting, CDialogEx)

CSceneImageSetting::CSceneImageSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneImageSetting::IDD, pParent)
{
    m_iCurSceneID = 0;
    m_iCurIndex = 0;
	m_ShutterTime = 15000;
	m_Gain  = 10;
	m_LightIndex = 0;
	m_LightValue  = 12;
	m_CameraIndex = 0;
	m_ProcProcess = -1;
	m_Interval = 200; //拍照时间隔时间为200
}



CSceneImageSetting::~CSceneImageSetting()
{
	
	
	
}

void CSceneImageSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSceneImageSetting, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SETTING, &CSceneImageSetting::OnBnClickedButtonSetting)
    ON_BN_CLICKED(IDC_BTN_LIGHTADD, &CSceneImageSetting::OnBnClickedBtnLightadd)
    ON_BN_CLICKED(IDC_BTN_LIGHTMINUS, &CSceneImageSetting::OnBnClickedBtnLightminus)
    ON_BN_CLICKED(IDC_BTN_GAIN_ADD, &CSceneImageSetting::OnBnClickedBtnGainAdd)
    ON_BN_CLICKED(IDC_BTN_GAIN_MINUS, &CSceneImageSetting::OnBnClickedBtnGainMinus)
END_MESSAGE_MAP()


// CSceneImageSetting 消息处理程序

void CSceneImageSetting::GetContainer(VisionProcBase* pContainer)
{
	m_pContainer =  pContainer;
	m_iCurSceneID = ((SceneAcqImage*)m_pContainer)->GetSceneId();

}



void CSceneImageSetting::UpdateUI()
{
	CString strBuf;	
	SetBackgroundColor(BACKGROUND_COLOR_M);
	strBuf.Format("%d", m_CameraIndex);
	GetDlgItem(IDC_EDIT1)->SetWindowText(strBuf);
	strBuf.Format("%d", m_LightIndex);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strBuf);
	strBuf.Format("%d", m_ShutterTime);
	GetDlgItem(IDC_EDIT_SHUTTER)->SetWindowText(strBuf);
	strBuf.Format("%d",m_Gain);
	GetDlgItem(IDC_EDIT_GAIN)->SetWindowText(strBuf);
	strBuf.Format("%d",m_Interval);
	GetDlgItem(IDC_EDIT_TIMEEPLASE)->SetWindowText(strBuf);
	
}

void CSceneImageSetting::CloseSettingDlg()
{
	OnOK();
}



void CSceneImageSetting::UpdateUI(int iSceneID, int index)
{
	
    m_iCurSceneID = iSceneID;
    m_iCurIndex = index;
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    SetBackgroundColor(BACKGROUND_COLOR_M);
    GetSceneParam(iSceneID,&config);
	
	if (index >= config.vecInfo.size())
	{
		AfxMessageBox("索引超出范围");
		return;
	}
	strBuf.Format("%d", config.vecInfo[index].CameraIndex);
	GetDlgItem(IDC_EDIT1)->SetWindowText(strBuf);
	strBuf.Format("%d", config.vecInfo[index].LightIndex);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strBuf);
	strBuf.Format("%d", config.vecInfo[index].CameraShutter);
	GetDlgItem(IDC_EDIT_SHUTTER)->SetWindowText(strBuf);
	strBuf.Format("%d", config.vecInfo[index].CameraGain);
	GetDlgItem(IDC_EDIT_GAIN)->SetWindowText(strBuf);
	   
}

void CSceneImageSetting::OnBnClickedButtonSetting()
{
    // TODO: 在此添加控件通知处理程序代码
	CString strBuf;
	if (0 ==m_ProcProcess)
	{	
		CString ProcIndexName, StrTemp,strBuf;

		GetDlgItem(IDC_EDIT1)->GetWindowText(strBuf);
		m_CameraIndex = atoi(strBuf);
		GetDlgItem(IDC_EDIT2)->GetWindowText(strBuf);
		m_LightIndex = atoi(strBuf);
		GetDlgItem(IDC_EDIT_SHUTTER)->GetWindowText(strBuf);
		m_ShutterTime = atoi(strBuf);
		GetDlgItem(IDC_EDIT_GAIN)->GetWindowText(strBuf);
		m_Gain = atoi(strBuf);
		//设置参数生效时间,有些dll只有从视频流中抓取数据，但是相同相机在不同场景，曝光等不一样，视频流中相片不一致
		GetDlgItem(IDC_EDIT_TIMEEPLASE)->GetWindowText(strBuf);
		m_Interval = atoi(strBuf);
		
		SaveProcConfig(m_ConfigPath, m_ProcIndex);//保存到场景配置文件

		SetCamParam(m_CameraIndex,m_ShutterTime,m_Gain);//设置曝光参数到实际相机

		return;
	}
   
}

void CSceneImageSetting::SetDisplayWindowID(int winID)
{
    m_winID = winID;
}

BOOL CSceneImageSetting::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN ) 
    {
        CString strBuf;
        SCENE_PKG config;  //从配置文件中读取图片有关roi信息
        GetSceneParam(m_iCurSceneID,&config);

        if (m_iCurIndex >= config.vecInfo.size())
        {
            AfxMessageBox("索引超出范围");
            return false;
        }
        if (pMsg->hwnd==GetDlgItem(IDC_EDIT1)->m_hWnd)
        {
            GetDlgItem(IDC_EDIT1)->GetWindowText(strBuf);
            config.vecInfo[m_iCurIndex].CameraIndex = atoi(strBuf);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT2)->m_hWnd)
        {
            GetDlgItem(IDC_EDIT2)->GetWindowText(strBuf);
            config.vecInfo[m_iCurIndex].LightIndex = atoi(strBuf);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_SHUTTER)->m_hWnd)
        {
            int  iValue = 0;
            GetDlgItem(IDC_EDIT_SHUTTER)->GetWindowText(strBuf);
            iValue = atoi(strBuf.GetBuffer());
            if (iValue >4095 || m_iCurIndex >= config.imageCount)
            {
                return false;
            }
            config.vecInfo[m_iCurIndex].CameraShutter = iValue;
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_GAIN)->m_hWnd)
        {
            int  iValue = 0;
            GetDlgItem(IDC_EDIT_GAIN)->GetWindowText(strBuf);
            iValue = atoi(strBuf.GetBuffer());
            if (iValue >4095 || m_iCurIndex >= config.imageCount)
            {
                return false;
            }
            config.vecInfo[m_iCurIndex].CameraGain = iValue;
        }
        SetSceneParam(&config);
        return true;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSceneImageSetting::OnBnClickedBtnLightadd()
{
        // TODO: 在此添加控件通知处理程序代码
	CString str;
	int iValue = 0;
	GetDlgItem(IDC_EDIT_SHUTTER)->GetWindowText(str);
	iValue = atoi(str.GetBuffer());
	iValue = iValue + 1;

	if (0 ==m_ProcProcess)
	{	
		m_ShutterTime = iValue;	
	}else
	{
		SCENE_PKG config;
		GetSceneParam(m_iCurSceneID, &config);

		if (iValue >4095 || m_iCurIndex >= config.imageCount)
		{
			AfxMessageBox("未知异常");
			return;
		}
		if (!config.vecInfo.empty())
		{
			if (m_iCurIndex >= config.vecInfo.size())
			{
				AfxMessageBox("索引超出范围");
				return;
			}
			config.vecInfo[m_iCurIndex].CameraShutter = iValue;
		}
		SetSceneParam(&config); 
	}
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_SHUTTER)->SetWindowText(str);
   
}


void CSceneImageSetting::OnBnClickedBtnLightminus()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    SCENE_PKG config;
    int iValue = 0;
	GetDlgItem(IDC_EDIT_SHUTTER)->GetWindowText(str);
	iValue = atoi(str.GetBuffer());
	iValue = iValue - 1;
	if (0 ==m_ProcProcess)
	{	
		m_ShutterTime = iValue;	
	}else
	{
		GetSceneParam(m_iCurSceneID, &config);
		if (iValue < 0 || m_iCurIndex >= config.imageCount)
		{
			return;
		}
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0)
		{
			if (m_iCurIndex >= config.vecInfo.size())
			{
				AfxMessageBox("索引超出范围");
				return;
			}
			config.vecInfo[m_iCurIndex].CameraShutter = iValue;
		}
		SetSceneParam(&config);
	}
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_SHUTTER)->SetWindowText(str);
    
}


void CSceneImageSetting::OnBnClickedBtnGainAdd()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    SCENE_PKG config;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_GAIN)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue + 1;
	if (0 ==m_ProcProcess)
	{	
		m_Gain = iValue;	
	}else
	{
		GetSceneParam(m_iCurSceneID, &config);
		if (iValue > 48 || m_iCurIndex >= config.imageCount)
		{
			//AfxMessageBox("未知异常");
			return;
		}
		if (!config.vecInfo.empty())
		{
			if (m_iCurIndex >= config.vecInfo.size())
			{
				AfxMessageBox("索引超出范围");
				return;
			}
			config.vecInfo[m_iCurIndex].CameraGain = iValue;
		}
		 SetSceneParam(&config); 
	}
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_GAIN)->SetWindowText(str);
   
}


void CSceneImageSetting::OnBnClickedBtnGainMinus()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    SCENE_PKG config;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_GAIN)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue - 1;
	if (0 ==m_ProcProcess)
	{	
		m_Gain = iValue;	
	}else
	{
		GetSceneParam(m_iCurSceneID, &config);
		if (iValue < 8 || m_iCurIndex >= config.imageCount)
		{
			return;
		}
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0)
		{
			if (m_iCurIndex >= config.vecInfo.size())
			{
				AfxMessageBox("索引超出范围");
				return;
			}
			config.vecInfo[m_iCurIndex].CameraGain = iValue;
		}
		 SetSceneParam(&config);
	}
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_GAIN)->SetWindowText(str);
   
}

bool CSceneImageSetting::ConfigGrabImage(bool sendflag)
{
	LARGE_INTEGER  start,end,litmp;
	QueryPerformanceFrequency(&litmp);//获取系统时钟频率
	QueryPerformanceCounter(&start); 
	CHECK_RESULT *res = NULL;
    m_iCurIndex =  m_pContainer->GetCurScene()->GetTypeIndex(IMAGE_ACQISITION,m_pContainer);//第几个图像
	SYSTEM_PARAM pSysParam;
	GetSysParam(&pSysParam);
	vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(pSysParam.MainImageWndVec));
	if (m_iCurIndex<0)
	{
		AfxMessageBox("相机抓取图像时，图像索引错误");
		return false;
	}
	res = GetSceneProcResultById(m_iCurSceneID);
	if (NULL == res)
	{
		AfxMessageBox("场景:%d不存在!",m_iCurSceneID);
		return false;
	}
	int GrabError =  SceneGrabImage(m_iCurSceneID,m_iCurIndex,m_CameraIndex,m_ShutterTime,m_Gain,m_Interval,m_LightIndex,m_LightValue);
	QueryPerformanceCounter(&end) ; 
	double caturetime = (double)(end.QuadPart-start.QuadPart) / (double)litmp.QuadPart;    //获取时间单位为秒	
	res->caturetime = caturetime;
	if (true == sendflag)
	{
		SceneGrabFinished(m_iCurSceneID,res, GrabError);
	}	
	long HalWinId = -1;
	long WinId = -1;
	//if(g_SceneInitialCount== m_pContainer->m_SceneInitialCount&&g_SceneInitialCount!=0)
	if(-1 != g_SceneDlgHwinId)
	{
		WinId  = g_SceneDlgHwinId;
	}else
	{
		SYSTEM_PARAM pSysParam;
		GetSysParam(&pSysParam);
		vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(pSysParam.MainImageWndVec));
		if (m_CameraIndex< pImageWnd->size())
		 {
		    WinId = (long)pImageWnd->at(m_CameraIndex)->m_hWnd;//GetSceneId()
		 }else
		{
			WinId = (long)pImageWnd->at(0)->m_hWnd; //当配置的窗口数目没有场景数目这么多时就显示在第一个窗口，图像显示在第一个窗口。

		}
		  
	}
	return	DispImage(WinId,m_iCurSceneID, m_iCurIndex);
}

void  CSceneImageSetting::SaveProcConfig(CString ConfigPath,int ProcIndex)
{
	CString ProcIndexName, StrTemp;
	ProcIndexName.Format("%d",ProcIndex);
	StrTemp.Format("%d",m_CameraIndex);
	WritePrivateProfileString(ProcIndexName, "CamIndex",StrTemp, ConfigPath);
	StrTemp.Format("%d",m_LightIndex);
    WritePrivateProfileString(ProcIndexName, "lightIndex",StrTemp, ConfigPath);
	StrTemp.Format("%d",m_ShutterTime);
	WritePrivateProfileString(ProcIndexName, "shutter", StrTemp, ConfigPath);
	StrTemp.Format("%d",m_Gain);
	WritePrivateProfileString(ProcIndexName, "gain",StrTemp, ConfigPath);
	StrTemp.Format("%d",m_Interval);
	WritePrivateProfileString(ProcIndexName, "Interval",StrTemp, ConfigPath);

}

void  CSceneImageSetting::ReadProcConfig(CString ConfigPath,int ProcIndex)
{
	CString ProcIndexName;
	ProcIndexName.Format("%d",ProcIndex);
	//m_CameraIndex = GetPrivateProfileInt(ProcIndexName, "CamIndex", -1, ConfigPath);
	m_LightIndex = GetPrivateProfileInt(ProcIndexName, "lightIndex", 0, ConfigPath);
	m_ShutterTime = GetPrivateProfileInt(ProcIndexName, "shutter", 0, ConfigPath);
	m_Gain = GetPrivateProfileInt(ProcIndexName, "gain", 0, ConfigPath);
	m_Interval = GetPrivateProfileInt(ProcIndexName, "Interval", 200, ConfigPath);
	m_ConfigPath = ConfigPath;
	m_ProcIndex = ProcIndex;
	m_ProcProcess = 0;


}

BOOL  CSceneImageSetting::VisionProcRun(CHECK_RESULT& ProcRes)
{
	bool ResBool= false;
	SYSTEM_PARAM pSysParam;
	GetSysParam(&pSysParam);
	vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(pSysParam.MainImageWndVec));
	long HalconWinID = -1;
	if(m_CameraIndex >= pImageWnd->size())//先要判断是否存在
	{
		ResBool = false;
		return ResBool;
	}
	if(-1 == g_SceneDlgHwinId)
	{
		FindHalconWinID((int)(pImageWnd->at(m_CameraIndex)->m_hWnd),HalconWinID);
	}
	else
	{
		FindHalconWinID(g_SceneDlgHwinId,HalconWinID);
	}
	
	//拍照,运行时VisionProcRun都发结果
	ResBool = ConfigGrabImage(true);
	//根据处理结果对ProcRes赋值
	ProcRes.CameraIndex[0] =  m_CameraIndex;
	if (ResBool)
	{
		SetVisionColorHeight(HalconWinID,"green");
		ShowVisionString(HalconWinID,"采图 OK",100,100);

	}else 
	{
		SetVisionColorHeight(HalconWinID,"red");
		ShowVisionString(HalconWinID,"采图 NG",100,100);
	}

	//是否保存图片
	SCENE_PKG *pConfig = (SCENE_PKG *)GetSceneParamHandle(m_iCurSceneID);
	if (true == ResBool && NULL != pConfig && true == pConfig->bSave)
	{
		SaveImage(m_iCurSceneID,m_iCurIndex);
	}
    return ResBool;
}
