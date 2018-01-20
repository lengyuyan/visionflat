// SysCamCalConfig.cpp : implementation file
//

#include "stdafx.h"
#include "SystemManager.h"
#include "SysCamCalConfig.h"
#include "SystemManagerDlg.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"

// SysCamCalConfig dialog

IMPLEMENT_DYNAMIC(SysCamCalConfig, CDialogEx)

SysCamCalConfig::SysCamCalConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(SysCamCalConfig::IDD, pParent)
{
	 m_RotToCamX = 0.0;
	 m_RotToCamY = 0.0;
	 m_AxeRotDirect = 1;
}

SysCamCalConfig::~SysCamCalConfig()
{
}

void SysCamCalConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMCAL_MOVESET, m_CalMoveConfig);
	//DDX_Control(pDX, IDC_CAMCAL_CAMINDEX, m_ECamIndex);
	//DDX_Control(pDX, IDC_CAMCAL_AXIINDEX, m_EAxiIndex);
	DDX_Control(pDX, IDC_CAMCAL_TOOLORI_X, m_ED_ToolOriX);
	DDX_Control(pDX, IDC_CAMCAL_TOOLORI_Y, m_ED_ToolOriY);
	DDX_Control(pDX, IDC_CAMCAL_CAMORI_X, m_ED_CamOriX);
	DDX_Control(pDX, IDC_CAMCAL_CAMORI_Y, m_ED_CamOriY);
	DDX_Control(pDX, IDC_CAMCAL_CAMFIXEDTYPE, m_COB_CamFixType);
	//DDX_Control(pDX, IDC_CAMCAL_TOOLINDEX2, m_ED_ToolIndex);
	DDX_Control(pDX, IDC_CAMCAL_CAMORI_ANGLE, m_ED_CamOriPhi);
	DDX_Control(pDX, IDC_CAMCAL_SELCAMINDEX, m_SelCamIndex);
	DDX_Control(pDX, IDC_CAMCAL_SELAXEINDEX, m_SelAxeIndex);
	DDX_Control(pDX, IDC_CAMCAL_SELTOOLINDEX, m_SelToolIndex);
	DDX_Control(pDX, IDC_CAMCAL_RESTYPE, m_SelMoveResType);
	DDX_Control(pDX, IDC_CAMCAL_TOOLORI_ANGLE, m_ED_ToolOriAngle);
	DDX_Control(pDX, IDC_CAMCAL_ROTTOCAM_X, m_ED_RotToCamX);
	DDX_Control(pDX, IDC_CAMCAL_ROTTOCAM_Y, m_ED_RotToCamY);
	DDX_Control(pDX, IDC_CAMCAL_AXEROTDIRECT, m_CB_AxeRotDirect);
	DDX_Control(pDX, IDC_CK_USECAMPARAM, m_CK_ParamMod);
	DDX_Control(pDX, IDC_ED_CALTHICKNESS, m_ED_CalThickness);
}


BEGIN_MESSAGE_MAP(SysCamCalConfig, CDialogEx)
	
	ON_BN_CLICKED(IDC_SYSCAMCAL_SHOWDATA, &SysCamCalConfig::OnBnClickedSyscamcalShowdata)
	ON_EN_CHANGE(IDC_CAMCAL_CAMINDEX, &SysCamCalConfig::OnEnChangeCamcalCamindex)
	ON_EN_CHANGE(IDC_CAMCAL_AXIINDEX, &SysCamCalConfig::OnEnChangeCamcalAxiindex)
	ON_BN_CLICKED(IDC_CAMCAL_SAVE, &SysCamCalConfig::OnBnClickedCamcalSave)
	ON_CBN_SELCHANGE(IDC_CAMCAL_CAMFIXEDTYPE, &SysCamCalConfig::OnCbnSelchangeCamcalCamfixedtype)
	ON_CBN_SELCHANGE(IDC_CAMCAL_SELCAMINDEX, &SysCamCalConfig::OnCbnSelchangeCamcalSelcamindex)
	ON_CBN_SELCHANGE(IDC_CAMCAL_SELAXEINDEX, &SysCamCalConfig::OnCbnSelchangeCamcalSelaxeindex)
	ON_CBN_SELCHANGE(IDC_CAMCAL_SELTOOLINDEX, &SysCamCalConfig::OnCbnSelchangeCamcalSeltoolindex)
	ON_CBN_SELCHANGE(IDC_CAMCAL_RESTYPE, &SysCamCalConfig::OnCbnSelchangeCamcalRestype)
	ON_CBN_SELCHANGE(IDC_CAMCAL_AXEROTDIRECT, &SysCamCalConfig::OnCbnSelchangeCamcalAxerotdirect)
	ON_BN_CLICKED(IDC_CK_USECAMPARAM, &SysCamCalConfig::OnBnClickedCkUsecamparam)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CAMCAL_MOVESET, &SysCamCalConfig::OnLvnItemchangedCamcalMoveset)
	ON_BN_CLICKED(IDC_ADD_MOVATION, &SysCamCalConfig::OnBnClickedAddMovation)
	ON_BN_CLICKED(IDC_DEL_MOVATION, &SysCamCalConfig::OnBnClickedDelMovation)
	ON_NOTIFY(NM_DBLCLK, IDC_CAMCAL_MOVESET, &SysCamCalConfig::OnNMDblclkCamcalMoveset)
	ON_NOTIFY(NM_CLICK, IDC_CAMCAL_MOVESET, &SysCamCalConfig::OnNMClickCamcalMoveset)
