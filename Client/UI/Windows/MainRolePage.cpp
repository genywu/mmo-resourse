#include "StdAfx.h"  
#include "../UIDef.h"  
#include "../CEGUI/cegui/include/CEGUIWindowManager.h"
#include "../CEGUI/cegui/include/CEGUIWindow.h"   
#include "../../GameClient/Game.h"
#include "../../ui/UIManager.h"
#include "../../GameClient/ClientApp/Player.h"      

CEGUI::Window*  InitPlayerRolePage();
bool  OnRoleAttr(const CEGUI::EventArgs& e);

#define MAX_ZANIAN 100

CEGUI::Window*  InitPlayerRolePage()
{
    CEGUI::Window* RolePage = LoadUI("MainRole");

    RolePage->setVisible(true);
    RolePage->setAlwaysOnTop(true);
    
    RolePage->setPosition( CEGUI::UVector2(cegui_absdim(0), cegui_absdim(0) ) );

    RolePage->subscribeEvent("RoleAttr",CEGUI::Event::Subscriber(OnRoleAttr));

    return RolePage;
}

bool  OnRoleAttr(const CEGUI::EventArgs& e)
{
    CPlayer* player = GetGame()->GetMainPlayer();
    char Format[128] = "";
    float curVal = 0.0;

    CEGUI::DefaultWindow* pLevel = WDefaultWindow( GetWndMgr().getWindow("RolePage/Level") );
    long lLev = player->GetLevel();
    sprintf_s<128>(Format,"Lv:%d",lLev);
    pLevel->setText(Format);

    CEGUI::DefaultWindow* pName = WDefaultWindow(GetWndMgr().getWindow("RolePage/Name"));
    const char * name = player->GetName();
    SetText(pName,name);

    /// pHp
    CEGUI::ProgressBar*  pHp  = WProgress( GetWndMgr().getWindow("RolePage/ProHp"));
    long  lHp = player->GetHp();
    long  lMaxHp = player->GetMaxHp();
    sprintf_s<128>(Format,"%d/%d",lHp,lMaxHp);   
    if( lMaxHp!=0 )
    {
        curVal = float(lHp) / float(lMaxHp);
    }
    pHp->setProgress( curVal );
    pHp->setText( Format );

    /// pMp
    CEGUI::ProgressBar*  pMp  = WProgress( GetWndMgr().getWindow("RolePage/ProMp"));
    long  lMp = player->GetMp();
    long  lMaxMp = player->GetMaxMp();
    sprintf_s<128>(Format,"%d/%d",lMp,lMaxMp);

    if( lMaxMp!=0 )
    {
        curVal = float(lMp) / float(lMaxMp);
    }
    pMp->setProgress( curVal );
    pMp->setText( Format );
    //pMp-SetFocus();

	/***********************************************************************/
	/*  zhaohang 2010-11-3 add ÔÓÄîÖµ*/
	/***********************************************************************/
	long lZaNian = player->GetProperty("lDistract");
	long lZao = player->GetProperty("lDryNum");
	long lEndTime = player->GetProperty("lDryEndTime");

	CEGUI::ProgressBar* pZaNian = WProgress(GetWndMgr().getWindow("RolePage/ProZaNian"));
	if( MAX_ZANIAN !=0)
	{
		curVal = float(lZaNian) / float(MAX_ZANIAN);
	}
	pZaNian->setProgress(curVal);
	sprintf_s<128>(Format,"%d/%d",lZaNian,MAX_ZANIAN);
	pZaNian->setText(Format);
	return true;
}