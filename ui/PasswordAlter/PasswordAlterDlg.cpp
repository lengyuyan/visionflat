
// PasswordAlterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PasswordAlter.h"
#include "PasswordAlterDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
databag userbag[2] = {0};
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPasswordAlterDlg 对话框




CPasswordAlterDlg::CPasswordAlterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPasswordAlterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPasswordAlterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_USERNAME, m_CobUserName);
}

BEGIN_MESSAGE_MAP(CPasswordAlterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_AFFIRM, &CPasswordAlterDlg::OnBnClickedButtonAffirm)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CPasswordAlterDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CPasswordAlterDlg 消息处理程序

BOOL CPasswordAlterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
    SetBackgroundColor(BACKGROUND_COLOR_M);
    this->SetWindowText("密码修改");
	// TODO: 在此添加额外的初始化代码
    Init(); 

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPasswordAlterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPasswordAlterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPasswordAlterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPasswordAlterDlg::Init()
{
    m_CobUserName.InsertString(0,"admin");
    m_CobUserName.InsertString(1,"engineer");

    m_CobUserName.SetCurSel(0);
}

void CPasswordAlterDlg::OnBnClickedButtonAffirm()
{
    // TODO: 在此添加控件通知处理程序代码
    //读取界面选择的用户
    CString str;
    m_CobUserName.GetLBText(m_CobUserName.GetCurSel(),str);

    //读取密码并解码
    if(!ReadPassword(str))
    {
        return;
    }

    //比较两次输入新密码是否相同，并且更新数组
   if(!WritePasswordComp_Upd(str))
   {
       return;
   }

    ofstream outfile("password.dat",ios::binary);
    if (!outfile)
    {
        MessageBox("Open error!");
        return;
    }
    for(int i=0;i<2;i++)
    {
        int iS = sizeof(userbag[i]);
        outfile.write((char*)&userbag[i],sizeof(userbag[i]));
    }
    outfile.close( );

    MessageBox("修改密码成功");
    OnOK();
}

bool CPasswordAlterDlg::WritePasswordComp_Upd(CString Username)
{
    CString newpasswordstr;
    CString renewpasswordstr;
    CEdit *pEditCurrentPassword = (CEdit *)GetDlgItem(IDC_EDIT_CURRENT_PASSWORD);
    CEdit *pEditRecurrentPassword = (CEdit *)GetDlgItem(IDC_EDIT_RECURRENT_PASSWORD);
    pEditCurrentPassword->GetWindowText(newpasswordstr);
    pEditRecurrentPassword->GetWindowText(renewpasswordstr);
    if (0 != strcmp(newpasswordstr,renewpasswordstr))
    {
        MessageBox("两次密码输入不一致");
        pEditRecurrentPassword->SetFocus();
        pEditRecurrentPassword->SetSel(0,-1);
        return false;
    }

    for (int i=0; i<2; i++) 
    {
        if (0 == strcmp(userbag[i].UserName,Username))
        {

            char Out[64] = {0};
            char *In =  (char *)(LPCTSTR)newpasswordstr;
            Execution_3DES(Out,In,newpasswordstr.GetLength(),"0123", strlen("0123"), true);

            memcpy(userbag[i].UserSecreat,Out,newpasswordstr.GetLength()+7);
            userbag[i].EncryptStrLen = newpasswordstr.GetLength();
        }
    }
    return true;
}

bool CPasswordAlterDlg::ReadPassword(CString Username)
{
    ifstream infile("password.dat",ios::binary);
    if (!infile)
    {
        //如果password文件丢失，则新建一个password.dat
        //默认    admin   tod8888
        //        engineer  Tod_123
        WritePasswordDat();
    }
    for(int i=0; i<2; i++)
    {
        infile.read((char*)&userbag[i],sizeof(userbag[i]));
    }
    infile.close();

    CString oldpasswordstr;
    CEdit *pEditOriginalPassword = (CEdit *)GetDlgItem(IDC_EDIT_ORIGINAL_PASSWORD);
    pEditOriginalPassword->GetWindowText(oldpasswordstr);
    for (int i=0; i<2; i++)
    {
        if (0 == strcmp(userbag[i].UserName,Username))
        {
            char Out[64] = {0};
            Execution_3DES(Out,userbag[i].UserSecreat,userbag[i].EncryptStrLen,"0123", strlen("0123"), false);
            strcpy_s(userbag[i].UserSecreat,64,Out);
            if (0 != strcmp(userbag[i].UserSecreat,oldpasswordstr))
            {
                MessageBox("原始密码输入错误");
                pEditOriginalPassword->SetFocus();
                pEditOriginalPassword->SetSel(0,-1);
                return false;
            }
        }
    }
    return true;
}

void CPasswordAlterDlg::WritePasswordDat()
{
    char Out_admin[64] = {0};
    char Out_enginee[64] = {0};
    Execution_3DES(Out_admin,"tod8888",strlen("tod8888"),"0123", strlen("0123"), true);
    Execution_3DES(Out_enginee,"Tod_123",strlen("Tod_123"),"0123", strlen("0123"), true);

    strcpy(userbag[0].UserName, "admin");
    memcpy(userbag[0].UserSecreat,Out_admin,strlen("tod8888")+7);
    userbag[0].EncryptStrLen = strlen("tod8888");

    strcpy(userbag[1].UserName, "engineer");
    memcpy(userbag[1].UserSecreat,Out_enginee,strlen("Tod_123")+7);
    userbag[1].EncryptStrLen = strlen("Tod_123");


    ofstream outfile("password.dat",ios::binary);
    for(int i=0;i<2;i++)
    {
        outfile.write((char*)&userbag[i],sizeof(userbag[i]));
    }
    outfile.close( );
}

void CPasswordAlterDlg::OnBnClickedButtonCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    //OnCancel();
    OnOK();
}
