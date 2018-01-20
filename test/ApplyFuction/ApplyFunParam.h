#pragma once


// ApplyFunParam 对话框

class   /*_declspec(dllexport)*/ ApplyFunParam : public CDialogEx
{
	DECLARE_DYNAMIC(ApplyFunParam)

public:
	ApplyFunParam(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ApplyFunParam();

// 对话框数据
	enum { IDD = IDD_APPLYFUC_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	



};
