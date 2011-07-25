#include "Precompiled.h"

CMsConsole* CMsLog::s_pCMsConsole = NULL;   // 日志控制台对象
DWORD       CMsLog::s_dwLogCount  = 0;      // 日志实例个数
MsLock      CMsLog::s_LogLock;              // 日志大锁
//MsLock      CMsLog::m_ShowLock;             // 显示锁


CMsLog::CMsLog(LOG_FILE_INFO* pLogFileInfo, DWORD dwLogFileCount, BOOL bLock)
: m_dwFileIndex         (0)
, m_bSaveLog            (TRUE)
, m_QuondamValueSaveLog (INVALID_NID)
, m_bEnable             (TRUE)
, m_pLogFileInfo        (NULL)
, m_dwLogFileCount      (dwLogFileCount)
, m_bPause              (FALSE)
, m_bfIntensity         (FALSE)
, m_bLock               (bLock)
{
    //AUTO_LOCK(s_Loglock);
    if (!s_dwLogCount)
    {
        s_pCMsConsole = NEW CMsConsole(NULL);
        AssertEx(AL_CATAST, s_pCMsConsole, _T("内存不足"));
    }
    s_dwLogCount++;

    m_pLogFileInfo = NEW LOG_FILE_INFO[dwLogFileCount];
    if (pLogFileInfo && m_dwLogFileCount && m_pLogFileInfo)
    {
        ZeroMemory(m_pLogFileInfo, sizeof(LOG_FILE_INFO) * dwLogFileCount);
        FOR_x_TO(i, dwLogFileCount)
        {
            if (pLogFileInfo->m_szLogFileName)
            {
                BOOL bRet = CreateFullPath(m_pLogFileInfo[i].m_szLogFileName, pLogFileInfo->m_szLogFileName);
                AssertEx(AL_NORMAL, bRet, _T("解析日志文件路径失败"));
            }
            else
            {
                AssertEx(AL_NORMAL, pLogFileInfo->m_szLogFileName, _T("日志路径不能为空"));
            }
        }
    }
    else
    {
        AssertEx(AL_NORMAL, m_dwLogFileCount, _T("日志信息不能为空"));
        AssertEx(AL_NORMAL, pLogFileInfo, _T("日志信息不能为空"));
        AssertEx(AL_CATAST, m_pLogFileInfo, _T("内存不足"));
    }

    this->Init();
}

CMsLog::CMsLog(TCHAR* szLogFileName, BOOL bLock)
: m_dwFileIndex         (0)
, m_bSaveLog            (TRUE)
, m_QuondamValueSaveLog (INVALID_NID)
, m_bEnable             (TRUE)
, m_pLogFileInfo        (NULL)
, m_dwLogFileCount      (1)
, m_bPause              (FALSE)
, m_bfIntensity         (FALSE)
, m_bLock               (bLock)
{
    //AUTO_LOCK(s_Loglock);
    if (!s_dwLogCount)
    {
        s_pCMsConsole = NEW CMsConsole(NULL);
        AssertEx(AL_CATAST, s_pCMsConsole, _T("内存不足"));
    }
    s_dwLogCount++;

    m_pLogFileInfo = NEW LOG_FILE_INFO;
    if (szLogFileName && m_pLogFileInfo)
    {
        BOOL bRet = CreateFullPath(m_pLogFileInfo->m_szLogFileName, szLogFileName);
        AssertEx(AL_NORMAL, bRet, _T("解析日志文件路径失败"));
        m_pLogFileInfo->m_hLogFile = NULL;
    }
    else
    {
        AssertEx(AL_CATAST, m_pLogFileInfo, _T("内存不足"));
        AssertEx(AL_NORMAL, szLogFileName, _T("日志信息不能为空"));
    }

    this->Init();
}

CMsLog::~CMsLog(VOID)
{
    this->Release();
    //AUTO_LOCK(s_Loglock);
    s_dwLogCount--;
    if (!s_dwLogCount)
    {
        SAFE_DELETE(s_pCMsConsole);
    }
}

