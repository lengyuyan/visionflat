// SetSceneDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Flatfish.h"
#include "SetSceneDlg.h"
#include "afxdialogex.h"
#include "DataType.h"


// CSetSceneDlg 对话框

IMPLEMENT_DYNAMIC(CSetSceneDlg, CDialogEx)

CSetSceneDlg::CSetSceneDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetSceneDlg::IDD, pParent)
{
    m_iCurSceneID = 0;
}

CSetSceneDlg::~CSetSceneDlg()
{
}

void CSetSceneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetSceneDlg, CDialogEx)
	
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CSetSceneDlg 消息处理程序


BOOL CSetSceneDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(pMsg->message)
	{
	case WM_LBUTTONUP:
		{
			UINT id = GetWindowLong(pMsg->hwnd, GWL_ID);
			if (id >= BTN_ID && id < (BTN_ID+ SCENE_SIZE - 1))
			{
				SYSTEM_PARAM config;
				GetSysParam(&config);
				HWND hwnd = ::FindWindowA(NULL,config.pstrSceneName[m_iCurSceneID].c_str());

				if (NULL == hwnd)
				{
					return FALSE;
				}
				m_iCurSceneID = id - BTN_ID - 1;
				//向主调试窗口发送切换消息
				::SendMessageW(hwnd,SET_SCENE,NULL,(LPARAM)(m_iCurSceneID)/*m_strTemp.GetBuffer()*/);
				CSetSceneDlg::OnOK();
			}
		}
		break;
    case WA_INACTIVE:
    case WM_KILLFOCUS:
    case WM_MOUSEACTIVATE:
    //case WM_NCMOUSEMOVE:
    case WM_CAPTURECHANGED:
        {
            ShowWindow(SW_HIDE);
        }
	default:
		break;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CSetSceneDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	int  iNum = 0;
    int  iRow = 0;
    int  iCol = 0;
	int  xb = 0;
	int  yb = 0;
	int  xe = 0;
	int  ye = 0;
	int  iWidth = 0;
	int  iHeight = 0;

    SetBackgroundColor(BACKGROUND_COLOR_M);
	int m = 0;
	m_iIdBegin = BTN_ID;
	SYSTEM_PARAM config;
	GetSysParam(&config);
	iNum =config.iSceneNum;
	iCol = (int)sqrt((float)iNum); //计算前用浮点型， 计算后改为int取整
	if (iNum > iCol*iCol)
	{
		iCol++;
    }
    iRow = iCol - (iCol * iCol - iNum)/iCol;
	CRect rt;
	CRect rtPic;
	GetClientRect(rt);
	
	
	iWidth = ((rt.Width() - 5)/iCol)-5;
	iHeight = ((rt.Height() - 5)/iRow)-5;
    int iscene = 0;
    SYSTEM_PARAM sysPara;
    GetSysParam(&sysPara);
	for (int i = 0; i < iRow; i++)
	{
		
		for ( int j = 0; j < iCol; j++)
		{
			m++;
			if (m > iNum)
			{
				break;
			}
			xb = j*iWidth + 10;
			yb = i*iHeight + 10;
			xe = (j+1)*iWidth;
			ye = (i+1)*iHeight;
			
			m_iHeight = ye+10;
			m_iWidth = rt.Width();

			m_iIdBegin = m_iIdBegin+1;
			m_pButton = new CButton();
			CString str;
			str = sysPara.pstrSceneName[iscene].c_str();
			m_pButton->Create(str,WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE,CRect(xb,yb,xe,ye), this,m_iIdBegin);
			iscene++;
		}
		if (m < 2)
		{
			GetDlgItem(BTN_ID+1)->MoveWindow(5,5,m_iWidth/3,m_iHeight/3);
			MoveWindow(0,0,m_iWidth/3+15,m_iHeight/3+15);
		}
		else
		{
			MoveWindow(0,0,m_iWidth,m_iHeight);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetSceneDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CDialogEx::OnActivate(nState, pWndOther, bMinimized);
    if(nState == WA_INACTIVE)
    {
        ShowWindow(SW_HIDE);
    }
    // TODO: 在此处添加消息处理程序代码
}
