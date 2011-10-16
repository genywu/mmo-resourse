#pragma once
#include "MonsterAI.h"

class CJiuMai : public CMonsterAI
{
private:
	//##另一只双生兽的ID
	LONG											m_lTwinsID;
	
protected:
	/*
	virtual VOID				OnSchedule			();
	virtual VOID				OnIdle				();
	virtual BOOL				OnSearchEnemy		();

	virtual VOID				SetTarget			( LONG lType, LONG lID );
	virtual BOOL				OnLoseTarget		();
*/
public:
	CJiuMai											();
	~CJiuMai										();
/*
	virtual AI_EXEC_STATE		Run					();
	//virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
	*/
};
