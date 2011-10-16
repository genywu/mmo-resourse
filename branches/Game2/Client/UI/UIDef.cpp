#include "StdAfx.h"
#include "UIDef.h"
#include "UIManager.h"
#include "Windows/SkillDef.h"
#include "Windows/TeamDef.h"
#include "Windows/QuestDef.h"
#include "Windows/ShopCityDef.h"
#include "Windows/GeniusDef.h"
#include "Windows/NotifyCenterDef.h"
using namespace CEGUI;
// 下面的接口声明须依字母序排列
extern CEGUI::Window* InitAuction();
extern CEGUI::Window* InitAttribute();
extern CEGUI::Window* InitChatPrompt();
extern CEGUI::Window* InitGameExit();
extern CEGUI::Window* InitGenius();
extern CEGUI::Window* InitLinkman();
extern CEGUI::Window* InitMiniPetPage();
extern CEGUI::Window* InitMiniMapFrame();
extern CEGUI::Window* InitPersonalMail();
extern CEGUI::Window* InitQuest();
extern CEGUI::Window* InitSelectLS();
extern CEGUI::Window* InitServant();
extern CEGUI::Window* InitSkill();
extern CEGUI::Window* InitShopCity();
extern CEGUI::Window* InitShopCityChild();
extern CEGUI::Window* InitShopCityItemSet();
extern CEGUI::Window* InitShopCityBuy();
extern CEGUI::Window* InitTalkBox();
extern CEGUI::Window* InitTeam();
extern CEGUI::Window* InitPetStrengthen();
extern CEGUI::Window* InitPlayerRolePage();
extern CEGUI::Window* InitPlayerShopMessageBoardPage();
extern CEGUI::Window* InitPlayerShopOrderPage();
extern CEGUI::Window* InitPlayerShopPage();
extern CEGUI::Window* InitPlayerTradePage();
extern CEGUI::Window* InitObjectPage();
extern CEGUI::Window* InitTeamCaptainMenu();
extern CEGUI::Window* InitTeamMenuPage();
extern CEGUI::Window* InitMiniMapFrame();
extern CEGUI::Window* InitMsgBoxPage();
extern CEGUI::Window* InitNotifyCenter();
extern CEGUI::Window* InitFactionPage();
extern CEGUI::Window* InitFactionList();
extern CEGUI::Window* InitCreateFactionPage();
extern CEGUI::Window* InitFactionMemInfoPage();
extern CEGUI::Window* InitFactionApplyPage();
extern CEGUI::Window* InitJoinFactionPage();
extern CEGUI::Window* InitWorldMap();
extern CEGUI::Window* InitGoodsDesp();

CEGUI::Window* GetWindow(const CEGUI::String& name);
CEGUI::Window* GetFocusWindow();
CEGUI::Window* LoadUI(const CEGUI::String& name);

void CloseAllWindow()
{
    if (GetWndMgr().isWindowPresent("Servant"))
        GetWndMgr().getWindow("Servant")->hide();
    if (GetWndMgr().isWindowPresent("MiniPetPage"))
        GetWndMgr().getWindow("MiniPetPage")->hide();
}

void DestroyAllWindow()
{
    if (GetWndMgr().isWindowPresent("Servant"))
        GetWndMgr().destroyWindow("Servant");
    if (GetWndMgr().isWindowPresent("MiniPetPage"))
        GetWndMgr().destroyWindow("MiniPetPage");
}

