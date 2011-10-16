// Fox@20090106----------------------------------------------
// File:    BasePetDef.h
// Brief:   定义宠物使用的枚举/结构/类型.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Jan.06, 2009
// Fox@20090106----------------------------------------------

#ifndef BASE_PET_DEF_H
#define BASE_PET_DEF_H

const int PET_STUDY_SKILL_TYPE	= 3;			// 插槽技能类型数

enum PET_ATTR_TYPE
{
	PET_ATTR_UNKNOWN	= 0x0000,
	PET_ATTR_Hp,								// HP当前值
	PET_ATTR_MaxHp,								// HP上限
	PET_ATTR_HpRecoverSpeed,					// HP回复速度
	PET_ATTR_Mp,								// MP当前值
	PET_ATTR_MaxMp,								// MP上限
	PET_ATTR_MpRecoverSpeed,					// MP回复速度
	PET_ATTR_Gorge,								// 饱食度
	PET_ATTR_GorgeRecoverSpeed,					// 饱食度回复速度
	PET_ATTR_GorgeLevel,						// 饱食度等级
	PET_ATTR_Loyalty,							// 忠诚度
	PET_ATTR_Exp,								// 经验值
	PET_ATTR_MaxExp,							// 经验值上限
	PET_ATTR_Level,								// 等级
	PET_ATTR_Strenth,							// 力量
	PET_ATTR_Agility,							// 敏捷
	PET_ATTR_Consti,							// 耐力
	PET_ATTR_Intell,							// 智力
	PET_ATTR_Wisdom,							// 智慧
	PET_ATTR_Spirit,							// 精神
	PET_ATTR_Charm,								// 魅力
	PET_ATTR_Luck,								// 幸运
	PET_ATTR_Parry,								// 招架
	PET_ATTR_Block,								// 格挡
	PET_ATTR_Pierce,							// 穿透
	PET_ATTR_Absorb,							// 专注
	PET_ATTR_Hit,								// 命中
	PET_ATTR_Dodge,								// 闪避
	PET_ATTR_Immunity,							// 免伤
	PET_ATTR_MAtkSpeed,							// 吟唱速度
	PET_ATTR_MinAtk,							// 最小物理攻击
	PET_ATTR_MaxAtk,							// 最大物理攻击
	PET_ATTR_Def,								// 物理防御
	PET_ATTR_MAtk,								// 魔法攻击
	PET_ATTR_MDef,								// 魔法防御
	PET_ATTR_Cri,								// 物理暴击
	PET_ATTR_CriDef,							// 物理暴击抗性
	PET_ATTR_MCri,								// 魔法暴击
	PET_ATTR_MCriDef,							// 魔法暴击抗性
	PET_ATTR_WillDef,							// 意志抗性
	PET_ATTR_ElemDef0,							// 火系元素抗性
	PET_ATTR_ElemDef1,							// 水系元素抗性
	PET_ATTR_ElemDef2,							// 土系元素抗性
	PET_ATTR_ElemDef3,							// 风系元素抗性
	PET_ATTR_ElemDef4,							// 暗系元素抗性
	PET_ATTR_ElemDef5,							// 圣系元素抗性
	PET_ATTR_FightType,							// 战斗模式
	PET_ATTR_MoveType,							// 移动模式
	PET_ATTR_Country,							// 国家
};

enum PET_FOOD_SATIETY							// 饱食度状态
{
	PFS_HUNGRY			= 0,					// 饥饿
	PFS_COMMON			= 1,					// 普通
	PFS_SATIEFY 		= 2,					// 饱食
	PFS_PLEASED 		= 3,					// 满意
};

enum PET_TYPE
{
	PET_TYPE_NULL		= 0x00,
	PET_TYPE_ELF		= 0x01,					// 精灵
	PET_TYPE_HUNTER,							// 战宠
	PET_TYPE_CONJURED,							// 召唤宠
	PET_TYPE_TRAP,								// 陷阱
	PET_TYPE_ORNA,								// 饰宠
	PET_TYPE_COUNT,								// 宠物种类数量
	PET_TYPE_ALL		= 0xFF,					// 所有类型
};

