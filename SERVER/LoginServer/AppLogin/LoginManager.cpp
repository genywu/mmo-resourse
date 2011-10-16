

#include "stdafx.h"
#include "LoginManager.h"
#include "..\nets\netlogin\message.h"


LoginManager::LoginManager(VOID)
:m_dwMaxWsMum(0)
,m_dwProcessRankInterval(0xFFFFFFFF)	
,m_dwSendQueueMsgInterval(0xFFFFFFFF)
,m_dwOneProcessNum(0)
,m_cqueueValidatingCdKey(tagAccountInfo_ToAS())
,m_dwLastProcessTime(0)
,m_dwLastSendQueueMsgTime(0)
,m_dwClearIpLoginTime(0)
,m_dwClearIpCreateTime(0)
{}
LoginManager::~LoginManager(VOID)
{}


//! 改变设置
BOOL LoginManager::ChangeSetup(DWORD dwMaxWsMum, DWORD dwProcessRankInterval, DWORD dwSendQueueMsgInterval, DWORD dwOneProcessNum)
{
	m_dwMaxWsMum				= dwMaxWsMum;
	m_dwProcessRankInterval		= dwProcessRankInterval;
	m_dwSendQueueMsgInterval	= dwSendQueueMsgInterval;
	m_dwOneProcessNum			= dwOneProcessNum;

	return TRUE;
}

//! 改变IP限制设置
BOOL LoginManager::ChangeIpSetup(tagLimitIpSetup &LimitIpSetup)
{
	memcpy(&m_LimitIpSetup, &LimitIpSetup, sizeof(tagLimitIpSetup));
	m_dwClearIpLoginTime = 0;
	m_dwClearIpCreateTime = 0;
	m_cqueueIpLoginCount.clear();
	m_cqueueIpCreateCount.clear();
	return TRUE;
}

//! 重读不排队账号
BOOL LoginManager::ReLoadNoQueue(VOID)
{
	m_setNotQueueCdKey.clear();
	ifstream ListFile;
	ListFile.open( "NoQueueCdkeyList.txt" ,ios_base::in);
	if( ListFile.fail() )
	{
		ListFile.close();
		AddLogText("Can't open NoQueueCdkeyList.txt");
		return FALSE;
	}
	char strTempt[256];
	while (!ListFile.eof())
	{
		ListFile >> strTempt;
		strlwr(strTempt);
		m_setNotQueueCdKey.insert(strTempt);
	}

	AddLogText("Load the No Queue Cdkey OK.");

	return TRUE;
}

//! 添加一个WS排队
BOOL LoginManager::AddWsWaitQueue(LONG lWsID)
{
	map<LONG, WaitInWsQueue>::iterator iteWS = m_mapAllWaitInWsQueue.find(lWsID);
	if(m_mapAllWaitInWsQueue.end() != iteWS)
		return FALSE;

	m_mapAllWaitInWsQueue[lWsID] = WaitInWsQueue();

#ifndef USE_GAME_WS_PLAYER_NUM
	map<LONG, set<string>>::iterator iteInGame = m_mapInGamePlayer.find(lWsID);
	if(m_mapInGamePlayer.end() != iteInGame)
		return FALSE;

	m_mapInGamePlayer[lWsID] = set<string>();
#endif

	return TRUE;
}

//! 删除一个WS排队
BOOL LoginManager::DelWsWaitQueue(LONG lWsID)
{

#ifndef USE_GAME_WS_PLAYER_NUM
	m_mapInGamePlayer.erase(lWsID);
#endif

	map<LONG, WaitInWsQueue>::iterator iteWS = m_mapAllWaitInWsQueue.find(lWsID);
	if(m_mapAllWaitInWsQueue.end() != iteWS)
	{
		//! 发送服务器关闭消息
		LONG lTxtCode = LOGIN_WORLD_SERVER_BE_CLOSED;
		iteWS->second.each_run(SendAnswerCdkeyMsg, &lTxtCode);

		m_mapAllWaitInWsQueue.erase(iteWS);
		return TRUE;
	}

	return FALSE;
}

//! 得到一个WS的进入队列
LoginManager::WaitInWsQueue* LoginManager::GetWaitInWsQueue(LONG lWsID)
{
	map<LONG, WaitInWsQueue>::iterator iteWS = m_mapAllWaitInWsQueue.find(lWsID);
	if(m_mapAllWaitInWsQueue.end() != iteWS)
		return &(iteWS->second);

	return NULL;
}

