/**
 * @file    NetHandler.h
 * @author  Fox (yulefox@gmail.com)
 * @date    Mar.02, 2010
 * @brief   将原Game管理的网络模块独立出来.
 */

#pragma once

#include "../../../Public/Common/NetDef.h"
#include "../../../Public/DefType.h"

class CMessage;

/// WorldServer连接的服务器信息
struct ServerConnection
{
    Connection con;                ///< 连接的基本信息
    HANDLE handle;                 ///< 连接句柄
    bool ready;

    ServerConnection()
        : handle(NULL)
        , ready(false)
    {}
};

/// 连接WorldServer的GameServer信息
struct GSConnection
{
    Connection con;                ///< 连接的基本信息
    uint index;                    ///< GameServer编号
    uint receivedPlayerData;       ///< 接收到的玩家数据数量(临时使用)
    uint playerNum;                ///< 玩家数量

    GSConnection()
        : receivedPlayerData(0)
        , playerNum(0)
    {}
};

typedef std::map<NetFlag, ServerConnection> Servers;
typedef std::map<uint, GSConnection> Clients;
typedef Servers::iterator ServerItr;
typedef Clients::iterator ClientItr;

/// 待连接服务器配置 [Setup.ini], [GSSetup.ini] 及 [LogSetup.ini]
struct ServerSetup
{
    uint serverNum;                 ///< 服务器编号
    string serverName;              ///< 服务器名字
    Servers servers;                ///< 服务器连接列表
    ServerConnection ws;            ///< WorldServer自身
    Clients clients;                ///< GS客户端连接列表
    uint useLogServer;              ///< 是否使用LogServer；0=不使用，1=使用
    string errFile;                 ///< 错误文件路径+文件名
    string logFile;                 ///< 本地日志文件路径+文件名（SQL格式）
    string txtLogFile;              ///< 本地日志文件路径+文件名（明文格式）
    uint clientNo;                  ///< 日志客户端编号
};

/// 网络连接相关配置: Client网络服务配置参数 [SetupEx.ini]
struct NetHandlerSetup
{
    uint dataBlockNum;             ///< 数据块数量
    uint dataBlockSize;            ///< 数据块大小
    uint freeMsgNum;               ///< 预分配的消息数量
    uint freeSockOperNum;          ///< 网络命令操作预分配数量
    uint freeIOOperNum;            ///< 完成端口上IO操作预分配数量
    uint IOOperDataBufNum;         ///< 默认IO操作的DataBuf数量
    uint checkNet;                 ///< 是否对网络进行检测
    uint banIPTime;                ///< 禁止IP的时间
    uint maxMsgLen;                ///< 允许传输的最大消息长度		
    uint maxConnectNum;            ///< 客户端的最大连接数量
    uint maxClientsNum;            ///< 最大的客户端发送缓冲区大小
    uint pendingWrBufNum;          ///< 最大的发送IO操作Buf总大小
    uint pendingRdBufNum;          ///< 最大的接受IO操作Buf总大小
    uint maxSendSizePerSecond;     ///< 向客户端每秒发送的最大字节数
    uint maxRecvSizePerSecond;     ///< 从客户端接受的每秒最大字节数
    uint maxBlockedSendMsgNum;     ///< 最大阻塞的发送消息数量
    uint conPendingWrBufNum;       ///< 客户端最大的发送IO操作Buf总大小
    uint conPendingRdBufNum;       ///< 客户端最大的接受IO操作Buf总大小
    uint conMaxSendSizePerSecond;  ///< 向服务器发送的每秒最大字节数
    uint conMaxRecvSizePerSecond;  ///< 从服务器接受的每秒最大字节数
    uint conMaxBlockedSendMsgNum;  ///< 最大阻塞的发送消息数量
    uint refreshInfoTime;          ///< 刷新显示时间
    uint saveInfoTime;             ///< 保存一次信息的时间间隔
    uint releaseLoginPlayerTime;   ///< 释放LoginList里的Player的超时时间
    uint clearPlayerDataTime;
    uint gappID;                   ///< GAPPID
};

class NetHandler
{
public:
    NetHandler() {}
    ~NetHandler() {}

public:
    /// 初始化网络处理模块
    bool Init();

    /// 卸载网络处理模块
    bool Fini();

    /// 加载服务器配置文件
    bool LoadSetup();

    bool LoadMapFileSetup();

    /// 作为客户端连接到指定标识的服务器
    /// @param flag 服务器标识
    /// @param first 为真, 需要创建连接线程; 为假, 不创建连接线程.
    bool ConnectServer(NetFlag flag, bool first = true);

    /// 消息处理
    void ProcessMessage();

    void ServerReady(NetFlag flag, bool ready = true);
    void GSReady(uint id, bool ready = true);
    /// 判断服务器是否连接正常
    /// @param flag 服务器标识
    bool IsServerReady(NetFlag flag);

	CServer* GetServer() { return m_server; }

    /// 获取配置的 GS 数量
    size_t GetGSNum();

    /// 向其他Server返回当前所有GS在线玩家数
    /// @param flag 服务器标识, 默认为NF_LOGIN_SERVER
    void PlayerNumToServer(NetFlag flag = NF_LOGIN_SERVER);

    /// 获取网络连接配置
    const NetHandlerSetup& GetSetup() { return m_netHandlerSetup; }

    /// 获取服务器配置
    const ServerSetup& GetServerSetup() { return m_serverSetup; }

    /// 根据IP, port获取对应GS信息
    GSConnection* GetGS(const string& ip, uint port);

    /// 根据ID获取对应GS信息
    GSConnection* GetGS(uint id);

    /// 创建服务器重连线程
    /// @param flag 服务器标识
    void CreateConnectThread(NetFlag flag);

    void PutoutInfo();

    ///  设置当前服务器初始化标志
    void SetServerInitSendFlag(NetFlag flag);

    ///  处理Ws给Ls的Db准备完成消息
    void ProcessServerInitSendMsg();

private:
    friend class CMessage;
    friend unsigned __stdcall ConnectServerFunc(void* arg);

    /// 初始化网络环境：DBAllocator, Msg, Socket
    void InitNetEnv();

    /// 初始化游戏网络服务器端，建立网络服务器端，装载数据
    bool InitNetServer();

    /// 获取服务器名称
    const char* GetServerName(NetFlag flag);

    /// 查找服务器连接实例
    ServerConnection* FindServer(NetFlag flag);

    /// 服务器[LoginServer], [DBServer], [LogServer]断开后处理
    /// @param flag 网络标识
    /// @param reconnect 是否重连, 默认为true.
    void OnServerLost(NetFlag flag, bool reConnect = true);

    /// 客户端[GameServer]断开后处理
    void OnClientLost(uint id);

    CDataBlockAllocator* m_DBAllocator;

    /// 作为GameServer的服务器端
	CServer* m_server;
    NetHandlerSetup m_netHandlerSetup;
    ServerSetup m_serverSetup;
    NetFlag         m_ServerInitSendFlag;
};

/// WorldServer连接到其他Server的线程, 当WorldServer断开时启用, 连接后关闭
/// @param server Server标识
unsigned __stdcall ConnectServerFunc(void* arg);

/// 获取 NetHandler 单件实例
#define GetNetHandler() GetInst(NetHandler)
