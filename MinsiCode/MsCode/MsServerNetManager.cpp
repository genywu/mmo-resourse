#include "Precompiled.h"

CMsServerNetManager::CMsServerNetManager(
    TCHAR* szIP,
    WORD wProt,
    DWORD dwLogicThreadId,
    DWORD dwMaxClientCount,
    CMsPacketFactory* pCMsPacketFactory
    )
: m_wProt               (wProt)
, m_MsServerNetLog      (_T("%s/Log/MsServerNetManager.Log"), TRUE)
, m_ppMsClientSocket    (NULL)
, m_bRunning            (FALSE)
, m_dwLogicThreadId     (dwLogicThreadId)
, m_pCMsPacketFactory   (pCMsPacketFactory)
{
    _ENTER_FUN_C;

    m_MsServerNetLog.SetSaveLog(TRUE);

    this->LogServerNet(_T("开始创建!"));

    AssertEx(m_pCMsPacketFactory, _T("封包工厂不能为空"));
    if (szIP)
    {
#ifdef UNICODE
        ::WideCharToMultiByte(
            CP_ACP, 
            0, 
            szIP,
            (INT)wcslen(szIP) + 1,
            m_szIp,
            IP_SIZE,
            NULL,
            NULL
            );
#else
        _tcscpy_s(m_szIp, IP_SIZE, szIP);
#endif
    }
    else
    {
        AssertEx(szIP, _T("IP地址不能为空"));
    }

    this->LogServerNet(_T("创建服务器套接字!"));
    m_pMsServerSocket = NEW CMsServerSocket(
        MAX_SOCK_BUFF_SIZE,
        dwLogicThreadId,
        m_dwThreadId,
        m_pCMsPacketFactory
        );
    if (m_pMsServerSocket)
    {
        ;
    }
    else
    {
        AssertEx(m_pMsServerSocket, _T("内存不足"));
    }

    this->LogServerNet(_T("创建完成端口!"));
    m_lpCMsIocpManager = NEW CMsIocpManager(_T("%s/Log/CMsServerIocpManager.log"), m_dwThreadId);
    this->LogServerNet(_T("创建索引池子!"));
    m_pCMsPoolManager = NEW CMsPoolManager(dwMaxClientCount, TRUE);
    this->LogServerNet(_T("创建客户端套接字数组!"));
    m_ppMsClientSocket = NEW lpCMsClientSocket[dwMaxClientCount];
    if (m_pCMsPoolManager && m_ppMsClientSocket)
    {
        FOR_x_TO(i, dwMaxClientCount)
        {
            m_ppMsClientSocket[i] = NEW CMsClientSocket(
                MAX_SOCK_BUFF_SIZE,
                dwLogicThreadId,
                m_dwThreadId,
                m_pCMsPacketFactory
                );
            if (m_ppMsClientSocket[i])
            {
                ;
            }
            else
            {
                AssertEx(m_ppMsClientSocket[i], _T("内存不足"));
            }
        }
    }
    else
    {
        AssertEx(m_pCMsPoolManager, _T("内存不足"));
        AssertEx(m_ppMsClientSocket, _T("内存不足"));
    }

    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_addr.s_addr = ::inet_addr(m_szIp);
    m_ServerAddr.sin_port = ::htons(m_wProt);

    this->LogServerNet(_T("创建成功!"));
    _LEAVE_FUN_DISPOSE_END_C;
}

CMsServerNetManager::~CMsServerNetManager(VOID)
{
    this->LogServerNet(_T("开始释放!"));
    this->StopNet();
    DWORD dwMaxClientCount = m_pCMsPoolManager->GetMaxCount();
    FOR_x_TO(i, dwMaxClientCount)
    {
        SAFE_DELETE(m_ppMsClientSocket[i]);
    }
    SAFE_DELETE_ARRAY(m_ppMsClientSocket);
    SAFE_DELETE(m_pCMsPoolManager);
    SAFE_DELETE(m_pMsServerSocket);
    SAFE_DELETE(m_lpCMsIocpManager);

    this->LogServerNet(_T("释放成功!"));
}

// 记录日志
VOID CMsServerNetManager::LogServerNet(TCHAR* szLog)
{
    if (szLog)
    {
        m_MsServerNetLog << L_LOCK << L_TIME;
        m_MsServerNetLog.Logf(_T("[服务器网络管理器]\t%s"), szLog);
        m_MsServerNetLog << L_NEWLINE << L_UNLOCK;
    }
}

