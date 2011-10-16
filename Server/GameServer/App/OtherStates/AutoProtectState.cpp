#include "stdafx.h"
#include <mmsystem.h>
#include "AutoProtectState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAutoProtectState::CAutoProtectState( LONG lTime )
{
	//##
	m_lType				= STATE_TYPE_STANDARD;
	m_lTimeToKeep		= lTime;
	m_lID				= STATE_AUTO_PROTECT;
}

CAutoProtectState::CAutoProtectState()
{
	//##
	m_lType				= STATE_TYPE_STANDARD;
	m_lTimeToKeep		= 0;
	m_lID				= STATE_AUTO_PROTECT;
}

CAutoProtectState::~CAutoProtectState()
{
}

DWORD CAutoProtectState::GetRemainedTime()
{
	if( timeGetTime() >= m_lTimeToKeep + m_dwTimeStamp )
		return 0;
	return m_lTimeToKeep + m_dwTimeStamp - timeGetTime();
}

BOOL CAutoProtectState::OnUpdateProperties()
{
	CMoveShape* pUser = GetSufferer();
	if( pUser && pUser->GetType() == TYPE_PLAYER )
	{
		CPlayer *pPlayer = (CPlayer*)pUser;

		if( pPlayer && !pPlayer->IsGM() )
		{
			pUser ->God(true);
			if( m_pCurrentVisualEffect )
			{
				m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
			}
		}
	}
	return TRUE;
}

BOOL CAutoProtectState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	if( pSufferer == NULL )
		return FALSE;

	// GM不会增加改技能
	if( pSufferer && pSufferer->GetType() == TYPE_PLAYER )
	{
		CPlayer *pPlayer = (CPlayer*)pSufferer;

		if( pPlayer && pPlayer->IsGM() )
		{
			return FALSE;
		}
	}

	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CAutoProtectStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CAutoProtectState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	// GM不会增加改技能
	CMoveShape* pSufferer = GetSufferer();
	if( pSufferer && pSufferer->GetType() == TYPE_PLAYER )
	{
		CPlayer *pPlayer = (CPlayer*)pSufferer;

		if( pPlayer && pPlayer->IsGM() )
		{
			return FALSE;
		}
	}


	CState::Begin( pUser, lX, lY );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CAutoProtectStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CAutoProtectState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	// GM不会增加改技能
	CMoveShape* pSufferer = GetSufferer();
	if( pSufferer && pSufferer->GetType() == TYPE_PLAYER )
	{
		CPlayer *pPlayer = (CPlayer*)pSufferer;

		if( pPlayer && pPlayer->IsGM() )
		{
			return FALSE;
		}
	}


	CState::Begin( pUser, otType, lID, lRegion );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CAutoProtectStateVisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

VOID CAutoProtectState::End()
{

	CMoveShape* pSufferer = GetSufferer();
	if( pSufferer && pSufferer->GetType() == TYPE_PLAYER)
	{

		CPlayer *pPlayer = (CPlayer*)pSufferer;

		if( pPlayer && !pPlayer->IsGM() )
		{

			if( m_pCurrentVisualEffect )
			{
				m_pCurrentVisualEffect -> UpdateVisualEffect( this, 1 );
			}

			pSufferer->God(false);
			pSufferer->RemoveState( this );


		}
	}
}

VOID CAutoProtectState::Restart()
{
	m_dwTimeStamp = timeGetTime();
}

VOID CAutoProtectState::AI()
{
	if( timeGetTime() > m_dwTimeStamp + m_lTimeToKeep )
	{
		End();
	}
}

VOID CAutoProtectState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于User身上.
	m_lUserRegion		= lRegionID;
}


//##类的序列化，用于在游戏服务器之间传输数据
VOID CAutoProtectState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##剩余时间
	_AddToByteArray( &vOut, GetRemainedTime() );

}

VOID CAutoProtectState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	//##恢复时间
	m_dwTimeStamp	= timeGetTime();
	m_lTimeToKeep	= _GetDwordFromByteArray( pBytes, lPos );

	//##之后直接调用Begin就可以启动这个状态
}

VOID CAutoProtectStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
{
	if( pParent && !m_bEnded )
	{
		CMoveShape* pShape = pParent -> GetSufferer();
		if( pShape )
		{
			switch( dwCmd )
			{
			case 0:
				{
					CMessage msg( MSG_S2C_SKILL_ADD_EX_STATE );
					msg.Add( pShape -> GetType() );
					msg.Add( pShape -> GetID() );
					msg.Add( pParent -> GetID() );
					msg.Add( pParent -> GetRemainedTime() );
					msg.Add( pParent -> GetAdditionalData() );
					msg.SendToAround( pShape );
				}
				break;
			case 1:
				{
					CMessage msg( MSG_S2C_SKILL_END_EX_STATE );
					msg.Add( pShape -> GetType() );
					msg.Add( pShape -> GetID() );
					msg.Add( pParent -> GetID() );
					msg.SendToAround( pShape );
				}
				break;
			}
		}
	}
	CVisualEffect::UpdateVisualEffect( pParent );
}