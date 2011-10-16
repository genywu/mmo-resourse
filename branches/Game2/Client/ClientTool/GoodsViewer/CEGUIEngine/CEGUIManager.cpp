#include "stdafx.h"
#include <tchar.h>

#include "BaseUIClass.h"
#include "CEGUIManager.h"
#include "../GoodsViewer/ui/ControlGoodsView.h"
#include "../GoodsViewer/ui/GoodsTree.h"
#pragma warning(disable : 4996)

IMPLEMENT_SINGLETON(CEGUIManager);

using namespace UIClass;
//CEGUIManager::UIClassList CEGUIManager::m_UIClassList = 0; 

//没增加一个新的UI类，就要在这里注册
void CEGUIManager::RegisterUIClassInList()
{
	m_UIClassList["ControlGoodsView"] = new UIClass::ControlGoodsView;
	m_UIClassList["GoodsTree"]	  = new UIClass::GoodsTree;
}

CEGUIManager::CEGUIManager()
{
	m_RenderCEGUI = NULL;
	m_SysCEGUI    = NULL;
	m_FPS_geometry = NULL;
	m_RootWindow  = NULL;
	m_LastFrameTime = GetTickCount();
	m_FPS_LastTime = m_LastFrameTime;
	m_FPS_Frames = 0;
	m_FPS_Value  = 0;

	m_ShowFPS   = true;
}

CEGUIManager::~CEGUIManager()
{
	
}

bool CEGUIManager::InitCEGUI(LPDIRECT3DDEVICE9 pd3ddevice)
{
	if(!pd3ddevice)
		return false;
	m_d3ddevice = pd3ddevice;
	m_RenderCEGUI	=	&CEGUI::Direct3D9Renderer::create(pd3ddevice);
	if(m_RenderCEGUI)
		m_SysCEGUI	=	&CEGUI::System::create(*m_RenderCEGUI);
	if(m_RenderCEGUI && m_SysCEGUI)
	{
		//创建根窗口
		m_RootWindow = CreateRootWnd();
		//Set the GUI root window
		m_SysCEGUI->setGUISheet(m_RootWindow);

		//初始化脚本系统
		CEGUI::LuaScriptModule &scriptmod(LuaScriptModule::create());
		m_SysCEGUI->setScriptingModule(&scriptmod);
		//初始化帧数显示部分
		const CEGUI::Rect scrn(CEGUI::Vector2(0,0),m_RenderCEGUI->getDisplaySize());
		m_FPS_geometry = &m_RenderCEGUI->createGeometryBuffer();
		m_FPS_geometry->setClippingRegion(scrn);
		m_RenderCEGUI->getDefaultRenderingRoot().clearGeometry(CEGUI::RQ_OVERLAY);

		//注册处理帧数显示的事件
		m_RenderCEGUI->getDefaultRenderingRoot().subscribeEvent(
			CEGUI::RenderingSurface::EventRenderQueueStarted,
			CEGUI::Event::Subscriber(&CEGUIManager::OverlayHandler,this));

		//完成后返回true
		return true;
	}
	else
		return false;
}

void CEGUIManager::ReleaseCEGUI()
{
	if(m_RootWindow)
	{
		m_RootWindow = NULL;//窗口内存将会在CEGUISystem的Destory中自动释放
	}
	if(m_SysCEGUI)
	{
		LuaScriptModule* ScriptMod = static_cast<LuaScriptModule*>(m_SysCEGUI->getScriptingModule());
		m_SysCEGUI->setScriptingModule(0);
		LuaScriptModule::destroy(*ScriptMod);
		if(m_UIClassList.empty()==false)
		{
			UIClassList::iterator iter = m_UIClassList.begin();
			for( ;iter != m_UIClassList.end() ; ++iter)
			{
				if( iter->second != NULL)
				{
					delete iter->second;iter->second = NULL;
				}
			}
			m_UIClassList.clear();
		}
		m_SysCEGUI->destroy();	m_SysCEGUI = NULL;
		if(m_RenderCEGUI)
		{
			Direct3D9Renderer::destroy(*m_RenderCEGUI);
			m_RenderCEGUI	=	NULL;
		}
	}
}

