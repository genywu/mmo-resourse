/**
============================================================
*  Copyright (c) 2009
*  All rights reserved
*  文件名称：GameTest.cpp
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

#include "stdafx.h"
#include "GameTest.h"
#include "GameTestDlg.h"
//#include "vld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CGameTestApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CGameTestApp::CGameTestApp()
{

	//_CrtSetBreakAlloc();
	//EnableDebugNew();
}

CGameTestApp theApp;

BOOL CGameTestApp::InitInstance()
{
	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CGameTestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	return FALSE;
}
