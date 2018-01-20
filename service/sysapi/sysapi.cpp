/*********************************************************************
    *文件说明: 提供windows api封装,为只维护一份基础库,此文件跟控制平台基础库
	           基本相同
***********************************************************************/
#include "GlobalDefine.h" //把其他工程用到的常用宏放到此文件
#include "GolobalError.h" //把其他工程用到的常用错误码放到此文件
#include "sys_internal.h"
#include "sysapi.h"
#include "serial.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

//线程相关结构体
struct ST_THREAD
{
    PROCEX  fun;
    void*   param;
};

/*static vector<Handle> g_vecHandle;              //句柄栈区
Handle  g_eventHandle = INVALID_HANDLE_VALUE;   //事件
*/

//获取windows错误消息描述
const char* GetLastErrorMsg()
{
    HLOCAL hLocal = NULL;
    DWORD errCode = ::GetLastError();
    static char serrMsgBuff[MAX_PATH] = {0};

    if (!errCode)
    {
        return "";
    }

    memset(serrMsgBuff, 0, MAX_PATH);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, errCode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR)&hLocal, 0, NULL);
    if (hLocal)
    {
		strcpy_s(serrMsgBuff, (PCTSTR)LocalLock(hLocal));
        LocalFree(hLocal);
    }
    return serrMsgBuff;
}

//句柄管理函数
//压入函数，把一个句柄压入管理栈
/*void handle_push(Handle handle)
{
    if (INVALID_HANDLE_VALUE == g_eventHandle)
    {
        g_eventHandle = CreateEvent(NULL, FALSE, TRUE, NULL);
    }
    WaitForSingleObject(g_eventHandle, INFINITE);
    g_vecHandle.push_back(handle);
    SetEvent(g_eventHandle);
}

bool handle_find(Handle handle)
{
    bool bFind = false;
    vector<Handle>::iterator it;
    if (INVALID_HANDLE_VALUE == g_eventHandle)
    {
        return false;
    }
    WaitForSingleObject(g_eventHandle, INFINITE);
    for(it = g_vecHandle.begin(); it != g_vecHandle.end(); it++)
    {
        if (handle == *it)
        {
            TRACEF("find the handle 0x%x\n", handle);
            bFind = true;
            break;
        }
    }
    SetEvent(g_eventHandle);
    return bFind;
}

//从管理栈删除一个句柄，若失败，则说明该句柄不存在
bool handle_erase(Handle handle)
{
    size_t nSize = 0;
    bool bFind = false;
    vector<Handle>::iterator it;
    if (INVALID_HANDLE_VALUE == g_eventHandle ||
        0 == g_vecHandle.size())
    {
        return false;
    }
    WaitForSingleObject(g_eventHandle, INFINITE);
    for(it = g_vecHandle.begin(); it != g_vecHandle.end(); it++)
    {
        if (handle == *it)
        {
            TRACEF("erase handle 0x%x", handle);
            g_vecHandle.erase(it);
            bFind = true;
            break;
        }
    }
    nSize = g_vecHandle.size();
    SetEvent(g_eventHandle);
    if (0 == nSize)
    {
        CloseHandle(g_eventHandle);
        g_eventHandle = INVALID_HANDLE_VALUE;
    }
    return bFind;
}*/

//常规线程执行函数
DWORD WINAPI ThreadFun(void* param)
{
    ST_THREAD* pThread = (ST_THREAD*)param;
    DWORD ret = pThread->fun(pThread->param);
    delete pThread; //创建线程时new 这里delete
    return ret;
}

Handle WINAPI sys_CreateThread(PROCEX lpStartAddress, void* lpParameter)
{
    HANDLE hThread = NULL;
    ST_THREAD* pThread = new ST_THREAD;
    pThread->fun = lpStartAddress;
    pThread->param = lpParameter;

    hThread = CreateThread(NULL, 0, ThreadFun, pThread, 0, NULL);
    if (!hThread)
    {
        TRACEF("CreateThread return error.%d", GetLastError());
        return INVALID_HANDLE;
    }
    return hThread;
}

int WINAPI sys_SuspendThread(Handle hThread)
{
    DWORD ret = SuspendThread(hThread);
    /*if (ret > 0)
    {//成功返回挂起数量
        return RETURN_OK;
    }
    else*/ if (-1 == ret)
    {
        TRACEF("SuspendThread return error.%d", GetLastError());
        return ERR_OPERATOR_THREAD;
    }
    return RETURN_OK;
}

