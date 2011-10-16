#ifndef _SHAPE_PROPERTIES_H
#define _SHAPE_PROPERTIES_H

//##Shape实际属性(对应原始属性的修正值)
struct tagProperties
{
	LONG	lMaxHp;					//##生命上限
	LONG	lMaxMp;					//##法力上限
	LONG	lMaxYp;					//##妖力上限
	LONG	lMaxRp;					//##怒值上限
	LONG	lStr;					//##力量
	LONG	lDex;					//##敏捷
	LONG	lCon;					//##体质
	LONG	lInt;					//##灵力
	LONG	lMinAtk;				//##最小攻击力
	LONG	lMaxAtk;				//##最大攻击力
	LONG	lHit;					//##命中能力
	LONG	lBurden;				//##负重上限
	LONG	lCCH;					//##致命一击几率
	LONG	lDef;					//##防御力
	LONG	lDodge;					//##闪避能力
	LONG	lAtcSpeed;				//##攻击速度
	LONG	lElementResistant;		//##元素伤害抗性
	LONG	lHpRecoverSpeed;		//##生命恢复速度
	LONG	lMpRecoverSpeed;		//##法力恢复速度

	LONG	lSoulResistant;			//##妖力伤害抗性
	LONG	lAddElementAtk;			//##附加法术攻击力（针对物理攻击）
	LONG	lAddSoulAtk;			//##附加妖力攻击力（针对物理攻击）
	LONG	lElementModify;			//##法术攻击修正(%)
};

#endif