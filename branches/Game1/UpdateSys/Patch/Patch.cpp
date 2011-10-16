// Patch.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Patch.h"
#include "PatchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPatchApp

BEGIN_MESSAGE_MAP(CPatchApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPatchApp 构造

CPatchApp::CPatchApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	CoUninitialize();
}


// 唯一的一个 CPatchApp 对象

CPatchApp theApp;


// CPatchApp 初始化

BOOL CPatchApp::InitInstance()
{
	// 如果更新程序已经打开，则关闭自己
	HANDLE hM = CreateMutex(NULL,TRUE,"CSUpdate");
	if( GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}
	HWND hWnd = FindWindow("《星尘传说》",NULL);
	if (hWnd != NULL)
	{
		AfxMessageBox(_T("你正在运行游戏程序，请先关闭游戏程序，再运行更新程序！"));
		return FALSE;
	}

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	CoInitialize(NULL);

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CPatchDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。

	if (hM) 
	{
		ReleaseMutex(hM);
		CloseHandle(hM);
	}

	return FALSE;
}
