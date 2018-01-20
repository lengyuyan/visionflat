#include "StdAfx.h"
#include "ScenePositon.h"
#include <assert.h>


ScenePositon::ScenePositon(void)
{
	BuildInstance();
}
ScenePositon::ScenePositon(CString ConfigPath,int ProcID):VisionProcBase(ConfigPath,ProcID)
{
	BuildInstance();
}

ScenePositon::~ScenePositon(void)
{
	m_ScenePositionSetting.CloseSettingDlg();
}


void ScenePositon::BuildInstance(void)
{
	CString szTemp;
	m_ProcTypeId = CONTOUR_POSITION;
	m_ScenePositionSetting.m_ProcProcess = 0;  //进入这个类肯定是0
	m_ScenePositionSetting.GetContainer(this);
	ReadProcConfig(m_ProcNo);

}

void ScenePositon::InitInstance(CWnd* pParentWnd)
{	
	CRect rtS;
	if (NULL == pParentWnd)
	{
		return;
	}
	
	m_pParentWnd = pParentWnd;

	assert(pParentWnd->IsKindOf( RUNTIME_CLASS(CWnd)));
	m_pParentWnd->GetClientRect(&rtS);
	m_ScenePositionSetting.Create(IDD_SCENE_POSITION, m_pParentWnd);
	m_ScenePositionSetting.SetWindowPos(NULL,rtS.left,rtS.top,rtS.right,rtS.bottom,SWP_NOZORDER);
	m_ScenePositionSetting.ShowWindow(SW_SHOW);
	m_ScenePositionSetting.UpdateUI();
	

	m_SceneInitialCount = g_SceneInitialCount;

}

void ScenePositon::DeleteSelf()
{
	delete this;

}


void ScenePositon::HideConfigWnd()
{
	if(g_SceneInitialCount==m_SceneInitialCount)
	{
		m_ScenePositionSetting.ShowWindow(SW_HIDE);
	}
}

void ScenePositon::ShowConfigWnd()
{	
	if(g_SceneInitialCount==m_SceneInitialCount)
	{
		m_ScenePositionSetting.ShowWindow(SW_SHOW);
	}
}

void ScenePositon::ShowAllShape(void)
{
	//OnShowAllRoi(g_SceneDlgHwinId,GetSceneId(),&m_ShapeIdList);
}


void ScenePositon::AddCurShapeId()
{
	
    m_RoiIdList.push_back(GetCurShapeId(GetSceneId()));  //m_ShapeIdList
}

void ScenePositon::DelCoordinateShapeId()
{
	m_RoiIdList.clear();
}


bool ScenePositon::PosFitCircle(int MatchIndex,double& row,double& col,double& Rii)
{
	
	//double Rii = 0.0;
	int ImageIndex = g_SceneList.GetSceneBySceneID(GetSceneId())->GetProcImageIndex(GetProcId());
	//借用参数Rii传入形状ID号。  // 借用row表示测量的数是像素值0,还是实际单位1
	Rii = m_RoiIdList.at(m_RoiIdList.size()-1);
	row = 1.0;

	long WinId = -1;
	if(g_SceneInitialCount==m_SceneInitialCount)
	{
		WinId = g_SceneDlgHwinId;
		//FindHalconWinID(g_SceneDlgHwinId,HalWinId);
	}else
	{
		SYSTEM_PARAM *pSysParam = NULL;
	
		pSysParam = (SYSTEM_PARAM *)GetSysParamPoint();
		vector<CStatic *>* pImageWnd = ((vector<CStatic *>*)(pSysParam->MainImageWndVec));
		if (GetSceneId() < pImageWnd->size())
		{
			WinId = (long)(pImageWnd->at(GetSceneId())->m_hWnd);
			//FindHalconWinID((long)(pImageWnd->at(GetSceneId())->m_hWnd),HalWinId);

		}
	}
	if (-1 == WinId )
	{
		return false;
	}
	//调用视觉库中的函数进行圆的拟合，返回拟合后的...... row, col, Rii .坐标。
	if(!VisionMeasureCircle(WinId,GetSceneId(),ImageIndex,MatchIndex,row,col,Rii))
	{
		return false;
	}

	//将像素坐标转换成相机坐标。
	
	return true;

}

bool ScenePositon::PosFitLine(int MatchIndex,double& row1,double& col1,double& row2,double& col2)
{




	return true;
}


void ScenePositon::SaveProcConfig(int ProcIndex)
{
	VisionProcBase::SaveProcConfig(ProcIndex);
	//添加子类代码
	//保存ROI的参数
	m_ScenePositionSetting.SaveProcConfig(m_ConfigPath,ProcIndex);
	//for (int i = 0;i<m_ShapeIdList.size();i++)
	//{


	//}



}


void ScenePositon::ReadProcConfig(int ProcIndex)
{
	VisionProcBase::ReadProcConfig(ProcIndex);
	//添加子类代码
	char szValue[256] = {0};
	CString ProcIndexName,strTemp;
	int CombShapeNum = 0;
	int ShapeId = -1;
	ST_ROI_INFO RoiInfor;
	ProcIndexName.Format("%d",ProcIndex);

	//读取模板名
	//memset(m_ModelName,0,sizeof(m_ModelName));
	//GetPrivateProfileStringA(ProcIndexName, "ModelName", "",m_ModelName,sizeof(m_ModelName), m_ConfigPath);
	// 

	m_ScenePositionSetting.ReadProcConfig(m_ConfigPath,ProcIndex);



}


BOOL ScenePositon::VisionProcRun(CHECK_RESULT& ProcRes)
{
	if(m_ScenePositionSetting.VisionProcRun(ProcRes))
	{
		ProcRes.err = 0;
		return TRUE;
	}else
	{
		ProcRes.err = CONTOUR_POSITION;
	    return FALSE;
	}
	return TRUE;
}
