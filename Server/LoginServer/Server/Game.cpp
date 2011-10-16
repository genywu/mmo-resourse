#include "StdAfx.h"
#include "Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long g_lVersion =	10000;		// 程序版本号（同GameClient统一）第一位是测试版本号，后三位是版本
CGame* g_pGame= NULL;		    // 游戏客户端数据
HANDLE	hASConnectThread=NULL;			// AS连接线程句柄 
bool	bASConnectThreadExit=false;		// 是否退出AS连接线程

enum MyUserID
{
    // 下拉菜单                  <item level = "0" info = "加载(_L)" />
    LOAD_SETUP = MY_MENU * 2,     //<item level = "1" info = "服务器配置(_S)" />
    LOAD_SETUPEX,                 //<item level = "1" info = "服务器扩展配置(_E)" />
    LOAD_NOQUEUECDKEYLIST,        //<item level = "1" info = "不排队CDKEY列表(_C)" />
    LOAD_LOGINSETUP,              //<item level = "1" info = "玩家登录配置(_P)" />
    LOAD_IPLIMIT,                 //<item level = "1" info = "IP限制列表(_I)" />
    LOAD_WSLIST,                  //<item level = "1" info = "WS列表(_W)" />

    // 下拉菜单:                   <item level = "0" info = "操作(_O)" />
    OPER_KICKCDKEY = MY_MENU * 3, //<item level = "1" info = "踢出账号(_K)" />
    OPER_RESUMELOGIN,             //<item level = "1" info = "开启登录(_S)" />
    OPER_PAUSELOGIN,              //<item level = "1" info = "暂停登录(_P)" />
    OPER_OUTPUTINFO,              //<item level = "1" info = "开关请求基本信息输出(_O)" />
};

void RefreshInfo();

