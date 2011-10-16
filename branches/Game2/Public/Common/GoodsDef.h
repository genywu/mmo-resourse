/** 
*  @file   GoodsDef.h
*  @author
*
*  @brief  物品相关结构定义
*  @date   2010/09/03
*/

#pragma once

/// 物品类型
enum eGoodsBaseType
{
	GBT_USELESS,				///< 不能用
	GBT_NORMAL,					///< 普通
	GBT_CONSUMABLE,				///< 消耗性道具
	GBT_CARD,					///< 卡片
	GBT_GOODS,					///< 商品
	GBT_SUBPACK,				///< 子背包
	GBT_PICKGOODS,				///< 拾取品
	GBT_MEDAL,                  ///< 勋章(称号)
	GBT_MONEY,	                ///< 金币
	GBT_SILVERMONEY,	        ///< 银币
	GBT_EQUIPMENT,				///< 装备
	GBT_TRUMP,					///< 法宝
};

// 装备类型
enum eEquipType
{
	// 装备类
	EC_E_HEAD,				  // 头盔1
	EC_E_NECKLACE,			  // 项链
	EC_E_WING,				  // 翅膀
	EC_E_BODY,				  // 盔甲
	EC_E_BACK,				  // 腰带5
	EC_E_HAND,                // 手部
	EC_E_SHOULDER,            // 肩部
	EC_E_WRIST,			      // 手腕	
	EC_E_LEG,                 // 腿部
	EC_E_BOOT,			      // 鞋子10	
	EC_E_LRING,			      // 左戒指
	EC_E_RRING,			      // 右戒指		
	EC_E_WEAPON,			  // 武器
	EC_E_CARD,			      // 护符
	EC_E_GODGOODS,            // 圣物15
	EC_E_EFFIGY,              // 雕像
	EC_E_LIFECOWRY,           // 本命法宝
	EC_E_SOULGUARD,           // 魂守
	EC_E_RIDER,               // 坐骑19   


	// 时装类    
	EC_P_HEADGEAR,			// 头饰
	EC_P_FROCK,			    // 外套	
	EC_P_WRIST,             // 手腕
	EC_P_LOIN,              // 腰
	EC_P_NECK,              // 颈
	EC_P_AMULET,            // 护符
	EC_P_FOOT,              // 脚
	EC_P_LEG,               // 腿

	//称号类
	EC_MEDAL1,			//	称号1
	EC_MEDAL2,			//	称号2
	EC_MEDAL3,			//	称号3

	EC_TOTAL
};

/// 装备类别
enum eEquipClass
{
	EC_NONE,					///< 都不是
	EC_ATTACK,					///< 武器
	EC_DEFENSE					///< 防具
};

/// 卡片类型
enum eCardType
{
	CARD_CAN_ENCHASE=1,			///< 镶嵌型
	CARD_ENHANCE_ATTACK,		///< +攻击
	CARD_ENHANCE_DEFEND,		///< +防御

	CARD_CANNOT_ENCHASE,		///< 非镶嵌型
	CARD_COLLECT				///< 搜集型
};

/// ICON类别
enum eIconType
{
	IT_CONTAINER,				///< 容器内
	IT_GROUND,					///< 地面上
	IT_EQUIPED					///< 装备后
};

/// 声音类别
enum eSoundType
{
	ST_PICKUP,					///< 拾取
	ST_DROP,					///< 丢弃
	ST_USE,						///< 使用
	ST_EQUIP,					///< 装备
	ST_FLOURISH,				///< 挥动,未命中
	ST_SLASH					///< 命中,特殊攻击
};

/// 法宝栏位类型
enum eTrumpType
{
	TT_GOD,					///< 天字栏位8个
	TT_GLAND,				///< 地字栏位8个
	TT_HUMAN,				///< 人字栏位8个
	TT_NUM					///< 法宝栏位个数
};


