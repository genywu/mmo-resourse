#include "Precompiled.h"

CGameStation* g_pCGameStation = NULL;

CGameStation::CGameStation(HWND hWnd, DWORD dwCurrentThreadId)
: m_hWnd                    (hWnd)
, m_pCMsPacketFactory       (NULL)
, m_pCMsClientNetManager    (NULL)
, m_dwCurrentThreadId       (dwCurrentThreadId)
, m_pGameTree               (NULL)
, m_bInit                   (FALSE)
, m_hDllGameWnd             (NULL)
, m_funSetGameStationLogicThreadId  (NULL)
, m_funGetDllGameLogicThread        (NULL)
, m_funStopDllGame          (NULL)
, m_hDllGameModule          (NULL)
{
    this->LoadConfig();
    m_dwGameWindowWidth = m_Config.dwGameStationWindowWidth;
    m_dwGameWindowHight = m_Config.dwGameStationWindowHeight;
    // 设置窗口大小
    ::SetWindowPos(
        hWnd,
        NULL,
        30,
        30,
        m_dwGameWindowWidth,
        m_dwGameWindowHight,
        SWP_NOZORDER
        );

    m_pBuffImage = NEW CMsGdiBuffImage;
    AssertEx(AL_CATAST, m_pBuffImage, _T("内存不足"));
    m_pBuffImage->Init(hWnd, m_dwGameWindowWidth, m_dwGameWindowHight, RGB(255,0,0));
}
CGameStation::~CGameStation(VOID)
{
    this->Realese();
}

BOOL CGameStation::Init()
{
    if (m_bInit)
    {
        return TRUE;
    }

    m_pCMsPacketFactory = NEW CMsPacketFactory(RR_GAME_PACKET_MAX_COUNT);
    if (m_pCMsPacketFactory)
    {
        m_pCMsPacketFactory->Register(g_lpCMsBasePacket, PACKET_MAX_COUNT);

        m_pCMsClientNetManager = NEW CMsClientNetManager(m_ServerConfig.IP, m_ServerConfig.wProt, ::GetCurrentThreadId(), m_pCMsPacketFactory);
        if (m_pCMsClientNetManager)
        {
            m_pCMsClientNetManager->Run();
        }
        else
        {
            AssertEx(AL_CATAST, m_pCMsClientNetManager, _T("内存不足"));
        }
    }
    else
    {
        AssertEx(AL_CATAST, m_pCMsPacketFactory, _T("内存不足"));
    }

    if (m_pBuffImage)
    {
        m_pGameTree = NEW CGameTree(m_pBuffImage);
        AssertEx(AL_CATAST, m_pGameTree, _T("内存不足"));
        m_bInit = TRUE;
        return TRUE;
    }
    else
    {
        this->Realese();
        return FALSE;
    }
}

VOID CGameStation::Realese()
{
    if (m_bInit)
    {
        m_pCMsClientNetManager->Disconnect();
        SAFE_DELETE(m_pCMsClientNetManager);
        SAFE_DELETE(m_pCMsPacketFactory);
        SAFE_DELETE(m_pGameTree);
        SAFE_DELETE(m_pBuffImage);
        m_bInit = FALSE;
    }
}

// 判断网络是否有效
BOOL CGameStation::IsNetValid(VOID)
{
    return m_pCMsClientNetManager->IsValid();
}

VOID CGameStation::Render(VOID)
{
    //BOOL bRet = m_pCMsClientNetManager->IsValid();
    //if (bRet)
    //{
    //    C2S_Ask_GameTypeList::MS_PACKET_DATA PacketData;
    //    PacketData.m_dwGameTypeIndex = 0;
    //    FOR_x_TO(i, 1000)
    //    {
    //        CMsServerSocket* lpServerSocket = m_pCMsClientNetManager->GetServerSocket();
    //        lpServerSocket->WritePacket(BPT(C2S_Ask_GameTypeList), &PacketData);
    //    }
    //}
    m_pBuffImage->BeginDraw();
    m_pGameTree->Render();
    m_pBuffImage->EndDraw();
}

// 获取服务器套接字
lpCMsServerSocket CGameStation::GetServerSocket(VOID)
{
    if (m_pCMsClientNetManager)
    {
        return m_pCMsClientNetManager->GetServerSocket();
    }
    return NULL;
}

S_GameStationConfig* CGameStation::GetConfig(VOID)
{
    return &m_Config;
}

S_GameStationResConfig* CGameStation::GetResConfig(VOID)
{
    return &m_ResConfig;
}

