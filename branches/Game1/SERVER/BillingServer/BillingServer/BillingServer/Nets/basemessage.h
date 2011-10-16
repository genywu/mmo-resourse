#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MsgQueue.h"

class CMySocket;
class CMessage;
class CGUID;

// 反馈信息列表
enum
{
	LOGIN_OK = 1,					// 登陆成功
	LOGIN_LOGINSERVEROK,
	LOGIN_GAMESERVEROK,
	LOGIN_VERSIONERROR,				// 游戏版本错误
	LOGIN_NOTCDKEY,					// 没有这个CDKEY
	LOGIN_ACC_LOCKED,				// 帐号被锁定
	LOGIN_PASSWORDERROR,			// 密码错误
	LOGIN_CHECKOUT,					// 以有玩家登陆游戏
	LOGIN_NOTLOGINSERVER,           //没有登陆登陆服务器,

	LOGIN_PLAYER_ALREAD_IN_GAME,	// 该玩家已经在游戏中了
	LOGIN_PLAYER_LOAD_ERROR,		// 读取玩家数据失败
	LOGIN_PLAYER_IN_OTHER_SERVER,	// 玩家被其他服务器登出了
	LOGIN_PLAYER_NO_REGION,			// 没有找到玩家的场景
	WORLD_REQUESTRGIONSERVEROK,
	LOGIN_CDKEY_IN_OTHER_SERVER,	// CDKEY被其他服务器登出了
	LOGIN_BEBANED,					// 此CDKEY被封
	LOGIN_CDKEYDISABLE,				// cdkey被禁用
	LOGIN_IP_FORBID,				// ip被限制
	LOGIN_DELETEROLE_FAILED,        //删除角色成功
	LOGIN_DELETEROLE_SUCCEED,       //删除角色失败
	LOGIN_RESTOREROLE_SUCCEED,		// 恢复角色成功
	LOGIN_RESTOREROLE_FAILED,		// 恢复角色失败
	LOGIN_CREATEROLE_NAMEEXIST,     //创建角色失败(玩家名字已经存在)
	LOGIN_CREATEROLE_DBERROR,		//创建角色失败(数据库操作失败)
	LOGIN_CREATEROLE_FULL,			//创建角色失败(角色建满失败)
	LOGIN_CREATEROLE_NAMEINVALID,	//名字INVALID，具有色情或政治上的INVALID字符。
	LOGIN_CREATEROLE_SUCCEED,		//创建角色成功
	LOGIN_QUEST_PLAYERDATA_FAILED,	//请求详细信息失败
	LOGIN_QUEST_PLAYERDATA_SUCCEED,	//请求详细信息成功
	GOODS_FETCH_SUCCEED,			//拿物品成功
	GOODS_FETCH_FAILED,				//拿物品失败
	GOODS_PUTDOWN_SUCCEED,			//放物品成功
	GOODS_PUTDOWN_FAILED,			//放物品失败
	GOODS_CHANGE_SUCCEED,			//调换物品成功
	GOODS_CHANGE_FAILED,			//调换物品失败
	GOODS_DROP_SUCCEED,				//扔物品成功
	GOODS_DROP_FAILED,				//扔物品失败
	GOODS_PICK_SUCCEED,				//拣物品成功
	GOODS_PICK_NOTSPACE,			//拣物品失败(原因:背包内没有足够空间)
	GOODS_PICK_NOTFOUND,			//拣物品失败(原因:找不到要拾取的物品)
	GOODS_PICK_PROTECTED,			//拣物品失败(原因:物品被保护)
	GOODS_PICK_TOOHEAVY,			//拣物品失败(原因:超过负重)
	GOODS_PICK_NEW,
	GOODS_PICK_OVERLAP,
	HOTKEY_SET_SUCCEED,				//热键设置成功
	HOTKEY_SET_FAILED,				//热键设置失败
	HOTKEY_CLEAR_SUCCEED,			//热键取消成功
	HOTKEY_CLEAR_FAILED,			//热键取消失败
	HOTKEY_EXCHANGE_SUCCEED,		//热键交换成功
	HOTKEY_EXCHANGE_FAILED,			//热键交换失败
	PLAYER_USEITEM_SUCCEED,			//玩家使用道具成功
	PLAYER_USEITEM_FAILED,			//玩家使用道具失败

	PLAYER_USEITEM_SKILLTOOLOW,		//玩家使用道具学习技能，因等级低于现有技能等级而失败
	PLAYER_USEITEM_SKILLTOOHIGH,	//玩家使用道具学习技能，因跳级别学习技能而失败
	SHOP_BUY_SUCCEED,				
	SHOP_BUY_MONEYNOTENOUGH,
	SHOP_BUY_GOODSNOTFOUND,
	SHOP_REPAIR_SUCCEED,
	SHOP_REPAIR_MONEYNOTENOUGH,
	SHOP_REPAIR_FAILED,
	TRADE_SUCCEED,
	TRADE_FAILED,


	LOGINDB_ERROR,
	WORLDDB_ERROR,

	LOGIN_CREATEROLE_FAILED,		//创建角色失败

	LOGIN_MATRIX_POS,				//合法验证矩阵卡者,
	LOGIN_MATRIX_RES_OK,
	LOGIN_MATRIX_RES_FAILED,
	LOGIN_MATRIX_RES_TIMEOUT,
	LOGIN_MATRIX_RES_KICKOUT,

	LOGIN_EKey_Err,
	LOGIN_TimeOut,  

    LOGIN_VALID_CODE,           // 发送验证码
    LOGIN_VALID_CODE_OK,        // 验证码正确
    LOGIN_VALID_CODE_FAILED,    // 验证码错误
    LOGIN_VALID_CODE_OVERTIME,  // 验证超时
    LOGIN_VALID_CODE_KICKOUT,   // 同帐号在其他地方登录并进入验证码界面
    LOGIN_VALID_CODE_SEND_FREQUENT, // 验证码发送频繁
    LOGIN_VALID_CODE_SEND_FAILED,   // 验证码发送失败

    LOGIN_FAILED_MANY_TIMES,    // 验证失败太多次

	// gongran sina 2008-3-3
	//LOGIN_UTG_PASSWORD_ERROR,	// 密码错误
	//LOGIN_UTG_ACC_FORBIDDEN,	// 用户被停权
	//LOGIN_UTG_ACC_INEXISTENT,	// 用户不存在
	//LOGIN_UTG_SERVER_INFO_ERROR,// 服务器信息错误
	//LOGIN_UTG_TIME_OUT,			// 操作超时
};

/* -----------------------------------------------------
[CMessage] 消息基类

功能：
游戏的消息都从本类派生
包含每个消息的基本内容（大小、类型和时间）
提供基本的组装消息、发送消息功能

-------------------------------------------------------*/
class CBaseMessage
{
public:

	// 例：发送一个消息
	// CMessage msg(QUEST_JOIN);
	// msg.Add(long)；						// 其他东西
	// msg.Add(char*);
	// msg.Send();							// 发送

	enum
	{
		MAX_SUB			= 0x100,				// 一个小类型的最大消息数量
		MAX_TYPE		= MAX_SUB * 0xff,		// 一个大类型的最大消息数量
	};

	enum
	{
		//////////////////
		//				//
		//	LoginServer	//
		//				//
		//////////////////

		//========================================================================================================
		MSG_L2L = MAX_TYPE * 1,			// LoginServer --> LoginServer
		//========================================================================================================

		MSG_L2L_SERVER				= MSG_L2L + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_L2L_SERVER_WORLDSERVERLOST,							// LoginServer监测到WorldServer断开

		MSG_L2L_LOG					= MSG_L2L + MAX_SUB * 1,
		// ----------------------------------------------------
		MSG_L2L_LOG_CLIENTCLOSE,								// LoginServer监测到Client断开

		//========================================================================================================
		MSG_W2L = MAX_TYPE * 2,			// WorldServer --> LoginServer
		//========================================================================================================

		MSG_W2L_SERVER				= MSG_W2L + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_W2L_SERVER_SENDWORLDINFO,							// 发送WORLD信息给LoginServer
		MSG_W2L_SERVER_SENDCDKEYLIST,							// 重连时发送World上的Cdkeylist到LoginServer
		MSG_W2L_SERVER_CLEARCDKEYS,								// gameserver断开,worldserver向loginserver发送需要清除的CDKEY

		MSG_W2L_SERVER_ANSWER_PING,								// ping服务器的信息，也是检测服务器是否正常

		MSG_W2L_SERVER_LOG_GS_CONNECT,
		MSG_W2L_SERVER_LOG_RELOAD_CONF,

		MSG_W2L_SERVER_Response_Info,							// 在线人数
		MSG_W2L_SERVER_Log_Error,								// 记录服务器错误日志

		MSG_W2L_LOG					= MSG_W2L + MAX_SUB * 1,
		// ----------------------------------------------------
		MSG_W2L_LOG_ANSWER_PLAYERDATA,							// worldserver向loginserver发送详细信息获取是否成功
		MSG_W2L_LOG_ANSWER_PLAYERBASELIST,						// worldserver向loginserver返回玩家基本信息列表
		MSG_W2L_LOG_DELETEROLE,									// worldserver返回角色是否成功删除
		MSG_W2L_LOG_RESTOREROLE,								// WORLDSERVER返回角色是否恢复
		MSG_W2L_LOG_CREATEROLE,									// worldserver返回角色是否成功创建
		MSG_W2L_LOG_PLAYERQUIT,									// worldserver向loginserver发送退出玩家的cdkey

		MSG_W2L_LOG_SENDACCOUNTINFO,							// 发送帐号的信息给客户段

		MSG_W2L_GM					= MSG_W2L + MAX_SUB * 2,
		// ----------------------------------------------------
		MSG_W2L_GM_BANPLAYER,

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool BEGIN
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MSG_W2L_GMA = MSG_W2L + MAX_SUB * 3,

		MSG_W2L_GMA_RESPONSE_KICKPLAYER,
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool END
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//========================================================================================================
		MSG_C2L = MAX_TYPE * 3,			// Client --> LoginServer
		//========================================================================================================

		MSG_C2L_LOG					= MSG_C2L + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_C2L_LOG_QUESTCDKEY,									// 客户端登陆时请求CDKEY
		MSG_C2L_LOG_QUEST_PLAYERBASELIST,						// 客户端登陆时请求帐号下的玩家基本数据列表
		MSG_C2L_LOG_QUEST_PLAYERDATA,							// 向LOGINSERVER请求详细数据
		MSG_C2L_LOG_QUESTCREATE,								// 客户端要求建立一个新的角色
		MSG_C2L_LOG_QUESTDELETE,								// 客户端要求删除一个角色
		MSG_C2L_LOG_QUESTRESTORE,								// CLIENT 请求恢复一个角色
		MSG_C2L_LOG_QUESTEXIT,									// 连接GameServer的结果
		MSG_C2L_LOG_VALIDATE_MATRIX,							// 请求验证矩阵卡
        MSG_C2L_LOG_QUEST_VALID_CODE,                           // 请求服务器验证验证码
        MSG_C2L_LOG_QUEST_VALID_CODE_ANOTHER,                   // 请求服务器重发验证码


		//////////////////
		//				//
		//	WorldServer	//
		//				//
		//////////////////

		//========================================================================================================
		MSG_W2W = MAX_TYPE * 4,			// WorldServer --> WorldServer
		//========================================================================================================

		MSG_W2W_SERVER					= MSG_W2W + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_W2W_SERVER_LOGINSERVERCLOSE,						// WorldServer检测到LoginServer关闭
		MSG_W2W_SERVER_GAMESERVERLOST,							// WorldServer检测到GameServer断开
		MSG_W2W_RECONNECTLOGINSERVEROK,							// 重连LoginServer成功

		//========================================================================================================
		MSG_L2W = MAX_TYPE * 5,			// LoginServer --> WorldServer
		//========================================================================================================

		MSG_L2W_LOG					= MSG_L2W + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_L2W_LOG_QUEST_PLAYERBASELIST,						// loginserver向world请求玩家基本信息列表
		MSG_L2W_LOG_DELETEROLE,									// loginserver向worldserver转发删除角色请求
		MSG_L2W_LOG_RESTOREROLE,								// LOGINSERVER向worldserver转发恢复角色请求
		MSG_L2W_LOG_CREATEROLE,									// loginserver向worldserver转发创建角色请求
		MSG_L2W_LOG_QUEST_PLAYERDATA,							// loginserver向worldserver转发获取详细信息请求
		MSG_L2W_LOG_KICKPLAYER,                                 // loginServer向WorldServer发送一条要求踢出某个玩家的消息
		MSG_L2W_LOG_REPEAT_LOGIN,								// Login Server通知World Server有一连接请求登陆一帐号并替换掉原来使用该帐号的连接。