int WINAPI sys_ResumeThread(Handle hThread)
{
    DWORD ret = ResumeThread(hThread);
    /*if (ret > 0)
    {//成功返回挂起数量
        return RETURN_OK;
    }
    else*/ if (-1 == ret)
    {
        TRACEF("ResumeThread return error.%d", GetLastError());
        return ERR_OPERATOR_THREAD;
    }
    return RETURN_OK;
}

int WINAPI sys_TerminateThread(Handle hThread)
{
    if (!TerminateThread(hThread, 100))
    {
        TRACEF("TerminateThread return error.%d", GetLastError());
        return ERR_OPERATOR_THREAD;
    }
    return RETURN_OK;
}

Handle WINAPI sys_CreateEvent(int bManualReset, int bInitialState, const char* lpName)
{
    HANDLE hEvent = NULL;

    hEvent = CreateEventA(NULL, bManualReset, bInitialState, lpName);
    if (!hEvent)
    {
        TRACEF("CreateEvent return error.%d", GetLastError());
        return INVALID_HANDLE;
    }
    return hEvent;
}

int WINAPI sys_WaitForSingleObject(Handle hHandle, uint dwMilliseconds)
{
    DWORD ret = 0;
    DWORD timeout = (MAX_TIMEOUT == dwMilliseconds) ? INFINITE : dwMilliseconds;
    ret = WaitForSingleObject(hHandle, timeout);
    if (WAIT_ABANDONED == ret)
    {
        return ERR_ABANDONED_EVENT;
    }
    else if (WAIT_TIMEOUT == ret)
    {
        return ERR_TIMEOUT;
    }
    else if (WAIT_OBJECT_0 == ret)
    {
        return RETURN_OK;
    }
    TRACEF("WaitForSingleObject return error.%d", ret);
    return ERR_OPERATOR_EVENT;
}

//最大只能等待64个事件
int WINAPI sys_WaitForMultipleObjects(uint nCount, const Handle* lpHandle, int bWaitAll, uint dwMilliseconds)
{
    DWORD ret = 0;
    DWORD timeout = (MAX_TIMEOUT == dwMilliseconds) ? INFINITE : dwMilliseconds;

    RETURN_CHK_NOPRT(nCount <= MAXIMUM_WAIT_OBJECTS, ERR_INPUT_PARAM);

    ret = WaitForMultipleObjects(nCount, lpHandle, bWaitAll, dwMilliseconds);
    if (ret >= WAIT_ABANDONED_0 && ret <= (WAIT_ABANDONED_0 + nCount - 1))
    {
        return ERR_ABANDONED_EVENT;
    }
    else if (WAIT_TIMEOUT == ret)
    {
        return ERR_TIMEOUT;
    }
    else if (ret >= WAIT_OBJECT_0 && ret <= WAIT_OBJECT_0 + nCount - 1)
    {
        return ret - WAIT_OBJECT_0;
    }
    TRACEF("WaitForMultipleObjects return error.%d", ret);
    return ERR_OPERATOR_EVENT;
}

int WINAPI sys_SetEvent(Handle hEvent)
{
    if (!SetEvent(hEvent))
    {
        TRACEF("SetEvent return error.%d", GetLastError());
        return ERR_OPERATOR_EVENT;
    }
    return RETURN_OK;
}

int WINAPI sys_ResetEvent(Handle hEvent)
{
    if (!ResetEvent(hEvent))
    {
        TRACEF("ResetEvent return error.%d", GetLastError());
        return ERR_OPERATOR_EVENT;
    }
    return RETURN_OK;
}

void WINAPI sys_CloseHandle(Handle hObject)
{
    if (INVALID_HANDLE != hObject && hObject)
    {
        if (!CloseHandle(hObject))
        {
            TRACEF("CloseHandle return error.%d", GetLastError());
        }
    }
}

Handle WINAPI sys_CreateMutex(int bInitialOwner, const char* lpName)
{
    HANDLE hMutex = NULL;

    hMutex = CreateMutexA(NULL, bInitialOwner, lpName);
    if (!hMutex)
    {
        TRACEF("CreateMutex return error.%d", GetLastError());
        return INVALID_HANDLE;
    }
    return hMutex;
}

