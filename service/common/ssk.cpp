/*********************************************************************
    *文件说明: 提供进程间通信功能,任意一个进程调用create创建服务端，
            另外一个进程调用conn连接服务端，成功了则可以模仿socket
            互相发送消息，调用send_text发送消息 recv_fexe接收消息
            ps:一个服务端只能和一个客户端通信
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/23 CW0793 创建
                
***********************************************************************/

#include <string>
#include "GlobalDefine.h"
#include "sysapi.h"
#include "common.h"

#define SSK_FILE_EVENT  "OCTOPUS_COMMON_SSK_EVENT-CW1024"   //共享内存访问互斥量
#define MAPPING_NAME    "OCTOPUS_COMMON_SSK_MAPPING-CW1024" //虚拟内存共享 名称
#define MAPPING_EVENT   "OCTOPUS_COMMON_SSK_MAPPING-CW1023" //虚拟内存 互斥量
#define MAPPING_FINISH  "OCTOPUS_COMMON_SSK_FINISH-CW1024"  //虚拟内存 拷贝完成 互斥量
#define PAGE_READWRITE 0x04
#define FILE_MAP_READ  0x0004
#define FILE_MAP_WRITE 0x0002

#define MAX_SEND_SIZE   4096    //单次发送最大长度 4kb
#define MAX_BUFF_SIZE   128     //消息缓冲区长度
#define MAX_MSG_SIZE    1014    //消息体长度
#define SSK_ARRAY_LEN   32      //ssk缓冲区长度
#define SSK_NAME_LEN    30      //ssk名称长度
#define SERVER 0x0100
#define CLIENT 0x1000
//#define SSK_CLOSE_MSG   1       //对方已经关闭心跳包
//#define SSK_CONN_MSG    2       //客户端已连接心跳包
#define BUFF_VALID      0       //未用
#define BUFF_INVALID    1       //已用

struct ST_SSK          //可操作句柄对象
{
    short sc;    //客户端还是服务端？ SERVER  CLIENT   不能换顺序
    short index; //通讯合集缓冲区中的索引
    ST_SSK(DWORD copy = INVALID_SSK)
    {
        index = copy>>16 & 0xFFFF;
        sc = copy & 0xFFFF;
    }

    friend bool operator==(DWORD dw, ST_SSK& ssk)
    {
        return (ssk.index == (dw>>16 & 0xFFFF) && ssk.sc==(dw&0xFFFF));
    }
    ST_SSK& operator=(DWORD copy)
    {
        index = copy>>16 & 0xFFFF;
        sc = copy & 0xFFFF;
        return *this;
    }
};

struct ST_EXE_MSG //消息结构体
{
    ST_EXE_MSG* next;               //下一个数据指针 用于list
    int         size;               //数据大小
    char        msg[MAX_MSG_SIZE];  //数据,考虑存储对齐
    bool        isNext;             //是否有切割消息？若消息比较大，则会进行切割后发送
    char        reserved;           //标示该缓冲区是否使用
};

ST_EXE_MSG* msg_malloc();           //消息缓冲区管理 申请内存
void msg_free(ST_EXE_MSG* pst_msg); //消息缓冲区管理 释放内存

struct MSG_LIST //消息队列
{
    ST_EXE_MSG*     top;    //头结点指针
    ST_EXE_MSG*     end;    //尾节点指针
    unsigned int    cnt;    //当前有多少个成员

    unsigned int size(){return cnt;}
    ST_EXE_MSG* front(){return top;}
    void pop_front()
    {
        ST_EXE_MSG* ptmp = top;
        top = top->next;
        cnt--;
        if (0 == cnt)
        {
            end = NULL;
        }
    }
    void push_back(ST_EXE_MSG* pmsg)
    {
        if (!pmsg)
        {
            return;
        }

        pmsg->next = NULL;
        if (0 == cnt)
        {
            top = pmsg;
        }
        else
        {
            end->next = pmsg;
        }
        cnt++;
        end = pmsg;
    }
};

struct ST_EXE_SOCKET //ssk 存储区结构体
{
    char            name[SSK_NAME_LEN]; //服务名称,称作密码也行,最多31位
    bool            connected;          //是否已连接客户端
    char            reserved;           //标示该缓冲区是否使用
    MSG_LIST        msgList[2];         //服务端和客户端消息队列
    ST_EXE_SOCKET*  next;               //下一个数据指针，用于node

