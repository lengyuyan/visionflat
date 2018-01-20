
// LightIntefaceTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LightIntefaceTest.h"
#include "LightIntefaceTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

string wcharTostring(wchar_t *orc)
{
	size_t len = wcslen(orc) + 1;
	size_t converted = 0;
	char *tem=(char*)malloc(len*sizeof(char));
	wcstombs_s(&converted, tem, len, orc, len);
	string comname(tem);
	return comname;

}

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


// CLightIntefaceTestDlg 对话框




CLightIntefaceTestDlg::CLightIntefaceTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLightIntefaceTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLightIntefaceTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, m_cb_com);
	DDX_Control(pDX, IDC_COMBO_INDEX, m_cb_index);
	DDX_Control(pDX, IDC_SLIDER_VALUE, m_slider_value);
	DDX_Control(pDX, IDC_COMBO_LIGHT_TYPE, m_cb_lighttype);
}

BEGIN_MESSAGE_MAP(CLightIntefaceTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CLightIntefaceTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CLightIntefaceTestDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CLightIntefaceTestDlg::OnBnClickedButtonClose)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_GETLIGHT_V, &CLightIntefaceTestDlg::OnBnClickedButtonGetlightV)
END_MESSAGE_MAP()


// CLightIntefaceTestDlg 消息处理程序

BOOL CLightIntefaceTestDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码

	//可以通过遍历注册表项
	//HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM\

	m_cb_com.AddString(_T("COM1"));
	m_cb_com.AddString(_T("COM2"));
	m_cb_com.AddString(_T("COM3"));
	m_cb_com.AddString(_T("COM4"));
	m_cb_com.AddString(_T("COM5"));
	m_cb_com.AddString(_T("COM6"));
	m_cb_com.AddString(_T("COM7"));
	m_cb_com.AddString(_T("COM8"));
	m_cb_com.SetCurSel(0);
	HKEY hKey;  
	int rtn;
	CString str;
	//    m_cmbComm.ResetContent();
	rtn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("Hardware\DeviceMap\SerialComm"),   
		NULL, KEY_READ, &hKey);
	if( rtn == ERROR_SUCCESS)   //   打开串口注册表   
	{   
		int   i=0;   
		char   portName[256],commName[256];   
		DWORD   dwLong,dwSize;   
		while(1)   
		{   
			dwSize =   sizeof(portName);   
			dwLong   =   dwSize;
			rtn = RegEnumValue( hKey, i, (LPWSTR)portName, &dwLong,   
				NULL, NULL, (PUCHAR)commName, &dwSize );
			if( rtn == ERROR_NO_MORE_ITEMS )   //   枚举串口   
				break;   //   commName就是串口名字  
			else
			{
				MessageBox((LPWSTR)commName);
				MessageBox((LPWSTR)portName);
				m_cb_com.AddString((LPWSTR)commName);
			}
			i++;   
		}   
		RegCloseKey(hKey);   
	}  
	m_cb_index.AddString(_T("1"));
	m_cb_index.AddString(_T("2"));
	m_cb_index.AddString(_T("3"));
	m_cb_index.AddString(_T("4"));
	m_cb_index.SetCurSel(0);

	m_cb_lighttype.AddString(_T("opt"));
	m_cb_lighttype.AddString(_T("ndps"));
	m_cb_lighttype.SetCurSel(0);

	m_slider_value.SetRange(0,255);
	m_slider_value.SetTicFreq(1);
	
	InitLight();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLightIntefaceTestDlg::InitLight()
{
	iLightInstance();
	LightInfo info[2]={};
	info[0].PortName = "COM1";
	info[0].LightType = LIGHTCTL_TYPE_OPT;

	initLightCtrl(info,2);
}

void CLightIntefaceTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLightIntefaceTestDlg::OnPaint()
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
HCURSOR CLightIntefaceTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLightIntefaceTestDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CLightIntefaceTestDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  temp;
	m_cb_com.GetWindowText(temp);
	m_name = wcharTostring(temp.GetBuffer(temp.GetLength()));	
	 
	m_cb_index.GetWindowText(temp);
	m_index = _ttoi(temp);
	
	openLight(m_name,m_index);
}


void CLightIntefaceTestDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  temp;
	m_cb_com.GetWindowText(temp);
	m_name = wcharTostring(temp.GetBuffer(temp.GetLength()));	

	m_cb_index.GetWindowText(temp);
	m_index = _ttoi(temp);

	closeLight(m_name,m_index);
}


void CLightIntefaceTestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar; 
	CString strText; 
	unsigned int val=0;

	//多个 SLIDER 控件控制 
	switch( pSlider->GetDlgCtrlID() ) 
	{ 
	case IDC_SLIDER_VALUE:
		val = pSlider->GetPos();
		strText.Format( _T("亮度值(0--255)：%d"), val ); 
		SetDlgItemText( IDC_STATIC_VALUE, strText ); 
		CString  temp;
		m_cb_com.GetWindowText(temp);
		m_name = wcharTostring(temp.GetBuffer(temp.GetLength()));	

		m_cb_index.GetWindowText(temp);
		m_index = _ttoi(temp);
		setLightValue(m_name,m_index,val);		
		break; 
	} 
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CLightIntefaceTestDlg::OnBnClickedButtonGetlightV()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  temp;
	m_cb_com.GetWindowText(temp);
	m_name = wcharTostring(temp.GetBuffer(temp.GetLength()));	

	m_cb_index.GetWindowText(temp);
	m_index = _ttoi(temp);
	unsigned int val = 0;
	getLightValue(m_name,m_index,val);

	m_slider_value.SetPos(val);
	CString strText;
	strText.Format( _T("亮度值(0--255)：%d"), val ); 
	SetDlgItemText( IDC_STATIC_VALUE, strText ); 
}
