#pragma once
#include "DataType.h"
#include "resource.h"

#define  ADD_FUNCTION_MSG    WM_USER +1200  

#define  FUN_TYPE PROCESS_TYPE;
// CSelectFunction 对话框
/*
功能：对话框显示
1，添加处理功能按钮
2，新版本保存了一个树结构，隐藏了旧版本中的功能按钮
3,最主要就是一些树的相应事件，然后通过ADD_FUNCTION_MSG,消息触底给场景调试窗口处理
*/

#define STR_FUN_IMAGE_READ       "图像读入"
#define STR_FUN_IMAGE_ACQ        "图像采集"
#define STR_FUN_IMAGE_ENHANCEMENT "图像增强"
#define STR_FUN_MATCH            "模板匹配"
#define STR_CONTOUR_POSITION     "轮廓定位"
#define STR_HALPROC_POSITION     "HAL定位"
#define STR_HALPROC_PROCESS      "HAL通用处理"
#define STR_NO_PROC              "未知处理"


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

	virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonImage();//图像采集 //一下都是旧版的功能按钮废弃
    afx_msg void OnBnClickedButtonModel();//模板匹配
    afx_msg void OnBnClickedButtonThreshold();//阈值处理
    afx_msg void OnBnClickedButtonFindLine();//找线
    afx_msg void OnBnClickedButtonFindCircle();//找圆

private:
    int m_iCurSceneID;//场景id
	int m_CurProcIndex; //当前处理项目的索引
	CTreeCtrl m_FunTree;
	CString m_ToolItemStr;
	BOOL m_ItemChange;
public:
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized); //激活窗口
	afx_msg void OnBnClickedButtonMore();  //更能按钮
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus); //显示窗口
	afx_msg void OnNMDblclkScenefuncTree(NMHDR *pNMHDR, LRESULT *pResult);//双击树种选项事件
	afx_msg void OnTvnSelchangedScenefuncTree(NMHDR *pNMHDR, LRESULT *pResult);//放在树上面就会响应事件,没用到
	afx_msg void OnBnClickedScenefunPosition();//轮廓定位功能
};
