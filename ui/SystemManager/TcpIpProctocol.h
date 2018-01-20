#pragma once
#include "afxwin.h"
#include "SystemManagerDlg.h"

// TcpIpProctocol dialog

class TcpIpProctocol : public CDialogEx
{
	DECLARE_DYNAMIC(TcpIpProctocol)

public:
	TcpIpProctocol(CWnd* pParent = NULL);   // standard constructor
	virtual ~TcpIpProctocol();

// Dialog Data
	enum { IDD = IDD_SYS_WEBPROCTOCOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSyswebproctocolCommit();


	CEdit m_EdCalRevHead;
	CEdit m_EdWorkRevHead;
	CEdit m_EdCalSendHead;
	CEdit m_EdWorkSendHead;
	CEdit m_EdIpDivid;
	CEdit m_EdIpProctocolEnd;
};
