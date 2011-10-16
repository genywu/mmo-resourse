// Fox@20090115----------------------------------------------
// File:    BasePetServerDef.h
// Brief:   定义宠物使用的枚举/结构/类型(仅供GS/WS使用).
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Jan.15, 2009
// Fox@20090115----------------------------------------------

#ifndef BASE_PET_SERVER_DEF_H
#define BASE_PET_SERVER_DEF_H

#include "BasePetDef.h"
#include "../public/GUID.h"

#define CONDITION_CHECK(SWITCH) \
	if( false == (SWITCH) ) return 0;

class CPet;

const long				MAX_LEVEL = 256;
const long				F_MAX_CLASSES = 16;
const long				S_MAX_CLASSES = 5;

enum MOVE_TYPE
{
	MT_WALK				= 0x00,					// 行走
	MT_RUN				= 0x01,					// 奔跑
};

enum INVISIBLE_TYPE
{
	IT_VISIBLE			= 0x00,					// 一直可见
	IT_VISIBLE_TEAM		= 0x01,					// 隐形, 仅队友可见
	IT_VISIBLE_COUNTRY	= 0x02,					// 隐形, 仅国人可见
	IT_VISIBLE_UNION	= 0x03,					// 隐形, 仅联盟可见
	IT_VISIBLE_FACTION	= 0x04,					// 隐形, 仅公会可见
	IT_INVISIBLE		= 0xFF,					// 完全不可见
};

enum SEL_ATK_TYPE
{
	SAT_CAN_BESEL		= 0x01,					// 可以被选择
	SAT_CAN_BEATK		= 0x02,					// 可以被攻击
	SAT_CAN_FIGHT		= 0x04,					// 可以参与战斗
};

enum DN_OP_TYPE
{
	NOT_BE_SAVED		= 0x00,					// 不需要存盘
	CAN_BE_SAVED		= 0x01,					// 需要存盘
};

enum INIT_TYPE
{
	IT_SAME_RGN			= 0x01,					// 同一场景切换
	IT_SAME_GS			= 0x02,					// 同一GS切换
	IT_CREATE			= 0x10,					// 第一次被创建
	IT_RELIVE			= 0x11,					// 复活
	IT_LOGIN_GS			= 0x12,					// 不同GS切换(登录游戏)
};

enum PET_EVENT_TYPE
{
	PET_HPRenew,								// 宠物刷新HP
	PET_MPRenew,								// 宠物刷新MP
	PET_GorgeRenew,								// 宠物刷新饱食度
	PET_LoyaltyRenew,							// 宠物刷新忠诚度
	PET_LifeTimeout,							// 宠物生命终结
	PET_SearchEnemy,							// 周期寻敌
};

enum PET_OPER_TYPE
{
	PET_OT_NULL				= 0x00,
	PET_OT_APPEAR			= 0x01,				// 出现
	PET_OT_INACTIVE			= 0x10,				// 不从玩家宠物列表删除, 非激活状态
	PET_OT_DISAPPEAR		= 0x11,				// 消失
	PET_OT_HOUSE			= 0x12,				// 宅藏或召唤物生命终结
	PET_OT_PLAYER_EXIT		= 0x13,				// 玩家下线(从玩家宠物列表清空, 仅玩家析构时调用, 除陷阱外均存盘)
	PET_OT_BEEN_KILLED		= 0x14,				// 战宠, 召唤物被杀死
	PET_OT_CHANGE_REGION	= 0x15,				// 切换场景

	PET_OT_DELETE			= 0x20,				// 从玩家宠物列表删除
	PET_OT_DETECT			= 0x21,				// 叛离
	PET_OT_CANCEL			= 0x23,				// 被放弃
	PET_OT_TRAP_END			= 0x24,				// 陷阱生命终结
};

