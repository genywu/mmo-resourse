//======================================================================================
/**
*	@file		GoodsSetup.h
*	@data		
*	@author		
*/
//======================================================================================

#pragma once

#include "../Common/BaseDef.h"
#include "../Common/GoodsDef.h"

/// 菜单编号
/// 主要为资源模块 (含部分操作模块), 包括WS独占和GS共用两部分.
/// 为与菜单操作统一起见, 二者使用同一组枚举 (且顺序相同).
enum MENU_INDEX
{
    // 下拉菜单                    加载(_L)
    LOAD_SETUP = MY_MENU * 2,      // 配置
    LOAD_LOGSYS,                   // 日志
    LOAD_TEXTRES,                  // 文本资源
    LOAD_BROADCAST,                // 系统广播
    LOAD_GLOBALSETUP,              // 全局参数
    LOAD_GAMESETUP,                // 游戏配置
    LOAD_GMLIST,                   // GM列表
    LOAD_SPEECH,                   // 言论过滤
    LOAD_PLAYER,                   // 玩家
    LOAD_GOODSLIST,                // 原始物品列表
	LOAD_GOODSEX,                  // 物品扩展配置
	LOAD_RELIVEGOODS,              // 复活道具
	LOAD_INCREMENTSHOP,            // 增值商店
    LOAD_NPCSHOP,                  // NPC商店
    LOAD_SKILL,                    // 技能
	LOAD_HITLEVEL,                 // 连击等级
    LOAD_QUEST,                    // 任务
    LOAD_MONSTER,                  // 怪物
    LOAD_MONSTERAI,                // 怪物AI
	LOAD_SCRIPTAI,                 // 脚本AI
    LOAD_PET,                      // 宠物
	LOAD_CONTACTS,		           // 好友
	LOAD_SUIT,                     // 套装
	LOAD_SCRIPT,                   // 一个或多个脚本
    LOAD_SCRIPTS,                  // 所有脚本
	LOAD_GENVARS,                  // 通用变量
    LOAD_REGIONLIST,               // 场景列表
    LOAD_REGIONSETUP,              // 场景设置
    LOAD_SYSMAIL,                  // 系统邮件
    LOAD_GENIUSSYS,                // 天赋

    LOAD_FACTION = MY_MENU * 3,    // 帮会
	LOAD_GATES,                    // 城门
	LOAD_COUNTRY,                  // 国家
    LOAD_DOCCULIST,                // 副职业列表
	LOAD_GOODSTAILOR,			   // 制作配方
    LOAD_PLAYERMGR,                // 玩家管理
    LOAD_ANTICHEAT,                // 反外挂
    LOAD_PHASE,                    // 相位
    LOAD_CONTRIBUTE,               // 贡献
    LOAD_AHCONF,                   // 仙贝寄售
    LOAD_REPUTATION,               // 声望
	LOAD_PROFESSION,               // 专业技能
    LOAD_INSTANCE,                 // 副本
	LOAD_DEPOT,                    // 仓库
	LOAD_EMOTE,                    // 动作表情
	LOAD_ENCHASE,                  // 强化
	LOAD_BUSINESS,                 // 商业
    LOAD_FBUSINESS,                // 家族跑商
	LOAD_SPRITE,                   // 精灵

	SYNC_INCLIMIT = MY_MENU * 4,   // 增值商店购买限制
    SYNC_REGION,                   // 单个场景
    SYNC_REGIONS,                  // 所有场景
	SYNC_BRANCHRGN,			       // 分线场景数据
	SYNC_INSTANCESELF,			   // 个人房屋副本数据
	SYNC_INSTANCEDATA,	           // 副本模板
    SYNC_SCRIPT,                   // 脚本
	SYNC_VARIABLES,                // 变量
    SYNC_GSINFO,                   // GS信息
	SYNC_WSINFO,                   // WS信息
    SYNC_END,

