///
/// @file GoodsHelper.h
/// @brief To provide some helper functions to query goods properties.
///
#ifndef ___FBUSINESS_GOODSHELPER_H_
#define ___FBUSINESS_GOODSHELPER_H_

#include <string>
#include <vector>

class CGoods;
class CPlayer;

namespace FBusiness
{
	namespace GoodsHelper
	{
		/// @return -1 indicate an error
		long GetPrice( long lIndex );

		struct GoodsInfo
		{
			long lIndex;
			long lAmount;
		};

		typedef std::vector<GoodsInfo> GoodsListType;

		/// 检查玩家是否可以继续购买物品（有商品数量限制）
		bool CanPlayerBuy( CPlayer *pPlayer, const GoodsListType &goods_list );

		/// 检查物品是否是商品
		bool IsBusinessGoods( long lIndex );

		/// 获取玩家背包内的商品列表
		size_t GetPlayerBusinessGoods( CPlayer *pPlayer, std::vector<CGoods*> &goods_list );
	}
}

#endif