VOID CGameStation::LoadConfig(VOID)
{
    //S_GameStationConfig 结构体数据
    m_Config.dwGameStationWindowWidth = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameStationWindowWidth"), 0);
    m_Config.dwGameStationWindowHeight = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameStationWindowHight"), 0);
    m_Config.m_dwBetween = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("Between"), 0);

    m_Config.dwGameTypeWidth = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameTypeWidth"), 0);
    m_Config.dwGameTypeHeight  = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameTypeHight"), 0);
    m_Config.dwGameTypeIndent = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameTypeIndent"), 0);

    m_Config.dwGameWidth = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameWidth"), 0);
    m_Config.dwGameHeight = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameHight"), 0);
    m_Config.dwGameIndent = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("GameIndent"), 0);

    m_Config.dwDistrictWidth = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("DistrictWidth"), 0);
    m_Config.dwDistrictHeight = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("DistrictHight"), 0);
    m_Config.dwDistrictIndent = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("DistrictIndent"), 0);

    m_Config.dwRoomWidth = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("RoomWidth"), 0);
    m_Config.dwRoomHeight = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("RoomHight"), 0);
    m_Config.dwRoomIndent = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("RoomIndent"), 0);

    m_Config.dwDeskWidth = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("DeskWidth"), 0);
    m_Config.dwDeskHeight = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("DeskHight"), 0);
    m_Config.dwDeskIndent = g_RrGameStationIni.GetDword(_T("Exe_RrGameStation"), _T("DeskIndent"), 0);

    g_RrResConfigIni.GetString(_T("RrGameResDir"), _T("ImageDir"), _T(""), m_ResConfig.m_szImageDir, MAX_PATH);

    FOR_x_TO(i, MAX_RES_COUNT)
    {
        g_RrResConfigIni.GetString(
            _T("RrGameResImageMap"),
            g_RrResConfigIni.ParseSectionIndex(_T("%02d"),i),
            _T(""),
            m_ResConfig.m_szImageName[i],
            MAX_RES_FILE_NAME_LEN
            );
    }
    //服务器信息
    g_RrGameStationIni.GetString(_T("Exe_RrServerConfig"), _T("IP"), _T(""),m_ServerConfig.IP,IP_SIZE);
    m_ServerConfig.wProt = g_RrGameStationIni.GetWord(_T("Exe_RrServerConfig"), _T("Prot"), 0);

}

// 获取列表树
CGameTree* CGameStation::GetTree(VOID)
{
    return m_pGameTree;
}

