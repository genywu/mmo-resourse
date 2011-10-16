#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\area.h"
#include "..\player.h"
#include "..\goods\cgoods.h"
#include "..\goods\CGoodsFactory.h"
#include "..\goods\IncrementShopManager.h"
#include "..\serverregion.h"
#include "../script/Script.h"
#include "../setup/GlobeSetup.h"
//#include "../Skills\SkillFactory.h"
#include "../setup/GMList.h"
#include "../setup/LogSystem.h"
#include "../setup/counttimerlist.h"
#include "../Listener/CSeekGoodsListener.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../OrganizingSystem/GameFaction.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../PKSys.h"
#include "../script/VariableList.h"
#include "..\PlayerMan\PlayerMan.h"
#include "..\..\gameserver\GameManager.h"
#include "..\..\AppServer\RgnManager.h"
#include "../public/WordsFilter.h"
#include "../public/Crc32Static.h"
#include "../public/Cosmetology.h"
#include "../AntiCheat/CPlayerManager.h"
#include "..\SMClient.h"

extern void CancelIncrementTrading( CPlayer* pPlayer );
extern void ProcessIncrementTrading( CPlayer* pPlayer, DWORD dwGoodsIndex, DWORD dwBuyNum, DWORD dwTradeMode, DWORD dwFenHongFlag );

//! 聊天中可以包含的最大物品个数
const LONG MAX_GOODS_NUM_IN_CHAT = 200;

struct tagChatsGoodsInfo_tmp
{
	tagChatsGoodsInfo	ChatsGoodsInfo;
	char				szGoodsName[256];
	vector<BYTE>		vGoodsData;
};


