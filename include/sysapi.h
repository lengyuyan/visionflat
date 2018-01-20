#pragma once
#ifndef DLL_INCLUDE_COMMON_SYSAPI
#define DLL_INCLUDE_COMMON_SYSAPI

#define SYSAPI_VERSION "sysapi_1.1.1.20170122_beta"

#include "GlobalDefine.h" //全局宏定义



/*-------宏定义，决定使用哪个平台*/
#define __WIN32
//#define __MAC
//#define __LINUX
/*--------宏定义结束------------*/

#ifdef __WIN32
#include <direct.h>
#include <iomanip>
#include  <io.h>
#include <stdarg.h>
#else
#include <dir.h>
#endif

//#ifdef _DEBUG
//#define TRACEF printf
//#else
#define TRACEF
//#endif

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1

/*-----------------------------socket定义--------------------------------*/

#ifndef WINAPI
#define WINAPI __stdcall
#endif



typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned char   uchar;
typedef const char*     lcpstr;

#define UINTMAX     ((uint)~((uint)0))
#define INTMAX      ((int)(UINTMAX >> 1))
#define INTMIN      ((int)~INTMAX)

#define MAX_NAME_LEN        32
#define MAX_IP_LEN          16
#define MAX_BUF_LEN         256
#define MAX_TIMEOUT         0xFFFFFFFF              //无限等待
#define ANY_IP              "0.0.0.0"               //ip通用

#define NULL                0
#define INVALID_SKT         (int)~0                 //无效socket
#define INVALID_HANDLE      (Handle)-1              //无效句柄
#define INVALID_HANDLE_INT  -1                      //无效句柄

#define EXPORT_C extern "C"

struct ST_SOCKADDR
{
    char    ip[MAX_IP_LEN];
    uint    port;
    int     s;
};

#define AF_INET         2               /* internetwork: UDP, TCP, etc. */

/*
 * Types
 */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */


/*
 * Protocols
 */
//跟控制平台不一样,这里要先取消,否则跟communicationCtrl中的ws2def.h重复定义了
//#define IPPROTO_IP              0               /* dummy for IP */
//#define IPPROTO_ICMP           1               /* control message protocol */
//#define IPPROTO_IGMP            2               /* group management protocol */
//#define IPPROTO_GGP            3               /* gateway^2 (deprecated) */
//#define IPPROTO_TCP             6               /* tcp */
//#define IPPROTO_PUP             12              /* pup */
//#define IPPROTO_UDP             17              /* user datagram protocol */
//#define IPPROTO_IDP            22              /* xns idp */
//#define IPPROTO_ND              77              /* UNOFFICIAL net disk proto */
//#define IPPROTO_RAW2             255             /* raw IP packet */


/****************************************************
    *Function: 初始化网络协议栈
    *Intput: 无
    *Output: 无
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_InitNet();

/****************************************************
    *Function: 释放网络协议栈
    *Intput: 无
    *Output: 无
    *Return: 无
******************************************************/
EXPORT_C void WINAPI sys_UninitNet();

/****************************************************
    *Function: 检测一个客户端网络连接是否ok
    *Intput: lpClient 客户端IP地址
    *Output: 无
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_ChkNet(const char* lpClient);

/****************************************************
    *Function: 创建一个socket
    *Intput: af     AF_INET 支持TCP UDP等通用协议
             type   一般选择 SOCK_STREAM 流协议
             protocol  TCP为6 UDP为17，根据平台可能会不同
    *Output: 无
    *Return: 成功返回socket句柄，失败返回 INVALID_SKT
    调用get_last_err获取错误代码
******************************************************/
EXPORT_C int WINAPI sys_Socket(int af, int type, int protocol);

/****************************************************
    *Function: 关闭一个socket
    *Intput: s socket句柄
    *Output: 无
    *Return: 无
******************************************************/
EXPORT_C void WINAPI sys_CloseSocket(int s);

