#include "Precompiled.h"

// 构造
CMsSocket::CMsSocket(
    DWORD dwBuffSize,
    DWORD dwLogicThreadId,
    DWORD dwNetThreadId,
    CMsPacketFactory* lpFactory
    )
: m_MsSendBuff          (dwBuffSize)
, m_MsRecvBuff          (TEMP_PACKET_RECV_COUNT, dwBuffSize)
, m_dwLogicThreadId     (dwLogicThreadId)
, m_dwNetThreadId       (dwNetThreadId)
, m_Socket              (INVALID_SOCKET)
, m_lpPacketData        (NULL)
, m_lpPacket            (NULL)
, m_u64RecvSize         (0)
, m_u64SendSize         (0)
, m_lpFactory           (lpFactory)
{
    ZeroMemoryStruct(m_RecvOverlapped);
    ZeroMemoryStruct(m_SendOverlapped);
    ZeroMemoryStruct(m_Sockaddr);
    m_MsRecvBuff.SetSocket(this);
    this->UpdataTime();
}

// 析构
CMsSocket::~CMsSocket(VOID)
{
    if (INVALID_SOCKET != m_Socket)
    {
        ::closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
    }
}

// 获取套接字
SOCKET CMsSocket::GetSocket(VOID)
{
    return m_Socket;
}

// 获取套接字端口
DWORD CMsSocket::GetProt(VOID)
{
    INT nSize = sizeof(m_Sockaddr);
    ::getsockname(m_Socket, (SOCKADDR*)&m_Sockaddr, &nSize);
    return ::ntohs(m_Sockaddr.sin_port);
}

// 获取套接字地址
ACHAR* CMsSocket::GetAddr(VOID)
{
    int nSize = sizeof(m_Sockaddr);
    ::getsockname(m_Socket, (SOCKADDR*)&m_Sockaddr, &nSize);
    return ::inet_ntoa(m_Sockaddr.sin_addr);
}

// 释放
BOOL CMsSocket::Release(VOID)
{
    ::closesocket(m_Socket);
    m_Socket = INVALID_SOCKET;
    return TRUE;
}

// 重置
BOOL CMsSocket::Reset(VOID)
{
    m_MsRecvBuff.Reset();
    m_MsSendBuff.Reset();
    this->Release();
    //// 创建服务器套接字
    //m_Socket = ::WSASocket(
    //    AF_INET,            // TCP、UDP
    //    SOCK_STREAM,        // TCP
    //    IPPROTO_TCP,        // TCP
    //    NULL,               // 附加条款结构体
    //    0,                  // 保留
    //    WSA_FLAG_OVERLAPPED // 重叠标记
    //    );
    //if (INVALID_SOCKET == m_Socket)
    //{
    //    AssertEx((INVALID_SOCKET != m_Socket), _T("套接字创建失败"));
    //}
    return TRUE;
}

//// 包到发送区
//BOOL CMsSocket::PacketToSend(CMsBasePacket* lpPacket)
//{
//    BOOL bRet = TRUE;
//    DWORD dwSize;
//    LPVOID lpData;
//
//    lpData = lpPacket->GetPacketHead(&dwSize);
//    bRet = m_MsSendBuff.PushData(lpData, dwSize);
//
//    lpData = lpPacket->GetValidPacketData(&dwSize);
//    bRet = m_MsSendBuff.PushData(lpData, dwSize);
//
//    if (bRet)
//    {
//        return ::PostThreadMessage(m_dwNetThreadId, MSM_MSG_SOCKET_SEND, (WPARAM)this, 0);
//    }
//    return bRet;
//}

//// 解析封包
//BOOL CMsSocket::ParsePacket(CMsPacketFactory* lpFactory)
//{
//
//}

// 执行包(仅限逻辑线程调用)
BOOL CMsSocket::ExecutePacket(DWORD dwLogicThreadId)
{
    if (m_dwLogicThreadId != dwLogicThreadId)
    {
        return FALSE;
    }
    BOOL bRet = TRUE;
    bRet = this->ReadPacket();
    if (bRet && m_lpPacket)
    {
        bRet = m_lpPacket->ExecutePacket(this);
        m_MsRecvBuff.CompleteExe();
    }
    else
    {
        AssertEx(m_lpPacket, _T("执行包不能为空"));
        AssertEx(bRet, _T("解析包错误"));
    }
    return bRet;
}

// 进入通知(仅限逻辑线程调用)
BOOL CMsSocket::OnEnter(DWORD dwLogicThreadId)
{
    if (m_dwLogicThreadId != dwLogicThreadId)
    {
        return FALSE;
    }
    return this->RecvData();
}

// 退出通知(仅限逻辑线程调用)
BOOL CMsSocket::OnExit(DWORD dwLogicThreadId)
{
    if (m_dwLogicThreadId != dwLogicThreadId)
    {
        return FALSE;
    }
    this->Reset();
    return TRUE;
}

// 读取封包,准备执行
BOOL CMsSocket::ReadPacket(VOID)
{
    lpPacketHead lpHead = NULL;
    m_MsRecvBuff.GetExePacket(lpHead, m_lpPacketData);
    AssertEx(lpHead, _T("错误逻辑"));
    AssertEx(m_lpPacketData, _T("错误逻辑"));

    if (m_lpFactory)
    {
        m_lpPacket = m_lpFactory->ParsePacketHead(lpHead);
        if (!m_lpPacket)
        {
            return FALSE;
        }
        return TRUE;
    }
    else
    {
        AssertEx(m_lpFactory, _T("错误逻辑"));
    }
    return FALSE;
}

