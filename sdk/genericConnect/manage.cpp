#include "common.h"
#include "sysapi.h"
#include "vision_api.h"
#include "manage.h"
#include "cvision.h"
#include "skt.h"

using std::pair;

CManage* CManage:: m_pInstance = NULL;
CManage::CManage(void)
{
    skt_init();
	m_hSection = sys_InitializeCriticalSection();
}

CManage::~CManage(void)
{
    skt_uninit();
    //sys_EnterCriticalSection(m_hSection);
	sys_DeleteCriticalSection(m_hSection);
}

CManage* CManage::Get_Instance()
{
	try
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new CManage;
		}
	}
	catch (...)
	{
	}
	return m_pInstance;
}

void CManage::ReleaseInstance()
{
    FREE_ANY(m_pInstance);
}

int CManage::InitVision(unsigned short visionID, TVisionInitParam* param)
{
    int ret = 0;
    CVision *pVision = NULL;

	RETURN_CHK(param, ERR_INPUT_PARAM);
    if (*param->data[0])
    {
        RETURN_CHK(chk_ipaddress(param->data[0]), ERR_INPUT_PARAM);
    }
    RETURN_CHK(EMSTR_INT == get_str_type(param->data[1]), ERR_INPUT_PARAM);
    RETURN_CHK(1 == strlen(param->data[2]) &&
        ('s' == param->data[2][0] ||
        'c' == param->data[2][0] ||
        'S' == param->data[2][0] ||
        'C' == param->data[2][0]), ERR_INPUT_PARAM);

	pVision = GetVision(visionID);

	RETURN_CHK(!pVision, ERR_REPEAT_INIT);

    pVision = new CVision();
    //传进来的参数依次为服务器ip port 客户端ip port 本机是否是客户端
    ret = pVision->Init(param->data[0], atoi(param->data[1]), 
        ('s' == param->data[2][0] ||'S' == param->data[2][0]) ? 0 : 1);
	if (!ret)
	{//success
		sys_EnterCriticalSection(m_hSection);
		m_hash_vision.insert(pair<unsigned short, CVision*>(visionID, pVision));
		sys_LeaveCriticalSection(m_hSection);
	}
	else
    {//fail则释放
        LOG_ERR("InitVision Init(%s,%s,%s,%s,%s) Fail...", param->data[0], param->data[1], param->data[2], param->data[3], param->data[4]);
		delete pVision;
	}
	return ret;
}

int CManage::UnInitVision(unsigned short visionID)
{
	hash_map<unsigned short, CVision*>::iterator itVison;

	sys_EnterCriticalSection(m_hSection);
	itVison = m_hash_vision.find(visionID);
	if (itVison == m_hash_vision.end())
	{
        sys_LeaveCriticalSection(m_hSection);
        return ERR_INVALID_ID;
	}
    delete itVison->second;
    m_hash_vision.erase(itVison);
	sys_LeaveCriticalSection(m_hSection);
	return 0;
}

CVision* CManage::GetVision(unsigned short visionID)
{
	CVision* pVision = NULL;
	hash_map<unsigned short, CVision*>::iterator itVison;

	sys_EnterCriticalSection(m_hSection);
	itVison = m_hash_vision.find(visionID);
	if (itVison != m_hash_vision.end())
	{
		pVision = itVison->second;
	}
	sys_LeaveCriticalSection(m_hSection);
	return pVision;
}

int CManage::GetVisionStatus(unsigned short visionID)
{	
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, EMVISION_NOINIT);

	return pVision->GetStatus();
}

int CManage::SendCustomCmd(unsigned short visionID, const char* cmd,  const char* msg)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, ERR_INPUT_PARAM);

	return pVision->SendCustomMsg(cmd, msg);
}

int CManage::RecvCustomCmd(unsigned short visionID, const char* cmd, char* recvMsg , int timeout )
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, ERR_INPUT_PARAM);

	return pVision->RecvCustomMsg(cmd, recvMsg, timeout);
}

int CManage::VisionProcess(unsigned short visionID, int secne)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, ERR_INPUT_PARAM);

	return pVision->VisionProcess(secne);

}
int CManage::RecvProcessResult(unsigned short visionID, int secne, TVisionResult* pres, int timeout)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision && pres, ERR_INPUT_PARAM);

	return pVision->VisionProcessResult(secne,pres,timeout);
}

int CManage::GetPprocessImage(unsigned short visionID, int secne, char* pdata)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, ERR_INPUT_PARAM);

	return pVision->GetVisionImage(secne,pdata);
}

int CManage::GetPprocessImagePath(unsigned short visionID, int secne, char* path)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision && path, ERR_INPUT_PARAM);

	return pVision->GetVisionImagePath(secne,path);
}

int CManage::SendCalibData(unsigned short visionID, int secne, double x, double y)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, ERR_INPUT_PARAM);

	return pVision->SendVisionCalibData(secne,x, y);

}
int CManage::RecvCalibData(unsigned short visionID, int secne, double x, double y)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, ERR_INPUT_PARAM);
	
	return pVision->RecvVisionCalibData(secne,x, y);
}

int CManage::ConvertCalibData(unsigned short visionID, int secne, TVisionResult* pres)
{
	CVision* pVision = GetVision(visionID);

	RETURN_CHK(pVision, ERR_INPUT_PARAM);

	return pVision->ConvertCalibData(secne, secne, pres);
}
