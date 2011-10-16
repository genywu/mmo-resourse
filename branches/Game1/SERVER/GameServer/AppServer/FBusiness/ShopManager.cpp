///
/// @file ShopManager.cpp
/// @brief Manage the shop list.
///
#include "stdafx.h"
#include "ShopManager.h"
#include "Shop.h"
#include "Configer.h"
#include "../../../../public/FBusinessCommon.h"

namespace FBusiness
{
	enum { INVALID_TIMER = -1 };

	CShopManager::CShopManager()
	{
		m_lUpdateBuyTimer = INVALID_TIMER;
	}

	void CShopManager::Release()
	{
		if( m_lUpdateBuyTimer != INVALID_TIMER )
		{
			GameManager::GetInstance()->Cancel( m_lUpdateBuyTimer, 0 );
			m_lUpdateBuyTimer = INVALID_TIMER;
		}

		for( ShopTableT::iterator it = m_ShopTable.begin();
			it != m_ShopTable.end(); ++ it )
		{
			MP_DELETE(it->second);
		}
		m_ShopTable.clear();
	}

	bool CShopManager::StartupTimer()
	{
		if( m_lUpdateBuyTimer != INVALID_TIMER )
		{
			return false;
		}
		unsigned long uInterval = GetInst( CConfiger ).GetGlobalCfg().uGoodsUpdateInter * 1000;
		tagTimerVar *pTimerVar = CBaseObject::CreateTimerVar(38);
		pTimerVar->TimerType = (eChildObjType)TT_UPDATE_BUY;
		// refresh immediately.
		m_lUpdateBuyTimer = GameManager::GetInstance()->Schedule( this, pTimerVar, timeGetTime(), 
			uInterval );
			
		return true;
	}

	bool CShopManager::CreateShop( long id, const char *npcOrigName, const SellGoodsListT &goods_list )
	{
		CShop *pShop = GetShop( id );
		if( pShop == NULL )
		{
			pShop = MP_NEW CShop(id, npcOrigName, goods_list);
			m_ShopTable.insert( std::make_pair( id, pShop ) );
			return true;
		}
		else
		{
			pShop->SetSellList( goods_list );
			return false;
		}
	}

	void CShopManager::Decode( DBReadSet &db )
	{
		GetInst( CConfiger ).Decode( this, db );
	}

	void CShopManager::RefreshAllShops()
	{
		for( ShopTableT::iterator it = m_ShopTable.begin();
			it != m_ShopTable.end(); ++ it )
		{
			it->second->RefreshBuyGoodsList();
		}
	}

	bool CShopManager::OpenShopForBuy( long id, const CGUID &playerID, const CGUID &npcID )
	{
		CShop *pShop = GetShop( id );
		if( pShop == NULL )
		{
			return false;
		}
		
		CMessage msg( MSG_S2C_FBUSINESS_OPER );
		msg.Add( (BYTE) S2C_SELL_PAGE );
		msg.Add( npcID );
		msg.Add( id );
		DBWriteSet db;
		msg.GetDBWriteSet( db );
		pShop->EncodeSellGoodsList( db );
		msg.SendToPlayer( playerID, false );
		return true;
	}

	bool CShopManager::OpenShopForSell( long id, const CGUID &playerID, const CGUID &npcID )
	{
		CShop *pShop = GetShop( id );
		if( pShop == NULL )
		{
			return false;
		}
		
		CMessage msg( MSG_S2C_FBUSINESS_OPER );
		msg.Add( (BYTE) S2C_BUY_PAGE );
		msg.Add( npcID );
		msg.Add( id );
		DBWriteSet db;
		msg.GetDBWriteSet( db );
		pShop->EncodeBuyGoodsList( db );
		msg.SendToPlayer( playerID, false );
		return true;
	}

	long CShopManager::OnTimeOut( DWORD timerid, DWORD curtime, const void *var )
	{
		tagTimerVar *pTimerVar = (tagTimerVar*) var;
		if( pTimerVar->TimerType == TT_UPDATE_BUY )
		{
			RefreshAllShops();
		}
		return 0;
	}

	void CShopManager::OnTimerCancel( DWORD timerid, const void *var )
	{
		if( var != NULL )
		{
			tagTimerVar *pTimervar = (tagTimerVar*)var;
			if( pTimervar->pvar != NULL )
			{
				// TODO: free the pvar memory.
			}
			CBaseObject::ReleaseTimerVar(&pTimervar);
		}
	}

	CShop *CShopManager::GetShop( long lID )
	{
		ShopTableT::iterator it = m_ShopTable.find( lID );
		if( it == m_ShopTable.end() )
		{
			return NULL;
		}
		return it->second;
	}

	void CShopManager::PlayerCloseShop( CPlayer *pPlayer )
	{
		// 鉴于之前已确定的结构，只能如此。
		for( ShopTableT::iterator it = m_ShopTable.begin(); 
			it != m_ShopTable.end(); ++ it )
		{
			it->second->PlayerClose( pPlayer );
		}
	}
}
