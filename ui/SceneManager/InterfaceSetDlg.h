#pragma once
#include "DataType.h"


// CInterfaceSetDlg 对话框

class CInterfaceSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInterfaceSetDlg)

public:
	CInterfaceSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInterfaceSetDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_INTERFACE_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void UpdataUI(int index = 0);
	void init_btn_info(void);
	void showWindowSEBtn(int index);
	void showWindowSRBtn(int index);
	void showWindowBtn(int index);
	void showWindowChk(int index);
	afx_msg void OnSelchangeComboRoiType();
	afx_msg void OnSetfocusEditBtnName();
	afx_msg void OnSelchangeBtnChangeRate1();
	afx_msg void OnSelchangeComboOptionNumb();
	afx_msg void OnSelchangeComboBtnType();

	INTERFACE_SET_DATA showWindow(CInterfaceSetDlg &m_InterfaceSet, int iSceneID, int iT, int nCIndex = 0);

private:
	CStatic *m_staticBtnType;
	CStatic *m_staticBtnName;
	CStatic *m_staticItemType;
	CStatic *m_staticItemNumb;
	CStatic *m_staticInitValue;
	CStatic *m_staticChangeRate;
	CStatic *m_staticOptionNumb;
	CComboBox *m_cboBtnType;
	CComboBox *m_cboItemType;
	CComboBox *m_cboOptionNumb;
	CEdit *m_editBtnName;
	CEdit *m_editItemNumb;
	CEdit *m_editInitValue;
	CButton *m_changeRate1;
	CButton *m_changeRate2;
	CButton *m_changeRate3;
	CStatic *m_staticOption1;
	CStatic *m_staticOption2;
	CStatic *m_staticOption3;
	CStatic *m_staticOption4;
	CStatic *m_staticOption5;
	CStatic *m_staticOption6;
	CStatic *m_staticOption7;
	CStatic *m_staticOption8;
	CEdit *m_editOption1;
	CEdit *m_editOption2;
	CEdit *m_editOption3;
	CEdit *m_editOption4;
	CEdit *m_editOption5;
	CEdit *m_editOption6;
	CEdit *m_editOption7;
	CEdit *m_editOption8;
	
	int m_iCurSceneID;
	int m_nItem;
	int m_nCIndex;
	int m_iT;
	INTERFACE_SET_DATA m_interfaceSetData;

};
