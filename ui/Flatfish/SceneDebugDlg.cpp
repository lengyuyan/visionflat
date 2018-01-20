// SceneDebugDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FlatFish.h"
#include "SceneDebugDlg.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"
#include "SetSceneDlg.h"

// CSceneDebugDlg 对话框

IMPLEMENT_DYNAMIC(CSceneDebugDlg, CDialogEx)

CSceneDebugDlg *CSceneDebugDlg::m_pInstance = NULL;

CSceneDebugDlg::CSceneDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneDebugDlg::IDD, pParent)
{
    m_iCurSceneID = 0;
    m_iCurIndex = 0;
    m_hROI=NULL;
    m_bIsContinousGrab = false;
	m_index = 0;
    m_iCurGroup = 0;
	m_sceneChange = false;
	m_initControl.resize(4, true);
	m_height = 0;
	m_iT = 0;
}

CSceneDebugDlg::~CSceneDebugDlg()
{
	m_SEBtn.clear();
	m_SRBtn.clear();
}

void CSceneDebugDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_FUNCTION, m_listFunction);
    DDX_Control(pDX,IDC_LIST_PIC,m_listPicture[0]);
    DDX_Control(pDX,IDC_LIST_PIC1,m_listPicture[1]);
    DDX_Control(pDX,IDC_LIST_PIC2,m_listPicture[2]);
    DDX_Control(pDX,IDC_LIST_PIC3,m_listPicture[3]);
    //DDX_Control(pDX,IDC_STATIC_SCNE,m_static_scene);
    DDX_Control(pDX,IDC_CHECK_PIC,m_checkbox);
}


BEGIN_MESSAGE_MAP(CSceneDebugDlg, CDialogEx)

    //自定义消息
    ON_MESSAGE(VISION_MANUAL_MSG, &CSceneDebugDlg::OnManualMessage)
	ON_MESSAGE(SET_SCENE, &CSceneDebugDlg::OnSetScene)
    ON_MESSAGE(ADD_FUNCTION_MSG, &CSceneDebugDlg::OnAddFunction)
    
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
    ON_NOTIFY(NM_CLICK, IDC_LIST_FUNCTION, &CSceneDebugDlg::OnClickListFunction)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_FUNCTION, &CSceneDebugDlg::OnDblclkListFunction)
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
END_MESSAGE_MAP()


BOOL CSceneDebugDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动

    register_notify(MODULE_SCENE_DEBUG_UI, OnRemoteMessage);
    SetBackgroundColor(BACKGROUND_COLOR_M);
    m_pInstance = this;
    // TODO: 在此添加额外的初始化代码
    init_scene_info();  //初始化UI
    update_list_info();
    GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("打开实时显示");
    
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSceneDebugDlg::init_scene_info()
{
	char szBuf[256] = {0};
	SCENE_PKG config;  //从配置文件中读取图片有关roi信息
	GetSceneParam(m_iCurSceneID,&config);

    m_iCurSceneID = 0;
    m_iCurIndex = 0;
    m_hROI=NULL;

    m_winID = (long)GetDlgItem(IDC_STATIC_BIND)->m_hWnd;
    CRect rt;
    GetDlgItem(IDC_STATIC_BIND)->GetClientRect(&rt);
    BindWindow(m_winID,rt.left,rt.top, rt.Width(), rt.Height(), true);
    m_hROI=GetDlgItem(IDC_STATIC_BIND);

    InitGrabSceneimage(m_iCurSceneID);
    ClearWindow(m_winID);
    RestoreImagePart(m_winID, m_iCurSceneID,rt.Width(), rt.Height());
    DispImage(m_winID,m_iCurSceneID);

    CWnd *pWnd = GetDlgItem(IDC_STATIC_SETTING);
    m_setScene.Create(IDD_DLG_SET_SCENE, this);
    //m_setPicindex.Create(IDD_DIG_SET_PICINDEX, this);
    m_SelectFunction.Create(IDD_DIALOG_FUN_SELECT, this);
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

    CString str = config.szSceneName;
    /*m_static_scene.*/SetWindowText(str);
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

    if (config.bIsAffine)
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(1);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(0);
    }
}

void CSceneDebugDlg::OnBnClickedBtnCrabpic()
{
    m_bIsContinousGrab = true;
    OnBnClickedBtnOpenShow();
    GrabSceneimage(m_iCurSceneID,m_iCurIndex);
    DispImage(m_winID, m_iCurSceneID, m_iCurIndex);
}

