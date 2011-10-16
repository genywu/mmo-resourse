#include "stdafx.h"
#include "../UIDef.h"
#include "../../GameClient/ClientApp/Goods/Goods.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/GameDesp/GameDespParser.h"
#include "../../ui/GamePage/Package/PackagePage.h"
#include "../../ui/GamePage/Package/PackageEvent.h"

bool updateDesp(const CEGUI::EventArgs &args);
bool AdjustDescriptionPos(const CEGUI::EventArgs &args);

CEGUI::Window* InitGoodsDesp()
{
    CEGUI::Window* wnd = LoadUI("GoodsDesp");
    wnd->setVisible(false);
    wnd->subscribeEvent("updateDesp",CEGUI::Event::Subscriber(updateDesp));
    wnd->subscribeEvent(EVENT_OPEN,CEGUI::Event::Subscriber(AdjustDescriptionPos));
    return wnd;
}

//调整界面位置
bool AdjustDescriptionPos(const CEGUI::EventArgs &args)
{
    CEGUI::Window *pDespPage = GetWndMgr().getWindow("GoodsDesp");
    
    return false;
}
//更新界面信息
bool updateDesp(const CEGUI::EventArgs &args)
{
    CEGUI::Window *pDespPage = GetWndMgr().getWindow("GoodsDesp");
    CEGUI::Window *pDespText = GetWndMgr().getWindow("GoodsDesp/stParseText");
    long lExtendID = 0;
    GameDespParser *pParser = GetGame()->GetGameDespParser();
    if(pParser->GetSourceUI() == GameDespParser::SI_PACKAGE)
    {
        lExtendID = PEI_PACKET;
    }
    long lpos = GetInst(PackagePage).GetPackageEvent()->GetGuidMouseHover();
    static CGoods* pLastGoods = 0;
    CGoods* pMouseGoods = GetGame()->GetMainPlayer()->GetGoodsByPosition(lExtendID,lpos);
    if(!pMouseGoods)
        return false;
    if (pLastGoods != pMouseGoods)
    {
        const char * strText = pParser->ParseGoods(pMouseGoods,true);
        SetText(pDespText,strText);
        pLastGoods = pMouseGoods;
    }
    return true;
}
