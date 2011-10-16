#pragma  once

const DWORD NOTIFY_COLOR_RED=0xffff0000;
const DWORD NOTIFY_COLOR_YELLOW=0xffffff00;
const DWORD NOTIFY_COLOR_WHITE=0xffffffff;

//字符串类型ID
enum eStrTypeID
{	
	IDS_WS_SKILL		= 1,
	IDS_WS_UNISTRING	= 2,
	IDS_WGS_SYSMAIL		= 3,

	IDS_GS_GOODS		= 20,

	IDS_WS_RGNMANAGER		= 200,
	IDS_WS_LOGMESSAGE		= 203,
	IDS_WS_SERVERMESSAGE	= 204,
	IDS_WS_GAME				= 205,
	IDS_GS_RGNMANAGER		= 207,
	IDS_GS_FUNCTION			= 208,
	IDS_GS_SERVERMESSAGE	= 209,
	IDS_GS_RGNFUNC			= 210,
	IDS_GS_SCRIPT			= 211,
	IDS_DBS_SERVERMESSAGE	= 212,
	IDS_DBS_DBINCREMENTLOG	= 213,
	IDS_DBS_RSDUPRGN		= 214,
	IDS_DBS_RSGOODS			= 215,
	IDS_DBS_RSPLAYER		= 216,
	IDS_DBS_SAVETHREAD		= 217,
	IDS_GS_NOBILITYTITILE	= 218,
	IDS_GS_OTHERINFO		= 219,

	IDS_WS_DOCCUSYS_ERROR	= 600,
	IDS_WS_QUESTSYS_ERROR	= 610,
	IDS_WS_MONSTERFILE_ERROR= 620,
	IDS_GS_REGION_ERROR		= 630,

	IDS_WS_REPORT_INFO		= 810,
	IDS_CF_BUILDLIST_CHAR	= 1000,
	IDS_CF_MONSTERLIST_CHAR = 1001,
	IDS_CF_GOODSLIST_CHAR	= 1002,
	IDS_CF_SYSBOARDCASTF_CHAR = 1003,

};
//字符串资源ID自定义
//技能类型字符串资源ID
enum
{	STR_WS_SKILL_FIRST				= 1,
	STR_WS_SKILL_SECOND,
	STR_WS_SKILL_THIRD,
	STR_WS_SKILL_FOURTH,
	STR_WS_SKILL_FIFTH,
	STR_WS_SKILL_SIXTH,
	STR_WS_SKILL_SEVENTH,
	STR_WS_SKILL_EIGHTH,
	STR_WS_SKILL_NINETH,
	STR_WS_SKILL_TENTH,
	STR_WS_SKILL_ELEVENTH,
	STR_WS_SKILL_TWELFTH,
	STR_WS_SKILL_THIRTEENTH,
	STR_WS_SKILL_FOURTEENTH,
	STR_WS_SKILL_FIFTEENTH,
};
//系统邮件
enum
{
	STR_WS_SYSMAIL_FIRST			= 1,
	STR_WS_SYSMAIL_SECOND			= 2,
	STR_WS_SYSMAIL_THIRD			= 3,
	STR_WS_SYSMAIL_FOURTH			= 4,
	STR_WS_SYSMAIL_FIFTH			= 5,
	STR_WS_SYSMAIL_SIXTH			= 6,
	STR_WS_SYSMAIL_SEVENTH			= 7,
	STR_WS_SYSMAIL_EIGHTH			= 8,
	STR_WS_SYSMAIL_NINE				= 9,
	STR_WS_SYSMAIL_TEN				= 10,
	STR_WS_SYSMAIL_ELEVEN			= 11,
	STR_WS_SYSMAIL_TWELVE			= 12,
};

//统一字符串类型字符串资源ID
enum
{
	STR_WS_UNISTRING_FIRST		   =  1,
	STR_WS_UNISTRING_SECOND		   =  2,
};


// WS场景管理器字符串资源ID
enum
{
	STR_WS_RGNMANAGER_CREATERGN = 0, 
	STR_WS_RGNMANAGER_DELETERGN	= 1, 

};

// WS登录消息字符串资源ID
enum
{
	STR_WS_LOGMESSAGE_QEUSTPROPERTYCHARDELED = 0,
	STR_WS_LOGMESSAGE_QUESTPROPERTYNOCREATEORLOADBASE	= 1,
	STR_WS_LOGMESSAGE_CHARINGAME						= 2,
	STR_WS_LOGMESSAGE_LOGINGSINVALID					= 3,
	STR_WS_LOGMESSAGE_LOGINWSQUEST						= 4,
	STR_WS_LOGMESSAGE_CHARSTATEERR						= 5,
	STR_WS_LOGMESSAGE_LOADBASEFAIL						= 6,
	STR_WS_LOGMESSAGE_SENDLOADPROPERTYTOGS				= 7,
	STR_WS_LOGMESSAGE_LOGOUTCHARNOTINWS					= 8,
	STR_WS_LOGMESSAGE_LOGOUTCHARINWSONLY				= 9,
	STR_WS_LOGMESSAGE_LOGOUTERRNOCHAR					= 10,
	STR_WS_LOGMESSAGE_LOGOUTOK							= 11,
	STR_WS_LOGMESSAGE_REPEATLOGINTOGS					= 12,
	STR_WS_LOGMESSAGE_REPEATLOGINQUIT					= 13,
	STR_WS_LOGMESSAGE_DELCHARNOCHAROBJ					= 14,
	STR_WS_LOGMESSAGE_DELCHARINVALID					= 15,
	STR_WS_LOGMESSAGE_DELCHAROK							= 16,
	STR_WS_LOGMESSAGE_RESTORCHARNOCHAROBJ				= 17,
	STR_WS_LOGMESSAGE_RESTORECHARVALID					= 18,
	STR_WS_LOGMESSAGE_CREATECHARNOCOUNTRY				= 19,
	STR_WS_LOGMESSAGE_SENDCREATETODBS					= 20,
};
// WS系统消息字符串资源ID
enum
{
	STR_WS_SERVERMESSAGE_DBSTOWSSETUPDATA = 0,	
	STR_WS_SERVERMESSAGE_DBSRECONNETIONINFO				= 1,
	STR_WS_SERVERMESSAGE_GSRECONNSENDDATABEGIN			= 2,
	STR_WS_SERVERMESSAGE_GSRECONNLOGINPLAYERNUM			= 3,
	STR_WS_SERVERMESSAGE_GSRECONNSENTLOGINPLAYERNUM		= 4,
	STR_WS_SERVERMESSAGE_GSRECONNSENDRGNDATA			= 5,
	STR_WS_SERVERMESSAGE_GSRECONNSETUPDATA				= 6,
	STR_WS_SERVERMESSAGE_CHANGERGNNORGNONGS				= 7,
	STR_WS_SERVERMESSAGE_CHANGERGNGSNOTRUN				= 8,
	STR_WS_SERVERMESSAGE_CHANGERGNNOPLAYERONWS			= 9,
	STR_WS_SERVERMESSAGE_WSRECONNGSNOPLAYERONWS			= 10,
};														
// WS游戏模块字符串资源ID								
enum													
{														
	STR_WS_GAME_LOADPACKOK = 0,							
	STR_WS_GAME_LOGSERVERINVALID							= 1,					
	STR_WS_GAME_CANTCONNWS									= 2,
	STR_WS_GAME_CANTCONNLS									= 3,
	STR_WS_GAME_CONNDBSOK									= 4,
	STR_WS_GAME_CONNLSOK									= 5,
	STR_WS_GAME_COSTSERVERINVALID							= 6,
	STR_WS_GAME_LOADCARDSERVERFAIL							= 7,
	STR_WS_GAME_RELOADSCRIPTOK								= 8,
	STR_WS_GAME_RELOADSCRIPTFAIL							= 9,
	STR_WS_GAME_LOADGOLBALSTRINGOK							= 10,
	STR_WS_GAME_LOADGOLBALSTRINGFAIL						= 11,
	STR_WS_GAME_LOADEXPLISTOK								= 12,
	STR_WS_GAME_LOADEXPLISTFAIL								= 13,
	STR_WS_GAME_LOADLEVELLISTOK								= 14,
	STR_WS_GAME_LOADLEVELLISTFAIL							= 15,
	STR_WS_GAME_LOADOCCUDEFLISTOK							= 16,
	STR_WS_GAME_LOADOCCUDEFLISTFAIL							= 17,
	STR_WS_GAME_LOADFACELISTOK								= 18,
	STR_WS_GAME_LOADFACELISTFAIL							= 19,
	STR_WS_GAME_LOADGOODSLISTOK								= 20,
	STR_WS_GAME_LOADGOODSLISTFAIL							= 21,	
	STR_WS_GAME_LOADCOMQUIPMENTLISTOK						= 22,
	STR_WS_GAME_LOADCOMQUIPMENTLISTFAIL						= 23,
	STR_WS_GAME_LOADHOLELISTOK								= 24,
	STR_WS_GAME_LOADHOLELISTFAIL							= 25,
	STR_WS_GAME_LOADGTREATEAREALISTOK						= 26,
	STR_WS_GAME_LOADTREATEAREALISTFAIL						= 27,
	STR_WS_GAME_LOADMONSTERLISTOK							= 28,
	STR_WS_GAME_LOADMONSTERLISTFAIL							= 29,
	STR_WS_GAME_LOADMGOODSLISTOK							= 30,
	STR_WS_GAME_LOADMGOODSLISTFAIL							= 31,
	STR_WS_GAME_LOADQUESTGOODSLISTOK						= 32,
	STR_WS_GAME_LOADQUESTGOODSLISTFAIL						= 33,
	STR_WS_GAME_LOADMSKILLLISTOK							= 34,
	STR_WS_GAME_LOADMSKILLLISTFAIL							= 35,
	STR_WS_GAME_LOADTREADELISTOK							= 36,
	STR_WS_GAME_LOADTREADELISTFAIL							= 37,
	STR_WS_GAME_LOADSKILLLISTOK								= 38,
	STR_WS_GAME_LOADSKILLLISTFAIL							= 39,
	STR_WS_GAME_LOADGLOBALPARAMLISTOK						= 40,
	STR_WS_GAME_LOADGLOBALPARAMLISTFAIL						= 41,
	STR_WS_GAME_LOADEXPGOODSLISTOK							= 42,
	STR_WS_GAME_LOADEXPGOODSLISTFAIL						= 43,
	STR_WS_GAME_LOADLOGLISTOK								= 44,
	STR_WS_GAME_LOADLOGLISTFAIL								= 45,
	STR_WS_GAME_LOADGMLISTOK								= 46,
	STR_WS_GAME_LOADGMLISTFAIL								= 47,
	STR_WS_GAME_LOADSCRIPTLISTOK							= 48,
	STR_WS_GAME_LOADSCRIPTLISTFAIL							= 49,
	STR_WS_GAME_LOADRGNLEVELLISTOK							= 50,
	STR_WS_GAME_LOADRGNLEVELLISTFAIL						= 51,
	STR_WS_GAME_LOADHITLEVELLISTOK							= 52,
	STR_WS_GAME_LOADHITLEVELLISTFAIL						= 53,
	STR_WS_GAME_LOADBROADCASTLISTOK							= 54,
	STR_WS_GAME_LOADCITYWARTIMEOK							= 55,
	STR_WS_GAME_LOADINVALIDWORDOK							= 56,
	STR_WS_GAME_LOADFACTIONPARAMOK							= 57,
	STR_WS_GAME_LOADVILWARPARAMTIMEOK						= 58,
	STR_WS_GAME_LOADCITYWARPARAMOK							= 59,
	STR_WS_GAME_LOADFACTIONWARTIMEOK						= 60,
	STR_WS_GAME_LOADQUESTPARAMOK							= 61,
	STR_WS_GAME_LOADQUESTPARAMFAIL							= 62,
	STR_WS_GAME_LOADCOUNTRYPARAMOK							= 63,
	STR_WS_GAME_LOADCOUNTRYPARAMFAIL						= 64,
	STR_WS_GAME_LOADINCGOODSOK								= 65,
	STR_WS_GAME_LOADINCGOODSFAIL							= 66,
	STR_WS_GAME_LOADCOUNTRYCONTRIBUTEGOODSOK				= 67,
	STR_WS_GAME_LOADCOUNTRYCONTRIBUTEGOODSFAIL				= 68,
	STR_WS_GAME_LOADDOCCUOK									= 69,
	STR_WS_GAME_LOADDOCCUFAIL								= 70,
	STR_WS_GAME_CONNECTLSOK									= 71,
	STR_WS_GAME_CONNECTDBSOK								= 72,
	STR_WS_GAME_CONNECTLOGSOK								= 73,
	STR_WS_GAME_CONNECTCOSTSOK								= 74,
	STR_WS_GAME_RECONNECTLSOK								= 75,
	STR_WS_GAME_RECONNECTDBSOK								= 76,
	STR_WS_GAME_RECONNECTLOGSOK								= 77,
	STR_WS_GAME_NETSERVERINITOK								= 78,
	STR_WS_GAME_WSFORGSINITOK								= 79,
	STR_WS_GAME_LOADWSSETUPOK								= 80,
	STR_WS_GAME_LOADRGNOK									= 81,
	STR_WS_GAME_LOADRGNFAIL									= 82,
	STR_WS_GAME_LOADRGNSETUPOK								= 83,
	STR_WS_GAME_MONSTERNUMINWS								= 84,
	STR_WS_GAME_GSLOST										= 85,
	STR_WS_GAME_UNKOWNGSLOST								= 86,
	STR_WS_GAME_QUESTPROPERTYBYRGNFAIL						= 87,
	STR_WS_GAME_QUESTPROPERTYBYGSFAIL						= 88,
	STR_WS_GAME_LOADOCCUATTRLISTOK							= 89,
	STR_WS_GAME_LOADOCCUATTRLISTFAIL						= 90,
	STR_WS_GAME_LOADSYSMAILOK								= 91,
	STR_WS_GAME_LOADSYSMAILFAIL								= 92,
	STR_WS_GAME_PM_OK										= 93,	// 载入玩家管理配置...ok!
	STR_WS_GAME_PM_FAILED									= 94,	// 载入玩家管理配置...failed!
	STR_WS_GAME_PET_OK										= 95,	// 载入宠物配置...ok!
	STR_WS_GAME_PET_FAILED									= 96,	// 载入宠物配置...failed!
	STR_WS_GAME_OCCUEXP_OK									= 97,	// 载入宠物配置...ok!
	STR_WS_GAME_OCCUEXP_FAILED								= 98,	// 载入宠物配置...failed!
    STR_WS_GAME_ANTI_CHEAT_OK                               = 99,   // 载入防外挂配置...ok!
	STR_WS_GAME_ANTY_CHEAT_FAILED                           = 100,  // 载入防外挂配置...failed!
	STR_WS_GAME_LOADCREDITSYSTEM_OK							= 101,
	STR_WS_GAME_LOADCREDITSYSTEM_FAILED						= 102,
	STR_WS_GAME_LOAD_COSMETOLOGY_OK							= 103,
	STR_WS_GAME_LOAD_COSMETOLOGY_FAILED						= 104,
	STR_WS_GAME_LOAD_QUESTINDEX_OK							= 105,
	STR_WS_GAME_LOAD_QUESTINDEX_FAILED						= 106,
	STR_WS_GAME_LOAD_CLOTTERYXML_OK							= 107,
	STR_WS_GAME_LOAD_CLOTTERYXML_FAILED						= 108,
};															
// GS场景管理器字符串资源ID									
enum														
{															
	STR_GS_RGNMANAGER_MAXCREATEPRGNNUM = 0,
	STR_GS_RGNMANAGER_MAXCREATETRGNNUM						= 1,
	STR_GS_RGNMANAGER_CREATEPRGN							= 2,
	STR_GS_RGNMANAGER_INITPRGN								= 3,
	STR_GS_RGNMANAGER_CREATEPHRGN							= 4,
	STR_GS_RGNMANAGER_INITPHRGN								= 5,
	STR_GS_RGNMANAGER_CREATETRGN							= 6,
	STR_GS_RGNMANAGER_INITTRGN								= 7,
	STR_GS_RGNMANAGER_ERRRGNOWNERNOTFOUND					= 8,
	STR_GS_RGNMANAGER_ERRRGNCREATEDFAILED					= 9,
	STR_GS_RGNMANAGER_DELPRGNOK								= 10,
	STR_GS_RGNMANAGER_DELPHRGNOK							= 11,
	STR_GS_RGNMANAGER_DELTRGNOK								= 12,
	STR_GS_RGNMANAGER_DELPHRGNGOODSOK						= 13,
};															 
// GS脚本函数字符串资源ID									
enum														
{															
	STR_GS_FUNCTION_PARSEPARAMNOARRAYBLOCK = 0,				
	STR_GS_FUNCTION_ERRARRAYSUFFIXOUTBOUND					= 1,
	STR_GS_FUNCTION_ITERATEPARAMERR							= 2,
	STR_GS_FUNCTION_DEFINEVARNOTINT							= 3,
	STR_GS_FUNCTION_DEFINEVARNOTSTR							= 4,
	STR_GS_FUNCTION_SCRIPTFILENODEFLINE						= 5,
	STR_GS_FUNCTION_SETATTRFIRSTVARISSTRING					= 6,
	STR_GS_FUNCTION_CHANGEATTRFIRSTVARISSTRING				= 7,
	STR_GS_FUNCTION_SETMSATTRFIRSTVARISSTRING				= 8,
	STR_GS_FUNCTION_GETMSATTRFIRSTVARISSTRING				= 9,
	STR_GS_FUNCTION_CHANGEMSATTRFIRSTVARISSTRING			= 10,
	STR_GS_FUNCTION_SETGOODSATTREXFIRSTVARISSTRING			= 11,
	STR_GS_FUNCTION_GETGOODSATTREXFIRSTVARISSTRING			= 12,
	STR_GS_FUNCTION_CHANGEGOODSATTREXFIRSTVARISSTRING		= 13,
};
// GS系统消息字符串资源ID
enum
{
	STR_GS_SERVERMESSAGE_INITGLOBALSTRING = 0,
	STR_GS_SERVERMESSAGE_INITGOODSLIST						= 1,
	STR_GS_SERVERMESSAGE_INITSUITSETUP						= 2,
	STR_GS_SERVERMESSAGE_INITHOLESETUP						= 3,
	STR_GS_SERVERMESSAGE_INITCOMMERCESETUP					= 4,
	STR_GS_SERVERMESSAGE_INITMONSTERLIST					= 5,
	STR_GS_SERVERMESSAGE_INITGOODSDROPLIST					= 6,
	STR_GS_SERVERMESSAGE_INITPLAYERLIST						= 7,
	STR_GS_SERVERMESSAGE_INITPLAYEREXPLIST					= 8,
	STR_GS_SERVERMESSAGE_INITSKILLLIST						= 9,
	STR_GS_SERVERMESSAGE_INITSKILLSTUDYOCCULIST				= 10,
	STR_GS_SERVERMESSAGE_INITSKILLSTUDYLIST					= 11,
	STR_GS_SERVERMESSAGE_INITCOMMERCEGOODSLIST				= 12,
	STR_GS_SERVERMESSAGE_INITINCGOODSLIST					= 13,
	STR_GS_SERVERMESSAGE_INITCOUNTRYCONTRIBUTEGOODSDROP		= 14,
	STR_GS_SERVERMESSAGE_INITGLOBALPARAMLIST				= 15,
	STR_GS_SERVERMESSAGE_LOGSERVERPARAMLIST					= 16,
	STR_GS_SERVERMESSAGE_SCRIPTFUNCLIST						= 17,
	STR_GS_SERVERMESSAGE_SCRIPTVARLIST						= 18,
	STR_GS_SERVERMESSAGE_GLOBALSCRIPTVARLIST				= 19,
	STR_GS_SERVERMESSAGE_GETQUESTLIST						= 20,
	STR_GS_SERVERMESSAGE_GETDOCCULIST						= 21,
	STR_GS_SERVERMESSAGE_NOTFINDPHRGNTEMPLATEID				= 22,
	STR_GS_SERVERMESSAGE_INITPHRGNOK						= 23,
	STR_GS_SERVERMESSAGE_GETGMLISTOK						= 24,
	STR_GS_SERVERMESSAGE_STARTNORMALRGNOK					= 25,
	STR_GS_SERVERMESSAGE_STARTPRGNOK						= 26,
	STR_GS_SERVERMESSAGE_STARTPHRGNOK						= 27,
	STR_GS_SERVERMESSAGE_STARTTRGNOK						= 28,
	STR_GS_SERVERMESSAGE_ADDPROXYRGNOK						= 29,
	STR_GS_SERVERMESSAGE_RELOADRGNSETUPOK					= 30,
	STR_GS_SERVERMESSAGE_RELOADRGNSETUPEXOK					= 31,
	STR_GS_SERVERMESSAGE_INITRGNLEVELLISTOK					= 32,
	STR_GS_SERVERMESSAGE_INITHITLEVELLISTOK					= 33,
	STR_GS_SERVERMESSAGE_INITSERVERNETFAILED				= 34,
	STR_GS_SERVERMESSAGE_GAMEWORLDMONSTERNUM				= 35,
	STR_GS_SERVERMESSAGE_INITFACELIST						= 36,
	STR_GS_SERVERMESSAGE_LOADCOUNTRYPARAMOK					= 37,
	STR_GS_SERVERMESSAGE_LOADCOUNTRYINFOOK					= 38,
	STR_GS_SERVERMESSAGE_LOADDUPRGNOK						= 39,
	STR_GS_SERVERMESSAGE_LOADCITYWARINFOOK					= 40,
	STR_GS_SERVERMESSAGE_LOADVILWARINFOOK					= 41,
	STR_GS_SERVERMESSAGE_LOADGOODSEXSETUPOK					= 42,
	STR_GS_SERVERMESSAGE_LOADGOODSEXSETUPFAILED				= 43,
	STR_GS_SERVERMESSAGE_LOADDEPOTSETUPOK					= 44,
	STR_GS_SERVERMESSAGE_LOADRECVFRIENDSYSSETUPOK			= 45,
	STR_GS_SERVERMESSAGE_RECONNECTINFOFROMWSOK				= 46,
	STR_GS_SERVERMESSAGE_RECONNECTINFOFROMDBSOK				= 47,
	STR_GS_SERVERMESSAGE_SENDSERVERSAVEPLAYERDATASIZE		= 48,
	STR_GS_SERVERMESSAGE_CREATEPHRGNOK						= 49,
	STR_GS_SERVERMESSAGE_SYSMAIL							= 50,
	STR_GS_SERVERMESSAGE_CANTFINDRELEASEGOODSOWNER			= 51,
	STR_GS_SERVERMESSAGE_CANTFINDRELEASEGOODS				= 52,
	STR_GS_SERVERMESSAGE_LEVELCANTUSEAREASPEAK				= 53,
	STR_GS_SERVERMESSAGE_CANTSPEAKWITHOUTSOMETING			= 54,
	STR_GS_SERVERMESSAGE_CANTSPEAKWITHOUTMONEY				= 55,
	STR_GS_SERVERMESSAGE_CANTMMMWITHOUTPLAYERONLINE			= 56,
	STR_GS_SERVERMESSAGE_PWDERRJOIN							= 67,
	STR_GS_SERVERMESSAGE_COSMETOLOTY						= 73,
    STR_GS_SERVERMESSAGE_RELIVETIMER                        = 74,
    STR_GS_SERVERMESSAGE_RETURNCITYTIMER                    = 75,
};															
// GS副本脚本字符串资源ID									
enum														
{															
	STR_GS_RGNFUNC_ADDRGNVARERR = 0,
	STR_GS_RGNFUNC_SETRGNVARERR								= 1,
	STR_GS_RGNFUNC_DELRGNVARERR								= 2,
	STR_GS_RGNFUNC_GETRGNVARERR								= 3,
	STR_GS_RGNFUNC_VIEWRGNVARERR							= 4,
	STR_GS_RGNFUNC_VIEWRGNDATA								= 5,
	STR_GS_RGNFUNC_COUNTRYINFO								= 6,
};															
// GS脚本文件字符串资源ID									
enum														
{															
	STR_GS_SCRIPT_STRVARBEENSETINTVALUE = 0,				
	STR_GS_SCRIPT_RETVALUEERR								= 1,
	STR_GS_SCRIPT_GUIDVARBEENSETINTVALUE					= 2,
	STR_GS_SCRIPT_CANTOPERWITHGUIDVAR						= 3,
};															
// DBS系统消息字符串资源ID									
enum														
{															
	STR_DBS_SERVERMESSAGE_WSCONNECTOK		= 0,			
	STR_DBS_SERVERMESSAGE_WSCONNECTFAIL		= 1,			
};

