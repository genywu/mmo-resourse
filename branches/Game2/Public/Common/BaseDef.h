#pragma once
#include "../../Public/Module/GUID.h"
//定义基本的一些游戏结构、枚举

/* 游戏中的流通物类别 
AddBy 肖杨 09.10.13*/
enum MERCHANTABLE_THING
{
    MT_XINGZHUAN = 0,	// 星钻
    MT_WEIMIAN ,		// 位面
    MT_DIANJUAN ,		// 点卷
    MT_GOLD ,			// 金币
    MT_SILVERMONEY,	    // 银币
    MT_NUM
};

enum COUNTRY_TYPE
{
    COUNTRY_STAR = 1,		// 星
    COUNTRY_SEA,			// 海
    COUNTRY_CLOUD,			// 云
    COUNTRY_LUNA,			// 月
    COUNTRY_SAND,			// 砂
    COUNTRY_WIND,			// 风
    COUNTRY_UPPER			// 上限
};

enum eOccupation
{
    OCC_Novice=0,			//初心者
    OCC_Fighter,			//战士
    OCC_Mystic,				//法师(人类法师)
    OCC_Rogue,				//行者
    OCC_Knight,				//骑士
    OCC_Warrior,			//斗士
    OCC_Scout,				//斥候
    OCC_Craftsman,			//艺人
    OCC_Warlock,			//术士
    OCC_Cleric,				//牧师
    OCC_Paladin,			//圣骑士
    OCC_DarkKnight,			//暗骑士
    OCC_Gladiator,			//角斗士
    OCC_Berserker,			//狂战士
    OCC_Ranger,				//巡守
    OCC_Assassin,			//刺客
    OCC_BladeDancer,		//剑舞者
    OCC_Bard,				//吟游诗人
    OCC_Sorcerer,			//巫师
    OCC_Necromancer,		//亡灵巫师
    OCC_Bishop,				//主教
    OCC_Prophet,			//贤者(先知)

    OCC_Max,				//职业最大数
};

typedef enum eConstellation
{
    CONST_No=0,				//无
    CONST_Aries,			//牧羊座
    CONST_Taurus,			//金牛座
    CONST_Gemini,			//双子座
    CONST_Cancer,			//巨蟹座
    CONST_Leo,				//狮子座
    CONST_Virgo,			//处女座
    CONST_Libra,			//天秤座
    CONST_Scorpio,			//天蝎座
    CONST_Sagittarius,		//射手座
    CONST_Capricorn,		//摩羯座
    CONST_Aquarius,			//水瓶座
    CONST_Pisces,			//双鱼座
}eConst;

//所有种族类别
const int MONSTER_SKILL_SUM=6;
const int MONSTER_AI_REFERRENCE_NUM=5;

// [070622 AHC]
//属性数据类型:	该属性的C++数据类型
enum ATTRIBUTE_DATA_TYPE
{
    ATTR_DATA_BYTE = 0,
    ATTR_DATA_WORD,
    ATTR_DATA_DWORD,
    ATTR_DATA_FLOAT,
    ATTR_DATA_LONG,
    ATTR_DATA_SHORT,
    ATTR_DATA_CHAR,
    ATTR_DATA_BOOL,

    ATTR_DATA_BYTEARRAY,
    ATTR_DATA_WORDARRAY,
    ATTR_DATA_DWORDARRAY,
    ATTR_DATA_FLOATARRAY,
    ATTR_DATA_LONGARRAY,
    ATTR_DATA_SHORTARRAY,
    ATTR_DATA_CHARARRAY,
    ATTR_DATA_BOOLARRAY,
};
/**
    S_ATTR_ROOT,                //定身
    S_ATTR_DIZZY,               //昏迷
    S_ATTR_PARALYSIS,           //瘫痪状态
    S_ATTR_BANISH,              //放逐
    S_ATTR_BAFFLE,              //困惑
    S_ATTR_CONFUSE,             //混乱
    S_ATTR_DREAD,               //恐惧
 */
//MoveShape属性类型:	定义的属性类型枚举值
enum S_ATTRIBUTE_TYPE
{
    S_ATTR_No=0,				//无
    S_ATTR_INVISIBLETIME,	    //隐形
    S_ATTR_HIDE,			    //隐蔽
    S_ATTR_PRETENDDEADTIME,     //假死
    S_ATTR_CHANGEMODELTYPE,     //改变类型
    S_ATTR_CHANGEMODELID,	    //改变ID
    S_ATTR_USEINSTRUCTION,      //无法使用指令
    S_ATTR_MOVEABLE,		    //能否移动
    S_ATTR_FIGHTABLE,		    //能否攻击
    S_ATTR_USEFIGHTSKILL,	    //能否使用战斗技能
    S_ATTR_USERABBSKILL,	    //能否使用法师技能
    S_ATTR_USEITEM, 		    //使用道具
    S_ATTR_ANTIINVISIBLEABLE,	//看隐形
    S_ATTR_ADDHP,			    //不能增加HP
    S_ATTR_UNATTACK,		    //不可攻击
    S_ATTR_FORCEATTACK,		    //强制攻击
    S_ATTR_RAMDOMMOVE,		    //随机跑动
    S_ATTR_SHAPESTATE,          //Cshape state
    S_ATTR_SPEED,			    //速度（shape上的）
    S_ATTR_COLLECTIONID,	    //采集物id
    S_ATTR_COLLECTIONTIMES,	    //能采集的次数
    S_ATTR_ISBEENCOLLECT,	    //是否正在被采集
    S_ATTR_CANBEENCOLLECT,	    //是否能被采集
    S_ATTR_NOTRIDE,			    //非坐骑状态
    S_ATTR_SELFCONTROL,         //是否可以控制角色
    S_ATTR_USEALLSKILL,         //是否可以使用所有的技能
    S_ATTR_MOVEDAMAGE,          //受到伤害是否移除
    S_ATTR_CANBEENATTACK,       //能否被攻击
    S_ATTR_MOVECONTROL,         //移动状态效果叠加