// 写入封包,准备发送
BOOL CMsSocket::WritePacket(LPVOID lpData, DWORD dwSize)
{
    m_MsSendBuff.Push((LPBYTE)lpData, dwSize);
    if (!m_MsSendBuff.IsSending())
    {
        return this->SendData();
    }
    return TRUE;
}

// 获取封包数据
LPVOID CMsSocket::GetPacketData(VOID)
{
    return m_lpPacketData;
}

// 更新接收
BOOL CMsSocket::OnUpdate(S_IOCP_UPDATE* lpIocpUpdate)
{
    if (lpIocpUpdate == &m_RecvOverlapped)
    {
        return this->OnUpdateRecv(lpIocpUpdate->m_dwNumOfBytesComplete);
    }
    else if (lpIocpUpdate == &m_SendOverlapped)
    {
        return this->OnUpdateSend(lpIocpUpdate->m_dwNumOfBytesComplete);
    }
    else
    {
        AssertEx(FALSE, _T("错误逻辑"));
        return FALSE;
    }
}

// 获取逻辑处理线程Id
DWORD CMsSocket::GetLogicThreadId(VOID)
{
    return m_dwLogicThreadId;
}

// 获取网络处理线程Id
DWORD CMsSocket::GetNetThreadId(VOID)
{
    return m_dwNetThreadId;
}

// 更新接收(仅限自己分配调用)
BOOL CMsSocket::OnUpdateRecv(DWORD dwNumOfBytesUpdate)
{
    // 压入数据
    BOOL bRet = m_MsRecvBuff.Push(dwNumOfBytesUpdate);
    if (!bRet)
    {
        return bRet;
    }

    //// 获取完成包个数
    //DWORD dwPacketCount = m_MsRecvBuff.GetCompletePacketCount();
    //// 如果有包,通知进行处理
    //if (dwPacketCount > 0)
    //{
    //    ::PostThreadMessage(m_dwLogicThreadId, MSM_MSG_SOCKET_EXECUTE, (WPARAM)this, 0);
    //}

    // 继续接收
    return this->RecvData();
}

// 更新发送(仅限自己分配调用)
BOOL CMsSocket::OnUpdateSend(DWORD dwNumOfBytesUpdate)
{
    m_MsSendBuff.Pop(dwNumOfBytesUpdate);

    DWORD dwDataSize = m_MsSendBuff.GetDataSize();
    if (dwDataSize == 0)
    {
        m_MsSendBuff.SetSending(FALSE);
        return TRUE;
    }
    else
    {
        return this->SendData();
    }
}

// 获取终端时间
FILETIME& CMsSocket::GetTime(VOID)
{
    return m_Time;
}

// 更新终端时间
VOID CMsSocket::UpdataTime(VOID)
{
    ::GetSystemTimeAsFileTime(&m_Time);
}

// 判断是否有效
BOOL CMsSocket::IsValid(VOID)
{
    return (m_Socket != INVALID_SOCKET);
}


// 设置池子索引
VOID CMsSocket::SetPoolIndex(ULONG_PTR ulIndex)
{
    m_ulIndexInPool = ulIndex;
}

// 获取池子索引
ULONG_PTR CMsSocket::GetPoolIndex(VOID)
{
    return m_ulIndexInPool;
}

// 获取接收IO结构体
S_IOCP_UPDATE* CMsSocket::GetRecvIO(VOID)
{
    return &m_RecvOverlapped;
}

// 接收数据
BOOL CMsSocket::RecvData(VOID)
{
    DWORD   dwFlags = 0;
    DWORD   dwTemp = 0;
    WSABUF  wb;

    // 准备接收
    wb.buf = (LPCHAR)m_MsRecvBuff.GetRecvAddr(wb.len);

    // 接收缓冲中没有控件
    if (wb.len == 0)
    {
        return TRUE;
    }

    // 接收
    INT nRet = ::WSARecv(
        m_Socket,
        &wb,
        1,
        &dwTemp,
        &dwFlags,
        &m_RecvOverlapped,
        NULL
        );

    // 如果没有顺利接收完毕
    if (SOCKET_ERROR == nRet)
    {
        // 获取错误标记
        nRet = ::GetLastError();

        // 如果不是正在接收中
        if (ERROR_IO_PENDING != nRet)
        {
            return FALSE;
        }
    }
    return TRUE;
}

// 发送数据
BOOL CMsSocket::SendData(VOID)
{
    DWORD   dwFlags = 0;
    DWORD   dwTemp = 0;
    WSABUF  wb;

    // 准备发送
    wb.buf = (LPCHAR)m_MsSendBuff.GetSendAddr(wb.len);

    // 发送缓冲中没有数据
    if (wb.len == 0)
    {
        return TRUE;
    }

    m_MsSendBuff.SetSending(TRUE);

    // 发送
    INT nRet = ::WSASend(
        m_Socket,
        &wb,
        1,
        &dwTemp,
        dwFlags,
        &m_SendOverlapped,
        NULL
        );

    // 如果没有顺利发送完毕
    if (SOCKET_ERROR == nRet)
    {
        // 获取错误标记
        nRet = ::GetLastError();

        // 如果不是正在发送中
        if (ERROR_IO_PENDING != nRet)
        {
            return FALSE;
        }
    }
    return TRUE;
}