/****************************************************
    *Function: 绑定一个socket到某个端口
    *Intput: s socket句柄
             lpIP ip地址
             uPort 端口
    *Output: 无
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_Bind(int s, const char* lpIP, ushort uPort);

/****************************************************
    *Function: 监听一个服务端socket
    *Intput: s socket句柄
             maxCnt 最大等待连接数量,同时连接时只支持
             最大数量连接，比如最大为3，如果4个客户端
             同时连接，会有一个失败,等其中一个连接ok，
             再连就能成功
    *Output: 无
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_Listen(int s, int maxCnt);

/****************************************************
    *Function: 接受一个客户端连接，只支持服务端socket
    *Intput: s     socket句柄
             timeout 超时设置，默认永远等待
    *Output: pclient 接受的客户端信息 包括通讯用的socket
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_Accept(int s, ST_SOCKADDR* pclient, int timeout = MAX_TIMEOUT);

/****************************************************
    *Function: 使用socket连接服务端
    *Intput: s     socket句柄
             lpServer  服务端IP
             uPort  服务端端口
    *Output: 无
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_Connect(int s, const char* lpServer, ushort uPort);

/****************************************************
    *Function: 同步接收一个socket消息, 先select判断，再接收，否则死等
    *Intput: s     socket句柄
             len   接收缓冲区大小 判断连接时 len = 0
    *Output: 无
    *Return: 成功返回接收到的字符串长度，失败返回错误码
    *PS: 返回 ERR_NO_CONNECT 说明socket已经断开连接
******************************************************/
EXPORT_C int WINAPI sys_Recv(int s, char* buf, int len);

/****************************************************
    *Function: 判断一组socket是否有消息可接收
    *Intput: s     socket句柄数组
             count socket数组大小
             len   接收缓冲区大小 判断连接时 len = 0
             timeout  超时设置，默认永远等待
    *Output: 无
    *Return: 成功返回有消息触发的s索引，失败返回错误
******************************************************/
EXPORT_C int WINAPI sys_Select(int* s, int count, int timeout = MAX_TIMEOUT);

/****************************************************
    *Function: 发送一个socket消息
    *Intput: s     socket句柄
             buf   要发送的消息内容
             len   要发送的内容大小
    *Output: 无
    *Return: 成功返回发送数据长度，失败返回错误码
    *PS: 返回 ERR_NO_CONNECT 说明socket已经断开连接
******************************************************/
EXPORT_C int WINAPI sys_Send(int s, const char* buf, int len);

/****************************************************
    *Function: 获取一个socket的相关信息(ip,port)
    *Intput: s     socket句柄
    *Output: paddr 获取的信息结构体缓冲区
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_GetSockName(int s, ST_SOCKADDR* paddr);

/****************************************************
    *Function: 获取本地所有IP地址， 返回数量 存入ips串， 以0分割
    *Intput: 无
    *Output: ips ip地址合集，以0分割 双0结尾
    *Return: 成功返回获取到的ip数量，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_local_ips(char* ips);
/*---------------------------socket定义结束------------------------------*/


typedef struct SYS_TIME //时间结构体
{
    ushort wYear;
    ushort wMonth;
    ushort wDayOfWeek;
    ushort wDay;
    ushort wHour;
    ushort wMinute;
    ushort wSecond;
    ushort wMilliseconds;
} *LPSYS_TIME;

typedef struct SYS_FILETIME {
    uint dwLowDateTime;
    uint dwHighDateTime;
} *LPSYS_FILETIME;

typedef struct SYS_FIND_DATAA
{
    uint dwFileAttributes;
    SYS_FILETIME ftCreationTime;
    SYS_FILETIME ftLastAccessTime;
    SYS_FILETIME ftLastWriteTime;
    uint nFileSizeHigh;
    uint nFileSizeLow;
    uint dwReserved0;
    uint dwReserved1;
    char   cFileName[MAX_BUF_LEN];
    char   cAlternateFileName[14];
#ifdef __MAC
    uint dwFileType;
    uint dwCreatorType;
    uint wFinderFlags;
#endif
} *LPSYS_FIND_DATAA;

