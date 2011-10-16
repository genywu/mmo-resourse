#pragma once

#include "shape.h"
#include "MoveShape.h"
#include "other\goodslist.h"

class CClientRegion;

class CGoods :	public CMoveShape
{
	friend class CGoodsList;
public:
	enum eAction
	{
		GOOD_DROP,
		GOOD_NORMAL,
		GOOD_EXCEPT
	};

	//附加属性全部类型
	enum GOODS_ADDON_PROPERTIES
	{
		//未知属性		
		GAP_UNKNOW				=0,
		//道具档次
		GAP_GOODS_CATEGORY		,//1
		//显示颜色
		GAP_COLOR				,//2
		//人物换装编号
		GAP_COSPLAYID           ,//3
		//装备道具特效1
		GAP_EQUIP_EFFECT1		,//4
		//装备道具特效2
		GAP_EQUIP_EFFECT2       ,//5
		//装备道具特效3						
		GAP_EQUIP_EFFECT3		,//6
		//武器类别
		GAP_WEAPON_CATEGORY		,//7
		//等级限制
		GAP_ROLE_MINIMUM_LEVEL_LIMIT			,//8
		//技能限制
		GAP_SKILL_LIMIT			,//9
		//职业限制
		GAP_REQUIRE_OCCUPATION	,//10
		//性别限制
		GAP_REQUIRE_GENDER			,//11
		//特殊性质设定
		GAP_PARTICULAR_ATTRIBUTE,//12
		//武器强度
		GAP_WEAPON_DAMAGE_LEVEL	,//13
		//最小攻击力
		GAP_MIN_ATTACK			,//14
		//最大攻击力
		GAP_MAX_ATTACK			,//15
		//物理防御力
		GAP_NOR_DEF				,//16
		//伤害吸收比例
		GAP_HURT_SUCK_PER		,//17
		//命中能力修正
		GAP_HIT_RATE_CORRECT	,//18
		//物理致命一击率
		GAP_NORATTACK_KILLRATE	,//19
		//闪避能力修正
		GAP_FLEE_CORRECT		,//20
		//格挡能力修正
		GAP_PARRY_CORRECT		,//21
		//招架能力修正
		GAP_BLOCK_CORRECT	    ,//22
		//魔法攻击
		GAP_FUMO_PROPERTY		,//23
		//魔法致命一击率
		GAP_MAGIC_KILLRATE		,//24
		//专注修正
		GAP_DEDICANT_CORRECT	,//25
		//施法速度
		GAP_MAGIC_SPEED			,//26
		//魔法防御
		GAP_MAGIC_DEF			,//27
		//火抗修正
		GAP_FIREDEF_CORRECT		,//28
		//水抗修正				
		GAP_WATERDEF_CORRECT	,//29
		//风抗修正
		GAP_WINDDEF_CORRECT		,//30
		//地抗修正
		GAP_EARTHDEF_CORRECT	,//31
		//神圣抗性修正
		GAP_HOLYDEF_CORRECT		,//32
		//黑暗抗性修正
		GAP_DARKDEF_CORRECT		,//33
		//意志抗性
		GAP_WILL_DEF			,//34
		//基本催眠抗性
		GAP_HYPNOSIS_DEF		,//35
		//基本中毒抗性
		GAP_POISON_DEF			,//36
		//基本定身抗性
		GAP_FIX_DEF				,//37
		//基本体质抗性
		GAP_CON_DEF				,//38
		//基本流血抗性
		GAP_BLED_DEF			,//39
		//基本击昏抗性
		GAP_DIZZY_DEF			,//40
		//基本麻痹抗性
		GAP_LULL_DEF			,//41
		//力量追加
		GAP_ADD_STR				,//42
		//敏捷追加				
		GAP_ADD_DEX				,//43
		//体质追加
		GAP_ADD_CON				,//44
		//智力追加
		GAP_ADD_INT				,//45
		//智慧追加
		GAP_ADD_WIS				,//46
		//精神追加
		GAP_ADD_SPI				,//47
		//生命上限修正
		GAP_MAXHP_CORRECT		,//48
		//HP恢复速度修正
		GAP_HP_RESUME_CORRECT	,//49
		//法力上限修正
		GAP_MAXMP_CORRECT		,//50
		//法力恢复速度修正
		GAP_MP_RESUME_CORRECT	,//51
		//增加HP
		GAP_ADD_HP				,//52
		//增加HP百分比
		GAP_ADD_PERCENT_HP		,//53
		//增加MP
		GAP_ADD_MP				,//54
		//增加MP百分比
		GAP_ADD_PERCENT_MP		,//55
		//道具最大耐久值
		GAP_GOODS_MAXIMUM_DURABILITY	,//56
		//道具最大叠加数
		GAP_MAXOVERLAP			,//57
		//学习技能的ID
		GAP_LEARN_SKILL			,//58
		//附带技能ID
		GAP_ADD_SKILL			,//59
		//吸收HP比
		GAP_LEECH_HP			,//60
		//吸收MP比
		GAP_LEECH_MP			,//61
		//损伤转MP比
		GAP_HURT2MP				,//62
		//击退
		GAP_KNOCK				,//63
		//回城
		GAP_HOME				,//64
		//随机传送
		GAP_RAN_TRANS			,//65
		//执行脚本
		GAP_SCRIPT				,//66
		//升级宝石类型
		GAP_GEM_TYPE			,//67
		//升级宝石级别
		GAP_GEM_LEVEL				,//68
		//升级宝石成功概率修正
		GAP_GEM_PROBABILITY		,//69
		//升级宝石成功结果
		GAP_GEM_UPGRADE_SUCCEED_RESULT	,//70
		//升级宝石失败结果		
		GAP_GEM_UPGRADE_FAILED_RESULT		,//71
		//坐骑类别
		GAP_MOUNT_TYPE			,//72
		//坐骑级别
		GAP_MOUNT_LEVEL			,//73
		//坐骑所需玩家角色限制
		GAP_MOUNT_PLAYER_ROLE_LIMIT	,//74
		//无限次使用
		GAP_UNLIMITED_ACCESS		,//75
		//异常状态
		GAP_EXCEPTION_STATE		,//76
		//提升EXP倍率
		GAP_EXP_BONUS			,//77
		//使用者ID
		GAP_EXPBALL_USERID				,//78
		//涵盖经验
		GAP_EXPBALL_EXP		,//79
		//抵扣元宝值
		GAP_YUANBAO_QUITS		,//80
		//记录场景ID
		GAP_RECORD_REGIONID		,//81
		//记录坐标ID
		GAP_RECORD_COOR			,//82
		//装备最大经验
		GAP_EQUIP_MAXEXP		,//83
		//套装属性ID
		GAP_SUITID				,//84
		//套装编号
		GAP_SUIT_NUMBER			,//85
		//制造者名字
		GAP_CREATOR_NAME		,//86
		//消失时间
		GAP_DEL_TIME			,//87
		//寿命
		GAP_LIFE				,//88
		//得到道具触发脚本
		GAP_GET_TRIGGER			,//89
		//失去道具触发脚本
		GAP_LOST_TRIGGER		,//90
		//基本孔数
		GAP_HOLE_NUM			,//91
		//镶嵌目标对象
		GAP_ENCHASE_TARGET		,//92
		//背包类型
		GAP_PACK_TYPE			,//93
		//背包格子
		GAP_PACK_CAPACITY		,//94
		//最小攻击力成长
		GAP_MIN_ATK_UP		,//95		
		//最大攻击力成长
		GAP_MAX_ATK_UP			,//97		
		//魔法攻击成长
		GAP_MAGIC_UP		,//100		
		//物理防御成长		
		GAP_DEF_UP			,//102		
		//魔法防御最高成长
		GAP_MAGICDEF_UP		,//104
		//命中修正成长
		GAP_HIT_RATE_UP			,//105
		//致命一击率成长
		GAP_KILL_RATE_UP		,//106
		//
		GAP_MAGIC_KILL_RATE_UP,
		//招架能力修正成长
		GAP_PARRY_UP			,//107
		//格挡能力修正成长
		GAP_BLOCK_UP			,//108
		//闪避能力
		GAP_FLEE_UP				,//109
		//道具最大耐久值成长值
		GAP_MAXDUR_UP			,//110
		//武器等级
		GAP_WEAPON_LEVEL		,//111
		//最小攻击修正百分比
		GAP_MINATK_MODIFY_PER	,//112
		//最大攻击修正百分比
		GAP_MAXATK_MODIFY_PER	,//113
		//普通防御修正百分比
		GAP_DEF_MODIFY_PER		,//114
		//魔法攻击修正百分比
		GAP_MAGICATK_MODIFY_PER	,//115
		//魔法防御修正百分比
		GAP_MAGICDEF_MODIFY_PER, //116
		//防具类别
		GAP_GUARD_EQUIP_CATEGORY,//117
		//装备时触发脚本
		GAP_EQUIP_TRIGGER, //118
		//卸装时触发脚本
		GAP_UNEQUIP_TRIGGER, //119
		//状态追加
		GAP_ADD_STATE,//120
		//冷却时间
		GAP_COOLDOWN,
		//双手武器图形ID
		GAP_BOTHHAND_GRAPHIC,
		//卡片类型
		GAP_CARD_CATEGORY,
		//卡片同一ID镶嵌数量限制
		GAP_CARD_ENCHASE_NUM_LIMIT,
		//物理暴击伤害
		GAP_CRI_DAMAGE,
		//物理暴击抗性
		GAP_CRI_DEF,
		//魔法暴击伤害
		GAP_MCRI_DAMAGE,
		//魔法暴击抗性
		GAP_MCRI_DEF,
		//! GoodsEx属性
		GAP_CAN_DISASSEMBLE,	//! 能否分解 125
		GAP_DISASSEMBLE_VALUE,	//! 分解价值
		GAP_QUALITY,			//! 物品品质

