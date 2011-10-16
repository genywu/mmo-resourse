#include "stdafx.h"
#include "UIManager.h"
#include <RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h>
#include <ScriptingModules/LuaScriptModule/CEGUILua.h>
#include "GamePage/GoodsViewer/GoodsRender.h"
#include "../GameClient/ClientApp/Other/Video.h"
#include "../GameClient/ClientApp/Other/VideoManager.h"
#include "../GameClient/Game.h"
#include "../Input/KeyBoard.h"
#include "../Input/Mouse.h"
#include "Pub/MyRenderedStringParser.h"
#include "../ui/UIExtendComponent/3DIconEffect.h"
#include "../ui/UIExtendComponent/AnimEffectIcon.h"
#include "../ui/UIExtendComponent/GameLayer2D.h"
#include "UIDef.h"
#include "../ui/UIExtendComponent/FalProgressBarEx.h"
#include "../ui/Pub/RenderedStringRollTextComponent.h"
// setup default-default path
#ifndef CEGUI_DATAPATH
#define CEGUI_DATAPATH "datafiles"
#endif

/*
静态类型成员声明
*/
CEGUI::Direct3D9Renderer	* UIManager::m_pCeguiD3D9Render = 0;
CEGUI::System				* UIManager::m_pSystemCEGUI = 0;
CEGUI::LuaScriptModule      * UIManager::m_pScriptModule = 0;
D3DXMATRIX UIManager::s_matWorld;
D3DXMATRIX UIManager::s_matView;
D3DXMATRIX UIManager::s_matProj;
map<string , GamePage *>  UIManager::mapPageManager;
render::Camera            UIManager::m_Camera;
DWORD	UIManager::m_FVF;
IDirect3DVertexShader9	*	UIManager::m_VertexShader = 0; ///< 顶点shader
IDirect3DPixelShader9	*	UIManager::m_PixelShader = 0; ///< 像素shader
DWORD UIManager::m_LIGHTING;
DWORD UIManager::m_FOGENABLE;
DWORD UIManager::m_ZENABLE;
DWORD UIManager::m_ALPHATESTENABLE;
DWORD UIManager::m_CULLMODE;
DWORD UIManager::m_FILLMODE;
DWORD UIManager::m_SCISSORTESTENABLE;

DWORD UIManager::m_ZWRITENABLE;  
DWORD UIManager::m_SAMPADDRESSU;
DWORD UIManager::m_SAMPADDRESSV;
DWORD UIManager::m_COLORARG1;
DWORD UIManager::m_COLORARG2;
DWORD UIManager::m_COLOROP;
DWORD UIManager::m_ALPHAARG1;
DWORD UIManager::m_ALPHAARG2;
DWORD UIManager::m_ALPHAOP;
DWORD UIManager::m_MINFILTER;
DWORD UIManager::m_MAGFILTER;
DWORD UIManager::m_TSS1_COLOROP;
DWORD UIManager::m_RS_ALPHABLENDENABLE;
DWORD UIManager::m_RS_SRCBLEND;
DWORD UIManager::m_RS_DESTBLEND;
DWORD UIManager::m_RS_SRCBLENDALPHA;
DWORD UIManager::m_RS_DESTBLENDALPHA;
CEGUI::Window * UIManager::m_UserEditWin = 0;
MyRenderedStringParser   * UIManager::myRenderParser = new MyRenderedStringParser;
map<CEGUI::Window *,C3DIconEffect*>   UIManager::map3DIconManager;
map<CEGUI::Window *,AnimEffectIcon*>  UIManager::mapAnimIconManager;
Layer2D * UIManager::gameLayer2D;
UIManager::UIManager():d_fps_geometry(0),d_logo_geometry(0)
{

}

void UIManager::Init()
{
    LPDIRECT3DDEVICE9 pDevice = render::Interface::GetInstance()->GetDevice();
    Create(pDevice);
}

void UIManager::Fini()
{
    Destroy();
}

