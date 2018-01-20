#pragma once
#include "DataType.h"

#define  ADD_FUNCTION_MSG    WM_USER +1200  

#define  FUN_TYPE PROCESS_TYPE;
// CSelectFunction 对话框

class CSelectFunction : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectFunction)

public:
	CSelectFunction(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelectFunction();

// 对话框数据
	enum { IDD = IDD_DIALOG_FUN_SELECT };

    void UpdateUI(int iSceneID, int index);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    void NotifyAddFunction(LPARAM iType);      //通知调试窗口增加功能
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonImage();
    afx_msg void OnBnClickedButtonModel();
    afx_msg void OnBnClickedButtonThreshold();
    afx_msg void OnBnClickedButtonFindLine();
    afx_msg void OnBnClickedButtonFindCircle();

private:
    int m_iCurSceneID;
public:
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedButtonMore();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
