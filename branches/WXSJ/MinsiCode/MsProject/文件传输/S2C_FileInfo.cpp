#include "Precompiled.h"
#include "S2C_FileInfo.h"
#include "FileClient.h"
BOOL S2C_FileInfo::ExecutePacket(CMsSocket* lpCMsSocket)
{
    USE_PARAM(lpCMsSocket);
    LPMS_PACKET_DATA lpData = (LPMS_PACKET_DATA)lpCMsSocket->GetPacketData();
    if (lpData->dwFileSize && lpData->szFileName)
    {
        if (g_pCFileClient)
        {
            g_pCFileClient->CreateFile(lpData->szFileName);
        }
        return TRUE;
    }
    return FALSE;
}

