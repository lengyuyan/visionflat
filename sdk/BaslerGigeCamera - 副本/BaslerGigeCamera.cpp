// BaslerGigeCamera.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "BaslerGigeCamera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  ICAMERAEXPORT 1

#include <GenApi/Container.h>
#include <GenApi/INode.h>
#include <pylon/PylonIncludes.h>


using namespace std;
using namespace GenApi;



// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

//LPCRITICAL_SECTION lpBasler_critical_section;

//int CBaslerGigeCamera::OpendIndex = -1;

bool  CBaslerGigeCamera::m_CamOpenedMap[64]={0};

HANDLE CBaslerGigeCamera::Basler_Mutex;
int  CBaslerGigeCamera::IsIniTrlFactory = 0;

char  BaslerFullNameList[256][256]={0};
DeviceInfoList_t CBaslerGigeCamera::EnumDeviceList = (DeviceInfoList_t)0;
//Basler相机库使用前需要处理的事务，使用库时必须调用。
void BaslercamStartUp(void)
{
	//InitializeCriticalSection(lpBasler_critical_section);
	CBaslerGigeCamera::Basler_Mutex = CreateMutex(NULL,false,NULL);

}

void BaslercamRelease(void)
{
	//DeleteCriticalSection(lpBasler_critical_section);
	//ReleaseMutex(CBaslerGigeCamera::Basler_Mutex);
	CloseHandle(CBaslerGigeCamera::Basler_Mutex);
}




/*
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			AfxMessageBox("开始加载");
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
			AfxMessageBox("开始加载");
			_tprintf(_T("错误: MFC 初始化失败\n"));

		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
*/

//继承的抽象虚函数 模式：异步同步  策略:抓头部和尾部
 bool CBaslerGigeCamera::InitCam(unsigned long Index)   //初始化相机
 { 
	 SetSynchroMode(AsynchroGrab);
	 OpenModeCam(m_CamIndex, AsynchroGrab,m_GrabStrategy);
	
	 return TRUE;
 }
 bool CBaslerGigeCamera::UnInitCam()
 {
	 CloseBaslerCam();
	 return TRUE;
 }

 int CBaslerGigeCamera::GetImageWidth()				// 获取图片宽度
 {
	 int64_t ImageWid = GetWidth();
	 if (-1 == ImageWid)
	 {
		  ImageWid = 2592;   //暂时采用常量，应该改为可设定
	 }
	 return (int)ImageWid;
	
 }


 int CBaslerGigeCamera::GetImageHeight() 			// 获取图片高度
 {
	 int64_t ImageHeight = GetHeight();
	 if (-1 == ImageHeight)
	 {
		 ImageHeight = 1944;   //暂时采用常量，应该改为可设定
	 }
	 return (int)ImageHeight;
 }

//virtual void SetWindowID() = 0;
// 设置回调函数, Index-相机号，pWnd-目标窗口指针
 void CBaslerGigeCamera::SetCallBack(void(*Callback)(void* context)  , void* pWnd) 
 {
	   m_pCallBack = Callback;
	  //Callback(&m_ptrGrabResult);
	  m_pGrabParent = pWnd ;
	
 }
 void CBaslerGigeCamera::SetBuffer(BYTE* pBuffer) 		// 设置图像缓冲区
 {
       m_pBuffer =  pBuffer ;	 
 }

 bool CBaslerGigeCamera::SingleSnap(bool bDisplay/* = true*/) 	// 单帧采集 // 叶
 {
	// return TRUE;
	 SetSynchroMode(SynchroGrab);
	 SetGrabStrategy(GrabStrategy_LatestImageOnly);
	 switch (m_SynchroMode)
	 {
	 case NoMode:
		 {
		    return false;
		 }
		 break;
	 case SynchroGrab:
		 {
			 if(BeginGrab())
			 {
				return BaslerCamGrab();
			 }else
			 {
				 return false;
			 }

		 }
		 break;
	 case AsynchroGrab:
		 {
			 if(BeginAsynchGrab(/*m_pGrabParent, m_pCallBack*/ ))
			 {
				return AsynchGrab();
			 }else
			 {
				 return FALSE;
			 }
		 }
		 break;
	 }
	 return TRUE;
 }

 bool CBaslerGigeCamera::StartSnap() // 连续采集
 {
	 SetSynchroMode(AsynchroGrab);
	 SetGrabStrategy(GrabStrategy_OneByOne);
	 m_IsContinuGrab = TRUE;
	 BeginAsynchGrab();

	 return TRUE;
 }
 bool CBaslerGigeCamera::StopSnap() // 停止采集
 {

	 StopAsynchGrab();
	 return TRUE;
 }

 bool CBaslerGigeCamera::SetTriggerMode(bool Mode)		//设置触发模式	
 {
	 return FALSE;
 }

 bool CBaslerGigeCamera::SetExposureTime(int iValue) 	//设置曝光时间
 {
	return SetBaslerExposureTime(iValue);
 }
 bool CBaslerGigeCamera::SetGain(int iValue) 		//设置增益
 {
	return SetCamGain(iValue);
 }

 int CBaslerGigeCamera::GetExposureTime() 		//获取曝光时间
 {
	return GetBaslerExposureTime();
 }
 int CBaslerGigeCamera::GetGain()			//获取增益
 {
	 return GetCamGain();
 }

 char*  CBaslerGigeCamera::GetFullName()
 {
	 //int CamIndex = m_CamIndex;
	 //return GetCamFullName(CamIndex);
	 return m_CamerName;
 }

 int CBaslerGigeCamera::GetParameInfo(ICAMERA_PARAMETER speciPara,unsigned long &info)///AVT中的参数
 {

	 return -1;
 }
 int CBaslerGigeCamera::SetParameInfo(ICAMERA_PARAMETER speciPara,int val)
 {
	 return -1;
 }