    OPER_CONNECTLS = MY_MENU * 5,  // 连接LoginServer
    OPER_CONNECTDBS,               // 连接DBServer
    OPER_CONNECTLOGS,              // 连接LogServer
    OPER_COLLECTPLAYERDATA,        // 收集GS数据
    OPER_SAVENOW,                  // 立刻存当前数据
    OPER_SAVEDATA,                 // 保存数据
    OPER_KICKALL,                  // 踢除所有玩家
    OPER_SHOWSAVEINFO,             // 显示存盘消息开关
};

/**
*	@class GlobalSetup
*
*	@brief 全局配置类
*/
class GlobalSetup
{
public:
	/// 构造
	GlobalSetup(void);

	/// 析构
	~GlobalSetup(void);

public:
	/// 配置结构定义
	struct tagSetup
	{
		short	btMaxCharactersNum;				// 角色最大数
		// 属性换算
		float	fStr2MaxAtk[OCC_Max];
		float	fStr2Burden[OCC_Max];
		float	fDex2MinAtk[OCC_Max];
		float	fDex2Stiff[OCC_Max];
		float	fCon2MaxHp[OCC_Max];
		float	fCon2Def[OCC_Max];
		float	fInt2Element[OCC_Max];
		float	fInt2MaxMp[OCC_Max];
		float	fInt2Resistant[OCC_Max];

		// 战斗公式
		long	lBaseHit;						// 基本命中
		float	fBaseAtcHitcMagnify;			// 命中鉴定放大系数
		float	fHitLvRate;						// 命中的等级差影响
		long	lMinHitMonster;					// 最低命中保障(怪物)
		long	lMaxHitMonster;					// 最高命中限制(怪物)
		long	lMinHit[OCC_Max];				// 最低命中保障
		long	lMaxHit[OCC_Max];				// 最高命中限制
		float	fCriticalRate;					// 致命一击的伤害系数
		float	fLossExp_Normal;				// 损失经验值比例(普通区域)
		float	fLossExp_Game;					// 损失经验值比例(竞技区域)
		float	fLossExp_War;					// 损失经验值比例(城战区域)

		// 掉物品几率(diedLosts[A][B][C]
        // A为装备等枚举ID，B为战场状态（普通、城战），C为PK状态（蓝、黄、红、灰）
		float diedLosts[EC_TOTAL][2][4]; 

		//物品保值比
		float	fBasePriceRate;

		//物品折价率
		FLOAT	fTradeInRate;

		//物品修理价格系数
		float	fRepairFactor;

		long	lGoodsDurFray;
		long	lGoodsDurSpecialFray;

		
		long	lHeadWaste;
		long	lBodyWaste;
		long	lGloveWaste;
		long	lBootWaste;

		long	lBackWaste;
		long	lDecorateWaste;
		long	lHeadgearWaste;
		long	lTalismanWaste;
		long	lFrockWaste;
		long	lMedalWaste;
		long	lWingWaste;
		long	lManteauWaste;
		long	lFairyWaste;
		long	lNeckWaste;
		long	lRingWaste;
		long	lShieldWaste;		

		//物品升级阶段系数和成功几率
		WORD	wGoodsBlessPhase[3];
		float	fGoodsUpgradeOdds[4];

		// 其他
		DWORD	dwGoodsDisappearTimer;			// 物品消失时间
		DWORD	dwGoodsProtectedTimer;			// 物品保护时间
		long	lResumeTimer;					// HP/MP恢复时间间隔
		long	lResumeFactor;					// 恢复系数
		float	fResumeFactorMini;				// 保底恢复系数
		
		// 怪物回复时间间隔
		long	lMonsterResumeTimer;

