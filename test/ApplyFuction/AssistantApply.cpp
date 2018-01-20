#pragma  once

#include "StdAfx.h"
#include "AssistantApply.h"


OpticProcManange::OpticProcManange()
{
	m_SceneId = -1;
}

OpticProcManange::~OpticProcManange()
{
	FreeSceneProc();

}


void OpticProcManange::FreeSceneProc(void)
{
	//for (int i = 0; i<m_ScenceProcArr.GetCount();i++)
	//{
	//	if (m_ScenceProcArr.GetAt(i))
	//	{
	//	   delete m_ScenceProcArr.GetAt(i);
	//	}
	//}
	//m_ScenceProcArr.RemoveAll();
	
}

void OpticProcManange::InsertProcItem(int Index,VisionProcBase* pProcItem)
{

	//m_ScenceProcArr.InsertAt(Index,pProcItem);
}

void OpticProcManange::PushBackProcItem(VisionProcBase* pProcItem)
{
	//m_ScenceProcArr.Add(pProcItem);
}
int OpticProcManange::GetProcCount()
{
	//return m_ScenceProcArr.GetCount();
	return -1;
}

void OpticProcManange::DelProcItem(int ProcId)
{
	//for (int i = 0;i<GetProcCount();i++)
	//{
	//	if ( m_ScenceProcArr.GetAt(i)->GetProcId() == ProcId )
	//	{
	//		delete m_ScenceProcArr.GetAt(i);
	//		m_ScenceProcArr.RemoveAt(i);
	//		break;
	//	}
	//}

}



void OpticProcManange::RunProc()
{

	//for (int i = 0;i<GetProcCount();i++)
	//{
	//	m_ScenceProcArr.GetAt(i)->VisionProcRun();
	//}

}

void OpticProcManange::SetSceneId(int SceneId)
{
	m_SceneId = SceneId;
}
int OpticProcManange::GetSceneId()
{
	return m_SceneId;
}

BOOL  OpticProcManange::CmpSceneId(int SceneId)
{
	if (SceneId == m_SceneId)
	{
		return TRUE;
	}
	return FALSE;
}

void  OpticProcManange::SetProcNum(int ProcNum)
{
	m_ProcNum = ProcNum;

}

int  OpticProcManange::GetProcNum()
{
	return m_ProcNum;
}






/**********************运行（场景 管理）***********/


AssistantApply::AssistantApply(void)
{
}


AssistantApply::~AssistantApply(void)
{

}

OpticProcManange* AssistantApply::GetSceneManaById(int SceneId)
{
	//for (int i = 0;i<m_SceneManaList.GetCount();i++)
	//{
		//if (m_SceneManaList.GetAt(i)->GetSceneId() == SceneId )
		//{
		//	return m_SceneManaList.GetAt(i);
		//}
	//}
	return NULL;

}

void AssistantApply::AddSceneManager(OpticProcManange* pSceneMana)
{
	//m_SceneManaList.Add(pSceneMana);

}

void AssistantApply::InsertSceneManager(int Index,OpticProcManange* pSceneMana)
{
     // m_SceneManaList.InsertAt(Index,pSceneMana);

}

void AssistantApply::DelSceneManager(int Index)
{
	//delete m_SceneManaList.GetAt(Index);
	//m_SceneManaList.RemoveAt(Index);
}

void AssistantApply::SwapSceneManager(int Index1,int Index2)
{
	if (Index1 == Index2)
	{
		return;
	}
	//OpticProcManange* pTemp1 = m_SceneManaList.GetAt(Index1);
   // m_SceneManaList.GetAt(Index1) = m_SceneManaList.GetAt(Index2);
	//m_SceneManaList.GetAt(Index2) = pTemp1;


}