		MSG_L2W_SERVER				= MSG_L2W + MAX_SUB * 1,
		MSG_L2W_SERVER_QUEST_PING,								// ping服务器的信息，也是检测服务器是否正常
		MSG_L2W_SERVER_Request_Info,							// 获取GS人数
		MSG_L2W_SERVER_SendSetup,

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool BEGIN
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MSG_L2W_GMA = MSG_L2W + MAX_SUB * 2,
		
		MSG_L2W_GMA_KICKPLAYER,
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool END
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//========================================================================================================
		MSG_S2W = MAX_TYPE * 6,			// GameServer --> WorldServer
		//========================================================================================================

		MSG_S2W_SERVER					= MSG_S2W + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_S2W_SERVER_SENDSERVERINFO,							// 发送server信息给WorldServer
		MSG_S2W_SERVER_CHANGEREGION,							// 请求切换服务器
		MSG_S2W_SERVER_SAVERESPONSE,							// 发送存盘点信息
		MSG_S2W_SERVER_ADDINFO,									// 玩家不在当前服务器，请求World Server向玩家对话框加入一条信息
		MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE,					// 改变脚本变量的值
		MSG_S2W_SERVER_REPORT_MURDERER,							// 报告杀人者
		MSG_S2W_SERVER_UpdateRegionInfo,						// 保存场景信息
		MSG_S2W_SERVER_UPDATEGOODSID,							// 同步GoodsID

		MSG_S2W_SERVER_UPDATEPLAYERDATA,						// 同步单个在线玩家的数据
		MSG_S2W_SERVER_ANSWER_PING,								// ping服务器的信息，也是检测服务器是否正常
		MSG_S2W_SERVER_CREATEMONSTER,							// 创建怪物
		MSG_S2W_SERVER_Response_Info,

		MSG_S2W_SERVER_Log_Error,								// 服务器错误日志

		MSG_S2W_LOG						= MSG_S2W + MAX_SUB * 1,
		// ----------------------------------------------------
		MSG_S2W_LOG_QUEST_PLAYERDATA,							// GameServer向WorldServer请求player详细数据
		MSG_S2W_LOG_PLAYERQUIT,									// gameserver向worldserver发送退出玩家的playerid

		MSG_S2W_PLAYER					= MSG_S2W + MAX_SUB * 2,
		// ----------------------------------------------------
		MSG_S2W_PLAYER_ADDSKILL,							
		MSG_S2W_PLAYER_DELSKILL,
		MSG_S2W_PLAYER_DELGOODS,
		MSG_S2W_PLAYER_SETLEVEL,



		MSG_S2W_OTHER					= MSG_S2W + MAX_SUB * 3,
		// ----------------------------------------------------
		MSG_S2W_OTHER_TALK,										// 对话消息
		MSG_S2W_OTHER_ADDINFO,							
		MSG_S2W_OTHER_GOODSLINK,                                // gameserver发送一个物品超链到worldserver
		MSG_S2W_OTHER_VIEWGOODSLINK,				

		MSG_S2W_OTHER_CHANGENAME,								// Gs转发C发出的更名请求到Ws
		MSG_S2W_OTHER_BROADCAST,								// 广播消息

		MSG_S2W_OTHER_TALK_WORLD,								// 世界喊话
		MSG_S2W_OTHER_TALK_COUNTRY,								// 国家喊话

		MSG_S2W_OTHER_TALK_NOTIFY,								// 国家通知

		MSG_S2W_OTHER_GETLOG_REQUEST,							// 请求增值日值

		MSG_S2W_DEPOT					= MSG_S2W + MAX_SUB * 4,
		// ----------------------------------------------------
		MSG_S2W_DEPOT_OPEN,										// 开仓库

		MSG_S2W_GM						= MSG_S2W + MAX_SUB * 5,
		// ----------------------------------------------------
		MSG_S2W_GM_QUERY_PLAYERCOUNT,							//查询用户数量
		MSG_S2W_GM_QUERY_PLAYERDATA,							//查询用户数据
		MSG_S2W_GM_ANSWER_PLAYERDATA,					
		MSG_S2W_GM_QUERY_FINDREGION,							//查询地图		
		MSG_S2W_GM_QUERY_PLAYERONLINE,							//查询用户在线状况
		MSG_S2W_GM_RELOAD,										// 重读设置文件
		MSG_S2W_GM_KICKPLAYER,									// 踢玩家下线
		MSG_S2W_GM_KICKPLAYEREX,								// 超级踢 gameserver 2 worldserver
		MSG_S2W_GM_ANSWER_KICKPLAYER,							// 应答踢玩家下线
		MSG_S2W_GM_KICK_ALLPLAYER,								// 踢所有玩家下线
		MSG_S2W_GM_KICK_REGIONPLAYER,							// 踢一个场景中的玩家下线
		MSG_S2W_GM_SILENCE_PLAYER,								// 请求将一个玩家禁言
		MSG_S2W_GM_ANSWER_SILENCE_PLAYER,						// 答复将一个玩家禁言
		MSG_S2W_GM_WORLD_MESSAGE,								// 向整个世界的玩家发送消息
		MSG_S2W_GM_QUERY_SILENCE_PLAYERS,						// 向世界服务器查询禁言的玩家
		MSG_S2W_GM_ANSWER_QUERY_SILENCE_PLAYERS,				// 向世界服务器返回禁言的玩家列表
		MSG_S2W_GM_CHANGEREGION,
		MSG_S2W_GM_BANPLAYER,
		MSG_S2W_GM_SAVEPLAYER,
		MSG_S2W_GM_QUERYGM,
		MSG_S2W_GM_ANSWERGM,
		//MSG_S2W_GM_TEAMCHANGEREGION,							//队伍切换场景
		MSG_S2W_GM_COUNTRY_MESSAGE,								// 向整个国家的玩家发送消息

		MSG_S2W_TEAM					= MSG_S2W + MAX_SUB * 6,
		// ----------------------------------------------------
		MSG_S2W_TEAM_CREATE,
		MSG_S2W_TEAM_DESTORY,
		MSG_S2W_TEAM_TEAMATE_JOIN,
		MSG_S2W_TEAM_TEAMATE_LEAVE,
		MSG_S2W_TEAM_TEAMATE_CHANGE_REGION,
		MSG_S2W_TEAM_CHANGE_LEAER,									//改变队长
		MSG_S2W_TEAM_KICK_PLAYER,
		MSG_S2W_TEAM_QUEST_SYNCHRONIZE,								//请求同步
		MSG_S2W_TEAM_QUERY_PLAYER_RESULT,							//查询玩家是否存在的结果返回.
		MSG_S2W_TEAM_CHANGE_ALLOCATION_SCHEME,						//改变分配方式.
		MSG_S2W_TEAM_CHAT,
		MSG_S2W_TEAM_CHANGE_STATE,									//学量改变

		//MSG_S2W_TEAM_QUERY_TEAMID,								//寻问队伍ID 只在真正创建用
		//MSG_S2W_TEAM_TEAMMATE_JOIN,								//队员加入
		//MSG_S2W_TEAM_TEAMMATE_QUIT,								//队员离队
		//MSG_S2W_TEAM_TEAMMATE_KICK,								//踢人
		//MSG_S2W_TEAM_TEAMMATE_MIGRATE_OUT,						//转移
		//MSG_S2W_TEAM_TEAMMATE_MIGRATE_IN,						//转移
		//MSG_S2W_TEAM_UPLOAD,									//上传队伍信息	
		//MSG_S2W_TEAM_QUERY_SIZE,								//询问队伍大小
		//MSG_S2W_TEAM_MISS_CAPTAIN_INFO,							//丢失队长信息	
		//MSG_S2W_TEAM_CHANGE_CAPTAIN,							//任命队长	
		//MSG_S2W_TEAM_SPREAD_CAPTAIN_CHANGE,						//发布队长信息
		//MSG_S2W_TEAM_CHAT,
		//MSG_S2W_TEAM_CHANGEREGION,								//队伍切换场景

		MSG_S2W_ORGSYS					= MSG_S2W + MAX_SUB * 7,
		// ----------------------------------------------------
		MSG_S2W_ORGSYS_OnPlayerDied,
		MSG_S2W_ORGSYS_OnDeleteRole,
		MSG_S2W_ORGSYS_CREATEFACTION,							//创建帮会
		MSG_S2W_ORGSYS_GetOrganizing,							//得到帮会、同盟
		MSG_S2W_ORGSYS_GetFaction,								//得到帮会
		MSG_S2W_ORGSYS_GetUnion,								//得到帮会
		MSG_S2W_ORGSYS_GETFACTIONLIST,							//得到帮会列表
		MSG_S2W_ORGSYS_APPLYJOINFACTION,						//申请加入帮会
		MSG_S2W_ORGSYS_CANCELJOINFACTION,						//取消加入帮会
		MSG_S2W_ORGSYS_DOJOINFACTION,							//加入帮会处理
		MSG_S2W_ORGSYS_FireOutFactionMember,					//开除帮员
		MSG_S2W_ORGSYS_FireOutUnionMember,						//开除盟员
		MSG_S2W_ORGSYS_ExitFaction,								//退出帮会
		MSG_S2W_ORGSYS_ExitUnion,								//退出同盟
		MSG_S2W_ORGSYS_Demise,									//帮主传位
		MSG_S2W_ORGSYS_UnionDemise,								//盟主传位
		MSG_S2W_ORGSYS_Disband,									//解散帮会
		MSG_S2W_ORGSYS_DisbandUnion,							//解散同盟
		MSG_S2W_ORGSYS_DubJobLvl,								//授予称号和帮会职级
		MSG_S2W_ORGSYS_EndueRight,								//授予权限
		MSG_S2W_ORGSYS_AbolishRight,							//取消权限
		MSG_S2W_ORGSYS_RequestEstaUnion,						//请求建立同盟
		MSG_S2W_ORGSYS_AnswerAffirmEstaUnion,					//回复确认建立同盟
		MSG_S2W_ORGSYS_InviteJoinUnion,							//邀请加入同盟
		MSG_S2W_ORGSYS_AnswerAffirmJoinUnion,					//回复确认邀请同盟
		MSG_S2W_ORGSYS_TurnOnLeaveWord,							//开启留言功能
		MSG_S2W_ORGSYS_LeaveWord,								//留言
		MSG_S2W_ORGSYS_DelLeaveWord,							//删除留言
		MSG_S2W_ORGSYS_EditPronounce,							//编辑公告
		MSG_S2W_FACTIONWAR_GETFACTIONLIST,						//帮战得到帮会列表
		MSG_S2W_ORGSYS_DigUpTheHatchet,							//宣战
		MSG_S2W_ORGSYS_AnswerAffirmDigUpTheHatchet,				//回复确认宣战
		MSG_S2W_ORGSYS_MakePeace,								//议和
		MSG_S2W_ORGSYS_AnswerAffirmMakePeace,					//回复确认议和
		MSG_S2W_ORGSYS_Surrender,								//投降
		MSG_S2W_ORGSYS_AnswerAffirmSurrender,					//回复确认投降
		MSG_S2W_ORGSYS_RequestGetFactionBillboard,				//得到帮会排行榜
		MSG_S2W_ORGSYS_UpgradeFaction,							//申请升级帮会
		MSG_S2W_ORGSYS_UploadFactionIcon,						//上传帮会徽标
		MSG_S2W_ORGSYS_SetFactionControbute,					//设置帮会的贡献者
		MSG_S2W_ORGSYS_FactionIncExp,							//给帮会增加经验
		MSG_S2W_ORGSYS_PlayerInfoChange,						//玩家的相关信息改变了
		MSG_S2W_CITY_RequestObtainTaxPayment,					//获取税金
		MSG_S2W_CITY_RequestAdjustTaxRate,						//调整税率
		MSG_S2W_CITY_UpdateTaxRate,								//更新税收
		MSG_S2W_CITY_AddTaxMoney,								//给某个场景添加税收
		MSG_S2W_CITY_OperatorCityGate,							//操作城门
		MSG_S2W_CITY_Transfer,									//转让主城
		MSG_S2W_CITY_AnswerAffirmTransfere,						//回复确认转让主城
		MSG_S2W_CITY_SetPermit,									//设置是否允许同盟、宣战
		MSG_S2W_ATTACKCITY_OnResult,							//发送标志物摧毁
		MSG_S2W_ATTACKCITY_AskAttackCityTime,					//询问攻城时间
		MSG_S2W_ApplyForVillageWar,								//申请参加争夺据点战
		MSG_S2W_WAR_FactionWinVillage,							//村战中一个争夺胜利
		MSG_S2W_CITYWAR_DeclareWarc,							//国王争霸战宣战
		MSG_S2W_WAR_FactionWinCity,								//国王争霸战中一个争夺胜利

