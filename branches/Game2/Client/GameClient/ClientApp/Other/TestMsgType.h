/***********************************************************************************
/* 创建     : 刘科
/* 创建时间 : 2009年2月2日
/* 摘要     : 定义了客户端与测试程序之间的消息通信类型（内存影射方式）
/*			  注：这里定义的消息值是对应于MsgType消息值的，主要是为了更便捷的转发
/**********************************************************************************/
#pragma once

enum eTMsgNum
{
	TMAX_SUB			= 0x100,				// 一个小类型的最大消息数量
	TMAX_TYPE			= TMAX_SUB * 0xff,		// 一个大类型的最大消息数量
};

enum eTMsgType
{

	//////////////////////
	//				    //
	//	   FILE_R_W		//
	//				    //
	//////////////////////

	//MSG_FILE						= TMAX_TYPE * 37,			// 文件操作
	MSG_FILE_OPEN	=	1,										// 打开文件
	MSG_FILE_WRITE,												// 写文件
	MSG_FILE_CLEAR,												// 清空文件
	MSG_FILE_CLOSE,												// 关闭文件

	//////////////////////
	//				    //
	//	 ClientToTest	//
	//				    //
	//////////////////////
	//========================================================================================================
	MSG_L2T							= TMAX_TYPE * 11,			// Log -> Test(对应MSG_L2C = 718080)
	//========================================================================================================
	MSG_L2T_LOG						= MSG_L2T + TMAX_SUB * 0,	// 登陆类型(对应MSG_L2C_LOG = 718080)
	// ----------------------------------------------------
	MSG_L2T_LOG_ANSWERCDKEY,								// 服务器返回CDKEY
	MSG_L2T_LOG_ANSWER_PLAYERBASELIST,						// 服务器返回玩家基本数据列表
	MSG_L2T_LOG_ANSWER_PLAYERDATA,							// 返回玩家详细数据
	MSG_L2T_LOG_ANSWERCREATE,								// 服务器返回建立角色的信息
	MSG_L2T_LOG_ANSWERDELETE,								// 服务器返回是否正功删除消息
	MSG_L2T_LOG_ANSWERRESTORE,								// 是否成功回复角色
	MSG_L2T_LOG_UPDATEQUEUENUM,								// 更新登陆缓冲队列的剩余数量

	MSG_L2T_LOG_SENDACCOUNTINFO,							// 发送帐号的信息给客户端
	MSG_L2T_LOG_UPDATEWORLDSERVERINFO,						// 更新WorldServer的状态信息


	//========================================================================================================
	MSG_C2T							= TMAX_TYPE * 12,			// Client --> Test(对应MSG_S2C = 783360)
	//========================================================================================================
	MSG_C2T_LOG						= MSG_S2C + MAX_SUB * 0,
	// ----------------------------------------------------
	MSG_C2T_LOG_ANSWER_PLAYERDATA,							// GameServer向GameClient返回player详细数据
	MSG_C2T_LOG_GAMESERVERTIME,								// 发送Server的时间
	MSG_C2T_LOG_SEQUENCESTRING,								// 验证消息合法性的字符串序列

	// ----------------------------------------------------
	MSG_C2T_RGN						= MSG_C2T + TMAX_SUB * 1,	// 场景信息(对应MSG_S2C_RGN = 783616)
	// ----------------------------------------------------
	MSG_C2T_RGN_ADDSHAPE = 783618,							// 有对象加入场景(对应MSG_S2C_RGN_ADDSHAPE = 783618)
	MSG_C2T_RGN_DELSHAPE = 783620,							// 有对象离开场景(对应MSG_S2C_RGN_DELSHAPE = 783620)
	MSG_C2T_RGN_CHANGE   = 783621,							// 服务器命令玩家切换场景(对应MSG_S2C_RGN_CHANGE)
	MSG_C2T_RGN_CHANGESERVER = 783622,						// 服务器命令玩家切换场景服务器(对应MSG_S2C_RGN_CHANGESERVER)
	MSG_C2T_RGN_CHANGEWEATHER = 783623,						// 改变天气
	MSG_C2T_RGN_PLAYACTION = 783624,						// 播放动作
	MSG_C2T_RGN_CHANGENAME = 783627,						// 改变场景名字(对应MSG_S2C_RGN_CHANGENAME)
	MSG_C2T_RGN_CLEARDUPRGNPLAYER = 783629,					// 副本清人消息(对应MSG_S2C_RGN_CLEARDUPRGNPLAYER)


	// ----------------------------------------------------
	MSG_C2T_SHAPE					= MSG_C2T + TMAX_SUB * 2,			// 对象信息(对应MSG_S2C_SHAPE = 783872)
	// ----------------------------------------------------
	MSG_C2T_SHAPE_CHANGEDIR = 783873,						// 改变方向
	MSG_C2T_SHAPE_SETPOS = 783875,							// 瞬移
	MSG_C2T_SHAPE_FORCEMOVE,								// 强制移动
	MSG_C2T_SHAPE_MOVE,										// 回复移动消息
	MSG_C2T_SHAPE_STOPMOVE,									// 停止移动消息
	MSG_C2T_SHAPE_CHANGEMOVEMODE,							// 移动模式改变
	MSG_C2T_SHAPE_CHANGESTATE = 783881,						// 改变状态(对应MSG_S2C_SHAPE_CHANGESTATE = 783881)
	MSG_C2T_SHAPE_ATK_DIED = 783884,						// 死亡
	MSG_C2T_SHAPE_SINSTATE = 783887,						// 通知client进入或退出犯罪状态
	MSG_C2T_SHAPE_Refresh = 783888,							// 刷新建筑状态 
	MSG_C2T_SHAPE_EMOTION = 783890,							// 表情

