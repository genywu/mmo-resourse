#include "StdAfx.h"
#include "../UIDef.h"

#include "../GamePage/Package/PackagePage.h"
#include "../GamePage/ChatList/ChatListPage.h"

#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/ClientRegion.h"
#include "../../GameClient/ClientApp/MainPlayerHand.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
#include "../../GameClient/ClientApp/TradeSystem/PlayerShop.h"

CEGUI::Window* InitPlayerShopOrderPage(void);							///< 初始化个人商店商品清单界面
void SubscribePlayerShopOrderPageEvent(CEGUI::Window* mainPage);		///< 个人商店商品清单界面事件注册

bool OnPlayerShopOrderPageOpen(const CEGUI::EventArgs& e);				///< 打开个人商店设置清单界面
bool OnPlayerShopOrderPageClose(const CEGUI::EventArgs& e);				///< 关闭个人商店设置清单界面

bool OnPlayerShopOrderOk(const CEGUI::EventArgs& e);					///< 确定
bool OnPlayerShopOrderClose(const CEGUI::EventArgs& e);					///< 关闭

bool OnPlayerShopOrderGoodsPriceChange(const CEGUI::EventArgs& e);		///< 价格变更
bool OnPlayerShopOrderGoodsSaleNumChange(const CEGUI::EventArgs& e);	///< 组数变更
bool OnPlayerShopOrderOneGroupNumChange(const CEGUI::EventArgs& e);		///< 单组个数变更

bool OnPlayerShopOrderGoodsSaleNumMaxClicked(const CEGUI::EventArgs& e);		///< 组数Max按钮
bool OnPlayerShopOrderGoodsOneGroupMaxNumClicked(const CEGUI::EventArgs& e);	///< 单组个数Max按钮

bool OnPlayerShopOrderTabChanged(const CEGUI::EventArgs& e);			///< 贩卖方式Tab页切换


void ClosePlayerShopOrderPage(void);									///< 清除清单界面

CEGUI::Window* InitPlayerShopOrderPage(void)
{
#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("PlayerShopOrder.log");
#endif

	CEGUI::Window* wnd = LoadUI("PlayerShopOrder");

#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("CEGUI.log",true);
#endif

	SubscribePlayerShopOrderPageEvent(wnd);

	wnd->setVisible(false);
	return wnd;
}

