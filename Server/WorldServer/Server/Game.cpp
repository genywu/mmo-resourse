#include "StdAfx.h"

#include "Game.h"

#include "../App/AH/OrderMgr.h"
#include "../App/AntiCheat/Configer.h"
#include "../App/AntiCheat/FileListManager.h"
#include "../App/AntiCheat/QuestionManager.h"
#include "../App/Business/BusinessManager.h"
#include "../App/CardsDealingSystem/CardsHandler.h"
#include "../App/Country/Country.h"
#include "../App/Country/CountryHandler.h"
#include "../App/Country/CountryParam.h"
#include "../App/DBEntity/Account.h"
#include "../App/DBEntity/EntityGroup.h"
#include "../App/DBEntity/EntityManager.h"
#include "../App/FBusiness/FBConfiger.h" 
#include "../App/GlobalRgnManager.h"
#include "../App/Goods/Goods.h"
#include "../App/Goods/GoodsFactory.h"
#include "../App/Goods/IncrementShopManager.h"
#include "../App/Linkman/LinkmanSystem.h"
#include "../App/Message/MsgOutTimeDef.h"
#include "../App/NewSkill/NewSkillFactory.h"
#include "../App/OrganizingSystem/OrganizingCtrl.h"
#include "../App/OrganizingSystem/OrganizingParam.h"
#include "../App/OrganizingSystem/union.h"
#include "../App/OrganizingSystem/Faction.h"
#include "../App/OrganizingSystem/Organizing.h"
#include "../App/Pet/PetCtrl.h"
#include "../App/Player.h"
#include "../App/PlayerDepot.h"
#include "../App/PlayerMan/PlayerMan.h"
#include "../App/Rank/RankManager.h"
#include "../App/Script/Variablelist.h"
#include "../App/Session/SessionFactory.h"
#include "../App/SMClient.h"
#include "../App/SpriteSystem.h"
#include "../App/WorldRegion.h"
#include "../App/Session/SessionFactory.h"
#include "../App/Session/Session.h"
#include "../Net/NetHandler.h"

#include "../../../public/Common/NetSessionManager.h"
#include "../../../Public/Common/DupliRegionSetup.h"
#include "../../../Public/Common/StudySkillCondition.h"
#include "../../../Public/Common/OccupSkillList.h"
#include "../../../Public/Common/CreditSystem.h"
#include "../../../Public/DefSetup.h"
#include "../../../Public/ServerPublic/Server/Common/WordsFilter.h"
#include "../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#include "../../../Public/ServerPublic/Server/Common/AppTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void RefreshInfo();

const DWORD g_ms = 40;

// 调试用
DWORD g_lCurTime = 0;
DWORD g_lLSMessageTime = 0;
DWORD g_lGSMessageTime = 0;
DWORD g_lCSMessageTime = 0;

char g_rootPath[MAX_PATH];
HANDLE g_hGameThreadExitEvent = NULL;        // 主线程退出事件
bool g_bGameThreadExit = false;              // 是否退出主线程

HANDLE hLSConnectThread=NULL;			     // 连接线程句柄 Login
bool bLSConnectThreadExit = false;		     // 是否退出连接线程

HANDLE g_hWriteLogThread = NULL;			 // 日志线程
bool volatile g_bWriteLogThreadExit = false; // 是否退出日志线程

vector<HANDLE> g_vPlayerLoadThreads;		 // 玩家读数据库线程
bool volatile g_bPlayerLoadThreadsExit = false;

HANDLE g_hSavingThread = NULL;               // 存盘线程句柄
CSessionFactory* CGame::m_pSessionFactory = NULL;

bool g_bSendSaveMsgNow = false;
bool g_bSaveNowData = false;

void ShowSaveInfo(LPCSTR szInfo, ...)
{
    char str[256];
    va_list va;
    va_start(va,szInfo);
    vsprintf(str,szInfo,va);
    va_end(va);

    Log4c::Trace(ROOT_MODULE,str);
}

// 刷新服务器信息
void RefreshInfo()
{
    static char info[10240];

    static long maxPlayerNum = 0;
    static long maxAvailableVM = 0;
    static long maxDupRgnNum = 0;

    // 网络流量分析
    static long maxNetW2L  = 0;
    static long maxNetL2W  = 0;
    static long maxNetW2DB  = 0;
    static long maxNetDB2W  = 0;
    static long maxNetW2S  = 0;
    static long maxNetS2W  = 0;
    long curPlayerNum = GetGame().GetMapPlayerCount();
    long curDupRgnNum = (long)GetGame().GetGlobalRgnManager()->GetRgnMap().size();
    MEMORYSTATUS vmStatus;
    GlobalMemoryStatus(&vmStatus);
    long curAvailableVM = (long)vmStatus.dwAvailVirtual / 1048576;
    CServer* server = GetNetHandler().GetServer();
    long curNetW2L = server->GetSendSizePerSecond(NF_LOGIN_SERVER);
    long curNetL2W  = server->GetRecvSizePerSecond(NF_LOGIN_SERVER);
    long curNetW2DB  = server->GetSendSizePerSecond(NF_DB_SERVER);
    long curNetDB2W  = server->GetRecvSizePerSecond(NF_DB_SERVER);
    long curNetW2S  = server->GetSendSizePerSecond(NF_GAME_SERVER);
    long curNetS2W  = server->GetRecvSizePerSecond(NF_GAME_SERVER);

    if (curNetW2L > maxNetW2L)
        maxNetW2L = curNetW2L;
    if (curNetL2W > maxNetL2W)
        if (curNetW2DB > maxNetW2DB)
            maxNetW2DB = curNetW2DB;
    if (curNetDB2W > maxNetDB2W)
        maxNetDB2W = curNetDB2W;
    if (curNetW2S > maxNetW2S)
        maxNetW2S = curNetW2S;
    if (curNetS2W > maxNetS2W)
        maxNetS2W = curNetS2W;

    if (curPlayerNum > maxPlayerNum)
        maxPlayerNum = curPlayerNum;

    if (curDupRgnNum > maxDupRgnNum)
        maxDupRgnNum = curDupRgnNum;

    if (curAvailableVM > maxAvailableVM)
        maxAvailableVM = curAvailableVM;

    _snprintf(info, 10240, "\
                           OnlinePlayer=%d/%d	AvaiVirMem=%d/%d(MB)\r\n\
                           副本及场景个数=%d/%d\r\n\
                           NT-W2L=%d/%d(B/S)	NT-L2W=%d/%d(B/S)\r\n\
                           NT-W2DB=%d/%d(B/S)	NT-DB2W=%d/%d(B/S)\r\n\
                           NT-W2S=%d/%d(B/S)	NT-S2W=%d/%d(B/S)\r\n",
                           curPlayerNum, maxPlayerNum,
                           curAvailableVM, maxAvailableVM,
                           curDupRgnNum, maxDupRgnNum,
                           curNetW2L, maxNetW2L, curNetL2W, maxNetL2W,
                           curNetW2DB, maxNetW2DB, curNetDB2W, maxNetDB2W,
                           curNetW2S, maxNetW2S, curNetS2W, maxNetS2W);

    SetWindowText(AppFrame::GetWnd("info"), info);
}

// 获取游戏日志逻辑接口对象
LogicLogInterface* GetLogicLogInterface()
{
    return GetGame().GetLogInterface();
}

CGame::CGame(void)
: b_HasQuestEnemyFactionMsgFlag(false),
b_HasQuestCountryMsgFlag(false),
b_HasGetEnemyFactionMsgFlag(false),
b_HasGetCountryMsgFlag(false),
b_IsValidatePlayerIDinCdkey(false),
b_ValidateDBPlayerIDinCdkey(false),
m_CurMsgID(-1),
m_MsgTimer(1024,1024),
g_GlobalRgnManager(NULL),
g_RgnManager(NULL),		
g_EntityManager(NULL),
g_RankManager(NULL),
m_pGoodsExSetup(NULL),
m_bInitVarLoadFlag(false),
m_bInitCountryLoadFlag(false),
m_bInitRegionLoadFlag(false),
m_bRestartSessionFlag(true),
m_bInitFactionLoadFlag(false),
m_bInitUnionLoadFlag(false),
m_bInitPersonalHouseLoadFlag(false),
m_bDbReady(false),
m_bIsKickPlayerFlag(false),
m_bPinging(true)
{
    b_IsFirstConn = true;
    InitMsgTimerMap();
    s_lAITick = 0;

    InitializeCriticalSection(&m_CSServerInitSendFlag);

    m_pDupliRegionSetup = NULL;
    m_pMapFile = NULL;
    m_strFunctionList_FileData = NULL;
    m_strVariableList_FileData = NULL;
    m_lFunctionList_FileData_Size = 0;
    m_lVariableList_FileData_Size = 0;

    m_listGoodsLink.resize(500);

    m_pGeneralVariableList = NULL;
    m_dwLastReLoginTime = timeGetTime();

    m_bInPing = false;
    m_dwLastPingGameServerTime = timeGetTime();

    // 全局对象
    g_GlobalRgnManager	= new CGlobalRgnManager;
    g_EntityManager	= new CEntityManager;
    g_pMailManager =  GetMailManager()->GetInstance();
    g_RankManager = new CRankManager;
    m_pLogInterface = new LogicLogInterface();
}

CGame::~CGame(void)
{
    ReleaseMsgTimerMap();

    SAFE_DELETE(g_pMailManager);
    SAFE_DELETE(g_RankManager);
    DeleteCriticalSection(&m_CSServerInitSendFlag);
}

CSessionFactory* CGame::GetSessionFactoryInst(void)
{
    if(!m_pSessionFactory)
        m_pSessionFactory = new CSessionFactory;
    return m_pSessionFactory;
}

CGlobalRgnManager* CGame::GetGlobalRgnManager(void)
{
    return g_GlobalRgnManager;
}

CMailSystem* CGame::GetMailManager(void)
{
    return g_pMailManager;
}

CRgnManager* CGame::GetRgnManager(void)
{
    return g_RgnManager;
}

CEntityManager*	CGame::GetEntityManager(void)
{
    return g_EntityManager;
}

bool CGame::LoadServerResource()
{
    // 初始化数据包
    string strFilesInfo("PackRes.cfg");
    CClientResource::InitialInstance();
    CClientResource* pClientResource = CClientResource::GetInstance();
    pClientResource->Initial(GAME_RES, string(g_rootPath), strFilesInfo);
    if (!pClientResource->LoadEx())
    {
        Log4c::Trace(ROOT_MODULE, "[SYS] Load resource Package +++ FAILED +++");
        return true;
    }

    Log4c::Trace(ROOT_MODULE, "[SYS] Load Resource Package--- OK ---");
    return true;
}

//void CGame::SetNoonTimer(CMsgTimer* msgTime)
//{
//}

// 游戏初始化
BOOL CGame::Init()
{
    GetCurrentDirectory(MAX_PATH, g_rootPath);

    // 创建一个log的保存目录
    CreateDirectory("log",	NULL);

#ifdef _RUNSTACKINFO_
    InitStackFileName();
#endif

#ifdef __MEMORY_LEAK_CHECK__
    CBaseObject::ClearAllConstrucRef();
#endif

    srand((unsigned)time(NULL));
    random(100);

    LoadServerResource(); // 资源包
    LoadEx(LOAD_TEXTRES); // 文本资源

    if(!GetNetHandler().Init())
    {
        return false;
    }

    //GetInst(AntiCheat::CFileListManager).Init();
    //GetInst(AntiCheat::CQuestionManager).Startup();
    LoadEx(LOAD_PHASE);
    LoadEx(LOAD_REPUTATION);
    //LoadEx(SYNC_SCRIPT);
    LoadEx(LOAD_GLOBALSETUP);
    LoadEx(LOAD_PLAYER);
    LoadEx(LOAD_GOODSLIST);
    LoadEx(LOAD_SUIT);
    LoadEx(LOAD_ENCHASE);
    LoadEx(LOAD_BUSINESS);
    LoadEx(LOAD_GENIUSSYS);
    LoadEx(LOAD_GOODSEX);
    LoadEx(LOAD_GOODSTAILOR);
    LoadEx(LOAD_DEPOT);
    LoadEx(LOAD_CONTACTS);
    LoadEx(LOAD_MONSTER);
    LoadEx(LOAD_GATES);
    LoadEx(LOAD_NPCSHOP);
	LoadEx(LOAD_PET);
    LoadEx(LOAD_INCREMENTSHOP);
    LoadEx(LOAD_CONTRIBUTE);
    LoadEx(LOAD_SKILL);
    LoadEx(LOAD_SYSMAIL);
    LoadEx(LOAD_LOGSYS);
    LoadEx(LOAD_SCRIPTS);
    LoadEx(LOAD_GMLIST);
    
    // 初始化副本的场景配置
    m_pDupliRegionSetup = new CDupliRegionSetup;
    if (!m_pDupliRegionSetup->Load())
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Can't find file Setup/DupliRegionsSetup.ini");
        return false;
    };

    GetGlobalRgnManager()->LoadRgnSetupIni("Setup/regionlist.ini");
    GetGlobalRgnManager()->LoadDupRgnServerIni("data/duprgnserver.ini");
    GetGlobalRgnManager()->LoadRegionPlugSetupIni("Setup/regionplug.ini");

    LoadEx(LOAD_REGIONSETUP);
    LoadEx(LOAD_HITLEVEL);
    //LoadEx(LOAD_HELP);
    LoadEx(LOAD_BROADCAST);
    LoadEx(LOAD_SPRITE);
    LoadEx(LOAD_SCRIPTAI);
    LoadEx(LOAD_RELIVEGOODS);   
    LoadEx(LOAD_FBUSINESS); // 家族跑商	
    LoadEx(LOAD_MONSTERAI);
    LoadEx(LOAD_AHCONF);

    //装载无效文字系统
    CWordsFilter::GetInstance()->Initial(string("Setup/InvalidStr.ini"));
    //初始化组织系统的参数
    WSOrganizingParam::getInstance().Initialize();
    //初始化组织系统
    GetOrganiCtrl()->Initialize();

    //装载任务列表
     if ( CQuestSystem::Init("quest/QuestList.xml") )
     {
         LoadLog("GAME_LOAD_QUESTLIST", "quest/QuestList.xml", 1);
     }
     CDeputyOccuSystem::Init("data/DeputyOccu/DeputyOccu.xml");

    //初始化国家的参数
    InitCountryPlayerNum();
    GetCountryParam()->Initialize();
    GetCountryHandler()->Initialize();

    CIncrementShopManager::Init();

    // 全局脚本变量
    m_pGeneralVariableList = new VariableList;
    VarList::Load( m_pGeneralVariableList, "data/general_variable.xml" );
    // Fox@20081110 玩家管理, 暂只用于在线玩家搜索
    GetInst(CPlayerMan).Initial();

    UnloadServerResource();
    // 创建日志线程
    unsigned threadID;
    g_hWriteLogThread = (HANDLE)_beginthreadex(NULL,0,ProcessWriteLogDataFunc,NULL,0,&threadID);

    ResetStarDay((DWORD)time(NULL));

    GetInst( AH::OrderMgr ).Startup();
    Log4c::Trace(ROOT_MODULE,FormatText("GAME_SYS_SERVEROK", "WorldServer"));
    return true;
}

