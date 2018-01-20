#pragma once


// CSceneThresholdSetting 对话框

class CSceneThresholdSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneThresholdSetting)

public:
	CSceneThresholdSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneThresholdSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCENE_THRESHOLD };

    void UpdateUI(int iSceneID, int index);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonSetting();

public:
    void SetDisplayWindowID(int winID);
private:
    int m_iCurSceneID;
    int m_iCurIndex;
    int m_winID;
public:
    afx_msg void OnBnClickedButtonAdd();
    afx_msg void OnBnClickedButtonSub();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
