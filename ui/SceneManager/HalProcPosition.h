#pragma once
#include "visionprocbase.h"
#include "HalProcPosSetting.h"

class HalProcPosition :
	public VisionProcBase
{
public:
	HalProcPosition(void);
    HalProcPosition(CString ConfigPath,int ProcID);
	~HalProcPosition(void);

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
public:
	bool CommonLocate(CString pathName,double (*inParam)[12],double& PosX,double& PosY,double& PosPhi);


public:

	HalProcPosSetting m_HalCommomPosSet;




};

