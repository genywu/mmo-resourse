/***************************************************
      filename:GameWorld.cpp
      created: 2009/12/16
      author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
      purpose: 世界类的实现
****************************************************/
#include "stdafx.h"
#include "GameWorld.h"

#include "GameSetting.h"
#include "GoodsModelManager.h"
#include "../CEGUIEngine/CEGUIManager.h"
#include "../CEGUIEngine/BaseUIClass.h"

//!< 调试功能开关
bool   g_bDebug = true;

//!< 静态变量初始化
GameWorld *GameWorld::m_Instance = NULL;

GameWorld *GameWorld::GetInstance()
{
	return m_Instance;
}

void GameWorld::InitInstance()
{
	m_Instance = new GameWorld;
}

void GameWorld::ExitInstance()
{
	delete m_Instance;
}


void GameWorld::Create(HWND hWnd)
{
	// 创建游戏世界需要的相关项
	m_hWnd = hWnd;//!< 保存窗口句柄

	// 建立控制台
	console::Manager::InitInstance();
	console::Manager::GetInstance()->Create(console::CONSOLE_ERROR | console::CONSOLE_WARRING | console::CONSOLE_OUTPUT);

	//	初始化其他接口
	render::Interface::InitInstance();//!< 渲染接口
	particle::Manager::InitInstance();//!< 粒子系统接口
	GoodsModelManager::InitInstance();//!< 物品管理器接口
	CEGUIManager::InitInstance();//!< 初始化CEGUI接口,有自己的一套Dinput

	//装载客户端资源
	CHAR *strPath = new CHAR[MAX_PATH];
	string strWorPath("");
	if(GetCurrentDirectory(MAX_PATH,strPath))
		strWorPath = strPath;
	delete []strPath;
	string strFilesInfo("FilesInfo.ril");//!< 客户端打包的资源信息文件
	CClientResource::InitialInstance();//!< 客户端资源装载器初始化
	CClientResource  *pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,strWorPath,strFilesInfo);
	if(!pClientResource->LoadEx())
	{
		//装载出错
		//MessageBox(NULL,"装载客户端资源包出错!","ERROR",MB_OK);
	}
	
	//初始化显示部分
	utility::File file;
	if(!file.Open("render.config"))
	{
		OutputConsole("render.config doesn't exits.\n");
		MessageBox(NULL,"render.config","Error",MB_OK);
		PostQuitMessage(-1);
	}

	int iFileLenght = file.GetLength();
	char *pszTemp = new char[iFileLenght+1];
	memcpy(pszTemp,file.GetData(),file.GetLength());
	pszTemp[iFileLenght]=0;
	render::Interface::GetInstance()->Create(hWnd,pszTemp);
	SAFEDELETEARRAY(pszTemp);
	file.Close();

	//粒子管理器相关的创建工作
	particle::Manager::GetInstance()->Create();
	//物品管理器的相关工作
	GoodsModelManager::GetInstance()->InitiliseCamera();//!< 初始化摄像机
	/////////////////////////////////////////////////
	//其他代码
	//CEGUI 初始化
	CEGUIManager  *pCEGUIManager = CEGUIManager::GetInstance();
	if(pCEGUIManager)
	{
		pCEGUIManager->InitCEGUI(render::Interface::GetInstance()->GetDevice());
		pCEGUIManager->InitiliseResourceDirectories();
		pCEGUIManager->InitialiseDefaultResourceGroups();
		pCEGUIManager->InitialiseDirectInput(hWnd,pCEGUIManager->GetDirectInputState());
		//是否显示帧数
		//pCEGUIManager->EnableDisplayFPS(false);
		//注册UIClass类
		pCEGUIManager->RegisterUIClassInList();

		//初始化UI类
		if(pCEGUIManager->GetUIClass("ControlGoodsView") != NULL)
			pCEGUIManager->GetUIClass("ControlGoodsView")->Init();
		if(pCEGUIManager->GetUIClass("GoodsTree") != NULL)
			pCEGUIManager->GetUIClass("GoodsTree")->Init();
		//pCEGUIManager->ExecuteScriptFile("demo8.lua");
	}
}

void GameWorld::Destroy()
{
	/////////////////////////////////////////////////
	//按栈顺序销毁
	/////////////////////////////////////////////////
	//客户端资源层
	CClientResource::GetInstance()->Release();
	CClientResource::ReleaseInstance();
	//UI层
	CEGUIManager::GetInstance()->CleanupDirectInput(CEGUIManager::GetInstance()->GetDirectInputState());
	CEGUIManager::GetInstance()->ReleaseCEGUI();

	//模型渲染层
	GoodsModelManager::GetInstance()->Destory();	
	//粒子系统层
	particle::Manager::GetInstance()->Destroy();
	//渲染层
	render::Interface::GetInstance()->Destroy();
	//控制台层
	console::Manager::GetInstance()->Destroy();//关闭日志

	/////////////////////////////////////////////////
	//释放
	/////////////////////////////////////////////////
	CEGUIManager::ExitInstance();
	GoodsModelManager::ExitInstance();
	particle::Manager::ExitInstance();
	render::Interface::ExitInstance();
	console::Manager::ExitInstance();
}

bool GameWorld::Update()
{
	//按层次（由低到高）刷新

	//检查资源装载情况
	CClientResource::GetInstance()->Run();

	//粒子系统刷新
	particle::Manager *pPM = particle::Manager::GetInstance();
	pPM->Update();

	//模型系统刷新
	GoodsModelManager::GetInstance()->Update();

	//CEGUI刷新,有自己的一套dinput输入消息处理
	CEGUIManager::GetInstance()->UpDate();

	return true;
}

void GameWorld::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->Clear(D3DCOLOR_ARGB(255,0,0,0));
	pInterface->BeginScene();

	//渲染模型
	GoodsModelManager::GetInstance()->RenderModel();
	CEGUIManager::GetInstance()->RenderCEGUI();
	pInterface->EndScene();

	//渲染缓冲
	if (pInterface->GetDevice()->Present( NULL, NULL, NULL, NULL ) == D3DERR_DEVICELOST)
	{
		HRESULT hr = pInterface->GetDevice()->TestCooperativeLevel();
		while(hr == D3DERR_DEVICELOST)
		{
			hr = pInterface->GetDevice()->TestCooperativeLevel();
		}

		if (hr == D3DERR_DEVICENOTRESET)
		{
			hr = pInterface->GetDevice()->Reset(&pInterface->GetInfo()->d3dpp);
			if (hr != S_OK)
			{
				pInterface->Reset();
			}
		}
	}
}