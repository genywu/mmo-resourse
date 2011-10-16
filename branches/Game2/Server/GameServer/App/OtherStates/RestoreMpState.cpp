#include "stdafx.h"
#include <mmsystem.h>
#include "RestoreMpState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRestoreMpState::CRestoreMpState( DWORD dwPersistTime, DWORD dwFrequency, DWORD dwMPGain )
{
	//##
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_RESTORE_MP;
	m_dwTimeToKeep			= dwPersistTime;
	m_dwFrequency			= dwFrequency;
	m_dwMPGain				= dwMPGain;
	m_dwRestoreMpCounts		= 0;
}

CRestoreMpState::CRestoreMpState()
{
	//##
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_RESTORE_MP;
	m_dwTimeToKeep			= 0;
	m_dwFrequency			= 0;
	m_dwMPGain				= 0;
	m_dwRestoreMpCounts		= 0;
}


CRestoreMpState::~CRestoreMpState()
{
}

DWORD CRestoreMpState::GetRemainedTime()
{
	if( timeGetTime() >= m_dwTimeToKeep + m_dwTimeStamp )
		return 0;
	return m_dwTimeToKeep + m_dwTimeStamp - timeGetTime();
}

BOOL CRestoreMpState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CRestoreMpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	m_dwRestoreMpCounts = 0;
	return TRUE;
}

BOOL CRestoreMpState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	m_pCurrentVisualEffect = new CRestoreMpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	m_dwRestoreMpCounts = 0;
	return TRUE;
}

BOOL CRestoreMpState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	m_pCurrentVisualEffect = new CRestoreMpStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	m_dwRestoreMpCounts = 0;
	return TRUE;
}

VOID CRestoreMpState::End()
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

VOID CRestoreMpState::AI()
{
	//##获取承受者
	CPlayer* pSufferer = dynamic_cast<CPlayer*>( GetSufferer() );
	if( !pSufferer )
	{
		End();
		return;
	}

	if( pSufferer -> IsDied() ) return;

	//##加MP
	if( timeGetTime() > m_dwTimeStamp + m_dwRestoreMpCounts * m_dwFrequency )
	{
		//##修改计数器
		++ m_dwRestoreMpCounts;

		LONG lMp = pSufferer -> GetMP() + m_dwMPGain;
		if( lMp > pSufferer -> GetMaxMP() )
		{
			lMp = pSufferer -> GetMaxMP();
		}

		pSufferer -> SetMP( static_cast<WORD>(lMp) );
		pSufferer -> OnChangeStates();
	}

	//##判断时间
	if( timeGetTime() > m_dwTimeStamp + m_dwTimeToKeep )
	{
		End();
	}
}

VOID CRestoreMpState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于Sufferer身上，只改变Sufferer的RID
	m_lSuffererRegion = lRegionID;
}

//##类的序列化，用于在游戏服务器之间传输数据
VOID CRestoreMpState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##剩余时间
	_AddToByteArray( &vOut, GetRemainedTime() );

	//##频率
	_AddToByteArray( &vOut, m_dwFrequency );

	//##加MP量
	_AddToByteArray( &vOut, m_dwMPGain );
	
}

VOID CRestoreMpState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	//##恢复时间
	m_dwTimeStamp	= timeGetTime();
	m_dwTimeToKeep	= _GetDwordFromByteArray( pBytes, lPos );

	//##频率
	m_dwFrequency	= _GetDwordFromByteArray( pBytes, lPos );

	//##MP
	m_dwMPGain		= _GetDwordFromByteArray( pBytes, lPos );

	//##之后直接调用Begin就可以启动这个状态
}

VOID CRestoreMpStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
{
	CVisualEffect::UpdateVisualEffect( pParent );
}