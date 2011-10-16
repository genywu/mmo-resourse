



#include "StdAfx.h"
#include "COnCmdConnect.h"






COnCmdConnect::COnCmdConnect(VOID)
:m_dwPort(0)
,m_bWorking(false)
,m_sListen(-1)
{

}

COnCmdConnect::~COnCmdConnect(VOID)
{
	Release();
}


//! 初始化
BOOL COnCmdConnect::Init(VOID)
{
	string strTemp;
	ifstream streamGsInfo;
	streamGsInfo.open("GsInfo.ini");
	if( streamGsInfo.is_open() )
	{
		streamGsInfo
			>> strTemp >> m_strNumber
			>> strTemp >> m_bWorking
			>> strTemp >> m_dwPort;

		streamGsInfo.close();
		AddLogText("Load GsInfo.ini file...ok!");
	}
	else
	{
		MessageBox(NULL, "Can't find file GsInfo.ini", "message", MB_OK);
		return FALSE;
	}

	if(m_bWorking && -1 == m_sListen)
		GetGame()->GetAsynchronousExecute().Execute(Watch, this);
	return TRUE;
}

//! 释放对象
VOID COnCmdConnect::Release(VOID)
{
	if(-1 != m_sListen)
	{
		closesocket(m_sListen);
		m_sListen = -1;
	}
	m_bWorking = false;
}


//! 开始监视
LPVOID COnCmdConnect::Watch(LPVOID pThis)
{
	COnCmdConnect *pOnCmdConnect = (COnCmdConnect*)pThis;

	//! 这里默认网络底层已经初始化好了环境

	pOnCmdConnect->m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (SOCKET_ERROR == pOnCmdConnect->m_sListen)
	{
		return FALSE;
	}

	sockaddr_in local;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	if (local.sin_addr.s_addr == INADDR_NONE)
		return FALSE;

	local.sin_family = AF_INET;
	local.sin_port = htons(pOnCmdConnect->m_dwPort);

	
	if (bind(pOnCmdConnect->m_sListen, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
		return FALSE;

	listen(pOnCmdConnect->m_sListen, 8);

	while (pOnCmdConnect->m_bWorking)
	{
		SOCKET	sClient;
		INT32	iAddrSize = 0;
		sockaddr_in client;
		iAddrSize = sizeof(client);
		sClient = accept(pOnCmdConnect->m_sListen, (struct sockaddr *)&client,
			&iAddrSize);        
		if (sClient == INVALID_SOCKET)
		{        
			break;
		}

		pOnCmdConnect->OnConnect(sClient, client);
		
	}
	if(-1 != pOnCmdConnect->m_sListen)
	{
		closesocket(pOnCmdConnect->m_sListen);
		pOnCmdConnect->m_sListen = -1;
	}

	return NULL;
}

//! 响应连接
#define BUF_SIZE 128U
VOID COnCmdConnect::OnConnect(SOCKET sClient, sockaddr_in &client)
{
	char szInfo[2048] = {0};
	m_Lock.Lock();
	{
		for (size_t i = 0; i < m_vGsInfo.size(); ++i)
		{
			sprintf(&szInfo[strlen(szInfo)], "%s/%d/%d/%d;", m_strNumber.c_str(), m_vGsInfo[i].lWSID, m_vGsInfo[i].lGSID, ((m_vGsInfo[i].bGsWorking) ? m_vGsInfo[i].lGSPlayerNum : -1L));
		}
	}
	m_Lock.UnLock();

	if(0 == m_vGsInfo.size())
	{
		sprintf(szInfo, "服务器没有准备好");
	}
	send(sClient, szInfo, (int)strlen(szInfo) + 1, 0);
	closesocket(sClient);
}