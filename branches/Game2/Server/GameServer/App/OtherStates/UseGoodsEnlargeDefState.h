#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _UseGoodsEnlargeDefState_h_
#define _UseGoodsEnlargeDefState_h_
#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CUseGoodsEnlargeDefState : public CState
{
	//##持续时间是毫秒为单位
	DWORD							m_dwTimeToKeep;
	DWORD							m_dwCoefficient;

public:
	CUseGoodsEnlargeDefState				( DWORD dwTimeToKeep, DWORD dwCoefficient );
	CUseGoodsEnlargeDefState				();
	virtual ~CUseGoodsEnlargeDefState		();

	virtual BOOL OnUpdateProperties ();

	virtual BOOL	Begin			( CMoveShape* pUser, CMoveShape* pSufferer );
	virtual BOOL	Begin			( CMoveShape* pUser, LONG lX, LONG lY );
	virtual BOOL	Begin			( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion );

	virtual VOID	AI				();
	virtual VOID	End				();

	//##需要修改使用者的RegionID
	virtual VOID	OnChangeRegion	( LONG );
	//##这个状态没有时间限制
	virtual DWORD	GetRemainedTime ();

	//##序列化
	virtual VOID	Serialize		( vector<BYTE>& vOut );
	//##反序列化
	virtual VOID	Unserialize		( BYTE* pBytes, LONG& lPos );
};

class CUseGoodsEnlargeDefVisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif