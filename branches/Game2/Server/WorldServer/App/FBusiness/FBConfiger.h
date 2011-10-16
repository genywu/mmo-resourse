///
/// @file Configer.h
/// @brief Read/maintain the configure for FBusiness.
///
#ifndef __FBUSINESS_CONFIGER_H_
#define __FBUSINESS_CONFIGER_H_

#include "../../../../Public/ServerPublic/Server/Basedef/FBusinessSvrCommon.h"

namespace FBusiness
{
	/// singleton
	class CConfiger
	{
	public:
		struct Shop
		{
			char npcOrigName[32];
			SellGoodsListT *SellList;
		};
		typedef std::map<long, Shop> ShopTableT;	
	public:
		CConfiger()
		{
			Reset();
		}

		~CConfiger()
		{
			Reset();
		}
		bool Load();

		bool ReLoad();

		void Reset();

		void Encode( DBWriteSet &db ) const;

		bool IsEnable() const
		{
			return m_GlobalCfg.lEnable != 0;
		}

	private:
		bool LoadGoodsList( const char *file );
		bool LoadShopList( const char *file );
	private:
		BuyGoodsListT m_BuyGoodsList;
		ShopTableT m_ShopTable;
		GlobalCfg m_GlobalCfg;
	};
}

#endif