	MSG_C2T_SHAPE_ATK_DAMAGE,								// 普通受伤消息
	MSG_C2T_SHAPE_ATK_BREAK,								// 被攻击
	MSG_C2T_SHAPE_ATK_FULLMISS,								// 闪避
	MSG_C2T_SHAPE_ATK_PARRY,								// 招架
	MSG_C2T_SHAPE_ATK_BLOCK,								// 格挡
	MSG_C2T_SHAPE_ATK_RESIST,								// 抵抗
	MSG_C2T_SHAPE_ATK_REBOUND, 							    // 反弹
	MSG_C2T_SHAPE_ATK_SUCK,									// 吸收
	MSG_C2T_SHAPE_RELIVE,									// 复活一个玩家
	MSG_C2T_SHAPE_CHANGE_PROPERTIES,						// 更新可移动物体的属性
	MSG_C2T_SHAPE_ATK_LOSE,									// 丢失
	MSG_C2T_SHAPE_CHANGE_FIRSTATTACKER,						// 改变第一个攻击自己的人
	MSG_C2T_SHAPE_LEAVEFIGHT,								// 脱离战斗
	MSG_C2T_SHAPE_SLIPPOS,									// 滑动位移
	MSG_C2T_SHAPE_ATK_ADDBLOOD = 783906,					// 加血
	MSG_C2T_SHAPE_GETTSELECTEDSHAPE,						// 获得当前选择的目标


	// ----------------------------------------------------
	MSG_C2T_PLAYER					= MSG_C2T + TMAX_SUB * 3,			// 玩家信息(对应MSG_S2C_PLAYER = 784128)
	// ----------------------------------------------------
	MSG_C2T_PLAYER_RESUME,									// 恢复HP、MP、YP。。。
	MSG_C2T_PLAYER_ANSWER_RELIVE = 784131,					// 恢复复活
	MSG_C2T_PLAYER_ADDEXP,									// 增加经验值
	MSG_C2T_PLAYER_LEVELUP,									// 升级
	MSG_C2T_PLAYER_ADDOCCUEXP,								// 增加职业经验值
	MSG_C2T_PLAYER_OCCULEVELUP,								// 职业等级升级
	MSG_C2T_PLAYER_ADDSP,									// 增加sp
	MSG_C2T_PLAYER_CREDIT_UPDATE,							// 声望更新
	MSG_C2T_PLAYER_SETLEVEL = 784138,						// 设置玩家等级
	MSG_C2T_PLAYER_USEITEM,									// gameserver向客户端返回消耗道具的使用结果
	MSG_C2T_PLAYER_CHANGEMAXATK,							// 消耗性道具提升最大攻击力
	MSG_C2T_PLAYER_CHANGEATKSPEED,							// 消耗性道具提升攻击速度
	MSG_C2T_PLAYER_CHANGEDEF,								// 消耗性道具提升防守能力
	MSG_C2T_PLAYER_SYNC_PKCOUNT = 784144,					// 同步杀人数
	MSG_C2T_PLAYER_TRADEREQUEST,							// GS转发交易请求给客户
	MSG_C2T_PLAYER_TRADEBEGIN,								// 交易开始
	MSG_C2T_PLAYER_TRADELOCK,
	MSG_C2T_PLAYER_WAITFORTRADE = 784154,					// 玩家进入交易等待状态的返回消息
	MSG_C2T_PLAYER_TRADECANCEL = 784156,					// 服务器通知玩家对方已经取消交易
	MSG_C2T_PLAYER_TRADERESULT,								// 交易的最终结果
	MSG_C2T_PLAYER_ADDSKILL = 784159,						// 添加技能
	MSG_C2T_PLAYER_DELSKILL,								// 删除技能
	MSG_C2T_PLAYER_CHANGE_EQUIPMENT = 784162,
	MSG_C2T_PLAYER_CHANGE_PROPERTIES,						// 玩家改变属性
	MSG_C2T_PLAYER_UpdateDisplayHeadPiece,					// 更新玩家是否显示头盔信息
	MSG_C2T_PLAYER_AUTOMOVE,								// 自动移动到指定的点
	MSG_C2T_PLAYER_CHANGE_CONTRIBUTE,						// 国家贡献改变
	MSG_C2T_PLAYER_FORCE_TARGET,							// 设置强制攻击目标(一般处理中了挑衅技能的玩家)

	MSG_C2T_PLAYER_COLLECTION_LEVELUP,						// 玩家采集升级
	MSG_C2T_PLAYER_FACTURE_LEVELUP,							// 玩家制作升级
	MSG_C2T_PLAYER_RELIVE_RESULT,							// 复活失败
	MSG_C2T_PLAYER_BUYRELIVEITEM_RESULT,					// 购买复活道具结果
	MSG_C2T_PLAYER_SKILL_RELIVE,							// 技能复活
	MSG_C2T_PLAYER_UPDATE_TITLE_TYPE,						// 玩家更新称号
	MSG_C2T_PLAYER_COLLECTION_ADDEXP,						// 玩家采集经验增加
	MSG_C2T_PLAYER_FACTURE_ADDEXP,							// 玩家制作经验增加


