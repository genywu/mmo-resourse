#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _ExState2_h_
#define _ExState2_h_

#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CExState2 : public CState
{
	tagExState						m_stStateParam;

public:
	CExState2						( tagExState &stState );
	CExState2						();
	virtual ~CExState2				();

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


class CExState2VisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif