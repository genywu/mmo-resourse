/**
* @filename:ShopCity.cpp
* @date: 2010/7/21
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 商城的UI部分
*/
#include "stdafx.h"
#include "ShopCityDef.h"
#include "../UIDef.h"
#include "../../GameClient/ClientApp/ShopCity/ShopCitySys.h"
//GoodsList.h 包含了GoodsExDef.h 而goodsExDef.h用到了CGUI类，所以先包含一个GUID.h文件
#include "../../../Public/Module/GUID.h"
#include "../../GameClient/ClientApp/Goods/GoodsList.h"

extern CEGUI::Window* InitShopCityChild();
extern CEGUI::Window* GetWindow(const CEGUI::String& name);
extern SCGData::eSCType GetShopCityTypeByTabContentSelIndex();
extern SCGData::eSType  GetShopTypeByTabContentSelIndex();
void SetInitProShopCity(CEGUI::Window* pageWnd);
void SubEventShopCity(CEGUI::Window* pageWnd);

///ShopCityTablContentSelChanged
bool OnShopCityContentSelChanged(const CEGUI::EventArgs& e);
///清空三个付费券的Edixbox显示数据
bool OnClearMoneyEdboxes(const CEGUI::EventArgs& e);
///更新三个付费券的UI数据
bool OnShopCityUpdateMoneyEdboxes(const CEGUI::EventArgs& e);

///搜索
bool OnShopCitySearch(const CEGUI::EventArgs& e);

///搜索按钮左边的ccboxItemchange事件
bool OnSearchLeftccboxItemChange(const CEGUI::EventArgs& e);

///更新左搜索(导购)菜单
bool OnSearchLeftMenuUpdate(const CEGUI::EventArgs& e);

///搜素按钮右边的ccboxItemChange事件
bool OnSearchRightccboxItemChange(const CEGUI::EventArgs& e);

///更新右搜索(筛选)菜单
bool OnSearchRightMenuUpdate(const CEGUI::EventArgs& e);

///更新最近购买记录显示
bool OnUpdateLatestBuy(const CEGUI::EventArgs& e);

///更新推荐
bool OnUpdateTwitter(const CEGUI::EventArgs& e);
///双击推荐列表，打开购买页面
bool OnShopCityTwitterMouseDoubleClicked(const CEGUI::EventArgs& e);
///充值
bool OnShopCitySaveMoney(const CEGUI::EventArgs& e);

///删除试穿的物品
bool OnShopCityTryOnRoomDel(const CEGUI::EventArgs& e);

///更新ShopCity界面
bool OnShopCityUpdate(const CEGUI::EventArgs& e);
///关闭shopCity界面
bool OnShopCityClosed(const CEGUI::EventArgs& e);

CEGUI::Window* InitShopCity()
{
	CEGUI::Window* wnd = LoadUI(SHOPCITY_PAGE_NAME);
	SubEventShopCity(wnd);
	//先注册事件，再设置初始化状态，因为初始化时调用了事件
	SetInitProShopCity(wnd);
	GetInst(ShopCityMsgMgr).SetPageState(true);
	return wnd;
}

void SetInitProShopCity(CEGUI::Window* pageWnd)
{
	CEGUI::TabControl* tbs = WTabControl(pageWnd->getChildRecursive(SHOPCITY_TAB_NAME));
	if(tbs)
	{
		//默认让第一个TabContent可见
		tbs->setSelectedTabAtIndex(0);
		CEGUI::Window* tbs1 = tbs->getTabContentsAtIndex(tbs->getSelectedTabIndex());
#ifdef _DEBUG
		const char* name = tbs1->getName().c_str();
		OutputDebugStr(name);
		OutputDebugStr("\n");
#endif
		tbs1->setVisible(true);
		//将唯一ShopCityChild添加到上面
		CEGUI::TabControl* child = WTabControl(InitShopCityChild());
		//默认让第一tabContent可见
		child->setSelectedTabAtIndex(0);
		tbs1->addChildWindow(child);
	}
	//清空moneyEdboxes
	FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_CLEARMONEYEDBOXES);
	//moneyEdboxes属性设置
	CEGUI::Editbox* edbox = NULL;
	//星钻
	edbox = WEditBox(pageWnd->getChildRecursive(SHOPCITY_XINGZUAN_EDBOX_NAME));
	if(edbox)
	{
		//只能输入数字
		edbox->setValidationString("[0-9]*");
		//数字宽
		edbox->setMaxTextLength(9);
		//只读
		edbox->setReadOnly(true);
	}
	//点券
	edbox = WEditBox(pageWnd->getChildRecursive(SHOPCITY_DIANJUAN_EDBOX_NAME));
	if(edbox)
	{
		//只能输入数字
		edbox->setValidationString("[0-9]*");
		//数字宽
		edbox->setMaxTextLength(9);
		//只读
		edbox->setReadOnly(true);
	}
	//位面
	edbox = WEditBox(pageWnd->getChildRecursive(SHOPCITY_WEIMIAN_EDBOX_NAME));
	if(edbox)
	{
		//只能输入数字
		edbox->setValidationString("[0-9]*");
		//数字宽
		edbox->setMaxTextLength(9);
		//只读
		edbox->setReadOnly(true);
	}
	//左搜素框
	CEGUI::Combobox* cbboxL = WComboBox(pageWnd->getChildRecursive(SHOPCITY_SEARCH_LEFTWND_NAME));
	if(cbboxL)
	{
		cbboxL->setReadOnly(true);
	}
	//右搜素框
	CEGUI::Combobox* cbboxR = WComboBox(pageWnd->getChildRecursive(SHOPCITY_SEARCH_RIGHTWND_NAME));
	if(cbboxR)
	{
		cbboxR->setReadOnly(true);
	}
}

