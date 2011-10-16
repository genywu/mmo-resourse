///
/// @file MsgProc.cp
/// @brief process AH message
///
#include "stdafx.h"
#include "OrderMgr.h"
#include "../../../../Public/Common/AHBaseDef.h"

using namespace AH;

#define NULL_EXP ;
#define SEND_RES_MSG( msg, op, ret, playerID, extExp ) \
    { \
       CMessage resMsg( MSG_W2S_AH_OPER ); \
       resMsg.Add( (uchar) op ); \
       resMsg.Add( playerID ); \
       resMsg.Add( (uchar) ret ); \
       extExp;  \
       resMsg.SendToSocket( msg->GetSocketID() ); \
    }

typedef void (*MsgHandleFunc)( CMessage* );

struct Handler
{
    MsgHandleFunc func;
    uchar op;
};

static void SendResData( CMessage *msg, uchar type )
{
    CGUID playerID;
    msg->GetGUID( playerID );

    CMessage resMsg( MSG_W2S_AH_OPER );
    resMsg.Add( (uchar) type );  
    resMsg.Add( playerID ); // only used in GS, client ignore this
    DBWriteSet db;
    resMsg.GetDBWriteSet( db );
    GetInst( OrderMgr ).EncodeToClient( playerID, db, type );
    resMsg.SendToSocket( msg->GetSocketID() );
}

static void OnRequestBuy( CMessage *msg )
{
    SendResData( msg, AH_REQUEST_BUY );
}

static void OnRequestSell( CMessage *msg )
{
    SendResData( msg, AH_REQUEST_SELL );
}

static void OnRequestAgent( CMessage *msg )
{
    SendResData( msg, AH_REQUEST_AGENT );
}

static void OnAddAgentOrder( CMessage *msg )
{
    long cnt = msg->GetLong();
    long price = msg->GetLong();
    long type = msg->GetLong();
    CGUID playerID;
    msg->GetGUID( playerID );

    // ret also is the order ID
    int ret = GetInst( OrderMgr ).AddAgent( playerID, cnt, price, type );

    CMessage resMsg( MSG_W2S_AH_OPER ); 
    resMsg.Add( (uchar) AH_ADD_AGENT ); 
    resMsg.Add( playerID ); 
    if( ret >= RES_END )
    {
        resMsg.Add( (uchar) RES_OK );
        resMsg.Add( (long) ret );
    }
    else
    {
        resMsg.Add( (uchar) ret ); 
    }
    resMsg.Add( cnt );
    resMsg.Add( price );
    resMsg.Add( type );
    resMsg.SendToSocket( msg->GetSocketID() ); 
}

static void OnRemoveAgentOrder( CMessage *msg )
{
    int orderID = msg->GetLong();
    CGUID playerID;
    msg->GetGUID( playerID );

    int ret = GetInst( OrderMgr ).RemoveAgent( playerID, orderID );

    SEND_RES_MSG( msg, AH_REMOVE_AGENT, ret, playerID, NULL_EXP );
}

static void OnBuyOrder( CMessage *msg )
{
    int orderID = msg->GetLong();
    int cnt = msg->GetLong();
    CGUID playerID;
    msg->GetGUID( playerID );
    if( cnt <= 0 )
    {
        SEND_RES_MSG( msg, AH_BUY, RES_INVALID_ARG, playerID, NULL_EXP );
        return;
    }
    int playerMoney = msg->GetLong();
    int spentMoney;
    int ret = GetInst( OrderMgr ).Buy( playerID, playerMoney, orderID, cnt, &spentMoney );

    SEND_RES_MSG( msg, AH_BUY, ret, playerID, resMsg.Add( (long) spentMoney ) );
}

static void OnSellOrder( CMessage *msg )
{
    int orderID = msg->GetLong();
    int cnt = msg->GetLong();
    CGUID playerID;
    msg->GetGUID( playerID );
    if( cnt <= 0 )
    {
        SEND_RES_MSG( msg, AH_SELL, RES_INVALID_ARG, playerID, NULL_EXP );
        return;
    }
    int playerGoods = msg->GetLong();
    int goodsCnt;
    int ret = GetInst( OrderMgr ).Sell( playerID, playerGoods, orderID, cnt, &goodsCnt );

    SEND_RES_MSG( msg, AH_SELL, ret, playerID, resMsg.Add( (long) goodsCnt ) );
}

static void OnPickup( CMessage *msg )
{
    uchar t = msg->GetByte();
    CGUID playerID;
    msg->GetGUID( playerID );

    AgentOrder *order = GetInst( OrderMgr ).GetAgentOrder( playerID );
    if( order == NULL )
    {
        return;
    }

    int v = order->GetRetVal( t );
    if( v == 0 )
    {
        return;
    }

    CMessage resMsg( MSG_W2S_AH_OPER );
    resMsg.Add( (uchar) AH_PICKUP_CHECK );
    resMsg.Add( playerID );
    resMsg.Add( t );
    resMsg.Add( (long) v );
    resMsg.SendToSocket( msg->GetSocketID() );

}

static void OnPickupCheck( CMessage *msg )
{
    uchar t = msg->GetByte();
    CGUID playerID;
    msg->GetGUID( playerID );

    int cnt = GetInst( OrderMgr ).Pickup( playerID, t );

    CMessage resMsg( MSG_W2S_AH_OPER ); 
    resMsg.Add( (uchar) AH_PICKUP ); 
    resMsg.Add( playerID );
    resMsg.Add( (uchar) t );
    resMsg.Add( (long) cnt );
    resMsg.SendToSocket( msg->GetSocketID() );
}

static Handler handlers[] = {
    { OnRequestBuy, AH_REQUEST_BUY },
    { OnRequestSell, AH_REQUEST_SELL },
    { OnRequestAgent, AH_REQUEST_AGENT },
    { OnAddAgentOrder, AH_ADD_AGENT },
    { OnRemoveAgentOrder, AH_REMOVE_AGENT },
    { OnBuyOrder, AH_BUY },
    { OnSellOrder, AH_SELL },
    { OnPickup, AH_PICKUP },
    { OnPickupCheck, AH_PICKUP_CHECK },
    { NULL, 0 }
};

static MsgHandleFunc GetHandler( uchar op )
{
    int i = 0;
    for( ; handlers[i].func != NULL && handlers[i].op != op; ++ i )
    {
    }
    return handlers[i].func;
}

void OnAHMsg( CMessage *msg )
{
    uchar opType = msg->GetByte();
    MsgHandleFunc func = GetHandler( opType );
    if( func )
    {
        func( msg );
    }
}
