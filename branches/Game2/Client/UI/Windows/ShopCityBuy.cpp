/**
* @filename:ShopCityBuy
* @date: 2010/7/27
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 商城的点击Item后弹出的购买页面
*/

#include "stdafx.h"
#include "ShopCityDef.h"
#include "../UIDef.h"
#include "../../GameClient/ClientApp/ShopCity/ShopCitySys.h"
//GoodsList.h 包含了GoodsExDef.h 而goodsExDef.h用到了CGUI类，所以先包含一个GUID.h文件
#include "../../../Public/Module/GUID.h"
#include "../../GameClient/ClientApp/Goods/GoodsList.h"

extern CEGUI::Window* GetWindow(const CEGUI::String& name);
extern SCGData::eSCType GetShopCityTypeByTabContentSelIndex();
void SubEventShopCityBuy(CEGUI::Window* pgWnd);
void SetInitProShopCityBuy(CEGUI::Window* pgWnd);

bool OnShopCityBuyClear(const CEGUI::EventArgs& e);
bool OnShopCityBuyUpdate(const CEGUI::EventArgs& e);
bool OnShopCityBuySubmit(const CEGUI::EventArgs& e);
bool OnShopCityBuyBtnAdd(const CEGUI::EventArgs& e);
bool OnShopCityBuyBtnMinus(const CEGUI::EventArgs& e);
bool OnShopCityBuyClosed(const CEGUI::EventArgs& e);

CEGUI::Window* InitShopCityBuy()
{
	CEGUI::Window* wnd = LoadUI(SHOPCITY_BUY_PAGE_NAME);
	CEGUI::Window* shopCity = GetWindow(SHOPCITY_PAGE_NAME);
	shopCity->addChildWindow(wnd);
	SubEventShopCityBuy(wnd);
	SetInitProShopCityBuy(wnd);
	return wnd;
}

void SubEventShopCityBuy(CEGUI::Window* pgWnd)
{
	pgWnd->subscribeEvent(EVENT_OPEN,CEGUI::SubscriberSlot(OnShopCityBuyUpdate));
	pgWnd->subscribeEvent(EVENT_CLOSE,CEGUI::SubscriberSlot(OnShopCityBuyClosed));
	pgWnd->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,CEGUI::SubscriberSlot(OnShopCityBuyClosed));
	pgWnd->subscribeEvent(SHOPCITY_BUY_EVENT_CLEAR,CEGUI::SubscriberSlot(OnShopCityBuyClear));
	CEGUI::Window* BuyBtn = pgWnd->getChildRecursive(SHOPCITY_BUY_BUY_BTN);
	if(BuyBtn)
	{
		BuyBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityBuySubmit));
	}
	CEGUI::Window* AddBtn = pgWnd->getChildRecursive(SHOPCITY_BUY_ADD_BTN);
	if(AddBtn)
	{
		AddBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityBuyBtnAdd));
	}
	CEGUI::Window* minusBtn = pgWnd->getChildRecursive(SHOPCITY_BUY_MINUS_BTN);
	if(minusBtn)
	{
		minusBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityBuyBtnMinus));
	}
}

void SetInitProShopCityBuy(CEGUI::Window* pgWnd)
{
	FireUIEvent(SHOPCITY_BUY_PAGE_NAME,SHOPCITY_BUY_EVENT_CLEAR);
	CEGUI::Editbox* buynumEdb = WEditBox(pgWnd->getChildRecursive(SHOPCITY_BUY_NUMEDBOX));
	if(buynumEdb)
	{
		//只能输入数字
		buynumEdb->setValidationString("[0-9]*");
		//数字宽
		buynumEdb->setMaxTextLength(10);
		//只读
		//buynumEdb->setReadOnly(true);
	}
}

