/*********************************************************************
*文件说明: 处理所有收到的消息，有线程处理
*作者: luojing
*日期: 2014/01/24
*修改日志:  2014/01/24 CW0793 创建
            2016/04/01 EW-0551 修改
            1. 修改接收消息函数，根据是否接收消息判断是否需要sleep
            2. 按id增加event，使用事件等待触发消息，反应更敏捷

***********************************************************************/
#include <stdio.h>
#include <string>
#include "common.h"
#include "skt.h"
#include "vision_api.h"
#include "cvision.h"

#include "GolobalError.h"

using std::pair;

//CVision类
CVision::CVision(void)
{
    m_bInit = false;
    m_usPort = 0;
    m_ucPort = 0;
    m_bRecv = false;
    m_bClient = false;
    m_socket = INVALID_SKT;
    memset(m_szSrv, 0, MAX_IP_LEN);
    memset(m_szCli, 0, MAX_IP_LEN);
    memset(m_szRecvMsg, 0, MAX_BUF_LEN);
    m_hSection = sys_InitializeCriticalSection();
}

CVision::~CVision(void)
{
    hash_map<int, TVisionMsg*>::iterator it;

    m_bInit = false;
    if (INVALID_SKT != m_socket)
    {
        skt_close(m_socket);
    }
    sys_EnterCriticalSection(m_hSection);
    for (it = m_hash_msg.begin(); it != m_hash_msg.end(); it++)
    {
        delete it->second;
    }
    m_hash_msg.clear();
    sys_DeleteCriticalSection(m_hSection);
}

int CVision::Init(const char* lpSrv, unsigned short sport, int bClient)
{
    if (sport < 1024)
    {
        LOG_ERR("CVision::Init(%s, %d) server port must large than 1024.", lpSrv, sport);
        return ERR_INPUT_PARAM;
    }

    m_usPort = sport;
    strcpy_s(m_szSrv, lpSrv);
    m_bClient = bClient ? true : false;
    strcpy_s(m_szCli, ANY_IP);
    m_socket = m_bClient ? create_c(m_szSrv, m_usPort) : create_s(m_szSrv, m_usPort);
    if (INVALID_SKT == m_socket)
    {
        LOG_ERR("Vision InitSKT Create Socket Server return error %d", get_last_err());
        return ERR_INIT_NET;
    }
    m_bInit = true;
    return RETURN_OK;
}

//new add 视觉处理
int CVision::VisionProcess(int secne)
{
    int ret = 0;
	char buff[32] = {0};

	//是否处理前要清除socket缓冲区
    RETURN_CHK(m_bInit, ERR_NO_INIT);

    ClearMsg(secne);    //清空缓冲区
    sprintf_s(buff, "S|%d|#", secne);
    ret = SendMsg(buff);
    sys_Sleep(1);
    if (!ret)
    {
        ret = RecvCCDOK(secne, REPLY_TIMEOUT);
    }
	return ret;
}

int CVision::VisionProcessResult(int secne, TVisionResult* pres, int timeout)
{
    int ret = 0;
	char buff[MAX_BUF_LEN] = {0};

    RETURN_CHK(pres, ERR_INPUT_PARAM);
    RETURN_CHK(m_bInit, ERR_NO_INIT);

	ret = RecvResult(secne, buff, timeout);

    RETURN_CHK(!ret, ret);

	return ParseResult(buff, pres);
}

int CVision::GetVisionImage(int secne, char* pdata)
{//no support
	return ERR_INPUT_PARAM;
}

int CVision::GetVisionImagePath(int secne, char* path)
{//no support
	return ERR_INPUT_PARAM;
}

int CVision::SendVisionCalibData(int secne, double x, double y)
{
    int ret = 0;
    char buff[MAX_BUF_LEN] = {0};

    //是否处理前要清楚socket缓冲区
    RETURN_CHK(m_bInit, ERR_NO_INIT);

    sprintf_s(buff, "S|%d|%.3lf,%.3lf|#", secne, x, y);
    LOG_INFO("send -> vision - %s", buff);
    ret = SendMsg(buff);
    if (!ret)
    {
        ret = RecvCCDOK(secne, REPLY_TIMEOUT);
        if (1 == ret)
        {//recv OK;
            ret = 0;
            LOG_INFO("recv -> vision: %d - %s", ret);
        }
    }
    return ret;
}

int CVision::RecvVisionCalibData(int secne, double x, double y)
{
	return ERR_INPUT_PARAM;
}

int CVision::ConvertCalibData(unsigned short visionID, int secne, TVisionResult* pres)
{
	return ERR_INPUT_PARAM;
}

