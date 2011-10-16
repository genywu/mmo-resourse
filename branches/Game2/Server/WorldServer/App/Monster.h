#pragma once
#include "MoveShape.h"
#include "../../../Public/Common/MonsterDef.h"

class CMonster :
	public CMoveShape
{
public:
	CMonster(void);
	virtual ~CMonster(void);

public:

	// 属性
	/* del by chenaly..........07.4.6.......
	struct stProperty
	{
		string			strOrginName;		//	原始名称
		DWORD			dwHP;				//	当前生命值

		WORD			wSign;				//  所在地图的唯一标识
		WORD			wLeaderSign;		//  领导的唯一标识
		long			lLeaderType;		//	领导者type
		long			lLeaderID;			//	领导者id
		WORD			wLeaderDistance;	//  和领导者保持的最大距离
		long			lLiveTime;			//	生存时间（-1等于一直生存，倒计时，单位：秒）
		bool			bDiedRemove;		//	死亡后删除，不重生
	};
	//*///end del ....................................

private:
	//chaneged by chenaly......07.4.6........
	//stProperty		m_Property;	       // 属性			
	tagMPro  m_Property ;       // 属性
	tagMFPro m_FightProperty ;   // 战斗属性

	//end change..................................

public:
	//changed by chenaly............07.4.6...........
	//const CMonsterList::tagMonster*		GetBaseProperty()	{return CMonsterList::GetPropertyByOrginName(m_Property.strOrginName.c_str());}
	//stProperty*	GetProperty()	{return &m_Property;}

	//virtual BYTE GetFigure()	{return GetBaseProperty()->dwFigure;}	// 体形
	//end change......................................
	
};
