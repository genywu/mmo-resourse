#pragma once
#include "../../GamePage.h"
class ChatListEvent;
class ChatLogicEvent;
class ChatListPage:public GamePage
{
public:    
    ChatListPage();

    virtual ~ChatListPage(){};

    virtual void LoadPage();

    virtual void ReleasePage();

    virtual bool Open();

    virtual bool Close();

    void AddChatWords(const char *pWords,D3DCOLOR color, D3DCOLOR bkcolor,D3DCOLOR shadow,const char * strName, long lType);

private:
    ChatListEvent  * m_ChatListEvent;
    ChatLogicEvent * m_ChatLogicEvent;
};

#define GetChatListPage()  GetInst(ChatListPage)