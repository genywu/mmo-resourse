#pragma once
#include "MonsterAI.h"

class CMoveShape;
class CPlayer;
class CMonsterTaming;


class CPet :
	public CMonsterAI
{
public:

	enum PET_SEARCH_ENEMY_MODE
	{
		PSEM_ACTIVE_ATTACK,				//##主动进攻
		PSEM_PASSIVELY_ATTACK,			//##被动进攻
		PSEM_DEFENSE,					//##防御
	};

	enum PET_ACTION
	{
		PA_ATTACKING,
		PA_FALLOWING,
		PA_STAYING,
		PA_DISMISSING,
	};

	CPet											();
	~CPet											();

//	virtual AI_EXEC_STATE		Run					();
//	virtual VOID				WhenBeenHurted		( LONG lType, LONG lID, DWORD dwStiffenTime = 0 );
	virtual VOID				OnIdle				();

	//##设置/获取宠物的当前AI模式
	VOID						SetPetCurrentAIMode	( PET_SEARCH_ENEMY_MODE psem );
	PET_SEARCH_ENEMY_MODE		GetPetCurrentAIMode	();

	VOID						SetPetCurrentAction	( PET_ACTION pa );
	PET_ACTION					GetPetCurrentAction	();

	//##获取宠物的主人
	CMoveShape*					GetPetMaster		();

protected:
	virtual VOID				OnSchedule			();
	virtual BOOL				OnSearchEnemy		();
	virtual BOOL				OnMoving			();

	virtual BOOL				OnLoseTarget		();
	virtual VOID				SetTarget			( LONG lType, const CGUID& TargetGuid );


	//##动作：攻击，跟随，停留
	VOID						OnAttackingSchedule	();
	VOID						OnFallowingSchedule	();
	VOID						OnStayingSchedule	();

private:
	PET_SEARCH_ENEMY_MODE		m_psemPetAIMode;
	PET_ACTION					m_paPetAction;

	//##用于待命模式的时候，记录主人是否有效的时间戳
	DWORD						m_dwInvalidMasterTimeStamp;
	DWORD						m_dwSeekMasterTimeStamp;
	BOOL						m_bMasterLogout;

	//##24小时的消失计时
	DWORD						m_dwPetLifeCycleTimeStamp;
	DWORD						m_dwPetLifeCycleCounter;

	friend class CPlayer;
	friend class CMonsterTaming;
};
