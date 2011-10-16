#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\worldregion.h"
#include "..\player.h"
#include "../script/VariableList.h"

#include "setup\playerlist.h"
#include "..\Goods\cgoodsfactory.h"
#include "..\goods\csuitrulefactory.h"
#include "..\goods\IncrementShopManager.h"
#include "..\goods\enchase\CKitCardRuleFactory.h"
#include "../business/CBusinessManager.h"
#include "setup\monsterlist.h"
#include "setup\GateList.h"
#include "setup\CountTimerList.h"
#include "setup\tradelist.h"
#include "setup\incrementShopList.h"
#include "setup\ContributeSetup.h"
#include "setup\hitlevelsetup.h"
#include "..\NewSkill\NewSkillFactory.h"
#include "setup\globesetup.h"
#include "setup/logsystem.h"
#include "setup/gmlist.h"
#include "setup/emotion.h"
#include "setup/QuestSystem.h"
#include "setup\MailList.h"
#include "setup\ScriptAILoader.h"
#include "../public/DeputyOccu.h"
#include "setup/regionsetup.h"
#include "..\public\dupliregionsetup.h"
#include "..\public\OccupSkillList.h"
#include "..\public\StudySkillCondition.h"
#include "../public/strtype.h"
#include "..\public\StrUnifyInput.h"
#include "..\public\WordsFilter.h"
#include "..\public\ReliveGoodsList.h"
#include "..\public\Cosmetology.h"
#include "..\..\appworld\session\csessionfactory.h"
#include "..\..\appworld\session\csession.h"
#include "..\..\appworld\Organizingsystem\OrganizingCtrl.h"
#include "..\..\appworld\Country\CountryParam.h"
#include "..\..\appworld\Country\CountryHandler.h"
#include <mmsystem.h>
#include "../GlobalRgnManager.h"
#include "../DBentity/EntityGroup.h"
#include "../DBentity/entityManager.h"
#include "../public/entityproperty.h"
#include "../PlayerDepot.h"
#include <vector>
#include "../../AppWorld/Linkman/LinkmanSystem.h"
#include "../public/StrUnifyInput.h"
#include "..\..\appworld\PlayerMan\PlayerMan.h"
#include "..\..\appworld\PetCtrl.h"
#include "..\..\AppWorld\DBEntity\Entity.h"
#include "..\..\appworld\anticheat\QuestionManager.h"
#include "..\public\CreditSystem.h"
#include "../FBusiness/Configer.h"
#include "..\public\QuestIndexXml.h"
#include "..\public\LotteryXml.h"



extern HANDLE g_hSavingThread;

extern const unsigned RELOAD_PLAYER_MAN;	// Fox@20081120 玩家管理
extern const unsigned RELOAD_PET_LIST;		// Fox@20081119 宠物
extern const unsigned RELOAD_ANTI_CHEAT;

