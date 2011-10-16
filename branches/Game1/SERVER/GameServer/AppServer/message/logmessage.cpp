#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\player.h"
#include "..\serverRegion.h"
#include "..\..\gameserver\game.h"
#include "../setup/GlobeSetup.h"
#include "../setup/QuestSystem.h"
#include "..\Session\CSessionFactory.h"
#include <MMSystem.h>
#include "SequenceString.h"
#include "../session/CPlug.h"
#include "..\public\dupliregionsetup.h"
#include "../setup/ContributeSetup.h"
#include "../RgnManager.h"
#include "..\PlayerMan\PlayerMan.h"


// 响应LOG消息
void OnLogMessage(CMessage* pMsg)
{
	char pszLogingInfo[512]="";
	switch(pMsg->GetType())
	{
	case MSG_C2S_LOG_PLAYERENTER://新玩家连接到gameserver
		{
			long lSignCode = pMsg->GetLong();

			CGUID PlayerID;
			pMsg -> GetGUID(PlayerID);

			char szCdkey[128];
			pMsg->GetStr(szCdkey, sizeof(szCdkey));

			// 登陆时角色存在于GameServer 并且player不为NULL，说明已经进去了游戏
			CPlayer* player = GetGame()->FindPlayer(PlayerID);
			//如果Player存在，做错误说明输出。
			if (player)
			{
				char szPlayerID[50] = "";
				PlayerID.tostring(szPlayerID);
				_snprintf(pszLogingInfo,512,"error,when the player(Name:%s,RegionID:%d)connect the gs,that has the same guid player!", player->GetName(),player->GetRegionID());
				PutStringToFile("Login_GS_Info",pszLogingInfo);
			}

			long lLoginCount = GameManager::GetInstance()->AddLoginSession(pMsg->GetSocketID());
			// 正常
			CMessage msg(MSG_S2W_LOG_QUEST_PLAYERDATA);
			msg.Add((long)lSignCode);
			msg.Add(PlayerID);
			msg.Add(lLoginCount);
			msg.Add(szCdkey);
			msg.Add(pMsg->GetIP());
			msg.Send();
		}
		break;	

	case MSG_W2S_LOG_KICKPLAYER://worldserver通知踢一个特定ID的玩家
		{
		}
		break;

	case MSG_W2S_LOG_ANSWER_PLAYERDATA://worldserver返回玩家的详细属性
		{
#ifdef _RUNSTACKINFO3_
			char pszStatckInfo[1024]="";
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MSG_W2S_LOG_ANSWER_PLAYERDATA Start");
#endif
			long lSignCode = pMsg->GetLong();
			long lFlag=pMsg->GetLong();
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			long lLoginCount = pMsg->GetLong();
			char szPlayerID[40];
			PlayerID.tostring(szPlayerID);

#ifdef _RUNSTACKINFO3_
			char pszGUID[50]="";
			PlayerID.tostring(pszGUID);
			_snprintf(pszStatckInfo,1024,"playerID:%s,flag:%d",pszGUID,lFlag);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

			long lC2SSocketID = GameManager::GetInstance()->RemoveLoginSessionByCountNum(lLoginCount);
			// login server上没有该玩家
			if (lFlag==0)
			{
				CMessage msg1(MSG_S2C_LOG_ANSWER_PLAYERDATA);
				msg1.Add(lSignCode);
				msg1.Add((long)0);
				msg1.SendToSocket(lC2SSocketID);

				_snprintf(pszLogingInfo,512,"返回玩家的详细属性时 (%s) 没有正常登陆,已被勒令断开!", szPlayerID);
				PutStringToFile("Login_GS_Info",pszLogingInfo);
				GetGame()->GetNetServer()->DisConn(lC2SSocketID);
				return;
			}

			// 该玩家在线
			if (lFlag==-1)
			{
				CMessage msg1(MSG_S2C_LOG_ANSWER_PLAYERDATA);
				msg1.Add(lSignCode);
				msg1.Add((long)0);
				msg1.SendToSocket(lC2SSocketID);

				_snprintf(pszLogingInfo,512,"返回玩家的详细属性时 (%s) 该玩家在线!", szPlayerID);
				PutStringToFile("Login_GS_Info",pszLogingInfo);
				GetGame()->GetNetServer()->DisConn(lC2SSocketID);
				return;
			}

			// GameServer非法
			if (lFlag==-2)
			{
				CMessage msg1(MSG_S2C_LOG_ANSWER_PLAYERDATA);
				msg1.Add(lSignCode);
				msg1.Add((long)0);
				msg1.SendToSocket(lC2SSocketID);

				_snprintf(pszLogingInfo,512,"返回玩家的详细属性时 (%s) GameServer非法!", szPlayerID);
				PutStringToFile("Login_GS_Info",pszLogingInfo);
				GetGame()->GetNetServer()->DisConn(lC2SSocketID);
				return;
			}

			// 验证码错误
			if (lFlag==-3)
			{
				CMessage msg1(MSG_S2C_LOG_ANSWER_PLAYERDATA);
				msg1.Add(lSignCode);
				msg1.Add((long)0);
				msg1.SendToSocket(lC2SSocketID);

				_snprintf(pszLogingInfo,512,"返回玩家的详细属性时 (%s) GameServer非法!", szPlayerID);
				PutStringToFile("Login_GS_Info",pszLogingInfo);
				GetGame()->GetNetServer()->DisConn(lC2SSocketID);
				return;
			}

			//该连接已经断开
			if(lC2SSocketID == 0)
			{
				//##发送消息
				CMessage msg( MSG_S2W_LOG_PLAYERQUIT );
				msg.Add( PlayerID );
				msg.Add((long)-1); // 非正常退出
				msg.Add(0L);
				msg.Add("");
				msg.Send();
				return;
			}
			// 该玩家不存在于PlayerList,也就是说没有收到该玩家的 MSG_C2S_LOG_PLAYERENTER 消息,或者在中间被其他登陆踢掉了
			if( lFlag == 1 )
			{
				// 起用了这个功能
				if( GetGame()->GetSetup()->dwMsgValidateTime )
				{
					// 添加到MAP里
					//GetGame()->RemoveValidateTime(lPlayerID);
					GetGame()->AppendValidateTime(PlayerID, true);

					// 发送GamerSErver 的时间到Cleint
					CMessage msgServerTime(MSG_S2C_LOG_GAMESERVERTIME);
					unsigned long ulServerTime = /*(ULONG)time(NULL)*/ timeGetTime();
					msgServerTime.Add( PlayerID );
					msgServerTime.Add( ulServerTime );
					//! 添加一个日期时间发送到客户端[2008-1-3]
					long ldatatime = 0;
					time(&ldatatime);
					msgServerTime.Add(ldatatime);
					//!
					msgServerTime.SendToSocket(lC2SSocketID);
				}

				CPlayer *pPlayer = GetGame()->FindPlayer(PlayerID);
				if(pPlayer)
				{
					//进入此处,程序逻辑发生了错误,要查找bug
					//如果情况发生做一些强制性处理
					_snprintf(pszLogingInfo,512,"error,when the player(Name:%s,RegionID:%d) enter the game,the gs has the same guid player!",
						pPlayer->GetName(),pPlayer->GetRegionID());
					PutStringToFile("Login_GS_Info",pszLogingInfo);

					CServerRegion* pRegion = dynamic_cast<CServerRegion*>(pPlayer->GetFather());
					if(pRegion)
						pRegion->RemoveObject(pPlayer);
				}
				else
				{
					pPlayer = MP_NEW CPlayer;
					if(pPlayer)
						pPlayer->SetExID(PlayerID);
				}
				pPlayer->SetSignCode(lSignCode);
				// 初始化玩家数据
				DBReadSet setReadDB;
				pMsg->GetDBReadSet(setReadDB);
				pPlayer->DecodeFromDataBlock(setReadDB);

				//GetGame()->FreeShareDB(ByteData);

				//关联SocketID到PlayerID
				CMessage::MapPlayerIDSocket(PlayerID,lC2SSocketID);
				GetGame()->AddPlayer(pPlayer);
				// Fox@20081110 增加玩家
				GetInst(CPlayerMan).AddPlayer(pPlayer);

				// 进入游戏
				if(!pPlayer->OnEnter())
				{
					GetGame()->KickPlayer(pPlayer->GetExID());
				}

				if(GetGame()->IsBSConnected())
				{
					CMessage msg(MSG_S2B_GET_BALANCE);
					msg.Add(pPlayer->GetAccount());
					msg.Add(pPlayer->GetExID());
					msg.SendToBS();
				}
			}
#ifdef _RUNSTACKINFO3_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MSG_W2S_LOG_ANSWER_PLAYERDATA End");
#endif
		}
		break;
	case MSG_W2S_LOG_REPEAT_LOGIN:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			char strCDKey[256];
			pMsg -> GetStr( strCDKey, 256 );
			char szPlayerID[40] = {0};
			PlayerID.tostring(szPlayerID);

			CPlayer* pPlayer	= GetGame() -> FindPlayer( PlayerID );
			if( pPlayer )
			{			
				//##首先发送消息给该玩家。
				CMessage repeateLoginMsg(MSG_S2C_OTHER_REPEATLOGIN);
				repeateLoginMsg.Add("有人尝试从其他地方登陆游戏,你被强制下线了.");
				repeateLoginMsg.SendToPlayer(PlayerID);

				GetGame()->OnPlayerExit(PlayerID, FALSE);

				GetGame()->RemovePlayer(PlayerID);
				// 断掉连接
				GetGame()->KickPlayer(PlayerID);
			}
			else
			{
				//##发送消息
				CMessage msg( MSG_S2W_LOG_PLAYERQUIT );
				msg.Add( PlayerID );
				msg.Add((long)-1); // 非正常退出
				msg.Add(0L);
				msg.Add( strCDKey );
				msg.Send();
			}
		}
		break;

	case MSG_W2S_LOG_FCM_TIME:
		{
			CGUID playerID;
			pMsg->GetGUID(playerID);
			long lTime = pMsg->GetLong();
			CPlayer* pPlayer = GetGame()->FindPlayer(playerID);
			if(pPlayer)
			{
				CMessage msg(MSG_L2C_LOG_FCM_TIME);
				msg.Add(lTime);
				msg.SendToPlayer(playerID);
			}
		}
		break;
	}
}
