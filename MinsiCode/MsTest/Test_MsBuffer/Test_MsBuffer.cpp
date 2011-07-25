#include "Precompiled.h"

CMsBuffer* g_pBuffer = NULL;
DWORD g_dwPushNumber = 1;
DWORD g_dwPopNumber = 1;

DWORD CALLBACK SubThread(LPVOID)
{
    DWORD dwNumber;
    FOR_x_TO(i, 1000000000)
    {
        //MsLog << _T("弹出") << L_NEND;
        if (g_pBuffer->CopyData(&dwNumber, sizeof(DWORD)))
        {
            if (g_dwPopNumber++ == dwNumber)
            {
                if (!g_pBuffer->PopData(sizeof(DWORD)))
                {
                    _tsystem(_T("Pause"));
                }
            }
            else
            {
                _tsystem(_T("Pause"));
            }
        }
    }
    return 0;
}

INT MSCALL Test_MsBuffer_Main(INT,LPTCHAR*)
{
    CMsLog MsLog(_T("%s/Log/Test_MsBuffer_Main.log"), FALSE);
    MsLog.SetSaveLog(FALSE);
    //MsLog.SetEnableLog(FALSE);
    MsLog << _T("启动成功") << L_NEND;

    g_pBuffer = NEW CMsBuffer(MB_SIZE * 100, TRUE);
    AssertEx(g_pBuffer, _T("内存不足"));

    HANDLE hThread = ::CreateThread(NULL, 0, SubThread, NULL, 0, NULL);

    FOR_x_TO(i, 1000000000)
    {
        //MsLog << _T("压入") << L_NEND;
        if (g_pBuffer->PushData(&g_dwPushNumber, sizeof(DWORD)))
        {
            g_dwPushNumber++;
        }
    }

    ::WaitForSingleObject(hThread, INFINITE);
    _tsystem(_T("Pause"));
    SAFE_DELETE(g_pBuffer);
    return 0;
}