/// 物品附加属性类型
enum eGoodsAddonProperties
{
	//////////////////////////////////////////////////////////////////////////
	GAP_UNKNOW=0,				//0 未知属性
	GAP_GOODS_CATEGORY,			//1 道具档次
	GAP_COLOR,					//2 显示颜色
	GAP_COSPLAYID,				//3 人物换装编号
	GAP_EQUIP_EFFECT1,			//4 装备道具特效1
	GAP_EQUIP_EFFECT2,			//5 装备道具特效2
	GAP_EQUIP_EFFECT3,			//6 装备道具特效3
	GAP_WEAPON_CATEGORY,		//7 武器类别
	GAP_ROLE_LEVEL_LIMIT,		//8 等级限制
	GAP_SKILL_LIMIT			,//9技能限制
	GAP_REQUIRE_OCCUPATION	,//10职业限制
	GAP_REQUIRE_GENDER			,//11性别限制
	GAP_PARTICULAR_ATTRIBUTE,//12特殊性质设定
	GAP_WEAPON_DAMAGE_LEVEL	,//13武器强度
	GAP_MIN_ATTACK			,//14最小攻击力
	GAP_MAX_ATTACK			,//15最大攻击力
	GAP_NOR_DEF				,//16物理防御力
	GAP_HURT_SUCK_PER		,//17伤害吸收比例
	GAP_HIT_RATE_CORRECT	,//18命中能力修正
	GAP_NORATTACK_KILLRATE	,//19物理致命一击率
	GAP_FLEE_CORRECT		,//20闪避能力修正
	GAP_PARRY_CORRECT		,//21格挡能力修正
	GAP_BLOCK_CORRECT	    ,//22招架能力修正
	GAP_FUMO_PROPERTY		,//23魔法攻击
	GAP_MAGIC_KILLRATE		,//24魔法致命一击率
	GAP_DEDICANT_CORRECT	,//25专注修正
	GAP_MAGIC_SPEED			,//26施法速度
	GAP_MAGIC_DEF			,//27魔法防御
	GAP_FIREDEF_CORRECT		,//28火抗修正
	GAP_WATERDEF_CORRECT	,//29水抗修正
	GAP_WINDDEF_CORRECT		,//30风抗修正
	GAP_EARTHDEF_CORRECT	,//31地抗修正
	GAP_HOLYDEF_CORRECT		,//32神圣抗性修正
	GAP_DARKDEF_CORRECT		,//33黑暗抗性修正
	GAP_WILL_DEF			,//34意志抗性
	GAP_HYPNOSIS_DEF		,//35基本催眠抗性
	GAP_POISON_DEF			,//36基本中毒抗性
	GAP_FIX_DEF				,//37基本定身抗性
	GAP_CON_DEF				,//38基本体质抗性
	GAP_BLED_DEF			,//39基本流血抗性
	GAP_DIZZY_DEF			,//40基本击昏抗性
	GAP_LULL_DEF			,//41基本麻痹抗性
	GAP_ADD_STR				,//42力量追加
	GAP_ADD_DEX				,//43敏捷追加
	GAP_ADD_CON				,//44体质追加
	GAP_ADD_INT				,//45智力追加
	GAP_ADD_WIS				,//46智慧追加
	GAP_ADD_SPI				,//47精神追加
	GAP_MAXHP_CORRECT		,//48生命上限修正
	GAP_HP_RESUME_CORRECT	,//49HP恢复速度修正
	GAP_MAXMP_CORRECT		,//50法力上限修正
	GAP_MP_RESUME_CORRECT	,//51法力恢复速度修正
	GAP_ADD_HP				,//52增加HP
	GAP_ADD_PERCENT_HP		,//53增加HP百分比
	GAP_ADD_MP				,//54增加MP
	GAP_ADD_PERCENT_MP		,//55增加MP百分比
	GAP_GOODS_MAX_DURABILITY	,//56道具最大耐久值
	GAP_MAXOVERLAP			,//57道具最大叠加数
	GAP_LEARN_SKILL			,//58学习技能的ID
	GAP_ADD_SKILL			,//59附带技能ID
	GAP_LEECH_HP			,//60吸收HP比
	GAP_LEECH_MP			,//61吸收MP比
	GAP_HURT2MP				,//62损伤转MP比
	GAP_KNOCK				,//63击退
	GAP_HOME				,//64回城
	GAP_RAN_TRANS			,//65随机传送
	GAP_SCRIPT				,//66执行脚本
	GAP_GEM_TYPE			,//67升级宝石类型
	GAP_GEM_LEVEL				,//68升级宝石级别
	GAP_GEM_PROBABILITY		,//69升级宝石成功概率修正
	GAP_GEM_UPGRADE_SUCCEED_RESULT	,//70升级宝石成功结果
	GAP_GEM_UPGRADE_FAILED_RESULT		,//71升级宝石失败结果	
	GAP_MOUNT_TYPE			,//72坐骑类别
	GAP_MOUNT_LEVEL			,//73坐骑级别
	GAP_MOUNT_ROLE_LIMIT	,//74坐骑所需玩家角色限制
	GAP_UNLIMITED_ACCESS		,//75无限次使用
	GAP_EXCEPTION_STATE		,//76异常状态
	GAP_EXP_BONUS			,//77提升EXP倍率
	GAP_EXPBALL_USERID				,//78使用者ID
	GAP_EXPBALL_EXP		,//79涵盖经验
	GAP_YUANBAO_QUITS		,//80抵扣元宝值
	GAP_RECORD_REGIONID		,//81记录场景ID
	GAP_RECORD_COOR			,//82记录坐标ID
	GAP_EQUIP_MAXEXP		,//83装备最大经验
	GAP_SUITID				,//84套装属性ID
	GAP_SUIT_NUMBER			,//85套装编号
	GAP_CREATOR_NAME		,//86制造者名字
	GAP_DEL_TIME			,//87消失时间
	GAP_LIFE				,//88寿命
	GAP_GET_TRIGGER			,//89得到道具触发脚本
	GAP_LOST_TRIGGER		,//90失去道具触发脚本
	GAP_HOLE_NUM			,//91基本孔数
	GAP_ENCHASE_TARGET		,//92镶嵌目标对象
	GAP_PACK_TYPE			,//93背包类型
	GAP_PACK_CAPACITY		,//94背包格子
	GAP_MIN_ATK_UP		,//95	最小攻击力成长	
	GAP_MAX_ATK_UP			,//97		最大攻击力成长
	GAP_MAGIC_UP		,//100		魔法攻击成长
	
