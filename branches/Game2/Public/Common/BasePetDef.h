/** 
* @file   BasePetDef.h 
* @author Fox (yulefox@gmail.com)
* @brief  定义宠物使用的枚举/结构/类型.
*
* @date   20090106        
*/

#ifndef BASE_PET_DEF_H
#define BASE_PET_DEF_H

const int PET_STUDY_SKILL_TYPE	= 3;			// 插槽技能类型数
const int PET_SELECT_WND_CNT = 7;				// 宠物选择窗口中宠物栏位数量

enum PET_ATTR_TYPE
{
	PET_ATTR_UNKNOWN	= 0x0000,
	PET_ATTR_Hp,								///< HP当前值
	PET_ATTR_MaxHp,								///< HP上限
	PET_ATTR_HpRecoverSpeed,					///< HP回复速度

	PET_ATTR_Mp,								///< MP当前值
	PET_ATTR_MaxMp,								///< MP上限
	PET_ATTR_MpRecoverSpeed,					///< MP回复速度

	PET_ATTR_Energy,							///< 内力当前值
	PET_ATTR_MaxEnergy,							///< 内力上限
	PET_ATTR_EnergyRecoverSpeed,				///< 内力回复速度

	PET_ATTR_Life,								///< 寿命
	PET_ATTR_Loyalty,							///< 忠诚度

	PET_ATTR_Exp,								///< 经验值
	PET_ATTR_MaxExp,							///< 经验值上限
	PET_ATTR_Level,								///< 等级

	PET_ATTR_DExp,								///< 修为经验
	PET_ATTR_MaxDExp,							///< 修为经验上限
	PET_ATTR_DLevel,							///< 道行等级

	PET_ATTR_Strenth,							///< 力量
	PET_ATTR_Dexterity,							///< 身法
	PET_ATTR_Con,								///< 根骨
	PET_ATTR_Intellect,							///< 意志
	PET_ATTR_Spiritualism,						///< 灵性

	PET_ATTR_Hit,								///< 命中
	PET_ATTR_Dodge,								///< 闪避
	PET_ATTR_MAtkSpeed,							///< 吟唱速度
	PET_ATTR_PhysicsAtk,						///< 物理伤害
	PET_ATTR_PhysicsDef,						///< 物理防御
	PET_ATTR_MAtk,								///< 魔法攻击
	PET_ATTR_MDef,								///< 魔法防御

	PET_ATTR_ElemDef0,							///< 水抗
	PET_ATTR_ElemDef1,							///< 土抗
	PET_ATTR_ElemDef2,							///< 木抗
	PET_ATTR_ElemDef3,							///< 金抗
	PET_ATTR_ElemDef4,							///< 火抗

	PET_ATTR_Race,								///< 种族
	PET_ATTR_TakeLev,							///< 携带等级
	PET_ATTR_StateType,							///< 状态类型
	PET_ATTR_FightType,							///< 战斗模式
	PET_ATTR_MoveType,							///< 移动模式
	PET_ATTR_Country,							///< 国家
	PET_ATTR_LoyLev,							///< 忠诚度等级

	PET_ATTR_Wild,								///< 野生标志
	PET_ATTR_Ages,								///< 代数
	PET_ATTR_Quality,							///< 品质
	PET_ATTR_Speciality,						///< 特性偏好
	PET_ATTR_DAfterValCount,					///< 后天资质强化次数
	PET_ATTR_StarLev,							///< 星等
	PET_ATTR_StarLevCount,						///< 星等强化次数

	PET_ATTR_talent_Strenth,
	PET_ATTR_talent_Dexterity,
	PET_ATTR_talent_Con,
	PET_ATTR_talent_Intellect,
	PET_ATTR_talent_Spiritualism,

	PET_ATTR_dAfter_Strenth,
	PET_ATTR_dAfter_Dexterity,
	PET_ATTR_dAfter_Con,
	PET_ATTR_dAfter_Intellect,
	PET_ATTR_dAfter_Spiritualism,

	PET_ATTR_syncretize_Strenth,
	PET_ATTR_syncretize_Dexterity,
	PET_ATTR_syncretize_Con,
	PET_ATTR_syncretize_Intellect,
	PET_ATTR_syncretize_Spiritualism,

