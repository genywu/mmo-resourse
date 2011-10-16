#include "stdafx.h"
#include "mmsystem.h"
#include "Mysocket.h"
#include "MyNetServer_World.h"
#include "MyNetServerClient_World.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMyNetServer_World::CMyNetServer_World(void)
{
	m_lTotalSendSize = 0;
	m_lMaxIPOperatorNum = 100;
	m_lPermitMaxClientSendBufSize = 1024*1024*16;
}

CMyNetServer_World::~CMyNetServer_World(void)
{
}

CServerClient* CMyNetServer_World::CreateServerClient()
{
	return new CMyNetServerClient_World(this);
}