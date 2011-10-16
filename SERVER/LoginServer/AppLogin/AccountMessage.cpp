#include "stdafx.h"
#include "..\nets\netlogin\message.h"
#include "LoginManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应AccountServer消息
void OnASMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_A2O_LOGIN_Affirm_Re:
		{
			DWORD	dwMyType = pMsg->GetDWord();

			AddLogText("登陆AccountServer成功！！！");
			GetGame()->SendOnlineInfoToAs();
		}
		break;
	case MSG_A2O_ACCOUNT_Validate_Re:
		{
			tagAccountInfo_FromAS AccountInfo_FromAS;
			pMsg->GetEx(&AccountInfo_FromAS, sizeof(tagAccountInfo_FromAS));

#ifdef SHOW_LOGIN_MSG_20090817
			if(GetGame()->OutInfo())
			{
				AddLogText("MSG_A2O_ACCOUNT_Validate_Re %s", AccountInfo_FromAS._szCdkey);
			}
#endif

			if ((eVT_Not_Pwd | eVT_Not_Fcm) == AccountInfo_FromAS._cValidateType)
				GetGame()->GetLoginManager().OnValidateReturn(AccountInfo_FromAS, 2);
			else
				GetGame()->GetLoginManager().OnValidateReturn(AccountInfo_FromAS, 0);	


		}
		break;

	case MSG_A2O_ACCOUNT_FcmTime_Notify:
		{
			char szCdkey[CDKEY_SIZE] = {0};
			pMsg->GetStr(szCdkey, CDKEY_SIZE);

			DWORD dwHoldSecond = pMsg->GetDWord();

			if(-1 != CMessage::GetSocketByCdkey(szCdkey) && 0 != dwHoldSecond)
			{
				GetGame()->GetLoginManager().AddFcmCdkey(szCdkey, dwHoldSecond);

				CMessage msg(MSG_L2C_LOG_FCM_TIME);
				msg.Add(dwHoldSecond);
				msg.SendToClient(szCdkey);

				return;
			}

			LONG lWSID = GetGame()->FindCdkey(szCdkey);
			if(0 != lWSID)
			{
				if(0 == dwHoldSecond)
				{
					CMessage msgToWorldServer( MSG_L2W_LOG_FCM_BEGIN );
					msgToWorldServer.Add(szCdkey);
					GetGame()->SendMsg2World( &msgToWorldServer, lWSID);
				}
				else
				{
					GetGame()->GetLoginManager().AddFcmCdkey(szCdkey, dwHoldSecond);

					CMessage msg(MSG_L2W_LOG_FCM_TIME);
					msg.Add(szCdkey);
					msg.Add(dwHoldSecond);
					GetGame()->SendMsg2World(&msg, lWSID);
				}
			}	
		}
	case MSG_A20_SECURITY_DATA:
		{
			tagAccountInfo_FromAS AccountInfo_FromAS;
			pMsg->GetEx(&AccountInfo_FromAS, sizeof(tagAccountInfo_FromAS));
			tagSecurityData SecurityData;
			pMsg->GetEx(&SecurityData, sizeof(tagSecurityData));
			GetGame()->GetLoginManager().OnSecurityData(AccountInfo_FromAS, SecurityData);
		}
	}
}


