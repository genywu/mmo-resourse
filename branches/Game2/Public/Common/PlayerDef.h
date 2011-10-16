
//定义和Player相关的结构、枚举
#ifndef PLAYER_PROPERTYDEF_H
#define PLAYER_PROPERTYDEF_H

#ifndef _USE_32BIT_TIME_T
#define _USE_32BIT_TIME_T
#endif

#include "BaseDef.h"
#include "StateDef.h"

// TODO: 将发布公告的类型重新调整
enum eNOTIFY_POS
{
    eNOTIFYPOS_LEFT_BOTTOM=1,//左下
    eNOTIFYPOS_RIGHT_BOTTOM,//右下
    eNOTIFYPOS_CENTER,//中间
    eNOTIFYPOS_SYSTEM,//系统
};

const DWORD NOTIFY_COLOR_RED=0xffff0000;
const DWORD NOTIFY_COLOR_YELLOW=0xffffff00;
const DWORD NOTIFY_COLOR_WHITE=0xffffffff;

//人物的属性(存盘属性)
typedef struct tagPlayerProperty
{
	eOccupation	byOccu;					//职业(流派)
	eDeputyOccu byAssOccu;				//副职业
	uchar		bySex;					//性别(0:男 1:女) 
	uchar		byHairPic;				//头发图片ID 
	uchar		byFacePic;				//面部图片ID 

	ulong		dwHp;					//当前生命值 
	ushort		wMp;					//当前魔法值 
	ushort		wEnergy;				//当前能量值(内力) 
	ushort		wStamina;				//当前怒气	
	ushort		wLevel;					//等级 

    ulong		dwExp;					//当前经验值
	ulong		dwExpMultiple;			//当前经验值倍数	
	ulong		dwPresentExp;			//赠送经验
	ulong		dwUpgradeExpMultiple;	//升级所需经验倍数
	ulong		dwUpgradeExp;			//升级所需经验值
	uchar		byOccuLvl[OCC_Max];		//每个职业的等级
	ulong		dwOccuExp[OCC_Max];		//每个职业的经验
	ulong		dwCurOccuUpgradeExp;	//当前职业的升级经验

    ushort      wTaoismLvl;             // 道行等级
    ulong       dwTaoismExp;            // 修为值，用于提升道行等级
    ulong       dwRelationVal;          // 人脉值，队伍中的低等级角色升级时，一定范围内高于他一定等级角色能获得的值，用于进行消费的值
    ulong       dwHiddenVirtue;         // 阴德 玩家行为所累计的数值，会影响某些事件的成功率
    ushort      wVigour;                // 活力，生活技能消耗（丹药、烹调、酿酒）
    ushort      wThew;                  // 体力，生活技能消耗（打造防具、武器、饰品、法宝等）

    short       wPoint2Str;                   //分配力量
    short       wPoint2Dex;                   //分配身法
    short       wPoint2Con;                   //分配根骨
    short       wPoint2Intellect;             //分配意志
    short       wPoint2Spiritualism;          //分配灵性

	uchar		byCountry;				        //国家ID
	long		lCountryContribute;		        //对国家的贡献值s
    ulong       dwCountryCredit[COUNTRY_NUM];   //玩家在各个阵营中的声望
    uchar       byCountryStatus;                //阵营地位

	ulong		dwSpouseID;				//配偶身份ID 
	ulong		dwSpouseParam;			//和配偶的关系值
	uchar		byBusinessLevel;		//商业等级
	uchar		byIsBusinessMan;		//是否是商人
	ulong		dwBusinessExp;			//商业经验

	ulong		dwPkCount;				//玩家PK杀死国内人数
	ulong		dwPkValue;				//玩家PK值(犯罪值)
	ulong		dwPVPCount;				//玩家PVP杀外国人数量
	ulong		dwPVPValue;				//玩家PVP值(恶名值)    

	ulong		dwHotKey[24];			//热健(F1~F12,1~+) 
	uchar		byPKOnOff;				//PK开关(b1:全体保护,b2:组队保护,b4:工会保护,b8:红名保护,b16:本国保护,b32:阵营保护,b64.海盗保护)
	ulong		dwOccuSP[OCC_Max];		//每个职业的技能点数
	ulong		dwLastExitGameTime;		//上一次退出游戏的时间
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
	short		wMaxStamina;			//怒气上限 
	short		wMaxEnergy;				//内力上限 
	short		wHpRecoverSpeed;		//生命恢复速度
    short		wMpRecoverSpeed;		//法力恢复速度    
	short		wHpRecoverRatio;		//HP恢复比例
	short		wEnerRecoverSpeed;		//内力恢复速度
    short		wStamRecoverSpeed;		//怒气恢复速度 
    short		wFightHpRecoverSpeed;	//战斗生命恢复速度	
    short		wFightMpRecoverSpeed;	//战斗法力恢复速度
	short		wFightEnerRecoverSpeed;	//战斗内力恢复速度
    short       wFightStamRecoverSpeed; //战斗怒气恢复速度

    short       wManly;                 //霸气，会随时间减少的值，处于不同数值区间段，降低速度不同
    short       wPerverseTrend;         //煞气，角色杀人堆叠的值
    short       wComplaint;             //怨气，角色被其他玩家击杀增加的值
    short       wMurderousLook;         //杀气，角色击杀敌对阵营玩家增加的值

    short       wStr;                   //力量
    short       wDex;                   //身法
    short       wCon;                   //根骨
    short       wIntellect;             //意志
    short       wSpiritualism;          //灵性
    short       wPoint;                 //潜能点(剩余待分配的属性点)

    short		wParry;					//招架等级(对应玩家等级，换算招架几率)
    float       fParryRatio;            //招架几率(招架不受外功伤害)
    short		wDodge;					//闪避等级(对应玩家等级，换算闪避几率)
    float       fDodgeRatio;            //闪避几率
    ulong       lArmorDef;              //护甲值(对应玩家等级，换算为外功伤害吸收率)
    float       fDefSuck;               //伤害吸收率
    ulong       lEnergyDef;             //内功抗性
    float       fEnergySuck;            //内功吸收率
    short       wOutEnergyHit;          //外功命中等级
    float       fOutEnergyHitRatio;     //外功命中几率
    short       wEnergyHit;             //内功命中等级
    float       fEnergyHitRatio;        //内功命中几率
    short       wOutEnergySpeed;        //外功加速等级
    float       fOutEnergySpeedRatio;   //外功加速比例
    short       wEnergySpeed;           //内功加速等级
    float       fEnergySpeedRatio;      //内功加速比例

    short       wOutEnergyHurt;         //外功伤害
    short       wEnergyHurtInc;         //外功伤害加成
    short       wEnergyHealInc;         //外功治疗加成
    short       wOutEnergyBlast;        //外功暴击等级
    short       wOutEnergyBlastRate;    //外功暴击率

    short		wDef;					//物理防御
    short		wMdef;					//魔法抗性
    short       wMdefSuck;              //魔法攻击吸收率
    short		wBloodSuck;				//生命吸收率
    short		wManaSuck;				//法力吸收率
    short       wStaminaSuck;           //怒气吸收率

    short		wHit;					//命中 
    short       wAtkSpeed;              //攻击速度
    short		wMAtkSpeed;				//施法速度
    short		wMinAtk;				//最小攻击力
    short		wMaxAtk;				//最大攻击力
    short       wMinMAtk;               //最小魔法攻击力
    short       wMaxMAtk;               //最大魔法攻击力
    short		wPhysicImmunity;		//物理免疫
    short		wMagicImmunity;			//魔法免疫
    short       wMoveSpeed;             //移动速度
    short       wMoveAcceleration;      //移动加速度
    short       wMoveRadius;            //移动半径
    short       wTurnSpeed;             //转身速度
    short		wElemDef[ET_Max];		//各系法术抗性
    short		wDeBuffDef[DET_Max];	//Debuff抗性
    short		wClassDam[C_Max];		//对各个种族伤害
 
	short		wFightBlock;			//格挡	
	short		wCharm;					//魅力 	
	short		wAbsorb;				//专注	
	short		wShieldDef;				//盾牌物理防御	
    short		wCri;					//物理爆击率	
	short		wBuffPower;				//Buff攻击强度	
	long		wMAtk;					//魔法攻击力
	long		wMCri;					//魔法爆击率	
	short		wWillDef;				//意志抗性
	short		wConstiDef;				//体质抗性
	short		wCriDef;				//物理爆击抗性
	short		wMCriDef;				//魔法爆击抗性
	short		wCriDamage;				//物理暴击伤害
	short		wMCriDamage;			//魔法暴击伤害
	short		wImmunity;				//免伤
	short		wPierce;				//穿透
	short		wLuck;					//幸运	
	short		wElemDam[ET_Max];		//对各个元素伤害
	short		wElemAtk[ET_Max];		//各个元素的攻击值
	long		lMaxPKCP;				///< 最大PK CP值
    short       HurtFactor;             //! 伤害系数

}tagPFPro;