void SubscribePlayerShopOrderPageEvent(CEGUI::Window* mainPage)
{
	if (!mainPage) return;
	
	// 确认按钮
	CEGUI::Window* singleOkBtn = mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 1/OkBtn");
	CEGUI::Window* groupOkBtn = mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 2/OkBtn");
	if (singleOkBtn && groupOkBtn )
	{
		singleOkBtn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopOrderOk));
		groupOkBtn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopOrderOk));
	}

	// 关闭按钮
	CEGUI::Window* singleCloseBtn = mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 1/CloseBtn");
	CEGUI::Window* groupCloseBtn = mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 2/CloseBtn");
	if (singleCloseBtn && groupCloseBtn )
	{
		singleCloseBtn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopOrderClose));
		groupCloseBtn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopOrderClose));
	}

	// 组数Max按钮
	CEGUI::Window* singleMaxSaleNumBtn = mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 1/MaxSaleNumBtn");
	CEGUI::Window* groupMaxSaleNumBtn = mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 2/MaxSaleNumBtn");
	if (singleMaxSaleNumBtn && groupMaxSaleNumBtn )
	{
		singleMaxSaleNumBtn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopOrderGoodsSaleNumMaxClicked));
		groupMaxSaleNumBtn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopOrderGoodsSaleNumMaxClicked));
	}

	// 单组个数Max按钮
	CEGUI::Window* groupOneGroupMaxNumBtn = mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 2/OneGroupMaxNumBtn");
	if (groupOneGroupMaxNumBtn)
	{
		groupOneGroupMaxNumBtn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopOrderGoodsOneGroupMaxNumClicked));
	}

	// 单组个数变更
	CEGUI::Editbox* oneGroupNum = WEditBox(mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 2/OneGroupNum"));
	if (oneGroupNum)
	{
		oneGroupNum->subscribeEvent(CEGUI::Window::EventDeactivated,
			CEGUI::Event::Subscriber(OnPlayerShopOrderOneGroupNumChange));
	}

	// 价格变更
	CEGUI::Editbox* singlePrice = WEditBox(mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 1/Price"));
	CEGUI::Editbox* groupPrice = WEditBox(mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 1/Price"));
	if (singlePrice && groupPrice)
	{	
		singlePrice->subscribeEvent(CEGUI::Window::EventDeactivated,
			CEGUI::Event::Subscriber(OnPlayerShopOrderGoodsPriceChange));
		groupPrice->subscribeEvent(CEGUI::Window::EventDeactivated,
			CEGUI::Event::Subscriber(OnPlayerShopOrderGoodsPriceChange));
	}

	// 组数变更
	CEGUI::Editbox* singleSaleNum = WEditBox(mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 1/SaleNum"));
	CEGUI::Editbox* groupSaleNum = WEditBox(mainPage->getChildRecursive("PlayerShopOrder/TabControl/Tab 2/SaleNum"));
	if (singleSaleNum && groupSaleNum)
	{	
		singleSaleNum->subscribeEvent(CEGUI::Window::EventDeactivated,
			CEGUI::Event::Subscriber(OnPlayerShopOrderGoodsSaleNumChange));
		groupSaleNum->subscribeEvent(CEGUI::Window::EventDeactivated,
			CEGUI::Event::Subscriber(OnPlayerShopOrderGoodsSaleNumChange));
	}

	// 切换贩卖方式标签
	CEGUI::Window* tabContorlWnd = mainPage->getChildRecursive("PlayerShopOrder/TabControl");
	if (tabContorlWnd)
	{
		tabContorlWnd->subscribeEvent(CEGUI::TabControl::EventSelectionChanged,
			CEGUI::Event::Subscriber(OnPlayerShopOrderTabChanged));
	}

	// 主界面打开&关闭操作
	mainPage->subscribeEvent("Opened", CEGUI::Event::Subscriber(OnPlayerShopOrderPageOpen));
	//mainPage->subscribeEvent("Closed", CEGUI::Event::Subscriber(OnPlayerShopOrderPageClose));
}


bool OnPlayerShopOrderPageOpen(const CEGUI::EventArgs& e)
{
	CEGUI::Window* mainPage = WEArgs(e).window;
	if(!mainPage) return false;

	mainPage->setAlwaysOnTop(true);

	CEGUI::Window* wnd;
	char tempText[256];
	char strImageFilePath[128] = "";
	char strImageFileName[128] = "";

	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		PlayerShop::stCacheGoodsInfo* cacheGoodsInfo = GetPlayerShop().GetCacheGoodsInfo(orderGoods->GetIndex());
		if (cacheGoodsInfo!=NULL)
		{
			// 不可叠加物品
			if (!orderGoods->HasEffect(GAP_MAXOVERLAP))
			{
				CEGUI::Window* groupTabWnd = GetWndMgr().getWindow("PlayerShopOrder/TabControl/Tab 2");
				groupTabWnd->disable();
			}

			for (int i=1; i<=PlayerShop::TT_NUM; ++i)
			{
				// 物品图片
				sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/Icon", i);
				CEGUI::DefaultWindow *deWnd = WDefaultWindow(GetWndMgr().getWindow(tempText));
				if (deWnd)
				{
					// 获得当前背包栏对应的物品图标数据，并将该图标设置成对应背包组件的额外图片。
					const char *strIconPath = GetGame()->GetPicList()->GetPicFilePathName(CPicList::PT_GOODS_ICON, orderGoods->GetProperty()->dwIconId);
					GetFilePath(strIconPath,strImageFilePath);
					GetFileName(strIconPath,strImageFileName);
					CEGUI::String strImagesetName = "GoodIcon/";
					strImagesetName += strImageFileName;
					SetBackGroundImage(deWnd, strImagesetName.c_str(),strImageFilePath,strImageFileName);
				}

				// 物品名字
				sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/Name", i);
				wnd = GetWndMgr().getWindow(tempText);
				if (wnd)
				{
					char strGoodsName[64] = "";
					DWORD dwNameSize = (DWORD)strlen(orderGoods->GetBaseProperty()->strName.c_str());
					if (dwNameSize>20)
					{
						_snprintf(strGoodsName,21,"%s",orderGoods->GetBaseProperty()->strName.c_str());
						sprintf((strGoodsName+20),"...");
					}else
						sprintf(strGoodsName,"%s",orderGoods->GetBaseProperty()->strName.c_str());
					wnd->setText(ToCEGUIString(strGoodsName));
				}

				// 价格
				sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/Price", i);
				wnd = GetWndMgr().getWindow(tempText);
				if (wnd)
				{
					if (cacheGoodsInfo->price!=0)
					{
						char strPrice[64] = "";
						sprintf(strPrice,"%d",cacheGoodsInfo->price);
						wnd->setText(ToCEGUIString(strPrice));
					}
				}

				// 组数
				sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/SaleNum", i);
				wnd = GetWndMgr().getWindow(tempText);
				if (wnd)
				{
					char szNum[32] = "";
					if (cacheGoodsInfo->treadeType == PlayerShop::TT_SINGLE)
					{
						if (orderGoods->HasEffect(GAP_MAXOVERLAP))
						{
							DWORD dwTotalNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
							if (cacheGoodsInfo->groupNum == 0)
							{
								wnd->setText("");
							}else if (cacheGoodsInfo->groupNum*cacheGoodsInfo->oneGroupNum<=dwTotalNum&&cacheGoodsInfo->oneGroupNum!=0)
							{
								sprintf(szNum,"%d",cacheGoodsInfo->groupNum);
								wnd->setText(ToCEGUIString(szNum));
							}else
							{
								cacheGoodsInfo->groupNum = dwTotalNum;
								sprintf(szNum,"%d",cacheGoodsInfo->groupNum);
								wnd->setText(ToCEGUIString(szNum));
							}
						}else
						{
							cacheGoodsInfo->groupNum = 1;
							wnd->setText("1");
						}
					}
					else if (cacheGoodsInfo->treadeType == PlayerShop::TT_GROUP)
					{
						if (orderGoods->HasEffect(GAP_MAXOVERLAP))
						{
							DWORD dwTotalNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
							if (cacheGoodsInfo->groupNum == 0)
							{
								wnd->setText("");
							}else if (cacheGoodsInfo->groupNum*cacheGoodsInfo->oneGroupNum<=dwTotalNum&&cacheGoodsInfo->oneGroupNum!=0)
							{
								sprintf(szNum,"%d",cacheGoodsInfo->groupNum);
								wnd->setText(ToCEGUIString(szNum));
							}else
							{
								cacheGoodsInfo->groupNum = dwTotalNum;
								sprintf(szNum,"%d",cacheGoodsInfo->groupNum);
								wnd->setText(ToCEGUIString(szNum));
							}
						}
					}
				}

				// 持有数量
				sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/HaveNum", i);
				wnd = GetWndMgr().getWindow(tempText);
				if (wnd)
				{
					// 玩家身上有此物品总数是多少
					DWORD dwCurTotalNum = 1;
					if (orderGoods->HasEffect(GAP_MAXOVERLAP))
					{
						dwCurTotalNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
					}
					char szCurTotalNum[32] = "";
					sprintf(szCurTotalNum,AppFrame::GetText("Shop_20")  //持有数量：%d
                        ,dwCurTotalNum);
					wnd->setText(ToCEGUIString(szCurTotalNum));
				}
			}

			// 单组个数
			wnd = GetWndMgr().getWindow("PlayerShopOrder/TabControl/Tab 2/OneGroupNum");
			if (wnd)
			{
				char szNum[32] = "";
				if (cacheGoodsInfo->treadeType == PlayerShop::TT_GROUP)
				{
					DWORD dwTotalNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
					if (cacheGoodsInfo->oneGroupNum == 0)
					{
						wnd->setText("");
					}else if(cacheGoodsInfo->groupNum*cacheGoodsInfo->oneGroupNum<=dwTotalNum&&cacheGoodsInfo->groupNum!=0)
					{
						sprintf(szNum,"%d",cacheGoodsInfo->oneGroupNum);
						wnd->setText(szNum);
					}else
					{
						cacheGoodsInfo->oneGroupNum = 1;
						wnd->setText("1");
					}

				}
			}
		}
	}
	return true;
}

bool OnPlayerShopOrderPageClose(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	ClosePlayerShopOrderPage();

	return true;
}

bool OnPlayerShopOrderOk(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	return GetPlayerShop().OnShopOrderOK();
}

bool OnPlayerShopOrderClose(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	ClosePlayerShopOrderPage();
	FireUIEvent("PlayerShopOrder", "Closed");

	return true;
}

void ClosePlayerShopOrderPage(void)
{
	CEGUI::Window* wnd;
	char tempText[256];

	for (int i=1; i<=PlayerShop::TT_NUM; ++i)
	{
		sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/Icon", i);
		wnd = GetWndMgr().getWindow(tempText);
		if (wnd)
			wnd->setProperty("Image","");

		sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/Name", i);
		wnd = GetWndMgr().getWindow(tempText);
		if (wnd)
			wnd->setText("");

		sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/Price", i);
		wnd = GetWndMgr().getWindow(tempText);
		if (wnd)
			wnd->setText("");

		sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/SaleNum", i);
		wnd = GetWndMgr().getWindow(tempText);
		if (wnd)
			wnd->setText("");

		sprintf(tempText, "PlayerShopOrder/TabControl/Tab %d/HaveNum", i);
		wnd = GetWndMgr().getWindow(tempText);
		if (wnd)
			wnd->setText("");
	}

	wnd = GetWndMgr().getWindow("PlayerShopOrder/TabControl/Tab 2/OneGroupNum");
	if (wnd)
		wnd->setText("");

	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		if (orderGoods->IsLocked())
		{
			orderGoods->UnLock();
		}
		orderGoods->SetHaveShadowState(false);
		// @todo 背包
		//GetGame()->GetCGuiEx()->GetItemPageEx()->UpdateGoodsShowByCGoods(orderGoods);
	}
	GetPlayerShop().SetOrderGoods(NULL);
}

bool OnPlayerShopOrderGoodsPriceChange(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::String price = wnd->getText();
	char str[32] = "";

	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		PlayerShop::stCacheGoodsInfo* cacheGoodsInfo = GetPlayerShop().GetCacheGoodsInfo(orderGoods->GetIndex());
		// 检查物品数输入框是否合法
		ulong inputNum = (ulong)atoi(price.c_str());
		ulong coinMaxNum = 2000000000;
		// 输入值超出最大值
		if(inputNum >= coinMaxNum)
		{
			sprintf(str,"%d",coinMaxNum);
			wnd->setText(str);
			cacheGoodsInfo->price = coinMaxNum;
			return true;
		}
		// 输入值小于0
		else if(inputNum <= 0)
		{
			wnd->setText("1");
			cacheGoodsInfo->price = 0;
			return true;
		}
		else
		{
			sprintf(str,"%d",inputNum);
			wnd->setText(str);
			cacheGoodsInfo->price = inputNum;
			return true;
		}
	}

	return false;
}

