#pragma once
#include "VisionProcBase.h"
#include "ScenePreProSetting.h"


class ScenePrePro :public VisionProcBase
{
public:
	ScenePrePro(void);
	ScenePrePro(CString ConfigPath,int ProcID);
	//SceneMatchModel(CWnd* pParentWnd);
	~ScenePrePro(void);

public:
	virtual void BuildInstance(void);
	virtual void InitInstance(CWnd* pParentWnd);
	//virtual BOOL IsInitialConfigWnd();
	virtual  void DeleteSelf();

	virtual void HideConfigWnd();
	virtual void ShowConfigWnd();

public:
	virtual void SaveProcConfig(int ProcIndex);
	virtual void ReadProcConfig(int ProcIndex);
	virtual BOOL VisionProcRun(CHECK_RESULT& ProcRes); //执行视觉的处理
	
	//供setting调用
	bool ImageEnhancement(double& multinum,double& addnum);


	
public:
	
	 ScenePreProSetting  m_ScenePreProSetting;
	
};

