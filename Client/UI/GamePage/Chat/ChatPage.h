#pragma once
#include "../../GamePage.h"

class ChatPage:public GamePage
{
public:    
    ChatPage();

    virtual ~ChatPage(){};

    virtual void LoadPage();

    virtual void ReleasePage();

    virtual bool Open();

    virtual bool Close();

};