// 刷新服务器信息
void RefreshInfo()
{
    static char str[102400];

    if (!GetGame() || !GetGame()->GetNetServer_Client())
    {
        return;
    }

    static long lMaxConnects = 0, lMaxCdkeys = 0, lMaxPWErrorCdkeys = 0;
    static long lMaxRecv = 0;
    long lConnects = GetGame()->GetNetServer_Client()->GetClientCount();
    long lRecv = GetGame()->GetNetServer_Client()->GetRecvSizePerSecond(NF_Server_Client);

    long lCdkeys = GetGame()->GetLoginWorldCdkeyNumbers();

    long lPWErrorCdkeys = GetGame()->GetLoginManager().GetFrozenCdkeyNum();

    if (lConnects>lMaxConnects)
        lMaxConnects = lConnects;
    if (lCdkeys>lMaxCdkeys)
        lMaxCdkeys = lCdkeys;
    if (lPWErrorCdkeys>lMaxPWErrorCdkeys)
        lMaxPWErrorCdkeys=lPWErrorCdkeys;

    lMaxRecv += lRecv;
    sprintf(str, "Connects = %d/%d  Cdkeys = %d/%d  PWErrorCdkeys = %d/%d Recv = %d/%d(B/S)",
        lConnects, lMaxConnects,
        lCdkeys, lMaxCdkeys,
        lPWErrorCdkeys, lMaxPWErrorCdkeys,lRecv,lMaxRecv);

    // 显示每个World的人数
    GetGame()->WsPlayerNumIntoStringFoot(str);
    SetWindowText(AppFrame::GetWnd("info"), str);
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
:m_AsynchronousExecute(TRUE)
,m_pMapFile(NULL)
,m_pPat(NULL)
#ifdef SHOW_LOGIN_MSG_20090817
,bOutInfo(false)
#endif
{
    s_lAITick = 0;

    s_pNetServer_Client = NULL;
    s_pNetServer_World = NULL;
    m_lASSocket = 0;
    m_bExit = false;

    m_StateLvl[0] = -1;
    m_StateLvl[1] = 10000;
    m_StateLvl[2] = 20000;
    m_StateLvl[3] = 30000;

    m_Setup.lClientMaxBlockConNum = 10;
    m_Setup.lClientValidDelayRecDataTime = 4000;

    m_Setup.lWorldMaxBlockConNum = 5;
    m_Setup.lWorldValidDelayRecDataTime = 4000;

    m_mapLoginSocket.clear();
}

CGame::~CGame(void)
{
}

void CGame::UpdateStateLvls()
{
        m_StateLvl[0] = -1;
        m_StateLvl[1] = (long)(GetLoginSetup()->m_dwMaxWsMum * GetLoginSetup()->fWorldBusyScale);
        m_StateLvl[2] = (long)(GetLoginSetup()->m_dwMaxWsMum * GetLoginSetup()->fWorldFullScale);
        m_StateLvl[3] = GetLoginSetup()->m_dwMaxWsMum;
}

bool CGame::LoadSetup()
{
    // 读入配置setup.ini
    string strTemp;
    string strSqlConType;//连接类型
    string strSqlIP;//数据库服务器IP
    string strSqlUserName;//数据库用户名
    string strSqlPassWord;//密码
    string strDBName;//DATABASE NAME
    ifstream stream;
    stream.open("setup.ini");
    if( stream.is_open() )
    {
        stream  >> strTemp >> GetSetup()->dwListenPort_Client
            >> strTemp >> GetSetup()->dwListenPort_World

            >> strTemp >> GetSetup()->strAccountServerIp	//! 验证服务器IP
            >> strTemp >> GetSetup()->dwAccountServerPort	//! 验证服务器端口
            >> strTemp >> GetSetup()->dwAccountClientNo		//! 验证客户端编号


            >> strTemp >> GetSetup()->strLocalSqlConType		//! 本地账号验证配置，DB相关
            >> strTemp >> GetSetup()->strLocalSqlServerIP
            >> strTemp >> GetSetup()->strLocalSqlUserName
            >> strTemp >> GetSetup()->strLocalSqlPassWord
            >> strTemp >> GetSetup()->strLocalDBName;

        stream.close();
        char title[256];
        sprintf(title, "[%d]LoginServer[%d]", GetSetup()->dwListenPort_World, g_lVersion);
        if (!AppFrame::SetTitle(title))
            return false;

        Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_0"));//加载setup.int成功!
    }

    // 从加密文件读取
    else
    {
        AppFrame::NotifyMessageBox("Can't find file setup.ini");
        return false;
    }

    ifstream streamSetupEx;
    streamSetupEx.open("SetupEx.ini");
    if( streamSetupEx.is_open() )
    {
        streamSetupEx>> strTemp >> GetSetup()->dwDataBlockNum
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
            >> strTemp >> GetSetup()->lClientMaxBlockConNum
            >> strTemp >> GetSetup()->lClientValidDelayRecDataTime

            >> strTemp >> GetSetup()->dwWorldFreeSockOperNum
            >> strTemp >> GetSetup()->dwWorldFreeIOOperNum
            >> strTemp >> GetSetup()->lWorldIOOperDataBufNum
            >> strTemp >> GetSetup()->bWorldCheckNet
            >> strTemp >> GetSetup()->dwWorldBanIPTime
            >> strTemp >> GetSetup()->dwWorldMaxMsgLen			
            >> strTemp >> GetSetup()->lWorldMaxConnectNum
            >> strTemp >> GetSetup()->lWorldMaxClientsNum
            >> strTemp >> GetSetup()->lWorldPendingWrBufNum
            >> strTemp >> GetSetup()->lWorldPendingRdBufNum
            >> strTemp >> GetSetup()->lWorldMaxSendSizePerSecond
            >> strTemp >> GetSetup()->lWorldMaxRecvSizePerSecond
            >> strTemp >> GetSetup()->lWorldMaxBlockedSendMsgNum
            >> strTemp >> GetSetup()->lWorldConPendingWrBufNum
            >> strTemp >> GetSetup()->lWorldConPendingRdBufNum
            >> strTemp >> GetSetup()->lWorldConMaxSendSizePerSecond
            >> strTemp >> GetSetup()->lWorldConMaxRecvSizePerSecond
            >> strTemp >> GetSetup()->lWorldConMaxBlockedSendMsgNum
            >> strTemp >> GetSetup()->lWorldMaxBlockConNum
            >> strTemp >> GetSetup()->lWorldValidDelayRecDataTime

            >> strTemp >> GetSetup()->dwRefeashInfoTime			// 刷新显示时间
            >> strTemp >> GetSetup()->dwSaveInfoTime				// 保存一次信息的时间间隔
            >> strTemp >> GetSetup()->dwPingWorldServerTime
            >> strTemp >> GetSetup()->dwPingWorldServerErrorTime

            >> strTemp >> GetSetup()->dwMinDbOptThreadNum		//! 本地账号验证配置, 最小DB操作线程保持数
            >> strTemp >> GetSetup()->dwMaxDbOptThreadNum		//! 本地账号验证配置，最大DB操作线程数
            >> strTemp >> GetSetup()->dwThreadNumAdjustSpace;	//! 本地账号验证配置，自动调整线程数的间隔时间

        streamSetupEx.close();
        UpdateStateLvls();
        ChangeAllWorldSate();
        Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_1"));// 加载setupEx.int成功!
    }
    else
    {
        AppFrame::NotifyMessageBox("Can't find file SetupEx.ini");
        return false;
    }

    return LoadLoginSetup() && LoadIpLimitSetup();
}

bool CGame::LoadLoginSetup(VOID)
{
    string strTemp;
    ifstream streamSetupEx;
    streamSetupEx.open("loginSetup.ini");
    if( streamSetupEx.is_open() )
    {
        streamSetupEx
            >> strTemp >> GetLoginSetup()->bSndaValidate				//! 是否使用盛大方式验证
            >> strTemp >> GetLoginSetup()->bXmCardEnable				//! 是否使用星漫密保卡
            >> strTemp >> GetLoginSetup()->bSndaFcmEnable				//! 是否启用盛大防沉迷

            >> strTemp >> GetLoginSetup()->dwDoQueueInter				//! 处理等待队列的间隔时间
            >> strTemp >> GetLoginSetup()->dwSendMsgToQueInter			//! 给等待队列发送消息的间隔时间

            >> strTemp >> GetLoginSetup()->m_dwMaxWsMum					//!	WS最大人数
            >> strTemp >> GetLoginSetup()->m_dwProcessRankInterval		//!	处理排队间隔（毫秒）
            >> strTemp >> GetLoginSetup()->m_dwSendQueueMsgInterval		//!	发送排队消息间隔（毫秒）
            >> strTemp >> GetLoginSetup()->m_dwOneProcessNum			//!	每次处理人数（每个WS）

            >> strTemp >> GetLoginSetup()->fWorldBusyScale				//! 繁忙的人数比例
            >> strTemp >> GetLoginSetup()->fWorldFullScale				//! 爆满的人数比例

            >> strTemp >> GetLoginSetup()->m_ValidateWaitTimeSpace		//!	验证等待时间（毫秒）
            >> strTemp >> GetLoginSetup()->m_SelectWaitTimeSpace		//!	选人界面的最长时间（毫秒）
            >> strTemp >> GetLoginSetup()->m_JustOutTimeSpace			//!	小退超时时间（毫秒）

            >> strTemp >> GetLoginSetup()->m_MaxPwdErrCount				//!	连续密码输入错误最大次数
            >> strTemp >> GetLoginSetup()->m_PwdErrSaveTime				//!	密码错误次数保存时间(秒)
            >> strTemp >> GetLoginSetup()->m_FrostCdkeyTime				//! 冻结登陆时间(秒)

            >> strTemp >> GetLoginSetup()->m_szCdkeyRegex				//! 验证账号用的正则表达式

            >> strTemp >> GetLoginSetup()->m_bValideClientResourceVersion//! 冻结登陆时间(秒)
            >> strTemp >> GetLoginSetup()->m_szClientResourceVersion;	//! 冻结登陆时间(秒)


        streamSetupEx.close();
        UpdateStateLvls();
        ChangeAllWorldSate();
        GetLoginManager().ChangeSetup(
            GetLoginSetup()->m_dwMaxWsMum,
            GetLoginSetup()->m_dwProcessRankInterval,
            GetLoginSetup()->m_dwSendQueueMsgInterval,
            GetLoginSetup()->m_dwOneProcessNum	
            );

        if(NULL != m_pPat)
            SAFE_DELETE(m_pPat);
        m_pPat = new rpattern(GetGame()->GetLoginSetup()->m_szCdkeyRegex);

         Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_2"));//加载loginSetup.ini成功!
        return true;
    }

    AppFrame::NotifyMessageBox("Can't find file loginSetup.ini");
    return false;
}

bool CGame::LoadIpLimitSetup(VOID)
{
    string strTemp;
    ifstream streamSetupEx;
    streamSetupEx.open("IpLimitSetup.ini");
    if( streamSetupEx.is_open() )
    {
        LoginManager::tagLimitIpSetup LimitIpSetup;
        streamSetupEx
            //! 是否限制IP登陆次数
            >> strTemp >> LimitIpSetup.m_bIpLogin
            //! 一个IP同一个账号最大登陆次数
            >> strTemp >> LimitIpSetup.m_MaxIpLoginOncCdKeyCount
            //! 一个IP登陆不同账号最大数
            >> strTemp >> LimitIpSetup.m_MaxIpLoginCdKeyCount
            //! 清除登陆IP记录时间（毫秒）
            >> strTemp >> LimitIpSetup.m_ClearIpLoginTime
            //! 是否限制IP创建角色次数
            >> strTemp >> LimitIpSetup.m_bIpCreate
            //! 同IP最大创建角色次数
            >> strTemp >> LimitIpSetup.m_MaxIpCreateCount
            //! 清除创建角色IP记录时间（毫秒）
            >> strTemp >> LimitIpSetup.m_ClearIpCreateTime
            //! 屏蔽IP时间（毫秒）
            >> strTemp >> LimitIpSetup.m_FrozenIpTime;		

        streamSetupEx.close();

        //! 从配置中读到是分钟数，转换时间单位
        LimitIpSetup.m_ClearIpLoginTime		*= 1000 * 60;
        LimitIpSetup.m_ClearIpCreateTime	*= 1000 * 60;
        LimitIpSetup.m_FrozenIpTime			*= 1000 * 60;

        GetLoginManager().ChangeIpSetup(LimitIpSetup);
         Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_3"));// 加载IpLimitSetup.ini成功!
    }
    else
    {
        AppFrame::NotifyMessageBox("Can't find file IpLimitSetup.ini");
        return false;
    }

    return true;
}

rpattern& CGame::GetRpattern(VOID)
{
    if(NULL == m_pPat)
        m_pPat = new rpattern(GetGame()->GetLoginSetup()->m_szCdkeyRegex);
    return *m_pPat;
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

bool CGame::ReLoadSetupEx()
{
    LoadSetup();

    s_pNetServer_Client->SetParamEx(GetSetup()->lClientMaxBlockConNum,GetSetup()->lClientValidDelayRecDataTime);
    s_pNetServer_World->SetParamEx(GetSetup()->lWorldMaxBlockConNum,GetSetup()->lWorldValidDelayRecDataTime);
    return true;
}


bool	CGame::ReLoadWorldSetup()
{
    LoadWorldSetup();

    //更新客户端的显示信息
    UpdateWorldInfoToAllClient();
    UpdateDisplayWorldInfo();

    return true;
}

BOOL CGame::InitStackFileName()
{
    SYSTEMTIME stTime;
    GetLocalTime(&stTime);
    _snprintf(m_pszRunStackFile,MAX_PATH,"RunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

    return TRUE;
}

bool	CGame::LoadWorldSetup()
{
    m_WorldInfoSetup.clear();
    ifstream stream;
    stream.open("WorldInfoSetup.ini");
    if( !stream.is_open() )
        return false;
    long	lNum;
    while(ReadTo(stream, "#"))
    {
        tagWorldInfo	WorldInfo;
        stream >> lNum >> WorldInfo.strName >> WorldInfo.lStateLvl;
        m_WorldInfoSetup[lNum] = WorldInfo;
    }
    stream.close();
    return true;
}

void CGame::SetListWorldInfoBySetup()
{
    m_listWorldInfo.clear();
    m_listWorldInfo = m_WorldInfoSetup;
    map<long,tagWorldInfo>::iterator it = m_listWorldInfo.begin();
    for(;it != m_listWorldInfo.end();it++)
    {
        (*it).second.lStateLvl = 0;//默认为维护中
    }
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
		//装载客户端资源信息出错
		//ASSERT(false);
		//return false;
	}
	// Log4c::Trace(ROOT_MODULE,"Load package file OK!");
	return true;
}

bool CGame::UnLoadServerResource()
{
	CClientResource* pClientResource = CClientResource::GetInstance();
	if(pClientResource)
	{
		pClientResource->Release();
		CClientResource::ReleaseInstance();
	}
	return true;
}

// 游戏初始化
BOOL CGame::Init()
{
    srand((unsigned)time(NULL));
    random(100);
    CreateDirectory( "log",	NULL);
    Log4c::Trace(ROOT_MODULE, "[GAME] LoginServer start!");


//////////////////////////////////////////////////////////////////////////add by shixi
	LoadServerResource();
	//读取全局字符串
	AppFrame::LoadTextRes();
//////////////////////////////////////////////////////////////////////////
    if (LoadSetup()==false)
    {
        AppFrame::NotifyMessageBox("Can't find file setup.ini");
        return false;
    }

#ifdef _RUNSTACKINFO_
    InitStackFileName();
#endif

    //! 初始化异步执行对象
    GetAsynchronousExecute().SetWorkParam(
        GetSetup()->dwMinDbOptThreadNum, 
        GetSetup()->dwMaxDbOptThreadNum, 
        GetSetup()->dwThreadNumAdjustSpace
        );
    GetAsynchronousExecute().BeginWork(GetSetup()->dwMinDbOptThreadNum);
    GetLoginManager().ReLoadNoQueue();

    string	strConnectionString = 
        "Provider=" + GetSetup()->strLocalSqlConType +
        "; Server=" + GetSetup()->strLocalSqlServerIP + 
        "; Database=" + GetSetup()->strLocalDBName + 
        "; UID=" + GetSetup()->strLocalSqlUserName + 
        "; PWD=" + GetSetup()->strLocalSqlPassWord + 
        "; OLE DB Services=-1; Driver={SQL Server}";
    LoaclAccountValidate::Init(strConnectionString);

    char strVersion[256];
    extern long g_lVersion;
    sprintf(strVersion,"LoginServer[%d][%d]-StarDust", g_lVersion, GetSetup()->dwListenPort_Client);

    //查看此程序是否在运行中
    HWND hWnd = FindWindow(NULL,strVersion);
    if (hWnd != NULL)
    {
        char strMsgBox[256]="";
        sprintf(strMsgBox,"%s 程序正在运行中!",strVersion);
        AppFrame::NotifyMessageBox(_T(strMsgBox), "error", MB_OK);
        return false;
    }

    SetWindowText(hWnd, strVersion);

    if(LoadWorldSetup() == false)
    {
       Log4c::Warn(ROOT_MODULE,"LOGIN_GAME_7");//Can't find file WorldSetup.ini
        return false;
    }
    SetListWorldInfoBySetup();
    UpdateDisplayWorldInfo();

    //初始化CMyAdoBase
    CMyAdoBase::Initialize(GetSetup()->strLocalSqlConType,GetSetup()->strLocalSqlServerIP,GetSetup()->strLocalDBName,
        GetSetup()->strLocalSqlUserName,GetSetup()->strLocalSqlPassWord); 


    //分配数据块
    m_pDBAllocator = new CDataBlockAllocator(m_Setup.dwDataBlockNum,m_Setup.dwDataBlockSize);

    //初始
    CMessage::Initial(m_pDBAllocator,m_Setup.dwFreeMsgNum);
    // 初始化服务器网络
    CMySocket::MySocketInit();
    CMySocket::SetEncryptFlag(m_Setup.lEncryptType);

    //装载内存影射文件配置
    LoadMapFileSetup();

    // 创建服务器端
    if( !InitNetServer_World() )
    {
        AppFrame::NotifyMessageBox("Can't init NetServerWorld!");
        return false;
    }

    // 连接到LoginServer
    if( !InitNetServer_Client() )
    {
        AppFrame::NotifyMessageBox("Can't init NetServerClient!");
        return false;
    }

    // 建立和AccountServer的连接
    if( !StartConnAccountServer() )
    {
       Log4c::Warn(ROOT_MODULE,GetText("LOGIN_GAME_24"));//不能连接AccountServer，请先运行AccountServer！
        CreateConnectAccountServer();
    }


    if(GetLoginSetup()->bSndaValidate)
    {
        if(GetCSdoaValidate().Init())
        {
            Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_8"));//盛大通行证模块初始化成功!
        }          
        else
        {
            AppFrame::NotifyMessageBox("盛大通行证模块初始化失败，请检查配置!");
            return false;
        }

        if(GetLoginSetup()->bSndaFcmEnable)
        {
            if(GetCSdoaFcm().Init())
            {
                Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_9"));//盛大防沉迷模块初始化成功!
            }
            else
            {
                AppFrame::NotifyMessageBox("盛大防沉迷模块初始化失败，请检查配置");
                return false;
            }
        }
    }

    //! 打开信息回应服务
    m_COnCmdConnect.Init();
	UnLoadServerResource();
    return true;
}

// 游戏释放
BOOL CGame::Release()
{
    Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_10"));//== LoginServer Exiting... ==
    if( !m_bExit )
    {
        m_bExit = true;
        bASConnectThreadExit = true;

        m_COnCmdConnect.Release();

        GetCSdoaFcm().Release();
        GetCSdoaValidate().Release();

        //! 结束异步执行对象
        GetAsynchronousExecute().Exit();

        m_AinMsgQueue.Release();

        if( GetNetServer_World() )	
        {
            s_pNetServer_World->Exit();			
        }

        if( GetNetServer_Client() )
        {
            s_pNetServer_Client->Exit();
        }

        SAFE_DELETE(s_pNetServer_Client);
        SAFE_DELETE(s_pNetServer_World);

        CMySocket::MySocketClearUp();		

        LoaclAccountValidate::Release();

        CMyAdoBase::Uninitalize();
        GetInst(CMemClient).Release();
        CMessage::Release();
        SAFE_DELETE(m_pDBAllocator);
        SAFE_DELETE(m_pPat);
    }
    Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_11"));//== LoginServer Exited! ==
    
    return true;
}

//建立和LoginServer的连接，作为客户端初始化
BOOL CGame::InitNetServer_Client(void)
{
    s_pNetServer_Client = new CServer;
    long lRet = s_pNetServer_Client->Start(2,m_pDBAllocator,
        m_Setup.dwFreeSockOperNum,m_Setup.dwFreeIOOperNum,m_Setup.lIOOperDataBufNum,
        m_Setup.bCheckNet,m_Setup.dwBanIPTime,
        m_Setup.dwMaxMsgLen,m_Setup.lMaxConnectNum,m_Setup.lMaxClientsNum,
        m_Setup.lPendingWrBufNum,m_Setup.lPendingRdBufNum,
        m_Setup.lMaxSendSizePerSecond,m_Setup.lMaxRecvSizePerSecond,
        m_Setup.lMaxBlockedSendMsgNum);
    if( !lRet )
    {
        AppFrame::NotifyMessageBox("Start Client Net server Error");
        return FALSE;
    }

    BOOL bRet = s_pNetServer_Client->Host(m_Setup.dwListenPort_Client,
        NULL, NF_Server_Client,SOCK_STREAM);
    if(!bRet)
    {
        AppFrame::NotifyMessageBox("Start Host for client Error");
        return FALSE;
    }

    CMessage::SetClientServer(s_pNetServer_Client);

    Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_12"));//LoginServer 作为Client的服务器初始化成功 !

    
    return TRUE;
}

//初始化游戏网络服务器端，建立网络服务器端，装载数据
BOOL CGame::InitNetServer_World(void)
{
    s_pNetServer_World = new CServer;
    long lRet = s_pNetServer_World->Start(3,m_pDBAllocator,
        m_Setup.dwWorldFreeSockOperNum,m_Setup.dwWorldFreeIOOperNum,m_Setup.lWorldIOOperDataBufNum,
        m_Setup.bWorldCheckNet,m_Setup.dwWorldBanIPTime,
        m_Setup.dwWorldMaxMsgLen,m_Setup.lWorldMaxConnectNum,m_Setup.lWorldMaxClientsNum,
        m_Setup.lWorldPendingWrBufNum,m_Setup.lWorldPendingRdBufNum,
        m_Setup.lWorldMaxSendSizePerSecond,m_Setup.lWorldMaxRecvSizePerSecond,
        m_Setup.lWorldMaxBlockedSendMsgNum);
    if( !lRet )
    {
        AppFrame::NotifyMessageBox("Start World Net server Error");
        return FALSE;
    }

    BOOL bRet = s_pNetServer_World->Host(m_Setup.dwListenPort_World,NULL,NF_Server_World,SOCK_STREAM);
    if(!bRet)
    {
        AppFrame::NotifyMessageBox("Start Host for world server Error");
    }

    CMessage::SetWorldServer(s_pNetServer_World);
    Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_13"));//LoginServer 作为WorldServer的服务器初始化成功 !

    return TRUE;
}



// 游戏AI
BOOL CGame::AI()
{
    return TRUE;
}

// 消息处理
BOOL CGame::ProcessMessage()
{

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Process World Message Start.");
#endif

    if(GetNetServer_World())
    {
        long lMessageCount = GetNetServer_World()->GetRecvMessages()->GetSize();

        for(int i=0; i<lMessageCount; i++)
        {
            CMessage* pMsg =(CMessage*) GetNetServer_World()->GetRecvMessages()->PopMessage();
            if( pMsg )
            {
                //ShowMessage("4:%x:%x",pMsg, pMsg->GetClientSocket());

                pMsg->Run();
                CBaseMessage::FreeMsg(pMsg);
            }
        }
    }

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Process Client Message Start.");
#endif
    if(GetNetServer_Client())
    {
        long lMessageCount = GetNetServer_Client()->GetRecvMessages()->GetSize();

        for(int i=0; i<lMessageCount; i++)
        {
            CMessage* pMsg = (CMessage*)GetNetServer_Client()->GetRecvMessages()->PopMessage();
            if( pMsg )
            {
                //ShowMessage("4:%x:%x",pMsg, pMsg->GetClientSocket());

                pMsg->Run();
                CBaseMessage::FreeMsg(pMsg);
            }
        }
    }

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Process Ain Message Start.");
#endif
    //! 处理自己发给自己的消息
    list<AinMessage*> listMsg;
    m_AinMsgQueue.PopMessage(listMsg);
    for(list<AinMessage*>::iterator ite = listMsg.begin(); listMsg.end() != ite; ite++)
    {
        if( NULL != *ite )
        {
            (*ite)->Run();
            AinMessage::Release(&(*ite));
        }
    }

    static msgQueue TemptMsgQueue;
    GetInst(CMemClient).GetRecvMsgs()->GetAllMessage(TemptMsgQueue);
    msgQueue::iterator it = TemptMsgQueue.begin();
    for(;it != TemptMsgQueue.end();it++)
    {
        CMessage* pMsg = (CMessage*)(*it);
        if( pMsg )
        {
            pMsg->Run();
            CBaseMessage::FreeMsg(pMsg);
        }
    }

    TemptMsgQueue.clear();

    return TRUE;
}

bool CGame::ProcMenus()
{
    bool res = false;
    if (AppFrame::MenuClicked(LOAD_SETUP))
        res = LoadSetup();
    if (AppFrame::MenuClicked(LOAD_SETUPEX))
        res = ReLoadSetupEx();
    if (AppFrame::MenuClicked(LOAD_NOQUEUECDKEYLIST))
        res = GetLoginManager().ReLoadNoQueue();
    if (AppFrame::MenuClicked(LOAD_LOGINSETUP))
        res = LoadLoginSetup();
    if (AppFrame::MenuClicked(LOAD_IPLIMIT))
        res = LoadIpLimitSetup();
    if (AppFrame::MenuClicked(LOAD_WSLIST))
        res = ReLoadWorldSetup();
    //if (AppFrame::MenuClicked(OPER_KICKCDKEY)); // 踢出帐号
    //if (AppFrame::MenuClicked(OPER_RESUMELOGIN)); // 恢复登陆
    //if (AppFrame::MenuClicked(OPER_PAUSELOGIN)); // 暂停登陆
#ifdef SHOW_LOGIN_MSG_20090817
    if (AppFrame::MenuClicked(OPER_OUTPUTINFO))
        GetGame()->OpenCloseInfo();
#endif

    return res;
}

//! 响应AccountServer连接断开
VOID CGame::OnNetASClose(VOID)
{
    m_AccountClient.SetASSocket(0);
    Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_14"));//AccountServer关闭,请重新启动AccountServer!

    CreateConnectAccountServer();
}

//! 建立和AccountServer的连接，作为客户端初始化
BOOL CGame::StartConnAccountServer(void)
{
    Log4c::Trace(ROOT_MODULE,GetText("LOGIN_GAME_15"));//正在连接AccountServer...
    m_lASSocket = s_pNetServer_World->Connect(GetSetup()->strAccountServerIp.c_str(), GetSetup()->dwAccountServerPort,NF_AsscountServer_Client);
    if(m_lASSocket>0)
    {
        CMessage::SetASSocket(m_lASSocket);
        m_AccountClient.SetASSocket(m_lASSocket);

        CMessage msg(MSG_O2A_LOGIN_Affirm_Request);
        msg.Add(GetSetup()->dwAccountClientNo);
        msg.SendToAS();
        return TRUE;
    }
    Log4c::Warn(ROOT_MODULE,GetText("LOGIN_GAME_22"));//连接AccountServer失败!
    return FALSE;
}
//! 重连AccountServer
BOOL CGame::ReConnectAccountServer(VOID)
{
    if(bASConnectThreadExit)
        return FALSE;
    m_lASSocket = s_pNetServer_World->Connect(GetSetup()->strAccountServerIp.c_str(), GetSetup()->dwAccountServerPort,NF_AsscountServer_Client);
    if(m_lASSocket>0)
    {
        CMessage::SetASSocket(m_lASSocket);
        m_AccountClient.SetASSocket(m_lASSocket);

        CMessage msg(MSG_O2A_LOGIN_Affirm_Request);
        msg.Add(GetSetup()->dwAccountClientNo);
        msg.SendToAS();
        return TRUE;
    }
    return FALSE;
}

//! 创建连接线程
VOID CGame::CreateConnectAccountServer(VOID)
{
    bASConnectThreadExit=true;
    WaitForSingleObject(hASConnectThread,INFINITE);
    CloseHandle(hASConnectThread);

    bASConnectThreadExit=false;

    unsigned threadID;
    hASConnectThread = (HANDLE)_beginthreadex(NULL,0,CGame::ConnectAccountFunc,NULL,0,&threadID);
}

unsigned __stdcall CGame::ConnectAccountFunc(LPVOID pArguments)
{
    try
    {
        while(true)
        {
            if(bASConnectThreadExit)
                break;
            Sleep(4000);

            if(GetGame()->ReConnectAccountServer()==TRUE)
            {
                break;
            }
        }
    }
    catch(...)
    {
       Log4c::Warn(ROOT_MODULE,"LOGIN_GAME_23");//重连AccountServer的线程出错!
    }
    hASConnectThread = 0;
    _endthreadex( 0 );
    return 0;
}

// 游戏主循环
BOOL CGame::MainLoop()
{
#ifdef _RUNSTACKINFO_
    CMessage::AsyClearFile(GetGame()->GetStatckFileName());
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MainLoop() Start.");
#endif
    static DWORD  now, lastRefeashInfoTime = now, lastShowLMInfo = now;
    now = timeGetTime();

    static DWORD ServerInfoLogLastTime = now;
    // 刷新显示
    if (now - lastRefeashInfoTime > GetSetup()->dwRefeashInfoTime)
    {
        lastRefeashInfoTime = now;
        RefreshInfo();
    }

    //! 5分钟显示一次调试信息
    if(now - lastShowLMInfo > 1000 * 60 * 5)
    {
        lastShowLMInfo = now;
        GetLoginManager().OutLoginInfo();
    }

    // 结束线程
    if( m_bExit )
    {
        return false;
    }

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessMessage() Start.");
#endif

    // 消息处理
    ProcessMessage();
    ProcMenus();

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"ProcessMessage() End.");
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"LoginManager::ProcessQueue() Start.");
#endif

    //! 处理登陆
    GetLoginManager().ProcessQueue();

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"LoginManager::ProcessQueue() End.");
#endif

    // 定时ping其下的WorldServer信息
    static DWORD lastPingWorldTime = now;
    static bool bInPing = false;

    if (bInPing == false)	// 检测是否需要ping的过程
    {
        if (now - lastPingWorldTime > GetSetup()->dwPingWorldServerTime)
        {
            lastPingWorldTime = now;
            bInPing = true;

            GetGame()->m_vectorPingWorldServerInfo.clear();
            CMessage msg(MSG_L2W_SERVER_QUEST_PING);
            msg.SendAllWorld();
        }
    }
    else	// 等待所有WorldServer返回的过程
    {
        // 出错，认为有的WorldServer已经失去响应
        if (now - lastPingWorldTime > GetSetup()->dwPingWorldServerErrorTime)
        {
            bInPing = false;
            long pings = (long)m_vectorPingWorldServerInfo.size();
            long total = GetWorldNum();
            if ( pings < total )
            {
                Log4c::Trace(ROOT_MODULE,FormatText("LOGIN_GAME_16", pings, total));//WorldServer 未全部启动 (%d/%d):
            }

            for (int i=0; i<(int)m_vectorPingWorldServerInfo.size(); i++)
            {
                //WorldServer[%s:%s] 在线玩家: %d 人
                Log4c::Trace(ROOT_MODULE,FormatText("LOGIN_GAME_17",
                    m_vectorPingWorldServerInfo[i].strName.c_str(),
                    m_vectorPingWorldServerInfo[i].strIP.c_str(),
                    m_vectorPingWorldServerInfo[i].dwPlayerNum));
            }
            CMessage msg(MSG_O2A_SERVER_RUN_ERROR);
            msg.SendToAS();
        }
    }

    Sleep(1);
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MainLoop() End.");
#endif
    return true;
}


