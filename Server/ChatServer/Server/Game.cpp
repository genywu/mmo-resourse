#include "StdAfx.h"

#include "Game.h"
#include "../Net/Msg.h"
#include "../../../Public/ServerPublic/DB/MyAdoBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AddLoginClientList(const char *str)
{
    AppFrame::AddToList("clients", str);
}

void DelLoginClientList(const char *str)
{
    AppFrame::DelFromList("clients", str);
}

Game::Game()
:m_pDBAllocator(NULL),
m_AsynchronousExecute(true),
m_NetServer4GS(NULL),
m_NetServer4Client(NULL),
m_bExit(false)
{
}

Game::~Game()
{
}

bool Game::LoadSetup()
{
    string strTemp;
    ifstream fileStream;
    fileStream.open("setup.ini");
    if (!fileStream.is_open())
        return false;

    fileStream
        >> strTemp >> GetSetup()->lServerID
        >> strTemp >> GetSetup()->dwListenPort_GS
        >> strTemp >> GetSetup()->dwListenPort_Client

        >> strTemp >> GetSetup()->strSqlConType
        >> strTemp >> GetSetup()->strSqlServerIP
        >> strTemp >> GetSetup()->strSqlUserName
        >> strTemp >> GetSetup()->strSqlPassWord
        >> strTemp >> GetSetup()->strDBName

        >> strTemp >> GetSetup()->strSecuritySqlConType  
        >> strTemp >> GetSetup()->strSecuritySqlServerIP
        >> strTemp >> GetSetup()->strSecuritySqlUserName
        >> strTemp >> GetSetup()->strSecuritySqlPassWord
        >> strTemp >> GetSetup()->strSecurityDBName

        >> strTemp >> GetSetup()->strFcmSqlConType  
        >> strTemp >> GetSetup()->strFcmSqlServerIP
        >> strTemp >> GetSetup()->strFcmSqlUserName
        >> strTemp >> GetSetup()->strFcmSqlPassWord
        >> strTemp >> GetSetup()->strFcmDBName

        >> strTemp >> GetSetup()->dwDataBlockNum
        >> strTemp >> GetSetup()->dwDataBlockSize
        >> strTemp >> GetSetup()->dwFreeMsgNum

        >> strTemp >> GetSetup()->dwFreeSockOperNum_GS
        >> strTemp >> GetSetup()->dwFreeIOOperNum_GS
        >> strTemp >> GetSetup()->lIOOperDataBufNum_GS
        >> strTemp >> GetSetup()->bCheckNet_GS
        >> strTemp >> GetSetup()->dwBanIPTime_GS
        >> strTemp >> GetSetup()->dwMaxMsgLen_GS
        >> strTemp >> GetSetup()->lMaxConnectNum_GS
        >> strTemp >> GetSetup()->lMaxClientsNum_GS
        >> strTemp >> GetSetup()->lPendingWrBufNum_GS
        >> strTemp >> GetSetup()->lPendingRdBufNum_GS
        >> strTemp >> GetSetup()->lMaxSendSizePerSecond_GS
        >> strTemp >> GetSetup()->lMaxRecvSizePerSecond_GS
        >> strTemp >> GetSetup()->lMaxBlockedSendMsgNum_GS
        >> strTemp >> GetSetup()->lConPendingWrBufNum_GS
        >> strTemp >> GetSetup()->lConPendingRdBufNum_GS
        >> strTemp >> GetSetup()->lConMaxSendSizePerSecond_GS
        >> strTemp >> GetSetup()->lConMaxRecvSizePerSecond_GS
        >> strTemp >> GetSetup()->lConMaxBlockedSendMsgNum_GS
        >> strTemp >> GetSetup()->lMaxBlockConNum_GS
        >> strTemp >> GetSetup()->lValidDelayRecDataTime_GS

        >> strTemp >> GetSetup()->dwFreeSockOperNum_Client
        >> strTemp >> GetSetup()->dwFreeIOOperNum_Client
        >> strTemp >> GetSetup()->lIOOperDataBufNum_Client
        >> strTemp >> GetSetup()->bCheckNet_Client
        >> strTemp >> GetSetup()->dwBanIPTime_Client
        >> strTemp >> GetSetup()->dwMaxMsgLen_Client
        >> strTemp >> GetSetup()->lMaxConnectNum_Client
        >> strTemp >> GetSetup()->lMaxClientsNum_Client
        >> strTemp >> GetSetup()->lPendingWrBufNum_Client
        >> strTemp >> GetSetup()->lPendingRdBufNum_Client
        >> strTemp >> GetSetup()->lMaxSendSizePerSecond_Client
        >> strTemp >> GetSetup()->lMaxRecvSizePerSecond_Client
        >> strTemp >> GetSetup()->lMaxBlockedSendMsgNum_Client
        >> strTemp >> GetSetup()->lConPendingWrBufNum_Client
        >> strTemp >> GetSetup()->lConPendingRdBufNum_Client
        >> strTemp >> GetSetup()->lConMaxSendSizePerSecond_Client
        >> strTemp >> GetSetup()->lConMaxRecvSizePerSecond_Client
        >> strTemp >> GetSetup()->lConMaxBlockedSendMsgNum_Client
        >> strTemp >> GetSetup()->lMaxBlockConNum_Client
        >> strTemp >> GetSetup()->lValidDelayRecDataTime_Client

        >> strTemp >> GetSetup()->dwRefeashInfoTime
        >> strTemp >> GetSetup()->dwSaveInfoTime
        >> strTemp >> GetSetup()->dwDoQueueInter
        >> strTemp >> GetSetup()->dwSendMsgToQueInter

        >> strTemp >> GetSetup()->dwWorldMaxPlayers
        >> strTemp >> GetSetup()->fWorldBusyScale  // 繁忙的人数比例
        >> strTemp >> GetSetup()->fWorldFullScale  // 爆满的人数比例

        >> strTemp >> GetSetup()->dwPingWorldServerTime
        >> strTemp >> GetSetup()->dwPingWorldServerErrorTime

        >> strTemp >> GetSetup()->bCheckForbidIP
        >> strTemp >> GetSetup()->bCheckAllowIP
        >> strTemp >> GetSetup()->bCheckBetweenIP

        >> strTemp >> GetSetup()->dwMinDbOptThreadNum   //! 最小DB操作线程保持数
        >> strTemp >> GetSetup()->dwMaxDbOptThreadNum   //! 最大DB操作线程数
        >> strTemp >> GetSetup()->dwThreadNumAdjustSpace//! 自动调整线程数的间隔时间
        ;
    Log4c::Trace(ROOT_MODULE,"load setup.ini...ok!");

    fileStream.close();
    fileStream.clear();
    fileStream.open("ValidGameServerIp.ini");
    if (!fileStream.is_open())
        return false;
    string line, word;
    while (getline(fileStream, line))
    {
        istringstream ss(line);
        ss >> word;
        if (word == "#")
        {
            ss >> word;//ip
            GetSetup()->validGsIp.insert(word);
        }
    }
    Log4c::Trace(ROOT_MODULE,"Load ValidGameServerIp.ini...ok!");
    return true;
}

