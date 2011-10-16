#include "stdafx.h"
#include "../../GamePage.h"
#include "../../UIDef.h"
#include "MainBarEvent.h"
#include "../ChatList/ChatListPage.h"

extern void SubscriberMainBarEvent(CEGUI::Window* pageWnd);
extern void SetInitializedMainBarUIProperty(CEGUI::Window* pageWnd);

MainBarEvent::MainBarEvent(EVENTTYPE type)
{
    SetEventType(type);
}

void MainBarEvent::OnPageLoad(GamePage *pPage)
{
    pPage->LoadPageWindow();
    //…Ë÷√ΩÁ√Ê±≥æ∞
    CEGUI::GUISheet *pageWin = static_cast<CEGUI::GUISheet*>(pPage->GetPageWindow());
    const CEGUI::Image & backgroundImage = SetBackGroundImage(pageWin,"MainBar","datafiles/imagesets/MainBar","interface.tga");
    float width  = backgroundImage.getSourceTextureArea().getWidth();
    float height = backgroundImage.getSourceTextureArea().getHeight();
    extern int SCREEN_WIDTH,SCREEN_HEIGHT;
    pageWin->setPosition(CEGUI::UVector2(cegui_absdim((SCREEN_WIDTH - width) / 2), CEGUI::UDim(1,-height)));
	SubscriberMainBarEvent(pageWin);
	SetInitializedMainBarUIProperty(pageWin);

    FireUIEvent("MiniMapFrame", EVENT_SWITCH);
}

void MainBarEvent::OnPageOpen(GamePage *pPage)
{
    GetInst(ChatListPage).Open();
    FireUIEvent("MainRole", EVENT_OPEN);
    FireUIEvent("MainRole", "RoleAttr");
}

void MainBarEvent::OnPageClose(GamePage *pPage)
{

}

void MainBarEvent::OnPageUpdate(GamePage *pPage)
{

}

void MainBarEvent::OnPageRelease(GamePage *pPage)
{

}

bool MainBarEvent::GamePageOp(const CEGUI::EventArgs &e)
{
    return true;
}

