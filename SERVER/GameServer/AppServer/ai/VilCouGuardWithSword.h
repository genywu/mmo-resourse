#ifndef _VILCOUGUARD_WTIH_SWORLD_H_
#define _VILCOUGUARD_WTIH_SWORLD_H_

#pragma once

#include "monsterai.h"

/*
*据点国家持剑守卫
*/

class CVilCouGuardWithSword :
	public CMonsterAI
{
public:
	CVilCouGuardWithSword(void);
	~CVilCouGuardWithSword(void);

private:
	LONG						m_lX;
	LONG						m_lY;

protected:
	/*
	virtual VOID				OnSchedule				();
	virtual BOOL				OnSearchEnemy			();
	virtual VOID				OnIdle					();
	virtual BOOL				OnLoseTarget			();
	virtual BOOL				OnMoving				();

	virtual CMoveShape*			SearchEnemyMember	();
	virtual CMoveShape*			SearchEnemyPet		();
	virtual CMoveShape*			SearchEnemyCarriage();

public:
	virtual AI_EXEC_STATE		Run						();
	//virtual VOID				WhenBeenHurted			( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
	*/
};

#endif
