#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SKILL_RELATED_INCLUDED
#define _INC_SKILL_RELATED_INCLUDED

//#define  _DEBUG_SKILL
//#define  _SKILL_DEBUG


enum eSkillTimerType
{
	SKILL_INTONATE,			//吟唱定时
	SKILL_FIRE,			    //施放定时
};

enum tagAction
{
	ACTION_WALK,
	ACTION_RUN,
	ACTION_STAND,
	ACTION_FIGHT,
	ACTION_DEFENSE,
	ACTION_CAST_SPELL,
	ACTION_TALK,
	ACTION_USE_ITEM,
};

//##ModelId=4088B5B3009B
enum tagSkillType
{
	//##ModelId=4088B5B300AA
	SKILL_TYPE_ATTACK,
	//##ModelId=4088B5B300BA
	SKILL_TYPE_DEFENSE,
	//##ModelId=4088B5B300CA
	SKILL_TYPE_STATE,
	//##ModelId=4088B5B300D9
	SKILL_TYPE_SUMMON,
	//##ModelId=4088B5B300DA
	SKILL_TYPE_UNKNOW							= 0x7fffffff,
};


enum tagStateID
{
	STATE_RESTORE_HP							= 100000,	// 物品恢复HP
	STATE_RESTORE_MP							= 100001,	// 物品恢复MP
	STATE_AUTOMATIC_RESTORE_HP					= 100002,	// 自动恢复HP
	STATE_AUTOMATIC_RESTORE_MP					= 100003,	// 自动恢复MP
	STATE_RIDE_HORSE							= 100004,	// 骑马的状态
	STATE_PARTICULAR_STATE						= 100005,	// 特殊状态
	STATE_TEAM									= 100006,	// 组队招募状态
	STATE_USER_GOODS_ENLARGE_MAX_HP				= 100007,	// 使用道具增加HP上限
	STATE_USER_GOODS_ENLARGE_MAX_MP				= 100008,	// 使用道具增加MP上限
	STATE_IMPROVE_EXP							= 100009,	// 使用道具增加EXP倍数
	STATE_USER_GOODS_ENLARGE_DEF				= 100010,	// 使用道具增加防
	STATE_USER_GOODS_ENLARGE_ELM_DEF			= 100011,	// 使用道具增加魔防
	STATE_USER_GOODS_ENLARGE_FULL_MISS			= 100012,	// 使用道具增加全闪避
	STATE_AUTO_PROTECT							= 110000,	// 进入场景时，自动释放，进入保护状态/
};