    S_ATTR_UNKOWN,              //无法识别的属性 1BYTE
    S_ATTR_END = 0x3F,
};
//player属性类型:	定义的属性类型枚举值
enum P_ATTRIBUTE_TYPE
{
    P_ATTR_MONEY = 0x3F+1,
    P_ATTR_YUANBAO,
    P_ATTR_FACTIONTITLE,		            //string类型
    P_ATTR_FACTIONNAME,			            //string类型
    P_ATTR_ENEMYFACTIONS,		            //longarray类型
    P_ATTR_FACTIONID,
    P_ATTR_FACTIONLOGOID,
    P_ATTR_FACMASTERID,
    P_ATTR_FACTIONLVL,
    P_ATTR_FACTIONEXP,
    P_ATTR_FACTIONCONTROBUTE,
    P_ATTR_UNIONID,
    P_ATTR_UNIONMASTERID,
    P_ATTR_CONTENDSTATE,
    P_ATTR_CITYWARDIEDSTATE,
    P_ATTR_CITYWARDIEDSTATETIME,			//服务器专用
    P_ATTR_CREATEFACTIONOPERATOR,			//服务器专用
    P_ATTR_APLLYJOINOPERATOR,				//服务器专用
    P_ATTR_FACTIONDELCAREOPERATOR,			//服务器专用
    //////////////////////客户端是否需要更新////////////////////////
    P_ATTR_TYPE,
    P_ATTR_ID,
    P_ATTR_GRAPHICSID,
    P_ATTR_REGIONID,
    P_ATTR_TILEX,
    P_ATTR_TILEY,
    P_ATTR_DIR,
    P_ATTR_POS,
    P_ATTR_SPEED,
    P_ATTR_STATE,
    P_ATTR_ACTION,
    //////////////////////////////////////////////////////////////////
    /*人物属性*/
    P_ATTR_OCCU,
    P_ATTR_DOCCU,//副职业
    P_ATTR_SEX,
    P_ATTR_HEADPIC,
    P_ATTR_FACEPIC,
    P_ATTR_HP,
    P_ATTR_MP,
    P_ATTR_ENERGY,
    P_ATTR_STAMINA,
    P_ATTR_LEVEL,
    P_ATTR_EXP,
    P_ATTR_EXPMULTIPLE,
    P_ATTR_PRESENTEXP,
    P_ATTR_BUDDHISM,
    P_ATTR_DRYNUM,
    P_ATTR_DRYENDTIME,
    P_ATTR_OCCULVL0,//初心者职业等级
    //...其他职业等级
    P_ATTR_OCCULVL21=P_ATTR_OCCULVL0+OCC_Prophet,//贤者职业等级
    P_ATTR_OCCUEXP0,//初心者职业经验
    //...其他职业经验
    P_ATTR_OCCUEXP21=P_ATTR_OCCUEXP0+OCC_Prophet,//贤者职业经验
    P_ATTR_CUROCCUUPGRADEEXP,//当前职业等级的升级经验

    P_ATTR_TAOISMLVL,
    P_ATTR_TAOISMEXP,
    P_ATTR_RELATIONVAL,
    P_ATTR_HIDDENVIRTUE,
    P_ATTR_VIGOUR,
    P_ATTR_THEW,

    P_ATTR_COUNTRY,
    P_ATTR_COUNTRYCONTRIBUTE,
    P_ATTR_COUNTRYSTATUS,

    P_ATTR_SPOUSEID,
    P_ATTR_SPOUSEPARAM,
    P_ATTR_BUSINESSLEVEL,
    P_ATTR_BUSINESSEXP,

    P_ATTR_PKCOUNT,
    P_ATTR_PKVALUE,
    P_ATTR_PVPCOUNT,
    P_ATTR_PVPVALUE,

    P_ATTR_PKONOFF,
    P_ATTR_OCCUSP0,//初心者职业SP
    //...其他职业SP
    P_ATTR_OCCUSP21=P_ATTR_OCCUSP0+OCC_Prophet,//贤者职业SP
    P_ATTR_UPGRADEEXP,
    P_ATTR_SHOWCOUNTRY,
    P_ATTR_HOTKEY0,
    P_ATTR_HOTKEY1,
    P_ATTR_HOTKEY2,
    P_ATTR_HOTKEY3,
    P_ATTR_HOTKEY4,
    P_ATTR_HOTKEY5,
    P_ATTR_HOTKEY6,
    P_ATTR_HOTKEY7,
    P_ATTR_HOTKEY8,
    P_ATTR_HOTKEY9,
    P_ATTR_HOTKEY10,
    P_ATTR_HOTKEY11,
    P_ATTR_HOTKEY12,
    P_ATTR_HOTKEY13,
    P_ATTR_HOTKEY14,
    P_ATTR_HOTKEY15,
    P_ATTR_HOTKEY16,
    P_ATTR_HOTKEY17,
    P_ATTR_HOTKEY18,
    P_ATTR_HOTKEY19,
    P_ATTR_HOTKEY20,
    P_ATTR_HOTKEY21,
    P_ATTR_HOTKEY22,
    P_ATTR_HOTKEY23,
    /*战斗属性*/
    P_ATTR_MAXHP,
    P_ATTR_MAXMP,
    P_ATTR_MAXSTAMINA,
    P_ATTR_MAXENERGY,	
    P_ATTR_HPRECOVERRATIO,
    P_ATTR_HPRECOVERSPEED,	
    P_ATTR_MPRECOVERSPEED,	
    P_ATTR_ENERRECOVERSPEED,
    P_ATTR_STAMRECOVERSPEED,		
    P_ATTR_FIGHTHPRECOVERSPEED,
    P_ATTR_FIGHTMPRECOVERSPEED,
    P_ATTR_FIGHTENERRECOVERSPEED,
    P_ATTR_FIGHTSTAMRECOVERSPEED,

