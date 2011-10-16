#pragma once

#ifndef _BossBlue_h_
#define _BossBlue_h_

#include "MonsterAI.h"
#include "..\Monster.h"

class CBossBlue :
	public CMonsterAI
{
private:
	bool						_isUsed[8];
protected:
	/*
//	virtual AI_EXEC_STATE		Run					();
	virtual VOID				OnSchedule			();
	virtual VOID				OnIdle				();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();


	virtual VOID				SelectAttackSkill	();
//	virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
public:
	CBossBlue										();
	~CBossBlue										();
/*

	//##AI休眠和唤醒的接口
	virtual VOID				Hibernate				();
	virtual VOID				WakeUp					();
*/
};

#endif //_BossBlue_h_