enum tagSkillStep
{
	SKILL_OBJECT,					// 判断目标对象（0）
	SKILL_FORCE_OBJECT,				// 判断强制目标对象
	FIRS_TATT,						// 判断是否可以对非pvp角色发动技能
	SKILL_STAR_BUFF,				// 判断要施放技能所需的buff
	SKILL_STAR_STATE,				// 判断施放技能时玩家必须处于何种游戏状态
	SKILL_STAR_EQUIP_ID,			// 设置要施放技能所需装备的id
	SKILL_STAR_EQUIP_PLACE,			// 设置要施放技能所需装备需放在何处
	SKILL_STAR_EQUIP_USE,			// 判断要施放技能必须的装备条件
	SKILL_ITEM_ID,					// 设置施放技能所需道具id
	SKILL_ITEM_NUM,					// 设置施放技能所需道具需要的数量
	SKILL_ITEM_PLACE,				// 设置施放技能所需道具需放在何处
	SKILL_USE_ITEM,					// 判断施放技能所需道具条件
	SKILL_HP,						// 判断施放技能所需hp
	SKILL_MP,						// 判断施放技能所需mp
	SKILL_ENERGY,					// 判断施放技能所需能量
	INTONATE_LEAST_SPACE,			// 判断吟唱最小施放距离
	INTONATE_MOST_SPACE,			// 判断吟唱最大施放距离
	FIRE_LEAST_SPACE,				// 判断施放最小施放距离
	FIRE_MOST_SPACE,				// 判断施放最大施放距离
	INTONATE_ACT,					// 设置吟唱动作ID
	INTONATE_EFFECT,				// 设置吟唱动作特效ID
	INTONATE_EFFECT_INDEX,			// 设置吟唱特效索引
	INTONATE_EFFECT_BY_EQUIP,		// 吟唱特效是否与武器相关
	INTONATE_EFFECT_POSITION,		// 设置吟唱特效绑定位置
	INTONATE_EFFECT_CYCLE,			// 设置吟唱特效是否循环
	INTONATE_PLAY,					// 开始吟唱
	INTONATE_SOUND,					// 设置吟唱声效id
	INTONATE_SOUND_BY_EQUIP,		// 吟唱音效是否与武器相关
	INTONATE_SOUND_CYCLE,			// 设置吟唱声效是否循环
	INTONATE_SOUND_PLAY,			// 开始播放吟唱声效
	INTONATE_BREAK,					// 设置是否可以被打断
	RELEASE_ACT,					// 设置施放动作ID
	RELEASE_EFFECT,					// 设置施放动作特效id
	RELEASE_EFFECT_INDEX,			// 设置动作特效索引
	RELEASE_EFECT_BY_EQUIP,			// 施放特效是否与武器相关
	RELEASE_EFFECT_DEST,			// 设置施放特效目标
	RELEASE_EFFECT_POSITION,		// 设置施放特效绑定位置
	RELEASE_EFFECT_CYCLE,			// 设置施放特效是否循环
	RELEASE_PLAY,					// 开始施放动作
	RELEASE_SOUND,					// 设置施放声效id
	RELEASE_SOUND_BY_EQUIP,			// 施放音效是否与武器相关
	RELEASE_SOUND_CYCLE,			// 设置施放声效是否循环
	RELEASE_SOUND_DEST,				// 设置施放声效目标
	RELEASE_SOUND_PLAY,				// 播放施放声效
	SHOCK_RATIO,					// 震动频率(百分比)
	SHOCK_INTERVAL,					// 震动间隔时
	SHOCK_TIME_LENGTH,				// 震动时间长度
	SHOCK_PLAY,						// 开始震动
	FLY_EFFECT,						// 设置飞行特效id
	FLY_EFFECT_INDEX,				// 设置飞行特效索引
	FLY_EFFECT_BY_EQUIP,			// 飞行特效是否与武器相关
	FLY_EFFECT_POSITION,			// 设置飞行特效从何处发出
	FLY_EFFECT_CYCLE,				// 设置飞行特效是否循环播放
	FLY_EFFECT_TURNX,				// 设置飞行特效X轴方向偏移角度
	FLY_EFFECT_TURNY,				// 设置飞行特效Y轴方向偏移角度
	FLY_PLAY,						// 开始飞行动作
	FLY_SOUND,						// 设置飞行音效id
	FLY_SOUND_BY_EQUIP,				// 飞行音效是否与武器相关
	FLY_SOUND_POSITION,				// 设置飞行音效从何处释放
	FLY_SOUND_CYCLE,				// 设置飞行音效是否循环
	FLY_SOUND_PLAY,					// 开始播放飞行音效
	FLY_TIME,						// 设置飞行时间长度
	FLY_DELAY,						// 设置飞行延迟时间
	DEL_LOC_EFFECT,					// 设定删除特效
	DEL_LOC_EFFECT_INDEX,			// 设定删除特效索引
	START_DEL_EFFECT,				// 开始删除特效
	SUMMON_EFFECT,					// 设置召唤技能特效
	SUMMON_SOUND,					// 设置召唤声效
	SUMMON_LIFETIME,				// 设置召唤生命周期
	RANDOM_SUMMONX,					// 设置随机召唤技能X轴范围
	RANDOM_SUMMONY,					// 设置随机召唤技能Y轴范围
	RANDOM_SUMMON_NUM,				// 设置随机召唤技能召唤物数量
	INTONATE_TIME,					// 设置吟唱的时间(72)
	FLY_EFFECT_NUM,					// 飞行特效的个数(73)
	INTONATE_MAINACT,				// 吟唱阶段的主要动作，一般在吟唱动作结束后循环播放(74)
	SUMMON_DELAYTIMA,				// 召唤特效的延迟播放时间（75）
	WEAPONEFFECT,					// 武器特效播放标识（76）
	CHANGEPROPERTY,					// 改变技能释放者的某个属性（77）
	ACTTIME,						// 当前动作的规定时间（78）
	RELEASE_ACTEX,					// 释放阶段的后续动作（79）
	DELAY_CYCELEFFECT_TIME,			// 循环特效删除的延迟时间（80）
	LAST_ACTEX_ISCYCEL,				// 最后一个释放阶段的后继动作是否循环播放（81）
	HITEFFECT = 98,					// 命中特效
	HITSOUND = 99,						// 命中音效
	FLASHEFFECT = 100,					// 闪光效果
	FLASHEEFFECTLUM = 101,				// 闪光亮度
	FLASHEEFFECTTIME = 102,				// 闪光持续时间
	SHAKEEFFECT = 103,				//震动效果
	SHAKEEFFECTEXTENT = 104,		//震动幅度值
	SHAKEEFFECTTIME = 105,			//震动效果时间
};
enum tagSkillID
{
	//#########################################################################
	STATE_BASE_WEAKNESS							= 6322,//虚弱状态
	STATE_BASE_GOD								= 7777,//无敌状态
	STATE_BASE_AC_ENTER							= 6217,//刚进入游戏的反外挂状态
	STATE_BASE_SUBSPEED							= 6322,//减速状态
	SKILL_BASE_BUFF								= 10000,// 缓冲释放技能
	SKILL_BASE_ATTACK							= 10001,// 空手基本攻击
	SKILL_BASE_SINGLEHANDED_GLAIVE				= 10002,// 单手剑基本攻击
	SKILL_BASE_SINGLEHANDED_HAMMER				= 10003,// 单手锤基本攻击
	SKILL_BASE_DAGGER							= 10004,// 匕首基本攻击
	SKILL_BASE_MAGIC_CRUTCH						= 10005,// 法杖基本攻击(双手)
	SKILL_BASE_BOTHHANDS_GLAIVE					= 10006,// 双手剑基本攻击(双手,一把剑)
	SKILL_BASE_BOTHHANDS_AX						= 10007,// 双手斧基本攻击(双手)
	SKILL_BASE_BOW								= 10008,// 弓基本攻击
	SKILL_BASE_BOTH_GLAIVES						= 10009,// 双剑基本攻击(双手,两把剑)
	SKILL_BASE_BLADEFIST						= 10010,// 拳刃基本攻击(双手)
	// 职业技能
	SKILL_BASE_NOVICE_OCC						= 10099,// 初心者职业技能
	SKILL_BASE_FIGHTER_OCC						= 10100,// 战士职业技能
	SKILL_BASE_RABBI_OCC						= 10200,// 法师职业技能
	SKILL_BASE_FAQUIR_OCC						= 10300,// 行者职业技能
	SKILL_BASE_KNIGHT_OCC						= 10400,// 骑士职业技能
	SKILL_BASE_WARRIOR_OCC						= 10500,// 斗士职业技能
	SKILL_BASE_SCOUT_OCC						= 10600,// 斥候职业技能

	SKILL_BASE_CRAFTSMAN_OCC					= 10700,// 艺人职业技能
	SKILL_BASE_WARLOCK_OCC						= 10800,// 术士职业技能
	SKILL_BASE_CLERIC_OCC						= 10900,// 牧师职业技能
	SKILL_BASE_PALADIN_OCC						= 11000,// 圣骑士职业技能
	SKILL_BASE_DARKKINGHT_OCC					= 11100,// 暗骑士职业技能