//查找文件修改参数
enum EM_FILE_NOTIFY_OPTION
{
    SYS_FILE_NOTIFY_CHANGE_NAME         =   0x001,  //文件名
    SYS_FILE_NOTIFY_CHANGE_DIR_NAME     =   0x002,  //路径
    SYS_FILE_NOTIFY_CHANGE_ATTRIBUTES   =   0x004,  //属性
    SYS_FILE_NOTIFY_CHANGE_SIZE         =   0x008,  //大小
    SYS_FILE_NOTIFY_CHANGE_LAST_WRITE   =   0x010,  //最后修改时间
    SYS_FILE_NOTIFY_CHANGE_LAST_ACCESS  =   0x020,  //最后读取、修改
    SYS_FILE_NOTIFY_CHANGE_CREATION     =   0x040,  //创建时间
    SYS_FILE_NOTIFY_CHANGE_SECURITY     =   0x100,  //访问权限
};

typedef void* Handle;
typedef int (WINAPI *PROCEX)(void* param);
typedef int (WINAPI *FUN)();

/****************************************************
    *Function: 创建一个普通线程
    *Intput: lpStartAddress 线程执行函数地址
             lpParameter    执行函数附加参数
    *Output: 无
    *Return: 返回线程句柄，失败返回 INVALID_HANDLE
******************************************************/
EXPORT_C Handle WINAPI sys_CreateThread(PROCEX lpStartAddress, void* lpParameter);

/****************************************************
    *Function: 挂起一个线程
    *Intput: hThread 线程句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_SuspendThread(Handle hThread);

/****************************************************
    *Function: 恢复一个线程，挂起时才能使用
    *Intput: hThread 线程句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_ResumeThread(Handle hThread);

/****************************************************
    *Function: 强制终止一个线程
    *Intput: hThread 线程句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_TerminateThread(Handle hThread);

/****************************************************
    *Function: 创建一个事件
    *Intput: bManualReset 是否为手动设置信号，自动模式，则等待到信号自动置为无信号
             bInitialState 初始信号
             lpName 事件名称，可以为NULL，垮进程调用时，应该设置为有名事件
    *Output: 无
    *Return: 返回事件句柄，失败返回 INVALID_HANDLE
******************************************************/
EXPORT_C Handle WINAPI sys_CreateEvent(int bManualReset, int bInitialState, const char* lpName);

/****************************************************
    *Function: 等待一个事件有信号
    *Intput: hHandle 事件句柄
             dwMilliseconds 等待时间，单位ms TIME_MAX 为无限等待
             lpName 事件名称，可以为NULL，垮进程调用时，应该设置为有名事件
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_WaitForSingleObject(Handle hHandle, uint iMilliseconds = MAX_TIMEOUT);

/****************************************************
    *Function: 等待多个事件有信号, 有信号时，返回最小
    索引值的event，不返回的可以继续调用sys_WaitForMultipleObjects
    等待，直到没有信号为止
    *Intput: nCount 事件数组大小
             lpHandle 事件数组首地址
             bWaitAll 是否等待所有事件有信号 0 表示不是
             dwMilliseconds 等待时间，单位ms TIME_MAX 为无限等待
    *Output: 无
    *Return: 成功返回数组中有信号的最小索引值，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_WaitForMultipleObjects(uint nCount, const Handle* lpHandle, int bWaitAll, uint dwMilliseconds = MAX_TIMEOUT);

/****************************************************
    *Function: 设置一个事件为有信号
    *Intput: hEvent 事件句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_SetEvent(Handle hEvent);

/****************************************************
    *Function: 设置一个事件为无信号
    *Intput: hEvent 事件句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_ResetEvent(Handle hEvent);

/****************************************************
    *Function: 设置一个事件为无信号
    *Intput: hEvent 事件句柄
    *Output: 无
    *Return: 无
******************************************************/
EXPORT_C void WINAPI sys_CloseHandle(Handle hObject);

