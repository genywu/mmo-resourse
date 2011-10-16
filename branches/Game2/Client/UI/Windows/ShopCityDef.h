/**
* @filename:ShopCityDef.h
* @date: 2010/7/19
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 商城的宏定义，原来星尘客户端的道具商店
*/

/////////////////////////////////////////////////
// zhaohang  2010/7/23 
// ShopCity部分
////////////////////////////////////////////////
#define  SHOPCITY_PAGE_NAME						"ShopCity"
#define  SHOPCITY_TAB_NAME						"ShopCity/Tabs"
#define  SHOPCITY_TAB_CONTENT_NAME_D			"ShopCity/Tabs/Tab %d"
#define  SHOPCITY_SEARCH_NAME					"ShopCity/Search"
#define  SHOPCITY_SEARCH_LEFTWND_NAME			"ShopCity/GuideBuy"
#define  SHOPCITY_SEARCH_RIGHTWND_NAME			"ShopCity/Filter"
#define  SHOPCITY_LATESTBUY_NAME				"ShopCity/LatestBuy"
#define  SHOPCITY_LATESTBUY_ITME_NAME_D			"ShopCity/LatestBuy/IG%d"
#define  SHOPCITY_TWITTER_NAME					"ShopCity/Recommend"
#define  SHOPCITY_TWITTER_CHILDLISTBOX_NAME		"ShopCity/Recommend/ContentListBox"
#define  SHOPCITY_SAVEMONEY_NAME				"ShopCity/SaveMoneyBtn"
#define  SHOPCITY_XINGZUAN_EDBOX_NAME			"ShopCity/xingzuanEditBox"
#define  SHOPCITY_DIANJUAN_EDBOX_NAME			"ShopCity/DianjuanEdbox"
#define  SHOPCITY_WEIMIAN_EDBOX_NAME			"ShopCity/WeimianEdbox"
#define  SHOPCITY_FILLTERROOM_NAME				"ShopCity/FittingRoom/Del"


#define  SHOPCITY_PAGE_EVENT_UPDATA				"ShopCityUpdata"
#define  SHOPCITY_PAGE_EVENT_LOGRESPONSE		"ShopCityLogResponse"
#define  SHOPCITY_PAGE_EVENT_BUYRESULT			"ShopCityBuyResult"
#define  SHOPCITY_PAGE_EVENT_AFFICHERESPONSE	"ShopCityAfficheResponse"
#define  SHOPCITY_PAGE_EVENT_CLEARMONEYEDBOXES  "ShopCityClearMoneyEdboxes"
#define  SHOPCITY_PAGE_EVENT_UPDATEMONEYEDBOXES "ShopCityUpdateMoneyEdboxes"
#define  SHOPCITY_PAGE_EVENT_UPDATE_LATESTBUY   "ShopCityOnUpdateLatestBuy"
#define  SHOPCITY_TWITTER_EVENT_NAME			"ShopCityOnUpdateTwitter"
#define  SHOPCITY_SEARCHLEFT_EVENT_MENUUPDATE_NAME  "ShopCitySearchLeftMenuUpdate"
#define  SHOPCITY_SEARCHRIGHT_EVENT_MENUUPDATE_NAME "ShopCitySearchRightMenuUpdate"

/////////////////////////////////////////////////
// zhaohang  2010/7/23 
// ShopCityChild部分
////////////////////////////////////////////////
#define  SHOPCITY_CHILD_PAGE_NAME					"ShopCityChild"
//#define  SHOPCITY_CHILD_PAGE_TABPAN_BUTTONS_NAME "ShopCityChild__auto_TabPane__Buttons"

/////////////////////////////////////////////////
// zhaohang  2010/7/23 
// ShopCityItemSet部分
////////////////////////////////////////////////
#define  SHOPCITY_ITEMSET_PAGE_NAME					"ShopCityItemSet"

