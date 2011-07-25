#include "Precompiled.h"

// 数据库管理器构造函数
CSqlServerManager::CSqlServerManager(VOID)
: m_IsInit              (FALSE) // 当前对象创建时并未初始化
, m_hSqlHenv            (NULL)
, m_hDbc                (NULL)
, m_hStmt               (NULL)
, m_SQLPrepareCount     (NULL)
, m_bConnectOK          (FALSE)
, m_fSQLSentenceResult  (-1.0f)
{
    _ENTER_FUN_C;

    ZeroMemoryArray(m_SQLPrepare);
    ZeroMemoryArray(m_szSQLSentenceT);
    ZeroMemoryArray(m_szSQLSentenceA);
    ZeroMemoryArray(m_SQLMessageText);

    _LEAVE_FUN_DISPOSE_END_C;
}

// 数据库管理器析构函数
CSqlServerManager::~CSqlServerManager(VOID)
{
    _ENTER_FUN_C;

    _LEAVE_FUN_DISPOSE_END_C;
}

// 初始化数据库管理器
BOOL CSqlServerManager::Init(VOID)
{
    _ENTER_FUN_C;
    INT nRet = SQL_SUCCESS;

    nRet = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hSqlHenv);
    AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLAllocHandle失败,函数失败"));

    nRet = ::SQLSetEnvAttr(m_hSqlHenv, SQL_ATTR_ODBC_VERSION,(LPVOID)SQL_OV_ODBC3, 0);
    AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLSetEnvAttr失败,函数失败"));

    nRet = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hSqlHenv, &m_hDbc);
    AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLAllocHandle失败,函数失败"));

    nRet = ::SQLSetConnectAttr(m_hDbc, SQL_LOGIN_TIMEOUT, (LPVOID)5, 0);
    AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLSetConnectAttr失败,函数失败"));

    m_IsInit = TRUE;
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 释放数据库管理器
BOOL CSqlServerManager::Release(VOID)
{
    INT nRet = SQL_SUCCESS;
    _ENTER_FUN_C;

    if (m_hStmt)
    {
        nRet = ::SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLFreeHandle失败,函数失败"));
        m_hStmt = NULL;
    }
    if (m_bConnectOK)
    {
        nRet = ::SQLDisconnect(m_hDbc);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLDisconnect失败,函数失败"));
        m_bConnectOK = FALSE;
    }
    if (m_hDbc)
    {
        nRet = ::SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLFreeHandle失败,函数失败"));
        m_hDbc = NULL;
    }
    if (m_hSqlHenv)
    {
        nRet = ::SQLFreeHandle(SQL_HANDLE_ENV, m_hSqlHenv);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLFreeHandle失败,函数失败"));
        m_hSqlHenv = NULL;
    }

    m_IsInit = FALSE;
    // 返回成功
    return TRUE;

    _DISPOSE_END_C
    {
        nRet = ::SQLGetDiagRec(SQL_HANDLE_ENV, m_hSqlHenv, 1, NULL, NULL, m_SQLMessageText, 1024, NULL);
        if ((nRet != SQL_SUCCESS) && (nRet != SQL_SUCCESS_WITH_INFO))
        {
            ::MessageBox(NULL, _T("获取SQL状态出错"), _T("致命错误"), MB_OK);
        }
        else
        {
            ::MessageBox(NULL, m_SQLMessageText, _T("SQL"), MB_OK);
        }
    }
    _LEAVE_FUN;
    return FALSE;
}