BOOL CGame::InitStackFileName()
{
    SYSTEMTIME stTime;
    GetLocalTime(&stTime);
    _snprintf(m_pszRunStackFile,MAX_PATH,"RunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

    _snprintf(m_pszSaveGoodsRunStackFile,MAX_PATH,"SaveGoodsRunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
    _snprintf(m_pszOperateMailFile,MAX_PATH,"OprateMailInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

    return TRUE;
}

// 读取所有的脚本文件
void FindScriptFile(const char* strFile, list<string>* plistFileName)
{
    char strPath[MAX_PATH];
    GetFilePath(strFile, strPath);

    WIN32_FIND_DATA stFindData;
    HANDLE hFind = FindFirstFile(strFile, &stFindData);
    if (hFind==INVALID_HANDLE_VALUE)
    {
        return;
    }

    BOOL bNext = true;
    while(1)
    {
        bNext = FindNextFile(hFind, &stFindData);
        if (bNext==0)
        {
            break;
        }

        // 是目录
        if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (stricmp(stFindData.cFileName, "..")!=0)
            {
                char str[MAX_PATH];
                sprintf(str, "%s%s/*.*", strPath, stFindData.cFileName);
                FindScriptFile(str, plistFileName);
            }
        }
        else	// 文件
        {
            char strExtName[MAX_PATH];
            GetFileExtName(stFindData.cFileName, strExtName);
            if (stricmp(strExtName, ".lua")==0)
            {
                char str[MAX_PATH];
                sprintf(str, "%s%s", strPath, stFindData.cFileName);
                strlwr(str);	//  转换成小写
                plistFileName->push_back(str);
            }
        }
    }

    FindClose(hFind);
}

// 读取指定目录中的所有脚本文件(包含子目录)
bool CGame::LoadScriptFileData(const char* dir_name,
                               const char* function_filename,
                               const char* var_filename,
                               const char* gen_var_filename)
{
    // 先清除原有数据
    SAFE_DELETE(m_strFunctionList_FileData);
    SAFE_DELETE(m_strVariableList_FileData);
    for(map<string, char*>::iterator it=m_mapScript_FileData.begin(); it!=m_mapScript_FileData.end(); it++)
    {
        char* p2 = (*it).second;
        SAFE_DELETE(p2);
    }
    m_mapScript_FileData.clear();

    // 读入functionlist.ini
    CRFile* rfile = rfOpen(function_filename);
    if( rfile )
    {
        m_lFunctionList_FileData_Size = rfile->GetDatalen();
        m_strFunctionList_FileData = new char[rfile->GetDatalen()+1];
        memset(m_strFunctionList_FileData, 0, rfile->GetDatalen()+1);
        rfile->ReadData(m_strFunctionList_FileData, m_lFunctionList_FileData_Size);
        rfClose(rfile);
    }
    else
    {
        Log4c::Error(ROOT_MODULE,"%-15s Can't found %s!",__FUNCTION__,function_filename);
        return false;
    }

    // 读入variablelist.ini
    rfile = rfOpen(var_filename);
    if( rfile )
    {
        m_lVariableList_FileData_Size = rfile->GetDatalen();
        m_strVariableList_FileData = new char[m_lVariableList_FileData_Size+1];
        memset(m_strVariableList_FileData, 0, m_lVariableList_FileData_Size+1);
        rfile->ReadData(m_strVariableList_FileData, m_lVariableList_FileData_Size);
        rfClose(rfile);
    }
    else
    {
        Log4c::Error(ROOT_MODULE,"%-15s Can't found %s!",__FUNCTION__,var_filename);
        return false;
    }

    // 读取Scripts/目录中的所有脚本文件
    list<string> listFileName;
    string strTemptFolder("scripts/*.*");
    CheckRFileStr(strTemptFolder);
    string strTempt(strTemptFolder);
    size_t nPos = strTemptFolder.find_last_of("\\");
    strTempt.erase(0,nPos+1);
    if(strTempt=="*.*")
        strTemptFolder.erase(nPos,strTemptFolder.length()-nPos);
    if(CClientResource::GetInstance()->IsFileExist(strTemptFolder.c_str()) )
    {
        string strTemptExt(".lua");
        CClientResource::GetInstance()->FindFileList(strTemptFolder,strTemptExt,listFileName);
    }
    else
    {
        FindScriptFile("scripts/*.*", &listFileName);
    }

    // 具体读取
    for(list<string>::iterator it=listFileName.begin(); it!=listFileName.end(); it++)
    {
        const char* filename = it->c_str();
        LoadOneScript(filename);
    }

    return true;
}

// 读取一个脚本文件
bool CGame::LoadOneScript(const char* filename)
{
    CRFile* rfile = rfOpen(filename);
    if( rfile )
    {
        int lFileLength = rfile->GetDatalen();

        char* pData = new char[lFileLength+1];
        memset(pData, 0, lFileLength+1);
        rfile->ReadData(pData, lFileLength);
        rfClose(rfile);

        // 把 '\' 转换成 '/'，并且去掉带头的 '\'
        char strName[MAX_PATH];
        strcpy(strName, filename);
        if (strName[0] == '\\')
            strcpy(strName, &strName[1]);
        ReplaceLine(strName);

        if (m_mapScript_FileData.find(strName) != m_mapScript_FileData.end())
        {
            SAFE_DELETE(m_mapScript_FileData[strName]);
        }

        m_mapScript_FileData[strName] = pData;
    }
    else
    {
        Log4c::Warn(ROOT_MODULE,"Can't found %s!", filename);
        return false;
    }

    return true;
}

bool CGame::ReLoadRegion(long lRegionID)
{
    // 只能找到Normal场景
    CGlobalRgnManager::tagRegion* ptagRegion = NULL;
    map<long, CGlobalRgnManager::tagRegion>::iterator RegionIter = g_GlobalRgnManager->GetTemplateRegionListMap()->find(lRegionID);
    if(RegionIter != g_GlobalRgnManager->GetTemplateRegionListMap()->end())
    {
        ptagRegion = &RegionIter->second;
    }

    if(ptagRegion && ptagRegion->pRegion)
    {
        ptagRegion->pRegion->LoadSetup();

        vector<BYTE> vectorByte;
        ptagRegion->pRegion->AddSetupToByteArray(&vectorByte);
        CMessage msg(MSG_W2S_SERVER_SENDSETUP);
        msg.Add((long)SYNC_REGION);
        msg.Add(lRegionID);
        msg.AddEx(&vectorByte[0], (long)vectorByte.size());
        msg.SendToGS(ptagRegion->dwGameServerIndex);
        return true;
    }
    return false;
}

bool CGame::ReLoadAllRegions()
{
    map<long, CGlobalRgnManager::tagRegion>::iterator it=GetGlobalRgnManager()->GetTemplateRegionListMap()->begin();//GetTemplateRegionListMap()->begin();
    for(;it != GetGlobalRgnManager()->GetTemplateRegionListMap()->end();it++)
    {
        CGlobalRgnManager::tagRegion& temptRegion = (*it).second;
        if( temptRegion.pRegion)
        {
            //long lRegionID = temptRegion.pRegion->GetID();
            CGUID lRegionID=temptRegion.pRegion->GetExID();
            temptRegion.pRegion->LoadSetup();

            vector<BYTE> vectorByte;
            temptRegion.pRegion->AddSetupToByteArray(&vectorByte);
            CMessage msg(MSG_W2S_SERVER_SENDSETUP);
            msg.Add((long)SYNC_REGION);
            msg.Add(lRegionID);
            msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            msg.SendToGS(temptRegion.dwGameServerIndex);
        }
    }
    return true;
}

bool CGame::LoadEx(long flag, long load, uint id)
{
    // 依字母序, 查看方便.
    bool res = false;
    bool sync = true;
    string module = "Unknown module";
    string position = "";
    uint state = 1;

    if (load & LOADFLAG_MOD)
    {
        if (load & LOADFLAG_PKG)
            LoadServerResource();

       switch(flag)
        {
        case LOAD_AHCONF: // 仙贝寄售
            {
                module = "GAME_LOAD_AHCONF"; position = "data/AHConf.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst( AH::ConfReader ).Load();
            }
            break;
        case LOAD_ANTICHEAT: // 反外挂
            {
                module = "GAME_LOAD_ANTICHEAT"; position = "Setup/AntiCheat.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst(AntiCheat::CConfiger).Load();
            }
            break;
            //case LOAD_BROADCAST: // 系统广播
            //    {
            //        module = "GAME_LOAD_BROADCAST"; position = "";
            //    }
            //    break;
        case LOAD_BUSINESS: // 商业
            {
                module = "GAME_LOAD_BUSINESS"; position = "business/";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst( Business::CBusinessManager ).Load();
            }
            break;
        case LOAD_CONTACTS: // 好友
            {
                module = "GAME_LOAD_CONTACTS"; position = "data/LinkmanSetup.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = LinkmanSystem::GetInstance().LoadSetupAndInit("data/LinkmanSetup.xml");
            }
            break;
        case LOAD_CONTRIBUTE: // 贡献
            {
                module = "GAME_LOAD_CONTRIBUTE"; position = "data/ContributeSetup.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = ContributeSetup::LoadContributeSetup("data/ContributeSetup.ini");
                ;
            }
            break;
        case LOAD_COUNTRY: // 国家
            {
                module = "GAME_LOAD_COUNTRY"; position = "data/CountryParam.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetCountryParam()->Load();
            }
            break;
        case LOAD_DEPOT: // 仓库
            {
                module = "GAME_LOAD_DEPOT"; position = "data/Depot.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = PlayerDepot::LoadSetup("data/Depot.xml");
                ;
            }
            break;
        case LOAD_EMOTE: // 动作表情
            {
                module = "GAME_LOAD_EMOTE"; position = "data/Emotions.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = EmotionSetup::LoadSetup( "data/Emotions.ini" );
            }
            break;
        case LOAD_ENCHASE: // 强化
            {
                module = "GAME_LOAD_ENCHASE"; position = "data/enchase.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = KitCardRuleSetup::Load( "data/enchase.ini" ) ? true : false;
            }
            break;
        case LOAD_FACTION: // 帮会
            {
                module = "GAME_LOAD_FACTION"; position = "data/Faction.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = WSOrganizingParam::getInstance().Initialize() ? true : false;
            }
            break;
        case LOAD_FBUSINESS: // 家族跑商
            {
                module = "GAME_LOAD_FBUSINESS"; position = "data/fbusiness/";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst( FBusiness::CConfiger ).Load();
            }
            break;
        case LOAD_GAMESETUP: // 游戏配置
            {
                module = "GAME_LOAD_GAMESETUP"; position = "Setup/gamesetup.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GlobalSetup::LoadGameSetup("Setup/gamesetup.ini");
                RefreshInfo();
            }
            break;
        case LOAD_GATES: // 城门
            {
                module = "GAME_LOAD_GATES"; position = "data/buildlist.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GateList::LoadGateList("data/buildlist.ini");
            }
            break;
        case LOAD_GENVARS: // 通用变量
            {
                module = "GAME_LOAD_GENVARS"; position = "GeneralVariableList";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = VarList::Load( m_pGeneralVariableList, "data/general_variable.xml" );
            }
            break;
        case LOAD_GLOBALSETUP: // 全局参数
            {
                module = "GAME_LOAD_GLOBALSETUP"; position = "data/GlobalSetup.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GlobalSetup::Load("data/GlobalSetup.ini");
            }
            break;
        case LOAD_GMLIST: // GM列表
            {
                module = "GAME_LOAD_GMLIST"; position = "Setup/GMList.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GMList::Load("Setup/GMList.ini");
            }
            break;
        case LOAD_GOODSEX: // 物品扩展配置
            {
                module = "GAME_LOAD_GOODSEX"; position = "data/GoodsExSetup.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                if (NULL == m_pGoodsExSetup)
                {
                    m_pGoodsExSetup = new GoodsExSetup();
                }
                res = m_pGoodsExSetup->ReadSetup("data/GoodsExSetup.xml") &&
                    m_pGoodsExSetup->InitServerSetup();
            }
            break;
        case LOAD_GOODSLIST: // 原始物品列表
            {
                module = "GAME_LOAD_GOODSLIST"; position = "data/goodslist.dat";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GoodsSetup::Load( "data/goodslist.dat" ) &&
                    GoodsSetup::LoadAttrDBSetup("data/GoodsAttrDBSetup.ini");
            }
            break;
        case LOAD_GOODSTAILOR: // 制作配方
            {
                module = "GAME_LOAD_GOODSTAILOR"; position = "data/GoodsTailor.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GoodsTailorSetup::LoadGoodsTailorSetup("data/GoodsTailor.xml");
            }
            break;
        case LOAD_HITLEVEL: // 连击等级
            {
                module = "GAME_LOAD_HITLEVEL"; position = "data/hitlevel.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res=HitLevelSetup::LoadHitLevelSetup("data/hitlevel.ini");
            }
            break;
        case LOAD_INCREMENTSHOP: // 增值商店
            {
                module = "GAME_LOAD_INCREMENTSHOP"; position = "data/incrementshoplist.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = CIncrementShopList::LoadIncrementShopList("data/incrementshoplist.xml");
            }
            break;
        case LOAD_INSTANCE: // 副本
            {
                module = "GAME_LOAD_INSTANCE"; position = "data/duprgnserver.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetGlobalRgnManager()->LoadDupRgnServerIni("data/duprgnserver.ini");
            }
            break;
        case LOAD_LOGSYS: // 日志系统
            {
                module = "GAME_LOAD_LOGSYS"; position = "Setup/LogSystem.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = LogSystem::Load("Setup/LogSystem.ini");
            }
            break;
        case LOAD_MONSTER: // 怪物
            {
                module = "GAME_LOAD_MONSTER"; position = "data/monsterlist.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = CMonsterList::LoadMonsterList("data/monsterlist.ini") &&
                    CMonsterList::LoadMonsterDrop("data/DropList.xml") &&
                    CMonsterList::LoadActivityDrop("data/ActivityDropList.xml") &&
                    CMonsterList::LoadSkillList("data/MonsterSkillList.xml");
            }
            break;
        case LOAD_MONSTERAI: // 怪物AI
            {
                module = "GAME_LOAD_MONSTERAI"; position = "data/MonsterAI.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst( StateAI::MonsterAISetup ).Load();
            }
            break;
        case LOAD_NPCSHOP: // NPC商店
            {
                module = "GAME_LOAD_NPCSHOP"; position = "data/tradelist.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = TradeList::LoadTradeList("data/tradelist.ini");
            }
            break;
        case LOAD_PET: // 宠物
            {
                module = "GAME_LOAD_PET"; position = "Data/PetConfig.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst(CPetCtrl).LoadConfig();
            }
            break;
        case LOAD_PHASE: // 相位资源
            {
                module = "GAME_LOAD_PHASE"; position = "data/PhaseSetup.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res=GetInst(PhaseSetup).Load() ? true : false;
            }
            break;
        case LOAD_GENIUSSYS:// 天赋
            {
                module = "GAME_LOAD_GENIUSSYS"; position = "Data/GeniusSys/GeniusList.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res=GetInst(GeniusSetup).Load() ? true : false;
            }
            break;

        case LOAD_PLAYER: // 玩家
            {
                module = "GAME_LOAD_PLAYER"; position = "data/LevelExp/PlayerLevelExp.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = PlayerList::LoadPlayerExpList("data/LevelExp/PlayerLevelExp.ini") &&
                    PlayerList::LoadOccuLvlExpList() &&
                    PlayerList::LoadOccuModifyProperty("data/Player_occupation_Property.ini") &&
                    PlayerList::LoadOccuDefParam("data/Player_OccuDef_Praram.ini") &&
                    PlayerList::LoadOccuAtkParam("data/Player_OccuAtk_Praram.ini");
            }
            break;
        case LOAD_PLAYERMGR: // 玩家管理
            {
                module = "GAME_LOAD_PLAYERMGR"; position = "Data/PMConfig.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst(CPlayerMan).LoadConfig();
            }
            break;
        case LOAD_PROFESSION: // 专业技能
            {
                module = "GAME_LOAD_PROFESSION"; position = "data/DeputyOccu/DeputyOccu.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = CDeputyOccuSystem::Init("data/DeputyOccu/DeputyOccu.xml");
            }
            break;
        case LOAD_QUEST: // 任务
            {
                module = "GAME_LOAD_QUEST"; position = "quest/QuestList.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = CQuestSystem::Init("quest/QuestList.xml") ? true : false;
            }
            break;
        case LOAD_REGIONSETUP: // 场景配置
            {
                module = "GAME_LOAD_REGIONSETUP"; position = "data/regionsetup.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = CRegionSetup::LoadRegionSetup("data/regionsetup.ini");
            }
            break;
        case LOAD_REGIONLIST: // 场景列表
            {
                module = "GAME_LOAD_REGIONLIST"; position = "Setup/regionlist.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = g_GlobalRgnManager->LoadRgnSetupIni("Setup/regionlist.ini");
            }
            break;
        case LOAD_RELIVEGOODS: // 复活道具
            {
                module = "GAME_LOAD_RELIVEGOODS"; position = "data/ReliveGoodsList.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = ReLiveGoodsList::Init("data/ReliveGoodsList.xml");
            }
            break;
        case LOAD_REPUTATION: // 声望
            {
                module = "GAME_LOAD_REPUTATION"; position = "data/area.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res=GetInst(CCreditSystem).LoadAreaCredit();
            }
            break;
        case LOAD_SCRIPTAI: // 脚本AI
            {
                module = "GAME_LOAD_SCRIPTAI"; position = "data/MonsterScriptAI.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = CScriptAILoader::Instance().Load( "data/MonsterScriptAI.xml" );
            }
            break;
        case LOAD_SCRIPT: // 部分脚本
            {
                module = "GAME_LOAD_SCRIPT"; position = "scripts/";
                LoadLog(module.c_str(), position.c_str(), 0);
                //if (GetFileDialog(true, NULL, "Script files (*.lua)\0*.lua\0All files (*.*)\0*.*\0\0"))
                //{
                //    SetCurrentDirectory(g_rootPath);
                //    str, &str[lstrlen(g_rootPath)+1]);
                //    ReplaceLine(str);
                //    strlwr(str);	//  转换成小写
                //    res = LoadOneScript(str);
                //}
                //SetCurrentDirectory(g_rootPath);
            }
            break;
        case LOAD_SCRIPTS: // 所有脚本
            {
                module = "GAME_LOAD_SCRIPTS"; position = "scripts/";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = LoadScriptFileData("scripts/", "data/function.ini", "data/variable.ini", "data/general_variable_data.ini");
            }
            break;
        case LOAD_SETUP: // 配置
            {
                module = "GAME_LOAD_SETUP"; position = "Setup.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                GetNetHandler().LoadSetup();
                sync = false;
            }
            break;
        case LOAD_SKILL: // 技能
            {
                module = "GAME_LOAD_SKILL"; position = "data/StudySkillCondition.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = GetInst( SkillAttribute ).Load() && CNewSkillFactory::LoadNewSkillCache() &&
                      CStudySkillCondition::Init("data/StudySkillCondition.xml") &&
                      COccupSkillList::Init("data/OccupSkillList.xml") &&
                      CStudySkillCondition::InitSkillList("data/TeacherSkill.xml") &&
                      CStudySkillCondition::InitFactureSkillList("data/factureskilllist.xml") ;
            }
            break;
        case LOAD_SPEECH: // 言论过滤
            {
                module = "GAME_LOAD_SPEECH"; position = "setup/invalidstr.ini";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = CWordsFilter::GetInstance()->ReloadFilter();
            }
            break;
        case LOAD_SPRITE: // 精灵
            {
                module = "GAME_LOAD_SPRITE"; position = "data/Sprite.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = SpriteSystem::LoadSetup() ? true : false;
            }
            break;
        case LOAD_SUIT: // 套装
            {		
                module = "GAME_LOAD_SUIT"; position = "data/suit.dat";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = SuitRuleSetup::Load( "data/suit.dat" ) ? true : false;
            }
            break;
        case LOAD_SYSMAIL: // 系统邮件
            {
                module = "GAME_LOAD_SYSMAIL"; position = "mail/sysmail.xml";
                LoadLog(module.c_str(), position.c_str(), 0);
                res = MailList::LoadSysMailList("mail/sysmail.xml") &&
                    GetMailManager()->InitSysMail();
            }
            break;
        case LOAD_TEXTRES: // 文本资源
            {
                module = "GAME_LOAD_TEXTRES"; position = "textres/*.xml";
                res = AppFrame::LoadTextRes();
            }
            break;
        default:
            {
                res = false;
                char str[64];
                sprintf(str, "Unknown loading module: %d", flag);
                Log4c::Warn(ROOT_MODULE,FormatText("GAME_COMMON_INVALID", str));
                return res;
            }
            break;
        }
        LoadLog(module.c_str(), position.c_str(), 1);

        if (load & LOADFLAG_PKG)
            UnloadServerResource();
    }

    // 发送
    if (sync && CMessage::IsGSReady(id))
    {
        CMessage msg(MSG_W2S_SERVER_SENDSETUP);
        msg.Add(flag);
        DBWriteSet setWriteDB;
        msg.GetDBWriteSet(setWriteDB);
        vector<BYTE> vectorByte;

        switch(flag)
        {
        case LOAD_AHCONF: // 仙贝寄售
            {
                GetInst( AH::ConfReader ).Encode( setWriteDB );
            }
            break;
        case LOAD_ANTICHEAT: // 反外挂
            {
                //GetInst(AntiCheat::CConfiger).EncodeToDataBlock(setWriteDB);
                return false;
            }
            break;
        case LOAD_BROADCAST: // 系统广播
            {
                return false;
            }
            break;
        case LOAD_BUSINESS: // 商业
            {
                // 编码与 GSID 相关, 需改写相关代码
                if( GetInst( Business::CBusinessManager ).IsEnable()
                    && GetInst( Business::CBusinessManager ).IsDBLoaded() )
                {
                    GetInst( Business::CBusinessManager ).EncodeToDataBlock(setWriteDB, id);
                }
                else
                    return false;
            }
            break;
        case LOAD_CONTACTS: // 好友
            {
                LinkmanSystem::GetInstance().SetupAddToByteArray(vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&(vectorByte[0]), (long)vectorByte.size());
            }
            break;
        case LOAD_CONTRIBUTE: // 贡献
            {
                ContributeSetup::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_COUNTRY: // 国家
            {
                GetCountryParam()->AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
                GetCountryHandler()->AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_DEPOT: // 仓库
            {
                msg.Add((long)sizeof(tagDepotSetup));
                msg.AddEx((void*)PlayerDepot::GetDepotSetup(), sizeof(tagDepotSetup));
            }
            break;
        case LOAD_EMOTE: // 动作表情
            {
                EmotionSetup::Serialize(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_ENCHASE: // 强化
            {
                KitCardRuleSetup::Serialize( &vectorByte );
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_FBUSINESS: // 家族跑商
            {
                if( GetInst( FBusiness::CConfiger ).IsEnable() )
                {
                    GetInst( FBusiness::CConfiger ).Encode(setWriteDB);
                }
                else
                    return false;
            }
            break;
        case LOAD_GATES: // 城门
            {
                GateList::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0],(long)vectorByte.size());
            }
            break;
        case LOAD_GENVARS: // 通用变量
            {
                DBWriteSet db;
                msg.GetDBWriteSet( db );
                VarList::Encode( m_pGeneralVariableList, db );
            }
            break;
        case LOAD_GLOBALSETUP: // 全局参数
            {
                GlobalSetup::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_GMLIST: // GM列表
            {
                GMList::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_GOODSEX: // 物品扩展配置
            {
                SerializeGoodsExSetup(vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_GOODSLIST: // 原始物品列表
            {
                GoodsSetup::Serialize( &vectorByte );
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_GOODSTAILOR: // 制作配方
            {
                msg.GetDBWriteSet(setWriteDB);
                GoodsTailorSetup::Code(setWriteDB);
            }
            break;
        case LOAD_HITLEVEL: // 连击等级
            {
                HitLevelSetup::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0],(long)vectorByte.size());
            }
            break;
        case LOAD_INCREMENTSHOP: // 增值商店
            {
                CIncrementShopList::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_INSTANCE: // 副本
            {
                GetDupliRegionSetup()->AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_LOGSYS: // 日志系统
            {
                LogSystem::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_MONSTER: // 怪物
            {
                CMonsterList::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_MONSTERAI: // 怪物AI
            {
                GetInst( StateAI::MonsterAISetup ).Encode( setWriteDB );
            }
            break;
        case LOAD_NPCSHOP: // NPC商店
            {
                TradeList::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_PET: // 宠物
            {
                GetInst(CPetCtrl).CodeToDataBlock(setWriteDB);
            }
            break;
        case LOAD_PHASE: // 相位
            {
                GetInst(PhaseSetup).PhaseConfigEnCode(setWriteDB);
            }
            break;
        case LOAD_GENIUSSYS: // 天赋
            {
                GetInst(GeniusSetup).GeniusConfigEnCode(setWriteDB);
            }
            break;
        case LOAD_PLAYER: // 玩家
            {
                PlayerList::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_PLAYERMGR: // 玩家管理
            {
                GetInst(CPlayerMan).CodeToDataBlock(setWriteDB);
            }
            break;
        case LOAD_PROFESSION: // 专业技能
            {
                CDeputyOccuSystem::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_QUEST: // 任务
            {
                CQuestSystem::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_REGIONLIST: // 场景列表
            {
                long size = 0;
                map<long, CGlobalRgnManager::tagRegion>* regions = GetGlobalRgnManager()->GetTemplateRegionListMap();
                map<long, CGlobalRgnManager::tagRegion>::iterator itr = regions->begin();
                for (; itr != regions->end(); ++itr)
                {
                    CGlobalRgnManager::tagRegion* pRegion = &itr->second;
                    if (pRegion && pRegion->dwGameServerIndex == id)
                    {
                        pRegion->pRegion->AddToByteArray(&vectorByte);
                        msg.Add((long)pRegion->RegionType);
                        msg.Add((long)pRegion->pRegion->GetID());
                        msg.Add(pRegion->pRegion->GetExID());
                        msg.Add((long)vectorByte.size());
                        msg.AddEx(&vectorByte[0], (long)vectorByte.size());
                        ++size;
                    }
                }
                msg.Add((long)RGN_INVALID);
                if (0 == size) return false;
            }
            break;
        case LOAD_REGIONSETUP: // 场景设置
            {
                CRegionSetup::AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_RELIVEGOODS: // 复活道具
            {
                ReLiveGoodsList::AddToByteArray( vectorByte );
                msg.Add( (long)vectorByte.size() );
                msg.AddEx( &vectorByte[0], (long)vectorByte.size() );
            }
            break;
        case LOAD_REPUTATION: // 声望
            {
                GetInst(CCreditSystem).CodeToByteArray(setWriteDB);
            }
            break;
        case LOAD_SCRIPT: // 脚本
            {
                //msg.Add(str.c_str());
                //msg.Add((long)lstrlen(GetScriptFileData(str)));
                //msg.Add(GetScriptFileData(str));
                return false;
            }
            break;
        case LOAD_SCRIPTS: // 所有脚本
            {
                msg.Add((long)GetScriptFileDataMap()->size());
                map<string, char*>::iterator itr=GetScriptFileDataMap()->begin();
                for (; itr != GetScriptFileDataMap()->end(); ++itr)
                {
                    long len = lstrlen(itr->second);
                    string str = itr->first;
                    msg.Add(str.c_str());
                    msg.Add(len);
                    msg.AddEx(itr->second, len);
                }
            }
            break;
        case LOAD_SCRIPTAI: // 脚本AI
            {
                CScriptAILoader::Instance().AddToByteArray( vectorByte );
                msg.Add( (long)vectorByte.size() );
                msg.AddEx( &vectorByte[0], (long)vectorByte.size() );
            }
            break;
        case LOAD_SKILL: // 技能
            {
                CNewSkillFactory::CodeNewSkill(vectorByte);
                COccupSkillList::AddToByteArray(vectorByte);
                CStudySkillCondition::AddToByteArray(vectorByte);
                msg.Add( static_cast<long>(vectorByte.size()) );
                msg.AddEx( &vectorByte[0], static_cast<long>( vectorByte.size() ) );
                GetInst( SkillAttribute ).ConfigEnCode( setWriteDB );
            }
            break;
        case LOAD_SPEECH: // 言论过滤
            {
                CWordsFilter::GetInstance()->AddToByteArray(vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_SPRITE: // 精灵
            {
                SpriteSystem::SetupAddToByteArray(vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0],(long)vectorByte.size());
            }
            break;
        case LOAD_SUIT: // 套装
            {
                SuitRuleSetup::Serialize( &vectorByte );
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case LOAD_SYSMAIL: // 系统邮件
            {
                MailList::AddToByteArray(&vectorByte);
                msg.Add(static_cast<long>(vectorByte.size()));
                msg.AddEx(&vectorByte[0],static_cast<long>(vectorByte.size()));
            }
            break;
        case LOAD_TEXTRES: // 文本资源
            {
                vector<string> prefixes;
                prefixes.push_back("GAME");
                prefixes.push_back("GS");
                prefixes.push_back("Q");
                AppFrame::EncodeTextRes(vectorByte, prefixes);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case SYNC_BRANCHRGN: // 分线场景数据
            {
                GetGlobalRgnManager()->AddLinedRgnInfoToByteArray(vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case SYNC_GSINFO: // GS信息
            {
                msg.Add((long)SYNC_GSINFO);
                msg.Add((BYTE)id);
            }
            break;
        case SYNC_INCLIMIT: // 增值商店购买限制
            {
                CIncrementShopManager::GetSingleton().AddToByteArray(&vectorByte);
                msg.Add((long)vectorByte.size());
                msg.AddEx(&vectorByte[0], (long)vectorByte.size());
            }
            break;
        case SYNC_INSTANCEDATA: // 副本模板
            {
                map<long, CGlobalRgnManager::tagRegion>* pPMap = GetGlobalRgnManager()->GetTemplatePersonalRgnListMap();
                map<long, CGlobalRgnManager::tagRegion>* pTMap = GetGlobalRgnManager()->GetTemplateTeamRgnListMap();
                if(pPMap && pTMap)
                {
                    msg.Add((long)(pPMap->size() + pTMap->size())); // 副本模板个数
                    map<long, CGlobalRgnManager::tagRegion>::iterator itr = pPMap->begin();
                    for(; itr != pPMap->end(); itr++)
                    {
                        msg.Add((long)itr->first); // 副本模板ID
                        if(itr->second.pRegion)
                        {
                            CWorldRegion* pRgn = new CWorldRegion;
                            pRgn->CloneAllData(itr->second.pRegion);
                            pRgn->AddToByteArray(&vectorByte);
                            delete pRgn;
                        }
                        msg.Add((long)vectorByte.size()); // bufsize
                        if(vectorByte.size() > 0)
                            msg.AddEx(&vectorByte[0], (long)vectorByte.size()); // buf	
                    }

                    itr = pTMap->begin();
                    vectorByte.clear();
                    for(; itr != pTMap->end(); itr++)
                    {
                        msg.Add((long)itr->first); // 副本模板ID
                        if(itr->second.pRegion)
                        {
                            CWorldRegion* pRgn = new CWorldRegion;
                            pRgn->CloneAllData(itr->second.pRegion);
                            pRgn->AddToByteArray(&vectorByte);
                            delete pRgn;
                        }
                        msg.Add((long)vectorByte.size()); // bufsize
                        if(vectorByte.size() > 0)
                            msg.AddEx(&vectorByte[0], (long)vectorByte.size()); // buf	
                    }
                }
                else
                    return false;
            }
            break;
        case SYNC_REGION: // 单个场景
            {
                return false;
            }
            break;
        case SYNC_REGIONS: // 所有场景
            {
                return false;
            }
            break;
        case SYNC_SCRIPT: // 脚本
            {
                if (GetFuncitonList_FileData())
                {
                    msg.Add((long)lstrlen(GetFuncitonList_FileData()));
                    msg.Add(GetFuncitonList_FileData());
                }
                else
                    return false;
            }
            break;
        case SYNC_VARIABLES: // 变量
            {
                // NOTE: no variables, only GeneralVariables
                //if (GetVariableList_FileData())
                //{
                //    msg.Add((long)lstrlen(GetVariableList_FileData()));
                //    msg.Add(GetVariableList_FileData());
                //}
                //else
                return false;
            }
            break;
        case SYNC_WSINFO: // WS信息
            {
                msg.Add((long)GetNetHandler().GetServerSetup().serverNum);
            }
            break;
        default:
            {
                char info[128];
                sprintf(info, "Unknown sync module: %u", flag);
                Log4c::Warn(ROOT_MODULE,FormatText("GAME_COMMON_INVALID", info));
                return false;
            }
            break;
        }

        if (0 == id || 1 == id)
        {
            msg.SendToAllGS();
        }
        else
        {
            msg.SendToGS(id);
        }

    }

    return res;
}

// 游戏释放
BOOL CGame::Release()
{
    Log4c::Trace(ROOT_MODULE, "WorldServer Exiting...");

#ifdef _RUNSTACKINFO_
    CMessage::AsyCloseFile(GetStatckFileName());
#endif

    // Fox@20081110 玩家管理, 暂只用于在线玩家搜索
    GetInst(CPlayerMan).Release();

    GetInst( AntiCheat::CFileListManager ).Release();
    GetInst( AntiCheat::CQuestionManager ).Shutdown();
    GetInst(CCreditSystem).Release();

    // 关闭网络连接
    GetNetHandler().Fini();

    ClearMapPlayer();

    // 关闭存盘线程句柄
    WaitForSingleObject(g_hSavingThread, INFINITE);
    CloseHandle(g_hSavingThread);


    /*
    *清理物品展示列表
    */
    list<tagGoodsLink>::iterator GoodsLinkIter=m_listGoodsLink.begin();
    for(;GoodsLinkIter!=m_listGoodsLink.end();GoodsLinkIter++)
    {
        if(GoodsLinkIter->bChange && GoodsLinkIter->pGoods)
            SAFE_DELETE(GoodsLinkIter->pGoods);
    }
    m_listGoodsLink.clear();

    SAFE_DELETE(m_strFunctionList_FileData);
    SAFE_DELETE(m_strVariableList_FileData);
    for(map<string, char*>::iterator it=m_mapScript_FileData.begin(); it!=m_mapScript_FileData.end(); it++)
    {
        char* p2 = (*it).second;
        SAFE_DELETE(p2);
    }

    SAFE_DELETE(m_pGeneralVariableList);

    CIncrementShopManager::Destroy();

    // 清除国家
    GetCountryHandler()->Release();
    CCountryParam::ReleaseInstance();

    //清理组织系统
    CWordsFilter::GetInstance()->Release();
    GetOrganiCtrl()->Release();
    COrganizingCtrl::releaseInstance();
    WSOrganizingParam::getInstance().Release();

    CQuestSystem::Relese();

    GetInst( AH::OrderMgr ).Shutdown();

    GetTimer()->Release();

    CNewSkillFactory::ClearNewSkillCache();

    CStudySkillCondition::Release();
    COccupSkillList::Release();
//    CStringReading::Release();
    MailList::Release();


    GoodsSetup::Release();
    SuitRuleSetup::Release();
    KitCardRuleSetup::Release();
    SAFE_DELETE(m_pGoodsExSetup);
    CMonsterList::Release();
    CDeputyOccuSystem::Relese();
    ReLiveGoodsList::Release();
    GoodsTailorSetup::Release();

    GetNetSessionManager()->Release();

    // 等待日志线程结束
    g_bWriteLogThreadExit = true;
    WaitForSingleObject(g_hWriteLogThread, INFINITE);
    CloseHandle(g_hWriteLogThread);

    //! 新日志系统
    SAFE_DELETE(m_pLogInterface);

    // 关闭玩家数据库载入线程
    for(DWORD i=0; i<g_vPlayerLoadThreads.size(); ++i)
    {
        TerminateThread(g_vPlayerLoadThreads[i],0);
        CloseHandle(g_vPlayerLoadThreads[i]);
    }


    CCardsHandler::delInstance();

    //释放资源管理器
    SAFE_DELETE(m_pDupliRegionSetup);

    GetSessionFactoryInst()->Release();
    SAFE_DELETE(m_pSessionFactory);
    SAFE_DELETE(g_GlobalRgnManager);
    SAFE_DELETE(g_EntityManager);


#ifdef __MEMORY_LEAK_CHECK__
    CBaseObject::OutputConstructRef(TYPE_GOODS);
#endif
    Log4c::Trace(ROOT_MODULE, "WorldServer Exited!");
    return true;
}
bool CGame::UnloadServerResource()
{
    CClientResource* pClientResource = CClientResource::GetInstance();
    if(pClientResource)
    {
        pClientResource->Release();
        CClientResource::ReleaseInstance();
    }
    return true;
}

// 发送当前WorldServer上的所有Cdkey到LoginServer
void CGame::SendCdkeyToLoginServer(BOOL bIsReady)
{
    // 发送当前worldserver上的cdkey
    // 把服务器基本信息发送到LOGINSERVER
    CMessage msg(MSG_W2L_SERVER_SENDWORLDINFO);
    msg.Add((long)GetNetHandler().GetServerSetup().serverNum);
    msg.Add(GetNetHandler().GetServerSetup().serverName.c_str());
    msg.Add((long)bIsReady);
    msg.Add((long)m_mPlayer.size());
    for(map<CGUID, CPlayer*>::iterator it=m_mPlayer.begin(); it!=m_mPlayer.end(); it++)
    {
        CPlayer* pPlayer = it->second;
        msg.Add(pPlayer->GetAccount());
    }
    msg.SendToServer(NF_LOGIN_SERVER);
}

// 游戏AI
BOOL CGame::AI()
{
    //Region AI
    CGlobalRgnManager::MapRgnItr GSNRgnManagerItr = GetGlobalRgnManager()->GetRgnMap().begin();
    for(; GSNRgnManagerItr != GetGlobalRgnManager()->GetRgnMap().end(); GSNRgnManagerItr++)
    {
        if(GSNRgnManagerItr->second)
        {
            GSNRgnManagerItr->second->AI();
        }
    }

    //system boardcast
    DWORD dwCurTim = timeGetTime() / 1000;
    list<CGame::tagSysBroadcast>::iterator iter=GetBroadcastList()->begin();
    for(;iter!=GetBroadcastList()->end();iter++)
    {
        tagSysBroadcast *pst = &(*iter);

        // 计算时间到达与否
        if (dwCurTim - pst->dwLastNotifyTime > pst->dwIntervalTime)
        {
            // 计算出现机率
            DWORD odds = random(100);
            if (odds < pst->dwOdds)
            {
                CMessage msgBroadcast(MSG_W2S_OTHER_BROADCAST);
                msgBroadcast.Add(pst->lRegionID);
                msgBroadcast.Add(pst->lImportLevel);
                msgBroadcast.Add(pst->dwTextColor);
                msgBroadcast.Add(pst->dwBackColor);
                msgBroadcast.Add(const_cast<char*>(iter->strMessage.c_str()));

                // 世界广播
                if (pst->lRegionID == 0)
                {
                    msgBroadcast.SendToAllGS();
                }
                else
                    // 场景广播
                {
                    GSConnection* pGS = GetRegionGameServer(pst->lRegionID);
                    if (pGS)
                    {
                        msgBroadcast.SendToGS(pGS->index);
                    }
                }

                pst->dwLastNotifyTime = dwCurTim;
                pst->dwIntervalTime = pst->dwMinTime + random(pst->dwMaxTime - pst->dwMinTime);
            }
        }
    }

    return TRUE;
}

bool CGame::ProcMenus()
{
#define LoadMenuItem(ID) do { \
    if (AppFrame::MenuClicked(ID)) \
    LoadEx(ID, LOADFLAG_ALL); } while (0)

#define SyncMenuItem(ID) do { \
    if (AppFrame::MenuClicked(ID)) \
    LoadEx(ID, LOADFLAG_NIL); } while (0)


    bool res = false;
    LoadMenuItem(LOAD_SETUP);                     // 配置
    LoadMenuItem(LOAD_LOGSYS);                    // 日志
    LoadMenuItem(LOAD_TEXTRES);                   // 文本资源
    LoadMenuItem(LOAD_BROADCAST);                 // 系统广播
    LoadMenuItem(LOAD_GLOBALSETUP);               // 全局参数
    LoadMenuItem(LOAD_GAMESETUP);                 // 游戏配置
    LoadMenuItem(LOAD_GMLIST);                    // GM列表
    LoadMenuItem(LOAD_SPEECH);                    // 言论过滤
    LoadMenuItem(LOAD_PLAYER);                    // 玩家
    LoadMenuItem(LOAD_GOODSLIST);                 // 原始物品列表
	LoadMenuItem(LOAD_GOODSEX);                   // 物品扩展配置
	LoadMenuItem(LOAD_RELIVEGOODS);               // 复活道具
	LoadMenuItem(LOAD_INCREMENTSHOP);             // 增值商店
    LoadMenuItem(LOAD_NPCSHOP);                   // NPC商店    
	LoadMenuItem(LOAD_HITLEVEL);                  // 连击等级
    LoadMenuItem(LOAD_QUEST);                     // 任务
    LoadMenuItem(LOAD_MONSTER);                   // 怪物
    LoadMenuItem(LOAD_MONSTERAI);                 // 怪物AI
	LoadMenuItem(LOAD_SCRIPTAI);                  // 脚本AI
    LoadMenuItem(LOAD_PET);                       // 宠物
    LoadMenuItem(LOAD_SKILL);                     // 技能
	LoadMenuItem(LOAD_CONTACTS); 		          // 好友
	LoadMenuItem(LOAD_SUIT);                      // 套装
	LoadMenuItem(LOAD_SCRIPT);                    // 一个或多个脚本
    LoadMenuItem(LOAD_SCRIPTS);                   // 所有脚本
	LoadMenuItem(LOAD_GENVARS);                   // 通用变量
    LoadMenuItem(LOAD_REGIONLIST);                // 场景列表
    LoadMenuItem(LOAD_REGIONSETUP);               // 场景设置
    LoadMenuItem(LOAD_SYSMAIL);                   // 系统邮件
    LoadMenuItem(LOAD_GENIUSSYS);                 // 天赋系统
    LoadMenuItem(LOAD_FACTION);                   // 帮会
	LoadMenuItem(LOAD_GATES);                     // 城门
	LoadMenuItem(LOAD_COUNTRY);                   // 国家
    LoadMenuItem(LOAD_DOCCULIST);                 // 副职业列表
	LoadMenuItem(LOAD_GOODSTAILOR);               // 制作配方
    LoadMenuItem(LOAD_PLAYERMGR);                 // 玩家管理
    LoadMenuItem(LOAD_ANTICHEAT);                 // 反外挂
    LoadMenuItem(LOAD_PHASE);                     // 相位
    LoadMenuItem(LOAD_CONTRIBUTE);                // 贡献
    LoadMenuItem(LOAD_AHCONF);                    // 仙贝寄售
    LoadMenuItem(LOAD_REPUTATION);                // 声望
	LoadMenuItem(LOAD_PROFESSION);                // 专业技能
    LoadMenuItem(LOAD_INSTANCE);                  // 副本
	LoadMenuItem(LOAD_DEPOT);                     // 仓库
	LoadMenuItem(LOAD_EMOTE);                     // 动作表情
	LoadMenuItem(LOAD_ENCHASE);                   // 强化
	LoadMenuItem(LOAD_BUSINESS);                  // 商业
    LoadMenuItem(LOAD_FBUSINESS);                 // 家族跑商
	LoadMenuItem(LOAD_SPRITE);                    // 精灵

	SyncMenuItem(SYNC_INCLIMIT);                  // 增值商店购买限制
	SyncMenuItem(SYNC_BRANCHRGN); 			      // 分线场景数据
	SyncMenuItem(SYNC_INSTANCESELF); 			  // 个人房屋副本数据
	SyncMenuItem(SYNC_INSTANCEDATA); 	          // 副本模板
    SyncMenuItem(SYNC_SCRIPT);                    // 脚本
	SyncMenuItem(SYNC_VARIABLES);                 // 变量
    SyncMenuItem(SYNC_GSINFO);                    // GS信息
	SyncMenuItem(SYNC_WSINFO);                    // WS信息

    if (AppFrame::MenuClicked(OPER_CONNECTLS))    // 连接LoginServer
        GetNetHandler().CreateConnectThread(NF_LOGIN_SERVER);
    if (AppFrame::MenuClicked(OPER_CONNECTDBS))   // 连接DBServer
        GetNetHandler().CreateConnectThread(NF_DB_SERVER);
    if (AppFrame::MenuClicked(OPER_CONNECTLOGS))  // 连接LogServer
        GetNetHandler().CreateConnectThread(NF_LOG_SERVER);
    //if (AppFrame::MenuClicked(OPER_COLLECTPLAYERDATA))
    if (AppFrame::MenuClicked(OPER_SAVENOW))
        g_bSaveNowData = true;
    //if (AppFrame::MenuClicked(OPER_SAVEDATA))
    if (AppFrame::MenuClicked(OPER_KICKALL))
        KickAllPlayer();
    //if (AppFrame::MenuClicked(OPER_SHOWSAVEINFO))
    return res;
}

// 游戏主循环
BOOL CGame::MainLoop()
{

#ifdef _RUNSTACKINFO_
    CMessage::AsyClearFile(GetStatckFileName());
    CMessage::AsyWriteFile(GetStatckFileName(),"MainLoop() Start.");
#endif
    // 统计每个步骤的耗时
    static DWORD dwProcessServerInitSize = 0,
        dwRefreshSize = 0,
        dwClearOutPlayerSize = 0,
        dwSaveSize = 0,
        dwAISize = 0,
        dwNetSessionSize = 0,
        dwClearPlayerSize = 0,
        dwMessageSize = 0,		
        dwOrgSysSize = 0,
        dwMailTimerSize = 0,
        dwTimerSize = 0,		
        dwNetSessionFactionSize = 0;

    static MEMORYSTATUS msA,msB,msC;


    static DWORD dwCurTime = 0;

    static DWORD ai_tick, now = timeGetTime(), dwClearSaveGoodsStackInfoTime=now, dwChangeRgnPlayerTimeCheckTime = now, dwClearPlayerDataTime = now, old = now, lastSaveTime=now, lastRefeashInfoTime = now, lastReleaseLoginPlayerTime = 0;
    static DWORD dwRestartSessionFlagTime = now; // WS重启后各初始化事务是否成功完成

    static bool bStart = true;
    if (bStart)
    {
        CSMClient::GetSM()->SendAITick(true);
        bStart = false;
    }
    now = timeGetTime();
    ai_tick++;


    GlobalMemoryStatus(&msA);
    GetNetHandler().ProcessServerInitSendMsg();
    ProcMenus(); // 处理菜单

    GlobalMemoryStatus(&msB);
    dwProcessServerInitSize += (ulong)(msA.dwAvailVirtual-msB.dwAvailVirtual);

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"RefreshInfo() Start.");
#endif
    // 刷新显示
    dwCurTime = timeGetTime();
    if (now - lastRefeashInfoTime > GetNetHandler().GetSetup().refreshInfoTime)
    {
        lastRefeashInfoTime = now;
        RefreshInfo();
    }
    GlobalMemoryStatus(&msA);
    dwRefreshSize += (ulong)(msB.dwAvailVirtual-msA.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"RefreshInfo() End.");
#endif
    // 显示消耗
    if( now - old > 600000 )
    {

        ai_tick = 0;
        old = now;
    }

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"ChangeRgnPlayerTimeCheck() Start.");
#endif

    GlobalMemoryStatus(&msA);
    // 清除部分数据
    if(now - dwChangeRgnPlayerTimeCheckTime >= 10000) // 10秒
    {
        dwChangeRgnPlayerTimeCheckTime = now;
        CEntityManager::AccountMapItr deleteItr = GetEntityManager()->GetAccountMap().begin();
        while(deleteItr != GetEntityManager()->GetAccountMap().end())
        {	
            if(deleteItr->second->GetLoginTimeoutValue() != -1 
                && now - deleteItr->second->GetLoginTimeoutValue() > 10000) // 10秒
            {
                if(deleteItr->second->GetPlayerState() == PLAYER_STATE_CREATING)
                {
                    deleteItr->second->SetPlayerState(PLAYER_STATE_VOID);
                    deleteItr->second->SetLoginTimeoutValue(timeGetTime());
                }

                if(deleteItr->second->GetPlayerState() == PLAYER_STATE_CHANGING_SERVER
                    || deleteItr->second->GetPlayerState() == PLAYER_STATE_ENTERING_GAME)
                {
                    CEntityGroup* pLoginPlayerGroup = (CEntityGroup*)GetEntityManager()->FindLeafByComFlag(deleteItr->second, string("[loginplayergroup]"));
                    if(pLoginPlayerGroup)
                    {
                        map<CGUID, CBaseEntity*>::iterator playerItr = pLoginPlayerGroup->GetEntityGroupMap().begin();
                        for(; playerItr != pLoginPlayerGroup->GetEntityGroupMap().end(); playerItr++)
                        {
                            CPlayer* pPlayer = GetMapPlayer(playerItr->first);
                            if(pPlayer)
                            {
                                OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
                            }
                        }
                        deleteItr->second->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
                        deleteItr->second->SetLoginTimeoutValue(timeGetTime());
                    }
                    else
                    {
                        // 发送消息到DBS清除DB对象
                        SendUnRegDBEntity(deleteItr->first.c_str());
                        delete deleteItr->second;
                        deleteItr = GetEntityManager()->GetAccountMap().erase(deleteItr);
                        continue;
                    }
                }
            }
            deleteItr++;
        }
    }

    GlobalMemoryStatus(&msB);
    dwClearOutPlayerSize += (ulong)(msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"ChangeRgnPlayerTimeCheck() End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"DoSaveData() Start.");
#endif
    //到达存盘点
    if(g_bSaveNowData==true || (now - lastSaveTime > GlobalSetup::GetSetup()->dwSavePointTime) )
    {
        lastSaveTime = now;

        DoSaveData(false);

        //! 内存信息输出
        //!-----------------------------
        CGoodsFactory::OutputUseGoodsInfo("MemAllocInfo");
        CMailSystem::GetInstance()->OutMailInfo();
        LinkmanSystem::GetInstance().OutLinkmanInfo();
        GetOrganiCtrl()->OutOrganiInfo();
        GetMemPool().OutInfo();

        GetEntityManager()->OutEntityInfo();
        CSessionFactory::OutSessionInfo();

        OutputCreatePlayerInfo();
        //!-----------------------------

        GetNetHandler().PutoutInfo();

        char szInfo[1024] = "";
        //输出虚拟内存信息
        GlobalMemoryStatus(&msC);
        _snprintf(szInfo,1024,"\r\rn\
                              CurAvailVirtual:%dMB\r\n\
                              dwProcessServerInitSize:%d\r\n\
                              dwRefreshSize:%d\r\n\
                              dwClearOutPlayerSize:%d\r\n\
                              dwSaveSize:%d\r\n\
                              dwAISize:%d\r\n\
                              dwNetSessionSize:%d\r\n\
                              dwClearPlayerSize:%d\r\n\
                              dwMessageSize:%d\r\n\
                              dwOrgSysSize:%d\r\n\
                              dwMailTimerSize:%d\r\n\
                              dwTimerSize:%d\r\n\
                              dwNetSessionFactionSize:%d\r\n",
                              msC.dwAvailVirtual/1048576,
                              dwProcessServerInitSize,
                              dwRefreshSize ,
                              dwClearOutPlayerSize,
                              dwSaveSize,
                              dwAISize,
                              dwNetSessionSize,
                              dwClearPlayerSize,
                              dwMessageSize,
                              dwOrgSysSize,
                              dwMailTimerSize,
                              dwTimerSize,	
                              dwNetSessionFactionSize);
        //PutStringToFile("MemAllocInfo", szInfo);
        Log4c::Trace(ROOT_MODULE,szInfo);

        // map<long,long>::iterator it = m_mapMsgVirMemUseSize.begin();
        // for(;it != m_mapMsgVirMemUseSize.end();it++)
        // {
        //     if((*it).second > 0)
        //     {
        //         char pszMsgSize[256]="";
        //         _snprintf(pszMsgSize,256,"MsgType:%d,MsgUseSize:%d",(*it).first,(*it).second);
        //         PutStringToFile("MemAllocInfo", pszMsgSize);
        //     }
        // }
        // m_mapMsgVirMemUseSize.clear();

        dwProcessServerInitSize = 0;
        dwRefreshSize = 0;
        dwClearOutPlayerSize = 0;
        dwSaveSize = 0;
        dwAISize = 0;
        dwNetSessionSize = 0;
        dwClearPlayerSize = 0;
        dwMessageSize = 0;	
        dwOrgSysSize = 0;
        dwMailTimerSize = 0;
        dwTimerSize = 0;	
        dwNetSessionFactionSize = 0;

        g_bSaveNowData = false;
    }
    GlobalMemoryStatus(&msA);


    dwSaveSize += (ulong)(msB.dwAvailVirtual-msA.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"DoSaveData() End.");
#endif

    // 游戏主AI
    AddAITick();
    GlobalMemoryStatus(&msB);
    // 会话AI
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"SessionAI() Start.");
#endif

    GetSessionFactoryInst()->AI();
    GlobalMemoryStatus(&msA);
    dwNetSessionSize += (ulong)(msB.dwAvailVirtual-msA.dwAvailVirtual);

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"SessionAI() End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"Clear Data Start.");
#endif
    // 清除部分数据
    if( (long)(now - dwClearPlayerDataTime) >= GlobalSetup::GetSetup()->lWSSaveTime)
    {
        dwClearPlayerDataTime = now;
        // 执行清除
        CEntityManager::AccountMapItr deleteItr = GetEntityManager()->GetAccountMap().begin();
        while(deleteItr != GetEntityManager()->GetAccountMap().end())
        {	
            // dbs未连接
            if(!GetNetHandler().IsServerReady(NF_DB_SERVER)) break;

            if(deleteItr->second->GetPlayerState() == PLAYER_STATE_BACKTOLOGIN
                || deleteItr->second->GetPlayerState() == PLAYER_STATE_LOADED_BASE_DATA
                || deleteItr->second->GetPlayerState() == PLAYER_STATE_LOADED_CHAR_DATA)
            {
                if( deleteItr->second->GetLoginTimeoutValue() != -1
                    && (now - deleteItr->second->GetLoginTimeoutValue() >= 15000) ) // 15秒超时
                {
                    Account* pAcc = deleteItr->second;
                    CEntityGroup* pLoginPlayerGroup =  (CEntityGroup*)GetEntityManager()->FindLeafByComFlag(deleteItr->second, string("[loginplayergroup]"));
                    if(pLoginPlayerGroup)
                    {
                        char szCdkey[128];
                        memset(szCdkey, 0, sizeof(szCdkey));
                        strcpy(szCdkey, pAcc->GetName());

                        map<CGUID, CBaseEntity*>::iterator guidItr = pLoginPlayerGroup->GetEntityGroupMap().begin();
                        while(guidItr != pLoginPlayerGroup->GetEntityGroupMap().end())
                        {		
                            CPlayer* pPlayer = GetMapPlayer(guidItr->first);
                            if(pPlayer)
                            {
                                OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
                            }
                            if(guidItr != pLoginPlayerGroup->GetEntityGroupMap().end())
                                guidItr++;
                        }

                        // 发送消息到DBS清除DB对象
                        SendUnRegDBEntity(szCdkey);
                    }
                    // 完全清除,删除该账号对象
                    delete deleteItr->second;
                    deleteItr = GetEntityManager()->GetAccountMap().erase(deleteItr);
                }
                else
                    deleteItr++;
            }
            else
                deleteItr++;
        }
    }

    GlobalMemoryStatus(&msB);	
    dwClearPlayerSize += (ulong)(msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"Clear Data End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"AI() Start.");
#endif
    AI();
    GlobalMemoryStatus(&msA);
    dwAISize += (ulong)(msB.dwAvailVirtual-msA.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"AI() End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"ProcessMessage() Start.");
#endif
    GetNetHandler().ProcessMessage();
    GlobalMemoryStatus(&msB);
    dwMessageSize += (ulong)(msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"ProcessMessage() End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"m_MsgTimer.Expire() Start.");
#endif
    // 消息定时器处理 [AHC DBMSGTIMER]
    dwCurTime = timeGetTime();
    m_MsgTimer.Expire(dwCurTime);
    GetOrganiCtrl()->TimerRun(dwCurTime);
    GlobalMemoryStatus(&msA);
    dwOrgSysSize += (ulong)(msB.dwAvailVirtual-msA.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"m_MsgTimer.Expire() End.");
#endif


#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"GetMailManager().Expire() Start.");
#endif

    GetMailManager()->Run();
    GlobalMemoryStatus(&msB);
    dwMailTimerSize  += (ulong)(msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"GetMailManager().Expire() End.");
#endif

#ifdef _RUNSTACKINFO_
    if(now - dwClearSaveGoodsStackInfoTime > 3600*1000*4) // 4个小时
    {
        dwClearSaveGoodsStackInfoTime = now;
        CMessage::AsyClearFile(GetSaveGoodsStatckFileName());
    }
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"GetTimer()->Run() Start.");
#endif
    //时间计时器
    dwCurTime = timeGetTime();
    GetTimer()->Run();
    GlobalMemoryStatus(&msA);
    dwTimerSize += (ulong)(msB.dwAvailVirtual-msA.dwAvailVirtual);

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"GetTimer()->Run() End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"AntiCheat::CQuestionManager::Run Start.");
#endif

    GetInst( AntiCheat::CQuestionManager ).Run();

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"AntiCheat::CQuestionManager::Run End.");
#endif
    //网络会话
    GetNetSessionManager()->Run();
    GlobalMemoryStatus(&msB);
    dwNetSessionFactionSize += (ulong)(msA.dwAvailVirtual-msB.dwAvailVirtual);
    // 桢数控制
    static DWORD new_timer = timeGetTime();
    static DWORD old_timer = new_timer;
    //组织系统控制运行
    static DWORD Old_OrgRunTime = timeGetTime();
    new_timer = timeGetTime();
    GetOrganiCtrl()->Run((new_timer - Old_OrgRunTime)/60000);
    Old_OrgRunTime = new_timer;

    new_timer = timeGetTime();
    if (new_timer - old_timer < g_ms)
    {
        WaitForSingleObject(GetCurrentProcess(), g_ms - (new_timer - old_timer));
    }
    old_timer += g_ms;
    if ((long)new_timer - (long)old_timer > 1000)
    {
        OutputDebugString("warning!!! 1 second not call AI()\n");
        old_timer = timeGetTime();
    }

#ifdef _DEBUG
    if( now - old > 600000 )
    {
        char str[128];
        sprintf(str, "600秒调用了 %d 次AI\n", ai_tick);
        OutputDebugString(str);
        ai_tick = 0;
        old = now;
    }
#endif
    new_timer = timeGetTime();
    if( new_timer - lastReleaseLoginPlayerTime > GetNetHandler().GetSetup().releaseLoginPlayerTime )
    {
        lastReleaseLoginPlayerTime = new_timer;
        ProcessTimeOutLoginPlayer();
    }

    if(m_bIsKickPlayerFlag)
    {
        if(m_mPlayer.size() == 0)
        {
            CMessage msgexit(MSG_W2DB_SERVER_SAVEDATAEND);
            msgexit.Add((BYTE)1); // 下线保存标志
            msgexit.SendToServer(NF_DB_SERVER);
            Log4c::Trace(ROOT_MODULE,GetText("WS_SYS_10"));
            m_bIsKickPlayerFlag = false;
        }
    }

    GetInst( AH::OrderMgr ).Run();

    //! 检测是否到了新的一天
    static DWORD dwCheckSpace = timeGetTime();
    new_timer = timeGetTime();
    if (new_timer - dwCheckSpace > 1000 * 60)
    {
        dwCheckSpace = new_timer;
        //! 
        static WORD wDay	= 0;
        SYSTEMTIME stOldTime;
        GetLocalTime(&stOldTime);
        ulong dwData = (ulong)time(NULL);
        tm *pCurrData = localtime((time_t*)&dwData);
        if(wDay != pCurrData->tm_mday)
        {
            //! 新的一天开始了
            wDay = pCurrData->tm_mday;

            //! 开始清理组织系统信息
            GetOrganiCtrl()->OnNewDay(dwData);

            //! 重新计算星座日
            ResetStarDay(dwData);
            //! 发送重置玩家活力点通知
            CMessage msg(MSG_W2S_OTHER_NEW_DAY);
            msg.Add(dwData);
            msg.SendToAllGS();
        }
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetStatckFileName(),"MainLoop() End.");
#endif

    return true;
}

// 读取LoadLoginGMList.dat
bool CGame::LoadLoginGMList(const char* filename)
{
    GetLoginGMListVec().clear();

    string strTemp;
    CRFile* prfile = rfOpen(filename);
    if( prfile )
    {
        stringstream stream;
        prfile->ReadToStream(stream);
        rfClose(prfile);

        extern long g_lTotalMonster;
        long lM=0;

        // 读账号信息
        string strCdkey;
        while(ReadTo(stream, "#"))
        {
            stream >> strCdkey;
            GetLoginGMListVec().push_back(strCdkey);
        }
        Log4c::Trace(ROOT_MODULE,GetText("WS_SYS_11"));
    }
    else
    {
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SYS_12", filename));
        return false;
    }

    return true;
}
bool CGame::CheckCdkeyIsExsitInLoginGMList(const char* szCdkey)
{
    vector<string>::iterator itr = m_vecLoginGMList.begin();
    for(; itr != m_vecLoginGMList.end(); itr++)
    {
        if( stricmp((*itr).c_str(),szCdkey) == 0)
            return true;
    }
    return false;
}
// 判断玩家是否是GM
bool CGame::PlayerIsGM(CPlayer* pPlayer)
{
    if(pPlayer)
    {
        tagGMInfo* pInfo = GMList::GetInfoByName(pPlayer->GetName());
        if(pInfo)
            return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
// 角色
//////////////////////////////////////////////////////////////////////////
void CGame::AppendMapPlayer(CPlayer *pPlayer)
{
    if(pPlayer==NULL)
        return;
    // 删除和恢复列表的pPlayer为NULL，仅有PlayerID,所以不用放在map里.

    // Fox@20081113 增加玩家, 这里要保留
    GetInst(CPlayerMan).AddPlayer(pPlayer);
    if (g_pMailManager)
    {
        g_pMailManager->RegisterCheckTime(pPlayer->GetExID(),GlobalSetup::GetSetup()->lMailDelayTime*1000);
    }

    map<CGUID, CPlayer*>::iterator it = m_mPlayer.find( pPlayer->GetExID() );

    if( it == m_mPlayer.end() || (*it).second == NULL )
    {
        // 插入
        m_mPlayer[pPlayer->GetExID()] = pPlayer;
        m_PlayerNameToGUID[pPlayer->GetName()]=pPlayer->GetExID();

        //加国家人数
        if(m_ContryPlayerNum.find(pPlayer->GetCountry()) != m_ContryPlayerNum.end() )
            m_ContryPlayerNum[pPlayer->GetCountry()]++;
    }
    else
    {
        Log4c::Trace(ROOT_MODULE,GetText("WS_SYS_13"));
    }
}

void CGame::DeleteMapPlayer(const CGUID& PlayerGuid)
{
    map<CGUID, CPlayer*>::iterator it = m_mPlayer.find(PlayerGuid);

    // 该玩家存在
    if( it != m_mPlayer.end() )
    {
        CPlayer *pOld = it->second;
        if( pOld != NULL )
        {
            //减国家人数
            if(m_ContryPlayerNum.find(pOld->GetCountry()) != m_ContryPlayerNum.end())
                m_ContryPlayerNum[pOld->GetCountry()]--;
            m_PlayerNameToGUID.erase(pOld->GetName());
            //玩家下线删除邮件
            GetMailManager()->DeleteMailOnPlayerExit(pOld);
            // Fox@20081113 删除玩家,可以删除
            GetInst(CPlayerMan).DelPlayer(pOld);

            if(GetNetHandler().IsServerReady(NF_DB_SERVER))
            {
                DelRawPlayer(pOld);
                pOld = NULL;
            }
            else
                PushSavePlayer(pOld);
        }
        m_mPlayer.erase(it);
    }
}

void CGame::ClearMapPlayer()
{
    for(map<CGUID, CPlayer*>::iterator it=m_mPlayer.begin(); it!=m_mPlayer.end(); )
    {
        CPlayer *pOld = it->second;
        SAFE_DELETE(pOld);
        it = m_mPlayer.erase(it);
    }
    m_ContryPlayerNum.clear();
}

bool CGame::IsNameExistInMapPlayer(LPCSTR szName)
{
    for(map<CGUID, CPlayer*>::iterator it=m_mPlayer.begin(); it!=m_mPlayer.end(); it++)
    {
        //在线玩家数据中有名字相同的
        if( !strcmpi(szName, it->second->GetName()) )
        {
            return true;
        }
    }
    return false;
}
CPlayer* CGame::GetMapPlayer(const char* playerName)
{
    if(NULL == playerName) return NULL;

    itSG it = m_PlayerNameToGUID.find(playerName);
    if( it != m_PlayerNameToGUID.end() )
    {
        map<CGUID, CPlayer*>::iterator itPlayer = m_mPlayer.find((*it).second);
        if(itPlayer != m_mPlayer.end())
            return (*itPlayer).second;
    }
    return NULL;
}

CPlayer* CGame::GetMapPlayer(const CGUID& PlayerGuid)
{
    CPlayer *pPlayer = NULL;
    map<CGUID, CPlayer*>::iterator it = m_mPlayer.find(PlayerGuid);

    // 该玩家存在
    if( it != m_mPlayer.end())
    {
        pPlayer = it->second;
    }
    return pPlayer;
}

CPlayer* CGame::CloneMapPlayer(const CGUID& PlayerGuid)
{
    CPlayer *pClonePlayer = NULL;
    map<CGUID, CPlayer*>::iterator it = m_mPlayer.find(PlayerGuid);

    // 该玩家存在
    if( it != m_mPlayer.end() )
    {
        pClonePlayer = new CPlayer;
        long lPos=0;
        vector<BYTE> vBuf;
        CPlayer* pPlayer = it->second;
        // code
        //pPlayer->AddToByteArrayToDB(&vBuf);
        pPlayer->AddToByteArray(&vBuf);
        // decord
        if( !pClonePlayer->DecordFromByteArray(&vBuf[0],lPos) ) {
            SAFE_DELETE(pClonePlayer);
        }
    }
    return pClonePlayer;
}

BYTE CGame::GetCreationPlayerCountInCdkey(LPCSTR szCdkey)
{
    _ASSERT(szCdkey);
    BYTE res = 0;
    //CDBEntityManager::tagLoginInfo* tLoginInfo = GetEntityManager()->FindLoginPlayerByCdkey(szCdkey);
    //if(tLoginInfo)
    //	res = tLoginInfo->setLoginGuid.size();
    return res;
}
//time_t CGame::GetDeletionPlayerTime(const CGUID& PlayerGuid)
//{
//	time_t t = 0;
//
//	CDBLoginPlayer* lPlayer = GetEntityManager()->FindLoginPlayer(PlayerGuid);
//	if(lPlayer != NULL)
//	{
//		t = lPlayer->GetDelTime();
//	}
//
//	return t;
//}

const CGUID& CGame::GetOnlinePlayerIDByName(LPCSTR szName)
{
    for(map<CGUID, CPlayer*>::iterator itMap=m_mPlayer.begin(); itMap!=m_mPlayer.end(); ++itMap)
    {
        // 相同
        if( !strcmpi(itMap->second->GetName(), szName) )
        {
            Account* tLoginInfo = GetEntityManager()->FindAccount(itMap->second->GetAccount());
            if(!tLoginInfo)
            {
                return NULL_GUID;
            }

            if ( (PLAYER_STATE_ENTERED_GAME == tLoginInfo->GetPlayerState() || 
                PLAYER_STATE_CHANGING_SERVER == tLoginInfo->GetPlayerState()
                )
                )
            {
                return itMap->second->GetExID();
            }
        }
    }

    return NULL_GUID;
}
const CGUID& CGame::GetPlayerIDByName(LPCSTR szName)
{
    itSG it = m_PlayerNameToGUID.find(szName);
    if( it != m_PlayerNameToGUID.end() )
    {
        return (*it).second;
    }
    return NULL_GUID;

}
void CGame::ProcessTimeOutLoginPlayer()
{
    DWORD now = timeGetTime();
    for(map<CGUID, CPlayer*>::iterator it=m_mPlayer.begin(); it!=m_mPlayer.end(); it++)
    {
        if(it->second) // 客户端连接GS超时
        {
            Account* tLoginInfo = GetEntityManager()->FindAccount(it->second->GetAccount());
            if(!tLoginInfo)
                continue;

            if( tLoginInfo->GetPlayerState() == PLAYER_STATE_LOADED_CHAR_DATA )
            {
                // 如果登录玩家超时，将提出他。
                if( now - it->second->GetLoginTime() > GetNetHandler().GetSetup().releaseLoginPlayerTime )
                {
                    // 通知LoginServer
                    CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
                    msg.Add(const_cast<char*>(it->second->GetAccount()));//strAccount.c_str()));
                    msg.Add(0L);
                    msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver

                    // 离队处理 soft 2006-10-23	
                    tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
                }
            }
        }
    }
}

bool CGame::GenerateDBPlayerID(IN CGUID& guid)
{
    return CGUID::CreateGUID(guid);
}

//断开所有玩家
void CGame::KickAllPlayer()
{
    CMessage msg(MSG_W2S_OTHER_KICKALL);
    msg.SendToAllGS();
    m_bIsKickPlayerFlag = true;
}


bool CGame::IsNameExistInDBData(LPCSTR szName)
{
    if(!szName) return true;
    bool res = false;
    return res;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

//添加一个物品超链
void CGame::AddGoodsLink(tagGoodsLink* pstGoodsLink)
{
    static DWORD dwCnt=1;
    if(m_listGoodsLink.size()==m_listGoodsLink.max_size())
    {
        //list达最大容量,则弹出首元素
        m_listGoodsLink.pop_front();
    }
    if(!pstGoodsLink->bChange)
    {
        pstGoodsLink->dwIndex=dwCnt;
        dwCnt++;
    }
    m_listGoodsLink.push_back(*pstGoodsLink);
}

//根据ID查找一个物品超链
CGame::tagGoodsLink* CGame::FindGoodsLink(DWORD dwID)
{
    list<tagGoodsLink>::iterator GoodsLinkIter=m_listGoodsLink.begin();
    for(;GoodsLinkIter!=m_listGoodsLink.end();GoodsLinkIter++)
    {
        if(dwID==GoodsLinkIter->dwIndex)
        {
            return &(*GoodsLinkIter);
        }
    }
    return NULL;
}

// 当某个GameServer断开时
bool CGame::OnGameServerLost(DWORD dwIndex)
{
    set<CGUID> setRemovePlayer;
    set<long>  setRegionID;
    // 记录该GS上所有场景TemplateID
    // 需要清除的Player
    for (map<CGUID, CPlayer*>::iterator it=m_mPlayer.begin(); it!=m_mPlayer.end(); it++)
    {
        CPlayer* pPlayer = it->second;
        if(pPlayer)
        {
            if( GetGameServerNumber_ByRegionGUID(pPlayer->GetRegionExID()) == dwIndex)
                setRemovePlayer.insert( pPlayer->GetExID() );
        }
    }

    // 清除个人副本数据
    CGlobalRgnManager::MapRgnItr pRgnItr = GetGlobalRgnManager()->GetRgnMap().begin();
    while(pRgnItr != GetGlobalRgnManager()->GetRgnMap().end())
    {
        if(pRgnItr->second->GetRgnType() != RGN_NORMAL && pRgnItr->second->GetGsid() == dwIndex)
        {
            SAFE_DELETE(pRgnItr->second);
            pRgnItr = GetGlobalRgnManager()->GetRgnMap().erase(pRgnItr);
        }
        else
            pRgnItr++;
    }



    set<CGUID>::iterator it;
    // 发消息通知LoginServer
    CMessage msg(MSG_W2L_SERVER_CLEARCDKEYS);
    msg.Add((long)setRemovePlayer.size());
    for (it=setRemovePlayer.begin(); it!=setRemovePlayer.end(); it++)
    {
        CPlayer *pPlayer = GetMapPlayer( *it );


        if(GetNetHandler().IsServerReady(NF_DB_SERVER))
        {
            // 发送DBS存盘消息
            CWorldServerSession session(5000);

            // 设置当前账号状态
            Account* pAcc = NULL;
            CEntityGroup* tDBLoginPlayer = NULL;
            if(pPlayer->GetAccount())
                pAcc = GetEntityManager()->FindAccount(pPlayer->GetAccount());
            if(pAcc)
            {
                if(pAcc->GetPlayerState() != PLAYER_STATE_VOID)
                    pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
                pAcc->SetCurGamedPlayerGuid(NULL_GUID);
                tDBLoginPlayer = pAcc->FindLoginPlayer(pPlayer->GetExID());
                session.NakeSendSaveLoginPlayerDataMsg(pPlayer, tDBLoginPlayer);
            }
            else
            {
                tDBLoginPlayer = (CEntityGroup*)GetEntityManager()->NewBaseEntity(string("[loginplayer]"), pPlayer->GetExID());
                session.NakeSendSaveLoginPlayerDataMsg(pPlayer, tDBLoginPlayer);
                GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBLoginPlayer);
            }

            CEntityGroup* tDBPlayer = (CEntityGroup*)GetEntityManager()->NewBaseEntity(string("[player]"), pPlayer->GetExID());
            session.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, pPlayer, tDBPlayer, true);
            GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBPlayer);			
        }
        msg.Add(const_cast<char*>(pPlayer->GetAccount()));//m_AppendProperty.strAccount.c_str()));

        OnPlayerQuit(pPlayer,dwIndex, 0L);
    }	
    msg.SendToServer(NF_LOGIN_SERVER);//send the cdkeys to loginserver

    return true;
}

GSConnection* CGame::GetRegionGameServer(long lRegionID)
{
    CGlobalRgnManager::tagRegion* pRegion = GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
    if (pRegion==NULL)
    {
        return NULL;
    }

    return GetNetHandler().GetGS(pRegion->dwGameServerIndex);
}

// 获取region所在的GameServer
GSConnection* CGame::GetRegionGameServer(const CGUID& RegionID)
{
    long lGSID = GetGlobalRgnManager()->FindGSIDByRgnID(RegionID);
    return GetNetHandler().GetGS(lGSID);
}

// 获取player所在的GameServer
GSConnection* CGame::GetPlayerGameServer(const CGUID& PlayerGuid)
{
    CPlayer* pPlayer = GetMapPlayer(PlayerGuid);
    if (pPlayer==NULL)
        return NULL;

    return GetRegionGameServer(pPlayer->GetRegionExID());
}

long CGame::GetGameServerNumber_ByRegionID(long lRegionID)
{
    GSConnection* gs = GetRegionGameServer(lRegionID);
    if (!gs)
        return 0;

    return gs->index;
}

long CGame::GetGameServerNumber_ByRegionGUID(const CGUID& RegionGuid)
{
    GSConnection* gs = GetRegionGameServer(RegionGuid);
    if (!gs)
        return 0;

    return gs->index;
}

long CGame::GetGameServerNumber_ByPlayerID(const CGUID& PlayerGuid)
{
    GSConnection* gs = GetPlayerGameServer(PlayerGuid);
    if (!gs)
        return 0;

    return gs->index;
}

bool CGame::CheckInvalidString(string& str,bool bReplace)
{
    return CWordsFilter::GetInstance()->Check(str,bReplace);
}
// 响应玩家操作
bool CGame::OnPlayerEnter(CPlayer* pPlayer, Account* pAcc, long lSignCode, long lIP, long gsid, long lSocketIDC2S, long lMsgSocketID)
{
    if(!pPlayer || !pAcc) return false;
    // 重置验证码
    long lNewCode = 0;
    GenerateSignCodeByTime(lNewCode);
    pAcc->SetSignCode(lNewCode);
    char pszLogingInfo[1024];
    // 检测请求玩家数据是否是正确的GameServer
    if(pPlayer->GetCurRgnType() == RGN_NORMAL)
    {
        long lGameServerNum = GetGameServerNumber_ByRegionGUID(pPlayer->GetRegionExID());
        if (lGameServerNum != gsid)
        {
            CMessage msg(MSG_W2S_LOG_ANSWER_PLAYERDATA);
            msg.Add(lSignCode);
            msg.Add((long)-2);
            msg.Add(pPlayer->GetExID());
            msg.Add(lSocketIDC2S);
            msg.SendToSocket(lMsgSocketID);   //send to gameserver

            pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);

            char szGuid[40] = {0};
            pPlayer->GetExID().tostring(szGuid);
            _snprintf(pszLogingInfo,1024,AppFrame::GetText("WS_LOGMESSAGE_3"), szGuid);
            //PutStringToFile("Login_WS_Info",pszLogingInfo);
            Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_3", szGuid));
            return false;
        }
    }

    //
    /*_snprintf(pszLogingInfo,1024,AppFrame::GetText("WS_LOGMESSAGE_LOGINWSQUEST"),
    pPlayer->GetName(),pPlayer->GetRegionID(),pPlayer->GetPosX(),pPlayer->GetPosY());
    PutStringToFile("Login_WS_Info",pszLogingInfo);*/

    //! 绑定socketID
    pPlayer->SetGsSocket(lMsgSocketID);
    //! 绑定IP
    pPlayer->SetIP(lIP);

    pPlayer->ResetActive();
    //! 玩家数据发送前必须处理的步骤
    if (pAcc->GetPlayerState() != PLAYER_STATE_CHANGING_SERVER)
    {
        //! 记录日志
        GetLogicLogInterface()->logT550_client_in_game_log(pPlayer, 1);
        //! 通知组织玩家进入游戏
        GetOrganiCtrl()->OnPlayerEnterGame(pPlayer->GetExID());
    }

    return true;
}
bool CGame::OnPlayerEnterLateCode(CPlayer* pPlayer, Account* pAcc)
{
    if (pAcc->GetPlayerState() != PLAYER_STATE_CHANGING_SERVER)
    {
        CMessage msg(MSG_W2S_ORGSYS_InitPlayerFacSys);
        msg.Add(pPlayer->GetExID());
        msg.SendToSocket(pPlayer->GetGsSocket());

        GetOrganiCtrl()->OnPlayerEnterGameLaterDataSend(pPlayer->GetExID());

        LinkmanSystem::GetInstance().OnPlayerOnline(pPlayer);
    }
    else
    {
        //如果有队伍则发布队伍切换消息
        const CGUID& TeamID  = GetOrganiCtrl()->FindPlayerTeamID(pPlayer->GetExID());
        if (NULL_GUID != TeamID)
        {
            CMessage teamMsg(MSG_W2S_TEAM_ServerChange);
            teamMsg.Add(TeamID);
            teamMsg.Add(pPlayer->GetExID());
            teamMsg.SendToAllGS();
        }
    }
    return true;
}
bool	CGame::OnPlayerQuit(CPlayer* pPlayer, long gsid, DWORD dwServerKey[], bool bIsChangingGs)
{
    if(!pPlayer) return false;
    //! 记录日志
    GetLogicLogInterface()->logT550_client_in_game_log(pPlayer, 2);

    //! 通知好友系统
    LinkmanSystem::GetInstance().OnPlayerOffline(pPlayer);
    //! 通知组织玩家退出
    GetOrganiCtrl()->OnPlayerExitGame(pPlayer, bIsChangingGs);

    // 通知LoginServer
    CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
    msg.Add(const_cast<char*>(pPlayer->GetAccount()));
    for (LONG i = 0; i < SERVER_KEY_DWORD_SIZE; ++i)
    {
        if(NULL == dwServerKey)
            msg.Add((DWORD)0);
        else
            msg.Add(dwServerKey[i]);
    }
    msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver

    GetGame().DeleteMapPlayer(pPlayer->GetExID());

    // gs玩家数减一
    GSConnection* pGameServer = GetNetHandler().GetGS(gsid);
    if (pGameServer && CMessage::IsGSReady(gsid))
    {
        --pGameServer->playerNum;
    }
    return true;
}
// 从player身上获取装备信息
void CGame::GetPlayerEquipID(CPlayer* pPlayer, DWORD& dwHead, DWORD& dwNecklace, DWORD& dwWing, DWORD& dwBody, 
                             DWORD& dwBack, DWORD& dwGlove, DWORD& dwShoulder, DWORD& dwWrist, DWORD& dwLeg, 
                             DWORD& dwBoot, DWORD& dwLring,DWORD& dwRring, DWORD& dwWeapon, DWORD& dwCard, 
                             DWORD& dwGodGoods, DWORD& dwEffigy,DWORD& dwLifeCowry,DWORD& dwSoulGuard, 
                             BYTE& nHeadLevel, BYTE& nNecklaceLevel, BYTE& nWingLevel, BYTE& nBodyLevel, 
                             BYTE& nBackLevel, BYTE& nGloveLevel, BYTE& nShoulderLevel, BYTE& nwWristLevel, 
                             BYTE& nLegLevel, BYTE& nBootLevel, BYTE& nLringLevel, BYTE& nRringLevel, 
                             BYTE& nWeaponLevel, BYTE& nCardLevel, BYTE& nGodGoodsLevel, BYTE& nEffigyLevel,
                             BYTE& nLifeCowryLevel,BYTE& nSoulGuardLevel)
{
    // 头盔
    //CGoods* pGoods = pPlayer->m_cEquipment.GetEquipment( CEquipmentContainer::EC_HEAD );
    CGoods* pGoods = pPlayer->m_cEquipment.GetGoods(DWORD(EC_E_HEAD ) );
    if( pGoods )
    {
        dwHead		= pGoods->GetBasePropertiesIndex();
        //nHelmLevel	= pGoods->GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
    }
    else
    {
        dwHead		= 0;
        nHeadLevel	= 0;
    }

    // 盔甲
    //pGoods = pPlayer->m_cEquipment.GetEquipment( CEquipmentContainer::EC_BODY );
    pGoods= pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_NECKLACE );
    if( pGoods )
    {
        dwNecklace		= pGoods->GetBasePropertiesIndex();
        //nBodyLevel	= pGoods->GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
    }
    else
    {
        dwNecklace		= 0;
        nNecklaceLevel	= 0;
    }
    // 翅膀
    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_WING );
    if( pGoods )
    {
        dwWing		= pGoods->GetBasePropertiesIndex();
        //nWingLevel	= pGoods->GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
    }
    else
    {
        dwWing		= 0;
        nWingLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD)EC_E_BODY );
    if( pGoods )
    {
        dwBody		= pGoods->GetBasePropertiesIndex();
        //nManteauLevel	= pGoods->GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
    }
    else
    {
        dwBody		= 0;
        nBodyLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD)EC_E_BACK );
    if( pGoods )
    {
        dwBack		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwBack		= 0;
        nBackLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_HAND );
    if( pGoods )
    {
        dwGlove		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwGlove		= 0;
        nGloveLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_SHOULDER );
    if( pGoods )
    {
        dwShoulder		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwShoulder		= 0;
        nShoulderLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_WRIST );
    if( pGoods )
    {
        dwWrist		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwWrist		    = 0;
        nwWristLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_LEG );
    if( pGoods )
    {
        dwLeg		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwLeg		    = 0;
        nLegLevel	    = 0;
    }
    // 靴子
    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_BOOT );
    if( pGoods )
    {
        dwBoot		= pGoods->GetBasePropertiesIndex();
        //nBootLevel	= pGoods->GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
    }
    else
    {
        dwBoot		= 0;
        nBootLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_LRING );
    if( pGoods )
    {
        dwLring		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwLring		= 0;
        nLringLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_RRING );
    if( pGoods )
    {
        dwRring		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwRring		= 0;
        nRringLevel	= 0;
    }


    // 武器
    //	pGoods = pPlayer->m_cEquipment.GetEquipment( CEquipmentContainer::EC_HAND );
    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_WEAPON );
    if( pGoods )
    {
        dwWeapon	= pGoods->GetBasePropertiesIndex();
        //nWeaponLevel= pGoods->GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
    }
    else
    {
        dwWeapon	= 0;
        nWeaponLevel= 0;
    }

    // 头饰
    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_CARD );
    if( pGoods )
    {
        dwCard		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwCard		= 0;
        nCardLevel	= 0;
    }

    // 外衣
    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_GODGOODS );
    if( pGoods )
    {
        dwGodGoods		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwGodGoods		= 0;
        nGodGoodsLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_EFFIGY);
    if( pGoods )
    {
        dwEffigy = pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwEffigy		= 0;
        nEffigyLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_LIFECOWRY);
    if( pGoods )
    {
        dwLifeCowry = pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwLifeCowry		= 0;
        nLifeCowryLevel	= 0;
    }

    pGoods = pPlayer->m_cEquipment.GetGoods((DWORD) EC_E_SOULGUARD );
    if( pGoods )
    {
        dwSoulGuard		= pGoods->GetBasePropertiesIndex();
    }
    else
    {
        dwSoulGuard		= 0;
        nSoulGuardLevel	= 0;
    }
}
//为玩家初始化技能
void CGame::InitSkillToPlayer(CPlayer* pPlayer)
{
    list<long> lInitSkill = CNewSkillFactory::GetInitSkillList();
    list<long> lOccuSkill = CNewSkillFactory::GetOccuSkillList(pPlayer->GetOccupation());
    while (lOccuSkill.size()>0)
    {
        lInitSkill.push_back(lOccuSkill.front());
        lOccuSkill.pop_front();
    }	

    for (list<long>::iterator it=lInitSkill.begin(); it!=lInitSkill.end(); ++it)
    {		
        DWORD dwID = (*it);
        WORD wLevel = 1;
        WORD wCooldown = 0;
        pPlayer->AddSkill(dwID,wLevel,wCooldown);	
    }	
}
// 为玩家添加初始物品装备
void CGame::AddOrginGoodsToPlayer(CPlayer* pPlayer)
{
    if (pPlayer==NULL)
        return;

    for (list<PlayerList::tagOrginEquip>::iterator it=PlayerList::GetOrginEquipList()->begin(); it!=PlayerList::GetOrginEquipList()->end(); it++)
    {
        PlayerList::tagOrginEquip *pst = &(*it);
        if (pPlayer->GetOccupation() == pst->lOccupation )
        {

#ifdef __MEMORY_LEAK_CHECK__
            CBaseObject::SetConstructFlag(31);
#endif
            CGoods* pGoods = //CGoodsList::CreateNewGood(pst->strOrginName.c_str());
                CGoodsFactory::CreateGoods( 
                GoodsSetup::QueryGoodsIDByOriginalName( pst->strOrginName.c_str() ),25 );
            if (pGoods)
            {
                //AddGoodsID(0);
                //long lID =	GetGoodsID(0);
                CGUID goodsid;
                CGUID::CreateGUID(goodsid);
                pGoods->SetExID(goodsid);

                if( pPlayer->m_cEquipment.Add( static_cast<eEquipType>
                    (pst->wPlacePos), pGoods ) == FALSE)
                {
                    assert(0);
                    OutputDebugString( "Failed to add equipment" );
                }
                //pPlayer->SetEquip(pst->wPlacePos, pGoods);
            }
        }
    }
}
void CGame::DoSaveData(bool isSavePlayerFlag)
{
    // 将CDBEntity对象发送到DBS
    Log4c::Trace(ROOT_MODULE,GetText("WS_SYS_14"));
    CGUID pGUID;
    long playersize = 0;

    if(isSavePlayerFlag)
    {
        map<CGUID, CPlayer*>::iterator enItr = m_mPlayer.begin();
        for(;enItr != m_mPlayer.end();++enItr)
        {
            // 发送个人房屋数据
            //GetEntityManager()->CreateSavePhRgnSession(enItr->second->GetExID());
            CPlayer* pPlayer = enItr->second;
            if(pPlayer)
            {
                // 发送账号数据
                Account* pAcc = GetEntityManager()->FindAccount(pPlayer->GetAccount());
                GetEntityManager()->CreateSaveAccountSession(pAcc);
                // 发送个人数据
                GetEntityManager()->CreateSavePlayerDataSession(enItr->first, SAVE_DETAIL_ALL);
                playersize++;
            }
        }
        Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_15", playersize));
    }

    //! 保存工会数据
    GetOrganiCtrl()->SaveAllFaction();
    //! 保存联盟数据
    GetOrganiCtrl()->SaveAllUnion();

    // 保存全局变量数据
    if(GetInitVarLoadFlag())
    {
        CEntityManager::DBEntityComponentMapItr varMapItr = GetEntityManager()->GetBaseEntityMap().find(string("[scriptvargroup]"));
        if(varMapItr == GetEntityManager()->GetBaseEntityMap().end())
        {
            Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_16"));
            return;
        }
        if(varMapItr->second.size() == 0)
        {
            Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_17"));
            return;
        }
        map<CGUID, CEntityGroup*>::iterator enItr = varMapItr->second.begin();
        if(enItr == varMapItr->second.end())
        {
            Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_18"));
            return;
        }
        CEntityGroup* pVarGroup = (CEntityGroup*)enItr->second;
        if(pVarGroup)
        {
            pVarGroup->ReleaseChilds();
            GetEntityManager()->CreateSaveGlobalVarSession();
            Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_19", pVarGroup->GetEntityGroupMap().size()));
        }
    }

    Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_20"));

    CMessage pEndMsg(MSG_W2DB_SERVER_SAVEDATAEND);
    pEndMsg.Add((BYTE)0); // 定时保存标志
    pEndMsg.SendToServer(NF_DB_SERVER);
}


// 游戏主线程
unsigned __stdcall GameThreadFunc(void* pArguments)
{
#ifndef _DEBUG
    __try
    {
#endif
        bool res = GetGame().Init() ? true : false ;
        AppFrame::ToggleMenus();

        while (res)
        {
            // 结束线程
            if (AppFrame::IsExit() || !GetGame().MainLoop())
            {
                break;
            }
        }

        GetGame().Release();

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

bool CGame::ValidateCreatioPlayerGuidinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid)
{

    return false;
}

bool CGame::ValidatePlayerIDinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid)
{
    return false;
}

unsigned __stdcall ProcessWriteLogDataFunc(void *pArguments)
{
    return 0;
}

// 将刚从DB读取的Player相关数据初始化
void CGame::InitPlayerInfo(CPlayer* pPlayer, char* szCdkey)
{
    if(pPlayer == NULL) 
    {
        CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);
        // 添加验证代码
        msg.Add((long)0xFFFFFFFF);
        msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
        msg.Add(szCdkey);
        msg.SendToServer(NF_LOGIN_SERVER);
        return;
    }

    CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);

    Account* tLoginInfo = GetEntityManager()->FindAccount(szCdkey);
    if(!tLoginInfo)
    {
        DeleteMapPlayer(pPlayer->GetExID());
        // 添加验证代码
        msg.Add((long)0xFFFFFFFF);
        msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
        msg.Add(szCdkey);
        msg.SendToServer(NF_LOGIN_SERVER);
        return;

    }
    if(tLoginInfo)
    {
        // 创建验证码
        long lSignCode = 0;
        GenerateSignCodeByTime(lSignCode);
        tLoginInfo->SetSignCode(lSignCode);
        // 添加验证代码
        msg.Add((long)lSignCode);
    }

    long lRegion = 0;
    long l = (long)pPlayer->GetPosX();
    long t = (long)pPlayer->GetPosY();
    long r=0;
    long b=0;
    long d=0;
    CGlobalRgnManager::tagRegion* pstRegion = NULL;

    long lRegionID = pPlayer->GetRegionID();
    CWorldRegion* pRegion = GetGlobalRgnManager()->FindRgnByGUID(pPlayer->GetRegionExID());
    if(pRegion && pRegion->GetID() != lRegionID)
    {
        char szGuid[128];
        pPlayer->GetRegionExID().tostring(szGuid);
        Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_21", szCdkey, szGuid, lRegionID));
        pPlayer->SetRegionID(pRegion->GetID());
        pPlayer->SetRegionExID(pRegion->GetExID());
    }

    if(!pRegion)// 判断是否被关掉 或 副本被删掉
    {
        // 先检查TemplateID，判断是否为副本
        pstRegion = GetGlobalRgnManager()->GetTemplatePersonalRegion(lRegionID);
        if(!pstRegion)
            pstRegion = GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(lRegionID);
        if(!pstRegion)
            pstRegion = GetGlobalRgnManager()->GetTemplateTeamRegion(lRegionID);
        if(pstRegion)// 副本被删掉
        {
            long dupRgnID = 0;
            if(pstRegion && pstRegion->pRegion)
            {
                pRegion = pstRegion->pRegion;
            }
            if(pRegion && pRegion->GetSetup().bDupRgnIsUsed)
                pRegion->GetDupRgnReturnPoint(pPlayer, dupRgnID, l, t, r, b, d);

            if(dupRgnID > 0)
            {
                // 只能返回到NormalRgn
                CGlobalRgnManager::tagRegion* pNewStRegion = GetGlobalRgnManager()->GetTemplateNormalRegion(dupRgnID);
                if(pNewStRegion)
                    pRegion = pNewStRegion->pRegion;
                else
                {
                    Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_22",szCdkey, dupRgnID));
                }
            }
            else if(dupRgnID == -1) // 回国都
            {
                long lCityRegion = GetCountryParam()->m_mpMainRegions[pPlayer->GetCountry()];
                CGlobalRgnManager::tagRegion* pstCityRegion = GetGlobalRgnManager()->GetTemplateNormalRegion(lCityRegion);
                if(!pstCityRegion || !pstCityRegion->pRegion)
                {
                    Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_23",szCdkey, lCityRegion));

                    msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
                    msg.Add(szCdkey);
                    msg.SendToServer(NF_LOGIN_SERVER);
                    tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
                    return;
                }
                l = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].left;
                t = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].top;
                r = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].right;
                b = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].bottom;
                pRegion = pstCityRegion->pRegion;
            }
            else if(dupRgnID == 0) // 非指定地图
            {
                char szPlayerID[128];
                pPlayer->GetExID().tostring(szPlayerID);
                //char pszLogingInfo[512];
               // _snprintf(pszLogingInfo,512, "InitPlayerInfo() pPlayer[%s]副本[%d]返回地图ID为0!", szCdkey, lRegionID);
                //PutStringToFile("Login_WS_Info",pszLogingInfo);
                Log4c::Trace(ROOT_MODULE,"InitPlayerInfo() pPlayer[%s]副本[%d]返回地图ID为0!", szCdkey, lRegionID);
                DeleteMapPlayer(pPlayer->GetExID());

                msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
                msg.Add(szCdkey);
                msg.SendToServer(NF_LOGIN_SERVER);
                tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
                return;
            }
        }
        else
        {
            // 再判断是否为普通场景
            pstRegion = GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
            if(!pstRegion) // 普通场景或副本场景被关掉
            {
                map<long, CRegionPlugSetup::tagRegionPlugSetup>::iterator rgnItr = CRegionPlugSetup::GetRegionPlugSetupMap().find(lRegionID);
                if(rgnItr != CRegionPlugSetup::GetRegionPlugSetupMap().end())
                {
                    if(rgnItr->second.bType == 0) // 回国都传送
                    {
                        lRegion = GetCountryParam()->m_mpMainRegions[pPlayer->GetCountry()];
                        CGlobalRgnManager::tagRegion* pstPlugRegion = GetGlobalRgnManager()->GetTemplateNormalRegion(lRegion);
                        if(!pstPlugRegion || !pstPlugRegion->pRegion)
                        {
                            Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_24",szCdkey, lRegion));

                            msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
                            msg.Add(szCdkey);
                            msg.SendToServer(NF_LOGIN_SERVER);
                            tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
                            return;
                        }
                        pRegion = pstPlugRegion->pRegion;
                        pstRegion = GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
                        l = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].left;
                        t = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].top;
                        r = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].right;
                        b = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].bottom;
                    }
                    else // 指定地图传送
                    {
                        pstRegion = GetGlobalRgnManager()->GetTemplateNormalRegion(rgnItr->second.lRegionID);
                        if(pstRegion)
                            pRegion = pstRegion->pRegion;
                        else
                        {
                            Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_25",szCdkey, rgnItr->second.lRegionID));
                        }
                    }

                    if(!pRegion)
                    {
                        msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
                        msg.Add(szCdkey);
                        msg.SendToServer(NF_LOGIN_SERVER);
                        tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
                        return;
                    }
                }
            }
            else
            {
                pRegion = pstRegion->pRegion;
            }
        }
    }
    if(pRegion == NULL)
    {
        char pszLogingInfo[512];
        _snprintf(pszLogingInfo,512, "Err InitPlayerInfo: player[%s], pRegion[%d] Is Null!", szCdkey, lRegionID);
        //PutStringToFile("Login_WS_Info",pszLogingInfo);
        Log4c::Trace(ROOT_MODULE,pszLogingInfo);

        DeleteMapPlayer(pPlayer->GetExID());

        msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
        msg.Add(szCdkey);
        msg.SendToServer(NF_LOGIN_SERVER);
        return;
    }

    pPlayer->SetPosXY((float)l, (float)t);
    pPlayer->SetRegionID(pRegion->GetID());
    pPlayer->SetRegionExID(pRegion->GetExID());
    pPlayer->SetCurRgnType((eRgnType)pRegion->GetRgnType());

    pPlayer->SetPersonalRgnGUID(GetGlobalRgnManager()->GetPersonalRgnGUID(pPlayer->GetExID()));
    if(pPlayer->GetPersonalRgnGUID() != NULL_GUID)
        pPlayer->SetCurPRgnNum(1);

    // GameServer没有连接
    GSConnection* pGameServer = GetNetHandler().GetGS(pRegion->GetGsid());
    if (!pGameServer || !CMessage::IsGSReady(pRegion->GetGsid()))
    {
       // char pszLogingInfo[512];
       // _snprintf(pszLogingInfo,512, AppFrame::GetText("GAME_LOAD_QUESTPROPERTYBYGSFAIL"), pRegion->GetGsid());
        //PutStringToFile("Login_WS_Info",pszLogingInfo);
       Log4c::Trace(ROOT_MODULE,FormatText("GAME_LOAD_QUESTPROPERTYBYGSFAIL", pRegion->GetGsid()));

        DeleteMapPlayer(pPlayer->GetExID());

        msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
        msg.Add(szCdkey);
        msg.SendToServer(NF_LOGIN_SERVER);
        tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
        return;
    }
    else
    {
        // 玩家数加一
        ++pGameServer->playerNum;
    }

    //	取得gameserver的ip和port
    string strGsIp;
    long lGsPort;
    strGsIp=pGameServer->con.ip;
    lGsPort=pGameServer->con.port;

    if(tLoginInfo)
        tLoginInfo->SetCurGamedPlayerGuid(pPlayer->GetExID());

    // 上线成功
    msg.Add((char)LOGIN_QUEST_PLAYERDATA_SUCCEED);
    msg.Add(szCdkey);
    msg.Add(const_cast<char*>(strGsIp.c_str()));
    msg.Add(lGsPort);
    // 发送角色名，日志需要
    msg.Add(const_cast<char*>(pPlayer->GetName ()));
    // 发送角色等级，日志需要
    msg.Add(pPlayer->GetLevel());
    msg.SendToServer(NF_LOGIN_SERVER);
    pPlayer->SetPlayerLogin(true);
    pPlayer->SetFirstLogin(true);
    pPlayer->SetLoginTime(timeGetTime());
    // 设置玩家状态
    tLoginInfo->SetPlayerState(PLAYER_STATE_ENTERING_GAME);
    tLoginInfo->SetLoginTimeoutValue(timeGetTime());
}

