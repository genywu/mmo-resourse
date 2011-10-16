#include "StdAfx.h"
#include "../Shape.h"
#include "../Player.h"
#include "CS2CContainerObjectMove.h"
#include "../Session/CSession.h"
#include "../Session/CPlug.h"
#include "../Session/CSessionFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CS2CContainerObjectMove::CS2CContainerObjectMove()
{
	m_otType							= OT_ROLL_BACK;

	m_lSourceContainerType				= 0;
	//m_SourceContainerID				= 0;
	m_lSourceContainerExtendID			= 0;
	m_dwSourceContainerPosition			= 0;

	m_lDestinationContainerType			= 0;
	//m_DestinationContainerID			= 0;
	m_lDestinationContainerExtendID		= 0;
	m_dwDestinationContainerPosition	= 0;

	m_lSourceObjectType					= 0;
	m_dwSourceObjectAmount				= 0;

	m_lDestinationObjectType			= 0;
	m_dwDestinationObjectAmount			= 0;
}

CS2CContainerObjectMove::~CS2CContainerObjectMove()
{
	m_vStream.clear();
}

void CS2CContainerObjectMove::SetOperation( OPERATION_TYPE ot )
{
	m_otType = ot;
}

void CS2CContainerObjectMove::SetSourceContainer( LONG lType, const CGUID& guid, DWORD dwPosition )
{
	m_lSourceContainerType				= lType;
	m_SourceContainerID				= guid;
	m_dwSourceContainerPosition			= dwPosition;
}

void CS2CContainerObjectMove::SetDestinationContainer( LONG lType, const CGUID& guid, DWORD dwPosition )
{
	m_lDestinationContainerType			= lType;
	m_DestinationContainerID			= guid;
	m_dwDestinationContainerPosition	= dwPosition;
}

void CS2CContainerObjectMove::SetSourceObject( LONG lType,const CGUID& guid )
{
	m_lSourceObjectType					= lType;
	m_guSourceObjectID					= guid;
}

void CS2CContainerObjectMove::SetSourceObjectAmount( DWORD dwAmount )
{
	m_dwSourceObjectAmount				= dwAmount;
}

void CS2CContainerObjectMove::SetDestinationObject( LONG lType,const CGUID& guid )
{
	m_lDestinationObjectType			= lType;
	m_guDestinationObjectID				= guid;
}

void CS2CContainerObjectMove::SetDestinationObjectAmount( DWORD dwAmount )
{
	m_dwDestinationObjectAmount			= dwAmount;
}

vector<BYTE>& CS2CContainerObjectMove::GetObjectStream()
{
	return m_vStream;
}

void CS2CContainerObjectMove::SetSourceContainerExtendID( long lID )
{
	m_lSourceContainerExtendID			= lID;
}

void CS2CContainerObjectMove::SetDestinationContainerExtendID( long lID )
{
	m_lDestinationContainerExtendID		= lID;
}

void CS2CContainerObjectMove::SendToSession( const CGUID& Sessionguid )
{
	CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(Sessionguid) );
	if( pSession )
	{
		list<long>::iterator it = pSession -> GetPlugList().begin();
		for( ; it != pSession -> GetPlugList().end(); it ++ )
		{
			CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(*it) );
			if( pPlug && pPlug -> GetOwnerType() == TYPE_PLAYER )
			{
				Send( pPlug -> GetOwnerID() );
			}
		}
	}
}

