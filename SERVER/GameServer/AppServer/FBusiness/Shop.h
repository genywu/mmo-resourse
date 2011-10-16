///
/// @file Shop.h
/// @brief Implement a NPC shop for FBusiness.
///
#ifndef ___FBUSINESS_SHOP_H_
#define ___FBUSINESS_SHOP_H_

#include "../../../../public/FBusinessSvrCommon.h"
#include "CachedSellData.h"

namespace FBusiness
{
	class CShop : public BaseMemObj
	{
	public:
		struct BuyGoods
		{
			long lIndex;
			long lPrice;
		};
		typedef std::vector<BuyGoods> BuyGoodsListT;
	public:
		CShop( long lId, const char *npcOrigName, const SellGoodsListT &goods_list );
	
		void SetSellList( const SellGoodsListT &goods_list );

		void RefreshBuyGoodsList();

		/// 编码出售的商品列表
		void EncodeSellGoodsList( DBWriteSet &db );

		/// 编码收购的商品列表
		void EncodeBuyGoodsList( DBWriteSet &db );

		/// 响应玩家购买消息
		void OnPlayerBuyMsg( CMessage *pMsg, const CGUID &npcID );

		/// 响应玩家出售消息
		void OnPlayerSellMsg( CMessage *pMsg, const CGUID &npcID );

		/// 响应玩家确认出售价格变动消息
		void OnPlayerConformSellMsg( CMessage *pMsg, const CGUID &npcID );

		/// 响应玩家关闭商店消息
		void OnPlayerCloseMsg( CMessage *Msg );

		/// 玩家关闭商店(服务器主动)
		void PlayerClose( CPlayer *pPlayer );

		/// 获取该商店对应得NPC原始名
		const char *GetNpcOrigName() const
		{
			return m_npcOrigName;
		}
	private:
		/// 检查价格是否变动
		bool IsPriceChanged( long lIndex, long lPrice );

		/// 获取指定物品的收购价格
		long GetBuyPrice( long lIndex );

		/// 执行玩家出售的操作
		void DoPlayerSell( CPlayer *pPlayer, const CCachedSellData::SellGoodsListT &sell_list );

		/// 检查指定物品是否在NPC出售列表里
		bool IsInSellList( long lIndex );
	private:
		CCachedSellData m_CachedSellData;
		SellGoodsListT m_SellGoodsList; ///< 出售列表
		BuyGoodsListT m_BuyGoodsList; ///< 收购列表
		long m_lID;
		char m_npcOrigName[32];
	};
}

#endif