int WINAPI CSceneDebugDlg::OnRemoteMessage( unsigned int wParam, void * lParam, SYSTEM_MSG_TYPE msgType)
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
            DispImage(m_pInstance->m_winID, m_pInstance->m_iCurSceneID, m_pInstance->m_iCurIndex);
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
        ColseContinousGrab(m_iCurSceneID, m_iCurIndex);
        GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("打开实时显示");
    }
    else
    {
        m_bIsContinousGrab = true;
	    OpenContinousGrab((long)this->GetSafeHwnd(), m_iCurSceneID, m_iCurIndex, VISION_MANUAL_MSG);
        GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("关闭实时显示");
    }
}


//void CSceneDebugDlg::OnBnClickedBtnCloseShow()
//{
//	ColseContinousGrab(m_iCurSceneID, m_iCurIndex);
//}


void CSceneDebugDlg::OnBnClickedBtnSaveoripic()//保存原始图片
{
	SCENE_PKG congfig;
    GetSceneParam(m_iCurSceneID,&congfig);
	SaveImage(m_iCurSceneID, m_iCurIndex);
	
}


void CSceneDebugDlg::OnBnClickedBtnReultpic()//保存结果图片
{
	SCENE_PKG congfig;
	GetSceneParam(m_iCurSceneID,&congfig);
	SaveImageDump(m_winID,m_iCurSceneID);
	
}
void CSceneDebugDlg::updata_ui_data(int iType, int index)
{
    char szBuf[256] = {0};
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    GetSceneParam(m_iCurSceneID,&config);
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
void CSceneDebugDlg::update_list_info()
{
    SCENE_PKG config;
    GetSceneParam(m_iCurSceneID, &config);

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
}

LRESULT CSceneDebugDlg::OnSetScene( WPARAM wParam, LPARAM lParam )//场景改变消息
{
    SYSTEM_PARAM sysParam;
    GetSysParam(&sysParam);

    if (m_bIsContinousGrab)
    {
        m_bIsContinousGrab = false;
        ColseContinousGrab(m_iCurSceneID, m_iCurIndex);
        GetDlgItem(IDC_BTN_OPEN_SHOW)->SetWindowText("打开实时显示");
    }
    if (1 == ((CButton *)GetDlgItem(IDC_CHECK_IS_SHOW_TMP))->GetCheck())
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_SHOW_TMP))->SetCheck(0);
        OnBnClickedCheckIsShowTmp();
    }

	m_sceneChange = true;
	m_iCurSceneID = lParam;//atoi(m_strTemp.Right(1))-1;
	/*m_static_scene.*/SetWindowText(sysParam.pstrSceneName[m_iCurSceneID].c_str());
    
    ShowFileListCtrl(0);
	SCENE_PKG config;
	GetSceneParam(m_iCurSceneID,&config);

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
    InitGrabSceneimage(m_iCurSceneID,m_iCurIndex);
    ClearWindow(m_winID);  //防止图像大小不一至导致窗口显示信息混乱
    RestoreImagePart((long)this->m_hWnd,m_iCurIndex,rt.Width(), rt.Height());
    if (config.bIsAffine)
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(1);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CHECK_IS_AFFINE))->SetCheck(0);
    }
	DispImage(m_winID,m_iCurSceneID);
    update_list_info();
    LRESULT Result;
    OnClickListFunction(NULL, &Result);
    //OnClickListFunction();
    //OnBnClickedButtonFindLine();
	return 1;
}

