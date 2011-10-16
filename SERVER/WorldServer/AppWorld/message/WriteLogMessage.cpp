#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\..\..\..\DBAccess\MyAdoBase.h"
#include "..\nets\Servers.h"
#include "..\IncrementLog\IncrementLog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应日志消息
void OnWriteLogMessage(CMessage* pMsg)
{
	//DWORD dwPlayerID;
	char *sql = new char[1024];
	memset(sql, 0,1024);

	switch(pMsg->GetType())
	{
	case MSG_S2W_WRITELOG_GOODSTRADE:// 接受GAMESERVER发的物品交易日志信息
		{
			BYTE byType;
			DWORD amount;
			CGUID goodsid;
			char GoodsID[40];
			DWORD dwPrice,dwPlayerCurMoney[2];
			CGUID PlayerGuid[2];
			char szGoodsName[32],sellerName[32],purchaserName[32];
			DWORD dwMapID[2],dwPosX[2],dwPosY[2];
			CPlayer *pSeller, *pPurchaser;

			byType = pMsg->GetByte();
			pMsg->GetGUID(PlayerGuid[0]);
			dwPlayerCurMoney[0] = pMsg->GetDWord();
			dwMapID[0] = pMsg->GetDWord();
			dwPosX[0]	= pMsg->GetDWord();
			dwPosY[0]	= pMsg->GetDWord();

			pMsg->GetGUID(PlayerGuid[1]);
			dwPlayerCurMoney[1] = pMsg->GetDWord();
			dwMapID[1] = pMsg->GetDWord();
			dwPosX[1]	= pMsg->GetDWord();
			dwPosY[1]	= pMsg->GetDWord();

			pSeller = GetGame()->GetMapPlayer(PlayerGuid[0]);
			pPurchaser = GetGame()->GetMapPlayer(PlayerGuid[1]);

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

			pMsg->GetGUID(goodsid);
			goodsid.tostring(GoodsID);
			dwPrice = pMsg->GetDWord();
			amount = pMsg->GetDWord();

			pMsg->GetStr(szGoodsName, 32);
			
			char szPlayerGuid1[40] = {0};
			char szPlayerGuid2[40] = {0};
			PlayerGuid[0].tostring(szPlayerGuid1);
			PlayerGuid[1].tostring(szPlayerGuid2);

			sprintf(sql, "INSERT INTO \
						 goods_trade_log(seller_id,seller_name,seller_cur_money,s_map_id,s_pos_x,s_pos_y,purchaser_id,purchaser_name,purchaser_cur_money,p_map_id,p_pos_x,p_pos_y,goods_id,goods_name,price,amount,log_type) \
						 VALUES(%s,'%s',%d,%d,%d,%d,%s,'%s',%d,%d,%d,%d,'%s','%s',%d,%d,%d)", 
						 szPlayerGuid1, sellerName,dwPlayerCurMoney[0], dwMapID[0], dwPosX[0], dwPosY[0],
						 szPlayerGuid2, purchaserName, dwPlayerCurMoney[1], dwMapID[1], dwPosX[1], dwPosY[1],
						 GoodsID, szGoodsName,dwPrice,amount,byType);

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);

		}
		break;

	case MSG_S2W_WRITELOG_GOODS:// 接受GAMESERVER发的一般物品日志信息
		{
			BYTE byType;
			DWORD dwPrice,dwMapID,dwPosX,dwPosY,dwCurMoney,dwCurBank;
			CGUID goodsid;
			char GoodsID[40];
			char szGoodsName[32],playerName[32];
			CPlayer *pPlayer;

			byType = pMsg->GetByte();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			
			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);

			if(!pPlayer)
			{
				strcpy(playerName, "NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			WORD pk_count = pMsg->GetWord();
			dwCurMoney = pMsg->GetLong();
			dwCurBank = pMsg->GetLong();
			//dwGoodsID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(GoodsID);
			dwPrice = pMsg->GetLong();
			pMsg->GetStr(szGoodsName, 32);
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO \
						 goods_log(player_id,player_name,pk_count,cur_money,cur_bank,goods_id,goods_name,price,map_id,pos_x,pos_y,log_type) \
						 VALUES(%s,'%s',%d,%d,%d,'%s','%s',%d,%d,%d,%d,%d)", 
						 szPlayerGuid, playerName,pk_count,
						 dwCurMoney,dwCurBank,
						 GoodsID, szGoodsName,dwPrice,
						 dwMapID, dwPosX, dwPosY,byType);


			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case MSG_S2W_WRITELOG_GOODSUPGRADE:
		{
			BYTE byType;
			DWORD posX, posY, mapID;
			CGUID goodsid;
			char GoodsID[40],gem1ID[40],gem2ID[40],gem3ID[40],gem4ID[40];
			char playerName[32],goodsName[32], gem1Name[32],gem2Name[32],gem3Name[32],gem4Name[32];
			CPlayer *pPlayer;
			byType = pMsg->GetByte();

			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);

			if(!pPlayer)
			{
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			//goodsID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(GoodsID);
			pMsg->GetStr(goodsName, 32);

			//gem1ID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(gem1ID);
			pMsg->GetStr( gem1Name, 32);

			//gem2ID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(gem2ID);
			pMsg->GetStr( gem2Name, 32);

			//gem3ID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(gem3ID);
			pMsg->GetStr( gem3Name, 32);

			//gem4ID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(gem4ID);
			pMsg->GetStr( gem4Name, 32);

			mapID = pMsg->GetLong();
			posX	= pMsg->GetLong();
			posY	= pMsg->GetLong();

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO \
						 goods_upgrade_log(player_id,player_name,goods_id,goods_name,gem1_id,gem1_name,gem2_id,gem2_name,gem3_id,gem3_name,gem4_id,gem4_name,map_id,pos_x,pos_y,log_type) \
						 VALUES(%s,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d,%d)", 
						 szPlayerGuid, playerName,
						 GoodsID, goodsName,
						 gem1ID, gem1Name,
						 gem2ID, gem2Name,
						 gem3ID, gem3Name,
						 gem4ID, gem4Name,
						 mapID, posX, posY, byType);

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case  MSG_S2W_WRITELOG_GOODSGEMEXCHANGE:
		{
			CPlayer *pPlayer;
			LONG  sGoodsAmount,mapID,posX, posY;
			CGUID goodsid;
			char dGoodsID[40], sGoodsID[40];
			char playerName[32],dGoodsName[32], sGoodsName[32];
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);

			if(!pPlayer)
			{
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}


			//dGoodsID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(dGoodsID);
			pMsg->GetStr(dGoodsName, 32);

			//sGoodsID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(sGoodsID);
			pMsg->GetStr(sGoodsName, 32);

			sGoodsAmount = pMsg->GetLong();

			mapID = pMsg->GetLong();
			posX = pMsg->GetLong();
			posY = pMsg->GetLong();

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO \
						 goods_gem_exchange_log(player_id,player_name,d_gem_id,d_gem_name,s_gem_id,s_gem_name,s_gem_amount,map_id,pos_x,pos_y) \
						 VALUES(%s,'%s','%s','%s','%s','%s',%d,%d,%d,%d)", 
						 szPlayerGuid, playerName,
						 dGoodsID, dGoodsName,
						 sGoodsID, sGoodsName, sGoodsAmount,
						 mapID, posX, posY);

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case  MSG_S2W_WRITELOG_GOODSJEWELRYMADE:
		{
			CPlayer *pPlayer;
			LONG JadeAmount,mapID,posX, posY;
			CGUID goodsid;
			char GoodsID[40], MaterialID[40], JadeID[40];
			char playerName[32],GoodsName[32], MaterialName[32], JadeName[32];
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);

			if(!pPlayer)
			{
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			//GoodsID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(GoodsID);
			pMsg->GetStr(GoodsName, 32);

			//MaterialID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(MaterialID);
			pMsg->GetStr(MaterialName, 32);

			//JadeID = pMsg->GetLong();
			pMsg->GetGUID(goodsid);
			goodsid.tostring(JadeID);
			pMsg->GetStr(JadeName, 32);

			JadeAmount = pMsg->GetLong();

			mapID = pMsg->GetLong();
			posX = pMsg->GetLong();
			posY = pMsg->GetLong();

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO \
						 goods_jewelry_made_log(player_id,player_name,goods_id,goods_name,material_id,material_name,jade_id,jade_name,jade_amount,map_id,pos_x,pos_y) \
						 VALUES(%s,'%s','%s','%s','%s','%s','%s','%s',%d,%d,%d,%d)", 
						 szPlayerGuid, playerName,
						 GoodsID, GoodsName,
						 MaterialID, MaterialName,
						 JadeID, JadeName, JadeAmount,
						 mapID, posX, posY);

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;


	case MSG_S2W_WRITELOG_PLAYEREXP:
		{
			DWORD dwExp;
			DWORD dwMapID,dwPosX,dwPosY;
			BYTE byType;
			char playerName[32];
			CPlayer *pPlayer;

			byType = pMsg->GetByte();
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);

			if(!pPlayer)
			{
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}

			dwExp = pMsg->GetLong();
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO \
						 player_exp_log(player_id,player_name,exp,map_id,pos_x,pos_y,log_type) \
						 VALUES(%s,'%s',%d,%d,%d,%d,%d)", 
						 szPlayerGuid, playerName,
						 dwExp,dwMapID, dwPosX, dwPosY, byType);


			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case MSG_S2W_WRITELOG_PLAYERLEVEL:
		{
			BYTE byOldLel,byCurLel;
			DWORD dwMapID,dwPosX,dwPosY;
			DWORD dwExp;
			char playerName[32];

			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer;
			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);

			if(!pPlayer)
			{
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


			// 传2不需要发计费信息 bugs 2006.06.05
            //计费
			//if(pPlayer && GetGame()->GetSetup()->bUseCostSys && GetGame()->GetCostNetClient()!=NULL)
			//{
			//	//long lRegionID=pMsg->GetLong();
			//	long lRegionID=pPlayer->GetRegionID();
			//	CMessage msg(MSG_W2COST_COST_CHANGE);
			//	msg.Add(pPlayer->GetAccount());
			//	msg.Add(lPlayerID);
			//	msg.Add(lRegionID);
			//	msg.Add(byCurLel);
			//	msg.Add(GetGame()->GetSetup()->dwNumber);
			//	msg.Add(GetGame()->GetCostNetClient()->GetCostTimePoint());
			//	//msg.Add(time(NULL)+8*60*60);
			//	msg.SendToCost();
			//}

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO \
						 player_level_log(player_id,player_name,exp,old_level,cur_level,map_id,pos_x,pos_y) \
						 VALUES(%s,'%s',%d,%d,%d,%d,%d,%d)", 
						 szPlayerGuid, playerName,
						 dwExp, byOldLel, byCurLel, dwMapID, dwPosX, dwPosY);

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case MSG_S2W_WRITELOG_PLAYERDIED:
		{
			CPlayer *pPlayer;
			DWORD dwMapID,dwPosX,dwPosY;
			char playerName[32];

			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			
			if(!pPlayer)
			{
				strcpy(playerName,"NULL");
			}
			else {
				strcpy(playerName, pPlayer->GetName());
			}
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO \
						 player_died_log(player_id,player_name,map_id,pos_x,pos_y) VALUES(%s,'%s',%d,%d,%d)", 
						 szPlayerGuid, playerName, dwMapID, dwPosX, dwPosY);

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case MSG_S2W_WRITELOG_PLAYERKILLER:
		{

			BYTE byType;
			DWORD dwMapID,dwPosX,dwPosY;
			char playerName[32], murdererName[32];
			CPlayer *pPlayer, *pMurderer;
			
			byType = pMsg->GetByte();

			CGUID PlayerGuid, MurdererID;
			pMsg->GetGUID(PlayerGuid);
			pMsg->GetGUID(MurdererID);

			pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
			pMurderer = GetGame()->GetMapPlayer( MurdererID );
			dwMapID = pMsg->GetLong();
			dwPosX	= pMsg->GetLong();
			dwPosY	= pMsg->GetLong();


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

			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			char szMurdererID[40] = {0};
			MurdererID.tostring(szMurdererID);
			sprintf(sql, "INSERT INTO \
						 player_killer_log(player_id,player_name,murderer_id,murderer_name,map_id,pos_x,pos_y,log_type) \
						 VALUES(%s,'%s',%s,'%s',%d,%d,%d,%d)", 
						 szPlayerGuid, playerName, szMurdererID, murdererName,
						 dwMapID, dwPosX, dwPosY, byType);

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);

		}
		break;

	case MSG_S2W_WRITELOG_TEAM:
		{
			BYTE byType;
			DWORD dwMapID,dwPosX, dwPosY;
			char playerName[32], captainName[32];
			CPlayer *pCaptain, *pPlayer;

			byType = pMsg->GetByte();

			CGUID CaptainID, PlayerGuid;
			pMsg->GetGUID(CaptainID);
			pMsg->GetGUID(PlayerGuid);

			pCaptain = GetGame()->GetMapPlayer( CaptainID );
			pPlayer = GetGame()->GetMapPlayer( PlayerGuid );

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

			char szCaptainID[40] = {0};
			CaptainID.tostring(szCaptainID);
			char szPlayerGuid[40] = {0};
			PlayerGuid.tostring(szPlayerGuid);
			sprintf(sql, "INSERT INTO team_log(captain_id,captain_name,player_id,player_name,map_id,pos_x,pos_y,log_type) \
						 VALUES(%s,'%s',%s,'%s',%d,%d,%d,%d)",
						 szCaptainID, captainName,
						 szPlayerGuid, playerName,
						 dwMapID, dwPosY, dwPosY, byType );

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case MSG_S2W_WRITELOG_CHAT:
		{
			BYTE byType;
			DWORD dwMapID, dwPosX, dwPosY;
			char senderName[32];
			CPlayer *pSender;
			char szContent[512];
			char szFixedContent[1024];

			byType = pMsg->GetByte();
			
			CGUID SenderID;
			pMsg->GetGUID(SenderID);
			pSender = GetGame()->GetMapPlayer(SenderID);
			if(!pSender)
			{
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

			char szSenderID[40] = {0};
			SenderID.tostring(szSenderID);
			switch( byType ) 
			{
			case 0:
				*szFixedContent = '\0';
		/*		sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%s,'%s',%d,%d,%d,0,'<屏幕>','%s',0)",
							 szSenderID,senderName, 
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );*/
				break;

			case 1:
				*szFixedContent = '\0';
			/*	sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%s,'%s',%d,%d,%d,0,'<区域>','%s',1)",
							 szSenderID,senderName, 
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );*/
				break;

			case 2:
				// 帮会
				break;
			case 3:
				// 联盟
				break;

			case 4:
				*szFixedContent = '\0';
	/*			sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%s,'%s',%d,%d,%d,0,'<队伍>','%s',4)",
							 szSenderID,senderName, 
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );*/
				break;

			case 5:
				{
					char receiverName[32];
					CPlayer *pReceiver;
					CGUID ReceiverID;
					pMsg->GetGUID(ReceiverID);
					pReceiver = GetGame()->GetMapPlayer(ReceiverID);
					if(!pReceiver)
					{
						strcpy(receiverName, "NULL");
					}
					else {
						strcpy(receiverName, pReceiver->GetName());
					}

					*szFixedContent = '\0';
					char szReceiverID[40];
					ReceiverID.tostring(szReceiverID);
			/*		sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
								 VALUES(%s,'%s',%d,%d,%d,%s,'%s','%s',5)",
								 szSenderID,senderName, 
								 dwMapID, dwPosX, dwPosY,
								 szReceiverID, receiverName,
								 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );*/
				}
				break;

			case 6:
				*szFixedContent = '\0';
		/*		sprintf(sql, "INSERT INTO chat_log(sender_id,sender_name,map_id,pos_x,pos_y,receiver_id,receiver_name,content,log_type) \
							 VALUES(%s,'%s',%d,%d,%d,0,'<GM指令>','%s',6)",
							 szSenderID,senderName,
							 dwMapID, dwPosX, dwPosY,
							 CMyAdoBase::FixSingleQuotes(szContent, szFixedContent) );*/
				break;
			}

			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);

		}
		break;

		
	case MSG_S2W_WRITELOG_CHANGEMAP:
		{
			BYTE byType;
			char playerName[32];
			CPlayer *pPlayer;
			DWORD dwMoney,dwBank,dwSMapID,dwSPosX,dwSPosY,dwDMapID,dwDPosX,dwDPosY;

			byType = pMsg->GetByte();

			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);

			pPlayer = GetGame()->GetMapPlayer(PlayerID);
			if(!pPlayer)
			{
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


			char szPlayerID[40] = {0};
			PlayerID.tostring(szPlayerID);
			sprintf(sql, "INSERT INTO change_map_log(player_id,player_name,money,bank,s_map_id,s_pos_x,s_pos_y,d_map_id,d_pos_x,d_pos_y,log_type) \
						 VALUES(%s,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d)",
						 szPlayerID,playerName, dwMoney, dwBank,
						 dwSMapID, dwSPosX, dwSPosY, dwDMapID, dwDPosX, dwDPosY, byType);


			GetGame()->GetWriteLogDataQueue()->PushWriteLogData(sql);
		}
		break;

	case MSG_S2W_WRITELOG_INCREMENT:
		{
			// 0商店购买(-) 1充值(+)  2交易给(-) 3交易得(+) 4交易买(-) 5交易卖(+) 6偿还(-)

			BYTE byType;
			char szDesc[256];
			char szAcc[32];
			CPlayer *pPlayer;
			DWORD dwMoney;

			byType = pMsg->GetByte();
			dwMoney = pMsg->GetLong();
			pMsg->GetStr(szDesc, 256);
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);

			pPlayer = GetGame()->GetMapPlayer(PlayerID);
			if(!pPlayer)
			{
				strcpy(szAcc,"NULL");
			}
			else
			{
				strcpy(szAcc, pPlayer->GetAccount());
			}

			SYSTEMTIME now;
			GetSystemTime(&now);

			char szPlayerID[40] = {0};
			PlayerID.tostring(szPlayerID);
			
			// 发送消息到DBS保存
			CMessage msg(MSG_W2DB_PSASS_OTHER_WRITELOG);
			msg.Add(PlayerID);//player_id
			msg.Add(byType);//type
			msg.Add(dwMoney);//money
			msg.Add(szDesc);//description
			msg.Add(szAcc);//player_acc
			msg.AddEx(&now, sizeof(SYSTEMTIME)); // log time 
			msg.SendToSocket(GetGame()->GetDBSocketID());
			CIncrementLog::GetInstance()->Add(PlayerID,&now, byType, dwMoney, szDesc);
		}
		break;
	}
}
//