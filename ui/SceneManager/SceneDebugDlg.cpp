// SceneDebugDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sysapi.h"
#include "SceneManager.h"
#include "SceneDebugDlg.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"
#include "SetSceneDlg.h"
#include  <io.h>
// CSceneDebugDlg 对话框

IMPLEMENT_DYNAMIC(CSceneDebugDlg, CDialogEx)
#define  VISION_MANUAL_MSG       WM_USER + 1000    //显示图像消息

CSceneDebugDlg *CSceneDebugDlg::m_pInstance = NULL;

CSceneDebugDlg::CSceneDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneDebugDlg::IDD, pParent)
{
    m_LoginLevels = 0;
    m_iCurSceneID = 0;
	m_CurProcIndex = 0;
    m_iCurIndex = 0;
    m_hROI=NULL;
    m_bIsContinousGrab = false;
	m_index = 0;
    m_iCurGroup = 0;
	m_sceneChange = false;
	m_height = 0;
	m_iT = 0;
    memset(m_initControl, 1, SCENE_SIZE*sizeof(bool));
	memset(m_loopcount, 0, sizeof(m_loopcount));
	m_loop = false;
	g_SceneInitialCount++;
}

CSceneDebugDlg::~CSceneDebugDlg()
{
	m_SEBtn.clear();
	m_SRBtn.clear();
	//g_IsSceneDlgInitial = FALSE;
	g_SceneInitialCount++;
	g_SceneDlgHwinId = -1;//没有调试界面就不显示在调试界面
}

void CSceneDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FUNCTION, m_listFunction);
	DDX_Control(pDX,IDC_LIST_PIC,m_listPicture[0]);
	DDX_Control(pDX,IDC_LIST_PIC1,m_listPicture[1]);
	DDX_Control(pDX,IDC_LIST_PIC2,m_listPicture[2]);
	DDX_Control(pDX,IDC_LIST_PIC3,m_listPicture[3]);
	DDX_Control(pDX,IDC_CHECK_PIC,m_checkbox);
	DDX_Control(pDX,IDC_CBSCENDEBUG_COMPOSETYPE,m_RoiCombineType); 
	DDX_Control(pDX,IDC_BTSCENDEBUG_DRAWCOMPOSE,m_RoiCombFlag); 

	//


	DDX_Control(pDX, IDC_BUTTON_ADD_FUNCTION, m_BAddFun);
	DDX_Control(pDX, IDC_BUTTON_SUB_FUNCTION, m_BDelProc);
	DDX_Control(pDX, IDC_SCENEDLG_AXE, m_SelAxisIndex);
	DDX_Control(pDX, IDC_SCENEDLG_TOOL, m_SelToolIndex);
	//DDX_Control(pDX, IDC_SCENEDLG_SCENENO, m_CB_SceneNo);
	DDX_Control(pDX, IDC_SCENEDLG_RELASCENENO, m_CB_RelaSceneNo);
	DDX_Control(pDX, IDC_SCENEDLG_SCENENO, m_ED_SceneNo);
	DDX_Control(pDX, IDC_SCENEDLG_NEXTSCENENO, m_CB_NextSceneId);
	DDX_Control(pDX, IDC_SCENEDLG_NEXTSCENEROT, m_NextSceneRot);
}


BEGIN_MESSAGE_MAP(CSceneDebugDlg, CDialogEx)

    //自定义消息
    ON_MESSAGE(VISION_MANUAL_MSG, &CSceneDebugDlg::OnManualMessage)
	ON_MESSAGE(SET_SCENE, &CSceneDebugDlg::OnSetScene) //场景选择消息
    ON_MESSAGE(ADD_FUNCTION_MSG, &CSceneDebugDlg::OnAddFunction)//处理功能选项
    
    //窗口item消息
	ON_BN_CLICKED(IDC_BTN_CRABPIC, &CSceneDebugDlg::OnBnClickedBtnCrabpic)
	ON_BN_CLICKED(IDC_BTN_OPEN_FOLDER, &CSceneDebugDlg::OnBnClickedBtnOpenFolder)
	ON_BN_CLICKED(IDC_BTN_OPEN_SHOW, &CSceneDebugDlg::OnBnClickedBtnOpenShow)
	//ON_BN_CLICKED(IDC_BTN_CLOSE_SHOW, &CSceneDebugDlg::OnBnClickedBtnCloseShow)
	ON_BN_CLICKED(IDC_BTN_SAVEORIPIC, &CSceneDebugDlg::OnBnClickedBtnSaveoripic)
	ON_BN_CLICKED(IDC_BTN_REULTPIC, &CSceneDebugDlg::OnBnClickedBtnReultpic)
	ON_BN_CLICKED(IDC_BUTTON1, &CSceneDebugDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON12, &CSceneDebugDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BTN_OPEN_PICFLODER, &CSceneDebugDlg::OnBnClickedBtnOpenPicfloder)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CSceneDebugDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CSceneDebugDlg::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_ALL_SELECT, &CSceneDebugDlg::OnBnClickedBtnAllSelect)
	ON_BN_CLICKED(IDC_CHECK_PIC, &CSceneDebugDlg::OnBnClickedCheckPic)
	ON_BN_CLICKED(IDC_BTN_START, &CSceneDebugDlg::OnBnClickedBtnStart)
	ON_WM_TIMER()
    ON_BN_CLICKED(IDC_CHECK_IS_SHOW_TMP, &CSceneDebugDlg::OnBnClickedCheckIsShowTmp)
    ON_NOTIFY(NM_CLICK, IDC_LIST_FUNCTION, &CSceneDebugDlg::OnClickListFunction) //单击函数列表框相应
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_FUNCTION, &CSceneDebugDlg::OnDblclkListFunction)//双击，新方法无响应,旧方法显示区域
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PIC, &CSceneDebugDlg::OnDblclkListPicture)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PIC1, &CSceneDebugDlg::OnDblclkListPicture)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PIC2, &CSceneDebugDlg::OnDblclkListPicture)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PIC3, &CSceneDebugDlg::OnDblclkListPicture)
    ON_BN_CLICKED(IDC_BTN_SAVE_PARAM, &CSceneDebugDlg::OnBnClickedBtnSaveParam)
    ON_BN_CLICKED(IDC_BUTTON_ADD_FUNCTION, &CSceneDebugDlg::OnBnClickedButtonAddFunction)
    ON_BN_CLICKED(IDC_BUTTON_SUB_FUNCTION, &CSceneDebugDlg::OnBnClickedButtonSubFunction)
    ON_STN_DBLCLK(IDC_STATIC_BIND, &CSceneDebugDlg::OnDblclkStaticBind)
    ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CSceneDebugDlg::OnBnClickedButtonRestore)
    ON_BN_CLICKED(IDC_CHECK_IS_AFFINE, &CSceneDebugDlg::OnBnClickedCheckIsAffine)
    ON_BN_CLICKED(IDC_CHECK_IS_TOP, &CSceneDebugDlg::OnBnClickedCheckIsTop)
    ON_BN_CLICKED(IDC_RADIO_DIS_ALL, &CSceneDebugDlg::OnBnClickedRadioDisAll)
    ON_BN_CLICKED(IDC_RADIO_DIS_1, &CSceneDebugDlg::OnBnClickedRadioDis1)
    ON_BN_CLICKED(IDC_RADIO_DIS_2, &CSceneDebugDlg::OnBnClickedRadioDis2)
    ON_BN_CLICKED(IDC_RADIO_DIS_3, &CSceneDebugDlg::OnBnClickedRadioDis3)
    ON_BN_CLICKED(IDC_BUTTON_I1, &CSceneDebugDlg::OnBnClickedButtonI1)
    ON_BN_CLICKED(IDC_BUTTON_I2, &CSceneDebugDlg::OnBnClickedButtonI2)
    ON_BN_CLICKED(IDC_BUTTON_I3, &CSceneDebugDlg::OnBnClickedButtonI3)
    ON_BN_CLICKED(IDC_BUTTON_I4, &CSceneDebugDlg::OnBnClickedButtonI4)
	ON_BN_CLICKED(IDC_BUTTON_MORE_CONTTROL, &CSceneDebugDlg::OnBnClickedButtonMoreConttrol)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_CONTTROL, &CSceneDebugDlg::OnBnClickedButtonDeleteConttrol)
    ON_BN_CLICKED(IDC_CHECK_IS_SIMULATE, &CSceneDebugDlg::OnBnClickedCheckIsSimulate)
	ON_BN_CLICKED(IDC_BUTTON_loop, &CSceneDebugDlg::OnBnClickedButtonloop)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_CBSCENDEBUG_COMPOSETYPE, &CSceneDebugDlg::OnCbnSelchangeCbscendebugComposetype)
	ON_BN_CLICKED(IDC_BTSCENDEBUG_DRAWCOMPOSE, &CSceneDebugDlg::OnBnClickedBtscendebugDrawcompose)
	ON_BN_CLICKED(IDC_BTSCENDEBUG_DRAWCIRCLE, &CSceneDebugDlg::OnBnClickedBtscendebugDrawcircle)
	ON_BN_CLICKED(IDC_BTSCENDEBUG_DRAWRECT, &CSceneDebugDlg::OnBnClickedBtscendebugDrawrect)
	ON_BN_CLICKED(IDC_BTSCENDEBUG_SHOWALL, &CSceneDebugDlg::OnBnClickedBtscendebugShowall)
	ON_CBN_SELCHANGE(IDC_SCENEDLG_RELASCENENO, &CSceneDebugDlg::OnCbnSelchangeScenedlgRelasceneno)
	ON_CBN_SELCHANGE(IDC_SCENEDLG_NEXTSCENENO, &CSceneDebugDlg::OnCbnSelchangeScenedlgNextsceneno)
END_MESSAGE_MAP()


BOOL CSceneDebugDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	CString strTemp;
    register_notify(MODULE_SCENE_DEBUG_UI, OnRemoteMessage,this);
    SetBackgroundColor(BACKGROUND_COLOR_M);
    m_pInstance = this;
    // TODO: 在此添加额外的初始化代码

	int RelativeSceneID = -1,NextSceneID = -1;


	//获取场景的第一个图像采集的相机索引,应该把当前场景号保存到系统参数中
	m_CurCamIndex = -1;
	if(0 == m_ProcProcess)
	{
		ProcList* pProcList = NULL;
		pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);//构造函数中赋值当前场景为0,获取第一个场景
		if (NULL == pProcList)
		{
			return false;
		}

		for (int i = 0 ;i<pProcList->GetProcCount();i++) //第一个场景的第一个图像采集的相机索引作为当前相机索引
		{
			if(IMAGE_ACQISITION == pProcList->GetProcType(i))
			{
				m_CurCamIndex = ((SceneAcqImage*)pProcList->GetProcByIndex(i))->m_CamIndex;
				break;
			}
		}
		strTemp.Format("%d",m_iCurSceneID);
		m_ED_SceneNo.SetWindowText(strTemp);

		RelativeSceneID = pProcList->GetRelaSceneID();
		NextSceneID = pProcList->GetNextSceneID();

	}


    init_scene_info();  //初始化UI
    update_list_info();
    GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("打开实时显示");
    
    if (m_LoginLevels < 2)
    {
        GetDlgItem(IDC_BUTTON_ADD_FUNCTION)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_SUB_FUNCTION)->EnableWindow(FALSE);

    }

	//g_IsSceneDlgInitial = TRUE;

	//初始化轴号和工具号的显示
	SYSTEM_PARAM sysParam ;

	if(GetSysParam(&sysParam))
	{
		int AxeIndex =0,ToolIndex = 0;

		for (AxeIndex = 0;AxeIndex<sysParam.iAxisNum;AxeIndex++)
		{
			strTemp.Format("%d",AxeIndex);
			m_SelAxisIndex.InsertString(AxeIndex,strTemp);
			m_SelAxisIndex.SetItemData(AxeIndex,AxeIndex);
		}
		m_SelAxisIndex.InsertString(AxeIndex,"无效");
		m_SelAxisIndex.SetItemData(AxeIndex,-1);

		for (ToolIndex = 0;ToolIndex<sysParam.iToolNum;ToolIndex++)
		{
			strTemp.Format("%d",ToolIndex);
			m_SelToolIndex.InsertString(ToolIndex,strTemp);
			m_SelToolIndex.SetItemData(ToolIndex,ToolIndex);
		}
		m_SelToolIndex.InsertString(ToolIndex,"无效");
		m_SelToolIndex.SetItemData(ToolIndex,-1);

		SCENE_PKG config; 
		GetSceneParam(m_iCurSceneID,&config);

		m_SelAxisIndex.SetCurSel((config.AxisIndex<=AxeIndex&&config.AxisIndex>=0)?config.AxisIndex:AxeIndex+1);
		m_SelToolIndex.SetCurSel((config.MotorToolIndex<=ToolIndex&&config.MotorToolIndex>=0)?
			                        config.MotorToolIndex:ToolIndex+1);
		

		//初始化关联场景。
		int SceneIndex = 0;
		for (;SceneIndex<sysParam.iSceneNum ;SceneIndex++)
		{
			strTemp.Format("%d",SceneIndex);
			m_CB_RelaSceneNo.InsertString(SceneIndex,strTemp);
			m_CB_RelaSceneNo.SetItemData(SceneIndex,SceneIndex);
			m_CB_NextSceneId.InsertString(SceneIndex,strTemp);
			m_CB_NextSceneId.SetItemData(SceneIndex,SceneIndex);
		}
		strTemp.Format("%d",-1);
		m_CB_RelaSceneNo.InsertString(SceneIndex,strTemp);
		m_CB_RelaSceneNo.SetItemData(SceneIndex,-1);
		m_CB_NextSceneId.InsertString(SceneIndex,strTemp);
		m_CB_NextSceneId.SetItemData(SceneIndex,-1);

		for (int RelaIndex = 0;RelaIndex<m_CB_RelaSceneNo.GetCount();RelaIndex++)
		{
			if (m_CB_RelaSceneNo.GetItemData(RelaIndex)==RelativeSceneID)
			{
				m_CB_RelaSceneNo.SetCurSel(RelaIndex);
			}
		}

		for (int NextIndex = 0;NextIndex<m_CB_NextSceneId.GetCount();NextIndex++)
		{
			if (m_CB_NextSceneId.GetItemData(NextIndex)==NextSceneID)
			{
				m_CB_NextSceneId.SetCurSel(NextIndex);
			}
		}

	}else
	{
		return false;
	}

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSceneDebugDlg::init_scene_info()
{
	char szBuf[256] = {0};
	SCENE_PKG config;  

    m_iCurSceneID = 0; //当前场景号直接为0,前面已经为0了
    m_iCurIndex = 0; //图像采集的第一个也直接赋值为0,对应manger中的image[0]
	m_CurCamIndex = GetSceneFirstCamIndex(m_iCurSceneID); //获取第一个图像采集的第一个索引

    m_hROI=NULL;

	GetSceneParam(m_iCurSceneID,&config);

	m_ProcProcess = config.ProcProcess ;
	m_winID = (long)GetDlgItem(IDC_STATIC_BIND)->m_hWnd;
	g_SceneDlgHwinId = m_winID; //场景调试id为调试窗口的id
    CRect rt;
    GetDlgItem(IDC_STATIC_BIND)->GetClientRect(&rt);
    BindWindow(m_winID,rt.left,rt.top, rt.Width(), rt.Height(), true);//注意这里是true，标识这个是调试窗口
    m_hROI=GetDlgItem(IDC_STATIC_BIND);

	// 拍照
	bool sendflag = false;
	SceneInitGrab(m_iCurSceneID,config.ProcProcess,sendflag);

	ClearWindow(m_winID);
 	RestoreImagePart(m_winID, m_iCurSceneID,rt.Width(), rt.Height(),m_CurCamIndex);//调整图像在halcon窗口的位置
	
	DispImage(m_winID,m_iCurSceneID);


    CWnd *pWnd = m_pFunSettingWnd = GetDlgItem(IDC_STATIC_SETTING);//场景窗口旁边的面板,老方法,把阈值啥的都摆在这上面
    m_setScene.Create(IDD_DLG_SET_SCENE, this);
    //m_setPicindex.Create(IDD_DIG_SET_PICINDEX, this);
    m_SelectFunction.Create(IDD_DIALOG_FUN_SELECT, this);

//#ifndef MYFUNC
	if (0 == config.ProcProcess)
	{


	}else
	{
		m_SceneImageSetting.Create(IDD_DIALOG_SCENE_IMAGE, pWnd);
		m_SceneModelSetting.Create(IDD_DIALOG_SCENE_MODEL, pWnd);
		m_SceneThresholdSetting.Create(IDD_DIALOG_SCENE_THRESHOLD, pWnd);
		m_SceneFindLine.Create(IDD_DIALOG_SCENE_FINDLINE, pWnd);
		m_SceneFindCircle.Create(IDD_DIALOG_SCENE_FINDCIRCLE, pWnd);

		CRect rtS;
		pWnd->GetClientRect(&rtS);
		m_SceneImageSetting.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
		m_SceneModelSetting.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
		m_SceneThresholdSetting.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
		m_SceneFindLine.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
		m_SceneFindCircle.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);

		m_SceneImageSetting.ShowWindow(SW_SHOW);
		m_SceneImageSetting.UpdateUI(m_iCurSceneID, m_iCurIndex);
		m_SceneModelSetting.ShowWindow(SW_HIDE);
		m_SceneThresholdSetting.ShowWindow(SW_HIDE);
		m_SceneFindLine.ShowWindow(SW_HIDE);
		m_SceneFindCircle.ShowWindow(SW_HIDE);

		m_SceneImageSetting.SetDisplayWindowID(m_winID);
		m_SceneModelSetting.SetDisplayWindowID(m_winID);
		m_SceneThresholdSetting.SetDisplayWindowID(m_winID);
		m_SceneFindLine.SetDisplayWindowID(m_winID);
		m_SceneFindCircle.SetDisplayWindowID(m_winID);
	}