//! 处理排队等待的玩家
VOID LoginManager::ProcessQueue(VOID)
{
	DWORD dwCurrTime = timeGetTime();
	if(m_dwLastProcessTime + m_dwProcessRankInterval > dwCurrTime)
		return;
	m_dwLastProcessTime = dwCurrTime;

	//! 处理防沉迷计时
	OnTimeOutFcmCdkey(time(NULL));

	map<LONG, WaitInWsQueue>::iterator iteWS = m_mapAllWaitInWsQueue.begin();
	for (; iteWS != m_mapAllWaitInWsQueue.end(); ++iteWS)
	{
#ifdef _RUNSTACKINFO_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"处理一个WS的排队 Start");
#endif
		WaitInWsQueue &CurrWaitInWsQueue = iteWS->second;

		//! WS人数m_dwLastProcessTime
		DWORD dwWsCurrNum = 0;
#ifdef USE_GAME_WS_PLAYER_NUM
		dwWsCurrNum = GetGame()->GetLoginWorldPlayerNumByWorldID(iteWS->first);
#else
		map<string, set<string>>::iterator iteInGame = m_mapInGamePlayer.find(string(pWsName));
		if(m_mapInGamePlayer.end() != iteInGame)
			dwWsCurrNum = iteInGame->second.size();
		else
			assert(false);
#endif

		QueueWsCanInGamePlayer &WsCanInGamePlayer = m_mapCanInGamePlayer[iteWS->first];

#ifdef _RUNSTACKINFO_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"清除在选人界面不动的超时玩家");
#endif
		//! 清除在选人界面不动的超时玩家
		DWORD dwCerrTime = timeGetTime();
		DWORD dwEndTime = WsCanInGamePlayer.get_front()._dwEndTime;
		while (dwCerrTime > dwEndTime && 0 != dwEndTime)
		{
			WsCanInGamePlayer.pop_front();
			dwEndTime = WsCanInGamePlayer.get_front()._dwEndTime;
		}


		dwWsCurrNum += (DWORD)WsCanInGamePlayer.size();//! 加上正在登陆的人数

		DWORD dwCanLoginNum = (m_dwMaxWsMum > dwWsCurrNum) ? m_dwMaxWsMum - dwWsCurrNum : 0;
		//! 最多只处理限制人数的登陆数量
		dwCanLoginNum = (dwCanLoginNum > m_dwOneProcessNum) ? m_dwOneProcessNum : dwCanLoginNum;

#ifdef _RUNSTACKINFO_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"放玩家进游戏 Start");
#endif
		//! 放玩家进游戏
		if(0 < dwCanLoginNum)
		{
			for (LONG i = 0; i < dwCanLoginNum; ++i)
			{
				tagAccountInfo_ToAS &AccountInfo_ToAS = CurrWaitInWsQueue.get_front();
				if(0 == AccountInfo_ToAS._dwIp)
					break;
				if(!GetGame()->EnterToSelect(AccountInfo_ToAS._szCdkey, AccountInfo_ToAS._szWSName, AccountInfo_ToAS._dwIp, AccountInfo_ToAS._lSocket))
				{
					//! 登陆失败
					CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
					msg.Add((LONG)LOGIN_PLAYER_LOAD_ERROR);
					msg.SendToClient(AccountInfo_ToAS._lSocket);
				}
				else
				{
					QueueWsCanInGamePlayer &WsCanInGamePlayer = m_mapCanInGamePlayer[AccountInfo_ToAS._lWsID];
					BOOL bRe = WsCanInGamePlayer.find_and_erase(string(AccountInfo_ToAS._szCdkey));
					assert(!bRe);
					AccountInfo_ToAS._dwEndTime = timeGetTime() + GetGame()->GetLoginSetup()->m_SelectWaitTimeSpace;
					WsCanInGamePlayer.push_back(string(AccountInfo_ToAS._szCdkey), AccountInfo_ToAS);

					//! 登陆成功消息
					CMessage msg(MSG_L2C_LOG_WAITDATALOAD);
					msg.SendToClient(AccountInfo_ToAS._lSocket);
				}
				CurrWaitInWsQueue.pop_front();
			}
		}

#ifdef _RUNSTACKINFO_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"放玩家进游戏 End");
#endif
		//! 
		if(m_dwLastSendQueueMsgTime + m_dwSendQueueMsgInterval < dwCurrTime)
		{
#ifdef _RUNSTACKINFO_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"发送排队消息");
#endif
			//! 发送等待次序更新消息
			LONG lOrder = 1;
			CurrWaitInWsQueue.each_run(SendWaitMsg, &lOrder);

			m_dwLastSendQueueMsgTime = dwCurrTime;
		}
#ifdef _RUNSTACKINFO_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"处理一个WS的排队 End");
#endif
	}
}

//! 输出登陆调试信息
VOID LoginManager::OutLoginInfo(VOID)
{
	char szTmpInfo[10240] = {0};
	sprintf(szTmpInfo, "------------------------------------------------\r\n");
	//! 正在验证的账号<账号， 验证超时时间>
	sprintf(&szTmpInfo[strlen(szTmpInfo)], "正在验证的账号[%d]个\r\n", m_cqueueValidatingCdKey.size());
	//! 通过验证的账号
	sprintf(&szTmpInfo[strlen(szTmpInfo)], "可以请求基本信息的玩家有[%d]个\r\n", m_cqueuePassedAccount.size());
	//! 所有WS的排队列表
	map<LONG, WaitInWsQueue>::iterator ite = m_mapAllWaitInWsQueue.begin();
	for (; m_mapAllWaitInWsQueue.end() != ite; ++ite)
	{
		sprintf(&szTmpInfo[strlen(szTmpInfo)], "	WS[%d]上的排队列表有[%d]个\r\n", ite->first, ite->second.size());
	}
	//! 一分钟内从WS退出的账号（用于小退验证）
	sprintf(&szTmpInfo[strlen(szTmpInfo)], "正在小退过程中的账号有[%d]个\r\n", m_cqueueJustOutCdKey_FromWS.size());

	//! 可以进入游戏的玩家
	sprintf(&szTmpInfo[strlen(szTmpInfo)], "可以请求详细信息的玩家有[%d]个\r\n", (LONG)m_mapCanInGamePlayer.size());
	//! 为防沉迷账号的计时记录
	sprintf(&szTmpInfo[strlen(szTmpInfo)], "进行防沉迷计时的玩家有[%d]个\r\n", (LONG)m_multimapFcmTimeCdkey.size());

	AddLogText(szTmpInfo);
}

//! 发送等待消息给玩家
VOID LoginManager::SendWaitMsg(tagAccountInfo_ToAS &AccountInfo_ToAS, LPVOID pParam)
{
	CMessage msg(MSG_L2C_LOG_UPDATEQUEUENUM);
	msg.Add(*((DWORD*)pParam));
	msg.SendToClient(AccountInfo_ToAS._lSocket);

	*((DWORD*)pParam) += 1;
}