// DBS商品增量日志字符串资源ID
enum
{
	STR_DBS_GOODSINCLOG_LOADALLFAIL = 0,
	STR_DBS_GOODSINCLOG_LOADFAIL			= 1,
	STR_DBS_GOODSINCLOG_SAVEROLLBACK		= 2,
	STR_DBS_GOODSINCLOG_SAVEFAIL			= 3,
	STR_DBS_GOODSINCLOG_SAVEERROR			= 4,
};											
// DBS个人房屋字符串资源ID					
enum
{
	STR_DBS_DUPRGN_CNPTRNULL = 0,
	STR_DBS_DUPRGN_FINDDUPRGNPROPERTYERR		= 1,
	STR_DBS_DUPRGN_LOADRGNOBJERR				= 2,
	STR_DBS_DUPRGN_LOADRGNOK					= 3,
	STR_DBS_DUPRGN_LOADRGNERR					= 4,
	STR_DBS_DUPRGN_LOADONERGNOK					= 5,
	STR_DBS_DUPRGN_LOADONERGNFAIL				= 6,
	STR_DBS_DUPRGN_DELDUPRGNPROPERTYERR			= 7,
	STR_DBS_DUPRGN_SAVERGNOBJOK					= 8,
	STR_DBS_DUPRGN_SAVERGNOBJFAIL				= 9,
	STR_DBS_DUPRGN_DELRGNOBJOK					= 10,
	STR_DBS_DUPRGN_DELRGNOBJFAIL				= 11,
	STR_DBS_DUPRGN_FINDRGNGOODSERR				= 12,
	STR_DBS_DUPRGN_LOADRGNGOODSOK				= 13,
	STR_DBS_DUPRGN_LOADRGNGOODSFAIL				= 14,
	STR_DBS_DUPRGN_SAVERGNGOODSOK				= 15,
	STR_DBS_DUPRGN_SAVERGNGOODSFAIL				= 16,
	STR_DBS_DUPRGN_QUESTRGNGUIDOK				= 17,
	STR_DBS_DUPRGN_QUESTRGNGUIDFAIL				= 18,
};												
// DBS物品字符串资源ID							
enum
{
	STR_DBS_GOODS_CNPTRNULL = 0,
	STR_DBS_GOODS_SAVEGOODSERR					= 1,
	STR_DBS_GOODS_DELEQUIPMENTERR				= 2,
	STR_DBS_GOODS_DELORIPACKERR					= 3,
	STR_DBS_GOODS_DELDEPOTERR					= 4,
	STR_DBS_GOODS_DELSUBPACKERR					= 5,
	STR_DBS_GOODS_CLEARALLFAIL					= 6,
	STR_DBS_GOODS_LOADALLOCERR					= 7,
	STR_DBS_GOODS_LOADERR						= 8,
};												
// DBS玩家字符串资源ID							
enum											
{												
	STR_DBS_PLAYER_CNPTRNULL = 0,				
	STR_DBS_PLAYER_OPENBASEERR								= 1,	
	STR_DBS_PLAYER_UPDATELOGOUTTIMEERR						= 2,
	STR_DBS_PLAYER_DELPROPERTYERR							= 3,
	STR_DBS_PLAYER_CHECKNAMEERR								= 4,
	STR_DBS_PLAYER_NAMEIS									= 5,
	STR_DBS_PLAYER_X										= 6,
	STR_DBS_PLAYER_N										= 7,
	STR_DBS_PLAYER_GETNAMEBYIDERR							= 8,
	STR_DBS_PLAYER_GETIDERR									= 9,
	STR_DBS_PLAYER_GETCDKEYERR								= 10,
	STR_DBS_PLAYER_CREATEBASEROLLBACK						= 11,
	STR_DBS_PLAYER_CREATEBASESTARTTRANSIMITFAIL				= 12,
	STR_DBS_PLAYER_CREATEPROPERTYERR						= 13,
	STR_DBS_PLAYER_LOADPROPERTYERR							= 14,
	STR_DBS_PLAYER_SAVEBASEOK								= 15,
	STR_DBS_PLAYER_SAVEBASEERR								= 16,
	STR_DBS_PLAYER_SAVEBASEROLLBACK							= 17,
	STR_DBS_PLAYER_SAVEBASESTARTTRANSIMITFAIL				= 18,
	STR_DBS_PLAYER_SAVEPROPERTYOK							= 19,
	STR_DBS_PLAYER_SAVEPROPERTYERR							= 20,
	STR_DBS_PLAYER_SAVEPROPERTYROLLBACK						= 21,
	STR_DBS_PLAYER_SAVEPROPERTYSTARTTRANSIMITFAIL			= 22,
	STR_DBS_PLAYER_SAVEALLOK								= 23,
	STR_DBS_PLAYER_SAVEALLERR								= 24,
	STR_DBS_PLAYER_SAVEROLLBACK								= 25,
	STR_DBS_PLAYER_SAVEALLTARTTRANSIMITFAIL					= 26,
	STR_DBS_PLAYER_SAVESKILLHOTKEYERR						= 27,
	STR_DBS_PLAYER_SAVESKILLHOTKEYROLLBACK					= 28,
	STR_DBS_PLAYER_SAVESKILLHOTKEYSTARTTRANSIMITFAIL		= 29,
	STR_DBS_PLAYER_SAVESKILLHOTKEYINFOERR					= 30,
	STR_DBS_PLAYER_SAVESKILLHOTKEYINFOOK					= 31,
	STR_DBS_PLAYER_SAVESKILLHOTKEYINFOFAIL					= 32,
	STR_DBS_PLAYER_LOADSKILLOK								= 33,
	STR_DBS_PLAYER_LOADSKILLFAIL							= 34,
	STR_DBS_PLAYER_SAVESKILLOK								= 35,
	STR_DBS_PLAYER_SAVESKILLFAIL							= 36,
	STR_DBS_PLAYER_SAVESKILLROLLBACK						= 37,
	STR_DBS_PLAYER_SAVESKILLSTARTTRANSIMITFAIL				= 38,
	STR_DBS_PLAYER_SAVESKILLINFOFAIL						= 39,
	STR_DBS_PLAYER_THREADSAVESTATEFAIL						= 40,
	STR_DBS_PLAYER_THREADSAVESTATEROLLBACK					= 41,
	STR_DBS_PLAYER_THREADSAVESTATESTARTTRANSIMITFAIL		= 42,
	STR_DBS_PLAYER_THREADSAVESTATEOK						= 43,
	STR_DBS_PLAYER_SAVESTATEFAIL							= 44,
	STR_DBS_PLAYER_SAVESTATEOK								= 45,
	STR_DBS_PLAYER_LOADSTATEFAIL							= 46,
	STR_DBS_PLAYER_LOADSTATEOK								= 47,
	STR_DBS_PLAYER_THREADSAVESCRIPTFAIL						= 48,
	STR_DBS_PLAYER_THREADSAVESCRIPTROLLBACK					= 49,
	STR_DBS_PLAYER_THREADSAVESCRIPTSTARTTRANSIMITFAIL		= 50,
	STR_DBS_PLAYER_SAVESCRIPTFAIL							= 51,
	STR_DBS_PLAYER_LOADSCRIPTOK								= 52,
	STR_DBS_PLAYER_LOADSCRIPTFAIL							= 53,
	STR_DBS_PLAYER_LOADNULLPLAYER							= 54,
	STR_DBS_PLAYER_LOADPLAYERNUMFAIL						= 55,
	STR_DBS_PLAYER_GETDELTIMEFAIL							= 56,
	STR_DBS_PLAYER_GETLASTLOGOUTTIMEFAIL					= 57,
	STR_DBS_PLAYER_RESTORECHARROLLBACK						= 58,
	STR_DBS_PLAYER_RESTORECHARSTARTTRANSIMITFAIL			= 59,
	STR_DBS_PLAYER_RESTORECHARERR							= 60,
	STR_DBS_PLAYER_CHECKGUIDBYCDKEYFAIL						= 61,
	STR_DBS_PLAYER_LOADQUESTOK								= 62,
	STR_DBS_PLAYER_LOADQWUESTERR							= 63,
	STR_DBS_PLAYER_SAVEQUESTOK								= 64,
	STR_DBS_PLAYER_SAVEQUESTFAIL							= 65,
	STR_DBS_PLAYER_LOADQUESTINFOOK							= 66,
	STR_DBS_PLAYER_LOADQUESTINFOERR							= 67,
	STR_DBS_PLAYER_SAVEDOCCUOK								= 68,
	STR_DBS_PLAYER_SAVEDOCCUFAIL							= 69,
	STR_DBS_PLAYER_LOADLINKMANFAIL							= 70,
	STR_DBS_PLAYER_CLEARLINKMANERR							= 71,
	STR_DBS_PLAYER_SAVELINKMANERR							= 72,
	STR_DBS_PLAYER_UPDATELINKMANOK							= 73,
	STR_DBS_PLAYER_THREADSAVEINCLOGFAIL						= 74,
	STR_DBS_PLAYER_SAVEINCLOGROLLBACK						= 75,
	STR_DBS_PLAYER_SAVEINCLOGSTARTTRANSIMITFAIL				= 76,
	STR_DBS_PLAYER_SAVEINCLOGFAIL							= 77,
	STR_DBS_PLAYER_LOADINCLOGOK								= 78,
	STR_DBS_PLAYER_LOADINCLOGFAIL							= 79,
	STR_DBS_PLAYER_PRELOADBASEFAIL							= 80,
	STR_DBS_PLAYER_THREADSAVEMERQUESTFAIL					= 81,
	STR_DBS_PLAYER_THREADSAVEMERQUESTROLLBACK				= 82,
	STR_DBS_PLAYER_THREADSAVEMERQUESTSTARTTRANSIMITFAIL		= 83,
	STR_DBS_PLAYER_THREADSAVEMERQUESTOK						= 84,
	STR_DBS_PLAYER_LOADMERQUESTOK							= 85,
	STR_DBS_PLAYER_LOADMERQUESTFAIL							= 86,
};
// DBS保存线程字符串资源ID
enum														
{															
	STR_DBS_SAVETHREAD_SAVE		= 0,
	STR_DBS_SAVETHREAD_LOAD									= 1,
	STR_DBS_SAVETHREAD_OK									= 2,
	STR_DBS_SAVETHREAD_FAIL									= 3,
	STR_DBS_SAVETHREAD_PLAYERNAME							= 4,
	STR_DBS_SAVETHREAD_BASEDATA								= 5,
	STR_DBS_SAVETHREAD_PROPERTY								= 6,
	STR_DBS_SAVETHREAD_SKILLHOTKEY							= 7,
	STR_DBS_SAVETHREAD_SKILL								= 8,
	STR_DBS_SAVETHREAD_STATE								= 9,
	STR_DBS_SAVETHREAD_SCRIPT								= 10,
	STR_DBS_SAVETHREAD_QUEST								= 11,
	STR_DBS_SAVETHREAD_DOCCU								= 12,
	STR_DBS_SAVETHREAD_LINKMAN								= 13,
	STR_DBS_SAVETHREAD_GOODSINCLOG							= 14,
	STR_DBS_SAVETHREAD_MERQUEST								= 15,
	STR_DBS_SAVETHREAD_GOODS								= 16,
};															
															