	GAP_DEF_UP			,//102		物理防御成长
	GAP_MAGICDEF_UP		,//104魔法防御最高成长
	GAP_HIT_RATE_UP			,//105命中修正成长
	GAP_KILL_RATE_UP		,//106致命一击率成长
	GAP_MAGIC_KILL_RATE_UP,
	GAP_PARRY_UP			,//107招架能力修正成长
	GAP_BLOCK_UP			,//108格挡能力修正成长
	GAP_FLEE_UP				,//109闪避能力
	GAP_MAXDUR_UP			,//110道具最大耐久值成长值
	GAP_WEAPON_LEVEL		,//111武器等级
	GAP_MINATK_MODIFY_PER	,//112最小攻击修正百分比
	GAP_MAXATK_MODIFY_PER	,//113最大攻击修正百分比
	GAP_DEF_MODIFY_PER		,//114普通防御修正百分比
	GAP_MAGICATK_MODIFY_PER	,//115魔法攻击修正百分比
	GAP_MAGICDEF_MODIFY_PER, //116魔法防御修正百分比
	GAP_GUARD_EQUIP_CATEGORY,//117防具类别
	GAP_EQUIP_TRIGGER, //118装备时触发脚本
	GAP_UNEQUIP_TRIGGER, //119卸装时触发脚本
	GAP_ADD_STATE,//120状态追加
	GAP_COOLDOWN,//冷却时间
	GAP_BOTHHAND_GRAPHIC,//双手武器图形ID
	GAP_CARD_CATEGORY,//卡片类型
	GAP_CARD_ENCHASE_NUM_LIMIT,//卡片同一ID镶嵌数量限制
	GAP_CRI_DAMAGE,//物理暴击伤害
	GAP_CRI_DEF,//物理暴击抗性
	GAP_MCRI_DAMAGE,//魔法暴击伤害
	GAP_MCRI_DEF,//魔法暴击抗性
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
	GAP_GOODSDESC_RES_ID, //用脚本向物品的m_strManufacturerName写入的字符串的ID，用于显示活动信息等
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

