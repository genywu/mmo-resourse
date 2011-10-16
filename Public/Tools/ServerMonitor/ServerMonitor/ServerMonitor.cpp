// ServerMonitor.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "sm.h"
#include "ServiceControl.h"
#include "CmdLine.h"
void Command();
BOOL CtrlHandler(DWORD dwCtrlType)
{
    switch(dwCtrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
        {
            //LogTools::MsgboxInfo(CStringReading::LoadString(IDS_SM_TIP,IDS_Tip0));
            //return TRUE;
        }
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        {

        }
    }
    g_bSMThreadExit = true;
    WaitForSingleObject(g_hSMThreadExitEvent, INFINITE);
    CloseHandle(g_hSMThreadExitEvent);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, FALSE);
    return FALSE;
}

int main(int argv ,char* argc[])
{
    BOOL bHideConsole = TRUE;
    if(NULL != argc[1])
    {
        if(0 == stricmp("show", argc[1]))
            bHideConsole = FALSE;
    }
    //! 隐藏控制台
    /*if(bHideConsole)
    ;
    FreeConsole();*/
    //Sleep(15000);
    //#ifdef NDEBUG
    //	LogTools::OutLog(CStringReading::LoadString(IDS_SM_TIP,IDS_Tip1));
    //	if(ServiceControl::StartService())
    //	{
    //		char szMsg[256]={0};
    //		sprintf(szMsg,"Windows服务注册成功!服务名:%s",ServiceControl::GetServiceName());
    //		MessageBox(NULL,szMsg,"SM",MB_OK);
    //	}
    //	g_bRunCmdLine=false;
    //#else
    //! 注册事件钩子
    BOOL bRe = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
    if(!bRe)
        LogTools::OutTextToInterface(AppFrame::GetText("IDS_SM_TIP2"));
    HANDLE hmutex = ::CreateMutex(NULL,true,"sm");
    if (hmutex == NULL)
    {
        MessageBox(NULL, "创建互斥锁失败","ServerMonitor" , MB_OK);
        return 0;
    }
    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
        MessageBox(NULL, "SM正在运行中", "ServerMonitor", MB_OK);
        return 0;
    }
    unsigned threadID;
    HANDLE  hSMThread = (HANDLE)_beginthreadex(NULL,0,SMThreadFunc,NULL,0,&threadID);
    g_hSMThreadExitEvent = CreateEvent(NULL,0,0,0);
    CCmdLine::GetInstance()->Init();
    while(1)
    {
        char szCmd[1024]={0};
        cin.getline(szCmd,1024);
        if (0 == strcmp(szCmd,"exit"))
        {
            g_bSMThreadExit = true;
            break;
        }
        else
            CCmdLine::GetInstance()->InsertCmd(szCmd);
        Sleep(10);
    }
    WaitForSingleObject(g_hSMThreadExitEvent, INFINITE);
    CloseHandle(hmutex);
    CloseHandle(g_hSMThreadExitEvent);
    CloseHandle(hSMThread);
    if(bRe)
        SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, FALSE);
    //#endif

    return 0;
}

VOID LogTools::OutTextToInterface(const char *pText)
{
    cout<<pText;
}

