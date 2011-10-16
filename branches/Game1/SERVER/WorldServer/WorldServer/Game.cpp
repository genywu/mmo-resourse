#include "StdAfx.h"
#include <sstream>

#include "appworld\player.h"
#include "appworld\goods\cgoods.h"
#include "appworld\goods\cgoodsfactory.h"
#include "appworld\goods\csuitrulefactory.h"
#include "appworld\goods\enchase\CKitCardRuleFactory.h"
#include "appworld\worldregion.h"
#include "appworld\script\variablelist.h"
#include "appworld\session\csessionfactory.h"
#include "..\LogClient\ws\LogicLogInterface.h"
#include ".\game.h"
#include "..\nets\MapFile.h"
#include "..\nets\Servers.h"
#include "..\nets\networld\Message.h"
#include "setup\playerlist.h"
#include "setup\monsterlist.h"
#include "setup\GateList.h"
#include "setup\tradelist.h"
#include "setup\IncrementShopList.h"
#include "setup\RegionPlugSetup.h"
#include "setup\ContributeSetup.h"
#include "setup\hitlevelsetup.h"
#include "setup\emotion.h"
#include "..\AppWorld\NewSkill\NewSkillFactory.h"
#include "setup\globesetup.h"
#include "setup\logsystem.h"
#include "setup\gmlist.h"
#include "setup\questsystem.h"
#include "setup\regionsetup.h"
#include "setup\MailList.h"
#include "setup\ScriptAILoader.h"
#include "..\AppWorld\goods\IncrementShopManager.h"
#include "..\AppWorld\Organizingsystem\organizingctrl.h"
#include "..\AppWorld\Organizingsystem\OrganizingParam.h"
#include "..\AppWorld\Organizingsystem\union.h"
#include "..\AppWorld\country\country.h"
#include "..\AppWorld\Organizingsystem\faction.h"
#include "..\AppWorld\Organizingsystem\Organizing.h"
#include "..\AppWorld\CardsDealingSystem\CardsHandler.h"
#include "..\AppWorld\Country\CountryHandler.h"
#include "../AppWorld/business/CBusinessManager.h"
#include "../AppWorld/SpriteSystem.h"
#include "..\public\GoodsExSetup.h"
#include "..\public\WordsFilter.h"
#include "..\public\dupliregionsetup.h"
#include "..\public\DataBlockAllocator.h"
#include "..\public\OccupSkillList.h"
#include "..\public\StudySkillCondition.h"
#include "../public/DeputyOccu.h"
#include "../public/strtype.h"
#include "../public/StrUnifyInput.h"
#include "../public/ReliveGoodsList.h"
#include "../public/Cosmetology.h"

#include "..\AppWorld\Country\CountryParam.h"
#include "..\appworld\session\csessionfactory.h"
#include "..\appworld\session\csession.h"
#include "../AppWorld/goods/CSuitRuleFactory.h"
#include "../AppWorld/business/cbusinessmanager.h"
#include "..\AppWorld\Message\MsgOutTimeDef.h"
#include "../AppWorld/DBEntity/Account.h"
#include "../AppWorld/dbentity/entityManager.h"
#include "../AppWorld/dbentity/entitygroup.h"
#include "../AppWorld/GlobalRgnManager.h"
#include "../AppWorld/PlayerDepot.h"
#include "../AppWorld/Linkman/LinkmanSystem.h"
#include "../public/Crc32Static.h"
#include "..\AppWorld\PlayerMan\PlayerMan.h"	// Fox@20081110 玩家搜索
#include "..\AppWorld\PetCtrl.h"				// Fox@20081120 宠物控制
#include "..\AppWorld\AntiCheat\FileListManager.h"
#include "..\AppWorld\AntiCheat\QuestionManager.h"
#include "../AppWorld/AntiCheat/CConfiger.h"
#include "..\public\CreditSystem.h"
#include "..\AppWorld\SMClient.h"
#include "../AppWorld/FBusiness/Configer.h" 
#include "setup\counttimerlist.h"
#include "..\AppWorld\organizingsystem\SentaiSystem.h"
#include "..\public\QuestIndexXml.h"
#include "..\public\LotteryXml.h"
#include "..\AppWorld\Lottery\CLotterySystem.h"

const DWORD g_ms = 40;

// 调试用
DWORD g_lCurTime = 0;
DWORD g_lLSMessageTime = 0;
DWORD g_lGSMessageTime = 0;
DWORD g_lCSMessageTime = 0;

CGame* g_pGame= NULL;		// 游戏客户端数据
HANDLE g_hGameThreadExitEvent=NULL;		// 主线程退出事件
bool g_bGameThreadExit=false;		// 是否退出主线程
HANDLE	hLSConnectThread=NULL;			// 连接线程句柄 Login
bool	bLSConnectThreadExit=false;		// 是否退出连接线程
HANDLE  hConnectThread1=NULL;		// Cost
bool	bConnectThreadExit1=false;
HANDLE  hDBSConnectThread=NULL;  //DB
bool	bDBSConnectThreadExit=false;
vector<HANDLE> g_vPlayerLoadThreads;		// 玩家读数据库线程
bool volatile g_bPlayerLoadThreadsExit = false;
HANDLE g_hSavingThread = NULL; // 存盘线程句柄
CSessionFactory* CGame::m_pSessionFactory = NULL;

// 默认不显示存盘信息
extern bool g_bShowSaveInfo = false;
void ShowSaveInfo(LPCSTR szInfo, ...)
{
	if(g_bShowSaveInfo)
	{
		char str[256];
		va_list va;
		va_start(va,szInfo);
		vsprintf(str,szInfo,va);
		va_end(va);

		AddLogText(const_cast<char*>(str));
	}
}

bool g_bSendSaveMsgNow = false;
bool g_bSaveNowData = false;
bool g_bSaveAllOrg = false;			//手动提请保存所有的帮会
bool g_bSendCollectPlayerDataMsgNow = false;
bool g_bKickAllPlayer = false;

bool g_bStatPlayerRanks = false;

unsigned volatile RELOAD_PROFILE_FLAGS			= 0x00000000;				// 重读配置文件标识
const unsigned ANTI_RELOAD_PROFILE_FLAGS		= ~RELOAD_PROFILE_FLAGS;
unsigned volatile RELOAD_CONFIG_FLAGS			= 0x80000000;				// 重读配置文件标识
const unsigned ANTI_RELOAD_CONFIG_FLAGS			= ~RELOAD_CONFIG_FLAGS;

// define const
extern const unsigned RELOAD_LOGING				= 0x00000001;	//读日志配置
extern const unsigned RELOAD_GMLIST				= 0x00000002;	//读GM配置
extern const unsigned RELOAD_BROADCAST			= 0x00000004;	//读广播
extern const unsigned RELOAD_MAPFILESETUP		= 0x00000008;	//内存影射文件配置

extern const unsigned RELOAD_BUILD				= 0x00000010;	//读城战
extern const unsigned RELOAD_INCREMENT			= 0x00000020;	//读增值商店
extern const unsigned RELOAD_GAMESETUP			= 0x00000040;	//读经验和掉宝率
extern const unsigned RELOAD_WORDFILTER			= 0x00000080;	//读文字过滤

extern const unsigned RELOAD_PLAYERLIST			= 0x00000100;	//玩家属性和经验
extern const unsigned RELOAD_GOODSLIST			= 0x00000200;	//物品属性
extern const unsigned RELOAD_MONSTERLIST		= 0x00000400;	//怪物属性和掉宝
extern const unsigned RELOAD_TRADELIST			= 0x00000800;	//商店配置

extern const unsigned RELOAD_SKILLLIST			= 0x00001000;	//技能配置
extern const unsigned RELOAD_GOLBELSETUP		= 0x00002000;	//全局配置
extern const unsigned RELOAD_ALLSCRIPT			= 0x00004000;	//所以脚本
extern const unsigned RELOAD_ONESCRIPT			= 0x00008000;	//单个脚本（没有实现，依然使用老实调用方法）

extern const unsigned RELOAD_GENVAR				= 0x00010000;	//脚本变量
extern const unsigned RELOAD_REGIONLIST			= 0x00020000;	//场景列表
extern const unsigned RELOAD_REGIONSETUP		= 0x00040000;	//场景设置
extern const unsigned RELOAD_ONEREGION			= 0x00080000;	//单个场景*.rs（没有实现，依然使用老实调用方法）

extern const unsigned RELOAD_ALLREGION			= 0x00100000;	//所有场景*.rs
extern const unsigned RELOAD_FACTION			= 0x00200000;	//帮会设置
extern const unsigned RELOAD_FACTIONWAR			= 0x00400000;	//帮会战
extern const unsigned RELOAD_QUEST				= 0x00800000;	//任务

extern const unsigned RELOAD_CONTRIBUTE			= 0x01000000;	//国家贡献暴出物品
extern const unsigned RELOAD_ALLREGIONALL		= 0x02000000;	//所有场景(所有数据)
extern const unsigned RELOAD_DOCCULIST			= 0x04000000;	//副职业列表
extern const unsigned RELOAD_STRINGRESOURCE		= 0x08000000;   //字符串资源

extern const unsigned RELOAD_SYSMAIL			= 0x10000000;   //系统邮件设置
extern const unsigned RELOAD_FUNCSETUP			= 0x20000000;	//WS的部分功能设置(FuncSetup.ini)
extern const unsigned RELOAD_DUPRGNSERVER		= 0x40000000;	//WS的副本功能设置(data/duprgnserver.ini)
extern const unsigned RELOAD_CREDIT				= 0x80000000;	//声望配置
// ------------------------------------------------------------------------------
extern const unsigned RELOAD_PLAYER_MAN			= 0x00000001;	// Fox@20081120 玩家管理
extern const unsigned RELOAD_PET_LIST			= 0x00000002;	// Fox@20081119 宠物

extern const unsigned RELOAD_ANTI_CHEAT         = 0x00000004;   // BaiYun@2009525 防外挂
extern const unsigned RELOAD_FBUSINESS			= 0x00000008;   // 家族跑商配置

extern const unsigned RELOAD_COUNT_TIMER		= 0x00000010;	// 玩家定时器
extern const unsigned RELOAD_COSMETOLOGY		= 0x00000020;	// 美容系统配置
extern const unsigned RELOAD_QUEST_INDEX        = 0x00000040;   // 任务引导
extern const unsigned RELOAD_LOTTERY            = 0x00000080;   // 彩票系统
// end define

void reload_profiles()
{	
#define TEST_BEGIN() \
	if (RELOAD_PROFILE_FLAGS & ANTI_RELOAD_PROFILE_FLAGS || RELOAD_CONFIG_FLAGS & ANTI_RELOAD_CONFIG_FLAGS) { \
	GetGame()->LoadServerResource();

#define TEST_END() \
	_TEST_END_: GetGame()->UnloadServerResource(); }

	// 做位检测，是否需要重读，结果为1时，并清除标志位。
#define TEST(TOTAL, SINGLE) \
	(TOTAL)&(SINGLE)?(((TOTAL)&=~(SINGLE)),1):0

#define TEST_EX(FLAGS, FLAG, SENDGS, RELOAD) \
	if( !((FLAGS)&(ANTI_##FLAGS)) ) goto _TEST_END_; \
	if( TEST((FLAGS), (FLAG)) ) GetGame()->LoadEx((FLAG), (SENDGS), (RELOAD));
	
	static int nNum = 0;

TEST_BEGIN()
	if (TEST(RELOAD_PROFILE_FLAGS,RELOAD_STRINGRESOURCE))
		GetGame()->ReLoad("GlobalString",true,true);

	if (TEST(RELOAD_PROFILE_FLAGS,RELOAD_SYSMAIL))
		GetGame()->ReLoad("SysMail",true,true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_LOGING) )
		GetGame()->ReLoad("LogSystem", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_GMLIST) )
		GetGame()->ReLoad("GMList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_BROADCAST) )
		GetGame()->ReLoad("Broadcast",false, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_BUILD) )
		GetGame()->ReLoad("GateList",true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_INCREMENT) )
		GetGame()->ReLoad("IncrementShopList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_GAMESETUP) )
		GetGame()->ReLoad("GameSetup", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_WORDFILTER) )
		GetGame()->ReLoad("InvalidStr", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_PLAYERLIST) )
		GetGame()->ReLoad("PlayerList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_FUNCSETUP) )
		GetGame()->ReLoad("GameSetup", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_MAPFILESETUP) )
		GetGame()->LoadMapFileSetup();

	if( TEST(RELOAD_PROFILE_FLAGS, RELOAD_DUPRGNSERVER) )
		GetGame()->ReLoad("duprgnserver", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_GOODSLIST) )
		GetGame()->ReLoad("GoodsList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_MONSTERLIST) )
		GetGame()->ReLoad("MonsterList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_TRADELIST) )
		GetGame()->ReLoad("TradeList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_SKILLLIST) )
		GetGame()->ReLoad("SkillList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_GOLBELSETUP) )
		GetGame()->ReLoad("GlobeSetup", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_ALLSCRIPT) )
		GetGame()->ReLoad("ScriptFile", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_REGIONLIST) )
		GetGame()->ReLoad("RegionList", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_REGIONSETUP) )
		GetGame()->ReLoad("RegionLevelSetup", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_ALLREGION) )
		GetGame()->ReLoadAllRegionSetup();

	if (TEST(RELOAD_PROFILE_FLAGS,RELOAD_ALLREGIONALL))
		GetGame()->ReLoadAllRegion();

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_FACTION) )
		GetGame()->ReLoad("FactionPara",false, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_FACTIONWAR) )
		GetGame()->ReLoad("FactionWarPara",false,true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_QUEST) )
		GetGame()->ReLoad("QuestData",false, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_CONTRIBUTE) )
		GetGame()->ReLoad("ContributeSetup", true, true);

	if( TEST(RELOAD_PROFILE_FLAGS,RELOAD_DOCCULIST) )
		GetGame()->ReLoad("DeputyOccuList", true, true);

	if (TEST(RELOAD_PROFILE_FLAGS,RELOAD_CREDIT) )
		GetGame()->ReLoad("CreditSystem",true,true);

	TEST_EX(RELOAD_CONFIG_FLAGS, RELOAD_PLAYER_MAN, -1, true);
	TEST_EX(RELOAD_CONFIG_FLAGS, RELOAD_PET_LIST, -1, true);
	TEST_EX(RELOAD_CONFIG_FLAGS, RELOAD_ANTI_CHEAT, -1, true ); // BaiYun@2009525 反外挂
	TEST_EX( RELOAD_CONFIG_FLAGS, RELOAD_FBUSINESS, -1, true );

	if (TEST(RELOAD_CONFIG_FLAGS, RELOAD_COUNT_TIMER) )
		GetGame()->ReLoad("CountTimerList",true,true);

	if (TEST(RELOAD_PROFILE_FLAGS,RELOAD_COSMETOLOGY))
			GetGame()->ReLoad("Cosmetology",true,true);
	if (TEST(RELOAD_CONFIG_FLAGS, RELOAD_QUEST_INDEX) )
		GetGame()->ReLoad("QuestIndex",true,true);
	if (TEST(RELOAD_CONFIG_FLAGS, RELOAD_LOTTERY) )
		GetGame()->ReLoad("Lottery",true,true);
	
TEST_END()

#undef TEST_EX
#undef TEST
#undef TEST_END
#undef TEST_BEGIN
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


// 获取游戏日志逻辑接口对象
LogicLogInterface* GetGameLogInterface()
{
	return GetGame()->GetLogicLogInterface();
}


CGame::CGame(void)
:	b_HasQuestEnemyFactionMsgFlag(false),
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
	m_mapMsgVirMemUseSize.clear();

	s_lAITick = 0;

	s_pNetServer = NULL;
	
	m_ServerInitSendFlag = 0;
	InitializeCriticalSection(&m_CSServerInitSendFlag);

	m_pDupliRegionSetup = NULL;
	m_pDBAllocator = NULL;
	m_pMapFile = NULL;

	m_strFunctionList_FileData = NULL;
	m_strVariableList_FileData = NULL;
	m_lFunctionList_FileData_Size = 0;
	m_lVariableList_FileData_Size = 0;
	m_nDBResponsed=0;

	GetSetup()->strName = "WorldServer";
	GetSetup()->strLoginIP = "127.0.0.1";
	GetSetup()->dwLoginPort = 2345;
	GetSetup()->dwListenPort = 8100;	

	m_listGoodsLink.resize(500);

	m_pGeneralVariableList = NULL;
	m_dwLastReLoginTime = timeGetTime();

	m_bInPing = false;
	m_dwLastPingGameServerTime = timeGetTime();

	// 全局对象
	g_GlobalRgnManager	= new CGlobalRgnManager;
	g_EntityManager	= new CEntityManager;
	g_pMailManager		=  GetMailManager()->GetInstance();

	m_pLogicLogInterface= new LogicLogInterface(NULL);
}

CGame::~CGame(void)
{
	ReleaseMsgTimerMap();

	SAFE_DELETE(g_pMailManager);
	DeleteCriticalSection(&m_CSServerInitSendFlag);
	OUT_RUNNING_INFO(true);
}
CSessionFactory* CGame::GetSessionFactoryInst(void)
{
	if(!m_pSessionFactory)
		m_pSessionFactory = new CSessionFactory;
	return m_pSessionFactory;
}
CGlobalRgnManager*	CGame::GetGlobalRgnManager(void)
{
	return g_GlobalRgnManager;
}
CMailSystem*		CGame::GetMailManager(void)
{
	return g_pMailManager;
}
CRgnManager*		CGame::GetRgnManager(void)
{
	return g_RgnManager;
}
CEntityManager*	CGame::GetEntityManager(void)
{
	return g_EntityManager;
}

bool CGame::LoadServerResource()
{
	CHAR* strPath = new CHAR[MAX_PATH];
	string strWorpath("");
	if(::GetCurrentDirectory(MAX_PATH,strPath))
	{
		strWorpath=string(strPath);
	}
	delete []strPath;

	// 初始化数据包

	string strFilesInfo("PackRes.cfg");	
	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,strWorpath,strFilesInfo);
	if(!pClientResource->LoadEx())
	{
		AddLogText("Load WorldServer Resource Failed!");
		//装载客户端资源信息出错	
		return true;
	}

	AddLogText("Load WorldServer Resource Ok!");
	return true;
}

