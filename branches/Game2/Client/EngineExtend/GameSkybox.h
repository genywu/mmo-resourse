#pragma once

#include "../Engine/model.h"
#include "displaymodel.h"

class GameSkybox
{
	AnimInfo *m_pAnimInfo;
	CDisplayModel* m_pDisplayModel;
	D3DXMATRIX m_matscal;
	DWORD m_dwModelID;
	DWORD m_dwScal;
public:

	GameSkybox(void);
	~GameSkybox(void);
	AnimInfo* GetAnimInfo(){return m_pAnimInfo;}
	CDisplayModel* GetDisplayModel(){return m_pDisplayModel;}
	DWORD GetModelID(){return m_dwModelID;}
	DWORD GetScal(){return m_dwScal;}
	void SetScal(float size);
	void Render(render::Camera *pCamera);
	BOOL Create(DWORD ModelID);
	void Destroy();
};