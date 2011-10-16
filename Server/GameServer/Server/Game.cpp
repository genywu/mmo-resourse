#include "StdAfx.h"
#include "Game.h"

#include "BillClient.h"
#include "GameManager.h"
#include "../App/AntiCheat/AntiCheatCommon.h"
#include "../App/AntiCheat/CPlayerManager.h"
#include "../App/BuffSkill/NewSkillFactory.h"
#include "../App/Business/CBusinessManager.h"
#include "../App/Country/CountryParam.h"
#include "../App/Country/CountryHandler.h"
#include "../App/FBusiness/ShopManager.h"
#include "../App/Goods/CGoods.h"
#include "../App/Goods/CGoodsFactory.h"
#include "../App/Linkman/LinkmanSystem.h"
#include "../App/LuaScript/ScriptSys.h"
#include "../App/Message/MsgOutTimeDef.h"
#include "../App/Message/SequenceString.h"
#include "../App/Monster.h"
#include "../App/OrganizingSystem/GameOrganizingCtrl.h"
#include "../App/Organizingsystem/gameunion.h"
#include "../App/Organizingsystem/gamefaction.h"
#include "../App/PetCtrl.h"
#include "../App/Player.h"
#include "../App/PlayerMan/PlayerMan.h"
#include "../App/PKSys.h"
#include "../App/RgnManager.h"
#include "../App/ServerRegion.h"
#include "../App/Session/CSessionFactory.h"
#include "../../../Public/Common/DupliRegionSetup.h"
#include "../../../Public/Common/NetSessionManager.h"
#include "../../../Public/Setup/GlobalSetup.h"
#include "../../../Public/Setup/GoodsSetup.h"
#include "../../../Public/Setup/SuitRuleSetup.h"
#include "../../../Public/Setup/KitCardRuleSetup.h"
#include "../../../Public/Setup/QuestSystem.h"
#include "../../../Public/Setup/MailList.h"
#include "../../../Public/Setup/ReliveGoodsList.h"
#include "../../../Public/Setup/GoodsTailorSetup.h"
#include "../../../Public/ServerPublic/Server/Common/WordsFilter.h"
#include "../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../Public/ServerPublic/Server/Common/VarListUtils.h"

extern bool IncShopProcessing(CPlayer* pPlayer, DWORD dwGoodsIndex, DWORD dwNum, DWORD dwTradeMode, long lTotalAmount);
extern int SNDACALLBACK MsgHandle(int msgType, ISDOAMsg* req, ISDOAMsg* res);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGame* g_pGame= NULL;					// 游戏客户端数据
HANDLE g_hGameThreadExitEvent=NULL;		// 主线程退出事件
bool	g_bGameThreadExit=false;		// 是否退出主线程

HANDLE	hConnectThread=NULL;			// WS连接线程句柄 
bool	bConnectThreadExit=false;		// 是否退出WS连接线程
HANDLE	hConnectThreadDBS=NULL;			// DBS连接线程句柄 
bool	bConnectThreadExitDBS=false;		// 是否退出DBS连接线程
HANDLE	hConnectThreadLogS=NULL;			// DBS连接线程句柄 
bool	bConnectThreadExitLogS=false;		// 是否退出DBS连接线程
HANDLE	hConnectThreadBouns = NULL;			// 连接BounsServer的线程句柄
bool	bConnectThreadExitBouns = false;	// 是否退出BounsS的连接线程

bool	bConnectThreadExitBS=false;
HANDLE	hConnectThreadBS=NULL;

HANDLE  hConnectThreadChatServer=NULL;      // 连接ChatServer的线程句柄
bool    bConnectThreadExitChatServer=false; // 是否退出ChatServer的连接线程

int g_ms = 5;		// 每一桢的时间（毫秒）

HANDLE g_hSavingThread = NULL; // 存盘线程句柄

unsigned __stdcall ConnectDBFunc( void* pArguments );

//extern unsigned __stdcall SaveThreadFunc(void* pArguments);

void OutMemInfo(const char *pFileName, const char *pTxt);
void RefreshInfo(); // 刷新服务器信息

enum MyMenuID
{
    // (加载)LOAD
    CONNECT_WOLDSERVER = MY_MENU * 2,       // 连接WorldServer
    CONNECT_BOUNSSERVER,                    // 连接BounsServer
    LOAD_SETUPEX,                           // 加载扩展配置文件
    LOAD_MAPFILESETUP,                      // 加载内存映射配置
    SHOW_SCRIPTINFO,                        // 显示脚本信息
    SHOW_MEMPOOLINFO,                       // 显示内存池信息
};

void RefreshInfo()
{
    static char str[102400];

    if (!GetGame() || !GetGame()->GetNetServer())
    {
        return;
    }

    static long lMaxConnects = 0,lMaxPlayers = 0, lMaxTeams = 0, lMaxScripts = 0;
    static long lSendSizePerSecondToClient = 0, lRecvSizePerSecondFromClient = 0;
    static long lSendSizePerSecondToWorld = 0, lRecvSizePerSecondFromWorld = 0;

    static long lMaxTeamRgnNum = 0;
    static long lMaxPRgnNum = 0;
    static DWORD dwMaxAvaiVirMem = 0;
    static DWORD dwHandleCount = 0;

    static long lMaxDupRgnDataNum = 0;

    long lConnects = GetGame()->GetNetServer()->GetClientCount();
    long lPlayers = GetGame()->GetPlayerCount();
    long lTeams = GetGame()->GetNumTeams();
    long lScripts = 0;
    MEMORYSTATUS msC;GlobalMemoryStatus(&msC);
    DWORD dwCurAvaiVirMem = (DWORD)(msC.dwAvailVirtual/1048576);
    DWORD dwCurNandleCount = 0;
    GetProcessHandleCount(GetCurrentProcess(),&dwCurNandleCount);

    DWORD dwAcceptThreadTick = GetGame()->GetNetServer()->GetAcceptThreadTick();
    DWORD dwNetThreadTick = GetGame()->GetNetServer()->GetNetThreadTick();
    DWORD dwWorkThreadTick = GetGame()->GetNetServer()->GetWorkThreadTick();
    DWORD dwMainThreadTick = GetGame()->GetAITick();

    long lCurTeamRgnNum = (long)GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().size();
    long lCurPRgnNum = (long)GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().size();

    long lSendSizePerSecondToClientTempt = GetGame()->GetNetServer()->GetSendSizePerSecond(NF_Server_Client);
    long lRecvSizePerSecondFromClientTempt = GetGame()->GetNetServer()->GetRecvSizePerSecond(NF_Server_Client);	
    long lSendSizePerSecondToWorldTempt = GetGame()->GetNetServer()->GetSendSizePerSecond(NF_WServer_Client);
    long lRecvSizePerSecondFromWorldTempt = GetGame()->GetNetServer()->GetRecvSizePerSecond(NF_WServer_Client);

    long lCurDupRgnDataNum = GameManager::GetInstance()->GetRgnManager()->GetDupRgnDataNum();

    if (lConnects>lMaxConnects)
        lMaxConnects = lConnects;
    if (lPlayers>lMaxPlayers)
        lMaxPlayers = lPlayers;
    if (lTeams>lMaxTeams)
        lMaxTeams = lTeams;
    if (lScripts>lMaxScripts)
        lMaxScripts = lScripts;

    if(lCurTeamRgnNum>lMaxTeamRgnNum)
        lMaxTeamRgnNum = lCurTeamRgnNum;

    if(lCurPRgnNum > lMaxPRgnNum)
        lMaxPRgnNum = lCurPRgnNum;

    if(dwCurAvaiVirMem > dwMaxAvaiVirMem)
        dwMaxAvaiVirMem = dwCurAvaiVirMem;
    if(dwCurNandleCount > dwHandleCount)
        dwHandleCount = dwCurNandleCount;

    if(lCurDupRgnDataNum > lMaxDupRgnDataNum)
        lMaxDupRgnDataNum = lCurDupRgnDataNum;


    if(lSendSizePerSecondToClientTempt > lSendSizePerSecondToClient)
        lSendSizePerSecondToClient = lSendSizePerSecondToClientTempt;
    if(lRecvSizePerSecondFromClientTempt > lRecvSizePerSecondFromClient)
        lRecvSizePerSecondFromClient = lRecvSizePerSecondFromClientTempt;
    if(lSendSizePerSecondToWorldTempt > lSendSizePerSecondToWorld)
        lSendSizePerSecondToWorld = lSendSizePerSecondToWorldTempt;
    if(lRecvSizePerSecondFromWorldTempt > lRecvSizePerSecondFromWorld)
        lRecvSizePerSecondFromWorld = lRecvSizePerSecondFromWorldTempt;

    // 反外挂信息监控 :Ques\AccWrong\AccTimeOut\AccRight
    const AntiCheat::LogInfo *ac_log = AntiCheat::GetLogInfo();

    _snprintf(str, 102400, "\
                           Connects = %d/%d  Players = %d/%d\r\n\
                           HandleCount=%d/%d	AvaiVirMem =%d/%d\r\n\
                           AccTTick=%u NetTTick=%u WorkTTick=%u MainTTick=%u\r\n\
                           Teams=%d/%d  Scripts=%d/%d  ServerID = %d\r\n\
                           NT-S2C=%d/%d(B/S)\t\tNT-C2S=%d/%d(B/S)\r\n\
                           NT-S2W=%d/%d(B/S)\t\tNT-W2S=%d/%d(B/S)\r\n\
                           队伍副本个数=%d/%d 个人副本个数=%d/%d 副本数据块个数=%d/%d\r\n\
                           AntiCheat=%u/%u/%u/%u/%u\r\n\r\n",
                           lConnects, lMaxConnects,lPlayers, lMaxPlayers,
                           dwCurNandleCount,dwHandleCount,dwCurAvaiVirMem,dwMaxAvaiVirMem,
                           dwAcceptThreadTick,dwNetThreadTick,dwWorkThreadTick,dwMainThreadTick,
                           lTeams, lMaxTeams,lScripts, lMaxScripts, GetGame()->GetIDIndex(),
                           lSendSizePerSecondToClientTempt,lSendSizePerSecondToClient,lRecvSizePerSecondFromClientTempt,lRecvSizePerSecondFromClient,
                           lSendSizePerSecondToWorldTempt,lSendSizePerSecondToWorld,lRecvSizePerSecondFromWorldTempt,lRecvSizePerSecondFromWorld, 
                           lCurTeamRgnNum, lMaxTeamRgnNum, lCurPRgnNum, lMaxPRgnNum, lCurDupRgnDataNum, lMaxDupRgnDataNum,
                           ac_log->AccQues, ac_log->AccWrong, ac_log->AccTimeOut, ac_log->AccRight, ac_log->AccSysErr );


    // 显示每个场景的人数
    static char strTemp[256];

    for(CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().begin(); it != GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end(); it++)
    {
        CServerRegion* pRegion = it->second;
        if (pRegion)
        {
            _snprintf(strTemp, 256, "%s=%d ", pRegion->GetName(), pRegion->GetPlayerAmout() );
            strcat(str, strTemp);
        }
    }

    SetWindowText(AppFrame::GetWnd("info"), str);
}

tagScriptDataInfo::tagScriptDataInfo(void* pParam)
: pInfoData(NULL),lInfoSize(0)
{
    if(!pParam) return;
    char* pData = ((tagScriptDataInfoParam*)pParam)->pInfoData;
    long lSize  = ((tagScriptDataInfoParam*)pParam)->lInfoSize;
    if(pData && lSize > 0)
    {
        lInfoSize = lSize;
        pInfoData = (char*)M_ALLOC(sizeof(char)*lInfoSize);
        memcpy(pInfoData, pData, lSize);
    }
}

tagScriptDataInfo::~tagScriptDataInfo(void)
{
    M_FREE(pInfoData, sizeof(char)*lInfoSize);
}

// 创建游戏客户端数据
BOOL CreateGame()
{
    g_pGame = new CGame;
    return true;
}

// 释放游戏客户端数据
BOOL DeleteGame()
{
    if (g_pGame)
    {
        delete g_pGame;
        g_pGame = NULL;
    }
    return true;
}

// 获取游戏客户端数据
CGame* GetGame()
{
    return g_pGame;
}

CGame::CGame(void)
: b_IsFirstConnectionWS(true),m_bIsGenVarInit(false),g_lSaveDeltaTime(0)
{

    m_ServerInitSendFlag = 0;
    InitializeCriticalSection(&m_CSServerInitSendFlag);

    s_lAITick = 0;

    m_dwWSNumber=0;

    m_bBSConnected=FALSE;

    s_pNetServer = NULL;
    m_pDupliRegionSetup = NULL;
    m_pDBAllocator = NULL;

    m_pSDOAOrderHandler=NULL;
    m_bSDOAInitialized=false;

    m_Setup.dwListenPort = 0;
    m_Setup.dwWorldPort = 0;

    //m_strFunctionList_FileData = NULL;
    //m_strVariableList_FileData = NULL;

    m_pLogInterface = new LogicLogInterface();

    m_Setup.lMaxBlockConNum = 10;
    m_Setup.lValidDelayRecDataTime = 4000;
#ifdef _DEBUG
    m_lInitScript = 0;
    m_lSkillBegin = 0;
    m_lTimeOutScript = 0;
#endif

    //m_lFuncFileDataLen = 0;
    //m_lVariableFileDataLen = 0;
}

CGame::~CGame(void)
{
    m_Setup.strLocalIP.clear();
    m_Setup.strWorldIP.clear();

    DeleteCriticalSection(&m_CSServerInitSendFlag);

    CGoodsFactory::OutputUseGoodsInfo("MemAllocInfo");
    CNewSkillFactory::OutputBasemodelInfo("MemAllocInfo");

    OutPlayerAndRegionInfo("MemAllocInfo");

    OUT_RUNNING_INFO(true);
}