	GAP_STARLEVEL,			// 星等
	GAP_GOODS_USE_LIMIT,	// 物品冲等时的被使用限制
	GAP_EQUIP_CATEGORY,		// 装备类别
	GAP_TRUMP_CATEGORY,		// 法宝类别

	//////////////////////////////////////////////////////////////////////////

	GAP_TATAL_NUM,

	//////////////////////////////////////////////////////////////////////////
	GAP_BASE_HP_UPPER_LIMIT,		///< 基本气血上限
	GAP_BASE_MP_UPPER_LIMIT,		///< 基本法力上限
	GAP_BASE_EP_UPPER_LIMIT,		///< 基本内力上限
	GAP_BASE_STAMINA_UPPER_LIMIT,	///< 基本怒气上限

	GAP_BASE_HP_RECOVER,			///< 基本气血恢复速度
	GAP_BASE_MP_RECOVER,			///< 基本法力恢复速度
	GAP_BASE_EP_RECOVER,			///< 基本内力恢复速度
	GAP_BASE_STAMINA_RECOVER,		///< 基本怒气恢复速度

	GAP_FIGHT_HP_RECOVER,			///< 基本战斗气血恢复速度
	GAP_FIGHT_MP_RECOVER,			///< 基本战斗法力恢复速度
	GAP_FIGHT_EP_RECOVER,			///< 基本战斗内力恢复速度
	GAP_FIGHT_STAMINA_RECOVER,		///< 基本战斗怒气恢复速度

	GAP_ARMOR_HP_1,					///< 基本护盾生命1
	GAP_ARMOR_HP_2,					///< 基本护盾生命2
	GAP_ARMOR_HP_3,					///< 基本护盾生命3

	GAP_BASE_MANLY,					///< 基本霸气
	GAP_BASE_MANLY_UPPER_LIMIT,		///< 基本霸气上限
	GAP_BASE_EVILLY,				///< 基本煞气
	GAP_BASE_EVILLY_UPPER_LIMIT,	///< 基本煞气上限
	GAP_BASE_RESENT,				///< 基本怨气
	GAP_BASE_RESENT_UPPER_LIMIT,	///< 基本怨气上限
	GAP_BASE_KILLLY,				///< 基本杀气
	GAP_BASE_KILLLY_UPPER_LIMIT,	///< 基本杀气上限

	GAP_BASE_STR,					///< 基本力量
	GAP_BASE_DEX,					///< 基本身法
	GAP_BASE_CON,					///< 基本根骨
	GAP_BASE_INTELLECT,				///< 基本意志
	GAP_BASE_SPIRITUALISM,			///< 基本灵性

	GAP_BASE_PARRY_LEVEL,			///< 基本招架等级
	GAP_BASE_PARRY_RATIO,			///< 基本招架机率
	GAP_BASE_DODGE_LEVEL,			///< 基本闪避等级
	GAP_BASE_DODGE_RATIO,			///< 基本闪避机率
	GAP_BASE_ARMOR_DEF,				///< 基本护甲值

