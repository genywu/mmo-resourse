//各种消息类型的定义
//王桥
//2007-3-1
// 反馈信息列表
#pragma once


enum
{
	//! 存储过程定义的信息代码
	///////////////////////////////
	//! 帐号未激活
	LOGIN_ACCOUNT_NOT_ACTIVE	= -13,
	//! 没找到用户所对应的记录
	LOGIN_NOTCDKEY			= -10,
	//! 用户被锁定
	LOGIN_ACCOUNT_IS_LOCKED	= -11,
	//! 密码错误
	LOGIN_PASSWORDERROR		= -12,
	//! 账号被封
	LOGIN_REJECTED			= -14,

	//! 程序定义的错误代码
	///////////////////////////////
	//! 验证对象没有准备好
	LOGIN_WAIT_INIT_FINISH		= -1000,
	//! 数据库操作对象错误
	LOGIN_DB_COM_ERROR,
	//! 不能连接到DB
	LOGIN_CANT_CONN_DB,
	//! DB操作失败
	LOGIN_DB_OPT_FAILED,
	//! 数据库维护中，不能通过账号验证（LS本地验证失败时发送）
	LOGIN_LOCAL_DB_FAILED,
	//!	WS被关闭
	LOGIN_WORLD_SERVER_BE_CLOSED,
	//! 健康游戏时间已用完
	LOGIN_NOTENOUGH_HEALTHY_TIME,
	//! 没有通过安全验证
	SECURITY_OPERATE_LOST,

	//! 所在IP登陆此角色的数量达到最大，将受到一段时间保护
	LOGIN_IP_LOGIN_MAX_SAME_CDKEY,
	//! 所在IP登陆的账号数量太多，将受到一段时间屏蔽
	LOGIN_IP_LOGIN_NIMIETY_CDKEY,
	//! 所在IP请求建立角色的次数太多，，将受到一段时间屏蔽
	LOGIN_IP_CREATE_MAX,

	//! 登陆方式错误（区别盛大账号和星漫账号）
	LOGIN_MODE_ERROR,
	//! 验证玩家信息失败
	LOGIN_AuthBusiness_ERROR,


	LOGIN_OK				= 1,	// 登陆成功
	LOGIN_LOGINSERVEROK,
	LOGIN_GAMESERVEROK,
	LOGIN_VERSIONERROR,				// 游戏版本错误
	LOGIN_CHECKOUT,					// 以有玩家登陆游戏
	LOGIN_NOTLOGINSERVER,           //没有登陆登陆服务器,
	LOGIN_LOGINSERVERPLAYERFULL,	// 登录的WS服务器玩家人数满了
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
	LOGIN_CREATEROLE_NAMEINVALID,	//名字非法，具有色情或政治上的非法字符。
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
	LOGIN_QUEST_PLAYERDATA_SIGNCODE_ERROR, // 验证码错误
};

enum
{
	MAX_SUB			= 0x100,				// 一个小类型的最大消息数量
	MAX_TYPE		= MAX_SUB * 0xff,		// 一个大类型的最大消息数量
};

