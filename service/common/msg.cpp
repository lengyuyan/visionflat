/*********************************************************************
    *文件说明: 提供同进程模块之间的通讯功能
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/26 CW0793 创建
                
***********************************************************************/
#include "GolobalError.h"
#include "msg.h"


using std::pair;

//CWorkMain 类
CMsg* CMsg::m_sInstance = 0;
CMsg::CMsg(void)
{
    m_bRun = true;
    m_msgFun = NULL;
    m_msgParam = NULL;
	m_msgISFun = NULL;
	m_msgISParam = NULL;
	m_pushCallback = NULL;
	m_pushParam = NULL;
    m_hQuitSign = sys_CreateEvent(TRUE, TRUE, NULL);
    m_hSection = sys_InitializeCriticalSection();
}

CMsg::~CMsg(void)
{
    m_bRun = false;
    sys_WaitForSingleObject(m_hQuitSign);
    //sys_EnterCriticalSection(m_hSection);
    ReleaseMsg();
    ReleaseReCall();
   // sys_LeaveCriticalSection(m_hSection);
    sys_DeleteCriticalSection(m_hSection);
    sys_CloseHandle(m_hQuitSign);
}

//获取实例
CMsg* CMsg::GetInstance()
{
    if(NULL == m_sInstance)
    {
        m_sInstance = new CMsg();
    }
    return m_sInstance;
}

//删除实例
void CMsg::ReleaseInstance()
{
    LOG_INF2("CMsg::ReleaseInstance()");
    FREE_ANY(m_sInstance);
}

TReCallFun* CMsg::FindRecall(const char* name)
{
    TReCallFun* pFun = NULL;
    hash_map<string, TReCallFun*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hashInfo.find(name);
    if (it != m_hashInfo.end())
    {
        pFun = it->second;
    }
    sys_LeaveCriticalSection(m_hSection);
    return pFun;
}

//注册消息接收函数
int CMsg::RegisterNotify(const char* name, RegistCallBackProc fun, void* lparam)
{
    TReCallFun* pFun = NULL;

    RETURN_CHK(name && fun && *name, ERR_INPUT_PARAM);
    RETURN_CHK(strlen(name) < MAX_NAME_LEN, ERR_INPUT_PARAM);
    RETURN_CHK(!FindRecall(name), ERR_REPEAT_REG);

    sys_EnterCriticalSection(m_hSection);
    pFun = new TReCallFun;
    pFun->callback = fun;
    pFun->lparam = lparam;
    m_hashInfo.insert(pair<string, TReCallFun*>(name, pFun));
    sys_LeaveCriticalSection(m_hSection);
    return RETURN_OK;
}

//取消注册消息接收函数
int CMsg::UnRegisterNotify(const char* name)
{
    hash_map<string, TReCallFun*>::iterator it;

	RETURN_CHK(name && *name, ERR_INPUT_PARAM);

	sys_EnterCriticalSection(m_hSection);
    it = m_hashInfo.find(name);
    if (it != m_hashInfo.end())
    {
        delete it->second;
        m_hashInfo.erase(it);
    }
	sys_LeaveCriticalSection(m_hSection);
	return RETURN_OK;
}

void CMsg::ReleaseReCall()
{
    hash_map<string, TReCallFun*>::iterator it;
    sys_EnterCriticalSection(m_hSection);
    for(it = m_hashInfo.begin(); it != m_hashInfo.end(); it++)
    {
        delete it->second;
    }
    m_hashInfo.clear();
    sys_LeaveCriticalSection(m_hSection);
}

void CMsg::ReleaseMsg()
{
    TMsg* pmsg = NULL;
    TMsgInfo* pmsgInfo = NULL;
    hash_map<string, TMsgInfo*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    for(it = m_hashMsg.begin(); it != m_hashMsg.end(); it++)
    {
        pmsgInfo = it->second;
        while(pmsgInfo->listMsg.size())
        {
            pmsg = pmsgInfo->listMsg.front();
            pmsgInfo->listMsg.pop_front();
            FREE_ANY(pmsg->lparam);
            delete pmsg;
        }
        sys_CloseHandle(pmsgInfo->msgEvt);
        delete pmsgInfo;
    }
    m_hashMsg.clear();
    sys_LeaveCriticalSection(m_hSection);
}

TMsgInfo* CMsg::GetMsgInfo(const char* name)
{
    TMsgInfo* pmsgInfo = NULL;
    hash_map<string, TMsgInfo*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hashMsg.find(name);
    if (it == m_hashMsg.end())
    {
        pmsgInfo = new TMsgInfo;
        pmsgInfo->msgEvt = sys_CreateEvent(TRUE, FALSE, NULL);
        m_hashMsg.insert(pair<string, TMsgInfo*>(name, pmsgInfo));
    }
    else
    {
        pmsgInfo = it->second;
    }
    sys_LeaveCriticalSection(m_hSection);
    return pmsgInfo;
}

void CMsg::PushMsg(const char* name, TMsg* pMsg)
{
    TMsgInfo* pmsgInfo = NULL;

    sys_EnterCriticalSection(m_hSection);
    pmsgInfo = GetMsgInfo(name);
    pmsgInfo->listMsg.push_back(pMsg);
    if  (1 == pmsgInfo->listMsg.size())
    {
        sys_SetEvent(pmsgInfo->msgEvt);
    }
    sys_LeaveCriticalSection(m_hSection);
}