CEGUI::Window* GetWindow(const CEGUI::String& name)
{
    if (!GetWndMgr().isWindowPresent(name))
    {
        // 下面的接口调用须依字母序排列
        if (name == "Attribute")
            return InitAttribute();
        else if ( name == "Auction")
            return InitAuction();
        else if ( name == "CreateFaction")
            return InitCreateFactionPage();
        else if (name == "ChatPrompt")
            return InitChatPrompt();
        else if (name == "fwLinkman")
            return InitLinkman();
        else if (name == "FactionPage")
            return InitFactionPage();
        else if (name == "FactionList")
            return InitFactionList();
        else if (name == "FactionApplyList")
            return InitFactionApplyPage();
        else if (name == "FactionMemInfo")
            return InitFactionMemInfoPage();
        else if (name == "ApplyJoinFaction")
            return InitJoinFactionPage();
        else if (name == "GameExit")
            return InitGameExit();
		else if (name == GENIUS_PAGE_NAME)
			return InitGenius();
        else if (name == "MiniPetPage")
            return InitMiniPetPage();
        else if (name == "MiniMapFrame")
            return InitMiniMapFrame();
		else if ( name == NOTIFYCENTER_PAGE_NAME)
			return InitNotifyCenter();
        else if (name == "PersonalMail")
            return  InitPersonalMail();
        else if (name == QUEST_PAGE_NAME)
            return InitQuest();
        else if (name == "MainRole")
            return InitPlayerRolePage();
        else if (name == "MessageBox")
            return InitMsgBoxPage();
        else if (name == "ObjectPage")
            return InitObjectPage();
        else if (name == "MiniPetPage")
            return InitMiniPetPage();
        else if (name == "SelectLS")//选登陆服务器
            return InitSelectLS();
        else if (name == "Servant")
            return InitServant();
        else if (name == SHOPCITY_PAGE_NAME)
            return InitShopCity();
        else if (name == SHOPCITY_BUY_PAGE_NAME)
            return InitShopCityBuy();
        else if (name == SHOPCITY_ITEMSET_PAGE_NAME)
            return InitShopCityItemSet();
        else if (name == SHOPCITY_CHILD_PAGE_NAME)
            return InitShopCityChild();
        else if (name == SKILL_PAGE_NAME)
            return InitSkill();
        else if (name == "TalkBox")
            return InitTalkBox();        
        else if (name == TEAM_PAGE_NAME)
            return InitTeam();
        else if (name == TEAM_CAPTAINMENU_NAME)
            return InitTeamCaptainMenu();
        else if (name == "TeamMenuPage")
            return InitTeamMenuPage();
        else if (name == "PetStrengthen")
            return InitPetStrengthen();
        else if (name == "WorldMap")
            return InitWorldMap();
        else if (name == "PlayerShopMessageBoard")
            return InitPlayerShopMessageBoardPage();
        else if (name == "PlayerShopOrder")
            return InitPlayerShopOrderPage();
        else if (name == "PlayerShop")
            return InitPlayerShopPage();
        else if (name == "PlayerTrade")
            return InitPlayerTradePage();
        else if (name == "GoodsDesp")
            return InitGoodsDesp();
        else
            return NULL; // 子窗口
    }

    return GetWndMgr().getWindow(name);
}

void FireUIEvent(const char* id, const char* e)
{
    CEGUI::Window* wnd = GetWindow(id);
    if (NULL != wnd)
    {
        CEGUI::WindowEventArgs args(wnd);
        wnd->fireEvent(e, args);
    }
}

const char * ReloadUILayout(char * WndName)
{
    CEGUI::Window* wnd =  GetCEGUISystem().getWindowContainingMouse();

    while(wnd->getParent()->getName() != "Root")
    {
        wnd = wnd->getParent();
    }
    if (wnd)
    {

        strcpy(WndName,wnd->getName().c_str());
        FireUIEvent(WndName, CEGUI::Window::EventHidden.c_str());
        UIManager::FreshCurEditComponent();
        GetWndMgr().destroyWindow(WndName);
        wnd = LoadUI(WndName);
        wnd->setVisible(true);
        UIManager::FreshCurEditComponent();
    }
    return WndName;
}

