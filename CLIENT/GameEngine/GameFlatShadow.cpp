#include "StdAfx.h"

#include "..\frostengine\utility.h"
#include "..\frostengine\console.h"
#include "..\frostengine\render.h"


#include "gameflatshadow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <list>
using namespace std;

GameFlatShadow::GameFlatShadow(void)
{
	m_dwShaderGroupHandle = -1;
}

GameFlatShadow::~GameFlatShadow(void)
{
	ASSERT(m_dwShaderGroupHandle == -1);
}

void GameFlatShadow::Create()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	static const char szShadowShader[] = "texture { \
										 layer0 { 0 = shadow.tga;} \
										 layer1 { default = noTexture; } \
										 } \
										 shader { \
										 sdr0 { stream = poscoltex0; \
										 twoside = true;\
										 alphatest = transparent; \
										 alphablend  = alpha; \
										 layer0 { ColorOp   = modulate;\
										 ColorOp   = Modulate;\
										 ColorArg1 = Texture;\
										 ColorArg2 = Diffuse;\
										 AlphaOp   = Modulate;\
										 AlphaArg1 = Texture;\
										 AlphaArg2 = Diffuse;\
										 tex = 0;} \
										 layer1 { ColorOp   = Disable;\
										 AlphaOp   = Disable;\
										 tex = 0; } \
										 } \
										 }";

	if(m_dwShaderGroupHandle != -1)
	{
		pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
		m_dwShaderGroupHandle = -1;
	}

	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(szShadowShader, "pictures");
	if(m_dwShaderGroupHandle == -1)
		ASSERT(0);

	/* 下列代码为原代码
	m_pPos[0] = D3DXVECTOR3(-0.5f, 0.04f, -0.5f);
	m_pPos[1] = D3DXVECTOR3(0.5f, 0.04f, -0.5f);
	m_pPos[2] = D3DXVECTOR3(0.5f, 0.04f, 0.5f);

	m_pPos[3] = D3DXVECTOR3(-0.5f, 0.04f, -0.5f);
	m_pPos[4] = D3DXVECTOR3(0.5f, 0.04f, 0.5f);
	m_pPos[5] = D3DXVECTOR3(-0.5f, 0.04f, 0.5f);

	m_pUV0[0] = D3DXVECTOR2(0.0f, 0.0f);
	m_pUV0[1] = D3DXVECTOR2(0.0f, 1.0f);
	m_pUV0[2] = D3DXVECTOR2(1.0f, 1.0f);

	m_pUV0[3] = D3DXVECTOR2(0.0f, 0.0f);
	m_pUV0[4] = D3DXVECTOR2(1.0f, 1.0f);
	m_pUV0[5] = D3DXVECTOR2(1.0f, 0.0f);

	m_pColor[0] = 0xffffffff;
	m_pColor[1] = 0xffffffff;
	m_pColor[2] = 0xffffffff;

	m_pColor[3] = 0xffffffff;
	m_pColor[4] = 0xffffffff;
	m_pColor[5] = 0xffffffff;
	*/

	for (int i = 0; i<36; i++)
	{
		m_pPos[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_pUV0[i] = D3DXVECTOR2(0.0f, 0.0f);
		m_pColor[i] = 0xffffffff;
	}

}

void GameFlatShadow::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	if(m_dwShaderGroupHandle != -1)
	{
		pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
		m_dwShaderGroupHandle = -1;
	}
}