//#endif

	//从配置文件中读取图片有关roi信息
    CString str = config.szSceneName;
    /*m_static_scene.*/
	SetWindowText(str);   // ***************** 重要 ************
    GetDlgItem(IDC_EDIT_TIME)->SetWindowText("2000");
    m_checkbox.SetCheck(true);
    UpdateData(FALSE);

    for (int i = 0; i < FILE_LIST_CTRL_COUNT; i++)
    {
        DWORD dwStyle = m_listPicture[i].GetExtendedStyle();
        dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
        dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
        m_listPicture[i].SetExtendedStyle(dwStyle); //设置扩展风格

        m_listPicture[i].GetWindowRect(rt);
        int iWidth = rt.Width()/6;
        m_listPicture[i].InsertColumn(0, "No.", 0, iWidth , 10);
        m_listPicture[i].InsertColumn(1, "path", 0, iWidth*3, 10);
        m_listPicture[i].InsertColumn(2, "status", 0, iWidth, 10);
        m_listPicture[i].InsertColumn(3, "time", 0, iWidth, 10);
        if (config.imageCount > 1 && i < config.imageCount)
        {
            CButton *pBt = (CButton *)GetDlgItem(IDC_BUTTON_I1 + i);
            pBt->ShowWindow(SW_SHOW);  //ID要保证连续
        }
    }

    if (config.bSave)
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(1);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(0);
    }
    if (config.bIsSimulate)
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_SIMULATE))->SetCheck(1);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_SIMULATE))->SetCheck(0);
    }


	m_RoiCombineType.InsertString(0,"差");
	m_RoiCombineType.SetItemData(0,RoiSubType);
	m_RoiCombineType.InsertString(0,"交");
	m_RoiCombineType.SetItemData(0,RoiInsectionType);
	m_RoiCombineType.InsertString(0,"并");
	m_RoiCombineType.SetItemData(0,RoiUnionType);
	m_CombType = NullCombine;
	
	//m_RoiCombFlag.ModifyStyleEx(0,BS_CHECKBOX);
	//m_RoiCombFlag.SetButtonStyle(m_RoiCombFlag.GetButtonStyle()|BS_PUSHLIKE);
	//m_DrawRoiComb.SetButtonStyle(m_RoiCombFlag.GetButtonStyle()|BS_PUSHLIKE);
	m_IsRoiComboin = FALSE;
	m_RoiCombineType.EnableWindow(FALSE);

	//m_BAddFun.ShowWindow(true);
	//m_BDelProc.ShowWindow(true);

}






void CSceneDebugDlg::OnBnClickedBtscendebugDrawcompose()
{
	// TODO: 在此添加控件通知处理程序代码
	//创建组很前先提示是否在模板匹配里创建组合
	CString str = m_listFunction.GetItemText(m_CurProcIndex,0);
	if (!m_IsRoiComboin && 0 != str.Compare(STR_FUN_MATCH))
	{
		CString strT = "在此处理项创建组合无效";
		strT +=str;
		MessageBox(strT, "警告", MB_ICONINFORMATION);	
		return;
	}

    m_IsRoiComboin  = !m_IsRoiComboin;
	if (m_IsRoiComboin)
	{
		//创建合并
		m_RoiCombFlag.SetWindowTextA("取消合并");
		if(!OnDrawROI(m_winID,m_iCurSceneID,COMBSHAPE_PRO,NullCombine,&(g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiIdList)))
		{
		     AfxMessageBox("ROI绘制失败!");
		}else
		{
			g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->AfterCreateComb();
			m_RoiCombineType.SetCurSel(0);
			m_RoiCombineType.EnableWindow(TRUE);
			OnCbnSelchangeCbscendebugComposetype(); // 获取合并的类型
		}
		

	}else
	{		
		m_RoiCombFlag.SetWindowTextA("创建合并");
		m_RoiCombineType.EnableWindow(FALSE);
		m_CombType =NullCombine;
	}

}


void CSceneDebugDlg::OnCbnSelchangeCbscendebugComposetype()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD_PTR  selRoiCombType = m_RoiCombineType.GetItemData(m_RoiCombineType.GetCurSel());
	m_CombType = static_cast<RoiCombType>(selRoiCombType);

}



void CSceneDebugDlg::OnBnClickedBtscendebugDrawcircle()
{
	// TODO: 在此添加控件通知处理程序代码
	//&(g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiIdList);
	long HalWinId = -1;
	FindHalconWinID(g_SceneDlgHwinId,HalWinId);
	SetVisionColorHeight(HalWinId,"red");
	OnDrawROI(m_winID,m_iCurSceneID,CIRCLE_PRO,m_CombType,&(g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiIdList));
	g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiComIdList.push_back(m_CombType);

}

void CSceneDebugDlg::OnBnClickedBtscendebugDrawrect() 
{
	// TODO: 在此添加控件通知处理程序代码
	long HalWinId = -1;
	FindHalconWinID(g_SceneDlgHwinId,HalWinId);
	SetVisionColorHeight(HalWinId,"red");
	OnDrawROI(m_winID,m_iCurSceneID,LINE_PRO,m_CombType,&(g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiIdList));
	//LINE_PRO在此处表示化矩形
	g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiComIdList.push_back(m_CombType);
}


void CSceneDebugDlg::OnBnClickedBtscendebugShowall()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<int> RoiID;
	//g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->


	RoiID.push_back(g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiID);
            //(g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(m_CurProcIndex)->m_RoiIdList)
	OnShowAllRoi(m_winID,m_iCurSceneID,&RoiID);

}


void CSceneDebugDlg::OnBnClickedBtnCrabpic()
{
    //m_bIsContinousGrab = true;
	bool res = false;
	SCENE_PKG *config = NULL;
	config = (SCENE_PKG *)GetSceneParamHandle(m_iCurSceneID);
    //OnBnClickedBtnOpenShow();//打开连续采集图片
	if ( 0 == m_ProcProcess)
	{
		//使用图像采集对象 来采集图像
		ProcList* pProcList = NULL;
		VisionProcBase* pVisionProc = NULL;
		pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
		pVisionProc = pProcList->GetProcByTypeIndex(IMAGE_ACQISITION,m_iCurIndex);
		if (pVisionProc)
		{
		   res = ((SceneAcqImage*)pVisionProc)->ProcGrabImage(false);
		}

	}
	 //DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
	// 0 == m_ProcProcess的情况下，已经显示了图像

}

int WINAPI CSceneDebugDlg::OnRemoteMessage( unsigned int wParam, void * lParam, void* otherparam)
{
    switch (wParam)
    {
    case CALIB_CAMERA:
        {
            ::SendMessage(m_pInstance->m_hWnd,VISION_MANUAL_MSG, wParam, (LPARAM)lParam);
        }
        break;
    default:
        {
            ::PostMessage(m_pInstance->m_hWnd,VISION_MANUAL_MSG, wParam, (LPARAM)lParam);
        }
        break;
    }
    return 0;
}
LRESULT CSceneDebugDlg::OnManualMessage( WPARAM wParam, LPARAM lParam )
{
    switch (wParam)
    {
    case CALIB_CAMERA:
        {
            CALIB_DATA pData;
            memcpy(&pData, (CALIB_DATA *)lParam, sizeof(CALIB_DATA));
            if (VisionCalibProcess(&pData))
            {
                DispObj(m_winID,m_iCurSceneID,true);
            }
        }
        break;
    default:
        {
			//Sleep(1000);
            DispImage(m_pInstance->m_winID, m_pInstance->m_iCurSceneID, m_pInstance->m_iCurIndex);
			//long HalWinId = 0;
			//ProcList* pScene = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
			//FindHalconWinID(g_SceneDlgHwinId,HalWinId);
			////int ImageIndex = g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcImageIndex(GetProcId());
			//int ImageIndex = m_pInstance->m_iCurIndex;
			//SceneMatchModel* mod = (SceneMatchModel*) pScene->GetProcByTypeIndex(MODEL_PRO,0);//获取第一个模板处理子类
			//if (NULL != mod)
			//{	
			//	char* m_ModelName = mod->m_ModelName;
			//	int PyramidLevel = mod->m_SceneModelSetting.m_BuildLevel;
			//	int MatchNum = 1;
			//	double  Score = mod->m_SceneModelSetting.m_MatchMinScore;
			//	double row =0.0,col = 0.0,phi = 0.0;
			//	bool IsBulid = false;//无用
			//	int ProcIndex  = mod->GetProcTypeIndex();
			//	SCENE_PKG *pConfig = (SCENE_PKG *)GetSceneParamHandle(m_iCurSceneID);
			//	int falg = pConfig->bSave;//先用保存模拟下
			//	//显示后再加上模板检测及十字叉显示中心
			//	FindModelRealTime(m_iCurSceneID,HalWinId,ImageIndex,m_ModelName,PyramidLevel,MatchNum,Score,falg,IsBulid,row,col,phi);
			//
			//}

        }
        break;
    }
    return 0;
}

void CSceneDebugDlg::OnBnClickedBtnOpenFolder()
{
	CString strFilePath;
	const char pszFilter[] = _T("(*.*)|*.*|*.bmp|*.bmp|(*.jpg,*.jpeg,*.jpe)|*.jpg|(*.png)|*.png|(*.tif)|*.tif||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,pszFilter, this);
	
	if(dlg.DoModal() == IDOK)
	{
		strFilePath = dlg.GetPathName();
		if (0 == strFilePath.GetLength())
		{
			return;
		}
		//BindWindow(m_winID,10,10, 500, 350);
		ReadImage(strFilePath.GetBuffer(),m_iCurSceneID, m_iCurIndex);
		DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
	}
	else
	{
		return ;
	}
}


void CSceneDebugDlg::OnBnClickedBtnOpenShow()
{
    if (m_bIsContinousGrab)
    {
        m_bIsContinousGrab = false;
	    ColseContinousGrab(m_iCurSceneID, m_iCurIndex,m_CurCamIndex);
        GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("打开实时显示");
    }
    else
    {
        m_bIsContinousGrab = true;
	    OpenContinousGrab((long)this->GetSafeHwnd(), m_iCurSceneID, m_iCurIndex, VISION_MANUAL_MSG,m_CurCamIndex);		
        GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("关闭实时显示");
    }
}

void CSceneDebugDlg::OnBnClickedBtnSaveoripic()//保存原始图片时更新次数为0
{
	CHECK_RESULT *res = GetSceneProcResultById(m_iCurSceneID);
	if (NULL == res)
	{
		AfxMessageBox("not find % scene id:%d",m_iCurSceneID);
	}
	SaveImage(m_iCurSceneID, m_iCurIndex,"test.jpg");	
}


void CSceneDebugDlg::OnBnClickedBtnReultpic()//保存结果图片
{
	//char *path = NULL;
	char path[MAX_PATH] = {0}; //不能传空指针
	SaveImageDump(m_winID,m_iCurSceneID,path);	
}
void CSceneDebugDlg::updata_ui_data(int iType, int index)
{
    char szBuf[256] = {0};
    switch (iType)
    {
    case MODEL_PRO:
        break;
    case LINE_PRO:
        {
        }
        break;
    case CIRCLE_PRO:
        {
        }
        break;
    }
}

void CSceneDebugDlg::updat_way2_list()
{
	ProcList* pProcList = NULL;

	pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);

	if (!pProcList)
	{
		return;
	}

	m_listFunction.DeleteAllItems();

	for (int j = 0; j< pProcList->GetProcCount();j++ )
	{
		switch (pProcList->GetProcType(j))
		{
		case IMAGE_READ:
			 m_listFunction.InsertItem(j,STR_FUN_IMAGE_READ);
			break;
		case IMAGE_ACQISITION:
			m_listFunction.InsertItem(j,STR_FUN_IMAGE_ACQ);
			break;
		case IMAGE_PREPROCESS:
			m_listFunction.InsertItem(j,STR_FUN_IMAGE_ENHANCEMENT);
			break;
			
		case MODEL_PRO:
			m_listFunction.InsertItem(j,STR_FUN_MATCH);
			break;
		case CONTOUR_POSITION:
			m_listFunction.InsertItem(j,STR_CONTOUR_POSITION);
			break;
		case HALPROC_POSITIONG:
			m_listFunction.InsertItem(j,STR_HALPROC_POSITION);
			break;	
		case HALPROC_PROCESS:
			m_listFunction.InsertItem(j,STR_HALPROC_PROCESS);
			break;	  
		default:
			m_listFunction.InsertItem(j,STR_NO_PROC);
			break;
		}
	}
	  m_listFunction.SetSelectionMark(0);//一初始化时选择为第一项
}

