#pragma once
#include "resource.h"

// ScenePreProSetting 对话框

class ScenePreProSetting : public CDialogEx
{
	DECLARE_DYNAMIC(ScenePreProSetting)

public:
	ScenePreProSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ScenePreProSetting();

// 对话框数据
	enum { IDD = IDD_PROPRECESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void UpdateUI(void);
	void GetContainer(void* pContainer);
	void CloseSettingDlg();
	void SaveProcConfig(CString ConfigPath,int ProcIndex);
	void ReadProcConfig(CString ConfigPath,int ProcIndex);
	BOOL VisionProcRun(CHECK_RESULT& ProcRes);

	//变量
	int m_ProcProcess;
	void* m_pContainer;
	double m_Multiplication;
	double m_Addition;

	afx_msg void OnBnClickedBtSetting();
	afx_msg void OnBnClickedBtTestenhance();
};