// 设置定时器
void CGame::SetMsgTimer(eMsgTYpe msg, CMsgTimer* msgTime)
{
    DWORD curTime = timeGetTime();
    msgTime->SetMsgType(msg);
    curTime += 5000;
    GetTimerQueue()->Schedule(msgTime, NULL, curTime);
}

// 产生MSGID
long CGame::AllocMsgID(void) 
{ 
    if(m_listFreeMsgID.size())
    {
        long id = *(m_listFreeMsgID.begin());
        m_listFreeMsgID.erase(m_listFreeMsgID.begin());
        return id;
    }
    return ++m_CurMsgID;
}

// 释放一个MSGID
void CGame::FreeMsgID(long id)
{
    std::list<long>::iterator itr = std::find(GetFreeMsgIDList().begin(), GetFreeMsgIDList().end(), id);
    // 未找到该ID并且ID小于最大值
    if( (itr == GetFreeMsgIDList().end()) && (id < MAX_MSG_TIME_NUM) ) 
        GetFreeMsgIDList().push_back(id);
}
// 取得MsgTimer
CMsgTimer* CGame::GetMsgTimer(long id)
{
    CMsgTimer* res = NULL;
    mapMsgTimerItr itr = m_mapMsgTimer.find(id);
    if(itr != m_mapMsgTimer.end())
        res = itr->second;

    return res;
}