/****************************************************
    *Function: 创建一个互斥量，基于进程间的
    *Intput: bInitialOwner 是否标识自己为所有者 0为假
             lpName  互斥量名称
    *Output: 无
    *Return: 返回互斥量事件句柄
******************************************************/
EXPORT_C Handle WINAPI sys_CreateMutex(int bInitialOwner, const char* lpName);

/****************************************************
    *Function: 打开一个互斥量，基于进程间的
    *Intput: bInitialOwner 是否标识自己为所有者 0为假
             lpName  互斥量名称
    *Output: 无
    *Return: 返回互斥量事件句柄
******************************************************/
EXPORT_C Handle WINAPI sys_OpenMutex(int bInitialOwner, const char* lpName);

/****************************************************
    *Function: 释放一个互斥量
    *Intput: bInitialOwner 是否标识自己为所有者 0为假
             lpName  互斥量名称
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_ReleaseMutex(Handle hMutex);

/****************************************************
    *Function: 初始化一个临界区，跨线程同步用
    *Intput: 无
    *Output: 无
    *Return: 返回一个句柄
******************************************************/
EXPORT_C Handle WINAPI sys_InitializeCriticalSection();

/****************************************************
    *Function: 删除临界区句柄
    *Intput: hSection 互斥量句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C void WINAPI sys_DeleteCriticalSection(Handle hSection);

/****************************************************
    *Function: 进入临界区
    *Intput: hSection 互斥量句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C void WINAPI sys_EnterCriticalSection(Handle hSection);

/****************************************************
    *Function: 离开临界区
    *Intput: hSection 互斥量句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C void WINAPI sys_LeaveCriticalSection(Handle hSection);

/****************************************************
    *Function: 动态加载一个dll
    *Intput: lpLibFileName 模块名包括.dll
    *Output: 无
    *Return: 返回一个模块句柄, 失败返回 INVALID_HANDLE
******************************************************/
EXPORT_C Handle WINAPI sys_LoadLibrary(const char* lpLibFileName);

/****************************************************
    *Function: 释放一个模块
    *Intput: hLibModule 模块句柄
    *Output: 无
    *Return: 返回错误代码 EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI sys_FreeLibrary(Handle hLibModule);

/****************************************************
    *Function: 获取一个模块中的函数接口地址
    *Intput: hModule     模块句柄
             lpProcName  函数名
    *Output: 无
    *Return: 成功返回函数地址，失败返回空
******************************************************/
EXPORT_C FUN WINAPI sys_GetProcAddress(Handle hModule, const char* lpProcName);

/****************************************************
    *Function: 使当前线程休眠一段时间
    *Intput: dwMilliseconds 休眠时间 单位ms
    *Output: 无
    *Return: 无
******************************************************/
EXPORT_C void WINAPI sys_Sleep(uint dwMilliseconds);

/****************************************************
    *Function: 获取本地时间
    *Intput: lpSysTime   时间结构体
    *Output: 无
    *Return: 无
******************************************************/
EXPORT_C void WINAPI sys_GetLocalTime(SYS_TIME* lpSysTime);

//---文件操作---
//创建文件访问方式
enum SYS_FILE_ACESS
{
    FGENERIC_READ    = 0x01,   //读取 可与 SYS_GENERIC_WRITE 或操作
    FGENERIC_WRITE   = 0x02,   //写入 可与 SYS_GENERIC_READ  或操作
};

//文件打开方式
enum SYS_FILE_OPEN
{
    SYS_CREATE_NEW          =   1,  //创建一个新的文件，若存在则失败
    SYS_CREATE_ALWAYS       =   2,  //创建一个新的文件，若存在则覆盖
    SYS_OPEN_EXISTING       =   3,  //打开一个已经存在的文件，若不存在，则失败
    SYS_OPEN_ALWAYS         =   4,  //打开一个已经存在的文件，若不存在，则创建
    SYS_TRUNCATE_EXISTING   =   5,  //打开一个已经存在的文件并置空，若不存在，则失败
};