//! 发送响应登陆消息给玩家
VOID LoginManager::SendAnswerCdkeyMsg(tagAccountInfo_ToAS &AccountInfo_ToAS, LPVOID pParam)
{
	CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
	msg.Add(*((DWORD*)pParam));
	msg.SendToClient(AccountInfo_ToAS._lSocket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! 响应客户端登陆请求
VOID LoginManager::OnClientLoginRequest(tagAccountInfo_ToAS &AccountInfo_ToAS)
{
	string strCdkey(AccountInfo_ToAS._szCdkey);
	//! 查看是否冻结登陆
	map<string, DWORD>::iterator ite = m_mapFrozenCdkey.find(strCdkey);
	if(m_mapFrozenCdkey.end() != ite)
	{
		DWORD lCurrTime = time(NULL);
		if(GetGame()->GetLoginSetup()->m_FrostCdkeyTime > lCurrTime - ite->second)
		{
			CMessage msg(MSG_L2C_LOG_FROZEN_CDKEY);
			msg.Add(GetGame()->GetLoginSetup()->m_FrostCdkeyTime - (lCurrTime - ite->second));
			msg.SendToClient(AccountInfo_ToAS._lSocket);
			return;
		}

		//! 超时解冻
		m_mapFrozenCdkey.erase(ite);
	}

	DWORD dwCerrTime = timeGetTime();

	//! 如果账号正在输入密保，则不允许再次登陆
	if(m_mapLSSecurityData.end() != m_mapLSSecurityData.find(strCdkey))
	{
		tagAccountInfo_ToAS &AccountInfo = m_cqueueValidatingCdKey.find_and_get(strCdkey);
		if(dwCerrTime >= AccountInfo._dwEndTime)
		{
			GetGame()->KickOut(AccountInfo._lSocket);
			m_cqueueValidatingCdKey.find_and_erase(strCdkey);
			m_mapLSSecurityData.erase(strCdkey);
		}

		CMessage msg(MSG_L2C_LOG_SECURITY_VALIDATINT);
		msg.SendToClient(AccountInfo_ToAS._lSocket);
		return;
	}

	m_cqueueValidatingCdKey.find_and_erase(strCdkey);
	m_mapLSSecurityData.erase(strCdkey);

	//! 清理超时的请求
	const tagAccountInfo_ToAS *pAccountInfo = &(m_cqueueValidatingCdKey.get_front());
	while (dwCerrTime > pAccountInfo->_dwEndTime && 0 != pAccountInfo->_dwEndTime)
	{
		if(m_mapLSSecurityData.end() == m_mapLSSecurityData.find(pAccountInfo->_szCdkey) && !pAccountInfo->_bLocalValidate)
		{
			tagAccountInfo_ToAS newAccountInfo(*pAccountInfo);
			m_cqueueValidatingCdKey.pop_front();

			string strTmpCdkey(newAccountInfo._szCdkey);
			newAccountInfo._dwEndTime = dwCerrTime + GetGame()->GetLoginSetup()->m_ValidateWaitTimeSpace;

#ifdef ENABLE_LOCAL_VALIDATE
			if(GetGame()->GetAccountClient().LocalValidate(newAccountInfo))
			{
				m_cqueueValidatingCdKey.push_back(strTmpCdkey, newAccountInfo);
			}
			else
#endif
			{
				m_mapLSSecurityData.erase(strTmpCdkey);
				GetGame()->KickOut(newAccountInfo._lSocket);
			}
		}
		else
		{
			GetGame()->KickOut(pAccountInfo->_lSocket);
			m_mapLSSecurityData.erase(pAccountInfo->_szCdkey);
			m_cqueueValidatingCdKey.pop_front();
		}

		pAccountInfo = &(m_cqueueValidatingCdKey.get_front());
	}

	//! 第一次登陆的玩家
	if (!m_cqueueValidatingCdKey.find(strCdkey))
	{
		//! 加入等待列表
		AccountInfo_ToAS._dwEndTime = dwCerrTime + GetGame()->GetLoginSetup()->m_ValidateWaitTimeSpace;

		if(GetGame()->GetAccountClient().ValidateAccount(AccountInfo_ToAS))
		{
			m_cqueueValidatingCdKey.push_back(strCdkey, AccountInfo_ToAS);
			CMessage::MapCDkeySocket(AccountInfo_ToAS._szCdkey, AccountInfo_ToAS._lSocket);
		}
		else
		{
			GetGame()->KickOut(AccountInfo_ToAS._lSocket);
		}
	}
}

//! 响应客户端小退后的登陆请求
VOID LoginManager::OnClientAgainLoginRequest(tagAccountInfo_ToAS &AccountInfo_ToAS, DWORD dwServerKey[])
{
	if(IsJustOutPlayer(AccountInfo_ToAS._szCdkey, AccountInfo_ToAS._lWsID, dwServerKey, TRUE))
	{
		const char *pWSName = GetGame()->GetWorldNameByID(AccountInfo_ToAS._lWsID);
		if(NULL == pWSName)
			return;
		strcpy(AccountInfo_ToAS._szWSName, pWSName);
		if(0 == GetGame()->EnterGame(
			AccountInfo_ToAS._szCdkey,
			AccountInfo_ToAS._dwIp,
			AccountInfo_ToAS._lSocket,
			pWSName
			)
			)
		{
			QueueWsCanInGamePlayer &WsCanInGamePlayer = m_mapCanInGamePlayer[AccountInfo_ToAS._lWsID];
			assert(!WsCanInGamePlayer.find(string(AccountInfo_ToAS._szCdkey)));
			AccountInfo_ToAS._dwEndTime = timeGetTime() + GetGame()->GetLoginSetup()->m_SelectWaitTimeSpace;
			WsCanInGamePlayer.push_back(string(AccountInfo_ToAS._szCdkey), AccountInfo_ToAS);
			CMessage::MapCDkeySocket(AccountInfo_ToAS._szCdkey, AccountInfo_ToAS._lSocket);
		}
		return;
	}
}

//! 添加一个正在验证的账号（用于PT验证）
VOID LoginManager::AddValidating(tagAccountInfo_ToAS &AccountInfo_ToAS)
{
	AccountInfo_ToAS._bLocalValidate = true;
	AccountInfo_ToAS._dwEndTime = timeGetTime() + GetGame()->GetLoginSetup()->m_ValidateWaitTimeSpace;

	m_cqueueValidatingCdKey.push_back(AccountInfo_ToAS._szCdkey, AccountInfo_ToAS);
	CMessage::MapCDkeySocket(AccountInfo_ToAS._szCdkey, AccountInfo_ToAS._lSocket);
}

//! 响应客户端输入安全信息
VOID LoginManager::OnClientSecurityInput(const char *pCdKey, const char arrCard[], LONG lSocketID)
{
	if(NULL == pCdKey || NULL == arrCard)
		return;

	map<string, tagLSSecurityData>::iterator ite = m_mapLSSecurityData.find(pCdKey);
	if (m_mapLSSecurityData.end() != ite)
	{
		if(lSocketID == ite->second._AccountInfo_FromAS._lSocket)
		{
			BOOL bPass = TRUE;
			for (LONG i = 0; i < SECURITY_VALIDATE_POS_NUM; ++i)
			{
				if(SECURITY_CARD_SIZE <= ite->second.arrPos[i])
				{
					bPass = FALSE;
					break;
				}

				if(arrCard[i] != ite->second._arrSecurityCard[ite->second.arrPos[i]])
				{
					bPass = FALSE;
					break;
				}
			}

			if(bPass)
			{
				m_mapPwdErrLog.erase(ite->second._AccountInfo_FromAS._szCdkey);
				OnValidateReturn(tagAccountInfo_FromAS(ite->second._AccountInfo_FromAS), 1);
			}
			else
			{
				if(!OnUserPwdErr(ite->second._AccountInfo_FromAS))
					SendSecurityToC(ite->second);
			}

			return;
		}
	}

	GetGame()->KickOut(lSocketID);
}

//! 响应客户端请求角色基本信息列表
VOID LoginManager::OnClientQuestPlayerBaseList(const char *pCdKey, const char *pWsName)
{
	assert(NULL != pCdKey && NULL != pWsName);
#ifdef _RUNSTACKINFO_
	BOOL bNormalClear = FALSE;
#endif
	do 
	{
		tagAccountInfo_ToAS &AccountInfo_ToAS = m_cqueuePassedAccount.find_and_get(string(pCdKey));
		if(0 == AccountInfo_ToAS._dwIp)//! IP为0，说明没有找到对象
		{
			//! 处理非法登陆
			GetGame()->KickOut(pCdKey);
#ifdef _OUT_LOGIN_FLOW_INFO_
			AddLogText("玩家[%s]请求基本角色信息时，还不存在于已通过验证的列表名单中，弹出玩家！", pCdKey);
#endif
			break;
		}

		assert(-1 == AccountInfo_ToAS._lWsID);
		LONG lWsID = GetGame()->GetWorldIDByName(pWsName);
		if(-1 == lWsID)
		{
			//! 处理非法登陆
			GetGame()->KickOut(pCdKey);
#ifdef _OUT_LOGIN_FLOW_INFO_
			AddLogText("玩家[%s]请求基本角色信息时，发送了非法的WS名字信息，弹出玩家！", pCdKey);
#endif
			break;
		}
		AccountInfo_ToAS._lWsID = lWsID;

		if(!GetGame()->WsIsReady(lWsID))
		{
			CMessage msg(MSG_L2C_LOG_ANSWER_PLAYERBASELIST);
			// 添加验证代码
			msg.Add((long)0xFFFFFFFF);
			msg.Add((BYTE)0);// flag
			msg.Add((pCdKey));// cdkey
			msg.Add((short)0);// flag
			msg.SendToClient(AccountInfo_ToAS._lSocket);
#ifdef _OUT_LOGIN_FLOW_INFO_
			AddLogText("玩家[%s]请求基本角色信息时，对应的WS还没有准备好！", pCdKey);
#endif
			break;
		}

		//! 放入WS等待队列
		if(WS_NAME_SIZE > strlen(pWsName))
			strcpy(AccountInfo_ToAS._szWSName, pWsName);
		else
		{
			//! 配置了错误的WS数据，WS名字过长
			assert(false);
			AddLogText("Server setup error!!!, World's name too long!!!");
			GetGame()->KickOut(pCdKey);
			break;
		}

		//! 如果是不排队玩家
		if(IsNotQueueCdKey(pCdKey))
		{
			if(GetGame()->EnterToSelect(
				AccountInfo_ToAS._szCdkey,
				AccountInfo_ToAS._szWSName,
				AccountInfo_ToAS._dwIp,
				AccountInfo_ToAS._lSocket))
			{
				QueueWsCanInGamePlayer &WsCanInGamePlayer = m_mapCanInGamePlayer[AccountInfo_ToAS._lWsID];
				assert(!WsCanInGamePlayer.find(string(AccountInfo_ToAS._szCdkey)));
				AccountInfo_ToAS._dwEndTime = timeGetTime() + GetGame()->GetLoginSetup()->m_SelectWaitTimeSpace;
				WsCanInGamePlayer.push_back(string(AccountInfo_ToAS._szCdkey), AccountInfo_ToAS);
			}
		}
		else
		{
			WaitInWsQueue *pWaitInWsQueue = GetWaitInWsQueue(AccountInfo_ToAS._lWsID);
			if(NULL == pWaitInWsQueue)
			{
				//! 非法登陆、WS不存在
				GetGame()->KickOut(pCdKey);
#ifdef _OUT_LOGIN_FLOW_INFO_
				AddLogText("玩家[%s]请求基本角色信息时，没有找到所请求的WS，属于非法消息，弹出玩家！", pCdKey);
#endif
			}
			else
			{
				pWaitInWsQueue->push_back(string(pCdKey), AccountInfo_ToAS);
#ifdef _OUT_LOGIN_FLOW_INFO_
				bNormalClear = TRUE;
#endif
				//! 发送第一次排队信息
				LONG uPlayerOrder = pWaitInWsQueue->size();
				DWORD dwWsCurrNum = GetGame()->GetLoginWorldPlayerNumByWorldID(AccountInfo_ToAS._lWsID);
				dwWsCurrNum += m_mapCanInGamePlayer[AccountInfo_ToAS._lWsID].size();
				if(1 < uPlayerOrder || m_dwMaxWsMum <= dwWsCurrNum)
					SendWaitMsg(AccountInfo_ToAS, &uPlayerOrder);
			}
		}
	} 
	while(false);

#ifdef _OUT_LOGIN_FLOW_INFO_
	if(bNormalClear)
	{
		AddLogText("玩家[%s]发送基本角色信息，正常清除CD-KEY！", pCdKey);
	}
#endif
	m_cqueuePassedAccount.erase(string(pCdKey));
}

//! 响应客户端请求详细数据
VOID LoginManager::OnClientQuestPlayerData(LONG lWsID, const char *pCdKey, CGUID& PlayerID, DWORD dwSignCode)
{
	assert(NULL != pCdKey);
	QueueWsCanInGamePlayer &WsCanInGamePlayer = m_mapCanInGamePlayer[lWsID];
	tagAccountInfo_ToAS &AccountInfo_ToAS = WsCanInGamePlayer.find_and_get(string(pCdKey));
	if(AccountInfo_ToAS._dwEndTime != 0 && timeGetTime() < AccountInfo_ToAS._dwEndTime)
	{
		GetGame()->L2W_QuestDetail_Send(
			AccountInfo_ToAS._szWSName,
			AccountInfo_ToAS._szCdkey,
			PlayerID,
			AccountInfo_ToAS._dwIp,
			dwSignCode
			);
	}
	else
	{
		//! 非法请求
		GetGame()->KickOut(pCdKey);
		return;
	}
}

//! 响应客户端断开连接
VOID LoginManager::OnClientNetClose(const char *pCdKey, BOOL bInWs)
{
	assert(NULL != pCdKey);
	m_cqueueValidatingCdKey.erase(string(pCdKey));
	m_mapLSSecurityData.erase(string(pCdKey));

#ifdef _OUT_LOGIN_FLOW_INFO_
	if(m_cqueuePassedAccount.find(pCdKey))
		AddLogText("响应客户端[%s]断开，从已通过验证的队列中清除该客户端", pCdKey);
#endif
	m_cqueuePassedAccount.erase(string(pCdKey));

	map<LONG, WaitInWsQueue>::iterator iteWS = m_mapAllWaitInWsQueue.begin();
	for (; iteWS != m_mapAllWaitInWsQueue.end(); ++iteWS)
		iteWS->second.erase(string(pCdKey));

	map<LONG, QueueWsCanInGamePlayer>::iterator iteC = m_mapCanInGamePlayer.begin();
	for (; iteC != m_mapCanInGamePlayer.end(); ++iteC)
		iteC->second.erase(string(pCdKey));

	if(!bInWs)
	{
		if(GetGame()->UseAsFcm())
		{
			DelFcmCdkey(pCdKey);

			CMessage msg(MSG_O2A_ACCOUNT_Logout);
			msg.Add(pCdKey);
			msg.SendToAS();

		}
		else
		{
			GetGame()->GetCSdoaFcm().UserOffline(pCdKey);
		}
	}
}

//! 响应AS验证结果（uValidateType：0=AS验证结果，1=局部验证结果 2=盛大验证结果）
VOID LoginManager::OnValidateReturn(tagAccountInfo_FromAS &AccountInfo_FromAS, ULONG uValidateType)
{
	if(ePLR_Lost != AccountInfo_FromAS._LoginRerult._dwLoginResult)
	{
		DWORD dwRe = OnIpLogin(AccountInfo_FromAS._dwIp, AccountInfo_FromAS._szCdkey);
		if(1 != dwRe)	
		{
			if(0 != dwRe)
			{
				//! 提示客户端
				CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
				msg.Add((LONG)dwRe);
				msg.SendToClient(AccountInfo_FromAS._lSocket);
				//! 日志
				UCHAR arrIp[4] = {0};
				LONG lIP = AccountInfo_FromAS._dwIp;
				memmove(arrIp, &lIP, sizeof(LONG));

				char szTmp[128] = {0};
				sprintf(szTmp, "登陆验证完成时请求时IP被首次限制：IP：%u.%u.%u.%u，账号：%s", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3], AccountInfo_FromAS._szCdkey);
				PutStringToFile("LimitIpLog", szTmp);
			}	
			return;
		}

		m_mapPwdErrLog.erase(AccountInfo_FromAS._szCdkey);
		if(m_cqueueValidatingCdKey.find_and_erase(AccountInfo_FromAS._szCdkey) || 2 == uValidateType)
		{
			m_mapLSSecurityData.erase(AccountInfo_FromAS._szCdkey);

			LONG lWsID = GetGame()->GetWorldIDByName(AccountInfo_FromAS._szWSName);
			AccountInfo_FromAS._lWsID = lWsID;

			//! 清理超时的CDKEY
			LONG dwCerrTime = timeGetTime();
			LONG dwEndTime = m_cqueuePassedAccount.get_front()._dwEndTime;
			while (dwCerrTime > dwEndTime && 0 != dwEndTime)
			{
				m_cqueuePassedAccount.pop_front();
				dwEndTime = m_cqueuePassedAccount.get_front()._dwEndTime;
			}

			//! 
			if (!m_cqueuePassedAccount.find(string(AccountInfo_FromAS._szCdkey)))
			{
				//! 
				if(0 == GetGame()->ReadyToEnter(
					AccountInfo_FromAS._szCdkey,
					AccountInfo_FromAS._dwIp,
					AccountInfo_FromAS._lSocket,
					AccountInfo_FromAS._szWSName)
					) 
				{
#ifdef SHOW_LOGIN_MSG_20090817
					if(GetGame()->OutInfo())
					{
						AddLogText("玩家【Account：%s】通过验证，进入等待列表", AccountInfo_FromAS._szCdkey);
					}
#endif
					//! 加入通过验证列表
					AccountInfo_FromAS._dwEndTime = dwCerrTime + GetGame()->GetLoginSetup()->m_ValidateWaitTimeSpace;
					m_cqueuePassedAccount.push_back(string(AccountInfo_FromAS._szCdkey), AccountInfo_FromAS);
				}
			}
		}
		else
		{
			//! 验证超时处理
			if(GetGame()->UseAsFcm())
			{
				CMessage msg(MSG_O2A_ACCOUNT_Logout);
				msg.Add(AccountInfo_FromAS._szCdkey);
				msg.SendToAS();
			}
			else
			{

			}
		}

		//! 如果是本地验证的，则不需要保存了
		if(0 != uValidateType)
			GetGame()->GetAccountClient().SaveAccount(AccountInfo_FromAS, NULL);

	}
	else
	{
		OnValidateReturnLost(AccountInfo_FromAS);
	}
}

//! 响应验证失败
VOID LoginManager::OnValidateReturnLost(tagAccountInfo_FromAS &AccountInfo_FromAS)
{
	// 密码错误
	if(LOGIN_PASSWORDERROR == AccountInfo_FromAS._LoginRerult._InfoCode)
	{
		if(OnUserPwdErr(AccountInfo_FromAS))
			return;
	}
	else
	{

	}


	CMessage msg(MSG_L2C_LOG_ANSWERCDKEY);
	msg.Add((LONG)AccountInfo_FromAS._LoginRerult._InfoCode);
	msg.SendToClient(AccountInfo_FromAS._lSocket);

	m_cqueueValidatingCdKey.erase(AccountInfo_FromAS._szCdkey);
	m_mapLSSecurityData.erase(AccountInfo_FromAS._szCdkey);
}

//! 响应用户密码输入错误，返回账户是否已经冻结
BOOL LoginManager::OnUserPwdErr(tagAccountInfo_FromAS &AccountInfo_FromAS)
{
	//! 记录连续错误次数
	DWORD dwCurr = time(NULL);
	map<string, pair<DWORD, DWORD>>::iterator ite = m_mapPwdErrLog.find(string(AccountInfo_FromAS._szCdkey));
	if (m_mapPwdErrLog.end() != ite)
	{
		if(GetGame()->GetLoginSetup()->m_PwdErrSaveTime > dwCurr - ite->second.first)
		{
			if(GetGame()->GetLoginSetup()->m_MaxPwdErrCount <= ite->second.second + 1)
			{
				//! 冻结登陆
				assert(m_mapFrozenCdkey.end() == m_mapFrozenCdkey.find(ite->first));
				m_mapFrozenCdkey.insert(pair<string, DWORD>(ite->first, dwCurr));
				m_mapPwdErrLog.erase(ite);

				CMessage msg(MSG_L2C_LOG_FROZEN_CDKEY);
				msg.Add(GetGame()->GetLoginSetup()->m_FrostCdkeyTime);
				msg.SendToClient(AccountInfo_FromAS._lSocket);
				return TRUE;
			}
			else
			{
				++(ite->second.second);
				return FALSE;
			}
		}
	}

	m_mapPwdErrLog[string(AccountInfo_FromAS._szCdkey)] = pair<DWORD, DWORD>(dwCurr, 1);
	return FALSE;
}

//! 响应安全信息通知
VOID LoginManager::OnSecurityData(tagAccountInfo_FromAS &AccountInfo_FromAS, tagSecurityData &SecurityData, BOOL bIsLocalValidate)
{
	//! 到这里，说明除了安全验证，其他都通过了
	DWORD dwRe = OnIpLogin(AccountInfo_FromAS._dwIp, AccountInfo_FromAS._szCdkey);
	if(1 != dwRe)	
	{
		if(0 != dwRe)
		{
			//! 提示客户端
			CMessage msg( MSG_L2C_LOG_ANSWERCDKEY );
			msg.Add((LONG)dwRe);
			msg.SendToClient(AccountInfo_FromAS._lSocket);
			//! 日志
			UCHAR arrIp[4] = {0};
			LONG lIP = AccountInfo_FromAS._dwIp;
			memmove(arrIp, &lIP, sizeof(LONG));

			char szTmp[128] = {0};
			sprintf(szTmp, "登陆验证完成时请求时IP被首次限制：IP：%u.%u.%u.%u，账号：%s", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3], AccountInfo_FromAS._szCdkey);
			PutStringToFile("LimitIpLog", szTmp);
		}	
		return;
	}

	tagAccountInfo_ToAS &mySaveInfo = m_cqueueValidatingCdKey.find_and_get(AccountInfo_FromAS._szCdkey);
	if(0 == strcmp(mySaveInfo._szCdkey, AccountInfo_FromAS._szCdkey))
	{
		tagLSSecurityData LSSecurityData(SecurityData, AccountInfo_FromAS);
		m_mapLSSecurityData[mySaveInfo._szCdkey] = LSSecurityData;
		SendSecurityToC(m_mapLSSecurityData[mySaveInfo._szCdkey]);

		if(!bIsLocalValidate)
			GetGame()->GetAccountClient().SaveAccount(AccountInfo_FromAS, &SecurityData);
	}
}