// 饱食度影响属性集
struct tagGorge
{
	const tagGorge& operator=(const tagGorge& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	int			nLoyalty;			// 忠诚度
	double		fHpMp;				// HP/MP回复
	int			nAbsorb;			// 专注
	int			nInterval;			// 技能释放间隔
	double		fMiss;				// 丢失几率
	double		fEffect;			// 效果加成
};

// 技能开启
struct tagSkillOpen
{
	const tagSkillOpen& operator=(const tagSkillOpen& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	int			nClass;				// 宠物职业(父类型|子类型)
	int			nLevel;				// 开启等级
	int			nRate;				// 开启概率
};

// 天赋基本配置
struct tagGenSkillBase
{
	long		lID;				// 技能ID
	long		lLevel;				// 技能等级
};

// 宠物系统统一配置(与宠物类型无关, 无需数据库存取)
struct tagPetConf
{
	const tagPetConf& operator=(const tagPetConf& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	int			nFuncSwitch;						// 功能模块开关
	int			nAISpringTime;						// AI触发间隔(目前仅对陷阱适用)
	int			nHPMPInt;							// HP自动回复间隔
	int			nGorgeInt;							// 饱食度自动回复间隔
	long		lGorgeLvNum;						// 忠诚度回复分级数量
	int			nLoyaltyInt;						// 忠诚度自动回复间隔
	long		lLoyaltyLvNum;						// 忠诚度回复分级数量
	int			nMaxGorge;							// 饱食度上限
	int			nMaxLoyalty;						// 忠诚度上限
	int			nFooddLvInt;						// 食物分级间隔
	long		lFeedLvNum;							// 喂食分级数量
	long		lMaxLevel;							// 宠物最大开放等级
	int			nInitGeniusSkillMin;				// 初始天赋技能最小数量
	int			nInitGeniusSkillMax;				// 初始天赋技能最大数量
	long		lGeniusSkillsSize;					// 天赋技能总数
	long		lGeniusSkillNum;					// 宠物天赋技能最大数
	long		lStudySkillsSize;					// 学习技能总数
	long		lStudySkillNum;						// 宠物学习技能最大数
	int			MaxDistances[S_MAX_CLASSES];		// 人宠最远分离距离
	int			HPAutoRenew[F_MAX_CLASSES];			// HP自动回复配置
	int			MPAutoRenew[F_MAX_CLASSES];			// MP自动回复配置
	int			GorgeAutoRenew[S_MAX_CLASSES];		// 饱食度自动回复配置
	int			Gorges[S_MAX_CLASSES];				// 饱食度等级分级
	int			Loyalties[S_MAX_CLASSES];			// 忠诚度等级分级
	int			GeniusSkills[F_MAX_CLASSES];		// 天赋技能列表
	long		Exps[MAX_LEVEL];					// 升级经验
	double		Feed[S_MAX_CLASSES];				// 喂食策略
	double		Speeds[S_MAX_CLASSES];				// 速度设定
	tagGorge	GorgeEffs[S_MAX_CLASSES];			// 饱食度影响属性
	tagSkillOpen GeniusSkillOpen[F_MAX_CLASSES];	// 天赋技能开启条件
	tagSkillOpen StudySkillOpen[F_MAX_CLASSES];		// 学习技能开启条件

	// 宠物原始属性换算
	double		fConsti2MaxHp;						// 耐力-->HP
	double		fConsti2HpRecoverSpeed;				// 耐力-->HP回复速度
	double		fConsti2Def;						// 耐力-->物防
	double		fConsti2CriDef;						// 耐力-->物爆抗性
	double		fWisdom2MaxMp;						// 智慧-->MP
	double		fWisdom2MpRecoverSpeed;				// 智慧-->MP回复速度
	double		fWisdom2Absorb;						// 智慧-->专注
	double		fWisdom2MDef;						// 智慧-->魔防
	double		fStrenth2Parry;						// 力量-->招架
	double		fStrenth2Block;						// 力量-->格挡
	double		fStrenth2MinAtk;					// 力量-->最小物攻
	double		fStrenth2MaxAtk;					// 力量-->最大物攻
	double		fStrenth2Cri;						// 力量-->物爆
	double		fAgility2Parry;						// 敏捷-->招架
	double		fAgility2Hit;						// 敏捷-->命中
	double		fAgility2Dodge;						// 敏捷-->闪避
	double		fAgility2MinAtk;					// 敏捷-->最小物攻
	double		fAgility2MaxAtk;					// 敏捷-->最大物攻
	double		fIntell2MAtk;						// 智力-->魔攻
	double		fIntell2MCri;						// 智力-->魔爆
	double		fSpirit2WillDef;					// 精神-->意志抗性
	double		fSpirit2MCriDef;					// 精神-->魔爆抗性
};

// 宠物原始属性
struct tagPetOrig
{
	tagPetOrig(void)
		: lLevel(0)
		, fMaxHp(0)
		, fMaxMp(0)
		, fStrenth(0)
		, fAgility(0)
		, fConsti(0)
		, fIntell(0)
		, fWisdom(0)
		, fSpirit(0)
		, fCharm(0)
		, fLuck(0)
	{
	}

