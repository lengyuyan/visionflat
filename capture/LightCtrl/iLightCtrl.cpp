#include "stdafx.h"
#include "iLightCtrl.h"
#include "TLight.h"


static TLight *instance = NULL;
int iLightInstance(LightInfo seriaInfo[],int cout, LightTypeInfo *pType, int iTypeCount) 
{
	if (NULL == instance)
	{
		instance = TLight::getInstance();
	}
	return instance->Init(seriaInfo,cout, pType, iTypeCount);
}
void destroyLightInstance()
{
    TLight::destroyInstance();
}

int deInitLightCtrl(string comName)
{
	RETURN_NULLPTR(instance,LIGHTCTL_NOINSTANCE);
	return instance->DeInit(comName);
}

int openLight(string comName,int index)
{
	RETURN_NULLPTR(instance,LIGHTCTL_NOINSTANCE);
	return instance->Open(comName,index);
}

int closeLight(string comName,int index)
{
	RETURN_NULLPTR(instance,LIGHTCTL_NOINSTANCE);
	return instance->Close(comName,index);
}

int setLightValue(string comName,int index ,unsigned int value)
{
	RETURN_NULLPTR(instance,LIGHTCTL_NOINSTANCE);
	return instance->SetLightValue(comName,index,value);
}

int getLightValue(string comName,int index ,unsigned int &val)
{
	RETURN_NULLPTR(instance,LIGHTCTL_NOINSTANCE);
	return instance->GetLightValue(comName,index,val);
}
int JudgeLightConnect(string comName)
{
    RETURN_NULLPTR(instance,LIGHTCTL_NOINSTANCE);
    return instance->JudgeLightConnect(comName);
}