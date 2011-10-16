#include "StdAfx.h"
#include "frender.h"
#include "fconsole.h"
#include "futility.h"
#include "fscene.h"

#include "stdafx.h"

#include "futility.h"
#include "fconsole.h"
#include "frender.h"
#include ".\fscene.h"
scene::SkyDome::SkyDome(void):
m_pPosBuffer(NULL),
m_pUV0Buffer(NULL),
m_pColBuffer(NULL)




{
}

scene::SkyDome::~SkyDome(void)
{
	ASSERT(!m_pPosBuffer);
	ASSERT(!m_pUV0Buffer);
	ASSERT(!m_pColBuffer);
}

void scene::SkyDome::Create(const char *pszFileName,int divisions, float PlanetRadius, float AtmosphereRadius,float hTile, float vTile)
{
	// Set the number of divisions into a valid range
	int divs = divisions;
	if (divisions < 1) 
		divs = 1;

	if (divisions > 256) 
		divs = 256; 

	m_fRadius = PlanetRadius;

	// Initialize the Vertex and Indices arrays
	NumPlaneVertices = (divs + 1) * (divs + 1);   // 1 division would give 4 verts
	NumIndices  = divs * divs * 2 * 3;       // 1 division would give 6 indices for 2 tris

	m_pPosBuffer = new D3DXVECTOR3[NumPlaneVertices];
	m_pUV0Buffer = new D3DXVECTOR2[NumPlaneVertices];
	m_pColBuffer = new D3DCOLOR[NumPlaneVertices];

	Indices = new DWORD[NumIndices];
	ZeroMemory(Indices, sizeof(WORD)*NumIndices);

	// Calculate some values we will need
	float plane_size = 2.0f * (float)sqrtf((SQR(AtmosphereRadius)-SQR(PlanetRadius)));
	float delta = plane_size/(float)divs;
	float tex_delta = 2.0f/(float)divs;
	
	// Variables we'll use during the dome's generation
	float x_dist   = 0.0f;
	float z_dist   = 0.0f;
	float x_height = 0.0f;
	float z_height = 0.0f;
	float height = 0.0f;

	int count = 0;

	D3DXVECTOR3 pos;
	D3DXVECTOR2 uv0;
	D3DCOLOR    col;

	for (int i=0;i <= divs;i++)
	{
	    for (int j=0; j <= divs; j++)
		{
			x_dist = (-0.5f * plane_size) + ((float)j*delta);
			z_dist = (-0.5f * plane_size) + ((float)i*delta);

		    x_height = (x_dist*x_dist) / AtmosphereRadius;
			z_height = (z_dist*z_dist) / AtmosphereRadius;
			height = x_height + z_height;

			pos.x = x_dist;
			pos.y = PlanetRadius - height;
			pos.z = z_dist;

			// Calculate the texture coordinates
			uv0.x = hTile*((float)j * tex_delta*0.5f);
			uv0.y = vTile*(1.0f - (float)i * tex_delta*0.5f);

			col = 0xffffffff;

			m_pPosBuffer[i*(divs+1)+j] = pos;
			m_pUV0Buffer[i*(divs+1)+j] = uv0;
			m_pColBuffer[i*(divs+1)+j] = col;
		}
	}


	// Calculate the indices
	int index = 0;
	for (i=0; i < divs;i++)
	{
		for (int j=0; j < divs; j++)
		{
			int startvert = (i*(divs+1) + j);

		    // tri 1
			Indices[index++] = startvert;
			Indices[index++] = startvert+1;
			Indices[index++] = startvert+divs+1;



			// tri 2
			Indices[index++] = startvert+1;
			Indices[index++] = startvert+divs+2;
			Indices[index++] = startvert+divs+1;
		}
	}

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D  = pInterface->GetLayer3D();

	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroupFromFile(pszFileName);


}

