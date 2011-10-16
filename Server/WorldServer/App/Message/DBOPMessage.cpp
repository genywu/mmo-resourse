#include "StdAfx.h"
#include "../DBEntity/EntityManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnDBOPMessage(CMessage* pMsg)
{
	// 该消息必定处于一次session中
	GetGame().GetEntityManager()->ProcessMsg(pMsg);
}
