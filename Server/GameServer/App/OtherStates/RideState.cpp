#include "stdafx.h"
#include <mmsystem.h>
#include "RideState.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\Listener\CGoodsParticularPropertyListener.h"
#include "../Listener/CSeekGoodsListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRideState::CRideState( DWORD dwType, DWORD dwLevel, DWORD dwRoleLimit )
{

	//##
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_RIDE_HORSE;
	m_dwHorseType			= dwType;
	m_dwHorseLevel			= dwLevel;
	m_dwCheckGoodsTimeStamp	= 0;
	m_dwRoleLimit			= dwRoleLimit;
	m_strGoodName			= "";
}

CRideState::CRideState()
{
	//##
	m_lType					= STATE_TYPE_STANDARD;
	m_lID					= STATE_RIDE_HORSE;
	m_dwHorseType			= 0;
	m_dwHorseLevel			= 0;
	m_dwCheckGoodsTimeStamp	= 0;
	m_dwRoleLimit			= 0;
	m_strGoodName			= "";
}

CRideState::~CRideState()
{
}

BOOL CRideState::Begin( CMoveShape* pUser, CMoveShape* pSufferer )
{
	if( pSufferer == NULL )
		return FALSE;
	CState::Begin( pUser, pSufferer );
	m_pCurrentVisualEffect = new CRideStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}

	pSufferer -> SetFightable( FALSE );
	m_dwCheckGoodsTimeStamp	= 0;

	return TRUE;
}

BOOL CRideState::Begin( CMoveShape* pUser, LONG lX, LONG lY )
{
	CState::Begin( pUser, lX, lY );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CRideStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}

	GetSufferer() -> SetFightable( FALSE );
	m_dwCheckGoodsTimeStamp	= 0;

	return TRUE;
}

BOOL CRideState::Begin( CMoveShape* pUser, OBJECT_TYPE otType, LONG lID, LONG lRegion )
{
	CState::Begin( pUser, otType, lID, lRegion );
	if( GetSufferer() == NULL )
	{
		End();
		return FALSE;
	}
	m_pCurrentVisualEffect = new CRideStateVisualEffect;
	if( m_pCurrentVisualEffect )
	{
		m_pCurrentVisualEffect -> BeginVisualEffect( TRUE );
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 0 );
	}
	GetSufferer() -> SetFightable( FALSE );
	m_dwCheckGoodsTimeStamp	= 0;

	return TRUE;
}

VOID CRideState::End()
{
	if( m_pCurrentVisualEffect )
		m_pCurrentVisualEffect -> UpdateVisualEffect( this, 1 );

	CMoveShape* pShape = GetSufferer();
	if( pShape )
	{
		pShape -> SetFightable( TRUE );
		pShape -> RemoveState( this );
	}
	else
	{
		//##获取不到承载者，有可能。
	}
}

BOOL	CRideState::OnUpdateProperties()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetSufferer() );
	if( pPlayer )
	{
		CSeekGoodsListener lSeeker;
		lSeeker.SetTarget( (char*)m_strGoodName.c_str() );
		pPlayer ->getOriginPack()->TraversingContainer( &lSeeker );
		CGoods* pGoods = NULL;
		for( size_t i = 0; i < lSeeker.m_vGoodsID.size(); i ++ )
		{
			pGoods = dynamic_cast<CGoods*>(pPlayer->getOriginPack()->Find(TYPE_GOODS,lSeeker.m_vGoodsID[i] ));
			if(pGoods != NULL)
				break;
		}
		//CGoods* pGoods = dynamic_cast<CGoods*>(pPlayer->m_cPacket.Find(TYPE_GOODS,m_lGoodID));
		if(pGoods)
		{
			pPlayer->MountEquip(pGoods);
			pPlayer->MountEquip(pGoods,false);
		}
	}
	return TRUE;
}

VOID CRideState::AI()
{
	//##每10秒检查一次.
	if( timeGetTime() >= m_dwCheckGoodsTimeStamp + 10000 )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetSufferer() );
		if( pPlayer )
		{
			CGoodsParticularPropertyListener lListener;
			lListener.m_gapID = CGoodsBaseProperties::GAP_MOUNT_TYPE;
			pPlayer ->getOriginPack()->TraversingContainer( &lListener );

			//##先检查角色物品
			BOOL bExist = FALSE;
			for( size_t i = 0; i < lListener.m_vGoodsID.size(); i ++ )
			{
				CGoods* pGoods = dynamic_cast<CGoods*>( pPlayer ->getOriginPack()->Find(TYPE_GOODS, lListener.m_vGoodsID[i]) );
				if( pGoods )
				{
					if( pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_MOUNT_TYPE, 1) == m_dwHorseType && 
						pGoods -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_MOUNT_LEVEL, 1) == m_dwHorseLevel )
					{
						bExist = TRUE;
						break;
					}
				}
			}

			if( bExist == FALSE )
			{
				End();
				return;
			}

			//##再检查角色身份
			if( m_dwRoleLimit )
			{
				if( m_dwRoleLimit | CGoodsBaseProperties::PR_GUILD_MASTER )
				{
					//bResult = FALSE;
				}

				if( m_dwRoleLimit | CGoodsBaseProperties::PR_UNION_MASTER )
				{
					//bResult = FALSE;
				}

				if( m_dwRoleLimit | CGoodsBaseProperties::PR_CITY_MASTER )
				{
					//bResult = FALSE;
				}
			}
		}
		else
		{
			End();
		}
	}
}

VOID CRideState::OnChangeRegion( LONG lRegionID )
{
	//##这个状态存在于Sufferer身上，只改变Sufferer的RID
	m_lSuffererRegion = lRegionID;
}

//##类的序列化，用于在游戏服务器之间传输数据
VOID CRideState::Serialize( vector<BYTE>& vOut )
{
	//##是否需要将技能的id和等级都压入？
	//##这个状态只有技能ID，不需要压入等级，但是有些其他的状态需要等级
	_AddToByteArray( &vOut, m_lID );

	//##马的类型,登记
	_AddToByteArray( &vOut, m_dwHorseType );
	_AddToByteArray( &vOut, m_dwHorseLevel );
	_AddToByteArray( &vOut, m_dwRoleLimit );
	_AddToByteArray( &vOut, (char*)m_strGoodName.c_str());
}

VOID CRideState::Unserialize( BYTE* pBytes, LONG& lPos )
{
	//##状态的技能ID已经被事先在CStateFactory中取出，所以不需要再次取出了

	m_dwHorseType	= _GetDwordFromByteArray( pBytes, lPos );
	m_dwHorseLevel	= _GetDwordFromByteArray( pBytes, lPos );
	m_dwRoleLimit	= _GetDwordFromByteArray( pBytes, lPos );
	char str[256];
	m_strGoodName =_GetStringFromByteArray( pBytes, lPos, str);

	//##之后直接调用Begin就可以启动这个状态
}

DWORD CRideState::GetHorseType()
{
	return m_dwHorseType;
}

DWORD CRideState::GetHorseLevel()
{
	return m_dwHorseLevel;
}

DWORD CRideState::GetAdditionalData()
{
	DWORD dwResult = m_dwHorseType << 16;
	dwResult = dwResult | m_dwHorseLevel;
	return dwResult;
}

VOID CRideStateVisualEffect::UpdateVisualEffect( CState* pParent, DWORD dwCmd )
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