// 连接数据库
BOOL CSqlServerManager::ConnectW(CONST WCHAR* szDBName, CONST WCHAR* szUserName, CONST WCHAR* szPassword)
{
    _ENTER_FUN_C;
    INT nRet = SQL_SUCCESS;

    if (szDBName && szUserName && szPassword)
    {
        nRet = ::SQLConnectW(m_hDbc, (SQLWCHAR*)szDBName, (SQLSMALLINT)wcslen(szDBName), (SQLWCHAR*)szUserName, (SQLSMALLINT)wcslen(szUserName), (SQLWCHAR*)szPassword, (SQLSMALLINT)wcslen(szPassword));
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLConnect失败,函数失败"));
        m_bConnectOK = TRUE;

        nRet = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLAllocHandle失败,函数失败"));

        // 返回成功
        return TRUE;
    }
    else
    {
        AssertEx(AL_NORMAL, szDBName, _T("szDBName为空,传参失败"));
        AssertEx(AL_NORMAL, szUserName, _T("szUserName为空,传参失败"));
        AssertEx(AL_NORMAL, szPassword, _T("szPassword为空,传参失败"));
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 连接数据库
BOOL CSqlServerManager::ConnectA(CONST CHAR* szDBName, CONST CHAR* szUserName, CONST CHAR* szPassword)
{
    _ENTER_FUN_C;
    INT nRet = SQL_SUCCESS;

    if (szDBName && szUserName && szPassword)
    {
        nRet = ::SQLConnectA(m_hDbc, (SQLCHAR*)szDBName, (SQLSMALLINT)strlen(szDBName), (SQLCHAR*)szUserName, (SQLSMALLINT)strlen(szUserName), (SQLCHAR*)szPassword, (SQLSMALLINT)strlen(szPassword));
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLConnect失败,函数失败"));
        m_bConnectOK = TRUE;

        nRet = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLAllocHandle失败,函数失败"));

        // 返回成功
        return TRUE;
    }
    else
    {
        AssertEx(AL_NORMAL, szDBName, _T("szDBName为空,传参失败"));
        AssertEx(AL_NORMAL, szUserName, _T("szUserName为空,传参失败"));
        AssertEx(AL_NORMAL, szPassword, _T("szPassword为空,传参失败"));
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 连接数据库
BOOL CSqlServerManager::Connect(CONST TCHAR* szDBName, CONST TCHAR* szUserName, CONST TCHAR* szPassword)
{
    _ENTER_FUN_C;
    INT nRet = SQL_SUCCESS;

    if (szDBName && szUserName && szPassword)
    {
        nRet = ::SQLConnect(m_hDbc, (SQLTCHAR*)szDBName, (SQLSMALLINT)_tcslen(szDBName), (SQLTCHAR*)szUserName, (SQLSMALLINT)_tcslen(szUserName), (SQLTCHAR*)szPassword, (SQLSMALLINT)_tcslen(szPassword));
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLConnect失败,函数失败"));
        m_bConnectOK = TRUE;

        nRet = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLAllocHandle失败,函数失败"));

        // 返回成功
        return TRUE;
    }
    else
    {
        AssertEx(AL_NORMAL, szDBName, _T("szDBName为空,传参失败"));
        AssertEx(AL_NORMAL, szUserName, _T("szUserName为空,传参失败"));
        AssertEx(AL_NORMAL, szPassword, _T("szPassword为空,传参失败"));
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 执行SQL语句
SR CSqlServerManager::ExecuteSQLSentence(TCHAR* szSQLSentence, BOOL IsNeedResult, CAutoTableManager* pTable)
{
    INT nRet = SQL_SUCCESS;
    _ENTER_FUN_C;
    if (!szSQLSentence)
    {
        szSQLSentence = m_szSQLSentenceT;
    }
    if (pTable)
    {
        pTable->Release();
        pTable->Init();
    }

    nRet = ::SQLExecDirect(m_hStmt, (SQLTCHAR*)szSQLSentence, (SQLSMALLINT)_tcslen(szSQLSentence) + 1);
    if (nRet == SQL_SUCCESS)
    {
        if ((IsNeedResult && !pTable) || (m_fSQLSentenceResult == 0.0f))
        {
            LONG Size = sizeof(m_fSQLSentenceResult);
            if (::SQLFetch(m_hStmt) == SQL_SUCCESS)
            {
                ::SQLGetData(
                    m_hStmt,
                    1,
                    MS_SQL_TYPE_FLOAT,
                    &m_fSQLSentenceResult,
                    Size,
                    &Size
                    );
            }
        }
        else if (IsNeedResult)
        {
            AssertEx(AL_NORMAL, pTable, _T("pTable为空,错误传参"));
            UINT uTableItemCount;
            UINT uTableLineCount;
            LONG  nSizeOfItem;
            SQL_TYPE SqlType;
            LPVOID lpData = NULL;
            pTable->GetTableSize(uTableItemCount, uTableLineCount);
            for (UINT j = 0; j < uTableLineCount; j++)
            {
                if (::SQLFetch(m_hStmt) != SQL_SUCCESS)
                {
                    break;
                }
                for (UINT i = 0; i < uTableItemCount; i++)
                {
                    SqlType = pTable->GetTableItemType(i);
                    lpData = pTable->GetTableItemInLine(i, j);
                    nSizeOfItem = pTable->GetTableItemSize(i, j, TRUE);
                    ::SQLGetData(
                        m_hStmt,
                        SQLUSMALLINT(i + 1),
                        (SQLSMALLINT)SqlType,
                        lpData,
                        nSizeOfItem,
                        &nSizeOfItem
                        );
                    pTable->SetValidLine(j + 1);
                }
            }
        }
        nRet = ::SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLFreeHandle失败,函数失败"));
        nRet = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
        AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLAllocHandle失败,函数失败"));
        return SR_OK;
    }

    else
    {
        SQLTCHAR SQLState[MAX_PATH] = {0}; SQLINTEGER NativeErrorPtr;
        nRet = ::SQLGetDiagRec(SQL_HANDLE_STMT, m_hStmt, 1, SQLState, &NativeErrorPtr, m_SQLMessageText, 1024, NULL);
        if ((nRet != SQL_SUCCESS) && (nRet != SQL_SUCCESS_WITH_INFO))
        {
            ::MessageBox(NULL, _T("获取SQL状态出错"), _T("致命错误"), MB_OK);
        }
        else
        {
            nRet = ::SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
            AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLFreeHandle失败,函数失败"));
            nRet = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
            AssertEx(AL_NORMAL, (nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO), _T("SQLAllocHandle失败,函数失败"));
            if (_tcsstr(m_SQLMessageText, _T("不存在。请确保正确地输入了该名称。")))
            {
                return SR_DB_NAME_ERROR;
            }
            else if (_tcsstr(m_SQLMessageText, _T("已将数据库上下文更改为")))
            {
                return SR_OK;
            }
            {
                return SR_ERROR;
            }
        }
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return SR_CODE_THROW;
}

// 执行准备好的SQL语句
SR CSqlServerManager::ExecuteSQLPrepare(INT nIndex, TCHAR* szSQLParameter, ...)
{
    va_list vaList;
    va_start(vaList, szSQLParameter);
    _ENTER_FUN_C;

    //if (!this->CheckSQLPrepare(nIndex, szSQLParameter))
    //{
    //    AssertEx(AL_NORMAL, FALSE, _T("传入SQL语句参数非法,错误逻辑"));
    //}
    ZeroMemoryArray(m_szSQLSentenceT);

    TCHAR* strFindNextParameter = m_SQLPrepare[nIndex].m_szSQLPrepare;
    TCHAR* strSQLSentence = m_szSQLSentenceT;
    while(*strFindNextParameter)
    {
        if (_T('%') == *strFindNextParameter)
        {
            *strSQLSentence++ = *strFindNextParameter++;
            switch(*strFindNextParameter)
            {
            case _T('d'):
                {
                    *(--strSQLSentence) = _T('\0');
                    _itot(va_arg(vaList, INT), strSQLSentence, RADIX_10);
                    strSQLSentence += _tcslen(strSQLSentence);
                    strFindNextParameter++;
                }break;
            case _T('s'):
                {
                    *(--strSQLSentence) = _T('\0');
                    _tcscat(strSQLSentence, va_arg(vaList, TCHAR*));
                    strSQLSentence += _tcslen(strSQLSentence);
                    strFindNextParameter++;
                }break;
            case _T('f'):
                {
                    *(--strSQLSentence) = _T('\0');
                    _stprintf(strSQLSentence, _T("%06f"), va_arg(vaList, FLOAT));
                    strSQLSentence += _tcslen(strSQLSentence);
                    strFindNextParameter++;
                }break;
            case _T('D'):
                {
                    *(--strSQLSentence) = _T('\0');
                    SQL_DATE_STRUCT date = va_arg(vaList, SQL_DATE_STRUCT);
                    _stprintf(strSQLSentence, _T("%d-%d-%d"), date.year, date.month, date.day);
                    strSQLSentence += _tcslen(strSQLSentence);
                    strFindNextParameter++;
                }break;
            case _T('T'):
                {
                    *(--strSQLSentence) = _T('\0');
                    SQL_TIME_STRUCT time = va_arg(vaList, SQL_TIME_STRUCT);
                    _stprintf(strSQLSentence, _T("%d-%d-%d"), time.hour, time.minute, time.second);
                    strSQLSentence += _tcslen(strSQLSentence);
                    strFindNextParameter++;
                }break;
            default:
                {
                    *strSQLSentence++ = *strFindNextParameter++;
                    continue;
                }break;
            }
        }
        else
        {
            *strSQLSentence++ = *strFindNextParameter++;
            continue;
        }
    }
    this->ExecuteSQLSentence(m_szSQLSentenceT);

    va_end(vaList);
    return SR_OK;

    _LEAVE_FUN_DISPOSE_END_C;
    va_end(vaList);
    return SR_CODE_THROW;
}

//// 检查预备SQL语句是否匹配
//BOOL CSqlServerManager::CheckSQLPrepare(INT nIndex, TCHAR* szSQLParameter)
//{
//    _ENTER_FUN_C;
//
//    TCHAR* pSrc = m_SQLPrepare[nIndex].m_szSQLPrepare;
//    TCHAR* pDes = szSQLParameter;
//
//    for (;;)
//    {
//        TCHAR* pSrcTemp = _tcschr(pSrc, _T('%'));
//        TCHAR* pDesTemp = _tcschr(pDes, _T('%'));
//        if (!pSrcTemp && !pDesTemp)
//        {
//            return TRUE;
//        }
//        if (pSrcTemp && pDesTemp)
//        {
//            switch (*pSrcTemp)
//            {
//            case _T('d'):
//            case _T('s'):
//            case _T('f'):
//            case _T('D'):
//            case _T('T'):
//                {
//                    return FALSE;
//                }break;
//            default:
//                {
//                    return TRUE;
//                }break;
//            }
//        }
//        pSrcTemp++;pDesTemp++;
//        if (*pSrcTemp != *pDesTemp)
//        {
//            switch (*pSrcTemp)
//            {
//            case _T('d'):
//            case _T('s'):
//            case _T('f'):
//            case _T('D'):
//            case _T('T'):
//                {
//                    return FALSE;
//                }break;
//            default:
//                {
//                    continue;
//                }break;
//            }
//        }
//        pSrc = pSrcTemp; pDes = pDesTemp;
//    }
//
//    _LEAVE_FUN_DISPOSE_END_C;
//    return FALSE;
//}

// 添加准备好的SQL语句
INT CSqlServerManager::AddSQLPrepare(TCHAR* szSQLSentence)
{
    _ENTER_FUN_C;
    AssertEx(AL_NORMAL, szSQLSentence, _T("szSQLSentence为空,错误传参"));

    AssertEx(AL_NORMAL, m_SQLPrepareCount < MAX_PREPARE_SQL_COUNT,_T("已超过SQL语句最大值,错误逻辑"));

    TCHAR* strFindNextParameter = szSQLSentence;

    while(strFindNextParameter && *strFindNextParameter)
    {
        if (_T('%') == *strFindNextParameter)
        {
            strFindNextParameter++;
            switch(*strFindNextParameter)
            {
            case _T('d'):
                {
                    m_SQLPrepare[m_SQLPrepareCount].m_SqlType[m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber] = MS_SQL_TYPE_INT;
                    m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber++;
                }break;
            case _T('s'):
                {
                    m_SQLPrepare[m_SQLPrepareCount].m_SqlType[m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber] = MS_SQL_TYPE_TCHAR;
                    m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber++;
                }break;
            case _T('f'):
                {
                    m_SQLPrepare[m_SQLPrepareCount].m_SqlType[m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber] = MS_SQL_TYPE_FLOAT;
                    m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber++;
                }break;
            case _T('D'):
                {
                    m_SQLPrepare[m_SQLPrepareCount].m_SqlType[m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber] = MS_SQL_TYPE_DATETIME;
                    m_SQLPrepare[m_SQLPrepareCount].m_ParameterNumber++;
                }break;
            default:
                {
                    strFindNextParameter++;
                }break;
            }
        }
        else
        {
            strFindNextParameter++;
        }
    }
    if (szSQLSentence && m_SQLPrepareCount < MAX_PREPARE_SQL_COUNT)
    {
        _tcscpy_s(m_SQLPrepare[m_SQLPrepareCount].m_szSQLPrepare, MAX_PREPARE_SQL_SENTENCE, szSQLSentence);
        m_SQLPrepareCount++;
        return m_SQLPrepareCount - 1;
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_NID;
}

// 删除准备好的SQL语句
BOOL CSqlServerManager::RemoveSQLPrepare(INT nIndex)
{
    _ENTER_FUN_C;
    AssertEx(AL_NORMAL, nIndex < m_SQLPrepareCount,_T("要删除的SQL语句并不存在,错误逻辑"));

    if (m_SQLPrepareCount < MAX_PREPARE_SQL_COUNT)
    {
        if (nIndex != m_SQLPrepareCount)
        {
            memcpy(&m_SQLPrepare[m_SQLPrepareCount], &m_SQLPrepare[nIndex], sizeof(m_SQLPrepare[m_SQLPrepareCount]));
        }
        m_SQLPrepare[m_SQLPrepareCount].Release();
        m_SQLPrepareCount--;
        return TRUE;
    }
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

BOOL CSqlServerManager::LoadSqlSentenceFromFile(TCHAR* szFileName, LSFFF nFlag)
{
    _ENTER_FUN_C;

    if (!szFileName)
    {
        return FALSE;
    }

    BOOL bRet = TRUE;
    AssertEx(AL_NORMAL, szFileName, _T("传入文件名为空,错误调用"));
    HANDLE hSqlFile = ::CreateFile(
        szFileName,
        FILE_READ_DATA,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    AssertEx(AL_NORMAL, hSqlFile != INVALID_HANDLE, _T("打开文件失败,函数失败"));

    DWORD SqlFileLen = ::GetFileSize(hSqlFile, NULL);
    AssertEx(AL_NORMAL, SqlFileLen, _T("文件为空,函数失败"));
    AssertEx(AL_NORMAL, SqlFileLen < MAX_PREPARE_SQL_SENTENCE, _T("SQL文件过大,错误逻辑"));

    ZeroMemoryArray(m_szSQLSentenceA);
    ZeroMemoryArray(m_szSQLSentenceT);

#ifdef UNICODE
    bRet = ::ReadFile(
        hSqlFile,
        m_szSQLSentenceA,
        SqlFileLen,
        &SqlFileLen,
        NULL);
    AssertEx(AL_NORMAL, bRet, _T("读取文件内容失败,函数失败"));
    ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)m_szSQLSentenceA, MAX_PREPARE_SQL_SENTENCE, m_szSQLSentenceT, MAX_PREPARE_SQL_SENTENCE);
#else
    bRet = ::ReadFile(
        hSqlFile,
        m_szSQLSentenceT,
        SqlFileLen,
        &SqlFileLen,
        NULL);
    AssertEx(AL_NORMAL, bRet, _T("读取文件内容失败,函数失败"));
#endif
    this->CheckFileSQL();
    if (nFlag == LSFFF_LOAD_AND_IMMEDIATELY_EXECUTE)
    {
        return this->ExecuteSQLSentence(m_szSQLSentenceT);
    }
    else if (nFlag == LSFFF_LOAD_AND_ADD_TO_PREPARE)
    {
        return this->AddSQLPrepare(m_szSQLSentenceT);
    }

    ::CloseHandle(hSqlFile);
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

inline VOID CSqlServerManager::CheckFileSQL()
{
    TCHAR* pFindChr = m_szSQLSentenceT;
    TCHAR* pFindChrBak = pFindChr;
    for (;;)
    {
        pFindChr = _tcschr(pFindChr, _T('G'));
        if (!pFindChr)
        {
            pFindChr = pFindChrBak;
            pFindChr = _tcschr(pFindChr, _T('g'));
            if (!pFindChr)
            {
                break;
            }
        }
        // 如果后一个字符为字母'O'
        if ((_T('O') == (*(pFindChr+1))) || (_T('o') == (*(pFindChr+1))))
        {
            if (0x0d == (*(pFindChr+2)))
            {
                *pFindChr = _T(' ');
                *(pFindChr + 1) = _T(' ');
                pFindChr+=2;
                continue;
            }
            else if (0x00 == (*(pFindChr+2)))
            {
                *pFindChr = _T(' ');
                *(pFindChr + 1) = _T('\0');
                break;
            }
        }
        pFindChr++;
        pFindChrBak = pFindChr;
    }
}

// 创建SQL语句
BOOL CSqlServerManager::CreateSQLSentence(OUT TCHAR* szSQLSentence, SIZE_T MaxSQLLen, SSET sset, TCHAR* szTableName, LPVOID pData)
{
    _ENTER_FUN_C;

    if (!szSQLSentence)
    {
        szSQLSentence = m_szSQLSentenceT;
        MaxSQLLen = MAX_PREPARE_SQL_SENTENCE;
    }
    ZeroMemory(szSQLSentence, (sizeof(TCHAR) * MaxSQLLen));

    switch (sset)
    {
    case SSET_CREATE:
        return this->CreateSQLSentenceAtCreateTable(szSQLSentence, MaxSQLLen, szTableName, (STIAC*)pData);
        break;
    case SSET_SELECT:
        return this->CreateSQLSentenceAtSelectItem(szSQLSentence, MaxSQLLen, (SSIA*)pData);
        break;
    case SSET_INSERT:
        return this->CreateSQLSentenceAtInsertItem(szSQLSentence, MaxSQLLen, szTableName, (SIIAC*)pData);
        break;
    }

    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 创建SQL语句之创建表格
BOOL CSqlServerManager::CreateSQLSentenceAtCreateTable(OUT TCHAR* szSQLSentence, SIZE_T sMaxSQLLen, TCHAR* szTableName, STIAC* pStiac)
{
    _ENTER_FUN_C;

    _sntprintf_s(
        szSQLSentence, sMaxSQLLen, _TRUNCATE,
        //_T("USE [%s]\n ")
        _T("CREATE TABLE [dbo].[%s]\n")
        _T("("),
        /*szDBName, */szTableName
        );

    STIA* pStia = pStiac->m_pStia;
    TCHAR* strSqlType = NULL;       // 定义SQL表项类型
    TCHAR* strSqlCharLen = NULL;    // 定义SQL表字符项长度
    TCHAR* strCanNull = NULL;       // 定义SQL表项是否允许为空
    SIZE_T TempSize = 0;
    for (INT i = 0; i < pStiac->m_Count; i++,pStia++)
    {
        TempSize = _tcslen(szSQLSentence);
        szSQLSentence += TempSize;
        sMaxSQLLen -= TempSize;

        switch (pStia->m_SqlType)
        {
        case MS_SQL_TYPE_INT:
            if (pStia->m_IsAutoAdd)
            {
                strSqlType = _T("[int] IDENTITY(1,1)");
            }
            else
            {
                strSqlType = _T("[int]");
            }
            strSqlCharLen = _T("");
            strCanNull = pStia->m_IsCanNull ? _T("") : _T("NOT ");
            break;
        case MS_SQL_TYPE_TCHAR:
            strSqlType = _T("[nvarchar]");
            strSqlCharLen = pStia->m_strLen;
            strCanNull = pStia->m_IsCanNull ? _T("") : _T("NOT ");
            break;
        case MS_SQL_TYPE_FLOAT:
            strSqlType = _T("[float]");
            strSqlCharLen = _T("");
            strCanNull = pStia->m_IsCanNull ? _T("") : _T("NOT ");
            break;
        case MS_SQL_TYPE_DATETIME:
            strSqlType = _T("[datetime]");
            strSqlCharLen = _T("");
            strCanNull = pStia->m_IsCanNull ? _T("") : _T("NOT ");
            break;
        default:
            {
                continue;
            }
        }
        _sntprintf_s(
            szSQLSentence, sMaxSQLLen, _TRUNCATE,
            _T("[%s]%s %s %s NULL,\0"),
            pStia->m_szTableItemName,
            strSqlType,
            strSqlCharLen,
            strCanNull
            );
    }
    szSQLSentence[sMaxSQLLen-1] = 0;

    TempSize = _tcsnlen(szSQLSentence, sMaxSQLLen);
    szSQLSentence += TempSize;
    sMaxSQLLen -= TempSize;
    _sntprintf_s(
        szSQLSentence, sMaxSQLLen, _TRUNCATE,
        _T("CONSTRAINT [PK_%s] PRIMARY KEY CLUSTERED")
        _T("([%s] ASC)")
        _T("WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY]")
        _T(") ON [PRIMARY]"),
        szTableName,
        pStiac->m_szTableKeyName
        );

    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 创建SQL语句之查询项
BOOL CSqlServerManager::CreateSQLSentenceAtInsertItem(OUT TCHAR* szSQLSentence, SIZE_T sMaxSQLLen, TCHAR* szTableName, SIIAC* pSiiac)
{
    _sntprintf_s(
        szSQLSentence, sMaxSQLLen, _TRUNCATE,
        //_T("USE [%s]\n ")
        _T("INSERT INTO [%s]\n")
        _T("("),
        /*szDBName, */szTableName
        );

    SIIA*  pSiia = pSiiac->m_pSiia;
    SIZE_T TempSize = 0;

    for (INT i = 0; i < pSiiac->m_Count; i++,pSiia++)
    {
        TempSize = _tcslen(szSQLSentence);
        szSQLSentence += TempSize;
        sMaxSQLLen -= TempSize;
        _sntprintf_s(
            szSQLSentence, sMaxSQLLen, _TRUNCATE,
            _T("[%s],"),
            pSiia->m_szItemName
            );
    }

    szSQLSentence[sMaxSQLLen-1] = 0;

    TempSize = _tcsnlen(szSQLSentence, sMaxSQLLen);
    szSQLSentence += TempSize;
    sMaxSQLLen -= TempSize;
    *(--szSQLSentence) = _T(')');
    *(++szSQLSentence) = _T('\n');
    _tcscpy(szSQLSentence, _T("VALUES \n("));

    pSiia = pSiiac->m_pSiia;
    for (INT i = 0; i < pSiiac->m_Count; i++,pSiia++)
    {
        TempSize = _tcslen(szSQLSentence);
        szSQLSentence += TempSize;
        sMaxSQLLen -= TempSize;
        switch (pSiia->m_SqlType)
        {
        case MS_SQL_TYPE_INT:
            {
                _sntprintf_s(
                    szSQLSentence, sMaxSQLLen, _TRUNCATE,
                    _T("%s,"),
                    pSiia->m_szItemValue
                    );
            }break;
        case MS_SQL_TYPE_TCHAR:
            {
                _sntprintf_s(
                    szSQLSentence, sMaxSQLLen, _TRUNCATE,
                    _T("N'%s',"),
                    pSiia->m_szItemValue
                    );
            }break;
        case MS_SQL_TYPE_FLOAT:
            {
                _sntprintf_s(
                    szSQLSentence, sMaxSQLLen, _TRUNCATE,
                    _T("%s,"),
                    pSiia->m_szItemValue
                    );
            }break;
        case MS_SQL_TYPE_DATETIME:
            {
                _sntprintf_s(
                    szSQLSentence, sMaxSQLLen, _TRUNCATE,
                    _T("CONVERT(DATETIME, '%s', 102),"),
                    pSiia->m_szItemValue
                    );
            }break;
        }
    }
    TempSize = _tcslen(szSQLSentence);
    szSQLSentence += TempSize;
    sMaxSQLLen -= TempSize;
    *(--szSQLSentence) = _T(')');
    *(++szSQLSentence) = _T('\n');
    return TRUE;
}

// 创建SQL语句之查询项
BOOL CSqlServerManager::CreateSQLSentenceAtSelectItem(OUT TCHAR* szSQLSentence, SIZE_T sMaxSQLLen, SSIA* pSsia)
{
    _ENTER_FUN_C;

    // 创建SELECT
    SIZE_T TempSize = 0;
    switch (pSsia->m_SelectType)
    {
    case SELECT_TYPE_COUNT:
        {
            _sntprintf_s(
                szSQLSentence, sMaxSQLLen, _TRUNCATE,
                _T("SELECT COUNT(%s) \n"),
                pSsia->m_szSelectItemName
                );
            m_fSQLSentenceResult = 0.0f;
        }break;
    case SELECT_TYPE_SUM:
        {
            _sntprintf_s(
                szSQLSentence, sMaxSQLLen, _TRUNCATE,
                _T("SELECT SUM(%s) \n"),
                pSsia->m_szSelectItemName
                );
            m_fSQLSentenceResult = 0.0f;
        }break;
    case SELECT_TYPE_AVG:
        {
            _sntprintf_s(
                szSQLSentence, sMaxSQLLen, _TRUNCATE,
                _T("SELECT AVG(%s) \n"),
                pSsia->m_szSelectItemName
                );
            m_fSQLSentenceResult = 0.0f;
        }break;
    case SELECT_TYPE_ALL_ITEM:
        {
            _tcscpy(szSQLSentence, _T("SELECT * \n"));
        }break;
    case SELECT_TYPE_ONE_ITEM:
        {
            _sntprintf_s(
                szSQLSentence, sMaxSQLLen, _TRUNCATE,
                _T("SELECT [%s] \n"),
                pSsia->m_szItemName
                );
        }break;
    default:
        {
            AssertEx(AL_NORMAL, FALSE, _T("类型错误,错误传参"));
        }
    }

    // 创建FROM
    TempSize = _tcsnlen(szSQLSentence, sMaxSQLLen);
    szSQLSentence += TempSize;
    sMaxSQLLen -= TempSize;
    AssertEx(AL_NORMAL, pSsia->m_strTableName, _T("pSsia->m_strTableName为空,错误传参"));
    _sntprintf_s(
        szSQLSentence, sMaxSQLLen, _TRUNCATE,
        _T("FROM [%s]\n"),
        pSsia->m_strTableName
        );


    if (pSsia->m_szWhereAttrTeam && _tcslen(pSsia->m_szWhereAttrTeam))
    {
        // 创建WHERE
        TempSize = _tcslen(szSQLSentence);
        szSQLSentence += TempSize;
        sMaxSQLLen -= TempSize;
        _tcscpy(szSQLSentence, _T("WHERE ("));

        TempSize = 0;

        if (pSsia->m_szWhereAttrTeam)
        {
            TempSize = _tcslen(szSQLSentence);
            szSQLSentence += TempSize;
            sMaxSQLLen -= TempSize;

            switch (pSsia->m_WhereAttrNexus)
            {
            case SSWN_UNKNOW:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("%s)"),
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            case SSWN_EQUAL:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("[%s] = N'%s' )"),
                        pSsia->m_szItemName,
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            case SSWN_NOTEQUAL:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("[%s] != N'%s' )"),
                        pSsia->m_szItemName,
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            case SSWN_BIG:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("[%s] > N'%s' )"),
                        pSsia->m_szItemName,
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            case SSWN_SMALL:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("[%s] < N'%s' )"),
                        pSsia->m_szItemName,
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            case SSWN_BIG_EQUAL:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("[%s] >= N'%s' )"),
                        pSsia->m_szItemName,
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            case SSWN_SMALL_EQUAL:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("[%s] <= N'%s' )"),
                        pSsia->m_szItemName,
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            case SSWN_LIKE:
                {
                    _sntprintf_s(
                        szSQLSentence, sMaxSQLLen, _TRUNCATE,
                        _T("[%s] LIKE N'%s' )"),
                        pSsia->m_szItemName,
                        pSsia->m_szWhereAttrTeam
                        );
                }break;
            }
        }
    }
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 获取语句执行结果
FLOAT CSqlServerManager::GetSQLSentenceResult(VOID)
{
    FLOAT fCount = m_fSQLSentenceResult;
    m_fSQLSentenceResult = -1.0f;
    return fCount;
}