bool CGame::LoadSetup()
{
    // 读取全局配置
    ifstream streamLog;

    streamLog.open("LogSetup.ini");
    string strTemp;
    if( streamLog.is_open() )
    {
        streamLog 
            >> strTemp >> GetSetup()->lUseLogServer
            >> strTemp >> GetSetup()->strLogIP
            >> strTemp >> GetSetup()->dwLogPort
            >> strTemp >> GetSetup()->strErrFile
            >> strTemp >> GetSetup()->strLogFile
            >> strTemp >> GetSetup()->strTxtLogFile
            >> strTemp >> GetSetup()->dClientNo;
        streamLog.close();
    }
    else
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't find file LogSetup.ini");
        return false;
    }

    ifstream streamBS;
    streamBS.open("BillSetup.ini");

    if(streamBS.is_open())
    {
        streamBS>>strTemp>>GetSetup()->lUseBillingServer;
        m_listBSSetup.clear();
        while(ReadTo(streamBS,"#"))
        {
            tagBSSetup stBSSetup;
            streamBS>>stBSSetup.dwIndex>>stBSSetup.strIp>>stBSSetup.dwPort;
            m_listBSSetup.push_back(stBSSetup);
        }
    }
    else
    {
        GetSetup()->lUseBillingServer=BILL_CONNECT_NONE;
       Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't find file BillSetup.ini");
        return false;
    }

    ifstream streamBDS;
    streamBDS.open("BounsSetup.ini");
    if(streamBDS.is_open())
    {
        streamBDS >> strTemp >> GetSetup()->lUseBounsServer;
        streamBDS >> strTemp >> GetSetup()->dwGameServerNum;
        m_listBDSSetup.clear();
        while(ReadTo(streamBDS,"#"))
        {
            tagBSSetup tgBDSSetup;
            streamBDS >> tgBDSSetup.dwIndex >> tgBDSSetup.strIp >> tgBDSSetup.dwPort;
            m_listBDSSetup.push_back(tgBDSSetup);
        }
    }
    else
    {
        GetSetup()->lUseBounsServer = 0;
       Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't find file BounsSetup.ini");
        return false;
    }

    ifstream stream;
    stream.open("setup.ini");
    if( stream.is_open() )
    {
        char str[256];
        stream >> str >> GetSetup()->strWorldIP
            >> str >> GetSetup()->dwWorldPort
            >> str >> GetSetup()->dwListenPort
            >> str >> GetSetup()->strLocalIP;

        stream.close();

        _snprintf(str, 256, "[%d]GameServer", GetSetup()->dwListenPort);
        if (!AppFrame::SetTitle(str))
            return false;
    }
    else
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't find file Setup.ini");
        return false;
    }


    ifstream streamSetupEx;
    streamSetupEx.open("SetupEx.ini");
    if( streamSetupEx.is_open() )
    {
        streamSetupEx  >> strTemp >> GetSetup()->lMaxBlockConNum
            >> strTemp >> GetSetup()->lValidDelayRecDataTime
            >> strTemp >> GetSetup()->dwDataBlockNum
            >> strTemp >> GetSetup()->dwDataBlockSize
            >> strTemp >> GetSetup()->dwFreeMsgNum
            >> strTemp >> GetSetup()->dwFreeSockOperNum
            >> strTemp >> GetSetup()->dwFreeIOOperNum
            >> strTemp >> GetSetup()->lIOOperDataBufNum
            >> strTemp >> GetSetup()->lEncryptType
            >> strTemp >> GetSetup()->bCheckNet
            >> strTemp >> GetSetup()->dwBanIPTime
            >> strTemp >> GetSetup()->dwMaxMsgLen
            >> strTemp >> GetSetup()->lMaxConnectNum
            >> strTemp >> GetSetup()->lMaxClientsNum
            >> strTemp >> GetSetup()->lPendingWrBufNum
            >> strTemp >> GetSetup()->lPendingRdBufNum
            >> strTemp >> GetSetup()->lMaxSendSizePerSecond
            >> strTemp >> GetSetup()->lMaxRecvSizePerSecond
            >> strTemp >> GetSetup()->lMaxBlockedSendMsgNum
            >> strTemp >> GetSetup()->lConPendingWrBufNum
            >> strTemp >> GetSetup()->lConPendingRdBufNum
            >> strTemp >> GetSetup()->lConMaxSendSizePerSecond
            >> strTemp >> GetSetup()->lConMaxRecvSizePerSecond
            >> strTemp >> GetSetup()->lConMaxBlockedSendMsgNum
            >> strTemp >> GetSetup()->dwRefeashInfoTime
            >> strTemp >> GetSetup()->dwSaveInfoTime
            >> strTemp >> GetSetup()->dwGoodsLogTime
            >> strTemp >> GetSetup()->bWatchRunTimeInfo
            >> strTemp >> GetSetup()->dwWatchRunTime
            >> strTemp >> GetSetup()->dwMsgValidateTime;
        streamSetupEx.close();
    }
    else
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't find file SetupEx.ini");
        return false;
    }

    ifstream s("ChatServerSetup.ini");
    if(s.is_open())
    {
        s >> strTemp >> GetSetup()->bUseChatServer
            >> strTemp >> GetSetup()->strChatServerIp
            >> strTemp >> GetSetup()->dwChatServerPort4Gs
            >> strTemp >> GetSetup()->dwChatServerPort4Gc;
        s.close();
    }
    else
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't find file ChatServerSetup.ini");
        return false;
    }
    return true;
}

bool CGame::ReloadSetupEx()
{
    LoadSetup();
    s_pNetServer->SetParamEx(GetSetup()->lMaxBlockConNum,GetSetup()->lValidDelayRecDataTime);

    return true;
}


bool CGame::LoadMapFileSetup()
{
    string strTempt("");
    string strID("");
    string strMapObjName("");
    ifstream stream;
    stream.open("MapFileSetup.ini");
    if( stream.is_open() )
    {
        stream  >> strTempt>> strID
            >> strTempt >> strMapObjName;
        GetSetup()->dwGappID = atoi(strID.c_str());
        GetInst(CMemClient).Initial(strMapObjName.c_str(), m_pDBAllocator);
        return true;
    }
    return false;
}

void OutMemInfo(const char *pFileName, const char *pTxt)
{
    if(NULL == pTxt)
        return;

    if (NULL != pFileName)
    {
        char strFile[MAX_PATH];
        SYSTEMTIME stTime;
        GetLocalTime(&stTime);
        _snprintf(strFile, MAX_PATH, "%s\\MemPoolInfo_%04d-%02d-%02d.txt", pFileName, stTime.wYear, stTime.wMonth, stTime.wDay);

        FILE *fp;
        if( (fp=fopen(strFile, "a+")) == NULL )
        {
            return;
        }

        fseek(fp, 0, SEEK_END);
        char strTime[MAX_PATH];
        _snprintf(strTime, MAX_PATH, "%d:%d:%d\r\n", stTime.wHour, stTime.wMinute, stTime.wSecond);
        fwrite(strTime, strlen(strTime), 1, fp);
        fwrite(pTxt, strlen(pTxt), 1, fp);
        fclose(fp);
    }
}
// 游戏初始化
BOOL CGame::Init()
{
    SET_OUT_INFO_SETUP(OutMemInfo, "log\\");
    //创建一个log的保存目录
    CreateDirectory( "log",	NULL);

    srand((unsigned)time(NULL));
    random(100);

#ifdef _RUNSTACKINFO_
    InitStackFileName();
#endif	

#ifdef _GOODSLOG_
    InitGoodsLogFileName();
#endif

    if (LoadSetup()==false)
    {
        return false;
    }

    //分配数据块
    m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize,true);


    //初始化副本的场景配置
    m_pDupliRegionSetup = new CDupliRegionSetup;

    CShape::InitMoveCheckCellList();
    CPlayer::InitUpdateDirtyAttrData();
    CMonster::InitUpdateDirtyAttrData();

    //初始化消息
    CMessage::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum);
    //初始化消息的优先级和丢弃信息
    InitMsgInfo();
    // 初始化服务器网络
    CMySocket::MySocketInit();
    CMySocket::SetEncryptFlag(m_Setup.lEncryptType);

    LoadMapFileSetup();

    if(!InitNetServer() )
    {
        return false;
    }

    // 建立和worldserver的连接
    if( !StartConnWS() )
    {
		Log4c::Warn(ROOT_MODULE,"[GAME] Can't connect WorldServer, please reboot WorldServer!");
        CreateConnectWorldThread();
    }

    GameManager::GetInstance()->Initialize();

    if(GetSetup()->lUseBillingServer==BILL_CONNECT_NONE)
    {
		Log4c::Trace(ROOT_MODULE,"[GAME] BillingServer configured to not connect!");
    }
    else if(GetSetup()->lUseBillingServer==BILL_CONNECT_XM)
    {
        if(!StartConnBS())
        {
			Log4c::Warn(ROOT_MODULE,"[GAME] Can't connect BillingServer, please reboot BillingServer!");
            CreateConnectBSThread();
        }
    }
    else if(GetSetup()->lUseBillingServer==BILL_CONNECT_SNDA)
    {
        m_pSDOAOrderHandler=sdoaCreateOrderHandler();
        int ret=m_pSDOAOrderHandler->Initialize("sdoa4server.ini",MsgHandle);
        if(ret==0)
        {
            //ok
            m_bSDOAInitialized=true;
           Log4c::Trace(ROOT_MODULE, "[GAME] Init SDOA　success!");//SDOA初始化成功！
        }
        else
        {
            m_bSDOAInitialized=false;
            Log4c::Warn(ROOT_MODULE, "[GAME] Init SDOAOrderHandler　failed!");//SDOAOrderHandler初始化失败
        }
    }
    // 连接到LogServer
    if (0 == GetSetup()->lUseLogServer)
    {
		Log4c::Trace(ROOT_MODULE,"[GAME] LogServer configured to not connect!");// %s 配置为不连接
    }
    else
    {
        if( !StartConnLogS() )
        {
			Log4c::Warn(ROOT_MODULE,"[GAME] Can't connect LogServer, please reboot LogServer!");//[NET] 无法连接 %s, 请重启 %s
            CreateConnectLogThread();
        }
        else
            Log4c::Trace(ROOT_MODULE, "[GAME] Connect LogServer success!");//[NET] 连接 %s 成功
    }

    // 连接BounsServer
    if( 0 == GetSetup()->lUseBounsServer )
    {
		Log4c::Trace(ROOT_MODULE,"[GAME] BounsServer configured to not connect!");
    }
    else
    {
        if( !StartConnBounsServer())
        {
			Log4c::Warn(ROOT_MODULE,"[GAME] Can't connect BounsServer, please reboot BounsServer");//[NET] 无法连接 %s, 请重启 %s
            CreateConnectBounsServerThread();
        }
    }

    // 连接ChatServer
    GetSetup()->bUseChatServer==0
        ? Log4c::Warn(ROOT_MODULE,"[NET] Config: do not connect chat server.")
        : StartConnChatServer();

    GetInst( ScriptSys ).Init();

    //! 初始化星座日
    ResetStarDay((long)time(NULL));

    return true;
}

//初始化网络消息的优先级和丢弃信息
BOOL CGame::InitMsgInfo()
{
    //把聊天相关的消息注册为最低优先级的消息
    CBaseMessage::RegisterMsgPrio(MSG_S2C_OTHER_TALK,1);
    CBaseMessage::RegisterMsgPrio(MSG_S2C_OTHER_FACTIONTALK,1);
    CBaseMessage::RegisterMsgPrio(MSG_S2C_OTHER_BROADCAST,1);
    CBaseMessage::RegisterMsgPrio(MSG_S2C_OTHER_AddTopInfo,1);
    CBaseMessage::RegisterMsgPrio(MSG_S2C_OTHER_TALK_WORLD,1);
    CBaseMessage::RegisterMsgPrio(MSG_S2C_OTHER_TALK_COUNTRY,1);
    CBaseMessage::RegisterMsgPrio(MSG_S2C_OTHER_TALK_NOTIFY,1);

    //初始化需要丢弃的消息
    CBaseMessage::RegisterDiscardMsg(MSG_S2C_RGN_ADDSHAPE);
    CBaseMessage::RegisterDiscardMsg(MSG_S2C_SHAPE_MOVE);
    CBaseMessage::RegisterDiscardMsg(MSG_S2C_SHAPE_STOPMOVE);
    return true;
}


