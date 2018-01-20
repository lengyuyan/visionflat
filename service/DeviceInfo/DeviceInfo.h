// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DEVICEINFO_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DEVICEINFO_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DEVICEINFO_EXPORTS
#define DEVICEINFO_API __declspec(dllexport)
#else
#define DEVICEINFO_API __declspec(dllimport)
#endif


enum    {DES_ENCRYPT,DES_DECRYPT};

//取得12位的关于CPU信息！便于加密！
DEVICEINFO_API int GetCpuInfo(char *szCpuInfo);

//取得12位的关于CPU4种情况的所有信息！便于加密！
DEVICEINFO_API int GetCpuALLInfo(char *szCpuInfo);

//取得硬盘的硬盘序列号！
DEVICEINFO_API int GetHDInfo(char *szHDInfo);

//取得网卡 MAC 地址，注意: MAC 地址是可以在注册表中修改的！
DEVICEINFO_API int GetMACInfo(char *szMACInfo);

//取得bios主板的信息,有可能取不到
DEVICEINFO_API int GetBISOInfo(char *szBIOSDnfo);

//取得语音卡的信息
DEVICEINFO_API int GetCARDInfo(char *szCARDnfo);

/****************************************************
*Function: 加/解密字符
*Intput:  In         输入字符串
          datalen    输入长度
          Key        密钥
          keylen     密钥长度
          Type       加密/解密
*Output: Out
*Return: 无
******************************************************/
DEVICEINFO_API bool Execution_3DES(char *Out, char *In, long datalen,const char *Key, int keylen, bool Type);

/****************************************************
*Function: 加/解密字符
*Intput:  In         输入字符串
          datalen    输入长度
*Output:  Out        MD5计算后的数据
*Return: 无
******************************************************/
DEVICEINFO_API void Execution_MD5(char *Out,const char *In, long datalen);

/****************************************************
*Function: 获取加密后的字符串
*Intput:  
          
*Output:  pData  ： 传出值 [加密后buffer大小为32个字符]
*Return: 无
******************************************************/
DEVICEINFO_API bool GetLicenseBuf(char *pStrMd5, int iLength);