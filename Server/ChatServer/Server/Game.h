#pragma once

#include "../../../Public/ServerPublic/Server/Common/AsynchronousExecute.h"

class CServer;
class Msg;
class CMyAdoBase;
class CMySocket;
class CDataBlockAllocator;

void AddLoginClientList(const char *str);
void DelLoginClientList(const char *str);

enum eNetFlag
{
    NF_Server_GameServer=0x1000000,
    NF_Server_GameClient=0x100,
};
// 游戏全局控制
class Game
{
public:
    Game();
    ~Game();

public:
    bool Init();
    bool Release();
    bool MainLoop();

    bool ProcessMessage();

    bool InitNetServer4GS();
    bool InitNetServer4Client();

    void DisconnectGS(long socket);
    void DisconnectClient(long socket);

private:
    //数据块分配器
    CDataBlockAllocator *m_pDBAllocator;

    CServer* m_NetServer4GS;        //负责接受GS消息的网络服务
    CServer* m_NetServer4Client;    //负责接受客户端消息的网络服务

    bool ProcMenus();

public:
    CServer* GetNetServer4GS() const {return m_NetServer4GS;}
    CServer* GetNetServer4Client() const {return m_NetServer4Client;}

    /////////////////////////////////////////////////////////////////////////
    // 线程管理
    /////////////////////////////////////////////////////////////////////////
public:
    HANDLE m_hEndEvent[2];      // 结束事件
    bool m_bExit;               // 退出游戏

    AsynchronousExecute&    GetAsynchronousExecute(VOID)        {return m_AsynchronousExecute;}
    string&                 GetConnectionString(VOID)           {return m_strConnectionString;}
    string&                 GetFcmConnectionString(VOID)        {return m_strFcmConnectionString;}
    string&                 GetSecurityConnectionString(VOID)   {return m_strSecurityConnectionString;}

private:
    //!                     异步执行对象
    AsynchronousExecute     m_AsynchronousExecute;
    //!
    string                  m_strConnectionString;
    string                  m_strFcmConnectionString;
    string                  m_strSecurityConnectionString;


    /////////////////////////////////////////////////////////////////////////
    // 游戏配置
    /////////////////////////////////////////////////////////////////////////
public:
    struct tagSetup
    {
        long lServerID;             //! AS自己的ID
        ulong dwListenPort_Client;  // 诊听Client的Port
        ulong dwListenPort_GS;    // 诊听GameServer的Port

        string strSqlConType;       // sql server connection type
        string strSqlServerIP;      // sql server ip address
        string strSqlUserName;
        string strSqlPassWord;
        string strDBName;

        string strSecuritySqlConType;   
        string strSecuritySqlServerIP;
        string strSecuritySqlUserName;
        string strSecuritySqlPassWord;
        string strSecurityDBName;

        string strFcmSqlConType;   
        string strFcmSqlServerIP;
        string strFcmSqlUserName;
        string strFcmSqlPassWord;
        string strFcmDBName;

        // Client网络服务配置参数
        ulong dwDataBlockNum;   // 数据块数量
        ulong dwDataBlockSize;  // 数据块大小
        ulong dwFreeMsgNum;     // 预分配的消息数量

        ulong dwFreeSockOperNum_GS;         // 网络命令操作预分配数量
        ulong dwFreeIOOperNum_GS;           // 完成端口上IO操作预分配数量
        long lIOOperDataBufNum_GS;          // 默认IO操作的DataBuf数量
        bool bCheckNet_GS;                  // 是否对网络进行检测
        ulong dwBanIPTime_GS;               // 禁止IP的时间
        ulong dwMaxMsgLen_GS;               // 允许传输的最大消息长度  
        long lMaxConnectNum_GS;             // 客户端的最大连接数量
        long lMaxClientsNum_GS;             // 最大的客户端发送缓冲区大小
        long lPendingWrBufNum_GS;           // 最大的发送IO操作Buf总大小
        long lPendingRdBufNum_GS;           // 最大的接受IO操作Buf总大小
        long lMaxSendSizePerSecond_GS;      // 向客户端每秒发送的最大字节数
        long lMaxRecvSizePerSecond_GS;      // 从客户端接受的每秒最大字节数
        long lMaxBlockedSendMsgNum_GS;      // 最大阻塞的发送消息数量
        long lConPendingWrBufNum_GS;        // 客户端最大的发送IO操作Buf总大小
        long lConPendingRdBufNum_GS;        // 客户端最大的接受IO操作Buf总大小
        long lConMaxSendSizePerSecond_GS;   // 向服务器发送的每秒最大字节数
        long lConMaxRecvSizePerSecond_GS;   // 从服务器接受的每秒最大字节数
        long lConMaxBlockedSendMsgNum_GS;   // 最大阻塞的发送消息数量
        long lMaxBlockConNum_GS;
        long lValidDelayRecDataTime_GS;

        ulong dwFreeSockOperNum_Client;         // 网络命令操作预分配数量
        ulong dwFreeIOOperNum_Client;           // 完成端口上IO操作预分配数量
        long lIOOperDataBufNum_Client;          // 默认IO操作的DataBuf数量
        bool bCheckNet_Client;                  // 是否对网络进行检测
        ulong dwBanIPTime_Client;               // 禁止IP的时间
        ulong dwMaxMsgLen_Client;               // 允许传输的最大消息长度  
        long lMaxConnectNum_Client;             // 客户端的最大连接数量
        long lMaxClientsNum_Client;             // 最大的客户端发送缓冲区大小
        long lPendingWrBufNum_Client;           // 最大的发送IO操作Buf总大小
        long lPendingRdBufNum_Client;           // 最大的接受IO操作Buf总大小
        long lMaxSendSizePerSecond_Client;      // 向客户端每秒发送的最大字节数
        long lMaxRecvSizePerSecond_Client;      // 从客户端接受的每秒最大字节数
        long lMaxBlockedSendMsgNum_Client;      // 最大阻塞的发送消息数量
        long lConPendingWrBufNum_Client;        // 客户端最大的发送IO操作Buf总大小
        long lConPendingRdBufNum_Client;        // 客户端最大的接受IO操作Buf总大小
        long lConMaxSendSizePerSecond_Client;   // 向服务器发送的每秒最大字节数
        long lConMaxRecvSizePerSecond_Client;   // 从服务器接受的每秒最大字节数
        long lConMaxBlockedSendMsgNum_Client;   // 最大阻塞的发送消息数量
        long lMaxBlockConNum_Client;
        long lValidDelayRecDataTime_Client;

        ulong dwRefeashInfoTime;    // 刷新显示时间
        ulong dwSaveInfoTime;       // 保存一次信息的时间间隔
        ulong dwDoQueueInter;       // 处理等待队列的间隔时间
        ulong dwSendMsgToQueInter;  // 给等待队列发送消息的间隔时间

        ulong dwWorldMaxPlayers;    // 登陆WorldServer的最大人数
        float fWorldBusyScale;      // 繁忙的人数比例
        float fWorldFullScale;      // 爆满的人数比例

        ulong dwPingWorldServerTime;        // 定时PingWorldServer的间隔
        ulong dwPingWorldServerErrorTime;   // PingWorldServer到收到返回的出错时间间隔

        bool bCheckForbidIP;    // 是否开启检测IP禁用
        bool bCheckAllowIP;     // 是否开启IP允许检测
        bool bCheckBetweenIP;   // 检测ip是否在允许IP内

        ulong dwMinDbOptThreadNum;      //! 最小DB操作线程保持数
        ulong dwMaxDbOptThreadNum;      //! 最大DB操作线程数
        ulong dwThreadNumAdjustSpace;   //! 自动调整线程数的间隔时间

        set<string> validGsIp;  //有效的game server ip（不在此列表中的gs无法连接cs）
    };

private:
    tagSetup m_Setup;
public:
    bool        LoadSetup();
    bool        ReLoadSetup();
    tagSetup*   GetSetup() {return &m_Setup;}
};

unsigned __stdcall GameThreadFunc(void* pArguments); // 游戏主线程