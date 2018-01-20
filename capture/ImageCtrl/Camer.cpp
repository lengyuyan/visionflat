#include "StdAfx.h"
#include "Camer.h"
#include "sysapi.h"
#include "DataType.h"//包含错误码
#include "common.h" //打印日志

typedef   CameraHANDLE ( WINAPI *createCamera)(char*pszCamerName);
typedef   void (WINAPI *releaseCamera)(CameraHANDLE handle);
typedef   int (WINAPI *getCameraList)(list<string>&,string&);

map<string ,HMODULE>Camer::m_maphdll;//静态成员初始化
int Camer::m_camnun = 0;

Camer::Camer(const char *pszCamerName,string &type)
{
	InitializeCriticalSection(&m_CriticalSection); 
	m_cam = NULL;//底层相机指针
	memset(m_szCamerName,0,sizeof(m_szCamerName));
	strncpy_s(m_szCamerName, pszCamerName, sizeof(m_szCamerName)-1);
	m_bisopen = false;//构造实例时为没有打开相机
	const char * mystr=type.c_str();
	map<string,HMODULE>::iterator itmap;
	itmap = m_maphdll.find(type);
	if (itmap == m_maphdll.end() )
	{
		LOG_ERR("加载相机dll:%s失败！",mystr);
		return;
	}
	
	//hdll = (HMODULE)sys_LoadLibrary(mystr);
	hdll = itmap->second;
	//if ((HMODULE)0 == hdll || INVALID_HANDLE == hdll )
	//{
	//	LOG_ERR("加载相机dll:%s失败！",mystr);
	//	return;
	//}
	createCamera createcameFUN = NULL;//函数指针
	createcameFUN = (createCamera)sys_GetProcAddress(hdll, "CreateCamera");
	if (createcameFUN)
	{
		m_cam = createcameFUN(m_szCamerName);	
	}
	else
	{
		sys_FreeLibrary(hdll);
	}
	if (!m_cam)
	{
		LOG_ERR("打开相机失败!");
	}
	m_camnun++;
	
}

Camer::~Camer()
{
	DeleteCriticalSection(&m_CriticalSection);
	if (m_cam)
	{
		m_cam->UnInitCam();
	}
	releaseCamera releasecameFUN = (releaseCamera)sys_GetProcAddress(hdll, "ReleaseCamera");
	if (releasecameFUN)
	{ 
		releasecameFUN(m_cam);
	}
	if (--m_camnun == 0)
	{
		sys_FreeLibrary(hdll);///要不要每个相机都加载一次	
	}
	
}

int Camer::open()
{
	char *stopstring = NULL;
	unsigned index = strtoul(m_szCamerName,&stopstring,10); ///这里需要修改根据名字来初始化打开相机
	if (!m_cam)
	{
		LOG_ERR("打开相机失败!");
		return 0;
	}
	if (!m_cam->InitCam(m_szCamerName))
	{
		return true;
	}
	m_bisopen = true;
	return 0;
}

int Camer::close()
{
	try
	{

	}
	catch(...)
	{

	}
	
	return 0;
}

string Camer::GetCamFullName()
{
	char szText[512] ={0};
	if (m_cam)
	{
		int sz = sizeof(szText);
		memcpy(szText,m_cam->GetFullName(),sizeof(szText)-1);
		return szText;
	}else
	{
	  return  "";
	}
	

}


int Camer::getImageSize(int &w, int &h)
{
	if (!m_cam)
	{
		LOG_ERR("相机为空!");
		return -1;
	}
	w = m_cam->GetImageWidth();
	h = m_cam->GetImageHeight();
	return 0;
}

int Camer::getImage(void *pImage)
{
	if ( NULL == pImage && (!m_cam))
	{
		if (!m_cam)
		{
			LOG_ERR("相机为空");

		}
		else
		{
			LOG_ERR("pImage为空");
		}
		return 1;
	}
	bool bRet = false;
	try
	{
		// 加锁
		EnterCriticalSection(&m_CriticalSection);
		m_cam->SetBuffer((BYTE*)pImage);
		bRet = m_cam->SingleSnap(false);
	}
	catch (...)
	{
		LeaveCriticalSection(&m_CriticalSection);
		return 1;
	}
	LeaveCriticalSection(&m_CriticalSection);
	if (bRet)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	
}

