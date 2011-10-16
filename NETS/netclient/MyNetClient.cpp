#include "StdAfx.h"
#include "mynetclient.h"
#include "message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyNetClient::CMyNetClient(void)
{
}

CMyNetClient::~CMyNetClient(void)
{
}

void CMyNetClient::HandleReceive()
{
}

void CMyNetClient::HandleClose()
{
	// ·þÎñÆ÷¶ÏÏß
	CMessage* msg;
	msg = new CMessage(MSG_C2C_LOG_SERVERLOST);
	GetRecvMessages()->PushMessage(msg);
}

void CMyNetClient::HandleConnect()
{
}

void CMyNetClient::SetSendRevBuf()
{
	return;
}