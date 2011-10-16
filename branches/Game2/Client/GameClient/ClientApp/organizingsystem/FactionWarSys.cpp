//////////////////////////////////////////////////////////////////////////
//FactionWarSys.cpp
//Function:帮战系统相关功能
//Author:Joe
//Create Time:2005.6.27
//////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "Factionwarsys.h"

#include "Faction.h"
#include "Union.h"
#include "NoOrganizing.h"
#include "OrganizingCtrl.h"
#include "Player.h"
#include "Message.h"
#include "../GameClient/Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFactionWarSys*		CFactionWarSys::pInstance = NULL;

CFactionWarSys::CFactionWarSys(void)
{
}

CFactionWarSys::~CFactionWarSys(void)
{
}

CFactionWarSys* CFactionWarSys::GetInstance()
{
	if(pInstance == NULL)
	{
		pInstance = new CFactionWarSys;
	}
	return pInstance;
}

void CFactionWarSys::Release()
{
	if(pInstance)
	{
		SAFE_DELETE(pInstance);
	}
}


CFactionWarSys* GetFactionWarSys()
{
	return CFactionWarSys::GetInstance();
}

//行会宣战
bool CFactionWarSys::DigUpTheHatchet(LONG64 lNetSessionID,long lPassword,const CGUID& FactionID,long lLevl)
{
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if(pPlayer==NULL)	return false;
	
	//如果自己没有行会，则退出
	if(pPlayer->GetFactionID() == CGUID::GUID_INVALID)
		return false;

	//判断是否可以向此帮宣战
	if( GetOrganiCtrl()->IsDecWarFaction(FactionID) == false )
		return false;
	
	//CMessage msg(MSG_C2S_ORGSYS_DigUpTheHatchet);
	//msg.Add(lNetSessionID);
	//msg.Add(lPassword);
	//msg.Add(FactionID);
	//msg.Add(lLevl);
	//msg.Send();


	//设置在交战中
	GetOrganiCtrl()->SetDecWarFactionRelation(FactionID,3);
	return true;
}

//关闭宣战会话
void CFactionWarSys::CloseDecWarSession(LONG64 lNetSessionID,long lPassword)
{
	//CMessage msg(MSG_C2S_ORGSYS_DigUpTheHatchet);
	//msg.Add(lNetSessionID);
	//msg.Add(lPassword);
	//msg.Add((long)0);
	//msg.Send();
}