bool Game::ReLoadSetup()
{
    LoadSetup();
    m_NetServer4GS->SetParamEx(GetSetup()->lMaxBlockConNum_GS,GetSetup()->lValidDelayRecDataTime_GS);
    m_NetServer4Client->SetParamEx(GetSetup()->lMaxBlockConNum_Client,GetSetup()->lValidDelayRecDataTime_Client);
    return true;
}

// 游戏初始化
bool Game::Init()
{
    srand((unsigned)time(NULL));
    random(100);
    CreateDirectory( "log", NULL);

    Log4c::Trace(ROOT_MODULE, "ChatServer start!");

    if (LoadSetup()==false)
    {
        AppFrame::NotifyMessageBox("Can't find file setup.ini");
        return false;
    }

    CMyAdoBase::Initialize(
        GetSetup()->strSqlConType,
        GetSetup()->strSqlServerIP,
        GetSetup()->strDBName,
        GetSetup()->strSqlUserName,
        GetSetup()->strSqlPassWord
        );

    GetConnectionString() = 
        "Provider=" + GetSetup()->strSqlConType +
        "; Server=" + GetSetup()->strSqlServerIP + 
        "; Database=" + GetSetup()->strDBName + 
        "; UID=" + GetSetup()->strSqlUserName + 
        "; PWD=" + GetSetup()->strSqlPassWord + 
        "; OLE DB Services=-1; Driver={SQL Server}";

    GetSecurityConnectionString() = 
        "Provider=" + GetSetup()->strSecuritySqlConType +
        "; Server=" + GetSetup()->strSecuritySqlServerIP + 
        "; Database=" + GetSetup()->strSecurityDBName + 
        "; UID=" + GetSetup()->strSecuritySqlUserName + 
        "; PWD=" + GetSetup()->strSecuritySqlPassWord + 
        "; OLE DB Services=-1; Driver={SQL Server}";

    GetFcmConnectionString() =
        "Provider=" + GetSetup()->strFcmSqlConType +
        "; Server=" + GetSetup()->strFcmSqlServerIP + 
        "; Database=" + GetSetup()->strFcmDBName + 
        "; UID=" + GetSetup()->strFcmSqlUserName + 
        "; PWD=" + GetSetup()->strFcmSqlPassWord + 
        "; OLE DB Services=-1; Driver={SQL Server}";

    char title[256];
    sprintf(
        title,
        "ChatServer, GS port[%d], client port[%d]",
        GetSetup()->dwListenPort_GS,
        GetSetup()->dwListenPort_Client);
    if (!AppFrame::SetTitle(title))
        return false;

    //! 初始化异步执行对象
    GetAsynchronousExecute().SetWorkParam(
        GetSetup()->dwMinDbOptThreadNum, 
        GetSetup()->dwMaxDbOptThreadNum, 
        GetSetup()->dwThreadNumAdjustSpace
        );
    GetAsynchronousExecute().BeginWork(GetSetup()->dwMinDbOptThreadNum);

    //分配数据块
    m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize);
    Msg::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum);
    CMySocket::MySocketInit();

    if (!InitNetServer4GS())
    {
        AppFrame::NotifyMessageBox("Can't init NetServer for GS!");
        return false;
    }
    if (!InitNetServer4Client())
    {
        AppFrame::NotifyMessageBox("Can't init NetServer for Client!");
        return false;
    }

    AddLoginClientList("登陆客户端列表：");
    return true;
}

