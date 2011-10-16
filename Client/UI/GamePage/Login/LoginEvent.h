#pragma once

#include "../../PageEvent.h"

class LoginEvent:public PageEvent
{
public:
    LoginEvent(){}
    LoginEvent(EVENTTYPE type);
    ~LoginEvent();
    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);
    void         OnLoginLS();

    bool HandleLogin(const CEGUI::EventArgs &e);
    bool HandleExitBtn(const CEGUI::EventArgs &e);
};