	PET_ATTR_stoneUp_Strenth,
	PET_ATTR_stoneUp_Dexterity,
	PET_ATTR_stoneUp_Con,
	PET_ATTR_stoneUp_Intellect,
	PET_ATTR_stoneUp_Spiritualism,

	PET_ATTR_savvy_Strenth,
	PET_ATTR_savvy_Dexterity,
	PET_ATTR_savvy_Con,
	PET_ATTR_savvy_Intellect,
	PET_ATTR_savvy_Spiritualism
};

/// 宠物基础属性
enum PET_BASE_ATTR
{
	PET_BASE_STRENTH,								///< 力量
	PET_BASE_DEXTERITY,								///< 身法
	PET_BASE_CON,									///< 根骨
	PET_BASE_INTELLECT,								///< 意志
	PET_BASE_SPIRITUALISM,							///< 灵性
	PET_BASE_NUM,									///< 宠物基础属性数量
};

/// 侍从品质
enum PET_QUALITY
{
	PET_QUALITY_ORDINARILY,							///< 品质普通
	PET_QUALITY_CHOICENESS,							///< 品质优良
	PET_QUALITY_EXCELLENCE,							///< 品质优秀
	PET_QUALITY_BRILLIANT,							///< 品质卓越
	PET_QUALITY_NUM,								///< 品质数量
};

/// 侍从技能类型
enum PET_SKILL_TYPE
{
	PET_SKILL_ORDINARILY,							///< 普通攻击技能
	PET_SKILL_GENIUS,								///< 天赋技能
	PET_SKILL_HEAVEN,								///< 天系技能
	PET_SKILL_TERRA,								///< 地系技能
	PET_SKILL_HUMAN,								///< 人统技能
	PET_SKILL_NUM,									///< 技能类型数量
};

/// 侍从种族
enum PET_RACE
{
	PP_HUMAN,									///< 人类
	PP_GRASS,									///< 草本
	PP_WORM,									///< 虫
	PP_CHAOS,									///< 混沌
	PP_MACHINE,									///< 机关
	PP_DRAGON,									///< 龙
	PP_ARHAT,									///< 罗汉
	PP_GHOST,									///< 亡魂
	PP_PERI,									///< 仙灵
	PP_SHURA,									///< 修罗
	PP_LUCIFER,									///< 妖魔
	PP_BEAST,									///< 妖兽
	PP_FISH,									///< 鱼贝
	PP_ELEMENT,									///< 元素
	PP_NUM,										///< 侍从种族数量
};

/// 宠物类型（目前仅有侍从一种类型）
enum PET_TYPE
{
	PET_TYPE_NULL		= 0x00,
	//PET_TYPE_ELF		= 0x01,					///< 精灵
	//PET_TYPE_HUNTER,							///< 战宠
	//PET_TYPE_CONJURED,						///< 召唤宠
	//PET_TYPE_TRAP,							///< 陷阱
	//PET_TYPE_ORNA,							///< 饰宠
	PET_TYPE_SERVANT,							///< 侍从
	PET_TYPE_COUNT,								///< 宠物种类数量
	PET_TYPE_ALL		= 0xFF,					///< 所有类型
};

/// 宠物移动模式
enum PET_MOVE_STYLE
{
	PET_MOVETYPE_NULL		= 0x00,				///< 无移动模式
	PET_MOVETYPE_STOP		= 0x01,				///< 停留
	PET_MOVETYPE_FOLLOW		= 0x02,				///< 跟随
};

/// 宠物战斗模式（AI模式）
enum PET_FIGHT_STYLE
{
	PET_FIGHETYPE_NULL		= 0x00,				///< 无战斗模式
	PET_FIGHETYPE_DEFEND	= 0x01,				///< 防御模式
	PET_FIGHETYPE_FIGHT		= 0x02,				///< 协助攻击模式
};

/// 宠物状态
enum PET_STATE
{
	PET_STATE_SLEEP		= 0x00,					///< 沉睡状态
	PET_STATE_AWAIT		= 0x01,					///< 待机状态
	PET_STATE_FIGHT		= 0x02,					///< 出战状态
	PET_STATE_ACTIVE	= 0x03,					///< 激活状态
};

/// 宠物激活状态
//enum PET_ACTIVE_STATE
//{
//	PET_AS_UNACTIVE		= 0x00,					///< 未激活状态
//	PET_AS_ACTIVE		= 0x01,					///< 激活状态
//};

