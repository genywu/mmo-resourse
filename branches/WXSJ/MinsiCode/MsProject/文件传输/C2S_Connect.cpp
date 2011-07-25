#include "Precompiled.h"
#include "C2S_Connect.h"
#include "FileServer.h"

BOOL C2S_Connect::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    
    S2C_Connect::MS_PACKET_DATA PacketData;
    if (0 == _tcscmp(lpData->sz_Password, g_pCFileServer->GetPassword()))
    {
        _tcout << _T("封包\t") << _T("登陆服务器成功\n");
        PacketData.m_bOK = TRUE;
        lpCMsSocket->WritePacket(BPT(S2C_Connect), &PacketData);
        return TRUE;
    }
    else
    {
        _tcout << _T("封包\t") << _T("登陆服务器失败\n");
        PacketData.m_bOK = FALSE;
        lpCMsSocket->WritePacket(BPT(S2C_Connect), &PacketData);
        return FALSE;
    }
}

