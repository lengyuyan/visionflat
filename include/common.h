/*********************************************************************
    *文件说明: common库头文件，所有导出函数在这声明
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/22 CW0793 创建
                
***********************************************************************/
#ifndef DLL_INCLUDE_COMMON
#define DLL_INCLUDE_COMMON
#include "GlobalDefine.h"

#pragma once


#ifndef RETURN_NULLPTR  //此处判断空值，0值得宏，代码中可考虑使用，简化代码
#define RETURN_NULLPTR(p,v) if (NULL == p) \
{	\
    return v; \
} 
#define RETURN_NULL(p) if (NULL == p) \
{	\
    return; \
} 
#endif

////打印级别
//#define LEVEL_CRIT  4
//#define LEVEL_ERR   3
//#define LEVEL_WARN  2
//#define LEVEL_INFO  1
//
////此处定义打印log接口
//#define LOG_INFO(msg, ...) trace(LEVEL_INFO, __FUNCTION__, __LINE__, msg, __VA_ARGS__)
//#define LOG_WARN(msg, ...) trace(LEVEL_WARN,__FUNCTION__, __LINE__, msg, __VA_ARGS__)
//#define LOG_ERR(msg, ...)  trace(LEVEL_ERR,__FUNCTION__, __LINE__, msg, __VA_ARGS__)
//#define LOG_CRIT(msg, ...) trace(LEVEL_CRIT,__FUNCTION__, __LINE__, msg, __VA_ARGS__)

enum EMStrType
{//字符串类型
	EMSTR_STRING = 0,   //字符串
	EMSTR_INT,          //整型
	EMSTR_DOUBLE,       //浮点型
};

enum EM_LOG_LEVEL
{
	LEVEL_CRIT = -3,
	LEVEL_ERR,
	LEVEL_WARN,
	LEVEL_INFO1,
	LEVEL_INFO2,
	LEVEL_INFO3,
};

//此处定义打印log接口
#define LOG_INFO(msg, ...) trace(LEVEL_INFO1, __FILE__, __LINE__, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_INF1(msg, ...) trace(LEVEL_INFO1, __FILE__, __LINE__, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_INF2(msg, ...) trace(LEVEL_INFO2, __FILE__, __LINE__, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_INF3(msg, ...) trace(LEVEL_INFO3, __FILE__, __LINE__, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_WARN(msg, ...) trace(LEVEL_WARN, __FILE__, __LINE__, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_ERR(msg, ...)  trace(LEVEL_ERR, __FILE__, __LINE__, __FUNCTION__, msg, __VA_ARGS__)
#define LOG_CRIT(msg, ...) trace(LEVEL_CRIT, __FILE__, __LINE__, __FUNCTION__, msg, __VA_ARGS__)


//线程函数类型，若设置使用线程池，则会使用线程池函数运行
typedef int (WINAPI *THREAD_PROC)(void* param);




//以下都是关于发送消息的声明
#define DLL_NAME_LEN 31 //模块名最大长度

enum    {DES_ENCRYPT,DES_DECRYPT};

//消息时间类型
enum SYSTEM_MSG_TYPE
{
    EVENT_USUALLY = 0,  //通用消息
    EVENT_SYS_CLOSE,    //程序退出事件
    EVENT_OCTOPUS_OP,   //主流程之间的对话消息 专用
    EVENT_LOGIN_MSG,   //登陆消息
    EVENT_FLATFISH_RUN,   //主流程之间的对话消息 专用


    EVENT_FLATFISH_BUF,      //主流程之间的对话消息, 字符串消息
    EVENT_FLATFISH_STRUCT,   //主流程之间的对话消息, 结构体消息
};
//ui弹窗消息
enum MessageType
{//消息类型
	MT_NORMAL = 0,  //正常提示消息
	MT_SELECT,      //选择消息 -- 是/否
	MT_RETRY,       //重试消息 -- 重试 / 忽略 / 取消
	MT_WARN,        //警告消息
	MT_CRIT,        //致命错误，程序必须关闭
	MT_RESTART,     //要求系统重新启动消息
};
enum MessageResult
{//弹窗消息返回结果
	MR_OK = 0,  //ok
	MR_CANCEL,  //取消
	MR_ABORT,   //终止
	MR_RETRY,   //重试
	MR_IGNORE,  //忽略
	MR_YES,     //选择是
	MR_NO,      //选择否
	MR_ABNORMAL,  //异常
};

