
// ImageIntefaceTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageIntefaceTest.h"
#include "ImageIntefaceTestDlg.h"
#include "afxdialogex.h"
#include "iImageCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CImageIntefaceTestDlg 对话框




CImageIntefaceTestDlg::CImageIntefaceTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageIntefaceTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_imageWidth=2599;
	m_imageHeight = 1950;
}

void CImageIntefaceTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGEWIND_FIRST, m_static_windfirst);
	DDX_Control(pDX, IDC_STATIC_IMAGEWIND_SECOND, m_static_windsecond);
	DDX_Control(pDX, IDC_SLIDER_EXT, m_slider_exm);
	DDX_Control(pDX, IDC_SLIDER_GAIN, m_slider_gain);
}

BEGIN_MESSAGE_MAP(CImageIntefaceTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GETIMAGE_FIRSTR, &CImageIntefaceTestDlg::OnBnClickedButtonGetimageFirstr)
	ON_BN_CLICKED(IDC_BUTTON_GETIMAGE_FIRST, &CImageIntefaceTestDlg::OnBnClickedButtonGetimageFirst)
	ON_BN_CLICKED(IDC_BUTTON_VEDIO_FIRST, &CImageIntefaceTestDlg::OnBnClickedButtonVedioFirst)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_EXT, &CImageIntefaceTestDlg::OnNMCustomdrawSliderExt)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CImageIntefaceTestDlg 消息处理程序

BOOL CImageIntefaceTestDlg::OnInitDialog()
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

	m_slider_exm.SetRange(0,2000);
	m_slider_exm.SetTicFreq(1);

	m_slider_gain.SetRange(8,48);
	m_slider_gain.SetTicFreq(1);

	// TODO: 在此添加额外的初始化代码
	gen_image_const(&m_hImage,"byte",m_imageWidth,m_imageHeight);
	get_image_size(m_hImage, &m_imageWidth, &m_imageHeight);
	set_window_attr("background_color","black");
	open_window(0,0,m_imageWidth,m_imageHeight,(int)m_static_windfirst.m_hWnd,"","visible",&m_diswin_first);
	set_part((HTuple)m_diswin_first, 0,0,m_imageWidth -1, m_imageHeight-1);

	open_window(0,0,m_imageWidth,m_imageHeight,(int)m_static_windfirst.m_hWnd,"","visible",&m_diswin_second);
	set_part((HTuple)m_diswin_second, 0,0, m_imageHeight-1, m_imageWidth-1);

	imageInstance(CAMARA_TYPE_AVT_1394);
	list<string> guidlist;
	getGUIDList(guidlist);
	m_list_str_it = guidlist.begin();
	m_name = m_list_str_it->c_str();
	int res = openCamera(m_list_str_it->c_str());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CImageIntefaceTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CImageIntefaceTestDlg::OnPaint()
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
HCURSOR CImageIntefaceTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CImageIntefaceTestDlg::OnBnClickedButtonGetimageFirstr()
{
	// TODO: 在此添加控件通知处理程序代码
	unsigned char *ptr = NULL;
	char typ[128];
	long w,h;
	get_image_pointer1(m_hImage, (Hlong *)&ptr, typ, &w, &h);
	getImage(m_name,(void *)ptr);
	disp_obj(m_hImage,m_diswin_first);
}


void CImageIntefaceTestDlg::OnBnClickedButtonGetimageFirst()
{
	// TODO: 在此添加控件通知处理程序代码
	getImage(m_list_str_it->c_str(),&m_hImage);
	disp_obj(m_hImage,m_diswin_second);
}

void VedioCallback(void *context)
{
	CImageIntefaceTestDlg * m_this = (CImageIntefaceTestDlg*)context;
	disp_obj(m_this->m_hImage,m_this->m_diswin_first);
}

void CImageIntefaceTestDlg::OnBnClickedButtonVedioFirst()
{
	// TODO: 在此添加控件通知处理程序代码
	unsigned char *ptr = NULL;
	char typ[128];
	long w,h;
	get_image_pointer1(m_hImage, (Hlong *)&ptr, typ, &w, &h);
	callback = VedioCallback;
	startSnap(m_name,ptr,callback,this);
}

void CImageIntefaceTestDlg::OnNMCustomdrawSliderExt(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
}


void CImageIntefaceTestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar; 
	CString strText; 
	unsigned int val=0;

	//多个 SLIDER 控件控制 
	switch( pSlider->GetDlgCtrlID() ) 
	{ 
	case IDC_SLIDER_EXT:
		val = pSlider->GetPos();
		strText.Format( _T("%d"), val ); 
		SetDlgItemText( IDC_STATIC_EXM_DIS, strText ); 
		setExposureTime(m_name,val);		
		break; 
	 case IDC_SLIDER_GAIN: 
		  val = pSlider->GetPos();
		 strText.Format( _T("%d"), val ); 
		 SetDlgItemText( IDC_STATIC_GAIN_DIS, strText ); 
		 setGain(m_name,val);		 
		 break;
	} 

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