    P_ATTR_MANLY,
    P_ATTR_PERVERSETREND,
    P_ATTR_COMPLAINT,
    P_ATTR_MURDEROUSLOOK,

    P_ATTR_STR,
    P_ATTR_DEX,
    P_ATTR_CON,
    P_ATTR_INTELLECT,
    P_ATTR_SPIRITUALISM,
    P_ATTR_POINT,
    P_ATTR_PARRY,
    P_ATTR_DODGE,
    P_ATTR_DEF,
    P_ATTR_DEFSUCK,
    P_ATTR_MDEF,
    P_ATTR_MDEFSUCK,
    P_ATTR_BLOODSUCK,
    P_ATTR_MANASUCK,
    P_ATTR_ENERGYSUCK,
    P_ATTR_STAMINASUCK,
    P_ATTR_HIT,
    P_ATTR_ATKSPEED,
    P_ATTR_MATKSPEED,
    P_ATTR_MINATK,
    P_ATTR_MAXATK,
    P_ATTR_MINMATK,
    P_ATTR_MAXMATK,
    P_ATTR_PHYSICIMMUNITY,
    P_ATTR_MAGICIMMUNITY,
    P_ATTR_MOVESPEED,
    P_ATTR_MOVEACCELERATION,
    P_ATTR_MOVERADIUS,
    P_ATTR_TURNSPEED,
    P_ATTR_ELEMDEF_FIRE,
    P_ATTR_ELEMDEF_WATER,
    P_ATTR_ELEMDEF_EARTH,
    P_ATTR_ELEMDEF_WIND,
    P_ATTR_ELEMDEF_DARK,
    P_ATTR_ELEMDEF_HOLY,
    P_ATTR_DEBUFFDEF_BLED,
    P_ATTR_DEBUFFDEF_DIZZY,
    P_ATTR_DEBUFFDEF_LULL,
    P_ATTR_DEBUFFDEF_HYPNOSIS,
    P_ATTR_DEBUFFDEF_POSION,
    P_ATTR_DEBUFFDEF_FIX,
    P_ATTR_DEBUFFDEF_SILENCE,
    P_ATTR_CLASSDAM_HUMAN,
    P_ATTR_CLASSDAM_ELEMENT,
    P_ATTR_CLASSDAM_ALIEN,
    P_ATTR_CLASSDAM_ANIMAL,
    P_ATTR_CLASSDAM_UNDEAD,
    P_ATTR_CLASSDAM_EVIL,
    P_ATTR_CLASSDAM_HEXAPODS,

    P_ATTR_STRENTH,
    P_ATTR_AGILITY,	
    P_ATTR_CONSTI,
    P_ATTR_INTELL,
    P_ATTR_WISDOM,
    P_ATTR_SPIRIT,
    P_ATTR_BLOCK,
    P_ATTR_CHARM,
    P_ATTR_ABSORB,	
    P_ATTR_SHIELD_DEF,
    P_ATTR_CRI,
    P_ATTR_BUFFPOWER,
    P_ATTR_MATK,
    P_ATTR_MCRI,
    P_ATTR_WILLDEF,
    P_ATTR_CONSTIDEF,
    P_ATTR_ELEMDAM_FIRE,
    P_ATTR_ELEMDAM_WATER,
    P_ATTR_ELEMDAM_EARTH,
    P_ATTR_ELEMDAM_WIND,
    P_ATTR_ELEMDAM_DARK,
    P_ATTR_ELEMDAM_HOLY,
    P_ATTR_ELEMATK_FIRE,	
    P_ATTR_ELEMATK_WATER,	
    P_ATTR_ELEMATK_EARTH,	
    P_ATTR_ELEMATK_WIND,	
    P_ATTR_ELEMATK_DARK,	
    P_ATTR_ELEMATK_HOLY,	
    P_ATTR_CRIDEF,
    P_ATTR_MCRIDEF,
    P_ATTR_CRIDAMAGE,
    P_ATTR_MCRIDAMAGE,
    P_ATTR_IMMUNITY,
    P_ATTR_PIERCE,
    P_ATTR_LUCK,
    P_ATTR_CONCEALMENT,
    P_ATTR_HURTFACTOR,

