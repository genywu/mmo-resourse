#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_PARTICULAR_STATE_INCLUDED
#define _INC_PARTICULAR_STATE_INCLUDED

#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CParticularState : public CState
{
	DWORD							m_dwAdditionalData;
	DWORD							m_dwCheckGoodsTimeStamp;

public:
	CParticularState				( DWORD dwValue );
	CParticularState				();
	virtual ~CParticularState		();
	
	virtual BOOL	Begin			( CMoveShape* pUser, CMoveShape* pSufferer );
	virtual BOOL	Begin			( CMoveShape* pUser, LONG lX, LONG lY );
	virtual BOOL	Begin			( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion );

	virtual VOID	AI				();
	virtual VOID	End				();

	virtual VOID	OnChangeRegion	( LONG );

	virtual DWORD	GetAdditionalData	();

	VOID	SetAdditionalData		( DWORD dwValue );

	//##当玩家在施法的时候进行了某种活动,比如走路,跑步,攻击,被攻击等
	//##状态缺省的操作是继续，但是这个状态在被攻击的时候会被中断
	//__inline virtual VOID OnAction( tagAction ta ) 
	//{
	//	if( ta == ACTION_DEFENSE )
	//	{
	//		End();
	//	}
	//}

	//##序列化
	virtual VOID Serialize( vector<BYTE>& vOut );

	//##反序列化
	virtual VOID Unserialize( BYTE* pBytes, LONG& lPos );
};


class CParticularStateVisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif