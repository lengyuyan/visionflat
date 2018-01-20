#pragma once
#ifndef _INCLUDE_VISIONSRV_GENERIC_H
#define _INCLUDE_VISIONSRV_GENERIC_H
#include "sysapi.h" ///包含了handle
#include <hash_map>
#include "vision_api.h"
#include "GolobalError.h"
using stdext::hash_map;

#define INSTANCE CManage::Get_Instance()

class CVision;
struct TVisionInitParam;
class CManage
{
public:
	static CManage* Get_Instance();
    static void ReleaseInstance();

	int InitVision(unsigned short visionID, TVisionInitParam* param);
	int UnInitVision(unsigned short visionID);

	int GetVisionStatus(unsigned short visionID);
	int SendCustomCmd(unsigned short visionID, const char* cmd, const char* msg = NULL);
	int RecvCustomCmd(unsigned short visionID, const char* cmd, char* recvMsg = 0, int timeout = 0);
	int VisionProcess(unsigned short visionID, int secne);
	int RecvProcessResult(unsigned short visionID, int secne, TVisionResult* pres, int timeout);
	int GetPprocessImage(unsigned short visionID, int secne, char* pdata);
	int GetPprocessImagePath(unsigned short visionID, int secne, char* path);
	int SendCalibData(unsigned short visionID, int secne, double x, double y);
	int RecvCalibData(unsigned short visionID, int secne, double x, double y);
	int ConvertCalibData(unsigned short visionID, int secne, TVisionResult* pres);


protected:
	CVision*  GetVision(unsigned short visionID);
private:
	CManage(void);
	~CManage(void);

	static CManage *m_pInstance;

	hash_map<unsigned short, CVision*>    m_hash_vision;
	Handle              m_hSection; //临界区
};


#endif