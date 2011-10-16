///
/// @file CachedSellData.cpp
/// @brief Cache the sell data when player sell goods to npc.
///
#include "StdAfx.h"
#include "CachedSellData.h"

namespace FBusiness
{
	void CCachedSellData::Clear()
	{
		for( SellGoodsTableT::iterator it = m_SellDataTable.begin();
			it != m_SellDataTable.end(); ++ it )
		{
			delete it->second;
		}
		m_SellDataTable.clear();
	}

	CCachedSellData::SellGoodsListT *CCachedSellData::New( const CGUID &guid )
	{
		SellGoodsTableT::iterator it = m_SellDataTable.find( guid );
		if( it != m_SellDataTable.end() )
		{
			char strGUID[64];
			guid.tostring( strGUID );
			Log4c::Warn(ROOT_MODULE,"The player [%s] already has cached sell data.", strGUID );
			return it->second;
		}
	
		SellGoodsListT *sell_list = new SellGoodsListT();
		m_SellDataTable.insert( std::make_pair( guid, sell_list ) );
		return sell_list;
	}

	const CCachedSellData::SellGoodsListT *CCachedSellData::Get( const CGUID &guid ) const
	{
		SellGoodsTableT::const_iterator it = m_SellDataTable.find( guid );
		return it == m_SellDataTable.end() ? NULL : it->second;
	}

	void CCachedSellData::Remove( const CGUID &guid )
	{
		const SellGoodsListT *sell_list = Get( guid );
		if( sell_list != NULL )
		{
			delete sell_list;
			m_SellDataTable.erase( guid );
		}
	}
}