	const tagPetOrig& operator=(const tagPetOrig& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lLevel;								// 等级
	double		fMaxHp;								// 生命
	double		fMaxMp;								// 法力
	double		fStrenth;							// 力量
	double		fAgility;							// 敏捷
	double		fConsti;							// 耐力
	double		fIntell;							// 智力
	double		fWisdom;							// 智慧
	double		fSpirit;							// 精神
	double		fCharm;								// 魅力
	double		fLuck;								// 幸运
};

// 宠物配置(与宠物类型有关, 无需数据库存取)
struct tagPetBase : public tagPetDisp
{
	tagPetBase(void)
		: lPetType(0)
		, lClass(0)
		, lIndex(0)
		, lPicID(0)
		, lExistTime(0)
		, lInvisible(0)
		, lMoveType(0)
		, lSelAtk(0)
		, lGorge(0)
		, lLoyalty(0)
		, lHpRecoverSpeed(0)
		, lMpRecoverSpeed(0)
	{
		memset(GeniusSkills, 0, sizeof(GeniusSkills));
		memset(szOriName, 0, sizeof(szOriName));
	}

	const tagPetBase& operator=(const tagPetBase& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lPetType;							// 宠物类型
	long		lClass;								// 宠物子类型(精灵星座, 战宠科属, 召唤物类型, 陷阱类型)
	long		lIndex;								// 索引编号
	long		lPicID;								// 图档编号
	long		lExistTime;							// 存在时间
	long		lInvisible;							// 隐形类型
	long		lMoveType;							// 移动类型
	long		lSelAtk;							// 选择攻击战斗位
	long		lGorge;								// 饱食度
	long		lLoyalty;							// 忠诚度
	long		lHpRecoverSpeed;					// 生命回复
	long		lMpRecoverSpeed;					// 法力回复
	long		lSkillNum;							// 宠物初始具有的技能数量
	char		szOriName[32];						// 原始名
	tagGenSkillBase	GeniusSkills[F_MAX_CLASSES];	// 预设天赋技能
};

// 逻辑操作数据(与宠物类型相关, 无需数据库存取)
struct tagPetTemp : public tagPetDetail
{
	tagPetTemp(void)
		: cSaveFlag(0)
		, lHpRecoverSpeed(0)
		, lMpRecoverSpeed(0)
		, lGorgeDelta(0)
		, lLoyaltyDelta(0)
		, lGeniusSkillNum(0)
		, lStudySkillNum(0)
		, lEffect(0)
		, lLastUseSkillTime(0)
	{
	}

	const tagPetTemp& operator=(const tagPetTemp& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	// 修正属性值
	char		cSaveFlag;
	long		lCountry;
	long		lHpRecoverSpeed;
	long		lMpRecoverSpeed;
	long		lGorgeDelta;
	long		lLoyaltyDelta;
	long		lGeniusSkillNum;
	long		lStudySkillNum;
	long		lEffect;
	long		lLastUseSkillTime;
};

typedef map<long, tagSkill> SKILL_LIST;
typedef SKILL_LIST::iterator ITR_SKILL_LIST;
typedef vector<tagPetOrig> GROW_LIST;
typedef map<long, GROW_LIST> GROW_CONF;
typedef GROW_CONF::iterator ITR_GROW_CONF;
typedef map<long, tagPetBase> PET_LIST;
typedef const PET_LIST& C_PET_LIST;
typedef PET_LIST::iterator ITR_PET_LIST;
typedef PET_LIST::const_iterator C_ITR_PET_LIST;
typedef map<char, CGUID> ACT_PETS;
typedef const ACT_PETS& C_ACT_PETS;
typedef ACT_PETS::iterator ITR_ACT_PET;
typedef ACT_PETS::const_iterator C_ITR_ACT_PET;
typedef map<CGUID, CPet*> PETS;
typedef const PETS& C_PETS;
typedef PETS::iterator ITR_PET;
typedef PETS::const_iterator C_ITR_PET;

#endif