		//任务相关
		MSG_S2W_QUEST_PlayerAddQuest,							//给某个玩家添加一个任务
		MSG_S2W_QUEST_PlayerRemoveQuest,						//给某个玩家移出一个任务
		MSG_S2W_QUEST_PlayerRunScript,							//给一个玩家触发一个脚本

		MSG_S2W_SetFacParam_ByMasterPlayerID,					//设置帮会参数值
		MSG_S2W_SetFacParam_ByFactionID,
		MSG_S2W_SetFacParam_ByFactionName,
		MSG_S2W_ChangeFacParam_ByMasterPlayerID,				//改变帮会参数值
		MSG_S2W_ChangeFacParam_ByFactionID,
		MSG_S2W_ChangeFacParam_ByFactionName,


		// 日志
		MSG_S2W_WRITELOG				= MSG_S2W + MAX_SUB * 8,//日志
		// ----------------------------------------------------
		MSG_S2W_WRITELOG_GOODSTRADE,							//交易物品
		MSG_S2W_WRITELOG_GOODS,									//物品变动（包括拾起，丢弃，暴）
		MSG_S2W_WRITELOG_GOODSUPGRADE,							// 物品升级
		MSG_S2W_WRITELOG_GOODSGEMEXCHANGE,						// 对话宝石
		MSG_S2W_WRITELOG_GOODSJEWELRYMADE,						// 制作首饰

		MSG_S2W_WRITELOG_PLAYERLEVEL,							//角色升级
		MSG_S2W_WRITELOG_PLAYEREXP,								//经验变动（打怪增加，死亡减少）
		//MSG_S2W_WRITELOG_PLAYERDELETED,						//角色删除
		MSG_S2W_WRITELOG_PLAYERDIED,							//玩家角色死亡日志

		MSG_S2W_WRITELOG_TEAM,									//队伍日志（创建和解散）
		MSG_S2W_WRITELOG_PLAYERKILLER,							//玩家PK

		MSG_S2W_WRITELOG_CHAT,									//聊天日志
		MSG_S2W_WRITELOG_CHANGEMAP,								//切换场景日志

		MSG_S2W_WRITELOG_INCREMENT,								// 增值日志

		MSG_S2W_WRITELOG_LOG,									// 自定义日志

		// 国家
		MSG_S2W_COUNTRY					= MSG_S2W + MAX_SUB * 9,
		// -----------------------------------------------------
		MSG_S2W_COUNTRY_CHANGE,									// 改变国籍
		MSG_S2W_COUNTRY_POWER_CHANGE,
		MSG_S2W_COUNTRY_TECHLEVEL_CHANGE,
		MSG_S2W_COUNTRY_CHANGE_CI,

		//////////////////////////////////////////////////////////////////
		// 国王权限 S2W yulefox
		MSG_S2W_COUNTRY_KING_GETSTATE,							// 国王操作 : 获取当前状态（是否是国王争夺战状态）
		MSG_S2W_COUNTRY_KING_GetInfo,							// 国王操作 : 获取所有国家信息
		MSG_S2W_COUNTRY_KING_GetPlayerList,						// 国王操作 : 获取在线玩家列表
		MSG_S2W_COUNTRY_KING_Demise,							// 国王操作 : 禅让
		MSG_S2W_COUNTRY_KING_Appoint,							// 国王操作 : 任命
		MSG_S2W_COUNTRY_KING_Depose,							// 国王操作 : 撤职
		MSG_S2W_COUNTRY_KING_Absolve,							// 国王操作 : 赦免
		MSG_S2W_COUNTRY_KING_Silence,							// 国王操作 : 禁言
		MSG_S2W_COUNTRY_KING_Exile,								// 国王操作 : 流放
		MSG_S2W_COUNTRY_KING_SuccessExiled,						// 国王操作 : 流放结果
		MSG_S2W_COUNTRY_KING_Convone,							// 国王操作 : 召集
		MSG_S2W_COUNTRY_COUNTRY_Message,						// 国家信息公告
		MSG_S2W_COUNTRY_WORLD_Message,							// 世界信息公告
		MSG_S2W_COUNTRY_PRIVATE_Message,						// 个人信息公告
		MSG_S2W_COUNTRY_SetNewDay,								// 设置新的一天
		MSG_S2W_COUNTRY_ChangeAttribute,						// 设置新的一天
		MSG_S2W_COUNTRY_ReleaseQuest,							// 发布国家任务


		// 国战部分
		MSG_S2W_COUNTRY_WAR_Declare_Quest,						// 某国向他家宣战
		MSG_S2W_COUNTRY_WAR_FlagDestroy,						// 旗帜被毁灭

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool BEGIN
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MSG_S2W_GMA = MSG_S2W + MAX_SUB * 10,
		MSG_S2W_GMA_RESPONSE_KICKPLAYER, 
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool END
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////
		//				//
		//	GameServer	//
		//				//
		//////////////////

		//========================================================================================================
		MSG_S2S = MAX_TYPE * 7,			// GameServer --> GameServer
		//========================================================================================================

		MSG_S2S_SERVER					= MSG_S2S + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_S2S_SERVER_WORLDSERVERCLOSE,						// GameServer检测到WorldServer关闭
		MSG_S2S_SERVER_RECONNECTWORLDSERVER,					// 请求重连WorldServer
		MSG_S2S_SERVER_BILLINGSERVERCLOSE,						// GameServer检测到BillingServer关闭
		MSG_S2S_SERVER_RECONNECTBILLINGSERVER,					//请求重连BillingServer

		MSG_S2S_LOG						= MSG_S2S + MAX_SUB * 1,
		// ----------------------------------------------------
		MSG_S2S_LOG_PLAYER_ONLOST,								// 服务器在接受到客户端断线消息时给自己发一条消息	

		MSG_S2S_TEAM					= MSG_S2S + MAX_SUB * 2,
		// ----------------------------------------------------
		//MSG_S2S_TEAM_TEAMMATE_MIGRATE_IN,						//转移
		//MSG_S2S_TEAM_TEAMMATE_MIGRATE_OUT,						//转移
		//MSG_S2S_TEAM_TEAMMATE_LOST,								//队员离队

		// ---------------------------------------------------- 统一计费消息
		MSG_S2S_UNIBILL					= MSG_S2S + MAX_SUB * 3,
		// ----------------------------------------------------
		MSG_S2S_UNIBILL_AccountAuthenRes,						// 帐号信息查询
		MSG_S2S_UNIBILL_ConsignTransferRes,						// C2C交易成功扣费
		MSG_S2S_UNIBILL_AccountLockRes,							// C2C交易成功预冻
		MSG_S2S_UNIBILL_AccountLockExRes,						// B2C交易成功预冻
		MSG_S2S_UNIBILL_AccountUnlockRes,						// C2C交易成功解冻
		MSG_S2S_UNIBILL_AccountUnlockExRes,						// B2C交易成功解冻
		MSG_S2S_UNIBILL_AwardAuthenRes,							// 领奖查询
		MSG_S2S_UNIBILL_AwardAckRes,							// 领奖确认

		//========================================================================================================
		MSG_W2S = MAX_TYPE * 8,			// WorldServer --> GameServer
		//========================================================================================================

		MSG_W2S_SERVER					= MSG_W2S + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_W2S_SERVER_SENDSETUP,								// WorldServer发送setup设置信息给GameServer
		MSG_W2S_SERVER_CHANGEREGION,							// 请求切换服务器
		MSG_W2S_SERVER_SAVENOTIFY,								// 存盘点通知
		MSG_W2S_SERVER_ADDINFO,									// 请求World Server向GAME SERVER返回结果
		MSG_W2S_SERVER_CHANGE_SCRIPT_VARIABLE,					// 改变脚本变量的值
		MSG_W2S_SERVER_SYNC_MURDERER_COUNT,						// 同步杀人者
		MSG_W2S_UPDATE_STRINGTABLE,								// 更新所有GameServer上的语言包。

		MSG_W2S_SERVER_QUEST_COLLECT_PLAYERDATA,				// 请求GameServer把在线玩家的数据发送过来
		MSG_W2S_SERVER_QUEST_PING,								// ping服务器的信息，也是检测服务器是否正常
		MSG_W2S_SERVER_CREATEMONSTER,							// 创建怪物
		MSG_W2S_SERVER_Request_Info,							// 在线人数

		MSG_W2S_LOG						= MSG_W2S + MAX_SUB * 1,
		// ----------------------------------------------------
		MSG_W2S_LOG_ANSWER_PLAYERDATA,							// WorldServer向GameServer返回player详细数据
		MSG_W2S_LOG_KICKPLAYER,									//worldserver向gameserver发送一条要求踢出某个玩家的消息
		MSG_W2S_LOG_REPEAT_LOGIN,								//World Server通知Game Server有一连接请求登陆一帐号并替换掉原来使用该帐号的连接。

		MSG_W2S_LOG_PLAYER_ONLINE,								// 玩家好友上线
		MSG_W2S_LOG_PLAYER_OFFLINE,								// 玩家好友下线

		MSG_W2S_OTHER					= MSG_W2S + MAX_SUB * 2,
		// ----------------------------------------------------
		MSG_W2S_OTHER_TALK,										// 对话消息
		MSG_W2S_OTHER_FACTIONTALK,								// 帮会聊天
		MSG_W2S_OTHER_BROADCAST,								// worldserver通知gameserver发广播给client
		MSG_W2S_OTHER_AddTopInfo,								// 向客户端添加顶端提示信息
		MSG_W2S_OTHER_ADDINFO,
		MSG_W2S_OTHER_GOODSLINK,							
		MSG_W2S_OTHER_VIEWGOODSLINK,
		MSG_W2S_OTHER_ADDSKILL,
		MSG_W2S_OTHER_DELSKILL,
		MSG_W2S_OTHER_DELGOODS,
		MSG_W2S_OTHER_SETLEVEL,
		MSG_W2S_OTHER_KICKALL,									// 从WorldServer直接踢世界所有人，包括GM


		MSG_W2S_OTHER_CHANGENAME,								// Ws更名后发送结果到Gs
		MSG_W2S_OTHER_TALK_WORLD,								// 世界喊话
		MSG_W2S_OTHER_TALK_COUNTRY,								// 世界喊话

		MSG_W2S_OTHER_TALK_NOTIFY,								// 国家通知

		MSG_W2S_OTHER_GETLOG_RESPONSE,							// 返回给gs增值日志

		MSG_W2S_OTHER_TIMETORETURN,								// 场景里所以玩家定时回城

		MSG_W2S_DEPOT					= MSG_W2S + MAX_SUB * 3,
		// ----------------------------------------------------
		MSG_W2S_DEPOT_OPEN,

		MSG_W2S_GM						= MSG_W2S + MAX_SUB * 4,
		// ----------------------------------------------------
		MSG_W2S_GM_ANSWER_PLAYERCOUNT,							//返回用户数量
		MSG_W2S_GM_ANSWER_PLAYERDATA,							//返回用户数据
		MSG_W2S_GM_QUERY_PLAYERDATA,
		MSG_W2S_GM_ANSWER_FINDREGION,
		MSG_W2S_GM_ANSWER_PLAYERONLINE,							//返回用户在线状况
		MSG_W2S_GM_KICKPLAYER,									// 踢玩家下线
		MSG_W2S_GM_KICKPLAYEREX,								// 超级踢 worldserver 2 gameserver
		MSG_W2S_GM_ANSWER_KICKPLAYER,							// 应答踢玩家下线
		MSG_W2S_GM_KICK_ALLPLAYER,								// 踢所有玩家下线
		MSG_W2S_GM_KICK_REGIONPLAYER,							// 踢一个场景中的玩家下线
		MSG_W2S_GM_SILENCE_PLAYER,								// 请求将一个玩家禁言
		MSG_W2S_GM_ANSWER_SILENCE_PLAYER,						// 答复将一个玩家禁言
		MSG_W2S_GM_WORLD_MESSAGE,								// 向整个世界的玩家发送消息
		MSG_W2S_GM_QUERY_SILENCE_PLAYERS,						// 向游戏服务器查询禁言的玩家
		MSG_W2S_GM_ANSWER_QUERY_SILENCE_PLAYERS,				// 向世界服务器返回禁言的玩家列表
		MSG_W2S_GM_CHANGEREGION,
		MSG_W2S_GM_QUERYGM,
		MSG_W2S_GM_ANSWERGM,
		MSG_W2S_GM_COUNTRY_MESSAGE,								// 想指定国家发送消息