// 取得一个未使用的MsgTime对象
CMsgTimer* CGame::AllocFreeMsgTimer(void)
{
    CMsgTimer* msgTime = NULL;
    // 先从空闲列表中取得
    if(m_listFreeMsgTimer.size())
    {
        msgTime = (*m_listFreeMsgTimer.begin());
        m_listFreeMsgTimer.erase(m_listFreeMsgTimer.begin());
    }
    else // 新创建一个对象
    {
        msgTime = new CMsgTimer;
    }

    if(msgTime)
    {
        long id = AllocMsgID();
        m_mapMsgTimer[id] = msgTime;
        msgTime->SetID(id);
    }

    return msgTime;
}

void CGame::InitCountryPlayerNum()
{
    m_ContryPlayerNum[COUNTRY_STAR] = 0;
    m_ContryPlayerNum[COUNTRY_SEA] = 0;
    m_ContryPlayerNum[COUNTRY_CLOUD] = 0;
    m_ContryPlayerNum[COUNTRY_LUNA] = 0;
    m_ContryPlayerNum[COUNTRY_SAND] = 0;
    m_ContryPlayerNum[COUNTRY_WIND] = 0;
}

//得到最少人数的3个国家,通过BYTE中0到5位来确定
BYTE CGame::GetCountryLessPlayer()
{
    static BYTE byteCountrys=0xFF;
    static DWORD dwLastTime = timeGetTime();
    DWORD dwCurTime = timeGetTime();
    //2分钟重设一次
    if(byteCountrys == 0xFF || 
        dwCurTime-dwLastTime > 120000)
    {
        dwLastTime = dwCurTime;
        byteCountrys=0x0;
        multimap<long,BYTE>	OrderCountry;OrderCountry.clear();
        map<BYTE,long>::iterator it = m_ContryPlayerNum.begin();
        for(;it != m_ContryPlayerNum.end();it++)
        {
            OrderCountry.insert(make_pair<long,BYTE>((*it).second,(*it).first));
        }
        multimap<long,BYTE>::iterator itOrder = OrderCountry.begin();
        long lNum = 0;
        for(;lNum < 3 && itOrder!=OrderCountry.end() ;lNum++)
        {
            byteCountrys |= (1<<(*itOrder).second );
            itOrder++;
        }
    }
    return byteCountrys;
}

