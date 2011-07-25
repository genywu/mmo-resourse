#include "Precompiled.h"
#include "S2C_FlyFile.h"
#include "FileClient.h"
BOOL S2C_FlyFile::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    if (lpData->m_Data)
    {
        if (g_pCFileClient)
        {
            g_pCFileClient->AppendFile(lpData->m_Data, MB_SIZE * 4);
        }
        return TRUE;
    }
    return FALSE;
}

