#include "stdafx.h"
#include <mmsystem.h>
#include "myserveclient.h"
#include "MyNetServer.h"
#include "message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyServerClient::CMyServerClient(void)
{
	m_listRegion.clear();
}

CMyServerClient::~CMyServerClient(void)
{
}

// 连接成功时激活
void CMyServerClient::OnAccept(int errorCode)
{
	// ----------------------
	// 具体游戏处理
	// ----------------------
}


// 与对方断开的时候激活
void CMyServerClient::OnClose(int errorCode)
{
	// 踢掉对应的玩家
	//theApp.GetCheckOutCdkey()->erase(m_strCdkey);
	//处理客户端断线
	//theApp.HandleGameServerClose(GetIP(),GetServerType());

	CMySocket::OnClose();
}

void CMyServerClient::OnReceive(int errorCode)
{
	CServerClient::OnReceive(errorCode);
}