//文件属性
enum SYS_FILE_ATTRIBUTE
{
    FATTRIBUTE_NORMAL = 0x01,       //普通文件
    FATTRIBUTE_COMPRESSED = 0x02,   //压缩文件
    FATTRIBUTE_HIDDEN = 0x04,       //隐藏文件或目录
    FATTRIBUTE_READONLY = 0x08,     //只读文件或目录
    FATTRIBUTE_DIRECTORY = 0x10,    //目录，文件夹
};

/****************************************************
    *Function: 获取当前exe/dll所在绝对路径
    *Intput:  NULL
    dwAccess  lpPath 路径缓冲区
    *Output: 无
    *Return: 成功返回0，失败返回错误吗
******************************************************/
EXPORT_C int WINAPI sys_GetCurrentPath(char* lpPath);

/****************************************************
    *Function: 获取当前exe/dll所在绝对路径
    *Intput:  NULL
    dwAccess  lpPath 路径缓冲区
    *Output: 无
    *Return: 成功返回0，失败返回错误吗
******************************************************/
EXPORT_C int WINAPI get_module_path(char* lpPath);

/****************************************************
    *Function: 创建一个文件，可用于串口，及文件操作
    *Intput: lpFileName         文件名
             dwAccess    访问方式 参考 SYS_FILE_ACESS
             dwCreationOption  打开方式，参考 SYS_FILE_OPEN
    *Output: 无
    *Return: 无
******************************************************/
EXPORT_C Handle WINAPI sys_CreateFile(const char* lpFileName, uint dwAccess, uint dwCreationOption);

/****************************************************
    *Function: 往一个已经打开的文件写数据
    *Intput: hFile         文件句柄
             lpBuffer      待写入数据
             nBufLen       待写入数据大小
    *Output: NULL
    *Return: 成功返回写入的长度，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_WriteFile(Handle hFile, const void* lpBuffer, uint nBufLen);

/****************************************************
    *Function: 从一个已经打开的文件读数据
    *Intput: hFile          文件句柄
             lpBuffer       数据缓冲区
             nBufSize       缓存区大小
    *Output: NULL
    *Return: 成功返回读取的长度，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_ReadFile(Handle hFile, void* lpBuffer, uint nBufSize);

/****************************************************
    *Function: 删除一个文件
    *Intput: lpFileName 文件详细路径
    *Output: NULL
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_DeleteFile(const char* lpFileName);

/****************************************************
    *Function: 获取一个文件的属性
    *Intput: lpFileName 文件详细路径
    *Output: NULL
    *Return: 成功返回一个整型属性值,失败返回错误码
    *PS 返回值类型请参考 SYS_FILE_ATTRIBUTE 可能是一个
    或多个的和值
******************************************************/
EXPORT_C int WINAPI sys_GetFileAttribute(const char* lpFileName);

/****************************************************
    *Function: 查找当前目录下第一个文件
    *Intput: lpFileName 可以是文件也可以是目录,也可以是通配符比如*.ini
    *Output: pFindData  返回查找到的文件结构体
    *Return: 成功返回一个文件句柄，失败返回INVALID_HANDLE
    *PS: 使用完该句柄后调用 sys_FindClose 关闭句柄
******************************************************/
EXPORT_C Handle WINAPI sys_FindFirstFile(const char* lpFileName, LPSYS_FIND_DATAA pFindData);

/****************************************************
    *Function: 查找当前目录下一个文件
    *Intput: hFindFile 文件句柄，通过 sys_FindFirstFile 函数创建
    *Output: pFindData  返回查找到的文件结构体
    *Return: 成功返回TRUE(1) 失败返回FALSE(0)
******************************************************/
EXPORT_C int WINAPI sys_FindNextFile(Handle hFindFile, LPSYS_FIND_DATAA pFindData);