    bool IsValid(){return 0 == *name ? false : true;}       //判断该ssk server是否有效
    bool IsConn() {return connected;}   //判断该ssk是否已经有客户端了

    void ClearMsg(unsigned int sc)
    {
        ST_EXE_MSG* pst_msg = NULL;
        int index = (SERVER == sc) ? 0 : 1;
        while(msgList[index].size() > 0)
        {
            pst_msg = msgList[index].front();
            msgList[index].pop_front();
            msg_free(pst_msg);
        }
    }
    void ClearSSK(unsigned int sc)
    {
        connected = false;
        ClearMsg(sc);

        if (SERVER == sc)    //清理server端
        {
            ClearSSK(CLIENT);   //同时关闭客户端
            memset(name, 0, sizeof(name));
        }
    }
};

struct SSK_NODE //ssk链表
{
    ST_EXE_SOCKET* top;
    ST_EXE_SOCKET* end;
    unsigned int cnt;

    SSK_NODE(int cnt)
    {
        top = NULL;
        end = NULL;
        cnt = 1;
    }
    unsigned int size(){ return cnt;}
    bool push(ST_EXE_SOCKET* pnode)
    {
        if (NULL == pnode)
        {
            return false;
        }
        pnode->next = NULL;
        if (NULL == top)
        {
            top = pnode;
        }
        else
        {
            end->next = pnode;
        }
        cnt++;
        end = pnode;
        return true;
    }
    bool popup(ST_EXE_SOCKET* pnode)
    {
        ST_EXE_SOCKET* ptmp;
        ST_EXE_SOCKET* prev;

        if (NULL == pnode || NULL == top)
        {
            return false;
        }

        if (pnode == top)   //头节点
        {
            if (top == end) //只有一个节点
            {
                end = NULL;
            }
            top = top->next;
            cnt--;
            return true;
        }

        ptmp = top->next;
        prev = top->next;
        while(NULL != ptmp)
        {
            if (pnode == ptmp)  //找到目标
            {
                if (end == ptmp)    //尾节点
                {
                    end = prev;
                }
                prev->next = ptmp->next;
                cnt--;
                return true;
            }
            prev = ptmp;
            ptmp = ptmp->next;
        }
        return false;
    }
    
    bool ChkName(LPCSTR lpName) //检查一个服务名是否可用
    {
        ST_EXE_SOCKET* ptmp = top;
        if (NULL == lpName)
        {
            return false;
        }
        for(int i=0; i<(int)cnt; i++)
        {
            if (0 == strcmp(ptmp->name, lpName))
            {//名字相同
                return false;
            }
            ptmp = ptmp->next;
        }
        return true;
    }
};

#pragma data_seg("SharedData")
//以下定义的数据为共享数据，即所有调用dll的进程可以共享,但必须初始化
ST_EXE_SOCKET   g_sockArray[SSK_ARRAY_LEN] = {0};   //点对点通讯合集，最多支持SSK_ARRAY_LEN对
ST_EXE_MSG      g_msgArray[MAX_BUFF_SIZE] = {0};    //消息缓冲区, 最大支持MAX_BUFF_SIZE条消息
BOOL            g_isMainExist = TRUE;               //主进程是否存在？
#pragma data_seg()
#pragma comment(linker, "/SECTION:SharedData,RWS")

//自己进程才能用的全局变量
ST_EXE_SOCKET*   g_psockArray = NULL;    //点对点通讯合集，内存映射文件中的缓存
ST_EXE_MSG*      g_pmsgArray = NULL;     //消息缓冲区, 
SSK_NODE*        g_psskNode = NULL;      //ssk链表，装载所有有效ssk

HANDLE g_hEvent = INVALID_HANDLEVALUE;     //写消息队列临界区
HANDLE g_hMapFile = INVALID_HANDLEVALUE;   //内存映射文件句柄
HANDLE g_hMainThread = INVALID_HANDLEVALUE;//写内存映射文件线程句柄
HANDLE g_hMapEvent = INVALID_HANDLEVALUE;  //写内存映射文件事件
HANDLE g_hMapFinish = INVALID_HANDLEVALUE; //写内存映射文件结束事件
BOOL   g_isskInit = FALSE;                  //是否已经初始化
BOOL   g_isMain = FALSE;                    //是否为主进程？

