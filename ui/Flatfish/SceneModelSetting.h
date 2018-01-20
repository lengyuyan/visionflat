#pragma once


// CSceneModelSetting 对话框

class CSceneModelSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneModelSetting)

public:
	CSceneModelSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneModelSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCENE_MODEL };

    void UpdateUI(int iSceneID, int index, CWnd *hROI);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSetting();
    afx_msg void OnBnClickedBtnModelRoi();
    afx_msg void OnBnClickedBtnCreatModel();
    afx_msg void OnBnClickedBtnCheckModel();
public:
    void SetDisplayWindowID(int winID);
private:
    int m_iCurSceneID;
    int m_iCurIndex;
    int m_winID;
    CWnd  *m_hROI;
public:
    afx_msg void OnBnClickedButtonModelScoreA();
    afx_msg void OnBnClickedButtonModelScoreS();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
