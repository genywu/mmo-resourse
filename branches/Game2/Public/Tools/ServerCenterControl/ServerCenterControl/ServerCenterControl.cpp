// ServerCenterControl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ServiceControl.h"

//! 响应用户输入
BOOL OnUserCommand(LPCSTR pCommand);
//! 事件处理函数
BOOL CtrlHandler(DWORD dwCtrlType);

//! 主函数
int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bHideConsole = TRUE;
	if(NULL != argv[1])
	{
		if(0 == stricmp("show", argv[1]))
			bHideConsole = FALSE;
	}
	//! 隐藏控制台
	if(bHideConsole)
	{
#ifndef _DEBUG
		//FreeConsole();
#endif
	}

	///////////////////////////////////////////////////////
	//! 启动服务
	{
#ifdef OUT_SERVICE
		if(!ServiceControl::StartService())
		{
			//char szInfo[256] = {0};
			//LogTools::MsgboxErrInfo("启动服务失败!!错误代码%d!!", GetLastError());
		}
#else
		//! 注册事件钩子
		BOOL bRe = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
		if(!bRe)
			LogTools::OutTextToInterface("SetConsoleCtrlHandler() lost\r\n");

		if(!ServiceSCC::GetInstance().Init())
		{
			ServiceSCC::GetInstance().Relese();
			return -1;
		}

		ServiceSCC::GetInstance().Begin();

		Sleep(50);
		LogTools::OutTextToInterface("view help: help, end exec: exit!!!\r\n");
		LogTools::OutTextToInterface("view help: help, end exec: exit!!!\r\n");
		LogTools::OutTextToInterface("view help: help, end exec: exit!!!\r\n");

		while (true)
		{
			string strCommand;
			cin >> strCommand;
			
			if(OnUserCommand(strCommand.c_str()))
				break;
		}

		ServiceSCC::GetInstance().CloseExec();
		ServiceSCC::GetInstance().Relese();

		if(bRe)
			SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, FALSE);

		//system("pause");
#endif
	}
	///////////////////////////////////////////////////////

	
	return 0;
}

BOOL OnUserCommand(LPCSTR pCommand)
{
	if(0 == stricmp(pCommand, "exit"))
	{
		return TRUE;
	}
	else if(0 == stricmp(pCommand, "help"))
	{
		LogTools::OutTextToInterface("\r\n\r\n");
		LogTools::OutTextToInterface("command:\r\n");
		LogTools::OutTextToInterface("show all SM info:			showall\r\n");
		LogTools::OutTextToInterface("update all Gapp:			update\r\n");
		LogTools::OutTextToInterface("turn off all Gapp:		closeup\r\n");
		LogTools::OutTextToInterface("turn on all Gapp:			startup\r\n");
		LogTools::OutTextToInterface("refresh update package:	refreshfile //only used after manually modify the package file.\r\n");
		
		LogTools::OutTextToInterface("\r\n\r\n");
	}
	else if (0 == stricmp(pCommand, "closeup"))
	{
		ServiceSCC::GetInstance().InputCommand(1);
	}
	else if (0 == stricmp(pCommand, "startup"))
	{
		ServiceSCC::GetInstance().InputCommand(2);
	}
	else if (0 == stricmp(pCommand, "refreshfile"))
	{
		ServiceSCC::GetInstance().InputCommand(3);
	}
	else if(0 == stricmp(pCommand, "update"))
	{
		LogTools::OutTextToInterface("update all Gapp!!!\r\nY=Verify Other=Cancel\r\n");
		string strParam1;
		cin >> strParam1;
		if(0 == stricmp(strParam1.c_str(), "y"))
		{
			ServiceSCC::GetInstance().InputCommand(4);
		}
		else
		{
			LogTools::OutTextToInterface("update cancel!\r\n");
		}
	}
	else if (0 == stricmp(pCommand, "showall"))
	{
		
		ServiceSCC::GetInstance().InputCommand(5);
		
	}
	else
	{
#if _DEBUG
		//! 测试用
		try
		{
			DWORD dwCommand = atoi(pCommand);
			if(0 == dwCommand)
				LogTools::OutTextToInterface("unrecognized command!!!");
			else
				ServiceSCC::GetInstance().InputCommand(dwCommand);
		}
		catch (...)
		{
			LogTools::OutTextToInterface("unrecognized command!!!");
		}
#endif
	}

	return FALSE;
}

BOOL CtrlHandler(DWORD dwCtrlType)
{
	switch(dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		//{
		//	LogTools::MsgboxInfo("合法关闭程序请键入“exit”!");
		//	return TRUE;
		//}
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		{
			ServiceSCC::GetInstance().CloseExec();
			ServiceSCC::GetInstance().Relese();
		}
	}

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, FALSE);
	return FALSE;
}



//! 在界面上输出信息
//! 这个方法必须在引用的模块中实现
VOID LogTools::OutTextToInterface(const char *pText)
{
#ifndef OUT_SERVICE
	cout<<pText;
#endif
}