//!						发送安全验证信息到客户端
VOID LoginManager::SendSecurityToC(tagLSSecurityData& LSSecurityData)
{
	set<CHAR> setTmp;
	for (ULONG i = 0; i < SECURITY_VALIDATE_POS_NUM; ++i)
	{
		CHAR cPos = rand() % SECURITY_CARD_SIZE;
		while(setTmp.end() != setTmp.find(cPos))
		{
			cPos = rand() % SECURITY_CARD_SIZE;
		}
		LSSecurityData.arrPos[i] = cPos;
		setTmp.insert(cPos);
	}

	CMessage msg(MSG_L2C_LOG_SECURITY_VALIDATE);
	msg.AddEx(LSSecurityData.arrPos, SECURITY_VALIDATE_POS_NUM);
	msg.Add(GetGame()->GetLoginSetup()->m_ValidateWaitTimeSpace);
	msg.SendToClient(LSSecurityData._AccountInfo_FromAS._lSocket);
}


//! 响应WS返回详细数据成功
VOID LoginManager::OnWsAnswerPlayerDataSucceed(const char *pCdKey, LONG lWsID)
{
	assert(NULL != pCdKey);
	m_mapCanInGamePlayer[lWsID].erase(string(pCdKey));

#ifndef USE_GAME_WS_PLAYER_NUM
	map<string, set<string>>::iterator iteInGame = m_mapInGamePlayer.find(string(pWsName));
	if(m_mapInGamePlayer.end() != iteInGame)
		iteInGame->second.insert(string(pCdKey));
#endif
}


