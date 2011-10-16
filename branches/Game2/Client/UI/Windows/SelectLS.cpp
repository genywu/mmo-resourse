/**
* @filename:SelectLS.cpp
* @date: 2010/6/4
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: Ñ¡ÔñµÇÂ¼·þÎñÆ÷
*/
#include "stdafx.h"
#include "../UIDef.h"
#include "../UIManager.h"
#include "../GamePage/Login/LoginPage.h"
#include "../../GameClient/ClientApp/SelectLS/SelectLSMgr.h"
#include "../../GameClient/Game.h"

bool UpdateLSListUI(const CEGUI::EventArgs& e);

bool OnDoubleClick(const CEGUI::EventArgs& e);

bool OnEscKeyDown(const CEGUI::EventArgs& e);

void InitializedSelectLSProperty(CEGUI::Window* pageWnd);

void SubscriberSelectLSEvent(CEGUI::Window* pageWnd);

CEGUI::Window* InitSelectLS()
{
	CEGUI::Window* wnd = LoadUI("SelectLS");
	InitializedSelectLSProperty(wnd);
	SubscriberSelectLSEvent(wnd);
	LSHelp& lsh = GetInst(LSHelp);
	lsh.ReadLSInfo();
	FireUIEvent("SelectLS","UpdateLSList");
	return wnd;
}

void InitializedSelectLSProperty(CEGUI::Window* pageWnd)
{
	if(!pageWnd)
		return;
	CEGUI::Listbox* lb = WListBox(pageWnd);
	if(!lb)
		return;
	lb->activate();
}

void SubscriberSelectLSEvent(CEGUI::Window* pageWnd)
{
	if(!pageWnd)
		return ;
	pageWnd->subscribeEvent("UpdateLSList",CEGUI::Event::Subscriber(UpdateLSListUI));
	CEGUI::Listbox* lb = WListBox(pageWnd->getChildRecursive("LSList"));
	lb->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick,CEGUI::Event::Subscriber(OnDoubleClick));
	pageWnd->subscribeEvent(CEGUI::Listbox::EventKeyDown,CEGUI::Event::Subscriber(OnEscKeyDown));
}

bool UpdateLSListUI(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pageWnd = WEArgs(e).window;
	if(!pageWnd)
		return false;
	CEGUI::Listbox* lsbox = WListBox(pageWnd->getChildRecursive("LSList"));
	if(!lsbox)
		return false;
	lsbox->resetList();
	const CEGUI::Image* cov_img = &CEGUI::ImagesetManager::getSingleton().get(IMAGES_FILE_NAME).getImage(CHECKBOXMARK);
	LSHelp &lshelp =  GetInst(LSHelp);
	for(size_t i = 0 ; i < lshelp.GetLSSize() ; ++i)
	{
		tagLS* ls = lshelp.GetLS(i);
		CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(ToCEGUIString(ls->serverName.c_str()));
		item->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		//item->setCoverBrushImage(cov_img);
		item->setUserData((void*)ls);

		lsbox->addItem(item);
	}
	lsbox->activate();
	return true;
}

bool OnDoubleClick(const CEGUI::EventArgs& e)
{
	CEGUI::Listbox* lb = WListBox(WEArgs(e).window);
	if(!lb)
		return false;
	CEGUI::ListboxItem* item = lb->getFirstSelectedItem();
	if(!item)
		return false;
	tagLS* ls = (tagLS*)item->getUserData();
	if(!ls)
		return false;
	LSHelp& lshelp = GetInst(LSHelp);
	lshelp.SetSelLS(ls);
    GetGame()->SetLoginServerIP(ls->ip.c_str());
    GetGame()->SetLoginServerPort(ls->port);
	FireUIEvent("SelectLS",EVENT_CLOSE);
	GetInst(LoginPage).Open();
	return true;
}

bool OnEscKeyDown(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& arg = WEArgs(e);
	const CEGUI::KeyEventArgs& key = static_cast<const CEGUI::KeyEventArgs&>(arg);
	if(CEGUI::Key::Escape == key.scancode)
	{
		FireUIEvent("SelectLS",EVENT_CLOSE);
		GetInst(LoginPage).Open();
		return true;
	}
	return false;
}