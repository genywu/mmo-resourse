/**
* @filename:Auction.cpp
* @date: 2010/5/4
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 位面寄售UI.
*/

#include "stdafx.h"
#include "../UIDef.h"
#include "../../GameClient/ClientApp/Auction/AuctionSys.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
#include "../../GameClient/ClientApp/Other/GlobeSetup.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/Game.h"


///手续费费率
#define AGENT_EXTRACUST CGlobeSetup::GetSetup()->agentExtracust 
///一单对应的位面数目
#define ORDER_PER_NUM CGlobeSetup::GetSetup()->oderPerNum

///位面寄售UI初始化
CEGUI::Window* InitAuction();

///设置UI控件的初始化属性
void SetInitializedAHUIProperty(CEGUI::Window* pageWnd);

///事件注册
void SubscriberAHEvent(CEGUI::Window* pageWnd);

///当界面打开的时候，自动请求一次数据
bool OnRequestData(const CEGUI::EventArgs& e);
///点击购买位面按钮事件处理函数
bool OnBuyWeimianBtnClicked(const CEGUI::EventArgs& e);

///点击出售位面按钮事件处理函数
bool OnSaleWeimianBtnClicked(const CEGUI::EventArgs& e);

///点击提取金币按钮事件处理函数
bool OnPickUpGold(const CEGUI::EventArgs& e);

///点击提取位面按钮事件处理函数
bool OnPickUpWeimian(const CEGUI::EventArgs& e);

///位面出售MCL（多行列表框)选中条目更改事件
bool OnSaleMCLSelChanged(const CEGUI::EventArgs& e);

///位面求购MCL选中条目更改事件
bool OnBuyMCLSelChanged(const CEGUI::EventArgs& e);

///委托界面MCL选中条目更改事件
bool OnPerMCLSelChanged(const CEGUI::EventArgs& e);

///关闭位面出售界面
bool OnSaleUIClosed(const CEGUI::EventArgs& e);

///打开位面出售界面
bool OpenSaleUI();

///位面出售界面,提交出售请求
bool OnSaleSubmit(const CEGUI::EventArgs& e);

///关闭位面购买界面
bool OnBuyUIClosed(const CEGUI::EventArgs& e);

///打开位面购买界面
bool OpenBuyUI();

///位面求购界面，提交求购请求
bool OnBuySubmit(const CEGUI::EventArgs& e);

///更新位面购买界面UI
bool UpdateBuyUIDate(const CEGUI::EventArgs& e);

///更新位面出售界面UI
bool UpdateSaleUIDate(const CEGUI::EventArgs& e);

///更新可购买位面列表MCLItem
bool UpdateCanBuyMCLDate(const CEGUI::EventArgs& e);

///更新可出售位面列表MCLItem
bool UpdateCanSaleMCLDate(const CEGUI::EventArgs& e);

///更新个人挂单MCLItem
bool UpdatePerMCLDate(const CEGUI::EventArgs& e);

///寄售界面，提交出售挂单请求
bool OnAgentBuySubmit(const CEGUI::EventArgs& e);

///寄售界面，提交出售挂单前，更新UI显示
bool UpdateAgentBuyUIDate(const CEGUI::EventArgs& e);

///寄售界面，提交求购挂单请求
bool OnAgentSaleSubmit(const CEGUI::EventArgs& e);

///寄售界面，提交出售挂单前，更新寄售UI提示
bool UpdateAgentSaleUIDate(const CEGUI::EventArgs& e);

///提取更新UI显示
bool UpdatePickUPUIDate(const CEGUI::EventArgs& e);

///个人委托界面，撤销挂单
bool OnCancelAgentOrder(const CEGUI::EventArgs& e);

///发送请求出售挂单列表
bool OnRefreshSaleMCL(const CEGUI::EventArgs& e);

///发送请求刷新求购挂单列表
bool OnRefreshBuyMCL(const CEGUI::EventArgs& e);

///发送请求个人挂单列表
bool OnRefreshPerMCL(const CEGUI::EventArgs& e);

///没重新打开界面时候，发送一次数据刷新请求
bool OnOpenedUpdate(const CEGUI::EventArgs& e);