CEGUI::Window* CEGUIManager::CreateRootWnd()
{
	return CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","Root");
}


UIClass::BaseUIClass* CEGUIManager::GetUIClass(const CEGUI::String ClassName)
{
	UIClassList::iterator iter = m_UIClassList.find(ClassName);
	if(iter != m_UIClassList.end())
	{
		return iter->second;
	}
	return NULL;
}

void CEGUIManager::SaveRenderState()
{
	if(m_d3ddevice)
	{
		m_d3ddevice->GetFVF(&m_FVF);
		m_d3ddevice->GetVertexShader(&m_VertexShader);
		m_d3ddevice->GetPixelShader(&m_PixelShader);

		m_d3ddevice->GetRenderState(D3DRS_LIGHTING,&m_LIGHTING);
		m_d3ddevice->GetRenderState(D3DRS_FOGENABLE,&m_FOGENABLE);
		m_d3ddevice->GetRenderState(D3DRS_ZENABLE,&m_ZENABLE);
		m_d3ddevice->GetRenderState(D3DRS_ALPHATESTENABLE,&m_ALPHATESTENABLE);
		m_d3ddevice->GetRenderState(D3DRS_CULLMODE,&m_CULLMODE);
		m_d3ddevice->GetRenderState(D3DRS_FILLMODE,&m_FILLMODE);
		m_d3ddevice->GetRenderState(D3DRS_SCISSORTESTENABLE,&m_SCISSORTESTENABLE);
		m_d3ddevice->GetRenderState(D3DRS_ZWRITEENABLE,&m_ZWRITENABLE);

		m_d3ddevice->GetSamplerState(0,D3DSAMP_ADDRESSU,&m_SAMPADDRESSU);
		m_d3ddevice->GetSamplerState(0,D3DSAMP_ADDRESSV,&m_SAMPADDRESSV);

		m_d3ddevice->GetTextureStageState(0,D3DTSS_COLORARG1,&m_COLORARG1);
		m_d3ddevice->GetTextureStageState(0,D3DTSS_COLORARG2,&m_COLORARG2);
		m_d3ddevice->GetTextureStageState(0,D3DTSS_COLOROP,&m_COLOROP);

		m_d3ddevice->GetTextureStageState(0,D3DTSS_ALPHAARG1,&m_ALPHAARG1);
		m_d3ddevice->GetTextureStageState(0,D3DTSS_ALPHAARG2,&m_ALPHAARG2);
		m_d3ddevice->GetTextureStageState(0,D3DTSS_ALPHAOP,&m_ALPHAOP);

		m_d3ddevice->GetSamplerState(0,D3DSAMP_MINFILTER,&m_MINFILTER);
		m_d3ddevice->GetSamplerState(0,D3DSAMP_MAGFILTER,&m_MAGFILTER);

		m_d3ddevice->GetTextureStageState(1,D3DTSS_COLOROP,&m_TSS1_COLOROP);

		m_d3ddevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&m_RS_ALPHABLENDENABLE);
		m_d3ddevice->GetRenderState(D3DRS_SRCBLEND,&m_RS_SRCBLEND);
		m_d3ddevice->GetRenderState(D3DRS_DESTBLEND,&m_RS_DESTBLEND);
	}
}