// 游戏释放
bool Game::Release()
{
    Log4c::Trace(ROOT_MODULE, "ChatServer Exiting...");
    if (!m_bExit)
    {
        m_bExit = true;

        //! 结束异步执行对象
        GetAsynchronousExecute().Exit();

        if (GetNetServer4GS())
        {
            m_NetServer4GS->Exit();
            SAFE_DELETE(m_NetServer4GS);
        }

        if (GetNetServer4Client())
        {
            m_NetServer4Client->Exit();
            SAFE_DELETE(m_NetServer4Client);
        }

        CMySocket::MySocketClearUp();  
        CMyAdoBase::Uninitalize();
        Msg::Release();
        SAFE_DELETE(m_pDBAllocator);
    }
    Log4c::Trace(ROOT_MODULE, "ChatServer Exited!");
    return true;
}

bool Game::InitNetServer4Client()
{
    m_NetServer4Client = new CServer;
    bool success = m_NetServer4Client->Start(
        2,
        m_pDBAllocator,
        m_Setup.dwFreeSockOperNum_Client,
        m_Setup.dwFreeIOOperNum_Client,
        m_Setup.lIOOperDataBufNum_Client,
        m_Setup.bCheckNet_Client,
        m_Setup.dwBanIPTime_Client,
        m_Setup.dwMaxMsgLen_Client,
        m_Setup.lMaxConnectNum_Client,
        m_Setup.lMaxClientsNum_Client,
        m_Setup.lPendingWrBufNum_Client,
        m_Setup.lPendingRdBufNum_Client,
        m_Setup.lMaxSendSizePerSecond_Client,
        m_Setup.lMaxRecvSizePerSecond_Client,
        m_Setup.lMaxBlockedSendMsgNum_Client);
    if (!success)
    {
        AppFrame::NotifyMessageBox("Start Client Net server Error");
        return false;
    }

    success = m_NetServer4Client->Host(
        m_Setup.dwListenPort_Client,
        NULL,
        NF_Server_GameClient,
        SOCK_STREAM);
    if (!success)
    {
        AppFrame::NotifyMessageBox("Start Host for client Error");
        return false;
    }

    Msg::SetNetServer4Client(m_NetServer4Client);
    Log4c::Trace(ROOT_MODULE,"ChatServer 作为 Client 的服务器初始化成功 !");
    return true;
}

