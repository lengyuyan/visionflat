// TcpIpProctocol.cpp : implementation file
//

#include "stdafx.h"
#include "SystemManager.h"
#include "TcpIpProctocol.h"
#include "afxdialogex.h"
#include "DataType.h"

// TcpIpProctocol dialog

IMPLEMENT_DYNAMIC(TcpIpProctocol, CDialogEx)

TcpIpProctocol::TcpIpProctocol(CWnd* pParent /*=NULL*/)
	: CDialogEx(TcpIpProctocol::IDD, pParent)
{

}

TcpIpProctocol::~TcpIpProctocol()
{
}

void TcpIpProctocol::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYSWEBPROCTOCOL_CALREVHEAD, m_EdCalRevHead);
	DDX_Control(pDX, IDC_SYSWEBPROCTOCOL_WORKREVHEAD, m_EdWorkRevHead);
	DDX_Control(pDX, IDC_SYSWEBPROCTOCOL_CALSENDHEAD, m_EdCalSendHead);
	DDX_Control(pDX, IDC_SYSWEBPROCTOCOL_WORKSENDHEAD, m_EdWorkSendHead);
	DDX_Control(pDX, IDC_SYSWEBPROCTOCOL_DIVID, m_EdIpDivid);
	DDX_Control(pDX, IDC_SYSWEBPROCTOCOL_END, m_EdIpProctocolEnd);
}


BEGIN_MESSAGE_MAP(TcpIpProctocol, CDialogEx)
	ON_BN_CLICKED(IDC_SYSWEBPROCTOCOL_COMMIT, &TcpIpProctocol::OnBnClickedSyswebproctocolCommit)
END_MESSAGE_MAP()


// TcpIpProctocol message handlers
BOOL  TcpIpProctocol::OnInitDialog()
{
	 CDialogEx::OnInitDialog();

	 PROCTOCOLDATA  SetProData;
	 CSystemManagerDlg* pParent = dynamic_cast<CSystemManagerDlg*>(GetParent());
	 pParent->GetCommunitProctocol(SetProData);

	 m_EdCalRevHead.SetWindowTextA(SetProData.CalRevHead);
	 m_EdWorkRevHead.SetWindowTextA(SetProData.WorkRevHead);
	 m_EdCalSendHead.SetWindowTextA(SetProData.CalSendHead);
	 m_EdWorkSendHead.SetWindowTextA(SetProData.WorkSendHead);
	 m_EdIpDivid.SetWindowTextA(SetProData.IPDivid);
	 m_EdIpProctocolEnd.SetWindowTextA(SetProData.IPEnd);

	 return TRUE;

}

void TcpIpProctocol::OnBnClickedSyswebproctocolCommit()
{
	// TODO: Add your control notification handler code here
	PROCTOCOLDATA  SetProData;
	CString strTemp1,strTemp2,strTemp3,strTemp4,strTemp5,strTemp6;
	CSystemManagerDlg* pParent = dynamic_cast<CSystemManagerDlg*>(GetParent());
	m_EdCalRevHead.GetWindowTextA(strTemp1);
	m_EdWorkRevHead.GetWindowTextA(strTemp2);
	m_EdCalSendHead.GetWindowTextA(strTemp3);
	m_EdWorkSendHead.GetWindowTextA(strTemp4);
	m_EdIpDivid.GetWindowTextA(strTemp5);
	m_EdIpProctocolEnd.GetWindowTextA(strTemp6);

	memcpy(SetProData.CalRevHead,strTemp1,sizeof(SetProData.CalRevHead));
	memcpy(SetProData.WorkRevHead,strTemp2,sizeof(SetProData.WorkRevHead));
	memcpy(SetProData.CalSendHead,strTemp3,sizeof(SetProData.CalSendHead));
	memcpy(SetProData.WorkSendHead,strTemp4,sizeof(SetProData.WorkSendHead));
	memcpy(SetProData.IPDivid,strTemp5,sizeof(SetProData.IPDivid));
	memcpy(SetProData.IPEnd,strTemp6,sizeof(SetProData.IPEnd));

	SetProData.MakeEndable();
	pParent->SetCommunitProctocol(SetProData);

	OnOK();


}