int Camer::SetSetGain(unsigned int GainValue)
{
	bool result = false;
	if (!m_cam)
	{
		LOG_ERR("相机为空");
		return 0;

	}
	result = m_cam->SetGain(GainValue);
	if (result)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int Camer::GetSetGain()
{
    int iValue = 8;
	if (!m_cam)
	{
		LOG_ERR("相机为空");
		return 0;

	}
    iValue = m_cam->GetGain();

    return iValue;
}

int Camer::SetExposureTime(unsigned int ExpTimeValue)
{
	bool result = false;
	if (!m_cam)
	{
		LOG_ERR("相机为空");
		return 0;

	}
	result = m_cam->SetExposureTime(ExpTimeValue);
	if (result)
	{
		return 0;
	}
	else
	{
		return 1;
	}	
}
int Camer::GetExposureTime()
{
	if (!m_cam)
	{
		LOG_ERR("相机为空");
		return 0;

	}
    return m_cam->GetExposureTime();
}



int  Camer::GetGUIDList(list<string> &guidlist,string &type)
{
	int res = 0;
	int i =0;
	HMODULE hDll;
	map<string,HMODULE>::iterator itmap;
	itmap = m_maphdll.find(type);
	if (itmap == m_maphdll.end())
	{
		
		hDll = (HMODULE)sys_LoadLibrary(type.c_str());
		if ((HMODULE)0 == hDll || INVALID_HANDLE == hDll )
		{
			LOG_ERR("加载dll：%s失败",type.c_str());
			res = DLL_NOEXIST;
			m_maphdll.clear();
			return res;
		}
		m_maphdll.insert(make_pair(type,hDll));
	}
	else
	{
		hDll = itmap->second;
	}
	getCameraList findcamlist;
	findcamlist =  (getCameraList)sys_GetProcAddress(hDll, "getList");
	//遍历相机
	if (!findcamlist)
	{
		sys_FreeLibrary(hDll);
		res = FIND_NOCAM;
		return res;
	}
	res = findcamlist(guidlist,type);
	///先释放，在构造函数中再加载，这样做的目的是这里是局部句柄
	//sys_FreeLibrary(hDll);

	list<string>::iterator it;
	for (it = guidlist.begin();it != guidlist.end();it++)
	{
		if (NULL != it->c_str())
		{   
			LOG_INFO("guidlist[%s]",it->c_str());
			i++; ///没用
		}
	}
	////读取配置文件，看是否加载别的相机来遍历
	return res;		
}

int Camer::GetParameInfo(ICAMERA_PARAMETER speciPara,unsigned long &info)
{
	if (!m_bisopen)
	{
		open();
	}
	if (!m_cam)
	{
		LOG_ERR("相机为空");

	}
	m_cam->GetParameInfo(speciPara,info);
	return 0;
}

int Camer::SetParameInfo(ICAMERA_PARAMETER speciPara,int val)
{
	if (!m_cam)
	{
		LOG_ERR("相机为空");

	}
	return m_cam->SetParameInfo(speciPara,val);
}

int Camer::StartSnap( void *pImage,void(*Callback)(void* context),void *pParent)
{
	if (!m_cam)
	{
		LOG_ERR("相机为空");

	}
	m_cam->SetBuffer((BYTE *)pImage);
	m_cam->SetCallBack(Callback,pParent);
	bool result = m_cam->StartSnap();
	if (result)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int Camer::StopSnap( )
{
	if (!m_cam)
	{
		LOG_ERR("相机为空");
		return -1;

	}
	m_cam->StopSnap();
	return 0;
}

bool Camer::IsBindRealCam()
{
	if (m_cam)
	{
		return true;
	}
	return false;
}