bool CGame::LoadSetup()
{
	// 读取全局配置
	string strTemp;
	ifstream streamLog;

	streamLog.open("LogSetup.ini");
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
		MessageBox(g_hWnd, "Can't find file LogSetup.ini", "message", MB_OK);
		return false;
	}
	ifstream stream;
	stream.open("setup.ini");
	if( stream.is_open() )
	{
		stream  >> strTemp >> GetSetup()->dwNumber
			>> strTemp >> GetSetup()->strName
			>> strTemp >> GetSetup()->strDBIP
			>> strTemp >> GetSetup()->dwDBPort
			>> strTemp >> GetSetup()->strLoginIP
			>> strTemp >> GetSetup()->dwLoginPort
			>> strTemp >> GetSetup()->strLocalIP
			>> strTemp >> GetSetup()->dwListenPort; //end ;

		stream.close();

		strTemp = "WorldServer";
		strTemp += "[";
		strTemp += GetSetup()->strName;
		strTemp += "]-StarDust";

		//查看此程序是否在运行中
		HWND hWnd = FindWindow(NULL,strTemp.c_str());
		if (hWnd != NULL)
		{
			char strMsgBox[256]="";
			sprintf(strMsgBox,"%s 程序正在运行中!",strTemp.c_str());
			MessageBox(g_hWnd,_T(strMsgBox), "error", MB_OK);
			return false;
		}

		SendMessage(g_hWnd, WM_SETTEXT, 0, (LPARAM)strTemp.c_str() );
	}
	else	// 从加密文件读取
	{
		MessageBox(g_hWnd, "Can't find file setup.ini", "message", MB_OK);
			return false;
		}

	ifstream streamSetupEx;
	streamSetupEx.open("SetupEx.ini");
	if( streamSetupEx.is_open() )
	{
		streamSetupEx >> strTemp >> GetSetup()->dwDataBlockNum
			>> strTemp >> GetSetup()->dwDataBlockSize
			>> strTemp >> GetSetup()->dwFreeMsgNum

			>> strTemp >> GetSetup()->dwFreeSockOperNum
			>> strTemp >> GetSetup()->dwFreeIOOperNum
			>> strTemp >> GetSetup()->lIOOperDataBufNum
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
		>> strTemp >> GetSetup()->dwReleaseLoginPlayerTime
		>> strTemp >> GetSetup()->dwClearPlayerDataTime;
		streamSetupEx.close();
	}
	else
		{
		MessageBox(g_hWnd, "Can't find file SetupEx.ini", "message", MB_OK);
			return false;
		}
		return true;
}

bool CGame::LoadMapFileSetup()
{
	return CSMClient::GetSM()->LoadMapFileSetup(m_pDBAllocator);
}

void CGame::SetNoonTimer(CMsgTimer* msgTime)
{
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
	//创建一个log的保存目录
	CreateDirectory( "log",	NULL);
	SET_OUT_INFO_SETUP(OutMemInfo, "log\\");
#ifdef _RUNSTACKINFO_
	InitStackFileName();
#endif

#ifdef __MEMORY_LEAK_CHECK__
	CBaseObject::ClearAllConstrucRef();
#endif

	srand((unsigned)time(NULL));
	random(100);

	PutDebugString("WorldServer start!");

	//装载资源
	LoadServerResource();

    //读入全局字符串
	ReLoad("GlobalString");
	if (LoadSetup()==false)
	{
		return false;
	}
	if( !LoadServerSetup("serverSetup.ini") )
	{
		return false;
	}

	InitCountryPlayerNum();

	//分配数据块
	m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize);
	

	//初始化副本的场景配置
	m_pDupliRegionSetup = new CDupliRegionSetup;
	if(m_pDupliRegionSetup->Load() == false)
	{
		MessageBox(g_hWnd, "Can't find file setup/DupliRegionsSetup.ini", "message", MB_OK);
		return false;
	};

	CMessage::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum);
	// 初始化服务器网络
	CMySocket::MySocketInit();  
	//装载内存影射文件配置
	LoadMapFileSetup();
	// 创建服务器端
	if( !InitNetServer() )
	{
		MessageBox(g_hWnd, "Can't init NetServer!", "Message", MB_OK);
		return false;
	}

	if(!StartAccept())
	{
		return false;
	}

	// 连接到LoginServer
	if (0 == GetSetup()->lUseLogServer)
	{
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOGSERVERINVALID));
	}
	else
	{
		// 连接到LogServer
		if( !StartConnLogS() )
		{
			MessageBox(g_hWnd, "Can't connect to LogServer, please run LogServer first!", "Message", MB_OK);
			CreateConnectLogThread();
		}
	}

	// 连接到DBServer
	if( !StartConnDBS() )
	{
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_CANTCONNWS));
		CreateConnectDBThread();
	}
	AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_CONNDBSOK));
	if( !StartConnLS() )
	{
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_CANTCONNLS));
		CreateConnectLoginThread();
	}

	AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_CONNLSOK));

	LoadEx(RELOAD_PLAYER_MAN);			// Fox@20081120 玩家管理
	LoadEx(RELOAD_PET_LIST);			// Fox@20081119 宠物

	LoadEx(RELOAD_ANTI_CHEAT);          // BaiYun@2009525 反外挂
	GetInst( AntiCheat::CFileListManager ).Init();
	GetInst( AntiCheat::CQuestionManager ).Startup();
	ReLoad("CreditSystem");
	ReLoad("FuncSetup");
	ReLoad("GlobeSetup");
	ReLoad("PlayerList");
	ReLoad("GoodsList");
	ReLoad("SuitSetup");
	ReLoad("EnchaseSetup");
	ReLoad("TradeAreaSetup");
	
	ReLoad("CountTimerList");

	ReLoad("GoodsExSetup");
	ReLoad("GoodsTailor");
	ReLoad("DepotSetup");
	ReLoad("LinkmanSetup");
	ReLoad("MonsterList");
	ReLoad("GateList");
	ReLoad("TradeList");
	ReLoad("IncrementShopList");
	ReLoad("ContributeSetup");
	ReLoad("SkillList");
	ReLoad("SysMail");

	ReLoad("LogSystem");
	ReLoad("ScriptFile");
	ReLoad("GMList");
	GetGame()->GetGlobalRgnManager()->LoadRgnSetupIni("setup/regionlist.ini");
	GetGame()->GetGlobalRgnManager()->LoadDupRgnServerIni("data/duprgnserver.ini");
	GetGame()->GetGlobalRgnManager()->LoadRegionPlugSetupIni("setup/regionplug.ini");

	ReLoad("RegionLevelSetup");
	ReLoad("HitLevelSetup");
	ReLoad("Help");
	ReLoad("Broadcast");
	ReLoad("SpriteSystem");
	ReLoad("scriptai");
	ReLoad("ReliveGoods");
	ReLoad("Cosmetology");
	ReLoad( "QuestIndex" );
	ReLoad( "Lottery" );
	LoadEx( RELOAD_FBUSINESS ); // 家族跑商	

	//装载无效文字系统
	CWordsFilter::GetInstance()->Initial(string("setup/InvalidStr.ini"));
	//初始化组织系统的参数
	WSOrganizingParam::getInstance().Initialize();
	//初始化组织系统
	GetOrganiCtrl()->Initialize();

	CMsgTimer* msgNoonTimer=GetGame()->AllocFreeMsgTimer();
	msgNoonTimer->SetDesSocketID(GetGame()->GetDBSocketID());
	msgNoonTimer->SetMsgType(MSG_W2S_SERVER_BUSINESS_UPDATE);	
	SetNoonTimer(msgNoonTimer);

	//装载任务列表
	CQuestSystem::Init("quest/QuestList.xml");
	CDeputyOccuSystem::Init("data/DeputyOccu/DeputyOccu.xml");


	//初始化国家的参数
	GetCountryParam()->Initialize();
	GetCountryHandler()->Initialize();
	CIncrementShopManager::Init();

	//! 战队系统
	GetInst(CSentaiSystem).Init("data/Sentai.xml");

	// 全局脚本变量
	m_pGeneralVariableList = MP_NEW CVariableList;
	m_pGeneralVariableList->LoadVarList("data/general_variable.ini", "GeneralVariableList");
	UnloadServerResource();

	// Fox@20081110 玩家管理, 暂只用于在线玩家搜索
	GetInst(CPlayerMan).Initial();
	CSMClient::GetSM()->InitOk();
	ResetStarDay(time(NULL));
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
			if (stricmp(strExtName, ".script")==0)
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
bool CGame::LoadScriptFileData(const char* dir_name, const char* function_filename, const char* var_filename, const char* gen_var_filename)
{
	// 先清除原有数据
	M_FREE(m_strFunctionList_FileData,m_lFunctionList_FileData_Size+1);
	M_FREE(m_strVariableList_FileData,m_lVariableList_FileData_Size+1);
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
		m_strFunctionList_FileData = (char*)M_ALLOC(rfile->GetDatalen()+1);
		memset(m_strFunctionList_FileData, 0, rfile->GetDatalen()+1);
		rfile->ReadData(m_strFunctionList_FileData, m_lFunctionList_FileData_Size);
		rfClose(rfile);
	}
	else
	{
		char str[256];
		sprintf(str, "Can't found %s!", function_filename);
		MessageBox(g_hWnd, str, "Message", MB_OK);
		return false;
	}

	// 读入variablelist.ini
	rfile = rfOpen(var_filename);
	if( rfile )
	{
		m_lVariableList_FileData_Size = rfile->GetDatalen();
		m_strVariableList_FileData = (char*)M_ALLOC(m_lVariableList_FileData_Size+1);
		memset(m_strVariableList_FileData, 0, m_lVariableList_FileData_Size+1);
		rfile->ReadData(m_strVariableList_FileData, m_lVariableList_FileData_Size);
		rfClose(rfile);
	}
	else
	{
		char str[256];
		sprintf(str, "Can't found %s!", var_filename);
		MessageBox(g_hWnd, str, "Message", MB_OK);
		return false;
	}

	// 读取Scripts/目录中的所有脚本文件
	list<string> listFileName;
	string strTemptFolder("scripts/*.*");
	CheckRFileStr(strTemptFolder);
	string strTempt(strTemptFolder);
	int nPos = strTemptFolder.find_last_of("\\");
	strTempt.erase(0,nPos+1);
	if(strTempt=="*.*")
		strTemptFolder.erase(nPos,strTemptFolder.length()-nPos);
	if(CClientResource::GetInstance()->IsFileExist(strTemptFolder.c_str()) )
	{
		string strTemptExt(".script");
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
		char str[256];
		sprintf(str, "Can't found %s!", filename);
		AddLogText(str);
		return false;
	}

	return true;
}

bool CGame::ReLoadOneScript(const char* filename)
{
	if (LoadOneScript(filename))
	{
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_RELOADSCRIPTOK), filename);
		CMessage msgScript(MSG_W2S_SERVER_SENDSETUP);
		msgScript.Add((long)SI_SCRIPTFILE);
		msgScript.Add(filename);
		msgScript.Add((long)lstrlen(GetScriptFileData(filename)));
		msgScript.Add(GetScriptFileData(filename));
		msgScript.SendAll();
		return true;
	}
	else
	{
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_RELOADSCRIPTFAIL), filename);
		return false;
	}
}

bool CGame::ReLoadOneRegionSetup(long lRegionID)
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
		msg.Add((long)SI_REGIONSETUP);
		msg.Add(lRegionID);
		msg.AddEx(&vectorByte[0], (long)vectorByte.size());
		msg.SendGSID(ptagRegion->dwGameServerIndex);
		return true;
	}
	return false;
}

bool CGame::ReLoadAllRegionSetup()
{
	map<long, CGlobalRgnManager::tagRegion>::iterator it=GetGlobalRgnManager()->GetTemplateRegionListMap()->begin();
	for(;it != GetGlobalRgnManager()->GetTemplateRegionListMap()->end();it++)
	{
		CGlobalRgnManager::tagRegion& temptRegion = (*it).second;
		if( temptRegion.pRegion)
		{
			CGUID lRegionID=temptRegion.pRegion->GetExID();
			temptRegion.pRegion->LoadSetup();

			vector<BYTE> vectorByte;
			temptRegion.pRegion->AddSetupToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_REGIONSETUP);
			msg.Add(lRegionID);
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendGSID(temptRegion.dwGameServerIndex);
		}
	}
	return true;
}

bool CGame::ReLoadAllRegion()
{
	map<long, CGlobalRgnManager::tagRegion>::iterator it=GetGlobalRgnManager()->GetTemplateRegionListMap()->begin();
	for(;it != GetGlobalRgnManager()->GetTemplateRegionListMap()->end();it++)
	{
		CGlobalRgnManager::tagRegion& temptRegion = (*it).second;
		if( temptRegion.pRegion)
		{
			CGUID lRegionID=temptRegion.pRegion->GetExID();
			temptRegion.pRegion->Load();

			vector<BYTE> vectorByte;
			temptRegion.pRegion->AddSetupToByteArray(&vectorByte);
			temptRegion.pRegion->AddToByteArray(&vectorByte);

			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_REGIONRELOAD);
			msg.Add(lRegionID);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendGSID(temptRegion.dwGameServerIndex);
		}
	}
	return true;
}

