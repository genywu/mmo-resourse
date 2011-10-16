




#include "stdafx.h"

#include "SmManage.h"
#include "UserManage.h"
#include "PackManage.h"
#include "GmSystem.h"


//! 处理消息
VOID OnSmMessage(CMessage *pMsg)
{
	switch( pMsg->GetType())
	{
		//! 登陆应答
	case MSG_SM2SCC_LOGIN_Request_Login:
		{
			char szPwd[MD5_16_PASSWORD_SIZE] = {0};
			pMsg->GetEx(szPwd, MD5_16_PASSWORD_SIZE);
			DWORD VersionNo = pMsg->GetDWord();
			SmManage::GetInstance().OnLogin(pMsg->GetSocketID(), szPwd, VersionNo);
		}
		break;
		//! 一个硬件信息
	case MSG_SM2SCC_INFO_Notify_HardInfo:
		{
			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);

			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{	
				tagHardwareInfo HardwareInfo;
				setReadDB.GetBufferFromByteArray(&HardwareInfo, sizeof(tagHardwareInfo));

				pSM->GetHardInfoList().AddNode(HardwareInfo, TRUE);
			}
		}
		break;
		//! 一组硬件信息
	case MSG_SM2SCC_INFO_Notify_HardInfoList:
		{
			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);

			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				tagHardwareInfo arrHardwareInfo[MAX_MESSAGE_INCLUDE_HARD_INFO];
				LONG lNum = setReadDB.GetLongFromByteArray();
				lNum = (MAX_MESSAGE_INCLUDE_HARD_INFO > lNum) ? lNum : MAX_MESSAGE_INCLUDE_HARD_INFO;
				for (LONG i = 0; i < lNum; ++i)
				{
					setReadDB.GetBufferFromByteArray(&arrHardwareInfo[i], sizeof(tagHardwareInfo));
				}

				pSM->GetHardInfoList().AddNodeList(arrHardwareInfo, lNum);
			}
		}
		break;
		//! Gapp基本信息
	case MSG_SM2SCC_INFO_Notify_Gapp_BaseInfo:
		{
			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);
			
			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				LONG lNum = setReadDB.GetLongFromByteArray();
				for (LONG i = 0; i < lNum; ++i)
				{
					tagGappBaseInfo GappBaseInfo;
					setReadDB.GetBufferFromByteArray(&GappBaseInfo, sizeof(tagGappBaseInfo));

					pSM->AddGapp(GappBaseInfo);
				}
			}
		}
		break;
		//! Gapp运行状态改变
	case MSG_SM2SCC_INFO_Notify_Gapp_ChangeRunState:
		{
			DWORD dwGappID		= pMsg->GetDWord();
			DWORD dwNewState	= pMsg->GetDWord();

			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				Gapp *pGapp = pSM->FindGapp(dwGappID);
				if(NULL != pGapp)
				{
					if(pGapp->ChangeRunState(dwNewState))
					{
						CMessage msg(MSG_SCC2SUC_INFO_Notify_Gapp_ChangeRunState);
						/*msg.Add(dwSMID);
						msg.Add(pGapp->GetID());
						msg.Add(dwNewState);*/
                        Json::Value var;
                        var["smID"] = (int)dwSMID;
                        var["gappID"] = (int)pGapp->GetID();
                        var["newState"] = (int)dwNewState;
                        msg.AddJson(var);
						UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
					}
				}
			}
		}
		break;
	case MSG_SM2SCC_INFO_Notify_ServerInfo:
		{
			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				DBReadSet dbReadSet;
				pMsg->GetDBReadSet(dbReadSet);

				DWORD dwGappID = dbReadSet.GetDwordFromByteArray();
				LONG lDataNum = dbReadSet.GetDwordFromByteArray();

				Gapp *pGapp = pSM->FindGapp(dwGappID);
				if(NULL != pGapp)
				{
					for (LONG i = 0; i < lDataNum; ++i)
					{
						tagDbGameInfo DbGameInfo;
						DbGameInfo.dwInfoType = dbReadSet.GetLongFromByteArray();
						dbReadSet.GetBufferFromByteArray(&DbGameInfo, sizeof(tagGameInfoData));
						pGapp->AddGameInfo(DbGameInfo);
					}
				}	
			}	
		}
		break;
		//! 错误消息
	case MSG_SM2SCC_INFO_Notify_Error:
		{
			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				pMsg->SetType(MSG_SCC2SUC_INFO_Notify_Error);
			//	pMsg->Add(dwSMID);
                Json::Value var;
                var["smID"] = (int)dwSMID;
                pMsg->AddJson(var);
				UserManage::GetInstance().SendToAll(ePT_Penniless, *pMsg);
			}
		}
		break;
		//! SM--->GAPP Echo延迟时间
	case MSG_SM2SCC_INFO_Notify_DELAYTIME:
		{
			DWORD dwGappID		= pMsg->GetDWord();
			DWORD dwDelayTime	= pMsg->GetDWord();

			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				Gapp *pGapp = pSM->FindGapp(dwGappID);
				if(NULL != pGapp)
				{
					pGapp->ChangeDelayTime(dwDelayTime);

					CMessage msg(MSG_SCC2SUC_INFO_Notify_Gapp_DELAYTIME);
					/*msg.Add(dwSMID);
					msg.Add(pGapp->GetID());
					msg.Add(dwDelayTime);*/
                    Json::Value var;
                    var["smID"] = (int)dwSMID;
                    var["gappID"] = (int)pGapp->GetID();
                    var["delayTime"] = (double)dwDelayTime;
                    msg.AddJson(var);
					UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
				}
			}
		}
		break;
		//! GAPP关闭通知
	case MSG_SM2SCC_CTRL_Notify_SHUTDOWN:
		{
			DWORD dwGappID = pMsg->GetDWord();
			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				Gapp *pGapp = pSM->FindGapp(dwGappID);
				if(NULL != pGapp)
				{
					if(pGapp->ChangeRunState(eSS_Stop))
					{
						CMessage msg(MSG_SCC2SUC_INFO_Notify_Gapp_ChangeRunState);
						/*msg.Add(dwSMID);
						msg.Add(pGapp->GetID());
						msg.Add((DWORD)eSS_Stop);*/
                        Json::Value var;
                        var["smID"] = (int)dwSMID;
                        var["gappID"] = (int)pGapp->GetID();
                        var["runState"] = eSS_Stop;
                        msg.AddJson(var);

						UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
					}
				}
			}
		}
		break;
		//! GAPP启动通知
	case MSG_SM2SCC_CTRL_Notify_START:
		{
			DWORD dwGappID = pMsg->GetDWord();
			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				Gapp *pGapp = pSM->FindGapp(dwGappID);
				if(NULL != pGapp)
				{
					if(pGapp->ChangeRunState(eSS_Running))
					{
						CMessage msg(MSG_SCC2SUC_INFO_Notify_Gapp_ChangeRunState);
						/*msg.Add(dwSMID);
						msg.Add(pGapp->GetID());
						msg.Add((DWORD)eSS_Running);*/
                        Json::Value var;
                        var["smID"] = (int)dwSMID;
                        var["gappID"] = (int)pGapp->GetID();
                        var["runState"] = eSS_Running;
                        msg.AddJson(var);

						UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
					}
				}
			}
		}
		break;
		//! 回复更新Gapp结果
	case MSG_SM2SCC_CTRL_Notify_UpdateGapp:	
		{
			DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
			SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
			if(NULL != pSM)
			{
				BYTE	cResult = pMsg->GetByte();
				DWORD	dwGappID = pMsg->GetDWord();
				char szGappVersion[MAX_GAPP_NAME_SZIE];
				pMsg->GetStr(szGappVersion, MAX_GAPP_NAME_SZIE);

				if(0 != cResult)
				{
					Gapp *pGapp = pSM->FindGapp(dwGappID);
					if(NULL != pGapp)
					{
						pGapp->SetVersion(szGappVersion);
					}
				}

				CMessage msg(MSG_SCC2SUC_INFO_Notify_Gapp_ChangeVersion);
				/*msg.Add(dwSMID);
				msg.Add(dwGappID);
				msg.Add(cResult);
				msg.Add(szGappVersion);*/
                Json::Value var;
                var["smID"] = (int)dwSMID;
                var["gappID"] = (int)dwGappID;
                var["result"] = (bool)cResult;
                var["gappVersion"] = szGappVersion;
                msg.AddJson(var);

				UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
			}
			
		}
		break;
		//! SM请求获得一个文件
	case MSG_SM2SCC_CTRL_Request_File:
		{
			DWORD dwGappID		= pMsg->GetDWord();
			DWORD dwGappType	= pMsg->GetDWord();
			char szFileName[MAX_PATH];
			pMsg->GetStr(szFileName, MAX_PATH);

			const CUpdateFile* pFile = PackManage::GetInstance().GetFile(dwGappType, szFileName);
			CMessage reMsg(MSG_SCC2SM_CTRL_Re_File);
			if(NULL == pFile)
			{
				reMsg.Add((BYTE)FALSE);
				reMsg.Add(dwGappID);
				reMsg.Add(dwGappType);
				reMsg.Add(szFileName);
			}
			else
			{
				reMsg.Add((BYTE)TRUE);
				reMsg.Add(dwGappID);
				reMsg.Add(dwGappType);

				DBWriteSet dbWriteSet;
				reMsg.GetDBWriteSet(dbWriteSet);
				pFile->Code(dbWriteSet);
			}

			reMsg.SendToSm(pMsg->GetSocketID());
		}
		break;
		//! SM请求一个GAPP的最新文件包
	case MSG_SM2SCC_CTRL_Request_ServerPack:
		{
			DWORD dwGappID		= pMsg->GetDWord();
			DWORD dwGappType	= pMsg->GetDWord();

			CMessage msg(MSG_SCC2SM_CTRL_Re_ServerPack);
			DBWriteSet dbWriteSet;
			msg.GetDBWriteSet(dbWriteSet);

			dbWriteSet.AddToByteArray(dwGappID);
			dbWriteSet.AddToByteArray(dwGappType);
			PackManage::GetInstance().FindAndGetHighestPack(dwGappType, dbWriteSet);
			
			msg.SendToSm(pMsg->GetSocketID());
		}
		break;
		//! 获取原始包结果
	case MSG_SM2SCC_CTRL_Notify_ServerPack:
		{
			BOOL bRe = pMsg->GetByte();
			if(bRe)
			{
				DWORD dwSMID = CMessage::GetSMID(pMsg->GetSocketID());
				SM *pSM = SmManage::GetInstance().FindSM(dwSMID);
				if(NULL != pSM)
				{
					DBReadSet dbReadSet;
					pMsg->GetDBReadSet(dbReadSet);

					tagGappBaseInfo GappBaseInfo;
					dbReadSet.GetBufferFromByteArray(&GappBaseInfo, sizeof(tagGappBaseInfo));

					Gapp *pGapp = pSM->FindGapp(GappBaseInfo._dwGappID);
					if(NULL != pGapp)
					{
						pGapp->UpdateInfo(GappBaseInfo);

						CMessage msg(MSG_SCC2SUC_INFO_Notify_Gapp_ChangeVersion);
						/*msg.Add(dwSMID);
						msg.Add(GappBaseInfo._dwGappID);
						msg.Add((BYTE)true);
						msg.Add(GappBaseInfo._szGappVersion);*/
                        Json::Value var;
                        var["smID"] = (int)dwSMID;
                        var["gappID"] = (int)GappBaseInfo._dwGappID;
                        var["result"] = true;
                        var["gappVersion"] = GappBaseInfo._szGappVersion;
                        msg.AddJson(var);


						UserManage::GetInstance().SendToAll(ePT_Penniless, msg);
					}
				}
			}
		}
		break;
	case MSG_SM2SCC_INFO_Notify_CTRL_GappAction:
		{
			pMsg->SetType(MSG_SCC2SUC_INFO_Notify_CTRL_GappAction);
			UserManage::GetInstance().SendToAll(ePT_Penniless, *pMsg);
		}
		break;
	case MSG_SM2SCC_CTRL_Notify_SMAction:
		{
			pMsg->SetType(MSG_SCC2SUC_CTRL_Re_SMAction);
			UserManage::GetInstance().SendToAll(ePT_Penniless, *pMsg);
		}
		break;

		//! GM相关功能
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case MSG_SM2SCC_MANAGE_Notify_Report:
		{
			tagReportInfo ReportInfo;
			pMsg->GetEx(&ReportInfo, sizeof(tagReportInfo));
			ReportInfo._WsSmId = CMessage::GetSMID(pMsg->GetSocketID());
			GmSystem::GetInstance().OnReport(ReportInfo);
		}
		break;

	case MSG_SM2SCC_MANAGE_Report_Result_Replay:
		{
			CGUID OptGuid;
			pMsg->GetGUID(OptGuid);
			DWORD OptSuccp = pMsg->GetByte();

			GmSystem::GetInstance().GetReportDbManage().SaveOptPlayerResult(OptGuid, OptSuccp);
		}		
		break;
	case MSG_SM2SCC_MANAGE_WATCH_Reply:
		{
			DWORD SMID = CMessage::GetSMID(pMsg->GetSocketID());

			DWORD GappID = pMsg->GetDWord();
			BOOL IsBegin = (0 != pMsg->GetByte());

			GmSystem::GetInstance().OnWatch(SMID, GappID, IsBegin);
		}
		break;
	case MSG_SM2SCC_MANAGE_Notify_Gm_Command_Info: //发送GM命令信息到SUC
		{
			DWORD SMID = CMessage::GetSMID(pMsg->GetSocketID());

			DWORD GappID = pMsg->GetDWord();
			DWORD GmCommand = pMsg->GetDWord();

			char szChatTxt[512] = {0};
			pMsg->GetStr(szChatTxt, 512);

			GmSystem::GetInstance().OnCommandToGm(SMID, GappID, GmCommand, szChatTxt);
		}
		break;
	default:
		break;
	}
}