//int CMsg::RecvMsgStr(char* lpstr, int timeout, const char* name)
//{
//    int ret = 0;
//    TMsg* pmsg = NULL;
//    TMsgInfo* pmsgInfo = NULL;
//
//    RETURN_CHK(lpstr && name && *name, ERR_INPUT_PARAM);
//
//    pmsgInfo = GetMsgInfo(name);
//
//    ret = sys_WaitForSingleObject(pmsgInfo->msgEvt, timeout);
//
//    RETURN_CHK(!ret, ret);
//
//    sys_EnterCriticalSection(m_hSection);
//    pmsg = pmsgInfo->listMsg.front();
//    pmsgInfo->listMsg.pop_front();
//    if (0 == pmsgInfo->listMsg.size())
//    {
//        sys_ResetEvent(pmsgInfo->msgEvt);
//    }
//    sys_LeaveCriticalSection(m_hSection);
//
//    if (pmsg->lparam)
//    {
//        comm_strcpy(lpstr, pmsg->lparam);
//        delete pmsg->lparam;
//    }
//    ret = pmsg->wparam;
//    delete pmsg;
//    return ret;
//}

int CMsg::SendMsg(unsigned int wparam, void* lparam, unsigned int lsize, const char* name)
{
    TMsg* pMsg = NULL;
    TReCallFun* pFun = NULL;

    RETURN_CHK(name && *name, ERR_INPUT_PARAM);

    pFun = FindRecall(name);
    if (pFun)
    {
        pFun->callback(wparam, lparam, pFun->lparam);
        return 0;
    }

    pMsg = new TMsg;
    pMsg->wparam = wparam;
    pMsg->lparam = new char[lsize];

    //memcpy_s(pMsg->lparam, lsize, lparam);
	memcpy_s(pMsg->lparam,lsize,lparam,lsize);
    PushMsg(name, pMsg);
    return 0;
}

int WINAPI CMsg::MsgPostProc(void* lparam)
{
    TPostData* pMsg = (TPostData*)lparam;
    pMsg->pFun.callback(pMsg->msg.wparam, pMsg->msg.lparam, pMsg->pFun.lparam);
    FREE_ANY(pMsg->msg.lparam);
    FREE_ANY(pMsg);
    return 0;
}

int CMsg::PostMsg(unsigned int wparam,  void* lparam, unsigned int lsize, const char* name)
{
    TMsg* pMsg = NULL;
    TReCallFun* pFun = NULL;
    TPostData* postData = NULL;
    RETURN_CHK(name && *name, ERR_INPUT_PARAM);

    pFun = FindRecall(name);
    if (pFun)
    {
        postData = new TPostData;
        postData->msg.wparam = wparam;
        postData->msg.size = lsize;
		postData->msg.lparam = NULL;
		if (NULL != lparam)
		{
			postData->msg.lparam = new char[lsize];
			memcpy_s(postData->msg.lparam,lsize,lparam, lsize);
		}
		else
		{
			postData->msg.lparam = NULL;
		}
        postData->pFun.callback = pFun->callback;
        postData->pFun.lparam = pFun->lparam;
        //pFun->callback(wparam, lparam, pFun->lparam);
        pool_thread(MsgPostProc, postData);
        return 0;
    }

    pMsg = new TMsg;
    pMsg->wparam = wparam;
    pMsg->lparam = new char[lsize];
    memcpy_s(pMsg->lparam, lsize, lparam,lsize);
    PushMsg(name, pMsg);
    return 0;
}



int CMsg::RegistMessage(CommMessageShow fun, void* param)
{
    RETURN_CHK(fun, ERR_INPUT_PARAM);
    //RETURN_CHK(m_msgFun, ERR_REPEAT_REG);

    sys_EnterCriticalSection(m_hSection);
    m_msgFun = fun;
    m_msgParam = param;
    sys_LeaveCriticalSection(m_hSection);
    return 0;
}

int CMsg::RegistIAStudioMessage(CommMessageShow fun, void* param)
{
	sys_EnterCriticalSection(m_hSection);
	m_msgISFun = fun;
	m_msgISParam = param;
	sys_LeaveCriticalSection(m_hSection);
	return 0;
}

int CMsg::ShowMessage(MessageType type, const char *msg)
{
    CommMessageShow fun;
    void* param;

    RETURN_CHK_NOPRT(m_msgFun, MR_OK);

    sys_EnterCriticalSection(m_hSection);
    //fun = m_msgFun;
    //param = m_msgParam;
	fun = m_msgISFun ? m_msgISFun : m_msgFun;
	param = m_msgISFun ? m_msgISParam : m_msgParam;
    sys_LeaveCriticalSection(m_hSection);
    return (int)fun(msg, type, param);
}

int CMsg::RegistDevice(PushDeviceInfoCallback callback, void* param)
{
	sys_EnterCriticalSection(m_hSection);
	m_pushCallback = callback;
	m_pushParam = param;
	sys_LeaveCriticalSection(m_hSection);
	return 0;
}

int CMsg::PushDeviceInfo(char *type, char *context)
{
	RETURN_CHK_NOPRT(m_pushCallback, MR_OK);
	return m_pushCallback(type, context, m_pushParam);
}