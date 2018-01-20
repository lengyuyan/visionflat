#pragma once
#include "afxwin.h"
#include "resource.h"

// SceneLogicSetting 对话框

class SceneLogicSetting : public CDialogEx
{
	DECLARE_DYNAMIC(SceneLogicSetting)

public:
	SceneLogicSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SceneLogicSetting();

// 对话框数据
	enum { IDD = IDD_SCENE_LOGIC };

	enum  LOGIC_TYPE
	{
       Logic_Loop = 0,
	   Logic_Branch = 1
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 选择逻辑类型
	CComboBox m_SelLogicType;
	// 循环逻辑的循环次数
	CEdit m_ELoopTimes;
	// 循环逻辑的开始模块（场景中的某个处理）
	CEdit m_ELoopBegin;
	// 循环逻辑的结束模块 （场景中的某个处理）
	CEdit m_ELoopEnd;

public:
	void UpdateUI();
	void GetDlgLoopParam();
	LOGIC_TYPE m_CurLogicType;
	int m_LoopTimes;
	int m_LoopBeginIndex;
	int m_LoopEndIndex;

	 


public:

	afx_msg void OnCbnSelchangeScenelogicLogictype();
	afx_msg void OnBnClickedScenelogicSetting();
};