void CEGUIManager::ResetRenderState()
{
	if(m_d3ddevice)
	{
		m_d3ddevice->SetFVF(m_FVF);

		m_d3ddevice->SetVertexShader(m_VertexShader);
		m_d3ddevice->SetPixelShader(m_PixelShader);

		m_d3ddevice->SetRenderState(D3DRS_LIGHTING,m_LIGHTING);
		m_d3ddevice->SetRenderState(D3DRS_FOGENABLE,m_FOGENABLE);
		m_d3ddevice->SetRenderState(D3DRS_ZENABLE,m_ZENABLE);
		m_d3ddevice->SetRenderState(D3DRS_ALPHATESTENABLE,m_ALPHATESTENABLE);
		m_d3ddevice->SetRenderState(D3DRS_CULLMODE,m_CULLMODE);
		m_d3ddevice->SetRenderState(D3DRS_FILLMODE,m_FILLMODE);
		m_d3ddevice->SetRenderState(D3DRS_SCISSORTESTENABLE,m_SCISSORTESTENABLE);
		m_d3ddevice->SetRenderState(D3DRS_ZWRITEENABLE,m_ZWRITENABLE);

		m_d3ddevice->SetSamplerState(0,D3DSAMP_ADDRESSU,m_SAMPADDRESSU);
		m_d3ddevice->SetSamplerState(0,D3DSAMP_ADDRESSV,m_SAMPADDRESSV);

		m_d3ddevice->SetTextureStageState(0,D3DTSS_COLORARG1,m_COLORARG1);
		m_d3ddevice->SetTextureStageState(0,D3DTSS_COLORARG2,m_COLORARG2);
		m_d3ddevice->SetTextureStageState(0,D3DTSS_COLOROP,m_COLOROP);

		m_d3ddevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,m_ALPHAARG1);
		m_d3ddevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,m_ALPHAARG2);
		m_d3ddevice->SetTextureStageState(0,D3DTSS_ALPHAOP,m_ALPHAOP);

		m_d3ddevice->SetSamplerState(0,D3DSAMP_MINFILTER,m_MINFILTER);
		m_d3ddevice->SetSamplerState(0,D3DSAMP_MAGFILTER,m_MAGFILTER);

		m_d3ddevice->SetTextureStageState(1,D3DTSS_COLOROP,m_TSS1_COLOROP);

		m_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE,m_RS_ALPHABLENDENABLE);
		m_d3ddevice->SetRenderState(D3DRS_SRCBLEND,m_RS_SRCBLEND);
		m_d3ddevice->SetRenderState(D3DRS_DESTBLEND,m_RS_DESTBLEND);
	}
}
void CEGUIManager::InitialiseDefaultResourceGroups()
{
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	CEGUI::XMLParser *parser	=	CEGUI::System::getSingleton().getXMLParser();
	if(parser->isPropertyPresent("SchemaDefaultResourceGroup"))
		parser->setProperty("SchemaDefaultResourceGroup","schemas");

	//创键字体
	//FontManager::getSingleton().create("Commonwealth-10.font");
	//FontManager::getSingleton().create("Chinese.font");

	//加载scheme文件
	SchemeManager::getSingleton().create("TaharezLook.scheme");
	//设置鼠标图标
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook","MouseArrow");
}

void CEGUIManager::InitiliseResourceDirectories(const char *pPath/* = "../datafiles"*/)
{
	CEGUI::DefaultResourceProvider *rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
	if(pPath)
	{
		char resourcePath[MAX_PATH];
		
		sprintf(resourcePath,"%s/%s",pPath,"schemes/");
		rp->setResourceGroupDirectory("schemes",resourcePath);

		sprintf(resourcePath,"%s/%s",pPath,"imagesets/");
		rp->setResourceGroupDirectory("imagesets",resourcePath);

		sprintf(resourcePath,"%s/%s",pPath,"fonts/");
		rp->setResourceGroupDirectory("fonts",resourcePath);

		sprintf(resourcePath,"%s/%s",pPath,"layouts/");
		rp->setResourceGroupDirectory("layouts",resourcePath);

		sprintf(resourcePath,"%s/%s",pPath,"looknfeel/");
		rp->setResourceGroupDirectory("looknfeels",resourcePath);

		sprintf(resourcePath,"%s/%s",pPath,"lua_scripts/");
		rp->setResourceGroupDirectory("lua_scripts",resourcePath);

		sprintf(resourcePath,"%s/%s",pPath,"xml_schemas/");
		rp->setResourceGroupDirectory("xml_schemas",resourcePath);
	}
}

