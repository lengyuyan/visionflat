#pragma once

#include "iManagerCtrl.h"
// CSetSceneDlg 对话框
#define  SET_SCENE    WM_USER +1100  
#define  BTN_ID   3000


class CSetSceneDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSceneDlg)

public:
	CSetSceneDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetSceneDlg();

// 对话框数据
	enum { IDD = IDD_DLG_SET_SCENE };

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	int m_iCurSceneID;
	CButton *m_pButton;
	UINT m_iIdBegin ;
	CString m_strTemp;
	int m_iHeight;
    int m_iWidth;
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
