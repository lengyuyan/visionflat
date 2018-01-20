#pragma once
#include "DataType.h"


#define  BTN_CLICKED_RADIO       WM_USER + 1002    //按钮加减消息 
enum BTN_RADIO_NUMB		//Radio选择
{
	BTN_RADIO_ZERO = 0,
	BTN_RADIO_ONE,
	BTN_RADIO_TOW,
	BTN_RADIO_THREE,
	BTN_RADIO_FOUR,
	BTN_RADIO_FIVE,
	BTN_RADIO_SIX,
	BTN_RADIO_SEVEN,
	BTN_RADIO_INIT,
	BTN_RADIO_DELETE,
};
// CSRBtn 对话框

class CSRBtn : public CDialogEx
{
	DECLARE_DYNAMIC(CSRBtn)

public:
	CSRBtn(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSRBtn();

// 对话框数据
	enum { IDD = IDD_DIALOG_SRBTN };

	void UpdateUI(int iSceneID, int index, CWnd *hROI, int revise = 0);;
	void getInterfaceSetData(INTERFACE_SET_DATA interfaceSetData);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg LRESULT OnBtnMessage( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()

private:
	INTERFACE_SET_DATA m_interfaceSetData;
	int m_iCurSceneID;
	int m_iCurIndex;
	int m_winID;
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
	afx_msg void OnBnClickedRadio8();
	void SetDisplayWindowID(int winID);
	void UpdateItem(BTN_RADIO_NUMB radioNumb, bool insert = false);
	virtual BOOL OnInitDialog();
	int CurSceneID();
	string BtnType();
	int m_radioNumb;
};