	SKILL_BASE_GLADIATOR_OCC					= 11200,// 角斗士职业技能
	SKILL_BASE_BERSERKER_OCC					= 11300,// 狂战士职业技能
	SKILL_BASE_RANGER_OCC						= 11400,// 巡守职业技能
	SKILL_BASE_ASSASSIN_OCC						= 11500,// 刺客职业技能
	SKILL_BASE_BLADEDANCER_OCC					= 11600,// 剑舞者职业技能


	SKILL_BASE_BARD_OCC							= 11700,// 吟游诗人职业技能
	SKILL_BASE_SORCERER_OCC						= 11800,// 巫师职业技能
	SKILL_BASE_NECROMANCER_OCC					= 11900,// 亡灵巫师职业技能
	SKILL_BASE_BISHOP_OCC						= 12000,// 主教职业技能
	SKILL_BASE_PROPHET_OCC						= 12100,// 贤者(先知)职业技能

	SKILL_RIDE_START							= 12201,// 坐骑技能
	SKILL_RIDE_END								= 12301,// 坐骑技能
	

	SKILL_DEPUTY_OCCU							= 30000,// 副职业技能标示
	//SKILL_OCCU_COLLECT_SMITHING					= 50001,// 副职业锻造采集技能
	//SKILL_OCCU_COLLECT_METALLURGY				= 50002,// 副职业炼金采集技能
	//SKILL_OCCU_COLLECT_SEWING					= 50003,// 副职业缝纫采集技能
	//SKILL_OCCU_COLLECT_OTHER					= 50004,// 副职业其他类型采集技能
	//
	//SKILL_OCCU_FACTURE							= 60000,
	//SKILL_OCCU_FACTURE_SMITHING					= 60001,// 副职业锻造制作技能
	//SKILL_OCCU_FACTURE_METALLURGY				= 60002,// 副职业炼金制作技能
	//SKILL_OCCU_FACTURE_SEWING					= 60003,// 副职业缝纫制作技能

	/*SKILL_BASE_ARCHERY							= 1002,
	SKILL_BASE_MAGIC							= 1003,
	SKILL_BASE_STAB								= 1004,
	SKILL_MONSTER_ATTACK						= 1005,
	SKILL_BASE_SUMMON							= 1006,
	SKILL_BASE_RSUMMON							= 1007,
	SKILL_BASE_DEFENCE							= 1008,
	SKILL_BASE_FLASH							= 1009,
	SKILL_BASE_RAINBOW							= 1010,*/


	//贯穿刺击
	//SKILL_BASE_DEFENSE							= 10,

	////#####################
	//SKILL_EX_STATE								= 50,

	////#########################################################################
	//SKILL_MOSOU									= 101,		// 无双斩
	//SKILL_GHOST_CUT								= 102,		// 鬼斩
	//SKILL_KNIGHT_CUT							= 103,		// 神武霸斩
	//SKILL_ARMY_BREAK							= 104,		// 破军
	//SKILL_FLASH									= 105,		// 一闪
	//SKILL_SWALLOW								= 106,		// 燕返
	//SKILL_LEAF_CUT								= 107,		// 页切合
	//SKILL_JU_CUT								= 108,		// 居合
	//SKILL_RAGE									= 109,		// 怒发冲冠
	//SKILL_RAGE_BREAK							= 110,		// 怒爆发
	//SKILL_SWORDSHIP								= 111,		// 基本剑术
	//SKILL_LIGHTNING_SWORD						= 112,		// 雷霆剑
	//SKILL_LITTLE_FLASH							= 113,		// 越切
	//SKILL_THUNDER_SLASH							= 114,		// 狂雷剑
	//SKILL_RUSH									= 115,		// 冲刺
	//SKILL_PILLAR								= 116,		// 中流砥柱
	//SKILL_CALLOSITY								= 117,		// 无情
	//SKILL_BLIND									= 118,		// 擒目

	////传2复制技能
	//SKILL_LIGHTNING_SWORD_2						= 119,		// 雷剑2
	//SKILL_LIGHTNING_SWORD_3						= 120,		// 雷剑3

	//SKILL_GHOST_CUT_2							= 121,		// 鬼斩2
	//SKILL_GHOST_CUT_3							= 122,		// 鬼斩2

	//SKILL_ARMY_BREAK_2							= 123,		// 破军2

	//SKILL_RUSH_2								= 124,		// 冲刺2

	//SKILL_CALLOSITY_2							= 125,		// 无情2
	//SKILL_LIGHTNING_SWORD_4						= 126,		// 雷剑4
	//SKILL_LITTLE_FLASH_2						= 127,		// 越切2
	//SKILL_LEAF_CUT_2							= 128,		// 页切合2

	//SKILL_AGILITY_2								= 129,		// 身手敏捷2


	////#########################################################################

	//SKILL_POISON_FOG							= 201,		// 迷魂烟
	//SKILL_HEARTLESS_ARROW						= 202,		// 绝情箭
	//SKILL_LIGHTING_ARROW						= 203,		// 流光
	//SKILL_METEOR_ARROW_MASS						= 204,		// 流星箭（射）
	//SKILL_METEOR_ARROW							= 205,		// 流星箭（落）
	//SKILL_RAIN_ARROW							= 206,		// 虹
	//SKILL_POISON_MOTH							= 207,		// 毒蛾
	//SKILL_BLOOD_ROSE							= 208,		// 血色玫瑰
	//SKILL_SCORPION								= 209,		// 三尾蝎刺
	//SKILL_BOA_LOCK								= 210,		// 缠足蟒
	//SKILL_HEAL									= 211,		// 医疗
	//SKILL_MONSTER_TAMING						= 212,		// 驯兽
	//SKILL_FALLING_STAR							= 213,		// 星坠
	//SKILL_EXPLOSIVE_ARROW						= 214,		// 爆炸矢
	//SKILL_PETS_CONTROL							= 215,		// 宠物攻击
	//SKILL_GIBE									= 216,		// 宠物的嘲讽
	//SKILL_SUPER_HEAL							= 217,		// 大治疗