// 响应其他消息
void OnOtherMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_W2S_OTHER_WARNING:
		{
			pMsg->SetType(MSG_S2C_OTHER_WARNING);
			pMsg->SendAll();
		}
		break;
	case MSG_W2S_OTHER_LIMIT_GOODS_RESPONSE:
		{
			CPlayer* pPlayer = NULL;
			CGUID guid;
			pMsg->GetGUID(guid);
			pPlayer = GetGame()->FindPlayer(guid);
			DWORD dwIncrementBuyRet = pMsg->GetDWord();
			if ( dwIncrementBuyRet == eICR_WORLD_NUM_LIMIT )
			{
				//超过购买额度
				CancelIncrementTrading( pPlayer );
				char* strRes=CStringReading::LoadString(22,33);
				if(strRes)
					pPlayer->SendNotifyMessage( strRes );
				return;
			}
			else if ( dwIncrementBuyRet == eICR_BUY_SUCCEED )
			{
				//可购买
				DWORD dwGoodsId=pMsg->GetDWord();
				DWORD dwGoodsNum=pMsg->GetDWord();
				DWORD dwTradeMode = pMsg->GetDWord();
				DWORD dwAlreadyBuyNum = pMsg->GetDWord();

				//处理购买
				ProcessIncrementTrading( pPlayer, dwGoodsId, dwGoodsNum, dwTradeMode, 0 );
				//即时刷新消息
				long lWorldAllowBuyNum = CIncrementShopList::GetWorldBuyLimitNum( dwTradeMode, dwGoodsId );
				assert( lWorldAllowBuyNum != -1 );

				CMessage msg(MSG_S2C_INCREMENTSHOP_UPDATA);
				msg.Add( (BYTE)1 );																										//刷新标示（0:个人限购1:限量购买）
				msg.Add( (BYTE)dwTradeMode );																				//商店类型
				CIncrementShopList::MAP_INCGOODS* pIncGoodsList = CIncrementShopList::GetIncGoodsList();
				CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( dwGoodsId );
				CIncrementShopList::ITR_INCGOODS itIncGoodsList = pIncGoodsList->find( dwTradeMode );
				CIncrementShopList::MAP_INCSHOP mapIncGoodsList = itIncGoodsList->second;
				CIncrementShopList::ITR_INCSHOP itInc = mapIncGoodsList.find( dwGoodsId );
				if ( pProperties && itInc != mapIncGoodsList.end() )
				{
					msg.Add( itInc->second.dwGoodsType );																	//物品分类
					assert( dwGoodsId == itInc->second.dwGoodsIndex );
					msg.Add( itInc->second.dwGoodsIndex );																	//物编ID
				}
				msg.Add( DWORD(lWorldAllowBuyNum - dwAlreadyBuyNum - dwGoodsNum) );		//剩余可购买数量
				msg.SendAll();
			}
		}
		break;
	case MSG_W2S_OTHER_LIMIT_GOODS_UPDATA:
		{
			DWORD dwGoodsId=pMsg->GetDWord();
			DWORD dwGoodsNum=pMsg->GetDWord();
			CIncrementShopManager::AddLimitGoodsRecord( dwGoodsId, dwGoodsNum );
		}
		break;
	case MSG_W2S_OTHER_NEW_DAY:
		{
			DWORD dwData = pMsg->GetDWord();
			GetGame()->OnNewDayStart(dwData);
		}
		break;
	case MSG_W2S_OTHER_DEL_SPOUSE:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
			if (NULL != pPlayer)
				pPlayer->SetSpouse(NULL);
		}
		break;
	case MSG_C2S_OTHER_UPDATENAMEANSWER:
		{
			// 检查玩家名字是否有‘#’符号
			CGUID scriptid;
			pMsg->GetGUID(scriptid);
			// 继续挂起的脚本
			listScript::iterator  itr = g_Scripts.find(scriptid);
			if(itr == g_Scripts.end())
			{
				char szGuid[128];
				scriptid.tostring(szGuid);
				AddLogText("MSG_C2S_OTHER_UPDATENAMEANSWER 未找到挂起的脚本[%s].", szGuid);
				return;
			}
			ContinueRunScript(scriptid, NULL);

			CGUID guid;
			pMsg->GetGUID(guid);
			char szName[64] = {0};
			pMsg->GetStr(szName, sizeof(szName));
			if(szName[0] == '\0')
			{
				// 回应客户端消息
				CMessage retmsg(MSG_S2C_OTHER_CHANGENAME);
				retmsg.Add(guid);
				retmsg.Add(szName);
				retmsg.Add((BYTE)0); // 名字不合法
				retmsg.SendToPlayer(guid);
				return;
			}

			std::string strName = szName;
			// 检查名字是否合法
			if(!CWordsFilter::GetInstance()->Check(strName))
			{
				// 回应客户端消息
				CMessage retmsg(MSG_S2C_OTHER_CHANGENAME);
				retmsg.Add(guid);
				retmsg.Add(szName);
				retmsg.Add((BYTE)-2); // 名字不合法
				retmsg.SendToPlayer(guid);
				return;
			}

			// 检查名字中是否有空格
			std::string::size_type loc = strName.find(' ');
			if( loc != std::string::npos)
			{
				// 回应客户端消息
				CMessage retmsg(MSG_S2C_OTHER_CHANGENAME);
				retmsg.Add(guid);
				retmsg.Add(szName);
				retmsg.Add((BYTE)-2); // 名字不合法
				retmsg.SendToPlayer(guid);
				return;
			}

			CMessage msg(MSG_S2W_OTHER_CHANGENAME);
			msg.Add(szName);
			msg.Add(guid);
			msg.Send();
		}
		break;
	case MSG_W2S_OTHER_CHANGENAME:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			
			char szName[32] = {0};
			pMsg->GetStr(szName, sizeof(szName));

			BYTE retFlag = pMsg->GetByte();

			

			// 发送消息到客户端
			CMessage retmsg(MSG_S2C_OTHER_CHANGENAME);
			retmsg.Add(guid);
			retmsg.Add(szName);
			retmsg.Add((BYTE)retFlag); // 名字不合法

			CPlayer* pPlayer = NULL;
			if(retFlag == 1)
			{
				pPlayer = GetGame()->FindPlayer(guid);
				if(pPlayer)
					pPlayer->SetName(szName);
				retmsg.SendToAround(pPlayer); // 成功后通知周围的人群
				// Fox@20081110 刷新角色名称
				GetInst(CPlayerMan).RefreshElem(PET_NAME, pPlayer);
			}
			else
				retmsg.SendToPlayer(guid); // 成功后通知周围的人群

		}
		break;
	case MSG_W2S_OTHER_GETLOG_RESPONSE:
		{
			CGUID PlayerID; pMsg->GetGUID(PlayerID);
			pMsg->SetType(MSG_S2C_INCREMENTSHOP_GETLOG_RESPONSE);
			pMsg->SendToPlayer(PlayerID);
		}
		break;
	case MSG_C2S_OTHER_FACEACTION:
		{
			//玩家发送的表情动作
				DWORD dwActID = pMsg->GetDWord();
				long lDestType = pMsg->GetLong(); 
				CGUID DestGuid ;
				pMsg->GetGUID(DestGuid);
				CMoveShape *pUser = pMsg->GetPlayer();
				if(pUser && pUser->GetState()==CShape::STATE_PEACE)
				{
					CMessage msg(MSG_S2C_OTHER_EXPATC);
					msg.Add(dwActID);
					msg.Add(pUser->GetType());
					msg.Add(pUser->GetExID());
					msg.Add(lDestType);
					msg.Add(DestGuid);
					msg.SendToAround(pUser);			
				}				
		}
		break;
	case MSG_C2S_OTHER_TALK:	// 对话消息
		{
			CPlayer *pMsgPlayer = pMsg->GetPlayer();
			if (pMsgPlayer == NULL || pMsg->GetRegion() == NULL)
			{
				//assert(0);
				return;
			}

			if( pMsgPlayer -> IsInSilence() )
			{
				pMsgPlayer -> SendNotifyMessage( "你已经被禁言！" );
				break;
			}
			BYTE bTalkType= pMsg->GetByte();

		//	//因物品连接功能,先取聊天字符串分析
			char strChatText[1024] = {0};
			char strChatObjName[256] = {0};
			long lType = pMsg->GetLong();	//chat type


			// GM命令
			if (lType == GM_CMD)
			{
				//跳过2个参数
				pMsg->GetLong();
				//玩家GUID
				CGUID playerGUID;
				pMsg->GetGUID(playerGUID);
				//
				pMsg->GetLong();

				//取命令行字符
				char CmdLine[512];
				pMsg->GetStr(CmdLine,512);

				extern bool ParseGMCommand(CMessage* msg, char *);
				bool bSuccess = ParseGMCommand(pMsg, CmdLine);

				// GM指令日志纪录
				long lGMLevel = 0;
				tagGMInfo *pCurrentGM;
				pCurrentGM=CGMList::GetInfoByName(pMsgPlayer->GetName());
				if (pCurrentGM)
				{
					lGMLevel = pCurrentGM->lLevel;
				}

				if (lGMLevel == GM_GOD)
					return;

				if (bSuccess)
				{
					char strOut[1024];
					_snprintf(strOut, 1024, "'%s' (lvl:%d map:%d) => %s [%s]", 
						pMsg->GetPlayer()->GetName(),
						lGMLevel,
						pMsg->GetRegion()->GetID(), 
						CmdLine,
						(bSuccess)?"成功":"失败");

					PutStringToFile("GMLog",strOut);

					GetGameLogInterface()->logT431_GM_log(pMsgPlayer,CmdLine);
				}
				return;
			}

			long lSenderType=pMsg->GetLong();	//sender type
			CGUID SenderID;
			pMsg->GetGUID(SenderID);	//sender id
			

			//!--------------------------------------------------------------------------------------
			//! 聊天中的物品信息提取
			LONG lGoodsNum = pMsg->GetLong();
			if(MAX_GOODS_NUM_IN_CHAT < lGoodsNum || lGoodsNum <0)
				return;
			
			vector<tagChatsGoodsInfo_tmp> vChatsGoodsInfo_tmp;

			for (LONG i = 0 ; i < lGoodsNum; ++i)
			{
				tagChatsGoodsInfo_tmp ChatsGoodsInfo_tmp;
				pMsg->GetEx(&ChatsGoodsInfo_tmp.ChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
				pMsg->GetStr(ChatsGoodsInfo_tmp.szGoodsName, 255);
				ChatsGoodsInfo_tmp.szGoodsName[255] = 0;
				switch(ChatsGoodsInfo_tmp.ChatsGoodsInfo.lInfoType)
				{
				case eCGIT_Player:
					{
						CPlayer *pOwner = GetGame()->FindPlayer(ChatsGoodsInfo_tmp.ChatsGoodsInfo.OwnerGuid);
						if (NULL == pOwner)
						{
							pMsgPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 61), 0xffff0000, eNOTIFYPOS_CENTER);
							return;
						}
						CGoods *pGoods = pOwner->FindGoodByGuid(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						if (NULL == pGoods)
						{
							pMsgPlayer -> SendNotifyMessage( CStringReading::LoadString(209, 62), 0xffff0000, eNOTIFYPOS_CENTER );
							return;
						}
						CGUID::CreateGUID(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						pGoods->SerializeForOldClient(&ChatsGoodsInfo_tmp.vGoodsData);
					}
					break;
				case eCGIT_Copy:
					break;
				case eCGIT_Npc:
				default:
				    return;
				}

				vChatsGoodsInfo_tmp.push_back(ChatsGoodsInfo_tmp);	
			}


			//!--------------------------------------------------------------------------------------

			if( lType == CHAT_PRIVATE )
			{
				pMsg->GetStr(strChatObjName,255);
			}

			pMsg->GetStr(strChatText,1024);	//chat text
			// 超常的消息，说明是外挂，不发送
			if (lstrlen(strChatText) > 255)
				return;

			
			//! 重组聊天信息
			CMessage newMsg(MSG_C2S_OTHER_TALK);
			newMsg.Add(bTalkType);
			newMsg.Add(lType);
			newMsg.Add(lSenderType);
			newMsg.Add(SenderID);
			newMsg.Add((LONG)vChatsGoodsInfo_tmp.size());
			for (LONG i = 0; i < vChatsGoodsInfo_tmp.size(); ++i)
			{
				newMsg.AddEx(&(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo), sizeof(tagChatsGoodsInfo));
				newMsg.Add(vChatsGoodsInfo_tmp[i].szGoodsName);
			}
			if( lType == CHAT_PRIVATE )
				newMsg.Add(strChatObjName);
			

			//! 发送成功标记
			BOOL bSendSucceed = FALSE;

			// 反外挂：玩家发言可能会增长活跃度
			GetInst( AntiCheat::CPlayerManager ).PlayerTalk( pMsgPlayer, strChatText, strChatObjName, lType );

			switch( lType )
			{			
			// 普通聊天
			case CHAT_NORMAL:
				{
					if (pMsg->GetPlayer()->GetArea()==NULL)
						return;

					newMsg.SetType(MSG_S2C_OTHER_TALK);

					CRegion* pLocalRgn = static_cast<CRegion*>(pMsgPlayer->GetFather());
					if(pMsgPlayer->GetHideFlag() == 1 && pLocalRgn && pLocalRgn->GetRgnHideFlag() != 2)
						newMsg.Add(CGlobeSetup::GetSetup()->szHideName);
					else
						newMsg.Add(pMsgPlayer->GetName());
					newMsg.Add(strChatText);

					BEGIN_LOOP_AROUND_SHAPE_BY_TYPE((CServerRegion*)pMsg->GetRegion(), pMsgPlayer->GetArea()->GetX(), pMsgPlayer->GetArea()->GetY(), TYPE_PLAYER)
					{
						if( pShape->GetType() == TYPE_PLAYER )
						{
							CPlayer* p = (CPlayer*)pShape;

							// 如果是自己
							if(p == pMsgPlayer || p->IsGM())
							{
								//! 重组聊天信息
								CMessage newOwnMsg(MSG_S2C_OTHER_TALK);
								newOwnMsg.Add(bTalkType);
								newOwnMsg.Add(lType);
								newOwnMsg.Add(lSenderType);
								newOwnMsg.Add(SenderID);
								newOwnMsg.Add((LONG)vChatsGoodsInfo_tmp.size());
								for (LONG i = 0; i < vChatsGoodsInfo_tmp.size(); ++i)
								{
									newOwnMsg.AddEx(&(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo), sizeof(tagChatsGoodsInfo));
									newOwnMsg.Add(vChatsGoodsInfo_tmp[i].szGoodsName);
								}
								if( lType == CHAT_PRIVATE )
									newMsg.Add(strChatObjName);

								newOwnMsg.Add(pMsgPlayer->GetName());
								newOwnMsg.Add(strChatText);
								newOwnMsg.SendToPlayer( p->GetExID() );
							}
							else
							{
								// 同屏玩家
								if( abs( (long)(p->GetTileX() - pMsg->GetPlayer()->GetTileX()) ) < AREA_WIDTH &&
									abs( (long)(p->GetTileY() - pMsg->GetPlayer()->GetTileY()) ) < AREA_HEIGHT )
								{
									//if( p != pMsg -> GetPlayer() )
									{
										newMsg.SendToPlayer( p->GetExID() );
									}
								}
							}
						}
					}
					END_LOOP_AROUND_SHAPE_BY_TYPE
					
					bSendSucceed = TRUE;
					CSMClient::GetSM()->SendWatchResult(pMsgPlayer->GetName(),eGGC_NomalChat,
						strChatText);
				}
				break;

			// 地图聊天
			case CHAT_REGION:
				{
					if (pMsg->GetPlayer()->GetLevel() < CGlobeSetup::GetSetup()->lRegionChatLevelLimit)
					{
						char str[256];
						_snprintf(str,  256, CStringReading::LoadString(209, 63),CGlobeSetup::GetSetup()->lRegionChatLevelLimit);
						pMsgPlayer -> SendNotifyMessage(str, 0xffff0000, eNOTIFYPOS_CENTER);
						break;
					}

					newMsg.SetType(MSG_S2C_OTHER_TALK);
					
					newMsg.Add(pMsgPlayer->GetName());
					newMsg.Add(strChatText);

					CServerRegion *pRegion = pMsg->GetRegion();
					if( pRegion )
					{
						DWORD dwNumAreas = pRegion -> GetAreaX() * pRegion -> GetAreaY();
						for( DWORD i = 0; i < dwNumAreas; i ++ )
						{
							CArea* pArea = pRegion -> GetArea( i );
							if( pArea )
							{
								vector<CShape*> vShapes;
								pArea -> FindShapes( TYPE_PLAYER, vShapes );
								for( size_t j = 0; j < vShapes.size(); j ++ )
								{
									CShape* pShape = vShapes[j];
									if( pShape )
									{
										newMsg.SendToPlayer( pShape->GetExID() );
										bSendSucceed = TRUE;
									}
								}
							}
						}
					}
					CSMClient::GetSM()->SendWatchResult(pMsgPlayer->GetName(),eGGC_NomalChat,
						strChatText);
				}
				break;

			// 两人私聊
			case CHAT_PRIVATE:
				{	
					CPlayer* pPlayer = (CPlayer*)GetGame()->FindPlayer(strChatObjName);
					if( pPlayer )
					{
						newMsg.SetType(MSG_S2C_OTHER_TALK);
						newMsg.Add(pMsgPlayer->GetName());
						newMsg.Add(strChatText);
						newMsg.SendToPlayer( pPlayer -> GetExID() );
						if( pMsgPlayer != pPlayer )
						{
							newMsg.SendToPlayer( pMsgPlayer->GetExID() );
						}
					}
					else
					{
						newMsg.SetType(MSG_S2W_OTHER_TALK);
						newMsg.Add(pMsgPlayer->GetName());
						newMsg.Add(strChatText);
						newMsg.Send();	// 发送到worldserver
						// to do log
						// 直接在worldserver日志
					}
					bSendSucceed = TRUE;

					GetGameLogInterface()->logT430_chat_log(pMsgPlayer,strChatObjName,strChatText,lType);
					char szInfo[1200]={0};
					sprintf(szInfo,"<%s> 密聊 %s 聊天内容: %s",pMsgPlayer->GetName(),strChatObjName,strChatText);
					CSMClient::GetSM()->SendWatchResult(pMsgPlayer->GetName(),eGGC_NomalChat,
						szInfo);
				}
				break;

			case CHAT_TEAM:
				{
					//没有队伍
					const CGUID TeamGuid = pMsgPlayer->GetTeamID();
					if(TeamGuid == NULL_GUID)
						return;
					GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(TeamGuid);
					if (NULL == pTeam)
						return;

					//! 如果全队都在本服直接发送，否则通过WS转发
					list<CGUID> relist;
					pTeam->GetOnServerTeamMember(relist);
					if (relist.size() == pTeam->GetMemberNum())
					{
						newMsg.SetType(MSG_S2C_OTHER_TALK);
						newMsg.Add(pMsgPlayer->GetName());
						newMsg.Add(strChatText);
						for (list<CGUID>::iterator ite = relist.begin(); ite != relist.end(); ++ite)
						{
							newMsg.SendToPlayer(*ite);
						}
					}
					else
					{
						newMsg.SetType(MSG_S2W_OTHER_TALK);
						newMsg.Add(pMsgPlayer->GetName());
						newMsg.Add(strChatText);
						newMsg.Add(TeamGuid);
						newMsg.Send();
					}

					GetGameLogInterface()->logT430_chat_log(pMsgPlayer,strChatObjName,strChatText,lType);
					bSendSucceed = TRUE;
					CSMClient::GetSM()->SendWatchResult(pMsgPlayer->GetName(),eGGC_NomalChat,
						strChatText);
				}
				break;
			case CHAT_FACTION:
				{
					//没有帮会
					const CGUID FactionGuid = pMsgPlayer->GetFactionID();
					if(FactionGuid == NULL_GUID)
						return;

					newMsg.SetType(MSG_S2W_OTHER_TALK);
					newMsg.Add(pMsgPlayer->GetName());
					newMsg.Add(strChatText);
					newMsg.Add(FactionGuid);
					newMsg.Send();

					GetGameLogInterface()->logT430_chat_log(pMsgPlayer,"",strChatText,lType);

					bSendSucceed = TRUE;
					CSMClient::GetSM()->SendWatchResult(pMsgPlayer->GetName(),eGGC_NomalChat,
						strChatText);
				}
				break;

			// 运行脚本
			case RUN_SCRIPT:
				{
					if (CGlobeSetup::GetSetup()->bAllowClientRunScript)
					{
						stRunScript st;
						st.pszFileName   = NULL;
						st.srcShape  = pMsg->GetPlayer();
						st.desShape	 = NULL;
						st.pRegion   = pMsg->GetRegion();
#ifdef _RUNSTACKINFO_
						char pszStatckInfo[10240]="";
						_snprintf(pszStatckInfo,10240,"RUN_SCRIPT(%s) Start",strChatText);
						CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
						RunScript(&st,strChatText);	
					}
				}
				break;
			}
			
			
			//! 保存聊天中的物品信息
			if (bSendSucceed && 0 < vChatsGoodsInfo_tmp.size())
			{
				vector<tagChatsGoodsInfo_tmp> ;

				CMessage msgGoodsInfo(MSG_S2W_OTHER_GoodsInfo_Create);

				LONG lNewInfoNum = 0;
				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
						++lNewInfoNum;
				}
				msgGoodsInfo.Add(lNewInfoNum);

				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
					{
						CGoodsFactory::AddGoodsInfo(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid, vChatsGoodsInfo_tmp[i].vGoodsData);
						msgGoodsInfo.Add(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid);
						msgGoodsInfo.Add((LONG)vChatsGoodsInfo_tmp[i].vGoodsData.size());
						msgGoodsInfo.AddEx(&(vChatsGoodsInfo_tmp[i].vGoodsData[0]), vChatsGoodsInfo_tmp[i].vGoodsData.size());
					}
				}

				msgGoodsInfo.Send();
			}
			
		}
		break;

	case MSG_W2S_OTHER_TALK:	// 对话消息
		{
			BYTE bTalkType= pMsg->GetByte();
			long lType = pMsg->GetLong();
			char strTargetName[256] = "";
			long lsenderType = pMsg->GetLong();	// SenderType
			CGUID PlayerGuid; pMsg->GetGUID(PlayerGuid);	// playerID

			LONG lGoodsNum = pMsg->GetLong();

			//! -----------------跳过物品信息部分-----------------
			tagChatsGoodsInfo tmpChatsGoodsInfo;
			char szGoodsName[256] = {0};
			for (LONG i = 0 ; i < lGoodsNum; ++i)
			{
				pMsg->GetEx(&tmpChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
				pMsg->GetStr(szGoodsName, 255);
			}
			//! ----------------------------------

			static char strSourceName[256]="";
			static char strText[1024]="";
			
			if(CHAT_PRIVATE == lType)
			{
				pMsg->GetStr(strTargetName,256);
				//! 密聊失败时，消息后面已经没有内容了
				if(9 == bTalkType)
				{
					CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
					if(NULL != pPlayer)
					{
						char szMsg[128] = {0};
						_snprintf(szMsg, 128, CStringReading::LoadString(209, 66), strTargetName);
						pPlayer -> SendNotifyMessage( szMsg , 0xffff0000, eNOTIFYPOS_CENTER);
					}
					return;
				}
			}

			
			pMsg->GetStr(strSourceName,256);
			pMsg->GetStr(strText,1024);

			switch(lType)
			{
			case CHAT_PRIVATE:
				{
					pMsg->SetType(MSG_S2C_OTHER_TALK);
					CPlayer *pTarget = GetGame()->FindPlayer(strTargetName);
					if(NULL != pTarget)
						pMsg->SendToPlayer(pTarget->GetExID());
					pMsg->SendToPlayer(PlayerGuid);
				}
				break;
			case CHAT_TEAM://! 队伍
				{
					CGUID TeamGuid;
					pMsg->GetGUID(TeamGuid);
					if(TeamGuid == NULL_GUID)
						return;
					GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(TeamGuid);
					if (NULL == pTeam)
						return;

					list<CGUID> relist;
					pTeam->GetOnServerTeamMember(relist);

					pMsg->SetType(MSG_S2C_OTHER_TALK);
					for (list<CGUID>::iterator ite = relist.begin(); ite != relist.end(); ++ite)
					{
						pMsg->SendToPlayer(*ite);
					}
				}
				break;
			case CHAT_FACTION://! 工会
				{
					CGUID FactionGuid;
					pMsg->GetGUID(FactionGuid);
					if(FactionGuid == NULL_GUID)
						return;
					GameFaction *pFaction = GetOrganiCtrl()->GetFactionOrganizing(FactionGuid);
					if (NULL == pFaction)
						return;

					pMsg->SetType(MSG_S2C_OTHER_TALK);
					pFaction->SendToAll(*pMsg);
				}
				break;
			}
		}
		break;
	/* [2007-9-11 del]
	case MSG_W2S_OTHER_FACTIONTALK:
		{
			long lTarPlayerID = pMsg->GetLong();
			long lsenderType = pMsg->GetLong();
			CGUID PlayerID; pMsg->GetGUID(PlayerID);
			static char charstrSourceName[256];
			static char strText[1024];
			CPlayer *pTargetPlayer = GetGame()->FindPlayer(lTarPlayerID);
			if (pTargetPlayer)
			{
				pMsg->GetStr(charstrSourceName,256);
				pMsg->GetStr(strText,1024);

				CMessage msg(MSG_S2C_OTHER_TALK);
				msg.Add((long)CHAT_UNION);
				msg.Add(lsenderType);
				msg.Add(lplayerID);
				msg.Add(charstrSourceName);
				msg.Add(strText);
				msg.SendToPlayer(pTargetPlayer->GetID());
			}
		}
		break;
	*/
	// 对话选择
	case MSG_C2S_OTHER_SENDTALKBOXSELECT:
		{
			if (pMsg->GetPlayer() == NULL || pMsg->GetRegion() == NULL)
			{
				//assert(0);
				return;
			}

			if (pMsg->GetPlayer()->JudgeNpcScriptslegality())
			{
				return;	
			}

			CGUID guScriptID;
			pMsg->GetGUID(guScriptID);
			long ret = pMsg->GetLong();	// 选项

			//点击热区链接
			if (ret == 1)
			{
				BYTE GoodsIndex= pMsg->GetByte();
				pMsg->GetPlayer()->SetScriptVarValue(NULL,"$Choose_Ret",0,GoodsIndex);
				
				//// 有效性判断
				//char szRetInfo[1024];
				//_snprintf(szRetInfo, "[GOODS %d", GoodsIndex);
				//if(ClientOptIsValid(guScriptID, szRetInfo))
				//	pMsg->GetPlayer()->SetScriptVarValue(NULL,"$Choose_Ret",0,GoodsIndex);
				
				static char chLink[50];
				pMsg->GetStr(chLink,50);

				//// 有效性判断
				//_snprintf(szRetInfo, "[A %s", chLink);
				//if(!ClientOptIsValid(guScriptID, szRetInfo)) return;

				//// 通过检查
				//ClearClientOptData(guScriptID);

				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar("$m_TalkRet", atoi(chLink));
				}
				ContinueRunScript(guScriptID, stackVarList);
				break;
			}
			else if (ret == -1)
			{
				//删除此脚本
				DelectPlayerScript(guScriptID,pMsg->GetPlayer()->GetExID());
			}
			else if (ret == 0)	// 脚本继续执行
			{
				ret = pMsg->GetLong();
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddVar("$m_TalkRet", ret);
				}
				ContinueRunScript(guScriptID, stackVarList);
			}
		}
		break;
	case MSG_C2S_OTHER_GoodsInfo_Request:
		{
			CGUID InfoKey;
			pMsg->GetGUID(InfoKey);
			vector<BYTE> vGoodsInfo;
			if (CGoodsFactory::GetGoodsInfo(InfoKey, vGoodsInfo))
			{
				CMessage msg(MSG_S2C_OTHER_GoodsInfo_Re);
				msg.Add(pMsg->GetPlayerID());
				msg.Add(InfoKey);
				msg.Add((LONG)vGoodsInfo.size());
				msg.AddEx(&vGoodsInfo[0], vGoodsInfo.size());
				msg.SendToSocket(pMsg->GetSocketID());
			}
			else
			{
				CMessage msg(MSG_S2W_OTHER_GoodsInfo_Request);
				msg.Add(pMsg->GetPlayerID());
				msg.Add(InfoKey);
				msg.Send();
			}
		}
		break;
	case MSG_W2S_OTHER_GoodsInfo_Re:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);

			CGUID InfoKey;
			pMsg->GetGUID(InfoKey);

			LONG lDataSize = pMsg->GetLong();
			vector<BYTE> vGoodsInfo(lDataSize + 1);

			pMsg->GetEx(&vGoodsInfo[0], lDataSize);

			CGoodsFactory::AddGoodsInfo(InfoKey, vGoodsInfo);

			pMsg->SetType(MSG_S2C_OTHER_GoodsInfo_Re);
			pMsg->SendToPlayer(PlayerGuid);
		}
		break;
	case MSG_W2S_OTHER_BROADCAST://worldserver转发的系统广播
		{
			pMsg->SetType(MSG_S2C_OTHER_BROADCAST);

			CGUID lRegionID;
			pMsg->GetGUID(lRegionID);

			// 世界广播
			if (lRegionID == NULL_GUID)
			{
				pMsg->SendAll();
			}
			else	// 场景广播
			{
				CServerRegion *pRegion = GetGame()->FindRegion(lRegionID);
				if (pRegion)
					pMsg->SendToRegion(pRegion);
			}			
		}
		break;
		//顶端提示信息
	case MSG_W2S_OTHER_AddTopInfo:
		{
			pMsg->SetType(MSG_S2C_OTHER_AddTopInfo);
			CGUID PlayerID; pMsg->GetGUID(PlayerID);
			if(PlayerID == NULL_GUID)
			{
				pMsg->SendAll();
			}
			else
			{
				pMsg->SendToPlayer(PlayerID);
			}
		}
		break;
	case MSG_W2S_OTHER_GOODSLINK://worldserver返回的物品超链结果
		{
			long lType=pMsg->GetLong();
			long lSenderType=pMsg->GetLong();
			CGUID SenderID;
			pMsg->GetGUID(SenderID);
			CPlayer* pSender=GetGame()->FindPlayer(SenderID);			
			if(!pSender)
				return;

			switch( lType )
			{		
				// 普通聊天		
			case CHAT_NORMAL:
				{
					if (pSender->GetArea()==NULL)
						return;

					pMsg->SetType(MSG_S2C_OTHER_TALK);
					BEGIN_LOOP_AROUND_SHAPE((CServerRegion*)pSender->GetFather(), pSender->GetArea()->GetX(), pSender->GetArea()->GetY())
					{
						if( pShape->GetType() == TYPE_PLAYER )
						{
							CPlayer* p = (CPlayer*)pShape;

							// 同屏玩家
							if( abs( (long)(p->GetTileX() - pSender->GetTileX()) ) < AREA_WIDTH &&
								abs( (long)(p->GetTileY() - pSender->GetTileY()) ) < AREA_HEIGHT )
							{
								pMsg->SendToPlayer( p->GetExID() );
							}
						}
					}
					END_LOOP_AROUND_SHAPE
				}
				break;

				// 地图聊天
			case CHAT_REGION:
				{
					pMsg->SetType(MSG_S2C_OTHER_TALK);
					BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pMsg->GetRegion(), TYPE_PLAYER)
					{
						//if(pShape)
							pMsg->SendToPlayer( pShape->GetExID() );
					}
					END_LOOP_REGION_SHAPE_BY_TYPE
				}
				break;				
			}
		}
		break;
	case MSG_C2S_OTHER_VIEWGOODSLINK:	//client请求查看一个物品超联
		{
			CGUID PlayerID; 
			pMsg->GetGUID(PlayerID);
			DWORD dwID=pMsg->GetDWord();
			//消息发送到worldserver请求id==dwID的物品超链
			CMessage msg(MSG_S2W_OTHER_VIEWGOODSLINK);			
			msg.Add(PlayerID);
			msg.Add(dwID);
			msg.Send();
		}
		break;
	case MSG_W2S_OTHER_VIEWGOODSLINK://worldserver对查看物品超链的返回消息
		{
			CGUID PlayerID; 
			pMsg->GetGUID(PlayerID);		
			CPlayer* pPlayer=GetGame()->FindPlayer(PlayerID);
			if(pPlayer)
			{
				pMsg->SetType(MSG_S2C_OTHER_VIEWGOODSLINK);
				pMsg->SendToPlayer(pPlayer -> GetExID());
			}
		}
		break;
	case MSG_W2S_OTHER_ADDINFO://跨服务器指定给某个玩家发送一条系统提示消息
		{
			pMsg->GetDWord();
			CGUID PlayerID; 
			pMsg->GetGUID(PlayerID);
			CPlayer* pPlayer=GetGame()->FindPlayer(PlayerID);
			if(pPlayer)
			{
				char strContent[256];
				DWORD dwColor=pMsg->GetDWord();
				DWORD dwBkColor=pMsg->GetDWord();
				pMsg->GetStr(strContent,256);
				pPlayer->SendNotifyMessage(strContent,dwColor,dwBkColor);
			}
		}
		break;
	case MSG_W2S_OTHER_ADDSKILL://跨服务器给一个玩家添加技能(worldserver转发)
		{
			//char strVal[256];
			//char strPlayerName[256];
			//char strSkillName[256];
			//pMsg->GetStr(strPlayerName,256);	//get player name
			//CPlayer* pPlayer=NULL;
			//pPlayer=GetGame()->FindPlayer(strPlayerName);
			//if(pPlayer)
			//{
			//	pMsg->GetStr(strSkillName,256);	//get skill name
			//	WORD wLevel=pMsg->GetWord(); //get skill level
			//	long lPlayerID=pMsg->GetLong();	//get playerid
			//	DWORD dwGSIndex=pMsg->GetDWord();	//get gameserverindex
			//	pPlayer->SetSkillLevel(strSkillName,wLevel);

			//	tagSkillID tID = CSkillFactory::QuerySkillID( strSkillName );
			//	if( tID != SKILL_UNKNOW ) 
			//	{
			//		CSkill* pSkill = pPlayer->GetSkill(tID);
			//		if( pSkill )
			//		{
			//			//msg to client
			//			CMessage msg(MSG_S2C_PLAYER_ADDSKILL);
			//			msg.Add(strSkillName);
			//			msg.Add(wLevel);
			//			msg.Add(pSkill->GetRestoreTime());
			//			msg.Add((WORD)pSkill->GetAffectRangeMin());
			//			msg.Add((WORD)pSkill->GetAffectRangeMax());
			//			msg.Add((WORD)pSkill->GetAffectCostMP());
			//			msg.SendToPlayer(pPlayer -> GetExID());
			//		}
			//	}

			//	//msg to server
			//	CMessage msg2World(MSG_S2W_OTHER_ADDINFO);
			//	msg2World.Add(dwGSIndex);
			//	msg2World.Add(lPlayerID);
			//	msg2World.Add(static_cast<DWORD>(0xffffffff));
			//	msg2World.Add( DWORD(0) );	// 背景色
			//	_snprintf(strVal,"玩家:%s的技能%s等级已经被设置为:%d",strPlayerName,strSkillName,wLevel);
			//	msg2World.Add(strVal);
			//	msg2World.Send();
			//}			
		}
		break;
	case MSG_W2S_OTHER_DELGOODS://跨服务器删除一个玩家身上的物品(worldserver转发)
		{
			char strPlayerName[256];
			pMsg->GetStr(strPlayerName,256);
			CPlayer* pPlayer=GetGame()->FindPlayer(strPlayerName);
			if(pPlayer)
			{
				char strGoodsName[256];
				pMsg->GetStr(strGoodsName,256);
				//pPlayer->DelGoods(strGoodsName,pMsg->GetLong());
			}
		}
		break;
	case MSG_W2S_OTHER_DELSKILL://跨服务器删除一个玩家的技能(worldserver转发)
		{
		}
		break;
	case MSG_W2S_OTHER_SETLEVEL://跨服务器设置一个玩家的等级(worldserver转发)
		{
			char strPlayerName[256];
			pMsg->GetStr(strPlayerName,256);
			CPlayer* pPlayer=GetGame()->FindPlayer(strPlayerName);
			if(pPlayer)
			{
				//BYTE lLevel=pMsg->GetByte();
				WORD wLevel = pMsg->GetWord();
				pPlayer->SetLevel(wLevel, TRUE);

				//得到该等级的初始经验
				LONG64 dwExp = CPlayerList::GetPlayerTotalExp(pPlayer->GetLevel()-2);
				pPlayer->IncExp(dwExp, eAET_NPC);
				pPlayer -> AddIncExpLog(pPlayer->GetExp(), pPlayer->GetExpMultiple(), CPlayerList::GetPlayerExp(pPlayer->GetLevel()-2), CPlayerList::GetPlayerExpMultiple(pPlayer->GetLevel()-2), eAET_NPC, "MSG_W2S_OTHER_SETLEVEL");

				CMessage msg(MSG_S2C_PLAYER_SETLEVEL);
				msg.Add(wLevel);
				msg.Add(pPlayer->GetExp());
				msg.Add(pPlayer->GetExpMultiple());
				msg.Add(pPlayer->GetNextExp());
				msg.Add(pPlayer->GetNextExpMultiple());
				msg.SendToPlayer(pPlayer -> GetExID());
			} 
		}
		break;
	case MSG_C2S_OTHER_CONNECTGAMESERVER:
		{
			if ( pMsg->GetPlayer() )
			{char szID[40] = {0}; pMsg->GetPlayer()->GetExID().tostring(szID);
				AddErrorLogText("(%s)%s 切换服务器失败!!!!", szID, pMsg->GetPlayer()->GetName());
			}

			GetGame()->GetNetServer()->DisConn(pMsg->GetSocketID());
		}
		break;
	case MSG_W2S_OTHER_KICKALL:
		{
			GetGame()->KickAllPlayer();
		}
		break;
	case MSG_W2S_OTHER_KICKPLAYER:
		{
			BYTE cFlag = pMsg->GetByte();
			CGUID playerID;
			pMsg->GetGUID(playerID);
			
			switch(cFlag)
			{
			case AT_ONCE_KICK:
				{
					GetGame()->KickPlayer(playerID);
				}
				break;
			case FCM_KICK_PLAYER:
				{
					CPlayer *pAim = GetGame()->FindPlayer(playerID);
					if(NULL != pAim)
					{
						pAim->RegisterFcmKickPlayerTimerID(CGlobeSetup::GetSetup()->lRgnKickPlayerDeltaTime);
						CMessage msg(MSG_S2C_OTHER_TIME_INFO_BOX);
						msg.Add((BYTE)FCM_KICK_PLAYER);
						msg.Add((long)CGlobeSetup::GetSetup()->lRgnKickPlayerDeltaTime);
						msg.SendToPlayer(playerID);
					}
					else
					{
						char szGuid[128] = {0};
						playerID.tostring(szGuid);
						AddErrorLogText("MSG_W2S_OTHER_KICKPLAYER: fcm kick player lose, can't find player【%s】", szGuid);
					}
				}
			    break;
			default:
				{
					AddErrorLogText("MSG_W2S_OTHER_KICKPLAYER: error flag【%d】", (LONG)cFlag);
				}
			    break;
			}
			
		}
		break;
	//case MSG_C2S_OTHER_CHANGENAME:	// c发送更名请求
	//	{
	//		pMsg->SetType(MSG_S2W_OTHER_CHANGENAME);
	//		pMsg->Send();
	//	}
	//	break;

	//case MSG_W2S_OTHER_CHANGENAME:	// Ws发送更名结果到c
	//	{
	//		char szNewName[32];
	//		CGUID PlayerID; pMsg->GetGUID(PlayerID);
	//		char cRes = pMsg->GetChar();
	//		pMsg->GetStr(szNewName,32);

	//		CPlayer *pPlayer = GetGame()->FindPlayer( PlayerID );
	//		if(pPlayer) {
	//			pMsg->SetType(MSG_S2C_OTHER_CHANGENAME);
	//			if( !cRes ) {
	//				// cRes==0时成功，其他失败
	//				pPlayer->SetName(szNewName); // 更改服务器上的NAME
	//				pMsg->SendToAround(pPlayer); // 成功后通知周围的人群

	//				// Fox@20081110 刷新角色名称
	//				GetInst(CPlayerMan).RefreshElem(PET_NAME, pPlayer);
	//			}
	//			else {
	//				// 失败通知操作玩家
	//				pMsg->SendToPlayer(PlayerID);
	//			}

	//			// 通知队伍
	//			//LONG lSessionID = GetGame() -> GetTeamSessionID( pPlayer -> GetTeamID() );
	//			//CTeam* pTeam = dynamic_cast<CTeam*>( const_cast<CSession*>( CSessionFactory::QuerySession( lSessionID ) ) );
	//		}
	//	}
	//	break;

	case MSG_C2S_OTHER_QUESTNPCLIST:	// 请求NPC列表
		{
			CServerRegion * pRegion = dynamic_cast<CServerRegion*>(pMsg->GetRegion());
			if (pRegion && pMsg->GetPlayer())
			{
				CMessage msg(MSG_S2C_OTHER_ANSWERNPCLIST);
				vector<BYTE> & str= pRegion->GetNpcNameList();
				if (str.size()==0)
				{
					msg.Add((LONG)0);
				}
				else
				{
					msg.Add(pRegion->GetNpcNameListNum());
					msg.Add((LONG)str.size());
					msg.AddEx(( void*)&str[0],str.size());
				}
				msg.SendToPlayer(pMsg->GetPlayer()->GetExID());
			}
		}
		break;
		//处理C发过来的置顶世界喊话
	case MSG_C2S_OTHER_TALK_TOP_WORLD:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(!pPlayer) return;

			if( pPlayer -> IsInSilence() )
			{
				pPlayer -> SendNotifyMessage( "你已经被禁言！" );
				break;
			}

			if(0 == CMessage::GetWSSocket())
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 67), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			//!--------------------------------------------------------------------------------------
	
			char szContent[1024];
			long lContainerID = pMsg->GetLong();
			long lPos		  = pMsg->GetLong();

			pMsg->GetStr(szContent,1024);

			if(!pPlayer) return;

			// 查询特殊道具
			if (CGlobeSetup::GetSetup()->nTalkTopWorldGoodsNum)
			{
				if (pPlayer->FindGoods(lContainerID,lPos,CGlobeSetup::GetSetup()->szTalkTopWorldGoodsName)<CGlobeSetup::GetSetup()->nTalkTopWorldGoodsNum)				
				{
					//失败
					char szTmp[256];					
					_snprintf(szTmp, 256, CStringReading::LoadString(209, 64), CGlobeSetup::GetSetup()->szTalkTopWorldGoodsName);
					pPlayer -> SendNotifyMessage(szTmp, 0xffff0000, eNOTIFYPOS_CENTER);
					return;
				}
			}

			// 查询金币
			if (pPlayer->GetMoney() < CGlobeSetup::GetSetup()->lTalkTopWorldGold)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			// 查询银币
			if (pPlayer->GetSilver() < CGlobeSetup::GetSetup()->lTalkTopWorldSilver)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}


			if (CGlobeSetup::GetSetup()->nTalkTopWorldGoodsNum)
			{
				CGoods *pGoods = pPlayer->FindGoods(lContainerID,lPos);
				if (pGoods)
				{
					if( !pPlayer->DeleteGoods((CS2CContainerObjectMove::PLAYER_EXTEND_ID)lContainerID,pGoods->GetExID(),CGlobeSetup::GetSetup()->nTalkTopWorldGoodsNum))
					{
						//删除物品失败就直接返回
						return;
					}
				}
				else 
				{
					return;
				}
				
			}
				

			if (CGlobeSetup::GetSetup()->lTalkTopWorldGold)
				pPlayer -> SetMoney( pPlayer->GetMoney() - CGlobeSetup::GetSetup()->lTalkWorldGold,19);

			if (CGlobeSetup::GetSetup()->lTalkTopWorldSilver)
				pPlayer -> SetSilver( pPlayer->GetSilver() - CGlobeSetup::GetSetup()->lTalkWorldSilver);

			CMessage msg(MSG_S2W_OTHER_TALK_TOP_WORLD);
		
			msg.Add((char*)pPlayer->GetName());
			msg.Add(szContent);
			msg.Send();

			
		}
		break;
		// 处理c接收过来的世界喊话
	case MSG_C2S_OTHER_TALK_WORLD:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(!pPlayer) return;

			if( pPlayer -> IsInSilence() )
			{
				pPlayer -> SendNotifyMessage( "你已经被禁言！" );
				break;
			}

			if(0 == CMessage::GetWSSocket())
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 67), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			//!--------------------------------------------------------------------------------------
			//! 聊天中的物品信息提取
			LONG lGoodsNum = pMsg->GetLong();
			if(MAX_GOODS_NUM_IN_CHAT < lGoodsNum  || lGoodsNum < 0)
				return;

			vector<tagChatsGoodsInfo_tmp> vChatsGoodsInfo_tmp;

			for (LONG i = 0 ; i < lGoodsNum; ++i)
			{
				tagChatsGoodsInfo_tmp ChatsGoodsInfo_tmp;
				pMsg->GetEx(&ChatsGoodsInfo_tmp.ChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
				pMsg->GetStr(ChatsGoodsInfo_tmp.szGoodsName, 255);
				ChatsGoodsInfo_tmp.szGoodsName[255] = 0;
				switch(ChatsGoodsInfo_tmp.ChatsGoodsInfo.lInfoType)
				{
				case eCGIT_Player:
					{
						CPlayer *pOwner = GetGame()->FindPlayer(ChatsGoodsInfo_tmp.ChatsGoodsInfo.OwnerGuid);
						if (NULL == pOwner)
						{
							pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 61), 0xffff0000, eNOTIFYPOS_CENTER);
							return;
						}
						CGoods *pGoods = pOwner->FindGoodByGuid(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						if (NULL == pGoods)
						{
							pPlayer -> SendNotifyMessage( CStringReading::LoadString(209, 62) , 0xffff0000, eNOTIFYPOS_CENTER);
							return;
						}
						CGUID::CreateGUID(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						pGoods->SerializeForOldClient(&ChatsGoodsInfo_tmp.vGoodsData);
					}
					break;
				case eCGIT_Copy:
					break;
				case eCGIT_Npc:
				default:
					return;
				}

				vChatsGoodsInfo_tmp.push_back(ChatsGoodsInfo_tmp);	
			}


			//!--------------------------------------------------------------------------------------
			

			char szContent[1024];
			pMsg->GetStr(szContent,1024);

			// 查询特殊道具
			if (CGlobeSetup::GetSetup()->nTalkWorldGoodsNum)
			{
				// 成功
				if( !pPlayer->FindGoods(CGlobeSetup::GetSetup()->szTalkWorldGoodsName, CGlobeSetup::GetSetup()->nTalkWorldGoodsNum))
				{
					char szTmp[512] = {0};
					CGoodsBaseProperties *pBaseProperties = CGoodsFactory::QueryGoodsBasePropertiesByOriginalName(CGlobeSetup::GetSetup()->szTalkWorldGoodsName);
					if(NULL != pBaseProperties)
					{
						_snprintf(szTmp, 512, CStringReading::LoadString(209, 64), pBaseProperties->GetName());
						pPlayer -> SendNotifyMessage(szTmp, 0xffff0000, eNOTIFYPOS_CENTER);
					}
					return;
				}
			}

			// 查询金币
			if (pPlayer->GetMoney() < CGlobeSetup::GetSetup()->lTalkWorldGold)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			// 查询银币
			if (pPlayer->GetSilver() < CGlobeSetup::GetSetup()->lTalkWorldSilver)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}


			if (CGlobeSetup::GetSetup()->nTalkWorldGoodsNum)
				pPlayer->DelGoods(CGlobeSetup::GetSetup()->szTalkWorldGoodsName, CGlobeSetup::GetSetup()->nTalkWorldGoodsNum);

			if (CGlobeSetup::GetSetup()->lTalkWorldGold)
				pPlayer -> SetMoney( pPlayer->GetMoney() - CGlobeSetup::GetSetup()->lTalkWorldGold,20);

			if (CGlobeSetup::GetSetup()->lTalkWorldSilver)
				pPlayer -> SetSilver( pPlayer->GetSilver() - CGlobeSetup::GetSetup()->lTalkWorldSilver);

			CMessage msg(MSG_S2W_OTHER_TALK_WORLD);
			msg.Add((LONG)vChatsGoodsInfo_tmp.size());
			for (LONG i = 0; i < vChatsGoodsInfo_tmp.size(); ++i)
			{
				msg.AddEx(&(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo), sizeof(tagChatsGoodsInfo));
				msg.Add(vChatsGoodsInfo_tmp[i].szGoodsName);
			}
			msg.Add(BYTE(1));
			msg.Add((char*)pPlayer->GetName());
			msg.Add(szContent);
			msg.Send();

			GetGameLogInterface()->logT430_chat_log(pPlayer,"",szContent,CHAT_WOLRD);

			//!--------------------------------------------------------------------------------------
			//! 保存聊天中的物品信息
			if (0 < vChatsGoodsInfo_tmp.size())
			{
				vector<tagChatsGoodsInfo_tmp> ;

				CMessage msgGoodsInfo(MSG_S2W_OTHER_GoodsInfo_Create);

				LONG lNewInfoNum = 0;
				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
						++lNewInfoNum;
				}
				msgGoodsInfo.Add(lNewInfoNum);

				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
					{
						CGoodsFactory::AddGoodsInfo(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid, vChatsGoodsInfo_tmp[i].vGoodsData);
						msgGoodsInfo.Add(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid);
						msgGoodsInfo.Add((LONG)vChatsGoodsInfo_tmp[i].vGoodsData.size());
						msgGoodsInfo.AddEx(&(vChatsGoodsInfo_tmp[i].vGoodsData[0]), vChatsGoodsInfo_tmp[i].vGoodsData.size());
					}
				}

				msgGoodsInfo.Send();
			}
			//!--------------------------------------------------------------------------------------


		}
		break;
	case MSG_C2S_OTHER_TALK_COUNTRY_UNION:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(!pPlayer) return;

			if( pPlayer -> IsInSilence() )
			{
				pPlayer -> SendNotifyMessage( "你已经被禁言！" );
				break;
			}

			if(0 == CMessage::GetWSSocket())
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 67), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			//!--------------------------------------------------------------------------------------
			//! 聊天中的物品信息提取
			LONG lGoodsNum = pMsg->GetLong();
			if(MAX_GOODS_NUM_IN_CHAT < lGoodsNum  || lGoodsNum < 0)
				return;

			vector<tagChatsGoodsInfo_tmp> vChatsGoodsInfo_tmp;

			for (LONG i = 0 ; i < lGoodsNum; ++i)
			{
				tagChatsGoodsInfo_tmp ChatsGoodsInfo_tmp;
				pMsg->GetEx(&ChatsGoodsInfo_tmp.ChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
				pMsg->GetStr(ChatsGoodsInfo_tmp.szGoodsName, 255);
				ChatsGoodsInfo_tmp.szGoodsName[255] = 0;
				switch(ChatsGoodsInfo_tmp.ChatsGoodsInfo.lInfoType)
				{
				case eCGIT_Player:
					{
						CPlayer *pOwner = GetGame()->FindPlayer(ChatsGoodsInfo_tmp.ChatsGoodsInfo.OwnerGuid);
						if (NULL == pOwner)
						{
							pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 61), 0xffff0000, eNOTIFYPOS_CENTER);
							return;
						}
						CGoods *pGoods = pOwner->FindGoodByGuid(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						if (NULL == pGoods)
						{
							pPlayer -> SendNotifyMessage( CStringReading::LoadString(209, 62) , 0xffff0000, eNOTIFYPOS_CENTER);
							return;
						}
						CGUID::CreateGUID(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						pGoods->SerializeForOldClient(&ChatsGoodsInfo_tmp.vGoodsData);
					}
					break;
				case eCGIT_Copy:
					break;
				case eCGIT_Npc:
				default:
					return;
				}

				vChatsGoodsInfo_tmp.push_back(ChatsGoodsInfo_tmp);	
			}


			//!--------------------------------------------------------------------------------------
			char szContent[1024];
			pMsg->GetStr(szContent,1024);

			// 查询特殊道具
			if (CGlobeSetup::GetSetup()->nTalkUnionGoodsNum)
			{
				// 成功
				if( !pPlayer->FindGoods(CGlobeSetup::GetSetup()->szTalkUnionGoodsName, CGlobeSetup::GetSetup()->nTalkUnionGoodsNum) )
				{
					char szTmp[512] = {0};
					CGoodsBaseProperties *pBaseProperties = CGoodsFactory::QueryGoodsBasePropertiesByOriginalName(CGlobeSetup::GetSetup()->szTalkUnionGoodsName);
					if(NULL != pBaseProperties)
					{
						_snprintf(szTmp, 512, CStringReading::LoadString(209, 64), pBaseProperties->GetName());
						pPlayer -> SendNotifyMessage(szTmp, 0xffff0000, eNOTIFYPOS_CENTER);
					}
					return;
				}
			}

			// 查询钱
			if (pPlayer->GetMoney() < CGlobeSetup::GetSetup()->lTalkUnionGold)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			if (pPlayer->GetSilver() < CGlobeSetup::GetSetup()->lTalkUnionSilver)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}


			if (CGlobeSetup::GetSetup()->nTalkUnionGoodsNum)
				pPlayer->DelGoods(CGlobeSetup::GetSetup()->szTalkCountryGoodsName, CGlobeSetup::GetSetup()->nTalkCountryGoodsNum);

			if (CGlobeSetup::GetSetup()->lTalkUnionGold)
				pPlayer -> SetMoney( pPlayer->GetMoney() - CGlobeSetup::GetSetup()->lTalkUnionGold,21 );

			if (CGlobeSetup::GetSetup()->lTalkUnionSilver)
				pPlayer -> SetSilver( pPlayer->GetSilver() - CGlobeSetup::GetSetup()->lTalkUnionSilver );

			CMessage msg(MSG_S2W_OTHER_TALK_COUNTRY_UNION);
			msg.Add((LONG)vChatsGoodsInfo_tmp.size());
			for (LONG i = 0; i < vChatsGoodsInfo_tmp.size(); ++i)
			{
				msg.AddEx(&(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo), sizeof(tagChatsGoodsInfo));
				msg.Add(vChatsGoodsInfo_tmp[i].szGoodsName);
			}
			msg.Add(BYTE(1));
			msg.Add((char*)pPlayer->GetName());
			msg.Add(szContent);
			msg.Add(pPlayer->GetCountry());
			msg.Send();

			GetGameLogInterface()->logT430_chat_log(pPlayer,"",szContent,CHAT_UNION);

			//!--------------------------------------------------------------------------------------
			//! 保存聊天中的物品信息
			if (0 < vChatsGoodsInfo_tmp.size())
			{
				vector<tagChatsGoodsInfo_tmp> ;

				CMessage msgGoodsInfo(MSG_S2W_OTHER_GoodsInfo_Create);

				LONG lNewInfoNum = 0;
				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
						++lNewInfoNum;
				}
				msgGoodsInfo.Add(lNewInfoNum);

				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
					{
						CGoodsFactory::AddGoodsInfo(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid, vChatsGoodsInfo_tmp[i].vGoodsData);
						msgGoodsInfo.Add(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid);
						msgGoodsInfo.Add((LONG)vChatsGoodsInfo_tmp[i].vGoodsData.size());
						msgGoodsInfo.AddEx(&(vChatsGoodsInfo_tmp[i].vGoodsData[0]), vChatsGoodsInfo_tmp[i].vGoodsData.size());
					}
				}

				msgGoodsInfo.Send();
			}
			//!--------------------------------------------------------------------------------------

		}
		break;

		// 处理c接收过来的国家喊话
	case MSG_C2S_OTHER_TALK_COUNTRY:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(!pPlayer) return;

			if( pPlayer -> IsInSilence() )
			{
				pPlayer -> SendNotifyMessage( "你已经被禁言！" );
				break;
			}

			if(0 == CMessage::GetWSSocket())
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 67), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			//!--------------------------------------------------------------------------------------
			//! 聊天中的物品信息提取
			LONG lGoodsNum = pMsg->GetLong();
			if(MAX_GOODS_NUM_IN_CHAT < lGoodsNum  || lGoodsNum < 0)
				return;

			vector<tagChatsGoodsInfo_tmp> vChatsGoodsInfo_tmp;

			for (LONG i = 0 ; i < lGoodsNum; ++i)
			{
				tagChatsGoodsInfo_tmp ChatsGoodsInfo_tmp;
				pMsg->GetEx(&ChatsGoodsInfo_tmp.ChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
				pMsg->GetStr(ChatsGoodsInfo_tmp.szGoodsName, 255);
				ChatsGoodsInfo_tmp.szGoodsName[255] = 0;
				switch(ChatsGoodsInfo_tmp.ChatsGoodsInfo.lInfoType)
				{
				case eCGIT_Player:
					{
						CPlayer *pOwner = GetGame()->FindPlayer(ChatsGoodsInfo_tmp.ChatsGoodsInfo.OwnerGuid);
						if (NULL == pOwner)
						{
							pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 61), 0xffff0000, eNOTIFYPOS_CENTER);
							return;
						}
						CGoods *pGoods = pOwner->FindGoodByGuid(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						if (NULL == pGoods)
						{
							pPlayer -> SendNotifyMessage( CStringReading::LoadString(209, 62), 0xffff0000, eNOTIFYPOS_CENTER);
							return;
						}
						CGUID::CreateGUID(ChatsGoodsInfo_tmp.ChatsGoodsInfo.KeyGuid);
						pGoods->SerializeForOldClient(&ChatsGoodsInfo_tmp.vGoodsData);
					}
					break;
				case eCGIT_Copy:
					break;
				case eCGIT_Npc:
				default:
					return;
				}

				vChatsGoodsInfo_tmp.push_back(ChatsGoodsInfo_tmp);	
			}


			//!--------------------------------------------------------------------------------------
			char szContent[1024];
			pMsg->GetStr(szContent,1024);

			// 查询特殊道具
			if (CGlobeSetup::GetSetup()->nTalkCountryGoodsNum)
			{
				// 成功
				if( !pPlayer->FindGoods(CGlobeSetup::GetSetup()->szTalkCountryGoodsName, CGlobeSetup::GetSetup()->nTalkCountryGoodsNum))
				{
					char szTmp[512] = {0};
					CGoodsBaseProperties *pBaseProperties = CGoodsFactory::QueryGoodsBasePropertiesByOriginalName(CGlobeSetup::GetSetup()->szTalkCountryGoodsName);
					if(NULL != pBaseProperties)
					{
						_snprintf(szTmp, 512, CStringReading::LoadString(209, 64), pBaseProperties->GetName());
						pPlayer -> SendNotifyMessage(szTmp, 0xffff0000, eNOTIFYPOS_CENTER);
					}
					return;
				}
			}

			// 查询钱
			if (pPlayer->GetMoney() < CGlobeSetup::GetSetup()->lTalkCountryGold)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}
			// 查询银币
			if (pPlayer->GetSilver() < CGlobeSetup::GetSetup()->lTalkCountrySilver)
			{
				pPlayer -> SendNotifyMessage(CStringReading::LoadString(209, 65), 0xffff0000, eNOTIFYPOS_CENTER);
				return;
			}

			if (CGlobeSetup::GetSetup()->nTalkCountryGoodsNum)
				pPlayer->DelGoods(CGlobeSetup::GetSetup()->szTalkCountryGoodsName, CGlobeSetup::GetSetup()->nTalkCountryGoodsNum);

			if (CGlobeSetup::GetSetup()->lTalkCountryGold)
				pPlayer -> SetMoney( pPlayer->GetMoney() - CGlobeSetup::GetSetup()->lTalkCountryGold,22);

			if (CGlobeSetup::GetSetup()->lTalkCountrySilver)
				pPlayer -> SetSilver( pPlayer->GetSilver() - CGlobeSetup::GetSetup()->lTalkCountrySilver);

			CMessage msg(MSG_S2W_OTHER_TALK_COUNTRY);
			msg.Add((LONG)vChatsGoodsInfo_tmp.size());
			for (LONG i = 0; i < vChatsGoodsInfo_tmp.size(); ++i)
			{
				msg.AddEx(&(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo), sizeof(tagChatsGoodsInfo));
				msg.Add(vChatsGoodsInfo_tmp[i].szGoodsName);
			}
			msg.Add(BYTE(1));
			msg.Add((char*)pPlayer->GetName());
			msg.Add(szContent);
			msg.Add(pPlayer->GetCountry());
			msg.Send();

			GetGameLogInterface()->logT430_chat_log(pPlayer,"",szContent,CHAT_COUNTRY);

			//!--------------------------------------------------------------------------------------
			//! 保存聊天中的物品信息
			if (0 < vChatsGoodsInfo_tmp.size())
			{
				vector<tagChatsGoodsInfo_tmp> ;

				CMessage msgGoodsInfo(MSG_S2W_OTHER_GoodsInfo_Create);

				LONG lNewInfoNum = 0;
				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
						++lNewInfoNum;
				}
				msgGoodsInfo.Add(lNewInfoNum);

				for (LONG i = 0 ; i < vChatsGoodsInfo_tmp.size(); ++i)
				{
					if (eCGIT_Player == vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.lInfoType)
					{
						CGoodsFactory::AddGoodsInfo(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid, vChatsGoodsInfo_tmp[i].vGoodsData);
						msgGoodsInfo.Add(vChatsGoodsInfo_tmp[i].ChatsGoodsInfo.KeyGuid);
						msgGoodsInfo.Add((LONG)vChatsGoodsInfo_tmp[i].vGoodsData.size());
						msgGoodsInfo.AddEx(&(vChatsGoodsInfo_tmp[i].vGoodsData[0]), vChatsGoodsInfo_tmp[i].vGoodsData.size());
					}
				}

				msgGoodsInfo.Send();
			}
			//!--------------------------------------------------------------------------------------

		}
		break;
		//处理ws接收过来的世界喊话
	case MSG_W2S_OTHER_TALK_WORLD:
		{
			pMsg->SetType(MSG_S2C_OTHER_TALK_WORLD);
			pMsg->SendAll();
		}
		break;
	//处理ws发过来的置顶世界喊话
	case MSG_W2S_OTHER_TALK_TOP_WORLD:
		{
			pMsg->SetType(MSG_S2C_OTHER_TALK_TOP_WORLD);
			pMsg->SendAll();
		}
		break;
	//世界新闻
	case MSG_W2S_OTHER_NEWS_TOP_WORLD:
		{
			pMsg->SetType(MSG_S2C_OTHER_NEWS_TOP_WORLD);
			pMsg->SendAll();
		}
		break;
	//场景新闻
	case MSG_W2S_OTHER_NEWS_TOP_REGION:
		{
			char strText[1024];
			long lRegionID = pMsg->GetLong();
			long lRgnType = pMsg->GetLong();
			long lNum = pMsg->GetLong();
			pMsg->GetStr(strText,1024);
			long lFileNum = pMsg->GetLong();
			CServerRegion* pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID((eRgnType)lRgnType, lRegionID);

			if( pRegion )
			{
				//本场景在本服务器
				CMessage msg( MSG_S2C_OTHER_NEWS_TOP_WORLD );
				msg.Add(lNum);
				msg.Add((char*)strText);
				msg.Add(lFileNum);

				BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pRegion, TYPE_PLAYER)
				{
					msg.SendToPlayer( pShape -> GetExID() );
				}
				END_LOOP_REGION_SHAPE_BY_TYPE
			}
		}
		break;
		//处理ws接收过来的国家喊话
	case MSG_W2S_OTHER_TALK_COUNTRY:
		{
			// 只发送给自己国家里的人
			BYTE bIsSuccess;
			BYTE btCountry;
			char szName[32];
			char szContent[1024];

			//! -----------------跳过物品信息部分-----------------
			LONG lGoodsNum = pMsg->GetLong();
			tagChatsGoodsInfo tmpChatsGoodsInfo;
			char szGoodsName[256] = {0};
			for (LONG i = 0 ; i < lGoodsNum; ++i)
			{
				pMsg->GetEx(&tmpChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
				pMsg->GetStr(szGoodsName, 255);
			}
			//! ----------------------------------

			
			bIsSuccess = pMsg->GetByte();
			pMsg->GetStr(szName, 32);
			pMsg->GetStr(szContent, 1024);
			btCountry = pMsg->GetByte();
			pMsg->SetType(MSG_S2C_OTHER_TALK_COUNTRY);

			GetGame()->SendMsgToSameCountryPlayer(pMsg,btCountry);
		}
		break;
	case MSG_W2S_OTHER_TALK_COUNTRY_UNION:
		{
			// 只发送给自己盟国的人
			BYTE bIsSuccess;
			BYTE btCountry;
			char szName[32];
			char szContent[1024];

			//! -----------------跳过物品信息部分-----------------
			LONG lGoodsNum = pMsg->GetLong();
			tagChatsGoodsInfo tmpChatsGoodsInfo;
			char szGoodsName[256] = {0};
			for (LONG i = 0 ; i < lGoodsNum; ++i)
			{
				pMsg->GetEx(&tmpChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
				pMsg->GetStr(szGoodsName, 255);
			}
			//! ----------------------------------


			bIsSuccess = pMsg->GetByte();
			pMsg->GetStr(szName, 32);
			pMsg->GetStr(szContent, 1024);
			btCountry = pMsg->GetByte();
			pMsg->SetType(MSG_S2C_OTHER_TALK_COUNTRY_UNION);

			GetGame()->SendMsgToSameCampPlayer(pMsg,btCountry);
		}
		break;

	case MSG_W2S_OTHER_TALK_NOTIFY:
		{
			// 只发送给自己国家里的人
			BYTE btType;
			BYTE btCountry;
			char szContent[1024];
			
			btType = pMsg->GetByte();
			btCountry = pMsg->GetByte();
			pMsg->GetStr(szContent,1024);

			CMessage msg(MSG_S2C_OTHER_TALK_NOTIFY);
			msg.Add(btType);
			msg.Add(szContent);

			GetGame()->SendMsgToSameCountryPlayer(&msg,btCountry);
		}
		break;

	case MSG_C2S_OTHER_QUESTGOD:
		{
			BYTE b = pMsg->GetByte();

			if (b == 0)
			{
				const char * strName = pMsg->GetPlayer()->GetName();
				if (CGMList::GetGMInfoMap()->find(strName) != CGMList::GetGMInfoMap()->end())
				{
					CGMList::GetGMInfoMap()->erase(strName);
				}
			}
			else if (b == 1)
			{
				char str[1024];
				pMsg->GetStr(str, 1024);

				char strGod[1024];
				SYSTEMTIME stTime;
				GetLocalTime(&stTime);
				_snprintf(strGod, 1024, "sdf!@#$aurora-348sklhw9lsdhf!@Dfsdf89s*LKHL@#$@#;sldjkfnv/z[q%04d%02d%02d%02d%s", 
					stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, CGMList::s_strGodPassport.c_str());

				if (strcmp(str, strGod) == 0)
				{
					tagGMInfo st;
					st.lLevel=GM_GOD;
					st.strName = pMsg->GetPlayer()->GetName();
					CGMList::GetGMInfoMap()->insert( map<string, tagGMInfo>::value_type (st.strName, st));
				}
			}
		}
		break;
	case MSG_S2C_OTHER_HITHELPHINT:
		{
			long lHintID = pMsg->GetLong();
			long lFlag = pMsg->GetLong();
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(pPlayer)
				pPlayer->OnPlayerHitHelpHint(lHintID,lFlag);
		}
		break;
	case MSG_S2C_OTHER_HITINTERFACEHINT:
		{
			long lHintID = pMsg->GetLong();
			long lFlag = pMsg->GetLong();
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(pPlayer)
				pPlayer->OnPlayerHitInterfaceHint(lHintID,lFlag);
		}
		break;
	case MSG_C2S_OTHER_UPDATEVARVALUE_QUEST:
		{
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			itScript it = g_Scripts.find(scriptID);
			if(it != g_Scripts.end())
			{
				char szValueName[128];
				pMsg->GetStr(szValueName, sizeof(szValueName));
				if(szValueName)
				{
					// 记录变量名的检查
					if(it->second && it->second->GetClientUpdateVarName()
						&& strcmp(it->second->GetClientUpdateVarName(), szValueName) == 0)
					{
						if(szValueName[0] == '$')// 1:整型($)
						{
							long Value = pMsg->GetLong();
							CPlayer *pPlayer = pMsg->GetPlayer();
							if(pPlayer)
							{
								pPlayer->SetScriptVarValue(it->second, szValueName, 0, Value);
							}
						}
						else if(szValueName[0] == '#')// 2:字符串型(#)
						{
							char szValue[128];
							pMsg->GetStr(szValue, sizeof(szValue));
							CPlayer *pPlayer = pMsg->GetPlayer();
							if(pPlayer)
							{
								pPlayer->SetScriptVarValue(it->second, szValueName, szValue);
							}
						}

						// 记录要修改的变量名
						it->second->SetClientUpdateVarName("");

						// 取消挂起的脚本
						ContinueRunScript(scriptID, NULL);
					}
				}
			}
		}
		break;
	case MSG_C2S_OTHER_QUITTOSELECT:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(pPlayer)
			{
				CMessage msg(MSG_S2C_OTHER_QUITTOSELECTANSEWR);
				DWORD dwServerKey[SERVER_KEY_DWORD_SIZE];
				WORD *pWord = (WORD*)dwServerKey;
				for (LONG i = 0; i < SERVER_KEY_DWORD_SIZE; ++i)
				{
					pWord[i * 2]		= rand();
					pWord[i * 2 + 1]	= rand();
					pPlayer->SetServerKey(i, dwServerKey[i]);
					msg.Add(dwServerKey[i]);
				}	
				msg.SendToPlayer(pPlayer->GetExID(), false);
			}
		}
		break;
	case MSG_C2S_OTHER_PING:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(pPlayer)
			{
				DWORD dwCrc32Client = pMsg->GetDWord();
				DWORD dwCrc32 = 0;
				CGUID regionID = pPlayer->GetRegionGUID();
				CCrc32Static::DataCrc32(&regionID,sizeof(CGUID),dwCrc32);
				DWORD dwValue = pPlayer->GetHotKey(0);
				CCrc32Static::DataCrc32(&dwValue,sizeof(DWORD),dwCrc32);
				dwValue = pPlayer->GetHotKey(12);
				CCrc32Static::DataCrc32(&dwValue,sizeof(DWORD),dwCrc32);
				if(dwCrc32 == dwCrc32Client)
				{
					pPlayer->SetLastPingTime(timeGetTime());
				}
			}
		}
		break;
	case MSG_C2S_OTHER_AccusePlayer_Request:
		{
			CPlayer* pPlayer=pMsg->GetPlayer();
			if (pPlayer==NULL)
				break;
			BYTE bSucc=0;
			DWORD dwTime=time(NULL);
			if(dwTime - pPlayer->GetReportTime() >
				CGlobeSetup::GetSetup()->dwReportTime)
			{
				pPlayer->SetReportTime(dwTime);
				pMsg->SetType(MSG_S2W_OTHER_AccusePlayer_Request);
				pMsg->Send();
				bSucc=1;
			}
			CMessage msg(MSG_S2C_OTHER_AccusePlayer_Notify);
			msg.Add(bSucc);
			msg.SendToPlayer(pPlayer->GetExID());	
		}
		break;
	case MSG_W2S_OTHER_WATCH:
		{
			DWORD bDel=pMsg->GetDWord();
			char szName[32]={0};
			pMsg->GetStr(szName,32);
			//修改SM监视列表
			CSMClient::GetSM()->ChangeWatchedPlayer(szName,bDel);
		}
		break;
	case MSG_W2S_OTHER_GM_CHAT:
		{
			//发送到客户端
			CGUID guid;
			pMsg->GetGUID(guid);
			char szText[1024]={};
			pMsg->GetStr(szText,1024);
			CMessage msg(MSG_S2C_OTHER_GM_CHAT);
			msg.Add((BYTE)TRUE);
			msg.Add(szText);
			msg.SendToPlayer(guid);
			
			
		}
		break;
	case MSG_C2S_OTHER_Replay_GM:
		{
			if(CSMClient::GetSM()->FindWatchedPlayer(pMsg->GetPlayer()->GetName()))
			{
				//发送到WS
				pMsg->SetType(MSG_S2W_OTHER_REPLY_GM);
				pMsg->Send();
			}
			else
			{
				CMessage msg(MSG_S2C_OTHER_GM_CHAT);
				msg.Add((BYTE)FALSE);
				msg.SendToSocket(pMsg->GetSocketID());
			}
		}
		break;
	case MSG_C2S_OTHER_COSMETOLOGY:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			tagCosKey tgKey;
			tgKey.bType = pMsg->GetByte();
			tgKey.bSex = pMsg->GetByte();
			tgKey.lID = pMsg->GetLong();
			long lHairColor = pMsg->GetLong();

			if (pPlayer)
			{
				long lRet = pPlayer->ChangeFaceHair(tgKey,lHairColor);
				if (lRet<0)
				{
					CMessage msg(MSG_S2C_PLAYER_COSMETOLOGY_SUCESS);
					msg.SendToSocket(pMsg->GetSocketID());
				}
				else
				{
					CMessage msg(MSG_S2C_PLAYER_COSMETOLOGY_FAILED);
					msg.Add((BYTE)lRet);
					msg.SendToSocket(pMsg->GetSocketID());
				}
			}
		}
		break;
	case MSG_C2S_OTHER_Replay_SETTIMER:
		{
			long timerID = pMsg->GetLong();
			CPlayer* player = pMsg->GetPlayer();
			if( !player ) return;

			CPlayer::tagCountTimer& timer = player->GetCountTimer(timerID);
			if( timer.m_dwID==-1 ) return;

			// 如果点击速度过快则踢玩家
			if (player->JudgeNpcScriptslegality())
			{
				AddLogText("玩家：%s 点击定时器过快！", player->GetAccount());
				return;	
			}

			const tagCountTimerParam& param = CCountTimerList::GetCountTimerParam(timer.m_dwTimerType);
			if( param.m_dwType==-1 ) return;

			if( param.m_strScriptPath=="" ) 
			{
				time_t curTime;
				time(&curTime);
				if( timer.m_dwRetTime<=curTime-timer.m_dwCountTime ) // 定时器计时到点
				{
					player->DelCountTimer(timer.m_dwID);
					CMessage msg(MSG_S2C_OTHER_CLOSETIMER);
					msg.Add((DWORD)timer.m_dwID);
					msg.SendToPlayer(player->GetExID());
					return;
				}
				else
				{
					player->SendNotifyMessage("时间未到，请稍候点击。", 0xffff0000, eNOTIFYPOS_CENTER);
				}
			}


			stRunScript st;
			st.pszFileName   = NULL;
			st.srcShape  = player;
			st.desShape	 = NULL;
			st.pRegion   = pMsg->GetRegion();
#ifdef _RUNSTACKINFO_
			char pszStatckInfo[10240]="";
			_snprintf(pszStatckInfo,10240,"RUN_SCRIPT(%s) Start",param.m_strScriptPath.c_str());
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
			RunScript(&st, (char*)GetGame()->GetScriptFileData(param.m_strScriptPath.c_str()));	
		}
		break;
	}
}


