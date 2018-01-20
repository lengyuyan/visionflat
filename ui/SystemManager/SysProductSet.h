#pragma once
#include "afxwin.h"
#include "SystemManagerDlg.h"
#include <map>
#include <hash_map>
#include "afxcmn.h"
#include <string>
using namespace std;
// SysProductSet dialog


class SysProductSet : public CDialogEx
{
	DECLARE_DYNAMIC(SysProductSet)

public:
	SysProductSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~SysProductSet();

// Dialog Data
	enum { IDD = IDD_PRODUCT_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	#define COMMONEDITID  9000
	DECLARE_MESSAGE_MAP()

protected:
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK();

public:
	afx_msg void OnBnClickedProductsetAddproduct();
	afx_msg void OnBnClickedProductsetAddmap();
	afx_msg void OnEnChangeProductsetName();
	CEdit m_EProductName;
	CString m_ProductName;

	CListBox m_LBNameList;
	int m_SelProductIndex;
	int m_SelMapIndex;
	CListCtrl m_MapList;
	CEdit* pCommonEdit;

	int m_MapRowIndex ;

	int m_MapColIndex;

	PMAPDATA m_ProductMap;  //用户存放产品及其映射数据。

	afx_msg void OnBnClickedProductsetDelproduct();
	afx_msg void OnLbnSelchangeProductsetList();
	afx_msg void OnBnClickedProductsetSave();

	void UpdateMapList(void);


public:
	//map<string ,map<int,int>>  m_ProductMap;




	afx_msg void OnLvnEndlabeleditProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnLvnKeydownProductsetScenemap(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedProductsetDelmap();
};