	//SKILL_AGILITY								= 218,		// 身手敏捷
	//SKILL_RAPTURE								= 219,		// 全神贯注
	//SKILL_NATURAL								= 220,		// 自然之神
	//SKILL_STRIKE								= 221,		// 冲击

	//SKILL_MACHINE_SHIELD						= 222,		// 机关护甲

	//SKILL_DAUB_POISON							= 223,		// 涂毒

	////传2复制技能
	//SKILL_SWORDSHIP_2							= 224,		// 基本剑术2
	//SKILL_EXPLOSIVE_ARROW_2						= 225,		// 爆炸矢2
	//SKILL_EXPLOSIVE_ARROW_3						= 226,		// 爆炸矢3
	//SKILL_HEAL_2								= 227,		// 医疗2
	//SKILL_SUPER_HEAL_2							= 228,		// 大治疗2
	//SKILL_HEARTLESS_ARROW_2						= 229,		// 绝情箭2
	//SKILL_HEARTLESS_ARROW_3						= 230,		// 绝情箭3
	//SKILL_LIGHTING_ARROW_2						= 231,		// 流光2
	//SKILL_SWORDSHIP_3							= 232,		// 基本剑术3
	//SKILL_SWORDSHIP_4							= 233,		// 基本剑术4

	////#########################################################################
	//SKILL_TAIJI									= 301,		// 太极避
	//SKILL_WEAK									= 302,		// 太虚阵
	//SKILL_GOD_BLESS								= 303,		// 金刚劲
	//SKILL_ORIGIN								= 304,		// 先天功
	//SKILL_CURE									= 305,		// 清心咒
	//SKILL_FIRE_BOLT								= 306,		// 冥火咒
	//SKILL_LIGHTNING								= 307,		// 奔雷
	//SKILL_FIRE_WALL								= 308,		// 三昧真火
	//SKILL_INFERNOL								= 309,		// 炼狱玄炎
	//SKILL_SEVEN_SHOOTING_STAR					= 310,		// 七星汇芒
	//SKILL_CHAOS_SPHERE							= 311,		// 混沌苍穹
	//SKILL_SEAL									= 312,		// 封魔符
	//SKILL_YINYANG								= 313,		// 阴阳界
	//SKILL_GOD_PUNISHMENT						= 314,		// 天罚
	//SKILL_SOUL_COLLECT							= 315,		// 集灵咒
	//SKILL_SOUL_MIRROR							= 316,		// 囚魂镜
	//SKILL_FIRE_BALL								= 317,		// 大冥火咒
	//SKILL_CHAIN_LIGHTNING						= 318,		// 电闪
	//SKILL_THUNDER_BLOW							= 319,		// 雷暴
	//SKILL_GOD_THUNDER							= 320,		// 天雷阵
	//SKILL_MANASHIELD							= 321,		// 灵气护甲
	//SKILL_PROMOTION								= 322,		// 增持

	////#########################################################################
	//SKILL_GOD_THUNDER_2							= 323,		// 天雷阵2
	//SKILL_HEARTEN								= 324,		// 振奋
	//SKILL_GOD_BLESS_2							= 325,		// 金刚劲2
	//SKILL_YINYANG_2								= 326,		// 阴阳界2

	////#########################################################################
	////##怪物的技能
	//SKILL_SPIDER_POISON							= 401,		// 蜘蛛毒击
	//SKILL_KNOCK_OUT								= 402,		// 夸父的击昏
	//SKILL_SNOW_STORM							= 403,		// 雪姬的雪暴
	//SKILL_CORPSE_CANDLE_BLASTING				= 404,		// 鬼火自爆
	//SKILL_SPORE_BLASTING						= 405,		// 孢子自爆
	//SKILL_YAKSHA_SLASH							= 406,		// 七道修罗重斩
	//SKILL_MONSTER_THORN							= 407,		// 怪物地刺
	//SKILL_SPIDER_MIST							= 408,		// 八角将毒雾
	//SKILL_SPIDER_WEB							= 409,		// 八角缠绕
	//SKILL_SUMMON_CORPSE_CANDLE					= 410,		// 召唤鬼火
	//SKILL_SUMMON_SKELETON						= 411,		// 召唤骷髅
	//SKILL_SUMMON_SPORE							= 412,		// 召唤孢子

	//SKILL_CHUCK_STONE							= 413,		// 山妖弹石
	//SKILL_YUNSHENG_LIGHTNING					= 414,		// 云生奔雷
	//SKILL_CORPSE_PTOMAINE						= 415,		// 尸毒
	//SKILL_ENERGY_BOLT							= 416,		// 邪灵冥火
	//SKILL_SKELETON_ARCHERY						= 417,		// 骇卒弓箭
	//SKILL_ZOMBIE_CLAW							= 418,		// 九幽鬼爪

	//SKILL_FURY									= 419,		// 狂暴
	//SKILL_LITTLE_STAR							= 420,		// 小七星
	//SKILL_SNAKE_BOLT							= 421,		// 飞蛇电球
	//SKILL_SPRITE_BURN							= 422,		// 火魔灼烧
	//SKILL_MACHINERY_STOMP						= 423,		// 机关践踏

	//SKILL_MONSTER_HEAL							= 424,		// 怪物回血技能

	////大国主技能
	//SKILL_LORD_FAST_ATTACK						= 501,		// 大国主快攻
	//SKILL_LORD_WIDERANGING_ATTACK				= 502,		// 大国主面攻

	//SKILL_BOSS_BLUE_FURY						= 503,		// 蓝蓝暴怒
	//SKILL_BOSS_BLUE_QUAKE						= 504,		// 蓝蓝震地

