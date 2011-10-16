/**
* @filename:ShopCityItemSet.cpp
* @date: 2010/7/22
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 商城商店里具体某类物品的Item集合
*/
#include "stdafx.h"
#include "ShopCityDef.h"
#include "../UIDef.h"
#include "../../GameClient/ClientApp/ShopCity/ShopCitySys.h"
//GoodsList.h 包含了GoodsExDef.h 而goodsExDef.h用到了CGUI类，所以先包含一个GUID.h文件
#include "../../../Public/Module/GUID.h"
#include "../../GameClient/ClientApp/Goods/GoodsList.h"

using namespace CEGUI;
extern Window* GetWindow(const CEGUI::String& name);
void SetInitProShopCityItemSet(CEGUI::Window* pageWnd);
void SubEventShopCityItemSet(CEGUI::Window* pageWnd);

///根据传入的数据计算页码,并更新对应页数据，返回区间[starIdx,endIdx)
void CalcShopCityItemSetPage(SCGData::VecGD& vec,SCGData::eSCType& scType,
							 SCGData::eSType& sType,
							 int& startIdx,int& endIdx);
void UpdateShopCityItemSet_impl(SCGData::VecGD& vecGD,int startIdx,int endIdx,CEGUI::Window* pageWnd);

///清空所有Item显示数据
bool OnShopCityItemSetClear(const CEGUI::EventArgs& e);
///由选中的商城和商店类型更新ItemSet显示
bool OnShopCityItemSetUpdate(const CEGUI::EventArgs& e);
///根据选中的筛选文本更新Itemset显示
bool OnShopCityItemSetUpdateByFilter(const CEGUI::EventArgs& e);
///根据选中的导购文本更新Itemset显示
bool OnShopCityItemSetUpdateByGuide(const CEGUI::EventArgs& e);

///翻页
bool OnShopCityItemSetPrePage(const CEGUI::EventArgs& e);
bool OnShopCityItemSetNextPage(const CEGUI::EventArgs& e);

///点击试装事件
bool OnShopCityItemSetTryOn(const CEGUI::EventArgs& e);
///点击购买事件
bool OnShopCityItemSetBuy(const CEGUI::EventArgs& e);

///翻页Btn是否启用
void ShopCityEnabledPrePageBtn(bool enabled);
void ShopCityEnabledNextPageBtn(bool enabled);

///根据当前选中的TabContent索引得到商城类型
SCGData::eSCType GetShopCityTypeByTabContentSelIndex();
///根据当前选中的TabContent索引得到商店类型
SCGData::eSType  GetShopTypeByTabContentSelIndex();

Window* InitShopCityItemSet()
{
	Window* wnd = LoadUI(SHOPCITY_ITEMSET_PAGE_NAME);
	SubEventShopCityItemSet(wnd);
	//先注册事件，再设置初始化状态，因为初始化时调用了事件
	SetInitProShopCityItemSet(wnd);
	return wnd;
}

void SetInitProShopCityItemSet(CEGUI::Window* pageWnd)
{
	//清空Itemset
	FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_CLEAR);
}

