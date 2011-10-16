///
/// @file AHBaseDef.h
/// @brief Auction House module base definitions
///
#ifndef ___AH_BASE_DEF_H_
#define ___AH_BASE_DEF_H_

namespace AH
{
    /// message operation type
    enum
    {
        AH_REQUEST_BUY,
        AH_REQUEST_SELL,
        AH_REQUEST_AGENT,
        AH_ADD_AGENT,
        AH_REMOVE_AGENT,
        AH_BUY,
        AH_SELL,
        AH_PICKUP,
        AH_PICKUP_CHECK, // only in Server
    };

    /// message operation result type
    enum
    {
        RES_OK,
        RES_NOT_ENOUGH_MONEY,
        RES_NOT_ENOUGH_GOODS,
        RES_INVALID_ORDER,
        RES_INVALID_ORDER_CNT,
        RES_MAX_MONEY,
        RES_MAX_GOODS,
        RES_INVALID_ARG,
        RES_AGENT_MAX,
        RES_UNKNWON_ERROR,
        RES_END
    };

    /// order type 
    enum 
    {
        OT_SELL, OT_BUY
    };

    /// pickup operation type 
    enum
    {
        PT_GOODS, PT_COIN
    };

    /// Order raw data, used in Encode/Decode between Server&Client.
    struct OrderRaw
    {
        int id;
        int cnt;
        int price;
    };

    /// Player agent order 
    struct SubOrderRaw
    {
        // represents the remain time in client
        uint gentime;
        int cnt;
        int price;
        int type;
    };

    /// from AgentOrder.
    struct OrderRet
    {
        int money;
        int conch;
    };
}

//#ifdef _GAME_SERVER
//#define AH_LOG_FILE "AHLogGS"
//#elif defined _WORLD_SERVER_
//#define AH_LOG_FILE "AHLogWS"
//#endif

//#ifdef AH_LOG_FILE
//#define AHLOG PutoutLog
//#endif

#ifdef USE_IN_SERVER

#define CONF (GetInst(AH::ConfReader).m_Cfg)
#define AH_TIMEOUT ( 60 * CONF.timeOut )
#define AH_CHECK_INTERVAL ( 60 * CONF.checkInter )
#define AH_SAVE_INTERVAL ( 60 * CONF.saveInter )
#define AH_GOODS_CNT CONF.goodsCnt
#define AH_GOODS CONF.goodsOrig
#define AH_CHARGE_PER CONF.chargePer
#define AH_ORDER_COUNT CONF.orderCnt
#define AH_AGENT_MAX CONF.agentMax

// LOGServer stuff
#define AHLOGS( player, op, cnt, price ) \
    GetLogicLogInterface()->logT987_auction_house_log( player, op, cnt, price )

#define AHOP_ADD_SELL "AddSell"
#define AHOP_ADD_BUY "AddBuy"
#define AHOP_REMOVE_SELL "RemoveSell"
#define AHOP_REMOVE_BUY "RemoveBuy"
#define AHOP_BUY "Buy"
#define AHOP_SELL "Sell"
#define AHOP_ASELL_RET "AgentSellRet"
#define AHOP_ABUY_RET "AgentBuyRet"
#define AHOP_PICKUP_GOODS "PickupGoods"
#define AHOP_PICKUP_MONEY "PickupMoney"
#define AHOP_DEL_GOODS "DelGoods"
#define AHOP_DEL_MONEY "DelMoney"
#define AHOP_ADD_GOODS "AddGoods"
#define AHOP_ADD_MONEY "AddMoney"

#define AH_NULL_VAL (-1)

#endif


#endif
