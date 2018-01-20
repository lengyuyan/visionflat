// SceneLogicSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneManager.h"
#include "SceneLogicSetting.h"
#include "afxdialogex.h"


// SceneLogicSetting 对话框

IMPLEMENT_DYNAMIC(SceneLogicSetting, CDialogEx)

SceneLogicSetting::SceneLogicSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(SceneLogicSetting::IDD, pParent)
{
	 m_CurLogicType = Logic_Loop;
	 m_LoopTimes = 1;
	 m_LoopBeginIndex = 0;
	 m_LoopEndIndex = 0;

}

SceneLogicSetting::~SceneLogicSetting()
{
}

void SceneLogicSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCENELOGIC_LOGICTYPE, m_SelLogicType);
	DDX_Control(pDX, IDC_SCENELOGIC_LOOPTIMES, m_ELoopTimes);
	DDX_Control(pDX, IDC_SCENELOGIC_LOOPBEGIN, m_ELoopBegin);
	DDX_Control(pDX, IDC_SCENELOGIC_LOOPEND, m_ELoopEnd);
}


BEGIN_MESSAGE_MAP(SceneLogicSetting, CDialogEx)
	ON_CBN_SELCHANGE(IDC_SCENELOGIC_LOGICTYPE, &SceneLogicSetting::OnCbnSelchangeScenelogicLogictype)
	ON_BN_CLICKED(IDC_SCENELOGIC_SETTING, &SceneLogicSetting::OnBnClickedScenelogicSetting)
END_MESSAGE_MAP()


// SceneLogicSetting 消息处理程序

void SceneLogicSetting::UpdateUI()
{
	CString strBuf;
	strBuf.Format("%d",m_LoopTimes);
	m_ELoopTimes.SetWindowText(strBuf);
	strBuf.Format("%d",m_LoopBeginIndex);
	m_ELoopBegin.SetWindowText(strBuf);
	strBuf.Format("%d",m_LoopEndIndex);
	m_ELoopEnd.SetWindowText(strBuf);
	m_SelLogicType.InsertString(0,"循环");
	m_SelLogicType.SetItemData(0,Logic_Loop);
	//m_SelLogicType.InsertString(1,"分支");
	//m_SelLogicType.SetItemData(1,Logic_Branch);
    m_SelLogicType.SetCurSel(0);
}

void SceneLogicSetting::GetDlgLoopParam()
{
	// m_LoopTimes
	CString strBuf;

	

	m_ELoopTimes.GetWindowText(strBuf);
	m_LoopTimes = atoi(strBuf);

	m_ELoopBegin.GetWindowText(strBuf);
	m_LoopBeginIndex  = atoi(strBuf);

	m_ELoopEnd.GetWindowText(strBuf);
	m_LoopEndIndex = atoi(strBuf);


}

void SceneLogicSetting::OnCbnSelchangeScenelogicLogictype()
{
	// TODO: 在此添加控件通知处理程序代码

	//此函数的目的是根据逻辑类型的不同来显示不同的参数

}


void SceneLogicSetting::OnBnClickedScenelogicSetting()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_CurLogicType = static_cast<LOGIC_TYPE>(m_SelLogicType.GetCurSel());
	int LogicTypeID =  (int)m_SelLogicType.GetItemData(m_SelLogicType.GetCurSel());

	switch (LogicTypeID)
	{
	case Logic_Loop:
		{
			m_CurLogicType = Logic_Loop;
			GetDlgLoopParam();
		}
		break;
	}
    
}
