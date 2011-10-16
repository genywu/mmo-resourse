#include "stdafx.h"
#include <mmsystem.h>
#include "ExState1.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CExState1::CExState1( tagExState &stState )
{
	CState();
	//##
	m_lType				= STATE_TYPE_STANDARD;
	m_lID				= SKILL_EX_STATE_1;
	memcpy(&m_stStateParam, &stState, sizeof(tagExState));
}

CExState1::CExState1()
{
	CState();
	//##
	m_lType				= STATE_TYPE_STANDARD;
	m_lID				= SKILL_EX_STATE_1;
	memset(&m_stStateParam, 0, sizeof(tagExState));
}

CExState1::~CExState1()
{
}

DWORD CExState1::GetRemainedTime()
{
	if( !m_stStateParam.keeptime ) return 0;

	if( timeGetTime() >= m_stStateParam.keeptime + m_dwTimeStamp )
		return 0;
	return m_stStateParam.keeptime + m_dwTimeStamp - timeGetTime();
}

BOOL CExState1::OnUpdateProperties()
{
	CMoveShape* pUser = GetSufferer();
	if( pUser == NULL ) return FALSE;

	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}

	if( pUser -> GetType() == TYPE_PLAYER ) 
	{
		CPlayer* pPlayer = (CPlayer*)( pUser );
		if( pPlayer )
		{
			if( m_stStateParam.maxhp )
				pPlayer->SetMaxHP( pPlayer->GetMaxHP() + m_stStateParam.maxhp );
			if( m_stStateParam.maxmp )
				pPlayer->SetMaxMP( pPlayer->GetMaxMP() + m_stStateParam.maxmp );

			if( m_stStateParam.minatk )
				pPlayer->SetMinAtk( pPlayer->GetMinAtk() + m_stStateParam.minatk );
			if( m_stStateParam.maxatk )
				pPlayer->SetMaxAtk( pPlayer->GetMaxAtk() + m_stStateParam.maxatk );
			if( m_stStateParam.elm )
				pPlayer->SetElementModify( pPlayer->GetElementModify() + m_stStateParam.elm );

			if( m_stStateParam.def )
				pPlayer->SetDef( pPlayer->GetDef() + m_stStateParam.def);
			if( m_stStateParam.elmdef )
				pPlayer->SetElementResistant( pPlayer->GetElementResistant() + m_stStateParam.elmdef );

			if( m_stStateParam.cch )
				pPlayer->SetCCH( pPlayer->GetCCH() + m_stStateParam.cch);
			if( m_stStateParam.fullmiss )
				pPlayer->SetFullMiss( pPlayer->GetFullMiss() + m_stStateParam.fullmiss );

			if( m_stStateParam.atkavoid )
				pPlayer->SetAttackAvoid( pPlayer->GetAttackAvoid() + m_stStateParam.atkavoid);
			if( m_stStateParam.elmavoid )
				pPlayer->SetElementAvoid( pPlayer->GetElementAvoid() + m_stStateParam.elmavoid );

			if( m_stStateParam.hit )
				pPlayer->SetHit( pPlayer->GetHit() + m_stStateParam.hit);
			if( m_stStateParam.dodge )
				pPlayer->SetDodge( pPlayer->GetDodge() + m_stStateParam.dodge );
		}
	}

	return TRUE;
}

BOOL CExState1::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	if( pSufferer == NULL )
		return FALSE;
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CExState1VisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CExState1::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CExState1VisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

BOOL CExState1::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CExState1VisualEffect;
	m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
	return TRUE;
}

VOID CExState1::End()
{
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 1 );
	}
	CMoveShape* pShape = GetSufferer();
	if( pShape )
	{
		pShape->RemoveState( this );
	}
	else
	{
		//##获取不到承载者，有可能。
	}
}



VOID CExState1::AI()
{
	if( !m_stStateParam.keeptime ) return;

	if( timeGetTime() > m_dwTimeStamp + m_stStateParam.keeptime )
	{
		End();
	}
}

VOID CExState1::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于User身上.
	m_lUserRegion		= lRegionID;
}


//##类的序列化，用于在游戏服务器之间传输数据
VOID CExState1::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##剩余时间
	m_stStateParam.keeptime	= GetRemainedTime();

	_AddToByteArray( &vOut, &m_stStateParam, sizeof(tagExState) );

}

VOID CExState1::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	//##恢复时间
	m_dwTimeStamp	= timeGetTime();

	_GetBufferFromByteArray( pBytes, lPos, &m_stStateParam,sizeof(tagExState) );

	//##之后直接调用Begin就可以启动这个状态
}

VOID CExState1VisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
{
	if( pParent && !m_bEnded )
	{
		CMoveShape* pShape = pParent -> GetSufferer();
		CExState2 *pState = (CExState2*)pParent;

		if( pShape && pState )
		{
			switch( dwCmd )
			{
			case 0:
				{
					CMessage msg( CMessage::MSG_S2C_SKILL_ADD_EX_STATE );
					msg.Add( pShape -> GetType() );
					msg.Add( pShape -> GetID() );
					msg.Add( pState -> GetID() );
					msg.Add( pState->m_stStateParam.level );
					msg.Add( pState -> GetRemainedTime() );
					msg.Add( pState -> GetAdditionalData() );
					msg.SendToAround( pShape );
				}
				break;
			case 1:
				{
					CMessage msg( CMessage::MSG_S2C_SKILL_END_EX_STATE );
					msg.Add( pShape -> GetType() );
					msg.Add( pShape -> GetID() );
					msg.Add( pState -> GetID() );
					msg.Add( pState->m_stStateParam.level );
					msg.SendToAround( pShape );
				}
				break;
			}
		}
	}
	CVisualEffect::UpdateVisualEffect( pParent );
}