END_MESSAGE_MAP()


// SysCamCalConfig message handlers

BOOL  SysCamCalConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	char szText[256] = {0};
	CString strTemp;

	//标定的移动设定
	DWORD listStyle = m_CalMoveConfig.GetExStyle();
	listStyle = listStyle|LVS_EX_GRIDLINES;
	listStyle = listStyle|LVS_EX_FULLROWSELECT;
	m_CalMoveConfig.SetExtendedStyle(listStyle);

	RECT ListRect;
	int margin = 30;
	m_CalMoveConfig.GetWindowRect(&ListRect);
	m_CalMoveConfig.InsertColumn(0,"点序号");
	m_CalMoveConfig.SetColumnWidth(0,(ListRect.right-margin)/4-30);
	m_CalMoveConfig.InsertColumn(1,"相对移动X");
	m_CalMoveConfig.SetColumnWidth(1,(ListRect.right-margin)/4);

	m_CalMoveConfig.InsertColumn(2,"相对移动Y");
	m_CalMoveConfig.SetColumnWidth(2,(ListRect.right-margin)/4);

	m_CalMoveConfig.InsertColumn(3,"相对转动");
	m_CalMoveConfig.SetColumnWidth(3,(ListRect.right-margin)/4);




	//其他设定
	m_COB_CamFixType.InsertString(0,"相机固定");
	m_COB_CamFixType.SetItemData(0,SYSTEM_PARAM::FIXED);

	m_COB_CamFixType.InsertString(1,"相机移动");
	m_COB_CamFixType.SetItemData(1,SYSTEM_PARAM::UNFIXED);

	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	for (int CamIndex = 0;CamIndex<pSysParam->iCameraNum ;CamIndex++)
	{
		strTemp.Format("%d",CamIndex);
		m_SelCamIndex.InsertString(CamIndex,strTemp);
		m_SelCamIndex.SetItemData(CamIndex,CamIndex);
	}
	m_SelCamIndex.SetCurSel(0);
	m_CamIndex = m_SelCamIndex.GetItemData(m_SelCamIndex.GetCurSel());

	for (int AxeIndex = 0;AxeIndex<pSysParam->iAxisNum ;AxeIndex++)
	{
		strTemp.Format("%d",AxeIndex);
		m_SelAxeIndex.InsertString(AxeIndex,strTemp);
		m_SelAxeIndex.SetItemData(AxeIndex,AxeIndex);
	}
	m_SelAxeIndex.SetCurSel(0);
	m_AxiIndex = m_SelAxeIndex.GetItemData(m_SelAxeIndex.GetCurSel());


	for (int ToolIndex = 0;ToolIndex<pSysParam->iToolNum ;ToolIndex++)
	{
		strTemp.Format("%d",ToolIndex);
		m_SelToolIndex.InsertString(ToolIndex,strTemp);
		m_SelToolIndex.SetItemData(ToolIndex,ToolIndex);
	}
	m_SelToolIndex.SetCurSel(0);
	m_ToolIndex = m_SelToolIndex.GetItemData(m_SelToolIndex.GetCurSel());

     //定位匹配需要的结果类型
	m_SelMoveResType.InsertString(0,"绝对坐标");
	m_SelMoveResType.SetItemData(0,0);
	m_SelMoveResType.InsertString(1,"旋转后绝对坐标");
	m_SelMoveResType.SetItemData(1,1);

	//轴的旋转方向
	m_CB_AxeRotDirect.InsertString(0,"方向相同");
	m_CB_AxeRotDirect.SetItemData(0,0);
	m_CB_AxeRotDirect.InsertString(1,"方向相反");
	m_CB_AxeRotDirect.SetItemData(1,1);


	//m_AxeRotDirect
	m_CK_ParamMod.SetCheck(0);


	OnBnClickedSyscamcalShowdata();

	pCommonEdit = NULL;

	return TRUE;
}