BOOL CGame::InitStackFileName()
{
    SYSTEMTIME stTime;
    GetLocalTime(&stTime);
    _snprintf(m_pszRunStackFile,MAX_PATH,"RunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);	
    return TRUE;
}

BOOL CGame::InitGoodsLogFileName()
{
    SYSTEMTIME stTime;
    GetLocalTime(&stTime);	
    _snprintf(m_pszGoodsLogFile,MAX_PATH,"GoodsLog[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
    return TRUE;
}

// 游戏释放
BOOL CGame::Release()
{
   Log4c::Trace(ROOT_MODULE, "Server Exiting...");

    if(m_pSDOAOrderHandler)
    {
        m_pSDOAOrderHandler->UnInitialize();
        m_pSDOAOrderHandler->Release();
    }	

    GetInst( ScriptSys ).Release();

    // Fox@20081105 玩家管理, 暂只用于在线玩家搜索
    GetInst(CPlayerMan).Release();

    //退出WS连接线程
    bConnectThreadExit=true;
    WaitForSingleObject(hConnectThread,INFINITE);
    CloseHandle(hConnectThread);

    //退出DBS连接线程
    bConnectThreadExitDBS=true;
    WaitForSingleObject(hConnectThreadDBS,INFINITE);
    CloseHandle(hConnectThreadDBS);

    bConnectThreadExitBS=true;
    WaitForSingleObject(hConnectThreadBS,INFINITE);
    CloseHandle(hConnectThreadBS);

    bConnectThreadExitBouns = true;
    WaitForSingleObject(hConnectThreadBouns, INFINITE);
    CloseHandle(hConnectThreadBouns);



    // 修改物品ID后，不需要更新了。
    // 发送GOODSID
    //CMessage msg(MSG_S2W_SERVER_UPDATEGOODSID);
    //msg.Add(GetGame()->GetIDIndex());
    //msg.Add(GetGame()->GetGoodsID());
    //msg.Send();

    char str[256] = "";
    try
    {
        // 清除组织
        GameOrganizingCtrl::Release();

        // 清除玩家
        for(itPlayer it=s_mapPlayer.begin(); it!=s_mapPlayer.end(); it++)
        {
            //delete (it->second);
            GameManager::GetInstance()->FreeObject((CBaseObject**)&(it->second));
        }
    }
    catch(...)
    {
        Log4c::Trace(ROOT_MODULE, "Clear Player data error!");
    }
   Log4c::Trace(ROOT_MODULE, "Clear Player data ok!");

    s_mapPlayer.clear();

    MedalContainer::Release();
    GoodsSetup::Release();
    SuitRuleSetup::Release();
    KitCardRuleSetup::Release();
    CMonsterList::Release();
    CDeputyOccuSystem::Relese();
    CSessionFactory::Release();
    GoodsTailorSetup::Release();


    if( s_pNetServer )
    {
        s_pNetServer->DisConn(m_lWSSocket);
        s_pNetServer->Exit();		
    }
    SAFE_DELETE(s_pNetServer);

    CMySocket::MySocketClearUp();

    GetNetSessionManager()->Release();
    GetPKSys()->Release();
    CQuestSystem::Relese();

    CleanSequenceMap();
    CleanValidateTime();

    // 清除奖励描述
    ClearBounsDes();

    // 清除用于验证消息合法性的序列字符串
    CSequenceString::Uninitialize();

    CWordsFilter::Release();

    SAFE_DELETE(m_pDupliRegionSetup);

    CCountryHandler::GetInstance()->Release();
    CCountryParam::GetInstance()->Release();

    GetInst( AntiCheat::CPlayerManager ).Release();
    GetInst( FBusiness::CShopManager ).Release();
    GameManager::GetInstance()->Release();
    GameManager::ReleaseInstance();


    CNewSkillFactory::ClearSkillCache();
    COccupSkillList::Release();
    CStudySkillCondition::Release();
    MailList::Release();
    ReLiveGoodsList::Release();
    GetInst(CCreditSystem).Release();
    //DeleteCriticalSection(&m_CriticalSection);

    // 先清除原有数据
    //M_FREE(m_strFunctionList_FileData, m_lFuncFileDataLen);
    //M_FREE(m_strVariableList_FileData, m_lVariableFileDataLen);
    //for(map<string, tagScriptDataInfo*>::iterator it=m_mapScript_FileData.begin(); it!=m_mapScript_FileData.end(); it++)
    //{
    //    OBJ_RELEASE(tagScriptDataInfo, it->second);
    //}
    //m_mapScript_FileData.clear();
    //Log4c::Trace(ROOT_MODULE, "Release m_mapScript_FileData ok!");

    //CScript::ReleaseFunction();
    //CScript::ReleaseGeneralVariable();	

    Log4c::Trace(ROOT_MODULE, "Release CScript Data ok!");
    // 清除全局的FreeScriptList
    //FreeScript::iterator scriptItr = g_FreeScriptList.begin();
    //while(scriptItr != g_FreeScriptList.end())
    //{
    //	OBJ_RELEASE(CScript, *scriptItr);
    //	scriptItr++;
    //}
    //g_FreeScriptList.clear();
    Log4c::Trace(ROOT_MODULE, "Clear FREE scripts data ok!");
    // 清除全局的运行ScriptList
    //itScript rgnScriptItr = g_Scripts.begin();
    //while(rgnScriptItr != g_Scripts.end())
    //{
    //	OBJ_RELEASE(CScript, rgnScriptItr->second);
    //	rgnScriptItr++;
    //}
    //g_Scripts.clear();
    // 清除递归调用返回值变量列表
    //CScript::ReleaseStackVarListInst();
   Log4c::Trace(ROOT_MODULE, "Clear Running scripts data ok!");

    //! 新日志系统
    SAFE_DELETE(m_pLogInterface);
    //SAFE_DELETE(g_pMailManager);

    // 关闭存盘线程句柄
    WaitForSingleObject(g_hSavingThread, INFINITE);
    CloseHandle(g_hSavingThread);
    GetInst(CMemClient).Release();

    CMessage::Release();
    SAFE_DELETE(m_pDBAllocator);
    Log4c::Trace(ROOT_MODULE, "Server Exited!");
    return true;
}

//为在线玩家发送收信信息
void CGame::SendSysMailMsgToOnlinePlayer(CMail* pMail)
{
    itPlayer it = s_mapPlayer.begin();
    for (;it!=s_mapPlayer.end();++it)
    {
        CPlayer *pPlayer = (*it).second;
        GameManager::GetInstance()->GetMailManager()->SendSysMailMsg(pMail,pPlayer);
    }
}

void CGame::AddReconnectionMsg(DBWriteSet& setWriteDB)
{
    // 发送全局变量值到WS
    //if(CScript::GetGeneralVariableList())
    //{
    //	setWriteDB.AddToByteArray((BYTE)1);
    //	CScript::GetGeneralVariableList()->CodeToDataBlock(setWriteDB);
    //}
    if( GetInst( ScriptSys ).GetGeneralVarList()->Count() > 0 )
    {
        setWriteDB.AddToByteArray( (BYTE) 1 );
        VarList::Encode( GetInst( ScriptSys ).GetGeneralVarList(), setWriteDB );
    }
    else
        setWriteDB.AddToByteArray((BYTE)0);

    // 发送每个玩家数据 MSG_S2DB_SERVER_QUEST_RECONNECTION : 1
    setWriteDB.AddToByteArray((long)s_mapPlayer.size());
    for(itPlayer it=s_mapPlayer.begin(); it!=s_mapPlayer.end(); it++)
    {
        CPlayer* pPlayer=it->second;
        if(pPlayer)
        {
            setWriteDB.AddToByteArray((BYTE)1);
            setWriteDB.AddToByteArray(pPlayer->GetExID());
            pPlayer->CodeToDataBlock(setWriteDB);
        }
        else
        {
            setWriteDB.AddToByteArray((BYTE)0);
        }
    }

    setWriteDB.AddToByteArray((long)GetSavePlayerMap().size());
    map<CGUID, CPlayer*>::iterator saveitr = GetSavePlayerMap().begin();
    while( saveitr != GetSavePlayerMap().end() )
    {
        if(saveitr->second)
        {
            setWriteDB.AddToByteArray((BYTE)1);
            setWriteDB.AddToByteArray( saveitr->second->GetExID() );
            saveitr->second->CodeToDataBlock(setWriteDB);
            // 清除数据
            GameManager::GetInstance()->FreeObject((CBaseObject**)&(saveitr->second));
            saveitr++;
        }
        else
        {
            setWriteDB.AddToByteArray((BYTE)0);
        }
    }
    GetSavePlayerMap().clear();

    // 发送副本及场景数据 MSG_S2DB_SERVER_QUEST_RECONNECTION : 3
    // rgn num
    long rgnNum = (long)GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().size()
        + (long)GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().size()
        + (long)GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().size();

    setWriteDB.AddToByteArray((long)rgnNum);

    vector<BYTE> pBA;
    // p rgn num
    CRgnManager::RgnByIDMapItr rgnItr = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().begin();
    for(; rgnItr != GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end(); rgnItr++)
    {
        pBA.clear();
        // rgn data
        rgnItr->second->AddToByteArray(&pBA);
        setWriteDB.AddToByteArray((long)pBA.size()); //rgn data buf size
        setWriteDB.AddToByteArray(&pBA[0], (long)pBA.size());

        pBA.clear();
        //rgnItr->second->GetVariableList()->AddToByteArray(&pBA);
        VarList::Encode( rgnItr->second->GetVariableList(), &pBA );
        setWriteDB.AddToByteArray((long)pBA.size());//rgn varlist data size
        // varlist data
        setWriteDB.AddToByteArray(&pBA[0], (long)pBA.size());
    }

    // ph rgn num
    rgnItr = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().begin();
    for(; rgnItr != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end(); rgnItr++)
    {
        pBA.clear();
        // rgn data
        rgnItr->second->AddToByteArray(&pBA);
        setWriteDB.AddToByteArray((long)pBA.size()); //rgn data buf size
        setWriteDB.AddToByteArray(&pBA[0], (long)pBA.size());

        pBA.clear();
        //rgnItr->second->GetVariableList()->AddToByteArray(&pBA);
        VarList::Encode( rgnItr->second->GetVariableList(), &pBA );
        setWriteDB.AddToByteArray((long)pBA.size());//rgn varlist data size
        // varlist data
        setWriteDB.AddToByteArray(&pBA[0], (long)pBA.size());
    }

    // t rgn num
    rgnItr = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().begin();
    for(; rgnItr != GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end(); rgnItr++)
    {
        pBA.clear();
        // rgn data
        rgnItr->second->AddToByteArray(&pBA);
        setWriteDB.AddToByteArray((long)pBA.size()); //rgn data buf size
        setWriteDB.AddToByteArray(&pBA[0], (long)pBA.size());

        pBA.clear();
        //rgnItr->second->GetVariableList()->AddToByteArray(&pBA);
        VarList::Encode( rgnItr->second->GetVariableList(), &pBA );
        setWriteDB.AddToByteArray((long)pBA.size());//rgn varlist data size
        // varlist data
        setWriteDB.AddToByteArray(&pBA[0], (long)pBA.size());
    }

    // 发送个人房屋副本场景物品数据 MSG_S2DB_SERVER_QUEST_RECONNECTION : 4
    CRgnManager::PHGoodsMap& goodsMap = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap();
    CRgnManager::PHGoodsMapItr goodsItr = goodsMap.begin();
    vector<BYTE> PHGoodsBufArray;

    for(; goodsItr != goodsMap.end(); goodsItr++)
    {
        // DupRgnID
        setWriteDB.AddToByteArray(goodsItr->first);

        // DupRgn goods num
        setWriteDB.AddToByteArray((long)goodsItr->second.size());

        // DupRgn Goods data
        CRgnManager::vecPHGoods::iterator vecItr = goodsItr->second.begin();
        for(; vecItr != goodsItr->second.end(); vecItr++)
            setWriteDB.AddToByteArray(&(*vecItr), sizeof(CRgnManager::PHGoods));
    }
    setWriteDB.AddToByteArray((long)GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap().size()); // 个人房屋个数
    setWriteDB.AddToByteArray((long)PHGoodsBufArray.size());
    if(PHGoodsBufArray.size())
        setWriteDB.AddToByteArray(&PHGoodsBufArray[0], (long)PHGoodsBufArray.size());
}
BOOL CGame::ReConnectWorldServer()
{
    m_lWSSocket = s_pNetServer->Connect(GetSetup()->strWorldIP.c_str(), GetSetup()->dwWorldPort,NF_WServer_Client);
    if(m_lWSSocket>0)
    {
        CMessage::SetWSSocket(m_lWSSocket);
        Log4c::Trace(ROOT_MODULE,FormatText("GS_GAME_7", "WorldServer"));// 连接 %s 成功

        SetServerInitSendFlag(World_Server_InitSendFlag);
        return TRUE;
    }
    return FALSE;
}

BOOL CGame::ReConnectBSServer()
{
    if(m_listBSSetup.size()<1)
        return FALSE;
    if(!s_pNetServer)
        return FALSE;
    list<tagBSSetup>::iterator it=m_listBSSetup.begin();
    tagBSSetup stSetup=*it;
    m_lBSSocket=s_pNetServer->Connect(stSetup.strIp.c_str(),stSetup.dwPort,NF_BillingServer_Client);

    if(m_lBSSocket>0)
    {
        m_bBSConnected=TRUE;
        CMessage::SetBSSocket(m_lBSSocket);
         Log4c::Trace(ROOT_MODULE,FormatText( "GS_GAME_7", "BillingServer"));//[NET] 连接 %s 成功
        CMessage msg(MSG_S2B_SENDINFO);
        msg.Add((char)1); //reconnect
        msg.Add(GetSetup()->dwListenPort);
        msg.Add((char*)GetSetup()->strLocalIP.c_str());
        msg.SendToSocket(m_lBSSocket);
        return TRUE;
    }
    return FALSE;	
}

bool CGame::ReConnectBounsServer()
{
    return StartConnBounsServer(true);
}

//创建连接线程
void   CGame::CreateConnectWorldThread()
{
    bConnectThreadExit=true;
    WaitForSingleObject(hConnectThread,INFINITE);
    CloseHandle(hConnectThread);

    bConnectThreadExit=false;

    unsigned threadID;
    hConnectThread = (HANDLE)_beginthreadex(NULL,0,ConnectWorldFunc,NULL,0,&threadID);
}

void  CGame::CreateConnectBSThread()
{
    bConnectThreadExitBS=true;
    WaitForSingleObject(hConnectThreadBS,INFINITE);
    CloseHandle(hConnectThreadBS);
    bConnectThreadExitBS=false;
    unsigned threadID;
    hConnectThreadBS = (HANDLE)_beginthreadex(NULL,0,ConnectBSFunc,NULL,0,&threadID);	
}

// 创建连接BounsServer的线程
void CGame::CreateConnectBounsServerThread()
{
    bConnectThreadExitBouns = true;
    WaitForSingleObject(hConnectThreadBouns, INFINITE);
    CloseHandle(hConnectThreadBouns);

    bConnectThreadExitBouns = false;
    unsigned threadID;
    hConnectThreadBouns = (HANDLE)_beginthreadex(NULL, 0, ConnectBounsServerFunc, NULL, 0, &threadID);
}

//初始化游戏网络服务器端，建立网络服务器端，装载数据
BOOL CGame::InitNetServer(void)
{
    s_pNetServer = new CServer;
    //做为服务器和客户端双重身份启动
    long lRet = s_pNetServer->Start(1|2,m_pDBAllocator,
        m_Setup.dwFreeSockOperNum,m_Setup.dwFreeIOOperNum,m_Setup.lIOOperDataBufNum,
        m_Setup.bCheckNet,m_Setup.dwBanIPTime,
        m_Setup.dwMaxMsgLen,m_Setup.lMaxConnectNum,m_Setup.lMaxClientsNum,
        m_Setup.lPendingWrBufNum,m_Setup.lPendingRdBufNum,
        m_Setup.lMaxSendSizePerSecond,m_Setup.lMaxRecvSizePerSecond,
        m_Setup.lMaxBlockedSendMsgNum,
        m_Setup.lConPendingWrBufNum,m_Setup.lConPendingRdBufNum,
        m_Setup.lConMaxSendSizePerSecond,m_Setup.lConMaxRecvSizePerSecond,
        m_Setup.lConMaxBlockedSendMsgNum);
    if( !lRet )
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"[NET] Init net module +++ FAILED +++, ErrorID: %u", ::GetLastError());
        return FALSE;
    }
    CMessage::SetServer(s_pNetServer);
	Log4c::Trace(ROOT_MODULE,"[NET] Init net module success!");

    return TRUE;
}

BOOL CGame::StartAccept()
{
    if(s_pNetServer == NULL)	return FALSE;

    BOOL bRet = s_pNetServer->Host(m_Setup.dwListenPort,NULL,NF_Server_Client,SOCK_STREAM);
    if(!bRet)
    {
       Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"GAME_NET_LISTENFAILED %s",::GetLastError());
        return FALSE;
    }
    Log4c::Trace(ROOT_MODULE,"GAME_NET_LISTENOK");
    return TRUE;
}

//建立和WordServer的连接，作为客户端初始化
BOOL CGame::StartConnWS(void)
{
    m_lWSSocket = s_pNetServer->Connect(GetSetup()->strWorldIP.c_str(), GetSetup()->dwWorldPort,NF_WServer_Client);
    if(m_lWSSocket>0)
    {
        CMessage::SetWSSocket(m_lWSSocket);

		Log4c::Trace(ROOT_MODULE,"[GAME] Connect WorldServer success!");

        CMessage msg(MSG_S2W_SERVER_SENDSERVERINFO);
        msg.Add((char)0);	// first connect
        msg.Add(GetSetup()->dwListenPort);
        msg.Add((char*)GetSetup()->strLocalIP.c_str());
        msg.SendToSocket(m_lWSSocket);

        b_IsFirstConnectionWS = false;
        return TRUE;
    }
    return FALSE;
}


BOOL CGame::StartConnBS()
{
    if(m_listBSSetup.size()<1)
        return FALSE;
    list<tagBSSetup>::iterator it=m_listBSSetup.begin();
    tagBSSetup stSetup=*it;
    m_lBSSocket=s_pNetServer->Connect(stSetup.strIp.c_str(),stSetup.dwPort,NF_BillingServer_Client);
    if(m_lBSSocket>0)
    {
        m_bBSConnected=TRUE;
        CMessage::SetBSSocket(m_lBSSocket);
        Log4c::Trace(ROOT_MODULE,FormatText( "GS_GAME_7", "BillingServer"));//[NET] 连接 %s 成功
        CMessage msg(MSG_S2B_SENDINFO);
        msg.Add((char)0); //first connect
        msg.Add(GetSetup()->dwListenPort);
        msg.Add((char*)GetSetup()->strLocalIP.c_str());
        msg.SendToSocket(m_lBSSocket);
        return TRUE;
    }
    return FALSE;
}

bool CGame::StartConnBounsServer(bool bReconn)
{
    if(m_listBDSSetup.size() < 1)
    {
        return false;
    }

    list<tagBSSetup>::iterator itor = m_listBDSSetup.begin();
    for(; itor != m_listBDSSetup.end(); ++itor)
    {
        tagBSSetup tgSetup = *itor;
        m_lBounsSocket = s_pNetServer->Connect(tgSetup.strIp.c_str(), tgSetup.dwPort, NF_BounsServer_Client);
        if(m_lBounsSocket > 0)
        {
            m_bBounsSConnected = true;
            CMessage::SetBounsSocket(m_lBounsSocket);
            Log4c::Trace(ROOT_MODULE,FormatText( "GS_GAME_7", "BounsServer"));//[NET] 连接 %s 成功
            CMessage msg(MSG_S2BDS_SENDSERVERINFO);
            msg.Add(GetSetup()->strLocalIP.c_str());
            msg.Add(GetSetup()->dwGameServerNum);
            msg.SendToSocket(m_lBounsSocket);
            return true;
        }
    }

    return false;
}

bool CGame::StartConnChatServer()
{
    long socket = s_pNetServer->Connect(m_Setup.strChatServerIp.c_str(), m_Setup.dwChatServerPort4Gs, NF_ChatServer_Client);
    if (socket > 0)
    {
        CMessage::SetChatServerSocket(socket);
        Log4c::Trace(ROOT_MODULE,"[NET] Connects chat server success.");
        return true;
    }
    else
    {
		Log4c::Warn(ROOT_MODULE,"[NET] Connects chat server fail.");
        CreateConnectChatServerThread();
        return false;
    }
}

