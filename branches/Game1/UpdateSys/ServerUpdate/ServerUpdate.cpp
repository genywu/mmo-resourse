// ServerUpdate.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ServerUpdate.h"
#include "ServerUpdateDlg.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//输出调试信息
void PutDebugString(const char *msg)
{
	AfxMessageBox(msg);
}
// CServerUpdateApp

BEGIN_MESSAGE_MAP(CServerUpdateApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServerUpdateApp 构造

CServerUpdateApp::CServerUpdateApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CServerUpdateApp 对象

CServerUpdateApp theApp;

// CServerUpdateApp 初始化

BOOL CServerUpdateApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
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

	bool bExceRun = false;
	string strExeWinTitle("");
	string strCurDirectory("");
	string strServerAddress("");
	string strServerFolder("");
	string strExceFileName("");

	CHAR* strPath = new CHAR[MAX_PATH];
	if(::GetCurrentDirectory(MAX_PATH,strPath)==false)
		return false;
	strCurDirectory=string(strPath);
	delete []strPath;

	string strTemp;
	ifstream stream;
	stream.open("UpdateSetup.ini");
	if( stream.is_open() )
	{
		stream	>> strTemp >> strServerAddress
			>> strTemp >> strServerFolder
			>> strTemp >> strExceFileName
			>> strTemp >> strExeWinTitle;
		stream.close();
	}
	CServerUpdateDlg dlg(strCurDirectory,strServerAddress,strServerFolder,strExceFileName,strExeWinTitle);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//查执行程序是否在运行中
		HWND hWnd = FindWindow(NULL,strExeWinTitle.c_str());
		if (hWnd != NULL)
		{
			bExceRun = true;
		}

		if(bExceRun == false)
		{
			WinExec(strExceFileName.c_str(),SW_SHOWNORMAL);
		}
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

