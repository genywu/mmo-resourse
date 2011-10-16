///
/// @file OrderMgr.h
/// @author Kevin Lynx
///
#ifndef ___AH_ORDER_MGR_H_
#define ___AH_ORDER_MGR_H_

#include "Order.h"

namespace AH
{
    class OrderMgr : public CGameEvent
    {
    public:
        enum TimerType { TT_CHECK, TT_SAVE, INVALID_TIMER = -1 };
        typedef CTimerQueue<CGameEvent*> TimerQueue;
    public:
        /// <playerID, AgentOrder*>
        typedef std::map<CGUID, AgentOrder*> AgentOrderTableT;
    public:
        OrderMgr();

        ~OrderMgr();

        void Startup();

        void Shutdown();

        void Clear();

        /// add an AgentOrder, this function will generate a SubOrder id and send
        /// the id to the client
        int AddAgent( const CGUID &playerID, int cnt, int price, int type );

        /// remove an AgentOrder
        int RemoveAgent( const CGUID &playerID, int orderID );

        /// player buy some cnt goods 
        int Buy( const CGUID &buyer, int playerMoney, int orderID, int cnt, int *operRet );

        /// player sell some cnt goods
        int Sell( const CGUID &seller, int playerGoods, int orderID, int cnt, int *operRet );

        /// pickup money or goods, return the cnt it pickuped
        int Pickup( const CGUID &picker, int type );

        /// called in every game frame, to check Order timeout
        void Run();

        /// load data from DB, only AgentOrders
        void LoadFromDB( int ret, CEntityGroup *entity );

        /// save data to DB, only AgentOrders
        void SaveToDB( CEntityGroup *entity );

        /// start to save all data
        void SaveAll();

        /// Set listeners to an Order
        void SetListeners( SubOrder *order );

        /// encode data to game client
        void EncodeToClient( const CGUID &playerID, DBWriteSet &db, int type );

        AgentOrder *GetAgentOrder( const CGUID &playerID );

        bool IsDBLoaded() const { return m_DBLoaded; }
    private:
        /// check and get the player's AgentOrder, if the order does not exist, 
        /// create a new one for him.
        AgentOrder *CheckAgentOrder( const CGUID &playerID );

        /// check whether the AgentOrder is empty, and delete if it's empty
        bool CheckDelete( AgentOrder *o );

        /// mark the status dirty so that it will save to db later
        void MarkDirty( bool flag = true );

        /// check can add more agent orders ?
        int CanAddAgent( AgentOrder *order );
    private:
		virtual long OnTimeOut( ulong timerid, ulong curtime, const void *var ) ;
		virtual void OnTimerCancel( ulong timerid, const void *var ) {}
		virtual void OnMessage( CMessage* ) {}
		virtual long Proc( eEventType type, const tagProcVar *pVar ) { return 0; }
    private:
        /// player buy from server
        OrderList m_BuyOrders;
        /// player sell to server
        OrderList m_SellOrders;
        AgentOrderTableT m_AgentOrders;
        /// DB loaded flag
        bool m_DBLoaded;
        /// Timer stuff
        TimerQueue m_Timer;
        long m_CheckTimer;
        long m_SaveTimer;
        bool m_ChangeFlag;
    };
}

#endif
