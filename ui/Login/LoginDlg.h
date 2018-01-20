#pragma once
#include <list>
using namespace std;

struct USER_LOGIN_DATA 
{
    int iLevel;            //用户级别          
    char szName[64];       //用户名
    char szPassword[64];   //密码
};

// CLoginDlg 对话框

struct databag
{
    char UserName[64];
    char UserSecreat[64];
    long EncryptStrLen;
};

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

    // 对话框数据
    enum { IDD = IDD_DIALOG_LOGIN };
public:
    void InitGetDat();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();

private:
    list<USER_LOGIN_DATA> m_LoginList;
public:
    CComboBox m_ComboName;
    afx_msg void OnBnClickedCancel();

};
