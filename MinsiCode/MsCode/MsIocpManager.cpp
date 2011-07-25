#include "Precompiled.h"
#include "MsIocpManager.h"

// 完成端口构造函数
CMsIocpManager::CMsIocpManager(TCHAR* szIocpLogFileName, DWORD dwThreadId)
: m_IsInit          (FALSE) // 当前对象创建时并未初始化
, m_hCompetionPort  (NULL)  // 完成端口句柄初始化为NULL
, m_MsIocpLog       (szIocpLogFileName, TRUE)
, m_dwParentThreadId(dwThreadId)
{
    _ENTER_FUN_C;

    m_MsIocpLog.SetSaveLog(FALSE);
    this->Init();

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}

// 完成端口析构函数
CMsIocpManager::~CMsIocpManager(VOID)
{
    _ENTER_FUN_C;

    this->Release();

    // 如果完成端口句柄被使用
    if (m_hCompetionPort)
    {
        // 释放完成端口对象
        ::CloseHandle(m_hCompetionPort);
        m_hCompetionPort = NULL;
    }
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}

// 初始化完成端口
BOOL CMsIocpManager::Init(VOID)
{
    _ENTER_FUN_C;
    this->LogIocp(_T("[开始初始化!]"));

    // 创建完成端口对象
    m_hCompetionPort = ::CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,   // 用来创建新的完成端口对象
        NULL,                   // 创建新的完成端口对象,必须为NULL
        0,                      // 创建新的完成端口对象,必须为0
        0);                     // 0代表使用当前系统CPU个数创建线程
    AssertEx(AL_CATAST, m_hCompetionPort, _T("完成端口创建失败,函数失败"));

    this->LogIocp(_T("[创建成功!]"));
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    this->LogIocp(_T("[创建失败!]"));
    return FALSE;
}

// 释放完成端口
BOOL CMsIocpManager::Release(VOID)
{
    _ENTER_FUN_C;
    this->LogIocp(_T("[开始释放!]"));

    BOOL bRet = TRUE;

    this->Stop(TRUE, TRUE);

    if (m_hCompetionPort)
    {
        bRet = ::CloseHandle(m_hCompetionPort);
        AssertEx(AL_CATAST, bRet, _T("释放完成端口失败,函数失败"));
        m_hCompetionPort = NULL;
    }
    m_IsInit = FALSE;

    this->LogIocp(_T("[释放成功!]"));
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    this->LogIocp(_T("[释放失败!]"));
    return FALSE;
}

// 记录日志
VOID CMsIocpManager::LogIocp(TCHAR* szLog)
{
    if (szLog)
    {
        m_MsIocpLog << L_LOCK_A << L_TIME;
        m_MsIocpLog.Logf(_T("[完成端口]\t\t%s"), szLog);
        m_MsIocpLog << L_NEWLINE << L_UNLOCK_A;
    }
}

// 绑定指定句柄到完成端口对象
BOOL CMsIocpManager::AssociateDevice(HANDLE hDevice, ULONG_PTR CompletionKey)
{
    _ENTER_FUN_C;
    this->LogIocp(_T("[绑定句柄]"));

    // 将新接受对象绑定到完成端口
    m_hCompetionPort = ::CreateIoCompletionPort(
        hDevice,            // 绑定这个句柄到完成端口
        m_hCompetionPort,   // 绑定到这个完成端口
        CompletionKey,      // 绑定后的唯一关键字(KEY)
        0);                 // 当绑定端口时忽略此参数
    fAssertEx(
		AL_CATAST,
        m_hCompetionPort,
        _T("完成端口绑定失败,函数失败\n%s"),
        m_MsIocpLog.ShowLastError()
        );

    this->LogIocp(_T("[绑定成功]"));
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    this->LogIocp(_T("[绑定失败]"));
    return FALSE;
}

// 绑定指定套接字到完成端口对象
BOOL CMsIocpManager::AssociateDevice(
    SOCKET hSocket,
    ULONG_PTR CompletionKey
    )
{
    _ENTER_FUN_C;
    this->LogIocp(_T("[开始绑定句柄]"));

    // 将新接受对象绑定到完成端口
    m_hCompetionPort = ::CreateIoCompletionPort(
        (HANDLE)hSocket,    // 绑定这个套接字到完成端口
        m_hCompetionPort,   // 绑定到这个完成端口
        CompletionKey,      // 绑定后的唯一关键字(KEY)
        0);                 // 当绑定端口时忽略此参数
    fAssertEx(
		AL_CATAST,
		m_hCompetionPort,
        _T("完成端口绑定失败,函数失败\n%s"),
        m_MsIocpLog.ShowLastError()
        );

    this->LogIocp(_T("[绑定成功]"));
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    this->LogIocp(_T("[绑定失败]"));
    return FALSE;
}

