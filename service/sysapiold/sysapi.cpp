/*********************************************************************
    *文件说明: 提供windows api封装
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/24 CW0793 创建
                
***********************************************************************/
#define _NO_INCLUDE_SYSAPI  //此处包含windows头文件,所以不用自定义类型

#include <windows.h>
#include "sysapi.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

HANDLE WINAPI sys_CreateEvent(BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
    return CreateEvent(NULL, bManualReset, bInitialState, lpName);
}

DWORD WINAPI sys_WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
    return WaitForSingleObject(hHandle, dwMilliseconds);
}

DWORD WINAPI sys_WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandle, BOOL bWaitAll, DWORD dwMilliseconds)
{
    return WaitForMultipleObjects(nCount, lpHandle, bWaitAll, dwMilliseconds);
}

int WINAPI sys_SetEvent(HANDLE hEvent)
{
    return SetEvent(hEvent);
}

int WINAPI sys_ResetEvent(HANDLE hEvent)
{
    return ResetEvent(hEvent);
}

int WINAPI sys_CloseHandle(HANDLE hObject)
{
    return CloseHandle(hObject);
}

void WINAPI sys_Sleep(DWORD dwMilliseconds)
{
    Sleep(dwMilliseconds);
}

HANDLE WINAPI sys_CreateThread(LPTHREAD_START_ROUTINE lpStartAddress,void* lpParameter)
{
    return CreateThread(NULL, 0, lpStartAddress, lpParameter, 0, NULL);
}

HANDLE WINAPI sys_CreateFileMapping(HANDLE hFile, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName)
{
    return CreateFileMapping(hFile, NULL, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
}

void* WINAPI sys_MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess)
{
    return MapViewOfFile(hFileMappingObject, dwDesiredAccess, 0, 0, 0);
}

HANDLE WINAPI sys_OpenFileMapping(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
    return OpenFileMapping(dwDesiredAccess, bInheritHandle, lpName);
}

BOOL WINAPI sys_UnmapViewOfFile(const void* lpBaseAddress)
{
    return UnmapViewOfFile(lpBaseAddress);
}

void WINAPI sys_OutputDebugString(LPCSTR lpOutputString)
{
    return OutputDebugString(lpOutputString);
}

HANDLE WINAPI sys_CreateMutex(BOOL bInitialOwner, LPCSTR lpName)
{
    return CreateMutex(NULL, bInitialOwner, lpName);
}

HANDLE WINAPI sys_OpenMutex(DWORD dwDesiredAccess, BOOL bInitialOwner, LPCSTR lpName)
{
    return OpenMutex(dwDesiredAccess, bInitialOwner, lpName);
}


void WINAPI sys_InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	return InitializeCriticalSection(lpCriticalSection);
}
void WINAPI sys_EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	return EnterCriticalSection(lpCriticalSection);
}
void WINAPI sys_LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	return LeaveCriticalSection(lpCriticalSection);
}

void WINAPI sys_DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	return DeleteCriticalSection(lpCriticalSection);
}

HMODULE WINAPI sys_LoadLibrary(LPCSTR lpLibFileName)
{
    return LoadLibraryA(lpLibFileName);
}

BOOL WINAPI sys_FreeLibrary(HMODULE hLibModule)
{
    return FreeLibrary(hLibModule);
}

PROC WINAPI sys_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    return GetProcAddress(hModule, lpProcName);
}

void WINAPI sys_GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    GetLocalTime(lpSystemTime);
}

DWORD WINAPI sys_GetModuleFileName(HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
    return GetModuleFileName(hModule, lpFilename, nSize);
}

int WINAPI sys_MultiByteToWideChar( UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr,
                            int cbMultiByte, LPWSTR  lpWideCharStr, int cchWideChar)
{
    return MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr,cbMultiByte, lpWideCharStr, cchWideChar);
}

int WINAPI sys_WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr,
    int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte)
{
    return WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar,
                            lpMultiByteStr, cbMultiByte, NULL, NULL);
}