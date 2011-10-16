/********************************************************************
	created:	2010/10/25
	created:	25:10:2010   11:25
	filename: 	e:\SVN_P6\trunk\Client\UI\Windows\Genius.cpp
	file path:	e:\SVN_P6\trunk\Client\UI\Windows
	file base:	Genius
	file ext:	cpp
	author:		zhaohang(kaba) (kuailekaba@gmail.com)
	
	purpose:	天赋界面
*********************************************************************/
#include "stdafx.h"
#include "../UIDef.h"
#include "GeniusDef.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"
///初始化天赋界面
CEGUI::Window* InitGenius();

///设置天赋界面的初始界面状态
void SetInitializedGeniusUIProperty(CEGUI::Window* pageWnd);

///注册天赋界面的事件
void SubscribeGeniusEvent(CEGUI::Window* pgWnd);

///天赋选中列表更改
bool OnGeniusListboxSelChanged(const CEGUI::EventArgs& e);

///更新天赋界面
bool OnUpdateGenius(const CEGUI::EventArgs& e);

CEGUI::Window* InitGenius()
{
	CEGUI::Window* wnd = LoadUI(GENIUS_PAGE_NAME);
	SetInitializedGeniusUIProperty(wnd);
	SubscribeGeniusEvent(wnd);
	return wnd;
}

void SetInitializedGeniusUIProperty(CEGUI::Window* pageWnd)
{
	if(!pageWnd)
		return;
	pageWnd->setVisible(false);
	CEGUI::Editbox* eLevel = WEditBox(pageWnd->getChildRecursive(GENIUS_E_LEVEL));
	if(eLevel)
	{
		eLevel->setReadOnly(true);
	}
	CEGUI::MultiLineEditbox* mleDesc = WMLEditBox(pageWnd->getChildRecursive(GENIUS_MLE_DESC));
	if(mleDesc)
	{
		mleDesc->setReadOnly(true);
	}

}

void SubscribeGeniusEvent(CEGUI::Window* pgWnd)
{
	if(!pgWnd)
		return;
	pgWnd->subscribeEvent(EVENT_OPEN,OnUpdateGenius);
	CEGUI::Listbox* lb = WListBox(pgWnd->getChildRecursive(GENIUS_L_LBOX));
	if(lb)
		lb->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,OnGeniusListboxSelChanged);
}

bool OnUpdateGenius(const CEGUI::EventArgs& e)
{
	CPlayer* player = GetGame()->GetMainPlayer();
	std::map<long,long>*mapGenius = player->GetGenius();
	if(mapGenius->size() <= 0)
		return false;
	CEGUI::Listbox* lb = NULL;
	CEGUI::Editbox* eLevel = NULL;
	CEGUI::MultiLineEditbox* mleDesc = NULL;
	CEGUI::Window* pgWnd = WEArgs(e).window;
	if(pgWnd)
	{
		lb = WListBox(pgWnd->getChildRecursive(GENIUS_L_LBOX));
		if(lb)
			lb->resetList();
		eLevel = WEditBox(pgWnd->getChildRecursive(GENIUS_E_LEVEL));
		if(eLevel)
			eLevel->setText("");
		mleDesc = WMLEditBox(pgWnd->getChildRecursive(GENIUS_MLE_DESC));
		if(mleDesc)
			mleDesc->setText("");
	}
	std::map<long,long>::iterator iter = mapGenius->begin();
	for (; iter != mapGenius->end() ; ++iter)
	{
		uint id = (*iter).first;
		uint level = (*iter).second;
		if(lb)
		{
			char temp[32];
			sprintf(temp,"%d",id);
			CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(temp,id);
			lti->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME);
			lb->addItem(lti);
		}
		if(eLevel)
		{
			eLevel->setText(CEGUI::PropertyHelper::intToString(level));
		}
	}
	return true;
}

bool OnGeniusListboxSelChanged(const CEGUI::EventArgs& e)
{

	return true;
}