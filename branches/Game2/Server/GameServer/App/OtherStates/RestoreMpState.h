#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_RESTORE_MP_STATE_INCLUDED
#define _INC_RESTORE_MP_STATE_INCLUDED

#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CRestoreMpState : public CState
{
private:
	//##持续时间是毫秒为单位
	DWORD									m_dwTimeToKeep;
	DWORD									m_dwFrequency;
	//##加MP相关
	DWORD									m_dwMPGain;
	//##加MP次数
	DWORD									m_dwRestoreMpCounts;

public:
	CRestoreMpState							( DWORD dwPersistTime, DWORD dwFrequency, DWORD dwMPGain );
	CRestoreMpState							();
	virtual	~CRestoreMpState				();
	
	virtual BOOL	Begin					( CMoveShape* pUser, CMoveShape* pSufferer );
	virtual BOOL	Begin					( CMoveShape* pUser, LONG lX, LONG lY );
	virtual BOOL	Begin					( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion );

	virtual VOID	AI						();
	virtual VOID	End						();

	virtual VOID	OnChangeRegion			( LONG );
	virtual DWORD	GetRemainedTime			();

	//##序列化
	virtual VOID	Serialize				( vector<BYTE>& vOut );
	//##反序列化
	virtual VOID	Unserialize				( BYTE* pBytes, LONG& lPos );
};


class CRestoreMpStateVisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif