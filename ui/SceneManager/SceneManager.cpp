// SceneManager.cpp : 定义 DLL 的初始化例程。
//

#pragma once

#include "stdafx.h"
#include "SceneManager.h"
#include "SceneDebugDlg.h"


//定义可用的处理选项
#include "SceneAcqImage.h"    //图像采集处理
#include "ScenePrePro.h"   //图像预处理
#include "SceneMatchModel.h"  //匹配处理
#include "HalProcPosition.h"   //halcon通用定位处理
#include "HalProcProcess.h"   //halcon通用定位处理

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CSceneManagerApp

BEGIN_MESSAGE_MAP(CSceneManagerApp, CWinApp)
END_MESSAGE_MAP()


// CSceneManagerApp 构造


CSceneManagerApp::CSceneManagerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSceneManagerApp 对象

CSceneManagerApp theApp;


//CArray<CArray<VisionProcBase*>*> g_SceneList;
//CArray<ProcList*> g_SceneList;

// CSceneManagerApp 初始化

BOOL CSceneManagerApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



extern "C" void PASCAL EXPORT ShowSceneDebugDlg(int iLevels)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (1)
	{
		 CSceneDebugDlg dlg;
		 dlg.m_LoginLevels = iLevels;
		 dlg.DoModal();
	}else
	{
		CSceneDebugDlg* pdlg = new CSceneDebugDlg;
		pdlg->Create(IDD_DIALOG_SCENE_DEBUG);
		pdlg->m_LoginLevels = 3;
		pdlg->ShowWindow(TRUE);
	}


}

void BuildProc(VisionProcBase** ppProcBase,CString configPath,int ProcType,int SceneID,int ProcIdBase)
{
	*ppProcBase = NULL;
	switch (ProcType)
	{
	case  IMAGE_READ://VisionProcBase::READ_IMAGE:
		{		

		}
		break;
	case MAIN_PRO:
		{

		}
		break;
	case MODEL_PRO:
		{
			SceneMatchModel*  pMatchModel = new SceneMatchModel(configPath,SceneID*SCENE_PROC_ID_FACTOR + ProcIdBase);//m_pFunSettingWnd
			*ppProcBase = pMatchModel;
		}
		break;
	case THRE_PRO:
		{

		}
		break;
	case IMAGE_ACQISITION:   //图像采集
		{
			//处理对象,构造函数的参数为:场景路径,procID=场景ID*10000+procIDBase(处理对象中的索引)
			SceneAcqImage* pAcqImage = new SceneAcqImage(configPath,SceneID*SCENE_PROC_ID_FACTOR + ProcIdBase); 
			//处理ID如此设计，是为了方便从处理ID反推出场景ID SceneID。
			*ppProcBase = pAcqImage;
		}
		break;
	case IMAGE_PREPROCESS:   //图像预处理
		{
			//处理对象,构造函数的参数为:场景路径,procID=场景ID*10000+procIDBase(处理对象中的索引)
			ScenePrePro* pPreImage = new ScenePrePro(configPath,SceneID*SCENE_PROC_ID_FACTOR + ProcIdBase); 
			*ppProcBase = pPreImage;
		}
		break;
		
	case CONTOUR_POSITION:
		{
			ScenePositon* pPosition = new ScenePositon(configPath,SceneID*SCENE_PROC_ID_FACTOR + ProcIdBase);
			*ppProcBase = pPosition;
		}
		break;
	case HALPROC_POSITIONG:
		{
			HalProcPosition* pHalPosition  =  new HalProcPosition(configPath,SceneID*SCENE_PROC_ID_FACTOR + ProcIdBase)  ;
			*ppProcBase = pHalPosition;
		}
		break;
	case HALPROC_PROCESS:
		{
			HalProcProcess* pHalPosition  =  new HalProcProcess(configPath,SceneID*SCENE_PROC_ID_FACTOR + ProcIdBase)  ;
			*ppProcBase = pHalPosition;
		}
		break;
	default:
		break;
	}


}

//把场景包里的东西都读进来
extern "C"  void PASCAL EXPORT IniSceneData(const char* SceneConfigPath ,int SceneID)
{
	 AFX_MANAGE_STATE(AfxGetStaticModuleState());
	 //1,为该场景建立处理的链表，链表中放入每个处理的对象地址，建立处理对象但不初始化窗口。
	  ProcList* pScene= new ProcList;
	  pScene->SetSceneID(SceneID);
	  //场景赋值
	  char ProcIndexName[256]={0};
	  char stText[256]={0};

	 int pkgNum =  GetPrivateProfileInt("scene", "pkgnum", 0, SceneConfigPath);
	 GetPrivateProfileString("scene","NextSceneRotAngle","",stText,sizeof(stText),SceneConfigPath);
	 pScene->SetNextRotAngle(atof(stText));
	 pScene->SetRelaSceneID(GetPrivateProfileInt("scene", "LastSceneId", -1, SceneConfigPath));
	 pScene->SetNextSceneID(GetPrivateProfileInt("scene", "NextSceneId", -1, SceneConfigPath));

	 VisionProcBase* pVisionProc = NULL;
	 for (int i = 0 ; i <pkgNum ;i++)
	 {
		 pVisionProc = NULL;
		 sprintf(ProcIndexName,"%d",i);//配置文件里从[0]开始读
		 int ProcType = GetPrivateProfileInt(ProcIndexName, "ProcType", -1, SceneConfigPath);

		  BuildProc(&pVisionProc,SceneConfigPath,ProcType,SceneID,i);


		if( NULL != pVisionProc)
		{
			 pScene->AddProc(pVisionProc);	  
		}

	 }
	  if (0<pScene->GetProcCount())
	  {
		  g_SceneList.AddScene(pScene);
	  }
	  
}

void  PASCAL EXPORT  InitGrabWay2Sceneimage(int SceneID,bool sendflag)
{
	//找到拍照对象，拿到拍照参数，
	//g_SceneList.
	 VisionProcBase* pVisionProc = NULL;
	ProcList* pScene =  g_SceneList.GetSceneBySceneID(SceneID);
	
	if (!pScene)
	{
		return;
	}
	for (int i=0;i<pScene->GetProcCount();i++)
	{
		if(pScene->GetProcType(i) == IMAGE_ACQISITION ) 
		{
			pVisionProc = pScene->GetProcByIndex(i);
			break;  //只抓场景中的第一个处理类为采集的
		}
	}
	if (!pVisionProc)
	{
		return;
	}

	//调用管理类的拍照函数,初始化拍照没有设置曝光等，这里完全可以用一个拍照函数来代替，可以把设置的曝光设置进去
	//InitGrabSceneimage1(((SceneAcqImage*)pVisionProc)->m_CamIndex,SceneID);


	((SceneAcqImage*)pVisionProc)->ProcGrabImage(sendflag);


}


BOOL  SceneVisionProcess(int iSceneID,CHECK_RESULT& ProcRes)
{
	ProcList* pScene = g_SceneList.GetSceneBySceneID(iSceneID);
	if (pScene)
	{
		if (pScene->RunSceneSingle(ProcRes))//每个处理子项都会有个RunSceneSingle函数，顺序执行,失败了就不再执行后面的子项
		{
			//处理正确完成后的后续处理，结果数据在场景的每一步都会添加和相应修改，
			return TRUE;
		}else
		{
			return FALSE;
		}
		
	}else
	{
		//ProcRes.err = -1;
		return FALSE;
	}

}