void CSceneDebugDlg::update_list_info()
{
    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);
	if ( 0 == config.ProcProcess)
	{
		updat_way2_list();
		return;
	} 

    int id = 0;
    int iType = 0;
    m_vecListInfo.clear();
    if (config.vecBody.empty())
    {
        ClearWindow((long)this->m_hWnd);
        //return;
    }
    int iPos = 0;
    for(int i = 0; i < config.pkgCount; i++)
    {
        iType = config.vecBody[i][0].iValue;
        m_vecListInfo.push_back(iType);
    }
    updata_list();

}

void CSceneDebugDlg::updata_list()
{
    int id = 0;
    int iType = 0;
    m_listFunction.DeleteAllItems();
    vector<unsigned int>::iterator it;
    for (it = m_vecListInfo.begin(); it != m_vecListInfo.end(); it++)
    {

        int iType = ((*it)%10000)/100;
        switch(iType)
        {
        case MAIN_PRO:
            m_listFunction.InsertItem(id,"图像输入");
            break;
        case MODEL_PRO:
            m_listFunction.InsertItem(id,"模板匹配");
            break;
        case THRE_PRO:
            m_listFunction.InsertItem(id,"阈值分割");
            break;
        case LINE_PRO:
            m_listFunction.InsertItem(id,"抓边");
            break;
        case CIRCLE_PRO:
            m_listFunction.InsertItem(id,"抓圆");
            break;
               

		case MORE_PRO:
			m_listFunction.InsertItem(id,"更多");
			break;
        default:
            break;
        }
        id++;
    }
    m_listFunction.SetSelectionMark(0);
    //m_listFunction.SetBkColor(RGB(125,123,125));
}

void CSceneDebugDlg::OnBnClickedButton1()
{
	CPoint pt;
	CRect rt;
	GetCursorPos(&pt);
	m_setScene.GetClientRect(rt);
	m_setScene.SetWindowPos(NULL,pt.x,pt.y,rt.Width(),rt.Height(),SWP_NOSIZE);
	m_setScene.ShowWindow(SW_SHOW);
	HWND hTest = m_hWnd;
	//int a = 0;

}
void CSceneDebugDlg::SceneInitGrab(int SceneID,int ProcType,bool sendflag)
{
	if ( 0 == ProcType)
	{
		InitGrabWay2Sceneimage(m_iCurSceneID,sendflag);
	} 
	else
	{
		InitGrabSceneimage(m_iCurSceneID,m_iCurIndex);
	}

}

void CSceneDebugDlg::SetShowScene(int SceneID)
{
	SYSTEM_PARAM* sysParam = NULL;
	sysParam = (SYSTEM_PARAM* )GetSysParamPoint();

	SCENE_PKG *config= NULL;
	config = (SCENE_PKG *)GetSceneParamHandle(m_iCurSceneID);

	if (m_iCurSceneID+1 > sysParam->iSceneNum )
	{//暂时未对新建场景添加处理， 
		return ;
	}
	m_CurCamIndex = GetSceneFirstCamIndex(m_iCurSceneID);
	if (m_CurCamIndex<0)
	{
		m_CurCamIndex = 0;
	}
	m_iCurIndex = 0;//默认一个场景的一个图像处理第一项
	//显示轴号
	if (config->AxisIndex>=0&&config->AxisIndex<sysParam->iAxisNum)
	{
		m_SelAxisIndex.SetCurSel(config->AxisIndex);
	}else
	{
		m_SelAxisIndex.SetCurSel(m_SelAxisIndex.GetCount()-1);
	}

	//显示工具号 
	if (config->MotorToolIndex>=0&&config->MotorToolIndex<sysParam->iToolNum)
	{
		m_SelToolIndex.SetCurSel(config->MotorToolIndex);
	}else
	{
		m_SelToolIndex.SetCurSel(m_SelToolIndex.GetCount()-1);
	}
}



LRESULT CSceneDebugDlg::OnSetScene( WPARAM wParam, LPARAM lParam )//场景改变消息
{
	CString strTemp;
	m_iCurSceneID = (int)lParam;
	if (m_iCurSceneID<0)
	{
		m_iCurSceneID = 0;
	}
	SYSTEM_PARAM sysParam;
	GetSysParam(&sysParam);

	SCENE_PKG config;
	GetSceneParam(m_iCurSceneID,&config);
	m_ProcProcess = config.ProcProcess;

	//获取当前场景ID和第一个相机索引,图像索引设定为0
	int PreSceneId = m_iCurSceneID;
	int PreCamIndex = m_CurCamIndex;
	SetShowScene(m_iCurSceneID);

    if (m_bIsContinousGrab)
    {
        m_bIsContinousGrab = false;
        ColseContinousGrab(PreSceneId, PreCamIndex);
        GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("打开实时显示");
    }
    if (1 == ((CButton *)GetDlgItem(IDC_CHECK_IS_SHOW_TMP))->GetCheck())//是否显示调试信息
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_SHOW_TMP))->SetCheck(0);
        OnBnClickedCheckIsShowTmp();
    }
	m_sceneChange = true;	
	SetWindowText(sysParam.pstrSceneName[m_iCurSceneID].c_str());  // 更改场景的窗口名称。  
    ShowFileListCtrl(0);
    for (int i = 0; i < FILE_LIST_CTRL_COUNT; i++)
    {
        if (i >= config.imageCount || config.imageCount <= 1)
        {
            CButton *pBt = (CButton *)GetDlgItem(IDC_BUTTON_I1 + i);
            pBt->ShowWindow(SW_HIDE);  //ID要保证连续
        }
        else if (config.imageCount > 1)
        {
            CButton *pBt = (CButton *)GetDlgItem(IDC_BUTTON_I1 + i);
            pBt->ShowWindow(SW_SHOW);  //ID要保证连续
        }
    }
    CRect rt;
    ((CWnd *)GetDlgItem(IDC_STATIC_BIND))->GetWindowRect(&rt);
	
	// 切换场景拍照
	bool sendflag = false;
	SceneInitGrab(m_iCurSceneID,config.ProcProcess,sendflag);
    ClearWindow(m_winID);  //防止图像大小不一至导致窗口显示信息混乱
    //RestoreImagePart((long)this->m_hWnd,m_iCurIndex,rt.Width(), rt.Height());

	RestoreImagePart(m_winID,m_iCurIndex,rt.Width(), rt.Height());//自适应窗口
    if (config.bSave)
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(1);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(0);
    }
    if (config.bIsSimulate)
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_SIMULATE))->SetCheck(1);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_SIMULATE))->SetCheck(0);
    }
	DispImage(m_winID,m_iCurSceneID);
    update_list_info();
    LRESULT Result;
    OnClickListFunction(NULL, &Result);
    //OnClickListFunction();
    //OnBnClickedButtonFindLine();
	strTemp.Format("%d",m_iCurSceneID);
	m_ED_SceneNo.SetWindowTextA(strTemp);

	//设置



	return 1;
}



int CSceneDebugDlg::AddWay2Function(int selPos,int FunType)
{
	//char* SceneConfigPath[256] = {0};

	//memcpy(SceneConfigPath,GetSceneConfigPath(m_iCurSceneID),sizeof(SceneConfigPath));

	CString SceneConfigPath;
	SceneConfigPath.Format("%s",GetSceneConfigPath(m_iCurSceneID));
	int ProcIdBase = 0;
	ProcList* pProcList = NULL;
	pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
	VisionProcBase* pProcBase = NULL;

	if(-1 != pProcList->GetProcCount())
	{
		ProcIdBase = pProcList->GetProcByIndex(pProcList->GetProcCount()-1)->GetProcIdBase()+1;//获取这个场景中最后一项的索引,+1就是需要创建的处理项索引
	}else
	{
		ProcIdBase = 0;
	}
	 
	BuildProc(&pProcBase,SceneConfigPath,FunType,m_iCurSceneID,ProcIdBase);

	//switch (FunType)
	//{
	//case MAIN_PRO:
	//	{

	//	}
	//	break;
	//case MODEL_PRO:
	//	{
	//		SceneMatchModel*  pMatchModel = new SceneMatchModel;//m_pFunSettingWnd
	//		pProcBase = pMatchModel;
	//		pMatchModel->InitInstance(m_pFunSettingWnd);
	//		pMatchModel->m_SceneModelSetting.SetDisplayWindowID(m_winID);
	//		
	//		//m_pFunArr.InsertAt(selPos+1,pMatchModel);
	//		//m_listFunction.InsertItem(selPos+1,"模板匹配");
	//	}
	//	break;
	//case THRE_PRO:
	//	{

	//	}
	//	break;
	//case IMAGE_ACQISITION:   //图像采集
	//	{
	//		SceneAcqImage* pAcqImage = new SceneAcqImage;
	//		pProcBase = pAcqImage;
	//		pAcqImage->InitInstance(m_pFunSettingWnd);
	//		pAcqImage->m_SceneAcqImageConfig.SetDisplayWindowID(m_winID);

	//	}
	//	break;
	//case CONTOUR_POSITION:
	//	{
	//		ScenePositon* pPosition = new ScenePositon;
	//		pProcBase = pPosition;
	//		pPosition->InitInstance(m_pFunSettingWnd);
	//		pPosition->m_ScenePositionSetting.SetDisplayWindowID(m_winID);
	//	}
	//	break;
	//case MORE_PRO:
	//	{

	//	}
	//	break;
	//default:
	//	break;
	//}


	if (pProcBase)
	{
		pProcList->InsertProc(ProcIdBase<=(selPos+1)?(selPos+1):ProcIdBase,pProcBase);//这里要重新排序
	}

	return 0;
}

int CSceneDebugDlg::AddProcessFunction(int FunType)
{
	
	return  0;
}


