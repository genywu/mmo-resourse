#include "StdAfx.h"
#include "../../GameClient\game.h"
#include "PlayerCommand.h"
#include "ClientRegion.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CPlayerCommand::Parse(char* txtCmd){

	char chCmd[128];
	char chParam1[128];
	char chBuf[128];

	sscanf(txtCmd,"%s %s",chCmd,chParam1);


	if (strcmp("拒绝交易",chCmd)==0)
	{
		GetGame()->SetTradeAble(!GetGame()->IsTradeAble());
		if (GetGame()->IsTradeAble())
		{
			//GetGame()->GetInterface()->AddText("允许交易");
		}
		else
		{
			//GetGame()->GetInterface()->AddText("拒绝交易");
		}
	}

	else if(strcmp("拒绝",chCmd)==0)
	{
		if(!GetGame()->CheckDisableChatter(chParam1))
		{
			GetGame()->AddDisableChatter(chParam1);
			sprintf(chBuf,"拒绝玩家:%s",chParam1);
			//GetGame()->GetInterface()->AddText(chBuf);
		}
		else
		{
			GetGame()->DelDisableChatter(chParam1);
			sprintf(chBuf,"允许玩家:%s",chParam1);
			//GetGame()->GetInterface()->AddText(chBuf);
		}
	}

	else if(strcmp("Record",chCmd)==0)
	{
		if(!strcmp("On",chParam1))
		{
			//GetGame()->GetCGuiEx()->GetMainBarPageEx()->Record(true);
		}
		else if(!strcmp("Off",chParam1))
		{
			//GetGame()->GetCGuiEx()->GetMainBarPageEx()->Record(false);
		}
	}
	else if(strcmp("FreeCamera",chCmd)==0)
	{
		if(!strcmp("On",chParam1))
		{
			GetGame()->GetSetup()->bfreecamera = true;
			if (GetGame()->GetRegion())
			{
				GetGame()->GetRegion()->GetRegionCamera()->SetMinVertAngle(0);
				GetGame()->GetRegion()->GetRegionCamera()->SetMaxVertAngle(89);
			}
			
		}
		else if(!strcmp("Off",chParam1))
		{
			GetGame()->GetSetup()->bfreecamera = false;
			if (GetGame()->GetRegion())
			{
				GetGame()->GetRegion()->GetRegionCamera()->SetMinVertAngle(30);
				GetGame()->GetRegion()->GetRegionCamera()->SetMaxVertAngle(80);
			}
		}
	}

	else
	{
		return false;
	}
	return true;
}