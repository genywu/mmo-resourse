#pragma once

// 3D±³¾°
class CBackModel
{
private:
	GameModel::AnimInfo			m_cBackModelAnimInfo;
	GameModel*					m_pBackModel;
	render::Camera*				m_pCamera;
	D3DXVECTOR3					m_vLightDirection;
	DWORD						m_dwAmbient;
	DWORD						m_dwDiffuse;
	DWORD						m_dwFogColor;
	DWORD						m_bFogEnable;
	float						m_fFogStart;
	float						m_fFogEnd;

public:
	CBackModel(void);
	virtual ~CBackModel(void);

	render::Camera *		    GetCamera() { return m_pCamera;}

	void Load(const char* strPath);
	void Display();
};
