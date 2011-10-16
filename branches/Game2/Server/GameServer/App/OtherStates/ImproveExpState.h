#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _ImproveExpState_h_
#define _ImproveExpState_h_
#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CImproveExpState : public CState
{
private:
	// 提升百分比系数
	DWORD									m_dwCoefficient;
	//##持续时间是毫秒为单位
	DWORD									m_dwTimeToKeep;
public:
	CImproveExpState						( DWORD dwPersistTime, DWORD dwCoefficient );
	CImproveExpState						();
	virtual	~CImproveExpState				();
	
	virtual BOOL	Begin					( CMoveShape* pUser, CMoveShape* pSufferer );
	virtual BOOL	Begin					( CMoveShape* pUser, LONG lX, LONG lY );
	virtual BOOL	Begin					( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion );

	virtual BOOL	OnUpdateProperties		();
	virtual VOID	AI						();
	virtual VOID	End						();

	virtual VOID	OnChangeRegion			( LONG );
	virtual DWORD	GetRemainedTime			();

	//##序列化
	virtual VOID	Serialize				( vector<BYTE>& vOut );
	//##反序列化
	virtual VOID	Unserialize				( BYTE* pBytes, LONG& lPos );

	DWORD			GetCoefficient()		{return m_dwCoefficient;}
};


class CImproveExpStateVisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif