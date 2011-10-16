
#include "stdafx.h"
#include "..\nets\netdb\message.h"
#include "..\nets\Servers.h"
#include "..\dbgood.h"
#include "..\..\dbserver\game.h"
#include "..\dbplayer.h"
//#include "StreamOperator.h"
#include "../../../../dbaccess/WorldDB/RsPlayer.h"
#include "../../../../dbaccess/WorldDB/RSGoods.h"
#include "../../../../dbaccess/WorldDB/RSCountry.h"
#include "../../../../dbaccess/WorldDB/RsCityWar.h"
#include "../../../../dbaccess/WorldDB/RsEnemyFactions.h"
#include "../../../../dbaccess/WorldDB/RsFaction.h"
#include "../../../../dbaccess/WorldDB/RsRegion.h"
#include "../../../../dbaccess/WorldDB/RsUnion.h"
#include "../../../../dbaccess/WorldDB/RsVillageWar.h"
#include"../../../../DBAccess/worlddb/RScards.h"
#include "..\DBentityManager.h"
#include "..\dbFaction.h"
#include "..\dbUnion.h"
#include "..\dbCityWar.h"
#include "..\dbVillageWar.h"
#include "..\dbCountry.h"
#include "..\dbRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnDBSaveMessage(CMessage* pMsg)
{
	size_t nSavePlayerCnt = 0, 
		nCreationCnt = 0,
		nRestoreCnt = 0,
		nDeletionCnt = 0;
	long nTransactionCounter = 0;
	_ConnectionPtr cnSave;

	switch(pMsg->GetType())
	{
	case MSG_W2DB_SAVE_CREATE_PLAYER:                    // 保存创建的教色
		{
	/*		BYTE Data[102400];
			pMsg->GetEx(Data,102400);

			long lPos = 0;
			// 取得该消息的定时器MSGID
			long msgTimerID = _GetLongFromByteArray(Data, lPos);
			
			long createFlag = 0; // 0:失败, 1:成功
			CMyAdoBase::CreateCn(cnSave);

			// 创建一个存盘连接
			if(!CMyAdoBase::OpenCn(cnSave)) 
				AddLogText("连接数据库失败");

			CDBPlayer *pPlayer = new CDBPlayer;
			pPlayer->DecordFromByteArray(Data, lPos);
			if(pPlayer)
			{
				try
				{
					nTransactionCounter = 0; 
					nTransactionCounter = cnSave->BeginTrans();	// 事务开始

					if( GetGame()->GetRsPlayer()->CreatePlayer(pPlayer, cnSave) )
					{
						ShowSaveInfo("○创建角色成功：%s.%s.%d",pPlayer->GetAccount(), pPlayer->GetName(), pPlayer->GetID());
						createFlag = 1;
						// 存盘成功，在DBDATE中删除
						//EnterCriticalSection(&g_CriticalSectionSavePlayerList);
						//CDBPlayer *pPlayer = (pPlayer);
						//SAFE_DELETE(pPlayer);
						//it = pCreationPlayer->erase(it);
						//LeaveCriticalSection(&g_CriticalSectionSavePlayerList);

						cnSave->CommitTrans();		// 事务提交
					}
					else 
					{
						AddLogText("●创建角色失败：%s.%s.%d",pPlayer->GetAccount(), pPlayer->GetName(), pPlayer->GetID() );
						//++it;
						cnSave->RollbackTrans(); //失败回滚
						createFlag = 0;
					}
				}
				catch(...) 
				{
					if(nTransactionCounter)
					{
						cnSave->RollbackTrans(); //失败回滚
					}
					AddLogText("※创建角色异常");
					createFlag = 0;
					//++it;
				}
			}

			CMessage msg(MSG_DB2W_SAVE_CREATE_PLAYER);
			msg.Add(msgTimerID);
			msg.Add(createFlag);
			SAFE_DELETE(pPlayer);*/
		}
		break;
	case MSG_W2DB_SAVE_DELETE_PLAYER:                  // 保存删除的教色
		{
			
		}
		break;

	case MSG_S2DB_SAVE_PLAYER://保存玩家数据
	case MSG_W2DB_SAVE_PLAYER://保存玩家数据
		{
			// 取得该消息的定时器MSGID
	/*		BYTE Data[10240];
			memset(Data, 0, sizeof(Data));
			pMsg->GetEx(Data, 10240);

			long pos = 0;
			long msgTimerID = _GetLongFromByteArray(Data, pos);
			CMyAdoBase::CreateCn(cnSave);

			// 创建一个存盘连接
			if(!CMyAdoBase::OpenCn(cnSave)) 
				AddLogText("连接数据库失败");
			else
			{
				//---------------- 保存Player
				CDBPlayer* player = new CDBPlayer;
	
				player->DecordFromByteArray(Data, pos);
			
				AddLogText("保存Player开始...");
				try
				{
					nTransactionCounter = cnSave->BeginTrans();	// 事务开始

					if( GetGame()->GetRsPlayer()->SavePlayer(player, cnSave))
					{
						cnSave->CommitTrans();		// 事务提交
						ShowSaveInfo("○保存Player成功");
					}
					else
					{
						cnSave->RollbackTrans(); //失败回滚
						AddLogText("●保存Player失败");
					}
				}
				catch(...) 
				{
					if(nTransactionCounter)
					{
						cnSave->RollbackTrans(); //失败回滚
					}
					AddLogText("※保存Player异常");
				}

				SAFE_DELETE(player);
			}

			CMessage msg(MSG_DB2W_SAVE_PLAYER);
			msg.Add(msgTimerID);
			msg.SendToSocket(pMsg->GetSocketID());*/
		}
		break;
	case MSG_W2DB_SAVE_COUNTRY://保存国家数据
		{
	/*		BYTE buf[102400];
			pMsg->GetEx(buf, 102400);

			long pos = 0;
			long msgid = _GetLongFromByteArray(buf, pos); // DBMSGID

			CCountry* pCountry = new CCountry;
			if(pCountry)
			{
				pCountry->DecordFromByteArray(buf, pos);

				CMyAdoBase::CreateCn(cnSave);

				// 创建一个存盘连接
				if(!CMyAdoBase::OpenCn(cnSave)) 
					AddLogText("连接数据库失败");
				else
				{
					AddLogText("保存Country开始...");
					try
					{
						nTransactionCounter = cnSave->BeginTrans();	// 事务开始

						if( GetGame()->GetDBCountry()->Save(pCountry, cnSave))
						{
							cnSave->CommitTrans();		// 事务提交
							ShowSaveInfo("○保存Country成功");
						}
						else
						{
							cnSave->RollbackTrans(); //失败回滚
							AddLogText("●保存Country失败");
						}
					}
					catch(...) 
					{
						if(nTransactionCounter)
						{
							cnSave->RollbackTrans(); //失败回滚
						}
						AddLogText("※保存Country异常");
					}
				}
			}

			
			SAFE_DELETE(pCountry);

			// 回应消息
			CMessage msg(MSG_DB2W_SAVE_COUNTRY);
			msg.Add(msgid);// DBMSGID
			msg.SendToSocket(pMsg->GetSocketID());*/
		}
		break;
	case MSG_W2DB_SAVE_CARDS_LARGESS://保存赠送卡片数据
		{
	/*		CCard* card = new CCard;
			if(card)
			{
				char CDKey[64];
				memset(CDKey, 0, sizeof(CDKey));

				pMsg->GetStr(CDKey, 64);
				card->SetBuyerAccount(CDKey); // Cdkey
				card->SetPrice(pMsg->GetLong()); // SendNum
		
				if( GetGame()->GetDBCards()->SaveLargess(card))
				{
					cnSave->CommitTrans();		// 事务提交
					ShowSaveInfo("○保存CardLargess成功");
				}
				else
				{
					cnSave->RollbackTrans(); //失败回滚
					AddLogText("●保存CardLargess失败");
				}			
			}

			long msgid = pMsg->GetLong(); // DBMSGID
			SAFE_DELETE(card);

			// 回应消息
			CMessage msg(MSG_DB2W_SAVE_CARDS_LARGESS);
			msg.Add(msgid);// DBMSGID
			msg.SendToSocket(pMsg->GetSocketID());*/
		}
		break;
	case MSG_W2DB_SAVE_ALL_CITY_WAR_PARAM:
		{
	/*		list<tagAttackCityTime*> actime;
			//if(actime)
			{
				long msgID    = pMsg->GetLong(); // MSGID
				long paramNum = pMsg->GetLong();// 要保存得城战参数个数
				long decWarFactionNum = 0; // 报名玩家列表

				for(int i = 0; i < paramNum; i++)
				{
					tagAttackCityTime* pSetup = new tagAttackCityTime;
					pSetup->DecWarFactions.clear();
					pSetup->lCityRegionID = pMsg->GetLong();
					pSetup->AttackCityStartTime.wYear = pMsg->GetWord();
					pSetup->AttackCityStartTime.wMonth = pMsg->GetWord();
					pSetup->AttackCityStartTime.wDay = pMsg->GetWord();
					pSetup->AttackCityStartTime.wHour = pMsg->GetWord();
					pSetup->AttackCityStartTime.wMinute = pMsg->GetWord();
					pSetup->AttackCityStartTime.wSecond = pMsg->GetWord();
					// 报名玩家列表
					decWarFactionNum = pMsg->GetLong();
					
					for(int j = 0; j < decWarFactionNum; j++)
					{
						pSetup->DecWarFactions.push_back(pMsg->GetLong());
					}

					actime.push_back(pSetup);
				}

				CMyAdoBase::CreateCn(cnSave);

				// 创建一个存盘连接
				if(!CMyAdoBase::OpenCn(cnSave)) 
					AddLogText("连接数据库失败");
				else
				{
					AddLogText("保存CityWarParam开始...");
					try
					{
						nTransactionCounter = cnSave->BeginTrans();	// 事务开始

						if( GetGame()->GetRsCityWar()->SaveAllCityWarParam(actime, cnSave) )
						{
							cnSave->CommitTrans();		// 事务提交
							ShowSaveInfo("○保存CityWarParam成功");
						}
						else
						{
							cnSave->RollbackTrans(); //失败回滚
							AddLogText("●保存CityWarParam失败");
						}
					}
					catch(...) 
					{
						if(nTransactionCounter)
						{
							cnSave->RollbackTrans(); //失败回滚
						}
						AddLogText("※保存CityWarParam异常");
					}
				}
			}

			long msgid = pMsg->GetLong(); // DBMSGID
			list<tagAttackCityTime*>::iterator itr = actime.begin();
			for(; itr != actime.end(); itr++)
			{
				if(*itr)
					SAFE_DELETE(*itr);
			}
			
			// 回应消息
			CMessage msg(MSG_DB2W_SAVE_ALL_CITY_WAR_PARAM);
			msg.Add(msgid);// DBMSGID
			msg.SendToSocket(pMsg->GetSocketID());*/
		}
		break;
	case MSG_W2DB_SAVE_ALL_VILLAGE_WAR_PARAM:
		{
	/*		list<tagVilWarSetup*> actime;
			//if(actime)
			{
				long msgID    = pMsg->GetLong(); // MSGID
				long paramNum = pMsg->GetLong();// 要保存得城战参数个数
				long decWarFactionNum = 0; // 报名玩家列表

				for(int i = 0; i < paramNum; i++)
				{
					tagVilWarSetup* pSetup = new tagVilWarSetup;
					pSetup->DecWarFactions.clear();
					pSetup->lWarRegionID = pMsg->GetLong();
					pSetup->WarStartTime.wYear = pMsg->GetWord();
					pSetup->WarStartTime.wMonth = pMsg->GetWord();
					pSetup->WarStartTime.wDay = pMsg->GetWord();
					pSetup->WarStartTime.wHour = pMsg->GetWord();
					pSetup->WarStartTime.wMinute = pMsg->GetWord();
					pSetup->WarStartTime.wSecond = pMsg->GetWord();
					// 报名玩家列表
					decWarFactionNum = pMsg->GetLong();

					for(int j = 0; j < decWarFactionNum; j++)
					{
						pSetup->DecWarFactions.push_back(pMsg->GetLong());
					}

					actime.push_back(pSetup);
				}

				CMyAdoBase::CreateCn(cnSave);

				// 创建一个存盘连接
				if(!CMyAdoBase::OpenCn(cnSave)) 
					AddLogText("连接数据库失败");
				else
				{
					AddLogText("保存VilWarParam开始...");
					try
					{
						nTransactionCounter = cnSave->BeginTrans();	// 事务开始

						if( GetGame()->GetRsVillageWar()->SaveAllVillageWarParam(actime, cnSave) )
						{
							cnSave->CommitTrans();		// 事务提交
							ShowSaveInfo("○保存VilWarParam成功");
						}
						else
						{
							cnSave->RollbackTrans(); //失败回滚
							AddLogText("●保存VilWarParam失败");
						}
					}
					catch(...) 
					{
						if(nTransactionCounter)
						{
							cnSave->RollbackTrans(); //失败回滚
						}
						AddLogText("※保存VilWarParam异常");
					}
				}
			}

			long msgid = pMsg->GetLong(); // DBMSGID
			list<tagVilWarSetup*>::iterator itr = actime.begin();
			for(; itr != actime.end(); itr++)
			{
				if(*itr)
					SAFE_DELETE(*itr);
			}

			// 回应消息
			CMessage msg(MSG_DB2W_SAVE_ALL_VILLAGE_WAR_PARAM);
			msg.Add(msgid);// DBMSGID
			msg.SendToSocket(pMsg->GetSocketID());*/
		}
		break;
	case MSG_W2DB_SAVE_ALL_ENEMY_FACTIONS:
		{
	/*		list<tagEnemyFaction*> actime;
				long msgid    = pMsg->GetLong(); // MSGID
				long paramNum = pMsg->GetLong();// 要保存得城战参数个数
				long decWarFactionNum = 0; // 报名玩家列表

				for(int i = 0; i < paramNum; i++)
				{
					tagEnemyFaction* pSetup = new tagEnemyFaction;
					pSetup->lFactionID1 = pMsg->GetLong();
					pSetup->lFactionID2 = pMsg->GetLong();
					pSetup->dwDisandTime = pMsg->GetDWord();
					actime.push_back(pSetup);
				}

				CMyAdoBase::CreateCn(cnSave);

				// 创建一个存盘连接
				if(!CMyAdoBase::OpenCn(cnSave)) 
					AddLogText("连接数据库失败");
				else
				{
					AddLogText("保存EnemyFactions开始...");
					try
					{
						nTransactionCounter = cnSave->BeginTrans();	// 事务开始

						if( GetGame()->GetRsEnemyFactions()->SaveAllEnemyFactions(actime, cnSave) )
						{
							cnSave->CommitTrans();		// 事务提交
							ShowSaveInfo("○保存EnemyFactions成功");
						}
						else
						{
							cnSave->RollbackTrans(); //失败回滚
							AddLogText("●保存EnemyFactions失败");
						}
					}
					catch(...) 
					{
						if(nTransactionCounter)
						{
							cnSave->RollbackTrans(); //失败回滚
						}
						AddLogText("※保存EnemyFactions异常");
					}
				}

			list<tagEnemyFaction*>::iterator itr = actime.begin();
			for(; itr != actime.end(); itr++)
			{
				if(*itr)
					SAFE_DELETE(*itr);
			}

			// 回应消息
			CMessage msg(MSG_DB2W_SAVE_ALL_ENEMY_FACTIONS);
			msg.Add(msgid);// DBMSGID
			msg.SendToSocket(pMsg->GetSocketID());*/
		}
		break;
	case MSG_W2DB_SAVE_REGION_PARAM:
		{
	/*		BYTE buf[102400];
			pMsg->GetEx(buf, 102400);

			long pos = 0;
			long msgid = _GetLongFromByteArray(buf, pos);

			CWorldRegion* pRegion = new CWorldRegion;

			if(pRegion)
			{
				pRegion->DecordFromByteArray(buf, pos);
				
				CMyAdoBase::CreateCn(cnSave);

				// 创建一个存盘连接
				if(!CMyAdoBase::OpenCn(cnSave)) 
					AddLogText("连接数据库失败");
				else
				{
					AddLogText("保存RegionParam开始...");
					try
					{
						nTransactionCounter = cnSave->BeginTrans();	// 事务开始

						if( GetGame()->GetRsRegion()->Save(pRegion, cnSave) )
						{
							cnSave->CommitTrans();		// 事务提交
							ShowSaveInfo("○保存RegionParam成功");
						}
						else
						{
							cnSave->RollbackTrans(); //失败回滚
							AddLogText("●保存RegionParam失败");
						}
					}
					catch(...) 
					{
						if(nTransactionCounter)
						{
							cnSave->RollbackTrans(); //失败回滚
						}
						AddLogText("※保存RegionParam异常");
					}
				}
			}

			SAFE_DELETE(pRegion);
			
			// 回应消息
			CMessage msg(MSG_DB2W_SAVE_REGION_PARAM);
			msg.Add(msgid);// DBMSGID
			msg.SendToSocket(pMsg->GetSocketID());*/
		}
		break;
	case MSG_S2DB_SAVE_CONFEDERATION:
	case MSG_W2DB_SAVE_CONFEDERATION:
		{
	/*		long msgid = pMsg->GetLong();

			//该联盟的全局唯一标识4
			long lID = pMsg->GetLong();
			//联盟名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//盟主帮会ID4
			long lMastterID = pMsg->GetLong();	

			CDBUnion* pConf = new CDBUnion(lID, lMastterID, string(strName));

			pConf->InitByWSMsg(pMsg);

			CMyAdoBase::CreateCn(cnSave);

			// 创建一个存盘连接
			if(!CMyAdoBase::OpenCn(cnSave)) 
				AddLogText("连接数据库失败");
			else
			{
				AddLogText("保存Confederation开始...");
				try
				{
					nTransactionCounter = cnSave->BeginTrans();	// 事务开始

					if( GetGame()->GetRsConfederation()->SaveConfederation(pConf, cnSave) )
					{
						cnSave->CommitTrans();		// 事务提交
						ShowSaveInfo("○保存Confederation成功");
					}
					else
					{
						cnSave->RollbackTrans(); //失败回滚
						AddLogText("●保存Confederation失败");
					}
				}
				catch(...) 
				{
					if(nTransactionCounter)
					{
						cnSave->RollbackTrans(); //失败回滚
					}
					AddLogText("※保存Confederation异常");
				}
			}

			// 回应消息
			if(pMsg->GetType() == MSG_W2DB_SAVE_CONFEDERATION)
			{
				CMessage msg(MSG_DB2W_SAVE_CONFEDERATION);
				msg.Add(msgid);// DBMSGID
				msg.SendToSocket(pMsg->GetSocketID());
			}
			else if(pMsg->GetType() == MSG_S2DB_SAVE_CONFEDERATION)
			{
				CMessage msg(MSG_DB2S_SAVE_CONFEDERATION);
				msg.Add(msgid);// DBMSGID
				msg.SendToSocket(pMsg->GetSocketID());
			}*/
		}
		break;

	case MSG_S2DB_SAVE_FACTION:
	case MSG_W2DB_SAVE_FACTION:
		{
	/*		long msgid = pMsg->GetLong(); // MSGID
			//该帮的全局唯一标识4
			long lID = pMsg->GetLong();
			//帮会名字16
			char strName[16] = {0};
			pMsg->GetStr(strName, 15);
			//帮主ID4
			long lMastterID = pMsg->GetLong();	
			//该帮会成立时间32
			tagTime EstablishedTime;
			pMsg->GetEx(&EstablishedTime, sizeof(tagTime));

			CDBFaction* pFaction = new CDBFaction(lID,lMastterID,EstablishedTime,string(strName));
			
			pFaction->InitByWSMsg(pMsg);
			
			CMyAdoBase::CreateCn(cnSave);

			// 创建一个存盘连接
			if(!CMyAdoBase::OpenCn(cnSave)) 
				AddLogText("连接数据库失败");
			else
			{
				AddLogText("保存Faction开始...");
				try
				{
					nTransactionCounter = cnSave->BeginTrans();	// 事务开始

					if( GetGame()->GetRsFaction()->SaveFaction(pFaction, cnSave) )
					{
						cnSave->CommitTrans();		// 事务提交
						ShowSaveInfo("○保存Faction成功");
					}
					else
					{
						cnSave->RollbackTrans(); //失败回滚
						AddLogText("●保存Faction失败");
					}
				}
				catch(...) 
				{
					if(nTransactionCounter)
					{
						cnSave->RollbackTrans(); //失败回滚
					}
					AddLogText("※保存Faction异常");
				}
			}
	
			SAFE_DELETE(pFaction);

			// 回应消息
			if(pMsg->GetType() == MSG_W2DB_SAVE_FACTION)
			{
				CMessage msg(MSG_DB2W_SAVE_FACTION);
				msg.Add(msgid);// DBMSGID
				msg.SendToSocket(pMsg->GetSocketID());
			}
			else if(pMsg->GetType() == MSG_S2DB_SAVE_FACTION)
			{
				CMessage msg(MSG_DB2S_SAVE_FACTION);
				msg.Add(msgid);// DBMSGID
				msg.SendToSocket(pMsg->GetSocketID());
			}*/
		}
		break;
	}
}