		MSG_W2S_TEAM					= MSG_W2S + MAX_SUB * 5,
		// ----------------------------------------------------
		MSG_W2S_TEAM_CREATE,
		MSG_W2S_TEAM_DESTORY,
		MSG_W2S_TEAM_TEAMATE_JOIN,
		MSG_W2S_TEAM_TEAMATE_LEAVE,
		MSG_W2S_TEAM_TEAMATE_CHANGE_REGION,
		MSG_W2S_TEAM_KICK_PLAYER,
		MSG_W2S_TEAM_CHANGE_LEAER,
		MSG_W2S_TEAM_SYNCHRONIZE,
		MSG_W2S_TEAM_QUERY_PLAYER,									//查询玩家是否存在
		MSG_W2S_TEAM_CHANGE_ALLOCATION_SCHEME,						//改变分配方式
		MSG_W2S_TEAM_CHAT,
		MSG_W2S_TEAM_CHANGE_STATE,

		//MSG_W2S_TEAM_ANSWER_TEAMID,								//返回队伍ID
		//MSG_W2S_TEAM_TEAMMATE_JOIN,								//队员加入
		//MSG_W2S_TEAM_TEAMMATE_QUIT,								//队员离队
		//MSG_W2S_TEAM_OVER,										//散队	
		//MSG_W2S_TEAM_TEAMMATE_KICK,								//踢人
		//MSG_W2S_TEAM_DOWNLOAD,									//下传队伍信息
		//MSG_W2S_TEAM_ANSWER_SIZE,								//返回队伍大小
		//MSG_W2S_TEAM_MISS_CAPTAIN_INFO,							//丢失队长信息	
		//MSG_W2S_TEAM_CHANGE_CAPTAIN,							//任命队长	
		//MSG_W2S_TEAM_SPREAD_CAPTAIN_CHANGE,						//...
		//MSG_W2S_TEAM_CHAT,
		//MSG_W2S_TEAM_CHANGEREGION,								//队伍切换场景

		MSG_W2S_ORGSYS					= MSG_W2S + MAX_SUB * 6,
		// ----------------------------------------------------
		MSG_W2S_ORGSYS_AnswerCREATEFACTION,
		MSG_W2S_ORGSYS_ADDFACTION,								//添加帮会
		MSG_W2S_ORGSYS_DELFACTION,								//删除帮会
		MSG_W2S_ORGSYS_ADDUNION,								//添加同盟
		MSG_W2S_ORGSYS_DELUNION,								//删除同盟
		MSG_W2S_ORGSYS_UPDATEPLAYERFACTIONINFO,					//更新帮会信息
		MSG_W2S_ORGSYS_ANSWERFACTIONLIST,						//返回帮会列表
		MSG_W2S_ORGSYS_AddAllFactions,							//返回所有的帮会给客户端
		MSG_W2S_ORGSYS_ANSWERAPPLYJOINFACTION,
		MSG_W2S_ORGSYS_UpdateApplyMember,						//更新申请人员
		MSG_W2S_ORGSYS_UpdateFactionPropert,					//更新帮会属性
		MSG_W2S_ORGSYS_UpdateFactionBasePropert,				//更新基本帮会属性
		MSG_W2S_ORGSYS_UpdateMemberInfo,						//更新成员信息
		MSG_W2S_ORGSYS_UpdateUnionMemberInfo,					//更新同盟成员信息
		MSG_W2S_ORGSYS_UpdateLeaveWord,							//更新留言信息
		MSG_W2S_ORGSYS_UpdatePronounce,							//更新公告信息
		MSG_W2S_ORGSYS_UpdateEnemyFaction,						//更新敌对帮会信息
		MSG_W2S_ORGSYS_UpdateCityWarEnemyFaction,				//更新国王争霸战敌对帮会信息
		MSG_W2S_ORGSYS_UpdateOwnedCity,							//更新拥有的主城信息
		MSG_W2S_ORGSYS_UpdateFactionExp,						//更新帮会的经验显示到客户端
		MSG_W2S_ORGSYS_UpdateFactionInfo,						//更新帮会信息到客户端
		MSG_W2S_ORGSYS_RequestAffirmEstaUnion,					//请求确认建立同盟
		MSG_W2S_ORGSYS_RequestAffirmJoinUnion,					//请求确认邀请同盟
		MSG_W2S_FACTIONWAR_ANSWERFACTIONLIST,					////返回帮战帮会列表
		MSG_W2S_ORGSYS_ANSWERDigUpTheHatchet,					//恢复宣战结果
		MSG_W2S_ORGSYS_RequestAffirmDigUpTheHatchet,			//请求确认宣战
		MSG_W2S_ORGSYS_RequestAffirmMakePeace,					//请求确认议和
		MSG_W2S_ORGSYS_RequestAffirmSurrender,					//请求确认投降
		MSG_W2S_ORGSYS_AnswerGetFactionBillboard,				//得到帮会排行榜
		MSG_W2S_ORGSYS_AnswerUpgradeFaction,					//恢复申请升级帮会
		//MSG_S2W_ORGSYS_AnswerUpgradeFaction,					//恢复申请升级帮会
		MSG_W2S_ATTACKCITY_Declar,								//攻国王争霸战宣战开始
		MSG_W2S_ATTACKCITY_Start,								//攻国王争霸战开始
		MSG_W2S_ATTACKCITY_TimeOut,								//攻城时间到
		MSG_W2S_ATTACKCITY_End,									//攻国王争霸战结束
		MSG_W2S_ATTACKCITY_Mass,								//攻国王争霸战集结开始
		MSG_W2S_ATTACKCITY_ClearOtherPlayer,					//攻城清除闲杂人
		MSG_W2S_ATTACKCITY_RefreshRegion,						//攻国王争霸战集结开始
		MSG_W2S_ATTACKCITY_GetTodayTax,							//收集当天税收
		MSG_W2S_ATTACKCITY_RefreshOwnedCityOrg,					//刷新拥有主城的组织
		MSG_W2S_CITY_AnswerObtainTaxPayment,					//获取税金
		MSG_W2S_CITY_AnswerOAdjustTaxRate,						//调整税率
		MSG_W2S_CITY_OperatorCityGate,							//操作城门
		MSG_W2S_CITY_RequestAffirmTransfere,					//请求确认转让主城
		MSG_W2S_CITY_RefreshState,								//刷新主城状态
		MSG_W2S_CITY_AddTaxMoney,								//给某个场景添加税收
		MSG_W2S_CITY_RefreshProxyRegionTax,						//给某个场景添加税收

		MSG_W2S_VILLAGE_DeclareWar,								//宣战开始
		MSG_W2S_VILLAGE_WarStart,								//村战开始
		MSG_W2S_VILLAGE_TimeOut,								//据点时间到
		MSG_W2S_VILLAGE_WarEnd,									//存在结束
		MSG_W2S_AVILLAGE_ClearPlayer,							//据点战清除据点玩家
		MSG_W2S_ANSWERApplyForVillageWar,						//恢复报名参加据点争霸战
		MSG_W2S_CITYWAR_UpdateDecWarFacs,						//更新申请战斗的帮会列表
		MSG_W2S_VIILWAR_UpdateDecWarFacs,						//更新申请战斗的帮会列表
		MSG_W2S_CITYWAR_DeclareWar,								//宣战

		MSG_W2S_QUEST_PlayerAddQuest,							//给某个玩家添加一个任务
		MSG_W2S_QUEST_PlayerRemoveQuest,						//给某个玩家移出一个任务
		MSG_W2S_QUEST_PlayerRunScript,							//给某个玩家运新一个脚本

		// 国家
		MSG_W2S_COUNTRY					= MSG_W2S + MAX_SUB * 7,
		//---------------------------------------------------------------------------
		MSG_W2S_COUNTRY_CHANGE,									// 更改国籍
		MSG_W2S_COUNTRY_POWER_CHANGE,
		MSG_W2S_COUNTRY_TECHLEVEL_CHANGE,
		MSG_W2S_COUNTRY_CHANGE_CI,
		//////////////////////////////////////////////////////////////////
		// 国王权限 W2S yulefox
		MSG_W2S_COUNTRY_PreSetKingID,							// 预设国王
		MSG_W2S_COUNTRY_KING_GETSTATE,							// 国王操作 : 获取当前状态（是否是国王争夺战状态）
		MSG_W2S_COUNTRY_KING_GetInfo,							// 国王操作 : 获取所有国家信息
		MSG_W2S_COUNTRY_KING_GetPlayerList,						// 国王操作 : 获取在线玩家列表
		MSG_W2S_COUNTRY_KING_Demise,							// 国王操作 : 禅让
		MSG_W2S_COUNTRY_KING_Appoint,							// 国王操作 : 任命
		MSG_W2S_COUNTRY_KING_Depose,							// 国王操作 : 撤职
		MSG_W2S_COUNTRY_KING_Absolve,							// 国王操作 : 赦免
		MSG_W2S_COUNTRY_KING_Silence,							// 国王操作 : 禁言
		MSG_W2S_COUNTRY_KING_Exile,								// 国王操作 : 流放
		MSG_W2S_COUNTRY_KING_Convone,							// 国王操作 : 召集
		MSG_W2S_COUNTRY_KING_ChangeControl,						// 國王統御力變更
		MSG_W2S_COUNTRY_COUNTRY_Message,						// 国家信息公告
		MSG_W2S_COUNTRY_WORLD_Message,							// 世界信息公告
		MSG_W2S_COUNTRY_PRIVATE_Message,						// 个人信息公告
		MSG_W2S_COUNTRY_ReleaseQuest,							// 发布国家任务

		// 国战部分
		MSG_W2S_COUNTRY_WAR_Declare_Response,					// 回答宣战
		MSG_W2S_COUNTRY_WAR_Declare_Begin,						//战场宣战开始
		MSG_W2S_COUNTRY_WAR_Declare_End,						//战场宣战结束
		
		MSG_W2S_COUNTRY_WAR_Prepare_Begin,						//战场备战开始
		MSG_W2S_COUNTRY_WAR_Prepare_End,						//战场备战结束

		MSG_W2S_COUNTRY_WAR_Start,								//战场开始
		MSG_W2S_COUNTRY_WAR_TimeOut,							//战场时间到
		MSG_W2S_COUNTRY_WAR_End,								//战场结束
		MSG_W2S_COUNTRY_WAR_Clear,								//清场
		MSG_W2S_COUNTRY_WAR_UpdateCountrys,

		MSG_W2S_COUNTRY_WAR_FlagDestory,						//旗帜被毁

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool BEGIN
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MSG_W2S_GMA = MSG_W2S + MAX_SUB * 8,
		MSG_W2S_GMA_KICKPLAYER,
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GMA tool END
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//========================================================================================================
		MSG_C2S = MAX_TYPE * 9,			// Client --> GameServer
		//========================================================================================================

		MSG_C2S_LOG						= MSG_C2S + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_C2S_LOG_PLAYEREXITGAME,								// 发送向GameServer
		MSG_C2S_LOG_PLAYERENTER,								// 请求进入游戏服务器

		MSG_C2S_RGN						= MSG_C2S + MAX_SUB * 1,
		// ----------------------------------------------------
		MSG_C2S_RGN_ENTER,										// 玩家向服务器请求进入场景
		MSG_C2S_RGN_EXIT,										// 玩家请求离开场景

