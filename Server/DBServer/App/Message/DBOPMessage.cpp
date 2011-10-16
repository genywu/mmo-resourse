#include "stdafx.h"

#include "../Entity/EntityGroup.h"
#include "../Entity/EntityManager.h"
#include "../../Net/Message.h"
#include "../../Server/Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void OnDBOPMessage(CMessage* pMsg)
{
	GetGame()->GetEntityManager()->ProcessMsg(pMsg);
}