// 重读设置
BOOL CGame::ReLoad(const char* name, bool bSendToGameServer,bool bReloadFileInfo)
{
	BOOL ret=false;

	//重读文件之前，重读资源信息列表

	if (stricmp(name, "GlobalString")==0)
	{
		//读取全局字符串
		ret = CStringReading::InitUniString("textresource/globaltextres.xml","GlobalFile");
		ret = ret | (BOOL)CStringReading::InitUniString("textresource/c_globaltextres.xml","GlobalFile");
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGOLBALSTRINGOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGOLBALSTRINGFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CStringReading::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_GLOBALSTRING);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "CountTimerList")==0)
	{
		// 读入玩家定时器配置
		ret = CCountTimerList::LoadSetup( "data/CountTimerList.ini" );
		(ret)?AddLogText("读取玩家定时器列表成功！"):AddLogText("读取玩家定时器列表失败！");
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CCountTimerList::Serialize( vectorByte );
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_COUNTTIMERLIST);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "PlayerList")==0)
	{
		// 读入玩家列表
		// changed by chenaly.................................................................
		ret = ret | (BOOL)CPlayerList::LoadPlayerExpList("data/LevelExp/PlayerLevelExp.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADEXPLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, 13));
		ret = ret | (BOOL)CPlayerList::LoadOccuLvlExpList();
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_OCCUEXP_OK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_OCCUEXP_FAILED));
		ret=CPlayerList::LoadOccuModifyProperty("data/Player_occupation_Property.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADLEVELLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADLEVELLISTFAIL));
		ret=CPlayerList::LoadOccuDefParam("data/Player_OccuDef_Praram.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADOCCUDEFLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADOCCUDEFLISTFAIL));
		ret=CPlayerList::LoadOccuAtkParam("data/Player_OccuAtk_Praram.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADOCCUATTRLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADOCCUATTRLISTFAIL));
		//end change..............................................................................
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CPlayerList::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_PLAYERLIST);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}

		ret = CEmotion::LoadSetup( "data/Emotions.ini" );
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADFACELISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADFACELISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CEmotion::Serialize( vectorByte );
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_EMOTION);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "GoodsList")==0)
	{
		// 读入原始物品列表
		ret = CGoodsFactory::Load( "data/goodslist.dat" );
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGOODSLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGOODSLISTFAIL));
		ret=CGoodsFactory::LoadAttrDBSetup("data/GoodsAttrDBSetup.ini");
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CGoodsFactory::Serialize( &vectorByte );
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_GOODSLIST);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "SuitSetup")==0)
	{		
		ret = CSuitRuleFactory::Load( "data/suit.dat" );
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADCOMQUIPMENTLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADCOMQUIPMENTLISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CSuitRuleFactory::Serialize( &vectorByte );
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_SUITSETUP);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "EnchaseSetup")==0)
	{
		ret = CKitCardRuleFactory::Load( "data/enchase.ini" );
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADHOLELISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADHOLELISTFAIL));
		if(ret && bSendToGameServer)
		{			
			vector<BYTE> vectorByte;
			CKitCardRuleFactory::Serialize( &vectorByte );
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_ENCHASESETUP);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if(stricmp(name,"TradeAreaSetup")==0)
	{
		// 商业配置
		ret=GetInst( Business::CBusinessManager ).Load();
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGTREATEAREALISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADTREATEAREALISTFAIL));
		if(ret && bSendToGameServer)
		{		
			// 需要将数据发给对应的GS
			for( map<DWORD, tagGameServer>::iterator it = s_mapGameServer.begin(); it != s_mapGameServer.end(); ++ it )
			{
				DWORD dwGsId = it->first;

				CMessage msg(MSG_W2S_SERVER_SENDSETUP);
				msg.Add((long)SI_TRADEAREASETUP);
				DBWriteSet db;
				msg.GetDBWriteSet( db );
				GetInst( Business::CBusinessManager ).EncodeToDataBlock( db, dwGsId );;
				msg.SendGSID( dwGsId, false );
			}
		}
	}
	else if (0 == stricmp(name, "GoodsExSetup"))
	{
		// 读入物品扩展配置
		if (NULL == m_pGoodsExSetup)
		{
			m_pGoodsExSetup = new GoodsExSetup();
		}

		do 
		{
			try
			{
				if(0 == m_pGoodsExSetup->ReadSetup("data/GoodsExSetup.xml"))
				{
					if(m_pGoodsExSetup->InitServerSetup())
					{
						vector<BYTE> vectorByte;
						SerializeGoodsExSetup(vectorByte);
						CMessage msg(MSG_W2S_SERVER_SENDSETUP);
						msg.Add((long)SI_GOODSEX);
						msg.Add((long)vectorByte.size());
						msg.AddEx(&vectorByte[0], (long)vectorByte.size());
						msg.SendAll();
						AddLogText(CStringReading::LoadString(10, 200));
						ret = TRUE;
						break;
					}
				}
			}
			catch (...)
			{
			}

			AddLogText(CStringReading::LoadString(10, 210));
#ifdef _DEBUG
			ret = TRUE;
#else
			ret = FALSE;
#endif
		} while(false);
	}
	else if (0 == stricmp(name, "GoodsTailor"))
	{
		ret = LoadGoodsTailorSetup("data/GoodsTailor.xml");
	}
	else if (0 == stricmp(name, "DepotSetup"))
	{
		if(PlayerDepot::LoadSetup("data/Depot.xml"))
		{
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_DEPOT);
			msg.Add((long)sizeof(tagDepotSetup));
			msg.AddEx((void*)PlayerDepot::GetDepotSetup(), sizeof(tagDepotSetup));
			msg.SendAll();
			AddLogText(CStringReading::LoadString(12, 10));
			ret = TRUE;
		}
		else
		{
			AddLogText(CStringReading::LoadString(12, 20));
			ret = FALSE;
		}

	}
	else if (0 == stricmp(name, "LinkmanSetup"))
	{
		if(LinkmanSystem::GetInstance().LoadSetupAndInit("data/LinkmanSetup.xml"))
		{
			AddLogText(CStringReading::LoadString(14, 10));
			ret = TRUE;
		}
		else
		{
			AddLogText(CStringReading::LoadString(14, 20));
#ifdef _DEBUG
			ret = TRUE;
#else
			ret = FALSE;
#endif
		}
	}
	else if (stricmp(name, "MonsterList")==0)
	{
		// 读入怪物列表
		ret = CMonsterList::LoadMonsterList("data/monsterlist.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADMONSTERLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADMONSTERLISTFAIL));
		ret = ret & (BOOL)CMonsterList::LoadMonsterDrop("data/DropList.xml");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADMGOODSLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADMGOODSLISTFAIL));
		ret = ret & (BOOL)CMonsterList::LoadActivityDrop("data/ActivityDropList.xml");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADQUESTGOODSLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADQUESTGOODSLISTFAIL));

		ret= CMonsterList::LoadSkillList("data/MonsterSkillList.xml");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADMSKILLLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADMSKILLLISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CMonsterList::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_MONSTERLIST);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if(stricmp(name,"GateList")==0)
	{
		ret=CGateList::LoadGateList("data/buildlist.ini");
	}
	else if (stricmp(name, "TradeList")==0)
	{
		// 读入买卖列表
		ret = CTradeList::LoadTradeList("data/tradelist.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADTREADELISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADTREADELISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CTradeList::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_TRADELIST);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name,"SysMail")==0)
	{
		ret = CMailList::LoadSysMailList("mail/sysmail.xml");
		GetGame()->GetMailManager()->InitSysMail();
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADSYSMAILOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADSYSMAILFAIL));
		if( ret && bSendToGameServer )
		{
			vector<BYTE> vContent;
			CMailList::AddToByteArray(&vContent);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add(static_cast<long>(SI_SYSMAIL));
			msg.Add(static_cast<long>(vContent.size()));
			msg.AddEx(&vContent[0],static_cast<long>(vContent.size()));
			msg.SendAll();			
		}
	}
	else if (stricmp(name, "SkillList")==0)
	{
		
		ret = CNewSkillFactory::LoadNewSkillCache();
		ret = ret &  (BOOL)CStudySkillCondition::Init("data/StudySkillCondition.xml");
		ret = ret &  (BOOL)COccupSkillList::Init("data/OccupSkillList.xml");
		ret = ret &  (BOOL)CStudySkillCondition::InitSkillList("data/TeacherSkill.xml");
		ret = ret &  (BOOL)CStudySkillCondition::InitFactureSkillList("data/factureskilllist.xml");

	
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADSKILLLISTOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADSKILLLISTFAIL));
		if( ret && bSendToGameServer )
		{
			vector<BYTE> vContent;			;
			CNewSkillFactory::CodeNewSkill(vContent);
			COccupSkillList::AddToByteArray(vContent);
			CStudySkillCondition::AddToByteArray(vContent);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add( static_cast<long>(SI_SKILLLIST) );
			msg.Add( static_cast<long>(vContent.size()) );
			msg.AddEx( &vContent[0], static_cast<long>( vContent.size() ) );
			msg.SendAll();
		}	

	}
	else if (stricmp(name, "GlobeSetup")==0)
	{


		// 读取全局参数表
		ret = CGlobeSetup::Load("data/globesetup.ini");
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGLOBALPARAMLISTOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGLOBALPARAMLISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CGlobeSetup::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_GLOBESETUP);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "GameSetup")==0)
	{
		// 读取GameSetup参数表
		ret = CGlobeSetup::LoadGameSetup("setup/gamesetup.ini");
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADEXPGOODSLISTOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADEXPGOODSLISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CGlobeSetup::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_GLOBESETUP);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
		RefeashInfoText();
	}
	else if( stricmp(name, "LogSystem") == 0 )
	{
		// 读取日志系统参数
		ret = CLogSystem::Load("setup/LogSystem.ini");
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADLOGLISTOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADLOGLISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CLogSystem::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_LOGSYSTEM);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "GMList")==0)
	{
		// 读取GM列表
		ret = CGMList::Load("setup/gmlist.ini");
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGMLISTOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADGMLISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CGMList::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_GMLIST);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if (stricmp(name, "ScriptFile")==0)
	{
		// 读取指定目录中的所有脚本文件(包含子目录)
		ret = LoadScriptFileData("scripts/", "data/function.ini", "data/variable.ini", "data/general_variable_data.ini");
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADSCRIPTLISTOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADSCRIPTLISTFAIL));
		if (ret && bSendToGameServer)
		{
			// FunctionList
			if (GetFuncitonList_FileData())
			{
				CMessage msgFunction(MSG_W2S_SERVER_SENDSETUP);
				msgFunction.Add((long)SI_FUNCTIONLIST);
				msgFunction.Add((long)lstrlen(GetFuncitonList_FileData()));
				msgFunction.Add(GetFuncitonList_FileData());
				msgFunction.SendAll();
			}

			// VariableList
			if (GetVariableList_FileData())
			{
				CMessage msgVariable(MSG_W2S_SERVER_SENDSETUP);
				msgVariable.Add((long)SI_VARIABLELIST);
				msgVariable.Add((long)lstrlen(GetVariableList_FileData()));
				msgVariable.Add(GetVariableList_FileData());
				msgVariable.SendAll();
			}

			for(map<string, char*>::iterator it2=GetGame()->GetScriptFileDataMap()->begin(); it2!=GetGame()->GetScriptFileDataMap()->end(); it2++)
			{
				long len = (long)lstrlen(it2->second);
				string filename = it2->first;
				CMessage msgScript(MSG_W2S_SERVER_SENDSETUP);
				msgScript.Add((long)SI_SCRIPTFILE);
				msgScript.Add(filename.c_str());
				msgScript.Add(len);
				msgScript.AddEx(it2->second, len);
				msgScript.SendAll();
			}
		}
	}
	else if (stricmp(name, "RegionList")==0)
	{
		// 读入场景列表
		g_GlobalRgnManager->LoadRgnSetupIni("setup/regionlist.ini");
	}
	else if (stricmp(name, "RegionLevelSetup")==0)
	{
		// 读入场景列表
		ret = CRegionSetup::LoadRegionSetup("data/regionsetup.ini");
		(ret) ? AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADRGNLEVELLISTOK)) : AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADRGNLEVELLISTFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CRegionSetup::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_REGIONLEVELSETUP);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if(stricmp(name,"HitLevelSetup")==0)
	{

		ret=CHitLevelSetup::LoadHitLevelSetup("data/hitlevel.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADHITLEVELLISTOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADHITLEVELLISTFAIL));

		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CHitLevelSetup::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_HITLEVEL);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0],(long)vectorByte.size());
			msg.SendAll();
		}
	}
	//读取系统广播的配置
	else if(stricmp(name,"Broadcast")==0)
	{
		char filename[] = "setup/sysboardcast.ini";

		ifstream stream;
		stream.open(filename);
		if (!stream.is_open())
		{
			char str[256];
			sprintf(str, "file '%s' can't found!", filename);
			MessageBox(g_hWnd, str, "error", MB_OK);
			return FALSE;
		}

		m_listBroadcast.clear();

		while (ReadTo(stream, "#"))
		{
			long a,r,g,b, ba,br,bg,bb;
			tagSysBroadcast stSysBroadcast;			
			stream >> stSysBroadcast.lImportLevel
				>> stSysBroadcast.lRegionID
				>> stSysBroadcast.dwMinTime
				>> stSysBroadcast.dwMaxTime
				>> stSysBroadcast.dwOdds
				>> a >> r >> g >> b
				>> ba >> br >> bg >> bb;

			char str[1000];
			stream.ignore();
			stream.getline(str, 1000);
			stSysBroadcast.strMessage = str;

			stSysBroadcast.dwTextColor = MAKELONG(MAKEWORD(b,g), MAKEWORD(r,a));
			stSysBroadcast.dwBackColor = MAKELONG(MAKEWORD(bb,bg), MAKEWORD(br,ba));

			stSysBroadcast.dwIntervalTime = stSysBroadcast.dwMinTime + random(stSysBroadcast.dwMaxTime - stSysBroadcast.dwMinTime);
			stSysBroadcast.dwLastNotifyTime=timeGetTime() / 1000;
			m_listBroadcast.push_back(stSysBroadcast);
		}
		stream.close();

		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADBROADCASTLISTOK));
	}

	else if(stricmp(name,"InvalidStr")==0)
	{
		if(CWordsFilter::GetInstance()->ReloadFilter())
			AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADINVALIDWORDOK));
	}
	else if(stricmp(name,"GeneralVariableList")==0)
	{
		CVariableList *pVarList = GetGeneralVariableList();
		if (pVarList)
		{
			pVarList->ReLoadVarList("data/general_variable.ini", "GeneralVariableList");	
		}

		AddLogText("重读并初始化全局脚本列表...ok!");
	}
	else if( stricmp(name,"FactionPara") == 0 )
	{
		WSOrganizingParam::getInstance().Initialize();
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADFACTIONPARAMOK));
	}
	else if( stricmp(name,"QuestData") == 0)
	{

		if(CQuestSystem::Init("quest/QuestList.xml"))
		{
			AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADQUESTPARAMOK));
		}
		else
		{
			AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADQUESTPARAMFAIL));
		}


		vector<BYTE> vectorByte;
		CQuestSystem::AddToByteArray(&vectorByte);
		CMessage msg(MSG_W2S_SERVER_SENDSETUP);
		msg.Add((long)SI_QUEST);
		msg.Add((long)vectorByte.size());
		msg.AddEx(&vectorByte[0], (long)vectorByte.size());
		msg.SendAll();
	}
	else if( stricmp(name,"CountryParam") == 0)
	{
		GetCountryParam()->Load();
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADCOUNTRYPARAMOK));
	}
	else if( stricmp(name,"CountryPara") == 0 )
	{
		GetCountryParam()->Load();
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADCOUNTRYPARAMFAIL));
	}

	else if( stricmp(name,"IncrementShopList") == 0 )
	{
		ret = CIncrementShopList::LoadIncrementShopList("data/incrementshoplist.xml");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADINCGOODSOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADINCGOODSFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CIncrementShopList::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_INCREMENTSHOPLIST);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if( stricmp(name,"ContributeSetup") == 0 )
	{
		ret = CContributeSetup::LoadContributeSetup("data/ContributeSetup.ini");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADCOUNTRYCONTRIBUTEGOODSOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADCOUNTRYCONTRIBUTEGOODSFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CContributeSetup::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_CONTRIBUTEITEM);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}
	else if( stricmp(name,"DeputyOccuList") == 0 )
	{
		ret = CDeputyOccuSystem::Init("data/DeputyOccu/DeputyOccu.xml");
		(ret)?AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADDOCCUOK)):AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADDOCCUFAIL));
		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CDeputyOccuSystem::AddToByteArray(&vectorByte);
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_DEPUTYOCCU);
			msg.Add((long)vectorByte.size());
			msg.AddEx(&vectorByte[0], (long)vectorByte.size());
			msg.SendAll();
		}
	}

	else if( stricmp(name,"SpriteSystem") == 0 )
	{
		SpriteSystem::LoadSetup();
	}

	else if( stricmp(name,"duprgnserver") == 0 )
	{
		GetGlobalRgnManager()->LoadDupRgnServerIni("data/duprgnserver.ini");
	}
	else if( stricmp( name, "scriptai" ) == 0 )
	{
		ret = CScriptAILoader::Instance().Load( "data/MonsterScriptAI.xml" );
		if( ret && bSendToGameServer )
		{
			vector<BYTE> vectorByte;
			CScriptAILoader::Instance().AddToByteArray( vectorByte );
			CMessage msg( MSG_W2S_SERVER_SENDSETUP );
			msg.Add( (long)SI_SCRIPTAI );
			msg.Add( (long)vectorByte.size() );
			msg.AddEx( &vectorByte[0], (long)vectorByte.size() );
			msg.SendAll();
		}
	}
	else if( stricmp(name,"ReliveGoods") ==0)
	{
		ret = CReLiveGoodsList::Init("data/ReliveGoodsList.xml");
		if( ret && bSendToGameServer )
		{
			vector<BYTE> vectorByte;
			CReLiveGoodsList::AddToByteArray( vectorByte );
			CMessage msg( MSG_W2S_SERVER_SENDSETUP );
			msg.Add( (long)SI_RELIVEGOODS );
			msg.Add( (long)vectorByte.size() );
			msg.AddEx( &vectorByte[0], (long)vectorByte.size() );
			msg.SendAll();
		}	
	}
	else if (stricmp(name,"CreditSystem")==0)
	{
		ret=GetInst(CCreditSystem).LoadAreaCredit();
		ret?AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOADCREDITSYSTEM_OK)):
			AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOADCREDITSYSTEM_FAILED));
		if (ret && bSendToGameServer)
		{
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_CREDITSYSTEM);
			DBWriteSet dbWriteSet;
			msg.GetDBWriteSet(dbWriteSet);
			GetInst(CCreditSystem).CodeToByteArray(dbWriteSet);
			msg.SendAll();
		}
	}
	else if (stricmp(name,"Cosmetology")==0)
	{
		ret = CCosmetology::InitCosmetology("data/Cosmetology.xml");
		ret?AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOAD_COSMETOLOGY_OK)):
			AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOAD_COSMETOLOGY_FAILED));
		

		if (ret && bSendToGameServer)
		{
			vector<BYTE> vectorByte;
			CCosmetology::AddToByteArray( vectorByte );
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_COSMESYSTEM);
			msg.Add( (long)vectorByte.size() );
			msg.AddEx( &vectorByte[0], (long)vectorByte.size() );
			msg.SendAll();
		}		
	}
	
	else if(stricmp(name,"QuestIndex")==0)
	{
		ret=GetInst(CQuestIndexXml).LoadQuestSetupXmlForServer();
		ret?AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOAD_QUESTINDEX_OK)):
			AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOAD_QUESTINDEX_FAILED));
		if (ret && bSendToGameServer)
		{
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_QUESTINDEX);
			DBWriteSet dbWriteSet;
			msg.GetDBWriteSet(dbWriteSet);
			GetInst(CQuestIndexXml).CodeToByteArray(dbWriteSet);
			msg.SendAll();
		}
	}

	else if(stricmp(name,"Lottery")==0)
	{
		ret=GetInst(CLotteryXml).LoadLotteryXml();
		ret?AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOAD_CLOTTERYXML_OK)):
			AddLogText(CStringReading::LoadString(IDS_WS_GAME,STR_WS_GAME_LOAD_CLOTTERYXML_FAILED));
		if (ret && bSendToGameServer)
		{
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_Lottery);
			DBWriteSet dbWriteSet;
			msg.GetDBWriteSet(dbWriteSet);
			GetInst(CLotteryXml).CodeToByteArray(dbWriteSet);
			msg.SendAll();
		}
	}

	return ret;
}

// 扩展的配置读取
bool CGame::LoadEx(unsigned uFlag, long lSocketID, bool bReload)
{
	bool ret = false;
	DWORD dwStrID = 0;

	if( bReload )
	{
		switch(uFlag)
		{
		case RELOAD_PLAYER_MAN:	// 载入玩家管理配置
			{
				ret = GetInst(CPlayerMan).LoadConfig();
				dwStrID = ret ? STR_WS_GAME_PM_OK : STR_WS_GAME_PM_FAILED;
			}
			break;

		case RELOAD_PET_LIST:		// 载入宠物配置
			{
				ret = GetInst(CPetCtrl).LoadConfig();
				dwStrID = ret ? STR_WS_GAME_PET_OK : STR_WS_GAME_PET_FAILED;
			}
			break;

		// BaiYun@2009525 读取反外挂配置
		case RELOAD_ANTI_CHEAT:  
			{
				ret = GetInst(AntiCheat::CConfiger).Load();
				dwStrID = ret ? STR_WS_GAME_ANTI_CHEAT_OK : STR_WS_GAME_ANTY_CHEAT_FAILED;
			}
			break;
		case RELOAD_FBUSINESS:
			{
				ret = GetInst( FBusiness::CConfiger ).Load();
			}
			break;

		default:
			break;
		}

		if( dwStrID != 0 )
		{
			AddLogText(CStringReading::LoadString(IDS_WS_GAME, dwStrID));
		}
	}

	if( lSocketID != 0 )
	{
	CMessage msg(MSG_W2S_SERVER_SENDSETUP);
	DBWriteSet setWriteDB;

	switch(uFlag)
	{
	case RELOAD_PLAYER_MAN:
		{
				msg.Add((long)SI_PLAYER_MAN);
				msg.GetDBWriteSet(setWriteDB);
				GetInst(CPlayerMan).CodeToDataBlock(setWriteDB);
		}
		break;

	case RELOAD_PET_LIST:
		{
				msg.Add((long)SI_PET_LIST);
				msg.GetDBWriteSet(setWriteDB);
				GetInst(CPetCtrl).CodeToDataBlock(setWriteDB);
		}
		break;

		// BaiYun@2009525 反外挂
		case RELOAD_ANTI_CHEAT:  
			{
				msg.Add((long)SI_ANTICHEAT);
				msg.GetDBWriteSet(setWriteDB);
				GetInst(AntiCheat::CConfiger).EncodeToDataBlock(setWriteDB);
			}
			break;

	default:
		break;
	}

		if( -1 == lSocketID )
		{
		msg.SendAll();
		}
		else
		{
			msg.SendToSocket(lSocketID);
		}

	}

	return ret;
}

