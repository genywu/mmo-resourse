#include "StdAfx.h"
#include "../UIDef.h"

#include "../GamePage/Package/PackagePage.h"
#include "../GamePage/ChatList/ChatListPage.h"

#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/MainPlayerHand.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
#include "../../GameClient/ClientApp/TradeSystem/PlayerShop.h"
#include "../../../Public/Common/DepotDef.h"

CEGUI::Window* InitPlayerShopPage(void);							///< 初始化个人商店界面
void SubscribePlayerShopPageEvent(CEGUI::Window* mainPage);			///< 个人商店界面事件注册
bool OnPlayerShopPageOpen(const CEGUI::EventArgs& e);				///< 打开个人商店
bool OnPlayerShopPageClose(const CEGUI::EventArgs& e);				///< 关闭个人商店
bool OnPlayerShopBlackboard(const CEGUI::EventArgs& e);				///< 打开/关闭留言板
bool OnHideShowPlayerShop(const CEGUI::EventArgs& e);				///< 隐藏/显示个人商店
bool OnBeginStopPlayerShop(const CEGUI::EventArgs& e);				///< 开始贩卖/停止贩卖

bool OnPlayerShopOperBtnUpdate(const CEGUI::EventArgs& e);			///< 个人商品界面操作按钮更新
bool OnPlayerShopClearGoodsInfo(const CEGUI::EventArgs& e);			///< 个人商品物品详情信息清空
bool OnPlayerShopUpdateMoneyInfo(const CEGUI::EventArgs& e);		///< 个人商店更新货币信息
bool OnPlayerShopOrderGoods(const CEGUI::EventArgs& e);				///< 个人商店物品上架
bool OnPlayerShopResetGoods(const CEGUI::EventArgs& e);				///< 双击重设上架物品信息
bool OnPlayerShopShowGoodsInfo(const CEGUI::EventArgs& e);			///< 左键单击查看物品信息
bool OnPlayerShopAddBuyNum(const CEGUI::EventArgs& e);				///< 增加购买数量按钮事件
bool OnPlayerShopSubBuyNum(const CEGUI::EventArgs& e);				///< 减少购买数量按钮事件
bool OnPlayerShopBuyNumChange(const CEGUI::EventArgs& e);			///< 购买数量变更


void SetPlayerShopGoodsItemInfo(PlayerShop::tagGoodsItem& tgGoodsItem, int index); ///< 添加货物列表

CEGUI::Window* InitPlayerShopPage(void)
{
#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("PlayerShop.log");
#endif

	CEGUI::Window* wnd = LoadUI("PlayerShop");

#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("CEGUI.log",true);
#endif

	SubscribePlayerShopPageEvent(wnd);

	wnd->setVisible(false);
	return wnd;
}