// 响应其他消息
void OnServerMessage(CMessage* pMsg)
{
	long lSocketID = pMsg->GetSocketID();
	switch(pMsg->GetType())
	{
	case MSG_DB2W_SERVER_SENDSETUP:
		{
			// 先清除原有数据
			std::map<string, tagEntityBuildInfo*>::iterator deItr = GetGame()->GetEntityManager()->GetObjAttrDef().begin();
			for(; deItr != GetGame()->GetEntityManager()->GetObjAttrDef().end(); deItr++)
			{
				OBJ_RELEASE(tagEntityBuildInfo,deItr->second);
			}
			GetGame()->GetEntityManager()->GetObjAttrDef().clear();

			long objAttrDefNum = pMsg->GetLong();
			long bufSize = pMsg->GetLong();
			BYTE* buf = (BYTE*)M_ALLOC(bufSize);
			pMsg->GetEx(buf, bufSize);
			long pos = 0;
			for(int i=0; i<objAttrDefNum; i++)
			{
				tagEntityBuildInfo* tBuildInfo = OBJ_CREATE(tagEntityBuildInfo);
				tBuildInfo->DecodeFromByteArray(buf, pos, bufSize);
				GetGame()->GetEntityManager()->GetObjAttrDef()[tBuildInfo->strComFlag] = tBuildInfo;
			}
			M_FREE(buf,bufSize);

			GetGame()->SetDbIsReady(true);
	
			// 读取初始化数据
			//if(!GetGame()->GetInitCountryLoadFlag())
			//	GetGame()->GetEntityManager()->CreateLoadCountrySession();
			if(!GetGame()->GetInitRegionLoadFlag())
				GetGame()->GetEntityManager()->CreateLoadRegionSession();
			if(!GetGame()->GetInitPersonalHouseLoadFlag())
				GetGame()->GetEntityManager()->CreateLoadPhRgnSession();
			if(!GetGame()->GetInitVarLoadFlag())
				GetGame()->GetEntityManager()->CreateLoadGlobalVarSession();
			if(!GetGame()->GetInitFactionLoadFlag())
				GetGame()->GetEntityManager()->CreateLoadFactionGroupSession();
			if(!GetGame()->GetInitUnionLoadFlag())
				GetGame()->GetEntityManager()->CreateLoadUnionGroupSession();

			// 检查是否所有GS都连接完成并且DBS准备完成, 成功则发送saveplayer缓冲区的数据
			bool isSaveFlag = false;
			map<DWORD, CGame::tagGameServer>::iterator gsItr = GetGame()->GetGameServerList()->begin();
			for(; gsItr != GetGame()->GetGameServerList()->end(); gsItr++)
			{
				if(gsItr->second.bConnected)
					isSaveFlag = true;
				else
				{
					isSaveFlag = false;
					break;
				}
			}
			
			// 同步DBS游戏数据
			GetGame()->DoSaveData(true);
			
			if(isSaveFlag && GetGame()->GetDbIsReady())
			{	
				//DoSaveData();
				GetGame()->ProcessDelaySavePlayer();
				GetGame()->SendCdkeyToLoginServer(TRUE);
			}
			else
				GetGame()->SendCdkeyToLoginServer(FALSE);

			// 发起读取商业数据的请求(之前未从DB装载)
			if( GetInst( Business::CBusinessManager ).IsEnable() && 
				!GetInst( Business::CBusinessManager ).IsDBLoaded() )
			{
				GetGame()->GetEntityManager()->CreateLoadBusinessSession();
			}

			GetGame()->GetEntityManager()->CreateLoadLimitGoodsRecordSession();
		}
		break;
	case MSG_DB2W_SERVER_QUEST_RECONNECTION:
		{
			long msgid = pMsg->GetLong();
			// 删除定时器
			GetGame()->DelMsgTimer(msgid);
			AddLogText(CStringReading::LoadString(IDS_WS_SERVERMESSAGE, STR_WS_SERVERMESSAGE_DBSRECONNETIONINFO));
		}
		break;
	case MSG_DB2W_SERVER_INIT_PLAYERDATABUFFER:
		{
			// 初始化CLoginPlayer和CPlayer的缓冲区

		}
		break;
	case MSG_S2W_SERVER_QUEST_RECONNECTION:
		
		break;
		// 发送server信息给WorldServer
	case MSG_S2W_SERVER_SENDSERVERINFO:
		{
			static long lGsSleepTime = 0; // Minutes

			// 设置gameserver标识
			bool bReconnect = pMsg->GetChar()?true:false;
			long lMsgLen = 0;
			DWORD dwPort = pMsg->GetLong();
			char strGameServerIP[256];
			pMsg->GetStr(strGameServerIP,256);

			CGame::tagGameServer* pGameServer = GetGame()->GetGameServer(strGameServerIP, dwPort);
			if (!pGameServer)
			{
				AddLogText(CStringReading::LoadString(IDS_WS_SERVERMESSAGE, STR_WS_SERVERMESSAGE_GSRECONNSETUPDATA), strGameServerIP, dwPort);
				break;
			}

			char str[256];
			sprintf(str, "%s [%d]", strGameServerIP, dwPort);
			SendMessage(g_hGameServerListBox,LB_INSERTSTRING,-1,(LPARAM)str);

			if (bReconnect)
			{
				// 取出数据块
				DBReadSet setReadDB;
				pMsg->GetDBReadSet(setReadDB);

				// 解码GS全局变量值
				BYTE byVarFlag = setReadDB.GetByteFromByteArray();
				if(1 == byVarFlag)
					GetGame()->GetGeneralVariableList()->DecodeFromDataBlock(setReadDB);

				// 解码在线玩家数据
				long playerNum = setReadDB.GetLongFromByteArray();
				for(int i=0; i<playerNum; i++)
				{
					bool bFlag = setReadDB.GetByteFromByteArray()==0?false:true;
					if(bFlag)
					{
						if (pGameServer)
						{
							pGameServer->lReceivedPlayerData++;
							pGameServer->lPlayerNum++;
						}

						CGUID lPlayerID;
						setReadDB.GetBufferFromByteArray(lPlayerID);

						GetGame()->DeleteMapPlayer(lPlayerID);
						CPlayer* pPlayer = GetGame()->NewRawPlayer(ePlayerGsReconnectOne);
						if(pPlayer)
						{
							pPlayer->SetExID(lPlayerID);
							pPlayer->DecodeFromDataBlock(setReadDB);
							GetGame()->AppendMapPlayer(pPlayer);
							pPlayer->SetGsSocket(lSocketID);
							if(GetGame()->GetDbIsReady())
							{
								GetGame()->GetEntityManager()->CreateSavePlayerDataSession(pPlayer->GetExID(), SAVE_DETAIL_ALL);
									
								Account* pAcc = NULL;
								if(pPlayer->GetAccount() != 0)
								{
									pAcc = MP_NEW Account;
									if (pAcc)
									{
										CGUID accGuid;
										CGUID::CreateGUID(accGuid);
										pAcc->SetGUID(accGuid);
										pAcc->SetName(pPlayer->GetAccount());
										if(!GetGame()->GetEntityManager()->AddAccount(pAcc))
										{
											MP_DELETE(pAcc);
										}
										else
										{
											pAcc->SetCurGamedPlayerGuid(pPlayer->GetExID());
											pAcc->SetPlayerState(PLAYER_STATE_ENTERED_GAME);
										}
									}
								}

								CEntityGroup* lPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[loginplayer]"), pPlayer->GetExID());
								if(pPlayer && pAcc)
								{
									pAcc->AddLoginPlayer(lPlayer);
									GetGame()->GetEntityManager()->CreateSaveLoginPlayerDataSession(pPlayer->GetExID());
								}
							}

							GetOrganiCtrl()->OnPlayerEnterGame(pPlayer->GetExID());
							LinkmanSystem::GetInstance().OnPlayerOnline(pPlayer->GetExID());
						}
					}
				}
				sprintf(str, CStringReading::LoadString(IDS_WS_SERVERMESSAGE, STR_WS_SERVERMESSAGE_GSRECONNLOGINPLAYERNUM), pGameServer->dwIndex, playerNum);
				AddLogText(str);

				// 解码下线玩家数据
				playerNum = setReadDB.GetLongFromByteArray();
				for(int i=0; i<playerNum; i++)
				{
					bool bFlag = setReadDB.GetByteFromByteArray()==0?false:true;
					if(bFlag)
					{
						CGUID lPlayerID;
						setReadDB.GetBufferFromByteArray(lPlayerID);
						GetGame()->DeleteMapPlayer(lPlayerID);

						CPlayer* pPlayer=GetGame()->NewRawPlayer(ePlayerGsReconnectTwo);
						if(pPlayer)
						{
							pPlayer->SetExID(lPlayerID);
							pPlayer->DecodeFromDataBlock(setReadDB);
							GetGame()->PushSavePlayer(pPlayer);
							pPlayer->SetGsSocket(lSocketID);
						}
					}
				}

				sprintf(str, CStringReading::LoadString(IDS_WS_SERVERMESSAGE, STR_WS_SERVERMESSAGE_GSRECONNSENTLOGINPLAYERNUM), pGameServer->dwIndex, playerNum);
				AddLogText(str);
				// 解码副本数据
				long rgnNum = setReadDB.GetLongFromByteArray();
				for(int i=0; i<rgnNum; i++)
				{
					BYTE* RgnDataBuf = NULL;
					// rgn data size
					long dataSize = setReadDB.GetLongFromByteArray();
					RgnDataBuf = (BYTE*)M_ALLOC(dataSize);
					setReadDB.GetBufferFromByteArray(RgnDataBuf, dataSize);
					BYTE* RgnVarBuf = NULL;
					// varlist size
					long varSize = setReadDB.GetLongFromByteArray();
					RgnVarBuf = (BYTE*)M_ALLOC(varSize);
					setReadDB.GetBufferFromByteArray(RgnVarBuf, varSize);
					// 替换rgn
					CWorldRegion* rgn = MP_NEW CWorldRegion;
					if(rgn)
					{
						long pos = 0;
						rgn->DecordFromByteArray(RgnDataBuf, pos);
						//rgn->Load();
						CGlobalRgnManager::tagRegion* pTemplateRgn = NULL;
						switch(rgn->GetRgnType())
						{
						case RGN_PERSONAL_HOUSE:
							{
								pTemplateRgn = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalHouseRegion(rgn->GetID());
							}
							break;
						case RGN_PERSONAL:
							{
								pTemplateRgn = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalRegion(rgn->GetID());
							}
							break;
						case RGN_TEAM:
							{
								pTemplateRgn = GetGame()->GetGlobalRgnManager()->GetTemplateTeamRegion(rgn->GetID());
							}
							break;
						}
						if(pTemplateRgn && pTemplateRgn->pRegion)
							rgn->CloneAllData(pTemplateRgn->pRegion);

						pos = 0;
						rgn->GetVariableList()->DecordFromByteArray(RgnVarBuf, pos);
						GetGame()->GetGlobalRgnManager()->GetRgnMap()[rgn->GetExID()] = rgn;
						// gsid
						rgn->SetGsid(pGameServer->dwIndex);
						if(rgn->GetRgnType() == RGN_PERSONAL)
							GetGame()->GetGlobalRgnManager()->AddPersonalRgnGUID(rgn->GetOwnerGUID(), rgn->GetExID());
					}
					M_FREE(RgnDataBuf,dataSize);
					M_FREE(RgnVarBuf,varSize);
				}
				sprintf(str, CStringReading::LoadString(IDS_WS_SERVERMESSAGE, STR_WS_SERVERMESSAGE_GSRECONNSENDRGNDATA), pGameServer->dwIndex);
				AddLogText(str);

				// 解码副本物品数据
				long PHRgnNum = setReadDB.GetLongFromByteArray();
				long bufSize = setReadDB.GetLongFromByteArray();
				BYTE* goodsBuf = (BYTE*)M_ALLOC(bufSize);
				setReadDB.GetBufferFromByteArray(goodsBuf, bufSize);
				long pos = 0;
				for(int j=0; j<PHRgnNum; j++)
				{
					CGUID phGUID;
					_GetBufferFromByteArray(goodsBuf, pos, phGUID);
					long goodsNum = _GetLongFromByteArray(goodsBuf, pos);
					if(goodsNum)
					{
						CEntityGroup* dupRgn = NULL;
						CEntityManager::DBEntityComponentMapItr dupItr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(string("[phrgngroup]"));
						if(dupItr == GetGame()->GetEntityManager()->GetBaseEntityMap().end())
						{
							AddLogText("未在WS上先创建PersonalHouseGroup对象!");
							return;
						}

						if(dupItr->second.size() != 1) return;
						CEntityGroup* pPhRgnGroup = dupItr->second.begin()->second;
						map<CGUID, CBaseEntity*>::iterator rgnItr = pPhRgnGroup->GetEntityGroupMap().find(phGUID);
						if(rgnItr == pPhRgnGroup->GetEntityGroupMap().end())
						{
							dupRgn = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgn]"), phGUID);
							pPhRgnGroup->GetEntityGroupMap()[phGUID] = dupRgn;
						}
						else
							dupRgn = (CEntityGroup*)rgnItr->second;

						if(dupRgn)
						{
							// 清除个人房屋副本内的物品
							CEntityGroup* pGoodsGroup = (CEntityGroup*)GetGame()->GetEntityManager()->FindLeafByComFlag(dupRgn, string("[phrgngoodsgroup]"));
							if(pGoodsGroup)
								pGoodsGroup->ReleaseChilds();
						}

						for(int i=0; i<goodsNum; i++)
						{
							struct PHGoods
							{
								// 物品GUID
								CGUID guid;
								// 物品名字
								char name[64];
								// 朝向
								int dir;
								// X长度
								int width;
								// Y长度
								int height;
								// 坐标
								float posX;
								float posY;
							};
							PHGoods tGoods;
							_GetBufferFromByteArray(goodsBuf, pos, &tGoods, sizeof(PHGoods));

							CEntityGroup* pGoodsGroup = (CEntityGroup*)GetGame()->GetEntityManager()->FindLeafByComFlag(dupRgn, string("[phrgngoodsgroup]"));
							if(pGoodsGroup)
							{
								CEntity* goods = (CEntity*)GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgngoods]"), tGoods.guid);

								goods->SetGuidAttr(string("guid"),  tGoods.guid);
								goods->SetStringAttr(string("Name"), tGoods.name);
								goods->SetLongAttr(string("Dir"), tGoods.dir);
								goods->SetLongAttr(string("Width"), tGoods.width );
								goods->SetLongAttr(string("Height"), tGoods.height);
								goods->SetLongAttr(string("PosX"), tGoods.posX);
								goods->SetLongAttr(string("PosY"), tGoods.posY);
								goods->SetGuidAttr(string("DupRgnGUID"), dupRgn->GetGUID());

								pGoodsGroup->AddChild(goods);
							}
						}	
					}
				}
				M_FREE(goodsBuf,bufSize);

				pGameServer->bConnected = true;
				CMessage::MapGSIDSocket(pGameServer->dwIndex,lSocketID);

				// 检查是否所有GS都连接完成并且DBS准备完成, 成功则发送saveplayer缓冲区的数据
				bool isSaveFlag = false;
				map<DWORD, CGame::tagGameServer>::iterator gsItr = GetGame()->GetGameServerList()->begin();
				for(; gsItr != GetGame()->GetGameServerList()->end(); gsItr++)
				{
					if(gsItr->second.bConnected)
						isSaveFlag = true;
					else
					{
						isSaveFlag = false;
						break;
					}
				}
				if(isSaveFlag && GetGame()->GetDbIsReady())
				{
					//DoSaveData();
					GetGame()->ProcessDelaySavePlayer();
					GetGame()->SendCdkeyToLoginServer(TRUE);
				}
				else
				{
					GetGame()->SendCdkeyToLoginServer(FALSE);
				}
			}
			else
			{
				GetGame()->InitPersonalHouseDataOnGS(pGameServer->dwIndex);

				//////////////////////////////////////////////////////////////////////////
				// 发送全局配置给GameServer
				vector<BYTE> vectorByte;
				DBWriteSet setWriteDB;

				//WS 信息				
				CMessage msgWSInfo(MSG_W2S_SERVER_SENDSETUP);
				msgWSInfo.Add((long)SI_WS_INFO);
				msgWSInfo.Add(GetGame()->GetSetup()->dwNumber);
				msgWSInfo.SendToSocket(lSocketID);

				//全局字符串
				vectorByte.clear();
				CStringReading::AddToByteArray(&vectorByte);
				CMessage msgGlobeString(MSG_W2S_SERVER_SENDSETUP);
				msgGlobeString.Add((long)SI_GLOBALSTRING);
				msgGlobeString.Add((long)vectorByte.size());
				msgGlobeString.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgGlobeString.SendToSocket(lSocketID);

				// 发送分线场景数据
				vectorByte.clear();
				GetGame()->GetGlobalRgnManager()->AddLinedRgnInfoToByteArray(vectorByte);
				CMessage msgLinedRgnInfo(MSG_W2S_SERVER_SENDSETUP);
				msgLinedRgnInfo.Add((long)SI_LINED_RGN_INFO);
				msgLinedRgnInfo.Add((long)vectorByte.size());
				msgLinedRgnInfo.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgLinedRgnInfo.SendToSocket(lSocketID);

				//! 敏感词
				vectorByte.clear();
				CWordsFilter::GetInstance()->AddToByteArray(vectorByte);
				CMessage msgSensitiveWord(MSG_W2S_SERVER_SENDSETUP);
				msgSensitiveWord.Add((long)SI_SensitiveWord);
				msgSensitiveWord.Add((long)vectorByte.size());
				msgSensitiveWord.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgSensitiveWord.SendToSocket(lSocketID);

				// 全局参数
				vectorByte.clear();
				CGlobeSetup::AddToByteArray(&vectorByte);
				CMessage msgGlobeSetup(MSG_W2S_SERVER_SENDSETUP);
				msgGlobeSetup.Add((long)SI_GLOBESETUP);
				msgGlobeSetup.Add((long)vectorByte.size());
				msgGlobeSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgGlobeSetup.SendToSocket(lSocketID);

				// 物品信息
				vectorByte.clear();
				CGoodsFactory::Serialize(&vectorByte);
				CMessage msgGoodsList(MSG_W2S_SERVER_SENDSETUP);
				msgGoodsList.Add((long)SI_GOODSLIST);
				msgGoodsList.Add((long)vectorByte.size());
				msgGoodsList.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgGoodsList.SendToSocket(lSocketID);

				//COUNTTIMERLIST SETUP
				vectorByte.clear();
				CCountTimerList::Serialize(vectorByte);
				CMessage msgCountTimer(MSG_W2S_SERVER_SENDSETUP);
				msgCountTimer.Add((long)SI_COUNTTIMERLIST);
				msgCountTimer.Add((long)vectorByte.size());
				msgCountTimer.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgCountTimer.SendToSocket(lSocketID);

				//SUIT SETUP
				vectorByte.clear();
				CSuitRuleFactory::Serialize(&vectorByte);
				CMessage msgSuitSetup(MSG_W2S_SERVER_SENDSETUP);
				msgSuitSetup.Add((long)SI_SUITSETUP);
				msgSuitSetup.Add((long)vectorByte.size());
				msgSuitSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgSuitSetup.SendToSocket(lSocketID);

				//ENCHASE SETUP
				vectorByte.clear();
				CKitCardRuleFactory::Serialize(&vectorByte);
				CMessage msgEnchaseSetup(MSG_W2S_SERVER_SENDSETUP);
				msgEnchaseSetup.Add((long)SI_ENCHASESETUP);
				msgEnchaseSetup.Add((long)vectorByte.size());
				msgEnchaseSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgEnchaseSetup.SendToSocket(lSocketID);

				// 跑商配置数据  
				GetInst( Business::CBusinessManager ).SendSetupToGS( pGameServer->dwIndex, lSocketID );

				//! 物品扩展配置
				vectorByte.clear();
				GetGame()->SerializeGoodsExSetup(vectorByte);
				CMessage msgGoodsExSetup(MSG_W2S_SERVER_SENDSETUP);
				msgGoodsExSetup.Add((long)SI_GOODSEX);
				msgGoodsExSetup.Add((long)vectorByte.size());
				msgGoodsExSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgGoodsExSetup.SendToSocket(lSocketID);

				//! 非副职业制作配方
				CMessage msgGoodsTailor(MSG_W2S_SERVER_SENDSETUP);
				msgGoodsTailor.Add((long)SI_GOODSTAILOR);
				DBWriteSet setGoodsTailorWriteDB;
				msgGoodsTailor.GetDBWriteSet(setGoodsTailorWriteDB);
				GoodsTailorSetup::GetInstance().Code(setGoodsTailorWriteDB);
				msgGoodsTailor.SendToSocket(lSocketID);
				
				//! 仓库配置
				CMessage msgDepotMsg(MSG_W2S_SERVER_SENDSETUP);
				msgDepotMsg.Add((long)SI_DEPOT);
				msgDepotMsg.Add((long)sizeof(tagDepotSetup));
				msgDepotMsg.AddEx((void*)PlayerDepot::GetDepotSetup(), sizeof(tagDepotSetup));
				msgDepotMsg.SendToSocket(lSocketID);

				//! 好友系统配置
				vectorByte.clear();
				LinkmanSystem::GetInstance().SetupAddToByteArray(vectorByte);
				CMessage msgLGSManage(MSG_W2S_SERVER_SENDSETUP);
				msgLGSManage.Add((long)SI_LINKMAN);
				msgLGSManage.Add((long)vectorByte.size());
				msgLGSManage.AddEx(&(vectorByte[0]), (long)vectorByte.size());
				msgLGSManage.SendToSocket(lSocketID);

				// 怪物属性
				vectorByte.clear();
				CMonsterList::AddToByteArray(&vectorByte);
				CMessage msgMonsterList(MSG_W2S_SERVER_SENDSETUP);
				msgMonsterList.Add((long)SI_MONSTERLIST);
				msgMonsterList.Add((long)vectorByte.size());
				msgMonsterList.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgMonsterList.SendToSocket(lSocketID);
				
				// Fox@20081120 玩家管理
				GetGame()->LoadEx(RELOAD_PLAYER_MAN, lSocketID, false);

				// Fox@20081119 宠物
				GetGame()->LoadEx(RELOAD_PET_LIST, lSocketID, false);
			
				// BaiYun@2009525 反外挂
				GetGame()->LoadEx(RELOAD_ANTI_CHEAT, lSocketID, false);

				// 题目缓存
				GetInst( AntiCheat::CQuestionManager ).SendQuestionsToGS( lSocketID );
			
				//门属性
				vectorByte.clear();
				CGateList::AddToByteArray(&vectorByte);
				CMessage msgGateList(MSG_W2S_SERVER_SENDSETUP);
				msgGateList.Add((long)SI_GATELIST);
				msgGateList.Add((long)vectorByte.size());
				msgGateList.AddEx(&vectorByte[0],(long)vectorByte.size());
				msgGateList.SendToSocket(lSocketID);
				
				//CDeputyOccuSystem
				vectorByte.clear();
				CDeputyOccuSystem::AddToByteArray(&vectorByte);
				CMessage msgDeputyOccu(MSG_W2S_SERVER_SENDSETUP);
				msgDeputyOccu.Add((long)SI_DEPUTYOCCU);
				msgDeputyOccu.Add((long)vectorByte.size());
				msgDeputyOccu.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgDeputyOccu.SendToSocket(lSocketID);

				// 连击属性
				vectorByte.clear();
				CHitLevelSetup::AddToByteArray(&vectorByte);
				CMessage msgHitLevelSetup(MSG_W2S_SERVER_SENDSETUP);
				msgHitLevelSetup.Add((long)SI_HITLEVEL);
				msgHitLevelSetup.Add((long)vectorByte.size());
				msgHitLevelSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgHitLevelSetup.SendToSocket(lSocketID);

				// 玩家信息
				vectorByte.clear();
				CPlayerList::AddToByteArray(&vectorByte);
				CMessage msgPlayerList(MSG_W2S_SERVER_SENDSETUP);
				msgPlayerList.Add((long)SI_PLAYERLIST);
				msgPlayerList.Add((long)vectorByte.size());
				msgPlayerList.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgPlayerList.SendToSocket(lSocketID);

				// 表情信息
				vectorByte.clear();
				CEmotion::Serialize( vectorByte );
				CMessage msgEmotion(MSG_W2S_SERVER_SENDSETUP);
				msgEmotion.Add((long)SI_EMOTION);
				msgEmotion.Add((long)vectorByte.size());
				msgEmotion.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgEmotion.SendToSocket(lSocketID);

				// 技能信息
				vectorByte.clear();
				CNewSkillFactory::CodeNewSkill(vectorByte);
				COccupSkillList::AddToByteArray(vectorByte);
				CStudySkillCondition::AddToByteArray(vectorByte);
				
				CMessage msgSkillList(MSG_W2S_SERVER_SENDSETUP);
				msgSkillList.Add((long)SI_SKILLLIST);
				msgSkillList.Add((long)vectorByte.size());
				msgSkillList.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgSkillList.SendToSocket(lSocketID);

				// 交易信息
				vectorByte.clear();
				CTradeList::AddToByteArray(&vectorByte);
				CMessage msgTradeList(MSG_W2S_SERVER_SENDSETUP);
				msgTradeList.Add((long)SI_TRADELIST);
				msgTradeList.Add((long)vectorByte.size());
				msgTradeList.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgTradeList.SendToSocket(lSocketID);

				// 增值物品交易信息
				vectorByte.clear();
				CIncrementShopList::AddToByteArray(&vectorByte);
				CMessage msgIncrementShopList(MSG_W2S_SERVER_SENDSETUP);
				msgIncrementShopList.Add((long)SI_INCREMENTSHOPLIST);
				msgIncrementShopList.Add((long)vectorByte.size());
				msgIncrementShopList.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgIncrementShopList.SendToSocket(lSocketID);

				vectorByte.clear();
				CIncrementShopManager::GetSingleton().AddToByteArray(&vectorByte);
				CMessage msgIncLimitGoodsRec( MSG_W2S_SERVER_SENDSETUP );
				msgIncLimitGoodsRec.Add((long)SI_INCLIMITGOODSREC);
				msgIncLimitGoodsRec.Add((long)vectorByte.size());
				msgIncLimitGoodsRec.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgIncLimitGoodsRec.SendToSocket(lSocketID);
				

				// 增值物品交易信息
				vectorByte.clear();
				CContributeSetup::AddToByteArray(&vectorByte);
				CMessage msgContributeSetup(MSG_W2S_SERVER_SENDSETUP);
				msgContributeSetup.Add((long)SI_CONTRIBUTEITEM);
				msgContributeSetup.Add((long)vectorByte.size());
				msgContributeSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgContributeSetup.SendToSocket(lSocketID);

				// 日志系统参数表
				vectorByte.clear();
				CLogSystem::AddToByteArray(&vectorByte);
				CMessage msgLogSystem(MSG_W2S_SERVER_SENDSETUP);
				msgLogSystem.Add((long)SI_LOGSYSTEM);
				msgLogSystem.Add((long)vectorByte.size());
				msgLogSystem.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgLogSystem.SendToSocket(lSocketID);

				// 国家参数表
				vectorByte.clear();
				GetCountryParam()->AddToByteArray(&vectorByte);
				CMessage msgCountryParam(MSG_W2S_SERVER_SENDSETUP);
				msgCountryParam.Add((long)SI_COUNTRYPARAM);
				msgCountryParam.Add((long)vectorByte.size());
				msgCountryParam.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgCountryParam.SendToSocket(lSocketID);

				// 国家信息
				vectorByte.clear();
				GetCountryHandler()->AddToByteArray(&vectorByte);
				CMessage msgCountry(MSG_W2S_SERVER_SENDSETUP);
				msgCountry.Add((long)SI_COUNTRY);
				msgCountry.Add((long)vectorByte.size());
				msgCountry.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgCountry.SendToSocket(lSocketID);

				//系统邮件
				vectorByte.clear();
				CMailList::AddToByteArray(&vectorByte);
				CMessage msgMailList(MSG_W2S_SERVER_SENDSETUP);
				msgMailList.Add((long)SI_SYSMAIL);
				msgMailList.Add((long)vectorByte.size());
				msgMailList.AddEx(&vectorByte[0],(long)vectorByte.size());
				msgMailList.SendToSocket(lSocketID);
			
				
				//脚本AI配置
				vectorByte.clear();
				CScriptAILoader::Instance().AddToByteArray( vectorByte );
				CMessage msgScriptAI( MSG_W2S_SERVER_SENDSETUP );
				msgScriptAI.Add( (long)SI_SCRIPTAI );
				msgScriptAI.Add( (long)vectorByte.size() );
				msgScriptAI.AddEx( &vectorByte[0], (long)vectorByte.size() );
				msgScriptAI.SendToSocket( lSocketID );

				//复活物品列表
				vectorByte.clear();
				CReLiveGoodsList::AddToByteArray( vectorByte );
				CMessage msgReLiveGoods( MSG_W2S_SERVER_SENDSETUP );
				msgReLiveGoods.Add( (long)SI_RELIVEGOODS );
				msgReLiveGoods.Add( (long)vectorByte.size() );
				msgReLiveGoods.AddEx( &vectorByte[0], (long)vectorByte.size() );
				msgReLiveGoods.SendToSocket( lSocketID );

				//美容配置列表
				vectorByte.clear();
				CCosmetology::AddToByteArray(vectorByte);
				CMessage msgCosme( MSG_W2S_SERVER_SENDSETUP );
				msgCosme.Add( (long)SI_COSMESYSTEM );
				msgCosme.Add( (long)vectorByte.size() );
				msgCosme.AddEx( &vectorByte[0], (long)vectorByte.size() );
				msgCosme.SendToSocket( lSocketID );
			


				//改变到的脚本数据
				// FunctionList
				if (GetGame()->GetFuncitonList_FileData())
				{
					long len = GetGame()->GetFuncitonList_FileData_Size();

					CMessage msgFunction(MSG_W2S_SERVER_SENDSETUP);
					msgFunction.Add((long)SI_FUNCTIONLIST);
					msgFunction.Add(len);
					msgFunction.AddEx((void*)GetGame()->GetFuncitonList_FileData(), len);
					msgFunction.SendToSocket(lSocketID);
				}

				// VariableList
				if (GetGame()->GetVariableList_FileData())
				{
					long len = GetGame()->GetVariableList_FileData_Size();

					CMessage msgVariable(MSG_W2S_SERVER_SENDSETUP);
					msgVariable.Add((long)SI_VARIABLELIST);
					msgVariable.Add(len);
					msgVariable.AddEx((void*)GetGame()->GetVariableList_FileData(), len);
					msgVariable.SendToSocket(lSocketID);
				}

				// GeneralVariableList
				if (GetGame()->GetGeneralVariableList() && GetGame()->GetInitVarLoadFlag())
				{
					CMessage msgGereralVariable(MSG_W2S_SERVER_SENDSETUP);
					msgGereralVariable.Add((long)SI_GENERALVARIABLELIST);
					vector<BYTE> GererData;
					GetGame()->GetGeneralVariableList()->AddToByteArray(&GererData);
					msgGereralVariable.Add((long)GererData.size());
					if(GererData.size() > 0)
						msgGereralVariable.AddEx(&GererData[0],GererData.size());
					msgGereralVariable.SendToSocket(lSocketID);
				}

				// 脚本文件
				for(map<string, char*>::iterator it2=GetGame()->GetScriptFileDataMap()->begin(); it2!=GetGame()->GetScriptFileDataMap()->end(); it2++)
				{
					string filename = it2->first;
					CMessage msgScript(MSG_W2S_SERVER_SENDSETUP);
					msgScript.Add((long)SI_SCRIPTFILE);
					msgScript.Add(filename.c_str());
					long lLen = (long)lstrlen(it2->second);
					msgScript.Add(lLen);
					msgScript.AddEx(it2->second,lLen);
					msgScript.SendToSocket(lSocketID);
				}

				// Normal Rgn
				for(map<long, CGlobalRgnManager::tagRegion>::iterator it = GetGame()->GetGlobalRgnManager()->GetTemplateRegionListMap()->begin(); it!=GetGame()->GetGlobalRgnManager()->GetTemplateRegionListMap()->end(); it++)
				{
					CGlobalRgnManager::tagRegion* pRegion = &it->second;
					if( pRegion )
					{
						if (pRegion->dwGameServerIndex == pGameServer->dwIndex)
						{
							vectorByte.clear();
							pRegion->pRegion->AddToByteArray(&vectorByte);
							CMessage msgTradeList(MSG_W2S_SERVER_SENDSETUP);
							msgTradeList.Add((long)SI_REGION);
							msgTradeList.Add((long)pRegion->RegionType);
							msgTradeList.Add((long)pRegion->pRegion->GetID());
							msgTradeList.Add(pRegion->pRegion->GetExID());
							msgTradeList.Add((long)vectorByte.size());
							msgTradeList.AddEx(&vectorByte[0], (long)vectorByte.size());
							msgTradeList.SendToSocket(lSocketID);
							Sleep(10);
						}
					}
				}
			
				// 场景等级列表
				vectorByte.clear();
				CRegionSetup::AddToByteArray(&vectorByte);
				CMessage msgRegionSetup(MSG_W2S_SERVER_SENDSETUP);
				msgRegionSetup.Add((long)SI_REGIONLEVELSETUP);
				msgRegionSetup.Add((long)vectorByte.size());
				msgRegionSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgRegionSetup.SendToSocket(lSocketID);

				//发送副本配置信息
				vectorByte.clear();
				GetGame()->GetDupliRegionSetup()->AddToByteArray(&vectorByte);
				CMessage msgDupliRegionSetup(MSG_W2S_SERVER_SENDSETUP);
				msgDupliRegionSetup.Add((long)SI_DUPLIREGIONSETUP);
				msgDupliRegionSetup.Add((long)vectorByte.size());
				msgDupliRegionSetup.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgDupliRegionSetup.SendToSocket(lSocketID);

				//QuestList
				vectorByte.clear();
				CQuestSystem::AddToByteArray(&vectorByte);
				CMessage msgQuest(MSG_W2S_SERVER_SENDSETUP);
				msgQuest.Add((long)SI_QUEST);
				msgQuest.Add((long)vectorByte.size());
				msgQuest.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgQuest.SendToSocket(lSocketID);

				// GMLIST
				vectorByte.clear();
				CGMList::AddToByteArray(&vectorByte);
				CMessage msgGMList(MSG_W2S_SERVER_SENDSETUP);
				msgGMList.Add((long)SI_GMLIST);
				msgGMList.Add((long)vectorByte.size());
				msgGMList.AddEx(&vectorByte[0], (long)vectorByte.size());
				msgGMList.SendToSocket(lSocketID);

				CMessage msgGoodsID(MSG_W2S_SERVER_SENDSETUP);
				msgGoodsID.Add((long)SI_GAMESERVERINDEX);
				msgGoodsID.Add((BYTE)pGameServer->dwIndex);
				msgGoodsID.SendToSocket(lSocketID);

				GetOrganiCtrl()->OnGsConnect(lSocketID);
				//! 队伍配置
				GetOrganiCtrl()->SendTeamSetupToGS(lSocketID);
				GetOrganiCtrl()->SendTeamDataToGS(lSocketID);

				//! 小精灵系统
				vectorByte.clear();
				SpriteSystem::SetupAddToByteArray(vectorByte);
				CMessage msgSprite(MSG_W2S_SERVER_SENDSETUP);
				msgSprite.Add((LONG)SI_SPRITE);
				msgSprite.Add((long)vectorByte.size());
				msgSprite.AddEx(&vectorByte[0], vectorByte.size());
				msgSprite.SendToSocket(lSocketID);
				
				pGameServer->bConnected = true;
				CMessage::MapGSIDSocket(pGameServer->dwIndex,lSocketID);
				
				map<long, CGlobalRgnManager::tagRegion>* pPMap = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalRgnListMap();
				map<long, CGlobalRgnManager::tagRegion>* pTMap = GetGame()->GetGlobalRgnManager()->GetTemplateTeamRgnListMap();
				if(pPMap && pTMap)
				{
					CMessage msgDupData(MSG_W2S_SERVER_SENDSETUP);
					msgDupData.Add((LONG)SI_DUPRGNDATA);
					msgDupData.Add((long)pPMap->size() + pTMap->size()); // 副本模板个数

					map<long, CGlobalRgnManager::tagRegion>::iterator prgnitr = pPMap->begin();
					for(; prgnitr != pPMap->end(); prgnitr++)
					{
						msgDupData.Add((long)prgnitr->first); // 副本模板ID
						vector<BYTE> pBA;
						if(prgnitr->second.pRegion)
						{
							CWorldRegion* pRgn = MP_NEW CWorldRegion;
							pRgn->CloneAllData(prgnitr->second.pRegion);
							pRgn->AddToByteArray(&pBA);
							delete pRgn;
						}
						msgDupData.Add((long)pBA.size()); // bufsize
						if(pBA.size() > 0)
							msgDupData.AddEx(&pBA[0], (long)pBA.size()); // buf	
					}

					map<long, CGlobalRgnManager::tagRegion>::iterator trgnitr = pTMap->begin();
					for(; trgnitr != pTMap->end(); trgnitr++)
					{
						msgDupData.Add((long)trgnitr->first); // 副本模板ID
						vector<BYTE> pBA;
						if(trgnitr->second.pRegion)
						{
							CWorldRegion* pRgn = MP_NEW CWorldRegion;
							pRgn->CloneAllData(trgnitr->second.pRegion);
							pRgn->AddToByteArray(&pBA);
							delete pRgn;
						}
						msgDupData.Add((long)pBA.size()); // bufsize
						if(pBA.size() > 0)
							msgDupData.AddEx(&pBA[0], (long)pBA.size()); // buf	
					}

					msgDupData.SendToSocket(lSocketID);
				}
				
				//发送声望配置
				CMessage msgCredit(MSG_W2S_SERVER_SENDSETUP);
				msgCredit.Add((long)SI_CREDITSYSTEM);
				DBWriteSet dbwriteset;
				msgCredit.GetDBWriteSet(dbwriteset);
				GetInst(CCreditSystem).CodeToByteArray(dbwriteset);
				msgCredit.SendToSocket(lSocketID);

				// 家族跑商配置
				GetInst( FBusiness::CConfiger ).SendToGS( lSocketID );

				//发送任务引导配置
				CMessage msgQuestIndex(MSG_W2S_SERVER_SENDSETUP);
				msgQuestIndex.Add((long)SI_QUESTINDEX);
				DBWriteSet dbwriteset_Index;
				msgQuestIndex.GetDBWriteSet(dbwriteset_Index);
				GetInst(CQuestIndexXml).CodeToByteArray(dbwriteset_Index);
				msgQuestIndex.SendToSocket(lSocketID);

				//发送彩票系统配置
				CMessage msgLottery(MSG_W2S_SERVER_SENDSETUP);
				msgLottery.Add((long)SI_Lottery);
				DBWriteSet dbwriteset_Lottery;
				msgLottery.GetDBWriteSet(dbwriteset_Lottery);
				GetInst(CLotteryXml).CodeToByteArray(dbwriteset_Lottery);
				msgLottery.SendToSocket(lSocketID);

				// 发送让GameServer正式启动的消息
				CMessage msg(MSG_W2S_SERVER_SENDSETUP);
				msg.Add((long)SI_END);
				msg.Add((long)lGsSleepTime);
				msg.SendToSocket(lSocketID);

				lGsSleepTime++;

				// 检查是否所有GS都连接完成并且DBS准备完成, 成功则发送saveplayer缓冲区的数据
				bool isSaveFlag = false;
				map<DWORD, CGame::tagGameServer>::iterator gsItr = GetGame()->GetGameServerList()->begin();
				for(; gsItr != GetGame()->GetGameServerList()->end(); gsItr++)
				{
					if(gsItr->second.bConnected)
						isSaveFlag = true;
					else
					{
						isSaveFlag = false;
						break;
					}
				}

				// 计数清零
				if( GetGame()->GetGameServerList() && lGsSleepTime > GetGame()->GetGameServerList()->size()) lGsSleepTime = 0;

				if(isSaveFlag && GetGame()->GetDbIsReady())
				{
					//DoSaveData();
					GetGame()->ProcessDelaySavePlayer();
					GetGame()->SendCdkeyToLoginServer(TRUE);
				}
				else
					GetGame()->SendCdkeyToLoginServer(FALSE);
			}
		}
		break;
		
	case MSG_W2W_SERVER_LOGINSERVERCLOSE:
		{
			AddLogText("========= LoginServer closed =========");
			GetGame()->CreateConnectLoginThread();
		}
		break;

		// 计费服务器断开
	case MSG_W2W_SERVER_COSTSERVERCLOSE:
		{
			AddLogText("========= CostServer closed =========");
			GetGame()->CreateConnectCostThread();
		}
		break;

	case MSG_W2W_RECONNECTLOGINSERVEROK:
		{
			
		}
		break;

	case MSG_W2W_RECONNECTCOSTSERVEROK:
		{
		
		}
		break;

		// 切换服务器
	case MSG_S2W_SERVER_CHANGEREGION:
		{
			// RgnType
			eRgnType rgnType = (eRgnType)pMsg->GetLong();
			// 普通场景用模板ID可以查找到GUID
			long templateID = pMsg->GetLong(); 

			CGUID guid; pMsg->GetGUID(guid);
			CGUID lRegionID;
			pMsg->GetGUID(lRegionID);

			CPlayer* pPlayer = GetGame()->GetMapPlayer(guid);
			if(!pPlayer)
			{
				char szGuid[128];
				guid.tostring(szGuid);
				AddLogText("ChangeRgn,未找到CPlayer对象, GUID[%s]!", szGuid);
				// 切换场景失败 
				CMessage msg(MSG_W2S_SERVER_CHANGEREGION);
				// 添加验证代码
				msg.Add((long)0xFFFFFFFF);
				msg.Add((char)0);	// 失败
				msg.Add(guid);
				msg.SendToSocket(lSocketID);
				return;
			}

			long lTileX = pMsg->GetLong();
			long lTileY = pMsg->GetLong();
			long lDir = pMsg->GetLong();
			long lUseGoods = pMsg->GetLong();
			long lRange = pMsg->GetLong();

			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);
			pPlayer->DecodeFromDataBlock(setReadDB);
			
			const char* szCdkey = pPlayer->GetAccount();
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
				char outStr[1024];
				sprintf(outStr, "MSG_S2W_SERVER_CHANGEREGION! LoginInfo[%s] Is Null!", szCdkey);
				AddLogText(outStr);
				// 切换场景失败 
				CMessage msg(MSG_W2S_SERVER_CHANGEREGION);
				// 添加验证代码
				msg.Add((long)0xFFFFFFFF);
				msg.Add((char)0);	// 失败
				msg.Add(guid);
				msg.SendToSocket(lSocketID);
				return;
			}
			if(tLoginInfo)
			{
				// 创建验证码
				long lTmpSignCode = 0;
				GetGame()->GenerateSignCodeByTime(lTmpSignCode);
				tLoginInfo->SetSignCode(lTmpSignCode);
				tLoginInfo->SetPlayerState(PLAYER_STATE_CHANGING_SERVER);
				tLoginInfo->SetLoginTimeoutValue(timeGetTime());
			}
			else
			{
				AddLogText("ChangeRgn,未找到tLoginInfo对象, cdkey[%s]!", szCdkey);
				return;
			}

			// 普通场景
			if(rgnType == RGN_NORMAL)
			{
				CGlobalRgnManager::tagRegion* tRgn = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(templateID);
				if(tRgn)
					lRegionID = tRgn->pRegion->GetExID();
			}

			CWorldRegion* pRegion = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(lRegionID);
			long changeGSID = -1;
			if(pRegion)
				changeGSID = pRegion->GetGsid();
		
			if (!pRegion)
			{
				// 切换场景失败 
				CMessage msg(MSG_W2S_SERVER_CHANGEREGION);
				// 添加验证代码
				msg.Add((long)tLoginInfo->GetSignCode());
				msg.Add((char)0);	// 失败
				msg.Add(guid);
				msg.SendToSocket(lSocketID);

				// 发送一次保存消息
				GetGame()->GetEntityManager()->CreateSavePlayerDataSession(pPlayer->GetExID(), SAVE_DETAIL_ALL);
				GetGame()->GetEntityManager()->CreateSaveLoginPlayerDataSession(pPlayer->GetExID());

				char szGuid[40] = {0};
				guid.tostring(szGuid);
				char pszLogingInfo[512]="";
				_snprintf(pszLogingInfo,512, CStringReading::LoadString(IDS_WS_SERVERMESSAGE, STR_WS_SERVERMESSAGE_CHANGERGNNORGNONGS), templateID, szGuid);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				return;
			}

			CGame::tagGameServer* pServer = GetGame()->GetGameServer(changeGSID);
			if (!pServer || pServer->bConnected == false)
			{
				// 设置状态
				tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
				
				// 发送一次保存消息
				GetGame()->GetEntityManager()->CreateSavePlayerDataSession(pPlayer->GetExID(), SAVE_DETAIL_ALL);
				GetGame()->GetEntityManager()->CreateSaveLoginPlayerDataSession(pPlayer->GetExID());

				// 切换场景失败
				CMessage msg(MSG_W2S_SERVER_CHANGEREGION);
				// 添加验证代码
				msg.Add((long)tLoginInfo->GetSignCode());
				msg.Add((char)0);	// 失败
				msg.Add(guid);
				msg.SendToSocket(lSocketID);
				char szGuid[40] = {0};
				guid.tostring(szGuid);
				char pszLogingInfo[512]="";
				_snprintf(pszLogingInfo,512, CStringReading::LoadString(IDS_WS_SERVERMESSAGE, STR_WS_SERVERMESSAGE_CHANGERGNGSNOTRUN), templateID, szGuid, changeGSID);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				return;
			}

			pPlayer->SetRegionExID(lRegionID);
			pPlayer->SetPosXY(lTileX+0.5f, lTileY+0.5f);
			pPlayer->SetDir(lDir);
			pPlayer->SetRegionID(pRegion->GetID());
			pPlayer->SetCurRgnType(rgnType);

			// Online -> Login
			// 添加到登陆列表中
			pPlayer->SetPlayerLogin(true);
			pPlayer->SetFirstLogin(false);
			pPlayer->SetBeginLoginTime(timeGetTime());
			
			// 发送一次保存消息
			GetGame()->GetEntityManager()->CreateSavePlayerDataSession(pPlayer->GetExID(), SAVE_DETAIL_ALL);
			GetGame()->GetEntityManager()->CreateSaveLoginPlayerDataSession(pPlayer->GetExID());

			// 切换场景成功
			CMessage msg(MSG_W2S_SERVER_CHANGEREGION);
			// 添加验证代码
			msg.Add((long)tLoginInfo->GetSignCode());
			msg.Add((char)1);	// 成功
			msg.Add(guid);
			msg.Add(pServer->strIP.c_str());
			msg.Add(pServer->dwPort);
			msg.SendToSocket(lSocketID);
		}
		break;
	case MSG_S2W_SERVER_ADDINFO:
		/**## 请求发送消息给其他GameServer的玩家 */
		{
			char	strName[24];
			char	strMsg[1024];
			DWORD	dwColor = 0;
			DWORD	dwBkColor = 0;
			CGUID	SenderGuid;

			/**## 变量初始化 */
			ZeroMemory( strName, sizeof( strName ) );
			ZeroMemory( strMsg,  sizeof( strMsg )  );

			pMsg -> GetStr( strName,24 );
			pMsg -> GetStr( strMsg,1024 );
			dwColor = pMsg -> GetDWord();
			dwBkColor = pMsg->GetDWord();
			pMsg -> GetGUID(SenderGuid);

			CPlayer* pPlayer;
			pPlayer = GetGame() -> GetMapPlayer( strName );
			if( pPlayer )
				/**## 找到用户，发送消息给用户所在的服务器 */
				/**## 成功：LONG Reciver, char* Msg, DWORD Color, char* Sender    
				## 失败：LONG Reciver(=0), char* Reciver, LONG Sender */
			{
				/**## 获取用户当前所在的GameServer */
				long lGameServer =
					GetGame() -> GetGameServerNumber_ByPlayerID( pPlayer->GetExID() );
				if( lGameServer )
				{
					CMessage msg( MSG_W2S_SERVER_ADDINFO );
					msg.Add( pPlayer->GetExID() );
					msg.Add( strMsg );
					msg.Add( dwColor );
					msg.Add( dwBkColor );
					/**## 填写发送者的名称 */
					CPlayer* pSender = GetGame() -> GetMapPlayer( SenderGuid );
					if( pSender )
					{
						msg.Add( const_cast<char*>( pSender -> GetName() ) );
					}
					else
					{
						char strSender[256];
						char szSenderGuid[40] = {0};
						SenderGuid.tostring(szSenderGuid);
						sprintf( strSender, "uid[%s]", szSenderGuid );
						msg.Add( strSender );
					}
					msg.SendGSID( lGameServer );
				}
			}
			else
				/**## 找不到用户，返回消息给请求者 */
			{
				CPlayer* pSender = GetGame() -> GetMapPlayer( SenderGuid );
				if( pSender )
				{
					long lGameServer =
						GetGame() -> GetGameServerNumber_ByPlayerID( pSender -> GetExID() );
					if( lGameServer )
					{
						CMessage msg( MSG_W2S_SERVER_ADDINFO );
						msg.Add( static_cast<long>(0) );
						msg.Add( strName );
						msg.Add( SenderGuid );
						msg.SendGSID( lGameServer );
					}
				}
			}
		}
		break;

		// 更新全局脚本变量的值
	case MSG_S2W_SERVER_CHANGE_SCRIPT_VARIABLE:
		{
			char strVarName[256] = {0};
			pMsg->GetStr(strVarName, 256);
			if( strVarName )
			{
				//统计次数
				//添加到map中,由于添加函数里面进行了加操作，这里只传入值
				GetGame()->AddVarScriptExpendTime(strVarName,1);

				long lPos = 0;
				int lRet = CVariableList::VAR_NOT_FOUND;
				double nValue = 0;
				LONG64 llRet = 0;
				BYTE nFlag = 0;
				char szStr[1024] = {0};
				CGUID guid;

				nFlag = pMsg->GetByte(); // 0 数值
				if( eVarNumber == nFlag )
				{
					lPos = pMsg->GetLong();
					llRet = pMsg->GetLONG64();
					nValue = *(double*)&llRet;
					lRet = GetGame()->GetGeneralVariableList()->SetVarValue(strVarName, lPos, nValue);
				}
				else if( eVarString == nFlag ) // 1 字符串
				{
					pMsg->GetStr(szStr, 1024);
					lRet = GetGame()->GetGeneralVariableList()->SetVarValue(strVarName, szStr);
				}
				else if( eVarGuid == nFlag ) // 2 guid
				{
					pMsg->GetGUID(guid);
					lRet = GetGame()->GetGeneralVariableList()->SetGuidValue(strVarName, &guid);
				}
				CEntityManager::DBEntityComponentMapItr varMapItr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(string("[scriptvargroup]"));
				if( varMapItr == GetGame()->GetEntityManager()->GetBaseEntityMap().end() )
				{
					AddLogText("MSG_S2W_OTHER_UPDATEGLOBALSCTIPTVARS 未找到VarGroup对象,请检查配置文件!");
					return;
				}
				map<CGUID, CEntityGroup*>::iterator enItr = varMapItr->second.begin();
				if( enItr == varMapItr->second.end() )
				{
					AddLogText("MSG_S2W_OTHER_UPDATEGLOBALSCTIPTVARS 未找到VarGroup对象,请检查对象初始化数据!");
					return;
				}
				CEntityGroup* pVarGroup = (CEntityGroup*)enItr->second;
				if( pVarGroup )
				{
					pVarGroup->ReleaseChilds();
					GetGame()->GetEntityManager()->CGlobalVarGroupToCGDBlobalVar(pVarGroup);
				}

				if( lRet != CVariableList::VAR_NOT_FOUND )
				{
					CMessage msg(MSG_W2S_SERVER_CHANGE_SCRIPT_VARIABLE);
					msg.Add(strVarName);
					msg.Add((BYTE)nFlag);
					if( eVarNumber == nFlag )
					{
						msg.Add((long)lPos);
						msg.Add((LONG64)llRet);
					}
					else if( eVarString == nFlag )
						msg.Add(szStr);
					else if( eVarGuid == nFlag )
						msg.Add(guid);
					msg.SendAll();
				}
			}
		}
		break;

		//##游戏服务器向世界服务器发送举报杀人者的消息
	case MSG_S2W_SERVER_REPORT_MURDERER:
		{
			LONG lSuffererType			= pMsg -> GetLong();
			LONG lSuffererID			= pMsg -> GetLong();
			LONG lMurdererType			= pMsg -> GetLong();
			CGUID MurdererID;
			pMsg -> GetGUID(MurdererID);
			//##查询玩家在那个服务器
			long lGameServer	= GetGame() -> GetGameServerNumber_ByPlayerID( MurdererID );
			if( lGameServer )
			{
				pMsg -> SetType( MSG_W2S_SERVER_SYNC_MURDERER_COUNT );
				pMsg -> SendGSID( lGameServer );
			}
			//##如果不在线,则尝试在本地查找
			else 
			{
				CPlayer* pPlayer = GetGame() -> GetMapPlayer( MurdererID );
				if( pPlayer )
				{
					pPlayer -> SetPkCount( pPlayer -> GetPkCount() + 1 );
					pPlayer -> SetPkValue( pPlayer -> GetPkValue() + 1 );
				}
				//##如果找不到,是否直接修改数据库?
				//##Jar@2004-7-16
			}
		}
		break;

	case MSG_S2W_SERVER_UpdateRegionInfo:
		{
			long lRegionID = pMsg->GetLong();
			//CGame::tagRegion* ptagRegion = GetGame()->GetRegion(lRegionID);
			CGlobalRgnManager::tagRegion* ptagRegion = GetGame()->GetGlobalRgnManager()->GetTemplateNormalRegion(lRegionID);
			if (ptagRegion && ptagRegion->pRegion)
			{
				long lBufSize = pMsg->GetLong();
				BYTE *pRegionData = (BYTE*)M_ALLOC(lBufSize);
				pMsg->GetEx(pRegionData,lBufSize);
				long lPos = 0;
				ptagRegion->pRegion->DecordFromByteArray(pRegionData,lPos);
				M_FREE(pRegionData,lBufSize);
			}
		}
		break;
	// DBServer的定时ping
	case MSG_DB2W_SERVER_QUEST_PING:
		{
			GetGame()->EndPingDS();

			if(GetGame()->GetPingGsNum() >= GetGame()->GetGSNum() && GetGame()->FinishPingDS())
			{
				CMessage msg(MSG_W2L_SERVER_ANSWER_PING);
				msg.Add((long)1);
				msg.SendToSocket(GetGame()->GetLSSocketID());
			}
		}
		break;
		// LoginServer的定时ping
	case MSG_L2W_SERVER_QUEST_PING:
		{
			GetGame()->SetInPing(true);
			GetGame()->ClearPingGameServerInfo();
			GetGame()->SetLastPingGameServerTime(timeGetTime());

			GetGame()->BeginPingDS();

			CMessage msg(MSG_W2S_SERVER_QUEST_PING);
			msg.Add((DWORD)pMsg->GetType());
			msg.SendAll();

			CMessage msgDB(MSG_W2DB_SERVER_QUEST_PING);
			msgDB.SendToSocket(GetGame()->GetDBSocketID());


			GetGame()->ReturnGsPlayerNum();
			
		}
		break;

		// 收到GameServer的返回
	case MSG_S2W_SERVER_ANSWER_PING:
		{
			char strIP[256];
			CGame::tagPingGameServerInfo stPingGame;
			stPingGame.dwPlayerNum = pMsg->GetLong();

			DWORD flag = pMsg->GetDWord();

			DWORD dwIP = pMsg->GetIP();
			sprintf(strIP, "%d.%d.%d.%d", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)));

			CGame::tagGameServer* gs = GetGame()->GetGameServer(pMsg->GetGSID());
			if(gs)
			{
				stPingGame.strIP = strIP;
				stPingGame.dwPort = gs->dwPort;
				gs->lPlayerNum = stPingGame.dwPlayerNum;
			}
			if(flag == MSG_L2W_SERVER_QUEST_PING)
			{
				GetGame()->AddPingGameServerInfo(stPingGame);

				if(GetGame()->GetPingGsNum() >= GetGame()->GetGSNum() && GetGame()->FinishPingDS())
				{
				CMessage msg(MSG_W2L_SERVER_ANSWER_PING);
				msg.Add((long)1);
				msg.SendToSocket(GetGame()->GetLSSocketID());
			}
		}
		}
		break;
	case MSG_S2W_SERVER_CREATEMONSTER:
		{
			CGUID lRegionID;
			pMsg -> GetGUID(lRegionID);
			
			pMsg -> SetType( MSG_W2S_SERVER_CREATEMONSTER );
			long lIndex = -1;
			CWorldRegion* rgn = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(lRegionID);
			if(rgn)
				lIndex = rgn->GetGsid();
			GetGame() -> SendMsg2GameServer( lIndex, pMsg );
		}
		break;
	case MSG_S2W_SERVER_CREATEPHGOODS:
	case MSG_S2W_SERVER_UPDATEPHGOODS:
		{
			BYTE bType = pMsg->GetByte();
	
			CGUID playerID = NULL_GUID;

			CGUID guid;
			
			CGUID phRgnID;
			
			char name[64];
			
			long dir = 0;
			long width = 0;
			long height = 0;
			long posX = 0;
			long posY = 0;
			long templateID = 0;

			if(bType == 1) // 本地创建
			{
				pMsg->GetGUID(guid);
				pMsg->GetGUID(phRgnID);
				pMsg->GetStr(name, 64);
				dir = pMsg->GetLong();
				width = pMsg->GetLong();
				height = pMsg->GetLong();
				posX = pMsg->GetLong();
				posY = pMsg->GetLong();
				templateID = pMsg->GetLong();

				pMsg -> SetType( MSG_W2S_SERVER_CREATEPHGOODS );
				long lIndex = pMsg->GetGSID();
				CWorldRegion* rgn = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(phRgnID);
				if(rgn)
					playerID = rgn->GetOwnerGUID();
			}
			else
			{
				pMsg->GetGUID(guid);
				pMsg->GetGUID(phRgnID);
				pMsg->GetStr(name, 64);
				dir = pMsg->GetLong();
				width = pMsg->GetLong();
				height = pMsg->GetLong();
				posX = pMsg->GetLong();
				posY = pMsg->GetLong();
				templateID = pMsg->GetLong();

				CGUID phRgnID = NULL_GUID;
				CPlayer* player = GetGame()->GetMapPlayer(phRgnID);
				if(player == NULL) return;

				phRgnID = player->GetPersonalHouseRgnGUID();

				pMsg -> SetType( MSG_W2S_SERVER_CREATEPHGOODS );
				long lIndex = -1;
				CWorldRegion* rgn = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(phRgnID);
				if(rgn)
				{
					lIndex = rgn->GetGsid();
					playerID = rgn->GetOwnerGUID();
				}

				// 不是在发送消息的GS上创建goods
				GetGame() -> SendMsg2GameServer( lIndex, pMsg );
			}

			// 加入PHGoods容器
			CEntityGroup* dbDupRgn = NULL;
			CEntityManager::DBEntityComponentMapItr itr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(string("[phrgngroup]"));
			if(itr != GetGame()->GetEntityManager()->GetBaseEntityMap().end())
			{
				if(itr->second.size() != 1)
				{
					AddLogText("添加个人房屋物品时phrgngroup对象错误!");
					return;
				}
				
				map<CGUID, CBaseEntity*>::iterator rgnItr = itr->second.begin()->second->GetEntityGroupMap().find(phRgnID);
				if(rgnItr != itr->second.begin()->second->GetEntityGroupMap().end())
				{
					dbDupRgn = (CEntityGroup*)rgnItr->second;
				}
			}
			else
			{
				dbDupRgn = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgn]"), phRgnID);
				itr->second.begin()->second->AddChild(dbDupRgn);
			}

			if(dbDupRgn)
			{
				CEntity* rgnGoods = (CEntity*)GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgngoods]"), NULL_GUID);
				rgnGoods->SetGUID(guid);
				rgnGoods->SetGuidAttr(string("DupRgnGUID"),		dbDupRgn->GetGUID());
				rgnGoods->SetGuidAttr(string("guid"),			guid);
				rgnGoods->SetStringAttr(string("Name"),			name);
				rgnGoods->SetLongAttr(string("Dir"),			dir);
				rgnGoods->SetLongAttr(string("Width"),			width);
				rgnGoods->SetLongAttr(string("Height"),			height);
				rgnGoods->SetLongAttr(string("PosX"),			posX);
				rgnGoods->SetLongAttr(string("PosY"),			posY);
				if(dbDupRgn->GetEntityGroupMap().size() != 1)
				{
					AddLogText("dbrgn对象的子节点错误,请检查配置文件!");
					return;
				}
				CEntityGroup* pRgnGoodsGroup = (CEntityGroup*)GetGame()->GetEntityManager()->FindLeafByComFlag(dbDupRgn, string("[phrgngoodsgroup]"));
				if(pRgnGoodsGroup)
					pRgnGoodsGroup->AddChild(rgnGoods);
			}
		}
		break;
	case MSG_S2W_SERVER_DELETEPHGOODS:
		{
			BYTE bType = pMsg->GetByte();
			CGUID lRegionID;
		
			CGUID guid;
	
			char name[64];
			pMsg->GetStr(name, 64);
			long dir = 0;
			long width = 0;
			long height = 0;
			long posX = 0;
			long posY = 0;
			
			pMsg -> GetGUID(lRegionID);
			pMsg->GetGUID(guid);
			pMsg->GetStr(name, 64);
			//dir = pMsg->GetLong();
			width = pMsg->GetLong();
			height = pMsg->GetLong();
			posX = pMsg->GetLong();
			posY = pMsg->GetLong();

			long lIndex = -1;
			CWorldRegion* rgn = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(lRegionID);
			if(rgn)
				lIndex = rgn->GetGsid();

			// 从PHGoods容器中删除
			bool isFindFlag = false;
			CEntityGroup* dbDupRgn = NULL;
			CEntityManager::DBEntityComponentMapItr itr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(string("[phrgngroup]"));
			if(itr != GetGame()->GetEntityManager()->GetBaseEntityMap().end())
			{
				if(itr->second.size() == 0)
				{
					AddLogText("添加个人房屋物品时未找到phrgngroup对象!");
					return;
				}
				map<CGUID, CEntityGroup*>::iterator rgnItr = itr->second.find(lRegionID);
				if(rgnItr != itr->second.end())
				{
					dbDupRgn = rgnItr->second;
				}
			}
			else
			{
				AddLogText("删除个人房屋物品时未找到phrgn对象!");
			}
			if(dbDupRgn)
			{
				if(dbDupRgn->GetEntityGroupMap().size() == 0)
				{
					AddLogText("删除个人房屋物品时dbrgn对象的子节点为空,请检查配置文件!");
					return;
				}
				CEntityGroup* pRgnGoodsGroup = (CEntityGroup*)GetGame()->GetEntityManager()->FindLeafByComFlag(dbDupRgn, string("[phrgngoodsgroup]"));
				if(pRgnGoodsGroup)
					pRgnGoodsGroup->ReleaseLeaf(guid);
			}
			pMsg -> SetType( MSG_W2S_SERVER_DELETEPHGOODS );
			pMsg->SendGSID(lIndex);
		}
		break;
	case MSG_S2W_SERVER_SAVEPERSONALHOUSE:
		{
			long bufSize = pMsg->GetLong();
			BYTE *byteArray = (BYTE*)M_ALLOC(bufSize);
			pMsg->GetEx(byteArray, bufSize);

			long pos = 0;
			CGUID rgnGUID;
			_GetBufferFromByteArray(byteArray, pos, rgnGUID); // 所有者ID

			// 找到对应的GS的RgnManager
			CWorldRegion* rgn = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(rgnGUID);
		
			long GSID = CMessage::GetGSIDBySocket(lSocketID);

			if(rgn)
			{	
				CGUID guid;
				_GetBufferFromByteArray(byteArray, pos, guid, bufSize);
				char name[128];
				memset(name, 0, sizeof(name));
				_GetStringFromByteArray(byteArray, pos, name, bufSize);
			
				CGUID ownerID;
				_GetBufferFromByteArray(byteArray, pos, ownerID, bufSize);
				long templateID = _GetLongFromByteArray(byteArray, pos, bufSize);

				long lResourceID = _GetLongFromByteArray(byteArray, pos, bufSize);

					// 保存到数据库的变量数组
					vector<BYTE> pVarBA;
					long varPos = pos;

					char strName[256];
					// 普通变量个数
					long lVarNum = _GetLongFromByteArray(byteArray, pos, bufSize);		// 变量个数

					//GUID个数
					long num = _GetLongFromByteArray( byteArray, pos, bufSize);		// 变量个数

					// buf size
					long tBufSize = _GetLongFromByteArray( byteArray, pos, bufSize); // 变量长度

					// 保存到数据库的变量数组
					for(int tVarPos=0; tVarPos<sizeof(long); tVarPos++)
					{
						pVarBA.push_back(((char*)(&lVarNum))[tVarPos]);
						varPos++;
					}
					for(int tVarPos=0; tVarPos<sizeof(long); tVarPos++)
					{
						pVarBA.push_back(((char*)(&num))[tVarPos]);
						varPos++;
					}
					for(int tVarPos=0; tVarPos<sizeof(long); tVarPos++)
					{
						pVarBA.push_back(((char*)(&tBufSize))[tVarPos]);
						varPos++;
					}
					for(int tVarPos=0; tVarPos<tBufSize; tVarPos++)
					{
						pVarBA.push_back(byteArray[varPos]);
						varPos++;
					}

					double value = 0;
					long lArray = 0;
					for(int i=0; i<lVarNum; i++)
					{
						_GetStringFromByteArray(byteArray, pos, strName, bufSize);
						lArray = _GetLongFromByteArray(byteArray, pos, bufSize);
						if( lArray == 0 )	// 个体变量
						{
							value = _GetDoubleFromByteArray(byteArray, pos, bufSize);
							rgn->GetVariableList()->AddVar(strName, value);
						}
						else if(lArray > 0)	// 数组
						{
							rgn->GetVariableList()->AddVar(strName, lArray, 0.0f);

							for(int j=0; j<lArray; j++)
							{
								value = _GetDoubleFromByteArray(byteArray, pos, bufSize);
								rgn->GetVariableList()->SetVarValue(strName, j, value);
							}
						}
						else
						{		
							//字符串
							char tmpString[1024];
							_GetStringFromByteArray(byteArray, pos, tmpString, bufSize);
							rgn->GetVariableList()->AddVar(strName, tmpString);
						}
					}

					// GUID
					char GuidName[256];
					CGUID tGuid;
					for(int i=0; i<num; i++)
					{
						_GetStringFromByteArray( byteArray, pos, GuidName, bufSize );	// 名字	
						_GetBufferFromByteArray(byteArray, pos, tGuid, bufSize);	// GUID变量
						rgn->GetVariableList()->AddGuid(GuidName, tGuid);
					}
				
					CEntityGroup* dupRgn = NULL;
					CEntityManager::DBEntityComponentMapItr phRgnMapItr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(string("[phrgngroup]"));
					if(phRgnMapItr != GetGame()->GetEntityManager()->GetBaseEntityMap().end())
					{
						if(phRgnMapItr->second.size() == 0)
						{
							AddLogText("添加个人房屋物品时未找到phrgngroup对象!");
							return;
						}
						map<CGUID, CEntityGroup*>::iterator rgnItr = phRgnMapItr->second.find(rgnGUID);
						if(rgnItr != phRgnMapItr->second.end())
						{
							dupRgn = rgnItr->second;
						}
						else
						{
							dupRgn = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgn]"), rgnGUID);
							GetGame()->GetEntityManager()->CPhRgnToCDBPhRgn(rgn, dupRgn);
							dupRgn->SetGuidAttr(string("OwnerGUID"),  ownerID);
							dupRgn->SetGuidAttr(string("guid"),  rgnGUID);
						}
					}
					else
					{
						AddLogText("删除个人房屋物品时未找到phrgn对象!");
					}

					if(dupRgn != NULL)
					{
						// 房间物品
						long goodsNum = _GetLongFromByteArray(byteArray, pos, bufSize);
						if (goodsNum != 0)
						{
							struct PHGoods
							{
								// 物品GUID
								CGUID guid;
								// 物品名字
								char name[64];
								// 朝向
								int dir;
								// X长度
								int width;
								// Y长度
								int height;
								// 坐标
								float posX;
								float posY;
							};

							for(int i=0; i<goodsNum; i++)
							{
								CEntity* tdrGoods = (CEntity*)GetGame()->GetEntityManager()->NewBaseEntity(string("[phrgngoods]"), NULL_GUID);
								PHGoods phGoods;
								_GetBufferFromByteArray(byteArray, pos, &phGoods, sizeof(PHGoods), bufSize);
								tdrGoods->SetGuidAttr(string("guid"),  phGoods.guid);
								tdrGoods->SetGuidAttr(string("DupRgnGUID"),  dupRgn->GetGUID());
								tdrGoods->SetStringAttr(string("Name"),  phGoods.name);
								tdrGoods->SetLongAttr(string("Dir"),  phGoods.dir);
								tdrGoods->SetLongAttr(string("Width"),  phGoods.width);
								tdrGoods->SetLongAttr(string("Height"),  phGoods.height);
								tdrGoods->SetLongAttr(string("PosX"),  phGoods.posX);
								tdrGoods->SetLongAttr(string("PosY"),  phGoods.posY);
								tdrGoods->SetGUID(phGoods.guid);
								
								CEntityGroup* pRgnGoodsGroup = (CEntityGroup*)GetGame()->GetEntityManager()->FindLeafByComFlag(dupRgn, string("[phrgngoodsgroup]"));
								if(pRgnGoodsGroup)
									pRgnGoodsGroup->AddChild(tdrGoods);
							}
						}
						dupRgn->SetLongAttr(string("GSID"),  GSID);
						
						char outStr[1024];
						sprintf(outStr, "---GS:SavePH() 接收到%d个PH Goods！---", goodsNum);
						AddLogText(outStr);
					}
				
					GetGame()->GetEntityManager()->CreateSavePhRgnSession(dupRgn);
			}

			M_FREE(byteArray,bufSize);
		}
		break;
	case MSG_S2W_SERVER_LOADPERSONALHOUSE:
		{
			CGUID OwnerID;
			pMsg->GetGUID(OwnerID);
			long GSID = CMessage::GetGSIDBySocket(lSocketID);
//			GetGame()->SendLoadPersonalHouse(GSID, OwnerID);
		}
		break;
		// GS的脚本发送的消息,需要回应处理
	case MSG_S2W_SERVER_PLAYER_PERSONALHOUSEGUID:
		{
			long scriptID = pMsg->GetLong();// Script ID

			// player name
			char playerName[128];
			pMsg->GetStr(playerName, 128);

			// PH GUID
			CGUID guid;

			char szPHGuid[128]; // 返回的PH GUID 变量名
			pMsg->GetStr(szPHGuid, 128);

			// 先在内存中查找
			CPlayer* pPlayer = GetGame()->GetMapPlayer(playerName);
			if(pPlayer)// 相同名字
			{
				// 返回给GS
				CMessage msg(MSG_W2S_SERVER_PLAYER_PERSONALHOUSEGUID);
				msg.Add(scriptID);// Script ID
				msg.Add(playerName); // player name
				msg.Add(szPHGuid);// 返回的PH GUID 变量名
				msg.Add(pPlayer->GetPersonalHouseRgnGUID());// // PH GUID
				msg.SendToSocket(lSocketID);
				break;
			}
		}
		break;
	}
}
