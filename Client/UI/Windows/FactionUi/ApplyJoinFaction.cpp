#include "StdAfx.h"  
#include "../../UIDef.h"  
#include "../../CEGUI/cegui/include/CEGUIWindowManager.h"
#include "../../CEGUI/cegui/include/CEGUIWindow.h"   
#include "../../../GameClient/Game.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../UIManager.h"
#include "../../../GameClient/ClientApp/organizingsystem/Faction.h"
#include "../../../GameClient/ClientApp/organizingsystem/FactionPageOper.h"
#include "../../../GameClient/ClientApp/organizingsystem/Union.h"

bool  OnOpenJoninFactionPage(const CEGUI::EventArgs& e); 
bool  OnApplyJoinFaction(const CEGUI::EventArgs& e);

namespace  nApplyJoinFaction
{
    tagApplyInfoList * g_ApplyInfoList = NULL;
}

CEGUI::Window*  InitJoinFactionPage()
{
    CEGUI::Window * Join = LoadUI("ApplyJoinFaction");

    Join->setVisible(false);
    Join->setAlwaysOnTop(true);

    Join->subscribeEvent("Opened",CEGUI::Event::Subscriber(OnOpenJoninFactionPage));

    CEGUI::PushButton * Apply  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("ApplyJoinFaction/BtnApply"));
    Apply->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnApplyJoinFaction));

    return Join;
}

bool  OnOpenJoninFactionPage(const CEGUI::EventArgs& e)
{
    /// …Í«Î
    CEGUI::PushButton * Apply  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("ApplyJoinFaction/BtnApply"));
    /// »°œ˚
    CEGUI::PushButton * Team  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("ApplyJoinFaction/BtnCancel"));

    if ( nApplyJoinFaction::g_ApplyInfoList != NULL )
    {
         CEGUI::DefaultWindow * Name  = WDefaultWindow(GetWndMgr().getWindow("ApplyJoinFaction/FactionName"));
         Name->setText( ToCEGUIString(nApplyJoinFaction::g_ApplyInfoList->info.szName) );

         CEGUI::DefaultWindow * Placard  = WDefaultWindow(GetWndMgr().getWindow("ApplyJoinFaction/MultPcard"));
         Placard->setText( ToCEGUIString(nApplyJoinFaction::g_ApplyInfoList->szPronounce) );
    }

    return true;
}

bool  OnApplyJoinFaction(const CEGUI::EventArgs& e)
{
    if ( nApplyJoinFaction::g_ApplyInfoList == NULL )
        return true;

    GetInst(FactionOper).JoinFactionList(nApplyJoinFaction::g_ApplyInfoList->info.guid);

    return true;
}