	// ----------------------------------------------------
	MSG_C2T_OTHER					= MSG_C2T + TMAX_SUB * 4,			// 其它信息(对应MSG_S2C_OTHER = 784384)
	// ----------------------------------------------------
	MSG_C2T_OTHER_TALK,										// 对话消息
	MSG_C2T_OTHER_FACTIONTALK,								// 行会聊天
	MSG_C2T_OTHER_BROADCAST,								// 广播
	MSG_C2T_OTHER_AddTopInfo,								// 添加顶端提示信息
	MSG_C2T_OTHER_OPENTALKBOX,								// 让客户端弹出TalkBox
	MSG_C2T_OTHER_OPENPIXYTALKBOX,							// 让客户端弹出PixyTalkBox
	MSG_C2T_OTHER_ADDINFO,									// 在玩家对话框加入一条信息
	MSG_C2T_OTHER_ADDINFO_BYINDEX,							// 在玩家对话框加入一条信息(按照索引添加)
	MSG_C2T_OTHER_ADDSYSTEMINFO,							// 在屏幕上方加入一条信息
	MSG_C2T_OTHER_GAMEMESSAGE,								// 弹出消息对话框
	MSG_C2T_OTHER_SENDOBJECTPROPERTIY = 784396,				// 接受Shape的所有属性
	MSG_C2T_OTHER_SCRIPTTIMERRUN = 784400,					// 脚本计时器运行
	MSG_C2T_OTHER_ANSWERNPCLIST = 784405,					// 返回NPC列表给客户端
	MSG_C2T_OTHER_OPENSTORYTALKBOX = 784415,				// 让客户端弹出StoryTalkBox
	MSG_C2T_OTHER_EXPATC = 784417,							// 表情运作	
	MSG_C2T_OTHER_GoodsInfo_Re,								// 物品信息回复
	MSG_C2T_OTHER_PLAY_VIDEO = 784420,						// 播放视频
	MSG_C2T_OTHER_STOP_VIDEO,								// 关闭视频
	MSG_C2T_OTHER_REPEATLOGIN = 784423,						// 账号重复登录
	MSG_C2T_OTHER_UPDATEVARVALUEOPEN,						// 打开修改脚本变量值的窗口


	// ----------------------------------------------------
	MSG_C2T_GOODS					= MSG_C2T + TMAX_SUB * 5,			// 物品信息(对应MSG_S2C_GOODS = 784640)
	// ----------------------------------------------------
	MSG_C2T_PICKGOOD = 784645,								// gameserver返回拣物品结果
	MSG_C2T_DROPMONEY,										// 丢钱						
	MSG_C2T_PICKMONEY,										// 捡钱
	MSG_C2T_SETHOTKEY,										// gameserver返回设置HOTKEY结果
	MSG_C2T_CLEARHOTKEY,						
	MSG_C2T_EXCHANGEHOTKEY,						
	MSG_C2T_EQUIPDAMAGE,                                    // 装备坏掉
	MSG_C2T_EQUIPDURUPDATE,									// 更新装备的耐久
	MSG_C2T_EQUIP_REFINE_UPDATE,							// 炼度更新
	MSG_C2T_DELETEEQUIP,									// 删除装备
	MSG_C2T_DELETEGOODS = 784656,							// 删除背包中指定位置指定数量的物品							
	MSG_C2T_DELETEALLGOODS,									// 删除背包中指定位置的全部物品	
	MSG_C2T_OPENGOODSUPGRADE = 784659,						// gameserver通知玩家打开物品升级界面
	MSG_C2T_OPENGOODSENCHASE,								// 打开卡片镶嵌功能页面
	MSG_C2T_ENCHASE_OPEN_HOLE,								// 开孔消息
	MSG_C2T_ENCHASE_CLOSE_HOLE,								// 堵孔消息
	MSG_C2T_ENCHASE_ENCHASECARD,							// 镶卡消息
	MSG_C2T_ENCHASE_REMOVECARD,								// 拆卡消息
	MSG_C2T_ENCHASE_ARRANGE,								// 打开小功能页面消息
	MSG_C2T_GOODSUPGRADE_UPGRADE = 784670,					// gameserver返回升级一个物品的结果
	MSG_C2T_GOODS_UPDATEPROPERTY,							// 更新物品属性

	MSG_C2T_SET_YUANBAO = 784674,							// 设置星钻
	MSG_C2T_SET_TICKET,										// 设置点券

	//! GoodsExManage应用
	////////////////////////////////////////////////////////////////////////
	MSG_C2T_GOODS_OPEN_WINDOWS,								//！打开物品操作窗口（指定窗口的类型）
	MSG_C2T_GOODS_CLOSE_WINDOWS,							//！关闭所有物品操作窗口
	//! 分解物品
	MSG_C2T_GOODS_RE_DISASSEMBLE_PREVIEW,					//! 返回预览结果
	MSG_C2T_GOODS_RE_DISASSEMBLE_DO,						//! 返回分解物品成败
	//! 合成物品
	MSG_C2T_GOODS_RE_SYNTHESIZE_PREVIEW,					//! 返回预览结果
	MSG_C2T_GOODS_RE_SYNTHESIZE_DO,							//! 返回合成成败
	//! 武器充能
	MSG_C2T_GOODS_RE_REINFORCE_POWER,						//! 补充武器能量
	MSG_C2T_GOODS_POWER_WARN,								//! 武器能量剩余通知
	//! 绑定冻结
	MSG_C2T_GOODS_BIND,										//! 物品绑定（解绑）通知
	MSG_C2T_GOODS_FROST,									//! 物品冻结（解冻）通知