BOOL CMsLog::Init(VOID)
{
    _ENTER_FUN_C;

    //BOOL bRet = TRUE;

    TCHAR szLog_t[MAX_PATH] = {0};
    ::CreateDirectory(_T("./Log/"), NULL);

    for (UINT i = 0; i < m_dwLogFileCount; i++)
    {
        if (!m_pLogFileInfo[i].m_hLogFile)
        {
            m_pLogFileInfo[i].m_hLogFile = ::CreateFile(
                m_pLogFileInfo[i].m_szLogFileName,
                FILE_APPEND_DATA,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                NULL,
                NULL
                );
        }
        if (INVALID_HANDLE_VALUE == m_pLogFileInfo[i].m_hLogFile)
        {
            if (ERROR_FILE_NOT_FOUND == ::GetLastError())
            {
                m_pLogFileInfo[i].m_hLogFile = ::CreateFile(
                    m_pLogFileInfo[i].m_szLogFileName,
                    FILE_APPEND_DATA,
                    FILE_SHARE_READ,
                    NULL,
                    CREATE_NEW,
                    NULL,
                    NULL
                    );
                if (INVALID_HANDLE_VALUE != m_pLogFileInfo[i].m_hLogFile)
                {
#ifdef UNICODE
                    TCHAR UnicodeHead = 0xFEFF;
                    DWORD uLogSize = sizeof(UnicodeHead);

                    if (::WriteFile(
                        m_pLogFileInfo[i].m_hLogFile,
                        &UnicodeHead,uLogSize,
                        &uLogSize,
                        NULL
                        ))
                    {
                        continue;
                    }
                    else
                    {
                        _stprintf_s(
                            szLog_t,
                            MAX_PATH,
                            _T("%s打开失败！！！\n[%s]"),
                            m_pLogFileInfo[i].m_szLogFileName,
                            this->ShowLastError()
                            );
                        MESSAGEBOX(AL_SHOW_ONLY, szLog_t);
                    }
#endif
                }
                else
                {
                    _stprintf_s(
                        szLog_t,
                        MAX_PATH,
                        _T("%s打开失败！！！\n[%s]"),
                        m_pLogFileInfo[i].m_szLogFileName,
                        this->ShowLastError()
                        );
                    MESSAGEBOX(AL_SHOW_ONLY, szLog_t);
                }
            }
            else
            {
                _stprintf_s(
                    szLog_t,
                    MAX_PATH,
                    _T("%s打开失败！！！\n[%s]"),
                    m_pLogFileInfo[i].m_szLogFileName,
                    this->ShowLastError()
                    );
                MESSAGEBOX(AL_SHOW_ONLY, szLog_t);
            }
        }
    }

    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}


BOOL CMsLog::Release(VOID)
{
    for (UINT i = 0; i < m_dwLogFileCount; i++)
    {
        ::CloseHandle(m_pLogFileInfo[i].m_hLogFile);
    }

    if (1 == m_dwLogFileCount)
    {
        SAFE_DELETE(m_pLogFileInfo);
    }
    else
    {
        SAFE_DELETE_ARRAY(m_pLogFileInfo);
    }
    return TRUE;
}

