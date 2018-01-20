
// PasswordAlterDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

struct databag
{
    char UserName[64];
    char UserSecreat[64];
    long EncryptStrLen;
};
// CPasswordAlterDlg 对话框
class CPasswordAlterDlg : public CDialogEx
{
// 构造
public:
	CPasswordAlterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PASSWORDALTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:
    void Init();
    bool ReadPassword(CString Username);
    bool WritePasswordComp_Upd(CString Username);
    void WritePasswordDat();
private:
    //databag userbag[2];
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
    afx_msg void OnBnClickedButtonAffirm();
    CComboBox m_CobUserName;
    afx_msg void OnBnClickedButtonCancel();
};
