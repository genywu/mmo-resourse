#include "Precompiled.h"
#include "MsPoolManager.h"

// 池管理器构造函数
CMsPoolManager::CMsPoolManager(
    DWORD dwPoolMaxCount,
    BOOL bNoData,
    BOOL bAutoInit,
    BOOL bAutoLock
    )
: m_lppCPoolMember          (NULL)
, m_dwPoolMaxCount          (dwPoolMaxCount)
, m_dwCurrentMemberCount    (0)
, m_lpdwLandArray           (NULL)
, m_ePoolErrorCode          (MS_ERROR_SUCCEED)
, m_bInit                   (FALSE)
, m_bLock                   (bAutoLock)
, m_bNoData                 (bNoData)
{
    _ENTER_FUN_C;

    AssertEx(AL_NORMAL, m_dwPoolMaxCount, _T("池的最大尺寸不能为0,错误调用"));

    ZeroMemoryStruct(m_stLandTime);

    m_lppCPoolMember = NEW CPoolMember*[m_dwPoolMaxCount];
    if (!m_bNoData)
    {
        ZeroMemory(m_lppCPoolMember, sizeof(CPoolMember*) * m_dwPoolMaxCount);
    }
    else
    {
        if (m_lppCPoolMember)
        {
            FOR_x_TO(i, m_dwPoolMaxCount)
            {
                m_lppCPoolMember[i] = NEW CNoDataPoolMember;
                AssertEx(
					AL_CATAST,
                    m_lppCPoolMember[i],
                    _T("m_lppCPoolMember[i]为空,内存不足")
                    );
            }
        }
        else
        {
            AssertEx(AL_CATAST, m_lppCPoolMember, _T("m_lpCPoolMember为空,内存不足"));
        }
    }

    m_lpdwLandArray = NEW DWORD[m_dwPoolMaxCount];
    AssertEx(AL_CATAST, m_lpdwLandArray, _T("m_lpdwLandArray为空,内存不足"));
    ZeroMemory(m_lpdwLandArray, sizeof(DWORD) * m_dwPoolMaxCount);

    if (bAutoInit)
    {
        this->Init();
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}

// 池子管理器析构函数
CMsPoolManager::~CMsPoolManager(VOID)
{
    _ENTER_FUN_C;

    this->Release();

    SAFE_DELETE_ARRAY(m_lpdwLandArray);
    if (!m_bNoData)
    {
        SAFE_DELETE_ARRAY(m_lppCPoolMember);
    }
    else
    {
        FOR_x_TO(i, m_dwPoolMaxCount)
        {
            SAFE_DELETE(m_lppCPoolMember[i]);
        }
        SAFE_DELETE_ARRAY(m_lppCPoolMember);
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
}

// 池子管理器初始化函数
BOOL CMsPoolManager::Init(VOID)
{
    _ENTER_FUN_C;

    if (m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_SUCCEED;
        return TRUE;
    }

    // 循环初始化状态数组中每个位置都可用
    for (DWORD i = 0; i < m_dwPoolMaxCount; i++)
    {
        // 使状态数组中每个位置都指向数据数组的对应位置
        m_lpdwLandArray[i] = i;
    }

    // 返回成功
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    return m_bInit = TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

// 释放池子管理器
BOOL CMsPoolManager::Release(VOID)
{
    _ENTER_FUN_C;

    // 去掉初始化标记
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    m_dwCurrentMemberCount = 0;
    ZeroMemoryStruct(m_stLandTime);
    ZeroMemory(m_lpdwLandArray, sizeof(DWORD) * m_dwPoolMaxCount);
    m_bInit = FALSE;
    return !m_bInit;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

// 重置索引池子
BOOL CMsPoolManager::Reset(VOID)
{
    this->Release();
    return this->Init();
}


// 获取当前元素个数
DWORD CMsPoolManager::GetCurrentCount(VOID)
{
    if (!m_bInit)
    {
        // 管理器未初始化
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return INVALID_UID;
    }
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    return m_dwCurrentMemberCount;
}

// 获取最大元素个数
DWORD CMsPoolManager::GetMaxCount(VOID)
{
    return m_dwPoolMaxCount;
}

// 检查池是否已经满了
BOOL CMsPoolManager::IsFull(VOID)
{
    if (!m_bInit)
    {
        // 管理器未初始化
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return INVALID_NID;
    }
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    return (m_dwCurrentMemberCount < m_dwPoolMaxCount) ?  FALSE : TRUE;
}

// 检查池是否已经空了
BOOL CMsPoolManager::IsEmpty(VOID)
{
    if (!m_bInit)
    {
        // 管理器未初始化
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return INVALID_NID;
    }
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    return m_dwCurrentMemberCount ? FALSE : TRUE;
}

// 添加元素到池中
BOOL CMsPoolManager::Append(CPoolMember* lpData)
{
    if (!m_bInit)
    {
        // 管理器未初始化
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return INVALID_NID;
    }
    _ENTER_FUN_C;

    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);
    AssertEx(AL_NORMAL, lpData, _T("lpData不能为空"));

    // 检查池子是否已满
    if (this->IsFull())
    {
        m_ePoolErrorCode = MS_ERROR_BUFFER_NOT_ENOUGH_FREE;
        // 返回添加失败
        return FALSE;
    }

    // 当前可用的池子索引
    DWORD dwCurrentLandIndexByUseableness = 
        m_lpdwLandArray[m_dwCurrentMemberCount];

    // 记录数据的地址
    m_lppCPoolMember[dwCurrentLandIndexByUseableness] = lpData;

    // 将数据中的池子信息写入
    if (lpData)
    {
        lpData->m_dwIndexInPool     = dwCurrentLandIndexByUseableness;
        lpData->m_dwIndexInLocate   = m_dwCurrentMemberCount++;
        ::GetLocalTime(&m_stLandTime);
        ::SystemTimeToFileTime(
            &m_stLandTime,
            (LPFILETIME)&lpData->m_n64EnterIndex
            );

        m_ePoolErrorCode = MS_ERROR_SUCCEED;
        return TRUE;
    }
    else
    {
        AssertEx(AL_NORMAL, lpData,_T("lpData为空"));
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

// 删除元素地址到池中
BOOL CMsPoolManager::Remove(CPoolMember* lpData)
{
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return INVALID_NID;
    }
    _ENTER_FUN_C;

    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);
    if (lpData == m_lppCPoolMember[lpData->m_dwIndexInPool])
    {
        return this->RemoveByPoolIndex(lpData->m_dwIndexInPool);
    }
    else
    {
        return FALSE;
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

// 根据池子索引删除
BOOL CMsPoolManager::RemoveByPoolIndex(DWORD dwIndexInPool)
{
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return FALSE;
    }
    _ENTER_FUN_C;

    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);

    // 检查池是否已经空了
    if (this->IsEmpty())
    {
        m_ePoolErrorCode = MS_ERROR_BUFFER_NOT_ENOUGH_DATA;
        // 返回删除失败
        return FALSE;
    }

    // 获取要删除的元素
    CPoolMember* lpCPoolMember  = m_lppCPoolMember[dwIndexInPool];

    // 获取要删除元素所在的登陆位置
    DWORD dwIndexInLand         = lpCPoolMember->m_dwIndexInLocate;

    // 验证池子索引值与传入值是否匹配
    AssertEx(
		AL_NORMAL,
        dwIndexInPool == m_lpdwLandArray[dwIndexInLand],
        _T("数据异常")
        );

    // 实现将被删除的元素与最后一个在线元素的索引信息交换，
    // 并将最后一个在线信息释放。
    {
        // 将元素中的池子信息清除
        lpCPoolMember->m_dwIndexInLocate    = INVALID_UID;
        lpCPoolMember->m_dwIndexInPool      = INVALID_UID;
        lpCPoolMember->m_n64EnterIndex      = INVALID_NID;

        // 提前减少计数是为了直接定位到最后一个在线元素,
        // 避免重复计算提高效率。
        m_dwCurrentMemberCount--;   // 减少一个成员个数

        // 被移动的元素的池子索引(登陆数组中在线的最后一个元素)
        DWORD dwPoolIndexByMotion = m_lpdwLandArray[m_dwCurrentMemberCount];

        // 将被删除元素的池子索引修改成最后一个登陆的元素的池子索引
        m_lpdwLandArray[dwIndexInLand]  = dwPoolIndexByMotion;

        // 将被移动的元素中的池子信息更新
        m_lppCPoolMember[dwPoolIndexByMotion]->m_dwIndexInLocate =
            dwIndexInLand;

        // 将最后登录的元素的池子索引修改成被删除元素的池子索引
        m_lpdwLandArray[m_dwCurrentMemberCount] = dwIndexInPool;

        if (!m_bNoData)
        {
            // 将元素数组中的指针清除
            m_lppCPoolMember[dwIndexInPool] = NULL;
        }
    }

    // 返回成功
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

BOOL CMsPoolManager::RemoveByLandIndex(DWORD dwIndexInLand)
{
    _ENTER_FUN_C;

    // 检查要删除的索引位置是否存在
    AssertEx(
		AL_NORMAL,
		(dwIndexInLand < m_dwCurrentMemberCount),
        _T("dwLandIndex值异常,调用错误")
        );

    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);

    // 间接调用池子索引进行删除
    return this->RemoveByPoolIndex(m_lpdwLandArray[dwIndexInLand]);

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

DWORD CMsPoolManager::GetNextIndex(BOOL bPossess)
{
    _ENTER_FUN_C;
    AssertEx(AL_NORMAL, m_bNoData, _T("调用错误,此函数不允许如此调用->请使用无数据模式"));
    if (!m_bInit)
    {
        // 管理器未初始化
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return INVALID_UID;
    }

    // 占有模式
    if (bPossess)
    {
        IS_AUTO_LOCK(m_MsPoolLock, m_bLock);

        // 检查池子是否已满
        if (this->IsFull())
        {
            m_ePoolErrorCode = MS_ERROR_BUFFER_NOT_ENOUGH_FREE;
            // 返回添加失败
            return INVALID_UID;
        }

        // 当前可用的池子索引
        DWORD dwCurrentLandIndexByUseableness =
            m_lpdwLandArray[m_dwCurrentMemberCount];

        // 记录数据的地址
        CPoolMember* lpData =
            m_lppCPoolMember[dwCurrentLandIndexByUseableness];

        // 将数据中的池子信息写入
        lpData->m_dwIndexInPool     = dwCurrentLandIndexByUseableness;
        lpData->m_dwIndexInLocate   = m_dwCurrentMemberCount++;
        ::GetLocalTime(&m_stLandTime);
        ::SystemTimeToFileTime(
            &m_stLandTime,
            (LPFILETIME)&lpData->m_n64EnterIndex
            );

        m_ePoolErrorCode = MS_ERROR_SUCCEED;
        return dwCurrentLandIndexByUseableness;
    }

    // 非占有模式
    else
    {
        IS_AUTO_LOCK(m_MsPoolLock, m_bLock);

        // 检查池子是否已满
        if (this->IsFull())
        {
            m_ePoolErrorCode = MS_ERROR_BUFFER_NOT_ENOUGH_FREE;
            // 返回添加失败
            return INVALID_UID;
        }
        // 当前可用的池子索引
        m_ePoolErrorCode = MS_ERROR_SUCCEED;
        return m_lpdwLandArray[m_dwCurrentMemberCount];
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return INVALID_UID;
}

// 根据池子释放索引位置
BOOL CMsPoolManager::FreeIndexByPool(DWORD dwIndexInPool)
{
    _ENTER_FUN_C;
    AssertEx(AL_NORMAL, m_bNoData, _T("调用错误,此函数不允许如此调用->请使用无数据模式"));
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return FALSE;
    }

    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);

    // 检查池是否已经空了
    if (this->IsEmpty())
    {
        m_ePoolErrorCode = MS_ERROR_BUFFER_NOT_ENOUGH_DATA;
        // 返回删除失败
        return FALSE;
    }

    // 获取要删除的元素
    CPoolMember* lpCPoolMember  = m_lppCPoolMember[dwIndexInPool];

    // 获取要删除元素所在的登陆位置
    DWORD dwIndexInLand         = lpCPoolMember->m_dwIndexInLocate;

    // 验证池子索引值与传入值是否匹配
    AssertEx(
		AL_NORMAL,
        dwIndexInPool == m_lpdwLandArray[dwIndexInLand],
        _T("数据异常")
        );

    // 实现将被删除的元素与最后一个在线元素的索引信息交换，
    // 并将最后一个在线信息释放。
    {
        // 将元素中的池子信息清除
        lpCPoolMember->m_dwIndexInLocate    = INVALID_UID;
        lpCPoolMember->m_dwIndexInPool      = INVALID_UID;
        lpCPoolMember->m_n64EnterIndex      = INVALID_NID;

        // 提前减少计数是为了直接定位到最后一个在线元素,
        // 避免重复计算提高效率。
        m_dwCurrentMemberCount--;   // 减少一个成员个数

        // 被移动的元素的池子索引(登陆数组中在线的最后一个元素)
        DWORD dwPoolIndexByMotion = m_lpdwLandArray[m_dwCurrentMemberCount];

        // 将被删除元素的池子索引修改成最后一个登陆的元素的池子索引
        m_lpdwLandArray[dwIndexInLand]  = dwPoolIndexByMotion;

        // 将被移动的元素中的池子信息更新
        m_lppCPoolMember[dwPoolIndexByMotion]->m_dwIndexInLocate =
            dwIndexInLand;

        // 将最后登录的元素的池子索引修改成被删除元素的池子索引
        m_lpdwLandArray[m_dwCurrentMemberCount] = dwIndexInPool;
    }

    // 返回成功
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

// 根据登陆释放索引位置
BOOL CMsPoolManager::FreeIndexByLand(DWORD dwIndexInLand)
{
    _ENTER_FUN_C;
    AssertEx(AL_NORMAL, m_bNoData, _T("调用错误,此函数不允许如此调用->请使用无数据模式"));

    // 检查要删除的索引位置是否存在
    AssertEx(
		AL_NORMAL,
        (dwIndexInLand < m_dwCurrentMemberCount),
        _T("dwLandIndex值异常,调用错误")
        );

    // 间接调用池子索引进行删除
    return this->FreeIndexByPool(m_lpdwLandArray[dwIndexInLand]);

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    m_ePoolErrorCode = MS_ERROR_EXCEPTION;
    return FALSE;
}

// 根据池子索引获取元素
CPoolMember* CMsPoolManager::GetDataByPool(DWORD dwIndexByPool)
{
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return NULL;
    }
    _ENTER_FUN_C;

    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);

    // 检查池是否已经空了
    if (this->IsEmpty())
    {
        m_ePoolErrorCode = MS_ERROR_BUFFER_NOT_ENOUGH_DATA;
        // 返回删除失败
        return NULL;
    }

    CPoolMember* pMember = m_lppCPoolMember[dwIndexByPool];

    // 如果指定位置的数据无效
    if (INVALID_NID == pMember->m_dwIndexInPool ||
        INVALID_NID == pMember->m_dwIndexInLocate)
    {
        m_ePoolErrorCode = MS_ERROR_INVALID_PARAMETER;
        return NULL;
    }
    else
    {
        m_ePoolErrorCode = MS_ERROR_SUCCEED;
        return pMember;
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return NULL;
}

// 根据登陆索引获取元素
CPoolMember* CMsPoolManager::GetDataByLand(DWORD dwIndexByLand)
{
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return NULL;
    }
    _ENTER_FUN_C;

    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);

    // 检查池是否已经空了
    if (this->IsEmpty())
    {
        m_ePoolErrorCode = MS_ERROR_BUFFER_NOT_ENOUGH_DATA;
        // 返回删除失败
        return NULL;
    }

    CPoolMember* pMember = m_lppCPoolMember[m_lpdwLandArray[dwIndexByLand]];

    // 如果指定位置的数据无效
    if (INVALID_NID == pMember->m_dwIndexInPool ||
        INVALID_NID == pMember->m_dwIndexInLocate)
    {
        m_ePoolErrorCode = MS_ERROR_INVALID_PARAMETER;
        return NULL;
    }
    else
    {
        m_ePoolErrorCode = MS_ERROR_SUCCEED;
        return pMember;
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return NULL;
}

// 获取错误代码
E_MS_ERROR CMsPoolManager::GetLastErrorCMsPoolManager(VOID)
{
    IS_AUTO_LOCK(m_MsPoolLock, m_bLock);
    return m_ePoolErrorCode;
}

VOID CMsPoolManager::PoolLock(VOID)
{
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
    }
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    m_MsPoolLock.Lock();
}

VOID CMsPoolManager::PoolUnLock(VOID)
{
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
    }
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    m_MsPoolLock.UnLock();
}

// 搜寻索引转换成池子索引
DWORD CMsPoolManager::LocateIndexToPoolIndex(DWORD dwIndexInLand)
{
    if (!m_bInit)
    {
        m_ePoolErrorCode = MS_ERROR_NO_INIT;
        //AssertEx(AL_NORMAL, FALSE, _T("池子管理器未初始化"));
        return INVALID_UID;
    }
    m_ePoolErrorCode = MS_ERROR_SUCCEED;
    return m_lpdwLandArray[dwIndexInLand];
}

// 根据池子索引获取元素登陆时间
LPFILETIME CMsPoolManager::GetDataLandTime(DWORD dwIndexByPool)
{
    CPoolMember* pMember = m_lppCPoolMember[dwIndexByPool];

    // 如果指定位置的数据无效
    if (INVALID_NID == pMember->m_dwIndexInPool ||
        INVALID_NID == pMember->m_dwIndexInLocate)
    {
        m_ePoolErrorCode = MS_ERROR_INVALID_PARAMETER;
        return NULL;
    }
    else
    {
        m_ePoolErrorCode = MS_ERROR_SUCCEED;
        return pMember->GetLandTime();
    }
}