LRESULT CSceneDebugDlg::OnAddFunction( WPARAM wParam, LPARAM lParam )//添加函数功能
{
  
	SCENE_PKG config;
	GetSceneParam(m_iCurSceneID, &config);
	int iType = (int)lParam;
    int iSel = m_listFunction.GetSelectionMark();//这时场景调试框中的函数list

	if (0 == config.ProcProcess)
	{
		if (IDNO == AfxMessageBox("确认添加", MB_YESNO))
		{
			return -1;
		}
		AddWay2Function(iSel,iType);
		updat_way2_list();
		m_listFunction.SetSelectionMark(iSel);
		return 0;
	}

	//下面都是旧版的，不会走下去
    //int iCount = m_listFunction.GetItemCount();
    int iValue = 0;
    int          iCurSize = 0;
    int    insertPos = 0;
    bool   btool = false;
    if (IDNO == AfxMessageBox("确认添加", MB_YESNO) || (iSel > m_vecListInfo.size() && MAIN_PRO != iType))
    {
        return -1;
    }
		switch (iType)
		{
		case MAIN_PRO:
			{
				int iCheck = 0;
				int iCurImage = 0;
				int iTD = 0;
				int iGroup = 0;
				//非最后一个
				if (iSel < m_vecListInfo.size() - 1) 
				{
					int itool = 0;
					iTD = m_vecListInfo[iSel]%10000;
					iCurImage = m_vecListInfo[iSel]/10000;
					int liTD = m_vecListInfo[iSel+1]%10000/100;
					//判断是否在图像前面添加
					if (MAIN_PRO != iTD/100 && 0 != liTD)
					{
						AfxMessageBox("请在图像输入前新增图像输入");
						return -1;
					}
					//前面保证了iSel + 1 有效
					if (0 == liTD)
					{
						itool = liTD%100;
						btool = true;
					}
					else
					{
						itool = iTD%100;
					}
					int iPos = IFindParamData(&config, MAIN_PRO, itool);
					insertPos = iPos;
				}
				//最后一个
				else if (iSel == m_vecListInfo.size() - 1)
				{
					if (config.imageCount >= m_vecListInfo.size())
					{
						config.imageCount = (unsigned int)m_vecListInfo.size();
					}
					iCurImage = config.imageCount;
					insertPos = config.pkgCount;
				}
				else
				{
					return -1;
				}

				iValue = iCurImage * 10000 + iType * 100 + iCurImage;
				VIRSION_DATA_PKG pInfo[2];
				pInfo[0].iValue = iValue;
				pInfo[1].iValue = 0;
				unsigned int pType[2] = {0};
				//向场景信息中添加图像参数
				InsertParamItem(m_iCurSceneID, pInfo, pType, 2, insertPos);

				//插入容器
				vector<unsigned int>::iterator itL = m_vecListInfo.begin() + (iSel + 1);
				m_vecListInfo.insert(itL, 1, iValue);
				for (int i = 0; i < FILE_LIST_CTRL_COUNT; i++)
				{
					if (config.imageCount > 1 && i < config.imageCount)
					{
						CButton *pBt = (CButton *)GetDlgItem(IDC_BUTTON_I1 + i);
						pBt->ShowWindow(SW_SHOW);  //ID要保证连续
					}
				}

				//向配置文档中增加相应的DemoN.jpg
				//char szCurrentPath[512] = {0};
				char demoCurrentPath[512] = {0};
				int iPicture = 0;

				for (int i = 0; i<m_listFunction.GetItemCount(); i++)
				{
					if(0 == strcmp(m_listFunction.GetItemText(i,0), "图像输入"))
					{
						iPicture++;
					}
				}

				CString str;
				CString namestr = "demo";
				str.Format("%d",iPicture);
				str = namestr + str + ".jpg";
				char szCurrentPath[512] = {0};
				get_module_path((LPSTR)szCurrentPath);
				//strcat_s(szCurrentPath, "\\");
				strcat_s(szCurrentPath,config.szSceneName);
				strcat_s(szCurrentPath,"\\");
				strcpy_s(demoCurrentPath,512,szCurrentPath);
				strcat_s(demoCurrentPath,"\\demo0.jpg");
				strcat_s(szCurrentPath,str);

				if(-1 == _access(szCurrentPath, 0))//图片文件不存在
				{
					CopyFile(demoCurrentPath,szCurrentPath,false);
				}

				//if (iCurImage != config.imageCount)
				//{
				//    m_vecListInfo[iSel + 1]++;
				//}
			}
			break;
		case MODEL_PRO:
			{
					
	//#ifndef MYFUNC
					int iCurMode = 0;
					int iCurImage = -1;
					int iTD = 0;
					int iGroup = 0;
					for (int index = iSel; index >= 0; index--)
					{
						iTD = m_vecListInfo[index]%10000;
						if (MODEL_PRO == iTD/100)
						{
							iCurMode++;
						}
						if (MAIN_PRO == iTD/100)
						{
							iCurImage++;
						}
						insertPos++;
					}
					iValue = iCurImage * 10000 + iType * 100 + iCurMode;
					unsigned int iFlag = ROI_TYPE * 100 + 1;
					int iModelStrSize = 17;
					int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iModelStrSize;
					VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
					unsigned int *pType = new unsigned int[iSize];
					ST_MODEL_INFO stInfo;
					memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
					memset(pType, 0, iSize * sizeof(unsigned int));
					pInfo[0].iValue = iValue;
					pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;
					memcpy((pInfo + VIRSION_DATA_PKG_HEADER_SIZE), &stInfo, sizeof(ST_MODEL_INFO));
					memcpy(&stInfo, (pInfo + VIRSION_DATA_PKG_HEADER_SIZE), sizeof(ST_MODEL_INFO)); //sjg:拷来拷去是什么意思？
					int iV = sizeof(ST_RECTANGLE_DATA)/sizeof(double) + 3;  //后三个记录模板查找结果
					int SizeTest = 20;

					//int* pInt = new int[SizeTest];

					for (int i = 0; i < iV; i++)
					{
						pType[VIRSION_DATA_PKG_HEADER_SIZE + i] = iFlag;
					}
					pType[VIRSION_DATA_PKG_HEADER_SIZE + iV] = ID_TYPE * 100 + VISION_INT_TYPE;
					//7个普通参数
					for (int j = 1; j <= 7; j++)
					{
						iV++;
						pType[VIRSION_DATA_PKG_HEADER_SIZE + iV] = NORMARL_TYPE * 100 + VISION_DOUBLE_TYPE;   //匹配分数、...
					}

					iV++;
					pType[VIRSION_DATA_PKG_HEADER_SIZE + iV] = SPEC_TYPE * 100 + VISION_INT_TYPE;   //模板类型.

					//向场景信息中添加图像参数
					InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

					vector<unsigned int>::iterator itML = m_vecListInfo.begin() + iSel + 1;
					m_vecListInfo.insert(itML, 1, iValue);
			
	//#endif
			}
			break;
		case THRE_PRO:
			{
				int iCurIndex = 0;
				int iTD = 0;
				int iGroup = -1;
				for (int index = iSel; index >= 0; index--)
				{
					iTD = m_vecListInfo[index]%10000;
					if (iType == iTD/100)
					{
						iCurIndex++;
					}
					if (MAIN_PRO == iTD/100)
					{
						iGroup++;
					}
					insertPos++;
				}
				if (iCurIndex >= SCENE_SIZE)
				{
					return -1;
				}
				iValue = iGroup * 10000 + iType * 100 + iCurIndex;
				unsigned int iFlag = THRE_TYPE * 100 + 0;
				int iSize = VIRSION_DATA_PKG_HEADER_SIZE + 1;
				VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
				unsigned int *pType = new unsigned int[iSize];
				memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
				memset(pType, 0, iSize * sizeof(unsigned int));
				pInfo[0].iValue = iValue;
				pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;
				pInfo[2].iValue = 0;
				pType[2] = iFlag;
				//向场景信息中添加图像参数
				InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

				vector<unsigned int>::iterator itCL = m_vecListInfo.begin() + iSel + 1;
				m_vecListInfo.insert(itCL, 1, iValue);
			}
			break;
		case LINE_PRO:
			{
				int iCurIndex = 0;
				int iTD = 0;
				int iGroup = -1;
				for (int index = iSel; index >= 0; index--)
				{
					iTD = m_vecListInfo[index]%10000;
					if (iType == iTD/100)
					{
						iCurIndex++;
					}
					if (MAIN_PRO == iTD/100)
					{
						iGroup++;
						//iCurIndex = 0;
					}
					insertPos++;
				}
				iValue = iGroup * 10000 + iType * 100 + iCurIndex;
				unsigned int iFlag = ROI_TYPE * 100 + 1;
				int iLSize = 13;
				int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iLSize;
				ST_FIND_LINE stInfo;
				VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
				unsigned int *pType = new unsigned int[iSize];
				memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
				memset(pType, 0, iSize * sizeof(unsigned int));
				pInfo[0].iValue = iValue;
				pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;

				//初始化参数
				memcpy((pInfo + VIRSION_DATA_PKG_HEADER_SIZE), &stInfo, sizeof(ST_RECTANGLE_DATA));
				int iV = sizeof(ST_RECTANGLE_DATA)/sizeof(double);
				for (int i = 0; i < iV; i++)
				{
					pType[VIRSION_DATA_PKG_HEADER_SIZE + i] = iFlag;
				}
				int indexT = VIRSION_DATA_PKG_HEADER_SIZE + iV;
				pInfo[indexT].dValue = stInfo.smooth;
				pType[indexT] = 1;   //平滑度
				indexT++;
				pInfo[indexT].iValue = stInfo.threshold;
				indexT++;
				pInfo[indexT].iValue = stInfo.ROIWidth;
				indexT++;
				pInfo[indexT].iValue = stInfo.iPosNum;
				indexT++;
				pInfo[indexT].iValue = stInfo.iFindType;
				indexT++;
				pInfo[indexT].iValue = stInfo.iTransition;
				indexT++;
				pInfo[indexT].iValue = stInfo.iAlgorithm;
				indexT++;
				pInfo[indexT].iValue = stInfo.iDirection;
				//向场景信息中添加图像参数
				InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

				vector<unsigned int>::iterator itCL = m_vecListInfo.begin() + iSel + 1;
				m_vecListInfo.insert(itCL, 1, iValue);
			}
			break;
		case CIRCLE_PRO:
			{
				int iCurIndex = 0;
				int iTD = 0;
				int iGroup = -1;
				for (int index = iSel; index >= 0; index--)
				{
					iTD = m_vecListInfo[index]%10000;
					if (iType == iTD/100)
					{
						iCurIndex++;
					}
					if (MAIN_PRO == iTD/100)
					{
						iGroup++;
					}
					insertPos++;
				}

				iValue = iGroup * 10000 + iType * 100 + iCurIndex;
				unsigned int iFlag = ROI_TYPE * 100 + 1;
				int iCSize = 14;
				int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iCSize;
				ST_FIND_CIRCLE stInfo;
				VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
				unsigned int *pType = new unsigned int[iSize];
				memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
				memset(pType, 0, iSize * sizeof(unsigned int));
				pInfo[0].iValue = iValue;
				pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;

				//初始化参数
				memcpy((pInfo + VIRSION_DATA_PKG_HEADER_SIZE), &stInfo, sizeof(ST_CIRCLE_DATA));
				int iV = sizeof(ST_CIRCLE_DATA)/sizeof(double);
				for (int i = 0; i < iV; i++)
				{
					pType[VIRSION_DATA_PKG_HEADER_SIZE + i] = iFlag;
				}
				int indexT = VIRSION_DATA_PKG_HEADER_SIZE + iV;
				pInfo[indexT].dValue = stInfo.smooth;
				pType[indexT] = 1;   //平滑度
				indexT++;
				pInfo[indexT].iValue = stInfo.threshold;
				indexT++;
				pInfo[indexT].iValue = stInfo.ROIWidth;
				indexT++;
				pInfo[indexT].iValue = stInfo.iPosNum;
				indexT++;
				pInfo[indexT].iValue = stInfo.iFindType;
				indexT++;
				pInfo[indexT].iValue = stInfo.iTransition;
				indexT++;
				pInfo[indexT].iValue = stInfo.iAlgorithm;
				indexT++;
				pInfo[indexT].iValue = stInfo.iDirection;

				//向场景信息中添加图像参数
				InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

				vector<unsigned int>::iterator itCL = m_vecListInfo.begin() + iSel + 1;
				m_vecListInfo.insert(itCL, 1, iValue);

			}
			break;
		case MORE_PRO:
			{
				int iCurIndex = 0;
				int iTD = 0;
				int iGroup = -1;
				for (int index = iSel; index >= 0; index--)
				{
					iTD = m_vecListInfo[index]%10000;
					if (iType == iTD/100)
					{
						iCurIndex++;
					}
					if (MAIN_PRO == iTD/100)
					{
						iGroup++;
					}
					insertPos++;
				}

				iValue = iGroup * 10000 + iType * 100 + iCurIndex;
				unsigned int iFlag = ROI_TYPE * 100 + 1;
				int iLSize = 0;
				int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iLSize;
				ST_FIND_LINE stInfo;
				VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
				unsigned int *pType = new unsigned int[iSize];
				memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
				memset(pType, 0, iSize * sizeof(unsigned int));
				pInfo[0].iValue = iValue;
				pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;

				//初始化参数
				//memcpy((pInfo + VIRSION_DATA_PKG_HEADER_SIZE), &stInfo, sizeof(ST_RECTANGLE_DATA));

				//向场景信息中添加图像参数
				InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

				vector<unsigned int>::iterator itCL = m_vecListInfo.begin() + iSel + 1;
				m_vecListInfo.insert(itCL, 1, iValue);
			}
			break;
		default:
			break;
        }
	
		//交换后面数据
		iSel++;
		if ((0 == iType && iSel != m_listFunction.GetItemCount()) || btool)
		{
			iSel--;
		}
		for (int index = iSel+1; index < m_vecListInfo.size(); index++)
		{
			if ( iType == m_vecListInfo[index]%10000/100 )
			{
				if (0 == iType)
				{
					m_vecListInfo[index] = m_vecListInfo[index] + 10001;
					int lindex = index+1;
					if (lindex < m_vecListInfo.size())
					{
						while (m_vecListInfo[lindex]%10000/100)
						{
							m_vecListInfo[lindex] = m_vecListInfo[lindex] + 10000;
							lindex++;
							if (lindex >= m_vecListInfo.size())
							{
								break;
							}

						}
					}
				}
				else
				{
					m_vecListInfo[index]++;
				}
			}
		}
    //SetSceneParam(&config);
     updata_list();
	
    m_listFunction.SetSelectionMark(iSel);
    return 0;
}


BOOL CSceneDebugDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message && (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
    {
        if (VK_RETURN == pMsg->wParam)
        {
        }
        return   TRUE;
    } 
    static bool bIsLButtonDown = false;
    static POINT ptB;
    POINT ptE;
    switch(pMsg->message)
    {
    case WM_MOUSEMOVE:
        {
            HWND hwnd = GetDlgItem(IDC_STATIC_BIND)->m_hWnd;
            if (GetDlgItem(IDC_STATIC_BIND)->m_hWnd == pMsg->hwnd)
            {
                ptE = pMsg->pt;
                //鼠标按下则拖动图像
                if (bIsLButtonDown)
                {
                    CRect rtWin;
                    GetDlgItem(IDC_STATIC_BIND)->GetClientRect(&rtWin);
                    double dXPer = 0.0;
                    double dYPer = 0.0;
                    dXPer = (double)(ptE.x - ptB.x)/rtWin.Width();
                    dYPer = (double)(ptE.y - ptB.y)/rtWin.Height();
					ClearWindow(m_winID);
                    UpdateImagePart(m_winID, m_iCurSceneID, MOVE_TPYE, dXPer, dYPer);	
                    DispObj(m_winID,m_iCurSceneID,true);				
                    //DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
                }
                ptB = ptE;
            }
        }
        break;
    case WM_LBUTTONDOWN:
        {
            if (GetDlgItem(IDC_STATIC_BIND)->m_hWnd == pMsg->hwnd)
            {
                bIsLButtonDown = true;
                ptB = pMsg->pt;
            }
        }
        break;
    case WM_LBUTTONUP:
        {
            bIsLButtonDown = false;
        }
        break;
    case WM_MOUSEHWHEEL:
    case 0x020A:
        {
            CRect rtWin;
            GetDlgItem(IDC_STATIC_BIND)->GetWindowRect(&rtWin);
            if (rtWin.PtInRect(pMsg->pt))
            {
                int iType = 0;
                ptE = pMsg->pt;
                CRect rtM;
                GetClientRect(&rtM);
                static double per = 1.0;
                double dXPer = 0.0;
                double dYPer = 0.0;
                dXPer = (double)(ptE.x - rtWin.left)/rtWin.Width();
                dYPer = (double)(ptE.y - rtWin.top)/rtWin.Height();
                if (0x780000 == pMsg->wParam)
                {
                    iType = ENLARGE_TYPE;
                }
                else
                {
                    iType = SHRINK_TYPE;
                }
				ClearWindow(m_winID);
                UpdateImagePart(m_winID, m_iCurSceneID, iType, dXPer, dYPer);
                DispObj(m_winID,m_iCurSceneID,true);
                //DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
            }
            
        }
        break;
    case WM_TIMER:
    case WM_NCMOUSEMOVE:
    case WM_PAINT:
        break;
    default:
        int i = (int)pMsg->lParam;
        break;
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSceneDebugDlg::OnBnClickedButton12()//整体验证
{
    SetDebugWindowID(m_iCurSceneID, m_winID);

	TriggerVisionProcess(m_iCurSceneID);

	/*VisionProcess(m_iCurSceneID,m_iCurIndex,m_strsult);*/
	if (m_strsult.bIsDisp)
	{
        DispObj(m_winID,m_iCurSceneID,true);
    }
}

INT CALLBACK _BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM pData)
{ 
    TCHAR szDir[MAX_PATH];
    switch(uMsg)
    {
    case BFFM_INITIALIZED: 
        // WParam is TRUE since you are passing a path. 
        // It would be FALSE if you were passing a pidl. 
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pData);
        break;
    case BFFM_SELCHANGED: 
        // Set the status window to the currently selected path. 
        if (SHGetPathFromIDList((LPITEMIDLIST)lParam ,szDir)) 
        {
            SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
        }
        break;
    }
    return 0; 
}

void CSceneDebugDlg::OnBnClickedBtnOpenPicfloder()
{
    CString strCurPath;
    char szCurrentPath[512] = {0};
    get_module_path((LPSTR)szCurrentPath);
	char pszPath[256] = {0};
	BROWSEINFO info;
	ZeroMemory(&info,sizeof(BROWSEINFO));
	info.hwndOwner=GetSafeHwnd();
	info.iImage=NULL;
	info.lpszTitle="请选择你的文件夹";
    info.pidlRoot=NULL;
    info.lParam = (LPARAM)szCurrentPath; 
	info.lpfn=_BrowseCallbackProc;
	info.ulFlags=BIF_EDITBOX|BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&info);  
	if (pidl == NULL)  
	{  
		return;  
	}  

	if (SHGetPathFromIDList(pidl, pszPath))  
	{  
		if (BST_CHECKED == m_checkbox.GetCheck())
		{
			m_vecorpic.clear();
			find(pszPath,true);
		}
		else
		{
			m_vecorpic.clear();
			find(pszPath,false);
		}
		
	}  

}

