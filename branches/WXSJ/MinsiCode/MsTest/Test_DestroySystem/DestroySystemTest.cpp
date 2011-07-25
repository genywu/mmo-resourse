#include "Precompiled.h"
#include "MsIocpManager.h"

// 最大队列长度
#define MAX_QUEUE_LEN   1024
CMsIocpManager g_MsIocp;

DWORD WINAPI WriteFileThread(LPVOID lpParameter)
{
    LPBYTE lpData = NEW BYTE[MB_SIZE*100];

    static INT s_nNumber = 0;
    s_nNumber++;
    TCHAR szTime[MAX_PATH] = {0};

    for (INT i = 0; i < 1; i++)
    {
        ::SleepEx(100, TRUE);

        UPDATA_LOCALTIME;
        _sntprintf_s(
            szTime,
            sizeof(szTime),
            _T("%d%d%d%d")
            _T("%d%d%d%d"),
            LOCALTIME_HOUR, LOCALTIME_MINUTE, 
            LOCALTIME_YEAR + s_nNumber, LOCALTIME_MONTH,
            LOCALTIME_DAY, LOCALTIME_WEEK,
            LOCALTIME_SECOND, LOCALTIME_MILLISECOND
            );

        HANDLE hFileA = ::CreateFile(
            szTime,
            FILE_ALL_ACCESS,
            0,
            NULL,
            OPEN_ALWAYS,
            FILE_FLAG_OVERLAPPED,
            NULL
            );
        g_MsIocp.AssociateDevice(hFileA, (ULONG_PTR)hFileA);
        OVERLAPPED ol;
        ZeroMemoryStruct(ol);

        DWORD dwOffset = 0;

        ol.Offset = 0;
        memset(lpData, 0, sizeof(BYTE) * MB_SIZE * 100);
        DWORD dwWriteSize = sizeof(BYTE) * MB_SIZE * 100;
        if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        {
            DWORD dwErrorCode = ::GetLastError();
            if (ERROR_IO_PENDING == dwErrorCode)
            {
                ::MessageBox(NULL, _T("操作暂未完成0"), _T("提示"), MB_OK);
            }
        }

        dwOffset += sizeof(BYTE) * MB_SIZE * 100;
        ol.Offset = dwOffset;
        memset(lpData, 1, sizeof(BYTE) * MB_SIZE * 100);
        dwWriteSize = sizeof(BYTE) * MB_SIZE * 100;
        if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        {
            DWORD dwErrorCode = ::GetLastError();
            if (ERROR_IO_PENDING == dwErrorCode)
            {
                ::MessageBox(NULL, _T("操作暂未完成1"), _T("提示"), MB_OK);
            }
        }

        dwOffset += sizeof(BYTE) * MB_SIZE * 100;
        ol.Offset = dwOffset;
        memset(lpData, 2, sizeof(BYTE) * MB_SIZE * 100);
        dwWriteSize = sizeof(BYTE)*MB_SIZE*100;
        if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        {
            DWORD dwErrorCode = ::GetLastError();
            if (ERROR_IO_PENDING == dwErrorCode)
            {
                ::MessageBox(NULL, _T("操作暂未完成2"), _T("提示"), MB_OK);
            }
        }

        OVERLAPPED* lpol;
        DWORD dwSumSize = 0;
        while (TRUE)
        {
            dwWriteSize = 0;
            g_MsIocp.GetCompletionStatus((ULONG_PTR*)&hFileA, dwWriteSize, &lpol);
            dwSumSize += dwWriteSize;
            if (dwSumSize >= sizeof(BYTE) * MB_SIZE * 300)
            {
                break;
            }
        }
        {
            INT a = 0;
        }

        //memset(lpData, 2, sizeof(BYTE) * MB_SIZE * 80);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*80;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成2"), _T("提示"), MB_OK);
        //    }
        //}

        //memset(lpData, 3, sizeof(BYTE) * MB_SIZE * 70);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*70;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成3"), _T("提示"), MB_OK);
        //    }
        //}

        //memset(lpData, 4, sizeof(BYTE) * MB_SIZE * 60);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*60;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成4"), _T("提示"), MB_OK);
        //    }
        //}

        //memset(lpData, 5, sizeof(BYTE) * MB_SIZE * 50);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*50;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成5"), _T("提示"), MB_OK);
        //    }
        //}

        //memset(lpData, 6, sizeof(BYTE) * MB_SIZE * 40);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*40;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成6"), _T("提示"), MB_OK);
        //    }
        //}

        //memset(lpData, 7, sizeof(BYTE) * MB_SIZE * 30);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*30;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成7"), _T("提示"), MB_OK);
        //    }
        //}

        //memset(lpData, 8, sizeof(BYTE) * MB_SIZE * 20);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*20;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成8"), _T("提示"), MB_OK);
        //    }
        //}

        //memset(lpData, 9, sizeof(BYTE) * MB_SIZE * 10);
        //dwWriteSize = sizeof(BYTE)*MB_SIZE*10;
        //if (!::WriteFile(hFileA, lpData, dwWriteSize, &dwWriteSize, &ol))
        //{
        //    DWORD dwErrorCode = ::GetLastError();
        //    if (ERROR_IO_PENDING == dwErrorCode)
        //    {
        //        ::MessageBox(NULL, _T("操作暂未完成9"), _T("提示"), MB_OK);
        //    }
        //}
    }

    SAFE_DELETE_ARRAY(lpData);
    return TRUE;
}

INT __fastcall MainCall(INT,LPTCHAR*)
{
    _ENTER_FUN_C;

    MSLOG.SetSaveLog(FALSE);
    MSLOG.SetEnableLog(TRUE);
    g_MsIocp.Init();

    HANDLE hThread = ::CreateThread(NULL, 0, WriteFileThread, NULL, 0, NULL);

    ::WaitForSingleObject(hThread, INFINITE);


    _tsystem(_T("pause"));
    return 0;
    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_NID;
}