    /*基本战斗属性*/
    P_ATTR_BASEMAXHP,
    P_ATTR_BASEMAXMP,		
    P_ATTR_BASEMAXSTAMINA,
    P_ATTR_BASEMAXENERGY,
    P_ATTR_BASEHPRECOVERSPEED,
    P_ATTR_BASEMPRECOVERSPEED,
    P_ATTR_BASESTAMRECOVERSPEED,	
    P_ATTR_BASESTRENTH,
    P_ATTR_BASEAGILITY,
    P_ATTR_BASECONSTI,
    P_ATTR_BASEINTELL,
    P_ATTR_BASEWISDOM,
    P_ATTR_BASESPIRIT,		
    P_ATTR_BASEBLOCK,		
    P_ATTR_BASEPARRY,	
    P_ATTR_BASEDODGE,
    P_ATTR_BASECHARM,
    P_ATTR_BASEHIT,
    P_ATTR_BASEABSORB,
    P_ATTR_BASEDEF,
    P_ATTR_BASEMDEF,		
    P_ATTR_BASECRI,		
    P_ATTR_BASEMATKSPEED,	
    P_ATTR_BASEBUFFPOWER,
    P_ATTR_BASEMINATK,
    P_ATTR_BASEMAXATK,
    P_ATTR_BASEMATK,		
    P_ATTR_BASEMCRI,		
    P_ATTR_BASEBLOODSUK,		
    P_ATTR_BASEMANASUCK,		
    P_ATTR_BASEWILLDEF,	
    P_ATTR_BASECONSTIDEF,
    P_ATTR_BASEDEBUFFDEF_BLED,
    P_ATTR_BASEDEBUFFDEF_DIZZY,
    P_ATTR_BASEDEBUFFDEF_LULL,
    P_ATTR_BASEDEBUFFDEF_HYPNOSIS,
    P_ATTR_BASEDEBUFFDEF_POSION,
    P_ATTR_ATTR_BASEDEBUFFDEF_FIX,
    P_ATTR_BASEELEMDEF_FIRE,
    P_ATTR_BASEELEMDEF_WATER,
    P_ATTR_BASEELEMDEF_EARTH,
    P_ATTR_BASEELEMDEF_WIND,
    P_ATTR_BASEELEMDEF_DARK,
    P_ATTR_BASEELEMDEF_HOLY,
    P_ATTR_BASECLASSDAM_HUMAN,
    P_ATTR_BASECLASSDAM_ELEMENT,
    P_ATTR_BASECLASSDAM_ALIEN,
    P_ATTR_BASECLASSDAM_ANIMAL,
    P_ATTR_BASECLASSDAM_UNDEAD,
    P_ATTR_BASECLASSDAM_EVIL,
    P_ATTR_BASECLASSDAM_HEXAPODS,
    P_ATTR_BASEELEMDAM_FIRE,
    P_ATTR_BASEELEMDAM_WATER,
    P_ATTR_BASEELEMDAM_EARTH,
    P_ATTR_BASEELEMDAM_WIND,
    P_ATTR_BASEELEMDAM_DARK,
    P_ATTR_BASEELEMDAM_HOLY,
    P_ATTR_BASECRIDEF,
    P_ATTR_BASEMCRIDEF,
    P_ATTR_BASECRIDAMAGE,
    P_ATTR_BASEMCRIDAMAGE,
    P_ATTR_SAVEDTIME,
    P_ATTR_REMAINPOINT,
    P_ATTR_DISPLAYHEADPIECE,

    P_ATTR_COLLECTIONEXP,
    P_ATTR_FACTUREEXP,
    P_ATTR_COLLECTIONGRADE,
    P_ATTR_FACTUREGRADE,

    P_ATTR_CURPKCP,
    P_ATTR_MAXPKCP,

    P_EARTH_ATTACK,
    P_EARTH_DEFEND,
    P_WATER_ATTACK,
    P_WATER_DEFEND,
    P_FIRE_ATTACK,	
    P_FIRE_DEFEND,	
    P_WIND_ATTACK,	
    P_WIND_DEFEND,	
    P_BRIGHT_ATTACK,
    P_BRIGHT_DEFEND,
    P_DARK_ATTACK,
    P_DARK_DEFEND,	

    P_EARTH_ATTACKXIANG,
    P_EARTH_DEFENDXIANG,
    P_WATER_ATTACKXIANG,
    P_WATER_DEFENDXIANG,
    P_FIRE_ATTACKXIANG,
    P_FIRE_DEFENDXIANG,
    P_WIND_ATTACKXIANG,
    P_WIND_DEFENDXIANG,
    P_BRIGHT_ATTACKXIANG,
    P_BRIGHT_DEFENDXIANG,
    P_DARK_ATTACKXIANG,
    P_DARK_DEFENDXIANG,


};



//monster属性类型:	定义的属性类型枚举值
enum M_ATTRIBUTE_TYPE
{
    M_ATTR_TYPE = 0x3F+1,
    M_ATTR_ID,
    M_ATTR_GRAPHICSID,
    M_ATTR_REGIONID,
    M_ATTR_TILEX,
    M_ATTR_TILEY,
    M_ATTR_DIR,
    M_ATTR_POS,

    M_ATTR_STATE,
    M_ATTR_ACTION,


    /*怪物属性*/
    // 0: uchar 1:ushort 2:ulong	3:FLOAT 4:long 5:SHORT 6:bool
    M_ATTR_INDEX,
    M_ATTR_PICLEVEL,
    M_ATTR_NAMECOLOR,
    M_ATTR_HPBARCOLOR,
    M_ATTR_SOUNDID,
    M_ATTR_LEVEL,
    M_ATTR_EXP,
    M_ATTR_OCCUPOINT,
    M_ATTR_COUNTRY,
    M_ATTR_TAMABLE,
    M_ATTR_MAXTAMECOUNT,
    M_ATTR_COUNTRYCONTRIVALUE,
    M_ATTR_SPCONTRIVALUE,
    M_ATTR_ELETYPE_,
    M_ATTR_NPCTYPE_,
    M_ATTR_CLASS,
    M_ATTR_RACE,
    M_ATTR_MOVERANDOMVALUE,
    M_ATTR_STOPTIME,
    M_ATTR_AIHANGUPTYPE,
    M_ATTR_FIGURE,
    M_ATTR_HP,
    M_ATTR_AITYPE,

    M_ATTR_HURTPROPORTION,
    M_ATTR_GUARDTIME,
    M_ATTR_ESCAPEPOINT,
    M_ATTR_SKILLPROPORTION,
    M_ATTR_SKILLSPACE	,

