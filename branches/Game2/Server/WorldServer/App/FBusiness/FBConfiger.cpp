///
/// @file Configer.cpp
/// @brief Read/maintain the configure for FBusiness.
///
#include "StdAfx.h"
#include "FBConfiger.h"
#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/GlobalSetup.h"

namespace FBusiness
{
#define QUERY_NUM( name, var, elem, type ) \
    do { \
    double _t = 0; \
    elem->Attribute( name, &_t ) ; \
    var = static_cast<type>( _t ); \
    }while( 0 )

#define QUERY_STR( name, var, elem ) \
    do { \
    const char *_t = elem->Attribute( name ); \
    if( _t != 0 ) strcpy( var, _t ); \
    }while( 0 ) 

    bool CConfiger::Load()
    {
        const char *GOODS_FILE = "data/fbusiness/goods_list.ini";
        const char *SHOP_FILE = "data/fbusiness/fbusiness.xml";

        bool ret = true;
        ret = LoadGoodsList( GOODS_FILE ) && ret;
        ret = LoadShopList( SHOP_FILE ) && ret;
        return ret;
    }

    bool CConfiger::LoadGoodsList( const char *file )
    {
        CRFile *prfile = rfOpen( file );
        if( prfile == NULL )
        {
            Log4c::Error(ROOT_MODULE,"%-15s Load file %s failed.",__FUNCTION__, file );
            return false;
        }

        std::stringstream stream;
        prfile->ReadToStream( stream );
        rfClose( prfile );

        while( ReadTo( stream, "#" ) )
        {
            std::string strGoodsName, strOrigName;
            BuyGoods goods;

            stream >> strGoodsName
                >> strOrigName
                >> goods.fPriceQuoMin
                >> goods.fPriceQuoMax;

            goods.lIndex = (long)GoodsSetup::QueryGoodsIDByOriginalName( strOrigName.c_str() );
            m_BuyGoodsList.push_back( goods );
        }
        return true;
    }

    bool CConfiger::LoadShopList( const char *file )
    {
        CRFile *prfile = rfOpen( file );
        if( prfile == NULL )
        {
           Log4c::Error(ROOT_MODULE,"%-15s Load file %s failed.",__FUNCTION__, file );
            return false;
        }

        TiXmlDocument doc;
        if( !doc.LoadData( prfile->GetData(), prfile->GetDatalen() ) )
        {
            Log4c::Error(ROOT_MODULE,"%-15s Parse %s failed, invalid xml format.",__FUNCTION__, file );
            rfClose( prfile );
            return false;
        }
        rfClose( prfile );
        TiXmlElement *root = doc.RootElement();
        {
            // global config
            TiXmlElement *node = root->FirstChildElement( "Global" );
            QUERY_NUM( "goods_update_interval", m_GlobalCfg.uGoodsUpdateInter, node, unsigned long );
            QUERY_NUM( "buy_count", m_GlobalCfg.lBuyCount, node, long );
            QUERY_NUM( "enable", m_GlobalCfg.lEnable, node, long );

            char strCoinGoods[64];
            QUERY_STR( "coin_goods", strCoinGoods, node );
            m_GlobalCfg.lCoinGoodsIndex = GoodsSetup::QueryGoodsIDByOriginalName( strCoinGoods );
        }
        {
            // shop list
            TiXmlElement *shoplist_node = root->FirstChildElement( "ShopList" );
            for( TiXmlElement *shop_node = shoplist_node->FirstChildElement();
                shop_node != NULL; shop_node = shop_node->NextSiblingElement() )
            {
                long id;
                QUERY_NUM( "id", id, shop_node, long );
                if( m_ShopTable.find( id ) != m_ShopTable.end() )
                {
                    // more than 1 shop with the same id, ignore it.
                    Log4c::Warn(ROOT_MODULE, "More than 1 shop with the same id [%d].", id );
                    continue;
                }
                Shop shop;
                QUERY_STR( "npc_orig_name", shop.npcOrigName, shop_node );
                SellGoodsListT *pSellGoodsList = new SellGoodsListT();
                shop.SellList = pSellGoodsList;
                for( TiXmlElement *goods_node = shop_node->FirstChildElement();
                    goods_node != NULL; goods_node = goods_node->NextSiblingElement() )
                {
                    SellGoods goods;
                    char strOrigName[64];
                    QUERY_STR( "orig_name", strOrigName, goods_node );
                    goods.lIndex = GoodsSetup::QueryGoodsIDByOriginalName( strOrigName );
                    pSellGoodsList->push_back( goods );
                }
                m_ShopTable.insert( std::make_pair( id, shop ) );
            }
        }
        return true;
    }

    void CConfiger::Reset()
    {
        memset( &m_GlobalCfg, 0, sizeof( m_GlobalCfg ) );
        m_BuyGoodsList.clear();
        for( ShopTableT::iterator it = m_ShopTable.begin(); it != m_ShopTable.end(); ++ it )
        {
            delete it->second.SellList;
        }
        m_ShopTable.clear();
    }

    bool CConfiger::ReLoad()
    {
        Reset();
        return Load();
    }

    void CConfiger::Encode( DBWriteSet &db ) const
    {
        db.AddToByteArray( (long) m_BuyGoodsList.size() );
        for( BuyGoodsListT::const_iterator it = m_BuyGoodsList.begin();
            it != m_BuyGoodsList.end(); ++ it )
        {
            const BuyGoods &goods = *it;
            db.AddToByteArray( const_cast<BuyGoods*>( &goods ), sizeof( goods ) );
        }
        db.AddToByteArray( const_cast<GlobalCfg*>( &m_GlobalCfg ), sizeof( m_GlobalCfg ) );
        db.AddToByteArray( (long) m_ShopTable.size() );
        for( ShopTableT::const_iterator it = m_ShopTable.begin();
            it != m_ShopTable.end(); ++ it )
        {
            db.AddToByteArray( (long) it->first );
            db.AddToByteArray( it->second.npcOrigName );
            const SellGoodsListT *sellgoods_list = it->second.SellList;
            db.AddToByteArray( (long) sellgoods_list->size() );
            for( SellGoodsListT::const_iterator sit = sellgoods_list->begin();
                sit != sellgoods_list->end(); ++ sit )
            {
                const SellGoods &goods = *sit;
                db.AddToByteArray( const_cast<SellGoods*>( &goods ), sizeof( goods ) );
            }
        }
    }
}
