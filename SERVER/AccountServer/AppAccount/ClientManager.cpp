
#include "stdafx.h"
#include "ClientManager.h"
#include "ValidateOperator.h"
#include "OtherOperator.h"




ClientManager::ClientManager(VOID)
{

}
ClientManager::~ClientManager(VOID)
{

}



//! 初始化对象
BOOL ClientManager::Init(const char *pFilename)
{
	ValidateOperator::Init(GetGame()->GetSetup()->lServerID);
	return LoadClientSetup(pFilename);
}

//! 重读客户端配置
VOID ClientManager::ReLoadClientSetup(const char *pFilename)
{
	if(LoadClientSetup(pFilename))
	{
		CheckOnlineClient();
	}
}

//! 释放对象
VOID ClientManager::Release(VOID)
{
	ValidateOperator::Release();
	OtherOperator::Release();
}



//! 验证一个客户端是否合法
//!		成功时，dwRetrunInfo返回客户端类型
//!		失败时，dwRetrunInfo返回登陆错误类型
BOOL ClientManager::OnClientLogin(DWORD dwIP, DWORD dwNo, DWORD dwSocket, DWORD &dwRetrunInfo)
{
	iteClientInfo iteInfo = m_mapClientInfo.find(dwNo);
	if(m_mapClientInfo.end() == iteInfo)
	{
		dwRetrunInfo = eALET_NotRegister;
		AddLogText("非法客户端登陆，ID[%d]没有在配置文件中找到！", dwNo);
		return FALSE;
	}
	if(iteInfo->second._dwIP != dwIP)
	{
		dwRetrunInfo = eALET_IpDiffer;
		AddLogText("客户端[%d]的IP跟配置有出入，拒绝登陆！", dwNo);
		return FALSE;
	}
	if(iteInfo->second._dwLoginState == eLS_OnLine)
	{
		//! 发现重复登陆，一起踢下线
		GetGame()->DisConnClient(iteInfo->second._dwCurrSocket);
		GetGame()->DisConnClient(dwSocket);
		AddLogText("客户端[%d]已经登陆了，拒绝多次登陆！", dwNo);
		return FALSE;
	}

	//! 通过验证
	iteInfo->second._dwLoginState = eLS_OnLine;
	iteInfo->second._dwCurrSocket = dwSocket;
	dwRetrunInfo = iteInfo->second._dwType;

	m_LockForClientInfo.Lock();
	m_mapMultiClientInfo[dwNo]._dwCurrSocket = dwSocket;
	m_LockForClientInfo.UnLock();
	m_mapOnlineClient[dwSocket] = dwNo;

	char szTmp[128] = {0};
	UCHAR arrIp[4] = {0};
	memmove(arrIp, &dwIP, sizeof(LONG));
	sprintf(szTmp, "%s[%d][%u.%u.%u.%u]", iteInfo->second._szName, dwNo, (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);
	
	AddLoginClientList(szTmp);

	return TRUE;
}

//! 响应客户端退出
BOOL ClientManager::OnClientLogout(DWORD dwSocket)
{
	iteOnlineClient iteOnline = m_mapOnlineClient.find(dwSocket);
	if(m_mapOnlineClient.end() == iteOnline) return FALSE;

	DWORD dwNo = iteOnline->second;
	iteClientInfo iteInfo = m_mapClientInfo.find(iteOnline->second);
	if(m_mapClientInfo.end() == iteInfo)
	{
		AddErrorLogText("未配置的客户端出现在登陆列表中[IP]！！！！");
		GetGame()->DisConnClient(dwSocket);
		assert(FALSE);
		return FALSE;
	}

	m_mapOnlineClient.erase(iteOnline);
	iteInfo->second._dwLoginState = eLS_OffLine;
	iteInfo->second._dwCurrSocket = 0;
#ifdef ENABLE_FCM
	iteInfo->second._bAbleWork = FALSE;
#else
	iteInfo->second._bAbleWork = TRUE;
#endif

	m_LockForClientInfo.Lock();
	m_mapMultiClientInfo[dwNo]._dwCurrSocket = 0;
	m_LockForClientInfo.UnLock();

	char szTmp[128] = {0};
	UCHAR arrIp[4] = {0};
	memmove(arrIp, &(iteInfo->second._dwIP), sizeof(LONG));
	sprintf(szTmp, "%s[%d][%u.%u.%u.%u]", iteInfo->second._szName, iteInfo->second._dwNo, (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);

	DelLoginClientList(szTmp);

	return TRUE;
}

//! 该客户端能否进行操作
DWORD ClientManager::ClientAbleWork(LONG lSocket)
{
	iteOnlineClient iteOnline = m_mapOnlineClient.find(lSocket);
	if(m_mapOnlineClient.end() == iteOnline)
		return 0;

	iteClientInfo iteInfo = m_mapClientInfo.find(iteOnline->second);
	if(m_mapClientInfo.end() == iteInfo)
	{
		AddErrorLogText("未配置的客户端出现在登陆列表中[IP]！！！！");
		GetGame()->DisConnClient(lSocket);
		assert(FALSE);
		return 0;
	}

#ifdef ENABLE_FCM
	return (iteInfo->second._bAbleWork) ? iteInfo->second._dwNo : 0;
#else
	return iteInfo->second._dwNo;
#endif
}


//! 读取客户端配置
BOOL ClientManager::LoadClientSetup(const char *pFilename)
{
	if(NULL == pFilename)
		return FALSE;

	// 读取全局配置
	ifstream stream;
	stream.open(pFilename);
	if( !stream.is_open() )
	{
		AddErrorLogText("%s文件打开失败！", pFilename);
		return FALSE;
	}

	string strTemp;
	stream >> strTemp; //文件注释

	tagClientInfo tmpClientInfo;
	tmpClientInfo._dwLoginState = eLS_OffLine;
	tmpClientInfo._dwCurrSocket = NULL;

	BOOL bRe = TRUE;
	do 
	{
		string strIp;
		string strType;
		stream 
			>> tmpClientInfo._dwNo
			>> strIp
			>> strType
			>> tmpClientInfo._szName;

		//! 转换IP
		tmpClientInfo._dwIP = inet_addr(strIp.c_str());   
		//tmpClientInfo._dwIP = htonl(tmpClientInfo._dwIP);

		//! 转换类型
		if(0 == stricmp("ReadOnly", strType.c_str()))
		{
			tmpClientInfo._dwType = eACT_ReadOnly;
		}
		else if(0 == stricmp("Admin", strType.c_str()))
		{
			tmpClientInfo._dwType = eACT_Admin;
		}
		else
		{
			AddErrorLogText("错误的客户端类型设置“%s”！", strType.c_str());
			bRe = FALSE;
			continue;
		}

		iteClientInfo iteInfo = m_mapClientInfo.find(tmpClientInfo._dwNo);
		if(m_mapClientInfo.end() != iteInfo)
		{
			AddErrorLogText("客户端编号重复“%d”，覆盖之前信息！", tmpClientInfo._dwNo);
			iteInfo->second._dwIP = tmpClientInfo._dwIP;
			iteInfo->second._dwType = tmpClientInfo._dwType;
			memcpy(iteInfo->second._szName, tmpClientInfo._szName, CLIENT_NAME_SIZE);
		}
		else
		{
			m_mapClientInfo[tmpClientInfo._dwNo] = tmpClientInfo;
			m_LockForClientInfo.Lock();
			m_mapMultiClientInfo[tmpClientInfo._dwNo] = tmpClientInfo;
			m_LockForClientInfo.UnLock();
		}
	} 
	while(!stream.eof());

	return bRe;
}

//! 重新检测当前在线客户端是否合法
VOID ClientManager::CheckOnlineClient(VOID)
{
}

//! 得到客户端的Socket
LONG ClientManager::GetClientSocket(DWORD dwNo)
{
	iteClientInfo iteInfo = m_mapClientInfo.find(dwNo);
	if(m_mapClientInfo.end() == iteInfo)
		return 0;
	return iteInfo->second._dwCurrSocket;
}

//! 得到客户端的Socket
LONG ClientManager::MultiGetClientSocket(DWORD dwNo)
{
	LONG lRe = 0;
	m_LockForClientInfo.Lock();

	iteClientInfo iteInfo = m_mapMultiClientInfo.find(dwNo);
	if(m_mapMultiClientInfo.end() != iteInfo)
		lRe = iteInfo->second._dwCurrSocket;

	m_LockForClientInfo.UnLock();
	return lRe;
}

//! 得到客户端的类型
DWORD ClientManager::GetClientType(DWORD dwNo)
{
	iteClientInfo iteInfo = m_mapClientInfo.find(dwNo);
	if(m_mapClientInfo.end() == iteInfo)
		return 0;
	return iteInfo->second._dwType;
}

#ifdef ENABLE_FCM
//! 在线信息修正
VOID ClientManager::OnlineInfoRevise(DBReadSet &dbReadSet, LONG lSocket)
{
	//! 解码得到LS上现有所有账号
	////////////////////////////////////////////////////////////
	LONG lNum = dbReadSet.GetLongFromByteArray();
	
	set<string> setOnLSAccount;
	char szTmpAccount[CDKEY_SIZE] = {0};
	for (LONG i = 0; i < lNum; ++i)
	{
		dbReadSet.GetStringFromByteArray(szTmpAccount, CDKEY_SIZE);
		setOnLSAccount.insert(string(szTmpAccount));
	}

	//! 得到DB中的所有账号
	////////////////////////////////////////////////////////////
	iteOnlineClient iteOnline = m_mapOnlineClient.find(lSocket);
	if(m_mapOnlineClient.end() == iteOnline)
		return;

	set<string> setOnDBAccount;
	OtherOperator::GetLsOnlineAccount(iteOnline->second, setOnDBAccount);

	//! 之前在线，现在仍然在线的账号
	set<string> setOnlineGoonAccount;

	//! 处理异同账号
	////////////////////////////////////////////////////////////
	set<string>::iterator iteLs = setOnLSAccount.begin();
	while (setOnLSAccount.end() != iteLs)
	{
		set<string>::iterator iteDB = setOnDBAccount.find(*iteLs);
		if (setOnDBAccount.end() != iteDB)
		{
			setOnlineGoonAccount.insert(*iteLs);
			setOnDBAccount.erase(iteDB);
			iteLs = setOnLSAccount.erase(iteLs);
		}
		else
			++iteLs;
	}

	//! setOnLSAccount中剩下的是新上线的用户
	for (set<string>::iterator ite = setOnLSAccount.begin(); setOnLSAccount.end() != ite; ++ite)
	{
		OtherOperator *pOtherOperator =	OtherOperator::CreateLoginAndLogout(ite->c_str(), iteOnline->second, eLALT_Login);
		if(NULL != pOtherOperator)
			pOtherOperator->LoginAndLogout();
	}

	//! setOnDBAccount中剩下的是已离线的用户
	for (set<string>::iterator ite = setOnDBAccount.begin(); setOnDBAccount.end() != ite; ++ite)
	{
		OtherOperator *pOtherOperator =	OtherOperator::CreateLoginAndLogout(ite->c_str(), iteOnline->second, eLALT_Logout);
		if(NULL != pOtherOperator)
			pOtherOperator->LoginAndLogout();
	}

	//! 在线状态为变的账号，也需要从新统计时间发送给LS
	for (set<string>::iterator ite = setOnlineGoonAccount.begin(); setOnlineGoonAccount.end() != ite; ++ite)
	{
		OtherOperator *pOtherOperator =	OtherOperator::CreateLoginAndLogout(ite->c_str(), iteOnline->second, eLALT_GetTime);
		if(NULL != pOtherOperator)
			pOtherOperator->LoginAndLogout();
	}

	iteClientInfo iteInfo = m_mapClientInfo.find(iteOnline->second);
	if(m_mapClientInfo.end() == iteInfo)
	{
		AddErrorLogText("未配置的客户端出现在登陆列表中[IP]！！！！");
		GetGame()->DisConnClient(lSocket);
		assert(FALSE);
		return;
	}

	iteInfo->second._bAbleWork = TRUE;
}

#endif