// 判断是否有效
BOOL CMsServerNetManager::IsValid(VOID)
{
    return m_pMsServerSocket->IsValid();
}

// 启动服务器
BOOL CMsServerNetManager::RunNet(VOID)
{
    _ENTER_FUN_C;

    SOCKET sock = m_pMsServerSocket->GetSocket();

    // 设置套接字为非阻塞模式
    ULONG IsNoEncumber = TRUE;
    ::ioctlsocket(sock, FIONBIO, &IsNoEncumber);

    this->LogServerNet(_T("绑定服务器!"));
    // 捆绑当前服务器地址
    INT nRet = ::bind(sock, (SOCKADDR*)&m_ServerAddr, sizeof(m_ServerAddr));
    AssertExFormat(SOCKET_ERROR != nRet, _T("::bind失败\n%s"), m_MsServerNetLog.ShowLastError());

    this->LogServerNet(_T("监听服务器!"));
    // 监听服务器
    nRet = ::listen(sock, MAX_COUNT_LISTEN_IN_ONE_FRAME);
    AssertExFormat(SOCKET_ERROR != nRet, _T("::listen失败\n%s"), m_MsServerNetLog.ShowLastError());

    m_bRunning = TRUE;

    this->LogServerNet(_T("启动服务器成功!"));
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    this->LogServerNet(_T("启动服务器失败!"));
    return FALSE;
}

// 断开服务器
BOOL CMsServerNetManager::StopNet(VOID)
{
    if (!m_bStop)
    {
        this->LogServerNet(_T("开始停止服务器!"));
        DWORD dwMaxClientCount = m_pCMsPoolManager->GetMaxCount();
        FOR_x_TO(i, dwMaxClientCount)
        {
            m_ppMsClientSocket[i]->Reset();
        }
        m_pCMsPoolManager->Reset();

        m_lpCMsIocpManager->Stop(TRUE);

        this->Stop();
        this->LogServerNet(_T("停止服务器成功!"));
    }
    return TRUE;
}

// 接受连接
BOOL CMsServerNetManager::Accept(VOID)
{
    _ENTER_FUN_C;

    // 获取池子下个位置
    DWORD dwNextIndex = m_pCMsPoolManager->GetNextIndex(FALSE);

    // 池子满返回失败
    if (INVALID_UID == dwNextIndex)
    {
        return FALSE;
    }

    // 获取服务器信息
    SOCKET ServerSocket = m_pMsServerSocket->GetSocket();
    INT nSockAddrSize = sizeof(m_ServerAddr);

    // 接受新连接
    SOCKET AcceptSocket = ::WSAAccept(
        ServerSocket,
        (SOCKADDR*)&m_ServerAddr,
        &nSockAddrSize,
        NULL,
        0
        );

    // 如果有新连接
    if (AcceptSocket != INVALID_SOCKET)
    {
        this->LogServerNet(_T("服务器接受到新的连接!"));
        // 占用一个池子位置
        m_pCMsPoolManager->GetNextIndex(TRUE);

        // 将池子位置记录到套接字中
        m_ppMsClientSocket[dwNextIndex]->SetPoolIndex(dwNextIndex);
        // 将连接套接字记录到套接字中
        m_ppMsClientSocket[dwNextIndex]->SetSocket(AcceptSocket);

        this->LogServerNet(_T("绑定新连接到完成端口!"));
        // 绑定完成端口
        BOOL bRet = m_lpCMsIocpManager->AssociateDevice(
            AcceptSocket, 
            dwNextIndex
            );
        AssertEx(bRet, _T("套接字绑定完成端口失败"));

        this->LogServerNet(_T("通知有新的连接进入!"));
        return ::PostThreadMessage(m_dwLogicThreadId, MSM_SERVER_SOCKET_ON_ENTER, (WPARAM)m_ppMsClientSocket[dwNextIndex], 0);
    }
    else
    {
        return FALSE;
    }

    _LEAVE_FUN_DISPOSE_END_C;
    this->LogServerNet(_T("接受新连接失败!"));
    return FALSE;
}

// 接受客户端
BOOL CMsServerNetManager::NetAccept(VOID)
{
    // 停止之后不处理
    if (!m_bStop)
    {
        return this->Accept();
    }
    return TRUE;
}

