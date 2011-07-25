#include "Precompiled.h"

// 构造
CMsServerSocket::CMsServerSocket(
    DWORD dwBuffSize,
    DWORD dwLogicThreadId,
    DWORD dwNetThreadId,
    CMsPacketFactory* lpFactory
    )
: CMsSocket(dwBuffSize, dwLogicThreadId, dwNetThreadId, lpFactory)
{
    // 创建服务器套接字
    m_Socket = ::WSASocket(
        AF_INET,            // TCP、UDP
        SOCK_STREAM,        // TCP
        IPPROTO_TCP,        // TCP
        NULL,               // 附加条款结构体
        0,                  // 保留
        WSA_FLAG_OVERLAPPED // 重叠标记
        );
    if (INVALID_SOCKET == m_Socket)
    {
        AssertEx(AL_CATAST, (INVALID_SOCKET != m_Socket), _T("套接字创建失败"));
    }
}

// 析构
CMsServerSocket::~CMsServerSocket(VOID)
{
    ;
}