// 游戏释放
BOOL CGame::Release()
{
	PutDebugString("WorldServer Exiting...");

#ifdef _RUNSTACKINFO_
	CMessage::AsyCloseFile(GetGame()->GetStatckFileName());
#endif

	// Fox@20081110 玩家管理, 暂只用于在线玩家搜索
	GetInst(CPlayerMan).Release();

	GetInst( AntiCheat::CFileListManager ).Release();
	GetInst( AntiCheat::CQuestionManager ).Shutdown();
	GetInst(CCreditSystem).Release();
	GetInst(CQuestIndexXml).Release();
	// 等待重连LS线程结束
	bLSConnectThreadExit = true;
	WaitForSingleObject(hLSConnectThread, INFINITE);
	CloseHandle(hLSConnectThread);

	// 等待重连DBS线程结束
	bDBSConnectThreadExit = true;
	WaitForSingleObject(hDBSConnectThread, INFINITE);
	CloseHandle(hDBSConnectThread);

	// 关闭网络连接
	if( GetNetServer() )
	{
		s_pNetServer->DisConn(m_lLogingSocket);
		s_pNetServer->DisConn(m_lCostSocket);
		s_pNetServer->DisConn(m_lDBSocket);
		s_pNetServer->Exit();

		SAFE_DELETE(s_pNetServer);
	}

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

	M_FREE(m_strFunctionList_FileData,m_lFunctionList_FileData_Size+1);
	M_FREE(m_strVariableList_FileData,m_lVariableList_FileData_Size+1);
	for(map<string, char*>::iterator it=m_mapScript_FileData.begin(); it!=m_mapScript_FileData.end(); it++)
	{
		char* p2 = (*it).second;
		SAFE_DELETE(p2);
	}

	MP_DELETE(m_pGeneralVariableList);

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
	GetTimer()->Release();

	SAFE_DELETE(s_pNetServer);

	CNewSkillFactory::ClearNewSkillCache();

	CStudySkillCondition::Release();
	COccupSkillList::Release();
	CStringReading::Release();
	CMailList::Release();


	CGoodsFactory::Release();
	CSuitRuleFactory::Release();
	CKitCardRuleFactory::Release();
	SAFE_DELETE(m_pGoodsExSetup);
	CMonsterList::Release();
	CDeputyOccuSystem::Relese();
	//090602 by tanglei 
	CSMClient::GetSM()->ExitOk();
	CReLiveGoodsList::Release();
	CCosmetology::Release();

	CMySocket::MySocketClearUp();

	CMessage::Release();

	GetNetSessionManager()->Release();

	//! 新日志系统
	SAFE_DELETE(m_pLogicLogInterface);
	m_LogClient.Release();

	// 关闭玩家数据库载入线程
	for(DWORD i=0; i<g_vPlayerLoadThreads.size(); ++i)
	{
		TerminateThread(g_vPlayerLoadThreads[i],0);
		CloseHandle(g_vPlayerLoadThreads[i]);
	}

	CCardsHandler::delInstance();
	
	CSMClient::GetSM()->ReleaseSM();
	CMessage::Release();
	
	//释放资源管理器
	SAFE_DELETE(m_pDupliRegionSetup);
	SAFE_DELETE(m_pDBAllocator);

	GetSessionFactoryInst()->Release();
	SAFE_DELETE(m_pSessionFactory);
	SAFE_DELETE(g_GlobalRgnManager);
	SAFE_DELETE(g_EntityManager);
	

#ifdef __MEMORY_LEAK_CHECK__
	CBaseObject::OutputConstructRef(TYPE_GOODS);
#endif
	PutDebugString("WorldServer Exited!");
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
//建立和LoginServer的连接，作为客户端初始化
BOOL CGame::StartConnLS(void)
{
	return ReConnectLoginServer();
}
void CGame::OnDBSLost()
{
	m_lDBSocket = 0;
	CMessage::SetDBSocket(0);
	m_bDbReady = false;
}
void CGame::OnLSLost()
{
	m_lLogingSocket = 0;
	CMessage::SetLogingSocket(0);
}

void CGame::OnLOGSLost()
{
	m_LogClient.SetSoket(0);
	CMessage::SetLogSocket(0);
	CreateConnectLogThread();
}

//建立和DBServer的连接，作为客户端初始化
BOOL CGame::StartConnDBS(void)
{
	m_lDBSocket = s_pNetServer->Connect(GetSetup()->strDBIP.c_str(), GetSetup()->dwDBPort,NF_DBServer_Client);
	if(m_lDBSocket>0)
	{
		b_IsFirstConn = false;

		CMessage::SetDBSocket(m_lDBSocket);
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_CONNECTDBSOK));

		CMessage msg(MSG_W2DB_SERVER_SENDSERVERINFO);
		msg.Add((char)0);	// first connect
		msg.Add(GetSetup()->dwListenPort);
		msg.Add(GetSetup()->strLocalIP.c_str());
		msg.SendToSocket(m_lDBSocket);
		return TRUE;
	}
	return FALSE;
}
//建立和LogServer的连接，作为客户端初始化
bool CGame::StartConnLogS(void)
{
	m_LogClient.SetSoket(s_pNetServer->Connect(GetSetup()->strLogIP.c_str(), GetSetup()->dwLogPort,NF_LogServer_Client));
	if(m_LogClient.GetSoket() > 0)
	{
		CMessage::SetLogSocket(m_LogClient.GetSoket());
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_CONNECTLOGSOK));

		CMessage msg(MSG_02LOG_SERVER_LOGIN);
		msg.Add(GetSetup()->dClientNo);
		msg.SendToSocket(m_LogClient.GetSoket());

		return true;
	}
	return false;
}
//计费
bool CGame::StartConnCS()
{

	return false;
}


VOID   CGame::CreateConnectLoginThread()
{
	bLSConnectThreadExit=true;
	WaitForSingleObject(hLSConnectThread,INFINITE);
	CloseHandle(hLSConnectThread);

	bLSConnectThreadExit=false;

	unsigned threadID;
	hLSConnectThread = (HANDLE)_beginthreadex(NULL,0,ConnectLoginServerFunc,NULL,0,&threadID);
}
VOID   CGame::CreateConnectDBThread()
{
	bDBSConnectThreadExit=true;
	WaitForSingleObject(hDBSConnectThread,INFINITE);
	CloseHandle(hDBSConnectThread);

	bDBSConnectThreadExit=false;

	unsigned threadID2;
	hDBSConnectThread = (HANDLE)_beginthreadex(NULL,0,ConnectDBServerFunc,NULL,0,&threadID2);
}

void CGame::CreateConnectLogThread(void)
{
	m_LogClient.bConnectThreadExit=true;
	WaitForSingleObject(m_LogClient.hConnectThread,INFINITE);
	CloseHandle(m_LogClient.hConnectThread);

	m_LogClient.bConnectThreadExit=false;

	unsigned threadID3;
	m_LogClient.hConnectThread = (HANDLE)_beginthreadex(NULL,0,ConnectLogServerFunc,NULL,0,&threadID3);
}

void CGame::CreateConnectCostThread()
{
	bConnectThreadExit1=true;
	WaitForSingleObject(hConnectThread1,INFINITE);
	CloseHandle(hConnectThread1);
	bConnectThreadExit1=false;
	unsigned threadID1;
	hConnectThread1 = (HANDLE)_beginthreadex(NULL,0,ConnectCostServerFunc,NULL,0,&threadID1);	
}

bool CGame::ReConnectLoginServer()
{

	m_lLogingSocket = s_pNetServer->Connect(GetSetup()->strLoginIP.c_str(), GetSetup()->dwLoginPort, NF_LServer_Client);

	if(m_lLogingSocket>0)
	{
		CMessage::SetLogingSocket(m_lLogingSocket);
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_RECONNECTLSOK));

		SetServerInitSendFlag(Login_Server_InitSendFlag);
		return true;
	}
	return false;
}
bool CGame::ReConnectCostServer()
{
	return false;
}


bool CGame::ReConnectDBServer()
{
	m_lDBSocket = s_pNetServer->Connect(GetSetup()->strDBIP.c_str(), GetSetup()->dwDBPort, NF_DBServer_Client);

	if(m_lDBSocket>0)
	{
		CMessage::SetDBSocket(m_lDBSocket);
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_RECONNECTDBSOK));

		// 向DBS发送初始化数据
		CMessage msg(MSG_W2DB_SERVER_SENDSERVERINFO);
		if(b_IsFirstConn) // 是第一次连接
			msg.Add((char)0);
		else
			msg.Add((char)1);	// first connect
		msg.Add(GetSetup()->dwListenPort);
		msg.Add(GetSetup()->strLocalIP.c_str());
		msg.SendToSocket(m_lDBSocket);

		SetServerInitSendFlag(DB_Server_InitSendFlag);
		return true;
	}
	return false;
}


bool CGame::ReConnectLogServer()
{
	return GetLogClient().bConnectThreadExit = StartConnLogS();
}
// 发送当前WorldServer上的所有Cdkey到LoginServer
void CGame::SendCdkeyToLoginServer(BOOL bIsReady)
{
	// 发送当前worldserver上的cdkey
	// 把服务器基本信息发送到LOGINSERVER
	CMessage msg(MSG_W2L_SERVER_SENDWORLDINFO);
	msg.Add(GetSetup()->dwNumber);
	msg.Add(GetSetup()->strName.c_str());
	msg.Add((long)bIsReady);
	msg.Add((long)m_mPlayer.size());
	for(map<CGUID, CPlayer*>::iterator it=m_mPlayer.begin(); it!=m_mPlayer.end(); it++)
	{
		CPlayer* pPlayer = it->second;
		msg.Add(pPlayer->GetAccount());
	}
		msg.SendToSocket(m_lLogingSocket);
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
		MessageBox(g_hWnd, "Start NetServer Error", "Message", MB_OK);
		return FALSE;
	}
	CMessage::SetServer(s_pNetServer);
	AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_NETSERVERINITOK));
	return TRUE;
}

//开始作为服务器监听
bool CGame::StartAccept()
{
	if(NULL == s_pNetServer)	return false;
	BOOL bRet = s_pNetServer->Host(m_Setup.dwListenPort,NULL,NF_Server_Client,SOCK_STREAM);
	if(!bRet)
	{
		MessageBox(g_hWnd, "Start Host Error", "Message", MB_OK);
		return false;
	}
	AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_WSFORGSINITOK));
	return true;
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
	list<CGame::tagSysBroadcast>::iterator iter=GetGame()->GetBroadcastList()->begin();
	for(;iter!=GetGame()->GetBroadcastList()->end();iter++)
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
					msgBroadcast.SendAll();
				}
				else
					// 场景广播
				{
					tagGameServer *pGS = GetRegionGameServer(pst->lRegionID);
					if (pGS)
					{
						msgBroadcast.SendGSID(pGS->dwIndex);
					}
				}

				pst->dwLastNotifyTime = dwCurTim;
				pst->dwIntervalTime = pst->dwMinTime + random(pst->dwMaxTime - pst->dwMinTime);
			}
		}
	}

	return TRUE;
}

long g_lRebackMessageCount = 0;
// 消息处理
BOOL CGame::ProcessMessage()
{
	static MEMORYSTATUS msAA,msBB;

	long lMessageCount = GetNetServer()->GetRecvMessages()->GetSize();
	for(int i=0; i<lMessageCount; i++)
	{
		CMessage* pMsg = (CMessage*)GetNetServer()->GetRecvMessages()->PopMessage();
		if( pMsg )
		{
			long lMsgType = pMsg->GetType();
			DWORD dwStartTime = timeGetTime();
			GlobalMemoryStatus(&msAA);
			pMsg->Run();
			GlobalMemoryStatus(&msBB);

			m_mapMsgVirMemUseSize[lMsgType] +=  (msAA.dwAvailVirtual-msBB.dwAvailVirtual);

			AddMsgExpendTime(lMsgType,timeGetTime()-dwStartTime);
			CBaseMessage::FreeMsg(pMsg);
		}
	}
	CSMClient::GetSM()->ProcessMessage();
	return TRUE;
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
	if(m_ServerInitSendFlag & Login_Server_InitSendFlag)
	{
		// 检查是否所有GS都连接完成并且DBS准备完成, 成功则发送saveplayer缓冲区的数据
		bool isSaveFlag = false;
		map<DWORD, CGame::tagGameServer>::iterator gsItr = GetGame()->GetGameServerList()->begin();
		for(; gsItr != GetGame()->GetGameServerList()->end(); gsItr++)
		{
			if(gsItr->second.bConnected)
				isSaveFlag = true;
			else
			{
				isSaveFlag = false;
				break;
			}
		}
		if(isSaveFlag && GetGame()->GetDbIsReady())
		{
			SendCdkeyToLoginServer(TRUE);
		}
		else
			SendCdkeyToLoginServer(FALSE);

		
	}
	if(m_ServerInitSendFlag & Log_Server_InitSendFlag)
	{

	}
	if(m_ServerInitSendFlag & DB_Server_InitSendFlag)
	{
		//! 重新初始化组织系统
		GetOrganiCtrl()->Initialize();
	}
	m_ServerInitSendFlag = Null_Server_InitSendFlag;
	LeaveCriticalSection(&m_CSServerInitSendFlag);
}
// 游戏主循环
BOOL CGame::MainLoop()
 {

#ifdef _RUNSTACKINFO_
	CMessage::AsyClearFile(GetGame()->GetStatckFileName());
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MainLoop() Start.");
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

	//统计每个步骤的消耗
	static DWORD dwProcessServerInitTime = 0,
		dwReloadTime = 0,
		dwClearPlayerTime = 0,
		dwSaveTime = 0,
	    dwAITime = 0,
		dwSessionTime = 0,
		dwClearOutPlayerTime = 0,
		dwMessageTime = 0,
		dwMsgTimerTime = 0,
		dwMailTimerTime,
		dwTimerTime = 0,
		dwNetSessionFactionTime = 0,
		dwOrgRunCtrlTime = 0,
		dwProcessTimeOutLoginPlayerTime = 0,
		dwLotteryTime = 0;


		
	
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
	//统计Server配置消息消耗时间
	dwCurTime = timeGetTime();
	ProcessServerInitSendMsg();
	dwProcessServerInitTime += timeGetTime() - dwCurTime;

	GlobalMemoryStatus(&msB);
	dwProcessServerInitSize += (msA.dwAvailVirtual-msB.dwAvailVirtual);

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"RefeashInfoText() Start.");
#endif
	// 刷新显示
	dwCurTime = timeGetTime();
	if (now - lastRefeashInfoTime > GetSetup()->dwRefeashInfoTime)
	{
		lastRefeashInfoTime = now;
		RefeashInfoText();
	}
	GlobalMemoryStatus(&msA);
	dwRefreshSize += (msB.dwAvailVirtual-msA.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"RefeashInfoText() End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"reload_profiles() Start.");
#endif
	//统计重读配置文件消耗
	dwCurTime = timeGetTime();
	reload_profiles();
	dwReloadTime += timeGetTime() - dwCurTime;
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"reload_profiles() End.");
#endif

	if(g_bKickAllPlayer == true)
	{
		g_bKickAllPlayer = false;
		KickAllPlayer();
		m_bIsKickPlayerFlag = true;
	}


#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ChangeRgnPlayerTimeCheck() Start.");
#endif

	GlobalMemoryStatus(&msA);
	// 清除部分数据
	// 统计清除部分数据的消耗
	dwCurTime = timeGetTime();
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
							CPlayer* pPlayer = GetGame()->GetMapPlayer(playerItr->first);
							if(pPlayer)
							{
								GetGame()->OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
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
	dwClearOutPlayerTime += timeGetTime() - dwCurTime;

	GlobalMemoryStatus(&msB);
	dwClearOutPlayerSize += (msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ChangeRgnPlayerTimeCheck() End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DoSaveData() Start.");
#endif
	//到达存盘点
	//统计存盘消耗
	dwCurTime = timeGetTime();
	if(g_bSaveNowData==true || (now - lastSaveTime > CGlobeSetup::GetSetup()->dwSavePointTime) )
	{
		lastSaveTime = now;

		DoSaveData(false);

		//! 内存信息输出
		//!-----------------------------
		CGoodsFactory::OutputUseGoodsInfo("MemAllocInfo");
		CMailSystem::GetInstance()->OutMailInfo();
		LinkmanSystem::GetInstance().OutLinkmanInfo();
		GetOrganiCtrl()->OutOrganiInfo();
		
		GetEntityManager()->OutEntityInfo();
		CSessionFactory::OutSessionInfo();

		OutputCreatePlayerInfo();
		//!-----------------------------
		
		//输出网络调试信息
		GetGame()->GetNetServer()->OutputMsgStatInfo();
		m_pDBAllocator->PutAllocInfo("MemAllocInfo");

		char szInfo[1024]="";
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
		PutStringToFile("MemAllocInfo", szInfo);

		map<long,long>::iterator it = m_mapMsgVirMemUseSize.begin();
		for(;it != m_mapMsgVirMemUseSize.end();it++)
		{
			if((*it).second > 0)
			{
				char pszMsgSize[256]="";
				_snprintf(pszMsgSize,256,"MsgType:%d,MsgUseSize:%d",(*it).first,(*it).second);
				PutStringToFile("MemAllocInfo", pszMsgSize);
			}
		}
		m_mapMsgVirMemUseSize.clear();
		
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
		OUT_RUNNING_INFO(true);
	}
	dwSaveTime += timeGetTime() - dwCurTime;

	GlobalMemoryStatus(&msA);


	dwSaveSize += (msB.dwAvailVirtual-msA.dwAvailVirtual);
	
	//到达输出时间消耗点
	if( now - old > GetSetup()->dwSaveInfoTime )
	{

		char pszTimeInfo[1024]="";
		_snprintf(pszTimeInfo,1024,"%d seconds called %d time AI\r\nProcessServerInit:%d\r\nReload:%d\r\nChangRgnClearPlayer:%d\r\n\
Save:%d\r\nAI:%d\r\nAISession:%d\r\nClearOutPlayer:%d\r\nMessage:%d\r\nMsgTimer:%d\r\nMailTimer:%d\r\nTimer:%d\r\nNetSessionFaction:%d\r\n\
OrgRunCtrl:%d\r\nProcessTimeOutLoginPlayer:%d\r\nLottery:%d\r\n", 
			GetSetup()->dwSaveInfoTime/1000, ai_tick, dwProcessServerInitTime, dwReloadTime, dwClearPlayerTime, dwSaveTime, 
			dwAITime, dwSessionTime, dwClearOutPlayerTime, dwMessageTime, dwMsgTimerTime, dwMailTimerTime,
			dwTimerTime, dwNetSessionFactionTime, dwOrgRunCtrlTime, dwProcessTimeOutLoginPlayerTime,
			dwLotteryTime);

		//AddLogText(pszTimeInfo);
		PutStringToFile("TimeExpendInfo",pszTimeInfo);
		OutputMsgExpendTimeInfo("TimeExpendInfo");

		dwProcessServerInitTime = 0;
		dwReloadTime = 0;
		dwClearPlayerTime = 0;
		dwSaveTime = 0;
		dwAITime = 0;
		dwSessionTime = 0;
		dwClearOutPlayerTime = 0;
		dwMessageTime = 0;
		dwMsgTimerTime = 0;
		dwMailTimerTime = 0;
		dwTimerTime = 0;
		dwNetSessionFactionTime = 0;
		dwOrgRunCtrlTime = 0;
		dwProcessTimeOutLoginPlayerTime = 0;
		dwLotteryTime = 0;

		ai_tick = 0;
		old = now;
	}

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DoSaveData() End.");
#endif

	// 游戏主AI
	AddAITick();

	GlobalMemoryStatus(&msB);
	// 会话AI
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SessionAI() Start.");
#endif
	
	//统计会话消耗
	dwCurTime = timeGetTime();
	GetSessionFactoryInst()->AI();
	dwSessionTime += timeGetTime() - dwCurTime;

	GlobalMemoryStatus(&msA);
	dwNetSessionSize += (msB.dwAvailVirtual-msA.dwAvailVirtual);

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"SessionAI() End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Clear Data Start.");
#endif
	// 清除部分数据
	// 统计清除玩家部分数据时间消耗
	dwCurTime = timeGetTime();
	if(now - dwClearPlayerDataTime >= CGlobeSetup::GetSetup()->lWSSaveTime)
	{
		dwClearPlayerDataTime = now;
		// 执行清除
		CEntityManager::AccountMapItr deleteItr = GetEntityManager()->GetAccountMap().begin();
		while(deleteItr != GetEntityManager()->GetAccountMap().end())
		{	
			// dbs未连接
			if(!GetDbIsReady()) break;

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
							CPlayer* pPlayer = GetGame()->GetMapPlayer(guidItr->first);
							if(pPlayer)
							{
								GetGame()->OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
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
	dwClearPlayerTime += timeGetTime() - dwCurTime;

	GlobalMemoryStatus(&msB);	
	dwClearPlayerSize += (msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Clear Data End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"AI() Start.");
#endif
	//统计主AI消耗
	dwCurTime = timeGetTime();
	AI();
	dwAITime += timeGetTime() - dwCurTime;
	GlobalMemoryStatus(&msA);
	dwAISize += (msB.dwAvailVirtual-msA.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"AI() End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessMessage() Start.");
#endif
	//统计消息循环时间消耗
	dwCurTime = timeGetTime();
	ProcessMessage();
	dwMessageTime += timeGetTime() - dwCurTime;
	GlobalMemoryStatus(&msB);
	dwMessageSize += (msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessMessage() End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"m_MsgTimer.Expire() Start.");
#endif
	// 消息定时器处理 [AHC DBMSGTIMER]
	// 统计消息定时器时间消耗
	dwCurTime = timeGetTime();
	m_MsgTimer.Expire(dwCurTime);
	GetOrganiCtrl()->TimerRun(dwCurTime);
	GetInst(CSentaiSystem).TimerRun(dwCurTime);
	dwMsgTimerTime += timeGetTime() - dwCurTime;
	GlobalMemoryStatus(&msA);
	dwOrgSysSize += (msB.dwAvailVirtual-msA.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"m_MsgTimer.Expire() End.");
#endif


#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetMailManager().Expire() Start.");
#endif
	//统计邮件定时器时间消耗
	dwCurTime = timeGetTime();
	GetMailManager()->Run();
	dwMailTimerTime += timeGetTime() - dwCurTime;
	GlobalMemoryStatus(&msB);
	dwMailTimerSize  += (msA.dwAvailVirtual-msB.dwAvailVirtual);
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetMailManager().Expire() End.");
#endif

#ifdef _RUNSTACKINFO_
	if(now - dwClearSaveGoodsStackInfoTime > 3600*1000*4) // 4个小时
	{
		dwClearSaveGoodsStackInfoTime = now;
		CMessage::AsyClearFile(GetGame()->GetSaveGoodsStatckFileName());
	}
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetTimer()->Run() Start.");
#endif
	//时间计时器
	//统计时间定时器时间消耗
	dwCurTime = timeGetTime();
	GetTimer()->Run();
	dwTimerTime += timeGetTime() - dwCurTime;
	GlobalMemoryStatus(&msA);
	dwTimerSize += (msB.dwAvailVirtual-msA.dwAvailVirtual);
	
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetTimer()->Run() End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"AntiCheat::CQuestionManager::Run Start.");
#endif

	GetInst( AntiCheat::CQuestionManager ).Run();

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"AntiCheat::CQuestionManager::Run End.");
#endif
	//网络会话
	//统计网络会话时间消耗
	dwCurTime = timeGetTime();
	GetNetSessionManager()->Run();
	dwNetSessionFactionTime += timeGetTime() - dwCurTime;
	GlobalMemoryStatus(&msB);
	dwNetSessionFactionSize += (msA.dwAvailVirtual-msB.dwAvailVirtual);
	// 桢数控制
	static DWORD new_timer = timeGetTime();
	static DWORD old_timer = new_timer;
	//组织系统控制运行
	//统计组织系统时间消耗
	dwCurTime = timeGetTime();
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
	dwOrgRunCtrlTime += timeGetTime() - dwCurTime;

	new_timer = timeGetTime();
	dwCurTime = timeGetTime();
	if( new_timer - lastReleaseLoginPlayerTime > GetSetup()->dwReleaseLoginPlayerTime )
	{
		lastReleaseLoginPlayerTime = new_timer;
		ProcessTimeOutLoginPlayer();
	}
	dwProcessTimeOutLoginPlayerTime += timeGetTime() - dwCurTime;

	if(m_bIsKickPlayerFlag)
	{
		if(m_mPlayer.size() == 0)
		{
			CMessage msgexit(MSG_W2DB_SERVER_SAVEDATAEND);
			msgexit.Add((BYTE)1); // 下线保存标志
			msgexit.SendToSocket(GetDBSocketID());
			AddLogText("玩家数据下线保存全部完成！");
			m_bIsKickPlayerFlag = false;
		}
	}

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
		DWORD dwData = time(NULL);
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
			msg.SendAll();
		}
	}
	//彩票系统时间计算
	//统计彩票系统时间消耗
	dwCurTime = timeGetTime();
	CLotterySystem::GetInstance().Run();
	dwLotteryTime += timeGetTime() - dwCurTime;
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MainLoop() End.");
#endif

	return true;
}