	//! 物品升级
	MSG_C2T_GOODS_RE_UPGRADE_VIEW,							//! 回复预览请求
	MSG_C2T_GOODS_RE_UPGRADE_DO,							//! 回复执行升级
	//! 物品魔化
	MSG_C2T_GOODS_RE_STUFF_SYNTHESIZE,						//! 回复材料合成
	MSG_C2T_GOODS_RE_PLUG_EVIL_VIEW,						//! 回复魔化预览请求
	MSG_C2T_GOODS_RE_PLUG_EVIL_DO,							//! 回复执行升级
	MSG_C2T_GOODS_RE_EVIL_BACKOUT_VIEW,						//! 回复拆解预览请求
	MSG_C2T_GOODS_RE_EVIL_BACKOUT,							//! 魔化装备拆除

	//! 仓库
	MSG_C2T_DEPOT_OPEN,										//!	通知客户端打开仓库
	MSG_C2T_DEPOT_RE_INPUT_PWD,								//! 回复客户端密码输入
	MSG_C2T_DEPOT_RE_CLOSE,									//! 回复关闭仓库，也作为关闭通知
	MSG_C2T_DEPOT_RE_CHANGE_PWD,							//! 回复修改密码
	MSG_C2T_DEPOT_RE_BUY_SUBPACK,							//! 购买子背包
	MSG_C2T_DEPOT_FROSTED,									//! 通知仓库被冻结

	//! 勋章
	MSG_C2T_MEDAL_ADD_NOTIFY,								//! 添加勋章通知
	MSG_C2T_MEDAL_DEL_NOTIFY,								//! 删除勋章通知

	//! 非副职业制作
	MSG_C2T_MAKE_BEGIN,										//! 开始一个制作
	MSG_C2T_MAKE_RESULT,									//! 发送一个制作结果


	// ----------------------------------------------------
	MSG_C2T_SHOP					= MSG_C2T + TMAX_SUB * 6,			// 商店信息(对应MSG_S2C_SHOP = 784896)
	// ----------------------------------------------------
	MSG_C2T_SHOP_OPEN,										// 打开商店
	MSG_C2T_SHOP_BUY_BAT = 784899,							// gameserver返回从商店中买物品的结果
	MSG_C2T_SHOP_SELL,										// 用于确定卖出物品
	MSG_C2T_SHOP_SELL_BAT,									// gameserver返回卖物品到商店的结果
	MSG_C2T_SHOP_GOODSCHANGEGOODS,							// 以物易物类商店的消息
	MSG_C2T_SHOP_REPAIR,									// gameserver返回在商店中修理一个物品的结果
	MSG_C2T_SHOP_REPAIRALL,
	MSG_C2T_SHOP_REPAIRALLCOST,								// 服务器返回修理背包中全部物品的花费
	MSG_C2T_BUSINESS_OPEN = 784908,
	MSG_C2T_BUSINESS_BUY,
	MSG_C2T_BUSINESS_SELL,
	MSG_C2T_INVEST_OPEN,
	MSG_C2T_SHOP_BUYBACK = 784913,
	MSG_C2T_BUSINESS_UPDATEPRICE,							// 商业界面价格更新


	// ----------------------------------------------------
	MSG_C2T_DEPOT					= MSG_C2T + TMAX_SUB * 7,
	// ----------------------------------------------------

	// ----------------------------------------------------
	MSG_C2T_GM						= MSG_C2T + TMAX_SUB * 8,
	// ----------------------------------------------------

	// ----------------------------------------------------
	MSG_C2T_TEAM					= MSG_C2T + TMAX_SUB * 9,			// 队伍信息(对应MSG_S2C_TEAM = 785664)
	// ----------------------------------------------------
	MSG_C2T_TEAM_TeamCreate = 785666,						//! 数据格式同MSG_S2C_TEAM_TeamData，但只在建立时发送
	MSG_C2T_TEAM_TeamData,									//! 所有队伍数据到成员
	MSG_C2T_TEAM_MemberData,								//! 一个成员的队伍数据
	MSG_C2T_TEAM_TeamDataChange,							//! 队伍数据改变

	MSG_C2T_TEAM_MemberAllIdioinfo,							//! 一个成员所有非队伍成员数据
	MSG_C2T_TEAM_UpdateIdioinfo,							//! 更新某个非队伍成员数据
	MSG_C2T_TEAM_MembereExStateChange,						//! 成员异常状态改变
	MSG_C2T_TEAM_MemberStateChange,							//! 成员在线状态改变

	MSG_C2T_TEAM_MemberLeave,								//! 成员离队
	MSG_C2T_TEAM_MemberLeaveHere,							//! 成员离开本服(GS)
	MSG_C2T_TEAM_MastterChange,								//! 队长改变