LRESULT CSceneDebugDlg::OnAddFunction( WPARAM wParam, LPARAM lParam )//
{
    int iType = (int)lParam;
    int iSel = m_listFunction.GetSelectionMark();
    int iCount = m_listFunction.GetItemCount();
    int iValue = 0;
    SCENE_PKG config;
    int          iCurSize = 0;
    int    insertPos = 0;
    if (IDNO == AfxMessageBox("确认添加", MB_YESNO) || (iSel > m_vecListInfo.size() && MAIN_PRO != iType))
    {
        return -1;
    }
    GetSceneParam(m_iCurSceneID, &config);
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
                iTD = m_vecListInfo[iSel]%10000;
                //判断是否在图像前面添加
                if (MAIN_PRO != iTD/100)
                {
                    AfxMessageBox("请在图像输入前新增图像输入");
                    return -1;
                }
                //前面保证了iSel + 1 有效
                iCurImage = iTD%100;
                int iPos = IFindParamData(&config, MAIN_PRO, iCurImage);
                insertPos = iPos;
            }
            //最后一个
            else if (iSel == m_vecListInfo.size() - 1)
            {
                if (config.imageCount >= m_vecListInfo.size())
                {
                    config.imageCount = m_vecListInfo.size();
                }
                iCurImage = config.imageCount;
                insertPos = config.pkgCount;
            }
            else
            {
                return -1;
            }

            int iT = iCurImage * 10000 + iType * 100 + iCurImage;
            VIRSION_DATA_PKG pInfo[2];
            pInfo[0].iValue = iT;
            pInfo[1].iValue = 0;
            unsigned int pType[2] = {0};
            //向场景信息中添加图像参数
            InsertParamItem(m_iCurSceneID, pInfo, pType, 2, insertPos);

            //插入容器
            iValue = iGroup * 10000 + MAIN_PRO * 100 + iCurImage;
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
            //if (iCurImage != config.imageCount)
            //{
            //    m_vecListInfo[iSel + 1]++;
            //}
        }
        break;
    case MODEL_PRO:
        {
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
            int iT = iCurImage * 10000 + iType * 100 + iCurMode;
            unsigned int iFlag = ROI_TYPE * 100 + 1;
            int iModelStrSize = 11;
            int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iModelStrSize;
            VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
            unsigned int *pType = new unsigned int[iSize];
            ST_MODEL_INFO stInfo;
            memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
            memset(pType, 0, iSize * sizeof(unsigned int));
            pInfo[0].iValue = iT;
            pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;
            memcpy((pInfo + VIRSION_DATA_PKG_HEADER_SIZE), &stInfo, sizeof(ST_MODEL_INFO));
            memcpy(&stInfo, (pInfo + VIRSION_DATA_PKG_HEADER_SIZE), sizeof(ST_MODEL_INFO));
            int iV = sizeof(ST_RECTANGLE_DATA)/sizeof(double) + 3;  //三个记录模板查找结果
            for (int i = 0; i < iV; i++)
            {
                pType[VIRSION_DATA_PKG_HEADER_SIZE + i] = iFlag;
            }
            pType[VIRSION_DATA_PKG_HEADER_SIZE + iV] = ID_TYPE * 100 + 0;
            pType[VIRSION_DATA_PKG_HEADER_SIZE + iV + 1] = 1;   //匹配分数

            //向场景信息中添加图像参数
            InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

            iValue = MODEL_PRO * 100 + iCurMode;
            vector<unsigned int>::iterator itML = m_vecListInfo.begin() + iSel + 1;
            m_vecListInfo.insert(itML, 1, iValue);
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
            int iT = iGroup * 10000 + iType * 100 + iCurIndex;
            unsigned int iFlag = THRE_TYPE * 100 + 0;
            int iSize = VIRSION_DATA_PKG_HEADER_SIZE + 1;
            VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
            unsigned int *pType = new unsigned int[iSize];
            memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
            memset(pType, 0, iSize * sizeof(unsigned int));
            pInfo[0].iValue = iT;
            pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;
            pInfo[2].iValue = 0;
            pType[2] = iFlag;
            //向场景信息中添加图像参数
            InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

            iValue = iGroup*10000 + iType * 100 + iCurIndex;
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
                }
                insertPos++;
            }
            int iT = iGroup * 10000 + iType * 100 + iCurIndex;
            unsigned int iFlag = ROI_TYPE * 100 + 1;
            int iLSize = 13;
            int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iLSize;
            ST_FIND_LINE stInfo;
            VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
            unsigned int *pType = new unsigned int[iSize];
            memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
            memset(pType, 0, iSize * sizeof(unsigned int));
            pInfo[0].iValue = iT;
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

            iValue = iGroup*10000 + iType * 100 + iCurIndex;
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

            int iT = iGroup * 10000 + iType * 100 + iCurIndex;
            unsigned int iFlag = ROI_TYPE * 100 + 1;
            int iCSize = 14;
            int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iCSize;
            ST_FIND_CIRCLE stInfo;
            VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
            unsigned int *pType = new unsigned int[iSize];
            memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
            memset(pType, 0, iSize * sizeof(unsigned int));
            pInfo[0].iValue = iT;
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

            iValue = iGroup * 10000 + iType * 100 + iCurIndex;
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

			int iT = iGroup * 10000 + iType * 100 + iCurIndex;
			unsigned int iFlag = ROI_TYPE * 100 + 1;
			int iLSize = 0;
			int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iLSize;
			ST_FIND_LINE stInfo;
			VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
			unsigned int *pType = new unsigned int[iSize];
			memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
			memset(pType, 0, iSize * sizeof(unsigned int));
			pInfo[0].iValue = iT;
			pInfo[1].iValue = iSize - VIRSION_DATA_PKG_HEADER_SIZE;

			//初始化参数
			//memcpy((pInfo + VIRSION_DATA_PKG_HEADER_SIZE), &stInfo, sizeof(ST_RECTANGLE_DATA));

			//向场景信息中添加图像参数
			InsertParamItem(m_iCurSceneID, pInfo, pType, iSize, insertPos);

			iValue = iGroup * 10000 + iType * 100 + iCurIndex;
			vector<unsigned int>::iterator itCL = m_vecListInfo.begin() + iSel + 1;
			m_vecListInfo.insert(itCL, 1, iValue);
		}
		break;
    default:
        break;
    }
    //交换后面数据
    iSel++;
    for (int index = iSel+1; index < m_vecListInfo.size(); index++)
    {
        if (iType == m_vecListInfo[index]/100)
        {
            m_vecListInfo[index]++;
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
        int i = pMsg->lParam;
        break;
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSceneDebugDlg::OnBnClickedButton12()//整体验证
{
    SetDebugWindowID(m_iCurSceneID, m_winID);
	VisionProcess(m_iCurSceneID,m_iCurIndex,m_strsult);
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
    GetCurrentDirectory(512, szCurrentPath);
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

    strcpy(szFind,lpPath);
    strcat(szFind,"//*.*");

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
		sprintf(buf,"%d",i+1);
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

void CSceneDebugDlg::OnTimer(UINT nIDEvent)
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
			m_listPicture[m_iCurGroup].SetItemText(m_iTestImageIndex,2,"OK");
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


void CSceneDebugDlg::OnClickListFunction(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
	SCENE_PKG pConfig;				//从配置文件中读取图片有关roi信息
	GetSceneParam(m_iCurSceneID, &pConfig);
	int tmp_iType;
	int iSEBtn = m_SEBtn.size();
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
                    DispROI(m_winID,m_iCurSceneID,index,LINE_PRO);
                    m_SceneFindLine.ShowWindow(SW_SHOW);
                    m_SceneFindLine.UpdateUI(m_iCurSceneID, index, m_hROI);
                }
                break;
            case CIRCLE_PRO:
                {
                    DispROI(m_winID,m_iCurSceneID,index,CIRCLE_PRO);
                    m_SceneFindCircle.ShowWindow(SW_SHOW);
                    m_SceneFindCircle.UpdateUI(m_iCurSceneID, index ,m_hROI);
                }
				break;
			case MORE_PRO:
				{
					style = GetWindowLong(m_listFunction.GetSafeHwnd(), GWL_STYLE);
					style = style | WS_CLIPSIBLINGS ;
					SetWindowLong(m_listFunction.GetSafeHwnd(), GWL_STYLE, style);

					//显示控件
					GetDlgItem(IDC_BUTTON_MORE_CONTTROL)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_BUTTON_DELETE_CONTTROL)->ShowWindow(SW_SHOW);

					DrawControl();	//当前场景第一次打开更多是读取数据

					if (!m_initControl[m_iCurSceneID])
					{
						for (int i = 0; i < m_interfaceSetData.size(); ++i)
						{
							if (i < m_SEBtn.size())
							{
								if (!strcmp("SEBtn", m_SEBtn[i]->BtnType().c_str())
									&& m_iCurSceneID == m_SEBtn[i]->CurSceneID())
								{
									m_SEBtn[i]->ShowWindow(SW_SHOW);
								}
							}
							if (i < m_SRBtn.size())
							{
								if (!strcmp("SRBtn", m_SRBtn[i]->BtnType().c_str())
									&& m_iCurSceneID == m_SRBtn[i]->CurSceneID())
								{
									m_SRBtn[i]->ShowWindow(SW_SHOW);
								}
							}
						}
					}
				}
                break;
            }
            updata_ui_data();
        }
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
    ReadImage(m_strTemp.GetBuffer(),m_iCurSceneID, m_iCurGroup);
    DispImage(m_winID,m_iCurSceneID, m_iCurGroup);
    
    *pResult = 0;
}

