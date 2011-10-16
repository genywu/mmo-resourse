///
/// @file Order.h
/// @author Kevin Lynx
///
#ifndef ___AH_ORDER_H_
#define ___AH_ORDER_H_

#include "OrderListener.h"
#include "NumIDGen.h"
#include "../../../../Public/Common/AHBaseDef.h"
#include <vector>

namespace AH
{
    class AgentOrder;

    /// SubOrder represents a single order of a player.
    /// Many same (compared by IsEqual) SubOrders combine to an Order.
    class SubOrder 
    {
    public:
        SubOrder( int id, int cnt, int price, int type );

        /// Check whether 2 SubOrder equals.
        bool IsEqual( const SubOrder *other ) const;

        /// check whether this SubOrder is timeout
        bool IsTimeOut() const;

        /// called when player add an agent order
        /// triger listener->OnAdd
        void Add();

        /// called when player cancel an agent order
        /// triger listener->OnRemove
        void Remove();

        /// called when player buy/sell some cnt goods in a sub order
        /// triger listener->OnUpdate
        void Update( int subCnt, AgentOrder *order );

        /// delete itself, by listeners
        void Delete();

    public:
        SubOrderRaw m_Raw;
        ListenerList m_Listeners;
        int m_ID;
    };

    typedef std::vector<uchar> Buffer;

    /// AgentOrder keeps a list of SubOrders for one player.
    class AgentOrder : public OrderListener
    {
    public:
        /// <orderID, SubOrder*>
        typedef std::map<int, SubOrder*> OrderTableT;
    public:
        AgentOrder( const CGUID &owner );

        ~AgentOrder();

        void Clear();

        /// Encode to DB
        void Encode( Buffer &buf );

        /// Decode from DB
        void Decode( const Buffer &buf );

        /// Encode to Client
        void Encode( DBWriteSet &db );

        /// pickup money/goods from OrderRet, return the cnt it pickuped
        int Pickup( int type );

        /// add a SubOrder, new a SubOrder and call its Add func
        /// return the SubOrder's id
        int Add( int cnt, int price, int type );

        /// remove a SubOrder, find the SubOrder first then call its Remove func
        int Remove( int id );

        /// check every SubOrder timeout, if timeout call its Remove func
        /// and then remove the SubOrder
        bool CheckTimeOut();	

        /// get the SubOrder's count, not the sum of the SubOrder::cnt
        int Count() const;

        /// check whether the AgnetOrder is still valid, has at least 1 SubOrder or the
        /// OrderRet is not 0 is a valid AgentOrder
        bool IsValid() const;

        /// if the SubOrder::cnt <= 0, then call Remove func to triger OnRemove
        /// event, update OrderRet also.
        virtual bool OnUpdate( SubOrder *order, int subCnt, AgentOrder *aorder );

        /// append log
        virtual void OnAdd( SubOrder *order );

        /// append log
        virtual void OnRemove( SubOrder *order );

        /// delete the SubOrder
        virtual bool OnDelete( SubOrder *order );

        /// get agent ret value
        int GetRetVal( int type );

        OrderRet *GetRet() { return &m_Ret; }
    private:
        void Remove( SubOrder *order );

        /// Give money/goods back to player
        void OnRemoveOrder( SubOrder *order );
    private:
        OrderTableT m_Orders;
        OrderRet m_Ret;
        NumIDGen m_IDGen;
        CGUID m_Owner;
    };

    /// Order in Sell/Buy order list
    /// it has one SubOrder at least, otherwise delete it
    class Order 
    {
    public:
        /// <genTime, SubOrder*>, the SubOrder* is only a reference to
        /// AgentOrder::Orders
        typedef std::map<time_t, SubOrder*> OrderTableT;
    public:
        Order( int id ) ;

        /// add a SubOrder to this
        void Add( SubOrder *order );

        /// remove a SubOrder from this
        void Remove( SubOrder *order );

        /// buy/sell some cnt goods in this order,
        /// get the earliest SubOrder and call its Update func
        int Update( int subCnt, AgentOrder *aorder );

        /// get the sell/buy price of this order
        int Price() const;

        /// get the count of this order, the count is the sum of all the SubOrder
        int Count() const;	

        /// get the SubOrder count
        int SubOrderCnt() const;

        /// check whether 1 SubOrder is equal, if so add the SubOrder to this
        bool IsEqual( const SubOrder *order ) const;

        int GetID() const { return m_ID; }
    private:
        OrderTableT m_Orders;
        int m_ID;
    };

    /// Sell/Buy order list
    class OrderList : public OrderListener
    {
    public:
        /// <orderID, Order*>
        typedef std::map<int, Order*> OrderTableT;
        /// <SubOrder*, Order*>, a helper table to get Order from SubOrder
        typedef std::map<SubOrder*, Order*> Sub2MainTableT;
        /// <price, Order*>
        typedef std::map<int, Order*> PriceTableT;
    public:
        OrderList( int type );

        ~OrderList();

        void Clear();

        /// get the Order specified by id, and call its Update func
        int Update( int id, int maxCnt, int subCnt, AgentOrder *aorder, int *operRet );

        /// called when an AgentOrder add a new SubOrder
        /// check whether the SubOrder can be added to an exist Order, if so add it,
        /// otherwise create a new Order.
        virtual void OnAdd( SubOrder *order );

        /// called when an AgentOrder remove a SubOrder
        /// get the Order contains the SubOrder and call Order::Remove
        virtual void OnRemove( SubOrder *order );

        /// encode top N order to game client
        /// format: N, OrderRaw1, OrderRaw2, ..., OrderRawN
        void EncodeTopN( int N, DBWriteSet &db, bool reverse = false );

    private:
        Order *GetOrder( int id );

        /// create a new Order contains the SubOrder
        void NewOrder( SubOrder *order );

        /// remove an Order when the Order is empty(contains 0 SubOrder)
        void RemoveOrder( Order *o, int price );

        /// build the map from SubOrder to Order
        void MapSub2Main( SubOrder *sub, Order *main );

        /// unmap SubOrder from Order
        void UnmapSub2Main( SubOrder *sub, Order *main );

        /// get the order which the SubOrder belongs
        Order *GetMainFromSub( SubOrder *sub );

        /// build the map from Price to Order
        void MapPrice2Order( Order *order );

        void UnmapPrice2Order( int price );

        void EncodeBaseRaw( DBWriteSet &db, Order *order );
    private:
        OrderTableT m_Orders;
        Sub2MainTableT m_Sub2MainTable;
        PriceTableT m_PriceTable;
        NumIDGen m_IDGen;
        int m_Type;
    };

}

#endif
