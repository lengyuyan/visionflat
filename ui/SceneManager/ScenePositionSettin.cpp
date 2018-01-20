// ScenePositionSettin.cpp : 实现文件
//
#pragma once

#include "stdafx.h"
#include "SceneManager.h"
#include "ScenePositionSettin.h"
#include "ScenePositon.h"
#include "afxdialogex.h"


// ScenePositionSettin 对话框

IMPLEMENT_DYNAMIC(ScenePositionSettin, CDialogEx)

ScenePositionSettin::ScenePositionSettin(CWnd* pParent /*=NULL*/)
	: CDialogEx(ScenePositionSettin::IDD, pParent)
{

	m_MatchIndex = -1;


}

ScenePositionSettin::~ScenePositionSettin()
{
}

void ScenePositionSettin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCENEPOSITION_COORDINATEWAY, m_CoordinateType);
	DDX_Control(pDX, IDC_SCENEPOSITION_ORIGIN_X, m_OriginalX);
	DDX_Control(pDX, IDC_SCENEPOSITION_ORIGIN_Y, m_OriginalY);
	DDX_Control(pDX, IDC_SCENEPOSITION_ORIGIN_A, m_OriginalA);
	DDX_Control(pDX, IDC_SCENEPOSITION_SELMATCH, m_CMatchSel);
}


BEGIN_MESSAGE_MAP(ScenePositionSettin, CDialogEx)
	ON_BN_CLICKED(IDC_SCENEPOSITION_COMMIT, &ScenePositionSettin::OnBnClickedScenepositionCommit)
	ON_BN_CLICKED(IDC_SCENEDLG_POSITION_SEARCHCONTOUR, &ScenePositionSettin::OnBnClickedScenedlgPositionSearchcontour)
	ON_CBN_SELCHANGE(IDC_SCENEPOSITION_COORDINATEWAY, &ScenePositionSettin::OnCbnSelchangeScenepositionCoordinateway)
	ON_BN_CLICKED(IDC_SCENEPOSITION_CANCEL, &ScenePositionSettin::OnBnClickedScenepositionCancel)
	ON_CBN_DROPDOWN(IDC_SCENEPOSITION_SELMATCH, &ScenePositionSettin::OnCbnDropdownScenepositionSelmatch)
	ON_CBN_SELCHANGE(IDC_SCENEPOSITION_SELMATCH, &ScenePositionSettin::OnCbnSelchangeScenepositionSelmatch)
END_MESSAGE_MAP()


// ScenePositionSettin 消息处理程序
void ScenePositionSettin::CloseSettingDlg()
{
	OnOK();
}



void ScenePositionSettin::OnCbnSelchangeScenepositionCoordinateway()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD_PTR TypeDate =   m_CoordinateType.GetItemData(m_CoordinateType.GetCurSel());
	m_CoordType =(coordinateType)TypeDate;

}

void ScenePositionSettin::OnCbnDropdownScenepositionSelmatch()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTemp;
	int CurProcIndex = ((ScenePositon*)m_Pcontainer)->GetProcIndexById(((ScenePositon*)m_Pcontainer)->GetProcId());
	for (int i = 0; i<m_CMatchSel.GetCount();i++)
	{
		m_CMatchSel.DeleteString(0);
	}
	
	for (int i =0,k = 0; i<=CurProcIndex;i++)
	{
		 if( MODEL_PRO == ((ScenePositon*)m_Pcontainer)->GetCurScene()->GetProcType(i))
		 {
			 strTemp.Format("第%d个匹配_序号%d",k,i);
			 m_CMatchSel.InsertString(k,strTemp);
			 m_CMatchSel.SetItemData(k,k);  //设置数据值为 匹配序号
			 k++;
		 }
	}


}


void ScenePositionSettin::OnCbnSelchangeScenepositionSelmatch()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_CMatchSel.GetCurSel()>=0)
	{
	     m_MatchIndex = m_CMatchSel.GetItemData(m_CMatchSel.GetCurSel());
	}else
	{
		 m_MatchIndex = -1;
	}
}



void ScenePositionSettin::UpdateUI()
{
	m_CoordinateType.InsertString(0,"点到点");
	m_CoordinateType.SetItemData(0,Point_To_Point);
	m_CoordinateType.InsertString(0,"点到直线");
	m_CoordinateType.SetItemData(0,Point_To_Line);
	m_CoordinateType.InsertString(0,"直线到直线");
	m_CoordinateType.SetItemData(0,Line_To_Line);
	
	//当前模式设为点到点模式。
	for (int i = 0 ; i<m_CoordinateType.GetCount(); i++)
	{
		if (Point_To_Point == m_CoordinateType.GetItemData(i))
		{
		    m_CoordinateType.SetCurSel(i);
			m_CoordType = Point_To_Point;
		}
	}
	m_OriginalX.SetWindowTextA("0.0");
	m_OriginalY.SetWindowTextA("0.0");
	m_OriginalA.SetWindowTextA("0.0");


}

