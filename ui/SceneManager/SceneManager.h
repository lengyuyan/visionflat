// SceneManager.h : SceneManager DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "VisionProcBase.h"

// CSceneManagerApp
// 有关此类实现的信息，请参阅 SceneManager.cpp
//

class CSceneManagerApp : public CWinApp
{
public:
	CSceneManagerApp();

// 重写
public:
	virtual BOOL InitInstance();

public:
	
	DECLARE_MESSAGE_MAP()
};

void BuildProc(VisionProcBase** ppProcBase,CString configPath,int ProcType,int SceneID,int ProcIdBase);

extern "C"  void PASCAL EXPORT InitGrabWay2Sceneimage(int SceneID,bool sendflag);

extern "C"  BOOL  SceneVisionProcess(int iSceneID,CHECK_RESULT& ProcRes);
//LIBRARY
//extern  CArray<CArray<VisionProcBase*>*> g_SceneList;
extern SceneList g_SceneList;