//进入临界区
void EnterCSection()
{
    if (INVALID_HANDLEVALUE == g_hEvent)
    {
        g_hEvent = sys_CreateEvent(FALSE, TRUE, SSK_FILE_EVENT);    //若有，则获取已有的
    }
    sys_WaitForSingleObject(g_hEvent,INFINITE);
}

//离开临界区
void LeaveCSection()
{
    if (INVALID_HANDLEVALUE == g_hEvent)
    {
        g_hEvent = sys_CreateEvent(FALSE, TRUE, SSK_FILE_EVENT);
    }
    sys_SetEvent(g_hEvent);
}

//检查一个ssk句柄是否有效
int chk_ssk(ST_SSK* pssk)
{
    RETURN_NULLPTR(pssk, FALSE);
    if (INVALID_SSK == *pssk)
    {
        return ERR_PARAM;
    }

    if (pssk->index < 0 || pssk->index >= SSK_ARRAY_LEN ||
        (SERVER != pssk->sc && CLIENT != pssk->sc))
    {
        LOG_ERR("the ssk is invalid. sc=%d,index=%d", pssk->sc, pssk->index);
        return ERR_PARAM;
    }

    if (!g_sockArray[pssk->index].IsValid())
    {
        return ERR_NOINIT;
    }

    return COMMON_NORMAL;
}

//缓冲区管理 申请内存
ST_EXE_MSG* msg_malloc()
{
    ST_EXE_MSG* pst_msg = NULL;
    while(g_isskInit)
    {
        for(unsigned int i= 0; i<MAX_BUFF_SIZE; i++)
        {
            if (BUFF_VALID == g_msgArray[i].reserved) //未使用
            {
                g_msgArray[i].reserved = BUFF_INVALID; //标示已使用
                pst_msg = g_msgArray + i;
                return pst_msg;
            }
        }
        sys_Sleep(1);
        LOG_WARN("msg space all used.");
    }
    return NULL;    //缓冲区已用完
}

//缓冲区管理 释放内存
void msg_free(ST_EXE_MSG* pst_msg)
{
    if (NULL == pst_msg)
    {
        return;
    }
    __int64 i = pst_msg - g_msgArray;
    if (i >= MAX_BUFF_SIZE)
    {
        return;
    }
    memset(pst_msg, 0, sizeof(ST_EXE_MSG));
}

ST_EXE_SOCKET* ssk_malloc() //ssk缓冲区管理
{
    ST_EXE_SOCKET* pst_ssk = NULL;
    for(unsigned int i= 0; i<SSK_ARRAY_LEN; i++)
    {
        if (BUFF_VALID == g_sockArray[i].reserved) //未使用
        {
            pst_ssk = g_sockArray + i;
            pst_ssk->reserved = BUFF_INVALID;   //标示已使用
            return pst_ssk;
        }
    }
    return NULL;    //缓冲区已用完
}

void ssk_free(ST_EXE_SOCKET* pst_ssk) //ssk缓冲区 释放内存
{
    if (NULL == pst_ssk)
    {
        return;
    }

    __int64 i = pst_ssk - g_sockArray;
    if (i >= SSK_ARRAY_LEN)
    {
        return;
    }
    memset(g_sockArray + i, 0, sizeof(ST_EXE_SOCKET));
}

//检测主进程是否退出
//主进程运行此线程，则负责把共享内存的数据拷入内存映射文件
int WINAPI ThreadChkExe(void* lpThreadParameter) 
{
    while(TRUE)
    {//占领主进程
        if (!g_isskInit)
        {
            return 0;
        }

        if (g_isMainExist)
        {//主进程存在，那就不用干活
            sys_Sleep(500);
            continue;
        }
        g_isMainExist = TRUE;
        g_isMain = TRUE;    //把自己作为主进程
        break;
    }
    LOG_INFO("this is main tanger.");
    while(TRUE)
    {//主进程负责写内存映射文件
        sys_WaitForSingleObject(g_hMapEvent, INFINITE);
        if (!g_isskInit)    //退出信号？
        {
            break;
        }
        EnterCSection(); //卡死，不让再写入了
        memcpy(g_psockArray, g_sockArray, sizeof(g_sockArray));
        memcpy(g_pmsgArray, g_msgArray, sizeof(g_msgArray));
        LeaveCSection(); //此处不释放信号，等待新进程拷贝完成后再释放?
        sys_SetEvent(g_hMapFinish);
    }
    return 0;
}

