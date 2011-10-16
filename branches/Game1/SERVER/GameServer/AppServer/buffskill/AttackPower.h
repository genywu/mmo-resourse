#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_ATTACKPOWER_INCLUDED
#define _INC_ATTACKPOWER_INCLUDED
#include "..\..\..\..\Public\SkillRelated.h"

enum AttackType
{
	ATTACK_TYPE_UNKNOW = 0,		//##未知类型攻击，构造用
	ATTACK_TYPE_PHYSICAL,		//##物理攻击
	ATTACK_TYPE_MAGERY,			//##魔法攻击
	ATTACK_TYPE_ELEMENT,		//##元素攻击
	ATTACK_TYPE_SOUL,			//##灵术攻击
	ATTACK_TYPE_POISON,			//##毒攻击
};

enum eDamageResult
{
	DAMAGE_RESULT_Damage,//受伤
	DAMAGE_RESULT_Dodge,//闪避
	DAMAGE_RESULT_FightBlock,//格挡
	DAMAGE_RESULT_Parry,//招架
	DAMAGE_RESULT_Resist,//抵抗
	DAMAGE_RESULT_Suck,//吸收
	DAMAGE_RESULT_Rebound,//反弹	
	DAMAGE_RESULT_Lose,//丢失
	DAMAGE_RESULT_IncHp,//加血
	DAMAGE_RESULT_DamageEx,//额外受伤
	DAMAGE_RESULT_DotaDamage,//DOTA伤害
};

//##描述某种攻击对对方造成的某种伤害
struct tagAttackPower : public BaseMemObj
{
	AttackType	atAttackType;					//##攻击类型
	LONG		lHPDamage;						//##对HP的伤害
	LONG		lMPDamage;						//##对MP的伤害
	LONG		lEnergyDamage;					//##对能量的伤害
	DWORD		dwOtherEffect;					//##其他的附加效果，如打晕，后退等等。
	DWORD		dwEffectPresentDegree;			//##表现的程度
	DWORD		dwEffectPresistTime;			//##持续的时间

	tagAttackPower				();
};

//##攻击者的详细描述，以及攻击的伤害强度。
struct tagAttackInformation
{
	tagSkillID				tSkillID;				//##攻击技能编号
	BYTE					bySkillLevel;			//##攻击机能等级
	LONG					lAttackerType;			//##攻击者类型
	CGUID					AttackerID;				//##攻击者索引
	bool					bCritical;				//##攻击是否是全力一击
	bool					bBlastAttack;			//##攻击是否是爆击
	LONG					lTargetDodgeModifier;	//##目标的躲避修正
	LONG					lHitModifier;			//##最终命中修正值
	FLOAT					fDamageFactor;			//##最终伤害系数[0.0-n]
	LONG					lDamageModifier;		//##最终伤害修正值
	LONG					bFullMiss;				//##是否被完全防御(1), 格当(2);

	//##攻击者在攻击瞬间的PVP开关状态，只对CPlayer有效
	bool					bAllowKillPlayer;		//##释放者想攻击任意的玩家
	bool					bAllowKillTeammate;		//##释放者想攻击队友
	bool					bAllowKillGuildMember;	//##释放者想攻击工会会员
	bool					bAllowKillCriminal;		//##释放者想攻击罪犯
	bool					bAllowKillCountry;		//##释放者想他国玩家
	CGUID					AttackerTeamID;		//##释放者的Team编号
	CGUID					AttackerGuildID;		//##释放者的帮会编号
	CGUID					AttackerUnionID;		//##释放者的同盟编号

	vector<tagAttackPower*> vDamage;				//##伤害列表

	VOID					Clear();

	tagAttackInformation();
	~tagAttackInformation();
};


//##描述某种行为对承受方造成的某种影响。

#endif