CEGUI::Window* InitAuction()
{
	//加载界面
	CEGUI::Window* wnd = LoadUI("Auction");
	//设置UI控件的初始化属性
	SetInitializedAHUIProperty(wnd);
	//事件注册函数
	SubscriberAHEvent(wnd);
	//默认打开出售求购UI，请求一次所有数据,更新出售求购UI显示
	AHdata& ahd = GetInst(AHdata);
	ahd.Send_AH_REQUEST_ALL();
	return wnd;
}

void SetInitializedAHUIProperty(CEGUI::Window* pageWnd)
{
	//第一次打开窗口，默认可见
	pageWnd->setVisible(false);

	CEGUI::Window* tab1 = pageWnd->getChildRecursive("Auction/Tab/BuySale");
	tab1->setVisible(true);//默认Tab1可见

	CEGUI::Editbox* editbox = WEditBox(pageWnd->getChildRecursive("Auction/Pickup/EditGold"));//金币提取编辑框只能为数字
	editbox->setText("");
	editbox->setReadOnly(true);
	//editbox->setMaxTextLength(10);
	//只能输入数字
	editbox->setValidationString("[0-9]*");

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Pickup/EditWeimian"));//位面提取编辑框只能为数字
	editbox->setText("");
	editbox->setReadOnly(true);
	editbox->setValidationString("[0-9]*");
	//editbox->setMaxTextLength(10);
	CEGUI::MultiColumnList* mcl = WMCL(pageWnd->getChildRecursive("Auction/Tab/BuySale/SaleMCL"));
	mcl->setUserColumnDraggingEnabled(false);//列不可拖动
	mcl->setUserSortControlEnabled(false);//列不可排序
	mcl->setUserColumnSizingEnabled(false);//列不可调整宽度

	mcl = WMCL(pageWnd->getChildRecursive("Auction/Tab/BuySale/BuyMCL"));
	mcl->setUserColumnDraggingEnabled(false);//列不可拖动
	mcl->setUserSortControlEnabled(false);//列不可排序
	mcl->setUserColumnSizingEnabled(false);//列不可调整宽度

	CEGUI::Window* wnd = pageWnd->getChildRecursive("Auction/SaleWnd");
	wnd->setVisible(false);//出售界面默认不可见

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/SaleWnd/saleNum"));//出售界面编辑框只能输入数字
	editbox->setText("");
	editbox->setMaxTextLength(10);
	//editbox->setReadOnly(false);
	editbox->setValidationString("[0-9]*");
	editbox = WEditBox(pageWnd->getChildRecursive("Auction/SaleWnd/Text6"));//出售界面交易文本提示
	editbox->setText("");
	editbox = WEditBox(pageWnd->getChildRecursive("Auction/SaleWnd/Text61"));//
	editbox->setText("");
	

	wnd = pageWnd->getChildRecursive("Auction/BuyWnd");
	wnd->setVisible(false);//购买界面默认不可见

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Buy/buyNum"));//购买界面编辑框只能输入数字
	editbox->setText("");
	//editbox->setReadOnly(false);
	editbox->setValidationString("[0-9]*");
	editbox->setMaxTextLength(4);

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Buy/Text6"));//购买界面交易文本提示
	editbox->setText("");
	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Buy/Text61"));
	editbox->setText("");

	mcl = WMCL(pageWnd->getChildRecursive("Auction/Tab/Query/MCL"));//个人查询MCL属性
	mcl->setUserColumnDraggingEnabled(false);//列不可拖动
	mcl->setUserSortControlEnabled(false);//列不可排序
	mcl->setUserColumnSizingEnabled(false);//列不可调整宽度

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/sale/EditNum"));//编辑框只能输入数字
	editbox->setText("");
	//editbox->setReadOnly(false);
	editbox->setValidationString("[0-9]*");
	editbox->setMaxTextLength(5);

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/sale/EditPrice"));//编辑框只能输入数字
	editbox->setText("");
	//editbox->setReadOnly(false);
	editbox->setValidationString("[0-9]*");
	editbox->setMaxTextLength(5);

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/EditNum"));//编辑框只能输入数字
	editbox->setText("");
	//editbox->setReadOnly(false);
	editbox->setValidationString("[0-9]*");
	editbox->setMaxTextLength(5);

	editbox = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/EditPrice"));//编辑框只能输入数字
	editbox->setText("");
	//editbox->setReadOnly(false);
	editbox->setValidationString("[0-9]*");
	editbox->setMaxTextLength(5);

	//////////////////////////////////////////////////
	//委托页面支付提示
	wnd = pageWnd->getChildRecursive("Auction/Tab/Agent/sale/subNum");//
	wnd->setText("");
	wnd = pageWnd->getChildRecursive("Auction/Tab/Agent/sale/subGold");//
	wnd->setText("");
	wnd = pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/subNum");
	wnd->setText("");
	wnd = pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/subGold");
	wnd->setText("");
	wnd = pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/Text");
	wnd->setText("");
	////////////////////////////////////////////////////

	/////////////////////////////////////////////////
	// zhaohang  2010/5/14 
	// 刷新按钮屏蔽
	wnd = pageWnd->getChildRecursive("Auction/Tab/BuySale/RefreshSale");
	wnd->setVisible(false);
	wnd = pageWnd->getChildRecursive("Auction/Tab/BuySale/RefreshBuy");
	wnd->setVisible(false);
	wnd = pageWnd->getChildRecursive("Auction/Tab/Query/Refresh");
	wnd->setVisible(false);
	/////////////////////////////////////////////////
}

