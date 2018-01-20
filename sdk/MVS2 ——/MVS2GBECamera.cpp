// MVS2.cpp 




#include "stdafx.h"
#include <string>
using namespace std;

#define MVS2GBECAMERA_EXPORTS 1

#include "MVS2GBECamera.h"
//海康威视提供的头文件



//静态成员初始化

bool MVS2GBECamera::m_isInitFlag = false;

MV_CC_DEVICE_INFO_LIST MVS2GBECamera::EnumDeviceList = {0};//相机列表

MVS2GBECamera::MVS2GBECamera()
{
	m_isInitFlag=false;

	m_iscontinueGrab=false;
	m_isstopGrab=true;
	m_CamIndex=0;
	memset(m_CamerName,0,CAMFULLNAME_LENTH_MAX);
	m_handle=nullptr;
	m_pBuffer=nullptr;
	pFrameInfo=nullptr;
	p_Xmldata=nullptr;
}

MVS2GBECamera::MVS2GBECamera(char*camerName)
{
	strcpy(m_CamerName,camerName);
	m_isInitFlag=false;

	m_iscontinueGrab=false;
	m_isstopGrab=true;
	m_CamIndex=0;
	m_handle=nullptr;
	m_pBuffer=nullptr;
	pFrameInfo=nullptr;
	p_Xmldata=nullptr;
}

MVS2GBECamera::~MVS2GBECamera()
{
	//delete m_pBuffer;
	//delete pFrameInfo;
	//delete p_Xmldata;
}

 bool MVS2GBECamera::InitCam(unsigned long Index) //初始化相机
{
	//获取相机和工控机传输协议 GigE USB IEEE1394 Camera Link
	unsigned int support_protocol = MV_CC_EnumerateTls();
	support_protocol=support_protocol&MV_GIGE_DEVICE;
	//判断是否支持GigE协议
	if( MV_GIGE_DEVICE != support_protocol )
	{
		return false;
	}
	//枚举出支持该协议的所有设备，并存放到设备信息列表中
	if( MV_CC_EnumDevices(MV_GIGE_DEVICE,&EnumDeviceList) )
	{
		return false;
	}
	m_CamIndex=Index;
	//判断索引值是否在 当前所有在线设备范围内
	if( EnumDeviceList.nDeviceNum >= Index && Index <= MV_MAX_DEVICE_NUM )
	{
		//创建设备实例句柄
		if( MV_CC_CreateHandle(&m_handle,EnumDeviceList.pDeviceInfo[Index]) )
			return false;
	}
	else
	{
		return false;
	}
	//连接设备
	if( MV_CC_OpenDevice(m_handle,MV_ACCESS_Control,0) )
		return false;
	else
		return true;
	//获取设备信息
	MV_CC_GetDeviceInfo(m_handle,&m_DeviceInfo);
}

bool MVS2GBECamera::UnInitCam() 
{
	//关闭设备，释放资源
	MV_CC_CloseDevice(m_handle);
	//销毁句柄，释放资源
	MV_CC_DestroyHandle(m_handle);
	return true;
}

int MVS2GBECamera::GetImageWidth()				// 获取图片宽度
{
	if( !MV_CC_GetWidth(m_handle,&ImageWidth))
		return ImageWidth.nCurValue;
	else
		return -1;
}
	

int MVS2GBECamera::GetImageHeight() 			// 获取图片高度
{
	if(!MV_CC_GetHeight(m_handle,&ImageHeight))
		return ImageHeight.nCurValue;
	else
		return -1;
}

//virtual void SetWindowID() = 0;
// 设置回调函数, Index-相机号，pWnd-目标窗口指针
void MVS2GBECamera::SetCallBack(void(*Callback)(void* context), void* pWnd) 	 
{
	m_fcallback=Callback;
	m_pGrabParent=pWnd;
}

void MVS2GBECamera::SetBuffer(BYTE* pBuffer_) 		// 设置图像缓冲区
{
	m_pBuffer=pBuffer_;
}

DWORD WINAPI MVS2GBECamera::ThreadGrading(LPVOID lpParamt)                  //采集图像的线程
{
	MVS2GBECamera* temp = (MVS2GBECamera*)lpParamt; 
	while(1)
	{
		if( temp->m_iscontinueGrab)
		{
			WaitForSingleObject(temp->m_hGrabEvent,INFINITE);
		}
		if (temp->m_isstopGrab)
		{
			temp->m_isstopGrab = FALSE;   // 仅此处赋值不够保险。
			break;
		}
		ResetEvent(temp->m_hGrabEvent );
		MVCC_INTVALUE stIntvalue = {0};
		MV_CC_GetIntValue(temp->m_handle, "PayloadSize", &stIntvalue);
		int nBufSize = stIntvalue.nCurValue + 2048; //一帧数据大小+预留字节(用于SDK内部处理)

		MV_CC_StartGrabbing(temp->m_handle);
		MV_CC_GetOneFrameTimeout(temp->m_handle,temp->m_pBuffer,nBufSize,temp->pFrameInfo,1000);
		//temp->m_fcallback(context);
	}
	CloseHandle(temp->m_hGrabEvent);
	temp->m_hGrabEvent=NULL;
	return 0;
}

