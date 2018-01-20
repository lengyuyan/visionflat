#pragma once

#include "iManagerCtrl.h"
#include "SetSceneDlg.h"
// CSetSceneDlg 对话框
#define  SET_PICINDEX    WM_USER +1200  
#define  PIC_ID   2200



class CSetPicindexDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetPicindexDlg)

public:
	CSetPicindexDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetPicindexDlg();

	// 对话框数据
	enum { IDD = IDD_DIG_SET_PICINDEX };

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnSetScene( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

public:

	CButton *m_pButton;
	UINT m_iIdBegin ;
	CString m_strTemp;
	int m_iscene;
	int m_iHeight;
	int m_iWidth;

};
