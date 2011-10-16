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
#include "../../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"

bool  OnOpenCreateFactionPage(const CEGUI::EventArgs& e); 
bool  OnBtnCreateFaction(const CEGUI::EventArgs& e); 
bool  OnBtnCancelCreatFaction(const CEGUI::EventArgs& e); 

CEGUI::Window*  InitCreateFactionPage()
{
    CEGUI::Window * creat = LoadUI("CreateFaction");

    creat->setVisible(false);
    creat->setAlwaysOnTop(true);

    creat->subscribeEvent("Opened",CEGUI::Event::Subscriber(OnOpenCreateFactionPage));
    
    CEGUI::PushButton * BtnCreate  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("CreateFaction/BtnCreate"));
    BtnCreate->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnBtnCreateFaction));

    CEGUI::PushButton * BtnCancel  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("CreateFaction/BtnCancel"));
    BtnCancel->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnBtnCancelCreatFaction));

    return creat;
}

bool  OnOpenCreateFactionPage(const CEGUI::EventArgs& e)
{
    /// 创建
    CEGUI::PushButton * BtnCreate  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("CreateFaction/BtnCreate"));
    /// 取消
    CEGUI::PushButton * Cancel  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("CreateFaction/BtnCancel"));

    return true;
}

bool  OnBtnCreateFaction(const CEGUI::EventArgs& e)
{
    CEGUI::DefaultWindow * stext = static_cast<CEGUI::DefaultWindow*>(GetWndMgr().getWindow("CreateFaction/FactionName"));
    CEGUI::DefaultWindow * pcard = static_cast<CEGUI::DefaultWindow*>(GetWndMgr().getWindow("CreateFaction/MultPcard"));
             
    if ( pcard->getTextVisual().length() > MAX_PronounceCharNum )
    {
        FireUIEvent("CreateFaction","Closed");
        return false;
    }
    else if ( stext->getTextVisual().length() <= 0 )
    {
        GetInst(MsgEventManager).PushEvent(Msg_Ok,"帮会名字太短!");//307
        return false;
    }
         
    GetGame()->GetFactionManager()->SendCreateMsg(stext->getTextVisual().c_str(),pcard->getTextVisual().c_str());
    FireUIEvent("CreateFaction","Closed");
    return true;
}

bool  OnBtnCancelCreatFaction(const CEGUI::EventArgs& e)
{
    FireUIEvent("CreateFaction","Closed");
    return true;
}
