#include "stdafx.h"
#include "ChatListPage.h"
#include "ChatListEvent.h"

ChatListPage::ChatListPage():m_ChatListEvent(0),m_ChatLogicEvent(0)
{
    LoadPage();
}

void ChatListPage::LoadPage()
{
    if(NULL == m_ChatListEvent)
        m_ChatListEvent = new ChatListEvent;
    if(NULL == m_ChatLogicEvent)
        m_ChatLogicEvent = new ChatLogicEvent;
    
    SetLayOutFileName("ChatListPage.layout");
    m_ChatListEvent->SetEventType(ON_LOAD);
    ProcessPageEvent(m_ChatListEvent);
}

void ChatListPage::ReleasePage()
{
    SAFE_DELETE(m_ChatListEvent);
    SAFE_DELETE(m_ChatLogicEvent);
}

bool ChatListPage::Open()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if (pPageWindow == NULL)
    {
        LoadPage();
        pPageWindow = GetPageWindow();
    }
    pPageWindow->setVisible(true);
	pPageWindow->setAlwaysOnTop(true);
    m_ChatListEvent->SetEventType(ON_OPEN);
    ProcessPageEvent(m_ChatListEvent);
    return true;
}

bool ChatListPage::Close()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if(pPageWindow)
	{
		pPageWindow->setVisible(false);
		pPageWindow->setAlwaysOnTop(false);
	}
    else
        return false;
    return true;
}

void ChatListPage::AddChatWords(const char *pWords,D3DCOLOR color, D3DCOLOR bkcolor,D3DCOLOR shadow,const char * strName, long lType)
{
    m_ChatLogicEvent->ReceiveNewChatMsg(pWords, color, bkcolor, shadow, strName, (eChatType)lType);
}