bool OnShopCityBuyUpdate(const CEGUI::EventArgs& e)
{
	//先清掉
	FireUIEvent(SHOPCITY_BUY_PAGE_NAME,SHOPCITY_BUY_EVENT_CLEAR);
	//让ShopCity无法使用，使得不会更改ShopCityType
	CEGUI::Window* shopcity = GetWindow(SHOPCITY_TAB_NAME);
	shopcity->setEnabled(false);
	//因为SHOPCITY_BUY_PAGE的窗口是ShopCity的child
	CEGUI::Window* pgWnd = WEArgs(e).window;
	pgWnd->setEnabled(true);
	//购买的物品显示项要同商城类型关联
	//商城类型
	SCGData::eSCType shopcityType = GetShopCityTypeByTabContentSelIndex();
	//由关联的ID获取物品的索引
	uint index = pgWnd->getID();
	ShopCityMsgMgr& scmgr = GetInst(ShopCityMsgMgr);
	SCGData* dt = scmgr.GetShopCityGoodsData();
	if(dt)
	{
		SCGData::tagGD* gd = dt->GetGoodsData(index,shopcityType);
		if(gd)
		{
			CGoodsList::tagGoods2* tg2 = CGoodsList::GetProperty(index);
			//再更新
			CEGUI::Window* pgWnd = WEArgs(e).window;
			//标题
			CEGUI::Window* name = pgWnd->getChildRecursive(SHOPCITY_BUY_NAME);
			if(name)
			{
				if(tg2)
					//name->setText(tg2->BaseProperty.strName.c_str());
					SetText(name, tg2->BaseProperty.strName.c_str());
			}
			//Icon
			CEGUI::GUISheet* Icon = WGUISheet(pgWnd->getChildRecursive(SHOPCITY_BUY_ICON));
			if(Icon)
			{
				char imagesetname[256];
				sprintf(imagesetname,GOODS_PREFIXID,tg2->BaseProperty
					.dwIconId);
				CEGUI::String imagename = CEGUI::PropertyHelper::intToString(tg2->BaseProperty
					.dwIconId)+".jpg";
				SetBackGroundImage(Icon,imagesetname,GOODS_ICON_PATH,imagename.c_str());
			}
			//price
			CEGUI::Window* price = pgWnd->getChildRecursive(SHOPCITY_BUY_PRICE);
			if(price)
			{
				price->setText(CEGUI::PropertyHelper::intToString(gd->price));
			}
			//PriceIcon
			CEGUI::GUISheet* priceIcon = WGUISheet(pgWnd->getChildRecursive(SHOPCITY_BUY_PRICE_ICON));
			if(priceIcon)
			{
				//priceIcon->setProperty("Image","");
			}
			//NowPrice
			CEGUI::Window* nowPrice = pgWnd->getChildRecursive(SHOPCITY_BUY_NOWPRICE);
			if(nowPrice)
			{
				uint i = gd->price * 10 * gd->discount / 100;
				nowPrice->setText(CEGUI::PropertyHelper::intToString(i));
			}
			//nowPriceIcon
			CEGUI::GUISheet* nowpriceIcon = WGUISheet(pgWnd->getChildRecursive(SHOPCITY_BUY_NOWPRICE_ICON));
			if(nowpriceIcon)
			{
				//nowpriceIcon->setProperty("Image","");
			}
			//lable
			CEGUI::Window* label = pgWnd->getChildRecursive(SHOPCITY_BUY_LABLE);
			if(label)
			{
				//label->setText("");
			}
			//LimitSale
			CEGUI::Window* limitSale = pgWnd->getChildRecursive(SHOPCITY_BUY_LIMITSALE);
			if(limitSale)
			{
				limitSale->setText(CEGUI::PropertyHelper::intToString(gd->numlimit));
			}
			//limitbuy
			CEGUI::Window* limitBuy = pgWnd->getChildRecursive(SHOPCITY_BUY_LIMITBUY);
			if(limitBuy)
			{
				limitBuy->setText(CEGUI::PropertyHelper::intToString(gd->buylimit));
			}
			//limittime
			CEGUI::Window* limitTime = pgWnd->getChildRecursive(SHOPCITY_BUY_LIMITTIME);
			if(limitTime)
			{
				//limitTime->setText(gd->timelimit.c_str());
				SetText(limitTime, gd->timelimit.c_str());
			}
			//Desc
			CEGUI::Window* desc = pgWnd->getChildRecursive(SHOPCITY_BUY_DESC);
			if(desc)
			{
				if(tg2)
					//desc->setText(tg2->BaseProperty.strContent.c_str());
					SetText(desc, tg2->BaseProperty.strContent.c_str());
			}
		}
	}
	return true;
}

