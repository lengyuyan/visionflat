// SysProductSet.cpp : implementation file
//

#include "stdafx.h"
#include "SystemManager.h"
#include "SysProductSet.h"
#include "afxdialogex.h"
#include "SystemManagerDlg.h"

// SysProductSet dialog

IMPLEMENT_DYNAMIC(SysProductSet, CDialogEx)

SysProductSet::SysProductSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(SysProductSet::IDD, pParent)
{
	m_SelProductIndex = -1;
	m_SelMapIndex = -1;
}

SysProductSet::~SysProductSet()
{
}

void SysProductSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRODUCTSET_NAME, m_EProductName);
	DDX_Control(pDX, IDC_PRODUCTSET_LIST, m_LBNameList);
	DDX_Control(pDX, IDC_PRODUCTSET_SCENEMAP, m_MapList);
}


BEGIN_MESSAGE_MAP(SysProductSet, CDialogEx)
	ON_BN_CLICKED(IDC_PRODUCTSET_ADDPRODUCT, &SysProductSet::OnBnClickedProductsetAddproduct)
	ON_BN_CLICKED(IDC_PRODUCTSET_ADDMAP, &SysProductSet::OnBnClickedProductsetAddmap)
	ON_EN_CHANGE(IDC_PRODUCTSET_NAME, &SysProductSet::OnEnChangeProductsetName)
	ON_BN_CLICKED(IDC_PRODUCTSET_DELPRODUCT, &SysProductSet::OnBnClickedProductsetDelproduct)
	ON_LBN_SELCHANGE(IDC_PRODUCTSET_LIST, &SysProductSet::OnLbnSelchangeProductsetList)
	ON_BN_CLICKED(IDC_PRODUCTSET_SAVE, &SysProductSet::OnBnClickedProductsetSave)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PRODUCTSET_SCENEMAP, &SysProductSet::OnLvnEndlabeleditProductsetScenemap)
	ON_NOTIFY(NM_DBLCLK, IDC_PRODUCTSET_SCENEMAP, &SysProductSet::OnNMDblclkProductsetScenemap)
	ON_NOTIFY(NM_CLICK, IDC_PRODUCTSET_SCENEMAP, &SysProductSet::OnNMClickProductsetScenemap)
	ON_NOTIFY(LVN_KEYDOWN, IDC_PRODUCTSET_SCENEMAP, &SysProductSet::OnLvnKeydownProductsetScenemap)
	ON_BN_CLICKED(IDC_PRODUCTSET_DELMAP, &SysProductSet::OnBnClickedProductsetDelmap)
END_MESSAGE_MAP()


// SysProductSet message handlers
BOOL SysProductSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	

	DWORD LdwStyle = m_MapList.GetExtendedStyle();
	LdwStyle =       m_MapList.GetExtendedStyle();
	LdwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	LdwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	m_MapList.SetExtendedStyle(LdwStyle); //设置扩展风格
	//Listctrl->ModifyStyle(0, LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL);//设置某一行选中


	//初始化产品列表
	 //PMAPDATA ProductMap;
	 CSystemManagerDlg* pParent = dynamic_cast<CSystemManagerDlg*>(GetParent());
	 pParent->GetSysProductMap(&m_ProductMap);
	 PMAPDATA::iterator it;
	 for (it = m_ProductMap.begin();it!=m_ProductMap.end();it++)
	 {
		m_LBNameList.AddString((it->first).c_str());

	 }

	//初始化映射列表
	RECT MapRect;
	m_MapList.GetClientRect(&MapRect);
	m_MapList.InsertColumn(0,"通信场景号");
	m_MapList.SetColumnWidth(0,MapRect.right/2);
	m_MapList.InsertColumn(1,"产品实际场景号");
	m_MapList.SetColumnWidth(1,MapRect.right/2);

	pCommonEdit = NULL;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



void SysProductSet::UpdateMapList(void)
{
	PMAPDATA::iterator it;
	SCENEMAP::iterator mapIt;
	char szSceneNo[256]={0};
	char szMapNo[256]={0};
	CString ProductName;

	m_LBNameList.GetText(m_SelProductIndex,ProductName);
	it = m_ProductMap.find(ProductName.GetBuffer());
	if (it != m_ProductMap.end())
	{
		//更新选中的产品的MAP状态
		m_MapList.DeleteAllItems();
		int i = 0;
		for (mapIt = it->second.begin();mapIt!=it->second.end();mapIt++,i++)
		{
			memset(szSceneNo,0,sizeof(szSceneNo));
			memset(szMapNo,0,sizeof(szMapNo));
			sprintf_s(szSceneNo,"%d",mapIt->first);
			sprintf_s(szMapNo,"%d",mapIt->second);
			m_MapList.InsertItem(i,szSceneNo,0);
			m_MapList.SetItemText(i,0,szSceneNo);
			m_MapList.SetItemText(i,1,szMapNo);

		}
	}else  
	{
		//如果是新增加产品，则MAP显示空白。
		m_MapList.DeleteAllItems();
	}


}


