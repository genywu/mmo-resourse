#include "Precompiled.h"

MsRecvBuffer::MsRecvBuffer(DWORD dwPacketCount, DWORD dwMaxBuffSize)
: m_dwPacketCount           (dwPacketCount)
, m_dwMaxBuffSize           (dwPacketCount * dwMaxBuffSize)
, m_dwCompletePacketCount   (0)
, m_dwCurrentRecvPacketIndex(0)
, m_dwCurrentExePacketIndex (0)
, m_dwRecvedSize            (0)
, m_dwCurrentDataSize       (0)
, m_dwExeingPacketIndex     (0)
, m_eRecvState              (RECV_HEAD)
{
    AssertEx(AL_CATAST, m_dwPacketCount, _T("包头个数不能为0"));
    AssertEx(AL_CATAST, dwMaxBuffSize, _T("缓冲区尺寸不能为0"));
    
    m_lpPacketHead = NEW RecvPacketBuff[m_dwPacketCount];
    if (m_lpPacketHead)
    {
        ZeroMemory(m_lpPacketHead, sizeof(RecvPacketBuff) * m_dwPacketCount);
    }
    else
    {
        AssertEx(AL_CATAST, m_lpPacketHead, _T("内存不足"));
    }

    m_lpBuffHead = NEW BYTE[m_dwMaxBuffSize];
    if (m_lpBuffHead)
    {
        ZeroMemory(m_lpBuffHead, sizeof(BYTE) * m_dwMaxBuffSize);

        m_lpBuffTail = &m_lpBuffHead[m_dwMaxBuffSize - 1];
        m_pDataHead = m_pDataTail = m_lpBuffHead;
    }
    else
    {
        AssertEx(AL_CATAST, m_lpBuffHead, _T("内存不足"));
    }
}

MsRecvBuffer::~MsRecvBuffer(VOID)
{
    SAFE_DELETE_ARRAY(m_lpBuffHead);
    SAFE_DELETE_ARRAY(m_lpPacketHead);
}

// 获取接收位置、尺寸
LPBYTE MsRecvBuffer::GetRecvAddr(DWORD& dwSize)
{
    AUTO_LOCK(m_Lock);

    LPBYTE lpRecvAddr = NULL;
    DWORD dwFreeSize = this->GetFreeSize();
    if (dwFreeSize == 0)
    {
        dwSize = 0;
        return NULL;
    }

    // 如果需要接收包头
    if (m_eRecvState == RECV_HEAD)
    {
        // 接收到包头数据区
        lpRecvAddr = (LPBYTE)&m_lpPacketHead[m_dwCurrentRecvPacketIndex].m_PacketHead;
        lpRecvAddr += m_dwRecvedSize;
        dwSize = sizeof(PacketHead) - m_dwRecvedSize;
        return lpRecvAddr;
    }

    // 如果需要接收数据
    else if (m_eRecvState == RECV_DATA)
    {
        DWORD dwNeedSize = m_lpPacketHead[m_dwCurrentRecvPacketIndex].m_PacketHead.m_dwPackSize - m_dwRecvedSize - sizeof(PacketHead);
        // 接收到数据头处
        lpRecvAddr = m_pDataHead;
        if (m_pDataHead >= m_pDataTail)
        {
            dwSize = (DWORD)(m_lpBuffTail - m_pDataHead);
        }
        else
        {
            dwSize = (DWORD)(m_pDataTail - m_pDataHead);
        }

        if (dwSize > dwNeedSize)
        {
            dwSize = dwNeedSize;
        }
        return lpRecvAddr;
    }
    dwSize = 0;
    return NULL;
}

// 重置缓冲区
VOID MsRecvBuffer::Reset(VOID)
{
    m_dwCompletePacketCount     = 0;
    m_dwCurrentRecvPacketIndex  = 0;
    m_dwCurrentExePacketIndex   = 0;
    m_dwRecvedSize              = 0;
    m_dwCurrentDataSize         = 0;
    m_dwExeingPacketIndex       = 0;
    m_eRecvState                = RECV_HEAD;

    ZeroMemory(m_lpPacketHead, sizeof(RecvPacketBuff) * m_dwPacketCount);

    ZeroMemory(m_lpBuffHead, sizeof(BYTE) * m_dwMaxBuffSize);

    m_lpBuffTail = &m_lpBuffHead[m_dwMaxBuffSize - 1];
    m_pDataHead = m_pDataTail = m_lpBuffHead;
}