BOOL CGameStation::GameStationWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    USE_PARAM(hWnd,uMsg,wParam,lParam);
    switch (uMsg)
    {
        // DllGame窗口句柄
    case MSM_DLL_NOTIFY_HWND:
        {
            m_hDllGameWnd = (HWND)wParam;
        }break;

    case WM_MOUSEMOVE:
        {
            BOOL bRet = m_pGameTree->IsScrollFocus();
            if (bRet)
            {
                if (((::GetKeyState(HGEK_LBUTTON) & 0x8000) == 0))
                {
                    m_pGameTree->ReleaseScrollFocus();
                }
                else
                {
                    POINT Pos = { LOWORD(lParam), HIWORD(lParam)};
                    POINT CurPos = Pos;
                    m_pGameTree->OperationPosOffset(Pos);
                    m_pGameTree->MoveScroll(Pos);
                    m_pGameTree->SaveMousePos(CurPos);
                }
            }
        }break;

    case WM_LBUTTONDOWN:
        {
            POINT Pos = { LOWORD(lParam), HIWORD(lParam)};
            BOOL bRet = m_pGameTree->IsOnScrollbar(Pos);
            if (bRet)
            {
                m_pGameTree->SaveMousePos(Pos);
                m_pGameTree->GetScrollFocus();
            }
        }break;

    case WM_LBUTTONUP:
        {
            BOOL bRet = m_pGameTree->IsScrollFocus();
            if (bRet)
            {
                m_pGameTree->ReleaseScrollFocus();
            }
            //m_GameTree.m_lpGameTree->SetScrollbarMove(FALSE);

            //POINT pos = { LOWORD(lParam), HIWORD(lParam)};
            //CBaseNode* Node = MouseTest(pos, m_GameTree.m_lpGameTree);
            //if (Node)
            //{
            //    if (Node->IsOpen())
            //    {
            //        Node->SetOpen(FALSE);
            //    }else
            //    {
            //        if (Node->GetType() == NODE_ROOM)
            //        {
            //        }else
            //        {
            //            Node->SetOpen(TRUE);
            //            m_GameTree.m_lpGameTree->UpDataNode(Node->GetType(),Node->GetNodeId(),2,NULL);
            //        }
            //    }
            //}
        }break;

        // 连接服务器通知
    case MSM_CLIENT_SOCKET_ON_ENTER:
        {
            if (!hWnd)
            {
                {
                    g_pCMsWinMain->SetWndName(_T("冗睿服务器,已连接"));

                    lpCMsServerSocket lpServerSocket = g_pCGameStation->GetServerSocket();
                    if (lpServerSocket)
                    {
                        C2S_Connect::MS_PACKET_DATA PacketData;
                        PacketData.m_dwData = CONNECT_KEY;
                        lpServerSocket->WritePacket(BPT(C2S_Connect), &PacketData, sizeof(PacketData));
                    }
                }
                {
                    lpCMsServerSocket lpServerSocket = g_pCGameStation->GetServerSocket();
                    if (lpServerSocket)
                    {
                        C2S_Ask_GameTypeList::MS_PACKET_DATA PacketData;
                        PacketData.m_dwGameTypeIndex = 0;
                        lpServerSocket->WritePacket(BPT(C2S_Ask_GameTypeList), &PacketData, sizeof(PacketData));
                    }
                }
            }
            return TRUE;
        }break;

        // 断开服务器通知
    case MSM_CLIENT_SOCKET_ON_EXIT:
        {
            if (!hWnd)
            {
                ::MessageBox(m_hWnd, _T("服务器断开"), _T("提示"), MB_OK);
                ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
            }
            return TRUE;
        }break;

        // 有包需要执行
    case MSM_MSG_SOCKET_EXECUTE:
        {
           return TRUE;
        }break;


        // 加载DllGame
    case WM_MBUTTONUP:
        {
            if (m_hDllGameModule)
            {
                return TRUE;
            }
            m_hDllGameModule = ::LoadLibrary(_T("Dll_RrTetris_C.dll"));
            if (m_hDllGameModule)
            {
                m_funSetGameStationLogicThreadId = (VOID(*)(DWORD))::GetProcAddress(m_hDllGameModule, "SetGameStationLogicThreadId");
                AssertEx(AL_SHOW_ONLY, m_funSetGameStationLogicThreadId, _T("加载游戏失败"));
                m_funSetGameStationLogicThreadId(m_dwCurrentThreadId);

                m_funGetDllGameLogicThread = (HANDLE(*)(VOID))::GetProcAddress(m_hDllGameModule, "GetDllGameLogicThread");
                AssertEx(AL_SHOW_ONLY, m_funGetDllGameLogicThread, _T("加载游戏失败"));

                m_funStopDllGame = (HANDLE(*)(VOID))::GetProcAddress(m_hDllGameModule, "StopDllGame");
                AssertEx(AL_SHOW_ONLY, m_funStopDllGame, _T("加载游戏失败"));
            }
            else
            {
                AssertEx(AL_SHOW_ONLY, m_hDllGameModule, _T("加载游戏失败"));
            }
            return TRUE;
        }break;

        // DllGame退出
    case MSM_DLL_EXIT:
        {
            HMODULE hModule = (HMODULE)wParam;
            if (hModule == m_hDllGameModule)
            {
                HANDLE hThread = m_funGetDllGameLogicThread();
                ::WaitForSingleObject(hThread, INFINITE);
                ::FreeLibrary(m_hDllGameModule);
                m_hDllGameModule = NULL;
            }
        }break;

        // 关闭窗口
    case WM_CLOSE:
        {
            if (m_hDllGameModule && m_funStopDllGame)
            {
                HANDLE hThread = m_funStopDllGame();
                ::WaitForSingleObject(hThread, INFINITE);
                ::FreeLibrary(m_hDllGameModule);
                m_hDllGameModule = NULL;
            }
            return FALSE;
        }break;
    }
    return FALSE;
}

HWND CGameStation::GetHwnd(VOID)
{
    return m_hWnd;
}

// 获取缓冲图
CMsGdiBuffImage* CGameStation::GetBuffImage(VOID)
{
    return m_pBuffImage;
}

// DllGame调用接口
BOOL CGameStation::DllGameWritePacket(CMsPacketFactory* lpFactory, PACKET_TYPE PacketId, LPVOID lpData, DWORD dwSize)
{
    if (g_pCGameStation)
    {
        lpCMsServerSocket lpSocket = g_pCGameStation->m_pCMsClientNetManager->GetServerSocket();
        if (lpSocket)
        {
            return lpSocket->WritePacket(lpFactory, PacketId, lpData, dwSize);
        }
    }
    return FALSE;
}