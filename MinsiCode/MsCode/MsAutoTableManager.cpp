#include "Precompiled.h"
#include "MsSqlServerManager.h"

// 表格管理器构造函数
CAutoTableManager::CAutoTableManager(ATAC* pAtac)
: m_IsInit      (FALSE) // 当前对象创建时并未初始化
, m_uItemCount  (pAtac->m_uItemCount)
, m_uLineCount  (pAtac->m_uLineCount)
, m_pAi         (NULL)
, m_uItemIndex   (0)
, m_uLineIndex   (0)
, m_uValidLine   (0)
{
    _ENTER_FUN_C;

    m_pAi = new AI[m_uItemCount];
    AssertEx(AL_NORMAL, m_pAi, _T("m_pAi为空,内存不足"));
    ZeroMemory(m_pAi, sizeof(AI) * m_uItemCount);

    for (UINT i = 0; i < m_uItemCount; i++)
    {
        m_pAi[i].m_pAta = pAtac->m_pAta[i];
    }

    _LEAVE_FUN_DISPOSE_END_C;
}

// 表格管理器析构函数
CAutoTableManager::~CAutoTableManager(VOID)
{
    _ENTER_FUN_C;

    this->Release();
    SAFE_DELETE_ARRAY(m_pAi);

    _LEAVE_FUN_DISPOSE_END_C;
}

