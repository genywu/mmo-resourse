#include "Precompiled.h"

PacketHead::PacketHead(
    DWORD wIdentifier,
    PACKET_TYPE ePackType,
    E_KEY_TYPE eKeyType,
    DWORD dwSize
)
: m_dwIdentifier    (wIdentifier)
, m_ePackType       (ePackType)
, m_eKeyType        (eKeyType)
, m_dwPackSize      (dwSize)
, m_i64Time         (0)
{
    ;
}

BOOL PacketHead::CheckRegular(VOID)
{
    return (m_dwIdentifier == PACKET_IDENTIFIER);
}


CMsBasePacket::CMsBasePacket(
    PACKET_TYPE ePackType,
    DWORD dwSize,
    DWORD wIdentifier,
    E_KEY_TYPE eKeyType
    )
: m_PackHead(wIdentifier, ePackType, eKeyType, dwSize)
{
    ;
}

// 锁定封包
VOID CMsBasePacket::Lock(VOID)
{
    m_IsLock = TRUE;
}

// 获取封包尺寸
DWORD CMsBasePacket::GetPacketSize(VOID)
{
    return m_PackHead.m_dwPackSize;
}

// 获取封包类型
PACKET_TYPE CMsBasePacket::GetPacketType(VOID)
{
    return m_PackHead.m_ePackType;
}

// 获取封包时间
INT64 CMsBasePacket::GetPacketTime(VOID)
{
    return m_PackHead.m_i64Time;
}

// 获取封包密钥
E_KEY_TYPE CMsBasePacket::GetPacketKeyType(VOID)
{
    return m_PackHead.m_eKeyType;
}

// 判断封包是否已锁定
BOOL CMsBasePacket::IsLock(VOID)
{
    return m_IsLock;
}

// 检查封包
BOOL CMsBasePacket::CheckRegular(VOID)
{
    return m_PackHead.CheckRegular();
}

// 获取包头地址
lpPacketHead CMsBasePacket::GetPacketHead(DWORD* lpSize)
{
    if (lpSize)
    {
        *lpSize = sizeof(m_PackHead);
    }
    return &m_PackHead;
}

// 重置封包
VOID CMsBasePacket::Reset(VOID)
{
    //ZeroMemoryStruct(MS_PACKET_DATA);
    m_IsLock = FALSE;
}

BOOL CMsBasePacket::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
    AssertEx(AL_SHOW_ONLY, FALSE, _T("非法调用"));
    return FALSE;
}
