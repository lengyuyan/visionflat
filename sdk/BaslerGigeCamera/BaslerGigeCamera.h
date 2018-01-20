// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BASLERGIGECAMERA_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BASLERGIGECAMERA_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
// 



#ifdef BASLERGIGECAMERA_EXPORTS
#define BASLERGIGECAMERA_API __declspec(dllexport)
#else
#define BASLERGIGECAMERA_API __declspec(dllimport)
#endif

#pragma once
#include "stdafx.h"

#define  ICAMERAEXPORT 1
#include "ICamera.h"

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;


#define  CAMFULLNAME_LENTH_MAX  512
#define  FILENAME_LENTH_MAX  512

using namespace Pylon;




#ifdef BASLERGIGECAMERA_EXPORTS
#define BASLERGIGECAMERA_API __declspec(dllexport)
#else
#define BASLERGIGECAMERA_API __declspec(dllimport)
#endif

// 此类是从 BaslerGigeCamera.dll 导出的
class BASLERGIGECAMERA_API CBaslerGigeCamera:public ICamera {
public:
	CBaslerGigeCamera(void);
	CBaslerGigeCamera(char*camerName);
	// TODO: 在此添加您的方法。
	~CBaslerGigeCamera();

public:
	
#define MSG_DISP_IMAGE  1124    

	enum SynMode
	{
		NoMode = 0 ,		
		SynchroGrab =1 ,
		AsynchroGrab = 2

	};
public:  //抽象函数

	virtual bool InitCam(char* CamerName) ;//初始化相机
	virtual bool UnInitCam() ;

	virtual int GetImageWidth();				// 获取图片宽度
	virtual int GetImageHeight() ;				// 获取图片高度

	//virtual void SetWindowID() = 0;
	// 设置回调函数, Index-相机号，pWnd-目标窗口指针
	virtual void SetCallBack(void(*Callback)(void* context), void* pWnd) ;	 
	virtual void SetBuffer(BYTE* pBuffer) ;		// 设置图像缓冲区

	virtual bool SingleSnap(bool bDisplay = true) ;	// 单帧采集 // 使用的是同步采集，不使用回调函数

	virtual bool StartSnap() ;	// 连续采集
	virtual bool StopSnap() ;	// 停止采集	

	virtual bool SetTriggerMode(bool Mode);		//设置触发模式	
	virtual bool SetExposureTime(int iValue) ;	//设置曝光时间	
	virtual bool SetGain(int iValue) ;			//设置增益

	virtual int GetExposureTime() ;		//获取曝光时间
	virtual int GetGain() ;				//获取增益
	virtual char* GetFullName();

	virtual int GetParameInfo(ICAMERA_PARAMETER speciPara,unsigned long &info);///AVT中的参数
	virtual int SetParameInfo(ICAMERA_PARAMETER speciPara,int val);





public:
	//初始化驱动工厂
	static void IniTrlFactory();

	//设置相机同步异步模式
	void SetSynchroMode(SynMode mode);

	//按照找到的相机的索引，得到相应相机的用户定义名称
	char* GetDeviceUserName(int FindCamIndex);	

	//根据相机的索引，得到相应相机的驱动名称
	char* GetDeviceTypeStr(int FindCamIndex);
	//根据相机索引，得到相机驱动的序列号
	char* GetDeviceSerialNo(int FindCamIndex);
	//根据相机索引，得到相机驱动友好名字
	char* GetDeviceFriendlyName(int FindCamIndex);
	//按照找到的相机的索引，获取相机全名
	//此函数还可以获取相机总索引
	static char* GetCamFullName(int& FindCamIndex);

	//获取相机数量
	static int GetCamCount();

	//根据相机全名称，得到相应相机索引
	int  GetDeviceIndexByFullName(char* DeviceFullName);

	//根据相机的索引，得到相应相机的驱动类型信息
	CDeviceInfo GetDeviceType(int FindCamIndex);

	//根据相机的索引，设定用户定义名称

	//根据相机索引，创建并打开相机对象
	BOOL OpenModeCam(int CamIndex,SynMode Mode,EGrabStrategy strategy);
	//关闭相机
	BOOL CloseBaslerCam();


	//获取相机当前曝光时间
	double GetBaslerExposureTime();
	//获取最大曝光时间
	double GetMaxExposureTime();
	//获取最小曝光时间
	double GetMinExposureTime();
	//设置相机曝光时间,单位微秒
	BOOL SetBaslerExposureTime(UINT MilSeconds);

	//获取相机当前增益
	double GetCamGain();
	//获取相机最大增益
	double GetMaxGain();
	//获取相机最小增益
	double GetMinGain();
	//设置相机增益
	BOOL SetCamGain(double GainValue);

	//获取图像设定的当前宽度
	int64_t GetWidth();
	//设定图像宽度
	BOOL SetWidth(int64_t width);
	//获取图像设定最大宽度
	int64_t GetMaxWidth();
	//获取图像设定最小宽度
	int64_t GetMinWidth();

	//获取图像设定的当前高度
	int64_t GetHeight();

	//设定图像宽度
	BOOL SetHeight(int64_t width);
	//获取图像设定最大高度
	int64_t GetMaxHeight();
	//获取图像设定最小高度
	int64_t GetMinHeight();

