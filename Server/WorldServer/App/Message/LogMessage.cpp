#include "StdAfx.h"

#include "../AH/OrderMgr.h"
#include "../Business/BusinessManager.h"
#include "../Country/CountryHandler.h"
#include "../DBEntity/Account.h"
#include "../DBEntity/EntityGroup.h"
#include "../DBEntity/EntityManager.h"
#include "../GlobalRgnManager.h"
#include "../Goods/Goods.h"
#include "../Linkman/LinkmanSystem.h"
#include "../OrganizingSystem/OrganizingCtrl.h"
#include "../Player.h"
#include "../PlayerMan/PlayerMan.h"
#include "../Session/SessionFactory.h"
#include "../Session/Session.h"
#include "../../../../Public/ServerPublic/Server/Common/WordsFilter.h"
#include "../../../../Public/Setup/GlobalSetup.h"
#include "../../../../Public/Setup/PlayerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

			//_snprintf(pszLogingInfo,512,"the cdkey(%s) MSG_L2W_LOG_QUEST_PLAYERBASELIST!", szCdkey);
			//PutStringToFile("Login_WS_Info",pszLogingInfo);
#ifdef _RUNSTACKINFO1_
			CMessage::AsyWriteFile(GetGame().GetStatckFileName(),pszLogingInfo);
