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
	afx_msg void OnBnClickedHalprocLoadpath();
	afx_msg void OnBnClickedHalprocTest();
	afx_msg void OnBnClickedHalprocCommit();
	CListCtrl m_list_input;

	CEditListCtrlEx m_L_INPUT;
	CEditListCtrlEx m_list_outparam;

	void InitVarList();    
	void EditVarList(int row, int col);
	void InsertNewRow(int row);
	afx_msg LRESULT OnMsgChangeItem(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	
};