//自定义函数

  

CBaslerGigeCamera::CBaslerGigeCamera(void)
{
	IsBeginGrab = TRUE ;
	//IsIniTrlFactory = FALSE;
	IsOpenCam = FALSE;
	IsReadyGrab = FALSE;
	m_StopGrab = FALSE;
	m_SynchroMode = NoMode;
	m_IsGrab = FALSE;
	m_GrabStrategy = GrabStrategy_LatestImageOnly;
	m_hGrabThreadRun = NULL;
	m_pCallBack = NULL;
	m_CamIndex = -1;

}
CBaslerGigeCamera::CBaslerGigeCamera(char*camerName)
{
	IsBeginGrab = TRUE ;
	//IsIniTrlFactory = FALSE;
	IsOpenCam = FALSE;
	IsReadyGrab = FALSE;
	m_StopGrab = FALSE;
	m_SynchroMode = NoMode;
	m_IsGrab = FALSE;
	m_GrabStrategy = GrabStrategy_LatestImageOnly;
	m_hGrabThreadRun = NULL;
	m_pCallBack = NULL;
	m_CamIndex = -1;
	memset(m_CamerName,0,CAMFULLNAME_LENTH_MAX);
	strcpy(m_CamerName,camerName);

}


CBaslerGigeCamera::~CBaslerGigeCamera(void)
{
	IsBeginGrab = TRUE;
	if (IsCamOpen())
	{
		CloseBaslerCam();
	}
	if (IsIniTrlFactory)
	{
		IsIniTrlFactory--;
		if (IsIniTrlFactory==0)
		{
			PylonTerminate();
		}
		 
		
	}
	
}
/*
功能:初始化工厂(静态函数)
输入:无
输出:EnumDeviceList
返回值:无
注意事项:驱动信息，工厂每次调用序列化EnumerateDevices的结果是不一样的
*/
void CBaslerGigeCamera::IniTrlFactory()
{
	char StrTemp[256] = {0};
	if(!IsIniTrlFactory)
	{
		PylonInitialize();//只调用一次
		CTlFactory::GetInstance().EnumerateDevices(EnumDeviceList);//这个是不是阻塞很久
	}
	IsIniTrlFactory++;
}
/*
功能:根据索引获取名字(暂时没用到)
输入:索引
输出:相机名字
返回值:无
注意事项:获取的是初始化时的遍历的一次的信息，如果此时拔相机不会发生变化
*/

char* CBaslerGigeCamera::GetDeviceUserName(int FindCamIndex)
{
	char userName[CAMFULLNAME_LENTH_MAX] = {0};
	IniTrlFactory();
	DeviceInfoList_t DeviceList;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(DeviceList);//获取相机信息，但是按照程序是只有获取一次
	ReleaseMutex(Basler_Mutex);
	sprintf(userName,"%s",DeviceList[FindCamIndex].GetUserDefinedName().c_str());
	return userName;
}
//获取类型(暂时没用到)
char* CBaslerGigeCamera::GetDeviceTypeStr(int FindCamIndex)
{
	char StrTemp[CAMFULLNAME_LENTH_MAX] = {0};
	IniTrlFactory();

	DeviceInfoList_t DeviceList;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(DeviceList);
	ReleaseMutex(Basler_Mutex);
	sprintf(StrTemp,"%s",DeviceList[FindCamIndex].GetDeviceClass().c_str());
	return  StrTemp;
}

//获取序列号(暂时没用到)
char* CBaslerGigeCamera::GetDeviceSerialNo(int FindCamIndex)
{
	char StrTemp[CAMFULLNAME_LENTH_MAX] = {0};
	IniTrlFactory();
	DeviceInfoList_t DeviceList;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(DeviceList);
	ReleaseMutex(Basler_Mutex);
	sprintf(StrTemp,"%s",DeviceList[FindCamIndex].GetSerialNumber().c_str());
		return  StrTemp;

}
//获取昵称(暂时没用到)
char* CBaslerGigeCamera::GetDeviceFriendlyName(int FindCamIndex)
{
	char StrTemp[CAMFULLNAME_LENTH_MAX] = {0};
	IniTrlFactory();
	DeviceInfoList_t DeviceList;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(DeviceList);
	ReleaseMutex(Basler_Mutex);
	sprintf(StrTemp,"%s",DeviceList[FindCamIndex].GetFriendlyName().c_str());
	return  StrTemp;

}