void SubEventShopCity(CEGUI::Window* pageWnd)
{
	//删除试装的物品
	CEGUI::Window* flDel = pageWnd->getChildRecursive(SHOPCITY_FILLTERROOM_NAME);
	if(flDel)
	{
		flDel->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityTryOnRoomDel));
	}
	//充值
	CEGUI::Window* saveMoney = pageWnd->getChildRecursive(SHOPCITY_SAVEMONEY_NAME);
	if(saveMoney)
	{
		saveMoney->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCitySaveMoney));
	}
	//最近购买记录
	CEGUI::Window* latesBuy = pageWnd->getChildRecursive(SHOPCITY_LATESTBUY_NAME);
	if(latesBuy)
		latesBuy->subscribeEvent(SHOPCITY_PAGE_EVENT_UPDATE_LATESTBUY,CEGUI::SubscriberSlot(OnUpdateLatestBuy));
	//清空三种money显示
	pageWnd->subscribeEvent(SHOPCITY_PAGE_EVENT_CLEARMONEYEDBOXES,CEGUI::SubscriberSlot(OnClearMoneyEdboxes));
	//更新三种付费money显示
	pageWnd->subscribeEvent(SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES,CEGUI::SubscriberSlot(OnShopCityUpdateMoneyEdboxes));
	//推荐
	CEGUI::Window* twitter = pageWnd->getChildRecursive(SHOPCITY_TWITTER_NAME);
	if(twitter)
	{
		twitter->subscribeEvent(SHOPCITY_TWITTER_EVENT_NAME,CEGUI::SubscriberSlot(OnUpdateTwitter));
	}
	//推荐双击
	CEGUI::Listbox* ltb = WListBox(GetWindow(SHOPCITY_TWITTER_CHILDLISTBOX_NAME));
	ltb->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick,CEGUI::SubscriberSlot(OnShopCityTwitterMouseDoubleClicked));
	CEGUI::TabControl* tbs = WTabControl(pageWnd->getChildRecursive(SHOPCITY_TAB_NAME));
	{
		if (tbs)
		{
			tbs->subscribeEvent(CEGUI::TabControl::EventSelectionChanged,CEGUI::SubscriberSlot(OnShopCityContentSelChanged));
		}
	}
	//搜索事件
	CEGUI::Window* search = pageWnd->getChildRecursive(SHOPCITY_SEARCH_NAME);
	if(search){
		search->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCitySearch));
	}
	//搜索左边的ccboxItemchange事件
	CEGUI::Window* searchL = pageWnd->getChildRecursive(SHOPCITY_SEARCH_LEFTWND_NAME);
	if (searchL)
	{
		searchL->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::SubscriberSlot(OnSearchLeftccboxItemChange));
		//搜索按钮左边的菜单(导购)更新事件
		searchL->subscribeEvent(SHOPCITY_SEARCHLEFT_EVENT_MENUUPDATE_NAME,CEGUI::SubscriberSlot(OnSearchLeftMenuUpdate));
	}
	//搜素右边的ccboxItemchange事件
	CEGUI::Window* searchR = pageWnd->getChildRecursive(SHOPCITY_SEARCH_RIGHTWND_NAME);
	if(searchR)
	{
		searchR->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::SubscriberSlot(OnSearchRightccboxItemChange));
		//搜索按钮右边的(筛选)菜单更新事件
		searchR->subscribeEvent(SHOPCITY_SEARCHRIGHT_EVENT_MENUUPDATE_NAME,CEGUI::SubscriberSlot(OnSearchRightMenuUpdate));
	}

	//打开时候，就更新界面UI显示
	pageWnd->subscribeEvent(EVENT_OPEN,CEGUI::SubscriberSlot(OnShopCityUpdate));
	//关闭时候
	pageWnd->subscribeEvent(EVENT_CLOSE,CEGUI::SubscriberSlot(OnShopCityClosed));
	pageWnd->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,CEGUI::SubscriberSlot(OnShopCityClosed));
}

