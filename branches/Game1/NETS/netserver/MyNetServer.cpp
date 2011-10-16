#include "stdafx.h"
#include "nets\netserver\message.h"
#include ".\mynetserver.h"
#include ".\myserverclient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMyNetServer::CMyNetServer(void)
{
	m_lTotalSendSize = 0;
	m_lMaxIPOperatorNum = 3;
	m_lPermitMaxClientSendBufSize = 1024*1024*4;
}

CMyNetServer::~CMyNetServer(void)
{
}

CServerClient* CMyNetServer::CreateServerClient()
{
	return new CMyServerClient(this);
}

void CMyNetServer::OnMapIDError(long lID)
{
	CMessage* msg = new CMessage(MSG_S2S_LOG_PLAYER_ONLOST);
	msg->Add(lID);
	msg->Add("");
	GetRecvMessages()->PushMessage(msg);
}
