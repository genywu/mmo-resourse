#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\..\..\DBAccess\MyAdoBase.h"
#include "..\nets\networld\CostNetClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应日志消息
void OnWriteLogMessage(CMessage* pMsg)
{
	//DWORD dwPlayerID;
	char sql[512];

	switch(pMsg->GetType())
	{
	case CMessage::MSG_S2W_WRITELOG_GOODSTRADE:// 接受GAMESERVER发的物品交易日志信息
		{
			BYTE byType;
			DWORD amount;
			DWORD dwGoodsID,dwPrice,dwPlayerID[2],dwPlayerCurMoney[2];
			char szGoodsName[32],sellerName[32],purchaserName[32];
			DWORD dwMapID[2],dwPosX[2],dwPosY[2];
			CPlayer *pSeller, *pPurchaser;

			byType = pMsg->GetByte();
			dwPlayerID[0] = pMsg->GetDWord();
			dwPlayerCurMoney[0] = pMsg->GetDWord();
			dwMapID[0] = pMsg->GetDWord();
			dwPosX[0]	= pMsg->GetDWord();
			dwPosY[0]	= pMsg->GetDWord();

			dwPlayerID[1] = pMsg->GetDWord();
			dwPlayerCurMoney[1] = pMsg->GetDWord();
			dwMapID[1] = pMsg->GetDWord();
			dwPosX[1]	= pMsg->GetDWord();
			dwPosY[1]	= pMsg->GetDWord();

			pSeller = GetGame()->GetMapPlayer(dwPlayerID[0]);
			pPurchaser = GetGame()->GetMapPlayer(dwPlayerID[1]);

			if(!pSeller || !pPurchaser)
			{
				AddLogText("trade %d 日志pPlayer为NULL %d, %d", byType, dwPlayerID[0], dwPlayerID[1]);
			}

			if(pSeller) {
				strcpy(sellerName, pSeller->GetName());
			}
			else {
				strcpy(sellerName, "NULL");
			}

			if(pPurchaser) {
				strcpy(purchaserName, pPurchaser->GetName());
			}
			else {
				strcpy(purchaserName, "NULL");
			}

			dwGoodsID = pMsg->GetDWord();
			dwPrice = pMsg->GetDWord();
			amount = pMsg->GetDWord();

			pMsg->GetStr(szGoodsName, 32);

			switch(byType)
			{
			case 0:		//交易
				sprintf(sql, "INSERT INTO \
							 goods_trade_log(seller_id,seller_name,seller_cur_money,s_map_id,s_pos_x,s_pos_y,purchaser_id,purchaser_name,purchaser_cur_money,p_map_id,p_pos_x,p_pos_y,goods_id,goods_name,price,amount,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,%d,'%s',%d,%d,0)", 
							 dwPlayerID[0], sellerName,dwPlayerCurMoney[0], dwMapID[0], dwPosX[0], dwPosY[0],
							 dwPlayerID[1], purchaserName, dwPlayerCurMoney[1], dwMapID[1], dwPosX[1], dwPosY[1],
							 dwGoodsID, szGoodsName,dwPrice,amount);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[物品交易]失败");
				break;

			case 1:		//商店

				sprintf(sql, "INSERT INTO \
							 goods_trade_log(seller_id,seller_name,seller_cur_money,s_map_id,s_pos_x,s_pos_y,purchaser_id,purchaser_name,purchaser_cur_money,p_map_id,p_pos_x,p_pos_y,goods_id,goods_name,price,amount,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,%d,'%s',%d,%d,1)", 
							 dwPlayerID[0], sellerName,dwPlayerCurMoney[0], dwMapID[0], dwPosX[0], dwPosY[0],
							 dwPlayerID[1], purchaserName, dwPlayerCurMoney[1], dwMapID[1], dwPosX[1], dwPosY[1],
							 dwGoodsID, szGoodsName,dwPrice,amount);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[商店交易]失败");
				break;
				
			case 2:		//个人商店交易失败后，返回物品给卖家时又失败的日志

				sprintf(sql, "INSERT INTO \
							 goods_trade_log(seller_id,seller_name,seller_cur_money,s_map_id,s_pos_x,s_pos_y,purchaser_id,purchaser_name,purchaser_cur_money,p_map_id,p_pos_x,p_pos_y,goods_id,goods_name,price,amount,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,%d,'%s',%d,%d,2)", 
							 dwPlayerID[0], sellerName,dwPlayerCurMoney[0], dwMapID[0], dwPosX[0], dwPosY[0],
							 dwPlayerID[1], purchaserName, dwPlayerCurMoney[1], dwMapID[1], dwPosX[1], dwPosY[1],
							 dwGoodsID, szGoodsName,dwPrice,amount);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[商店交易]失败");
				break;
			}


		}
		break;

	case CMessage::MSG_S2W_WRITELOG_GOODS:// 接受GAMESERVER发的一般物品日志信息
		{
			BYTE byType;
			DWORD dwGoodsID,dwPrice,dwMapID,dwPosX,dwPosY,dwCurMoney,dwCurBank;
			char szGoodsName[32],playerName[32];
			CPlayer *pPlayer;

			byType = pMsg->GetByte();
			UINT nPlayerID = pMsg->GetLong();
			
			pPlayer = GetGame()->GetMapPlayer(nPlayerID);

			if(!pPlayer)
			{
				AddLogText("goods %d 日志pPlayer为NULL %d", byType, nPlayerID);
				strcpy(playerName, "NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			WORD pk_count = pMsg->GetWord();
			dwCurMoney = pMsg->GetLong();
			dwCurBank = pMsg->GetLong();
			dwGoodsID = pMsg->GetLong();
			dwPrice = pMsg->GetLong();
			pMsg->GetStr(szGoodsName, 32);
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();

			switch(byType)
			{
			case 0:		//拾起
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,0)", 
							 nPlayerID, playerName,pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[从场景拾取日志]失败");

				break;
			case 1:		//NPC购得
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,1)", 
							 nPlayerID, playerName, pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[从NPC购买日志]失败");

				break;

			case 2:		// 卖给NPC
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,2)", 
							 nPlayerID, playerName,pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[卖到NPC日志]失败");
				break;

			case 3:		// 丢弃
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,3)", 
							 nPlayerID, playerName,pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[从玩家丢弃到场景日志]失败");
				break;

			case 4:		// 损坏
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,4)", 
							 nPlayerID, playerName,pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[损坏耐久后消失日志]失败");
				break;

			case 5:		// 升级失败
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,5)", 
							 nPlayerID, playerName, pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[升级物品失败后消失日志]失败");
				break;
			case 6:		// 人物死亡后掉落
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,6)", 
							 nPlayerID, playerName,pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[人物死亡后掉落日志]失败");
				break;

			case 7:		// 仓库存
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,7)", 
							 nPlayerID, playerName,pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[仓库存]失败");
				break;

			case 8:		// 仓库取
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,8)", 
							 nPlayerID, playerName,pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[仓库取]失败");
				break;

			case 9:		// 仓库存
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,9)", 
							 nPlayerID, playerName, pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[银行存]失败");
				break;

			case 10:		// 仓库存
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,10)", 
							 nPlayerID, playerName, pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[银行取]失败");
				break;

			case 11:
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,11)", 
							 nPlayerID, playerName, pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[物品拆分日志]失败");
				break;

			case 12:
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,12)", 
							 nPlayerID, playerName, pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[物品炼制日志]失败");
				break;

			case 13:
				sprintf(sql, "INSERT INTO \
							 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,'%s',%d,%d,%d,%d,13)", 
							 nPlayerID, playerName, pk_count,
							 dwCurMoney,dwCurBank,
							 dwGoodsID, szGoodsName,dwPrice,
							 dwMapID, dwPosX, dwPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[物品合成日志]失败");
				break;
			}
		}
		break;

	case CMessage::MSG_S2W_WRITELOG_GOODSUPGRADE:
		{
			BYTE byType;
			DWORD goodsID, gem1ID,gem2ID,gem3ID,gem4ID, posX, posY, mapID;
			char playerName[32],goodsName[32], gem1Name[32],gem2Name[32],gem3Name[32],gem4Name[32];
			CPlayer *pPlayer;
			byType = pMsg->GetByte();

			UINT nPlayerID = pMsg->GetLong();

			pPlayer = GetGame()->GetMapPlayer(nPlayerID);

			if(!pPlayer)
			{
				AddLogText("goods %d 日志pPlayer为NULL %d", byType, nPlayerID);
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			goodsID = pMsg->GetLong();
			pMsg->GetStr(goodsName, 32);

			gem1ID = pMsg->GetLong();
			pMsg->GetStr( gem1Name, 32);

			gem2ID = pMsg->GetLong();
			pMsg->GetStr( gem2Name, 32);

			gem3ID = pMsg->GetLong();
			pMsg->GetStr( gem3Name, 32);

			gem4ID = pMsg->GetLong();
			pMsg->GetStr( gem4Name, 32);

			mapID = pMsg->GetLong();
			posX	= pMsg->GetLong();
			posY	= pMsg->GetLong();

			switch(byType) 
			{
			case 1:
				sprintf(sql, "INSERT INTO \
							 goods_upgrade_log(player_id,player_name,goods_id,goods_name,gem1_id,gem1_name,gem2_id,gem2_name,gem3_id,gem3_name,gem4_id,gem4_name,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,'%s',%d,'%s',%d,'%s',%d,'%s',%d,'%s',%d,%d,%d,1)", 
							 nPlayerID, playerName,
							 goodsID, goodsName,
							 gem1ID, gem1Name,
							 gem2ID, gem2Name,
							 gem3ID, gem3Name,
							 gem4ID, gem4Name,
							 mapID, posX, posY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[物品升级成功日志]失败");
				break;

			case 2:
				sprintf(sql, "INSERT INTO \
							 goods_upgrade_log(player_id,player_name,goods_id,goods_name,gem1_id,gem1_name,gem2_id,gem2_name,gem3_id,gem3_name,gem4_id,gem4_name,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,'%s',%d,'%s',%d,'%s',%d,'%s',%d,'%s',%d,%d,%d,2)", 
							 nPlayerID, playerName,
							 goodsID, goodsName,
							 gem1ID, gem1Name,
							 gem2ID, gem2Name,
							 gem3ID, gem3Name,
							 gem4ID, gem4Name,
							 mapID, posX, posY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[物品升级失败日志]失败");
				break;

			}
		}
		break;

	case  CMessage::MSG_S2W_WRITELOG_GOODSGEMEXCHANGE:
		{
			CPlayer *pPlayer;
			LONG dGoodsID, sGoodsID, sGoodsAmount,mapID,posX, posY;
			char playerName[32],dGoodsName[32], sGoodsName[32];
			UINT nPlayerID = pMsg->GetLong();

			pPlayer = GetGame()->GetMapPlayer(nPlayerID);

			if(!pPlayer)
			{
				AddLogText("goods gem exchange 日志pPlayer为NULL %d", nPlayerID);
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}


			dGoodsID = pMsg->GetLong();
			pMsg->GetStr(dGoodsName, 32);

			sGoodsID = pMsg->GetLong();
			pMsg->GetStr(sGoodsName, 32);
			sGoodsAmount = pMsg->GetLong();

			mapID = pMsg->GetLong();
			posX = pMsg->GetLong();
			posY = pMsg->GetLong();

			sprintf(sql, "INSERT INTO \
						 goods_gem_exchange_log(player_id,player_name,d_gem_id,d_gem_name,s_gem_id,s_gem_name,s_gem_amount,map_id,pos_x,pos_y) \
						 VALUES(%d,'%s',%d,'%s',%d,'%s',%d,%d,%d,%d)", 
						 nPlayerID, playerName,
						 dGoodsID, dGoodsName,
						 sGoodsID, sGoodsName, sGoodsAmount,
						 mapID, posX, posY);

			if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
				AddLogText("写入[物品宝石兑换日志]失败");
		}
		break;

	case  CMessage::MSG_S2W_WRITELOG_GOODSJEWELRYMADE:
		{
			CPlayer *pPlayer;
			LONG GoodsID, MaterialID, JadeID ,JadeAmount,mapID,posX, posY;
			char playerName[32],GoodsName[32], MaterialName[32], JadeName[32];
			UINT nPlayerID = pMsg->GetLong();

			pPlayer = GetGame()->GetMapPlayer(nPlayerID);

			if(!pPlayer)
			{
				AddLogText("goods jade made 日志pPlayer为NULL %d", nPlayerID);
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			GoodsID = pMsg->GetLong();
			pMsg->GetStr(GoodsName, 32);

			MaterialID = pMsg->GetLong();
			pMsg->GetStr(MaterialName, 32);

			JadeID = pMsg->GetLong();
			pMsg->GetStr(JadeName, 32);

			JadeAmount = pMsg->GetLong();

			mapID = pMsg->GetLong();
			posX = pMsg->GetLong();
			posY = pMsg->GetLong();


			sprintf(sql, "INSERT INTO \
						 goods_jewelry_made_log(player_id,player_name,goods_id,goods_name,material_id,material_name,jade_id,jade_name,jade_amount,map_id,pos_x,pos_y) \
						 VALUES(%d,'%s',%d,'%s',%d,'%s',%d,'%s',%d,%d,%d,%d)", 
						 nPlayerID, playerName,
						 GoodsID, GoodsName,
						 MaterialID, MaterialName,
						 JadeID, JadeName, JadeAmount,
						 mapID, posX, posY);

			if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
				AddLogText("写入[物品首饰制造日志]失败");

		}
		break;


	case CMessage::MSG_S2W_WRITELOG_PLAYEREXP:
		{
			DWORD dwExp;
			DWORD dwMapID,dwPosX,dwPosY;
			BYTE byType;
			char playerName[32];
			CPlayer *pPlayer;

			byType = pMsg->GetByte();
			UINT nPlayerID = pMsg->GetLong();
			pPlayer = GetGame()->GetMapPlayer(nPlayerID);

			if(!pPlayer)
			{
				AddLogText("exp %d 日志pPlayer为NULL %d", byType, nPlayerID);
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			dwExp = pMsg->GetLong();
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();

			switch(byType)
			{
			case 0: // 升级增加经验
				sprintf(sql, "INSERT INTO \
							 player_exp_log(player_id,player_name,exp,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d)", 
							 nPlayerID, playerName,
							 dwExp,dwMapID, dwPosX, dwPosY, 0);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[获取经验]失败");

				break;

			case 1: // 被杀后掉经验
				sprintf(sql, "INSERT INTO \
							 player_exp_log(player_id,player_name,exp,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d)", 
							 nPlayerID, playerName,
							dwExp, dwMapID, dwPosX, dwPosY, 1);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[死亡丢失经验]失败");

				break;
			}
		}
		break;

	case CMessage::MSG_S2W_WRITELOG_PLAYERLEVEL:
		{
			BYTE byOldLel,byCurLel;
			DWORD dwMapID,dwPosX,dwPosY;
			DWORD dwExp;
			char playerName[32];

			long lPlayerID=pMsg->GetLong();

			CPlayer *pPlayer;
			pPlayer = GetGame()->GetMapPlayer(lPlayerID);

			if(!pPlayer)
			{
				AddLogText("level 日志pPlayer为NULL %d", lPlayerID);
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}


			dwExp = pMsg->GetDWord();
			byOldLel = pMsg->GetByte();
			byCurLel = pMsg->GetByte();
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();


            //计费
			if(pPlayer && GetGame()->GetSetup()->bUseCostSys && GetGame()->GetCostNetClient()!=NULL)
			{
				//long lRegionID=pMsg->GetLong();
				long lRegionID=pPlayer->GetRegionID();
				CMessage msg(CMessage::MSG_W2COST_COST_CHANGE);
				msg.Add((char*)pPlayer->GetAccount());
				msg.Add(lPlayerID);
				msg.Add(lRegionID);
				msg.Add(byCurLel);
				msg.Add(GetGame()->GetSetup()->dwNumber);
				msg.Add(GetGame()->GetCostNetClient()->GetCostTimePoint());
				//msg.Add(time(NULL)+8*60*60);
				msg.SendToCost();
			}


			sprintf(sql, "INSERT INTO \
						 player_level_log(player_id,player_name,exp,old_level,cur_level,map_id,pos_x,pos_y) \
						 VALUES(%d,'%s',%d,%d,%d,%d,%d,%d)", 
						 lPlayerID, playerName,
						 dwExp, byOldLel, byCurLel, dwMapID, dwPosX, dwPosY);

			if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
				AddLogText("写入[升级日志]失败");
		}
		break;

	case CMessage::MSG_S2W_WRITELOG_PLAYERDIED:
		{
			CPlayer *pPlayer;
			DWORD dwMapID,dwPosX,dwPosY;
			char playerName[32];

			UINT nPlayerID = pMsg->GetLong();
			pPlayer = GetGame()->GetMapPlayer(nPlayerID);
			
			if(!pPlayer)
			{
				AddLogText("died 日志pPlayer为NULL %d", nPlayerID);
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();


			sprintf(sql, "INSERT INTO \
						 player_died_log(player_id,player_name,map_id,pos_x,pos_y) VALUES(%d,'%s',%d,%d,%d)", 
						 nPlayerID, playerName, dwMapID, dwPosX, dwPosY);

			if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
				AddLogText("写入[死亡日志]失败");
		}
		break;

	case CMessage::MSG_S2W_WRITELOG_PLAYERKILLER:
		{

			BYTE byType;
			DWORD dwMapID,dwPosX,dwPosY;
			char playerName[32], murdererName[32];
			CPlayer *pPlayer, *pMurderer;
			UINT nPlayerID, nMurdererID;
			
			byType = pMsg->GetByte();

			nPlayerID = pMsg->GetLong();
			nMurdererID = pMsg->GetLong();

			pPlayer = GetGame()->GetMapPlayer(nPlayerID);
			pMurderer = GetGame()->GetMapPlayer( nMurdererID );
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();

			if( !pPlayer || !pMurderer )
			{
				AddLogText("kill %d 日志pPlayer为NULL %d %d", byType, nPlayerID, nMurdererID);
			}

			if(pPlayer) {
				strcpy(playerName, pPlayer->GetName());
			}
			else {
				strcpy(playerName, "NULL");
			}

			
			if(pMurderer) {
				strcpy(murdererName, pMurderer->GetName());
			}
			else {
				strcpy(murdererName, "NULL");
			}


			switch(byType) 
			{
			case 0:
				sprintf(sql, "INSERT INTO \
							 player_killer_log(player_id,player_name,murderer_id,murderer_name,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,'%s',%d,%d,%d,%d)", 
							 nPlayerID, playerName, nMurdererID, murdererName,
							 dwMapID, dwPosX, dwPosY, 0);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[玩家PK日志]失败");
				break;

			case 1:
				sprintf(sql, "INSERT INTO \
							 player_killer_log(player_id,player_name,murderer_id,murderer_name,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,'%s',%d,%d,%d,%d)", 
							 nPlayerID, playerName, nMurdererID, murdererName,
							 dwMapID, dwPosX, dwPosY, 1);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[玩家PK被杀死日志]失败");
				break;
			default:
				return;
			}

		}
		break;

	case CMessage::MSG_S2W_WRITELOG_TEAM:
		{
			BYTE byType;
			DWORD dwMapID,dwPosX, dwPosY;
			char playerName[32], captainName[32];
			CPlayer *pCaptain, *pPlayer;
			UINT nCaptainID, nPlayerID;

			byType = pMsg->GetByte();


			nCaptainID = pMsg->GetLong();
			nPlayerID = pMsg->GetLong();
			pCaptain = GetGame()->GetMapPlayer( nCaptainID );
			pPlayer = GetGame()->GetMapPlayer( nPlayerID );


			if( !pCaptain || !pPlayer )
			{
				AddLogText("team %d 日志pPlayer为NULL %d %d",byType, nCaptainID, nPlayerID);

			}

			if(pPlayer) {
				strcpy(playerName, pPlayer->GetName());
			}
			else {
				strcpy(playerName, "NULL");
			}

			
			if(pCaptain) {
				strcpy(captainName, pCaptain->GetName());
			}
			else {
				strcpy(captainName, "NULL");
			}


			dwMapID = pMsg->GetLong();
			dwPosX = pMsg->GetLong();
			dwPosY = pMsg->GetLong();


			switch( byType ) 
			{
			case 0:
				sprintf(sql, "INSERT INTO team_log(captain_id,captain_name,player_id,player_name,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,'%s',%d,%d,%d,0)",
							 nCaptainID, captainName,
							 nPlayerID, playerName,
							 dwMapID, dwPosY, dwPosY );

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[组建队伍日志]失败");

				break;

			case 1:
				sprintf(sql, "INSERT INTO team_log(captain_id,captain_name,player_id,player_name,map_id,pos_x,pos_y,log_type) \
							 VALUES(%d,'%s',%d,'%s',%d,%d,%d,1)",
							 nCaptainID, captainName,
							 nPlayerID, playerName,
							 dwMapID, dwPosY, dwPosY );

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[离开队伍日志]失败");

				break;

			}
		}
		break;

	case CMessage::MSG_S2W_WRITELOG_CHAT:
		{
			BYTE byType;
			DWORD nSenderID,dwMapID, dwPosX, dwPosY;
			char senderName[32];
			CPlayer *pSender;
			char szContent[512];
			char szFixedContent[1024];

			byType = pMsg->GetByte();
			
			nSenderID = pMsg->GetDWord();
			pSender = GetGame()->GetMapPlayer(nSenderID);
			if(!pSender)
			{
				AddLogText("chat %d 日志pPlayer为NULL %d", byType, nSenderID);
				strcpy(senderName, "NULL");
			}
			else {
				strcpy(senderName, pSender->GetName());
			}


			dwMapID = pMsg->GetLong();
			dwPosX = pMsg->GetLong();
			dwPosY = pMsg->GetLong();

			szContent[0] = 0;
			pMsg->GetStr(szContent, 512);
			// 内容为空
			if( !strlen(szContent) )
			{
				return;
			}

			switch( byType ) 
			{
			case 0:
				*szFixedContent = '\0';
				sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,0,'<屏幕>','%s',0)",
							 nSenderID,senderName, 
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[屏幕聊天日志]失败");
				break;

			case 1:
				*szFixedContent = '\0';
				sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,0,'<区域>','%s',1)",
							 nSenderID,senderName, 
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[区域聊天日志]失败");
				break;

			case 2:
				// 帮会
				break;
			case 3:
				// 联盟
				break;

			case 4:
				*szFixedContent = '\0';
				sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,0,'<队伍>','%s',4)",
							 nSenderID,senderName, 
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[队伍聊天日志]失败2");
				break;

			case 5:
				{
					DWORD nReceiverID;
					char receiverName[32];
					CPlayer *pReceiver;
					nReceiverID = pMsg->GetDWord();
					pReceiver = GetGame()->GetMapPlayer(nReceiverID);
					if(!pReceiver)
					{
						AddLogText("chat密语 日志pPlayer为NULL %d", nReceiverID);
						strcpy(receiverName, "NULL");
					}
					else {
						strcpy(receiverName, pReceiver->GetName());
					}

					*szFixedContent = '\0';
					sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
								 VALUES(%d,'%s',%d,%d,%d,%d,'%s','%s',5)",
								 nSenderID,senderName, 
								 dwMapID, dwPosX, dwPosY,
								 nReceiverID, receiverName,
								 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );

					if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
						AddLogText("写入[密语日志2]失败");
				}
				break;

			case 6:
				*szFixedContent = '\0';
				sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,0,'<GM指令>','%s',6)",
							 nSenderID,senderName,
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[GM指令日志]失败");
				break;
			}

		}
		break;

		
	case CMessage::MSG_S2W_WRITELOG_CHANGEMAP:
		{
			BYTE byType;
			char playerName[32];
			CPlayer *pPlayer;
			DWORD dwPlayerID,dwMoney,dwBank,dwSMapID,dwSPosX,dwSPosY,dwDMapID,dwDPosX,dwDPosY;

			byType = pMsg->GetByte();

			dwPlayerID = pMsg->GetLong();

			pPlayer = GetGame()->GetMapPlayer(dwPlayerID);
			if(!pPlayer)
			{
				AddLogText("changemap %d 日志pPlayer为NULL %d", byType, dwPlayerID);
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}


			dwMoney = pMsg->GetDWord();
			dwBank = pMsg->GetDWord();
			
			dwSMapID = pMsg->GetLong();
			dwSPosX = pMsg->GetLong();
			dwSPosY = pMsg->GetLong();

			dwDMapID = pMsg->GetLong();
			dwDPosX = pMsg->GetLong();
			dwDPosY = pMsg->GetLong();

			switch( byType ) 
			{
			case 0:
				sprintf(sql, "INSERT INTO change_map_log(player_id,player_name,money,bank,s_map_id,s_pos_x,s_pos_y,d_map_id,d_pos_x,d_pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,0)",
							 dwPlayerID,playerName, dwMoney, dwBank,
							 dwSMapID, dwSPosX, dwSPosY, dwDMapID, dwDPosX, dwDPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[场景内切换日志]失败");
				break;

			case 1:
				sprintf(sql, "INSERT INTO change_map_log(player_id,player_name,money,bank,s_map_id,s_pos_x,s_pos_y,d_map_id,d_pos_x,d_pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,1)",
							 dwPlayerID,playerName, dwMoney, dwBank,
							 dwSMapID, dwSPosX, dwSPosY, dwDMapID, dwDPosX, dwDPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[服务器内切换日志]失败");
				break;

			case 2:
				sprintf(sql, "INSERT INTO change_map_log(player_id,player_name,money,bank,s_map_id,s_pos_x,s_pos_y,d_map_id,d_pos_x,d_pos_y,log_type) \
							 VALUES(%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,2)",
							 dwPlayerID,playerName, dwMoney, dwBank,
							 dwSMapID, dwSPosX, dwSPosY, dwDMapID, dwDPosX, dwDPosY);

				if( !CMyAdoBase::ExecuteCn(sql, GetGame()->m_cnLogSys))
					AddLogText("写入[服务器间切换日志]失败");
				break;
			}

		}
		break;
	}
}