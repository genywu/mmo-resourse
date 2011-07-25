#include "Precompiled.h"

CMsClientNetManager::CMsClientNetManager(
    TCHAR* szIP,
    WORD wProt,
    DWORD dwLogicThreadId,
    CMsPacketFactory* pCMsPacketFactory
    )
: m_wProt               (wProt)
, m_MsClientNetLog      (_T("%s/Log/MsClientNetManager.Log"), TRUE)
, m_pMsServerSocket     (NULL)
, m_bConnected          (FALSE)
, m_dwLogicThreadId     (dwLogicThreadId)
, m_pCMsPacketFactory   (pCMsPacketFactory)
{
    _ENTER_FUN_C;

    m_MsClientNetLog.SetSaveLog(FALSE);

    this->LogClientNet(_T("开始创建!"));

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

    this->LogClientNet(_T("创建完成端口!"));
    m_lpCMsIocpManager = NEW CMsIocpManager(_T("%s/Log/CMsClientIocpManager.log"), m_dwThreadId);
    this->LogClientNet(_T("创建服务器套接字!"));
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

    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_addr.s_addr = ::inet_addr(m_szIp);
    m_ServerAddr.sin_port = ::htons(m_wProt);

    this->LogClientNet(_T("创建成功!"));
    _LEAVE_FUN_DISPOSE_END_C;
}

CMsClientNetManager::~CMsClientNetManager(VOID)
{
    _ENTER_FUN_C;
    this->LogClientNet(_T("开始释放!"));
    this->Disconnect();
    SAFE_DELETE(m_pMsServerSocket);
    SAFE_DELETE(m_lpCMsIocpManager);

    this->LogClientNet(_T("释放成功!"));
    _LEAVE_FUN_DISPOSE_END_C;
    this->LogClientNet(_T("释放失败!"));
}

// 记录日志
VOID CMsClientNetManager::LogClientNet(TCHAR* szLog)
{
    if (szLog)
    {
        m_MsClientNetLog << L_LOCK << L_TIME;
        m_MsClientNetLog.Logf(_T("[客户端网络管理器]\t%s"), szLog);
        m_MsClientNetLog << L_NEWLINE << L_UNLOCK;
    }
}

// 判断是否有效
BOOL CMsClientNetManager::IsValid(VOID)
{
    return (m_bConnected && m_pMsServerSocket->IsValid());
}

BOOL CMsClientNetManager::Connect(VOID)
{
    this->LogClientNet(_T("连接服务器!"));
    SOCKET sock = m_pMsServerSocket->GetSocket();

    // 请求连接服务器
    INT nRet = ::WSAConnect(
        sock,
        (SOCKADDR*)&m_ServerAddr,
        sizeof(m_ServerAddr),
        NULL,
        NULL,
        NULL,
        NULL
        );

    if (SOCKET_ERROR != nRet)
    {
        this->LogClientNet(_T("绑定套接字到完成端口!"));
        BOOL bRet = m_lpCMsIocpManager->AssociateDevice(
            sock, 
            SERVER_SOCKET_INDEX
            );
        AssertEx(bRet, _T("套接字绑定完成端口失败"));

        // 设置套接字池子索引
        m_pMsServerSocket->SetPoolIndex(SERVER_SOCKET_INDEX);

        m_bConnected = TRUE;
        this->LogClientNet(_T("通知连接成功!"));
        return ::PostThreadMessage(m_dwLogicThreadId, MSM_CLIENT_SOCKET_ON_ENTER, (WPARAM)m_pMsServerSocket, 0);
    }
    else
    {
        m_MsClientNetLog.ShowLastError();
        m_MsClientNetLog.LogfA(
            "连接服务器失败\nIP:%s\n端口:%d\n",
            m_szIp,
            m_wProt            
            );
    }
    this->LogClientNet(_T("连接服务器失败!"));
    return FALSE;
}

// 断开服务器
BOOL CMsClientNetManager::Disconnect(VOID)
{
    if (!m_bStop)
    {
        this->LogClientNet(_T("通知连接断开!"));
        ::PostThreadMessage(m_dwLogicThreadId, MSM_CLIENT_SOCKET_ON_EXIT, (WPARAM)m_pMsServerSocket, 0);

        this->LogClientNet(_T("开始停止网络!"));
        m_lpCMsIocpManager->Stop(TRUE);

        this->Stop(TRUE);

        return m_pMsServerSocket->Release();
    }
    return TRUE;
}

// 获取服务器套接字
lpCMsServerSocket CMsClientNetManager::GetServerSocket(VOID)
{
    return m_pMsServerSocket;
}

// 消息处理
BOOL CMsClientNetManager::OnThreadMsgExecute(LPMSG lpMsg)
{
    // 无事通知
    if (!lpMsg)
    {
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
            S_IOCP_UPDATE* lpIocpUpdate = (S_IOCP_UPDATE*)lpMsg->wParam;
            m_pMsServerSocket->OnUpdate(lpIocpUpdate);
        }break;

        // 退出或错误
    case MSM_IOCP_EXIT_OR_ERROR:
        {
            this->LogClientNet(_T("连接错误!"));
            this->Disconnect();
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
            this->LogClientNet(_T("未知消息!"));
            AssertEx(FALSE, _T("未知IOCP消息"));
        }break;
    }
    return TRUE;
}

// 线程事件通知
VOID CMsClientNetManager::OnEventNotify(E_THREAD_EVENT_NOTIFY eThreadEvent)
{
    BOOL bRet = TRUE;
    switch (eThreadEvent)
    {
    case E_ON_THREAD_RUN:
        {
            this->LogClientNet(_T("工作线程开始工作!"));
            m_lpCMsIocpManager->Run();

            // 直到连接成功或停止运行
            while (!m_bStop && !m_bConnected)
            {
                bRet = this->Connect();
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
            this->LogClientNet(_T("未知通知状态!"));
            AssertEx(FALSE, _T("严重错误逻辑"));
        }
    }
}