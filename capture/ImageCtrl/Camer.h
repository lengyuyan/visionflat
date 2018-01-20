#pragma once
#include "stdlib.h"
//#include "AVTCam.h" 
#include "ICamera.h"
#include <map>
using namespace std;
class Camer
{
public:
	Camer( const char *pszCamerName,string &type);
	~Camer();
	int open();
	int close();
	string GetCamFullName();
	int getImage(void *pImage);
	int getImageSize(int &w, int &h);
    int SetSetGain(unsigned int GainValue);
    int GetSetGain();
    int SetExposureTime(unsigned int ExpTimeValue);
    int GetExposureTime();
	//int GetParameInfo(FG_PARAMETER speciPara,unsigned long &info);
	//int SetParameInfo(FG_PARAMETER speciPara,int val);
	int GetParameInfo(ICAMERA_PARAMETER speciPara,unsigned long &info);
	int SetParameInfo(ICAMERA_PARAMETER speciPara,int val);
	int StartSnap(void *pImage,void(*Callback)(void* context),void *pParent);
	int StopSnap();
	bool IsBindRealCam();


	static int  GetGUIDList(list<string> &guidlist,string &type);
	static map<string ,HMODULE>m_maphdll;
	static int m_camnun;
protected:
private:
	char m_szCamerName[256];
	CRITICAL_SECTION m_CriticalSection;
	ICamera *m_cam;//µ×²ãÏà»úÊµÀý
	HMODULE hdll;
	bool m_bisopen;
};

