#include "StdAfx.h"
#include "myserverclient.h"
#include "nets\netlogin\message.h"
#include <map>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CMyServerClient::CMyServerClient(CServer* pServer)
:CServerClient(pServer)
{
}

CMyServerClient::~CMyServerClient(void)
{
}