void UIManager::Create(LPDIRECT3DDEVICE9 pDeviece)
{
    D3DXMatrixIdentity(&s_matWorld);
    D3DXMatrixIdentity(&s_matView);
    D3DXMatrixIdentity(&s_matProj);

    render::Interface *pInterface = render::Interface::GetInstance();
    int iWidth =  pInterface->GetWndWidth();
    int iHeight = pInterface->GetWndHeight();
    float fAspect = (float)iWidth / (float)iHeight;
    m_Camera.SetAspect(fAspect);
    m_Camera.SetFOVY(D3DXToRadian(60));
    m_Camera.SetNearClipPlane(0.01f);
    m_Camera.SetFarClipPlane(1000.0f);
    m_Camera.SetPosition(&D3DXVECTOR3(0,0,0));
    m_Camera.SetView(&D3DXVECTOR3(0.0f,0,0.0f));

    m_pCeguiD3D9Render = &CEGUI::Direct3D9Renderer::create(pDeviece);

    // initialise the gui system
    if(m_pCeguiD3D9Render)
        m_pSystemCEGUI	= &CEGUI::System::create(*m_pCeguiD3D9Render, 0, 0, 0, 0, "", "log/CEGUI.log");


    //getWindowRendererModule().registerFactory("C3DIconRender");
    //m_pScriptModule = &CEGUI::LuaScriptModule::create();
    //m_pSystemCEGUI->setScriptingModule(m_pScriptModule);

    initialiseResourceGroupDirectories();
    initialiseDefaultResourceGroups();

    CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

    CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
    //注册并加载自定义的进度条渲染器
    CEGUI::WindowRendererManager::addFactory<CEGUI::TplWindowRendererFactory<FalagardProgressBarEx> >();
   
    m_pSystemCEGUI->setDefaultFont("MSYH");
    m_pSystemCEGUI->setDefaultTooltip ("TaharezLook/Tooltip");
    //CEGUI::FontManager::getSingleton().createAll("*.font", "fonts");

    ////m_pSystemCEGUI->setDefaultMouseCursor("TaharezLook", "MouseArrow");
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    CEGUI::DefaultWindow* root = (CEGUI::DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
    m_pSystemCEGUI->setGUISheet(root);
    gameLayer2D = new Layer2D;
    CEGUI::Renderer *pCEGUIRender = CEGUI::System::getSingleton().getRenderer();

    //const CEGUI::Rect scrn(CEGUI::Vector2(0, 0), pCEGUIRender->getDisplaySize());

    CEGUI::RenderingRoot *pRenderRoot = &pCEGUIRender->getDefaultRenderingRoot();
    //pRenderRoot->subscribeEvent(CEGUI::RenderingSurface::EventRenderQueueStarted,
    //    CEGUI::Event::Subscriber(&UIManager::Draw2DLayer),GetInst(UIManager));
    //设置文字渲染解析类
    m_pSystemCEGUI->setDefaultCustomRenderedStringParser(myRenderParser);

	//加入自己的特效类
	//RenderEffectManager::getSingleton().addEffect<RenderedStringRollTextComponent>("RollTextButton");
	//WindowFactoryManager::getSingleton().addFalagardWindowMapping("TaharezLook/RollTextButton",
	//	"CEGUI/PushButton",
	//	"TaharezLook/Button",
	//	"Falagard/Button",
	//	"RollTextButton");
    //注册

    //CEGUI::FrameWindow* wnd = (CEGUI::FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo Window");
    //root->addChildWindow(wnd);

    //loginPage->setCloseButtonEnabled(true);
    //loginPage->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,CEGUI::Event::Subscriber(&Destroyed));

    //m_pSystemCEGUI->setDefaultTooltip("Hello    World !");

    // create a script module.
    //CEGUI::LuaScriptModule& scriptmod(CEGUI::LuaScriptModule::create());

    // tell CEGUI to use this scripting module

    // execute the demo8 script which controls the rest of this demo
    //m_pSystemCEGUI->executeScriptFile("demo8.lua");
}

void   UIManager::OutPutText(int iX,int iY,const char *pszString, D3DCOLOR color, int iStyleIndex,
                             const float space_extra ,const float x_scale , const float y_scale)
{
    gameLayer2D->RenderText(iX,iY,pszString, color, iStyleIndex ,space_extra, x_scale, y_scale);
}


void   UIManager::Clear2DBuff()
{
    gameLayer2D->ClearBuffer();
}

bool    UIManager::isImageSetDefined(const char *ImageSetName)
{
    bool isDef = CEGUI::ImagesetManager::getSingleton().isDefined(ToCEGUIString(ImageSetName));
    return isDef;
}
void UIManager::CreateImageSetFromFile(const char *ImageSetName,const char * filePath)
{
    CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
        (CEGUI::System::getSingleton().getResourceProvider());
    static char strPath[128];
    static char strName[128];
    GetFilePath(filePath,strPath);
    GetFileName(filePath,strName);
    rp->setResourceGroupDirectory(UIRes, strPath);
    CEGUI::Imageset *pImageset = &CEGUI::ImagesetManager::getSingleton().
        createFromImageFile(ToCEGUIString(ImageSetName),ToCEGUIString(strName),UIRes);
    pImageset->setAutoScalingEnabled(false);
}

long   UIManager::GetImageWidth(const char *ImageSetName,const char * imageName)
{
    int iWidth = (int)CEGUI::ImagesetManager::getSingleton().get(ImageSetName).
        getImage(imageName).getSourceTextureArea().getWidth();
    return iWidth;
}

long   UIManager::GetImageHeight(const char *ImageSetName,const char * imageName)
{
    int iHeight = (int)CEGUI::ImagesetManager::getSingleton().get(ImageSetName)
        .getImage(imageName).getSourceTextureArea().getHeight();
    return iHeight;
}

//绘制图片
void   UIManager::DrawImage(int iX,int iY,const char *imageSetName,const char *image)
{
    int iWidth = GetImageWidth(imageSetName,image);
    int iHeight = GetImageHeight(imageSetName,image);
    gameLayer2D->RenderImage(iX, iY, iWidth, iHeight, imageSetName, image);
}

void   UIManager::DrawImage(int iX,int iY, int iWidth, int iHeight, const char *imageSetName,const char *image)
{
    gameLayer2D->RenderImage(iX, iY, iWidth, iHeight, imageSetName, image);
}

void   UIManager::DrawImage(CEGUI::Window *pDestWin, int iX, int iY, const char *imageSetName,const char *image)
{
    if (!pDestWin)
        return;
    int iWidth = GetImageWidth(imageSetName,image);
    int iHeight = GetImageHeight(imageSetName,image);
    gameLayer2D->RenderImage(pDestWin->getGeometryBuffer(), iX, iY, iWidth, iHeight, imageSetName, image);
}

int   UIManager::GetFontWidth(const char* text, int iStyle, float x_fscale)
{
    CEGUI::Font *pFont = CEGUI::System::getSingleton().getDefaultFont();
    int lWidth = (int)pFont->getTextExtent(text,x_fscale);
    return lWidth;
}

int    UIManager::GetFontHeight(int iStyle, float y_scale)
{
    CEGUI::Font *pFont = CEGUI::System::getSingleton().getDefaultFont();
    int lHeight = (int)pFont->getFontHeight(y_scale);
    return lHeight;
}

void UIManager::Destroy()
{
    //释放所有界面
    ReleaseAllPage();

    //释放CEGUI对象
    if( m_pSystemCEGUI != NULL)
    {
        //释放自定义渲染器
        CEGUI::WindowRendererFactory *pFactory = CEGUI::WindowRendererManager::getSingleton().getFactory("Falagard/ProgressBarEx");
        SAFE_DELETE(pFactory);
        
        // clear script module, since we're going to destroy it.
        if(m_pScriptModule)
        {
            m_pSystemCEGUI->setScriptingModule(0);
            CEGUI::LuaScriptModule::destroy(*m_pScriptModule);
        }
        m_pScriptModule = 0;

        m_pSystemCEGUI->destroy();
        m_pSystemCEGUI = NULL;
        if(m_pCeguiD3D9Render != NULL)
        {
            CEGUI::Direct3D9Renderer::destroy(*m_pCeguiD3D9Render);
            m_pCeguiD3D9Render = NULL;
        }
        // 释放字符解析对象
        SAFE_DELETE(myRenderParser);
        SAFE_DELETE(gameLayer2D);
    }
}

UIManager::~UIManager()
{

}

const char * UIManager::getDataPathPrefix()
{
    static char dataPathPrefix[500];
    char* envDataPath = 0;

    // get data path from environment var
    static const char DATAPATH_VAR_NAME[200];
    envDataPath = getenv(DATAPATH_VAR_NAME);

    // set data path prefix / base directory.  This will
    // be either from an environment variable, or from
    // a compiled in default based on original configure
    // options
    if (envDataPath != 0)
        strcpy_s<500>(dataPathPrefix, envDataPath);
    else
        strcpy_s<500>(dataPathPrefix, CEGUI_DATAPATH);

    return dataPathPrefix;
}

void UIManager::initialiseResourceGroupDirectories()
{
    // initialise the required dirs for the DefaultResourceProvider
    CEGUI::DefaultResourceProvider* rp =
        static_cast<CEGUI::DefaultResourceProvider*>
        (m_pSystemCEGUI->getResourceProvider());

    const char* dataPathPrefix = getDataPathPrefix();
    char resourcePath[255];

    // for each resource type, set a resource group directory
    sprintf_s<255>(resourcePath, "%s/%s", dataPathPrefix, "schemes/");
    rp->setResourceGroupDirectory("schemes", resourcePath);
    sprintf_s<255>(resourcePath, "%s/%s", dataPathPrefix, "imagesets/");
    rp->setResourceGroupDirectory("imagesets", resourcePath);
    sprintf_s<255>(resourcePath, "%s/%s", dataPathPrefix, "fonts/");
    rp->setResourceGroupDirectory("fonts", resourcePath);
    sprintf_s<255>(resourcePath, "%s/%s", dataPathPrefix, "layouts/");
    rp->setResourceGroupDirectory("layouts", resourcePath);
    sprintf_s<255>(resourcePath, "%s/%s", dataPathPrefix, "looknfeel/");
    rp->setResourceGroupDirectory("looknfeels", resourcePath);
    sprintf_s<255>(resourcePath, "%s/%s", dataPathPrefix, "lua_scripts/");
    rp->setResourceGroupDirectory("lua_scripts", resourcePath);
    sprintf_s<255>(resourcePath, "%s/%s", dataPathPrefix, "xml_schemas/");
    rp->setResourceGroupDirectory("schemas", resourcePath);   
}

void UIManager::initialiseDefaultResourceGroups()
{
    // set the default resource groups to be used
    CEGUI::Imageset::setDefaultResourceGroup("imagesets");
    CEGUI::Font::setDefaultResourceGroup("fonts");
    CEGUI::Scheme::setDefaultResourceGroup("schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
    CEGUI::WindowManager::setDefaultResourceGroup("layouts");
    CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

    // setup default group for validation schemas
    CEGUI::XMLParser* parser = m_pSystemCEGUI->getXMLParser();
    if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
        parser->setProperty("SchemaDefaultResourceGroup", "schemas"); 
}

bool Destroyed(const CEGUI::EventArgs &e)
{
    CEGUI::WindowManager::getSingleton().destroyWindow("Demo Window");
    return true;
}

void UIManager::SetCameraPosAndView(float XPos,float YPos,float ZPos)
{
    m_Camera.SetPosition(&D3DXVECTOR3(XPos,YPos,ZPos));
    m_Camera.SetView(&D3DXVECTOR3(0.0f,YPos,0.0f));
}

bool UIManager::ChnInjectChar(WPARAM wParam)
{
    CEGUI::utf32 code_point = (CEGUI::utf32)wParam;
#ifndef UNICODE
    static char s_tempChar[3] = "";
    static wchar_t s_tempWchar[2] = L"";
    static bool s_flag = false;
    unsigned char uch = (unsigned char)code_point;
    if( uch >= 0x81 || s_flag)
    {
        if( !s_flag )
        {
            s_tempChar[0] = (char)uch; //第一个字节
            s_flag = true;
            return true;
        }
        else
        {
            s_tempChar[1] = (char)uch; //第二个字节
            s_flag = false;
            MultiByteToWideChar( 0, 0, s_tempChar, 2, s_tempWchar, 1); //转成宽字节
            s_tempWchar[1] = L'\0';
            CEGUI::utf32 code = (CEGUI::utf32)s_tempWchar[0];
            return m_pSystemCEGUI->injectChar( code );
        }
    }
    else
    {
        s_flag = false;
        return m_pSystemCEGUI->injectChar(code_point);
    }
#else
    return m_pSystemCEGUI->injectChar(code_point );
#endif
}

UINT UIManager::VirtualKeyToScanCode(WPARAM wParam, LPARAM lParam)
{
    if(HIWORD(lParam) & 0x0F00) 
    { 
        UINT scancode = MapVirtualKey((UINT)wParam, 0); 
        return scancode | 0x80; 
    } 
    else 
    { 
        return HIWORD(lParam) & 0x00FF; 
    } 
}

bool UIManager::getFocusedInputBoxCoord(POINT& point, float& height)
{
    //寻找到有输入焦点的EditBox的左上坐标
    //遍历所有窗口
    CEGUI::WindowManager::WindowIterator wit = CEGUI::WindowManager::getSingleton().getIterator();
    while(!wit.isAtEnd())
    {
        const CEGUI::Window* widget = (*wit)->getActiveChild();
        //如果是EditBox或者MultiLineEditBox
        if(widget)
        {
            CEGUI::String windowType = widget->getType();
            if(windowType == "TaharezLook/Editbox"||
                windowType == "TaharezLook/MultiLineEditbox")								//根据具体的scheme来修改。
            {
                const CEGUI::UVector2& winPos = widget->getPosition();
                height = widget->getHitTestRect().getHeight();

                CEGUI::Vector2 winPos1 = CEGUI::CoordConverter::windowToScreen(*widget, winPos);

                point.x = (long)winPos1.d_x;
                point.y = (long)winPos1.d_y;
                return true;
            }
        }
        wit++;
    }

    return false;
}

bool UIManager::IMEFollow()
{
    //判断输入法是否打开
    if (!ImmIsIME(GetKeyboardLayout(0)))
        return false;

    //获得输入框左上坐标
    bool result;
    POINT point;
    float height;
    result = getFocusedInputBoxCoord(point, height);
    if(!result)
        return false;

    //获得客户区的坐标
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    point.x+=rect.left;
    point.y+=rect.top;

    //设置输入法位置
    HIMC hImc = ImmGetContext(g_hWnd);
    if(hImc==NULL) return false;
    COMPOSITIONFORM form;
    ImmGetCompositionWindow(hImc, &form);
    form.ptCurrentPos.x = point.x;
    form.ptCurrentPos.y = point.y;
    ImmSetCompositionWindow(hImc, &form);

    return true;	
}

void UIManager::Render()
{
    map<CEGUI::Window *,AnimEffectIcon*>::iterator itor = mapAnimIconManager.begin();
    while (itor != mapAnimIconManager.end())
    {
        AnimEffectIcon * pEffect = itor->second;
        pEffect->update();
        ++itor;
    }
    //保存原始的矩阵信息
    SaveRenderState();
    //绘制cegui
    m_pSystemCEGUI->renderGUI();

    ResetRenderState();
    /////////////////////////////////////////////////
    // zhaohang  2010/3/17 
    // 测试物品查看器渲染
    GetInst(GoodsRender).Render();
    /////////////////////////////////////////////////
}

void UIManager::CEGUI_ResetD3D()
{
    // perform ops needed prior to reset
    m_pCeguiD3D9Render->preD3DReset();
}

bool UIManager::IOProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    bool res = false;
    CEGUI::utf32 code;
    char coder = ' ';
    switch (message)
    {
    case WM_CHAR:
        code = (CEGUI::utf32)wParam;
        if (ImmIsIME(GetKeyboardLayout(0)))
            res = ChnInjectChar(code);
        else
            res = m_pSystemCEGUI->injectChar(code);
        break;
    case WM_KEYDOWN:
        {
            // 输入法跟随
            IMEFollow();
            // 输入法状态时，输入不传递到UI系统中去。
            UINT vk = ImmGetVirtualKey(g_hWnd); 
            if(vk == wParam)
                break;
            code = VirtualKeyToScanCode(wParam, lParam);
            res = m_pSystemCEGUI->injectKeyDown(code);
            
            if(GetInputsCapturedWin())
                res = true;

            switch (wParam)
            {
            case VK_ESCAPE:
                if (CVideoManager::GetInstance()->GetPlayThreadCount())
                    CVideoManager::GetInstance()->StopVideo();
                break;
            case VK_SHIFT:
                //调试代码 应删除
                {
                    char uiName[128] = "";
                    sprintf_s<128>(uiName,"MainBarPage/StaticImage%d/CoolImage%d",0,0);
					if(CEGUI::WindowManager::getSingleton().isWindowPresent(uiName))
					{
						CEGUI::Window *pCoolUI = CEGUI::WindowManager::getSingleton().getWindow(uiName);
						UIManager::GetAnimIcon(pCoolUI)->BeginAnimateEffect(5000);
					}
                }
                break;
            };
            if (!res)
            {
                extern BYTE g_cPressedKey;
                g_cPressedKey = (BYTE)wParam;
            }
        }
        break;

    case WM_KEYUP:
        code = VirtualKeyToScanCode(wParam, lParam);
        res = m_pSystemCEGUI->injectKeyUp(code);
        break;

    case WM_MOUSEMOVE:
        res = m_pSystemCEGUI->injectMousePosition((float)(LOWORD(lParam)), (float)(HIWORD(lParam)));
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        {
            res = m_pSystemCEGUI->injectMouseButtonDown(CEGUI::LeftButton);
            CEGUI::Window *pClickWin = m_pSystemCEGUI->getWindowContainingMouse();
            if (pClickWin)
            {
                CEGUI::String name = pClickWin->getName();
                // 如果点击聊天列表 需要响应角色操作。
                if (name == "ChatList" || name == "Root") 
                    res = false;
                else
                    res = true;
            }
        }
        break;

    case WM_LBUTTONUP:
        {
            res = m_pSystemCEGUI->injectMouseButtonUp(CEGUI::LeftButton);
            CEGUI::Window* pClickWin = m_pSystemCEGUI->getWindowContainingMouse();
            if (pClickWin)
            {
                CEGUI::String name = pClickWin->getName();
                // 如果点击聊天列表 需要响应角色操作。
                if (name == "ChatList" || name == "Root") 
                    res = false;
            }
        }
        break;

    case WM_RBUTTONDOWN:
        m_pSystemCEGUI->injectMouseButtonDown(CEGUI::RightButton);
        break;

    case WM_RBUTTONUP:
        m_pSystemCEGUI->injectMouseButtonUp(CEGUI::RightButton);
        break;

    case WM_MBUTTONDOWN:
        res = m_pSystemCEGUI->injectMouseButtonDown(CEGUI::MiddleButton);
        break;

    case WM_MBUTTONUP:
        res = m_pSystemCEGUI->injectMouseButtonUp(CEGUI::MiddleButton);
        break;

    case WM_MOUSEWHEEL: // WM_MOUSEWHEEL:
        m_pSystemCEGUI->injectMouseWheelChange(static_cast<float>((short)HIWORD(wParam)) / static_cast<float>(120));
        break;
    }

    return res;
}

void UIManager::UpDateCEGUISystemTime()
{
    static DWORD d_lastFrameTime = GetCurTickCount();
    DWORD thisTime = GetTickCount();
    float elapsed = static_cast<float>(thisTime - d_lastFrameTime);
    d_lastFrameTime = thisTime;
    // inject the time pulse
    m_pSystemCEGUI->injectTimePulse(elapsed / 1000.0f);
}

void UIManager::ReleaseAllPage()
{
    //清除root下所有子窗口
    CEGUI::Window *pRootWin = CEGUI::WindowManager::getSingleton().getWindow("Root");

    while (pRootWin ->getChildCount() != 0 )
    {
#ifdef _DEBUG
        size_t count = pRootWin->getChildCount();
#endif
        CEGUI::Window *pPageWnd = pRootWin->getChildAtIdx(0);
        if( pPageWnd->getName() == "CEGUI::System::default__auto_tooltip__")
            break;
        ReleasePage(pPageWnd);
    }
    if (mapPageManager.size()!=0)
    {
        MessageBox(NULL,"释放失败，请检查配置文件名与界面窗口名是否一致！","错误",MB_OK);
    }
    //释放所有3DICON
    ReleaseAll3DIcon();
    //释放所有AnimIcon
    ReleaseAllAnimIcon();
}
void UIManager::ReleasePage(CEGUI::Window *pWnd)
{
    if(!pWnd) return;
    const char* name = NULL;
    //如果管理器里有这个界面就先清除 否则直接清除
    CEGUI::String wndName = pWnd->getName();
#ifdef _DEBUG
    name = wndName.c_str();
#endif
    CEGUI::String layoutName = wndName + ".layout";
    map<string , GamePage *>::iterator IterWnd =
        mapPageManager.find(layoutName.c_str());
    if (IterWnd != mapPageManager.end())
    {
        CEGUI::WindowManager::getSingleton().destroyWindow(wndName);
        IterWnd->second->ReleasePage();
        IterWnd->second->SetPageWindow(NULL);
        mapPageManager.erase(IterWnd);
    }
    else
    {
        pWnd->destroy();
    }
    m_UserEditWin = NULL;
}

void UIManager::ReleasePage(const char * name)
{
    if (CEGUI::WindowManager::getSingleton().isWindowPresent(name))
    {
        CEGUI::Window *pWnd = CEGUI::WindowManager::getSingleton().getWindow(name);

        ReleasePage(pWnd);
    }
}

void UIManager::AddGamePage(GamePage *page)
{
    if (page)
    {
        GamePage *pPage = mapPageManager[page->GetLayoutFileName().c_str()];
        if (pPage == NULL)
        {
            mapPageManager[page->GetLayoutFileName().c_str()] = page;
        }
    }
}

void UIManager::CloseAllPage(const char *exceptName)
{
    CEGUI::Window *pRootWin = CEGUI::WindowManager::getSingleton().getWindow("Root");
    size_t count = pRootWin->getChildCount();
    for (size_t i = 0; i<count; i++)
    {
        CEGUI::Window *pChild = pRootWin->getChildAtIdx(i);
        if(exceptName && pChild->getName().compare(exceptName) == 0)
            continue;
        pChild->setVisible(false);
    }
}

void UIManager::SaveRenderState()
{
    render::Interface *pInterface = render::Interface::GetInstance();
    LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
    if(pDevice)
    {
        pDevice->GetFVF(&m_FVF);
        pDevice->GetVertexShader(&m_VertexShader);
        pDevice->GetPixelShader(&m_PixelShader);

        pDevice->GetRenderState(D3DRS_LIGHTING,&m_LIGHTING);
        pDevice->GetRenderState(D3DRS_FOGENABLE,&m_FOGENABLE);
        pDevice->GetRenderState(D3DRS_ZENABLE,&m_ZENABLE);
        pDevice->GetRenderState(D3DRS_ALPHATESTENABLE,&m_ALPHATESTENABLE);
        pDevice->GetRenderState(D3DRS_CULLMODE,&m_CULLMODE);
        pDevice->GetRenderState(D3DRS_FILLMODE,&m_FILLMODE);
        pDevice->GetRenderState(D3DRS_SCISSORTESTENABLE,&m_SCISSORTESTENABLE);
        pDevice->GetRenderState(D3DRS_ZWRITEENABLE,&m_ZWRITENABLE);

        pDevice->GetSamplerState(0,D3DSAMP_ADDRESSU,&m_SAMPADDRESSU);
        pDevice->GetSamplerState(0,D3DSAMP_ADDRESSV,&m_SAMPADDRESSV);

        pDevice->GetTextureStageState(0,D3DTSS_COLORARG1,&m_COLORARG1);
        pDevice->GetTextureStageState(0,D3DTSS_COLORARG2,&m_COLORARG2);
        pDevice->GetTextureStageState(0,D3DTSS_COLOROP,&m_COLOROP);

        pDevice->GetTextureStageState(0,D3DTSS_ALPHAARG1,&m_ALPHAARG1);
        pDevice->GetTextureStageState(0,D3DTSS_ALPHAARG2,&m_ALPHAARG2);
        pDevice->GetTextureStageState(0,D3DTSS_ALPHAOP,&m_ALPHAOP);

        pDevice->GetSamplerState(0,D3DSAMP_MINFILTER,&m_MINFILTER);
        pDevice->GetSamplerState(0,D3DSAMP_MAGFILTER,&m_MAGFILTER);

        pDevice->GetTextureStageState(1,D3DTSS_COLOROP,&m_TSS1_COLOROP);

        pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&m_RS_ALPHABLENDENABLE);
        pDevice->GetRenderState(D3DRS_SRCBLEND,&m_RS_SRCBLEND);
        pDevice->GetRenderState(D3DRS_DESTBLEND,&m_RS_DESTBLEND);

        pDevice->GetRenderState(D3DRS_SRCBLENDALPHA,&m_RS_SRCBLENDALPHA);
        pDevice->GetRenderState(D3DRS_DESTBLENDALPHA,&m_RS_DESTBLENDALPHA);
    }
}