void scene::SkyDome::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();







	render::Interface::Layer3D   *pLayer3D  = pInterface->GetLayer3D();

	pInterface->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	D3DXVECTOR3 pos[3];
	D3DXVECTOR2 uv0[3];
	D3DCOLOR    color[3];


	
	for (int i=0; i < NumIndices /3 ; i++)
	{
		uv0[0] = m_pUV0Buffer[Indices[i * 3]];
		uv0[1] = m_pUV0Buffer[Indices[i * 3 + 1]];
		uv0[2] = m_pUV0Buffer[Indices[i * 3 + 2]];


		pos[0] = m_pPosBuffer[Indices[i * 3]];
		pos[1] = m_pPosBuffer[Indices[i * 3 + 1]];
		pos[2] = m_pPosBuffer[Indices[i * 3 + 2]];

		color[0] = m_pColBuffer[Indices[i * 3]];
		color[1] = m_pColBuffer[Indices[i * 3 + 1]];
		color[2] = m_pColBuffer[Indices[i * 3 + 2]];

		pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,0,1,&pos[0],&uv0[0],&uv0[0],&color[0]);
	}
}

void scene::SkyDome::Flow(float fOffset)
{
	for(DWORD n = 0; n < NumPlaneVertices; n++)
	{
		m_pUV0Buffer[n].x += fOffset;
	}
}

void scene::SkyDome::Destroy()
{
	SAFEDELETEARRAY(m_pPosBuffer);
	SAFEDELETEARRAY(m_pUV0Buffer);
	SAFEDELETEARRAY(m_pColBuffer);
}

scene::Skybox::Skybox(void)
{
}

scene::Skybox::~Skybox(void)
{
}