void CGame::OutputMsgExpendTimeInfo(const char* pszFileName)
{
	PutStringToFile(pszFileName,"MsgExpendTime:");
	char pszInfo[256]="";
	map<long,long>::iterator it = m_mapMsgExpendTime.begin();
	for(;it != m_mapMsgExpendTime.end();it++)
	{
		if((*it).second > 0)
		{
			_snprintf(pszInfo,256,"MsgType:%d,ExpendTime;%d",(*it).first,(*it).second);
			PutStringToFile(pszFileName,pszInfo);
		}
	}
	m_mapMsgExpendTime.clear();

	PutStringToFile(pszFileName,"TimerExpendTime:");
	it = m_mapTimerExpendTime.begin();
	for(;it != m_mapTimerExpendTime.end();it++)
	{
		if((*it).second > 0)
		{
			long lType = (*it).first;
			_snprintf(pszInfo,256,"TimerType:%d,TimerlVar1:%d,ExpendTime;%d",(lType&0xFF),(lType>>16) ,(*it).second);
			PutStringToFile(pszFileName,pszInfo);
		}
	}
	m_mapTimerExpendTime.clear();

	PutStringToFile(pszFileName,"ChangeVarScriptExpendTime:");
	map<string,long>::iterator itr = m_mapVarScriptExpendTime.begin();
	for (;itr != m_mapVarScriptExpendTime.end();itr++)
	{
		if ((*itr).second > 0)
		{
			const string& strVar = (*itr).first;
			_snprintf(pszInfo,256,"VarName:%s,ExpendTime:%d",strVar.c_str(),(*itr).second);
			PutStringToFile(pszFileName,pszInfo);
		}
	}
	m_mapVarScriptExpendTime.clear();
}

bool CGame::LoadServerSetup(const char *filename)
{
	CRFile* prfile = rfOpen(filename);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "Can't find file %s", filename);
		MessageBox(g_hWnd, str, "message", MB_OK);
		return false;
	}

	string strTemp;
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	// 读游戏服务器信息
	tagGameServer stGS;
	long lGameServerNum = 0;
	stream >> strTemp >> lGameServerNum;
	for(int i=0; i<lGameServerNum; i++)
	{
		ReadTo(stream, "#");
		stream >> stGS.dwIndex >> stGS.strIP >> stGS.dwPort;
		stGS.bConnected = false;
		s_mapGameServer[stGS.dwIndex] = stGS;
	}
	AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_LOADWSSETUPOK));
	return true;
}

//! 读取非副职业制作配置，这个接口应该在物品配置完成后再调用
bool CGame::LoadGoodsTailorSetup(const char *filename)
{
	//! ----------------------------根节点----------------------------
	TiXmlNode *pNode = NULL;

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
		return false;
	}

	TiXmlDocument m_Tdoc(filename);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	pNode=m_Tdoc.FirstChild("GoodsTailor");
	if (pNode==NULL) 
		return false;

	TiXmlElement* pGoodsTailor = pNode->ToElement();//获取node 的指针
	if (pGoodsTailor==NULL) 
		return false;

	//! ----------------------------获取配置----------------------------
	GoodsTailorSetup::GetInstance().Clear();

	TiXmlElement* pStuffList = pGoodsTailor->FirstChildElement("StuffList");
	for (; pStuffList!=NULL; pStuffList = pStuffList->NextSiblingElement("StuffList"))
	{
		OneMakeTable *pOneMakeTable = new OneMakeTable();
		if(NULL != pOneMakeTable)
		{
			LONG lID				= atoi(pStuffList->Attribute("ID"));
			const char *pName		= pStuffList->Attribute("Name");
			const char *pNarrate	= pStuffList->Attribute("Narrate");

			if(pOneMakeTable->SetBaseInfo(lID, pName, pNarrate))
			{
				TiXmlElement* pStuff = pStuffList->FirstChildElement("Stuff");
				for (; pStuff!=NULL; pStuff = pStuff->NextSiblingElement("Stuff"))
				{
					tagExpend Expend;
					const char *pGoodsName	= pStuff->Attribute("GoodsName");

					Expend.lGoodsIndex		= CGoodsFactory::QueryGoodsIDByOriginalName(pGoodsName);
					Expend.lGoodsNum		= atoi(pStuff->Attribute("GoodsNum"));
					if(0 == Expend.lGoodsIndex)
					{
						AddErrorLogText("NPC制作配置错误！没有找到原始名为[%s]的材料，此项材料被忽略！！！", pGoodsName);
						continue;
					}
					if(0 >= Expend.lGoodsNum)
					{
						AddErrorLogText("NPC制作配置错误！原始名为[%s]的材料需要数量小于等于零，此项材料被忽略！！！", pGoodsName);
						continue;
					}

					pOneMakeTable->AddExpend(Expend);
				}

				TiXmlElement* pProduction = pStuffList->FirstChildElement("Production");
				for (; pProduction!=NULL; pProduction = pProduction->NextSiblingElement("Production"))
				{
					tagProduction Production;
					const char *pGoodsName	= pProduction->Attribute("GoodsName");

					Production.lGoodsIndex	= CGoodsFactory::QueryGoodsIDByOriginalName(pGoodsName);
					Production.lGoodsNum	= atoi(pProduction->Attribute("GoodsNum"));
					Production.lOutOdds		= atoi(pProduction->Attribute("Odds"));
					if(0 == Production.lGoodsIndex)
					{
						AddErrorLogText("NPC制作配置错误！没有找到原始名为[%s]的产物，此项产物被忽略！！！", pGoodsName);
							continue;
					}
					if(0 >= Production.lGoodsNum)
					{
						AddErrorLogText("NPC制作配置错误！原始名为[%s]的产物需要数量小于等于零，此项产物被忽略！！！", pGoodsName);
						continue;
					}
					if(0 >= Production.lOutOdds)
					{
						AddErrorLogText("NPC制作配置错误！原始名为[%s]的产物的产生几率小于等于零，此项产物被忽略！！！", pGoodsName);
						continue;
					}

					pOneMakeTable->AddProduction(Production);
				}
			}
			
			if(!GoodsTailorSetup::GetInstance().AddOneMakeTable(pOneMakeTable))
			{
				AddErrorLogText("NPC制作配方错误！配方[%s][%d]的ID重复了！！！", pOneMakeTable->GetName(), pOneMakeTable->GetID());
			}
		}
	}

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
		AddLogText("重读LoginGMList.dat成功!");
	}
	else
	{
		char str[256];
		sprintf(str, "Can't find file %s", filename);
		MessageBox(g_hWnd, str, "message", MB_OK);
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
		 tagGMInfo* pInfo = CGMList::GetInfoByName(pPlayer->GetName());
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
		g_pMailManager->RegisterCheckTime(pPlayer->GetExID(),CGlobeSetup::GetSetup()->lMailDelayTime*1000);
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
		AddLogText( "MapPlayer不存在或为空" );
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

			if(GetDbIsReady())
			{
				GetGame()->DelRawPlayer(pOld);
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
		pClonePlayer = MP_NEW CPlayer;
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
	return res;
}


const CGUID& CGame::GetOnlinePlayerIDByName(LPCSTR szName)
{
	for(map<CGUID, CPlayer*>::iterator itMap=m_mPlayer.begin(); itMap!=m_mPlayer.end(); ++itMap)
	{
		// 相同
		if( !strcmpi(itMap->second->GetName(), szName) )
		{
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(itMap->second->GetAccount());
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
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(it->second->GetAccount());
			if(!tLoginInfo)
				continue;

			if( tLoginInfo->GetPlayerState() == PLAYER_STATE_LOADED_CHAR_DATA )
			{
				// 如果登录玩家超时，将提出他。
				if( now - it->second->GetBeginLoginTime() > GetSetup()->dwReleaseLoginPlayerTime )
				{
					// 通知LoginServer
					CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
					msg.Add(const_cast<char*>(it->second->GetAccount()));
					msg.Add(0L);
					msg.Send();//send to loginserver

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
//////////////////////////////////////////////////////////////////////////
//	存盘 （操作以上三种需要进数据库的数据）
//////////////////////////////////////////////////////////////////////////
void CGame::GenerateDBData()
{

}

void CGame::GeterateRegionDBData()
{

}

//断开所有玩家
void CGame::KickAllPlayer()
{
	CMessage msg( MSG_W2S_OTHER_KICKALL );
	for( map<DWORD, CGame::tagGameServer>::iterator itBegin = s_mapGameServer.begin();
		itBegin != s_mapGameServer.end(); itBegin ++ ) {
		if( itBegin -> second.bConnected )
		{				
			msg.SendGSID( itBegin->first );
		}
	}
}


bool CGame::IsNameExistInDBData(LPCSTR szName)
{
	if(!szName) return true;
	bool ret = false;
	return ret;
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

long CGame::GetConnectedGameServerCount()
{
	long lCnt=0;
	map<DWORD,tagGameServer>::iterator it=s_mapGameServer.begin();
	for(;it!=s_mapGameServer.end();it++)
	{
		if(it->second.bConnected)
		{
			lCnt++;
		}
	}
	return lCnt;
}

VOID CGame::ReturnGsPlayerNum(VOID)
{
	CMessage msgLS(MSG_W2L_SERVER_ANSWER_GsPlayerNum);
	msgLS.Add((LONG)s_mapGameServer.size());

	for(map<DWORD, tagGameServer>::iterator it=s_mapGameServer.begin(); it!=s_mapGameServer.end(); it++)
	{
		tagGameServer *p = &it->second;
		msgLS.Add((BYTE)p->bConnected);
		msgLS.Add(p->dwIndex);
		msgLS.Add(p->lPlayerNum);
	}

	msgLS.SendToSocket(GetGame()->GetLSSocketID());
}

CGame::tagGameServer* CGame::GetGameServer(const char* strIP, DWORD dwPort)
{
	for(map<DWORD, tagGameServer>::iterator it=s_mapGameServer.begin(); it!=s_mapGameServer.end(); it++)
	{
		tagGameServer *p = &it->second;
		if (p->strIP==strIP && p->dwPort==dwPort)
		{
			return p;
		}
	}
	return NULL;
}

bool CGame::OnGameServerClose(DWORD dwIndex)
{
	tagGameServer* pGameServer = GetGameServer(dwIndex);
	if (pGameServer)
	{
		pGameServer->bConnected = false;
		// 删除Player列表
		char str[256];
		sprintf(str, "%s [%d]", pGameServer->strIP.c_str(), pGameServer->dwPort);
		long lPos = (long)SendMessage(g_hGameServerListBox, LB_FINDSTRING, 0, (LPARAM)str);
		if( lPos != LB_ERR )
		{
			SendMessage(g_hGameServerListBox,LB_DELETESTRING, lPos, 0);
		}

		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_GSLOST), pGameServer->strIP.c_str(), pGameServer->dwPort);
	}
	else
	{
		AddLogText(CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_UNKOWNGSLOST), dwIndex);
		return false;
	}

	// 清除在该GameServer上的player和场景，并通知LoginSever
	OnGameServerLost(dwIndex);
	
	return true;
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
		
		
		if(GetGame()->GetDbIsReady())
		{
			// 发送DBS存盘消息
			CWorldServerSession session(5000);

			// 设置当前账号状态
			Account* pAcc = NULL;
			CEntityGroup* tDBLoginPlayer = NULL;
			if(pPlayer->GetAccount())
				pAcc = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());
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
				tDBLoginPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[loginplayer]"), pPlayer->GetExID());
				session.NakeSendSaveLoginPlayerDataMsg(pPlayer, tDBLoginPlayer);
				GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBLoginPlayer);
			}

			CEntityGroup* tDBPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[player]"), pPlayer->GetExID());
			session.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, pPlayer, tDBPlayer, true);
			GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBPlayer);			
		}
		msg.Add(const_cast<char*>(pPlayer->GetAccount()));

		OnPlayerQuit(pPlayer,dwIndex, 0L);
	}	
	msg.Send();//send the cdkeys to loginserver

	return true;
}


void CGame::SendMsg2GameServer(DWORD dwIndex,CMessage* pMsg)
{
	if(pMsg)
	{
		pMsg->SendGSID(dwIndex);
	}
}

//! [2007-7-9 add]
void CGame::SendAllGameServer(CMessage* pMsg)
{
	if(pMsg)
	{
		map<DWORD, tagGameServer>::iterator iteGS = s_mapGameServer.begin();
		for (; iteGS != s_mapGameServer.end(); ++iteGS)
		{
			long dwIndex = iteGS->first;
			pMsg->SendGSID(dwIndex);
		}
	}
}

CGame::tagGameServer* CGame::GetRegionGameServer(long lRegionID)
{
	CGlobalRgnManager::tagRegion* pRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
	if (pRegion==NULL)
	{
			return NULL;
	}

	return GetGameServer(pRegion->dwGameServerIndex);
}

// 获取region所在的GameServer
CGame::tagGameServer* CGame::GetRegionGameServer(const CGUID& RegionID)
{
	long lGSID = GetGame()->GetGlobalRgnManager()->FindGSIDByRgnID(RegionID);
	return GetGameServer(lGSID);
}

// 获取player所在的GameServer
CGame::tagGameServer* CGame::GetPlayerGameServer(const CGUID& PlayerGuid)
{
	CPlayer* pPlayer = GetMapPlayer(PlayerGuid);
	if (pPlayer==NULL)
		return NULL;

	return GetRegionGameServer(pPlayer->GetRegionExID());
}

long CGame::GetGameServerNumber_ByRegionID(long lRegionID)
{
	tagGameServer* pGameServer = GetRegionGameServer(lRegionID);
	if (pGameServer==NULL)
		return 0;

	return pGameServer->dwIndex;
}

long CGame::GetGameServerNumber_ByRegionGUID(const CGUID& RegionGuid)
{
	tagGameServer* pGameServer = GetRegionGameServer(RegionGuid);
	if (pGameServer==NULL)
		return 0;

	return pGameServer->dwIndex;
}