void UIManager::ResetRenderState()
{
    render::Interface *pInterface = render::Interface::GetInstance();
    LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
    if(pDevice)
    {
        pDevice->SetFVF(m_FVF);

        pDevice->SetVertexShader(m_VertexShader);
        pDevice->SetPixelShader(m_PixelShader);

        pDevice->SetRenderState(D3DRS_LIGHTING,m_LIGHTING);
        pDevice->SetRenderState(D3DRS_FOGENABLE,m_FOGENABLE);
        pDevice->SetRenderState(D3DRS_ZENABLE,m_ZENABLE);
        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,m_ALPHATESTENABLE);
        pDevice->SetRenderState(D3DRS_CULLMODE,m_CULLMODE);
        pDevice->SetRenderState(D3DRS_FILLMODE,m_FILLMODE);
        pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,m_SCISSORTESTENABLE);
        pDevice->SetRenderState(D3DRS_ZWRITEENABLE,m_ZWRITENABLE);

        pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,m_SAMPADDRESSU);
        pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,m_SAMPADDRESSV);

        pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,m_COLORARG1);
        pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,m_COLORARG2);
        pDevice->SetTextureStageState(0,D3DTSS_COLOROP,m_COLOROP);

        pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,m_ALPHAARG1);
        pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,m_ALPHAARG2);
        pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,m_ALPHAOP);

        pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,m_MINFILTER);
        pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,m_MAGFILTER);

        pDevice->SetTextureStageState(1,D3DTSS_COLOROP,m_TSS1_COLOROP);

        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,m_RS_ALPHABLENDENABLE);
        pDevice->SetRenderState(D3DRS_SRCBLEND,m_RS_SRCBLEND);
        pDevice->SetRenderState(D3DRS_DESTBLEND,m_RS_DESTBLEND);

        pDevice->SetRenderState(D3DRS_SRCBLENDALPHA,m_RS_SRCBLENDALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLENDALPHA,m_RS_DESTBLENDALPHA);
    }
}



