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

bool  OnOpenFactionListPage(const CEGUI::EventArgs& e); 
bool  OnOpenJoinFactionPage(const CEGUI::EventArgs& e);
bool  OnFactionListChangePage(const CEGUI::EventArgs& e);

namespace  nApplyJoinFaction
{
    extern tagApplyInfoList * g_ApplyInfoList;
}

CEGUI::Window*  InitFactionList()
{
    CEGUI::Window * ApplyList = LoadUI("FactionList");

    ApplyList->setVisible(false);
    ApplyList->setAlwaysOnTop(true);

    ApplyList->subscribeEvent("Opened",CEGUI::Event::Subscriber(OnOpenFactionListPage));
    CEGUI::MultiColumnList * qlist = static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionList/FacTab/Tab 1/FacList"));
    qlist->subscribeEvent(CEGUI::Window::EventMouseDoubleClick,CEGUI::Event::Subscriber(OnOpenJoinFactionPage));
    qlist->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged,CEGUI::Event::Subscriber(OnFactionListChangePage));
    return ApplyList;
}

bool  OnOpenFactionListPage(const CEGUI::EventArgs& e)
{
    CEGUI::MultiColumnList * qlist = static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionList/FacTab/Tab 1/FacList"));
    qlist->resetList();
    if( GetGame()->GetFactionManager() )
    {
        std::list<tagApplyInfoList>* Apply = GetGame()->GetFactionManager()->GetListOfApplyInfo();
        long i = 0 ;
        for ( std::list<tagApplyInfoList>::iterator p = Apply->begin() ; p != Apply->end() ; p++,i++)
        {
            qlist->addRow() ;

            UpFactionPageListAppData(qlist, CEGUI::PropertyHelper::intToString(i+1),0,i,(void*)&(*p));
            UpFactionPageList(qlist, CEGUI::PropertyHelper::intToString(i+1001),1,i);
            UpFactionPageList(qlist, (*p).info.szName,2,i);
            UpFactionPageList(qlist, "°ïÖ÷Ãû",3,i);  
            char  forMat[256];
            sprintf_s( forMat ,"%d/%d", (*p).info.lMemberNum,(*p).info.lMaxMemberNum );
            UpFactionPageList(qlist,forMat,4,i);  
            UpFactionPageList(qlist,CEGUI::PropertyHelper::intToString((*p).info.lCurrRes),5,i);
            UpFactionPageList(qlist,"Áª°î",6,i);   
        }
    }
    return true;
}

bool  OnOpenJoinFactionPage(const CEGUI::EventArgs& e)
{
    CEGUI::MultiColumnList * mlist =  static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionList/FacTab/Tab 1/FacList"));
    if ( mlist )
    {
        CEGUI::ListboxTextItem * sel =  static_cast<CEGUI::ListboxTextItem*>( mlist->getFirstSelectedItem() );
        if( !sel )    return false;
        tagApplyInfoList* SelMem = static_cast<tagApplyInfoList*>( sel->getUserData() );
        if( !SelMem ) return false;

        nApplyJoinFaction::g_ApplyInfoList = SelMem;
        FireUIEvent("ApplyJoinFaction","Opened");
    }
    return true;
}

bool  OnFactionListChangePage(const CEGUI::EventArgs& e)
{
    CEGUI::Window * szPronuce= GetWndMgr().getWindow("FactionList/FacTab/Tab 1/FacAffiche");

    CEGUI::MultiColumnList * mlist =  static_cast<CEGUI::MultiColumnList*>(GetWndMgr().getWindow("FactionList/FacTab/Tab 1/FacList"));
    if ( mlist )
    {
        CEGUI::ListboxTextItem * sel =  static_cast<CEGUI::ListboxTextItem*>( mlist->getFirstSelectedItem() );
        if( !sel )    return false;
        tagApplyInfoList* SelMem = static_cast<tagApplyInfoList*>( sel->getUserData() );
        if( !SelMem ) return false;

        szPronuce->setText( ToCEGUIString(SelMem->szPronounce) );
    }

    return true;
}