bool OnPlayerShopOrderGoodsSaleNumChange(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::String saleNum = wnd->getText();
	char str[32] = "";

	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		PlayerShop::stCacheGoodsInfo* cacheGoodsInfo = GetPlayerShop().GetCacheGoodsInfo(orderGoods->GetIndex());
		// 检查物品数输入框是否合法
		ulong inputNum = (ulong)atoi(saleNum.c_str());
		// 玩家身上有此物品总数是多少
		ulong haveGoodsNum = 1;
		if (orderGoods->HasEffect(GAP_MAXOVERLAP))
		{
			haveGoodsNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
		}
		if (cacheGoodsInfo->treadeType==PlayerShop::TT_GROUP)
		{
			if (cacheGoodsInfo->oneGroupNum>0)
			{
				haveGoodsNum/=cacheGoodsInfo->oneGroupNum;
			}
		}
		// 输入值超出最大值
		if(inputNum >= haveGoodsNum)
		{
			sprintf(str,"%d",haveGoodsNum);
			wnd->setText(str);
			cacheGoodsInfo->groupNum = haveGoodsNum;
			return true;
		}
		// 输入值小于0
		else if(inputNum <= 0)
		{
			wnd->setText("1");
			cacheGoodsInfo->groupNum = 1;
			return true;
		}
		else
		{
			sprintf(str,"%d",inputNum);
			wnd->setText(str);
			cacheGoodsInfo->groupNum = inputNum;
			return true;
		}
	}
	return false;
}

