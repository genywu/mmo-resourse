#include "stdafx.h"
#include <mmsystem.h>
#include "TeamState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\Session\CSessionFactory.h"
#include "..\Session\CTeam.h"

#include "..\OrganizingSystem\GameOrganizingCtrl.h"
#include "..\OrganizingSystem\GSTeam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTeamState::CTeamState( CHAR* pName, CHAR* pPassword )
{
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_TEAM;
	m_dwLastCheckTimeStamp	= 0;

	if( pName )
	{
		m_strTeamName = pName;
	}

	if( pPassword )
	{
		m_strTeamPassword = pPassword;
	}
}

CTeamState::CTeamState()
{
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_TEAM;
}

CTeamState::~CTeamState()
{
}

BOOL CTeamState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	if( pSufferer == NULL )
		return FALSE;
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CTeamStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}
	m_dwLastCheckTimeStamp = 0;
	return TRUE;
}

BOOL CTeamState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CTeamStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}
	m_dwLastCheckTimeStamp = 0;
	return TRUE;
}

BOOL CTeamState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CTeamStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}
	m_dwLastCheckTimeStamp = 0;
	return TRUE;
}

VOID CTeamState::End()
{
	if( m_pCurrentVisualEffect )
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 1 );

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

VOID CTeamState::AI()
{
	if( timeGetTime() >= m_dwLastCheckTimeStamp + 5000 )
	{
		m_dwLastCheckTimeStamp = timeGetTime();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetSufferer() );
		if( pPlayer )
		{
			if( pPlayer -> GetTeamID() )
			{
				//LONG lSessionID = GetGame() -> GetTeamSessionID( pPlayer -> GetTeamID() );
				//CTeam* pTeam = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession( lSessionID ) ) );
				GSTeam *pGSTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(pPlayer -> GetTeamID());
				if( pGSTeam && pGSTeam -> GetLeader() != pPlayer -> GetID() )
				{
					End();
				}
			}
		}
		else
		{
			End();
		}
	}
}

VOID CTeamState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于Sufferer身上，只改变Sufferer的RID
	m_lSuffererRegion = lRegionID;
}

//##类的序列化，用于在游戏服务器之间传输数据
VOID CTeamState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );
	_AddToByteArray( &vOut, const_cast<CHAR*>( m_strTeamName.c_str() ) );
	_AddToByteArray( &vOut, const_cast<CHAR*>( m_strTeamPassword.c_str() ) );
}

VOID CTeamState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了
	CHAR szTemp[256];
	m_strTeamName		= _GetStringFromByteArray( pBytes, lPos, szTemp );
	m_strTeamPassword	= _GetStringFromByteArray( pBytes, lPos, szTemp );

	//##之后直接调用Begin就可以启动这个状态
}

CHAR* CTeamState::GetTeamName()
{
	return const_cast<CHAR*>( m_strTeamName.c_str() );
}

CHAR* CTeamState::GetTeamPassword()
{
	return const_cast<CHAR*>( m_strTeamPassword.c_str() );
}

DWORD CTeamState::GetAdditionalData()
{
	DWORD dwResult				= 0;
	DWORD dwNumTeamate			= 1;
	DWORD dwPasswordProtected	= 0;
	
	//##检查人数
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetSufferer() );
	if( pPlayer )
	{
		//##已经有队伍
		if( pPlayer -> GetTeamID() )
		{
			//LONG lSessionID = GetGame() -> GetTeamSessionID( pPlayer -> GetTeamID() );
			//CTeam* pSession = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession(lSessionID) ) );
			GSTeam *pGSTeam = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(pPlayer -> GetTeamID());
			if( pGSTeam )
			{
				dwNumTeamate = pGSTeam -> GetTeamatesAmount();
			}
		}
	}

	//##检查密码保护
	if( m_strTeamPassword.size() )
	{
		dwPasswordProtected = 1;
	}

	dwResult = dwPasswordProtected << 16 | dwNumTeamate;

	return dwResult;
}

VOID CTeamStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
{
	if( pParent && !m_bEnded )
	{
		//##只能获取Sufferer
		CMoveShape* pShape = pParent -> GetSufferer();
		if( pShape )
		{
			switch( dwCmd )
			{
			case 0:
				{
					CTeamState* pTeamState = dynamic_cast<CTeamState*>( pParent );
					if( pTeamState )
					{
						CMessage msg( MSG_S2C_SKILL_ADD_EX_STATE );
						msg.Add( pShape -> GetType() );
						msg.Add( pShape -> GetID() );
						msg.Add( pParent -> GetID() );
						msg.Add( pParent -> GetRemainedTime() );
						msg.Add( pParent -> GetAdditionalData() );
						msg.Add( pTeamState -> GetTeamName() );
						msg.SendToAround( pShape );
					}
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