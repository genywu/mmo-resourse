#include "StdAfx.h"

#include "NetHandler.h"
#include "Message.h"
#include "../App/SMClient.h"

#include "../../../Public/ServerPublic/Server/Common/AppTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool NetHandler::Init()
{
    m_ServerInitSendFlag = NF_NULL;

    // 加载 [Setup.ini], [LogSetup.ini], [SetupEx.ini]及 [ServerSetup.ini] 配置
    if (!LoadSetup())
    {
        return false;
    }

    InitNetEnv();

    // 创建服务器端
    if (!InitNetServer())
    {
        return false;
    }

    // 连接到LogServer
    if (0 == m_serverSetup.useLogServer)
    {
        Log4c::Warn(ROOT_MODULE,FormatText("GAME_NET_SERVERINVALID", "LogServer"));
    }
    else
    {
        ConnectServer(NF_LOG_SERVER);
    }

    ConnectServer(NF_DB_SERVER); // 连接到DBServer
    ConnectServer(NF_LOGIN_SERVER); // 连接到LoginServer

    // 装载内存影射文件配置
    LoadMapFileSetup();
    CSMClient::GetSM()->InitOk();
    return true;
}

bool NetHandler::Fini()
{
    ServerItr itr = m_serverSetup.servers.begin();
    for (; itr != m_serverSetup.servers.end(); ++itr)
    {
        ServerConnection& server = itr->second;
        server.ready = false;
        WaitForSingleObject(server.handle, INFINITE);
        CloseHandle(server.handle);
        m_server->DisConn(server.con.socket);
    }
    m_server->Exit();

    CSMClient::GetSM()->ExitOk();
    CSMClient::GetSM()->ReleaseSM();
    SAFE_DELETE(m_DBAllocator);
    SAFE_DELETE(m_server);
    CMySocket::MySocketClearUp();
    CMessage::Release();

    return true;
}

