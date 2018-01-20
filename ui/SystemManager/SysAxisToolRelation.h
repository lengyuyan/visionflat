#pragma once


// SysAxisToolRelation 对话框

class SysAxisToolRelation : public CDialogEx
{
	DECLARE_DYNAMIC(SysAxisToolRelation)

public:
	SysAxisToolRelation(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SysAxisToolRelation();

// 对话框数据
	enum { IDD = IDD_SYS_AXIS_TOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	#define COMMONEDITID  9000

	virtual BOOL OnInitDialog();
	virtual void OnOK();


public:

	CListCtrl m_AxisToolList;
	CEdit* pCommonEdit;

	int m_ListRowIndex;
	int m_ListColIndex;

	void UpdataUI();


public:
	afx_msg void OnLvnItemchangedSysAxistoolMetrix(NMHDR *pNMHDR, LRESULT *pResult);







	afx_msg void OnBnClickedSysaxistoolSave();
	afx_msg void OnNMDblclkSysAxistoolMetrix(NMHDR *pNMHDR, LRESULT *pResult);
};