void SaveLayout()
{
    CEGUI::Window * PageWin = GetCEGUISystem().getWindowContainingMouse();
    if(PageWin->getParent() == NULL)
        return;
    while(PageWin->getParent()->getName() != "Root")
    {
        PageWin = PageWin->getParent();
    }
    if(PageWin == NULL)
    {
        MessageBox(g_hWnd,"NotFindWindow","OK",MB_OK);
        return;
    }
    CEGUI::String layoutFileName = PageWin->getName() + ".layout";
    std::fstream stream;
    char strLayoutPath[256] = "";
    const char * filename = layoutFileName.c_str();
    sprintf(strLayoutPath,"datafiles/layouts/%s",filename);
    stream.open(strLayoutPath);
    if(stream.is_open())
    {
        CEGUI::WindowManager::getSingleton().writeWindowLayoutToStream(*PageWin,stream);
        stream.close();
        static char info[256];
        sprintf(info,"%s Save successful!",filename);
        MessageBox(g_hWnd,info,"OK",MB_OK);
    }
}

CEGUI::Window* GetFocusWindow()
{
    CEGUI::Window* root = GetWndMgr().getWindow("Root");
    CEGUI::Window* wnd = NULL;
    for(long i = (long)root->getChildCount()-1;i>=0;i--)
    {
        wnd = root->getChildAtIdx(i);
        if(wnd->isVisible())
        {
            const char* name = wnd->getName().c_str();
            if (wnd->getName() == "Root" || 
                wnd->getName() == "CEGUI::System::default__auto_tooltip__"||
                wnd->getName() == "MainBarPage" ||
                wnd->getName() == "ChatListPage"||
                wnd->getName() == "MainRole" ||
                wnd->getName() == "MiniMapFrame" ||
                wnd->getName() == "MiniPetPage" ||
                wnd->getName() == "ChatListPage" ||
                wnd->getName() == TEAM_PAGE_NAME ) 
            {
                wnd = NULL;
                continue;
            }
            return wnd;
        }
        else
            wnd = NULL;
    }

    return wnd;
}

CEGUI::Window* LoadUI(const CEGUI::String& name)
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::DefaultWindow* root = (CEGUI::DefaultWindow*)winMgr.getWindow("Root");
    CEGUI::String layoutName = name + ".layout";
    CEGUI::Window* wnd = winMgr.loadWindowLayout(layoutName);
    wnd->setVisible(false);
    root->addChildWindow(wnd);
    wnd->setMaxSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
    wnd->setMinSize(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f)));
    wnd->subscribeEvent(EVENT_OPEN,
        CEGUI::Event::Subscriber(OnShown));
    wnd->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
        CEGUI::Event::Subscriber(OnHide));
    wnd->subscribeEvent(EVENT_CLOSE,
        CEGUI::Event::Subscriber(OnHide));
    wnd->subscribeEvent(EVENT_SWITCH,
            CEGUI::Event::Subscriber(OnSwitch));
    return wnd;
}

bool OnShown(const CEGUI::EventArgs &args)
{
    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(args).window;
    wnd->setVisible(true);
    wnd->setEnabled(true);
    wnd->setAlwaysOnTop(true);
    return true;
}

bool OnHide(const CEGUI::EventArgs &args)
{
    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(args).window;
    wnd->setVisible(false);
    wnd->setEnabled(false);
    wnd->setAlwaysOnTop(false);
    return true;
}

bool OnSwitch(const CEGUI::EventArgs &args)
{
    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(args).window;
    if(wnd->isVisible())
    {
        FireUIEvent(wnd->getName().c_str(),EVENT_CLOSE);
    }
    else
    {
        FireUIEvent(wnd->getName().c_str(),EVENT_OPEN);
    }
    return true;
}

CEGUI::String ToCEGUIString(const char *str)
{
    const static CEGUI::String txt = "";
    if(str == NULL)
        return txt;

    int ret = 1;
    size_t len = strlen(str);
    if(!len)
        return txt;

    LPINT iRet = 0;
    if(IsTextUnicode(str, (int)len, iRet))
    {
        return CEGUI::String((CEGUI::utf8*)str);
    }

    const int len1 = MultiByteToWideChar(CP_ACP, 0, str, (int)len, NULL, 0);
    std::vector<wchar_t> buf1(len1+1);
    ret = MultiByteToWideChar(CP_ACP, 0, str, (int)len, &buf1[0], len1);

    const int len2 = WideCharToMultiByte(CP_UTF8, 0, &buf1[0], (int)len1, NULL, 0, NULL, NULL);
    std::vector<char> buf2(len2+1);
    ret = WideCharToMultiByte(CP_UTF8, 0, &buf1[0], len1, &buf2[0], (int)len2, NULL, NULL);
    buf2[len2] = '\0';
    
    return CEGUI::String(reinterpret_cast<CEGUI::utf8*>(&buf2[0]));
}

