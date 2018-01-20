#pragma once

#include "math.h"
#include "atltypes.h"
#include "string.h"

#include "common.h"

#define MATH_PI 3.1415926535

using namespace std;
// 定义坐标系，原点+方向
typedef struct FuncCoordinate
{
	double  OriginalX;
	double  OriginalY;
	double  OriginalA;
	FuncCoordinate()
	{
		OriginalX = 0.0;
		OriginalY = 0.0;
		OriginalA = 0.0;
	}
	void operator=(const FuncCoordinate& sourse)
	{ 
		OriginalX = sourse.OriginalX;
		OriginalY = sourse.OriginalY;
		OriginalA = sourse.OriginalA;
	}


}FUNCCOORDINATE,*LPFUNCCOORDINATE;


class  MyFunClass
{

public:
	 static double CalDistance(double x,double y)
	 {
		 return sqrt((x*x)+(y*y));
	 }


// 计算像素的实际大小。
  static double CalPixSize(double(*MotorDataX),double(*MotorDataY),double (*CamDataCol), double (*CamDataRow),int DataNum)
	{
		if (DataNum<2)
		{
			LOG_WARN("计算像素机械距离时小于2个点");
			return 0.0;
		}
		double DistanceMotor = 0.0;
		double DistancePix = 0.0;

		double PixSize = 0;
		int CalNum = 0;
		for (int i = 0;i<DataNum;i++)
		{
			for (int j = i+1;j<DataNum;j++)
			{
				DistanceMotor = CalDistance((MotorDataX[j]-MotorDataX[i]),MotorDataY[j] - MotorDataY[i]);
				DistancePix = CalDistance((CamDataCol[j]-CamDataCol[i]),CamDataRow[j] - CamDataRow[i]);
				if (DistanceMotor<2)  //机械坐标变化<1mm或像素坐标变化小于3个像素，不计算
				{
					LOG_WARN("两个机械坐标距离小于2个单位");
					//continue;
				}
				if (DistancePix<10)
				{
					LOG_WARN("两个像素坐标距离小于10个单位");
				}
				if (DistancePix == 0)
				{
					LOG_WARN("两个像素坐标距离等于等于0");
					continue;
				}

				PixSize += DistanceMotor/DistancePix;	 
				CalNum++;
			}
		 }
		if (CalNum == 0) //分母不能为0
		{
			return 0.0;
		}
		return PixSize/CalNum;
	}

	//根据两点计算连接的直线的系数。
  static void Point2ToLine( const double x1,const double y1,const double x2,const double y2,double& K,double& phi,double& B)
  {
	  K = (y2-y1)/(x2-x1);
	  phi = atan(K);
	  B =y1 - x1*K;

  }

  //求直线交点
  static void GetInsectionPoint(const double K1,const double B1 ,const double K2,const double B2,double& x,double& y)
  {
	  x = (B2 - B1)/(K1 - K2);   //列或X坐标（非行）
	  y = K1*x + B1;             //行或Y坐标（非列）   

  }

  //根据两点和旋转角度得到旋转中心,为方便使用，旋转角度单位为度。
 static void CalRotCenterTool(double& x1,double& y1,double x2,double y2,double rotPhi)
  {
	  double LineK,LinePhi,LineB;
	  double LineK1,LineB1,LineK2,LineB2;
	  double addPhi = 0.0;
	  Point2ToLine( x1, y1,  x2,  y2, LineK,LinePhi,LineB);

	  //rotAng = rotAng*MATH_PI/180;
	  addPhi = rotPhi<0?(-rotPhi/2 - MATH_PI/2):(MATH_PI/2-rotPhi/2);


	  LineK1 = tan(LinePhi+addPhi);
	  LineK2 = tan(LinePhi-addPhi);
	  
	  LineB1 = y1 - LineK1*x1;
	  LineB2 = y2 - LineK2*x2;

	  GetInsectionPoint(LineK1,LineB1,LineK2,LineB2,x1,y1);
  }

 static void TransMetrixData( double PreSoData, double soMetix,double&  DisMetix)
 {
	 DisMetix = soMetix+PreSoData;

 }

 //由于0-1-1 变成 0-1-2
 static void ParshRelativeToObsolute(double (*RelativeMetric),double (*ObsoluteMetric),int MetricNum)
 {

	 ObsoluteMetric[0] = RelativeMetric[0];
	 for (int i = 1 ;i < MetricNum;i++)
	 {
		 ObsoluteMetric[i] =ObsoluteMetric[i-1]+ RelativeMetric[i];
	 }

 }

  

};