    M_ATTR_AIREFERRENCE0	,
    M_ATTR_AIREFERRENCE1,
    M_ATTR_AIREFERRENCE2,
    M_ATTR_AIREFERRENCE3,
    M_ATTR_AIREFERRENCE4,

    /*战斗属性*/
    M_ATTR_MAXHP		,
    M_ATTR_HPRECOVERSPEED	,		
    M_ATTR_MINATK,
    M_ATTR_MAXATK		,
    M_ATTR_MINMATK	,	
    M_ATTR_MAXMATK,
    M_ATTR_HIT,
    M_ATTR_DEF,
    M_ATTR_MDEF,
    M_ATTR_DODGE,
    M_ATTR_BLOCK,
    M_ATTR_PARRY,
    M_ATTR_IMMUNITY,
    M_ATTR_PIERCE,
    M_ATTR_ATCSPEED,
    M_ATTR_MOVESPEED,
    M_ATTR_FIGHTMOVESPEED,
    /*M_ATTR_FIGHTBLOCK*/	
    M_ATTR_FARATKDIS,
    M_ATTR_NEAATKDIS,
    M_ATTR_ATKRANGE,
    M_ATTR_GUARDRANGE,
    M_ATTR_CHASERANGE	,
    M_ATTR_WILLDEF,
    M_ATTR_CONSTIDEF		,	
    M_ATTR_CRI			,
    M_ATTR_CRIDEF,
    M_ATTR_MCRIDEF	,
    M_ATTR_CANTALK	,

    M_ATTR_DEBUFFDEF_BLED,
    M_ATTR_DEBUFFDEF_DIZZY,
    M_ATTR_DEBUFFDEF_LULL,
    M_ATTR_DEBUFFDEF_HYPNOSIS,
    M_ATTR_DEBUFFDEF_POSION,
    M_ATTR_DEBUFFDEF_FIX,
    M_ATTR_DEBUFFDEF_SILENCE,

    M_ATTR_ELEMDEF_FIRE,
    M_ATTR_ELEMDEF_WATER,
    M_ATTR_ELEMDEF_EARTH,
    M_ATTR_ELEMDEF_WIND,
    M_ATTR_ELEMDEF_DARK,
    M_ATTR_ELEMDEF_HOLY,

    M_ATTR_CLASSDAM_HUMAN,
    M_ATTR_CLASSDAM_ELEMENT,
    M_ATTR_CLASSDAM_ALIEN,
    M_ATTR_CLASSDAM_ANIMAL,
    M_ATTR_CLASSDAM_UNDEAD,
    M_ATTR_CLASSDAM_EVIL,
    M_ATTR_CLASSDAM_HEXAPODS,

    M_ATTR_ELEMDAM_FIRE,
    M_ATTR_ELEMDAM_WATER,
    M_ATTR_ELEMDAM_EARTH,
    M_ATTR_ELEMDAM_WIND,
    M_ATTR_ELEMDAM_DARK,
    M_ATTR_ELEMDAM_HOLY,

    /*基本战斗属性*/
    M_ATTR_BASEMAXHP,
    M_ATTR_BASEHPRECOVERSPEED	,	
    M_ATTR_BASEMINATK		,
    M_ATTR_BASEMAXATK		,
    M_ATTR_BASEMINMATK		,
    M_ATTR_BASEMAXMATK		,
    M_ATTR_BASEHIT	,
    M_ATTR_BASEDEF,
    M_ATTR_BASEDODGE,
    M_ATTR_BASEBLOCK,
    M_ATTR_BASEPARRY,
    M_ATTR_BASEATCSPEED,
    M_ATTR_BASEMOVESPEED		,

    M_ATTR_BASEFARATKDIS,
    M_ATTR_BASENEAATKDIS,
    M_ATTR_BASEATKRANGE,
    M_ATTR_BASEGUARDRANGE,
    M_ATTR_BASECHASERANGE		,
    M_ATTR_BASEWILLDEF,
    M_ATTR_BASECONSTIDEF		,
    M_ATTR_BASECRI	,
    M_ATTR_BASECRIDEF		,
    M_ATTR_BASEMCRIDEF		,
    M_ATTR_BASEDEBUFFDEF_BLED,
    M_ATTR_BASEDEBUFFDEF_DIZZY,
    M_ATTR_BASEDEBUFFDEF_LULL,
    M_ATTR_BASEDEBUFFDEF_HYPNOSIS,
    M_ATTR_BASEDEBUFFDEF_POSION,
    M_ATTR_BASEDEBUFFDEF_FIX,
    M_ATTR_BASEELEMDEF_FIRE,
    M_ATTR_BASEELEMDEF_WATER,
    M_ATTR_BASEELEMDEF_EARTH,
    M_ATTR_BASEELEMDEF_WIND,
    M_ATTR_BASEELEMDEF_DARK,
    M_ATTR_BASEELEMDEF_HOLY,
    M_ATTR_BASECLASSDAM_HUMAN,
    M_ATTR_BASECLASSDAM_ELEMENT,
    M_ATTR_BASECLASSDAM_ALIEN,
    M_ATTR_BASECLASSDAM_ANIMAL,
    M_ATTR_BASECLASSDAM_UNDEAD,
    M_ATTR_BASECLASSDAM_EVIL,
    M_ATTR_BASECLASSDAM_HEXAPODS,
    M_ATTR_BASEELEMDAM_FIRE,
    M_ATTR_BASEELEMDAM_WATER,
    M_ATTR_BASEELEMDAM_EARTH,
    M_ATTR_BASEELEMDAM_WIND,
    M_ATTR_BASEELEMDAM_DARK,
    M_ATTR_BASEELEMDAM_HOLY,
};
//建物属性
enum B_ATTRIBUTE_TYPE
{
    B_ATTR_TYPE =  0x3F+1,
    B_ATTR_GRAPHICSID,
    B_ATTR_REGIONID,
    B_ATTR_TILEX,
    B_ATTR_TILEY,
    B_ATTR_DIR,
    B_ATTR_POS,
    B_ATTR_STATE,
    B_ATTR_ACTION,
    B_ATTR_INDEX,
    B_ATTR_HP,
    B_ATTR_NAMECOLOR,
    B_ATTR_LEVEL,
    B_ATTR_MAXHP,
    B_ATTR_DEF,
    B_ATTR_MDEF,
};

