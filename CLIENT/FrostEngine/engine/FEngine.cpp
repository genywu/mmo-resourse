#include "stdafx.h"
#include "fengine.h"
#include "futility.h"
#include "fconsole.h"
#include "finput.h"
#include "fparticle.h"
#include "fui.h"
#include "frender.h"


IMPLEMENT_SINGLETON(engine::Manager);

engine::Manager::Manager(void)
{
}

engine::Manager::~Manager(void)
{
}

BOOL engine::Manager::Create(HWND hWnd)
{
	m_hWnd = hWnd;

	//建立控制台
	console::Manager::InitInstance();
	console::Manager::GetInstance()->Create(console::CONSOLE_ERROR |
		console::CONSOLE_OUTPUT | console::CONSOLE_WARRING);
	
	//初始其他接口
	input::Interface::InitInstance();
	ui::Manager::InitInstance();
	render::Interface::InitInstance();
	particle::Manager::InitInstance();

	//建立
	input::Interface::GetInstance()->Create(hWnd);
	ui::Manager::GetInstance()->Create();
	//初始化显示部分
	utility::File file;
	if (!file.Open("frost.engine.config"))
	{
        OutputConsole("Engine config file \"frost.engine.config\" doesn't exist.\n");
		PostQuitMessage(-1);		
		return FALSE;
	}
	int iFileLength = file.GetLength();
	char *pszTemp = new char[iFileLength + 1];			
	memcpy(pszTemp,file.GetData(),file.GetLength());
	pszTemp[iFileLength] = 0;
	render::Interface::GetInstance()->Create(hWnd,pszTemp);
	SAFEDELETE(pszTemp);
	file.Close();
	//初始化粒子管理器
	particle::Manager::GetInstance()->Create();

	//写入编译版本类型
#ifdef _DEBUG
	OutputConsole("<FONT style=\"BACKGROUND-COLOR: lavender\">Engine Compiling:<FONT COLOR=\"#4f2000\">_DEBUG</FONT></FONT>\n");
#else
	OutputConsole("<FONT style=\"BACKGROUND-COLOR: lavender\">Engine Compiling:<FONT COLOR=\"#4f2000\">_RELEASE</FONT></FONT>\n");
#endif

	
	
	return TRUE;	
}

void engine::Manager::Destroy()
{
	//SAFEDELETE(m_pFrustum);

	//销毁
	particle::Manager::GetInstance()->Destroy();
	render::Interface::GetInstance()->Destroy();
	ui::Manager::GetInstance()->Destroy();
	input::Interface::GetInstance()->Destroy();

	//释放................................
	particle::Manager::ExitInstance();
	ui::Manager::ExitInstance();
	render::Interface::ExitInstance();
	input::Interface::ExitInstance();


	//销毁控制台
	console::Manager::GetInstance()->Destroy();//关闭日志
	console::Manager::ExitInstance();
}

//处理部分Windows消息
BOOL engine::Manager::WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage) 
	{
	case WM_ACTIVATE:
			m_bWndActive = LOWORD(wParam);  
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:		
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case 0x020a:
	case WM_CHAR:		
		{
			input::Interface *pInput = input::Interface::GetInstance();
			if (pInput)
			{
				pInput->ParseMsg(uMessage,wParam,lParam);
			}
		}
		break;
	default:
		return FALSE;//未处理任何消息
	}

	return TRUE;
}

void engine::Manager::Update()
{
	static input::Interface::_tagEVENT stEvent;
	input::Interface * pInput = input::Interface::GetInstance();
	//输入消息
	pInput->UpdateInput();
	pInput->PopEvent(&stEvent);
	//传送INPUT消息到UI
	ui::Manager *pUiMgr = ui::Manager::GetInstance();
	ui::Manager::_tagINPUTINFO *pInputInfo =  pUiMgr->GetInputInfoBuffer();
	pInputInfo->eType  = (UI_INPUT_MSG_TYPE)stEvent.eType;
	pInputInfo->dwData = stEvent.dwData;
	pInputInfo->wParam = stEvent.wParam;
	pInputInfo->lParam = stEvent.lParam;

	memcpy(pInputInfo->byKeyBuffer,pInput->GetKeyStateBuffer(),256);
	pInputInfo->dwMouseButtonState = pInput->GetMouseBtnStatus();
	pInput->GetMousePosition(&pInputInfo->ptMouse,&pInputInfo->ptMouseOld);

	pUiMgr->Update();

	//粒子系统刷新
	particle::Manager *pPM = particle::Manager::GetInstance();
	pPM->Update();
}

void engine::Manager::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->BeginRender();

	//粒子渲染
	particle::Manager::GetInstance()->Render();
	
	//界面渲染
	ui::Manager::GetInstance()->Render();	
	
	//渲染缓冲
	pInterface->EndRender();
}
void engine::Manager::SetWnd(void * pWnd)
{
	ui::Manager::GetInstance()->SetMainWnd((ui::Wnd*)pWnd);
}

//void engine::Manager::SetActiveCamera(render::Camera *pCamera)
//{
//	//计算当前使用的摄像机的Frustum
//	m_pFrustum->CalculateFrustum(pCamera->GetViewMatrix(),pCamera->GetProjectionMatrix());
//}
