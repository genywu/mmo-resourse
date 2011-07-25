#include "Precompiled.h"
#include "MsPoolManager.h"

// 池管理器构造函数
CMsQueueManager::CMsQueueManager(DWORD dwMaxCount, BOOL bAutoLock)
: m_dwQueueMemberMaxCount   (dwMaxCount)
, m_lppQueueMemberData      (NULL)
, m_lpdwQueueMemberSize     (NULL)
, m_dwQueueCurrentCount     (0)
, m_dwQueueCurrentFirstUse  (INVALID_UID)
, m_dwQueueCurrentFirstFree (0)
, m_lpLandTime              (NULL)
, m_eQueueErrorCode         (ERROR_NO_ERROR)
, m_bInit                   (FALSE)
, m_bLock                   (bAutoLock)

{
    _ENTER_FUN_C;

    ZeroMemoryStruct(m_stLandTime);

    AssertEx(AL_NORMAL, m_dwQueueMemberMaxCount, _T("队列的最大尺寸不能为0,错误调用"));

    AssertEx(AL_NORMAL, this->Init(), _T("队列初始化失败"));

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}

// 队列管理器析构函数
CMsQueueManager::~CMsQueueManager(VOID)
{
    ;
}

// 队列管理器初始化函数
BOOL CMsQueueManager::Init(VOID)
{
    _ENTER_FUN_C;

    // 申请队列成员数组
    m_lppQueueMemberData = NEW LPVOID[m_dwQueueMemberMaxCount];
    AssertEx(AL_CATAST, m_lppQueueMemberData, _T("m_lppQueueMemberData为空,内存不足"));
    ZeroMemory(m_lppQueueMemberData, sizeof(LPVOID) * m_dwQueueMemberMaxCount);

    // 申请队列成员尺寸数组
    m_lpdwQueueMemberSize = NEW DWORD[m_dwQueueMemberMaxCount];
    AssertEx(AL_CATAST, m_lpdwQueueMemberSize, _T("m_lpdwQueueMemberSize为空,内存不足"));
    ZeroMemory(m_lpdwQueueMemberSize, sizeof(DWORD) * m_dwQueueMemberMaxCount);

    m_lpLandTime = NEW FILETIME[m_dwQueueMemberMaxCount];
    AssertEx(AL_CATAST, m_lpLandTime, _T("m_lpdwQueueMemberSize为空,内存不足"));
    ZeroMemory(m_lpLandTime, sizeof(FILETIME) * m_dwQueueMemberMaxCount);

    m_eQueueErrorCode = ERROR_NO_ERROR;
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    m_eQueueErrorCode = ERROR_OPERATE_EXCEPTION;
    return FALSE;
}

// 释放队列管理器
BOOL CMsQueueManager::Release(VOID)
{
    _ENTER_FUN_C;

    while (m_dwQueueCurrentCount)
    {
        this->DeleteData();
    }
    SAFE_DELETE_ARRAY(m_lppQueueMemberData);
    SAFE_DELETE_ARRAY(m_lpdwQueueMemberSize);
    SAFE_DELETE_ARRAY(m_lpLandTime);

    m_eQueueErrorCode = ERROR_NO_ERROR;
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    m_eQueueErrorCode = ERROR_OPERATE_EXCEPTION;
    return FALSE;
}

// 获取当前元素个数
DWORD CMsQueueManager::GetCurrentCount(VOID)
{
    m_eQueueErrorCode = ERROR_NO_ERROR;
    return m_dwQueueCurrentCount;
}

// 检查是否已经满了
BOOL CMsQueueManager::IsFull(VOID)
{
    m_eQueueErrorCode = ERROR_NO_ERROR;
    return m_dwQueueCurrentCount >= m_dwQueueMemberMaxCount;
}

// 检查是否已经空了
BOOL CMsQueueManager::IsEmpty(VOID)
{
    m_eQueueErrorCode = ERROR_NO_ERROR;
    return m_dwQueueCurrentCount ? FALSE : TRUE;
}

// 压入数据(数据靠内部存储)
BOOL CMsQueueManager::PushData(LPVOID lpData, DWORD dwSize)
{
    _ENTER_FUN_C;

    AssertEx(AL_NORMAL, FALSE, _T("局部堆内存申请,此函数被禁用"));
    if (this->IsFull())
    {
        m_eQueueErrorCode = ERROR_ISFULL;
        return FALSE;
    }

    lpData = NEW BYTE[dwSize];
    AssertEx(AL_CATAST, lpData, _T("lpData为空,内存不足"));

    BOOL bRet = this->PushData(lpData);
    if (bRet)
    {
        m_lpdwQueueMemberSize[m_dwQueueCurrentFirstFree] = dwSize;
        m_eQueueErrorCode = ERROR_NO_ERROR;
    }
    return bRet;

    _LEAVE_FUN_DISPOSE_END_C;
    m_eQueueErrorCode = ERROR_OPERATE_EXCEPTION;
    return FALSE;
}