	MSG_C2T_TEAM_Disband,									//! 队伍解散
	MSG_C2T_TEAM_GoodsSetChange,							//! 修改物品分配设置

	MSG_C2T_TEAM_MemberPos,									//! 更新成员位置
	MSG_C2T_TEAM_RECRUIT,									//! 更新招募状态
	MSG_C2T_TEAM_RECRUITED_NUM,								//! 更新已招募的人数
	//! [add end]

	//! old used
	MSG_C2T_TEAM_QUERY_JOIN,								// 邀请加入
	MSG_C2T_TEAM_CHAT,										// 队聊

	//! old unused
	MSG_C2T_TEAM_ANSWER_JOIN,								//回复邀请加入
	MSG_C2T_TEAM_SYNCHRONIZE,								//同步队伍的信息
	MSG_C2T_TEAM_TEAMATE_JOIN,
	MSG_C2T_TEAM_TEAMATE_LEAVE,
	MSG_C2T_TEAM_TEAMATE_CHANGE_REGION,
	MSG_C2T_TEAM_CHANGE_LEAER,									//改变队长
	MSG_C2T_TEAM_CHANGE_ALLOCATION_SCHEME,

	MSG_C2T_TEAM_CHANGE_STATE,


	// -----------------------------------------------------
	MSG_C2T_SKILL					= MSG_C2T + TMAX_SUB * 10,			// 技能信息(对应MSG_S2C_SKILL = 785920)
	// -----------------------------------------------------
	MSG_C2T_SKILL_USE,										// 使用技能
	MSG_C2T_SKILL_STATE_CHANGE,								// 使用技能后导致HP/MP/RP/YP状态改变
	MSG_C2T_SKILL_ADD_EX_STATE,								// 使用技能后添加异常状态
	MSG_C2T_SKILL_END_EX_STATE,								// 异常状态的消失,服务器通知客户端
	MSG_C2T_SKILL_UPDATE_EX_STATE,							// 更新状态时间
	MSG_C2T_SKILL_EX_STATE_ADDITIONAL_DATA,					// 异常状态的附加数据
	MSG_C2T_SKILL_AFFECT_FIELD,								// 发送受影响的区域给客户端
	MSG_C2T_SKILL_STUDY_FAILED,								// 学习技能失败
	MSG_C2T_SKILL_STUDY_BEGIN,								// 客户端返回打开技能学习导师页面
	MSG_C2T_OCCUSKILL_STUDY_BEGIN,							// 收到打开副职业技能学习页面消息
	MSG_C2T_OCCUSKILL_STUDY,								// 收到学习副职业技能消息
	MSG_C2T_OCCUSKILL_DELETE,								// 向客户端返回删除制作条目消息
	MSG_C2T_OCCUSKILL_NPCFACTURE_BEGIN,						// 向客户端发送NPC制作列表
	MSG_C2T_OCCUSKILL_NPCFACTURE,							// 发送NPC制作消息
	MSG_C2T_SKILL_UNLOCK_GOODS,								// 发送解除物品锁定消息
	MSG_C2T_SKILL_LOCK_GOODS,								// 锁定物品消息

	MSG_C2T_SKILL_ADD_PET,
	MSG_C2T_SKILL_DEL_PET,
	MSG_C2T_SKILL_USE_PET,
	MSG_C2T_SKILL_END_PET,


	// -----------------------------------------------------
	MSG_C2T_ORGSYS					= MSG_C2T + TMAX_SUB * 11,			// 家族信息(对应MSG_S2C_ORGSYS = 786176)
	// -----------------------------------------------------

	// 行会协议-------------------------------------------------
	MSG_C2T_ORGSYS_InitOrgSys_Notify,					//! 初始化组织系统通知
	MSG_C2T_ORGSYS_UpdatePlayerFacInfo,					//! 更新一个玩家的工会信息

	//! 会长操作
	MSG_C2T_ORGSYS_CreateFaction_Notify,				//! 创建行会通知(创建检测完毕，检测客户端应回需一个新行会名称)
	MSG_C2T_ORGSYS_CreateFaction_Re,					//! 创建行会回应
	MSG_C2T_ORGSYS_UpgradeFaction_Notify,				//! 升级行会通知
	MSG_C2T_ORGSYS_UpgradeFaction_Re,					//! 升级行会回复
	MSG_C2T_ORGSYS_DisbandFaction_Notify,				//! 解散行会通知
	MSG_C2T_ORGSYS_DisbandFaction_Re,					//! 解散行会回应	
	MSG_C2T_ORGSYS_SetPlacard_Re,						//! 修改公告回应
	MSG_C2T_ORGSYS_SetLeaveword_Re,						//! 修改备忘回应
	MSG_C2T_ORGSYS_SetJobName_Re,						//! 修改官阶名字回应
	MSG_C2T_ORGSYS_SetJobPurview_Re,					//! 修改官阶权限回应
	MSG_C2T_ORGSYS_SetJobPurview_Notify,				//! 修改官阶权限回应
	MSG_C2T_ORGSYS_SetMemberJob_Re,						//! 设置成员官阶回应
	MSG_C2T_ORGSYS_SetMemberJob_Notify,					//! 设置成员官阶通知（新加）
	MSG_C2T_Faction_SetMemberTitle_Re,					//! 设置成员称号回应
	MSG_C2T_Faction_SetMemberTitle_Notify,				//! 设置成员称号通知（新加）
	MSG_C2T_Faction_SetChairman_Re,						//! 传位回复
	MSG_C2T_Faction_SetChairman_Notify,					//! 传位通知
	MSG_C2T_Faction_ChangeRecruitState_Notify,			//! 招募状态改变通知

