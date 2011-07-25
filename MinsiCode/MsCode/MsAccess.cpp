#include "MFCPrecompiled.h"

// 构造、析构
CMsAccess::CMsAccess(VOID)
: m_pCDaoDatabase   (NULL)
, m_pCDaoRecordset  (NULL)
, m_pCDaoException  (NULL)
, m_COleVariant     (m_ResultBuff)
{
    m_pCDaoDatabase = NEW CDaoDatabase;
    AssertEx(m_pCDaoDatabase, _T("内存不足"));

    m_pCDaoRecordset = NEW CDaoRecordset(m_pCDaoDatabase);
    AssertEx(m_pCDaoRecordset, _T("内存不足"));

    ZeroMemoryArray(m_ResultBuff);
}

CMsAccess::~CMsAccess(VOID)
{
    SAFE_DELETE(m_pCDaoRecordset);
    SAFE_DELETE(m_pCDaoDatabase);
}

// 打开数据库文件
BOOL CMsAccess::Open(TCHAR* szDBName)
{
    _ENTER_FUN_C;

    BOOL bRet = m_pCDaoDatabase->IsOpen();
    if (bRet)
    {
        m_pCDaoDatabase->Close();
    }
    m_pCDaoDatabase->Open(szDBName);

    return m_pCDaoDatabase->IsOpen();;

    _DISPOSE_CUSTOM_END_C(CDaoException* pException)
    {
        m_pCDaoException = pException;
        fAssertEx(
            FALSE,
            _T("执行SQL语句错误.\n错误码[%d]\n源[%s]\n描述[%s]\n帮助[%s]"),
            m_pCDaoException->m_pErrorInfo->m_lErrorCode,
            m_pCDaoException->m_pErrorInfo->m_strSource,
            m_pCDaoException->m_pErrorInfo->m_strDescription,
            m_pCDaoException->m_pErrorInfo->m_strHelpFile
            );
    }
    _LEAVE_FUN;
    return FALSE;
}

// 关闭数据库文件
BOOL CMsAccess::Close(VOID)
{
    BOOL bRet = m_pCDaoDatabase->IsOpen();
    if (bRet)
    {
        m_pCDaoDatabase->Close();
    }
    bRet = m_pCDaoRecordset->IsOpen();
    if (bRet)
    {
        m_pCDaoRecordset->Close();
    }

    return !(m_pCDaoDatabase->IsOpen() || m_pCDaoRecordset->IsOpen());

}

// 执行SQL语句
VOID CMsAccess::ExecuteSQL(TCHAR* szSQLText)
{
    _ENTER_FUN_C;

    if (m_pCDaoRecordset->IsOpen())
    {
        m_pCDaoRecordset->Close();
    }
    m_pCDaoRecordset->Open(dbOpenDynaset, szSQLText);

    _DISPOSE_CUSTOM_END_C(CDaoException* pException)
    {
        m_pCDaoException = pException;
        fAssertEx(
            FALSE,
            _T("执行SQL语句错误.\n错误码[%d]\n源[%s]\n描述[%s]\n帮助[%s]"),
            m_pCDaoException->m_pErrorInfo->m_lErrorCode,
            m_pCDaoException->m_pErrorInfo->m_strSource,
            m_pCDaoException->m_pErrorInfo->m_strDescription,
            m_pCDaoException->m_pErrorInfo->m_strHelpFile
            );
    }
    _LEAVE_FUN;
}

// 获取结果记录个数
LONG CMsAccess::GetResultCount(VOID)
{
    LONG lCount = m_pCDaoRecordset->GetRecordCount();
    if (lCount)
    {
        m_pCDaoRecordset->MoveLast();
        m_pCDaoRecordset->MoveFirst();
        return m_pCDaoRecordset->GetRecordCount();
    }
    else
    {
        return 0;
    }
}

// 获取结果字段个数
SHORT CMsAccess::GetFieldCount(VOID)
{
    return m_pCDaoRecordset->GetFieldCount();
}

// 第一个
VOID CMsAccess::MoveFirst(VOID)
{
    m_pCDaoRecordset->MoveFirst();
}

// 最后一个
VOID CMsAccess::MoveLast(VOID)
{
    m_pCDaoRecordset->MoveLast();
}

// 上一个
VOID CMsAccess::MoveNext(VOID)
{
    m_pCDaoRecordset->MoveNext();
}

// 下一个
VOID CMsAccess::MovePrev(VOID)
{
    m_pCDaoRecordset->MovePrev();
}

// 获取结果记录,根据字段名
COleVariant* CMsAccess::GetResultValue(TCHAR* szFieldName)
{
    m_pCDaoRecordset->GetFieldValue(szFieldName, m_COleVariant);
    return &m_COleVariant;
}

// 获取结果记录,根据字段索引
COleVariant* CMsAccess::GetResultValue(INT nIndex)
{
    m_pCDaoRecordset->GetFieldValue(nIndex, m_COleVariant);
    return &m_COleVariant;
}
