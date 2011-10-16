///
/// @file Configer.h
/// @brief Maintain the configure for FBusiness.
///
#ifndef ___FBUSINESS_CONFIGER_H_
#define ___FBUSINESS_CONFIGER_H_

#include "../../../../Public/ServerPublic/Server/BaseDef/FBusinessSvrCommon.h"

namespace FBusiness
{
	class CShopManager;

	class CConfiger
	{
	public:
		CConfiger()
		{
			Reset();
		}

		~CConfiger()
		{
		}

		void Reset();

		void Decode( CShopManager *pShopMgr, DBReadSet &db );

		size_t GetRandBuyGoods( const SellGoodsListT &exceptGoods, BuyGoodsListT &outGoods );

		const GlobalCfg &GetGlobalCfg() const
		{
			return m_GlobalCfg;
		}

		bool IsEnable() const
		{
			return m_GlobalCfg.lEnable != 0;
		}
	private:
		GlobalCfg m_GlobalCfg;
		BuyGoodsListT m_BuyGoodsList;
	};
}

#endif
