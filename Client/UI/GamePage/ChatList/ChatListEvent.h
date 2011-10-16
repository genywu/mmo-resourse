#pragma once

#include "../../PageEvent.h"
#include "../../../../Public/Common/BaseDef.h"

class ChatListEvent:public PageEvent
{
public:
    ChatListEvent(){}
    ChatListEvent(EVENTTYPE type);
    ~ChatListEvent(){}
    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);

};

class ChatLogicEvent
{
public:
    struct tagMsg
    {
        D3DCOLOR  TextColor;
        D3DCOLOR  BkColor;
        D3DCOLOR  Shadow;
        eChatType lType;
        CEGUI::String    strContent;
    };
    void ReceiveNewChatMsg(const char *pWords,D3DCOLOR color, D3DCOLOR bkcolor,D3DCOLOR shadow,const char * strName, eChatType lType);
    ChatLogicEvent(){}
    ~ChatLogicEvent(){}
private:
    std::queue<tagMsg>  m_chatMsgQueue;
};