//enum PET_SKILL_TYPE
//{
//	PET_SKILL_NULL		= 0x0000,				// 未启用
//	PET_SKILL_RED		= 0x0001,				// 红色攻击技能
//	PET_SKILL_YELLOW	= 0x0002,				// 黄色防御技能
//	PET_SKILL_BLUE		= 0x0003,				// 蓝色高级技能
//	PET_SKILL_GENIUS	= 0x0010,				// 天赋技能
//	PET_SKILL_ENCHASED	= 0x1000,				// 已镶嵌
//};

enum PET_OT_AI_TYPE
{
	PET_OT_AI_SET_TARGET	= 0x80,				// 自动寻敌
	PET_OT_AI_ADD_SKILL		= 0x81,				// 技能操作
};

/// 宠物强化类型ID
enum PET_ELF_UPGRADE_ID
{
	PET_QUALITYUP_ID		=1000,				///< 品质强化ID
	PET_SAVVY_RESET_ID		=2000,				///< 悟性点重置ID
	PET_FSTONEUP_ID			=3000,				///< 魂石强化ID
	PET_STARTUP_ID			=4000,				///< 星等强化ID
	PET_TALENT_RESET_ID		=5000,				///< 天资重置ID
	PET_DAFTERUP_ID			=6000,				///< 后天资质强化ID
	PET_DAFTER_RESET_ID		=7000,				///< 后天资质强化次数重置ID
	PET_FFUP_ID				=8000,				///< 感悟神位强化ID
};


/// 宠物技能
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

	bool		bAuto;				///< 是否自动释放
	long		lID;				///< 编号
	long		lType;				///< 技能类型
	long		lPos;				///< 技能位置
	long		lLevel;				///< 等级
	long		lEndCooldown;		///< 冷却结束时间
	long		lActPos;			///< 动作条上的位置
};

// 宠物战斗属性/可变属性/显示属性
struct tagPetDisp
{
	tagPetDisp(void)
		: lMaxHp(0)
		, lMaxMp(0)
		, lMaxEnergy(0)
		, lStrenth(0)
		, lDexterity(0)
		, lCon(0)
		, lIntellect(0)
		, lSpiritualism(0)
		, lHit(0)
		, lDodge(0)
		, lMAtkSpeed(0)
		, lPhysicsAtk(0)
		, lPhysicsDef(0)
		, lMAtk(0)
		, lMDef(0)
		, lElemDef0(0)
		, lElemDef1(0)
		, lElemDef2(0)
		, lElemDef3(0)
		, lElemDef4(0)
	{
	}

	const tagPetDisp& operator=(const tagPetDisp& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lMaxHp;								///< 生命
	long		lMaxMp;								///< 法力
	long		lMaxEnergy;							///< 内力
	long		lStrenth;							///< 力量
	long		lDexterity;							///< 身法
	long		lCon;								///< 根骨
	long		lIntellect;							///< 意志
	long		lSpiritualism;						///< 灵性
	long		lHit;								///< 命中
	long		lDodge;								///< 闪避
	long		lMAtkSpeed;							///< 吟唱速度
	long		lPhysicsAtk;						///< 物理伤害
	long		lPhysicsDef;						///< 物理防御
	long		lMAtk;								///< 魔法攻击
	long		lMDef;								///< 魔法防御
	long		lElemDef0;							///< 水抗
	long		lElemDef1;							///< 土抗
	long		lElemDef2;							///< 木抗
	long		lElemDef3;							///< 金抗
	long		lElemDef4;							///< 火抗
};


/// 宠物基本属性（一级属性），对于可培养系需进行数据库存储
struct tagPetBaseAttr
{
	tagPetBaseAttr():lStrenth(0)
		,lDexterity(0)
		,lCon(0)
		,lIntellect(0)
		,lSpiritualism(0)
	{}

	const tagPetBaseAttr& operator=(const tagPetBaseAttr& eTag)
	{
		memcpy(this, &eTag, sizeof(eTag));
		return *this;
	}

	long GetTotalVal() const
	{
		return lStrenth + lDexterity + lCon + lIntellect +lSpiritualism;
	}

	void ReSet()
	{
		lStrenth = lDexterity = lCon = lIntellect = lSpiritualism = 0;
	}