/////////////////////////////////////////////////////////////////////////
// WorldServer信息列表
/////////////////////////////////////////////////////////////////////////

// 添加世界
long CGame::AddWorld(long lWorldNumber,const char* strWorldName)
{
    if(strWorldName==NULL)
        return -1;

    map<long,tagWorldInfo>::iterator it = m_listWorldInfo.find(lWorldNumber);
    if(it == m_listWorldInfo.end())
    {
        Log4c::Warn(ROOT_MODULE,"LOGIN_GAME_18");//非法的World Server连接!
        return -1;
    }
    if( (*it).second.strName != strWorldName)
    {
        Log4c::Warn(ROOT_MODULE,"LOGIN_GAME_18");//非法的World Server连接!
        return -1;
    }
    (*it).second.lStateLvl = 1;

    //更新客户端World的显示信息
    UpdateWorldInfoToAllClient();
    UpdateDisplayWorldInfo();
    long lSize = (long)m_listWorldInfo.size();

    //添加一个WorldServer的 CDkeyList
    WorldCdkeyList CDkeyList;
    CDkeyList.clear();
    s_listCdkey[lWorldNumber] = CDkeyList;
    GetLoginManager().AddWsWaitQueue(lWorldNumber);

    return lSize;
}

// 删除世界
long CGame::DelWorld(long lNum)
{
    map<long,tagWorldInfo>::iterator it = m_listWorldInfo.find(lNum);
    if(it != m_listWorldInfo.end())
    {
        (*it).second.lStateLvl = 0;
    }

    //更新客户端的显示信息
    UpdateWorldInfoToAllClient();
    UpdateDisplayWorldInfo();
    long lSize = (long)m_listWorldInfo.size();

    //清除
    GetLoginManager().DelWsWaitQueue(lNum);
    s_listCdkey.erase(lNum);

    return lSize;
}