// 删除定时器
void CGame::DelMsgTimer(CMsgTimer* msgTime)
{
    GetTimerQueue()->Cancel(msgTime);
}

void CGame::DelMsgTimer(long msgID)
{
    // 清除多余空闲MSG
    if((long)m_listFreeMsgTimer.size() > MAX_MSG_TIME_NUM)
    {
        std::list<CMsgTimer*>::iterator itr = m_listFreeMsgTimer.begin();
        while((long)m_listFreeMsgTimer.size() > MAX_MSG_TIME_NUM)
        {
            CMsgTimer* msgTimer = *m_listFreeMsgTimer.begin();
            if(msgTimer)
                SAFE_DELETE(msgTimer);
            m_listFreeMsgTimer.erase(m_listFreeMsgTimer.begin());
        }
    }

    CMsgTimer* msgTime = GetMsgTimer(msgID);
    if(msgTime)
    {
        DelMsgTimer(msgTime);
        m_mapMsgTimer.erase(msgID);	
        if(msgID >= MAX_MSG_TIME_NUM) // 超过最大限制
        {
            SAFE_DELETE(msgTime);
            return;
        }
        m_listFreeMsgTimer.push_back(msgTime);
        FreeMsgID(msgID);
    }
}

void CGame::ReleaseMsgTimerMap()
{
    mapMsgTimerItr itr = m_mapMsgTimer.begin();
    for(; itr != m_mapMsgTimer.end(); itr++)
    {
        if(itr->second)
            SAFE_DELETE(itr->second);
    }
    m_mapMsgTimer.clear();

    list<CMsgTimer*>::iterator fitr = m_listFreeMsgTimer.begin();
    for(; fitr != m_listFreeMsgTimer.end(); fitr++)
    {
        if(*fitr)
            SAFE_DELETE(*fitr);
    }
    m_listFreeMsgTimer.clear();
}