enum eMsgTYpe
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
	MSG_W2L_SERVER_ANSWER_GsPlayerNum,						// 返回服务器人数

	MSG_W2L_SERVER_QUEST_RECONNECTION,

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
	MSG_C2L_LOG_VALIDATE_MATRIX,							// 客户端提交密保卡

	MSG_C2L_LOG_SEND_PT_TOKEN,								// 客户端发送盛大PT的token数据（请求登陆）
	MSG_C2L_LOGIN_AND_QUEST_PLAYERBASELIST,					//! 客户端请求直接进入选人界面（小退）

	MSG_C2L_LOG_Validate_Business,                          // 客户端要求进行二次资料验证

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
	MSG_W2W_SERVER_COSTSERVERCLOSE,
	MSG_W2W_RECONNECTCOSTSERVEROK,
	//MSG_W2W_SERVER_CLEAROFFLINEPLAYER,					// 存盘线程在存盘成功后发消息给进程清楚下线玩家列表

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
	MSG_L2W_LOG_BACKTOLOGINSTEP,							// Login Server通知World Server客户端退回登录界面。
	MSG_L2W_LOG_FCM_BEGIN,									// 玩家进入防沉迷状态，请求WS踢掉玩家
	MSG_L2W_LOG_FCM_TIME,									// 请求WS通知玩家还剩余多少时间

	MSG_L2W_SERVER				= MSG_L2W + MAX_SUB * 1,
	MSG_L2W_SERVER_QUEST_PING,								// ping服务器的信息，也是检测服务器是否正常
	MSG_L2W_SERVER_QUEST_RECONNECTION,

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
	MSG_S2W_SERVER_CREATEPHGOODS,							// 创建个人房屋物品
	MSG_S2W_SERVER_UPDATEPHGOODS,							// 更新个人房屋物品
	MSG_S2W_SERVER_DELETEPHGOODS,							// 删除个人房屋副本物品
	MSG_S2W_SERVER_SAVEPERSONALHOUSE,								// 保存个人房屋物品
	MSG_S2W_SERVER_LOADPERSONALHOUSE,								// 读取个人房屋物品

	// [AHC 080809 DBSMSG] 
	MSG_S2W_SERVER_QUEST_RECONNECTION,
	MSG_S2W_SERVER_PLAYER_PERSONALHOUSEGUID,
	MSG_S2W_JOE,

	MSG_S2W_LOG						= MSG_S2W + MAX_SUB * 1,
	// ----------------------------------------------------
	MSG_S2W_LOG_QUEST_PLAYERDATA,							// GameServer向WorldServer请求player详细数据
	MSG_S2W_LOG_PLAYERQUIT,									// gameserver向worldserver发送退出玩家的playerid

	MSG_S2W_PLAYER					= MSG_S2W + MAX_SUB * 2,
	// ----------------------------------------------------
	MSG_S2W_PLAYER_ADDSKILL,							
	MSG_S2W_PLAYER_DELSKILL,
	MSG_S2W_PLAYER_DELGOODS,
	MSG_S2W_PLAYER_ADDCREDIT,	//改变区域声望
	MSG_S2W_PLAYER_SETLEVEL,
	MSG_S2W_PLAYER_OCCU_CHANGE,


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
	MSG_S2W_OTHER_TALK_COUNTRY_UNION,						// 盟国喊话
	MSG_S2W_OTHER_TALK_NOTIFY,								// 国家通知
	MSG_S2W_OTHER_TALK_TOP_WORLD,							// 置顶世界喊话
	MSG_S2W_OTHER_GETLOG_REQUEST,							// 请求增值日值
	MSG_S2W_OTHER_GoodsInfo_Request,						// 请求物品信息
	MSG_S2W_OTHER_GoodsInfo_Create,							// 创建新的物品信息
	MSG_S2W_OTHER_UPDATEGLOBALSCTIPTVARS,					// 更新全局脚本变量]
	MSG_S2W_OTHER_NEWS_TOP_REGION,							// 区域置顶新闻
	MSG_S2W_OTHER_NEWS_TOP_WORLD,							// 全服置顶新闻
	MSG_S2W_OTHER_UPDATE_HIDE_FLAG,							// 更新玩家隐匿状态
	MSG_S2W_OTHER_WARNING,									// 遇敌报警通告
	MSG_S2W_OTHER_DEL_SPOUSE,								//! 跨服离婚检测
	MSG_S2W_OTHER_LIMIT_GOODS_REQUEST,						// gs向ws进行限购物品查询
	MSG_S2W_OTHER_AccusePlayer_Request,						// 举报信息
	MSG_S2W_OTHER_WATCH_INFO,								// 监视结果
	MSG_S2W_OTHER_REPLY_GM,									// 被监视玩家回复GM
    MSG_S2W_OTHER_CHANGEHOOKTIME,                           // 更新稳定挂机时间
	MSG_S2W_DEPOT					= MSG_S2W + MAX_SUB * 4,
	// ----------------------------------------------------
	MSG_S2W_DEPOT_OPEN,										// 开仓库

	MSG_S2W_GM						= MSG_S2W + MAX_SUB * 5,
	// ----------------------------------------------------
	MSG_S2W_GM_QUERY_PLAYERCOUNT,							//查询用户数量
	MSG_S2W_GM_QUERY_PLAYERDATA,							//查询用户数据
	MSG_S2W_GM_QUERY_PLAYER,								//查询用户数据(GUID)
	MSG_S2W_GM_QUERY_PLAYER_P,								//返回用户数据(GUID)
	MSG_S2W_GM_QUERY_PLAYER_PH,								//返回用户数据(GUID)
	MSG_S2W_GM_QUERY_PLAYER_T,								//返回用户数据(GUID)

	MSG_S2W_GM_ANSWER_PLAYERDATA,	
	MSG_S2W_GM_ANSWER_PLAYER,								//返回用户数据(GUID)
	MSG_S2W_GM_ANSWER_PLAYER_P,								//返回用户数据(GUID)
	MSG_S2W_GM_ANSWER_PLAYER_PH,							//返回用户数据(GUID)
	MSG_S2W_GM_ANSWER_PLAYER_T,								//返回用户数据(GUID)

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
	MSG_S2W_GM_GETPLAYER_REGION,							// 跨服得到Player的场景信息
	MSG_S2W_GM_ANSWERGETPLAYER_REGION,						// 回复跨服得到Player的场景信息
	MSG_S2W_GM_CHANGETPLAYER_REGION,						// 跨服切换Player的场景
	MSG_S2W_GM_ANSWERCHANGETPLAYER_REGION,					// 回复跨服切换Player的场景
	MSG_S2W_GM_QUERY_REGIONISEXIT,							// 查询场景是否存在

	MSG_S2W_TEAM					= MSG_S2W + MAX_SUB * 6,
	// ----------------------------------------------------
	MSG_S2W_TEAM_QUERY_JOIN,								//! 发出组队邀请
	MSG_S2W_TEAM_QUERY_JOIN_BYNAME,							//! 发出组队邀请
	MSG_S2W_TEAM_ANSWER_JOIN,								//! 被邀请方回复
	MSG_S2W_TEAM_CHANGE_LEADER,								//! 改变队长
	MSG_S2W_TEAM_SetGoods,									//! 物品分配设置
	MSG_S2W_TEAM_RECRUIT,									//! 招募控制
	MSG_S2W_TEAM_BeRecruited,								//! 应征
	MSG_S2W_TEAM_CHAT,										//! 队聊
	MSG_S2W_TEAM_MemberAllIdioinfo,							//! 请求中转的对象详细信息
	MSG_S2W_TEAM_MemberPos,									//! 请求中转位置信息

	MSG_S2W_TEAM_SwapRgnID,									//! 请求转发移交队伍副本ID数据
	MSG_S2W_TEAM_RequestSwapRgnID,							//! 请求交换两个玩家的所有队伍副本ID
	MSG_S2W_TEAM_AddRgnID,									//! 添加一个队伍的副本ID
	MSG_S2W_TEAM_DelRgnID,									//! 删除一个队伍的副本ID
	MSG_S2W_TEAM_ClearRgnID,								//! 清除一个队伍的副本ID

	MSG_S2W_TEAM_GetRgnNum,									//! 获取一个玩家的队伍副本数量
	MSG_S2W_TEAM_ReturnGetRgnNum,							//! 返回一个玩家的队伍副本数量
	MSG_S2W_TEAM_GetRgnGuid,								//! 获取一个玩家的队伍副本GUID by Index
	MSG_S2W_TEAM_ReturnGetRgnGuid,							//! 返回一个玩家的队伍副本GUID
	MSG_S2W_TEAM_GetRgnTemplateID,							//! 获取一个玩家的队伍副本模板ID by Index
	MSG_S2W_TEAM_ReturnGetRgnTemplateID,					//! 返回一个玩家的队伍副本模板ID

	MSG_S2W_TEAM_KICK_PLAYER,								//! 踢人
	MSG_S2W_TEAM_INLVLDIFFERENCE,							//! 判断队员最大等级差
	MSG_S2W_TEAM_INLVLSPACE,								//! 判断所有队员的等级是否在输入的范围内

	MSG_S2W_TEAM_Recommend_ByName,							//! 客户端请求推荐


	

	MSG_S2W_ORGSYS					= MSG_S2W + MAX_SUB * 7,
	// ----------------------------------------------------
	MSG_S2W_ORGSYS_CreateFaction_Reques,					//! 请求WS创建工会
	MSG_S2W_ORGSYS_UpgradeFaction_Reques,					//! 请求WS升级工会
	MSG_S2W_ORGSYS_DisbandFaction_Reques,					//! 请求删除帮会
	MSG_S2W_Faction_InvitePlayer_Reques,					//! 请求发送工会邀请请求
	MSG_S2W_Faction_AddMember,								//! 请求添加一个成员
	MSG_S2W_ORGSYS_SetJobName_Request,						//! 设置官阶名称
	MSG_S2W_ORGSYS_SetJobPurview_Request,					//! 设置官阶权限
	MSG_S2W_ORGSYS_Quit_Request,							//! 成员退出
	MSG_S2W_Faction_SetPronounce_Reques,					//! 请求修改公告
	MSG_S2W_Faction_SetLeaveword_Reques,					//! 请求修改备忘
	MSG_S2W_Faction_Fire_Reques,							//! 请求开除成员
	MSG_S2W_ORGSYS_SetMemberJob_Request,					//! 设置成员官阶
	MSG_S2W_Faction_SetMemberTitle_Reques,					//! 设置成员官阶
	MSG_S2W_Faction_SetNewMaster_Reques,					//! 设置会长
	MSG_S2W_Faction_CheckRequisition_Reques,				//! 申请列表操作请求
	MSG_S2W_ORGSYS_JoinFac_Request,							//! 玩家申请操作
	MSG_S2W_Faction_UpLoadIcon_Reques,						//! 玩家上传工会图标

	MSG_S2W_Faction_ChangeRes_Reques,						//! 修改资源请求
	MSG_S2W_Faction_ChangeExp_Reques,						//! 修改经验请求
	MSG_S2W_Faction_ChangeMemberContribute_Reques,			//! 修改成员贡献请求
	MSG_S2W_Faction_ChangeTickets,							//	修改据点门票数
	MSG_S2W_Faction_ChangeBaseBattle,						//	修改据点战功值
	MSG_S2W_Faction_ChangeBase,								//	据点增删
	MSG_S2W_Faction_ChangeBattle_Reques,					//	修改战功总值
	MSG_S2W_Faction_ChangeArmyInvest_Reques,				//	修改军事投资值
	MSG_S2W_Faction_ChangeJoinBattle,						//	修改成员参与权
	MSG_S2W_Faction_ChangeTechLevel,						//	修改家族科技等级
	MSG_S2W_Faction_ChangeEnemy,							//	修改敌对家族信息	

	//任务相关
	MSG_S2W_QUEST_PlayerAddQuest,							//给某个玩家添加一个任务
	MSG_S2W_QUEST_PlayerRemoveQuest,						//给某个玩家移出一个任务
	MSG_S2W_QUEST_PlayerRunScript,							//给一个玩家触发一个脚本

	MSG_S2W_SetFacParam_ByMasterPlayerID,					//设置行会参数值
	MSG_S2W_SetFacParam_ByFactionID,
	MSG_S2W_SetFacParam_ByFactionName,
	MSG_S2W_ChangeFacParam_ByMasterPlayerID,				//改变行会参数值
	MSG_S2W_ChangeFacParam_ByFactionID,
	MSG_S2W_ChangeFacParam_ByFactionName,

	//! 联盟
	MSG_S2W_UNION_Request_Create,							//! 创建联盟请求
	MSG_S2W_UNION_Request_Disband,							//! 解散联盟请求
	MSG_S2W_UNION_Request_Quit,								//! 退出联盟请求
	MSG_S2W_UNION_Request_Kick,								//! 开除联盟请求
	MSG_S2W_UNION_Request_SetChairman,						//! 传位
	MSG_S2W_UNION_Request_SetPurview,						//! 设置权限
	MSG_S2W_UNION_Request_Invit,							//! 邀请
	MSG_S2W_UNION_Request_AnswerInvit,						//! 回答邀请
	
	//赏金任务共享
	MSG_S2W_SHARE_QUESTLIST_UPDATE,							//更新共享任务列表
	MSG_S2W_SHARE_CURRQUEST,								//更新当前任务
	MSG_S2W_SHARE_DELQUEST,									//删除共享列表中指定任务
	MSG_S2W_SHARE_SETMEMBERQUEST,							//设置队员当前已成完成共享的任务

	// 日志
	MSG_S2W_WRITELOG				= MSG_S2W + MAX_SUB * 8,//日志
	// ----------------------------------------------------

	//计费
	MSG_S2W_COST					= MSG_S2W + MAX_SUB * 9,
	// -----------------------------------------------------

	// 点卡寄售 s->w
	MSG_S2W_CARD					= MSG_S2W + MAX_SUB * 10,
	// -----------------------------------------------------

	// 国家
	MSG_S2W_COUNTRY					= MSG_S2W + MAX_SUB * 11,
	// -----------------------------------------------------
	MSG_S2W_COUNTRY_CHANGE,									// 改变国籍
	MSG_S2W_COUNTRY_POWER_CHANGE,
	MSG_S2W_COUNTRY_TECHLEVEL_CHANGE,
	MSG_S2W_COUNTRY_KING_CHANGE,

	// Region 
	MSG_S2W_RGN					= MSG_S2W + MAX_SUB * 12,
	// -----------------------------------------------------
	MSG_S2W_RGN_CHANGE,									// 

	MSG_S2W_RGN_CREATE,
	MSG_S2W_RGN_CREATE_QUEST,  // 从GS0到GS1上创建，GS1返回GS0的消息

	MSG_S2W_RGN_DELETE,
	MSG_S2W_RGN_DELETE_QUEST,  // 从GS0到GS1上删除，GS1返回GS0的消息

	MSG_S2W_RGN_VAR_ADD,
	MSG_S2W_RGN_VAR_DEL,
	MSG_S2W_RGN_VAR_SET,
	MSG_S2W_RGN_VAR_GET,

	// 跨服操作
	MSG_S2W_RGN_VAR_ADD_ANSWER,
	MSG_S2W_RGN_VAR_DEL_ANSWER,
	MSG_S2W_RGN_VAR_SET_ANSWER,
	MSG_S2W_RGN_VAR_GET_ANSWER,

	MSG_S2W_RGN_VIEWRGNVAR,
	MSG_S2W_RGN_VIEWRGNNUM,
	MSG_S2W_RGN_VIEWRGNDATA,
	MSG_S2W_RGN_VIEWRGNAVLID, // 查看场景是否存在

	MSG_S2W_RGN_GETTEMPLATEID,	//返回指定副本ＩＤ的副本的蓝图ＩＤ
	MSG_S2W_RGN_GETDUPTYPE,		//返回指定副本ＩＤ的副本的类型
	MSG_S2W_RGN_GETDUPRGNONWER, //返回指定副本ＩＤ的副本的所有者ＩＤ。

	MSG_S2W_RGN_VIEWPLAYERPID, // 查看玩家的个人副本
	MSG_S2W_RGN_VIEWPLAYERPHID, // 查看玩家的个人房屋副本
	MSG_S2W_RGN_VIEWPLAYERTID, // 查看玩家的组队副本
	MSG_S2W_RGN_VIEWGSPLAYERNUM,// 查看指定GS的某场景人数
	MSG_S2W_RGN_ANSWERVIEWGSPLAYERNUM,// 回应WS的MSG_W2S_RGN_QUESTVIEWGSPLAYERNUM查看指定GS的某场景人数
	MSG_S2W_RGN_QUESTPLAYERRGNLIST,// 查询WS的副本是否存在
	MSG_S2W_RGN_ANSWERWSQUESTPLAYERRGNLIST,// 回应WS查询玩家副本list
	MSG_S2W_RGN_ANSWERGSUPDATEPLAYERRGNLIST,// 回应更新GS玩家副本list

	MSG_S2W_RGN_UPDATE_PLAYER_TEAM_RGNGUID, //! 更新一个玩家的队伍副本数据
	MSG_S2W_RGN_CAMP_PLAYER_NUM_QUEST,	// 查询指定场景的指定阵营玩家数量
	MSG_S2W_RGN_CAMP_PLAYER_NUM_ANSWER,	// 回应指定场景的指定阵营玩家数量

	// DBEntity Operation 
	MSG_S2W_OPER					= MSG_S2W + MAX_SUB * 13, 
	// -----------------------------------------------------
	MSG_S2W_OPER_PLAYER,		// 操作玩家
	MSG_S2W_OPER_UNION,			// 操作联盟

	// DBEntity Operation 
	MSG_S2W_PLAYER_DETAIL_SAVE					= MSG_S2W + MAX_SUB * 14,
	// -----------------------------------------------------
	// 操作玩家属性(包括登录属性和详细属性),使用一个32位标志处理不同层属性
	// 0x00000001:property,0x00000002:EQUIPMENT,
	// 0x00000004:ORIGINALPACK,0x00000008:SUBPACK
	// 0x00000008:VARLIST,0x00000010:QUESTLIST,
	// 0x00000020:SKILLLIST,0x00000040:STATELIST
	// 组合使用:
	// 0x0x00000006:ORIGINALPACK+EQUIPMENT...
	MSG_S2W_PLAYER_DETAIL_SAVE_PROPERTY,


	MSG_S2W_REGION_DETAIL_SAVE	=MSG_S2W + MAX_SUB*15,
	// -----------------------------------------------------



	//! 好友系统
	MSG_S2W_LINKMAN					= MSG_S2W + MAX_SUB * 16,
	// -----------------------------------------------------
	MSG_S2W_LINKMAN_ADD,					//! 添加联系人
	MSG_S2W_LINKMAN_DEL,					//! 删除联系人

	MSG_S2W_LINKMAN_MAP_CHANGE,				//! 通知WS玩家地图改变
	MSG_S2W_LINKMAN_DATA,					//! 一个玩家的联系人数据

	MSG_S2W_LINKMAN_SYSTEM_REQUEST_ADD,		//! 系统添加联系人请求
	MSG_S2W_LINKMAN_PALYER_REQUEST_ADD,		//! 玩家添加联系人请求
	MSG_S2W_LINKMAN_PALYER_RE_ADD,			//! 玩家回复添加联系人请求
	MSG_S2W_LINKMAN_PALYER_REQUEST_MOVE,	//! 玩家移动联系人请求
	MSG_S2W_LINKMAN_PALYER_REQUEST_DEL,		//! 玩家删除联系人请求
	//!邮件系统
	MSG_S2W_MAIL					= MSG_S2W + MAX_SUB * 17,
	// -----------------------------------------------------
	MSG_S2W_MAIL_SEND,						//! 发信
	MSG_S2W_MAIL_RECEIVE,					//! 收信
	MSG_S2W_MAIL_OPERA_GOODS,				//! 邮件中物品处理
	MSG_S2W_MAIL_OPERA_GOLD,				//! 邮件中的金币处理
	MSG_S2W_MAIL_DELETE,					//! 删除邮件
	MSG_S2W_MAIL_PAY,						//! 付款收信
	MSG_S2W_MAIL_REJECT,					//! 退信
	MSG_S2W_MAIL_READ,						//! 看信
	MSG_S2W_MAIL_SEND_RECEIVER,				//! 发信给某个玩家
	MSG_S2W_MAIL_PRESENT_RECEIVER,			//! 赠送类邮件发送
	

	// Fox@20081107----------------------------------------------
	// 玩家管理系统( PlayerManager )
	MSG_S2W_PM						= MSG_S2W + MAX_SUB * 18,
	// -----------------------------------------------------
	MSG_S2W_PM_ADD,							// GS请求增加玩家信息
	MSG_S2W_PM_CHG,							// GS请求刷新玩家信息
	MSG_S2W_PM_REQ,							// GS请求查询玩家信息

	/// 商业系统消息
	MSG_S2W_BUSINESS = MSG_S2W + MAX_SUB * 19,
	// -----------------------------------------------------
	MSG_S2W_BUSINESS_SAVE,	/// GS主动发起存盘数据库请求


	//! 战队系统
	MSG_S2W_SENTAI = MSG_S2W + MAX_SUB * 20,
	// -----------------------------------------------------
	MSG_S2W_SENTAI_REQUEST_QUEUE,			//! 请求进入或退出排队
	MSG_S2W_SENTAI_REQUEST_CHANGE_POINT,	//! 请求修改积分
	MSG_S2W_SENTAI_END_MATCH,				//! 结束一个场地的比赛
	MSG_S2W_SENTAI_REQUEST_PLAYER_STATE,	//! 查询玩家的战队状态
	MSG_S2W_SENTAI_END_TEAM_MATCH,			//! 结束一个队伍的比赛

	//！彩票系统
	MSG_S2W_LOTTERY = MSG_S2W + MAX_SUB * 21,
	// -----------------------------------------------------
	MSG_S2W_LOTTERY_CHANGEWORLDCREDIT,      //! 通知改变世界积分
	MSG_S2W_LOTTERY_CHANGEPRECEDE,          //! 改变累计积分领先记录
	MSG_S2W_LOTTERY_WORLDINFO,              //! 彩票系统世界信息

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
	MSG_S2S_SERVER_RECONNECDTWORLDSERVER,					//请求重连WorldServer

	MSG_S2S_LOG						= MSG_S2S + MAX_SUB * 1,
	// ----------------------------------------------------
	MSG_S2S_LOG_PLAYER_ONLOST,								// 服务器在接受到客户端断线消息时给自己发一条消息	

	MSG_S2S_TEAM					= MSG_S2S + MAX_SUB * 2,
	// ----------------------------------------------------
	//MSG_S2S_TEAM_TEAMMATE_MIGRATE_IN,						//转移
	//MSG_S2S_TEAM_TEAMMATE_MIGRATE_OUT,						//转移
	//MSG_S2S_TEAM_TEAMMATE_LOST,								//队员离队

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
	MSG_W2S_SERVER_BUSINESS_UPDATE,
	MSG_W2S_SERVER_BUSINESS_BUYLIMIT_CLEAR,

	MSG_W2S_SERVER_QUEST_COLLECT_PLAYERDATA,				// 请求GameServer把在线玩家的数据发送过来
	MSG_W2S_SERVER_QUEST_PING,								// ping服务器的信息，也是检测服务器是否正常
	MSG_W2S_SERVER_CREATEMONSTER,							// 创建怪物
	MSG_W2S_SERVER_CREATEPHGOODS,							// 创建个人房屋副本物品
	MSG_W2S_SERVER_UPDATEPHGOODS,							// 更新个人房屋物品
	MSG_W2S_SERVER_DELETEPHGOODS,							// 删除个人房屋副本物品
	MSG_W2S_SERVER_SAVEPERSONALHOUSE,								// 保存个人房屋物品
	MSG_W2S_SERVER_LOADPERSONALHOUSE,								// 读取个人房屋物品

	// [AHC DBSMSG]
	MSG_W2S_SERVER_QUEST_RECONNECTION,
	MSG_W2S_SERVER_PLAYER_PERSONALHOUSEGUID,               // 查询玩家的个人房屋数据

	MSG_W2S_LOG						= MSG_W2S + MAX_SUB * 1,
	// ----------------------------------------------------
	MSG_W2S_LOG_ANSWER_PLAYERDATA,							// WorldServer向GameServer返回player详细数据
	MSG_W2S_LOG_KICKPLAYER,									//worldserver向gameserver发送一条要求踢出某个玩家的消息
	MSG_W2S_LOG_REPEAT_LOGIN,								//World Server通知Game Server有一连接请求登陆一帐号并替换掉原来使用该帐号的连接。
	MSG_W2S_LOG_FCM_TIME,									// WS向GS通知玩家防沉迷时间

	MSG_W2S_OTHER					= MSG_W2S + MAX_SUB * 2,
	// ----------------------------------------------------
	MSG_W2S_OTHER_TALK,										// 对话消息
	MSG_W2S_OTHER_FACTIONTALK,								// 行会聊天
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
	MSG_W2S_OTHER_KICKPLAYER,								// 防沉迷系统踢人
	MSG_W2S_OTHER_UPDATEGLOBALSCTIPTVARS,					// 更新全局脚本变量


	MSG_W2S_OTHER_CHANGENAME,								// Ws更名后发送结果到Gs
	MSG_W2S_OTHER_TALK_WORLD,								// 世界喊话
	MSG_W2S_OTHER_TALK_COUNTRY,								// 国家喊话
	MSG_W2S_OTHER_TALK_COUNTRY_UNION,						// 盟国喊话
	MSG_W2S_OTHER_TALK_NOTIFY,								// 国家通知
	MSG_W2S_OTHER_TALK_TOP_WORLD,							// 置顶世界喊话	
	MSG_W2S_OTHER_GETLOG_RESPONSE,							// 返回给gs增值日志
	MSG_W2S_OTHER_GoodsInfo_Re,								// 返回物品信息
	MSG_W2S_OTHER_NEWS_TOP_REGION,							// 区域新闻
	MSG_W2S_OTHER_NEWS_TOP_WORLD,							// 世界新闻

	MSG_W2S_OTHER_WARNING,									// 遇敌报警通告
	MSG_W2S_OTHER_NEW_DAY,									// 通知GS新的一天到了
	MSG_W2S_OTHER_DEL_SPOUSE,								//! 离婚通知
	MSG_W2S_OTHER_LIMIT_GOODS_RESPONSE,						// ws返回限购物品查询
	MSG_W2S_OTHER_LIMIT_GOODS_UPDATA,						//WS通知所有GS更新限量信息
	MSG_W2S_OTHER_WATCH,									//请求监视某个玩家
	MSG_W2S_OTHER_REPORT_RESLUT,							//举报处理结果
	MSG_W2S_OTHER_GM_CHAT,									//GM与被监视玩家密聊

	MSG_W2S_DEPOT					= MSG_W2S + MAX_SUB * 3,
	// ----------------------------------------------------
	MSG_W2S_DEPOT_OPEN,

	MSG_W2S_GM						= MSG_W2S + MAX_SUB * 4,
	// ----------------------------------------------------
	MSG_W2S_GM_ANSWER_PLAYERCOUNT,							//返回用户数量
	MSG_W2S_GM_ANSWER_PLAYERDATA,							//返回用户数据
	MSG_W2S_GM_ANSWER_PLAYER,								//返回用户数据(GUID)
	MSG_W2S_GM_ANSWER_PLAYER_P,								//返回用户数据(GUID)
	MSG_W2S_GM_ANSWER_PLAYER_PH,							//返回用户数据(GUID)
	MSG_W2S_GM_ANSWER_PLAYER_T,								//返回用户数据(GUID)

	MSG_W2S_GM_QUERY_PLAYERDATA,
	MSG_W2S_GM_QUERY_PLAYER,								//返回用户数据(GUID)
	MSG_W2S_GM_QUERY_PLAYER_P,								//返回用户数据(GUID)
	MSG_W2S_GM_QUERY_PLAYER_PH,								//返回用户数据(GUID)
	MSG_W2S_GM_QUERY_PLAYER_T,								//返回用户数据(GUID)

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
	MSG_W2S_GM_GETPLAYER_REGION,
	MSG_W2S_GM_ANSWER_GETPLAYER_REGION,						// 返回获得玩家场景结果
	MSG_W2S_GM_CHANGEPLAYER_REGION,
	MSG_W2S_GM_ANSWER_CHANGEPLAYER_REGION,					// 返回获得玩家场景结果
	MSG_W2S_GM_ANSWER_REGIONISEXIT,							// 返回场查询场景是否存在

	MSG_W2S_TEAM					= MSG_W2S + MAX_SUB * 5,
	// ----------------------------------------------------
	MSG_W2S_TEAM_InitTeamSet,								//初始化队伍配置

	MSG_W2S_TEAM_ToPlayerInfo,								//! 转发送文字信息
	MSG_W2S_TEAM_QUERY_JOIN,								//! 转发邀请信息
	MSG_W2S_TEAM_PlayerJoin,								//! 玩家加入队伍
	MSG_W2S_TEAM_MastterChange,								//! 队长变更
	MSG_W2S_TEAM_MemberStateChange,							//! 队员在线状态改变
	MSG_W2S_TEAM_ServerChange,								//! 成员切换服务器
	MSG_W2S_TEAM_MemberExit,								//! 队员退出
	MSG_W2S_TEAM_Disband,									//! 解散
	MSG_W2S_TEAM_GoodsSetChange,							//! 物品分配设置修改
	MSG_W2S_TEAM_RECRUIT,									//! 招募状态改变
	MSG_W2S_TEAM_MemberAllIdioinfo,							//! 转发队员详细信息
	MSG_W2S_TEAM_MemberPos,									//! 转发队员位置信息
	MSG_W2S_TEAM_LEVELCHANGE,								//! 广播队员等级变化
	MSG_W2S_TEAM_OCCUCHANGE,								//! 广播队员职业变化

	MSG_W2S_TEAM_SetupInit,									//! 初始化队伍设置
	MSG_W2S_TEAM_DataInit,									//! 初始化队伍数据

	MSG_W2S_TEAM_SwapRgnID,									//! 转发 队伍副本ID数据（源数据已包含在消息中）
	MSG_W2S_TEAM_GetSrcSwapRgnID,							//! 转发 获取交换队伍副本ID数据的源数据
	MSG_W2S_TEAM_AddRgnID,									//! 转发 添加玩家的队伍副本ID
	MSG_W2S_TEAM_DelRgnID,									//! 转发 删除玩家的队伍副本ID
	MSG_W2S_TEAM_ClearRgnID,								//! 转发 清除玩家的队伍副本ID

	MSG_W2S_TEAM_GetRgnNum,									//! 转发 获取一个玩家的队伍副本数量
	MSG_W2S_TEAM_ReturnGetRgnNum,							//! 转发 返回一个玩家的队伍副本数量
	MSG_W2S_TEAM_GetRgnGuid,								//! 转发 获取一个玩家的队伍副本GUID by Index
	MSG_W2S_TEAM_ReturnGetRgnGuid,							//! 转发 返回一个玩家的队伍副本GUID
	MSG_W2S_TEAM_GetRgnTemplateID,							//! 转发 获取一个玩家的队伍副本模板ID by Index
	MSG_W2S_TEAM_ReturnGetRgnTemplateID,					//! 转发 返回一个玩家的队伍副本模板ID

	MSG_W2S_TEAM_CREATE,									// 队伍建立
	MSG_W2S_TEAM_CHAT,										// 队聊

	MSG_W2S_TEAM_INLVLDIFFERENCE,
	MSG_W2S_TEAM_INLVLSPACE,

	MSG_W2S_TEAM_Recommend_ByName,							//! 转发推荐入会
	MSG_W2S_TEAM_ChangeOptControl,							//! 修改操作控制

	MSG_W2S_TEAM_SentaiUpdate,								//! 战队积分更新


	MSG_W2S_ORGSYS					= MSG_W2S + MAX_SUB * 6,
	// ----------------------------------------------------

	MSG_W2S_ORGSYS_InitGsFaction,							//! 初始化GS行会消息
	MSG_W2S_ORGSYS_InitPlayerFacSys,						//! 通知GS向玩家发送初始化工会信息
	MSG_W2S_ORGSYS_CreateFaction_Re,						//! 回复GS创建结果
	MSG_W2S_ORGSYS_DelFaction_Notify,						//! 删除行会通知
	MSG_W2S_Faction_InvitePlayer_Re,						//! 邀请回复
	MSG_W2S_Faction_AddMember_Notify,						//! 添加新会员通知
	MSG_W2S_ORGSYS_SetJobName_Notify,						//! 修改职级名称通知
	MSG_W2S_ORGSYS_SetJobPurview_Notify,					//! 修改职级权限通知
	MSG_W2S_ORGSYS_Quit_Notify,								//! 成员退出通知
	MSG_W2S_Faction_EditPronounce_Notify,					//! 编辑公告通知
	MSG_W2S_Faction_EditLeaveword_Notify,					//! 编辑备忘通知
	MSG_W2S_ORGSYS_Fire_Notify,								//! 开除成员通知
	MSG_W2S_ORGSYS_SetMemJobLvl_Notify,						//! 设置成员职级
	MSG_W2S_Faction_SetMemberTitle_Notify,					//! 设置成员称号通知
	MSG_W2S_Faction_SetMaster_Notify,						//! 设置新会长通知
	MSG_W2S_Faction_OnlineChange_Notify,					//! 成员在线状态改变
	MSG_W2S_Faction_MemberMap_Notify,						//! 成员地图通知
	MSG_W2S_Faction_MemberLevel_Notify,						//! 成员等级通知 
	MSG_W2S_Faction_MemberHide_Notify,						//! 成员隐匿通知
	MSG_W2S_Faction_MemberOccu_Notify,						//! 成员职业通知
	MSG_W2S_Faction_Applicant_Notify,						//! 操作申请者通知
	MSG_W2S_Faction_Join_Re,								//! 回复申请加入请求（错误才回复）
	MSG_W2S_Faction_LvlChange_Notify,						//! 工会等级改变通知
	MSG_W2S_Faction_IconChange_Notify,						//! 工会图标数据修改通知
	MSG_W2S_Faction_ClearBase_Notify,						//!	清空据点信息,只保留据点名称
	MSG_W2S_Faction_ChangeRes_Notify,						//! 修改资源通知
	MSG_W2S_Faction_ChangeExp_Notify,						//! 修改经验通知
	MSG_W2S_Faction_ChangeMemberContribute_Notify,			//! 修改成员贡献通知
	MSG_W2S_Faction_ChangeRes_Re,							//! 修改资源回复
	MSG_W2S_Faction_ChangeExp_Re,							//! 修改经验回复
	MSG_W2S_Faction_ChangeMemberContribute_Re,				//! 修改成员贡献回复
	MSG_W2S_Faction_ChangeTickets_Re,						//	修改据点门票数
	MSG_W2S_Faction_ChangeBaseBattle_re,					//	修改据点战功值
	MSG_W2S_Faction_ChangeBase_Re,							//	据点增删
	MSG_W2S_Faction_ChangeBattle_Re,						//	修改战功总值
	MSG_W2S_Faction_ChangeArmyInvest_Re,					//	修改军事投资值
	MSG_W2S_Faction_ChangeJoinBattle_Re,					//	修改成员参与权
	MSG_W2S_Faction_ChangeTechLevel_Re,						//	修改家族科技等级
	MSG_W2S_Faction_ChangeEnemy_Re,							//	修改敌对家族信息

	MSG_W2S_Faction_BoonRun_Notify,							//! 通知GS执行福利

	MSG_W2S_QUEST_PlayerAddQuest,							//给某个玩家添加一个任务
	MSG_W2S_QUEST_PlayerRemoveQuest,						//给某个玩家移出一个任务
	MSG_W2S_QUEST_PlayerRunScript,							//给某个玩家运新一个脚本

	MSG_W2S_Union_InitGs,									//! 初始化GS行会消息
	MSG_W2S_Union_Create_Notify,							//! 回复GS创建结果
	MSG_W2S_Union_Disband_Notify,							//! 解散
	MSG_W2S_Union_Quit_Notify,								//! 退出
	MSG_W2S_Union_Kick_Notify,								//! 开除
	MSG_S2W_Union_SetChairman_Notify,						//! 传位
	MSG_S2W_Union_SetPurview_Notify,						//! 设置权限
	MSG_S2W_Union_Invit_Re,									//! 回复邀请者
	MSG_S2W_Union_TransferInvit,							//! 转发邀请
	MSG_S2W_Union_TransferAnswer,							//! 转发回复
	MSG_S2W_Union_NewMemberJoin,							//! 新成员加入
	
	//赏金任务
	MSG_W2S_SHARE_QUESTLIST_UPDATE,						//更新共享任务列表
	MSG_W2S_SHARE_CURRQUEST,							//更新当前任务
	MSG_W2S_SHARE_SETMEMBERQUEST,		

	//计费
	MSG_W2S_COST					= MSG_W2S + MAX_SUB * 7,  
	//---------------------------------------------------------------------------

	// 点卡寄售  w->s
	MSG_W2S_CARD					= MSG_W2S + MAX_SUB * 8,  
	//---------------------------------------------------------------------------

	// 国家
	MSG_W2S_COUNTRY					= MSG_W2S + MAX_SUB * 9,  
	//---------------------------------------------------------------------------
	MSG_W2S_COUNTRY_CHANGE,									// 更改国籍
	MSG_W2S_COUNTRY_POWER_CHANGE,
	MSG_W2S_COUNTRY_TECHLEVEL_CHANGE,
	MSG_W2S_COUNTRY_KING_CHANGE,

	//! 好友系统
	MSG_W2S_LINKMAN					= MSG_W2S + MAX_SUB * 10,  
	//---------------------------------------------------------------------------
	MSG_W2S_LINKMAN_PLAYER_INITDATA,			//! 一个玩家的初始化数据
	MSG_W2S_LINKMAN_PLAYER_RELEASEDATA,			//!	清除玩家数据通知
	MSG_W2S_LINKMAN_PLAYER_LOGIN,				//! 一个联系人登陆
	MSG_W2S_LINKMAN_PLAYER_LOGOUT,				//! 一个联系人离线
	MSG_W2S_LINKMAN_PLAYER_INFO_CHANGE,			//! 一个玩家的信息改变
	MSG_W2S_LINKMAN_PLAYER_DELSTATE_CHANGE,		//! 一个玩家的删除状态改变

	MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_NOTIFY,	//! 玩家请求添加联系人通知
	MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_RE,		//! 玩家请求添加联系人回复
	MSG_W2S_LINKMAN_ADD_NOTIFY,					//! 添加联系人通知
	MSG_W2S_LINKMAN_MOVE_NOTIFY,				//! 移动联系人通知
	MSG_W2S_LINKMAN_DEL_NOTIFY,					//! 删除联系人通知

	MSG_W2S_LEAVEWORD_SAVE_RESULT,				//! 留言结果
	MSG_W2S_LEAVEWORD_POP,						//! 取出的留言
	//! 邮件系统
	MSG_W2S_MAIL					= MSG_W2S + MAX_SUB * 11,
	MSG_W2S_MAIL_SEND_FAILED,					//! 发信失败
	MSG_W2S_MAIL_SEND,							//! 发信
	MSG_W2S_MAIL_OPERA_GOODS,					//! 物品操作
	MSG_W2S_MAIL_OPERA_GOLD,					//! 金钱操作
	MSG_W2S_MAIL_DELETE,						//! 删除信件
	MSG_W2S_MAIL_RECEIVE,						//! 收信
	MSG_W2S_MAIL_PAY,							//! 付款收信
	MSG_W2S_MAIL_SEND_RESULT,					//! 发信结果
	MSG_W2S_MAIL_SEND_SYS,						//! 群发系统邮件
	MSG_W2S_MAIL_CHANGE_TO_SERVER,  			//! 邮件转移

	// Fox@20081107----------------------------------------------
	// 玩家管理系统( PlayerManager )
	MSG_W2S_PM						= MSG_W2S + MAX_SUB * 12,
	// -----------------------------------------------------
	MSG_W2S_PM_CHG,							// WS响应刷新玩家信息
	MSG_W2S_PM_ACK,							// WS响应查询玩家信息

	// Region 
	MSG_W2S_RGN					= MSG_W2S + MAX_SUB * 13,
	// -----------------------------------------------------
	MSG_W2S_RGN_CHANGE,									// 

	MSG_W2S_RGN_CREATE,
	MSG_W2S_RGN_CREATE_ANSWER,  // 从GS0到GS1上创建，WS向GS1发送的消息


	MSG_W2S_RGN_DELETE,
	MSG_W2S_RGN_DELETE_ANSWER,  // 从GS0到GS1上创建，WS向GS1发送的消息

	MSG_W2S_RGN_VAR_ADD,
	MSG_W2S_RGN_VAR_DEL,
	MSG_W2S_RGN_VAR_SET,
	MSG_W2S_RGN_VAR_GET,

	// 跨服操作
	MSG_W2S_RGN_VAR_ADD_QUEST,
	MSG_W2S_RGN_VAR_DEL_QUEST,
	MSG_W2S_RGN_VAR_SET_QUEST,
	MSG_W2S_RGN_VAR_GET_QUEST,

	MSG_W2S_RGN_VIEWRGNVAR,
	MSG_W2S_RGN_VIEWRGNNUM,
	MSG_W2S_RGN_VIEWRGNDATA,
	MSG_W2S_RGN_VIEWTOTALDUPRGNNUM,
	MSG_W2S_RGN_VIEWRGNAVLID, // 查看场景是否存在

	MSG_W2S_RGN_GETTEMPLATEID,	//返回指定副本ＩＤ的副本的蓝图ＩＤ
	MSG_W2S_RGN_GETDUPTYPE,		//返回指定副本ＩＤ的副本的类型
	MSG_W2S_RGN_GETDUPRGNONWER, //返回指定副本ＩＤ的副本的所有者ＩＤ。

	MSG_W2S_RGN_VIEWPLAYERPID, // 查看玩家的个人副本
	MSG_W2S_RGN_VIEWPLAYERPHID, // 查看玩家的个人房屋副本
	MSG_W2S_RGN_VIEWPLAYERTID, // 查看玩家的组队副本
	MSG_W2S_RGN_VIEWGSPLAYERNUM,// 查看指定GS的某场景人数
	MSG_W2S_RGN_QUESTVIEWGSPLAYERNUM, // WS向GS发送查询消息
	MSG_W2S_RGN_ANSWERPLAYERRGNLIST,// 回应GS的副本是否存在
	MSG_W2S_RGN_QUESTGSQUESTPLAYERRGNLIST,// 查询GS玩家副本list
	MSG_W2S_RGN_QUESTGSUPDATEPLAYERRGNLIST,// 查询更新GS玩家副本list

	MSG_W2S_RGN_UPDATE_PLAYER_TEAM_RGNGUID,//! 更新一个玩家的队伍副本数据
	MSG_W2S_RGN_CAMP_PLAYER_NUM_QUEST,	// 查询指定场景的指定阵营玩家数量
	MSG_W2S_RGN_CAMP_PLAYER_NUM_ANSWER,	// 回应指定场景的指定阵营玩家数量


	// AntiCheat 
	MSG_W2S_ANTICHEAT = MSG_W2S + MAX_SUB * 14,
	// -----------------------------------------------------
	MSG_W2S_AC_UPDATEQUESTIONS,


	// Region 
	MSG_W2S_SENTAI					= MSG_W2S + MAX_SUB * 15,
	// -----------------------------------------------------
	MSG_W2S_SENTAI_RE_QUEUE,		//! 回复请求进入或退出排队
	MSG_W2S_SENTAI_UPDATE_POINT,	//! 更新积分
	MSG_W2S_SENTAI_INTO_READY,		//! 进场准备
	MSG_W2S_SENTAI_INTO_FIELD,		//! 通知进场
	MSG_W2S_SENTAI_END_MACTH,		//! 通知结束战斗
	MSG_W2S_SENTAI_RE_PLAYER_STATE, //! 返回玩家状态

	//彩票系统
	MSG_W2S_LOTTERY                 = MSG_W2S + MAX_SUB * 16,
	// -----------------------------------------------------
	MSG_W2S_LOTTERY_SENDWORLDCREDIT,  //! 世界积分刷新
	MSG_W2S_LOTTREY_SENDCreditHighest, //! 最高个人积分刷新
	MSG_W2S_LOTTREY_SENDWorldInfo,     //! 世界积分信息
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
	MSG_C2S_SHAPE_MOVE,										// 移动消息
	MSG_C2S_SHAPE_STOPMOVE,									// 停止移动消息
	MSG_C2S_SHAPE_CHANGEMOVEMODE,							// 移动模式改变
	MSG_C2S_SHAPE_QUESTINFO,								// 主动向服务器请求shape的信息
	MSG_C2S_SHAPE_EMOTION,									// 表情动作
	MSG_C2S_SHAPE_CHANGELINEDRGN,							// 切换分线场景

	MSG_C2S_SHAPE_SETSELECTEDSHAPE,							// 设定当前选择的目标
	MSG_C2S_SHAPE_GETTSELECTEDSHAPE,						// 获得当前选择的目标

	MSG_C2S_PLAYER					= MSG_C2S + MAX_SUB * 3,
	// ----------------------------------------------------
	//MSG_C2S_PLAYER_ADDPOINT,								// 加点
	MSG_C2S_PLAYER_QUEST_RELIVE,							// 请求复活
	MSG_C2S_PLAYER_TOUCHNPC,								// 触发NPC
	MSG_C2S_PLAYER_USEITEM,									// client到gameserver验证一个消耗道具的使用
	MSG_C2S_PLAYER_PKSWITCH,								// 切换PK开关
	MSG_C2S_PLAYER_TRADELOCK,								//进入锁定状态
	MSG_C2S_PLAYER_TRADEREQUEST,							//交易请求
	MSG_C2S_PLAYER_TRADERESPONSE,							//客户端同意或拒绝交易请求
	MSG_C2S_PLAYER_PUTGOODSTOTRADEPAGE,						//客户端放一个物品到交易展示区
	MSG_C2S_PLAYER_FETCHGOODSFROMTRADEPAGE,					//玩家从展示区拿出一个物品
	MSG_C2S_PLAYER_TRADECHANGEMONEY,						//客户端请求调整金钱
	MSG_C2S_PLAYER_WAITFORTRADE,							//客户端通知进入交易等待状态
	MSG_C2S_PLAYER_TRADECANCEL,								//客户端请求取消交易
	MSG_C2S_PLAYER_HELPREQUEST,								// 玩家请求帮助
	MSG_C2S_PLAYER_UpdateShowFashion,						//更新是否显示头盔,披风,外套等属性
	//MSG_C2S_PLAYER_CHANGE_QUEST_TIMEREQUEST,
	MSG_C2S_PLAYER_TRADING,									// 玩家确定后点交易时发送
	MSG_C2S_PLAYER_SKILL_RELIVE,							//
	MSG_C2S_PLAYER_SKILL_CANCEL_RELIVE,						//取消技能复活
	MSG_C2S_PLAYER_CHANGE_TITLE_TYPE,						// 玩家请求更改称号显示类型

	MSG_C2S_PLAYER_SETHOTKEY,								// 玩家设置热键的消息




	MSG_C2S_OTHER					= MSG_C2S + MAX_SUB * 4,
	// ----------------------------------------------------
	MSG_C2S_OTHER_TALK,										// 对话消息
	MSG_C2S_OTHER_SENDTALKBOXSELECT,						// 客户端返回TalkBox的选择
	MSG_C2S_OTHER_VIEWGOODSLINK,							// 客户端请求查看一个物品超链接
	MSG_C2S_OTHER_CONNECTGAMESERVER,						// 连接GameServer的结果
	MSG_C2S_OTHER_CHANGENAME,								// 玩家请求更名操作
	MSG_C2S_OTHER_QUESTNPCLIST,								// 请求NPC列表
	MSG_C2S_OTHER_TALK_WORLD,								// 世界喊话
	MSG_C2S_OTHER_TALK_COUNTRY_UNION,						// 盟国喊话
	MSG_C2S_OTHER_TALK_COUNTRY,								// 国家喊话
	MSG_C2S_OTHER_TALK_TOP_WORLD,							// 置顶世界喊话
	MSG_S2C_OTHER_HITHELPHINT,								// 设置帮助提示
	MSG_S2C_OTHER_HITINTERFACEHINT,							// 设置界面提示
	MSG_C2S_OTHER_FACEACTION,								// 发送表情动画消息
	MSG_C2S_OTHER_GoodsInfo_Request,						// 请求物品信息
	MSG_C2S_OTHER_Message_Busyness,							// 客户端消息繁忙
	MSG_C2S_OTHER_Replay_SETTIMER,                          // 客户端通知服务器点击的定时器
	MSG_C2S_OTHER_COSMETOLOGY,								// 改变形象

	MSG_C2S_OTHER_QUESTGOD = MSG_C2S_OTHER + MAX_SUB - 7,	// 请求天神降临
	MSG_C2S_OTHER_UPDATEVARVALUE_QUEST,						// 请求修改脚本变量值
	MSG_C2S_OTHER_UPDATENAMEANSWER,							//! 客户端修改名字发送消息
    MSG_C2S_OTHER_QUITTOSELECT,                             //  请求退回到选人界面
	MSG_C2S_OTHER_PING,										// 客户端定时主动ping服务器
	MSG_C2S_OTHER_AccusePlayer_Request,						// 举报玩家		
	MSG_C2S_OTHER_Replay_GM,								// 回复GM的密聊
	



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
	MSG_C2S_CLOSEGOODSUPGRADE,
	MSG_C2S_GOODSUPGRADE_PUT,								//放置一个物品到升级界面
	MSG_C2S_GOODSUPGRADE_FETCH,								//从升级界面中取出一个物品
	MSG_C2S_GOODSUPGRADE_CHANGE,							//在升级界面中交换一个物品
	MSG_C2S_GOODSUPGRADE_UPGRADE,                           //client请求升级一个物品

	/// [卡片镶嵌
	MSG_C2S_ENCHASE_INPUT_PWD,								//! 客户端输入拆卡密码
	MSG_C2S_ENCHASE_REQUEST_CHANGE_PWD,						//! 要求修改拆卡密码
	MSG_C2S_ENCHASE_ENCHASECARD,							//镶卡消息
	MSG_C2S_ENCHASE_REMOVECARD,								//拆卡消息
	MSG_C2S_ENCHASE_ENDSESSION,								// 结束会话
	/// 卡片镶嵌]

	MSG_C2S_GOODS_CONTAINER,								// 物品容器返回

	MSG_C2S_GOODS_LOCK,										// 物品锁定协议

	//! GoodsExManage应用
	////////////////////////////////////////////////////////////////////////
	MSG_C2S_GOODS_CLOSE_EX_WND,								//! 物品扩展窗口关闭通知
	//! 分解物品
	MSG_C2S_GOODS_DISASSEMBLE_PREVIEW,						//! 预览分解结果
	MSG_C2S_GOODS_DISASSEMBLE_DO,							//! 请求分解物品
	//! 合成物品
	MSG_C2S_GOODS_SYNTHESIZE_PREVIEW,						//! 预览合成结果
	MSG_C2S_GOODS_SYNTHESIZE_DO,							//! 请求合成物品
	//! 武器充能
	//! 使用统一物品消息：MSG_C2S_PLAYER_USEITEM

	//！绑定冻结
	MSG_C2S_GOODS_BIND,										//! 绑定请求
	MSG_C2S_GOODS_FROST,									//! 冻结请求

	//! 物品升级
	MSG_C2S_GOODS_UPGRADE_VIEW,								//! 预览请求
	MSG_C2S_GOODS_UPGRADE_DO,								//! 执行升级
	//! 物品魔化
	MSG_C2S_GOODS_PLUG_EVIL_DO,								//! 执行魔化
	MSG_C2S_GOODS_RESET_ELEMENT_TYPE,						//! 重置魔化元素类型
	MSG_C2S_GOODS_RESET_XIANG_XING,							//! 重置魔化相性

	//! 仓库
	MSG_C2S_DEPOT_INPUT_PWD,								//! 客户端输入仓库密码
	MSG_C2S_DEPOT_REQUEST_CLOSE,							//! 要求关闭仓库
	MSG_C2S_DEPOT_REQUEST_CHANGE_PWD,						//! 要求修改密码
	MSG_C2S_DEPOT_REQUEST_BUY_SUBPACK,						//! 要求购买子背包

	//! 通用物品操作容器
	MSG_C2S_GOODS_AllPurpose_RequestPush,					//! 请求放入物品
	MSG_C2S_GOODS_AllPurpose_RequestPop,					//! 请求取出物品
	MSG_C2S_GOODS_AllPurpose_RequestClose,					//! 请求关闭容器
	MSG_C2S_GOODS_AllPurpose_RequestFinish,					//! 请求执行容器操作


	MSG_C2S_SHOP					= MSG_C2S + MAX_SUB * 6,
	// ----------------------------------------------------
	MSG_C2S_SHOP_BUY,										//player请求买东西
	MSG_C2S_SHOP_SELL,										//player卖一个物品回商店
	MSG_C2S_SHOP_REPAIR,									//在商店中修理一个物品
	MSG_C2S_SHOP_REPAIRALL, 								//商店中修理全部物品
	MSG_C2S_SHOP_REQUESTREPAIRALLPRICE,					//查询修理全部物品的价格
	MSG_C2S_SHOP_REQUESTREPAIRSINGLEPRICE,				//查询修理单个物品的价格
	MSG_C2S_SHOP_DURUPDATE,								//耐久更新
	MSG_C2S_SHOP_CLOSE,									//关闭个人商店

	MSG_C2S_NPCSHOP_OPEN,									//打开NPC商店（临时使用）
	MSG_C2S_BUSINESS_BUY,
	MSG_C2S_BUSINESS_SELL,
	MSG_C2S_BUSINESS_INVEST,	//商业贸易点投资
	MSG_C2S_SHOP_BUYBACK,									// 回购物品

	MSG_C2S_ESSENCE_SHOP_OPEN,								// 精华商店
	MSG_C2S_FBUSINESS_OPER,		// 家族跑商操作

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
	MSG_C2S_TEAM_QUERY_JOIN_BYNAME,								//通过名字邀请加入队伍
	MSG_C2S_TEAM_ANSWER_JOIN,									//回复邀请加入
	MSG_C2S_TEAM_CHANGE_LEADER,									//改变队长
	MSG_C2S_TEAM_KICK_PLAYER,									//踢人(队长可以踢所有人，队员可以踢自己)
	MSG_C2S_TEAM_SetGoods,										//修改物品分配设置
	MSG_C2S_TEAM_CHAT,											//队聊
	MSG_C2S_TEAM_RECRUIT,										//招募控制
	MSG_C2S_TEAM_BeRecruited,									//应征
	MSG_C2S_TEAM_Recommend_ByName,								//! 推荐玩家


	MSG_C2S_SKILL					= MSG_C2S + MAX_SUB * 9,
	// ----------------------------------------------------
	MSG_C2S_SKILL_USE,										// 使用技能
	MSG_C2S_SKILL_USE_END,									// 使用技能结束，有些技能松开快捷键的时候发送
	MSG_C2S_SKILL_STUDY,									// 学习技能
	MSG_C2S_SKILL_STUDY_BEGIN,								// 向服务器发送打开技能学习导师页面
	MSG_C2S_DOCCUSKILL_USE,									// 使用副职业技能
	MSG_C2S_OCCUSKILL_STUDY_BEGIN,							// 向服务器发送打开副职业技能学习页面
	MSG_C2S_OCCUSKILL_STUDY,								// 向服务器发送学习副职业技能消息
	MSG_C2S_OCCUSKILL_DELETE,								// 向服务器发送删除制作条目消息

	MSG_C2S_BUFF_END,										// 向服务器发送删除状态消息
	MSG_C2S_OCCUSKILL_NPCFACTURE_BEGIN,						// 收到客户NPC制作列表消息
	MSG_C2S_OCCUSKILL_NPCFACTURE,							// 收到客户端制作消息

	MSG_C2S_SKILL_ADD_PET,
	MSG_C2S_SKILL_DEL_PET,
	MSG_C2S_SKILL_USE_PET,
	MSG_C2S_SKILL_END_PET,
	MSG_C2S_SKILL_SAVE,

	MSG_C2S_ORGSYS					= MSG_C2S + MAX_SUB * 10,
	// ----------------------------------------------------

	// 行会协议-------------------------------------------------
	//! 会长操作
	MSG_C2S_ORGSYS_CreateFaction_Reques,					//! 创建行会请求
	MSG_C2S_ORGSYS_DisbandFaction_Request,					//! 解散行会请求	
	MSG_C2S_ORGSYS_SetPlacard_Request,						//! 修改公告请求
	MSG_C2S_ORGSYS_SetLeaveword_Request,					//! 修改备忘请求
	MSG_C2S_ORGSYS_SetJobName_Request,						//! 修改官阶名字请求
	MSG_C2S_ORGSYS_SetJobPurview_Request,					//! 修改官阶权限请求
	MSG_C2S_ORGSYS_SetMemberJob_Request,					//! 设置成员官阶请求
	MSG_C2S_Faction_SetMemberTitle_Request,					//! 设置成员称号请求
	MSG_C2S_Faction_SetChairman_Request,					//! 传位请求

	//! 成员操作
	MSG_C2S_ORGSYS_Quit_Request,							//! 退出行会请求
	MSG_C2S_ORGSYS_Kick_Request,							//! 踢出行会请求
	MSG_C2S_Faction_InvitePlayer_Request,					//! 邀请玩家加入请求
	MSG_C2S_Faction_InvitePlayer_Answer,					//! 回答邀请
	MSG_C2S_ORGSYS_CheckRequisition_Request,				//! 审批入会请求
	MSG_C2S_Faction_UpLoadIcon_Request,						//! 上传工会图标

	MSG_C2S_ORGSYS_JoinFac_Request,							//! 玩家入会（加入到申请列表）请求

	//! 数据请求
	MSG_C2S_FacData_Request,								//! 行会资料请求
	MSG_C2S_Faction_IconData_Request,						//! 请求单个工会图标数据
	MSG_C2S_Faction_AllIconData_Request,					//! 工会图标数据请求（暂不使用了）

	MSG_C2S_ORGSYS_CloseFacBoard_Notifye,					//! 玩家关闭工会界面通知（新加）

	MSG_C2S_FACTION_Upgrade_Request,						//! 工会升级请求
	MSG_C2S_Faction_ChangeJoinBattle,						//	修改成员参与权
	// End：行会------------------------------------------------


	// 联盟协议-------------------------------------------------
	MSG_C2S_UNION_Request_Creation,					//! 创建联盟请求
	MSG_C2S_UNION_Request_Disband,					//! 解散联盟请求
	MSG_C2S_UNION_Request_Quit,						//! 行会退出联盟请求
	MSG_C2S_UNION_Request_Kick,						//! 盟主将行会踢出联盟请求
	MSG_C2S_UNION_Request_SetChairman,				//! 盟主转让请求
	MSG_C2S_UNION_Request_SetRights,				//! 盟主赋予会长权限请求
	MSG_C2S_UNION_Request_Invite,					//! 盟主 邀请行会会长 加入联盟的发送请求
	MSG_C2S_UNION_Request_Anwser_Invite,			//! 行会会长 发送回复盟主的邀请
	// End：联盟------------------------------------------------

	MSG_C2S_QUEST_Accept,									//提交完成任务
	MSG_C2S_QUEST_Complete,									//提交完成任务
	MSG_C2S_QUEST_Disband,									//提交放弃任务

	MSG_C2S_MERCQUEST_Update,								//刷新俑兵任务

	MSG_C2S_QUEST_PlayerRanks,								//请求玩家的排行榜
	
	//赏金任务消息
	MSG_C2S_SHARE_QUESTLIST_UPDATE,							//收到推荐赏金任务消息						

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
	MSG_C2S_PLAYERSHOP_REMOVE,								//从个人商店上移除物品时的消息
	MSG_C2S_PLAYERSHOP_BUYRELIVEITEM,						//购买增值道具复活

  	MSG_C2S_PLAYERSHOP_MSGBOARDSET,							//设置留言板信息(包括设置商店名称、店主推荐)
	MSG_C2S_PLAYERSHOP_MSGBOARDCHAT,			        	//客户端发送的留言板中的聊天信息

	//MSG_C2S_PLAYERSHOP_FETCHGOODS,
	//MSG_C2S_PLAYERSHOP_PUTGOODS,
	//MSG_C2S_PLAYERSHOP_EXCHANGEITEM,

	// 容器
	MSG_C2S_CONTAINER				= MSG_C2S + MAX_SUB * 12,
	// ----------------------------------------------------
	MSG_C2S_CONTAINER_OBJECT_MOVE,
	MSG_C2S_CONTAINER_OBJECT_UNLOCK,
	MSG_C2S_CONTAINER_TIDY,                                //整理背包消息    

	// 宠物
	MSG_C2S_PET						= MSG_C2S + MAX_SUB * 13,
	// ----------------------------------------------------
	MSG_C2S_PET_MOVE,										// 移动
	MSG_C2S_PET_STOP,										// 停止
	MSG_C2S_PET_CHANGE_MODE,								// 改变宠物当前模式
	MSG_C2S_PET_CHANGE_ACTION,								// 改变宠物当前动作
	MSG_C2S_PET_CHANGE_SPEED,								// 改变速度
	MSG_C2S_PET_HOUSE,										// 宅藏宠物
	MSG_C2S_PET_CANCEL,										// 放弃宠物

	// 点卡寄售 c->s
	MSG_C2S_CARD					= MSG_C2S + MAX_SUB * 14,
	// -----------------------------------------------------

	// 国家
	MSG_C2S_COUNTRY					= MSG_C2S + MAX_SUB * 15,
	// -----------------------------------------------------
	//MSG_C2S_COUNTRY_INFO_GET,								// 获取所有国家信息

	// 增值商店
	MSG_C2S_INCREMENTSHOP					= MSG_C2S + MAX_SUB * 16,
	// ----------------------------------------------------
	MSG_C2S_INCREMENTSHOP_OPEN,
	MSG_C2S_INCREMENTSHOP_BUY,										//player请求买东西
	MSG_C2S_INCREMENTSHOP_CLOSE,									//关闭个人商店

	MSG_C2S_INCREMENTSHOP_AFFICHE_REQUEST,							// 公告窗口开
	MSG_C2S_INCREMENTSHOP_GETLOG_REQUEST,					// 请求增值日志

	MSG_C2S_INCREMENTSHOP_GET_INFO,							// 请求单个商品信息，并打开购买状态

	//!
	MSG_C2S_STATE					= MSG_C2S + MAX_SUB * 17,
	MSG_C2S_STATE_END,										//状态结束

	// 好友系统
	MSG_C2S_LINKMAN					= MSG_C2S + MAX_SUB * 18,
	// A：源操作客户端-------------------------------------
	// B：被操作客户端-------------------------------------
	MSG_C2S_LINKMAN_ADD,										// A 向 S 申请添加联系人 B
	MSG_C2S_LINKMAN_DEL,										// A 向 S 请求删除联系人 B
	MSG_C2S_LINKMAN_MOVE,										// A 向 S 请求移动联系人 B

	MSG_C2S_LINKMAN_RESPONSE_ADD,								// B 向 S 回应 A 的添加请求
	// 好友系统 end----------------------------------------

	// 小精灵
	MSG_C2S_SPRITE					= MSG_C2S + MAX_SUB * 19,
	// ----------------------------------------------------
	MSG_C2S_SPRITE_AutoSupply_OnOff_Request,					//! 自动补给系统：功能开关请求
	MSG_C2S_SPRITE_AutoSupply_ChangeSetup_Request,				//! 自动补给系统：修改一个补给设置请求
	MSG_C2S_SPRITE_AutoFight_ChangeSetup_Request,               //! 自动补给系统：修改一个打怪挂机请求 
	MSG_C2S_SPRITE_AutoRepair,                                  //! 自动补给系统：选择自动修理装备选勾




	MSG_C2S_SaveNewSetupData_Request,							//! 玩家保存新的游戏设置
	MSG_C2S_Calendar_Request,                                   //! 请求打开日程表
	MSG_C2S_JanaweBox_Request,                                  //! 请求打开迦纳维魔盒
	MSG_C2S_Answer_Request,                                     //! 问答与奖励
	MSG_C2S_Help_Request,										//! 帮助


	// ----------------------------------------------------
	//邮件系统
	MSG_C2S_MAIL					= MSG_C2S + MAX_SUB * 20,
	MSG_C2S_MAIL_SEND,											//! 发信
	MSG_C2S_MAIL_RECEIVER,										//! 收信
	MSG_C2S_MAIL_READ,											//！读邮件
	MSG_C2S_MAIL_GET_GOODS,										//! 从邮件中取物品到背包
	MSG_C2S_MAIL_GET_GOLDS,										//! 从邮件中取金币
	MSG_C2S_MAIL_DELETE,										//! 删除邮件
	MSG_C2S_MAIL_REJECT,										//! 拒收邮件
	MSG_C2S_MAIL_AGREED_PAY,									//! 同意付费
	MSG_C2S_MAIL_AGREED_PRESENT,								//! 同意赠送

	// Fox@20081107----------------------------------------------
	// 玩家管理系统( PlayerManager )
	MSG_C2S_PM						= MSG_C2S + MAX_SUB * 21,
	// -----------------------------------------------------
	MSG_C2S_PM_REQ,							// CLIENT请求查询玩家信息

	/// Expter@20090527
	///======================================================================================================
	MSG_C2S_ANTICHEAT				= MSG_C2S + MAX_SUB * 22,
	MSG_C2S_AC_ANSWER,

	// 赠送相关
	MSG_C2S_BOUNS					= MSG_C2S + MAX_SUB * 23,
	MSG_C2S_BOUNS_INFO,					// 获取赠送信息
	MSG_C2S_BOUNS_GET,					// 取赠送品
	MSG_C2S_BOUNS_CLOSE,				// 关闭领奖界面


	//! 战队系统
	MSG_C2S_SENTAI					= MSG_C2S + MAX_SUB * 24,
	// -----------------------------------------------------
	MSG_C2S_SENTAI_REQUEST_QUEUE,		//! 请求进入或退出排队


	// ----------------------------------------------------
	//彩票系统
	MSG_C2S_LOTTERY					= MSG_C2S + MAX_SUB * 25,
	MSG_C2S_LOTTERY_Startup,					//! 开始旋转
	MSG_C2S_LOTTERY_ResetPrizes,				//! 重置奖品
	MSG_C2S_LOTTERY_ResetConstellations,		//! 重置星座
	MSG_C2S_LOTTERY_Endup,		                //! 旋转停止，发送奖品
	MSG_C2S_LOTTERY_DoSystemReset,				//! 进行强制重置
	MSG_C2S_LOTTERY_OpenPage,				    //! 打开抽奖界面


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
	MSG_L2C_LOG_WAITDATALOAD,								// 通知客户端正在读取角色基本信息
	MSG_L2C_LOG_FROZEN_CDKEY,								// 密码重试过多账号冻结
	MSG_L2C_LOG_FCM_TIME,									// 通知玩家防沉迷剩余时间（秒）
	MSG_L2C_LOG_SECURITY_VALIDATE,							// 通知客户端进行安全信息输入
	MSG_L2C_LOG_SECURITY_VALIDATINT,						// 通知客户端，已有其他玩家正在输入密保卡信息，请稍等再试
	MSG_L2C_LOG_Business_URL,                               // 通知客户端，补全玩家注册资料

	//========================================================================================================
	MSG_S2C = MAX_TYPE * 12,		// GameServer --> Client
	//========================================================================================================

	MSG_S2C_LOG						= MSG_S2C + MAX_SUB * 0,
	// ----------------------------------------------------
	MSG_S2C_LOG_ANSWER_PLAYERDATA,							// GameServer向GameClient返回player详细数据
	MSG_S2C_LOG_GAMESERVERTIME,								// 发送Server的时间
	MSG_S2C_LOG_SEQUENCESTRING,								// 验证消息合法性的字符串序列

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
	MSG_S2C_RGN_CHANGENAME,									// 改变名字
	MSG_S2C_RGN_UPDATEEFFECT,								// 刷新特效
	//!MSG_S2C_RGN_CLEARDUPRGNPLAYER,							// 副本清人消息（次消息改为MSG_S2C_OTHER_TIME_INFO_BOX）
	MSG_S2C_RGN_PLAY3DSOUND,                                // 播放3D声音
	MSG_S2C_RGN_PLAYMUSIC,                                  // 播放音乐
	MSG_S2C_RGN_STOPMUSIC,                                  // 停止播放音乐
	MSG_S2C_RGN_STRONGPOINT_STATE_CHANGE,					// 服务器通告场景所有玩家某个据点被改变

	//[071127 AHC]
	MSG_S2C_RGN_ANSWER_CREATE,
	MSG_S2C_RGN_ANSWER_DELETE,

	MSG_S2C_SHAPE					= MSG_S2C + MAX_SUB * 2,
	// ----------------------------------------------------
	MSG_S2C_SHAPE_CHANGEDIR,								// 改变方向
	MSG_S2C_SHAPE_CHECKPOS,									// 坐标效验
	MSG_S2C_SHAPE_SETPOS,									// 瞬移
	MSG_S2C_SHAPE_FORCEMOVE,								// 强制移动
	MSG_S2C_SHAPE_MOVE,										// 回复移动消息
	MSG_S2C_SHAPE_STOPMOVE,									// 停止移动消息
	MSG_S2C_SHAPE_CHANGEMOVEMODE,							// 移动模式改变
	MSG_S2C_SHAPE_MOVETILE,									// 服务器发来消息移动一格
	MSG_S2C_SHAPE_CHANGESTATE,								// 改变状态
	MSG_S2C_SHAPE_ATK_LOCK,									// 指示锁定目标
	MSG_S2C_SHAPE_ATK_BEGIN,								// 服务器通知客户端开始攻击
	MSG_S2C_SHAPE_ATK_DIED,									// 死亡
	MSG_S2C_SHAPE_ATK_REJECT,								// 攻击指令被拒绝
	MSG_S2C_SHAPE_FIGHTSTATE,								// 通知client进入或退出战斗状态
	MSG_S2C_SHAPE_SINSTATE,									// 通知client进入或退出犯罪状态
	MSG_S2C_SHAPE_Refresh,									// 刷新建筑状态 
	MSG_S2C_SHAPE_STIFFEN,									// 僵直状态
	MSG_S2C_SHAPE_EMOTION,									// 僵直状态

	MSG_S2C_SHAPE_ATK_DAMAGE,								//普通受伤消息
	MSG_S2C_SHAPE_ATK_BREAK,								// 被攻击
	MSG_S2C_SHAPE_ATK_FULLMISS,								// 闪避
	MSG_S2C_SHAPE_ATK_PARRY,								// 招架
	MSG_S2C_SHAPE_ATK_BLOCK,								// 格挡
	MSG_S2C_SHAPE_ATK_RESIST,								// 抵抗
	MSG_S2C_SHAPE_ATK_REBOUND, 							    // 反弹
	MSG_S2C_SHAPE_ATK_SUCK,									// 吸收
	MSG_S2C_SHAPE_RELIVE,									// 复活一个玩家
	MSG_S2C_SHAPE_CHANGE_PROPERTIES,						// 更新可移动物体的属性
	MSG_S2C_SHAPE_ATK_LOSE,									// 丢失
	MSG_S2C_SHAPE_CHANGE_FIRSTATTACKER,						// 改变第一个攻击自己的人
	MSG_S2C_SHAPE_LEAVEFIGHT,								// 脱离战斗
	MSG_S2C_SHAPE_SLIPPOS,									// 滑动位移
	MSG_S2C_SHAPE_LEAVEFIGHTEND,							// 脱离战斗结束
	//MSG_S2C_SHAPE_ATK_EFFECT,								// 特效播放
	MSG_S2C_SHAPE_ATK_ADDBLOOD,								// 加血
	MSG_S2C_SHAPE_GETTSELECTEDSHAPE,						// 获得当前选择的目标


	MSG_S2C_PLAYER					= MSG_S2C + MAX_SUB * 3,
	// ----------------------------------------------------
	MSG_S2C_PLAYER_RESUME,									// 恢复HP、MP、YP。。。
	MSG_S2C_PLAYER_ADDPOINT,								// 加点
	MSG_S2C_PLAYER_ANSWER_RELIVE,							// 恢复复活
	MSG_S2C_PLAYER_ADDEXP,									// 增加经验值
	MSG_S2C_PLAYER_LEVELUP,									// 升级
	MSG_S2C_PLAYER_ADDOCCUEXP,								// 增加职业经验值
	MSG_S2C_PLAYER_OCCULEVELUP,								// 职业等级升级
	MSG_S2C_PLAYER_ADDSP,									// 增加经验值
	MSG_S2C_PLAYER_CREDIT_UPDATE,							// 声望更新
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
	MSG_S2C_PLAYER_TRADELOCK,
	MSG_S2C_PLAYER_CHANGE_TO_LOCKSTATE,						//退回交易锁定状态
	MSG_S2C_PLAYER_PUTGOODSTOTRADEPAGE,						//服务器返回放一个物品到展示区的结果
	MSG_S2C_PLAYER_GETGOODSFROMTRADEPAGE,					//服务器推一个物品信息给对方玩家
	MSG_S2C_PLAYER_FETCHGOODSFORMTRADEPAGE,					//服务器返回从展示区拿一个物品的相关结果
	MSG_S2C_PLAYER_DELGOODSFROMTRADEPAGE,					//服务器通知对方一个物品从交易区中删除
	MSG_S2C_PLAYER_TRADECHANGEMONEY,						//服务器返回调整金钱的相关结果
	MSG_S2C_PLAYER_WAITFORTRADE,	
	MSG_S2C_PLAYER_TRADING,									// 玩家点交易时发送此消息
	MSG_S2C_PLAYER_TRADECANCEL,								//服务器通知玩家对方已经取消交易
	MSG_S2C_PLAYER_TRADERESULT,								//交易的最终结果
	MSG_S2C_PLAYER_HELPRESPONSE,							// 相应玩家帮助
	MSG_S2C_PLAYER_ADDSKILL,								// 添加技能
	MSG_S2C_PLAYER_DELSKILL,								// 删除技能
	MSG_S2C_PLAYER_SETATTACKRANGE,				
	MSG_S2C_PLAYER_CHANGE_EQUIPMENT,
	MSG_S2C_PLAYER_CHANGE_PROPERTIES,						//玩家改变属性
	MSG_S2C_PLAYER_UpdateShowFashion,					//更新玩家是否显示头盔信息
	MSG_S2C_PLAYER_AUTOMOVE,								//自动移动到指定的点
	MSG_S2C_PLAYER_CHANGE_CONTRIBUTE,						// 国家贡献改变
	MSG_S2C_PLAYER_FORCE_TARGET,
	MSG_S2C_PLAYER_COLLECTION_LEVELUP,								// 玩家采集升级
	MSG_S2C_PLAYER_FACTURE_LEVELUP,									// 玩家制作升级
	MSG_S2C_PLAYER_RELIVE_RESULT,									// 复活失败
	MSG_S2C_PLAYER_BUYRELIVEITEM_RESULT,							// 返回检查原地活道具结果
	MSG_S2C_PLAYER_SKILL_RELIVE,									// 技能复活
	MSG_S2C_PLAYER_UPDATE_TITLE_TYPE,								// 玩家更新称号
	MSG_S2C_PLAYER_COLLECTION_ADDEXP,								// 玩家采集经验增加
	MSG_S2C_PLAYER_FACTURE_ADDEXP,									// 玩家制作经验增加
	MSG_S2C_PLAYER_ACTIVE_CHANGE,									// 玩家活力点改变
	MSG_S2C_PLAYER_SHOW_SPOUSENAME,									// 通知客户端是否显示配偶名称
	MSG_S2C_PLAYER_ELEMENT_PROPERTY_CHANGE,							// 玩家元素属性改变
	MSG_S2C_PLAYER_COSMETOLOGY_FAILED,								// 改变形象失败
	MSG_S2C_PLAYER_COSMETOLOGY_SUCESS,								// 改变形象成功

	MSG_S2C_PLAYER_SETHOTKEY_RESULT,								// 玩家设置热键成功与否的消息

	//MSG_S2C_MONSTER					= MSG_S2C + MAX_SUB * 19,

	MSG_S2C_OTHER					= MSG_S2C + MAX_SUB * 4,
	// ----------------------------------------------------
	MSG_S2C_OTHER_TALK,										// 对话消息
	MSG_S2C_OTHER_FACTIONTALK,								// 行会聊天
	MSG_S2C_OTHER_BROADCAST,								// 广播
	MSG_S2C_OTHER_AddTopInfo,								// 添加顶端提示信息
	MSG_S2C_OTHER_OPENTALKBOX,								// 让客户端弹出TalkBox
	MSG_S2C_OTHER_OPEN_PIC_TALKBOX,							//! 让客户端弹出可自定义图片的TalkBox
	MSG_S2C_OTHER_OPENPIXYTALKBOX,							// 让客户端弹出PixyTalkBox
	MSG_S2C_OTHER_ADDINFO,									// 在玩家对话框加入一条信息
	MSG_S2C_OTHER_ADDINFO_BYINDEX,							// 在玩家对话框加入一条信息(按照索引添加)
	MSG_S2C_OTHER_ADDSYSTEMINFO,							// 在屏幕上方加入一条信息
	MSG_S2C_OTHER_GAMEMESSAGE,								// 弹出消息对话框
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
	MSG_S2C_OTHER_TALK_COUNTRY_UNION,						// 盟国喊话
	MSG_S2C_OTHER_TALK_NOTIFY,								// 国家通知
	MSG_S2C_OTHER_TALK_TOP_WORLD,							// 置顶世界喊话	
	MSG_S2C_OTHER_TRANSFORCAMERA,							// 转换摄像机
	MSG_S2C_OTHER_OPERAINTERFACE,							// 操作界面
	MSG_S2C_OTHER_SETHELPHINT,								// 设置帮助提示
	MSG_S2C_OTHER_SETINTERFACEHINT,							// 设置界面提示
	MSG_S2C_OTHER_OPENSTORYTALKBOX,							// 让客户端弹出StoryTalkBox
	MSG_S2C_OTHER_EFFECT,									// 让客户端播放一个特效动画
	MSG_S2C_OTHER_EXPATC,									// 表情运作	
	MSG_S2C_OTHER_GoodsInfo_Re,								// 物品信息回复
	MSG_S2C_OTHER_Message_Busyness,							// 返回客户端繁忙消息
	MSG_S2C_OTHER_PLAY_VIDEO,								// 播放视频
	MSG_S2C_OTHER_STOP_VIDEO,								// 关闭视频
	MSG_S2C_OTHER_PackGlint,								// 背包闪烁
	MSG_S2C_OTHER_REPEATLOGIN,								// 账号重复登录
	MSG_S2C_OTHER_UPDATEVARVALUEOPEN,						// 打开修改脚本变量值的窗口
	MSG_S2C_OTHER_SHOW_STRING,								//! 在客户端右侧显示一个字符串
	MSG_S2C_OTHER_NEWS_TOP_WORLD,							// 紧急公告
	MSG_S2C_OTHER_ADDINFO_AND_GOODSINFO,					//! 一条带物品信息的消息
	MSG_S2C_OTHER_UPDATENAMEOPEN,							//! 客户端修改名字对话框弹出
	MSG_S2C_OTHER_QUITTOSELECTANSEWR,                       // 请求返回选人界面的回复
	MSG_S2C_OTHER_UPDATE_HIDE_FLAG,							// 玩家隐匿状态该变，通告周围玩家
	MSG_S2C_OTHER_BILLURL,									//通知计费URL
	MSG_S2C_OTHER_TIME_INFO_BOX,							//! 通知客户端显示一个倒计时的消息矿
	MSG_S2C_OTHER_WARNING,									// 玩家被其它阵营玩家攻击通告
	MSG_S2C_OTHER_OPEN_RELIVEWINDOW,						//打开复活界面
	MSG_S2C_OTHER_AccusePlayer_Notify,						//举报结果
	MSG_S2C_OTHER_GM_CHAT,									//GM与玩家对话//GM与玩家对话
	MSG_S2C_OTHER_SETTIMER,									// 通知客户端显示定时器
	MSG_S2C_OTHER_CLOSETIMER,								// 通知客户端关闭定时器
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
	MSG_S2C_EQUIP_REFINE_UPDATE,							//炼度更新
	MSG_S2C_DELETEEQUIP,									//删除装备
	MSG_S2C_DELETEBAGGOODS,									//删除背包物品
	MSG_S2C_DELETEGOODS,									
	MSG_S2C_DELETEALLGOODS,						
	MSG_S2C_VIEWEQUIP,                                      
	MSG_S2C_OPENGOODSUPGRADE,								//gameserver通知玩家打开物品升级界面

	/// [卡片镶嵌
	MSG_S2C_ENCHASE_HAS_PWD,								//! 通知客户端输入卡片操作密码
	MSG_S2C_ENCHASE_RE_INPUT_PWD,							//! 回复客户端密码输入
	MSG_S2C_ENCHASE_RE_CHANGE_PWD,							//! 回复修改密码

	MSG_S2C_OPENGOODSENCHASE,								// 打开镶嵌功能的某个子功能界面
	MSG_S2C_ENCHASE_OPEN_HOLE,							//开孔消息
	MSG_S2C_ENCHASE_CLOSE_HOLE,							//堵孔消息
	MSG_S2C_ENCHASE_ENCHASECARD,							//镶卡消息
	MSG_S2C_ENCHASE_REMOVECARD,							//拆卡消息
	MSG_S2C_ENCHASE_ARRANGE,								//打开小功能页面消息
	/// 卡片镶嵌]

	MSG_S2C_CLOSEGOODSUPGRADE,
	MSG_S2C_GOODSUPGRADE_PUT,					
	MSG_S2C_GOODSUPGRADE_FETCH,					
	MSG_S2C_GOODSUPGRADE_CHANGE,				
	MSG_S2C_GOODSUPGRADE_UPGRADE,							//gameserver返回升级一个物品的结果
	MSG_S2C_GOODS_UPDATEPROPERTY,							//更新物品属性
	MSG_S2C_GOODS_OPEN_CONTAINER,							// 打开物品容器
	MSG_S2C_SUIT_ACTIVE,									//套装激活

	MSG_S2C_SET_YUANBAO,  //设置元宝
	MSG_S2C_SET_TICKET,	  //设置点券

	//! GoodsExManage应用
	////////////////////////////////////////////////////////////////////////
	MSG_S2C_GOODS_OPEN_WINDOWS,								//！打开物品操作窗口（指定窗口的类型）
	MSG_S2C_GOODS_CLOSE_WINDOWS,							//！关闭所有物品操作窗口
	//! 分解物品
	MSG_S2C_GOODS_RE_DISASSEMBLE_PREVIEW,					//! 返回预览结果
	MSG_S2C_GOODS_RE_DISASSEMBLE_DO,						//! 返回分解物品成败
	//! 合成物品
	MSG_S2C_GOODS_RE_SYNTHESIZE_PREVIEW,					//! 返回预览结果
	MSG_S2C_GOODS_RE_SYNTHESIZE_DO,							//! 返回合成成败
	//! 武器充能
	MSG_S2C_GOODS_RE_REINFORCE_POWER,						//! 补充武器能量
	MSG_S2C_GOODS_POWER_WARN,								//! 武器能量剩余通知
	//! 绑定冻结
	MSG_S2C_GOODS_BIND,										//! 物品绑定（解绑）通知
	MSG_S2C_GOODS_FROST,									//! 物品冻结（解冻）通知

	//! 物品升级
	MSG_S2C_GOODS_RE_UPGRADE_VIEW,							//! 回复预览请求
	MSG_S2C_GOODS_RE_UPGRADE_DO,							//! 回复执行升级
	//! 物品魔化
	MSG_S2C_GOODS_RE_PLUG_EVIL_DO,							//! 回复执行升级
	MSG_C2S_GOODS_RE_RESET_ELEMENT_TYPE,					//! 回复执行重置魔化元素类型
	MSG_C2S_GOODS_RE_RESET_XIANG_XING,						//! 回复执行重置魔化相性

	//! 仓库
	MSG_S2C_DEPOT_OPEN,										//!	通知客户端打开仓库
	MSG_S2C_DEPOT_RE_INPUT_PWD,								//! 回复客户端密码输入
	MSG_S2C_DEPOT_RE_CLOSE,									//! 回复关闭仓库，也作为关闭通知
	MSG_S2C_DEPOT_RE_CHANGE_PWD,							//! 回复修改密码
	MSG_S2C_DEPOT_RE_BUY_SUBPACK,							//! 购买子背包
	MSG_S2C_DEPOT_FROSTED,									//! 通知仓库被冻结

	//! 勋章
	MSG_S2C_MEDAL_ADD_NOTIFY,								//! 添加勋章通知
	MSG_S2C_MEDAL_DEL_NOTIFY,								//! 删除勋章通知

	//! 非副职业制作
	MSG_S2C_MAKE_BEGIN,										//! 开始一个制作
	MSG_S2C_MAKE_RESULT,									//! 发送一个制作结果

	//! 通用物品操作容器
	MSG_S2C_GOODS_AllPurpose_Open,							//! 打开通用容器
	MSG_S2C_GOODS_AllPurpose_Close,							//! 关闭容器
	MSG_S2C_GOODS_AllPurpose_UpdateTxt,						//! 更新说明文本
	MSG_S2C_GOODS_AllPurpose_RePush,						//! 回复放入物品
	MSG_S2C_GOODS_AllPurpose_RePop,							//! 回复取出物品
	MSG_S2C_GOODS_UPDATE_SCRIPTDESC,						//! 物品额外描述改变
	MSG_S2C_GOODS_AllPurpose_UpdateGold,					//! 更新容器显示钱数
	MSG_S2C_GOODS_Restore_Model,                            //! 更新换装模型	

	MSG_S2C_SHOP					= MSG_S2C + MAX_SUB * 6,
	// ----------------------------------------------------
	MSG_S2C_SHOP_OPEN,										//打开商店			
	MSG_S2C_SHOP_BUY,
	MSG_S2C_SHOP_BUY_BAT,
	MSG_S2C_SHOP_SELL,	
	MSG_S2C_SHOP_SELL_BAT,
	MSG_S2C_SHOP_GOODSCHANGEGOODS,
	MSG_S2C_SHOP_REPAIR,						
	MSG_S2C_SHOP_REPAIRALL,	
	MSG_S2C_SHOP_REPAIRALLCOST,					// 服务器返回修理背包中全部物品的花费
	MSG_S2C_SHOP_REPAIRSINGLECOST,				// 服务器返回修理背包中单个物品的花费
	MSG_S2C_SHOP_DURUPDATE,						
	MSG_S2C_SHOP_CLOSE,		//##关闭个人商店	

	MSG_S2C_BUSINESS_OPEN,
	MSG_S2C_BUSINESS_BUY,
	MSG_S2C_BUSINESS_SELL,
	MSG_S2C_INVEST_OPEN,
	MSG_S2C_BUSINESS_INVEST,	//商业贸易点投资
	MSG_S2C_SHOP_BUYBACK,

	MSG_S2C_BUSINESS_UPDATEPRICE, // 商业界面价格更新
	MSG_S2C_FBUSINESS_OPER,		// 家族跑商操作

	MSG_S2C_DEPOT					= MSG_S2C + MAX_SUB * 7,
	// ----------------------------------------------------

	MSG_S2C_GM						= MSG_S2C + MAX_SUB * 8,
	// ----------------------------------------------------
	MSG_S2C_GM_ANSWER_GMMODE,								// 响应客户端请求GM模式的消息
	MSG_S2C_GM_VIEW_HIDENAME_AROUND,						// 响应GM查看周围隐匿者名字

	MSG_S2C_TEAM					= MSG_S2C + MAX_SUB * 9,
	// ----------------------------------------------------
	MSG_S2C_TEAM_SKILL_ABOUT,								//! 技能相关信息（初始化发送）
	MSG_S2C_TEAM_TeamCreate,								//! 数据格式同MSG_S2C_TEAM_TeamData，但只在建立时发送
	MSG_S2C_TEAM_TeamData,									//! 所有队伍数据到成员
	MSG_S2C_TEAM_MemberData,								//! 一个成员的队伍数据
	MSG_S2C_TEAM_TeamDataChange,							//! 队伍数据改变

	MSG_S2C_TEAM_MemberAllIdioinfo,							//! 一个成员所有非队伍成员数据
	MSG_S2C_TEAM_UpdateIdioinfo,							//! 更新某个非队伍成员数据
	MSG_S2C_TEAM_MembereExStateChange,						//! 成员异常状态改变
	MSG_S2C_TEAM_MemberStateChange,							//! 成员在线状态改变

	MSG_S2C_TEAM_MemberLeave,								//! 成员离队
	MSG_S2C_TEAM_MemberLeaveHere,							//! 成员离开本服(GS)
	MSG_S2C_TEAM_MastterChange,								//! 队长改变

	MSG_S2C_TEAM_Disband,									//! 队伍解散
	MSG_S2C_TEAM_GoodsSetChange,							//! 修改物品分配设置

	MSG_S2C_TEAM_MemberPos,									//! 更新成员位置
	MSG_S2C_TEAM_RECRUIT,									//! 更新招募状态
	MSG_S2C_TEAM_RECRUITED_NUM,								//! 更新已招募的人数

	MSG_S2C_TEAM_QUERY_JOIN,								//邀请加入
	MSG_S2C_TEAM_CHAT,										//队聊

	MSG_S2C_TEAM_Recommend_ByName,							//! 推荐玩家通知
	MSG_S2C_TEAM_ChangeOptControl,							//! 修改操作控制




	MSG_S2C_SKILL					= MSG_S2C + MAX_SUB * 10,
	// ----------------------------------------------------
	MSG_S2C_SKILL_USE,										// 使用技能
	MSG_S2C_SKILL_STATE_CHANGE,								// 使用技能后导致HP/MP/RP/YP状态改变
	MSG_S2C_SKILL_ADD_EX_STATE,								// 使用技能后添加异常状态
	MSG_S2C_SKILL_END_EX_STATE,								// 异常状态的消失,服务器通知客户端
	MSG_S2C_SKILL_UPDATE_EX_STATE,							// 更新状态时间
	MSG_S2C_SKILL_EX_STATE_ADDITIONAL_DATA,					// 异常状态的附加数据
	MSG_S2C_SKILL_AFFECT_FIELD,								// 发送受影响的区域给客户端
	MSG_S2C_SKILL_STUDY_FAILED,								// 学习技能失败
	MSG_S2C_SKILL_STUDY_BEGIN,								// 客户端返回打开技能学习导师页面
	MSG_S2C_OCCUSKILL_STUDY_BEGIN,							// 收到打开副职业技能学习页面消息
	MSG_S2C_OCCUSKILL_STUDY,								// 收到学习副职业技能消息
	MSG_S2C_OCCUSKILL_DELETE,								// 向客户端返回删除制作条目消息
	MSG_S2C_OCCUSKILL_NPCFACTURE_BEGIN,						// 向客户端发送NPC制作列表
	MSG_S2C_OCCUSKILL_NPCFACTURE,							// 发送NPC制作消息
	MSG_S2C_SKILL_UNLOCK_GOODS,								// 发送解除物品锁定消息
	MSG_S2C_SKILL_LOCK_GOODS,								// 锁定物品消息

	MSG_S2C_SKILL_ADD_PET,
	MSG_S2C_SKILL_DEL_PET,
	MSG_S2C_SKILL_USE_PET,
	MSG_S2C_SKILL_END_PET,

	MSG_S2C_ORGSYS					= MSG_S2C + MAX_SUB * 11,
	// ----------------------------------------------------

	// 行会协议-------------------------------------------------
	MSG_S2C_ORGSYS_InitOrgSys_Notify,					//! 初始化组织系统通知
	MSG_S2C_ORGSYS_UpdatePlayerFacInfo,					//! 更新一个玩家的工会信息

	//! 会长操作
	MSG_S2C_ORGSYS_CreateFaction_Notify,				//! 创建行会通知(创建检测完毕，检测客户端应回需一个新行会名称)
	MSG_S2C_ORGSYS_CreateFaction_Re,					//! 创建行会回应
	MSG_S2C_ORGSYS_UpgradeFaction_Notify,				//! 升级行会通知
	MSG_S2C_ORGSYS_UpgradeFaction_Re,					//! 升级行会回复
	MSG_S2C_ORGSYS_DisbandFaction_Notify,				//! 解散行会通知
	MSG_S2C_ORGSYS_DisbandFaction_Re,					//! 解散行会回应	
	MSG_S2C_ORGSYS_SetPlacard_Re,						//! 修改公告回应
	MSG_S2C_ORGSYS_SetLeaveword_Re,						//! 修改备忘回应
	MSG_S2C_ORGSYS_SetJobName_Re,						//! 修改官阶名字回应
	MSG_S2C_ORGSYS_SetJobPurview_Re,					//! 修改官阶权限回应
	MSG_S2C_ORGSYS_SetJobPurview_Notify,				//! 修改官阶权限回应
	MSG_S2C_ORGSYS_SetMemberJob_Re,						//! 设置成员官阶回应
	MSG_S2C_ORGSYS_SetMemberJob_Notify,					//! 设置成员官阶通知（新加）
	MSG_S2C_Faction_SetMemberTitle_Re,					//! 设置成员称号回应
	MSG_S2C_Faction_SetMemberTitle_Notify,				//! 设置成员称号通知（新加）
	MSG_S2C_Faction_SetChairman_Re,						//! 传位回复
	MSG_S2C_Faction_SetChairman_Notify,					//! 传位通知
	MSG_S2C_Faction_ChangeRecruitState_Notify,			//! 招募状态改变通知

	//! 成员操作
	MSG_S2C_ORGSYS_Quit_Re,								//! 退出行会回应
	MSG_S2C_ORGSYS_Quit_Notify,							//! 退出行会通知
	MSG_S2C_ORGSYS_Kick_Re,								//! 踢出行会回应
	MSG_S2C_ORGSYS_Kick_Notify,							//! 踢出行会通知
	MSG_S2C_ORGSYS_CheckRequisition_Re,					//! 审批入会回应
	MSG_S2C_ORGSYS_CheckRequisition_Notify,				//! 审批入会名单操作通知（新加）
	MSG_S2C_Faction_InvitePlayer_Re,					//! 邀请玩家加入回应
	MSG_S2C_Faction_InvitePlayer_Notify,				//! 邀请玩家加入通知
	MSG_S2C_Faction_AddMember_Notify,					//! 添加成员通知
	MSG_S2C_Faction_ApplyBoardOpen_Notify,				//! 打开申请入会面板通知

	MSG_S2C_ORGSYS_JoinFac_Re,							//! 加入请求回复

	//! 数据请求
	MSG_S2C_Faction_PublicData_Notify,					//! 行会资料（tagFacPublicData结构）
	MSG_S2C_Faction_BaseData_Notify,					//! 行会资料（tagFacBaseData结构）
	MSG_S2C_Faction_Member_Notify,						//! 成员和申请者
	MSG_S2C_Faction_Apply_Notify,						//! 成员和申请者
	MSG_S2C_Faction_Placard_Notify,						//! 公告资料回应
	MSG_S2C_Faction_Leaveword_Notify,					//! 备忘资料回应


	MSG_S2C_Faction_IconBoardOpen_Notify,				//! 打开图标上传面板通知
	MSG_S2C_Faction_IconData_Re,						//! 上传一个工会图标数据回复
	MSG_S2C_Faction_IconData_Notify,					//! 一个工会的图标数据更新通知
	MSG_S2C_Faction_AllIconData_Re,						//! 发送所有工会图标信息						

	MSG_S2C_Faction_MemberMap_Notifye,					//! 更新成员地图
	MSG_S2C_Faction_MemberLevel_Notifye,				//! 更新成员等级
	MSG_S2C_Faction_MemberOccu_Notifye,					//! 更新成员职业
	MSG_S2C_Faction_MemberOnline_Notifye,				//! 更新成员在线状态

	MSG_S2C_Faction_ChangeRes_Notify,					//! 资源值变更
	MSG_S2C_Faction_ChangeExp_Notify,					//! 经验值变更
	MSG_S2C_Faction_ChangeMemberContribute_Notify,		//! 成员贡献变更
	MSG_S2C_Faction_Beachheads_Notify,					//	据点战信息
	MSG_S2C_Faction_ChangeBattle_Notify,				//	修改战功总值
	MSG_S2C_Faction_ChangeArmyInvest_Notify,			//	修改军事投资值
	MSG_S2C_Faction_ChangeJoinBattle_Notify,			//	修改成员参与权
	MSG_S2C_Faction_ChangeEnemy_Notify,					//	修改敌对家族信息
	MSG_S2C_Faction_OpenGUI,							//	脚本调用,打开家族指定界面
	// End：行会------------------------------------------------



	// 联盟协议-------------------------------------------------
	MSG_S2C_UNION_Script_Creation,						//! 创建联盟脚本请求通知，打开联盟创建界面
	MSG_S2C_UNION_Re_Creation,							//! 创建联盟回复
	MSG_S2C_UNION_Notify_Creation,						//! 创建联盟通知
	MSG_S2C_UNION_Notify_Disband,						//! 解散联盟通知所有盟友
	MSG_S2C_UNION_Notify_Quit,							//! 行会退出联盟通知所有盟友
	MSG_S2C_UNION_Notify_Kick,							//! 盟主将行会踢出联盟通知
	MSG_S2C_UNION_Re_Kick,								//! 盟主将行会踢出联盟通知
	MSG_S2C_UNION_Notify_SetChairman,					//! 盟主转让通知所有盟友
	MSG_S2C_UNION_Re_SetRights,							//! 盟主赋予会长权限回复
	MSG_S2C_UNION_Notify_GetRights,						//! 会长一个人获得盟主赋予权限的通知
	MSG_S2C_UNION_Response_Invite,						//! 盟主 收到 行会会长 回复的邀请结果
	MSG_S2C_UNION_Notify_Anwser_Invite,					//! 行会会长 收到 盟主邀请加入联盟的请求
	MSG_S2C_UNION_Notify_Join,							//! 行会加入联盟通知所有盟友
	MSG_S2C_UNION_Notify_BaseData,						//! 一个联盟的基本数据通知
	MSG_S2C_UNION_Notify_MemberData,					//! 一个联盟的一个成员数据添加或更新的通知
	MSG_S2C_UNION_Notify_AllMemberData,					//! 一个联盟的所有成员数据通知
	// End：联盟------------------------------------------------




	MSG_S2C_Quest_Add,										//添加一个任务
	MSG_S2C_Quest_Complete,									//完成一个任务
	MSG_S2C_Quest_Remove,									//移出一个任务
	//MSG_S2C_Quest_Update,									
	MSG_S2C_Quest_UpdateStep,								//更新一个步骤
	MSG_S2C_Quest_UpdateParamData,							//更新一个任务的所有变量
	MSG_S2C_Quest_UpdateData,								//更新一个任务的变量
	MSG_S2C_Quest_UpdateRecordData,							//更新添加一个任务记录
	MSG_S2C_Quest_Talk,
	MSG_S2C_MercQuest_UpdateData,							//给客户端返回俑兵任务数据并同时打开界面
	MSG_S2C_MercQuest_OpenInterface,						//打开俑兵任务界面
	
	MSG_S2C_ANSWER_PlayerRanks,								//向客户端发送玩家排行榜
	//赏金任务
	MSG_S2C_SHARE_QUESTLIST_UPDATE,							//更新共享任务列表
	MSG_S2C_SHARE_QUEST_OPEN,								//打开队伍正在进行的任务界面
	MSG_S2C_SHARE_CURRQUEST,								//发送当前正在进行的任务ID
	//任务引导
	MSG_S2C_QuestIndex_OpanInterface,                       //打开任务索引界面
	MSG_S2C_QuestIndex_UpdateQuestParam,                    //更新一个任务的完成次数
	MSG_S2C_QuestIndex_UpdateQuestState,                    //更新一个任务的显示状态


	MSG_S2C_PLAYERSHOP				= MSG_S2C + MAX_SUB * 12,
	// ----------------------------------------------------
	MSG_S2C_PALYERSHOP_CREATE,
	MSG_S2C_PLAYERSHOP_BUYITEM,
	MSG_S2C_PLAYERSHOP_ADDSUCCEED,
	MSG_S2C_PLAYERSHOP_OPENFORBUSINESS,
	MSG_S2C_PLAYERSHOP_CLOSEDOWN,
	MSG_S2C_PLAYERSHOP_LOOKGOODS,
	MSG_S2C_PLAYERSHOP_CLOSE,
	MSG_S2C_PLAYERSHOP_QUIT,
	MSG_S2C_PLAYERSHOP_TOBUYER,
	MSG_S2C_PLAYERSHOP_TOSELLER,
	MSG_S2C_PLAYERSHOP_UPDATE,
	MSG_S2C_PLAYERSHOP_REMOVE,

	MSG_S2C_PLAYERSHOP_MSGBOARDGET,				// 服务器返回设置的留言板信息（商店名称、店主推荐以及交易信息）
	MSG_S2C_PLAYERSHOP_MSGBOARDCHAT,			// 服务器返回的在留言板中的聊天信息

	// 容器
	MSG_S2C_CONTAINER				= MSG_S2C + MAX_SUB * 13,
	// ----------------------------------------------------
	MSG_S2C_CONTAINER_OBJECT_MOVE,
	MSG_S2C_CONTAINER_OBJECT_UNLOCK,
	MSG_S2C_CONTAINER_OBJECT_AMOUNT_CHANGE,
	MSG_S2C_CONTAINER_CLEAR,

	// 宠物
	MSG_S2C_PET						= MSG_S2C + MAX_SUB * 14,
	// ----------------------------------------------------
	MSG_S2C_PET_AI,											// 宠物AI事件
	MSG_S2C_PET_MOVE,										// 移动
	MSG_S2C_PET_STOP,										// 停止
	MSG_S2C_PET_CHANGE_MODE,								// 改变宠物当前模式
	MSG_S2C_PET_CHANGE_ACTION,								// 改变宠物当前动作
	MSG_S2C_PET_CHANGE_SPEED,								// 改变速度
	MSG_S2C_PET_HOUSE,										// 宅藏宠物
	MSG_S2C_PET_CANCEL,										// 放弃宠物
	MSG_S2C_PET_CHANGE_MASTER,								// 改变主人

	//计费
	MSG_S2C_COST					= MSG_S2C + MAX_SUB * 15, 
	// -----------------------------------------------------
	MSG_S2C_COST_NOTENOUGHCOST,

	//寄售点卡 s->c
	MSG_S2C_CARD					= MSG_S2C + MAX_SUB * 16, 
	// -----------------------------------------------------
	MSG_S2C_CARD_SHOWWINDOW,								//　通知打开点卡出售窗口
	MSG_S2C_CARD_SELL_RESPONSE,								//　gs回应寄售请求结果
	MSG_S2C_CARD_BUY_RESPONSE,
	MSG_S2C_CARD_CANCEL_RESPONSE,
	MSG_S2C_CARD_LIST_RESPONSE,								//	返回当前出售的点卡列表
	MSG_S2C_CARD_SALECOSTS_RESPONSE,						//	返回当前出售的点卡列表

	MSG_S2C_CARD_CHARGE_RESPONSE,

	//国家
	MSG_S2C_COUNTRY					= MSG_S2C + MAX_SUB * 17, 
	// -----------------------------------------------------
	MSG_S2C_COUNTRY_CHANGE,									// 更改国籍


	// 增值商店
	MSG_S2C_INCREMENTSHOP					= MSG_S2C + MAX_SUB * 18,
	// ----------------------------------------------------
	MSG_S2C_INCREMENTSHOP_OPEN,										//打开商店			
	MSG_S2C_INCREMENTSHOP_BUY,										//gameserver返回从商店中购买一个物品的结果
			
	MSG_S2C_INCREMENTSHOP_CLOSE,							//##关闭个人商店
	MSG_S2C_INCREMENTSHOP_AFFICHE_RESPONSE,					// 公告窗口开

	MSG_S2C_INCREMENTSHOP_GETLOG_RESPONSE,					// 返回增值日志
	MSG_S2C_INCREMENTSHOP_UPDATA,							// 数据更新
	MSG_S2C_INCREMENTSHOP_UPDATE_RATIO,						// 返还比例数据更新

	MSG_S2C_INCREMENTSHOP_RE_INFO,							// 单条信息返回

	// 好友系统
	MSG_S2C_LINKMAN					= MSG_S2C + MAX_SUB * 19,
	// ----------------------------------------------------
	MSG_S2C_LINKMAN_INIT,										//! 初始化联系人列表
	MSG_S2C_LINKMAN_ADD,										// S 通知 A 添加联系人 B 的结果
	MSG_S2C_LINKMAN_DEL,										// S 通知 A 删除联系人 B 的结果
	MSG_S2C_LINKMAN_MOVE,										// S 通知 A 移动联系人 B 的结果

	MSG_S2C_LINKMAN_OTHER_HANDLE,								// S 通知 B： A 对 B 的 添加、删除、移动 等操作结果

	MSG_S2C_LINKMAN_REQUEST_ADD,								// S 通知 B： A 对 B 的添加申请

	MSG_S2C_LINKMAN_LOGIN,										// 联系人上线
	MSG_S2C_LINKMAN_LOGOUT,										// 联系人下线

	MSG_S2C_LINKMAN_INFO_CHANGE,								// 信息改变：地图、等级、职业
	MSG_S2C_LINKMAN_DELSTATE_CHANGE,							// 删除状态改变  

	MSG_S2C_LINKMAN_LEAVE_WORD,									//! 发送留言到客户端
	MSG_S2C_LEAVEWORD_SAVE_RESULT,								//! 留言储存结果

	//! 小精灵
	MSG_S2C_SPRITE					= MSG_S2C + MAX_SUB * 20,
	// ----------------------------------------------------
	MSG_S2C_SPRITE_AutoSupply_OnOff_Re,							//! 自动补给系统：回复功能开关
	MSG_S2C_SPRITE_AutoSupply_ChangeSetup_Re,					//! 自动补给系统：回复修改一个补给设置
	MSG_S2C_SPRITE_AutoSupply_UseItem_Notify,					//! 自动补给系统：通知使用了一个药品
	MSG_S2C_SPRITE_AutoFight_ChangeSetup_Re,                    //! 自动补给系统：回复修改一个打怪挂机设置
	MSG_S2C_SPRITE_AutoFight_ChangeStableTime,                  //! 自动补给系统：修改了稳定挂机时间



	MSG_S2C_SaveNewSetupData_Re,								//! 玩家保存新的游戏设置回复
	//! 邮件系统
	MSG_S2C_MAIL					= MSG_S2C + MAX_SUB * 29,
	MSG_S2C_MAIL_RECEIVER,										//!获得邮件数据
	MSG_S2C_MAIL_SEND_FAILED,									//!发信失败
	MSG_S2C_MAIL_SEND,											//!新邮件发送
	MSG_S2C_MAIL_READ,											//!读取邮件
	MSG_S2C_MAIL_FAILED_MOVE_GOODS,								//!邮件取物品失败
	MSG_S2C_MAIL_MOVE_GOODS_RESULT,								//!邮件取物品结果
	MSG_S2C_MAIL_DELETE_RESULT,									//!删除有附件邮件的确认消息
	MSG_S2C_MAIL_DELETE_OK,										//!删除邮件
	MSG_S2C_MAIL_AGREED_PAY,									//!付款收信接收消息
	MSG_S2C_MAIL_AGREED_PRESENT,								//!同意赠送
	MSG_S2C_MAIL_GET_GOLD_RESULT,								//!邮箱取金钱结果
	MSG_S2C_MAIL_PRESENT_FAILED,								//赠送失败
	MSG_S2C_MAIL_PRESENT_OK,									//附合赠送条件
	MSG_S2C_MAIL_SYSMAILMSG,									//系统邮件发送消息

	// Fox@20081107----------------------------------------------
	// 玩家管理系统( PlayerManager )
	MSG_S2C_PM						= MSG_S2C + MAX_SUB * 30,
	// -----------------------------------------------------
	MSG_S2C_PM_ACK,							// GS应答查询玩家信息

	// Expter@20090527
	//=======================================================================================================
	MSG_S2C_ANTICHEAT			    = MSG_S2C + MAX_SUB * 31,
	MSG_S2C_AC_QUESTION,
	MSG_S2C_AC_ANSWER_RET,
	MSG_S2C_AC_LOGINFAILED,

	// 赠送相关
	MSG_S2C_BOUNS					= MSG_S2C + MAX_SUB * 32,
	MSG_S2C_BOUNS_INFO_RESPOND,
	MSG_S2C_BOUNS_GET_RESULT,


	//! 战队系统
	MSG_S2C_SENTAI					= MSG_S2C + MAX_SUB * 33,
	// -----------------------------------------------------
	MSG_S2C_SENTAI_RE_QUEUE,		//! 答复进入或退出排队
	MSG_S2C_SENTAI_NOTIFY_READY,	//! 通知战队准备进入
	MSG_S2C_SENTAI_PK,				//! 当前场景的PK控制


	// ----------------------------------------------------
	//彩票系统
	MSG_S2C_LOTTERY					= MSG_C2S + MAX_SUB * 34,
	MSG_S2C_LOTTERY_Startup,					//! 开始旋转（long 0为成功，非0为失败枚举）（BYTE 抽中栏位值1～12）
	MSG_S2C_LOTTERY_ResetPrizes,				//! 重置奖品（long 0为成功，非0为失败枚举）（DWORD数组 12个物品的index）
	MSG_S2C_LOTTERY_ResetConstellations,		//! 重置星座（long 0为成功，非0为失败枚举）（BYTE数组 12个星座的pos）

	MSG_S2C_LOTTERY_PersonPoint,				//! 个人积分（long 积分值）
	MSG_S2C_LOTTERY_WorldPoint,					//! 世界积分（long 积分值）
	MSG_S2C_LOTTERY_TodayPoint,					//! 今日个人积分（long 积分值）
	MSG_S2C_LOTTERY_MaxTodayPoint,				//! 今日最高个人积分（long 积分值）
	MSG_S2C_LOTTERY_OpenLotteryPage,				//! 打开抽奖界面
	MSG_S2C_LOTTERY_FIELDTYPE,                  //! 轮盘类型
	MSG_S2C_LOTTERY_SystemReset,				//!进行强制重置
	MSG_S2C_LOTTERY_WorldInfo,				    //!世界积分信息 


	//////////////////
	//				//
	//	CostServer	//
	//				//
	//////////////////

	//========================================================================================================
	MSG_W2COST = MAX_TYPE * 13,			// World Server -> Cost Server
	//========================================================================================================

	//计费
	MSG_W2COST_COST					= MSG_W2COST + MAX_SUB * 0,
	// -------------------------------------------------------
	MSG_W2COST_COST_LOGIN,
	MSG_W2COST_COST_CHANGE,
	MSG_W2COST_COST_COST,
	MSG_W2COST_COST_OFFLINE,
	MSG_W2COST_COST_ALLOFFLINE,						//当WorldServer重启后，发送此消息
	//MSG_W2COST_COST_BATCHLOGIN,					//当WorldServer被GameServer重连成功时，发送此消息
	MSG_W2COST_COST_QUESTINFO,						// 请求帐号信息

	// 点卡寄售
	MSG_W2COST_CARD					= MSG_W2COST + MAX_SUB * 1,
	// ------------------------------------------------------
	MSG_W2COST_CARD_VALIDATE_REQUEST,				// 转发寄售点卡消息到COST服务器，进行点卡验证
	MSG_W2COST_CARD_SELL_REQUEST,					// 发送寄售请求
	MSG_W2COST_CARD_RESERVE_REQUEST,					// 发送购买请求
	MSG_W2COST_CARD_BUY_REQUEST,					// 发送购买请求
	MSG_W2COST_CARD_CANCEL_REQUEST,					// 取消寄售
	MSG_W2COST_CARD_TIMEOUT,						// 寄售卡超时下架

	MSG_W2COST_CARD_CHARGE_REQUEST,

	//========================================================================================================
	MSG_COST2W = MAX_TYPE * 14,		// Cost Server -> World Server
	//========================================================================================================

	//计费
	MSG_COST2W_COST					= MSG_COST2W + MAX_SUB * 0,
	//---------------------------------------------------------
	MSG_COST2W_COST_NOTENOUGHCOST,
	MSG_COST2W_COST_INFORMATION,		
	MSG_COST2W_COST_ISEVERCONNECT,
	MSG_COST2W_COST_ANSWERINFO,						// 返回帐号信息

	// 点卡寄售
	MSG_COST2W_CARD					= MSG_COST2W + MAX_SUB * 1,  
	// --------------------------------------------------------
	MSG_COST2W_CARD_VALIDATE_RESPONSE,				// 点卡验证结果
	MSG_COST2W_CARD_SELL_RESPONSE,					// 返回寄售请求
	MSG_COST2W_CARD_RESERVE_RESPONSE,
	MSG_COST2W_CARD_BUY_RESPONSE,					// 返回购买请求
	MSG_COST2W_CARD_CANCEL_RESPONSE,					

	MSG_COST2W_CARD_CHARGE_RESPONSE,

	//////////////////
	//				//
	//	DBServer	//
	//				//
	//////////////////

	//========================================================================================================
	MSG_DB2DB = MAX_TYPE * 15,			// DBServer --> DBServer
	//========================================================================================================

	MSG_DB2DB_SERVER					= MSG_DB2DB + MAX_SUB * 0,
	// ----------------------------------------------------

	//========================================================================================================
	MSG_W2DB = MAX_TYPE * 16,			// WS --> DBS
	//========================================================================================================

	MSG_W2DB_SERVER					= MSG_W2DB + MAX_SUB * 0, //WS->DBS
	// ----------------------------------------------------
	MSG_W2DB_SERVER_SENDSERVERINFO,							// 发送server信息给DBServer
	MSG_W2DB_SERVER_WORLDSERVERCLOSE,						// DBS检测到WS关闭
	MSG_W2DB_SERVER_GAMESERVERLOST,							// DB检测到GS断开
	MSG_W2DB_RECONNECTWORLDSERVEROK,							// 重连LoginServer成功
	MSG_W2DB_SERVER_COSTSERVERCLOSE,
	MSG_W2DB_RECONNECTCOSTSERVEROK,
	MSG_W2DB_SERVER_ANSWER_PING,
	MSG_W2DB_SERVER_QUEST_RECONNECTION,
	MSG_W2DB_SERVER_SENDSETUP,
	//MSG_W2W_SERVER_CLEAROFFLINEPLAYER,					// 存盘线程在存盘成功后发消息给进程清楚下线玩家列表


	MSG_W2DB_SERVER_UNREGENTITY,							// 清除DBS管理的对象
	MSG_W2DB_SERVER_PLAYER_PERSONALHOUSEGUID,               // 查询玩家的个人房屋数据
	MSG_W2DB_SERVER_SAVEDATAEND,							// 调用DoSaveData消息发送结束标志

	MSG_W2DB_SERVER_QUEST_PING,


	MSG_W2DB_SAVE					= MSG_W2DB + MAX_SUB * 1, //WS->DBS
	// ----------------------------------------------------
	MSG_W2DB_SAVE_PLAYER,									// 保存Player数据
	MSG_W2DB_SAVE_PLAYER_GOOD,								// 保存Player的Good数据
	MSG_W2DB_SAVE_COUNTRY,							// 重连LoginServer成功
	MSG_W2DB_SAVE_CREATE_PLAYER,                    // 保存创建的教色
	MSG_W2DB_SAVE_DELETE_PLAYER,                    // 保存删除的教色
	MSG_W2DB_SAVE_CARDS_LARGESS,	
	MSG_W2DB_SAVE_ALL_CITY_WAR_PARAM,
	MSG_W2DB_SAVE_ALL_VILLAGE_WAR_PARAM,
	MSG_W2DB_SAVE_ALL_ENEMY_FACTIONS,
	MSG_W2DB_SAVE_REGION_PARAM,
	MSG_W2DB_SAVE_CONFEDERATION,
	MSG_W2DB_SAVE_FACTION,
	MSG_W2DB_SAVE_PERSONALHOUSE,

	MSG_W2DB_LOAD					= MSG_W2DB + MAX_SUB * 2, //WS->DBS
	// ----------------------------------------------------
	MSG_W2DB_LOAD_INIT_PLAYER,								// WS初始化从DB读取Player
	MSG_W2DB_LOAD_PLAYER,									// 保存Player数据
	MSG_W2DB_LOAD_PLAYER_GOOD,								// 保存Player的Good数据
	MSG_W2DB_LOAD_COUNTRY,								// 重连LoginServer成功
	MSG_W2DB_LOAD_INIT_ENEMY_FACTION,					// 读取敌对行会数据
	MSG_W2DB_LOAD_INIT_COUNTRY,
	MSG_W2DB_LOAD_PLAYER_WORLD_ID,
	MSG_W2DB_LOAD_REGION_PARAM,
	MSG_W2DB_LOAD_CONFEDERATION,
	MSG_W2DB_LOAD_ALL_CONFEDERATIONS,
	MSG_W2DB_LOAD_FACTION,
	MSG_W2DB_LOAD_ALL_FACTIONS,
	MSG_W2DB_LOAD_CARDS_LARGESS,
	MSG_W2DB_LOAD_ALL_CITY_WAR_PARAM,
	MSG_W2DB_LOAD_ALL_VILLAGE_WAR_PARAM,
	MSG_W2DB_LOAD_ALL_ENEMY_FACTIONS,
	MSG_W2DB_LOAD_PERSONALHOUSE,
	MSG_WSDB_LOAD_UNISTRINGRES,

	MSG_W2DB_PASS					= MSG_W2DB + MAX_SUB * 3, //WS->DBS
	// ----------------------------------------------------
	MSG_W2DB_PASS_LOG_PLAYERBASELIST,				// WS传递LS的请求
	MSG_W2DB_PASS_LOG_QUEST_PLAYERDATA,				// WS传递LS的请求
	MSG_W2DB_PASS_LOG_DELETEROLE,					// WS请求删除一个角色
	MSG_W2DB_PASS_LOG_RESTOREROLE,					// WS请求恢复一个角色
	MSG_W2DB_PASS_LOG_CREATEROLE,					// WS请求创建一个角色
	MSG_W2DB_PASS_CARD_VALIDATE_RESPONSE,			// 棘手卡
	MSG_W2DB_PASS_GM_BANPLAYER,						// 传递S2W的GM_BANPLAYER消息
	MSG_W2DB_PASS_LOAD_ALL_DUPRGN,					// 初始化读取所有PH数据
	MSG_W2DB_PSASS_OTHER_GETLOG_REQUEST,			// WS传递GS来的请求读增量日志消息
	MSG_W2DB_PSASS_OTHER_WRITELOG,					// WS传递GS来的写增量日志消息
	MSG_W2DB_PASS_LOAD_ALL_FACTION,					// 请求所有工会数据
	MSG_W2DB_PASS_DelOneFaction,					// 请求删除一个工会数据
	MSG_W2DB_PASS_LOAD_ALL_SCRIPTVAR,				// 初始化读取所有全局脚本变量数据

	MSG_W2DB_OPER					= MSG_W2DB + MAX_SUB * 4, //WS->DBS
	// ----------------------------------------------------
	MSG_W2DB_OPER_ENTITY,

	MSG_W2DB_RGN					= MSG_W2DB + MAX_SUB * 5, //DBS->WS
	// ----------------------------------------------------
	MSG_W2DB_RGN_LOAD_PERSONALHOUSE,
	MSG_W2DB_RGN_SAVE_PERSONALHOUSE,

	//! 好友系统
	MSG_W2DB_LINKMAN			= MSG_W2DB + MAX_SUB * 6, 
	// ----------------------------------------------------
	MSG_W2DB_LEAVEWORD_SETUP,								//! 发送留言配置
	MSG_W2DB_LEAVEWORD_SAVE,								//! 保存留言请求
	MSG_W2DB_LEAVEWORD_POP,									//! 取出一个玩家的留言
	MSG_W2DB_LEAVEWORD_CLEAR_OUTTIME,						//! 清理超时的记录

	//! 邮件系统
	MSG_W2DB_MAIL				= MSG_W2DB + MAX_SUB * 7,
	// ----------------------------------------------------
	MSG_W2DB_MAIL_INITLOAD,
	MSG_W2DB_MAIL_INSERT_OFFLINE, // 添加一个不在线玩家邮件Group
	MSG_W2DB_MAIL_DELETE_OFFLINE, // 删除一个不在线玩家邮件Group
	MSG_W2DB_MIAL_QUERY,
	MSG_W2DB_MIAL_ADDOFFLINEPLAYERCOSTGOLD, // 添加不在线玩家的付费信件金币
	MSG_W2DB_MIAL_SENDSAVEGROUPMAILS,

	//========================================================================================================
	MSG_S2DB = MAX_TYPE * 17,			// GS --> DBS
	//========================================================================================================

	MSG_S2DB_SERVER					= MSG_S2DB + MAX_SUB * 0, //WS->DBS
	// ----------------------------------------------------
	MSG_S2DB_SERVER_SENDSERVERINFO,							// 发送server信息给DBServer
	MSG_S2DB_SERVER_WORLDSERVERCLOSE,						// DBS检测到WS关闭
	MSG_S2DB_SERVER_GAMESERVERCLOSE,							// DB检测到GS断开
	MSG_S2DB_RECONNECTGAMESERVEROK,							// 重连LoginServer成功
	MSG_S2DB_SERVER_ANSWER_PING,
	MSG_S2DB_SERVER_QUEST_RECONNECTION,


	MSG_S2DB_SAVE					= MSG_S2DB + MAX_SUB * 1, //WS->DBS
	// ----------------------------------------------------
	MSG_S2DB_SAVE_PLAYER,									// 保存Player数据
	MSG_S2DB_SAVE_PLAYER_GOOD,								// 保存Player的Good数据
	MSG_S2DB_SAVE_COUNTRY,									// 重连LoginServer成功
	MSG_S2DB_SAVE_FACTION,				
	MSG_S2DB_SAVE_CONFEDERATION,


	MSG_S2DB_LOAD					= MSG_S2DB + MAX_SUB * 2, //WS->DBS
	// ----------------------------------------------------
	MSG_S2DB_LOAD_INIT_PLAYER,								// DB反馈WS初始化读取玩家数据
	MSG_S2DB_LOAD_PLAYER,									// 保存Player数据
	MSG_S2DB_LOAD_PLAYER_GOOD,								// 保存Player的Good数据
	MSG_S2DB_LOAD_COUNTRY,								// 重连LoginServer成功
	MSG_S2DB_LOAD_FACTION,				
	MSG_S2DB_LOAD_CONFEDERATION,

	MSG_S2DB_OPER					= MSG_S2DB + MAX_SUB * 3, //GS->DBS
	// ----------------------------------------------------
	MSG_S2DB_OPER_CARD,
	MSG_S2DB_OPER_CITY_WAR,
	MSG_S2DB_OPER_ENEMY_FACTION,
	MSG_S2DB_OPER_FACTION,
	MSG_S2DB_OPER_GEN_VAR,
	MSG_S2DB_OPER_GOOD,
	MSG_S2DB_OPER_PLAYER,
	MSG_S2DB_OPER_DUPRGN,
	MSG_S2DB_OPER_REGION,
	MSG_S2DB_OPER_SETUP,
	MSG_S2DB_OPER_UNION,
	MSG_S2DB_OPER_VILLAGEWAR,
	MSG_S2DB_OPER_COUNTRY,
	//========================================================================================================
	MSG_DB2W = MAX_TYPE * 18,			// DBS --> WS 
	//========================================================================================================

	MSG_DB2W_SERVER					= MSG_DB2W + MAX_SUB * 0, //DBS --> WS 
	// ----------------------------------------------------
	MSG_DB2W_SERVER_QUEST_PING,
	MSG_DB2W_SERVER_QUEST_RECONNECTION,
	MSG_DB2W_SERVER_SENDSETUP,
	MSG_DB2W_SERVER_INIT_PLAYERDATABUFFER, // 初始化CLoginPlayer和CPlayer数据缓冲区,提高查找速度
	MSG_DB2W_SERVER_PLAYER_PERSONALHOUSEGUID,               // 查询玩家的个人房屋数据

	MSG_DB2W_PASS						= MSG_DB2W + MAX_SUB * 1, //DBS --> WS 
	// ----------------------------------------------------
	MSG_DB2W_PASS_LOG_PLAYERBASELIST,        // 回应LS从WS传递的请求
	MSG_DB2W_PASS_LOG_PLAYERBASELIST_DELTIMER, // 发送回应LS从WS传递的请求删除定时器消息
	MSG_DB2W_PASS_LOG_QUEST_PLAYERDATA,				// 回应LS从WS传递的请求
	MSG_DB2W_PASS_LOG_CREATEROLE,
	MSG_DB2W_PASS_LOG_DELETEROLE,
	MSG_DB2W_PASS_LOG_RESTOREROLE,
	MSG_DB2W_PASS_GM_BANPLAYER,						// 传递S2W的GM_BANPLAYER消息
	MSG_DB2W_PASS_LOAD_ALL_DUPRGN,
	MSG_DB2W_PSASS_OTHER_GETLOG_REQUEST,			// DBS回应WS从GS来的请求读增量日志消息
	MSG_DB2W_PASS_LOAD_ALL_FACTION_FINISH,			// 读取工会信息完成
	MSG_DB2W_PASS_DelFaction_Finish,				// 删除工会完成
	MSG_DB2W_PASS_LOAD_ALL_SCRIPTVAR,				// 初始化读取所有全局脚本变量数据

	MSG_DB2W_SAVE					= MSG_DB2W + MAX_SUB * 2, //DBS --> WS 
	// ----------------------------------------------------
	MSG_DB2W_SAVE_PLAYER,
	MSG_DB2W_SAVE_PLAYER_GOOD,
	MSG_DB2W_SAVE_COUNTRY,
	MSG_DB2W_SAVE_CREATE_PLAYER,                    // 保存创建的教色
	MSG_DB2W_SAVE_DELETE_PLAYER,                    // 保存删除的教色
	MSG_DB2W_SAVE_CARDS_LARGESS,
	MSG_DB2W_SAVE_ALL_CITY_WAR_PARAM,
	MSG_DB2W_SAVE_ALL_VILLAGE_WAR_PARAM,
	MSG_DB2W_SAVE_ALL_ENEMY_FACTIONS,
	MSG_DB2W_SAVE_REGION_PARAM,
	MSG_DB2W_SAVE_CONFEDERATION,
	MSG_DB2W_SAVE_FACTION,
	MSG_DB2W_SAVE_PERSONALHOUSE,

	MSG_DB2W_LOAD					= MSG_DB2W + MAX_SUB * 3, //DBS --> WS 
	// ----------------------------------------------------
	MSG_DB2W_LOAD_INIT_ENEMY_FACTION,
	MSG_DB2W_LOAD_INIT_COUNTRY,
	MSG_DB2W_LOAD_PLAYER,
	MSG_DB2W_LOAD_INIT_PLAYER,
	MSG_DB2W_LOAD_PLAYER_GOOD,
	MSG_DB2W_LOAD_COUNTRY,
	MSG_DB2W_LOAD_PLAYER_WORLD_ID,
	MSG_DB2W_LOAD_REGION_PARAM,
	MSG_DB2W_LOAD_CONFEDERATION,
	MSG_DB2W_LOAD_ALL_CONFEDERATIONS,
	MSG_DB2W_LOAD_FACTION,
	MSG_DB2W_LOAD_ALL_FACTIONS,
	MSG_DB2W_LOAD_CARDS_LARGESS,
	MSG_DB2W_LOAD_ALL_CITY_WAR_PARAM,
	MSG_DB2W_LOAD_ALL_VILLAGE_WAR_PARAM,
	MSG_DB2W_LOAD_ALL_ENEMY_FACTIONS,
	MSG_DB2W_LOAD_PERSONALHOUSE,

	MSG_DB2W_OPER					= MSG_DB2W + MAX_SUB * 4, //GS->DBS
	// ----------------------------------------------------
	MSG_DB2W_OPER_ENTITY,

	MSG_DB2W_RGN					= MSG_DB2W + MAX_SUB * 5, //DBS->WS
	// ----------------------------------------------------
	MSG_DB2W_RGN_LOAD_PERSONALHOUSE,
	MSG_DB2W_RGN_SAVE_PERSONALHOUSE,

	//! 好友系统
	MSG_DB2W_LINKMAN				= MSG_DB2W + MAX_SUB * 6,
	// ----------------------------------------------------
	MSG_DB2W_LINKMAN_PLAYER_DATA_SAVE_NOTIFY,					//! 玩家联系人数据保存通知

	MSG_DB2W_LEAVEWORD_SAVE_RESULT,								//! 回复留言保存结果
	MSG_DB2W_LEAVEWORD_RE_POP,									//! 回复取出玩家留言

	//! 邮件系统
	MSG_DB2W_MAIL				= MSG_DB2W + MAX_SUB * 7,
	// ----------------------------------------------------
	MSG_DB2W_MAIL_INITLOAD,
	MSG_DB2W_MAIL_OPERATION_RESLUT,
	MSG_DB2W_MIAL_QUERY,
	MSG_DB2W_MIAL_ADDOFFLINEPLAYERCOSTGOLD, // 回应添加不在线玩家的付费信件金币


	//========================================================================================================
	MSG_DB2S = MAX_TYPE * 19,			// DBS --> GS 
	//========================================================================================================

	MSG_DB2S_SERVER					= MSG_DB2S + MAX_SUB * 0, //DBS --> GS 
	// ----------------------------------------------------
	MSG_DB2S_SERVER_QUEST_PING,
	MSG_DB2S_SERVER_QUEST_RECONNECTION,
	MSG_DB2S_SERVER_PLAYER_PERSONALHOUSEGUID,               // 查询玩家的个人房屋数据


	MSG_DB2S_SAVE					= MSG_DB2S + MAX_SUB * 1, //DBS --> GS 
	// ----------------------------------------------------
	MSG_DB2S_SAVE_PLAYER,
	MSG_DB2S_SAVE_FACTION,				
	MSG_DB2S_SAVE_CONFEDERATION,

	MSG_DB2S_LOAD					= MSG_DB2S + MAX_SUB * 2, //DBS --> GS 
	// ----------------------------------------------------
	MSG_DB2S_LOAD_PLAYER,
	MSG_DB2S_LOAD_FACTION,				
	MSG_DB2S_LOAD_CONFEDERATION,


	MSG_DB2S_PASS						= MSG_DB2S + MAX_SUB * 3, //DBS --> WS 
	// ----------------------------------------------------
	MSG_DB2S_PASS_LOG_PLAYERBASELIST,        // 回应LS从WS传递的请求
	MSG_DB2S_PASS_LOG_QUEST_PLAYERDATA,				// 回应LS从WS传递的请求

	MSG_DB2S_LOG						= MSG_DB2S + MAX_SUB * 4, //DBS --> WS 
	// ----------------------------------------------------
	MSG_DB2S_LOG_PLAYER_ONLINE,

	MSG_DB2S_OPER					= MSG_DB2S + MAX_SUB * 5, //GS->DBS
	// ----------------------------------------------------
	MSG_DB2S_OPER_CARD,
	MSG_DB2S_OPER_CITY_WAR,
	MSG_DB2S_OPER_ENEMY_FACTION,
	MSG_DB2S_OPER_FACTION,
	MSG_DB2S_OPER_GEN_VAR,
	MSG_DB2S_OPER_GOOD,
	MSG_DB2S_OPER_PLAYER,
	MSG_DB2S_OPER_REGION,
	MSG_DB2S_OPER_DUPRGN,
	MSG_DB2S_OPER_SETUP,
	MSG_DB2S_OPER_UNION,
	MSG_DB2S_OPER_VILLAGEWAR,
	MSG_DB2S_OPER_COUNTRY,

	//========================================================================================================
	MSG_02LOG = MAX_TYPE * 20,			// OtherServer --> LogServer
	//========================================================================================================
	MSG_02LOG_SERVER					= MSG_02LOG + MAX_SUB * 0, //! 一般消息
	// ----------------------------------------------------
	MSG_02LOG_SERVER_LOGIN,			//! 登陆信息发送
	MSG_02LOG_SERVER_LOG,			//! 发送需要记录的日志
	MSG_02LOG_SERVER_LOG_SQLString, //! 发送需要记录的日志，以SQL语句形式发送



	//MSG_02LOG_LOG					= MSG_02LOG + MAX_SUB * 1, //! 日志发送
	// ----------------------------------------------------


	//========================================================================================================
	MSG_LOG20 = MAX_TYPE * 21,			// LogServer --> OtherServer
	//========================================================================================================
	MSG_LOG20_SERVER					= MSG_LOG20 + MAX_SUB * 1, // 一般消息
	// ----------------------------------------------------
	MSG_LOG20_SERVER_TABLE,			//表格结构信息
	MSG_LOG20_SERVER_CONDITION,		//消息筛选条件
	MSG_LOG20_SERVER_AFFIRM,		//确认服务器在LogServer上有配置的数据库

	///
	/// WorldServer与MonsterServer之间的消息值，具体消息值定义在其他文件中。
	///
	MSG_W2M_BEGIN = MAX_TYPE * 22,
	/// Do NOT insert any values here
	MSG_W2M_END = MAX_TYPE * 23 - 1,

	MSG_M2W_BEGIN = MAX_TYPE * 23,
	/// Do NOT insert any values here
	MSG_M2W_END = MAX_TYPE * 24 - 1,

	///
	/// GameServer与Monster之间的消息值，具体消息值定义在其他文件中。
	///
	MSG_G2M_BEGIN = MAX_TYPE * 24,
	/// Do NOT insert any values here
	MSG_G2M_END = MAX_TYPE * 25 - 1,

	MSG_M2G_BEGIN = MAX_TYPE * 25,
	/// Do NOT insert any values here
	MSG_M2G_END = MAX_TYPE * 26 - 1,

	///
	/// GameServer与WorldServer之间的消息值(关于加入怪物服务器逻辑)，具体消息定义在其他文件中。
	///
	MSG_G2W_BEGIN = MAX_TYPE * 26,
	/// Do NOT insert any values here
	MSG_G2W_END = MAX_TYPE * 27 - 1,

	MSG_W2G_BEGIN = MAX_TYPE * 27,
	/// Do NOT insert any values here
	MSG_W2G_END = MAX_TYPE * 28 - 1,

	MSG_B2S = MAX_TYPE * 30,			// BillingServer --> GameServer
	MSG_B2S_GET_BALANCE,
	MSG_B2S_ACCOUNT_LOCK,
	MSG_B2S_ACCOUNT_UNLOCK,
	MSG_B2S_ACCOUNT_UPDATED,
	MSG_B2S_SDO_ITEM_SEND_REQUEST,



	//========================================================================================================



	MSG_S2B = MAX_TYPE * 31,			// GameServer --> BillingServer
	MSG_S2B_SENDINFO,
	MSG_S2B_GET_BALANCE,
	MSG_S2B_ACCOUNT_LOCK,
	MSG_S2B_ACCOUNT_UNLOCK,
	MSG_S2B_ACCOUNT_LOCK_ROLLBACK,

	//========================================================================================================
	MSG_O2A = MAX_TYPE * 31,			//! OtherServer --> AccountServer
	//========================================================================================================
	MSG_O2A_LOGIN					= MSG_O2A + MAX_SUB * 0,	//! 登陆消息
	// ----------------------------------------------------
	MSG_O2A_LOGIN_Affirm_Request,								//! 登陆确认请求
	MSG_O2A_LOGIN_SendOnlineInfo,								//! 发送在线账号信息


	MSG_O2A_ACCOUNT					= MSG_O2A + MAX_SUB * 1,	//! 账号操作消息
	// ----------------------------------------------------
	MSG_O2A_ACCOUNT_Validate_Request,							//! 账号验证请求
	MSG_O2A_ACCOUNT_Reject_Request,								//! 封号请求
	MSG_O2A_ACCOUNT_Logout,										//! 通知AS账号退出
	MSG_O2A_ACCOUNT_ListLogout,									//! 通知AS一堆账号退出
	MSG_O2A_SERVER_RUN_ERROR,									//! 报告游戏服务器运行错误

	



	//========================================================================================================
	MSG_A2O = MAX_TYPE * 32,			//! AccountServer --> OtherServer
	//========================================================================================================
	MSG_A2O_LOGIN					= MSG_A2O + MAX_SUB * 0,	//! 登陆消息
	// ----------------------------------------------------
	MSG_A2O_LOGIN_Affirm_Re,									//! 登陆确认回应


	MSG_A2O_ACCOUNT					= MSG_A2O + MAX_SUB * 1,	//! 账号操作消息
	// ----------------------------------------------------
	MSG_A2O_ACCOUNT_Validate_Re,								//! 账号验证回应
	MSG_A2O_ACCOUNT_FcmTime_Notify,								//! 防沉迷剩余时间通知
	MSG_A20_SECURITY_DATA,										//! 发送账号的安全数据


	//========================================================================================================
	MSG_SCC2SM = MAX_TYPE * 33,			//! SCC --> SM
	//========================================================================================================

	//========================================================================================================
	MSG_SM2SCC = MAX_TYPE * 34,			//! SM --> SCC
	//========================================================================================================

	//========================================================================================================
	MSG_SCC2SUC = MAX_TYPE * 34,			//! SCC --> SUC
	//========================================================================================================

	//========================================================================================================
	MSG_SUC2SCC = MAX_TYPE * 35,			//! SUC --> SCC
	//========================================================================================================

	//////////////////////////////////////////////////////////////////////////
	// GameServer ---> BounsServer
	//////////////////////////////////////////////////////////////////////////
	MSG_S2BDS	= MAX_TYPE * 36,
	MSG_S2BDS_SENDSERVERINFO,				// GameServer信息
	MSG_S2BDS_BOUNS_LOAD,					// 查询奖励信息
	MSG_S2BDS_BOUNS_GET,					// 获取奖励
	MSG_S2BDS_BOUNS_ROLLBACK,				// 数据回滚
    MSG_S2BDS_UPLOAD_GOODS,                 // 上传物品数据

	//////////////////////////////////////////////////////////////////////////
	// BounsServer ----> GameServer
	//////////////////////////////////////////////////////////////////////////
	MSG_BDS2S	= MAX_TYPE * 37,
	MSG_BDS2S_BOUNS_LOAD_RESPOND,
	MSG_BDS2S_BOUNS_GET_RESPOND,
	MSG_BDS2S_BOUNS_DES_UPDATE,				// 更新奖励描述资源
    MSG_BDS2S_NOTICE,                       // 公告
};