//获取相机信息(暂时没用到)
CDeviceInfo CBaslerGigeCamera::GetDeviceType(int FindCamIndex)
{
	IniTrlFactory();
	int AllIndex = 0;
		
 	DeviceInfoList_t lstDevices;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(lstDevices);
	ReleaseMutex(Basler_Mutex);		
	if (FindCamIndex>AllIndex||FindCamIndex<0)
	{
		//return "";
	}
	return lstDevices[FindCamIndex];

}
//设置相机采集模式
void CBaslerGigeCamera::SetSynchroMode(SynMode mode)
{
	m_SynchroMode = mode;
	
}

/*
功能:根据索引找到同类型的相机标识(静态函数)
输入:同类型相机索引
输出:相机索引
返回值:
注意事项:静态函数,枷锁
*/
char* CBaslerGigeCamera::GetCamFullName(int& FindCamIndex)
{
	char StrTemp[CAMFULLNAME_LENTH_MAX] = {0};
	int  FindIndex = FindCamIndex;
	IniTrlFactory();
 	DeviceInfoList_t lstDevices;
	WaitForSingleObject(Basler_Mutex,INFINITE);//枷锁
	GetDeviceInfoList(lstDevices);
	ReleaseMutex(Basler_Mutex);
	FindCamIndex = lstDevices.size()-1;
	if (FindIndex>FindCamIndex||FindIndex<0)
	{
		return "";
	}
	sprintf(StrTemp,"%s",lstDevices[FindIndex].GetFullName().c_str());
	return StrTemp;
}

//获取相机数量
int CBaslerGigeCamera::GetCamCount()
{
	IniTrlFactory();
	DeviceInfoList_t lstDevices;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(lstDevices);
	ReleaseMutex(Basler_Mutex);
	return lstDevices.size();
}

//通过相机名字获取索引(暂时没用到)
int  CBaslerGigeCamera::GetDeviceIndexByFullName(char* DeviceFullName)
{
	IniTrlFactory();
	DeviceInfoList_t lstDevices;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(lstDevices);
	ReleaseMutex(Basler_Mutex);
	int CamNum = lstDevices.size();
	for (int i = 0;i< CamNum;i++)
	{
		if (lstDevices[i].GetFullName().c_str() == DeviceFullName )
		{
			return i;
		}
	}
	return -1;

}

/*
功能:打开相机
输入:相机索引，采集模式,策略
输出:无
返回值:打开相机 1成功 0失败
注意事项:静态函数,枷锁
*/
BOOL CBaslerGigeCamera::OpenModeCam(int  CamIndex,SynMode Mode,EGrabStrategy strategy)
{
	if (Mode != m_SynchroMode)
	{
		return FALSE;
	}
	if(!OpenBaslerCam(CamIndex, strategy))
		return FALSE;
	return TRUE;
}

//相机索引是否在设备的索引范围内
BOOL CBaslerGigeCamera::IsAvailabCamIndex(int CamIndex)
{
	int MaxCamIndex;
	DeviceInfoList_t lstDevices;
	WaitForSingleObject(Basler_Mutex,INFINITE);
	GetDeviceInfoList(lstDevices);
	ReleaseMutex(Basler_Mutex);
	MaxCamIndex = lstDevices.size()-1;
	if (CamIndex>MaxCamIndex||CamIndex<0)
	{
		return FALSE;
	}else
	{
		return TRUE;
	}

}
//获取相机所有信息
void CBaslerGigeCamera::GetDeviceInfoList(DeviceInfoList_t& DeviceList)
{
	  DeviceList = EnumDeviceList;
}

//获取打开的第一的相机,最多64 一个映射表,初始化为都没有打开
 int  CBaslerGigeCamera::GetUnOpenIndex(void)
 {

	 for (int i = 0;i<64;i++)
	 {
		 if (!m_CamOpenedMap[i])
		 {
			 return i;
		 }
	 }
	 return -1;
 }
 /*
 功能:创建并打开相机
 输入:相机索引,策略
 输出:无
 返回值:打开相机 1成功 0失败
 注意事项:静态函数,枷锁
 */