// 获取完成包状态
PORT_STATUS CMsIocpManager::GetCompletionStatus(
    ULONG_PTR* pCompletionKey,
    DWORD& dwNumOfBytesReceive,
    S_IOCP_UPDATE** lppOverlapped,
    DWORD dwWaitTime
    )
{
    _ENTER_FUN_C;

    BOOL bRet = FALSE;  // 布尔返回值

    // 获取完成包状态
    bRet = ::GetQueuedCompletionStatus(
        m_hCompetionPort,               // 获取哪个完成端口的状态
        &dwNumOfBytesReceive,           // 获取完成端口收到了多少个字节
        pCompletionKey,                 // 获取完成端口绑定中哪个关键字(KEY)的状态
        (LPOVERLAPPED*)lppOverlapped,   // 获取完成端口重叠状态结构体指针
        dwWaitTime);                    // 等待每个连接完成的时间

    // 如果完成包获取失败
    if (!bRet)
    {
        INT nErrorCode = ::GetLastError();
        if(WAIT_TIMEOUT == nErrorCode)
        {
            // 无事状态
            return COMPLETIONPORT_STATUS_NOTHING;
        }
        else if (ERROR_ABANDONED_WAIT_0 == nErrorCode)
        {
            this->LogIocp(_T("[连接断开]"));
            // 连接被主动退出
            return COMPLETIONPORT_STATUS_EXIT;
        }
        else
        {
            this->LogIocp(_T("[连接错误]"));
            // 连接错误
            return COMPLETIONPORT_STATUS_ERROR;
        }
    }

    // 如果完成包接收到了数据
    else if (dwNumOfBytesReceive == 0)
    {
        this->LogIocp(_T("[连接断开]"));
        // 连接被主动退出
        return COMPLETIONPORT_STATUS_EXIT;
    }

    else if (dwNumOfBytesReceive == INVALID_UID)
    {
        dwNumOfBytesReceive = 0;
        this->LogIocp(_T("[数据更新]"));
        // 返回有数据更新
        return COMPLETIONPORT_STATUS_UPDATAED;
    }
    else
    {
        this->LogIocp(_T("[数据更新]"));
        // 返回有数据更新
        return COMPLETIONPORT_STATUS_UPDATAED;
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    this->LogIocp(_T("[未知状态]"));
    return COMPLETIONPORT_STATUS_UNKNOW;
}

// 投递完成包状态
BOOL CMsIocpManager::PostCompletionStatus(
    ULONG_PTR CompletionKey,
    DWORD dwNumBytes,
    S_IOCP_UPDATE *pOverlapped
    )
{
    _ENTER_FUN_C;

    this->LogIocp(_T("[投递状态]"));

    return ::PostQueuedCompletionStatus(
        m_hCompetionPort,
        dwNumBytes,
        CompletionKey,
        pOverlapped
        );

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    this->LogIocp(_T("[投递状态失败]"));
    return FALSE;
}

// 线程回调
BOOL CMsIocpManager::ThreadCallBack(VOID)
{
    S_IOCP_UPDATE*  lpIocpUpdate;
    PORT_STATUS     PortStatus;
    ULONG_PTR       ulpCompletionKey = (ULONG_PTR)INVALID_NID;
    DWORD           dwNumOfBytesComplete;
    this->LogIocp(_T("[开始工作线程]"));
    while (!m_bStop)
    {
        PortStatus = this->GetCompletionStatus(
            &ulpCompletionKey,
            dwNumOfBytesComplete,
            &lpIocpUpdate,
            10
            );

        if (ulpCompletionKey == (ULONG_PTR)INVALID_NID)
        {
            continue;
        }

        switch (PortStatus)
        {
            // 有数据完成更新
        case COMPLETIONPORT_STATUS_UPDATAED:
            {
                lpIocpUpdate->m_PortStatus = PortStatus;
                lpIocpUpdate->m_ulpCompletionKey = ulpCompletionKey;
                lpIocpUpdate->m_dwNumOfBytesComplete = dwNumOfBytesComplete;
                this->LogIocp(_T("[通知数据更新]"));
                ::PostThreadMessage(m_dwParentThreadId, MSM_IOCP_UPDATE, (WPARAM)lpIocpUpdate, NULL);
            }break;

            // 异常断开
        case COMPLETIONPORT_STATUS_ERROR:

            // 正常断开
        case COMPLETIONPORT_STATUS_EXIT:
            {
                this->LogIocp(_T("[通知连接断开]"));
                ::PostThreadMessage(m_dwParentThreadId, MSM_IOCP_EXIT_OR_ERROR, (WPARAM)ulpCompletionKey, NULL);
            }break;
        }
    }
    this->LogIocp(_T("[成功退出]"));
    return TRUE;
}

// 停止之前
BOOL CMsIocpManager::ForeStop(VOID)
{
    return TRUE;
}

// 线程事件通知
VOID CMsIocpManager::OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent, BOOL bReady)
{
    USE_PARAM(eThreadEvent, bReady);
}