void SubscriberAHEvent(CEGUI::Window* pageWnd)
{
	pageWnd->subscribeEvent(EVENT_OPEN,CEGUI::Event::Subscriber(OnOpenedUpdate));//绑定打开界面刷新数据事件

	pageWnd->subscribeEvent("UpdatePickUpUI",CEGUI::Event::Subscriber(UpdatePickUPUIDate));//更新提取界面UI

	CEGUI::PushButton* btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/BuySale/BtnBuyWeimian"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnBuyWeimianBtnClicked));//点击按钮购买位面

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/BuySale/BtnSalWeimian"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnSaleWeimianBtnClicked));//点击按钮出售位面

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Pickup/BtnGold"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnPickUpGold));//提取金币

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Pickup/BtnWeimian"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnPickUpWeimian));//提取位面

	CEGUI::MultiColumnList* mcl = WMCL(pageWnd->getChildRecursive("Auction/Tab/BuySale/SaleMCL"));
	mcl->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged,CEGUI::Event::Subscriber(OnSaleMCLSelChanged));//位面可购买条目选中事件
	mcl->subscribeEvent("UpdateCanBuyMCL",CEGUI::Event::Subscriber(UpdateCanBuyMCLDate));//更新可购买出售列表
	mcl->subscribeEvent(CEGUI::Window::EventMouseDoubleClick,CEGUI::Event::Subscriber(OnBuyWeimianBtnClicked));//双击可购买Item，事件响应

	mcl = WMCL(pageWnd->getChildRecursive("Auction/Tab/BuySale/BuyMCL"));
	mcl->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged,CEGUI::Event::Subscriber(OnBuyMCLSelChanged));//位面可出售条目选中事件
	mcl->subscribeEvent("UpdateBuyMCL",CEGUI::Event::Subscriber(UpdateCanSaleMCLDate));//更新可出售列表
	mcl->subscribeEvent(CEGUI::Window::EventMouseDoubleClick,CEGUI::Event::Subscriber(OnSaleWeimianBtnClicked));//双击可出售Item，事件响应

	mcl = WMCL(pageWnd->getChildRecursive("Auction/Tab/Query/MCL"));
	mcl->subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged,CEGUI::Event::Subscriber(OnPerMCLSelChanged));//个人挂单条目选中事件
	mcl->subscribeEvent("UpdatePerMCL",CEGUI::Event::Subscriber(UpdatePerMCLDate));//更新个人挂单列表

	CEGUI::Window* wnd = pageWnd->getChildRecursive("Auction/SaleWnd");
	wnd->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,CEGUI::Event::Subscriber(OnSaleUIClosed));//出售界面关闭

	btn = WPushButton(wnd->getChildRecursive("Auction/SaleWnd/Submit"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnSaleSubmit));//提交出售请求

	wnd = pageWnd->getChildRecursive("Auction/BuyWnd");
	wnd->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,CEGUI::Event::Subscriber(OnBuyUIClosed));//购买界面关闭

	btn = WPushButton(wnd->getChildRecursive("Auction/Buy/Submit"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnBuySubmit));//提交购买请求

	CEGUI::Editbox* edb = WEditBox(pageWnd->getChildRecursive("Auction/Buy/buyNum"));//
	edb->subscribeEvent(CEGUI::Window::EventTextChanged,CEGUI::Event::Subscriber(UpdateBuyUIDate));//购买界面UI更新

	edb = WEditBox(pageWnd->getChildRecursive("Auction/SaleWnd/saleNum"));
	edb->subscribeEvent(CEGUI::Window::EventTextChanged,CEGUI::Event::Subscriber(UpdateSaleUIDate));//出售界面UI更新

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/Agent/sale/Submit"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnAgentSaleSubmit));//委托出售挂单提交

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/Submit"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnAgentBuySubmit));//委托求购挂单提交

	edb = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/sale/EditNum"));
	edb->subscribeEvent(CEGUI::Window::EventTextChanged,CEGUI::Event::Subscriber(UpdateAgentSaleUIDate));//委托出售挂单UI更新
	edb = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/sale/EditPrice"));
	edb->subscribeEvent(CEGUI::Window::EventTextChanged,CEGUI::Event::Subscriber(UpdateAgentSaleUIDate));//委托出售挂单UI更新


	edb = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/EditNum"));
	edb->subscribeEvent(CEGUI::Window::EventTextChanged,CEGUI::Event::Subscriber(UpdateAgentBuyUIDate));//委托求购UI更新
	edb = WEditBox(pageWnd->getChildRecursive("Auction/Tab/Agent/Buy/EditPrice"));
	edb->subscribeEvent(CEGUI::Window::EventTextChanged,CEGUI::Event::Subscriber(UpdateAgentBuyUIDate));//委托求购UI更新

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/Query/Cancel"));//个人委托撤销挂单
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnCancelAgentOrder));

	//
	btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/Query/Refresh"));//发送请求更新个人挂单列表
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnRefreshPerMCL));

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/BuySale/RefreshSale"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnRefreshSaleMCL));//发送请求更新出售委托挂单列表

	btn = WPushButton(pageWnd->getChildRecursive("Auction/Tab/BuySale/RefreshBuy"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnRefreshBuyMCL));//发送请求更新求购委托挂单列表


}

