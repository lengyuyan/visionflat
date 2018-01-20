#include "StdAfx.h"
#include "VisionProcBase.h"


VisionProcBase::VisionProcBase(void)
{
}


VisionProcBase::~VisionProcBase(void)
{
}

VisionProcBase* VisionProcBase::CreateProcInstance(char* szName)
{
    //VisionProcBase* pProcBase = new VisionProcBase; 
	return this;
}


void VisionProcBase::VisionProc(...)
{
	
}