	//! 成员操作
	MSG_C2T_ORGSYS_Quit_Re,								//! 退出行会回应
	MSG_C2T_ORGSYS_Quit_Notify,							//! 退出行会通知
	MSG_C2T_ORGSYS_Kick_Re,								//! 踢出行会回应
	MSG_C2T_ORGSYS_Kick_Notify,							//! 踢出行会通知
	MSG_C2T_ORGSYS_CheckRequisition_Re,					//! 审批入会回应
	MSG_C2T_ORGSYS_CheckRequisition_Notify,				//! 审批入会名单操作通知（新加）
	MSG_C2T_Faction_InvitePlayer_Re,					//! 邀请玩家加入回应
	MSG_C2T_Faction_InvitePlayer_Notify,				//! 邀请玩家加入通知
	MSG_C2T_Faction_AddMember_Notify,					//! 添加成员通知
	MSG_C2T_Faction_ApplyBoardOpen_Notify,				//! 打开申请入会面板通知

	MSG_C2T_ORGSYS_JoinFac_Re,							//! 加入请求回复

	//! 数据请求
	MSG_C2T_Faction_PublicData_Notify,					//! 行会资料（tagFacPublicData结构）
	MSG_C2T_Faction_BaseData_Notify,					//! 行会资料（tagFacBaseData结构）
	MSG_C2T_Faction_Member_Notify,						//! 成员和申请者
	MSG_C2T_Faction_Apply_Notify,						//! 成员和申请者
	MSG_C2T_Faction_Placard_Notify,						//! 公告资料回应
	MSG_C2T_Faction_Leaveword_Notify,					//! 备忘资料回应


	MSG_C2T_Faction_IconBoardOpen_Notify,				//! 打开图标上传面板通知
	MSG_C2T_Faction_IconData_Re,						//! 上传一个工会图标数据回复
	MSG_C2T_Faction_IconData_Notify,					//! 一个工会的图标数据更新通知
	MSG_C2T_Faction_AllIconData_Re,						//! 发送所有工会图标信息						

	MSG_C2T_Faction_MemberMap_Notifye,					//! 更新成员地图
	MSG_C2T_Faction_MemberLevel_Notifye,				//! 更新成员等级
	MSG_C2T_Faction_MemberOccu_Notifye,					//! 更新成员职业
	MSG_C2T_Faction_MemberOnline_Notifye,				//! 更新成员在线状态

	MSG_C2T_Faction_ChangeRes_Notify,					//! 资源值变更
	MSG_C2T_Faction_ChangeExp_Notify,					//! 经验值变更
	MSG_C2T_Faction_ChangeMemberContribute_Notify,		//! 成员贡献变更
	// End：行会------------------------------------------------



	// 联盟协议-------------------------------------------------
	MSG_C2T_UNION_Script_Creation,						//! 创建联盟脚本请求通知，打开联盟创建界面
	MSG_C2T_UNION_Re_Creation,							//! 创建联盟回复
	MSG_C2T_UNION_Notify_Creation,						//! 创建联盟通知
	MSG_C2T_UNION_Notify_Disband,						//! 解散联盟通知所有盟友
	MSG_C2T_UNION_Notify_Quit,							//! 行会退出联盟通知所有盟友
	MSG_C2T_UNION_Notify_Kick,							//! 盟主将行会踢出联盟通知
	MSG_C2T_UNION_Re_Kick,								//! 盟主将行会踢出联盟通知
	MSG_C2T_UNION_Notify_SetChairman,					//! 盟主转让通知所有盟友
	MSG_C2T_UNION_Re_SetRights,							//! 盟主赋予会长权限回复
	MSG_C2T_UNION_Notify_GetRights,						//! 会长一个人获得盟主赋予权限的通知
	MSG_C2T_UNION_Response_Invite,						//! 盟主 收到 行会会长 回复的邀请结果
	MSG_C2T_UNION_Notify_Anwser_Invite,					//! 行会会长 收到 盟主邀请加入联盟的请求
	MSG_C2T_UNION_Notify_Join,							//! 行会加入联盟通知所有盟友
	MSG_C2T_UNION_Notify_BaseData,						//! 一个联盟的基本数据通知
	MSG_C2T_UNION_Notify_MemberData,					//! 一个联盟的一个成员数据添加或更新的通知
	MSG_C2T_UNION_Notify_AllMemberData,					//! 一个联盟的所有成员数据通知
	// End：联盟------------------------------------------------




	MSG_C2T_Quest_Add,										//添加一个任务
	MSG_C2T_Quest_Complete,									//完成一个任务
	MSG_C2T_Quest_Remove,									//移出一个任务								
	MSG_C2T_Quest_UpdateStep,								//更新一个步骤
	MSG_C2T_Quest_UpdateParamData,							//更新一个任务的所有变量
	MSG_C2T_Quest_UpdateData,								//更新一个任务的变量
	MSG_C2T_Quest_UpdateRecordData,							//更新添加一个任务记录
	MSG_C2T_Quest_Talk,
	MSG_C2T_MercQuest_UpdateData,							//给客户端返回俑兵任务数据并同时打开界面
	MSG_C2T_MercQuest_OpenInterface,						//打开俑兵任务界面
	
