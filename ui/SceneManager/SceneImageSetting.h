#pragma once
#include "resource.h"
#include "iManagerCtrl.h"
//#include "ProcConfigBase.h"
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
	void UpdateUI();  //新版本的函数
	void CloseSettingDlg();
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
    afx_msg void OnBnClickedBtnGainAdd();
    afx_msg void OnBnClickedBtnGainMinus();

//新的方式 增加的部分
public:
	int m_ShutterTime;
	int m_Gain;
	int m_LightIndex;
	int m_LightValue;
	int m_CameraIndex;
	int m_Interval;  //设置参数间隔时间,ms
	VisionProcBase* m_pContainer;


	bool ConfigGrabImage(bool sendflag);
	void GetContainer(VisionProcBase* pContainer);

	void SaveProcConfig(CString ConfigPath,int ProcIndex);
	void ReadProcConfig(CString ConfigPath,int ProcIndex);
	BOOL VisionProcRun(CHECK_RESULT& ProcRes); 

	int m_ProcProcess;
	CString m_ConfigPath;
	int m_ProcIndex;



};