// 消息处理
BOOL CMsServerNetManager::OnThreadMsgExecute(LPMSG lpMsg)
{
    // 无事通知
    if (!lpMsg)
    {
        FOR_x_TO(i, MAX_COUNT_LISTEN_IN_ONE_FRAME)
        {
            if (m_pCMsPoolManager && !m_pCMsPoolManager->IsFull())
            {
                // 接受新的客户端
                BOOL bRet = this->NetAccept();
                if (!bRet)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        ::SleepEx(10, TRUE);
        return TRUE;
    }

    // 消息通知
    switch (lpMsg->message)
    {
        // 有数据完成
    case MSM_IOCP_UPDATE:
        {
            // 完成端口通知结构体
            S_IOCP_UPDATE*  lpIocpUpdate = (S_IOCP_UPDATE*)lpMsg->wParam;
            m_ppMsClientSocket[lpIocpUpdate->m_ulpCompletionKey]->OnUpdate(lpIocpUpdate);
            return TRUE;
        }break;

        // 退出或错误
    case MSM_IOCP_EXIT_OR_ERROR:
        {
            CMsClientSocket* pCMsClientSocket = m_ppMsClientSocket[(ULONG_PTR)lpMsg->wParam];
            this->RemoveClient(pCMsClientSocket);
            return TRUE;
        }break;

        // 外部投递完成消息
    case MSM_IOCP_POST_QUEUED_COMPLETION_STATUS:
        {
            CMsSocket* lpSocket = (CMsSocket*)lpMsg->wParam;
            ULONG_PTR ulKey = lpSocket->GetPoolIndex();
            S_IOCP_UPDATE* pRecvIO = lpSocket->GetRecvIO();
            m_lpCMsIocpManager->PostCompletionStatus(ulKey, 0, pRecvIO);
        }break;

    default:
        {
            this->LogServerNet(_T("未知消息!"));
            AssertEx(FALSE, _T("未知IOCP消息"));
        }break;
    }
    return FALSE;
}

// 获取指定客户端套接字
lpCMsClientSocket CMsServerNetManager::GetClientSocket(DWORD dwIndexByPool)
{
    DWORD dwMaxClientCount = m_pCMsPoolManager->GetMaxCount();
    if (dwMaxClientCount <= dwIndexByPool || !m_ppMsClientSocket)
    {
        return NULL;
    }
    return m_ppMsClientSocket[dwIndexByPool];
}

// 获取当前客户端连接个数
DWORD CMsServerNetManager::GetCurrentClientCount(VOID)
{
    return m_pCMsPoolManager->GetCurrentCount();
}

// 关闭一个客户端
BOOL CMsServerNetManager::RemoveClient(CMsClientSocket* pCMsClientSocket)
{
    DWORD dwIndexByPool = (DWORD)pCMsClientSocket->GetPoolIndex();
    (m_MsServerNetLog << L_TIME).Logf(_T("[服务器网络管理器]\t[断开连接!][%d]\n"), dwIndexByPool);
    DWORD dwMaxClientCount = m_pCMsPoolManager->GetMaxCount();
    if (dwMaxClientCount <= dwIndexByPool || !m_ppMsClientSocket)
    {
        return FALSE;
    }
    else
    {
        this->LogServerNet(_T("通知有连接断开!"));
        ::PostThreadMessage(m_dwLogicThreadId, MSM_SERVER_SOCKET_ON_EXIT, (WPARAM)pCMsClientSocket, 0);
        return m_pCMsPoolManager->RemoveByPoolIndex(dwIndexByPool);
    }
}

// 线程事件通知
VOID CMsServerNetManager::OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent)
{
    BOOL bRet = TRUE;
    switch (eThreadEvent)
    {
    case E_ON_THREAD_RUN:
        {
            this->LogServerNet(_T("工作线程开始工作!"));
            m_lpCMsIocpManager->Run();
            // 直到启动成功或停止运行
            while (!m_bStop && !m_bRunning)
            {
                bRet = this->RunNet();
                if (bRet)
                {
                    break;
                }
                else
                {
                    ::SleepEx(100, TRUE);
                }
            }
        }break;

    case E_ON_THREAD_SUSPEND:
        {
            ;
        }break;

    default:
        {
            this->LogServerNet(_T("未知通知状态!"));
            AssertEx(FALSE, _T("严重错误逻辑"));
        }
    }
}