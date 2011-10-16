///
/// @file AHMessageProc.cpp
/// @brief process AH message
///
#include "stdafx.h"
#include "../Player.h"
#include "../Goods/CGoodsFactory.h"
#include "../Container/CTestContainer.h"
#include "../../../../Public/Common/AHBaseDef.h"
#include "../../../../Public/Setup/GlobalSetup.h"
#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/AHConf.h"


using namespace AH;

typedef void (*MsgHandleFunc)( CMessage*, const CGUID& );

struct Handler
{
    MsgHandleFunc func;
    uchar op;
};

// transfer client message
#define TRANSFER_CMSG( msg, playerID ) \
    msg->Add( playerID ); \
    msg->SetType( MSG_S2W_AH_OPER ); \
    msg->Send()

static void SendResMsg( const CGUID &playerID, uchar op, uchar ret )
{
    CMessage msg( MSG_S2C_AH_OPER );
    msg.Add( (uchar) op );
    msg.Add( (uchar) ret );
    msg.SendToPlayer( playerID );
}

static ulong GetCharge( long cnt, long price )
{
    return (ulong)( cnt * price * AH_CHARGE_PER );
}

// op(byte)
static void OnClientRequest( CMessage *msg, const CGUID &playerID )
{
    TRANSFER_CMSG( msg, playerID );
}

// op(byte), cnt(int), price(int), type(int)
static void OnClientAddAgentOrder( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = msg->GetPlayer();
    if( player == NULL )
    {
        return;
    }
    long cnt = msg->GetLong();
    long price = msg->GetLong();
    long type = msg->GetLong();

    if( cnt <= 0 || price < 0 )
    {
        SendResMsg( playerID, AH_ADD_AGENT, RES_INVALID_ARG );
        return ;
    }

    if( type == OT_SELL )
    {
        ulong amount = player->GetGoodsAmountByOriginName( AH_GOODS );
        if( cnt * AH_GOODS_CNT > (long)amount )
        {
            SendResMsg( playerID, AH_ADD_AGENT, RES_NOT_ENOUGH_GOODS );
            return;
        }
        ulong charge = GetCharge( cnt, price );
        if( player->GetMoney() < charge )
        {
            SendResMsg( playerID, AH_ADD_AGENT, RES_NOT_ENOUGH_MONEY );
            return;
        }
    }
    else if( type == OT_BUY )
    {
        long sum = cnt * price;
        sum += (long)( sum * AH_CHARGE_PER );
        if( (long)player->GetMoney() < sum )
        {
            SendResMsg( playerID, AH_ADD_AGENT, RES_NOT_ENOUGH_MONEY );
            return;
        }
    }
    else
    {
        SendResMsg( playerID, AH_ADD_AGENT, RES_INVALID_ARG );
        return;
    }
    // condition check success, can transfer the request to WS
    CMessage resMsg( MSG_S2W_AH_OPER );
    resMsg.Add( (uchar) AH_ADD_AGENT );
    resMsg.Add( cnt );
    resMsg.Add( price );
    resMsg.Add( type );
    resMsg.Add( playerID );
    resMsg.Send();
}

// op(byte), orderID(int)
static void OnClientRemoveAgentOrder( CMessage *msg, const CGUID &playerID )
{
    TRANSFER_CMSG( msg, playerID );
}

// op(byte), orderID(int), cnt(int)
static void OnClientBuyOrder( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = msg->GetPlayer();
    if( player == NULL )
    {
        return;
    }
    msg->Add( playerID );
    msg->Add( player->GetMoney() );
    msg->SetType( MSG_S2W_AH_OPER );
    msg->Send();
}

// op(byte), orderID(int), cnt(int)
static void OnClientSellOrder( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = msg->GetPlayer();
    if( player == NULL )
    {
        return;
    }
    ulong amount = player->GetGoodsAmountByOriginName( AH_GOODS );
    msg->Add( playerID );
    msg->Add( amount );
    msg->SetType( MSG_S2W_AH_OPER );
    msg->Send();
}

// op(byte), type(byte)
static void OnClientPickup( CMessage *msg, const CGUID &playerID )
{
    TRANSFER_CMSG( msg, playerID );
}

