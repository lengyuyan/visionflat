// DeviceInfo.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DeviceInfo.h"
#include "cpuid.h"
#include "GetOtherInfo.h"


DEVICEINFO_API int GetCpuALLInfo(char *szCpuInfo)
{
    int num = 0;
    for (int i=0;i<5;i++)
    {
        char buf[1024];
        int count = GetEXAKEYCpu(i,buf);
        CopyMemory( szCpuInfo + num, buf, count);
        num += count;
    }
    return num;
}
//取得12位的关于CPU信息！便于加密！
DEVICEINFO_API int GetCpuInfo(char *szCpuInfo)
{
    WORD index = wincpuidext();
    DWORD features = wincpufeatures();	
    sprintf(szCpuInfo,"%.4x%.8x",index,features);
    return 12;

}
//取得硬盘的硬盘序列号！同时，过滤掉空格键！
DEVICEINFO_API int GetHDInfo(char *szHDInfo)
{
    BYTE szSystemInfo[4096]; 
    UINT uSystemInfoLen = 0;
    {
        OSVERSIONINFO ovi = { 0 };
        ovi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
        GetVersionEx( &ovi );

        if( ovi.dwPlatformId != VER_PLATFORM_WIN32_NT )
        {
            // Only Windows 2000, Windows XP, Windows Server 2003...
            //return FALSE;
        }
        else
        {
            if( !WinNTHDSerialNumAsPhysicalRead( szSystemInfo, &uSystemInfoLen, 1024 ) )
            {
                WinNTHDSerialNumAsScsiRead( szSystemInfo, &uSystemInfoLen, 1024 );
            }
        }

    }	
    int count = 0;
    for (int i=uSystemInfoLen-1;i>0;i--)
    {
        if (szSystemInfo[i]!=' ')
        {
            szHDInfo[count++] = szSystemInfo[i];
        }
    }	
    szHDInfo[count]= '\0';	
    return count;
}
//取得网卡 MAC 地址，注意: MAC 地址是可以在注册表中修改的！
//int GetMACInfo(char *szMACInfo)
//{
//    NCB ncb;
//    UCHAR uRetCode;
//    LANA_ENUM lana_enum;
//    memset( &ncb, 0, sizeof(ncb) );
//    ncb.ncb_command = NCBENUM;
//    ncb.ncb_buffer = (unsigned char *) &lana_enum;
//    ncb.ncb_length = sizeof(lana_enum);
//    // 向网卡发送NCBENUM命令,以获取当前机器的网卡信息,如有多少个网卡、每张网卡的编号等
//    uRetCode = Netbios( &ncb );
//    int number = 0;
//    if ( uRetCode == 0 )
//    {
//        // 对每一张网卡,以其网卡编号为输入编号,获取其MAC地址
//        for ( int i=0; i<lana_enum.length; ++i)
//        {
//            char buf[1024];
//            int count = getmac_one(lana_enum.lana[i],buf);
//            CopyMemory( szMACInfo + number, buf, count );
//            number +=count;
//        }
//    }
//    szMACInfo[number] = '\0';
//    return number;
//}
//取得bios主板的信息,有可能取不到
DEVICEINFO_API int GetBISOInfo(char *szBIOSDnfo)
{
    int uSystemInfoLen = 0;
    SIZE_T ssize; 

    LARGE_INTEGER so; 
    so.LowPart=0x000f0000;
    so.HighPart=0x00000000; 
    ssize=0xffff; 
    wchar_t strPH[30]=L"\\device\\physicalmemory"; 

    DWORD ba=0;

    UNICODE_STRING struniph; 
    struniph.Buffer=(wchar_t *)strPH; 
    struniph.Length=0x2c; 
    struniph.MaximumLength =0x2e; 

    OBJECT_ATTRIBUTES obj_ar; 
    obj_ar.Attributes =64;
    obj_ar.Length =24;
    obj_ar.ObjectName=&struniph;
    obj_ar.RootDirectory=0; 
    obj_ar.SecurityDescriptor=0; 
    obj_ar.SecurityQualityOfService =0; 

    HMODULE hinstLib = LoadLibrary("ntdll.dll"); 
    ZWOS ZWopenS=(ZWOS)GetProcAddress(hinstLib,"ZwOpenSection"); 
    ZWMV ZWmapV=(ZWMV)GetProcAddress(hinstLib,"ZwMapViewOfSection"); 
    ZWUMV ZWunmapV=(ZWUMV)GetProcAddress(hinstLib,"ZwUnmapViewOfSection"); 
    //调用函数，对物理内存进行映射 
    HANDLE hSection; 
    if( 0 == ZWopenS(&hSection,4,&obj_ar))
        if ( 
            0 == ZWmapV( 
            ( HANDLE )hSection,   //打开Section时得到的句柄 
            ( HANDLE )0xFFFFFFFF, //将要映射进程的句柄， 
            &ba,                  //映射的基址 
            0,
            0xFFFF,               //分配的大小 
            &so,                  //物理内存的地址 
            &ssize,               //指向读取内存块大小的指针 
            1,                    //子进程的可继承性设定 
            0,                    //分配类型 
            2                     //保护类型 
            ) )
            //执行后会在当前进程的空间开辟一段64k的空间，并把f000:0000到f000:ffff处的内容映射到这里 
            //映射的基址由ba返回,如果映射不再有用,应该用ZwUnmapViewOfSection断开映射 
        {		
            BYTE* pBiosSerial = ( BYTE* )ba;
            UINT uBiosSerialLen = FindAwardBios( &pBiosSerial );
            if( uBiosSerialLen == 0U )
            {
                uBiosSerialLen = FindAmiBios( &pBiosSerial );
                if( uBiosSerialLen == 0U )
                {
                    uBiosSerialLen = FindPhoenixBios( &pBiosSerial );
                }
            }
            if( uBiosSerialLen != 0U )
            {
                CopyMemory( szBIOSDnfo + uSystemInfoLen, pBiosSerial, uBiosSerialLen );
                uSystemInfoLen += uBiosSerialLen;
            }
            ZWunmapV( ( HANDLE )0xFFFFFFFF, ( void* )ba );
        }
        szBIOSDnfo[uSystemInfoLen] = '\0';
        return uSystemInfoLen;
}
////取得语音卡的信息
//int GetCARDInfo(char *szCARDnfo)
//{
//    DWORD  serailno;
//    int flag = GetPciBoardSerailNo(0,&serailno);
//    if (flag==0)
//    {
//        if (serailno!=0)		
//        {
//            sprintf(szCARDnfo,"%.8x",serailno);
//            return 8;
//        }
//    }
//    return 0;
//}