bool NetHandler::LoadSetup()
{
    const char* file = "Setup.ini";
    LoadLog("GAME_LOAD_SETUP", file, 0);
    ifstream stream;
    string omitSeparator;
    stream.open(file);
    Servers& servers = m_serverSetup.servers;
    if (stream.is_open())
    {
        stream >> omitSeparator >> m_serverSetup.serverNum
            >> omitSeparator >> m_serverSetup.serverName
            >> omitSeparator >> servers[NF_DB_SERVER].con.ip
            >> omitSeparator >> servers[NF_DB_SERVER].con.port
            >> omitSeparator >> servers[NF_LOGIN_SERVER].con.ip
            >> omitSeparator >> servers[NF_LOGIN_SERVER].con.port
            >> omitSeparator >> m_serverSetup.ws.con.ip
            >> omitSeparator >> m_serverSetup.ws.con.port;
        stream.close();
        stream.clear();
        char title[256];
        sprintf(title, "[%d]WorldServer", m_serverSetup.ws.con.port);
        if (!AppFrame::SetTitle(title))
            return false;

        LoadLog("GAME_LOAD_SETUP", file, 1);
    }
    else
    {
        LoadLog("GAME_LOAD_SETUP", file, 2);
        return false;
    }

    file = "LogSetup.ini";
    LoadLog("GAME_LOAD_SETUP", file, 0);
    stream.open(file);
    if (stream.is_open())
    {
        stream >> omitSeparator >> m_serverSetup.useLogServer
               >> omitSeparator >> servers[NF_LOG_SERVER].con.ip
               >> omitSeparator >> servers[NF_LOG_SERVER].con.port
               >> omitSeparator >> m_serverSetup.errFile
               >> omitSeparator >> m_serverSetup.logFile
               >> omitSeparator >> m_serverSetup.txtLogFile
               >> omitSeparator >> m_serverSetup.clientNo;
        stream.close();
        stream.clear();
        LoadLog("GAME_LOAD_SETUP", file, 1);
    }
    else
    {
        LoadLog("GAME_LOAD_SETUP", file, 2);
        return false;
    }

    file = "SetupEx.ini";
    LoadLog("GAME_LOAD_SETUP", file, 0);
    stream.open(file);
    if (stream.is_open())
    {
        stream >> omitSeparator >> m_netHandlerSetup.dataBlockNum
               >> omitSeparator >> m_netHandlerSetup.dataBlockSize
               >> omitSeparator >> m_netHandlerSetup.freeMsgNum
               >> omitSeparator >> m_netHandlerSetup.freeSockOperNum
               >> omitSeparator >> m_netHandlerSetup.freeIOOperNum
               >> omitSeparator >> m_netHandlerSetup.IOOperDataBufNum
               >> omitSeparator >> m_netHandlerSetup.checkNet
               >> omitSeparator >> m_netHandlerSetup.banIPTime
               >> omitSeparator >> m_netHandlerSetup.maxMsgLen
               >> omitSeparator >> m_netHandlerSetup.maxConnectNum
               >> omitSeparator >> m_netHandlerSetup.maxClientsNum
               >> omitSeparator >> m_netHandlerSetup.pendingWrBufNum
               >> omitSeparator >> m_netHandlerSetup.pendingRdBufNum
               >> omitSeparator >> m_netHandlerSetup.maxSendSizePerSecond
               >> omitSeparator >> m_netHandlerSetup.maxRecvSizePerSecond
               >> omitSeparator >> m_netHandlerSetup.maxBlockedSendMsgNum
               >> omitSeparator >> m_netHandlerSetup.conPendingWrBufNum
               >> omitSeparator >> m_netHandlerSetup.conPendingRdBufNum
               >> omitSeparator >> m_netHandlerSetup.conMaxSendSizePerSecond
               >> omitSeparator >> m_netHandlerSetup.conMaxRecvSizePerSecond
               >> omitSeparator >> m_netHandlerSetup.conMaxBlockedSendMsgNum
               >> omitSeparator >> m_netHandlerSetup.refreshInfoTime
               >> omitSeparator >> m_netHandlerSetup.saveInfoTime
               >> omitSeparator >> m_netHandlerSetup.releaseLoginPlayerTime
               >> omitSeparator >> m_netHandlerSetup.clearPlayerDataTime;
        stream.close();
        stream.clear();
        LoadLog("GAME_LOAD_SETUP", file, 1);
    }
    else
    {
        LoadLog("GAME_LOAD_SETUP", file, 2);
        return false;
    }

    file = "ServerSetup.ini";
    LoadLog("GAME_LOAD_SETUP", file, 0);
    CRFile* fp = rfOpen(file);
    if (!fp)
    {
        LoadLog("GAME_LOAD_SETUP", file, 2);
        return false;
    }

    stringstream sstream;
    fp->ReadToStream(sstream);
    rfClose(fp);

    // 读取游戏服务器信息
    GSConnection gs;
    int GSNum = 0;
    sstream >> omitSeparator >> GSNum;
    for (int i = 0; i < GSNum; ++i)
    {
        ReadTo(sstream, "#");
        sstream >> gs.index >> gs.con.ip >> gs.con.port;
        m_serverSetup.clients[gs.index] = gs;
    }
    LoadLog("GAME_LOAD_SETUP", file, 1);
    return true;
}

void  NetHandler::SetServerInitSendFlag(NetFlag Flag)
{
    m_ServerInitSendFlag = Flag;
}

void  NetHandler::ProcessServerInitSendMsg()
{
    if ( m_ServerInitSendFlag == NF_NULL )  return;

    if ( m_ServerInitSendFlag & NF_LOGIN_SERVER )
    {
        /// GS,DB都准备完成
        if ( CMessage::IsGSReady(-1) & IsServerReady(NF_DB_SERVER) )
        {
            GetGame().SendCdkeyToLoginServer(TRUE);
        }
        else 
            GetGame().SendCdkeyToLoginServer(FALSE);
    }
    if ( m_ServerInitSendFlag & NF_DB_SERVER )
    {
        //! 重新初始化组织系统
        GetOrganiCtrl()->Initialize();
    }
    m_ServerInitSendFlag = NF_NULL; 
}

const char* NetHandler::GetServerName(NetFlag flag)
{
    switch (flag)
    {
    case NF_DB_SERVER:
        return "DBServer";
    case NF_LOG_SERVER:
        return "LogServer";
    case NF_LOGIN_SERVER:
        return "LoginServer";
    default:
        return "UNKNOWN_SERVER";
    }
}

