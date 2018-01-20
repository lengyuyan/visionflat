#pragma once
#include "list"
#include "afxwin.h"
using namespace std;

struct USER_LOGIN_DATA 
{
    int iLevel;            //用户级别          
    char szName[16];       //用户名
    char szPassword[16];   //密码
};

// CLogin 对话框

class CLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogin();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();

private:
    list<USER_LOGIN_DATA> m_LoginList;
public:
    CComboBox m_ComboName;
};