long CGame::GetGameServerNumber_ByPlayerID(const CGUID& PlayerGuid)
{
	tagGameServer* pGameServer = GetPlayerGameServer(PlayerGuid);
	if (pGameServer==NULL)
		return 0;

	return pGameServer->dwIndex;
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
		long lGameServerNum = GetGame()->GetGameServerNumber_ByRegionGUID(pPlayer->GetRegionExID());
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
			_snprintf(pszLogingInfo,1024,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_LOGINGSINVALID), szGuid);
			PutStringToFile("Login_WS_Info",pszLogingInfo);
			return false;
		}
	}

	//! 绑定socketID
	pPlayer->SetGsSocket(lMsgSocketID);
	//! 绑定IP
	pPlayer->SetIP(lIP);

	//! 玩家数据发送前必须处理的步骤
	if (pAcc->GetPlayerState() != PLAYER_STATE_CHANGING_SERVER)
	{
		//! 记录日志
		GetGameLogInterface()->logT550_client_in_game_log(pPlayer, 1);
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

		LinkmanSystem::GetInstance().OnPlayerOnline(pPlayer->GetExID());
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
			GetGame()->SendAllGameServer(&teamMsg);
		}
	}
	return true;
}
bool	CGame::OnPlayerQuit(CPlayer* pPlayer, long gsid, DWORD dwServerKey[], bool bIsChangingGs)
{
	if(!pPlayer) return false;
	//! 记录日志
	GetGameLogInterface()->logT550_client_in_game_log(pPlayer, 2);

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
	msg.SendToSocket(GetGame()->GetLSSocketID());//send to loginserver
	
	GetGame()->DeleteMapPlayer(pPlayer->GetExID());

	// gs玩家数减一
	CGame::tagGameServer* pGameServer = GetGame()->GetGameServer(gsid);
	if (pGameServer && pGameServer->bConnected)
	{
		pGameServer->lPlayerNum--;
	}
	return true;
}
// 从player身上获取装备信息
void CGame::GetPlayerEquipID(CPlayer* pPlayer,
							 DWORD& dwHelm, DWORD& dwBody, DWORD& dwGlov, DWORD& dwBoot, DWORD& dwWeapon, DWORD& dwBack,
							 DWORD& dwHeadgear, DWORD& dwFrock, DWORD& dwWing, DWORD& dwManteau, DWORD& dwFairy,
							 BYTE& nHelmLevel, BYTE& nBodyLevel, BYTE& nGlovLevel, BYTE& nBootLevel, BYTE& nWeaponLevel, BYTE& nBackLevel,
							 BYTE& nHeadgearLevel, BYTE& nFrockLevel, BYTE& nWingLevel, BYTE& nManteauLevel, BYTE& nFairyLevel)
{
	// 头盔
	CGoods* pGoods = pPlayer -> m_cEquipment.GetGoods(DWORD(CEquipmentContainer::EC_HEAD ) );
	if( pGoods )
	{
		dwHelm		= pGoods -> GetBasePropertiesIndex();		
	}
	else
	{
		dwHelm		= 0;
		nHelmLevel	= 0;
	}

	// 盔甲
	pGoods= pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_BODY );
	if( pGoods )
	{
		dwBody		= pGoods -> GetBasePropertiesIndex();	
	}
	else
	{
		dwBody		= 0;
		nBodyLevel	= 0;
	}

	// 护腕
	
	pGoods = pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_GLOVE );
	if( pGoods )
	{
		dwGlov		= pGoods -> GetBasePropertiesIndex();
		
	}
	else
	{
		dwGlov		= 0;
		nGlovLevel	= 0;
	}

	// 靴子	
	pGoods = pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_BOOT );
	if( pGoods )
	{
		dwBoot		= pGoods -> GetBasePropertiesIndex();
	}
	else
	{
		dwBoot		= 0;
		nBootLevel	= 0;
	}

	// 武器
	pGoods = pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_WEAPON );
	if( pGoods )
	{
		dwWeapon	= pGoods -> GetBasePropertiesIndex();
	}
	else
	{
		dwWeapon	= 0;
		nWeaponLevel= 0;
	}

	// 头饰
	pGoods = pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_HEADGEAR );
	if( pGoods )
	{
		dwHeadgear		= pGoods -> GetBasePropertiesIndex();
	}
	else
	{
		dwHeadgear		= 0;
		nHeadgearLevel	= 0;
	}

	// 外衣
	pGoods = pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_FROCK );
	if( pGoods )
	{
		dwFrock		= pGoods -> GetBasePropertiesIndex();
	}
	else
	{
		dwFrock		= 0;
		nFrockLevel	= 0;
	}

	// 翅膀
	pGoods = pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_WING );
	if( pGoods )
	{
		dwWing		= pGoods -> GetBasePropertiesIndex();
	}
	else
	{
		dwWing		= 0;
		nWingLevel	= 0;
	}


	// 精灵
	pGoods = pPlayer -> m_cEquipment.GetGoods((DWORD) CEquipmentContainer::EC_FAIRY );
	if( pGoods )
	{
		dwFairy		= pGoods -> GetBasePropertiesIndex();
	}
	else
	{
		dwFairy		= 0;
		nFairyLevel	= 0;
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

	for (list<CPlayerList::tagOrginEquip>::iterator it=CPlayerList::GetOrginEquipList()->begin(); it!=CPlayerList::GetOrginEquipList()->end(); it++)
	{
		CPlayerList::tagOrginEquip *pst = &(*it);
		if (pPlayer->GetOccupation() == pst->lOccupation )
		{

#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(31);
#endif
			CGoods* pGoods = CGoodsFactory::CreateGoods( CGoodsFactory::QueryGoodsIDByOriginalName( pst->strOrginName.c_str() ),25 );
			if (pGoods)
			{
				CGUID goodsid;
				CGUID::CreateGUID(goodsid);
				pGoods->SetExID(goodsid);

				if( pPlayer -> m_cEquipment.Add( static_cast<CEquipmentContainer::EQUIPMENT_COLUMN>
					(pst -> wPlacePos), pGoods ) == FALSE)
				{
					assert(0);
					OutputDebugString( "Failed to add equipment" );
				}
			}
		}
	}
}
void CGame::DoSaveData(bool isSavePlayerFlag)
{
	// 将CDBEntity对象发送到DBS
	AddLogText("定时保存数据开始!");
	CGUID pGUID;
	long playersize = 0;
	
	if(isSavePlayerFlag)
	{
		map<CGUID, CPlayer*>::iterator enItr = m_mPlayer.begin();
		for(;enItr != m_mPlayer.end();++enItr)
		{
			// 发送个人房屋数据
			CPlayer* pPlayer = enItr->second;
			if(pPlayer)
			{
				// 发送账号数据
				Account* pAcc = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());
				GetGame()->GetEntityManager()->CreateSaveAccountSession(pAcc);
				// 发送个人数据
				GetGame()->GetEntityManager()->CreateSavePlayerDataSession(enItr->first, SAVE_DETAIL_ALL);
				playersize++;
			}
		}
		AddLogText("定时保存[%d]个玩家数据!", playersize);
	}

	//! 保存工会数据
	GetOrganiCtrl()->SaveAllFaction();
	//! 保存联盟数据
	GetOrganiCtrl()->SaveAllUnion();

	// 保存全局变量数据
	if(GetGame()->GetInitVarLoadFlag())
	{
		CEntityManager::DBEntityComponentMapItr varMapItr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(string("[scriptvargroup]"));
		if(varMapItr == GetGame()->GetEntityManager()->GetBaseEntityMap().end())
		{
			AddLogText("MSG_S2W_OTHER_UPDATEGLOBALSCTIPTVARS 未找到VarGroup对象,请检查配置文件!");
			return;
		}
		if(varMapItr->second.size() == 0)
		{
			AddLogText("MSG_S2W_OTHER_UPDATEGLOBALSCTIPTVARS 未找到VarGroup对象,请检查对象初始化数据!");
			return;
		}
		map<CGUID, CEntityGroup*>::iterator enItr = varMapItr->second.begin();
		if(enItr == varMapItr->second.end())
		{
			AddLogText("MSG_S2W_OTHER_UPDATEGLOBALSCTIPTVARS 未找到VarGroup对象,请检查对象初始化数据!");
			return;
		}
		CEntityGroup* pVarGroup = (CEntityGroup*)enItr->second;
		if(pVarGroup)
		{
			pVarGroup->ReleaseChilds();
			GetGame()->GetEntityManager()->CreateSaveGlobalVarSession();
			AddLogText("定时保存[%d]个全局变量数据!", pVarGroup->GetEntityGroupMap().size());
		}
	}

	AddLogText("定时保存数据结束!");

	CMessage pEndMsg(MSG_W2DB_SERVER_SAVEDATAEND);
	pEndMsg.Add((BYTE)0); // 定时保存标志
	pEndMsg.SendToSocket(GetGame()->GetDBSocketID());
}


// 游戏主线程
unsigned __stdcall GameThreadFunc(void* pArguments)
{
#ifndef _DEBUG
	__try
	{
#endif
		CreateGame();
		BOOL ret = GetGame()->Init();

		if (ret)
		{
			while(1)
			{
				// 结束线程
				if( g_bGameThreadExit )
				{
					break;
				}

				if( !GetGame()->MainLoop() )
				{
					break;
				}
			}

		}

		if(GetGame())
			GetGame()->Release();
		DeleteGame();

#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode(),"Main"))
	{
		PutDebugString("程序出现错误，请查看最新的错误报告文件");
		AddErrorLogText("程序出现错误，请查看最新的错误报告文件");

		if(GetGame())
			GetGame()->Release();
		DeleteGame();

		Sleep(5000);

		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
		SetEvent(g_hGameThreadExitEvent);
		_endthreadex( 0 );

		return 0;
	}
#endif

	SetEvent(g_hGameThreadExitEvent);
	PostMessage(g_hWnd, WM_CLOSE, 1,0);

	_endthreadex( 0 );
	return 0;
}

// 连接WorldServer的线程、当WorldServer断开时启用，连同时候关闭
unsigned __stdcall	 ConnectLoginServerFunc( void* pArguments )
{
	while(true)
	{
		if(bLSConnectThreadExit)
			break;
		Sleep(8000);

		if(GetGame()->ReConnectLoginServer()==true)
		{
			break;
		}

	}
	SetEvent(hLSConnectThread);
	return 0;
}

//CostServer断开时启用此线程
unsigned __stdcall	 ConnectCostServerFunc( void* pArguments )
{
	while(true)
	{
		if(bConnectThreadExit1)
			break;
		Sleep(8000);

		if(GetGame()->ReConnectCostServer()==true)
		{
			break;
		}

	}
	_endthreadex( 0 );
	return 0;
}


// 连接DBServer的线程、当WorldServer断开时启用，连同时候关闭
unsigned __stdcall	 ConnectDBServerFunc( void* pArguments )
{
	while(true)
	{
		if(bDBSConnectThreadExit)
			break;
		Sleep(8000);

		if(GetGame()->ReConnectDBServer()==TRUE)
		{
			break;
		}

	}
	SetEvent(hDBSConnectThread);
	return 0;
}

unsigned __stdcall ConnectLogServerFunc( void* pArguments )
{
	while(true)
	{
		if(GetGame()->GetLogClient().bConnectThreadExit)
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

bool CGame::ValidateCreatioPlayerGuidinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid)
{

	return false;
}

bool CGame::ValidatePlayerIDinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid)
{
	return false;
}

void DoSaveLog()
{
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
		msg.SendToSocket(GetGame()->GetLSSocketID());
		return;
	}

	CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);

	Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(szCdkey);
	if(!tLoginInfo)
	{
		GetGame()->DeleteMapPlayer(pPlayer->GetExID());
		// 添加验证代码
		msg.Add((long)0xFFFFFFFF);
		msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
		msg.Add(szCdkey);
		msg.SendToSocket(GetGame()->GetLSSocketID());
		return;

	}
	if(tLoginInfo)
	{
		// 创建验证码
		long lSignCode = 0;
		GetGame()->GenerateSignCodeByTime(lSignCode);
		tLoginInfo->SetSignCode(lSignCode);
		// 添加验证代码
		msg.Add((long)lSignCode);
	}

	long lRegion = 0;
	long l=pPlayer->GetPosX();
	long t=pPlayer->GetPosY();
	long r=0;
	long b=0;
	long d=0;
	CGlobalRgnManager::tagRegion* pstRegion = NULL;

	long lRegionID = pPlayer->GetRegionID();
	CWorldRegion* pRegion = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(pPlayer->GetRegionExID());
	if(pRegion && pRegion->GetID() != lRegionID)
	{
		char szGuid[128];
		pPlayer->GetRegionExID().tostring(szGuid);
		AddLogText("InitPlayerInfo() 玩家[%s]的场景GUID[%s]和lID[%d]不吻合!", szCdkey, szGuid, lRegionID);
		pPlayer->SetRegionID(pRegion->GetID());
		pPlayer->SetRegionExID(pRegion->GetExID());
	}
	
	if(!pRegion)// 判断是否被关掉 或 副本被删掉
	{
		// 先检查TemplateID，判断是否为副本
		pstRegion = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalRegion(lRegionID);
		if(!pstRegion)
			pstRegion = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(lRegionID);
		if(!pstRegion)
			pstRegion = GetGame()->GetGlobalRgnManager()->GetTemplateTeamRegion(lRegionID);
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
					AddLogText("玩家%s登录时返回指定场景%d为空！",szCdkey, dupRgnID);
				}
			}
			else if(dupRgnID == -1) // 回国都
			{
				long lCityRegion = GetCountryParam()->m_mpMainRegions[pPlayer->GetCountry()];
				CGlobalRgnManager::tagRegion* pstCityRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lCityRegion);
				if(!pstCityRegion || !pstCityRegion->pRegion)
				{
					AddLogText("玩家%s登录时返回国都场景%d为空！",szCdkey, lCityRegion);

					msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
					msg.Add(szCdkey);
					msg.SendToSocket(GetGame()->GetLSSocketID());
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
				char pszLogingInfo[512];
				_snprintf(pszLogingInfo,512, "InitPlayerInfo() pPlayer[%s]副本[%d]返回地图ID为0!", szCdkey, lRegionID);
				PutStringToFile("Login_WS_Info",pszLogingInfo);

				GetGame()->DeleteMapPlayer(pPlayer->GetExID());

				msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());
				tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
				return;
			}
		}
		else
		{
			// 再判断是否为普通场景
			pstRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
			if(!pstRegion) // 普通场景或副本场景被关掉
			{
				map<long, CRegionPlugSetup::tagRegionPlugSetup>::iterator rgnItr = CRegionPlugSetup::GetRegionPlugSetupMap().find(lRegionID);
				if(rgnItr != CRegionPlugSetup::GetRegionPlugSetupMap().end())
				{
					if(rgnItr->second.bType == 0) // 回国都传送
					{
						lRegion = GetCountryParam()->m_mpMainRegions[pPlayer->GetCountry()];
						CGlobalRgnManager::tagRegion* pstPlugRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegion);
						if(!pstPlugRegion || !pstPlugRegion->pRegion)
						{
							AddLogText("玩家%s登录时普通场景返回国都场景%d为空！",szCdkey, lRegion);

							msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
							msg.Add(szCdkey);
							msg.SendToSocket(GetGame()->GetLSSocketID());
							tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
							return;
						}
						pRegion = pstPlugRegion->pRegion;
						pstRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
						l = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].left;
						t = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].top;
						r = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].right;
						b = GetCountryParam()->m_mpMainRect[pPlayer->GetCountry()].bottom;
					}
					else // 指定地图传送
					{
						pstRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(rgnItr->second.lRegionID);
						if(pstRegion)
							pRegion = pstRegion->pRegion;
						else
						{
							AddLogText("玩家%s登录时普通场景指定地图返回场景%d为空！",szCdkey, rgnItr->second.lRegionID);
						}
					}

					if(!pRegion)
					{
						msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
						msg.Add(szCdkey);
						msg.SendToSocket(GetGame()->GetLSSocketID());
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
		PutStringToFile("Login_WS_Info",pszLogingInfo);

		GetGame()->DeleteMapPlayer(pPlayer->GetExID());

		msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
		msg.Add(szCdkey);
		msg.SendToSocket(GetGame()->GetLSSocketID());
		return;
	}

	pPlayer->SetPosXY(l, t);
	pPlayer->SetRegionID(pRegion->GetID());
	pPlayer->SetRegionExID(pRegion->GetExID());
	pPlayer->SetCurRgnType((eRgnType)pRegion->GetRgnType());
	
	pPlayer->SetPersonalRgnGUID(GetGame()->GetGlobalRgnManager()->GetPersonalRgnGUID(pPlayer->GetExID()));
	if(pPlayer->GetPersonalRgnGUID() != NULL_GUID)
		pPlayer->SetCurPRgnNum(1);
	
	// GameServer没有连接
	CGame::tagGameServer* pGameServer = GetGame()->GetGameServer(pRegion->GetGsid());
	if (!pGameServer || !pGameServer->bConnected)
	{
		char pszLogingInfo[512];
		_snprintf(pszLogingInfo,512, CStringReading::LoadString(IDS_WS_GAME, STR_WS_GAME_QUESTPROPERTYBYGSFAIL), pRegion->GetGsid());
		PutStringToFile("Login_WS_Info",pszLogingInfo);

		GetGame()->DeleteMapPlayer(pPlayer->GetExID());

		msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
		msg.Add(szCdkey);
		msg.SendToSocket(GetGame()->GetLSSocketID());
		tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
		return;
	}
	else
	{
		// 玩家数加一
		pGameServer->lPlayerNum++;
	}

	//	取得gameserver的ip和port
	string strGsIp;
	long lGsPort;
	strGsIp=pGameServer->strIP;
	lGsPort=pGameServer->dwPort;

	if(tLoginInfo)
		tLoginInfo->SetCurGamedPlayerGuid(pPlayer->GetExID());

	// 上线成功
	msg.Add((char)LOGIN_QUEST_PLAYERDATA_SUCCEED);
	msg.Add(szCdkey);
	msg.Add(const_cast<char*>(strGsIp.c_str()));
	msg.Add(lGsPort);
	msg.Add(const_cast<char*>(pPlayer->GetName ()));	//发送角色名，日志需要
	msg.Add(pPlayer->GetLevel());						//发送角色等级，日志需要
	msg.SendToSocket(GetGame()->GetLSSocketID());
	pPlayer->SetPlayerLogin(true);
	pPlayer->SetFirstLogin(true);
	pPlayer->SetBeginLoginTime(timeGetTime());
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
	GetGame()->GetTimerQueue()->Schedule(msgTime, NULL, curTime);
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
	std::list<long>::iterator itr = std::find(GetGame()->GetFreeMsgIDList().begin(), GetGame()->GetFreeMsgIDList().end(), id);
	// 未找到该ID并且ID小于最大值
	if( (itr == GetGame()->GetFreeMsgIDList().end()) && (id < MAX_MSG_TIME_NUM) ) 
		GetGame()->GetFreeMsgIDList().push_back(id);
}
// 取得MsgTimer
CMsgTimer* CGame::GetMsgTimer(long id)
{
	CMsgTimer* ret = NULL;
	mapMsgTimerItr itr = m_mapMsgTimer.find(id);
	if(itr != m_mapMsgTimer.end())
		ret = itr->second;

	return ret;
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
			GetGame()->SendAllGameServer(&msg);
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


//////////////////////////////////////////////////////////////////////////
void OnLogServerMessage(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_LOG20_SERVER_TABLE://表格结构信息
		{
			GetGame()->InitLogClient(pMsg);
		}
		break;
	case MSG_LOG20_SERVER_AFFIRM://确认服务器在LogServer上有配置的数据库
		{
			GetGame()->LogServerAffirm(pMsg);
		}
		break;
	case MSG_LOG20_SERVER_CONDITION://消息的发送控制信息
		{
			GetGame()->LogServerCondition(pMsg);
		}
		break;

	}
}

void	CGame::InitLogClient(CMessage *pMsg)
{
	LogClientInfo tmpLogClientInfo;
	tmpLogClientInfo.ErrLogFileName = GetSetup()->strErrFile;
	tmpLogClientInfo.LogFileName	= GetSetup()->strLogFile;
	tmpLogClientInfo.TxtLogFileName = GetSetup()->strTxtLogFile;
	tmpLogClientInfo.lClientNo		= GetSetup()->dClientNo;

	bool re = m_LogClient.Init(pMsg, tmpLogClientInfo);
	if(re)
	{
		if (NULL != m_pLogicLogInterface)
		{
			SAFE_DELETE(m_pLogicLogInterface);
		}
		m_pLogicLogInterface = new LogicLogInterface(&m_LogClient);
	}
}

void	CGame::LogServerAffirm(CMessage *pMsg)
{
	if(0 == pMsg->GetLong())
		m_LogClient.PassAffirm(false);
	else
		m_LogClient.PassAffirm(true);
}

void	CGame::LogServerCondition(CMessage *pMsg)
{
	m_LogClient.UpdateCondition(pMsg);
}

