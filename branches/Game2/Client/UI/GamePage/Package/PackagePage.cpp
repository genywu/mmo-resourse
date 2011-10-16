#include "stdafx.h"
#include "PackagePage.h"
#include "PackageEvent.h"
PackagePage::PackagePage()
{
    LoadPage();
}

PackagePage::~PackagePage()
{
    SAFE_DELETE(m_PackageEvent);
}

void PackagePage::LoadPage()
{
    if(m_PackageEvent == NULL)
        m_PackageEvent = new PackageEvent;
    SetLayOutFileName("PackagePage.layout");
    m_PackageEvent->SetEventType(ON_LOAD);
    ProcessPageEvent(m_PackageEvent);
}

void PackagePage::ReleasePage()
{
    
}

bool PackagePage::Open()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if (pPageWindow)
    {
        pPageWindow->setVisible(true);
        pPageWindow->setAlwaysOnTop(true);
        pPageWindow->setEnabled(true);
        m_PackageEvent->SetEventType(ON_OPEN);
        ProcessPageEvent(m_PackageEvent);
    }
    else
    {
        assert("PackagePage Open failed!!");
        return false;
    }
    return true;
}

bool PackagePage::Close()
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

void PackagePage::UpdatePage(long lPos)
{
    m_PackageEvent->SetUpdatePos(lPos);
    m_PackageEvent->SetEventType(ON_UPDATE);
    ProcessPageEvent(m_PackageEvent);
}