DEVICEINFO_API  bool GetLicenseBuf(char *pStrMd5, int iLength)
{
    bool bRet = false;
    char skey[16] = {"TestDES"};  //密钥
    char *pDesEncrypt = NULL;   //加密前内存
    char *pDesDecrypt = NULL;   //解密后内存
    //char szMd5[33] = {0};       //转换md5后数据
    char szMd5FildData[33] = {0};       //文件中的md5数据

    char szMID[1024] = {0};    //存储硬件内存数据 
    do 
    {
        if (NULL == pStrMd5)
        {
            break;
        }
        //Step 1 读取MID
        GetCpuInfo(pStrMd5);
        GetHDInfo(pStrMd5+strlen(pStrMd5));
        GetBISOInfo(pStrMd5+strlen(pStrMd5));

        //防止越界
        if (iLength < strlen(pStrMd5))
        {
            memset(pStrMd5, 0, iLength);
            break;
        }
        
        //Step 2 加密处理
        int iLength = (strlen(szMID)+7)&0xfffffff8;
        pDesEncrypt = new char[iLength + 1];
        if (NULL == pDesEncrypt)
        {
            break;
        }
        memset(pDesEncrypt, 0, iLength+1);
        strcpy(pDesEncrypt, szMID);

        pDesDecrypt = new char[iLength + 1];
        if (NULL == pDesDecrypt)
        {
            break;
        }
        memset(pDesEncrypt, 0, iLength+1);
        if (!Execution_3DES(pDesDecrypt, pDesEncrypt, iLength,skey, strlen(skey), DES_DECRYPT))
        {
            break;
        }
        delete pDesEncrypt;
        pDesEncrypt = NULL;

        //计算md5值
        Execution_MD5(pStrMd5, pDesDecrypt, iLength);
        delete pDesDecrypt;
        pDesDecrypt = NULL;

        bRet = true;
    } while (0);
    return bRet;
}