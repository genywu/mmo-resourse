#include "stdafx.h"
#include "mmsystem.h"
#include "message.h"
#include "Mysocket.h"
#include "MyNetServer_Client.h"
#include "MyNetServerClient_Client.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMyNetServer_Client::CMyNetServer_Client(void)
{
	m_lTotalSendSize = 0;
	m_lMaxIPOperatorNum = 5;
	m_lPermitMaxClientSendBufSize = 1024*1024*4;
}

CMyNetServer_Client::~CMyNetServer_Client(void)
{
}

CServerClient* CMyNetServer_Client::CreateServerClient()
{
	return new CMyNetServerClient_Client(this);
}

void CMyNetServer_Client::OnMapStrError(const char* strMap)
{
	CMessage* msg = new CMessage(MSG_L2L_LOG_CLIENTCLOSE);
	msg->Add((char*)strMap);
	GetRecvMessages()->PushMessage(msg);
}