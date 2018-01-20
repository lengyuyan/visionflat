/*********************************************************************
    *文件说明: 本文件主要封装一些通用函数接口，后面所有通用接口请放在这
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/22 CW0793 创建
                
***********************************************************************/
#include <iomanip>

#include  <io.h>
#include <direct.h>
#include "sysapi.h"
#include "common.h"

char g_timeBuff[32] = {0};

const char* WINAPI get_time_str()
{
	SYS_TIME sysTime;

	sys_GetLocalTime(&sysTime);
	sprintf_s(g_timeBuff, "%04d-%02d-%02d %02d:%02d:%02d:%03d", sysTime.wYear, \
		sysTime.wMonth, sysTime.wDay, sysTime.wHour, \
		sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

	return g_timeBuff;
}
/*
EXPORT_C void WINAPI get_module_path(LPSTR lpPath)
{
    char buffer[MAX_PATH] = {0};
    char* ptmp = NULL;
    sys_GetModuleFileName(NULL,buffer,sizeof(buffer));
    ptmp = strrchr(buffer, '\\');
    if (!ptmp)
    {
        return;
    }

    *(ptmp+1) = '\0';
    strcpy_s(lpPath, MAX_PATH, buffer);
}*/

EXPORT_C bool WINAPI is_file_exist(LPCSTR filepath)
{
	if((_access( filepath, 0 )) != -1)
	{
		return true;
	}
	return false;
}

EXPORT_C bool WINAPI is_file_read(LPCSTR filepath)
{
	if( (_access( filepath, 4 )) != -1  )
	{
		return true;
	}
	return false;
}
/*
EXPORT_C void WINAPI utf8_to_gbk(LPCSTR utf8, LPSTR gbk, int datalen)
{
    int len=sys_MultiByteToWideChar(CP_UTF8, 0,utf8, -1, NULL,0);
    unsigned short * wszGBK = new unsigned short[len+1];
    memset(wszGBK, 0, len * 2 + 2);
    sys_MultiByteToWideChar(CP_UTF8, 0, utf8, -1,  (LPWSTR)wszGBK, len);

    len = sys_WideCharToMultiByte(CP_ACP, 0,  (LPWSTR)wszGBK, -1, NULL, 0);
    char *szGBK=new char[len + 1];
    memset(szGBK, 0, len + 1);
    sys_WideCharToMultiByte (CP_ACP, 0,  (LPWSTR)wszGBK, -1, szGBK, len);
    strcpy_s(gbk, datalen, szGBK);
    delete[] szGBK;
    delete[] wszGBK;
}

EXPORT_C void WINAPI gbk_to_utf8(LPCSTR gbk, LPSTR utf8, int datalen)
{
    int len=sys_MultiByteToWideChar(CP_ACP, 0, gbk, -1, NULL,0);
    unsigned short * wszUtf8 = new unsigned short[len+1];
    memset(wszUtf8, 0, len * 2 + 2);
    sys_MultiByteToWideChar(CP_ACP, 0, gbk, -1,  (LPWSTR)wszUtf8, len);

    len = sys_WideCharToMultiByte(CP_UTF8, 0,  (LPWSTR)wszUtf8, -1, NULL, 0);
    char *szUtf8=new char[len + 1];
    memset(szUtf8, 0, len + 1);
    sys_WideCharToMultiByte (CP_UTF8, 0,  (LPWSTR)wszUtf8, -1, szUtf8, len);

    strcpy_s(utf8, datalen, szUtf8);

    delete[] szUtf8;
    delete[] wszUtf8;
}*/

EXPORT_C void WINAPI create_image_file()
{
	char dirbuffer[256] = {0};
	char RecheckData[256] = {0};
	char rawimagedir[256] = {0};
	char logimagedir[256] = {0};
	char ngimagedir[256] = {0};
	char ngmbimagedir[256] = {0};
	char nggasketimagedir[256] = {0};
	char ngrecheckimagedir[256] = {0};
	SYS_TIME timep;
	sys_GetLocalTime(&timep);
	sprintf_s(dirbuffer,"mypic\\%04d%02d%02d", timep.wYear, timep.wMonth, timep.wDay);
	sprintf_s(RecheckData,"mypic\\%04d%02d%02d\\%04d%02d%02d.csv", timep.wYear, timep.wMonth, timep.wDay, timep.wYear, timep.wMonth, timep.wDay);

	sprintf_s(rawimagedir,"mypic\\%04d%02d%02d\\rawimage", timep.wYear, timep.wMonth, timep.wDay);
	sprintf_s(logimagedir,"mypic\\%04d%02d%02d\\logimage", timep.wYear, timep.wMonth, timep.wDay);
	sprintf_s(ngimagedir,"mypic\\%04d%02d%02d\\ngimage", timep.wYear, timep.wMonth, timep.wDay);
	sprintf_s(ngmbimagedir,"mypic\\%04d%02d%02d\\ngimage\\mb", timep.wYear, timep.wMonth, timep.wDay);
	sprintf_s(nggasketimagedir,"mypic\\%04d%02d%02d\\ngimage\\gasket", timep.wYear, timep.wMonth, timep.wDay);
	sprintf_s(ngrecheckimagedir,"mypic\\%04d%02d%02d\\ngimage\\recheck", timep.wYear, timep.wMonth, timep.wDay);
	_mkdir("mypic");
	_mkdir(dirbuffer);
	_mkdir(rawimagedir); 
	_mkdir(logimagedir);
	_mkdir(ngimagedir);
	_mkdir(ngmbimagedir);
	_mkdir(nggasketimagedir);
	_mkdir(ngrecheckimagedir);
}
EXPORT_C int WINAPI create_path(const char* path)
{
	if (!is_file_exist(path))
	{
		return _mkdir(path);
	}
	return 0;
}
EXPORT_C int WINAPI chk_ipaddress(const char* lpIP)
{
	char numordot;       //字符判断
	int dotnum = 0;      //分隔符数
	int oldcount = 0;
	int nowcount = 0;
	int diffcount = 0;
	int maxmin = 0;
	char seps[4] = {0};  //段保存

	RETURN_CHK(lpIP, 0);
	RETURN_CHK(strlen(lpIP), 0);
	RETURN_CHK(0 != strcmp(lpIP, ANY_IP), 1);

	while (*lpIP != '\0')
	{
		numordot = *lpIP++;
		nowcount++;
		if ((numordot >= '0') && (numordot <= '9'))
		{
			continue;
		}
		else if ('.' == numordot)
		{
			//统计分隔符数
			dotnum++;
			if (dotnum > 3)
			{
				break;
			}
			//根据分隔符来提取段
			diffcount = nowcount - oldcount -1;
			switch(diffcount)
			{
			case 1:  //1位
				seps[0] = *(lpIP-2);
				seps[1] = '\0';
				break;
			case 2:  //2位
				seps[0] = *(lpIP-3);
				seps[1] = *(lpIP-2);
				seps[2] = '\0';
				break;
			case 3:  //3位
				seps[0] = *(lpIP-4);
				seps[1] = *(lpIP-3);
				seps[2] = *(lpIP-2);
				seps[3] = '\0';
				break;
			default:
				break;
			}
			oldcount = nowcount;
			//判断段
			maxmin = atoi(seps);
			RETURN_CHK(maxmin >= 0 && maxmin <= 255, 0);
			continue;
		}
		else
		{
			return 0;
		}
	}
	//判断分隔符数是否正确
	if (3 == dotnum)
	{
		//判断最后一个字段是否正确
		diffcount = nowcount - oldcount;
		switch(diffcount)
		{
		case 1:
			seps[0] = *(lpIP-1);
			seps[1] = '\0';
			break;
		case 2:
			seps[0] = *(lpIP-2);
			seps[1] = *(lpIP-1);
			seps[2] = '\0';
			break;
		case 3:
			seps[0] = *(lpIP-3);
			seps[1] = *(lpIP-2);
			seps[2] = *(lpIP-1);
			seps[3] = '\0';
			break;
		default:
			break;
		}
		maxmin = atoi(seps);
		RETURN_CHK(maxmin >= 0 && maxmin <= 255, 0);
	}
	else
	{
		return 0;
	}

	return 1;
}

EXPORT_C char* WINAPI comm_strcpy(char* src, const char* dest, char ch)
{
	RETURN_CHK_NOPRT(src && dest, NULL);

	while(*dest && *dest != ch)
	{
		*src++ = *dest++;
	}
	*src = 0;
	return src;
}
EXPORT_C int WINAPI get_str_type(const char* pstr)
{
	bool bDot = false;

	RETURN_CHK(pstr, EMSTR_STRING);

	if ('-' == *pstr || '+' == *pstr)
	{//首字母可以为 正负符号
		pstr++;
	}

	RETURN_CHK(*pstr, EMSTR_STRING);  //接下来没有了？不行

	while(*pstr)
	{
		if ('.' == *pstr)
		{//只能出现一次小数点
			if (!bDot)
			{
				bDot = true;
				pstr++;
				continue;
			}
			return EMSTR_STRING;
		}
		if ('0' > *pstr || '9' < *pstr)
		{
			return EMSTR_STRING;
		}
		pstr++;
	}

	return bDot ? EMSTR_DOUBLE : EMSTR_INT;
}
EXPORT_C void WINAPI trim_char(char* pbuff, const char* key)
{
	int index = 0;
	const char* ptmp = key;

	ZERO_CHK(pbuff && key);

	index = (int)strlen(pbuff) - 1;

	ZERO_CHK(index >= 0);

	//删除尾部
	while (index >= 0)
	{
		if (!strchr(key, pbuff[index]))
		{//无特殊字符
			break;
		}
		pbuff[index--] = 0;
	}

	//删除头部
	index = 0;
	while (pbuff[index])
	{
		if (!strchr(key, pbuff[index]))
		{
			break;
		}
		index++;
	}

	ZERO_CHK(index);    //查无特殊字符，退出

	//往前拷贝
	ptmp = &pbuff[index];
	while (*ptmp)
	{
		*pbuff++ = *ptmp++;
	}
	*pbuff = 0;
}
int g_iLastErrCode = 0; //最后的错误

EXPORT_C int WINAPI get_last_err()
{
	return g_iLastErrCode;
}