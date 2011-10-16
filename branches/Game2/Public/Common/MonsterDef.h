//定义和怪物相关的枚举和结构

#ifndef MONSTER_DEF_H
#define MONSTER_DEF_H

#include "BaseDef.h"
#include "StateDef.h"

#define MONSTER_DIS_FLOAT
#ifdef MONSTER_DIS_FLOAT
#define DisType float
#define DisTypeID 3 /* for InitNameValueMap */
#define MemP pfValue
#else
#define DisType short
#define DisTypeID 5
#define MemP psValue
#endif


typedef struct tagMonsterProperty
{   
    eEleType	eEType;					//元素类别
	eNpcType	eNType;					//Npc类别 (NNPC,Monster,Guard)
	eClass		eClass_;				//种类

    uchar		byBlockType;			//阻挡类型
    uchar		byCountry;				//国家ID
	uchar		byCatchable;			//是否可以捕获
    uchar		byIsFlyToPeace;			//转换为和平状态时候，是否飞回去
	uchar		byIsResumeAll;			//转换为和平状态的时候是否完全恢复

	ushort		wTitleColor;			//称谓颜色
	ushort		wTitlePicID;			//称谓图形id
    ushort		wLevel;				    //等级
    ushort		wCountryContriValue;	//国家贡献值
	ushort		wSPContriValue;			//SP贡献值
    ushort      wSkillProportion;       //技能概率,一个小于等于100，大于等于0的整型。
    DisType		ReturnRange;			//归位距离
	DisType		PeaceMoveRange;		//和平移动范围
    ushort		wMinStopTimes;			//最小停留次数
	ushort		wMaxStopTimes;			//最大停留次数
	ushort		wMinRunTimes;			//最少行走步数
	ushort		wMaxRunTimes;			//最多行走步数
	ushort		wCanTalk;				//是否能对话
    ushort		wRace;					//族群(AI使用)
	ushort		wMoveRandomValue;		//产生移动的概率(活力)
	ushort		wStopTime;				//停留的时间(在一个格子中间停留帧数)
	ushort		wAIHangupType;			//AI挂起方式
	ushort		wFigure;				//体形(0:1x1 1:3*3 2:5*5)*
	ushort		wBossLevel;				//boss等级

	float       fHurtProportion;        //重伤比率         
    float       fEscapePoint;           //逃跑底线（逃跑点）
    float		fMoveSpeed;				//移动速度
	float		fFightMoveSpeed;		//战斗移动速度
	float       fTurningSpeed;			//转身速度

    long		lRefeashIndex;			//更新索引

	ulong       dwGuardTime;            //警戒时间
	ulong       dwSkillSpace;           //技能间隔
	ulong		dwChaseTime;			//追击时间，在追击范围外的没受到攻击转化为和平的时间
	ulong       dwAIReferrence[MONSTER_AI_REFERRENCE_NUM];      //AI参考
    ulong		dwHp;					//当前生命值
	ulong       dwAIType;               //AI类型
    ulong		dwIndex;				//种类索引编号
	ulong		dwPicID;				//图形ID
	ulong		dwPicLevel;				//图形等级
	ulong		dwNameColor;			//名字颜色编号,对应到客户端配置
	ulong		dwHpBarColor;			//血条颜色，0：表示不显示血条 1：红色 2：黄色
	ulong		dwSoundID;				//声音ID
	ulong		dwExp;					//经验值*
	ulong		dwDExp;					//修为经验值
	ulong		dwOccuPoint;			//职业点数
	ulong		dwMaxCatchFastness;		//怪物最大捕获抗性
	ulong		dwServantID;			//怪物对应侍从ID

}tagMPro;


// 怪物的战斗属性
typedef struct tagMonsterFightProperty
{	
	short		wHpRecoverSpeed;		//生命恢复速度
	short		wMinAtk;				//最小攻击力		    
	short		wMaxAtk;				//最大攻击力
	short		wMinMAtk;				//最小法术攻击力
	short		wMaxMAtk;				//最大法术攻击力
	short		wHit;					//命中能力
	short		wDef;					//防御力
	short		wMDef;					//魔法防御力
	short		wDodge;					//闪避能力
	short		wBlock;					//格挡
	short		wParry;					//招架
	short		wImmunity;				//免伤
	short		wPierce;				//穿透

	DisType		FarAtkDis;				//最远攻击距离*
	DisType		NeaAtkDis;				//最近攻击距离*
	DisType		GuardRange;			//警戒范围*
	DisType		ChaseRange;			//追击范围*

	short		wWillDef;				//意志抗性
	short		wConstiDef;				//体质抗性
	short		wCri;					//物理爆击率
	short		wCriDamg;				//物理暴击伤害	
	short		wCriDef;				//物理爆击抗性
	short		wMCri;					//魔法暴击率
	short		wMCriDamg;				//魔法暴击伤害
	short		wMCriDef;				//魔法爆击抗性

	short		wDeBuffDef[DET_Max];	//Debuff抗性
	short		wElemDef[ET_Max];		//各个元素抗性
	short		wClassDam[C_Max];		//对各个种族伤害
	short		wElemDam[ET_Max];		//对各个元素伤害

    long		dwMaxHp;				//生命上限

}tagMFPro;
#endif