	//SKILL_BOSS_FIEND_SUMMON						= 505,		// 邪神召唤
	//SKILL_BOSS_FIEND_PENETRATE					= 506,		// 邪神贯穿


	////#########################################################################
	//// 传2新技能
	//SKILL_ENLARGE_MAX_HP						= 601,		// 增加HP上限的被动技能
	//SKILL_ENLARGE_MAX_MP						= 602,		// 增加MP上限的被动技能
	//SKILL_ENLARGE_FULL_MISS						= 603,		// 增加FULLMISS被动技能

	////*****************
	//SKILL_NON_FUN								= 900,		//无作用技能，仅做为显示

	//SKILL_NON_FUN_1								= 901,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_2								= 902,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_3								= 903,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_4								= 904,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_5								= 905,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_6								= 906,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_7								= 907,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_8								= 908,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_9								= 909,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_10							= 910,		//无作用技能，仅做为显示

	//SKILL_NON_FUN_11							= 911,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_12							= 912,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_13							= 913,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_14							= 914,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_15							= 915,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_16							= 916,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_17							= 917,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_18							= 918,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_19							= 919,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_20							= 920,		//无作用技能，仅做为显示

	//SKILL_NON_FUN_21							= 921,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_22							= 922,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_23							= 923,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_24							= 924,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_25							= 925,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_26							= 926,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_27							= 927,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_28							= 928,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_29							= 929,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_30							= 930,		//无作用技能，仅做为显示

	//SKILL_NON_FUN_31							= 931,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_32							= 932,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_33							= 933,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_34							= 934,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_35							= 935,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_36							= 936,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_37							= 937,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_38							= 938,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_39							= 939,		//无作用技能，仅做为显示
	//SKILL_NON_FUN_40							= 940,		//无作用技能，仅做为显示

	//***************************
	SKILL_UNKNOW								= 0x7fffffff,
};

enum tagSkillUsage
{
	//##使用者所损失的基本点数
	SKILL_USAGE_USER_HP_LOSE					= 1,
	SKILL_USAGE_USER_MP_LOSE					= 2,
	SKILL_USAGE_USER_RP_LOSE					= 3,
	SKILL_USAGE_USER_YP_LOSE					= 4,

	SKILL_USAGE_USER_HP_PERCENT_LOSE			= 5,
	SKILL_USAGE_USER_MP_PERCENT_LOSE			= 6,
	SKILL_USAGE_USER_RP_PERCENT_LOSE			= 7,
	SKILL_USAGE_USER_YP_PERCENT_LOSE			= 8,

	//##被施法者所损失的基本点数
	SKILL_USAGE_TARGET_HP_LOSE					= 11,
	SKILL_USAGE_TARGET_MP_LOSE					= 12,
	SKILL_USAGE_TARGET_RP_LOSE					= 13,
	SKILL_USAGE_TARGET_YP_LOSE					= 14,

	SKILL_USAGE_TARGET_HP_PERCENT_LOSE			= 15,
	SKILL_USAGE_TARGET_MP_PERCENT_LOSE			= 16,
	SKILL_USAGE_TARGET_RP_PERCENT_LOSE			= 17,
	SKILL_USAGE_TARGET_YP_PERCENT_LOSE			= 18,

	//##施法者所增加的基本点数
	SKILL_USAGE_USER_HP_GAIN					= 21,
	SKILL_USAGE_USER_MP_GAIN					= 22,
	SKILL_USAGE_USER_RP_GAIN					= 23,
	SKILL_USAGE_USER_YP_GAIN					= 24,

	SKILL_USAGE_USER_HP_PERCENT_GAIN			= 25,
	SKILL_USAGE_USER_MP_PERCENT_GAIN			= 26,
	SKILL_USAGE_USER_RP_PERCENT_GAIN			= 27,
	SKILL_USAGE_USER_YP_PERCENT_GAIN			= 28,

	//##被施法者所增加的基本点数
	SKILL_USAGE_TARGET_HP_GAIN					= 31,
	SKILL_USAGE_TARGET_MP_GAIN					= 32,
	SKILL_USAGE_TARGET_RP_GAIN					= 33,
	SKILL_USAGE_TARGET_YP_GAIN					= 34,

	SKILL_USAGE_TARGET_HP_PERCENT_GAIN			= 35,
	SKILL_USAGE_TARGET_MP_PERCENT_GAIN			= 36,
	SKILL_USAGE_TARGET_RP_PERCENT_GAIN			= 37,
	SKILL_USAGE_TARGET_YP_PERCENT_GAIN			= 38,

	//###############################################################
	//##被施法者的基本属性的改变
	SKILL_USAGE_TARGET_STR_GAIN					= 101,
	SKILL_USAGE_TARGET_DEX_GAIN					= 102,
	SKILL_USAGE_TARGET_CON_GAIN					= 103,
	SKILL_USAGE_TARGET_INT_GAIN					= 104,
	SKILL_USAGE_TARGET_ATK_GAIN					= 105,
	SKILL_USAGE_TARGET_HIT_GAIN					= 106,
	SKILL_USAGE_TARGET_BURDEN_GAIN				= 107,
	SKILL_USAGE_TARGET_CCH_GAIN					= 108,
	SKILL_USAGE_TARGET_DEF_GAIN					= 109,
	SKILL_USAGE_TARGET_DODGE_GAIN				= 110,
	SKILL_USAGE_TARGET_ATK_SPEED_GAIN			= 111,
	SKILL_USAGE_TARGET_ELEMENT_RESISTANT_GAIN	= 112,
	SKILL_USAGE_TARGET_HP_RECOVER_SPEED_GAIN	= 113,
	SKILL_USAGE_TARGET_MP_RECOVER_SPEED_GAIN	= 114,
	SKILL_USAGE_TARGET_ELEMENT_MODIFY_GAIN		= 115,
	SKILL_USAGE_TARGET_MIN_ATK_GAIN				= 116,
	SKILL_USAGE_TARGET_MAX_ATK_GAIN				= 117,
	SKILL_USAGE_MAX_HP_GAIN						= 118,
	SKILL_USAGE_MAX_MP_GAIN						= 119,

