#include "Precompiled.h"
#include "MsPacketExample.h"

CMsPacketExample g_CMsPacketExample;
CMsPacketExample::CMsPacketExample(VOID)
: CMsBasePacket(0, sizeof(MS_PACKET_DATA) + sizeof(PacketHead))
{
    ;
}

// 获取有效数据地址
LPVOID CMsPacketExample::GetValidPacketData(DWORD* lpSize)
{
    *lpSize = sizeof(ValidData);
    return &ValidData;
}

// 获取有效数据尺寸
DWORD CMsPacketExample::GetValidPacketDataSize(VOID)
{
    return sizeof(ValidData);
}

BOOL CMsPacketExample::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketBuff();
    USE_PARAM(lpData);
#ifdef MS_TEST_SERVER
    //g_MsLog.Logf(_T("接收到客户端的数据【%d】"), lpData->m_dwData);
    return TRUE;
#else
    //g_MsLog.Logf(_T("接收到服务器的数据【%d】"), lpData->m_dwData);
    return TRUE;
#endif
}

