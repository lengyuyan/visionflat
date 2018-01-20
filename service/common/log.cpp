/****************************************************
    *文件说明: 本文件主要封装打印log功能
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/22 CW0793 创建
                
******************************************************/



#include <stdio.h>
#include <stdarg.h>
#include <fstream>
#include "sysapi.h"
#include "common.h"

#define LOG_NAME    "log\\Flatfish.log"
#define LOG_MAX_BUFF 1024
#define MAX_FILE_LEN    1024*1024*20    //20M

#define _DEBUG__ 1

int initflag = 0;
const char* g_strLogLevel[] ={"", " <INFO> ", " <WARN> ", " <ERRO> ", " <CRIT> "};
std::fstream m_sfpLog;
long  m_nLogSize;
//CRITICAL_SECTION      m_hLogSection;     //日志临界区

HANDLE     m_hLogSection;     //日志临界区

void CloseLogFile()
{
	if (m_sfpLog.is_open())
	{
		m_sfpLog.close();
	}

}

void init_log()
{
	if (0!= initflag)
	{
		return;
	}
	m_hLogSection = sys_InitializeCriticalSection();
	create_path("log");
	m_nLogSize = 0;
	initflag++;
}
void uinit_log()
{
	if (0 == initflag)
	{
		return;
	}
	sys_EnterCriticalSection(m_hLogSection);
	CloseLogFile();
	sys_LeaveCriticalSection(m_hLogSection);

	sys_DeleteCriticalSection(m_hLogSection);
	initflag--;
}

EXPORT_C void WINAPI trace(int LogLevel, const char* file, int line, const char* fun, const char *pszFormat, ...)
{
    if (LogLevel > LEVEL_CRIT)
    {
        LogLevel = LEVEL_CRIT;
    }

    if (LogLevel < LEVEL_INFO1)
    {
        LogLevel = LEVEL_INFO1;
    }

    char szBuf[LOG_MAX_BUFF*2] = {0};
	SYS_TIME sysTime;
	char newName[LOG_MAX_BUFF] = {0};
    va_list args;

    va_start(args, pszFormat); 
    int nWrite = _vsnprintf_s(szBuf, LOG_MAX_BUFF - 1, pszFormat, args);
	va_end (args);    
	char szOutputMsg[LOG_MAX_BUFF] = {0};
	int nLogLen = _snprintf_s(szOutputMsg, LOG_MAX_BUFF, "%s %s %s %d %s\n", get_time_str(), 
		g_strLogLevel[LogLevel], fun, line, szBuf);
#ifdef _DEBUG__
    sys_OutputDebugString(szOutputMsg);//影响效率
#endif
	 sys_EnterCriticalSection(m_hLogSection);
    if(!m_sfpLog.is_open())//if (!g_fpLog)
    {
		m_sfpLog.open(LOG_NAME,std::ios::app);

		if(!m_sfpLog.is_open()) //(!m_fpLog)
		{
			sys_LeaveCriticalSection(m_hLogSection);
			return;
		}
		m_sfpLog.seekp(0,std::ios::end);    //定位到文件末尾
		m_nLogSize = (long)m_sfpLog.tellp();

    }
	else if (MAX_FILE_LEN < m_nLogSize)
	{
		m_sfpLog.close();
		sys_GetLocalTime(&sysTime);
		sprintf_s(newName,"log\\%04d%02d%02d%02d%02d%02d.log", sysTime.wYear, sysTime.wMonth, 
			sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		remove(newName);
		rename(LOG_NAME, newName);
		m_sfpLog.open(LOG_NAME,std::ios::out);
		m_nLogSize = 0;
		if(!m_sfpLog.is_open())
		{
			sys_LeaveCriticalSection(m_hLogSection);
			return;
		}

	}
    if (nWrite >= 0)
    {
        szOutputMsg[LOG_MAX_BUFF - 1] = '\0';
    }
    else
    {
		nLogLen = _snprintf_s(szOutputMsg, LOG_MAX_BUFF, "%s %s %s %d %s\n", get_time_str(), 
			g_strLogLevel[LogLevel], fun, line, "Too long message, cannot output!\n");
		szOutputMsg[LOG_MAX_BUFF - 1] = '\0';
    }
	m_sfpLog.write(szOutputMsg,strlen(szOutputMsg));
	m_nLogSize += nLogLen;
	m_sfpLog.flush();
	sys_LeaveCriticalSection(m_hLogSection);

}