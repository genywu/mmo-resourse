#pragma once

#define PI 3.1415926535897f
#define DTOR (PI/180.0f)
#define SQR(x) (x*x)

class scene
{
public:
	//天空球
	class SkyDome
	{


		DWORD m_dwShaderGroupHandle;

		DWORD NumPlaneVertices;

		D3DXVECTOR3 *m_pPosBuffer;
		D3DXVECTOR2 *m_pUV0Buffer;
		D3DCOLOR    *m_pColBuffer;	

		DWORD *Indices;
		int NumIndices;

		float m_fRadius; 

	public:
		void Create(const char *pszFileName,int divisions, float PlanetRadius, float AtmosphereRadius,float hTile, float vTile);
		void Destroy();

		void Render();

		void Flow(float fOffset);



		SkyDome(void);
		~SkyDome(void);
	};

	//天空面
	//天空盒
	
	class Skybox
	{
		enum SIDETYPE{ SIDEFRONT = 0,SIDEBACK,SIDETOP,SIDEBOTTOM,SIDELEFT,SIDERIGHT};

		DWORD m_dwShaderGroupHandle;

		D3DXVECTOR3 m_pPosBuffer[36];
		D3DXVECTOR2 m_pUV0Buffer[36];
	public:
		BOOL Create(const char *pszFileName,float fWidth,float fHeight,float fDepth,const D3DXVECTOR3 *pCenter = NULL);
		void Destroy();

		void Render();

		Skybox(void);
		~Skybox(void);
	};

	//天空面
	class SkyPlane
	{
		D3DXVECTOR3 m_pPosBuffer[6];
		D3DXVECTOR2 m_pUV0Buffer[6];
		D3DXVECTOR2 m_pUV1Buffer[6];
		D3DCOLOR	m_pColBuffer[6];

		DWORD m_dwShaderGroupHandle;

		float m_fSpeed[2];//每毫秒

		DWORD m_dwLastTime;

	public:

		BOOL Create(const char *pszFileName,float fWidth,float fHeight,float fDepth);
		void Destroy();

		void Render();

		SkyPlane(void);
		~SkyPlane(void);

		void ProcessFlow(DWORD dwCurrentTime);
		void SetFlowSpeed(DWORD dwLayer,float fSpeed);
		void ProcessColor(DWORD dwColor);
	};
};
