#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\nets\Servers.h"
#include "..\..\worldserver\game.h"
#include "..\player.h"
#include "../goods/CGoods.h"
#include "..\..\appworld\session\csessionfactory.h"
#include "..\..\appworld\session\csession.h"
#include "..\..\appworld\Organizingsystem\OrganizingCtrl.h"
#include "../../public/wordsfilter.h"
#include "../../Server/setup/GlobeSetup.h"
#include "../../server/setup/LogSystem.h"
#include "..\nets\Servers.h"
#include "..\..\appworld\Country\CountryHandler.h"
#include "..\..\appworld\PlayerMan\PlayerMan.h"
#include "../DBEntity/Account.h"
#include "..\dbentity/entityManager.h"
#include "../DBEntity/EntityGroup.h"

#include "../GlobalRgnManager.h"
#include "../../AppWorld/Linkman/LinkmanSystem.h"
#include "../public/strtype.h"
#include "../public/StrUnifyInput.h"
#include "../business/CBusinessManager.h"

#include "..\..\appworld\SMclient.h"



// 响应LOG消息
void OnLogMessage(CMessage* pMsg)
{
	DWORD curTime = timeGetTime();
	char pszLogingInfo[512]="";
	switch(pMsg->GetType())
	{
	case MSG_L2W_LOG_QUEST_PLAYERBASELIST://loginserver请求一个账号下的玩家基本信息列表
		{
			char szCdkey[ACCOUNT_SIZE];
			pMsg->GetStr(szCdkey,ACCOUNT_SIZE);
			//去掉字符串右边的空格
			TrimRight(szCdkey);

#ifdef _RUNSTACKINFO1_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszLogingInfo);
#endif

			// 判断DBS是否准备好了
			if(	!GetGame()->GetInitVarLoadFlag() 
				|| !GetGame()->GetInitRegionLoadFlag()
				|| !GetGame()->GetInitFactionLoadFlag()
				|| !GetGame()->GetInitUnionLoadFlag()
				|| !GetGame()->GetDbIsReady()
				|| !GetGame()->GetInitPersonalHouseLoadFlag()
				|| ( GetInst( Business::CBusinessManager ).IsEnable() &&
				!GetInst( Business::CBusinessManager ).IsDBLoaded() ) )
			{
				AddLogText("玩家[%s]登录时DBS未全部准备完成!",szCdkey);
				CMessage pMsga(MSG_W2L_LOG_ANSWER_PLAYERBASELIST);
				// 添加验证代码
				pMsga.Add((long)0xFFFFFFFF);
				pMsga.Add((BYTE)2);// flag
				pMsga.Add((szCdkey));// cdkey
				pMsga.Add((short)0);// flag
				pMsga.SendToSocket(GetGame()->GetLSSocketID());
				return;
			}

			// 判断GS是否准备好了
			map<DWORD, CGame::tagGameServer>::iterator gsItr = GetGame()->GetGameServerList()->begin();
			for(; gsItr != GetGame()->GetGameServerList()->end(); gsItr++)
			{
				if(!gsItr->second.bConnected)
				{
					CMessage pMsga(MSG_W2L_LOG_ANSWER_PLAYERBASELIST);
					// 添加验证代码
					pMsga.Add((long)0xFFFFFFFF);
					pMsga.Add((BYTE)2);// flag
					pMsga.Add((szCdkey));// cdkey
					pMsga.Add((short)0);// flag
					pMsga.SendToSocket(GetGame()->GetLSSocketID());
					return;
				}
			}
			
			Account* acc = GetGame()->GetEntityManager()->FindAccount(szCdkey);
			// 现在LoginPlayer对象缓冲区查找
			if(acc)
			{
				acc->ClearAllLoginPlayer();
				acc->SetName(szCdkey);
				GetGame()->GetEntityManager()->CreateLoadAccountSession(acc);
			}
			else // 未找到缓冲区中的对象
			{
				// 创建session
				if(szCdkey[0] != '\0')
				{
					acc = MP_NEW Account;
					if(acc)
					{
						CGUID accGuid;
						CGUID::CreateGUID(accGuid);
						acc->SetGUID(accGuid);
						acc->SetName(szCdkey);
					}
					GetGame()->GetEntityManager()->CreateLoadAccountSession(acc);
					MP_DELETE(acc);
				}
			}
		}
		break;

	case MSG_L2W_LOG_QUEST_PLAYERDATA://loginserver请求玩家的详细信息
		{
			// 取得验证码
			long lTestSignCode = pMsg->GetLong();

			char szCdkey[ACCOUNT_SIZE];
			CGUID guid;
			pMsg->GetGUID(guid);//获取PLAYERID
			pMsg->GetStr(szCdkey,ACCOUNT_SIZE);//获取CDKEY

			// 检查验证码
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
				char szGUID[128];
				guid.tostring(szGUID);
				_snprintf(pszLogingInfo,512,"MSG_L2W_LOG_QUEST_PLAYERDATA! Account[%s] Is Null!", szGUID, szCdkey);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);
				msg.Add((long)0xFFFFFFFF);
				msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());
				return;
			}		
			
			if(tLoginInfo->GetSignCode() != lTestSignCode)
			{
				char szGUID[128];
				guid.tostring(szGUID);
				_snprintf(pszLogingInfo,512,"MSG_W2L_LOG_ANSWER_PLAYERDATA 玩家ID[%s],Cdkey[%s] 验证码错误!", szGUID, szCdkey);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				
				// 现在CLoginPlayer中找,如果没有对象或者状态为OFFLINE表示非法登录
				tLoginInfo->SetPlayerState(PLAYER_STATE_VOID);
				CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);
				long tmpSignCode = 0;
				GetGame()->GenerateSignCodeByTime(tmpSignCode);
				tLoginInfo->SetSignCode(tmpSignCode);
				msg.Add((long)tLoginInfo->GetSignCode());
				msg.Add((char)LOGIN_QUEST_PLAYERDATA_SIGNCODE_ERROR);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());
				return;
			}	
			// 现在CLoginPlayer中找,如果没有对象或者状态为OFFLINE表示非法登录
			CEntityGroup* lPlayer = (CEntityGroup*)tLoginInfo->FindLoginPlayer(guid);
			// 找到CDBLoginPlayer对象
			if( lPlayer && (tLoginInfo->GetPlayerState() == PLAYER_STATE_LOADED_BASE_DATA) )
			{	
				if(GetGame()->GetEntityManager()->ComputeLoginPlayerDelTime(lPlayer) != -1)
				{
					char szGUID[128];
					guid.tostring(szGUID);
					_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_QEUSTPROPERTYCHARDELED), szCdkey);
					PutStringToFile("Login_WS_Info",pszLogingInfo);

					CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);	
					msg.Add((long)-1);
					msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
					msg.Add(szCdkey);
					msg.SendToSocket(GetGame()->GetLSSocketID());

					tLoginInfo->SetPlayerState(PLAYER_STATE_VOID);
					return;
				}
				// 需要到数据库读取
				GetGame()->GetEntityManager()->CreateLoadPlayerDataSession(szCdkey, guid);
				GetGame()->GetEntityManager()->CreateInitLoadMailSession(guid);
			}
			else
			{
				CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);
				msg.Add((long)-1);
				msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());

				tLoginInfo->SetPlayerState(PLAYER_STATE_VOID);
			}
		}
		break;

	case MSG_S2W_LOG_QUEST_PLAYERDATA://gameserver请求某个玩家的详细属性
		{
			long lTestSignCode = pMsg->GetLong();

			CGUID guid;
			pMsg->GetGUID(guid);
			long lSocketIDC2S = pMsg->GetLong();

			char szCdkey[128];
			pMsg->GetStr(szCdkey, sizeof(szCdkey));
			LONG lIP = pMsg->GetLong();
			
			// 检查验证码
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(guid);
				if(pPlayer)
				{
					GetGame()->OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
				}
				_snprintf(pszLogingInfo,512, "MSG_S2W_LOG_QUEST_PLAYERDATA! Account[%s] Is Null!", szCdkey);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				CMessage msg(MSG_W2S_LOG_ANSWER_PLAYERDATA);
				msg.Add((long)0xFFFFFFFF);
				msg.Add((long)-1);
				msg.Add(guid);
				msg.Add(lSocketIDC2S);
				msg.SendToSocket(pMsg->GetSocketID());   //send to gameserver
				return;
			}	
			
			CEntityGroup* lPlayer = (CEntityGroup*)tLoginInfo->FindLoginPlayer(guid);
			if(!lPlayer)
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(guid);
				if(pPlayer)
				{
					GetGame()->OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
				}
				tLoginInfo->SetPlayerState(PLAYER_STATE_VOID);
				CMessage msg(MSG_W2S_LOG_ANSWER_PLAYERDATA);
				msg.Add((long)0xFFFFFFFF);
				msg.Add((long)-1);
				msg.Add(guid);
				msg.Add(lSocketIDC2S);
				msg.SendToSocket(pMsg->GetSocketID());   //send to gameserver
				return;
			}

			if(tLoginInfo->GetSignCode() != lTestSignCode)
			{
				char szGUID[128];
				guid.tostring(szGUID);
				_snprintf(pszLogingInfo,512, "MSG_S2W_LOG_QUEST_PLAYERDATA 玩家ID[%s],Cdkey[%s] 验证码错误!", szGUID, lPlayer->GetStringAttr(string("Account")));
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				CMessage msg(MSG_W2S_LOG_ANSWER_PLAYERDATA);
				msg.Add(lTestSignCode);
				msg.Add((long)-3);
				msg.Add(guid);
				msg.Add(lSocketIDC2S);
				msg.SendToSocket(pMsg->GetSocketID());   //send to gameserver
				return;
			}
			
			if( tLoginInfo->GetPlayerState() == PLAYER_STATE_LOADED_CHAR_DATA// 玩家已登录服务器并读取详细数据
				|| tLoginInfo->GetPlayerState() == PLAYER_STATE_CHANGING_SERVER
				|| tLoginInfo->GetPlayerState() == PLAYER_STATE_ENTERING_GAME )//正在切换服务器
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(guid);
				if(pPlayer)
				{
					long lPlayerState = tLoginInfo->GetPlayerState();
					if(GetGame()->OnPlayerEnter(pPlayer, tLoginInfo, lTestSignCode, lIP, pMsg->GetGSID(), lSocketIDC2S, pMsg->GetSocketID()))
					{
						CMessage msg(MSG_W2S_LOG_ANSWER_PLAYERDATA);
						msg.Add(lTestSignCode);
						msg.Add(1L);//添加标记
						msg.Add(guid);
						msg.Add(lSocketIDC2S);
						DBWriteSet setWriteDB;
						msg.GetDBWriteSet(setWriteDB);
						pPlayer->CodeToDataBlock(setWriteDB);
						msg.SendToSocket(pMsg->GetSocketID());   //send to gameserver

						GetGame()->OnPlayerEnterLateCode(pPlayer, tLoginInfo);
						tLoginInfo->SetPlayerState(PLAYER_STATE_ENTERED_GAME);	
						tLoginInfo->SetLoginTimeoutValue(-1);
					}

					if(lPlayerState == PLAYER_STATE_CHANGING_SERVER)
					{	
						//! SM监视的玩家的处理
						if(pPlayer->GetExID() == CSMClient::GetSM()->GetWatchedGuid())
						{
							//发送到该玩家所在的GS
							CMessage msgLogout(MSG_W2S_OTHER_WATCH);
							msgLogout.Add((DWORD)FALSE);
							msgLogout.Add(pPlayer->GetName());
							msgLogout.SendToSocket(CSMClient::GetSM()->GetWatchedGsSocket());

							CSMClient::GetSM()->SetWatchedPlayer(pPlayer->GetExID(), pPlayer->GetName(), pMsg->GetSocketID());

							CMessage msgLogin(MSG_W2S_OTHER_WATCH);
							msgLogin.Add((DWORD)TRUE);
							msgLogin.Add(pPlayer->GetName());
							msgLogin.SendToSocket(pMsg->GetSocketID());
						}

						pPlayer->SetGsSocket(pMsg->GetSocketID());
						LinkmanSystem::GetInstance().OnPlayerChangeGs(pPlayer);
					}
				}
			}
			else
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(guid);
				if(pPlayer)
				{
					CMessage msg(MSG_W2S_OTHER_KICKPLAYER);
					msg.Add((BYTE)AT_ONCE_KICK);
					msg.Add(pPlayer->GetExID());
					msg.SendToSocket(pPlayer->GetGsSocket());
				}
				tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
				CMessage msg(MSG_W2S_LOG_ANSWER_PLAYERDATA);
				msg.Add((long)0xFFFFFFFF);
				msg.Add((long)-1);
				msg.Add(guid);
				msg.Add(lSocketIDC2S);
				msg.SendToSocket(pMsg->GetSocketID());   //send to gameserver
			}
		}
		break;

	//gameserver通知worldserver一个玩家已退出游戏, 转发DBS存盘消息
	case MSG_S2W_LOG_PLAYERQUIT:
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			long lFlag = pMsg->GetLong();
			DWORD dwServerKey[SERVER_KEY_DWORD_SIZE];
			for (LONG i = 0; i < SERVER_KEY_DWORD_SIZE; ++i)
			{
				dwServerKey[i] = pMsg->GetDWord();
			}

			char szGUID[128]="";
			PlayerGuid.tostring(szGUID);

			switch(lFlag)
			{
			case 1: // 正常退出
				{
					CPlayer* pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
					if(NULL == pPlayer)
					{
						pPlayer = GetGame()->NewRawPlayer(ePlayerQuitMsg);
						pPlayer->SetExID(PlayerGuid);
						time_t curTime;
						time(&curTime);
						pPlayer->SetLastExitGameTime((DWORD)curTime);
						// 更新WS上该玩家数据
						DBReadSet setReadDB;
						pMsg->GetDBReadSet(setReadDB);
						pPlayer->DecodeFromDataBlock(setReadDB);
						if(pPlayer)
						{
							if(GetGame()->GetDbIsReady())
							{
								CWorldServerSession pSession(5000);
								// 设置当前账号状态
								Account* pAcc = NULL;
								CEntityGroup* tDBLoginPlayer = NULL;
								if(pPlayer->GetAccount())
									pAcc = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());
								if(pAcc)
								{
									if(pAcc->GetPlayerState() != PLAYER_STATE_VOID)
										pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
									pAcc->SetCurGamedPlayerGuid(NULL_GUID);
									tDBLoginPlayer = pAcc->FindLoginPlayer(pPlayer->GetExID());
									pSession.NakeSendSaveLoginPlayerDataMsg(pPlayer, tDBLoginPlayer);
								}
								else
								{
									tDBLoginPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[loginplayer]"), pPlayer->GetExID());
									pSession.NakeSendSaveLoginPlayerDataMsg(pPlayer, tDBLoginPlayer);
									GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBLoginPlayer);
								}

								CEntityGroup* tDBPlayer = (CEntityGroup*)GetGame()->GetEntityManager()->NewBaseEntity(string("[player]"), pPlayer->GetExID());
								pSession.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, pPlayer, tDBPlayer, true);
								GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBPlayer);
								GetGame()->DelRawPlayer(pPlayer);
							}
							else
								GetGame()->PushSavePlayer(pPlayer);
						}
						return;
					}
					else
					{
						// 更新WS上该玩家数据
						DBReadSet setReadDB;
						pMsg->GetDBReadSet(setReadDB);
						pPlayer->DecodeFromDataBlock(setReadDB);
						pPlayer->SetPlayerLogin(false);
						pPlayer->SetFirstLogin(false);
						time_t curTime;
						time(&curTime);
						pPlayer->SetLastExitGameTime((DWORD)curTime);
						if(GetGame()->GetDbIsReady())
						{
							const char* szCdkey = pPlayer->GetAccount();
							Account* pAcc = NULL;
							if(szCdkey)
							{
								pAcc = GetGame()->GetEntityManager()->FindAccount(szCdkey);
								if(pAcc)
								{
									if(pAcc->GetPlayerState() != PLAYER_STATE_VOID)
										pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
									pAcc->SetCurGamedPlayerGuid(NULL_GUID);
									pAcc->SetLoginTimeoutValue(timeGetTime());
								}
							}
							GetGame()->GetEntityManager()->CreateSavePlayerDataSession(PlayerGuid, SAVE_DETAIL_ALL);
							GetGame()->GetEntityManager()->CreateSaveLoginPlayerDataSession(PlayerGuid);	
						}
					}

					//! SM监视的玩家的处理
					if(pPlayer->GetExID() == CSMClient::GetSM()->GetWatchedGuid())
					{
						CMessage Replymsg(MSG_GAPP2SM_WATCH_INFO_Reply);
						Replymsg.Add(GetGame()->GetSetup()->dwGappID);
						Replymsg.Add((BYTE)0);
						Replymsg.SendToSM();

						CMessage msg(MSG_W2S_OTHER_WATCH);
						msg.Add((DWORD)0);
						msg.Add(pPlayer->GetName());
						msg.SendToSocket(CSMClient::GetSM()->GetWatchedGsSocket());

						CSMClient::GetSM()->ClearWatch();
					}

					GetGame()->OnPlayerQuit(pPlayer, pMsg->GetGSID(), dwServerKey);
				}
				break;
			default:
				{
					char szGuid[128];
					PlayerGuid.tostring(szGuid);
					_snprintf(pszLogingInfo,512,"玩家[%s]退出时异常,退出标志[%d],请检查代码逻辑!",szGuid, lFlag);
					PutStringToFile("Login_WS_Info",pszLogingInfo);

					CPlayer* pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
					if(pPlayer)
					{
						time_t curTime;
						time(&curTime);
						pPlayer->SetLastExitGameTime((DWORD)curTime);
						Account* pAcc = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());
						if(pAcc)
						{
							if(pAcc->GetPlayerState() != PLAYER_STATE_VOID)
								pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
							pAcc->SetCurGamedPlayerGuid(NULL_GUID);
							pAcc->SetLoginTimeoutValue(timeGetTime());
						}
						GetGame()->OnPlayerQuit(pPlayer, pMsg->GetGSID(), dwServerKey);
					}
				}
				break;
			}
		}
		break;

	case MSG_L2W_LOG_FCM_TIME:
		{
			char szCdkey[128];
			pMsg->GetStr(szCdkey, 1228);
			long lTime = pMsg->GetLong();
			if(szCdkey)
			{
				Account* pAcc = GetGame()->GetEntityManager()->FindAccount(szCdkey);
				if(pAcc)
				{
					CPlayer* pPlayer = GetGame()->GetMapPlayer(pAcc->GetCurGamedPlayerGuid());
					if(pPlayer)
					{
						CMessage msg(MSG_W2S_LOG_FCM_TIME);
						msg.Add(pPlayer->GetExID());
						msg.Add(lTime);
						msg.SendToSocket(pPlayer->GetGsSocket());
					}
				}
			}
		}
		break;
	case MSG_L2W_LOG_FCM_BEGIN:
		{
			char szCdkey[128];
			pMsg->GetStr(szCdkey, 1228);
			if(szCdkey)
			{
				Account* pAcc = GetGame()->GetEntityManager()->FindAccount(szCdkey);
				if(pAcc)
				{
					CPlayer* pPlayer = GetGame()->GetMapPlayer(pAcc->GetCurGamedPlayerGuid());
					if(pPlayer)
					{
						CMessage msg(MSG_W2S_OTHER_KICKPLAYER);
						msg.Add((BYTE)FCM_KICK_PLAYER);
						msg.Add(pPlayer->GetExID());
						msg.SendToSocket(pPlayer->GetGsSocket());
					}
				}
			}
		}
		break;

	case MSG_L2W_LOG_KICKPLAYER://根据CDKEY踢出一个玩家
		{
			char strCDKey[256];
			pMsg->GetStr(strCDKey,256);
			Account* pAcc = GetGame()->GetEntityManager()->FindAccount(strCDKey);
			if(pAcc 
				&& pAcc->GetPlayerState() != PLAYER_STATE_ENTERED_GAME)
			{
				pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
				pAcc->SetLoginTimeoutValue(timeGetTime());
			}
		}
		break;

	case MSG_L2W_LOG_REPEAT_LOGIN:
		{
			char strCDKey[256];
			pMsg -> GetStr( strCDKey, 256 );

			// 看该帐号是否有角色在游戏中
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(strCDKey);
			if(tLoginInfo)
			{
				if(tLoginInfo->GetEntityGroupMap().size())
				{
					CGUID CurGamedPlayerGuid = tLoginInfo->GetCurGamedPlayerGuid();
					if( CurGamedPlayerGuid != NULL_GUID)
					{
						CPlayer* pPlayer = GetGame()->GetMapPlayer(CurGamedPlayerGuid);
						if(pPlayer)
						{
							long gsid = GetGame()->GetGlobalRgnManager()->FindGSIDByRgnID(pPlayer->GetRegionExID());
							
							if(gsid != -1)
							{
							CMessage msg( MSG_W2S_LOG_REPEAT_LOGIN );
							msg.Add( CurGamedPlayerGuid );
							msg.SendGSID(gsid);
							}
							else
							{
								tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
								CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
								msg.Add(strCDKey);
								msg.Add(0L);
								msg.SendToSocket(GetGame()->GetLSSocketID());//send to loginserver
							}
							
							_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_REPEATLOGINTOGS), strCDKey);
							PutStringToFile("Login_WS_Info",pszLogingInfo);
							return;
						}
					}
				}
			}

			CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
			msg.Add(strCDKey);
			msg.Add(0L);
			msg.SendToSocket(GetGame()->GetLSSocketID());//send to loginserver
		}
		break;

	case MSG_L2W_LOG_DELETEROLE://loginserver请求删除一个角色
		{
			CGUID guid;
			DWORD dwIP;
			char szIP[64];
			char cdkey[ACCOUNT_SIZE];
			pMsg->GetStr(cdkey,ACCOUNT_SIZE);	//cdkey
			pMsg->GetGUID(guid);	//playerid
			dwIP = pMsg->GetDWord();
		
			char szGUID[128];
			guid.tostring(szGUID);
			CEntityGroup* lPlayer = NULL;
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(cdkey);
			if(!tLoginInfo)
			{
				_snprintf(pszLogingInfo,512,"DeleteRole::未先创建账号对象[%s]!", cdkey);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Send();//send to loginserver
				return;
			}
			lPlayer = tLoginInfo->FindLoginPlayer(guid);
			if(NULL == lPlayer)
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Send();//send to loginserver
				_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_DELCHARNOCHAROBJ), cdkey, szGUID);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				return;
			}
			if( tLoginInfo->GetPlayerState() != PLAYER_STATE_LOADED_BASE_DATA )
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Send();//send to loginserver
		
				_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_DELCHARINVALID), 
					tLoginInfo->GetPlayerState(), cdkey, szGUID);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				return;
			}

			// 判断人物角色等级是否满足删除条件
			if ((DWORD)lPlayer->GetLongAttr(string("Levels"))<CGlobeSetup::GetSetup()->dwDelLevelCondition)
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Add((long)4);
				msg.Send();//send to loginserver
				return;
			}
			
			sprintf(szIP, "%d.%d.%d.%d", LOBYTE(LOWORD(dwIP)), HIBYTE(LOWORD(dwIP)), LOBYTE(HIWORD(dwIP)), HIBYTE(HIWORD(dwIP)));

			time_t tDelDate;		// 删除日期
			time(&tDelDate);		// 当前日期的DWORD

			//! 联系人系统
			LinkmanSystem::GetInstance().OnPlayerDelStateChange(guid, TRUE);

			//删除玩家所在的帮会组织
			int nResult = GetOrganiCtrl()->OnDeleteRole(guid);
			//拥有主城 
			if( nResult == 1 )
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Add((long)1);
				msg.Send();//send to loginserver

				return;
			}
			//退出同盟
			else if( nResult == 2 )
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Add((long)2);
				msg.Send();//send to loginserver
				return;
			}
			//解散同盟
			else if(nResult == 3)
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Add((long)3);
				msg.Send();//send to loginserver
				return;
			}

			// 以下数据加入MSG发送给DBS
			GetGame()->GetEntityManager()->CreateUpdateLoginPlayerDelTimeSession(cdkey, guid);

			char szGuid[40] = {0};
			guid.tostring(szGuid);			
			_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_DELCHAROK), szGuid);
			PutStringToFile("Login_WS_Info",pszLogingInfo);
			return;
		}
		break;

	case MSG_L2W_LOG_RESTOREROLE:
		{
			CGUID guid;
			char cdkey[ACCOUNT_SIZE];
			pMsg->GetStr(cdkey,ACCOUNT_SIZE);	//cdkey
			pMsg->GetGUID(guid);	//playerid

			char szGUID[128];
			guid.tostring(szGUID);
			CEntityGroup* lPlayer = NULL;//GetGame()->GetEntityManager()->FindLoginPlayer(guid);
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(cdkey);
			if(!tLoginInfo)
			{
				_snprintf(pszLogingInfo,512,"RestoreRole::未先创建账号对象[%s]!", cdkey);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				CMessage msg(MSG_W2L_LOG_RESTOREROLE);
				msg.Add((char)LOGIN_RESTOREROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Send();//send to loginserver
				return;
			}
			lPlayer = tLoginInfo->FindLoginPlayer(guid);
			if(NULL == lPlayer)
			{
				CMessage msg(MSG_W2L_LOG_RESTOREROLE);
				msg.Add((char)LOGIN_RESTOREROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Send();//send to loginserver
				_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_RESTORCHARNOCHAROBJ), cdkey, szGUID);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				return;
			}
			if(tLoginInfo->GetPlayerState() != PLAYER_STATE_LOADED_BASE_DATA)
			{
				CMessage msg(MSG_W2L_LOG_RESTOREROLE);
				msg.Add((char)LOGIN_RESTOREROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.Send();//send to loginserver
				_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_RESTORECHARVALID), 
					tLoginInfo->GetPlayerState(), cdkey, szGUID);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				return;
			}

			//! 联系人系统
			LinkmanSystem::GetInstance().OnPlayerDelStateChange(guid, TRUE);

			// 以下数据加入MSG发送给DBS
			GetGame()->GetEntityManager()->CreateRestoreLoginPlayerDelTimeSession(cdkey, guid);
		}
		break;

	case MSG_L2W_LOG_CREATEROLE://loginserver请求创建一个角色
		{
			char szName[50],szCdkey[ACCOUNT_SIZE];
			pMsg->GetStr(szName,50);
			char nOccupation = pMsg->GetChar();
			char nSex = pMsg->GetChar();
			BYTE nHead = pMsg->GetByte();
			BYTE nFace = pMsg->GetByte();
			BYTE btCountry = pMsg->GetByte();
			BYTE byConstellation = pMsg->GetByte();
			//是否选择了推荐的国家
			BYTE bySelectRecommCountry = pMsg->GetByte();
			bool bSelectRecommCountry = (bySelectRecommCountry==0?false:true);
			BYTE btMaxCharactersNum = CGlobeSetup::GetSetup()->btMaxCharactersNum;
			pMsg->GetStr(szCdkey,ACCOUNT_SIZE);

			if(szName[0] == '\0') // 名字为空
			{
				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_NAMEINVALID);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());
				return;
			}

			// 结合创建列表和数据库，检测角色是否被建满。
			// 检测职业和性别是否符合条件
			CGlobeSetup::GetSetup()->btMaxCharactersNum;
			bool bValid = false;
			//判断职业是否合法
			if(nOccupation < 0 || nOccupation >= OCC_Max)
				return;
			//判断性别是否合法
			if( nSex < 0 || nSex >= CPlayerList::SEX_NUMS)
				return;
			//判断星座
			if(byConstellation < CONST_Aries || byConstellation > CONST_Pisces)
				return;

			// 检测国家
			bValid = false;
			if( !GetCountryHandler()->GetCountry(btCountry) )
			{
				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_DBERROR);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());

				_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_CREATECHARNOCOUNTRY),btCountry);
				PutStringToFile("Login_WS_Info",pszLogingInfo);
				return;
			}

			// 检测名字是否存在
			string strName(szName);
			if (!CWordsFilter::GetInstance()->Check(strName))
			{
				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_NAMEINVALID);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());
				return;
			}

			// 角色名也被使用(先list和MapPlayer 最后DBS)
			if(	GetGame()->IsNameExistInMapPlayer(szName) )
			{
				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_NAMEEXIST);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());
				return;
			}

			// 再检查一次是否已有该玩家的创建请求(通过LoginPlayer查找)
			Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
				_snprintf(pszLogingInfo,512,"MSG_W2L_LOG_CREATEROLE! LoginInfo[%s] Is Null!", szCdkey);
				PutStringToFile("Login_WS_Info",pszLogingInfo);

				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_FAILED);
				msg.Add(szCdkey);
				msg.SendToSocket(GetGame()->GetLSSocketID());
				return;
			}

			if(tLoginInfo->GetPlayerState() == PLAYER_STATE_CREATING) return;
			
			// 再到DB对象中查找一次
			Account* pAcc = NULL;
			CEntityManager::AccountMapItr accItr = GetGame()->GetEntityManager()->GetAccountMap().begin();
			for(; accItr != GetGame()->GetEntityManager()->GetAccountMap().end(); accItr++)
			{
				pAcc = accItr->second;
				if(pAcc->GetEntityGroupMap().size() == 1)
				{
					CEntityGroup* pLoginPlayerGroup = (CEntityGroup*)pAcc->GetEntityGroupMap().begin()->second;
					map<CGUID, CBaseEntity*>::iterator setItr;
					for(setItr=pLoginPlayerGroup->GetEntityGroupMap().begin(); setItr!=pLoginPlayerGroup->GetEntityGroupMap().end(); setItr++)
					{
						const char* szEPName = setItr->second->GetStringAttr(string("Name"));
						if(szEPName)
						{
							if(strcmp(szName, szEPName) == 0)
							{
								// 找到该玩家的LoginPlayer对象 丢弃该创建消息
								CMessage msg(MSG_W2L_LOG_CREATEROLE);
								msg.Add((BYTE)LOGIN_CREATEROLE_NAMEEXIST);
								msg.Add(szCdkey);
								msg.SendToSocket(GetGame()->GetLSSocketID());
								return;
							}
						}
					}
				}
			}

			
			// 创建一个创建玩家会话
			GetGame()->GetEntityManager()->CreatePlayerCreateSession(szCdkey, szName, nOccupation, nSex, nHead, nFace,
				btCountry, btMaxCharactersNum,byConstellation,bSelectRecommCountry);		
			_snprintf(pszLogingInfo,512,CStringReading::LoadString(IDS_WS_LOGMESSAGE, STR_WS_LOGMESSAGE_SENDCREATETODBS), szName);
			PutStringToFile("Login_WS_Info",pszLogingInfo);
		}
		break;
	}
}
/*
<!--客户端上下线日志-->
<Table name="t550_client_in_game_log" log_no="550" log_pos="1">
	<item type="client_head"     head_name=""></item>
	<item type="alone"    Item_name="cdkey"         data_type="str128"></item><!--IP-->
	<item type="alone"    Item_name="ip"            data_type="int32"></item><!--CDKEY-->
	<item type="player"   head_name=""></item>
	<item type="pos"      head_name=""></item>
	<item type="alone"    Item_name="region_guid"   data_type="int32"></item><!--level-->
	<item type="alone"    Item_name="level"         data_type="int32"></item><!--level-->
	<item type="alone"    Item_name="gold"          data_type="int32"></item><!--gold-->
	<item type="alone"    Item_name="silver"        data_type="int32"></item><!--silver-->
	<item type="alone"    Item_name="flag"          data_type="int32"></item><!--操作类型：1=上线、2=下线-->
	<item type="alone"    Item_name="online_time"   data_type="int32"></item><!--此次操作在线分钟数，上线类型，此处为0，下线类型，此处有值-->
</Table>
*/
bool LogicLogInterface::logT550_client_in_game_log(CPlayer *pPlayer, LONG lState)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;
	if(NULL == m_pLogClient)return false;
	if(NULL == pPlayer) return false;

	string strTime;
	GetCurrTimeString(strTime);

	const char *pRegionName = "";
	map<LONG, CGlobalRgnManager::tagRegion> &mapRegionList = *(GetGame()->GetGlobalRgnManager()->GetTemplateRegionListMap());
	map<LONG, CGlobalRgnManager::tagRegion>::iterator ite = mapRegionList.find(pPlayer->GetRegionID());
	if (ite != mapRegionList.end())
	{
		CWorldRegion *pWorldRegion = ite->second.pRegion;
		if (NULL != pWorldRegion)
		{
			pRegionName = pWorldRegion->GetName();
		}
	}

	float fPosX = pPlayer->GetPosX();
	float fPosY = pPlayer->GetPosY();

	//! 转换IP
	;
	char szTmp[128] = {0};
	UCHAR arrIp[4] = {0};
	LONG lIP = pPlayer->GetIP();
	memmove(arrIp, &lIP, sizeof(LONG));
	sprintf(szTmp, "%u.%u.%u.%u", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);

	LONG lBankMoney = 0;
	CGoodsContainer* pBank = pPlayer->m_PlayerDepot.FindContainer(eDCT_Gold);
	if(NULL != pBank)
		lBankMoney = pBank->GetGoodsAmount();

	DWORD OnlineTime = 0;
	if(2 == lState)
		OnlineTime = (timeGetTime() - pPlayer->GetLoginTime()) / (1000 * 60);

	return m_pLogClient->SendLogToServer( 
		550, strTime.c_str(),
		pPlayer->GetAccount(),
		szTmp,
		&(pPlayer->GetExID()),	pPlayer->GetName(),
		pPlayer->GetRegionID(),
		pRegionName,
		&fPosX,
		&fPosY,
		&(pPlayer->GetRegionExID()),
		pPlayer->GetLevel(),
		pPlayer->GetMoney() + lBankMoney,
		pPlayer->GetSilver(),
		lState,
		OnlineTime
		);
}
