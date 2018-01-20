// SelectFunction.cpp : 实现文件
//

#include "stdafx.h"
#include "Flatfish.h"
#include "SelectFunction.h"
#include "afxdialogex.h"

#include "iManagerCtrl.h"

// CSelectFunction 对话框

IMPLEMENT_DYNAMIC(CSelectFunction, CDialogEx)

CSelectFunction::CSelectFunction(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectFunction::IDD, pParent)
{
    m_iCurSceneID = 0;
}

CSelectFunction::~CSelectFunction()
{
}

void CSelectFunction::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectFunction, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CSelectFunction::OnBnClickedButtonImage)
    ON_BN_CLICKED(IDC_BUTTON_MODEL, &CSelectFunction::OnBnClickedButtonModel)
    ON_BN_CLICKED(IDC_BUTTON_THRESHOLD, &CSelectFunction::OnBnClickedButtonThreshold)
    ON_BN_CLICKED(IDC_BUTTON_FIND_LINE, &CSelectFunction::OnBnClickedButtonFindLine)
    ON_BN_CLICKED(IDC_BUTTON_FIND_CIRCLE, &CSelectFunction::OnBnClickedButtonFindCircle)
    ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_MORE, &CSelectFunction::OnBnClickedButtonMore)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSelectFunction 消息处理程序


void CSelectFunction::OnBnClickedButtonImage()
{
    // TODO: 在此添加控件通知处理程序代码
    NotifyAddFunction((LPARAM)MAIN_PRO);
}

void CSelectFunction::UpdateUI(int iSceneID, int index)
{
    SetBackgroundColor(BACKGROUND_COLOR_M);
    m_iCurSceneID = iSceneID;
}

void CSelectFunction::OnBnClickedButtonModel()
{
    // TODO: 在此添加控件通知处理程序代码
    NotifyAddFunction((LPARAM)MODEL_PRO);
}


void CSelectFunction::OnBnClickedButtonThreshold()
{
    // TODO: 在此添加控件通知处理程序代码
    NotifyAddFunction((LPARAM)THRE_PRO);
}


void CSelectFunction::OnBnClickedButtonFindLine()
{
    // TODO: 在此添加控件通知处理程序代码
    NotifyAddFunction((LPARAM)LINE_PRO);
}


void CSelectFunction::OnBnClickedButtonFindCircle()
{
    // TODO: 在此添加控件通知处理程序代码
    NotifyAddFunction((LPARAM)CIRCLE_PRO);
}


void CSelectFunction::NotifyAddFunction(LPARAM iType)
{
    CString strSceneName;
    SYSTEM_PARAM config;
    GetSysParam(&config);
    HWND hwnd = ::FindWindowA(NULL,config.pstrSceneName[m_iCurSceneID].c_str());
    if (NULL != hwnd)
    {
        //GetDlgItem(id)->GetWindowText(m_strTemp);
        ::SendMessageW(hwnd,ADD_FUNCTION_MSG,NULL,(LPARAM)(iType));
        CSelectFunction::OnOK();
    }
}

void CSelectFunction::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CDialogEx::OnActivate(nState, pWndOther, bMinimized);

    // TODO: 在此处添加消息处理程序代码
    if(nState == WA_INACTIVE)
    {
        ShowWindow(SW_HIDE);
    }
}


void CSelectFunction::OnBnClickedButtonMore()
{
	// TODO: 在此添加控件通知处理程序代码
	NotifyAddFunction((LPARAM)MORE_PRO);
}


void CSelectFunction::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	vector<VIRSION_DATA_PKG *>::iterator it;
	SCENE_PKG pConfig;				//从配置文件中读取图片有关roi信息
	GetSceneParam(m_iCurSceneID, &pConfig);
	VIRSION_DATA_PKG *pTmp = NULL;
	//启用或禁用按钮
	for (it = pConfig.vecBody.begin(); it != pConfig.vecBody.end(); it++)
	{
		pTmp = *it;
		if ((pTmp->iValue % 10000)/100 == MORE_PRO)
		{
			GetDlgItem(IDC_BUTTON_MORE)->EnableWindow(FALSE);
			break;
		}
		else
		{
			GetDlgItem(IDC_BUTTON_MORE)->EnableWindow(TRUE);
		}
	}
}