const char *CGame::GetWorldNameByID(long lNum)
{
    std::map<long,tagWorldInfo>::iterator it = m_listWorldInfo.find(lNum);
    if(it != m_listWorldInfo.end())
    {
        return (*it).second.strName.c_str();
    }
    return NULL;
}

long  CGame::GetWorldIDByName(const char* strWorldName)
{
    if(strWorldName==NULL)
        return -1;
    std::map<long,tagWorldInfo>::iterator it = m_listWorldInfo.begin();
    for(;it != m_listWorldInfo.end();it++)
    {
        if(strcmp(strWorldName,(*it).second.strName.c_str())==0)
        {
            if( (*it).second.lStateLvl == 0 )	return -1;
            else return (*it).first;
        }
    }
    return -1;
}

bool CGame::WorldServerIsOpenState(long lNum)
{
    map<long,tagWorldInfo>::iterator it = m_WorldInfoSetup.find(lNum);
    if(it != m_WorldInfoSetup.end())
    {
        return (*it).second.lStateLvl != 0;
    }
    return false;
}

void CGame::AddWorldInfoToMsg(CMessage& msg,string &strCdkey)
{
    msg.Add( (short)GetGame()->GetWorldNum() );	//当前有效的worldserver数
    std::map<long,tagWorldInfo>::iterator it = m_listWorldInfo.begin();
    for(;it != m_listWorldInfo.end();it++)
    {
        //判断这个服务器是否是开启状态和是否是不排队的CDKEY
        if( WorldServerIsOpenState(it->first))
        {
            msg.Add( (*it).second.lStateLvl );
        }
        else
        {
            msg.Add( (long)0 );
        }
        msg.Add( (char*)((*it).second.strName.c_str()) );
    }
}