bool CGame::ReConnectChatServer()
{
    long socket = s_pNetServer->Connect(m_Setup.strChatServerIp.c_str(), m_Setup.dwChatServerPort4Gs, NF_ChatServer_Client);
    if (socket > 0)
    {
        CMessage::SetChatServerSocket(socket);
       Log4c::Trace(ROOT_MODULE,"[NET] Connects chat server success.");

        // validate all players for chat server
        for (itPlayer it = s_mapPlayer.begin();
            it != s_mapPlayer.end(); ++it)
        {
            it->second->SendValidateMsgForCs();
        }

        return true;
    }
    return false;
}

bool CGame::IsChatServerConnected()
{
    return CMessage::GetChatServerSocket() > 0 ? true : false;
}

void CGame::OnChatServerLost()
{
    CMessage::SetChatServerSocket(0);
    Log4c::Warn(ROOT_MODULE,"[NET] Chat server is losted.");
    CreateConnectChatServerThread();
}

void CGame::CreateConnectChatServerThread()
{
    bConnectThreadExitChatServer = true;
    WaitForSingleObject(hConnectThreadChatServer, INFINITE);
    CloseHandle(hConnectThreadChatServer);

    bConnectThreadExitChatServer = false;
    unsigned threadID;
    hConnectThreadChatServer = (HANDLE)_beginthreadex(NULL, 0, ConnectChatServerFunc, NULL, 0, &threadID);
}

//建立和DBServer的连接，作为客户端初始化
BOOL CGame::StartConnDBS(void)
{
    return FALSE;
}
void CGame::OnPlayerLost(const CGUID& PlayerGuid,long lSocketID)
{
    GameManager::GetInstance()->RemoveLoginSessionBySocketID(lSocketID);
    OnPlayerExit(PlayerGuid);

    RemovePlayer(PlayerGuid);
}


void CGame::OnPlayerExit(const CGUID& PlayerGuid, BOOL isChangeServer)
{
#ifdef _RUNSTACKINFO1_
    char pszStatckInfo[1024]="";
    char pszGUID[50]="";
    PlayerGuid.tostring(pszGUID);
    _snprintf(pszStatckInfo,1024,"CGame::OnPlayerExit(PlayerID:%s) Start",pszGUID);
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
    if(NULL_GUID == PlayerGuid)	return;

    m_TransCongPlayers.erase(PlayerGuid);

    CPlayer* pPlayer = FindPlayer(PlayerGuid);

    if(!pPlayer)
    {
        return;
    }
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::OnPlayerExit() DelPlayer Start");
#endif

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::OnPlayerExit() OnExitRegion Start");
#endif
    //触发该事件
    pPlayer->OnExitRegion();		
    //从场景中删除该玩家
    CServerRegion* pServerRegion = dynamic_cast<CServerRegion*>( pPlayer -> GetFather() );
    if(pServerRegion)
    {
#ifdef _RUNSTACKINFO1_
        CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::OnPlayerExit() pServerRegion->PlayerExit Start");
#endif
        pServerRegion->PlayerExit(pPlayer);
    }

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::OnPlayerExit() pPlayer->OnExit Start");
#endif
    pPlayer->OnExit(isChangeServer);

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::OnPlayerExit() End");
#endif
}

void CGame::UpdateSaveRegion(CServerRegion* pRegion)
{
    if(pRegion)
    {
        set<CServerRegion*>::iterator itr = m_setSaveRegion.find(pRegion);
        if(itr == m_setSaveRegion.end())
        {
            m_setSaveRegion.insert(pRegion);
        }		
    }
}

//网络传输从畅通到拥塞时触发
void CGame::OnTransferCongestion(const CGUID& PlayerGuid,long lSocketID)
{
    CPlayer* pPlayer = FindPlayer(PlayerGuid);
    if(!pPlayer)	return;
    m_TransCongPlayers.insert(PlayerGuid);
    pPlayer->SetNetTransferStatus(true);
    return;
}
//网络传输从拥塞到畅通时触发
void CGame::OnTransferSmoothly(const CGUID& PlayerGuid,long lSocketID)
{
    CPlayer* pPlayer = FindPlayer(PlayerGuid);
    if(!pPlayer)	return;
    m_TransCongPlayers.erase(PlayerGuid);
    pPlayer->SetNetTransferStatus(false);
    return;
}

void CGame::RemoveTransferCongPlayerID(hashsetGUID& PlayersGUID)
{
    if(m_TransCongPlayers.size() == 0)	return;
    hsGUIDIt it = PlayersGUID.begin();
    for(;it != PlayersGUID.end();)
    {
        if(m_TransCongPlayers.find((*it)) != m_TransCongPlayers.end())
            it =PlayersGUID.erase(it);
        else
            it++;
    }

}
bool CGame::IsTransferCongPlayerID(const CGUID& PlayerGUID)
{
    return !(m_TransCongPlayers.find(PlayerGUID) == m_TransCongPlayers.end());
}

void CGame::ClearSaveRegion(CServerRegion* pRegion)
{
    std::set<CServerRegion*>::iterator it=m_setSaveRegion.find(pRegion);
    if(it!=m_setSaveRegion.end())
        m_setSaveRegion.erase(it);
}

void CGame::ClearSaveRegion()
{
    m_setSaveRegion.clear();
}

void CGame::OnWSLost()
{
    Log4c::Trace(ROOT_MODULE, "============WorldServer closed==========!!!");
    m_lWSSocket = 0;
    CMessage::SetWSSocket(0);
}

void CGame::SetServerInitSendFlag(eServerInitSendMsgFlag flag) 
{
    EnterCriticalSection(&m_CSServerInitSendFlag);	
    m_ServerInitSendFlag = m_ServerInitSendFlag | flag;
    LeaveCriticalSection(&m_CSServerInitSendFlag);
}
void CGame::ProcessServerInitSendMsg()
{
    if(m_ServerInitSendFlag == Null_Server_InitSendFlag) return;

    EnterCriticalSection(&m_CSServerInitSendFlag);
    if(m_ServerInitSendFlag & Log_Server_InitSendFlag)
    {

    }
    if(m_ServerInitSendFlag & Bill_Server_InitSendFlag)
    {

    }
    if(m_ServerInitSendFlag & World_Server_InitSendFlag)
    {
        //! 发送玩家联系人数据到WS
        LinkmanSystem::GetInstance().SendDataToWs();

        // 发送玩家数据和副本数据到WS
        CMessage msg1(MSG_S2W_SERVER_SENDSERVERINFO);
        if(false == b_IsFirstConnectionWS) // 不是初次连接
            msg1.Add((char)1);	// reconnect?
        else
            msg1.Add((char)0);

        msg1.Add(GetSetup()->dwListenPort);
        msg1.Add((char*)GetSetup()->strLocalIP.c_str());
        DBWriteSet writeSet;
        msg1.GetDBWriteSet(writeSet);
        AddReconnectionMsg(writeSet);
        msg1.SendToSocket(m_lWSSocket);
    }
    m_ServerInitSendFlag = Null_Server_InitSendFlag;
    LeaveCriticalSection(&m_CSServerInitSendFlag);
}
// 消息处理
BOOL CGame::ProcessMessage()
{
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessMessage() Start");
#endif
    static msgQueue TemptMsgQueue;
    //处理客户端来的消息
    if(s_pNetServer)
    {
        s_pNetServer->GetRecvMessages()->GetAllMessage(TemptMsgQueue);
        msgQueue::iterator it = TemptMsgQueue.begin();
        for(;it != TemptMsgQueue.end();it++)
        {
            CMessage* pMsg = (CMessage*)(*it);
            if( pMsg )
            {
                long lType = pMsg->GetType();
                DWORD dwStartTime = timeGetTime();
                pMsg->Run();
                AddMsgExpendTime(lType,timeGetTime()-dwStartTime);
                CBaseMessage::FreeMsg(pMsg);
            }
        }
    }

    TemptMsgQueue.clear();
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessMessage() End");
#endif
    return TRUE;
}

bool CGame::ProcMenus()
{
    if (AppFrame::MenuClicked(CONNECT_WOLDSERVER)) // 连接WorldServer
        GetGame()->CreateConnectWorldThread();
    if (AppFrame::MenuClicked(CONNECT_BOUNSSERVER) &&
        GetGame()->GetSetup()->lUseBounsServer) // 连接BounsServer
        GetGame()->CreateConnectBounsServerThread();
    if (AppFrame::MenuClicked(LOAD_SETUPEX)) // 加载配置文件
        ReloadSetupEx();
    if (AppFrame::MenuClicked(LOAD_MAPFILESETUP)) // 加载内存映射配置
        LoadMapFileSetup();
    if (AppFrame::MenuClicked(SHOW_SCRIPTINFO)) // 显示脚本信息
    {
        if (AppFrame::MenuClicked(SHOW_MEMPOOLINFO)) // 显示内存池信息
        {
            MEMORYSTATUS msC;
            GlobalMemoryStatus(&msC);
            size_t dwCurAvaiVirMem = msC.dwAvailVirtual/1048576;
            char szTmp[128] = {0};
            _snprintf(szTmp, 128, "CurAvaiVirMem = %d M\r\n", dwCurAvaiVirMem);
            OutMemInfo("log\\", szTmp);
            OUT_RUNNING_INFO(true);
        }
    }
    return true; 
}