void SubEventShopCityItemSet(CEGUI::Window* pageWnd)
{
	pageWnd->subscribeEvent(SHOPCITY_ITEMSET_EVENT_CLEAR,CEGUI::SubscriberSlot(OnShopCityItemSetClear));
	pageWnd->subscribeEvent(SHOPCITY_ITEMSET_EVENT_UPDATE,CEGUI::SubscriberSlot(OnShopCityItemSetUpdate));
	pageWnd->subscribeEvent(SHOPCITY_ITEMSET_EVENT_UPDATE_BY_FILTER,CEGUI::SubscriberSlot(OnShopCityItemSetUpdateByFilter));
	pageWnd->subscribeEvent(SHOPCITY_ITEMSET_EVENT_UPDATE_BY_GUIDE,CEGUI::SubscriberSlot(OnShopCityItemSetUpdateByGuide));
	CEGUI::Window* prepg = pageWnd->getChildRecursive(SHOPCITY_PREPAGE_NAME);
	if(prepg)
	{
		prepg->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityItemSetPrePage));
	}
	CEGUI::Window* nextpg = pageWnd->getChildRecursive(SHOPCITY_NEXTPAGE_NAME);
	if (nextpg)
	{
		nextpg->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityItemSetNextPage));
	}
	//试装和购买事件
	char name[256] = "";
	CEGUI::Window* tryOn = NULL;
	CEGUI::Window* buy = NULL;
	for(int i = 0 ; i < SHOPCITY_ITEMSET_ITEM_COUNT ; ++i)
	{
		//试装
		sprintf(name,SHOPCITY_ITEMSET_ITEM_TRYONBTN_D,i);
		tryOn = pageWnd->getChildRecursive(name);
		if(tryOn)
		{
			tryOn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityItemSetTryOn));
		}
		//购买
		sprintf(name,SHOPCITY_ITEMSET_ITEM_BUYBTN_D,i);
		buy = pageWnd->getChildRecursive(name);
		if(buy)
		{
			buy->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::SubscriberSlot(OnShopCityItemSetBuy));
		}
	}
}

bool OnShopCityItemSetClear(const CEGUI::EventArgs& e)
{
	//窗口名称
	char wndName[256] = "";
	CEGUI::Window* pageWnd = WEArgs(e).window;
	CEGUI::Window* tempWnd = NULL;
	CEGUI::Window* item = NULL;
	for(int i = 0 ; i < SHOPCITY_ITEMSET_ITEM_COUNT ; ++i)
	{
		//清空标题
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_TITLE_D,i);
		item = tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
		{
			tempWnd->setText("");
			//让Item不可见
			item->setVisible(false);
		}
		//背景图片
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_BGIMG_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setProperty("Image","");
		//热门图片
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_HOTIMG_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setProperty("Image","");
		//折扣图
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_DISCONT_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setProperty("Image","");
		//moneyType图
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_MONEYTYPE_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setProperty("Image","");
		//现价
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_PRICEEDBOX_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setText("");
		//hot
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_LABLEEDBOX_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setText("");
		//购买
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_BUYBTN_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setID(0);//重置ID
		sprintf(wndName,SHOPCITY_ITEMSET_ITEM_TRYONBTN_D,i);
		tempWnd = pageWnd->getChildRecursive(wndName);
		if(tempWnd)
			tempWnd->setID(0);//重置ID
	}
	return true;
}