	GAP_BASE_DEF_SUCK,				///< 基本伤害吸收率
	GAP_BASE_ENERGY_DEF,			///< 基本内功抗性
	GAP_BASE_ENERGY_SUCK,			///< 基本内功吸收率
	GAP_BASE_OUTENERGY_HIT_LEVEL,	///< 基本外功命中等级
	GAP_BASE_OUTENERGY_HIT_RATIO,	///< 基本外功命中机率
	GAP_BASE_ENERGY_HIT_LEVEL,		///< 基本内功命中等级
	GAP_BASE_ENERGY_HIT_RATIO,		///< 基本内功命中机率
	GAP_BASE_OUTENERGY_SPEED_LEVEL,	///< 基本外功加速等级
	GAP_BASE_OUTENERGY_SPEED_RATIO,	///< 基本外功加速机率
	GAP_BASE_ENERGY_SPEED_LEVEL,	///< 基本内功加速等级
	GAP_BASE_ENERGY_SPEED_RATIO,	///< 基本内功加速机率

	GAP_BASE_OUTENERGY_INTENSITY,	///< 基本外功强度
	GAP_BASE_OUTENERGY_HERT,		///< 基本外功伤害
	GAP_BASE_ENERGY_HERT_INC,		///< 基本内功伤害加成
	GAP_BASE_ENERGY_HEAL_INC,		///< 基本内功治疗加成
	GAP_BASE_OUTENERGY_BLAST_LEVEL,	///< 基本外功暴击等级
	GAP_BASE_OUTENERGY_BLAST_RATE,	///< 基本外功暴击率
	GAP_BASE_OUTENERGY_BLAST_HERT,	///< 基本外功暴击伤害
	GAP_BASE_ENERGY_BLAST_LEVEL,	///< 基本内功暴击等级
	GAP_BASE_ENERGY_BLAST_RATE,		///< 基本内功暴击率
	GAP_BASE_ENERGY_BLAST_HERT,		///< 基本内功暴击伤害

	GAP_BASE_SLEIGHT_LEVEL,			///< 基本熟练等级
	GAP_BASE_UNFLEE_RATIO,			///< 基本无视目标闪避比例
	GAP_BASE_UNPARRY_RATIO,			///< 基本无视目标招架比例
	GAP_BASE_DEF_LEVEL,				///< 基本防御等级
	GAP_BASE_POJIA_LEVEL,			///< 基本破甲等级
	GAP_BASE_UNDEF_RATIO,			///< 基本无视目标护甲比例

	//////////////////////////////////////////////////////////////////////////


	GP_PRICE,					///< 物品价格
	GP_DESCRIBE,				///< 物品描述
	GP_TYPE,					///< 物品类型
	//GP_EQUIPPLACE,				///< 装备栏位标识
	GP_ORIGINAL_NAME,			///< 原始名字
	GP_NAME						///< 显示名
};

/// 玩家的角色划分,用于宠物属性
enum PLAYER_ROLE
{
	PR_NORMAL_PLAYER			= 0x00000001,///< 普通玩家
	PR_GUILD_MASTER				= 0x00000002,///< 工会会长
	PR_UNION_MASTER				= 0x00000004,///< 联盟会长
	PR_CITY_MASTER				= 0x00000008,///< 城主
};

/// 特殊属性
enum PARTICULAR_ATTRIBUTE
{
	PA_CANNOT_REPAIRED			= 0x00000001,///< 不可修理
	PA_NEVER_DROP_DOWN			= 0x00000002,///< 永不掉落
	PA_DROP_DOWN_WHEN_LOGOUT	= 0x00000004,///< 离线后会掉出
	PA_DROP_DOWN_WHEN_DEAD		= 0x00000008,///< 死亡必掉落
	PA_DELETE_WHEN_DROP_DOWN	= 0x00000010,///< 落地后会消失	
	PA_CANNOT_TRADE_AND_STORE	= 0x00000020,///< 不可交易也不可存仓库不可摆摊
	PA_UNIQUE					= 0x00000040,///< 神器	
	PA_DROP_DOWN_WHEN_RECALL	= 0x00000080,///< 使用回城卷\瞬移卷后掉落	
	PA_RARITY					= 0x00000100///< 珍贵
};

