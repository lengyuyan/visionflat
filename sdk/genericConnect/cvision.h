#pragma once
#ifndef DLL_INCLUDE_GENERIC_VISIONPROCESS
#define DLL_INCLUDE_GENERIC_VISIONPROCESS
#include "sysapi.h"
#include <hash_map>
#include "skt.h"

using std::hash_map;

#define INVALID_VALUE -1
#define DATA_SEG_FLAG   ","            //解析符号
#define SPLIT_FLAG      "|"            //分割符号
#define END_CHAR        '#'
#define REPLY_TIMEOUT   5000

struct TVisionMsg
{//消息结构体
    int     ccdok;  //拍照结果
    int     res;    //是否有处理结果   0 标识有消息 否则为错误代码
    int     cures;  //是否有自定义消息 0 标识有消息 否则为错误代码
    char    msg[MAX_BUF_LEN];   //处理结果
    char    cus[MAX_BUF_LEN];   //自定义消息
};

struct TVisionResult;
class CVision
{
public:
	CVision(void);
	virtual ~CVision(void);

    int GetStatus() { return SOCKET_CONNECTED == skt_status(m_socket) ? EMVISION_NORMAL : EMVISION_ERROR; }

	int Init(const char* lpSrv, unsigned short sport, int bClient = 0);

	int VisionProcess(int secne); //new add 
	int VisionProcessResult(int secne, TVisionResult* pres, int timeout);//new add 
	int GetVisionImage(int secne, char* pdata);//new add 
	int GetVisionImagePath(int secne, char* path);//new add 
	int SendVisionCalibData(int secne, double x, double y);//new add 
	int RecvVisionCalibData(int secne, double x, double y);//new add 
	int ConvertCalibData(unsigned short visionID, int secne, TVisionResult* pres);//new add 

	int SendCustomMsg(const char* cmd, const char* msg);      //发送自定义消息
	int RecvCustomMsg(const char* cmd, char* recvMsg, int timeout);   //接收自定义消息

protected:
	// TODO: 在此定义内部功能接口
    void ClearMsg(int secne);
    void ClearCusMsg(int secne);
    int ParseResult(char *psrc, TVisionResult* pres);

    int ReadOkMsg(int secne);
    int ReadMsg(int secne, char* recvMsg);    //读取并重置消息缓冲区
    int ReadCusMsg(int secne, char* recvMsg);
    int RecvCCDOK(int secne, int timeout);
	int RecvResult(int secne, char* recvMsg, int timeout = 0);   //接收消息
    int RecvMsg();
    int SendMsg(const char* pdata);

    //解析已经收到的单条消息，返回是否已找到目标消息
    bool ParseMessage();

private:

	bool    m_bInit;        //是否初始化
	bool    m_bClient;      //标识是否为客户端
    bool    m_bRecv;        //是否在接收
	int     m_socket;       //与vision通讯句柄
	char    m_szSrv[MAX_IP_LEN];    //srver  ip
	char    m_szCli[MAX_IP_LEN];    //client ip
	unsigned short m_usPort;        //srv port
	unsigned short m_ucPort;        //client port

    Handle  m_hSection;     //临界区

    char    m_szRecvMsg[MAX_BUF_LEN];   //保存上一次消息截取后的不完整消息
    hash_map<int, TVisionMsg*>  m_hash_msg;  //消息内容map key = scene
};

#endif