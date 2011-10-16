#pragma once

#ifndef _Carriage_h_
#define _Carriage_h_
#include "MonsterAI.h"

class CMoveShape;
class CPlayer;
/*
class CCarriage :
	public CMonsterAI
{
public:
	
	enum CAR_ACTION
	{
		FALLOWING,		//跟随
		STAYING,		//停留
	};

	CCarriage											();
	~CCarriage											();

	virtual AI_EXEC_STATE		Run					();
	virtual VOID				OnIdle				();

	//##设置/获取宠物的当前AI模式
	VOID						SetCurrentAction	( CAR_ACTION ca );
	CAR_ACTION					GetCurrentAction	();

	//##获取镖车的主人
	CMoveShape*					GetMaster			();

protected:
	virtual VOID				OnSchedule			();
	virtual BOOL				OnMoving			();


	//##动作：跟随，停留
	VOID						OnFallowingSchedule	();
	VOID						OnStayingSchedule	();

private:
	CAR_ACTION					m_caAction;

	//##用于待命模式的时候，记录主人是否有效的时间戳
	DWORD						m_dwInvalidMasterTimeStamp;
	DWORD						m_dwSeekMasterTimeStamp;
	BOOL						m_bMasterLogout;

	//##24小时的消失计时
	//DWORD						m_dwPetLifeCycleTimeStamp;
	//DWORD						m_dwPetLifeCycleCounter;
	friend class CPlayer;
};
*/
const LONG CARRIAGE_STATE_DISTANCE=3;
const LONG CARRIAGE_STAY_TIME=500;

class CCarriage:
	public CMonsterAI
{
public:
	enum CAR_ACTION
	{
		NOMONSTER=0,	//无主人停留状态
		FALLOWING,		//跟随
		STAYING,		//停留
	};

public:
	CCarriage											();
	~CCarriage											();
	BOOL WhenBeenHurted();
	CMoveShape*	GetMaster();
	VOID						SetCurrentAction	( CAR_ACTION ca ){m_caFollowState=ca;}
	CAR_ACTION					GetCurrentAction	(){return m_caFollowState;}
protected:
	VOID ChangeStateTo(AI_EXEC_STATE aes);
	BOOL SearchEnemy();
	BOOL RandomRun();
	//INT Tracing(CMoveShape* pTarget);
protected:
	//##用于待命模式的时候，记录主人是否有效的时间戳
	DWORD						m_dwInvalidMasterTimeStamp;
	DWORD						m_dwSeekMasterTimeStamp;
	BOOL						m_bMasterLogout;

	//##24小时的消失计时
	DWORD						m_dwPetLifeCycleTimeStamp;
	DWORD						m_dwPetLifeCycleCounter;

	CAR_ACTION					m_caFollowState;


	friend class CPlayer;
};

#endif