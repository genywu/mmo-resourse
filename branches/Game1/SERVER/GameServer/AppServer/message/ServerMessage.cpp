#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "setup\playerlist.h"
#include "..\goods\cgoodsfactory.h"
#include "..\goods\csuitrulefactory.h"
#include "..\goods\IncrementShopManager.h"
#include "..\goods\enchase\ckitcardrulefactory.h"
#include "../business/CBusinessManager.h"
#include "setup\monsterlist.h"
#include "setup\GateList.h"
#include "setup\tradelist.h"
#include "setup\IncrementShopList.h"
#include "setup\skilllist.h"
#include "setup\globesetup.h"
#include "setup/logsystem.h"
#include "setup/gmlist.h"
#include "setup/emotion.h"
#include "setup\hitlevelsetup.h"
#include "setup\questsystem.h"
#include "setup/regionsetup.h"
#include "setup\contributesetup.h"
#include "setup\MailList.h"
#include "setup\ScriptAILoader.h"
#include "..\Monster.h"
#include "../serverregion.h"
#include "../Player.h"
#include "..\BuffSkill\\NewSkillFactory.h"
#include "../script/Script.h"
#include "../script/VariableList.h"
#include "..\public\dupliregionsetup.h"
#include "..\public\OccupSkillList.h"
#include "..\public\StudySkillCondition.h"
#include "../public/DeputyOccu.h"
#include "../public/StrUnifyInput.h"
#include "../public/WordsFilter.h"
#include "../public/ReliveGoodsList.h"
#include "../public/Cosmetology.h"
#include "..\nets\Servers.h"

#include "appserver\country\countryparam.h"
#include "appserver\country\countryHandler.h"

#include "..\GameServer\GameServer\GameManager.h"

#include "../RgnManager.h"
#include "AppServer/GoodsExManage.h"

#include "../Linkman/LinkmanSystem.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"

#include "Appserver\PlayerMan\PlayerMan.h"
#include "AppServer\PetCtrl.h"

#include "AppServer/AntiCheat/CConfiger.h"
#include "..\public\CreditSystem.h"
#include "../FBusiness/ShopManager.h"
#include "setup\CountTimerList.h"
#include "..\public\QuestIndexXml.h"
#include "..\public\LotteryXml.h"


extern DWORD WINAPI SaveThreadFunc(LPVOID lpParameter);


//更新返还参数数据到客户端
void UpdateReturnParam()
{
	CIncrementShopList::tagReturnParam *ptgReturnParam = CIncrementShopList::GetReturnParam();

	list<string>::iterator it = ptgReturnParam->lSGoodsName.begin();

	CMessage msg(MSG_S2C_INCREMENTSHOP_UPDATE_RATIO);
	msg.Add(ptgReturnParam->fNoramlRatio);
	msg.Add(ptgReturnParam->fSpecialRatio);
	msg.Add((char*)ptgReturnParam->strStartTime.c_str());
	msg.Add((char*)ptgReturnParam->strEndTime.c_str());
	msg.Add(long(ptgReturnParam->lSGoodsName.size()));
	for (; it!=ptgReturnParam->lSGoodsName.end(); ++it)
	{
		msg.Add((char*)(*it).c_str());
	}
	msg.SendAll();
}