ServerConnection* NetHandler::FindServer(NetFlag server)
{
    ServerItr itr = m_serverSetup.servers.find(server);
    if (itr == m_serverSetup.servers.end())
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("GAME_NET_SERVERUNKNOWN"));
        return NULL;
    }

    ServerConnection& con = itr->second;
    return &con;
}

GSConnection* NetHandler::GetGS(uint id)
{
    ClientItr itr = m_serverSetup.clients.find(id);
    if (itr == m_serverSetup.clients.end())
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_NET_40", id));
        return NULL;
    }

    GSConnection& con = itr->second;
    return &con;
}

void NetHandler::InitNetEnv()
{
    m_DBAllocator = new CDataBlockAllocator(m_netHandlerSetup.dataBlockNum,
                                            m_netHandlerSetup.dataBlockSize);

    CMessage::Initial(m_DBAllocator, m_netHandlerSetup.freeMsgNum);
    CMySocket::MySocketInit();
}

bool NetHandler::LoadMapFileSetup()
{
    return CSMClient::GetSM()->LoadMapFileSetup(m_DBAllocator);
}

bool NetHandler::InitNetServer()
{
    m_server = new CServer;
    bool ret = m_server->Start(1 | 2,
                               m_DBAllocator,
                               m_netHandlerSetup.freeSockOperNum,
                               m_netHandlerSetup.freeIOOperNum,
                               m_netHandlerSetup.IOOperDataBufNum,
                               m_netHandlerSetup.checkNet ? true : false,
                               m_netHandlerSetup.banIPTime,
                               m_netHandlerSetup.maxMsgLen,
                               m_netHandlerSetup.maxConnectNum,
                               m_netHandlerSetup.maxClientsNum,
                               m_netHandlerSetup.pendingWrBufNum,
                               m_netHandlerSetup.pendingRdBufNum,
                               m_netHandlerSetup.maxSendSizePerSecond,
                               m_netHandlerSetup.maxRecvSizePerSecond,
                               m_netHandlerSetup.maxBlockedSendMsgNum,
                               m_netHandlerSetup.conPendingWrBufNum,
                               m_netHandlerSetup.conPendingRdBufNum,
                               m_netHandlerSetup.conMaxSendSizePerSecond,
                               m_netHandlerSetup.conMaxRecvSizePerSecond,
                               m_netHandlerSetup.conMaxBlockedSendMsgNum);
    if (!ret)
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GAME_NET_INITFAILED", GetLastError()));
        return false;
    }

    Log4c::Trace(ROOT_MODULE,GetText("GAME_NET_INITOK"));

    ret = m_server->Host(m_serverSetup.ws.con.port,
                         NULL,
                         NF_GAME_SERVER,
                         SOCK_STREAM);

    if(!ret)
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GAME_NET_LISTENFAILED"));
        return false;
    }

    Log4c::Trace(ROOT_MODULE,GetText("GAME_NET_LISTENOK"));
    CMessage::SetServer(m_server);
    return true;
}

bool NetHandler::ConnectServer(NetFlag flag, bool first)
{
    ServerConnection* server = FindServer(flag);
    if (!server)
    {
        return false;
    }

    server->con.socket = m_server->Connect(server->con.ip.c_str(),
                                           server->con.port,
                                           flag);

    if (server->con.socket != SOCKET_ERROR )
    {
        switch (flag)
        {
        case NF_DB_SERVER:
            {
                // 向DBS发送初始化数据
                CMessage msg(MSG_W2DB_SERVER_SENDSERVERINFO);
                msg.Add(m_serverSetup.ws.con.port);
                msg.Add(m_serverSetup.ws.con.ip.c_str());
                msg.SendToServer(NF_DB_SERVER);
            }
            break;

        case NF_LOG_SERVER:
            {
                CMessage msg(MSG_02LOG_SERVER_LOGIN);
                msg.Add((long)m_serverSetup.clientNo);
                msg.SendToServer(NF_LOG_SERVER);
                GetLogicLogInterface()->GetLogClient()->SetSoket(server->con.socket);
            }
            break;

        case NF_LOGIN_SERVER:
            {
                SetServerInitSendFlag( NF_LOGIN_SERVER );
            }
            break;

        default:
            {
                char str[64];
                sprintf(str, "Unknown netflag: %u", flag);
                Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("GAME_COMMON_INVALID", str));
                return false;
            }
        }
        Log4c::Trace(ROOT_MODULE,FormatText("GAME_NET_SERVEROK", GetServerName(flag)));
        return true;
    }
    if (first)
    {
        CreateConnectThread(flag);
    }
    return false;
}