		long	lFightStateTimer;				// 战斗状态持续时间
		long	lSinStateTimer;					// 犯罪状态持续时间
		DWORD	dwAttackMonsterProtectedTimer;	// 攻击怪物的保护时间
		DWORD	dwMonsterKeepDeadTime;			//怪物尸体保持时间
		DWORD	dwCanCollectMonsterKeepDeadTime;//可采集怪物尸体保持时间
		DWORD   dwBeneficiaryTime;				//攻击获益保护时间
		DWORD   dwCollectionKeepDeadTime;		//采集物保持死亡状态时间
		DWORD	dwMonsterTransitionTime;		//怪物保持发呆状态时间
		DWORD	dwMonsterKindlyTime;			//怪物刚出生时的友好时间，不主动索敌
		DWORD	dwReturnPeaceEffectTime;		//恢复和平状态时间
		float	fReturnPeaceSpeed;				//恢复和平时候的回跑速度
		DWORD	dwReturnPeaceStandTime;			//恢复和平时候的站立时间
		DWORD	dwFightStateTime;				//（玩家）战斗状态保持时间
		DWORD	dwDHFQueueSize;					//对于DHF(Distance and Hurted First)怪物维护的目标队列最大长度
		// 怒值和连击
		WORD	wBaseRp_Lv1;					// 第一段怒值出现等级
		WORD	wBaseRp_Lv2;					// 第一段怒值出现等级
		WORD	wBaseMaxRp_Lv1;					// 第一段怒值上限
		WORD	wBaseMaxRp_Lv2;					// 第二段怒值上限
		WORD	wBaseAtcRp_Sword;				// 每次攻击增加的怒值
		float	fRpAdd_Lv1[6];					// 被攻击增加怒值的伤血比例
		WORD	wRpAdd_Lv1[6];					// 被攻击增加怒值的值

		long	lHitBaseLv;						// (连击等级差)			5
		long	lHitTime;						// (连击时间-毫秒)			10000
		float	fHitExpPrize;					// (连击经验值奖励系数)		0.1
		float	fMaxHitExpPrize;				// 经验奖励系数最大值		1.0
		DWORD	dwHitTopLogNum[12];				// 最大连击记录
		DWORD	dwHitTopLogPrizeExp[12];		// 达到最大连击记录的奖励经验值

		// 升级基本属性修正
		long	lHpLiveUp[OCC_Max];				//	最大Hp升级修正
		long	lMpLiveUp[OCC_Max];				//	最大Mp升级修正
		long	lBurdenLiveUp[OCC_Max];			//	最大负重升级修正

		float	fExpScale;						// 获取经验值得比例
		float	fExpScriptScale;				// 脚本获取经验值得比例
		float	fOccuExpScale;					// 获取职业经验比例
		float	fOccuExpScriptScale;			// 脚本获取职业经验比例
		float	fSpScale;						// 获取技能点数的比例
		float	fSpScriptScale;					// 脚本获取技能点数的比例
		float	fMonsterDropGoodsScale;			// 怪物爆物品的比例

		//赠送经验相关
		DWORD	dwPresentExpCooldown;			// 赠送经验冷却时间(分钟)
		float	fPresentExpScaleInPersonalHouse;// 个人房屋副本中赠送经验系数
		float	fPresentExpScaleInSafeArea;		// 安全区赠送经验系数
		float	fPresentExpScale;				// 其他赠送经验系数
		long	lPresentExpBuffId;				// 显示赠送经验的buffid
		long	lPresentExpBuffLvl;				// 显示赠送经验的bufflevel


		//##硬直系统
		WORD	wNumStiffSetup;					// 多少级
		FLOAT	fStiff_Damage[4];				// 伤害
		WORD	wStiff_Probability[4];			// 概率
		DWORD	dwStiffDelayTime;				// 每次硬直的延迟时间
		DWORD	dwStiffBoundTime;				// 单位时间长度
		DWORD	dwStiffLimit;					// 每单位时间内最多允许的硬直次数

