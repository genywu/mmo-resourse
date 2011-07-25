#include "Precompiled.h"
#include "S2C_Connect.h"

BOOL S2C_Connect::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    if (lpData->m_bOK)
    {
        _tcout << _T("封包\t") << _T("连接服务器成功\n");
        // 连接服务器成功
        return TRUE;
    }
    return FALSE;
}

