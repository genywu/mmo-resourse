//定义和怪物相关的枚举和结构

#ifndef MONSTER_DEF_H
#define MONSTER_DEF_H

#include "BaseDef.h"
#include "StateDef.h"


typedef struct tagMonsterProperty
{
	WORD		wTitleColor;			//称谓颜色
	WORD		wTitlePicID;			//称谓图形id
	DWORD		dwIndex;				//种类索引编号
	DWORD		dwPicID;				//图形ID
	DWORD		dwPicLevel;				//图形等级
	BYTE		byBlockType;			//阻挡类型
	DWORD		dwNameColor;			//名字颜色编号,对应到客户端配置
	DWORD		dwHpBarColor;			//血条颜色，0：表示不显示血条 1：红色 2：黄色
	DWORD		dwSoundID;				//声音ID
	WORD		wLevel;				    //等级
	DWORD		dwExp;					//经验值*
	DWORD		dwOccuPoint;			//职业点数
	BYTE		byCountry;				//国家ID
	BYTE		byTamable;				//是否可以驯服
	DWORD		dwMaxTameCount;			//最大尝试驯服次数
	WORD		wCountryContriValue;	//国家贡献值
	WORD		wSPContriValue;			//SP贡献值
	eEleType	eEType;					//元素类别
	eNpcType	eNType;					//Npc类别 (NNPC,Monster,Guard)
	WORD		wBossLevel;				//boss等级
	eClass		eClass_;				//种类
	WORD		wRace;					//族群(AI使用)
	WORD		wMoveRandomValue;		//产生移动的概率(活力)
	WORD		wStopTime;				//停留的时间(在一个格子中间停留帧数)
	WORD		wAIHangupType;			//AI挂起方式
	WORD		wFigure;				//体形(0:1x1 1:3*3 2:5*5)*
	DWORD		dwHp;					//当前生命值
	DWORD       dwAIType;               //AI类型

	float		fMoveSpeed;				//移动速度
	float		fFightMoveSpeed;		//战斗移动速度
	float       fTurningSpeed;			//转身速度
	WORD		wMinStopTimes;			//最小停留次数
	WORD		wMaxStopTimes;			//最大停留次数
	WORD		wMinRunTimes;			//最少行走步数
	WORD		wMaxRunTimes;			//最多行走步数
	WORD		wCanTalk;				//是否能对话
	LONG		lRefeashIndex;			//更新索引
	//
	float       fHurtProportion;        //重伤比率
	DWORD       dwGuardTime;            //警戒时间
	float       fEscapePoint;           //逃跑底线（逃跑点）
    WORD        wSkillProportion;       //技能概率,一个小于等于100，大于等于0的整型。
	DWORD       dwSkillSpace;           //技能间隔
	BYTE		byIsFlyToPeace;			//转换为和平状态时候，是否飞回去
	BYTE		byIsResumeAll;			//转换为和平状态的时候是否完全恢复
	WORD		wReturnRange;			//归位距离
	WORD		wPeaceMoveRange;		//和平移动范围
	DWORD		dwChaseTime;			//追击时间，在追击范围外的没受到攻击转化为和平的时间
	DWORD       dwAIReferrence[MONSTER_AI_REFERRENCE_NUM];      //AI参考
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
	short		wDodge;					//闪避能力
	short		wBlock;					//格挡
	short		wParry;					//招架	
	short		wPierce;				//穿透
	long		lImmunity;				//免伤
	long		lDef;					//防御力
	long		lMDef;					//魔法防御力
	long		dwMaxHp;				//生命上限

	short		wFarAtkDis;				//最远攻击距离*
	short		wNeaAtkDis;				//最近攻击距离*
	short		wGuardRange;			//警戒范围*
	short		wChaseRange;			//追击范围*

	long		lWillDef;				//意志抗性
	long		lConstiDef;				//体质抗性	
	long		lCriDef;				//物理爆击抗性	
	long		lMCriDef;				//魔法爆击抗性
	short		wCri;					//物理爆击率
	short		wCriDamg;				//物理暴击伤害	
	short		wMCri;					//魔法暴击率
	short		wMCriDamg;				//魔法暴击伤害

	long		lDeBuffDef[DET_Max];	//Debuff抗性
	long		lElemDef[ET_Max];		//各个元素抗性
	short		wClassDam[C_Max];		//对各个种族伤害
	short		wElemDam[ET_Max];		//对各个元素伤害
}tagMFPro;
#endif