bool OnShopCityContentSelChanged(const CEGUI::EventArgs& e)
{
	CEGUI::TabControl* tbs = WTabControl(WEArgs(e).window);
	if(tbs)
	{
		CEGUI::Window* tbcontent =  tbs->getTabContentsAtIndex(tbs->getSelectedTabIndex());
		if(tbcontent)
		{
			tbcontent->addChildWindow(GetWindow(SHOPCITY_CHILD_PAGE_NAME));//把唯一ShopCityChild加到当前选中tbcontent上
			//更新ItemSet的显示
			FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE);
			//更新最近购买
			FireUIEvent(SHOPCITY_LATESTBUY_NAME,SHOPCITY_PAGE_EVENT_UPDATE_LATESTBUY);
			//更新推荐
			FireUIEvent(SHOPCITY_TWITTER_NAME,SHOPCITY_TWITTER_EVENT_NAME);
			//更新左搜索(导购)菜单
			FireUIEvent(SHOPCITY_SEARCH_LEFTWND_NAME,SHOPCITY_SEARCHLEFT_EVENT_MENUUPDATE_NAME);
			//更新右搜索(筛选)菜单
			FireUIEvent(SHOPCITY_SEARCH_RIGHTWND_NAME,SHOPCITY_SEARCHRIGHT_EVENT_MENUUPDATE_NAME);
			//设置更新源类型
			ShopCityMsgMgr& msgMgr = GetInst(ShopCityMsgMgr);
			msgMgr.SetStateUpdateUIByType(0);//由选中的商城商店类型来更新
		}
	}
	return true;
}

bool OnClearMoneyEdboxes(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pageWnd = WEArgs(e).window;
	//星钻
	CEGUI::Window* temp = pageWnd->getChildRecursive(SHOPCITY_XINGZUAN_EDBOX_NAME);
	if(temp)
		temp->setText("");
	//点券
	temp = pageWnd->getChildRecursive(SHOPCITY_DIANJUAN_EDBOX_NAME);
	if(temp)
		temp->setText("");
	//位面
	temp = pageWnd->getChildRecursive(SHOPCITY_WEIMIAN_EDBOX_NAME);
	if(temp)
		temp->setText("");
	return true;
}
bool OnShopCitySearch(const CEGUI::EventArgs& e)
{
	//获取导购文本
	CEGUI::Combobox* guide = WComboBox(GetWindow(SHOPCITY_SEARCH_LEFTWND_NAME));
	if(guide && guide->getEditbox()->getText()=="")
	{
		return true;//文本为空直接返回
	}
	ShopCityMsgMgr& msgMgr = GetInst(ShopCityMsgMgr);
	msgMgr.SetStateUpdateUIByType(2);//设为更新源为导购
	//根据导购数据更新显示项目
	FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE_BY_GUIDE);
	return true;
}
bool OnSearchLeftccboxItemChange(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnSearchRightccboxItemChange(const CEGUI::EventArgs& e)
{
	//
	ShopCityMsgMgr& msgMgr = GetInst(ShopCityMsgMgr);
	//设为根据筛选数据更新显示项目
	msgMgr.SetStateUpdateUIByType(1);
	//根据筛选数据更新Itemset的显示
	FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE_BY_FILTER);
	return true;
}

