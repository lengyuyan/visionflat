#pragma once
#include "resource.h"
#include "afxwin.h"

// CSceneModelSetting 对话框

class CSceneModelSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneModelSetting)

public:
	CSceneModelSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneModelSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCENE_MODEL };

public:
    void UpdateUI(int iSceneID, int index, CWnd *hROI);
	void UpdateUI();
	void GetContainer(void* pContainer);
	void GetDlgParam(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSetting();
    afx_msg void OnBnClickedBtnModelRoi();
    afx_msg void OnBnClickedBtnCreatModel();
    afx_msg void OnBnClickedBtnCheckModel();
    afx_msg void OnBnClickedButtonModelScoreA();
    afx_msg void OnBnClickedButtonModelScoreS();
    afx_msg void OnCbnSelchangeComboModelType();
    afx_msg void OnBnClickedButtonModelNumA();
    afx_msg void OnBnClickedButtonModelNumS();
    afx_msg void OnBnClickedButtonModelNumlevelsA();
    afx_msg void OnBnClickedButtonModelNumlevelsS();
    afx_msg void OnBnClickedButtonModelCNumlevelsA();
    afx_msg void OnBnClickedButtonModelCNumlevelsS();
    afx_msg void OnBnClickedButtonModelCostrastA();
    afx_msg void OnBnClickedButtonModelCostrastS();
    afx_msg void OnBnClickedButtonModelAngleStartA();
    afx_msg void OnBnClickedButtonModelAngleStartS();
    afx_msg void OnBnClickedButtonModelAngleExtentA();
    afx_msg void OnBnClickedButtonModelAngleExtentS();
public:
    void SetDisplayWindowID(int winID);
	void CloseSettingDlg();

private:
    int m_iCurSceneID;
    int m_iCurIndex;
    int m_winID;
    CWnd  *m_hROI;
	bool m_IsResetOriMatch;

public:
	void SaveProcConfig(CString ConfigPath,int ProcIndex);
	void ReadProcConfig(CString ConfigPath,int ProcIndex);
    BOOL VisionProcRun(CHECK_RESULT& ProcRes);

	void* m_pContainer;
	int m_ProcProcess;

	int m_ModeType;
	int m_BuildLevel;
	int m_MinContrast;
	int m_Contrast;
	double m_AngleStart;
	double m_AngleExtent;

	double m_OriMatchRow;
	double m_OriMatchCol;
	double m_OriMatchPhi;



	int m_MatchLevel;
	double m_MatchMinScore;
	int m_MatchNum;

	CEdit m_SceneMatchInfo;
	// 是否需要坐标变换
	CButton m_CheckAffine;
	afx_msg void OnBnClickedScenemodelAffine();
	CComboBox m_ResType;
	afx_msg void OnCbnSelchangeScenemodeRestype();
	CButton m_ResetOriMatch;
	afx_msg void OnBnClickedResetOrimatch();
};
