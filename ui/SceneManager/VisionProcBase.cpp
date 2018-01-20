#include "StdAfx.h"
#include "VisionProcBase.h"
#include "SceneLogic.h"




VisionProcBase::VisionProcBase(void)
{
	BuildInstance();
}

VisionProcBase::VisionProcBase(CString ConfigPath,int ProcID):m_ConfigPath(ConfigPath)
{
     BuildInstance();
	 m_VisonProcId = ProcID; //为场景Id*10000+index  在处理
	 VisionProcIdCount = ProcID; //暂时没有用到
	 m_ProcNo = ProcID%SCENE_PROC_ID_FACTOR; //包体中的索引,也是处理链表中的索引
	 
}

VisionProcBase::~VisionProcBase(void)
{
	
}

int VisionProcBase::VisionProcIdCount = -1;



void VisionProcBase::VisionProc(...)
{

}

void VisionProcBase::BuildInstance(void)   //该函数只能在构造函数中调用，只能调用一次
{
	m_pParentWnd = NULL;
	m_VisonProcId = -1;
	m_SceneInitialCount = 0;//判断是否打开场景窗口
	m_CurSceneCamIndex = 0; //当前相机索引
}

void  VisionProcBase::InitInstance(CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;//场景debu上有个设置面板作为父亲
}

 BOOL VisionProcBase::IsInitialConfigWnd()
{
	//g_SceneInitialCount全局初始为0,场景dug构造和析构时++
	//m_SceneInitialCount在vp中是不增加的

	return  (m_SceneInitialCount!=g_SceneInitialCount);  // !g_IsSceneDlgInitial;
} 



int  VisionProcBase::GetProcId()
{
	return m_VisonProcId;
}

int  VisionProcBase::GetProcTypeID()
{
	return m_ProcTypeId;
}

void VisionProcBase::DeleteSelf()
{



}

int VisionProcBase::GetProcIdBase()
{
	return m_VisonProcId%SCENE_PROC_ID_FACTOR;
}


int VisionProcBase::GetSceneId()
{
	 return m_VisonProcId/SCENE_PROC_ID_FACTOR;

}
int VisionProcBase::GetProcIndexById(int ProcID)
{
	return (g_SceneList.GetSceneBySceneID(GetSceneId()))->GetProcIndex(ProcID);
}

int VisionProcBase::GetProcTypeIndex()
{
	int TypeIndex = -1;
	for (int i =0,k = 0; i<=GetProcIndexById(GetProcId());i++)//跟据ID找到他的索引,然后根据索引找到本身在相同类型中的索引
	{
		if( m_ProcTypeId == GetCurScene()->GetProcType(i))
		{	
			TypeIndex = k;
			k++;
		}
	}
	return TypeIndex;
}


ProcList* VisionProcBase::GetCurScene()
{
	return g_SceneList.GetSceneBySceneID(GetSceneId());
}


