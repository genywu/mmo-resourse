#include "stdafx.h"
#include "CreateRolePage.h"
#include "CreateRoleEvent.h"
#include "../../../EngineExtend/GameScene.h"
#include "../../../ui/UIManager.h"
CreateRolePage::CreateRolePage():m_event(0)
{
    m_event = new CREvent();
	//m_CreateSence = NULL;
}
CreateRolePage::~CreateRolePage()
{
    SAFE_DELETE(m_event);
}
void CreateRolePage::LoadPage()
{
    //if (m_CreateSence == NULL)
    //{
    //    m_CreateSence = new GameScene();
    //    m_CreateSence->CreateSence("model/interface/selectchar/map",
    //        "model/interface/selectchar/map/camera_end",
    //        "model/interface/selectchar/map/envcreature",
    //        "model/interface/selectchar/map/enveffect");
    //}
    SetLayOutFileName("CreateRolePage.layout");    

    m_event->SetEventType(ON_LOAD);
    ProcessPageEvent(m_event);
}

void CreateRolePage::ReleasePage()
{
    if(m_event)
    {   
        m_event->SetEventType(ON_RELEASE);
        ProcessPageEvent(m_event);
    }
    //SAFE_DELETE(m_CreateSence); 
}

bool CreateRolePage::Open()
{
    CEGUI::Window *pPage = GetPageWindow();
    if (pPage == NULL)
    {
        LoadPage();
        pPage = GetPageWindow();
    }
    m_event->SetEventType(ON_OPEN);
    ProcessPageEvent(m_event);
    pPage->setVisible(true);
    pPage->activate();
    return true;
}

bool CreateRolePage::Close()
{
    CEGUI::Window *pPage = GetPageWindow();
    if(pPage)
        pPage->setVisible(false);
    else
        return false;
    return true;
}

void CreateRolePage::SetRandCountryRange(BYTE lRange)
{
    CREvent::SetRandCountryRange(lRange);
}

void CreateRolePage::Display()
{
    //m_CreateSence->RenderLayerBack();
	if(m_event)
		m_event->Display();
    // ½çÃæÏÔÊ¾
    UIManager::Render();
}