bool OnBuyWeimianBtnClicked(const CEGUI::EventArgs& e)
{
	//点击购买按钮，打开购买界面
	OpenBuyUI();
	FireUIEvent("Auction/SaleWnd","CloseClicked");//关闭出售界面

	return true;
}

bool OnSaleWeimianBtnClicked(const CEGUI::EventArgs& e)
{
	//点击出售按钮，打开出售界面
	OpenSaleUI();
	FireUIEvent("Auction/BuyWnd","CloseClicked");//关闭购买界面
	return true;
}

bool OnPickUpGold(const CEGUI::EventArgs& e)
{
	AHdata& ah = GetInst(AHdata);
	ah.Send_AH_PICKUP(AH::PT_COIN);
	return true;
}

bool OnPickUpWeimian(const CEGUI::EventArgs& e)
{
	AHdata& ah = GetInst(AHdata);
	ah.Send_AH_PICKUP(AH::PT_GOODS);
	return true;
}

bool OnBuyMCLSelChanged(const CEGUI::EventArgs& e)
{
	return false;
}

bool OnSaleMCLSelChanged(const CEGUI::EventArgs& e)
{

	return false;
}

bool OnPerMCLSelChanged(const CEGUI::EventArgs& e)
{
	return false;
}

bool OnSaleUIClosed(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	wnd->setVisible(false);
	return true;
}

bool OpenSaleUI()
{
	CEGUI::WindowManager& wndmgr = GetWndMgr();
	//获取出售订单ID
	CEGUI::MultiColumnList* mcl = WMCL(wndmgr.getWindow("Auction/Tab/BuySale/BuyMCL"));
	if(!mcl)
		return false;
	CEGUI::ListboxItem* lbi = mcl->getFirstSelectedItem();
	if(!lbi)
	{
		//MessageBox(g_hWnd,AppFrame::GetText("AU_100"),"ERROR",MB_OK);
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_100"),NULL,NULL,true);
		return false;
	}

	CEGUI::Window* wnd = wndmgr.getWindow("Auction/SaleWnd");
	wnd->setVisible(true);
	wnd->setAlwaysOnTop(true);
	CEGUI::Editbox* editbox = WEditBox(wnd->getChildRecursive("Auction/SaleWnd/saleNum"));//出售界面编辑框激活
	editbox->activate();

	AHdata& ah = GetInst(AHdata);
	uint ID = lbi->getID();
	ah.SetCanSaleID(ID);
	return true;
}