		//##杀人数减免时间
		DWORD	dwCriminalTime;
		DWORD	dwMurdererTime;					//废弃
		DWORD	dwRipperTime;					//废弃
		DWORD	dwPkCountPerKill;				// 杀一个增加的PK值
		DWORD   dwPkCountPerAttack;				//攻击一次增加的PK值
		DWORD	dwOnePkCountTime;				// 消除一个PK值的时间
		DWORD	dwActTime;						//施放动作时间
		DWORD   dwHpMpRestoreTime;				//自动恢复HpMp的间隔时间
		long	lPostage[MAX_MAIL];				//邮资
		long	lMaxName;						//名字字数限制
		long	lMaxSubject;					//主题最大字数
		long    lMaxText;						//邮件内容最大字数
		long    lMaxExText;						//邮件附加文本的最大字数
		long	lMaxMoney;						//最大金钱
		float	fMaxRemainTime[MAX_MAIL];		//邮件最大保存时间
		long	lPrisonIslandPkValue;			//流亡岛的PK值
		long	lMailDelayTime;					//邮件发送延时时间
		long	lBaseModuleTime;				//技能状态超时检测时间
		long    lTotalMaxSkillNum;				//总技能越界个数
		float   fOneSecMaxSkillNum;				//每秒技能越界个数

		long    lTotalMaxNpcScriptsNum;			//总NPC脚本越界个数
		float   fOneSecMaxNpcScriptsNum;		//每秒NPC脚本越界个数

		//金钱索引
		DWORD   dwGoldGoodsId;
		//银币索引
		DWORD   dwSilverGoodsId;

		//位面精华索引
		DWORD   dwWeimianGoodsId;

		DWORD   dwNpcSellGoldToSilver;

		//##金钱上限
		DWORD	dwGoldCoinLimit;
		//银币上限
		DWORD   dwSilverLimit;
		//点券上限
		DWORD   dwTicketLimit;

		//##道具加血允许的时间间隔。
		DWORD	dwHealTimeInterval;

		//##道具加MP允许的时间间隔。
		DWORD	dwMpTimeInterval;

		//拾取物品时间间隔
		DWORD   dwPickGoodsInterval;

		float	fMonsterNumScale;				// 出生怪物数量比例
		bool	bAllowClientRunScript;			// 是否允许client执行脚本
		bool	bAllowClientChangePos;			// 是否允许client执行瞬移

		//! 显示配偶名字的步骤值
		long	lShowSpouseNameStep;

		//系统存盘点间隔
		DWORD	dwSavePointTime;

		long	lAreaWidth;						// 区域大小格子宽
		long	lAreaHeight;					// 区域大小格子高

		DWORD   dwDelDays;						// n天后物理删除
		char	strSpeStr[16];					// 允许更名名字所必须含有的字符串
		long	lRegionChatLevelLimit;			// 场景喊话的等级限制

		DWORD   lOfferExpPercent;				// 帮会贡献者贡献经验的比例
		float	fDecTimeParam;					// 争夺状态受攻击减少时间系数
		long	lDiedStateTime;					// 死亡状态时间S


		//##PK伤害比例
		FLOAT			m_fPvPDamageFactor;

		//##新人等级限制
		DWORD			m_btNewbieLevelLimit;
		DWORD			m_btNewSoldierLevel;

		// 新兵保护等级


		//武器强度修正
		FLOAT			m_fWeaponPowerModifier;

		//武器强度最小修正值。
		FLOAT			m_fWeaponMinPowerModifier;

		// 寄售时间手
		DWORD			m_dwCard6Hours;
		DWORD			m_dwCard12Hours;
		DWORD			m_dwCard24Hours;

		// 寄售时间手续费
		DWORD			m_dwCard6HoursChargePrice;
		DWORD			m_dwCard12HoursChargePrice;
		DWORD			m_dwCard24HoursChargePrice;

		// 寄售金额上下限
		DWORD			m_dwCardLowerLimitPrice;
		DWORD			m_dwCardUpperLimitPrice;
		

		// 帐号下的最大寄售数量
		DWORD			m_dwCardMaxNum;
		// 最大寄售数量 
		DWORD			m_dwCardWorldMaxNum;

		// 寄售尝试次数
		DWORD			m_dwCardRetryNum;
		// 拒绝寄售时间
		DWORD			m_dwCardDenyTime;