/****************************************************
    *Function: 关闭文件查找句柄
    *Intput: hFindFile  通过sys_FindFirstFile 返回的有效句柄
    *Output: NULL
    *Return: 成功返回TRUE(1) 失败返回FALSE(0)
******************************************************/
EXPORT_C int WINAPI sys_FindClose(Handle hFindFile);

/****************************************************
    *Function: 创建一个 监控目录下所有文件变化 句柄
    *Intput: lpPathName 文件或目录
             bWatchSubtree  是否监视其子目录 TRUE(1)-监视  FALSE(0)-不监视
             dwNotifyOption 监视附加参数 参考 EM_FILE_NOTIFY_OPTION
    *Output: NULL
    *Return: 成功返回一个文件监控句柄，失败返回INVALID_HANDLE
    *PS: 返回的句柄，可用sys_WaitForSingleObject/sys_WaitForMultipleObjects等待其触发
******************************************************/
EXPORT_C Handle WINAPI sys_CreateFileChangeNotify(const char* lpPathName, int bWatchSubtree, uint dwNotifyOption);

/****************************************************
    *Function: 关闭 文件监控 句柄
    *Intput: hChangeHandle  监控句柄，通过函数sys_CreateFileChangeNotification 获得
    *Output: NULL
    *Return: 成功返回TRUE(1) 失败返回FALSE(0)
******************************************************/
EXPORT_C int WINAPI sys_CloseFileChangeNotify(Handle hChangeHandle);

enum EMParity
{//波特率
    EMP_NO = 0,     //无校验位
    EMP_ODD,        //奇校验
    EMP_EVEN,       //偶校验
    EMP_MARK,       //mark
    EMP_SPACE,      //space
};

enum EMByteSize
{//数据位
    EMD_BITS5 = 5,  //5位
    EMD_BITS6,      //6位
    EMD_BITS7,      //7位
    EMD_BITS8,      //8位
};

enum EMStopBits
{//停止位
    EMS_ONE = 0,    //1位
    EMS_ONE5,       //1.5位
    EMS_TWO,        //2位
};

struct ComParam
{//串口参数
    uint        comPort;    //串口index 1、2、3...
    uint        baudRate;   //波特率600、1200、...9600...115200
    EMParity    parity;     //校验位
    EMByteSize  btSize;     //数据位
    EMStopBits  stopBit;    //停止位
    uint        readSize;   //读缓冲区
    uint        writeSize;  //写缓冲区
    uint        bOverlapped;//是否异步 1 是 0不是
};

/****************************************************
    *Function: 打开一个串口
    *Intput: pComData 串口参数
    *Output: 无
    *Return: 成功返回串口句柄，失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_OpenCom(const ComParam* pComData);

/****************************************************
    *Function: 关闭一个串口
    *Intput: hCom      串口句柄
    *Output: 无
    *Return: 成功返回0, 失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_CloseCom(int hCom);

/****************************************************
    *Function: 读取串口数据
    *Intput: hCom      串口句柄
             pBuffer   读缓冲区
             nBufSize  缓冲区大小
             timeout  超时
    *Output: NULL
    *Return: 成功返回读取字节数，失败返回错误代码
******************************************************/
EXPORT_C int WINAPI sys_ReadCom(int hCom, char* pBuffer, int nBufSize, int timeout = 2000);

/****************************************************
    *Function: 往串口写入数据
    *Intput: hCom       串口句柄
             pBuffer    待写入数据
             nSize      数据大小
    *Output: NULL
    *Return: 成功返回写入字节数，失败返回错误代码
******************************************************/
EXPORT_C int WINAPI sys_WriteCom(int hCom, const char* pBuffer, int nBufLen);

