#pragma once

#include "VisionProcBase.h"
#include "SceneImageSetting.h"

class SceneAcqImage:public VisionProcBase
{
public:
	SceneAcqImage(void);
	SceneAcqImage(CString ConfigPath,int ProcID);
	~SceneAcqImage(void);

public:
	virtual void BuildInstance(void);  //不创建 m_SceneAcqImageConfig设定对话框;
	virtual void InitInstance(CWnd* pParentWnd);
	//virtual BOOL IsInitialConfigWnd();
	virtual void DeleteSelf();


	virtual void HideConfigWnd();
	virtual void ShowConfigWnd();

	//void GetInstance(int ProcId);// 

public:
	virtual void SaveProcConfig(int ProcIndex); 
	virtual void ReadProcConfig(int ProcIndex);
	virtual BOOL VisionProcRun(CHECK_RESULT& ProcRes); //执行视觉的处理


public:
	bool ProcGrabImage(bool sendflag);

    
public:

	int m_CamIndex;   //相机索引，与系统参数中的相机索引对应。
	CSceneImageSetting m_SceneAcqImageConfig;


	

};

