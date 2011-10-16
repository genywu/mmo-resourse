/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：GameTest.h
*  功能：GametestMFC界面入口函数
*  当前版本：1.1.0
*  作者： 
*  完成日期：
*  取代版本：1.0.0
*  原作者：
*  完成日期：2009.2.2
*
============================================================
*/

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"

class CGameTestApp : public CWinApp
{
public:
	CGameTestApp();
	public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CGameTestApp theApp;