void SysCamCalConfig::OnBnClickedSyscamcalShowdata()
{

	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	CString strTemp;
	char szText[256] = {0};
	if (m_CamIndex<0||m_AxiIndex<0||m_ToolIndex<0)
	{
		AfxMessageBox("机械序号选择错误");
		return ;
	}

	m_PosMoveType = pSysParam->PosMoveType[m_CamIndex][m_AxiIndex][m_ToolIndex];

	m_CamFixedType = pSysParam->CamFixedType[m_CamIndex]; //相机的安装方式
	m_CurToolCalOriX = pSysParam->ToolCalOriX[m_CamIndex][m_AxiIndex][m_ToolIndex];
	m_CurToolCalOriY = pSysParam->ToolCalOriY[m_CamIndex][m_AxiIndex][m_ToolIndex];
	m_CurToolCalOriAngle = pSysParam->ToolCalOriPhi[m_CamIndex][m_AxiIndex][m_ToolIndex];
	m_CurCamCalOriX = pSysParam->CamCalOriX[m_CamIndex][m_AxiIndex][m_ToolIndex];
	m_CurCamCalOriY = pSysParam->CamCalOriY[m_CamIndex][m_AxiIndex][m_ToolIndex];
	m_CurCamCalOriPhi = pSysParam->CamCalOriPhi[m_CamIndex][m_AxiIndex][m_ToolIndex];
	m_RotToCamX = pSysParam->RotpointToCamX[m_CamIndex][m_AxiIndex][m_ToolIndex];
	m_RotToCamY = pSysParam->RotpointToCamY[m_CamIndex][m_AxiIndex][m_ToolIndex];

	pSysParam->AxeRotDirect[m_CamIndex][m_AxiIndex][m_ToolIndex]?m_AxeRotDirect=1:m_AxeRotDirect=0;


	strTemp.Format("%0.12lf",m_CurToolCalOriX);
	m_ED_ToolOriX.SetWindowTextA(strTemp);
	strTemp.Format("%0.12lf",m_CurToolCalOriY);
    m_ED_ToolOriY.SetWindowTextA(strTemp);
	strTemp.Format("%0.12lf",m_CurToolCalOriAngle);
	m_ED_ToolOriAngle.SetWindowTextA(strTemp);
	strTemp.Format("%0.12lf",m_CurCamCalOriX);
	m_ED_CamOriX.SetWindowTextA(strTemp);
	strTemp.Format("%0.12lf",m_CurCamCalOriY);
	m_ED_CamOriY.SetWindowTextA(strTemp);
	strTemp.Format("%0.12lf",m_CurCamCalOriPhi);
	m_ED_CamOriPhi.SetWindowTextA(strTemp);
	strTemp.Format("%0.12lf",m_RotToCamX);
	m_ED_RotToCamX.SetWindowTextA(strTemp);
	strTemp.Format("%0.12lf",m_RotToCamY);
	m_ED_RotToCamY.SetWindowTextA(strTemp);

	//


	//显示相机安装方式
	for (int i =0;i<m_COB_CamFixType.GetCount();i++ )
	{
		if (m_COB_CamFixType.GetItemData(i) == m_CamFixedType )
		{
			m_COB_CamFixType.SetCurSel(i);
		}
	}
	for (int i = 0; i< m_SelMoveResType.GetCount() ;i++)
	{
		if (m_SelMoveResType.GetItemData(i)== m_PosMoveType)
		{
			m_SelMoveResType.SetCurSel(i);
		}

	}

	for (int i = 0; i< m_CB_AxeRotDirect.GetCount() ;i++)
	{
		if (m_CB_AxeRotDirect.GetItemData(i) == m_AxeRotDirect)
		{
			m_CB_AxeRotDirect.SetCurSel(i);
		}

	}

	//显示相机内外参
	ShowSysCamParamAndPos();
	
	if (pSysParam->CalWithCamParam[m_CamIndex][m_AxiIndex][m_ToolIndex])
	{
        m_CK_ParamMod.SetCheck(1);
	}else
	{
		m_CK_ParamMod.SetCheck(0);
	}

	strTemp.Format("%0.5lf",pSysParam->CalThickNess[m_CamIndex][m_AxiIndex][m_ToolIndex]);
	m_ED_CalThickness.SetWindowTextA(strTemp);

	////标定时的移动参数。
	m_CalMoveConfig.DeleteAllItems();
	m_SelMoveRow = -1;
	m_SelMoveCol = -1;

	for (int i= 0;i<pSysParam->CalPointNum[m_CamIndex][m_AxiIndex][m_ToolIndex];i++)
	{

		sprintf_s(szText,"%d",i);
		m_CalMoveConfig.InsertItem(i,szText);
		//m_CalMoveConfig.SetItemText(i,0,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%0.12lf",pSysParam->CalMotorX[m_CamIndex][m_AxiIndex][m_ToolIndex][i]);
		m_CalMoveConfig.SetItemText(i,1,szText);
		sprintf_s(szText,"%0.12lf",pSysParam->CalMotorY[m_CamIndex][m_AxiIndex][m_ToolIndex][i]);
		m_CalMoveConfig.SetItemText(i,2,szText);
		sprintf_s(szText,"%0.12lf",pSysParam->CalMotorPhi[m_CamIndex][m_AxiIndex][m_ToolIndex][i]);
		m_CalMoveConfig.SetItemText(i,3,szText);
	}	
}