//! 响应WS发送的玩家退出游戏消息
VOID LoginManager::OnWsPlayerQuit(const char *pCdKey, LONG lWsID, DWORD dwServerKey[])
{
	if(0 != dwServerKey[0])
	{
		tagWSIDAndPwd WSIDAndPwd;
		WSIDAndPwd._dwEndTime	= timeGetTime() + GetGame()->GetLoginSetup()->m_JustOutTimeSpace;
		memcpy(WSIDAndPwd._dwServerKey, dwServerKey, sizeof(DWORD) * SERVER_KEY_DWORD_SIZE);
		WSIDAndPwd._lWSID		= lWsID;

		m_cqueueJustOutCdKey_FromWS.push_back(string(pCdKey), WSIDAndPwd);
		//! 玩家小退，说明之前的冻结是有其他的客户端引起，在这里给小退账号解冻，基本能够保证账号能够小退成功
		m_mapFrozenCdkey.erase(pCdKey);
	}

#ifndef USE_GAME_WS_PLAYER_NUM
	map<string, set<string>>::iterator iteInGame = m_mapInGamePlayer.find(string(pWsName));
	if(m_mapInGamePlayer.end() != iteInGame)
		iteInGame->second.erase(string(pCdKey));
#endif
}

//! 判断是否为小退玩家
BOOL LoginManager::IsJustOutPlayer(const char *pCdKey, LONG &lWsID, DWORD dwServerKey[], LONG bDel)
{
	assert(NULL != pCdKey);
	m_cqueueJustOutCdKey_FromWS.get_front();
	//! 清理超时的请求
	DWORD dwCerrTime = timeGetTime();
	tagWSIDAndPwd *pWSIDAndPwd = &m_cqueueJustOutCdKey_FromWS.get_front();
	while (dwCerrTime > pWSIDAndPwd->_dwEndTime && 0 != pWSIDAndPwd->_dwEndTime)
	{
		m_cqueueJustOutCdKey_FromWS.pop_front();
		pWSIDAndPwd = &m_cqueueJustOutCdKey_FromWS.get_front();
	}

	BOOL bRe = FALSE;
	pWSIDAndPwd = &m_cqueueJustOutCdKey_FromWS.find_and_get(string(pCdKey));
	if(0 != pWSIDAndPwd->_dwEndTime)
	{
		if(0 == memcmp(pWSIDAndPwd->_dwServerKey, dwServerKey, sizeof(DWORD) * SERVER_KEY_DWORD_SIZE))
		{
			lWsID = pWSIDAndPwd->_lWSID;
			bRe = TRUE;
		}
		//! 验证完是否清除
		if(bDel && bRe)
			m_cqueueJustOutCdKey_FromWS.erase(string(pCdKey));
	}
	return bRe;
}