	void SetElfBase(const tagPetBaseAttr& eTag)
	{
		lStrenth += eTag.lStrenth;
		lDexterity += eTag.lDexterity;
		lCon += eTag.lCon;
		lIntellect += eTag.lIntellect;
		lSpiritualism += eTag.lSpiritualism;
	}

	long		lStrenth;				///< 力量
	long		lDexterity;				///< 身法
	long		lCon;					///< 根骨
	long		lIntellect;				///< 意志
	long		lSpiritualism;			///< 灵性
};

/// 宠物战斗属性（二级属性），可通过基本属性（一级属性）计算，不需要数据进行存储
struct tagPetAtkAttr
{
	tagPetAtkAttr():lHit(0)
		,lDodge(0)
		,lMAtkSpeed(0)
		,lPhysicsAtk(0)
		,lPhysicsDef(0)
		,lMAtk(0)
		,lMDef(0)
	{}

	const tagPetAtkAttr& operator=(const tagPetAtkAttr& eTag)
	{
		memcpy(this, &eTag, sizeof(eTag));
		return *this;
	}

	long		lHit;								///< 命中
	long		lDodge;								///< 闪避
	long		lMAtkSpeed;							///< 吟唱速度
	long		lPhysicsAtk;						///< 物理伤害
	long		lPhysicsDef;						///< 物理防御
	long		lMAtk;								///< 魔法攻击
	long		lMDef;								///< 魔法防御
};

/// 侍从基本属性，需要数据库进行存储
struct tagServantBase
{
	tagServantBase():lWild(0)
		, lAges(1)
		, lDLevel(0)
		, lDExp(0)
		, lQuality(0)
		, lSpeciality(0)
		, lDAfterValCount(0)
		, lFStoneUpgradeFlag(-1)
		, lFUpgradeCount(-1)
		, lStarLev(0)
		, lStarLevCount(0)
	{
	}

	const tagServantBase& operator=(const tagServantBase& eTag)
	{
		memcpy(this, &eTag, sizeof(eTag));
		return *this;
	}

	long 		lWild;					///< 是否野生
	long		lAges;					///< 当前代数
	long		lDLevel;				///< 道行等级
	long		lDExp;					///< 修为经验
	long		lQuality;				///< 品质
	long		lSpeciality;			///< 特性偏好
	long		lDAfterValCount;		///< 后天资质强化次数

	long		lFStoneUpgradeFlag;		///< 魂石强化栏位 和 顿悟神位  标志值 ( 0 , 1 , 2 , 3 , 4 )
	long		lFUpgradeCount;			///< 感悟神位的值，即强化成功的次数
	long		lStarLev;				///< 星等等级
	long		lStarLevCount;			///< 星等强化次数
	tagPetBaseAttr  talentVal;			///< 天资值
	tagPetBaseAttr	dAfterVal;			///< 后天资质强化属性
	tagPetBaseAttr  syncretizeVal;		///< 融合后增加的天资属性
	tagPetBaseAttr  stoneUpVal;			///< 魂石强化增加的附加属性
	tagPetBaseAttr	savvyVal;			///< 通过悟性点分配属性
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
		, lEnergy(0)
		, lLife(0)
		, lNameChgTimes(0)
		, lLoyalty(0)
		, lStateType(-1)
	{
		memset(szName, 0, sizeof(szName));
		memset(szHostName, 0, sizeof(szHostName));
	}