// 获取空闲尺寸
DWORD MsRecvBuffer::GetFreeSize(VOID)
{
    AUTO_LOCK(m_Lock);

    if (m_dwCurrentDataSize < m_dwMaxBuffSize)
    {
        return m_dwMaxBuffSize - m_dwCurrentDataSize;
    }
    return 0;
}

// 压入数据
BOOL MsRecvBuffer::Push(DWORD dwSize)
{
    AUTO_LOCK(m_Lock);

    if (dwSize > this->GetFreeSize())
    {
        //AssertEx(AL_NORMAL, FALSE, _T("没有足够的空余空间,错误逻辑"));
        return FALSE;
    }

    // 如果是接收包头
    if (m_eRecvState == RECV_HEAD)
    {
        m_dwRecvedSize += dwSize;

        // 如果接收完毕
        if (m_dwRecvedSize >= sizeof(PacketHead))
        {
            m_eRecvState = RECV_DATA;
            m_dwRecvedSize = 0;
        }

        // 不允许接收尺寸越界
        else if (m_dwRecvedSize >= sizeof(PacketHead))
        {
            AssertEx(AL_CATAST, FALSE, _T("错误逻辑"));
        }
    }

    // 如果是接收数据
    else if (m_eRecvState == RECV_DATA)
    {
        m_dwRecvedSize += dwSize;

        // 如果接收完毕
        if (m_dwRecvedSize >= m_lpPacketHead[m_dwCurrentRecvPacketIndex].m_PacketHead.m_dwPackSize - sizeof(PacketHead))
        {
            m_lpPacketHead[m_dwCurrentRecvPacketIndex].m_IsValid = TRUE;
            m_lpPacketHead[m_dwCurrentRecvPacketIndex].m_lpPacketData = m_pDataHead;
            m_dwCurrentRecvPacketIndex++;
            if (m_dwCurrentRecvPacketIndex >= m_dwPacketCount)
            {
                m_dwCurrentRecvPacketIndex = 0;
            }
            if (m_lpPacketHead[m_dwCurrentRecvPacketIndex].m_IsValid)
            {
                // 接收缓冲已满
                DWORD dwNetThreadId = m_lpSocket->GetNetThreadId();
                return ::PostThreadMessage(dwNetThreadId, MSM_IOCP_POST_QUEUED_COMPLETION_STATUS, (WPARAM)m_lpSocket, INVALID_UID);
            }
            m_eRecvState = RECV_HEAD;
            m_dwCompletePacketCount++;
            m_dwRecvedSize = 0;

            DWORD dwLogicThreadId = m_lpSocket->GetLogicThreadId();
            m_lpSocket->ExecutePacket();
            return ::PostThreadMessage(dwLogicThreadId, MSM_MSG_SOCKET_EXECUTE, (WPARAM)m_lpSocket, 0);
        }
    }

    // 更新数据头位置
    m_pDataHead += dwSize;

    // 更新数据尺寸
    m_dwCurrentDataSize += dwSize;
    AssertEx(AL_CATAST, (m_pDataHead <= m_lpBuffTail), _T("数据头不能在缓冲尾后面,错误逻辑"));

    // 如果数据头已经在最后
    if (m_pDataHead == m_lpBuffTail)
    {
        // 将数据头指向缓冲头
        m_pDataHead = m_lpBuffHead;
    }
    return TRUE;
}

// 弹出数据
VOID MsRecvBuffer::Pop(DWORD dwSize)
{
    AUTO_LOCK(m_Lock);

    // 判断是否有足够的数据空间
    if (dwSize > m_dwCurrentDataSize)
    {
        return;
    }

    // 如果数据尾在数据头左侧
    if (m_pDataTail < m_pDataHead)
    {
        _DEBUG_CODE(ZeroMemory(m_pDataTail, dwSize));
        m_dwCurrentDataSize -= dwSize;
        m_pDataTail += dwSize;
        if (m_pDataTail == m_lpBuffTail)
        {
            m_pDataTail = m_lpBuffHead;
        }
    }

    // 如果数据尾在数据头右侧或重叠
    else
    {
        // 获取连续空余空间尺寸
        DWORD dwSeriateSize = (DWORD)(m_lpBuffTail - m_pDataTail);

        // 如果连续数据空间够用
        if (dwSize <= dwSeriateSize)
        {
            _DEBUG_CODE(ZeroMemory(m_pDataTail, dwSize));
            m_dwCurrentDataSize -= dwSize;
            m_pDataTail += dwSize;
            if (m_pDataTail == m_lpBuffTail)
            {
                m_pDataTail = m_lpBuffHead;
            }
        }

        // 如果连续数据空间不够用
        else
        {
            _DEBUG_CODE(ZeroMemory(m_pDataTail, dwSeriateSize));
            m_dwCurrentDataSize -= dwSeriateSize;
            dwSize -= dwSeriateSize;
            _DEBUG_CODE(ZeroMemory(m_lpBuffHead, dwSize));
            m_dwCurrentDataSize -= dwSize;
            m_pDataTail = m_lpBuffHead + dwSize;
            if (m_pDataTail == m_lpBuffTail)
            {
                m_pDataTail = m_lpBuffHead;
            }
        }
    }
}

