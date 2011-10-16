///
/// @file CachedSellData.h
/// @brief Cache the sell data when player sell goods to npc.
///
#ifndef ___FBUSINESS_CACHEDSELLDATA_H_
#define ___FBUSINESS_CACHEDSELLDATA_H_

#include "../../../../Public/ServerPublic/Server/BaseDef/FBusinessSvrCommon.h"

namespace FBusiness
{
	class CCachedSellData
	{
	public:
		struct GoodsInfo
		{
			long index;
			long amount;
			long price;
		};
		typedef std::vector<GoodsInfo> SellGoodsListT;

		typedef std::map<CGUID, SellGoodsListT*> SellGoodsTableT;

	public:
		~CCachedSellData()
		{
			Clear();
		}

		void Clear();

		/// create a new SellGoodsListT and insert it into the table.
		SellGoodsListT *New( const CGUID &guid );	

		const SellGoodsListT *Get( const CGUID &guid ) const;

		void Remove( const CGUID &guid );
	private:
		SellGoodsTableT m_SellDataTable;
	};
}

#endif