// 游戏主循环
BOOL CGame::MainLoop()
{
    // 结束线程
    if (g_bGameThreadExit)
    {
        return false;
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyClearFile(GetGame()->GetStatckFileName());
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MainLoop() Start.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessServerInitSendMsg Start.");
#endif
    ProcessServerInitSendMsg();
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessServerInitSendMsg End.");
#endif
    /*static bool bStart = true;
    if (bStart)
    {
    CMessage msg(MSG_GAPP2SM_GAPP_STARTOK);
    msg.Add(GetSetup()->dwGappID);
    msg.SendToSM();
    bStart = false;
    }*/
    // 统计每个步骤的耗时
    static DWORD dwScriptTime = 0, dwAITime = 0, dwMessageTime = 0, dwSessionTime = 0, 
        dwNetSessionTime = 0,dwRgnManagerTime = 0,dwGameManagerTime = 0, dwBillClientTime = 0;
    static DWORD dwCurTime = 0;

    static DWORD ai_tick, now = timeGetTime(), lClearChangeRgnPlayer = now, old = now, lastRefeashInfoTime = now, lastSaveTime = now, lastDelayOperPlayerTime = now,lastGoodsLogClearTime=now,lCheckBaseModuleTime=now
        ;
    now = timeGetTime();
    ai_tick++;

#ifdef _GOODSLOG_
    if(now-lastGoodsLogClearTime>GetSetup()->dwGoodsLogTime*60*1000)
    {
        CMessage::AsyClearFile(GetGame()->GetGoodsLogFile());
        lastGoodsLogClearTime=now;
    }	
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetRegionTimer()->Expire() Start.");
#endif
    // 场景定时器
    dwCurTime = timeGetTime();
    GameManager::GetInstance()->GetRgnManager()->GetRegionTimer()->Expire(now);
    dwRgnManagerTime += timeGetTime() - dwCurTime;

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetRegionTimer()->Expire() End.");
#endif


#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetQueueTimer()->Expire() Start.");
#endif
    // 副本相关定时器
    GameManager::GetInstance()->GetRgnManager()->GetQueueTimer()->Expire(now);
    GameManager::GetInstance()->GetRgnManager()->GetCreatePRgnTimer()->Expire(now);
    GameManager::GetInstance()->GetRgnManager()->GetCreateTRgnTimer()->Expire(now);
    GameManager::GetInstance()->GetRgnManager()->GetRgnKickPlayerTimer()->Expire(now);
    GSAutoSupply::GetTimerQueue().Expire(now);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetQueueTimer()->Expire End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"RefreshInfo() Start.");
#endif
    // 刷新显示
    if (now - lastRefeashInfoTime > GetSetup()->dwRefeashInfoTime)
    {
        lastRefeashInfoTime = now;
        RefreshInfo();
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"RefreshInfo() End");
#endif


#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CheckBaseModuleTimeOut() Start.");
#endif	
    if ( (GlobalSetup::GetSetup()->lBaseModuleTime != 0) 
        && ( (long)(now - lCheckBaseModuleTime) > GlobalSetup::GetSetup()->lBaseModuleTime) )
    {
        lCheckBaseModuleTime = now;
        CNewSkillFactory::CheckBaseModuleTimeOut(now);
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CheckBaseModuleTimeOut() End.");
#endif




#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Send Save Data Start.");
#endif
    //到达存盘点
    dwCurTime = timeGetTime();
    if ( (GlobalSetup::GetSetup()->lSaveTime != 0) 
        && ((long)(now - lastSaveTime) > GlobalSetup::GetSetup()->lSaveTime) )
    {
        lastSaveTime=now;	
        // 发送保存消息
        SendQueuePlayerOperMsg();
        SendSaveRegionDetailMsg();	
        if( GetInst( Business::CBusinessManager ).IsEnable() )
        {
            GetInst( Business::CBusinessManager ).SendSaveMsg();
        }
        m_setSaveRegion.clear();

        //零时输出对象分配信息

        //m_pDBAllocator->PutAllocInfo("MemAllocInfo");
        GameManager::GetInstance()->PutObjNumInfo("MemAllocInfo");
        CGoodsFactory::OutputUseGoodsInfo("MemAllocInfo");
        CNewSkillFactory::OutputBasemodelInfo("MemAllocInfo");

        OutPlayerAndRegionInfo("MemAllocInfo");
        //输出消息统计信息
        //GetGame()->GetNetServer()->OutputMsgStatInfo();

        //! 记录地图日志
        LogRegion();

        //! 输出内存池日志
        MEMORYSTATUS msC;GlobalMemoryStatus(&msC);
        size_t dwCurAvaiVirMem = msC.dwAvailVirtual/1048576;
        Log4c::Trace(ROOT_MODULE, "CurAvaiVirMem = %d M\r\n", dwCurAvaiVirMem);
        OUT_RUNNING_INFO(false);
    }

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Send Save Data End.");
#endif

    lastSaveTime += timeGetTime() - dwCurTime;
    if( now - old > GetSetup()->dwWatchRunTime )
    {
        if (GetSetup()->bWatchRunTimeInfo)
        {
            //Log4c::Trace(ROOT_MODULE,FormatText"GS_GAME_0", //%d秒调用了 %d 次AI/r\nScript:%d\r\nAI:%d\r\nMessage:%d\r\nSession:%d\r\nNetSession:%d\r\nRgnManger:%d\r\nGameManger:%d\r\nBillClient:%d
            //    GetSetup()->dwWatchRunTime/1000, ai_tick, dwScriptTime, dwAITime, dwMessageTime, dwSessionTime, 
            //    dwNetSessionTime,dwRgnManagerTime, dwGameManagerTime, dwBillClientTime));


			Log4c::Trace(ROOT_MODULE,"%d second call %d time AI/r\nScript:%d\r\nAI:%d\r\nMessage:%d\r\nSession:%d\r\nNetSession:%d\r\nRgnManger:%d\r\nGameManger:%d\r\nBillClient:%d", 
				GetSetup()->dwWatchRunTime/1000, ai_tick, dwScriptTime, dwAITime, dwMessageTime, dwSessionTime, 
				dwNetSessionTime,dwRgnManagerTime, dwGameManagerTime, dwBillClientTime);


            OutputMsgExpendTimeInfo("TimeExpendInfo");
#ifdef _SCRIPT_INFO_
            string strScriptInfo;
            GetScriptInfo(strScriptInfo);
            Log4c::Trace(ROOT_MODULE,strScriptInfo.c_str());
            ClearScriptInfo();
#endif

            dwScriptTime = 0;
            dwAITime = 0;
            dwMessageTime = 0;
            dwSessionTime = 0;
            dwNetSessionTime = 0;
            dwRgnManagerTime = 0;
            dwGameManagerTime = 0;
            dwBillClientTime = 0;
        }
        else
        {
            Log4c::Trace(ROOT_MODULE, "GS_GAME_1", GetSetup()->dwWatchRunTime/1000, ai_tick);//%d秒调用了 %d 次AI
        }

        ai_tick = 0;
        old = now;
    }

    AddAITick();

    // 性能监控
    if (GetSetup()->bWatchRunTimeInfo)
    {
        // 游戏主AI
        dwCurTime = timeGetTime();
        dwAITime += timeGetTime() - dwCurTime;

        // 消息处理
        dwCurTime = timeGetTime();
        ProcessMessage();
        ProcMenus();
        dwMessageTime += timeGetTime() - dwCurTime;

        //##所有会话的处理
        dwCurTime = timeGetTime();
        CSessionFactory::AI();
        dwSessionTime += timeGetTime() - dwCurTime;

        // 网络会话
        dwCurTime = timeGetTime();
        GetNetSessionManager()->Run();
        dwNetSessionTime += timeGetTime() - dwCurTime;

        dwCurTime = timeGetTime();
        GameManager::GetInstance()->Run();
        dwGameManagerTime += timeGetTime() - dwCurTime;
    }
    else
    {
        // 消息处理
        ProcessMessage();
        ProcMenus();

        //##所有会话的处理
        CSessionFactory::AI();

        // 网络会话
        GetNetSessionManager()->Run();

        GameManager::GetInstance()->Run();
    }

    //! [2007-8-6]更新队伍里成员的坐标
    static long lOldTimeForTeam = 0;
    if(timeGetTime() - lOldTimeForTeam > 10000)
    {
        lOldTimeForTeam = timeGetTime();
        GetOrganiCtrl()->UnpdateTeamMemberPos();
    }

    // 定时清除切换场景的玩家数据
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ClearChangeRgnPlayerVec() Start.");
#endif
    ClearChangeRgnPlayerVec();
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ClearChangeRgnPlayerVec() End.");
#endif

    // 定时清除场景数据
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ClearDupRgn() Start.");
#endif
    ClearDelDupRgn();
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ClearDupRgn() End.");
#endif

    CNewSkillFactory::RealeaseModuParam();

    CGoodsFactory::ReleaseGCGoodsSet();
    //删除待删除的脚本
    GetInst( ScriptSys ).DelScripts();
	// 桢数控制
    static DWORD new_timer = timeGetTime();
    static DWORD old_timer = new_timer;
    new_timer = timeGetTime();
    if (new_timer - old_timer < (DWORD)g_ms)
    {
        WaitForSingleObject(GetCurrentProcess(), g_ms - (new_timer - old_timer));
    }

    old_timer += g_ms; 
    if ((long)new_timer - (long)old_timer > 1000)
    {
        OutputDebugString("warning!!! 1 second not call AI()\n");
        old_timer = timeGetTime();
    }

    //LeaveCriticalSection(&m_CriticalSection);

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MainLoop() End.");
#endif
    return true;
}



void CGame::OutputMsgExpendTimeInfo(const char* pszFileName)
{
    Log4c::Trace(ROOT_MODULE,"MsgExpendTime:");
    char pszInfo[256]="";
    map<long,long>::iterator it = m_mapMsgExpendTime.begin();
    for(;it != m_mapMsgExpendTime.end();it++)
    {
        if((*it).second > 0)
        {
            Log4c::Trace(ROOT_MODULE,"MsgType:%d,ExpendTime;%d",(*it).first,(*it).second);
        }
    }
    m_mapMsgExpendTime.clear();


    Log4c::Trace(ROOT_MODULE,"TimerExpendTime:");
    it = m_mapTimerExpendTime.begin();
    for(;it != m_mapTimerExpendTime.end();it++)
    {
        if((*it).second > 0)
        {
            long lType = (*it).first;

            Log4c::Trace(ROOT_MODULE,"TimerType:%d,TimerlVar1:%d,ExpendTime;%d",(lType&0xFF),(lType>>16) ,(*it).second);
        }
    }
    m_mapTimerExpendTime.clear();
    //  [5/4/2009 chenxianj]
#ifdef _DEBUG

    Log4c::Trace(ROOT_MODULE,"初始化脚本耗时:%d",m_lInitScript);
    m_lInitScript = 0;

    Log4c::Trace(ROOT_MODULE,"技能启动耗时:%d",m_lSkillBegin);
    m_lSkillBegin = 0;

    Log4c::Trace(ROOT_MODULE,"超时脚本耗时:%d",m_lTimeOutScript);
    m_lTimeOutScript = 0;
#endif

    //  [5/4/2009 chenxianj]
}

const char* CGame::GetCountryName(BYTE cCountryType)
{
    switch(cCountryType)
    {
    case 1:
        return "星之国普瑞恩";
    case 2:
        return "海之国波尔菲特";
    case 3:
        return "云之国赛诺特";
    case 4:
        return "月之国罗德莱斯";
    case 5:
        return "天之国摩兰德尔";
    case 6:
        return "风之国曼特菲";
    }
    return "错误的国家ID";
}

CPlayer* CGame::FindPlayer(const char* strName)
{
    if(!strName)	return NULL;
    /*for(itPlayer it=s_mapPlayer.begin(); it!=s_mapPlayer.end(); it++)
    {
    CPlayer* pPlayer = dynamic_cast<CPlayer*>((*it).second);
    if (pPlayer)
    {
    if (stricmp(pPlayer->GetName(), strName)==0)
    {
    return pPlayer;
    }
    }
    }*/
    itSG it = m_PlayerNameToGUID.find(strName);
    if( it != m_PlayerNameToGUID.end() )
    {
        itPlayer itP = s_mapPlayer.find((*it).second);
        if( itP != s_mapPlayer.end() )
            return (*itP).second;
    }
    return NULL;
}

void CGame::AddPlayer(CPlayer* pPlayer)
{
    if(!pPlayer)	return;

    map<CGUID,CPlayer*,guid_compare>::iterator it = s_mapPlayer.find(pPlayer->GetExID());
    if(it != s_mapPlayer.end())
        Log4c::Warn(ROOT_MODULE,FormatText("GS_GAME_2", pPlayer->GetName()));//添加玩家%s时错误，该玩家的实例已经存在!
    s_mapPlayer[pPlayer->GetExID()]=pPlayer;

    m_PlayerNameToGUID[pPlayer->GetName()] = pPlayer->GetExID();

}
void CGame::RemovePlayer(const CGUID& PlayerID)
{
    GetInst(AntiCheat::CPlayerManager).PlayerExit(PlayerID );
    map<CGUID,CPlayer*,guid_compare>::iterator it = s_mapPlayer.find(PlayerID);
    if(it != s_mapPlayer.end())
    {
        CPlayer* pPlayer = (*it).second;
        if(pPlayer)
        {
            // Fox@20081110 删除玩家
            GetInst(CPlayerMan).DelPlayer(pPlayer);
            m_PlayerNameToGUID.erase((*it).second->GetName());

            if(GetGame()->GetWSSocket() > 0)
            {
#ifdef _RUNSTACKINFO1_
                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::OnPlayerExit() FreeObject Start");
#endif
                GameManager::GetInstance()->FreeObject((CBaseObject**)&pPlayer);
            }
            else
            {
                pPlayer->SetSavePartFlag(0x007FFFFF);	
                GetGame()->PushSavePlayer(pPlayer);
            }			
        }
        s_mapPlayer.erase( PlayerID);
    }
}

CMonster* CGame::FindMonster(const CGUID& MonsterID)
{
    CMonster * pMonster=NULL;
    /*
    std::map<long, CServerRegion*>::iterator it=s_mapRegion.begin();
    for ( ; it!=s_mapRegion.end() ; it ++)
    {
    CServerRegion* pRegion=it->second;
    pMonster = pRegion->FindMonsterByID(MonsterID);
    if (pMonster!=NULL)
    {
    break;
    }
    }
    return pMonster;*/

    // 默认 Normal rgn
    CRgnManager::RgnByIDMap& rgnMap = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap();

    CRgnManager::RgnByIDMapItr rit=rgnMap.begin();
    for ( ; rit != rgnMap.end(); rit++)
    {
        CServerRegion* pRegion=rit->second;
        if(pRegion)
        {
            pMonster = pRegion->FindMonsterByID(MonsterID);
            if (pMonster!=NULL)
            {
                break;
            }
        }
    }

    return pMonster;
}

// 查找一个宠物
CPet* CGame::FindPet(const CGUID& guid)
{
    CPet* pet = NULL;
    CRgnManager::RgnByIDMap& rgnMap = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap();

    CRgnManager::RgnByIDMapItr rit=rgnMap.begin();
    for ( ; rit != rgnMap.end(); rit++)
    {
        CServerRegion* pRegion=rit->second;
        if(pRegion)
        {
            pet = pRegion->FindPetByID(guid);
            if (pet!=NULL)
            {
                break;
            }
        }
    }

    return pet;
}

bool CGame::KickPlayer(const CGUID& PlayerGuid)
{
    long lSocketID = CMessage::GetSocketByPlayerID(PlayerGuid);
    s_pNetServer->DisConn(lSocketID);
    return true;
}

void CGame::KickAllPlayer(const CGUID& exceptID)
{
    for( itPlayer itBegin = s_mapPlayer.begin();itBegin != s_mapPlayer.end(); itBegin ++ ) 
    {
        if((*itBegin).first != exceptID)
        {
            GetGame()->KickPlayer( (*itBegin).first);
        }
    }
}

void CGame::AddRegion(eRgnType rgnType, const CGUID& RegionID, CServerRegion* pRegion)
{
    //	s_mapRegion[lRegionID] = pRegion;
    switch(rgnType)
    {
    case RGN_PERSONAL:
        {
            CRgnManager::RgnByIDMap& rgnMap = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap();
            CRgnManager::RgnByIDMapItr itr = rgnMap.find(RegionID);
            if(itr == rgnMap.end())
                rgnMap[RegionID] = pRegion;
        }
        break;
    case RGN_NORMAL:
        {
            CRgnManager::RgnByIDMap& rgnMap = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap();
            CRgnManager::RgnByIDMapItr itr = rgnMap.find(RegionID);
            if(itr == rgnMap.end())
                rgnMap[RegionID] = pRegion;
        }
        break;
    case RGN_PERSONAL_HOUSE:
        {
            CRgnManager::RgnByIDMap& rgnMap = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap();
            CRgnManager::RgnByIDMapItr itr = rgnMap.find(RegionID);
            if(itr == rgnMap.end())
                rgnMap[RegionID] = pRegion;
        }
        break;
    case RGN_TEAM:
        {
            CRgnManager::RgnByIDMap& rgnMap = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap();
            CRgnManager::RgnByIDMapItr itr = rgnMap.find(RegionID);
            if(itr == rgnMap.end())
                rgnMap[RegionID] = pRegion;
        }
        break;
    case RGN_PROXY:
        {
            /*		CRgnManager::ProxyRgnByIDMap* rgnMap = GameManager::GetInstance()->GetRgnManager()->GetProxyRgnByGUIDMap();
            CRgnManager::RgnByIDMapItr itr = rgnMap.find(RegionID);
            if(itr == rgnMap.end())
            rgnMap[RegionID] = pRegion;*/
        }
        break;
    }

}

void CGame::AddProxyRegion(const CGUID& RegionID, CProxyServerRegion* pProxyRegion)
{
    //	s_mapProxyRegion[lRegionID] = pProxyRegion;
    //	CRgnManager::ProxyRgnByIDMap* rgnMap = GameManager::GetInstance()->GetRgnManager()->GetProxyRgnByGUIDMap(RegionID);
    //	if(pProxyRegion)
    //	{
    //		CGUID tGUID;
    //		CGUID::CreateGUID(tGUID);
    //		(*rgnMap)[tGUID] = pProxyRegion;
    //	}

}
CServerRegion* CGame::FindRegionByRgnID(const CGUID& RgnGUID)
{
    CServerRegion* rgn = NULL;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_NORMAL, RgnGUID);
    if(rgn != NULL)
        return rgn;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL, RgnGUID);
    if(rgn != NULL)
        return rgn;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_TEAM, RgnGUID);
    if(rgn != NULL)
        return rgn;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL_HOUSE, RgnGUID);

    return rgn;
}
CServerRegion* CGame::FindRegion(const CGUID& RgnGUID)
{
    /*std::map<long, CServerRegion*>::iterator ite = s_mapRegion.begin();
    for (;ite != s_mapRegion.end(); ++ite)
    {
    if (RegionGuid == ite->second->GetRegionGUID())
    {
    return ite->second;
    }
    }*/
    CServerRegion* rgn = NULL;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_NORMAL, RgnGUID);
    if(rgn != NULL)
        return rgn;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL, RgnGUID);
    if(rgn != NULL)
        return rgn;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_TEAM, RgnGUID);
    if(rgn != NULL)
        return rgn;

    rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RGN_PERSONAL_HOUSE, RgnGUID);
    return rgn;
}
CServerRegion* CGame::FindProxyRegion(const CGUID& RegionID)
{
    //	return (s_mapProxyRegion.find(lRegionID) != s_mapProxyRegion.end()) ? s_mapProxyRegion[lRegionID] : NULL;
    //	CRgnManager::ProxyRgnByIDMap* rgnMap = GameManager::GetInstance()->GetRgnManager()->GetProxyRgnByGUIDMap(RegionID);
    //	// [RGN TEST]
    //	CRgnManager::ProxyRgnByIDMapItr itr = rgnMap->begin();
    //	return itr->second;
    return NULL;
}

CServerRegion* CGame::FindRegion(const char* strName)
{
    /*for(map<long, CServerRegion*>::iterator it=s_mapRegion.begin(); it!=s_mapRegion.end(); it++)
    {
    CServerRegion* pServerRegion = dynamic_cast<CServerRegion*>((*it).second);
    if (pServerRegion)
    {
    if (strcmp(pServerRegion->GetName(), strName)==0)
    {
    return pServerRegion;
    }
    }
    }
    */
    CServerRegion* rgn = NULL;

    for(CRgnManager::RgnByIDMapItr it=GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().begin(); it!=GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end(); it++)
    {
        rgn = dynamic_cast<CServerRegion*>((*it).second);
        if (rgn)
        {
            if (strcmp(rgn->GetName(), strName)==0)
            {
                return rgn;
            }
        }
        rgn = NULL;
    }

    if(rgn == NULL)
    {
        for(CRgnManager::RgnByIDMapItr it1=GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().begin(); it1!=GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end(); it1++)
        {
            rgn = dynamic_cast<CServerRegion*>((*it1).second);
            if (rgn)
            {
                if (strcmp(rgn->GetName(), strName)==0)
                {
                    return rgn;
                }
            }
            rgn = NULL;
        }
    }
    if(rgn == NULL)
    {
        for(CRgnManager::RgnByIDMapItr it1=GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().begin(); it1!=GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end(); it1++)
        {
            rgn = dynamic_cast<CServerRegion*>((*it1).second);
            if (rgn)
            {
                if (strcmp(rgn->GetName(), strName)==0)
                {
                    return rgn;
                }
            }
            rgn = NULL;
        }
    }
    if(rgn == NULL)
    {
        for(CRgnManager::RgnByIDMapItr it1=GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().begin(); it1!=GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end(); it1++)
        {
            rgn = dynamic_cast<CServerRegion*>((*it1).second);
            if (rgn)
            {
                if (strcmp(rgn->GetName(), strName)==0)
                {
                    return rgn;
                }
            }
            rgn = NULL;
        }
    }

    return rgn;
}


CMoveShape* CGame::FindShape(OBJECT_TYPE type, const CGUID& entityID)
{
    switch(type)
    {
    case TYPE_PLAYER:
        return (CMoveShape*)FindPlayer(entityID);
        break;
    case TYPE_MONSTER:
        return (CMoveShape*)FindMonster(entityID);
        break;
    case TYPE_PET:
        return (CMoveShape*)FindPet(entityID);
        break;
    default:
        return NULL;
        break;
    }
    return NULL;
}
//void CGame::SetFunctionFileData(char* pData, long lLen)
//{
//    M_FREE(m_strFunctionList_FileData, m_lFuncFileDataLen);
//    m_strFunctionList_FileData = pData;
//    m_lFuncFileDataLen = lLen;
//    //CScript::LoadFunction(NULL, m_strFunctionList_FileData);
//}
//
//void CGame::SetVariableFileData(char* pData, long lLen)
//{
//    M_FREE(m_strVariableList_FileData, m_lVariableFileDataLen);
//    m_strVariableList_FileData = pData;
//
//    m_lVariableFileDataLen = lLen;
//    //	CScript::LoadVariable(NULL, m_strVariableList_FileData);
//}
//
//void CGame::SetGeneralVariableFileData(BYTE* pByte,long lPos)
//{
//    //CScript::LoadGeneralVariable(pByte, lPos);
//}
//
//void CGame::SetScriptFileData(const char* filename, char* pData, long lLen)
//{
//    if(!filename) return;
//
//    tagScriptDataInfoParam param;
//    param.pInfoData = pData;
//    param.lInfoSize = lLen;
//    tagScriptDataInfo* pScriptDataInfo = OBJ_CREATE_PVOID(tagScriptDataInfo, (void*)&param);
//    map<string, tagScriptDataInfo*>::iterator it = m_mapScript_FileData.find(filename);
//    if (it != m_mapScript_FileData.end())
//    {
//        OBJ_RELEASE(tagScriptDataInfo, it->second);
//        it->second = pScriptDataInfo;
//    }
//    else
//    {
//        m_mapScript_FileData[filename] = pScriptDataInfo;
//    }
//}

//申请共享数据块
BYTE* CGame::AllockShareDB(long lSize)
{
    return m_pDBAllocator->AllockShareDB(lSize);
}
//释放共享数据块
void CGame::FreeShareDB(BYTE* pData)
{
    m_pDBAllocator->FreeShareDB(pData);
}
// 刷新所有怪物的BaseProperty指针
void CGame::RefreashAllMonsterBaseProperty()
{
    // normal region
    for(CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().begin(); it != GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end(); it++)
    {
        CServerRegion* pRegion = it->second;
        if( pRegion )
        {
            BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pRegion, TYPE_MONSTER)
            {
                CMonster* pMonster = dynamic_cast<CMonster*>( pShape );
                if( pMonster )
                {
                    //CMonsterList::tagMonster* p = CMonsterList::GetPropertyByOrginName(pMonster->GetProperty()->strOrginName.c_str());
                    //pMonster->SetBaseProperty(p);
                }
            }
            END_LOOP_REGION_SHAPE_BY_TYPE;
        }
    }

    // Personal Region
    for(CRgnManager::RgnByIDMapItr it1 = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().begin(); it1 != GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end(); it1++)
    {
        CServerRegion* pRegion = it1->second;
        if( pRegion )
        {
            BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pRegion, TYPE_MONSTER)
            {
                CMonster* pMonster = dynamic_cast<CMonster*>( pShape );
                if( pMonster )
                {
                    //CMonsterList::tagMonster* p = CMonsterList::GetPropertyByOrginName(pMonster->GetProperty()->strOrginName.c_str());
                    //pMonster->SetBaseProperty(p);
                }
            }
            END_LOOP_REGION_SHAPE_BY_TYPE;
        }
    }
}

//保存主城信息
void CGame::SaveCityRegion(long lID)
{
    if(lID==0)
    {

    }
}

//发送顶部显示信息到客户端
void CGame::SendTopInfoToClient(long lTopInfoID,const CGUID& PlayerGuid,long lFlag,long lParam,string strInfo)
{
    CMessage msgTopInf(MSG_S2C_OTHER_AddTopInfo);
    msgTopInf.Add(PlayerGuid);
    msgTopInf.Add(lTopInfoID);
    msgTopInf.Add(lFlag);
    msgTopInf.Add(lParam);
    msgTopInf.Add((char*)strInfo.c_str());
    //发送给所有客户端端
    if(PlayerGuid == NULL_GUID)
    {
        msgTopInf.SendAll();
    }
    else
    {
        msgTopInf.SendToPlayer(PlayerGuid,false);
    }
}

bool CGame::CheckInvalidString(string& str,bool bReplace)
{
    return CWordsFilter::GetInstance()->Check(str,bReplace);
}


// 游戏主线程
unsigned __stdcall GameThreadFunc(void* pArguments)
{
    ::CoInitialize(NULL);
#ifndef _DEBUG
    __try
    {
#endif
        CreateGame();
        BOOL ret = GetGame()->Init();
        AppFrame::ToggleMenus();

        while(ret)
        {
            if(AppFrame::IsExit() || !GetGame()->MainLoop() )
            {
                break;
            }
        }

        if(GetGame())
            GetGame()->Release();
        DeleteGame();

#ifndef _DEBUG
    }
    __except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
    {
        AppFrame::ExitThread();
        return 0;
    }
#endif

    AppFrame::ExitThread();
    return 0;
}

// 连接WorldServer的线程、当WorldServer断开时启用，连通时候关闭
unsigned __stdcall ConnectWorldFunc( void* pArguments )
{
    try
    {
        while(true)
        {
            if(bConnectThreadExit)
                break;
            Sleep(4000);

            if(GetGame()->ReConnectWorldServer()==TRUE)
            {
                break;
            }

        }
    }
    catch(...)
    {
        Log4c::Warn(ROOT_MODULE, GetText("GS_GAME_8"));//连接WorldServer的线程出错!
    }
    _endthreadex( 0 );
    return 0;
}

unsigned __stdcall ConnectBSFunc( void* pArguments )
{
    try
    {
        while(true)
        {
            if(bConnectThreadExitBS)
                break;
            Sleep(4000);

            if(GetGame()->ReConnectBSServer()==TRUE)
            {
                break;
            }

        }
    }
    catch(...)
    {
        Log4c::Warn(ROOT_MODULE, GetText("GS_GAME_9"));//连接BillingServer的线程出错!
    }
    _endthreadex( 0 );
    return 0;
}

unsigned __stdcall ConnectBounsServerFunc(void* pArguments)
{
    try
    {
        while(true)
        {
            if(bConnectThreadExitBouns)
            {
                break;
            }

            Sleep(8000);

            if(GetGame()->ReConnectBounsServer())
            {
                break;
            }
        }
    }
    catch(...)
    {
        Log4c::Warn(ROOT_MODULE, GetText("GS_GAME_10"));//连接BounsServer的线程出错!
    }
    _endthreadex(0);
    return 0;
}

unsigned __stdcall ConnectChatServerFunc(void* pArguments)
{
    try
    {
        while (true)
        {
            Sleep(2000);
            if (bConnectThreadExitChatServer)
                break;
            if (GetGame()->ReConnectChatServer())
                break;
        }
    }
    catch(...)
    {
        Log4c::Warn(ROOT_MODULE,"[NET] The thread connects chat server occurs error.");
    }
    _endthreadex(0);
    return 0;
}

void CGame::CleanSequenceMap()
{
    for(map<CGUID,CSequenceString*>::iterator it = GetSequenceMap()->begin(); it!=GetSequenceMap()->end();)
    {
        SAFE_DELETE(it->second);
        it = GetSequenceMap()->erase(it);

    }
}
CSequenceString* CGame::GetSequence(const CGUID& guid)
{
    map<CGUID,CSequenceString*>::iterator it = GetSequenceMap()->find(guid);
    if(it != GetSequenceMap()->end())
    {
        return it->second;
    }
    return NULL;
}

bool *CGame::GetValidateTime(const CGUID& guid)
{
    map<CGUID, bool>::iterator it = GetValidateTimeMap()->find(guid);
    if( it != GetValidateTimeMap()->end() )
    {
        return &it->second;
    }
    return NULL;
}

void CGame::AppendValidateTime(const CGUID& guid, bool enable)
{
    map<CGUID, bool>::iterator it = GetValidateTimeMap()->find(guid);
    if( it != GetValidateTimeMap()->end() )
    {
        //return it->second;
    }
    GetValidateTimeMap()->insert( pair<CGUID,bool>(guid, enable) );
#ifdef _DEBUG
    Log4c::Warn(ROOT_MODULE,FormatText("GS_GAME_3", guid));//添加 玩家 %d 验证时间
#endif
}

void CGame::RemoveValidateTime(const CGUID& guid)
{
    map<CGUID, bool>::iterator it = GetValidateTimeMap()->find(guid);
    if( it != GetValidateTimeMap()->end() )
    {
        GetValidateTimeMap()->erase(guid);
#ifdef _DEBUG
        Log4c::Warn(ROOT_MODULE,FormatText( "GS_GAME_4", guid));//清除 玩家 %d 验证时间
#endif
    }
}

long CGame::GetNumTeams()
{
    return GetOrganiCtrl()->GetTeamNum();
}



/////////////////////////
// 获取游戏日志逻辑接口对象
LogicLogInterface* GetLogicLogInterface()
{
    return GetGame()->GetLogInterface();
}

void CGame::OnLOGSLost()
{
    GetLogicLogInterface()->GetLogClient()->SetSoket(0);
    CMessage::SetLogSocket(0);
    CreateConnectLogThread();
}

void CGame::OnBSLost()
{
}

void CGame::OnBounsServerLost()
{
    m_lBounsSocket = 0;
    CMessage::SetBounsSocket(0);
    Log4c::Warn(ROOT_MODULE, GetText("GS_GAME_11"));//BounsServer失去连接!
    CreateConnectBounsServerThread();
}

//建立和DBServer的连接，作为客户端初始化
bool CGame::StartConnLogS(void)
{
    GetLogicLogInterface()->GetLogClient()->SetSoket(s_pNetServer->Connect(GetSetup()->strLogIP.c_str(), GetSetup()->dwLogPort,NF_LogServer_Client));
    if(GetLogicLogInterface()->GetLogClient()->GetSoket() > 0)
    {
        CMessage::SetLogSocket(GetLogicLogInterface()->GetLogClient()->GetSoket());
        Log4c::Warn(ROOT_MODULE,GetText("GS_GAME_12"));//连接LogServer 成功 !

        CMessage msg(MSG_02LOG_SERVER_LOGIN);
        msg.Add(GetSetup()->dClientNo);
        msg.SendToSocket(GetLogicLogInterface()->GetLogClient()->GetSoket());

        return true;
    }


    return false;
}

void CGame::CreateConnectLogThread(void)
{
    GetLogicLogInterface()->GetLogClient()->bConnectThreadExit=true;
    WaitForSingleObject(GetLogicLogInterface()->GetLogClient()->hConnectThread,INFINITE);
    CloseHandle(GetLogicLogInterface()->GetLogClient()->hConnectThread);

    GetLogicLogInterface()->GetLogClient()->bConnectThreadExit=false;

    unsigned threadID3;
    GetLogicLogInterface()->GetLogClient()->hConnectThread = (HANDLE)_beginthreadex(NULL,0,ConnectLogServerFunc,NULL,0,&threadID3);
}

bool CGame::ReConnectLogServer()
{
    return GetLogicLogInterface()->GetLogClient()->bConnectThreadExit = StartConnLogS();
}

unsigned __stdcall ConnectLogServerFunc( void* pArguments )
{
    while(true)
    {
        if(GetLogicLogInterface()->GetLogClient()->bConnectThreadExit)
            break;
        Sleep(8000);

        if(GetGame()->ReConnectLogServer()==TRUE)
        {
            break;
        }

    }
    _endthreadex( 0 );
    return 0;
}


// 发送保存PHDupRgn 数据
void CGame::SendSavePHDupRgn(const CGUID& ownerGUID)
{
    CPlayer* player = FindPlayer(ownerGUID);
    if(player == NULL) return;

    CGUID RgnGUID = player->GetPersonalHouseRgnGUID();

    CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(RgnGUID);
    if(rgn)
    {
        vector<BYTE> pBA;
        CMessage msg(MSG_S2W_SERVER_SAVEPERSONALHOUSE);

        // OPEntityID:    4byte, 操作实体的ID值
        _AddToByteArray(&pBA, RgnGUID);
        _AddToByteArray(&pBA, rgn->GetExID());	
        _AddToByteArray(&pBA, rgn->GetName());
        _AddToByteArray(&pBA, ownerGUID);
        _AddToByteArray(&pBA, (long)rgn->GetID());
        _AddToByteArray(&pBA, (long)rgn->GetResourceID());
        //rgn->GetVariableList()->AddToByteArray(&pBA);
        VarList::Encode( rgn->GetVariableList(), &pBA );

        CRgnManager::PHGoodsMap& goodsMap = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap();
        CRgnManager::PHGoodsMapItr goodsItr = goodsMap.find(rgn->GetExID());
        if(goodsItr != goodsMap.end())
        {
            _AddToByteArray(&pBA, (long)(*goodsItr).second.size());
            if ((*goodsItr).second.size() != 0)
            {
                for(int i = 0; i < (int)(*goodsItr).second.size(); i++)
                {
                    _AddToByteArray(&pBA, &((*goodsItr).second[i]), sizeof(CRgnManager::PHGoods));
                }
            }
        }
        else
        {
            _AddToByteArray(&pBA, (long)0);
        }

        // 发送给WS
        msg.Add((long)pBA.size());
        if(pBA.size())
            msg.AddEx(&pBA[0], (long)pBA.size());
        msg.SendToSocket(GetGame()->GetWSSocket());
    }
}

void CGame::AppendSaveMsgRegion(CServerRegion* pRegion,CMessage* pSendMsg)
{
    if(pRegion)
    {
        vector<BYTE> vecData;		
        _AddToByteArray(&vecData,(BYTE*)pRegion->GetRegionParam(),(long)sizeof(tagRegionParam));		
        pSendMsg->AddEx(&vecData[0],(long)vecData.size());
    }
}