#endif

			// 判断DBS是否准备好了
			if (!GetGame().GetInitVarLoadFlag() 
				|| !GetGame().GetInitRegionLoadFlag()
				|| !GetGame().GetInitFactionLoadFlag()
				|| !GetGame().GetInitUnionLoadFlag()
				|| !GetNetHandler().IsServerReady(NF_DB_SERVER)
				|| !GetGame().GetInitPersonalHouseLoadFlag()
				|| ( GetInst( Business::CBusinessManager ).IsEnable() &&
				!GetInst( Business::CBusinessManager ).IsDBLoaded() ) 
                || !GetInst( AH::OrderMgr ).IsDBLoaded() )
			{
				Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGINMSG_40", szCdkey));
				CMessage pMsga(MSG_W2L_LOG_ANSWER_PLAYERBASELIST);
				// 添加验证代码
				pMsga.Add((long)0xFFFFFFFF);
				pMsga.Add((BYTE)2);// flag
				pMsga.Add((szCdkey));// cdkey
				pMsga.Add((short)0);// flag
				pMsga.SendToServer(NF_LOGIN_SERVER);
				return;
			}

			// 判断GS是否准备好了
            if (!CMessage::IsGSReady(-1))
            {
                CMessage pMsga(MSG_W2L_LOG_ANSWER_PLAYERBASELIST);
                // 添加验证代码
                pMsga.Add((long)0xFFFFFFFF);
                pMsga.Add((BYTE)2);// flag
                pMsga.Add((szCdkey));// cdkey
                pMsga.Add((short)0);// flag
                pMsga.SendToServer(NF_LOGIN_SERVER);
                return;
            }
			
			Account* acc = GetGame().GetEntityManager()->FindAccount(szCdkey);
			// 现在LoginPlayer对象缓冲区查找
			if(acc)
			{
				acc->ClearAllLoginPlayer();
				acc->SetName(szCdkey);
				GetGame().GetEntityManager()->CreateLoadAccountSession(acc);
			}
			else // 未找到缓冲区中的对象
			{
				// 创建session
				if(szCdkey[0] != '\0')
				{
					acc = new Account;
					if(acc)
					{
						CGUID accGuid;
						CGUID::CreateGUID(accGuid);
						acc->SetGUID(accGuid);
						acc->SetName(szCdkey);
					}
					GetGame().GetEntityManager()->CreateLoadAccountSession(acc);
					SAFE_DELETE(acc);
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
			Account* tLoginInfo = GetGame().GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
				char szGUID[128];
				guid.tostring(szGUID);
// 				_snprintf(pszLogingInfo,512,"MSG_L2W_LOG_QUEST_PLAYERDATA! Account[%s] Is Null!", szGUID, szCdkey);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);
                Log4c::Warn(ROOT_MODULE,"MSG_L2W_LOG_QUEST_PLAYERDATA! Account[%s] Is Null!", szCdkey);
				CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);
				msg.Add((long)0xFFFFFFFF);
				msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
				msg.Add(szCdkey);
				msg.SendToServer(NF_LOGIN_SERVER);
				return;
			}		
			
			if(tLoginInfo->GetSignCode() != lTestSignCode)
			{
				char szGUID[128];
				guid.tostring(szGUID);
// 				_snprintf(pszLogingInfo,512,"MSG_W2L_LOG_ANSWER_PLAYERDATA 玩家ID[%s],Cdkey[%s] 验证码错误!", szGUID, szCdkey);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Trace(ROOT_MODULE,"MSG_W2L_LOG_ANSWER_PLAYERDATA 玩家ID[%s],Cdkey[%s] 验证码错误!", szGUID, szCdkey);
				
				// 现在CLoginPlayer中找,如果没有对象或者状态为OFFLINE表示非法登录
				tLoginInfo->SetPlayerState(PLAYER_STATE_VOID);
				CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);
				long tmpSignCode = 0;
				GetGame().GenerateSignCodeByTime(tmpSignCode);
				tLoginInfo->SetSignCode(tmpSignCode);
				msg.Add((long)tLoginInfo->GetSignCode());
				msg.Add((char)LOGIN_QUEST_PLAYERDATA_SIGNCODE_ERROR);
				msg.Add(szCdkey);
				msg.SendToServer(NF_LOGIN_SERVER);
				return;
			}	
			// 现在CLoginPlayer中找,如果没有对象或者状态为OFFLINE表示非法登录
			CEntityGroup* lPlayer = (CEntityGroup*)tLoginInfo->FindLoginPlayer(guid);
			// 找到CDBLoginPlayer对象
			if( lPlayer && (tLoginInfo->GetPlayerState() == PLAYER_STATE_LOADED_BASE_DATA) )
			{	
				if(GetGame().GetEntityManager()->ComputeLoginPlayerDelTime(lPlayer) != -1)
				{
					char szGUID[128];
					guid.tostring(szGUID);
// 					_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_0"), szCdkey);
// 					PutStringToFile("Login_WS_Info",pszLogingInfo);

                    Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_0", szCdkey));

					CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);	
					msg.Add((long)-1);
					msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
					msg.Add(szCdkey);
					msg.SendToServer(NF_LOGIN_SERVER);

					tLoginInfo->SetPlayerState(PLAYER_STATE_VOID);
					return;
				}
				// 需要到数据库读取
				GetGame().GetEntityManager()->CreateLoadPlayerDataSession(szCdkey, guid);
				GetGame().GetEntityManager()->CreateInitLoadMailSession(guid);
			}
			else
			{
				CMessage msg(MSG_W2L_LOG_ANSWER_PLAYERDATA);
				msg.Add((long)-1);
				msg.Add((char)LOGIN_QUEST_PLAYERDATA_FAILED);
				msg.Add(szCdkey);
				msg.SendToServer(NF_LOGIN_SERVER);

				//char szGUID[128];
				//guid.tostring(szGUID);
				/*_snprintf(pszLogingInfo,512, AppFrame::GetText("WS_LOGMESSAGE_QUESTPROPERTYNOCREATEORLOADBASE"), szGUID, szCdkey);
				PutStringToFile("Login_WS_Info",pszLogingInfo);*/
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
			Account* tLoginInfo = GetGame().GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
				CPlayer *pPlayer = GetGame().GetMapPlayer(guid);
				if(pPlayer)
				{
					GetGame().OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
				}
// 				_snprintf(pszLogingInfo,512, "MSG_S2W_LOG_QUEST_PLAYERDATA! Account[%s] Is Null!", szCdkey);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Warn(ROOT_MODULE,"MSG_S2W_LOG_QUEST_PLAYERDATA! Account[%s] Is Null!", szCdkey);
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
				CPlayer *pPlayer = GetGame().GetMapPlayer(guid);
				if(pPlayer)
				{
					GetGame().OnPlayerQuit(pPlayer, CMessage::GetGSIDBySocket(pPlayer->GetGsSocket()), 0L);
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
// 				_snprintf(pszLogingInfo,512, "MSG_S2W_LOG_QUEST_PLAYERDATA 玩家ID[%s],Cdkey[%s] 验证码错误!", szGUID, lPlayer->GetStringAttr(string("Account")));
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Warn(ROOT_MODULE,"MSG_S2W_LOG_QUEST_PLAYERDATA 玩家ID[%s],Cdkey[%s] 验证码错误!", 
                    szGUID, lPlayer->GetStringAttr(string("Account")));
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
				CPlayer *pPlayer = GetGame().GetMapPlayer(guid);
				if(pPlayer)
				{
					long lPlayerState = tLoginInfo->GetPlayerState();
					if(GetGame().OnPlayerEnter(pPlayer, tLoginInfo, lTestSignCode, lIP, pMsg->GetGSID(), lSocketIDC2S, pMsg->GetSocketID()))
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

						GetGame().OnPlayerEnterLateCode(pPlayer, tLoginInfo);
						tLoginInfo->SetPlayerState(PLAYER_STATE_ENTERED_GAME);	
						tLoginInfo->SetLoginTimeoutValue(-1);
					}

					if(lPlayerState == PLAYER_STATE_CHANGING_SERVER)
					{	
						pPlayer->SetGsSocket(pMsg->GetSocketID());
						LinkmanSystem::GetInstance().OnPlayerChangeGs(pPlayer);
					}
				}
			}
			else
			{
				CPlayer *pPlayer = GetGame().GetMapPlayer(guid);
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
					CPlayer* pPlayer = GetGame().GetMapPlayer(PlayerGuid);
					if(NULL == pPlayer)
					{
						pPlayer = GetGame().NewRawPlayer(ePlayerQuitMsg);
						pPlayer->SetExID(PlayerGuid);
						
						// 更新WS上该玩家数据
						DBReadSet setReadDB;
						pMsg->GetDBReadSet(setReadDB);
						pPlayer->DecodeFromDataBlock(setReadDB);
						if(pPlayer)
						{
							if(GetNetHandler().IsServerReady(NF_DB_SERVER))
							{
								CWorldServerSession pSession(5000);
								// 设置当前账号状态
								Account* pAcc = NULL;
								CEntityGroup* tDBLoginPlayer = NULL;
								if(pPlayer->GetAccount())
									pAcc = GetGame().GetEntityManager()->FindAccount(pPlayer->GetAccount());
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
									tDBLoginPlayer = (CEntityGroup*)GetGame().GetEntityManager()->NewBaseEntity(string("[loginplayer]"), pPlayer->GetExID());
									pSession.NakeSendSaveLoginPlayerDataMsg(pPlayer, tDBLoginPlayer);
									GetGame().GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBLoginPlayer);
								}

								CEntityGroup* tDBPlayer = (CEntityGroup*)GetGame().GetEntityManager()->NewBaseEntity(string("[player]"), pPlayer->GetExID());
								pSession.NakeSendSavePlayerDataMsg(SAVE_DETAIL_ALL, pPlayer, tDBPlayer, true);
								GetGame().GetEntityManager()->DelBaseEntity((CBaseEntity*)tDBPlayer);
								GetGame().DelRawPlayer(pPlayer);
							}
							else
								GetGame().PushSavePlayer(pPlayer);
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

						if(GetNetHandler().IsServerReady(NF_DB_SERVER))
						{
							const char* szCdkey = pPlayer->GetAccount();
							Account* pAcc = NULL;
							if(szCdkey)
							{
								pAcc = GetGame().GetEntityManager()->FindAccount(szCdkey);
								if(pAcc)
								{
									if(pAcc->GetPlayerState() != PLAYER_STATE_VOID)
										pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
									pAcc->SetCurGamedPlayerGuid(NULL_GUID);
									pAcc->SetLoginTimeoutValue(timeGetTime());
								}
							}
							GetGame().GetEntityManager()->CreateSavePlayerDataSession(PlayerGuid, SAVE_DETAIL_ALL);
							GetGame().GetEntityManager()->CreateSaveLoginPlayerDataSession(PlayerGuid);	
						}
					}
					GetGame().OnPlayerQuit(pPlayer, pMsg->GetGSID(), dwServerKey);
				}
				break;
			default:
				{
					char szGuid[128];
					PlayerGuid.tostring(szGuid);
// 					_snprintf(pszLogingInfo,512,"玩家[%s]退出时异常,退出标志[%d],请检查代码逻辑!",szGuid, lFlag);
// 					PutStringToFile("Login_WS_Info",pszLogingInfo);

                    Log4c::Warn(ROOT_MODULE,"玩家[%s]退出时异常,退出标志[%d],请检查代码逻辑!",szGuid, lFlag);

					CPlayer* pPlayer = GetGame().GetMapPlayer(PlayerGuid);
					if(pPlayer)
					{
						Account* pAcc = GetGame().GetEntityManager()->FindAccount(pPlayer->GetAccount());
						if(pAcc)
						{
							if(pAcc->GetPlayerState() != PLAYER_STATE_VOID)
								pAcc->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
							pAcc->SetCurGamedPlayerGuid(NULL_GUID);
							pAcc->SetLoginTimeoutValue(timeGetTime());
						}
						GetGame().OnPlayerQuit(pPlayer, pMsg->GetGSID(), dwServerKey);
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
				Account* pAcc = GetGame().GetEntityManager()->FindAccount(szCdkey);
				if(pAcc)
				{
					CPlayer* pPlayer = GetGame().GetMapPlayer(pAcc->GetCurGamedPlayerGuid());
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
				Account* pAcc = GetGame().GetEntityManager()->FindAccount(szCdkey);
				if(pAcc)
				{
					CPlayer* pPlayer = GetGame().GetMapPlayer(pAcc->GetCurGamedPlayerGuid());
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
			Account* pAcc = GetGame().GetEntityManager()->FindAccount(strCDKey);
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
			pMsg->GetStr( strCDKey, 256 );

			// 看该帐号是否有角色在游戏中
			Account* tLoginInfo = GetGame().GetEntityManager()->FindAccount(strCDKey);
			if(tLoginInfo)
			{
				if(tLoginInfo->GetEntityGroupMap().size())
				{
					CGUID CurGamedPlayerGuid = tLoginInfo->GetCurGamedPlayerGuid();
					if( CurGamedPlayerGuid != NULL_GUID)
					{
						CPlayer* pPlayer = GetGame().GetMapPlayer(CurGamedPlayerGuid);
						if(pPlayer)
						{
							long gsid = GetGame().GetGlobalRgnManager()->FindGSIDByRgnID(pPlayer->GetRegionExID());
							
							if(gsid != -1)
							{
							CMessage msg( MSG_W2S_LOG_REPEAT_LOGIN );
							msg.Add( CurGamedPlayerGuid );
							msg.SendToGS(gsid);
							}
							else
							{
								tLoginInfo->SetPlayerState(PLAYER_STATE_BACKTOLOGIN);
								CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
								msg.Add(strCDKey);
								msg.Add(0L);
								msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
							}
							
// 							_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_12"), strCDKey);
// 							PutStringToFile("Login_WS_Info",pszLogingInfo);

                            Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_12", strCDKey));
							return;
						}
					}
				}
			}

			CMessage msg(MSG_W2L_LOG_PLAYERQUIT);
			msg.Add(strCDKey);
			msg.Add(0L);
			msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
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
			CEntityGroup* lPlayer = NULL;//GetGame().GetEntityManager()->FindLoginPlayer(guid);
			Account* tLoginInfo = GetGame().GetEntityManager()->FindAccount(cdkey);
			if(!tLoginInfo)
			{
// 				_snprintf(pszLogingInfo,512,"DeleteRole::未先创建账号对象[%s]!", cdkey);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);
                Log4c::Warn(ROOT_MODULE,"DeleteRole::未先创建账号对象[%s]!", cdkey);
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
				return;
			}
			lPlayer = tLoginInfo->FindLoginPlayer(guid);
			if(NULL == lPlayer)
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
// 				_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_14"), cdkey, szGUID);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_14", cdkey, szGUID));
				return;
			}
			if( tLoginInfo->GetPlayerState() != PLAYER_STATE_LOADED_BASE_DATA )
			{
				CMessage msg(MSG_W2L_LOG_DELETEROLE);
				msg.Add((char)LOGIN_DELETEROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
		
// 				_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_15"), 
// 					tLoginInfo->GetPlayerState(), cdkey, szGUID);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_15",tLoginInfo->GetPlayerState(), cdkey, szGUID));
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
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver

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
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
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
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
				return;
			}

			// 以下数据加入MSG发送给DBS
			GetGame().GetEntityManager()->CreateUpdateLoginPlayerDelTimeSession(cdkey, guid);

			char szGuid[40] = {0};
			guid.tostring(szGuid);			
// 			_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_16"), szGuid);
// 			PutStringToFile("Login_WS_Info",pszLogingInfo);

            Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_16", szGuid));
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
			CEntityGroup* lPlayer = NULL;//GetGame().GetEntityManager()->FindLoginPlayer(guid);
			Account* tLoginInfo = GetGame().GetEntityManager()->FindAccount(cdkey);
			if(!tLoginInfo)
			{
// 				_snprintf(pszLogingInfo,512,"RestoreRole::未先创建账号对象[%s]!", cdkey);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);
                Log4c::Warn(ROOT_MODULE,"RestoreRole::未先创建账号对象[%s]!", cdkey);
				CMessage msg(MSG_W2L_LOG_RESTOREROLE);
				msg.Add((char)LOGIN_RESTOREROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
				return;
			}
			lPlayer = tLoginInfo->FindLoginPlayer(guid);
			if(NULL == lPlayer)
			{
				CMessage msg(MSG_W2L_LOG_RESTOREROLE);
				msg.Add((char)LOGIN_RESTOREROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
// 				_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_17"), cdkey, szGUID);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_17", cdkey, szGUID));
				return;
			}
			if(tLoginInfo->GetPlayerState() != PLAYER_STATE_LOADED_BASE_DATA)
			{
				CMessage msg(MSG_W2L_LOG_RESTOREROLE);
				msg.Add((char)LOGIN_RESTOREROLE_FAILED);
				msg.Add(guid);
				msg.Add(cdkey);
				msg.SendToServer(NF_LOGIN_SERVER);//send to loginserver
// 				_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_18"), 
// 					tLoginInfo->GetPlayerState(), cdkey, szGUID);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_18", 
                    tLoginInfo->GetPlayerState(), cdkey, szGUID));
				return;
			}

			//! 联系人系统
			LinkmanSystem::GetInstance().OnPlayerDelStateChange(guid, TRUE);

			// 以下数据加入MSG发送给DBS
			GetGame().GetEntityManager()->CreateRestoreLoginPlayerDelTimeSession(cdkey, guid);
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
			BYTE btMaxCharactersNum = (BYTE)GlobalSetup::GetSetup()->btMaxCharactersNum;
			pMsg->GetStr(szCdkey,ACCOUNT_SIZE);

			if(szName[0] == '\0') // 名字为空
			{
				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_NAMEINVALID);
				msg.Add(szCdkey);
				msg.SendToServer(NF_LOGIN_SERVER);
				return;
			}

			// 结合创建列表和数据库，检测角色是否被建满。
			// 检测职业和性别是否符合条件
			GlobalSetup::GetSetup()->btMaxCharactersNum;
			bool bValid = false;
			//判断职业是否合法
			if(nOccupation < 0 || nOccupation >= OCC_Max)
				return;
			//判断性别是否合法
			if( nSex < 0 || nSex >= PlayerList::SEX_NUMS)
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
				msg.SendToServer(NF_LOGIN_SERVER);

// 				_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_19"),btCountry);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_19",btCountry));
				return;
			}

			// 检测名字是否存在
			string strName(szName);
			if (!CWordsFilter::GetInstance()->Check(strName))
			{
				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_NAMEINVALID);
				msg.Add(szCdkey);
				msg.SendToServer(NF_LOGIN_SERVER);
				return;
			}

			// 角色名也被使用(先list和MapPlayer 最后DBS)
			if(	GetGame().IsNameExistInMapPlayer(szName) )
			{
				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_NAMEEXIST);
				msg.Add(szCdkey);
				msg.SendToServer(NF_LOGIN_SERVER);
				return;
			}

			// 再检查一次是否已有该玩家的创建请求(通过LoginPlayer查找)
			Account* tLoginInfo = GetGame().GetEntityManager()->FindAccount(szCdkey);
			if(!tLoginInfo)
			{
// 				_snprintf(pszLogingInfo,512,"MSG_W2L_LOG_CREATEROLE! LoginInfo[%s] Is Null!", szCdkey);
// 				PutStringToFile("Login_WS_Info",pszLogingInfo);

                Log4c::Trace(ROOT_MODULE,"MSG_W2L_LOG_CREATEROLE! LoginInfo[%s] Is Null!", szCdkey);

				CMessage msg(MSG_W2L_LOG_CREATEROLE);
				msg.Add((BYTE)LOGIN_CREATEROLE_FAILED);
				msg.Add(szCdkey);
				msg.SendToServer(NF_LOGIN_SERVER);
				return;
			}

			if(tLoginInfo->GetPlayerState() == PLAYER_STATE_CREATING) return;
			
			// 再到DB对象中查找一次
			Account* pAcc = NULL;
			CEntityManager::AccountMapItr accItr = GetGame().GetEntityManager()->GetAccountMap().begin();
			for(; accItr != GetGame().GetEntityManager()->GetAccountMap().end(); accItr++)
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
								msg.SendToServer(NF_LOGIN_SERVER);
								return;
							}
						}
					}
				}
			}

			
			// 创建一个创建玩家会话
			GetGame().GetEntityManager()->CreatePlayerCreateSession(szCdkey, szName, nOccupation, nSex, nHead, nFace,
				btCountry, btMaxCharactersNum,byConstellation,bSelectRecommCountry);		
// 			_snprintf(pszLogingInfo,512,AppFrame::GetText("WS_LOGMESSAGE_20"), szName);
// 			PutStringToFile("Login_WS_Info",pszLogingInfo);

            Log4c::Trace(ROOT_MODULE,FormatText("WS_LOGMESSAGE_20", szName));
		}
		break;
	}
}