void UpdateSSKArray()
{
    __int64 index = 0;
    ST_EXE_SOCKET* ptmp = g_psskNode->top;
    ST_EXE_MSG* pmsg = NULL;
    sys_WaitForSingleObject(g_hMapFinish, INFINITE);
    EnterCSection(); //此处应该保证有信号，其它进程都应该无法写入
    while(ptmp)
    {
        index = ptmp - g_sockArray;       //计算地址偏移
        memcpy(ptmp, g_psockArray + index, sizeof(ST_EXE_SOCKET));  //更新一个ssk有效缓冲区
        for(int i= 0; i< 2; i++)
        {
            pmsg = ptmp->msgList[i].top;
            while(pmsg)
            {
                index = pmsg - g_msgArray;       //计算msg地址偏移
                memcpy(pmsg, g_pmsgArray + index, sizeof(ST_EXE_MSG));  //更新一个msg有效缓冲区
                pmsg = pmsg->next;
            }
        }
        ptmp = ptmp->next;
    }
    LeaveCSection();    //更新完毕，释放写入事件
}

EXPORT_C bool WINAPI init_ssk()
{
    if (g_isskInit)
    {
        return true;
    }

    UINT sskMemory = sizeof(ST_EXE_SOCKET) * SSK_ARRAY_LEN;
    UINT msgMemory = sizeof(ST_EXE_MSG) * MAX_BUFF_SIZE;
    UINT nodeMemory = sizeof(SSK_NODE);
    char* pmalloc = NULL;

    g_isskInit = TRUE;
    g_hMapEvent = sys_CreateEvent(FALSE, FALSE, MAPPING_EVENT);
    g_hMapFinish = sys_CreateEvent(FALSE, FALSE, MAPPING_FINISH);
    g_hMainThread = sys_CreateThread(ThreadChkExe, NULL);

    g_hMapFile = sys_OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, MAPPING_NAME);
    if (NULL == g_hMapFile)
    {
        LOG_WARN("sys_OpenFileMapping fail, create the mapping File.");
        g_hMapFile = sys_CreateFileMapping(INVALID_HANDLEVALUE, PAGE_READWRITE, 
            0, nodeMemory+msgMemory+sskMemory, MAPPING_NAME);
        g_isMain = TRUE;        //第一个进程调用ssk 默认作为主进程
        g_isMainExist = FALSE;  //标示主进程不存在，让当前线程可以抢占主进程位置
    }

    if (NULL == g_hMapFile)  //再次失败，真的发生错误了
    {
        LOG_WARN("sys_CreateFileMapping Create File error.");
        uninit_ssk();
        return false;
    }

    pmalloc = (char*)sys_MapViewOfFile(g_hMapFile, FILE_MAP_READ | FILE_MAP_WRITE);

    g_psskNode = (SSK_NODE*)pmalloc;
    g_psockArray = (ST_EXE_SOCKET*)(pmalloc + nodeMemory);
    g_pmsgArray = (ST_EXE_MSG*)(pmalloc + nodeMemory + sskMemory);

    if (g_isMain)
    {//刚创建文件，初始化数值
        memset(pmalloc, 0, nodeMemory+msgMemory+sskMemory);
    }
    else
    {//主进程存在？从映射文件更新共享内存
        sys_SetEvent(g_hMapEvent);  //发送事件让主进程开始从缓冲区更新到内存映射文件
        UpdateSSKArray();           //更新到共享缓冲区
    }

    return true;
}

EXPORT_C void WINAPI uninit_ssk()
{
    if (INVALID_HANDLEVALUE != g_hEvent)
    {
        sys_CloseHandle(g_hEvent);
    }

    if (!g_isskInit)
    {
        return;
    }

    void* pmalloc = (void*)g_psskNode;
    if (pmalloc)
    {
        sys_UnmapViewOfFile(pmalloc);
    }

    if (NULL != g_hMapFile)
    {
        sys_CloseHandle(g_hMapFile);
    }

    g_isskInit = FALSE;
    if (TRUE == g_isMain)       //主进程退出
    {
        sys_SetEvent(g_hMapEvent);
        EnterCSection();
        g_isMainExist = FALSE;  //让别人抢去
        LeaveCSection();
    }
    
    sys_WaitForSingleObject(g_hMainThread, INFINITE);
    sys_CloseHandle(g_hMapEvent);
    sys_CloseHandle(g_hMapFinish);
    sys_CloseHandle(g_hMainThread);
}