		float	fBlastAttackScale;				// 物攻爆击放大系数
		float	fBlastDefendScale;				// 物防爆击衰弱系数
		float	fElementBlastAttackScale;		// 法术爆击放大系数
		float	fElementBlastDefendScale;		// 法术爆击衰弱系数
		float	fElementFullMissScale;			// 法术躲闪衰弱系数
		
		char	szTalkWorldGoodsName[32];		// 世界喊话物品原始名
		int		nTalkWorldGoodsNum;				// 世界喊话物品数量
		long	lTalkWorldGold;					// 世界喊话金币数量
		long	lTalkWorldSilver;				// 世界喊话银币数量

		char	szTalkUnionGoodsName[32];		// 阵营喊话物品原始名
		int		nTalkUnionGoodsNum;				// 阵营喊话物品数量
		long	lTalkUnionGold;					// 阵营喊话金钱数量
		long	lTalkUnionSilver;				// 阵营喊话银币数量

		char	szTalkCountryGoodsName[32];		// 国家喊话物品原始名
		int		nTalkCountryGoodsNum;			// 国家喊话物品数量
		long	lTalkCountryGold;				// 国家喊话金钱数量
		long	lTalkCountrySilver;				// 国家喊话银币数量

		char	szTalkTopWorldGoodsName[32];	// 置顶世界喊话物品原始名
		int		nTalkTopWorldGoodsNum;			// 置顶世界喊话物品数量
		long	lTalkTopWorldGold;				// 置顶世界喊话金币数量
		long	lTalkTopWorldSilver;			// 置顶世界喊话银币数量

		char	szReliveGoodsName[32];			//原地复活原始名
		long	lRelivePrice;					//原地复活价格

		float	fPlayerWalkSpeed;				// 玩家走速度<毫秒/每格子>
		float	fPlayerRunSpeed;				// 玩家跑速度<毫秒/每格子>

		float   fRidePlayerWalkSpeed;			// 坐骑下走速度
		float   fRidePlayerRunSpeed;			// 坐骑下跑速度

		//聚魂珠获取经验公式：角色最终获取经验值×（n1/100－角色自身等级/n2），n1暂定=10，n2暂定＝1428，n1和n2都可以在全局参数表中配置。
		long	lExpBallFactor1;				// 聚灵珠获取经验参数1
		long	lExpBallFactor2;				// 聚灵珠获取经验参数2

		DWORD	dwAutoProtectTime;	
		DWORD	dwIncrementLogDays;
		long	lMaxHelpHintNum;				//最大帮助提示个数

		//玩家登陆时候触发的脚本
		char	pszPlayerLoginScript[MAX_PATH];
		//玩家死亡时触发的脚本
		char	pszPlayerDiedScript[MAX_PATH];
		//玩家升级时候触发的脚本
		char	pszPlayerUpLevelScript[MAX_PATH];
		//转职时触发脚本
		char	pszPlayerChangeOccuScript[MAX_PATH];
		//玩家PVP杀人时触发脚本
		char	pszPVPKilled[MAX_PATH];
		//玩家PVP被杀时触发脚本
		char	pszPVPBeKilled[MAX_PATH];
		//玩家PK值为负时触发的脚本
		char	pszConValueNegative[MAX_PATH];
		//玩家PK值为非负时触发的脚本
		char	pszConValueNotNegative[MAX_PATH];
		 //玩家复活
		char	szReliveScriptName[MAX_PATH];
		//日程表
		char	pszCalendar[MAX_PATH];
		//迦纳维魔盒
		char	pszMagicBox[MAX_PATH];
		//问答与奖励
		char	pszAnswerRequest[MAX_PATH];
		//切换分线
		char	pszChangeLineRgn[MAX_PATH];