// 压入数据(数据靠外部存储)
BOOL CMsQueueManager::PushData(LPVOID lpData)
{
    if (this->IsFull())
    {
        m_eQueueErrorCode = ERROR_ISFULL;
        return FALSE;
    }

    // 将数据放入数组中记录
    m_lppQueueMemberData[m_dwQueueCurrentFirstFree] = lpData;
    m_lpdwQueueMemberSize[m_dwQueueCurrentFirstFree] = 0;
    ::GetLocalTime(&m_stLandTime);
    ::SystemTimeToFileTime(
        &m_stLandTime,
        &m_lpLandTime[m_dwQueueCurrentFirstFree]
    );

    m_dwQueueCurrentFirstFree++;
    // 如果已经是最后一个空间,循环到头部重新使用
    if (m_dwQueueCurrentFirstFree >= (m_dwQueueMemberMaxCount - 1))
    {
        m_dwQueueCurrentFirstFree = 0;
    }

    // 个数自增
    m_dwQueueCurrentCount++;

    // 如果之前没有数据,将第一个已用位置定位
    if (INVALID_NID == m_dwQueueCurrentFirstUse)
    {
        m_dwQueueCurrentFirstUse = 0;
    }

    m_eQueueErrorCode = ERROR_NO_ERROR;
    return TRUE;
}

// 获取数据(数据外部存储时,尺寸为0)
LPVOID CMsQueueManager::GetData(DWORD& dwSize)
{
    if (this->IsEmpty())
    {
        m_eQueueErrorCode = ERROR_ISEMPTY;
        return NULL;
    }
    AssertEx(
		AL_NORMAL,
        (0 == m_lpdwQueueMemberSize[m_dwQueueCurrentFirstUse]),
        _T("局部堆内存申请,此函数被禁用")
        );

    dwSize = m_lpdwQueueMemberSize[m_dwQueueCurrentFirstUse];

    LPVOID lpRet = m_lppQueueMemberData[m_dwQueueCurrentFirstUse];

    //m_dwQueueCurrentFirstUse++;
    //// 如果已经是最后一个空间,循环到头部重新使用
    //if (m_dwQueueCurrentFirstUse >= (m_dwQueueMemberMaxCount - 1))
    //{
    //    m_dwQueueCurrentFirstUse = 0;
    //}

    return lpRet;
}

// 删除数据
VOID CMsQueueManager::DeleteData(VOID)
{
    if (this->IsEmpty())
    {
        m_eQueueErrorCode = ERROR_ISEMPTY;
        return;
    }

    if (0 != m_lpdwQueueMemberSize[m_dwQueueCurrentFirstUse])
    {
        AssertEx(AL_NORMAL, FALSE, _T("局部堆内存申请,此函数被禁用"));
        SAFE_DELETE_ARRAY(m_lppQueueMemberData[m_dwQueueCurrentFirstUse]);
        m_lpdwQueueMemberSize[m_dwQueueCurrentFirstUse] = 0;
    }
    if (INVALID_NID != m_dwQueueCurrentFirstUse)
    {
        m_lppQueueMemberData[m_dwQueueCurrentFirstUse] = NULL;
        m_dwQueueCurrentCount--;
        //if (m_dwQueueCurrentCount > 0)
        {
            m_dwQueueCurrentFirstUse++;
            // 如果已经是最后一个空间,循环到头部重新使用
            if (m_dwQueueCurrentFirstUse >= (m_dwQueueMemberMaxCount - 1))
            {
                m_dwQueueCurrentFirstUse = 0;
            }
        }
        //else
        //{
            //m_dwQueueCurrentFirstUse = INVALID_NID;
        //}
    }
}

// 锁定队列
VOID CMsQueueManager::QueueLock(VOID)
{
    m_eQueueErrorCode = ERROR_NO_ERROR;
    m_MsQueueLock.Lock();
}

// 解锁队列
VOID CMsQueueManager::QueueUnLock(VOID)
{
    m_eQueueErrorCode = ERROR_NO_ERROR;
    m_MsQueueLock.UnLock();
}

// 获取错误代码
CMsQueueManager::E_ERROR_CODE
CMsQueueManager::GetLastErrorCMsQueueManager(VOID)
{
    IS_AUTO_LOCK(m_MsQueueLock, m_bLock);
    return m_eQueueErrorCode;
}

// 获取下个可用索引位置
ULONG_PTR CMsQueueManager::GetNextIndex(BOOL bPossess)
{
    _ENTER_FUN_C;

    // 占有模式
    if (bPossess)
    {
        IS_AUTO_LOCK(m_MsQueueLock, m_bLock);

        // 检查队列是否已满
        if (this->IsFull())
        {
            m_eQueueErrorCode = ERROR_ISFULL;
            // 返回添加失败
            return INVALID_UID;
        }

        ULONG_PTR ulFreeIndex = m_dwQueueCurrentFirstFree;
        this->PushData((LPVOID)ulFreeIndex);
        return ulFreeIndex;
    }

    // 非占有模式
    else
    {
        IS_AUTO_LOCK(m_MsQueueLock, m_bLock);

        // 检查队列是否已满
        if (this->IsFull())
        {
            m_eQueueErrorCode = ERROR_ISFULL;
            // 返回添加失败
            return INVALID_UID;
        }

        m_eQueueErrorCode = ERROR_NO_ERROR;
        return m_dwQueueCurrentFirstFree;
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_eQueueErrorCode = ERROR_OPERATE_EXCEPTION;
    return INVALID_UID;
}

// 清除所有数据
BOOL CMsQueueManager::ClearAll(VOID)
{
    while (m_dwQueueCurrentCount)
    {
        this->DeleteData();
    }
    return TRUE;
}