//! 队伍字符串ID
static const LONG TEAM_STRING_TYPEID	= 16;
//! 队伍字符串枚举
enum eTeamStringID
{															
	eTSID_Warning						= 0,	//! 警告"/>	
	eTSID_AimHadTeam					= 10,	//! 对方已有队伍。"/>
	eTSID_TeamIsFull					= 20,	//! 队伍人数已满。"/>
	eTSID_PleaseWait					= 30,	//! 你已经发出了邀请，请耐心等待对方答复。"/>
	eTSID_OutTime						= 40,	//! 操作已过期。"/>
	eTSID_YouHadTeam					= 50,	//! 你已经加入了其他队伍"/>
	eTSID_CreateFailedAndTautology		= 60,	//! 队伍创建失败，请稍候重试。"/>
	eTSID_JoinFailedAndTautology		= 70,	//! 加入队伍失败，请稍候重试。"/>
	eTSID_AimRefuse						= 80,	//! 对方拒绝加入队伍。"/>
	eTSID_AimNotOnlineOrBeing			= 90,	//! 目标不在线或不存在"/>
	eTSID_Busyness						= 100,	//! 目标不在线或不存在"/>
	eTSID_NotOpenTeam					= 110,	//! 对方没有开组"/>
	eTSID_AimIsDead						= 120,	//! 对方是死亡状态"/>
	eTSID_OperatorNotMaster				= 130,	//! 你不是队长，不能这样操作"/>
	eTSID_AimHideName					= 140,	//! 该玩家目前正处于“匿名者”状态，无法执行组队操作！"/>
};

//! <FileTable  typeid="10" name="IDS_WS_GOODSEX"   file="textresource/GoodsEx.xml"/>
static const LONG GOODSEX_STRING_TYPEID = 10;
//! 物品扩展字符串枚举
//! 这部分句子太长，因此用文件名方式表示出文本位置
enum eGoodsExStringID
{
	WS_GoodsEx_10						= 10,//! 警告"/>
	WS_GoodsEx_20						= 20,//! 初始化分解配方出错！未找到原始名为：“"/>
	WS_GoodsEx_30						= 30,//! ”的物品"/>
	WS_GoodsEx_40						= 40,//! 初始化分解配方出错！原始名为：“"/>
	WS_GoodsEx_50						= 50,//! ”的物品既配置了“物品品质”属性，又配置了“最大叠加数属性”！"/>
	WS_GoodsEx_60						= 60,//! 获取“分解物品配置”成功！"/>

	WS_GoodsEx_70						= 70,//! 初始化合成配方出错！未找到原始名为：“"/>
	WS_GoodsEx_80						= 80,//! 初始化合成配方出错！原始名为：“"/>
	WS_GoodsEx_90						= 90,//! 合成“分解物品配置”成功！"/>

	WS_GoodsEx_100						= 100,//! 初始化分装备升级配置出错！没有找到原始名为“"/>
	WS_GoodsEx_110						= 110,//! ”的冲等材料"/>
	WS_GoodsEx_120						= 120,//! 获取“装备升级配置”成功！"/>

	WS_GoodsEx_130						= 130,//! 初始化分冲等辅助道具配置出错！没有找到原始名为“"/>
	WS_GoodsEx_140						= 140,//! 获取“特殊物品配置”成功！"/>

	WS_GoodsEx_150						= 150,//! 初始化魔化材料合成配置出错！没有找到原始名为“"/>
	WS_GoodsEx_160						= 160,//! ”的魔化材料"/>
	WS_GoodsEx_170						= 170,//! 初始化魔化材料合成配置出错！原始名为：“"/>
	WS_GoodsEx_180						= 180,//! ”的物品既配置了“物品品质”属性，又配置了“最大叠加数属性”！"/>
	WS_GoodsEx_190						= 190,//! 获取“魔化材料合成几率配置”成功！"/>
	WS_GoodsEx_200						= 200,//! 获取“魔化配置”成功！"/>

	WS_GoodsEx_210						= 210,//! 载入物品扩展设置...成功！"/>
	WS_GoodsEx_220						= 220,//! 载入物品扩展设置...失败！！！"/>	
};


//! 王峰：公用字符串
enum ENUM_IDS_PUBLIC
{
	eIDS_ChineseZero					= 0,	//! 

	eIDS_ChineseFu						= 10,	//! 
	eIDS_ChineseGe,								//! 
	eIDS_ChineseShi,							//! 
	eIDS_ChineseBai,							//! 
	eIDS_ChineseQian,							//! 
	eIDS_ChineseWan,							//! 
	eIDS_ChineseYi,								//! 

	eIDS_GoodsHasLocked					= 101,	//! "您已经选择了该物品，或该物品正在其它地方被操作！"
	eIDS_InputGoodsNum,							//! "请输入物品数："
	eIDS_GoodsDecordError,						//! "物品解码失败！"

	eIDS_JuBaoError                      = 104, //! 您相同的举报信息时间过短,举报信息尚在处理,谢谢
	eIDS_JuBaoSucceed,                          //! 谢谢您提供的信息,我们会在第一时间对您的信息进行查证后处理
	eIDS_GameName                      = 120,  //! 星尘传说
	eIDS_SDOALogin                     = 121,  //! "[盛大通行证用户登陆]"/>    
	eIDS_SDOALoginError                = 122,  //! "登陆圈圈错误:%d"
	eIDS_ErrorInfo                     = 123,  //! "错误信息(%d)""
	eIDS_OpenUrl                       = 124,  //! "打开网站:%s\n"
	eIDS_SDOAInfoTOLogin               = 125,  //! "登陆成功后才可用!"
	eIDS_ServerError                   = 126,  //! ""服务器未响应,是否继续等待?"
	eIDS_TimeOver                      = 127,  //! "处理超时,请稍后再试!"/>"
	eIDS_QueeInfo                      = 128,  //! 正在进入游戏......\n队列第%d位,估计用时%d秒."/>
	eIDS_LoginError                    = 129,  //! "连接服务器失败,请稍后重试!"
	eIDS_LoadLater                     = 130,  //! "请稍后登陆！"
	eIDS_InputAllInfo                  = 131,  //! "请填写完整的信息！"

	// 通用错误
	eIDS_CantFindAim					= 10001,//! "没有可操作的物品！"
	eIDS_SrcIsLock,								//! "该物品在其它地方被锁定了！"
	eIDS_SessionLapse,							//! "对话距离过远或已经失效！"
	eIDS_ErrOperation,							//! "错误的操作值！"
	eIDS_ErrAssistant,							//! "辅助材料错误！"
	eIDS_CantOperateAim,						//! "目标不能作此操作！"
	eIDS_DelAssistantFailed,					//! "删除辅助材料失败！"
	eIDS_NoEnoughSpace,							//! "你需要空出 %d 格背包空间！"
	eIDS_NotEnoughMoney,						//! "没有足够的金钱完成操作！"
	eIDS_ErrStuff,								//! "材料错误！"
	eIDS_NumError,								//! "材料数量错误！"
	eIDS_NoEnoughSpace2,						//! "您的背包空间不足！"
	eIDS_NotUpgrade,                            //! 没有强化无法进行附魔操作
	eIDS_UpgradeLow,                            //! 该物品已达到物品强化等级所允许的附魔等级上限，进一步附魔需要先提升强化等级
};


//! 王峰：魔方字符串枚举
enum ENUM_IDS_MAGIC_BOX
{
	eIDS_CantHandleTheGoodsOnWear		= 1,	//! "穿戴在身上的装备不可执行合成操作！"
	eIDS_NormalComposeSucceed,					//! "物品合成成功！"
	eIDS_ComposeConfigError,					//! "合成配方错误！"
	eIDS_ComposeConfigNotExist,					//! "合成配方不存在！"
	eIDS_CantComposeTheGoodsWasFreezed,			//! "不能合成冻结的物品！"
	eIDS_AreYouSureComposeTheseGoods,			//! "您确定要按此配方合成物品吗！"
	eIDS_ComposeConfigErrorCantHandle,			//! "配方错误，不能操作！"
	eIDS_InputTheGoodsFirst,					//! "请先放入需要操作的物品！"

	eIDS_AreYouSureComposeMagicGoods	= 100,	//! "您确定要炼制此魔化材料吗！"
	eIDS_MagicComposeSucceed,					//! "魔化材料合成成功！"
	eIDS_MagicComposeFailed,					//! "魔化材料合成失败！"
	eIDS_MagicGoodsTopLevel,					//! "魔化材料已经达到最高合成等级！"

	eIDS_AreYouSureUnBind				= 200,	//! "您确定要 解绑此物品吗！"
	eIDS_AreYouSureBind,						//! "您确定要 绑定此物品吗！"
	eIDS_CantBind,								//! "此物品无法进行绑定！"
	eIDS_CantUnBind,							//! "此物品无法进行解绑！"
	eIDS_BindSucceed,							//! "物品绑定成功！"
	eIDS_UnBindSucceed,							//! "物品解绑成功！"
	eIDS_CantBindTheFreezeGoods,				//! "不能对冻结物品进行 绑定／解绑 操作！"

	eIDS_AreYouSureUnFreeze				= 300,	//! "您确定要 解冻此物品吗！"
	eIDS_AreYouSureFreeze,						//! "您确定要 冻结此物品吗！"
	eIDS_FreezeSucceed,							//! "物品冻结成功！"
	eIDS_UnFreezeSucceed,						//! "物品解冻成功！"
	eIDS_FreezeNeedBindFirst,					//! "请先绑定物品后才可进行冻结操作！"
};


//! 王峰：物品分解 字符串
enum ENUM_IDS_DECOMPOSE
{
	eIDS_AreYouSureDecompose			= 1,	//! "你确定要执行分解操作吗？"
	eIDS_WithoutDecomposeGoods,					//! "没有可分解的物品！"
	eIDS_GoodsCantDecompose,					//! "该物品不可分解！"
	eIDS_GoodsOnWearCantDecompose,				//! "穿戴在身上的装备不可执行分解操作！"
	eIDS_DecomposePreviwFailed,					//! "获取分解预览结果失败！"
	eIDS_DecomposeSucceed,						//! "物品分解成功！"
	eIDS_AreYouSureDecomposeWithCard,			//! "该物品镶嵌有卡片，如果继续分解将导致卡片消失。是否继续？"
	eIDS_CantDecomposeFreezeGoods,				//! "该物品已被冻结，解冻后才可分解！"
	eIDS_DecomposeDesc_1,						//! "！"
};


//! 王峰：装备魔化 字符串
enum ENUM_IDS_EQUIP_MAGIC
{
	eIDS_AreYouSureMagicEquip			= 1,	//! "您确定需要魔化吗？"
	eIDS_NotEnoughMoneyMagicEquip,				//! "您没有足够的金钱来完成魔化！"

	eIDS_InputMagicStuff,						//! "请先放入魔化所需材料！"
	eIDS_InputMagicEquip,						//! "请先放入需要魔化的装备！"

	eIDS_GetMagicStuffCfgError,					//! "获取相应的魔化材料失败！"
	eIDS_GetMagicCfgError,						//! "获取魔化配置失败！"
	eIDS_NotEnoughQualityForMagic,				//! "该装备的品质太低无法进行魔化！"
	eIDS_EquipHasBeenTopMagicLevel,				//! "该装备已经魔化到最高等级，无法再继续魔化了！"
	eIDS_MagicEquipNeed80Levle,					//! "必须要 80 级以上的装备才能进行魔化！"
	eIDS_CantMagicTheEquipOnWear,				//! "穿戴在身上的装备不可直接执行魔化操作！"
	eIDS_EquipCantMagic,						//! "该物品无法进行魔化！"
	eIDS_NeedMoreMagicStuff,					//! "您还需要增加 %d 个 %s！"