		//! 队伍设置
		long	lCorrelativeSkillID;			//与组队关联的技能ID
		long	lMinSkillLevel;					//可以组队的最小技能等级
		long	lMinTradeLevel;					//可以交易最小等级
		long	new_lGoodsAllot;				//默认分配方式
		long	new_lMaxMemberNum;				//默认最大人数
		long	new_lSessionTimeoutS;			//会话超时秒数
		float	new_fIncreasedExp_2;			//2人经验加成比例
		float	new_fIncreasedExp_3;			//3人经验加成比例
		float	new_fIncreasedExp_4;			//4人经验加成比例
		float	new_fIncreasedExp_5;			//5人经验加成比例
		float	new_fIncreasedExp_6;			//6人经验加成比例
		long	new_lExperienceRadii;			//经验分配半径（单位：格）
		long	new_lExpDiff;					//经验差
		long	new_lExpLimit;					//玩家经验值份额基本保障
		float	new_fExpAmerce;					//经验惩罚系数
		float	new_fExpHortation;				//经验奖励系数
		float	new_fExpMinLvlDiff;				//经验奖励开始的最小等级差
		
		float	vigour_fIncreasedExp_2;			//2人元气加成比例
		float	vigour_fIncreasedExp_3;			//3人元气加成比例
		float	vigour_fIncreasedExp_4;			//4人元气加成比例
		float	vigour_fIncreasedExp_5;			//5人元气加成比例
		float	vigour_fIncreasedExp_6;			//6人元气加成比例
		long	vigour_lExpDiff;				//元气差
		long	vigour_lExpLimit;				//元气经验值份额基本保障
		float	vigour_fExpAmerce;				//元气惩罚系数
		float	vigour_fExpHortation;			//元气奖励系数
		float	vigour_fExpMinLvlDiff;			//元气奖励开始的最小等级差
		float	vigour_fOccExpSpRatio;			//职业经验和SP比值

		//公共冷却时间
		long	lCommoCoolDownTime;
		//pk/pvp比例值设定
		float	fPkToPvpValue;
		//因PK/PVP触发得脚本
		char	pszPKToPvPScript[MAX_PATH];

		//限制发消息的最大玩家数量
		short	lSendMsgLimitPlayerNum;
		//限制发消息时的最大距离
		short	lSendMsgLimitDistance;
		//限制发技能消息时最大玩家数量
		short	lSendSkillMsgLimitPlayerNum;
		//更新属性到周围的延迟时间
		long	lUpdateOtherPropertyDelayTime;
		//发送AddShape消息时的间隔时间
		long	lSendAddShapeMsgInteTime;
		//立即发送AddShape消息的距离限制
		long	lSendAddSahpeMsgDistance;
		//验证移动消息的最大个数
		long	lCheckMsgCount;
		//最大允许每秒发送移动消息的个数
		float	fMaxMsgCountPerSecond;
		//验证移动时候平均移动的速度允许的最大误差
		float	fMaxMistakeAverSpeed;
		//验证移动速度的时候最大速度的误差
		float	fMaxMistakeMaxSpeed;
		//Ping机制检测时间(分钟)
		long	lPingChekTime;
		//客户端Ping错误封号时间(分钟)
		long	lPingErrorBanTime;
		//死亡复活倒计时(分钟)
		long	lReLiveTime;
		// 报警时间间隔（秒）
		long	lWarningDeltaTime;
		// 玩家限制操作赠送间隔时间(毫秒)
		long	lOperBounsTime;
       
        
        ///  杂念值下降时间
        long    lDistractTime;
        /// 每次下降点数
        long    lDistractValue;
        /// 使用技能增加杂念数
        long    lAddDistractValue;
        /// 连续使用技能时间
        long    lInterUseSkillTime;
        /// 连续使用技能增加杂念值
        long    lAddInterDistractValue;
        /// 燥的最大存在数量
        long    lDryCount;
        /// 燥状态增加杂念值
        long    lDryValue;
        /// 燥存在最大时间
        long    lDryTime;

		
		//! 游戏对象分配策略设置
		struct ObjAllotPolicy
		{
			long	ObjNum;			//每预分配一块内存，包含的对象数量
			long	blockNum;	//初始分配内存是，预分配的块数
			ObjAllotPolicy():ObjNum(0),blockNum(0){}
		};
		ObjAllotPolicy Goods_Policy;			//!物品
		ObjAllotPolicy Monster_Policy;			//!怪物
		ObjAllotPolicy Collection_Policy;		//!采集物
		ObjAllotPolicy Npc_Policy;				//!NPC
		ObjAllotPolicy Player_Policy;			//!玩家
		ObjAllotPolicy Region_Policy;			//!区域
		ObjAllotPolicy SummonedCreature_Policy;	//!召唤怪
		ObjAllotPolicy Build_Policy;			//!建筑
		ObjAllotPolicy Citygate_Policy;			//!城门
		
