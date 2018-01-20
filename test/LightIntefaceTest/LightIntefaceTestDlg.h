
// LightIntefaceTestDlg.h : 头文件
//

#pragma once
#include "iLightCtrl.h"
#include "afxwin.h"
#include "afxcmn.h"

// CLightIntefaceTestDlg 对话框
class CLightIntefaceTestDlg : public CDialogEx
{
// 构造
public:
	CLightIntefaceTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LIGHTINTEFACETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cb_com;
	CComboBox m_cb_index;
	CSliderCtrl m_slider_value;
	afx_msg void OnBnClickedOk();
	CComboBox m_cb_lighttype;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
	void InitLight();

	string m_name;
	int m_index;
	LIGHTCTL_TYPE m_ltype;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonGetlightV();
};