	//获取图像X偏置
	int64_t GetOffsetX();
	//获取X最大偏置
	int64_t GetMaxOffsetX();
	//获取X最小偏置
	int64_t GetMinOffsetX();
	//设定图像X偏置
	BOOL SetOffsetX(int64_t offsetX);

	//获取图像Y偏置
	int64_t GetOffsetY();
	//获取Y最大偏置
	int64_t GetMaxOffsetY();
	//获取Y最小偏置
	int64_t GetMinOffsetY();
	//设定图像Y偏置
	BOOL SetOffsetY(int64_t offsetY);

	//获取相机参数总数量
	int64_t GetCamParamNum();

	// 获取触发延迟 ****无效函数***
	double GetTriggerDelay();
	//设定触发延迟  ****无效函数***
	BOOL SetTriggerDelay(double DelayValue);

	//保存相机当前的参数到文件
	BOOL SaveParamToFile(char* FileFullName);
	//从文件中读取参数到相机
	BOOL LoadParamFromFile(char*  FileFullName);


	//判断相机是否打开
	BOOL IsCamOpen();

	//设定图像抓取策略
	void SetGrabStrategy(EGrabStrategy strategy); //GrabStrategy_OneByOne   c_countOfImagesToGrab   GrabStrategy_LatestImageOnly
	// 获取抓取策略
	EGrabStrategy GetGrabStrategy();
	
	//同步方式准备抓取
	BOOL BeginGrab(); 
	//采用同步方式抓取单次图像
	BOOL BaslerCamGrab();

	//准备用回调的方式准备抓取，以下两个回调函数都要实现，第一个回调将数据格式转换成BYTE*并传给SetBuffer的参数。
	//typedef void (*pCallbackGenBuffer)(BYTE* pBffer, CGrabResultPtr m_ptrGrabResult);  //转换图像数据m_ptrGrabResult类型为BYTE*,为了满足
	                                                    //继承的void SetBuffer(BYTE* pBuffer)虚函数。
	typedef void (*pCallbackFunc)(void* pCamera);  //将本类对象指针传入，在实现函数时，得到成员变量图像数据指针，并生成图像给到m_pBuffer指向的对象。


	BOOL BeginAsynchGrab();//(void* pGrabParent, pCallbackFunc );

	//采用回调的方式抓取图像
	BOOL AsynchGrab();


	//结束回调方式的抓取线程；
	void StopAsynchGrab();

	//static int GetMaxCamNum(void);
	static int GetUnOpenIndex(void);

	//根据相机索引，创建并打开相机对象
	BOOL CreateOpenBaslerCam(int  CamIndex,EGrabStrategy strategy = GrabStrategy_OneByOne);

	//根据相机索引，创建并打开相机对象
	BOOL OpenBaslerCam(int  CamIndex,EGrabStrategy strategy = GrabStrategy_OneByOne);
	//关闭相机
protected:
	//判断给定的索引是否合法
	BOOL IsAvailabCamIndex(int CamIndex);

	

private:
    static void GetDeviceInfoList(DeviceInfoList_t& DeviceList);
	//GetDeviceInfoList();


public:  //采集图像后的处理
	//获取采集的图像宽度
	long GetImageAcqWidth();
	//获取采集的图像高度
	long GetImageAcqHeight();

public:
	static BOOL  IsIniTrlFactory;//是否初始化库
	static int  CameraNum;//是否初始化库
	static DeviceInfoList_t EnumDeviceList; //驱动信息，工厂每次调用序列化EnumerateDevices的结果是不一样的,
	BOOL  IsOpenCam;  // 相机是否打开
	BOOL IsReadyGrab; // 准备抓图
	BOOL m_IsGrab;   //图像是否采集成功
	BOOL m_StopGrab; // 确定是否停止采集

	EGrabStrategy m_GrabStrategy; //图像采集策略 GrabStrategy_OneByOne   c_countOfImagesToGrab   GrabStrategy_LatestImageOnly
	BOOL  m_IsContinuGrab;   // 是否连续采集，连续采集时不受控制，一直采集，其策略是GrabStrategy_OneByOne。
	
	
	//采集到的图像的数据指针，用以生成图像;
	CGrabResultPtr m_ptrGrabResult;
	BYTE* m_pBuffer;

	int  m_CamIndex;  //当前打开的相机序号
	char m_CamerName[CAMFULLNAME_LENTH_MAX];
    

	CInstantCamera camera;  //  CInstantCamera  //Camera_t
	BOOL IsBeginGrab;
	IPylonDevice* m_pDevice;
	
	SynMode m_SynchroMode;  //图像采集的同步异步模式

	static HANDLE Basler_Mutex;
	
	//static int OpendIndex;


	static bool m_CamOpenedMap[64];


public:
	HANDLE     m_hGrabThreadRun;	//
	//pCallbackGenBuffer m_pGenBuffer;
	pCallbackFunc m_pCallBack;
	void* m_pGrabParent;



	static DWORD WINAPI BalerCamGetDevice(void* pThis);

	//
	static DWORD WINAPI GrabThread(void* pThis);







};

//Basler相机库使用前需要处理的事务，使用库时必须调用。
void BaslercamStartUp(void); 
//Basler相机库卸载需要处理的事务
void BaslercamRelease(void); 


extern BASLERGIGECAMERA_API int nBaslerGigeCamera;

BASLERGIGECAMERA_API int fnBaslerGigeCamera(void);