/****************************************************
    *Function: 往串口写入数据，并读取返回值
    *Intput: hCom   句柄
             lpBuff 要读入数据缓冲区
             nWrite 要写入的数据大小
             nSize  缓冲区大小
             timeout  超时
    *Output: NULL
    *Return: 成功返回读取字节数，失败返回错误代码
******************************************************/
EXPORT_C int WINAPI sys_wReadCom(int hCom, char* pBuffer, int nBufLen, int nBufSize, int timeout = 2000);

//----------------------------------系统api定义结束----------------------------------//

/****************************************************
    *Function: 把一段utf8编码字符串转换为gbk编码
    *Intput: utf8 需要转换的utf8字符串
    *Output: gbk 转换后的gbk字符串
    datalen gbk字符串容量
    *Return: NULL
******************************************************/
EXPORT_C void WINAPI utf8_to_gbk(const char* utf8, char* gbk, int datalen);

/****************************************************
    *Function: 把一段gbk编码字符串转换为utf8编码
    *Intput: gbk 需要转换的gbk字符串
    *Output: utf8 转换后的utf8字符串
    datalen utf8字符串容量
    *Return: NULL
******************************************************/
EXPORT_C void WINAPI gbk_to_utf8(const char* gbk, char* utf8, int datalen);

/****************************************************
	*Function: 把一段gbk编码字符串转换为ascii编码
	*Intput: gbk 需要转换的gbk字符串
	*Output: ascii 转换后的ascii字符串
	datalen ascii 字符串容量
	*Return: NULL
******************************************************/
EXPORT_C void WINAPI gbk_to_ascii(const wchar_t* gbk, char* ascii, int datalen);

/****************************************************
	*Function: 把一段ascii编码字符串转换为gbk编码
	*Intput: ascii 需要转换的ascii字符串
	*Output: gbk 转换后的gbk字符串
	datalen gbk字符串容量
	*Return: NULL
******************************************************/
EXPORT_C void WINAPI ascii_to_gbk(const char* ascii, wchar_t* gbk, int datalen);

/****************************************************
    *Function: 查找某usb设备是否存在
    *Intput: keywork 查找关键字(硬件ID)
    *Output: NULL
    *Return: 成功返回1 失败返回0
******************************************************/
EXPORT_C int WINAPI search_usb_dev(const char* keywork);

/****************************************************
    *Function: 拷贝一个字符串到剪贴板
    *Intput: pbuff 字符串
    *Output: NULL
    *Return: 成功返回0 失败返回错误码
******************************************************/
EXPORT_C int WINAPI copy_to_board(const char* pbuff);


/****************************************************
    *Function: 映射地址空间,ssk用到
    *Intput: lpBaseAddress
    *Output: NULL
    *Return: 成功返回0 失败返回错误码
******************************************************/
EXPORT_C int WINAPI sys_UnmapViewOfFile(const void* lpBaseAddress);

/****************************************************
    *Function: 打开映射,ssk用到
    *Intput: lpBaseAddress
    *Output: NULL
    *Return: 成功返回0 失败返回错误码
******************************************************/
HANDLE WINAPI sys_OpenFileMapping(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
/****************************************************
    *Function: 创建文件映射,ssk用到
    *Intput: lpBaseAddress
    *Output: NULL
    *Return: 成功返回0 失败返回错误码
******************************************************/
HANDLE WINAPI sys_CreateFileMapping(HANDLE hFile, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName);
/****************************************************
    *Function: 映射文件试图,ssk用到
    *Intput: lpBaseAddress
    *Output: NULL
    *Return: 成功返回0 失败返回错误码
******************************************************/
void* WINAPI sys_MapViewOfFile(HANDLE hFileMappingObject, DWORD dwDesiredAccess);


/****************************************************
    *Function: 调试,debug时与编译器通讯调试 log用到
    *Intput: lpOutputString
    *Output: NULL
    *Return: 成功返回0 失败返回错误码
******************************************************/
void WINAPI sys_OutputDebugString(LPCSTR lpOutputString);

#endif
