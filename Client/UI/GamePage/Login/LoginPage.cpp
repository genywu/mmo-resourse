#include "stdafx.h"
#include "LoginPage.h"
#include "LoginEvent.h"
#include "../../../GameClient/Game.h"
#include "../../../EngineExtend/GameScene.h"
#include "../../../ui/UIManager.h"
#include "../../../ui/UIDef.h"
#include "../../../UIExtendComponent/GameLayer2D.h"
LoginPage::LoginPage():m_LoginSence(0),m_event(0)
{
    m_event = new LoginEvent;
}
LoginPage::~LoginPage()
{
    SAFE_DELETE(m_event);
}
void LoginPage::LoadPage()
{
    if(m_LoginSence == NULL)
    {
        m_LoginSence = new GameScene();
        //创建登陆3D背景
        m_LoginSence->CreateSence("model/interface/login2/map",
            "model/interface/login2/map/camera",
            "model/interface/login2/map/envcreature",
            "model/interface/login2/map/enveffect");
    }
    CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
        (CEGUI::System::getSingleton().getResourceProvider());

    rp->setResourceGroupDirectory(UIRes, "datafiles/imagesets/Login");
    CEGUI::ImagesetManager::getSingleton().
        createFromImageFile("login_topSideBar", "login-top.tga", UIRes);

    CEGUI::ImagesetManager::getSingleton().
        createFromImageFile("login_BottomSideBar", "login-bottom.tga", UIRes);

    CEGUI::ImagesetManager::getSingleton().
        createFromImageFile("login_CorpIcon", "title.tga", UIRes);

    SetLayOutFileName("Login.layout");
	m_event->SetEventType(ON_LOAD);
	ProcessPageEvent(m_event);
    m_LastLoginTime = 0;
}

bool LoginPage::DisplaySideBar()
{
    float fHeight = CEGUI::ImagesetManager::getSingleton().get("login_topSideBar").
        getImage("full_image").getSourceTextureArea().getHeight();
    GetInst(UIManager).DrawImage(0,0,SCREEN_WIDTH,(int)fHeight,"login_topSideBar");
        
    fHeight = CEGUI::ImagesetManager::getSingleton().get("login_BottomSideBar").
        getImage("full_image").getSourceTextureArea().getHeight();
    GetInst(UIManager).DrawImage(0,SCREEN_HEIGHT - (int)fHeight,SCREEN_WIDTH,(int)fHeight,"login_BottomSideBar");
    
    fHeight = CEGUI::ImagesetManager::getSingleton().get("login_CorpIcon").
        getImage("full_image").getSourceTextureArea().getHeight();;
    float width = CEGUI::ImagesetManager::getSingleton().get("login_CorpIcon").
        getImage("full_image").getSourceTextureArea().getWidth();
    GetInst(UIManager).DrawImage((SCREEN_WIDTH - (int)width)/2,40,"login_CorpIcon");

    return true;
}

void LoginPage::ReleasePage()
{
    SAFE_DELETE(m_LoginSence);
}

bool LoginPage::Open()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if (pPageWindow  == NULL)
    {
        LoadPage();
        pPageWindow = GetPageWindow(); 
    }
    pPageWindow->setVisible(true);
	m_event->SetEventType(ON_OPEN);
    ProcessPageEvent(m_event);
    return true;
}

bool LoginPage::Close()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if(pPageWindow)
        pPageWindow->setVisible(false);
    else
        return false;
    return true;
}


void LoginPage::Display()
{
	if(m_LoginSence)
	{
		m_LoginSence->RenderLayerBack();
		m_LoginSence->RenderLayerFront();
	}
    DisplaySideBar();
    // 显示版本号
    extern long g_lVersion;
    extern int SCREEN_WIDTH,SCREEN_HEIGHT;
    char str[256];
    sprintf(str, "Version %s [%d]", CClientResource::GetInstance()->GetVersion().c_str(), g_lVersion);
    GetGame()->OutPutText(SCREEN_WIDTH - GetGame()->GetFontWidth(str) - 12, SCREEN_HEIGHT - GetGame()->GetFontHeight(-1) - 6, str, lstrlen(str), D3DCOLOR_ARGB(255, 255,255,255));

    string strServerName("");
    sprintf(str, "当前所在区：%s",strServerName.c_str());
    int iStrlen   = (int)strlen(str);
    int iTextXPos = SCREEN_WIDTH - GetGame()->GetFontWidth(str) - 12;
    int iTextYPos = 6;
    GetGame()->OutPutText(iTextXPos - 1, iTextYPos, str, iStrlen, D3DCOLOR_ARGB(255, 0, 0, 0));
    GetGame()->OutPutText(iTextXPos + 1, iTextYPos, str, iStrlen, D3DCOLOR_ARGB(255, 0, 0, 0));
    GetGame()->OutPutText(iTextXPos, iTextYPos - 1, str, iStrlen, D3DCOLOR_ARGB(255, 0, 0, 0));
    GetGame()->OutPutText(iTextXPos, iTextYPos + 1, str, iStrlen, D3DCOLOR_ARGB(255, 0, 0, 0));
    GetGame()->OutPutText(iTextXPos, iTextYPos, str, iStrlen, D3DCOLOR_ARGB(255, 250, 190, 0));
	
	//GetInst(UIManager).OutPutRollText(40,40,"nihao",D3DCOLOR_ARGB(255,255,0,0),CEGUI::Rect(30,40,60,70));
#ifdef _DEBUG
    if (g_bDebug)
    {
        sprintf(str, "Debug Version[Debug]");
    }
    else
        sprintf(str, "Release Version[Debug]");
    GetGame()->OutPutText(4, 4, str, lstrlen(str), D3DCOLOR_ARGB(255, 255,255,255));
#else
    if (g_bDebug)
    {
        sprintf(str, "Debug Version[Release]");
        GetGame()->OutPutText(4, 4, str, lstrlen(str), D3DCOLOR_ARGB(255, 255,255,255));
    }
#endif

    // 界面显示
    UIManager::Render();
}
//
//void LoginPage::DisplaySideBar()
//{
//    //GameGraphics2D::GetInstance()->SetUIModelRendState();
//    extern int SCREEN_WIDTH,SCREEN_HEIGHT;
//    if(m_TopSideBar)
//        m_TopSideBar->Draw(0,0,(float)SCREEN_WIDTH,(float)m_TopSideBar->GetHeight());
//
//
//    if (m_BottomSideBar)			
//        m_BottomSideBar->Draw(0,SCREEN_HEIGHT - m_BottomSideBar->GetHeight(),(float)SCREEN_WIDTH,(float)m_BottomSideBar->GetHeight());
//
//
//    if (m_CorpIcon)
//        m_CorpIcon->Draw((SCREEN_WIDTH - m_CorpIcon->GetWidth())/2, 40);
//}