void CGame::InitMsgTimerMap()
{
    for(long i = 0; i < MAX_MSG_TIME_NUM; i++)
    {
        CMsgTimer* msgTime = new CMsgTimer;
        m_listFreeMsgTimer.push_back(msgTime);
    }

}

//超时响应
long CMsgTimer::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
    switch(m_DBSMsgType)
    {	
    case MSG_W2S_SERVER_BUSINESS_UPDATE:
        {
            //
            CMessage msg(MSG_W2S_SERVER_BUSINESS_UPDATE);
            msg.SendToAllGS();
        }
        break;
    }
    if(timerid == m_lTimerID)
        OnMsgTimeout(timerid);
    return 1;
}
//当定时器被取消的调用
void CMsgTimer::OnTimerCancel(DWORD timerid,const void* var)
{

}

void CGame::SendGMBanPlayerToDBSMsg(char* strPlayerName, long time)
{
    GetEntityManager()->CreateBanPlayerSession(strPlayerName, time);
}

long CGame::GetNumTeams()
{
    return GetOrganiCtrl()->GetGameTeamNum();
}


//!			序列化物品扩展配置
bool		CGame::SerializeGoodsExSetup(vector<BYTE> &vecData)
{
    return m_pGoodsExSetup->Serialize(&vecData);
}