bool OnShopCityItemSetUpdate(const CEGUI::EventArgs& e)
{
	//清空Itemset
	FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_CLEAR);
	CEGUI::Window* pageWnd = WEArgs(e).window;
	//商城类型
	SCGData::eSCType shopcityType = GetShopCityTypeByTabContentSelIndex();
	//商店类型
	SCGData::eSType shoptype = GetShopTypeByTabContentSelIndex();
	//获取数据
	ShopCityMsgMgr& scmm = GetInst(ShopCityMsgMgr);
	SCGData* dt = scmm.GetShopCityGoodsData();
	//size_t curPage = scmm.GetPageInfo(shopcityType,shoptype);//获取当前页记录[0,x)
	if(dt)
	{
		SCGData::VecGD vecGD;//获得数据
		if(shoptype != SCGData::TABTYPE_HOT)
			dt->GetVecGoodsData(shopcityType,shoptype,vecGD);
		else //如果是热销商品
			dt->GetTop10InGoodsData(shopcityType,vecGD);
		int startIdx = 0;
		int endIdx = 0;
		CalcShopCityItemSetPage(vecGD,shopcityType,shoptype,startIdx,endIdx);
		UpdateShopCityItemSet_impl(vecGD,startIdx,endIdx,pageWnd);
	}
	return true;
}
bool OnShopCityItemSetUpdateByGuide(const CEGUI::EventArgs& e)
{
	//清空Itemset
	FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_CLEAR);
	CEGUI::Window* pageWnd = WEArgs(e).window;
	//商城类型
	SCGData::eSCType shopcityType = GetShopCityTypeByTabContentSelIndex();
	////商店类型
	SCGData::eSType shoptype = GetShopTypeByTabContentSelIndex();
	//获取数据
	ShopCityMsgMgr& scmm = GetInst(ShopCityMsgMgr);
	SCGData* dt = scmm.GetShopCityGoodsData();
	//获取导购数据
	string guidstr;
	CEGUI::Combobox* guide = WComboBox(GetWindow(SHOPCITY_SEARCH_LEFTWND_NAME));
	if(guide)
	{
		//guidstr = guide->getEditbox()->getText().c_str();
		guidstr = CEGUIStringToAnsiChar(guide->getEditbox()->getText());
	}
	if(dt)
	{
		SCGData::VecGD vecGD;//获得物品数据
		dt->GetGuideInGoodsData(shopcityType,guidstr,vecGD);
		int startIdx = 0;
		int endIdx = 0;
		CalcShopCityItemSetPage(vecGD,shopcityType,shoptype,startIdx,endIdx);
		UpdateShopCityItemSet_impl(vecGD,startIdx,endIdx,pageWnd);
	}
	return true;
}
bool OnShopCityItemSetUpdateByFilter(const CEGUI::EventArgs& e)
{
	//清空Itemset
	FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_CLEAR);
	CEGUI::Window* pageWnd = WEArgs(e).window;
	//商城类型
	SCGData::eSCType shopcityType = GetShopCityTypeByTabContentSelIndex();
	//商店类型
	SCGData::eSType shoptype = GetShopTypeByTabContentSelIndex();
	//获取数据
	ShopCityMsgMgr& scmm = GetInst(ShopCityMsgMgr);
	SCGData* dt = scmm.GetShopCityGoodsData();
	//获取筛选文本
	string filterStr;
	CEGUI::Combobox* filter = WComboBox(GetWindow(SHOPCITY_SEARCH_RIGHTWND_NAME));
	if(filter)
	{
		//filterStr = filter->getEditbox()->getText().c_str();
		filterStr = CEGUIStringToAnsiChar(filter->getEditbox()->getText());
	}
	if(dt)
	{
		SCGData::VecGD vecGD;//获得物品数据
		dt->GetFilterInGoodsData(shopcityType,shoptype,filterStr,vecGD);
		int startIdx = 0;
		int endIdx = 0;
		CalcShopCityItemSetPage(vecGD,shopcityType,shoptype,startIdx,endIdx);
		UpdateShopCityItemSet_impl(vecGD,startIdx,endIdx,pageWnd);
	}
	return true;
}

bool OnShopCityItemSetPrePage(const CEGUI::EventArgs& e)
{
	//商城类型
	SCGData::eSCType shopcityType = GetShopCityTypeByTabContentSelIndex();
	//商店类型
	SCGData::eSType shoptype = GetShopTypeByTabContentSelIndex();
	ShopCityMsgMgr& scmgr = GetInst(ShopCityMsgMgr);
	int CurPG = scmgr.GetPageInfo(shopcityType,shoptype);
	scmgr.SetPageInfo(shopcityType,shoptype,CurPG-1);
	//由更新源类型来进行UI更新
	switch(scmgr.GetStateUpdateUIByType())
	{
	case 0:
		//更新ItemSet的显示
		FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE);
		break;
	case 1:
		FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE_BY_FILTER);
		break;
	case 2:
		FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE_BY_GUIDE);
		break;
	default:
		break;
	}	
	return true;
}