static Handler clientHandlers[] = {
    { OnClientRequest, AH_REQUEST_BUY },
    { OnClientRequest, AH_REQUEST_SELL },
    { OnClientRequest, AH_REQUEST_AGENT },
    { OnClientAddAgentOrder, AH_ADD_AGENT },
    { OnClientRemoveAgentOrder, AH_REMOVE_AGENT },
    { OnClientBuyOrder, AH_BUY },
    { OnClientSellOrder, AH_SELL },
    { OnClientPickup, AH_PICKUP },
    { NULL, 0 }
};

static void OnWSRequest( CMessage *msg, const CGUID &playerID )
{
    msg->SetType( MSG_S2C_AH_OPER );
    msg->SendToPlayer( playerID );
}

static void OnWSAddAgentOrder( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = GetGame()->FindPlayer( playerID );
    if( player == NULL )
    {
        return;
    }
    uchar ret = msg->GetByte();

    CMessage resMsg( MSG_S2C_AH_OPER );
    resMsg.Add( (uchar) AH_ADD_AGENT );
    resMsg.Add( (uchar) ret );
    if( ret == RES_OK )
    {
        long id = msg->GetLong();
        long cnt = msg->GetLong();
        long price = msg->GetLong();
        long type = msg->GetLong();
        if( type == OT_SELL )
        {
            ulong charge = GetCharge( cnt, price );
            player->SetMoney( (long)( player->GetMoney() - charge ), 0 );
            AHLOGS( player->GetExID(), AHOP_DEL_MONEY, charge, AH_NULL_VAL );
            player->DelGoods( AH_GOODS, cnt * AH_GOODS_CNT );
            AHLOGS( player->GetExID(), AHOP_DEL_GOODS, cnt * AH_GOODS_CNT, AH_NULL_VAL );
        }
        else if( type == OT_BUY )
        {
            ulong sum = cnt * price;
            sum += (long)( sum * AH_CHARGE_PER );
            player->SetMoney( (long)( player->GetMoney() - sum ), 0 );
            AHLOGS( player->GetExID(), AHOP_DEL_MONEY, sum, AH_NULL_VAL );
        }
        resMsg.Add( id );
    }
    resMsg.SendToPlayer( playerID );
}

static void OnWSRemoveAgentOrder( CMessage *msg, const CGUID &playerID )
{
    uchar ret = msg->GetByte();
    SendResMsg( playerID, AH_REMOVE_AGENT, ret );
}

static void OnWSBuyOrder( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = GetGame()->FindPlayer( playerID );
    if( player == NULL )
    {
        return;
    }
    uchar ret = msg->GetByte();
    if( ret == RES_OK )
    {
        long spentMoney = msg->GetLong();
        // del money
        player->SetMoney( player->GetMoney() - spentMoney, 0 );
        AHLOGS( player->GetExID(), AHOP_DEL_MONEY, spentMoney, AH_NULL_VAL );
    }
    SendResMsg( playerID, AH_BUY, ret );
}

static void OnWSSellOrder( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = GetGame()->FindPlayer( playerID );
    if( player == NULL )
    {
        return;
    }
    uchar ret = msg->GetByte();
    if( ret == RES_OK )
    {
        long goodsCnt = msg->GetLong();
        // del goods
        player->DelGoods( AH_GOODS, goodsCnt );

        AHLOGS( player->GetExID(), AHOP_DEL_GOODS, goodsCnt, AH_NULL_VAL );
    }
    SendResMsg( playerID, AH_SELL, ret );
}

