#include "stdafx.h"
#include "../dbentity/EntityManager.h"



void OnDBOPMessage(CMessage* pMsg)
{
	// 该消息必定处于一次session中
	GetGame()->GetEntityManager()->ProcessMsg(pMsg);
}