// 数据分层存储接口
void CGame::AppendSaveMsgProperty(CPlayer* pPlayer, DBWriteSet& setWriteDB)
{
    if(pPlayer)
    {
        // flag
        setWriteDB.AddToByteArray( (BYTE)1);
        // 登录属性----------------BEGIN
        setWriteDB.AddToByteArray( pPlayer->GetName());
        //	等级
        setWriteDB.AddToByteArray( pPlayer->GetLevel());
        //	职业
        setWriteDB.AddToByteArray( pPlayer->GetOccupation() );
        //	性别(0:男 1:女)
        setWriteDB.AddToByteArray(  pPlayer->GetSex() );	
        setWriteDB.AddToByteArray(  pPlayer->GetCountry());
        //	头发
        setWriteDB.AddToByteArray( pPlayer->GetHeadPic() );
        // posX
        setWriteDB.AddToByteArray( (float)pPlayer->GetPosX());
        // posY
        setWriteDB.AddToByteArray( (float)pPlayer->GetPosY());
        //ticket
        setWriteDB.AddToByteArray(pPlayer->GetTicket());

        DWORD dwNull = 0;
        CGoods* pGoods = NULL;
        // ----------------------------
        for(int i=EC_E_HEAD; i<=EC_E_RIDER;i++)
        {
            pGoods = pPlayer->m_cEquipment.GetGoods(i);
            setWriteDB.AddToByteArray( (DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
            setWriteDB.AddToByteArray( (long)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
        }

        setWriteDB.AddToByteArray( (long)pPlayer->GetRegionID() );
        // 登录属性----------------END

        // 详细属性----------------BEGIN
        //重新设置下线时间,避免宕机引起下线时间不准确
        time_t curTime;
        time(&curTime);
        pPlayer->SetLastExitGameTime((DWORD)curTime);
        setWriteDB.AddToByteArray( &pPlayer->GetProperty(), sizeof(tagPlayerProperty));
        if(pPlayer)
            pPlayer->GetPlayerDepot()->CodeSetupToDataBlock(setWriteDB);
        // 详细属性----------------END

        //系统邮件----------------BEGIN		
        map<long,long> mapSysMailList = pPlayer->GetSysMailList();
        setWriteDB.AddToByteArray((long)mapSysMailList.size());
        map<long,long>::iterator mit=mapSysMailList.begin();
        for (; mit!=mapSysMailList.end(); ++mit)
        {
            setWriteDB.AddToByteArray((*mit).first);
            setWriteDB.AddToByteArray((*mit).second);			
        }
        //系统邮件----------------END

        //! 勋章容器
        pPlayer->m_MedalContainer.CodeToDataBlock(setWriteDB);

    }
    else
    {
        // flag
        setWriteDB.AddToByteArray( (BYTE)0);
    }
}
void CGame::AppendSaveMsgWalletGoods(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->GetPackage()->GetWallet().Serialize(setWriteDB);	
}

void CGame::AppendSaveMsgSilverGoods(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->GetPackage()->GetSilverWallet().Serialize(setWriteDB);
}
void CGame::AppendSaveMsgDepotGold(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;

    char outStr[128] = {0};
    CGoodsContainer *pContainer = player->GetPlayerDepot()->FindContainer(eDCT_Gold, true);
    if(NULL == pContainer)
    {
        Log4c::Warn(ROOT_MODULE, "AppendSaveMsgDepotGold:Goods's Container is NULL!");
        setWriteDB.AddToByteArray( (long)0); // crc32
        return;
    }
    setWriteDB.AddToByteArray( (long)1); // flag
    pContainer->Serialize(setWriteDB);	
}
void CGame::AppendSaveMsgDepotSilver(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;

    char outStr[128] = {0};
    CGoodsContainer *pContainer = player->GetPlayerDepot()->FindContainer(eDCT_Silver, true);
    if(NULL == pContainer)
    {
        Log4c::Warn(ROOT_MODULE,FormatText("AppendSaveMsgDepotSilver:Goods's Container is NULL!"));
        setWriteDB.AddToByteArray( (long)0); // flag
        return;
    }
    setWriteDB.AddToByteArray( (long)1); // flag
    pContainer->Serialize(setWriteDB);
}
void CGame::AppendSaveMsgDepotGoods(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;

    char outStr[128] = {0};
    CGoodsContainer *pContainer = player->GetPlayerDepot()->FindContainer(eDCT_Primary, true);
    if(NULL == pContainer)
    {
        Log4c::Warn(ROOT_MODULE,FormatText("DecodeDepotGoodsMsg:Goods's Container is NULL!"));
        setWriteDB.AddToByteArray((long)0); // flag
        return;
    }

    setWriteDB.AddToByteArray((long)1); // flag
    pContainer->Serialize(setWriteDB);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGame::AppendSaveMsgSubDepot(long packID, CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    //子背包序列化
    player->SerializeSubDepot(setWriteDB, packID + eDCT_Secondary1);
}
void CGame::AppendSaveMsgSpriteSystem(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->m_SpriteSystem.CodeToDataBlock(setWriteDB);
}

void CGame::AppendSaveMsgEquipmentGoods(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->m_cEquipment.Serialize(setWriteDB);
}
void CGame::AppendSaveMsgPackGoods(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->getOriginPack()->Serialize(setWriteDB);
}
void CGame::AppendSaveMsgSubPackGoods(long packID, CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    //子背包序列化
    player->GetPackage()->SerializeSubpack(setWriteDB, (CS2CContainerObjectMove::PLAYER_EXTEND_ID)(packID+CS2CContainerObjectMove::PEI_PACK1));
}
void CGame::AppendSaveMsgVarList(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    //player->GetVariableList()->CodeToDataBlock(setWriteDB);
    VarList::Encode( player->GetVariableList(), setWriteDB );
}
void CGame::AppendSaveMsgQuestList(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->CodeQuestDataToDataBlock(setWriteDB);
}

void CGame::AppendSaveMsgMerQuestList(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->CodeMerQuestDataToDataBlock(setWriteDB);
}


void CGame::AppendSaveMsgSkillList(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    //skills id field
    setWriteDB.AddToByteArray((long)player->GetNumSkills());
    CPlayer::itSkill it = player->GetSkillMap().begin();
    for(;it!=player->GetSkillMap().end();++it)
    {
        setWriteDB.AddToByteArray( static_cast<DWORD>(it->second.dwID) );
        setWriteDB.AddToByteArray( static_cast<DWORD>(it->second.wLevel) );
        setWriteDB.AddToByteArray( static_cast<DWORD>(it->second.wCooldown) );
    }
}

void CGame::AppendSaveMsgMailList(CPlayer* player, DBWriteSet& setWriteDB)
{

    //if(player)
    //{
    //	_AddToByteArray(&pBA, (long)player->GetMapMail().size());//邮件个数
    //	map<CGUID,CMail*>::iterator it=player->GetMapMail().begin();
    //	for (;it!=player->GetMapMail().end();++it)
    //	{
    //		(*it).second->AddToByteArrayForWS(pBA,player);
    //	}
    //}
}

void CGame::AppendSaveMsgStateList(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->CodeBuffToDataBlock(setWriteDB);
}
void CGame::AppendSaveMsgDeputyOccuList(CPlayer* player, DBWriteSet& setWriteDB)
{
    if(!player) return;
    player->CodeDOccuToDataBlock(setWriteDB);
}

// 添加或者修改一个玩家要保存的属性枚举值
void CGame::UpdateSavePlayer(const CGUID& playerID, SAVE_DETAIL_ATTR_TYPE type)
{
    CPlayer* player = GetGame()->FindPlayer(playerID);
    if(player)
        UpdateSavePlayer(player, type);
}


void CGame::UpdatePlayerPack(CPlayer* pPlayer,DWORD dwContainerId,DWORD dwPos)
{
    if(!pPlayer)
        return;
    switch((CS2CContainerObjectMove::PLAYER_EXTEND_ID)dwContainerId)
    {
    case CS2CContainerObjectMove::PEI_PACKET://普通背包
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRPACKGOODS);
        break;
    case CS2CContainerObjectMove::PEI_EQUIPMENT://装备栏
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTREQUIPMENTGOODS);
        break;
    case CS2CContainerObjectMove::PEI_WALLET://金钱
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRWALLETGOODS);
        break;
    case CS2CContainerObjectMove::PEI_SILVERWALLET://银币
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSILVERGOODS);
        break;
    case CS2CContainerObjectMove::PEI_PACK:
        {
            switch(dwPos)
            {
            case CS2CContainerObjectMove::PEI_PACK1://子背包栏位1
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS1);
                break;
            case CS2CContainerObjectMove::PEI_PACK2://子背包栏位2
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS2);
                break;
            case CS2CContainerObjectMove::PEI_PACK3://子背包栏位3
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS3);
                break;
            case CS2CContainerObjectMove::PEI_PACK4://子背包栏位4
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS4);
                break;
            case CS2CContainerObjectMove::PEI_PACK5://子背包栏位5
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS5);
                break;
            }
        }
        break;
    case CS2CContainerObjectMove::PEI_PACK1://子背包栏位1
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS1);
        break;
    case CS2CContainerObjectMove::PEI_PACK2://子背包栏位2
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS2);
        break;
    case CS2CContainerObjectMove::PEI_PACK3://子背包栏位3
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS3);
        break;
    case CS2CContainerObjectMove::PEI_PACK4://子背包栏位4
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS4);
        break;
    case CS2CContainerObjectMove::PEI_PACK5://子背包栏位5
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBPACKGOODS5);
        break;
    case eDCT_Gold:
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRPROPERTY);
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRDEPOTGOLD);
        break;
    case eDCT_Silver:
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRPROPERTY);
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRDEPOTSILVER);
        break;
    case eDCT_Primary:
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRPROPERTY);
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRPRIMARY);
        break;
    case eDCT_Secondary:
        {
            switch(dwPos)
            {
            case CS2CContainerObjectMove::PEI_PACK1://子背包栏位1
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT1);
                break;
            case CS2CContainerObjectMove::PEI_PACK2://子背包栏位2
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT2);
                break;
            case CS2CContainerObjectMove::PEI_PACK3://子背包栏位3
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT3);
                break;
            case CS2CContainerObjectMove::PEI_PACK4://子背包栏位4
                UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT4);
                break;
            }
        }
        break;
    case eDCT_Secondary1:						
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT1);
        break;
    case eDCT_Secondary2:						
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT2);
        break;
    case eDCT_Secondary3:						
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT3);
        break;
    case eDCT_Secondary4:						
        UpdateSavePlayer(pPlayer, SAVE_DETAIL_ATTRSUBDEPOT4);
        break;
        // 商业背包
    case CS2CContainerObjectMove::PEI_BUSSINESPACK:
    case CS2CContainerObjectMove::PEI_BUSSINESPACKFIELD:
        UpdateSavePlayer( pPlayer, SAVE_DETAIL_ATTRBUSINESSPACK );
        break;
    }
}




// 添加或者修改一个玩家要保存的属性枚举值
void CGame::UpdateSavePlayer(CPlayer* player, SAVE_DETAIL_ATTR_TYPE type)
{
    if(player)
    {
        DWORD flag = player->GetSavePartFlag();
        flag = flag | type;
        player->SetSavePartFlag(flag);
    }
}
//
//CMailManager *CGame::GetMailManager()
//{
//	return m_pMailManager;
//}

void CGame::SendSaveRegionDetailMsg()
{
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SendSaveRegionDetailMsg() Start.");
#endif
    char szGUID[128];
    //GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap()
    set<CServerRegion*>::iterator it=m_setSaveRegion.begin();
    for(;it!=m_setSaveRegion.end();it++)
    {		
        CServerRegion* pRegion=*it;
        CServerRegion* pServerRegion=GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, pRegion->GetID());
        if(pServerRegion)
        {
            char pszSaveInfo[512]="";

            CMessage msg(MSG_S2W_REGION_DETAIL_SAVE_PROPERTY);
            msg.Add(pServerRegion->GetExID());
            pServerRegion->GetExID().tostring(szGUID);
            AppendSaveMsgRegion(pServerRegion,&msg);
            msg.SendToSocket(GetWSSocket());
        }			
    }
    m_setSaveRegion.clear();
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SendSaveRegionDetailMsg() End.");
#endif
}

//void CGame::SendNotifyMessage( char* szText,const CGUID& playerGuid, DWORD dwColor, DWORD dwBkColor,eNOTIFY_POS ePos)
//{
//    if( szText )
//    {
//        CMessage msg( MSG_S2C_OTHER_ADDINFO );
//        msg.Add( dwColor );
//        msg.Add( dwBkColor );
//        msg.Add((BYTE)ePos);
//        msg.Add( szText );
//        msg.SendToPlayer(playerGuid,false);
//    }
//}

//void CGame::SendNotifyMessage( char* szText,CServerRegion* pRegion, DWORD dwColor, DWORD dwBkColor,eNOTIFY_POS ePos)
//{
//    if( szText && pRegion)
//    {
//        CMessage msg( MSG_S2C_OTHER_ADDINFO );
//        msg.Add( dwColor );
//        msg.Add( dwBkColor );
//        msg.Add((BYTE)ePos);
//        msg.Add( szText );
//        msg.SendToRegion(pRegion);
//    }
//}
void CGame::SendNotifyMessage( char* szText,CServerRegion* pRegion,eNOTIFY_POS ePos)
{
	if( szText && pRegion)
	{
		CMessage msg( MSG_S2C_OTHER_ADDINFO );
		msg.Add((BYTE)ePos);
		msg.Add( szText );
		msg.SendToRegion(pRegion);
	}

}
void CGame::SendNotifyMessage(char *szText, const CGUID &playerGuid, eNOTIFY_POS ePos)
{
	if(szText)
	{
		CMessage msg( MSG_S2C_OTHER_ADDINFO );
		msg.Add((BYTE)ePos);
		msg.Add( szText );
		msg.SendToPlayer(playerGuid,false);
	}
}
//发送消息到所有玩家
void CGame::SendMsgToAllPlayer(CMessage *pMsg)
{
    for( itPlayer it = s_mapPlayer.begin();it != s_mapPlayer.end(); ++it)
    {	
        pMsg->SendToPlayer(it->second->GetExID());
    }
}
//发送消息到同国家的玩家
void CGame::SendMsgToSameCountryPlayer(CMessage *pMsg,BYTE btCountry)
{
    for( itPlayer it = s_mapPlayer.begin();it != s_mapPlayer.end(); ++it)
    {
        if( it->second && it->second->GetCountry() == btCountry)
        {
            pMsg->SendToPlayer(it->second->GetExID());
        }
    }
}
//发送消息到同阵营国家的玩家
void CGame::SendMsgToSameCampPlayer(CMessage *pMsg,BYTE btCountry)
{
    //! 得到一个阵营的所有国家ID
    list<BYTE> UnionCountryIDList;
    GetPKSys()->GetCountryList(btCountry, UnionCountryIDList);

    for( itPlayer it = s_mapPlayer.begin();it != s_mapPlayer.end(); ++it)
    {
        for (list<BYTE>::iterator ite = UnionCountryIDList.begin(); ite != UnionCountryIDList.end(); ++ite)
        {
            if (it->second && it->second->GetCountry() == *ite)
            {
                pMsg->SendToPlayer(it->second->GetExID());
                break;
            }
        }
    }
}

//查询禁言玩家
void CGame::QuerySilencePlayers(CGUID& GmID)
{
    size_t lSize = 0;
    /**## 首先准备缓冲区大小 */
    for( itPlayer itBegin = s_mapPlayer.begin();itBegin != s_mapPlayer.end(); itBegin ++ )
    {
        CPlayer* pPlayer = (*itBegin).second;
        if( pPlayer && pPlayer->IsInSilence() )
        {
            lSize += strlen( pPlayer->GetName() );
            lSize += 2; /**## 为";"号保留一个位置 */
        }
    }

    /**## 判断是否有人被禁言 */
    /**## MSG_S2W_GM_ANSWER_QUERY_SILENCE_PLAYERS 格式 */
    /**## long lGMID, long size, char* pList */
    CMessage msg( MSG_S2W_GM_ANSWER_QUERY_SILENCE_PLAYERS );
    if( lSize )
    {
        char* pText = (char*)M_ALLOC((ulong)(sizeof(char)*(lSize + 64)));
        if( pText )
        {
            ZeroMemory( pText, lSize + 64 );
            msg.Add( GmID );
            msg.Add( static_cast<long>(lSize + 64) );
            for( itPlayer itBegin = s_mapPlayer.begin();
                itBegin != s_mapPlayer.end(); itBegin ++ )
            {
                if( itBegin -> second && itBegin -> second -> IsInSilence() )
                {
                    strcat( pText, itBegin -> second -> GetName() );
                    strcat( pText, "；" );
                }
            }
            msg.Add( pText );
            msg.Send();

            /**## 删除分配的资源 */
            M_FREE(pText, (ulong)(sizeof(char)*(lSize + 64)));
        }
        else 
        {
            msg.Add( GmID );
            msg.Add( static_cast<long>(24) );
            msg.Add( "资源不足，无法查询。" );
            msg.Send();
        }
    }
    else 
    {
        msg.Add( GmID );
        msg.Add( static_cast<long>(24) );
        msg.Add( "无禁言玩家。" );
        msg.Send();
    }
}