Handle WINAPI sys_OpenMutex(int bInitialOwner, const char* lpName)
{
    HANDLE hMutex = NULL;

    hMutex = OpenMutexA(MUTEX_ALL_ACCESS, bInitialOwner, lpName);
    if (!hMutex)
    {
        TRACEF("CreateMutex return error.%d", GetLastError());
        return INVALID_HANDLE;
    }
    return hMutex;
}

int WINAPI sys_ReleaseMutex(Handle hMutex)
{
    if (!ReleaseMutex(hMutex))
    {
        TRACEF("ReleaseMutex return error.%d", GetLastError());
        return ERR_OPERATOR_MUTEX;
    }
    return RETURN_OK;
}

Handle WINAPI sys_InitializeCriticalSection()
{
    CRITICAL_SECTION* pCSection = new CRITICAL_SECTION;
    //handle_push(pCSection); //压入栈管理
    InitializeCriticalSection(pCSection);
    return pCSection;
}

void WINAPI sys_EnterCriticalSection(Handle hSection)
{
    EnterCriticalSection((CRITICAL_SECTION*)hSection);
}

void WINAPI sys_LeaveCriticalSection(Handle hSection)
{
    LeaveCriticalSection((CRITICAL_SECTION*)hSection);
}

void WINAPI sys_DeleteCriticalSection(Handle hSection)
{
    /*if (handle_erase(hSection))
    {*/
        CRITICAL_SECTION* pCSection = (CRITICAL_SECTION*)hSection;
        DeleteCriticalSection(pCSection);
        delete pCSection;
    //}
}

Handle WINAPI sys_LoadLibrary(const char* lpLibFileName)
{
    DWORD ret = 0;
    HMODULE hLibModule = NULL;
    char* ptmp = NULL;
    char prevPath[MAX_PATH] = { 0 };
    char curPath[MAX_PATH] = { 0 };

    ret = GetCurrentDirectoryA(MAX_PATH, prevPath);
    strcpy_s(curPath, lpLibFileName);
    ptmp = strrchr(curPath, '\\');
    if (ptmp)
    {//路径,则需要设置工作路径
        *ptmp = '\0';   //丢掉文件名，得到路径
        SetDllDirectoryA(curPath);
    }
    hLibModule = LoadLibraryA(lpLibFileName);
    SetDllDirectoryA(prevPath);
    if (!hLibModule)
    {
        TRACEF("LoadLibrary return error.%d:%s", GetLastError(), GetLastErrorMsg());
        return INVALID_HANDLE;
    }
    //handle_push(hLibModule);
    return hLibModule;
}

int WINAPI sys_FreeLibrary(Handle hLibModule)
{
    /*if (handle_erase(hLibModule))
    {*/
        if (FreeLibrary((HMODULE)hLibModule))
        {
            return RETURN_OK;
        }
    //}
    return ERR_INPUT_PARAM;
}

FUN WINAPI sys_GetProcAddress(Handle hModule, const char* lpProcName)
{
    return (FUN)GetProcAddress((HMODULE)hModule, lpProcName);
}

void WINAPI sys_Sleep(uint dwMilliseconds)
{
    Sleep(dwMilliseconds);
}

void WINAPI sys_GetLocalTime(SYS_TIME* lpSysTime)
{
    _SYSTEMTIME time;

    if (!lpSysTime)
    {
        return;
    }

    GetLocalTime(&time);
    lpSysTime->wYear = time.wYear;
    lpSysTime->wMonth = time.wMonth;
    lpSysTime->wDayOfWeek = time.wDayOfWeek;
    lpSysTime->wDay = time.wDay;
    lpSysTime->wHour = time.wHour;
    lpSysTime->wMinute = time.wMinute;
    lpSysTime->wSecond = time.wSecond;
    lpSysTime->wMilliseconds = time.wMilliseconds;
}

EXPORT_C int WINAPI sys_GetCurrentPath(char* lpPath)
{
    if (GetModuleFileNameA(NULL, lpPath, MAX_BUF_LEN - 1) > 0)
    {
        (*strrchr(lpPath, '\\')) = '\0';    //丢掉文件名，得到路径
        return 0;
    }
    return -1;
}
EXPORT_C int WINAPI get_module_path(char* lpPath)
{
	if (GetModuleFileNameA(NULL, lpPath, MAX_BUF_LEN - 1) > 0)
	{
		(*(strrchr(lpPath, '\\')+1)) = '\0';    //丢掉文件名，得到路径
		return 0;
	}
	return -1;
}


