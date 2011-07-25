#include "Precompiled.h"
#include "GameStation.h"

VOID CALLBACK RenderTimer(HWND, UINT, UINT_PTR, DWORD)
{
    if (g_pCGameStation)
    {
        g_pCGameStation->Render();
    }
}

BOOL MSCALL RrGameClientHookProc(MSG* pMsg)
{
    USE_PARAM(pMsg);
    if (g_pCGameStation)
    {
        return g_pCGameStation->GameStationWndProc(
            pMsg->hwnd,
            pMsg->message,
            pMsg->wParam,
            pMsg->lParam
            );
    }
    return FALSE;
}

LRESULT CALLBACK RrGameClientWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _ENTER_FUN_C;

    switch (uMsg)
    {
        // 创建窗口
    case WM_CREATE:
        {
            ::SetTimer(hWnd, 0, 50, RenderTimer);
            DWORD dwCurrentThreadId = ::GetCurrentThreadId();
            g_pCGameStation = NEW CGameStation(hWnd, dwCurrentThreadId);
            g_pCGameStation->Init();
            return TRUE;
        }break;

    case WM_ACTIVATEAPP:
        {
            if (wParam)
            {
                ::KillTimer(hWnd,0);
                ::SetTimer(hWnd, 0, 50, RenderTimer);

            }else
            {
                ::KillTimer(hWnd,0);
                ::SetTimer(hWnd, 0, 500, RenderTimer);
            }
        }break;

    case WM_ERASEBKGND:
        {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }break;

    case WM_RBUTTONUP:
        {
            lpCMsServerSocket lpServerSocket = g_pCGameStation->GetServerSocket();
            if (lpServerSocket)
            {
                C2S_Ask_GameTypeList::MS_PACKET_DATA PacketData;
                PacketData.m_dwGameTypeIndex = 0;
                lpServerSocket->WritePacket(BPT(C2S_Ask_GameTypeList), &PacketData, sizeof(PacketData));
            }
        }break;

        // 关闭窗口
    case WM_CLOSE:
        {
            lpCMsServerSocket lpServerSocket = g_pCGameStation->GetServerSocket();
            if (lpServerSocket)
            {
                C2S_ExitServer::MS_PACKET_DATA PacketData;
                _tcscpy_s(PacketData.m_szShutDownPassword, _T("abcdefghijklmn"));
                lpServerSocket->WritePacket(BPT(C2S_ExitServer), &PacketData, sizeof(PacketData));
            }
            ::SleepEx(100, TRUE);
            ::KillTimer(hWnd, 0);
            ::PostQuitMessage(0);
            g_pCGameStation->Realese();
            SAFE_DELETE(g_pCGameStation);
        }break;

        // 默认处理
    default:
        {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }break;
    }

    return 0;
    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_NID;
}