void CGame::AddSaveDetailPartToByteArray(const CGUID& guid, DWORD flag, CPlayer* player)
{
    if(!player) return;

    char szGUID[128];
    CMessage msg(MSG_S2W_PLAYER_DETAIL_SAVE_PROPERTY);
    msg.Add(guid); // player guid

    guid.tostring(szGUID);


#ifdef _RUNSTACKINFO3_
    char pszStackInfo[1024]="";
    _snprintf(pszStackInfo,1024,"AddSaveDetailPartToByteArray(PlayerName:%s,guid:%s,saveflat:%d) Start.",player->GetName(),szGUID,flag);
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStackInfo);
#endif

    flag = flag|SAVE_DETAIL_ATTRVARLIST;
    flag = flag|SAVE_DETAIL_ATTRQUESTLIST;

    msg.Add(flag); // flag

    DBWriteSet setWriteDB;
    msg.GetDBWriteSet(setWriteDB);
    // 组装消息
    if(flag & SAVE_DETAIL_ATTRPROPERTY)// 基本属性:金钱,等级...等
    {
        AppendSaveMsgProperty(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTREQUIPMENTGOODS)// 装备物品
    {
        AppendSaveMsgEquipmentGoods(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRPACKGOODS)// 主背包物品
    {
        AppendSaveMsgPackGoods(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBPACKGOODS1)// 子背包1物品
    {
        AppendSaveMsgSubPackGoods(0, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBPACKGOODS2)// 子背包2物品
    {
        AppendSaveMsgSubPackGoods(1, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBPACKGOODS3)// 子背包3物品
    {	
        AppendSaveMsgSubPackGoods(2, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBPACKGOODS4)// 子背包4物品
    {	
        AppendSaveMsgSubPackGoods(3, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBPACKGOODS5)// 子背包5物品
    {
        AppendSaveMsgSubPackGoods(4, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRWALLETGOODS)// 金币钱包物品
    {
        AppendSaveMsgWalletGoods(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSILVERGOODS)// 银币钱包物品
    {
        AppendSaveMsgSilverGoods(player, setWriteDB);
    }

    if(flag & SAVE_DETAIL_ATTRDEPOTGOLD)// 黄金仓库
    {
        AppendSaveMsgDepotGold(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRDEPOTSILVER)// 银币仓库
    {
        AppendSaveMsgDepotSilver(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRPRIMARY)// 主仓库
    {
        AppendSaveMsgDepotGoods(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBDEPOT1)// 子仓库1
    {
        AppendSaveMsgSubDepot(0, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBDEPOT2)// 子仓库2
    {
        AppendSaveMsgSubDepot(1, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBDEPOT3)// 子仓库3
    {
        AppendSaveMsgSubDepot(2, player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSUBDEPOT4)// 子仓库4
    {
        AppendSaveMsgSubDepot(3, player, setWriteDB);
    }
    if( flag & SAVE_DETAIL_ATTRBUSINESSPACK ) // 商业背包
    {
        player->GetBusinessContainer().EncodeToSave( setWriteDB );
    }
    if(flag & SAVE_DETAIL_ATTRVARLIST)// 变量列表
    {
        AppendSaveMsgVarList(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRQUESTLIST)// 任务列表
    {	
        AppendSaveMsgQuestList(player, setWriteDB);
    }

    if(flag & SAVE_DETAIL_ATTRSKILLLIST)// 技能列表
    {
        AppendSaveMsgSkillList(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRSTATELIST)// 状态列表
    {	
        AppendSaveMsgStateList(player, setWriteDB);
    }
    if(flag & SAVE_DETAIL_ATTRPROFESSION)// 副职业
    {	
        AppendSaveMsgDeputyOccuList(player, setWriteDB);
    }

    if(flag & SAVE_DETAIL_ATTRMERQUESTLIST)// 俑兵任务列表
    {	
        AppendSaveMsgMerQuestList(player, setWriteDB);
    }

    if( flag & SAVE_DETAIL_PETS )			// 宠物列表
    {
        GetInst(CPetCtrl).CodeToDataBlock(setWriteDB, player);
    }

    msg.SendToSocket(GetWSSocket());
    // 清除标志位
    player->SetSavePartFlag((DWORD)0);

#ifdef _RUNSTACKINFO3_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"AddSaveDetailPartToByteArray() End.");
#endif
}
void CGame::ClearSavePlayer(const CGUID& guid)
{
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::ClearSavePlayer() Start.");
#endif

    map<CGUID, CPlayer*>::iterator setItr = m_mapSavePlayers.find(guid);
    if( setItr != m_mapSavePlayers.end() )
    {
        GameManager::GetInstance()->FreeObject((CBaseObject**)&(setItr->second));
        setItr = m_mapSavePlayers.erase(setItr);
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::ClearSavePlayer() End.");
#endif
}
// 处理玩家保存,发送消息
void CGame::SendQueuePlayerOperMsg(void)
{
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SendSaveDetailPartMsg() Start.");
#endif

    if(GetWSSocket() > 0)
    {
        itPlayer spItr = s_mapPlayer.begin();
        while( spItr != s_mapPlayer.end() )
        {
            if( NULL == spItr->second )
            {
                spItr++;
                continue;
            }
            DWORD flag = spItr->second->GetSavePartFlag();
            if( 0 == flag ) // 表示切换服务器清除操作
            {
                spItr++;
                continue;
            }
            AddSaveDetailPartToByteArray(spItr->first, flag, spItr->second);
            spItr++;
        }
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SendSaveDetailPartMsg() End.");
#endif
}
void CGame::ClearChangeRgnPlayerVec(void)
{
    itCRP itr = m_ChangeRgnPlayers.begin();
    for(; itr != m_ChangeRgnPlayers.end(); ++itr)
    {
        ////场景不在此服务器上，交换服务器
        OnPlayerExit( (*itr).first, TRUE);

        //发送切换场景服务器消息
        CPlayer* pPlayer = GetGame()->FindPlayer((*itr).first);
        if(pPlayer)
        {
            tagPCR& ChangePlayer = (*itr).second;
            CMessage msg( MSG_S2W_SERVER_CHANGEREGION);
            msg.Add(ChangePlayer.lType);
            msg.Add(ChangePlayer.ltemplateID); // 普通场景用模板ID可以查找到GUID
            msg.Add(pPlayer->GetExID());
            msg.Add(ChangePlayer.RegionID);
            msg.Add(ChangePlayer.lX);
            msg.Add(ChangePlayer.lY);
            msg.Add(ChangePlayer.lDir);
            msg.Add(ChangePlayer.lUseGoods);
            msg.Add(ChangePlayer.lRange);
            DBWriteSet setWriteDataDB;
            msg.GetDBWriteSet(setWriteDataDB);
            pPlayer->CodeToDataBlock(setWriteDataDB);
            msg.Send();
        }

        RemovePlayer((*itr).first);
    }
    m_ChangeRgnPlayers.clear();
}

void CGame::PushChangeRgnPlayer(const CGUID& playerID,tagPCR& ChangeRegion)
{
    if(playerID != NULL_GUID)
    {
        m_ChangeRgnPlayers[playerID] = ChangeRegion;
    }
}

void CGame::PushDelDupRgn(CServerRegion* pRgn)
{
    if(pRgn)
    {
        map<CGUID, CServerRegion*>::iterator itr = m_mapDelDupRgn.find(pRgn->GetExID());
        if(itr == m_mapDelDupRgn.end())
        {
            m_mapDelDupRgn[pRgn->GetExID()] = pRgn;
        }
        else
        {
            char szGuid[128];
            pRgn->GetExID().tostring(szGuid);
            Log4c::Warn(ROOT_MODULE,FormatText( "PushDelDupRgn() rgn[%s] is exist!", szGuid));
        }
    }
}
void CGame::ClearDelDupRgn(void)
{
    map<CGUID, CServerRegion*>::iterator itr = m_mapDelDupRgn.begin();
    for(; itr != m_mapDelDupRgn.end(); itr++)
    {
        if(itr->second)
        {
            // 先将该场景的玩家强制下线
            hashsetGUID tmpPlayerSet;
            itr->second->FindAllPlayer(tmpPlayerSet);
            hashsetGUID::iterator pitr = tmpPlayerSet.begin();
            for(; pitr != tmpPlayerSet.end(); pitr++)
            {
                // 输出异常
                char szGuid[128];
                (*pitr).tostring(szGuid);
                Log4c::Warn(ROOT_MODULE,FormatText("GS_GAME_5", szGuid, itr->second->GetID()));//ClearDelDupRgn() 玩家[%s]从被清除副本[%d]切换场景时异常
                CPlayer* pPlayer = GetGame()->FindPlayer((*pitr));
                // 将所在场景指针设置为空
                if(pPlayer)
                    pPlayer->SetFather(NULL);
                GetGame()->KickPlayer((*pitr));
            }
        }
        // 删除该场景
        OBJ_RELEASE(CServerRegion, itr->second);
    }
    m_mapDelDupRgn.clear();
}
void DoSaveData()
{
    // 将CDBEntity对象发送到WS

}
// 存盘线程
//unsigned __stdcall SaveThreadFunc(void* pArguments)
//{
//    //	EnterCriticalSection(&g_CriticalSectionSaveThread);
//    //	::CoInitialize(NULL);
//    Log4c::Trace(ROOT_MODULE,GetText("SaveThread Starting..."));
//
//#ifndef _DEBUG
//    __try
//    {
//#endif
//
//        DoSaveData();
//
//#ifndef _DEBUG
//    }
//    __except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
//    {
//      Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "GS_GAME_6");//存盘线程出现错误，请查看最新的错误报告文件
//    }
//#endif
//
//    //	::CoUninitialize();
//    //	LeaveCriticalSection(&g_CriticalSectionSaveThread);
//
//    _endthreadex( 0 );
//    return 0;
//}

void CGame::PushSavePlayer(CPlayer* pPlayer)
{
    if(!pPlayer) return;
    map<CGUID, CPlayer*>::iterator itr = m_mapSavePlayers.find(pPlayer->GetExID());
    if(itr == m_mapSavePlayers.end())
        m_mapSavePlayers[pPlayer->GetExID()] = pPlayer;
    else
    {
        if(itr->second != pPlayer)
        {
            delete itr->second;
            m_mapSavePlayers.erase(itr);
            m_mapSavePlayers[pPlayer->GetExID()] = pPlayer;
        }
        else
        {
            char szGuid[128];
            pPlayer->GetExID().tostring(szGuid);
            Log4c::Warn(ROOT_MODULE,FormatText("PushSavePlayer() player[%s] is Exist!", szGuid));
        }
    }
}
CPlayer* CGame::FindSavePlayer(const CGUID& guid)
{
    map<CGUID, CPlayer*>::iterator itr = m_mapSavePlayers.find(guid);
    if(itr != m_mapSavePlayers.end())
        return itr->second;
    return NULL;
}

void CGame::LogRegion(void)
{
    for(CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().begin(); it != GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end(); it++)
        GetLogicLogInterface()->logT800_player_num_in_region(GetGame()->GetIDIndex(), it->second);

    //! 副本相关人数的日志记录
    for(CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().begin(); it != GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end(); it++)
        GetLogicLogInterface()->logT800_player_num_in_region(GetGame()->GetIDIndex(), it->second);

    for(CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().begin(); it != GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end(); it++)
        GetLogicLogInterface()->logT800_player_num_in_region(GetGame()->GetIDIndex(), it->second);

    for(CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().begin(); it != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end(); it++)
        GetLogicLogInterface()->logT800_player_num_in_region(GetGame()->GetIDIndex(), it->second);
}

void CGame::OutPlayerAndRegionInfo(const char* pszFileName)
{
    if(pszFileName)
    {
        //char str[256];
        //PutStringToFile(pszFileName,"OutPlayerAndRegionInfo:");
        //_snprintf(str, 256, "DelaySavePlayer Num:%d",m_mapSavePlayers.size());
        Log4c::Trace(ROOT_MODULE,"OutPlayerAndRegionInfo:");
        //PutStringToFile(pszFileName,str);
        Log4c::Trace(ROOT_MODULE,"DelaySavePlayer Num:%d",m_mapSavePlayers.size());

       // _snprintf(str, 256, "DeleteDupRgn Num:%d",m_mapDelDupRgn.size());
       // PutStringToFile(pszFileName,str);
        Log4c::Trace(ROOT_MODULE,"DeleteDupRgn Num:%d",m_mapDelDupRgn.size());
    }
}

//! 响应新的一天开始了
void CGame::OnNewDayStart(DWORD dwTime)
{
    ResetStarDay(dwTime);

    //! 重置玩家CP
    itPlayer ite = s_mapPlayer.begin();
    for (; ite != s_mapPlayer.end(); ++ite)
    {
        if(NULL != ite->second)
            ite->second->ResetActive(dwTime);
    }
}

//! 计算星座日
void CGame::ResetStarDay(DWORD dwTime)
{
    tm tm2007;
    tm2007.tm_sec	= 59;/* seconds after the minute - [0,59] */
    tm2007.tm_min	= 59;/* minutes after the hour - [0,59] */
    tm2007.tm_hour	= 23;/* hours since midnight - [0,23] */
    tm2007.tm_mday	= 1;/* day of the month - [1,31] */
    tm2007.tm_mon	= 3;/* months since January - [0,11] */
    tm2007.tm_year	= 2007 - 1900;/* years since 1900 */

    long lDiffSec = (long)(dwTime - mktime(&tm2007));
    long lDiffDay = lDiffSec / (60 * 60 * 24) + 1 + 7/*为了跟策划的计算结果统一，+7*/;

    static eConstellation arrConstellation[14] = 
    {
        CONST_No,				//无
        CONST_Aries,			//牧羊座
        CONST_Taurus,			//金牛座
        CONST_Gemini,			//双子座
        CONST_Cancer,			//巨蟹座
        CONST_Leo,				//狮子座
        CONST_Virgo,			//处女座
        CONST_No,
        CONST_Libra,			//天秤座
        CONST_Scorpio,			//天蝎座
        CONST_Sagittarius,		//射手座
        CONST_Capricorn,		//摩羯座
        CONST_Aquarius,			//水瓶座
        CONST_Pisces,			//双鱼座
    };

    m_Constellation = arrConstellation[lDiffDay % 14];
}

//! 得到星座日
eConstellation CGame::GetStarDay(void)
{
    return m_Constellation;
}

// 增加奖励描述信息
void CGame::AddBounsDes(tagBounsDes *pBD)
{
    if(pBD == NULL)
    {
        return;
    }

    map<DWORD, tagBounsDes*>::iterator itor = m_mapBounsDes.find(pBD->dwBounsID);
    if(itor != m_mapBounsDes.end())
    {
        // 已经存在有奖励描述了
        OBJ_RELEASE(tagBounsDes, itor->second);
        m_mapBounsDes.erase(itor);
        //	OBJ_RELEASE(tagBounsDes, pBD);
        //	return;
    }

    m_mapBounsDes[pBD->dwBounsID] = pBD;
}

tagBounsDes* CGame::FindBounsDes(DWORD dwBounsID)
{
    map<DWORD, tagBounsDes*>::iterator itor = m_mapBounsDes.find(dwBounsID);
    if(itor != m_mapBounsDes.end())
    {
        return itor->second;
    }
    return NULL;
}

void CGame::ClearBounsDes()
{
    for(map<DWORD, tagBounsDes*>::iterator itor = GetBounsDes()->begin(); itor!=GetBounsDes()->end();)
    {
        OBJ_RELEASE(tagBounsDes, itor->second);
        itor = GetBounsDes()->erase(itor);
    }
}