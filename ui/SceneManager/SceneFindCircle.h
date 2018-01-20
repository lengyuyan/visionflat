#pragma once


// CSceneFindCircle 对话框

class CSceneFindCircle : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneFindCircle)

public:
	CSceneFindCircle(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneFindCircle();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCENE_FINDCIRCLE };

    void UpdateUI(int iSceneID, int index ,CWnd *hROI, bool btemp);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSetting();
    afx_msg void OnBnClickedBtnEdgeadd();
    afx_msg void OnBnClickedBtnEdgeminus();
    afx_msg void OnBnClickedBtnSmoothadd();
    afx_msg void OnBnClickedBtnSmoothminus();
    afx_msg void OnBnClickedBtnRoiadd();
    afx_msg void OnBnClickedBtnRoiminus();
    afx_msg void OnBnClickedBtnNumadd();
    afx_msg void OnBnClickedBtnNumminus();
    afx_msg void OnBnClickedRadioTransitionAll();
    afx_msg void OnBnClickedRadioTransitionPositive();
    afx_msg void OnBnClickedRadioTransitionNegative();
    afx_msg void OnBnClickedRadioFindTypeAll();
    afx_msg void OnBnClickedRadioFindTypeFirst();
    afx_msg void OnBnClickedRadioFindTypeLast();
    afx_msg void OnBnClickedRadioDirectionP();
    afx_msg void OnBnClickedRadioDirectionN();
	afx_msg void OnBnClickedRadioAlgorithmAlgebraic();
	afx_msg void OnBnClickedRadioAlgorithmAhuber();
	afx_msg void OnBnClickedRadioAlgorithmAtukey();
	afx_msg void OnBnClickedRadioAlgorithmGeometric();
	afx_msg void OnBnClickedRadioAlgorithmGeohuber();
	afx_msg void OnBnClickedRadioAlgorithmGeotukey();
    afx_msg void OnBnClickedBtnCheckOne();
    afx_msg void OnBnClickedBtnUpdateRoi();
    afx_msg void OnBnClickedCheckAutoFollow();
public:
    void SetDisplayWindowID(int winID);
    bool SetSceneParamItemValue(int index, int iType, int iValue, double dValue);
private:
    int m_iCurSceneID;
    int m_iCurIndex;
    CWnd *m_hROI;
    int m_winID;
    bool m_bIsAutoFollow;
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnBnClickedButtonSetBeginAngle();
    afx_msg void OnBnClickedButtonSetEndAngle();
    int m_FindCircleBeginAngle;
    int m_FindCircleEndAngle;
};
