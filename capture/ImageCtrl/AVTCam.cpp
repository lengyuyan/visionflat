#include "stdafx.h"
#include <stdlib.h>
#include "windows.h"
#include "AVTCam.h"
#include "common.h"

bool AVTCam::m_b_FGIintFlag = false;
// 连续采集线程函数
DWORD WINAPI ContinousGrab(LPVOID pParam)
{ 
	FGFRAME   Frame;
	UINT32    error;

	AVTCam *pOwner = (AVTCam *)pParam;
	pOwner->m_bBusy = TRUE;
	pOwner->m_Camera.OpenCapture();
	pOwner->m_Camera.StartDevice();

	while(pOwner->m_bBusy) 
	{	
        try
        {
            error = pOwner->m_Camera.GetFrame(&Frame,0);
            if(error == FCE_NOERROR && Frame.pData != NULL)
            {			
                memcpy(pOwner->m_pImageBuffer, Frame.pData, pOwner->m_iImageHeight * pOwner->m_iImageWidth);
                if (pOwner->m_fcallback != NULL)
                {
                    pOwner->m_fcallback(pOwner->m_pParent);
                }
                error = pOwner->m_Camera.PutFrame(&Frame);  
            }
            else 
            {
                //pOwner->m_Camera.StopDevice();
                //pOwner->m_Camera.CloseCapture();
                Sleep(10);
                //pOwner->m_Camera.OpenCapture();
                //pOwner->m_Camera.StartDevice();
            }
        }
        catch (...)
        {
        }
	} 

	pOwner->m_Camera.CloseCapture();
	pOwner->m_Camera.StopDevice();
	return 0;
}

// 构造、析构函数
AVTCam::AVTCam(void)
{
	if (!m_b_FGIintFlag)
	{
		FGInitModule(NULL);
		m_b_FGIintFlag = true;
	}
	m_iResult = 0;
    m_iNodeCount = 0;
    if (FCE_NOERROR == m_iResult)
    {
        m_iResult = FGGetNodeList(m_NodeInfo, 3, &m_iNodeCount);
    }
	m_bBusy = false;
	m_iCamIdx = -1;
	m_pImageBuffer = NULL;
	m_fcallback = NULL;
	m_pParent = NULL;
}

AVTCam::~AVTCam(void)
{
	m_Camera.StopDevice();
	m_Camera.CloseCapture();
	//if (m_Camera!=NULL)
	//{
	//	delete m_pImageBuffer;
		m_pImageBuffer = NULL;
	//}
	FGExitModule();
}

// 初始化相机
bool AVTCam::InitCam(unsigned long Index)
{
	//if(Index >= m_iNodeCount)
	//{
	//	//AfxMessageBox(_T("No Camer"));
	//	return false;
	//}

	// 连接相机
	FGPINFO PacketSize;
    UINT32HL uGuid;
    uGuid.Low = Index;
    uGuid.High = m_NodeInfo[0].Guid.High;
	if(m_iResult == FCE_NOERROR && m_iNodeCount)
    {
	    //m_iResult = m_Camer.Connect(&m_NodeInfo[Index].Guid);		
        m_iResult = m_Camera.Connect(&uGuid);		
    }

	if (m_iResult == FCE_NOERROR)
	{
		m_iCamIdx = Index;
	}
	else if (m_iResult == HALER_BUSY)
	{
		//LOG_ERRO("Camer is busy!, CamerName=%lu", Index);	
		return false;
	}
	else
	{
		//LOG_ERRO("Open error, CamerName=%lu", Index);
		return false;
	}

	//m_Camera.StartDevice();
	//m_Camera.StopDevice();
	//Sleep(300);

	m_Camera.SetParameter(FGP_IMAGEFORMAT, MAKEDCAMFORMAT(7, 0, 0));
	
	FGPINFO ParmInfo;
 	m_iResult = m_Camera.GetParameterInfo(FGP_PHYSPEED,&ParmInfo);
 	m_iResult = m_Camera.SetParameter(FGP_PHYSPEED,PS_800MBIT);
 	m_iResult = m_Camera.GetParameterInfo(FGP_PACKETSIZE,&ParmInfo);
 	m_iResult = m_Camera.SetParameter(FGP_PACKETSIZE,ParmInfo.MaxValue/4);

	m_Camera.GetParameter(FGP_XSIZE, &m_iImageWidth);
	m_Camera.GetParameter(FGP_YSIZE, &m_iImageHeight);

	m_Camera.GetParameterInfo(FGP_PACKETSIZE,&PacketSize);
	PacketSize.IsValue = 2048;
	m_Camera.SetParameter(FGP_PACKETSIZE, PacketSize.IsValue);

//	m_pImageBuffer = new BYTE[m_iImageWidth * m_iImageHeight];
	
	m_iResult = m_Camera.OpenCapture();

	m_Camera.StartDevice();
	m_Camera.WriteRegister(0xF1000208, 0x80026006);	// 设置曝光时间单位为100 us

	return true;
}