void SubscribePlayerShopPageEvent(CEGUI::Window* mainPage)
{
	if (!mainPage)
		return;

	CEGUI::Window* wnd;
	char tempText[256];

	// 注册打开/关闭留言板按钮事件
	wnd = WPushButton(mainPage->getChildRecursive("PlayerShop/backgrond/Blackboard"));
	if (wnd)
	{
		wnd->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopBlackboard));
	}

	// 注册隐藏/显示商店按钮事件
	wnd = WPushButton(mainPage->getChildRecursive("PlayerShop/backgrond/HideShowBtn"));
	if (wnd)
	{
		wnd->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnHideShowPlayerShop));
	}

	// 开始贩卖/停止贩卖按钮事件
	wnd = WPushButton(mainPage->getChildRecursive("PlayerShop/backgrond/BeginStopBtn"));
	if (wnd)
	{
		wnd->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnBeginStopPlayerShop));
	}

	// 关闭/离开按钮事件
	wnd = WPushButton(mainPage->getChildRecursive("PlayerShop/backgrond/CloseBtn"));
	if (wnd)
	{
		wnd->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnPlayerShopPageClose));
	}

	// 主界面打开&关闭操作
	mainPage->subscribeEvent("Opened", CEGUI::Event::Subscriber(OnPlayerShopPageOpen));
	mainPage->subscribeEvent("Closed", CEGUI::Event::Subscriber(OnPlayerShopPageClose));

	// 注册商店操作按钮更新事件
	mainPage->subscribeEvent("UpdateOperBtn", CEGUI::Event::Subscriber(OnPlayerShopOperBtnUpdate));
	// 注册更新交易货币显示事件
	mainPage->subscribeEvent("UpdateMoneyInfo", CEGUI::Event::Subscriber(OnPlayerShopUpdateMoneyInfo));
	// 注册个人商品物品详情信息清空事件
	mainPage->subscribeEvent("ClearGoodsInfo", CEGUI::Event::Subscriber(OnPlayerShopClearGoodsInfo));
	
	// 注册左键点击个人商店界面，开启设置物品清单界面事件
	CEGUI::Window* backgrondWnd = mainPage->getChild("PlayerShop/backgrond");
	backgrondWnd->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
		CEGUI::Event::Subscriber(OnPlayerShopOrderGoods));

	for (int i = 1; i <= GetPlayerShop().GetShopGridNum(); ++i)
	{
		// 物品显示框双击重设事件 & 左键单击显示物品信息事件
		sprintf(tempText, "PlayerShop/backgrond/Goods%d", i);
		wnd = mainPage->getChildRecursive(tempText);
		if (wnd)
		{
			wnd->subscribeEvent(CEGUI::Window::EventMouseDoubleClick,
				CEGUI::Event::Subscriber(OnPlayerShopResetGoods));

			wnd->subscribeEvent(CEGUI::Window::EventMouseClick,
				CEGUI::Event::Subscriber(OnPlayerShopShowGoodsInfo));
		}

		// 增加购买数量按钮事件
		sprintf(tempText, "PlayerShop/backgrond/Goods%d/AddBuyNum", i);
		wnd = mainPage->getChildRecursive(tempText);
		if (wnd)
		{
			wnd->subscribeEvent(CEGUI::PushButton::EventClicked,
				CEGUI::SubscriberSlot(OnPlayerShopAddBuyNum));
		}

		// 减少购买数量按钮事件
		sprintf(tempText, "PlayerShop/backgrond/Goods%d/SubBuyNum", i);
		wnd = mainPage->getChildRecursive(tempText);
		if (wnd)
		{
			wnd->subscribeEvent(CEGUI::PushButton::EventClicked,
				CEGUI::SubscriberSlot(OnPlayerShopSubBuyNum));
		}

		// 购买数量输入框
		sprintf(tempText, "PlayerShop/backgrond/Goods%d/BuyNum", i);
		wnd = mainPage->getChildRecursive(tempText);
		if (wnd)
		{
			wnd->subscribeEvent(CEGUI::Window::EventDeactivated,
				CEGUI::SubscriberSlot(OnPlayerShopBuyNumChange));
		}
	}
}

bool OnPlayerShopPageOpen(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	PlayerShop& playerShop = GetPlayerShop();

	// 打开个人商店前关闭其他界面（卡片镶嵌、制作、TalkBox、NPC商店、交易、仓库、装备、魔化等）

	// 打开背包

	playerShop.SetIsOpenShop(true);

	// 打开留言板界面
	FireUIEvent("PlayerShopMessageBoard", "Opened");

	// 更新操作界面按钮状态
	FireUIEvent("PlayerShop", "UpdateOperBtn");
	// 更新货币统计信息
	FireUIEvent("PlayerShop", "UpdateMoneyInfo");

	playerShop.UpdataPlayerShopGoodsList();

	return true;
}

bool OnPlayerShopPageClose(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	GetPlayerShop().OnCloseShop();
	return true;
}