void NetHandler::ProcessMessage()
{
    uint msgCount = m_server->GetRecvMessages()->GetSize();
    for (uint i = 0; i<msgCount; ++i)
    {
        CMessage* msg =
            (CMessage*)(m_server->GetRecvMessages()->PopMessage());
        if (msg)
        {
            uint msgType = msg->GetType();
            msg->Run();
            CBaseMessage::FreeMsg(msg);
        }
    }

    CSMClient::GetSM()->ProcessMessage();
}

void NetHandler::ServerReady(NetFlag flag, bool ready)
{
    ServerConnection* server = FindServer(flag);
    if (server)
        server->ready = ready;
}

bool NetHandler::IsServerReady(NetFlag flag)
{
    ServerConnection* server = FindServer(flag);
    if (!server) return false;
    return server->ready;
}

size_t NetHandler::GetGSNum()
{
    return m_serverSetup.clients.size();
}

GSConnection* NetHandler::GetGS(const string& ip, uint port)
{
    ClientItr itr = m_serverSetup.clients.begin();
    for (; itr != m_serverSetup.clients.end(); ++itr)
    {
        if (ip == itr->second.con.ip &&
            port == itr->second.con.port)
            return &(itr->second);
    }
    return NULL;
}

void NetHandler::PlayerNumToServer(NetFlag flag)
{
    CMessage msg(MSG_W2L_SERVER_ANSWER_GsPlayerNum);
    msg.Add((long)CMessage::GetGSNum());

    ClientItr itr = m_serverSetup.clients.begin();
    for (; itr != m_serverSetup.clients.end(); ++itr)
    {
        GSConnection& gs = itr->second;
        msg.Add((BYTE)CMessage::IsGSReady(gs.index));
        msg.Add((long)gs.index);
        msg.Add((long)gs.playerNum);
    }

    msg.SendToServer(flag);
}

void NetHandler::OnServerLost(NetFlag flag, bool reConnect)
{
    ServerConnection* server = FindServer(flag);
    if (!server) return;
    server->con.socket = 0;

    if (reConnect)
    {
        CreateConnectThread(flag);
    }
}

void NetHandler::OnClientLost(uint id)
{
    GSConnection* gs = GetGS(id);
    if (!gs)
    {
        Log4c::Trace(ROOT_MODULE,FormatText("GAME_LOAD_UNKOWNGSLOST", id));
        return;
    }

    gs->con.socket = 0;
    // 删除Player列表
    char str[256];
    sprintf(str, "%s [%d]", gs->con.ip.c_str(), gs->con.port);
    AppFrame::DelFromList("list", str);

   Log4c::Trace(ROOT_MODULE,FormatText("WS_NET_41",gs->con.ip.c_str(),gs->con.port));
    GetGame().OnGameServerLost(id);
}

void NetHandler::CreateConnectThread(NetFlag flag)
{
    const char* info = GetServerName(flag);
    Log4c::Warn(ROOT_MODULE,FormatText("GAME_NET_SERVERFAILED", info, info));
    ServerConnection* server = FindServer(flag);
    if (!server) return;

    server->ready = false;
    WaitForSingleObject(server->handle, INFINITE);
    CloseHandle(server->handle);
    server->ready = true;

    unsigned threadID;
    long *pArg = new long( (long)flag );
    server->handle = (HANDLE)_beginthreadex(NULL,
                                            0,
                                            ConnectServerFunc,
                                            pArg,
                                            0,
                                            &threadID);	
}

void NetHandler::PutoutInfo()
{
    //输出网络调试信息
    m_server->OutputMsgStatInfo();
    m_DBAllocator->PutAllocInfo("MemAllocInfo");
}

unsigned __stdcall ConnectServerFunc(void* arg)
{
    NetFlag flag = *(NetFlag*)arg;
    ServerConnection* server = GetNetHandler().FindServer(flag);

    SAFE_DELETE(arg);
    
    if (!server) return -1;

    while (true)
    {
        if(!server->ready)
            break;

        Sleep(8000);

        if(GetNetHandler().ConnectServer(flag, false))
            break;
    }
    SetEvent(server->handle);
    return 0;
}
