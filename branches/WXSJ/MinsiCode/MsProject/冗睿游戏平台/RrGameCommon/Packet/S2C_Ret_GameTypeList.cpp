#include "Precompiled.h"
#ifdef MS_RRGAME_CLIENT
#include "GameStation.h"
#endif

BOOL S2C_Ret_GameTypeList::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
#ifdef MS_RRGAME_CLIENT
    g_pCMsWinMain->SetWndName(_T("冗睿服务器,已接收"));
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    USE_PARAM(lpData);
    g_pCGameStation->GetTree()->Reset();
    FOR_x_TO(i, lpData->m_dwCount)
    {
        CGameTypeNode* pTypeNode = g_pCGameStation->GetTree()->AddGameTypeNode();
        if (pTypeNode)
        {
            pTypeNode->Init(&lpData->m_GameTypeInfo[i]);
        }
    }
    return TRUE;
#else
    return FALSE;
#endif
}

