/********************************************************
//海康威视GBE相机
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 MVS2GBECAMERA_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BASLERGIGECAMERA_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
*********************************************************/
#include "stdafx.h"


#include "ICamera.h"
/*
防止重复包含头文件后的重复定义，即多次包含只会定义一次
#ifndef MVS2GBECAMERA_H
#define MVS2GBECAMERA_H
#endif
*/
#ifndef MVS2GBECAMERA_H
#define MVS2GBECAMERA_H


#ifdef  MVS2GBECAMERA_EXPORTS        
#define ICAMERASPEC __declspec(dllexport)
#else
#define ICAMERASPEC __declspec(dllimport)
#endif

#define  CAMFULLNAME_LENTH_MAX  512
#define  FILENAME_LENTH_MAX  512

#include "MvCameraControl.h"


// 此类是从 MVS2GBECamrea.dll 导出的
class ICAMERASPEC MVS2GBECamera : public ICamera 
{
//数据
	public:
		typedef void (*fcallback)(void* context);
		fcallback m_fcallback;   //回调函数
		static bool m_isInitFlag; //是否初始化
		bool m_iscontinueGrab;  //是否连续采图
		bool m_isstopGrab;        //是否停止采图
		int  m_CamIndex;  //当前打开的相机序号
		char m_CamerName[CAMFULLNAME_LENTH_MAX];
		/*
		MV_ACQ_MODE_SINGLE        单帧模式
		MV_ACQ_MODE_MUTLI         多帧模式
		MV_ACQ_MODE_CONTINUOUS 持续采集模式
		*/
		MV_CAM_ACQUISITION_MODE m_GrabStrategy; //图像采集策略
		
		void* m_handle;            //设备句柄

		static MV_CC_DEVICE_INFO_LIST EnumDeviceList;   //设备信息列表

		MV_CC_DEVICE_INFO m_DeviceInfo;//设备信息
		MVCC_INTVALUE ImageWidth;    //图片宽度
		MVCC_INTVALUE ImageHeight;   //图片高度

		typedef struct 
		{
			MVCC_ENUMVALUE Mode;    //增益模式
			MVCC_FLOATVALUE Gain;   //增益
		}CAMERAGAIN;
		CAMERAGAIN CameraGain; 

		typedef struct 
		{
			MVCC_ENUMVALUE Mode;   //曝光模式
			MVCC_FLOATVALUE  Time; //曝光时间
		}CAMERAEXPOSURETIME;
		CAMERAEXPOSURETIME CameraExposureTime;     //相机曝光时间    

		MV_FRAME_OUT_INFO_EX* pFrameInfo;//图像数据的帧信息
		BYTE* m_pBuffer; //采集到的图像的数据指针，用以生成图像

		static HANDLE MVS2GBE_Mutex;
		HANDLE     m_hGrabEvent;	//采集图像事件
		void* m_pGrabParent;

		unsigned char    *p_Xmldata; //xml文件数据缓存地址，xml储存着相机的各种参数
	public:
		MVS2GBECamera();
		MVS2GBECamera(char*camerName_);
		~MVS2GBECamera();
		virtual bool InitCam(unsigned long Index) ;//初始化相机
		virtual bool UnInitCam() ;
		virtual int GetImageWidth();				// 获取图片宽度
		virtual int GetImageHeight() ;				// 获取图片高度
		//virtual void SetWindowID() = 0;
		// 设置回调函数, Index-相机号，pWnd-目标窗口指针
		virtual void SetCallBack(void(*Callback)(void* context), void* pWnd) ;	 
		virtual void SetBuffer(BYTE* pBuffer_) ;		// 设置图像缓冲区
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
		static int GetGuidList(list<string> &guidList,string &type);         //获取GUID列表
	//自定义函数
	public:
		static DWORD WINAPI ThreadGrading(LPVOID lpParamt); //采集图像的线程
		
};





#endif