BOOL scene::Skybox::Create(const char *pszFileName,float fWidth,float fHeight,float fDepth,const D3DXVECTOR3 *pCenter)
{
	D3DXVECTOR3 vCenter = (pCenter)? *pCenter : D3DXVECTOR3(0,0,0);

	//           __________
	//         /		  /|
	//        /	 top	 / |
	//       /			/  |                      
	//		 ----------    |                 0 _________3
	// left	|		   |   |right            |\         |
	//		|	  +    |   /	  y   z      |  \       |
	//		|		   |  /	      | /        |    \     |
	//		|	front  | /	    __|/___x     |       \  |
	//		------------          /          1________ \2

	//front 
	float fHalfWidth = fWidth  * 0.5f;
	float fHalfHeight= fHeight * 0.5f;
	float fHalfDepth = fDepth  * 0.5f;

	//Front plane
	m_pPosBuffer[0] = D3DXVECTOR3(-fHalfWidth,fHalfHeight,-fHalfDepth)  + vCenter;
	m_pPosBuffer[1] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight,-fHalfDepth) + vCenter;
	m_pPosBuffer[2] = D3DXVECTOR3( fHalfWidth,-fHalfHeight,-fHalfDepth) + vCenter;
	m_pUV0Buffer[0] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[1] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[2] = D3DXVECTOR2( 0.0f,1.0f);

	m_pPosBuffer[3] = D3DXVECTOR3(-fHalfWidth, fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[4] = D3DXVECTOR3( fHalfWidth,-fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[5] = D3DXVECTOR3( fHalfWidth, fHalfHeight,-fHalfDepth)+ vCenter;
	m_pUV0Buffer[3] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[4] = D3DXVECTOR2( 0.0f,1.0f);
	m_pUV0Buffer[5] = D3DXVECTOR2( 0.0f,0.0f);
	//Back plane
	m_pPosBuffer[6] = D3DXVECTOR3(-fHalfWidth,fHalfHeight, fHalfDepth)+ vCenter;
	m_pPosBuffer[7] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight,fHalfDepth)+ vCenter;
	m_pPosBuffer[8] = D3DXVECTOR3( fHalfWidth,-fHalfHeight,fHalfDepth)+ vCenter;
	m_pUV0Buffer[6] = D3DXVECTOR2( 0.0f,0.0f);
	m_pUV0Buffer[7] = D3DXVECTOR2( 0.0f,1.0f);
	m_pUV0Buffer[8] = D3DXVECTOR2( 1.0f,1.0f);

	m_pPosBuffer[9] = D3DXVECTOR3(-fHalfWidth, fHalfHeight,fHalfDepth) + vCenter;
	m_pPosBuffer[10] = D3DXVECTOR3( fHalfWidth,-fHalfHeight,fHalfDepth)+ vCenter;;
	m_pPosBuffer[11] = D3DXVECTOR3( fHalfWidth, fHalfHeight,fHalfDepth)+ vCenter;
	m_pUV0Buffer[9] = D3DXVECTOR2( 0.0f,0.0f);
	m_pUV0Buffer[10] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[11] = D3DXVECTOR2( 1.0f,0.0f);
	//Top Plane
	m_pPosBuffer[12] = D3DXVECTOR3(-fHalfWidth,fHalfHeight, fHalfDepth)+ vCenter;
	m_pPosBuffer[13] = D3DXVECTOR3(-fHalfWidth,fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[14] = D3DXVECTOR3( fHalfWidth,fHalfHeight,-fHalfDepth)+ vCenter;
	m_pUV0Buffer[12] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[13] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[14] = D3DXVECTOR2( 0.0f,1.0f);

	m_pPosBuffer[15] = D3DXVECTOR3(-fHalfWidth, fHalfHeight,fHalfDepth) + vCenter;
	m_pPosBuffer[16] = D3DXVECTOR3( fHalfWidth, fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[17] = D3DXVECTOR3( fHalfWidth, fHalfHeight,fHalfDepth) + vCenter;
	m_pUV0Buffer[15] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[16] = D3DXVECTOR2( 0.0f,1.0f);
	m_pUV0Buffer[17] = D3DXVECTOR2( 0.0f,0.0f);
	//Bottom Plane
	m_pPosBuffer[18] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight, fHalfDepth)+ vCenter;
	m_pPosBuffer[19] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[20] = D3DXVECTOR3( fHalfWidth,-fHalfHeight,-fHalfDepth)+ vCenter;
	m_pUV0Buffer[18] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[19] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[20] = D3DXVECTOR2( 0.0f,0.0f);

	m_pPosBuffer[21] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight,fHalfDepth) + vCenter;
	m_pPosBuffer[22] = D3DXVECTOR3( fHalfWidth,-fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[23] = D3DXVECTOR3( fHalfWidth,-fHalfHeight,fHalfDepth) + vCenter;
	m_pUV0Buffer[21] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[22] = D3DXVECTOR2( 0.0f,0.0f);
	m_pUV0Buffer[23] = D3DXVECTOR2( 0.0f,1.0f);
	//Left Plane
	m_pPosBuffer[24] = D3DXVECTOR3(-fHalfWidth, fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[25] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[26] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight,fHalfDepth) + vCenter;
	m_pUV0Buffer[24] = D3DXVECTOR2(0.0f,0.0f);
	m_pUV0Buffer[25] = D3DXVECTOR2(0.0f,1.0f);
	m_pUV0Buffer[26] = D3DXVECTOR2(1.0f,1.0f);

	m_pPosBuffer[27] = D3DXVECTOR3(-fHalfWidth, fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[28] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight,fHalfDepth) + vCenter;
	m_pPosBuffer[29] = D3DXVECTOR3(-fHalfWidth,fHalfHeight,fHalfDepth)	+ vCenter;
	m_pUV0Buffer[27] = D3DXVECTOR2( 0.0f,0.0f);
	m_pUV0Buffer[28] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[29] = D3DXVECTOR2( 1.0f,0.0f);

	//Right Plane
	m_pPosBuffer[30] = D3DXVECTOR3(fHalfWidth, fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[31] = D3DXVECTOR3(fHalfWidth,-fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[32] = D3DXVECTOR3(fHalfWidth,-fHalfHeight,fHalfDepth) + vCenter;
	m_pUV0Buffer[30] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[31] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[32] = D3DXVECTOR2( 0.0f,1.0f);

	m_pPosBuffer[33] = D3DXVECTOR3(fHalfWidth, fHalfHeight,-fHalfDepth)+ vCenter;
	m_pPosBuffer[34] = D3DXVECTOR3(fHalfWidth,-fHalfHeight,fHalfDepth) + vCenter;
	m_pPosBuffer[35] = D3DXVECTOR3(fHalfWidth,fHalfHeight,fHalfDepth)  + vCenter;
	m_pUV0Buffer[33] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[34] = D3DXVECTOR2( 0.0f,1.0f);
	m_pUV0Buffer[35] = D3DXVECTOR2( 0.0f,0.0f);


	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroupFromFile(pszFileName);

	return TRUE;
}

void scene::Skybox::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
}

void scene::Skybox::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D  = pInterface->GetLayer3D();

	LPDIRECT3DDEVICE9  pDevice = pInterface->GetDevice();


	pDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	//pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);
	//pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
	
	//Setup Color OP
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//Setup Alpha OP
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	//Setup Color OP
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	D3DCOLOR    color[6] = { 0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff};
	
	D3DXVECTOR3 *pPos = m_pPosBuffer;
	D3DXVECTOR2 *pUV0 = m_pUV0Buffer;

	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,0,2,pPos,pUV0,color);
	pPos += 6;pUV0 += 6;
	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,1,2,pPos,pUV0,color);
	pPos += 6;pUV0 += 6;
	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,2,2,pPos,pUV0,color);
	pPos += 6;pUV0 += 6;
	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,3,2,pPos,pUV0,color);
	pPos += 6;pUV0 += 6;
	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,4,2,pPos,pUV0,color);
	pPos += 6;pUV0 += 6;
	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,5,2,pPos,pUV0,color);
}


