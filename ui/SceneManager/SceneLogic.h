#pragma once
#include "VisionProcBase.h"
#include "SceneLogicSetting.h"

class SceneLogic : public VisionProcBase
{
public:
	SceneLogic(void);
	SceneLogic(CString ConfigPath,int ProcNo);
	~SceneLogic(void);

public:
	virtual void BuildInstance(void);
	virtual void InitInstance(CWnd* pParentWnd);
    virtual  void DeleteSelf();
	virtual void HideConfigWnd();
	virtual void ShowConfigWnd();

public:
	virtual void SaveProcConfig(int ProcIndex);
	virtual void ReadProcConfig(int ProcIndex);
	virtual BOOL VisionProcRun(CHECK_RESULT& ProcRes); //执行视觉的处理

public:
	int GetLoopTimes();
	int GetLoopBeginIndex();
	int GetLoopEndIndex();

public:
	SceneLogicSetting m_LogicSetting;


};

