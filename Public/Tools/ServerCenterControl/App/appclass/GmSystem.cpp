

#include "StdAfx.h"
#include "GmSystem.h"

//! 每个客户端，读取数据集操作，强制间隔的毫秒数
const DWORD LOAD_RS_SPACE_MS = 5000;


//! 接收SM发来的举报
VOID GmSystem::OnReport(tagReportInfo &ReportInfo)
{
	m_ReportDbManage.SaveReportInfo(ReportInfo);
}



//! 监视开始/结束
VOID GmSystem::OnWatch(DWORD SMID, DWORD GappID, BOOL IsBegin)
{
	tagKey ServerKey(SMID, GappID);
	map<__int64, LONG>::iterator ite = m_SmToUser.find(ServerKey._64Key);
	if (m_SmToUser.end() != ite)
	{
		CMessage msg(MSG_SCC2SUC_MANAGE_Notify_Watch);
		/*msg.Add((DWORD)IsBegin);
		msg.SendToSuc(ite->second);*/
        Json::Value var;
        var["isBegin"] = IsBegin;
        msg.AddJson(var);
        msg.SendToSuc(ite->second);

		if (!IsBegin)
		{
			OnScoutPlayer("", FALSE, ite->second);
		}
	}
}

//! 响应GM命令信息发送
VOID GmSystem::OnCommandToGm(DWORD WsSmid, DWORD GappID, DWORD GmCommand, const char *pChatTxt)
{
	tagKey ServerKey(WsSmid, GappID);
	map<__int64, LONG>::iterator iteUserID = m_SmToUser.find(ServerKey._64Key);
	if (m_SmToUser.end() != iteUserID)
	{
		map<LONG, tagListenInfo>::iterator iteInfo = m_UserToSm.find(iteUserID->second);
		if (m_UserToSm.end() != iteInfo)
		{
			string FlagName;
			CreatePlayerName(FlagName, iteInfo->second._WatchPlayerName.c_str(), WsSmid);
			//! GM已经没有监视了
			map<string, tagWatchInfo>::iterator iteLog = m_PlayerToUser.find(FlagName);
			if (m_PlayerToUser.end() == iteLog)
			{
				CMessage msg(MSG_SCC2SM_MANAGE_WATCH);
				msg.Add((DWORD)FALSE);
				msg.Add(GappID);
				msg.SendToSm(WsSmid);
				return;
			}

			CMessage msg(MSG_SCC2SUC_MANAGE_Notify_Gm_Command_Info);
			//msg.Add(GmCommand);
			//msg.Add(pChatTxt);//! size <= 512
            Json::Value var;
            var["gmCommand"] = (int)GmCommand;
            var["chatText"] = pChatTxt;
            msg.AddJson(var);
			msg.SendToSuc(iteUserID->second);

			FormatLogToString(iteLog->second._strInfo, GmCommand, pChatTxt, FALSE);
		}
	}
}