bool OnUpdateLatestBuy(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	//先把图片清掉 ,默认为十个条目
	char name[256] = "";
	for(uint i = 0 ;i < 10 ; ++i)
	{
		sprintf(name,SHOPCITY_LATESTBUY_ITME_NAME_D,i);
		CEGUI::Window* temp = wnd->getChild(name);
		if(temp)
		{
			temp->setProperty("Image","");
			OutputDebugStr(temp->getName().c_str());
			OutputDebugStr("\n");
		}
	}

	//由索引关联商城类型
	SCGData::eSCType eCityType = GetShopCityTypeByTabContentSelIndex();
	SCGData* dt = GetInst(ShopCityMsgMgr).GetShopCityGoodsData();
	SCGData::MapSBT10& personal = dt->GetSelfBuyTop10();
	SCGData::VecGDPTA perDTA = personal[eCityType];

	size_t count = perDTA.size();
	for(size_t i = 0 ; i < count ; ++i)
	{
		char name[256] = "";
		sprintf(name,SHOPCITY_LATESTBUY_ITME_NAME_D,i);
		CEGUI::Window* temp = wnd->getChild(name);
		if(temp)
		{
			CGoodsList::tagGoods2* tg2 = CGoodsList::GetProperty(perDTA[i].index);
			if(tg2)
			{
				char imagesetname[256];
				sprintf(imagesetname,GOODS_PREFIXID,tg2->BaseProperty.dwIconId);
				CEGUI::String imagename = CEGUI::PropertyHelper::intToString(tg2->BaseProperty.dwIconId)+".jpg";
				SetBackGroundImage(WGUISheet(temp),imagesetname,GOODS_ICON_PATH,imagename.c_str());
			}
		}
	}
	return true;
}

//新品推荐
bool OnUpdateTwitter(const CEGUI::EventArgs& e)
{
	CEGUI::Window* twitter = WEArgs(e).window;
	CEGUI::Listbox*  lb = WListBox(twitter->getChildRecursive(SHOPCITY_TWITTER_CHILDLISTBOX_NAME));
#ifdef _DEBUG
	OutputDebugStr(lb->getName().c_str());
	OutputDebugStr("\n");
	OutputDebugStr(twitter->getChildAtIdx(0)->getName().c_str());
	OutputDebugStr("n");
#endif
	//清空
	lb->resetList();

	//由索引关联商城类型
	SCGData::eSCType eCityType = GetShopCityTypeByTabContentSelIndex();
	SCGData* dt = GetInst(ShopCityMsgMgr).GetShopCityGoodsData();
	//新品推荐显示
	SCGData::MapNewestA& resdta = dt->GetNewestVec();
	SCGData::VecGDPTA& vecDTA = resdta[eCityType];
	for(uint i = 0 ; i < vecDTA.size() ; ++i)
	{
		CGoodsList::tagGoods2* ptg2 = CGoodsList::GetProperty(vecDTA[i].index);
		if(ptg2)
		{
			string str  = ptg2->BaseProperty.strName.c_str();
			//CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(str.c_str(),vecDTA[i].index);//索引关联Item ID
			CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(ToCEGUIString(str.c_str()),vecDTA[i].index);//索引关联Item ID
			lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
			lb->addItem(lti);
		}
	}
	return true;
}

//充值
bool OnShopCitySaveMoney(const CEGUI::EventArgs& e)
{
	return true;
}

//删除试穿的物品
bool OnShopCityTryOnRoomDel(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnShopCityUpdate(const CEGUI::EventArgs& e)
{
	//更新Itemset
	FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE);
	//更新推荐
	FireUIEvent(SHOPCITY_TWITTER_NAME,SHOPCITY_TWITTER_EVENT_NAME);
	//更新最近购买
	FireUIEvent(SHOPCITY_LATESTBUY_NAME,SHOPCITY_PAGE_EVENT_UPDATE_LATESTBUY);
	//更新右搜索(筛选)菜单
	FireUIEvent(SHOPCITY_SEARCH_RIGHTWND_NAME,SHOPCITY_SEARCHRIGHT_EVENT_MENUUPDATE_NAME);
	//更新左搜索(导购)菜单
	FireUIEvent(SHOPCITY_SEARCH_LEFTWND_NAME,SHOPCITY_SEARCHLEFT_EVENT_MENUUPDATE_NAME);
	//清空付费券的UI
	FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_CLEARMONEYEDBOXES);
	//然后更新付费券的UI
	FireUIEvent(SHOPCITY_PAGE_NAME,SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES);
	return true;
}
//更新三个付费券的UI数据
bool OnShopCityUpdateMoneyEdboxes(const CEGUI::EventArgs& e)
{
	ShopCityMsgMgr& mgr = GetInst(ShopCityMsgMgr);
	CEGUI::Window* pgWnd = WEArgs(e).window;
	CEGUI::Window* temp = pgWnd->getChildRecursive(SHOPCITY_XINGZUAN_EDBOX_NAME);
	if(temp)
	{
		temp->setText(CEGUI::PropertyHelper::intToString(mgr.GetPlayerXinZuan()));
	}
	temp = pgWnd->getChildRecursive(SHOPCITY_WEIMIAN_EDBOX_NAME);
	if(temp)
		temp->setText(CEGUI::PropertyHelper::intToString(mgr.GetPlayerWeiMian()));
	temp = pgWnd->getChildRecursive(SHOPCITY_DIANJUAN_EDBOX_NAME);
	if(temp)
		temp->setText(CEGUI::PropertyHelper::intToString(mgr.GetPlayerDianQuan()));
	return true;
}