		long	lTimerVarNum;					//预分配定时器参数的数量
		long    lPreAllocedScriptNum;			// 预分配的脚本内存块数量
		long    lMaxPreAllocedScriptNum;		// 预分配的脚本内存块最大数量
		long    lDupRgnQueueNum;				// 创建副本排队数
		long    lPDupRgnMaxNum;					// 创建个人副本最大数
		long    lPHDupRgnMaxNum;				// 创建个人房屋副本最大数
		long    lTDupRgnMaxNum;					// 创建组队副本最大数
		long    lDupRgnQueueDeltaTime;			// 创建副本排队时间间隔
		long    lDupRgnQueueMaxNum;				// 创建副本排队最大数
		long	lPDupRgnDelTime;				// 个人副本删除时间
		long	lCreatePDupRgnMaxNum;			//玩家创建个人副本最大数
		long	lCreateTDupRgnMaxNum;			//玩家创建组队副本最大数
		long	lCreateDupRgnDeltaTime;			//玩家创建副本时间间隔
		long    lSaveTime;						// 玩家保存数据的时间间隔
		long	lWSSaveTime;					// WS保存数据的时间间隔

		long	lMaxGoodInfoNum;				//最大[聊天中物品信息]保存数量

		long	lReliveDebuffId;				// 玩家复活DebuffID
		long	lReliveDebuffLevel;				// 玩家复活DebuffLevel

		long	lCreateRgnPlayerLoadMax;		//GS创建副本玩家数负载上限
		long	lCreateRgnMonsterLoadMax;		//GS创建副本怪物数负载上限
		long    lPDupRgnMaxPlayerNum;			// 个人副本最大人数
		long    lPHDupRgnMaxPlayerNum;			// 个人房屋副本最大人数
		long    lTDupRgnMaxPlayerNum;			// 组队副本最大人数
		long	lRgnKickPlayerDeltaTime;		// 副本清人时间
		long    lMLevelEnablePlayerSearch;		// 根据等级开启玩家玩家功能

		char	szHideName[32];					// 玩家隐匿名
		ULONG	uUpgradeTo10FontColor;			//! 武器成功到达10以上，公告显示的字体颜色
		ULONG	uUpgradeTo20BackIdx;			//! 武器成功到达10以上，走马灯显示的背景索引
		ULONG	uUpgrade10LostFontColor;		//! 10以上武器升级失败，公告显示的字体颜色
	};

    /// 移动判定/同步配置
    struct MoveCheckConf
    {
        int reqMax; ///< 请求（消息）数量，大于此值检测
        float disPerStep; ///< 服务器端每次移动的距离
        float syncMaxDis; ///< 同步时的最大距离
        float errMaxDis;
        float errMinDis;
        float reqMaxDis;
    };

private:
	/// 全局配置结构
	static tagSetup	m_stSetup;

    static MoveCheckConf m_moveConf;
public:
	/// 从文件加载全局配置
	static bool Load(const char* filename);

	/// 加载Setup/gamesetup.ini配置
	static bool LoadGameSetup(const char* filename);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 取得全局配置表
	static tagSetup* GetSetup()	{return &m_stSetup;}

	/// 获取最大怒值上限
	static WORD GetBaseMaxRp(uchar lOccupation, uchar lLevel);

    static bool LoadMoveConf( stringstream &stream );

    static const MoveCheckConf &MoveConf()  { return m_moveConf; }

    /// 载入佛之特色系统
    static bool LoadBuddhism( stringstream& stream );
};