void VisionProcBase::ReadAndCreateRect(CString ProcIndexName,int ShapeIndex)
{
	CString strTemp;
	char szResValue[256]={0};
	ST_ROI_INFO RectRoi;

	strTemp.Format("ShapeID%d",ShapeIndex);
	int ShapeId = GetPrivateProfileInt(ProcIndexName,strTemp,-1,m_ConfigPath);
	m_RoiIdList.push_back(ShapeId);

	strTemp.Format("ShapeParam%d_Row",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	RectRoi.strect.row = atof(szResValue);

	strTemp.Format("ShapeParam%d_Col",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	RectRoi.strect.col = atof(szResValue);

	strTemp.Format("ShapeParam%d_Angle",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	RectRoi.strect.angle = atof(szResValue);

	strTemp.Format("ShapeParam%d_Len1",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	RectRoi.strect.length1 = atof(szResValue);

	strTemp.Format("ShapeParam%d_Len2",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	RectRoi.strect.length2 = atof(szResValue);
	//调用SDK的管理函数在场景对应的图形处理对象中生成形状。
	GenShape( GetSceneId(),RectRoi, HoRecTangleType,ShapeId,m_CurSceneCamIndex);

}

void  VisionProcBase::ReadAndCreateCircle(CString ProcIndexName,int ShapeIndex)
{
	CString strTemp;
	char szResValue[256]={0};
	ST_ROI_INFO CircleRoi;

	strTemp.Format("ShapeID%d",ShapeIndex);
	int ShapeId = GetPrivateProfileInt(ProcIndexName,strTemp,-1,m_ConfigPath);
	m_RoiIdList.push_back(ShapeId);

	strTemp.Format("ShapeParam%d_Row",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	CircleRoi.stcircle.row = atof(szResValue);

	strTemp.Format("ShapeParam%d_Col",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	CircleRoi.stcircle.col = atof(szResValue);

	strTemp.Format("ShapeParam%d_Rii",ShapeIndex);
	memset(szResValue,0,sizeof(szResValue));
	GetPrivateProfileStringA(ProcIndexName,strTemp,"-1",szResValue,sizeof(szResValue),m_ConfigPath);
	CircleRoi.stcircle.Radius = atof(szResValue);
	//调用SDK的管理函数在场景对应的图形处理对象中生成形状。
	GenShape( GetSceneId(),CircleRoi, HoCircleType,ShapeId,m_CurSceneCamIndex);


}

void VisionProcBase::AfterCreateComb()
{
	m_RoiID = m_RoiIdList.at(m_RoiIdList.size()-1);
	m_RoiIdList.clear();
	m_RoiComIdList.clear();

}


void VisionProcBase::ReadProcConfig(int ProcIndex)
{
	//读取各个处理类的信息应该放在各个子类里面去,不应该放在基类里

	//读取的Roi相关参数，生成Shape和ROI,并将ROI的Id放入基类的m_RoiIdList中。
	//作为创建一个模板，ROI只能有一个，
	//获取组合图形的信息并生成对象。
	// 
	char szValue[256] = {0};
	CString ProcIndexName,strTemp;
	int CombShapeNum = 0;
	int ShapeId = -1;
	ST_ROI_INFO RoiInfor;
	ProcIndexName.Format("%d",ProcIndex);

	ShapeId = GetPrivateProfileInt(ProcIndexName,"CombShapeID", -1, m_ConfigPath);//组合总ID:207
	GetPrivateProfileStringA(ProcIndexName, "CombParamID", "",szValue,sizeof(szValue), m_ConfigPath);//组合中的形状ID: 208_209
	CombShapeNum =  MyFunction::MyGetSubstrNum(szValue,"_");//例子:1_2_3则返回3,标识由3个形状
	for (int i = 0;i<CombShapeNum;i++)
	{
		strTemp = MyFunction::MyGetSubStr(szValue,i+1,1,"_");
		RoiInfor.stCombine.m_ShapIdList.push_back(atoi(strTemp));
	}
	GetPrivateProfileStringA(ProcIndexName, "CombParamType", "",szValue,sizeof(szValue), m_ConfigPath);
	for (int i = 0;i< MyFunction::MyGetSubstrNum(szValue,"_");i++)
	{
		strTemp =  MyFunction::MyGetSubStr(szValue,i+1,1,"_");      //
		RoiInfor.stCombine.m_CombTypeList.push_back(atoi(strTemp));//组合中参数类型 并 差 交
		m_RoiComIdList.push_back(atoi(strTemp));//类型放到visionProcBase的成员m_RoiComIdList中
	}
	//处理的结果类型 相对坐标
	m_ResultType =(ProcResultType)GetPrivateProfileInt(ProcIndexName,"ResultType", 0, m_ConfigPath);



	m_RoiID = ShapeId;
	//manager向外提供的函数
	GenShape( GetSceneId(),RoiInfor, HoComposeType,ShapeId,m_CurSceneCamIndex);  //仅生成组合区域ID并传入基本图元ID list，但没绘制，
	//m_RoiIdList.push_back(ShapeId);
	//获取基本图元信息并绘制
	//int ShapeNum = GetPrivateProfileInt(ProcIndexName, "RoiShapeNum", -1, m_ConfigPath);
	for (int i = 0;i<CombShapeNum ;i++)
	{
		strTemp.Format("ShapeType%d",i);
		int ShapeType = GetPrivateProfileInt(ProcIndexName,strTemp, -1, m_ConfigPath);
		switch (ShapeType)  //读取并创建图形
		{
		case HoRecTangleType:
			ReadAndCreateRect(ProcIndexName,i);
			break;
		case HoCircleType:
			ReadAndCreateCircle(ProcIndexName,i);
			break;
		}	

	}

}

void VisionProcBase::SaveProcConfig(int ProcIndex)
{
	CString ProcIndexName, StrTemp,StrTemp1;
	ProcIndexName.Format("%d",ProcIndex);
	StrTemp.Format("%d",m_ProcTypeId);
	WritePrivateProfileStringA(ProcIndexName, "ProcType",StrTemp, m_ConfigPath);
	StrTemp.Format("%d",m_VisonProcId);
	WritePrivateProfileStringA(ProcIndexName, "ProcId",StrTemp, m_ConfigPath);

	StrTemp.Format("%d",m_ResultType);
	WritePrivateProfileStringA(ProcIndexName, "ResultType",StrTemp, m_ConfigPath);
	
	//StrTemp.Format("%d",m_AxisIndex);
	//WritePrivateProfileStringA(ProcIndexName, "AxisIndex",StrTemp, m_ConfigPath);

	//将ROI的信息保存
	ProcIndexName.Format("%d",ProcIndex);
	StrTemp.Format("%d",m_RoiID);
	WritePrivateProfileStringA(ProcIndexName,"CombShapeID", StrTemp, m_ConfigPath);
	StrTemp.Empty();
	for (int i = 0;i<m_RoiIdList.size();i++)
	{
		StrTemp1.Format("%d_",m_RoiIdList.at(i));
		StrTemp +=StrTemp1;
	}
	WritePrivateProfileStringA(ProcIndexName,"CombParamID", StrTemp, m_ConfigPath);  

	StrTemp.Empty();
	for (int i = 0;i<m_RoiComIdList.size();i++)
	{
		StrTemp1.Format("%d_",m_RoiComIdList.at(i));
		StrTemp +=StrTemp1;
	}
	WritePrivateProfileStringA(ProcIndexName,"CombParamType", StrTemp, m_ConfigPath);  
   //保存基本图元的参数
	  ST_ROI_INFO RoiParam;
	  for (int i = 0;i<m_RoiIdList.size();i++)
	  {
		  StrTemp1.Format("%d",m_RoiIdList.size());
		  WritePrivateProfileStringA(ProcIndexName,"RoiShapeNum", StrTemp1, m_ConfigPath);
		  StrTemp.Format("ShapeID%d",i);
		  StrTemp1.Format("%d",m_RoiIdList.at(i));
		  WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);

		  StrTemp.Format("ShapeType%d",i);
		  GetRoiParam(GetSceneId(),m_RoiIdList.at(i),&RoiParam);
		  switch (RoiParam.iType)
		  {
		  case HoRecTangleType:
			  {
				   StrTemp1.Format("%d",HoRecTangleType);
				   WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				   StrTemp.Format("ShapeParam%d_Row",i);
				   StrTemp1.Format("%.6lf",RoiParam.strect.row);
				   WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				   StrTemp.Format("ShapeParam%d_Col",i);
				   StrTemp1.Format("%.6lf",RoiParam.strect.col);
				    WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				   StrTemp.Format("ShapeParam%d_Angle",i);
				   StrTemp1.Format("%.6lf",RoiParam.strect.angle);
				   WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				   StrTemp.Format("ShapeParam%d_Len1",i);
				   StrTemp1.Format("%.6lf",RoiParam.strect.length1);
				   WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				   StrTemp.Format("ShapeParam%d_Len2",i);
				   StrTemp1.Format("%.6lf",RoiParam.strect.length2);
				   WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);

			  }
		  	break;
		  case HoCircleType:
			  {
				  StrTemp1.Format("%d",HoCircleType);
				  WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				  StrTemp.Format("ShapeParam%d_Row",i);
				  StrTemp1.Format("%.6lf",RoiParam.stcircle.row);
				  WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				  StrTemp.Format("ShapeParam%d_Col",i);
				   StrTemp1.Format("%.6lf",RoiParam.stcircle.col);
				   WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
				  StrTemp.Format("ShapeParam%d_Rii",i);
				   StrTemp1.Format("%.6lf",RoiParam.stcircle.Radius);
				   WritePrivateProfileStringA(ProcIndexName,StrTemp, StrTemp1, m_ConfigPath);
			  }
			  break;

		  }
	  }
}


 BOOL VisionProcBase::VisionProcRun(CHECK_RESULT& ProcRes)
 {


	 return TRUE;

 }


//*****************************************   场景的类 ***************************************//

 ProcList:: ProcList()
 {
	 m_RelativeSceneId = -1;
	 m_NextScenenId = -1;
	 m_NextSceneRotAngle = 0.0;
 }




void ProcList::AddProc(VisionProcBase* pProc)
{
	m_ProcList.Add(pProc);

}
void ProcList:: InsertProc(int Index,VisionProcBase* pProc)
{
	m_ProcList.InsertAt(Index,pProc);
}
void ProcList:: RemoveProc(int Index)
{
	
	if (Index >=m_ProcList.GetCount())
	{
		return ;
	}
	m_ProcList.RemoveAt(Index);
}
void  ProcList::delProc(int Index)
{
	if (Index >=m_ProcList.GetCount())
	{
		return ;
	}

	m_ProcList.GetAt(Index)->DeleteSelf();
	//delete m_ProcList.GetAt(Index);
	RemoveProc(Index);
	
}
int  ProcList::GetProcCount()
{
	return (int)m_ProcList.GetCount();
}

int ProcList::GetProcIndex(int ProcID)
{
	for (int i = 0;i< m_ProcList.GetCount();i++)
	{
		if (ProcID == m_ProcList.GetAt(i)->GetProcId())
		{
			return i;
		}
	}
	return -1;

}

VisionProcBase* ProcList::GetProcByIndex(int Index)
{
	if (Index<0)
	{
		return NULL;
	}
	if (Index<m_ProcList.GetCount())
	{
	     return m_ProcList.GetAt(Index);
	}else
	{
		return NULL;
	}
}


VisionProcBase* ProcList::GetProcByTypeIndex(int ProcTypeID,int TypeIndex)
{
	int Index = -1;
	for (int i = 0;i<m_ProcList.GetSize();i++)
	{
		if (ProcTypeID == m_ProcList.GetAt(i)->m_ProcTypeId)
		{
			Index++;
			if ( Index == TypeIndex)
			{
				return m_ProcList.GetAt(i);
			}
		}

	}
	return NULL;

}

int ProcList::GetSceneID()
{
	return m_SceneID;
}
void ProcList::SetSceneID(int SceneID)
{
	m_SceneID = SceneID;
}

int  ProcList::GetProcType(int Index)
{
	if (Index >=m_ProcList.GetCount())
	{
		return -1;
	}
	return m_ProcList.GetAt(Index)->m_ProcTypeId;
}

int ProcList::GetTypeIndex(int ProcType,void* pProc)
{
	int Index = -1;
	for (int i = 0;i<m_ProcList.GetCount(); i++)
	{
		if (GetProcType(i) == ProcType)//同类型的第几个 如采集图片有两个,则有可能是012
		{
			Index++;
			if (GetProcByIndex(i) ==  pProc)
			{
				return Index;
			}
		}
	}
	return -1;
}

BOOL ProcList::RunSceneSingle(CHECK_RESULT& ProcRes)
{
	int LoopCount = 0 ,LoopBeginIndex = 0,LoopEndIndex = 0;
	for (int i  = 0;i<m_ProcList.GetCount(); i++)
	{
		if (LOGIC_CTR == m_ProcList.GetAt(i)->m_ProcTypeId)
		{
			LoopCount = ((SceneLogic*)m_ProcList.GetAt(i))->GetLoopTimes();
			LoopBeginIndex = ((SceneLogic*)m_ProcList.GetAt(i))->GetLoopBeginIndex();
			LoopEndIndex =  ((SceneLogic*)m_ProcList.GetAt(i))->GetLoopEndIndex();
		}else
		{
			 LoopCount = 1;
             LoopBeginIndex = i;
			 LoopEndIndex = i;
		}

		for (int j = 0;j< LoopCount;j++)
		{
		    for (int k = LoopBeginIndex;k<=LoopEndIndex;k++ )
		    {
				if (!m_ProcList.GetAt(k)->VisionProcRun(ProcRes))
				{
					return FALSE;
				}else
				{
					//每一步执行完后进行的其他处理可在此添加

				}
				i = k;  //i从循环结束后的位置开始执行。
			}
		}
	}
	return TRUE;

}
int ProcList::GetProcImageIndex(int ProcId)
{
	int ImageIndex = -1;
	for (int i = 0;i<m_ProcList.GetCount();i++)
	{
		if (IMAGE_ACQISITION == m_ProcList.GetAt(i)->m_ProcTypeId) //||IMAGE_READ == m_ProcList.GetAt(i)->m_ProcTypeId
		{
			ImageIndex++;
		}
		if (ProcId == m_ProcList.GetAt(i)->GetProcId()) //先找到这个场景里的图像采集索引,直到找到最前面一项，就找到了
		{
			break;
		}
	}
	return ImageIndex;
}

 void  ProcList::ReOrderIndex()
 {
	 for (int i = 0;i<m_ProcList.GetCount();i++)
	 {
		 m_ProcList.GetAt(i)->m_ProcNo = i;
	 }
 }

 void ProcList::SetAxeIndex(int Index)
 {
	 m_AxeIndex = Index;
	 
 }

 void ProcList::SetToolIndex(int Index)
 {
	  m_ToolIndex = Index;
 }

 void ProcList::SetRelaSceneID(int RelativeId)
 {
	 m_RelativeSceneId = RelativeId;
 }
 int ProcList::GetRelaSceneID(void)
 {
	 return m_RelativeSceneId;
 }

 void ProcList::SetNextSceneID(int NextSceneId)
 {
	 m_NextScenenId = NextSceneId ;
 }

 int  ProcList::GetNextSceneID(void)
 {
	 return m_NextScenenId;
 }

 void  ProcList::SetNextRotAngle(double RotAngle)
 {
	 m_NextSceneRotAngle = RotAngle;
 }
 double  ProcList::GetNextRotAngle(void)
 {
	 return m_NextSceneRotAngle;
 }



 //*****************************************   场景链的类 ***************************************//

void SceneList::AddScene(ProcList* pScene)
{
	m_SceneList.Add(pScene);

}

ProcList* SceneList::GetSceneBySceneID(int SceneID)
{

	for (int i = 0;i<m_SceneList.GetCount();i++)
	{
		if ( m_SceneList.GetAt(i)->GetSceneID() == SceneID)
		{
			return  m_SceneList.GetAt(i);
		}
	}
	return NULL;

}

ProcList*  SceneList::GetSceneByIndex(int SceneIndex)
{
	if (SceneIndex >=m_SceneList.GetCount())
	{
		return NULL;
	}
	return m_SceneList.GetAt(SceneIndex);
}



int SceneList::GetSceneCount()
{
	return (int)m_SceneList.GetCount();
}


int SceneList::GetIndexBySceneID(int SceneID)
{
	for (int i = 0;i<m_SceneList.GetCount();i++)
	{
		if ( m_SceneList.GetAt(i)->GetSceneID() == SceneID)
		{
			return i;
		}
	}
	return -1;
}

	