enum PET_MOVE_STYLE
{
	PET_AS_STOP			= 0x00,					// 停留
	PET_AS_FOLLOW		= 0x01,					// 跟随
};

enum PET_FIGHT_STYLE
{
	PET_AS_PASSIVE		= 0x00,					// 被动
	PET_AS_DEFEND		= 0x01,					// 防御
	PET_AS_ACTIVE		= 0x02,					// 主动
};

enum PET_SKILL_TYPE
{
	PET_SKILL_NULL		= 0x0000,				// 未启用
	PET_SKILL_RED		= 0x0001,				// 红色攻击技能
	PET_SKILL_YELLOW	= 0x0002,				// 黄色防御技能
	PET_SKILL_BLUE		= 0x0003,				// 蓝色高级技能
	PET_SKILL_GENIUS	= 0x0010,				// 天赋技能
	PET_SKILL_ENCHASED	= 0x1000,				// 已镶嵌
};

enum PET_OT_AI_TYPE
{
	PET_OT_AI_SET_TARGET	= 0x80,				// 自动寻敌
	PET_OT_AI_ADD_SKILL		= 0x81,				// 技能操作
};

// 宠物技能
struct tagSkill
{
	tagSkill(void)
		: lID(0)
		, lType(0)
		, lPos(0)
		, lLevel(0)
		, lEndCooldown(0)
		, lActPos(-1)
		, bAuto(false)
	{}

	const tagSkill& operator=(const tagSkill& rSkill)
	{
		memcpy(this, &rSkill, sizeof(rSkill));
		return *this;
	}

	bool		bAuto;				// 是否自动释放
	long		lID;				// 编号
	long		lType;				// 技能类型
	long		lPos;				// 技能位置
	long		lLevel;				// 等级
	long		lEndCooldown;		// 冷却结束时间
	long		lActPos;			// 动作条上的位置
};

// 宠物战斗属性/可变属性/显示属性
struct tagPetDisp
{
	tagPetDisp(void)
		: lMaxHp(0)
		, lMaxMp(0)
		, lStrenth(0)
		, lAgility(0)
		, lConsti(0)
		, lIntell(0)
		, lWisdom(0)
		, lSpirit(0)
		, lCharm(0)
		, lLuck(0)
		, lParry(0)
		, lBlock(0)
		, lPierce(0)
		, lAbsorb(0)
		, lHit(0)
		, lDodge(0)
		, lImmunity(0)
		, lMAtkSpeed(0)
		, lMinAtk(0)
		, lMaxAtk(0)
		, lDef(0)
		, lMAtk(0)
		, lMDef(0)
		, lCri(0)
		, lCriDef(0)
		, lMCri(0)
		, lMCriDef(0)
		, lWillDef(0)
		, lElemDef0(0)
		, lElemDef1(0)
		, lElemDef2(0)
		, lElemDef3(0)
		, lElemDef4(0)
		, lElemDef5(0)
	{
	}

	const tagPetDisp& operator=(const tagPetDisp& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lMaxHp;								// 生命
	long		lMaxMp;								// 法力
	long		lStrenth;							// 力量
	long		lAgility;							// 敏捷
	long		lConsti;							// 耐力
	long		lIntell;							// 智力
	long		lWisdom;							// 智慧
	long		lSpirit;							// 精神
	long		lCharm;								// 魅力
	long		lLuck;								// 幸运
	long		lParry;								// 招架
	long		lBlock;								// 格挡
	long		lPierce;							// 穿透
	long		lAbsorb;							// 专注
	long		lHit;								// 命中
	long		lDodge;								// 闪避
	long		lImmunity;							// 免伤
	long		lMAtkSpeed;							// 吟唱速度
	long		lMinAtk;							// 最小物理攻击
	long		lMaxAtk;							// 最大物理攻击
	long		lDef;								// 物理防御
	long		lMAtk;								// 魔法攻击
	long		lMDef;								// 魔法防御
	long		lCri;								// 物理暴击
	long		lCriDef;							// 物理暴击抗性
	long		lMCri;								// 魔法暴击
	long		lMCriDef;							// 魔法暴击抗性
	long		lWillDef;							// 意志抗性
	long		lElemDef0;							// 火系元素抗性
	long		lElemDef1;							// 水系元素抗性
	long		lElemDef2;							// 土系元素抗性
	long		lElemDef3;							// 风系元素抗性
	long		lElemDef4;							// 暗系元素抗性
	long		lElemDef5;							// 圣系元素抗性
};

// 宠物属性数据(与宠物类型相关或无关, 需要数据库存取)
struct tagPetDiff
{
	tagPetDiff(void)
		: lIndex(0)
		, lPos(0)
		, lLevel(0)
		, lExp(0)
		, lHp(0)
		, lMp(0)
		, lNameChgTimes(0)
		, lGorge(0)
		, lLoyalty(0)
		, lSP(0)
		, lActive(0)
	{
		memset(tBirthday, 0, sizeof(tBirthday));
		memset(szName, 0, sizeof(szName));
		memset(szHostName, 0, sizeof(szHostName));
	}