void  ScenePositionSettin::GetContainer(void* Pcontainer)   //建立的初始化可在这个函数中写
{
	m_Pcontainer = Pcontainer;
	m_iCurSceneID = ((ScenePositon*)Pcontainer)->GetSceneId();
	//m_iCurIndex = ((ScenePositon*)Pcontainer)->get

}

void ScenePositionSettin::SetDisplayWindowID(int winID)
{
	m_winID = winID;
}

void ScenePositionSettin::SetCoordinate()
{
	double K,phi,B;
	int ElemNum = m_CoordinatePoint.GetCount();
	
	switch (m_CoordType)
	{
	case Point_To_Point:
		{
			if (ElemNum<2)
			{
				AfxMessageBox("点-点系缺少点");
				return;
			}
			//原点为第一个点，根据第二个点调用算法计算出角度，赋值给
			
			MyFunClass::Point2ToLine(m_CoordinatePoint.GetAt(ElemNum-2).x,m_CoordinatePoint.GetAt(ElemNum-2).y,
				        m_CoordinatePoint.GetAt(ElemNum-1).x, m_CoordinatePoint.GetAt(ElemNum-1).y,K,phi,B);	
			m_Coodinate.OriginalX = m_CoordinatePoint.GetAt(ElemNum-2).x;
			m_Coodinate.OriginalY = m_CoordinatePoint.GetAt(ElemNum-2).y;
			m_Coodinate.OriginalA = phi;
			m_CoordinatePoint.RemoveAll();
		}
		break;
	case Point_To_Line:
		{
			if (ElemNum<1)
			{
				AfxMessageBox("点-线系缺少点或线");
				return;
			}
		}
		break;


	}

}


void ScenePositionSettin::ReadProcConfig(CString ConfigPath,int ProcIndex)
{
	char szValue[256]={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);
	m_CoordType = (coordinateType)GetPrivateProfileInt(ProcIndexName,"CoordinateType",-1,ConfigPath);
	m_MatchIndex = GetPrivateProfileInt(ProcIndexName,"MatchIndex",-1,ConfigPath);



}
void ScenePositionSettin::SaveProcConfig(CString ConfigPath,int ProcIndex)
{
	char szValue[256]={0};
	char szName[256] ={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);
	//存储模板类型
	memset(szName,0,sizeof(szName));
	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%d",m_CoordType);
	WritePrivateProfileStringA(ProcIndexName, "CoordinateType",szValue, ConfigPath);

	memset(szValue,0,sizeof(szValue));
	sprintf_s(szValue,"%d",m_MatchIndex);
	WritePrivateProfileStringA(ProcIndexName, "MatchIndex",szValue, ConfigPath);
	// 存放绘图ID


}
BOOL ScenePositionSettin::VisionProcRun( CHECK_RESULT& ProcRes)
{
	//将选择的匹配号传入视觉处理函数，函数中先对拟合图形仿射变换，再拟合；如果匹配号为负数（无效值），则直接拟合定位。
	//if (m_MatchIndex >= 0)
	double row,col,Rii;

    ((ScenePositon*)m_Pcontainer)->PosFitCircle(m_MatchIndex,row,col,Rii);
	
	//将 拟合的数据变换到建立的坐标系中

	  //......


	return TRUE;
}


void ScenePositionSettin::OnBnClickedScenedlgPositionSearchcontour()
{
	// TODO: 在此添加控件通知处理程序代码
	 double row,col,Rii;
	 char szValue[256]={0};
	 ((ScenePositon*)m_Pcontainer)->AddCurShapeId();

	if( !((ScenePositon*)m_Pcontainer)->PosFitCircle(m_MatchIndex,row,col,Rii))
	{
		AfxMessageBox("轮廓匹配失败");
		return;
	} 
	 //将圆心放入m_CoordinatePoint向量中,后续使用时，为安全起见只使用最后两个元素。
	 DoublePoint p1(col,row);
	 m_CoordinatePoint.Add(p1);
	 memset(szValue,0,sizeof(szValue));
	 sprintf_s(szValue,"%.6lf",col);
	  m_OriginalX.SetWindowTextA(szValue);
	 memset(szValue,0,sizeof(szValue));
	 sprintf_s(szValue,"%.6lf",row);

	 m_OriginalY.SetWindowTextA(szValue);



}

void ScenePositionSettin::OnBnClickedScenepositionCommit()
{
	// TODO: 在此添加控件通知处理程序代码
	SetCoordinate();

}


void ScenePositionSettin::OnBnClickedScenepositionCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	((ScenePositon*)m_Pcontainer)->DelCoordinateShapeId();
	m_CoordinatePoint.RemoveAll();


}


