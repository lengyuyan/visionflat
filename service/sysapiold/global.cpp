#include "sys_internal.h"
#include "sysapi.h"
#include "comm.h"

#pragma comment (lib,"user32.lib") 

void WINAPI utf8_to_gbk(const char* utf8, char* gbk, int datalen)
{
    int len = MultiByteToWideChar(CP_UTF8, 0,utf8, -1, NULL, 0);
    ushort * wszGBK = new ushort[len+1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1,  (LPWSTR)wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0,  (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
    strcpy_s(gbk, datalen, szGBK);
    delete[] szGBK;
    delete[] wszGBK;
}

void WINAPI gbk_to_utf8(const char* gbk, char* utf8, int datalen)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gbk, -1, NULL, 0);
    ushort * wszUtf8 = new ushort[len+1];
    memset(wszUtf8, 0, len * 2 + 2);
    MultiByteToWideChar(CP_ACP, 0, gbk, -1,  (LPWSTR)wszUtf8, len);

    len = WideCharToMultiByte(CP_UTF8, 0,  (LPWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
    char *szUtf8=new char[len + 1];
    memset(szUtf8, 0, len + 1);
    WideCharToMultiByte (CP_UTF8, 0,  (LPWSTR)wszUtf8, -1, szUtf8, len, NULL, NULL);

    strcpy_s(utf8, datalen, szUtf8);

    delete[] szUtf8;
    delete[] wszUtf8;
}

void WINAPI gbk_to_ascii(const wchar_t* gbk, char* ascii, int datalen)
{
    int len = WideCharToMultiByte(CP_ACP, 0, gbk, -1, NULL, 0, NULL, NULL);
    if (len >= datalen)
    {//error.
        return;
    }
    memset(ascii, 0, datalen);
    WideCharToMultiByte(CP_ACP, 0, gbk, -1, ascii, datalen, NULL, NULL);
}

void WINAPI ascii_to_gbk(const char* ascii, wchar_t* gbk, int datalen)
{
    int len = MultiByteToWideChar(CP_ACP, 0, ascii, -1, NULL, 0);
    if (len >= datalen)
    {//error.
        return;
    }
    memset(gbk, 0, datalen * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, ascii, -1, gbk, datalen);
}

EXPORT_C int WINAPI copy_to_board(const char* pbuff)
{
    HGLOBAL clipbuffer;
    int nLen = 0;
    char *buffer = NULL;

    RETURN_CHK_NOPRT(pbuff && *pbuff, ERR_INPUT_PARAM);

    if(!::OpenClipboard(NULL))
    {
        return ERR_INPUT_PARAM;
    }

    ::EmptyClipboard();
    nLen = (int)strlen(pbuff) + 1;
    clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, nLen);
    buffer = (char*)::GlobalLock(clipbuffer);
    strcpy_s(buffer, nLen, pbuff);
    ::GlobalUnlock(clipbuffer);
    ::SetClipboardData(CF_TEXT, clipbuffer);
    ::CloseClipboard();
    return 0;
}