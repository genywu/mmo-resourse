#include "StdAfx.h"
#include "../UIDef.h"
#include "../UIManager.h"
#include "../../GameClient/ClientApp/Team/Team.h"
#include "../../GameClient/Game.h"
#include "../../Net/Message.h"
#include "../../ui/GamePage/Login/LoginPage.h"
#include "../../../Public/Common/MsgType.h"

CEGUI::Window* InitGameExit();
bool OnReturnCharSel(const CEGUI::EventArgs &args);
bool OnReturnServerSel(const CEGUI::EventArgs &args);
bool OnReturnLogin(const CEGUI::EventArgs &args);
bool OnReturnExit(const CEGUI::EventArgs &args);

CEGUI::Window* InitGameExit()
{
    CEGUI::Window* wnd = LoadUI("GameExit");
	wnd->setVisible(false);
    CEGUI::PushButton* btn = WPushButton(wnd->getChild("GameExit/CharSel"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(OnReturnCharSel));
    btn = WPushButton(wnd->getChild("GameExit/ServerSel"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(OnReturnServerSel));
    btn = WPushButton(wnd->getChild("GameExit/Login"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(OnReturnLogin));
    btn = WPushButton(wnd->getChild("GameExit/Exit"));
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::Event::Subscriber(OnReturnExit));

    return wnd;
}

bool OnReturnCharSel(const CEGUI::EventArgs &args)
{
	// 玩家有队伍发送离开队伍消息
	TeamMsgMgr& mgr = GetInst(TeamMsgMgr);
	mgr.SendSelfLeaveTeam();
    CMessage msg(MSG_C2S_OTHER_QUITTOSELECT);
    msg.Send();
    GetInst(UIManager).CloseAllPage();
    return true;
}

bool OnReturnServerSel(const CEGUI::EventArgs &args)
{
	// 玩家有队伍发送离开队伍消息
//	TeamMsgMgr& mgr = GetInst(TeamMsgMgr);
//	mgr.SendSelfLeaveTeam();
//	GetWndMgr().getWindow("GameExit")->setVisible(false);
//	GetGame()->SetGameState(GS_LOGIN);
//#ifdef _DEBUG
//	GetInst(UIManager).ReleaseAllPage();
//#endif
//	GetInst(UIManager).CloseAllPage();
//	GetInst(LoginPage).Open();
//	FireUIEvent("SelectLS",EVENT_OPEN);
    return true;
}

bool OnReturnLogin(const CEGUI::EventArgs &args)
{
	// 玩家有队伍发送离开队伍消息
	//TeamMsgMgr& mgr = GetInst(TeamMsgMgr);
	//mgr.SendSelfLeaveTeam();
 //   GetGame()->CloseNetClient();
 //   GetWndMgr().getWindow("GameExit")->setVisible(false);
 //   GetGame()->SetGameState(GS_LOGIN);
	//GetInst(UIManager).CloseAllPage();
	//GetInst(LoginPage).Open();
    return true;
}

bool OnReturnExit(const CEGUI::EventArgs &args)
{
    CGame::s_bExit = true;
	GetWndMgr().getWindow("GameExit")->setVisible(false);
	GetInst(UIManager).CloseAllPage();
    PostQuitMessage(0);
    return true;
}