#define  SHOPCITY_PREPAGE_NAME						"ShopCityItemSet/PrePage"
#define  SHOPCITY_NEXTPAGE_NAME						"ShopCityItemSet/NextPage"
#define  SHOPCITY_ITEMSET_ITEM_COUNT				12
#define  SHOPCITY_ITEMSET_ITEM_BGIMG_D				"ShopCityItemSet/Item%d/Icon"
#define  SHOPCITY_ITEMSET_ITEM_HOTIMG_D				"ShopCityItemSet/Item%d/Icon/Hot"
#define  SHOPCITY_ITEMSET_ITEM_TITLE_D				"ShopCityItemSet/Item%d"
#define  SHOPCITY_ITEMSET_ITEM_DISCONT_D			"ShopCityItemSet/Item%d/DiscontIcon"
#define  SHOPCITY_ITEMSET_ITEM_MONEYTYPE_D			"ShopCityItemSet/Item%d/MoneyType"
#define  SHOPCITY_ITEMSET_ITEM_PRICEEDBOX_D			"ShopCityItemSet/Item%d/PriceEditBox"
#define  SHOPCITY_ITEMSET_ITEM_LABLEEDBOX_D			"ShopCityItemSet/Item%d/LableEdbox"
#define  SHOPCITY_ITEMSET_ITEM_BUYBTN_D				"ShopCityItemSet/Item%d/Buy"
#define  SHOPCITY_ITEMSET_ITEM_TRYONBTN_D			"ShopCityItemSet/Item0/TryOn"

#define  SHOPCITY_ITEMSET_EVENT_CLEAR				"OnShopCityItemSetClear"
#define  SHOPCITY_ITEMSET_EVENT_UPDATE				"OnShopCityItemSetUpdate"
//#define  SHOPCITY_ITEMSET_EVENT_UPDATE_BY_HOT		"OnShopCityItemSetUpdateByHot"
#define  SHOPCITY_ITEMSET_EVENT_UPDATE_BY_FILTER	"OnShopCityITemSetUpdateByFilter"
#define  SHOPCITY_ITEMSET_EVENT_UPDATE_BY_GUIDE		"OnShopCityItemSetUpdateByGuide"
/////////////////////////////////////////////////
// zhaohang  2010/7/27 
// 购买页面部分
////////////////////////////////////////////////
#define  SHOPCITY_BUY_PAGE_NAME					"ShopCityBuyPage"
#define  SHOPCITY_BUY_ICON						"ShopCityBuyPage/Icon"
#define  SHOPCITY_BUY_NAME						"ShopCityBuyPage/Name"
#define  SHOPCITY_BUY_PRICE						"ShopCityBuyPage/Price"
#define  SHOPCITY_BUY_PRICE_ICON				"ShopCityBuyPage/MoneyIcon"
#define  SHOPCITY_BUY_NOWPRICE					"ShopCityBuyPage/NowPrice"
#define  SHOPCITY_BUY_NOWPRICE_ICON				"ShopCityBuyPage/MoneyIcon1"
#define  SHOPCITY_BUY_LABLE						"ShopCityBuyPage/lable"
#define  SHOPCITY_BUY_LIMITSALE					"ShopCityBuyPage/limitSale"
#define  SHOPCITY_BUY_LIMITBUY					"ShopCityBuyPage/limitBuy"
#define  SHOPCITY_BUY_LIMITTIME					"ShopCityBuyPage/limitTime"
#define  SHOPCITY_BUY_DESC						"ShopCityBuyPage/desc"
#define  SHOPCITY_BUY_NUMEDBOX					"ShopCityBuyPage/BuyNum"
#define  SHOPCITY_BUY_ADD_BTN					"ShopCityBuyPage/BuyBtn+"
#define  SHOPCITY_BUY_MINUS_BTN					"ShopCityBuyPage/BuyBtn-"
#define  SHOPCITY_BUY_BUY_BTN					"ShopCityBuyPage/BuyBtn"

//#define  SHOPCITY_BUY_EVENT_UPDATE				"OnShopCityBuyUdate"
#define  SHOPCITY_BUY_EVENT_CLEAR				"OnShopCityBuyClear"
/////////////////////////////////////////////////
// zhaohang  2010/7/26 
// 物品背景图标的宏定义
#define GOODS_ICON_PATH			"pictures/goods"
#define GOODS_GROUP				"GoodsIcon"
#define GOODS_PREFIXID			"GoodsID%d"
////////////////////////////////////////////////