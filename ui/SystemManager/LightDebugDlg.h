#pragma once
#include "afxcmn.h"


// CLightDebugDlg 对话框

class CLightDebugDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLightDebugDlg)

public:
	CLightDebugDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLightDebugDlg();

// 对话框数据
	enum { IDD = IDD_LIGHTDEBUG_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedButtonOpLight();
    afx_msg void OnNMCustomdrawSliderLightValue(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonSetLight();
    afx_msg void OnDblclkListDoport(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClickListDoport(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclkListSerialport(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClickListSerialport(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonUpsortport();
    afx_msg void OnBnClickedButtonDownsortport();
    afx_msg void OnBnClickedButtonAddport();
    afx_msg void OnBnClickedButtonUpdataport();
    afx_msg void OnBnClickedButtonLightnummodify();
    afx_msg void OnBnClickedButtonDeletePort();
    afx_msg void OnBnClickedButtonResetLight();

protected:
    void InitLightComBox();
    void SetLightParam();
    void InitAllListctrl();
    void InitListctrl(CListCtrl *Listctrl);
    void SortPortInstall();				//串口排序设置
    void SortPortSave();              //串口排序的保存
    CString GetPortNum();				//获取本机可用的串口号
    void SortShowBut();
public:
    CComboBox m_CommList;
    CSpinButtonCtrl m_SpinValue;
    CSliderCtrl m_SliderChanl;
    CListCtrl m_DoPort;				//本机可用串口枚举
    CListCtrl m_SortPort;				//串口选择并排序 
    CEdit m_PortModiEdit;
    int                 m_DoPortChoose;                //串口枚举列表框选中行
    int                 m_SortPortItem;                //串口排序后列表框选中行  
    int                 m_SortPortSubItem;             //串口排序后列表框选中列
    int m_LightValue;
    int m_OpenStatus;
    afx_msg void OnCbnSelchangeComboChannelIndex();
};
