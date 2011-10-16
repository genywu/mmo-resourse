#include "stdafx.h"
#include "..\nets\netdb\message.h"
#include "..\nets\Servers.h"

#include "..\..\dbserver\game.h"

#include "..\Entity/entityManager.h"

#include "../Entity/EntityGroup.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void OnDBOPMessage(CMessage* pMsg)
{
	GetGame()->GetEntityManager()->ProcessMsg(pMsg);
}