bool CGame::IsExitWorld(const char* strWorldName)
{
    if(strWorldName == NULL)
        return false;

    if( GetWorldIDByName(strWorldName) != -1 )
        return true;
    return false;
}

bool CGame::SendMsg2World(CMessage* pMsg,long lWorldServerID)
{
    if(!pMsg)
        return false;

    pMsg->SendToWorldNumID(lWorldServerID);
    return true;
}

void CGame::WsPlayerNumIntoStringFoot(char *pString)
{
    itWorldCdkeyList itWCList = s_listCdkey.begin();
    for (; s_listCdkey.end() != itWCList; ++itWCList)
    {
        map<long,tagWorldInfo>::iterator iteWsInfo = m_WorldInfoSetup.find(itWCList->first);
        if (m_WorldInfoSetup.end() != iteWsInfo)
        {
            char szTmp[256] = {0};
            sprintf(szTmp, "\r\n世界服务器[Name：%s][ID：%d]，当前人数[%d]", iteWsInfo->second.strName.c_str(), itWCList->first, (LONG)itWCList->second.size());
            strcat(pString, szTmp);
        }
    }
}

long CGame::GetLoginWorldPlayerNumByWorldID(long lID)
{
    itWorldCdkeyList itWCList = s_listCdkey.find(lID);
    if(itWCList != s_listCdkey.end())
    {
        return (long)(*itWCList).second.size();
    }
    return -1;
}
long  CGame::GetLoginWorldPlayerNumByWorldName(const char* strWorldName)
{
    long lWorlNumber = GetWorldIDByName(strWorldName);
    if(lWorlNumber != -1)
        return GetLoginWorldPlayerNumByWorldID(lWorlNumber);
    return -1;
}