//! 格式化一个命令到日志字符串
VOID GmSystem::FormatLogToString(string &strLog, DWORD GmCommand, const char *pChatTxt, BOOL GmToS)
{
	//! 时间
	char szTxt[32] = {0};
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(szTxt, "\r\n[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	//! 命令值
	char szCommand[32] = {0};
	if(GmToS)
		sprintf(szCommand, "Send Command[%u] ", GmCommand);
	else
		sprintf(szCommand, "Recv Command[%u] ", GmCommand);

	strLog += szTxt;
	strLog += szCommand;
	strLog += pChatTxt;
}

//! GM接管一个WS
VOID GmSystem::GmInWs(DWORD WsSmid, DWORD GappID, const OnlineUser *pUser)
{
	if(NULL != pUser)
	{
		if(pUser->HasPurview(ePT_GmNormal))
		{
			DelListen(pUser->GetID());

			tagKey ServerKey(WsSmid, GappID);
			map<__int64, LONG>::iterator iteS = m_SmToUser.find(ServerKey._64Key);
			if (m_SmToUser.end() != iteS)
			{
				//! 已被其他用户管理
				map<LONG, tagListenInfo>::iterator iteU = m_UserToSm.find(iteS->second);
				if (m_UserToSm.end() != iteU)
				{
					if(UserSetup::GetInstance().GetTimeOutSec() > time(NULL) - iteU->second.LastOptTime)
					{
						pUser->SendInfo("other user in the world,can't in.");
						return;
					}
				}
				else
				{
					LogTools::OutErrLog("m_UserToSm is different with m_SmToUser.");
				}

				GmOutWs(iteS->second);
			}

			tagListenInfo ListenInfo;
			ListenInfo._ServerID._64Key = ServerKey._64Key;
			ListenInfo._SmSocket = CMessage::GetSMSocket(WsSmid);
			ListenInfo._pUser = pUser;
			ListenInfo._IsWatching = FALSE;
			ListenInfo.LastOptTime = (DWORD)time(NULL);
			ListenInfo.LastLoadReportTime = 0;
			ListenInfo.LastLoadResultTime = 0;
			m_UserToSm[pUser->GetID()] = ListenInfo;
			m_SmToUser[ServerKey._64Key] = pUser->GetID();

			CMessage msg(MSG_SCC2SUC_MANAGE_Notify_GmToWs);
			/*msg.Add((DWORD)TRUE);
			msg.Add(ServerKey._SMID);
			msg.Add(ServerKey._GappID);*/
            Json::Value var;
            var["result"] = true;
            var["smID"] = (int)ServerKey._SMID;
            var["gappID"] = (int)ServerKey._GappID;
            msg.AddJson(var);
			msg.SendToSuc(pUser->GetID());
		}
	}
}

//! GM退出一个WS
VOID GmSystem::GmOutWs(LONG UserID)
{
	CMessage msg(MSG_SCC2SUC_MANAGE_Notify_GmToWs);
	//msg.Add((DWORD)FALSE);
    Json::Value var;
    var["result"] = false;
    msg.AddJson(var);
	msg.SendToSuc(UserID);

	DelListen(UserID);
}

//! 响应SUC发来的举报查看请求
VOID GmSystem::OnViewReport(LONG UserID)
{
	tagListenInfo* pListenInfo = FindGmWs(UserID, ePT_GmNormal);
	if (NULL != pListenInfo)
	{
		if(LOAD_RS_SPACE_MS < timeGetTime() - pListenInfo->LastLoadReportTime)
		{
			pListenInfo->LastLoadReportTime = timeGetTime();
			m_ReportDbManage.LoadReportList(pListenInfo->_ServerID._64Key, UserID);
		}
	}
}

//! 响应SUC发来的处理结果的查看请求
VOID GmSystem::OnViewResult(LONG UserID, DWORD BeginDat)
{
	tagListenInfo* pListenInfo = FindGmWs(UserID, ePT_ViewGmOpt);
	if (NULL != pListenInfo)
	{
		if(LOAD_RS_SPACE_MS < timeGetTime() - pListenInfo->LastLoadResultTime)
		{
			pListenInfo->LastLoadResultTime = timeGetTime();
			m_ReportDbManage.LoadResultList(pListenInfo->_ServerID._64Key, UserID, BeginDat);
		}
	}
}
//! 创建一个SMID+玩家名字关联的名字
VOID GmSystem::CreatePlayerName(string &outName, const char *PlayerName, DWORD SMID)
{
	char szWsSmId[16] = {0};
	sprintf(szWsSmId, "%u ", SMID);
	outName = string(szWsSmId) + PlayerName;
}

//! 响应SUC的账号监视请求
VOID GmSystem::OnScoutPlayer(const char *PlayerName, BOOL IsWatch, LONG UserID)
{
	tagListenInfo* pListenInfo = FindGmWs(UserID, ePT_GmNormal);
	if (NULL != pListenInfo)
	{
		if (IsWatch != pListenInfo->_IsWatching)
		{	
			pListenInfo->_IsWatching = IsWatch;
			pListenInfo->_WatchPlayerName = PlayerName;
			tagWatchInfo WatchInfo;
			WatchInfo._UserID = UserID;
			string FlagName;
			CreatePlayerName(FlagName, PlayerName, pListenInfo->_ServerID._SMID);
			m_PlayerToUser[FlagName] = WatchInfo;

			CMessage msg(MSG_SCC2SM_MANAGE_WATCH);
			msg.Add(pListenInfo->_ServerID._GappID);
			if(IsWatch)
			{
				msg.Add((DWORD)TRUE);
				msg.Add(PlayerName);
			}
			else
			{
				m_PlayerToUser.erase(pListenInfo->_WatchPlayerName);
				pListenInfo->_WatchPlayerName = "";
				msg.Add((DWORD)FALSE);
			}
			msg.SendToSm(pListenInfo->_SmSocket);
		}
	}
}

//!
VOID GmSystem::EndScoutPlayer(string *pStrLog, const char *PlayerName, LONG UserID)
{
	tagListenInfo* pListenInfo = FindGmWs(UserID, ePT_GmNormal);
	if (NULL != PlayerName && NULL != pListenInfo)
	{
		string FlagName;
		CreatePlayerName(FlagName, PlayerName, pListenInfo->_ServerID._SMID);
		map<string, tagWatchInfo>::iterator ite = m_PlayerToUser.find(FlagName);
		if (m_PlayerToUser.end() != ite && UserID == ite->second._UserID)
		{
			if(NULL != pStrLog)
				*pStrLog = ite->second._strInfo;

			CMessage msg(MSG_SCC2SM_MANAGE_WATCH);
			msg.Add((DWORD)FALSE);
			msg.Add(pListenInfo->_ServerID._GappID);
			msg.SendToSm(pListenInfo->_SmSocket);

			m_PlayerToUser.erase(ite);
		}
	}
}

//! 响应SUC的账号处理请求
VOID GmSystem::OnOptPlayer(tagReportResultInfo &ReportResultInfo, LONG UserID)
{
	const tagListenInfo* pListenInfo = FindGmWs(UserID, ePT_GmNormal);
	if (NULL != pListenInfo)
	{
		strcpy_s(ReportResultInfo._szGmName, GAPP_NAME_SZIE, pListenInfo->_pUser->GetName());
		
		CGUID OptGuid;
		CGUID::CreateGUID(OptGuid);

		CMessage msg(MSG_SCC2SM_MANAGE_Report_Result);
		msg.Add(pListenInfo->_ServerID._GappID);
		msg.AddEx(&ReportResultInfo, sizeof(tagBaseResultInfo));
		msg.Add(OptGuid);
		msg.SendToSm(pListenInfo->_SmSocket);

		//! 保存日志
		EndScoutPlayer(&ReportResultInfo._strOptLog, ReportResultInfo._IndicteeName, UserID);
		m_ReportDbManage.LogOptPlayer(ReportResultInfo, OptGuid, pListenInfo->_ServerID._64Key);	
	}
}

//! 响应GM命令发送
VOID GmSystem::OnCommandToPlayer(DWORD GmCommand, const char *pChatTxt, LONG UserID)
{
	const tagListenInfo* pListenInfo = FindGmWs(UserID, ePT_GmNormal);
	if (NULL != pListenInfo)
	{
		string FlagName;
		CreatePlayerName(FlagName, pListenInfo->_WatchPlayerName.c_str(), pListenInfo->_ServerID._SMID);
		//! GM已经没有监视了
		map<string, tagWatchInfo>::iterator iteLog = m_PlayerToUser.find(FlagName);
		if (m_PlayerToUser.end() == iteLog)
		{
			CMessage msg(MSG_SCC2SM_MANAGE_WATCH);
			msg.Add((DWORD)FALSE);
			msg.Add(pListenInfo->_ServerID._GappID);
			msg.SendToSm(pListenInfo->_SmSocket);
			return;
		}

		CMessage msg(MSG_SCC2SM_MANAGE_Request_Gm_Command);
		msg.Add(pListenInfo->_ServerID._GappID);
		msg.Add(GmCommand);
		msg.Add(pChatTxt);//! size <= 512
		msg.SendToSm(pListenInfo->_SmSocket);

		FormatLogToString(iteLog->second._strInfo, GmCommand, pChatTxt, TRUE);
	}
}

//! 响应SUC发来的发送公告请求
VOID GmSystem::OnSendAffiche(const char *pAffiche, LONG UserID, DWORD SMID, DWORD GappID)
{
	const OnlineUser *pUser = UserManage::GetInstance().FindUser(UserID);
	if (NULL != pUser)
	{
		if(pUser->HasPurview(ePT_GmAffiche))
		{
			CMessage msg(MSG_SCC2SM_MANAGE_Notify_Affiche);
			msg.Add(GappID);
			msg.Add(pAffiche);
			msg.SendToSm(CMessage::GetSMSocket(SMID));

			//! 保存日志
		}
	}
}

//! 删除一个GM的管理信息
VOID GmSystem::DelListen(LONG UserID)
{
	map<LONG, tagListenInfo>::iterator iteU = m_UserToSm.find(UserID);
	if (m_UserToSm.end() != iteU)
	{
		EndScoutPlayer(NULL, iteU->second._WatchPlayerName.c_str(), UserID);
		m_SmToUser.erase(iteU->second._ServerID._64Key);
		m_UserToSm.erase(iteU);
	}
}

//! 找到GM管理的一个WS
GmSystem::tagListenInfo* GmSystem::FindGmWs(LONG UserID, eGMTPurviewType TestPurview)
{
	map<LONG, tagListenInfo>::iterator iteU = m_UserToSm.find(UserID);
	if (m_UserToSm.end() != iteU)
	{
		if(iteU->second._pUser->HasPurview(TestPurview))
			return &(iteU->second);
	}
	return 0;
}