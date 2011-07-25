#include "Precompiled.h"

// 构造
CMsPacketFactory::CMsPacketFactory(PACKET_TYPE PacketMaxCount)
: m_PacketMaxCount  (PacketMaxCount)
, m_ppPacketPool    (NULL)
{
    if (m_PacketMaxCount)
    {
        m_ppPacketPool = NEW lpCMsBasePacket[m_PacketMaxCount];
        AssertEx(AL_CATAST, m_ppPacketPool, _T("内存不足"));
        ZeroMemory(m_ppPacketPool, sizeof(lpCMsBasePacket) * m_PacketMaxCount);
    }
    else
    {
        AssertEx(AL_CATAST, m_PacketMaxCount, _T("注册的封包个数不能为空"));
    }
}

// 析构
CMsPacketFactory::~CMsPacketFactory(VOID)
{
    SAFE_DELETE_ARRAY(m_ppPacketPool);
    m_PacketMaxCount = 0;
}

// 注册封包
BOOL CMsPacketFactory::Register(lpCMsBasePacket* lppPacket, DWORD dwPacketCount)
{
    DWORD dwPacketType = INVALID_UID;
    if (lppPacket)
    {
        FOR_x_TO(i, dwPacketCount)
        {
            dwPacketType = lppPacket[i]->GetPacketType();
            if (m_ppPacketPool[dwPacketType] != NULL)
            {
                fAssertEx(
					AL_CATAST,
                    FALSE,
                    _T("封包Id重复注册【%d,%d】"),
                    dwPacketType,
                    i
                    );
                return FALSE;
            }
            else
            {
                m_ppPacketPool[dwPacketType] = lppPacket[i];
            }
        }
    }
    return TRUE;
}

// 获取封包默认尺寸
DWORD CMsPacketFactory::GetPacketDefaultSize(PACKET_TYPE PacketId)
{
    CMsBasePacket* pPacket = this->GetPacket(PacketId);
    if (pPacket)
    {
        return pPacket->GetPacketSize();
    }
    return 0;
}

// 获取封包数据默认尺寸
DWORD CMsPacketFactory::GetPacketDataDefaultSize(PACKET_TYPE PacketId)
{
    CMsBasePacket* pPacket = this->GetPacket(PacketId);
    if (pPacket)
    {
        return pPacket->GetPacketSize() - sizeof(PacketHead);
    }
    return 0;
}

// 获取封包
CMsBasePacket* CMsPacketFactory::GetPacket(PACKET_TYPE PacketId)
{
    if (PacketId >= m_PacketMaxCount)
    {
        return NULL;
    }
    else
    {
        return m_ppPacketPool[PacketId];
    }
}

// 解析包头得到封包
CMsBasePacket* CMsPacketFactory::ParsePacketHead(PacketHead* lpPacketHead)
{
    if (lpPacketHead->CheckRegular())
    {
        return this->GetPacket(lpPacketHead->m_ePackType);
    }
    return NULL;
}