		GAP_CURR_POWER,			//! 当前充能值
		GAP_MAX_POWER,			//! 充能上限
		GAP_POWER_INTENSITY,	//! 充能强度
		GAP_MIN_REINFORCE_LEVEL,//! 等级下限
		GAP_MAX_REINFORCE_LEVEL,//! 等级上限

		GAP_BIND,				//! 绑定
		GAP_FREEZE,				//! 冻结 134
		GAP_THAWTIME,			//! 解冻时间

		GAP_ATK_ADD_RATIO,//物理攻击力加成系数
		GAP_MAGICATK_ADD_RATIO,//魔法攻击力加成系数
		GAP_WEAPON_INTEN_UP,//武器强度成长
		GAP_DEF_ADD_RATIO,//物理防御力加成系数
		GAP_MAGICDEF_ADD_RATIO,//魔法防御力加成系数
		GAP_MAX_ENERGY_COR,//能量上限修正
		GAP_ENERGY_RESUME_COR,//能量恢复速度修正
		GAP_ENERGY_SUCK,//吸收能量
		GAP_PEOPLE_HURT_RATIO,//对人形类怪物伤害加成
		GAP_ELEMENT_HURT_RATIO,//对元素类怪物伤害加成
		GAP_DIFFRACE_HURT_RATIO,//对异族类怪物伤害加成
		GAP_ANIM_HURT_RATIO,//对动物类怪物伤害加成
		GAP_SOUL_HURT_RATIO,//对亡灵类怪物伤害加成
		GAP_DEMON_HURT_RATIO,//对恶魔类怪物伤害加成
		GAP_INSECT_HURT_RATIO,//对昆虫类怪物伤害加成
		GAP_FIRE_HURT_RATIO,//对火系怪物伤害加成
		GAP_WATER_HURT_RATIO,//对水系怪物伤害加成
		GAP_EARTH_HURT_RATIO,//对土系怪物伤害加成
		GAP_WIND_HURT_RATIO,//对风系怪物伤害加成
		GAP_DARK_HURT_RATIO,//对暗系怪物伤害加成
		GAP_HOLINESS_HURT_RATIO,//对圣系怪物伤害加成
		GAP_GOODS_SELL_PRICE,//交易品买出价格
		GAP_GOODS_BUY_PRICE,//交易品基本价格