CEGUI::String & ToCEGUIString(const char *str,CEGUI::String &outString)
{
    if(str == NULL)
        return outString="";

    int ret = 1;
    size_t len = strlen(str);
    if(!len)
        return outString="";
    
    LPINT result = 0;
    if(IsTextUnicode(str,(int)len,result))
    {
        return outString = CEGUI::String((CEGUI::utf8*)str);  
    }

    const int len1 = MultiByteToWideChar(CP_ACP, 0, str, (int)len, NULL, 0);
    std::vector<wchar_t> buf1(len1+1);
    ret = MultiByteToWideChar(CP_ACP, 0, str, (int)len, &buf1[0], len1);
   
    const int len2 = WideCharToMultiByte(CP_UTF8, 0, &buf1[0], (int)len1, NULL, 0, NULL, NULL);
    std::vector<char> buf2(len2+1);
    ret = WideCharToMultiByte(CP_UTF8, 0, &buf1[0], len1, &buf2[0], (int)len2, NULL, NULL);
    buf2[len2] = '\0';
    std::string strText = &buf2[0];
    
    return outString = reinterpret_cast<CEGUI::utf8*>(&buf2[0]);
}


const char *CEGUIStringToAnsiChar(const CEGUI::String &inStr, char *outStr)
{
    const char *strUTF_8 = inStr.c_str();
    int len = (int)strlen(strUTF_8);
    if (!strUTF_8 || !outStr || len == 0)
    {
        return outStr;
    }

    const int len1 = MultiByteToWideChar(CP_UTF8 , 0, strUTF_8, len - 1, NULL, 0);
    std::vector<wchar_t> buf1(len1);
    MultiByteToWideChar(CP_UTF8, 0, strUTF_8, len, &buf1[0], (int)len1);

    const int len2 = WideCharToMultiByte(CP_ACP, 0, &buf1[0], (int)len1, NULL, 0, NULL, NULL);
    std::vector<char>buf2(len2+1);
    WideCharToMultiByte(CP_ACP, 0, &buf1[0], (int)len1, &buf2[0], (int)len2, NULL, NULL);
    buf2[len2] = '\0';

    strcpy(outStr,&buf2[0]);
    return outStr;
}

const char *CEGUIStringToAnsiChar(const CEGUI::String &inStr)
{
    const char *strUTF_8 = inStr.c_str();
    int len = (int)strlen(strUTF_8);
    if (!strUTF_8 || len == 0)
    {
        return NULL;
    }

    const int len1 = MultiByteToWideChar(CP_UTF8 , 0, strUTF_8, len, NULL, 0);
    std::vector<wchar_t> buf1(len1);
    MultiByteToWideChar(CP_UTF8, 0, strUTF_8, len, &buf1[0], (int)len1);

    const int len2 = WideCharToMultiByte(CP_ACP, 0, &buf1[0], (int)len1, NULL, 0, NULL, NULL);
    static std::vector<char>buf2;
    buf2.resize(len2+1);
    WideCharToMultiByte(CP_ACP, 0, &buf1[0], (int)len1, &buf2[0], (int)len2, NULL, NULL);
    buf2[len2] = '\0';

    return &buf2[0];
}