BOOL CBaslerGigeCamera::CreateOpenBaslerCam(int  CamIndex,EGrabStrategy strategy)
{
	char CamGuid[256] = {0};
	if (IsCamOpen())
	{
		memset(CamGuid,0,sizeof(CamGuid));
		memcpy(CamGuid, CBaslerGigeCamera::GetCamFullName(CamIndex),sizeof(CamGuid));
		return FALSE;
	}
	IniTrlFactory();

	if (!IsAvailabCamIndex(CamIndex))
	{
		return FALSE;
	}

	try{

		DeviceInfoList_t lstDevices;
		WaitForSingleObject(Basler_Mutex,INFINITE);
		GetDeviceInfoList(lstDevices);
		ReleaseMutex(Basler_Mutex);
		camera.Attach(CTlFactory::GetInstance().CreateDevice(lstDevices[CamIndex]));//.CreateFirstDevice( info)
		if (camera.IsOpen())
		{
			return FALSE;
		}
		camera.Open();
		camera.StartGrabbing(strategy);  //因为只能调用一次
		m_GrabStrategy = strategy;
	}
	catch(...)
	{
		return IsOpenCam = FALSE;

	}
	m_CamIndex = CamIndex;	
	return IsOpenCam = TRUE;

}
 

BOOL CBaslerGigeCamera::OpenBaslerCam(int CamIndex,EGrabStrategy strategy)
{
	char CamGuid[256] = {0};
	if (IsCamOpen())
	{
		memset(CamGuid,0,sizeof(CamGuid));
		memcpy(CamGuid, CBaslerGigeCamera::GetCamFullName(CamIndex),sizeof(CamGuid));
		return FALSE;
	}
	

	if (!IsAvailabCamIndex(CamIndex))
	{
		return FALSE;
	}

	try{
		
		if (camera.IsOpen())
		{
			return FALSE;
		}
		camera.Open();
		camera.StartGrabbing(strategy);  //因为只能调用一次
		m_GrabStrategy = strategy;
	}
	catch(...)
	{
		return IsOpenCam = FALSE;
	    
	}
	return IsOpenCam = TRUE;
	
}
//查询相机是否开着
BOOL CBaslerGigeCamera::IsCamOpen()
{
	return camera.IsOpen();

}



BOOL CBaslerGigeCamera::CloseBaslerCam()
{
	if (!camera.IsOpen())
	{
		return FALSE;
	}
	camera.Close();
	IsOpenCam = FALSE;
	return TRUE;
}

double CBaslerGigeCamera::GetBaslerExposureTime()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CFloatPtr  ExposureTime;
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		ExposureTime = control.GetNode("ExposureTime"); 

	}else
	{
		ExposureTime = control.GetNode("ExposureTimeAbs");
	}
	if (IsReadable(ExposureTime))
	{
		return ExposureTime->GetValue();
	}else
	{
		return -1;
	}
}


double CBaslerGigeCamera::GetMaxExposureTime()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CFloatPtr  ExposureTime;
	//uint64_t NodesNumber = 	control.GetNumNodes();

	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		ExposureTime = control.GetNode("ExposureTime"); 

	}else
	{
		ExposureTime = control.GetNode("ExposureTimeAbs");
	}
	if (IsReadable(ExposureTime))
	{
		return ExposureTime->GetMax();
	}else
	{
		return -1;
	}
}

double CBaslerGigeCamera::GetMinExposureTime()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CFloatPtr  ExposureTime;
	//uint64_t NodesNumber = 	control.GetNumNodes();

	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		ExposureTime = control.GetNode("ExposureTime"); 

	}else
	{
		ExposureTime = control.GetNode("ExposureTimeAbs");
	}
	if (IsReadable(ExposureTime))
	{
		return ExposureTime->GetMin();
	}else
	{
		return -1;
	}
}


BOOL  CBaslerGigeCamera::SetBaslerExposureTime(UINT MilSeconds)
{
	if (!IsCamOpen())
	{
		return FALSE;
	}
	CFloatPtr  ExposureTime;

	INodeMap &control = camera.GetNodeMap();
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		ExposureTime = control.GetNode("ExposureTime"); 

	}else
	{
		ExposureTime = control.GetNode("ExposureTimeAbs");

	}

	if (IsReadable(ExposureTime))
	{
		if ( MilSeconds > ExposureTime->GetMax()|| MilSeconds < ExposureTime->GetMin() )
		{
			return FALSE;
		}
	}

	if (IsWritable(ExposureTime))
	{
		ExposureTime->SetValue(MilSeconds);
		return TRUE;
	}else
	{
		return FALSE;
	}
}

double CBaslerGigeCamera::GetCamGain()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CFloatPtr CamGain2;
	CIntegerPtr CamGain;  //CIntegerPtr
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		CamGain2 =  control.GetNode("Gain");
		if (IsReadable(CamGain2))
		{

			return CamGain2->GetValue();
		}else
		{
			return -1;
		}

	}else
	{
		CamGain =  control.GetNode("GainRaw");//GainRaw

		if (IsReadable(CamGain))
		{
			return (double)CamGain->GetValue();
		}else
		{
			return -1;
		}
	}
}


