#pragma once
#include "moveshape.h"
#include "setup\monsterlist.h"
#include "..\PUBLIC\MonsterDef.h"
class CMonster :
	public CMoveShape
{
public:
	CMonster(void);
	virtual ~CMonster(void);

private:	
	tagMPro  m_Property ;       //  Ù–‘
	tagMFPro m_FightProperty ;   // ’Ω∂∑ Ù–‘	
};