	eIDS_NOT_PutDown         = 15,                      //! "该道具无法被放置"
	eIDS_Select_Comp         = 16,                       //! "选择需要附魔的栏位"
	eIDS_NeedMagicEquip      = 17,                      //! "请放入需要附魔的装备!"
	eIDS_NeedMagicStuff      = 18,                      //! "请在该栏位放入%s!"
	eIDS_NeedXiangXingStuff  = 19,                    //! "放入元素相性晶石!"
	eDIS_SureEnchant         = 20,                      //! "确认进行该栏位元素属性的强化？"
	eDIS_SureChangeEnchant   = 21,                     //! "确认进行该栏位元素属性的重置？"
	eDIS_SureChangeXiangXing = 22,                     //! "确认进行该栏位元素相性的更换？"
	eDIS_Select_Enchant      = 30,                     //! "请选择要操作的栏位"

	eDIS_MainGoodsError      = 50,                     //! "主材料错误"
	eDIS_HelpGoodsError      = 51,                     //! "辅助材料错误"
	eDIS_Not_Enchant         = 52,                     //! "目标不能做此操作"
	eDIS_NumError            = 53,                     //! "操作数量错误"
    eDIS_ChangeStuffCanPutDown = 54,                     //! "你只能放入元素属性转换宝石"
    eDIS_MagicStuffNotEnogh = 55,                     //!

	eIDS_MagicPreview,							//! "属性预览"

	eIDS_InputMagicStuffFirst			= 100,	//! "请先放入相应等级的 魔化材料！"
	eIDS_MagicEquipSucceed,						//! "装备已经魔化成功！"

	eIDS_InputMagicEquipFirst,					//! "请将需要魔化的装备放入装备容器！"
	eIDS_InputOddsStuff,						//! "请将 [COLOR FF0000]%d 颗 %s[/COLOR] 放入上方 材料容器。"

	eIDS_InputEnchantStuff   = 104,             //! "请放入一种元素晶石"   
	eIDS_EnchantNote_30      = 105,             //! "开启晶槽需要装备魔化度30%"
	eIDS_EnchantNote_60      = 106,             //! "开启晶槽需要装备魔化度60%"
	eIDS_EnchantNote_90      = 107,             //! "开启晶槽需要装备魔化度90%"
	eIDS_NeedEnchantGoods    = 109,             //! "没有放入装备或属性变更道具"

	eIDS_EnchantStuff        = 130,             //! "元素属性的其始位置索引"
};



//! 王峰：装备强化 字符串
enum ENUM_IDS_EQUIP_UPGRADE
{
	eIDS_AreYouSureUpgradeEquip			= 1,	//! "您确定需要强化吗？"
	eIDS_NotEnoughMoneyUpgradeEquip,			//! "您没有足够的金钱来完成强化！"
	eIDS_InputUpgradeStoneFirst,				//! "请先放入强化所需宝石！"
	eIDS_InputUpgradeEquipFirst,				//! "请先放入需要强化的装备！"

	eIDS_EquipHasBeenMaxUpgradeLevel,			//! "该装备无法再进行强化了！"
	eIDS_CantUpgradeTheEquipOnWear,				//! "穿戴在身上的装备不可直接执行强化操作！"
	eIDS_EquipCantUpgrade,						//! "该物品无法进行强化！"
	eIDS_UpgradeStuffError,						//! "该物品不是 %s！"
	eIDS_UpgradeStuffAmountError,				//! "%s 最多只能使用 %d 个！"
	eIDS_NeedNotOddsStone,						//! "强化当前等级的装备不需要使用 辅助宝石！"
	eIDS_TheGoodsIsntOddsStone,					//! "该物品不是 辅助宝石！"
	eIDS_EquipUpgradePreviewError,				//! "装备强化预览失败！"

	eIDS_UpgradePreview,						//! "属性预览"
	eIDS_StoneLvlNotMatching,					//! "宝石等级限制"

	eIDS_UpgradeEquipDesc_1				= 100,	//! "！"
	eIDS_UpgradeEquipDesc_2,					//! "！"
	eIDS_UpgradeEquipDesc_3,					//! "！"
	eIDS_UpgradeEquipDesc_4,					//! "！"
	eIDS_UpgradeEquipDesc_5,					//! "！"
	eIDS_UpgradeEquipDesc_6,					//! "！"
};



//! 王峰：魔化拆解 字符串
enum ENUM_IDS_MAGIC_DECOMPOSE
{
	eIDS_AreYouSureMagicDecompose		= 1,	//! "您确定要拆解材料吗？"
	eIDS_GetMagicDecomposeCfgError,				//! "获取该装备的拆解材料结构 失败！"
	eIDS_GetMagicInfoError,						//! "获取该装备的魔化结构 失败！"
	eIDS_MagicLevelIsZero,						//! "该物品尚未得到品质提升，无法拆解材料！"
	eIDS_CantMagicDecomposeTheEquipOnWear,		//! "穿戴在身上的装备不可执行拆解操作！"
	eIDS_DecomposeMagicStuffNum,				//! "拆解可获得 %d 个 %s！"
	eIDS_NotEnoughMoneyMagicDecompose,			//! "没有足够的金钱完成材料拆解！"
	eIDS_NoMagicEquip,							//! "没有选择拆解材料的装备！"
	eIDS_EquipHasMagicDecomposeOver,			//! "装备已经拆解到最低等级！"
	eIDS_MagicDecomposeSucceed,					//! "装备拆解成功！"
	eIDS_InputMagicDecomposEquipFirst,			//! "请先放入待剥离的 魔化装备！"

	eIDS_PeelPreview,							//! "属性预览"
};




//! 王峰：仓库 字符串
enum ENUM_IDS_DEPOT
{
	eIDS_DepotHasFreezed				= 1,	//! "由于多次输入错误密码，仓库已被冻结，请等待解冻！"
	eIDS_PasswordError,							//! "密码错误，打开失败！"
	eIDS_SetPasswordSucceed,					//! "密码设置成功！"
	eIDS_SetPasswordFailed,						//! "密码设置失败！"

	eIDS_InputSaveAmountOfMoney,				//! "存入 金钱数量"
	eIDS_InputDrawAmountOfMoney,				//! "提取 金钱数量"
	eIDS_InputSaveAmountOfDiamond,				//! "存入 宝石数量"
	eIDS_InputDrawAmountOfDiamond,				//! "提取 宝石数量"

	eIDS_NotEnoughMoneyInPacket,				//! "背包内没有这么多金钱！"
	eIDS_NotEnoughMoneyInDepot,					//! "仓库内没有这么多金钱！"
	eIDS_NotEnoughDiamondInPacket,				//! "背包内没有这么多宝石！"
	eIDS_NotEnoughDiamondInDepot,				//! "仓库内没有这么多宝石！"

	eIDS_AreYouSureBuyExtItem,					//! "您确定要花费 %d 的金钱来购买栏位吗？"
	eIDS_HavntEnoughMoneyBuyExtItem,			//! "金钱不足 %d，无法购买此栏位！"
	eIDS_ExtItemHasSoldOut,						//! "子仓库的扩展栏位已售完！"
	eIDS_CantMoveSubPacketWhenHasGoods,			//! "子背包内装有物品不可移动！"
	eIDS_CantMoveSubPacketWhenOpening,			//! "打开的子背包不可移动，请先关闭！"
	eIDS_ExtItemOnlyCanPutSubPacket,			//! "此栏位只能放置子背包！"
	eIDS_ExtItemHavntBought,					//! "此栏位尚未购买！"
	eIDS_ExtItemCantExchangeNotSubPacket,		//! "非背包物品 不能和背包栏位的背包进行交换！"
	eIDS_Depot_Reserved1,						//! "尚未"
	eIDS_Depot_Reserved2,						//! "购买"

	eIDS_BuyExtItemSucceed,						//! "购买栏位成功！"
	eIDS_BuyExtItemFailed,						//! "购买栏位失败！"

	eIDS_InputOldPWError,						//! "旧密码输入错误！"
	eIDS_InputTwicePWDiffer,					//! "两次输入的密码不一致！"
	eIDS_InputOldPW,							//! "请输入旧密码"
	eIDS_InputNewPW,							//! "请输入新密码"
	eIDS_InputPWAgain,							//! "请再次输入新密码"
	eIDS_InputDepotPW,							//! "请输入银行密码"

	eIDS_MoneyInDepotHasFull,					//! "仓库内装不下这么多的金钱！"
	eIDS_MoneyInPacketHasFull,					//! "背包内装不下这么多的金钱！"
	eIDS_SilverInDepotHasFull,					//! "仓库内装不下这么多的银币！"
	eIDS_SilverInPacketHasFull,					//! "背包内装不下这么多的银币！"
	eIDS_GoodsCantSaveInDepot,					//! "该物品禁止放入仓库！"
	eIDS_PleaseCloseBeforeTidyDepot,			//! "请关闭背包再整理仓库！"
	eIDS_PleaseCloseBeforeTidyPack,			    //! "请关闭仓库再整理背包！"
};



//! 王峰：彩票系统 字符串
enum ENUM_IDS_LOTTERY
{
	eIDS_Lottery_MaxTodayoint				= 1,	//! "%d   【[COLOR FFF600]%s[/COLOR]】"
	eIDS_Lottery_ForceReset,						//! 强制重置

	eIDS_Lottery_Prompt_ResetPrizes			= 10,	//! "%d   【[COLOR FFF600]%s[/COLOR]】"
	eIDS_Lottery_Prompt_ResetConstellations,		//! "%d   【[COLOR FFF600]%s[/COLOR]】"
	eIDS_Lottery_Prompt_AddLotteryCost,				//! "本轮以后进行抽奖操作，都将扣除 %d 枚命运之星"
	eIDS_Lottery_Tip_LotteryCost,					//! "本次抽奖将扣除 %d 枚命运之星"

	eIDS_Lottery_TIP_Roulette				= 20,	//! 轮盘说明

	eIDS_Lottery_Describe					= 100,	//! 规则说明

	eIDS_Lottery_Error						= 1000	//! 错误
};




//! hlc:monster.ini 配置错误
enum
{
	WS_MONSTERLIST_DataSlopOver=1,				//! "momster。ini 文件字符串过长"
	WS_MONSTERLIST_IndexDataSlopOver=2,			//! "索引数据越界"
};

//!物品
enum
{
	GS_GOODS_GOLD			=35,						//金币
	GS_GOODS_SILVER			=36,						//银币
};


//! hlc:任务系统配置错误
enum
{
	WS_QS_OTIOSEAIM=1,						//"输出错误信息：超出步骤目标个数的目标配置."
	WS_QS_OTIOSESTEPGAIN,					//"输出错误信息：超出步骤物品获得个数的物品获得配置"
	WS_QS_OTIOSESTEP,						//"输出错误信息：任务步骤数多于步骤最大限度"
	WS_QS_OTIOSEENCOURAGEMENT,				//"输出错误信息：任务奖励数多于奖励最大限度"
	WS_QS_SYSNAMEERROR,						//"输出错误信息：错误的任务系统配置文件名"
	WS_QS_SYSFILEERROR,						//"输出错误信息：任务系统配置文件出错"
	WS_QS_VOIDONEFILE,						//"输出错误信息：单个任务配置文件不存在"
	WS_QS_ONEFILEERROR,						//"输出错误信息：单个任务配置文件出错"
	WS_QS_QUESTADDERROR,					//"输出错误信息：任务加入任务列表时出错"
};

//! hlc:场景错误
enum
{
	GS_RGN_BringMonster=1,					//编号=%d的怪物没有机会出生！
	GS_RGN_NoMonsterBringPlace,				//无法为Monster “%s” 找到合适的位置！
	GS_RGN_BringCollection,					//编号=%d的采集物没有机会出生！
	GS_RGN_NoCollectionBringPlace,			//无法为Collection “%s” 找到合适的位置！
	GS_RGN_MonsterAddRoad,					//无法为Monster “%s” 添加路径！
	GS_RGN_NoNpcBringPlace,					//无法为npc “%s” 找到合适的位置！
};

//! 王峰：好友系统 字符串
enum ENUM_IDS_FRIENDS
{
	eIDS_AreYouSureAddLinkman			= 1,	//! "您确定要将 %s 加入 %s 列表吗？"
	eIDS_AreYouSureDelLinkman,					//! "您确定要将 %s 从 %s 中删除吗？"
	eIDS_AreYouSureAddFriendToo,				//! "您是否也将 %s 加为好友？"
	eIDS_RequestYouAddFriend,					//! "%s 请求加你为好友！"
	eIDS_RejectYourAddRequest,					//! "%s 拒绝了您的添加请求！"
	eIDS_AreYouSureDelFriendLinkman,			//! "您确定要将 %s 从 %s 中删除吗？\
												//   确认后，将无法再与该玩家进行私聊，只有重新将该玩家加入好友列表后才能继续进行私聊）" 

	eIDS_UnknownLinkmanMap				= 101,	//! " - ？？？"
	eIDS_UnknownLinkmanLevel,					//! "等级 ？？？"
	eIDS_LinkmanLevel,							//! "等级 %d"
	eIDS_UnknownLinkmanOccupation,				//! "职业 ？？？"
	eIDS_PlayerHasDel,							//! "%s - 角色已删除"
	eIDS_PlayerHavntOffline,					//! "%s - 未上线"

	eIDS_NotifyPlayerOnline,					//! "%s：%s 上线了"
	eIDS_NotifyPlayerOffline,					//! "%s：%s 下线了"
	eIDS_NotifyLoverOnline,						//! "[恋人] %s 上线了"
	eIDS_NotifyLoverOffline,					//! "[恋人] %s 下线了"

	eIDS_NotifyPlayerAddYou				= 201,	//! "%s 将您添加到了 %s"
	eIDS_NotifyPlayerDelYou,					//! "%s 将您从 %s 中删除了"
	eIDS_NotifyYouAddPlayer,					//! "%s 已加入到您的 %s"
	eIDS_NotifyYouDelPlayer,					//! "%s 已从您的 %s 中被删除"
	eIDS_NotifyYouMovePlayer,					//! "%s 已从您的 %s 移至 %s"
	eIDS_NotifyPlayerHasAddInOtherGroup,		//! "%s 已被你放入其他组中"
	eIDS_NotifyPlayerHasAddInGroup,				//! "%s 已经在您的 %s 列表内了"
	eIDS_NotifyTheGroupHasMaxPlayers,			//! "您的 %s 分组人数已达上线"
	eIDS_NotifyTheOperatNeedPlayerLogin,		//! "你对 %s 的操作需要他在本服务器上线后才能完成"
	eIDS_NotifyYouHasSendTheRequest,			//! "您已经发送过请求了，请耐心等待。。。"
	eIDS_NotifyYouHasAutoReject,				//! "您自动拒绝了 %s 向您添加好友的请求！"


	eIDS_NotifyPlayerHasHideInfo,				//! "该玩家目前正处于“匿名者”状态，无法执行好友操作！"

	eIDS_CantLeavewordForDelPlayer		= 301,	//! "不能和已经删除的角色 聊天或留言"
	eIDS_CantLeavewordForOfflinePlayer,			//! "不能给不在线的 %s 玩家留言！"
	eIDS_AreYouSureLeaveword,					//! "%s 不在线，您是否要留言？"
	eIDS_NotifyYourLeavewordHasSaved,			//! "你对 %s 的留言已存储"
	eIDS_NotifyYourLeavewordHasOverflow,		//! "你对 %s 的留言已超过限制数量"
	eIDS_PlayerForYourLeavewordHasOverflow,		//! "%s 对你的留言已超过限制数量"
	eIDS_LeavewordTooFast,						//! "留言过于频繁，请稍候再试。。。"
	eIDS_LeavewordFormat,						//! "%d年%d月%d日 %d时%d分%d秒 => %s 留言：%s"
	eIDS_BlackListFlag,							//! "@黑名单 "

	eIDS_InputAddPlayerName				= 401,	//! "输入添加人名字"
	eIDS_NotifyFrendHasDelete           = 402   //! "对方已经将你从好友列表中删除，无法进行私聊"
};


//! 王峰：家族系统 字符串
enum ENUM_IDS_FACTION
{
	eIDS_FAC_FactionPanel				= 1,	//! "家族"
	eIDS_FAC_UnionPanel,						//! "联盟"

	eIDS_FAC_FacWarInfo					= 10,	//! "战争信息"
	eIDS_FAC_BeachheadWarInfo,					//! "据点战信息"
	eIDS_FAC_WarList,							//! "参战名单"

	eIDS_FAC_State_Open					= 101,	//! "开放竞争"
	eIDS_FAC_State_Close,						//! "闭关自守"
	eIDS_FAC_Notify_CreateSucceed,				//! "行会创建成功！"
	eIDS_FAC_Notify_SetChairmanSucceed,			//! "会长职位转让成功！"
	eIDS_FAC_Notify_Disband,					//! "行会 %s 已经解散了！"
	eIDS_FAC_InviteYou,							//! "%s 邀请您加入行会：%s！"
	eIDS_FAC_Notify_MemberHasKickedFromYourFac,	//! "%s 已经被踢出本行会！"
	eIDS_FAC_PlayerAcceptYourInvite,			//! "%s 已经接受了您的入会邀请！"
	eIDS_FAC_NotEnoughMoneyUpgradeEquip,		//! "%s 邀请您加入行会：%s！"
	eIDS_FAC_ThePurviewNeedFacLevel,			//! "该权限需要行会等级达到 %d级 方可开放！"
	eIDS_FAC_Notify_MemberHasKicked,			//! "%s 已经被踢出行会！"
	eIDS_FAC_Notify_MemberHasQuit,				//! "%s 已经退出行会！"
	eIDS_FAC_UploadFacIconSucceed,				//! "行会徽标 上传成功！"
	eIDS_FAC_SetPlacardSucceed,					//! "行会公告 已经修改成功！"
	eIDS_FAC_SetMemoSucceed,					//! "家族备忘录 已经修改成功！"
	eIDS_FAC_SetJobPurviewSucceed,				//! "权限修改成功！"
	eIDS_FAC_FunHasTopLevel,					//! "已达到最高等级"

	eIDS_FAC_InputPlacard				= 201,	//! "请输入行会公告："
	eIDS_FAC_InputMemberTitle,					//! "请输入 %s 的称号："
	eIDS_FAC_InputFacName,						//! "请输入行会名称："
	eIDS_FAC_InputFacIconFileName,				//! "请输入徽标图片的文件名："

	eIDS_FAC_YouHavntJoinTheFac			= 301,	//! "您尚未加入行会！"
	eIDS_FAC_YouHaveBeenChairman,				//! "您已经被任命为 %s 的新会长！"
	eIDS_FAC_OldMasterSetNewMaster,				//! "%s 任命您为 %s 的新族长！"
	eIDS_FAC_YouHaveBeenKicked,					//! "您已经被踢出行会！"
	eIDS_FAC_YouHaveSetMemberJob,				//! "您已经将 %s 的官阶设置为 %s ！"
	eIDS_FAC_YouHaveDeposalMemberJob,			//! "您已经取消了 %s 的官阶权限！"
	eIDS_FAC_YouHaveQuitFac,					//! "您已经退出行会！"
	eIDS_FAC_YouHaveJoinTheFac,					//! "您已经加入行会： %s ！"
	eIDS_FAC_YourFacHasDisband,					//! "您所在的行会已经解散！"
	eIDS_FAC_YouHaveSetMemberTitle,				//! "您已经将 %s 的称号设置为 %s 。"
	eIDS_FAC_ChairmanHasSetYourJob,				//! "会长已经授予您 %s 的官阶 。"
	eIDS_FAC_ChairmanHasDeposalYourJob,			//! "会长已经取消了您的官阶。"
	eIDS_FAC_YouHasSendYourApply,				//! "您的行会申请已提交！请等待审批。。。"
	eIDS_FAC_YourFacLevelHasUpgrade,			//! "您所属行会的等级已经升至 %d 级！"
	eIDS_FAC_YourFacLevelHasDegrade,			//! "您所属行会的等级已经降至 %d 级！"
	eIDS_FAC_YourFacHasOpenRecruit,				//! "您所在的行会已经 开启招募！"
	eIDS_FAC_YourFacHasCloseRecruit,			//! "您所在的行会已经 停止招募！"

	eIDS_FAC_AreYouSureDisbandFac		= 401,	//! "您确定要解散行会吗？"
	eIDS_FAC_AreYouSureQuitFac,					//! "您确定要退出行会吗？"
	eIDS_FAC_AreYouSureKickMember,				//! "您确定要将 %s 逐出行会吗？"
	eIDS_FAC_AreYouSureAbdication,				//! "您确定将会长位置转让给 %s 吗？"
	eIDS_FAC_AreYouSureSetJobName,				//! "您确定将 %s 修改为 %s 吗？"
	eIDS_FAC_AreYouSureAuthorize,				//! "您确定批准 %s 加入本行会吗？"
	eIDS_FAC_AreYouSureInvite,					//! "您确定要邀请 %s 加入行会吗？"
	eIDS_FAC_AreYouSureReject,					//! "您确定拒绝 %s 的入会申请吗？"
	eIDS_FAC_AreYouSureApply,					//! "您确定要申请加入 %s 吗？"
	eIDS_FAC_AreYouSureCancelApply,				//! "您确定要取消加入 %s 的申请吗？"
	eIDS_FAC_AreYouSureEditPlacard,				//! "您确定要修改行会公告吗？"
	eIDS_FAC_AreYouSureEditMemo,				//! "您确定要修改 备忘录吗？"

	eIDS_FAC_Chairman					= 501,	//! "会长"
	eIDS_FAC_NoJob,								//! "无官阶"
	eIDS_FAC_MemberNum,							//! "会员数：%d/%d"
	eIDS_FAC_Recruit,							//! "招募中..."
	eIDS_FAC_UnRecruit,							//! "停止招募..."
	eIDS_FAC_WelfareExp,						//! "家族福利奖励"

	eIDS_FAC_KickMember					= 601,	//! "开除会员"
	eIDS_FAC_SetJob,							//! "设置官阶"
	eIDS_FAC_SetTitle,							//! "设置称号"
	eIDS_FAC_Abdication,						//! "转让会长位置"
	eIDS_FAC_DeposalJob,						//! "取消官阶"
	eIDS_FAC_SecretChat,						//! "密聊"
	eIDS_FAC_AddFriend,							//! "加为好友"
	eIDS_FAC_InviteTeam,						//! "邀请组队"

	eIDS_FAC_Purview					= 701,	//! "编辑家族公告"

	eIDS_FAC_PlacardText				= 801,	//! "【行会公告】：%s"
	eIDS_FAC_Prompt_LessMemNum,					//! "【家族通知】：家族人员过少，如持续 %d 天不足 %d 人，本家族将于（%d年%d月%d日24:00时正）被解散！"
	eIDS_FAC_Prompt_MasterLeaveTooLong,			//! "【家族通知】：族长已经 %d 天没有上线处理本族事务，如果超过 %d 天不上线，族长之位将于（%d年%d月%d日24:00时正）自动传于本族贡献最高的族员！"
	eIDS_FAC_Prompt_ResNotEnough,				//! "【家族通知】：家族资源不足以维持家族经营。如家族资源持续1周为负值，家族等级将被降级。所有家族科技将降低到该家族等级相应等级。"

	eIDS_FAC_Require_FacLvl				= 901,	//! "[COLOR 5B3200]家族等级：[/COLOR][COLOR %s] %d [/COLOR][BR]"
	eIDS_FAC_Require_MasterLvl,					//! "[COLOR 5B3200]族长等级：[/COLOR][COLOR %s] %d [/COLOR][BR]"
	eIDS_FAC_Require_FacExp,					//! "[COLOR 5B3200]家族经验：[/COLOR][COLOR %s] %d [/COLOR][BR]"
	eIDS_FAC_Require_FacRes,					//! "[COLOR 5B3200]家族资源：[/COLOR][COLOR %s] %d [/COLOR][BR]"
	eIDS_FAC_Require_Gold,						//! "[COLOR 5B3200]金币：[/COLOR][COLOR %s] %d [/COLOR][BR]"
	eIDS_FAC_Require_Prop,						//! "[COLOR 5B3200]道具：[/COLOR][COLOR %s] %d [/COLOR][BR]"
	eIDS_FAC_Require_Beachhead,					//! "[COLOR 5B3200]据点：[/COLOR][COLOR %s] 占领 %d 座城镇 [/COLOR][BR]"

	eIDS_FAC_Upgrade_FunDesc			= 950,	//! "[COLOR 5B3200]家族等级：[/COLOR][COLOR %s] %d [/COLOR]"

	eIDS_FAC_NumAndOnlineOfMembers		= 1001,	//! "[COLOR FFFFFF]%d[/COLOR][COLOR FFFF00] 位族员（[/COLOR][COLOR 00FF00]%d[/COLOR] [COLOR FFFF00]位[/COLOR][COLOR 00FF00]在线[/COLOR][COLOR FFFF00]）[/COLOR]"/>
	eIDS_FAC_NumAndMaxNumOfMembers,				//! "[COLOR FFFFFF]%d [/COLOR][COLOR FFFF00]位族员（[/COLOR][COLOR FFFFFF]最多能容纳[/COLOR][COLOR FF0000] %d [/COLOR][COLOR FFFFFF]位[/COLOR][COLOR FFFF00]）[/COLOR]"

	eIDS_FAC_WarInfoDesc				= 1010,	//! "[B][BR][IMG 1000][/IMG][BR]%s[BR][IMG 1001][/IMG][BR]%s[BR][IMG 1002][/IMG][BR]%s[/B]"
	eIDS_FAC_FacWarInfoDesc,					//! "[B][BR][IMG 1000][/IMG][BR]%s[BR][IMG 1001][/IMG][BR]%s[BR][IMG 1002][/IMG][BR]%s[/B]"
	eIDS_FAC_BeachheadDesc,						//! "[B][BR][IMG 1000][/IMG][BR]%s[BR][IMG 1001][/IMG][BR]%s[BR][IMG 1002][/IMG][BR]%s[/B]"
	eIDS_FAC_WarListDesc,						//! "[B][BR][IMG 1000][/IMG][BR]%s[BR][IMG 1001][/IMG][BR]%s[BR][IMG 1002][/IMG][BR]%s[/B]"

	eIDS_FAC_UnknownError				= 10000,//! "未知错误，操作失败！"
	eIDS_FAC_CountryError				= 10001,//! "国家不符，操作失败！"
	eIDS_FAC_CantCreateWhenYouHasFac,			//! "请先退出之前的行会，才能创建新的行会！"
	eIDS_FAC_FacNameRepeat,						//! "行会名称重复，请换一个名字！"
	eIDS_FAC_YourLevelNotEnough,				//! "您的等级不足，操作失败！"
	eIDS_FAC_YourMoneyNotEnough,				//! "您的金钱不足，操作失败！"
	eIDS_FAC_YourGemNotEnough,					//! "您缺少宝石，操作失败！"
	eIDS_FAC_GoodsError,						//! "物品错误，操作失败！"
	eIDS_FAC_ConfigFileError,					//! "配置获取还未完成，操作失败！"
	eIDS_FAC_ServerCoreError,					//! "核心服务器错误，操作失败！"
	eIDS_FAC_FacLevelNotEnough,					//! "帮会等级太低，操作失败！"
	eIDS_FAC_YouHavntThePurview,				//! "您没有此行会权限，操作失败！"
	eIDS_FAC_CantHandleTheMemberHasSamePurview,	//! "不能操作有同样权限的会员，操作失败！"
	eIDS_FAC_TheOperationNeedPlayerOnline,		//! "此操作需要对方在线才能完成！"
	eIDS_FAC_OperateObjectNotExist,				//! "操作目标不存在！"
	eIDS_FAC_PlayerHasJoinAntherFac,			//! "对方已经加入其它行会了！"
	eIDS_FAC_FacMaxNumError,					//! "行会人数上限已满！"
	eIDS_FAC_PlayerRejectYourInvite,			//! "对方拒绝了您的邀请！"
	eIDS_FAC_ApplyRepeat,						//! "不能重复申请加入，请先取消之前的申请后在进行操作！"
	eIDS_FAC_Timeout,							//! "操作时限已到，请重新点击NPC！"
	eIDS_FAC_SensitiveWord,						//! "名称中含有敏感字符！"
	eIDS_FAC_NotInFaction,						//! "没有加入行会！"
	eIDS_FAC_SameValue,							//! "原设置与新设置相同！"
};

//! 王峰：游戏设置 字符串
enum ENUM_IDS_GAMECONFIG
{
	eIDS_GAMECFG_AreYouSureLoadDefault	= 1,	//! "您确定要读取默认 游戏设置吗？"
	eIDS_GAMECFG_AreYouSureModify,				//! "您确定要修改 游戏设置吗？"
};


//! 王峰：置顶聊天 字符串
enum ENUM_IDS_TOPMSG
{
	eIDS_TOPMSG_InputYourWords			= 1,	//! "请输入需要发布的信息"

	eIDS_TOPMSG_NumOfLeavingWords		= 201,	//! "剩余字数"

	eIDS_TOPMSG_TopChatInBox			= 301,	//! "天籁"

	eIDS_TOPMSG_Err1					= 901,	//! "错误信息"

	eIDS_TOPMSG_BEKILLED				= 902,  //! "本阵营玩家在%s的（%d，%d）附近被击杀！"

	eIDS_TOPMSG_KILLIED					= 903,  //! "本阵营玩家 %s 在%s的(%d,%d)附近成功击杀敌人！！"

	eIDS_TOPMSG_BEATTACKED				= 904,  //! "本阵营玩家在%s的（%d，%d）附近被击杀！"

	eIDS_TOPMSG_ATTACKED				= 905,  //! "本阵营玩家正在%s地图的 (%d,%d)处，攻击敌对阵营，请速增援!"

	eIDS_TOPMSG_YOUATTACKED				= 906,  //! "你被 %s 攻击了!"
};



// 刘科：界面基本信息08.6.14
enum eIDS_CT_STR
{
	eIDS_BASEUI	=	1,						// 基本界面信息
	eIDS_SKILL,								// 技能（状态）相关
	eIDS_TEAM,								// 队伍相关
	eIDS_MAP,								// 地图相关
	eIDS_TSSHOP,							// 增值商店相关
	eIDS_CONTROL,							// 控制相关
	eIDS_QUEST,								// 任务相关
	eIDS_POPMENU,							// 探出菜单
	eIDS_RELIVE,                            // 死亡重生
	eIDS_SPRITE,                            // 小精灵界面相关
	eIDS_SYSTEMSETUP,                       // 系统设置界面相关
	eIDS_EQUIPWEAR,                         // 装备耐久度界面
	eIDS_TITLEMENU,                         // 称号菜单
	eIDS_MERCENARY = 15,                    // 佣兵任务
	eIDS_INPUTBOX,                          // 脚本输入框提示
	eIDS_MAIL,		                        // 邮件相关
	eIDS_SELECTSECTION,                     // 选择游戏分区
	eIDS_LUVINIA,                           // 表情动画界面
	eIDS_GAMENOTE,                          // 游戏公告
	eIDS_TIMEBOX,                           // 时间对话框
	eIDS_MEDAL,                             // 勋章相关
	eIDS_TOPMSG,							// 置顶聊天相关
	eIDS_PUBLIC = 300,                      // 公用文字
	eIDS_Enchant = 306,                     // 魔化界面

	eIDS_PRESENT = 400,						// 游戏外赠送系统相关

	eIDS_MAPKEY = 210,							// 功能映射热键系统

	eIDS_KEYNAME = 211,						// 虚拟键的名字
};

enum eIDS_CT_MAPKEY
{
	eIDS_MAPKEY_TYPE_MOVE = 100,
	eIDS_MAPKEY_FUNC_RUNORWALK, //走或跑
	eIDS_MAPKEY_FUNC_FORWARD,		// 向前
	eIDS_MAPKEY_FUNC_GOBACK,		// 向后
	eIDS_MAPKEY_FUNC_GOLEFT,		// 向左
	eIDS_MAPKEY_FUNC_GORIGHT,     // 向右