// 发送MSG_W2DB_SERVER_UNREGENTITY 清除DBS管理的对象
void CGame::SendUnRegDBEntity(const char* szCdkey)
{
    if(szCdkey)
    {
        CMessage msg(MSG_W2DB_SERVER_UNREGENTITY);
        msg.Add(szCdkey);
        msg.SendToServer(NF_DB_SERVER);
    }
}

//!	注册一个排队定时器，并返回定时器ID
long CMsgTimer::RegisterMsgTimerID(long lFutureTime)
{
    //! m_PlayerTimer对象会自动释放pVar
    void* pVar = new tagTimerVar;
    //void* pVar = GameManager::GetInstance()->CreateBlock(sizeof(tagTimerVar));
    /*assert(NULL != pVar);*/
    ((tagTimerVar*)pVar)->TimerType = ChildObjType_WSMsg;

    long lTimerID = GetGame().GetTimerQueue()->Schedule(this, pVar, lFutureTime + timeGetTime());
    m_lTimerID = lTimerID;
    return lTimerID;
}

//!	取消一个排队的Timer
void CMsgTimer::CancelMsgTimer(void)
{
    void* pVar = NULL;
    GetGame().GetTimerQueue()->Cancel(m_lTimerID, (const void**)&pVar);
}

//!	响应排队定时器触发的事件
void CMsgTimer::OnMsgTimeout(long lTimerID)
{
    // 清除MSG map
    CGame::mapMsgTimer& timer = GetGame().GetMsgTimerMap();
    CGame::mapMsgTimerItr itr = timer.find(m_ID);
    if(itr != timer.end())
    {
        timer.erase(itr);
    }

    // 加入到空闲列表, 不管是否超过最大数量
    GetGame().GetFreeMsgList().push_back(this);

    // 释放MSGID
    GetGame().FreeMsgID(m_ID);
}

// decode MSG_S2W_PLAYER_DETAIL_SAVE_PROPERTY 消息
bool CGame::DecodeSavePropertyMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSavePropertyMsg() Start.");
#endif

    BYTE flag = setReadDB.GetByteFromByteArray();

    if(flag == 1) // 有数据
    {
        // 将数据更新到CPlayer和CDBPlayer		
        char name[64];
        setReadDB.GetStringFromByteArray(name,64 );

        //	等级
        WORD wLevel = setReadDB.GetWordFromByteArray( );
        //	职业
        WORD wOccu = setReadDB.GetByteFromByteArray( );
        //	性别(0:男 1:女)
        BYTE bySex = setReadDB.GetByteFromByteArray( );
        // 国家
        BYTE byCountry = setReadDB.GetByteFromByteArray( );
        //头发
        BYTE byHair = setReadDB.GetByteFromByteArray( );
        // posX
        float posX = setReadDB.GetFloatFromByteArray( );
        // posY
        float posY = setReadDB.GetFloatFromByteArray( );
        //ticket
        long lTicket=setReadDB.GetLongFromByteArray();

        // ----------------------------
        //	1：头盔
        DWORD dwHead = setReadDB.GetDwordFromByteArray();// ID
        long lHeadLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	2：项链
        DWORD dwNecklace = setReadDB.GetDwordFromByteArray( );// ID
        long lNecklaceLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	3：翅膀
        DWORD dwWing = setReadDB.GetDwordFromByteArray( );// ID
        long lWingLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	4：盔甲
        DWORD dwBody = setReadDB.GetDwordFromByteArray( );// ID
        long lBodyLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	5：腰带
        DWORD dwBack = setReadDB.GetDwordFromByteArray( );// ID
        long lBackLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	6：手套	
        DWORD dwGlove = setReadDB.GetDwordFromByteArray( );// ID
        long lGloveLevel = setReadDB.GetLongFromByteArray( ); // Level
        // 肩部
        DWORD dwShoulder = setReadDB.GetDwordFromByteArray( );// ID
        long lShoulderLevel = setReadDB.GetLongFromByteArray( ); // Level
        // 手腕	
        DWORD dwWrist = setReadDB.GetDwordFromByteArray( );// ID
        long lWristLevel = setReadDB.GetLongFromByteArray( ); // Level
        // 腿部	
        DWORD dwLeg = setReadDB.GetDwordFromByteArray( );// ID
        long lLegLevel = setReadDB.GetLongFromByteArray( ); // Level
        //  鞋子10	
        DWORD dwBoot = setReadDB.GetDwordFromByteArray();
        long lBootLevel = setReadDB.GetLongFromByteArray();
        //	11：左戒指
        DWORD dwLring = setReadDB.GetDwordFromByteArray( );// ID
        long lLringLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	12：右戒指		
        DWORD dwRring = setReadDB.GetDwordFromByteArray( );// ID
        long lRringLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	13：武器
        DWORD dwWeapon = setReadDB.GetDwordFromByteArray( );// ID
        long lWeaponLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	护符
        DWORD dwCard = setReadDB.GetDwordFromByteArray( );// ID
        long lCardLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	圣物15
        DWORD dwGodGoods = setReadDB.GetDwordFromByteArray( );// ID
        long lGodGoodsLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	雕像
        DWORD dwEffigy = setReadDB.GetDwordFromByteArray( );// ID
        long lEffigyLevel = setReadDB.GetLongFromByteArray( ); // Level
        //	本命法宝
        DWORD dwLifeCowry = setReadDB.GetDwordFromByteArray( );// ID
        long lLifeCowryLevel = setReadDB.GetLongFromByteArray( ); // Level        
        //	魂守
        DWORD dwSoulGuard = setReadDB.GetDwordFromByteArray( );// ID
        long lSoulGuardLevel = setReadDB.GetLongFromByteArray( ); // Level
        // 坐骑19
        DWORD dwRider = setReadDB.GetDwordFromByteArray();
        long lRiderLevel = setReadDB.GetLongFromByteArray();
        long lRegionID = setReadDB.GetLongFromByteArray( ); // region id

        // property 
        tagPlayerProperty tProperty;
        setReadDB.GetBufferFromByteArray(&tProperty, sizeof(tagPlayerProperty));

        //! 仓库信息
        if(pPlayer)
        {
            pPlayer->m_PlayerDepot.DecodeSetupFromDataBlock(setReadDB);
        }
        else
        {
            //! 密码
            char szPassword[128] = {0};
            setReadDB.GetBufferFromByteArray(szPassword, DEPOT_PASSWORD_ARRAY_LEN);
            //! 扩展栏位标识
            setReadDB.GetLongFromByteArray( );
            //! 冻结时间
            setReadDB.GetLongFromByteArray( );
        }

        //系统邮件
        long lMailNum = setReadDB.GetLongFromByteArray();
        for (int i=0; i<lMailNum;++i)
        {
            long lID = setReadDB.GetLongFromByteArray();
            long lEndTime = setReadDB.GetLongFromByteArray();
            pPlayer->AddSysMail(lID,lEndTime);
        }

        //! 勋章容器解码
        pPlayer->m_MedalContainer.DecodeFromDataBlock(setReadDB);

        // ----------------------------
        if(pPlayer)
        {

            WORD wOldLevel = pPlayer->GetLevel();
            WORD wOldOccu = pPlayer->GetOccupation();

            memcpy(&pPlayer->GetPlayerProperty(), &tProperty, sizeof(tagPlayerProperty));

            pPlayer->SetName(name);
            pPlayer->SetLevel(wLevel);
            pPlayer->SetOccupation((BYTE)wOccu);
            pPlayer->SetSex(bySex);	
            pPlayer->SetCountry(byCountry);
            pPlayer->SetHeadPic(byHair);
            pPlayer->SetPosX(posX);
            pPlayer->SetPosY(posY);
            pPlayer->SetTicket(lTicket);

            if (wOldLevel != wLevel)
            {
                LinkmanSystem::GetInstance().OnPlayerLevelChange(pPlayer);
                GetOrganiCtrl()->OnPlayerLevelChange(pPlayer, wLevel);
            }

            if (wOldOccu != wOccu)
            {
                LinkmanSystem::GetInstance().OnPlayerOccuChange(pPlayer);
                GetOrganiCtrl()->OnPlayerOccuChange(pPlayer, wOccu);
            }
        }

        CEntityGroup* dbLPlayer = NULL;
        if(pPlayer)
        {
            Account* pAcc = GetEntityManager()->FindAccount(pPlayer->GetAccount());
            if(pAcc)
                dbLPlayer = pAcc->FindLoginPlayer(pPlayer->GetExID());
        }

        if(dbLPlayer)
        {
            dbLPlayer->SetStringAttr(string("Account"), pPlayer->GetAccount());
            dbLPlayer->SetGuidAttr(string("guid"), pPlayer->GetExID());
            dbLPlayer->SetStringAttr(string("Name"), pPlayer->GetName());
            dbLPlayer->SetStringAttr(string("Account"), pPlayer->GetAccount());
            dbLPlayer->SetLongAttr(string("Levels"), pPlayer->GetLevel());
            dbLPlayer->SetLongAttr(string("Occupation"), pPlayer->GetOccupation());
            dbLPlayer->SetLongAttr(string("Sex"), pPlayer->GetSex());
            dbLPlayer->SetLongAttr(string("Country"), pPlayer->GetCountry());
            dbLPlayer->SetLongAttr(string("Region"), pPlayer->GetRegionID());
            dbLPlayer->SetLongAttr(string("ticket"), pPlayer->GetTicket());


            //	头发
            dbLPlayer->SetLongAttr(string("HEAD"), pPlayer->GetHeadPic());
            //	头发
            dbLPlayer->SetLongAttr(string("HAIR"), pPlayer->GetHeadPic());
            //	头发
            dbLPlayer->SetLongAttr(string("FACE"), pPlayer->GetFacePic());

            BYTE dwNull = 0;
            CGoods* pGoods = NULL;
            // ----------------------------
            //	1：头盔
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_HEAD );
            // ID
            //dbLPlayer->SetLongAttr(string("HEAD"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull));
            dbLPlayer->SetLongAttr(string("HeadLevel"),lHeadLevel ); // Level
            //	2：项链
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_NECKLACE );
            dbLPlayer->SetLongAttr(string("NECKLACE"),dwNecklace );// ID
            dbLPlayer->SetLongAttr(string("NecklaceLevel"),lNecklaceLevel ); // Level
            //	3：翅膀
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_WING );
            dbLPlayer->SetLongAttr(string("WING"),dwWing );// ID
            dbLPlayer->SetLongAttr(string("WingLevel"),lWingLevel ); // Level
            //	4：盔甲
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_BODY );
            dbLPlayer->SetLongAttr(string("BODY"),dwBody );// ID
            dbLPlayer->SetLongAttr(string("BodyLevel"),lBodyLevel ); // Level
            //	5：腰带
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_BACK );
            dbLPlayer->SetLongAttr(string("BACK"),dwBack );// ID
            dbLPlayer->SetLongAttr(string("BackLevel"), lBackLevel ); // Level
            //	6：手套	
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_HAND );
            dbLPlayer->SetLongAttr(string("GLOVE"), dwGlove );// ID
            dbLPlayer->SetLongAttr(string("GloveLevel"), lGloveLevel ); // Level
            // 肩
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_SHOULDER);
            dbLPlayer->SetLongAttr(string("Shoulder"), dwShoulder );// ID
            dbLPlayer->SetLongAttr(string("ShoulderLevel"),lShoulderLevel ); // Level
            // 手腕
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_WRIST);
            dbLPlayer->SetLongAttr(string("Wrist"), dwWrist );// ID
            dbLPlayer->SetLongAttr(string("WristLevel"),lWristLevel); // Level
            // 腿部
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_LEG);
            dbLPlayer->SetLongAttr(string("Leg"),dwLeg);// ID
            dbLPlayer->SetLongAttr(string("LegLevel"), lLegLevel); // Level
            //	7：鞋子
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_BOOT );
            dbLPlayer->SetLongAttr(string("BOOT"),dwBoot );// ID
            dbLPlayer->SetLongAttr(string("BootLevel"),lBootLevel ); // Level
            //	10：左戒指
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_LRING );
            dbLPlayer->SetLongAttr(string("LRING"),dwLring );// ID
            dbLPlayer->SetLongAttr(string("LRingLevel"),lLringLevel); // Level
            //	11：右戒指		
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_RRING );
            dbLPlayer->SetLongAttr(string("RRING"),dwRring );// ID
            dbLPlayer->SetLongAttr(string("RRingLevel"),lRringLevel ); // Level
            //	16：武器
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_WEAPON );
            dbLPlayer->SetLongAttr(string("WEAPON"),dwWeapon);// ID
            dbLPlayer->SetLongAttr(string("WeaponLevel"),lWeaponLevel); // Level
            // 护符
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_CARD );
            dbLPlayer->SetLongAttr(string("Card"),dwCard );// ID
            dbLPlayer->SetLongAttr(string("CardLevel"),lCardLevel ); // Level
            // 圣物15
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_GODGOODS );
            dbLPlayer->SetLongAttr(string("GodGoods"),dwGodGoods );// ID
            dbLPlayer->SetLongAttr(string("GodGoodsLevel"),lGodGoodsLevel ); // Level
            // 雕像
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_EFFIGY );
            dbLPlayer->SetLongAttr(string("Effigy"),dwEffigy);// ID
            dbLPlayer->SetLongAttr(string("EffigyLevel"),lEffigyLevel ); // Level
            // 本命法宝
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_LIFECOWRY );
            dbLPlayer->SetLongAttr(string("LifeCowry"),dwLifeCowry );// ID
            dbLPlayer->SetLongAttr(string("LifeCowryLevel"),lLifeCowryLevel ); // Level
            // 魂守
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_SOULGUARD );
            dbLPlayer->SetLongAttr(string("SoulGuard"),dwSoulGuard );// ID
            dbLPlayer->SetLongAttr(string("SoulGuardLevel"),lSoulGuardLevel ); // Level
            // 坐骑19   
            pGoods = pPlayer->m_cEquipment.GetGoods(EC_E_RIDER );
            dbLPlayer->SetLongAttr(string("Rider"),dwRider );// ID
            dbLPlayer->SetLongAttr(string("RiderLevel"),lRiderLevel ); // Level            

            dbLPlayer->SetLongAttr(string("Region"),  (long)pPlayer->GetRegionID() );


        }
    }

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSavePropertyMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSaveWalletGoodsMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveWalletGoodsMsg() Start.");
#endif

    // 解码装备物品
    pPlayer->m_cWallet.Release();
    pPlayer->m_cWallet.Unserialize(setReadDB);

