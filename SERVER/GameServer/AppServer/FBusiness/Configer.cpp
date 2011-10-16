///
/// @file Configer.cpp
/// @brief Maintain the configure for FBusiness.
///
#include "stdafx.h"
#include "Configer.h"
#include "ShopManager.h"

namespace FBusiness
{
	void CConfiger::Reset()
	{
		memset( &m_GlobalCfg, 0, sizeof( m_GlobalCfg ) );
		m_BuyGoodsList.clear();
	}

	void CConfiger::Decode( CShopManager *pShopMgr, DBReadSet &db )
	{
		Reset();
		long lSize = db.GetLongFromByteArray();
		for( long i = 0; i < lSize; ++ i )
		{
			BuyGoods goods;
			db.GetBufferFromByteArray( &goods, sizeof( goods ) );
			m_BuyGoodsList.push_back( goods );
		}
		db.GetBufferFromByteArray( &m_GlobalCfg, sizeof( m_GlobalCfg ) );
		// shop list, use pShopMgr to create shop entities.
		lSize = db.GetLongFromByteArray();
		for( long i = 0; i < lSize; ++ i )
		{
			long lID = db.GetLongFromByteArray();
			char npcOrigName[32];
			db.GetStringFromByteArray( npcOrigName, sizeof( npcOrigName ) );
			long lSellCount = db.GetLongFromByteArray();
			SellGoodsListT sell_list;
			for( long j = 0; j < lSellCount; ++ j )
			{
				SellGoods goods;
				db.GetBufferFromByteArray( &goods, sizeof( goods ) );
				sell_list.push_back( goods );
			}
			// create a shop
			pShopMgr->CreateShop( lID, npcOrigName, sell_list );
		}

		// startup
		pShopMgr->StartupTimer();
	}

	size_t CConfiger::GetRandBuyGoods( const SellGoodsListT &exceptGoods, BuyGoodsListT &outGoods )
	{
		BuyGoodsListT tmpGoodsList;
		for( BuyGoodsListT::const_iterator it = m_BuyGoodsList.begin();
			it != m_BuyGoodsList.end(); ++ it )
		{
			const BuyGoods &buy_goods = *it;
			bool bExcept = false;
			for( SellGoodsListT::const_iterator sit = exceptGoods.begin();
				sit != exceptGoods.end(); ++ sit )
			{
				if( buy_goods.lIndex == sit->lIndex )
				{
					bExcept = true;
					break;
				}
			}
			if( !bExcept )
			{
				tmpGoodsList.push_back( buy_goods );
			}
		}
		for( long c = GetGlobalCfg().lBuyCount; c > 0 && tmpGoodsList.size() > 0; -- c )
		{
			long item = random( tmpGoodsList.size() );
			outGoods.push_back( tmpGoodsList[item] );
			tmpGoodsList.erase( tmpGoodsList.begin() + item );
		}
		return outGoods.size();
	}
}