//提交出售请求
bool OnSaleSubmit(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& wndmgr = GetWndMgr();
	CEGUI::Editbox* edb = WEditBox(wndmgr.getWindow("Auction/SaleWnd/saleNum"));
	AHdata& ah = GetInst(AHdata);
	//获取出售数量
	uint saleNum = CEGUI::PropertyHelper::stringToInt(edb->getText());
	uint cansaleNum = ah.GetCountByCanSaleID(ah.GetCanSaleID());
	if(saleNum == 0 || saleNum > cansaleNum )
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_101"),NULL,NULL,true);
		return false;
	}
	ah.Send_AH_SEL(cansaleNum);//发送出售请求前要设置出售的订单ID
	return true;
}

bool OnBuyUIClosed(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = WEArgs(e).window;
	wnd->setVisible(false);
	return true;
}

bool OpenBuyUI()
{
	CEGUI::WindowManager& wndmgr = GetWndMgr();
	CEGUI::MultiColumnList* mcl = WMCL(wndmgr.getWindow("Auction/Tab/BuySale/SaleMCL"));
	if(!mcl)
		return false;
	CEGUI::ListboxItem* lbi = mcl->getFirstSelectedItem();
	if(!lbi)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_102"),NULL,NULL,true);
		return false;
	}
	CEGUI::Window* wnd = wndmgr.getWindow("Auction/BuyWnd");
	wnd->setVisible(true);
	wnd->setAlwaysOnTop(true);
	CEGUI::Editbox* editbox = WEditBox(wnd->getChildRecursive("Auction/Buy/buyNum"));//购买界面编辑框激活
	editbox->activate();
	AHdata& ah = GetInst(AHdata);
	//界面获取购买订单ID
	uint ID = lbi->getID();
	ah.SetCanBuyID(ID);//保存要购买的订单ID
	return true;
}

////提交购买请求
bool OnBuySubmit(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& wndmgr = GetWndMgr();
	CEGUI::Editbox* edb = WEditBox(wndmgr.getWindow("Auction/Buy/buyNum"));
	AHdata& ah = GetInst(AHdata);
	//获取购买数量
	uint buyNum = CEGUI::PropertyHelper::stringToInt(edb->getText());
	uint canbuyNum = ah.GetCountByCanBuyID(ah.GetCanBuyID());
	if(buyNum== 0 || buyNum > canbuyNum)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_103"),NULL,NULL,true);
		return false;
	}
	ah.Send_AH_BUY(buyNum);//发送购买请求前要设置购买的订单ID
	return true;
}

//更新位面购买界面UI
bool UpdateBuyUIDate( const CEGUI::EventArgs& e )
{
	CEGUI::WindowManager& mgr = GetWndMgr();
	CEGUI::Window* weimian = mgr.getWindow("Auction/Buy/Text6");
	weimian->setText("");
	CEGUI::Window* gold = mgr.getWindow("Auction/Buy/Text61");
	gold->setText("");
	//获取输入的购买数目
	CEGUI::Editbox* edb = WEditBox(mgr.getWindow("Auction/Buy/buyNum"));
	if(!edb)
		return false;
	uint buyNum = CEGUI::PropertyHelper::stringToInt(edb->getText());
	if(buyNum == 0)
		return false;
	//获得位面提示
	weimian->setText(edb->getText());

	AHdata& ah = GetInst(AHdata);
	uint orderID = ah.GetCanBuyID();//购买的订单ID
	uint price = ah.GetPriceByCanBuyID(orderID);
	//计算价格
	uint subCust = buyNum * price ;
	//设置需要支付金币
	gold->setText(CEGUI::PropertyHelper::intToString(subCust));
	return true;
}