void CSceneDebugDlg::find(char * lpPath,bool bflag)//查找文件路径
{
    char szFind[MAX_PATH];
    WIN32_FIND_DATA FindFileData;
	int i=0;
	char buf[MAX_PATH]={0};

    strcpy_s(szFind,MAX_PATH,lpPath);
    strcat_s(szFind,"//*.*");

    HANDLE hFind=::FindFirstFile(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)    
	{
		return;
	}
    while(TRUE)
    {
		
        if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
			if (!bflag)
			{
				if(FindFileData.cFileName[0]!='.')
				{
					m_strTemp.Format("%s\\%s",lpPath,FindFileData.cFileName);
					m_vecorpic.push_back(m_strTemp);
				}
			}
			
        }
        else
        {
			m_strTemp.Format("%s\\%s",lpPath,FindFileData.cFileName);
			if (bflag)
			{

				if (m_strTemp.Right(4) == ".BMP" || m_strTemp.Right(4) == ".bmp" || m_strTemp.Right(4) == ".jpg" || m_strTemp.Right(4) == ".png" || m_strTemp.Right(4) == ".tif")
				{
					m_vecorpic.push_back(m_strTemp);
				}
			}
			else
			{
				m_vecorpic.push_back(m_strTemp);
			}
			
		   
        }
        if(!FindNextFile(hFind,&FindFileData))   
			break;
    }
    FindClose(hFind);

	m_listPicture[m_iCurGroup].DeleteAllItems();
	for (i=0 ; i < m_vecorpic.size();i++)
	{
		int iRow=m_listPicture[m_iCurGroup].InsertItem(i,"");
		sprintf_s(buf,"%d",i+1);
		m_listPicture[m_iCurGroup].SetItemText(iRow,0,buf);
		m_listPicture[m_iCurGroup].SetItemText(iRow,1,m_vecorpic[i].GetBuffer());
	}
}

void CSceneDebugDlg::OnBnClickedBtnClear()
{
    for (int i = 0; i < FILE_LIST_CTRL_COUNT; i++)
    {
        m_listPicture[i].DeleteAllItems();
    }
}


void CSceneDebugDlg::OnBnClickedBtnDelete()
{
	int i = m_listPicture[m_iCurGroup].GetSelectionMark();
	if (-1 != i)
	{
		m_listPicture[m_iCurGroup].DeleteItem(i);
		int size = m_listPicture[m_iCurGroup].GetItemCount();
		for (int j = 0;j < size;j++)
		{
			m_strTemp.Format("%d",j+1);
			m_listPicture[m_iCurGroup].SetItemText(j,0,m_strTemp.GetBuffer());
		}
		
	}
}


void CSceneDebugDlg::OnBnClickedBtnAllSelect()
{
	
}


void CSceneDebugDlg::OnBnClickedCheckPic()
{
	if (BST_UNCHECKED == m_checkbox.GetCheck())
	{
		//m_checkbox.SetCheck(true);
	}
	if (BST_CHECKED == m_checkbox.GetCheck())
	{
		//m_checkbox.SetCheck(false);
	}

}

void CSceneDebugDlg::OnBnClickedBtnStart()
{
	GetDlgItem(IDC_EDIT_TIME)->GetWindowText(m_strTemp);
	int time = atoi(m_strTemp.GetBuffer());
	GetDlgItem(IDC_BTN_START)->GetWindowText(m_strTemp);
	if (0 == strcmp("开始",m_strTemp.GetBuffer()))
	{
		GetDlgItem(IDC_BTN_START)->SetWindowText("停止");
        m_iTestImageIndex = 0;
        int iSel = m_listPicture[m_iCurGroup].GetSelectionMark();
        if (-1 != iSel)
        {
            m_iTestImageIndex = iSel;
        }
		SetTimer(1,time,NULL);
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BTN_START)->SetWindowText("开始");
		KillTimer(1);
		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
	
	}
	
	
}

void CSceneDebugDlg::OnTimer(UINT_PTR nIDEvent)
{
	int listsize = m_listPicture[m_iCurGroup].GetItemCount();
    if (m_iTestImageIndex<listsize)
	{ 
        SCENE_PKG pkg;
        GetSceneParam(m_iCurSceneID, &pkg);
        for (int i = 0; i < pkg.imageCount; i++)
        {
            m_strTemp =m_listPicture[m_iCurGroup].GetItemText(m_iTestImageIndex,1);
            ReadImage(m_strTemp.GetBuffer(),m_iCurSceneID, i);
            DispImage(m_winID,m_iCurSceneID, i);
        }
		if (VisionProcess(m_iCurSceneID,m_iCurIndex,m_strsult))
		{
			DispObj(m_winID,m_iCurSceneID);
			if (0 == m_strsult.err)
			{
				m_listPicture[m_iCurGroup].SetItemText(m_iTestImageIndex,2,"OK");
			} 
			else
			{
				m_listPicture[m_iCurGroup].SetItemText(m_iTestImageIndex,2,"NG");
			}

			m_strTemp.Format("%f",m_strsult.time);
			m_listPicture[m_iCurGroup].SetItemText(m_iTestImageIndex,3,m_strTemp);

		}
		else
		{

			m_listPicture[m_iCurGroup].SetItemText(m_iTestImageIndex,2,"NG");
		}		

        m_iTestImageIndex++;
	}
    else
    {
		KillTimer(1);
        if (IDYES == AfxMessageBox("检测完毕是否继续", MB_YESNO))
        {
            m_iTestImageIndex = 0;

			GetDlgItem(IDC_EDIT_TIME)->GetWindowText(m_strTemp);
			int time = atoi(m_strTemp.GetBuffer());
			SetTimer(1,time,NULL);
        }
        else
        {
			GetDlgItem(IDC_BTN_START)->SetWindowText("开始");
			GetDlgItem(IDC_EDIT_TIME)->EnableWindow(TRUE);
        }
    }

}

void CSceneDebugDlg::OnBnClickedCheckIsShowTmp()
{
    // TODO: 在此添加控件通知处理程序代码
    SCENE_PKG config;
    CButton *pBt = (CButton *)GetDlgItem(IDC_CHECK_IS_SHOW_TMP);
    int iCS = pBt->GetCheck();
    GetSceneParam(m_iCurSceneID, &config);
    if (0 == iCS)  //TBD 待修改为分级显示
    {
        config.display = 0;
        GetDlgItem(IDC_RADIO_DIS_ALL)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RADIO_DIS_1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RADIO_DIS_2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_RADIO_DIS_3)->ShowWindow(SW_HIDE);
    }
    else
    {
        config.display = 1;
        GetDlgItem(IDC_RADIO_DIS_ALL)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_RADIO_DIS_1)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_RADIO_DIS_2)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_RADIO_DIS_3)->ShowWindow(SW_SHOW);
        ((CButton *)GetDlgItem(IDC_RADIO_DIS_ALL))->SetCheck(1);
    }
    SetSceneParam(&config);
}


//void CSceneDebugDlg::OnClickListFunction(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//    // TODO: 在此添加控件通知处理程序代码
//	SCENE_PKG pConfig;				//从配置文件中读取图片有关roi信息
//	GetSceneParam(m_iCurSceneID, &pConfig);
//	int tmp_iType;
//	int iSEBtn = m_SEBtn.size();
//	LONG style;
//    int i = m_listFunction.GetSelectionMark();
//    if (-1 != i)
//    {
//        if (i < m_vecListInfo.size())
//        {
//            vector<unsigned int>::iterator it = m_vecListInfo.begin() + i;
//            m_SceneImageSetting.ShowWindow(SW_HIDE);
//            m_SceneModelSetting.ShowWindow(SW_HIDE);
//            m_SceneThresholdSetting.ShowWindow(SW_HIDE);
//            m_SceneFindLine.ShowWindow(SW_HIDE);
//            m_SceneFindCircle.ShowWindow(SW_HIDE);
//			GetDlgItem(IDC_BUTTON_MORE_CONTTROL)->ShowWindow(SW_HIDE);
//			GetDlgItem(IDC_BUTTON_DELETE_CONTTROL)->ShowWindow(SW_HIDE);
//			//隐藏控件
//            for (int indexE = 0; indexE < m_SEBtn.size();)
//            {
//                if (!strcmp("SEBtn", m_SEBtn[indexE]->BtnType().c_str()))
//                {
//                    m_SEBtn[indexE]->ShowWindow(SW_HIDE);
//                }
//                indexE++;
//            }
//            for (int indexR = 0; indexR < m_SRBtn.size();)
//            {
//                if (!strcmp("SRBtn", m_SRBtn[indexR]->BtnType().c_str()))
//                {
//                    m_SRBtn[indexR]->ShowWindow(SW_HIDE);
//                }
//                indexR++;
//            }
//
//            int iT = (*it)%10000;
//            int iType = (iT)/100;
//            int index = (iT)%100;
//			tmp_iType = iType;
//            m_iCurIndex = (*it)/10000;
//            //DispROI(m_winID,m_iCurSceneID,index,MAIN_PRO);
//            DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
//            switch (iType)
//            {
//            case MAIN_PRO:             
//                {
//                    m_SceneImageSetting.ShowWindow(SW_SHOW);
//                    m_SceneImageSetting.UpdateUI(m_iCurSceneID, index);
//                }
//                break;
//            case MODEL_PRO:                
//                {
//                    DispROI(m_winID,m_iCurSceneID,index,MODEL_PRO);
//                    m_SceneModelSetting.ShowWindow(SW_SHOW);
//                   // m_SceneModelSetting.UpdateUI(m_iCurSceneID, index);
//                    m_SceneModelSetting.UpdateUI(m_iCurSceneID, index, m_hROI);
//                }
//                break;
//            case THRE_PRO:
//                {
//                    //DispROI(m_winID,m_iCurSceneID,index,MODEL_ROI);
//                    m_SceneThresholdSetting.ShowWindow(SW_SHOW);
//                    m_SceneThresholdSetting.UpdateUI(m_iCurSceneID, index);
//                }
//                break;
//            case LINE_PRO:
//                {
//                    bool btool = false;   //用以判断是否有模板匹配
//                    while (true)
//                    {
//                        if (1 == *it%10000/100)
//                        {
//                            btool = true;
//                        }
//                        if (0 == *(--it)%10000/100)
//                        {
//                            break;
//                        }
//                    }
//                    DispROI(m_winID,m_iCurSceneID,index,LINE_PRO);
//                    m_SceneFindLine.ShowWindow(SW_SHOW);
//                    m_SceneFindLine.UpdateUI(m_iCurSceneID, index, m_hROI, btool);
//                }
//                break;
//            case CIRCLE_PRO:
//                {
//                    bool btool = false;    //用以判断是否有模板匹配
//                    while (true)
//                    {
//                        if (1 == *it%10000/100)
//                        {
//                            btool = true;
//                        }
//                        if (0 == *(--it)%10000/100)
//                        {
//                            break;
//                        }
//                    }
//                    DispROI(m_winID,m_iCurSceneID,index,CIRCLE_PRO);
//                    m_SceneFindCircle.ShowWindow(SW_SHOW);
//                    m_SceneFindCircle.UpdateUI(m_iCurSceneID, index ,m_hROI,btool);
//                }
//				break;
//			case MORE_PRO:
//				{
//					style = GetWindowLong(m_listFunction.GetSafeHwnd(), GWL_STYLE);
//					style = style | WS_CLIPSIBLINGS ;
//					SetWindowLong(m_listFunction.GetSafeHwnd(), GWL_STYLE, style);
//
//                    //显示控件
//                    if (m_LoginLevels >= 2)
//                    {
//                        GetDlgItem(IDC_BUTTON_MORE_CONTTROL)->ShowWindow(SW_SHOW);
//                        GetDlgItem(IDC_BUTTON_DELETE_CONTTROL)->ShowWindow(SW_SHOW);
//                    }
//
//					DrawControl();	//当前场景第一次打开更多是读取数据
//
//					if (!m_initControl[m_iCurSceneID])
//					{
//                        int iECount = 0;
//                        int iRCount = 0;
//						for (int i = 0; i < m_interfaceSetData.size(); ++i)
//						{
//                            if (m_SEBtn.size() > 0 && 0 == strcmp("SEBtn", m_SEBtn[i]->BtnType().c_str()))
//                            {
//                                if (iECount < m_SEBtn.size() && m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
//                                {
//                                    m_SEBtn[iECount]->ShowWindow(SW_SHOW);
//                                }
//                                iECount++;
//                            }
//                            else
//                            {
//                                if (iRCount < m_SRBtn.size() && m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
//                                {
//                                    m_SRBtn[iECount]->ShowWindow(SW_SHOW);
//                                }
//                                iRCount++;
//                            }
//						}
//					}
//				}
//                break;
//            }
//            updata_ui_data();
//        }
//    }
//    *pResult = 0;
//	m_sceneChange = false;
//}