void SysCamCalConfig::OnEnChangeCamcalCamindex()
{

}


void SysCamCalConfig::OnEnChangeCamcalAxiindex()
{
}

void SysCamCalConfig::ShowSysCamParamAndPos(void)
{
	CString strTemp;
	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][0]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM1,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][1]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM2,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][2]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM3,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][3]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM4,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][4]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM5,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][5]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM6,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][6]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM7,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][7]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM8,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][8]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM9,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][9]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM10,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][10]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM11,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][11]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPARAM12,strTemp);

	strTemp.Format("%0.12lf",pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][0]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPOS1,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][1]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPOS2,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][2]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPOS3,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][3]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPOS4,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][4]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPOS5,strTemp);
	strTemp.Format("%0.12lf",pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][5]);
	SetDlgItemTextA(IDC_CAMCAL_CAMPOS6,strTemp);

}

void SysCamCalConfig::SetSysCamParamAndPos()
{
    CString strTemp;
	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM1,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][0] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM2,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][1] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM3,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][2] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM4,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][3] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM5,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][4] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM6,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][5] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM7,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][6] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM8,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][7] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM9,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][8] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM10,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][9] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM11,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][10] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPARAM12,strTemp);
	pSysParam->CamParam[m_CamIndex][m_AxiIndex][m_ToolIndex][11] = atof(strTemp);

	GetDlgItemTextA(IDC_CAMCAL_CAMPOS1,strTemp);
	pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][0] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPOS2,strTemp);
	pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][1] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPOS3,strTemp);
	pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][2] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPOS4,strTemp);
	pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][3] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPOS5,strTemp);
	pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][4] = atof(strTemp);
	GetDlgItemTextA(IDC_CAMCAL_CAMPOS6,strTemp);
	pSysParam->CamPose[m_CamIndex][m_AxiIndex][m_ToolIndex][5] = atof(strTemp);


}