//更新左搜索(导购)菜单
bool OnSearchLeftMenuUpdate(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox* cbbox = WComboBox(WEArgs(e).window);
	cbbox->clearAllSelections();
	cbbox->resetList();
	cbbox->getEditbox()->setText("");
	//由索引关联商城类型
	SCGData::eSCType eCityType = GetShopCityTypeByTabContentSelIndex();
	SCGData* dt = GetInst(ShopCityMsgMgr).GetShopCityGoodsData();
	SCGData::MapGuideDataA& mapGuide = dt->GetGuideList();
	//根据索引获取导购数据
	SCGData::MapStrGGDTPA& mapGuideDTA = mapGuide[eCityType];
	CEGUI::Combobox* cbboxRight = WComboBox(GetWndMgr().getWindow(SHOPCITY_SEARCH_RIGHTWND_NAME));
	if(cbboxRight)
	{
		CEGUI::ListboxItem* lbi = cbboxRight->getSelectedItem();
		size_t idx = 0;
		if(lbi)
			idx = cbboxRight->getItemIndex(lbi);
		SCGData::MapStrGGDTPA::iterator iter = mapGuideDTA.begin();
		for(; iter != mapGuideDTA.end() ; ++iter)
		{
			//添加导购菜单
			string menuStr = iter->first;
			//CEGUI::ListboxItem* lbi = new CEGUI::ListboxTextItem(menuStr.c_str());
			CEGUI::ListboxItem* lbi = new CEGUI::ListboxTextItem(ToCEGUIString(menuStr.c_str()));
			lbi->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
			if(iter == mapGuideDTA.begin())//默认让第一个为选中
				lbi->setSelected(true);
			cbbox->addItem(lbi);
		}
	}
	return true;
}

//更新右搜索(筛选)菜单
bool OnSearchRightMenuUpdate(const CEGUI::EventArgs& e)
{
	CEGUI::Combobox* cbbox = WComboBox(WEArgs(e).window);
	cbbox->clearAllSelections();
	cbbox->resetList();
	cbbox->getEditbox()->setText("");

	//由索引关联商城类型
	SCGData::eSCType eShopCityType = GetShopCityTypeByTabContentSelIndex();
	//由索引关联商店类型，tabControl的索引0单独对应热销商品
	SCGData::eSType shoptype = GetShopTypeByTabContentSelIndex();
	if(shoptype == SCGData::TABTYPE_HOT)//热销没有筛选项
		return true;
	//根据商城和商店类型获取筛选数据
	SCGData* dt = GetInst(ShopCityMsgMgr).GetShopCityGoodsData();
	SCGData::MapFLDTA& mapSel = dt->GetFilterList();
	SCGData::MapUFLDTPA& mapUSel = mapSel[eShopCityType];
	SCGData::MapStrFilDTPA& mapStrSel = mapUSel[shoptype];
	SCGData::MapStrFilDTPA::iterator iter = mapStrSel.begin();
	for( ; iter != mapStrSel.end() ; ++iter)
	{
		//初始化筛选菜单
		string str = (*iter).first;
		//CEGUI::ListboxItem* lbi = new CEGUI::ListboxTextItem(str.c_str());
		CEGUI::ListboxItem* lbi = new CEGUI::ListboxTextItem(ToCEGUIString(str.c_str()));
		lbi->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		if(iter == mapStrSel.begin() )//默认让第一个为选中项
			lbi->setSelected(true);
		cbbox->addItem(lbi);
	}
	return true;
}

bool OnShopCityClosed(const CEGUI::EventArgs& e)
{
	GetInst(ShopCityMsgMgr).Send_CloseShop();
	FireUIEvent(SHOPCITY_BUY_PAGE_NAME, EVENT_CLOSE);
	return true;
}

//双击推荐列表，打开购买页面
bool OnShopCityTwitterMouseDoubleClicked(const CEGUI::EventArgs& e)
{
	CEGUI::Listbox* twitterList = WListBox(WEArgs(e).window);  
	CEGUI::ListboxItem* lbi = twitterList->getFirstSelectedItem();
	if(lbi)
	{
		uint index = lbi->getID();//获取索引，索引关联物品索引
		CEGUI::Window* buyPage = GetWindow(SHOPCITY_BUY_PAGE_NAME);
		buyPage->setID(index);//购买界面ID与物品索引关联
		//打开购买界面
		FireUIEvent(SHOPCITY_BUY_PAGE_NAME,EVENT_OPEN);
	}
	return true;
}