	SKILL_USAGE_TARGET_ELEMENT_MODIFY_GAIN_COEFFICIENT	= 120,	// 增加目标元素系数coefficient
	SKILL_USAGE_TARGET_MIN_ATK_GAIN_COEFFICIENT			= 121,	// 增加目标最小攻击系数coefficient
	SKILL_USAGE_TARGET_MAX_ATK_GAIN_COEFFICIENT			= 122,	// 增加目标最大攻击系数

	SKILL_USAGE_TARGET_DEF_GAIN_COEFFICIENT		= 123,			// 增加目标DEF系数coefficient
	SKILL_USAGE_TARGET_ER_GAIN_COEFFICIENT		= 124,			// 增加目标ER（元素抵抗力）系数

	SKILL_USAGE_TARGET_BLAST_COEFFICIENT_GAIN			= 125,	// 增加物理暴击系数
	SKILL_USAGE_TARGET_ELEMENTBLAST_COEFFICIENT_GAIN	= 126,	// 增加法术暴击系数
	SKILL_USAGE_TARGET_FULLMISS_GAIN					= 127,
	SKILL_USAGE_ATTACK_AVOID_COEFFICIENT_GAIN	= 128,
	SKILL_USAGE_ELEMENT_AVOID_COEFFICIENT_GAIN	= 129,

	//###############################################################
	SKILL_USAGE_TARGET_STR_LOSE					= 201,
	SKILL_USAGE_TARGET_DEX_LOSE					= 202,
	SKILL_USAGE_TARGET_CON_LOSE					= 203,
	SKILL_USAGE_TARGET_INT_LOSE					= 204,
	SKILL_USAGE_TARGET_ATK_LOSE					= 205,
	SKILL_USAGE_TARGET_HIT_LOSE					= 206,
	SKILL_USAGE_TARGET_BURDEN_LOSE				= 207,
	SKILL_USAGE_TARGET_CCH_LOSE					= 208,
	SKILL_USAGE_TARGET_DEF_LOSE					= 209,
	SKILL_USAGE_TARGET_DODGE_LOSE				= 210,
	SKILL_USAGE_TARGET_ATK_SPEED_LOSE			= 211,
	SKILL_USAGE_TARGET_ELEMENT_RESISTANT_LOSE	= 212,
	SKILL_USAGE_TARGET_HP_RECOVER_SPEED_LOSE	= 213,
	SKILL_USAGE_TARGET_MP_RECOVER_SPEED_LOSE	= 214,
	SKILL_USAGE_TARGET_ELEMENT_MODIFY_LOSE		= 215,
	SKILL_USAGE_TARGET_MIN_ATK_LOSE				= 216,
	SKILL_USAGE_TARGET_MAX_ATK_LOSE				= 217,
	SKILL_USAGE_MAX_HP_LOSE						= 218,
	SKILL_USAGE_MAX_MP_LOSE						= 219,

	SKILL_USAGE_TARGET_ELEMENT_MODIFY_LOSE_COEFFICIENT	= 220,	// 减少目标元素系数coefficient
	SKILL_USAGE_TARGET_MIN_ATK_LOSE_COEFFICIENT			= 221,	// 减少目标最小攻击系数coefficient
	SKILL_USAGE_TARGET_MAX_ATK_LOSE_COEFFICIENT			= 222,	// 减少目标最大攻击系数


	SKILL_USAGE_TARGET_DEF_LOSE_COEFFICIENT		= 223,			// 减少目标DEF系数coefficient
	SKILL_USAGE_TARGET_ER_LOSE_COEFFICIENT		= 224,			// 减少目标ER（元素抵抗力）系数


	SKILL_USAGE_TARGET_BLAST_COEFFICIENT_LOSE			= 225,	// 减少物理暴击系数
	SKILL_USAGE_TARGET_ELEMENTBLAST_COEFFICIENT_LOSE	= 226,	// 减少法术暴击系数
	SKILL_USAGE_TARGET_FULLMISS_LOSE					= 227,
	SKILL_USAGE_ATTACK_AVOID_COEFFICIENT_LOSE	= 228,
	SKILL_USAGE_ELEMENT_AVOID_COEFFICIENT_LOSE	= 229,
	//###############################################################
	//SKILL_USAGE_TARGET_ATK_MULTIPLE_GAIN		= 705,
	//SKILL_USAGE_TARGET_ATK_MULTIPLE_LOSE		= 805,

	//##特殊效果
	SKILL_USAGE_TARGET_BACK_STEP				= 1001,
	SKILL_USAGE_TARGET_CANNOT_MOVE				= 1002,

	//##特殊效果相关的一些参数
	SKILL_USAGE_TARGET_MOVE_SPEED				= 2001,			//##移动速度
	SKILL_USAGE_MIN_ANGLE						= 2002,			//##最小角度
	SKILL_USAGE_MAX_ANGLE						= 2003,			//##最大角度

	//##攻击范围
	SKILL_USAGE_TARGET_ATTACK_SQUARE			= 5001,
	SKILL_USAGE_TARGET_ATTACK_LINE				= 5002,
	SKILL_USAGE_TARGET_MAX_DISTANT				= 5003,
	SKILL_USAGE_TARGET_MIN_DISTANT				= 5004,

	//##频率相关的参数
	SKILL_USAGE_TARGET_AFFECT_FREQUENCY			= 6001,			//##频率

