#include "stdafx.h"
#include "../LoginManager.h"
#include "../../Net/Message.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应LOG消息
void OnLogMessage(CMessage* pMsg)
{
	char pszLogingInfo[512]="";
	switch(pMsg->GetType())
	{
	case MSG_C2L_LOG_SEND_PT_TOKEN:
		{
			if(!GetGame()->GetLoginSetup()->bSndaValidate)
			{
				CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
				msg.Add((LONG)LOGIN_MODE_ERROR);
				msg.SendToClient(pMsg->GetSocketID());
				break;
			}

			//! 程序版本
			long lVersion = pMsg->GetLong();
			extern long g_lVersion;
			if(lVersion!=g_lVersion)
			{
				CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
				msg.Add((LONG)LOGIN_VERSIONERROR);
				msg.SendToClient(pMsg->GetSocketID());
				break;
			}

			//! 资源版本
			char szClientResourceVersion[RESOURCE_VERSION_SIZE]="99.99.9999";
			pMsg->GetEx(szClientResourceVersion,RESOURCE_VERSION_SIZE);
			if(GetGame()->GetLoginSetup()->m_bValideClientResourceVersion &&
				strcmp(szClientResourceVersion,GetGame()->GetLoginSetup()->m_szClientResourceVersion) !=0 )
			{
				CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
				msg.Add((LONG)LOGIN_VERSIONERROR);
				msg.SendToClient(pMsg->GetSocketID());
				break;
			}

			//! WS名称
			char szWSName[WS_NAME_SIZE] = {0};
			pMsg->GetEx(szWSName, WS_NAME_SIZE);

			char szToken[PT_TOKEN_SIZE] = {0};
			pMsg->GetEx(szToken, PT_TOKEN_SIZE);
#ifdef SHOW_LOGIN_MSG_20090817
			if(GetGame()->OutInfo())
			{
                Log4c::Trace(ROOT_MODULE,"MSG_C2L_LOG_SEND_PT_TOKEN");
			}
#endif
			GetGame()->GetCSdoaValidate().RequestValidate(szToken, pMsg->GetIP(), pMsg->GetSocketID(), szWSName);
		}
		break;
	case MSG_C2L_LOG_QUESTCDKEY://客户CDKEY验证
		{
			if(GetGame()->GetLoginSetup()->bSndaValidate)
			{
				CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
				msg.Add((LONG)LOGIN_MODE_ERROR);
				msg.SendToClient(pMsg->GetSocketID());
				break;
			}

			tagAccountInfo_ToAS AccountInfo_ToAS;
			long lVersion = pMsg->GetLong();
			extern long g_lVersion;
			if(lVersion!=g_lVersion)
			{
				CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
				msg.Add((LONG)LOGIN_VERSIONERROR);
				msg.SendToClient(pMsg->GetSocketID());
				break;
			}

			char szClientResourceVersion[RESOURCE_VERSION_SIZE]="99.99.9999";
			pMsg->GetEx(szClientResourceVersion,RESOURCE_VERSION_SIZE);
			if(GetGame()->GetLoginSetup()->m_bValideClientResourceVersion &&
				strcmp(szClientResourceVersion,GetGame()->GetLoginSetup()->m_szClientResourceVersion) !=0 )
			{
				CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
				msg.Add((LONG)LOGIN_VERSIONERROR);
				msg.SendToClient(pMsg->GetSocketID());
				break;
			}

			//! 账号
			pMsg->GetEx(AccountInfo_ToAS._szCdkey, CDKEY_SIZE);

			match_results results;
			rpattern &pat = GetGame()->GetRpattern();
			match_results::backref_type br = pat.match(string(AccountInfo_ToAS._szCdkey), results);
			if(!br.matched)
			{
				CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
				msg.Add((LONG)LOGIN_NOTCDKEY);
				msg.SendToClient(pMsg->GetSocketID());
				return;
			}

			size_t CdKeySize = strlen(AccountInfo_ToAS._szCdkey);
			for( size_t i = 0; i < CdKeySize; ++i )
			{
				//!将szCdkey变成Lower-case的字符串。
				AccountInfo_ToAS._szCdkey[i] = static_cast<char>( tolower(AccountInfo_ToAS._szCdkey[i]) );
			}

			DWORD dwRe = GetGame()->GetLoginManager().IsFrozenIp(pMsg->GetIP(), AccountInfo_ToAS._szCdkey, timeGetTime());
			if(1 != dwRe)	
			{
				if(0 != dwRe)
				{
					//! 提示客户端
					CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
					msg.Add((LONG)dwRe);
					msg.SendToClient(pMsg->GetSocketID());
					//! 日志
					UCHAR arrIp[4] = {0};
					LONG lIP = pMsg->GetIP();
					memmove(arrIp, &lIP, sizeof(LONG));

					Log4c::Trace(ROOT_MODULE, "登陆请求时IP被限制：IP：%u.%u.%u.%u，账号：%s", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3], AccountInfo_ToAS._szCdkey);
				}

				return;
			}

			//! 密码
			BYTE	bytePassword[16];
			if(pMsg->GetEx(bytePassword,16) == NULL)
			{
				break;
			}

			//把二进制密码转换成字符串
			static char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			BYTE* pByte = &bytePassword[0];
			for(int i=0; i<16; i++)
			{
				int bValue = bytePassword[i];
				int bValue1 = bValue>>4;
				int bValue2 = bValue&0x0F;
				if(bValue1 >= 0 && bValue1 <= 15 && bValue2 >= 0 && bValue2 <= 15)
				{
					AccountInfo_ToAS._szPwd[i*2] = hexDigits[bValue1];
					AccountInfo_ToAS._szPwd[i*2+1] = hexDigits[bValue2];
				}
			}
			AccountInfo_ToAS._szPwd[PWD_SIZE - 1] = 0;
			strupr(AccountInfo_ToAS._szPwd);

			//! WS名称
			pMsg->GetEx(AccountInfo_ToAS._szWSName, WS_NAME_SIZE);

			//! 挑战码、密宝
			long lChallengeCode = pMsg->GetLong();
			long lEkey = pMsg->GetLong();

			//加入到处理队列
			AccountInfo_ToAS._dwIp		= pMsg->GetIP();
			AccountInfo_ToAS._lSocket	= pMsg->GetSocketID();
			GetGame()->GetLoginManager().OnClientLoginRequest( AccountInfo_ToAS );
		}
		break;

	case MSG_C2L_LOGIN_AND_QUEST_PLAYERBASELIST:
		{
            char szTmp[128];
            SYSTEMTIME t;
            GetLocalTime(&t);            
            sprintf(szTmp, "MSG_C2L_LOGIN_AND_QUEST_PLAYERBASELIST : %d:%d:%d %d\n", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
            OutputDebugStr(szTmp);
			tagAccountInfo_ToAS AccountInfo_ToAS;
			pMsg->GetEx(AccountInfo_ToAS._szCdkey, CDKEY_SIZE);
			DWORD dwServerKey[SERVER_KEY_DWORD_SIZE];
			for (LONG i = 0; i < SERVER_KEY_DWORD_SIZE; ++i)
			{
				dwServerKey[i] = pMsg->GetDWord();
			}

			AccountInfo_ToAS._lSocket	= pMsg->GetSocketID();
			AccountInfo_ToAS._dwIp		= pMsg->GetIP();
			GetGame()->GetLoginManager().OnClientAgainLoginRequest(AccountInfo_ToAS, dwServerKey);

		}
		break;
	case MSG_C2L_LOG_VALIDATE_MATRIX:
		{
			char szCdKey[CDKEY_SIZE] = {0};
			pMsg->GetStr(szCdKey, CDKEY_SIZE);

			size_t CdKeySize = strlen(szCdKey);
			for( size_t i = 0; i < CdKeySize; ++i )
			{
				//!将szCdkey变成Lower-case的字符串。
				szCdKey[i] = static_cast<char>( tolower(szCdKey[i]) );
			}

			char m_cMatrixCode[SECURITY_VALIDATE_POS_NUM] = {0};
			pMsg->GetEx(m_cMatrixCode, SECURITY_VALIDATE_POS_NUM);

			GetGame()->GetLoginManager().OnClientSecurityInput(szCdKey, m_cMatrixCode, pMsg->GetSocketID());
		}
		break;
	case MSG_C2L_LOG_QUEST_PLAYERBASELIST://客户请求自己的基本信息
		{
			char szWSName[20];
			pMsg->GetStr(szWSName,20);
			const char* pCDkey = pMsg->GetCdkey();
			if(NULL == pCDkey) return;
#ifdef SHOW_LOGIN_MSG_20090817
			if(GetGame()->OutInfo())
			{
				Log4c::Trace(ROOT_MODULE,"MSG_C2L_LOG_QUEST_PLAYERBASELIST %s", pCDkey);
			}
#endif
			GetGame()->GetLoginManager().OnClientQuestPlayerBaseList(pCDkey, szWSName);
		}
		break;

	case MSG_W2L_LOG_ANSWER_PLAYERBASELIST://worldserver返回一个账号的玩家基本信息列表
		{
			long lSignCode = pMsg->GetLong();

			char szCdkey[CDKEY_SIZE] = {0};
			BYTE cLogin = pMsg->GetByte();
			pMsg->GetEx(szCdkey,CDKEY_SIZE);//取得客户CDKEY

			long num = pMsg->GetShort();

			pMsg->GetShort();
			CGUID guid;
			pMsg->GetGUID(guid);

			if(0 != strlen(szCdkey))
			{
				if(1 == cLogin)
				{
					GetGame()->GetPassDetailDataCdkeyMap()[string(szCdkey)] = GetGame()->GetLoginCdkeyWorldServerID(szCdkey);
				}
				else
				{
					Log4c::Warn(ROOT_MODULE,FormatText("LOGIN_LOGMSG_0", szCdkey, cLogin));//WS返回取得玩家%s 基本信息失败,flag %d
				}

			}
			else
				Log4c::Warn(ROOT_MODULE,FormatText("LOGIN_LOGMSG_1", szCdkey));//WS返回玩家%s基本信息时,LS找不到该CDKEY

			pMsg->SetType(MSG_L2C_LOG_ANSWER_PLAYERBASELIST);

			pMsg->SendToClient(szCdkey);
		}
		break;

	case MSG_C2L_LOG_QUEST_PLAYERDATA://客户请求自己的详细信息 -> 进入游戏
		{
			long lSignCode = pMsg->GetLong();
			CGUID playerid;
			pMsg->GetGUID(playerid);
			const char *szCdkey = pMsg->GetCdkey();//获取该连接的CDKEY
			if(NULL == szCdkey) return;

			LONG lWsID = GetGame()->GetLoginCdkeyWorldServerID(szCdkey);
#ifdef SHOW_LOGIN_MSG_20090817
			if(GetGame()->OutInfo())
			{
				Log4c::Trace(ROOT_MODULE,"MSG_C2L_LOG_QUEST_PLAYERDATA %s", szCdkey);
			}
#endif
			GetGame()->GetLoginManager().OnClientQuestPlayerData(lWsID, szCdkey, playerid, lSignCode);
		}
		break;

	case MSG_W2L_LOG_ANSWER_PLAYERDATA://worldserver返回客户的详细信息是否成功取出
		{
			long lSignCode = pMsg->GetLong();
			char szCdkey[CDKEY_SIZE];
			int nRet=pMsg->GetChar();
			pMsg->GetStr(szCdkey,CDKEY_SIZE);

			long lWorldNumber = pMsg->GetWorldID();
			// 清除登录玩家的cdkey
			if(szCdkey)
				GetGame()->GetPassDetailDataCdkeyMap().erase(string(szCdkey));
			if(nRet==LOGIN_QUEST_PLAYERDATA_SUCCEED)
			{
				//添加日志
				char str[256];
				pMsg->GetStr (str, 256);
				pMsg->GetLong ();
				pMsg->GetStr(str, 256);
				BYTE btLevel = pMsg->GetByte ();
				GetGame()->AddCdkey(szCdkey, pMsg->GetWorldID());	// 该CDKEY登陆游戏
				GetGame()->GetLoginManager().OnWsAnswerPlayerDataSucceed(szCdkey, lWorldNumber);
			}
			else
			{
				GetGame()->ClearCDKey(szCdkey);
				GetGame()->GetLoginManager().OnClientNetClose(szCdkey, FALSE);
			}

			pMsg->SetType(MSG_L2C_LOG_ANSWER_PLAYERDATA);
			pMsg->SendToClient(szCdkey);

			CMessage::UnCDkeyBySocket(szCdkey);
		}
		break;

	case MSG_W2L_LOG_PLAYERQUIT://worldserver发送退出玩家的cdkey
		{
            char szTmp[128];
            SYSTEMTIME t;
            GetLocalTime(&t);            
            sprintf(szTmp, "MSG_W2L_LOG_PLAYERQUIT : %d:%d:%d %d\n", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
            OutputDebugStr(szTmp);
			char szCdkey[CDKEY_SIZE];
			pMsg->GetStr(szCdkey,CDKEY_SIZE);
			DWORD dwServerKey[SERVER_KEY_DWORD_SIZE];
			for (LONG i = 0; i < SERVER_KEY_DWORD_SIZE; ++i)
			{
				dwServerKey[i] = pMsg->GetDWord();
			}
			GetGame()->ClearCDKey(szCdkey);
			GetGame()->ClearLoginCdkey(szCdkey);
            GetGame()->ClearSocketCdkey(szCdkey);
			GetGame()->KickOut(szCdkey);

			LONG lWsID = CMessage::GetWorldIDBySocket(pMsg->GetSocketID());
			GetGame()->GetLoginManager().OnWsPlayerQuit(szCdkey, lWsID, dwServerKey);

			if(0 == CMessage::GetSocketByCdkey(szCdkey))
			{
				GetGame()->GetLoginManager().DelFcmCdkey(szCdkey);

				if(GetGame()->UseAsFcm())
				{
					CMessage msg(MSG_O2A_ACCOUNT_Logout);
					msg.Add(szCdkey);
					msg.SendToAS();
				}
				else
				{
					GetGame()->GetCSdoaFcm().UserOffline(szCdkey);
				}
			}
		}
		break;

	case MSG_W2L_LOG_DELETEROLE://worldserver返回一个角色删除成功与否
		{
			char szCdkey[CDKEY_SIZE];				
			pMsg->GetChar();
			CGUID tGUID;
			pMsg->GetGUID(tGUID);
			pMsg->GetStr(szCdkey,CDKEY_SIZE);

			pMsg->SetType(MSG_L2C_LOG_ANSWERDELETE);
			pMsg->SendToClient(szCdkey);
		}
		break;

	case MSG_W2L_LOG_RESTOREROLE:  
		{
			char szCdkey[CDKEY_SIZE];
			pMsg->GetChar();
			CGUID guid;
			pMsg->GetGUID(guid);
			pMsg->GetStr(szCdkey, CDKEY_SIZE);

			pMsg->SetType(MSG_L2C_LOG_ANSWERRESTORE);
			pMsg->SendToClient(szCdkey);
		}
		break;

	case MSG_C2L_LOG_QUESTDELETE://客户请求删除一个角色
		{					
			const char *szCdkey=pMsg->GetCdkey();//获取该连接的CDKEY
			if(NULL == szCdkey) return;
			const char *szWSName= GetGame()->GetLoginCdkeyWorldServer(szCdkey);//获得worldserver的名字
			CGUID playerid;
			pMsg->GetGUID(playerid);
			GetGame()->L2W_DeleteRole_Send(szWSName,szCdkey,playerid,pMsg->GetIP());
		}
		break;


	case MSG_C2L_LOG_QUESTRESTORE:
		{
			const char *szCdkey=pMsg->GetCdkey();//获取该连接的CDKEY
			if(NULL == szCdkey) return;
			const char *szWSName= GetGame()->GetLoginCdkeyWorldServer(szCdkey);//获得worldserver的名字

			CGUID guid;
			pMsg->GetGUID(guid);

			GetGame()->L2W_RestoreRole_Send(szWSName,szCdkey,guid);
		}
		break;

	case MSG_C2L_LOG_QUESTCREATE://客户请求创建一个角色
		{
			const char *szCdkey=pMsg->GetCdkey();//获取该连接的CDKEY
			if(NULL == szCdkey) return;

			DWORD dwRe = GetGame()->GetLoginManager().OnIpCreate(pMsg->GetIP());
			if(1 != dwRe)	
			{
				if(0 != dwRe)
				{
					//! 提示客户端
					CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
					msg.Add((LONG)dwRe);
					msg.SendToClient(pMsg->GetSocketID());
					//! 日志
					UCHAR arrIp[4] = {0};
					LONG lIP = pMsg->GetIP();
					memmove(arrIp, &lIP, sizeof(LONG));

					Log4c::Trace(ROOT_MODULE, "创建角色请求时IP被限制：IP：%u.%u.%u.%u，账号：%s", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3], szCdkey);
				}

				return;
			}

			const char *szWSName= GetGame()->GetLoginCdkeyWorldServer(szCdkey);//获得worldserver的名字

			GetGame()->L2W_CreateRole_Send(szWSName,szCdkey,pMsg);
		}
		break;

	case MSG_W2L_LOG_CREATEROLE://worldserver返回一个角色创建成功与否
		{
			char szCdkey[CDKEY_SIZE];
			pMsg->GetChar();
			pMsg->GetStr(szCdkey,CDKEY_SIZE);

			pMsg->SetType(MSG_L2C_LOG_ANSWERCREATE);
			pMsg->SendToClient(szCdkey);
		}
		break;
	case MSG_C2L_LOG_QUESTEXIT:
		{
			long lFlag = pMsg->GetLong();
			//连接GameServer失败
			if(lFlag==0)
			{
				const char *szCdkey = pMsg->GetCdkey();//获取该连接的CDKEY
				if(NULL == szCdkey) return;

				// 发消息到WorldServer踢掉对应的玩家LoginList信息
				long lWorldID =GetGame()->GetLoginCdkeyWorldServerID(szCdkey);
				if (lWorldID != -1)
				{
					// 发消息到WorldServer踢掉对应的玩家LoginList信息
					CMessage msgToWorldServer(MSG_L2W_LOG_KICKPLAYER);
					msgToWorldServer.Add( (char*)(szCdkey) );
					GetGame()->SendMsg2World( &msgToWorldServer, lWorldID);
				}

				GetGame()->ClearCDKey(szCdkey);
				//GetGame()->GetLoginQueue()->OnClientLost(szCdkey);
			}
		}
		break;

		// 发送帐号信息给客户端
	case MSG_W2L_LOG_SENDACCOUNTINFO:
		{
			char szCdkey[256];
			pMsg->GetStr(szCdkey, 256);

			pMsg->SetType(MSG_L2C_LOG_SENDACCOUNTINFO);
			pMsg->SendToClient(szCdkey);
		}
		break;
	}
}