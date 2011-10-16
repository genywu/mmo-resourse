///
/// @file BusinessActLog.cpp
/// @brief 记录跑商操作日志
///
#include "StdAfx.h"
#include "../goods/CGoodsFactory.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../../LogClient/gs/LogicLogInterface.h"
#include "BusinessCommon.h"

/*
 <!--跑商玩家在NPC处买卖日志-->
 <Table name="t700_business_npc_log" log_no="700" log_pos="1">
	<item type="client_head" head_name=""></item>
	<item type="pos" head_name=""></item>
	<item type="player" head_name=""></item>
	<item type="goods" head_name=""></item>
	<item type="alone" Item_name="act_type" data_type="int32"></item> <!--操作类型，1：买，2：卖-->
	<item type="alone" Item_name="goods_amount" data_type="int32"></item> <!--交易数量-->
	<item type="alone" Item_name="trade_spot" data_type="int32"></item> <!--贸易点ID-->
	<item type="alone" Item_name="spent_money" data_type="int32"></item> <!--消费的钱-->
 </Table>
*/
bool LogicLogInterface::logT700_business_npc( CPlayer *pPlayer, CGoods *pGoods, int iTradeAmount, 
		int iTradeMoney, int iActType, int iTradeSpot )
{
	if( 0 == GetGame()->GetSetup()->lUseLogServer )
	{
		return true;
	}
	if( NULL == m_pLogClient )
	{
		return false;
	}
	if( NULL == pPlayer || NULL == pGoods )
	{
		return false;
	}
	CRegion *pRegion = static_cast<CRegion*>( GetGame()->FindRegion( pPlayer->GetRegionGUID() ) );
	if( pRegion == NULL )
	{
		return false;
	}

	string strTime;
	GetCurrTimeString(strTime);

	float fPosX = pPlayer->GetPosX();
	float fPosY = pPlayer->GetPosY();

	return m_pLogClient->SendLogToServer( 
		700, strTime.c_str(), 
		pRegion->GetID(), pRegion->GetName(), &fPosX, &fPosY,
		&pPlayer->GetExID(), pPlayer->GetName(), 
		pGoods->GetBasePropertiesIndex(), &pGoods->GetExID(), pGoods->GetName(), 
		iActType, 
		iTradeAmount,
		iTradeSpot,
		iTradeMoney );
}


#ifdef BUSINESS_DUMP
#include "CTradeSpot.h"
#include "CBusinessGoods.h"

using namespace Business;

void business_dump( CTradeSpot *pSpot )
{
	const tagTradeSpotProperty &p = pSpot->GetProperty();
	char file[MAX_PATH];
	_snprintf( file, MAX_PATH, "spot_%d_.txt", p.Id );
	FILE *fp = fopen( file, "a" );
	if( fp == NULL )
	{
		return;
	}

	tagTime time_desc;
	fprintf( fp, "贸易点 %d %s 商品信息\n", p.Id, time_desc.GetDateFormatStr() );
	fprintf( fp, "--------------------------------------------------------------------------------\n" );
	fprintf( fp, "商品名\t当前库存\t本地\t买入价格\t卖出价格\n" );
	const CTradeSpot::GoodsTemplateTable &goods_table = pSpot->GetGoodsTemplateTable();
	for( CTradeSpot::GoodsTemplateTable::const_iterator cit = goods_table.begin();
		cit != goods_table.end(); ++ cit )
	{
		CBusinessGoods *pBusinessGoods = const_cast<CBusinessGoods*>( cit->second );		
		const tagGoods &goods_info = pBusinessGoods->GetBaseProperty();
		const char *name = CGoodsFactory::QueryGoodsName( pBusinessGoods->GetIndex() );

		fprintf( fp, "%s\t%d\t%c\t%d\t%d\n", 
			name, pBusinessGoods->GetVolume(), 
			goods_info.Local == 1 ? 'Y':'N', 
			goods_info.Local == 1 ? pBusinessGoods->GetBuyPrice() : -1, 
			goods_info.Local == 1 ? -1 : pBusinessGoods->GetSellPrice() );
	}

	fprintf( fp, "\n\n" );
	fclose( fp );
}
#endif
