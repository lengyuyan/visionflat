#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "FlatfishDlg.h"

// SystemConfig 对话框
struct SceneName 
{
    CString Oldname;
    CString Updataname;
};

class SystemConfig : public CDialogEx
{
	DECLARE_DYNAMIC(SystemConfig)

public:
	SystemConfig(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SystemConfig();

 
// 对话框数据
	enum { IDD = IDD_DIALOG_GUIDE };
	void InitListctrl(CListCtrl *Listctrl);   //设置ListCtrl风格
protected:
    static SystemConfig *m_pInstance;
	virtual BOOL OnInitDialog();
	bool DeleteDirFile(char* psDirName);				//删除文件夹得操作
	void AddScenePross();                    //增加场景  
	void InitAllListctrl();				//初始化界面所有的ListCtrl
    static int WINAPI OnConfigMessage( unsigned int wParam, void * lParam, SYSTEM_MSG_TYPE msgType);
	CString GetPortNum();				//获取本机可用的串口号
    void SceneInstall();                  //系统参数设置到结构体
	virtual void SceneModifyInstall();				//场景修改设置
	virtual void SortPortInstall();				//串口排序设置
    virtual void SceneSave();				//场景参数的保存
	virtual void SortPortSave();              //串口排序的保存
    void Deletecamera();                      //删除相机序号
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:

	CListCtrl           m_Scene;				//场景列表
	CListCtrl           m_Camera;				//所有相机初始列表
	CListCtrl           m_Cameralist;				//相机排序后列表
	int                 SceneChoose;                 //场景修改列表框选中行
	int                 CameraChoose;                //相机枚举列表框选中行
	int                 reCameraChoose;              //相机排序后列表框选中行
	int                 DoPortChoose;                //串口枚举列表框选中行
	int                 SortPortItem;                //串口排序后列表框选中行  
	int                 SortPortSubItem;             //串口排序后列表框选中列
    int                 SceneItem;                   //场景修改列表框选中行
    int                 SceneSubItem;             //场景修改列表框选中列

    CString strFilename;    //增加的文件名的传递
    //static bool         LandType;
	afx_msg void OnClickListSceneModify(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonAddscene();    
	afx_msg void OnBnClickedButtonDeletescene();    
	afx_msg void OnBnClickedButtonUpscene();      
	afx_msg void OnBnClickedButtonDownscene();        
	afx_msg void OnClickListCameraCurrent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddCamera();      
	afx_msg void OnClickListCameraModify(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUpcamera();
	afx_msg void OnBnClickedButtonDowncamera();
	afx_msg void OnBnClickedButtonCameralistInstall();
	afx_msg void OnDblclkListCameraCurrent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListCameraModify(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonScenesave();
    afx_msg void OnBnClickedButtonCameraseave();
	CListCtrl m_DoPort;				//本机可用串口枚举
	CListCtrl m_SortPort;				//串口选择并排序
	afx_msg void OnDblclkListDoport(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickListDoport(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListSerialport(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickListSerialport(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUpsortport();
	afx_msg void OnBnClickedButtonDownsortport();
	afx_msg void OnClose();
    afx_msg void OnBnClickedButtonResetdisplay();
    afx_msg void OnBnClickedButtonAlterscenename();
    afx_msg void OnBnClickedButtonUpdatacamera();
    afx_msg void OnBnClickedButtonAddport();
    afx_msg void OnBnClickedButtonLightnummodify();
    afx_msg void OnBnClickedButtonUpdataport();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedCheckLandtype();
    CButton m_CheckLandType;
    CEdit m_SceneModiEdit;
    CEdit m_PortModiEdit;
    afx_msg void OnBnClickedButtonSavepath();
};