int64_t  CBaslerGigeCamera::GetCamParamNum()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	uint64_t NodesNumber = 	control.GetNumNodes();
	return NodesNumber;
}

double CBaslerGigeCamera::GetMaxGain()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CFloatPtr CamGain2;
	CIntegerPtr CamGain;  //CIntegerPtr
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		CamGain2 =  control.GetNode("Gain");
		if (IsReadable(CamGain2))
		{
			return CamGain2->GetMax();
		}else
		{
			return -1;
		}

	}else
	{
		CamGain =  control.GetNode("GainRaw");//GainRaw

		if (IsReadable(CamGain))
		{
			return (double)CamGain->GetMax();
		}else
		{
			return -1;
		}
	}

}

double CBaslerGigeCamera::GetMinGain()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	int intTest = 0;
	//camera.GainAbs;
	CFloatPtr CamGain2;
	CIntegerPtr CamGain;  //CIntegerPtr
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		CamGain2 =  control.GetNode("Gain");
		if (IsReadable(CamGain2))
		{
			return CamGain2->GetMin();
		}else
		{
			return -1;
		}

	}else
	{
		CamGain =  control.GetNode("GainRaw");//GainRaw

		if (IsReadable(CamGain))
		{
			return (double)CamGain->GetMin();
		}else
		{
			return -1;
		}
	}

}


BOOL CBaslerGigeCamera::SetCamGain(double GainValue )
{
	if (!IsCamOpen())
	{
		return FALSE;
	}
	INodeMap &control = camera.GetNodeMap();
	int intTest = 0;
	//camera.GainAbs;
	CFloatPtr CamGain2;//CFloatPtr
	CIntegerPtr CamGain;  
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		CamGain2 =  control.GetNode("GainAbs");
		if (IsReadable(CamGain2))
		{
			if (GainValue > CamGain2->GetMax() || GainValue < CamGain2->GetMin() )
			{
				return FALSE;
			}

		}
		if (IsWritable(CamGain2))
		{ 
			CamGain2->SetValue(GainValue);
			return TRUE;
		}else
		{
			return FALSE;
		}
	}else
	{
		CamGain =  control.GetNode("GainRaw");
		if (IsReadable(CamGain))
		{
			if (GainValue > CamGain->GetMax() || GainValue < CamGain->GetMin() )
			{
				return FALSE;
			}

		}

		if (IsWritable(CamGain))
		{ 
			CamGain->SetValue((int64_t)GainValue);
			return TRUE;
		}else
		{
			return FALSE;

		}
	}

}


int64_t CBaslerGigeCamera::GetWidth()
{
	if (!IsCamOpen())
	{
		if (-1 == m_CamIndex)
		{
			return -1;
		}
		


	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr WidthPtr;  

	WidthPtr =  control.GetNode("Width");
	if (IsReadable(WidthPtr))
	{
		return WidthPtr->GetValue();   
	}else
	{
		return -1;
	}

}
int64_t CBaslerGigeCamera::GetMaxWidth()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr WidthPtr;  

	WidthPtr =  control.GetNode("Width");
	if (IsReadable(WidthPtr))
	{
		return WidthPtr->GetMax();
	}else
	{
		return -1;
	}
}

int64_t CBaslerGigeCamera::GetMinWidth()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr WidthPtr;  

	WidthPtr =  control.GetNode("Width");
	if (IsReadable(WidthPtr))
	{
		return WidthPtr->GetMin();
	}else
	{
		return -1;
	}

}

BOOL CBaslerGigeCamera::SetWidth(int64_t width)
{

	if (!IsCamOpen())
	{
		return FALSE;
	}
	INodeMap &control = camera.GetNodeMap();

	//CFloatPtr WidthPtr;//CFloatPtr
	CIntegerPtr WidthPtr;  

	WidthPtr =  control.GetNode("Width");
	if (IsReadable(WidthPtr))
	{
		if (width > WidthPtr->GetMax()|| width < WidthPtr->GetMin())
		{
			return FALSE;
		}

	}

	if (IsWritable(WidthPtr))
	{ 
		WidthPtr->SetValue(width);
		return TRUE;
	}else
	{
		return FALSE;
	}

}


int64_t CBaslerGigeCamera::GetHeight()
{
	int64_t ImageHeight= -1;
	BOOL PreOpen = TRUE;
	if (!IsCamOpen())
	{

		return ImageHeight;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr HeightPtr;  

	HeightPtr =  control.GetNode("Height");
	if (IsReadable(HeightPtr))
	{
		ImageHeight = HeightPtr->GetValue();
	 
	}else
	{
		ImageHeight = -1;
	}

	return ImageHeight;
}

int64_t CBaslerGigeCamera::GetMaxHeight()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr HeightPtr;  
	HeightPtr =  control.GetNode("Height");
	if (IsReadable(HeightPtr))
	{
		return HeightPtr->GetMax();   
	}else
	{
		return -1;
	}

}
int64_t  CBaslerGigeCamera::GetMinHeight()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr HeightPtr;  
	HeightPtr =  control.GetNode("Height");
	if (IsReadable(HeightPtr))
	{
		return HeightPtr->GetMin();   
	}else
	{
		return -1;
	}
}




