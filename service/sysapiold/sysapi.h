#ifndef DLL_INCLUDE_COMMON_SYSAPI
#define DLL_INCLUDE_COMMON_SYSAPI
//-----------------------------此处定义所有系统api 及宏-----------------------------//
#pragma warning(disable: 4005)

#pragma once

//sysapi.cpp里面会定义这个宏，所以在这里可以定义一些windows的类型
#ifndef _NO_INCLUDE_SYSAPI

#define WINAPI __stdcall
#define APIENTRY WINAPI

typedef int (WINAPI *PROC)();

typedef struct HINSTANCE__
{
    int unused;
} *HINSTANCE;

typedef HINSTANCE HMODULE;      /* HMODULEs can be used in place of HINSTANCEs */

#define READ_THREAD_PROFILING_FLAG_DISPATCHING        0x00000001
#define READ_THREAD_PROFILING_FLAG_HARDWARE_COUNTERS  0x00000002
#define DLL_PROCESS_ATTACH   1
#define DLL_THREAD_ATTACH    2
#define DLL_THREAD_DETACH    3
#define DLL_PROCESS_DETACH   0
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#define WAIT_TIMEOUT        258L    // dderror

#define MAX_PATH            260     //最大路径长度
#define NULL 0

#define CP_ACP                    0           // default to ANSI code page
#define CP_UTF8                   65001       // UTF-8 translation



typedef unsigned char   BYTE;
typedef char            *LPSTR;
typedef const char      *LPCSTR;
typedef wchar_t         *LPWSTR;
typedef const wchar_t   *LPCWSTR;

typedef void    *HANDLE;
typedef void    *LPVOID;

typedef int     BOOL;
typedef long    LONG;
typedef long    LONG_PTR, *PLONG_PTR;


typedef unsigned int    u_int;
typedef unsigned short  u_short;
typedef unsigned char   UCHAR;
typedef unsigned int    UINT;
typedef unsigned short  WORD;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef unsigned long   DWORD;
typedef unsigned long   u_long;
typedef long long       LONGLONG;
typedef unsigned long   ULONG_PTR;
typedef unsigned int    UINT_PTR, *PUINT_PTR;

typedef UINT_PTR        WPARAM;
typedef LONG_PTR        LPARAM;
typedef UINT_PTR        SOCKET;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#endif

#ifndef INFINITE
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

#define Max(a,b)            (((a) > (b)) ? (a) : (b))
#define Min(a,b)            (((a) < (b)) ? (a) : (b))

#define WAIT_FAILED         ((DWORD)0xFFFFFFFF)
#define WAIT_OBJECT_0       0

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
    LPVOID lpThreadParameter
    );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;


/***********************临界区定义*************************/
#if defined(_M_MRX000) && !(defined(MIDL_PASS) || defined(RC_INVOKED)) && defined(ENABLE_RESTRICTED)
#define RESTRICTED __restrict
#else
#define RESTRICTED
#endif


