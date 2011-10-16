#include "stdafx.h"
#include "MainBarPage.h"
#include "MainBarEvent.h"
#include "../Chat/ChatPage.h"
#include "../Chat/ChatEvent.h"
#include "../GoodsViewer/GoodsViewerPage.h"
#include "../Package/PackagePage.h"
#include "../RankPage/RankPage.h"
#include "../../UIDef.h"
#include "../../Windows/QuestDef.h"
#include "../../Windows/SkillDef.h"
#include "../../Windows/TeamDef.h"
#include "../../Windows/ShopCityDef.h"
#include "../../Windows/GeniusDef.h"
#include "../../../GameClient/Game.h"
#include "../../../GameClient/ClientApp/ClientRegion.h"
#include "../../../GameClient/ClientApp/Auction/AuctionSys.h"
#include "../../../GameClient/ClientApp/organizingsystem/FactionPageOper.h"

#include "../../../GameClient/ClientApp/ShopCity/ShopCitySys.h"

MainBarPage::MainBarPage()
{
    LoadPage();
}

void MainBarPage::LoadPage()
{
    SetLayOutFileName("MainBarPage.layout");
    MainBarEvent event(ON_LOAD);
    ProcessPageEvent(&event);
}

void MainBarPage::ReleasePage()
{

}

bool MainBarPage::Open()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if (pPageWindow)
    {
        pPageWindow->setVisible(true);
        MainBarEvent event(ON_OPEN);
        ProcessPageEvent(&event);
    }
    else
    {
        assert("PackagePage Open failed!!");
        return false;
    }
    return true;
}

bool MainBarPage::Close()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if(pPageWindow)
        pPageWindow->setVisible(false);
    else
        return false;
    return true;
}

void MainBarPage::Run()
{
    CEGUI::Window* wnd = NULL;
    extern BYTE g_cPressedKey;
    switch (g_cPressedKey)
    {
    case 0:
        return;
    case 'A':
        {
            FireUIEvent("PersonalMail", EVENT_SWITCH);
        }
        break;
    case 'B':
        GetInst(PackagePage).SwitchPage();
        break;
    case 'C':
        {
            FireUIEvent("Attribute",EVENT_SWITCH);
        }
        break;
	case 'D':
		{
			FireUIEvent(GENIUS_PAGE_NAME,EVENT_SWITCH);
		}
		break;
    case 'G':
        {
            GetInst(FactionOper).Switch();
        }
        break;
    case 'H':
        {
            GetInst(FactionOper).OpenCountryFaction();
        }
        break;
    case 'L':
        FireUIEvent("fwLinkman", EVENT_SWITCH);
        break;
    case 'M':
        {
            FireUIEvent("WorldMap","Switch");
        }
        break;
    case 'P':
        {
            FireUIEvent("Servant", EVENT_SWITCH);
        }
        break;
    case 'Q':
        {
            FireUIEvent(QUEST_PAGE_NAME, EVENT_SWITCH);
        }
        break;
    case 'R':         // 暂时作为排行榜的开关
        GetInst(RankPage).SwitchPage();
        break;
    case 'S':
        {
            FireUIEvent(SKILL_PAGE_NAME,EVENT_SWITCH);//技能模板界面
        }
        break;
    case 'T':
        {
            FireUIEvent(TEAM_PAGE_NAME,EVENT_SWITCH);
        }
        break;
    case 'V':
#ifdef _DEBUG
        GetInst(GoodsViewerPage).SwitchPage();
#endif
        break;
    case 'W':
        {
            FireUIEvent("Auction" ,EVENT_SWITCH);//位面寄售,绑定了一个Open时更新数据的事件
        }
        break;
    case 'Z':
        {
            GetInst(ShopCityMsgMgr).SwitchPage();
            //FireUIEvent(SHOPCITY_PAGE_NAME,EVENT_SWITCH);//商城
        }
        break;
    case VK_RETURN:                               // 打开聊天输入窗口
         //GetInst(ChatPage).SwitchPage();
         FireUIEvent("ChatPrompt", EVENT_SWITCH);
        break;
    case VK_ESCAPE:                      // 关闭当前界面或打开退出界面
        {
            CEGUI::Window* wnd = GetFocusWindow();
            if(wnd)
            {
                FireUIEvent( wnd->getName().c_str() , EVENT_CLOSE );
            }
            else
                FireUIEvent("GameExit", EVENT_SWITCH);

            break;
        }

    default:
        {
#ifdef _DEBUG
            Log4c::Warn(ROOT_MODULE, "[%c]键尚未设置为快捷键！", g_cPressedKey);
#endif
        }
        break;
    }
}

void MainBarPage::Display()
{
    GetGame()->GetRegion()->DisplayMiniMap(800,0);
}