// stdafx.cpp : 只包括标准包含文件的源文件
// SceneManager.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"



int  g_SceneInitialCount = 0;
long g_SceneDlgHwinId = -1 ;
SceneList  g_SceneList;
SceneList* SceneGetSceneList()
{
	return &g_SceneList;
}