	//##发出技能时的参数
	SKILL_USAGE_DELAY_TIME						= 10001,		//##技能准备时间
	SKILL_USAGE_STATE_PERSIST_TIME				= 10002,		//##状态持续时间
	SKILL_USAGE_STATE_PERSIST_TIME_MODIFIER		= 10003,		//##状态持续时间修正
	SKILL_USAGE_STATE_PRESENT_DEGREE			= 10004,		//##状态表现程度
	SKILL_USAGE_REUSE_SKILL_DELAY_TIME			= 10005,
	SKILL_USAGE_CAN_BE_BREAKED					= 10006,		//##能否被打断
	SKILL_USAGE_SKILL_PERSIST_TIME				= 10007,		//##技能的最大使用时间
	SKILL_USAGE_MISSILE_FLYING_TIME				= 10008,		//##飞行物飞行时间，单位：毫秒/格
	SKILL_USAGE_ACTION_INTERVAL					= 10009,		//##动作间隔
	SKILL_USAGE_STATE_HP						= 10010,		// 状态生命值

	//##时间相关的参数
	SKILL_USAGE_FIRST_TIME						= 15001,		//##第一次时间
	SKILL_USAGE_SECOND_TIME						= 15002,		//##第二次时间
	SKILL_USAGE_THIRD_TIME						= 15003,		//##第三次时间
	SKILL_USAGE_TIME_LIMIT						= 15004,		//##时间上限
	SKILL_USAGE_TIME_PERCENT					= 15005,		//##时间百分比

	//##战斗相关的参数
	SKILL_USAGE_USER_HIT_MODIFIER				= 20001,		//##命中率的修正(躲闪计算后的增加量)
	SKILL_USAGE_TARGET_FINAL_DAMAGE_MODIFIER	= 20002,		//##最终伤害修正(防御计算后的增加量)
	SKILL_USAGE_TARGET_DAMAGE_FACTOR			= 20003,		//##伤害加成系数
	SKILL_USAGE_USER_DEX_TO_DAMAGE_RATE			= 20004,		//##敏捷到伤害的换算系数
	SKILL_USAGE_USER_DAMAGE_FACTOR				= 20005,		//##攻方攻击力加成系数
	SKILL_USAGE_ADDITION_EX_STATE				= 20006,		//##附带的异常状态
	SKILL_USAGE_ADDITION_EX_STATE_RATE			= 20007,		//##附带的异常状态的几率
	SKILL_USAGE_MIN_ATTACK						= 20008,		//##最小攻击力
	SKILL_USAGE_MAX_ATTACK						= 20009,		//##最大攻击力
	SKILL_USAGE_CONST							= 20010,		//##常数

	SKILL_USAGE_MARJO_TARGET_DAMAGE_FACTOR		= 20011,		//##主要目标的伤害加成系数
	SKILL_USAGE_MINOR_TARGET_DAMAGE_FACTOR		= 20012,		//##次要目标的伤害加成系数

	SKILL_USAGE_ADDITION_ELEMENT_ATK			= 20013,		//##附加的元素攻击力
	SKILL_USAGE_ADDITION_SOUL_ATK				= 20014,		//##附加的灵系攻击力
	SKILL_USAGE_EM_MODIFIER						= 20015,		//##元素修正的修正值[百分比 0 - N]

	SKILL_USAGE_AMOUNT							= 20016,		//##数量
	SKILL_USAGE_AMOUNT_LIMIT					= 20017,		//##数量上限

	SKILL_USAGE_WEAPON_DAMAGE_LEVEL_MODIFIER	= 20018,		// 武器强度修正
	SKILL_USAGE_HEAL_RECOVER_COEFFICIENT		= 20019,		// 治疗回复系数
	SKILL_USAGE_PARAMETER_PERCENT				= 20020,		// 参数百分比

	SKILL_USAGE_TARGET_DAMAGE_FACTOR2			= 20021,		//##伤害加成系数2
	SKILL_USAGE_TARGET_DAMAGE_FACTOR3			= 20022,		//##伤害加成系数3
	SKILL_USAGE_TARGET_DAMAGE_FACTOR4			= 20023,		//##伤害加成系数4

	SKILL_USAGE_TARGET_HP_DECREASE_FACTOR		= 20024,		// HP 减少系数
	SKILL_USAGE_TARGET_MP_DECREASE_FACTOR		= 20025,		// MP 减少系数

	//##召唤相关的参数
	SKILL_USAGE_SUMMONED_CREATURE_LIEF_TIME		= 30001,		//##召唤兽生存时间
	SKILL_USAGE_SUMMONED_CREATURE_SPEED			= 30002,		//##召唤兽移动速度,单位:毫秒/格s
	SKILL_USAGE_SUMMONED_CREATURE_ID			= 30003,		//##召唤兽的ID
	SKILL_USAGE_SUMMONED_CREATURE_ID2			= 30004,		//##召唤兽的ID2
	SKILL_USAGE_SUMMONED_CREATURE_ID3			= 30005,		//##召唤兽的ID3

	//##宠物相关
	SKILL_USAGE_PET_LEVEL_LIMIT					= 31000,		//##宠物的最高级别限制
	SKILL_USAGE_PET_AMOUNT_LIMIT				= 31001,		//##宠物的数量限制

	//##概率相关
	SKILL_USAGE_BASE_PROBABILITY				= 40001,		//##最小的概率

	//道具相关
	SKILL_USAGE_ITEM_IDX						= 50001,		// 物品的索引号
	SKILL_USAGE_ITEM_NUM						= 50002,		// 物品的数量

	SKILL_USAGE_UNKNOW							= 0x7fffffff,
};

enum tagUseSkillResult
{
	SKILL_USE_RESULT_BEGIN,						//##表示条件满足，开始使用技能
	SKILL_USE_RESULT_FAILED,
	SKILL_USE_RESULT_END,						//##施法结束
};