int CVision::ParseResult(char *psrc, TVisionResult* pres)
{
	int index = 0;
    char* ptmp = NULL;

	memset(pres, 0, sizeof(TVisionResult));
    do
    {
        ptmp = strtok_s(NULL, DATA_SEG_FLAG, &psrc);
        if (NULL == ptmp)
        {
            break;
        }

        if (index >= VISION_RES_CNT)
        {//数据超限
            LOG_ERR("CVision::ParseResult the result count to large.");
            return ERR_DATA_OVERLIMIT;
        }

        trim_char(ptmp, "\n ");
        if (!get_str_type(ptmp))
        {//数据格式不对
            LOG_ERR("CVision::ParseResult the param is not number -- %s", ptmp);
            return ERR_FORMAT_MSG;
        }
        pres->val[index++] = atof(ptmp);
    } while (1);
    pres->res = 0;
	pres->resCnt = index;
    return pres->resCnt > 0 ? 0 : ERR_TIMEOUT;
}

int CVision::SendCustomMsg(const char* cmd, const char* msg)
{
    char buff[MAX_BUF_LEN] = {0};
	//char buff[4096] = {0}; //发送数据太大时用
    RETURN_CHK(EMSTR_INT == get_str_type(cmd), ERR_INPUT_PARAM);
    RETURN_CHK(m_bInit, ERR_NO_INIT);

    if (msg && *msg)
    {
        sprintf_s(buff, "T|%s|%s|#", cmd, msg);
    }
    else
    {
        sprintf_s(buff, "T|%s|#", cmd);
    }
    ClearCusMsg(atoi(cmd));
    return SendMsg(buff);
}

int CVision::RecvCustomMsg(const char* cmd, char* recvMsg, int timeout)
{
    int ret = 0;
    int sence = 0;
    int start = clock();

    RETURN_CHK(m_bInit, ERR_NO_INIT);
	RETURN_CHK(recvMsg && EMSTR_INT == get_str_type(cmd), ERR_INPUT_PARAM);

    sence = atoi(cmd);
    RETURN_CHK_NOPRT(ReadCusMsg(sence, recvMsg), 0);

    while(clock() - start <= timeout)
    {
        ret = RecvMsg();
        if (ERR_TIMEOUT != ret && ret < 0)
        {
            break;
        }

        RETURN_CHK_NOPRT(ReadCusMsg(sence, recvMsg), 0);
    }
    return ret;
}

int CVision::SendMsg(const char* pdata)
{
    int ret = 0;
    int sendLen = (int)strlen(pdata);
    LOG_INFO("send -> vision - %s", pdata);
    sys_EnterCriticalSection(m_hSection);
    ret = skt_send(m_socket, pdata, sendLen);
    sys_LeaveCriticalSection(m_hSection);
    return ret > 0 ? 0 : ret;
}

void CVision::ClearMsg(int secne)
{
    hash_map<int, TVisionMsg*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hash_msg.find(secne);
    if (it != m_hash_msg.end())
    {//new
        it->second->ccdok = ERR_TIMEOUT;
        it->second->res = ERR_TIMEOUT;
        memset(it->second->msg, 0, MAX_BUF_LEN);
    }
    sys_LeaveCriticalSection(m_hSection);
}

void CVision::ClearCusMsg(int secne)
{
    hash_map<int, TVisionMsg*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hash_msg.find(secne);
    if (it != m_hash_msg.end())
    {//new
        it->second->cures = ERR_TIMEOUT;
        memset(it->second->cus, 0, MAX_BUF_LEN);
    }
    sys_LeaveCriticalSection(m_hSection);
}

