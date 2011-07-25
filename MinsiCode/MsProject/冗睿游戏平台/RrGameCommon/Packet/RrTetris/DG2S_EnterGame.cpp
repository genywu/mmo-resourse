#include "Precompiled.h"
#include "DG2S_EnterGame.h"
BOOL DG2S_EnterGame::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
#ifdef MS_RRGAME_SERVER
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    if (lpData->m_dwData == CONNECT_KEY)
    {
        g_pMsLog->Logf(_T("¶íÂÞË¹·½¿é[%08X]µÇÂ½ÓÎÏ·\n"), lpCMsSocket->GetSocket());
        return TRUE;
    }
    g_pMsLog->Logf(_T("¶íÂÞË¹·½¿é[%08X]µÇÂ½ÓÎÏ·Ê§°Ü:Key´íÎó\n"), lpCMsSocket->GetSocket());
    return FALSE;
#else
    return FALSE;
#endif
}