EXPORT_C DWORD WINAPI ssk_server(LPCSTR sskName)
{
    ST_SSK ssk;
    DWORD ret = 0;

    RETURN_NULLPTR(sskName, INVALID_SSK);

    if (!g_isskInit)
    {
        init_ssk();
    }

    if (false == g_psskNode->ChkName(sskName))
    {//遍历ssk缓冲区，检查名字是否重复
        LOG_ERR("init ssk error . the ssk name already exists.");
        return INVALID_SSK;
    }

    EnterCSection();
    ST_EXE_SOCKET* pssk = ssk_malloc();
    if (NULL == pssk)
    {
        LeaveCSection();
        LOG_ERR("ssk_malloc return NULL.");
        return INVALID_SSK;
    }
    ssk.index = (int)(pssk - g_sockArray);
    ssk.sc = SERVER;
    g_psskNode->push(pssk);
    strcpy_s(pssk->name, SSK_NAME_LEN, sskName);
    memcpy(&ret, &ssk, sizeof(DWORD));
    LeaveCSection();
    return ret;
}

EXPORT_C DWORD WINAPI ssk_conn(LPCSTR sskName)
{
    ST_SSK st_ssk;
    DWORD ssk = 0;
    //char ch = SSK_CONN_MSG;    //发送心跳包，标示连接上了。

    RETURN_NULLPTR(sskName, INVALID_SSK);

    if (!g_isskInit)
    {
        init_ssk();
    }

    ST_EXE_SOCKET* ptmp = g_psskNode->top;
    for(int i=0; i<(int)g_psskNode->cnt; i++)
    {
        if (0 == strcmp(ptmp->name, sskName))
        {//名字相同
            if (true == ptmp->connected)
            {
                return INVALID_SSK;
            }

            EnterCSection();
            ptmp->connected = true;    //标示以连接上
            LeaveCSection();

            st_ssk.index =(int)(ptmp - g_sockArray);
            st_ssk.sc = CLIENT;
            memcpy(&ssk, &st_ssk, sizeof(DWORD));
            return ssk;
        }
        ptmp = ptmp->next;
    }

    return INVALID_SSK;
}

EXPORT_C int WINAPI ssk_status(DWORD ssk)
{
    ST_SSK st_ssk = ssk;
    //char ch = SSK_CLOSE_MSG;    //发送停止心跳包
    RETURN_NULLPTR(g_isskInit, ERR_NOINIT);

    int ret = chk_ssk(&st_ssk);
    if (COMMON_NORMAL != ret)
    {
        return ret;
    }

    if (!g_sockArray[st_ssk.index].IsConn())
    {
        return ERR_NOCONNECT;
    }

    return COMMON_NORMAL;
}

EXPORT_C int WINAPI ssk_is_server(DWORD ssk)
{
    ST_SSK st_ssk = ssk;
    int ret = chk_ssk(&st_ssk);
    if (COMMON_NORMAL != ret)
    {
        return ret;
    }

    return (SERVER == st_ssk.sc) ? 0 : 1;
}

EXPORT_C void WINAPI ssk_close(DWORD ssk)
{
    ST_SSK st_ssk = ssk;
    //char ch = SSK_CLOSE_MSG;    //发送停止心跳包
    if (!g_isskInit)
    {
        return;
    }

    int ret = chk_ssk(&st_ssk);
    if (COMMON_NORMAL != ret)
    {
        return;
    }

    //ssk_send(ssk, &ch, sizeof(ch));  //发送心跳包,1标示已关闭
    EnterCSection();
    ST_EXE_SOCKET* pssk = &g_sockArray[st_ssk.index];
    pssk->ClearSSK(st_ssk.sc);
    if (SERVER == st_ssk.sc)
    {
        g_psskNode->popup(pssk);
        ssk_free(pssk);
    }
    LeaveCSection();
}

EXPORT_C bool WINAPI regist_ssk_recv(DWORD ssk, RegistRecvProc proc)
{
    RETURN_NULLPTR(g_isskInit, false);

    return false;
}

