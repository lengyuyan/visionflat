#include "stdafx.h"
#include "iImageCtrl.h"
#include "common.h"
#include "TCamera.h"


static TCamera *instance = NULL;

int imageInstance(int dllnum, string dlllist[])
{
	if (NULL == instance)
	{
		instance = TCamera::GetInstance();
	}
	return instance->Init(dllnum,dlllist);
}

int deinitImage()
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
    int iRet = 0;
    iRet = instance->Deinit();
    TCamera::ReleaseInstance();
    instance = NULL;
	return iRet;
}	

int getGUIDList(std::list<std::string> &guidlist,int dllnum,string dlllist[])
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	int i = 0 ;
	for (i= 0;i<dllnum;i++)
	{
		list<string> guidlisttemp;
		int result = instance->GetGUIDList(guidlisttemp,dlllist[i]);


		list<string>::iterator it;
		for (it = guidlisttemp.begin();it != guidlisttemp.end();it++)
		{
			if (NULL != it->c_str())
			{
				guidlist.push_back(it->c_str());
			}
		}
	}
	///不关心返回的列表
	return 0;
}

int getParameInfo(string name,CAMARA_PARA_ITEM speciPara,unsigned long &val)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->GetParameInfo(name,speciPara,val);
}

int setParameInfo(string name,CAMARA_PARA_ITEM speciPara,int val)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->SetParameInfo(name,speciPara,val);
}

int openCamera (string name )
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->Open(name);
}

int closeCamera (string name)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->Close(name);
}

int getImageSize(string name ,int &w, int &h)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->GetImageSize(name,w,h);
}

int getImage(string name, void *pImage)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->GetImage(name,pImage);
}

int setGain(string name,unsigned int value)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->SetGain(name,value);
}
int getGain(string name)
{
    RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
    return instance->GetGain(name);
}
int setExposureTime(string name,unsigned int value)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->SetExposureTime(name,value);
}
int getExposureTime(string name)
{
    RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
        return instance->GetExposureTime(name);
}

int startSnap(string name,void *pImage,void(*Callback)(void* context),void *pParent)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->StartSnap(name,pImage,Callback,pParent);
}

int stopSnap(string name)
{
	RETURN_NULLPTR(instance,IMAGE_NOINSTANCE)
	return instance->StopSnap(name);
}