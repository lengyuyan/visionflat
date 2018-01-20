// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Login.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include "common.h"

#define ADMIN_DEFAULT_NAME        "admin"
#define ADMIN_ENGINEE_NAME        "enginee"
#define ADMIN_DEFAULT_PASSWORD    "tod8888"
#define ADMIN_ENGINEE_PASSWORD    "Tod_123"


// CLoginDlg 对话框
using namespace std;
IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{
        InitGetDat();
   /* FILE *pfile = NULL;
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
        fclose(pfile);
    }
    if (m_LoginList.empty())
    {
        //data.iLevel = 0;
        //strcpy(data.szName,"op");
        //strcpy(data.szPassword,"123");
        //m_LoginList.push_back(data);
        //m_ComboName.AddString(data.szName);
        data.iLevel = 1;
        strcpy_s(data.szName,16,"admin");
        strcpy_s(data.szPassword,16,"tod8888");
        m_LoginList.push_back(data);
        data.iLevel = 2;
        strcpy_s(data.szName,16,"enginee");
        strcpy_s(data.szPassword,16,"Tod_123");
        m_LoginList.push_back(data);
    }*/
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_USERNAME, m_ComboName);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序

BOOL CLoginDlg::OnInitDialog()
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
void CLoginDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    //有效性判断
    int iLevels = -1;
    CString strName;
    GetDlgItem(IDC_COMBO_USERNAME)->GetWindowText(strName);
    CString strPassword;
    GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(strPassword);
    list<USER_LOGIN_DATA>::iterator it;
    for (it = m_LoginList.begin(); it != m_LoginList.end(); it++)
    {
        if (0 == stricmp((*it).szName, strName) && 0 == stricmp((*it).szPassword, strPassword))
        {
            iLevels = (*it).iLevel;
        }
    }
    EndDialog(iLevels);
    //CDialogEx::OnOK();
}


void CLoginDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    EndDialog(-2);
    //CDialogEx::OnCancel();
}
void CLoginDlg::InitGetDat()
{

    databag userbag[2];
    ifstream infile("password.dat",ios::binary);
    if (!infile)
    {
        //如果读取password.dat文件失败
        //则默认     admin    tod8888
        //           enginee  Tod_123
        USER_LOGIN_DATA data;
        if (m_LoginList.empty())
        {
            data.iLevel = 1;
            strcpy_s(data.szName,64,"admin");
            strcpy_s(data.szPassword,64,"tod8888");
            m_LoginList.push_back(data);
            data.iLevel = 2;
            strcpy_s(data.szName,64,"enginee");
            strcpy_s(data.szPassword,64,"Tod_123");
            m_LoginList.push_back(data);
        }
    }
    else
    {
        for(int i=0; i<2; i++)
        {
            char Out[64] = {0};
            USER_LOGIN_DATA data;
            infile.read((char*)&userbag[i],sizeof(userbag[i]));
            Execution_3DES(Out,userbag[i].UserSecreat,userbag[i].EncryptStrLen,"0123", strlen("0123"), false);

            data.iLevel = i+1;
            strcpy_s(data.szName,64,userbag[i].UserName);
            strcpy_s(data.szPassword,64,Out);
            m_LoginList.push_back(data);
        }
        infile.close();
    }

}