// SceneModelSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneManager.h"
#include "SceneModelSetting.h"
#include "SceneMatchModel.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"

#include "common.h" //主要引用场景类型10，或者11，后续要改

//模板参数的排列
#define MODEL_NUM_INDEX             1
#define MODEL_NUMLEVELS_INDEX       2
#define MODEL_C_NUMLEVELS_INDEX     3
#define MODEL_CONTRAST_INDEX        4
#define MODEL_ANGLE_START_INDEX     5
#define MODEL_ANGLE_EXTENT_INDEX    6



// CSceneModelSetting 对话框

IMPLEMENT_DYNAMIC(CSceneModelSetting, CDialogEx)

CSceneModelSetting::CSceneModelSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneModelSetting::IDD, pParent)
{
    m_iCurSceneID = 0;
    m_iCurIndex = 0;
	m_ProcProcess = -1;
	m_pContainer = NULL;
	//
	m_MatchMinScore = 0.55;
	m_MatchNum = 1;
    m_MatchLevel = 0;
	m_BuildLevel = 4;
	m_Contrast = 20;
	m_AngleStart = -30;
	m_AngleExtent = 60;
	m_ModeType = 1 ;

	m_OriMatchRow = 0.0;
	m_OriMatchCol = 0.0;
	m_OriMatchPhi = 0.0;

	m_IsResetOriMatch = false;

}

CSceneModelSetting::~CSceneModelSetting()
{
	
}

void CSceneModelSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCENE_MATCH_INFO, m_SceneMatchInfo);
	DDX_Control(pDX, IDC_SCENEMODEL_AFFINE, m_CheckAffine);
	DDX_Control(pDX, IDC_SCENEMODE_RESTYPE, m_ResType);
	DDX_Control(pDX, IDC_RESET_ORIMATCH, m_ResetOriMatch);
}


BEGIN_MESSAGE_MAP(CSceneModelSetting, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SETTING, &CSceneModelSetting::OnBnClickedButtonSetting)
    ON_BN_CLICKED(IDC_BTN_MODEL_ROI, &CSceneModelSetting::OnBnClickedBtnModelRoi)
    ON_BN_CLICKED(IDC_BTN_CREAT_MODEL, &CSceneModelSetting::OnBnClickedBtnCreatModel)
    ON_BN_CLICKED(IDC_BTN_CHECK_MODEL, &CSceneModelSetting::OnBnClickedBtnCheckModel)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_SCORE_A, &CSceneModelSetting::OnBnClickedButtonModelScoreA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_SCORE_S, &CSceneModelSetting::OnBnClickedButtonModelScoreS)
    ON_CBN_SELCHANGE(IDC_COMBO_MODEL_TYPE, &CSceneModelSetting::OnCbnSelchangeComboModelType)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_NUM_A, &CSceneModelSetting::OnBnClickedButtonModelNumA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_NUM_S, &CSceneModelSetting::OnBnClickedButtonModelNumS)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_NUMLEVELS_A, &CSceneModelSetting::OnBnClickedButtonModelNumlevelsA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_NUMLEVELS_S, &CSceneModelSetting::OnBnClickedButtonModelNumlevelsS)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_C_NUMLEVELS_A, &CSceneModelSetting::OnBnClickedButtonModelCNumlevelsA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_C_NUMLEVELS_S, &CSceneModelSetting::OnBnClickedButtonModelCNumlevelsS)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_ANGLE_START_A, &CSceneModelSetting::OnBnClickedButtonModelAngleStartA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_ANGLE_START_S, &CSceneModelSetting::OnBnClickedButtonModelAngleStartS)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_ANGLE_EXTENT_A, &CSceneModelSetting::OnBnClickedButtonModelAngleExtentA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_ANGLE_EXTENT_S, &CSceneModelSetting::OnBnClickedButtonModelAngleExtentS)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_COSTRAST_A, &CSceneModelSetting::OnBnClickedButtonModelCostrastA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_COSTRAST_S, &CSceneModelSetting::OnBnClickedButtonModelCostrastS)
	ON_BN_CLICKED(IDC_SCENEMODEL_AFFINE, &CSceneModelSetting::OnBnClickedScenemodelAffine)
	ON_CBN_SELCHANGE(IDC_SCENEMODE_RESTYPE, &CSceneModelSetting::OnCbnSelchangeScenemodeRestype)
	ON_BN_CLICKED(IDC_RESET_ORIMATCH, &CSceneModelSetting::OnBnClickedResetOrimatch)