bool OnPlayerShopOrderOneGroupNumChange(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::String oneGroupNum = wnd->getText();
	char str[32] = "";

	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		PlayerShop::stCacheGoodsInfo* cacheGoodsInfo = GetPlayerShop().GetCacheGoodsInfo(orderGoods->GetIndex());
		if (cacheGoodsInfo->treadeType==PlayerShop::TT_GROUP)
		{
			// 检查物品数输入框是否合法
			ulong inputNum = (ulong)atoi(oneGroupNum.c_str());
			// 玩家身上有此物品总数是多少
			DWORD goodsNum = 1;
			if (orderGoods->HasEffect(GAP_MAXOVERLAP))
			{
				goodsNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
			}
			if (cacheGoodsInfo->groupNum>0)
			{
				goodsNum/=cacheGoodsInfo->groupNum;
			}
			// 输入值超出最大值
			if(inputNum >= goodsNum)
			{
				sprintf(str,"%d",goodsNum);
				wnd->setText(str);
				cacheGoodsInfo->oneGroupNum = goodsNum;
				return true;
			}
			// 输入值小于0
			else if(inputNum <= 0)
			{
				wnd->setText("1");
				cacheGoodsInfo->oneGroupNum = 1;
				return true;
			}
			else
			{
				sprintf(str,"%d",inputNum);
				wnd->setText(str);
				cacheGoodsInfo->oneGroupNum = inputNum;
				return true;
			}
		}
	}
	return false;
}