	MSG_C2T_ANSWER_PlayerRanks,								//向客户端发送玩家排行榜
	//赏金任务
	MSG_C2T_SHARE_QUESTLIST_UPDATE,							//更新共享任务列表
	MSG_C2T_SHARE_QUEST_OPEN,								//打开队伍正在进行的任务界面
	MSG_C2T_SHARE_CURRQUEST,								//发送当前正在进行的任务ID


	// -----------------------------------------------------
	MSG_C2T_PLAYERSHOP				= MSG_C2T + TMAX_SUB * 12,			// 玩家商店信息(对应MSG_S2C_PLAYERSHOP = 786432)
	// -----------------------------------------------------
	MSG_C2T_PALYERSHOP_CREATE,								// 创建玩家商店
	MSG_C2T_PLAYERSHOP_BUYITEM,
	MSG_C2T_PLAYERSHOP_ADDSUCCEED,
	MSG_C2T_PLAYERSHOP_OPENFORBUSINESS,
	MSG_C2T_PLAYERSHOP_CLOSEDOWN,
	MSG_C2T_PLAYERSHOP_LOOKGOODS,
	MSG_C2T_PLAYERSHOP_CLOSE,
	MSG_C2T_PLAYERSHOP_QUIT,
	MSG_C2T_PLAYERSHOP_TOBUYER,
	MSG_C2T_PLAYERSHOP_TOSELLER,
	MSG_C2T_PLAYERSHOP_UPDATE,
	MSG_C2T_PLAYERSHOP_REMOVE,



	// ----------------------------------------------------
	MSG_C2T_CONTAINER				= MSG_C2T + TMAX_SUB * 13,			// 容器信息
	// ----------------------------------------------------
	MSG_C2T_CONTAINER_OBJECT_MOVE,
	MSG_C2T_CONTAINER_OBJECT_UNLOCK,
	MSG_C2T_CONTAINER_OBJECT_AMOUNT_CHANGE,
	//MSG_C2T_CONTAINER_CLEAR,


	// ----------------------------------------------------
	MSG_C2T_PET						= MSG_C2T + TMAX_SUB * 14,			// 宠物信息
	// ----------------------------------------------------
	MSG_C2T_PET_MOVE,										// 移动
	MSG_C2T_PET_STOP,										// 停止
	MSG_C2T_PET_CHANGE_MODE,								// 改变宠物当前模式
	MSG_C2T_PET_CHANGE_ACTION,								// 改变宠物当前动作
	MSG_C2T_PET_CHANGE_SPEED,								// 改变速度
	MSG_C2T_PET_CHANGE_MASTER,								// 改变主人
	MSG_C2T_PET_CHANGE_LEVEL,								// 改变等级



	// -----------------------------------------------------
	MSG_C2T_COST					= MSG_C2T + TMAX_SUB * 15,			//计费
	// -----------------------------------------------------
	MSG_C2T_COST_NOTENOUGHCOST,

	//寄售点卡 s->c
	// -----------------------------------------------------
	MSG_C2T_CARD					= MSG_C2T + TMAX_SUB * 16, 
	// -----------------------------------------------------
	

	//国家
	// -----------------------------------------------------
	MSG_C2T_COUNTRY					= MSG_C2T + TMAX_SUB * 17, 
	// -----------------------------------------------------
	MSG_C2T_COUNTRY_CHANGE,									// 更改国籍



	// ----------------------------------------------------
	MSG_C2T_INCREMENTSHOP			= MSG_C2T + TMAX_SUB * 18,			// 增值商店
	// ----------------------------------------------------
	MSG_C2T_INCREMENTSHOP_OPEN,								// 打开商店			
	MSG_C2T_INCREMENTSHOP_BUY,								// gameserver返回从商店中购买一个物品的结果
	MSG_C2T_INCREMENTSHOP_CLOSE,							// ##关闭个人商店
	MSG_C2T_INCREMENTSHOP_AFFICHE_RESPONSE,					// 公告窗口开

	MSG_C2T_INCREMENTSHOP_GETLOG_RESPONSE,					// 返回增值日志


	// ----------------------------------------------------
	MSG_C2T_LINKMAN					= MSG_C2T + TMAX_SUB * 19,			// 好友系统
	// ----------------------------------------------------
	MSG_C2T_LINKMAN_INIT,										//! 初始化联系人列表
	MSG_C2T_LINKMAN_ADD,										// S 通知 A 添加联系人 B 的结果
	MSG_C2T_LINKMAN_DEL,										// S 通知 A 删除联系人 B 的结果
	MSG_C2T_LINKMAN_MOVE,										// S 通知 A 移动联系人 B 的结果

	MSG_C2T_LINKMAN_OTHER_HANDLE,								// S 通知 B： A 对 B 的 添加、删除、移动 等操作结果

	MSG_C2T_LINKMAN_REQUEST_ADD,								// S 通知 B： A 对 B 的添加申请

	MSG_C2T_LINKMAN_LOGIN,										// 联系人上线
	MSG_C2T_LINKMAN_LOGOUT,										// 联系人下线