END_MESSAGE_MAP()


// CSceneModelSetting 消息处理程序
void CSceneModelSetting::CloseSettingDlg()
{
	OnOK();
}

void CSceneModelSetting::UpdateUI()
{
	m_CheckAffine.ShowWindow(TRUE);

	CString strBuf;
	strBuf.Format("%0.2lf", m_MatchMinScore);
	GetDlgItem(IDC_EDIT_SCORE)->SetWindowText(strBuf);
	strBuf.Format("%d", m_MatchNum);
	GetDlgItem(IDC_EDIT_NUM)->SetWindowText(strBuf);
	strBuf.Format("%d",m_MatchLevel);
	GetDlgItem(IDC_EDIT_NUMLEVELS)->SetWindowText(strBuf);
	strBuf.Format("%d", m_BuildLevel);
	GetDlgItem(IDC_EDIT_C_NUMLEVELS)->SetWindowText(strBuf);
	strBuf.Format("%d", m_Contrast);
	GetDlgItem(IDC_EDIT_CONTRAST)->SetWindowText(strBuf);
	strBuf.Format("%0.2lf", m_AngleStart);
	GetDlgItem(IDC_EDIT_ANGLE_START)->SetWindowText(strBuf);
	strBuf.Format("%0.2lf", m_AngleExtent);
	GetDlgItem(IDC_EDIT_ANGLE_EXTENT)->SetWindowText(strBuf);

	((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->ResetContent();
	((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->AddString("通用模板");  //NORMAL_MODEL
	((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->AddString("ncc模板");   //NCC_MODEL
	((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->AddString("手绘模板");   //NCC_MODEL
	((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->SetCurSel(m_ModeType);

	m_ResType.ResetContent();

	m_ResType.InsertString(0,"图像绝对坐标");
	m_ResType.SetItemData(0,VisionProcBase::IMAGE_ABS);

	m_ResType.InsertString(0,"图像相对坐标");
	m_ResType.SetItemData(0,VisionProcBase::TEST_REPOSITION);

	m_ResType.InsertString(0,"绝对坐标");
	m_ResType.SetItemData(0,VisionProcBase::MODLE_DISTENCE);

	m_ResType.InsertString(0,"相对坐标(不补偿角度)");
	m_ResType.SetItemData(0,VisionProcBase::NO_MATCH_ROT);

	m_ResType.InsertString(0,"相对校正(补偿角度)");
	m_ResType.SetItemData(0,VisionProcBase::REAL_UNIT_RESULT);

	m_ResType.InsertString(0,"标定(匹配像素结果)");
	m_ResType.SetItemData(0,VisionProcBase::NOMAL_RESULT);





	int TypeCount = m_ResType.GetCount();
	for (int i =0;i<TypeCount;i++)
	{
		if (m_ResType.GetItemData(i) == ((SceneMatchModel*)m_pContainer)->m_ResultType)
		{
			m_ResType.SetCurSel(i);
			break;
		}
	}
	
	m_IsResetOriMatch = false;//这里应该删除

	if (m_IsResetOriMatch)
	{
		m_ResetOriMatch.SetCheck(1);
	}else
	{
		m_ResetOriMatch.SetCheck(0);
	}



}

void CSceneModelSetting::GetDlgParam(void)
{
	CString strBuf;
	GetDlgItem(IDC_EDIT_SCORE)->GetWindowText(strBuf);
	m_MatchMinScore = atof(strBuf);

	GetDlgItem(IDC_EDIT_NUM)->GetWindowText(strBuf);
	m_MatchNum = atoi(strBuf);

	GetDlgItem(IDC_EDIT_NUMLEVELS)->GetWindowText(strBuf);
	m_MatchLevel = atoi(strBuf);
	
	GetDlgItem(IDC_EDIT_C_NUMLEVELS)->GetWindowText(strBuf);
	m_BuildLevel = atoi(strBuf);

	GetDlgItem(IDC_EDIT_CONTRAST)->GetWindowText(strBuf);
	m_Contrast = atoi(strBuf);

	GetDlgItem(IDC_EDIT_ANGLE_START)->GetWindowText(strBuf);
	m_AngleStart= atof(strBuf);

	GetDlgItem(IDC_EDIT_ANGLE_EXTENT)->GetWindowText(strBuf);
	m_AngleExtent= atof(strBuf);
	m_ModeType = ((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->GetCurSel();

	//电机设置也设置下是
	SCENE_PKG *pConfig = NULL;
	pConfig =(SCENE_PKG *) GetSceneParamHandle(m_iCurSceneID);
	if (VisionProcBase::NOMAL_RESULT == ((SceneMatchModel*)m_pContainer)->m_ResultType)
	{
		pConfig->SceneType = CALIB_CAMERA;		
	}
	else
	{
		pConfig->SceneType = WORKING_CAMERA;
	}	
	pConfig->CalPointIndex = 0; //标定从0开始


}

void CSceneModelSetting::GetContainer(void* pContainer)
{
	m_pContainer = pContainer;
	m_iCurSceneID = ((SceneMatchModel*)m_pContainer)->GetSceneId();

}


void CSceneModelSetting::UpdateUI(int iSceneID, int index, CWnd *hROI )
{
    m_iCurSceneID = iSceneID;
    m_iCurIndex = index;
    m_hROI = hROI;
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    SetBackgroundColor(BACKGROUND_COLOR_M);
    GetSceneParam(iSceneID,&config);
    int iPos = IFindParamData(&config, MODEL_PRO, index);
    int iModelType = 0;
    if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
    {
        int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos]);
        VIRSION_DATA_PKG *pTmp = config.vecBody[iPos];

        strBuf.Format("%0.2lf", pTmp[index].dValue);
        GetDlgItem(IDC_EDIT_SCORE)->SetWindowText(strBuf);
        strBuf.Format("%0.2lf", pTmp[index+MODEL_NUM_INDEX].dValue);
        GetDlgItem(IDC_EDIT_NUM)->SetWindowText(strBuf);
        strBuf.Format("%0.2lf", pTmp[index+MODEL_NUMLEVELS_INDEX].dValue);
        GetDlgItem(IDC_EDIT_NUMLEVELS)->SetWindowText(strBuf);
        strBuf.Format("%0.2lf", pTmp[index+MODEL_C_NUMLEVELS_INDEX].dValue);
        GetDlgItem(IDC_EDIT_C_NUMLEVELS)->SetWindowText(strBuf);
        strBuf.Format("%0.2lf", pTmp[index+MODEL_CONTRAST_INDEX].dValue);
        GetDlgItem(IDC_EDIT_CONTRAST)->SetWindowText(strBuf);
        strBuf.Format("%0.2lf", (pTmp[index+MODEL_ANGLE_START_INDEX].dValue * 180/3.14));
        GetDlgItem(IDC_EDIT_ANGLE_START)->SetWindowText(strBuf);
        strBuf.Format("%0.2lf", (pTmp[index+MODEL_ANGLE_EXTENT_INDEX].dValue * 180/3.14));
        GetDlgItem(IDC_EDIT_ANGLE_EXTENT)->SetWindowText(strBuf);

        index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos],SPEC_TYPE);
        iModelType = pTmp[index].iValue;
    }
    else
    {
        GetDlgItem(IDC_EDIT_SCORE)->SetWindowText("0.8");
    }
    if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
    {
        int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos]);
        VIRSION_DATA_PKG *pTmp = config.vecBody[iPos];
        strBuf.Format("%0.2lf", pTmp[index].dValue);
        //iModelType = pTmp[index + 1].iValue;
    }
    else
    {
        strBuf = "0.8";
    }
    ((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->ResetContent();
    ((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->AddString("通用模板");  //NORMAL_MODEL
    ((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->AddString("ncc模板");   //NCC_MODEL
    ((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->AddString("手绘模板");   //NCC_MODEL
    ((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->SetCurSel(iModelType);
}

void CSceneModelSetting::OnBnClickedButtonSetting()
{
    // TODO: 在此添加控件通知处理程序代码
	if ( 0 == m_ProcProcess)
	{
		GetDlgParam();
	}else
	{
		CString strBuf;
		SCENE_PKG config;  //从配置文件中读取图片有关roi信息
		GetSceneParam(m_iCurSceneID,&config);
		GetDlgItem(IDC_EDIT_SCORE)->GetWindowText(strBuf);
		int index = 2 + 0;
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index].dValue = atof(strBuf); 
		}
		SetSceneParam(&config);
	}
}

void CSceneModelSetting::OnBnClickedBtnModelRoi()
{
	if (0 == m_ProcProcess)
	{
		 if (m_pContainer)
		 {
			 ((SceneMatchModel*)m_pContainer)->ShowAllShape();
		 }
	}else
	{
		CRect irt=0;
		m_hROI->GetWindowRect(irt);
		::ClipCursor(&irt);
		DrawROI(m_winID,m_iCurSceneID,m_iCurIndex,MODEL_PRO);
		UpdateData(TRUE);	
		ClipCursor(NULL);
	}

}


void CSceneModelSetting::OnBnClickedBtnCreatModel()
{
	//
	if (0 == m_ProcProcess)
	{
		 //CreateVisionModel(m_winID,m_iCurSceneID);
		 ((SceneMatchModel*)m_pContainer)->CreateModel(m_BuildLevel,m_AngleStart,m_AngleExtent,m_Contrast,m_OriMatchRow,
			 m_OriMatchCol,m_OriMatchPhi);

	}else
	{
		CreateVisionModel(m_winID,m_iCurSceneID,m_iCurIndex);
		FindVisionModel(m_iCurSceneID, m_iCurIndex, true);
		DispObj(m_winID,m_iCurSceneID);
	}
}


void CSceneModelSetting::OnBnClickedBtnCheckModel()
{
	double MatchRow = m_OriMatchRow , MatchCol = m_OriMatchCol ,MatchPhi = m_OriMatchPhi ;
	double MatchScore = m_MatchMinScore;
	CString strTemp;
	//MatchCol = 1;
	if (0 == m_ProcProcess)
	{	 
		// m_OriMatchRow = 0.0;
		// m_OriMatchCol = 0.0;
		// m_OriMatchPhi = 0.0;
		
		if (((SceneMatchModel*)m_pContainer)->MatchModel(m_MatchLevel,m_MatchNum,MatchScore,false,MatchRow,MatchCol,MatchPhi))
		{
		     strTemp.Format("Row:%0.4lf,\n Col:%0.4lf,\n Angle:%0.4lf,\n Score:%0.4lf",MatchRow,MatchCol,MatchPhi,MatchScore);
		     if (m_IsResetOriMatch)
		     {
				 m_OriMatchRow = MatchRow; //这里我的是原来的
				 m_OriMatchCol = MatchCol;
				 m_OriMatchPhi = MatchPhi;
		     }
		}else
		{
			strTemp.Format("匹配失败");
		}
		m_SceneMatchInfo.SetWindowTextA(strTemp);
	}else
	{
        FindVisionModel(m_iCurSceneID,m_iCurIndex);
        DispObj(m_winID,m_iCurSceneID);
	}
}

void CSceneModelSetting::SetDisplayWindowID(int winID)
{
    m_winID = winID;
}

void CSceneModelSetting::OnBnClickedButtonModelScoreA()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetDlgItem(IDC_EDIT_SCORE)->GetWindowText(strBuf);
    dValue = atof(strBuf);
    if (dValue < 1.0)
    {
        dValue += 0.1;
    }
	if (0 != m_ProcProcess)
	{
		GetSceneParam(m_iCurSceneID,&config);
		int index = 2 + 0;
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index].dValue = dValue; //TBD
		}
		
		SetSceneParam(&config);
	}
	strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_SCORE)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelScoreS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    int index = 2 + 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_SCORE)->GetWindowText(strBuf);
    dValue = atof(strBuf);
    if (dValue > 0.0)
    {
        dValue -= 0.1;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index].dValue = dValue; 
		}
		SetSceneParam(&config);
	}

	strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_SCORE)->SetWindowText(strBuf);

}


BOOL CSceneModelSetting::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
	

    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN &&0 != m_ProcProcess) 
    {
		
        if (pMsg->hwnd==GetDlgItem(IDC_EDIT_SCORE)->m_hWnd)
        {
            CString strBuf;
            SCENE_PKG config;  //从配置文件中读取图片有关roi信息
            GetSceneParam(m_iCurSceneID,&config);
            GetDlgItem(IDC_EDIT_SCORE)->GetWindowText(strBuf);

            int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
            if (iPos >= config.vecBody.size() || iPos >= config.vecDataType.size())
            {
                return FALSE;
            }
            int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
            if (iPos >= 0)
            {
                config.vecBody[iPos][index].dValue = atof(strBuf); 
            }
            SetSceneParam(&config);
            return true;
        }

    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSceneModelSetting::OnCbnSelchangeComboModelType()
{
    // TODO: 在此添加控件通知处理程序代码
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    int iCur = ((CComboBox *)GetDlgItem(IDC_COMBO_MODEL_TYPE))->GetCurSel();
	
	if (0 != m_ProcProcess)
	{
		GetSceneParam(m_iCurSceneID,&config);
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], SPEC_TYPE);
			if (index >= 0)
			{
				config.vecBody[iPos][index].iValue = iCur; //TBD
				SetSceneParam(&config);
			}
		}
	}

}


void CSceneModelSetting::OnBnClickedButtonModelNumA()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_NUM)->GetWindowText(strBuf);
    dValue = atof(strBuf);
    if (dValue >= 0)
    {
        dValue += 1;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_NUM_INDEX].dValue = dValue; //TBD
		}
	    SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_NUM)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelNumS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_NUM)->GetWindowText(strBuf);
    dValue = atof(strBuf);
    if (dValue > 1)
    {
        dValue -= 1;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_NUM_INDEX].dValue = dValue; //TBD
		}
		  SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_NUM)->SetWindowText(strBuf);
   
}