//表示与郡和友好关系都
struct tagFriendshipCounty
{
	ulong	dwCountyID;				//郡ID
	ulong	dwFrienDeg;				//友好度
};
//保存附加属性
typedef struct tagPlayerAppendProperty
{
	bool		bIsCharged;					// 是否充值
	uchar		bShowFashion;				// 头盔,披风,外套的显示状态
	char		strAccount[ACCOUNT_SIZE];	// 帐号
	char		strTitle[TITLE_SIZE];		// 称号	

}tagPAPro;

//结义金兰及亲密度
struct tagBrotherInfo
{
    CGUID   guidBrother;        //兄弟guid
    long    lFriendVal;         //亲密度
};

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
    SAVE_DETAIL_ATTRPROFESSION		= 0x00400000,	// 副职业
    SAVE_DETAIL_ATTRMERQUESTLIST	= 0x00800000,	// 俑兵任务列表
    SAVE_DETAIL_PETS				= 0x01000000,	// 宠物列表
    SAVE_DETAIL_EQUIPMENTGOD        = 0x02000000, 
    SAVE_DETAIL_EQUIPMENTLAND       = 0x04000000, 
    SAVE_DETAIL_EQUIPMENTHUMAN      = 0x08000000, 

    //SAVE_DETAIL_ATTRMAILLIST		= 0x00800000,	// 邮件列表
    SAVE_DETAIL_ALL					= 0xFFFFFFFF,	// 宠物列表
};													

//经验值倍率值
const static ulong dwExpMutipleValue = 2000000000;

#endif												
// 状态列表