#pragma once


// CSceneFindLine 对话框

class CSceneFindLine : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneFindLine)

public:
	CSceneFindLine(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneFindLine();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCENE_FINDLINE };

    void UpdateUI(int iSceneID, int index, CWnd *hROI);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    //afx_msg void OnBnClickedButtonSetting();
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
    afx_msg void OnBnClickedRadioAlgorithmReg();
    afx_msg void OnBnClickedRadioAlgorithmHub();
    afx_msg void OnBnClickedRadioAlgorithmTukey();
    afx_msg void OnBnClickedRadioAlgorithmGau();
    afx_msg void OnBnClickedRadioAlgorithmDrop();
    afx_msg void OnBnClickedBtnCheckOne();
    afx_msg void OnBnClickedBtnUpdateRoi();
    afx_msg void OnBnClickedCheckAutoFollow();

public:
    void SetDisplayWindowID(int winID);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    bool SetSceneParamItemValue(int index, int iType, int iValue, double dValue);
private:
    int m_iCurSceneID;
    int m_iCurIndex;
    CWnd  *m_hROI;
    int m_winID;
    bool m_bIsAutoFollow;

public:
};