BOOL CBaslerGigeCamera::SetHeight(int64_t Height)
{
	if (!IsCamOpen())
	{
		return FALSE;
	}
	INodeMap &control = camera.GetNodeMap();

	//CFloatPtr WidthPtr;//CFloatPtr
	CIntegerPtr HeightPtr;  

	HeightPtr =  control.GetNode("Height");
	if (IsReadable(HeightPtr))
	{
		if (Height > HeightPtr->GetMax()|| Height < HeightPtr->GetMin())
		{
			return FALSE;
		}
	}
	if (IsWritable(HeightPtr))
	{ 
		HeightPtr->SetValue(Height);
		return TRUE;
	}else
	{
		return FALSE;
	}

}

int64_t CBaslerGigeCamera::GetOffsetX()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	


	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr offsetXPtr;  

	offsetXPtr =  control.GetNode("OffsetX");
	if (IsReadable(offsetXPtr))
	{
		return offsetXPtr->GetValue();   
	}else
	{
		return -1;
	}
}

int64_t CBaslerGigeCamera::GetMaxOffsetX()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr offsetXPtr;  

	offsetXPtr =  control.GetNode("OffsetX");
	if (IsReadable(offsetXPtr))
	{
		return offsetXPtr->GetMax();   
	}else
	{
		return -1;
	}
}

int64_t  CBaslerGigeCamera::GetMinOffsetX()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr offsetXPtr;  

	offsetXPtr =  control.GetNode("OffsetX");
	if (IsReadable(offsetXPtr))
	{
		return offsetXPtr->GetMin();   
	}else
	{
		return -1;
	}
}

BOOL CBaslerGigeCamera::SetOffsetX(int64_t offsetX)
{
	if (!IsCamOpen())
	{
		return FALSE;
	}
	INodeMap &control = camera.GetNodeMap();

	CIntegerPtr offsetXPtr;  
	offsetXPtr =  control.GetNode("OffsetX");
	if (IsReadable(offsetXPtr))
	{
		if (offsetX > offsetXPtr->GetMax()|| offsetX < offsetXPtr->GetMin())
		{
			return FALSE;
		}
	}
	if (IsWritable(offsetXPtr))
	{ 
		offsetXPtr->SetValue(offsetX);
		return TRUE;
	}else
	{
		return FALSE;
	}
}


int64_t CBaslerGigeCamera::GetOffsetY()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr offsetYPtr;  

	offsetYPtr =  control.GetNode("OffsetY");
	if (IsReadable(offsetYPtr))
	{
		return offsetYPtr->GetValue();   
	}else
	{
		return -1;
	}
}

int64_t CBaslerGigeCamera::GetMaxOffsetY()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr offsetYPtr;  

	offsetYPtr =  control.GetNode("OffsetY");
	if (IsReadable(offsetYPtr))
	{
		return offsetYPtr->GetMax();   
	}else
	{
		return -1;
	}
}

int64_t CBaslerGigeCamera::GetMinOffsetY()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr offsetYPtr;  

	offsetYPtr =  control.GetNode("OffsetY");
	if (IsReadable(offsetYPtr))
	{
		return offsetYPtr->GetMin();   
	}else
	{
		return -1;
	}
}



BOOL CBaslerGigeCamera::SetOffsetY(int64_t offsetY)
{
	if (!IsCamOpen())
	{
		return FALSE;
	}
	INodeMap &control = camera.GetNodeMap();
	CIntegerPtr offsetYPtr;  
	offsetYPtr =  control.GetNode("OffsetY");
	if (IsReadable(offsetYPtr))
	{
		if (offsetY > offsetYPtr->GetMax()|| offsetY < offsetYPtr->GetMin())
		{
			return FALSE;
		}
	}
	if (IsWritable(offsetYPtr))
	{ 
		offsetYPtr->SetValue(offsetY);
		return TRUE;
	}else
	{
		return FALSE;
	}
}

