#include <string>
#include "GlobalDefine.h"
#include "GolobalError.h" 
#include "sysapi.h"
#include "serial.h"
#include <windows.h>

CSerial::CSerial(void)
{
    m_pOL = NULL;
    m_hFile = INVALID_HANDLE;
    m_bOverlapped = FALSE;
    m_hOperationEvt = sys_CreateEvent(FALSE, TRUE, NULL);
}

CSerial::~CSerial(void)
{
    close();
    if (m_pOL)
    {
        sys_CloseHandle(m_pOL->hEvent);
        delete m_pOL;
    }
    sys_CloseHandle(m_hOperationEvt);
}

int CSerial::open(const ComParam* pComData)
{
    int ret = 0;
    DCB cb;
    COMMTIMEOUTS tmout;

    char szBuff[MAX_BUF_LEN] = {0};

    RETURN_CHK_NOPRT(pComData, ERR_INPUT_PARAM);

    m_bOverlapped = pComData->bOverlapped;
    if (m_bOverlapped)
    {
        m_pOL = new _OVERLAPPED;
        m_pOL->InternalHigh = 0;
        m_pOL->Offset = 0;
        m_pOL->OffsetHigh = 0;
        m_pOL->hEvent = sys_CreateEvent(FALSE, FALSE, NULL);
    }
    sprintf_s(szBuff, "\\\\.\\COM%d", pComData->comPort);
    m_hFile = CreateFile(
        szBuff,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | (m_bOverlapped ? FILE_FLAG_OVERLAPPED : 0),   //异步打开
        NULL);
    if(INVALID_HANDLE == m_hFile)
    {
        TRACEF("CreateFile return error %d.", GetLastError());
        return ERR_OPEN_COM;
    }

    if (!GetCommState(m_hFile, &cb))
    {
        TRACEF("sys_SetCommState GetCommState return error.%d", GetLastError());
        return ERR_GET_COMSTATE;
    }

    cb.BaudRate = pComData->baudRate;
    cb.ByteSize = (BYTE)pComData->btSize;
    cb.Parity = (BYTE)pComData->parity;
    cb.StopBits = (BYTE)pComData->stopBit;
    cb.fDtrControl  = 0x01; //12-30,usb转串口的一个比设值
    cb.fDsrSensitivity = 0;
    cb.fOutxCtsFlow = 0;
    cb.fOutxDsrFlow = 0;
    cb.fOutX        = 0;
    cb.fInX         = 0;
    cb.fParity      = (EMP_NO == pComData->parity) ? 0 : 1;

    if (!SetCommState(m_hFile, &cb))
    {
        TRACEF("sys_SetCommState SetCommState return error.%d", GetLastError());
        return ERR_SET_COMSTATE;
    }

    if (!SetupComm(m_hFile, pComData->readSize, pComData->writeSize))
    {
        TRACEF("sys_SetCommState SetupComm return error.%d", GetLastError());
        return ERR_SET_COMSTATE;
    }

    tmout.ReadIntervalTimeout = MAX_TIMEOUT;
    tmout.ReadTotalTimeoutMultiplier = 0;
    tmout.ReadTotalTimeoutConstant = m_bOverlapped ? 10 : 0;
    tmout.WriteTotalTimeoutMultiplier = m_bOverlapped ? 10 : 10;
    tmout.WriteTotalTimeoutConstant = m_bOverlapped ? 1000 : 1000;
    if (!SetCommTimeouts(m_hFile, &tmout))
    {
        TRACEF("sys_SetCommState SetCommTimeouts return error.%d", GetLastError());
        return ERR_SET_COMSTATE;
    }

    if (!SetCommMask(m_hFile, 0x01))
    {//char 触发
        TRACEF("sys_SetCommState SetCommMask return error.%d", GetLastError());
        return ERR_SET_COMSTATE;
    }

    // 清空缓冲区
    if (!PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_TXABORT | PURGE_RXABORT | PURGE_RXCLEAR))
    {
        TRACEF("sys_SetCommState GetCommState return error.%d", GetLastError());
        return ERR_CLEAR_COMSTACK;
    }

    return RETURN_OK;
}

void CSerial::close()
{
    if (INVALID_HANDLE != m_hFile)
    {
        PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_TXABORT | PURGE_RXABORT | PURGE_RXCLEAR);
        sys_CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE;
    }
}