scene::SkyPlane::SkyPlane(void) :
m_dwLastTime(0)
{
	m_fSpeed[0] = 0.0f;
	m_fSpeed[1] = 0.0f;
}

scene::SkyPlane::~SkyPlane(void)
{
}

BOOL scene::SkyPlane::Create(const char *pszFileName,float fWidth,float fHeight,float fDepth)
{
	//     0 ___3
	//     | \  |
	//     1__\_2

	//front 
	float fHalfWidth = fWidth  * 0.5f;
	float fHalfDepth = fDepth  * 0.5f;

	//Front plane
	m_pPosBuffer[0] = D3DXVECTOR3(-fHalfWidth,fHeight, fHalfDepth);
	m_pPosBuffer[1] = D3DXVECTOR3(-fHalfWidth,fHeight,-fHalfDepth);
	m_pPosBuffer[2] = D3DXVECTOR3( fHalfWidth,fHeight,-fHalfDepth);
	m_pUV0Buffer[0] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[1] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV0Buffer[2] = D3DXVECTOR2( 0.0f,1.0f);
	m_pUV1Buffer[0] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV1Buffer[1] = D3DXVECTOR2( 1.0f,1.0f);
	m_pUV1Buffer[2] = D3DXVECTOR2( 0.0f,1.0f);

	m_pPosBuffer[3] = D3DXVECTOR3(-fHalfWidth,fHeight, fHalfDepth);
	m_pPosBuffer[4] = D3DXVECTOR3( fHalfWidth,fHeight,-fHalfDepth);
	m_pPosBuffer[5] = D3DXVECTOR3( fHalfWidth,fHeight, fHalfDepth);
	m_pUV0Buffer[3] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV0Buffer[4] = D3DXVECTOR2( 0.0f,1.0f);
	m_pUV0Buffer[5] = D3DXVECTOR2( 0.0f,0.0f);
	m_pUV1Buffer[3] = D3DXVECTOR2( 1.0f,0.0f);
	m_pUV1Buffer[4] = D3DXVECTOR2( 0.0f,1.0f);
	m_pUV1Buffer[5] = D3DXVECTOR2( 0.0f,0.0f);

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroupFromFile(pszFileName);

	return TRUE;
}

void scene::SkyPlane::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
}

void scene::SkyPlane::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D  = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9  pDevice = pInterface->GetDevice();


	pDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

	pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(1,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(1,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	//Setup Color OP
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//Setup Alpha OP
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//Setup Color OP
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD);
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	////Setup Alpha OP
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,0,2,m_pPosBuffer,m_pUV0Buffer,m_pUV1Buffer,m_pColBuffer);
}

void scene::SkyPlane::ProcessFlow(DWORD dwCurrentTime)
{
	DWORD dwElapse = dwCurrentTime - m_dwLastTime;
	
	float fOffset0 = m_fSpeed[0] * (float)dwElapse;
	float fOffset1 = m_fSpeed[1] * (float)dwElapse;

	for(int i = 0; i < 6; i++)
	{
		m_pUV0Buffer[i].y += fOffset0;
		m_pUV1Buffer[i].y += fOffset1;
	}


	m_dwLastTime = dwCurrentTime;
}

void scene::SkyPlane::SetFlowSpeed(DWORD dwLayer,float fSpeed)
{
	m_fSpeed[dwLayer & 0x1] = fSpeed;
}

void scene::SkyPlane::ProcessColor(DWORD dwColor)
{
	for(int i = 0; i < 6; i++)
	{
		m_pColBuffer[i] = dwColor;
	}
}