typedef MessageResult (WINAPI *CommMessageShow)(const char*, MessageType, void*);
typedef int (WINAPI *PushDeviceInfoCallback)(char *, char *, void*);//不需要

//回调函数原型，注册消息接口时按此原型声明函数
//typedef int (WINAPI *RegistCallBackProc)(unsigned int, void *, SYSTEM_MSG_TYPE msgType);

//回调函数原型，注册消息接口时按此原型声明函数
//typedef int (WINAPI *RegistCallBackProc)(unsigned int, unsigned int, void* lparam);

//回调函数原型，注册消息接口时按此原型声明函数:改进的地方:输入为消息类型，消息参数(可以为任意类型),第三个参数(c++一般为this,c可以理解为另外一个附加参数全局的或者静态的)
//第4个为参数类型
typedef int (WINAPI *RegistCallBackProc)(unsigned int, void *lparam, void* otherparam);

//错误代码
enum EM_ERR_COMMON
{
    ERR_TOOLARGE = -7,  //数据太大
    ERR_TIMEOUT2,        //等待超时
    ERR_NOMSGLIST,      //没有可用的消息缓冲区
    ERR_NOCONNECT,      //客户端未连接
    ERR_NOREGIST,       //对方未注册接收消息接口
    ERR_NOINIT,         //未初始化
    ERR_PARAM,          //参数错误
    COMMON_NORMAL,      //正常
};

enum USER_DEFINE_MESSAGE
{
    RESET_SYS = 0,
    CLOSE_SYS,

	CALIB_ROTTOOL = 5, //标定旋转机械手
    CALIB_CAMERA = 10,  //标定相机
    WORKING_CAMERA,
    RECV_REMOTE_MSG,        //收到远程消息
    RECV_UI_LOGIN_MSG,        //收到远程消息
    MSG_LOGIN_SYSTEM,
    MSG_SCENE_BUSY,          //当前场景正在工作中
	MSG_SEND_IMAGE,			//发送图片
	MSG_SEND_IMAGE_PATH,			//发送图片路径

    MSG_SUCCESSFUL               = 20,      //运行正常提示
    MSG_CAMERA_INIT_FAILED,                //相机初始化失败
    MSG_LIGHT_INIT_FAILED,                 //光源初始化失败
    MSG_HALCON_ERROR,                      //halcon处理异常
    MSG_IMAGEDEAL_INIT_FAILED,             //初始化图像处理失败
    MSG_CONNECT_FAILED,              //初始化通信连接失败
    MSG_CONNECT_SUCCESS,             //通信连接成功
    MSG_INITCONFIG_INIT_FAILED,           //初始化配置失败
    MSG_DISP_WORKING_INFO,
    MSG_CONNECT_CAMERA_FAILED,
    MSG_CONNECT_CAMERA_SUCCESS,
	MSG_DISP_CLIB_INFO,            //

    MSG_DISP_IMAGE               = 100,
    MSG_DISP_IMAGE_DUMP,

    MSG_PROCESS_ERROR            = 200,       

    MSG_CLOSE_SYSTEM             = 1000,
    MSG_RESET_SYSTEM,

    MSG_GRABIMAGE_INDEX_ERROR     = 1100,
    MSG_GRABIMAGE_ERROR,
    MSG_GRABIMAGE_LIGHTON_ERROR,
    MSG_GRABIMAGE_LIGHTOFF_ERROR,
    MSG_GRABIMAGE_LIGHTON_SUCCESS,
    MSG_GRABIMAGE_LIGHTOFF_SUCCESS,

    MSG_RESET_PARAMETER           =2000,

    MSG_LANDTYPE_INFORMATION      = 2200,
  

    MSG_GET_MEMORY_INFO      = 3000,


	MSG_COM_COMMUNITE = 4000,

	MSG_DIS_MAINWND_NAME = 5000,

    
};
enum COMMAND_TYPE
{
};

//此处定义消息机制，各模块的模块名称，
//也可以自己使用自己的名称，只要与通信方协商好即可
#define MODULE_SOCKETSERVER     "socketserver"  //socket server模块
#define MODULE_ROBOT            "robot"         //robot
#define MODULE_OPS              "ops"           //主流程
#define MODULE_MOTOR            "motor"         //主运动
#define MODULE_UI               "ui"            //ui界面 
#define MODULE_OP               "op"            //业务处理模块
#define MODULE_CONFIG           "config"        //配置模块

