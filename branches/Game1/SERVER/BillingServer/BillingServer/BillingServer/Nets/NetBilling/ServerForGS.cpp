#include "stdafx.h"
#include "mmsystem.h"
#include "Mysocket.h"
#include "ServerForGS.h"
#include "ClientForGS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CServerForGS::CServerForGS(void)
{
	m_lTotalSendSize = 0;
	m_lMaxIPOperatorNum = 100;
	m_lPermitMaxClientSendBufSize = 1024*1024*16;
}

CServerForGS::~CServerForGS(void)
{
}

CServerClient* CServerForGS::CreateServerClient()
{
	return new CClientForGS(this);
}