bool OnPlayerShopOrderGoodsSaleNumMaxClicked(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::Window* saleNumItem;
	char str[32] = "";
	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		PlayerShop::stCacheGoodsInfo* cacheGoodsInfo = GetPlayerShop().GetCacheGoodsInfo(orderGoods->GetIndex());
		// 玩家身上有此物品总数是多少
		ulong goodsNum = 1;
		if (orderGoods->HasEffect(GAP_MAXOVERLAP))
		{
			goodsNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
		}
		if (cacheGoodsInfo->treadeType==PlayerShop::TT_SINGLE)
		{
			saleNumItem = GetWndMgr().getWindow("PlayerShopOrder/TabControl/Tab 1/SaleNum");
		}
		else if (cacheGoodsInfo->treadeType==PlayerShop::TT_GROUP)
		{
			if (cacheGoodsInfo->groupNum>0)
			{
				goodsNum/=cacheGoodsInfo->groupNum;
			}
			saleNumItem =GetWndMgr().getWindow("PlayerShopOrder/TabControl/Tab 2/SaleNum");
		}

		if (saleNumItem!=NULL)
		{
			sprintf(str,"%d",goodsNum);
			saleNumItem->setText(str);
			cacheGoodsInfo->groupNum = goodsNum;
		}
	}
	return true;
}

bool OnPlayerShopOrderGoodsOneGroupMaxNumClicked(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	char str[32] = "";
	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		PlayerShop::stCacheGoodsInfo* cacheGoodsInfo = GetPlayerShop().GetCacheGoodsInfo(orderGoods->GetIndex());
		if (cacheGoodsInfo->treadeType==PlayerShop::TT_GROUP)
		{
			// 玩家身上有此物品总数是多少
			ulong goodsNum = 1;
			if (orderGoods->HasEffect(GAP_MAXOVERLAP))
			{
				goodsNum = GetGame()->GetMainPlayer()->GetGoodsAmount(orderGoods->GetIndex());
			}
			
			CEGUI::Window* oneGroupNumItem = GetWndMgr().getWindow("PlayerShopOrder/TabControl/Tab 2/OneGroupNum");
			if (oneGroupNumItem!=NULL)
			{
				if (cacheGoodsInfo->groupNum>0)
				{
					goodsNum/=cacheGoodsInfo->groupNum;
				}
				sprintf(str,"%d",goodsNum);
				oneGroupNumItem->setText(str);
				cacheGoodsInfo->oneGroupNum = goodsNum;
			}
		}
	}
	return true;
}

bool OnPlayerShopOrderTabChanged(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::TabControl* tab = static_cast<CEGUI::TabControl*>(wnd);
	size_t tabIndex = tab->getSelectedTabIndex();
	CEGUI::Window* selectedWnd = tab->getTabContentsAtIndex(tabIndex);
	if (!selectedWnd) return false;

	CGoods* orderGoods = GetPlayerShop().GetOrderGoods();
	if (orderGoods!=NULL)
	{
		PlayerShop::stCacheGoodsInfo* cacheGoodsInfo = GetPlayerShop().GetCacheGoodsInfo(orderGoods->GetIndex());
		if (tabIndex == PlayerShop::TT_SINGLE)
		{
			cacheGoodsInfo->treadeType = PlayerShop::TT_SINGLE;
		}
		else if (tabIndex == PlayerShop::TT_GROUP)
		{
			cacheGoodsInfo->treadeType = PlayerShop::TT_GROUP;
		}
	}
	
	return true;
}