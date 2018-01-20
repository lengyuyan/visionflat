#pragma once
#include "resource.h"

// HalProcPosSetting dialog

class HalProcPosSetting : public CDialogEx
{
	DECLARE_DYNAMIC(HalProcPosSetting)

public:
	HalProcPosSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~HalProcPosSetting();

// Dialog Data
	enum { IDD = IDD_HALPROC_LOCATION };

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
	//参数



	int InputParamId[INPUTPARAMNUM];
	double m_InParam[INPUTPARAMNUM];
	//参数名字
	int InputTitleId[INPUTPARAMNUM];
	CString m_InputTitle[INPUTPARAMNUM];

	CString m_ProcdurePath;



	afx_msg void OnBnClickedHalprocLoadpath();
	afx_msg void OnBnClickedHalprocTest();
	afx_msg void OnBnClickedHalprocCommit();
};