// 响应其他消息
void OnServerMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	// WorldServer发送setup设置信息给GameServer
	case MSG_W2S_SERVER_SENDSETUP:
		{
			DBReadSet setReadDB;
			//const long lByteDataLen = 10485760;
			BYTE *ByteData = NULL;
			long lDelByteDataLen = 0;
			long lType = pMsg->GetLong();
			switch(lType)
			{
			case SI_WS_INFO:
				{
					DWORD dwNumber=pMsg->GetDWord();
					GetGame()->SetWSNumber(dwNumber);
				}
				break;
			case SI_GLOBALSTRING:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData =(BYTE*)(BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CStringReading::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITGLOBALSTRING));
				}
				break;
			case SI_LINED_RGN_INFO:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					GameManager::GetInstance()->GetRgnManager()->DecodeLinedRgnInfoFromByteArray(ByteData,Pos, lByteDataLen);
					AddLogText("初始化分线场景配置数据完成!");
				}
				break;
			case SI_GOODSLIST:		// GoodsList
				{
					long lByteDataLen = pMsg->GetLong();
					DBReadSet setReadDB;
					pMsg->GetDBReadSet(setReadDB);
					setReadDB.GetDwordFromByteArray();
					CGoodsFactory::Unserialize(setReadDB);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITGOODSLIST));
					CPlayer::InitFightEnumMap();
					MedalContainer::InitializeMedalSystem();
				}
				break;
			case SI_SUITSETUP:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CSuitRuleFactory::Unserialize( ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITSUITSETUP));
				}
				break;
			case SI_ENCHASESETUP:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CKitCardRuleFactory::Unserialize( ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITHOLESETUP));
				}
				break;
			case SI_TRADEAREASETUP:
				{
					pMsg->GetDBReadSet( setReadDB );
					GetInst( Business::CBusinessManager ).DecodeFromDataBlock( setReadDB );
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITCOMMERCESETUP));
				}
				break;

			case SI_MONSTERLIST:		// MonsterList
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CMonsterList::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITMONSTERLIST));
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITGOODSDROPLIST));

					// 重设所有怪物的BasePropertiy
					GetGame()->RefreashAllMonsterBaseProperty();
				}
				break;
			//GateList
			case SI_GATELIST:
				{
					long lByteDataLen=pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData=(BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos=0;
					CGateList::DecordFromByteArray(ByteData,Pos);
				}
				break;

			case SI_PLAYERLIST:		// PlayerList
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CPlayerList::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITPLAYERLIST));
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITPLAYEREXPLIST));
				}
				break;

			case SI_SKILLLIST:		// SkillList
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CNewSkillFactory::DecodeNewSkillMessage(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITSKILLLIST));
					COccupSkillList::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITSKILLSTUDYOCCULIST));
					//
					CStudySkillCondition::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITSKILLSTUDYLIST));
					
					
					
				}
				break;
			case SI_SYSMAIL:
				{
					long lByteDataLen =pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long lPos = 0;
					CMailList::DecordFromByteArray(ByteData,lPos);	
					GameManager::GetInstance()->GetMailManager()->InitSysMail();
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_SYSMAIL));					
				}
				break;
			case SI_TRADELIST:		// TradeList
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CTradeList::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITCOMMERCEGOODSLIST));
				}
				break;

			case SI_INCREMENTSHOPLIST:		// TradeList
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CIncrementShopList::DecordFromByteArray(ByteData,Pos);
					UpdateReturnParam();
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITINCGOODSLIST));
				}

				break;
			case SI_INCLIMITGOODSREC:
				{
					lDelByteDataLen = pMsg->GetLong();
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lDelByteDataLen);	
					pMsg->GetEx(ByteData,lDelByteDataLen);
					long Pos = 0;
					CIncrementShopManager::DecordFromByteArray( ByteData, Pos );
				}
				break;
			case SI_CONTRIBUTEITEM:		// TradeList
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CContributeSetup::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITCOUNTRYCONTRIBUTEGOODSDROP));
				}
				break;

			case SI_GLOBESETUP:		// GlobeSetup
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CGlobeSetup::DecordFromByteArray(ByteData,Pos);

					AREA_WIDTH = CGlobeSetup::GetSetup()->lAreaWidth;
					AREA_HEIGHT = CGlobeSetup::GetSetup()->lAreaHeight;

					GameManager::GetInstance()->SetPolicy();
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITGLOBALPARAMLIST));
				}
				break;

			case SI_LOGSYSTEM:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CLogSystem::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOGSERVERPARAMLIST));
				}
				break;

			case SI_FUNCTIONLIST:		// FunctionListFile
				{
					long lFileLength = pMsg->GetLong();
					lDelByteDataLen = lFileLength+1;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*(lFileLength+1));
					memset(ByteData, 0, lFileLength+1);
					pMsg->GetEx(ByteData, lFileLength);

					GetGame()->SetFunctionFileData((char*)ByteData, sizeof(BYTE)*(lFileLength+1));
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_SCRIPTFUNCLIST));
					return;
				}
				break;

			case SI_VARIABLELIST:		// VariableListFile
				{
					long lFileLength = pMsg->GetLong();
					lDelByteDataLen = lFileLength+1;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*(lFileLength+1));
					memset(ByteData, 0, lFileLength+1);
					pMsg->GetEx(ByteData, lFileLength);

					GetGame()->SetVariableFileData((char*)ByteData, sizeof(BYTE)*(lFileLength+1));
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_SCRIPTVARLIST));
					return;
				}
				break;

			case SI_GENERALVARIABLELIST:		// GeneralVariableListFile
				{
					if(!GetGame()->GetIsGenVarInit())
					{
						GetGame()->SetIsGenVarInit(true);
						long lByteDataLen = pMsg->GetLong();
						lDelByteDataLen = lByteDataLen;
						ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
						pMsg->GetEx(ByteData,lByteDataLen);
						long Pos = 0;
						GetGame()->SetGeneralVariableFileData(ByteData,Pos);
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_GLOBALSCRIPTVARLIST));
					}
				}
				break;


			case SI_SCRIPTFILE:		// ScriptFile
				{		
					char strFileName[MAX_PATH];
					pMsg->GetStr(strFileName,MAX_PATH);
					long lFileLength = pMsg->GetLong();
					lDelByteDataLen = lFileLength+1;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*(lFileLength+1));
					memset(ByteData, 0, lFileLength+1);
					pMsg->GetEx(ByteData, lFileLength);					
					GetGame()->SetScriptFileData(strFileName, (char*)ByteData, sizeof(BYTE)*(lFileLength+1));	
				//	AddLogText("%s...ok!", strFileName);
				}
				break;
			case SI_QUEST:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;					
					CQuestSystem::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_GETQUESTLIST));
				}
				break;
			case SI_DEPUTYOCCU:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long pos = 0;
					CDeputyOccuSystem::DecordFromByteArray(ByteData,pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_GETDOCCULIST));

				}
				break;
			case SI_PHDUPRGN:
				{
					// 
					CGUID dupRgnID;
					pMsg->GetGUID(dupRgnID);
			
					long GSID = pMsg->GetLong();
				
					char name[128];
					pMsg->GetStr(name, 128);
					
					CGUID ownerID;
					pMsg->GetGUID(ownerID);
					long tRgnID = pMsg->GetLong();
					long lResourceID = pMsg->GetLong();
					
					// 创建PHDupRgn
					CRgnManager* rgnManger = GameManager::GetInstance()->GetRgnManager();
					CServerRegion* wRgn = NULL;
					CRgnManager::RgnByIDMapItr gsRgnItr = rgnManger->GetPersonalHouseDupRgnMap().find(dupRgnID);
					if(gsRgnItr == rgnManger->GetPersonalHouseDupRgnMap().end())
					{
						wRgn = rgnManger->AddCreatedDupRgn(dupRgnID, RGN_PERSONAL_HOUSE);
						rgnManger->GetPersonalHouseDupRgnMap()[dupRgnID] = wRgn;
					}
					else
						wRgn = gsRgnItr->second;

					if(wRgn)
					{	
						wRgn->SetID(tRgnID);			
						wRgn->SetExID(dupRgnID);
						wRgn->SetOwnerGUID(ownerID);
						wRgn->SetRgnType((long)RGN_PERSONAL_HOUSE);
						wRgn->SetName(name);
						wRgn->SetResourceID(lResourceID);

						char outStr[512];
						char szRGUID[128];
						wRgn->GetExID().tostring(szRGUID);
						_snprintf(outStr, 512, CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITPHRGNOK),
							szRGUID, wRgn->GetID(), wRgn->GetName());
						AddLogText(outStr);
						long dataBufSize = pMsg->GetLong();
						if(dataBufSize)
						{
							BYTE* rgnData = (BYTE*)M_ALLOC(sizeof(BYTE)*dataBufSize);
							pMsg->GetEx(rgnData, dataBufSize);
							long pos = 0;
							wRgn->DecordFromByteArray(rgnData, pos);
							M_FREE(rgnData, dataBufSize);
						}
					}
				
					long goodsNum = pMsg->GetLong();
					CRgnManager::PHGoodsMap& goodsMap = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap();
					CRgnManager::PHGoodsMapItr goodsItr = goodsMap.find(dupRgnID);
					if(goodsItr == goodsMap.end())
					{
						vector<CRgnManager::PHGoods> goodsVec;
						for(int t=0; t<goodsNum; t++)
						{
							CRgnManager::PHGoods goods;
							pMsg->GetEx(&goods, sizeof(CRgnManager::PHGoods));
							goodsVec.push_back(goods);
						}
						goodsMap[dupRgnID] = goodsVec;
					}
					else
					{
						goodsItr->second.clear();
						for(int t=0; t<goodsNum; t++)
						{
							CRgnManager::PHGoods goods;
							pMsg->GetEx(&goods, sizeof(CRgnManager::PHGoods));
							goodsItr->second.push_back(goods);
						}
					}

					if(wRgn)
					{	
						CPlayer* owner = GetGame()->FindPlayer(ownerID);
						if(owner)
							owner->SetPersonalHouseRgnGUID(wRgn->GetExID());

						CRgnManager::PHGoodsMap& tmpGoodsMap = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap();
						CRgnManager::PHGoodsMapItr tmpGoodsItr = tmpGoodsMap.find(wRgn->GetExID());
						if(tmpGoodsItr != tmpGoodsMap.end())
						{
							for(int t=0; t<tmpGoodsItr->second.size(); t++)
							{
								CRgnManager::PHGoods& goods = (tmpGoodsItr->second)[t];
								
								// 创建物品
								LONG lNewX = 0; 
								LONG lNewY = 0;
								wRgn -> GetRandomPosInRange( lNewX, lNewY, 
									goods.width, goods.height, 
									goods.width - goods.posX,
									goods.height - goods.posY );

								CMonster* pLeader = wRgn -> AddMonster( goods.name, lNewX, lNewY, -1 );
								if( pLeader == NULL ) 
								{
									break;
								}
							}
						}
					}
				}
				break;
			case SI_GMLIST:		// GMList
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CGMList::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_GETGMLISTOK));
				}
				break;

			case SI_REGION:		// RegionInfo
				{
					eRgnType RegionType= (eRgnType)pMsg->GetLong();
					long rgnTemplateID = pMsg->GetLong();
					CGUID rgnID;
					pMsg->GetGUID(rgnID);
					CServerRegion* pRegion = NULL;
				
					if(RegionType == RGN_NORMAL)
						pRegion = GameManager::GetInstance()->GetRgnManager()->AddNormalRgn(rgnID, rgnTemplateID);//new CServerRegion(rgnID);

					if (pRegion==NULL) break;

					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);	

					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					pRegion->DecordFromByteArray(ByteData,Pos);
					pRegion->OnChangeWeather();
					pRegion->RegisterWeatherTimer();
					
					GetGame()->UpdateSaveRegion(pRegion);

					char str[256];
					_snprintf(str, 256, "%s (%d)", pRegion->GetName(), pRegion->GetID());
					AddRegionList(str);
						
					extern long g_lTotalMonster, g_lTotalNpc;
					static long lM=0, lN=0;

					switch(RegionType)
					{
					case RGN_NORMAL:
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_STARTNORMALRGNOK), pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN);
						break;
					case RGN_PERSONAL:
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_STARTPRGNOK), pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN);
						break;
					case RGN_PERSONAL_HOUSE:
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_STARTPHRGNOK), pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN);
						break;
					case RGN_TEAM:
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_STARTTRGNOK), pRegion->GetID(), pRegion->GetName(), g_lTotalMonster-lM, g_lTotalNpc-lN);
						break;
					}
					
					lM = g_lTotalMonster;
					lN = g_lTotalNpc;
				}
				break;
			case SI_DUPRGNDATA:
				{
					GameManager::GetInstance()->GetRgnManager()->DelAllDupRgnData();
					long lRgnNum = pMsg->GetLong();// 副本模板个数
					for(int i=0; i<lRgnNum; i++)
					{
						long lRgnTemplateId = pMsg->GetLong(); // 副本模板ID
						long lBufSize = pMsg->GetLong(); // bufsize
						if(lBufSize > 0)
						{
							BYTE* pBuf = (BYTE*)M_ALLOC(sizeof(BYTE)*lBufSize);
							pMsg->GetEx(pBuf, lBufSize);
							// 将该内存转移到map管理
							if(!GameManager::GetInstance()->GetRgnManager()->AddDupRgnData(lRgnTemplateId, pBuf, lBufSize))
								M_FREE(pBuf, sizeof(BYTE)*lBufSize);
							pBuf = NULL;
							lBufSize = 0;
						}
					}
					AddLogText("添加副本模板缓冲数据 %d 个 成功！", lRgnNum);
				}
				break;
			case SI_PROXY_REGION:		
				{	
					
				}
				break;
			case SI_REGIONRELOAD:
				{
					CGUID lRegionID;
					pMsg->GetGUID(lRegionID);
					CServerRegion* pServerRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(lRegionID);
					if(pServerRegion)
					{
						long lByteDataLen = pMsg->GetLong();
						lDelByteDataLen = lByteDataLen;
						ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
						pMsg->GetEx(ByteData,lByteDataLen);
						long Pos = 0;
						pServerRegion->DecordSetupFromByteArray(ByteData,Pos);
						pServerRegion->DecordFromByteArray(ByteData,Pos);
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_RELOADRGNSETUPOK),pServerRegion->GetID(),
							pServerRegion->GetName());
					}
				}
				break;
			case SI_REGIONSETUP:
				{
					CGUID lRegionID;
					pMsg->GetGUID(lRegionID);
					CServerRegion*pServerRegion=GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(lRegionID);
					if(pServerRegion)
					{
						long lByteDataLen = pMsg->GetLong();
						lDelByteDataLen = lByteDataLen;
						ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
						pMsg->GetEx(ByteData,lByteDataLen);
						long Pos = 0;
						pServerRegion->DecordSetupFromByteArray(ByteData,Pos);
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_RELOADRGNSETUPEXOK),pServerRegion->GetID(),
									pServerRegion->GetName());
					}
				}
				break;

			case SI_REGIONLEVELSETUP:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CRegionSetup::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITRGNLEVELLISTOK));
				}
				break;

			case SI_GAMESERVERINDEX:
				{
					GetGame()->SetIDIndex(pMsg->GetByte());
				}
				break;
			case SI_HITLEVEL:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CHitLevelSetup::DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITHITLEVELLISTOK));
				}
				break;
			case SI_END:	// 正式启动，初始化服务器网络
				{
					GetGame()->SetSaveDeltaTime(pMsg->GetLong());

					//// 建立服务器端网络
					if( !GetGame()->StartAccept() )
					{
						extern HWND g_hWnd;
						MessageBox(g_hWnd, "Can't init NetServer!", "Message", MB_OK);
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITSERVERNETFAILED));
					}
					extern long g_lTotalMonster, g_lTotalNpc;
					char str[256];
					_snprintf(str, 256, CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_GAMEWORLDMONSTERNUM), g_lTotalMonster, g_lTotalNpc);
					AddLogText(str);
				}
				break;
			case SI_EMOTION:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					CEmotion::Unserialize( ByteData,Pos );
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_INITFACELIST));
				}
				break;
			case SI_COUNTRYPARAM:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					GetCountryParam()->DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOADCOUNTRYPARAMOK));
				}
				break;
			case SI_COUNTRY:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					GetCountryHandler()->DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOADCOUNTRYINFOOK));
				}
				break;
				//副本配置信息
			case SI_DUPLIREGIONSETUP:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					GetGame()->GetDupliRegionSetup()->DecordFromByteArray(ByteData,Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOADDUPRGNOK));
				}
				break;
			case SI_GOODSEX:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					if(GoodsExManage::GetInstance().InitSetup(ByteData, Pos))
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOADGOODSEXSETUPOK));
					else
						AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOADGOODSEXSETUPFAILED));
				}
				break;
			case SI_DEPOT: 
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(PlayerDepot::GetDepotSetup(), sizeof(tagDepotSetup));

					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOADDEPOTSETUPOK));
				}
				break;
			case SI_LINKMAN:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					LinkmanSystem::GetInstance().InitSetup(ByteData, Pos);
					AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_LOADRECVFRIENDSYSSETUPOK));
				}
				break;
			case SI_SPRITE:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long Pos = 0;
					SpriteSystem::SetupGetFormByteArray(ByteData, Pos);
				}
				break;
			case SI_SensitiveWord:
				{
					long lByteDataLen = pMsg->GetLong();
					vector<BYTE> vData(lByteDataLen + 1);
					pMsg->GetEx(&vData[0], lByteDataLen);
					LONG lPos = 0;
					CWordsFilter::GetInstance()->Decode(&vData[0], lPos);
				}
				break;
			case SI_SCRIPTAI:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx( ByteData, lByteDataLen );
					long lPos = 0;
					CScriptAILoader::Instance().DecodeFromByteArray( ByteData, lPos );
				}
				break;
			case SI_PLAYER_MAN:				// Fox@20081120 玩家管理
				{
					pMsg->GetDBReadSet(setReadDB);
					GetInst(CPlayerMan).DecodeFromDataBlock(setReadDB);
				}
				break;
			case SI_PET_LIST:				// Fox@20081119 宠物
				{
					pMsg->GetDBReadSet(setReadDB);
					GetInst(CPetCtrl).DecodeFromDataBlock(setReadDB);
				}
				break;
			case SI_GOODSTAILOR:
				{
					pMsg->GetDBReadSet(setReadDB);
					GoodsTailorSetup::GetInstance().Decode(setReadDB);
				}
				break;
			case SI_ANTICHEAT:               // BaiYun@2009525 反外挂
				{
					pMsg->GetDBReadSet(setReadDB);
					GetInst( AntiCheat::CConfiger ).DecodeFromDataBlock(setReadDB);
				}
				break;
			case SI_RELIVEGOODS:
				{
					long lByteDataLen = pMsg->GetLong();
					lDelByteDataLen = lByteDataLen;
					ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
					pMsg->GetEx(ByteData,lByteDataLen);
					long lPos = 0;
					CReLiveGoodsList::DecordFromByteArray(&ByteData[0], lPos);
				}
				break;
	       case SI_CREDITSYSTEM:
				{
					pMsg->GetDBReadSet(setReadDB);
					GetInst(CCreditSystem).DecodeToByteArray(setReadDB);
				}
				break;
		   case SI_FBUSINESS:
			   {
				   pMsg->GetDBReadSet( setReadDB );
				   GetInst( FBusiness::CShopManager ).Decode( setReadDB );
			   }
			   break;
		   case SI_COUNTTIMERLIST:
			   {
				   long lByteDataLen = pMsg->GetLong();
				   lDelByteDataLen = lByteDataLen;
				   ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
				   pMsg->GetEx(ByteData,lByteDataLen);
				   long lPos = 0;
				   CCountTimerList::Unserialize(&ByteData[0], lPos);
				   AddLogText("初始化定时器配置成功！");
			   }
			   break;
		   case SI_COSMESYSTEM:
			   {
				   long lByteDataLen = pMsg->GetLong();
				   lDelByteDataLen = lByteDataLen;
				   ByteData = (BYTE*)M_ALLOC(sizeof(BYTE)*lByteDataLen);
				   pMsg->GetEx(ByteData,lByteDataLen);
				   long Pos = 0;
				   CCosmetology::DecordFromByteArray(&ByteData[0],Pos);
				   AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_COSMETOLOTY));
				   

			   }
			   break;
		    case SI_QUESTINDEX:
				{
					pMsg->GetDBReadSet(setReadDB);
					GetInst(CQuestIndexXml).DecodeToByteArray(setReadDB);
				}
				break;
			case SI_Lottery:
				{
					pMsg->GetDBReadSet(setReadDB);
					GetInst(CLotteryXml).DecodeToByteArray(setReadDB);
				}
				break;
			}
			M_FREE(ByteData, lDelByteDataLen);
		}
		break;

	// WorldServer关闭
	case MSG_S2S_SERVER_WORLDSERVERCLOSE:
		{
			AddLogText("============WorldServer closed==========!!!");
			GetGame()->CreateConnectWorldThread();
		}
		break;
		// [AHC 080809 DBSMSG]
	case MSG_W2S_SERVER_QUEST_RECONNECTION:
		{
			AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_RECONNECTINFOFROMWSOK));
			
		}
		break;
		// [AHC 080809 DBSMSG]
	case MSG_DB2S_SERVER_QUEST_RECONNECTION:
		{
			// 
			long msgTimerID = pMsg->GetLong();
			AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_RECONNECTINFOFROMDBSOK));
		}
		break;
	// 返回切换服务器
	case MSG_W2S_SERVER_CHANGEREGION:
		{
			long lSignCode = pMsg->GetLong();

			char bSuccess = pMsg->GetChar();
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);

			// 成功
			if(bSuccess!=0)
			{
				// 成功
				char strIP[256];
				pMsg->GetStr(strIP,256);
				DWORD dwPort = pMsg->GetDWord();

				CMessage msg(MSG_S2C_RGN_CHANGESERVER);
				// 添加验证代码
				msg.Add(lSignCode);
				msg.Add(strIP);
				msg.Add(dwPort);
				msg.SendToPlayer(PlayerID);
			}
			else
				GetGame()->KickPlayer(PlayerID);
		}
		break;
	// 发送信息
	case MSG_W2S_SERVER_ADDINFO:
		{
			CGUID PlayerID;
			pMsg -> GetGUID(PlayerID);
			if( PlayerID != NULL_GUID )
			/**## 发送给指定的玩家 */
			{
				char strMsg[1024];
				char strSender[24];
				char strSend[1024];
				DWORD dwColor = 0;
				DWORD dwBkColor = 0;
				LONG lPos = 0;

				pMsg -> GetStr( strMsg,1024 );
				dwColor		= pMsg -> GetDWord();
				dwBkColor	= pMsg->GetDWord();
				lPos		= pMsg->GetLong();

				pMsg -> GetStr( strSender,24 );

				if(strlen(strSender) > 0)
				{
					_snprintf( strSend, 1024, "%s：%s", strSender, strMsg );
				}
				else
				{
					_snprintf( strSend, 1024, "%s",strMsg );
				}
				GetGame()->SendNotifyMessage(strSend,PlayerID,dwColor,dwBkColor, (eNOTIFY_POS)lPos);
			}
			else 
			/**## 返回给发送者无法发送的消息 */
			/**## 失败：LONG Reciver(=0), char* Reciver, LONG Sender */
			{
				char strPlayer[24];
				

				pMsg -> GetStr( strPlayer,24 );
				CGUID SenderID;
				pMsg -> GetGUID(SenderID);

				char strSend[256];
				_snprintf( strSend, 256, "消息无法发送给%s。", strPlayer );
				GetGame()->SendNotifyMessage(strSend,SenderID,0xffffffff);
			}
		}
		break;

	// 改变全局脚本变量的值
	case MSG_W2S_SERVER_CHANGE_SCRIPT_VARIABLE:
		{
			char strVarName[256];
			pMsg->GetStr(strVarName, 256);
			BYTE nFlag = pMsg->GetByte();
			if( eVarNumber == nFlag ) // 0 数值
			{
				long pos = pMsg->GetLong();
				LONG64 llRet = pMsg->GetLONG64();
				double nValue = *(double*)&llRet;
				if( CScript::GetGeneralVariableList() )
					CScript::GetGeneralVariableList()->SetVarValue(strVarName, pos, nValue);
			}
			else if( eVarString == nFlag ) // 字符串
			{
				char szValue[1024];
				pMsg->GetStr(szValue, 1024);
				if( CScript::GetGeneralVariableList() )
					CScript::GetGeneralVariableList()->SetVarValue(strVarName, szValue);
			}
			else if( eVarGuid == nFlag ) // guid
			{
				CGUID guid;
				pMsg->GetGUID(guid);
				if( CScript::GetGeneralVariableList() )
					CScript::GetGeneralVariableList()->SetGuidValue(strVarName, &guid);
			}
		}
		break;

	//##同步玩家的杀人数
	case MSG_W2S_SERVER_SYNC_MURDERER_COUNT:
		{
			LONG lSuffererType	= pMsg -> GetLong();
			CGUID SuffererID; pMsg->GetGUID(SuffererID);
			LONG lMurdererType	= pMsg -> GetLong();
			CGUID MurdererID; pMsg->GetGUID(MurdererID);

			CPlayer* pPlayer	= GetGame() -> FindPlayer( MurdererID );
			if( pPlayer ) 
			{
				long lNum = pPlayer -> GetPkValue() + CGlobeSetup::GetSetup()->dwPkCountPerKill;
				if (lNum > 65535)
					lNum = 65535;
				pPlayer->SetAttribute("PkValue",lNum);// etAttribute();
				pPlayer->SetAttribute("PkCount",pPlayer -> GetPkCount() + 1);
				pPlayer -> OnUpdateMurdererSign();
				pPlayer->UpdateAttribute();
			}
			else 
			{
				//##如果找不到怎么办？继续交给WorldServer处理？
				pMsg -> SetType( MSG_S2W_SERVER_REPORT_MURDERER );
				pMsg -> Send();
			}
		}
		break;
	// 返回ping信息
	case MSG_W2S_SERVER_QUEST_PING:
		{		
				DWORD flag = pMsg->GetDWord();
				CMessage msg(MSG_S2W_SERVER_ANSWER_PING);
				msg.Add(GetGame()->GetPlayerCount());
				msg.Add(flag);
				msg.Send();			
		}
		// 返回ping信息
	case MSG_DB2S_SERVER_QUEST_PING:
		{		
				CMessage msg(MSG_S2DB_SERVER_ANSWER_PING);
				msg.Add(GetGame()->GetPlayerCount());
				msg.Send();
		}
		break;
	case MSG_W2S_SERVER_CREATEMONSTER:
		{
			CHAR szMonsterName[256];
			CHAR szScriptName[256];
			ZeroMemory( szMonsterName, 256 );
			ZeroMemory( szScriptName, 256 );

				CGUID lRegionID;
				pMsg->GetGUID(lRegionID);
				pMsg -> GetStr( szMonsterName, 256 );
				long nCounts 	= pMsg -> GetLong();
				long nLeft		= pMsg -> GetLong();
				long nTop		= pMsg -> GetLong();
				long nRight		= pMsg -> GetLong();
				long nBottom	= pMsg -> GetLong();
				BYTE bHasScript	= pMsg -> GetByte();
				if( bHasScript )
				{
					pMsg -> GetStr( szScriptName, 256 );
				}

				CServerRegion* pRegion	= GetGame() -> FindRegion( lRegionID );
				if( pRegion )
				{
					LONG lNewX = 0; 
					LONG lNewY = 0;
					pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );

					for (int i=0; i < nCounts; i++)
					{
						pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
						CMonster* pMonster = pRegion -> AddMonster( szMonsterName, lNewX, lNewY, -1 );
						if (pMonster)
						{
							pMonster->SetScriptFile(szScriptName);
						}
					}
				}

		}
		break;
	case MSG_W2S_SERVER_CREATEPHGOODS:
		{
			CHAR szMonsterName[256];
			CHAR szScriptName[256];
			ZeroMemory( szMonsterName, 256 );
			ZeroMemory( szScriptName, 256 );

			BYTE bType		= pMsg->GetByte();
			if (bType == 0)	// monster
			{
				CGUID guid;
				pMsg->GetGUID(guid);
				CGUID lRegionID;
				pMsg->GetGUID(lRegionID);
				pMsg -> GetStr( szMonsterName, 256 );
				long nCounts 	= pMsg -> GetLong();
				long nLeft		= pMsg -> GetLong();
				long nTop		= pMsg -> GetLong();
				long nRight		= pMsg -> GetLong();
				long nBottom	= pMsg -> GetLong();
				long tempID		= pMsg -> GetLong();
				long bHasScript	= pMsg -> GetByte();
				if( bHasScript )
				{
					pMsg -> GetStr( szScriptName, 256 );
				}

				CServerRegion* pRegion	= GetGame() -> FindRegion( lRegionID );
				if( pRegion )
				{
					LONG lNewX = 0; 
					LONG lNewY = 0;
					pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );

					for (int i=0; i < nCounts; i++)
					{
						pRegion -> GetRandomPosInRange( lNewX, lNewY, nLeft, nTop, nRight - nLeft, nBottom - nTop );
						CMonster* pMonster = pRegion -> AddMonster( szMonsterName, lNewX, lNewY, -1 );
						if (pMonster)
						{
							pMonster->SetScriptFile(szScriptName);

							// 加入PHGoods容器
							CRgnManager::PHGoods tGoods;
							CRgnManager::PHGoodsMapItr goodsItr = GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap().find(pRegion->GetExID());

							CRgnManager::vecPHGoods* vecPHGoodsPtr = NULL;

							if(goodsItr == GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap().end())
							{
								GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap()[pRegion->GetExID()] = CRgnManager::vecPHGoods();
								vecPHGoodsPtr = &GameManager::GetInstance()->GetRgnManager()->GetPHGoodsMap()[pRegion->GetExID()];
							}

							if(vecPHGoodsPtr != NULL)
							{
								CRgnManager::vecPHGoodsItr vecGoodsItr = vecPHGoodsPtr->begin();
								for(; vecGoodsItr != vecPHGoodsPtr->end(); vecGoodsItr++)
								{
									if(strcmp((*vecGoodsItr).name, szMonsterName) == 0 &&
										(*vecGoodsItr).dir == nCounts &&
										(*vecGoodsItr).width == nLeft &&
										(*vecGoodsItr).height == nTop &&
										(*vecGoodsItr).posX == nRight &&
										(*vecGoodsItr).posY == nBottom) // 是同一个GOODS
										break;
								}

								if(vecGoodsItr == vecPHGoodsPtr->end()) // 未找到
								{
									CRgnManager::PHGoods tGoods;
									CGUID::CreateGUID(tGoods.guid);
									memcpy(tGoods.name, szMonsterName, sizeof(tGoods.name));
									tGoods.dir = nCounts;
									tGoods.width = nTop;
									tGoods.height = nLeft;
									tGoods.posX = nRight;
									tGoods.posY = nBottom;
									vecPHGoodsPtr->push_back(tGoods);
								}
							}

							char szGUID[128];
							pMonster->GetExID().tostring(szGUID);
							char szRgnID[128];
							lRegionID.tostring(szRgnID);
							AddLogText(CStringReading::LoadString(IDS_GS_SERVERMESSAGE, STR_GS_SERVERMESSAGE_CREATEPHRGNOK), szRgnID, szMonsterName, szGUID);
						}
					}
				}
			}
		}
		break;
		// 挂起的脚本函数数据库返回消息
	case MSG_W2S_SERVER_PLAYER_PERSONALHOUSEGUID:
		{
			CGUID scriptID;
			pMsg->GetGUID(scriptID); //script id
			char szPlayerName[128];
			pMsg->GetStr(szPlayerName, 128); // player name
			char szGuidName[128];
			pMsg->GetStr(szGuidName, 128); // ret guid name string
			CGUID retGUID;
			pMsg->GetGUID(retGUID);

			//wq:2008-7-9,需要修改
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddGuid(szGuidName, retGUID);
			}
			ContinueRunScript(scriptID, stackVarList);
		}
		break;
	case MSG_W2S_SERVER_BUSINESS_UPDATE:
		{
			//更新贸易点的商业国家归属
			int i=0;
			
		}
		break;
	}
}
