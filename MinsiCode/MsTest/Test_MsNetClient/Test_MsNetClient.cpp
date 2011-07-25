#include "Precompiled.h"
#include "MsPacketExample.h"
INT MSCALL Test_MsNetClient_Main(INT,LPTCHAR*)
{
    CMsPacketFactory* lpCMsPacketFactory = NEW CMsPacketFactory(1);
    lpCMsPacketFactory->Register(&g_CMsPacketExample, 1);

    DWORD dwCurrentThreadId = ::GetCurrentThreadId();
    CMsClientNetManager* lpMsClientNetManager = NEW CMsClientNetManager(_T("127.0.0.1"), 9900, dwCurrentThreadId, lpCMsPacketFactory);

    lpMsClientNetManager->Run();

    MSG Msg;
    while (::GetMessage(&Msg, NULL, 0, 0))
    {
        switch (Msg.message)
        {
        case MSM_CLIENT_SOCKET_ON_ENTER:
            {
                g_MsLog << _T("连接服务器成功\n");
            }break;

        case MSM_CLIENT_SOCKET_ON_EXECUTE:
            {
                CMsServerSocket* pMsServerSocket = (CMsServerSocket*)Msg.wParam;
                pMsServerSocket->ExecutePacket(dwCurrentThreadId);
            }break;

        default:
            {
                ;
            }break;
        }
    }

    lpMsClientNetManager->Stop(TRUE, TRUE);
    g_MsLog << _T("程序结束\n");
    _tsystem(_T("Pause"));

    SAFE_DELETE(lpMsClientNetManager);
    SAFE_DELETE(lpCMsPacketFactory);
    return 0;
}
