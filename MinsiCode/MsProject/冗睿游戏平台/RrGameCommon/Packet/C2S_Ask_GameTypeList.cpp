#include "Precompiled.h"
BOOL C2S_Ask_GameTypeList::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
#ifdef MS_RRGAME_SERVER
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    g_pMsLog->Logf(_T("客户端[%08X]请求游戏类型列表\n"), lpCMsSocket->GetSocket());

    S2C_Ret_GameTypeList::MS_PACKET_DATA PacketData;
    PacketData.m_dwCount = 5;
    PacketData.m_dwGameTypeIndex = lpData->m_dwGameTypeIndex;

    PacketData.m_GameTypeInfo[0].m_IsGreenGroup = FALSE;
    _tcscpy_s(PacketData.m_GameTypeInfo[0].m_szGameTypeName, GAME_TYPE_NAME_LEN, _T("竞技类"));
    PacketData.m_GameTypeInfo[0].m_wIconId = 0;

    PacketData.m_GameTypeInfo[1].m_IsGreenGroup = TRUE;
    _tcscpy_s(PacketData.m_GameTypeInfo[1].m_szGameTypeName, GAME_TYPE_NAME_LEN, _T("麻将类"));
    PacketData.m_GameTypeInfo[1].m_wIconId = 1;

    PacketData.m_GameTypeInfo[2].m_IsGreenGroup = FALSE;
    _tcscpy_s(PacketData.m_GameTypeInfo[2].m_szGameTypeName, GAME_TYPE_NAME_LEN, _T("休闲类"));
    PacketData.m_GameTypeInfo[2].m_wIconId = 2;

    PacketData.m_GameTypeInfo[3].m_IsGreenGroup = TRUE;
    _tcscpy_s(PacketData.m_GameTypeInfo[3].m_szGameTypeName, GAME_TYPE_NAME_LEN, _T("音乐类"));
    PacketData.m_GameTypeInfo[3].m_wIconId = 3;

    PacketData.m_GameTypeInfo[4].m_IsGreenGroup = TRUE;
    _tcscpy_s(PacketData.m_GameTypeInfo[4].m_szGameTypeName, GAME_TYPE_NAME_LEN, _T("纸牌类"));
    PacketData.m_GameTypeInfo[4].m_wIconId = 4;

    lpCMsSocket->WritePacket(BPT(S2C_Ret_GameTypeList), &PacketData, sizeof(PacketData));
    return TRUE;
#else
    return FALSE;
#endif
}

