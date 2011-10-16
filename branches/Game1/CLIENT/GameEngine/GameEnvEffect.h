#pragma once

#include "..\frostengine\utility.h"

//包含在地图中

class GameBaseEnvEffect
{
public:
	virtual void Update(D3DXVECTOR3 *pPositon){};
	virtual void Render(const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX *pBillboardMatrixY){};
	virtual BOOL Create(utility::Text::Node *pNode,char *szResourcePath){return TRUE;};
	virtual void Destroy(){};

	static  GameBaseEnvEffect * CreateEnvEffect(const char *szFileName);
};

class GameEnvEffect_Particle 
    : public GameBaseEnvEffect
{

	DWORD m_dwBillboardType;//0 无 1 有 2 垂直
	DWORD m_dwShaderGroupHandle;
	

	DWORD m_dwLastTime;

	DWORD m_dwAlphaMode;
	DWORD m_dwMinLifeTime;
	DWORD m_dwMaxLifeTime;

	float m_fScale;
	float m_fRadius;
	DWORD m_dwColumns;
	DWORD m_dwRows;
	
	D3DXVECTOR3 m_vInitSpeed;
	D3DXVECTOR3 m_vBaseWind;
	D3DXVECTOR3 m_vRadomWind;

	BOOL  m_bRotation;
	DWORD m_dwRotationBaseInterval;
	DWORD m_dwRotationRadomInterval;
	int   m_iRotationAngle;

	//模拟光照效果
	BOOL  m_bLight;
	DWORD m_dwLightMinInterval;
	DWORD m_dwLightMaxInterval;
	float m_fLightMinRatio;
	float m_fLightMaxRatio;

	struct tagParticle
	{
		DWORD dwTextureRectIndex;
		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vSpeed;
		DWORD dwLifeTime;
		DWORD dwColor;
		int   iAngleX;
		int   iAngleY;
		int   iAngleZ;
		DWORD dwLastRotationTime;
		DWORD dwLastLightTime;
		D3DXVECTOR3 vRadomWind;
	};

	tagParticle * m_pParticleArray;
	DWORD m_dwParticleCount;

	public:

	GameEnvEffect_Particle();
	~GameEnvEffect_Particle();
	BOOL Create(utility::Text::Node *pNode,char *szResourcePath);
	void Destroy();
	void Update(D3DXVECTOR3 *pPostion);
	void Render(const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX *pBillboardMatrixY);
};

class GameEnvEffect_Flow
	: public GameBaseEnvEffect
{
	struct tagLayer
	{
		DWORD dwType;
		DWORD dwTexureHandle;
		float fU,fV;
		DWORD dwTimeCycle;
		DWORD dwStartTime;
		DWORD dwTimeElapse;
		float fUVScaleRatio;
	} *m_pLayerArray;

	DWORD m_dwLayerCount;

	public:
	GameEnvEffect_Flow();
	~GameEnvEffect_Flow();

	BOOL Create(utility::Text::Node *pNode,char *szResourcePath);
	void Destroy();
	void Update(D3DXVECTOR3 *pPostion);
	void Render(const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX *pBillboardMatrixY);
};