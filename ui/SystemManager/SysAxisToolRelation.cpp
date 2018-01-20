// SysAxisToolRelation.cpp : 实现文件
//

#include "stdafx.h"
#include "SystemManager.h"
#include "SysAxisToolRelation.h"
#include "afxdialogex.h"
#include "SystemManagerDlg.h"
#include "iManagerCtrl.h"
#include "DataType.h"
// SysAxisToolRelation 对话框

IMPLEMENT_DYNAMIC(SysAxisToolRelation, CDialogEx)

SysAxisToolRelation::SysAxisToolRelation(CWnd* pParent /*=NULL*/)
	: CDialogEx(SysAxisToolRelation::IDD, pParent)
{

}

SysAxisToolRelation::~SysAxisToolRelation()
{
}

void SysAxisToolRelation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYS_AXISTOOL_METRIX, m_AxisToolList);

}


BEGIN_MESSAGE_MAP(SysAxisToolRelation, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SYS_AXISTOOL_METRIX, &SysAxisToolRelation::OnLvnItemchangedSysAxistoolMetrix)
	ON_BN_CLICKED(IDC_SYSAXISTOOL_SAVE, &SysAxisToolRelation::OnBnClickedSysaxistoolSave)
	ON_NOTIFY(NM_DBLCLK, IDC_SYS_AXISTOOL_METRIX, &SysAxisToolRelation::OnNMDblclkSysAxistoolMetrix)
END_MESSAGE_MAP()


// SysAxisToolRelation 消息处理程序


void SysAxisToolRelation::OnLvnItemchangedSysAxistoolMetrix(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

BOOL SysAxisToolRelation::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	DWORD ExtSty = m_AxisToolList.GetExStyle();
	ExtSty = ExtSty|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT;
	m_AxisToolList.SetExtendedStyle(ExtSty);


	//初始表头
	RECT ListRect;
	m_AxisToolList.GetClientRect(&ListRect);
	m_AxisToolList.InsertColumn(0,"轴序号");
	m_AxisToolList.SetColumnWidth(0,ListRect.right/6);
	m_AxisToolList.InsertColumn(1,"Tool序号");
	m_AxisToolList.SetColumnWidth(1,ListRect.right/6);
	m_AxisToolList.InsertColumn(2,"AinT_X");
	m_AxisToolList.SetColumnWidth(2,ListRect.right/6);
	m_AxisToolList.InsertColumn(3,"AinT_Y");
	m_AxisToolList.SetColumnWidth(3,ListRect.right/6);
	m_AxisToolList.InsertColumn(4,"X_Adjust");
	m_AxisToolList.SetColumnWidth(4,ListRect.right/6);
	m_AxisToolList.InsertColumn(5,"Y_Adjust");
	m_AxisToolList.SetColumnWidth(5,ListRect.right/6);

	pCommonEdit = NULL;

	//显示数据
	UpdataUI();


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void SysAxisToolRelation::UpdataUI()
{
	  m_AxisToolList.DeleteAllItems();
	//CSystemManagerDlg* pParent = dynamic_cast<CSystemManagerDlg*>(GetParent());
	 //int ListRowIndex = 0;
	 SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	 char szText[64]={0};
	 for (int i = 0, ListRowIndex =0;i <pSysParam->iAxisNum;i++)
	 {
		 for (int j = 0;j<pSysParam->iToolNum;j++, ListRowIndex++)
		 {

			 sprintf_s(szText,"%d",ListRowIndex);
			 m_AxisToolList.InsertItem(ListRowIndex,szText);

			 sprintf_s(szText,"%d",i);
			 m_AxisToolList.SetItemText(ListRowIndex,0,szText);

			 sprintf_s(szText,"%d",j);
			 m_AxisToolList.SetItemText(ListRowIndex,1,szText);

			 sprintf_s(szText,"%lf",pSysParam->RotX[i][j]);
			 m_AxisToolList.SetItemText(ListRowIndex,2,szText);

			 sprintf_s(szText,"%lf",pSysParam->RotY[i][j]);
			 m_AxisToolList.SetItemText(ListRowIndex,3,szText);

			 sprintf_s(szText,"%lf",pSysParam->RotX_Adjust[i][j]);
			 m_AxisToolList.SetItemText(ListRowIndex,4,szText);

			 sprintf_s(szText,"%lf",pSysParam->RotY_Adjust[i][j]);
			 m_AxisToolList.SetItemText(ListRowIndex,5,szText);

		 }
	 }
}

void SysAxisToolRelation::OnBnClickedSysaxistoolSave()
{
	// TODO: 在此添加控件通知处理程序代码
	SYSTEM_PARAM* pSysParam = ( SYSTEM_PARAM*)GetSysParamPoint();
	char szText[64]={0};
	int ToolNum = pSysParam->iToolNum;
	for (int i = 0;i <pSysParam->iAxisNum;i++)
	{
		for (int j = 0;j<ToolNum;j++)
		{
			m_AxisToolList.GetItemText(i*ToolNum+j,4,szText,sizeof(szText));
			pSysParam->RotX_Adjust[i][j] = atof(szText);

			m_AxisToolList.GetItemText(i*ToolNum+j,5,szText,sizeof(szText));
			pSysParam->RotY_Adjust[i][j] = atof(szText);

			//并将上面的数保存到配置文件中
		    SaveSysRotAxleAdjust(i,j,pSysParam->RotX_Adjust[i][j],pSysParam->RotY_Adjust[i][j]);

		}
	}




}


void SysAxisToolRelation::OnNMDblclkSysAxistoolMetrix(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	m_ListRowIndex = pNMItemActivate->iItem;
	m_ListColIndex = pNMItemActivate->iSubItem;
	CRect SubRect;
	//编辑对应的子项内容
	if (m_ListRowIndex>=0&&m_ListColIndex>=4)  //第0列的通信场景号不允许更改！！
	{
		//m_MapList.GetColumnWidth(ColIndex)  ;
		m_AxisToolList.GetSubItemRect(m_ListRowIndex,m_ListColIndex,0,SubRect);
		if (pCommonEdit)
		{
			delete	pCommonEdit;
			pCommonEdit = NULL;
		}

		pCommonEdit = new CEdit;
		pCommonEdit->Create(WS_CHILD|WS_VISIBLE|WS_BORDER,SubRect,&m_AxisToolList,COMMONEDITID);

		
		pCommonEdit->SetWindowTextA(m_AxisToolList.GetItemText(m_ListRowIndex,m_ListColIndex));

	}

	*pResult = 0;
}

void SysAxisToolRelation::OnOK()
{
	CString str;
	if (NULL == pCommonEdit)
	{
		return;
	}
	pCommonEdit->GetWindowTextA(str);
	m_AxisToolList.SetItemText(m_ListRowIndex,m_ListColIndex,str);
	//将设定的数字放入
	int AxisIndex = atoi(m_AxisToolList.GetItemText(m_ListRowIndex,0));
	int ToolIndex = atoi(m_AxisToolList.GetItemText(m_ListRowIndex,1));

	//CString strName;

	SYSTEM_PARAM* pSysParam = (SYSTEM_PARAM*)GetSysParamPoint();
	pSysParam->RotX_Adjust[AxisIndex][ToolIndex] = atof(m_AxisToolList.GetItemText(m_ListRowIndex,4));
	pSysParam->RotY_Adjust[AxisIndex][ToolIndex] = atof(m_AxisToolList.GetItemText(m_ListRowIndex,5));
	
	delete	pCommonEdit;
	pCommonEdit = NULL;
	

	return;

	CDialogEx::OnOK();
}