const CEGUI::Image& SetBackGroundImage(CEGUI::GUISheet *pBackWin, const char *ImageSetName,
                                       const char * filePath,const char *ImageFileName,
                                       bool autoadjustWndsize)
{
    using namespace CEGUI;
    DefaultResourceProvider* rp = static_cast<DefaultResourceProvider*>
        (System::getSingleton().getResourceProvider());

    rp->setResourceGroupDirectory(UIRes, filePath);
    Imageset *pMainBarImageSet = 0;
    pMainBarImageSet = &ImagesetManager::getSingleton().createFromImageFile(ImageSetName, ImageFileName,UIRes);
    const Image &backgroundImage = pMainBarImageSet->getImage("full_image");
    //
    if(autoadjustWndsize == false)
        pMainBarImageSet->setAutoScalingEnabled(false);

    Rect ImageRect = backgroundImage.getSourceTextureArea();
    float lImageWidth  = ImageRect.getWidth();
    float lImageHeight = ImageRect.getHeight();
    
    pBackWin->setMaxSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
    pBackWin->setMinSize(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
    if(autoadjustWndsize == true)
    {    
        pBackWin->setSize(UVector2(cegui_absdim(lImageWidth), cegui_absdim(lImageHeight)));
        // disable frame and standard background
        pBackWin->setProperty("FrameEnabled", "false");
        pBackWin->setProperty("BackgroundEnabled", "false");
    }
    // set the background image
    char ppValue[128];
    sprintf(ppValue,"set:%s image:full_image",ImageSetName);
    pBackWin->setProperty("Image", ppValue);
    return backgroundImage;
}

long GetIndexByChild(const CEGUI::Window *pParent, const CEGUI::Window *pChild)
{
    if (!pParent)
        return -1;
    const size_t child_count = pParent->getChildCount();
    if (!pParent->isChild(pChild))
        return -1;
    size_t lIndex = 0;
    for (;lIndex<child_count;lIndex++)
    {
        if (pParent->getChildAtIdx(lIndex) == pChild)
        {
            return (long)lIndex;
        }
    }
    return -1;
}

void SetText(CEGUI::Window * pWnd, const char * Text)
{
    if(!pWnd || !Text)
    {    
        assert("Null Pointer Error!");
        return;
    }
    pWnd->setText(ToCEGUIString(Text));
}

const char *GetText(CEGUI::Window *pWnd)
{
    if(!pWnd)
    {    
        assert("Null Pointer Error!");
        return 0;
    }
    return CEGUIStringToAnsiChar(pWnd->getText());
}

//PopupMenu的特点：
//打开后，需要位于所有界面的顶层
//失去焦点后，需要隐藏起来
static bool OnHide_PopupMenu(const EventArgs& args)
{
    Window* wnd = WEArgs(args).window;
    wnd->setAlwaysOnTop(false);
    return false;
}
static bool OnShow_PopupMenu(const EventArgs& args)
{
    Window* wnd = WEArgs(args).window;
    wnd->activate();
    wnd->setAlwaysOnTop(true);
    return false;
}
static bool OnDeactive_PopupMenu(const EventArgs& args)
{
    Window* wnd = WEArgs(args).window;
    wnd->hide();
    return false;
}
PopupMenu* CreatePopupMenu(const String& name, const String& parent)
{
    WindowManager& wndMgr = WindowManager::getSingleton();
    PopupMenu* popup = (PopupMenu*)wndMgr.createWindow("TaharezLook/PopupMenu", name);
    popup->subscribeEvent(Window::EventHidden, OnHide_PopupMenu);
    popup->subscribeEvent(Window::EventShown, OnShow_PopupMenu);
    popup->subscribeEvent(Window::EventDeactivated, OnDeactive_PopupMenu);
    wndMgr.getWindow(parent)->addChildWindow(popup);
    return popup;
}
MenuItem* CreateMenuItem(PopupMenu* popup, const String& name, const String& text, const SubscriberSlot& onClicked)
{
    WindowManager& wndMgr = WindowManager::getSingleton();
    MenuItem* menuItem = (MenuItem*)wndMgr.createWindow("TaharezLook/MenuItem", name);
    menuItem->setText(text);
    menuItem->subscribeEvent(MenuItem::EventClicked, onClicked);
    popup->addItem(menuItem);
    return menuItem;
}