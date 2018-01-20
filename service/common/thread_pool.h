#pragma once
#include <vector>
#include "sysapi.h"

using std::vector;

//线程相关结构体
struct ST_THREAD_FUN
{
    PROCEX  fun;        //执行函数
    void*   param;      //执行参数
    Handle  thread;     //线程句柄
    Handle  startEvt;   //触发执行
    Handle  hIdleEvt;   //就绪句柄
    void*   self;       //自身类指针
};

#define POOL_INSTANCE CThreadPool::GetInstance()

class CThreadPool
{
public:
    CThreadPool(void);
    virtual ~CThreadPool(void);

    static CThreadPool* GetInstance();
    static void ReleaseInstance();

    int Run(PROCEX fun, void* param);       //线程执行

protected:
    
    void RunAsNewThread(PROCEX fun, void* param);   //创建新线程执行
    void DelThreadData(ST_THREAD_FUN* pThread);     //删除一个线程

    int Process(ST_THREAD_FUN* pData);  //执行函数
    static int WINAPI ThreadFun(void* param);

private:
    static CThreadPool*     m_sInstance;

    bool                    m_bRun;             //是否运行
    bool                    m_bAllExit;         //是否所有线程退出
    int                     m_iNoUsePool;       //是否不使用线程池，1不使用 0使用

    vector<ST_THREAD_FUN*>  m_vecThread;        //线程数据结构数组
    Handle                  m_hSection;         //临界区
};