/// 物品扩展操作窗口类型
enum eGoodsExWindowType
{
	eGWT_DisassembleWindow,				///< 分解窗口
	eGWT_PrescriptionWindow,			///< 合成窗口、绑定和冻结窗口（加纳为魔盒）
	eGWT_UpgradeWindow = 3,				///< 升级窗口
	eGWT_PlusEvilWindow,				///< 魔化窗口
	eGWT_ResetElementTypeWindow,		///< 重置魔化类型窗口
	eGWT_ResetElementXiangXingWindow,	///< 重置魔化相性窗口
	eGWT_DepotWindow,					///< 仓库窗口
	eGWT_CardFuntionWindow,				///< 镶卡主功能页面	
	eGWT_PlayerTradeWindow,				///< 玩家交易功能页面
	eGWT_End,
};

/// 镶嵌窗口类型
enum eEnchaseWindowType
{
	EBWT_OPEN_HOLE=1,					///< 开启镶嵌孔
	EWT_CLOSE_HOLE,						///< 关闭镶嵌孔
	EWT_ENCHASE_CARD,					///< 镶嵌卡片
	EWT_REMOVE_CARD,					///< 移除卡片
};

/// 增值购买类型
enum eIncrementConsumeType
{
	eICT_NORMAL_BUY_ITEM=1,				///< 普通购买
	eICT_MAIL_BUY_ITEM,					///< 邮件购买
	//eICT_USE_FUNCTION					///< 使用增值功能
};

/// 增值购买返回类型
enum eIncrementBuyRet
{
	eICR_BUY_SUCCEED=1,					///< 成功
	eICR_GOLD_NOT_ENOUGH,				///< 金币不足
	eICR_TICKET_NOT_ENOUGH,				///< 点券不足
	eICR_WEIMIAN_NOT_ENOUGH,			///< 位面不足
	eICR_PERSONAL_NUM_LIMIT,			///< 个人限购
	eICR_WORLD_NUM_LIMIT,				///< 世界限购
	eICR_TIME_LIMIT						///< 时间限制
};

enum eIncrementFunction
{
	eIFUNC_RELIVE=90000		//原地复活
};

/// 武器类型
enum eWeaponType
{
	WT_UNKNOWN,							///< 未知
	WT_SINGLE_SWORD,					///< 单手剑
	WT_SINGLE_HAMMER,					///< 单手锤
	WT_SINGLE_KNIFE,					///< 匕首
	WT_WAND,							///< 法杖
	WT_HEAVY_SWORD,						///< 双手剑
	WT_HEAVY_AX,						///< 双手斧
	WT_BOW,								///< 弓
	WT_DOUBLE_SWORD,					///< 双剑
	WT_FIST								///< 拳刃
};

/// 武器手持类型
enum eWeaponHandType
{
	WHT_UNKNOWN,						///< 未知
	WHT_SINGLE_HAND=1,					///< 单手
	WHT_DOUBLE_HAND						///< 双手
};

/// 子背包类型
enum eSubPackType
{
	SPT_NORMAL=1,						///< 普通子背包
	SPT_BUSINESS						///< 跑商专用背包
};

/// 跑商物品类型
enum eBusinessGoodsType 
{
	BGT_NOT_BUSINESS_GOODS,				///< 未知	

	BGT_FOOD,							///< 食物
	BGT_ORE,							///< 矿石
	BGT_PLANT,							///< 植物
	BGT_FIBRE,							///< 纤维
	BGT_ALCOHOL,						///< 酒
	BGT_WOOD,							///< 木材
	BGT_LEATHER,						///< 皮革
	BGT_CLOTH,							///< 布
	BGT_WEAPON,							///< 武具
	BGT_FLAVOR,							///< 香料
	BGT_HANDIWORK,						///< 艺品
	BGT_GEM,							///< 宝石

	BGT_MAX_TYPE,						///< 最大类型
};

/// 帐号锁定返回类型
enum eAccountLockRetVal
{
	ACCOUNT_LOCK_OK=1,					///< 锁定OK
	ACCOUNT_LOCK_ERROR_CDKEY,			///< 错误CDKey
	ACCOUNT_LOCK_NOT_ENOUGH,			///< 余额不足
	ACCOUNT_LOCK_UNKNOWN_ERROR			///< 未知错误
};

