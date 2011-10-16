#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _AutoProtectState_h_
#define _AutoProtectState_h_

#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CAutoProtectState : public CState
{
	//##持续时间是毫秒为单位
	LONG							m_lTimeToKeep;

public:
	CAutoProtectState					( LONG lTime );
	CAutoProtectState					();
	virtual ~CAutoProtectState		();

	virtual BOOL OnUpdateProperties ();
	VOID			Restart			();

	virtual BOOL	Begin			( CMoveShape* pUser, CMoveShape* pSufferer );
	virtual BOOL	Begin			( CMoveShape* pUser, LONG lX, LONG lY );
	virtual BOOL	Begin			( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion );

	virtual VOID	AI				();
	virtual VOID	End				();

	virtual VOID	OnChangeRegion	( LONG );
	virtual DWORD	GetRemainedTime ();

	//##序列化
	virtual VOID	Serialize		( vector<BYTE>& vOut );
	//##反序列化
	virtual VOID	Unserialize		( BYTE* pBytes, LONG& lPos );
};


class CAutoProtectStateVisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif