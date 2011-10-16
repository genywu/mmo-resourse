// GameWorld.h: interface for the GameWorld class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

class WndViewer;

extern bool g_bDebug;

class GameWorld  
{
	HWND   m_hWnd;
	static GameWorld * m_pInstance;
public:

	BOOL Update();
	void Render();

	WndViewer *m_pMainWnd;						     

	static GameWorld * GetInstance();
	static void InitInstance();
	static void ExitInstance();

	void Create(HWND hWnd);
	void Destroy(void);
};


