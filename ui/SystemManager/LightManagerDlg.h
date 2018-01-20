#pragma once
#include "afxwin.h"


// LightManagerDlg 对话框

class LightManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LightManagerDlg)

public:
	LightManagerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~LightManagerDlg();

// 对话框数据
	enum { IDD = IDD_LIGHTDEBUG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonOpLight();
    virtual BOOL OnInitDialog();

protected:
    void InitLightComBox();
public:
    CComboBox m_CommList;
};
