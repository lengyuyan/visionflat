#pragma once
#include "resource.h"
#include "afxwin.h"
#include "FuncDefine.h"

// ScenePositionSettin 对话框

class ScenePositionSettin : public CDialogEx
{
	DECLARE_DYNAMIC(ScenePositionSettin)

public:
	ScenePositionSettin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ScenePositionSettin();

// 对话框数据
	enum { IDD = IDD_SCENE_POSITION };
	enum coordinateType
	{
		Point_To_Point =0 ,
		Point_To_Line =1,
		Line_To_Line = 2,

	};
	struct DoublePoint
	{
		double x;
		double y;
		DoublePoint()
		{
			x = 0.0;
			y = 0.0;
		}
		DoublePoint(double InX,double InY)
		{
			x = InX;
			y = InY;
		}
		void operator=(const DoublePoint& src)
		{
			x = src.x;
			y = src.y;

		}

	};


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnBnClickedScenepositionCommit();
	void CloseSettingDlg();


	CEdit m_OriginalX;
	CEdit m_OriginalY;
	CEdit m_OriginalA;
	CComboBox m_CoordinateType;
	CComboBox m_CMatchSel;

	void UpdateUI();
	void GetContainer(void* Pcontainer);
	void SetDisplayWindowID(int winID);
	void SetCoordinate();
	
	void ReadProcConfig(CString ConfigPath,int ProcIndex);
	void SaveProcConfig(CString ConfigPath,int ProcIndex);
	BOOL VisionProcRun( CHECK_RESULT& ProcRes);

public:
	int m_iCurSceneID;
	int m_iCurIndex;
	int m_winID;
	int m_MatchIndex;

	coordinateType m_CoordType;

	int m_ProcProcess ;
	void* m_Pcontainer;
	

	CArray<DoublePoint> m_CoordinatePoint; ////建立坐标系就是对这个成员的元素刷新。

	//存放实时的坐标信息，每定位一次都会刷新一次。
	FUNCCOORDINATE m_Coodinate;
	//vector<int> m_ShapeIdList;  //基本图元的ID，用在一些需要的场合。

	afx_msg void OnBnClickedScenedlgPositionSearchcontour();
	afx_msg void OnCbnSelchangeScenepositionCoordinateway();
	afx_msg void OnBnClickedScenepositionCancel();
	afx_msg void OnCbnDropdownScenepositionSelmatch();
	afx_msg void OnCbnSelchangeScenepositionSelmatch();

};
