// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "manage.h"

int __stdcall DllMain(int hModule,
                       unsigned int ul_reason_for_call,
                       void* lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case 1/*DLL_PROCESS_ATTACH*/:
        //CCard_Service::Get_Instance();
        break;
	case 2/*DLL_THREAD_ATTACH*/:
        break;
	case 3/*DLL_THREAD_DETACH*/:
        break;
    case 0/*DLL_PROCESS_DETACH*/:
        CManage::ReleaseInstance();
		break;
	}
	return 1;
}