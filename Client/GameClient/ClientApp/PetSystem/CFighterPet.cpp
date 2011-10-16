/*战宠  nikai 2009/1/10*/
#include "stdafx.h"
#include "CPet.h"
#include "CFighterPet.h"
#include "Player.h"
#include "../GameClient/Game.h"
CFighterPet::CFighterPet()
{
	m_PetType = PET_TYPE_HUNTER;
}

CFighterPet::~CFighterPet()
{

}

void	CFighterPet::AI()
{
	CMoveShape::AI();
	if (m_MasterGUID != GetGame()->GetMainPlayer()->GetExID())
		return;

}

bool CFighterPet::Display()
{
	if( !CPet::Display() )
		return false;
	return true;
}

bool CFighterPet::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CPet::DecodeFromDataBlock(setReadDB,bExData);
	if(!ret)
		MessageBox(g_hWnd,"CFighterPet Decoded failed!!","error",MB_OK);
	//设置等级

	m_bIsActived = true;
	return true;
}