void CEGUIManager::RenderCEGUI()
{
	SaveRenderState();
	if(m_RenderCEGUI && m_SysCEGUI)
		m_SysCEGUI->renderGUI();
	ResetRenderState();
}

void CEGUIManager::DoUpdateFPS()
{
	++m_FPS_Frames;

	DWORD thisTime = GetTickCount();

	if(thisTime - m_FPS_LastTime >= 1000)
	{
		sprintf(m_FPS_TextBuffer,"FPS:  %d",m_FPS_Frames);
		m_FPS_Value = m_FPS_Frames;
		m_FPS_Frames = 0;
		m_FPS_LastTime = thisTime;
	}
}

void CEGUIManager::DoUpdateTime()
{
	DWORD thisTime = GetTickCount();
	float elapsed = static_cast<float>(thisTime - m_LastFrameTime);
	m_LastFrameTime = thisTime;
	//inject the time pulse
	m_SysCEGUI->injectTimePulse(elapsed / 1000.0f);
}

bool CEGUIManager::OverlayHandler(const CEGUI::EventArgs &args)
{
	if(static_cast<const RenderQueueEventArgs&>(args).queueID != RQ_OVERLAY)
		return false;
	//显示帧数
	if(m_ShowFPS)
	{
		Font *fnt = System::getSingleton().getDefaultFont();
		if(fnt)
		{
			m_FPS_geometry->reset();
			fnt->drawText(*m_FPS_geometry,m_FPS_TextBuffer,Vector2(0,5),0,colour(0xFFFFFFFF));
			m_FPS_geometry->draw();
		}
	}
	return true;
}

void CEGUIManager::ExecuteScriptFile(const char *scriptFile)
{
	if(!scriptFile)
		return;
	m_SysCEGUI->executeScriptFile(scriptFile);
}

//void CEGUIManager::ExecuteClassConstruct(const CEGUI::String ClassName)
//{
//
//}

