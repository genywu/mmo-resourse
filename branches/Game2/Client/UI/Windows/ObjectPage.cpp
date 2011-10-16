#include "StdAfx.h"  
#include "../UIDef.h"

#include "../../../Public/Common/MsgType.h"

#include "../CEGUI/cegui/include/CEGUIWindowManager.h"
#include "../CEGUI/cegui/include/CEGUIWindow.h"   
#include "../../GameClient/Game.h"
#include "../../ui/UIManager.h"
#include "../../GameClient/ClientApp/MoveShape.h"
#include "../../GameClient/ClientApp/Monster.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/GameControl.h"
#include "../../GameClient/ClientApp/Team/team.h"
#include "../../GameClient/ClientApp/PhaseSys/PhaseSys.h"
      
/// 事件声明
bool  OnObjectAttr(const CEGUI::EventArgs& e);
bool  OnPopTeamMenu(const CEGUI::EventArgs& e);
bool  OnInivteJoinTeam(const CEGUI::EventArgs& e);
bool  OnLeaveTeam(const CEGUI::EventArgs& e);
bool  OnFollowLeader(const CEGUI::EventArgs& e);
bool  OnAddLinkMan(const CEGUI::EventArgs& e);
bool  OnPrivateChat(const CEGUI::EventArgs& e);
bool  OnInivteJoinPhase(const CEGUI::EventArgs& e);
bool  OnAddBlackList(const CEGUI::EventArgs& e);
bool  OnTraderQuest(const CEGUI::EventArgs& e);
bool  OnObjectClosed(const CEGUI::EventArgs& e);
      
/// 界面声明
CEGUI::Window*  InitTeamMenu();
CEGUI::Window*  InitObjectPage();

CEGUI::Window*  InitObjectPage()
{
    CEGUI::Window*  ObjectPage= LoadUI("ObjectPage");

    ObjectPage->setVisible(false);
    ObjectPage->setAlwaysOnTop(true);

    ObjectPage->setPosition( CEGUI::UVector2(cegui_reldim(0.4f), cegui_reldim(0.0f) ) );

    ObjectPage->subscribeEvent("ObjectAttr",CEGUI::Event::Subscriber(OnObjectAttr));

    ObjectPage->subscribeEvent(EVENT_CLOSE,CEGUI::Event::Subscriber(OnObjectClosed));

    CEGUI::DefaultWindow*  image = WDefaultWindow( ObjectPage->getChildRecursive("ObjectPage/ObjectImage/Name"));
    if ( image )
    {
        image->subscribeEvent(CEGUI::DefaultWindow::EventMouseButtonDown,
            CEGUI::SubscriberSlot(OnPopTeamMenu));
    }

    return ObjectPage;
}

bool  OnObjectAttr(const CEGUI::EventArgs& e)
{
    CMoveShape* shape = static_cast<CMoveShape*>(GetGame()->GetGameControl()->GetSelectedShape());
    if ( !shape ) return false ;

    char Format[128] = "";
    float curVal = 0.0;
    long  lLev = 0;

    CEGUI::DefaultWindow* pLevel = WDefaultWindow( GetWndMgr().getWindow("ObjectPage/ObjLevel") );   

    CEGUI::DefaultWindow* pName = WDefaultWindow(GetWndMgr().getWindow("ObjectPage/ObjectName"));
    const char * name = shape->GetName();
    pName->setText(ToCEGUIString(name));

    /// pHp ,Level
    CEGUI::ProgressBar*  pHp  = WProgress( GetWndMgr().getWindow("ObjectPage/ObjectHp"));

    if ( shape->GetType() == TYPE_MONSTER )
    {
        if ( static_cast<CMonster*>(shape)->GeteNpcType() == NT_Normal )
        {
            pHp->setText( "Npc" );
            pLevel->setText("");
            return true ;
        }
        lLev = static_cast<CMonster*>(shape)->GetLevel();
    }
    else
    {
        lLev = static_cast<CPlayer*>(shape)->GetLevel();
    }    
    sprintf_s<128>(Format,"%d",lLev);
    pLevel->setText( Format);
 
    long  lHp = shape->GetHp();
    long  lMaxHp = shape->GetMaxHp();
    sprintf_s<128>(Format,"%d/%d",lHp,lMaxHp);   
    if( lMaxHp!=0 )
    {
        curVal = float(lHp) / float(lMaxHp);
    }
    pHp->setProgress( curVal );
    pHp->setText( Format );

    return true;
}

bool OnObjectClosed(const CEGUI::EventArgs& e)
{
    CMoveShape* shape = static_cast<CMoveShape*>(GetGame()->GetGameControl()->GetSelectedShape());
    if ( shape ) 
    {
        GetGame()->GetGameControl()->SetSelectedShape(NULL);
        GetGame()->GetGameControl()->SetCurLogSelectShape(NULL);
    }

    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    wnd->setVisible(false);
    
    return true;
}

bool OnOpenTeamMenu(const CEGUI::EventArgs& e)
{
    CEGUI::PopupMenu* teamMenuPage = static_cast<CEGUI::PopupMenu*>(WEArgs(e).window);
    teamMenuPage->openPopupMenu();
    return true;
}

