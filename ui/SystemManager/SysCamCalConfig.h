#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// SysCamCalConfig dialog

class SysCamCalConfig : public CDialogEx
{
	DECLARE_DYNAMIC(SysCamCalConfig)

public:
	SysCamCalConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~SysCamCalConfig();

// Dialog Data
	enum { IDD = IDD_CAMCAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	#define MOVEDATAEDITID  9001


	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_CalMoveConfig;
	int m_SelMoveRow;
	int m_SelMoveCol;
   CEdit* pCommonEdit;
	
	afx_msg void OnBnClickedSyscamcalShowdata();


	//CEdit m_ECamIndex;  //取消
	//CEdit m_EAxiIndex;  //取消
    //CEdit m_ED_ToolIndex;  //取消


	int m_CamIndex;
	int m_AxiIndex;
	int m_ToolIndex;

	int m_PosMoveType;


	afx_msg void OnEnChangeCamcalCamindex();
	afx_msg void OnEnChangeCamcalAxiindex();
	CEdit m_ED_ToolOriX;
	CEdit m_ED_ToolOriY;
	CEdit m_ED_CamOriX;
	CEdit m_ED_CamOriY;
	CEdit m_ED_CamOriPhi;
	CComboBox m_COB_CamFixType;
	CComboBox m_SelCamIndex;
	CComboBox m_SelAxeIndex;
	CComboBox m_SelToolIndex;
	CComboBox m_SelMoveResType;
	afx_msg void OnBnClickedCamcalSave();


	int m_CamFixedType;
	double m_CurToolCalOriX;
	double m_CurToolCalOriY;
	double m_CurToolCalOriAngle;
	double m_CurCamCalOriX;
	double m_CurCamCalOriY;
	double m_CurCamCalOriPhi;

    int   m_AxeRotDirect;

	double m_RotToCamX;
	double m_RotToCamY;

	void SetSysCamParamAndPos(void);
	void ShowSysCamParamAndPos(void);

	afx_msg void OnCbnSelchangeCamcalCamfixedtype();

	afx_msg void OnCbnSelchangeCamcalSelcamindex();
	afx_msg void OnCbnSelchangeCamcalSelaxeindex();
	afx_msg void OnCbnSelchangeCamcalSeltoolindex();
	afx_msg void OnCbnSelchangeCamcalRestype();
	CEdit m_ED_ToolOriAngle;
	CEdit m_ED_RotToCamX;
	CEdit m_ED_RotToCamY;
	CComboBox m_CB_AxeRotDirect;
	afx_msg void OnCbnSelchangeCamcalAxerotdirect();
	afx_msg void OnBnClickedCkUsecamparam();
private:
	//启用相机参数模式，九点标定和实际定位时，都是使用参数模式
	CButton m_CK_ParamMod;
	// 标定板厚度
	CEdit m_ED_CalThickness;


public:
	afx_msg void OnLvnItemchangedCamcalMoveset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddMovation();
	afx_msg void OnBnClickedDelMovation();
	afx_msg void OnNMDblclkCamcalMoveset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickCamcalMoveset(NMHDR *pNMHDR, LRESULT *pResult);
};