//
bool OnShopCityItemSetNextPage(const CEGUI::EventArgs& e)
{
	//商城类型
	SCGData::eSCType shopcityType = GetShopCityTypeByTabContentSelIndex();
	//商店类型
	SCGData::eSType shoptype = GetShopTypeByTabContentSelIndex();
	ShopCityMsgMgr& scmgr = GetInst(ShopCityMsgMgr);
	int CurPG = scmgr.GetPageInfo(shopcityType,shoptype);
	scmgr.SetPageInfo(shopcityType,shoptype,CurPG+1);
	//由更新源类型来进行UI更新
	switch(scmgr.GetStateUpdateUIByType())
	{
	case 0:
		//更新ItemSet的显示
		FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE);
		break;
	case 1:
		FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE_BY_FILTER);
		break;
	case 2:
		FireUIEvent(SHOPCITY_ITEMSET_PAGE_NAME,SHOPCITY_ITEMSET_EVENT_UPDATE_BY_GUIDE);
		break;
	default:
		break;
	}
	return true;
}

bool OnShopCityItemSetTryOn(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnShopCityItemSetBuy(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = GetWindow(SHOPCITY_BUY_PAGE_NAME);
	//关联物品ID
	CEGUI::Window* buy = WEArgs(e).window;
	wnd->setID(buy->getID());
	FireUIEvent(SHOPCITY_BUY_PAGE_NAME,EVENT_OPEN);
	return true;
}

void ShopCityEnabledPrePageBtn(bool enabled)
{
	CEGUI::Window* temp = GetWindow(SHOPCITY_PREPAGE_NAME);
	if(temp)
		temp->setEnabled(enabled);
}
void ShopCityEnabledNextPageBtn(bool enabled)
{
	CEGUI::Window* temp = GetWindow(SHOPCITY_NEXTPAGE_NAME);
	if(temp)
		temp->setEnabled(enabled);
}

SCGData::eSCType GetShopCityTypeByTabContentSelIndex()
{
	//商城类型
	SCGData::eSCType shopcityType = SCGData::ST_XINGZUAN;
	CEGUI::TabControl* ShopCityTab  = WTabControl(GetWindow(SHOPCITY_TAB_NAME));
	if(ShopCityTab)
	{
		shopcityType = SCGData::eSCType(ShopCityTab->getSelectedTabIndex());//商城类型同index关联 
	}
	return shopcityType;
}
SCGData::eSType GetShopTypeByTabContentSelIndex()
{
	//商店类型
	SCGData::eSType shoptype = SCGData::TABTYPE_HOT;
	CEGUI::TabControl* shopcityChildTab = WTabControl(GetWindow(SHOPCITY_CHILD_PAGE_NAME));
	if(shopcityChildTab)
	{
		shoptype = SCGData::eSType(shopcityChildTab->getSelectedTabIndex());
	}
	return shoptype;
}

void CalcShopCityItemSetPage(SCGData::VecGD& vecGD,SCGData::eSCType& scType, SCGData::eSType& sType, int& startIdx,int& endIdx)
{
	ShopCityMsgMgr& scmm = GetInst(ShopCityMsgMgr);
	SCGData* dt = scmm.GetShopCityGoodsData();
	int curPage = scmm.GetPageInfo(scType,sType);//获取当前页记录[0,x)
	//计算最大页数
	int maxPage = (int)vecGD.size() / SHOPCITY_ITEMSET_ITEM_COUNT;//[0,x)
	if((int)vecGD.size() % SHOPCITY_ITEMSET_ITEM_COUNT == 0)//能够整除
		maxPage--;
	//计算当前页,并更新页记录
	if(curPage > maxPage)
	{
		curPage = maxPage;
		scmm.SetPageInfo(scType,sType,(int)curPage);
	}
	else if( curPage < 0)
	{
		curPage = 0;
		scmm.SetPageInfo(scType,sType,(int)curPage);
	}
	//计算起始和结束索引
	if(maxPage == 0)//只有一页
	{
		startIdx = 0;
		endIdx =  min((int)vecGD.size(),SHOPCITY_ITEMSET_ITEM_COUNT);//结束索引取较小的数
		//上下页无效
		ShopCityEnabledPrePageBtn(false);
		ShopCityEnabledNextPageBtn(false);
	}
	else if(maxPage > 0)//大于1页
	{
		startIdx =curPage * SHOPCITY_ITEMSET_ITEM_COUNT;//起始位置
		if(curPage < maxPage && curPage != 0)//没有到最后一页并且不是第一页
		{
			endIdx = (curPage+1) * SHOPCITY_ITEMSET_ITEM_COUNT;
			ShopCityEnabledPrePageBtn(true);
			ShopCityEnabledNextPageBtn(true);
		}
		else if ( curPage == maxPage) //到了最后一页
		{
			ShopCityEnabledPrePageBtn(true);
			ShopCityEnabledNextPageBtn(false);//下页无效
			endIdx = (int)vecGD.size();
		}
		else if ( curPage == 0 )//到了第一页
		{
			endIdx = (curPage + 1 ) * SHOPCITY_ITEMSET_ITEM_COUNT;
			ShopCityEnabledPrePageBtn(false);//上页无效
			ShopCityEnabledNextPageBtn(true);
		}
	}//完成索引计算
}

void UpdateShopCityItemSet_impl(SCGData::VecGD& vecGD,int startIdx,int endIdx,CEGUI::Window* pageWnd)
{
	//更新Item
	char wndName[256] = "";
	CEGUI::Window* tempWnd = NULL;
	for(int i = 0  ; startIdx < endIdx ; ++i,++startIdx)
	{

		SCGData::tagGD& gd = vecGD[startIdx];
		CGoodsList::tagGoods2* tgg2 = CGoodsList::GetProperty(gd.index);
		if(tgg2){
			//标题(物品名称)
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_TITLE_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
			{
				//tempWnd->setText(tgg2->BaseProperty.strName.c_str());
				SetText(tempWnd, tgg2->BaseProperty.strName.c_str());
				//让Item不可见
				tempWnd->setVisible(true);
			}
			//背景图片
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_BGIMG_D,i);
			CEGUI::GUISheet* tpGSheet = WGUISheet(pageWnd->getChildRecursive(wndName));
			if(tpGSheet)
			{
				char imagesetname[256];
				sprintf(imagesetname,GOODS_PREFIXID,tgg2->BaseProperty.dwIconId);
				CEGUI::String imagename = CEGUI::PropertyHelper::intToString(tgg2->BaseProperty.dwIconId)+".jpg";
				SetBackGroundImage(tpGSheet,imagesetname,GOODS_ICON_PATH,imagename.c_str(),false);
			}
			//热门图片
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_HOTIMG_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
			{
				tempWnd->setProperty("Image","");
				tempWnd->setVisible(false);
			}
			//折扣图
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_DISCONT_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
			{
				tempWnd->setProperty("Image","");
				tempWnd->setVisible(false);
			}
			//moneyType图
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_MONEYTYPE_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
			{
				tempWnd->setProperty("Image","");
				tempWnd->setVisible(false);
			}
			////现价
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_PRICEEDBOX_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
			{
				//tempWnd->setText(CEGUI::PropertyHelper::intToString(gd.price));
				SetText(tempWnd, CEGUI::PropertyHelper::intToString(gd.price).c_str());
			}
			//hot
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_LABLEEDBOX_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
				//tempWnd->setText("HOT");
				SetText(tempWnd, "HOT");
			//购买
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_BUYBTN_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
				tempWnd->setID(gd.index);//id与物品ID关联
			sprintf(wndName,SHOPCITY_ITEMSET_ITEM_TRYONBTN_D,i);
			tempWnd = pageWnd->getChildRecursive(wndName);
			if(tempWnd)
				tempWnd->setID(gd.index);//id与物品ID关联
		}
	}
}