//! 判断是否为不排队的玩家
BOOL LoginManager::IsNotQueueCdKey(const char *pCdKey)
{
	return (m_setNotQueueCdKey.end() != m_setNotQueueCdKey.find(string(pCdKey)));
}

//! 添加一个防沉迷账号的计时
VOID LoginManager::AddFcmCdkey(const char *pCdKey, DWORD dwHoldSecond)
{
	DelFcmCdkey(pCdKey);

	DWORD dwEndTime = dwHoldSecond + time(NULL);
	m_mapCdkeyToFcmTime[string(pCdKey)]= dwEndTime;
	m_multimapFcmTimeCdkey.insert(make_pair(dwEndTime, string(pCdKey)));
}

//! 删除一个防沉迷账号的计时
VOID LoginManager::DelFcmCdkey(const char *pCdKey)
{
	map<string, DWORD>::const_iterator iteCdkey = m_mapCdkeyToFcmTime.find(string(pCdKey));
	if(m_mapCdkeyToFcmTime.end() != iteCdkey)
	{
		typedef multimap<DWORD, string>::iterator ITE;
		typedef pair<ITE, ITE> Range;
		Range range = m_multimapFcmTimeCdkey.equal_range(iteCdkey->second);
		for (ITE ite = range.first; range.second != ite; ++ite)
		{
			if(0 == stricmp(ite->second.c_str(), pCdKey))
			{
				m_multimapFcmTimeCdkey.erase(ite);
				return;
			}
		}
	}
}

