/** 
* @file   BasePetServerDef.h
* @author Fox (yulefox@gmail.com)
* @brief  定义宠物使用的枚举/结构/类型(仅供GS/WS使用).
*
* @date   20090115       
*/

#ifndef BASE_PET_SERVER_DEF_H
#define BASE_PET_SERVER_DEF_H

#include "../../../Common/BasePetDef.h"
#define CONDITION_CHECK(SWITCH) \
	if( false == (SWITCH) ) return 0;

class CPet;

/// 最大等级
const long				MAX_LEVEL = 256;
const long				F_MAX_CLASSES = 16;
const long				S_MAX_CLASSES = 5;

/// 忠诚度等级数
const long				Max_Loyalties_Lev = 4;
/// 一级属性数
const long				MAX_FIRST_ATTR = 5;
/// 二级属性数
const long				MAX_SECOND_ATTR = 7;
/// 插槽技能数
const long				MAX_HOLE_SKILL = 3;

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
	PET_HPRenew,								///< 宠物刷新HP
	PET_MPRenew,								///< 宠物刷新MP
	PET_EnergyRenew,							///< 宠物刷新内力
	PET_LoyaltyRenew,							///< 宠物刷新忠诚度
	PET_LifeRenew,								///< 宠物刷新忠诚度
	PET_LifeTimeout,							///< 宠物生命终结
	PET_SearchEnemy,							///< 周期寻敌
};

enum PET_OPER_TYPE
{
	PET_OT_NULL				= 0x00,
	PET_OT_APPEAR			= 0x01,				// 出现
	PET_OT_INACTIVE			= 0x10,				// 不从玩家宠物列表删除, 非激活状态
	PET_OT_DISAPPEAR		= 0x11,				// 消失
	PET_OT_HOUSE			= 0x12,				// 宅藏
	PET_OT_PLAYER_EXIT		= 0x13,				// 玩家下线(从玩家宠物列表清空, 仅玩家析构时调用, 除陷阱外均存盘)
	PET_OT_BEEN_KILLED		= 0x14,				// 侍从被杀死
	PET_OT_CHANGE_REGION	= 0x15,				// 切换场景

	PET_OT_DELETE			= 0x20,				// 从玩家宠物列表删除
	PET_OT_DETECT			= 0x21,				// 叛离
	PET_OT_CANCEL			= 0x23,				// 被放弃
	PET_OT_TRAP_END			= 0x24,				// 陷阱生命终结
};

/// 宠物强化消息类型
enum  PET_UPGRADE_MSG_TYPE
{  
	PET_ERROR_ID			=0x502,				///< 错误的ID
	PET_LEVELUPGR_MSGTYPE	=0x503,				///< 等级
	PET_DISTURBUTE_MSGTYPE	=0x504,				///< 悟性点分配成功
	PET_DAFTERUPGR_MSGTYPE	=0x505,				///< 后天资质强化
	PET_DAFTERRESET_MSGTYPE	=0x506,				///< 后天资质重塑消息
	PET_EXPISENOUGH_MSGTYPE	=0x507,				///< 强化失败,经验需要不足
	PET_PENVALUEUPGR_MSGTYPE=0x508,				///< 品质强化
	PET_FSTONEUPGR_MSGTYPE	=0x509,				///< 魂石强化
	PET_LFUP_MSGTYPE		=0x510,				///< 顿位神位强化
	PET_STARLEVUPGR_MSGTYPE	=0x511,				///< 星等强化
	PET_AMALG_MSGTYPE		=0x512,				///< 侍从融合
};

/// 天赋基本配置
struct tagGenSkillBase
{
	long		lID;				// 技能ID
	long		lLevel;				// 技能等级
};

