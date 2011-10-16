/*ÏÝÚå nikai 2009/3/13*/
#include "stdafx.h"
#include "CPet.h"
#include "CTrap.h"
#include "../GameClient/Game.h"
#include "../ClientApp/Player.h"
CTrap::CTrap()
{
	m_PetType = PET_TYPE_TRAP;
	counter = 10;
}

CTrap::~CTrap()
{
	
}

void CTrap::AI()
{
	CMoveShape::AI();
	if (m_MasterGUID != GetGame()->GetMainPlayer()->GetExID() && !m_bIsActived)
		return;
	char Info[256] = "";
	static DWORD begin = GetCurTickCount();
	
	if (counter < 0)
	{
		counter = 10;
	}
	if (GetCurTickCount() - begin >= 1000 )
	{
		begin = GetCurTickCount();
		//sprintf(Info,"ÏÝÚåÏûÊ§µ¹¼ÆÊ±£º %ld Ãë", --counter);
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(3,Info,0xffff0000);
	}
	
}

bool CTrap::Display()
{
	if( !CPet::Display() )
		return false;
	return true;
}

bool CTrap::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodeFromDataBlock(setReadDB,bExData);
	if(!ret)
		MessageBox(g_hWnd,"Pet CTrap Decoded failed!!","error",MB_OK);
	m_bIsActived = true;
	return true;
}