//! 删除一个时间点的所有的防沉迷账号计时
VOID LoginManager::OnTimeOutFcmCdkey(DWORD dwCurrTime)
{
	multimap<DWORD, string>::iterator ite = m_multimapFcmTimeCdkey.begin();

	while (m_multimapFcmTimeCdkey.end() != ite)
	{
		if(ite->first > dwCurrTime)
			break;

		string strCdkey(ite->second);

		//! 通知WS踢玩家下线
		LONG lWSID = GetGame()->FindCdkey(strCdkey.c_str());
		if(-1 != lWSID)
		{
			CMessage msgToWorldServer( MSG_L2W_LOG_FCM_BEGIN );
			msgToWorldServer.Add( strCdkey.c_str() );
			GetGame() -> SendMsg2World( &msgToWorldServer, lWSID);
		}

		if(GetGame()->UseAsFcm())
		{
			CMessage msg(MSG_O2A_ACCOUNT_Logout);
			msg.Add(ite->second.c_str());
			msg.SendToAS();
		}
		else
		{

		}

		//! 从计时队列中移除
		m_mapCdkeyToFcmTime.erase(strCdkey);
		ite = m_multimapFcmTimeCdkey.erase(ite);

		//! LS自己也踢
		GetGame()->KickOut(strCdkey.c_str());
	}
}