void CSceneDebugDlg::OnDblclkListFunction(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
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
    *pResult = 0;
}


void CSceneDebugDlg::OnBnClickedBtnSaveParam()
{
    // TODO: 在此添加控件通知处理程序代码
    if (IDYES == AfxMessageBox("保存修改", MB_YESNO))
    {
        SaveSceneParam(m_iCurSceneID);
    }
}


void CSceneDebugDlg::OnBnClickedButtonAddFunction()
{
    // TODO: 在此添加控件通知处理程序代码
    CPoint pt;
    CRect rt;
    GetCursorPos(&pt);
    CSceneDebugDlg *pTmp = this;
    HWND hwd = pTmp->m_hWnd;
    m_SelectFunction.GetClientRect(rt);
    m_SelectFunction.SetWindowPos(NULL,pt.x,pt.y - rt.Height(),rt.Width(),rt.Height(),SWP_NOSIZE);
    m_SelectFunction.UpdateUI(m_iCurSceneID, m_iCurIndex);
    m_SelectFunction.ShowWindow(SW_SHOW);
    m_SelectFunction.SetFocus();
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
        if (iType == m_vecListInfo[index]/100)
        {
            m_vecListInfo[index]--;
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
        config.bIsAffine = true;
    }
    else
    {
        config.bIsAffine = false;
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
        ColseContinousGrab(m_iCurSceneID, m_iCurIndex);
    }
    unregister_notify(MODULE_SCENE_DEBUG_UI);
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

			nIndex = m_SEBtn.size();
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

			nIndex = m_SRBtn.size();
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
		m_index = m_interfaceSetData.size();
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

	nIndex = m_interfaceSetData.size();

	//读取数据
	for (int i = 0; i < pConfig.InterfaceDataNumb; ++i)
	{
		m_interfaceSetData.push_back(*pConfig.InterfaceSetData[i]);
		if (!strcmp("SEBtn", m_interfaceSetData[i].BtnType.c_str())
			&& m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
		{
			SEBtnSize = m_SEBtn.size();
			m_SEBtn.resize(SEBtnSize + 1);
			m_SEBtn[SEBtnSize] = new CSEBtn;
			m_SEBtn[SEBtnSize]->Create(IDD_DIALOG_SEBTN, pWnd);
			m_SEBtn[SEBtnSize]->GetClientRect(&rtS);
			m_SEBtn[SEBtnSize]->SetWindowPos(&wndTop, rtS.left, m_interfaceSetData[m_index].height, rtS.right, rtS.bottom, 
				SWP_SHOWWINDOW);
			m_SEBtn[SEBtnSize]->getInterfaceSetData(m_interfaceSetData[m_index]);
			m_SEBtn[SEBtnSize]->ShowWindow(SW_SHOW);
			m_SEBtn[SEBtnSize]->SetDisplayWindowID(m_winID);
			m_SEBtn[SEBtnSize]->UpdateUI(m_iCurSceneID, m_interfaceSetData[m_index].nIndex, m_hROI);
			m_height = m_interfaceSetData[m_index].height + rtS.bottom;
			++m_index;
		}
		else if (!strcmp("SRBtn", m_interfaceSetData[i].BtnType.c_str())
			&& m_iCurSceneID == m_interfaceSetData[i].CurSceneID)
		{
			SRBtnSize = m_SRBtn.size();
			m_SRBtn.resize(SRBtnSize + 1);
			m_SRBtn[SRBtnSize] = new CSRBtn;
			m_SRBtn[SRBtnSize]->Create(IDD_DIALOG_SRBTN, pWnd);
			m_SRBtn[SRBtnSize]->GetClientRect(&rtS);
			m_SRBtn[SRBtnSize]->SetWindowPos(&wndTop, rtS.left, m_interfaceSetData[m_index].height, rtS.right, rtS.bottom, 
				SWP_SHOWWINDOW);
			m_SRBtn[SRBtnSize]->getInterfaceSetData(m_interfaceSetData[m_index]);
			m_SRBtn[SRBtnSize]->ShowWindow(SW_SHOW);
			m_SRBtn[SRBtnSize]->SetDisplayWindowID(m_winID);
			m_SRBtn[SRBtnSize]->UpdateUI(m_iCurSceneID, m_interfaceSetData[m_index].nIndex, m_hROI);
			m_height = m_interfaceSetData[m_index].height + rtS.bottom;
			++m_index;
		}
	}
	m_initControl[m_iCurSceneID] = false;
}
