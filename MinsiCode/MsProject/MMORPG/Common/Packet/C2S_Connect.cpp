#include "Precompiled.h"
BOOL C2S_Connect::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
#ifdef MS_RRGAME_SERVER
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    if (lpData->m_dwData == CONNECT_KEY)
    {
        g_pMsLog->Logf(_T("客户端[%08X]进入服务器\n"), lpCMsSocket->GetSocket());
        S2C_Connect S2C_Packet;
        S2C_Connect::MS_PACKET_DATA PacketData;
        PacketData.m_dwData = CONNECT_KEY;
        lpCMsSocket->WritePacket(BPT(S2C_Connect), &S2C_Packet, sizeof(PacketData));
        return TRUE;
    }
    g_pMsLog->Logf(_T("客户端[%08X]进入服务器但Key错误\n"), lpCMsSocket->GetSocket());
    return FALSE;
#else
    return FALSE;
#endif
}