		GAP_PLUG_EVIL_LEVEL,	//! 装备魔化等级

		GAP_HURT_RID,	//物理免伤
		GAP_MAGIC_HURT_RID,//魔法免伤
		GAP_UPGRADE_PRICE,	//物品升级价格
		GAP_SUBPACK_TYPE,	//子背包类型
		GAP_TRADEGOODS_TYPE,//跑商物品类别
		GAP_SHIELD_DEF,//盾牌物理防御
		GAP_SHIELD_DEF_UP,//盾牌物理防御成长		
		GAP_TIME_LIMIT,	//限时
		GAP_ADDON_CHARM,//附加魅力
		GAP_EQUIP_DEGREE,// 装备等阶
		GAP_EQUIP_SEAL,//装备封印
		GAP_SEAL_ATTR,//封印属性
		GAP_GOODS_TYPE,//物品分类
		GAP_REFINE,//炼度
		GAP_CANNOT_SAVE_DEPOT,//不能存仓库
		GAP_HP_RESUME_RATE,//生命值恢复比例
		GAP_BUFF_ATK_STRENGTH,//状态攻击强度		

		GAP_RESERVE_CARD,//保留卡片
		GAP_UNIQUE,//唯一物品
		GAP_HIDE_HEAR,//是否隐藏头发
		GAP_HIDE_FACE,//是否隐藏脸部
		GAP_HIDE_BODY,//是否隐藏身体
		GAP_NORMAL_DEF_UP,//物理免伤成长
		GAP_MAGIC_DEF_UP,//魔法免伤成长
		GAP_BLUE_CRYSTAL,//分解蓝色晶体数目
		GAP_ATK_ED,//攻击ED范围值
		GAP_CUR_ATK_ED,//当前攻击ED值
		GAP_DEF_ED,//防御ED范围值
		GAP_CUR_DEF_ED,//当前防御ED值