void CSceneDebugDlg::ClickProcessFunc()
{
	SCENE_PKG pConfig;				//从配置文件中读取图片有关roi信息
	GetSceneParam(m_iCurSceneID, &pConfig);
	int tmp_iType;
	int iSEBtn =(int)m_SEBtn.size();
	LONG style;
	int i = m_listFunction.GetSelectionMark();
	if (-1 != i)
	{
		if (i < m_vecListInfo.size())
		{
			vector<unsigned int>::iterator it = m_vecListInfo.begin() + i;
			m_SceneImageSetting.ShowWindow(SW_HIDE);
			m_SceneModelSetting.ShowWindow(SW_HIDE);
			m_SceneThresholdSetting.ShowWindow(SW_HIDE);
			m_SceneFindLine.ShowWindow(SW_HIDE);
			m_SceneFindCircle.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_MORE_CONTTROL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_DELETE_CONTTROL)->ShowWindow(SW_HIDE);
			//隐藏控件
			for (int indexE = 0; indexE < m_SEBtn.size();)
			{
				if (!strcmp("SEBtn", m_SEBtn[indexE]->BtnType().c_str()))
				{
					m_SEBtn[indexE]->ShowWindow(SW_HIDE);
				}
				indexE++;
			}
			for (int indexR = 0; indexR < m_SRBtn.size();)
			{
				if (!strcmp("SRBtn", m_SRBtn[indexR]->BtnType().c_str()))
				{
					m_SRBtn[indexR]->ShowWindow(SW_HIDE);
				}
				indexR++;
			}

			int iT = (*it)%10000;
			int iType = (iT)/100;
			int index = (iT)%100;
			tmp_iType = iType;
			m_iCurIndex = (*it)/10000;
			//DispROI(m_winID,m_iCurSceneID,index,MAIN_PRO);
			DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
			switch (iType)
			{
			case MAIN_PRO:             
				{
					m_SceneImageSetting.ShowWindow(SW_SHOW);
					m_SceneImageSetting.UpdateUI(m_iCurSceneID, index);
				}
				break;
			case MODEL_PRO:                
				{
					DispROI(m_winID,m_iCurSceneID,index,MODEL_PRO);
					m_SceneModelSetting.ShowWindow(SW_SHOW);
					// m_SceneModelSetting.UpdateUI(m_iCurSceneID, index);
					m_SceneModelSetting.UpdateUI(m_iCurSceneID, index, m_hROI);
				}
				break;
			case THRE_PRO:
				{
					//DispROI(m_winID,m_iCurSceneID,index,MODEL_ROI);
					m_SceneThresholdSetting.ShowWindow(SW_SHOW);
					m_SceneThresholdSetting.UpdateUI(m_iCurSceneID, index);
				}
				break;
			case LINE_PRO:
				{
					bool btool = false;   //用以判断是否有模板匹配
					while (true)
					{
						if (1 == *it%10000/100)
						{
							btool = true;
						}
						if (0 == *(--it)%10000/100)
						{
							break;
						}
					}
					DispROI(m_winID,m_iCurSceneID,index,LINE_PRO);
					m_SceneFindLine.ShowWindow(SW_SHOW);
					m_SceneFindLine.UpdateUI(m_iCurSceneID, index, m_hROI, btool);
				}
				break;
			case CIRCLE_PRO:
				{
					bool btool = false;    //用以判断是否有模板匹配
					while (true)
					{
						if (1 == *it%10000/100)
						{
							btool = true;
						}
						if (0 == *(--it)%10000/100)
						{
							break;
						}
					}
					DispROI(m_winID,m_iCurSceneID,index,CIRCLE_PRO);
					m_SceneFindCircle.ShowWindow(SW_SHOW);
					m_SceneFindCircle.UpdateUI(m_iCurSceneID, index ,m_hROI,btool);
				}
				break;
			case MORE_PRO:
				{
					style = GetWindowLong(m_listFunction.GetSafeHwnd(), GWL_STYLE);
					style = style | WS_CLIPSIBLINGS ;
					SetWindowLong(m_listFunction.GetSafeHwnd(), GWL_STYLE, style);

					//显示控件
					if (m_LoginLevels >= 2)
					{
						GetDlgItem(IDC_BUTTON_MORE_CONTTROL)->ShowWindow(SW_SHOW);
						GetDlgItem(IDC_BUTTON_DELETE_CONTTROL)->ShowWindow(SW_SHOW);
					}

					DrawControl();	//当前场景第一次打开更多是读取数据

					if (!m_initControl[m_iCurSceneID])
					{
						int iECount = 0;
						int iRCount = 0;
						for (int i = 0; i < m_interfaceSetData.size(); ++i)
						{
							if (m_SEBtn.size() > 0 && 0 == strcmp("SEBtn", m_SEBtn[i]->BtnType().c_str()))
							{
								if (iECount < m_SEBtn.size() && m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
								{
									m_SEBtn[iECount]->ShowWindow(SW_SHOW);
								}
								iECount++;
							}
							else
							{
								if (iRCount < m_SRBtn.size() && m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
								{
									m_SRBtn[iECount]->ShowWindow(SW_SHOW);
								}
								iRCount++;
							}
						}
					}
				}
				break;
			}
			updata_ui_data();
		}
	}
}

void CSceneDebugDlg::ClickWay2Func()
{
	//隐藏所用功能的配置窗口
	ProcList* pProcList = NULL;
	
	for (int k = 0;k<g_SceneList.GetSceneCount();k++)
	{
	    pProcList = g_SceneList.GetSceneBySceneID(k);
		for (int i = 0;i< pProcList->GetProcCount();i++)
		{
			if (!pProcList->GetProcByIndex(i))
			{
				continue;
			}
			int procID = pProcList->GetProcByIndex(i)->GetProcId();
			if (pProcList->GetProcByIndex(i)->IsInitialConfigWnd())//全局的不等于成员的
			{
				pProcList->GetProcByIndex(i)->InitInstance(m_pFunSettingWnd);
			}
			pProcList->GetProcByIndex(i)->HideConfigWnd();

		}
	}
	//显示选中功能的配置窗口
	pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
	int selIndex = m_listFunction.GetSelectionMark();
	m_CurProcIndex = selIndex;
	pProcList->GetProcByIndex(selIndex)->ShowConfigWnd();

	if (IMAGE_ACQISITION == pProcList->GetProcByIndex(selIndex)->m_ProcTypeId)
	{
		m_CurCamIndex =((SceneAcqImage*)pProcList->GetProcByIndex(selIndex))->m_CamIndex;
	}
		
	//m_iCurIndex = pProcList->GetTypeIndex(IMAGE_ACQISITION,pProcList->GetProcByIndex(selIndex));

	m_iCurIndex = pProcList->GetProcImageIndex(pProcList->GetProcByIndex(selIndex)->GetProcId());

	if ( 0 > m_iCurIndex)
	{
		m_iCurIndex = 0;
	}

}


void CSceneDebugDlg::OnClickListFunction(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	//1，此时点击的是场景的某一个处理项，从场景参数中获取相应处理项的配置参数，
	SCENE_PKG pConfig;				//从配置文件中读取有关参数
	GetSceneParam(m_iCurSceneID, &pConfig);
	 //int i = m_listFunction.GetSelectionMark(); 

	//2，根据配置参数获取该项的库的名字，调用该库的创建实体函数创建该处理项的对象。
	   



	//3，根据名字，创建对应项的对话框，其子类对象（实际处理项的基类）指针指向上一步创建的对象。

	    //（说明：本来是想根据配置文件的参数，对通用参数配置对话框ApplyFuction进行配置,动态创建各控件，以适应对应的处理项。
	    //(对话框控件的设定、调整如何对应到相应的处理项的参数，采用的方法是将控件实时变化的值放到一个数据list中，处理项在调用相应
	    //  函数进行图像处理时，从该数据list中读取相应的参数，完成处理。问题是如何方便的配置对话框，这个课题可能有风险，所有暂时采用事先
	    //  定义对应处理项的对话框及其相应控件的方法。)


	//4，将对话框显示出来。

	if ( 0 == pConfig.ProcProcess)
	{
		ClickWay2Func();

	}else
	{
		ClickProcessFunc();

	}

	*pResult = 0;
	m_sceneChange = false;
}



void CSceneDebugDlg::OnDblclkListPicture(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    int index = m_listPicture[m_iCurGroup].GetSelectionMark();
    m_strTemp =m_listPicture[m_iCurGroup].GetItemText(index,1);
   // ReadImage(m_strTemp.GetBuffer(),m_iCurSceneID, m_iCurGroup);
   // DispImage(m_winID,m_iCurSceneID, m_iCurGroup);
    
    *pResult = 0;
}

void CSceneDebugDlg::DBClickProcessFunc()
{
	int iType = 0;
	int index = 0;
	RECT rcClip;           // new area for ClipCursor
	RECT rcOldClip;        // previous area for ClipCursor
	int i = m_listFunction.GetSelectionMark();
	if (-1 != i)
	{
		GetClipCursor(&rcOldClip); 

		// Get the dimensions of the application's window. 
		((CWnd *)GetDlgItem(IDC_STATIC_BIND))->GetWindowRect(&rcClip);

		// Confine the cursor to the application's window. 
		ClipCursor(&rcClip); 
		if (i < m_vecListInfo.size())
		{
			vector<unsigned int>::iterator it = m_vecListInfo.begin() + i;
			int iT = (*it)%10000;
			iType = (iT)/100;
			index = (iT)%100;
			switch (iType)
			{
			case MAIN_PRO:
				{
					//DrawROI(m_winID,m_iCurSceneID,index,MAIN_PRO);
				}
				break;
			case MODEL_PRO:
				{
					DrawROI(m_winID,m_iCurSceneID,index,MODEL_PRO);
				}
				break;
			case THRE_PRO:
				{
					//DrawROI(m_winID,m_iCurSceneID,index,MODEL_ROI);
				}
				break;
			case LINE_PRO:
				{
					DrawROI(m_winID,m_iCurSceneID,index,LINE_PRO);
				}
				break;
			case CIRCLE_PRO:
				{
					DrawROI(m_winID,m_iCurSceneID,index,CIRCLE_PRO);
				}
				break;
			}
			updata_ui_data(iType, index);
		}
		ClipCursor(&rcOldClip); 
	}
}

void CSceneDebugDlg::DBClickWay2Func()
{

}

int  CSceneDebugDlg::GetSceneFirstCamIndex(int SceneID)
{
	VisionProcBase* pVisionProc = NULL;
	ProcList* pScene =  g_SceneList.GetSceneBySceneID(SceneID);

	if (!pScene)
	{
		return -1;
	}
	for (int i=0;i<pScene->GetProcCount();i++)
	{
		if(pScene->GetProcType(i) == IMAGE_ACQISITION ) //VisionProcBase::GRAB_IMABGE 
		{
			pVisionProc = pScene->GetProcByIndex(i);
			if (!pVisionProc)
			{
				return -1;
			}else
			{
				return ((SceneAcqImage*)pVisionProc)->m_CamIndex;
			}

			//break;  //只抓场景中的第一个图像
		}
	}
	return -1;
}



void CSceneDebugDlg::OnDblclkListFunction(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
	SCENE_PKG pConfig;				//从配置文件中读取有关参数
	GetSceneParam(m_iCurSceneID, &pConfig);
	if ( 0 == pConfig.ProcProcess)
	{
		DBClickWay2Func();
	} 
	else
	{
		DBClickProcessFunc();
	}

    *pResult = 0;
}

void CSceneDebugDlg::SaveSceneWay2Config()
{
	 CString strTemp;
	 int ProcNum = 0;
	 SCENE_PKG* pConfig = (SCENE_PKG*)GetSceneParamHandle(m_iCurSceneID);
	 //读取修改的场景参数 
	 pConfig->AxisIndex = m_SelAxisIndex.GetItemData(m_SelAxisIndex.GetCurSel());
	 pConfig->MotorToolIndex = m_SelToolIndex.GetItemData(m_SelToolIndex.GetCurSel());

	 CString strConfigPath =  GetSceneConfigPath(m_iCurSceneID); 
	 CFile SaveFile(strConfigPath,CFile::modeCreate); ///新建立一个文件,然后把所有信息都保存进去,这个方式不高效
	 SaveFile.Close();

	 //对处理模块的类型ID进行说明
	  WritePrivateProfileStringA("scene","ProcType的说明：","",strConfigPath);
	  
	  strTemp.Format("ProcType=%d:模板匹配",MODEL_PRO);
	  WritePrivateProfileStringA("scene",strTemp,"",strConfigPath);
	  strTemp.Format("ProcType=%d:图像读取",IMAGE_READ);
	  WritePrivateProfileStringA("scene",strTemp,"",strConfigPath);
	  strTemp.Format("ProcType=%d:图像采集",IMAGE_ACQISITION);
	  WritePrivateProfileStringA("scene",strTemp,"",strConfigPath);
	  strTemp.Format("ProcType=%d:轮廓定位",CONTOUR_POSITION);
	  WritePrivateProfileStringA("scene",strTemp,"",strConfigPath);
	  strTemp.Format("ProcType=%d:逻辑控制",LOGIC_CTR);
	  WritePrivateProfileStringA("scene",strTemp,"",strConfigPath);

	 //场景基础信息保存
	 strTemp.Format("%d",m_iCurSceneID);
	 WritePrivateProfileStringA("scene","sceneId",strTemp,strConfigPath);
	 ProcNum = g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcCount();
	 strTemp.Format("%d",ProcNum);
     WritePrivateProfileStringA("scene","pkgnum",strTemp,strConfigPath);
	 strTemp.Format("%d",0);
	 WritePrivateProfileStringA("scene","ProcProcess",strTemp,strConfigPath);

	 strTemp.Format("%d",pConfig->MotorToolIndex);
	 WritePrivateProfileStringA("scene","MotorToolIndex",strTemp,strConfigPath);

	 strTemp.Format("%d",pConfig->CalPointNum);
	 WritePrivateProfileStringA("scene","CalPointNum",strTemp,strConfigPath);

	 strTemp.Format("%d",pConfig->SceneType);
	 WritePrivateProfileStringA("scene","sceneType",strTemp,strConfigPath);

	 strTemp.Format("%d",pConfig->AxisIndex);
	 WritePrivateProfileStringA("scene","AxisIndex",strTemp,strConfigPath);
	 
	 
	 strTemp.Format("%d",pConfig->AxisIndex);
	 WritePrivateProfileStringA("scene","AxisIndex",strTemp,strConfigPath);
	 //以下几个参数可能用不到，这里要跟以前一致
	 strTemp.Format("%d",pConfig->bSave);
	 WritePrivateProfileStringA("scene", "affine", strTemp, strConfigPath);
	 strTemp.Format("%d",pConfig->bIsSimulate);
	 WritePrivateProfileStringA("scene", "simulate", strTemp, strConfigPath);
	 strTemp.Format("%d",pConfig->stationID);
	 WritePrivateProfileStringA("scene", "station", strTemp, strConfigPath);
	 strTemp.Format("%d",pConfig->imageCount);
	 WritePrivateProfileStringA("scene", "imagenum", strTemp, strConfigPath);
	 strTemp.Format("%d",pConfig->pkgBodySize);
	 WritePrivateProfileStringA("scene", "pkgSize", strTemp, strConfigPath);
	 strTemp.Format("%d",pConfig->ModelCount);
	 WritePrivateProfileStringA("scene", "modelnum", strTemp, strConfigPath);
	 strTemp.Format("%d",pConfig->InterfaceDataNumb);
	 WritePrivateProfileStringA("scene", "InterfaceDataNumb", strTemp, strConfigPath);


	 strTemp.Format("%lf",pConfig->defcamwidth);
	 WritePrivateProfileStringA("scene", "defcamwidth", strTemp, strConfigPath);
	 strTemp.Format("%lf",pConfig->defcamheight);
	 WritePrivateProfileStringA("scene", "defcamheight", strTemp, strConfigPath);
	 strTemp.Format("%d",pConfig->testcount);
	 WritePrivateProfileStringA("scene","testcount",strTemp,strConfigPath);

	 //
	 ProcList* pProcList = NULL;
	 pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
	 if (pProcList)
	 {
		 strTemp.Format("%d",pProcList->GetRelaSceneID());
		 WritePrivateProfileStringA("scene", "LastSceneId", strTemp, strConfigPath);

		 strTemp.Format("%d",pProcList->GetNextSceneID());
		 WritePrivateProfileStringA("scene", "NextSceneId", strTemp, strConfigPath);

		 m_NextSceneRot.GetWindowTextA(strTemp);
		 WritePrivateProfileStringA("scene", "NextSceneRotAngle", strTemp, strConfigPath);
		 pProcList->SetNextRotAngle(atof(strTemp));

	 }



	 //场景中各个处理的信息保存
	 for (int i = 0;i<ProcNum;i++)
	 {
          g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcByIndex(i)->SaveProcConfig(i);
	 }

}


void CSceneDebugDlg::OnBnClickedBtnSaveParam()
{
    // TODO: 在此添加控件通知处理程序代码
    if (IDYES == AfxMessageBox("保存修改", MB_YESNO))
    {
		SCENE_PKG* pConfig = (SCENE_PKG*)GetSceneParamHandle(m_iCurSceneID);
		if ( 0 == pConfig->ProcProcess)
		{
			SaveSceneWay2Config();

		}else
		{
			SaveSceneParam(m_iCurSceneID);
		}
    }
}


void CSceneDebugDlg::OnBnClickedButtonAddFunction()//添加功能按钮
{
    // TODO: 在此添加控件通知处理程序代码
    CPoint pt;
    CRect rt;
    GetCursorPos(&pt); //获取鼠标
    CSceneDebugDlg *pTmp = this;
    HWND hwd = pTmp->m_hWnd;
    m_SelectFunction.GetClientRect(rt);//获取函数选择对话框的大小
    m_SelectFunction.SetWindowPos(NULL,pt.x,pt.y - rt.Height(),rt.Width(),rt.Height(),SWP_NOSIZE);
    m_SelectFunction.UpdateUI(m_iCurSceneID, m_iCurIndex);//id，当前第几个图形处理m_iCurIndex，没什么用,这里传过来的是图像处理第几项
    m_SelectFunction.ShowWindow(SW_SHOW);
    m_SelectFunction.SetFocus();
}


void CSceneDebugDlg::DelSceneFunc(int selIndex)
{
	ProcList* pProcList = NULL;
	pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
	VisionProcBase* pProcBase = NULL;

	if( selIndex < pProcList->GetProcCount()&& selIndex >= 0 )
	{	
		//delete ((ScenePositon*)pProcList->GetProcByIndex(selIndex));
		pProcList->delProc(selIndex);
		//pProcList->RemoveProc(selIndex);

	}else
	{
		//ProcIdBase = 0;
	}

	//BuildProc(&pProcBase,SceneConfigPath,FunType,m_iCurSceneID,ProcIdBase);


}


void CSceneDebugDlg::OnBnClickedButtonSubFunction()
{
    // TODO: 在此添加控件通知处理程序代码
    int iType = 0;
    int iSel = m_listFunction.GetSelectionMark();
    int iCount = m_listFunction.GetItemCount();
    SCENE_PKG config;
    int          iCurSize = 0;

    if (IDNO == AfxMessageBox("确认删除", MB_YESNO))
    {
        return ;
    }
    GetSceneParam(m_iCurSceneID, &config);
	if (0 == config.ProcProcess)
	{

		DelSceneFunc(iSel);
		updat_way2_list();

		return ;
	}

	if (iSel < 0 || iSel >= m_vecListInfo.size())
	{
		AfxMessageBox("请选择要删除项");
		return;
	}

	vector<unsigned int>::iterator it = m_vecListInfo.begin() + iSel;
	iType = ((*it)%10000)/100;
	switch (iType)
	{
	case MAIN_PRO:
		{
			if (1 != config.pkgCount && iSel < config.pkgCount -1 && iType != (m_vecListInfo[iSel + 1]%10000)/100)
			{
				AfxMessageBox("请先清空当前图像下数据！");
				return;
			}
			if (config.imageCount == 2)
			{
				CButton *pBt = (CButton *)GetDlgItem(IDC_BUTTON_I1 + 1);
				pBt->ShowWindow(SW_HIDE);  //ID要保证连续
			}
			CButton *pBt = (CButton *)GetDlgItem(IDC_BUTTON_I1 + config.imageCount -1);
			pBt->ShowWindow(SW_HIDE);  //ID要保证连续
		}
	case MODEL_PRO:
	case THRE_PRO:
	case LINE_PRO:
	case CIRCLE_PRO:
		{
			int index = m_vecListInfo[iSel]%10000;
			index = index%100;
			int iPos = IFindParamData(&config, iType, index);
			if (iPos >= 0)
			{
				DeleteParamItem(m_iCurSceneID,iPos);
				//删除显示列表中数据
				vector<unsigned int>::iterator itI = m_vecListInfo.begin() + iSel;
				m_vecListInfo.erase(itI);
			}
		}
		break;
	case MORE_PRO:
		{
			if (0 < config.InterfaceDataNumb)
			{
				AfxMessageBox("请先手动清空控件");
				return;
			}
			else if (0 == config.InterfaceDataNumb)
			{
				int index = m_vecListInfo[iSel]%10000;
				index = index%100;
				int iPos = IFindParamData(&config, iType, index);
				if (iPos >= 0)
				{
					DeleteParamItem(m_iCurSceneID,iPos);
					//删除显示列表中数据
					vector<unsigned int>::iterator itI = m_vecListInfo.begin() + iSel;
					m_vecListInfo.erase(itI);
				}
			}
		}
		break;
	default:
		break;
	}

	for (int index = iSel; index < m_vecListInfo.size(); index++)
	{
		if (iType == m_vecListInfo[index]%10000/100)
		{
			if (0 == iType)
			{
				m_vecListInfo[index] = m_vecListInfo[index] -10001;
				int lindex = index+1;
				if (lindex < m_vecListInfo.size())
				{
					while (m_vecListInfo[lindex]%10000/100)
					{
						m_vecListInfo[lindex] = m_vecListInfo[lindex] - 10000;
						lindex++;
						if (lindex >= m_vecListInfo.size())
						{
							break;
						}

					}
				}
			}
			else
			{
				m_vecListInfo[index]--;
			}
		}
	}
    updata_list();

}


void CSceneDebugDlg::OnDblclkStaticBind()
{
    // TODO: 在此添加控件通知处理程序代码
    CRect rt;
    ((CWnd *)GetDlgItem(IDC_STATIC_BIND))->GetWindowRect(&rt);
    RestoreImagePart(m_winID, m_iCurSceneID,rt.Width(), rt.Height());

}


void CSceneDebugDlg::OnBnClickedButtonRestore()
{
    // TODO: 在此添加控件通知处理程序代码
    CRect rt;
    ((CWnd *)GetDlgItem(IDC_STATIC_BIND))->GetWindowRect(&rt);
    ClearWindow(m_winID);
    RestoreImagePart(m_winID, m_iCurSceneID,rt.Width(), rt.Height());
    //UpdateImagePart(m_winID, m_iCurSceneID, RESTORE_TYPE);
    //DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
    DispObj(m_winID,m_iCurSceneID,true);
}

//void CSceneDebugDlg::UpdateImagePart(double dPer, Point point)
//{
//    CRect rt;
//    GetDlgItem(IDC_STATIC_BIND)->GetWindowRect(&rt);
//    double xp = point.X/rt.Width();
//    double yp = point.Y/rt.Height();
//    dooble w, h;
//    xb = w * xp /dPer;
//    Getim
//}

void CSceneDebugDlg::OnBnClickedCheckIsAffine()
{
    // TODO: 在此添加控件通知处理程序代码
    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);
    if (((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->GetCheck())
    {
        config.bSave = true;
    }
    else
    {
        config.bSave = false;
    }
    SetSceneParam(&config);
}

void CSceneDebugDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    CDialogEx::OnCancel();
}


BOOL CSceneDebugDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    int iType = 0;
    msg_post(RECV_UI_LOGIN_MSG,&iType,sizeof(int),MODULE_SCENE_MAIN_SDK);
    if (m_bIsContinousGrab)
    {
        ColseContinousGrab(m_iCurSceneID, m_iCurIndex,m_iCurIndex);
    }
	//删除halcon里的id绑定
    unregister_notify(MODULE_SCENE_DEBUG_UI,OnRemoteMessage);
    return CDialogEx::DestroyWindow();
}


