///
/// @file OrderMgr.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "OrderMgr.h"
#include "../../../../Public/Common/AHBaseDef.h"

namespace AH
{
    OrderMgr::OrderMgr() : m_BuyOrders( OT_BUY ), 
        m_SellOrders( OT_SELL ), m_Timer( 1024, 1000 )
    {
        m_CheckTimer = m_SaveTimer = INVALID_TIMER;
        MarkDirty( false );
    }

    OrderMgr::~OrderMgr()
    {
        Clear();
    }

    void OrderMgr::Startup()
    {
        m_CheckTimer = m_Timer.Schedule( this, (void*) TT_CHECK, timeGetTime() + AH_CHECK_INTERVAL, AH_CHECK_INTERVAL );
        m_SaveTimer = m_Timer.Schedule( this, (void*) TT_SAVE, timeGetTime() + AH_SAVE_INTERVAL, AH_SAVE_INTERVAL );
    }

    void OrderMgr::Shutdown()
    {
        if( m_CheckTimer != INVALID_TIMER )
        {
            m_Timer.Cancel( m_CheckTimer );
        }
        if( m_SaveTimer != INVALID_TIMER )
        {
            m_Timer.Cancel( m_SaveTimer );
        }
    }

    void OrderMgr::Clear()
    {
        for( AgentOrderTableT::iterator it = m_AgentOrders.begin();
            it != m_AgentOrders.end(); ++ it )
        {
            delete it->second;
        }
        m_AgentOrders.clear();
    }

    int OrderMgr::AddAgent( const CGUID &playerID, int cnt, int price, int type )
    {
        AgentOrder *order = CheckAgentOrder( playerID );
        int ret = CanAddAgent( order );
        if( ret != RES_OK )
        {
            return ret;
        }
        int id = order->Add( cnt, price, type );
        MarkDirty();
        return id;
    }

    int OrderMgr::RemoveAgent( const CGUID &playerID, int orderID )
    {
        AgentOrder *order = GetAgentOrder( playerID );
        if( order == NULL )
        {
            return RES_INVALID_ARG;
        }
        int ret = order->Remove( orderID );
        if( CheckDelete( order ) )
        {
            m_AgentOrders.erase( playerID );
        }
        MarkDirty();
        return ret;
    }

    int OrderMgr::Buy( const CGUID &buyer, int playerMoney, int orderID, int cnt, int *operRet )
    {
        MarkDirty();
        AgentOrder *o = CheckAgentOrder( buyer );
        return m_BuyOrders.Update( orderID, playerMoney, cnt, o, operRet );
    }

    int OrderMgr::Sell( const CGUID &seller, int playerGoods, int orderID, int cnt, int *operRet )
    {
        MarkDirty();
        AgentOrder *o = CheckAgentOrder( seller );
        return m_SellOrders.Update( orderID, playerGoods, cnt, o, operRet );
    }

    int OrderMgr::Pickup( const CGUID &picker, int type )
    {
        AgentOrder *o = GetAgentOrder( picker );
        if( o == NULL )
        {
            // what a poor man, he has nothing to pickup
            return 0;
        }
        int ret = o->Pickup( type );
        if( CheckDelete( o ) )
        {
            m_AgentOrders.erase( picker );
        }
        return ret;
    }

    void OrderMgr::Run()
    {
		m_Timer.Expire( timeGetTime() );
    }

    long OrderMgr::OnTimeOut( ulong timerid, ulong curtime, const void *var )
    {
        int type = (int) var;
        if( type == TT_CHECK )
        {
            AgentOrderTableT checkOrders;
            for( AgentOrderTableT::iterator it = m_AgentOrders.begin();
                it != m_AgentOrders.end(); ++ it )
            {
                AgentOrder *o = it->second;
                if( o->CheckTimeOut() )
                {
                    checkOrders[it->first] = o;     
                }
            }
            for( AgentOrderTableT::iterator it = checkOrders.begin();
                it != checkOrders.end(); ++ it )
            {
                AgentOrder *o = it->second;
                if( CheckDelete( o ) )
                {
                    m_AgentOrders.erase( it->first );
                }
            }
        }
        else if( type == TT_SAVE )
        {
            if( m_ChangeFlag )
            {
                SaveAll();
                MarkDirty( false );
            }
        }
        return 0;
    }

    void OrderMgr::EncodeToClient( const CGUID &playerID, DBWriteSet &db, int type )
    {
        if( type == AH_REQUEST_BUY )
        {
            m_BuyOrders.EncodeTopN( AH_ORDER_COUNT, db );
        }
        else if( type == AH_REQUEST_SELL )
        {
            m_SellOrders.EncodeTopN( AH_ORDER_COUNT, db, true );
        }
        else if( type == AH_REQUEST_AGENT )
        {
            AgentOrder *order = GetAgentOrder( playerID );
            if( order == NULL )
            {
                db.AddToByteArray( (uchar) 0 );
            }
            else
            {
                db.AddToByteArray( (uchar) 1 );
                order->Encode( db );
            }
        }
    }

    AgentOrder *OrderMgr::CheckAgentOrder( const CGUID &playerID )
    {
        AgentOrder *o = GetAgentOrder( playerID );
        if( o == NULL )
        {
            o = new AgentOrder( playerID );
            m_AgentOrders.insert( std::make_pair( playerID, o ) );
        }
        return o;
    }

    AgentOrder *OrderMgr::GetAgentOrder( const CGUID &playerID )
    {
        AgentOrderTableT::iterator it = m_AgentOrders.find( playerID );
        return it == m_AgentOrders.end() ? NULL : it->second;
    }

    bool OrderMgr::CheckDelete( AgentOrder *o )
    {
        if( !o->IsValid() )
        {
            delete o;
            return true;
        }
        return false;
    }

    void OrderMgr::MarkDirty( bool flag )
    {
        m_ChangeFlag = flag;
    }

    int OrderMgr::CanAddAgent( AgentOrder *order )
    {
        if( order->Count() > AH_AGENT_MAX )
        {
            return RES_AGENT_MAX;
        }
        return RES_OK;
    }

    void OrderMgr::SetListeners( SubOrder *order )
    {
        order->m_Listeners.Add( &m_BuyOrders );
        order->m_Listeners.Add( &m_SellOrders );
    }
}
