///
/// @file ShopManager.h
/// @brief Manage the shop list.
///
#ifndef ___FBUSINESS_SHOPMANAGER_H_
#define ___FBUSINESS_SHOPMANAGER_H_

#include <map>
#include "../../../../public/GameEvent.h"
#include "Configer.h"

namespace FBusiness
{
	class CShop;

	class CShopManager : public CGameEvent
	{
	public:
		typedef std::map<long, CShop*> ShopTableT;

		/// timer type
		enum TimerType
		{
			TT_UPDATE_BUY,
		};
	public:
		CShopManager();

		~CShopManager()
		{
		}

		void Release();

		/// create and save it in the table.
		/// if the shop exists, only update it's sell goods list.
		bool CreateShop( long id, const char *npcOrigName, const SellGoodsListT &goods_list );

		/// if the timer already running, ignore it.
		bool StartupTimer();

		/// just a wrapper function for CConfiger::Decode.
		void Decode( DBReadSet &db );

		/// refresh the buy goods list of all the shops.
		void RefreshAllShops();

		/// 打开玩家购买商品界面
		bool OpenShopForBuy( long id, const CGUID &playerID, const CGUID &npcID );

		/// 打开玩家出售商品界面
		bool OpenShopForSell( long id, const CGUID &playerID, const CGUID &npcID );

		/// 玩家关闭商店（服务器主动）
		void PlayerCloseShop( CPlayer *pPlayer );

		bool HasShop( long lID ) 
		{
			return GetShop( lID ) != NULL;
		}

		CShop *GetShop( long lId );

		void OnMessage( CMessage* );

		long OnTimeOut( DWORD timerid, DWORD curtime, const void *var );
		void OnTimerCancel( DWORD timerid, const void *var );
		long Proc( eEventType type, const tagProcVar *pVar ) { return 0; }

	private:
		ShopTableT m_ShopTable;
		long m_lUpdateBuyTimer;

	};
}

#endif