BOOL CBaslerGigeCamera::SaveParamToFile(char* FileFullName)
{
	char FileName[FILENAME_LENTH_MAX] = {0};
	strcpy_s(FileName,FileFullName);

	if (!IsCamOpen())
	{
		return FALSE;
	}
	try
	{
		CFeaturePersistence::Save( FileName, &camera.GetNodeMap() );
	}
	catch (const GenericException )
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CBaslerGigeCamera::LoadParamFromFile(char* FileFullName)
{
	char FileName[FILENAME_LENTH_MAX];
	strcpy(FileName,FileFullName);

	if (!IsCamOpen())
	{
		return FALSE;
	}
	try
	{
		CFeaturePersistence::Load( FileName, &camera.GetNodeMap(), true );
	}
	catch (const GenericException )
	{
		return FALSE;
	}
	return TRUE;

}






double  CBaslerGigeCamera::GetTriggerDelay()
{
	if (!IsCamOpen())
	{
		return -1;
	}
	INodeMap &control = camera.GetNodeMap();
	int intTest = 0;
	//camera.TriggerDelayAbs;

	CFloatPtr CamTriggerDelay;
	CIntegerPtr CamTriggerDelayInt;  
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		CamTriggerDelay =  control.GetNode("TriggerDelay");
		if (IsReadable(CamTriggerDelay))
		{
			return CamTriggerDelay->GetValue();
		}else
		{
			return -1.0;
		}

	}else
	{
		CamTriggerDelay =  control.GetNode("TriggerDelayAbs");//

		EAccessMode AccMod = CamTriggerDelay->GetAccessMode(); 


		if (IsReadable(CamTriggerDelay))
		{
			return CamTriggerDelay->GetValue();
		}else
		{
			return -1.0;
		}

	}

}

BOOL CBaslerGigeCamera::SetTriggerDelay(double DelayValue)
{
	if (!IsCamOpen())
	{
		return FALSE;
	}
	INodeMap &control = camera.GetNodeMap();
	int intTest = 0;
	//camera.GainAbs;
	CFloatPtr CamTriggerDelay;
	//CIntegerPtr CamGain;  
	if( camera.GetSfncVersion() >= Sfnc_2_0_0)
	{
		CamTriggerDelay =  control.GetNode("TriggerDelay");
	}else
	{
		CamTriggerDelay =  control.GetNode("TriggerDelayAbs");//GainRaw
	}
	if (IsReadable(CamTriggerDelay))
	{
		if (DelayValue> CamTriggerDelay->GetMax()||DelayValue<CamTriggerDelay->GetMin())
		{
			return FALSE;
		} 
	}

	if (IsWritable(CamTriggerDelay))
	{
		CamTriggerDelay->SetValue(DelayValue);
		return TRUE;
	}else
	{
		return FALSE;
	}



}



void CBaslerGigeCamera::SetGrabStrategy(EGrabStrategy strategy)
{
	if (m_GrabStrategy == strategy)
	{
		return;
	}

	if (IsCamOpen())
	{
		CloseBaslerCam();
		OpenModeCam(m_CamIndex,m_SynchroMode,strategy);	
	}
}

EGrabStrategy CBaslerGigeCamera::GetGrabStrategy()
{
	return m_GrabStrategy;
}

BOOL  CBaslerGigeCamera::BeginGrab()
{
	
	if (m_SynchroMode != SynchroGrab )
	{
		return FALSE;
	} 
	if(!IsCamOpen())
	{
		return FALSE;
	}
	IsReadyGrab = TRUE;
	return TRUE;
}

void CBaslerGigeCamera::StopAsynchGrab()
{
	m_StopGrab = TRUE; //可以结束进程
	SetEvent(m_hGrabThreadRun); 
}

long CBaslerGigeCamera::GetImageAcqWidth()
{
	if (!m_IsGrab)
	{
		return -1;
	}
	return m_ptrGrabResult->GetWidth();

}

long CBaslerGigeCamera::GetImageAcqHeight()
{
	if (!m_IsGrab)
	{
		return -1;
	}
	return m_ptrGrabResult->GetHeight();

}




DWORD WINAPI  CBaslerGigeCamera::GrabThread(void* pThis)
{

	CBaslerGigeCamera* pCam = (CBaslerGigeCamera*)pThis;

	while (TRUE)
	{  
        if(!pCam->m_IsContinuGrab)
		{
	    	WaitForSingleObject(pCam->m_hGrabThreadRun,INFINITE);
		}

		if (pCam->m_StopGrab)
		{
			pCam->m_StopGrab = FALSE;   // 仅此处赋值不够保险。
			break;
		}

		ResetEvent(pCam->m_hGrabThreadRun);
		pCam->m_IsGrab = FALSE;
		try{
			if( pCam->camera.IsGrabbing())
			{
				// Wait for an image and then retrieve it. A timeout of 5000 ms is used.
				pCam->camera.RetrieveResult( 5000, pCam->m_ptrGrabResult, TimeoutHandling_ThrowException);
				// Image grabbed successfully?
				if (pCam->m_ptrGrabResult->GrabSucceeded())
				{
					//调用回调函数
					 pCam->m_IsGrab = TRUE;					
					 size_t ImageSize =  pCam->m_ptrGrabResult->GetImageSize();
					 memcpy(pCam->m_pBuffer,pCam->m_ptrGrabResult->GetBuffer(),ImageSize); //没有加锁
					 pCam->m_pCallBack((void*)pThis);   //参数是lparam,参数是
				}
				else
				{
					pCam->m_IsGrab = FALSE;
				}
			}
		}
		catch (const GenericException &e)
		
		{
			TRACE("异常：%s",e.GetDescription());
			pCam->m_IsGrab = FALSE;

		}

	}
	CloseHandle(pCam->m_hGrabThreadRun);
	pCam->m_hGrabThreadRun = NULL;

	return 0;

}




