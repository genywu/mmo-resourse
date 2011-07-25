#include "Precompiled.h"

// 构造
CMsClientSocket::CMsClientSocket(
    DWORD dwBuffSize,
    DWORD dwLogicThreadId,
    DWORD dwNetThreadId,
    CMsPacketFactory* lpFactory
    )
: CMsSocket(dwBuffSize, dwLogicThreadId, dwNetThreadId, lpFactory)
{
    ;
}

// 析构
CMsClientSocket::~CMsClientSocket(VOID)
{
    ;
}

// 设置套接字
VOID CMsClientSocket::SetSocket(SOCKET Socket)
{
    m_Socket = Socket;
}