void SysCamCalConfig::OnBnClickedCamcalSave()
{
	// TODO: Add your control notification handler code here
	CString strTemp;

	if (m_CamIndex<0||m_AxiIndex<0||m_ToolIndex<0)
	{
		AfxMessageBox("机械序号选择错误");
		return ;
	}

	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	//strTemp.Format("%0.5lf",m_CurToolCalOriX);

	//标定初始坐标值
	m_ED_ToolOriX.GetWindowTextA(strTemp);
	pSysParam->ToolCalOriX[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 
	m_ED_ToolOriY.GetWindowTextA(strTemp);
	pSysParam->ToolCalOriY[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 
	m_ED_ToolOriAngle.GetWindowTextA(strTemp);
	pSysParam->ToolCalOriPhi[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 

	m_ED_CamOriX.GetWindowTextA(strTemp);
	pSysParam->CamCalOriX[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 
	m_ED_CamOriY.GetWindowTextA(strTemp);
	pSysParam->CamCalOriY[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 
	m_ED_CamOriPhi.GetWindowTextA(strTemp);
	pSysParam->CamCalOriPhi[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 

	//RotpointToCamX

	m_ED_RotToCamX.GetWindowTextA(strTemp);
	pSysParam->RotpointToCamX[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 
	m_ED_RotToCamY.GetWindowTextA(strTemp);
	pSysParam->RotpointToCamY[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp); 

	//相机固定方式
	//pSysParam->CamFixedType[m_CamIndex] = m_CamFixedType;


	//定位结果处理方式
	pSysParam->PosMoveType[m_CamIndex][m_AxiIndex][m_ToolIndex] = m_PosMoveType;



   //轴的旋转方向，一般是相反的。
	if (0 == m_AxeRotDirect)
	{
		pSysParam->AxeRotDirect[m_CamIndex][m_AxiIndex][m_ToolIndex] = false;
	}else
	{
		pSysParam->AxeRotDirect[m_CamIndex][m_AxiIndex][m_ToolIndex] = true;

	}

	//设定标定板厚度
	m_ED_CalThickness.GetWindowTextA(strTemp);
	pSysParam->CalThickNess[m_CamIndex][m_AxiIndex][m_ToolIndex] = atof(strTemp);

	//相机参数
	SetSysCamParamAndPos();

	//保持标定的运动数据
	for (int i = 0;i<m_CalMoveConfig.GetItemCount();i++)
	{
		strTemp = m_CalMoveConfig.GetItemText(i,1);
		pSysParam->CalMotorX[m_CamIndex][m_AxiIndex][m_ToolIndex][i] = atof(strTemp);
		strTemp = m_CalMoveConfig.GetItemText(i,2);
		pSysParam->CalMotorY[m_CamIndex][m_AxiIndex][m_ToolIndex][i] = atof(strTemp);
		strTemp = m_CalMoveConfig.GetItemText(i,3);
		pSysParam->CalMotorPhi[m_CamIndex][m_AxiIndex][m_ToolIndex][i] = atof(strTemp);

	}
    pSysParam->CalPointNum[m_CamIndex][m_AxiIndex][m_ToolIndex] = m_CalMoveConfig.GetItemCount();


	CDialogEx::OnOK();

}


void SysCamCalConfig::OnCbnSelchangeCamcalCamfixedtype()
{
	// TODO: Add your control notification handler code here
	//相机固定方式
	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	pSysParam->CamFixedType[m_CamIndex]=m_COB_CamFixType.GetItemData(m_COB_CamFixType.GetCurSel());

}

void SysCamCalConfig::OnCbnSelchangeCamcalAxerotdirect()
{
	// TODO: Add your control notification handler code here

	 m_AxeRotDirect = m_CB_AxeRotDirect.GetItemData(m_CB_AxeRotDirect.GetCurSel());

}




void SysCamCalConfig::OnCbnSelchangeCamcalSelcamindex()
{
	// TODO: Add your control notification handler code here
	m_CamIndex = m_SelCamIndex.GetItemData(m_SelCamIndex.GetCurSel());
	OnBnClickedSyscamcalShowdata();

	
}


void SysCamCalConfig::OnCbnSelchangeCamcalSelaxeindex()
{
	// TODO: Add your control notification handler code here
	m_AxiIndex = m_SelAxeIndex.GetItemData(m_SelAxeIndex.GetCurSel()); 
	OnBnClickedSyscamcalShowdata();

}


void SysCamCalConfig::OnCbnSelchangeCamcalSeltoolindex()
{
	// TODO: Add your control notification handler code here
	m_ToolIndex =  m_SelToolIndex.GetItemData(m_SelToolIndex.GetCurSel());
	OnBnClickedSyscamcalShowdata();
}


void SysCamCalConfig::OnCbnSelchangeCamcalRestype()
{
	// TODO: Add your control notification handler code here
	m_PosMoveType = m_SelMoveResType.GetItemData(m_SelMoveResType.GetCurSel());

}




void SysCamCalConfig::OnBnClickedCkUsecamparam()
{
	// TODO: Add your control notification handler code here
	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	int IsCheck = m_CK_ParamMod.GetCheck();
	if (0 == IsCheck)
	{
		pSysParam->CalWithCamParam[m_CamIndex][m_AxiIndex][m_ToolIndex] = false;

	}else if (1 == IsCheck)
	{
		pSysParam->CalWithCamParam[m_CamIndex][m_AxiIndex][m_ToolIndex] = true;
	}
}



//
void SysCamCalConfig::OnLvnItemchangedCamcalMoveset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here


	*pResult = 0;
}


void SysCamCalConfig::OnBnClickedAddMovation()
{
	// TODO: Add your control notification handler code here
	char szText[256] = {0};
	int ListRowIndex = m_SelMoveRow+1;//
	if (ListRowIndex>11)
	{
		return;
	}
	for (int i = ListRowIndex;i<m_CalMoveConfig.GetItemCount();i++)
	{
		sprintf_s(szText,"%d",i+1);
		m_CalMoveConfig.SetItemText(i,0,szText);
	}

	sprintf_s(szText,"%d",ListRowIndex);
	m_CalMoveConfig.InsertItem(ListRowIndex,szText);
	m_CalMoveConfig.SetItemText(ListRowIndex,1,"0.0");
	m_CalMoveConfig.SetItemText(ListRowIndex,2,"0.0");
	m_CalMoveConfig.SetItemText(ListRowIndex,3,"0.0");





}



void SysCamCalConfig::OnBnClickedDelMovation()
{
	// TODO: Add your control notification handler code here
	//m_CalMoveConfig.DeleteItem()
	if (m_SelMoveRow<0)
	{
		return;
	}
	char szText[64] ={0};

	for (int i = m_SelMoveRow+1;i<m_CalMoveConfig.GetItemCount();i++)
	{
		sprintf_s(szText,"%d",i-1);
		m_CalMoveConfig.SetItemText(i,0,szText);
	}
	m_CalMoveConfig.DeleteItem(m_SelMoveRow);
	//删除后去掉选择焦点行。
	m_SelMoveRow = -1;
	m_SelMoveCol = -1;

}


void SysCamCalConfig::OnNMDblclkCamcalMoveset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_SelMoveRow = pNMItemActivate->iItem;
	m_SelMoveCol = pNMItemActivate->iSubItem;
	CRect SubRect;
	//编辑对应的子项内容
	if (m_SelMoveRow>=0&&m_SelMoveCol>0)  //
	{
		//m_MapList.GetColumnWidth(ColIndex)  ;
		m_CalMoveConfig.GetSubItemRect(m_SelMoveRow,m_SelMoveCol,0,SubRect);
		if (pCommonEdit)
		{
			delete	pCommonEdit;
			pCommonEdit = NULL;
		}

		pCommonEdit = new CEdit;
		pCommonEdit->Create(WS_CHILD|WS_VISIBLE|WS_BORDER,SubRect,&m_CalMoveConfig,MOVEDATAEDITID);


		pCommonEdit->SetWindowTextA(m_CalMoveConfig.GetItemText(m_SelMoveRow,m_SelMoveCol));

	}


	*pResult = 0;
}


void SysCamCalConfig::OnOK()
{
	CString str;
	if (NULL == pCommonEdit)
	{
		return;
	}
	SYSTEM_PARAM* pSysParam = (SYSTEM_PARAM*)GetSysParamPoint();
	pCommonEdit->GetWindowTextA(str);
	m_CalMoveConfig.SetItemText(m_SelMoveRow,m_SelMoveCol,str);
	//将设定的数字放入
	double MoveData = atof(m_CalMoveConfig.GetItemText(m_SelMoveRow,m_SelMoveCol));
	//int ToolIndex = atoi(m_AxisToolList.GetItemText(m_ListRowIndex,1));
	switch (m_SelMoveCol)
	{
	case 1:
		pSysParam->CalMotorX[m_CamIndex][m_AxiIndex][m_ToolIndex][m_SelMoveRow]= MoveData;
		break;
	case 2:
		pSysParam->CalMotorY[m_CamIndex][m_AxiIndex][m_ToolIndex][m_SelMoveRow]= MoveData;
		break;
	case 3:
		pSysParam->CalMotorPhi[m_CamIndex][m_AxiIndex][m_ToolIndex][m_SelMoveRow]= MoveData;
		break;
	}


	delete	pCommonEdit;
	pCommonEdit = NULL;






    return;
    CDialogEx::OnOK();
}

void SysCamCalConfig::OnNMClickCamcalMoveset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_SelMoveRow = pNMItemActivate->iItem;
	m_SelMoveCol = pNMItemActivate->iSubItem;


	*pResult = 0;
}
