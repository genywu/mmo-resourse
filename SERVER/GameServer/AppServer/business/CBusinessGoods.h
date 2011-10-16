///
/// @file CBusinessGoods.h
/// @brief 商业系统商品模板 
///
#ifndef ___BUSINESS_GOODS_H_
#define ___BUSINESS_GOODS_H_

#include "../../../../public/BusinessGoodsDef.h"
#include "../../../../public/GUID.h"

namespace Business
{
	class CTradeSpot;

	class CBusinessGoods : public BaseMemObj
	{
	public:
		typedef stdext::hash_map<CGUID, DWORD, hash_guid_compare> BuyLimitTable;
	public:
		CBusinessGoods( CTradeSpot *pSpot );
		CBusinessGoods( const tagGoods &bp, CTradeSpot *pSpot );
		
		///
		/// 根据前段时间买卖总数量更新当前买卖价格，清零买卖总数计数。
		/// 物价变化系统参见策划文档《跑商数值公式》
		///
		void UpdatePrice();

		///
		/// 获取指定玩家对于该物品的限购额
		///
		/// 如果存在限购额记录中，则返回当前限购额记录，否则重新计算限购额，但不添加限购额记录
		///
		DWORD GetBuyLimit( const CGUID &player_id );
		
		///
		/// 获取某个玩家的最大限购额
		///
		DWORD GetMaxBuyLimit( const CGUID &player_id );

		void SetBuyLimit( const CGUID &player_id, DWORD dwBuyLimit );

		///
		/// 刷新限购额，如果记录不存在则不添加（效率考虑）
		///
		void RefreshBuyLimit( const CGUID &player_id );

		///
		/// 检查某个玩家是否有限购额记录
		///
		bool HasBuyLimitRecord( const CGUID &player_id );

		///
		/// 减少限购额，如果该玩家没有记录，添加新纪录
		///
		DWORD SubBuyLimit( const CGUID &player_id, DWORD dwSub );

		///
		/// 获取税率指数，百分制分子
		///
		void GetTaxScale( CPlayer *pPlayer, int &OrigTaxScale, int &TaxScale );

		///
		/// 编码数据库存档数据
		///
		void EncodeToSave( vector<BYTE> &buf );
		
		///
		/// 解码数据库存档数据
		///
		void DecodeFromDB( DBReadSet &db );

		void AddBuyCount( DWORD add )
		{
			// TODO : remove this invalid function later
		}

		void AddSellCount( DWORD add )
		{
			// TODO : remove this invalid function later
		}

		void SetBaseProperty( const tagGoods &bp );

		const tagGoods &GetBaseProperty() const 
		{
			return m_BaseProperty;
		}

		/// 得到该物品物编ID
		int GetIndex() const
		{
			return m_Index;
		}

		DWORD GetBuyPrice() const
		{
			return m_dwBuyPrice;
		}

		///
		/// 获取该商品在此贸易点的出售价格（玩家向贸易点出售）
		/// 如果该贸易点也出售此商品，那么商品的出售价格为购买价格50%
		///
		DWORD GetSellPrice( bool bLocal = false ) const
		{
			return bLocal ? GetBuyPrice() * 0.5f : m_dwSellPrice;
		}

		int GetPriceState() const
		{
			return m_PriceState;
		}

		int GetVolume() const
		{
			return m_Volume;
		}

		void SetVolume( int v )
		{
			m_Volume = v;
		}

	private:
		void SetPriceState( int sub );
	private:
		tagGoods m_BaseProperty;
		/// 物编中的ID
		int m_Index;
		/// 当前库存
		int m_Volume;
		/// 价格走势
		int m_PriceState;
		/// 买入价格（相对于玩家买入）
		DWORD m_dwBuyPrice;
		/// 卖出价格（相对于玩家卖出）
		DWORD m_dwSellPrice;
		/// 总被买入数量
		DWORD m_dwBuyCount;
		/// 总被卖出数量
		DWORD m_dwSellCount;
		/// 正常买入变化系数
		float m_fBuyQuo;
		/// 正常卖出变化系数
		float m_fSellQuo;
		/// 记录每个玩家对于此商品的限购额
		BuyLimitTable m_BuyLimitRecord;
		/// 最大库存量(受发展度变化)
		int m_MaxVolume;
		/// 该商品所在贸易点
		CTradeSpot *m_pFather;
	};
}
#endif