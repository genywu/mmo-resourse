// GameWorld.cpp: implementation of the GameWorld class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameWorld.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "..\..\engine\ui.h"
#include "..\..\engine\render.h"
#include "..\..\engine\input.h"
#include "..\..\engine\particle.h"

#include "timesystem.h"
#include "cmdmgr.h"
#include "commui.h"
#include "commdb.h"
#include "setting.h"

#include "Map.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool g_bDebug = true;		// 调试功能开关

GameWorld * GameWorld::m_pInstance = NULL;

GameWorld* GameWorld::GetInstance()
{
	return m_pInstance;
}

void GameWorld::InitInstance()
{
	m_pInstance = new GameWorld;
}

void GameWorld::ExitInstance()
{
	delete m_pInstance;
}

BOOL GameWorld::Update()
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

	//检查资源装载情况
	static CClientResource *pClientResource = CClientResource::GetInstance();
	pClientResource->Run();

	return TRUE;
}



void GameWorld::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->BeginScene();

	//界面渲染
	ui::Manager::GetInstance()->Render();

	//粒子渲染
	particle::Manager::GetInstance()->Render();

	//pInterface->GetLayer3D()->Render();
	//pInterface->GetLayer3D()->Clear();
	
	//渲染缓冲
	pInterface->EndScene();
	pInterface->GetDevice()->Present(NULL, NULL, NULL, NULL);
}



void GameWorld::Create(HWND hWnd)
{
	m_hWnd = hWnd;

	//初始化BUILDLIST和CLINETRESOURCE和GameModelManager
	CHAR* strPath = new CHAR[MAX_PATH];
	string strWorpath("");
	if(::GetCurrentDirectory(MAX_PATH,strPath))
	{
		strWorpath=string(strPath);
	}
	delete []strPath;
	string strPackName("Game.pak");
	string strFilesInfo("FilesInfo.ril");
	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,strWorpath,strFilesInfo);
	if(!pClientResource->LoadEx())
	{
		//装载客户端资源信息出错
		//ASSERT(false);
		//return false;
	}
	PutDebugString("Init package file OK!");
	//检查资源装载情况

	//初始化模型管理器
	CGameModelManager::InitInstance();
	CGameModelManager::GetInstance()->Initial();
	CGameModelManager::GetInstance()->SetTextureShareFolder(Setting::GetInstance()->m_pszTextureSharFolder);

	GameBuildList::InitInstance();
	GameBuildList::GetInstance()->Initial();

	//建立控制台
	console::Manager::InitInstance();
	console::Manager::GetInstance()->Create(console::CONSOLE_ERROR | console::CONSOLE_OUTPUT | console::CONSOLE_WARRING);

	//初始其他接口
	input::Interface::InitInstance();
	ui::Manager::InitInstance();
	render::Interface::InitInstance();
	particle::Manager::InitInstance();
	Water::CManager::InitInstance();			//初始化水面管理器

	//建立
	input::Interface::GetInstance()->Create(hWnd);
	ui::Manager::GetInstance()->Create();
	//初始化显示部分
	utility::File file;
	if (!file.Open("render.config"))
	{
		OutputConsole("render.config doesn't exist.\n");
		MessageBox(NULL,"render.config","打开失败",MB_OK);
		PostQuitMessage(-1);		
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

	CommDB::InitInstance();

	CmdMgr::InitInstance();
	
	CommUI::InitInstance();
	CommUI::GetInstance()->Create();

	TimeSystem::InitInstance();

	ui::Manager * pUIMgr = 	ui::Manager::GetInstance();
	//pUIMgr->ReadUIConfig();
}



void GameWorld::Destroy(void)
{
	TimeSystem::ExitInstance();

	CommUI::GetInstance()->Destroy();

	CmdMgr::ExitInstance();
	CommDB::ExitInstance();
	CommUI::ExitInstance();
	//SAFEDELETE(m_pFrustum);
	//销毁BUILDLIST和CLIENTRESOURCE和GameModelManager
	//释放资源管理器
	//释放

	CClientResource::GetInstance()->Release();
	CClientResource::ReleaseInstance();
	GameBuildList::GetInstance()->Release();
	GameBuildList::ReleaseInstance();
	CGameModelManager::GetInstance()->Release();
	CGameModelManager::ReleaseInstance();
	CTextureProjective::Destroy();
	PutDebugString("Release Model OK!");

	//销毁
	Water::CManager::GetInstance()->Destroy();
	particle::Manager::GetInstance()->Destroy();
	render::Interface::GetInstance()->Destroy();
	ui::Manager::GetInstance()->Destroy();
	input::Interface::GetInstance()->Destroy();

	//释放................................
	Water::CManager::ExitInstance();
	particle::Manager::ExitInstance();
	ui::Manager::ExitInstance();
	render::Interface::ExitInstance();
	input::Interface::ExitInstance();

	//销毁控制台
	console::Manager::GetInstance()->Destroy();//关闭日志
	console::Manager::ExitInstance();
}