BOOL CMsLog::SendLogWPrivate(DWORD /*dwLogIndex*/, WCHAR* /*szLog*/)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CMsLog::SendLogAPrivate(DWORD /*dwLogIndex*/, ACHAR* /*szLog*/)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CMsLog::ShowLogWPrivate(WCHAR* szLog)
{
    USE_PARAM(szLog);
    _BENTER_FUN_C(m_bEnable, TRUE);

    if (m_bPause)
    {
        this->UnPause();
        NO_MFC_CODE(std::wcout << szLog);
        this->RePause();
    }
    else
    {
        NO_MFC_CODE(std::wcout << szLog);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CMsLog::ShowLogAPrivate(ACHAR* szLog)
{
    USE_PARAM(szLog);
    _BENTER_FUN_C(m_bEnable, TRUE);

    if (m_bPause)
    {
        this->UnPause();
        NO_MFC_CODE(std::cout << szLog);
        this->RePause();
    }
    else
    {
        NO_MFC_CODE(std::cout << szLog);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CMsLog::WriteLogWPrivate(DWORD dwLogIndex, WCHAR* szLog)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    AssertEx(AL_NORMAL, dwLogIndex < m_dwLogFileCount, _T("日志索引错误"));

    BOOL bRet = TRUE;
    DWORD dwEnd = 0x000A000D;
    DWORD dwLogLen = (DWORD)wcslen(szLog);

    if (m_pLogFileInfo[dwLogIndex].m_hLogFile)
    {
        if (_T('\n') == szLog[dwLogLen - 1])
        {
            dwLogLen--;
            bRet = ::WriteFile(
                m_pLogFileInfo[dwLogIndex].m_hLogFile,
                szLog,
                (dwLogLen * (DWORD)sizeof(WCHAR)),
                &dwLogLen,
                NULL
                );

            fAssertEx(
				AL_NORMAL,
                bRet,
                _T("日志内容保存失败\n%s"),
                this->ShowLastError()
                );

            bRet = ::WriteFile(
                m_pLogFileInfo[dwLogIndex].m_hLogFile,
                &dwEnd,
                sizeof(dwEnd),
                &dwLogLen,
                NULL
                );

            fAssertEx(
				AL_NORMAL,
				bRet,
                _T("日志内容保存失败\n%s"),
                this->ShowLastError()
                );
        }
        else
        {
            if (m_pLogFileInfo[dwLogIndex].m_hLogFile)
            {
                bRet = ::WriteFile(
                    m_pLogFileInfo[dwLogIndex].m_hLogFile,
                    szLog,
                    (dwLogLen * (DWORD)sizeof(WCHAR)),
                    &dwLogLen,
                    NULL
                    );

                fAssertEx(
					AL_NORMAL,
					bRet,
                    _T("日志内容保存失败\n%s"),
                    this->ShowLastError()
                    );

                ::FlushFileBuffers(m_pLogFileInfo[dwLogIndex].m_hLogFile);
            }
            else
            {
                AssertEx(AL_NORMAL, m_pLogFileInfo[dwLogIndex].m_hLogFile, _T("错误逻辑"));
            }
        }
    }
    else
    {
        AssertEx(AL_NORMAL, m_pLogFileInfo[dwLogIndex].m_hLogFile, _T("日志文件不存在"));
    }

    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CMsLog::WriteLogAPrivate(DWORD dwLogIndex, ACHAR* szLog)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    AssertEx(AL_NORMAL, dwLogIndex < m_dwLogFileCount, _T("日志索引错误"));
    AssertEx(AL_NORMAL, m_pLogFileInfo[dwLogIndex].m_hLogFile, _T("日志文件不存在"));

#ifdef UNICODE
    ACharToWChar(szLog, m_szLog, LOG_STR_MAX_LEN);
    return this->WriteLogWPrivate(dwLogIndex, m_szLog);
#else
    DWORD dwLogLen = (DWORD)(strlen(szLog) * sizeof(ACHAR));
    BOOL bRet = TRUE;
    if (m_pLogFileInfo[dwLogIndex].m_hLogFile)
    {
        bRet = ::WriteFile(
            m_pLogFileInfo[dwLogIndex].m_hLogFile,
            szLog,
            dwLogLen,
            &dwLogLen,
            NULL
            );

        AssertEx(AL_NORMAL, bRet, _T("日志内容保存失败"));
        ::FlushFileBuffers(m_pLogFileInfo[dwLogIndex].m_hLogFile);
    }
    else
    {
        AssertEx(m_pLogFileInfo[dwLogIndex].m_hLogFile, _T("错误逻辑"));
    }
    return TRUE;
#endif

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

inline BOOL CMsLog::SendLogW(DWORD dwLogIndex, WCHAR* szLog)
{
    if (!m_bEnable)return TRUE;
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    BOOL bRet;
    if (m_bLock)
    {
        s_LogLock.Lock();
        bRet = this->SendLogWPrivate(dwLogIndex, szLog);
        s_LogLock.UnLock();
    }
    else
    {
        bRet = this->SendLogWPrivate(dwLogIndex, szLog);
    }
    return bRet;
}

inline BOOL CMsLog::SendLogA(DWORD dwLogIndex, ACHAR* szLog)
{
    if (!m_bEnable)return TRUE;
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    BOOL bRet = this->SendLogAPrivate(dwLogIndex, szLog);
    return bRet;
}

inline BOOL CMsLog::ShowLogW(WCHAR* szLog)
{
    if (!m_bEnable)return TRUE;
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    BOOL bRet = this->ShowLogWPrivate(szLog);
    return bRet;
}
inline BOOL CMsLog::ShowLogA(ACHAR* szLog)
{
    if (!m_bEnable)return TRUE;
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    BOOL bRet = this->ShowLogAPrivate(szLog);
    return bRet;
}

inline BOOL CMsLog::WriteLogA(DWORD dwLogIndex, ACHAR* szLog)
{
    if (!m_bEnable)return TRUE;
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    BOOL bRet = this->WriteLogAPrivate(dwLogIndex, szLog);
    return bRet;
}

inline BOOL CMsLog::WriteLogW(DWORD dwLogIndex, WCHAR* szLog)
{
    if (!m_bEnable)return TRUE;
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    BOOL bRet = this->WriteLogWPrivate(dwLogIndex, szLog);
    return bRet;
}

BOOL CMsLog::TimeW(DWORD dwLogIndex, BOOL bSaveLog)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    WCHAR szTemp[MAX_PATH] = {0};

    UPDATA_LOCALTIME;
    _snwprintf_s(
        szTemp,
        MAX_PATH,
        L"[%04d-%02d-%02d "         // 年月日
        L"%02d:%02d:%02d.%03d]",    // 时分秒毫秒
        LOCALTIME_YEAR,
        LOCALTIME_MONTH,
        LOCALTIME_DAY,
        LOCALTIME_HOUR,
        LOCALTIME_MINUTE,
        LOCALTIME_SECOND,
        LOCALTIME_MILLISECOND
        );

    this->ShowLogW(szTemp);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogW(dwLogIndex, szTemp);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CMsLog::TimeA(DWORD dwLogIndex, BOOL bSaveLog)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    ACHAR szTemp[MAX_PATH] = {0};

    UPDATA_LOCALTIME;

    _snprintf_s(
        szTemp,
        MAX_PATH,
        "[%04d-%02d-%02d "          // 年月日
        "%02d:%02d:%02d.%03d]",     // 时分秒毫秒
        LOCALTIME_YEAR,
        LOCALTIME_MONTH,
        LOCALTIME_DAY,
        LOCALTIME_HOUR,
        LOCALTIME_MINUTE,
        LOCALTIME_SECOND,
        LOCALTIME_MILLISECOND
        );

    this->ShowLogA(szTemp);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogA(dwLogIndex, szTemp);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

inline BOOL CMsLog::SendLog(DWORD dwLogIndex, TCHAR* szLog)
{
    if (!m_bEnable)return TRUE;
#ifdef UNICODE
    return this->SendLogW(dwLogIndex, szLog);
#else
    return this->SendLogA(dwLogIndex, szLog);
#endif
}

inline BOOL CMsLog::ShowLog(TCHAR* szLog)
{
    if (!m_bEnable)return TRUE;
#ifdef UNICODE
    return this->ShowLogW(szLog);
#else
    return this->ShowLogA(szLog);
#endif
}

inline BOOL CMsLog::WriteLog(DWORD dwLogIndex, TCHAR* szLog)
{
    if (!m_bEnable)return TRUE;
#ifdef UNICODE
    return this->WriteLogW(dwLogIndex, szLog);
#else
    return this->WriteLogA(dwLogIndex, szLog);
#endif
}

//BOOL CMsLog::WriteLogf(DWORD dwLogIndex, TCHAR* szFomat, ...)
//{
//    if (!m_bEnable)return TRUE;
//    va_list ap;
//    _ENTER_FUN;
//    BOOL bRet = TRUE;
//
//    va_start(ap, szFomat);
//    TCHAR szStrBuff[MAX_PATH] = {0};
//    _vsntprintf_s(szStrBuff, MAX_PATH, szFomat, ap);
//    bRet = this->WriteLog(dwLogIndex, szStrBuff);
//    va_end(ap);
//
//    return bRet;
//    _LEAVE_FUN_ONLY_CUE;
//    va_end(ap);
//    return FALSE;
//}
//
//BOOL CMsLog::WriteLogfW(DWORD dwLogIndex, WCHAR* szFomat, ...)
//{
//    if (!m_bEnable)return TRUE;
//    va_list ap;
//    va_start(ap, szFomat);
//    BOOL bRet = TRUE;
//    WCHAR szStrBuff[MAX_PATH] = {0};
//    _vsnwprintf_s(szStrBuff, MAX_PATH, szFomat, ap);
//    bRet = this->WriteLogW(dwLogIndex, szStrBuff);
//    va_end(ap);
//    return bRet;
//}
//
//BOOL CMsLog::WriteLogfA(DWORD dwLogIndex, ACHAR* szFomat, ...)
//{
//    if (!m_bEnable)return TRUE;
//    va_list ap;
//    va_start(ap, szFomat);
//    BOOL bRet = TRUE;
//    ACHAR szStrBuff[MAX_PATH] = {0};
//    _vsnprintf_s(szStrBuff, MAX_PATH, szFomat, ap);
//    bRet = this->WriteLogA(dwLogIndex, szStrBuff);
//    va_end(ap);
//    return bRet;
//}
//
//BOOL CMsLog::WriteLogf(DWORD dwLogIndex, TCHAR* szFomat, va_list ap)
//{
//    if (!m_bEnable)return TRUE;
//    _ENTER_FUN;
//    TCHAR szStrBuff[MAX_PATH] = {0};
//    _vsntprintf_s(szStrBuff, MAX_PATH, szFomat, ap);
//    return this->WriteLog(dwLogIndex, szStrBuff);
//    _LEAVE_FUN_ONLY_CUE;
//    return FALSE;
//}
//
//BOOL CMsLog::WriteLogfW(DWORD dwLogIndex, WCHAR* szFomat, va_list ap)
//{
//    if (!m_bEnable)return TRUE;
//    _ENTER_FUN;
//    WCHAR szStrBuff[MAX_PATH] = {0};
//    _vsnwprintf_s(szStrBuff, MAX_PATH, szFomat, ap);
//    return this->WriteLogW(dwLogIndex, szStrBuff);
//    _LEAVE_FUN_ONLY_CUE;
//    return FALSE;
//}
//
//BOOL CMsLog::WriteLogfA(DWORD dwLogIndex, ACHAR* szFomat, va_list ap)
//{
//    if (!m_bEnable)return TRUE;
//    _ENTER_FUN;
//    ACHAR szStrBuff[MAX_PATH] = {0};
//    _vsnprintf_s(szStrBuff, MAX_PATH, szFomat, ap);
//    return this->WriteLogA(dwLogIndex, szStrBuff);
//    _LEAVE_FUN_ONLY_CUE;
//    return FALSE;
//}



BOOL CMsLog::Time(DWORD dwLogIndex, BOOL bSaveLog)
{
    if (!m_bEnable)return TRUE;
#ifdef UNICODE
    return this->TimeW(dwLogIndex, bSaveLog);
#else
    return this->TimeA(dwLogIndex, bSaveLog);
#endif
}

BOOL CMsLog::Log(DWORD dwLogIndex, BOOL bSaveLog, TCHAR* strInof)
{
    if (!m_bEnable)return TRUE;
#ifdef UNICODE
    return this->LogW(dwLogIndex, bSaveLog, strInof);
#else
    return this->LogA(dwLogIndex, bSaveLog, strInof);
#endif
}

BOOL CMsLog::LogW(DWORD dwLogIndex, BOOL bSaveLog, WCHAR* strInof)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    this->ShowLogW(strInof);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogW(dwLogIndex, strInof);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CMsLog::LogA(DWORD dwLogIndex, BOOL bSaveLog, ACHAR* strInof)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    this->ShowLogA(strInof);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogA(dwLogIndex, strInof);
    }
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

BOOL CMsLog::Logf(DWORD dwLogIndex, BOOL bSaveLog, TCHAR* strInof, ...)
{
    if (!m_bEnable)return TRUE;
    va_list ap;
    va_start(ap, strInof);
#ifdef UNICODE
    return this->LogfW(dwLogIndex, bSaveLog, strInof, ap);
#else
    return this->LogfA(dwLogIndex, bSaveLog, strInof, ap);
#endif
}

BOOL CMsLog::LogfW(DWORD dwLogIndex, BOOL bSaveLog, WCHAR* strInof, ...)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    va_list ap;
    va_start(ap, strInof);

    WCHAR szStrBuff[MAX_PATH] = {0};
    _vsnwprintf_s(szStrBuff, MAX_PATH, strInof, ap);

    this->ShowLogW(szStrBuff);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogW(dwLogIndex, szStrBuff);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

BOOL CMsLog::LogfA(DWORD dwLogIndex, BOOL bSaveLog, ACHAR* strInof, ...)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    va_list ap;
    va_start(ap, strInof);

    ACHAR szStrBuff[MAX_PATH] = {0};
    _vsnprintf_s(szStrBuff, MAX_PATH, strInof, ap); 

    this->ShowLogA(szStrBuff);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogA(dwLogIndex, szStrBuff);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

BOOL CMsLog::Logf(DWORD dwLogIndex, BOOL bSaveLog, TCHAR* strInof, va_list ap)
{
    if (!m_bEnable)return TRUE;
#ifdef UNICODE
    return this->LogfW(dwLogIndex, bSaveLog, strInof, ap);
#else
    return this->LogfA(dwLogIndex, bSaveLog, strInof, ap);
#endif
}

BOOL CMsLog::LogfW(DWORD dwLogIndex, BOOL bSaveLog, WCHAR* strInof, va_list ap)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    WCHAR szStrBuff[MAX_PATH] = {0};
    _vsnwprintf_s(szStrBuff, MAX_PATH, strInof, ap);

    this->ShowLogW(szStrBuff);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogW(dwLogIndex, szStrBuff);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

BOOL CMsLog::LogfA(DWORD dwLogIndex, BOOL bSaveLog, ACHAR* strInof, va_list ap)
{
    _BENTER_FUN_C(m_bEnable, TRUE);

    ACHAR szStrBuff[MAX_PATH] = {0};
    _vsnprintf_s(szStrBuff, MAX_PATH, strInof, ap); 

    this->ShowLogA(szStrBuff);

    if (COMMAND_NORMAL_LOG == g_Command_Assert_Log && bSaveLog)
    {
        this->WriteLogA(dwLogIndex, szStrBuff);
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

BOOL CMsLog::Time(VOID)
{
    return this->Time(m_dwFileIndex, m_bSaveLog);
}

BOOL CMsLog::TimeW(VOID)
{
    return this->TimeW(m_dwFileIndex, m_bSaveLog);
}

BOOL CMsLog::TimeA(VOID)
{
    return this->TimeA(m_dwFileIndex, m_bSaveLog);
}

BOOL CMsLog::Log(TCHAR* strInof)
{
    return this->Log(m_dwFileIndex, m_bSaveLog, strInof);
}

BOOL CMsLog::LogW(WCHAR* strInof)
{
    return this->LogW(m_dwFileIndex, m_bSaveLog, strInof);
}

BOOL CMsLog::LogA(ACHAR* strInof)
{
    return this->LogA(m_dwFileIndex, m_bSaveLog, strInof);
}

BOOL CMsLog::Logf(TCHAR* strInof, ...)
{
    if (!m_bEnable)return TRUE;
    va_list ap;
    va_start(ap, strInof);
#ifdef UNICODE
    return this->LogfW(m_dwFileIndex, m_bSaveLog, strInof, ap);
#else
    return this->LogfA(m_dwFileIndex, m_bSaveLog, strInof, ap);
#endif
}

BOOL CMsLog::LogfW(WCHAR* strInof, ...)
{
    va_list ap;
    va_start(ap, strInof);
    return this->LogfW(strInof, ap);
}

BOOL CMsLog::LogfA(ACHAR* strInof, ...)
{
    va_list ap;
    va_start(ap, strInof);
    return this->LogfA(strInof, ap);
}

BOOL CMsLog::Logf(TCHAR* strInof, va_list ap)
{
    if (!m_bEnable)return TRUE;
#ifdef UNICODE
    return this->LogfW(m_dwFileIndex, m_bSaveLog, strInof, ap);
#else
    return this->LogfA(m_dwFileIndex, m_bSaveLog, strInof, ap);
#endif
}

BOOL CMsLog::LogfW(WCHAR* strInof, va_list ap)
{
    return this->LogfW(m_dwFileIndex, m_bSaveLog, strInof, ap);
}

BOOL CMsLog::LogfA(ACHAR* strInof, va_list ap)
{
    return this->LogfA(m_dwFileIndex, m_bSaveLog, strInof, ap);
}

CMsLog& CMsLog::operator << (WCHAR Inof)
{
    this->LogfW(m_dwFileIndex, m_bSaveLog, L"%uc", Inof);
    return *this;
}

CMsLog& CMsLog::operator << (ACHAR Inof)
{
    this->LogfA(m_dwFileIndex, m_bSaveLog, "%c", Inof);
    return *this;
}

CMsLog& CMsLog::operator << (WCHAR* strInof)
{
    this->LogW(m_dwFileIndex, m_bSaveLog, strInof);
    return *this;
}

CMsLog& CMsLog::operator << (ACHAR* strInof)
{
    this->LogA(m_dwFileIndex, m_bSaveLog, strInof);
    return *this;
}

CMsLog& CMsLog::operator << (INT nInof)
{
    this->Logf(m_dwFileIndex, m_bSaveLog, _T("%d"), nInof);
    return *this;
}

CMsLog& CMsLog::operator << (DWORD dwInof)
{
    this->Logf(m_dwFileIndex, m_bSaveLog, _T("%u"), dwInof);
    return *this;
}

CMsLog& CMsLog::operator << (INT64 n64Inof)
{
    TCHAR szi64Buff[N64_TO_S_MAX_LEN_BY_R10] = {0};
    _i64tot_s(n64Inof, szi64Buff, N64_TO_S_MAX_LEN_BY_R10, RADIX_10);
    this->Log(m_dwFileIndex, m_bSaveLog, szi64Buff);
    return *this;
}

CMsLog& CMsLog::operator << (UINT64 u64Inof)
{
    TCHAR szi64Buff[N64_TO_S_MAX_LEN_BY_R10] = {0};
    _ui64tot_s(u64Inof, szi64Buff, N64_TO_S_MAX_LEN_BY_R10, RADIX_10);
    this->Log(m_dwFileIndex, m_bSaveLog, szi64Buff);
    return *this;
}

CMsLog& CMsLog::operator << (DOUBLE dInof)
{
    this->Logf(m_dwFileIndex, m_bSaveLog, _T("%lf"), dInof);
    return *this;
}

CMsLog& CMsLog::operator << (FLOAT fInof)
{
    this->Logf(m_dwFileIndex, m_bSaveLog, _T("%f"), fInof);
    return *this;
}

CMsLog& CMsLog::operator << (CMSLOG_FLAG LogFlag)
{
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    switch (LogFlag)
    {
    case CMSLOG_FLAG_LOCK:
        {
            m_Loglock.Lock();
        }break;
    case CMSLOG_FLAG_UNLOCK:
        {
            m_Loglock.UnLock();
        }break;
    case CMSLOG_FLAG_LOCK_A:
        {
            s_LogLock.Lock();
        }break;
    case CMSLOG_FLAG_UNLOCK_A:
        {
            s_LogLock.UnLock();
        }break;
    case CMSLOG_FLAG_TIME:
        {
            this->Time(m_dwFileIndex, m_bSaveLog);
        }break;
    case CMSLOG_FLAG_TIMEW:
        {
            this->TimeW(m_dwFileIndex, m_bSaveLog);
        }break;
    case CMSLOG_FLAG_TIMEA:
        {
            this->TimeA(m_dwFileIndex, m_bSaveLog);
        }break;
    case CMSLOG_FLAG_NOSAVE:
        {
            if (INVALID_NID == m_QuondamValueSaveLog)
            {
                m_QuondamValueSaveLog = m_bSaveLog;
            }
            m_bSaveLog = FALSE;
        }break;
    case CMSLOG_FLAG_SAVE:
        {
            if (INVALID_NID == m_QuondamValueSaveLog)
            {
                m_QuondamValueSaveLog = m_bSaveLog;
            }
            m_bSaveLog = TRUE;
        }break;
    case CMSLOG_FLAG_NEWLINE:
        {
            this->Log(m_dwFileIndex, m_bSaveLog, _T("\n"));
        }break;
        //case CMSLOG_FLAG_BEGIN:
        //    {
        //        m_Loglock.Lock();
        //    }break;
    case CMSLOG_FLAG_END:
        {
            if (INVALID_NID != m_QuondamValueSaveLog)
            {
                m_bSaveLog = m_QuondamValueSaveLog;
                m_QuondamValueSaveLog = INVALID_NID;
            }
        }break;
    case CMSLOG_FLAG_NEWLINE_END:
        {
            this->Log(m_dwFileIndex, m_bSaveLog, _T("\n"));
            if (INVALID_NID != m_QuondamValueSaveLog)
            {
                m_bSaveLog = m_QuondamValueSaveLog;
                m_QuondamValueSaveLog = INVALID_NID;
            }
        }
    case CMSLOG_FLAG_TIME_BEGIN:
        {
            SYSTEMTIME st;
            ::GetLocalTime(&st);
            ::SystemTimeToFileTime(&st, (LPFILETIME)&m_u64TimeBegin);
        }break;
    case CMSLOG_FLAG_TIME_END:
        {
            UINT64 u64TimeEnd;
            SYSTEMTIME st;
            ::GetLocalTime(&st);
            ::SystemTimeToFileTime(&st, (LPFILETIME)&u64TimeEnd);

            DWORD dwTimeDifference = 
                (DWORD)((u64TimeEnd - m_u64TimeBegin) / 10000);

            this->Logf(m_dwFileIndex, m_bSaveLog, _T("%u"), dwTimeDifference);
        }break;
    case CMSLOG_FLAG_TIME_ENDA:
        {
            UINT64 u64TimeEnd;
            SYSTEMTIME st;
            ::GetLocalTime(&st);
            ::SystemTimeToFileTime(&st, (LPFILETIME)&u64TimeEnd);

            DWORD dwTimeDifference = 
                (DWORD)((u64TimeEnd - m_u64TimeBegin) / 10000);

            this->LogfA(m_dwFileIndex, m_bSaveLog, "%u", dwTimeDifference);
        }break;
    case CMSLOG_FLAG_TIME_ENDW:
        {
            UINT64 u64TimeEnd;
            SYSTEMTIME st;
            ::GetLocalTime(&st);
            ::SystemTimeToFileTime(&st, (LPFILETIME)&u64TimeEnd);

            DWORD dwTimeDifference = 
                (DWORD)((u64TimeEnd - m_u64TimeBegin) / 10000);

            this->LogfW(m_dwFileIndex, m_bSaveLog, L"%u", dwTimeDifference);
        }break;
    case CMSLOG_FLAG_PAUSE:
        {
            this->Pause();
        }break;
    case CMSLOG_FLAG_PAUSE_REPEAT:
        {
            this->Pause(TRUE);
        }break;
    case CMSLOG_FLAG_FC_FINTENSITY:
        {
            m_bfIntensity = TRUE;
        }break;
    case CMSLOG_FLAG_FC_UN_FINTENSITY:
        {
            m_bfIntensity = FALSE;
        }break;
    case CMSLOG_FLAG_FC_COLOR_BLACK:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_BLACK, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_WHITE:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_WHITE, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_BLUE:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_BLUE, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_GREEN:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_GREEN, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_RED:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_RED, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_BLUE_GREEN:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_BLUE_GREEN, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_BLUE_RED:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_BLUE_RED, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_GREEN_RED:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(FC_GREEN_RED, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    case CMSLOG_FLAG_FC_COLOR_RV:
        {
            if (s_pCMsConsole)
            {
                s_pCMsConsole->SetSithTextColor(CL_RV, BC_BLACK, m_bfIntensity, FALSE);
            }
        }break;
    }
    return *this;
}

VOID CMsLog::Pause(BOOL bRepeatShow)
{
    if (s_pCMsConsole)
    {
        m_Loglock.Lock();
        s_LogLock.Lock();
        s_pCMsConsole->SetSithTextColor(FC_RED, BC_BLACK, TRUE, FALSE);
        NO_MFC_CODE(_tcout << _T("按任意键继续..."));
        s_pCMsConsole->SetSithTextColor(FC_WHITE, BC_BLACK, TRUE, FALSE);
        if (bRepeatShow)
        {
            m_bPause = TRUE;
        }
        m_Loglock.UnLock();
        s_LogLock.UnLock();
        INT nRet = _gettch();
        if (!nRet)
        {
            AssertEx(AL_NORMAL, nRet, _T("_gettch失败"));
        }

        m_bPause = FALSE;
    }
}

VOID CMsLog::UnPause(VOID)
{
    if (m_bPause)
    {
        NO_MFC_CODE(
        _tcout 
            << 
            _T("\b\b\b\b\b")
            _T("\b\b\b\b\b")
            _T("\b\b\b\b\b")
            << 
            _T("     ")
            _T("     ")
            _T("     ")
            <<
            _T("\b\b\b\b\b")
            _T("\b\b\b\b\b")
            _T("\b\b\b\b\b"));
    }
}

VOID CMsLog::RePause(VOID)
{
    if (s_pCMsConsole)
    {
        if (m_bPause)
        {
            s_pCMsConsole->SetSithTextColor(FC_RED, BC_BLACK, TRUE, FALSE);
            NO_MFC_CODE(_tcout << _T("按任意键继续..."));
            s_pCMsConsole->SetSithTextColor(FC_WHITE, BC_BLACK, TRUE, FALSE);
        }
    }
}

TCHAR* CMsLog::SwitchError(DWORD dwErrorCode, BOOL bShow)
{
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    static TCHAR MsgBuf[KB_SIZE] = {0};
    BOOL bRet = ::FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_CUSTOM_DEFAULT),
        MsgBuf,
        KB_SIZE,
        NULL);

    SIZE_T sMsgLen = _tcslen(MsgBuf);
    if (sMsgLen)
    {
        MsgBuf[sMsgLen-1] = _T('\0');
    }

    if (bRet)
    {
        if (bShow)
        {
            ::MessageBox(NULL, MsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
        }
    }
    return MsgBuf;
}

TCHAR* CMsLog::ShowLastError(BOOL bShow)
{
    IS_AUTO_LOCK(m_Loglock, m_bLock);
    IS_AUTO_LOCK(s_LogLock, m_bLock);
    static TCHAR MsgBuf[KB_SIZE] = {0};
    BOOL bRet = ::FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        ::GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_CUSTOM_DEFAULT),
        MsgBuf,
        KB_SIZE,
        NULL);

    SIZE_T sMsgLen = _tcslen(MsgBuf);
    if (sMsgLen)
    {
#ifdef UNICODE
        MsgBuf[sMsgLen-1] = _T('\0');
        MsgBuf[sMsgLen-2] = _T('\0');
#else
        MsgBuf[sMsgLen-1] = _T('\0');
#endif
    }

    if (bRet)
    {
        if (bShow)
        {
            ::MessageBox(NULL, MsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
        }
    }
    return MsgBuf;
}

// 设置前景字颜色
E_FOREGOUND_TEXT_COLOR CMsLog::SetFontColor(
    E_FOREGOUND_TEXT_COLOR eColor,
    OUT IN BOOL& bfIntensity
    )
{
    if (s_pCMsConsole)
    {

        IS_AUTO_LOCK(m_Loglock, m_bLock);
        IS_AUTO_LOCK(s_LogLock, m_bLock);
        DWORD dwSithColor = s_pCMsConsole->GetSithColor();
        s_pCMsConsole->SetSithTextColor(eColor, BC_BLACK, bfIntensity, FALSE);
        if (dwSithColor & 0x00000008)
        {
            bfIntensity = TRUE;
        }
        else
        {
            bfIntensity = FALSE;
        }
        dwSithColor &= 0x00000007;
        return (E_FOREGOUND_TEXT_COLOR)dwSithColor;
    }
    return FC_BLACK;
}