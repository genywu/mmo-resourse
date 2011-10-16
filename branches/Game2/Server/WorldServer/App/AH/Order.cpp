///
/// @file Order.cpp
/// @author Kevin Lynx
///
#include "stdafx.h"
#include "Order.h"
#include "OrderMgr.h"

#define CAST_TIME( t ) ( assert( sizeof( time_t ) == sizeof( uint ) ), (uint) t )

namespace AH
{
    SubOrder::SubOrder( int id, int cnt, int price, int type )
    {
        m_ID = id;
        m_Raw.cnt = cnt;
        m_Raw.price = price;
        m_Raw.type = type;
        m_Raw.gentime = CAST_TIME( time( NULL ) );
    }

    bool SubOrder::IsEqual( const SubOrder *other ) const
    {
        return m_Raw.price == other->m_Raw.price && 
            m_Raw.type == other->m_Raw.type;
    }

    bool SubOrder::IsTimeOut() const
    {
        time_t now = time( NULL );
        return CAST_TIME( now ) >= m_Raw.gentime + AH_TIMEOUT;
    }

    void SubOrder::Add()
    {
        TRAVERSE_LISTENER( m_Listeners, listener, listener->OnAdd( this ) );
    }

    void SubOrder::Remove()
    {
        TRAVERSE_LISTENER( m_Listeners, listener, listener->OnRemove( this ) );
    }

    void SubOrder::Update( int subCnt, AgentOrder *order )
    {
        bool del = false;
        TRAVERSE_LISTENER( m_Listeners, listener, ( del = listener->OnUpdate( this, subCnt, order ) || del ) );
        if( del )
        {
            Delete();
        }
    }