static void OnWSPickupCheck( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = GetGame()->FindPlayer( playerID );
    if( player == NULL )
    {
        return;
    }
    uchar ret = 1;
    uchar type = msg->GetByte();
    long val = msg->GetLong();
    if( type == PT_COIN )
    {
        if( player->GetMoney() + val > GlobalSetup::GetSetup()->dwGoldCoinLimit )
        {
            SendResMsg( playerID, AH_PICKUP, RES_MAX_MONEY );
            return;
        }
    }
    else if( type == PT_GOODS )
    {
		CTestContainer testContainer;
		testContainer.Init( player );

        // create goosds to test
        ulong index = GoodsSetup::QueryGoodsIDByOriginalName( AH_GOODS );
        std::vector<CGoods*> gainList;
        CGoodsFactory::CreateGoods( index, val, gainList, 101 ); 

        std::vector<tagTestResult> addResult;
        ret = testContainer.AddTest( gainList, &addResult ) ? 1 : 0;
        
        // del goods
        for( size_t i = 0; i < gainList.size(); ++ i )
        {
            CGoodsFactory::GarbageCollect( &gainList[i], 101 );
        }
    }
    else
    {
        return;
    }

    if( ret )
    {
        CMessage resMsg( MSG_S2W_AH_OPER );
        resMsg.Add( (uchar) AH_PICKUP_CHECK );
        resMsg.Add( type );
        resMsg.Add( playerID );
        resMsg.Send();
    }
}

static void OnWSPickup( CMessage *msg, const CGUID &playerID )
{
    CPlayer *player = GetGame()->FindPlayer( playerID );
    if( player == NULL )
    {
        return;
    }
    uchar type = msg->GetByte();
    long cnt = msg->GetLong();
    if( type == PT_COIN )
    {
        player->SetMoney( player->GetMoney() + cnt, 0 ); 
        AHLOGS( player->GetExID(), AHOP_ADD_MONEY, cnt, AH_NULL_VAL );
    }
    else if( type == PT_GOODS )
    {
        CTestContainer testContainer;
		testContainer.Init( player );

        // create goosds
        ulong index = GoodsSetup::QueryGoodsIDByOriginalName( AH_GOODS );
        std::vector<CGoods*> gainList;
        CGoodsFactory::CreateGoods( index, cnt, gainList, 102 ); 

        AHLOGS( player->GetExID(), AHOP_ADD_GOODS, cnt, AH_NULL_VAL );

        std::vector<tagTestResult> addResult;
        bool ret = testContainer.AddTest( gainList, &addResult ) ? true : false;
        if( !ret )
        {
            // del goods
            for( size_t i = 0; i < gainList.size(); ++ i )
            {
                CGoodsFactory::GarbageCollect( &gainList[i], 102 );
            }
        }
        else
        {
            // add goods
            for( size_t i = 0; i < addResult.size(); ++ i )
			{
				if( !player->AddGoodsByTest( &addResult[i], gainList[i] ) )
				{
					CGoodsFactory::GarbageCollect( &gainList[i], 102 );
				}
            }
        }
    }
    else
    {
        SendResMsg( playerID, AH_PICKUP, RES_INVALID_ARG );
        return;
    }
    SendResMsg( playerID, AH_PICKUP, RES_OK );
}

static Handler wsHandlers[] = {
    { OnWSRequest, AH_REQUEST_BUY },
    { OnWSRequest, AH_REQUEST_SELL },
    { OnWSRequest, AH_REQUEST_AGENT },
    { OnWSAddAgentOrder, AH_ADD_AGENT },
    { OnWSRemoveAgentOrder, AH_REMOVE_AGENT },
    { OnWSBuyOrder, AH_BUY },
    { OnWSSellOrder, AH_SELL },
    { OnWSPickup, AH_PICKUP },
    { OnWSPickupCheck, AH_PICKUP_CHECK },
    { NULL, 0 }
};

static MsgHandleFunc GetHandler( Handler *handlers, uchar op )
{
    int i = 0;
    for( ; handlers[i].func != NULL && handlers[i].op != op; ++ i )
    {
    }
    return handlers[i].func;
}

static void DispatchHandle( CMessage *msg, Handler *handlers, bool fromSelf )
{
    uchar opType = msg->GetByte();
    CGUID playerID;
    if( fromSelf )
    {
        playerID = msg->GetPlayerID();
    }
    else
    {
        msg->GetGUID( playerID );
    }
    MsgHandleFunc func = GetHandler( handlers, opType );
    if( func )
    {
        func( msg, playerID );
    }
}

// op(byte), ...
void OnAHClientMsg( CMessage *msg )
{
    DispatchHandle( msg, clientHandlers, true );
}

// playerID(CGUID), op(byte), ...
void OnAHWSMsg( CMessage *msg )
{
    DispatchHandle( msg, wsHandlers, false );
}
