#pragma once


#include "DataType.h"
#include "iManagerCtrl.h"
#include "MyFunction.h"
#define  SCENE_PROC_ID_FACTOR 10000
/*
 *这个类是视觉处理各种方法的基类，后续的 视觉处理模块都继承自这个类来建立动态库，而在本程序中，该类为本程序提供基本的处理接口，接口输入参数
 *是图像，以及各种可调参数；输出参数是处理后的结果，包含坐标、区域、数字、等。
 *
 **/
class ProcList;

class  VisionProcBase
{
public:
	VisionProcBase(void);
	VisionProcBase(CString ConfigPath,int ProcID);
	~VisionProcBase(void);
public:
	enum ProcResultType
	{
		//现场用的是标定像素坐标,不带角度的相对坐标，图像坐标
		NOMAL_RESULT=0, //
		REAL_UNIT_RESULT =1,//使用机械单位，同时处理角度和位置偏移
		NO_MATCH_ROT = 2,   //匹配后不处理旋转，只处理位置偏移
		MODLE_DISTENCE,    //绝对坐标
		TEST_REPOSITION,   //测试重复定位  IMAGE_RELA   图像坐标标定，相对坐标
		IMAGE_ABS,       //图像坐标标定，绝对坐标
	};

public:
    //接口函数
	virtual void VisionProc(...);									//暂时没用
	virtual void BuildInstance(void) ;								// 只创建对象用于运行，不初始化界面 ，此函数应该可以不为虚函数。
	virtual void InitInstance(CWnd* pParentWnd);					//创建对象，并初始化界面，用于人机交互。
	virtual BOOL IsInitialConfigWnd();								//是否是初始化窗口,有个全局g_SceneInitialCount跟成员m_SceneInitialCount，不等就是初始化的
	virtual void HideConfigWnd()=0;									//判断是否隐藏配置窗口
	virtual void ShowConfigWnd() = 0;								//判断是否显示本身对应配置项的设置窗口
	virtual void SaveProcConfig(int ProcIndex);						//保存该处理到场景配置文件的ProcIndex子项位置
	virtual void ReadProcConfig(int ProcIndex);						//读取参数
	virtual BOOL VisionProcRun(CHECK_RESULT& ProcRes);				//每个处理项的核心运行函数
	CString m_ConfigPath;											//处理项场景路径
	void ReadAndCreateRect(CString ProcIndexName,int ShapeIndex);	//读屏创建一个矩形
	void ReadAndCreateCircle(CString ProcIndexName,int ShapeIndex); //ProcIndexName标识第几个处理项目,读并创建一个圆
	int GetProcIdBase();											//获取处理项索引
	int GetProcId();												//获取处理项ID
	int GetProcTypeID();											//处理类项类型
	virtual void DeleteSelf();										//删除处理类本身体
	int GetSceneId();												//处理子项ID/10000;
	int GetProcIndexById(int ProcID);								//根据ID来获取索引
	int GetProcTypeIndex();											//跟据ID找到他的索引,然后根据索引找到本身在相同类型中的索引
	ProcList* GetCurScene();										//找到本身所在的处理表
	void AfterCreateComb();											//清空图元id列表 组合id列表,可以放在模板类中
	CWnd* m_pParentWnd;												//场景调试面板上的设置框,上放处理项目对应的设置类
	PROCESS_TYPE m_ProcTypeId;										//处理的类型；
    int m_VisonProcId;												// 视觉处理本身的标识 ID = 场景号*10000+处理索引;
	int m_ProcNo;													//这个序号在动态增、删时不太好维护，只能用在读取配置文件的时候，所以最好永远不使用该成员变量。
																	//如果要使用，则一定要在任何改动（增、删、移）时，对整个场景所有处理模块重新更新:ReOrderIndex()后才能使用。	                 
    static int VisionProcIdCount;									//ID计数器，暂未用到;
	int m_RoiID;													//本处理唯一的ROI的ID     总id
	vector<int> m_RoiIdList;										//ROI组成的基本图元的ID  List   子图元id
	vector<int> m_RoiComIdList;										//ROI基本图元的组合方法ID list; 类型
	//vector<int> m_ShapeIdList;									//基本图元的ID，用在一些需要的场合。
	int m_SceneInitialCount;										//判断是否打开场景窗口
	ProcResultType m_ResultType;									//确定处理结果 这个要放在模板子类去，模板处理时要发送的坐标类型
protected:
	int m_CurSceneCamIndex; //这个要放在采集子类里去
};



//  **************************  场景的类  ******************//

class ProcList
{
public:
  ProcList();
public:	
	void AddProc(VisionProcBase* pProc);							//后插一个处理类
	void InsertProc(int Index,VisionProcBase* pProc);				//在指定索引之后插入一个索引类 
	void RemoveProc(int Index);										//移除指定的处理类,但是不会delete处理类的内存及析构函数等，慎用
	void delProc(int Index);										//删除指定处理类及析构
	int GetProcCount();												//获取处理类数量
	int GetProcIndex(int ProcID);									//处理类有一个ID=场景号*10000+处理类中的位置(从0开始)，这里根据处理类id来获取他在处理中的索引,返错-1
	int GetProcType(int Index);										//根据索引来获取这个处理类的类型,如采集,模板
	VisionProcBase* GetProcByIndex(int Index);						//根据索引来获取处理类
	VisionProcBase* GetProcByTypeIndex(int ProcTypeID,int TypeIndex);//获取对应类型的对应索引处理类
	int GetSceneID();												//每个处理类会有一个场景号
	void SetSceneID(int SceneID);									//设置场景号
	BOOL RunSceneSingle(CHECK_RESULT& ProcRes);						//核心函数,运行场景表中的所有处理项,成功返回ture,失败返回false
	int GetTypeIndex(int ProcType,void* pProc);						//获取指定类型和指定处理类指针 在处理表中的索引
	int GetProcImageIndex(int ProcId);								//获取指定处理类ID前面的最靠近的那项采集类的索引
    void ReOrderIndex();											//慎用 每个子项重新排序的一个序号,如删除一项后,他们中的索引要变化
	void SetAxeIndex(int Index);									//慎用
	void SetToolIndex(int Index);									//慎用
	void SetRelaSceneID(int RelativeId);							//慎用 	设置相关场景的ID
	int GetRelaSceneID(void);										//慎用 	获取相关场景的ID
	void SetNextSceneID(int NextSceneId);							//慎用 	设置下个场景的ID
	int GetNextSceneID(void);										//慎用 	获取下个场景的ID
	void SetNextRotAngle(double RotAngle);							//慎用 	设置场景的角度
	double GetNextRotAngle(void);									//慎用 	获取场景角度

private:
	CArray <VisionProcBase*> m_ProcList; 
	int m_SceneID;
	int m_AxeIndex;
	int m_ToolIndex;
	int m_RelativeSceneId; 
	int m_NextScenenId;    
	double m_NextSceneRotAngle;
};


//***********************   场景链的类 *********************//

class SceneList
{
public:
	void AddScene(ProcList* pScene);					//增加场景表到全局表
	int GetIndexBySceneID(int SceneID);					//获取对应场景id的处理表在全局表中的索引
	ProcList* GetSceneBySceneID(int SceneID);			//获取对应场景id对应处理表
	ProcList* GetSceneByIndex(int SceneIndex);			//获取对应索引的处理表
	int GetSceneCount();								//获取数量
private:
	CArray<ProcList*> m_SceneList;


};

