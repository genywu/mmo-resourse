// UniEditor.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include <fstream>
#include <string>

#define			MAX_TIME	500
using namespace std;

const string strSharp		= _T("#\t");
const string strEnd			= _T("<end>");					// 结束标志
const string strSep			= _T("\t");						// 分隔符
const string strLine		= _T("----------------------------------------------------------------------------\n");
const string strMonsterInfoHead = _T("	名字	几率	标识	领导	距离	路径标识	脚本	对话脚本		\n");
const string strCollInfoHead = _T("	名字		几率		\n");
const string strMonsterHead = _T("	编号	坐标范围(左X1\\Y1\\右X2\\Y2)	数量	时间	起始时	方向\n");
const string strNPCHead		= _T("	坐标范围(左X1\\Y1\\右X1\\Y1)	原始名字			图片	数量	方向	脚本\n");
const string strLogFile		= _T("");
const string strXMLHead		=_T("<?xml version=\"1.0\" encoding=\"gb2312\" standalone=\"no\" ?>\n <RoadPoint>\n");
const string strXMLTail		=_T("\n</Region>\n</RoadPoint>");
// CUniEditorApp:
// 有关此类的实现,请参阅 UniEditor.cpp
//

class CUniEditorApp : public CWinApp
{
public:
	CUniEditorApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
public:
	static BOOL ReadTo(const string &strReadTo, ifstream &ifStream);

	DECLARE_MESSAGE_MAP()
};

extern CUniEditorApp theApp;