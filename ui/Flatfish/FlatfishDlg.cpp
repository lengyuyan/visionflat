
// FlatFishDlg.cpp : 实现文件
//

#pragma  once
#include "stdafx.h"
#include "FlatFish.h"
#include "FlatFishDlg.h"
//#include "afxdialogex.h"
#include "iManagerCtrl.h"
#include "DataType.h" 
#include "map"
#include "sysapi.h"//调用了获取路径

// CFlatFishDlg 消息处理程序
#include "assert.h"
//#include "HalconCpp.h"
//using namespace Halcon;
#include <complex>

#define UPDATE_STATUS_TIMER 1000       //刷新状态栏时间定时器
#define FLATFISH_STATUS_TIME 1000      //状态栏时间定时器间隔
#define  VISION_MANUAL_MSG       WM_USER + 1000    //业务消息

#define MAX_WIDTH   1024
#define MAX_HEIGHT  768

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFlatFishDlg 对话框

typedef void (WINAPI ShowDebugDlg)(int);
typedef int (WINAPI ShowLoginDlg)(void);
typedef void (WINAPI BuildScene)(const char*,int );//
typedef void  (WINAPI GrabWay2Image)(int ,bool);
//typedef BOOL (*pSceneProcess)(int,CHECK_RESULT&);

static UINT indicators[] =
{
    IDS_STRING_CONNECT_STATUS,        //与服务器连接状态    
    IDS_STRING_TIME,                  //系统时间
};

#define FLATFISH_ERROR_BASECODE -1000//定义此模块错误码
#define FLATFISH_STATUSPAN_NOTREADY FLATFISH_ERROR_BASECODE-1  //状态栏没有准备好

CFlatFishDlg *CFlatFishDlg::m_pInstance = NULL;
bool   CFlatFishDlg::isstatusready = false;

CFlatFishDlg::CFlatFishDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFlatFishDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pInstance = this;
    m_pIndicators = NULL;
    m_IndicatorsSize = 2;			//默认为2个为服务端连接状态、及时间显示
    m_hSceneDebugDLL = NULL;		//场景调试
    m_hSystemSettingDLL = NULL;		//系统调试
    m_hLoginDLL = NULL;				//登陆窗口
	m_IsSceneInit = false;			//场景是否初始化

}

void CFlatFishDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFlatFishDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

    //自定义业务消息
    ON_MESSAGE(VISION_MANUAL_MSG, &CFlatFishDlg::OnManualMessage)


    ON_BN_CLICKED(IDC_BUTTON_SHOW_SCENE_WIN, &CFlatFishDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFlatFishDlg::OnBnClickedButton2)
    ON_WM_SIZE()
    ON_WM_SHOWWINDOW()
    ON_WM_CTLCOLOR()
    ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CFlatFishDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标
	SetIcon(AfxGetApp()->LoadIcon(IDI_BIG_ICON), TRUE);
	SetIcon(AfxGetApp()->LoadIcon(IDI_BIG_ICON), FALSE);

	char szText[256] = {0};
	::GetWindowTextA(m_hWnd,szText,sizeof(szText));
	m_TitleName = szText;

    register_notify(MODULE_SCENE_MAIN_UI, OnSDKMessage,this);
    InitManager();  //iManagerCtrl库中的函数 打开相机
    MoveWindow(0, 0, MAX_WIDTH, MAX_HEIGHT);  //从配置读取   
    SetBackgroundColor(BACKGROUND_COLOR_M);

	//加载库
	initSettingDll();
	//初始化UI窗口
	initLayout();
	// 建立各个场景对象
	m_IsSceneInit = false;
	initSceneList();
	// UI拍照
	GrabSceneInit();
    initStatusBar(); 
    //打开状态检测线程
    OpenCheckStateThread();

    SYSTEM_PARAM* pSparam =(SYSTEM_PARAM*)GetSysParamPoint();

	PROCTOCOLDATA proctoData ;
	pSparam->stSocketParam.GetProctocolData(proctoData);
	proctoData.MakeEndable();
	
	SetWebProctocol(proctoData);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFlatFishDlg::initSettingDll()
{
    char szPath[512] = {0};
    get_module_path((LPSTR)szPath);
    CString strDll;
    strDll = szPath;
    strDll = strDll + "SceneManager.dll";
    m_hSceneDebugDLL = LoadLibrary(strDll);
    strDll = szPath;
    strDll = strDll + "SystemManager.dll";
    m_hSystemSettingDLL = LoadLibrary(strDll);
    strDll = szPath;
    strDll = strDll + "Login.dll";
    m_hLoginDLL = LoadLibrary(strDll);
}
void  CFlatFishDlg::initSceneList()
{
	if (m_IsSceneInit)
	{
		return;
	}
	m_IsSceneInit = true;
	int SceneNum = GetSceneNum();
	BuildScene*  pBuildScene = NULL;

	pBuildScene = (BuildScene*)GetProcAddress(m_hSceneDebugDLL, "IniSceneData"); //


	for (int i = 0; i < SceneNum; i++)
	{		
		if( 0 == GetSceneProcType(i))   //判断是否是0 == ProcProcess
		{
		     //建立每个场景及其中的图像处理。
			if (NULL != pBuildScene)	
			{		
		         pBuildScene(GetSceneConfigPath(GetSceneId(i)),GetSceneId(i));
			}

		}
	}
	
	GetSceneProcess((pSceneProcess*)GetProcAddress(m_hSceneDebugDLL, "SceneVisionProcess"));

}

void CFlatFishDlg::unLoadDll()
{
    if (NULL != m_hSceneDebugDLL)
    {
        FreeLibrary(m_hSceneDebugDLL);
        m_hSceneDebugDLL = NULL;
    }
    if (NULL != m_hSystemSettingDLL)
    {
        FreeLibrary(m_hSystemSettingDLL);
        m_hSystemSettingDLL = NULL;
    }
    if (NULL != m_hLoginDLL)
    {
        FreeLibrary(m_hLoginDLL);
        m_hLoginDLL = NULL;
    }
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFlatFishDlg::OnPaint()
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
HCURSOR CFlatFishDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//功能：主窗口注册消息后,别的模块调用msg_post()可以向主窗口发送消息,即回调此函数
//wParam:业务消息类型
//msgType:系统消息类型
//lParam: 参数
int WINAPI CFlatFishDlg::OnSDKMessage( unsigned int wParam, void *lParam,void* otherparam)
{
    if (NULL == m_pInstance)
    {
        return false;
    }
	switch (wParam)
	{
    case MSG_RESET_SYSTEM:
        {
            InitManager();
            ResetFinishedSendMsg();         //增加重置系统返回函数
            ::SendMessage(m_pInstance->m_hWnd,VISION_MANUAL_MSG, wParam, (LPARAM)lParam); //界面刷新处理通过自定义消息处理
        }
        break;
	case MSG_DIS_MAINWND_NAME:
	    {
			//char szText[256]={0};
            char szText1[256]={0};
			//::GetWindowTextA(m_pInstance->m_hWnd,szText,sizeof(szText));
			sprintf_s(szText1,"%s_产品:%s",m_pInstance->m_TitleName.GetBuffer(),lParam);
			::SetWindowTextA(m_pInstance->m_hWnd,szText1);

	    }
		break;
	case MSG_DISP_CLIB_INFO:
		{
			//char szText[256]={0};
			char szText1[256]={0};
			//::GetWindowTextA(m_pInstance->m_hWnd,szText,sizeof(szText));
			sprintf_s(szText1,"标定错误:%s",lParam);
			::SetWindowTextA(m_pInstance->m_hWnd,szText1);

		}

	 break;
    default:
        {
            ::SendMessage(m_pInstance->m_hWnd,VISION_MANUAL_MSG, wParam, (LPARAM)lParam); //界面刷新处理通过自定义业务消息处理
        }
        break;
    }
    return 0;
}
/**********************************************************************
函数      : OnManualMessage
功能      : 处理发向主对话框业务消息
输入参数  : WPARAM wParam:消息类型
			LPARAM lParam ：消息参数 
输出参数  : N/A
返回值    : 
备注      : 
修改记录：
日期        修改人               修改内容
2016-08-16  guopengzhou          保存结果图片但是不要发送图片，
**********************************************************************/

LRESULT CFlatFishDlg::OnManualMessage( WPARAM wParam, LPARAM lParam )
{
    CListBox *pList = NULL;
    pList = (CListBox *)m_pInstance->GetDlgItem(IDC_LIST_RUN_INFO);
    switch (wParam)
    {
    case MSG_CLOSE_SYSTEM:
        {
            OnOK();
        }
        break;
    case MSG_DISP_IMAGE_DUMP:
        {	
            CString str;
            SYSTEM_PARAM *sysParam;
			SCENE_PKG pkg;
			char *buf = NULL;
			char path[MAX_PATH*2] = {0};
			int w = 0,h = 0,size = 0;
			sysParam = (SYSTEM_PARAM* )GetSysParamPoint();
            int id = *(int *)lParam;
            if (id < 0 || id >= sysParam->iSceneNum)
            {
                LOG_INFO("场景ID无效");
                return -1;
            }

          //  str.Format("视觉处理，处理序号%d",id);
         //   pList->AddString(str.GetBuffer());
          
            GetSceneParam(id, &pkg);
            //DispObj((int)m_vecPicItem[pkg.stationID]->m_hWnd ,id,true);
			SaveImageDump((int)m_vecPicItem[pkg.stationID]->m_hWnd ,id,path);

			//buf = (char*)GetImageBufByFile(path,size,w,h);
			//buf = (char*)GetImageDump((int)m_pInstance->m_vecPicItem[pkg.stationID]->m_hWnd,0,w,h);			
			//SendImageDump(buf,size,w,h,id);
			return 0;
        }
        break;
    case MSG_DISP_IMAGE:
        {
            SCENE_PKG config;
            SYSTEM_PARAM* sysParam;
			sysParam = (SYSTEM_PARAM* )GetSysParamPoint();
            int SceneID = *(int *)lParam;
            if (SceneID < 0 || SceneID >= sysParam->iSceneNum)
            {
                LOG_INFO("场景ID无效");
                return -1;
            }
            GetSceneParam(SceneID, &config);
            if (config.stationID >= m_pInstance->m_vecPicItem.size())
            {
                LOG_INFO("窗口不存在");
                //窗口不存在
                return -1;
            }
            DispObj((int)m_pInstance->m_vecPicItem[config.stationID]->m_hWnd, SceneID);
        }
        break;
    case MSG_RESET_SYSTEM:
        {
            pList->AddString("重置系统");
            pList->SetCurSel(pList->GetCount() - 1); 
        }
        break;
    case MSG_DISP_WORKING_INFO:
        {
            char *pStr = (char *)lParam;
            pList->AddString(pStr);
        }
        break;
    case MSG_CONNECT_FAILED:
        {
			if (false == isstatusready) //后续要加锁
			{
				LOG_ERR("状态栏有没有初始化成功,不能接受消息");
				return FLATFISH_STATUSPAN_NOTREADY;
			}
            m_statusBar.SetPaneText(0, "连接失败", 1); //状态栏有没有初始化成功,成功后才能处理
        }
        break;
    case MSG_CONNECT_SUCCESS:
        {
			if (false == isstatusready) //后续要加锁
			{
				LOG_ERR("状态栏有没有初始化成功,不能接受消息");
				return FLATFISH_STATUSPAN_NOTREADY;
			}
            m_statusBar.SetPaneText(0, "连接成功", 1);
        }
        break;
    case MSG_CONNECT_CAMERA_FAILED:
        {
			if (false == isstatusready) //后续要加锁
			{
				LOG_ERR("状态栏有没有初始化成功,不能接受消息");
				return FLATFISH_STATUSPAN_NOTREADY;
			}
            int iCamera = *(int *)lParam + 1;
            char szBuf[128] = {0};
            sprintf_s(szBuf, "相机%d连接失败", iCamera - 1);
            m_statusBar.SetPaneText(iCamera, szBuf, 1);
        }
        break;
    case MSG_CONNECT_CAMERA_SUCCESS:
        {
			if (false == isstatusready) //后续要加锁
			{
				LOG_ERR("状态栏有没有初始化成功,不能接受消息");
				return FLATFISH_STATUSPAN_NOTREADY;
			}
            int iCamera = *(int *)lParam + 1;
            char szBuf[128] = {0};
            sprintf_s(szBuf, "相机%d连接成功", iCamera - 1);
            m_statusBar.SetPaneText(iCamera, szBuf, 1);
        }
        break;
    case MSG_RESET_PARAMETER:
        {
            InitManager();
            initLayout();
            pList->AddString("重置参数成功");
        }
        break;
    case MSG_LOGIN_SYSTEM:
        {
            m_LoginLevel = *(int *)lParam;
        }
        break;
    default:
        break;
    }
    pList->SetCurSel(pList->GetCount() - 1); 
    return 0;
}
void CFlatFishDlg::initLayout()
{
    int indexImage = 0;
    int  iNum = 0;
    int  iRow = 0;
    int  iCol = 0;
    int  xb = 0;
    int  yb = 0;
    int  xe = 0;
    int  ye = 0;
    int  iWidth = 0;
    int  iHeight = 0;

    int  iPicID = 5000;
    SYSTEM_PARAM *config;
	config = (SYSTEM_PARAM *)GetSysParamPoint();
    iNum = config->iWindowNum;

	
    iCol = (int)sqrt((float)iNum);
    if (iNum > iCol * iCol)
    {
        iCol++;
    }
	//若满行满列,会多出几行,可以计算行数
    iRow = iCol - (iCol * iCol - iNum)/iCol;
    CRect rt;
    CRect rtPic;
    GetClientRect(rt);
    rt.bottom -= 20;   //去掉状态栏高度
    CWnd *pPic = GetDlgItem(IDC_STATIC_IMAGE);
    rtPic.left = rt.left+10;
    rtPic.top = rt.top+10;
    rtPic.bottom = rt.bottom - rt.Height()/5;
    rtPic.right = rt.right;
    pPic->MoveWindow(rtPic);
    iWidth = ((rtPic.Width() - 5)/iCol)-5;
    iHeight = ((rtPic.Height() - 5)/iRow)-5;
    CStatic *pstPic = NULL;

    //运行信息显示框
    CRect rtList;
    //rtList.left = rtPic.right + 15;
    rtList.left = rt.left+10;
    rtList.top = rtPic.bottom + 5;
    rtList.right = rt.right - 10 + - rt.Width()/5;
    rtList.bottom = rt.bottom - 5;
    GetDlgItem(IDC_LIST_RUN_INFO)->MoveWindow(rtList);

    //按钮
    int HeightV = rt.Height()/5 / 9;
    //rtList.left = rtPic.right + 45;
    rtList.left = rtList.right + 45;
    rtList.top = rt.bottom - (rt.Height()/5) + HeightV;
    rtList.right = rt.right - 30;
    rtList.bottom = rt.bottom - rt.Height()/5 + 4 * HeightV;
    GetDlgItem(IDC_BUTTON_SHOW_SCENE_WIN)->MoveWindow(rtList);

    //rtList.left = rtPic.right + 45;
    rtList.left = rtList.left;
    rtList.top = rt.bottom - (rt.Height()/5) + 5 * HeightV;
    rtList.right = rt.right - 30;
    rtList.bottom = rt.bottom - rt.Height()/5 + 8 * HeightV;
    GetDlgItem(IDC_BUTTON2)->MoveWindow(rtList);

    if (iNum == m_vecPicItem.size())
    {
        return;
    }
    else
    {
        for (int index = 0; index < m_vecPicItem.size(); index++)
        {
            delete m_vecPicItem[index];
            m_vecPicItem[index]=NULL;
        }
        m_vecPicItem.clear();
    }

	m_LayRow = iRow;
	m_LayCol = iCol;
    for (int i = 0; i < iRow; i++)
    {
        for (int j = 0; j < iCol; j++)
        {
            if ((i+1) * (j+1) > iNum)
            {
				//m_LayRow = 1;
				//m_LayCol = 1;
                break;
            }
            xb = j*iWidth + 1;
            yb = i*iHeight + 1;
            xe = (j+1)*iWidth;
            ye = (i+1)*iHeight;
            // 创建pic控件
            CRect rect;
            pstPic = new CStatic();
            pstPic->Create(NULL,WS_VISIBLE|SS_BITMAP|SS_BLACKFRAME|SS_CENTERIMAGE,CRect(xb,yb,xe,ye), GetDlgItem(IDC_STATIC_IMAGE), iPicID);
            pstPic->ShowWindow(SW_SHOW);
            pstPic->GetClientRect(rect);
            BindWindow((long)pstPic->m_hWnd,0, 0, xe-xb, ye - yb);//传进去窗口id,起点,宽度和高度
			if (indexImage> config->iCameraNum-1)
			{
				indexImage = config->iCameraNum-1;
			}
			RestoreImagePart((long)pstPic->m_hWnd,indexImage, xe-xb, ye-yb,indexImage); //根据相序号来划分显示窗口
           // DispImage((long)pstPic->m_hWnd, indexImage,0);
            indexImage++;
            m_vecPicItem.push_back(pstPic);
        }
    }
	SYSTEM_PARAM* pSysParam = (SYSTEM_PARAM*)GetSysParamPoint();
	assert(pSysParam);
	pSysParam->MainImageWndVec = &m_vecPicItem;
}

void  CFlatFishDlg::GrabSceneInit()
{
	int SceneId = 0;
	SCENE_PKG pscene;
	bool sendflag = false;
	for (int i = 0; i < m_LayRow; i++)
	{
		for (int j = 0; j < m_LayCol; j++)
		{
			//step1 根据场景ID找到对应配置
			GetSceneParam(SceneId, &pscene);   //indexImage 就是SceneID;
			if ( 0 == pscene.ProcProcess)
			{
				//调用场景动态库中的函数，以场景ID为参数，调用相应场景中的图像采集处理来采集图像。
				GrabWay2Image* pGrabWay2Image;
				pGrabWay2Image = (GrabWay2Image *)GetProcAddress(m_hSceneDebugDLL, "InitGrabWay2Sceneimage");
				pGrabWay2Image(SceneId,sendflag);			//采集并显示	
			}else
			{
				InitGrabSceneimage(SceneId); //里面有涉及到halocn的图像缓存指针
			}
			//根据相机索引来显示图像，一个相机显示一个窗口，因为相机同时只能在一个窗口显示，SceneId 改为 相机索引
			// DispImage((long)m_vecPicItem.at(SceneId)->m_hWnd, SceneId,0);//左后一个参数为一个场景中的第几个图像处理项
			 SceneId++;
		}
	}

}


void CFlatFishDlg::initStatusBar()
{
    m_statusBar.CreateEx(this, SBT_TOOLTIPS, CBRS_BOTTOM | WS_VISIBLE | WS_CHILD, AFX_IDW_STATUS_BAR); //创建状态栏
    m_statusBar.GetStatusBarCtrl().SetMinHeight(20);//设置状态栏的最小高度
    SYSTEM_PARAM *param;
   // GetSysParam(&param);
	param = (SYSTEM_PARAM *)GetSysParamPoint();
    m_IndicatorsSize += param->iCameraNum;
    m_IndicatorsSize += param->iLightNum;
    m_pIndicators = new UINT[m_IndicatorsSize];//状态栏格数为相机和灯光数目
    m_pIndicators[0] = IDS_STRING_CONNECT_STATUS; //第一格为网络连接状态栏
    int iBegin = 1;
    int iSize = param->iCameraNum + iBegin;//相机状态从第2格开始
    int j = 0;
    for (int i = iBegin; i < iSize; i++)
    {
        m_pIndicators[i] = IDS_STRING_CAMERA_0 + j; //这种写法就注定了状态栏只有16个相机,因为只有IDS_STRING_CAMERA_0到15
        j++;
    }
    iBegin = iSize;
    iSize = iSize + param->iLightNum + 1; //接着就是灯光的状态,灯光这里只有7个
    j = 0;
    for (int i = iBegin; i < iSize; i++)
    {
        m_pIndicators[i] = IDS_STRING_COMM_0 + j;
        j++;
    }
    m_pIndicators[m_IndicatorsSize - 1] = IDS_STRING_TIME; //最后一栏是时间
    m_statusBar.SetIndicators(m_pIndicators, m_IndicatorsSize); //设置状态栏数目：网络连接+相机(最多16)+灯光(最多8)+时间
    m_statusBar.GetStatusBarCtrl().SetBkColor(BACKGROUND_COLOR_M);

    int iWidth = 0;
    CRect rt;
    GetClientRect(&rt);
    iWidth = rt.Width() / m_IndicatorsSize;
    for (int i = 0; i < m_IndicatorsSize - 1; i++)
    {
        m_statusBar.SetPaneInfo(i, m_pIndicators[i], SBPS_NORMAL, iWidth); //平均分隔状态栏

    }
    m_statusBar.SetPaneInfo(m_IndicatorsSize - 1, m_pIndicators[m_IndicatorsSize - 1], SBPS_STRETCH, 0);

    m_statusBar.GetStatusBarCtrl().SetBkColor(BACKGROUND_COLOR_BAR);//设置状态栏颜色
    //屏幕上绘制状态栏时间分隔
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDS_STRING_TIME);

	isstatusready = true;//状态栏准备好了,可以接受消息了
    //定时刷新时间
    SetTimer(UPDATE_STATUS_TIMER, FLATFISH_STATUS_TIME, NULL);
	
}
 
void CFlatFishDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    SYSTEM_PARAM   *Sparam;
	Sparam = (SYSTEM_PARAM* )GetSysParamPoint();
    if (Sparam->bLandType)
    {
        m_LoginLevel = -1;
        ShowLoginDlg *pLoginDlg = NULL;
        if (NULL != m_hLoginDLL)
        {
            pLoginDlg = (ShowLoginDlg *)GetProcAddress(m_hLoginDLL, "ShowLoginDlg");//
            if (NULL != pLoginDlg)
            {
                m_LoginLevel = pLoginDlg();
            }
        }
    }
    else
    {
        m_LoginLevel = 2;
    }
	if (-1 <= m_LoginLevel)
	{
		 m_LoginLevel = 2;
	}
	 ////gpz
    if (1 < m_LoginLevel)
    {
       // msg_send(MSG_LOGIN_SYSTEM,&m_LoginLevel,MODULE_ALL, EVENT_LOGIN_MSG);
		msg_send(MSG_LOGIN_SYSTEM,&m_LoginLevel,sizeof(m_LoginLevel),MODULE_ALL);
        msg_post(RECV_UI_LOGIN_MSG,&m_LoginLevel,sizeof(int),MODULE_SCENE_MAIN_SDK);

        ShowDebugDlg *pShow;
        if (NULL != m_hSystemSettingDLL)
        {
            pShow = (ShowDebugDlg *)GetProcAddress(m_hSystemSettingDLL, "ShowSettingDebugDlg");
            if (NULL != pShow)
            {
                pShow(m_LoginLevel);
            }
        }
    }
    else if(0 <= m_LoginLevel)
    {
        AfxMessageBox("权限不够, 请切换为enginee");
    }
    else if (-1 == m_LoginLevel)
    {
        AfxMessageBox("密码错误");
    }
    else  //取消
    {
        ;
    }
}
void CFlatFishDlg::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    SYSTEM_PARAM*   Sparam;
	Sparam = (SYSTEM_PARAM* )GetSysParamPoint();
    if (Sparam->bLandType)
    {
        m_LoginLevel = -1;
        ShowLoginDlg *pLoginDlg = NULL;
        if (NULL != m_hLoginDLL)
        {
            pLoginDlg = (ShowLoginDlg *)GetProcAddress(m_hLoginDLL, "ShowLoginDlg");//
            if (NULL != pLoginDlg)
            {
                m_LoginLevel = pLoginDlg();
            }
        }
    }
    else
    {
        m_LoginLevel = 2;
    }
	if (-1 <= m_LoginLevel)
	{
		m_LoginLevel = 2;
	}	
    if (0 < m_LoginLevel)
    {
        msg_send(MSG_LOGIN_SYSTEM,&m_LoginLevel,sizeof(m_LoginLevel),MODULE_ALL);
        if (NULL != m_hSceneDebugDLL)
        {
            ShowDebugDlg *pShow;
            pShow = (ShowDebugDlg *)GetProcAddress(m_hSceneDebugDLL, "ShowSceneDebugDlg");
            if (NULL != pShow)
            {
                //msg_post(RECV_UI_LOGIN_MSG,&m_LoginLevel,sizeof(int),MODULE_SCENE_MAIN_SDK);
                pShow(m_LoginLevel);
            }
        }

    }
    else if(-4 == m_LoginLevel)
    {
        AfxMessageBox("权限不够");
    }
    else if (-5 == m_LoginLevel)
    {
        AfxMessageBox("密码错误");
    }
    else  //取消
    {
        ;
    }

}

void CFlatFishDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    // TODO: 在此处添加消息处理程序代码
    MoveWindow(0, 0, MAX_WIDTH, MAX_HEIGHT);
}

void CFlatFishDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    /*if (IDNO == AfxMessageBox("确认", MB_YESNO))
    {
        return;
    }*/
    CDialogEx::OnOK();
}

void CFlatFishDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    if (IDNO == AfxMessageBox("确认", MB_YESNO))
    {
        return;
    }
    CDialogEx::OnCancel();
}


HBRUSH CFlatFishDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性
    if(nCtlColor== IDC_LIST_RUN_INFO)
    {

        //hbr = CreateSolidBrush(RGB(255,0,255) );         //listbox背景颜色       
        //pDC->SetBkColor(RGB(255,0,255));
        //CRect rt;
        //GetWindowRect(&rt);
        //FillRect(*pDC, rt, hbr);
        return hbr;
    } 

    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}


BOOL CFlatFishDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (NULL != m_pIndicators)
    {
        delete m_pIndicators;
        m_pIndicators = NULL;
    }
    vector<CStatic *>::iterator it;
    CStatic *pTmp = NULL;
    for (it = m_vecPicItem.begin(); it != m_vecPicItem.end(); it++)
    {
        pTmp = *it;
        if (NULL != pTmp)
        {
            delete pTmp;
        }
    }
    UnInitManager();
    unregister_notify(MODULE_SCENE_MAIN_UI,OnSDKMessage);
    return CDialogEx::DestroyWindow();
}

void CFlatFishDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    switch (nIDEvent)
    {
    case UPDATE_STATUS_TIMER:
        {
            SYSTEMTIME st;
            char szBuf[64] = {0};
            GetLocalTime(&st);
            sprintf_s(szBuf, "%04d/%02d/%02d  %02d:%02d:%02d",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            m_statusBar.SetPaneText(m_IndicatorsSize-1, szBuf, 1);
        }
        break;
    }
    CDialogEx::OnTimer(nIDEvent);
}

void *CFlatFishDlg::GetImageBufByFile(char *path,int &size,int &x,int &h)
{
	CFileStatus fstatus; 
	CFile file; 
	LONG cb; 
	if (file.Open(path,CFile::modeRead)&&file.GetStatus(path,fstatus)&&((cb = (LONG)fstatus.m_size) != -1)) 
	{ 
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, cb); 
		LPVOID pvData = NULL; 
		if (hGlobal != NULL) 
		{ 
			pvData = GlobalLock(hGlobal);
			if (pvData != NULL) 
			{ 
				file.Read(pvData, cb); 
				GlobalUnlock(hGlobal); 
				size = cb;
				return pvData;
			}
		}
	}
	return NULL;
}


BOOL CFlatFishDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if(WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
    {//屏蔽esc键
        return TRUE;
    }
    if(WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)
    {//屏蔽enter
        return TRUE;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}