bool AVTCam::UnInitCam()
{
    m_Camera.CloseCapture();
    m_Camera.Disconnect();
    return true;
}

int AVTCam::GetImageWidth()
{
	return m_iImageWidth;
}

int AVTCam::GetImageHeight()
{
	return m_iImageHeight;
}

void AVTCam::SetCallBack(void(*Callback)(void* context), void* pWnd)
{
	m_fcallback = Callback;
    m_pParent = pWnd;
}

void AVTCam::SetBuffer(BYTE* pBuffer)
{
	m_pImageBuffer = pBuffer;
}

// 单帧采集
bool AVTCam::SingleSnap(bool bDisplay)
{
	if (m_bBusy)
	{
		m_bBusy = false;
		Sleep(100);
	}
	bool bRet = false;

	
	if (m_iCamIdx >= 0)
	{
		LOG_INFO("StartDevice_s");
		m_iResult = m_Camera.OpenCapture();
		m_iResult = m_Camera.StartDevice();
		//m_iResult = m_Camera.DiscardFrames();
		LOG_INFO("StartDevice_e");
		FGFRAME Frame;
		m_iResult = m_Camera.GetFrame(&Frame, 1000);
		if (m_iResult == FCE_NOERROR)
		{
			m_iResult = m_Camera.PutFrame(&Frame);
			if (Frame.pData)
			{
				memcpy(m_pImageBuffer, Frame.pData, m_iImageWidth * m_iImageHeight);
				bRet = true;
			}
			m_iResult = m_Camera.DiscardFrames();
		}
        else
        {
           m_Camera.StopDevice();
        }
		LOG_INFO("StopDevice_s");
		m_Camera.CloseCapture();
		m_Camera.StopDevice();
		LOG_INFO("StopDevice_e");
		return bRet;
	}
	else
	{
		return false;
	}
}

// 连续采集
bool AVTCam::StartSnap()
{
	if(m_bBusy)
	{
		return false;
	}

	// 启动采集线程
	m_hThread = CreateThread(NULL, 0, ContinousGrab, this, 0, NULL);	
	if (FAILED(m_hThread))
	{
		return false;
	}
	return true;
}

// 停止连续采集
bool AVTCam::StopSnap()
{
	m_bBusy = false;
	Sleep(200);
	return true;
}

// 设置触发模式
bool AVTCam::SetTriggerMode(bool Mode)
{	
	UINT32 nOn;	// 0 = ext. trigger off, 1 = ext. trigger on
	if (Mode){
		nOn = 1;
	}
	else{
		nOn=0;
	}
	 
	UINT32 nPolarity = 0;	// 0 = low active input, 1 = high active input
	UINT32 nMode	 = 0;	// 0 = edge mode, 1 = level mode, 15 = bulk mode
	UINT32 TriggerValue = MAKETRIGGER(nOn, nPolarity, 1, nMode, 0);
	m_Camera.SetParameter(FGP_TRIGGER, TriggerValue);

	return true;
}

// 设置曝光时间
bool AVTCam::SetExposureTime(int iValue)
{
	m_Camera.SetParameter(FGP_SHUTTER, iValue);
	return true;
}

// 设置增益
bool AVTCam::SetGain(int iValue)
{
	m_Camera.SetParameter(FGP_GAIN, iValue);
	return true;
}

// 获取当前曝光时间
int AVTCam::GetExposureTime()
{
	UINT32 iValue = 0;
	m_Camera.GetParameter(FGP_SHUTTER, &iValue);
	return iValue;
}

// 获取当前增益
int AVTCam::GetGain(void)
{
	UINT32 iValue = 0;
	m_Camera.GetParameter(FGP_GAIN, &iValue);
	return iValue;
}

 //获取GUID列表;
int AVTCam::GetGuidList(list<string> &guidList) 
{
	int result = 0;
	UINT32 nodeCount = 0;
	list<string> guidl;

	if (!m_b_FGIintFlag)
	{
		FGInitModule(NULL);
		m_b_FGIintFlag = true;
	}	
	
	FGNODEINFO NodeInfo[20]={};
	if (FCE_NOERROR == result)
	{
		 	result = FGGetNodeList(NodeInfo, 20, &nodeCount);
			for (int i=0;i<20;i++)
			{
				if (NULL != NodeInfo[i].Guid.Low)
				{
					char buf[20] = {};
					itoa(NodeInfo[i].Guid.Low,buf,10);
					guidl.push_back(buf);
				}
			}
		guidList = guidl;				
	}
	return result;
}

int AVTCam::GetParameInfo(FG_PARAMETER speciPara,unsigned long &info)
{
	int result = 0;
	result = m_Camera.GetParameter(speciPara,&info);
	return result;
}
int AVTCam::SetParameInfo(FG_PARAMETER speciPara,int val)
{
	int result = 0;
	result = m_Camera.SetParameter(speciPara,val);
	return result;
}