void CSceneDebugDlg::OnBnClickedCheckIsTop()
{
    // TODO: 在此添加控件通知处理程序代码
    if (((CButton *)GetDlgItem(IDC_CHECK_IS_TOP))->GetCheck())
    {
        ::SetWindowPos(this->m_hWnd,HWND_TOPMOST,10,10,10,10,SWP_NOMOVE|SWP_NOSIZE);
    }
    else
    {
        ::SetWindowPos(this->m_hWnd,HWND_NOTOPMOST,10,10,10,10,SWP_NOMOVE|SWP_NOSIZE);
    }
}


void CSceneDebugDlg::OnBnClickedRadioDisAll()
{
    // TODO: 在此添加控件通知处理程序代码
    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);
    if (((CButton *)GetDlgItem(IDC_RADIO_DIS_ALL))->GetCheck())
    {
        config.display = 1;
    }
    SetSceneParam(&config);
}


void CSceneDebugDlg::OnBnClickedRadioDis1()
{
    // TODO: 在此添加控件通知处理程序代码
    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);
    if (((CButton *)GetDlgItem(IDC_RADIO_DIS_1))->GetCheck())
    {
        config.display = 2;
    }
    SetSceneParam(&config);
}


void CSceneDebugDlg::OnBnClickedRadioDis2()
{
    // TODO: 在此添加控件通知处理程序代码
    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);
    if (((CButton *)GetDlgItem(IDC_RADIO_DIS_2))->GetCheck())
    {
        config.display = 3;
    }
    SetSceneParam(&config);
}


void CSceneDebugDlg::OnBnClickedRadioDis3()
{
    // TODO: 在此添加控件通知处理程序代码
    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);
    if (((CButton *)GetDlgItem(IDC_RADIO_DIS_3))->GetCheck())
    {
        config.display = 4;
    }
    SetSceneParam(&config);
}


void CSceneDebugDlg::OnBnClickedButtonI1()
{
    // TODO: 在此添加控件通知处理程序代码
    ShowFileListCtrl(0);
}


void CSceneDebugDlg::OnBnClickedButtonI2()
{
    // TODO: 在此添加控件通知处理程序代码
    ShowFileListCtrl(1);
}


void CSceneDebugDlg::OnBnClickedButtonI3()
{
    // TODO: 在此添加控件通知处理程序代码
    ShowFileListCtrl(2);
}


void CSceneDebugDlg::OnBnClickedButtonI4()
{
    // TODO: 在此添加控件通知处理程序代码
    ShowFileListCtrl(3);
}

void CSceneDebugDlg::ShowFileListCtrl(int index)
{
    for (int i = 0; i < FILE_LIST_CTRL_COUNT; i++)
    {
        if (index == i)
        {
            m_listPicture[i].ShowWindow(SW_SHOW);
            m_iCurGroup = index;
        }
        else
        {
            m_listPicture[i].ShowWindow(SW_HIDE);
        }
    }
}



