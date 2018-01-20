#pragma once
#ifndef DLL_INCLUDE_SYSAPI_SERIAL_H
#define DLL_INCLUDE_SYSAPI_SERIAL_H

#define  MAX_PORT_NAME_LEN 256
#define  INVALID_COM_HANDLE  -1
#define  MAX_DATA_SIZE_B 4096

struct ComParam;
struct _OVERLAPPED;
class CSerial
{//串口类
public:
    CSerial(void);
    ~CSerial(void);

    //打开串口，成功返回0， 失败返回错误码
    int open(const ComParam* pComData);

    void close();

    int read(char* pBuffer, int nSize, int timeout, int bLock = true);  //读取
    int write(const char* pBuffer, int nWrite, int bLock = true);       //写入
    int wRead(char* lpBuff, int nWrite, int nSize, int timeout);        //写入并读取,阻塞锁执行

private:
    
    Handle  m_hFile;            //com句柄
    Handle  m_hOperationEvt;    //操作锁
    _OVERLAPPED*    m_pOL;      //异步
    int     m_bOverlapped;      //是否异步
};
#endif // __CNTLCOM_H__