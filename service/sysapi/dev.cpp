#include "sysapi.h"

#ifdef __WIN32
#include <guiddef.h> 
#include <windows.h>
#include <setupapi.h>

#pragma comment(lib, "setupapi.lib")

//定义USB设备的guid类型
#define USB_GUID_BUFF {0xa5dcbf10, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed}

EXPORT_C int WINAPI search_usb_dev(const char* keywork)
{
    BOOL bFind = FALSE;
    DWORD dwIndex = 0;
    GUID IID_USB = USB_GUID_BUFF;
    SP_DEVICE_INTERFACE_DATA spDevInterData;        //a structure of device interface data
    PSP_DEVICE_INTERFACE_DETAIL_DATA pSpDIDetailData = NULL;

    memset(&spDevInterData, 0x00, sizeof(SP_DEVICE_INTERFACE_DATA));
    spDevInterData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    HDEVINFO hDeviceInfo = SetupDiGetClassDevs(&IID_USB, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (INVALID_HANDLEVALUE == hDeviceInfo)
    {
        return 0;
    }

    while(TRUE)
    {
        if (!SetupDiEnumDeviceInterfaces(hDeviceInfo, NULL, &IID_USB, dwIndex, &spDevInterData))
        {
            break;
        }

        // get length of interface detail info
        DWORD dwRequiredLength = 0;        //for getting length of inter face detail data
        SetupDiGetDeviceInterfaceDetail(hDeviceInfo, &spDevInterData, NULL, 0, &dwRequiredLength, NULL);

        pSpDIDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwRequiredLength);
        if(NULL == pSpDIDetailData)
        {
            break;
        }

        pSpDIDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SetupDiGetDeviceInterfaceDetail(hDeviceInfo, &spDevInterData, pSpDIDetailData, 
            dwRequiredLength, &dwRequiredLength, NULL);
        if (strstr(pSpDIDetailData->DevicePath, keywork))
        {
            bFind = TRUE;
            break;
        }
        
        HeapFree(GetProcessHeap(), 0, pSpDIDetailData);
        pSpDIDetailData = NULL;

        dwIndex++;
    }

    if (pSpDIDetailData)
    {
        HeapFree(GetProcessHeap(), 0, pSpDIDetailData);
    }
    
    SetupDiDestroyDeviceInfoList(hDeviceInfo);

    return bFind;
}

#else

int WINAPI search_dev(const char* keywork)
{
    return 0;
}

#endif