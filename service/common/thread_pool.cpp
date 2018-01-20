#include "sysapi.h"
#include "common.h"
#include "thread_pool.h"

#include "GolobalError.h"

CThreadPool* CThreadPool::m_sInstance = NULL;
CThreadPool::CThreadPool(void)
{
    m_bRun = true;
    m_bAllExit = true;
    m_iNoUsePool = 1;//get_cfg_int(COMM_CFG_KEY, NUSE_POOL);
    m_hSection = sys_InitializeCriticalSection();
}

CThreadPool::~CThreadPool(void)
{
    int tm = clock();
    vector<ST_THREAD_FUN*>::iterator it;

    m_bRun = false; //标识退出信号
    sys_EnterCriticalSection(m_hSection);
    for(it = m_vecThread.begin(); it != m_vecThread.end(); it++)
    {//触发信号
        if (INVALID_HANDLE != (*it)->startEvt)
        {//线程池才会用事件
            sys_SetEvent((*it)->startEvt);
        }
    }
    sys_LeaveCriticalSection(m_hSection);

    while(!m_bAllExit)
    {//等待退出
        sys_Sleep(10);
        if (clock() - tm > 5000)
        {
            tm = clock();
            LOG_WARN("超过5s 有线程未退出或者skt句柄未关闭，请确认。");
        }
    }

    if (!m_bAllExit)
    {
        for(it = m_vecThread.begin(); it != m_vecThread.end(); it++)
        {//触发信号
            sys_TerminateThread((*it)->thread);
            sys_CloseHandle((*it)->thread);
            if (INVALID_HANDLE != (*it)->hIdleEvt)
            {//线程池才会用事件
                sys_CloseHandle((*it)->hIdleEvt);
            }
            if (INVALID_HANDLE != (*it)->startEvt)
            {//线程池才会用事件
                sys_CloseHandle((*it)->startEvt);
            }
            delete *it;
        }
    }

    LOG_INF2("all thread pool exit.");
    sys_DeleteCriticalSection(m_hSection);
}

//获取实例
CThreadPool* CThreadPool::GetInstance()
{
    if(NULL == m_sInstance)
    {
        m_sInstance = new CThreadPool();
    }
    return m_sInstance;
}

//删除实例
void CThreadPool::ReleaseInstance()
{
    FREE_ANY(m_sInstance);
}

//线程池线程执行函数
int WINAPI CThreadPool::ThreadFun(void* param)
{
    int ret = 0;
    ST_THREAD_FUN* pdata = (ST_THREAD_FUN*)param;
    CThreadPool* pMain = (CThreadPool*)(pdata->self);
    ret = pMain->Process(pdata);
    return ret;
}

int CThreadPool::Run(PROCEX fun, void* param)
{
    int nCnt = 0;
    vector<Handle> vecThread;
    vector<ST_THREAD_FUN*>::iterator it;

    RETURN_CHK(fun, ERR_INPUT_PARAM);

    sys_EnterCriticalSection(m_hSection);
    nCnt = (int)m_vecThread.size();
    LOG_INF2("CThreadPool::Run(0x%x,0x%x), current total thread=%d.", fun, param, nCnt);
    if (m_iNoUsePool || 0 == nCnt)
    {//不使用线程池
        RunAsNewThread(fun, param);
        sys_LeaveCriticalSection(m_hSection);
        return 0;
    }

    for(it = m_vecThread.begin(); it != m_vecThread.end(); it++)
    {
        vecThread.push_back((*it)->hIdleEvt);
    }
    int res = sys_WaitForMultipleObjects(nCnt, &vecThread[0], FALSE, 0);
    if (res >= 0 && res < nCnt)
    {//成功，则返回就绪事件索引
        m_vecThread[res]->fun = fun;
        m_vecThread[res]->param = param;
        sys_ResetEvent(m_vecThread[res]->hIdleEvt); //重置信号
        sys_SetEvent(m_vecThread[res]->startEvt);   //置开始信号
    }
    else if (ERR_TIMEOUT == res)
    {//未找到就绪线程
        RunAsNewThread(fun, param);
    }
    sys_LeaveCriticalSection(m_hSection);
    return 0;
}

void CThreadPool::DelThreadData(ST_THREAD_FUN* pThread)
{
    int nCnt = 0;
    vector<ST_THREAD_FUN*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    for(it = m_vecThread.begin(); it != m_vecThread.end(); it++)
    {
        if ((*it) == pThread)
        {//找到
            LOG_INF2("CThreadPool::DelThreadData 0x%x,0x%x,0x%x", (*it)->thread, (*it)->startEvt, (*it));
            sys_CloseHandle((*it)->thread);
            if (INVALID_HANDLE != (*it)->startEvt)
            {
                sys_CloseHandle((*it)->startEvt);
            }
            if (INVALID_HANDLE != (*it)->hIdleEvt)
            {
                sys_CloseHandle((*it)->hIdleEvt);
            }
            delete *it;
            m_vecThread.erase(it);
            break;
        }
    }
    nCnt = (int)m_vecThread.size();
    sys_LeaveCriticalSection(m_hSection);

    if (0 == nCnt)
    {//所有线程退出
        m_bAllExit = true;
    }
}

//外部加锁
void CThreadPool::RunAsNewThread(PROCEX fun, void* param)
{
    ST_THREAD_FUN* pThread = new ST_THREAD_FUN;

    m_bAllExit = false;
    pThread->fun = fun;
    pThread->param = param;
    pThread->self = this;
    pThread->startEvt = m_iNoUsePool ? INVALID_HANDLE : sys_CreateEvent(FALSE, TRUE, NULL);
    pThread->hIdleEvt = m_iNoUsePool ? INVALID_HANDLE : sys_CreateEvent(TRUE, FALSE, NULL);
    pThread->thread = sys_CreateThread(ThreadFun, pThread);
    m_vecThread.push_back(pThread);
}

int CThreadPool::Process(ST_THREAD_FUN* pData)
{
    int res = 0;

    LOG_INF2("CThreadPool::Process..handle=0x%x", pData->thread);
    while(m_bRun)
    {//是否正在运行
        if (INVALID_HANDLE != pData->startEvt)
        {//线程池
            res = sys_WaitForSingleObject(pData->startEvt, 60 * 1000 * 2);    //等待2分钟
            if (res != RETURN_OK || !m_bRun)
            {//超时或者退出程序时, 线程退出
                break;
            }
        }

        if (pData->fun)
        {
            LOG_INF2("CThreadPool::Process(0x%x,0x%x) run..", pData->fun, pData->param);
            pData->fun(pData->param); //执行函数
            LOG_INF2("CThreadPool::Process thread:0x%x fun:0x%x run finished.", pData->thread, pData->fun);
        }

        if (INVALID_HANDLE == pData->hIdleEvt)
        {//不使用线程池
            break;
        }
        sys_SetEvent(pData->hIdleEvt);  //就绪状态
    }

    DelThreadData(pData); //从数组删除
    return 0;
}