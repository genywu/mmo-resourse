#include "stdafx.h"
//#include "..\..\nets\networld\message.h"
//#include "CTeamate.h"
//#include "CSession.h"
//#include "CSessionFactory.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
//
//CTeamate::CTeamate()
//{
//	m_lOwnerRegionID			= 0;
//	m_dwLastQueriedTimeStamp	= 0;
//	m_bPlayerStillExisted		= TRUE;
//	m_dwPlugType				= CSessionFactory::PT_TEAMATE;
//}
//
//CTeamate::~CTeamate()
//{
//	Release();
//}
//
//VOID CTeamate::SetOwnerName( CHAR* szName )
//{
//	if( szName )
//	{
//		m_strOwnerName = szName;
//	}
//}
//
//CHAR* CTeamate::GetOwnerName()
//{
//	return const_cast<CHAR*>( m_strOwnerName.c_str() );
//}
//
//BOOL CTeamate::IsPlugAvailable()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile	BOOL bResult = TRUE;
//	//##每5分钟检测一次玩家是否存在.
//	BOOL bResult = TRUE;
//	if( m_dwLastQueriedTimeStamp )
//	{
//		if( timeGetTime() > m_dwLastQueriedTimeStamp + 60000 )
//		{
//			if( m_bPlayerStillExisted == FALSE )
//			{
//				bResult = FALSE;
//			}
//			else
//			{
//				//##发送消息到各个Gameserver查询玩家是否存在。
//				CGame::tagGameServer* pServer = GetGame() -> GetPlayerGameServer( GetOwnerID() );
//				if( pServer )
//				{
//					DWORD dwIndex = pServer -> dwIndex;
//					m_bPlayerStillExisted = FALSE;
//
//					CMessage msg( MSG_W2S_TEAM_QUERY_PLAYER );
//					msg.Add( GetID() );
//					msg.Add( GetOwnerType() );
//					msg.Add( GetOwnerID() );
//					msg.SendGSID( dwIndex );
//				}
//			}
//
//			m_dwLastQueriedTimeStamp = timeGetTime();
//		}
//	}
//	else
//	{
//		m_dwLastQueriedTimeStamp	= timeGetTime();
//		m_bPlayerStillExisted		= TRUE;
//	}
//	return bResult;
//}
//
////##ModelId=41E0DC5C005D
//BOOL CTeamate::OnSessionEnded()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	//##发送消息给下面的GameServer？还是Session一次性发送？
//	return TRUE;
//}
//
////##ModelId=41E0DC64032C
//BOOL CTeamate::OnSessionAborted()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	//##发送消息给下面的GameServer？还是Session一次性发送？
//	return TRUE;
//}
//
//VOID CTeamate::SetOwnerRegionID( LONG lID )
//{
//	m_lOwnerRegionID = lID;
//	ChangeState( CTeamate::TS_CHANGE_REGION, reinterpret_cast<UCHAR*>(&m_lOwnerRegionID) );
//}
//
//LONG CTeamate::GetOwnerRegionID()
//{
//	return m_lOwnerRegionID;
//}
//
//BOOL CTeamate::Serialize( vector<BYTE>* pStream )
//{
//	BOOL bResult = FALSE;
//	if( pStream )
//	{
//		if( CPlug::Serialize(pStream) )
//		{
//			_AddToByteArray( pStream, m_lOwnerRegionID );
//			_AddToByteArray( pStream, const_cast<CHAR*>( m_strOwnerName.c_str() ) );
//			bResult = TRUE;
//		}
//	}
//	return bResult;
//}
//
//BOOL CTeamate::Unserialize( BYTE* pStream, LONG& lOffset )
//{
//	BOOL bResult = FALSE;
//	if( pStream )
//	{
//		if( CPlug::Unserialize(pStream, lOffset) )
//		{
//			CHAR szMsg[256];
//			m_lOwnerRegionID	= _GetLongFromByteArray( pStream, lOffset );
//			m_strOwnerName		= _GetStringFromByteArray( pStream, lOffset, szMsg );
//			bResult = TRUE;
//		}
//	}
//	return bResult;
//}
//
//BOOL CTeamate::OnPlugInserted()
//{
//	return TRUE;
//}
//
//BOOL CTeamate::OnPlugAborted()
//{
//	return TRUE;
//}
//
//BOOL CTeamate::OnPlugEnded()
//{
//	return TRUE;
//}
//
//BOOL CTeamate::OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData)
//{
//	CSession* pSession = GetSession();
//	if( pSession )
//	{
//		CPlug* pPlug = pSession -> QueryPlugByID( lPlugID );
//		if( pPlug )
//		{
//			switch( lStateID )
//			{
//			case CTeamate::TS_CHANGE_ALLOCATION_SCHEME:
//				break;
//			case CTeamate::TS_CHANGE_LEADER:
//				break;
//			case CTeamate::TS_CHANGE_REGION:
//				break;
//			case CTeamate::PS_INSERTED:
//				//CMessage msg( CMessage
//				break;
//			case CTeamate::PS_REMOVED:
//				break;
//			case CTeamate::PS_ABORTED:
//				break;
//			}
//		}
//	}
//	return TRUE;
//}
//
//VOID CTeamate::PlayerStillExisted( BOOL bResult )
//{
//	m_bPlayerStillExisted = bResult;
//}