///
/// @file GoodsHelper.cpp
/// @brief To provide some helper functions to query goods properties.
///
#include "stdafx.h"
#include "GoodsHelper.h"
#include "../Goods/CGoodsFactory.h"
#include "../Goods/CGoodsBaseProperties.h"
#include "../Player.h"
#include "Configer.h"

namespace FBusiness
{
	namespace GoodsHelper
	{
		long GetPrice( long lIndex )
		{
			CGoodsBaseProperties *pProperty = CGoodsFactory::QueryGoodsBaseProperties( lIndex );
			if( pProperty == NULL )
			{
				return -1;
			}
			
			if( !pProperty->IsHasAddonPropertie( GAP_FBUSINESS_PRICE ) )
			{
				return -1;
			}
			DWORD dwRet = pProperty->GetAddonPropertyValue( GAP_FBUSINESS_PRICE, 1 );
			return (long) dwRet;
		}

		bool CanPlayerBuy( CPlayer *pPlayer, const GoodsListType &goods_list )
		{
			if( goods_list.size() > GetInst( CConfiger ).GetGlobalCfg().lPlayerBuyCount )
			{
				return false;
			}
			std::vector<CGoods*> player_goods_list_t;
			GetPlayerBusinessGoods( pPlayer, player_goods_list_t );
			if( player_goods_list_t.size() == 0 )
			{
				return true;
			}

			std::vector<CGoods*> player_goods_list;
			// 整理拆分物品
			for( std::vector<CGoods*>::iterator it = player_goods_list_t.begin();
				it != player_goods_list_t.end(); ++ it )
			{
				bool bExist = false;
				for( std::vector<CGoods*>::iterator git = player_goods_list.begin();
					git != player_goods_list.end(); ++ git )
				{
					if( (*git)->GetBasePropertiesIndex() == (*it)->GetBasePropertiesIndex() )
					{
						bExist = true;
						break;
					}
				}
				if( !bExist )
				{
					player_goods_list.push_back( *it );
				}
			}

			long lBuyCount = (long)goods_list.size() + (long)player_goods_list.size();
			// 欲购买列表内物品没有重复
			for( GoodsListType::const_iterator it = goods_list.begin();
				it != goods_list.end() && player_goods_list.size() > 0; ++ it )
			{
				const GoodsInfo &goodsInfo = *it;
				for( std::vector<CGoods*>::iterator git = player_goods_list.begin();
					git != player_goods_list.end();  ++ git )
				{
					CGoods *pGoods = *git;
					if( goodsInfo.lIndex == pGoods->GetBasePropertiesIndex() )
					{
						// 种类相同
						lBuyCount --;
						player_goods_list.erase( git );	
						break;
					}
				}
			}

			return lBuyCount <= GetInst( CConfiger ).GetGlobalCfg().lPlayerBuyCount;
		}

		bool IsBusinessGoods( long lIndex )
		{
			CGoodsBaseProperties *pProperty = CGoodsFactory::QueryGoodsBaseProperties( lIndex );
			if( pProperty == NULL )
			{
				return false;
			}
			
			return pProperty->IsHasAddonPropertie( GAP_FBUSINESS_PRICE )  == TRUE;
		}

		typedef hash_map<CGUID,CGoods*,hash_guid_compare> GoodsTableT;

		static size_t GetPackBusinessGoods( const GoodsTableT *goods_table, std::vector<CGoods*> &goods_list )
		{
			for( GoodsTableT::const_iterator it = goods_table->begin();
				it != goods_table->end(); ++ it )
			{
				if( IsBusinessGoods( it->second->GetBasePropertiesIndex() ) )
				{
					goods_list.push_back( it->second );
				}
			}
			return goods_list.size();
		}

		size_t GetPlayerBusinessGoods( CPlayer *pPlayer, std::vector<CGoods*> &goods_list )
		{
			GoodsTableT *goods_table = pPlayer->GetOriginPackGoodsMap();
			// main pack
			GetPackBusinessGoods( goods_table, goods_list );
			// sub pack
			for( long i = CSubpackContainer::EC_PACK1; i <= CSubpackContainer::EC_PACK5; ++ i )
			{
				goods_table = pPlayer->GetSubPackGoodsMap( i );
				if( goods_table != NULL )
				{
					GetPackBusinessGoods( goods_table, goods_list );
				}
			}
			return goods_list.size();
		}
	}
}
