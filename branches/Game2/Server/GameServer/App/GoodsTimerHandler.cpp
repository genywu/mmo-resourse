///
/// @file GoodsTimerHandler.cpp
/// @author Kevin Lynx
/// @brief Implement a simple timer wrapper for player's goods.
///
#include "StdAfx.h"
#include "Player.h"
#include "GoodsTimerHandler.h"
#include "../Server/GameManager.h"

long GoodsTimerHandler::Register( CPlayer *pPlayer, const CGUID &goodsID, 
								 long lTrigerTime, long lInterval, void *arg )
{
	if( IsUnique() )
	{
		UnRegister( pPlayer, goodsID );
	}
	GoodsTimerTableT::iterator it = m_GoodsTimers.find( TimerKeyHandle( TimerKey( lTrigerTime ) ) );
	if( it == m_GoodsTimers.end() )
	{
		// new 
		long lTimerID = GameManager::GetInstance()->Schedule( pPlayer, arg, lTrigerTime + timeGetTime(), lInterval );
		GoodsListT *pGoodsList = new GoodsListT();
		pGoodsList->push_back( goodsID );
		m_GoodsTimers.insert( GoodsTimerTableT::value_type( TimerKeyHandle( TimerKey( lTrigerTime ), lTimerID ), 
			pGoodsList ) );
		return lTimerID;
	}
	else
	{
		// add
		GoodsListT *pGoodsList = it->second;
		// be sure it's unique.
		GoodsListT::const_iterator git = std::find( pGoodsList->begin(), pGoodsList->end(), goodsID );
		if( git == pGoodsList->end() )
		{
			pGoodsList->push_back( goodsID );
		}
		return 0;
	}
}

void GoodsTimerHandler::UnRegister( CPlayer *pPlayer, const CGUID &goodsID )
{
	for( GoodsTimerTableT::iterator it = m_GoodsTimers.begin();
		it != m_GoodsTimers.end(); ++ it )
	{
		GoodsListT *pGoodsList = it->second;
		GoodsListT::iterator git = std::find( pGoodsList->begin(), pGoodsList->end(), goodsID );
		if( git != pGoodsList->end() )
		{
			pGoodsList->erase( git );
			if( pGoodsList->size() == 0 )
			{
				// unregister the timer.
				GameManager::GetInstance()->Cancel( it->first.lTimerID );
				delete pGoodsList;
				m_GoodsTimers.erase( it );
			}
			// be sure the goods is unique in the table.
			break;
		}
	}
}

void GoodsTimerHandler::UnRegisterAll( CPlayer *pPlayer )
{
	for( GoodsTimerTableT::iterator it = m_GoodsTimers.begin();
		it != m_GoodsTimers.end(); ++ it )
	{
		GameManager::GetInstance()->Cancel( it->first.lTimerID );
		delete it->second;
	}
	m_GoodsTimers.clear();
}

void GoodsTimerHandler::OnTimeOut( CPlayer *pPlayer, const TimerKey &key, HandleFuncT callback )
{
	GoodsTimerTableT::iterator it = m_GoodsTimers.find( TimerKeyHandle( key ) );
	if( it != m_GoodsTimers.end() )
	{
		GoodsListT *pGoodsList = it->second;
		for( GoodsListT::const_iterator git = pGoodsList->begin();
			git != pGoodsList->end(); ++ git )
		{
			( pPlayer->*callback )( *git );
		}
	}
}


