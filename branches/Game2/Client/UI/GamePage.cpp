#include "stdafx.h"
#include "GamePage.h"
#include "PageEvent.h"
#include "UIManager.h"
#include "UIDef.h"
GamePage::GamePage():m_PageWindow(NULL),m_layout(""),m_luaFileName("")
{

}

GamePage::~GamePage()
{
	m_PageWindow = NULL;
}

void GamePage::SetLayOutFileName(const char* filename)
{
    m_layout = filename;
}

void GamePage::SetLuaFileName(const char* filename)
{
    m_luaFileName = filename;
}

void GamePage::ProcessPageEvent(PageEvent *pEvent)
{
    EVENTTYPE type = pEvent->GetEventType();
    if (type == ON_LOAD)
        pEvent->OnPageLoad(this);
    else if(type == ON_OPEN)
        pEvent->OnPageOpen(this);
    else if(type == ON_CLOSE)
        pEvent->OnPageClose(this);
    else if(type == ON_UPDATE)
        pEvent->OnPageUpdate(this);
    else if(type == ON_RELEASE)
        pEvent->OnPageRelease(this);
}

void GamePage::LoadPageWindow()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::DefaultWindow* root = (CEGUI::DefaultWindow*)winMgr.getWindow("Root");
    m_PageWindow = winMgr.loadWindowLayout(m_layout);
    root->addChildWindow(m_PageWindow);
    m_PageWindow->subscribeEvent(EVENT_OPEN,
        CEGUI::Event::Subscriber(OnShown));
    m_PageWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
        CEGUI::Event::Subscriber(OnHide));
    m_PageWindow->subscribeEvent(EVENT_CLOSE,
        CEGUI::Event::Subscriber(OnHide));
    m_PageWindow->subscribeEvent(EVENT_SWITCH,
        CEGUI::Event::Subscriber(OnSwitch));
    m_PageWindow->setMaxSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
    m_PageWindow->setMinSize(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));
    m_PageWindow->setVisible(false);
    UIManager::AddGamePage(this);
}

CEGUI::Window*   GamePage::GetPageWindow()
{
    if(m_PageWindow == NULL)
    {
        LoadPage();
    }
    return m_PageWindow;
}

bool GamePage::isOpen()
{
    return GetPageWindow()->isVisible();
}

void GamePage::SwitchPage()
{
    if (isOpen())
    {
        Close();
    }
    else
        Open();
}