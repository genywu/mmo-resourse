// GameWorld.h: interface for the GameWorld class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4996)

#include <windows.h>

#ifdef _DEBUG
#define MAPEDITORNAME "Map Editor 3.12.3002 LastUpdate: 2010/01/04 (DEBUG)" 
#else
#define MAPEDITORNAME "Map Editor 3.12.3002 LastUpdate: 2010/01/04 (RELEASE)"
#endif

extern bool g_bDebug;

class GameWorld  
{
	HWND   m_hWnd;
	static GameWorld * m_pInstance;

public:

	BOOL Update();
	void Render();

	static GameWorld * GetInstance();
	static void InitInstance();
	static void ExitInstance();

	void Create(HWND hWnd);
	void Destroy(void);
};