		GAP_EXCUSE_HURT,//免伤
		GAP_PENETRATE,//穿透
		GAP_CONSTELLATION,// 星座
		GAP_LUCKY,//幸运
		GAP_SILENCE_DEF,//沉默抗性
		GAP_MEDAL_POINT,//勋章积分
		GAP_MIN_ATK_ADDON,// 小攻追加
		GAP_MAX_ATK_ADDON,// 大攻追加
		GAP_MAGIC_ATK_ADDON,//魔攻追加
		GAP_DEF_ADDON,//物防追加
		GAP_MAGIC_DEF_ADDON,//魔防追加

		GAP_SKILL_CALL_SCRIPT,//技能调用脚本

		GAP_SCRIPT_VAR1,	//脚本变量
		GAP_SCRIPT_VAR2,	//脚本变量
		GAP_SCRIPT_VAR3,	//脚本变量
		GAP_ADD_MAXHP_UP,	//生命上限追加成长
		GAP_ADD_MAXHP_LEVEL, //冲等追加生命上限

		GAP_EXCUSE_HURT_RANGE,	//免伤范围
		GAP_PENETRATE_RANGE,	//穿透范围
		GAP_LUCKY_RANGE,		//幸运范围
		GAP_ROULEAU_MAKE_GOODS,	//卷轴制作物品ID	
		GAP_ADDON_HP_RANGE,		//HP上线追加范围
		GAP_RANDOM_ADDON_MAXHP, //随机生命上限追加

		GAP_ADDON_PENETRATE,//穿透附加
		GAP_ADDON_EXCUSE_HURT,//免伤附加
		GAP_ADDON_LUCKY,//幸运附加
		GAP_ADDON_ATK_ED,//攻击ED附加
		GAP_ADDON_DEF_ED,//防御ED附加

		GAP_MOUNT_POTENTIAL,//坐骑潜能
		GAP_MOUNT_MAXHP,//坐骑生命上限追加
		GAP_MOUNT_MIN_ATK,//坐骑物理小攻追加
		GAP_MOUNT_MAX_ATK,//坐骑物理大攻追加
		GAP_MOUNT_MAGIC_ATK,//坐骑魔法攻击追加
		GAP_MOUNT_PENETRATE,//坐骑穿透追加
		GAP_MOUNT_EXCUSE_HURT,//坐骑免伤追加

		GAP_PROPERTY_TRIGGER,	//控制属性激发的属性
		GAP_WRITEPROPERTY_ID, //用脚本向物品的m_strManufacturerName写入的字符串的ID，用于显示活动信息等
		GAP_ADD_MAX_PKCP=226,//增加PK CP值上限
		GAP_ADD_MAX_PKCPRANGE,///增加的PK CP值范围
		GAP_TIMES_MAX_PKCP, //增加PK CP上限值倍率系数（乘法）
		GAP_TIMES_MAX_PKCPRANGE,//倍率随机范围 
		GAP_DATE_FOR_SCRIPT, //! 脚本日期，为脚本提供的时间保存栏位，值的意义由脚本定义