bool CEGUIManager::InitialiseDirectInput(HWND hWnd,DirectInputState& dis)
{
	if(SUCCEEDED(DirectInput8Create(GetModuleHandle(0),DIRECTINPUT_VERSION,IID_IDirectInput8,(LPVOID*)&dis.directInput,0)))
	{
		if(SUCCEEDED(dis.directInput->CreateDevice(GUID_SysKeyboard,&dis.KeyboardDevice,0)))
		{
			if(SUCCEEDED(dis.KeyboardDevice->SetDataFormat(&c_dfDIKeyboard)))
			{
				DIPROPDWORD inputProp;
				if(SUCCEEDED(dis.KeyboardDevice->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
				{
					//the header
					inputProp.diph.dwSize	= sizeof(DIPROPDWORD);
					inputProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
					inputProp.diph.dwObj	= 0;
					inputProp.diph.dwHow	= DIPH_DEVICE;
					inputProp.dwData		= 16;
				}
				if(SUCCEEDED(dis.KeyboardDevice->SetProperty(DIPROP_BUFFERSIZE,&inputProp.diph)))
				{
					dis.KeyboardDevice->Acquire();
					return true;
				}
				else
				{
					MessageBox(0,_TEXT("Failed to set buffer size for keyboard device,"),"Error",MB_ICONWARNING | MB_OK);
				}
			}
			else
			{
				MessageBox(0,_TEXT("Failed to set co-operative level for keyboard device."),"Error",MB_ICONWARNING|MB_OK);
			}
		}
		else
		{
			MessageBox(0,_TEXT("failed to set data format for keyboard device,"),"Error",MB_ICONWARNING | MB_OK);
		}
		dis.KeyboardDevice->Release();
		dis.KeyboardDevice = 0;
	}
	else
	{
		MessageBox(0,_TEXT("failed to create main DirectInput objece."),"Error",MB_ICONWARNING|MB_OK);
	}
	return false;
}

void CEGUIManager::DoDirectInputEvents(const CEGUIManager::DirectInputState& dis)
{
	DIDEVICEOBJECTDATA devDat;
	DWORD itemCount = 1;

	HRESULT res = dis.KeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&devDat,&itemCount,0);
	if(SUCCEEDED(res))
	{
		if(itemCount > 0)
		{
			if(LOBYTE(devDat.dwData) & 0x80)
			{
				if(devDat.dwOfs == CEGUI::Key::Escape)
				{
					//处理ESC
					PostQuitMessage(0);
				}
				else
				{
					CEGUI::System::getSingleton().injectKeyDown(devDat.dwOfs);
				}
			}
			else
			{
				CEGUI::System::getSingleton().injectKeyUp(devDat.dwOfs);
			}
		}
	}
	else
	{
		//尝试重新轮询DirectInput 设备
		if((res == DIERR_NOTACQUIRED) || (res == DIERR_INPUTLOST))
		{
			dis.KeyboardDevice->Acquire();
		}
	}
}

void CEGUIManager::CleanupDirectInput(CEGUIManager::DirectInputState& dis)
{
	if(dis.KeyboardDevice)
	{
		dis.KeyboardDevice->Unacquire();
		dis.KeyboardDevice->Release();
		dis.KeyboardDevice = 0;
	}
	if(dis.directInput)
	{
		dis.directInput->Release();
		dis.directInput = 0;
	}
}

bool CEGUIManager::ParseMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CHAR:
		m_SysCEGUI->injectChar((CEGUI::utf32)wParam);
		break;
	case WM_MOUSELEAVE:
		ShowCursor(true);
		break;
	case WM_NCMOUSEMOVE:
		ShowCursor(true);
		break;
	case WM_MOUSEMOVE:
		ShowCursor(false);
		m_SysCEGUI->injectMousePosition( (float)(LOWORD(lParam)) , (float)(HIWORD(lParam)) );
		break;
	case WM_LBUTTONDOWN:
		m_SysCEGUI->injectMouseButtonDown(CEGUI::LeftButton);
		break;
	case WM_LBUTTONUP:
		m_SysCEGUI->injectMouseButtonUp(CEGUI::LeftButton);
		break;
	case WM_RBUTTONDOWN:
		m_SysCEGUI->injectMouseButtonDown(CEGUI::RightButton);
		break;
	case WM_RBUTTONUP:
		m_SysCEGUI->injectMouseButtonUp(CEGUI::RightButton);
		break;
	case WM_MBUTTONDOWN:
		m_SysCEGUI->injectMouseButtonDown(CEGUI::MiddleButton);
		break;
	case WM_MBUTTONUP:
		m_SysCEGUI->injectMouseButtonUp(CEGUI::MiddleButton);
		break;
	case 0x020A:  //WM_MOUSEWHEEL:
		m_SysCEGUI->injectMouseWheelChange(static_cast<float>( (short)HIWORD(wParam) ) / static_cast<float>(120) );
		break;
	case WM_SIZE: //因为创建的窗口暂不支持大小调整，所以这里不做处理
		{
			//CEGUI::System* pSysCEGUI = CEGUI::System::getSingletonPtr();
			////不是最小化才做处理
			//if (pSysCEGUI != 0 && (wParam !=SIZE_MINIMIZED))
			//{
			//	CEGUI::Renderer* pRenderer = pSysCEGUI->getRenderer();
			//	CEGUI::String id(pRenderer->getIdentifierString());
			//}
		}
		break;
	default:
		return false;
	}
	return true;
}

void CEGUIManager::UpDate()
{
	//CEGUI 刷新
	CEGUIManager::GetInstance()->DoUpdateTime();
	CEGUIManager::GetInstance()->DoUpdateFPS();
	CEGUIManager::GetInstance()->DoDirectInputEvents(CEGUIManager::GetInstance()->GetDirectInputState());
}