// 初始化表格管理器
BOOL CAutoTableManager::Init(VOID)
{
    _ENTER_FUN_C;

    for (UINT i = 0; i < m_uItemCount; i++)
    {
        switch (m_pAi[i].m_pAta)
        {
        case MS_SQL_TYPE_INT:
            {
                m_pInt = new INT[m_uLineCount];
                AssertEx(AL_NORMAL, m_pInt, _T("m_pInt为空,内存不足"));
                ZeroMemory(m_pInt, sizeof(INT) * m_uLineCount);
                m_pAi[i].m_lpItem = m_pInt;
            }break;
        case MS_SQL_TYPE_TCHAR:
            {
                m_pTchar = new TCA[m_uLineCount];
                AssertEx(AL_NORMAL, m_pTchar, _T("m_pInt为空,内存不足"));
                ZeroMemory(m_pTchar, sizeof(TCA) * m_uLineCount);
                m_pAi[i].m_lpItem = m_pTchar;
            }break;
        case MS_SQL_TYPE_FLOAT:
            {
                m_pFloat = new FLOAT[m_uLineCount];
                AssertEx(AL_NORMAL, m_pFloat, _T("m_pInt为空,内存不足"));
                ZeroMemory(m_pFloat, sizeof(FLOAT) * m_uLineCount);
                m_pAi[i].m_lpItem = m_pFloat;
            }break;
        case MS_SQL_TYPE_DATETIME:
            {
                m_pDataTime = new TIMESTAMP_STRUCT[m_uLineCount];
                AssertEx(AL_NORMAL, m_pDataTime, _T("m_pInt为空,内存不足"));
                ZeroMemory(m_pDataTime, sizeof(TIMESTAMP_STRUCT) * m_uLineCount);
                m_pAi[i].m_lpItem = m_pDataTime;
            }break;
        default:
            {
                AssertEx(AL_NORMAL, FALSE,_T("无效类型,错误逻辑"));
            }
        }
    }
    m_uValidLine = 0;
    m_IsInit = TRUE;
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 检查是否已经初始化表格管理器
BOOL CAutoTableManager::IsInit(VOID)
{
    return m_IsInit;
}

// 释放表格管理器
BOOL CAutoTableManager::Release(VOID)
{
    _ENTER_FUN_C;

    if (!m_IsInit)
    {
        return TRUE;
    }
    for (UINT i = 0; i < m_uItemCount; i++)
    {
        switch (m_pAi[i].m_pAta)
        {
        case MS_SQL_TYPE_INT:
            {
                m_pInt = (INT*)m_pAi[i].m_lpItem;
                SAFE_DELETE_ARRAY(m_pInt);
                m_pAi[i].m_lpItem = m_pInt;
            }break;
        case MS_SQL_TYPE_TCHAR:
            {
                m_pTchar = (TCA*)m_pAi[i].m_lpItem;
                SAFE_DELETE_ARRAY(m_pTchar);
                m_pAi[i].m_lpItem = m_pTchar;
            }break;
        case MS_SQL_TYPE_FLOAT:
            {
                m_pFloat = (FLOAT*)m_pAi[i].m_lpItem;
                SAFE_DELETE_ARRAY(m_pFloat);
                m_pAi[i].m_lpItem = m_pFloat;
            }break;
        case MS_SQL_TYPE_DATETIME:
            {
                m_pDataTime = (TIMESTAMP_STRUCT*)m_pAi[i].m_lpItem;
                SAFE_DELETE_ARRAY(m_pDataTime);
                m_pAi[i].m_lpItem = m_pDataTime;
            }break;
        default:
            {
                AssertEx(AL_NORMAL, FALSE,_T("无效类型,错误逻辑"));
            }
        }
    }
    m_uValidLine = 0;
    m_IsInit = FALSE;

    // 返回成功
    return TRUE;
    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

LPVOID CAutoTableManager::GetTableItemInLine(UINT uItemIndex, UINT uLineIndex)
{
    _ENTER_FUN_C;

    AssertEx(AL_NORMAL, (uItemIndex < m_uItemCount),_T("uItemIndex访问越界,错误传参"));
    AssertEx(AL_NORMAL, (uLineIndex < m_uLineCount),_T("uLineIndex访问越界,错误传参"));

    switch (m_pAi[uItemIndex].m_pAta)
    {
    case MS_SQL_TYPE_INT:
        {
            m_pInt = (INT*)m_pAi[uItemIndex].m_lpItem;
            return &m_pInt[uLineIndex];
        }break;

    case MS_SQL_TYPE_TCHAR:
        {
            m_pTchar = (TCA*)m_pAi[uItemIndex].m_lpItem;
            return m_pTchar[uLineIndex].m_strText;
        }break;

    case MS_SQL_TYPE_FLOAT:
        {
            m_pFloat = (FLOAT*)m_pAi[uItemIndex].m_lpItem;
            return &m_pFloat[uLineIndex];
        }break;

    case MS_SQL_TYPE_DATETIME:
        {
            m_pDataTime = (TIMESTAMP_STRUCT*)m_pAi[uItemIndex].m_lpItem;
            return &m_pDataTime[uLineIndex];
        }break;

    default:
        {
            AssertEx(AL_NORMAL, FALSE,_T("无效类型,错误逻辑"));
            return NULL;
        }break;
    }

    _LEAVE_FUN_DISPOSE_END_C;
    return NULL;
}

INT* CAutoTableManager::GetTableItemInLineAsINT(UINT uItemIndex, UINT uLineIndex)
{
    m_pInt = (INT*)this->GetTableItemInLine(uItemIndex, uLineIndex);
    AssertEx(AL_NORMAL, m_pInt,_T("m_pInt为空,函数失败"));
    AssertEx(AL_NORMAL, (m_pAi[uItemIndex].m_pAta == MS_SQL_TYPE_INT),_T("类型不匹配,错误传参"));
    return m_pInt;
}

TCA* CAutoTableManager::GetTableItemInLineAsTCHAR(UINT uItemIndex, UINT uLineIndex)
{
    m_pTchar = (TCA*)this->GetTableItemInLine(uItemIndex, uLineIndex);
    AssertEx(AL_NORMAL, m_pTchar,_T("m_pTchar为空,函数失败"));
    AssertEx(AL_NORMAL, (m_pAi[uItemIndex].m_pAta == MS_SQL_TYPE_TCHAR),_T("类型不匹配,错误传参"));
    return m_pTchar;
}

FLOAT* CAutoTableManager::GetTableItemInLineAsFLOAT(UINT uItemIndex, UINT uLineIndex)
{
    m_pFloat = (FLOAT*)this->GetTableItemInLine(uItemIndex, uLineIndex);
    AssertEx(AL_NORMAL, m_pFloat,_T("m_pFloat为空,函数失败"));
    AssertEx(AL_NORMAL, (m_pAi[uItemIndex].m_pAta == MS_SQL_TYPE_FLOAT),_T("类型不匹配,错误传参"));
    return m_pFloat;
}

TIMESTAMP_STRUCT* CAutoTableManager::GetTableItemInLineAsDATETIME(UINT uItemIndex, UINT uLineIndex)
{
    m_pDataTime = (TIMESTAMP_STRUCT*)this->GetTableItemInLine(uItemIndex, uLineIndex);
    AssertEx(AL_NORMAL, m_pDataTime,_T("m_pDataTime为空,函数失败"));
    AssertEx(AL_NORMAL, (m_pAi[uItemIndex].m_pAta == MS_SQL_TYPE_DATETIME),_T("类型不匹配,错误传参"));
    return m_pDataTime;
}

VOID CAutoTableManager::SetTablePointer(UINT uItemIndex, UINT uLineIndex)
{
    AssertEx(AL_NORMAL, (uItemIndex < m_uItemCount),_T("uItemIndex访问越界,错误传参"));
    AssertEx(AL_NORMAL, (uLineIndex < m_uLineCount),_T("uLineIndex访问越界,错误传参"));
    m_uItemIndex = uItemIndex;
    m_uLineIndex = uLineIndex;
}

VOID CAutoTableManager::GetTablePointer(UINT& uItemIndex, UINT& uLineIndex)
{
    AssertEx(AL_NORMAL, (uItemIndex < m_uItemCount), _T("uItemIndex访问越界,错误传参"));
    AssertEx(AL_NORMAL, (uLineIndex < m_uLineCount), _T("uLineIndex访问越界,错误传参"));
    uItemIndex = m_uItemIndex;
    uLineIndex = m_uLineIndex;
}


VOID CAutoTableManager::SetValidLine(UINT uCount)
{
    AssertEx(AL_NORMAL, (uCount <= m_uLineCount),_T("nCount访问越界,错误传参"));
    m_uValidLine = uCount;
}

INT CAutoTableManager::GetValidLine(VOID)
{
    return m_uValidLine;
}

BOOL CAutoTableManager::AddTableItemPointer(VOID)
{
    if (m_uItemIndex + 1 < m_uItemCount)
    {
        m_uItemIndex++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CAutoTableManager::SubTableItemPointer(VOID)
{
    if (m_uItemIndex > 0)
    {
        m_uItemIndex--;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CAutoTableManager::AddTableLinePointer(VOID)
{
    if (m_uLineIndex + 1 < m_uLineCount)
    {
        m_uLineIndex++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CAutoTableManager::SubTableLinePointer(VOID)
{
    if (m_uLineIndex > 0)
    {
        m_uLineIndex--;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CAutoTableManager::operator ++ (VOID)
{
    return this->AddTableItemPointer();
}

BOOL CAutoTableManager::operator ++ (INT)
{
    return this->AddTableLinePointer();
}

BOOL CAutoTableManager::operator -- (VOID)
{
    return this->SubTableItemPointer();
}

BOOL CAutoTableManager::operator -- (INT)
{
    return this->SubTableLinePointer();
}

VOID CAutoTableManager::GetTableSize(UINT& uItemCount, UINT& uLineCount)
{
    uItemCount = m_uItemCount;
    uLineCount = m_uLineCount;
}

SQL_TYPE CAutoTableManager::GetTableItemType(UINT uItemIndex)
{
    AssertEx(AL_NORMAL, (uItemIndex < m_uItemCount),_T("uItemIndex访问越界,错误传参"));
    return m_pAi[uItemIndex].m_pAta;
}

INT CAutoTableManager::GetTableItemSize(UINT uItemIndex, UINT uLineIndex, BOOL IsUsable)
{
    _ENTER_FUN_C;
    AssertEx(AL_NORMAL, (uItemIndex < m_uItemCount),_T("uItemIndex访问越界,错误传参"));
    switch (m_pAi[uItemIndex].m_pAta)
    {
    case MS_SQL_TYPE_INT:
        {
            return sizeof(INT);
        }break;
    case MS_SQL_TYPE_TCHAR:
        {
            if (IsUsable)
            {
                return sizeof(TCA);
            }
            else
            {
                m_pTchar = (TCA*)m_pAi[uItemIndex].m_lpItem;
                return INT((_tcslen(m_pTchar[uLineIndex].m_strText) + 1) * sizeof(TCHAR));
            }
        }break;
    case MS_SQL_TYPE_FLOAT:
        {
            return sizeof(FLOAT);
        }break;
    case MS_SQL_TYPE_DATETIME:
        {
            return sizeof(TIMESTAMP_STRUCT);
        }break;
    default:
        {
            return 0;
        }break;
    }
    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_NID;
}