		// 王峰：魔化相关属性
		GAP_ELEMENT_1,//! 物品魔化属性，值代表魔化属性，值表示魔化等级
		GAP_ELEMENT_2,//! 物品魔化属性，值代表魔化属性，值表示魔化等级
		GAP_ELEMENT_3,//! 物品魔化属性，值代表魔化属性，值表示魔化等级
		GAP_ELEMENT_XIANG_XING_1,//! 魔化相性
		GAP_ELEMENT_XIANG_XING_2,//! 魔化相性
		GAP_ELEMENT_XIANG_XING_3,//! 魔化相性

		GAP_ENERGY_MAX_GUARDIAN, //最大星座守护石能量
		GAP_GUARDIAN_ENERGY_DEC, //星座能量值降低因子
		GAP_FBUSINESS_PRICE, //家族跑商商品价格

		GAP_AMOUNT,
	};

	//基本属性
	struct tagBaseProperty
	{
		string	strName;		//	中文名称 < 20
		DWORD	dwValue;		//	金币价格
		DWORD	dwSilverPrice;	//	银币价格
		DWORD	dwBuyPrice;		//  物品买入价格（商业系统使用）
		DWORD	dwPriceType;	//  价格类型
		DWORD	dwType;			//	物品类型

		tagBaseProperty()
		{
			strName = "";
			dwValue = 0;
			dwSilverPrice = 0;
			dwBuyPrice = 0;
			dwPriceType = 0;
			dwType = 0;
		}
	};

	//卡片属性
	struct tagCardProperty
	{
		DWORD  dwCardIndex;			//  卡片的物编号
		CGUID  CardGUID;			//	卡片的GUID
		string strCardName;			//	卡片的名字
		DWORD  dwCardType;			//	卡片的类型
		DWORD  dwInlayEquipType;	//	卡片适应的装备类型
		BYTE   bInlayEquipLevel;	//	卡片适应的装备级别
		BYTE   bMaxNum;				//	此卡片在同一装备最大可使用数量
		bool   bIsSeriesCard;		//此卡是否为套卡

		vector<CGoodsList::tagEffect>  CardEffect;	//卡片的附加属性
		tagCardProperty()
		{
			dwCardIndex = -1;
			CardGUID = CGUID::GUID_INVALID;
			strCardName = "";
			dwCardType = 0;
			dwInlayEquipType = 0;
			bInlayEquipLevel = 0;
			bMaxNum = 0;
			bIsSeriesCard = false;
		}
		tagCardProperty(const tagCardProperty& src)
		{
			dwCardIndex = src.dwCardIndex;
			CardGUID = src.CardGUID;
			strCardName = src.strCardName;
			dwCardType = src.dwCardType;
			dwInlayEquipType = src.dwInlayEquipType;
			bInlayEquipLevel = src.bInlayEquipLevel;
			bMaxNum = src.bMaxNum;
			bIsSeriesCard = src.bIsSeriesCard;
			CGoodsList::tagGoods2* pstGoods=CGoodsList::GetProperty(dwCardIndex);
			if (pstGoods)
			{
				vector<CGoodsList::tagEffect>::iterator EffectIter=pstGoods->vectorEffect.begin();
				for(;EffectIter!=pstGoods->vectorEffect.end();EffectIter++)
				{
					CardEffect.push_back(*EffectIter);
				}
			}
		}
	};

	CGoods(void);
	CGoods(DWORD dwIndexo);
	CGoods(const CGoods &srcGoods);
	virtual ~CGoods(void);

	DWORD GetIndex(){return m_dwIndex;}
	void  SetIndex(DWORD dwVal){m_dwIndex=dwVal;}

	void SetNum(long lVal){m_lNum=lVal;}
	long GetNum(){return m_lNum;}

	const char* GetOrginName(){return GetProperty()->strOrginName.c_str();}

	VOID SetGoodsBaseType(eGoodsBaseType type){m_GoodsBaseType=type;}
	DWORD GetGoodsBaseType();

	vector<CGoodsList::tagEffect>*	GetEffect()				{return &m_vectorEffect;}

	tagBaseProperty* GetBaseProperty()						{return &m_stBaseProperty;}
	void SetBaseProperty(tagBaseProperty *pstBaseProperty)	{m_stBaseProperty=*pstBaseProperty;}