/// 宠物系统统一配置(与宠物类型无关, 无需数据库存取)
struct tagPetConf
{
	const tagPetConf& operator=(const tagPetConf& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	int			nFuncSwitch;						///< 功能模块开关
	int			nLoyaltyInt;						///< 忠诚度自动回复间隔
	int			nHPMPInt;							///< HP\MP\内力自动回复间隔

	long		lMaxLevel;							///< 宠物最大开放等级
	int			MaxDistances[S_MAX_CLASSES];		///< 人宠最远分离距离
	//int		HPAutoRenew[F_MAX_CLASSES];			///< HP自动回复配置
	//int		MPAutoRenew[F_MAX_CLASSES];			///< MP自动回复配置

	int			loyaltyMax;							///< 忠诚度上限
	int			loyaltyMin;							///< 无法召唤限值
	int			loyaDownUnit;						///< 忠诚度衰减单位
	int			loyaDownVal;						///< 忠诚度衰减速度
	int			loyaUpGrageAdd;						///< 忠诚度升级增加值
	int			loyaTradeRate;						///< 忠诚度交易降低比率 
	int			loyaDieRate;						///< 忠诚度死亡降低比率
	int			loyaltyLvNum;						///< 忠诚度等级
	int			loyalties[Max_Loyalties_Lev];		///< 忠诚度等级分级
	int			layaExp[Max_Loyalties_Lev];			///< 忠诚度相应获得经验比例
	int			layaHurt[Max_Loyalties_Lev];		///< 忠诚度造成伤害比例

	long		Exps[MAX_LEVEL];					///< 升级经验
	long		DExps[MAX_LEVEL];					///< 道行经验
	double		Speeds[S_MAX_CLASSES];				///< 速度设定

	double		Quality[PET_QUALITY_NUM];			///< 品质属性
	double		syncretismRate[9];					///< 品级对应融合系数
	int			gradeNum;							///< 品级数目
	int			Speciality[PET_BASE_NUM];			///< 特性偏好额外提升点
	int			moreHostLevel;						///< 宠物超过玩家等级限制
	int			maxTakeNum;							///< 宠物最大携带量限制
	int			maxExpMultiple;						///< 宠物最大经验存储倍数
	int			maxDExpMultiple;					///< 宠物最大修为经验存储倍数

	int			syncretismMinLev;					///< 侍从融合最小等级
	int			syncretismMaxCount;					///< 侍从融合最大次数
	int			syncretismMaxDen;					///< 侍从融合最大代数

	int			maxLife;							///< 寿命最大值
	int			lifeSubRate;						///< 寿命衰减速度
	int			lifeSubUnit;						///< 寿命衰减单位
	int			lifeDieDownRate;					///< 寿命死亡降低比率

	int			normalExp;							///< 正常经验倍率
	int			normalDExp;							///< 正常修为倍率
	int			expsBaseLevDiff;					///< 经验倍率基本等级差
	int			expsMaxLevDiff;						///< 经验倍率最大等级差
	int			levDiffExpRate[7];					///< 等级差经验倍率
	int			levDiffDExpRate[7];					///< 等级差修为倍率

	int			lunarMaxUseCnt;						///< 阴阳八卦阵每天最大使用次数
	int			lunarMaxUpCnt;						///< 阴阳八卦阵每天最大充能次数
	int			lunarBase;							///< 阴阳八卦阵基数 1000*Lev
	int			lunarReverRate;						///< 阴阳八卦阵回复速度
	int			lunarReverUnit;						///< 阴阳八卦阵回复单位（秒）

	int			talentResetLev;						///< 天资的等级分割线
	double		talentMaxVal;						///< 天资最大值
	double		talentMinVal;						///< 天资最小值
	int			talentNumismatics;					///< 天资重置消耗古币数
	int			talentAlogRate[PET_QUALITY_NUM][3];	///< 天资计算的随机值

	int			qualityMinLev;						///< 品质强化最小等级
	int			qualityLoyaProp;					///< 品质强化忠诚度比例
	int			qualityDropProp;					///< 品质强化成功率降低比例
	int			qualityExpProp;						///< 品质强化需等级经验比例
	int			qualityMaxCostGoodsCnt;				///< 品质强化最大付费道具数
	int			qualityNumismatics;					///< 品质强化消耗古币数
	int			qualitySuccessRate[PET_QUALITY_NUM-1];	///< 品质强化成功率

	int			fStoneMinLev;						///< 魂石强化最小等级
	int			fStoneMaxLev;						///< 魂石强化最大等级
	int			fStoneExpProp;						///< 魂石强化消耗等级经验比例
	int			fStoneN;							///< 魂石强化数据N值
	int			fStoneNumismatics;					///< 魂石强化消耗古币数
	int			fStoneSuccesRate[4];				///< 魂石强化成功率

	int			exclamaMinLev;						///< 顿悟神位最小等级
	int			exclamaLoyaProp;					///< 顿悟神位忠诚度比例
	int			exclamaDropProp;					///< 顿悟神位成功率降低比例
	int			exclamaExpProp;						///< 顿悟神位消耗等级经验比例
	double		exclamaSuccesRate[11];				///< 顿悟神位成功率

	int			starMinLev;							///< 星等最小等级
	int			starMaxLev;							///< 星等最大等级
	int			starNumismatics;					///< 星等强化消耗古币数
	int			starSuccessRate[10];				///< 星等强化成功率
	int			starDropVal[10];					///< 星等强化失败降低值

	int			dAfterStartRate;					///< 后天资质强化起始机率
	int			dAfterDropRate;						///< 后天资质强化递减机率
	int			dAfterMaxVal;						///< 后天资质强化最大获得资质点数
	int			dAfterNumismatics;					///< 后天资质强化消耗古币数

	int			ReSetDAfterNumismatics;				///< 重置后天资质强化次数消耗古币数

	int			savvyCnt;							///< 升级获得悟性点数
	int			savvyNumismatics;					///< 悟性点重置消耗古币数

	// 宠物原始属性换算
	double		fStrenth2Hit;						///< 力量--命中
	double		fStrenth2Dodge;						///< 力量--闪避
	double		fStrenth2MAtkSpeed;					///< 力量--吟唱速度
	double		fStrenth2PhysicsAtk;				///< 力量--物理伤害
	double		fStrenth2PhysicsDef;				///< 力量--物理防御
	double		fStrenth2MAtk;						///< 力量--魔法伤害
	double		fStrenth2MDef;						///< 力量--魔法防御

	double		fDexterity2Hit;						///< 身法--命中
	double		fDexterity2Dodge;					///< 身法--闪避
	double		fDexterity2MAtkSpeed;				///< 身法--吟唱速度
	double		fDexterity2PhysicsAtk;				///< 身法--物理伤害
	double		fDexterity2PhysicsDef;				///< 身法--物理防御
	double		fDexterity2MAtk;					///< 身法--魔法伤害
	double		fDexterity2MDef;					///< 身法--魔法防御

	double		fCon2Hit;							///< 根骨--命中
	double		fCon2Dodge;							///< 根骨--闪避
	double		fCon2MAtkSpeed;						///< 根骨--吟唱速度
	double		fCon2PhysicsAtk;					///< 根骨--物理伤害
	double		fCon2PhysicsDef;					///< 根骨--物理防御
	double		fCon2MAtk;							///< 根骨--魔法伤害
	double		fCon2MDef;							///< 根骨--魔法防御

	double		fIntellect2Hit;						///< 意志--命中
	double		fIntellect2Dodge;					///< 意志--闪避
	double		fIntellect2MAtkSpeed;				///< 意志--吟唱速度
	double		fIntellect2PhysicsAtk;				///< 意志--物理伤害
	double		fIntellect2PhysicsDef;				///< 意志--物理防御
	double		fIntellect2MAtk;					///< 意志--魔法伤害
	double		fIntellect2MDef;					///< 意志--魔法防御

	double		fSpiritualism2Hit;					///< 灵性--命中
	double		fSpiritualism2Dodge;				///< 灵性--闪避
	double		fSpiritualism2MAtkSpeed;			///< 灵性--吟唱速度
	double		fSpiritualism2PhysicsAtk;			///< 灵性--物理伤害
	double		fSpiritualism2PhysicsDef;			///< 灵性--物理防御
	double		fSpiritualism2MAtk;					///< 灵性--魔法伤害
	double		fSpiritualism2MDef;					///< 灵性--魔法防御
};

/// 宠物原始属性
struct tagPetOrig
{
	tagPetOrig(void)
		: lLevel(0)
		, fMaxHp(0)
		, fMaxMp(0)
		, fMaxEnergy(0)
		, fStrenth(0)
		, fDexterity(0)
		, fCon(0)
		, fIntellect(0)
		, fSpiritualism(0)
	{
	}

