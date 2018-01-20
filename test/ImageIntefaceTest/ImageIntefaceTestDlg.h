
// ImageIntefaceTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "HalconCpp.h"
#include <list>
#include "afxcmn.h"
using namespace Halcon;
using namespace std;

// CImageIntefaceTestDlg 对话框
class CImageIntefaceTestDlg : public CDialogEx
{
// 构造
public:
	CImageIntefaceTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IMAGEINTEFACETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonGetimageFirstr();
	CStatic m_static_windfirst;
	CStatic m_static_windsecond;
	afx_msg void OnBnClickedButtonGetimageFirst();

	Hobject m_hImage;
	Hlong m_diswin_first;
	Hlong m_diswin_second;
	Hlong m_imageHeight;
	Hlong m_imageWidth;
	void (*callback)(void *context);
	void DispImage();
	//void VedioCallback(void *context);
	list<string>::iterator m_list_str_it;
	string m_name;
public:
	afx_msg void OnBnClickedButtonVedioFirst();
	

	afx_msg void OnNMCustomdrawSliderExt(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_slider_exm;
	CSliderCtrl m_slider_gain;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedButtonVedioSecond();
};
