#include "Precompiled.h"

// DLlGame加载释放通知回调
BOOL WINAPI DllMain(
    HANDLE  hDllHandle,
    DWORD   dwReason,
    LPVOID  lpreserved
    )
{
    USE_PARAM(hDllHandle);
    USE_PARAM(lpreserved);
    switch (dwReason)
    {
        // 如果是进程加载
    case DLL_PROCESS_ATTACH:
        {
            // 启动DllGame启动线程
            RrTetris::gs_hThread = ::CreateThread(NULL, 0, RrTetris::RrTetrisThread, NULL, 0, NULL);
        }break;

        // 如果是进程卸载
    case DLL_PROCESS_DETACH:
        {
            // 释放配置对象
            SAFE_DELETE(RrTetris::gs_pRrTetrisConfig);

            // 释放日志对象
            SAFE_DELETE(RrTetris::gs_pMsLog);

            // 释放DllGame初始化对象
            SAFE_DELETE(RrTetris::gs_pCMsBaseRun);

            RrTetris::gs_dwGameStationLogicThreadId = INVALID_UID;
            RrTetris::gs_hThread = NULL;
        }break;

        // 如果是线程操作,忽略
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        {
            ;
        }break;
    }

    // 返回
    return TRUE;
}

// DllGame启动线程函数
DWORD WINAPI RrTetris::RrTetrisThread(LPVOID)
{
    RrTetris::gs_lpCMsPacketFactory = NEW CMsPacketFactory(DG2S_PACKET_MAX_COUNT);
    AssertEx(AL_CATAST, gs_lpCMsPacketFactory, _T("内存不足"));
    RrTetris::gs_lpCMsPacketFactory->Register(g_lpCMsBasePacket, PACKET_MAX_COUNT);

    RrTetris::gs_pCMsBaseRun = NEW CMsBaseRun(_T("%s/Log/Dll_RrTetris_Assertx.log"), _T("RrTetrisClass"), _T("RrTetris"), RrTetrisHookProc, RrTetrisWndProc, NULL);
    AssertEx(AL_CATAST, gs_pCMsBaseRun, _T("内存不足"));
    S_2DEngineWndInfo s_ewi;
    s_ewi.m_bTopMost= FALSE;
    s_ewi.m_nX      = CW_USEDEFAULT;
    s_ewi.m_nY      = 0;
    s_ewi.m_nWidth  = CW_USEDEFAULT;
    s_ewi.m_nHeight = 0;
    s_ewi.m_dwStyle = WS_OVERLAPPEDWINDOW;

    // 等待基本初始化
    while(!g_IsMsBaseInit)
    {
        NO_CONTINUOUS;
    }

    // 申请日志对象
    RrTetris::gs_pMsLog = NEW CMsLog(_T("%s/Log/Dll_RrTetris_C.log"), TRUE);
    AssertEx(AL_CATAST, gs_pMsLog, _T("内存不足"));

    // 申请配置对象
    RrTetris::gs_pRrTetrisConfig = NEW CMsIni(_T("%s/RrTetris/RrTetris.ini"));
    AssertEx(AL_CATAST, gs_pRrTetrisConfig, _T("内存不足"));

    RrTetris::gs_hModule = ::GetModuleHandle(_T("Dll_RrTetris_C.dll"));
    RrTetris::gs_hInstance = (HINSTANCE)RrTetris::gs_hModule;
    if (RrTetris::gs_hInstance)
    {
        g_pCMsWinMain->Init(RrTetris::gs_hInstance, NULL, _T(""), SW_SHOW, &s_ewi);
        g_pCMsWinMain->SetLockSize(BACKSCENE_MAX_WIDTH, BACKSCENE_MAX_HEIGHT);
        g_pCMsWinMain->Run();
        g_pCMsWinMain->WaitQuit();
    }
    ::PostThreadMessage(gs_dwGameStationLogicThreadId, MSM_DLL_EXIT, (WPARAM)RrTetris::gs_hModule, 0);
    SAFE_DELETE(RrTetris::gs_pCMsBaseRun);
    SAFE_DELETE(RrTetris::gs_lpCMsPacketFactory);

    // 等待基本释放
    while(g_IsMsBaseInit)
    {
        NO_CONTINUOUS;
    }
    return 0;
}