		MSG_C2S_SHAPE					= MSG_C2S + MAX_SUB * 2,
		// ----------------------------------------------------
		MSG_C2S_SHAPE_CHANGEDIR,								// 改变方向
		MSG_C2S_SHAPE_SETPOS,									// 瞬移
		MSG_C2S_SHAPE_MOVESTEP,									// 移动一步
		MSG_C2S_SHAPE_QUESTINFO,								// 主动向服务器请求shape的信息
		MSG_C2S_SHAPE_EMOTION,									// 表情动作

		MSG_C2S_PLAYER					= MSG_C2S + MAX_SUB * 3,
		// ----------------------------------------------------
		MSG_C2S_PLAYER_ADDPOINT,								// 加点
		MSG_C2S_PLAYER_QUEST_RELIVE,							// 请求复活
		MSG_C2S_PLAYER_TOUCHNPC,								// 触发NPC
		MSG_C2S_PLAYER_USEITEM,									// client到gameserver验证一个消耗道具的使用
		MSG_C2S_PLAYER_PKSWITCH,								// 切换PK开关
		MSG_C2S_PLAYER_TRADEREQUEST,							//交易请求
		MSG_C2S_PLAYER_TRADERESPONSE,							//客户端同意或拒绝交易请求
		MSG_C2S_PLAYER_PUTGOODSTOTRADEPAGE,						//客户端放一个物品到交易展示区
		MSG_C2S_PLAYER_FETCHGOODSFROMTRADEPAGE,					//玩家从展示区拿出一个物品
		MSG_C2S_PLAYER_TRADECHANGEMONEY,						//客户端请求调整金钱
		MSG_C2S_PLAYER_WAITFORTRADE,							//客户端通知进入交易等待状态
		MSG_C2S_PLAYER_TRADECANCEL,								//客户端请求取消交易
		MSG_C2S_PLAYER_ADDFRIENDREQUEST,						//客户端发送添加好友的请求到GameServer
		MSG_C2S_PLAYER_ADDFRIENDRESPONSE,						//被请求方玩家给GameServer做同意或拒绝的消息响应
		MSG_C2S_PLAYER_DELFRIEND,								//client向gameserver发送删除一个好友的请求
		MSG_C2S_PLAYER_HELPREQUEST,								// 玩家请求帮助
		MSG_C2S_PLAYER_UpdateDisplayHeadPiece,					//更新是否显示头盔信息
		MSG_C2S_PLAYER_CHANGE_QUEST_TIMEREQUEST,


		MSG_C2S_OTHER					= MSG_C2S + MAX_SUB * 4,
		// ----------------------------------------------------
		MSG_C2S_OTHER_TALK,										// 对话消息
		MSG_C2S_OTHER_SENDTALKBOXSELECT,						// 客户端返回TalkBox的选择
		MSG_C2S_OTHER_VIEWGOODSLINK,							// 客户端请求查看一个物品超链接
		MSG_C2S_OTHER_CONNECTGAMESERVER,						// 连接GameServer的结果
		MSG_C2S_OTHER_CHANGENAME,								// 玩家请求更名操作
		MSG_C2S_OTHER_QUESTNPCLIST,								// 请求NPC列表

		MSG_C2S_OTHER_TALK_WORLD,								// 世界喊话
		MSG_C2S_OTHER_TALK_COUNTRY,								// 世界喊话

		//MSG_C2S_OTHER_TALK_NOTIFY,							// 国家通知，死亡后通知。

		MSG_C2S_OTHER_REQUEST_LOANTIMELIMIT,					// 请求还款期限

		MSG_C2S_OTHER_QUESTGOD = MSG_C2S_OTHER + MAX_SUB - 7,	// 请求天神降临

		MSG_C2S_GOODS					= MSG_C2S + MAX_SUB * 5,
		// ----------------------------------------------------
		MSG_C2S_FETCHGOOD,										//player取一个物品
		MSG_C2S_PUTDOWNGOOD,									//player放下一个物品
		MSG_C2S_CHANGEGOOD,										//player调换物品位置
		MSG_C2S_DROPGOOD,										//player请求手上物品
		MSG_C2S_PICKGOOD,										//player请求拣物品
		MSG_C2S_DROPMONEY,							
		MSG_C2S_PICKMONEY,							
		MSG_C2S_SETHOTKEY,										//player请求设置HOTKEY
		MSG_C2S_CLEARHOTKEY,									//取消热键设置
		MSG_C2S_EXCHANGEHOTKEY,									//替换热键设置
		MSG_C2S_VIEWEQUIP,										//查看玩家装备

		MSG_C2S_GOODSUPGRADE_PUT,								//放置一个物品到升级界面
		MSG_C2S_GOODSUPGRADE_FETCH,								//从升级界面中取出一个物品
		MSG_C2S_GOODSUPGRADE_CHANGE,							//在升级界面中交换一个物品
		MSG_C2S_GOODSUPGRADE_UPGRADE,                           //client请求升级一个物品
		MSG_C2S_GOODSUPGRADE_CLOSE,								// 关闭升级界面(结束session)
		MSG_C2S_GOODS_CONTAINER,								// 物品容器返回
		MSG_C2S_GOODS_PRECIOUBOX_ITEM,							// 获取百宝箱道具

		// gongran fruitmachine 2008-3-3
		//MSG_C2S_FRUIT_MACHINE_START,							// 八面玲珑-开始
		//MSG_C2S_FRUIT_MACHINE_RECEIVE,							// 八面玲珑-领奖
		//MSG_C2S_FRUIT_MACHINE_CLOSE,							// 八面玲珑-关闭

		MSG_C2S_SHOP					= MSG_C2S + MAX_SUB * 6,
		// ----------------------------------------------------
		MSG_C2S_SHOP_BUY,										//player请求买东西
		MSG_C2S_SHOP_SELL,										//player卖一个物品回商店
		MSG_C2S_SHOP_REPAIR,									//在商店中修理一个物品
		MSG_C2S_SHOP_REPAIRALL, 								//商店中修理全部物品
		MSG_C2S_SHOP_DURUPDATE,									//耐久更新
		MSG_C2S_SHOP_CLOSE,										//关闭个人商店

		MSG_C2S_DEPOT					= MSG_C2S + MAX_SUB * 7,
		// ----------------------------------------------------
		MSG_C2S_DEPOT_PUTDOWN,
		MSG_C2S_DEPOT_FETCH,
		MSG_C2S_DEPOT_CHANGE,
		MSG_C2S_DEPOT_SAVEMONEY,
		MSG_C2S_DEPOT_FETCHMONEY,
		MSG_C2S_DEPOT_CHECKPASSWORD,
		MSG_C2S_DEPOT_CHANGEPASSWORD,
		MSG_C2S_DEPOT_CLOSE,

		MSG_C2S_TEAM					= MSG_C2S + MAX_SUB * 8,
		// ----------------------------------------------------
		MSG_C2S_TEAM_QUERY_JOIN,									//邀请加入
		MSG_C2S_TEAM_ANSWER_JOIN,									//回复邀请加入
		MSG_C2S_TEAM_TEAMATE_JOIN,
		MSG_C2S_TEAM_TEAMATE_LEAVE,
		MSG_C2S_TEAM_CHANGE_LEAER,									//改变队长
		MSG_C2S_TEAM_KICK_PLAYER,									//踢人
		MSG_C2S_TEAM_DESTORY,
		MSG_C2S_TEAM_RECRUIT,										//招募控制
		MSG_C2S_TEAM_CHANGE_ALLOCATION_SCHEME,
		MSG_C2S_TEAM_CHAT,
		MSG_C2S_TEAM_QUERY_JOIN_BYNAME,									//通过名字邀请加入队伍
		//MSG_C2S_TEAM_TEAMMATE_QUIT,						        //队员离队
		//MSG_C2S_TEAM_TEAMMATE_KICK,								//踢人
		//MSG_C2S_TEAM_UPLOAD,									//上传队伍信息	
		//MSG_C2S_TEAM_ANSWER_CAPTAIN_INFO,						//返回队长信息
		//MSG_C2S_TEAM_CHAT,										//队聊

		MSG_C2S_SKILL					= MSG_C2S + MAX_SUB * 9,
		// ----------------------------------------------------
		MSG_C2S_SKILL_USE,										// 使用技能
		MSG_C2S_SKILL_USE_END,									// 使用技能结束，有些技能松开快捷键的时候发送
		MSG_C2S_SKILL_STUDY,									// 学习技能

		MSG_C2S_ORGSYS					= MSG_C2S + MAX_SUB * 10,
		// ----------------------------------------------------
		MSG_C2S_ORGSYS_CREATEFACTION_AnswerName,				//返回输入的帮会名字
		MSG_C2S_ORGSYS_GetOrganizing,							//得到帮会、同盟
		MSG_C2S_ORGSYS_GetFaction,								//得到帮会
		MSG_C2S_ORGSYS_GetUnion,								//得到帮会
		MSG_C2S_ORGSYS_GETFACTIONLIST,
		MSG_C2S_ORGSYS_APPLYJOINFACTION,						//申请加入帮会
		MSG_C2S_ORGSYS_DOJOINFACTION,							//加入帮会处理
		MSG_C2S_ORGSYS_FireOutFactionMember,					//开除帮员
		MSG_C2S_ORGSYS_FireOutUnionMember,						//开除盟员
		MSG_C2S_ORGSYS_ExitFaction,								//退出帮会
		MSG_C2S_ORGSYS_ExitUnion,								//退出同盟
		MSG_C2S_ORGSYS_Demise,									//帮主传位 
		MSG_C2S_ORGSYS_UnionDemise,								//盟主传位 
		MSG_C2S_ORGSYS_Disband,									//解散帮会 
		MSG_C2S_ORGSYS_DisbandUnion,							//解散同盟 
		MSG_C2S_ORGSYS_DubJobLvl,								//授予称号和帮会职级
		MSG_C2S_ORGSYS_EndueRight,								//授予权限
		MSG_C2S_ORGSYS_AbolishRight,							//取消权限
		MSG_C2S_ORGSYS_RequestEstaUnion,						//请求建立同盟
		MSG_C2S_ORGSYS_AnswerAffirmEstaUnion,					//回复确认建立同盟
		MSG_C2S_ORGSYS_InviteJoinUnion,							//邀请加入同盟
		MSG_C2S_ORGSYS_AnswerAffirmJoinUnion,					//回复确认邀请同盟
		MSG_C2S_ORGSYS_LeaveWord,								//留言
		MSG_C2S_ORGSYS_DelLeaveWord,							//删除留言
		MSG_C2S_ORGSYS_EditPronounce,							//编辑公告
		MSG_C2S_FACTIONWAR_GETFACTIONLIST,
		MSG_C2S_ORGSYS_DigUpTheHatchet,							//宣战
		MSG_C2S_ORGSYS_AnswerAffirmDigUpTheHatchet,				//回复确认宣战
		MSG_C2S_ORGSYS_MakePeace,								//议和
		MSG_C2S_ORGSYS_AnswerAffirmMakePeace,					//回复确认议和
		MSG_C2S_ORGSYS_Surrender,								//投降
		MSG_C2S_ORGSYS_AnswerAffirmSurrender,					//回复确认投降
		MSG_C2S_ORGSYS_SetFactionControbute,					//设置帮会的贡献者
		MSG_C2S_CITY_AnswerInputTaxPayment,						//获取税金
		MSG_C2S_CITY_AnswerInputTaxRate,						//调整税率
		MSG_C2S_CITY_Transfer,									//转让主城
		MSG_C2S_CITY_AnswerAffirmTransfere,						//回复确认转让主城
		MSG_C2S_CITY_SetPermit,									//设置是否允许同盟、宣战

		MSG_C2S_QUEST_Complete,									//提交完成任务
		MSG_C2S_QUEST_Disband,									//提交放弃任务

		MSG_C2S_QUEST_PlayerRanks,								//请求玩家的排行榜

		MSG_C2S_PLAYERSHOP				= MSG_C2S + MAX_SUB * 11,
		// ----------------------------------------------------
		MSG_C2S_PLAYERSHOP_CREATE,								//请求创建个人商店
		MSG_C2S_PLAYERSHOP_GOODSPRICE,
		MSG_C2S_PLAYERSHOP_BUYITEM,								//购买商品
		MSG_C2S_PLAYERSHOP_OPENFORBUSINESS,						//开张
		MSG_C2S_PLAYERSHOP_CLOSEDOWN,							//关张
		MSG_C2S_PLAYERSHOP_LOOKGOODS,							//察看商品
		MSG_C2S_PLAYERSHOP_CLOSE,								//关闭个人商店
		MSG_C2S_PLAYERSHOP_QUIT,								//购买者离开个人商店
		//MSG_C2S_PLAYERSHOP_FETCHGOODS,
		//MSG_C2S_PLAYERSHOP_PUTGOODS,
		//MSG_C2S_PLAYERSHOP_EXCHANGEITEM,