Handle WINAPI sys_CreateFile(const char* lpFileName, uint dwAccess, uint dwCreationOption)
{
    HANDLE hFile;
    DWORD dwAcce = 0;

    if (dwAccess & FGENERIC_READ)
    {
        dwAcce |= GENERIC_READ;
    }
    if (dwAccess & FGENERIC_WRITE)
    {
        dwAcce |= GENERIC_WRITE;
    }

    hFile = CreateFileA(lpFileName, dwAcce, 0, NULL, dwCreationOption, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLEVALUE == hFile)
    {
        TRACEF("CreateFile return error.%d:%s", GetLastError(), GetLastErrorMsg());
        return INVALID_HANDLE;
    }
    return hFile;
}

int WINAPI sys_WriteFile(Handle hFile, const void* lpBuffer, uint nBufLen)
{
    DWORD writelen = 0;
    if (!WriteFile(hFile, lpBuffer, nBufLen, &writelen, NULL))
    {
        TRACEF("WriteFile return error.%d:%s", GetLastError(), GetLastErrorMsg());
        return ERR_WRITE_FILE;
    }
    return (int)writelen;
}

int WINAPI sys_ReadFile(Handle hFile, void* lpBuffer, uint nBufSize)
{
    DWORD readLen = 0;
    if (!ReadFile(hFile, lpBuffer, nBufSize, &readLen, NULL))
    {
        TRACEF("ReadFile return error.%d", GetLastError());
        return ERR_READ_FILE;
    }
    return (int)readLen;
}

int WINAPI sys_DeleteFile(const char* lpFileName)
{
    if (!DeleteFileA(lpFileName))
    {
        TRACEF("DeleteFile return error.%d", GetLastError());
        return ERR_DEL_FILE;
    }
    return RETURN_OK;
}

int WINAPI sys_GetFileAttribute(const char* lpFileName)
{
    int iAttr = 0;
    DWORD dwAttr = GetFileAttributesA(lpFileName);
    if (0xFFFFFFFF == dwAttr)
    {
        TRACEF("GetFileAttributes return error.%d", GetLastError());
        return ERR_GET_ATTRIBUTES;
    }

    if (dwAttr & FILE_ATTRIBUTE_NORMAL)
    {
        iAttr |= FATTRIBUTE_NORMAL;
    }
    if (dwAttr & FILE_ATTRIBUTE_COMPRESSED)
    {
        iAttr |= FATTRIBUTE_COMPRESSED;
    }
    if (dwAttr & FILE_ATTRIBUTE_HIDDEN)
    {
        iAttr |= FATTRIBUTE_HIDDEN;
    }
    if (dwAttr & FILE_ATTRIBUTE_READONLY)
    {
        iAttr |= FATTRIBUTE_READONLY;
    }
    if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
    {
        iAttr |= FATTRIBUTE_DIRECTORY;
    }
    return iAttr;
}

void ConvertFileTime(FILETIME& ft, SYS_FILETIME& sft)
{
    sft.dwHighDateTime = ft.dwHighDateTime;
    sft.dwLowDateTime = ft.dwLowDateTime;
}

void ConvertFileData(WIN32_FIND_DATAA& wData, SYS_FIND_DATAA& sData)
{
    strcpy_s(sData.cFileName, wData.cFileName);
    strcpy_s(sData.cAlternateFileName, wData.cAlternateFileName);
    ConvertFileTime(wData.ftCreationTime, sData.ftCreationTime);
    ConvertFileTime(wData.ftLastAccessTime, sData.ftLastAccessTime);
    ConvertFileTime(wData.ftLastWriteTime, sData.ftLastWriteTime);
    sData.dwFileAttributes = wData.dwFileAttributes;
    sData.nFileSizeHigh = wData.nFileSizeHigh;
    sData.nFileSizeLow = wData.nFileSizeLow;
    sData.dwReserved0 = wData.dwReserved0;
    sData.dwReserved1 = wData.dwReserved1;
#ifdef __MAC
    sData.dwFileType = wData.dwFileType;
    sData.dwCreatorType = wData.dwCreatorType;
    sData.wFinderFlags = wData.wFinderFlags;
#endif
}