bool CGame::AddCdkey(const char *szCdkey, long lWorldNumber)
{
    assert(NULL != szCdkey);
    if(0 == strlen(szCdkey))
        return false;
    itWorldCdkeyList itWCList = s_listCdkey.find(lWorldNumber);
    if(itWCList == s_listCdkey.end())
        return false;
    WorldCdkeyList& TemptWorldCdkeyList = (*itWCList).second;

    itCdkeyList iter;
    for(iter=TemptWorldCdkeyList.begin();iter!=TemptWorldCdkeyList.end();iter++)
    {
        if(strcmp(szCdkey,(*iter).c_str()) == 0)
        {
            return false;
        }
    }

    TemptWorldCdkeyList.push_back(szCdkey);

    //统计服务器状态
    long lCurNum = (long)TemptWorldCdkeyList.size();
    OnChangeStateLvl(lWorldNumber,lCurNum);

    return true;
}

VOID CGame::SendOnlineInfoToAs(VOID)
{
    if(0 == m_AccountClient.GetASSocket())
        return;

    WorldCdkeyList listCdkey;

    for (itWorldCdkeyList ite = s_listCdkey.begin(); s_listCdkey.end() != ite; ++ite)
    {
        for (itCdkeyList iteCdkey = ite->second.begin(); ite->second.end() != iteCdkey; ++iteCdkey)
        {
            listCdkey.push_back(*iteCdkey);
        }
    }

    CMessage msg(MSG_O2A_LOGIN_SendOnlineInfo);
    DBWriteSet dbWriteSet;
    msg.GetDBWriteSet(dbWriteSet);

    dbWriteSet.AddToByteArray((LONG)listCdkey.size());
    for (itCdkeyList iteCdkey = listCdkey.begin(); listCdkey.end() != iteCdkey; ++iteCdkey)
    {
        dbWriteSet.AddToByteArray((*iteCdkey).c_str());
    }

    msg.SendToAS();

    GetLoginManager().ClearFcm();
}

VOID CGame::DelWsCdKey(long lWorldNumber)
{
    itWorldCdkeyList itWCList = s_listCdkey.find(lWorldNumber);
    if(s_listCdkey.end() == itWCList)
        return;
    WorldCdkeyList& TemptWolrdCdkeyList = (*itWCList).second;
    itCdkeyList iter;
    for(iter=TemptWolrdCdkeyList.begin();iter!=TemptWolrdCdkeyList.end();iter++)
    {
        //ClearLoginCdkey(iter->c_str());
    }

    TemptWolrdCdkeyList.clear();
    OnChangeStateLvl((*itWCList).first,0);
}

void CGame::ClearCDKey(const char *szCdkey)
{
    itWorldCdkeyList itWCList = s_listCdkey.begin();
    for(;itWCList != s_listCdkey.end();itWCList++)
    {
        WorldCdkeyList& TemptWolrdCdkeyList = (*itWCList).second;
        itCdkeyList iter;
        for(iter=TemptWolrdCdkeyList.begin();iter!=TemptWolrdCdkeyList.end();iter++)
        {
            if(strcmp(szCdkey,(*iter).c_str()) ==0)
            {
                TemptWolrdCdkeyList.erase(iter);
                long lCurNum = (long)TemptWolrdCdkeyList.size();
                OnChangeStateLvl((*itWCList).first,lCurNum);
                ClearLoginCdkey(szCdkey);
                return;
            }
        }
    }
}

long CGame::GetWorldSateLvl(long lPlayerNum)
{
    int i = 1;
    for(i = 1;i <= 3;i++)
    {
        if( lPlayerNum < m_StateLvl[i] )
        {
            break;
        }
    }
    if(i > 3)
        i = 3;
    return i;
}


void CGame::OnChangeStateLvl(long lWorldNum,long lPlayerNum)
{
    long lStateLvl = GetWorldSateLvl(lPlayerNum);

    map<long,tagWorldInfo>::iterator it = m_listWorldInfo.find(lWorldNum);
    if(it != m_listWorldInfo.end())
        (*it).second.lStateLvl = lStateLvl;
}

void CGame::ChangeAllWorldSate()
{
    std::map<long,tagWorldInfo>::iterator it = m_listWorldInfo.begin();
    for(;it != m_listWorldInfo.end();it++)
    {
        itWorldCdkeyList itWCList = s_listCdkey.find((*it).first);
        if(itWCList != s_listCdkey.end())
        {
            WorldCdkeyList& TemptWorldCdkeyList = (*itWCList).second;
            long lStateLvl = GetWorldSateLvl((long)TemptWorldCdkeyList.size());
            (*it).second.lStateLvl = lStateLvl;
        }
    }
}
void CGame::ClearCDKeyByWorldServerID(long lID)
{
    itWorldCdkeyList itWCList = s_listCdkey.find(lID);
    if(itWCList != s_listCdkey.end())
    {
        WorldCdkeyList& TemptWolrdCdkeyList = (*itWCList).second;
        TemptWolrdCdkeyList.clear();
    }
}

int CGame::FindCdkey(const char *szCdkey)
{
    itWorldCdkeyList itWCList = s_listCdkey.begin();
    for(;itWCList != s_listCdkey.end();itWCList++)
    {
        WorldCdkeyList& TemptWolrdCdkeyList = (*itWCList).second;
        itCdkeyList iter;
        for(iter=TemptWolrdCdkeyList.begin();iter!=TemptWolrdCdkeyList.end();iter++)
        {
            if(strcmp(szCdkey,(*iter).c_str()) ==0)
            {
                return (*itWCList).first;
            }
        }
    }
    return -1;
}

long CGame::GetLoginWorldCdkeyNumbers()
{
    long lNumbers = 0;
    itWorldCdkeyList itWCList = s_listCdkey.begin();
    for(;itWCList != s_listCdkey.end();itWCList++)
    {
        lNumbers += (long)(*itWCList).second.size();
    }
    return lNumbers;
}

void CGame::AddSocketCdkey(long lSocketID, const char* szCdkey)
{
    m_mapLoginSocket[lSocketID] = szCdkey;
}

void CGame::RemoveSocketCdkey(long lSocketID)
{
    map<long, string>::iterator itor = m_mapLoginSocket.find(lSocketID);
    if(itor != m_mapLoginSocket.end())
        m_mapLoginSocket.erase(lSocketID);
}

void CGame::ClearSocketCdkey(const char* szCdkey)
{
    map<long, string>::iterator itor = m_mapLoginSocket.begin();
    while(itor!=m_mapLoginSocket.end())
    {
        if(strcmp(szCdkey, (*itor).second.c_str())==0)
        {
            m_mapLoginSocket.erase(itor++);
        }
        else
        {
            ++itor;
        }
    }
}

bool CGame::IsInLoginServer(const char* szCdkey)
{
    map<long, string>::iterator itor = m_mapLoginSocket.begin();
    for(; itor!=m_mapLoginSocket.end(); ++itor)
    {
        if(strcmp(szCdkey, (*itor).second.c_str())==0)
        {
            return true;
        }
    }
    return false;
}

