
#include "StdAfx.h"
/*
#include "cteam.h"
#include "cteamate.h"
#include "csessionfactory.h"
#include <mmsystem.h>
#include "..\Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<DWORD, CTeam::tagQueryResult> CTeam::s_mQuestedTeams;

CTeam::CTeam(LONG lID, DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime):CSession(lID, dwMinPlugs, dwMaxPlugs, dwLifeTime)
{
	m_dwSessionType				= CSessionFactory::ST_TEAM;
	m_asAllocationScheme		= AS_SHARE;
	m_dwLastCheckedTimeStamp	= 0;
	m_lTeamLeaderID				= 0;
}

CTeam::CTeam(DWORD dwMinPlugs, DWORD dwMaxPlugs, DWORD dwLifeTime):CSession(dwMinPlugs, dwMaxPlugs, dwLifeTime)
{
	m_dwSessionType				= CSessionFactory::ST_TEAM;
	m_asAllocationScheme		= AS_SHARE;
	m_dwLastCheckedTimeStamp	= 0;
	m_lTeamLeaderID				= 0;
}

CTeam::~CTeam(void)
{
	Release();
}

CTeam::tagQueryResult::tagQueryResult() : dwQueryTimeStamp(0), bQueried(FALSE), bSucceed(FALSE)
{
}

VOID CTeam::QuestTeamData( DWORD dwID )
{
	tagQueryResult tResult;
	s_mQuestedTeams[dwID] = tResult;
}

VOID CTeam::CompleteTeamData( DWORD dwID )
{
	map<DWORD,tagQueryResult>::iterator it = s_mQuestedTeams.find( dwID );
	if( it != s_mQuestedTeams.end() )
	{
		it -> second.bSucceed = TRUE;
	}
}

//##对于队长的传位,只能由队长所在的服务器来完成.如果队长的服务器因为宕机导致无法传位,则由WorldServer来处理一次。
VOID CTeam::SetTeamID( DWORD dwID )
{
	m_dwTeamID = dwID;
}

DWORD CTeam::GetTeamID()
{
	return m_dwTeamID;
}

VOID CTeam::SetLeader( LONG lPlayerID )
{
	CPlug* pTeamate = QueryPlugByOwner( TYPE_PLAYER, lPlayerID );
	if( IsSessionEnded() == FALSE && pTeamate )
	{
		LONG lPlugID = pTeamate -> GetID();
		if( m_lTeamLeaderID )
		{
			CPlug* pPlug = QueryPlugByOwner( TYPE_PLAYER, m_lTeamLeaderID );
			if( pPlug )
			{
				lPlugID = pPlug -> GetID();
			}
		}
		m_lTeamLeaderID = lPlayerID;
		OnPlugChangeState( lPlugID, CTeamate::TS_CHANGE_LEADER, reinterpret_cast<BYTE*>(&lPlayerID), TRUE );
	}
}

LONG CTeam::GetLeader()
{
	return m_lTeamLeaderID;
}

CHAR* CTeam::GetPassword()
{
	return const_cast<CHAR*>( m_strPassword.c_str() );
}

VOID CTeam::SetPassword( CHAR* pPassword )
{
	if( pPassword )
	{
		m_strPassword = pPassword;
	}
}

CHAR* CTeam::GetTeamName()
{
	return const_cast<CHAR*>( m_strTeamName.c_str() );
}

VOID CTeam::SetTeamName( CHAR* pTeamName )
{
	if( pTeamName )
	{
		m_strTeamName = pTeamName;
	}
}

VOID CTeam::AI()
{
	CSession::AI();

	//##需要做得是判定该Session内是否有Plug是否都在本服务器，如果一个队员都没有，则删除这个Session
	if( m_dwLastCheckedTimeStamp )
	{
		if( timeGetTime() >= m_dwLastCheckedTimeStamp + 60000 )
		{
			//##确定该Session是否还有效。
			BOOL bSessionValid = FALSE;
			list<LONG>::iterator it = GetPlugList().begin();
			for( ; it != GetPlugList().end(); it ++ )
			{
				CTeamate* pTeamate = dynamic_cast<CTeamate*>( const_cast<CPlug*>( CSessionFactory::QueryPlug( (*it) ) ) );
				if( pTeamate )
				{
					if( GetGame() -> FindPlayer( pTeamate -> GetOwnerID() ) )
					{
						bSessionValid = TRUE;
						break;
					}
				}
			}

			//##如果会话已经无效，则删除该Session；
			if( bSessionValid == FALSE )
			{
				Abort();
			}
		}
	}
	else
	{
		m_dwLastCheckedTimeStamp = timeGetTime();
	}
}

DWORD CTeam::GetTeamatesAmount()
{
	DWORD dwResult = 0;
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CPlug* pPlug = QueryPlugByID( *it );
		if( pPlug )
		{
			++ dwResult;
		}
	}
	return dwResult;
}

DWORD CTeam::GetCurrentServerTeamatesAmount()
{
	DWORD dwResult = 0;
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CPlug* pPlug = QueryPlugByID( *it );
		if( pPlug )
		{
			if( pPlug -> GetOwner() )
			{
				++ dwResult;
			}
		}
	}
	return dwResult;
}

//给队伍添加一个任务
VOID CTeam::AddQuest(WORD wQuestID,CPlayer* pPlayer,long lDistance)
{
	if(pPlayer == NULL)	return;

	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CPlug* pPlug = QueryPlugByID( *it );
		if( pPlug && pPlug->GetOwnerType() == TYPE_PLAYER)
		{
			CPlayer* pTemptPlayer = GetGame()->FindPlayer( pPlug->GetOwnerID() );
			if( pTemptPlayer )
			{
				if(lDistance == 0)
				{
					pTemptPlayer->AddQuest(wQuestID);
				}
				else
				{
					if( pPlayer->Distance(pTemptPlayer) <= lDistance)
					{
						pTemptPlayer->AddQuest(wQuestID);
					}
				}
			}
			else
			{
				if(lDistance == 0)
				{
					CMessage msg(MSG_S2W_QUEST_PlayerAddQuest);
					msg.Add( pPlug->GetOwnerID() );
					msg.Add(wQuestID);
					msg.Send();
				}
			}
		}
	}
}

//给队伍出发一个脚本
VOID CTeam::RunScript(char* strScirptName,CPlayer* pPlayer,long lDistance)
{
	if(pPlayer == NULL)	return;

	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CPlug* pPlug = QueryPlugByID( *it );
		if( pPlug && pPlug->GetOwnerType() == TYPE_PLAYER)
		{
			CPlayer* pTemptPlayer = GetGame()->FindPlayer( pPlug->GetOwnerID() );
			if( pTemptPlayer && pTemptPlayer->IsDied() == false)
			{
				if(lDistance == 0)
				{
					pTemptPlayer->PlayerRunScript(strScirptName);
				}
				else
				{
					if( pPlayer->Distance(pTemptPlayer) <= lDistance)
					{
						pTemptPlayer->PlayerRunScript(strScirptName);
					}
				}
			}
			else
			{
				if(lDistance == 0)
				{
					CMessage msg(MSG_S2W_QUEST_PlayerRunScript);
					msg.Add( pPlug->GetOwnerID() );
					msg.Add(strScirptName);
					msg.Send();
				}
			}
		}
	}
}

DWORD CTeam::GetCurrentRegionTeamatesAmount( LONG lRegionID )
{
	DWORD dwResult = 0;
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(*it) );
		if( pPlug )
		{
			if( pPlug -> GetOwnerRegionID() == lRegionID )
			{
				++ dwResult;
			}
		}
	}
	return dwResult;
}

// 获取当前场景中活着的队友
DWORD CTeam::GetCurrentRegionTeamatesAmount_Alive( LONG lRegionID )
{
	DWORD dwResult = 0;
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(*it) );
		if( pPlug )
		{
			if( pPlug -> GetOwnerRegionID() == lRegionID )
			{
				CMoveShape* pShape = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
				if (pShape && pShape->IsDied() == false)
				{
					++ dwResult;
				}
			}
		}
	}
	return dwResult;
}

//得到队伍里边在当前场景的成员
VOID CTeam::GetCurrentRegionTeamates(map<long,CPlayer*>& RegionTeamates,LONG lRegionID)
{
	if(lRegionID <= 0)	return;

	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(*it) );
		if( pPlug )
		{
			CPlayer* pTeamate = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
			if( pTeamate && pTeamate->GetRegionID() == lRegionID)
			{
				RegionTeamates[pTeamate->GetID()] = pTeamate;
			}
		}
	}
}

//得到队伍里边在当前场景的活着的成员
VOID CTeam::GetCurrentRegionTeamates_Alive(map<long,CPlayer*>& RegionTeamates,LONG lRegionID)
{
	if(lRegionID <= 0)	return;

	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(*it) );
		if( pPlug )
		{
			CPlayer* pTeamate = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
			if( pTeamate && pTeamate->GetRegionID() == lRegionID && pTeamate->IsDied() == false)
			{
				RegionTeamates[pTeamate->GetID()] = pTeamate;
			}
		}
	}
}

FLOAT CTeam::GetCurrentRegionTeamatesAverageLevel( LONG lRegionID )
{
	FLOAT fResult = 0.0f;
	DWORD dwCount = 0;
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(*it) );
		if( pPlug )
		{
			if( pPlug -> GetOwnerRegionID() == lRegionID )
			{
				CMoveShape* pShape = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
				if( pShape )
				{
					fResult += pShape -> GetLevel();
					++ dwCount;
				}
			}
		}
	}
	if( dwCount > 0 )
	{
		fResult /= dwCount;
	}
	return fResult;
}

FLOAT CTeam::GetCurrentRegionTeamatesAverageLevel_Alive( LONG lRegionID )
{
	FLOAT fResult = 0.0f;
	DWORD dwCount = 0;
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(*it) );
		if( pPlug )
		{
			if( pPlug -> GetOwnerRegionID() == lRegionID )
			{
				CMoveShape* pShape = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
				if( pShape && pShape->IsDied() == false)
				{
					fResult += pShape -> GetLevel();
					++ dwCount;
				}
			}
		}
	}
	if( dwCount > 0 )
	{
		fResult /= dwCount;
	}
	return fResult;
}

CPlayer* CTeam::FindTeamatesInCurrentRegion( LONG lRegionID )
{
	CPlayer* pResult = NULL;
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(*it) );
		if( pPlug )
		{
			if( pPlug -> GetOwnerRegionID() == lRegionID )
			{
				pResult = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
				if( pResult )
				{
					break;
				}
			}
		}
	}
	return pResult;
}

BOOL CTeam::Serialize( vector<BYTE>* pStream )
{
	CSession::Serialize( pStream );

	//##Session的信息
	_AddToByteArray( pStream, m_dwTeamID );
	_AddToByteArray( pStream, const_cast<CHAR*>( m_strTeamName.c_str() ) );
	_AddToByteArray( pStream, const_cast<CHAR*>( m_strPassword.c_str() ) );
	_AddToByteArray( pStream, m_lTeamLeaderID );

	//##Plug的信息
	DWORD dwNumTeamates = GetTeamatesAmount();
	_AddToByteArray( pStream, dwNumTeamates );

	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		CPlug* pPlug = QueryPlugByID( *it );
		if( pPlug )
		{
			pPlug -> Serialize( pStream );
		}
	}

	return TRUE;
}

BOOL CTeam::Unserialize( BYTE* pStream, LONG& lOffset )
{
	BOOL bResult = TRUE;
	CSession::Unserialize( pStream, lOffset );
	if( pStream )
	{
		CHAR szMsg[256];
		m_dwTeamID		= _GetDwordFromByteArray( pStream, lOffset );
		m_strTeamName	= _GetStringFromByteArray( pStream, lOffset, szMsg );
		m_strPassword	= _GetStringFromByteArray( pStream, lOffset, szMsg );
		m_lTeamLeaderID	= _GetLongFromByteArray( pStream, lOffset );

		Start();

		//##获取队员的信息
		DWORD dwNumTeamates = _GetDwordFromByteArray( pStream, lOffset );
		for( DWORD i = 0; i < dwNumTeamates; i ++ )
		{
			LONG lPlugID = CSessionFactory::UnserializePlug( pStream, lOffset );
			if( lPlugID )
			{
				CSessionFactory::InsertPlug( GetID(), lPlugID );
			}
			else
			{
				bResult = FALSE;
				break;
			}
		}
	}
	return bResult;
}

BOOL CTeam::OnSessionStarted( BOOL bFlag )
{
	//##添加队伍的索引
	GetGame() -> AddTeam( m_dwTeamID, GetID() );

	if( bFlag == TRUE )
	{
		//##发送消息给World Server
		vector<BYTE> vOut;
		Serialize( &vOut );

		CMessage msg( MSG_S2W_TEAM_CREATE );
		msg.AddEx( &vOut[0], static_cast<LONG>( vOut.size() ) );
		msg.Send();
	}

	return TRUE;
}

//##对于End，将是队长直接调用该函数。终止队伍的存在。这儿需要发送消息给WorldServer。
BOOL CTeam::OnSessionEnded( BOOL bFlag )
{
	//##发送消息给World Server
	CMessage msg( MSG_S2W_TEAM_DESTORY );
	msg.Add( GetTeamID() );
	msg.Send();

	//##在这儿移出所有的Plug
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		OnPlugEnded( *it );
	}

	GetGame() -> RemoveTeam( m_dwTeamID );
	return TRUE;
}

//##对于End，是在人数不足，Session不再有效的时候，终止队伍的存在。这儿不需要发送消息给WorldServer。
BOOL CTeam::OnSessionAborted( BOOL bFlag )
{
	//##在这儿移出所有的Plug
	list<LONG>::iterator it = GetPlugList().begin();
	for( ; it != GetPlugList().end(); it ++ )
	{
		OnPlugAborted( *it );
	}

	//##发送消息给World Server
	GetGame() -> RemoveTeam( m_dwTeamID );
	return TRUE;
}

BOOL CTeam::OnPlugInserted(LONG lPlugID)
{
	BOOL bResult = FALSE;

	//##首先要设置好这个Plug的TeamID
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(lPlugID) );
	if( pPlug )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
		if( pPlayer )
		{
			pPlayer -> SetTeamID( GetTeamID() );
		}

		//##然后让其他Plug知道。
		bResult = CSession::OnPlugInserted( lPlugID );
	}

	return bResult;
}

BOOL CTeam::OnPlugEnded(LONG lPlugID)
{
	BOOL bResult = FALSE;

	//##首先要设置好这个Plug的TeamID
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(lPlugID) );
	if( pPlug )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
		if( pPlayer )
		{
			pPlayer -> SetTeamID( 0 );
		}
		bResult = CSession::OnPlugEnded( lPlugID );
	}
	return bResult;
}

BOOL CTeam::OnPlugAborted(LONG lPlugID)
{
	BOOL bResult = FALSE;

	//##首先要设置好这个Plug的TeamID
	CPlug* pPlug = const_cast<CPlug*>( CSessionFactory::QueryPlug(lPlugID) );
	if( pPlug )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( pPlug -> GetOwner() );
		if( pPlayer )
		{
			pPlayer -> SetTeamID( 0 );
		}

		bResult = CSession::OnPlugEnded( lPlugID );
	}

	return bResult;
}


VOID CTeam::SetAllocationScheme( CTeam::ALLOCATION_SCHEME as )
{
	CPlug* pPlug = QueryPlugByOwner( TYPE_PLAYER, m_lTeamLeaderID );
	if( pPlug )
	{
		m_asAllocationScheme = as;

		OnPlugChangeState( pPlug -> GetID(), CTeamate::TS_CHANGE_ALLOCATION_SCHEME, 
			reinterpret_cast<UCHAR*>(&m_asAllocationScheme), TRUE );
	}
}

CTeam::ALLOCATION_SCHEME CTeam::GetAllocationScheme()
{
	return m_asAllocationScheme;
}

VOID CTeam::KickPlayer( LONG lPlayerID )
{
	CPlug* pPlug = QueryPlugByOwner( TYPE_PLAYER, lPlayerID );
	if( pPlug )
	{
		if( pPlug -> GetOwner() )
		{
			//##在当前服务器上。
			pPlug -> Exit();
		}
		else
		{
			//##发送消息给WorldServer，踢出某个玩家。
			CMessage msg( MSG_S2W_TEAM_KICK_PLAYER );
			msg.Add( GetTeamID() );
			msg.Add( lPlayerID );
			msg.Send();
		}
	}
}

BOOL CTeam::OnPlugChangeState(LONG lPlugID, LONG lState, BYTE* pData, BOOL bSendToTrigger)
{
	BOOL bResult = CSession::OnPlugChangeState( lPlugID, lState, pData, bSendToTrigger );
	if( bResult )
	{
		//##如果需要发送给World Server，则发送。
		switch( lState )
		{
		case CTeamate::TS_CHANGE_ALLOCATION_SCHEME:
			{
				if( pData )
				{
					CPlug* pPlug = QueryPlugByID( lPlugID );
					if( pPlug )
					{
						//##如果这个Owner在该服务器。那么就向WorldServer发送消息。否则不发送。
						if( pPlug -> GetOwner() )
						{
							CTeam::ALLOCATION_SCHEME as = CTeam::AS_FORCE_DWORD;
							memcpy( &as, pData, sizeof(CTeam::ALLOCATION_SCHEME) );

							CMessage msg( MSG_S2W_TEAM_CHANGE_ALLOCATION_SCHEME );
							msg.Add( GetTeamID() );
							msg.Add( LONG(as) );
							msg.Send();
						}
					}
				}
			}
			break;
		case CTeamate::TS_CHANGE_LEADER:
			{
				if( pData )
				{
					CPlug* pPlug = QueryPlugByID( lPlugID );
					if( pPlug )
					{
						//##如果这个Owner在该服务器。那么就向WorldServer发送消息。否则不发送。
						if( pPlug -> GetOwner() )
						{
							LONG lNewLeaderID = 0;
							memcpy( &lNewLeaderID, pData, sizeof(LONG) );
							if( lNewLeaderID && QueryPlugByOwner(TYPE_PLAYER, lNewLeaderID) )
							{
								CMessage msg( MSG_S2W_TEAM_CHANGE_LEAER );
								msg.Add( GetTeamID() );
								msg.Add( lNewLeaderID );
								msg.Send();
							}
						}
					}
				}
			}
			break;
		case CTeamate::TS_CHANGE_REGION:
			{
				if( pData )
				{
					//CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(lPlugID) );
					//if( pPlug )
					//{
					//	//##如果这个Owner在该服务器。那么就向WorldServer发送消息。否则不发送。
					//	if( pPlug -> GetOwner() )
					//	{
					//		LONG lNewRegionID = 0;
					//		memcpy( &lNewRegionID, pData, sizeof(LONG) );
					//		if( lNewRegionID )
					//		{
					//			CMessage msg( MSG_S2W_TEAM_TEAMATE_CHANGE_REGION );
					//			msg.Add( GetTeamID() );
					//			msg.Add( pPlug -> GetOwnerType() );
					//			msg.Add( pPlug -> GetOwnerID() );
					//			msg.Add( lNewRegionID );
					//			msg.Send();
					//		}
					//	}
					//}
				}
			}
			break;
		case CTeamate::PS_INSERTED:
			{
				//##由该玩家所在的服务器负责发送消息给WorldServer。
				CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(lPlugID) );
				if( pPlug )
				{
					CMoveShape* pShape = pPlug -> GetOwner();
					//##如果这个Owner在该服务器。那么就向WorldServer发送消息。否则不发送。
					if( pShape )
					{
						CMessage msg( MSG_S2W_TEAM_TEAMATE_JOIN );
						msg.Add( GetTeamID() );
						msg.Add( pPlug -> GetOwnerType() );
						msg.Add( pPlug -> GetOwnerID() );
						msg.Add( pPlug -> GetOwnerRegionID() );
						msg.Add( pPlug -> GetOwnerName() );
						msg.Send();
					}
				}
			}
			break;
		case CTeamate::TS_CHAT:
			{
				if( pData )
				{
					CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(lPlugID) );
					if( pPlug )
					{
						//##如果这个Owner在该服务器。那么就向WorldServer发送消息。否则不发送。
						if( pPlug -> GetOwner() )
						{
							CMessage msg( MSG_S2W_TEAM_CHAT );
							msg.Add( GetTeamID() );
							msg.Add( pPlug -> GetOwnerType() );
							msg.Add( pPlug -> GetOwnerID() );
							msg.Add( reinterpret_cast<CHAR*>(pData) );
							msg.Send();
						}
					}
				}
			}
			break;
		case CTeamate::TS_CHANGE_STATE:
			{
				if( pData )
				{
					CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(lPlugID) );
					if( pPlug )
					{
						//##如果这个Owner在该服务器。那么就向WorldServer发送消息。否则不发送。
						if( pPlug -> GetOwner() )
						{
							FLOAT fHp = 0;
							memcpy( &fHp, pData, sizeof(FLOAT) );

							CMessage msg( MSG_S2W_TEAM_CHANGE_STATE );
							msg.Add( GetTeamID() );
							msg.Add( pPlug -> GetOwnerType() );
							msg.Add( pPlug -> GetOwnerID() );
							msg.Add( fHp );
							msg.Send();
						}
					}
				}
			}
			break;
		case CTeamate::PS_REMOVED:
		case CTeamate::PS_ABORTED:
			{
				//##由该玩家所在的服务器负责发送消息给WorldServer。
				CTeamate* pPlug = dynamic_cast<CTeamate*>( QueryPlugByID(lPlugID) );
				if( pPlug )
				{
					CMoveShape* pShape = pPlug -> GetOwner();
					//##如果这个Owner在该服务器。那么就向WorldServer发送消息。否则不发送。
					if( pShape )
					{
						CMessage msg( MSG_S2W_TEAM_TEAMATE_LEAVE );
						msg.Add( GetTeamID() );
						msg.Add( pPlug -> GetOwnerType() );
						msg.Add( pPlug -> GetOwnerID() );
						msg.Send();
					}
				}
			}
			break;
		}
	}
	return bResult;
}
*/