void GameFlatShadow::Render(float x,float y,float z,float fScaleFactor, GameMap *pGameMap)
{
	//pShape->GetPosY(), pShape->GetHeight(), pShape->GetPosX()
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	static D3DXVECTOR3 pPos[36];
	static D3DCOLOR    dwColorSpecular[36] = {0};
	static int dwCount = 2;	//要渲染多少个三角形

	if ( x-(int)x>0.5f && z-(int)z<0.5f )	//上下移动 1/2
	{
		//先取出一些直接可以得到的坐标
		m_pPos[0].x = x - 0.5f;		m_pPos[0].z = z - 0.5f;
		m_pPos[3].x = x - 0.5f;		m_pPos[3].z = z + 0.5f;
		m_pPos[9].x = x + 0.5f;		m_pPos[9].z = z - 0.5f;
		m_pPos[12].x = x + 0.5f;	m_pPos[12].z = z + 0.5f;

		m_pPos[13].x = (int)x;		m_pPos[13].z = (int)z - 1;
		m_pPos[14].x = (int)x;		m_pPos[14].z = (int)z;
		m_pPos[15].x = (int)x;		m_pPos[15].z = (int)z + 1;

		m_pPos[16].x = (int)x + 1;	m_pPos[16].z = (int)z - 1;
		m_pPos[7].x = (int)x + 1;	m_pPos[7].z = (int)z;
		m_pPos[17].x = (int)x + 1;	m_pPos[17].z = (int)z + 1;

		m_pPos[18].x = (int)x + 2;	m_pPos[18].z = (int)z - 1;
		m_pPos[19].x = (int)x + 2;	m_pPos[19].z = (int)z;
		m_pPos[20].x = (int)x + 2;	m_pPos[20].z = (int)z + 1;

		//再计算一些交点
		m_pPos[1] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[3].x, m_pPos[3].z, 
			m_pPos[14].x, m_pPos[14].z, m_pPos[7].x, m_pPos[7].z);

		m_pPos[2] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[3].x, m_pPos[3].z, 
			m_pPos[14].x, m_pPos[14].z, m_pPos[17].x, m_pPos[17].z);

		m_pPos[4] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[9].x, m_pPos[9].z, 
			m_pPos[13].x, m_pPos[13].z, m_pPos[7].x, m_pPos[7].z);

		m_pPos[6] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[9].x, m_pPos[9].z, 
			m_pPos[16].x, m_pPos[16].z, m_pPos[7].x, m_pPos[7].z);

		m_pPos[5] = pGameMap->LineIntersectLine(m_pPos[3].x, m_pPos[3].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[14].x, m_pPos[14].z, m_pPos[17].x, m_pPos[17].z);

		m_pPos[8] = pGameMap->LineIntersectLine(m_pPos[3].x, m_pPos[3].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[7].x, m_pPos[7].z, m_pPos[17].x, m_pPos[17].z);

		m_pPos[10] = pGameMap->LineIntersectLine(m_pPos[9].x, m_pPos[9].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[7].x, m_pPos[7].z, m_pPos[19].x, m_pPos[19].z);

		m_pPos[11] = pGameMap->LineIntersectLine(m_pPos[9].x, m_pPos[9].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[7].x, m_pPos[7].z, m_pPos[20].x, m_pPos[20].z);

		for (int i=0; i<21; i++)
		{
			m_pPos[i].y = pGameMap->CalculateHeight(m_pPos[i].x, m_pPos[i].z)+0.04f;
		}

		pPos[0]		= m_pPos[0];	m_pUV0[0] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[1]		= m_pPos[4];	m_pUV0[1] = D3DXVECTOR2(0.0f,						m_pPos[4].x-m_pPos[0].x);
		pPos[2]		= m_pPos[1];	m_pUV0[2] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);

		pPos[3]		= m_pPos[4];	m_pUV0[3] = D3DXVECTOR2(0.0f,						m_pPos[4].x-m_pPos[0].x);
		pPos[4]		= m_pPos[6];	m_pUV0[4] = D3DXVECTOR2(0.0f,						m_pPos[6].x-m_pPos[0].x);
		pPos[5]		= m_pPos[7];	m_pUV0[5] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);

		pPos[6]		= m_pPos[1];	m_pUV0[6] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[7]		= m_pPos[4];	m_pUV0[7] = D3DXVECTOR2(0.0f,						m_pPos[4].x-m_pPos[0].x);
		pPos[8]		= m_pPos[7];	m_pUV0[8] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);

		pPos[9]		= m_pPos[1];	m_pUV0[9] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[10]	= m_pPos[7];	m_pUV0[10] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	 m_pPos[7].x-m_pPos[1].x);
		pPos[11]	= m_pPos[2];	m_pUV0[11] = D3DXVECTOR2(m_pPos[2].z-m_pPos[0].z,	0.0f);

		pPos[12]	= m_pPos[2];	m_pUV0[12] = D3DXVECTOR2(m_pPos[2].z-m_pPos[0].z,	0.0f);
		pPos[13]	= m_pPos[7];	m_pUV0[13] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);
		pPos[14]	= m_pPos[5];	m_pUV0[14] = D3DXVECTOR2(1.0f,						m_pPos[5].x-m_pPos[3].x);

		pPos[15]	= m_pPos[2];	m_pUV0[15] = D3DXVECTOR2(m_pPos[2].z-m_pPos[0].z,	0.0f);
		pPos[16]	= m_pPos[5];	m_pUV0[16] = D3DXVECTOR2(1.0f,						m_pPos[5].x-m_pPos[3].x);
		pPos[17]	= m_pPos[3];	m_pUV0[17] = D3DXVECTOR2(1.0f,						0.0f);

		pPos[18]	= m_pPos[5];	m_pUV0[18] = D3DXVECTOR2(1.0f,						m_pPos[5].x-m_pPos[3].x);
		pPos[19]	= m_pPos[7];	m_pUV0[19] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);
		pPos[20]	= m_pPos[8];	m_pUV0[20] = D3DXVECTOR2(1.0f,						m_pPos[8].x-m_pPos[3].x);

		pPos[21]	= m_pPos[6];	m_pUV0[21] = D3DXVECTOR2(0.0f,						m_pPos[6].x-m_pPos[0].x);
		pPos[22]	= m_pPos[9];	m_pUV0[22] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[23]	= m_pPos[7];	m_pUV0[23] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);

		pPos[24]	= m_pPos[7];	m_pUV0[24] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);
		pPos[25]	= m_pPos[9];	m_pUV0[25] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[26]	= m_pPos[10];	m_pUV0[26] = D3DXVECTOR2(m_pPos[10].z-m_pPos[9].z,	1.0f);

		pPos[27]	= m_pPos[7];	m_pUV0[27] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);
		pPos[28]	= m_pPos[10];	m_pUV0[28] = D3DXVECTOR2(m_pPos[10].z-m_pPos[9].z,	1.0f);
		pPos[29]	= m_pPos[11];	m_pUV0[29] = D3DXVECTOR2(m_pPos[11].z-m_pPos[9].z,	1.0f);

		pPos[30]	= m_pPos[7];	m_pUV0[30] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	m_pPos[7].x-m_pPos[1].x);
		pPos[31]	= m_pPos[11];	m_pUV0[31] = D3DXVECTOR2(m_pPos[11].z-m_pPos[9].z,	1.0f);
		pPos[32]	= m_pPos[8];	m_pUV0[32] = D3DXVECTOR2(1.0f,						m_pPos[8].x-m_pPos[3].x);

		pPos[33]	= m_pPos[8];	m_pUV0[33] = D3DXVECTOR2(1.0f,						m_pPos[8].x-m_pPos[3].x);
		pPos[34]	= m_pPos[11];	m_pUV0[34] = D3DXVECTOR2(m_pPos[11].z-m_pPos[9].z,	1.0f);
		pPos[35]	= m_pPos[12];	m_pUV0[35] = D3DXVECTOR2(1.0f,						1.0f);

		dwCount = 12;

	}
	else if ( x-(int)x<0.5f && z-(int)z>0.5f )	//上下移动 2/2
	{
		//先取出一些直接可以得到的坐标
		m_pPos[0].x = x - 0.5f;		m_pPos[0].z = z - 0.5f;
		m_pPos[3].x = x - 0.5f;		m_pPos[3].z = z + 0.5f;
		m_pPos[9].x = x + 0.5f;		m_pPos[9].z = z - 0.5f;
		m_pPos[12].x = x + 0.5f;	m_pPos[12].z = z + 0.5f;

		m_pPos[13].x = (int)x - 1;	m_pPos[13].z = (int)z;
		m_pPos[14].x = (int)x - 1;	m_pPos[14].z = (int)z + 1;
		m_pPos[15].x = (int)x - 1;	m_pPos[15].z = (int)z + 2;

		m_pPos[16].x = (int)x;		m_pPos[16].z = (int)z;
		m_pPos[5].x = (int)x;		m_pPos[5].z = (int)z + 1;
		m_pPos[17].x = (int)x;		m_pPos[17].z = (int)z + 2;

		m_pPos[18].x = (int)x + 1;	m_pPos[18].z = (int)z;
		m_pPos[19].x = (int)x + 1;	m_pPos[19].z = (int)z + 1;
		m_pPos[20].x = (int)x + 1;	m_pPos[20].z = (int)z + 2;

		//再计算一些交点
		m_pPos[1] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[3].x, m_pPos[3].z, 
			m_pPos[13].x, m_pPos[13].z, m_pPos[5].x, m_pPos[5].z);

		m_pPos[2] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[3].x, m_pPos[3].z, 
			m_pPos[14].x, m_pPos[14].z, m_pPos[5].x, m_pPos[5].z);

		m_pPos[4] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[9].x, m_pPos[9].z, 
			m_pPos[16].x, m_pPos[16].z, m_pPos[5].x, m_pPos[5].z);

		m_pPos[6] = pGameMap->LineIntersectLine(m_pPos[3].x, m_pPos[3].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[5].x, m_pPos[5].z, m_pPos[17].x, m_pPos[17].z);

		m_pPos[7] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[9].x, m_pPos[9].z, 
			m_pPos[16].x, m_pPos[16].z, m_pPos[19].x, m_pPos[19].z);

		m_pPos[8] = pGameMap->LineIntersectLine(m_pPos[3].x, m_pPos[3].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[5].x, m_pPos[5].z, m_pPos[20].x, m_pPos[20].z);

		m_pPos[10] = pGameMap->LineIntersectLine(m_pPos[9].x, m_pPos[9].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[16].x, m_pPos[16].z, m_pPos[19].x, m_pPos[19].z);

		m_pPos[11] = pGameMap->LineIntersectLine(m_pPos[9].x, m_pPos[9].z, m_pPos[12].x, m_pPos[12].z, 
			m_pPos[5].x, m_pPos[5].z, m_pPos[19].x, m_pPos[19].z);

		for (int i=0; i<21; i++)
		{
			m_pPos[i].y = pGameMap->CalculateHeight(m_pPos[i].x, m_pPos[i].z)+0.04f;
		}

		pPos[0]		= m_pPos[0];	m_pUV0[0] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[1]		= m_pPos[4];	m_pUV0[1] = D3DXVECTOR2(0.0f,						m_pPos[4].x-m_pPos[0].x);
		pPos[2]		= m_pPos[1];	m_pUV0[2] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);

		pPos[3]		= m_pPos[1];	m_pUV0[3] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[4]		= m_pPos[4];	m_pUV0[4] = D3DXVECTOR2(0.0f,						m_pPos[4].x-m_pPos[0].x);
		pPos[5]		= m_pPos[5];	m_pUV0[5] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);

		pPos[6]		= m_pPos[1];	m_pUV0[6] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[7]		= m_pPos[5];	m_pUV0[7] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);
		pPos[8]		= m_pPos[2];	m_pUV0[8] = D3DXVECTOR2(m_pPos[2].z-m_pPos[0].z,	0.0f);

		pPos[9]		= m_pPos[2];	m_pUV0[9] = D3DXVECTOR2(m_pPos[2].z-m_pPos[0].z,	0.0f);
		pPos[10]	= m_pPos[5];	m_pUV0[10] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);
		pPos[11]	= m_pPos[3];	m_pUV0[11] = D3DXVECTOR2(1.0f,						0.0f);

		pPos[12]	= m_pPos[3];	m_pUV0[12] = D3DXVECTOR2(1.0f,						0.0f);
		pPos[13]	= m_pPos[5];	m_pUV0[13] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);
		pPos[14]	= m_pPos[6];	m_pUV0[14] = D3DXVECTOR2(1.0f,						m_pPos[6].x-m_pPos[3].x);

		pPos[15]	= m_pPos[4];	m_pUV0[15] = D3DXVECTOR2(0.0f,						m_pPos[4].x-m_pPos[0].x);
		pPos[16]	= m_pPos[7];	m_pUV0[16] = D3DXVECTOR2(0.0f,						m_pPos[7].x-m_pPos[0].x);
		pPos[17]	= m_pPos[5];	m_pUV0[17] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);

		pPos[18]	= m_pPos[5];	m_pUV0[18] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);
		pPos[19]	= m_pPos[7];	m_pUV0[19] = D3DXVECTOR2(0.0f,						m_pPos[7].x-m_pPos[0].x);
		pPos[20]	= m_pPos[10];	m_pUV0[20] = D3DXVECTOR2(m_pPos[10].z-m_pPos[9].z,	1.0f);

		pPos[21]	= m_pPos[7];	m_pUV0[21] = D3DXVECTOR2(0.0f,						m_pPos[7].x-m_pPos[0].x);
		pPos[22]	= m_pPos[9];	m_pUV0[22] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[23]	= m_pPos[10];	m_pUV0[23] = D3DXVECTOR2(m_pPos[10].z-m_pPos[9].z,	1.0f);

		pPos[24]	= m_pPos[5];	m_pUV0[24] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);
		pPos[25]	= m_pPos[10];	m_pUV0[25] = D3DXVECTOR2(m_pPos[10].z-m_pPos[9].z,	1.0f);
		pPos[26]	= m_pPos[11];	m_pUV0[26] = D3DXVECTOR2(m_pPos[11].z-m_pPos[9].z,	1.0f);

		pPos[27]	= m_pPos[5];	m_pUV0[27] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);
		pPos[28]	= m_pPos[8];	m_pUV0[28] =D3DXVECTOR2(1.0f,						m_pPos[8].x-m_pPos[3].x);
		pPos[29]	= m_pPos[6];	m_pUV0[29] = D3DXVECTOR2(1.0f,						m_pPos[6].x-m_pPos[3].x);

		pPos[30]	= m_pPos[5];	m_pUV0[30] = D3DXVECTOR2(m_pPos[5].z-m_pPos[4].z,	m_pPos[5].x-m_pPos[2].x);
		pPos[31]	= m_pPos[11];	m_pUV0[31] = D3DXVECTOR2(m_pPos[11].z-m_pPos[9].z,	1.0f);
		pPos[32]	= m_pPos[8];	m_pUV0[32] =D3DXVECTOR2(1.0f,						m_pPos[8].x-m_pPos[3].x);

		pPos[33]	= m_pPos[8];	m_pUV0[33] = D3DXVECTOR2(1.0f,						m_pPos[8].x-m_pPos[3].x);
		pPos[34]	= m_pPos[11];	m_pUV0[34] = D3DXVECTOR2(m_pPos[11].z-m_pPos[9].z,	1.0f);
		pPos[35]	= m_pPos[12];	m_pUV0[35] = D3DXVECTOR2(1.0f,						1.0f);

		dwCount = 12;
	}
	else if ( (x-(int)x<0.5f && z-(int)z<0.5f) || (x-(int)x>0.5f && z-(int)z>0.5f) )	//左右移动
	{
		//先取出一些直接可以得到的坐标
		m_pPos[0].x = x - 0.5f;		m_pPos[0].z = z - 0.5f;
		m_pPos[2].x = x - 0.5f;		m_pPos[2].z = z + 0.5f;
		m_pPos[6].x = x + 0.5f;		m_pPos[6].z = z - 0.5f;
		m_pPos[8].x = x + 0.5f;		m_pPos[8].z = z + 0.5f;

		if ( x-(int)x<0.5f && z-(int)z<0.5f )
		{
			m_pPos[9].x = (int)x - 1;	m_pPos[9].z = (int)z - 1;
			m_pPos[10].x = (int)x - 1;	m_pPos[10].z = (int)z;
			m_pPos[11].x = (int)x - 1;	m_pPos[11].z = (int)z + 1;

			m_pPos[12].x = (int)x;		m_pPos[12].z = (int)z - 1;
			m_pPos[4].x = (int)x;		m_pPos[4].z = (int)z;
			m_pPos[13].x = (int)x;		m_pPos[13].z = (int)z + 1;

			m_pPos[14].x = (int)x + 1;	m_pPos[14].z = (int)z - 1;
			m_pPos[15].x = (int)x + 1;	m_pPos[15].z = (int)z;
			m_pPos[16].x = (int)x + 1;	m_pPos[16].z = (int)z + 1;
		} 
		else if ( x-(int)x>0.5f && z-(int)z>0.5f )
		{
			m_pPos[9].x = (int)x;		m_pPos[9].z = (int)z;
			m_pPos[10].x = (int)x;		m_pPos[10].z = (int)z + 1;
			m_pPos[11].x = (int)x;		m_pPos[11].z = (int)z + 2;

			m_pPos[12].x = (int)x + 1;	m_pPos[12].z = (int)z;
			m_pPos[4].x = (int)x + 1;	m_pPos[4].z = (int)z + 1;
			m_pPos[13].x = (int)x + 1;	m_pPos[13].z = (int)z + 2;

			m_pPos[14].x = (int)x + 2;	m_pPos[14].z = (int)z;
			m_pPos[15].x = (int)x + 2;	m_pPos[15].z = (int)z + 1;
			m_pPos[16].x = (int)x + 2;	m_pPos[16].z = (int)z + 2;
		}
		else
		{
			m_pPos[9].x = x - 1;		m_pPos[9].z = z - 1;
			m_pPos[10].x = x - 1;		m_pPos[9].z = z;
			m_pPos[11].x = x - 1;		m_pPos[9].z = z + 1;

			m_pPos[12].x = x;			m_pPos[9].z = z - 1;
			m_pPos[4].x = x;			m_pPos[9].z = z;
			m_pPos[13].x = 

				m_pPos[14].x = x + 1;		m_pPos[9].z = z - 1;
			m_pPos[15].x = x + 1;		m_pPos[9].z = z;
			m_pPos[16].x = x + 1;		m_pPos[9].z = z + 1;
		}


		//再计算一些交点
		m_pPos[1] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[2].x, m_pPos[2].z, 
			m_pPos[4].x, m_pPos[4].z, m_pPos[10].x, m_pPos[10].z);

		m_pPos[3] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[6].x, m_pPos[6].z, 
			m_pPos[4].x, m_pPos[4].z, m_pPos[12].x, m_pPos[12].z);

		m_pPos[5] = pGameMap->LineIntersectLine(m_pPos[2].x, m_pPos[2].z, m_pPos[8].x, m_pPos[8].z, 
			m_pPos[4].x, m_pPos[4].z, m_pPos[13].x, m_pPos[13].z);

		m_pPos[7] = pGameMap->LineIntersectLine(m_pPos[6].x, m_pPos[6].z, m_pPos[8].x, m_pPos[8].z, 
			m_pPos[4].x, m_pPos[4].z, m_pPos[15].x, m_pPos[15].z);

		for (int i=0; i<17; i++)
		{
			m_pPos[i].y = pGameMap->CalculateHeight(m_pPos[i].x, m_pPos[i].z)+0.04f;
		}

		pPos[0]		= m_pPos[0];	m_pUV0[0] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[1]		= m_pPos[3];	m_pUV0[1] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[2]		= m_pPos[4];	m_pUV0[2] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);

		pPos[3]		= m_pPos[0];	m_pUV0[3] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[4]		= m_pPos[4];	m_pUV0[4] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);
		pPos[5]		= m_pPos[1];	m_pUV0[5] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);

		pPos[6]		= m_pPos[1];	m_pUV0[6] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[7]		= m_pPos[4];	m_pUV0[7] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);
		pPos[8]		= m_pPos[2];	m_pUV0[8] = D3DXVECTOR2(1.0f,						0.0f);

		pPos[9]		= m_pPos[2];	m_pUV0[9] = D3DXVECTOR2(1.0f,						0.0f);
		pPos[10]	= m_pPos[4];	m_pUV0[10] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);
		pPos[11]	= m_pPos[5];	m_pUV0[11] = D3DXVECTOR2(1.0f,						m_pPos[5].x-m_pPos[2].x);

		pPos[12]	= m_pPos[3];	m_pUV0[12] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[13]	= m_pPos[6];	m_pUV0[13] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[14]	= m_pPos[4];	m_pUV0[14] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);

		pPos[15]	= m_pPos[4];	m_pUV0[15] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);
		pPos[16]	= m_pPos[6];	m_pUV0[16] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[17]	= m_pPos[7];	m_pUV0[17] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	1.0f);

		pPos[18]	= m_pPos[4];	m_pUV0[18] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);
		pPos[19]	= m_pPos[7];	m_pUV0[19] = D3DXVECTOR2(m_pPos[7].z-m_pPos[6].z,	1.0f);
		pPos[20]	= m_pPos[8];	m_pUV0[20] = D3DXVECTOR2(1.0f,						1.0f);

		pPos[21]	= m_pPos[4];	m_pUV0[21] = D3DXVECTOR2(m_pPos[4].z-m_pPos[3].z,	m_pPos[4].x-m_pPos[1].x);
		pPos[22]	= m_pPos[8];	m_pUV0[22] = D3DXVECTOR2(1.0f,						1.0f);
		pPos[23]	= m_pPos[5];	m_pUV0[23] = D3DXVECTOR2(1.0f,						m_pPos[5].x-m_pPos[2].x);

		dwCount = 8;
	}
	else if ( x-(int)x!=0.5f && z-(int)z==0.5f )	//左上右下 \ 移动
	{
		//先取出一些直接可以得到的坐标
		m_pPos[0].x = x - 0.5f;		m_pPos[0].z = z - 0.5f;
		m_pPos[2].x = x - 0.5f;		m_pPos[2].z = z + 0.5f;
		m_pPos[5].x = x + 0.5f;		m_pPos[5].z = z - 0.5f;
		m_pPos[7].x = x + 0.5f;		m_pPos[7].z = z + 0.5f;

		if ( x-(int)x<0.5f )
		{
			m_pPos[8].x = (int)x - 1;	m_pPos[8].z = (int)z;
			m_pPos[9].x = (int)x - 1;	m_pPos[9].z = (int)z + 1;

			m_pPos[3].x = (int)x;		m_pPos[3].z = (int)z;
			m_pPos[4].x = (int)x;		m_pPos[4].z = (int)z + 1;

			m_pPos[10].x = (int)x + 1;	m_pPos[10].z = (int)z;
			m_pPos[11].x = (int)x + 1;	m_pPos[11].z = (int)z + 1;
		} 
		else if ( x-(int)x>0.5f )
		{
			m_pPos[8].x = (int)x;		m_pPos[8].z = (int)z;
			m_pPos[9].x = (int)x;		m_pPos[9].z = (int)z + 1;

			m_pPos[3].x = (int)x + 1;	m_pPos[3].z = (int)z;
			m_pPos[4].x = (int)x + 1;	m_pPos[4].z = (int)z + 1;

			m_pPos[10].x = (int)x + 2;	m_pPos[10].z = (int)z;
			m_pPos[11].x = (int)x + 2;	m_pPos[11].z = (int)z + 1;
		}
		else
		{
			m_pPos[8].x = x - 1;	m_pPos[8].z = (int)z;
			m_pPos[9].x = x - 1;	m_pPos[9].z = (int)z + 1;

			m_pPos[3].x = x;		m_pPos[3].z = (int)z;
			m_pPos[4].x = x;		m_pPos[4].z = (int)z + 1;

			m_pPos[10].x = x + 1;	m_pPos[10].z = (int)z;
			m_pPos[11].x = x + 1;	m_pPos[11].z = (int)z + 1;
		}


		//再计算一些交点
		m_pPos[1] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[2].x, m_pPos[2].z, 
			m_pPos[4].x, m_pPos[4].z, m_pPos[8].x, m_pPos[8].z);

		m_pPos[6] = pGameMap->LineIntersectLine(m_pPos[5].x, m_pPos[5].z, m_pPos[7].x, m_pPos[7].z, 
			m_pPos[3].x, m_pPos[3].z, m_pPos[11].x, m_pPos[11].z);

		for (int i=0; i<12; i++)
		{
			m_pPos[i].y = pGameMap->CalculateHeight(m_pPos[i].x, m_pPos[i].z)+0.04f;
		}

		pPos[0]		= m_pPos[0];	m_pUV0[0] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[1]		= m_pPos[3];	m_pUV0[1] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[2]		= m_pPos[1];	m_pUV0[2] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);

		pPos[3]		= m_pPos[1];	m_pUV0[3] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[4]		= m_pPos[3];	m_pUV0[4] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[5]		= m_pPos[4];	m_pUV0[5] = D3DXVECTOR2(1.0f,						m_pPos[4].x-m_pPos[2].x);

		pPos[6]		= m_pPos[1];	m_pUV0[6] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[7]		= m_pPos[4];	m_pUV0[7] = D3DXVECTOR2(1.0f,						m_pPos[4].x-m_pPos[2].x);
		pPos[8]		= m_pPos[2];	m_pUV0[8] = D3DXVECTOR2(1.0f,						0.0f);

		pPos[9]		= m_pPos[3];	m_pUV0[9] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[10]	= m_pPos[5];	m_pUV0[10] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[11]	= m_pPos[6];	m_pUV0[11] = D3DXVECTOR2(m_pPos[6].z-m_pPos[5].z,	1.0f);

		pPos[12]	= m_pPos[3];	m_pUV0[12] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[13]	= m_pPos[6];	m_pUV0[13] = D3DXVECTOR2(m_pPos[6].z-m_pPos[5].z,	1.0f);
		pPos[14]	= m_pPos[4];	m_pUV0[14] = D3DXVECTOR2(1.0f,						m_pPos[4].x-m_pPos[2].x);

		pPos[15]	= m_pPos[4];	m_pUV0[15] = D3DXVECTOR2(1.0f,						m_pPos[4].x-m_pPos[2].x);
		pPos[16]	= m_pPos[6];	m_pUV0[16] = D3DXVECTOR2(m_pPos[6].z-m_pPos[5].z,	1.0f);
		pPos[17]	= m_pPos[7];	m_pUV0[17] = D3DXVECTOR2(1.0f,						1.0f);

		dwCount = 6;
	}
	else if ( x-(int)x==0.5f && z-(int)z!=0.5f )	//右上左下 / 移动
	{
		//先取出一些直接可以得到的坐标
		m_pPos[0].x = x - 0.5f;		m_pPos[0].z = z - 0.5f;
		m_pPos[2].x = x - 0.5f;		m_pPos[2].z = z + 0.5f;
		m_pPos[5].x = x + 0.5f;		m_pPos[5].z = z - 0.5f;
		m_pPos[7].x = x + 0.5f;		m_pPos[7].z = z + 0.5f;

		if ( z-(int)z>0.5f )
		{
			m_pPos[8].x = (int)x;		m_pPos[8].z = (int)z;
			m_pPos[1].x = (int)x;		m_pPos[1].z = (int)z + 1;
			m_pPos[9].x = (int)x;		m_pPos[9].z = (int)z + 2;

			m_pPos[10].x = (int)x + 1;	m_pPos[10].z = (int)z;
			m_pPos[6].x = (int)x + 1;	m_pPos[6].z = (int)z + 1;
			m_pPos[11].x = (int)x + 1;	m_pPos[11].z = (int)z + 2;
		} 
		else if ( z-(int)z<0.5f )
		{
			m_pPos[8].x = (int)x;		m_pPos[8].z = (int)z - 1;
			m_pPos[1].x = (int)x;		m_pPos[1].z = (int)z;
			m_pPos[9].x = (int)x;		m_pPos[9].z = (int)z + 1;

			m_pPos[10].x = (int)x + 1;	m_pPos[10].z = (int)z - 1;
			m_pPos[6].x = (int)x + 1;	m_pPos[6].z = (int)z;
			m_pPos[11].x = (int)x + 1;	m_pPos[11].z = (int)z + 1;
		}
		else
		{
			m_pPos[8].x = (int)x;		m_pPos[8].z = z - 1;
			m_pPos[1].x = (int)x;		m_pPos[1].z = z;
			m_pPos[9].x = (int)x;		m_pPos[9].z = z + 1;

			m_pPos[10].x = (int)x + 1;	m_pPos[10].z = z - 1;
			m_pPos[6].x = (int)x + 1;	m_pPos[6].z = z;
			m_pPos[11].x = (int)x + 1;	m_pPos[11].z = z + 1;
		}


		//再计算一些交点
		m_pPos[3] = pGameMap->LineIntersectLine(m_pPos[0].x, m_pPos[0].z, m_pPos[5].x, m_pPos[5].z, 
			m_pPos[6].x, m_pPos[6].z, m_pPos[8].x, m_pPos[8].z);

		m_pPos[4] = pGameMap->LineIntersectLine(m_pPos[2].x, m_pPos[2].z, m_pPos[7].x, m_pPos[7].z, 
			m_pPos[1].x, m_pPos[1].z, m_pPos[11].x, m_pPos[11].z);

		for (int i=0; i<12; i++)
		{
			m_pPos[i].y = pGameMap->CalculateHeight(m_pPos[i].x, m_pPos[i].z)+0.04f;
		}

		pPos[0]		= m_pPos[0];	m_pUV0[0] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[1]		= m_pPos[3];	m_pUV0[1] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[2]		= m_pPos[1];	m_pUV0[2] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);

		pPos[3]		= m_pPos[1];	m_pUV0[3] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[4]		= m_pPos[3];	m_pUV0[4] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[5]		= m_pPos[6];	m_pUV0[5] = D3DXVECTOR2(m_pPos[6].z-m_pPos[5].z,	1.0f);

		pPos[6]		= m_pPos[3];	m_pUV0[6] = D3DXVECTOR2(0.0f,						m_pPos[3].x-m_pPos[0].x);
		pPos[7]		= m_pPos[5];	m_pUV0[7] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[8]		= m_pPos[6];	m_pUV0[8] = D3DXVECTOR2(m_pPos[6].z-m_pPos[5].z,	1.0f);

		pPos[9]		= m_pPos[1];	m_pUV0[9] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[10]	= m_pPos[4];	m_pUV0[10] = D3DXVECTOR2(1.0f,						m_pPos[4].x-m_pPos[2].x);
		pPos[11]	= m_pPos[2];	m_pUV0[11] = D3DXVECTOR2(1.0f,						0.0f);

		pPos[12]	= m_pPos[1];	m_pUV0[12] = D3DXVECTOR2(m_pPos[1].z-m_pPos[0].z,	0.0f);
		pPos[13]	= m_pPos[6];	m_pUV0[13] = D3DXVECTOR2(m_pPos[6].z-m_pPos[5].z,	1.0f);
		pPos[14]	= m_pPos[4];	m_pUV0[14] = D3DXVECTOR2(1.0f,						m_pPos[4].x-m_pPos[2].x);

		pPos[15]	= m_pPos[4];	m_pUV0[15] = D3DXVECTOR2(1.0f,						m_pPos[4].x-m_pPos[2].x);
		pPos[16]	= m_pPos[6];	m_pUV0[16] = D3DXVECTOR2(m_pPos[6].z-m_pPos[5].z,	1.0f);
		pPos[17]	= m_pPos[7];	m_pUV0[17] = D3DXVECTOR2(1.0f,						1.0f);

		dwCount = 6;
	}
	else	//站立不动
	{
		//先取出一些直接可以得到的坐标
		m_pPos[0].x = x - 0.5f;		m_pPos[0].z = z - 0.5f;
		m_pPos[1].x = x - 0.5f;		m_pPos[1].z = z + 0.5f;
		m_pPos[2].x = x + 0.5f;		m_pPos[2].z = z - 0.5f;
		m_pPos[3].x = x + 0.5f;		m_pPos[3].z = z + 0.5f;

		for (int i=0; i<4; i++)
		{
			m_pPos[i].y = pGameMap->CalculateHeight(m_pPos[i].x, m_pPos[i].z)+0.04f;
		}

		pPos[0]		= m_pPos[0];	m_pUV0[0] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[1]		= m_pPos[2];	m_pUV0[1] = D3DXVECTOR2(0.0f,						1.0f);
		pPos[2]		= m_pPos[3];	m_pUV0[2] = D3DXVECTOR2(1.0f,						1.0f);

		pPos[3]		= m_pPos[0];	m_pUV0[3] = D3DXVECTOR2(0.0f,						0.0f);
		pPos[4]		= m_pPos[3];	m_pUV0[4] = D3DXVECTOR2(1.0f,						1.0f);
		pPos[5]		= m_pPos[1];	m_pUV0[5] = D3DXVECTOR2(1.0f,						0.0f);

		dwCount = 2;
	}


	/* 下列代码为原代码
	D3DXMATRIX matTranslation, matScale, matWorld;
	//WORD wScaleFactor = max(pShape->GetFigure(CShape::DIR_UP),pShape->GetFigure(CShape::DIR_RIGHT))*2+1;
	//if(pShape->GetAction() == CShape::ACT_DIED || pShape->GetAction()==CShape::ACT_OPEN)
	//{
	//	wScaleFactor=0;
	//}

	D3DXMatrixScaling(&matScale, fScaleFactor, fScaleFactor, fScaleFactor);
	D3DXMatrixTranslation(&matTranslation,x,y,z);
	D3DXMatrixMultiply(&matWorld, &matScale, &matTranslation);

	for(DWORD i = 0; i < 6; ++i)
	{
	D3DXVec3TransformCoord(&pPos[i], &m_pPos[i], &matWorld);

	pPos[i].y = pGameMap->CalculateHeight(pPos[i].x, pPos[i].z)+0.04f;
	float posx = pPos[i].x;
	float posz = pPos[i].z;
	float posy = pPos[i].y;
	}
	*/



	pLayer3D->SendTriangleToCache(m_dwShaderGroupHandle, 0, dwCount, pPos, m_pUV0, m_pColor,dwColorSpecular,1);
}