//场景属性类型属性类型:	定义的属性类型枚举值
enum R_ATTRIBUTE_TYPE
{
    R_ATTR_RGNTYPE=0x7000+1,
    R_ATTR_MERISCALE,
    R_ATTR_COUNTRY,
    R_ATTR_ISPK,
    R_ATTR_ISCONTRIBUTE,
    R_ATTR_ISRIDE,
    R_ATTR_SPACETYPE,
    R_ATTR_WARTYPE,
};
enum eMAILTYPE
{
    SYSTEM_MAIL,	//系统邮件
    COMMON_MAIL,	//普通邮件
    AFFIX_MAIL,		//普通附件
    PAYGOLD_MAIL,	//付费邮件
    SENDGOLD_MAIL,	//付费回复类邮件
    MAX_MAIL	= 100,	
    //PRESENT_MAIL,	//赠送邮件
    //REQUEST_MAIL,	//索取邮件
};

//	//采集物属性
enum C_ATTRIBUTE_TYPE
{
    C_ATTRIBUTE_CANCOLLECTION,
};

#define INVILID_VALUE -99999999

enum eClass
{
    C_Human=0,		//人形
    C_Element,		//元素
    C_Alien,		//异族
    C_Animal,		//动物
    C_Undead,		//亡灵
    C_Evil,			//恶魔
    C_Hexapods,		//昆虫

    C_Max,
};

//元素类型
typedef enum eElementalType
{
    ET_Fire=0,		//火系
    ET_Water,		//水系
    ET_Earth,		//土系
    ET_Wind,		//风系
    ET_Dark,		//暗系
    ET_Holy,		//圣系
    ET_Other,		//无类型

    ET_Max,
}eEleType;

//王峰：魔化攻防，属性相性
typedef enum eAttackDefend
{
    AD_Attack,		//属性攻击
    AD_Defend,		//属性防御
    AD_AttackXiang,	//相性攻击
    AD_DefendXiang,	//相性防御

    AD_Max,
    AD_Invalid = 0xFFFFFFFF,
}eAttackDefend;

//! 相性索引
enum eXiangXingIndex
{
    eXXV_3,
    eXXV_6,
    eXXV_9,
    eXXV_12,
    eXXV_15,

    eXXV_NUM,
};

//! 相性值
static long s_XiangXingValus[eXXV_NUM] = 
{
    3, 6, 9, 12, 15
};

//王峰：魔化元素类型
typedef enum eMagicElement
{
    ME_Earth,		//地系
    ME_Water,		//水系
    ME_Fire,		//火系
    ME_Wind,		//风系
    ME_Bright,		//光系
    ME_Dark,		//暗系

    MT_Max,
    MT_Invalid = 0xFFFFFFFF,
}eMagicElement;

//! 物品魔化孔类型（元素相性也使用这个枚举）
//! 使用10进制的模式，便于脚本操作
//! 物品孔的类型=10位上的类型+个位上的类型（其中，个位上的类型沿用eMagicElement的值）
enum eGoodsElementType
{
    eGET_Disabled	= 0,	//! 没有开启
    eGET_Enabled	= 1,	//! 初始开启
    eGET_Attack		= 10L,	//! 攻击类型
    eGET_Defend		= 20L,	//! 防御类型
};


enum eEquipLimitType
{
    ELT_UNKNOWN=0,
    ELT_LIGHT,
    ELT_MID,
    ELT_HEAVY,

    ELT_Max,
};

enum eNpcType
{
    NT_Normal=0,	//普通NPC
    NT_Monster,		//怪物
    NT_S_Monster,   //特殊怪物
    NT_Guard,		//守卫

    NT_Max,
};

//副职业系统职业列表
enum  eDeputyOccu		
{
    DOccu_Smithing=1,		//锻造
    DOccu_Metallurgy,		//炼金
    DOccu_Sewing,			//缝纫
    DOccu_Other,			//其他(主要用于任务)	
};

// 类型定义
enum OBJECT_TYPE
{
    TYPE_SESSION			= 10,	// 会话
    TYPE_PLUG				= 11,	// 会话的插头
    TYPE_WORLD				= 100,	// 世界
    TYPE_REGION				= 200,	// 地图场景
    TYPE_AREA				= 300,	// 区域（仅server有）

    TYPE_PLAYER				= 400,	// 玩家

    TYPE_COLLECTION			= 500,	// 采集物
    TYPE_MONSTER			= 600,	// 怪物
    TYPE_PET				= 604,	// 宠物

    TYPE_GOODS				= 700,	// 物品

    TYPE_SKILL				= 800,	// 技能（仅client有）
    TYPE_EFFECT				= 900,	// 特效（仅client有）

    TYPE_SUMMON				= 1000,	// 召唤物（比如火墙）

    TYPE_BUILD				= 1100, // 有生命的建筑物
    TYPE_CITYGATE			= 1200, // 城门
    TYPE_TALKBOXMODEL       = 1300, // TalkBox模型

