#pragma once
#include "monsterai.h"

/*
*AI基本类型:肉搏被动
*/
class CPassiveGladiator :
	public CMonsterAI
{
private:
	vector<CGUID>				m_vEnemy;

protected:
	virtual BOOL				WhenBeenHurted			(LONG lType,const CGUID& guid, DWORD ll);
	virtual BOOL				SearchEnemy				();
	virtual VOID				Clear					();
	virtual VOID				ResumeAll				( bool bForce = false );

public:
	VOID				SetTarget				( LONG lType, const CGUID& TargetGuid );
	CPassiveGladiator								();
	~CPassiveGladiator								();
};