LogicLogInterface*	CGame::GetLogicLogInterface(void)
{
	assert(NULL != m_pLogicLogInterface); 
	return m_pLogicLogInterface;
}

//!			序列化物品扩展配置
bool		CGame::SerializeGoodsExSetup(vector<BYTE> &vecData)
{
	return m_pGoodsExSetup->Serialize(&vecData);
}



//! GoodsExSetup服务器相关方法实现
//! ----------------------------------------------------------------------------------------
#ifdef USE_IN_SERVER 

//! 初始化服务器配置
//! 功能：将配置中的物品原始名对应的物品索引找出来添加到配置中
bool GoodsExSetup::InitServerSetup(void)
{
	bool bLocalResult = true;
	bool bReValue = true;
	//! 分解物品配置
	map<LONG, tagDisassemblePrescription>::iterator iteDP = m_mapDisassemblePrescription.begin(); 
	for (; iteDP != m_mapDisassemblePrescription.end(); ++iteDP)
	{
		for (long idx = 0; idx < MAX_DISASSEMBLE_NUM; ++idx)
		{
			if (0 == iteDP->second.arr_szOutGoodsName[idx][0]) break;
			iteDP->second.arr_lOutGoodsId[idx] = CGoodsFactory::QueryGoodsIDByOriginalName(iteDP->second.arr_szOutGoodsName[idx]);
			if (0 >= iteDP->second.arr_lOutGoodsId[idx])
			{
				AddLogText("%s%s%s", 
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_10),
					iteDP->second.arr_szOutGoodsName[idx],
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_20)
					);
				bLocalResult = false;
				continue;
			}
			vector<CGoodsBaseProperties::tagAddonPropertyValue> vOut1;
			CGoodsFactory::QueryGoodsBaseProperties(iteDP->second.arr_lOutGoodsId[idx])->GetAddonPropertyValues(GAP_QUALITY, vOut1);
			vector<CGoodsBaseProperties::tagAddonPropertyValue> vOut2;
			CGoodsFactory::QueryGoodsBaseProperties(iteDP->second.arr_lOutGoodsId[idx])->GetAddonPropertyValues(GAP_MAXOVERLAP, vOut2);
			if(0 < vOut1.size() && 0 < vOut2.size())
			{
				AddLogText("%s%s%s", 
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_30),
					iteDP->second.arr_szOutGoodsName[idx],
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_40)
					);
				bLocalResult = false;
				continue;
			}
		}
	}

	if (bLocalResult)
	{
		AddLogText(CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_50));
	}
	else
	{
		bReValue = false;
	}

	//! 合成物品配置
	bLocalResult = true;
	map<long, tagSynthesizePrescription>::iterator ite = m_mapSynthesizePrescription.begin();
	for ( ; ite != m_mapSynthesizePrescription.end(); ++ite)
	{
		ite->second.lOutGoodsId = CGoodsFactory::QueryGoodsIDByOriginalName(ite->second.szOutGoodsName);
		if (0 >= ite->second.lOutGoodsId)
		{
			AddLogText("%s%s%s", 
				CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_60),
				ite->second.szOutGoodsName,
				CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_20)
				);
			bLocalResult = false;
			continue;
		}

		for (long i = 0; i < MAX_STUFF_NUM; ++i)
		{
			if(0 == ite->second.sz_arr_StuffName[i][0]) break;
			ite->second.arr_lStuffId[i] = CGoodsFactory::QueryGoodsIDByOriginalName(ite->second.sz_arr_StuffName[i]);
			if (0 >= ite->second.arr_lStuffId[i])
			{
				AddLogText("%s%s%s",
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_60),
					ite->second.sz_arr_StuffName[i],
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_20)
					);
				bLocalResult = false;
				continue;
			}
		}
	}

	if (bLocalResult)
	{
		AddLogText(CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_80));
	}
	else
	{
		bReValue = false;
	}


	//! 装备升级配置
	bLocalResult = true;
	for (long i = 0; i < m_vecUpgradeSetList.size(); ++i)
	{
		map<long, tagUpgradeSet>::iterator ite = m_vecUpgradeSetList[i].mapUpgradeSet.begin();
		for (; ite != m_vecUpgradeSetList[i].mapUpgradeSet.end(); ++ite)
		{
			for (list<string>::iterator itrStuff = ite->second.listStuffName.begin(); ite->second.listStuffName.end() != itrStuff; ++itrStuff)
			{
				long StuffIndex = CGoodsFactory::QueryGoodsIDByOriginalName((*itrStuff).c_str());
				if(0 == StuffIndex) 
				{
					AddLogText("%s%s%s",
						CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_90),
						(*itrStuff).c_str(),
						CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_100)
						);
					bLocalResult = false;
				}
				else
				{
					ite->second.listStuffIndex.push_back(StuffIndex);
				}
			}
		}
	}

	if (bLocalResult)
	{
		AddLogText(CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_110));
	}
	else
	{
		bReValue = false;
	}

	//! 特殊物品配置
	bLocalResult = true;
	for (long i = 0; i < m_vecExtraStuff.size(); ++i)
	{
		for (list<string>::iterator itrStuff = m_vecExtraStuff[i].listStuffName.begin(); m_vecExtraStuff[i].listStuffName.end() != itrStuff; ++itrStuff)
		{
			long StuffIndex = CGoodsFactory::QueryGoodsIDByOriginalName((*itrStuff).c_str());
			if(0 == StuffIndex) 
			{
				AddLogText("%s%s%s",
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_120),
					(*itrStuff).c_str(),
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_100)
					);
				bLocalResult = false;
			}
			else
			{
				m_vecExtraStuff[i].listStuffIndex.push_back(StuffIndex);
			}
		}
	}

	if (bLocalResult)
	{
		AddLogText(CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_130));
	}
	else
	{
		bReValue = false;
	}


	//! 魔化配置
	bLocalResult = true;

	for (long i = 0; i < m_ElementSetup.vElementBound.size(); ++i)
	{
		for (long n = 0; n < m_ElementSetup.vElementBound[0].vElementStuff.size(); ++n)
		{
		for (long j = 0; j < MT_Max; ++j)
		{
				long lStuffIndex = CGoodsFactory::QueryGoodsIDByOriginalName(m_ElementSetup.vElementBound[i].vElementStuff[n].szStuffNameA[j]);
			if(0 == lStuffIndex) 
			{
				AddLogText("%s%s%s",
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_140),
						m_ElementSetup.vElementBound[i].vElementStuff[n].szStuffNameA[j],
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_150)
					);
				bLocalResult = false;
			}
		}
		for (long j = 0; j < MT_Max; ++j)
		{
				long lStuffIndex = CGoodsFactory::QueryGoodsIDByOriginalName(m_ElementSetup.vElementBound[i].vElementStuff[n].szStuffNameD[j]);
			if(0 == lStuffIndex) 
			{
				AddLogText("%s%s%s",
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_140),
						m_ElementSetup.vElementBound[i].vElementStuff[n].szStuffNameD[j],
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_150)
					);
				bLocalResult = false;
			}
		}
	}
	}

	for (long i = 0; i < m_ElementSetup.vHelpfulSetup.size(); ++i)
	{
		for (long j = 0; j < m_ElementSetup.vHelpfulSetup[i].vHelpfulStuff.size(); ++j)
		{
			long lStuffIndex = CGoodsFactory::QueryGoodsIDByOriginalName(m_ElementSetup.vHelpfulSetup[i].vHelpfulStuff[j].szStuffName);
			if(0 == lStuffIndex) 
			{
				AddLogText("%s%s%s",
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_140),
					m_ElementSetup.vHelpfulSetup[i].vHelpfulStuff[j].szStuffName,
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_150)
					);
				bLocalResult = false;
			}
		}
	}

	for (long i = 0; i < m_ElementSetup.vResetElement.size(); ++i)
	{
		for (long j = 0; j < m_ElementSetup.vResetElement[i].vResetStuff.size(); ++j)
		{
			long lStuffIndex = CGoodsFactory::QueryGoodsIDByOriginalName(m_ElementSetup.vResetElement[i].vResetStuff[j].szStuffName);
			if(0 == lStuffIndex) 
			{
				AddLogText("%s%s%s",
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_140),
					m_ElementSetup.vResetElement[i].vResetStuff[j].szStuffName,
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_150)
					);
				bLocalResult = false;
			}
		}
	}

	for (long i = 0; i < m_ElementSetup.vResetXiangXing.size(); ++i)
	{
		for (long j = 0; j < m_ElementSetup.vResetXiangXing[i].vResetStuff.size(); ++j)
		{
			long lStuffIndex = CGoodsFactory::QueryGoodsIDByOriginalName(m_ElementSetup.vResetXiangXing[i].vResetStuff[j].szStuffName);
			if(0 == lStuffIndex) 
			{
				AddLogText("%s%s%s",
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_140),
					m_ElementSetup.vResetXiangXing[i].vResetStuff[j].szStuffName,
					CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_150)
					);
				bLocalResult = false;
			}
		}
	}

	if (bLocalResult)
	{
		AddLogText(CStringReading::LoadString(GOODSEX_STRING_TYPEID, WS_GoodsEx_190));
	}
	else
	{
		bReValue = false;
	}

	return bReValue;
}

//! GoodsExSetup的编码定义，对应编码接口（Unserialize）位置：GS GoodsExManage_base.cpp
bool GoodsExSetup::Serialize(vector<BYTE>* pStream)
{
	//! 分解物品配置
	_AddToByteArray(pStream, (long)m_mapDisassemblePrescription.size());

	map<LONG, tagDisassemblePrescription>::iterator iteDP = m_mapDisassemblePrescription.begin(); 
	for (; iteDP != m_mapDisassemblePrescription.end(); ++iteDP)
	{
		_AddToByteArray(pStream, &(iteDP->second), sizeof(tagDisassemblePrescription));
	}

	//! 合成物品配置
	_AddToByteArray(pStream, (long)m_mapSynthesizePrescription.size());
	map<long, tagSynthesizePrescription>::iterator ite = m_mapSynthesizePrescription.begin();
	for ( ; ite != m_mapSynthesizePrescription.end(); ++ite)
	{
		_AddToByteArray(pStream, &(ite->second), sizeof(tagSynthesizePrescription));
	}

	//! 武器充能
	_AddToByteArray(pStream, m_lReinforceMaxPercent);

	_AddToByteArray(pStream, (long)m_vecPowerIni.size());
	for (long i = 0; i < m_vecPowerIni.size(); ++i)
	{
		_AddToByteArray(pStream, m_vecPowerIni[i].lMinLevel);
		_AddToByteArray(pStream, m_vecPowerIni[i].lMaxLevel);
		_AddToByteArray(pStream, (long)m_vecPowerIni[i].mapPowerAccessorial.size());
		map<long, tagPowerAccessorial>::iterator ite = m_vecPowerIni[i].mapPowerAccessorial.begin();
		for ( ; ite != m_vecPowerIni[i].mapPowerAccessorial.end(); ++ite)
		{
			_AddToByteArray(pStream, &(ite->second), sizeof(tagPowerAccessorial));
		}
	}

	//! 绑定物品索引列表
	_AddToByteArray(pStream, (long)m_setBindGoodsIndex.size());
	for (set<long>::iterator ite = m_setBindGoodsIndex.begin(); ite != m_setBindGoodsIndex.end(); ++ite)
	{
		_AddToByteArray(pStream, *ite);
	}
	//! 解绑物品索引列表
	_AddToByteArray(pStream, (long)m_setUntieGoodsIndex.size());
	for (set<long>::iterator ite = m_setUntieGoodsIndex.begin(); ite != m_setUntieGoodsIndex.end(); ++ite)
	{
		_AddToByteArray(pStream, *ite);
	}
	//! 冻结物品索引列表
	_AddToByteArray(pStream, (long)m_setFrostGoodsIndex.size());
	for (set<long>::iterator ite = m_setFrostGoodsIndex.begin(); ite != m_setFrostGoodsIndex.end(); ++ite)
	{
		_AddToByteArray(pStream, *ite);
	}
	//! 解冻物品索引列表
	_AddToByteArray(pStream, (long)m_setThawGoodsIndex.size());
	for (set<long>::iterator ite = m_setThawGoodsIndex.begin(); ite != m_setThawGoodsIndex.end(); ++ite)
	{
		_AddToByteArray(pStream, *ite);
	}

	//! 装备升级配置
	_AddToByteArray(pStream, (long)m_vecUpgradeSetList.size());
	for (long i = 0; i < m_vecUpgradeSetList.size(); ++i)
	{
		_AddToByteArray(pStream, m_vecUpgradeSetList[i].lMinLimitLevel);
		_AddToByteArray(pStream, m_vecUpgradeSetList[i].lMaxLimitLevel);
		_AddToByteArray(pStream, (long)m_vecUpgradeSetList[i].mapUpgradeSet.size());
		map<long, tagUpgradeSet>::iterator ite = m_vecUpgradeSetList[i].mapUpgradeSet.begin();
		for ( ; ite != m_vecUpgradeSetList[i].mapUpgradeSet.end(); ++ite)
		{
			_AddToByteArray(pStream, ite->second.lOdds);			//! 成功几率
			_AddToByteArray(pStream, ite->second.lFailedOperate);	//! 失败时的处理办法（枚举值）
			_AddToByteArray(pStream, ite->second.lOperateNum);		//! 失败时的操作数
			_AddToByteArray(pStream, ite->second.ArmLevel);			//! 适用的武器等级

			//! 生机材料的物品索引
			long lStuffNum = ite->second.listStuffIndex.size();
			_AddToByteArray(pStream, lStuffNum);
			for (list<long>::iterator itrStuff = ite->second.listStuffIndex.begin() ; ite->second.listStuffIndex.end() != itrStuff; ++itrStuff)
				_AddToByteArray(pStream, *itrStuff);
		}
	}

	//! 特殊物品配置
	_AddToByteArray(pStream, (long)m_vecExtraStuff.size());
	for (long i = 0; i < m_vecExtraStuff.size(); ++i)
	{
		_AddToByteArray(pStream, m_vecExtraStuff[i].lMaxUseCount);			//! 每次升级装备最大使用数量
		_AddToByteArray(pStream, m_vecExtraStuff[i].lMinGoodsLevel);		//! 使用该宝石的冲等物品的最小冲等等级
		_AddToByteArray(pStream, m_vecExtraStuff[i].lAddOdds);				//! 使用后增加的成功几率百分数
		_AddToByteArray(pStream, m_vecExtraStuff[i].lFailedMinishLevel);	//! 使用后，冲等失败时，物品降低的冲等级数

		long lStuffNum = m_vecExtraStuff[i].listStuffIndex.size();
		_AddToByteArray(pStream, lStuffNum);
		for (list<long>::iterator itrStuff = m_vecExtraStuff[i].listStuffIndex.begin(); m_vecExtraStuff[i].listStuffIndex.end() != itrStuff; ++itrStuff)
			_AddToByteArray(pStream, *itrStuff);
	}

	//! 装备升级金钱配置
	_AddToByteArray(pStream, (long)m_vecUpgradeMoneySetList.size());
	for (long i = 0; i < m_vecUpgradeMoneySetList.size(); ++i)
	{
		_AddToByteArray(pStream, m_vecUpgradeMoneySetList[i].lMinLimitLevel);
		_AddToByteArray(pStream, m_vecUpgradeMoneySetList[i].lMaxLimitLevel);
		_AddToByteArray(pStream, (long)m_vecUpgradeMoneySetList[i].mapUpgradeMoneySet.size());
		map<long, long>::iterator ite = m_vecUpgradeMoneySetList[i].mapUpgradeMoneySet.begin();
		for ( ; ite != m_vecUpgradeMoneySetList[i].mapUpgradeMoneySet.end(); ++ite)
		{
			_AddToByteArray(pStream, ite->first);
			_AddToByteArray(pStream, ite->second);
		}
	}

	//! 装备最大等级
	_AddToByteArray(pStream, m_lUpgradeLimit);	

	//! 魔化配置
	{
		//! 装备等级范围设置
		_AddToByteArray(pStream, (long)m_ElementSetup.vElementBound.size());
		for (long i = 0; i < m_ElementSetup.vElementBound.size(); ++i)
		{
			_AddToByteArray(pStream, (long)m_ElementSetup.vElementBound[i].vElementStuff.size());
			if(0 < m_ElementSetup.vElementBound[i].vElementStuff.size())
				_AddToByteArray(pStream, &m_ElementSetup.vElementBound[i].vElementStuff[0], sizeof(tagElementStuff) * m_ElementSetup.vElementBound[i].vElementStuff.size());
			_AddToByteArray(pStream, (long)m_ElementSetup.vElementBound[i].vNeedNum.size());
			if(0 < m_ElementSetup.vElementBound[i].vNeedNum.size())
				_AddToByteArray(pStream, &m_ElementSetup.vElementBound[i].vNeedNum[0], sizeof(tagNeedNum) * m_ElementSetup.vElementBound[i].vNeedNum.size());
		}

		//! 魔化升级配置
		_AddToByteArray(pStream, (long)m_ElementSetup.vEquipmentLevelBound.size());
		if(0 < m_ElementSetup.vEquipmentLevelBound.size())
			_AddToByteArray(pStream, &m_ElementSetup.vEquipmentLevelBound[0], sizeof(tagEquipmentLevelBound) * m_ElementSetup.vEquipmentLevelBound.size());

		//! 几率辅助材料配置
		_AddToByteArray(pStream, (long)m_ElementSetup.vHelpfulSetup.size());
		for (long i = 0; i < m_ElementSetup.vHelpfulSetup.size(); ++i)
		{
			_AddToByteArray(pStream, m_ElementSetup.vHelpfulSetup[i].lMinLevel);
			_AddToByteArray(pStream, m_ElementSetup.vHelpfulSetup[i].lMaxLevel);
			_AddToByteArray(pStream, (long)m_ElementSetup.vHelpfulSetup[i].vHelpfulStuff.size());
			if(0 < m_ElementSetup.vHelpfulSetup[i].vHelpfulStuff.size())
				_AddToByteArray(pStream, &m_ElementSetup.vHelpfulSetup[i].vHelpfulStuff[0], sizeof(tagHelpfulStuff) * m_ElementSetup.vHelpfulSetup[i].vHelpfulStuff.size());
		}

		//! 魔化等级对应魔化属性值
		_AddToByteArray(pStream, (long)m_ElementSetup.vElementValue.size());
		if(0 < m_ElementSetup.vElementValue.size())
			_AddToByteArray(pStream, &m_ElementSetup.vElementValue[0], sizeof(tagElementValue) * m_ElementSetup.vElementValue.size());

		//! 元素重置配置
		_AddToByteArray(pStream, (long)m_ElementSetup.vResetElement.size());
		for (long i = 0; i < m_ElementSetup.vResetElement.size(); ++i)
		{
			_AddToByteArray(pStream, m_ElementSetup.vResetElement[i].lMinLevel);
			_AddToByteArray(pStream, m_ElementSetup.vResetElement[i].lMaxLevel);
			_AddToByteArray(pStream, (long)m_ElementSetup.vResetElement[i].vResetStuff.size());
			if(0 < m_ElementSetup.vResetElement[i].vResetStuff.size())
				_AddToByteArray(pStream, &m_ElementSetup.vResetElement[i].vResetStuff[0], sizeof(tagResetStuff) * m_ElementSetup.vResetElement[i].vResetStuff.size());
		}

		//! 相性重置配置
		_AddToByteArray(pStream, (long)m_ElementSetup.vResetXiangXing.size());
		for (long i = 0; i < m_ElementSetup.vResetXiangXing.size(); ++i)
		{
			_AddToByteArray(pStream, m_ElementSetup.vResetXiangXing[i].lMinLevel);
			_AddToByteArray(pStream, m_ElementSetup.vResetXiangXing[i].lMaxLevel);
			_AddToByteArray(pStream, &m_ElementSetup.vResetXiangXing[i].XiangXingOdds, sizeof(tagXiangXingOdds));
			_AddToByteArray(pStream, (long)m_ElementSetup.vResetXiangXing[i].vResetStuff.size());
			if(0 < m_ElementSetup.vResetXiangXing[i].vResetStuff.size())
				_AddToByteArray(pStream, &m_ElementSetup.vResetXiangXing[i].vResetStuff[0], sizeof(tagResetStuff) * m_ElementSetup.vResetXiangXing[i].vResetStuff.size());
		}

		//! 强化等级对应的魔化等级
		_AddToByteArray(pStream, (long)mapUpgradeToElementLevel.size());
		map<long,long>::iterator itor = mapUpgradeToElementLevel.begin();
		for(;itor!=mapUpgradeToElementLevel.end();itor++)
		{
			_AddToByteArray(pStream, itor->first);
			_AddToByteArray(pStream, itor->second);
		}
	}

	return true;
}




