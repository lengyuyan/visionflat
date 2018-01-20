#pragma once
#ifndef FLATFISH_INCLUDE_GLOBALDEFINE_H
#define FLATFISH_INCLUDE_GLOBALDEFINE_H

//type
typedef void    *HANDLE;
typedef void    *Handle;
typedef void    *LPVOID;
typedef int     BOOL;
typedef unsigned char   BYTE;
typedef unsigned int    UINT;
typedef unsigned long   DWORD;
typedef long    LONG;
//typedef long    LONG_PTR, *PLONG_PTR;

typedef char            *LPSTR;
typedef const char      *LPCSTR;
typedef wchar_t         *LPWSTR;
typedef const wchar_t   *LPCWSTR;






//宏定义

#ifndef EXPORT_C
#define EXPORT_C extern "C"
#endif

#ifndef WINAPI
#define WINAPI __stdcall
#endif

#ifndef APIENTRY
#define APIENTRY WINAPI
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef INVALID_HANDLEVALUE
//#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#define INVALID_HANDLEVALUE ((HANDLE)(-1))
#endif

#ifndef INFINITE
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

#ifndef MAXDWORD
#define MAXDWORD    0xffffffff
#endif

#ifndef Max
#define Max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef Min
#define Min(a,b)            (((a) < (b)) ? (a) : (b))
#endif




#define RETURN_CHK(p, v) if (!(p)) \
{\
	return (v); \
}

#define RETURN_CHK_NOPRT(p, v) if (!(p)) \
{\
	return (v); \
}

#define ZERO_CHK(p) if (!(p)) \
{    \
	return; \
}

//安全释放,并置空
#define FREE_ANY(p) if (0 != (p)) \
{\
	delete (p);\
	(p) = NULL;\
}

#endif