//增加产品
void SysProductSet::OnBnClickedProductsetAddproduct()
{
	// TODO: Add your control notification handler code here
	//m_LBNameList.AddString(m_ProductName);
	char szText[256] ={0};
	sprintf_s(szText,"是否添加新产品:%s",m_ProductName);
	if (0 >= m_ProductName.GetLength())
	{
		AfxMessageBox("产品名字不能为空");
		return;
	}
	if(0 < m_LBNameList.FindString(0,m_ProductName))
	{
		AfxMessageBox("产品名字重复，请重新命名");
		return;
	}


	if( 1 == MessageBoxA(szText,m_ProductName,MB_OKCANCEL))
	{
		m_LBNameList.InsertString(++m_SelProductIndex,m_ProductName);
		//m_SelProductIndex = m_LBNameList.GetCount()-1;
		m_LBNameList.SetCurSel(m_SelProductIndex);	

		UpdateMapList();	
	}

}

//增加映射
void SysProductSet::OnBnClickedProductsetAddmap()
{
	// TODO: Add your control notification handler code here

	PMAPDATA::iterator it;
	SCENEMAP::iterator mapIt;
	SCENEMAP PSceneMap;

	char szSceneNo[256]={0};
	//char szMapNo[256]={0};
	if(m_SelProductIndex<0)
		return;

	memset(szSceneNo,0,sizeof(szSceneNo));
	//memset(szMapNo,0,sizeof(szMapNo));
	int CurNo = m_MapList.GetItemCount();

	sprintf_s(szSceneNo,"%d",CurNo);
	//sprintf_s(szMapNo,"%d",mapIt->second);
	m_MapList.InsertItem(CurNo,szSceneNo,0);
	m_MapList.SetItemText(CurNo,0,szSceneNo);
	//int m_MapList.GetItemText(CurNo,)
	m_MapList.SetItemText(CurNo,1,"-1");

	//将新的map数据放入到产品的映射数据中。
	CString ProductName;
	//m_LBNameList.GetWindowTextA(ProductName);


	m_LBNameList.GetText(m_SelProductIndex,ProductName);

	int strIndex = m_LBNameList.FindString(0,ProductName);
	
	it = m_ProductMap.find(ProductName.GetBuffer());
	if (it == m_ProductMap.end())
	{
		//没有该产品的映射，则增加映射
		if (strIndex >= 0)
		{
			PSceneMap.insert(make_pair(CurNo,-1));
		    m_ProductMap.insert(make_pair(ProductName.GetBuffer(),PSceneMap));
		}

	}else 
	{
		//已经有该产品的映射，在其映射后增加
		it->second.insert(make_pair(CurNo,-1));

	}
	UpdateMapList();

}


void SysProductSet::OnEnChangeProductsetName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_EProductName.GetWindowTextA(m_ProductName);

}


void SysProductSet::OnBnClickedProductsetDelproduct()
{
	// TODO: Add your control notification handler code here
	CString strName;
	PMAPDATA::iterator it;
	if (m_SelProductIndex > -1)
	{
		m_LBNameList.GetText(m_SelProductIndex,strName);
		m_LBNameList.DeleteString(m_SelProductIndex);

		it = m_ProductMap.find(strName.GetBuffer());
		if (it!= m_ProductMap.end())
		{
			m_ProductMap.erase(m_ProductMap.find(strName.GetBuffer()));

		}
	}

	if (m_SelProductIndex>m_LBNameList.GetCount()-1)
	{
		m_SelProductIndex = m_LBNameList.GetCount()-1;
	
	}

    //重新设定焦点
	if (m_SelProductIndex > -1)
	{
		m_LBNameList.SetCurSel(m_SelProductIndex);
	}

	UpdateMapList();


}


void SysProductSet::OnLbnSelchangeProductsetList()
{
	// TODO: Add your control notification handler code here
	CSystemManagerDlg* pParent = dynamic_cast<CSystemManagerDlg*>(GetParent());
	CString strName;
	 m_SelProductIndex = m_LBNameList.GetCurSel();
	 UpdateMapList();

	 //设置选中产品
	 m_LBNameList.GetText(m_SelProductIndex,strName);
	 pParent->SetCurProductName(strName.GetBuffer());
	

}


