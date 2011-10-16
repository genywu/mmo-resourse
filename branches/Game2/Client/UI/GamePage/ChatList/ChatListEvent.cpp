#include "stdafx.h"
#include "ChatListEvent.h"
#include "ChatListPage.h"
#include "../../../ui/UIDef.h"

ChatListEvent::ChatListEvent(EVENTTYPE type)
{

}

void ChatListEvent::OnPageLoad(GamePage *pPage)
{
    pPage->LoadPageWindow();
    pPage->GetPageWindow()->setZOrderingEnabled(false);
    pPage->GetPageWindow()->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
        CEGUI::Event::Subscriber(OnHide));
}

void ChatListEvent::OnPageOpen(GamePage *pPage)
{

}

void ChatListEvent::OnPageClose(GamePage *pPage)
{

}

void ChatListEvent::OnPageUpdate(GamePage *pPage)
{

}

void ChatListEvent::OnPageRelease(GamePage *pPage)
{

}


/*  class  ChatLogicEvent*/
void ChatLogicEvent::ReceiveNewChatMsg(const char *pWords,D3DCOLOR color, D3DCOLOR bkcolor,D3DCOLOR shadow,const char * strName, eChatType lType)
{
    tagMsg newMsg;
    newMsg.strContent = ToCEGUIString(pWords);
    newMsg.TextColor = color;
    newMsg.BkColor = bkcolor;
    newMsg.Shadow = shadow;
    newMsg.lType = lType;
    m_chatMsgQueue.push(newMsg);

    long MAX_CHAT_SIZE = 20;
    long lQueueSize = (long)m_chatMsgQueue.size();

    CEGUI::Listbox * pChatList = static_cast<CEGUI::Listbox*>(GetInst(ChatListPage).GetPageWindow()->getChild("ChatList"));
    
    static const float lLineWidth = pChatList->getListRenderArea().getWidth();

    float fontWidth = pChatList->getFont()->getTextExtent(pWords);
    static long lItemIndex = 0;
    size_t pos = 0;
    CEGUI::String strContext(ToCEGUIString(pWords));
    CEGUI::String strlineContent("");

    static float HoritalScroBarWidth = pChatList->getHorzScrollbar()->getPixelSize().d_width;
    //GetLocalTime(&g_startTime);
    do//
    {
        size_t newlineWidth = 0;
        size_t  iWordStep = 1;
        while(newlineWidth <= lLineWidth-16 && iWordStep <= strContext.length() && pos <= (int)strContext.length())
        {
            strlineContent = newMsg.strContent.substr(pos,iWordStep++);
            newlineWidth = (long)pChatList->getFont()->getTextExtent(strlineContent);
        }
        
        CEGUI::ListboxTextItem *pNewItem = new CEGUI::ListboxTextItem(strlineContent, lItemIndex++);
        pNewItem->setTextParsingEnabled(false);
        pNewItem->setTextColours(color);
        pChatList->addItem(pNewItem);    
        CEGUI::Scrollbar *pVertBar = pChatList->getVertScrollbar();
        float fScroBarPos = pVertBar->getScrollPosition();
        float ftotalHeight   = (float)pChatList->getTotalItemsHeight();
        pVertBar->setScrollPosition(ftotalHeight);
        fontWidth -= lLineWidth;
        pos += iWordStep-1;
    }
    while (fontWidth>0);
    //GetLocalTime(&g_endTime);
    //PutDebugStrCurRunTime(g_endTime.wMilliseconds);
    if (lQueueSize >= MAX_CHAT_SIZE)
    {   
        CEGUI::String strDell = m_chatMsgQueue.front().strContent;
        m_chatMsgQueue.pop();
        float strDelPixlWidth = pChatList->getFont()->getTextExtent(strDell);
        //do
        //{
        //    CEGUI::ListboxItem *pDelItem = pChatList->getListboxItemFromIndex(0);
        //    pChatList->removeItem(pDelItem);
        //    strDelLength -= lLineWidth;
        //}
        //while(strDelLength > lLineWidth);

        float lingcout = strDelPixlWidth/lLineWidth;
        if ((long)strDelPixlWidth%(long)lLineWidth != 0)
        {
            lingcout++;
        }
        for (int l = 0; l< (long)lingcout; l++)
        {
            CEGUI::ListboxItem *pDelItem = pChatList->getListboxItemFromIndex(0);
            pChatList->removeItem(pDelItem);
        }
    }
    pChatList->getHorzScrollbar()->hide();
}
