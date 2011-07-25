#include "stdafx.h"
#include "Server.h"

// 服务器构造函数
CServer::CServer(VOID)
{
    _ENTER_FUN;

    _LEAVE_FUN_KEEP_NO_DISPOSE;
}

// 服务器析构函数
CServer::~CServer(VOID)
{
    _ENTER_FUN;

    _LEAVE_FUN_KEEP_NO_DISPOSE;
}

// 消息处理
// 数据更新状态
BOOL CServer::OnUpDataed(VOID)
{
    _ENTER_FUN;

    _LEAVE_FUN_KEEP_NO_DISPOSE;
    return FALSE;
}
// 退出状态
BOOL CServer::OnExit(VOID)
{
    _ENTER_FUN;

    _LEAVE_FUN_KEEP_NO_DISPOSE;
    return FALSE;
}

// 错误状态
BOOL CServer::OnError(VOID)
{
    _ENTER_FUN;

    ::AfxMessageBox(_T("服务器断开"));

    _LEAVE_FUN_KEEP_NO_DISPOSE;
    return FALSE;
}

// 将发送封包拷贝到待发送缓冲中
BOOL CServer::WritePacket(CBasePacket* lpPacket)
{
    return CMsSocket::WritePacket(lpPacket);
}

// 将收到的数据拷贝到接收缓冲中
USIZE CServer::ReadPacket(CBasePacket* lpPacket)
{
    return CMsSocket::ReadPacket(lpPacket);
}

// 执行收到的封包
BOOL CServer::ExecutePacket(LPVOID pCClientNetManager)
{
    _ENTER_FUN;

    CClientNetManager* lpCClientNetManager = (CClientNetManager*)pCClientNetManager;
    return m_pBasePacket->ExecutePacket(lpCClientNetManager, this);

    _LEAVE_FUN_KEEP_NO_DISPOSE;
    return FALSE;
}