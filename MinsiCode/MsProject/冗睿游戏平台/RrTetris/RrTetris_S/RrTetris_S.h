#pragma once

namespace RrTetris
{
    // DLlGame加载释放通知回调
    extern BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved);

    // 全局日志指针
    static CMsLog*      gs_pMsLog = NULL;

    // DllGame启动线程函数
    static DWORD WINAPI RrTetrisThread(LPVOID lpThreadParameter);

    // DllGame发送封包接口
    static BOOL(*PrivateRrTetrisWritePacket)(CMsPacketFactory*,PACKET_TYPE,LPVOID,DWORD) = NULL;

    // DllGame发送封包
    static BOOL RrTetrisWritePacket(
        PACKET_TYPE PacketId,
        LPVOID lpData,
        DWORD dwSize = INVALID_UID
        );

    static CMsPacketFactory* gs_lpCMsPacketFactory; // DllGame封包工厂
    static CMsIni*  gs_pRrTetrisConfig;             // DllGame配置管理器指针
    static DWORD    gs_dwGameStationLogicThreadId;  // 游戏平台逻辑线程Id
    static HANDLE   gs_hThread;                     // DllGame线程句柄

    // 设置游戏平台发包接口
    VOID USE_DLL SetGameStationWritePacketFun(BOOL(*FunWritePacket)(CMsPacketFactory*,PACKET_TYPE,LPVOID,DWORD)); 
    VOID USE_DLL SetGameStationLogicThreadId(DWORD dwThreadId); // 设置游戏平台逻辑线程Id给DllGame
    HANDLE USE_DLL GetDllGameLogicThread(VOID);                 // 获取DllGame线程句柄
    HANDLE USE_DLL StopDllGame(VOID);                           // 停止DllGame
};