DWORD WINAPI CBaslerGigeCamera::BalerCamGetDevice(void* pThis)
{
	CBaslerGigeCamera* ThisCam = (CBaslerGigeCamera*)pThis;
	ThisCam->BeginGrab();
	while(!ThisCam->IsBeginGrab)
	{
		PylonTerminate(); 
		return 0;
	}
	return 0;
}


BOOL CBaslerGigeCamera::AsynchGrab()
{
	if (m_SynchroMode != AsynchroGrab )
	{
		return FALSE;
	}
	SetEvent(m_hGrabThreadRun);

	return TRUE;

}


BOOL CBaslerGigeCamera::BaslerCamGrab()
{
	if (m_SynchroMode != SynchroGrab )
	{
		return FALSE;
	}
	m_IsGrab = FALSE;
	if(!IsReadyGrab)
	{
		return FALSE;
	}
	try
	{

		if( camera.IsGrabbing())
		{
			// Wait for an image and then retrieve it. A timeout of 5000 ms is used.
			camera.RetrieveResult( 1000, m_ptrGrabResult, TimeoutHandling_ThrowException);

			// Image grabbed successfully?
			if (m_ptrGrabResult->GrabSucceeded())
			{
				 //memset(m_pBuffer,0,5038848);
				size_t ImageSize =  m_ptrGrabResult->GetImageSize();
				memcpy(m_pBuffer,m_ptrGrabResult->GetBuffer(),ImageSize); 
				m_IsGrab  =  TRUE;
			}
			else
			{
				//AfxMessageBox("采集图像数据不正确");
				m_IsGrab = FALSE;
			}
		}
	}
	catch (const GenericException &e)
	{
		TRACE("异常：%s",e.GetDescription());
		m_IsGrab = FALSE;
	}
	return m_IsGrab;
}


BOOL  CBaslerGigeCamera::BeginAsynchGrab(/*void* pGrabParent, pCallbackFunc pCallback*/)
{

	if (m_SynchroMode != AsynchroGrab )
	{
		return FALSE;
	}
	if (!IsCamOpen())
	{
		return FALSE;
	}

	IsReadyGrab = TRUE;
	if(m_hGrabThreadRun)
	{
		return FALSE;
	}
	
	//创建采集线程
	m_hGrabThreadRun = CreateEventA(NULL,true,false,NULL);
	m_StopGrab = FALSE;
	CreateThread(NULL,0,GrabThread,this,0,NULL);

	return TRUE;

}

 CBaslerGigeCamera* pBaslerCamera = NULL;


EXTERN_C ICAMERASPEC CameraHANDLE WINAPI CreateCamera(char*camerName)
{
	//创建相机时，应该分配资源，即将该对象分配实际的相机（驱动）绑定，以及为成员变量赋值,此处打开相机才能绑定
	pBaslerCamera = new CBaslerGigeCamera(camerName);
	CBaslerGigeCamera::IniTrlFactory();
	int NeedOpenIndex = CBaslerGigeCamera::GetUnOpenIndex();
	if ( NeedOpenIndex >= 0 && NeedOpenIndex < CBaslerGigeCamera::GetCamCount() )
	{
		 if(pBaslerCamera->CreateOpenBaslerCam(NeedOpenIndex))
		 {	
			 CBaslerGigeCamera::m_CamOpenedMap[NeedOpenIndex] = true;
		 }else
		 {
			 delete pBaslerCamera;
			 pBaslerCamera = NULL;
		 }

	 }else
	{
		delete pBaslerCamera;
		pBaslerCamera = NULL;
	}
	return pBaslerCamera;
	
}



 EXTERN_C void WINAPI ReleaseCamera( CameraHANDLE  pCamera)
{
	CBaslerGigeCamera * pCamer = (CBaslerGigeCamera*)pCamera;
	if (pCamer)
	{		
		delete pCamer;
		pCamer = NULL;
	}
}
/*
功能:获取同类型相机列表
输入:列表,dll类型
输出:
返回值:相机数量
注意事项:静态函数,枷锁
*/
 EXTERN_C int WINAPI getList(list<string> &guidList,string &type)
{
	CBaslerGigeCamera::IniTrlFactory();		
	char CamGuid[256] = {0};
	int CameraNum = CBaslerGigeCamera::GetCamCount();
	int i=0;
	for (i = 0;i<CameraNum;i++)
	{
		 int CameraIndex = i;
		 memset(CamGuid,0,sizeof(CamGuid));
		 memcpy(CamGuid, CBaslerGigeCamera::GetCamFullName(CameraIndex),sizeof(CamGuid));
		 guidList.push_back(CamGuid);
	}
	return i;
}