bool UpdateSaleUIDate( const CEGUI::EventArgs& e )
{	
	CEGUI::WindowManager& mgr = GetWndMgr();
	CEGUI::Window* weimian = mgr.getWindow("Auction/SaleWnd/Text61");
	weimian->setText("");
	CEGUI::Window* gold = mgr.getWindow("Auction/SaleWnd/Text6");
	gold->setText("");
	//获取输入的出售数目
	CEGUI::Editbox* edb = WEditBox(mgr.getWindow("Auction/SaleWnd/saleNum"));
	if(!edb)
		return false;
	uint saleNum = CEGUI::PropertyHelper::stringToInt(edb->getText());
	if(saleNum==0)
		return false;
	//设置支付位面数
	weimian->setText(edb->getText());

	AHdata& ah = GetInst(AHdata);
	uint orderID = ah.GetCanSaleID();//获取要出售的订单ID
	uint price = ah.GetPriceByCanSaleID(orderID);//单价
	//计算价格
	uint subCust = saleNum * price;
	//设置需要支付金币
	gold->setText(CEGUI::PropertyHelper::intToString(subCust));
	return true;
}

bool UpdateCanBuyMCLDate(const CEGUI::EventArgs& e)
{
	AHdata& ah = GetInst(AHdata);
	AHdata::vecOrderRaw listOR = ah.m_CanBuyOrder;//可购买列表
	CEGUI::MultiColumnList* mcl = WMCL(WEArgs(e).window);
	if(!mcl)
		return false;
	mcl->resetList();
	AHdata::vecOrderRaw::iterator it = listOR.begin();
	for (uint i = 0 ; i < listOR.size() ; ++i,++it)
	{
		mcl->addRow();
		CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(ToCEGUIString(AppFrame::GetText("AU_105")),it->id);//第二个参数与订单ID关联
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,0,i);

		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(it->price));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,1,i);

		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(it->cnt));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,2,i);
	}

	return true;
}

bool UpdateCanSaleMCLDate(const CEGUI::EventArgs& e)
{
	AHdata& ah = GetInst(AHdata);
	AHdata::vecOrderRaw listOR = ah.m_CanSaleOrder;//可出售列表
	CEGUI::MultiColumnList* mcl = WMCL(WEArgs(e).window);
	if(!mcl)
		return false;
	mcl->resetList();
	AHdata::vecOrderRaw::iterator it = listOR.begin();
	for (uint i = 0 ; i < listOR.size() ; ++i,++it)
	{
		mcl->addRow();
		CEGUI::ListboxTextItem* lti = new CEGUI::ListboxTextItem(ToCEGUIString(AppFrame::GetText("AU_104")),it->id);//第二个参数与订单ID关联
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,0,i);

		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(it->price));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,1,i);

		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(it->cnt));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,2,i);
	}
	return true;
}

bool UpdatePerMCLDate(const CEGUI::EventArgs& e)
{
	AHdata& ah = GetInst(AHdata);
	//AHdata::listSubOrderRaw listSubOR = ah.m_AgentOrder;
	AHdata::MapSubOrderRaw mapSubOR = ah.m_AgentOrder;
	CEGUI::MultiColumnList* mcl = WMCL(WEArgs(e).window);
	if(!mcl)
		return false;
	mcl->resetList();

	AHdata::MapSubOrderRaw::iterator it = mapSubOR.begin();
	for (int i = 0 ; it != mapSubOR.end(); ++it,++i)
	{
		mcl->addRow();
		CEGUI::ListboxTextItem* lti = NULL;
		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(it->first),it->first);//控件ID与订单ID关联
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,0,i);
		
		AH::SubOrderRaw& mapOr = it->second;
		if(mapOr.type == AH::OT_BUY)
			lti = new CEGUI::ListboxTextItem(ToCEGUIString(AppFrame::GetText("AU_104")));
		else if(mapOr.type == AH::OT_SELL)
			lti = new CEGUI::ListboxTextItem(ToCEGUIString(AppFrame::GetText("AU_105")));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,1,i);

		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(mapOr.cnt));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,2,i);

		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(mapOr.price));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,3,i);

		/////////////////////////////////////////////////
		// zhaohang  2010/5/12 
		// 时间UI未处理
		/////////////////////////////////////////////////
		uint subtime = mapOr.gentime / 60 / 60;
		lti = new CEGUI::ListboxTextItem(CEGUI::PropertyHelper::intToString(subtime));
		lti->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
		mcl->setItem(lti,4,i);
	}
	return false;
}