		// 容器
		MSG_C2S_CONTAINER				= MSG_C2S + MAX_SUB * 12,
		// ----------------------------------------------------
		MSG_C2S_CONTAINER_OBJECT_MOVE,

		// 宠物
		MSG_C2S_PET						= MSG_C2S + MAX_SUB * 13,
		// ----------------------------------------------------
		MSG_C2S_PET_CHANGE_MODE,								//改变宠物当前模式
		MSG_C2S_PET_CHANGE_ACTION,								//改变宠物当前动作

		// 国家
		MSG_C2S_COUNTRY					= MSG_C2S + MAX_SUB * 14,
		// -----------------------------------------------------
		//////////////////////////////////////////////////////////////////
		// 国王权限 C2S yulefox
		MSG_C2S_COUNTRY_KING_GETSTATE,							// 国王操作 : 获取当前状态（是否是国王争夺战状态）
		MSG_C2S_COUNTRY_KING_GetInfo,							// 国王操作 : 获取所有国家信息
		MSG_C2S_COUNTRY_KING_GetPlayerList,						// 国王操作 : 获取在线玩家列表
		MSG_C2S_COUNTRY_KING_Demise,							// 国王操作 : 禅让
		MSG_C2S_COUNTRY_KING_Appoint,							// 国王操作 : 任命
		MSG_C2S_COUNTRY_KING_Depose,							// 国王操作 : 撤职
		MSG_C2S_COUNTRY_KING_Absolve,							// 国王操作 : 赦免
		MSG_C2S_COUNTRY_KING_Silence,							// 国王操作 : 禁言
		MSG_C2S_COUNTRY_KING_Exile,								// 国王操作 : 流放
		MSG_C2S_COUNTRY_KING_Convone,							// 国王操作 : 召集

		MSG_C2S_COUNTRY_WAR_EntryRegionBySummon,				// 被召唤进入国战地图
		// 增值商店
		MSG_C2S_INCREMENTSHOP					= MSG_C2S + MAX_SUB * 15,
		// ----------------------------------------------------
		MSG_C2S_INCREMENTSHOP_OPEN,
		MSG_C2S_INCREMENTSHOP_LIST,										//player请求商城列表
		MSG_C2S_INCREMENTSHOP_BUY,										//player请求买东西
		MSG_C2S_INCREMENTSHOP_CLOSE,									//关闭个人商店

		MSG_C2S_INCREMENTSHOP_GETLOG_REQUEST,							// 请求增值日志

		//MSG_C2S_GETLOG_RESPONSE,

		//////////////////
		//				//
		//	GameClient	//
		//				//
		//////////////////

		//========================================================================================================
		MSG_C2C = MAX_TYPE * 10,			// Client --> Client
		//========================================================================================================

		MSG_C2C_LOG						= MSG_C2C + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_C2C_LOG_SERVERLOST,									// CLIENT在接受到Server断线消息时给自己发一条消息

		//========================================================================================================
		MSG_L2C = MAX_TYPE * 11,		// LoginServer --> Client
		//========================================================================================================

		MSG_L2C_LOG						= MSG_L2C + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_L2C_LOG_ANSWERCDKEY,								// 服务器返回CDKEY
		MSG_L2C_LOG_ANSWER_PLAYERBASELIST,						// 服务器返回玩家基本数据列表
		MSG_L2C_LOG_ANSWER_PLAYERDATA,							// 返回玩家详细数据
		MSG_L2C_LOG_ANSWERCREATE,								// 服务器返回建立角色的信息
		MSG_L2C_LOG_ANSWERDELETE,								// 服务器返回是否正功删除消息
		MSG_L2C_LOG_ANSWERRESTORE,								// 是否成功回复角色
		MSG_L2C_LOG_UPDATEQUEUENUM,								// 更新登陆缓冲队列的剩余数量

		MSG_L2C_LOG_SENDACCOUNTINFO,							// 发送帐号的信息给客户端
		MSG_L2C_LOG_UPDATEWORLDSERVERINFO,						// 更新WorldServer的状态信息

		//========================================================================================================
		MSG_S2C = MAX_TYPE * 12,		// GameServer --> Client
		//========================================================================================================

		MSG_S2C_LOG						= MSG_S2C + MAX_SUB * 0,
		// ----------------------------------------------------
		MSG_S2C_LOG_ANSWER_PLAYERDATA,							// GameServer向GameClient返回player详细数据
		MSG_S2C_LOG_GAMESERVERTIME,								// 发送Server的时间
		MSG_S2C_LOG_SEQUENCESTRING,								// 验证消息合法性的字符串序列

		MSG_S2C_LOG_PLAYER_ONLINE,								// 玩家好友上线
		MSG_S2C_LOG_PLAYER_OFFLINE,								// 玩家好友下线

		MSG_S2C_RGN						= MSG_S2C + MAX_SUB * 1,
		// ----------------------------------------------------
		MSG_S2C_RGN_ENTER,										// 服务器返回请求进入场景
		MSG_S2C_RGN_ADDSHAPE,									// 有对象加入场景
		MSG_S2C_RGN_ADDSUMMONEDSHAPE,							// 召唤的Shape加入场景
		MSG_S2C_RGN_DELSHAPE,									// 有对象离开场景
		MSG_S2C_RGN_CHANGE,										// 服务器命令玩家切换场景
		MSG_S2C_RGN_CHANGESERVER,								// 服务器命令玩家切换场景服务器
		MSG_S2C_RGN_CHANGEWEATHER,								// 改变天气
		MSG_S2C_RGN_PLAYACTION,									// 播放动作
		MSG_S2C_RGN_PLAYSOUND,									// 播放声音
		MSG_S2C_RGN_PLAYEFFECT,									// 播放特效

		MSG_S2C_SHAPE					= MSG_S2C + MAX_SUB * 2,
		// ----------------------------------------------------
		MSG_S2C_SHAPE_CHANGEDIR,								// 改变方向
		MSG_S2C_SHAPE_CHECKPOS,									// 坐标效验
		MSG_S2C_SHAPE_SETPOS,									// 瞬移
		MSG_S2C_SHAPE_FORCEMOVE,								// 强制移动
		MSG_S2C_SHAPE_MOVESTEP,									// 回复移动一步
		MSG_S2C_SHAPE_MOVETILE,									// 服务器发来消息移动一格
		MSG_S2C_SHAPE_CHANGESTATE,								// 改变状态
		MSG_S2C_SHAPE_ATK_LOCK,									// 指示锁定目标
		MSG_S2C_SHAPE_ATK_BEGIN,								// 服务器通知客户端开始攻击
		MSG_S2C_SHAPE_ATK_BREAK,								// 被攻击停顿
		MSG_S2C_SHAPE_ATK_DIED,									// 死亡
		MSG_S2C_SHAPE_ATK_REJECT,								// 攻击指令被拒绝
		MSG_S2C_SHAPE_FIGHTSTATE,								// 通知client进入或退出战斗状态
		MSG_S2C_SHAPE_SINSTATE,									// 通知client进入或退出犯罪状态
		MSG_S2C_SHAPE_Refresh,									// 刷新建筑状态 
		MSG_S2C_SHAPE_STIFFEN,									// 僵直状态
		MSG_S2C_SHAPE_EMOTION,									// 僵直状态
		MSG_S2C_SHAPE_ATK_FULLMISS,								// 闪避
		MSG_S2C_SHAPE_RELIVE,									// 复活一个玩家

		MSG_S2C_PLAYER					= MSG_S2C + MAX_SUB * 3,
		// ----------------------------------------------------
		MSG_S2C_PLAYER_RESUME,									// 恢复HP、MP、YP。。。
		MSG_S2C_PLAYER_ADDPOINT,								// 加点
		MSG_S2C_PLAYER_ANSWER_RELIVE,							// 恢复复活
		MSG_S2C_PLAYER_ADDEXP,									// 增加经验值
		MSG_S2C_PLAYER_LEVELUP,									// 升级
		MSG_S2C_PLAYER_HITLEVELUPGRADE,							// 连击数升级
		MSG_S2C_PLAYER_SYNC_HITS,								// 同步连击数
		MSG_S2C_PLAYER_SETLEVEL,
		MSG_S2C_PLAYER_USEITEM,									// gameserver向客户端返回消耗道具的使用结果
		MSG_S2C_PLAYER_CHANGEMAXATK,							// 消耗性道具提升最大攻击力
		MSG_S2C_PLAYER_CHANGEATKSPEED,							// 消耗性道具提升攻击速度
		MSG_S2C_PLAYER_CHANGEDEF,								// 消耗性道具提升防守能力
		MSG_S2C_PLAYER_CHANGEELEMENTMODIFY,						// 消耗性道具改变技能攻击系数
		MSG_S2C_PLAYER_SYNC_PKCOUNT,							// 同步杀人数
		MSG_S2C_PLAYER_TRADEREQUEST,							//GS转发交易请求给客户
		MSG_S2C_PLAYER_TRADEBEGIN,								//交易开始
		MSG_S2C_PLAYER_PUTGOODSTOTRADEPAGE,						//服务器返回放一个物品到展示区的结果
		MSG_S2C_PLAYER_GETGOODSFROMTRADEPAGE,					//服务器推一个物品信息给对方玩家
		MSG_S2C_PLAYER_FETCHGOODSFORMTRADEPAGE,					//服务器返回从展示区拿一个物品的相关结果
		MSG_S2C_PLAYER_DELGOODSFROMTRADEPAGE,					//服务器通知对方一个物品从交易区中删除
		MSG_S2C_PLAYER_TRADECHANGEMONEY,						//服务器返回调整金钱的相关结果
		MSG_S2C_PLAYER_WAITFORTRADE,				
		MSG_S2C_PLAYER_TRADECANCEL,								//服务器通知玩家对方已经取消交易
		MSG_S2C_PLAYER_TRADERESULT,								//交易的最终结果
		MSG_S2C_PLAYER_ADDFRIENDREQUEST,						//GameServer转发好友添加消息给被请求玩家
		MSG_S2C_PLAYER_ADDFRIENDRESPONSE,						//GameServer转发被请求玩家的响应结果给发出请求的那个玩家
		MSG_S2C_PLAYER_DELFRIEND,								//gameserver向client返回删除一个好友的结果
		MSG_S2C_PLAYER_HELPRESPONSE,							// 相应玩家帮助
		MSG_S2C_PLAYER_ADDSKILL,								// 添加技能
		MSG_S2C_PLAYER_DELSKILL,								// 删除技能
		MSG_S2C_PLAYER_SETATTACKRANGE,				
		MSG_S2C_PLAYER_CHANGE_EQUIPMENT,
		MSG_S2C_PLAYER_CHANGE_PROPERTIES,						//玩家改变属性
		MSG_S2C_PLAYER_UpdateDisplayHeadPiece,					//更新玩家是否显示头盔信息
		MSG_S2C_PLAYER_AUTOMOVE,								//自动移动到指定的点
		MSG_S2C_PLAYER_CHANGE_CONTRIBUTE,						// 国家贡献改变
		MSG_S2C_PLAYER_CHANGE_LOAN,
		MSG_S2C_PLAYER_CHANGE_LOANMAX,
		MSG_S2C_PLAYER_CHANGE_LOANTIME,
		MSG_S2C_PLAYER_CHANGE_QUEST_ON,
		MSG_S2C_PLAYER_CHANGE_QUEST_TIMEBEGIN,
		MSG_S2C_PLAYER_CHANGE_QUEST_TIMECLEAR,
		MSG_S2C_PLAYER_CHANGE_QUEST_TIMERESPONSE,
		MSG_S2C_PLAYER_CHANGE_ENERGY,							// 精力值的改变
		MSG_S2C_PLAYER_CHANGE_MAXENERGY,						// 设置精力值上限
		MSG_S2C_PLAYER_CHANGE_Exploit,							// 改变 军功
		MSG_S2C_PLAYER_CHANGE_Kudos,							// 改变 荣誉
		MSG_S2C_PLAYER_UpdateOnlineTime,						// 更新在线时间