void CSceneModelSetting::OnBnClickedButtonModelNumlevelsA()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_NUMLEVELS)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue >= 0 && dValue <= 9)
    {
        dValue += 1;
    }
    else
    {
        dValue = 0;
    }
	if (0 != m_ProcProcess)
	{

		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_NUMLEVELS_INDEX].dValue = dValue; 
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_NUMLEVELS)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelNumlevelsS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_NUMLEVELS)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue > 0 && dValue <= 10)
    {
        dValue -= 1;
    }
    else
    {
        dValue = 0;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_NUMLEVELS_INDEX].dValue = dValue; 
		}
		SetSceneParam(&config);
	}

    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_NUMLEVELS)->SetWindowText(strBuf);
   
}


void CSceneModelSetting::OnBnClickedButtonModelCNumlevelsA()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_C_NUMLEVELS)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue >= -1 && dValue <= 9)
    {
        dValue += 1;
    }
    else
    {
        dValue = 0;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_C_NUMLEVELS_INDEX].dValue = dValue; 
		}
		SetSceneParam(&config);
	}

    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_C_NUMLEVELS)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelCNumlevelsS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_C_NUMLEVELS)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue >= 0 && dValue <= 10)
    {
        dValue -= 1;
    }
    else
    {
        dValue = 0;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_C_NUMLEVELS_INDEX].dValue = dValue; 
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_C_NUMLEVELS)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelCostrastA()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_CONTRAST)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue >= 0 && dValue <= 254)
    {
        dValue += 1;
    }
    else
    {
        dValue = 0;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_CONTRAST_INDEX].dValue = dValue; 
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_CONTRAST)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelCostrastS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_CONTRAST)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue > 0 && dValue <= 255)
    {
        dValue -= 1;
    }
    else
    {
        dValue = 0;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_CONTRAST_INDEX].dValue = dValue; 
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_CONTRAST)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelAngleStartA()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_ANGLE_START)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue < 359.0 && dValue > -360.0)
    {
        dValue += 1;
    }
    else
    {
        dValue = -22;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_ANGLE_START_INDEX].dValue = (dValue * 3.14)/180; //TBD
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_ANGLE_START)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelAngleStartS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_ANGLE_START)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue < 360.0 && dValue > -359.0)
    {
        dValue -= 1;
    }
    else
    {
        dValue = -22;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_ANGLE_START_INDEX].dValue = (dValue * 3.14)/180; //TBD
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_ANGLE_START)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelAngleExtentA()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_ANGLE_EXTENT)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue < 360.0 && dValue > 0)
    {
        dValue += 1;
    }
    else
    {
        dValue = 45;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_ANGLE_EXTENT_INDEX].dValue = (dValue * 3.14)/180; //TBD
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_ANGLE_EXTENT)->SetWindowText(strBuf);
    
}


