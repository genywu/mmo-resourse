#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_RESTORE_HP_STATE_INCLUDED
#define _INC_RESTORE_HP_STATE_INCLUDED
#include "..\States\State.h"
#include "..\States\VisualEffect.h"

class CRestoreHpState : public CState
{
private:
	//##持续时间是毫秒为单位
	DWORD									m_dwTimeToKeep;
	DWORD									m_dwFrequency;
	//##加血相关
	DWORD									m_dwHPGain;
	//##加血次数
	DWORD									m_dwRestoreHpCounts;

public:
	CRestoreHpState							( DWORD dwPersistTime, DWORD dwFrequency, DWORD dwHPGain );
	CRestoreHpState							();
	virtual	~CRestoreHpState				();
	
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


class CRestoreHpStateVisualEffect : public CVisualEffect
{
public:
	virtual VOID		UpdateVisualEffect	( CState* pParent, DWORD dwCmd );
};

#endif