		MSG_S2C_OTHER					= MSG_S2C + MAX_SUB * 4,
		// ----------------------------------------------------
		MSG_S2C_OTHER_TALK,										// 对话消息
		MSG_S2C_OTHER_FACTIONTALK,								// 帮会聊天
		MSG_S2C_OTHER_BROADCAST,								// 广播
		MSG_S2C_OTHER_AddTopInfo,								// 添加顶端提示信息
		MSG_S2C_OTHER_OPENTALKBOX,								// 让客户端弹出TalkBox
		MSG_S2C_OTHER_ADDINFO,									// 在玩家对话框加入一条信息
		MSG_S2C_OTHER_ADDSYSTEMINFO,							// 在屏幕上方加入一条信息
		MSG_S2C_OTHER_GAMEMESSAGE,								// 弹出消息对话框
		MSG_S2C_OTHER_PING,										// 服务器定时检测Client是否存在线，client返回同样消息
		MSG_S2C_OTHER_SENDOBJECTPROPERTIY,						// 接受Shape的所有属性
		MSG_S2C_OTHER_CHANGESHAPEPROPERTIY,						// 改变Shape的一个属性
		MSG_S2C_OTHER_SETSHAPEPROPERTIY,						// 设置Shape的一个属性
		MSG_S2C_OTHER_VIEWGOODSLINK,			
		MSG_S2C_OTHER_SCRIPTTIMERRUN,							// 脚本计时器运行
		MSG_S2C_OTHER_CHANGENAME,								// 接受Gs转发Ws更名结果
		MSG_S2C_OTHER_CHANGENAME_OPENUI,						// 启动更名窗口
		MSG_S2C_OTHER_ADDPLAYERINFO,
		MSG_S2C_OTHER_SHOWREADMEWINDOW,
		MSG_S2C_OTHER_ANSWERNPCLIST,							// 返回NPC列表给客户端

		MSG_S2C_OTHER_TALK_WORLD,								// 世界喊话
		MSG_S2C_OTHER_TALK_COUNTRY,								// 国家喊话

		MSG_S2C_OTHER_TALK_NOTIFY,								// 国家通知

		MSG_S2C_OTHER_RESPONSE_LOANTIMELIMIT,					// 返回还款期限
		
		

		MSG_S2C_GOODS					= MSG_S2C + MAX_SUB * 5,
		// ----------------------------------------------------
		MSG_S2C_FETCHGOOD,										//GAMESERVER返回取一个物品的结果
		MSG_S2C_PUTDOWNGOOD,									//gameserver返回放下一个物品的结果
		MSG_S2C_CHANGEGOOD,										//gameserver返回物品调换结果
		MSG_S2C_DROPGOOD,										//gameserver返回扔物品结果
		MSG_S2C_PICKGOOD,										//gameserver返回拣物品结果
		MSG_S2C_DROPMONEY,							
		MSG_S2C_PICKMONEY,							
		MSG_S2C_SETHOTKEY,										//gameserver返回设置HOTKEY结果
		MSG_S2C_CLEARHOTKEY,						
		MSG_S2C_EXCHANGEHOTKEY,						
		MSG_S2C_EQUIPDAMAGE,                                    //装备坏掉
		MSG_S2C_EQUIPDURUPDATE,									//更新装备的耐久
		MSG_S2C_DELETEEQUIP,									//删除装备
		MSG_S2C_DELETEBAGGOODS,									//删除背包物品
		MSG_S2C_DELETEGOODS,									
		MSG_S2C_DELETEALLGOODS,						
		MSG_S2C_VIEWEQUIP,                                      
		MSG_S2C_OPENGOODSUPGRADE,								//gameserver通知玩家打开物品升级界面
		MSG_S2C_CLOSEGOODSUPGRADE,
		MSG_S2C_GOODSUPGRADE_PUT,					
		MSG_S2C_GOODSUPGRADE_FETCH,					
		MSG_S2C_GOODSUPGRADE_CHANGE,				
		MSG_S2C_GOODSUPGRADE_UPGRADE,							//gameserver返回升级一个物品的结果
		MSG_S2C_GOODS_UPDATEPROPERTY,							//更新物品属性
		MSG_S2C_GOODS_OPEN_CONTAINER,							// 打开物品容器

		MSG_S2C_GOODS_PRECIOUSBOX_OPEN,							// 打开百宝箱
		MSG_S2C_GOODS_PRECIOUSBOX_ITEM,							// 百宝箱获取道具信息
		MSG_S2C_GOODS_PRECIOUSBOX_CLOSE,

		// gongran fruitmachine 2008-3-3
		//MSG_S2C_FRUIT_MACHINE_OPEN,								// 八面玲珑-打开界面
		//MSG_S2C_FRUIT_MACHINE_RESULT,							// 八面玲珑-通知客户端游戏结果


		MSG_S2C_SHOP					= MSG_S2C + MAX_SUB * 6,
		// ----------------------------------------------------
		MSG_S2C_SHOP_OPEN,										//打开商店			
		MSG_S2C_SHOP_BUY,									   //gameserver返回从商店中购买一个物品的结果
		MSG_S2C_SHOP_SELL,							
		MSG_S2C_SHOP_REPAIR,						
		MSG_S2C_SHOP_REPAIRALL,						
		MSG_S2C_SHOP_DURUPDATE,						
		MSG_S2C_SHOP_CLOSE,		//##关闭个人商店	

		MSG_S2C_DEPOT					= MSG_S2C + MAX_SUB * 7,
		// ----------------------------------------------------
		MSG_S2C_DEPOT_OPEN,							
		MSG_S2C_DEPOT_PUTDOWN,						
		MSG_S2C_DEPOT_FETCH,						
		MSG_S2C_DEPOT_CHANGE,						
		MSG_S2C_DEPOT_SAVEMONEY,					
		MSG_S2C_DEPOT_FETCHMONEY,					
		MSG_S2C_DEPOT_CHECKPASSWORD,				
		MSG_S2C_DEPOT_CHANGEPASSWORD,				

		MSG_S2C_GM						= MSG_S2C + MAX_SUB * 8,
		// ----------------------------------------------------
		MSG_S2C_GM_ANSWER_GMMODE,								// 响应客户端请求GM模式的消息

		MSG_S2C_TEAM					= MSG_S2C + MAX_SUB * 9,
		// ----------------------------------------------------
		MSG_S2C_TEAM_QUERY_JOIN,								//邀请加入
		MSG_S2C_TEAM_ANSWER_JOIN,								//回复邀请加入
		MSG_S2C_TEAM_SYNCHRONIZE,								//同步队伍的信息
		MSG_S2C_TEAM_TEAMATE_JOIN,
		MSG_S2C_TEAM_TEAMATE_LEAVE,
		MSG_S2C_TEAM_TEAMATE_CHANGE_REGION,
		MSG_S2C_TEAM_CHANGE_LEAER,									//改变队长
		MSG_S2C_TEAM_CHANGE_ALLOCATION_SCHEME,
		MSG_S2C_TEAM_CHAT,
		MSG_S2C_TEAM_CHANGE_STATE,
		//MSG_S2C_TEAM_TEAMMATE_JOIN,								//队员加入
		//MSG_S2C_TEAM_TEAMMATE_QUIT,								//队员离队
		//MSG_S2C_TEAM_OVER,										//散队	
		//MSG_S2C_TEAM_TEAMMATE_KICK,								//踢人
		//MSG_S2C_TEAM_DOWNLOAD,									//下传队伍信息	
		//MSG_S2C_TEAM_QUERY_CAPTAIN_INFO,						//询问队长信息
		//MSG_S2C_TEAM_CHANGE_CAPTAIN,							//任命队长	
		//MSG_S2C_TEAM_SPREAD_CAPTAIN_CHANGE,						//...
		//MSG_S2C_TEAM_CHAT,
		//MSG_S2C_TEAM_BID,										//无队伍人员不能队聊

		MSG_S2C_SKILL					= MSG_S2C + MAX_SUB * 10,
		// ----------------------------------------------------
		MSG_S2C_SKILL_USE,										// 使用技能
		MSG_S2C_SKILL_STATE_CHANGE,								// 使用技能后导致HP/MP/RP/YP状态改变
		MSG_S2C_SKILL_ADD_EX_STATE,								// 使用技能后添加异常状态
		MSG_S2C_SKILL_END_EX_STATE,								// 异常状态的消失,服务器通知客户端
		MSG_S2C_SKILL_EX_STATE_ADDITIONAL_DATA,					// 异常状态的附加数据
		MSG_S2C_SKILL_AFFECT_FIELD,								// 发送受影响的区域给客户端

		MSG_S2C_ORGSYS					= MSG_S2C + MAX_SUB * 11,
		// ----------------------------------------------------
		MSG_S2C_ORGSYS_CREATEFACTION_RequiestName,				//创建帮会消息,要求客户端输入帮会名字
		MSG_S2C_ORGSYS_ADDFACTION,								//添加帮会
		MSG_S2C_ORGSYS_DELFACTION,								//删除帮会
		MSG_S2C_ORGSYS_ADDUNION,								//添加同盟
		MSG_S2C_ORGSYS_DELUNION,								//删除同盟
		MSG_S2C_ORGSYS_UPDATEPLAYERFACTIONINFO,					//更新帮会信息
		MSG_S2C_ORGSYS_ANSWERFACTIONLIST,						//返回帮会列表
		MSG_S2C_ORGSYS_AddAllFactions,							//返回所有的帮会给客户端
		MSG_S2C_ORGSYS_ANSWERAPPLYJOINFACTION,					//允许加入帮会
		MSG_S2C_ORGSYS_UpdateApplyMember,						//更新申请人员				
		MSG_S2C_ORGSYS_UpdateFactionProperty,					//更新帮会属性
		MSG_S2C_ORGSYS_UpdateFactionBaseProperty,				//更新帮户基本属性
		MSG_S2C_ORGSYS_UpdateMemberInfo,						//更新成员信息
		MSG_S2C_ORGSYS_UpdateUnionMemberInfo,					//更新同盟成员信息
		MSG_S2C_ORGSYS_UpdateLeaveWord,							//更新留言信息
		MSG_S2C_ORGSYS_UpdatePronounce,							//更新公告信息
		MSG_S2C_ORGSYS_UpdateEnemyFaction,						//更新敌对帮会信息
		MSG_S2C_ORGSYS_UpdateCityWarEnemyFaction,				//更新国王争霸战敌对帮会信息
		MSG_S2C_ORGSYS_UpdateOwnedCity,							//更新拥有的主城信息
		MSG_S2C_ORGSYS_UpdateFactionExp,						//更新帮会的经验显示到客户端
		MSG_S2C_ORGSYS_UpdateFactionInfo,						//更新帮会信息到客户端
		MSG_S2C_ORGSYS_PlayerAddFactionExp,						//玩家贡献了帮会经验
		MSG_S2C_ORGSYS_RequestAffirmEstaUnion,					//请求确认建立同盟
		MSG_S2C_ORGSYS_RequestAffirmJoinUnion,					//请求确认邀请同盟
		MSG_S2C_FACTIONWAR_ANSWERFACTIONLIST,					//返回宣战帮会列表
		MSG_S2C_ORGSYS_RequestAffirmDigUpTheHatchet,			//请求确认宣战
		MSG_S2C_ORGSYS_RequestAffirmMakePeace,					//请求确认议和
		MSG_S2C_ORGSYS_RequestAffirmSurrender,					//请求确认投降
		MSG_S2C_ORGSYS_AnswerGetFactionBillboard,				//得到帮会排行榜
		MSG_S2C_ATTACKCITY_Start,								//攻国王争霸战开始
		MSG_S2C_ATTACKCITY_End,									//攻国王争霸战结束
		MSG_S2C_ATTACKCITY_Mass,								//攻国王争霸战集结开始
		MSG_S2C_ATTACKCITY_ClearOtherPlayer,					//攻城清除闲杂人
		MSG_S2C_ATTACKCITY_RefreshRegion,						//攻国王争霸战集结开始
		MSG_S2C_ATTACKCITY_GetTodayTax,							//收集当天税收
		MSG_S2C_CITY_RequestInputTaxPayment,					//获取税金
		MSG_S2C_CITY_RequestInputTaxRate,						//调整税率
		MSG_S2C_CITY_UpdateTaxRate,								//税率更新税率
		MSG_S2C_CITY_RequestAffirmTransfer,						//请求确认转让主城

		MSG_S2C_WAR_UpdateContendState,							//更新争夺状态
		MSG_S2C_WAR_UpdateContendTime,							//更新争夺剩余时间
		MSG_S2C_WAR_UpdateDiedState,							//更新死亡状态
		MSG_S2C_WAR_UpdateDiedStateTime,						//更新死亡状态剩余时间

		MSG_S2C_Quest_Add,										//添加一个任务
		MSG_S2C_Quest_Complete,									//完成一个任务
		MSG_S2C_Quest_Remove,									//移出一个任务
		MSG_S2C_Quest_Update,									//更新一个任务

		MSG_S2C_ANSWER_PlayerRanks,								//向客户端发送玩家排行榜

		MSG_S2C_ORGSYS_RESPONSE_DigUpTheHatchet,				//宣战结果回应4.15.2008
		MSG_S2C_PLAYERSHOP				= MSG_S2C + MAX_SUB * 12,
		// ----------------------------------------------------
		MSG_S2C_PALYERSHOP_CREATE,
		MSG_S2C_PLAYERSHOP_BUYITEM,
		MSG_S2C_PLAYERSHOP_GOODSPRICE,
		MSG_S2C_PLAYERSHOP_OPENFORBUSINESS,
		MSG_S2C_PLAYERSHOP_CLOSEDOWN,
		MSG_S2C_PLAYERSHOP_LOOKGOODS,
		MSG_S2C_PLAYERSHOP_CLOSE,
		MSG_S2C_PLAYERSHOP_QUIT,
		//MSG_S2C_PLAYERSHOP_FETCHGOODS,
		//MSG_S2C_PLAYERSHOP_PUTGOODS,
		//MSG_S2C_PLAYERSHOP_EXCHANGEITEM,
		//MSG_S2C_PLAYERSHOP_HASCLOSED,
		//MSG_S2C_PLAYERSHOP_NOMOREMONEY,
		//MSG_S2C_PLAYERSHOP_NOSPACE,
		//MSG_S2C_PLAYERSHOP_NOTHISGOODS,
		//MSG_S2C_PLAYERSHOP_NOTSAFE,

		// 容器
		MSG_S2C_CONTAINER				= MSG_S2C + MAX_SUB * 13,
		// ----------------------------------------------------
		MSG_S2C_CONTAINER_OBJECT_MOVE,
		MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE,
		MSG_S2C_CONTAINER_CLEAR,

		// 宠物
		MSG_S2C_PET						= MSG_S2C + MAX_SUB * 14,
		// ----------------------------------------------------
		MSG_S2C_PET_CHANGE_MASTER,								//改变主人
		MSG_S2C_PET_CHANGE_MODE,								//改变攻击模式
		MSG_S2C_PET_CHANGE_EXPERIENCE,							//改变等级
		MSG_S2C_PET_CHANGE_ACTION,								//改变宠物当前动作
		MSG_S2C_PET_CARRIAGE,

		//国家
		MSG_S2C_COUNTRY					= MSG_S2C + MAX_SUB * 15,
		// -----------------------------------------------------
		MSG_S2C_COUNTRY_CHANGE,									// 更改国籍

		MSG_S2C_COUNTRY_CHANGE_CI,								// 设置国王
		//////////////////////////////////////////////////////////////////
		// 国王权限 S2C yulefox
		MSG_S2C_COUNTRY_KING_GETSTATE,							// 国王操作 : 获取当前状态（是否是国王争夺战状态）
		MSG_S2C_COUNTRY_KING_GetInfo,							// 国王操作 : 获取所有国家信息
		MSG_S2C_COUNTRY_KING_GetPlayerList,						// 国王操作 : 获取在线玩家列表
		MSG_S2C_COUNTRY_KING_Demise,							// 国王操作 : 禅让
		MSG_S2C_COUNTRY_KING_Appoint,							// 国王操作 : 任命
		MSG_S2C_COUNTRY_KING_Depose,							// 国王操作 : 撤职
		MSG_S2C_COUNTRY_KING_Absolve,							// 国王操作 : 赦免
		MSG_S2C_COUNTRY_KING_Silence,							// 国王操作 : 禁言
		MSG_S2C_COUNTRY_KING_Exile,								// 国王操作 : 流放
		MSG_S2C_COUNTRY_KING_Convone,							// 国王操作 : 召集
		MSG_S2C_COUNTRY_KING_Private,							// 国王操作 : 警告消息
		MSG_S2C_COUNTRY_KING_ChangeControl,						// 国王统御力变更
		MSG_S2C_COUNTRY_COUNTRY_Message,						// 国家信息公告
		MSG_S2C_COUNTRY_WORLD_Message,							// 世界信息公告
		MSG_S2C_COUNTRY_PRIVATE_Message,						// 个人信息公告

		// 国战
		MSG_S2C_COUNTRY_WAR_Start,								//战场开始
		MSG_S2C_COUNTRY_WAR_TimeOut,							//战场时间到
		MSG_S2C_COUNTRY_WAR_End,								//战场结束
		MSG_S2C_COUNTRY_WAR_UpdateCountrys,

		// 增值商店
		MSG_S2C_INCREMENTSHOP					= MSG_S2C + MAX_SUB * 16,
		// ----------------------------------------------------
		MSG_S2C_INCREMENTSHOP_OPEN,										//打开商店			
		MSG_S2C_INCREMENTSHOP_LIST,										//gameserver返回从商店中道具列表
		MSG_S2C_INCREMENTSHOP_BUY,										//gameserver返回从商店中购买一个物品的结果
		MSG_S2C_INCREMENTSHOP_CLOSE,							//##关闭个人商店

		MSG_S2C_INCREMENTSHOP_GETLOG_RESPONSE,					// 返回增值日志

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// For AuthServer messages, donot use these values between 
		// MSG_LS2AS_TYPE_START and MSG_LS2AS_TYPE_END.  - Kevin Lynx 3.6.2008
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MSG_LS2AS_TYPE_START = MAX_TYPE * 13,

		MSG_LS2AS_TYPE_END = MAX_TYPE * 14 - 1,
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////


		//-----------------------------------------------------------------------------------------------------
		// BillingServer <--> GameServer
		// Fox, 2008/03/06
		//-----------------------------------------------------------------------------------------------------

		MSG_S2B = MAX_TYPE * 15,									// GameServer --> BillingServer

		MSG_S2B_SERVER = MSG_S2B + MAX_SUB * 0,
		//-----------------------------------------------------------------------------------------------------
		MSG_S2B_SERVER_SENDSERVERINFO,								// GS is connecting

		MSG_S2B_BILLING = MSG_S2B + MAX_SUB * 1,
		//-----------------------------------------------------------------------------------------------------
		MSG_S2B_BILLING_AUTHEN,										// Client request for account info
		MSG_S2B_BILLING_INCBUY,										// Incrementshop buying
		MSG_S2B_BILLING_TRADE,										// GS request lock client account

		MSG_B2S = MAX_TYPE * 16,									// BillingServer --> GameServer

		MSG_B2S_BILLING = MSG_B2S + MAX_SUB * 0,
		//-----------------------------------------------------------------------------------------------------
		MSG_B2S_BILLING_AUTHEN,										// BS respond with client's account info
		MSG_B2S_BILLING_INCBUY,										// Incrementshop buying
		MSG_B2S_BILLING_TRADE,										// BS respond to foot the bill

		MSG_B2B = MAX_TYPE * 17,									// BillingServer --> BillingServer
		MSG_B2B_SERVER = MSG_B2B + MAX_SUB * 0,
		//-----------------------------------------------------------------------------------------------------
		MSG_B2B_SERVER_GAMESERVERLOST,								// GS lost process

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// For GMA tool messages, do NOT use these values between 
		// MSG_GM2AS_TYPE_START and MSG_GM2AS_TYPE_END.  - Kevin Lynx 4.22.2008
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MSG_GM2AS_TYPE_START = MAX_TYPE * 17,

		MSG_GM2AS_TYPE_END = MAX_TYPE * 18 - 1,
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	};

	struct stMsg
	{
		unsigned long lSize;		// 消息大小
		unsigned long eType;		// 消息类型
		unsigned long lTime;		// 时间戳(发送的时间)
		unsigned long lVerifyCode;	// 校验码
		//unsigned long ulReserveSize;// 保留数据的长度
		//char *pReserveDate;				// 保留数据（发送一个不定长的数据）


		//stMsg() {lSize=0;eType=0;lTime=0;ulReserveSize=(ULONG)strlen(pReserveDate)+1;pReserveDate=NULL;}
		//~stMsg() {}
	};

	////////////////////////////////////////////////////////////////////////
	//	基本数据
	////////////////////////////////////////////////////////////////////////

	vector<BYTE> m_MsgData;			// 消息的数据头指针

protected:
	void SetSize(unsigned long l)	{((stMsg*)GetMsgBuf())->lSize = l;}

public:
	void SetType(unsigned long t)	{((stMsg*)GetMsgBuf())->eType = t;}
	void SetTime(unsigned long l)	{((stMsg*)GetMsgBuf())->lTime = l;}
	void SetCode(unsigned long l)	{((stMsg*)GetMsgBuf())->lVerifyCode = l;}
	unsigned long GetSize()			{return ((stMsg*)GetMsgBuf())->lSize;}
	unsigned long GetType()			{return ((stMsg*)GetMsgBuf())->eType;}
	unsigned long GetMsgSendTime()	{return ((stMsg*)GetMsgBuf())->lTime;}
	unsigned long GetCode()			{return ((stMsg*)GetMsgBuf())->lVerifyCode;}
	//void SetReserve(const char *pIn) { strcpy( ((stMsg*)GetMsgBuf())->pReserveDate , pIn); ((stMsg*)GetMsgBuf())->ulReserveSize = (ULONG)strlen((const char*)pIn)+1; }
	//ULONG GetReserve(char *pOut) { pOut = ((stMsg*)GetMsgBuf())->pReserveDate; return (ULONG)strlen(pOut)+1; }

	BYTE* GetMsgBuf()	{ return &m_MsgData[0]; }

	void Update()
	{
		SetSize((unsigned long)m_MsgData.size());
	}


	////////////////////////////////////////////////////////////////////////
	//	添加和获取数据
	////////////////////////////////////////////////////////////////////////
public:
	void Add(char);
	void Add(BYTE);
	void Add(short);
	void Add(WORD);
	void Add(long);
	void Add(LONG64);
	void Add(DWORD);
	void Add(float);
	void Add(char*);
	void Add(const CGUID&	guid);
	void Add(void*, long size);
	void AddEx(void*, long size);//增加安全性，和GetEx配合使用
	void Reset(void);           //清空消息内容

	long m_lPtr;				// 当前获取的位置

	char GetChar();
	BYTE GetByte();
	short GetShort();
	WORD GetWord();
	long GetLong();
	LONG64 GetLONG64();
	DWORD GetDWord();
	float GetFloat();
	bool  GetGUID(CGUID& guid);
	//char* Get(char*);
	void* Get(void*, long size);
	void* GetEx(void*, long size);
	char* GetStr( char* pStr,long lMaxLen );


	////////////////////////////////////////////////////////////////////////
	//	消息处理
	////////////////////////////////////////////////////////////////////////

	CBaseMessage();
	virtual ~CBaseMessage();
	virtual long Run() = 0;            //纯虚函数，消息运行函数；

	////////////////////////////////////////////////////////////////////////
	//	消息压缩
	////////////////////////////////////////////////////////////////////////
protected:
	//	static BYTE s_pRLEBuffer[524288];
	static DWORD s_dwRLEBufferLen;
	static BYTE *s_pRLEBuffer;
	static DWORD s_dwTemptBufferLen;
	static BYTE *s_pTemptBuffer;
	static CRITICAL_SECTION m_CriticalSectionCreateMSG;
	static CRITICAL_SECTION m_CSTemptBuffer;

public:
	static bool Initial();
	static bool Release();

	static int DoTemptBuffer(void* pIN,int nAddPos,int nLen);
	static int DoRLE(void* pIn, BYTE* pOut, int len);			// 压缩
	static int DecodeRLE(BYTE* pIn, BYTE* pOut, int len);		// 解压
	static int DecodeRLE_SAFE(BYTE* pIn, BYTE* pOut, int nInBufLen,int nOutBufLen);		//解压,带数组边界检查

	friend class CMsgQueue;
};

#endif