	const tagPetDiff& operator=(const tagPetDiff& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	long		lIndex;					///< 索引编号
	long		lPos;					///< 存放位置, 0为激活宠物
	long		lLevel;					///< 等级
	long		lExp;					///< 经验
	long		lHp;					///< 生命值
	long		lMp;					///< 魔法值
	long		lEnergy;				///< 内力值
	long		lLife;					///< 寿命
	long		lLoyalty;				///< 忠诚度
	long		lNameChgTimes;			///< 改名次数
	long		lStateType;				///< 状态类型（见@PET_STATE）
	char		szName[32];				///< 宠物名字
	char		szHostName[32];			///< 主人名字
	tagServantBase ServantAttr;			///< 侍从基本属性
};

// 宠物临时属性数据
struct tagPetDetail : public tagPetDisp
{
	tagPetDetail(void)
		: lMoveType(0)
		, lFightType(0)
		, lRace(-1)
		, lMaxExp(0)
		, lMaxDExp(0)
		, lExistTime(0)
		, lTakeLev(0)
		, lPetType(-1)
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
		lMaxEnergy += rTag.lMaxEnergy;
		lStrenth += rTag.lStrenth;
		lDexterity += rTag.lDexterity;
		lCon += rTag.lCon;
		lIntellect += rTag.lIntellect;
		lSpiritualism += rTag.lSpiritualism;
		lHit += rTag.lHit;
		lDodge += rTag.lDodge;
		lMAtkSpeed += rTag.lMAtkSpeed;
		lPhysicsAtk += rTag.lPhysicsAtk;
		lPhysicsDef += rTag.lPhysicsDef;
		lMAtk += rTag.lMAtk;
		lMDef += rTag.lMDef;
		lElemDef0 += rTag.lElemDef0;	
		lElemDef1 += rTag.lElemDef1;	
		lElemDef2 += rTag.lElemDef2;	
		lElemDef3 += rTag.lElemDef3;	
		lElemDef4 += rTag.lElemDef4;	

		return *this;
	}

	long		lMoveType;				///< 行走模式
	long		lFightType;				///< 战斗模式
	long		lRace;					///< 种族
	long		lMaxExp;				///< 等级经验上限
	long		lMaxDExp;				///< 修为经验上限
	long		lExistTime;				///< 生存时间
	long		lTakeLev;				///< 携带等级
	long		lPetType;				///< 宠物类型
	long		lNominateMode;			///< 推荐加点
};

// 逻辑操作数据(与宠物类型相关, 无需数据库存取)
struct tagPetTemp : public tagPetDetail
{
	tagPetTemp(void)
		: cSaveFlag(0)
		, lCountry(0)
		, lHpRecoverSpeed(0)
		, lMpRecoverSpeed(0)
		, lEnergyRecoverSpeed(0)
		, lSkillNum(0)
		, lLastUseSkillTime(0)
		, lLoyLev(-1)
	{
	}

	const tagPetTemp& operator=(const tagPetTemp& rTag)
	{
		memcpy(this, &rTag, sizeof(rTag));
		return *this;
	}

	// 修正属性值
	char		cSaveFlag;						///< 保存Flag
	long		lCountry;						///< 国家
	long		lHpRecoverSpeed;				///< 生命回复速度
	long		lMpRecoverSpeed;				///< 法力回复速度
	long		lEnergyRecoverSpeed;			///< 内力回复速度
	long		lSkillNum;						///< 技能数（包括天赋技能和后天学习技能）
	long		lLastUseSkillTime;				///< 最后使用技能时间
	long		lLoyLev;						///< 忠诚度等级
};


/// 侍从强化材料需求类型
enum eMaterialType
{
	MT_NULL		= 0x00,								///< 无类型
	MT_MUST		= 0x01,								///< 必须材料
	MT_OPTIONAL = 0x02,								///< 非必须材料
};

/// 强化材料结构
struct tagMaterial
{
	tagMaterial(void)
		:orderFlag(0)
		,goodsID("")
		,goodsNum(0)
		,rate(0)
	{}

	tagMaterial(long flag, string goodsIdx, long goodsNum, long rate)
		:orderFlag(flag)
		,goodsID(goodsIdx)
		,goodsNum(goodsNum)
		,rate(rate)
	{}

	long orderFlag;									///< 材料需求标志(必须材料&非必须材料)
	string goodsID;									///< 材料ID
	long goodsNum;									///< 材料数量
	long rate;										///< 机率(必须材料为0，非必须材料增加强化成功率)
};

/// 侍从强化单个材料结构（必须材料和非必须材料都使用该结构）
struct tagMaterialGoods
{
	tagMaterialGoods(void)
		:keyNum(0)
		,key("")
	{
		goodstable.clear();
	}
	int keyNum;										///< 材料键值数目				
	string key;										///< 材料键值
	vector<tagMaterial> goodstable;					///< 材料可选列表
};

/// 侍从强化材料列表
struct  tagMaterialList
{
	tagMaterialList(void)
	{
		goodsList.clear();
	}
	string explainText;								///< 强化描述文字
	vector<tagMaterialGoods> goodsList;				///< 材料列表
};

typedef map<long,tagMaterialList> MATERIAL_LIST;
typedef MATERIAL_LIST::iterator ITR_MATERIAL_LIST;

#endif