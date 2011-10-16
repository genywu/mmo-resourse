#pragma once

#include "../../PageEvent.h"

class MainBarEvent:public PageEvent
{
public:
    MainBarEvent(){}
    MainBarEvent(EVENTTYPE type);
    ~MainBarEvent(){}
    virtual void OnPageLoad(GamePage *pPage);
    virtual void OnPageOpen(GamePage *pPage);
    virtual void OnPageClose(GamePage *pPage);
    virtual void OnPageUpdate(GamePage *pPage);
    virtual void OnPageRelease(GamePage *pPage);

    bool GamePageOp(const CEGUI::EventArgs &e);
};