#endif //! USE_IN_SERVER

// 发送MSG_W2DB_SERVER_UNREGENTITY 清除DBS管理的对象
void CGame::SendUnRegDBEntity(const char* szCdkey)
{
	if(szCdkey)
	{
		CMessage msg(MSG_W2DB_SERVER_UNREGENTITY);
		msg.Add(szCdkey);
		msg.SendToSocket(GetDBSocketID());
	}
}

//!	注册一个排队定时器，并返回定时器ID
long CMsgTimer::RegisterMsgTimerID(long lFutureTime)
{
	//! m_PlayerTimer对象会自动释放pVar
	tagTimerVar* pVar = CBaseObject::CreateTimerVar();
	pVar->TimerType = ChildObjType_WSMsg;

	long lTimerID = GetGame()->GetTimerQueue()->Schedule(this, pVar, lFutureTime + timeGetTime());
	m_lTimerID = lTimerID;
	return lTimerID;
}
//!	取消一个排队的Timer
void CMsgTimer::CancelMsgTimer(void)
{
	void* pVar = NULL;
	GetGame()->GetTimerQueue()->Cancel(m_lTimerID, (const void**)&pVar);
}
//!	响应排队定时器触发的事件
void CMsgTimer::OnMsgTimeout(long lTimerID)
{
	// 清除MSG map
	CGame::mapMsgTimerItr itr = GetGame()->GetMsgTimerMap().find(m_ID);
	if(itr != GetGame()->GetMsgTimerMap().end())
	{
		GetGame()->GetMsgTimerMap().erase(itr);
	}

	// 加入到空闲列表, 不管是否超过最大数量
	GetGame()->GetFreeMsgList().push_back(this);

	// 释放MSGID
	GetGame()->FreeMsgID(m_ID);
}

// decode MSG_S2W_PLAYER_DETAIL_SAVE_PROPERTY 消息
bool CGame::DecodeSavePropertyMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSavePropertyMsg() Start.");
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
		long lHair = setReadDB.GetLongFromByteArray();
		//脸型
		long lFace = setReadDB.GetLongFromByteArray();
		// posX
		float posX = setReadDB.GetFloatFromByteArray( );
		// posY
		float posY = setReadDB.GetFloatFromByteArray( );
		//ticket
		long lTicket=setReadDB.GetLongFromByteArray();

		// ----------------------------
		//	1：头盔
		BYTE byHead = setReadDB.GetByteFromByteArray( );// ID
		long lHeadLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	2：项链
		BYTE byNecklace = setReadDB.GetByteFromByteArray( );// ID
		long lNecklaceLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	3：翅膀
		BYTE byWing = setReadDB.GetByteFromByteArray( );// ID
		long lWingLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	4：盔甲
		BYTE byBody = setReadDB.GetByteFromByteArray( );// ID
		long lBodyLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	5：腰带
		BYTE byBack = setReadDB.GetByteFromByteArray( );// ID
		long lBackLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	6：手套	
		BYTE byGlove = setReadDB.GetByteFromByteArray( );// ID
		long lGloveLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	7：鞋子
		BYTE byBoot = setReadDB.GetByteFromByteArray( );// ID
		long lBootLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	8：头饰
		BYTE byHeadgear = setReadDB.GetByteFromByteArray( );// ID
		long lHeadgearLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	9：外套		
		BYTE byFrock = setReadDB.GetByteFromByteArray( );// ID
		long lFrockLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	10：左戒指
		BYTE byLring = setReadDB.GetByteFromByteArray( );// ID
		long lLringLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	11：右戒指		
		BYTE byRring = setReadDB.GetByteFromByteArray( );// ID
		long lRringLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	12：勋章1
		BYTE byMedal1 = setReadDB.GetByteFromByteArray( );// ID
		long lMedal1Level = setReadDB.GetLongFromByteArray( ); // Level
		//	13	勋章2
		BYTE byMedal2 = setReadDB.GetByteFromByteArray( );// ID
		long lMedal2Level = setReadDB.GetLongFromByteArray( ); // Level
		//	14	勋章3
		BYTE byMedal3 = setReadDB.GetByteFromByteArray( );// ID
		long lMedal3Level = setReadDB.GetLongFromByteArray( ); // Level
		//	15：小精灵
		BYTE byFairy = setReadDB.GetByteFromByteArray( );// ID
		long lFairyLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	16：武器
		BYTE byWeapon = setReadDB.GetByteFromByteArray( );// ID
		long lWeaponLevel = setReadDB.GetLongFromByteArray( ); // Level
		//	17：副手武器
		BYTE byWeapon2 = setReadDB.GetByteFromByteArray( );// ID
		long lWeapon2Level = setReadDB.GetLongFromByteArray( ); // Level
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
			setReadDB.GetBufferFromByteArray(szPassword, PASSWORD_SIZE);
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
			pPlayer->SetOccupation(wOccu);
			pPlayer->SetSex(bySex);	
			pPlayer->SetCountry(byCountry);
			pPlayer->SetHeadPic(lHair);
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
			Account* pAcc = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());
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
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_HEAD );
			dbLPlayer->SetLongAttr(string("HeadLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	2：项链
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_NECKLACE );
			dbLPlayer->SetLongAttr(string("NECKLACE"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("NecklaceLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	3：翅膀
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_WING );
			dbLPlayer->SetLongAttr(string("WING"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("WingLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	4：盔甲
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_BODY );
			dbLPlayer->SetLongAttr(string("BODY"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("BodyLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	5：腰带
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_BACK );
			dbLPlayer->SetLongAttr(string("BACK"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("BackLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	6：手套	
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_GLOVE );
			dbLPlayer->SetLongAttr(string("GLOVE"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("GloveLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	7：鞋子
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_BOOT );
			dbLPlayer->SetLongAttr(string("BOOT"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("BootLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	8：头饰
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_HEADGEAR );
			dbLPlayer->SetLongAttr(string("HEADGEAR"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("HeadgearLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	9：外套		
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_FROCK );
			dbLPlayer->SetLongAttr(string("FROCK"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("FrockLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	10：左戒指
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_LRING );
			dbLPlayer->SetLongAttr(string("LRING"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("LRingLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	11：右戒指		
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_RRING );
			dbLPlayer->SetLongAttr(string("RRING"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("RRingLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	12：勋章1
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_MEDAL1 );
			dbLPlayer->SetLongAttr(string("MEDAL1"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("Medal1Level"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	13	勋章2
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_MEDAL2 );
			dbLPlayer->SetLongAttr(string("MEDAL2"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("Medal2Level"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	14	勋章3
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_MEDAL3 );
			dbLPlayer->SetLongAttr(string("MEDAL3"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("Medal3Level"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	15：小精灵
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_FAIRY );
			dbLPlayer->SetLongAttr(string("FAIRY"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("FairyLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	16：武器
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON );
			dbLPlayer->SetLongAttr(string("WEAPON"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
			dbLPlayer->SetLongAttr(string("WeaponLevel"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level
			//	17：副手武器
			pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON2 );
			dbLPlayer->SetLongAttr(string("WEAPON2"),(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) ); // ID
			dbLPlayer->SetLongAttr(string("Weapon2Level"),(DWORD)(pGoods?pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL,1):dwNull) ); // Level

			dbLPlayer->SetLongAttr(string("Region"),  (long)pPlayer->GetRegionID() );

			
		}
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSavePropertyMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveWalletGoodsMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveWalletGoodsMsg() Start.");
#endif

	// 解码装备物品
	pPlayer->m_cWallet.Release();
	pPlayer->m_cWallet.Unserialize(setReadDB);

#ifdef _RUNSTACKINFO1_	
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveWalletGoodsMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveSivlerGoodsMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSilverMsg() Start.");
#endif

	// 解码装备物品
	pPlayer->m_cSilverWallet.Release();
	pPlayer->m_cSilverWallet.Unserialize(setReadDB);

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSilverMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveEquipmentGoodsMsg(CPlayer* pPlayer, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveEquipmentGoodsMsg() Start.");
#endif
	// 解码装备物品
	pPlayer->m_cEquipment.Release();
	pPlayer->m_cEquipment.Unserialize(setReadDB);
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveEquipmentGoodsMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSavePackGoodsMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSavePackGoodsMsg() Start.");
#endif

	// 解码原始背包物品
	player->getOriginPack()->Release();
	player->getOriginPack()->SetContainerVolume(5*4);
	player->getOriginPack()->Unserialize(setReadDB);

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSavePackGoodsMsg() End.");
#endif
	return true;
}
bool CGame::DecodeDepotGoldMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeDepotGoldMsg() Start.");
#endif

	// 解码原始背包物品
	CGoodsContainer *pContainer = player->m_PlayerDepot.FindContainer(eDCT_Gold);
	if(NULL == pContainer)
	{
		char outStr[1024];
		sprintf(outStr, "DecodeDepotGoldMsg:Goods's Container is NULL!");
		AddLogText(outStr);
		return false;
	}
	long lFlag = setReadDB.GetLongFromByteArray();
	if(lFlag)
		pContainer->Unserialize(setReadDB);		
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeDepotGoldMsg() End.");
#endif
	return true;
}
bool CGame::DecodeDepotSilverMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeDepotSilverMsg() Start.");
#endif

	// 解码原始背包物品
	CGoodsContainer *pContainer = player->m_PlayerDepot.FindContainer(eDCT_Silver);
	if(NULL == pContainer)
	{
		char outStr[1024];
		sprintf(outStr, "DecodeDepotSilverMsg:Goods's Container is NULL!");
		AddLogText(outStr);
		return false;
	}
	long lFlag = setReadDB.GetLongFromByteArray();
	if(lFlag)
		pContainer->Unserialize(setReadDB);
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeDepotSilverMsg() End.");
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
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeDepotGoodsMsg() Start.");
#endif

	// 解码原始背包物品
	CVolumeLimitGoodsContainer *pContainer = dynamic_cast<CVolumeLimitGoodsContainer *>(player->m_PlayerDepot.FindContainer(eDCT_Primary));
	if(NULL == pContainer)
	{
		char outStr[1024];
		sprintf(outStr, "DecodeDepotGoodsMsg:Goods's Container is NULL!");
		AddLogText(outStr);
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
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeDepotGoodsMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveSubPackGoodsMsg(long subPackPos, CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSubPackGoodsMsg() Start.");
#endif
	player->GetSubpackContainer()->Release(subPackPos);
	player->UnserializeSubpack(setReadDB);
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSubPackGoodsMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveVarListMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveVarListMsg() Start.");
#endif
	
	// 将数据更新到CPlayer
	long lVarNum = 0;
	long lDataSize = 0;
	long tmpPos = 0;		
		
	//普通变量个数
	lVarNum = setReadDB.GetLongFromByteArray( );  
	//GUID个数
	int guidNum = setReadDB.GetLongFromByteArray( );		// 变量个数

	lDataSize=setReadDB.GetLongFromByteArray( );
	player->SetGuidNum(guidNum);
	player->SetVariableNum(lVarNum);
	// Vars Data
	if(lDataSize)
	{
		char* pVarList = (char*)M_ALLOC(lDataSize);
		setReadDB.GetBufferFromByteArray(pVarList, lDataSize);

		if(player)
		{			
			player->SetVariableData(pVarList);
			player->SetVariableDataLength(lDataSize);
		}	
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveVarListMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveSpriteSystemMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSpriteSystemMsg() Start.");
#endif
	
	player->m_SpriteSystem.DecodeFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSpriteSystemMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveQuestListMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveQuestListMsg() Start.");
#endif
	player->DecodeQuestDataFromDataBlock(setReadDB);

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveQuestListMsg() End.");
#endif
	return true;
}

//俑兵任务数据
bool CGame::DecodeSaveMerQuestListMsg(CPlayer* player, DBReadSet& setReadDB)
{
	player->DecodeMerQuestDataFromDataBlock(setReadDB);
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveMerQuestListMsg() End.");
#endif
	return true;
}

bool CGame::DecodeSaveSkillListMsg(CPlayer* player, DBReadSet& setReadDB)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSkillListMsg() Start.");
#endif
	long skillNum = setReadDB.GetLongFromByteArray();
	if(skillNum)
	{
		list<CPlayer::tagSkill> tSkillList;
		CPlayer::tagSkill tmpSkill;
		for(int i=0; i<skillNum; i++)
		{
			tmpSkill.wID  = setReadDB.GetDwordFromByteArray();
			tmpSkill.wLevel = setReadDB.GetDwordFromByteArray();
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
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"DecodeSaveSkillListMsg() End.");
#endif
	return true;
}
bool CGame::DecodeSaveDeputyOccuListMsg(CPlayer* player, DBReadSet& setReadDB)
{
	if(player)
		player->DecodeDOccuFromDataBlock(setReadDB);
	return true;
}
bool CGame::DecodeSaveStateListMsg(CPlayer* player, DBReadSet& setReadDB)
{
	// 将数据更新到CPlayer和CDBPlayer	
	long tmpBufSize = setReadDB.GetDwordFromByteArray( );
	BYTE* pTemptBuff = (BYTE*)M_ALLOC(tmpBufSize);
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
	tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(strGroup);
	if(pInfo)
		pPhRgnGroup = GetGame()->GetEntityManager()->FindRootEntity(strGroup, pInfo->guid);
	if(!pPhRgnGroup) return;

	map<CGUID, CBaseEntity*>::iterator dupRgnItr = pPhRgnGroup->GetEntityGroupMap().begin();
	for(; dupRgnItr != pPhRgnGroup->GetEntityGroupMap().end(); dupRgnItr++)
	{
		if(gsid == dupRgnItr->second->GetLongAttr(string("GSID"))) // 同一个GS的PH对象
		{
			CWorldRegion* wRgn = GetGlobalRgnManager()->FindRgnByGUID(dupRgnItr->second->GetGUID());
			if(!wRgn)
			{
				wRgn = MP_NEW CWorldRegion;
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
				CGlobalRgnManager::tagRegion* pTemplateRgn = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(wRgn->GetID());
				if(pTemplateRgn && pTemplateRgn->pRegion)
					wRgn->CloneAllData(pTemplateRgn->pRegion);
				
				wRgn->SetOwnerGUID(oGUID);
				wRgn->SetGsid(gsid);
				wRgn->SetName(dupRgnItr->second->GetStringAttr(string("name")));
				CPlayer* owner = GetGame()->GetMapPlayer(oGUID);
				if(owner)
					owner->SetPersonalHouseRgnGUID(wRgn->GetExID());
			}

			// 向WS发送副本数据
			CMessage msg(MSG_W2S_SERVER_SENDSETUP);
			msg.Add((long)SI_PHDUPRGN);
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
				AddLogText("InitPersonalHouseDataOnGS() phrgngoodsgroup is null!");
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
				tGoods.posX	  =	goodsItr->second->GetLongAttr(string("PosX")	);
				tGoods.posY	  =	goodsItr->second->GetLongAttr(string("PosY")	);

				msg.AddEx( &tGoods, sizeof(PHGoods));
			}
			msg.SendGSID(dupRgnItr->second->GetLongAttr(string("GSID")));
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
			GetGame()->DelRawPlayer(itr->second);
			m_mapSavePlayers.erase(itr);

			DelPlayerLog(pPlayer);
			m_mapSavePlayers[pPlayer->GetExID()] = pPlayer;
			AddCreatePlayerLog(ePlayerPushSave, pPlayer);
		}
		else
		{
			char szGuid[128];
			pPlayer->GetExID().tostring(szGuid);
			AddLogText("PushSavePlayer() player[%s] is Exist!", szGuid);
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
			pAcc = GetGame()->GetEntityManager()->FindAccount(saveItr->second->GetAccount());
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
			tDBLoginPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[loginplayer]"), saveItr->second->GetExID());
			pSession.NakeSendSaveLoginPlayerDataMsg(saveItr->second, tDBLoginPlayer);
			GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBLoginPlayer);
		}

		CEntityGroup* dbPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[player]"), saveItr->second->GetExID());
		pSession.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, saveItr->second, dbPlayer, true);
		GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)dbPlayer);
		
		// 通知LoginServer
		CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
		msg.Add(const_cast<char*>(saveItr->second->GetAccount()));
		msg.Add(0L);
		msg.SendToSocket(GetGame()->GetLSSocketID());//send to loginserver
#ifdef _RUNSTACKINFO_
		char pszLogingInfo[512];
		char szGuid[128];
		saveItr->first.tostring(szGuid);
		_snprintf(pszLogingInfo,512, "玩家[%s]延迟存储操作完成!", szGuid);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), pszLogingInfo);
#endif
		GetGame()->DelRawPlayer(saveItr->second);
	}
	m_mapSavePlayers.clear();
}

void CGame::SavePlayer(CPlayer *pPlayer)
{
	if(NULL != pPlayer)
	{
		CWorldServerSession pSession(5000);

		CEntityGroup* dbPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[player]"), pPlayer->GetExID());
		pSession.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, pPlayer, dbPlayer, true);
		GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)dbPlayer);
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
		lRet += itr->second.size();
		sprintf(szTmp, "place: %d, Num: %d.\r\n", itr->first, lRet);
		strcat(szInfo, szTmp);
	}

	PutStringToFile("MemAllocInfo", szInfo);
}
CPlayer* CGame::NewRawPlayer(eCreatePlayerPlace place)
{
	CPlayer* ret = MP_NEW CPlayer;
	AddCreatePlayerLog(place, ret);
	return ret;
}
void CGame::DelRawPlayer(CPlayer* player)
{
	DelPlayerLog(player);
	MP_DELETE(player);
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

	long lDiffSec = dwTime - mktime(&tm2007);
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