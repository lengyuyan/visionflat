// TCamara.cpp : 定义 DLL 应用程序的导出函数。
#include "stdafx.h"
#include "common.h"  //有日志
#include "TCamera.h"
TCamera* TCamera::m_this = NULL;

TCamera::TCamera(void)
{
	m_en_camaratype = CAMARA_TYPE_AVT_1394;
	m_iresult = CAMERA_SUCCESS;
}

TCamera* TCamera::GetInstance()
{
	if (NULL == m_this)
	{
		m_this = new TCamera;
	}
	return m_this;
}

void TCamera::ReleaseInstance()
{
    if (NULL != m_this)
    {
        delete m_this;
        m_this = NULL;
    }
}

//初始化相机
int TCamera::Init(int dllnum, string dlllist[])
{
	int result = 0;
	//m_en_camaratype = camaratype;
	int i = 0;
	for (i= 0;i<dllnum;i++)
	{
		list<string> guidlist;
		result = GetGUIDList(guidlist,dlllist[i]);//遍历每种相机名字返回数量
		list<string>::iterator it;
		for (it = guidlist.begin();it != guidlist.end();it++)
		{
			if (NULL != it->c_str())
			{
				Camer *camer = new Camer(it->c_str(),dlllist[i]);//dll类型暂时没用到 
				m_hm_camera.insert(pair<string,Camer*>(it->c_str(),camer));//本来只要插入一次,但是当相机为2时会有问题，这里先还是插入一次
				if (camer->IsBindRealCam())
				{
					m_hm_camera.insert(pair<string,Camer*>(camer->GetCamFullName().c_str(),camer));
				}
			}
		}
	}
	return 0;
}

//释放相机
int TCamera::Deinit()
{
    Camer *pCamera = NULL;
	for (m_it_camera = m_hm_camera.begin();m_it_camera != m_hm_camera.end();m_it_camera++)
	{
		pCamera = GetCamer(m_it_camera->first.c_str());
		if (NULL != pCamera)
		{
			delete pCamera;
			pCamera = NULL;
		}
	}		
	return m_iresult;
}

//打开相机
int TCamera::Open(string name)
{	
    int iResult = 0;
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        iResult = pCamera->open();
	}	
    else
    {
		LOG_ERR("打开相机(%s)失败！",name.c_str());
        iResult = IMAGE_NOINSTANCE;
    }
	return iResult;
}

//获取相机图像大小
int TCamera::GetImageSize(string name, int &w, int &h)
{
    int iResult = 0;
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        iResult = pCamera->getImageSize(w,h);
	}
    else
    {
        iResult = IMAGE_NOINSTANCE;
    }
	return iResult;
}

//关闭相机
int TCamera::Close(string name)
{
    int iResult = 0;
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        iResult = pCamera->close();
	}
	return iResult;
}

//获取相机图像
int TCamera::GetImage(string name, void *pImage)
{
    int iResult = 0;
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        iResult = pCamera->getImage(pImage);
    }
    else
    {
        iResult = IMAGE_NOINSTANCE;
    }
	return iResult;
}

// 设置相机增益
int TCamera::SetGain(string name,unsigned int GainValue) 
{
    int iResult = 0;
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        iResult = pCamera->SetSetGain(GainValue);
    }
    else
    {
        iResult = IMAGE_NOINSTANCE;
    }
	return iResult;
}
// 获取相机增益
int TCamera::GetGain(string name) 
{
    int GainValue = 0;
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        GainValue = pCamera->GetSetGain();
    }
    return GainValue;
}

// 设置相机曝光时间
int TCamera::SetExposureTime(string name,unsigned int ExpTimeValue)
{
    int iResult = 0;
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        iResult = pCamera->SetExposureTime(ExpTimeValue);
    }
    else
    {
        iResult = IMAGE_NOINSTANCE;
    }
	return iResult;
}
// 获取相机曝光时间
int TCamera::GetExposureTime(string name)
{
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        m_iresult = pCamera->GetExposureTime();
    }
    return m_iresult;
}

// 设置相机ID号
int TCamera::GetGUIDList(list<string> &guidlist,string &type)
{
	int ret = Camer::GetGUIDList(guidlist,type);
	return ret;
}

//获取相机参数
int TCamera::GetParameInfo(string name,CAMARA_PARA_ITEM speciPara,unsigned long &info)
{
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        m_iresult = pCamera->GetParameInfo((ICAMERA_PARAMETER)speciPara,info);
	}
	return m_iresult;
}

//设置相机参数
int TCamera::SetParameInfo(string name,CAMARA_PARA_ITEM speciPara,int val)
{
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
    {
        m_iresult = pCamera->SetParameInfo((ICAMERA_PARAMETER)speciPara,val);
	}
	return m_iresult;
}

//开始实时取像
int TCamera::StartSnap(string name,void *pImage,void(*Callback)(void* context),void *pParent)
{
    Camer *pCamera = GetCamer(name);
    if (NULL != pCamera)
	{
		m_iresult = pCamera->StartSnap(pImage,Callback,pParent);
	}
	return m_iresult;
}

//停止实时取像
int TCamera::StopSnap(string name)
{
	Camer *pCamera = GetCamer(name);
	if (NULL != pCamera)
	{
		m_iresult = pCamera->StopSnap();
	}
	return m_iresult;
}

//获取相机对像
Camer *TCamera::GetCamer(string name)
{
    Camer *pCamera = NULL;
    hash_map<string,Camer*>::iterator it;
	it = m_hm_camera.find(name);

	if (m_hm_camera.end() != it)
	{
        pCamera = it->second;
	}
	return pCamera;
}