// 踢出一个用户,如果用户处于登陆状态,那么直接断掉并发消息给对应的WorldServer去清除;
// 如果已经进入游戏,那么发消息给对应的WorldServer去清除.
bool CGame::KickOut(const char *szCdkey)
{
    //是否已经有玩家使用该CDKEY
    //这里查找正在连接LOGINSERVER的玩家
    if( GetGame()->GetLoginCdkeyWorldServer(szCdkey)!= NULL )
    {
        //断掉相应的客户端
        long lSocketID = CMessage::GetSocketByCdkey(szCdkey);
        if(lSocketID)
        {
            GetGame()->GetNetServer_Client()->DisConn(lSocketID);
            GetLoginManager().OnClientNetClose(szCdkey, FALSE);
        }
    }

    int nWordNum = FindCdkey(szCdkey);
    if(nWordNum != -1)
    {
        //##发送消息给World Server，告诉World Server有同一帐号尝试登陆。
        CMessage msgToWorldServer( MSG_L2W_LOG_REPEAT_LOGIN );
        msgToWorldServer.Add( (char*)(szCdkey) );
        GetGame() -> SendMsg2World( &msgToWorldServer, nWordNum);
        return true;
    }
    return false;
}
void CGame::KickOut(LONG lSocket)
{
    GetGame()->GetNetServer_Client()->DisConn(lSocket);
}

//设置Cdkey登陆的worldServer;
void CGame::SetLoginCdkeyWorldServer(const char* szCdkey,const char* strWorldServer)
{
    if(szCdkey == NULL)	return ;

    m_LoginCdkeyWorld[szCdkey] = pair<string, long>(strWorldServer, GetWorldIDByName(strWorldServer));
}

//得到Cdkey登陆的WorldServer;
const char* CGame::GetLoginCdkeyWorldServer(const char* szCdkey)
{
    if(NULL != szCdkey)
    {
        map<string, pair<string, LONG>>::iterator it = m_LoginCdkeyWorld.find(szCdkey);
        if( it != m_LoginCdkeyWorld.end() )
        {
            return (*it).second.first.c_str();
        }
    }
    return NULL;
}

LONG CGame::GetLoginCdkeyWorldServerID(const char* szCdkey)
{
    if(NULL != szCdkey)
    {
        map<string, pair<string, LONG>>::iterator it = m_LoginCdkeyWorld.find(szCdkey);
        if( it != m_LoginCdkeyWorld.end() )
        {
            return (*it).second.second;
        }
    }
    return -1;
}

//清楚登陆过程中的Cdkey
void CGame::ClearLoginCdkey(const char* szCdkey)
{
    m_LoginCdkeyWorld.erase(szCdkey);
}

//loginserver向worldserver请求玩家基本信息 
bool CGame::L2W_PlayerBase_Send(const char *wsName,const char *szCdkey)
{
    if(wsName == NULL || szCdkey == NULL)
        return false;

    //cdkey错误
    if(strcmp(szCdkey,"") == 0)
        return false;

    //判断某个WorldServer是否是正常工作
    long lWorldeNumber = GetWorldIDByName(wsName);
    if(lWorldeNumber != -1)
    {
        //是开状态或者是不排队的Cdkey
        if( WorldServerIsOpenState(lWorldeNumber))
        {
            CMessage msg(MSG_L2W_LOG_QUEST_PLAYERBASELIST);
            msg.Add((char*)szCdkey);
            msg.SendToWorldNumID(lWorldeNumber);

#ifdef SHOW_LOGIN_MSG_20090817
            if(bOutInfo)
            {
                Log4c::Trace(ROOT_MODULE,FormatText("LOGIN_GAME_19", wsName, lWorldeNumber, CMessage::GetWorldIDBySocket(lWorldeNumber), szCdkey));//向WS发送基本信息请求，WS【NAME：%s，ID：%d, SocketID：%s】，玩家【Account：%s】
            }
#endif
            return true;
        }
    }
    return false;
}

//loginserver向worldserver发送删除角色的请求
void CGame::L2W_DeleteRole_Send(const char *szWSName,const char *szCdkey,CGUID& PlayerID,DWORD dwClientIP)
{
    if (szWSName==NULL)
        return;

    //cdkey错误
    if(strcmp(szCdkey,"") == 0)
        return;

    long lWorldeNumber = GetWorldIDByName(szWSName);
    if(lWorldeNumber != -1)
    {
        CMessage msg(MSG_L2W_LOG_DELETEROLE);
        msg.Add(const_cast<char*>(szCdkey));
        msg.Add(PlayerID);
        msg.Add((DWORD) dwClientIP );
        msg.SendToWorldNumID(lWorldeNumber);
    }
}

void CGame::L2W_RestoreRole_Send(LPCSTR szWSName, LPCSTR szCdkey, const CGUID& playerID)
{
    _ASSERT(szWSName);_ASSERT(szCdkey);

    if(szWSName == NULL || NULL == szCdkey)
        return;

    long lWorldNumber = this->GetWorldIDByName(szWSName);
    if(lWorldNumber != -1)
    {
        CMessage msg(MSG_L2W_LOG_RESTOREROLE);
        msg.Add(const_cast<char*>(szCdkey));
        msg.Add(playerID);

        msg.SendToWorldNumID(lWorldNumber);
    }
}

//loginserver向worldserver发送创建角色的请求
void CGame::L2W_CreateRole_Send(const char *szWSName,const char *szCdkey,CMessage *pMsg)
{
    if (szWSName==NULL)
        return;

    long lWorldNumber = this->GetWorldIDByName(szWSName);
    if(lWorldNumber != -1)
    {
        pMsg->SetType(MSG_L2W_LOG_CREATEROLE);
        pMsg->Add(const_cast<char*>(szCdkey));	
        pMsg->SendToWorldNumID(lWorldNumber);
    }
}

//loginserver向worldserver请求玩家的详细信息
void CGame::L2W_QuestDetail_Send(const char *szWSName,const char *szCdkey,CGUID& PlayerID,DWORD dwClientIP, DWORD dwSignCode)
{
    if (szWSName==NULL)
        return;

    //cdkey错误
    if(strcmp(szCdkey,"") == 0)
        return;

    long lWorldNumber = this->GetWorldIDByName(szWSName);
    if(lWorldNumber != -1)
    {
        CMessage msg(MSG_L2W_LOG_QUEST_PLAYERDATA);
        msg.Add(dwSignCode);
        msg.Add(PlayerID);
        msg.Add(const_cast<char*>(szCdkey));

        //计费需
        msg.Add(dwClientIP);

        msg.SendToWorldNumID(lWorldNumber);
    }
}

void CGame::UpdateDisplayWorldInfo()
{
    AppFrame::ResetList("list");
    //更新控件里的服务器状态
    std::map<long,tagWorldInfo>::iterator it = m_listWorldInfo.begin();
    for(;it != m_listWorldInfo.end();it++)
    { 
        string strName = (*it).second.strName;
        strName += "[";
        long lFlag = (*it).second.lStateLvl;
        map<long,tagWorldInfo>::iterator itSetup =
            m_WorldInfoSetup.find( (*it).first );			
        if(itSetup != m_WorldInfoSetup.end())
        {
            if(lFlag != 0 && (*itSetup).second.lStateLvl == 0)
            {
                lFlag = -1;
            }
        }
        if(lFlag == 0)
        {
            strName += "断开";
        }
        else if(lFlag == -1)
        {
            strName += "关闭";
        }
        else
        {
            strName += "开启";
        }
        strName += "]";
        AppFrame::AddToList("list", strName);
    }

}

//更新WorldServer的状态信息到客户端
void CGame::UpdateWorldInfoToAllClient()
{
    map<string,pair<string, LONG>>::iterator it = m_LoginCdkeyWorld.begin();
    for(;it != m_LoginCdkeyWorld.end();it++)
    {
        if( (*it).second.first == "" )
        {
            string strCdkey = (*it).first;
            CMessage msg(MSG_L2C_LOG_UPDATEWORLDSERVERINFO);
            AddWorldInfoToMsg( msg,strCdkey);
            msg.SendToClient(strCdkey.c_str());
        }
    }
}