bool OnPlayerShopBlackboard(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnHideShowPlayerShop(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	int curShopState = GetPlayerShop().GetCurShopState();

	if (curShopState==PlayerShop::SET_SHOP)
	{
	}
	else if (curShopState==PlayerShop::SHOPPING_SHOP)
	{
		// 确定要交易的物品
		if (!GetPlayerShop().GetbSetTrue())
		{		
			if(!GetPlayerShop().IsTradeItemReady())
				return false;
			GetPlayerShop().CalcTradeGolds();
			FireUIEvent("PlayerShop", "UpdateMoneyInfo");
			GetPlayerShop().SetbSetTrue(true);
			FireUIEvent("PlayerShop", "UpdateOperBtn");
			//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
		}
		// 取消要交易的物品
		else if (GetPlayerShop().GetbSetTrue())
		{
			GetPlayerShop().CancelAllTrade();
			FireUIEvent("PlayerShop", "UpdateMoneyInfo");
			GetPlayerShop().SetbSetTrue(false);
			FireUIEvent("PlayerShop", "UpdateOperBtn");
			//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
		}
	}

	return true;
}

bool OnBeginStopPlayerShop(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if (!pPlayer) return false;

	int curShopState = GetPlayerShop().GetCurShopState();
	// 开始摆摊
	if (curShopState==PlayerShop::SET_SHOP)
	{
		/// 判断玩家周围5*5的范围内是否有别的玩家已经在开店了，如果有则不容许开店
		if (pPlayer->CheckIsAnyBodyShopOpenedOnRange(2))
		{
			GetChatListPage().AddChatWords(AppFrame::GetText("Shop_16") //已经有玩家在您周围开店，请您重新选择开店地址
                , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
			FireUIEvent("PlayerShop", "Closed");
			return false;
		}
		// 发送摆摊消息
		//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");

		if (!pPlayer->IsRecruitingTeam())
		{
			GetPlayerShop().SetbSetTrue(true);
			GetPlayerShop().SendOpenShopMes();
		}else
			GetChatListPage().AddChatWords(AppFrame::GetText("Shop_12") //招募状态下不能开店
            , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
	}
	// 取消摆摊
	else if (curShopState==PlayerShop::OPEN_SHOP)
	{
		GetPlayerShop().SendCloseShopMes();
	}
	// 购买
	else if (curShopState==PlayerShop::SHOPPING_SHOP)
	{
		// 发送交易消息
		GetPlayerShop().SendBuyGoodsMes();
		//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
	}

	return true;
}

bool OnPlayerShopUpdateMoneyInfo(const CEGUI::EventArgs& e)
{
	// 目前个人商店只支持金币一种货币类型@todo
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if (!pPlayer) return false;

	CEGUI::Window* tradeGoldsWnd = wnd->getChildRecursive("PlayerShop/backgrond/SumUp/TradeGolds");
	CEGUI::Window* haveGoldsWnd = wnd->getChildRecursive("PlayerShop/backgrond/SumUp/HaveGolds");
	if (!tradeGoldsWnd || !haveGoldsWnd) return false;

	int curShopState = GetPlayerShop().GetCurShopState();
	ulonglong tradeGolds = GetPlayerShop().GetTradeGold();

	char str[256];
	if(curShopState==PlayerShop::SET_SHOP || curShopState==PlayerShop::OPEN_SHOP)
	{
		// 获得金币
		if (tradeGolds>=2000000000)
		{
			wsprintf(str,"+ %d",2000000000);
		}else
			wsprintf(str,"+ %d",tradeGolds);
		tradeGoldsWnd->setText(ToCEGUIString(str));

		// 持有金币
		if (pPlayer->GetMoney()>=2000000000)
		{
			wsprintf(str,"%d",2000000000);
		}else
			wsprintf(str,"%d",pPlayer->GetMoney());
		haveGoldsWnd->setText(ToCEGUIString(str));
	}
	else if(curShopState==PlayerShop::SHOPPING_SHOP)
	{
		// 花费金币
		if (tradeGolds>=2000000000)
		{
			wsprintf(str,"- %d",2000000000);
		}else
			wsprintf(str,"- %d",tradeGolds);
		tradeGoldsWnd->setText(ToCEGUIString(str));

		// 持有金币
		if (pPlayer->GetMoney()>=2000000000)
		{
			wsprintf(str,"%d",2000000000);
		}else
			wsprintf(str,"%d",pPlayer->GetMoney());
		haveGoldsWnd->setText(ToCEGUIString(str));
	}
	return true;
}

// 更新界面操作按钮状态
bool OnPlayerShopOperBtnUpdate(const CEGUI::EventArgs& e)
{
	CEGUI::Window* mainPage = WEArgs(e).window;
	if(!mainPage) return false;

	CEGUI::Window* messageBoardBtn = mainPage->getChildRecursive("PlayerShop/backgrond/Messageboard");
	CEGUI::Window* HideShowBtn = mainPage->getChildRecursive("PlayerShop/backgrond/HideShowBtn");
	CEGUI::Window* beginStopBtn = mainPage->getChildRecursive("PlayerShop/backgrond/BeginStopBtn");
	CEGUI::Window* closeBtn = mainPage->getChildRecursive("PlayerShop/backgrond/CloseBtn");
	if (!messageBoardBtn || !HideShowBtn || !beginStopBtn || !closeBtn) return false;

	PlayerShop& playerShop = GetPlayerShop();
	int shopState = playerShop.GetCurShopState();
	bool shopSetSure = playerShop.GetbSetTrue();
	if(shopState < 0 || shopState >= PlayerShop::SHOP_STATE)
		return false;

	vector<PlayerShop::tagGoodsItem>& myShopGoods = playerShop.GetMyShopGoods();

	// 设置商店
	if(shopState == PlayerShop::SET_SHOP)
	{
		if (!myShopGoods.empty())
		{
			// 如果商店中有物品就把开始摆摊显示出来
			beginStopBtn->setVisible(true);
			beginStopBtn->enable();
			beginStopBtn->setText(ToCEGUIString(AppFrame::GetText("开始贩卖")));
		}else
		{
			// 如果商店中有物品就让开始摆摊不显示出来
			beginStopBtn->setVisible(false);
		}
		HideShowBtn->setVisible(false);
	}
	// 打开商店
	else if(shopState == PlayerShop::OPEN_SHOP)
	{
		// 停止摆摊 & 隐藏商店显示出来
		beginStopBtn->setVisible(true);
		beginStopBtn->enable();
		beginStopBtn->setText(ToCEGUIString(AppFrame::GetText("停止贩卖")));
		HideShowBtn->setVisible(true);
		HideShowBtn->enable();
		HideShowBtn->setText(ToCEGUIString(AppFrame::GetText("隐藏商店")));
	}
	// 购买状态
	else if(shopState == PlayerShop::SHOPPING_SHOP)
	{	
		if (shopSetSure)
		{
			// 重设 & 购买显示出来
			beginStopBtn->setVisible(true);
			beginStopBtn->enable();
			beginStopBtn->setText(ToCEGUIString(AppFrame::GetText("购买")));
			HideShowBtn->setVisible(true);
			HideShowBtn->enable();
			HideShowBtn->setText(ToCEGUIString(AppFrame::GetText("重设")));
		}
		else
		{
			// 确定 & 购买显示出来
			beginStopBtn->setVisible(true);
			beginStopBtn->disable();
			beginStopBtn->setText(ToCEGUIString(AppFrame::GetText("购买")));
			HideShowBtn->setVisible(true);
			HideShowBtn->enable();
			HideShowBtn->setText(ToCEGUIString(AppFrame::GetText("确定")));
		}
	}

	return true;
}

bool OnPlayerShopOrderGoods(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
	if (!pPlayer || !pHand) return false;

	PlayerShop& playShop = GetPlayerShop();
	ChatListPage& chatListPage = GetChatListPage();
	int curShopState = playShop.GetCurShopState();
	int index = -1;

	FireUIEvent("PlayerShop", "ClearGoodsInfo");

	CGoods* pHandGoods = pHand->GetPGoodsOfMainPlayerHand();
	CPlayer::tagGoods* psTagGoods = pHand->GetTGoodsOfMainPlayerHand();
	// 如果手上有物品
	if (pHandGoods!=NULL && curShopState==PlayerShop::SET_SHOP && !playShop.GetbSetTrue())
	{
		if (pHand->GetTGoodsOfMainPlayerHand()->lType > DEPOT_CONTAINER_TYPE_BEGIN_VALUE  
			&&   pHand->GetTGoodsOfMainPlayerHand()->lType < DEPOT_CONTAINER_TYPE_END_VALUE )
		{
            chatListPage.AddChatWords(AppFrame::GetText("Shop_13")  //仓库中的物品不能直接出售！
                ,0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
			return false;
		}
		if (pHand->GetTGoodsOfMainPlayerHand()->lType==PEI_EQUIPMENT)
		{
			chatListPage.AddChatWords(AppFrame::GetText("Package_6")  //您得到了物品：%s
                , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
			return false;
		}
		if (pHand->GetTGoodsOfMainPlayerHand()->lType==PEI_SUBPACK)
		{
			chatListPage.AddChatWords(AppFrame::GetText("Package_16") //正在使用的子背包不能出售！
                , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
			return false;
		}
		//如果是勋章，不能放入
		if(pHandGoods->GetProperty()->dwType == GBT_MEDAL)
			return false;
		long lFreezeVal = pHandGoods->GetEffectVal(GAP_FREEZE,1);
		long lBindVal = pHandGoods->GetEffectVal(GAP_BIND,1);
		//冻结的物品不能放入个人商店进行交易
		if (lFreezeVal!=eFT_UnFrost&&lFreezeVal!=eFT_CantFrost)
		{
			GetInst(MsgEventManager).PushEvent(Msg_Ok, AppFrame::GetText("Goods_156")); //该物品处于冻结状态,不能出售!
			pHandGoods->SetHaveShadowState(false);
			// @todo 背包
			/*if (pItemPage!=NULL)
			{
				pItemPage->UpdateGoodsShowByCGoods(pHandGoods);
			}*/
			pHand->ClearMainPlayerHand();
			return false;
		}
		//绑定的物品不能放入个人商店进行交易
		if (lBindVal==1||lBindVal==3)
		{
			GetInst(MsgEventManager).PushEvent(Msg_Ok, AppFrame::GetText("Goods_157"));//该物品处于绑定状态,不能出售!
			pHandGoods->SetHaveShadowState(false);
			// @todo 背包
			//if (pItemPage!=NULL)
			//{
			//	pItemPage->UpdateGoodsShowByCGoods(pHandGoods);
			//	//pItemPage->UpdataAllItems();
			//	//pItemPage->UpdataAllSubPack();
			//}
			pHand->ClearMainPlayerHand();
			return false;
		}
		// 不可交易的不能放入个人商店中贩卖
		if (pHandGoods->GetEffectVal(GAP_PARTICULAR_ATTRIBUTE,1)&32)
		{
            chatListPage.AddChatWords(AppFrame::GetText("Shop_11")  //不可交易物品不能贩卖！
                , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
			return false;
		}

		// 打开商品贩卖清单界面
		if (index == -1 || (playShop.GetCurGoodsNum()<playShop.GetShopGridNum())) 
		{
			index = playShop.GetCurGoodsNum();
		}
		playShop.OnOrderPageOpen(pHandGoods,index);

		pHand->ClearMainPlayerHand();
	}
	return true;
}

// 清空商品详情中的显示
bool OnPlayerShopClearGoodsInfo(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::Window* iconWnd = wnd->getChildRecursive("PlayerShop/backgrond/GoodsInfo/Icon");
	CEGUI::Window* nameWnd = wnd->getChildRecursive("PlayerShop/backgrond/GoodsInfo/Name");
	CEGUI::Window* oneGroupNumWnd = wnd->getChildRecursive("PlayerShop/backgrond/GoodsInfo/OneGroupNum");
	CEGUI::Window* priceWnd = wnd->getChildRecursive("PlayerShop/backgrond/GoodsInfo/Price");
	CEGUI::Window* averagePriceWnd = wnd->getChildRecursive("PlayerShop/backgrond/GoodsInfo/AveragePrice");
	if (!iconWnd || !nameWnd || !oneGroupNumWnd || !priceWnd || !averagePriceWnd)
		return false;

	iconWnd->setProperty("Image","");
	nameWnd->setText("");
	oneGroupNumWnd->setText("");
	priceWnd->setText("");
	averagePriceWnd->setText("");

	return true;
}

// 添加货物列表
void SetPlayerShopGoodsItemInfo(PlayerShop::tagGoodsItem& tgGoodsItem, int index)
{
	/// 项目背景图

	CEGUI::Window* wnd;
	char tempText[256];
	char strImageFilePath[128] = "";
	char strImageFileName[128] = "";

	sprintf(tempText, "PlayerShop/backgrond/Goods%d", index+1);
	CEGUI::Window* goodsWnd = GetWndMgr().getWindow(tempText);
	goodsWnd->setUserData(tgGoodsItem.pItemGoods);
	goodsWnd->setVisible(true);

	//根据商店状态设置UI排列
	int shopState = GetPlayerShop().GetCurShopState();
	if(shopState >= 0 && shopState < PlayerShop::SHOP_STATE)
	{
		//设置商店
		if(shopState == PlayerShop::SET_SHOP)
		{
			sprintf(tempText, "PlayerShop/backgrond/Goods%d/BuyNum", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);
			
			sprintf(tempText, "PlayerShop/backgrond/Goods%d/AddBuyNum", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);

			sprintf(tempText, "PlayerShop/backgrond/Goods%d/SubBuyNum", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);

			sprintf(tempText, "PlayerShop/backgrond/Goods%d/Text", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setText(ToCEGUIString("双击重新设置价格"));
		}
		//开店
		else if(shopState == PlayerShop::OPEN_SHOP)
		{
			sprintf(tempText, "PlayerShop/backgrond/Goods%d/BuyNum", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);

			sprintf(tempText, "PlayerShop/backgrond/Goods%d/AddBuyNum", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);

			sprintf(tempText, "PlayerShop/backgrond/Goods%d/SubBuyNum", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);

			sprintf(tempText, "PlayerShop/backgrond/Goods%d/Text", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);
		}
		// 逛商店页面
		else if( shopState == PlayerShop::SHOPPING_SHOP)
		{
			sprintf(tempText, "PlayerShop/backgrond/Goods%d/Text", index+1);
			wnd = GetWndMgr().getWindow(tempText);
			wnd->setVisible(false);
		}
	}

	// 设置物品上架信息
	// 物品图片
	sprintf(tempText, "PlayerShop/backgrond/Goods%d/Icon", index+1);
	CEGUI::DefaultWindow* iconWnd = WDefaultWindow(GetWndMgr().getWindow(tempText));
	if(iconWnd && tgGoodsItem.goodsIconId != 0)
	{
		// 获得当前背包栏对应的物品图标数据，并将该图标设置成对应背包组件的额外图片。
		const char *strIconPath = GetGame()->GetPicList()->GetPicFilePathName(CPicList::PT_GOODS_ICON, tgGoodsItem.goodsIconId);
		GetFilePath(strIconPath,strImageFilePath);
		GetFileName(strIconPath,strImageFileName);
		CEGUI::String strImagesetName = "GoodIcon/";
		strImagesetName += strImageFileName;
		SetBackGroundImage(iconWnd,strImagesetName.c_str(),strImageFilePath,strImageFileName);

		// 当物品数大于1的时候才显示数量
		if(tgGoodsItem.tradeType==PlayerShop::TT_GROUP && tgGoodsItem.oneGroupNum>=1)
		{
			char strGoodsNum[32];
			sprintf_s(strGoodsNum,"%4d",tgGoodsItem.oneGroupNum);
			iconWnd->setText(strGoodsNum);
		}
	}

	// 物品数
	sprintf(tempText, "PlayerShop/backgrond/Goods%d/Num", index+1);
	wnd = GetWndMgr().getWindow(tempText);
	if (wnd)
	{	
		if (tgGoodsItem.tradeType==PlayerShop::TT_SINGLE)
		{
			sprintf_s(tempText,"剩%d件",tgGoodsItem.groupNum);
		}else if (tgGoodsItem.tradeType==PlayerShop::TT_GROUP)
		{
			sprintf_s(tempText,"剩%d组",tgGoodsItem.groupNum);
		}
		wnd->setText(ToCEGUIString(tempText));
	}
	
	// 物品名
	sprintf(tempText, "PlayerShop/backgrond/Goods%d/Name", index+1);
	wnd = GetWndMgr().getWindow(tempText);
	if (wnd)
	{
		char strGoodsName[32] = "";
		DWORD dwNameSize = (DWORD)strlen(tgGoodsItem.strGoodsName.c_str());
		if (tgGoodsItem.tradeType==PlayerShop::TT_SINGLE&&dwNameSize>18)
		{
			_snprintf(strGoodsName,19,"%s",tgGoodsItem.strGoodsName.c_str());
			sprintf((strGoodsName+18),"...");
		}else if (tgGoodsItem.tradeType==PlayerShop::TT_GROUP&&dwNameSize>10)
		{
			_snprintf(strGoodsName,11,"%s",tgGoodsItem.strGoodsName.c_str());
			sprintf((strGoodsName+10),"...");
		}
		else
			sprintf(strGoodsName,"%s",tgGoodsItem.strGoodsName.c_str());
		wnd->setText(ToCEGUIString(strGoodsName));
		wnd->setTooltipText(tgGoodsItem.strGoodsName);
	}

	// 交易方式
	sprintf(tempText, "PlayerShop/backgrond/Goods%d/TradeType", index+1);
	wnd = GetWndMgr().getWindow(tempText);
	if (wnd)
	{
		if (tgGoodsItem.tradeType==PlayerShop::TT_SINGLE)
		{
			wnd->setText(ToCEGUIString("单个贩卖"));
			//CEGUI::FreeTypeFont * font = static_cast<CEGUI::FreeTypeFont*>( wnd->getFont() );
			//font->setPointSize(8);
		}
		else if (tgGoodsItem.tradeType==PlayerShop::TT_GROUP)
		{
			wnd->setText(ToCEGUIString("成组贩卖"));
		}
	}

	// 价格
	sprintf(tempText, "PlayerShop/backgrond/Goods%d/Price", index+1);
	wnd = GetWndMgr().getWindow(tempText);
	if (wnd)
	{
		sprintf(tempText,"%d",tgGoodsItem.price);
		wnd->setText(tempText);
	}
}

bool OnPlayerShopResetGoods(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CGoods* goods = static_cast<CGoods*>(wnd->getUserData());
	if (!goods) return false;

	CPlayer *pPlayer = GetGame()->GetMainPlayer();
	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
	if (!pPlayer || !pHand) return false;
	
	int shopState = GetPlayerShop().GetCurShopState();
	if (shopState==PlayerShop::SET_SHOP)
	{
		GetPlayerShop().ReMoveShopGoods(goods->GetIndex(),goods->GetExID());
		wnd->setUserData(NULL);
		int index = GetPlayerShop().GetCurGoodsNum();
		GetPlayerShop().OnOrderPageOpen(goods, index);

		FireUIEvent("PlayerShop","ClearGoodsInfo");
		FireUIEvent("PlayerShop","UpdateMoneyInfo");

		return true;
	}

	return false;
}

bool OnPlayerShopShowGoodsInfo(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CGoods* goods = static_cast<CGoods*>(wnd->getUserData());
	if (!goods) return false;

	CEGUI::DefaultWindow* iconWnd = WDefaultWindow(GetWndMgr().getWindow("PlayerShop/backgrond/GoodsInfo/Icon"));
	CEGUI::Window* nameWnd = GetWndMgr().getWindow("PlayerShop/backgrond/GoodsInfo/Name");
	CEGUI::Window* oneGroupNumWnd = GetWndMgr().getWindow("PlayerShop/backgrond/GoodsInfo/OneGroupNum");
	CEGUI::Window* priceWnd = GetWndMgr().getWindow("PlayerShop/backgrond/GoodsInfo/Price");
	CEGUI::Window* averagePriceWnd = GetWndMgr().getWindow("PlayerShop/backgrond/GoodsInfo/AveragePrice");
	if (!iconWnd || !nameWnd || !oneGroupNumWnd || !priceWnd || !averagePriceWnd)
		return false;

	char tempText[256];
	char strImageFilePath[128] = "";
	char strImageFileName[128] = "";

	PlayerShop::tagGoodsItem* pGoodsItem = GetPlayerShop().FindtagGoods(goods);
	if (pGoodsItem!=NULL)
	{
		// 物品名字
		DWORD dwNameSize = (DWORD)strlen(pGoodsItem->strGoodsName.c_str());
		if (dwNameSize>20)
		{
			_snprintf(tempText,21,"%s", pGoodsItem->strGoodsName.c_str());
			sprintf((tempText+20),"...");
		}else
			sprintf(tempText,"%s", pGoodsItem->strGoodsName.c_str());
		nameWnd->setText(ToCEGUIString(tempText));

		// 物品图片
		const char *strIconPath = GetGame()->GetPicList()->GetPicFilePathName(CPicList::PT_GOODS_ICON, pGoodsItem->goodsIconId);
		GetFilePath(strIconPath,strImageFilePath);
		GetFileName(strIconPath,strImageFileName);
		CEGUI::String strImagesetName = "GoodIcon/";
		strImagesetName += strImageFileName;
		SetBackGroundImage(iconWnd,strImagesetName.c_str(),strImageFilePath,strImageFileName);

		// 物品单组个数
		sprintf(tempText,"%d",pGoodsItem->oneGroupNum);
		oneGroupNumWnd->setText(ToCEGUIString(tempText));

		// 物品售价
		sprintf(tempText,"%d", pGoodsItem->price);
		priceWnd->setText(ToCEGUIString(tempText));

		// 物品单个均价
		char strGoodsPrice[64] = "",strNumLen[64] = "";
		sprintf(strNumLen,"%d",pGoodsItem->price/pGoodsItem->oneGroupNum);
		float fPrice = static_cast<float>(pGoodsItem->price)/static_cast<float>(pGoodsItem->oneGroupNum);
		sprintf(strGoodsPrice,"%*.2f",(int)strlen(strNumLen)+2,fPrice);
		averagePriceWnd->setText(ToCEGUIString(strGoodsPrice));
	}

	return true;
}

bool OnPlayerShopAddBuyNum(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::Window* goodsWnd = wnd->getParent();
	if (goodsWnd)
	{	
		CGoods* goods = static_cast<CGoods*>(goodsWnd->getUserData());
		if (!goods) return false;

		PlayerShop::tagGoodsItem* pGoodsItem = GetPlayerShop().FindtagGoods(goods);
		if (pGoodsItem!=NULL)
		{
			char str[32];
			// 取得输入框控件名
			CEGUI::String name = wnd->getName();
			name.assign(name, 0, name.find_last_of("/"));
			name += "/BuyNum";

			CEGUI::Window* buyNumWnd = GetWndMgr().getWindow(name);
			ulong num = atoi(buyNumWnd->getText().c_str());

			if (num>=pGoodsItem->groupNum)
			{
				sprintf(str,"%d",num);
				wnd->disable();
			}
			else
				sprintf(str,"%d",++num);
			buyNumWnd->setText(ToCEGUIString(str));
			pGoodsItem->readyTradeNum = num;			
		}
	}

	return true;
}

bool OnPlayerShopSubBuyNum(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::Window* goodsWnd = wnd->getParent();
	if (goodsWnd)
	{	
		CGoods* goods = static_cast<CGoods*>(goodsWnd->getUserData());
		if (!goods) return false;

		PlayerShop::tagGoodsItem* pGoodsItem = GetPlayerShop().FindtagGoods(goods);
		if (pGoodsItem!=NULL)
		{
			char str[32];
			// 取得输入框控件名
			CEGUI::String name = wnd->getName();
			name.assign(name, 0, name.find_last_of("/"));
			name += "/BuyNum";

			CEGUI::Window* buyNumWnd = GetWndMgr().getWindow(name);
			int num = atoi(buyNumWnd->getText().c_str());

			if (num<=0)
			{
				sprintf(str,"%d",0);
				wnd->disable();
			}
			else
				sprintf(str,"%d",num--);
			buyNumWnd->setText(ToCEGUIString(str));
			pGoodsItem->readyTradeNum = num;	
		}
	}

	return true;
}

bool OnPlayerShopBuyNumChange(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	if(!wnd) return false;

	CEGUI::String buyNum = wnd->getText();
	char str[32] = "";

	CEGUI::Window* goodsWnd = wnd->getParent();
	if (goodsWnd)
	{	
		CGoods* goods = static_cast<CGoods*>(goodsWnd->getUserData());
		if (!goods) return false;

		PlayerShop::tagGoodsItem* pGoodsItem = GetPlayerShop().FindtagGoods(goods);
		if (pGoodsItem!=NULL)
		{
			ulong num = atoi(buyNum.c_str());

			if (num>=pGoodsItem->groupNum)
			{
				sprintf(str,"%d",pGoodsItem->groupNum);
			}
			else if (num<=0)
			{
				sprintf(str,"%d",0);
			}
				sprintf(str,"%d",num);
			wnd->setText(ToCEGUIString(str));
			pGoodsItem->readyTradeNum = num;	
		}
	}
	return true;
}