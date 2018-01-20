#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "EditListCtrlEx.h"

// HalProcProcessSetting dialog

class HalProcProcessSetting : public CDialogEx
{
	DECLARE_DYNAMIC(HalProcProcessSetting)

public:
	HalProcProcessSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~HalProcProcessSetting();

// Dialog Data
	enum { IDD = IDD_HALPROC_NPROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

#define  INPUTPARAMNUM 12
public:

	void GetContainer(void* pContainer);
	void UpdateUI(void);
	void CloseSettingDlg();

	int m_ProcProcess;
	void* m_pContainer;

	void SaveProcConfig(CString ConfigPath,int ProcIndex);
	void ReadProcConfig(CString ConfigPath,int ProcIndex);
	BOOL VisionProcRun(CHECK_RESULT& ProcRes);

	//halcon Procdure输入参数
	//输入参数
	int m_Inputnum;
	int m_Isdisplay;
	vector<double>m_InParam;
	//输出参数
	int m_error;
	string hmessage;
	vector<double>m_OutParam;
	CString m_ProcdurePath;


	#define MOVEDATAEDITID  10001
	CListCtrl  m_L_INPUT;
	int m_SelMoveRow;
	int m_SelMoveCol;
	CEdit* pCommonEdit;


	CEditListCtrlEx m_list_outparam;
	void InitVarList();   
	void InsertNewRow(int row);
	virtual BOOL OnInitDialog();
	//下面5个完成输入参数
	void InitInputList();	
	afx_msg void OnBnClickedAddinput();//增加输入
	afx_msg void OnBnClickedDelinput();//删除输入
	afx_msg void OnLvnItemchangedListInputparam(NMHDR *pNMHDR, LRESULT *pResult);//无用
	afx_msg void OnNMClickListInputparam(NMHDR *pNMHDR, LRESULT *pResult);//单击中，选中
	afx_msg void OnNMDblclkListInputparam(NMHDR *pNMHDR, LRESULT *pResult);//双击，准备改变值,生成一个edit，新生成
	virtual void OnOK();//回车响应


	//3个相应按钮，设定，加载，测试
	afx_msg void OnBnClickedProcsettingp();
	afx_msg void OnBnClickedLoadproc();
	afx_msg void OnBnClickedProctestp();
};