void UIManager::FreshCurEditComponent()
{
    m_UserEditWin = m_pSystemCEGUI->getWindowContainingMouse();
}

void UIManager::EditUI()
{
    static char strTEXT[256] = "";
    CEGUI::Window *pWin = m_UserEditWin;
    sprintf_s<256>(strTEXT,"CEGUI编辑状态!");
    OutPutText((SCREEN_WIDTH - GetFontWidth(strTEXT)) / 2,10,strTEXT, D3DCOLOR_ARGB(255,255,0,0));
    if (pWin && pWin->getName() != "Root")
    {
        pWin->setMinSize(CEGUI::UVector2(CEGUI::UDim(0,0),CEGUI::UDim(0,0)));
        pWin->setMaxSize(CEGUI::UVector2(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
        float xPos   = CEGUI::CoordConverter::windowToScreenX(*pWin,0) ;
        float yPos   = CEGUI::CoordConverter::windowToScreenY(*pWin,0) ;

        float width    = pWin->getUnclippedRect(true).getWidth();
        float height   = pWin->getUnclippedRect(true).getHeight();

        const char *strWinName = pWin->getName().c_str();
        sprintf_s(strTEXT,"WindowName:%s X:%0.2f Y:%.2f width:%.2f height:%.2f",strWinName, xPos,yPos,width,height);
        GetGame()->OutPutText((SCREEN_WIDTH - GetFontWidth(strTEXT))/2,80,strTEXT,(int)strlen(strTEXT),
                              D3DCOLOR_ARGB(255, 255, 255, 0));
        //按下左按钮
        if (GetKeyboard()->GetCurrentKey()==DIK_LEFT)
        {
            //按下ctrl+← 控件缩短
            if(GetKeyboard()->IsKeyDown(DIK_LCONTROL))
                pWin->setWidth(pWin->getWidth() - CEGUI::UDim(0,1));
            else
                pWin->setXPosition(pWin->getXPosition() - CEGUI::UDim(0,1));
        }
        else if(GetKeyboard()->GetCurrentKey()==DIK_RIGHT)
        {
            if(GetKeyboard()->IsKeyDown(DIK_LCONTROL))
                pWin->setWidth(pWin->getWidth() + CEGUI::UDim(0,1));
            else
                pWin->setXPosition(pWin->getXPosition() + CEGUI::UDim(0,1));
        }
        else if(GetKeyboard()->GetCurrentKey()==DIK_UPARROW)
        {
            //按下ctrl+↑ 减小高度值
            if(GetKeyboard()->IsKeyDown(DIK_LCONTROL))
                pWin->setHeight(pWin->getHeight() - CEGUI::UDim(0,1));
            else
                pWin->setYPosition(pWin->getYPosition() - CEGUI::UDim(0,1));

            //按下左shift则向里一层找父容器
            if (GetKeyboard()->IsKeyDown(DIK_LSHIFT))
            {
                CEGUI::Window *pParent = pWin->getParent();
                if (pParent->getName() == "Root")
                    return;
                else
                    m_UserEditWin = pParent;
            } 

        }
        else if(GetKeyboard()->GetCurrentKey()==DIK_DOWNARROW)
        {
            //按下ctrl+↓ 增加高度值
            if (GetKeyboard()->IsKeyDown(DIK_LCONTROL))
                pWin->setHeight(pWin->getHeight() + CEGUI::UDim(0,1));
            else
                pWin->setYPosition(pWin->getYPosition() + CEGUI::UDim(0,1));
        }
    }
}

void UIManager::Add3DIconEffect(CEGUI::Window *pWin,C3DIconEffect *Icon)
{
    map3DIconManager[pWin] = Icon;
}

C3DIconEffect* UIManager::Get3DIconEffect(CEGUI::Window *pWnd)
{
    map<CEGUI::Window *,C3DIconEffect*>::iterator itor = map3DIconManager.find(pWnd);
    if (itor != map3DIconManager.end())
    {
        return itor->second;
    }
    else
        return NULL;
}
void UIManager::ReleaseAll3DIcon()
{
    map<CEGUI::Window *,C3DIconEffect*>::iterator itor = map3DIconManager.begin();
    while (itor != map3DIconManager.end())
    {
        C3DIconEffect * pEffect = itor->second;
        SAFE_DELETE(pEffect);
        itor = map3DIconManager.erase(itor);
    }
}

void UIManager::AddAnimIcon(CEGUI::Window *pWnd,AnimEffectIcon *Icon)
{
    mapAnimIconManager[pWnd] = Icon;
}

AnimEffectIcon * UIManager::GetAnimIcon(CEGUI::Window *pWnd)
{
    map<CEGUI::Window *,AnimEffectIcon*>::iterator itor = mapAnimIconManager.find(pWnd);
    if (itor != mapAnimIconManager.end())
    {
        return itor->second;
    }
    else
        return NULL;
}

void UIManager::ReleaseAllAnimIcon()
{
    map<CEGUI::Window *,AnimEffectIcon*>::iterator itor = mapAnimIconManager.begin();
    while (itor != mapAnimIconManager.end())
    {
        AnimEffectIcon * pEffect = itor->second;
        SAFE_DELETE(pEffect);
        itor = mapAnimIconManager.erase(itor);
    }
}

void UIManager::ShowUI(bool b)
{
    CEGUI::System::getSingleton().getGUISheet()->setVisible(b);
}

void UIManager::ShowEditColorArea()
{
    if(!m_UserEditWin) 
        return;
    if(!m_UserEditWin->isVisible())
        return;
    float xPos   = CEGUI::CoordConverter::windowToScreenX(*m_UserEditWin,0) ;
    float yPos   = CEGUI::CoordConverter::windowToScreenY(*m_UserEditWin,0) ;
    float width    = m_UserEditWin->getUnclippedRect(true).getWidth();
    float height   = m_UserEditWin->getUnclippedRect(true).getHeight();
    
    GetGame()->DrawSolidQuad((int)xPos,(int)yPos,(int)width,(int)height,D3DCOLOR_ARGB(100,255,0,0));
}

void  UIManager::Run()
{
    UpDateCEGUISystemTime();
    if(GetKeyboard()->IsKeyDown(DIK_LCONTROL))
    {
        if(GetKeyboard()->GetCurrentKey() == DIK_C)
            ProcessCopy();
        if(GetKeyboard()->GetCurrentKey() == DIK_V)
            ProcessPaste();
        if(GetKeyboard()->GetCurrentKey() == DIK_X)
            ProcessClip();
    }
    else if(GetKeyboard()->GetCurrentKey() == DIK_TAB)
    {
        ProcessTab();
    }
}

CEGUI::Window * UIManager::GetInputsCapturedWin()
{
    CEGUI::Window *pEdit = CEGUI::System::getSingleton().getGUISheet()->getActiveChild();
    if(pEdit)
    {
        CEGUI::String windowType = pEdit->getType();
        if(windowType == "TaharezLook/Editbox"||
            windowType == "TaharezLook/MultiLineEditbox")								//根据具体的scheme来修改。
        {
            return pEdit;
        }
    }
    return NULL;
}
void UIManager::ProcessCopy()
{
    const CEGUI::Window *pEdit = GetInputsCapturedWin();
    if(pEdit == NULL)   
        return;
    //const char *AnsiName = CEGUIStringToAnsiChar(pEdit->getName());
    long lStartIndex = 0,lEndIndex = 0;
    CEGUI::String type = pEdit->getType();
    if(type == "TaharezLook/Editbox")
    {
        lStartIndex = (long)((CEGUI::Editbox*)pEdit)->getSelectionStartIndex();	
        lEndIndex   = (long)((CEGUI::Editbox*)pEdit)->getSelectionEndIndex();
    }
    else if (type == "TaharezLook/MultiLineEditbox")
    {
        lStartIndex = (long)((CEGUI::MultiLineEditbox*)pEdit)->getSelectionStartIndex();	
        lEndIndex   = (long)((CEGUI::MultiLineEditbox*)pEdit)->getSelectionEndIndex();
    }
    const char *text = CEGUIStringToAnsiChar(pEdit->getTextVisual().substr(lStartIndex,lEndIndex - lStartIndex));
    if(!text && strlen(text) == 0)
        return;
    if (!OpenClipboard(0))
        return;
    EmptyClipboard();
    HGLOBAL clipbuff;
    char * buffer;
    clipbuff = GlobalAlloc(GMEM_DDESHARE,strlen(text)+1);
    buffer = (char*)GlobalLock(clipbuff);
    memcpy(buffer,text,strlen(text));
    GlobalUnlock(clipbuff);
    SetClipboardData(CF_TEXT,clipbuff);
    CloseClipboard();
}

void UIManager::ProcessPaste()
{
    if(!OpenClipboard(NULL))
        return;
    char *buffer = 0;
    HANDLE hData = GetClipboardData(CF_TEXT);
    buffer = (char*)GlobalLock(hData);
    GlobalUnlock(hData);
    CloseClipboard();
    if(!buffer)
        return;
    
    CEGUI::String strClipBoard = ToCEGUIString(buffer);
    CEGUI::Window *pEdit = GetInputsCapturedWin();
    if(pEdit == NULL)   
        return;

    long lStartIndex = 0,caratPos = 0;
    CEGUI::String type = pEdit->getType();
    if(type == "TaharezLook/Editbox")
    {
        lStartIndex = (long)((CEGUI::Editbox*)pEdit)->getSelectionStartIndex();	
        caratPos    = (long)((CEGUI::Editbox*)pEdit)->getCaratIndex();
    }
    else if (type == "TaharezLook/MultiLineEditbox")
    {
        lStartIndex = (long)((CEGUI::MultiLineEditbox*)pEdit)->getSelectionStartIndex();
        caratPos    = (long)((CEGUI::MultiLineEditbox*)pEdit)->getCaratIndex();
    }
    pEdit->insertText(strClipBoard,lStartIndex);
    long llength = (long)strClipBoard.length();
    if(type == "TaharezLook/Editbox")
        ((CEGUI::Editbox*)pEdit)->setCaratIndex(caratPos + llength);
    else if(type == "TaharezLook/MultiLineEditbox")
        ((CEGUI::MultiLineEditbox*)pEdit)->setCaratIndex(caratPos + llength);
}

void UIManager::ProcessClip()
{
    ProcessCopy();
    CEGUI::Window *pEdit = GetInputsCapturedWin();
    if(pEdit == NULL)   
        return;

    long lStartIndex = 0,lEndIndex = 0;
    CEGUI::String type = pEdit->getType();
    if(type == "TaharezLook/Editbox")
    {
        lStartIndex = (long)((CEGUI::Editbox*)pEdit)->getSelectionStartIndex();	
        lEndIndex   = (long)((CEGUI::Editbox*)pEdit)->getSelectionEndIndex();
    }
    else if (type == "TaharezLook/MultiLineEditbox")
    {
        lStartIndex = (long)((CEGUI::MultiLineEditbox*)pEdit)->getSelectionStartIndex();
        lEndIndex    = (long)((CEGUI::MultiLineEditbox*)pEdit)->getSelectionEndIndex();
    }
    CEGUI::String content = pEdit->getTextVisual();
    content.replace(lStartIndex, lEndIndex - lStartIndex, "");
    pEdit->setText(content);
    if(type == "TaharezLook/Editbox")
        ((CEGUI::Editbox*)pEdit)->setCaratIndex(lStartIndex);
    else if (type == "TaharezLook/MultiLineEditbox")
        ((CEGUI::MultiLineEditbox*)pEdit)->setCaratIndex(lStartIndex);
}

void UIManager::ProcessTab()
{
    Window *pEdit = GetInputsCapturedWin();
    if (!pEdit)
        return;
    Window *pParent = pEdit->getParent();
    if(!pParent)
        return;
    for (unsigned int i = 0;i < pParent->getChildCount();i++)
    {
        Window *pChild = pParent->getChildAtIdx(i);
        if ((pChild->getType() == "TaharezLook/Editbox" ||
            pChild->getType() == "TaharezLook/MultiLineEditbox")
            && pChild != pEdit)
        {
            pChild->activate();
            break;
        }
    }
}

//当前鼠标是否在ui 窗口内, root窗口必须排除
bool UIManager::isMouseRollOverWindow()
{
	CEGUI::Window *win = m_pSystemCEGUI->getWindowContainingMouse();
	return win != NULL && win->getName() != "Root";
}

MyRenderedStringParser* UIManager::GetMyRenderedStringParser()
{
	return myRenderParser;
}