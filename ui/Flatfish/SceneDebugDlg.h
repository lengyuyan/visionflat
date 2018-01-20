#pragma once
#include "afxcmn.h"
#include "iManagerCtrl.h"
#include "DataType.h"
#include "SetSceneDlg.h"
#include "SceneImageSetting.h"
#include "SceneModelSetting.h"
#include "SceneThresholdSetting.h"
#include "SceneFindLine.h"
#include "SceneFindCircle.h"
#include "SelectFunction.h"
#include "InterfaceSetDlg.h"
#include "SEBtn.h"
#include "SRBtn.h"
#include "common.h"
#include <vector>
#include <map>
using namespace std;

#define  VISION_MANUAL_MSG       WM_USER + 1000    //显示图像消息

// CSceneDebugDlg 对话框
#define FILE_LIST_CTRL_COUNT   4   //显示图像列表个数


class CSceneDebugDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneDebugDlg)

public:
	CSceneDebugDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneDebugDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCENE_DEBUG };
	//CVisionServer m_visions;
protected:
    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg LRESULT OnManualMessage( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnSetScene( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnAddFunction( WPARAM wParam, LPARAM lParam );
    afx_msg void OnBnClickedBtnCrabpic();
    afx_msg void OnBnClickedBtnOpenFolder();
    afx_msg void OnBnClickedBtnOpenShow();
    //afx_msg void OnBnClickedBtnCloseShow();
    afx_msg void OnBnClickedBtnSaveoripic();
    afx_msg void OnBnClickedBtnReultpic();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton12();
    afx_msg void OnBnClickedBtnOpenPicfloder();
    afx_msg void OnBnClickedBtnClear();
    afx_msg void OnBnClickedBtnDelete();
    afx_msg void OnBnClickedBtnAllSelect();
    afx_msg void OnBnClickedCheckPic();
    afx_msg void OnBnClickedBtnStart();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnBnClickedBtnEdgeadd2();
    afx_msg void OnBnClickedBtnEdgeminus2();
    afx_msg void OnBnClickedCheckIsShowTmp();
    afx_msg void OnClickListFunction(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclkListFunction(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclkListPicture(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnSaveParam();
    afx_msg void OnBnClickedButtonAddFunction();
    afx_msg void OnBnClickedButtonSubFunction();
    afx_msg void OnDblclkStaticBind();
    afx_msg void OnBnClickedButtonRestore();
    afx_msg void OnBnClickedCheckIsAffine();
    virtual void OnCancel();
    virtual BOOL DestroyWindow();
    afx_msg void OnBnClickedCheckIsTop();
    afx_msg void OnBnClickedRadioDisAll();
    afx_msg void OnBnClickedRadioDis1();
    afx_msg void OnBnClickedRadioDis2();
    afx_msg void OnBnClickedRadioDis3();
    afx_msg void OnBnClickedButtonI1();
    afx_msg void OnBnClickedButtonI2();
    afx_msg void OnBnClickedButtonI3();
    afx_msg void OnBnClickedButtonI4();
	afx_msg void OnBnClickedButtonMoreConttrol();
	afx_msg void OnBnClickedButtonDeleteConttrol();

	DECLARE_MESSAGE_MAP()
private:
public:
    void init_scene_info();
    void init_list();
    void updata_ui_data(int iType = 0, int index = 0);
    void update_list_info();
    void updata_list();
    void find(char * lpPath,bool bflag);
    void UpdateMapIndex(int iSel);    //更新map中被删除项后面的索引号
    static int WINAPI OnRemoteMessage( unsigned int wParam, void *lParam, SYSTEM_MSG_TYPE msgType);
    void ShowFileListCtrl(int index);

private:
    CListCtrl m_listPicture[FILE_LIST_CTRL_COUNT];
    int       m_iCurGroup;
    CListCtrl m_listFunction;
	CButton m_checkbox;
	//CStatic m_static_scene;
	//CStatic m_static_index;
	long m_winID;
    CString m_strTemp;
    int          m_iTestImageIndex;     //批量文件测试
	CHECK_RESULT m_strsult;
	CSetSceneDlg m_setScene;
    CSelectFunction         m_SelectFunction;
    CSceneImageSetting      m_SceneImageSetting;
    CSceneModelSetting      m_SceneModelSetting;
    CSceneThresholdSetting  m_SceneThresholdSetting;
    CSceneFindLine          m_SceneFindLine;
    CSceneFindCircle        m_SceneFindCircle;
	CInterfaceSetDlg		m_InterfaceSet;
	vector<INTERFACE_SET_DATA>		m_interfaceSetData;
	vector<CSEBtn*>			m_SEBtn;
	vector<CSRBtn*>			m_SRBtn;

	bool m_sceneChange;
	//bool m_initControl;
	vector<bool> m_initControl;
	double m_height;
	int m_iT;
	int m_index;
    int m_iCurSceneID;
    int m_iCurIndex;
    CWnd  *m_hROI;

	vector<CString> m_vecorpic;
    vector<unsigned int> m_vecListInfo;
    static CSceneDebugDlg *m_pInstance;
    bool   m_bIsContinousGrab;  //是否处于实时显示状态
    void DrawControl();

public:

};