enum tagUseFailed
{
	SKILL_USE_FAILED_INVALID_UNKNOWN=0,			//未知失败	
	SKILL_USE_FAILED_INVALID_HP,
	SKILL_USE_FAILED_INVALID_MP,
	SKILL_USE_FAILED_INVALID_RP,
	SKILL_USE_FAILED_INVALID_YP,
	SKILL_USE_FAILED_INVALID_TARGET,
	SKILL_USE_FAILED_INVALID_DISTANT,
	SKILL_USE_FAILED_INVALID_SKILL,
	SKILL_USE_FAILED_INVALID_TIME,				//##允许再次使用技能的时间未到达
    SKILL_USE_FAILED_INVALID_CDTIME,            //CD时间
	SKILL_USE_FAILED_INVALID_EQUIP,				//##无效的装备
	SKILL_USE_FAILED_BLOCKED,					//##被阻挡了	
	SKILL_USE_FAILED_BREAK,						//中断
	SKILL_USE_FAILED_NOOBJECT,					//没有作用目标
	SKILL_USE_FAILED_INVALID_INTONATE,			//吟唱没有结束
	SKILL_USE_FAILED_INVALID_OCCUPATION,		//职业
	SKILL_USE_FAILED_INVALID_FACTURE,			//制作失败
	SKILL_USE_FAILED_INVALID_COLLECT,			//采集失败
	SKILL_USE_FAILED_INVALID_ACT,				//动作没有完成
	SKILL_USE_FAILED_INVALID_MOVING,			//移动中不能使用技能

	SKILL_USE_FAILED_C_NOCOLLECTION,			//无采集对象
	SKILL_USE_FAILED_C_STATE_ERROR,				//不能被采集状态
	SKILL_USE_FAILED_C_ROLE_PROTECT,			//任务保护状态
	SKILL_USE_FAILED_C_UNKNOW_COLLECTION,		//未知采集物
	SKILL_USE_FAILED_C_DOCCU_ERROR,				//副职业限制
	SKILL_USE_FAILED_C_LEVEL_ERROR,				//采集等级限制
	SKILL_USE_FAILED_C_GOODSCONDITION_ERROR,	//物品条件不满足
	SKILL_USE_FAILED_C_NO_BAGSPACE,				//背包空间不满足
	SKILL_USE_FAILED_C_NOT_ROLECOLLECTION,		//不是任务采集物
	SKILL_USE_FAILED_C_QUESTSTEP_ERROR,			//任务步骤不能对应
	SKILL_USE_FAILED_C_UNKOWN_COLLECTTYPE,		//未知的采集类型
	SKILL_USE_FAILED_CANNOTCOLLECTION_STATE,	//当前状态不能采集

	SKILL_USE_FAILED_F_NO_FACTURE,				//不存在的制作项
	SKILL_USE_FAILED_F_DOCCU_ERROR,				//副职业限制
	SKILL_USE_FAILED_F_NOTHAS_FACTURE,			//无此制作条目
	SKILL_USE_FAILED_F_GOODSCONDITION_ERROR,	//工具限制
	SKILL_USE_FAILED_F_MATERIALS_ERROR,			//原料限制
	SKILL_USE_FAILED_F_NPC_ERROR,				//npc距离限制
	SKILL_USE_FAILED_F_NO_BAGSPACE,				//背包空间限制
	SKILL_USE_FAILED_CANNOTFACTURE_STATE,		//当前状态不能制作

	SKILL_USE_FAILED_OBJECT_INGOD,				//目标处于无敌状态
	SKILL_USE_FAILED_INVALID_REGION,			//场景不符合施放条件
	SKILL_USE_FAILED_INVALID_STATE,				//##当前状态不能使用技能，比如被麻痹
    SKILL_USE_FAILED_INVALID_NOTSAMEPHASE,      //不同的相位,不能互相使用技能
    SKILL_USE_FAILED_INVALID_NOTUSESKILL,       //当前状态不能使用技能
    SKILL_USE_FAILED_INVALID_FULLDISTRACT,      //满的杂念值

	
	SKILL_USE_NO_FAILED,						//成功
};
enum tagStudyFailed
{
	SKILL_STUDY_FAILED_OCCUPATION,				//职业不符合学习条件
	SKILL_STUDY_FAILED_RANK,					//等级不符合学习条件
	SKILL_STUDY_FAILED_SP,						//SP不符合学习条件
	SKILL_STUDY_FAILED_SKILL,					//前置技能不符合学习条件
	SKILL_STUDY_FAILED_LIMIT_SKILL,				//前置限制技能不符合条件
	SKILL_STUDY_FAILED_GOOD,					//物品
	SKILL_STUDY_FAILED_GOLD,					//金币不足
	SKILL_STUDY_FAILED_SILVER,					//银币不足
	SKILL_STUDY_FAILED_OCCULV,					//职业等级不符合条件
	
};

enum tagDamageType
{
	DAMAGE_TYPE_NO,
	DAMAGE_TYPE_HP,
	DAMAGE_TYPE_MP,
    DAMAGE_TYPE_TAUNT,
};

struct tagExState
{
	ushort	type;		// 类型
	ulong	level;		//等级					2
	ulong	keeptime;	//状态持续时间			4

	ushort	maxhp;		//增加MAXHP上限		2
	ushort	maxmp;		//增加MAXMP上限		2

	ushort	minatk;		//增加目标最小ATK		2
	ushort	maxatk;		//增加目标最大ATK		2

	ushort	elm;		//增加目标元素攻击力	2
	ushort	def;		//增加目标DEF			2
	ushort	elmdef;		//增加目标元素抵抗力	2

	ushort	cch;		//增加目标CCH			2
	ushort	fullmiss;	//增加FULLMISS系数		2
	ushort	atkavoid;	//增加AttackAvoid系数	2
	ushort	elmavoid;	//增加ElementAvoid系数	2
	ushort	hit;		//命中修正				2
	ushort	dodge;		//增加目标DODGE			2

	ulong	item_idx;	// 道具索引编号
	ulong	item_num;	// 道具数量
	ulong	frequency;	// 单位毫秒的频率
};



#endif