//! 清除所有计时
VOID LoginManager::ClearFcm(VOID)
{
	m_mapCdkeyToFcmTime.clear();
	m_multimapFcmTimeCdkey.clear();
}


//! 响应一个IP登陆账号，返回值（0=执行失败  1=允许登陆 LOGIN_IP_LOGIN_MAX_SAME_CDKEY, LOGIN_IP_LOGIN_NIMIETY_CDKEY,LOGIN_IP_CREATE_MAX）
DWORD LoginManager::OnIpLogin(LONG lIP, LPCTSTR pCdKey)
{
	if(NULL == pCdKey)
		return 0;

	if(!m_LimitIpSetup.m_bIpLogin)
		return 1;

	const DWORD dwCerrTime = timeGetTime();
	if(m_dwClearIpLoginTime <= dwCerrTime || 0 == m_dwClearIpLoginTime)
	{
		m_cqueueIpLoginCount.clear();
		m_dwClearIpLoginTime = dwCerrTime + m_LimitIpSetup.m_ClearIpLoginTime;
	}

	map<LONG, map<string, LONG>>::iterator iteIpInfo = m_cqueueIpLoginCount.find(lIP);
	if(m_cqueueIpLoginCount.end() == iteIpInfo)
	{
		m_cqueueIpLoginCount.insert(make_pair(lIP, map<string, LONG>()));
		m_cqueueIpLoginCount[lIP].insert(make_pair(string(pCdKey), 1L));
		return 1;
	}

	//! 检测同一IP登陆的不同账号数
	BOOL bIsFull = FALSE;
	if(m_LimitIpSetup.m_MaxIpLoginCdKeyCount <= iteIpInfo->second.size())
		bIsFull = TRUE;

	map<string, LONG>::iterator iteCdkey = iteIpInfo->second.find(pCdKey);
	if(iteIpInfo->second.end() == iteCdkey)
	{
		if(bIsFull)
			return LOGIN_IP_LOGIN_NIMIETY_CDKEY;
		iteIpInfo->second.insert(make_pair(string(pCdKey), 1L));
		return 1;
	}

	//! 检测同一IP登陆的同一账号次数
	if(m_LimitIpSetup.m_MaxIpLoginOncCdKeyCount <= iteIpInfo->second[pCdKey])
		return LOGIN_IP_LOGIN_MAX_SAME_CDKEY;

	iteIpInfo->second[pCdKey] += 1;
	return 1;	
}
//! 响应一个IP创建角色，返回值（0=执行失败  LOGIN_IP_LOGIN_MAX_SAME_CDKEY, LOGIN_IP_LOGIN_NIMIETY_CDKEY,LOGIN_IP_CREATE_MAX）
DWORD LoginManager::OnIpCreate(LONG lIP)
{
	if(!m_LimitIpSetup.m_bIpCreate)
		return 1;

	const DWORD dwCerrTime = timeGetTime();
	if(m_dwClearIpCreateTime <= dwCerrTime || 0 == m_dwClearIpLoginTime)
	{
		m_cqueueIpCreateCount.clear();
		m_dwClearIpCreateTime = dwCerrTime + m_LimitIpSetup.m_ClearIpCreateTime;
	}

	map<LONG, LONG>::iterator iteIpInfo = m_cqueueIpCreateCount.find(lIP);
	if(m_cqueueIpCreateCount.end() == iteIpInfo)
	{
		m_cqueueIpCreateCount.insert(make_pair(lIP, 1L));
		return 1;
	}

	if(m_LimitIpSetup.m_MaxIpCreateCount <= iteIpInfo->second)
		return LOGIN_IP_CREATE_MAX;

	m_cqueueIpCreateCount[lIP] += 1;
	return 1;
}

//!  返回值（0=执行失败 1=允许登陆 LOGIN_IP_LOGIN_MAX_SAME_CDKEY, LOGIN_IP_LOGIN_NIMIETY_CDKEY,LOGIN_IP_CREATE_MAX）
DWORD LoginManager::IsFrozenIp(LONG lIP, LPCTSTR pCdKey, DWORD dwCurrTime)
{
	if(NULL == pCdKey)
		return 0;

	if(m_LimitIpSetup.m_bIpCreate)
	{
		map<LONG, map<string, LONG>>::iterator iteIpInfo = m_cqueueIpLoginCount.find(lIP);
		if(m_cqueueIpLoginCount.end() != iteIpInfo)
		{
			DWORD dwCanAddNew = 1;
			if(m_LimitIpSetup.m_MaxIpLoginCdKeyCount <= iteIpInfo->second.size())
				dwCanAddNew = LOGIN_IP_LOGIN_NIMIETY_CDKEY;

			map<string, LONG>::iterator iteCdkey = iteIpInfo->second.find(pCdKey);
			if(iteIpInfo->second.end() != iteCdkey)
			{
				if(m_LimitIpSetup.m_MaxIpLoginOncCdKeyCount <= iteIpInfo->second[pCdKey])
					return LOGIN_IP_LOGIN_MAX_SAME_CDKEY;
			}
			else
				return dwCanAddNew;
		}
	}

	return 1;
}