	const tagPetOrig& operator=(const tagPetOrig& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lLevel;								///< 等级
	double		fMaxHp;								///< 生命
	double		fMaxMp;								///< 法力
	double		fMaxEnergy;							///< 内力
	double		fStrenth;							///< 力量
	double		fDexterity;							///< 身法
	double		fCon;								///< 根骨
	double		fIntellect;							///< 意志
	double		fSpiritualism;						///< 灵性
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
		, lGrade(0)
		, lLife(0)
		, lLoyalty(0)
		, lRace(0)
		, lTakeLevel(0)
		, lHpRecoverSpeed(0)
		, lMpRecoverSpeed(0)
		, lEnergyRecoverSpeed(0)
		, lMinStrenth(0)
		, lMaxStrenth(0)
		, lMinDexterity(0)
		, lMaxDexterity(0)
		, lMinCon(0)
		, lMaxCon(0)
		, lMinIntellect(0)
		, lMaxIntellect(0)
		, lMinSpiritualism(0)
		, lMaxSpiritualism(0)
	{
		memset(szOriName, 0, sizeof(szOriName));
		memset(lQualityRate, 0, PET_QUALITY_NUM*sizeof(long));
		memset(lSkillHole, 0, 3*sizeof(long));
		//memset(lBaseAttrBound, 0, sizeof(lBaseAttrBound));
	}

	const tagPetBase& operator=(const tagPetBase& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lPetType;							///< 宠物类型
	long		lClass;								///< 宠物子类型(精灵星座, 战宠科属, 召唤物类型, 陷阱类型)
	long		lIndex;								///< 索引编号
	long		lPicID;								///< 图档编号
	long		lExistTime;							///< 生存时间
	long		lInvisible;							///< 隐形类型
	long		lMoveType;							///< 移动类型
	long		lSelAtk;							///< 选择攻击战斗位
	long		lGrade;								///< 品级
	long		lLife;								///< 寿命
	long		lLoyalty;							///< 忠诚度
	long		lRace;								///< 种族
	long		lTakeLevel;							///< 携带等级

	long		lHpRecoverSpeed;					///< 生命回复速度
	long		lMpRecoverSpeed;					///< 法力回复速度
	long		lEnergyRecoverSpeed;				///< 内力回复速度
	//long		lBaseAttrBound[PET_BASE_NUM][2];	///< 基础属性上下限
	long		lMinStrenth;						///< 力量天资下限
	long		lMaxStrenth;						///< 力量天资上限
	long		lMinDexterity;						///< 身法天资下限
	long		lMaxDexterity;						///< 身法天资上限
	long		lMinCon;							///< 根骨天资下限
	long		lMaxCon;							///< 根骨天资上限
	long		lMinIntellect;						///< 意志天资下限
	long		lMaxIntellect;						///< 意志天资上限
	long		lMinSpiritualism;					///< 灵性天资下限
	long		lMaxSpiritualism;					///< 灵性天资上限
	long		lNominateMode;						///< 推荐加点

	long		lQualityRate[PET_QUALITY_NUM];		///< 品质机率
	long		lSkillHole[MAX_HOLE_SKILL];			///< 技能槽数量
	char		szOriName[32];						///< 原始名
	tagGenSkillBase	GeniusSkill;					///< 天赋技能
	tagGenSkillBase	OrdinarilySkill;				///< 普通技能

};

//// 逻辑操作数据(与宠物类型相关, 无需数据库存取)
//struct tagPetTemp : public tagPetDetail
//{
//	tagPetTemp(void)
//		: cSaveFlag(0)
//		, lCountry(0)
//		, lHpRecoverSpeed(0)
//		, lMpRecoverSpeed(0)
//		, lEnergyRecoverSpeed(0)
//		, lLoyaltyDelta(0)
//		, lSkillNum(0)
//		, lLoyaltyLev(0)
//		, lEffect(0)
//		, lLastUseSkillTime(0)
//		, lPetLoyaltyLev(-1)
//	{
//	}
//
//	const tagPetTemp& operator=(const tagPetTemp& rTag)
//	{
//		memcpy(this, &rTag, sizeof(rTag));
//		return *this;
//	}
//
//	// 修正属性值
//	char		cSaveFlag;						///< 保存Flag
//	long		lCountry;						///< 国家
//	long		lHpRecoverSpeed;				///< 生命回复速度
//	long		lMpRecoverSpeed;				///< 法力回复速度
//	long		lEnergyRecoverSpeed;			///< 内力回复速度
//	long		lLoyaltyDelta;
//	long		lSkillNum;						///< 技能数（包括天赋技能和后天学习技能）
//	long		lLoyaltyLev;					///< 忠诚度等级
//	long		lEffect;
//	long		lLastUseSkillTime;				///< 最后使用技能时间
//	long		lPetLoyaltyLev;					///< 忠诚度等级
//};

// 其他操作数据(侍从装备、侍从强化所附加的属性集)
struct  tagPetOther
{
	long		lPhysicsAbsorb;						///< 物理伤害吸收率
	long		lMDef;								///< 法力抗性
	long		lMAbsorb;							///< 法术伤害吸收率
	long		lAtkSpeed;							///< 攻击速度
	long		lMatcSpeed;							///< 施法速度
	long		lPhysicsAtk;						///< 物理伤害
	long		lMatcAtk;							///< 法术攻击力
	long		lMoveSpeed;							///< 移动速度

	long		lBledDef;							///< 流血抗性
	long		lDizzyDef;							///< 击晕抗性
	long		lLullDef;							///< 麻痹抗性
	long		lHypnosisDef;						///< 催眠抗性
	long		lPoisonDef;							///< 中毒抗性
	long		lFixDef;							///< 定身抗性
	long		lHpSuck;							///< 吸收气血率
	long		lMpSuck;							///< 吸收法力率
	long		lEnergySuck;						///< 吸收内力率
	long		lElemAtk[PP_NUM];					///< 对其他种族的伤害集
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
//typedef map<char, CGUID> ACT_PETS;
typedef vector<CGUID> ACT_PETS;
typedef const ACT_PETS& C_ACT_PETS;
typedef ACT_PETS::iterator ITR_ACT_PET;
typedef ACT_PETS::const_iterator C_ITR_ACT_PET;
typedef map<CGUID, CPet*> PETS;
typedef const PETS& C_PETS;
typedef PETS::iterator ITR_PET;
typedef PETS::const_iterator C_ITR_PET;

#endif