	CGoodsList::tagBaseProperty* GetProperty()	{return &(CGoodsList::GetProperty(m_dwIndex)->BaseProperty);}

	bool AddToByteArray(vector<BYTE>*,bool bExtData=true);
	// 解码
	bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData=true);
	bool DecordFromDataBlock(DBReadSet& readDB,bool bExData = true );

	////////////XY 2007.11.13///////////////
	void UpdateProperty(CMessage* pMsg);
	////////////////////////////////////////

	long GetEffectVal(WORD wType,int nWhich);
	void SetEffectVal(WORD wType,int nWhich,long lVal);
	void SetNewEffectVal(WORD wType,int nWhich,long lVal);		//给物品添加新的附加属性
	//long GetFixVal(WORD wType,int nWhich);
	bool CanRepair();
	bool CanUpgrade();

	bool HasEffect( WORD wType );
	bool IsEffectChange();
	//判断物品是否可以叠加
	static bool isGoodsCanAccumulate(CGoods *pOnGroundGoods,CPlayer* pPlayer);
	DWORD RepairPrice();
	DWORD GetUpdatePrice();
	DWORD GetPrice();
	DWORD GetPriceType();
	DWORD GetSalePrice();

	//void SetNameColor(DWORD dwColor);
	//DWORD GetNameColor();
	float GetHeight();
	void  SetDropEffect();
	BOOL  GetDropEffect() {return m_bDropEffect;}


	//物品的镶嵌孔接口
	bool Stiletto();						//给物品打孔的接口
	bool PlugUpHole(BYTE index,WORD wHoleNum);								//封堵镶嵌孔
	bool EnchaseCard(BYTE index,tagCardProperty card);			//镶嵌卡片
	bool RemoveCard(BYTE index);								//拆除卡片
	void SetHoleNum(WORD wNum);									//设置物品的当前已开的孔数
	WORD GetHoleNum()	{return m_wHoleNum;}					//获取物品的当前已开的孔数
	WORD GetCardNum()	{return (WORD)m_InlayHole.size();}		//返回物品上已镶嵌的卡片数
	map<WORD,tagCardProperty>& GetInlayHole() {return m_InlayHole;}	//返回物品上的镶嵌孔信息
	bool GetHoleState(BYTE HoleIndex);							//获取物品指定位置的镶嵌孔的状态（开、关）
	void SetHoleState(BYTE HoleIndex,bool bState);				//设置物品指定位置的镶嵌孔的状态（开、关）

	// 锁定操作
	void Lock();
	void UnLock();
	bool IsLocked();
	////////XY 2008.1.3////////
	bool IsHaveShadow()	{return m_bHaveShadow;}
	void SetHaveShadowState(bool bState)	{m_bHaveShadow = bState;}

	//* add by MartySa
	/// 获取制造者的名字
	const string& GetManufacturerName() { return m_strManufacturerName; }
	/// 设置制造者的名字
	void SetManufacturerName(const char* pStrName);
	///////////////////////////
private:
	DWORD	m_dwIndex;						// 编号
	long	m_lNum;
	eGoodsBaseType m_GoodsBaseType;
	bool	m_bHaveShadow;					//此物品是否有虚影

	///////商业系统中所需属性（特殊的NPC物品才有此属性）///////
	DWORD	m_dwPurchaseLimit;				//限购额
	///////////////////////////////////////////////////////////

	tagBaseProperty m_stBaseProperty;		//物品基本属性
	vector<CGoodsList::tagEffect>	m_vectorEffect;		// 扩展属性列表

	///////////镶嵌孔属性//////////
	map<WORD,tagCardProperty> m_InlayHole;	//镶嵌孔（只有武器或防具等物品可能具有）
	WORD	m_wHoleNum;						//此物品当前已开的镶嵌孔数
	map<WORD,bool> m_mapHoleState;			//物品当前各个孔的开孔状态
	///////////////////////////////

	//CEffect*		m_pFlashEffect;			//闪光特效
	//DWORD			m_dwNameColor;			//名字颜色

	//物品掉落的效果
	BOOL  m_bDropEffect;
	DWORD m_dwDropTimeStart;
	float m_fDropHeight;

	// 锁定标志
	long	m_lLockCounter;

	// 制造者名字(add by MartySa)
	string  m_strManufacturerName;
};
