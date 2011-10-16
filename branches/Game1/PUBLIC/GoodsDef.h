

#pragma once


//附加属性全部类型
static enum GOODS_ADDON_PROPERTIES
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
	//转态追加
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
	GAP_QUEST_GOODS_ATTR,//任务物品属性
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
	GAP_ATK_ED,//攻击ED
	GAP_CUR_ATK_ED,//当前攻击ED
	GAP_DEF_ED,//防御ED
	GAP_CUR_DEF_ED,//当前防御ED
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
	GAP_SCRIPT_VAR1,	//脚本变量1
	GAP_SCRIPT_VAR2,	//脚本变量2
	GAP_SCRIPT_VAR3,	//脚本变量3
	GAP_ADD_MAXHP_UP,	//生命上限追加成长
	GAP_ADD_MAXHP_LEVEL, //冲等追加生命上限
	GAP_EXCUSE_HURT_RANGE,//免伤范围
	GAP_PENETRATE_RANGE,//穿透范围
	GAP_LUCKY_RANGE,//幸运范围
	GAP_ROULEAU_MAKE_GOODS,//卷轴制作物品ID	
	GAP_ADDON_HP_RANGE,//HP上线追加范围
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
	GAP_PROPERTY_TRIGGER,//道具属性激发
	GAP_GOODSDESC_RES_ID,//道具附加描述资源ID
	GAP_ADD_MAX_PKCP,//增加PK CP值上限
	GAP_ADD_MAX_PKCPRANGE,///增加的PK CP值范围
	GAP_TIMES_MAX_PKCP, //增加PK CP上限值倍率系数（乘法）
	GAP_TIMES_MAX_PKCPRANGE,//倍率随机范围
	GAP_DATE_FOR_SCRIPT,//! 脚本日期，为脚本提供的时间保存栏位，值的意义由脚本定义

	GAP_ELEMENT_1,//! 物品魔化属性1，值1代表魔化属性，值2表示魔化等级
	GAP_ELEMENT_2,//! 物品魔化属性2，值1代表魔化属性，值2表示魔化等级
	GAP_ELEMENT_3,//! 物品魔化属性3，值1代表魔化属性，值2表示魔化等级

	GAP_ELEMENT_XIANG_XING_1,//! 魔化相性1
	GAP_ELEMENT_XIANG_XING_2,//! 魔化相性2
	GAP_ELEMENT_XIANG_XING_3,//! 魔化相性3

	GAP_GUARDIAN_ENERGY, //星座能量值：值1：当前值；值2：最大值
	GAP_GUARDIAN_ENERGY_DEC, //星座能量值降低因子
	GAP_FBUSINESS_PRICE, //家族跑商商品价格
	GAP_STARLEVEL,//卡片星等

	GAP_UPGRADE_LIMIT, //! 物品冲等时的被使用限制，值1=可冲等物品最小限制等级，值2=可冲等物品最大限制等级

	GAP_CARD_ELEMENT_EARTH_A,	//! 卡片地攻
	GAP_CARD_ELEMENT_WATER_A,	//! 卡片水攻
	GAP_CARD_ELEMENT_FIRE_A,	//! 卡片火攻
	GAP_CARD_ELEMENT_WIND_A,	//! 卡片风攻
	GAP_CARD_ELEMENT_BRIGHT_A,	//! 卡片光攻
	GAP_CARD_ELEMENT_DARK_A,	//! 卡片暗攻

	GAP_CARD_ELEMENT_EARTH_D,	//! 卡片地防
	GAP_CARD_ELEMENT_WATER_D,	//! 卡片水防
	GAP_CARD_ELEMENT_FIRE_D,	//! 卡片火防
	GAP_CARD_ELEMENT_WIND_D,	//! 卡片风防
	GAP_CARD_ELEMENT_BRIGHT_D,	//! 卡片光防
	GAP_CARD_ELEMENT_DARK_D,	//! 卡片暗防

	GAP_STARLEVEL_RANGE,		// 拆卡时适用的星等范围
	GAP_CARDPICTURE_INDEX,	// 用这个值1来索引卡片要显示的大图片，这个值就是图片的文件名


	//物品价格
	GP_PRICE, 
	//物品描述
	GP_DESCRIBE,
	// 物品类型
	GP_TYPE, 
	//装备栏位标识
	GP_EQUIPPLACE, 
	// 原始名字
	GP_ORIGINAL_NAME, 
	//显示名
	GP_NAME, 

	GAP_WEAPON_LEVEL_MAX,     //物品强化等级上限
	GAP_WEAPON_MODEL_EX,      //武器换装模型ID
	GAP_WEAPON_MODEL_TIME,    //武器换装模型时效
	GAP_WEAPON_SCRIPT_TIME,   //脚本通过赋予时间

};

enum InterfaceType  
{
	TYPE_BROACH = 1,					//钻孔页面
	TYPE_JAM,							//堵孔页面
	TYPE_ENCHASE,						//镶嵌页面
	TYPE_BACKOUT,						//拆卡页面

	TYPE_COUNT							//页面数量
};


struct tagCardValueMap 
{
	//! 装备类型（对应物编上物品类别所设定的值）
	DWORD BaseType;
	//! 武器/防具类别（对应物品附加属性——武器GAP_WEAPON_CATEGORY、防具GAP_GUARD_EQUIP_CATEGORY、盾牌戒指项链没有）
	DWORD ExType;
	//! 对应卡片镶嵌的位值
	DWORD BitValue;
};

//! 从策划案获得的值对应表
const tagCardValueMap CardValueMap[] = 
{
	8, 1, 0x1,			//! 单手剑
	8, 2, 0x2,			//! 单手锤
	8, 3, 0x4,			//! 匕首
	8, 4, 0x8,			//! 法杖
	8, 5, 0x10,			//! 双手剑
	8, 6, 0x20,			//! 双手斧
	8, 7, 0x40,			//! 弓
	8, 8, 0x80,			//! 双剑
	8, 9, 0x100,		//! 拳刃

	9, 0, 0x200,		//! 盾牌

	4, 1, 0x400,		//! 布头
	6, 1, 0x800,		//! 布手
	7, 1, 0x1000,		//! 布脚
	5, 1, 0x2000,		//! 布衣

	4, 2, 0x4000,		//! 皮头
	6, 2, 0x8000,		//! 皮手
	7, 2, 0x10000,		//! 皮脚
	5, 2, 0x20000,		//! 皮衣

	4, 3, 0x40000,		//! 重头
	6, 3, 0x80000,		//! 重手
	7, 3, 0x100000,		//! 重脚
	5, 3, 0x200000,		//! 重衣

	10, 0, 0x400000,	//! 项链
	11, 0, 0x800000,	//! 戒指
};

const DWORD CARD_VALUE_MAP_SIZE = sizeof(CardValueMap) / sizeof(tagCardValueMap);


//! 检测卡片能否镶嵌
//! CardSet：卡片上的镶嵌设置值
//! BaseType：装备的物品类型值（对应物编上物品类别所设定的值）
//! ExType：武器/防具类别（对应物品附加属性——武器GAP_WEAPON_CATEGORY、防具GAP_GUARD_EQUIP_CATEGORY、盾牌戒指项链没有=0）
bool CheckCardValue(DWORD CardSet, DWORD BaseType, DWORD ExType = 0);