// DllGame消息钩子函数
BOOL MSCALL RrTetris::RrTetrisHookProc(MSG* pMsg)
{
    USE_PARAM(pMsg);
    return FALSE;
}

// DllGame窗口消息回调
LRESULT CALLBACK RrTetris::RrTetrisWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    USE_PARAM(hWnd);
    USE_PARAM(wParam);
    USE_PARAM(lParam);
    RrTetrisWritePacket(NULL, 0);

    switch (uMsg)
    {
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                // 开始
            case ID_START_GAME:
                {
                    g_pGame->StartGame();
                }break;

                // 停止游戏
            case ID_STOP_GAME:
                {
                    g_pGame->EndGame();
                }break;

                // 结束
            case ID_EXIT_GAME:
                {
                    g_pGame->ReleaseGame();
                }break;

                // 重新开始
            case ID_RESTART_GAME:
                {
                    g_pGame->EndGame();
                    g_pGame->StartGame();
                }break;

                // 暂停继续
            case ID_PAUSE_CONTINUE_GAME:
                {
                    if (g_pGame->GetGameState() == GAME_STATE_PLAYING)
                    {
                        g_pGame->PauseGame();
                    }
                    else if (g_pGame->GetGameState() == GAME_STATE_PAUSE)
                    {
                        g_pGame->ContinueGame();
                    }
                }break;

                // 重置游戏
            case ID_RESET_GAME:
                {
                    g_pGame->ReleaseGame();
                    g_pGame->InitGame();
                }break;
            }
        }break;

    case WM_TIMER:
        {
            g_pGame->GameHeartbeat();
        }break;

    case WM_CLOSE:
        {
            g_pGame->ReleaseGame();
            SAFE_DELETE(g_pGame);
            ::PostQuitMessage(0);
        }break;

    case WM_CREATE:
        {
            ::SetWindowPos(hWnd, NULL, 0, 0, BACKSCENE_MAX_WIDTH, BACKSCENE_MAX_HEIGHT, SWP_NOMOVE|SWP_NOZORDER);
            while (INVALID_UID == gs_dwGameStationLogicThreadId)
            {
                NO_CONTINUOUS;
            }
            g_pCMsWinMain->SetAcceleratorID(IDR_ACCELERATOR);
            g_pGame = NEW CT_Game(hWnd);
            AssertEx(AL_CATAST, g_pGame, _T("内存不足"));
            g_pGame->InitGame();
            return ::PostThreadMessage(gs_dwGameStationLogicThreadId, MSM_DLL_NOTIFY_HWND, (WPARAM)hWnd, 0);
        }break;

    case WM_LBUTTONUP:
        {
            ::SendMessage(hWnd, WM_PRINT, 0, 0);
        }break;

    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case VK_SUBTRACT:
                {
                    g_pGame->SubMusicVolume();
                }break;
            case VK_ADD:
                {
                    g_pGame->AddMusicVolume();
                }break;
            }
            g_pGame->MoveShape((INT)wParam);
        }break;

    default:
        {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }break;
    }
    return TRUE;
}

// DllGame发送封包
BOOL RrTetris::RrTetrisWritePacket(
    PACKET_TYPE PacketId,
    LPVOID lpData,
    DWORD dwSize
    )
{
    if (PrivateRrTetrisWritePacket && gs_lpCMsPacketFactory)
    {
        return PrivateRrTetrisWritePacket(gs_lpCMsPacketFactory, PacketId, lpData, dwSize);
    }
    return FALSE;
}

// 设置游戏平台发包接口
VOID USE_DLL RrTetris::SetGameStationWritePacketFun(BOOL(*FunWritePacket)(CMsPacketFactory*,PACKET_TYPE,LPVOID,DWORD))
{
    PrivateRrTetrisWritePacket = FunWritePacket;
}

// 设置游戏平台逻辑线程Id给DllGame
VOID USE_DLL RrTetris::SetGameStationLogicThreadId(DWORD dwThreadId)
{
    RrTetris::gs_dwGameStationLogicThreadId = dwThreadId;
}

// 获取DllGame线程句柄
HANDLE USE_DLL RrTetris::GetDllGameLogicThread(VOID)
{
    return RrTetris::gs_hThread;
}

// 停止DllGame
HANDLE USE_DLL RrTetris::StopDllGame(VOID)
{
    g_pCMsWinMain->SendMessage(WM_CLOSE);
    return RrTetris::GetDllGameLogicThread();
}