Handle WINAPI sys_FindFirstFile(const char* lpFileName, LPSYS_FIND_DATAA pFindData)
{
    WIN32_FIND_DATAA data;
    HANDLE hFile = INVALID_HANDLEVALUE;

    if (!pFindData)
    {
        return INVALID_HANDLE;
    }

    memset(&data, 0, sizeof(WIN32_FIND_DATAA));
    hFile = FindFirstFileA(lpFileName, &data);
    if (INVALID_HANDLEVALUE == hFile)
    {
        return INVALID_HANDLE;
    }
    ConvertFileData(data, *pFindData);
    return hFile;
}

int WINAPI sys_FindNextFile(Handle hFindFile, LPSYS_FIND_DATAA pFindData)
{
    WIN32_FIND_DATAA data;

    if (!pFindData)
    {
        return FALSE;
    }

    memset(&data, 0, sizeof(WIN32_FIND_DATAA));
    if (!FindNextFileA(hFindFile, &data))
    {
        return FALSE;
    }
    ConvertFileData(data, *pFindData);
    return TRUE;
}

int WINAPI sys_FindClose(Handle hFindFile)
{
    if (INVALID_HANDLE != hFindFile)
    {
        return FindClose(hFindFile);
    }
    return TRUE;
}

Handle WINAPI sys_CreateFileChangeNotify(const char* lpPathName, int bWatchSubtree, uint dwNotifyFilter)
{
    HANDLE hFile = FindFirstChangeNotificationA(lpPathName, bWatchSubtree, dwNotifyFilter);
    if (INVALID_HANDLEVALUE == hFile)
    {
        return INVALID_HANDLE;
    }
    return hFile;
}

int WINAPI sys_CloseFileChangeNotify(HANDLE hChangeHandle)
{
    return FindCloseChangeNotification(hChangeHandle);
}

int WINAPI sys_OpenCom(const ComParam* pComData)
{
    int ret = 0;
    CSerial* pSerial = new CSerial();
    ret = pSerial->open(pComData);
    if (ret)
    {
        FREE_ANY(pSerial);
        return ret;
    }
    return (int)pSerial;
}

int WINAPI sys_CloseCom(int hCom)
{
    int ret = 0;
    CSerial* pSerial = NULL;
    try
    {
        pSerial = (CSerial*)hCom;
        pSerial->close();
        delete pSerial;
    }
    catch(...)
    {
        ret = ERR_OPEN_COM;
    }
    return ret;
}

int WINAPI sys_ReadCom(int hCom, char* pBuffer, int nBufSize, int timeout/* = 2000*/)
{
    int ret = 0;
    CSerial* pSerial = NULL;
    try
    {
        pSerial = (CSerial*)hCom;
        ret = pSerial->read(pBuffer, nBufSize, timeout);
    }
    catch(...)
    {
        ret = ERR_OPEN_COM;
    }
    return ret;
}

int WINAPI sys_WriteCom(int hCom, const char* pBuffer, int nBufLen)
{
    int ret = 0;
    CSerial* pSerial = NULL;
    try
    {
        pSerial = (CSerial*)hCom;
        ret = pSerial->write(pBuffer, nBufLen);
    }
    catch(...)
    {
        ret = ERR_OPEN_COM;
    }
    return ret;
}

int WINAPI sys_wReadCom(int hCom, char* pBuffer, int nBufLen, int nBufSize, int timeout/* = 2000*/)
{
    int ret = 0;
    CSerial* pSerial = NULL;
    try
    {
        pSerial = (CSerial*)hCom;
        ret = pSerial->wRead(pBuffer, nBufLen, nBufSize, timeout);
    }
    catch(...)
    {
        ret = ERR_OPEN_COM;
    }
    return ret;
}


///下面为控制平台没有的接口
//1，ssk用到的
int WINAPI sys_UnmapViewOfFile(const void* lpBaseAddress)
{
	return UnmapViewOfFile(lpBaseAddress);
}
HANDLE WINAPI sys_OpenFileMapping(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
	return OpenFileMapping(dwDesiredAccess, bInheritHandle, lpName);
}
HANDLE WINAPI sys_CreateFileMapping(HANDLE hFile, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName)
{
	return CreateFileMapping(hFile, NULL, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
}

void* WINAPI sys_MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess)
{
	return MapViewOfFile(hFileMappingObject, dwDesiredAccess, 0, 0, 0);
}
//2，log用到
void WINAPI sys_OutputDebugString(LPCSTR lpOutputString)
{
	return OutputDebugString(lpOutputString);
}