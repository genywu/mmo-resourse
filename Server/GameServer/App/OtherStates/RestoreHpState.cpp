#include "stdafx.h"
#include <mmsystem.h>
#include "RestoreHpState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRestoreHpState::CRestoreHpState( DWORD dwPersistTime, DWORD dwFrequency, DWORD dwHPGain )
{
	//##
	m_lType				= STATE_TYPE_STANDARD;
	m_lID				= STATE_RESTORE_HP;
	m_dwTimeToKeep		= dwPersistTime;
	m_dwFrequency		= dwFrequency;
	m_dwHPGain			= dwHPGain;
	m_dwRestoreHpCounts	= 0;
}

CRestoreHpState::CRestoreHpState()
{
	//##
	m_lType				= STATE_TYPE_STANDARD;
	m_lID				= STATE_RESTORE_HP;
	m_dwTimeToKeep		= 0;
	m_dwFrequency		= 0;
	m_dwHPGain			= 0;
	m_dwRestoreHpCounts	= 0;
}


CRestoreHpState::~CRestoreHpState()
{
}

DWORD CRestoreHpState::GetRemainedTime()
{
	if( timeGetTime() >= m_dwTimeToKeep + m_dwTimeStamp )
		return 0;
	return m_dwTimeToKeep + m_dwTimeStamp - timeGetTime();
}

BOOL CRestoreHpState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CRestoreHpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	m_dwRestoreHpCounts = 0;
	return TRUE;
}

BOOL CRestoreHpState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	m_pCurrentVisualEffect = new CRestoreHpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	m_dwRestoreHpCounts = 0;
	return TRUE;
}

BOOL CRestoreHpState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	m_pCurrentVisualEffect = new CRestoreHpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	m_dwRestoreHpCounts = 0;
	return TRUE;
}

VOID CRestoreHpState::End()
{
	CMoveShape* pShape = GetSufferer();
	if( pShape )
	{
		pShape -> RemoveState( this );
	}
	else
	{
		//##获取不到承载者，有可能。
	}
}

VOID CRestoreHpState::AI()
{
	//##获取承受者
	CMoveShape* pSufferer = GetSufferer();
	if( !pSufferer )
	{
		End();
		return;
	}

	if( pSufferer -> IsDied() ) return;

	//##加血
	if( timeGetTime() > m_dwTimeStamp + m_dwRestoreHpCounts * m_dwFrequency )
	{
		//##修改加血计数器
		++ m_dwRestoreHpCounts;

		LONG lHp = pSufferer -> GetHP() + m_dwHPGain;
		if( lHp > pSufferer -> GetMaxHP() )
		{
			lHp = pSufferer -> GetMaxHP();
		}

		pSufferer -> SetHP( static_cast<DWORD>(lHp) );
		pSufferer -> OnChangeStates();
	}

	//##判断时间
	if( timeGetTime() > m_dwTimeStamp + m_dwTimeToKeep )
	{
		End();
	}
}

VOID CRestoreHpState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于Sufferer身上，只改变Sufferer的RID
	m_lSuffererRegion = lRegionID;
}

//##类的序列化，用于在游戏服务器之间传输数据
VOID CRestoreHpState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##剩余时间
	_AddToByteArray( &vOut, GetRemainedTime() );

	//##频率
	_AddToByteArray( &vOut, m_dwFrequency );

	//##加血量
	_AddToByteArray( &vOut, m_dwHPGain );
	
}

VOID CRestoreHpState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	//##恢复时间
	m_dwTimeStamp	= timeGetTime();
	m_dwTimeToKeep	= _GetDwordFromByteArray( pBytes, lPos );

	//##频率
	m_dwFrequency	= _GetDwordFromByteArray( pBytes, lPos );

	//##伤血
	m_dwHPGain		= _GetDwordFromByteArray( pBytes, lPos );

	//##之后直接调用Begin就可以启动这个状态
}

VOID CRestoreHpStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
{
	CVisualEffect::UpdateVisualEffect( pParent );
}