void CSceneDebugDlg::OnBnClickedButtonMoreConttrol()
{
	// TODO: 在此添加控件通知处理程序代码
	int i = m_listFunction.GetSelectionMark();
	int nIndex = 0;
	INTERFACE_SET_DATA interfaceSetData;

	if (i < m_vecListInfo.size())
	{
		vector<unsigned int>::iterator it = m_vecListInfo.begin() + i;
		int iT = (*it)%10000;
		int iType = (iT)/100;
		int index = (iT)%100;
		m_iCurIndex = (*it)/10000;
		int iValue = 0;

		vector<VIRSION_DATA_PKG *>::iterator itConfig;
		SCENE_PKG pConfig;				//从配置文件中读取图片有关roi信息
		GetSceneParam(m_iCurSceneID, &pConfig);
		VIRSION_DATA_PKG *pTmp = NULL;
		//获取已有数据numb
		for (itConfig = pConfig.vecBody.begin(); itConfig != pConfig.vecBody.end(); itConfig++)
		{
			pTmp = *itConfig;
			if (pTmp->iValue == *it)
			{
				iValue = pTmp[1].iValue;
				break;
			}
		}

		interfaceSetData = m_InterfaceSet.showWindow(m_InterfaceSet, m_iCurSceneID, iT);

		if (!strcmp("SEBtn", interfaceSetData.BtnType.c_str()))	//点击确定后
		{
			//获得界面设置参数
			m_interfaceSetData.push_back(interfaceSetData);
			m_interfaceSetData[m_index].nIndex = iValue;
			m_interfaceSetData[m_index].iT = iT;
			m_interfaceSetData[m_index].CurSceneID = m_iCurSceneID;

			CWnd *pWnd = GetDlgItem(IDC_STATIC_SETTING);
			CRect rtS;

			nIndex = (int)m_SEBtn.size();
			m_SEBtn.resize(nIndex + 1);
			m_SEBtn[nIndex] = new CSEBtn;
			m_SEBtn[nIndex]->Create(IDD_DIALOG_SEBTN, pWnd);
			m_SEBtn[nIndex]->GetClientRect(&rtS);
			//设置控件显示高度
			//for (int i = 0; i < pConfig.InterfaceDataNumb; ++i)
			for (int i = 0; i < m_interfaceSetData.size() - 1; ++i)
			{
				if (m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
				{
					m_interfaceSetData[m_index].height = m_interfaceSetData[i].height + rtS.bottom;
				}
				else
				{
					//m_interfaceSetData[m_index].height = 0;
				}
			}
			m_SEBtn[nIndex]->SetWindowPos(&wndTop, rtS.left, m_interfaceSetData[m_index].height, rtS.right, rtS.bottom, 
				SWP_SHOWWINDOW);
			m_height += rtS.bottom;
			m_SEBtn[nIndex]->getInterfaceSetData(m_interfaceSetData[m_index]);
			m_SEBtn[nIndex]->ShowWindow(SW_SHOW);
			m_SEBtn[nIndex]->SetDisplayWindowID(m_winID);
			m_SEBtn[nIndex]->UpdateUI(m_iCurSceneID, m_interfaceSetData[m_index].nIndex, m_hROI, 1);
			++m_index;
		}
		else if (!strcmp("SRBtn", interfaceSetData.BtnType.c_str()))
		{
			//获得界面设置参数
			m_interfaceSetData.push_back(interfaceSetData);
			m_interfaceSetData[m_index].nIndex = iValue;
			m_interfaceSetData[m_index].iT = iT;
			m_interfaceSetData[m_index].CurSceneID = m_iCurSceneID;


			CWnd *pWnd = GetDlgItem(IDC_STATIC_SETTING);
			CRect rtS;

			nIndex = (int)m_SRBtn.size();
			m_SRBtn.resize(nIndex + 1);
			m_SRBtn[nIndex] = new CSRBtn;
			m_SRBtn[nIndex]->Create(IDD_DIALOG_SRBTN, pWnd);
			m_SRBtn[nIndex]->GetClientRect(&rtS);
			//设置控件显示高度
			for (int i = 0; i < m_interfaceSetData.size() - 1; ++i)
			{
				if (m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
				{
					m_interfaceSetData[m_index].height = m_interfaceSetData[i].height + rtS.bottom;
				}
				else
				{
					//m_interfaceSetData[m_index].height = 0;
				}
			}
			m_SRBtn[nIndex]->SetWindowPos(&wndTop, rtS.left, m_interfaceSetData[m_index].height, rtS.right, rtS.bottom, 
				SWP_SHOWWINDOW);
			m_height += rtS.bottom;
			m_SRBtn[nIndex]->getInterfaceSetData(m_interfaceSetData[m_index]);
			m_SRBtn[nIndex]->ShowWindow(SW_SHOW);
			m_SRBtn[nIndex]->SetDisplayWindowID(m_winID);
			m_SRBtn[nIndex]->UpdateUI(m_iCurSceneID, m_interfaceSetData[m_index].nIndex, m_hROI, 1);
			//m_SEBtn[nIndex]->UpdateUI(m_iCurSceneID, m_interfaceSetData[m_index].nIndex, m_hROI, 1);

			++m_index;
		}

		//给控件增加 WS_CLIPSIBLINGS 属性
		LONG style;

		style = GetWindowLong(m_SceneImageSetting.GetSafeHwnd(), GWL_STYLE);
		style = style | WS_CLIPSIBLINGS ;
		SetWindowLong(m_SceneImageSetting.GetSafeHwnd(), GWL_STYLE, style);

		style = GetWindowLong(m_SceneModelSetting.GetSafeHwnd(), GWL_STYLE);
		style = style | WS_CLIPSIBLINGS ;
		SetWindowLong(m_SceneModelSetting.GetSafeHwnd(), GWL_STYLE, style);

		style = GetWindowLong(m_SceneThresholdSetting.GetSafeHwnd(), GWL_STYLE);
		style = style | WS_CLIPSIBLINGS ;
		SetWindowLong(m_SceneThresholdSetting.GetSafeHwnd(), GWL_STYLE, style);

		style = GetWindowLong(m_SceneFindLine.GetSafeHwnd(), GWL_STYLE);
		style = style | WS_CLIPSIBLINGS ;
		SetWindowLong(m_SceneFindLine.GetSafeHwnd(), GWL_STYLE, style);

		style = GetWindowLong(m_SceneFindCircle.GetSafeHwnd(), GWL_STYLE);
		style = style | WS_CLIPSIBLINGS ;
		SetWindowLong(m_SceneFindCircle.GetSafeHwnd(), GWL_STYLE, style);
	}
}


void CSceneDebugDlg::OnBnClickedButtonDeleteConttrol()
{
	// TODO: 在此添加控件通知处理程序代码
	if (0 < m_SEBtn.size() || 0 < m_SRBtn.size())
	{
		if (0 < m_SEBtn.size())
		{
			m_SEBtn[0]->UpdateUI(m_iCurSceneID, m_iCurSceneID, m_hROI, -1);	//清除内存中的数据

			for (int i = 0; i < m_SEBtn.size(); ++i)
			{
				if (m_iCurSceneID == m_SEBtn[i]->CurSceneID())
				{
					m_SEBtn[i]->ShowWindow(SW_HIDE);
					vector<CSEBtn*>::iterator itI = m_SEBtn.begin() + i;
					m_SEBtn.erase(itI);
					--i;
				}
			}
		}
		if (0 < m_SRBtn.size())
		{
			m_SRBtn[0]->UpdateUI(m_iCurSceneID, m_iCurSceneID, m_hROI, -1);	//清除内存中的数据

			for (int i = 0; i < m_SRBtn.size(); ++i)
			{
				if (m_iCurSceneID == m_SRBtn[i]->CurSceneID())
				{
					m_SRBtn[i]->ShowWindow(SW_HIDE);
					vector<CSRBtn*>::iterator itI = m_SRBtn.begin() + i;
					m_SRBtn.erase(itI);
					--i;
				}
			}
		}
		for (int i = 0; i < m_interfaceSetData.size(); ++i)
		{
			if (m_iCurSceneID == m_interfaceSetData[i].CurSceneID
				&& (!strcmp("SEBtn", m_interfaceSetData[i].BtnType.c_str())
					|| !strcmp("SRBtn", m_interfaceSetData[i].BtnType.c_str())))
			{
				vector<INTERFACE_SET_DATA>::iterator itI = m_interfaceSetData.begin() + i;
				m_interfaceSetData.erase(itI);
				--i;
			}
		}
		//清空数据
		m_index = (int)m_interfaceSetData.size();
		m_height = 0;
	}
}

void CSceneDebugDlg::DrawControl()
{
	int SEBtnSize = 0;
	int SRBtnSize = 0;
	int nIndex = 0;
	CWnd *pWnd = GetDlgItem(IDC_STATIC_SETTING);
	CRect rtS;
	SCENE_PKG pConfig;				//从配置文件中读取图片有关roi信息
	GetSceneParam(m_iCurSceneID, &pConfig);

	if (!m_initControl[m_iCurSceneID]
	|| 0 >= pConfig.InterfaceDataNumb)		//判断当前场景是否是第一次打开
	{
		m_initControl[m_iCurSceneID] = false;
		return;
	}

	nIndex = (int)m_interfaceSetData.size();

	//读取数据
	for (int i = 0; i < pConfig.InterfaceDataNumb; ++i)
	{
		m_interfaceSetData.push_back(*pConfig.InterfaceSetData[i]);
		if (!strcmp("SEBtn", m_interfaceSetData[i + nIndex].BtnType.c_str())
			&& m_iCurSceneID == m_interfaceSetData[i + nIndex].CurSceneID)
		{
			SEBtnSize = (int)m_SEBtn.size();
			m_SEBtn.resize(SEBtnSize + 1);
			m_SEBtn[SEBtnSize] = new CSEBtn;
			m_SEBtn[SEBtnSize]->Create(IDD_DIALOG_SEBTN, pWnd);
			m_SEBtn[SEBtnSize]->GetClientRect(&rtS);
			m_SEBtn[SEBtnSize]->SetWindowPos(&wndTop, rtS.left, pConfig.InterfaceSetData[i]->height/*m_interfaceSetData[i].height*/, rtS.right, rtS.bottom, 
				SWP_SHOWWINDOW);
			m_SEBtn[SEBtnSize]->getInterfaceSetData(*pConfig.InterfaceSetData[i]);
			m_SEBtn[SEBtnSize]->ShowWindow(SW_SHOW);
			m_SEBtn[SEBtnSize]->SetDisplayWindowID(m_winID);
			m_SEBtn[SEBtnSize]->UpdateUI(m_iCurSceneID, pConfig.InterfaceSetData[i]->nIndex, m_hROI);
			m_height = pConfig.InterfaceSetData[i]->height + rtS.bottom;
			++m_index;
		}
		else if (!strcmp("SRBtn", m_interfaceSetData[i + nIndex].BtnType.c_str())
			&& m_iCurSceneID == m_interfaceSetData[i + nIndex].CurSceneID)
		{
			SRBtnSize = (int)m_SRBtn.size();
			m_SRBtn.resize(SRBtnSize + 1);
			m_SRBtn[SRBtnSize] = new CSRBtn;
			m_SRBtn[SRBtnSize]->Create(IDD_DIALOG_SRBTN, pWnd);
			m_SRBtn[SRBtnSize]->GetClientRect(&rtS);
			m_SRBtn[SRBtnSize]->SetWindowPos(&wndTop, rtS.left, pConfig.InterfaceSetData[i]->height, rtS.right, rtS.bottom, 
				SWP_SHOWWINDOW);
			m_SRBtn[SRBtnSize]->getInterfaceSetData(*pConfig.InterfaceSetData[i]);
			m_SRBtn[SRBtnSize]->ShowWindow(SW_SHOW);
			m_SRBtn[SRBtnSize]->SetDisplayWindowID(m_winID);
			m_SRBtn[SRBtnSize]->UpdateUI(m_iCurSceneID, pConfig.InterfaceSetData[i]->nIndex, m_hROI);
			m_height = pConfig.InterfaceSetData[i]->height + rtS.bottom;
			++m_index;
		}
	}
	m_initControl[m_iCurSceneID] = false;
}


void CSceneDebugDlg::OnBnClickedCheckIsSimulate()
{
    // TODO: 在此添加控件通知处理程序代码

    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);
    if (((CButton *)GetDlgItem(IDC_CHECK_IS_SIMULATE))->GetCheck())
    {
        config.bIsSimulate = true;
    }
    else
    {
        config.bIsSimulate = false;
    }
    SetSceneParam(&config);
}
DWORD WINAPI CSceneDebugDlg::CalGrabThread(void* pThis)
{
	CSceneDebugDlg* pSC = (CSceneDebugDlg*)pThis;
	 

	return 0;
}

void CSceneDebugDlg::OnBnClickedButtonloop()
{
	// TODO: 在此添加控件通知处理程序代码

	//CreateThread(NULL,0,CalGrabThread,this,0,NULL);

	bool res = false;
	SCENE_PKG *config = NULL;
	config = (SCENE_PKG *)GetSceneParamHandle(m_iCurSceneID);

	CString str;
	GetDlgItem(IDC_BUTTON_loop)->GetWindowText(str);
	if (0 == str.Compare("结束匹配"))
	{
		str = _T("开始匹配");
		GetDlgItem(IDC_BUTTON_loop)->SetWindowText(str);
		m_loop = false;
	}
	else
	{
		str = _T("结束匹配");
		GetDlgItem(IDC_BUTTON_loop)->SetWindowText(str);
		m_loop = true;	
		_beginthreadex(NULL,0,(unsigned (__stdcall *)(void*))CSceneDebugDlg::LoopSenceTest,(void *)this,0,NULL);
	

	}
	return;



	//下面是循环测试的
//	CString str;
	GetDlgItem(IDC_BUTTON_loop)->GetWindowTextA(str);
	GetDlgItem(IDC_EDIT_TIME)->GetWindowText(m_strTemp);
	m_time = atoi(m_strTemp.GetBuffer());

	if (0 == str.Compare("结束测试"))
	{
		str = _T("开始测试");
		GetDlgItem(IDC_BUTTON_loop)->SetWindowTextA(str);
		m_loop = false;
	}
	else
	{
		str = _T("结束测试");
		GetDlgItem(IDC_BUTTON_loop)->SetWindowTextA(str);
		m_loop = true;		
		_beginthreadex(NULL,0,(unsigned (__stdcall *)(void*))CSceneDebugDlg::LoopSenceTest,(void *)this,0,NULL);
	}
	
}

void  CSceneDebugDlg::DispLoolCount()
{
		CString str;
		str.Format(_T("第%d次"),m_loopcount[m_iCurSceneID]);
		GetDlgItem(IDC_STATIC_loop)->SetWindowTextA(str);
}

void CSceneDebugDlg::LoopSenceTest(void * param)
{
	CSceneDebugDlg* lthis = (CSceneDebugDlg*)param;
	bool res = false;
	while(lthis->m_loop)
	{
		//老测试方法
		//lthis->m_loopcount[lthis->m_iCurSceneID]++;
		////TriggerVisionProcess(lthis->m_iCurSceneID,1);
		//GrabSceneimage(lthis->m_iCurSceneID,lthis->m_iCurIndex);//这是老的循环测试按钮，调用的是老的采集函数		
		//VisionProcess(lthis->m_iCurSceneID,lthis->m_iCurIndex,lthis->m_strsult);
		//DispImage(lthis->m_winID, lthis->m_iCurSceneID, lthis->m_iCurIndex);
		//DispObj(lthis->m_winID,lthis->m_iCurSceneID,true);
		//Sleep(lthis->m_time);
		//lthis->DispLoolCount();

		//新的测试
		if ( 0 == m_pInstance->m_ProcProcess)
		{
			//使用图像采集对象 来采集图像
			ProcList* pProcList = NULL;
			VisionProcBase* pVisionProc = NULL;
			pProcList = g_SceneList.GetSceneBySceneID(m_pInstance->m_iCurSceneID);
			pVisionProc = pProcList->GetProcByTypeIndex(IMAGE_ACQISITION,m_pInstance->m_iCurIndex);
			if (pVisionProc)
			{
				res = ((SceneAcqImage*)pVisionProc)->ProcGrabImage(false);
			}

		}
		long HalWinId = 0;
		ProcList* pScene = g_SceneList.GetSceneBySceneID(m_pInstance->m_iCurSceneID);
		FindHalconWinID(g_SceneDlgHwinId,HalWinId);
		//int ImageIndex = g_SceneList.GetSceneBySceneID(m_iCurSceneID)->GetProcImageIndex(GetProcId());
		int ImageIndex = m_pInstance->m_iCurIndex;
		SceneMatchModel* mod = (SceneMatchModel*) pScene->GetProcByTypeIndex(MODEL_PRO,0);//获取第一个模板处理子类
		if (NULL != mod)
		{	
			char* m_ModelName = mod->m_ModelName;
			int PyramidLevel = mod->m_SceneModelSetting.m_BuildLevel;
			int MatchNum = 1;
			double  Score = mod->m_SceneModelSetting.m_MatchMinScore;
			double row =0.0,col = 0.0,phi = 0.0;
			bool IsBulid = false;//无用
			int ProcIndex  = mod->GetProcTypeIndex();
			//显示后再加上模板检测及十字叉显示中心
			SCENE_PKG *pConfig = (SCENE_PKG *)GetSceneParamHandle(m_pInstance->m_iCurSceneID);
			int falg = pConfig->bSave;//先用保存模拟下
			FindModelRealTime(m_pInstance->m_iCurSceneID,HalWinId,ImageIndex,m_ModelName,PyramidLevel,MatchNum,Score,falg,IsBulid,row,col,phi);

			//Sleep(lthis->m_time);


		}


	}
	
}


void CSceneDebugDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_loop)
	{
		m_loop = false;
		Sleep(3000);
	}
	CDialogEx::OnClose();
}


void CSceneDebugDlg::OnDestroy()
{	
	if(m_loop)
	{
		m_loop = false;
		Sleep(2000);
	}
	
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CSceneDebugDlg::OnCbnSelchangeScenedlgRelasceneno()
{
	// TODO: Add your control notification handler code here
	DWORD RelativeId =  m_CB_RelaSceneNo.GetItemData(m_CB_RelaSceneNo.GetCurSel());
	ProcList* pProcList = NULL;
	pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
	if (pProcList)
	{
		pProcList->SetRelaSceneID(RelativeId);
	}

}


void CSceneDebugDlg::OnCbnSelchangeScenedlgNextsceneno()
{
	// TODO: Add your control notification handler code here
	DWORD NextId =  m_CB_NextSceneId.GetItemData(m_CB_NextSceneId.GetCurSel());
	ProcList* pProcList = NULL;
	pProcList = g_SceneList.GetSceneBySceneID(m_iCurSceneID);
	if (pProcList)
	{
		pProcList->SetNextSceneID(NextId);
	}


}