    TYPE_COUNTRY			= 1400, // 国家
    TYPE_FACTION			= 1500,	// 行会
    TYPE_TEAM				= 1600, // 队伍
};

enum EVENT_TYPE
{
    EVENT_OnEquipMount		= 0,	//穿上装备
    EVENT_OnEquipUnMount,			//御下装备
};

enum QUEST_TYPE						//任务类型
{
    QUEST_MAIN=1,					//主线任务
    QUEST_LATERAL,					//支线任务
    QUEST_ENGAGEARMS,				//佣兵任务
    QUEST_OTHER						//其他任务
};

//定义游戏世界的实体对象标示
struct stDestObj
{
    long nDestType;
    CGUID DestID;
};

// 定义场景类型
enum eRgnType
{
    RGN_NORMAL = 0,
    RGN_PERSONAL,
    RGN_PERSONAL_HOUSE,
    RGN_TEAM,
    RGN_PROXY,
    RGN_VILLAGE,		// 村站地图
    RGN_CITY,			// 城
    RGN_INVALID = 0xFE, // 非法类型
    RGN_ALL = 0xFF,		// 表示所有场景类型
};
// 玩家状态类型
enum ePlayerState
{
    PLAYER_STATE_VOID,					// 玩家处于无效状态(初始值)
    PLAYER_STATE_LOADED_BASE_DATA,		// 已读取基本数据
    PLAYER_STATE_LOADED_CHAR_DATA,		// 已读取角色数据
    PLAYER_STATE_ENTERING_GAME,			// 正在进入游戏
    PLAYER_STATE_ENTERED_GAME,			// 已进入游戏
    PLAYER_STATE_EXITED_GAME,			// 已退出游戏
    PLAYER_STATE_CHANGING_SERVER,		// 切换服务器中
    PLAYER_STATE_CREATING,				// 正在创建玩家对象
    PLAYER_STATE_BACKTOLOGIN,			// 退回登录界面
};

// 玩家需要保存的属性部分枚举值
enum ePlayerSavePartFlag
{
    PLAYER_SAVE_BASEATTRS = 0,			// 基本属性:金钱,等级...等
    PLAYER_SAVE_ORIGINALPACKGOODS,		// 主背包物品
    PLAYER_SAVE_SUBPACK1GOODS,			// 子背包1物品
    PLAYER_SAVE_SUBPACK2GOODS,			// 子背包2物品
    PLAYER_SAVE_SUBPACK3GOODS,			// 子背包3物品
    PLAYER_SAVE_SUBPACK4GOODS,			// 子背包4物品
    PLAYER_SAVE_SUBPACK5GOODS,			// 子背包5物品
    PLAYER_SAVE_VARLIST,				// 变量列表
    PLAYER_SAVE_QUESTLIST,				// 变量列表
    PLAYER_SAVE_SKILLLIST,				// 技能列表
    PLAYER_SAVE_STATEIST,				// 状态列表
};

// 聊天类型
enum eChatType
{
    CHAT_NORMAL = 0,		// 同屏聊天
    CHAT_REGION,			// 同场景区域聊天
    CHAT_UNION,				// 联盟聊天
    CHAT_FACTION,			// 行会聊天
    CHAT_TEAM,				// 队伍聊天
    CHAT_PRIVATE,			// 两人私聊
    CHAT_COUNTRY,			// 国家聊天
    CHAT_COUNTRY_UNION,		// 盟国聊天
    CHAT_WORLD,				// 世界聊天

    CHAT_SYSTEM,			// 系统频道

    CHAT_BOARDCAST,			// 广播

    GM_CMD,					// GM命令
    RUN_SCRIPT,				// 运行脚本
    RUN_HELP,				// 帮助
    PLAYER_CMD,				// 玩家命令

    CHAT_NPC,				// NPC说话

    TYPE_NUM,
};

//! 物品信息所在类型
enum eChatsGoodsInfoType
{
    eCGIT_Player,	//! 物品在玩家身上
    eCGIT_Npc,		//！物品在Npc
    eCGIT_Copy,		//! 物品是从聊天信息中复制
};

//! 聊天中附带的物品信息
struct tagChatsGoodsInfo
{	
    long	color;
    long	posBegin;
    long	posEnd;

    long	lInfoType;	//! 物品信息所在类型 eChatsGoodsInfoType
    CGUID	OwnerGuid;	//! 拥有者GUID，如果拥有者类型为eCGIT_Copy，则不需要设置该值
    CGUID	KeyGuid;	//! C2S时昨物品源GUID或物品信息GUID，S2C时只作物品信息GUID
};

//! 玩家的称号类型
enum ePlayerTitleType
{	
    ePTT_Hide,		//! 称号隐藏
    ePTT_Knight,	//! 爵位称号
    ePTT_Faction,	//! 帮会称号
    ePTT_Medal1,	//! 勋章1称号
    ePTT_Medal2,	//! 勋章2称号
    ePTT_Medal3,	//! 勋章3称号
    ePTT_LOVER,	    //! 配偶称号 
};

//! 增加经验的类型
enum eAddExpType
{
    eAET_Monster,	//! 怪物经验
    eAET_FacBoon,	//! 工会福利经验
    eAET_NPC,		//! NPC经验
    eAET_Script,	//! 脚本
};

