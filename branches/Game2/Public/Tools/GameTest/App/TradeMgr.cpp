


#include "stdafx.h"
#include "TradeMgr.h"

CTradeMgr::CTradeMgr()
{

}

CTradeMgr::~CTradeMgr()
{

}

void CTradeMgr::StartTrade(const CGUID& sessionGuid,const CGUID& myGuid,long myPlugID,const CGUID& youGuid,long youPlugID)
{
	m_SessionGuid = sessionGuid;

	m_MyGuid = myGuid;
	m_YourGuid = youGuid;
	m_lMyPlugID = myPlugID;
	m_lYouPlugID = youPlugID;
}