#ifndef _VILCOUGUARD_WTIH_BOW_H_
#define _VILCOUGUARD_WTIH_BOW_H_

#pragma once
#include "monsterai.h"

/*
*据点国家持弓守卫
*只攻击别国人，不能被本国人攻击
*/

class CVilCouGuardWithBow :
	public CMonsterAI
{
public:
	CVilCouGuardWithBow(void);
	virtual ~CVilCouGuardWithBow(void);

protected:
	/*
	virtual VOID				OnSchedule				();
	virtual VOID				OnIdle					();
	virtual BOOL				OnSearchEnemy			();
	virtual BOOL				OnMoving				();
	virtual BOOL				OnFighting				();

	virtual CMoveShape*			SearchEnemyMember	();
	virtual CMoveShape*			SearchEnemyPet		();
	virtual CMoveShape*			SearchEnemyCarriage();
public:
	virtual AI_EXEC_STATE		Run						();
//	virtual VOID				WhenBeenHurted			( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
*/
};

#endif
