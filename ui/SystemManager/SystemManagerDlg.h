#pragma once

#include "common.h"
#include "LightDebugDlg.h"

#include "DataType.h"
#include "afxwin.h"


// CSystemManagerDlg 对话框
struct SceneName 
{
    CString Oldname;
    CString Updataname;
};

class CSystemManagerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemManagerDlg)

public:
	CSystemManagerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSystemManagerDlg();

// 对话框数据
	enum { IDD = IDD_SYSTEMSETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickListSceneModify(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonAddscene();    
	afx_msg void OnBnClickedButtonDeletescene();    
	afx_msg void OnBnClickedButtonUpscene();      
    afx_msg void OnBnClickedButtonDownscene();   
    afx_msg void OnBnClickedButtonScenesave();     
	afx_msg void OnClickListCameraCurrent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddCamera();      
	afx_msg void OnClickListCameraModify(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUpcamera();
	afx_msg void OnBnClickedButtonDowncamera();
	afx_msg void OnBnClickedButtonCameralistInstall();
	afx_msg void OnDblclkListCameraCurrent(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclkListCameraModify(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonCameraseave();
    afx_msg void OnBnClickedButtonUpdatacamera();
	//afx_msg void OnDblclkListDoport(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnClickListDoport(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnDblclkListSerialport(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnClickListSerialport(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedButtonUpsortport();
	//afx_msg void OnBnClickedButtonDownsortport();
 //   afx_msg void OnBnClickedButtonAddport();
    //   afx_msg void OnBnClickedButtonUpdataport();
    //afx_msg void OnBnClickedButtonLightnummodify();
    afx_msg void OnBnClickedButtonResetdisplay();
    afx_msg void OnBnClickedButtonAlterscenename();
    afx_msg void OnBnClickedCheckLandtype();
    afx_msg void OnBnClickedButtonSavepath();
    
    afx_msg void  CSystemManagerDlg::OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnClose();

private:
    void AddScenePross();                    //增加场景  
    virtual void SceneSave();				//场景参数的保存
    void Deletecamera();                      //删除相机序号
    void InitAllListctrl();
    void InitListctrl(CListCtrl *Listctrl);
    void InitPasswordDll();
    void SceneInstall();                  //系统参数设置到结构体
    virtual void SceneModifyInstall();				//场景修改设置
    //virtual void SortPortInstall();				//串口排序设置
    bool DeleteDirFile(char* psDirName);				//删除文件夹得操作
    static int WINAPI OnConfigMessage( unsigned int wParam, void * lParam, void* otherparam);

public:
    static CSystemManagerDlg *m_pInstance;
    CButton m_CheckLandType;
    CEdit m_SceneModiEdit;
	CEdit m_AxisNum;

    CListCtrl           m_Scene;				//场景列表
    CListCtrl           m_Camera;				//所有相机初始列表
    CListCtrl           m_Cameralist;				//相机排序后列表
    int                 SceneChoose;                 //场景修改列表框选中行
    int                 CameraChoose;                //相机枚举列表框选中行
    int                 reCameraChoose;              //相机排序后列表框选中行
    int                 SceneItem;                   //场景修改列表框选中行
    int                 SceneSubItem;             //场景修改列表框选中列

    CString strFilename;    //增加的文件名的传递
    
	  //增加通讯协议设定的内容
	CString m_CalRevHead;
	CString m_WorkRevHead;
	CString m_CalSendHead;
	CString m_WorkSendHead;
	CString m_IPDivid;
	CString m_IPEnd;

	void SetCommunitProctocol(const PROCTOCOLDATA&  SetProData);
	void GetCommunitProctocol(PROCTOCOLDATA&  SetProData);
	void GetCamCalData(int CamIndex,int AxiIndex);

	//产品的设定函数，用于换产品时一键切换
	void SetSysProductMap(void* pMapData);
	void GetSysProductMap(void* pMapData);
	void SetCurProductName(string ProductName);



public:
    int m_LoginLevels;
    afx_msg void OnBnClickedButtonLightnummodify();
    afx_msg void OnBnClickedCheckIsUserComm();
    CButton m_AutoOperLine;
    afx_msg void OnBnClickedButtonPasswordalter();
	afx_msg void OnBnClickedSyssetdlgIpprotocol();
	afx_msg void OnBnClickedSysconfigCalcam();


private:
        HINSTANCE m_hPasswordDLL;
public:

	afx_msg void OnBnClickedSyssettingProduct();
	afx_msg void OnBnClickedSysAxistool();
	afx_msg void OnBnClickedSimulateWork();
	CButton m_WorkSimulate;
};