void CSceneModelSetting::OnBnClickedButtonModelAngleExtentS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT_ANGLE_EXTENT)->GetWindowText(strBuf);
    dValue = atoi(strBuf);
    if (dValue < 360.0 && dValue > 0)
    {
        dValue -= 1;
    }
    else
    {
        dValue = 45;
    }
	if (0 != m_ProcProcess)
	{
		int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
		if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
		{
			int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
			config.vecBody[iPos][index + MODEL_ANGLE_EXTENT_INDEX].dValue = (dValue * 3.14)/180; //TBD
		}
		SetSceneParam(&config);
	}
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT_ANGLE_EXTENT)->SetWindowText(strBuf);
    
}

void CSceneModelSetting::SaveProcConfig(CString ConfigPath,int ProcIndex)
{

	char szValue[256]={0};
	char szName[256] ={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);
	//存储模板类型
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%d",m_BuildLevel);
	WritePrivateProfileStringA(ProcIndexName, "BuildLevel",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%d",m_MinContrast);
	WritePrivateProfileStringA(ProcIndexName, "BuildMinContrast",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%d",m_Contrast);
	WritePrivateProfileStringA(ProcIndexName, "BuildContrast",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%.6lf",m_AngleStart);
	WritePrivateProfileStringA(ProcIndexName, "BuildAngleStart",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%.6lf",m_AngleExtent);
	WritePrivateProfileStringA(ProcIndexName, "BuildAngleExtent",szValue, ConfigPath);

	//
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%.6lf",m_OriMatchRow);
	WritePrivateProfileStringA(ProcIndexName, "BuildOriginMatchRow",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%.6lf",m_OriMatchCol);
	WritePrivateProfileStringA(ProcIndexName, "BuildOriginMatchCol",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%.6lf",m_OriMatchPhi);
	WritePrivateProfileStringA(ProcIndexName, "BuildOriginMatchPhi",szValue, ConfigPath);


	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%d",m_MatchLevel);
	WritePrivateProfileStringA(ProcIndexName, "MatchLevel",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%.6lf",m_MatchMinScore);
	WritePrivateProfileStringA(ProcIndexName, "MatchMinScore",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%d",m_MatchNum);
	WritePrivateProfileStringA(ProcIndexName, "MatchNum",szValue, ConfigPath);

}
void CSceneModelSetting::ReadProcConfig(CString ConfigPath,int ProcIndex)
{
	//
	char szValue[256]={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);

	m_ModeType = GetPrivateProfileInt(ProcIndexName,"ModleType",-1,ConfigPath);
	m_BuildLevel = GetPrivateProfileInt(ProcIndexName,"BuildLevel",-1,ConfigPath);
	m_MinContrast = GetPrivateProfileInt(ProcIndexName,"BuildMinContrast",-1,ConfigPath);
	m_Contrast = GetPrivateProfileInt(ProcIndexName,"BuildContrast",-1,ConfigPath);

	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA(ProcIndexName,"BuildAngleStart","",szValue,sizeof(szValue),ConfigPath);
	m_AngleStart = atof(szValue);

	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA(ProcIndexName,"BuildAngleExtent","",szValue,sizeof(szValue),ConfigPath);
	m_AngleExtent = atof(szValue);

	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA(ProcIndexName,"BuildOriginMatchRow","",szValue,sizeof(szValue),ConfigPath);
	m_OriMatchRow = atof(szValue);

	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA(ProcIndexName,"BuildOriginMatchCol","",szValue,sizeof(szValue),ConfigPath);
	m_OriMatchCol = atof(szValue);

	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA(ProcIndexName,"BuildOriginMatchPhi","",szValue,sizeof(szValue),ConfigPath);
	m_OriMatchPhi = atof(szValue);




	m_MatchLevel = GetPrivateProfileInt(ProcIndexName,"MatchLevel",-1,ConfigPath);

	memset(szValue,0,sizeof(szValue));
	GetPrivateProfileStringA(ProcIndexName,"MatchMinScore","",szValue,sizeof(szValue),ConfigPath);
	m_MatchMinScore = atof(szValue);

	m_MatchNum = GetPrivateProfileInt(ProcIndexName,"MatchNum",-1,ConfigPath);
}
BOOL CSceneModelSetting::VisionProcRun(CHECK_RESULT& ProcRes)
{
	//  这里的参数初值必须 MatchCol<0，因为在别的地方借用了该参数，>0会有其他意义。
	double MatchRow = m_OriMatchRow , MatchCol = m_OriMatchCol ,MatchPhi = m_OriMatchPhi ;
	double MatchScore = m_MatchMinScore;
	BOOL bRes = TRUE;
	CString strTemp;
	if(((SceneMatchModel*)m_pContainer)->MatchModel(m_MatchLevel,m_MatchNum,MatchScore,false,MatchRow,MatchCol,MatchPhi))
	{
	    // 是否需要将匹配的值变成//模板匹配成功,直接封装结果,4个结果
		sprintf(ProcRes.szValue[1],"%0.4lf",MatchRow);
		sprintf(ProcRes.szValue[2],"%0.4lf",MatchCol);
		sprintf(ProcRes.szValue[3],"%0.4lf",MatchPhi);
		bRes = TRUE;

    }else
	{	
		sprintf(ProcRes.szValue[1],"%0.4lf",0.0);
		sprintf(ProcRes.szValue[2],"%0.4lf",0.0);
		sprintf(ProcRes.szValue[3],"%0.4lf",0.0);
		ProcRes.err = MODEL_PRO;
	    bRes =  FALSE;
	}
	sprintf(ProcRes.szValue[0],"%d",bRes);//模板匹配成功,直接封装结果
	ProcRes.iCount  = 4;
	return bRes;
}


void CSceneModelSetting::OnBnClickedScenemodelAffine()
{
	// TODO: 在此添加控件通知处理程序代码
	if (0 == m_CheckAffine.GetCheck())
	{
		m_CheckAffine.SetWindowTextA("像素坐标");
	}else
	{
		m_CheckAffine.SetWindowTextA("机械坐标");
	}

}


void CSceneModelSetting::OnCbnSelchangeScenemodeRestype()
{
	((SceneMatchModel*)m_pContainer)->m_ResultType =(VisionProcBase::ProcResultType)m_ResType.GetItemData(m_ResType.GetCurSel());
   //同时根据选择来把此场景改为标定场景还是处理场景
	SCENE_PKG *pConfig = NULL;
	pConfig =(SCENE_PKG *) GetSceneParamHandle(m_iCurSceneID);
	if (VisionProcBase::NOMAL_RESULT == ((SceneMatchModel*)m_pContainer)->m_ResultType)
	{
		pConfig->SceneType = CALIB_CAMERA;		
	}
	else
	{
		pConfig->SceneType = WORKING_CAMERA;
	}	
	pConfig->CalPointIndex = 0; //标定从0开始

}


void CSceneModelSetting::OnBnClickedResetOrimatch()
{
	// TODO: 在此添加控件通知处理程序代码
	int CheckReset = m_ResetOriMatch.GetCheck();
	if (0 == CheckReset)
	{
		m_IsResetOriMatch = false;
	}else
	{
		m_IsResetOriMatch = true;
	}
		


}
