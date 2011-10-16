///
/// @file CBusinessManager.cpp
/// @brief 商业系统对外模块顶层接口
///
#include "StdAfx.h"
#include "CBusinessManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Business
{
	CBusinessManager::~CBusinessManager()
	{
		Release();
	}

	void CBusinessManager::Release()
	{
		// trade area
		for( TradeAreaTable::iterator it = m_Areas.begin(); it != m_Areas.end(); ++ it )
		{
			OBJ_RELEASE(CTradeArea, it->second);
		}

		// trade spot
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			OBJ_RELEASE(CTradeSpot, it->second);
		}

		m_CreditDesc.clear();
		m_Areas.clear();
		m_Spots.clear();
		Log4c::Trace(ROOT_MODULE, "BusinessManager released." );
	}

	void CBusinessManager::StartRefreshBuyLimitTimer( int RegionResID, const CGUID &player_id )
	{
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			const tagTradeSpotProperty &p = it->second->GetProperty();
			if( p.RegionResID == RegionResID )
			{
				it->second->StartRefreshBuyLimitTimer( player_id );
			}
		}
	}

	void CBusinessManager::Startup()
	{
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			it->second->StartPriceTimer();
		}
	}

	void CBusinessManager::Shutdown()
	{
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			it->second->StopPriceTimer();
		}
	}

	void CBusinessManager::DecodeFromDataBlock( DBReadSet &db )
	{
		// 全局配置
		db.GetBufferFromByteArray( &m_Setup, sizeof( m_Setup ) );

		// 声望描述
		m_CreditDesc.clear();
		long lCreditDescSize = db.GetLongFromByteArray();
		for( long i = 0; i < lCreditDescSize; ++ i )
		{
			tagCreditDesc cd;
			db.GetBufferFromByteArray( &cd, sizeof( cd ) );
			m_CreditDesc.push_back( cd );
		}

		// 贸易区域配置
		long lAreaCount = db.GetLongFromByteArray();
		for( long i = 0; i < lAreaCount; ++ i )
		{
			tagTradeAreaProperty property;
			db.GetBufferFromByteArray( &property, sizeof( property ) );
			TradeAreaTable::iterator it = m_Areas.find( property.Id );
			if( it == m_Areas.end() )
			{
				CTradeArea *pArea = OBJ_CREATE(CTradeArea);
				pArea->SetProperty( property );			
				m_Areas.insert( std::make_pair( property.Id, pArea ) );
			}
			else
			{
				it->second->SetProperty( property );
			}
		}

		// 贸易点配置
		long lSpotCount = db.GetLongFromByteArray();
		for( long i = 0; i < lSpotCount; ++ i )
		{
			// get the flag
			BYTE valid = db.GetByteFromByteArray();
			if( valid == 0 )
			{
				// invalid
				continue;
			}

			tagTradeSpotProperty property;
			db.GetBufferFromByteArray( &property, sizeof( property ) );
			TradeSpotTable::iterator it = m_Spots.find( property.Id );
			if( it == m_Spots.end() )
			{
				CTradeSpot *pSpot = OBJ_CREATE_PVOID(CTradeSpot, (void*)&property );
				pSpot->DecodeFromDataBlock( db );
				m_Spots.insert( std::make_pair( property.Id, pSpot ) );
			}
			else
			{
				it->second->SetProperty( property );
				it->second->DecodeFromDataBlock( db );
			}
		}

		// TODO: maybe should put it other place
		Startup();
		Log4c::Trace(ROOT_MODULE, FormatText( "Decode business data finished [credit desc size : %d], [area size : %d], [spot size : %d].", 
			m_CreditDesc.size(), m_Areas.size(), m_Spots.size() ) );
	}

	int CBusinessManager::QueryCreditLevel( int credit )
	{
		for( CreditDescList::const_iterator it = m_CreditDesc.begin(); it != m_CreditDesc.end(); ++ it )
		{
			if( credit >= it->min && credit < it->max )
			{
				return it->level ;
			}
		}

		return CL_BLAND;
	}

	void CBusinessManager::EncodeToSave( DBWriteSet &db )
	{
		// trade spot count
		db.AddToByteArray( (long) m_Spots.size() );
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			it->second->EncodeToSave( db );
		}
	}

	void CBusinessManager::SendSaveMsg()
	{
		Log4c::Trace(ROOT_MODULE, "Send business save message." );
		CMessage msg( MSG_S2W_BUSINESS_SAVE );
		DBWriteSet db;
		msg.GetDBWriteSet( db );
		EncodeToSave( db );
		msg.Send();
	}

	void CBusinessManager::AddNPCSpotRecord( const CGUID &npc_id, int spot_id )
	{
		if( m_NpcSpotTable.find( npc_id ) != m_NpcSpotTable.end() )
		{
			Log4c::Warn(ROOT_MODULE, FormatText( "Trade spot [%d] has NPC already.", spot_id ) );
		}
		else
		{
			m_NpcSpotTable.insert( std::make_pair( npc_id, spot_id ) );
		}
	}

	int CBusinessManager::GetSpotByNPC( const CGUID &npc_id )
	{
		NPCSpotTable::const_iterator it = m_NpcSpotTable.find( npc_id );
		if( it != m_NpcSpotTable.end() )
		{
			return it->second;
		}
		else
		{
			return 0;
		}
	}

	CTradeSpot *CBusinessManager::GetTradeSpot( int id )
	{
		TradeSpotTable::iterator it = m_Spots.find( id );
		if( it == m_Spots.end() )
		{
			Log4c::Warn(ROOT_MODULE,FormatText( "Invalid trade spot id : %d", id ) );
			return NULL;
		}
		return it->second;
	}
}