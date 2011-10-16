#include "stdafx.h"
#include "ChatPage.h"
#include "ChatEvent.h"

ChatPage::ChatPage()
{
    LoadPage();
}

void ChatPage::LoadPage()
{
    SetLayOutFileName("ChatPage.layout");
    ChatEvent event(ON_LOAD);
    ProcessPageEvent(&event);
}

void ChatPage::ReleasePage()
{

}

bool ChatPage::Open()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if (pPageWindow)
    {
        pPageWindow->setVisible(true); 
		pPageWindow->setEnabled(true);
        ChatEvent event(ON_OPEN);
        ProcessPageEvent(&event);
    }
    else
    {
        assert("PackagePage Open failed!!");
        return false;
    }
    return true;
}

bool ChatPage::Close()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if(pPageWindow)
	{
		pPageWindow->setVisible(false);
	}
    else
        return false;
    ChatEvent event(ON_CLOSE);
    ProcessPageEvent(&event);
    return true;
}
