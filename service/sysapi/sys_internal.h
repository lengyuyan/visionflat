/*此头文件主要用来包含系统相关函数，供sysapi内部使用，不应该在其它模块引用此文件
*/
#pragma once
#ifndef _INCLUDE_SYS_H
#define _INCLUDE_SYS_H

//开关
#define ___WINDOWS
//#define ___MAC

#ifdef ___WINDOWS       //windows
#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#endif  //end of window include and comment

#ifdef ___MAC           //MAC

#endif

#endif   //防止重复包含