void OnAinMessage(AinMessage* pAimMsg)
{
	switch(pAimMsg->GetType())
	{
	case AIN_MSG_ACCOUNT_Validate_Re:
		{
			tagAccountInfo_FromAS AccountInfo_FromAS;
			tagSecurityData SecurityData;
			
			pAimMsg->GetEx(&AccountInfo_FromAS, sizeof(tagAccountInfo_FromAS));
			pAimMsg->GetEx(&SecurityData, sizeof(tagSecurityData));

			if(2 == SecurityData._lWorkFlag)
			{
				if(ePLR_Lost != AccountInfo_FromAS._LoginRerult._dwLoginResult)
					GetGame()->GetLoginManager().OnSecurityData(AccountInfo_FromAS, SecurityData, TRUE);
				else
					GetGame()->GetLoginManager().OnValidateReturnLost(AccountInfo_FromAS);
			}
			else
			GetGame()->GetLoginManager().OnValidateReturn(AccountInfo_FromAS, 1);	
		}
		break;

	case AIN_MSG_SDOA_VALIDARE_RETURN:
		{
			tagSdoaValidateResult SdoaValidateResult;
			pAimMsg->GetEx(&SdoaValidateResult, sizeof(tagSdoaValidateResult));
			GetGame()->GetCSdoaValidate().OnValidateResult(SdoaValidateResult);
		}
		break;

	case AIN_MSG_SDOA_FCM_NOTIFY:
		{
			int nHoldSecond = pAimMsg->GetDword();
			char szSndaID[CDKEY_SIZE] = {0};
			pAimMsg->GetStr(szSndaID, CDKEY_SIZE);

#ifdef OUT_VALIDATE_FCM_INFO_20090720
			PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "防沉迷结果处理开始：账号【%s】，剩余在线时间【%d】", szSndaID, nHoldSecond);
#endif
			if(0 < nHoldSecond)//! 还有在线时间
			{
#ifdef OUT_VALIDATE_FCM_INFO_20090720
				PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "发送到LS上的客户端");
#endif
				if(-1 != CMessage::GetSocketByCdkey(szSndaID))
				{
					CMessage msg(MSG_L2C_LOG_FCM_TIME);
					msg.Add((LONG)nHoldSecond);
					msg.SendToClient(szSndaID);
				}

#ifdef OUT_VALIDATE_FCM_INFO_20090720
				PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "通知WS转发");
#endif
				//! 已登陆游戏，通知游戏服务器
				LONG lWSID = GetGame()->FindCdkey(szSndaID);
				if(0 != lWSID)
				{
					CMessage msg(MSG_L2W_LOG_FCM_TIME);
					msg.Add(szSndaID);
					msg.Add((LONG)nHoldSecond);
					GetGame()->SendMsg2World(&msg, lWSID);
				}
				//! 已经不在游戏上
				else
				{
					//! 防止玩家正处理断开LS连接GS，但WS尚未发送客户端已登陆的中间状态
					CMessage msgToWorldServer( MSG_L2W_LOG_FCM_BEGIN );
					msgToWorldServer.Add( szSndaID );
					msgToWorldServer.SendAllWorld();

					GetGame()->GetCSdoaFcm().UserOffline(szSndaID);
					GetGame()->GetLoginManager().OnClientNetClose(szSndaID, FALSE);
				}
			}
			else//! 没有时间，踢下线
			{
				if(-1 != CMessage::GetSocketByCdkey(szSndaID))
				{
#ifdef OUT_VALIDATE_FCM_INFO_20090720
					PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "踢掉LS上的客户端");
#endif
					CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
					msg.Add((LONG)LOGIN_NOTENOUGH_HEALTHY_TIME);
					msg.SendToClient(szSndaID);

					CMessage::UnCDkeyBySocket(szSndaID);
				}

				//! 防止玩家正处理断开LS连接GS，但WS尚未发送客户端已登陆的中间状态
				//! 通知WS踢玩家下线

#ifdef OUT_VALIDATE_FCM_INFO_20090720
				PutoutLog("SDOA_FCM_INFO", LT_DEFAULT, "通知WS踢人");
#endif
				CMessage msgToWorldServer( MSG_L2W_LOG_FCM_BEGIN );
				msgToWorldServer.Add( szSndaID );
				msgToWorldServer.SendAllWorld();

				GetGame()->GetCSdoaFcm().UserOffline(szSndaID);
				GetGame()->GetLoginManager().OnClientNetClose(szSndaID, FALSE);
			}
		}
		break;
	case AIN_MSG_ACCOUNT_Business_Re:
		{
			DWORD dwRpcId = pAimMsg->GetDword();
			BYTE nResult =  pAimMsg->GetByte();
			char szSndaUrl[250] = {0};
			pAimMsg->GetStr(szSndaUrl, 250);
			BYTE nResultEx =  pAimMsg->GetByte();
			if(nResult==0&&nResultEx==0)
			{
				//不需要补全资料，进行后续操作
				GetGame()->GetCSdoaValidate().OnValidateByRpcId(dwRpcId);
			}else{
				string strSndaId = GetGame()->GetCSdoaValidate().GetUserCDKey(dwRpcId);
				//向客户端发送补全资料消息
				string strUrl = szSndaUrl;
				long lSocketID =  GetGame()->GetCSdoaValidate().GetSocketIDByCDKey(strSndaId);
				CMessage msg( MSG_L2C_LOG_Business_URL );
				msg.Add((long)strUrl.size());
				msg.Add(szSndaUrl);
				msg.SendToClient(lSocketID);
			}
		}
		break;
	case AIN_MSG_SDOA_Business_RETURN:
		{
			tagSdoaValidateResult SdoaValidateResult;
			pAimMsg->GetEx(&SdoaValidateResult, sizeof(tagSdoaValidateResult));
			GetGame()->GetCSdoaValidate().OnAsyncGetUserInfo(SdoaValidateResult);
		}
		break;
	case AIN_MSG_SDOA_Business_Validate:
		{
			DWORD dwRpcId = pAimMsg->GetDword();
			GetGame()->GetCSdoaValidate().OnValidateByRpcId(dwRpcId);
		}
		break;
	case AIN_MSG_SDOA_Business_Re_Error:
		{
			int dwRpcId = (int)pAimMsg->GetDword();
			int nResult = (int)pAimMsg->GetDword();
			string strSndaId = GetGame()->GetCSdoaValidate().GetUserCDKey(dwRpcId);
			long lSocketID =  GetGame()->GetCSdoaValidate().GetSocketIDByCDKey(strSndaId);
		    CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
		    msg.Add((long)LOGIN_AuthBusiness_ERROR);
		    msg.Add((long)nResult);
		    msg.SendToClient(lSocketID);
		}
		break;
	}
}