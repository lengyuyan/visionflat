#pragma once


// CSceneImageSetting 对话框

class CSceneImageSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneImageSetting)

public:
	CSceneImageSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneImageSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCENE_IMAGE };

public:
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
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedBtnLightadd();
    afx_msg void OnBnClickedBtnLightminus();
};