bool UpdatePickUPUIDate(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pageWnd = WEArgs(e).window;
	CEGUI::Editbox* edb = WEditBox(pageWnd->getChildRecursive("Auction/Pickup/EditGold"));
	AHdata& ah = GetInst(AHdata);
	edb->setText(CEGUI::PropertyHelper::intToString(ah.GetNumGoldCanPickUp()));

	edb = WEditBox(pageWnd->getChildRecursive("Auction/Pickup/EditWeimian"));
	edb->setText(CEGUI::PropertyHelper::intToString(ah.GetNumWeimianCanPickUp()));
	return true;
}

bool OnOpenedUpdate(const CEGUI::EventArgs& e)
{
	GetInst(AHdata).Send_AH_REQUEST_ALL();//发送一次列表请求
	return true;
}

bool OnAgentBuySubmit(const CEGUI::EventArgs& e)
{
	//得到委托购买数目和单价
	AH::OrderRaw or;
	CEGUI::Editbox* edb = WEditBox(GetWndMgr().getWindow("Auction/Tab/Agent/Buy/EditNum"));
	if(!edb)
		return false;
	or.id = -1 ;// -1标识ID不会用到
	//数目
	or.cnt = CEGUI::PropertyHelper::stringToInt(edb->getText());//单位为：百个
	if(or.cnt == 0)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_106"),NULL,NULL,true);
		return false;
	}
	edb = WEditBox(GetWndMgr().getWindow("Auction/Tab/Agent/Buy/EditPrice"));
	if(!edb)
		return false;
	//单价
	or.price = CEGUI::PropertyHelper::stringToInt(edb->getText());// 金币/100位面
	if(or.price == 0)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_107"),NULL,NULL,true);
	}
	//检查玩家身上金币是否满足购买条件
	CPlayer* player = GetGame()->GetMainPlayer();
	long money = player->GetMoney();
	float custm = or.price * or.cnt * AGENT_EXTRACUST ;//算上手续费
	if( custm > money)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_108"),NULL,NULL,true);
		return false;
	}

	//提交委托购买订单
	AHdata& ah = GetInst(AHdata);
	ah.Send_AH_ADD_AGENT(or,AH::OT_BUY);
	return true;
}

//更新委托求购界面提示
bool UpdateAgentBuyUIDate(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& mgr = GetWndMgr();
	CEGUI::Window* wnd = NULL;
	//求购数量
	CEGUI::Editbox* edb = WEditBox(mgr.getWindow("Auction/Tab/Agent/Buy/EditNum"));
	uint cnt = CEGUI::PropertyHelper::stringToInt(edb->getText());
	//求购单价
	edb = WEditBox(mgr.getWindow("Auction/Tab/Agent/Buy/EditPrice"));
	uint price = CEGUI::PropertyHelper::stringToInt(edb->getText());
	if(cnt == 0 || price == 0)//如何一个为0,直接返回
	{
		wnd = mgr.getWindow("Auction/Tab/Agent/Buy/subGold");
		wnd->setText("");//手续费
		wnd = mgr.getWindow("Auction/Tab/Agent/Buy/Text");//总共支付
		wnd->setText("");
		return false;
	}
	//设置委托购买位面总价提示
	 wnd = mgr.getWindow("Auction/Tab/Agent/Buy/subNum");
	uint sumPrice = price * cnt;
	wnd->setText(CEGUI::PropertyHelper::intToString(sumPrice));

	float extreCust = price * cnt * AGENT_EXTRACUST;//手续费
	wnd = mgr.getWindow("Auction/Tab/Agent/Buy/subGold");
	wnd->setText(CEGUI::PropertyHelper::intToString((int)extreCust));

	wnd = mgr.getWindow("Auction/Tab/Agent/Buy/Text");//总共支付
	wnd->setText(CEGUI::PropertyHelper::intToString((sumPrice + (int)extreCust)));
	return true;
}

