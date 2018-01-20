#include "StdAfx.h"
#include "SceneLogic.h"
#include <assert.h>


SceneLogic::SceneLogic(void)
{
	BuildInstance();
}
SceneLogic::SceneLogic(CString ConfigPath,int ProcNo):VisionProcBase(ConfigPath,ProcNo)
{
	BuildInstance();
}

SceneLogic::~SceneLogic(void)
{
}

void SceneLogic::BuildInstance(void)
{
	CString szTemp;
	m_ProcTypeId = LOGIC_CTR;
	
}

void SceneLogic::InitInstance(CWnd* pParentWnd)
{
	CRect rtS;
	if (NULL == pParentWnd)
	{
		return;
	}
	m_pParentWnd = pParentWnd;

	assert(pParentWnd->IsKindOf( RUNTIME_CLASS(CWnd)));
	m_pParentWnd->GetClientRect(&rtS);
	m_LogicSetting.Create(IDD_SCENE_LOGIC, m_pParentWnd);
	m_LogicSetting.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
	m_LogicSetting.ShowWindow(SW_SHOW);
	//m_LogicSetting.UpdateUI();

	m_SceneInitialCount = g_SceneInitialCount;
}


void SceneLogic::DeleteSelf()
{
	delete this;

}

void SceneLogic::HideConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
	{
		m_LogicSetting.ShowWindow(SW_HIDE);
	}


}
void SceneLogic::ShowConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
		 m_LogicSetting.ShowWindow(SW_SHOW);

}


int SceneLogic::GetLoopTimes()
{
	return m_LogicSetting.m_LoopTimes;

}
int SceneLogic::GetLoopBeginIndex()
{
	return m_LogicSetting.m_LoopBeginIndex;

}
int SceneLogic::GetLoopEndIndex()
{
	return m_LogicSetting.m_LoopEndIndex;
}





void SceneLogic::SaveProcConfig(int ProcIndex)
{

}
void SceneLogic::ReadProcConfig(int ProcIndex)
{

}
BOOL SceneLogic::VisionProcRun(CHECK_RESULT& ProcRes) //执行视觉的处理
{

	return TRUE;
}