#define TIME_BASE 1     //检测间隔 单位 ms
EXPORT_C int WINAPI ssk_recv(DWORD ssk, LPSTR pdata, int len, DWORD timeout)
{
    ST_SSK st_ssk = ssk;
    ST_EXE_MSG* pst_msg = NULL;
    int datalen = 0;
    int nLen = 0;
    int ret = COMMON_NORMAL;
    bool isNext = true;
    LPSTR ptmp = pdata;
    int tout = (int)timeout;

    RETURN_NULLPTR(g_isskInit, ERR_NOINIT);

    int index = (SERVER == st_ssk.sc) ? 0 : 1;
    while(tout >= 0 || INFINITE == timeout)
    {
        ret = ssk_status(ssk);
        if (COMMON_NORMAL != ret)
        {
            return ret;
        }

        if (0 == g_sockArray[st_ssk.index].msgList[index].size())   //判断有没有消息
        {
            sys_Sleep(TIME_BASE);
            tout -= TIME_BASE;
            continue;
        }
        tout = 0;
        break;
    }

    if (0 != tout)
    {
        return ERR_TIMEOUT2;
    }

    while(isNext)
    {//防止有多条消息要合并，所以给个循环
        if (0 == g_sockArray[st_ssk.index].msgList[index].size())   //判断有没有消息
        {
            break;
        }
        EnterCSection();
        pst_msg = g_sockArray[st_ssk.index].msgList[index].front(); //取出第一个消息
        g_sockArray[st_ssk.index].msgList[index].pop_front();
        datalen = Min(len, pst_msg->size);
        if (datalen > 0 && COMMON_NORMAL == ret)    //可以拷贝数据
        {//若有
            memcpy(ptmp, pst_msg->msg, datalen);
            ptmp += datalen;
            nLen += datalen;
            len -= datalen;
        }
        if (datalen <= 0 && COMMON_NORMAL == ret)   //无存储空间
        {//小于0 则说明应用层给的缓冲区大小不够，但还是需要把连续消息读完
            LOG_ERR("recv data size is so small.datalen=%d, but input len=%d", pst_msg->size, len);
            ret = ERR_TOOLARGE;
        }
        
        isNext = pst_msg->isNext;
        msg_free(pst_msg);
        LeaveCSection();
    }
    
    return nLen;
}

EXPORT_C int WINAPI ssk_send(DWORD ssk, LPSTR pdata, int len)
{
    ST_SSK st_ssk = ssk;
    int datalen = 0;
    int sendlen = 0;

    RETURN_NULLPTR(g_isskInit, ERR_NOINIT);

    int ret = ssk_status(ssk);
    if (COMMON_NORMAL != ret)
    {
        return ret;
    }

    if (len > MAX_SEND_SIZE)
    {
        return ERR_TOOLARGE;
    }

    //来个取反,因为获取的是自己的sc标示，发消息时，必须写入到对方的消息队列
    int index = (CLIENT == st_ssk.sc) ?  0: 1;
    ST_EXE_MSG* pst_msg = NULL;
    while(TRUE)
    {
        pst_msg = msg_malloc(); //malloc内部不能加锁，否则会死锁哦
        if (NULL == pst_msg)
        { //判断是否申请到内存
            LOG_ERR("msg_malloc return NULL.");
            return sendlen;
        }
        datalen = Min(len, MAX_MSG_SIZE);
        EnterCSection();
        memcpy(pst_msg->msg, pdata+sendlen, datalen);
        sendlen += datalen;
        pst_msg->size = datalen;
        g_sockArray[st_ssk.index].msgList[index].push_back(pst_msg);   //将消息压入消息队列后排
        len -= MAX_MSG_SIZE;
        if (len <= 0)
        {
            LeaveCSection();
            break;
        }
        pst_msg->isNext = true; //若还没退出，则认为是大文件，标示后面还有
        LeaveCSection();
    }

    return sendlen;
}

EXPORT_C int WINAPI ssk_clear_msg(DWORD ssk)
{
    ST_SSK st_ssk = ssk;

    RETURN_NULLPTR(g_isskInit, ERR_NOINIT);

    int ret = ssk_status(ssk);
    if (COMMON_NORMAL != ret)
    {
        return ret;
    }
    EnterCSection();
    g_sockArray[st_ssk.index].ClearMsg(st_ssk.sc);
    LeaveCSection();
    return COMMON_NORMAL;
}