#pragma once
class MyFunction
{
public:
	MyFunction(void);
	~MyFunction(void);

static	int   MyGetSubstrNum(CString str,CString invStr)  ////例子:1_2_3则返回3,标识由3个形状 _1_2_3_也返回3
	{
		int strNum = 0;
		if (str.IsEmpty())
		{
			return strNum ;
		}else
		{
			strNum = 1;
			
		}

		if (invStr == str.GetAt(str.GetLength()-1))  //如果最后面有符号，则去掉
		{
			str.Delete(str.GetLength()-1);
		}
		if (invStr == str.GetAt(0))   //如果最前面有符号，则去掉
		{
			str.Delete(0);
		}


		CString strTemp;
		while (-1 != str.Find(invStr))
		{
			strTemp = str.Left(str.Find(invStr)+1);
			str.Delete(0,strlen(strTemp));
			strNum++;
		}
		return strNum;
	}



//在BaseStr中找到invStr,BeginNum为第几个(从第一个开始),InvNum为找几个,invStr为子字符串,返回为找到的
static	CString MyGetSubStr(CString BaseStr,int BeginNum,int InvNum,CString invStr)   
	{
		CString strTemp,strTemp1;

		if (BaseStr.IsEmpty())
		{
			return  BaseStr;
		}
		if (BeginNum+InvNum-1> MyGetSubstrNum(BaseStr,invStr))
		{
			strTemp.Empty();
			return strTemp;
		}
		int numTemp = 1;
		while (numTemp < BeginNum)//若是第2个删除第一个
		{
			strTemp = BaseStr.Left(BaseStr.Find(invStr)+1);
			int len = strlen(strTemp);
			BaseStr.Delete(0,strlen(strTemp));
			numTemp++;
		}

		numTemp = 0;
		strTemp.Empty();
		while (numTemp < InvNum)
		{

			strTemp1 = BaseStr.Left(BaseStr.Find(invStr)+1);
			if (strTemp1.IsEmpty())
			{
				return strTemp+BaseStr;
			} 
			else
			{
				strTemp+=strTemp1;
			}

			BaseStr.Delete(0,strlen(strTemp1));
			numTemp++;
		}
		return strTemp;

	}


};