bool Game::InitNetServer4GS()
{
    m_NetServer4GS = new CServer;
    bool success = m_NetServer4GS->Start(
        2,
        m_pDBAllocator,
        m_Setup.dwFreeSockOperNum_GS,
        m_Setup.dwFreeIOOperNum_GS,
        m_Setup.lIOOperDataBufNum_GS,
        m_Setup.bCheckNet_GS,
        m_Setup.dwBanIPTime_GS,
        m_Setup.dwMaxMsgLen_GS,
        m_Setup.lMaxConnectNum_GS,
        m_Setup.lMaxClientsNum_GS,
        m_Setup.lPendingWrBufNum_GS,
        m_Setup.lPendingRdBufNum_GS,
        m_Setup.lMaxSendSizePerSecond_GS,
        m_Setup.lMaxRecvSizePerSecond_GS,
        m_Setup.lMaxBlockedSendMsgNum_GS);
    if (!success)
    {
        AppFrame::NotifyMessageBox("Start net server for game server error!");
        return false;
    }

    success = m_NetServer4GS->Host(
        m_Setup.dwListenPort_GS,
        NULL,
        NF_Server_GameServer,
        SOCK_STREAM);
    if (!success)
    {
        AppFrame::NotifyMessageBox("Start host for game server error!");
        return false;
    }

    Msg::SetNetServer4GS(m_NetServer4GS);
    Log4c::Trace(ROOT_MODULE,"ChatServer 作为 GameServer 的服务器初始化成功 !");
    return true;
}

// 消息处理
bool Game::ProcessMessage()
{
    if(GetNetServer4GS())
    {
        long lMessageCount = GetNetServer4GS()->GetRecvMessages()->GetSize();

        for(int i=0; i<lMessageCount; i++)
        {
            Msg* pMsg = (Msg*)GetNetServer4GS()->GetRecvMessages()->PopMessage();
            if( pMsg )
            {
                pMsg->Run();
                CBaseMessage::FreeMsg(pMsg);
            }
        }
    }

    if(GetNetServer4Client())
    {
        long lMessageCount = GetNetServer4Client()->GetRecvMessages()->GetSize();

        for(int i=0; i<lMessageCount; i++)
        {
            Msg* pMsg = (Msg*)GetNetServer4Client()->GetRecvMessages()->PopMessage();
            if( pMsg )
            {
                pMsg->Run();
                CBaseMessage::FreeMsg(pMsg);
            }
        }
    }

    return true;
}

bool Game::ProcMenus()
{
    return true;
}

void Game::DisconnectGS(long socket)
{
    m_NetServer4GS->DisConn(socket);
}
void Game::DisconnectClient(long socket)
{
    m_NetServer4Client->DisConn(socket);
}

// 游戏主循环
bool Game::MainLoop()
{
    if (m_bExit)        // 结束线程
        return false;
    ProcessMessage();   // 消息处理
    ProcMenus();        // 菜单处理
    Sleep(1);
    return true;
}

// 游戏主线程
unsigned __stdcall GameThreadFunc(void* pArguments)
{
#ifndef _DEBUG
    __try
    {
#endif
        bool ret = GetInst(Game).Init();
        AppFrame::ToggleMenus();

        while (ret)
            if (AppFrame::IsExit() || !GetInst(Game).MainLoop())
                break;

        GetInst(Game).Release();
        AppFrame::ExitThread();
        return 0;

#ifndef _DEBUG
    }
    __except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
    {
        GetInst(Game).Release();
        AppFrame::ExitThread();
        return 0;
    }
#endif
}