enum RET_CREATE_RGN_FLAG
{
    C_RET_NOT_FIND_OWNER_ERROR	= -7,   // 未在WS上找到该玩家
    C_RET_RGN_HAS_CREATED_ERROR	= -6,	// 该副本已创建
    C_RET_TIMED_RGNS_FULL_ERROR	= -4,	// 玩家限时内创建此类副本已达上限
    C_RET_ALL_RGNS_FULL_ERROR	= -3,	// 服务器副本空间已满
    C_RET_THESE_RGNS_FULL_ERROR	= -2,	// 此类副本已满
    C_RET_RGN_TEMPLAYTEID_ERROR	= -1,	// 蓝图ID不存在或不是此次要创建的副本类型
    C_RET_LOCAL_SUCCESS			= 1,	// 服务器本地创建副本成功
    C_RET_REMOTE_SUCCESS		= 2,	// 服务器远程创建副本成功
};
enum RET_TEAMRGN_REFRESH_FLAG
{
    RET_TEAMRGN_REMOTE_OK = 2,
    RET_TEAMRGN_LOCAL_OK  = 1,
    RET_TEAMRGN_LOCAL_ERROR = -1,
    RET_TEAMRGN_REMOTE_ERROR = -2,
};

//! 消息MSG_S2C_OTHER_TIME_INFO_BOX中指定的文字类容
enum eOtherTimeInfoType
{
    AT_ONCE_KICK,			//! 立即踢人通知（默认）
    RGN_CLEARDUPRGNPLAYER,	//! 副本踢人通知
    FCM_KICK_PLAYER,		//! 防沉迷踢人通知 
};

// 游戏外赠送系统奖励策略的类型
enum eAwardType
{
    AWARD_GOODS = 1,	// 道具
    AWARD_PROPERTY,		// 属性
    AWARD_SCRIPT,		// 脚本
};

#define SERVER_KEY_DWORD_SIZE 4

const ulong ACCOUNT_SIZE = 32;
const ulong TITLE_SIZE = 32;

const ulong PLAYER_NAME_SIZE = 20;

//! 通用物品容器的最大格子数
const ulong MAX_ALLPURPOSE_CONTAINER_SIZE = 4;
//! 阵营类型
enum eCampType
{
    eCampUnion  = 0, // 联邦
    eCampEmpire = 1  // 帝国
};

//! 变量类型
enum eVariableType
{
    eVarNumber = 0, // 数值型
    eVarString = 1, // 字符串型
    eVarGuid   = 2  // GUID型
};

// 阵营个数
const int COUNTRY_NUM   = 6;

//! 用于GMT工具
//! ---------------------------------------------------------------------------------------------------
//GAPP类型
enum eGappType
{
    eGT_INVALID = 0,
    eGT_SM	= 1,			//ServerMonitor
    eGT_AS,					//AccountServer
    eGT_LOGS,				//LogServer
    eGT_BS,					//BillServer
    eGT_LS,					//LoginServer
    eGT_DB,					//DBServer
    eGT_WS,					//WorldServer	
    eGT_GS,					//GameServer

};
//! GM操作方式
enum eGmOptType
{
    eGOT_Ban,			//! 封停
    eGOT_Unban,			//! 解封

    eGOT_ShutUp,		//! 禁言
    eGOT_OpenUp,		//! 解除禁言
};
//! 举报类型
enum eReportType
{
    eRT_LawlessSoft,	//! 外挂
    eRT_LawlessInfo,	//! 传播非法信息
    eRT_UseBug,			//! 利用bug
};

//! 玩家行为
enum eGmtGmCommand
{
    eGGC_Move,			//! 移动
    eGGC_Skill,			//! 使用技能
    eGGC_TouchNpc,		//! 解发NPC
    eGGC_NomalChat,		//! 普通聊天
    eGGC_GmChat,		//! GM聊天
};

// 当前语言标志(注意该枚举内的定义顺序请不要轻易变更,需要添加则按顺序加在末尾)
enum CURRENT_LANGUAGE
{
    CL_CHINESE = 0,		//中文
    CL_KOREA,			//朝鲜语
    CL_JAPANESE,		//日语

    CL_NUM,
};

const int GAPP_NAME_SZIE=32;
//! 基本举报处理信息
struct tagBaseResultInfo 
{
    //!				被告名字
    char			_IndicteeName[GAPP_NAME_SZIE];
    //!				处理者名字
    char			_szGmName[GAPP_NAME_SZIE];
    //!				处理时间
    DWORD			_OptTime;
    //!				处理结果
    DWORD			_OptResult;		//	封号。。。。
    //!				原因
    DWORD			_dwCause;		//	使用外挂。。。
    //!				-1是解封，0是永久封停,非0是封停时间
    long			_lBanTime;
};

//! 完整举报处理信息
struct tagReportResultInfo : public tagBaseResultInfo
{
    //!				处理原因描述（长度应小于MAX_AFFICHE_SIZE）
    string			_strWhys;
    //!				处理依据记录（长度小于8000）
    string			_strOptLog;
};

//! 基本举报信息
struct tagBaseReportInfo
{
    //!				被告名字
    char			_IndicteeName[GAPP_NAME_SZIE];
    //!				举报者名字
    char			_ReportName[GAPP_NAME_SZIE];
    //!				举报类型(以 中定义的 类型为准)
    DWORD			_ReportType;
    //!				举报时间
    DWORD			_ReportTime;
};

//! 举报信息
struct tagReportInfo : public tagBaseReportInfo
{
    //!				世界服务器的SMID
    DWORD			_WsSmId;
    //!				世界服务器的GappID
    DWORD			_WsGappId;
};

//! 公告最大长度
const DWORD MAX_AFFICHE_SIZE = 256;


//! 密码最大字数
const DWORD PASSWORD_CHAT_COUNT			= 16;
const DWORD PASSWORD_SIZE				= PASSWORD_CHAT_COUNT + 1;