void CS2CContainerObjectMove::Send( const CGUID& Playerguid )
{
	if( NULL_GUID != Playerguid )
	{
		CMessage mMsg( MSG_S2C_CONTAINER_OBJECT_MOVE );

		//##如果目标和源的类型相同,就是一条标准的移动消息.
		if( m_lSourceObjectType == m_lDestinationObjectType &&
			m_guSourceObjectID == m_guDestinationObjectID )
		{
			m_lDestinationObjectType	= 0;
			m_guDestinationObjectID		= CGUID::GUID_INVALID;

			if( m_lSourceContainerType == m_lDestinationContainerType &&
				m_SourceContainerID == m_DestinationContainerID &&
				m_lSourceContainerExtendID == m_lDestinationContainerExtendID &&
				m_dwSourceContainerPosition == m_dwDestinationContainerPosition )
			{
				//##如果源/目标容器的数据位置也完全相同。那么就表示不需要移动。
				SetOperation( OT_ROLL_BACK );
			}
		}
		else
		{
			//##否则改变数量
		}

		mMsg.Add( static_cast<BYTE>(m_otType) );

		if( m_otType != OT_ROLL_BACK )
		{
			mMsg.Add( m_lSourceContainerType );
			mMsg.Add( m_SourceContainerID );
			mMsg.Add( m_lSourceContainerExtendID );
			mMsg.Add( m_dwSourceContainerPosition );
			mMsg.Add( m_lDestinationContainerType );
			mMsg.Add( m_DestinationContainerID );
			mMsg.Add( m_lDestinationContainerExtendID );
			mMsg.Add( m_dwDestinationContainerPosition );
			mMsg.Add( m_lSourceObjectType );
			//mMsg.Add( m_lSourceObjectID );
			mMsg.Add( m_guSourceObjectID );
			mMsg.Add( m_lDestinationObjectType );
			//mMsg.Add( m_lDestinationObjectID );
			mMsg.Add( m_guDestinationObjectID );

			//##添加原物品的描述
			if( m_otType == OT_NEW_OBJECT )
			{
				mMsg.Add( m_dwDestinationObjectAmount );
			}
			else
			{
				mMsg.Add( m_dwSourceObjectAmount );
			}

			//##添加目标物的描述
			if( m_otType == OT_NEW_OBJECT )
			{
				mMsg.Add( static_cast<DWORD>( m_vStream.size() ) );
				if(m_vStream.size() > 0)
					mMsg.AddEx( &m_vStream[0], static_cast<LONG>( m_vStream.size() ) );				
			}
			else if(m_lSourceContainerType==TYPE_REGION && m_lDestinationContainerType==TYPE_PLAYER)
			{
				mMsg.Add( static_cast<DWORD>( m_vStream.size() ) );
				if(m_vStream.size() > 0)
					mMsg.AddEx( &m_vStream[0], static_cast<LONG>( m_vStream.size() ) );	
			}
			else if( m_otType != OT_DELETE_OBJECT )
			{
				
			}

			if(m_otType==OT_SWITCH_FILL)
			{
				if(m_lDestinationContainerExtendID==PEI_PACK)
				{
					CVolumeLimitGoodsContainer* pContainer=NULL;
					CPlayer* pDesPlayer=GetGame()->FindPlayer(m_DestinationContainerID);
					if(pDesPlayer)
					{
						int index=m_dwDestinationContainerPosition-PEI_PACK1;
						pContainer=pDesPlayer->getSubpackContainer()->GetSubpack(index)->pSubpackContainer;
						pContainer->AddGuid2Msg(&mMsg);
					}					
				}
				else if(m_lDestinationContainerExtendID==eDCT_Secondary)
				{
					CPlayer* pDesPlayer=GetGame()->FindPlayer(m_DestinationContainerID);
					if(pDesPlayer)
					{
						CVolumeLimitGoodsContainer* pContainer = dynamic_cast<CVolumeLimitGoodsContainer*>(pDesPlayer->GetPlayerDepot()->FindContainer(eDCT_Secondary + m_dwDestinationContainerPosition - 10000));
						if (NULL != pContainer)
						{
							pContainer->AddGuid2Msg(&mMsg);
						}
					}
				}
				// 商业背包交换
				else if( m_lDestinationContainerExtendID == PEI_BUSSINESPACKFIELD )
				{
					CPlayer *pPlayer = GetGame()->FindPlayer( m_DestinationContainerID );
					if( pPlayer != NULL && pPlayer->GetBusinessContainer().IsValid() )
					{
						pPlayer->GetBusinessContainer().m_pContainer->AddGuid2Msg( &mMsg );
					}
				}
			}
		}

//#ifdef _DEBUG
//		CPlayer* pPlayer = GetGame() -> FindPlayer( Playerguid );
//		if( pPlayer )
//		{
//			CHAR szDebug[512];
//			_snprintf( szDebug, "回复客户端%s消息：Op: %d Src: %d, %d, eid %d, pos %d. Dest: %d, %d, eid %d, pos %d. Src Obj: %d, %d, %d. Dest Obj: %d, %d, %d.\n", pPlayer -> GetName(), 
//				m_otType, m_lSourceContainerType, m_SourceContainerID, m_lSourceContainerExtendID, m_dwSourceContainerPosition, 
//				m_lDestinationContainerType, m_DestinationContainerID, m_lDestinationContainerExtendID, m_dwDestinationContainerPosition, 
//				m_lSourceObjectType, 0, m_dwSourceObjectAmount,
//				m_lDestinationObjectType, 0, m_dwDestinationObjectAmount );
//			OutputDebugString( szDebug );
//		}
//#endif

		mMsg.SendToPlayer( Playerguid );
	}
}

