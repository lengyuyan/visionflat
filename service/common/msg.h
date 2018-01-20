
#pragma once
#ifndef DLL_INCLUDE_COMM_MSG
#define DLL_INCLUDE_COMM_MSG
#include "sysapi.h"
#include <hash_map>
#include <list>
#include <string>
#include "common.h"

using std::list;
using std::hash_map;
using std::string;

#define MSG_INSTANCE CMsg::GetInstance()

struct TReCallFun
{//回掉函数
    RegistCallBackProc callback;
    void* lparam;
};

struct TMsg
{//消息体
    unsigned int wparam;
	void* lparam;				//新增通用的void消息,通过传递的消息大小来申请内存
	unsigned int size;			//消息大小
};

struct TMsgInfo
{//消息队列
    list<TMsg*> listMsg;
    Handle      msgEvt;
};

struct TPostData
{//post消息体
    TReCallFun pFun;
    TMsg msg;
};

#define MAX_STR_CNT 256

class CMsg
{
public:
    CMsg(void);
    virtual ~CMsg(void);

    static CMsg* GetInstance();
    static void ReleaseInstance();

    int RegisterNotify(const char* name, RegistCallBackProc fun, void* lparam);
	int UnRegisterNotify(const char* name); //gpz


    //int RecvMsgStr(char* lpstr, int timeout, const char* name);//先不实现
 


    int SendMsg(unsigned int wparam, void* lparam, unsigned int lsize, const char* name);
    int PostMsg(unsigned int wparam,  void* lparam, unsigned int lsize, const char* name);
    
	int RegistMessage(CommMessageShow fun, void* param);
	int RegistIAStudioMessage(CommMessageShow fun, void* param);
    int ShowMessage(MessageType type, const char *msg);

	int RegistDevice(PushDeviceInfoCallback callback, void* param);
	int PushDeviceInfo(char *type, char *context);
protected:

    TReCallFun* FindRecall(const char* name);
    TMsgInfo*   GetMsgInfo(const char* name);

    void PushMsg(const char* name, TMsg* pMsg);

    void ReleaseReCall();
    void ReleaseMsg();

    static int WINAPI MsgPostProc(void* lparam); //post消息

private:
    static CMsg*            m_sInstance;

    bool                    m_bRun;             //post线程是否在运行

    hash_map<string, TReCallFun*>   m_hashInfo; //所有对象
    hash_map<string, TMsgInfo*>     m_hashMsg;  //所有消息

    Handle                  m_hQuitSign;        //post线程退出信号
    Handle                  m_hSection;         //临界区句柄

    CommMessageShow         m_msgFun;           //弹窗消息 只接受注册一次
    void*                   m_msgParam;

	CommMessageShow         m_msgISFun;         //弹窗消息 IAStudio注册，  与m_msgFun，同一时刻只能调用一个， IAStudio优先
	void*                   m_msgISParam;

	PushDeviceInfoCallback  m_pushCallback;     //中控或看板界面 注册回调
	void*                   m_pushParam;
};

#endif  //防止重复包含