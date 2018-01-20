#pragma once
#include "DataType.h"


#define  BTN_ADD_SUB       WM_USER + 1001    //按钮加减消息 
enum BTN_ALGORITHM	//按钮运算法则
{
	BTN_ADD = 0,	//加法
	BTN_SUB,		//减法
	BTN_INIT,		//初始化
	BTN_DELETE,		//删除
};
// CSEBtn 对话框

class CSEBtn : public CDialogEx
{
	DECLARE_DYNAMIC(CSEBtn)

public:
	CSEBtn(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSEBtn();

// 对话框数据
	enum { IDD = IDD_DIALOG_SEBTN };

	void UpdateUI(int iSceneID, int index, CWnd *hROI, int revise = 0);
	void getInterfaceSetData(INTERFACE_SET_DATA interfaceSetData);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg LRESULT OnBtnMessage( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonSebAdd();
	afx_msg void OnClickedButtonSebSub();
	//afx_msg void OnBnClickedBtnCheckOne();
	//bool SetSceneParamItemValue(int index, int iType, int iValue, double dValue);
	void SetDisplayWindowID(int winID);
	void UpdateItem(CString str, BTN_ALGORITHM btnAlgorithm, bool insert = false);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	INTERFACE_SET_DATA m_interfaceSetData;
	int m_iCurSceneID;
	int m_iCurIndex;
	int m_winID;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditSeb();
	int CurSceneID();
	string BtnType();
};
