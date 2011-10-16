
//定义和Player相关的结构、枚举
#ifndef PLAYER_PROPERTYDEF_H
#define PLAYER_PROPERTYDEF_H

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#include "BaseDef.h"
#include "StateDef.h"

//人物的属性(存盘属性)
typedef struct tagPlayerProperty
{
	eOccupation	byOccu;					//职业
	eDeputyOccu byAssOccu;				//副职业
	eConst		byConst;				//星座
	BYTE		bySex;					//性别(0:男 1:女) 
	BYTE		byClass;				//生物种类
	
	long		lHairPic;				//头发图片ID 
	long		lFacePic;				//面部图片ID 

	DWORD		dwHp;					//当前生命值 
	WORD		wMp;					//当前魔法值 
	WORD		wEnergy;				//当前能量值 
	WORD		wStamina;				//当前体力	
	WORD		wLevel;					//等级 

	DWORD		dwExpMultiple;			//当前经验值倍数
	DWORD		dwExp;					//当前经验值
	DWORD		dwPresentExp;			//赠送经验
	DWORD		dwUpgradeExpMultiple;	//升级所需经验倍数
	DWORD		dwUpgradeExp;			//升级所需经验值
	BYTE		byOccuLvl[OCC_Max];		//每个职业的等级
	DWORD		dwOccuExp[OCC_Max];		//每个职业的经验
	DWORD		dwCurOccuUpgradeExp;	//当前职业的升级经验

	BYTE		byRankOfNobility;		//爵位等级
	DWORD		dwRankOfNobCredit;		//爵位的声望值
	DWORD		dwBatakCredit;			//巴塔克声望
	DWORD		dwMedalScores;			//勋章分数
	BYTE		byRankOfMercenary;		//佣兵等级
	DWORD		dwMercenaryCredit;		//佣兵的声望值

	BYTE		byCountry;				//国家ID
	long		lCountryContribute;		//对国家的贡献值s
	DWORD		dwSpouseID;				//配偶身份ID 
	DWORD		dwSpouseParam;			//和配偶的关系值
	BYTE		byBusinessLevel;		//商业等级
	BYTE		byIsBusinessMan;		//是否是商人
	DWORD		dwBusinessExp;			//商业经验
	DWORD		dwArtisanCredit;		//工匠声望
	BYTE		byArtisanLevel;			//工匠等级
	DWORD		dwArtisanExp;			//工匠经验
	DWORD		dwChurchCredit;			//教会声望
	DWORD		dwZanDoHunterCredit;	//赞多狩猎声望值
	DWORD		dwPkCount;				//玩家PK杀死国内人数
	DWORD		dwPkValue;				//玩家PK值(犯罪值)
	DWORD		dwPVPCount;				//玩家PVP杀外国人数量
	DWORD		dwPVPValue;				//玩家PVP值(恶名值)
	DWORD		dwHotKey[24];			//热健(F1~F12,1~+) 
	WORD		wPKOnOff;				//PK开关(b1:全体保护,b2:组队保护,b4:工会保护,b8:红名保护,b16:本国保护,b32:阵营保护,b64.海盗保护,b128攻击提示，b256自动选人，b512自动选怪,)
	DWORD		dwOccuSP[OCC_Max];		//每个职业的技能点数
	DWORD		dwLastExitGameTime;		//上一次退出游戏的时间
	bool		bShowCountry;			//显示国家标志

	long		lCurrBaseActive;		//! 剩余基本CP
	long		lCurrExActive;			//! 剩余扩展CP
	ULONG		lLastResetActiveTime;	//! 上次重置CP时间

	long		lCurPKCP;				///< 当前PK CP值
	long		lMarriageStep;			//! 结婚步骤
}tagPPro;

//! 单条元素属性
typedef struct tagPlayerElementProperty
{
	long		uArrPro[AD_Max];

	tagPlayerElementProperty(void){memset(this, 0, sizeof(tagPlayerElementProperty));}
}tagPlayerElementPro;


