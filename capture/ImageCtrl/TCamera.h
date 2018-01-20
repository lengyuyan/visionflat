#include "Camer.h"
#include <string>
#include <list>
#include <hash_map>
#include "iImageCtrl.h"
using namespace std;

class Camer;
class  TCamera
{
public:
		TCamera(void);

        static TCamera* GetInstance();
        static void ReleaseInstance();
		static TCamera *m_this;

		//初始化,连接图像采集卡
		virtual int Init(int dllnum, string dlllist[]); 

		//释放图像采集卡资源
		virtual int Deinit();	

		// 获取相机guid列表
		virtual int GetGUIDList(list<string> &guidlist,string &type);

		//获取相机参数
		virtual int GetParameInfo(string name,CAMARA_PARA_ITEM speciPara,unsigned long &info);

		//设置相机参数
		virtual int SetParameInfo(string name,CAMARA_PARA_ITEM speciPara,int val);

		//打开相机
		virtual int Open (string name );

		//关闭相机
		virtual int Close (string name);

		//获取相机图像大小
		virtual int GetImageSize(string name ,int &w, int &h);

		//获取相机图像
		virtual int GetImage(string name, void *pImage);

		// 设置相机增益
        virtual int SetGain(string name,unsigned int value); 
        virtual int GetGain(string name); 

		// 设置相机曝光时间
        virtual int SetExposureTime(string name,unsigned int value);
        // 获取相机曝光时间
        virtual int GetExposureTime(string name);

		//开始实时取像
		virtual int StartSnap(string name,void *pImage,void(*Callback)(void* context),void *pParent);

		//停止实时取像
		virtual int StopSnap(string name);

private:
	//获取相机对像
	Camer *GetCamer(string name);

private:
	hash_map<string,Camer*> m_hm_camera;
	hash_map<string,Camer*>::iterator m_it_camera;
	int m_iresult;
	CAMARA_ADN_CONNECT_TYPE m_en_camaratype;


};