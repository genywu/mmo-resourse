///
/// @file Shop.cpp
/// @brief Implement a NPC shop for FBusiness.
///
#include "StdAfx.h"
#include "Shop.h"
#include "Configer.h"
#include "GoodsHelper.h"

namespace FBusiness
{
	CShop::CShop( long lId, const char *npcOrigName, const SellGoodsListT &goods_list ) 
		: m_lID( lId ), m_SellGoodsList( goods_list ) 
	{
		memset( m_npcOrigName, 0, sizeof( m_npcOrigName ) );
		strncpy( m_npcOrigName, npcOrigName, sizeof( m_npcOrigName ) - 1 );
	}
	CShop::CShop(void* pParam)
		: m_SellGoodsList(*(((tagParam*)pParam)->goods_list))
	{
		if(pParam)
		{
			m_lID = ((tagParam*)pParam)->id;
			memset( m_npcOrigName, 0, sizeof( m_npcOrigName ) );
			strncpy( m_npcOrigName, ((tagParam*)pParam)->npcOrigName, sizeof( m_npcOrigName ) - 1 );
		}
	}

	void CShop::SetSellList( const SellGoodsListT &goods_list )
	{
		m_SellGoodsList = goods_list;
	}

	void CShop::RefreshBuyGoodsList()
	{
		FBusiness::BuyGoodsListT buy_goods_list;
		GetInst( CConfiger ).GetRandBuyGoods( m_SellGoodsList, buy_goods_list );
		m_BuyGoodsList.clear();
		for( FBusiness::BuyGoodsListT::const_iterator it = buy_goods_list.begin();
			it != buy_goods_list.end(); ++ it )
		{
			const FBusiness::BuyGoods &buy_goods = *it;
			BuyGoods ret_buy_goods;
			ret_buy_goods.lIndex = buy_goods.lIndex;
			long quo = random( int( buy_goods.fPriceQuoMax - buy_goods.fPriceQuoMin ) * 100 )
				+ (long)buy_goods.fPriceQuoMin * 100;
			long lBasePrice = GoodsHelper::GetPrice( buy_goods.lIndex );
			if( lBasePrice != -1 )
			{
				ret_buy_goods.lPrice = long( lBasePrice * quo / 100.0f );

				// save
				m_BuyGoodsList.push_back( ret_buy_goods );
			}
			else
			{
				Log4c::Warn(ROOT_MODULE, "The goods [%d] does not have price.", buy_goods.lIndex );
			}
		}
	}

	void CShop::EncodeSellGoodsList( DBWriteSet &db )
	{
		db.AddToByteArray( (long) m_SellGoodsList.size() );
		for( SellGoodsListT::const_iterator it = m_SellGoodsList.begin();
			it != m_SellGoodsList.end(); ++ it )
		{
			db.AddToByteArray( it->lIndex );
		}
	}

	void CShop::EncodeBuyGoodsList( DBWriteSet &db )
	{
		db.AddToByteArray( (long) m_BuyGoodsList.size() );
		for( BuyGoodsListT::const_iterator it = m_BuyGoodsList.begin();
			it != m_BuyGoodsList.end(); ++ it )
		{
			db.AddToByteArray( (long) it->lIndex );
			db.AddToByteArray( (long) it->lPrice );
		}
	}

	bool CShop::IsPriceChanged( long lIndex, long lPrice )
	{
		if( GoodsHelper::GetPrice( lIndex ) == lPrice )
		{
			return false;
		}

		bool bRet = true;
		for( BuyGoodsListT::const_iterator it = m_BuyGoodsList.begin();
			it != m_BuyGoodsList.end(); ++ it )
		{
			if( it->lIndex == lIndex && lPrice == it->lPrice )
			{
				bRet = false;
				break;
			}
		}
		return bRet;
	}

	long CShop::GetBuyPrice( long lIndex )
	{
		for( BuyGoodsListT::const_iterator it = m_BuyGoodsList.begin();
			it != m_BuyGoodsList.end(); ++ it )
		{
			if( it->lIndex == lIndex )
			{
				return it->lPrice;
			}
		}
		return GoodsHelper::GetPrice( lIndex );
	}

	bool CShop::IsInSellList( long lIndex )
	{
		for( SellGoodsListT::const_iterator it = m_SellGoodsList.begin();
			it != m_SellGoodsList.end(); ++ it )
		{
			if( it->lIndex == lIndex )
			{
				return true;
			}
		}
		return false;
	}
}