/// 物品扩展功能，S2C信息代码
enum eGoodsExTxtCode
{
	eGTC_Failed = 0,					///< 失败
	eGTC_Succeed = 1,					///< 成功

	eGTC_SessionLapse = -0xFFFF,		///< 对话距离过远或已经失效
	eGTC_UnknownErr,					///< 未知原因的错误

	eGTC_CantDisassemble,				///< 不能分解
	eGTC_CantFindAim,					///< 不能找到操作目标
	eGTC_NoEnoughSpace,					///< 包袱空间不够
	eGTC_SrcIsLock,						///< 原料锁定中

	eGTC_NoPrescription,				///< 合成配方不存在
	eGTC_ErrSP,							///< 错误的合成配方

	eGTC_NoArmInHand,					///< 手中没有武器 “该武器不能充能！”
	eGTC_NoReinforceArm,				///< 不是充能武器 “该武器不能充能！”
	eGTC_LevelError,					///< “等级不符不能充能！”
	eGTC_ArmIsFull,						///< “能量充足时不能被同级或更低的能量替换！”

	eGTC_ErrOperation,					///< 错误的操作值
	eGTC_ErrAssistant,					///< 辅助材料错误
	eGTC_CantOperateAim,				///< 目标不能作此操作
	eGTC_DelAssistantFailed,			///< 删除辅助材料失败

	eGTC_ErrStuff,						///< 主材料错误
	eGTC_LevelTiptop,					///< 等级已到最高
	eGTC_NotEnoughMoney,				///< 钱不够

	eGTC_LimitTooLow,					///< 限制等级太低
	eGTC_QualityTooLow,					///< 品质太低

	eGTC_NumError,						///< 操作数量错误
};

/// 装备升级结果
enum eUpgradeResult
{
	eUR_Succeed,						///< 成功
	eUR_Failed_And_LevelNotChange,		///< 失败，等级未改变
	eUR_Failed_And_Degraded,			///< 失败，并且被降级
};

/// 绑定属性值
enum eBindType
{
	eBT_Untie = 0,						///< 未绑定
	eBT_Bind,							///< 已绑定
	eBT_CantBind,						///< 不能绑定
	eBT_CantUntie,						///< 不能松绑
	eBT_GetAndCantUntie,				///< 得到即绑定（且不能解绑）
	eBT_GetAndBind,						///< 得到即绑定（可以解绑）
	eBT_DressAndCantUntie,				///< 装备后绑定（且不能解绑）
	eBT_DressAndBind,					///< 装备后绑定（可以解绑）
};


/// 冻结属性值
enum eFrostType
{
	eFT_UnFrost = 0,					///< 未冻结
	eFT_Frost,							///< 已冻结
	eFT_Thawing,						///< 解冻中
	eFT_CantThaw,						///< 不可解冻（永久冻结）
	eFT_CantFrost,						///< 不可冻结
};


/// 装备升级失败后的操作
enum eFailedOperation
{
	eFO_Null,							///< 无操作
	eFO_Reduce,							///< 减小等级
	eFO_SetTo,							///< 设置等级为
};


/// 装备升级的特殊材料功能
enum eExtraStuffOperate
{
	eESO_Null,							///< 无功能
	eESO_AddOdds,						///< 增加成功率
	eESO_AmendFailOpt,					///< 替换失败操作为
};

/// 魔化材料合成办法
enum eEvilStuffOperation
{
	eESO_1in1 = 1,						///< 1合1
	eESO_2in1,							///< 2合1
	eESO_3in1,							///< 3合1
	eESO_4in1,							///< 4合1
};