// 完成执行
VOID MsRecvBuffer::CompleteExe(VOID)
{
    AUTO_LOCK(m_Lock);

    DWORD dwSize = m_lpPacketHead[m_dwCurrentExePacketIndex].m_PacketHead.m_dwPackSize;
    this->Pop(dwSize);
    m_lpPacketHead[m_dwExeingPacketIndex].m_IsValid = FALSE;
}

// 获取执行封包
VOID MsRecvBuffer::GetExePacket(OUT lpPacketHead& lpPacketHead, OUT LPVOID& lpData)
{
    AUTO_LOCK(m_Lock);

    if (m_dwCompletePacketCount == 0)
    {
        AssertEx(AL_CATAST, FALSE, _T("错误逻辑"));
    }
    if (!m_lpPacketHead[m_dwCurrentExePacketIndex].m_IsValid)
    {
        AssertEx(AL_CATAST, FALSE, _T("错误逻辑"));
    }

    lpPacketHead = &m_lpPacketHead[m_dwCurrentExePacketIndex].m_PacketHead;
    lpData = m_lpPacketHead[m_dwCurrentExePacketIndex].m_lpPacketData;
    m_dwExeingPacketIndex = m_dwCurrentExePacketIndex;
    m_dwCurrentExePacketIndex++;
    m_dwCompletePacketCount--;
    if (m_dwCurrentExePacketIndex >= m_dwPacketCount)
    {
        m_dwCurrentExePacketIndex = 0;
    }
}

// 获取完成包个数
DWORD MsRecvBuffer::GetCompletePacketCount(VOID)
{
    AUTO_LOCK(m_Lock);

    return m_dwCompletePacketCount;
}

// 设置套接字指针
VOID MsRecvBuffer::SetSocket(CMsSocket* lpSocket)
{
    AssertEx(AL_CATAST, lpSocket, _T("套接字指针不能为空"));
    m_lpSocket = lpSocket;
}





MsSendBuffer::MsSendBuffer(DWORD dwMaxBuffSize)
: m_dwMaxBuffSize       (dwMaxBuffSize)
, m_dwCurrentDataSize   (0)
, m_bSending            (FALSE)
{
    AssertEx(AL_CATAST, dwMaxBuffSize, _T("缓冲区尺寸不能为0"));

    m_lpBuffHead = NEW BYTE[m_dwMaxBuffSize];
    if (m_lpBuffHead)
    {
        m_lpBuffTail = &m_lpBuffHead[m_dwMaxBuffSize - 1];
        m_pDataHead = m_pDataTail = m_lpBuffHead;
    }
    else
    {
        AssertEx(AL_CATAST, m_lpBuffHead, _T("内存不足"));
    }
}

MsSendBuffer::~MsSendBuffer(VOID)
{
    SAFE_DELETE_ARRAY(m_lpBuffHead);
}

// 获取发送位置、尺寸
LPBYTE MsSendBuffer::GetSendAddr(DWORD& dwSize)
{
    AUTO_LOCK(m_Lock);

    if (m_dwCurrentDataSize == 0)
    {
        dwSize = 0;
        return NULL;
    }

    if (m_pDataHead > m_pDataTail)
    {
        dwSize = (DWORD)(m_pDataHead - m_pDataTail);
    }
    else
    {
        dwSize = (DWORD)(m_lpBuffTail - m_pDataTail);
    }
    return m_pDataTail;
}

// 获取数据尺寸
DWORD MsSendBuffer::GetDataSize(VOID)
{
    AUTO_LOCK(m_Lock);

    return m_dwCurrentDataSize;
}

// 重置缓冲区
VOID MsSendBuffer::Reset(VOID)
{
    m_dwCurrentDataSize = 0;
    m_bSending = FALSE;
    ZeroMemory(m_lpBuffHead, sizeof(BYTE) * m_dwMaxBuffSize);
    m_pDataHead = m_pDataTail = m_lpBuffHead;
}

