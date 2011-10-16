///
/// @file BusinessState.cpp
/// @brief 维护处于跑商操作界面的玩家列表
///
#include "StdAfx.h"
#include "BusinessState.h"
#include "CTradeSpot.h"
#include "../Player.h"
#include "../../../../public/BusinessGoodsDef.h"

namespace Business
{
	void CStateGroup::AddPlayer( const CGUID &player_id, int iOperType )
	{
		if( m_PlayerTable.find( player_id ) != m_PlayerTable.end() )
		{
			BUSINESS_LOG_WARNING( FMT_STR( "Player [%s] has already in business oper state.", GUID_STR( player_id ) ) );
		}
		else
		{
			tagState s;
			memset( &s, 0, sizeof( s ) );
			s.iState = iOperType;
			m_PlayerTable.insert( std::make_pair( player_id, s ) );
		}
	}

	void CStateGroup::RemovePlayer( const CGUID &player_id )
	{
		m_PlayerTable.erase( player_id );
	}

	const tagState &CStateGroup::GetPlayerState( const CGUID &player_id )
	{
		return QueryPlayerState( player_id );
	}

	tagState &CStateGroup::QueryPlayerState( const CGUID &player_id )
	{
		static tagState invalid_s = { OP_NONE, 0 };
		PlayerStateTable::iterator it = m_PlayerTable.find( player_id );
		if( it != m_PlayerTable.end() )
		{
			return it->second;
		}
		else
		{
			return invalid_s;
		}
	}

	void CStateGroup::SendUpdatePriceMsg()
	{
		for( PlayerStateTable::iterator it = m_PlayerTable.begin(); it != m_PlayerTable.end(); )
		{
			tagState &s = it->second;
			CPlayer *player = GetGame()->FindPlayer( it->first );
			if( player == NULL )
			{
				it = m_PlayerTable.erase( it );
			}
			else
			{
				switch( s.iState )
				{
				case OP_BUY:
					SendUpdateBuyPriceMsg( player );
					break;

				case OP_SELL:
					SendUpdateSellPriceMsg( player );
					break;

				default:
					BUSINESS_LOG_WARNING( FMT_STR( "Unknown business oper state [%d]", s.iState ) );
				}
			}
		}
	}

	void CStateGroup::SendUpdateBuyPriceMsg( CPlayer *player )
	{
		assert( m_pSpot != NULL );
		CMessage msg( MSG_S2C_BUSINESS_UPDATEPRICE );
		msg.Add( (BYTE)BUSINESS_BUYPAGE );
		msg.Add( (DWORD)m_pSpot->GetProperty().Id );
		DBWriteSet db;
		msg.GetDBWriteSet( db );
		m_pSpot->GetGoodsList( player, db );
		msg.SendToPlayer( player->GetExID() );
	}

	void CStateGroup::SendUpdateSellPriceMsg( CPlayer *player )
	{
		assert( m_pSpot != NULL );
		CMessage msg( MSG_S2C_BUSINESS_UPDATEPRICE );
		msg.Add( (BYTE)BUSINESS_SELLPAGE );
		msg.Add( (DWORD)m_pSpot->GetProperty().Id );
		DBWriteSet db;
		msg.GetDBWriteSet( db );
		m_pSpot->GetGoodsSellPrice( player, db );
		msg.SendToPlayer( player->GetExID() );
	}

	DWORD CStateGroup::AdjustPrice( CPlayer *pPlayer, DWORD dwBasePrice )
	{
		const tagState &s = GetPlayerState( pPlayer->GetExID() );
		DWORD ret = dwBasePrice;
		switch( s.iState )
		{
		case OP_NONE:
			ret = dwBasePrice;
			break;

		case OP_BUY:
		case OP_SELL:
			{
				if( s.dwHiggleCount != 0 )
				{
					// 有使用‘讨价还价’技能的成功记录
					ret = dwBasePrice * pow( ( 1.0 + s.iPriceQuo * 0.01 ), (int)s.dwHiggleCount );
				}
				else
				{
					ret = dwBasePrice;
				}
			}
			break;

		default:
			BUSINESS_LOG_WARNING( FMT_STR( "Unknown business oper state [%d]", s.iState ) );
		}

		return ret;
	}

	void CStateGroup::SetHiggleRecord( const CGUID &player_id, int iPriceQuo, bool bSuccess )
	{
		tagState &s = QueryPlayerState( player_id );
		if( s.iState == OP_NONE )
		{
			BUSINESS_LOG_WARNING( FMT_STR( "Player [%s] is not in the business state group", GUID_STR( player_id ) ) );
		}
		else
		{
			if( bSuccess )
			{
				s.iPriceQuo = iPriceQuo;
				s.dwHiggleCount ++;
			}
			else
			{
				s.dwHiggleCount = 0;
				s.iPriceQuo = 0;
			}
		}
	}

	bool CStateGroup::UseHiggleSkill( const CGUID &player_id, int iBuyQuo, int iSellQuo, int iSuccessOdds )
	{
		tagState &s = QueryPlayerState( player_id );
		if( random( 100 ) > iSuccessOdds )
		{
			// 一旦失败，价格回到商品本身的价格 
			SetHiggleRecord( player_id, 0, false );
		}
		else
		{
			// success
			if( s.iState == OP_BUY )
			{
				SetHiggleRecord( player_id, iBuyQuo, true );
			}
			else if( s.iState == OP_SELL )
			{
				SetHiggleRecord( player_id, iSellQuo, true );
			}
		}

		// 更新客户端价格显示
		CPlayer *pPlayer = GetGame()->FindPlayer( player_id );
		if( pPlayer == NULL )
		{
			m_PlayerTable.erase( player_id );
			return false;
		}
		else
		{
			if( s.iState == OP_BUY )
			{
				SendUpdateBuyPriceMsg( pPlayer );
			}
			else if( s.iState == OP_SELL )
			{
				SendUpdateSellPriceMsg( pPlayer );
			}
		}

		return true;
	}
}

