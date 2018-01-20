// SceneModelSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "Flatfish.h"
#include "SceneModelSetting.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"


// CSceneModelSetting 对话框

IMPLEMENT_DYNAMIC(CSceneModelSetting, CDialogEx)

CSceneModelSetting::CSceneModelSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneModelSetting::IDD, pParent)
{
    m_iCurSceneID = 0;
    m_iCurIndex = 0;
}

CSceneModelSetting::~CSceneModelSetting()
{
}

void CSceneModelSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSceneModelSetting, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SETTING, &CSceneModelSetting::OnBnClickedButtonSetting)
    ON_BN_CLICKED(IDC_BTN_MODEL_ROI, &CSceneModelSetting::OnBnClickedBtnModelRoi)
    ON_BN_CLICKED(IDC_BTN_CREAT_MODEL, &CSceneModelSetting::OnBnClickedBtnCreatModel)
    ON_BN_CLICKED(IDC_BTN_CHECK_MODEL, &CSceneModelSetting::OnBnClickedBtnCheckModel)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_SCORE_A, &CSceneModelSetting::OnBnClickedButtonModelScoreA)
    ON_BN_CLICKED(IDC_BUTTON_MODEL_SCORE_S, &CSceneModelSetting::OnBnClickedButtonModelScoreS)
END_MESSAGE_MAP()


// CSceneModelSetting 消息处理程序

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
    if (iPos >= 0)
    {
        int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos]);
        VIRSION_DATA_PKG *pTmp = config.vecBody[iPos];
        strBuf.Format("%0.2lf", pTmp[index].dValue);
    }
    GetDlgItem(IDC_EDIT1)->SetWindowText(strBuf);
}

void CSceneModelSetting::OnBnClickedButtonSetting()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT1)->GetWindowText(strBuf);
    int index = 2 + 0;
    int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
    if (iPos >= 0)
    {
        int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
        config.vecBody[iPos][index].dValue = atof(strBuf); 
    }
    SetSceneParam(&config);
}

void CSceneModelSetting::OnBnClickedBtnModelRoi()
{
    CRect irt=0;
    m_hROI->GetWindowRect(irt);
    ::ClipCursor(&irt);
    DrawROI(m_winID,m_iCurSceneID,m_iCurIndex,MODEL_PRO);
    UpdateData(TRUE);	
    ClipCursor(NULL);
}


void CSceneModelSetting::OnBnClickedBtnCreatModel()
{
    CreateVisionModel(m_winID,m_iCurSceneID,m_iCurIndex);
    FindVisionModel(m_iCurSceneID, m_iCurIndex, true);
    DispObj(m_winID,m_iCurSceneID);
}


void CSceneModelSetting::OnBnClickedBtnCheckModel()
{
    FindVisionModel(m_iCurSceneID,m_iCurIndex);
    DispObj(m_winID,m_iCurSceneID);
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
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT1)->GetWindowText(strBuf);
    dValue = atof(strBuf);
    if (dValue < 1.0)
    {
        dValue += 0.1;
    }
    int index = 2 + 0;
    int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
    if (iPos >= 0)
    {
        int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
        config.vecBody[iPos][index].dValue = dValue; //TBD
    }
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT1)->SetWindowText(strBuf);
    SetSceneParam(&config);
}


void CSceneModelSetting::OnBnClickedButtonModelScoreS()
{
    // TODO: 在此添加控件通知处理程序代码
    CString strBuf;
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    double dValue = 0;
    int index = 2 + 0;
    GetSceneParam(m_iCurSceneID,&config);
    GetDlgItem(IDC_EDIT1)->GetWindowText(strBuf);
    dValue = atof(strBuf);
    if (dValue > 0.0)
    {
        dValue -= 0.1;
    }
    int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
    if (iPos >= 0)
    {
        int index = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos], NORMARL_TYPE);
        config.vecBody[iPos][index].dValue = dValue; 
    }
    strBuf.Format("%0.2lf", dValue);
    GetDlgItem(IDC_EDIT1)->SetWindowText(strBuf);
    SetSceneParam(&config);
}


BOOL CSceneModelSetting::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN ) 
    {
        if (pMsg->hwnd==GetDlgItem(IDC_EDIT1)->m_hWnd)
        {
            CString strBuf;
            SCENE_PKG config;  //从配置文件中读取图片有关roi信息
            GetSceneParam(m_iCurSceneID,&config);
            GetDlgItem(IDC_EDIT1)->GetWindowText(strBuf);

            int iPos = IFindParamData(&config, MODEL_PRO, m_iCurIndex);
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
