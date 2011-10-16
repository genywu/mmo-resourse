#include "stdafx.h"
#include <mmsystem.h>
#include "ParticularState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\Listener\CGoodsParticularPropertyListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CParticularState::CParticularState( DWORD dwValue )
{

	//##
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_PARTICULAR_STATE;
	m_dwAdditionalData		= dwValue;
	m_dwCheckGoodsTimeStamp	= 0;
}

CParticularState::CParticularState()
{
	//##
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_PARTICULAR_STATE;
	m_dwAdditionalData		= 0;
	m_dwCheckGoodsTimeStamp	= 0;
}

CParticularState::~CParticularState()
{
}

BOOL CParticularState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	if( pSufferer == NULL )
		return FALSE;
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CParticularStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}

	m_dwCheckGoodsTimeStamp	= 0;

	return TRUE;
}

BOOL CParticularState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CParticularStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}

	m_dwCheckGoodsTimeStamp	= 0;

	return TRUE;
}

BOOL CParticularState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CParticularStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}
	m_dwCheckGoodsTimeStamp	= 0;

	return TRUE;
}

VOID CParticularState::End()
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

VOID CParticularState::AI()
{
	//##每2秒检查一次.
	if( timeGetTime() >= m_dwCheckGoodsTimeStamp + 2000 )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetSufferer() );
		if( pPlayer )
		{
			CGoodsParticularPropertyListener lListener;
			lListener.m_gapID = CGoodsBaseProperties::GAP_EXCEPTION_STATE;
			pPlayer ->getOriginPack()->TraversingContainer( &lListener );

			//##先检查角色物品
			BOOL bExist = FALSE;
			size_t i = 0;
			for( ; i < lListener.m_vGoodsID.size(); i ++ )
			{
				CGoods* pGoods = dynamic_cast<CGoods*>( pPlayer -> getOriginPack()->Find(TYPE_GOODS, lListener.m_vGoodsID[i]) );
				if( pGoods )
				{
					if( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_EXCEPTION_STATE, 1) == m_dwAdditionalData )
					{
						bExist = TRUE;
						break;
					}
				}
			}

			//##检查角色装备
			if (bExist == FALSE)
			{
				pPlayer -> m_cEquipment.TraversingContainer( &lListener );
				for( i = 0; i < lListener.m_vGoodsID.size(); i ++ )
				{
					CGoods* pGoods = dynamic_cast<CGoods*>( pPlayer -> m_cEquipment.Find(TYPE_GOODS, lListener.m_vGoodsID[i]) );
					if( pGoods )
					{
						if( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_EXCEPTION_STATE, 1) == m_dwAdditionalData )
						{
							bExist = TRUE;
							break;
						}
					}
				}
			}

			if( bExist == FALSE )
			{
				End();
				return;
			}
		}
		else
		{
			End();
		}
	}
}

VOID CParticularState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于Sufferer身上，只改变Sufferer的RID
	m_lSuffererRegion = lRegionID;
}

//##类的序列化，用于在游戏服务器之间传输数据
VOID CParticularState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##马的类型,登记
	_AddToByteArray( &vOut, m_dwAdditionalData );
}

VOID CParticularState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	m_dwAdditionalData = _GetDwordFromByteArray( pBytes, lPos );

	//##之后直接调用Begin就可以启动这个状态
}


DWORD CParticularState::GetAdditionalData()
{
	return m_dwAdditionalData;
}

VOID CParticularState::SetAdditionalData( DWORD dwValue )
{
	m_dwAdditionalData = dwValue;
}

VOID CParticularStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
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