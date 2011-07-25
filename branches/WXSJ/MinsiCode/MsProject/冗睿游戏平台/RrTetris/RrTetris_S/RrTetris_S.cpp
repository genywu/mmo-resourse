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

    RrTetrisWritePacket(0,NULL);

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


    ::PostThreadMessage(gs_dwGameStationLogicThreadId, MSM_DLL_EXIT, 0, 0);
    SAFE_DELETE(RrTetris::gs_lpCMsPacketFactory);

    // 等待基本释放
    while(g_IsMsBaseInit)
    {
        NO_CONTINUOUS;
    }
    return 0;
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
    return RrTetris::GetDllGameLogicThread();
}