void	DisposalChatsGoods(CMessage *pMsg)
{
	//! 处理聊天中夹带的物品信息]
	LONG lGoodsNum = pMsg->GetLong();
	for (LONG i = 0; i < lGoodsNum; i++)
	{
		tagChatsGoodsInfo ChatsGoodsInfo;
		pMsg->GetEx(&ChatsGoodsInfo, sizeof(tagChatsGoodsInfo));
		char szGoodsName[256] = {0};
		pMsg->GetStr(szGoodsName, 255);
	}
}

bool LogicLogInterface::logT430_chat_log(CPlayer* pPlayer,const char* pAccepterName,const char* pChatText,long lType)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;

	if(NULL == m_pLogClient)
		return false;
	if (NULL == pPlayer || pAccepterName == NULL || pChatText == NULL) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		430, strTime.c_str(),
		&(pPlayer->GetExID()),	pPlayer->GetName(),
		pAccepterName, pChatText,pPlayer->GetRegionID(),lType
		);
}

	
bool LogicLogInterface::logT431_GM_log(CPlayer* pPlayer,const char* pCommandLine)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;

	if(NULL == m_pLogClient)
		return false;
	if (NULL == pPlayer ||  pCommandLine == NULL) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		431, strTime.c_str(),
		&(pPlayer->GetExID()),	pPlayer->GetName(),pPlayer->GetRegionID(),
		pCommandLine
		);
}