// PackageTool.h : PackageTool 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号


// CPackageToolApp:
// 有关此类的实现，请参阅 PackageTool.cpp
//

class CPackageToolApp : public CWinApp
{
public:
	CPackageToolApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CPackageToolApp theApp;