#define MODULE_SCENE_MAIN_UI     "scene_main_ui"    //场景主窗口
#define MODULE_SCENE_DEBUG_UI     "scene_debug_ui"   //场景调试UI
#define MODULE_SCENE_MAIN_SDK     "scene_main_sdk"   //场景调试UI

#define MODULE_ALL              "all"           //所有模块


#define MODULE_SYSTEMCONFIG_UI   "systemconfig"  //场景参数设置界面

/****************************************************
    *Function: 初始化common资源
    *Intput: NULL
    *Output: NULL
    *Return: 成功返回true ，失败返回false
******************************************************/
EXPORT_C bool WINAPI init_common();

/****************************************************
    *Function: 回收common资源
    *Intput: NULL
    *Output: NULL
    *Return: 无
******************************************************/
EXPORT_C void WINAPI uninit_common();

//一些通用函数，在这里声明，定义在global.cpp


/****************************************************
    *Function: 判断一个文件是否存在
    *Intput: filepath 需要检测的文件全路径
    *Output: NULL
    *Return: 存在返回true ，不存在返回false
******************************************************/
EXPORT_C bool WINAPI is_file_exist(LPCSTR filepath);

/****************************************************
    *Function: 判断一个文件是否只读
    *Intput: filepath 需要检测的文件全路径
    *Output: NULL
    *Return: 只读返回true ，否则返回false
******************************************************/
EXPORT_C bool WINAPI is_file_read(LPCSTR filepath);

/****************************************************
    *Function: 获取系统时间
    *Intput: 无
    *Output: NULL
    *Return: 返回当前时间格式字符串，返回的字符串指针
        是全局指针，所以，连续获取两次，则第一次获取的
        时间也会变成第二次的时间，调用者须自己保存
******************************************************/
EXPORT_C const char* WINAPI get_time_str();



EXPORT_C void WINAPI create_image_file();

/****************************************************
*Function: 注册消息接收函数,不注册无法接收到消息
*Intput: dllName 调用模块名字,随便定义,只要发消息方知道
        fun 接收消息函数地址,调用者必须按RegistCallBackProc
            原型定义一个函数，来接收消息
*Output: 无
*Return: 注册成功返回true，否则返回false
******************************************************/
//EXPORT_C bool WINAPI register_notify(LPCSTR dllName, RegistCallBackProc fun);

/****************************************************
*Function: 反注册消息接收函数,反注册后对方发送消息会失败
*Intput: dllName 调用模块名字,必须和注册时传的名字一样
*Output: 无
*Return: 注册成功返回true，否则返回false
******************************************************/
//EXPORT_C void WINAPI unregister_notify(LPCSTR dllName);

/****************************************************
*Function: 发送消息函数，对方必须注册才能发送成功
*Intput: wparam 消息
         lparam 消息附加参数
         dllName 消息接收方模块名字，注册了才有 默认广播
         msgType 消息类型 默认为通用消息
*Output: 无
*Return: 发送成功返回0，否则返回错误码
******************************************************/
//EXPORT_C int WINAPI msg_send(unsigned int wparam, void *lparam,
//        LPCSTR dllName = 0, SYSTEM_MSG_TYPE msgType = EVENT_USUALLY);

//EXPORT_C int WINAPI msg_send(unsigned int wparam, unsigned int lparam, const char* name);
/****************************************************
    *Function: 初始化post消息机制，必须调用uninit_msg
                回收资源，关闭线程
    *Intput: NULL
    *Output: NULL
    *Return: 成功返回true ，失败返回false
******************************************************/
//EXPORT_C bool WINAPI init_msg();

/****************************************************
    *Function: 关闭post线程，回收资源
    *Intput: NULL
    *Output: NULL
    *Return: 无
******************************************************/
//EXPORT_C void WINAPI uninit_msg();

/****************************************************
*Function: 发送消息函数，只发送给common，由common负责发送给对方
*Intput: wparam 消息
        lparam 消息附加参数(指针类型)
        lsize  指针内存大小单位， 字节, 如果是字符串则包括结束符的长度
        dllName 消息接收方模块名字，注册了才有 默认广播
        msgType 消息类型 默认为通用消息
*Output: 无
*Return: 正常返回0，参数错误返回错误码
******************************************************/
//EXPORT_C int WINAPI msg_post(unsigned int wparam,void *lparam, unsigned int lsize,
//    LPCSTR dllName = 0, SYSTEM_MSG_TYPE msgType = EVENT_USUALLY);

