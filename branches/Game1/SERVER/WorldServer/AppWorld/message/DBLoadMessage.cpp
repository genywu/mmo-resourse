
#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\goods\cgoods.h"
#include "..\..\worldserver\game.h"
#include "..\cardsdealingsystem\Card.h"
#include "..\cardsdealingsystem\CardsHandler.h"

#include "..\Country\Country.h"
#include "..\Country\CountryHandler.h"
#include "..\organizingsystem\FactionWarSys.h"
#include "..\organizingsystem\Faction.h"
#include "..\organizingsystem\union.h"
#include "..\organizingsystem\VillageWarSys.h"
#include "..\organizingsystem\OrganizingCtrl.h"
#include "..\organizingsystem\AttackCitySys.h"
#include "..\player.h"
#include "../GlobalRgnManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnDBLoadMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_DB2W_LOAD_REGION_PARAM:
		{
			long msgTimerID = pMsg->GetLong();
			long regID = pMsg->GetLong();
			//CGame::tagRegion* reg = GetGame()->GetRegion(regID); // RegionID
			CGlobalRgnManager::tagRegion* reg = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(regID);

			if(reg->pRegion)
			{
				CGUID tGuid;
				pMsg->GetGUID(tGuid);
				CGUID tGuid1;
				pMsg->GetGUID(tGuid1);
				reg->pRegion->SetParamFromDB(
					tGuid,		//OwnedFactionID
					tGuid1,		//OwnedUnionID
					pMsg->GetLong(),		//CurTaxRate
					pMsg->GetDWord(),		//TodayTotalTax
					pMsg->GetDWord());		//TotalTax
			}

			// 删除定时器
			GetGame()->DelMsgTimer(msgTimerID);

			AddLogText("从DBS读取RegionParam数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_PLAYER_WORLD_ID:
		{
			long msgTimerID = pMsg->GetLong();
			CGUID playerID;
			pMsg->GetGUID(playerID);
			long worldID  = pMsg->GetLong();
			GetGame()->SetDBPlayerID(playerID);
			GetGame()->SetDBLeaveWordID(worldID);

			// 删除定时器
			GetGame()->DelMsgTimer(msgTimerID);
			AddLogText("从DBS读取Player WorldID数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_INIT_ENEMY_FACTION:
		{
			long msgTimerID = pMsg->GetLong();
			long num = pMsg->GetLong(); // 个数
		
			for(int i=0; i < num; i++)
			{
				CGUID tGuid, tGuid1;
				pMsg->GetGUID(tGuid);
				pMsg->GetGUID(tGuid1);
				GetFactionWarSys()->AddOneEnmeyFaction(tGuid,
					tGuid1, pMsg->GetLong());
			}

			GetFactionWarSys()->Initialize();
			GetGame()->b_HasGetEnemyFactionMsgFlag = true;

			// 删除定时器
			GetGame()->DelMsgTimer(msgTimerID);

			AddLogText("从DBS读取Enemy factions数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_INIT_COUNTRY:
		{
			BYTE *pBA = new BYTE[102400];
			pMsg->GetEx(pBA, 102400);
			long pos = 0;
			long msgTimerID = _GetLongFromByteArray(pBA, pos);
			GetCountryHandler()->DecordFromByteArray(pBA, pos);
			GetGame()->b_HasGetCountryMsgFlag = true;

			// 删除定时器
			GetGame()->DelMsgTimer(msgTimerID);
			SAFE_DELETE_ARRAY(pBA);

			AddLogText("从DBS读取Init Country数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_INIT_PLAYER:
		{
			BYTE *pData = new BYTE[102400];
			pMsg->GetEx(pData,102400);

			long lPtr = 0;
			long msgTimerID = _GetLongFromByteArray(pData, lPtr);
			CGUID id;
			_GetBufferFromByteArray(pData, lPtr, id);
			char CDKey[64];
			_GetStringFromByteArray(pData, lPtr, CDKey);
			
			//if(player)
//			{
				// 将该ID的Player放入InitPlayerID队列,如果成功处理DBS反馈MSG_DB2W_INIT_LOAD_PLAYER消息则弹出
//				CGame::InitPlayerIDQueueItr itr = std::find(GetGame()->GetInitPlayerIDQueue()->begin(), 
//															GetGame()->GetInitPlayerIDQueue()->end(), id);
//				if(itr != GetGame()->GetInitPlayerIDQueue()->end())
			//	{
			//		CPlayer* player = GetGame()->GetMapPlayer(id);
			//		if(NULL == player)
			//		{
			//			player = new CPlayer;
			//			player->SetExID(id);
			//			GetGame()->AppendMapPlayer(player);
			//		}
			//		
			//		// 解消息
			//		player->DecordFromByteArray(pData, lPtr);
			//		GetGame()->GetInitPlayerIDQueue()->erase(itr);
			//		// 初始化玩家数据
			//		GetGame()->InitPlayerInfo(player, CDKey);

			//		// 初始化
			//		player->SetExID(id);
			//		player->SetAccount(CDKey);
			//	}
			//	else // 未找到
			//	{
			//		char str[256];
			//		char szGuid[128];
			//		id.tostring(szGuid);
			//		sprintf(str, "从数据库装载玩家数据出错, WS未找到创建列表中的Player项(ID:%s)", szGuid);
			//		PutStringToFile("debug-DB", str);
			//		AddErrorLogText(str);
			//	}				
			//}

			//// 删除定时器
			//GetGame()->DelMsgTimer(msgTimerID);

			//SAFE_DELETE_ARRAY(pData);
			//AddLogText("从DBS读取Init Player数据消息...OK！");

		}
		break;
	case MSG_DB2W_LOAD_PLAYER:
		{
			BYTE *pBuf = new BYTE[102400];
			pMsg->GetEx(pBuf, 102400);
			long pos = 0;
			long msgTimerID = _GetLongFromByteArray(pBuf, pos);
			CGUID id;
			_GetBufferFromByteArray(pBuf, pos, id);
			CPlayer* player = GetGame()->GetMapPlayer(id);
			if(player)
			{
				player->DecordFromByteArray(pBuf, pos);
			}
			// 删除定时器
			GetGame()->DelMsgTimer(msgTimerID);
			SAFE_DELETE_ARRAY(pBuf);
			AddLogText("从DBS读取Player数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_COUNTRY:
		{
			long msgTimerID = pMsg->GetLong();
			CCountry* country = new CCountry;
			long cNum = pMsg->GetLong();
			for(int i=0; i < cNum; i++)
			{
				CGUID tGuid;
				
				country->SetID(pMsg->GetLong());
				country->SetPower(pMsg->GetLong());
				pMsg->GetGUID(tGuid);
				country->SetKing(tGuid);
				country->SetTechLevel(pMsg->GetLong());
				GetCountryHandler()->Append(country);
			}
			// 删除定时器
			GetGame()->DelMsgTimer(msgTimerID);
			AddLogText("从DBS读取Country数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_CARDS_LARGESS:
		{
			long msgid = pMsg->GetLong();
			long cardNum = pMsg->GetLong();
			char buf[128];
			for(int i = 0; i < cardNum; i++)
			{
				CCard *pCard = new CCard;

				if( !pCard) 
				{
					return;
				}


				pMsg->GetStr(buf, 128);
				pCard->SetCardNumber( buf );

				pMsg->GetStr(buf, 128);
				pCard->SetCardType( buf );

				pMsg->GetStr(buf, 128);
				pCard->SetSellerAccount(buf);

				CGUID tGuid;
				pMsg->GetGUID(tGuid);
				pCard->SetSellerID( tGuid );

				pMsg->GetStr(buf, 128);
				pCard->SetBuyerAccount(buf);

				pMsg->GetGUID(tGuid);
				pCard->SetBuyerID( tGuid );

				pCard->SetPrice( pMsg->GetLong() );
				pCard->SetChargePrice( pMsg->GetLong() );
				pCard->SetCardState((CCard::eCardState)(long)pMsg->GetLong());
				pCard->SetCardTime( (CCard::eCardTime)(long)pMsg->GetLong() );
				pCard->SetStartTime( pMsg->GetDWord() );
				pCard->SetSerialNumber( pMsg->GetDWord() );

				// 返回0为真。
				if( CCardsHandler::getInstance()->InsertCard(pCard) )
				{
					return;
				}
			}

			// 删除定时器
			GetGame()->DelMsgTimer(msgid);

			AddLogText("从DBS读取cards largess数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_ALL_CITY_WAR_PARAM:
		{
			BYTE *pBuf = new BYTE[102400];
			pMsg->GetEx(pBuf, 102400);

			long pos = 0;
			long msgid = _GetLongFromByteArray(pBuf, pos);
			long tNum = _GetLongFromByteArray(pBuf, pos);

			list<CGUID> warFactions;
			for(int i = 0; i < tNum; i++)
			{
				warFactions.clear();
				long lCityRegionID = _GetLongFromByteArray(pBuf, pos);
				tagTime AttackCityStartTime;
				_GetBufferFromByteArray(pBuf, pos, &AttackCityStartTime, sizeof(tagTime));

				long warNum = _GetLongFromByteArray(pBuf, pos);
				for(int j = 0; j < warNum; j++)
				{
					CGUID tGuid;
					_GetBufferFromByteArray(pBuf, pos, tGuid);
					warFactions.push_back(tGuid);
				}

				GetAttackCitySys()->SetDecWarFactionsFromDB(lCityRegionID, AttackCityStartTime, warFactions);
			}

			// 删除定时器
			GetGame()->DelMsgTimer(msgid);
			SAFE_DELETE_ARRAY(pBuf);

			AddLogText("从DBS读取cities war param数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_ALL_VILLAGE_WAR_PARAM:
		{
			BYTE *pBuf = new BYTE[102400];
			pMsg->GetEx(pBuf, 102400);

			long pos = 0;
			long msgid = _GetLongFromByteArray(pBuf, pos);
			long tNum = _GetLongFromByteArray(pBuf, pos);

			list<CGUID> warFactions;
			for(int i = 0; i < tNum; i++)
			{
				warFactions.clear();
				long lCityRegionID = _GetLongFromByteArray(pBuf, pos);
				tagTime AttackCityStartTime;
				_GetBufferFromByteArray(pBuf, pos, &AttackCityStartTime, sizeof(tagTime));

				long warNum = _GetLongFromByteArray(pBuf, pos);
				for(int j = 0; j < warNum; j++)
				{
					CGUID tGuid;
					_GetBufferFromByteArray(pBuf, pos, tGuid);
					warFactions.push_back(tGuid);
				}

				GetVilWarSys()->SetDecWarFactionsFromDB(lCityRegionID, AttackCityStartTime, warFactions);
			}

			// 删除定时器
			GetGame()->DelMsgTimer(msgid);

			SAFE_DELETE_ARRAY(pBuf);

			AddLogText("从DBS读取villages war param数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_ALL_ENEMY_FACTIONS:
		{
			long msgid = pMsg->GetLong();
			long num = pMsg->GetLong();
			
			CGUID lFactionID1 = NULL_GUID;
			CGUID lFactionID2 = NULL_GUID;
			DWORD dwLeaveTime  = 0;
		
			for(int i = 0; i < num; i++)
			{
				pMsg->GetGUID(lFactionID1);
				pMsg->GetGUID(lFactionID2);
				dwLeaveTime = pMsg->GetDWord();
				GetFactionWarSys()->AddOneEnmeyFaction(lFactionID1, lFactionID2, dwLeaveTime);
			}
			// 删除定时器
			GetGame()->DelMsgTimer(msgid);

			AddLogText("从DBS读取enemy factions数据消息...OK！");
		}
		break;
	case MSG_DB2W_LOAD_ALL_FACTIONS:
		{
			static long countNum = 0;
			long msgid = pMsg->GetLong();
			long totalNum = pMsg->GetLong();

			//该帮的全局唯一标识4
			CGUID lID;
			pMsg->GetGUID(lID);
			//帮会名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//帮主ID4
			CGUID lMastterID;
			pMsg->GetGUID(lMastterID);	
			//该帮会成立时间32
			tagTime EstablishedTime;
			pMsg->GetEx(&EstablishedTime, sizeof(tagTime));

			CFaction* pFaction = new CFaction(lID,lMastterID,EstablishedTime,string(strName));

			pFaction->InitByWSMsg(pMsg);
			GetOrganiCtrl()->AddFactionOrganizing(lID,pFaction);
			pFaction->SetChangeData(Org_NoDataChange);

			countNum++;
			if(countNum == totalNum)
			{
				char str[128];
				sprintf(str,"成功读取[%d]个帮会", countNum);
				AddLogText(str);
				// 删除定时器
				GetGame()->DelMsgTimer(msgid);
				countNum = 0;
			}
		}
		break;
	case MSG_DB2W_LOAD_ALL_CONFEDERATIONS:
		{
			static long countNum = 0;
			long msgid = pMsg->GetLong();
			long totalNum = pMsg->GetLong();
			
			//该联盟的全局唯一标识4
			CGUID lID;
			pMsg->GetGUID(lID);
			//联盟名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//盟主帮会ID4
			CGUID lMastterID;
			pMsg->GetGUID(lMastterID);	

			CUnion* pConf = new CUnion(lID, lMastterID, string(strName));

			pConf->InitByWSMsg(pMsg);

			pConf->SetChangeData(Org_NoDataChange);
			GetOrganiCtrl()->AddConfederationOrganizing(lID, pConf);

			countNum++;
			
			if(countNum == totalNum)
			{
				char str[128];
				sprintf(str,"成功读取[%d]个联盟", countNum);
				AddLogText(str);
				// 删除定时器
				GetGame()->DelMsgTimer(msgid);
				countNum = 0;
			}
		}
		break;
	case MSG_DB2W_LOAD_FACTION:
		{
			long msgid = pMsg->GetLong();

			//该帮的全局唯一标识4
			CGUID lID;
			pMsg->GetGUID(lID);
			//帮会名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//帮主ID4
			CGUID lMastterID;
			pMsg->GetGUID(lMastterID);
			//该帮会成立时间32
			tagTime EstablishedTime;
			pMsg->GetEx(&EstablishedTime, sizeof(tagTime));

			CFaction* pFaction = new CFaction(lID,lMastterID,EstablishedTime,string(strName));

			pFaction->InitByWSMsg(pMsg);

			GetOrganiCtrl()->AddConfederationOrganizing(lID, pFaction);

			// 删除定时器
			GetGame()->DelMsgTimer(msgid);

			char str[128];
			sprintf(str,"成功读取帮会 ID[%d]", lID);
			AddLogText(str);
		}
		break;
	case MSG_DB2W_LOAD_CONFEDERATION:
		{
			long msgid = pMsg->GetLong();

			//该联盟的全局唯一标识4
			CGUID lID;
			pMsg->GetGUID(lID);
			//联盟名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//盟主帮会ID4
			CGUID lMastterID;
			pMsg->GetGUID(lMastterID);	

			CUnion* pConf = new CUnion(lID, lMastterID, string(strName));

			pConf->InitByWSMsg(pMsg);

			GetOrganiCtrl()->AddConfederationOrganizing(lID, pConf);

			// 删除定时器
			GetGame()->DelMsgTimer(msgid);
			
			char str[128];
			sprintf(str,"成功读取同盟 ID[%d]", lID);
			AddLogText(str);
		}
		break;
	case MSG_S2DB_LOAD_PLAYER://读取玩家数据
		{
			DWORD dwGSIndex=pMsg->GetGSID();
			pMsg->SetType(MSG_W2S_OTHER_ADDSKILL);
			pMsg->Add(dwGSIndex);
			pMsg->SendAll();			
		}
		break;
	case MSG_S2DB_LOAD_PLAYER_GOOD://保存玩家物品
		{
			pMsg->SetType(MSG_W2S_OTHER_DELGOODS);
			pMsg->SendAll();
		}
		break;
	case MSG_S2W_PLAYER_DELSKILL://gm跨服务器删除一个玩家的技能,该消息转发给下面连接的gameserver
		{
			pMsg->SetType(MSG_W2S_OTHER_DELSKILL);
			pMsg->SendAll();
		}
		break;
	case MSG_S2W_PLAYER_SETLEVEL://gm跨服务器设置一个玩家的等级,该消息转发给下面连接的gameserver
		{
			pMsg->SetType(MSG_W2S_OTHER_SETLEVEL);
			pMsg->SendAll();
		}
		break;	

	}
}