unsigned CGame::GetCdkeyCount()
{
    unsigned cnt = 0;
    for(std::map<long,WorldCdkeyList>::iterator it = GetGame()->s_listCdkey.begin();
        it != GetGame()->s_listCdkey.end(); it++)
    {
        cnt += (long)it->second.size();
    }
    return cnt;
}


void CGame::OnNetWorldClose(long lWorldID)
{
    const char* strWorldName = GetGame()->GetWorldNameByID(lWorldID);
    if( strWorldName )
    {
        Log4c::Warn(ROOT_MODULE,"LOGIN_GAME_20",strWorldName);//WorldServer [%s] lost!
        ClearCDKeyByWorldServerID(lWorldID);	//清除和该worldserver相关的玩家的CDKEY
        SetWsReadyState(lWorldID, FALSE);
        m_mapLoginSocket.clear();
    }
    GetGame()->DelWorld(lWorldID);
}
void CGame::OnNetClientClose(const char* szCdkey, long lSocketID)
{
    if(szCdkey==NULL)	return;


    // 进入游戏
    BOOL bInWs = FALSE;
    if( FindCdkey(szCdkey) != -1 )
    {
        bInWs = TRUE;
    }
    else	// 直接退出
    {
        // 发消息到WorldServer踢掉对应的玩家LoginList信息
        long lWorldID = GetWorldIDByName(GetLoginCdkeyWorldServer(szCdkey));
        if (lWorldID != -1)
        {
            // 发消息到WorldServer踢掉对应的玩家LoginList信息
            CMessage msgToWorldServer(MSG_L2W_LOG_KICKPLAYER);
            msgToWorldServer.Add( (char*)(szCdkey) );
            SendMsg2World( &msgToWorldServer, lWorldID);
        }

        // 退出日志（更新）
        ClearCDKey(szCdkey);	
    }

    GetLoginManager().OnClientNetClose(szCdkey, bInWs);
    ClearLoginCdkey(szCdkey);
    RemoveSocketCdkey(lSocketID);
}

int CGame::ReadyToEnter(const char *szCdkey, DWORD dwIP, long lSockID, const char *szWSName)
{
    if( !szCdkey || !dwIP || !lSockID || !szWSName ) return 1;

    LONG lWsID = FindCdkey(szCdkey);
    if(-1 == lWsID)
    {
        if( GetGame()->GetLoginCdkeyWorldServer(szCdkey)!= NULL || IsInLoginServer(szCdkey))
        {
            //断掉相应的客户端
            long lSocketID = CMessage::GetSocketByCdkey(szCdkey);
            if(lSocketID)
                GetGame()->GetNetServer_Client()->DisConn(lSocketID);
            GetGame()->ClearLoginCdkey(szCdkey);

            CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
            msg.Add((LONG)LOGIN_CHECKOUT);
            msg.SendToClient(lSockID);

            return 1;
        }
        else if( strlen(szWSName) ) 	
        {
            //! 非法选择，断掉客户端
            GetGame()->GetNetServer_Client()->DisConn(lSockID);
            return 1;
        }
    }
    else
    {
        // 发消息到WorldServer踢掉对应的玩家LoginList信息
        CMessage msgToWorldServer( MSG_L2W_LOG_REPEAT_LOGIN );
        msgToWorldServer.Add( (char*)(szCdkey) );
        GetGame() -> SendMsg2World( &msgToWorldServer, lWsID);

        CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
        msg.Add((LONG)LOGIN_CHECKOUT);
        msg.SendToClient(lSockID);
        GetLoginManager().OnClientNetClose(szCdkey, FALSE);

        GetGame()->ClearLoginCdkey(szCdkey);

        return 1;
    }

    //给客户端套接字关联Cdkey
    SetLoginCdkeyWorldServer(szCdkey,"");
    AddSocketCdkey(lSockID, szCdkey);

    // 返回client
    CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
    msg.Add((LONG)LOGIN_LOGINSERVEROK);
    GetGame()->AddWorldInfoToMsg(msg,string(szCdkey));
    msg.SendToClient(lSockID);

    return 0;
}

BOOL CGame::EnterToSelect(const char *pCdkey, const char *pWSName, DWORD dwIP, long lSockID)
{
    LONG lWsID = FindCdkey(pCdkey);
    if(-1 != lWsID)
    {
        // 发消息到WorldServer踢掉对应的玩家LoginList信息
        CMessage msgToWorldServer( MSG_L2W_LOG_REPEAT_LOGIN );
        msgToWorldServer.Add( (char*)(pCdkey) );
        GetGame() -> SendMsg2World( &msgToWorldServer, lWsID);

        CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
        msg.Add((LONG)LOGIN_CHECKOUT);
        msg.SendToClient(lSockID);
        GetLoginManager().OnClientNetClose(pCdkey, FALSE);
        GetGame()->ClearLoginCdkey(pCdkey);

        return FALSE;
    }

    if( L2W_PlayerBase_Send(pWSName,pCdkey) )//向worldserver请求该客户的基本信息
    {
        SetLoginCdkeyWorldServer(pCdkey,pWSName);//设置该连接所选择的worldserver名
        GetCSdoaFcm().UserOnline(pCdkey, dwIP);
        return TRUE;
    }
    else
    {
        // 返回client
        CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
        msg.Add((LONG)LOGIN_LOGINSERVEROK);
        GetGame()->AddWorldInfoToMsg(msg,string(pCdkey));
        msg.SendToClient(lSockID);
    }
    return FALSE;
}

int CGame::EnterGame( const char *szCdkey, DWORD dwIP, long lSockID, const char *szWSName, bool bLimit, bool bMobileUser )
{
    if( !szCdkey || !dwIP || !lSockID || !szWSName ) return 1;

    // 如果已经在游戏中,那么踢掉该帐号
    if( !strlen(szWSName) && GetGame()->KickOut(szCdkey) ) 	{
        //##如果帐号在游戏中,通知玩家,暂时无法登陆.
        CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
        msg.Add( (LONG)(LOGIN_CHECKOUT) );
        msg.SendToClient(lSockID);

        GetGame()->ClearLoginCdkey(szCdkey);
        return 1;
    }

    //给客户端套接字关联Cdkey
    SetLoginCdkeyWorldServer(szCdkey,"");

    // 直接进入选人界面（退到选人界面代码）
    if (szWSName[0]!=0)
    {
        if( L2W_PlayerBase_Send(szWSName,szCdkey) )//向worldserver请求该客户的基本信息
        {
            SetLoginCdkeyWorldServer(szCdkey,szWSName);//设置该连接所选择的worldserver名
            GetCSdoaFcm().UserOnline(szCdkey, dwIP);
        }
    }
    else
    {
        // 返回client
        CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
        msg.Add((LONG)LOGIN_LOGINSERVEROK);
        GetGame()->AddWorldInfoToMsg(msg,string(szCdkey));
        msg.SendToClient(lSockID);
    }

    return 0;
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
        AppFrame::ToggleMenus();

        if (ret)
            while(1)
            {
                // 结束线程
                if (AppFrame::IsExit() || !GetGame()->MainLoop() )
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

bool ClearOnlineUserDatabase() {

    _ConnectionPtr cn;
    if(!CMyAdoBase::CreateCn(cn))
    {
        CMyAdoBase::ReleaseCn(cn);
        return false;
    }
    if(!CMyAdoBase::OpenCn(cn))
    {
        CMyAdoBase::ReleaseCn(cn);
        return false;
    }
    if(!CMyAdoBase::ExecuteCn("DELETE FROM online_user" ,cn))
    {
        CMyAdoBase::ReleaseCn(cn);
        return false;
    }
    CMyAdoBase::ReleaseCn(cn);
    return true;
}