// 人物的战斗属性
typedef struct tagPlayerFightProperty
{
	long		dwMaxHp;				//生命上限 
	short		wMaxMp;					//法力上限 
	short		wMaxStamina;			//体力上限 
	short		wMaxEnergy;				//能量上限 
	short		wHpRecoverSpeed;		//生命恢复速度
	short		wFightHpRecoverSpeed;	//战斗生命恢复速度
	short		wMpRecoverSpeed;		//法力恢复速度
	short		wFightMpRecoverSpeed;	//战斗法力恢复速度
	short		wHpRecoverRatio;		//HP恢复比例
	///////////////////////////
	short		wEnerRecoverSpeed;		//能量恢复速度
	short		wFightEnerRecoverSpeed;	//战斗能量恢复速度 	
	///////////////////////////
	short		wStamRecoverSpeed;		//体力恢复速度 
	short		wStrenth;				//力量 
	short		wAgility;				//敏捷 
	short		wConsti;				//体质 
	short		wIntell;				//智力 
	short		wWisdom;				//智慧 
	short		wSpirit;				//精神 
	short		wFightBlock;			//格挡 
	short		wParry;					//招架	
	short		wDodge;					//闪避
	short		wCharm;					//魅力 
	short		wHit;					//命中 
	short		wAbsorb;				//专注 
	long		wDef;						//LONG 物理防御
	short		wShieldDef;				//盾牌物理防御
	long		wMdef;						//LONG 魔法抗性
	long		wCri;						//LONG 物理爆击率
	short		wMAtkSpeed;				//施法速度
	short		wBuffPower;				//Buff攻击强度
	long		wMinAtk;					//LONG 最小攻击力
	long		wMaxAtk;					//LONG 最大攻击力
	long		wMAtk;						//LONG 魔法攻击力
	long		wMCri;						//LONG 魔法爆击率
	short		wBloodSuk;				//生命吸收率
	short		wManaSuck;				//法力吸收率
	short		wEnergySuck;			//能量吸收率
	short		wWillDef;				//意志抗性
	short		wConstiDef;				//体质抗性
	short		wCriDef;				//物理爆击抗性
	short		wMCriDef;				//魔法爆击抗性
	short		wCriDamage;				//物理暴击伤害
	short		wMCriDamage;			//魔法暴击伤害	
	short		wPhysicImmunity;		//物理免疫
	short		wMagicImmunity;			//魔法免疫
	short		wImmunity;				//免伤
	short		wPierce;				//穿透
	short		wLuck;					//幸运
	short		wDeBuffDef[DET_Max];	//Debuff抗性
	short		wElemDef[ET_Max];		//各个元素抗性
	short		wClassDam[C_Max];		//对各个种族伤害
	short		wElemDam[ET_Max];		//对各个元素伤害
	short		wElemAtk[ET_Max];		//各个元素的攻击值

	long		lMaxPKCP;				///< 最大PK CP值 

}tagPFPro;

//表示与郡和友好关系都
struct tagFriendshipCounty
{
	DWORD	dwCountyID;				//郡ID
	DWORD	dwFrienDeg;				//友好度
};
//保存附加属性
typedef struct tagPlayerAppendProperty
{
	bool		bIsCharged;					// 是否充值
	BYTE		bShowFashion;				// 头盔,披风,外套的显示状态
	char		strAccount[ACCOUNT_SIZE];	// 帐号
	char		strTitle[TITLE_SIZE];		// 称号	
}tagPAPro;

enum AREA_CREDIT_LEVEL
{
	CREDIT_LEV_NORMAL=1,
	//CREDIT_LEV_
};

enum SAVE_DETAIL_ATTR_TYPE
{
	SAVE_DETAIL_NONE				= 0,
	SAVE_DETAIL_ATTRPROPERTY		= 0x00000001,	// 基本属性:金钱,等级...等
	SAVE_DETAIL_ATTREQUIPMENTGOODS	= 0x00000002,	// 装备物品
	SAVE_DETAIL_ATTRPACKGOODS		= 0x00000004,	// 主背包物品
	SAVE_DETAIL_ATTRSUBPACKGOODS1	= 0x00000008,	// 子背包1物品
	SAVE_DETAIL_ATTRSUBPACKGOODS2	= 0x00000010,	// 子背包2物品
	SAVE_DETAIL_ATTRSUBPACKGOODS3	= 0x00000020,	// 子背包3物品
	SAVE_DETAIL_ATTRSUBPACKGOODS4	= 0x00000040,	// 子背包4物品
	SAVE_DETAIL_ATTRSUBPACKGOODS5	= 0x00000080,	// 子背包5物品
	SAVE_DETAIL_ATTRWALLETGOODS		= 0x00000100,	// 钱包物品
	SAVE_DETAIL_ATTRSILVERGOODS		= 0x00000200,	// 银币物品
	SAVE_DETAIL_ATTRDEPOTGOLD		= 0x00000400,	// 黄金仓库
	SAVE_DETAIL_ATTRDEPOTSILVER		= 0x00000800,	// 银币仓库
	SAVE_DETAIL_ATTRPRIMARY			= 0x00001000,	// 主仓库
	SAVE_DETAIL_ATTRSUBDEPOT1		= 0x00002000,	// 子仓库1
	SAVE_DETAIL_ATTRSUBDEPOT2		= 0x00004000,	// 子仓库2
	SAVE_DETAIL_ATTRSUBDEPOT3		= 0x00008000,	// 子仓库3
	SAVE_DETAIL_ATTRSUBDEPOT4		= 0x00010000,	// 子仓库4
	SAVE_DETAIL_ATTRBUSINESSPACK	= 0x00020000,	// 商业背包
	SAVE_DETAIL_ATTRVARLIST			= 0x00040000,	// 变量列表
	SAVE_DETAIL_ATTRQUESTLIST		= 0x00080000,	// 任务列表
	SAVE_DETAIL_ATTRSKILLLIST		= 0x00100000,	// 技能列表
	SAVE_DETAIL_ATTRSTATELIST		= 0x00200000,	// 状态列表
	SAVE_DETAIL_ATTRDEPUTYOCCU		= 0x00400000,	// 副职业
	SAVE_DETAIL_ATTRMERQUESTLIST	= 0x00800000,	// 俑兵任务列表
	SAVE_DETAIL_PETS				= 0x01000000,	// 宠物列表
    SAVE_DETAIL_INNERHOOK           = 0x02000000,   // 挂机数据,现在只保存稳定挂机时间
	
	//SAVE_DETAIL_ATTRMAILLIST		= 0x00800000,	// 邮件列表
	SAVE_DETAIL_ALL					= 0xFFFFFFFF,	// 宠物列表
};													

//经验值倍率值
const static DWORD dwExpMutipleValue = 2000000000;

#endif												
// 状态列表