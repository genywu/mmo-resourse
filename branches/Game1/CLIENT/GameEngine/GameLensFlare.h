#pragma once

#include "..\frostengine\render.h"

class GameLensFlare
{
	DWORD m_dwShaderGroupHandle;
public:
	GameLensFlare(void); 
	~GameLensFlare(void);

	BOOL Create(const char * szFileName);
	void Render(render::Camera * pCamera);
};
