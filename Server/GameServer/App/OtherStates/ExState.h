#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _ExState_h_
#define _ExState_h_

#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CExState : public CState
{
public:
	tagExState						m_stStateParam;
	DWORD							_last_time_stamp;
	int								use_item(DWORD item_idx, DWORD item_num);

	CExState						( tagExState &stState );
	CExState						();
	virtual ~CExState				();

	virtual BOOL OnUpdateProperties ();

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


class CExStateVisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif