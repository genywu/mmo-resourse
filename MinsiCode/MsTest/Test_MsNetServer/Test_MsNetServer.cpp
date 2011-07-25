#include "Precompiled.h"
#include "MsPacketExample.h"
INT MSCALL Test_MsNetServer_Main(INT,LPTCHAR*)
{
    CMsPacketFactory* lpCMsPacketFactory = NEW CMsPacketFactory(1);
    lpCMsPacketFactory->Register(&g_CMsPacketExample, 1);

    DWORD dwCurrentThreadId = ::GetCurrentThreadId();
    CMsServerNetManager* lpMsServerNetManager = NEW CMsServerNetManager(_T("127.0.0.1"), 9900, dwCurrentThreadId, 2000, lpCMsPacketFactory);

    lpMsServerNetManager->Run();

    MSG Msg;
    while (::GetMessage(&Msg, NULL, 0, 0))
    {
        switch (Msg.message)
        {
        case MSM_SERVER_SOCKET_ON_ENTER:
            {
                CMsClientSocket* pMsClientSocket = (CMsClientSocket*)Msg.wParam;
                CMsPacketExample Packet;
                Packet.ValidData.m_dwData = 123456789;
                pMsClientSocket->PacketToSend(&Packet);
                g_MsLog << _T("有客户端登陆服务器\n");
            }break;

        case MSM_SERVER_SOCKET_ON_EXECUTE:
            {
                CMsClientSocket* pMsClientSocket = (CMsClientSocket*)Msg.wParam;
                pMsClientSocket->ExecutePacket(dwCurrentThreadId);
            }break;
        default:
            {
                ;
            }break;
        }
    }

    lpMsServerNetManager->Stop(TRUE, TRUE);
    g_MsLog << _T("程序结束\n");
    _tsystem(_T("Pause"));

    SAFE_DELETE(lpMsServerNetManager);
    return 0;
}
