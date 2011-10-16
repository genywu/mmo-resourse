/********************************************************************
	created:	2010/11/02
	created:	2:11:2010   9:44
	filename: 	e:\SVN_P6\trunk\Client\UI\Windows\NotifyCenter.cpp
	file path:	e:\SVN_P6\trunk\Client\UI\Windows
	file base:	NotifyCenter
	file ext:	cpp
	author:		zhaohang(kaba) (kuailekaba@gmail.com)
	
	purpose:	屏幕中央滚动的通告消息界面
*********************************************************************/

#include "stdafx.h"
#include "../UIDef.h"
#include "../UIManager.h"
#include "../Pub/MyRenderedStringParser.h"
#include "../../GameClient/ClientApp/NotifySys/NotyfySys.h"
#include "NotifyCenterDef.h"

using namespace CEGUI;

CEGUI::Window* InitNotifyCenter();

bool UpdateNotifyCenterWindow(const EventArgs& e);
void SetInitNotifyCenterProperty(Window* wnd);
void SubEventNotifyCenter(Window* wnd);
bool NotifyCenterChangerText(const EventArgs& e);

CEGUI::Window* InitNotifyCenter()
{
	Window* wnd = LoadUI(NOTIFYCENTER_PAGE_NAME);
	if(wnd)
	{
		SetInitNotifyCenterProperty(wnd);
		SubEventNotifyCenter(wnd);
	}
	return wnd;
}

void SetInitNotifyCenterProperty(Window* wnd)
{
	//MyRenderedStringParser* myParse = GetInst(UIManager).GetMyRenderedStringParser();
	//wnd->setCustomRenderedStringParser(myParse);
	wnd->setMinSize(UVector2(UDim(0.0f,0),UDim(0.0f,0)));
	wnd->setSize(UVector2(UDim(0.6f,50),UDim(0.0001f,80)));
	wnd->setVisible(true);
}

void SubEventNotifyCenter(Window* wnd)
{
	wnd->subscribeEvent(Window::EventWindowUpdated,Event::Subscriber(UpdateNotifyCenterWindow));
	wnd->subscribeEvent(NOTIFYCENTER_EVENT_CHANGETEXT,Event::Subscriber(NotifyCenterChangerText));
}

bool UpdateNotifyCenterWindow(const EventArgs& e)
{
	Window* wnd = WEArgs(e).window;
	wnd->invalidate(false);
	
	NotifyRenderStringMgr& mgr = GetInst(NotifyRenderStringMgr);
	mgr.UpdateTime();

	return true;
}

bool NotifyCenterChangerText(const EventArgs& e)
{
	NotifyRenderStringMgr& mgr = GetInst(NotifyRenderStringMgr);
	Window* wnd = WEArgs(e).window;
	if(mgr.IsEmpty() == false)
	{
		std::string& str = mgr.FrontText();
		wnd->setText(ToCEGUIString(str.c_str()));
		wnd->setVisible(true);
	}
	else
	{
		wnd->setText("");
		wnd->setVisible(false);
	}
	return true;
}