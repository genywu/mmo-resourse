#include "Precompiled.h"
#include "FileClient.h"
#include "FileServer.h"

VOID ClientLogic(VOID)
{
    TCHAR szServerIP[IP_SIZE] = {0};
    TCHAR szPassword[MAX_PATH] = {0};
    WORD wProt = 0;
    _tcout << _T("请输入服务器IP:\n");
    _tcin >> szServerIP;
    _tcout << _T("请输入网络端口:\n");
    _tcin >> wProt;
    _tcout << _T("请输入连接密码:\n");
    _tcin >> szPassword;
    g_pCFileClient = NEW CFileClient(szServerIP, wProt, szPassword);
    AssertEx(g_pCFileClient, _T("内存不足"));
    g_pCFileClient->Run();
}


VOID ServerLogic(VOID)
{
    WORD wProt = 0;
    DWORD dwMaxClient = 0;
    TCHAR szPassword[MAX_PATH] = {0};
    _tcout << _T("请输入网络端口:\n");
    _tcin >> wProt;
    _tcout << _T("请输入允许最大连接数:\n");
    _tcin >> dwMaxClient;
    _tcout << _T("请输入连接密码:\n");
    _tcin >> szPassword;
    if (wProt && dwMaxClient)
    {
        g_pCFileServer = NEW CFileServer(wProt, dwMaxClient, szPassword);
        AssertEx(g_pCFileServer, _T("内存不足"));
        g_pCFileServer->Run();
    }
    else
    {
        _tcout << _T("输入无效:\n");
    }
}

INT MSCALL Exe_FileTransfers_Main(INT,LPTCHAR*)
{
    BOOL bExit = FALSE;
    INT nIndex = 0;
    while (!bExit)
    {
        _tcout << _T("请选择:\n")
            << _T("1、建立服务器->共享文件\n")
            << _T("2、建立服务器->共享路径\n")
            << _T("3、登陆共享服务器\n")
            << _T("4、退出\n");
        nIndex = _getch();
        switch(nIndex)
        {
        case _T('1'):
            {
                ServerLogic();
            }break;
        case _T('2'):
            {
                _tcout << _T("暂不支持\n");
            }break;
        case _T('3'):
            {
                ClientLogic();
            }break;
        case _T('4'):
            {
                bExit = TRUE;
            }break;
        default:
            {
                _tcout << _T("选择无效\n");
            }break;
        }
        _tsystem(_T("Pause"));
        _tsystem(_T("Cls"));
    }

    SAFE_DELETE(g_pCFileServer);
    SAFE_DELETE(g_pCFileClient);
    return 0;
}
