
// FlatFishDlg.h : 头文件
//

#pragma once

#include <vector>
#include "common.h"
#include "iManagerCtrl.h"
using namespace std;
// CFlatFishDlg 对话框
class CFlatFishDlg : public CDialogEx
{
// 构造
public:
	CFlatFishDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FLATFISH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
    static DWORD WINAPI  CheckStateThread(LPVOID lpParameter);

    afx_msg LRESULT OnManualMessage( WPARAM wParam, LPARAM lParam );
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    virtual BOOL DestroyWindow();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

    //afx_msg LRESULT OnSDKMessage( WPARAM wParam, LPARAM lParam );

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    void initLayout();
    void initStatusBar();
    void initSettingDll();
	void initSceneList();
	void GrabSceneInit();

    void unLoadDll();
    static int WINAPI OnSDKMessage( unsigned int wParam, void * lParam, void* otherparam);
    virtual void OnOK();
    virtual void OnCancel();
	void *GetImageBufByFile(char *path,int &size,int &x,int &h);
public:
    vector<CStatic *>m_vecPicItem;   //用于保存主窗口界面图像
	


private:
    static CFlatFishDlg *m_pInstance;
    //状态栏信息
    CStatusBar m_statusBar;    
	static bool isstatusready;//判断状态栏是否准备好，先不枷锁

    UINT      *m_pIndicators;             
    int       m_LoginLevel;   
    UINT      m_IndicatorsSize;   //状态栏状态个数
	bool      m_IsSceneInit;
	int     m_LayRow;
	int     m_LayCol;

    
    HINSTANCE m_hSystemSettingDLL;
    HINSTANCE m_hSceneDebugDLL;
    HINSTANCE m_hLoginDLL;
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	CString m_TitleName;
	
	
};
