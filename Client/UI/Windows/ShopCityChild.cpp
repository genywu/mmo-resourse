/**
* @filename:ShopChild.cpp
* @date: 2010/7/21
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 商城页面里Tab（商店类型）下的唯一一个子Tab
*/

#include "stdafx.h"
#include "ShopCityDef.h"
#include "../UIDef.h"
#include "../../GameClient/ClientApp/ShopCity/ShopCitySys.h"

using namespace CEGUI;
extern Window* InitShopCityItemSet();
extern Window* GetWindow(const CEGUI::String& name);

bool OnShopCityChildTabContentSelChanged(const CEGUI::EventArgs& e);
void SetInitProShopCityChild(Window* pageWnd);
void SubEventShopCityChild(Window* pageWnd);

Window* InitShopCityChild()
{
	Window* wnd = LoadUI(SHOPCITY_CHILD_PAGE_NAME);
	SetInitProShopCityChild(wnd);
	SubEventShopCityChild(wnd);
	return wnd;
}

void SetInitProShopCityChild(Window* pageWnd)
{
	TabControl* tbc = WTabControl(pageWnd);
	//默认第一个可见
	tbc->setSelectedTabAtIndex(0);
	Window* ctt1 = tbc->getTabContentsAtIndex(tbc->getSelectedTabIndex());
	if(ctt1)
	{
		ctt1->setVisible(true);
		//把ShopCityItemSet 加到他上面
		ctt1->addChildWindow(InitShopCityItemSet());
	}
}

void SubEventShopCityChild(Window* pageWnd)
{
	TabControl* tbc = WTabControl(pageWnd);
	//
	tbc->subscribeEvent(TabControl::EventSelectionChanged,CEGUI::SubscriberSlot(OnShopCityChildTabContentSelChanged));
}

bool OnShopCityChildTabContentSelChanged(const CEGUI::EventArgs& e)
{
	TabControl* tbc = WTabControl(WEArgs(e).window);
	if(tbc)
	{
		CEGUI::Window* tbcontent = tbc->getTabContentsAtIndex(tbc->getSelectedTabIndex());
		if(tbcontent)
		{
			//把唯一ShopCityItemSet加到单前选中项上
			CEGUI::Window* itemSet = GetWindow(SHOPCITY_ITEMSET_PAGE_NAME);
			tbcontent->addChildWindow(itemSet);
			//更新ItemSet的显示
			FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE);
			//更新右搜索(筛选)菜单
			FireUIEvent(SHOPCITY_SEARCH_RIGHTWND_NAME,SHOPCITY_SEARCHRIGHT_EVENT_MENUUPDATE_NAME);
			//更新左搜索(导购)菜单
			FireUIEvent(SHOPCITY_SEARCH_LEFTWND_NAME,SHOPCITY_SEARCHLEFT_EVENT_MENUUPDATE_NAME);
		}
	}
	return true;
}