	eIDS_MAPKEY_TYPE_INTERFACE = 200,
	eIDS_MAPKEY_FUNC_ESC,			// 取消打开界面或者开启系统菜单界面
	eIDS_MAPKEY_FUNC_HIDEINTERFACE,	// 隐藏操作界面
	eIDS_MAPKEY_FUNC_TALKFACE,		// 开启私聊界面
	eIDS_MAPKEY_FUNC_QUST,			// 开启任务界面
	eIDS_MAPKEY_FUNC_SHOWGOODSNAME,	// 标示地面掉落物品名字显示
	eIDS_MAPKEY_FUNC_FASTREPLY,		// 快速回复
	eIDS_MAPKEY_FUNC_AUTOSUPPLY,		// 开启药品自动补给界面
	eIDS_MAPKEY_FUNC_SKILL,			// 开启技能界面
	eIDS_MAPKEY_FUNC_PRESTIGE,		// 开启人物声望界面
	eIDS_MAPKEY_FUNC_PACKAGE,		// 开启背包
	eIDS_MAPKEY_FUNC_FRIEND,			// 开启好友界面
	eIDS_MAPKEY_FUNC_PRODUCE,		// 开启专业制作界面
	eIDS_MAPKEY_FUNC_UNDERWORLD,		// 开启帮会界面
	eIDS_MAPKEY_FUNC_PK,				// 开启PK设置界面
	eIDS_MAPKEY_FUNC_HELP,			// 开启帮助界面
	eIDS_MAPKEY_FUNC_INCREASESHOP,	// 开启增殖商店
	eIDS_MAPKEY_FUNC_NPCLIST,		// 开启NPC列表
	eIDS_MAPKEY_FUNC_PLAYERPROPERTY,// 开启人物属性界面
	eIDS_MAPKEY_FUNC_SERCHFRIEND,	// 开启好友搜索界面
	eIDS_MAPKEY_FUNC_PACKAGE2,		// 开启背包界面
	eIDS_MAPKEY_FUNC_BIGMAP,			// 开启大地图

	eIDS_MAPKEY_TYPE_HOTKEY = 300,
	eIDS_MAPKEY_FUNC_HOTKEY_301,	
	eIDS_MAPKEY_FUNC_HOTKEY_302,
	eIDS_MAPKEY_FUNC_HOTKEY_303,
	eIDS_MAPKEY_FUNC_HOTKEY_304,
	eIDS_MAPKEY_FUNC_HOTKEY_305,
	eIDS_MAPKEY_FUNC_HOTKEY_306,
	eIDS_MAPKEY_FUNC_HOTKEY_307,
	eIDS_MAPKEY_FUNC_HOTKEY_308,
	eIDS_MAPKEY_FUNC_HOTKEY_309,
	eIDS_MAPKEY_FUNC_HOTKEY_310,
	eIDS_MAPKEY_FUNC_HOTKEY_311,
	eIDS_MAPKEY_FUNC_HOTKEY_312,
	eIDS_MAPKEY_FUNC_HOTKEY_313,
	eIDS_MAPKEY_FUNC_HOTKEY_314,
	eIDS_MAPKEY_FUNC_HOTKEY_315,
	eIDS_MAPKEY_FUNC_HOTKEY_316,
	eIDS_MAPKEY_FUNC_HOTKEY_317,
	eIDS_MAPKEY_FUNC_HOTKEY_318,
	eIDS_MAPKEY_FUNC_HOTKEY_319,
	eIDS_MAPKEY_FUNC_HOTKEY_320,
	eIDS_MAPKEY_FUNC_HOTKEY_321,
	eIDS_MAPKEY_FUNC_HOTKEY_322,
	eIDS_MAPKEY_FUNC_HOTKEY_323,
	eIDS_MAPKEY_FUNC_HOTKEY_324,

	eIDS_MAPKEY_TYPE_OTHER = 400,
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_LOCKATTACK,		// 锁定目标进行普通攻击
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_AUTOMOVE,		// 自动跟随鼠标移动
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_FITTING,		// 试穿功能
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_AUTOPRINTSCREEN,// 自动截图
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_CHAT,			// 开启聊天框
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_CHANGETARGET,	// 切换攻击目标
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_PICKGOODS,		// 拾取物品
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_AFFIRMTARGET,	// 显示当前选中的目标对象
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_FIRESPELLSELF,	// 与技能快捷键配合使用对自己施放法术
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_CLOSEDISTANCE,	// 拉近视角距离
	eIDS_MAPKEY_FUNC_OTHERHOTKEY_FARDISTANCE,	// 拉远视角距离

};

enum eIDS_CT_BASEUI
{
	eIDS_WAR_FOMAT = 1,						// 警告：类型ID：%d中，存在相同的字符串ID：%d
	eIDS_WAR_UPDATE,						// 找不到服务器列表文件，请运行update.exe进行修复
	eIDS_WAR_EDITION,						// 游戏版本错误！
	eIDS_WAR_CDKEY,							// 该CDKEY不存在！
	eIDS_WAR_ACCOUNT,						// 该账号被禁止使用至%s
	eIDS_WAR_ACCOUNT_FORBIDUSE,				// 对不起，您的账号已经被禁止使用！
	eIDS_WAR_CANNOTLOADGAME,				// 对不起，您所在的网段无法登陆本游戏区，请与游戏运营商联系。
	eIDS_WAR_CDKEY_ERROR,					// CDKEY或密码错误(请注意英文字母的大小写！)
	eIDS_WAR_ACCOUNT_ONLINE,				// 该帐号目前处于登陆状态，服务器已将该帐号强制断线，请稍候再试。
	eIDS_WAR_SERV_DATAERROR,				// 服务器数据库产生异常,请稍后再试！
	eIDS_WAR_SERV_BUSY,						// 服务器繁忙，请稍候再试！
	eIDS_WAR_BASEINFO_UNSUCCESSFUL,			// 取得基本信息失败！
	eIDS_WAR_DELROLE_SUCCESS,				// 角色删除成功！
	eIDS_WAR_DELROLE_UNSUCCESS,				// 角色删除失败！
	eIDS_WAR_DELROLE_UNSUCCESS_CITY,		// 该角色的帮会拥有主城，不能删除。
	eIDS_WAR_DELROLE_UNSUCCESS_JOINUNION,	// 该角色的帮会加入了同盟。不能删除。
	eIDS_WAR_DELROLE_UNSUCCESS_HAVEUNION,	// 该角色的帮会创建了同盟。不能删除。
	eIDS_WAR_RENEWROLE_SUCCESS,				// 角色恢复成功
	eIDS_WAR_RENEWROLE_UNSUCCESS,			// 角色恢复失败
	eIDS_WAR_ADDROLE_UNSUCCESS_DB,			// 添加角色失败(数据库操作错误)！
	eIDS_WAR_ADDROLE_UNSUCCESS_BENAME,		// 添加角色失败(玩家名已经存在)！
	eIDS_WAR_ADDROLE_UNSUCCESS_MAXNUM,		// 添加角色失败(达到创建人数上限)！
	eIDS_WAR_ADDROLE_UNSUCCESS_NAMERROR,	// 添加角色失败(玩家名字有非法字符)！
	eIDS_WAR_ADDROLE_UNSUCCESS_UNKNOWN,		// 添加角色失败(未知的错误类型)！
	eIDS_WAR_CONNECT_UNSUCCESS,				// 连接游戏服务器失败，请重试...
	eIDS_WAR_AMPLYINFO_UNSUCCESS,			// 取得详细信息失败！
	eIDS_WAR_ROLE_UNLOAD,					// 该角色没有登陆！
	eIDS_WAR_RECONNECT,						// 啊哦~~~~服务器连接不上了，重试一下吧:)
	eIDS_WAR_QUITGAME_ONFIGHT,				// 战斗状态不能离线，是否强制退出？
	eIDS_WAR_QUITGAME_TOSELECT,				// 退出游戏到人物选择页面？
	eIDS_WAR_QUITGAME_TOWINDOWS,			// 退出游戏到Windows桌面？
	eIDS_WAR_CREATEROLE,					// 请先创建一个角色！
	eIDS_WAR_ISRENEWROLE,					// 角色已经被删除，要恢复该角色吗?
	eIDS_WAR_CANNTCREATEROLE,				// 目前不能创建更多的角色了！
	eIDS_WAR_READYDEL,						// 该角色已经被删除！
	eIDS_WAR_DEL_DISFAC,					// 角色将在7日后彻底删除。如果你是帮主，行会将立即解散。
	eIDS_WAR_DEL_AFTERDAY,					// 还有%d天被删除
	eIDS_WAR_PLAYER_FULL,                   // 服务器人物已满
	eIDS_WAR_NOT_ACTIVE,                    // 帐号未激活
	eIDS_WAR_IS_LOCKED,                     // 用户被锁定
	eIDS_WAR_REJECTED,                      // 账号被封
	eIDS_WAR_INIT_FINISH,                   // 验证对象没有准备好
    eIDS_WAR_DB_COM_ERROR,                  // 数据库操作对象错误
	eIDS_WAR_CONN_DB,                       // 不能连接到DB
	eIDS_WAR_OPT_FAILED,                    // DB操作失败
	eIDS_WAR_DB_FAILED,                     // 数据库维护中，不能通过账号验证
	eIDS_WAR_QUEUE_SUCCESS,                 // 进入游戏成功，正在读取角色信息。。。
	eIDS_WAR_FROZEN_CDKEY,                  // 密码重试过多账号冻结
	
 
	eIDS_WAR_DOUBCLICKCOMP = 50,			// 鼠标双击控件 %s\n
	eIDS_WAR_ENTERCOMP,						// 鼠标进入控件 %s\n
	eIDS_WAR_LEAVECOMP,						// 鼠标离开控件 %s\n
	eIDS_WAR_PRESSCOMP,						// 鼠标按下控件 %s\n
	eIDS_WAR_CLICKCOMP,						// 鼠标点击控件 %s\n
	eIDS_WAR_RELEASEDCOMP,					// 鼠标松开控件 %s\n
	eIDS_WAR_SETCOMPNULL,					// 设置新的鼠标控件为NULL\n
	eIDS_WAR_SETCOMP,						// 设置新的鼠标控件\n
	eIDS_WAR_SETACTCOMPNULL,				// 设置新的活动控件为NULL\n
	eIDS_WAR_SETACTCOMP,					// 设置新的活动控件 %s\n

	eIDS_TAB_LOCK,							// 开启队友选取后，将自动关闭其他的所有选取
	eIDS_TAB_UNLOCK,						// 设置您的TAB键选取范围，只有被勾选的对象可被TAB键选取到，取消所有勾选将关闭TAB功能

	eIDS_STR_NEUTRAL	=	100,			// 【中立】
	eIDS_STR_HONOR_JAZZ_INFO,				// 这里是勋章和爵位信息
	eIDS_STR_SPEAK_FASE,					// 你说话太快了!
	eIDS_STR_COUNCHAT_GOODS_GOLD,			// 在国家频道发布消息将消耗%d个[%s]消耗金钱:%d
	eIDS_STR_COUNCHAT_UNSUCCESS_GOLD,		// 持有金钱不足，无法发布国家喊话。
	eIDS_STR_COUNCHAT_UNSUCCESS_GOODS,		// 所需道具[%s]不足，无法发布国家喊话。
	eIDS_STR_COUNCHAT_GOLD,					// 在国家频道发布消息将消耗金钱:%d
	eIDS_STR_COUNCHAT_GOODS,				// 在国家频道发布消息将消耗%d个[%s]

	eIDS_STR_WORLDCHAT_GOODS_GOLD,			// 在世界频道发布消息将消耗%d个[%s]消耗金钱:%d
	eIDS_STR_WORLDCHAT_UNSUCCESS_GOLD,		// 持有金钱不足，无法发布世界喊话。
	eIDS_STR_WORLDCHAT_UNSUCCESS_GOODS,		// 所需道具[%s]不足，无法发布世界喊话。
	eIDS_STR_WORLDCHAT_GOLD,				// 在世界频道发布消息将消耗金钱:%d
	eIDS_STR_WORLDCHAT_GOODS,				// 在世界频道发布消息将消耗%d个[%s]

	eIDS_STR_PRIVATECHAT_TIP,				// /玩家名字 说话内容
	eIDS_STR_START_FACEACT,					// %s开始表情动作%d
	eIDS_STR_INPUT_FINDCODE,				// 请输入搜索内容


	eIDS_HELP_PRIVATECHAT	=	120,		// [/对方名字 内容...] 同指定玩家私聊。
	eIDS_HELP_REGIONCHAT,					// [!内容...] 同本场景的所有玩家聊天
	eIDS_HELP_TEAMCHAT,						// [#内容...] 同一个队伍玩家聊天。
	eIDS_HELP_FACCHAT,						// [%内容...] 同一个帮会玩家聊天。

	eIDS_STR_WORLDCHAT	=	130,			// [世界喊话]%s:%s
	eIDS_STR_WORLDCHAT_UNSUCCESS,			// [世界喊话] 失败！
	eIDS_STR_COUNCHAT,						// [国家喊话]%s:%s"
	eIDS_STR_COUNCHAT_UNSUCCESS,			// [国家喊话] 失败！
	eIDS_STR_UNIONCOUNCHAT,					// [盟国喊话]%s:%s
	eIDS_STR_UNIONCOUNCHAT_UNSUCCESS,		// [盟国喊话] 失败！

	eIDS_STR_SECURTIY_SAFE	=	150,		// [安全区]
	eIDS_STR_ENTER_SAFEAREA,				// 你已进入[安全区]
	eIDS_STR_SECURTIY_FIGHT,				// [竞技区]
	eIDS_STR_ENTER_FIGHTAREA,				// 你已进入[竞技区]
	eIDS_STR_YOU_LEAVEAREA,					// 你离开了[安全区]
	eIDS_STR_AUTOMOVETO,					// 自动移动到:%s(%d,%d)
	eIDS_STR_AUTOMOVETO_FAILED,				// 不能移动到该位置，请设置正确的坐标！

	eIDS_STR_WHO_PET = 200,					// 的宠物
	eIDS_STR_WHO_CARRIAGE,					// 的护送物

	eIDS_STR_PAGE_ROLE	=	300,			// 角  色
	eIDS_STR_PAGE_REPUTE,					// 声  望
	eIDS_STR_ENCOURAGINGEXP,				// ;奖励经验%d
	eIDS_STR_LINENUM = 304,                 // %线

	/* *****************************
	功能:   防外挂字符串资源定义
	作者:zhaohang(kaba)
	创建日期: 2009/5/27
	修改日志:
		修改日期:
		修改内容:
		修改人  :
	* *****************************/
	eIDS_CHECKCHEAT_QUESTION = 409,			//亲爱的玩家请尽快答题，有惊喜等着你哟!
	eIDS_CHECKCHEAT_REMAINTIME = 410,		//答题剩余时间：%d秒！
	eIDS_CHECKCHEAT_LOCKACCOUNT = 411,		//您今天的游戏时间已经到了，请好好休息!
	eIDS_CHECKCHEAT_CUE			= 412,       //请在剩余时间内完成答题！
											//413:所在IP登陆此角色的数量达到最大，将受到一段时间保护
											//414:所在IP登陆的账号数量太多，将受到一段时间屏蔽
											//415:所在IP请求建立角色的次数太多，将受到一段时间屏蔽
	eIDS_CHECKCHEAT_ANSRIGHT    =416,		//本次答题正确
	eIDS_CHECKCHEAT_ANSWRONG    =417,        //本次答题错误
	eIDS_CHECKCHEAT_OVERTIME    =418,		//本次答题超时