	MSG_C2T_LINKMAN_INFO_CHANGE,								// 信息改变：地图、等级、职业
	MSG_C2T_LINKMAN_DELSTATE_CHANGE,							// 删除状态改变  

	MSG_C2T_LINKMAN_LEAVE_WORD,									//! 发送留言到客户端
	MSG_C2T_LEAVEWORD_SAVE_RESULT,								//! 留言储存结果


	// ----------------------------------------------------
	MSG_C2T_SPRITE					= MSG_C2T + TMAX_SUB * 20,			//! 小精灵
	// ----------------------------------------------------
	MSG_C2T_SPRITE_AutoSupply_OnOff_Re,							//! 自动补给系统：回复功能开关
	MSG_C2T_SPRITE_AutoSupply_ChangeSetup_Re,					//! 自动补给系统：回复修改一个补给设置
	MSG_C2T_SPRITE_AutoSupply_UseItem_Notify,					//! 自动补给系统：通知使用了一个药品
	MSG_C2T_SaveNewSetupData_Re,								//! 玩家保存新的游戏设置回复



	// ----------------------------------------------------
	MSG_C2T_MAIL					= MSG_C2T + TMAX_SUB * 29,			//! 邮件系统
	// ----------------------------------------------------
	MSG_C2T_MAIL_RECEIVER,										//!获得邮件数据
	MSG_C2T_MAIL_SEND_FAILED,									//!发信失败
	MSG_C2T_MAIL_SEND,											//!新邮件发送
	MSG_C2T_MAIL_READ,											//!读取邮件
	MSG_C2T_MAIL_FAILED_MOVE_GOODS,								//!邮件取物品失败
	MSG_C2T_MAIL_MOVE_GOODS_RESULT,								//!邮件取物品结果
	MSG_C2T_MAIL_DELETE_RESULT,									//!删除有附件邮件的确认消息
	MSG_C2T_MAIL_DELETE_OK,										//!删除邮件
	MSG_C2T_MAIL_AGREED_PAY,									//!付款收信接收消息
	MSG_C2T_MAIL_AGREED_PRESENT,								//!同意赠送
	MSG_C2T_MAIL_GET_GOLD_RESULT,								//!邮箱取金钱结果
	MSG_C2T_MAIL_PRESENT_FAILED,								//赠送失败
	MSG_C2T_MAIL_PRESENT_OK,									//附合赠送条件
	MSG_C2T_MAIL_SYSMAILMSG,									//系统邮件发送消息

	// Fox@20081107----------------------------------------------
	// 玩家管理系统( PlayerManager )
	MSG_C2T_PM						= MSG_C2T + TMAX_SUB * 30,
	// -----------------------------------------------------
	MSG_C2T_PM_ACK,							// GS应答查询玩家信息


	MSG_C2T_LOGIC					= MSG_C2T + TMAX_SUB * 40,
	MSG_C2T_LOGIC_UIISOPENED,				// 指定的界面是否开启






	//////////////////////
	//				    //
	//	 TestToClient	//
	//				    //
	//////////////////////

	//========================================================================================================
	MSG_T2C							= TMAX_TYPE * 38,			// Test -> Client
	//========================================================================================================
	MSG_T2C_MOUSE					= MSG_T2C + TMAX_SUB * 0,	// 鼠标消息

	MSG_T2C_MOUSE_MOVE,											// 鼠标移动
	MSG_T2C_MOUSE_LBUTTONDOWN,									// 左键按下
	MSG_T2C_MOUSE_LBUTTONUP,									// 左键释放
	MSG_T2C_MOUSE_LBUTTONDBLCLK,								// 左键双击

	MSG_T2C_MOUSE_RBUTTONDOWN,									// 右键按下
	MSG_T2C_MOUSE_RBUTTONUP,									// 右键释放
	MSG_T2C_MOUSE_RBUTTONDBLCLK,								// 右键双击
	
	MSG_T2C_MOUSE_MOUSEWHEEL,									// 鼠标中间滚动

	// -----------------------------------------------------
	MSG_T2C_KEY						= MSG_T2C + TMAX_SUB * 1,	// 键盘消息
	// -----------------------------------------------------
	MSG_T2C_KEY_DOWN,											// 键盘按下
	MSG_T2C_KEY_UP,												// 键盘释放
	MSG_T2C_KEY_CHAR,											// 键盘操作

	// -----------------------------------------------------
	MSG_T2C_LOGIC					= MSG_T2C + TMAX_SUB * 2,	// 逻辑处理消息
	// -----------------------------------------------------
	MSG_T2C_LOGIC_MOVE,											// 移动到某个坐标点
	MSG_T2C_LOGIC_SETPOS,										// 瞬移到某个坐标点

	MSG_T2C_LOGIC_SELECTEDSHAPE,								// 选择某个对象
	MSG_T2C_LOGIC_UIISOPENED,									// 请求客户端返回一个指定界面是否开启的消息
	
	MSG_T2C_LOGIC_PICKGOODS,									// 请求客户端拾取物品

	MSG_T2C_LOGIC_CALLSCREAPT,									// 请求客户端向服务器发送脚本调用功能
	MSG_T2C_LOGIC_SPLITGOODS,									// 请求客户端向服务器发送拆分物品消息
};