/// 分解物种类最大数量
#define	MAX_DISASSEMBLE_NUM		3U
/// 最大物品原始名字长
#define MAX_GOOODS_NAME_SIZE	32U
///	最大魔化等级
#define	MAX_ELEMENT_LEVEL		2000U
///	最大魔化等级old
#define	MAX_EVIL_LEVEL			10U
///	分解物种类数量
#define	DISASSEMBLE_TYPE_NUM	8U
///	分解物种类的子类型数量
#define	DISASSEMBLE_CHILD_TYPE_NUM	10U
///	非副职业制作配方名称长度
#define	MAKE_TABLE_NAME_SIZE	21U
///	非副职业制作配方说明长度
#define	MAKE_TABLE_NARRATE_SIZE	2048U
/// 最大合成材料数量
#define	MAX_STUFF_NUM			15U

/// C2S消息用 合成物数组结构
struct tagSynthesizeGoods
{
	CGUID	arrGoodsGuid[MAX_STUFF_NUM];		///< 物品GUID
	long	arrContainerFlag[MAX_STUFF_NUM];	///< 物品所在容器
	long	arrPos[MAX_STUFF_NUM];				///< 物品所在容器位置
	long	arrNum[MAX_STUFF_NUM];				///< 物品数量
};


/// 冲等最大主材料数量
#define	MAX_UPGRADE_STUFF_NUM			1U
/// 冲等最大辅助材料数量
#define	MAX_UPGRADE_ASSISTANT_NUM		3U
/// 数量说明[1个升级目标 + MAX_UPGRADE_STUFF_NUM个主料 + MAX_UPGRADE_ASSISTANT_NUM个辅料]
#define	IN_TAGUPGRADESTUFF_GOODS_NUM	(1 + MAX_UPGRADE_STUFF_NUM + MAX_UPGRADE_ASSISTANT_NUM)

/// C2S消息用 冲等材料结构
struct tagUpgradeStuff
{
	CGUID	arrGoodsGuid[IN_TAGUPGRADESTUFF_GOODS_NUM];		///< 物品GUID
	long	arrContainerFlag[IN_TAGUPGRADESTUFF_GOODS_NUM];	///< 物品所在容器
	long	arrPos[IN_TAGUPGRADESTUFF_GOODS_NUM];			///< 物品所在容器位置
};

/// 
struct tagEvilStuff
{
	CGUID	GoodsGuid;		///< 物品GUID
	long	lContainerFlag;	///< 物品所在容器
	long	lPos;			///< 物品所在容器位置
	long	lUseNum;		///< 消耗数量
};

/// 物品公用冷却时间
struct tagGoodsCommonCooldown
{
	ulong dwCooldownTime;		///< 公用冷却时间
	ulong dwCooldownStartTime;	///< 开始时间
};

/// 跑商物品
struct tagBusinessGoods
{
	string strOriginName;		///< 原始名
	ulong dwId;					///< 物品ID
	bool bSell;					///< 是否出售
	bool bSpecial;				///< 是否特产
	bool bNeed;					///< 是否特需
	ulong dwBear;				///< 承载量
	ulong dwCategory;			///< 类别
	ulong dwBasePrice;			///< 基本价
	double dBuyPriceMin;		///< 买入最低
	double dBuyPriceMax;		///< 买入最高
	double dSellPriceMin;		///< 卖出最低
	double dSellPriceMax;		///< 卖出最高
};

/// 打开商业买卖界面时传给CLIENT的结构
struct tagBusinessGoodsToClient
{
	ulong dwId;					///< 物品ID
	ulong dwPrice;				///< 价格
	ulong dwBuyLimit;			///< 限购额
	uchar btSpecial;			///< 是否特产
	uchar btNeed;				///< 是否特需品
};

/// 跑商物品类别
struct tagBusinessGoodsCategory
{
	ulong dwCategoryId;			///< 类别ID
	string strName;				///< 名称
	ulong dwBuyLimit;			///< 限购额
};

/// 交易区数据类型
struct tagTradeSpotData
{
	ulong dwSpotId;										///< 交易区ID
	ulong dwGameServerId;								///< GS ID
	ulong dwAreaId;										///< 范围ID
	ulong dwCountryId;									///< 国家ID
	map<ulong,tagBusinessGoodsCategory> mapCategory;	///< 类别
	map<ulong,tagBusinessGoods> mapGoods;				///< 物品
};