	eIDS_IME_HOR_STYLE			= 423,		//切换到竖版
	eIDS_IME_VER_STYLE			= 424,		//切换到横版
	eIDS_LIVEPOINT				= 425,		//活力点资源
	eIDS_LIVEPOINTDESP			= 426,
	eIDS_CPPOINT                = 427,      //CP值提示信息
											//428:当前输入的聊天内容过多.
											//429:与[%s]的对话
											//430:与[%s]的对话记录
	eIDS_PRICHATMSGBOX			= 431,      //信息已满，请逐次查看
	eIDS_PRICHATNEWMSG          = 432,       //新消息(%d)

	eIDS_WAR_DELROLE_LEVELLESS = 800,				// 未达到删除角色需要的等级
	eIDS_WAR_DELROLE_DELETING,				// 角色删除中...
	eIDS_WAR_LEVEL,							// 角色等级
	eIDS_WAR_REGION						// 角色所在区
};

enum eIDS_CT_SKILL
{
	eIDS_USE_UNSUCCESS_OBJERROR = 1,			// 技能释放失败，请选择一个合适的目标！
	eIDS_USE_UNSUCCESS_OBJNULL,					// 无效的目标对象，该技能无法释放！
	eIDS_USE_UNSUCCESS_ARM,						// 要释放该技能必须装备对应的武器！
	eIDS_USE_UNSUCCESS_GOODS,					// 没有必要的道具，无法使用技能！
	eIDS_USE_UNSUCCESS_HP,						// 没有足够的HP，使用技能失败！
	eIDS_USE_UNSUCCESS_MP,						// 没有足够的MP，使用技能失败！
	eIDS_USE_UNSUCCESS_NP,						// 没有足够的怒值，使用技能失败！
	eIDS_USE_UNSUCCESS_INT_MINDEST,				// 吟唱的最小施法距离不足，使用技能失败！
	eIDS_USE_UNSUCCESS_INT_MAXDEST,				// 吟唱的最大施法距离不足，使用技能失败！
	eIDS_USE_UNSUCCESS_MINDEST,					// 最小施法距离不足，使用技能失败！
	eIDS_USE_UNSUCCESS_MAXDEST,					// 最大施法距离不足，使用技能失败！

	eIDS_USE_UNSUCCESS_STATE,					// 当前状态下无法使用该技能！
	eIDS_USE_UNSUCCESS_YVALUE,					// 没有足够的妖值，使用技能失败！
	eIDS_USE_UNSUCCESS_OUCC,					// 当前职业不能使用该技能！
	eIDS_USE_UNSUCCESS_OUTAREA,					// 超出作用范围！
	eIDS_USE_UNSUCCESS_NOSKILL,					// 没有该技能！
	eIDS_USE_UNSUCCESS_COOLTIME,				// 技能冷却时间未过！
	eIDS_USE_UNSUCCESS_BLOCK,					// 被阻挡了！
	eIDS_USE_UNSUCCESS_DELSKILL,				// 技能被取消了！

	eIDS_USE_UNSUCCESS_NOOBJ,					// 没有作用目标！
	eIDS_USE_UNSUCCESS_INTTIME,					// 吟唱时间没完，使用技能失败！
	eIDS_USE_UNSUCCESS_MADE,					// 制作失败！
	eIDS_USE_UNSUCCESS_COLLECTION,				// 采集失败！
	eIDS_USE_UNSUCCESS_ACTING,					// 动作没有完成！
	eIDS_USE_UNSUCCESS_MOVEING,					// 移动中不能使用技能！
	eIDS_USE_UNSUCCESS_CANNOTPHY,				// 目前不能使用物理系技能！
	eIDS_USE_UNSUCCESS_CANNOTMAGIC,				// 目前不能使用魔法系技能！
	eIDS_USE_UNSUCCESS_OBJDIED,					// 该技能不能对已死亡的对象使用！
	eIDS_ERROR_ATTACK_QUESTOBJ,					// 你在攻击一个不属于你的任务怪物，杀死它并不会使你完成任务！
	eIDS_ERROR_ATTACK_INGOD,					// 目标玩家正处于切换场景的保护状态！
	eIDS_ERROR_USEGOODS_FORSKILL,				// 使用物品施放技能时离施放目标太远的错误提示
	eIDS_ERROR_BYREGION,						// 当前场景下无法使用该技能！
	eIDS_ERROR_BYEQHORSE,						// 未装备坐骑无法进入乘骑状态！

	eIDS_STUDY_SKILLTOP	= 48,					// 在当前职业下，技能已达到最高等级，无法提升!
	eIDS_STUDY_NEEDLEV,							// 等级
	eIDS_STUDY_OUCC,							// 职业不符合学习条件！
	eIDS_STUDY_LEV,								// 等级不符合学习条件！
	eIDS_STUDY_SP,								// SP不符合学习条件！
	eIDS_STUDY_PRESKILL,						// 前置技能不符合学习条件！
	eIDS_STUDY_LIMITSKILL,						// 限制技能不符合条件！
	eIDS_STUDY_GOODS,							// 缺少学习技能所必需的物品！
	eIDS_STUDY_UNSUCCESS,						// 条件不满足，学习技能失败！
	eIDS_STUDY_MADESKILL_UNSUCCESS,				// 制作技能学习失败！
	eIDS_STUDYED,								//   （已学）
	eIDS_STUDY_DIS_CONDITION,					//   （条件不足）
	eIDS_STUDY_NOGOLDS,							// 金币不足，学习技能失败！
	eIDS_STUDY_NOSILVER,						// 银币不足，学习技能失败！
	eIDS_STUDY_FAILED_OCCULV,					// 职业等级不符合条件，学习技能失败！

	eIDS_PAGE_NORMALATTC = 98,					// 普通攻击
	eIDS_PAGE_SKILL,							// 技  能
	eIDS_PAGE_STUDYED,							// 习  得
	eIDS_PAGE_CHANGEOCCU,						// 转  职


	eIDS_ERROR_CONDITION_NOOBJ = 201,			// 采集对象不存在！
	eIDS_ERROR_CONDITION_STATE,					// 当前状态下不能采集！
	eIDS_ERROR_CONDITION_NOINFO,				// 未知的采集物！
	eIDS_ERROR_CONDITION_OCCUEX,				// 您的副职业不能采集此物！

	eIDS_ERROR_CONDITION_LEV,					// 您的采集等级不够！
	eIDS_ERROR_CONDITION_GOODS,					// 物品条件不满足！
	eIDS_ERROR_CONDITION_PACK,					// 您的背包满啦~！
	eIDS_ERROR_CONDITION_NOTQUESTOBJ,			// 这不是任务采集物！
	eIDS_ERROR_CONDITION_SPECQUEST,				// 要采集此物，您需要执行特殊的任务！
	eIDS_ERROR_CONDITION_TYPE,					// 未知的采集类型！

	eIDS_ERROR_MADE_NOOBJ,						// 没有这种制作物！
	eIDS_ERROR_MADE_OCCUEX,						// 您的副职业无法制作此物品！
	eIDS_ERROR_MADE_NOSKILL,					// 您还没学会制作此物品！
	eIDS_ERROR_MADE_TOOL,						// 制作此物品需要特殊的工具！
	eIDS_ERROR_MADE_GOODS,						// 制作此物品需要特殊的材料！
	eIDS_ERROR_MADE_NPCDEST,					// 您离NPC太远拉！
	eIDS_ERROR_MADE_PACK,						// 您没有足够的背包空间存放制作物品啦！
};

enum eIDS_CT_TEAM
{
	eIDS_TEAM_CAPED	= 1,						// 该队友已经是队长！
	eIDS_TEAM_CAPSELF,							// 您成为了本队队长！
	eIDS_TEAM_CAPMEMBER,						// %s成为了本队队长！
	eIDS_TEAM_OCCULEV,							// 职业技能不达要求，不能组队！
	eIDS_TEAM_EXPALLOT,							// 队伍当前的分配方式为：%d
	eIDS_TEAM_MEMBERLOADEX,						// 队友%s非正常离线
	eIDS_TEAM_MEMBERRELOAD,						// 队友%s重新上线

	eIDS_TEAM_SELFLEAV,							// 你离开了队伍
	eIDS_TEAM_MEMBERLEAV,						// %s离开了队伍
	eIDS_TEAM_ON_OFF,							// 您没有打开组队开关，拒绝了对方玩家的组队请求！

	eIDS_TEAM_SELFJOINED,						// 邀请你加入队伍，是否同意？
	eIDS_TEAM_BEGJOIN,							// %s 邀请你加入队伍。
	eIDS_TEAM_JOIN,								// 加入队伍！
	eIDS_TEAM_OBJNOTJOIN,						// 对方暂时不愿组队！
	eIDS_TEAM_NOJOIN,							// 不愿加入您的队伍！
	eIDS_TEAM_WHOSETEAM,						// 的队伍
	eIDS_TEAM_OPENSHOP,							// 开店状态下无法打开招募
	eIDS_TEAM_HASTEAM,							// 你已经有队伍了，不能加入其他队伍
	eIDS_TEAM_MEMINOTHERMAP,					// 队友不在当前地图中
	eIDS_TEAM_MEMLOST,							// 队友意外离线
	eIDS_TEAM_INTHISTEAM,						// 你已经在本队伍中
	eIDS_TEAM_ISJOINTEAM,						// 您确定要加入该队伍？
	eIDS_TEAM_NOMINATE,							// 队伍成员%s推荐%s加入队伍，您是否同意？
	eIDS_TEAM_INSPOINT,							// 本场景下，该操作无效



	eIDS_TEAMALLOT_GA_FREE = 30,				// 自由拾取
	eIDS_TEAMALLOT_GA_QUEUE,					// 顺序拾取
	eIDS_TEAMALLOT_GA_RANDOM,					// 随机拾取

	eIDS_TEAM_TooFar = 40,						// 目标距离太远不能跟随移动
};

enum eIDS_CT_MAP
{
	eIDS_MAP_POS1 = 1,							// 当前坐标   X：%d  Y：%d
	eIDS_MAP_REIONG,							// 当前场景：%s
	eIDS_MAP_POS2,								// [COLOR 00FF00]%s[br]坐标: (%d, %d)[/COLOR]
	eIDS_MAP_POS3,								// [COLOR FF0000]坐标: (%d, %d)[/COLOR]
	eIDS_MAP_POS4,								// 坐标: (%d, %d)

	eIDS_MAP_NOOCCUPY = 10,						// 未占领
	eIDS_MAP_OUROCCUPY,							// 我方占领
	eIDS_MAP_OURWILLOCCUPY,						// 我方预占领
	eIDS_MAP_ENEMYOCCUPY,						// 被敌方占领
	eIDS_MAP_ENEMYWILLOCCUPY,					// 被敌方预占领
};

enum eIDS_CT_TSSHOP
{
	eIDS_TSSHOP_YUANBAO = 1,					// 星钻
	eIDS_TSSHOP_DIANJUAN,						// 点卷
	eIDS_TSSHOP_BUYINFO,						// 您将确定花费%d的%s购买%d数量的商品吗？
	eIDS_TSSHOP_BUYSHOP,						// 商店购买
	eIDS_TSSHOP_CHONGZHI,						// 充值

	eIDS_TSSHOP_TRADETO,						// 交易给
	eIDS_TSSHOP_TRADEGIVE,						// 交易得
	eIDS_TSSHOP_TRADEBU,						// 交易买
	eIDS_TSSHOP_TRADEYSALE,						// 交易卖

	eIDS_TSSHOP_PAY,							// 偿还
	eIDS_TSSHOP_OMIT,							// ...
	eIDS_TSSHOP_TRADEYING,						// 在交易中，不能打开增值商店。
	eIDS_TSSHOP_DIED,							// 死亡状态下，无法打开增值商店
	eIDS_TSSHOP_WEIMIAN,						// 位面精华
	eIDS_TSSHOP_NOWEIMIAN,						// 您的位面精华不足！
	eIDS_TSSHOP_NODIANJUAN,						// 您的点卷不足！
	eIDS_TSSHOP_NOYUANBAO,						// 您的星钻不足！
	eIDS_TSSHOP_ASKCOMPFULL,					// 索要栏已满！
	eIDS_TSSHOP_LARCOMPFULL,					// 赠送栏已满！
	eIDS_TSSHOP_XZSHOP,							// 星钻商店
	eIDS_TSSHOP_DQSHOP,							// 点卷商店
	eIDS_TSSHOP_WMSHOP,							// 位面精华商店
	eIDS_TSSHOP_SEARCHTXT,						// 搜索结果
	eIDS_TSSHOP_OTHER,							// 其它
	eIDS_TSSHOP_SEARCHFAILED,					// 对不起，没有搜到符合条件的商品！
	eIDS_TSSHOP_RETURN = 28,					//返还点券
};

enum eIDS_CT_CONTROL
{
	eIDS_CONTROL_AUTOMOVE	=	1,				// 进入自动跑步状态,按鼠标左键取消...
	eIDS_CONTROL_FOLLOWPLAYER,					// 自动跟随玩家 %s ,按鼠标左键取消...
	eIDS_CONTROL_PASSSKILL,						// 该技能为被动技能，无法直接使用！
	eIDS_CONTROL_NOTUSESKILLEX,					// 当前状态不能使用副职业技能！
	eIDS_CONTROL_OBJNULL,						// 目标空！
	eIDS_CONTROL_MSGSEND,						// 消息发送成功！！！！！
	eIDS_CONTROL_NOGOODS,						// 物品条件没满足！！！！
	eIDS_CONTROL_NOQUEST,						// 任务条件没满足！！！！
	eIDS_CONTROL_NOQUESTSTEP,					// 任务步骤条件没满足！！！！
	eIDS_CONTROL_NOOCCU,						// 职业条件不满足！！！！
	eIDS_CONTROL_BEINGCOLLECTED,				// 采集物正在被采集！
	eIDS_CONTROL_CANTBECOLLECTED,				// 采集物不能被采集！
	eIDS_CONTROL_CHANGETOWALKMODE=13,			// 切换到行走状态！
	eIDS_CONTROL_CHANGETORUNMODE=14,			// 切换到跑步状态！
	eIDS_CONTROL_CANTTOUCHNPC=15,				// 不能点击NPC！
	eIDS_CONTROL_CANTFOLLOWPLAYER=18,			// 增值商店开启时，不能跟随玩家移动！

	eIDS_MOVETEST_0		=	20,					// (Time:%d)移动模式下玩家到达x:%d,y:%d.
	eIDS_MOVETEST_1,							// (Time:%d)玩家退出移动模式x:%d,y:%d.
	eIDS_MOVETEST_2,							// (Time:%d)攻击模式下玩家到达x:%d,y:%d.
	eIDS_MOVETEST_3,							// (Time:%d)攻击模式下玩家攻击x:%d,y:%d.
	eIDS_MOVETEST_4,							// (Time:%d)玩家退出攻击模式x:%d,y:%d.
	eIDS_MOVETEST_5,							// (Time:%d)玩家进入移动模式x:%d,y:%d.
	eIDS_MOVETEST_6,							// (Time:%d)玩家进入攻击模式x:%d,y:%d.
	eIDS_MOVETEST_7,							// (Time:%d)结束移动,MPX:%d,MPY:%d,DestX:%d,DestY:%d
	eIDS_MOVETEST_8,							// (Time:%d)移动一步,MPrX:%d,MPY:%d,action:%d,dir:%d,nDis:%d,newx1:%d,newy1:%d,newx2:%d,newy2:%d
	eIDS_MOVETEST_9,							// (Time:%d)请求释放技能消息,skillid:%d,MPX:%d,MPY:%d
	eIDS_MOVETEST_10,							// (Time:%d)请求移动,oldx:%d,oldy:%d,newx:%d,newy:%d
	eIDS_MOVETEST_11,							// 开始攻击(Time:%d)%s怪物,Px:%d,Py:%d,Mx:%d,My:%d
	eIDS_MOVETEST_12,							// (Time:%d)请求停止自动移动模式x:%d,y:%d
	eIDS_MOVETEST_13,							// (Time:%d)请求停止自动攻击模式x:%d,y:%d.
	eIDS_MOVETEST_14,							// (Time:%d)收到技能回复ret:%d,sendmsgcount:%d,Mx:%d,My:%d,MAction:%d
	eIDS_MOVETEST_15,							// (Time:%d)设置动作%d,x:%d,y:%d.
	eIDS_CONTROL_NOTCOLLECT,					// 交易时不能采集！！！！
	eIDS_CONTROL_MOVEHIGHFREQ=37,				// 移动太频繁
	eIDS_CONTROL_AUTOMOVETIP=38,				// [COLOR FFFFFF]点击自动移动到：[/COLOR][BR][COLOR FFF600]%s（%d,%d）[BR]%s[/COLOR][BR][COLOR 00FF00]＜右键点击按钮取消自动移动＞[/COLOR]

