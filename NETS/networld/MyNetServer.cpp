#include "stdafx.h"
#include "mmsystem.h"
#include "MyNetServer.h"
#include "Mysocket.h"
#include "MyServerClient.h"
#include "message.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMyNetServer::CMyNetServer(void)
{
	m_lTotalSendSize = 0;
	m_lMaxIPOperatorNum = 100;
	m_lPermitMaxClientSendBufSize = 1024*1024*32;
}

CMyNetServer::~CMyNetServer(void)
{
}

CServerClient* CMyNetServer::CreateServerClient()
{
	return new CMyServerClient(this);
}


