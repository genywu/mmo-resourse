#include "StdAfx.h"  
#include "FactionPageDef.h"
#include "../../UIDef.h"  
#include "../../CEGUI/cegui/include/CEGUIWindowManager.h"
#include "../../CEGUI/cegui/include/CEGUIWindow.h"   
#include "../../../GameClient/Game.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../UIManager.h"
#include "../../../GameClient/ClientApp/organizingsystem/Faction.h"
#include "../../../GameClient/ClientApp/organizingsystem/FactionPageOper.h"
#include "../../../GameClient/ClientApp/organizingsystem/Union.h"

bool  OnOpenFactionApplyPage(const CEGUI::EventArgs& e); 
bool  OnApplyListSelectionPage(const CEGUI::EventArgs& e);
bool  OnAgreeJoinFaction(const CEGUI::EventArgs& e);
bool  OnRefuseJoinFaction(const CEGUI::EventArgs& e);

CEGUI::Window*  InitFactionApplyPage()
{
    CEGUI::Window * ApplyList = LoadUI("FactionApplyList");

    ApplyList->setVisible(false);
    ApplyList->setAlwaysOnTop(true);

    ApplyList->subscribeEvent("Opened",CEGUI::Event::Subscriber(OnOpenFactionApplyPage));

    CEGUI::MultiColumnList * qlist = static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionApplyList/ApplyList"));
    qlist->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged,CEGUI::Event::Subscriber(OnApplyListSelectionPage));

    /// 确定
    CEGUI::PushButton * BuOk  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionApplyList/BtnAccept"));
    /// 取消
    CEGUI::PushButton * BuCancel  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionApplyList/BtnRefuse"));

    BuOk->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnAgreeJoinFaction));
    BuOk->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnRefuseJoinFaction));
    return ApplyList;
}

bool  OnOpenFactionApplyPage(const CEGUI::EventArgs& e)
{
    /// 确定
    CEGUI::PushButton * BuOk  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionApplyList/BtnAccept"));
    if ( BuOk )
    {
        BuOk->disable();
    }
    /// 取消
    CEGUI::PushButton * BuCancel  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionApplyList/BtnRefuse"));
    if ( BuCancel )
    {
        BuCancel->disable();
    }

    CEGUI::MultiColumnList * qlist = static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionApplyList/ApplyList"));
    qlist->resetList();
    if( CFaction* pMyFaction = GetGame()->GetFactionManager()->GetMyFaction() )
    {
        std::list<CFacMember*>*	pApplyList = pMyFaction->GetListOfApply();
        long i = 0 ;
        for ( std::list<CFacMember*>::iterator p = pApplyList->begin() ; p != pApplyList->end() ; p++,i++)
        {
            qlist->addRow() ;

            UpFactionPageListAppData(qlist, (*p)->GetName(),0,i,*p);
            UpFactionPageList(qlist, CEGUI::PropertyHelper::intToString((*p)->GetLevel()),1,i);
            UpFactionPageList(qlist, "0",2,i);
            UpFactionPageList(qlist, (*p)->GetOccuText(),3,i);   ///流派制定为职业

        }
    }
    return true;
}

bool  OnApplyListSelectionPage(const CEGUI::EventArgs& e)
{
    /// 确定
    CEGUI::PushButton * BuOk  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionApplyList/BtnAccept"));
    /// 取消
    CEGUI::PushButton * BuCancel  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionApplyList/BtnRefuse"));
    CEGUI::MultiColumnList * qlist = static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionApplyList/ApplyList"));
    CEGUI::ListboxTextItem * sel =  static_cast<CEGUI::ListboxTextItem*>( qlist->getFirstSelectedItem() );
    if( !sel ) 
    {
        BuOk->disable();                                                                                                           
        BuCancel->disable();
        return true;
    }
    BuOk->enable();                                                                                                           
    BuCancel->enable();
    return true;
}


bool  OnAgreeJoinFaction(const CEGUI::EventArgs& e)
{
    CEGUI::MultiColumnList * qlist = static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionApplyList/ApplyList"));
    CEGUI::ListboxTextItem * sel =  static_cast<CEGUI::ListboxTextItem*>( qlist->getFirstSelectedItem() );
    if( !sel )    return false;
    CFacMember* SelMem = static_cast<CFacMember*>( sel->getUserData() );
    if( !SelMem ) return false;
    GetInst(FactionOper).PushApplyInfo( SelMem->GetGUID(),TRUE );

    return true;
}

bool  OnRefuseJoinFaction(const CEGUI::EventArgs& e)
{
    CEGUI::MultiColumnList * qlist = static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionApplyList/ApplyList"));
    CEGUI::ListboxTextItem * sel =  static_cast<CEGUI::ListboxTextItem*>( qlist->getFirstSelectedItem() );
    if( !sel )    return false;
    CFacMember* SelMem = static_cast<CFacMember*>( sel->getUserData() );
    if( !SelMem ) return false;
    GetInst(FactionOper).PushApplyInfo( SelMem->GetGUID(),FALSE );

    return false;
}