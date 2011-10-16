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

namespace nFactionMemInfo
{
    CFacMember *g_FacMemInfo = NULL;
}

bool  OnOpenFactionMemInfoPage(const CEGUI::EventArgs& e); 
bool  OnCloseFactionMemInfoPage(const CEGUI::EventArgs& e);

CEGUI::Window*  InitFactionMemInfoPage()
{
    CEGUI::Window * MemInfo = LoadUI("FactionMemInfo");

    MemInfo->setVisible(false);
    MemInfo->setAlwaysOnTop(true);

    MemInfo->subscribeEvent("Opened",CEGUI::Event::Subscriber(OnOpenFactionMemInfoPage));
    MemInfo->subscribeEvent("Closed",CEGUI::Event::Subscriber(OnCloseFactionMemInfoPage));

    return MemInfo;
}

bool  OnOpenFactionMemInfoPage(const CEGUI::EventArgs& e)
{
    /// 密语
    CEGUI::PushButton * Chat  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionMemInfo/BtnChat"));
    /// 邀请组队
    CEGUI::PushButton * Team  = static_cast<CEGUI::PushButton*>( GetWndMgr().getWindow("FactionMemInfo/BtnTeam"));

    if ( nFactionMemInfo::g_FacMemInfo != NULL )
    {
        CEGUI::DefaultWindow * Name  = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/Name"));
        Name->setText( ToCEGUIString(nFactionMemInfo::g_FacMemInfo->GetName()) );                      
        
        CEGUI::DefaultWindow * Level = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/Level"));
        Level->setText( CEGUI::PropertyHelper::intToString(nFactionMemInfo::g_FacMemInfo->GetLevel()) );

        CEGUI::DefaultWindow * dValue= WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/dvalue"));
        dValue->setText("0");

        CEGUI::DefaultWindow * genre = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/genre"));
        genre->setText(ToCEGUIString("流派"));

        CEGUI::DefaultWindow * Map   = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/map"));
        Map->setText( ToCEGUIString(nFactionMemInfo::g_FacMemInfo->GetRegion()) );

        CEGUI::DefaultWindow * Job   = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/Job"));
        Job->setText( ToCEGUIString(nFactionMemInfo::g_FacMemInfo->GetJob() == -1 ? "帮主":"成员") );

        CEGUI::DefaultWindow * Title = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/title"));
        Title->setText( ToCEGUIString(nFactionMemInfo::g_FacMemInfo->GetTitle()) );

        CEGUI::DefaultWindow * sValue= WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/skillvalue"));
        sValue->setText( CEGUI::PropertyHelper::intToString(nFactionMemInfo::g_FacMemInfo->GetAchievement()) );

        CEGUI::DefaultWindow * sCon  = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/contribute"));
        sCon->setText( CEGUI::PropertyHelper::intToString(nFactionMemInfo::g_FacMemInfo->GetContribute()) );

        CEGUI::DefaultWindow * sTime = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/lasttime"));

        char pstr[512];
        long lTime = nFactionMemInfo::g_FacMemInfo->GetLastTime();
        time_t tTime = lTime;
        tm * t = localtime( &tTime );
        sprintf_s(pstr,"%d/%d/%d %d:%d:%d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
        sTime->setText( ToCEGUIString(pstr) );

        CEGUI::DefaultWindow * Brief = WDefaultWindow(GetWndMgr().getWindow("FactionMemInfo/Brief"));
        sprintf_s(pstr,"%s 的个人简介",nFactionMemInfo::g_FacMemInfo->GetName());
        Brief->setText( ToCEGUIString(pstr) );
        
    }
    return true;
}

bool  OnCloseFactionMemInfoPage(const CEGUI::EventArgs& e)
{
    nFactionMemInfo::g_FacMemInfo = NULL;
    return true;
}