	eIDS_CONTROL_CANTFACEACTION_ON_RIDE	   = 41,		// 乘骑状态不能使用仪态动作！
	eIDS_CONTROL_CANTFACEACTION_IN_WATER   = 42,		// 水下状态不能使用仪态动作！
	eIDS_CONTROL_CANTFACEACTION_AT_CURRENT = 43,		// 当前状态不能使用仪态动作！
	eIDS_CONTROL_CANTFACEACTION_AT_FIGHT   = 44,		// 战斗状态不能使用仪态动作！
};

enum eIDS_CT_QUEST
{
	eIDS_QUEST_NOTMOVETO = 1,						// 对不起，在本场景中无法自动移动到目标！
	eIDS_QUEST_XMLERROR,							// 错误：当前配置不存在章节为%d的主线任务，请检查！
	eIDS_QUEST_DOON,								// （完成）
	eIDS_QUEST_ADDQUEST,							// 添加一个任务\n
	eIDS_QUEST_ADDQUESTSHOW,						// 界面显示上添加一个任务
	eIDS_QUEST_SHOWQUESTNUM,						// 界面显示任务数
	eIDS_QUEST_ADDNEWQUEST,							// 界面提示接受了新任务
	eIDS_QUEST_DELNPCTIP,							// 删除该任务相关NPC的提示
	eIDS_QUEST_RETESTNPCINFO,						// 从新检测场景中与该任务相关NPC的提示信息
	eIDS_QUEST_RIGHTTARGETMAX,						// 任务提示最多同时显示5条！
	eIDS_QUEST_NOTRACE,								// 该任务没有需要提示的任务目标！

	eIDS_QUEST_PAGE_QUEST	=	99,					// 任  务
	eIDS_QUEST_PAGE_SAGA,							// 传  记
};

enum eIDS_CT_POPMENU
{
	eIDS_POPMENU_JOINTEAM	= 1,					// 邀请组队
	eIDS_POPMENU_JOINCAP,							// 升为队长
	eIDS_POPMENU_JOINKICK,							// 踢出队伍
	EIDS_POPMENU_EXCHANGE,							// 交易请求
	EIDS_POPMENU_FRIEND,							// 加为好友
	EIDS_POPMENU_FOLLOW,							// 跟随移动
	EIDS_POPMENU_CHAT,								// 私聊密语
	eIDS_POPMENU_JOINFAC, 							// 邀请入会
	EIDS_POPMENU_PLAYERINFO,						// 玩家信息
	EIDS_POPMENU_COPYNAME,							// 复制名字
	EIDS_POPMENU_BLACKLIST,							// 加入黑名单
	EIDS_POPMENU_ALARM,								// 举报玩家
	EIDS_POPMENU_NOMINATE,							// 推荐入队
	EIDS_POPMENU_ALARM_USEWG,							// 使用游戏外挂
	EIDS_POPMENU_ALARM_WGINFO,							// 发布外挂信息
	EIDS_POPMENU_ALARM_GAMECRACK,						// 利用游戏漏洞

	EIDS_POPMENU_FREECOLLECT = 100,					// 自由拾取
	EIDS_POPMENU_TACTICCOLLECT,						// 顺序拾取
	EIDS_POPMENU_RANDOMCOLLECT,						// 随机拾取
};

enum eIDS_CT_MAIL
{
	eIDS_MAIL_OPENFAILD = 1,						// 玩家死亡，无法打开邮箱！
	eIDS_MAIL_TYPEERROR = 2,						// 效验邮件类型出错，请检查程序逻辑!
	eIDS_MAIL_ADDGOODSFAILD = 3,					// 此物品与之前的附件物品类型不符，物品添加失败！
	eIDS_MAIL_READFAILD = 4,						// 阅读邮件失败！
	eIDS_MAIL_GOODS_CLICKPAYMENT = 5,				// 不能获取物品，请点击付款收信按钮！
	eIDS_MAIL_GOLDS_CLICKPAYMENT = 6,				// 不能获取金币，请点击付款收信按钮！
	eIDS_MAIL_PAYMENT = 7,							// 付款
	eIDS_MAIL_PRESENT = 8,							// 赠送
	eIDS_MAIL_REQUEST = 9,							// 索取
	eIDS_MAIL_PRESENT_NOGOLD = 10,					// 赠送这些物品所需的星钻不够，发送失败！
	eIDS_MAIL_PRESENT_ASSURE = 11,					// 您确实要赠送这些物品吗！
	eIDS_MAIL_REQUEST_ASSURE = 12,					// 您确实要要向对方索要这些物品吗！
	eIDS_MAIL_NO_GOLDS = 13,						// 您的金币不足，发送失败！
	eIDS_MAIL_ISPAYGOLD = 14,						// 是否确定让对方付费收信？
	eIDS_MAIL_NOTDEL_BYPAYGOLD = 15,				// 付费类信件无法删除！
	eIDS_MAIL_ISDELMAIL_HAVEGOODS = 16,				// 您选中要删除的信件当中含有物品附件，请确认是否删除？
	eIDS_MAIL_ISDELMAIL = 17,						// 是否确定删除所选信件？
	eIDS_MAIL_ISPAYTOACCEPT = 18,					// 是否同意以%d金币价格收取信件中的附件？
	eIDS_MAIL_NOTPAYTOACCEPT = 19,					// 您的金币不足，不能付款！
	eIDS_MAIL_ISAGREEDPRESENT = 20,					// 是否同意以%d星钻的价格将邮件中的附件赠送给%s？
	eIDS_MAIL_UPDATAGOODSLIST = 21,					// 附件中有商品已经撤柜无法购买，信件中予以删除！
	eIDS_MAIL_CANNOTADDGOODS = 22,					// 绑定物品不能作为附件寄送！

	eIDS_MAIL_SYSTEM = 23,							// 系统
	eIDS_MAIL_ADDGOODSFAILD_BYMULIT = 24,			// 带有附件物品或金币的信件不能群发！
	eIDS_MAIL_GETNEWMAIL = 25,						// 您有新的信件，是关于%s。
	eIDS_MAIL_OPENFAILD_BYSHOPORTRADE = 26,			// 您正处于买卖或交易状态，不能打开邮箱！
	eIDS_MAIL_SENDTOSELF = 27,						// 对不起，不能给自己发送邮件！
	eIDS_MAIL_GETGOODSFAILED = 28,					// 取附件物品失败，请检查背包是否有空位！
	eIDS_MAIL_GROUPSEND = 29,						// 您确定要花费%d银币群发此邮件？
	eIDS_MAIL_NEWMAIL = 30,							// 您有新的邮件，请注意查收！
	eIDS_MAIL_HAVEGOODSORGOLD = 31,					// 您是否确定在邮件中附赠金币或物品？
	eIDS_MAIL_PAYNOGOLDS = 32,						// 请正确输入付款收信的金额！

	eIDS_MAIL_SEND_ERROR = 50,							// 发送失败提示信息
	eIDS_MAIL_SENDING_FAILED_INVALID_POSTAGE = 51,		// 邮资不够，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_SUBJECT = 52,		// 主题包含非法字符，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_TEXT = 53,			// 内容包含非法字符，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_EXTEXT = 54,		// 附加内容包含非法字符，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_GOLD = 55,			// 金币超出限制，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_GOODS = 56,		// 物品超出限制，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_MAXNAME = 57,		// 名字过长，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_MAXSUBJECT = 58,	// 主题过长，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_MAXTEXT = 59,		// 文字内容过长，发送失败！
	eIDS_MAIL_SENDING_FAILED_INVALID_NOPLAYER = 60,		// 没有找到收信人，发送失败！
	eIDS_MAIL_SENDING_FAILED_NOPLAYERORTITLE = 61,		// 没有收信人或主题，发送失败！


	eIDS_MAIL_CURPAGE = 90,								// 第 %d 页
	eIDS_MAIL_TIME_DAYS = 91,							// %d 天
	eIDS_MAIL_TIME_HOURS = 92,							// %d 小时
	eIDS_MAIL_TIME_MINS = 93,							// %d 分钟

	eIDS_MAIL_ACCEPTPAGE = 99,							// 收件箱
	eIDS_MAIL_SENDPAGE = 100,							// 发件箱
};

enum eIDS_CT_RELIVE
{
	eIDS_RELIVE_FIRSTPAGE     = 1,                        //您已经精疲力尽了
	eIDS_RELIVE_ASKBUYITEM    = 2,                        //询问购买复活道具
	eIDS_RELIVE_RELIVESKILL   = 3,                        //接受复活术
	eIDS_RELIVE_RELIVESKILLEX = 4,                        //接受复活术
	eIDS_RELIVE_BUYITEMFAIL   = 5,                        //身上星钻不足
    eIDS_RELIVE_BUYGOODSNOTE  = 9,                        //复活道具提示
};

enum eIDS_CT_SPRITE
{
	eIDS_SPRITE_PUTDOWNFAIL = 1,                      //放置物品失败 
    eIDS_SPRITE_WEARING     = 2,                      //还剩药个数                               
    eIDS_SPRITE_FAILED_OPEN = 3,                      //打开自动补给界面失败
	eIDS_SPRITE_FAILED_FIGHTAREA = 4,                 //自动寻怪半径范围无效
	eIDS_SPRITE_SAVE = 5,                             //已经保存数据到本地
	eIDS_SPRITE_FAILED_ASSISTPROP = 6,                //请放入有效辅助物品
	eIDS_SPRITE_FAILED_NOTMAPSTABLE = 7,              //此场景不能进行稳定辅助打怪功能
	eIDS_SPRITE_FAILED_NOTMAPCOMMON = 8,              //此场景不能进行普通辅助打怪功能
	eIDS_SPRITE_FAILED_ASSISTSKILL = 9,               //请放入有效辅助技能
	eIDS_SPRITE_PROTECT_STABLE = 10,                  //提示已经进入了稳定辅助保护状态
	eIDS_SPRITE_FAILED_NOSTABLETIME = 11,             //没有足够的稳定辅助时间
	eIDS_SPRITE_FAILED_ENOUGHMONEY = 12,              //没有足够的银币
	eIDS_SPRITE_FAILED_NOCONTROL = 13,                //数据已锁定不能进行修改
	eIDS_SPRITE_FAILED_FIGHTSKILL = 14,               //请放入有效战斗技能.
	eIDS_SPRITE_FAILED_REPEATFIGHTSKILL = 15,         //已有该战斗技能
	eIDS_SPRITE_FAILED_REPEATASSISTSKILL = 16,        //已有该辅助技能
	eIDS_SPRITE_FAILED_REPEATASSISTPROP = 17          //已有该类型道具
};

enum eIDS_CT_TIMEBOX
{
	eIDS_TIMEBOX_NOTE = 1,                           //时间文本框说明
};

enum eIDD_CT_EQUIPWEAR
{
	eIDS_EQUIPWEAR_VERYMANGLE    = 1,
	eIDS_EQUIPWEAR_VERYMANGLEEX  = 2,
	eIDS_EQUIPWEAR_MANGLE        = 3,
	eIDS_EQUIP_SUCCESS		     = 4,				// [%s]已装备！
};

enum eIDS_CT_SYSTEMSETUP
{
	eIDS_SYSTEMSETUP_SETDEFAULT = 1,                      //设置默认
    eIDS_SYSTEMSETUP_SCREEN     = 2,                      //全屏显示 
    eIDS_SYSTEMSETUP_APPLICATION = 3,                     //是否应用
};

enum eIDS_CT_TITLEMENU
{
	eIDS_TITLEMENU_CURTITLE  = 1,                          //当前称号
	eIDS_TITLEMENU_HIDETITLE = 2,                          //不显示称号
	eIDS_TITLEMENU_KNIGHT    = 3,                          //爵位称号
    eIDS_TITLEMENU_FACTION   = 4,                          //帮会称号
    eIDS_TITLEMENU_MEDAL1    = 5,                          //勋章称号1
	eIDS_TITLEMENU_MEDAL2    = 6,                          //勋章称号2
	eIDS_TITLEMENU_MEDAL3    = 7,                          //勋章称号3
};

enum eIDS_CT_SELECTSECTION
{
	eIDS_SELECTSECTION_UNSELECT = 1,                        //没有选择游戏分区
	eIDS_SELECTSECTION_WIRE     = 2,                        //电信
	eIDS_SELECTSECTION_TOIL     = 3,                        //网通 
};

enum eIDS_CT_GAMENOTE
{
	eIDS_GAMENOTE_TEXT = 1,                                //游戏公告
};

enum eIDS_CT_MERCENARY
{
	eIDS_MERCENARY_COMPLATE          = 1,                     //新的刷新已经发布，请点击刷新任务按钮
	eIDS_MERCENARY_UNCOMPLATE             = 2,                     //距离下次任务发布的时间还有
	eIDS_MERCENARY_HAVEITEM   = 3,                           //拥有刷新道具
	eIDS_MERCENARY_UNHAVEITEM = 4,                           //没有刷新道具
	eIDS_MERCENARY_Min               = 5,                     //分钟
	eIDS_MERCENARY_Sec               = 6,                     //秒
	eIDS_MERCENARY_ItemNum           = 7,                     //拥有数量
	eIDS_MERCENARY_QuestHave         = 8,                     //委托失效
	eIDS_MERCENARY_AcceptQuest       = 9,                     //接受任务
	eIDS_MERCENARY_DelQuest          = 10,                    //放弃任务
	eIDS_MERCENARY_AcceptTeamQuest   = 11,                    //接受为全队任务
	eIDS_MERCENARY_Indiv             = 12,                    //个人
	eIDS_MERCENARY_Conti             = 13,                    //共享
	eIDS_MERCENARY_Accept            = 14,                    //委托接受
	eIDS_MERCENARY_Complate          = 15,                    //完成任务
    eIDS_MERCENARY_UnComplated       = 16,                    //未完成
	eIDS_MERCENARY_Complated         = 17,                    //已完成
	eIDS_MERCENARY_Shared            = 18,                    //已推荐
};

enum eIDS_CT_INPUTBOX 
{
	eIDS_INPUTBOX_NUMBER = 1,                                 //请输入一个数字
	eIDS_INPUTBOX_NULL   = 2,                                 //输入不能为空
};

enum eNOTIFY_POS
{
	eNOTIFYPOS_LEFT_BOTTOM=1,//左下
	eNOTIFYPOS_RIGHT_BOTTOM,//右下
	eNOTIFYPOS_CENTER,//中间
	eNOTIFYPOS_SYSTEM,//系统
};

// 游戏外赠送系统
enum ePRESENT
{
	ePRESENT_DATE = 1,					// 过期时间：%04d年%02d月%02d日
	ePRESENT_HAVEPRESENT,				// 您有未领取的奖励，请注意领取!
	ePRESENT_UNCHECKED,					// 您没有选中任何奖励!
	ePRESENT_NOSPACE,					// 您的背包空间不足，请至少空出 1 格背包空间!
	ePRESENT_FROZE,						// 该奖励已被冻结，正在处理中，请稍侯再试!
	ePRESENT_DREW,						// 该奖励已被领取，不能再领!
	ePRESENT_OVERDUE,					// 该奖励已过期，不能再领取!
	ePRESENT_UNSUCCESS,					// 领取失败!
	ePRESENT_SUCCESS,					// 领取成功!
};

enum eReportInfo
{
	eTipInfo1=1,//提示信息1
	eTipInfo11=11,

	eTipOther=31,
};