	const tagPetDiff& operator=(const tagPetDiff& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lIndex;					// 索引编号
	long		lPos;					// 存放位置, 0为激活宠物
	long		lLevel;					// 等级
	long		lExp;					// 经验
	long		lHp;					// 生命值
	long		lMp;					// 魔法值
	long		lNameChgTimes;			// 改名次数
	long		lGorge;					// 饱食度
	long		lLoyalty;				// 忠诚度
	long		lSP;					// 训练点数
	long		lActive;				// 激活状态
	long		tBirthday[6];			// 生日
	char		szName[32];				// 宠物名字
	char		szHostName[32];			// 主人名字
};

// 宠物临时属性数据
struct tagPetDetail : public tagPetDisp
{
	tagPetDetail(void)
		: lMoveType(0)
		, lFightType(0)
		, lClass(0)
		, lMaxExp(0)
		, lGorgeLevel(0)
		, lExistTime(0)
	{
	}

	const tagPetDetail& operator=(const tagPetDetail& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	const tagPetDetail& operator+=(const tagPetDisp& rTag)
	{
		lMaxHp += rTag.lMaxHp;		
		lMaxMp += rTag.lMaxMp;		
		lStrenth += rTag.lStrenth;	
		lAgility += rTag.lAgility;	
		lConsti += rTag.lConsti;		
		lIntell += rTag.lIntell;		
		lWisdom += rTag.lWisdom;	
		lSpirit += rTag.lSpirit;	
		lCharm += rTag.lCharm;		
		lLuck += rTag.lLuck;		
		lParry += rTag.lParry;		
		lBlock += rTag.lBlock;		
		lPierce += rTag.lPierce;	
		lAbsorb += rTag.lAbsorb;		
		lHit += rTag.lHit;		
		lDodge += rTag.lDodge;		
		lImmunity += rTag.lImmunity;	
		lMAtkSpeed += rTag.lMAtkSpeed;	
		lMinAtk += rTag.lMinAtk;	
		lMaxAtk += rTag.lMaxAtk;	
		lDef += rTag.lDef;	
		lMAtk += rTag.lMAtk;	
		lMDef += rTag.lMDef;	
		lCri += rTag.lCri;	
		lCriDef += rTag.lCriDef;	
		lMCri += rTag.lMCri;	
		lMCriDef += rTag.lMCriDef;	
		lWillDef += rTag.lWillDef;	
		lElemDef0 += rTag.lElemDef0;	
		lElemDef1 += rTag.lElemDef1;	
		lElemDef2 += rTag.lElemDef2;	
		lElemDef3 += rTag.lElemDef3;	
		lElemDef4 += rTag.lElemDef4;	
		lElemDef5 += rTag.lElemDef5;	

		return *this;
	}

	long		lMoveType;				// 行走模式
	long		lFightType;				// 战斗模式
	long		lClass;					// 科属(精灵星座, 战宠科属, 召唤物类型, 陷阱类型)
	long		lMaxExp;				// 等级经验
	long		lGorgeLevel;			// 饥饿等级
	long		lExistTime;				// 生存时间
};

#endif