#ifdef _RUNSTACKINFO1_	
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveWalletGoodsMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSaveSivlerGoodsMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSilverMsg() Start.");
#endif

    // 解码装备物品
    pPlayer->m_cSilverWallet.Release();
    pPlayer->m_cSilverWallet.Unserialize(setReadDB);

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSilverMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSaveEquipmentGoodsMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveEquipmentGoodsMsg() Start.");
#endif
    // 解码装备物品
    pPlayer->m_cEquipment.Release();
    pPlayer->m_cEquipment.Unserialize(setReadDB);
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveEquipmentGoodsMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSavePackGoodsMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSavePackGoodsMsg() Start.");
#endif

    // 解码原始背包物品
    player->getOriginPack()->Release();
    player->getOriginPack()->SetContainerVolume(5*4);
    player->getOriginPack()->Unserialize(setReadDB);

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSavePackGoodsMsg() End.");
#endif
    return true;
}
bool CGame::DecodeDepotGoldMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeDepotGoldMsg() Start.");
#endif

    // 解码原始背包物品
    CGoodsContainer *pContainer = player->m_PlayerDepot.FindContainer(eDCT_Gold);
    if(NULL == pContainer)
    {
       Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_26"));
        return false;
    }
    long lFlag = setReadDB.GetLongFromByteArray();
    if(lFlag)
        pContainer->Unserialize(setReadDB);		
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeDepotGoldMsg() End.");
#endif
    return true;
}
bool CGame::DecodeDepotSilverMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeDepotSilverMsg() Start.");
#endif

    // 解码原始背包物品
    CGoodsContainer *pContainer = player->m_PlayerDepot.FindContainer(eDCT_Silver);
    if(NULL == pContainer)
    {
        Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_27"));
        return false;
    }
    long lFlag = setReadDB.GetLongFromByteArray();
    if(lFlag)
        pContainer->Unserialize(setReadDB);
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeDepotSilverMsg() End.");
#endif
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CGame::DecodeSaveSubDepotGoodsMsg(long subPackPos, CPlayer* player, DBReadSet& setReadDB)
{
    player->UnserializeSubDepot(setReadDB);
    return true;
}

bool CGame::DecodeDepotGoodsMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeDepotGoodsMsg() Start.");
#endif

    // 解码原始背包物品
    CVolumeLimitGoodsContainer *pContainer = dynamic_cast<CVolumeLimitGoodsContainer *>(player->m_PlayerDepot.FindContainer(eDCT_Primary));
    if(NULL == pContainer)
    {
        Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_28"));
        return false;
    }
    long lFlag = setReadDB.GetLongFromByteArray();
    if(lFlag)
    {
        pContainer->Release();
        pContainer->SetContainerVolume(PRIMARY_SIZE);
        pContainer->Unserialize(setReadDB);
    }

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeDepotGoodsMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSaveSubPackGoodsMsg(long subPackPos, CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSubPackGoodsMsg() Start.");
#endif
    player->GetSubpackContainer()->Release(subPackPos);
    player->UnserializeSubpack(setReadDB);
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSubPackGoodsMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSaveVarListMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveVarListMsg() Start.");
#endif

    player->GetVariableList()->RemoveAll();
    VarList::Decode( player->GetVariableList(), setReadDB );

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveVarListMsg() End.");
#endif
    return true;
}

bool CGame::DecodeSaveSpriteSystemMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSpriteSystemMsg() Start.");
#endif

    player->m_SpriteSystem.DecodeFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSpriteSystemMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSaveQuestListMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveQuestListMsg() Start.");
#endif
    player->DecodeQuestDataFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveQuestListMsg() End.");
#endif
    return true;
}

//俑兵任务数据
bool CGame::DecodeSaveMerQuestListMsg(CPlayer* player, DBReadSet& setReadDB)
{
    player->DecodeMerQuestDataFromDataBlock(setReadDB);
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveMerQuestListMsg() End.");
#endif
    return true;
}

bool CGame::DecodeSaveSkillListMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSkillListMsg() Start.");
#endif
    long skillNum = setReadDB.GetLongFromByteArray();
    if(skillNum)
    {
        list<CPlayer::tagSkill> tSkillList;
        CPlayer::tagSkill tmpSkill;
        for(int i=0; i<skillNum; i++)
        {
            tmpSkill.wID    = setReadDB.GetDwordFromByteArray();
            tmpSkill.wLevel = (WORD)setReadDB.GetDwordFromByteArray();
            tmpSkill.wCooldown = setReadDB.GetDwordFromByteArray();
            tSkillList.push_back(tmpSkill);
        }

        if(player)
        {
            list<CPlayer::tagSkill>& psl = player->GetSkillList();
            psl.clear();
            CPlayer::tagSkill tsl;
            list<CPlayer::tagSkill>::iterator skillItr = tSkillList.begin();
            for(int i=0; i<skillNum; i++)
            {
                tsl.wID = (*skillItr).wID;
                tsl.wLevel = (*skillItr).wLevel;
                tsl.wCooldown = (*skillItr).wCooldown;
                psl.insert(psl.end(), tsl);
                skillItr++;
            }
        }
    }

#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetStatckFileName(),"DecodeSaveSkillListMsg() End.");
#endif
    return true;
}
bool CGame::DecodeSaveDeputyOccuListMsg(CPlayer* player, DBReadSet& setReadDB)
{
    if(player)
        player->DecodeDOccuFromDataBlock(setReadDB);
    /*if(dbPlayer)
    dbPlayer->SetBufAttr(string("DOccuData"), tmpBuf, (long)tmpBufSize);*/
    return true;
}
bool CGame::DecodeSaveStateListMsg(CPlayer* player, DBReadSet& setReadDB)
{
    // 将数据更新到CPlayer和CDBPlayer	
    long tmpBufSize = setReadDB.GetDwordFromByteArray( );
    BYTE* pTemptBuff = new BYTE[tmpBufSize];
    setReadDB.GetBufferFromByteArray(pTemptBuff,tmpBufSize);
    player->GetExStates().clear();
    player->SetExStates(pTemptBuff, tmpBufSize);
    SAFE_DELETE_ARRAY(pTemptBuff);
    return true;
}

// 当GS连接WS时初始化个人房屋副本
void CGame::InitPersonalHouseDataOnGS(long gsid)
{
    // 从DBDupRgn中找相应CWorldRegion对象的CDBDupRgn对象
    CEntityGroup* pPhRgnGroup = NULL;
    string strGroup = "[phrgngroup]";
    tagEntityBuildInfo* pInfo = GetEntityManager()->GetEntityBuildInfo(strGroup);
    if(pInfo)
        pPhRgnGroup = GetEntityManager()->FindRootEntity(strGroup, pInfo->guid);
    if(!pPhRgnGroup) return;

    map<CGUID, CBaseEntity*>::iterator dupRgnItr = pPhRgnGroup->GetEntityGroupMap().begin();
    for(; dupRgnItr != pPhRgnGroup->GetEntityGroupMap().end(); dupRgnItr++)
    {
        if(gsid == dupRgnItr->second->GetLongAttr(string("GSID"))) // 同一个GS的PH对象
        {
            CWorldRegion* wRgn = GetGlobalRgnManager()->FindRgnByGUID(dupRgnItr->second->GetGUID());
            if(!wRgn)
            {
                wRgn = new CWorldRegion;
                GetGlobalRgnManager()->GetRgnMap()[dupRgnItr->second->GetGUID()] = wRgn;
            }

            if(wRgn)
            {
                wRgn->SetExID(dupRgnItr->second->GetGUID());
                wRgn->SetID(dupRgnItr->second->GetLongAttr(string("TemplateRgnID")));
                CGUID oGUID;
                dupRgnItr->second->GetGuidAttr(string("OwnerGUID"),oGUID);
                wRgn->SetResourceID(dupRgnItr->second->GetLongAttr(string("ResourceID")));
                wRgn->SetRgnType((long)RGN_PERSONAL_HOUSE);
                //wRgn->New();
                //wRgn->Load();
                CGlobalRgnManager::tagRegion* pTemplateRgn = GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(wRgn->GetID());
                if(pTemplateRgn && pTemplateRgn->pRegion)
                    wRgn->CloneAllData(pTemplateRgn->pRegion);

                wRgn->SetOwnerGUID(oGUID);
                wRgn->SetGsid(gsid);
                wRgn->SetName(dupRgnItr->second->GetStringAttr(string("name")));
                CPlayer* owner = GetMapPlayer(oGUID);
                if(owner)
                    owner->SetPersonalHouseRgnGUID(wRgn->GetExID());
            }

            // 向WS发送副本数据
            CMessage msg(MSG_W2S_SERVER_SENDSETUP);
            msg.Add((long)SYNC_INSTANCESELF);
            msg.Add( dupRgnItr->second->GetGUID());
            msg.Add( dupRgnItr->second->GetLongAttr(string("GSID")) );// GSID
            msg.Add( (char*)wRgn->GetName() );
            CGUID ownerID;
            dupRgnItr->second->GetGuidAttr(string("OwnerGUID"), ownerID);
            msg.Add( ownerID );
            msg.Add( dupRgnItr->second->GetLongAttr(string("TemplateRgnID")) );
            msg.Add( dupRgnItr->second->GetLongAttr(string("ResourceID")) );

            vector<BYTE> pRgnData;
            wRgn->AddToByteArray(&pRgnData);
            msg.Add((long)pRgnData.size());
            if(pRgnData.size())
                msg.AddEx(&pRgnData[0], (long)pRgnData.size());

            struct PHGoods
            {
                // 物品GUID
                CGUID guid;
                // 物品名字
                char name[64];
                // 朝向
                int dir;
                // X长度
                int width;
                // Y长度
                int height;
                // 坐标
                float posX;
                float posY;
            };

            CEntityGroup* pGoodsGroup = (CEntityGroup*)GetEntityManager()->FindLeafByComFlag(
                (CEntityGroup*)dupRgnItr->second, string("[phrgngoodsgroup]"));
            if(!pGoodsGroup)
            {
                Log4c::Warn(ROOT_MODULE, GetText("WS_SYS_29"));
                return;
            }

            msg.Add( (long)pGoodsGroup->GetEntityGroupMap().size() );

            map<CGUID, CBaseEntity*>::iterator goodsItr = pGoodsGroup->GetEntityGroupMap().begin();
            for(; goodsItr != pGoodsGroup->GetEntityGroupMap().end(); goodsItr++)
            {
                PHGoods tGoods;
                goodsItr->second->GetGuidAttr(string("guid"),tGoods.guid);
                strcpy(tGoods.name, goodsItr->second->GetStringAttr(string("Name")));
                tGoods.dir    = goodsItr->second->GetLongAttr(string("Dir")	);
                tGoods.width  = goodsItr->second->GetLongAttr(string("Width")	);
                tGoods.height =	goodsItr->second->GetLongAttr(string("Height")	);
                tGoods.posX	  =	(float)goodsItr->second->GetLongAttr(string("PosX")	);
                tGoods.posY	  =	(float)goodsItr->second->GetLongAttr(string("PosY")	);

                msg.AddEx( &tGoods, sizeof(PHGoods));
            }
            msg.SendToGS(dupRgnItr->second->GetLongAttr(string("GSID")));
        }
    }
}
// 根据时间创建验证码值
void CGame::GenerateSignCodeByTime(long& lRetCodeValue)
{
    long lFirstTimeValue = timeGetTime();
    long lFirstCodeValue = random(lFirstTimeValue);
    lRetCodeValue = random(lFirstCodeValue);
}


// 连接GateServer
bool CGame::ConnectGate(bool bRec)
{
    return false;
}

// GateServer断开
void CGame::OnGateLost(void)
{}

// 断开后创建连接线程
void CGame::CreateGateThread(void)
{}

// 连接GateServer的线程, 断开时启用, 连通时关闭
unsigned __stdcall ConnectGateFunc(void* pArg)
{
    return 0;
}
void CGame::PushSavePlayer(CPlayer* pPlayer)
{
    if(!pPlayer) return;
    map<CGUID, CPlayer*>::iterator itr = m_mapSavePlayers.find(pPlayer->GetExID());
    if(itr == m_mapSavePlayers.end())
    {
        DelPlayerLog(pPlayer);
        m_mapSavePlayers[pPlayer->GetExID()] = pPlayer;
        AddCreatePlayerLog(ePlayerPushSave, pPlayer);
    }
    else
    {
        if(itr->second != pPlayer)
        {
            DelRawPlayer(itr->second);
            m_mapSavePlayers.erase(itr);

            DelPlayerLog(pPlayer);
            m_mapSavePlayers[pPlayer->GetExID()] = pPlayer;
            AddCreatePlayerLog(ePlayerPushSave, pPlayer);
        }
        else
        {
            char szGuid[128];
            pPlayer->GetExID().tostring(szGuid);
            Log4c::Warn(ROOT_MODULE,FormatText("WS_SYS_30", szGuid));
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
void CGame::ProcessDelaySavePlayer(void)
{
    map<CGUID, CPlayer*>::iterator saveItr = m_mapSavePlayers.begin();
    for(; saveItr != m_mapSavePlayers.end(); saveItr++)
    {
        if(!saveItr->second) continue;


        CWorldServerSession pSession(5000);

        // 设置当前账号状态
        Account* pAcc = NULL;
        CEntityGroup* tDBLoginPlayer = NULL;
        if(saveItr->second->GetAccount())
            pAcc = GetEntityManager()->FindAccount(saveItr->second->GetAccount());
        if(pAcc)
        {
            if(pAcc->GetPlayerState() != PLAYER_STATE_VOID)
                pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
            pAcc->SetCurGamedPlayerGuid(NULL_GUID);
            tDBLoginPlayer = pAcc->FindLoginPlayer(saveItr->second->GetExID());
            pSession.NakeSendSaveLoginPlayerDataMsg(saveItr->second, tDBLoginPlayer);
        }
        else
        {
            tDBLoginPlayer = (CEntityGroup*)GetEntityManager()->NewBaseEntity(string("[loginplayer]"), saveItr->second->GetExID());
            pSession.NakeSendSaveLoginPlayerDataMsg(saveItr->second, tDBLoginPlayer);
            GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBLoginPlayer);
        }

        CEntityGroup* dbPlayer = (CEntityGroup*)GetEntityManager()->NewBaseEntity(string("[player]"), saveItr->second->GetExID());
        pSession.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, saveItr->second, dbPlayer, true);
        GetEntityManager()->DelBaseEntity((CBaseEntity*)dbPlayer);

        // 通知LoginServer
        CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
        msg.Add(const_cast<char*>(saveItr->second->GetAccount()));
        msg.Add(0L);
        msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
#ifdef _RUNSTACKINFO_
        char pszLogingInfo[512];
        char szGuid[128];
        saveItr->first.tostring(szGuid);
        _snprintf(pszLogingInfo,512, "玩家[%s]延迟存储操作完成!", szGuid);
        CMessage::AsyWriteFile(GetStatckFileName(), pszLogingInfo);
#endif
        DelRawPlayer(saveItr->second);
    }
    m_mapSavePlayers.clear();
}

void CGame::SavePlayer(CPlayer *pPlayer)
{
    if(NULL != pPlayer)
    {
        CWorldServerSession pSession(5000);

        CEntityGroup* dbPlayer = (CEntityGroup*)GetEntityManager()->NewBaseEntity(string("[player]"), pPlayer->GetExID());
        pSession.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, pPlayer, dbPlayer, true);
        GetEntityManager()->DelBaseEntity((CBaseEntity*)dbPlayer);
    }
}


void CGame::AddCreatePlayerLog(eCreatePlayerPlace place, CPlayer* player)
{
    std::map<eCreatePlayerPlace, std::set<CPlayer*>>::iterator itr = m_mapCreatePlayerLog.find(place);
    if(itr != m_mapCreatePlayerLog.end())
    {
        itr->second.insert(player);
    }
    else
    {
        std::set<CPlayer*> guidSet;
        guidSet.insert(player);
        m_mapCreatePlayerLog[place] = guidSet;
    }
}
void CGame::DelPlayerLog(CPlayer* player)
{
    std::map<eCreatePlayerPlace, std::set<CPlayer*>>::iterator itr = m_mapCreatePlayerLog.begin();
    for(; itr != m_mapCreatePlayerLog.end(); itr++)
    {
        itr->second.erase(player);
    }
}

void CGame::OutputCreatePlayerInfo(void)
{
    char szInfo[1024 * 16];

    strcpy(szInfo, "Create Player Info:\r\n");
    long lRet = 0;
    std::map<eCreatePlayerPlace, std::set<CPlayer*>>::iterator itr = m_mapCreatePlayerLog.begin();
    for(; itr != m_mapCreatePlayerLog.end(); itr++)
    {
        char szTmp[1024];
        lRet += (long)itr->second.size();
        sprintf(szTmp, "place: %d, Num: %d.\r\n", itr->first, lRet);
        strcat(szInfo, szTmp);
    }

    //PutStringToFile("MemAllocInfo", szInfo);
    Log4c::Trace(ROOT_MODULE, szInfo);
}
CPlayer* CGame::NewRawPlayer(eCreatePlayerPlace place)
{
    CPlayer* res = new CPlayer;
    AddCreatePlayerLog(place, res);
    return res;
}
void CGame::DelRawPlayer(CPlayer* player)
{
    DelPlayerLog(player);
    delete player;
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

    long lDiffSec = (long)( dwTime - mktime(&tm2007) );
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
