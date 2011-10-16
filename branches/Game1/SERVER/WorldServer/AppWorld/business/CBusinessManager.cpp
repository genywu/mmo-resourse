///
/// @file CBusinessManager.cpp
/// @brief 跑商系统对外接口类
///
#include "StdAfx.h"
#include "CBusinessManager.h"
#include "../DBEntity/Entity.h"
#include "../DBEntity/EntityGroup.h"
#include "../DBEntity/EntityManager.h"
#include "../../../../nets/networld/Message.h"



#define QUERY_STR( name, var, elem ) \
	do { \
	const char *_t = elem->Attribute( name ); \
	if( _t != 0 ) strcpy( var, _t ); \
	}while( 0 ) 

#define QUERY_NUM( name, var, elem, type ) \
	do { \
	double _t = 0; \
	elem->Attribute( name, &_t ) ; \
	var = static_cast<type>( _t ); \
	}while( 0 )


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
			MP_DELETE (it->second);
		}

		// trade spot
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			MP_DELETE(it->second);
		}

		m_CreditDesc.clear();
		m_Areas.clear();
		m_Spots.clear();

		BUSINESS_LOG_INFO( "BusinessManager released." );
	}

	bool CBusinessManager::Load()
	{
		const char *file = "business/business.xml";
		CRFile *prFile = rfOpen( file );
		if( prFile == NULL )
		{
			BUSINESS_LOG_WARNING( "Load busines/business.xml FAILED." );
			return false;
		}

		TiXmlDocument doc( file );
		doc.LoadData( prFile->GetData(), prFile->GetDatalen() );
		rfClose( prFile );

		TiXmlElement *pRoot = doc.RootElement();
		if( pRoot == NULL )
		{
			return false;
		}

		bool ret = true;
		m_Areas.clear();
		m_Spots.clear();
		m_CreditDesc.clear();
		TiXmlElement *pElem = pRoot->FirstChildElement( "CreditLevel" );
		ret = LoadCreditDesc( pElem ) && ret;
		pElem = pRoot->FirstChildElement( "TradeArea" );
		ret = LoadTradeArea( pElem ) && ret;

		/// load global setup
		pElem = pRoot->FirstChildElement( "System" );
		if( pElem == NULL )
		{
			BUSINESS_LOG_WARNING( "No system config element, set default." );
			m_Setup.dwUpdatePriceInterval = 300000;
			m_Setup.dwUpdateBuyLimitInterval = 90000;
			m_Setup.fEndureQuo = 0.001f;
		}
		else
		{
			QUERY_NUM( "UpdatePriceInterval", m_Setup.dwUpdatePriceInterval, pElem, DWORD );
			QUERY_NUM( "UpdateBuyLimitInterval", m_Setup.dwUpdateBuyLimitInterval, pElem, DWORD );
			QUERY_NUM( "EndureQuo", m_Setup.fEndureQuo, pElem, float );
			QUERY_NUM( "Enable", m_Setup.enable, pElem, BYTE );
		}

		BUSINESS_LOG_INFO( "Load business config file finished." );
		if( !IsEnable() )
		{
			BUSINESS_LOG_INFO( "Business system has been DISABLED!" );
		}
		return ret;
	}

	void CBusinessManager::EncodeToDataBlock( DBWriteSet &db, int GsId )
	{
		// system setup
		db.AddToByteArray( &m_Setup, sizeof( m_Setup ) );

		// credit description
		db.AddToByteArray( (long) m_CreditDesc.size() );
		for( CreditDescList::iterator it = m_CreditDesc.begin(); it != m_CreditDesc.end(); ++ it )
		{
			db.AddToByteArray( &(*it), sizeof( tagCreditDesc ) );
		}

		// trade area
		db.AddToByteArray( (long) m_Areas.size() );
		for( TradeAreaTable::iterator it = m_Areas.begin(); it != m_Areas.end(); ++ it )
		{
			it->second->EncodeToDataBlock( db );
		}

		// trade spot
		db.AddToByteArray( (long) m_Spots.size() );
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			if( it->second->GetProperty().GsId == GsId )
			{
				// add a flag to indicate whether the spot is valid for this GS
				db.AddToByteArray( (BYTE) 1 );
				it->second->EncodeToDataBlock( db );
			}
			else
			{
				db.AddToByteArray( (BYTE)0 );
			}
		}

	}

	void CBusinessManager::EncodeToEntityGroup( CEntityGroup *entity_root )
	{
		if( entity_root == NULL )
		{
			return;
		}
		entity_root->ReleaseChilds();

		// encode trade spot data
		for( TradeSpotTable::iterator it = m_Spots.begin(); it != m_Spots.end(); ++ it )
		{
			CGUID guid;
			CGUID::CreateGUID( guid );
			CEntity *entity_child = (CEntity*)GetGame()->GetEntityManager()->NewBaseEntity( "[trade_spot]", guid );

			CTradeSpot *pSpot = it->second;
			entity_child->SetLongAttr( "id", pSpot->GetProperty().Id );

			// goods list
			CTradeSpot::RawBufType &goods_list = pSpot->QueryGoodsRawBuf();
			entity_child->SetBufAttr( "goods_list", &goods_list[0], goods_list.size() );
			
			// country invest list
			CTradeSpot::RawBufType &country_invest = pSpot->QueryCountryInvestRawBuf();
			entity_child->SetBufAttr( "country_invest", &country_invest[0], country_invest.size() );
			entity_root->AddChild( entity_child );
		}
	}

	void CBusinessManager::DecodeFromEntityGroup( long ret, CEntityGroup *entity )
	{
		if( ret != S_OK || entity == NULL )
		{
			BUSINESS_LOG_WARNING( "Load data from database failed!" );
			return;
		}

		CEntityGroup *pVarGroup = NULL;
		CEntityManager::DBEntityComponentMapItr entityMapItr = 
			GetGame()->GetEntityManager()->GetBaseEntityMap().find( "[trade_spot_group]" );
		if( entityMapItr != GetGame()->GetEntityManager()->GetBaseEntityMap().end() )
		{
			typedef std::map<CGUID, CBaseEntity*> EntityGroupTable;
			CEntityGroup *pVarGroup = entityMapItr->second.begin()->second;
			if( pVarGroup == NULL )
			{
				BUSINESS_LOG_WARNING( "pVarGroup == NULL, load data from db failed" );
				return;
			}

			EntityGroupTable &entity_table = pVarGroup->GetEntityGroupMap();
			// 遍历每一组属性(数据库表的一行)
			for( EntityGroupTable::iterator it = entity_table.begin(); it != entity_table.end(); ++ it )
			{
				pVarGroup = (CEntityGroup*)it->second;
				// trade spot id
				long id = pVarGroup->GetLongAttr( "id" );

				// find the specified trade spot
				CTradeSpot *pSpot = GetTradeSpot( id );
				if( pSpot == NULL )
				{
					BUSINESS_LOG_WARNING( FMT_STR( "Unknown trade spot [%d]", id  ) );
				}
				else
				{
					// goods list buffer
					string strBuf = "goods_list";
					long lAttrBufSize = pVarGroup->GetBufSize(strBuf);
					if( lAttrBufSize > 0 )
					{
						CTradeSpot::RawBufType &raw_buf = pSpot->QueryGoodsRawBuf();
						raw_buf.resize( lAttrBufSize );
						pVarGroup->GetBufAttr(strBuf, &raw_buf[0], lAttrBufSize );
					}
					

					// country invest buffer
					strBuf = "country_invest";
					lAttrBufSize = pVarGroup->GetBufSize(strBuf);
					if( lAttrBufSize > 0 )
					{
						CTradeSpot::RawBufType &raw_buf = pSpot->QueryCountryInvestRawBuf();
						raw_buf.resize( lAttrBufSize );
						pVarGroup->GetBufAttr(strBuf, &raw_buf[0], lAttrBufSize );
					}

					BUSINESS_LOG_INFO( FMT_STR( "Load trade spot [%d] ok", id ) );
				}
			}// for
		}// if

		m_bDBLoaded = true;
		// send setup to all
		if( IsEnable() )
		{
			SendSetupToAll();
		}
	}

	void CBusinessManager::SendSetupToGS( long lGSIndex, long lSocketID )
	{
		if( IsEnable() && IsDBLoaded() )
		{
			CMessage msg( MSG_W2S_SERVER_SENDSETUP );
			msg.Add( (long)SI_TRADEAREASETUP );
			DBWriteSet db;
			msg.GetDBWriteSet( db );			
			EncodeToDataBlock( db, lGSIndex );	
			msg.SendToSocket( lSocketID );
		}
		else
		{
			BUSINESS_LOG_WARNING( FMT_STR( "DB data did not loaded when GS [%d] request setup", lGSIndex ) ); 
		}
	}

	void CBusinessManager::SendSetupToAll()
	{
		typedef std::map<DWORD, CGame::tagGameServer> ServerListType;
		ServerListType *server_list = GetGame()->GetGameServerList();
		for( ServerListType::iterator it = server_list->begin(); it != server_list->end(); ++ it )
		{
			if( it->second.bConnected )
			{
				CMessage msg( MSG_W2S_SERVER_SENDSETUP );
				msg.Add( (long)SI_TRADEAREASETUP );
				DBWriteSet db;
				msg.GetDBWriteSet( db );			
				EncodeToDataBlock( db, it->second.dwIndex );	
				msg.SendGSID( it->first );
			}
		}
	}

	void CBusinessManager::DecodeFromGS( DBReadSet &db )
	{
		// spot count
		long spot_count = db.GetLongFromByteArray();
		for( long i = 0; i < spot_count; ++ i )
		{
			// trade spot id
			long id = db.GetLongFromByteArray();
			// find the trade spot object first
			CTradeSpot *pSpot = GetTradeSpot( id );
			if( pSpot == NULL )
			{
				BUSINESS_LOG_WARNING( FMT_STR( "Unknown trade spot [%d]", id ) );
			}
			else
			{
				// goods list
				long buf_size = db.GetLongFromByteArray();
				CTradeSpot::RawBufType &goods_list = pSpot->QueryGoodsRawBuf();
				if( buf_size > 0 )
				{
					goods_list.resize( buf_size + 1 );
					goods_list[0] = 1;
					db.GetBufferFromByteArray( &goods_list[1], buf_size );
				}
				else
				{
					goods_list.resize( 1 );
					goods_list[0] = 0;
				}
				// country invest list
				buf_size = db.GetLongFromByteArray();
				CTradeSpot::RawBufType &country_invest = pSpot->QueryCountryInvestRawBuf();
				if( buf_size > 0 )
				{
					country_invest.resize( buf_size + 1 );
					country_invest[0] = 1;
					db.GetBufferFromByteArray( &country_invest[1], buf_size );
				}
				else
				{
					country_invest.resize( 1 );
					country_invest[0] = 0;
				}
				BUSINESS_LOG_INFO( FMT_STR( "Decode trade spot [%d] ok.", id ) );
			}
		}
	}

	bool CBusinessManager::LoadCreditDesc( TiXmlElement *elm )
	{
		if( elm == NULL )
		{
			return false;
		}
	
		elm = elm->FirstChildElement();
		tagCreditDesc cd;
		while( elm != NULL )
		{
			QUERY_NUM( "level", cd.level, elm, int );
			QUERY_NUM( "min", cd.min, elm, int );
			QUERY_NUM( "max", cd.max, elm, int );
			QUERY_STR( "desc", cd.desc, elm );
			elm = elm->NextSiblingElement();

			m_CreditDesc.push_back( cd );
		}
		return true;
	}

	bool CBusinessManager::LoadTradeArea( TiXmlElement *elm )
	{
		if( elm == NULL )
		{
			return false;
		}

		elm = elm->FirstChildElement();
		tagTradeAreaProperty p;
		bool ret = true;
		while( elm != NULL )
		{
			QUERY_NUM( "id", p.Id, elm, int );
			QUERY_STR( "name", p.name, elm );

			ret = LoadTradeSpot( elm, p.Id ) && ret;
			m_Areas.insert( std::make_pair( p.Id, MP_NEW CTradeArea( p ) ) );

			elm = elm->NextSiblingElement();
		}

		return ret;
	}

	bool CBusinessManager::LoadTradeSpot( TiXmlElement *elm, int AreaId )
	{
		if( elm == NULL )
		{
			return false;
		}

		elm = elm->FirstChildElement();
		tagTradeSpotProperty p;
		bool ret = true;
		char file[512];
		CTradeSpot *pSpot = 0;
		p.areaId = AreaId;

		while( elm != NULL )
		{
			QUERY_NUM( "id", p.Id, elm, int );
			QUERY_NUM( "GameServerId", p.GsId, elm, int );
			QUERY_NUM( "RegionResID", p.RegionResID, elm, int );
			QUERY_STR( "file", file, elm );
			QUERY_NUM( "fCategoryVolumeQuo", p.fCategoryVolumeQuo, elm, float );
			QUERY_NUM( "fAllVolumeQuo", p.fAllVolumeQuo, elm, float );
			pSpot = MP_NEW CTradeSpot( p );
			m_Spots.insert( std::make_pair( p.Id, pSpot ) );
			ret = pSpot->LoadGoodsTemplate( file ) && ret;

			elm = elm->NextSiblingElement();
		}

		return ret;
	}
}