typedef struct _LIST_ENTRY 
{
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *PRLIST_ENTRY;

typedef struct _RTL_CRITICAL_SECTION_DEBUG {
    WORD   Type;
    WORD   CreatorBackTraceIndex;
    struct _RTL_CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD   CreatorBackTraceIndexHigh;
    WORD   SpareWORD  ;
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, *PRTL_RESOURCE_DEBUG;

typedef struct _RTL_CRITICAL_SECTION 
{
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

    //
    //  The following three fields control entering and exiting the critical
    //  section for the resource
    //

    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;
/**********************************************************/

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;


//socket

#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */


#endif  //结束自定义类型

/***********************临界区定义*************************/
//#if defined(_M_MRX000) && !(defined(MIDL_PASS) || defined(RC_INVOKED)) && defined(ENABLE_RESTRICTED)
//#define RESTRICTED __restrict
//#else
//#define RESTRICTED
//#endif
//
//
//typedef struct _LIST_D {
//	struct _LIST_D *Flink;
//	struct _LIST_D *Blink;
//} List_entry, *PList_entry, *RESTRICTED PList_entry;
//
//typedef struct _RTL_CRITICAL_SECTION_DEBUGG {
//	WORD   Type;
//	WORD   CreatorBackTraceIndex;
//	struct _RTL_CRITICAL_SECTIONG *CriticalSection;
//	List_entry ProcessLocksList;
//	DWORD EntryCount;
//	DWORD ContentionCount;
//	DWORD Flags;
//	WORD   CreatorBackTraceIndexHigh;
//	WORD   SpareWORD  ;
//} RTL_CRITICAL_SECTION_DEBUGG, *PRTL_CRITICAL_SECTION_DEBUGG, RTL_RESOURCE_DEBUGG, *PRTL_RESOURCE_DEBUGG;
//
//typedef struct _RTL_CRITICAL_SECTIONG {
//	PRTL_CRITICAL_SECTION_DEBUGG DebugInfo;
//
//	//
//	//  The following three fields control entering and exiting the critical
//	//  section for the resource
//	//
//
//	LONG LockCount;
//	LONG RecursionCount;
//	HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
//	HANDLE LockSemaphore;
//	ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
//} RTL_CRITICAL_SECTIONG, *PRTL_CRITICAL_SECTIONG;
//
//typedef RTL_CRITICAL_SECTIONG CRITICAL_SECTIONG;
//typedef PRTL_CRITICAL_SECTIONG LPCRITICAL_SECTIONG;
/**********************************************************/


/*-----------------------------socket定义--------------------------------*/
typedef UINT_PTR  SOCKET;
#define MAX_IP_LEN 16
#define WSA_NO_MSG   -1013        //没有消息
#ifndef u_short
typedef unsigned short  u_short;
#endif

struct ST_SOCKADDR
{
    char    ip[MAX_IP_LEN];
    u_short port;
    SOCKET  s;
// 	ST_SOCKADDR()
// 	{
// 		memset(ip,0,MAX_IP_LEN);
// 		port = 0;
// 		s = INVALID_SOCKET;
// 	}
};



bool WINAPI sys_WSAStartup();
void WINAPI sys_WSACleanup();
bool WINAPI sys_ChkNetAsICMP(LPCSTR lpIP);
SOCKET WINAPI sys_socket(int af, int type, int protocol);
int WINAPI sys_closesocket(SOCKET s);
int WINAPI sys_bind(SOCKET s, LPCSTR lpIP, u_short uPort);
int WINAPI sys_listen(SOCKET s, int backlog);
bool WINAPI sys_accept(SOCKET s, ST_SOCKADDR* pclient, int timeout = 0);
int WINAPI sys_connect(SOCKET s, LPCSTR lpServer, u_short uPort);
int WINAPI sys_recv(SOCKET s, LPSTR buf, int len, int timeout = 0);
int WINAPI sys_send(SOCKET s, LPCSTR buf, int len, int flags = 0);
int WINAPI sys_WSAGetLastError();
int WINAPI sys_shutdown(SOCKET s, int how);
int WINAPI sys_getsockname(SOCKET s, ST_SOCKADDR* paddr);

/*---------------------------socket定义结束------------------------------*/

//------------
//DWORD WINAPI sys_GetPrivateProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault,
//	LPCSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName);
//BOOL WINAPI sys_WritePrivateProfileString(LPCSTR lpAppName,LPCSTR lpKeyName,
//	LPCSTR lpString,LPCSTR lpFileName);

//------------
HANDLE WINAPI sys_CreateEvent(BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
DWORD WINAPI sys_WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
DWORD WINAPI sys_WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandle, BOOL bWaitAll, DWORD dwMilliseconds);
int WINAPI sys_SetEvent(HANDLE hEvent);
int WINAPI sys_ResetEvent(HANDLE hEvent);
BOOL WINAPI sys_CloseHandle(HANDLE hObject);
void WINAPI sys_Sleep(DWORD dwMilliseconds);
HANDLE WINAPI sys_CreateThread(LPTHREAD_START_ROUTINE lpStartAddress,void* lpParameter);
HANDLE WINAPI sys_CreateFileMapping(HANDLE hFile, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);
void* WINAPI sys_MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess);
HANDLE WINAPI sys_OpenFileMapping(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
BOOL WINAPI sys_UnmapViewOfFile(const void* lpBaseAddress);
void WINAPI sys_OutputDebugString(LPCSTR lpOutputString);
HANDLE WINAPI sys_CreateMutex(BOOL bInitialOwner, LPCSTR lpName);
HANDLE WINAPI sys_OpenMutex(DWORD dwDesiredAccess, BOOL bInitialOwner, LPCSTR lpName);

void WINAPI sys_InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void WINAPI sys_DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void WINAPI sys_EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void WINAPI sys_LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

HMODULE WINAPI sys_LoadLibrary(LPCSTR lpLibFileName);
BOOL WINAPI sys_FreeLibrary(HMODULE hLibModule);
PROC WINAPI sys_GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

void WINAPI sys_GetLocalTime(LPSYSTEMTIME lpSystemTime);

DWORD WINAPI sys_GetModuleFileName(HMODULE hModule, LPSTR lpFilename, DWORD nSize);

int WINAPI sys_MultiByteToWideChar( UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr,
                            int cbMultiByte, LPWSTR  lpWideCharStr, int cchWideChar);

int WINAPI sys_WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr,
                            int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte);
//----------------------------------系统api定义结束----------------------------------//
#endif