// 获取空闲尺寸
DWORD MsSendBuffer::GetFreeSize(VOID)
{
    AUTO_LOCK(m_Lock);

    if (m_dwCurrentDataSize < m_dwMaxBuffSize)
    {
        return m_dwMaxBuffSize - m_dwCurrentDataSize;
    }
    return 0;
}

// 压入数据
BOOL MsSendBuffer::Push(LPBYTE lpData, DWORD dwSize)
{
    AUTO_LOCK(m_Lock);

    // 判断是否有足够的空余空间
    if (dwSize > this->GetFreeSize())
    {
        return FALSE;
    }

    // 如果数据头在数据尾左侧
    if (m_pDataHead < m_pDataTail)
    {
        memcpy(m_pDataHead, lpData, dwSize);
        m_dwCurrentDataSize += dwSize;
        m_pDataHead += dwSize;
        if (m_pDataHead == m_lpBuffTail)
        {
            m_pDataHead = m_lpBuffHead;
        }
        return TRUE;
    }

    // 如果数据头在数据尾右侧或重叠
    else
    {
        // 获取连续空余空间尺寸
        DWORD dwSeriateSize = (DWORD)(m_lpBuffTail - m_pDataHead);

        // 如果连续空余空间够用
        if (dwSize <= dwSeriateSize)
        {
            memcpy(m_pDataHead, lpData, dwSize);
            m_dwCurrentDataSize += dwSize;
            m_pDataHead += dwSize;
            if (m_pDataHead == m_lpBuffTail)
            {
                m_pDataHead = m_lpBuffHead;
            }
            return TRUE;
        }

        // 如果连续空余空间不够用
        else
        {
            LPBYTE lpByte = lpData;
            memcpy(m_pDataHead, lpByte, dwSeriateSize);
            m_dwCurrentDataSize += dwSeriateSize;
            lpByte += dwSeriateSize;
            dwSize -= dwSeriateSize;
            memcpy(m_lpBuffHead, lpByte, dwSize);
            m_dwCurrentDataSize += dwSize;
            m_pDataHead = m_lpBuffHead + dwSize;
            if (m_pDataHead == m_lpBuffTail)
            {
                m_pDataHead = m_lpBuffHead;
            }
            return TRUE;
        }
    }
}

// 弹出数据
VOID MsSendBuffer::Pop(DWORD dwSize)
{
    AUTO_LOCK(m_Lock);

    // 判断是否有足够的数据空间
    if (dwSize > m_dwCurrentDataSize)
    {
        return;
    }

    // 如果数据尾在数据头左侧
    if (m_pDataTail < m_pDataHead)
    {
        _DEBUG_CODE(ZeroMemory(m_pDataTail, dwSize));
        m_dwCurrentDataSize -= dwSize;
        m_pDataTail += dwSize;
        if (m_pDataTail == m_lpBuffTail)
        {
            m_pDataTail = m_lpBuffHead;
        }
    }

    // 如果数据尾在数据头右侧或重叠
    else
    {
        // 获取连续空余空间尺寸
        DWORD dwSeriateSize = (DWORD)(m_lpBuffTail - m_pDataTail);

        // 如果连续数据空间够用
        if (dwSize <= dwSeriateSize)
        {
            _DEBUG_CODE(ZeroMemory(m_pDataTail, dwSize));
            m_dwCurrentDataSize -= dwSize;
            m_pDataTail += dwSize;
            if (m_pDataTail == m_lpBuffTail)
            {
                m_pDataTail = m_lpBuffHead;
            }
        }

        // 如果连续数据空间不够用
        else
        {
            _DEBUG_CODE(ZeroMemory(m_pDataTail, dwSeriateSize));
            m_dwCurrentDataSize -= dwSeriateSize;
            dwSize -= dwSeriateSize;
            _DEBUG_CODE(ZeroMemory(m_pDataTail, dwSize));
            m_dwCurrentDataSize -= dwSize;
            m_pDataTail = m_lpBuffHead + dwSize;
            if (m_pDataTail == m_lpBuffTail)
            {
                m_pDataTail = m_lpBuffHead;
            }
        }
    }
}

// 是否正在发送
BOOL MsSendBuffer::IsSending(VOID)
{
    AUTO_LOCK(m_Lock);

    return m_bSending;
}

// 设置是否正在发送
VOID MsSendBuffer::SetSending(BOOL bSending)
{
    AUTO_LOCK(m_Lock);

    m_bSending = bSending;
}