    void SubOrder::Delete()
    {
        TRAVERSE_LISTENER( m_Listeners, listener, if( listener->OnDelete( this ) ) break );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    AgentOrder::AgentOrder( const CGUID &owner ) : m_IDGen( RES_END ), m_Owner( owner )
    {
        memset( &m_Ret, 0, sizeof( m_Ret ) );
    }

    AgentOrder::~AgentOrder()
    {
        Clear();
    }

    void AgentOrder::Clear()
    {
        for( OrderTableT::iterator it = m_Orders.begin(); it != m_Orders.end(); ++ it )
        {
            delete it->second;
        }
        m_Orders.clear();
        memset( &m_Ret, 0, sizeof( m_Ret ) );
    }

    void AgentOrder::Encode( Buffer &buf )
    {
        _AddToByteArray( &buf, (long) m_Orders.size() );
        for( OrderTableT::const_iterator it = m_Orders.begin();
            it != m_Orders.end(); ++ it )
        {
            const SubOrder *subOrder = it->second;
            _AddToByteArray( &buf, (void*) &subOrder->m_Raw, sizeof( subOrder->m_Raw ) );
        }
        _AddToByteArray( &buf, &m_Ret, sizeof( m_Ret ) );
    }

    void AgentOrder::Encode( DBWriteSet &db )
    {
        uint now = CAST_TIME( time( NULL ) );
        uint maxTime = AH_TIMEOUT;
        db.AddToByteArray( (long) m_Orders.size() );
        for( OrderTableT::const_iterator it = m_Orders.begin();
            it != m_Orders.end(); ++ it )
        {
            const SubOrder *subOrder = it->second;
            db.AddToByteArray( (long) it->first );

            SubOrderRaw raw = subOrder->m_Raw;
            uint pastTime = now >= raw.gentime ? now - raw.gentime : 0;
            raw.gentime = maxTime >= pastTime ? maxTime - pastTime : 0;
            db.AddToByteArray( (void*) &raw, sizeof( raw ) );
        }
        db.AddToByteArray( &m_Ret, sizeof( m_Ret ) );
    }

#define BUF_ADDR( buf ) (uchar*)(&buf[0])

    void AgentOrder::Decode( const Buffer &buf )
    {
        Clear();
        long pos = 0;
        long size = _GetLongFromByteArray( BUF_ADDR( buf ), pos );
        for( long i = 0; i < size; ++ i )
        {
            SubOrderRaw raw;
            _GetBufferFromByteArray( BUF_ADDR( buf ), pos, &raw, sizeof( raw ) );
            // triger OnAdd event
            Add( raw.cnt, raw.price, raw.type );
        }
        _GetBufferFromByteArray( BUF_ADDR( buf ), pos, &m_Ret, sizeof( m_Ret ) );
    }

    int AgentOrder::Pickup( int type )
    {
        int ret = 0;
        if( type == PT_GOODS )
        {
            ret = m_Ret.conch;
            m_Ret.conch = 0;
            
            AHLOGS( m_Owner, AHOP_PICKUP_GOODS, ret, AH_NULL_VAL );
        }
        else if( type == PT_COIN )
        {
            ret = m_Ret.money;
            m_Ret.money = 0;

            AHLOGS( m_Owner, AHOP_PICKUP_MONEY, ret, AH_NULL_VAL );
        }
        return ret;
    }

    int AgentOrder::Add( int cnt, int price, int type )
    {
        assert( cnt > 0 && price > 0 && ( type == OT_SELL || type == OT_BUY ) );
        int id = m_IDGen.Gen();
        SubOrder *order = new SubOrder( id, cnt, price, type );
        // listeners 
        order->m_Listeners.Add( this );
        GetInst( OrderMgr ).SetListeners( order );

        m_Orders.insert( std::make_pair( id, order ) );
        order->Add();
        return id;
    }

    int AgentOrder::Remove( int id )
    {
        OrderTableT::iterator it = m_Orders.find( id );
        if( it != m_Orders.end() )
        {
            SubOrder *order = it->second;
            m_Orders.erase( it );
            OnRemoveOrder( order );
            Remove( order );
            return RES_OK;
        }
        return RES_INVALID_ORDER;
    }

    bool AgentOrder::CheckTimeOut()
    {
        bool changed = false;
        for( OrderTableT::iterator it = m_Orders.begin();
            it != m_Orders.end(); )
        {
            SubOrder *order = it->second;
            if( order->IsTimeOut() )
            {
                it = m_Orders.erase( it );
                Remove( order );
                changed = true;
            }
            else
            {
                ++it;
            }
        } 
        return changed;
    }

    int AgentOrder::Count() const
    {
        return (int) m_Orders.size();
    }

    bool AgentOrder::IsValid() const
    {
        return Count() > 0 || m_Ret.money > 0 || m_Ret.conch > 0;
    }

    bool AgentOrder::OnUpdate( SubOrder *order, int subCnt, AgentOrder *aorder )
    {
        int t = order->m_Raw.type;
        assert( t == OT_BUY || t == OT_SELL );
        OrderRet *ret = aorder->GetRet();
        if( t == OT_BUY )
        {
            // increase the goods
            m_Ret.conch += subCnt * AH_GOODS_CNT;
            // increase money for the seller
            ret->money += subCnt * order->m_Raw.price;

            AHLOGS( m_Owner, AHOP_ABUY_RET, subCnt, order->m_Raw.price );
            AHLOGS( aorder->m_Owner, AHOP_SELL, subCnt, order->m_Raw.price );
        }
        else if( t == OT_SELL )
        {
            // increase the money
            m_Ret.money += subCnt * order->m_Raw.price;
            // increase goods for the buyer
            ret->conch += subCnt * AH_GOODS_CNT;

            AHLOGS( m_Owner, AHOP_ASELL_RET, subCnt, order->m_Raw.price );
            AHLOGS( aorder->m_Owner, AHOP_BUY, subCnt, order->m_Raw.price );
        }

        if( order->m_Raw.cnt <= 0 )
        {
            return true;
        }
        return false;
    }

    void AgentOrder::OnAdd( SubOrder *order )
    {
        if( order->m_Raw.type == OT_SELL )
        {
            AHLOGS( m_Owner, AHOP_ADD_SELL, order->m_Raw.cnt, order->m_Raw.price );
        }
        else
        {
            AHLOGS( m_Owner, AHOP_ADD_BUY, order->m_Raw.cnt, order->m_Raw.price );
        }
    }

    void AgentOrder::OnRemove( SubOrder *order )
    {
        if( order->m_Raw.type == OT_SELL )
        {
            AHLOGS( m_Owner, AHOP_REMOVE_SELL, order->m_Raw.cnt, order->m_Raw.price );
        }
        else
        {
            AHLOGS( m_Owner, AHOP_REMOVE_BUY, order->m_Raw.cnt, order->m_Raw.price );
        }
    }

    bool AgentOrder::OnDelete( SubOrder *order )
    {
        m_Orders.erase( order->m_ID );
        Remove( order );
        return true;
    }

    int AgentOrder::GetRetVal( int type )
    {
        if( type == PT_COIN )
        {
            return m_Ret.money;
        }
        else if( type == PT_GOODS )
        {
            return m_Ret.conch;
        }
        return 0;
    }

    void AgentOrder::Remove( SubOrder *order )
    {
        order->Remove();
        delete order;
    }

    void AgentOrder::OnRemoveOrder( SubOrder *order )
    {
        int t = order->m_Raw.type;
        assert( t == OT_BUY || t == OT_SELL );
        if( t == OT_BUY )
        {
            m_Ret.money += order->m_Raw.price * order->m_Raw.cnt;
        }
        else if( t == OT_SELL )
        {
            m_Ret.conch += order->m_Raw.cnt * AH_GOODS_CNT;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    Order::Order( int id )
    {
        m_ID = id;
    }

    void Order::Add( SubOrder *order )
    {
        assert( order->m_Raw.cnt > 0 );
        m_Orders.insert( std::make_pair( order->m_Raw.gentime, order ) );
    }

    void Order::Remove( SubOrder *order )
    {
        for( OrderTableT::iterator it = m_Orders.begin();
            it != m_Orders.end(); ++it )
        {
            if( it->second == order )
            {
                m_Orders.erase( it );
                return ;
            }
        }
    }

    int Order::Update( int subCnt, AgentOrder *aorder )
    {
        if( subCnt > Count() )
        {
            return RES_INVALID_ORDER_CNT;
        }
        typedef std::map<int, SubOrder*> UpdateTableT;
        UpdateTableT updateOrders;

        for( OrderTableT::iterator it = m_Orders.begin();
            subCnt > 0 && it != m_Orders.end();  ++it )
        {
            SubOrder *order = it->second;
            int cnt = order->m_Raw.cnt;
            order->m_Raw.cnt -= subCnt;
            updateOrders.insert( std::make_pair( subCnt, order ) );
            subCnt -= cnt;
        }
        // After this loop, Order(this) may be deleted, so do NOT
        // add any codes operate on this->data below.
        for( UpdateTableT::iterator it = updateOrders.begin();
            it != updateOrders.end(); ++ it )
        {
            SubOrder *order = it->second;
            int subCnt = it->first;
            order->Update( subCnt, aorder );
        }
        return RES_OK;
    }

    int Order::Price() const
    {
        if( SubOrderCnt() == 0 )
        {
            return -1;
        }
        return m_Orders.begin()->second->m_Raw.price;
    }

    int Order::Count() const
    {
        int ret = 0;
        for( OrderTableT::const_iterator it = m_Orders.begin();
            it != m_Orders.end(); ++ it )
        {
            ret += it->second->m_Raw.cnt;
        }
        return ret;
    }

    int Order::SubOrderCnt() const
    {
        return (int) m_Orders.size();
    }

    bool Order::IsEqual( const SubOrder *order ) const
    {
        if( SubOrderCnt() == 0 )
        {
            return false;
        }
        return m_Orders.begin()->second->IsEqual( order );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    OrderList::OrderList( int type ) : m_IDGen( RES_END )
    {
        m_Type = type;
    }

    OrderList::~OrderList()
    {
        Clear();
    }

    void OrderList::Clear()
    {
        for( OrderTableT::iterator it = m_Orders.begin();
            it != m_Orders.end(); ++ it )
        {
            delete it->second;
        }
        m_Orders.clear();
        m_PriceTable.clear();
        m_Sub2MainTable.clear();
    }

    int OrderList::Update( int id, int maxCnt, int subCnt, AgentOrder *aorder, int *operRet )
    {
        Order *order = GetOrder( id );
        if( order == NULL )
        {
            return RES_INVALID_ORDER;
        }
        assert( m_Type == OT_BUY || m_Type == OT_SELL );
        if( m_Type == OT_BUY )
        {
            if( subCnt * order->Price() > maxCnt )
            {
                return RES_NOT_ENOUGH_MONEY;
            }
            *operRet = subCnt * order->Price();
        }
        else if( m_Type == OT_SELL )
        {
            if( subCnt * AH_GOODS_CNT > maxCnt )
            {
                return RES_NOT_ENOUGH_GOODS;
            }
            *operRet = subCnt * AH_GOODS_CNT;
        }
        return order->Update( subCnt, aorder );
    }

    void OrderList::OnAdd( SubOrder *order )
    {
        // the 2 types have different meaning
        if( order->m_Raw.type == m_Type )
        {
            return ;
        }
        for( OrderTableT::iterator it = m_Orders.begin();
            it != m_Orders.end(); ++ it )
        {
            Order *o = it->second;
            if( o->IsEqual( order ) )
            {
                o->Add( order );
                // build the relation map
                MapSub2Main( order, o );
                return ;
            }
        }
        // create a new Order
        NewOrder( order );
    }

    void OrderList::OnRemove( SubOrder *order )
    {
        Order *o = GetMainFromSub( order );
        if( o == NULL )
        {
            return;
        }
        int price = o->Price();
        o->Remove( order );
        UnmapSub2Main( order, o );
        if( o->SubOrderCnt() == 0 )
        {
            m_Orders.erase( o->GetID() );
            RemoveOrder( o, price );
        }
    }

    void OrderList::EncodeTopN( int N, DBWriteSet &db, bool reverse )
    {
        N = N > (int)m_Orders.size() ? (int)m_Orders.size() : N;
        db.AddToByteArray( (long) N );
        if( reverse )
        {
            for( PriceTableT::reverse_iterator it = m_PriceTable.rbegin(); 
                it != m_PriceTable.rend(); ++ it )
            {
                EncodeBaseRaw( db, it->second );
            }
        }
        else
        {
            for( PriceTableT::iterator it = m_PriceTable.begin();
                it != m_PriceTable.end(); ++ it )
            {
                EncodeBaseRaw( db, it->second );
            }
        }
    }

    void OrderList::EncodeBaseRaw( DBWriteSet &db, Order *order )
    {
        OrderRaw raw;
        raw.id = order->GetID();
        raw.cnt = order->Count();
        raw.price = order->Price();
        db.AddToByteArray( &raw, sizeof( raw ) );
    }

    void OrderList::NewOrder( SubOrder *order )
    {
        int id = m_IDGen.Gen();
        Order *o = new Order( id );
        o->Add( order );
        m_Orders.insert( std::make_pair( id, o ) );
        MapSub2Main( order, o );
        MapPrice2Order( o );
    }

    void OrderList::RemoveOrder( Order *o, int price )
    {
        UnmapPrice2Order( price );
        delete o;
    }

    Order *OrderList::GetOrder( int id )
    {
        OrderTableT::iterator it = m_Orders.find( id );
        return it == m_Orders.end() ? NULL : it->second;
    }

    void OrderList::MapSub2Main( SubOrder *sub, Order *main )
    {
        m_Sub2MainTable[sub] = main;
    }

    void OrderList::UnmapSub2Main( SubOrder *sub, Order *main )
    {
        m_Sub2MainTable.erase( sub );
    }

    Order *OrderList::GetMainFromSub( SubOrder *sub )
    {
        Sub2MainTableT::iterator it = m_Sub2MainTable.find( sub );
        return  it == m_Sub2MainTable.end() ? NULL : it->second;
    }

    void OrderList::MapPrice2Order( Order *order )
    {
        int price = order->Price();
        if( price == -1 )
        {
            return;
        }
        m_PriceTable[price] = order;
    }

    void OrderList::UnmapPrice2Order( int price )
    {
        assert( m_PriceTable.find( price ) != m_PriceTable.end() );
        m_PriceTable.erase( price );
    }
}