int CSerial::read(char* pBuffer, int nSize, int timeout, int bLock)
{
    DWORD error;
    DWORD readLen = 0;

    RETURN_CHK_NOPRT(INVALID_HANDLE != m_hFile, ERR_OPEN_COM);
    RETURN_CHK_NOPRT(pBuffer && nSize > 0, ERR_INPUT_PARAM);

    if (bLock)
    {
        sys_WaitForSingleObject(m_hOperationEvt);
    }

    if(!ClearCommError(m_hFile, &error, NULL) && error > 0)
    {//清除错误
        PurgeComm(m_hFile, PURGE_RXABORT | PURGE_RXCLEAR);
    }

    if (m_bOverlapped)
    {
        if(!ReadFile(m_hFile, pBuffer, nSize, &readLen, m_pOL) 
            && ERROR_IO_PENDING == GetLastError())
        {//写入中
            if (WAIT_OBJECT_0 == WaitForSingleObject(m_pOL->hEvent, timeout))
            {
                readLen = (DWORD)strlen(pBuffer);
            }
            else
            {
                PurgeComm(m_hFile, PURGE_RXABORT | PURGE_RXCLEAR);
            }
        }
    }
    else
    {
        ReadFile(m_hFile, pBuffer, nSize, &readLen, NULL);
    }

    if (bLock)
    {
        sys_SetEvent(m_hOperationEvt);
    }
    return (int)readLen;
}

int CSerial::write(const char* pBuffer, int nWrite, int bLock)  
{
    DWORD error;
    DWORD writelen = 0;

    RETURN_CHK_NOPRT(INVALID_HANDLE != m_hFile, ERR_OPEN_COM);
    RETURN_CHK_NOPRT(pBuffer && nWrite > 0, ERR_INPUT_PARAM);

    if (bLock)
    {
        sys_WaitForSingleObject(m_hOperationEvt);
    }

    if(!ClearCommError(m_hFile, &error, NULL) && error > 0)
    {//清除错误
        PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_TXABORT);
    }

    if (m_bOverlapped)
    {
        if(!WriteFile(m_hFile, pBuffer, nWrite, &writelen, m_pOL) 
            && ERROR_IO_PENDING == GetLastError())
        {//写入中
            if (WAIT_OBJECT_0 == WaitForSingleObject(m_pOL->hEvent, 100))
            {
                writelen = nWrite;
            }
            else
            {
                PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_TXABORT);
            }
        }
    }
    else
    {
        WriteFile(m_hFile, pBuffer, nWrite, &writelen, NULL);
    }

    if (bLock)
    {
        sys_SetEvent(m_hOperationEvt);
    }
    return (int)writelen;
}

int CSerial::wRead(char* pBuffer, int nWrite, int nSize, int timeout)
{
    /*clock_t begin;
    clock_t cost;*/
    int ret = 0;

    sys_WaitForSingleObject(m_hOperationEvt);
    ret = write(pBuffer, nWrite, FALSE);
    if (nWrite == ret)
    {
        /*DWORD dwBegin = GetTickCount(), dwEnd, dwCost, uReadReturn;
        ReadFile(m_hFile, pBuffer, nSize, &uReadReturn, m_pOL);
        readLen += uReadReturn;
        do
        {
            if (!ReadFile(m_hFile, pBuffer + readLen, 1, &uReadReturn, m_pOL))
            {
                if (timeout > 5 && WAIT_OBJECT_0 == WaitForSingleObject(m_pOL->hEvent, timeout))
                {
                    dwEnd = GetTickCount();
                    dwCost = dwEnd >= dwBegin ? dwEnd - dwBegin : DWORD(-1L) - dwBegin + dwEnd;
                    GetOverlappedResult(m_hFile, m_pOL, &uReadReturn, FALSE);
                    timeout = timeout > dwCost ? timeout - dwCost : 0;
                }
                else
                {
                    PurgeComm(m_hFile, PURGE_RXABORT | PURGE_RXCLEAR);
                    break;
                }
            }
        }while (uReadReturn && ++readLen < nSize);
        return readLen;*/

        if (!m_bOverlapped)
        {
            Sleep(timeout);
        }
        ret = read(pBuffer, nSize, timeout, FALSE);
        /*while(timeout)
        {
            begin = clock();
            if (ReadFile(m_hFile, pBuffer, nSize, &readLen, m_pOL))
            {
                break;
            }

            if (WAIT_OBJECT_0 != WaitForSingleObject(m_pOL->hEvent, timeout))
            {
                break;
            }

            cost = clock() - begin;
            timeout = timeout > cost ? timeout - cost : 0;
        }*/
    }
    //PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_TXABORT | PURGE_RXABORT | PURGE_RXCLEAR);
    sys_SetEvent(m_hOperationEvt);
    return ret;
}