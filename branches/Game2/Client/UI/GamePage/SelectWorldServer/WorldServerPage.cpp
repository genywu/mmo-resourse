#include "stdafx.h"
#include "WorldServerPage.h"
#include "WSEvent.h"

WorldServerPage::WorldServerPage()
{
    LoadPage();
}

void WorldServerPage::LoadPage()
{
    SetLayOutFileName("WorldServer.layout");    
    WSEvent wsevent(ON_LOAD);
    ProcessPageEvent(&wsevent);
}

void WorldServerPage::ReleasePage()
{

}

bool WorldServerPage::Open()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if (pPageWindow)
    {
        WSEvent event(ON_OPEN);
        ProcessPageEvent(&event);
        pPageWindow->setVisible(true);
        pPageWindow->activate();
    }
    else
    {
        assert("WorldServerPage Open Failed!");
        return false;
    }
    return true;
}

bool WorldServerPage::Close()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if(pPageWindow)
        pPageWindow->setVisible(false);
    else
        return false;
    return true;
}

void WorldServerPage::AddWorldInfo(long lID,char* strWorldName,long lState)
{
    WSEvent wsevent;
    wsevent.AddWorldInfo(lID,strWorldName,lState);
}

void WorldServerPage::ClearWorldInfo()
{
    WSEvent wsevent;
    wsevent.ClearWS();
}

void WorldServerPage::UpdateWSList()
{
    WSEvent wsevent(ON_UPDATE);
    ProcessPageEvent(&wsevent);
}