void SysProductSet::OnBnClickedProductsetSave()
{
	// TODO: Add your control notification handler code here
	CSystemManagerDlg* pParent = dynamic_cast<CSystemManagerDlg*>(GetParent());

	//填充数据
	//PMAPDATA ProductMap;

	//设置参数
	pParent->SetSysProductMap(&m_ProductMap);

	CDialogEx::OnOK();

}


void SysProductSet::OnLvnEndlabeleditProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码




	*pResult = 0;
}


void SysProductSet::OnNMDblclkProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_MapRowIndex = pNMItemActivate->iItem;
	m_MapColIndex = pNMItemActivate->iSubItem;
	CRect SubRect;
	//编辑对应的子项内容
	if (m_MapRowIndex>=0&&m_MapColIndex>0)  //第0列的通信场景号不允许更改！！
	{
		//m_MapList.GetColumnWidth(ColIndex)  ;
		m_MapList.GetSubItemRect(m_MapRowIndex,m_MapColIndex,0,SubRect);
		if (pCommonEdit)
		{
			delete	pCommonEdit;
			pCommonEdit = NULL;
		}

	    pCommonEdit = new CEdit;
		pCommonEdit->Create(WS_CHILD|WS_VISIBLE|WS_BORDER,SubRect,&m_MapList,COMMONEDITID);

	}

	*pResult = 0;
}


void SysProductSet::OnNMClickProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
     m_SelMapIndex = pNMItemActivate->iItem;


	*pResult = 0;
}


void SysProductSet::OnLvnKeydownProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	WORD vkKeydown =  pLVKeyDow->wVKey;
	if (VK_RETURN == vkKeydown||VK_ESCAPE == vkKeydown)
	{
	    delete	pCommonEdit;

	}

	*pResult = 0;
}

 void SysProductSet::OnOK()
{
	 CString str;
	 if (NULL == pCommonEdit)
	 {
		 return;
	 }
	 pCommonEdit->GetWindowTextA(str);
	 m_MapList.SetItemText(m_MapRowIndex,m_MapColIndex,str);
	 //将设定的数字放入映射中
	 CString strName;
	 PMAPDATA::iterator it;
	 if (m_SelProductIndex > -1)
	 {
		 m_LBNameList.GetText(m_SelProductIndex,strName);
		 it = m_ProductMap.find(strName.GetBuffer());
		 if (it!= m_ProductMap.end())
		 {
			it->second.find(m_MapRowIndex)->second = atoi(str);
		     
		 }
	 }
	 delete	pCommonEdit;
	 pCommonEdit = NULL;


	return;

	CDialogEx::OnOK();
}


 void SysProductSet::OnBnClickedProductsetDelmap()
 {
	 // TODO: 在此添加控件通知处理程序代码
	 PMAPDATA::iterator it;
	 SCENEMAP::iterator mapIt;
	 SCENEMAP PSceneMap;

	 char szSceneNo[256]={0};
	 //char szMapNo[256]={0};
	 if(m_SelProductIndex<0)
		 return;

	 memset(szSceneNo,0,sizeof(szSceneNo));
	 //memset(szMapNo,0,sizeof(szMapNo));
	 int CurNo = m_MapList.GetItemCount();
	 if (CurNo<1)
	 {
		 return;
	 }

	 //sprintf_s(szSceneNo,"%d",CurNo);
	 //m_MapList.DeleteItem(CurNo-1);


	 //sprintf_s(szMapNo,"%d",mapIt->second);
	// m_MapList.InsertItem(CurNo,szSceneNo,0);
	// m_MapList.SetItemText(CurNo,0,szSceneNo);
	 //int m_MapList.GetItemText(CurNo,)
	// m_MapList.SetItemText(CurNo,1,"-1");

	 //将新的map数据放入到产品的映射数据中。
	 CString ProductName;
	 //m_LBNameList.GetWindowTextA(ProductName);


	 m_LBNameList.GetText(m_SelProductIndex,ProductName);

	 int strIndex = m_LBNameList.FindString(0,ProductName);

	 it = m_ProductMap.find(ProductName.GetBuffer());
	 if (it == m_ProductMap.end())
	 {
		 //没有该产品的映射，则什么都不做。
		 return;		

	 }else 
	 {
		 //已经有该产品的映射，则减去最后一个映射

		 it->second.erase(CurNo-1);

	 }
	 UpdateMapList();

 }
