#pragma once
#include "VisionProcBase.h"
#include "SceneModelSetting.h"


class SceneMatchModel :public VisionProcBase
{
public:
	SceneMatchModel(void);
	SceneMatchModel(CString ConfigPath,int ProcID);
	~SceneMatchModel(void);

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
	//void GetInstance(int ProcId);// 
//	void ReadAndCreateRect(CString ProcIndexName,int ShapeIndex);
	//void ReadAndCreateCircle(CString ProcIndexName,int ShapeIndex);

	void ShowAllShape(void);
	//void ;
	void CreateModel(int Nevels,double AngleStart,double AngleExtent,int Contrast,double& ModRow,double ModCol,
		double& ModPhi);
	BOOL MatchModel(int PyramidLevel,int MatchNum, double& Score,bool IsBulid,double& row,double& col,double& phi);
	

public:
	//CString m_ModelName;
	char m_ModelName[256];
	
public:
	
	 CSceneModelSetting  m_SceneModelSetting;
	 double m_CalModRotAng;  //匹配位与 标定位的不一致角度。

	 
};