CEGUI::Window*  InitTeamMenuPage()
{
    CEGUI::PopupMenu*  TeamMenuPage= static_cast<CEGUI::PopupMenu*>(LoadUI("TeamMenuPage"));
    
    TeamMenuPage->setVisible(true);

    TeamMenuPage->setAlwaysOnTop(true);

    TeamMenuPage->openPopupMenu();

    CEGUI::Window* wnd = GetWndMgr().getWindow("ObjectPage");
        
    const CEGUI::UVector2& winPos = wnd->getPosition();

    const CEGUI::UDim&     winWd  = wnd->getWidth();

    const CEGUI::UDim&     winHg  = wnd->getHeight();

    TeamMenuPage->setPosition( CEGUI::UVector2(cegui_reldim(winPos.d_x.d_scale + winWd.d_scale ), cegui_reldim(winHg.d_offset+0.01f) ) );

    TeamMenuPage->subscribeEvent(EVENT_OPEN,CEGUI::Event::Subscriber(OnOpenTeamMenu));

    /// 邀请组队
    CEGUI::MenuItem* item1 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item1"));
    if ( item1 )
    {
        item1->subscribeEvent(CEGUI::MenuItem::EventClicked,
            CEGUI::SubscriberSlot(OnInivteJoinTeam));
    }

    /// 离开队伍
    CEGUI::MenuItem* item2 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item2"));
    if ( item2 )
    {
        item2->subscribeEvent(CEGUI::MenuItem::EventClicked,
            CEGUI::SubscriberSlot(OnLeaveTeam));
    }

    /// 跟随队长
    CEGUI::MenuItem* item3 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item3"));
    if ( item3 )
    {
        item3->subscribeEvent(CEGUI::MenuItem::EventClicked,
            CEGUI::SubscriberSlot(OnFollowLeader));
    }

    /// 加为好友
    CEGUI::MenuItem* item4 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item4"));
    if ( item4 )
    {
        item4->subscribeEvent(CEGUI::MenuItem::EventClicked,
            CEGUI::SubscriberSlot(OnAddLinkMan));
    }

    /// 私聊密语
    CEGUI::MenuItem* item5 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item5"));
    if ( item5 )
    {
        item1->subscribeEvent(CEGUI::MenuItem::EventClicked,
            CEGUI::SubscriberSlot(OnPrivateChat));
    }

    /// 邀请相位
    CEGUI::MenuItem* item6 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item6"));
    if ( item6 )
    {
        item6->subscribeEvent(CEGUI::MenuItem::EventClicked,
            CEGUI::SubscriberSlot(OnInivteJoinPhase));
    }

    /// 加入黑名单
    CEGUI::MenuItem* item7 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item7"));
    if ( item7 )
    {
        item7->subscribeEvent(CEGUI::MenuItem::EventClicked,
            CEGUI::SubscriberSlot(OnAddBlackList));
    }

	/// 请求交易
	CEGUI::MenuItem* item8 = static_cast<CEGUI::MenuItem*>( TeamMenuPage->getChildRecursive("TeamMenuPage/Item8"));
	if ( item8 )
	{
		item8->subscribeEvent(CEGUI::MenuItem::EventClicked,
			CEGUI::SubscriberSlot(OnTraderQuest));
	}

    return TeamMenuPage;
}

bool  OnPopTeamMenu(const CEGUI::EventArgs& e)
{
    const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(e);

    if ( e_mouse.button != CEGUI::RightButton )
        return false ;

    CMoveShape* shape = static_cast<CMoveShape*>(GetGame()->GetGameControl()->GetSelectedShape());
    if ( !shape ) return false ;

    if ( shape->GetType() != TYPE_PLAYER )
       return false;

    FireUIEvent("TeamMenuPage", EVENT_OPEN);
    return true;
}

bool  OnInivteJoinTeam(const CEGUI::EventArgs& e)
{
    CMoveShape* shape = static_cast<CMoveShape*>(GetGame()->GetGameControl()->GetSelectedShape());
    if ( !shape ) return false ;

    if ( shape->GetType() != TYPE_PLAYER )
        return false;

    GetInst(TeamMsgMgr).SendInviteJoinTeam( GetGame()->GetMainPlayer(),static_cast<CPlayer*>(shape));
    return true;
}

bool  OnLeaveTeam(const CEGUI::EventArgs& e)
{
    GetInst(TeamMsgMgr).SendLeaveTeam( GetGame()->GetMainPlayer() );

    return true;
}

bool  OnFollowLeader(const CEGUI::EventArgs& e)
{
    GetInst(TeamMsgMgr).SendFollowLeader( GetGame()->GetMainPlayer() );

    return true;  
}

bool  OnAddLinkMan(const CEGUI::EventArgs& e)
{
   
    return true;
}

bool  OnPrivateChat(const CEGUI::EventArgs& e)
{

    return true;
}

bool  OnInivteJoinPhase(const CEGUI::EventArgs& e)
{
    CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
    CPlayer* shape = static_cast<CPlayer*>(GetGame()->GetGameControl()->GetSelectedShape());
    if ( !shape || !pMainPlayer) return false ;

    GetInst(PhaseSys).PhaseOper( pMainPlayer->GetExID(), shape->GetExID() ,C2S_Invite );
    return true;
}

bool  OnAddBlackList(const CEGUI::EventArgs& e)
{
   return true;
}

bool  OnTraderQuest(const CEGUI::EventArgs& e)
{
	CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
	CMoveShape* shape = static_cast<CMoveShape*>(GetGame()->GetGameControl()->GetSelectedShape());
	if ( !shape || !pMainPlayer) return false ;

	if ( shape->GetType() != TYPE_PLAYER )
		return false;

	if (pMainPlayer->Distance(shape)<6)
	{
		CMessage msg(MSG_C2S_PLAYERTRADE_REQUEST);
		msg.Add(shape->GetExID());
		msg.Send();
	}

	return true;
}