bool OnAgentSaleSubmit(const CEGUI::EventArgs& e)
{
	//得到委托出售数目和单价
	AH::OrderRaw or;
	CEGUI::Editbox* edb = WEditBox(GetWndMgr().getWindow("Auction/Tab/Agent/sale/EditNum"));
	if(!edb)
		return false;
	or.id = -1;//-1标识 id不会用到
	//单数
	or.cnt = CEGUI::PropertyHelper::stringToInt(edb->getText());
	if(or.cnt == 0)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_109"),NULL,NULL,true);
		return false;
	}
	edb = WEditBox(GetWndMgr().getWindow("Auction/Tab/Agent/sale/EditPrice"));
	if(!edb)
		return false;
	//单价
	or.price = CEGUI::PropertyHelper::stringToInt(edb->getText());
	if(or.price == 0)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_110"),NULL,NULL,true);
		return false;
	}
	//检查玩家身上位面是否满足数目
	CPlayer* player = GetGame()->GetMainPlayer();
	long cnt = player->GetGoodsAmount(CGlobeSetup::GetSetup()->strWeiMianName);
	if(cnt < or.cnt * 100 ) //玩家身上数量小于出售数量
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_111"),NULL,NULL,true);
		return false;
	}
	//检查玩家身上的金币是否满足委托收取的手续费
	long money = player->GetMoney();
	float custm = or.cnt * or.price * AGENT_EXTRACUST;//算上手续费
	if ( custm > money  )
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_112"),NULL,NULL,true);
		return false;
	}
	//提交委托出售订单
	AHdata& ah = GetInst(AHdata);
	ah.Send_AH_ADD_AGENT(or,AH::OT_SELL);
	return true;
}

//更新寄售UI提示
bool UpdateAgentSaleUIDate(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& mgr = GetWndMgr();
	CEGUI::Window* wnd = NULL;
	CEGUI::Editbox* edb = WEditBox(mgr.getWindow("Auction/Tab/Agent/sale/EditNum"));//寄售数量
	uint cnt = CEGUI::PropertyHelper::stringToInt(edb->getText());

	edb = WEditBox(mgr.getWindow("Auction/Tab/Agent/sale/EditPrice"));//寄售单价
	uint price = CEGUI::PropertyHelper::stringToInt(edb->getText());

	if(cnt == 0 || price == 0)//任意一个为0，则直接返回，不更新UI
	{
		//设置支付手续费提示
		wnd = mgr.getWindow("Auction/Tab/Agent/sale/subGold");
		wnd->setText("");
		return false;
	}
	//设置支付位面提示
	wnd = mgr.getWindow("Auction/Tab/Agent/sale/subNum");
	wnd->setText(CEGUI::PropertyHelper::intToString(cnt * ORDER_PER_NUM));
	//设置支付手续费提示
	wnd = mgr.getWindow("Auction/Tab/Agent/sale/subGold");
	float extraCust = cnt * price * AGENT_EXTRACUST ;
	wnd->setText(CEGUI::PropertyHelper::intToString((int)extraCust));
	return true;
}


//撤销委托订单事件
bool OnCancelAgentOrder(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& mgr = GetWndMgr();
	CEGUI::MultiColumnList* mcl = WMCL(mgr.getWindow("Auction/Tab/Query/MCL"));
	//获得选中Item对应的订单ID
	CEGUI::ListboxItem* li = mcl->getFirstSelectedItem();
	if(!li)
	{
		/***********************************************************************/
		/* zhaohang fix 2010-9-3
		/* 修改消息框形式，采用封装的MsgEventManager,以及新的字符资源加载方式Appframe
		/***********************************************************************/
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("AU_113"),NULL,NULL,true);
		return false;
	}
	uint orderID = li->getID();
	AHdata& ah = GetInst(AHdata);
	ah.Send_AH_REMOVE_AGENT(orderID);
	return true;
}

bool OnRefreshSaleMCL(const CEGUI::EventArgs& e)
{
	//发送刷新列表信息
	AHdata& ad = GetInst(AHdata);
	ad.Send_AH_REQUEST_BUY();
	return true;
}

bool OnRefreshBuyMCL(const CEGUI::EventArgs& e)
{
	//发送刷新列表信息
	AHdata& ad = GetInst(AHdata);
	ad.Send_AH_REQUEST_SELL();
	return true;
}

bool OnRefreshPerMCL(const CEGUI::EventArgs& e)
{
	//发送刷新列表信息
	AHdata& ad = GetInst(AHdata);
	ad.Send_AH_REQUEST_AGENT();
	return true;
}