EXPORT_C int WINAPI msg_post(unsigned int wparam, void* lparam, unsigned int lsize,const char* name);

/****************************************************
*Function: 打印log函数,静态调用时，可直接用宏调用
*Intput: LogLevel打印级别，分为info warn error crit
        msgFormat 字符串格式，与printf用法相同
        后面可加N个参数，最大支持1024个字节
*Output: 无
*Return: 无
******************************************************/
EXPORT_C void WINAPI trace(int LogLevel, const char* file, int line, const char* fun, const char *pszFormat, ...);

typedef void (WINAPI *RECALL_LOG)(LPCSTR, LPVOID);

//type  0000 每位标示一个, 1为真 0为假
//     crit/error/warn/info
//打印log时,判断有没有注册过的接口,如果有,根据type调用
//假设type=0b1111,则所有日志全部传给它
bool WINAPI regist_recall_status(int type, RECALL_LOG rcLog, LPVOID lparam);




//--------------------------------此处定义进程间的通信--------------------------------------//
typedef int (WINAPI *RegistRecvProc)(char*, UINT);
#define INVALID_SSK     ((DWORD)-1)      //无效句柄

typedef HANDLE SSK_ST;  //ssk句柄
/****************************************************
*Function: 初始化ssk模块,可以不调用，创建ssk时会
            自动调用
*Intput: 无
*Output: 无
*Return: 成功返回true, 失败返回false
******************************************************/
EXPORT_C bool WINAPI init_ssk();

/****************************************************
*Function: 回收ssk模块资源，若不再使用ssk服务，必须调
*Intput: 无
*Output: 无
*Return: 无
******************************************************/
EXPORT_C void WINAPI uninit_ssk();

/****************************************************
*Function: 创建一个ssk服务端，使用ssk进程间通信必须
        先调用此接口创建服务，连接上才能通信
*Intput: sskName 需要创建的ssk服务名，connect时用到
*Output: 无
*Return: 成功返回ssk句柄, 失败返回INVALID_SSK
******************************************************/
EXPORT_C DWORD WINAPI ssk_server(LPCSTR sskName);

/****************************************************
*Function: 连接一个ssk服务，通过服务名连接，必须先
        创建成功，才能连接
*Intput: sskName 需要连接的ssk服务名
*Output: 无
*Return: 成功返回ssk句柄, 失败返回INVALID_SSK
******************************************************/
EXPORT_C DWORD WINAPI ssk_conn(LPCSTR sskName);

/****************************************************
*Function: 查询一个ssk服务连接状态
*Intput: ssk 需要查询的ssk句柄
*Output: 无
*Return: 返回当前状态
        COMMON_NORMAL   正常
        ERR_PARAM       ssk无效
        ERR_NOINIT      未初始化 包括ssk服务、服务端
        ERR_NOCONNECT   客户端未连接
******************************************************/
EXPORT_C int WINAPI ssk_status(DWORD ssk);

/****************************************************
*Function: 查询一个ssk句柄是server还是client
*Intput: ssk ssk句柄，conn init都会返回的句柄
*Output: 无
*Return: ERR_PARAM       ssk无效
        ERR_NOINIT      未初始化 包括ssk服务、服务端
        ERR_NOCONNECT   客户端未连接
        0 标示是server
        1 标示是client
******************************************************/
EXPORT_C int WINAPI ssk_is_server(DWORD ssk);

/****************************************************
*Function: 关闭一个ssk服务，使用ssk进程间通信完毕后必须
        调用此接口关闭服务，释放资源
*Intput: ssk ssk句柄，conn init都会返回的句柄
*Output: 无
*Return: 无
******************************************************/
EXPORT_C void WINAPI ssk_close(DWORD ssk);

/****************************************************
*Function: 注册一个ssk通信消息回调函数，当收到属于
    自己ssk句柄的消息时，会自动通知注册方
*Intput: ssk ssk句柄，conn init都会返回的句柄
         proc 回调函数指针，必须按照格式写
*Output: 无
*Return: 成功返回true, 失败返回false
******************************************************/
EXPORT_C bool WINAPI regist_ssk_recv(DWORD ssk, RegistRecvProc proc);

/****************************************************
*Function: 从ssk接收消息
*Intput: ssk ssk句柄，conn init都会返回的句柄
         pdata 数据待写入的缓冲区
         len 缓冲区大小
         timeout 等待时间单位ms INFINITE 无限等待
*Output: 无
*Return: 成功返回接收到的数据长度, 失败返回错误码
******************************************************/
EXPORT_C int WINAPI ssk_recv(DWORD ssk, char* pdata, int len, DWORD timeout = INFINITE);

/****************************************************
*Function: 从ssk发送消息
*Intput: ssk ssk句柄，conn init都会返回的句柄
        pdata 数据待写入的缓冲区
        len 缓冲区大小
*Output: 无
*Return: 成功返回0, 失败返回错误码
******************************************************/
EXPORT_C int WINAPI ssk_send(DWORD ssk, char* pdata, int len);

/****************************************************
*Function: 清空ssk接收消息队列
*Intput: ssk ssk句柄，conn init都会返回的句柄
*Output: 无
*Return: 成功返回0, 失败返回错误码
******************************************************/
EXPORT_C int WINAPI ssk_clear_msg(DWORD ssk);

EXPORT_C bool WINAPI Execution_3DES(char *Out, char *In, long datalen,const char *Key, int keylen, bool Type);

/****************************************************
    *Function: 创建一个文件夹，若存在，则不创建
    *Intput: path 文件夹全路径
    *Output: NULL
    *Return: 成功返回0, 失败返回错误码
******************************************************/
EXPORT_C int WINAPI create_path(const char* path);


/****************************************************
    *Function: 检查一个IP地址是否合法
    *Intput: lpIP ip地址字符串
    *Output: NULL
    *Return: ip地址合法返回1 不合法返回0
******************************************************/
EXPORT_C int WINAPI chk_ipaddress(const char* lpIP);

/****************************************************
    *Function: 使用comm线程池执行一个函数,会使用线程池
    comm析构时，会自动关闭所有线程,所以如果要线程安全退出
    则最好在comm析构前保证线程退出
    *Intput: fun 函数地址
             lparam 任意型指针
    *Output: NULL
    *Return: 成功返回0，失败返回错误码
    *PS: 线程池只用来运行重复效率高，时间短的工作
    不能等待，不能关闭，请慎用
******************************************************/
EXPORT_C int WINAPI pool_thread(THREAD_PROC fun, void* lparam);


/****************************************************
    *Function: 拷贝目标字符串，直到结束或者找到指定字符
    *Intput: src 目标字符串
             dest待拷贝字符串
             ch  指定字符
    *Output: NULL
    *Return: 返回结束的指针地址(src+strlen(dest))，指向0结束符
******************************************************/
EXPORT_C char* WINAPI comm_strcpy(char* src, const char* dest, char ch = 0);

/****************************************************
    *Function: 判断一个字符串是否为数字
    *Intput: pstr 待检测字符串
    *Output: NULL
    *Return: double返回2 整数返回1, 其它返回0 参考EMStrType
******************************************************/
EXPORT_C int WINAPI get_str_type(const char* pstr);
/****************************************************
    *Function: 删除一个字符串前后两头的特殊字符(逐字符比较)
    *Intput:    pbuff   待处理字符串
                key     特殊字符数组
    *Output: NULL
    *Return: NULL
******************************************************/
EXPORT_C void WINAPI trim_char(char* pbuff, const char* key = " ");

/****************************************************
    *Function: 获取最后一次设置的错误代码
    *Intput: NULL
    *Output: NULL
    *Return: 返回最后发生的错误代码
******************************************************/
EXPORT_C int WINAPI get_last_err();


//加入消息
//消息类接口
EXPORT_C int WINAPI register_notify(const char* name, RegistCallBackProc fun, void* lparam);
EXPORT_C int WINAPI unregister_notify(const char* name, RegistCallBackProc fun);

//EXPORT_C int WINAPI msg_recv_str(char* lpstr, int timeout, const char* name);

EXPORT_C int WINAPI msg_send(unsigned int wparam,  void* lparam, unsigned int lsize, const char* name);
EXPORT_C int WINAPI msg_post(unsigned int wparam, void* lparam, unsigned int lsize,const char* name);


#endif  //防止重复包含头文件