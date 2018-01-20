#pragma once
#include "ApplyFunParam.h"


// PositionConfig 对话框

class PositionConfig : public ApplyFunParam
{
	DECLARE_DYNAMIC(PositionConfig)

public:
	PositionConfig(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~PositionConfig();

// 对话框数据
	//enum { IDD = IDD_POSITIONCONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