void CS2CContainerObjectMove::SendToAround( CShape* pShape )
{
	if( pShape )
	{
		CMessage mMsg( MSG_S2C_CONTAINER_OBJECT_MOVE );
		mMsg.Add( static_cast<BYTE>(m_otType) );
		if( m_otType != OT_ROLL_BACK )
		{
			mMsg.Add( m_lSourceContainerType );
			mMsg.Add( m_SourceContainerID );
			mMsg.Add( m_lSourceContainerExtendID );
			mMsg.Add( m_dwSourceContainerPosition );
			mMsg.Add( m_lDestinationContainerType );
			mMsg.Add( m_DestinationContainerID );
			mMsg.Add( m_lDestinationContainerExtendID );
			mMsg.Add( m_dwDestinationContainerPosition );
			mMsg.Add( m_lSourceObjectType );
			mMsg.Add( m_guSourceObjectID );
			mMsg.Add( m_lDestinationObjectType );
			mMsg.Add( m_guDestinationObjectID );

			//##添加原物品的描述			
			mMsg.Add( m_dwSourceObjectAmount );
			

			//##添加目标物的描述
			if( m_otType == OT_NEW_OBJECT )
			{
				mMsg.Add( static_cast<DWORD>( m_vStream.size() ) );
				mMsg.AddEx( &m_vStream[0], static_cast<LONG>( m_vStream.size() ) );
			}
			else if( m_otType != OT_DELETE_OBJECT )
			{
				mMsg.Add( m_dwDestinationObjectAmount );
			}
		}
		mMsg.SendToAround( pShape, dynamic_cast<CPlayer*>(pShape) );
	}
}

LONG CS2CContainerObjectMove::GetSourceContainerType()
{
	return m_lSourceContainerType;
}

LONG CS2CContainerObjectMove::GetDestinationContainerType()
{
	return m_lDestinationContainerType;
}

LONG CS2CContainerObjectMove::GetSourceObjectType()
{
	return m_lSourceObjectType;
}

CGUID& CS2CContainerObjectMove::GetguSourceObjectID()
{
	return m_guSourceObjectID;
}

DWORD CS2CContainerObjectMove::GetSourceObjectAmount()
{
	return m_dwSourceObjectAmount;
}


LONG CS2CContainerObjectMove::GetDestinationObjectType()
{
	return m_lDestinationObjectType;
}

CGUID&	CS2CContainerObjectMove::GetguDestinationObjectID()
{
	return m_guDestinationObjectID;
}

DWORD CS2CContainerObjectMove::GetDestinationObjectAmount()
{
	return m_dwDestinationObjectAmount;
}

CPlayer* CS2CContainerObjectMove::GetDestPlayer(void)
{
	if(TYPE_PLAYER != m_lDestinationContainerType) return NULL;
	return GetGame()->FindPlayer(m_DestinationContainerID);
}

CS2CContainerObjectMove::OPERATION_TYPE CS2CContainerObjectMove::GetOperation()
{
	return m_otType;
}

LONG CS2CContainerObjectMove::GetSourceContainerExtendID()
{
	return m_lSourceContainerExtendID;
}

LONG CS2CContainerObjectMove::GetDestinationContainerExtendID()
{
	return m_lDestinationContainerExtendID;
}