bool MVS2GBECamera::SingleSnap(bool bDisplay ) 	// 单帧采集 // 使用的是同步采集，不使用回调函数
{
	//先前创建过了实例句柄
	//连接设备
	if( MV_CC_OpenDevice(m_handle,MV_ACCESS_Control,0) )
		return false;
	//设置采集模式为单帧
	m_GrabStrategy=MV_ACQ_MODE_SINGLE;
	if( MV_CC_SetAcquisitionMode(m_handle,m_GrabStrategy) )
		return false;
	else
		return true;

	MVCC_INTVALUE stIntvalue = {0};
	MV_CC_GetIntValue(m_handle, "PayloadSize", &stIntvalue);
	int nBufSize = stIntvalue.nCurValue + 2048; //一帧数据大小+预留字节(用于SDK内部处理)
	MV_CC_StartGrabbing(m_handle);
	MV_CC_GetOneFrameTimeout(m_handle,m_pBuffer,nBufSize,pFrameInfo,1000);
}

bool MVS2GBECamera::StartSnap() 	// 连续采集
{
	//先前创建过了实例句柄
	//连接设备
	if( MV_CC_OpenDevice(m_handle,MV_ACCESS_Control,0) )
		return false;
	//设置采集模式为连续
	m_GrabStrategy= MV_ACQ_MODE_CONTINUOUS;
	if( MV_CC_SetAcquisitionMode(m_handle,m_GrabStrategy) )
		return false;
	else
		return true;
	m_hGrabEvent = CreateEventA(NULL,true,false,NULL);
	CreateThread(NULL,0,ThreadGrading,this,0,NULL);
}

bool MVS2GBECamera::StopSnap() 	// 停止采集	
{
	if( MV_CC_StopGrabbing(m_handle) )
		return false;
	else
		return true;
}

bool MVS2GBECamera::SetTriggerMode(bool Mode)		//设置触发模式	
{
	if(!MV_CC_SetTriggerMode(m_handle,Mode))
		return true;
	else
		return false;
}

bool MVS2GBECamera::SetExposureTime(int iValue) 	//设置曝光时间	
{
	CameraExposureTime.Mode.nCurValue=MV_EXPOSURE_AUTO_MODE_OFF;
	if(!MV_CC_SetExposureAutoMode(m_handle,CameraExposureTime.Mode.nCurValue) )
	{	
		if(!MV_CC_SetExposureTime(m_handle,iValue))
			return true;
		else
			return false;
	}
	else
		return false;
}

bool MVS2GBECamera::SetGain(int iValue) 	//设置增益
{
	CameraGain.Mode.nCurValue=MV_GAIN_MODE_OFF;
	//设置模式
	if( !MV_CC_SetGainMode(m_handle,CameraGain.Mode.nCurValue))
	{
		//设置增益
		if(!MV_CC_SetGain(m_handle,iValue))
			return true;
		else
			return false;
	}
	else
		return false;
}

int MVS2GBECamera::GetExposureTime() 		//获取曝光时间
{
	if(!MV_CC_GetExposureTime(m_handle,&(CameraExposureTime.Time) )  )
		return CameraExposureTime.Time.fCurValue;
	else
		return -1;

}

int MVS2GBECamera::GetGain() 			//获取增益
{
	if(!MV_CC_GetGain(m_handle,&(CameraGain.Gain)))
		return CameraGain.Gain.fCurValue;
	else
		return -1;
}

char* MVS2GBECamera::GetFullName()
{

	return m_CamerName;
}

int MVS2GBECamera::GetParameInfo(ICAMERA_PARAMETER speciPara,unsigned long &info)///AVT中的参数
{
	return -1;
}

int MVS2GBECamera::SetParameInfo(ICAMERA_PARAMETER speciPara,int val)
{
	return -1;
}

int MVS2GBECamera::GetGuidList(list<string> &guidList,string &type)          //获取GUID列表
{
	int nRet = MV_OK;
	void* handle = NULL;
	char CamGuid[256] = {0};
	int i = 0;
	string cuuid;
	int camerNum = EnumDeviceList.nDeviceNum;
	memset(&EnumDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
	// 1. 枚举设备
	nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &EnumDeviceList);
	if (MV_OK != nRet)
	{
		printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
		return -1;
	}
	camerNum = EnumDeviceList.nDeviceNum;	
	for (i = 0;i<camerNum;i++)
	{
		MV_CC_DEVICE_INFO* pstMVDevInfo= EnumDeviceList.pDeviceInfo[i];
		if (NULL == pstMVDevInfo)
		{
			printf("%s\n" , "The Pointer of pstMVDevInfoList is NULL!");
			return i;
		}
		if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
		{
			cuuid = (char*)pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName;
			guidList.push_back(cuuid);
			printf("%s %x\n" , "nCurrentIp:" , pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp);                   //当前IP
			printf("%s %s\n\n" , "chUserDefinedName:" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);     //用户定义名
		
		}
		else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
		{
			cuuid = (char*)pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName;
			guidList.push_back(cuuid);
			printf("UserDefinedName:%s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
		}
		else
		{
			printf("Not support.\n");
		}
	}

	return i;
}

EXTERN_C CameraHANDLE WINAPI CreateCamera(char*camerName)
{
	MVS2GBECamera* example= new MVS2GBECamera(camerName);
	return example;
}

EXTERN_C void WINAPI ReleaseCamera(CameraHANDLE handle)
{
	handle->UnInitCam();
}

EXTERN_C int WINAPI getList(list<string> &guidList,string &type)
{

	return  MVS2GBECamera::GetGuidList(guidList,type);
}