bool OnShopCityBuySubmit(const CEGUI::EventArgs& e)
{
	//获得购买量
	CEGUI::Window* buynum = GetWindow(SHOPCITY_BUY_NUMEDBOX);
	uint cnt = 0;
	if(buynum)
	{
		cnt = CEGUI::PropertyHelper::stringToUint(buynum->getText());
	}
	ShopCityMsgMgr& scmgr = GetInst(ShopCityMsgMgr);
	//获得付费类型
	//商城类型
	SCGData::eSCType shopcityType = GetShopCityTypeByTabContentSelIndex();
	//获得购买物品ID
	uint index = 0; 
	CEGUI::Window* pgWnd = GetWindow(SHOPCITY_BUY_PAGE_NAME);
	index = pgWnd->getID();
	CGoodsList::tagGoods2* tg2 = CGoodsList::GetProperty(index);
	if(tg2)
	{
		scmgr.Send_BuyQuery(tg2->BaseProperty.dwIndex,cnt,shopcityType);
	}
	FireUIEvent(SHOPCITY_BUY_PAGE_NAME, EVENT_CLOSE);

	return true;
}

bool OnShopCityBuyClear(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pgWnd = WEArgs(e).window;
	//标题
	CEGUI::Window* name = pgWnd->getChildRecursive(SHOPCITY_BUY_NAME);
	if(name)
	{
		name->setText("");
	}
	//Icon
	CEGUI::Window* Icon = pgWnd->getChildRecursive(SHOPCITY_BUY_ICON);
	if(Icon)
	{
		Icon->setProperty("Image","");
	}
	//price
	CEGUI::Window* price = pgWnd->getChildRecursive(SHOPCITY_BUY_PRICE);
	if(price)
	{
		price->setText("");
	}
	//PriceIcon
	CEGUI::Window* priceIcon = pgWnd->getChildRecursive(SHOPCITY_BUY_PRICE_ICON);
	if(priceIcon)
	{
		priceIcon->setProperty("Image","");
	}
	//NowPrice
	CEGUI::Window* nowPrice = pgWnd->getChildRecursive(SHOPCITY_BUY_NOWPRICE);
	if(nowPrice)
	{
		nowPrice->setText("");
	}
	//nowPriceIcon
	CEGUI::Window* nowpriceIcon = pgWnd->getChildRecursive(SHOPCITY_BUY_NOWPRICE_ICON);
	if(nowpriceIcon)
	{
		nowpriceIcon->setProperty("Image","");
	}
	//lable
	CEGUI::Window* label = pgWnd->getChildRecursive(SHOPCITY_BUY_LABLE);
	if(label)
	{
		label->setText("");
	}
	//LimitSale
	CEGUI::Window* limitSale = pgWnd->getChildRecursive(SHOPCITY_BUY_LIMITSALE);
	if(limitSale)
	{
		limitSale->setText("");
	}
	//limitbuy
	CEGUI::Window* limitBuy = pgWnd->getChildRecursive(SHOPCITY_BUY_LIMITBUY);
	if(limitBuy)
	{
		limitBuy->setText("");
	}
	//limittime
	CEGUI::Window* limitTime = pgWnd->getChildRecursive(SHOPCITY_BUY_LIMITTIME);
	if(limitTime)
	{
		limitTime->setText("");
	}
	//Desc
	CEGUI::Window* desc = pgWnd->getChildRecursive(SHOPCITY_BUY_DESC);
	if(desc)
	{
		desc->setText("");
	}
	//buyNumEdbox
	CEGUI::Window* edbox = pgWnd->getChildRecursive(SHOPCITY_BUY_NUMEDBOX);
	{
		edbox->setText(CEGUI::PropertyHelper::intToString(0));
	}
	return true;
}

bool OnShopCityBuyBtnMinus(const CEGUI::EventArgs& e)
{
	CEGUI::Window* edbox = GetWindow(SHOPCITY_BUY_NUMEDBOX);
	if(edbox)
	{
		int i = CEGUI::PropertyHelper::stringToInt(edbox->getText());
		//i = max(0,i-1);
		edbox->setText(CEGUI::PropertyHelper::intToString(max(0,i-1)));
	}
	return true;
}

bool OnShopCityBuyBtnAdd(const CEGUI::EventArgs& e)
{
	CEGUI::Window* edbox = GetWindow(SHOPCITY_BUY_NUMEDBOX);
	if(edbox)
	{
		uint i = CEGUI::PropertyHelper::stringToInt(edbox->getText());
		edbox->setText(CEGUI::PropertyHelper::intToString(i+1));
	}
	return true;
}

bool OnShopCityBuyClosed(const CEGUI::EventArgs& e)
{
	//恢复ShopCity 窗口可用
	CEGUI::Window* shopcity = GetWindow(SHOPCITY_TAB_NAME);
	shopcity->setEnabled(true);

	return true;
}