int CVision::ReadOkMsg(int secne)
{
    int ret = ERR_TIMEOUT;
    hash_map<int, TVisionMsg*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hash_msg.find(secne);
    if (it != m_hash_msg.end())
    {//ok
        ret = it->second->ccdok;
    }
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

int CVision::ReadMsg(int secne, char* recvMsg) //从hashmap读msg
{
    int ret = ERR_TIMEOUT;
    char* ptmp = NULL;
    hash_map<int, TVisionMsg*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hash_msg.find(secne);
    if (it != m_hash_msg.end() && 0 == it->second->res)
    {//ok
        ret = 0;
        ptmp = it->second->msg;
        it->second->ccdok = ERR_TIMEOUT;
        it->second->res = ERR_TIMEOUT;
        while(*ptmp)
        {//copy
            *recvMsg++ = *ptmp++;
        }
    }
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

int CVision::ReadCusMsg(int secne, char* recvMsg) //从hashmap读msg
{
    int ret = ERR_TIMEOUT;
    char* ptmp = NULL;
    hash_map<int, TVisionMsg*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hash_msg.find(secne);
    if (it != m_hash_msg.end() && 0 == it->second->cures)
    {//ok
        ret = 0;
        ptmp = it->second->cus;
        it->second->cures = ERR_TIMEOUT;
        while(*ptmp)
        {//copy
            *recvMsg++ = *ptmp++;
        }
    }
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

//外部加锁
bool CVision::ParseMessage()
{
    int sence = 0;
    bool bcus = false;
    char *ptmp, *pNextToken;
    TVisionMsg* pmsg = NULL;
    hash_map<int, TVisionMsg*>::iterator it;

    ptmp = strtok_s(m_szRecvMsg, SPLIT_FLAG, &pNextToken);
    RETURN_CHK(ptmp && 1 == strlen(ptmp) && ('C' == *ptmp || 'T' == *ptmp), false);
    if ('T' == *ptmp)
    {
        bcus = true;
    }
    ptmp = strtok_s(NULL, SPLIT_FLAG, &pNextToken);
    RETURN_CHK(EMSTR_INT == get_str_type(ptmp), false);
    sence = atoi(ptmp);
    ptmp = strtok_s(NULL, SPLIT_FLAG, &pNextToken);
    RETURN_CHK(ptmp && *ptmp, false);

    it = m_hash_msg.find(sence);
    if (it == m_hash_msg.end())
    {//new
        pmsg = new TVisionMsg;
        pmsg->ccdok = ERR_TIMEOUT;
        pmsg->res = ERR_TIMEOUT;
        pmsg->cures = ERR_TIMEOUT;
        memset(pmsg->msg, 0, MAX_BUF_LEN);
        memset(pmsg->cus, 0, MAX_BUF_LEN);
        m_hash_msg.insert(pair<int, TVisionMsg*>(sence, pmsg));
    }
    else
    {//exist
        pmsg = it->second;
    }
    
    if (bcus)
    {//自定义消息
        pmsg->cures = 0;
        strcpy_s(pmsg->cus, ptmp);
    }
    else if (EMSTR_INT == get_str_type(ptmp))
    {//拍照结果
        pmsg->ccdok = (1 == atoi(ptmp)) ? 0 : ERR_VISION_RES;
    }
    else
    {//结果返回
        pmsg->res = 0;
        strcpy_s(pmsg->msg, ptmp);
    }

    return true;
}

int CVision::RecvMsg()
{
    int ret = 0;
    int kskt = 0;
    char buff[MAX_BUF_LEN] = {0};
    char* pdata = buff;
    char *ptmp = m_szRecvMsg + strlen(m_szRecvMsg);

    sys_EnterCriticalSection(m_hSection);
    if (m_bRecv)
    {//标识正在接收
        sys_LeaveCriticalSection(m_hSection);
        sys_Sleep(1);
        return 1;
    }
    m_bRecv = true;
    sys_LeaveCriticalSection(m_hSection);

    kskt = get_socket(m_socket);
    ret = sys_Select(&kskt, 1, 50);
    if (ERR_NO_CONNECT == ret && m_bInit)
    {
        skt_conn(m_socket);
    }

    if (ret)
    {//有消息应该返回0
        m_bRecv = false;
        return ret;
    }

    sys_EnterCriticalSection(m_hSection);
    ret = sys_Recv(kskt, buff, MAX_BUF_LEN - 1);
    sys_LeaveCriticalSection(m_hSection);
    if (ERR_NO_CONNECT == ret && m_bInit)
    {
        skt_conn(m_socket);
    }
    if (ret < 0)
    {
        m_bRecv = false;
        return ret;
    }

    LOG_INFO("recv from vision:%s, len=%d", buff, ret);
    while(*pdata)
    {//一直读取到结束符
        if (END_CHAR != *pdata)
        {
            *ptmp++ = *pdata++;
            continue;
        }

        trim_char(m_szRecvMsg, "\r\n ");
        sys_EnterCriticalSection(m_hSection);
        ParseMessage();
        sys_LeaveCriticalSection(m_hSection);
        memset(m_szRecvMsg, 0, MAX_BUF_LEN);
        ptmp = m_szRecvMsg;
        pdata++;    //过滤分隔符
    }
    m_bRecv = false;
    return 0;
}

int CVision::RecvCCDOK(int secne, int timeout)
{
    int ret = 0;
    int start = clock();

    ret = ReadOkMsg(secne);

    RETURN_CHK_NOPRT(ERR_TIMEOUT == ret, ret);

    while(clock() - start <= timeout)
    {
        ret = RecvMsg();
        if (ERR_TIMEOUT != ret && ret < 0)
        {
            break;
        }

        ret = ReadOkMsg(secne);
        if (ERR_TIMEOUT != ret)
        {//先读取
            break;
        }
    }
    return ret;
}

int CVision::RecvResult(int secne, char* recvMsg, int timeout)
{
    int ret = 0;
    int start = clock();

    RETURN_CHK_NOPRT(ReadMsg(secne, recvMsg), 0);

    while(clock() - start <= timeout)
    {
        ret = RecvMsg();
        if (ERR_TIMEOUT != ret && ret < 0)
        {
            break;
        }

        RETURN_CHK_NOPRT(ReadMsg(secne, recvMsg), 0);
    }
    return ret;
}