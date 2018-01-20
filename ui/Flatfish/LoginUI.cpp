// Login.cpp : 实现文件
//

#include "stdafx.h"
#include "Flatfish.h"
#include "LoginUI.h"
#include "afxdialogex.h"
#include "common.h"


// CLogin 对话框

IMPLEMENT_DYNAMIC(CLogin, CDialogEx)

CLogin::CLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogin::IDD, pParent)
{

    FILE *pfile = NULL;
    pfile = fopen("user", "r");
    USER_LOGIN_DATA data;
    if (NULL != pfile)
    {
        int iSize = sizeof(USER_LOGIN_DATA);
        char szBuf[256] = {0};
        while (0 < fread(szBuf, iSize, sizeof(char), pfile))
        {
            memcpy(&data, szBuf, iSize);
            m_LoginList.push_back(data);
        }
    }
    if (m_LoginList.empty())
    {
        //data.iLevel = 0;
        //strcpy(data.szName,"op");
        //strcpy(data.szPassword,"123");
        //m_LoginList.push_back(data);
        //m_ComboName.AddString(data.szName);
        data.iLevel = 1;
        strcpy(data.szName,"admin");
        strcpy(data.szPassword,"tod8888");
        m_LoginList.push_back(data);
        data.iLevel = 2;
        strcpy(data.szName,"enginee");
        strcpy(data.szPassword,"Tod_123");
        m_LoginList.push_back(data);
    }
    if (NULL != pfile)
    {
       fclose(pfile);
    }

}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_USERNAME, m_ComboName);
}


BEGIN_MESSAGE_MAP(CLogin, CDialogEx)
    ON_BN_CLICKED(IDOK, &CLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// CLogin 消息处理程序
//初始化用户名密码


BOOL CLogin::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetBackgroundColor(BACKGROUND_COLOR_M);
    CenterWindow(GetParent());
    //初始化Commbox， 默认为OP

    USER_LOGIN_DATA data;
    list<USER_LOGIN_DATA>::iterator it;
    for (it = m_LoginList.begin(); it != m_LoginList.end(); it++)
    {
        data = *it;
        m_ComboName.AddString(data.szName);
    }
    m_ComboName.SetCurSel(0);
    GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


//登陆
void CLogin::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    //有效性判断
    CString strName;
    GetDlgItem(IDC_COMBO_USERNAME)->GetWindowText(strName);
    CString strPassword;
    GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(strPassword);
    list<USER_LOGIN_DATA>::iterator it;
    for (it = m_LoginList.begin(); it != m_LoginList.end(); it++)
    {
        if (0 == stricmp((*it).szName, strName) && 0 == stricmp((*it).szPassword, strPassword))
        {
            msg_send(MSG_LOGIN_SYSTEM,&(*it).iLevel,MODULE_ALL, EVENT_LOGIN_MSG);
        }
    }
    CDialogEx::OnOK();
}

