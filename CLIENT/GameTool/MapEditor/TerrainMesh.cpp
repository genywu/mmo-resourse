//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月7日 11:28:55
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#include "StdAfx.h"
#include <stdlib.h>

#include "..\..\frostengine\ui.h"
#include "..\..\frostengine\utility.h"

#include "..\..\GameEngine\gameselectray.h"

#include "Map.h"
#include "commui.h"
#include "EditWnd.h"
#include "VisibleSet.h"
#include "terrainmesh.h"
#include "commdb.h"
#include "math.h"
#include "color.h"
#include "setting.h"

//static CONST FLOAT MAX_HEIGHT = 50.0f;

//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::TerrainMesh
// Description     : 
// Return type     : 
// Argument        : void
//---------------------------------------------------------------------------------
TerrainMesh::TerrainMesh(void)
{
	m_dwNumVertex  = 0;
	m_ptVertexList = NULL;

	m_dwNumGrid = 0;
	m_ptGrid    = NULL;

	m_dwShaderGroupHandle = -1;
	m_dwTexSetIndex = -1;

	m_tProcessData.bBrushSize = BS_TINY;
	m_tProcessData.bBrushType = BT_CIRCLE;
	m_tProcessData.nCurrentTextureID = 0;
	m_tProcessData.dwSelectedGridIndex = -1;
	m_tProcessData.dwSelectedVertexIndex = -1;

	m_nDefaultBackTextureID = 0;

	m_bDrawBrushFlag = TRUE;
	m_bIntersectFlag = FALSE;

	srand(timeGetTime());

	QuadVertex = new D3DXVECTOR3[4];
	GridVertex = new D3DXVECTOR3[4];
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::~TerrainMesh
// Description     : 
// Return type     : 
// Argument        : void
//---------------------------------------------------------------------------------
TerrainMesh::~TerrainMesh(void)
{
	ASSERT(!m_ptVertexList);
	ASSERT(!m_ptGrid);
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::CreateShader
// Description     : 创建ShaderHandle
// Return type     : DWORD                : 返回创建的ShaderHandle
// Argument        : TCHAR* pzFilename    : Shader 文件名
//---------------------------------------------------------------------------------
DWORD TerrainMesh::CreateShader(TCHAR* pszFilename)
{
	utility::File fileShader;	
	if (!fileShader.Open(pszFilename))
	{
		MessageBox(NULL,"文件找不到",pszFilename,MB_OK);
		OutputConsole("error: open \"%s\" failed.\n",pszFilename);
		return 0;
	}

	char *pszShaderText = new char [fileShader.GetLength() + 1];
	memcpy(pszShaderText,fileShader.GetData(),fileShader.GetLength());
	pszShaderText[fileShader.GetLength()] = 0;	
	fileShader.Close();

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();

	//get shader group handle


	DWORD dwShaderGroupHandle;
	dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pszShaderText,"terrain\\texture");
	if (dwShaderGroupHandle == 0xFFFFFFFF)
	{
		MessageBox(NULL,"terrain\\texture错误",pszShaderText,MB_OK);
	}

	SAFEDELETE(pszShaderText);

	m_dwShaderGroupHandle = dwShaderGroupHandle;

	return dwShaderGroupHandle;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::CreateTerrain
// Description     : 创建地表网格
// Return type     : BOOL 
// Argument        : DWORD dwWidth
// Argument        : DWORD dwDepth
//---------------------------------------------------------------------------------
BOOL TerrainMesh::CreateTerrain(DWORD dwWidth, DWORD dwDepth)
{
	m_dwWidth = dwWidth;
	m_dwDepth = dwDepth;

	m_dwNumVertex = (dwWidth + 1) * (dwDepth + 1);

	SAFEDELETE(m_ptVertexList);
	m_ptVertexList = new _tagVertex[m_dwNumVertex];		
	ZeroMemory(m_ptVertexList, sizeof(_tagVertex) * m_dwNumVertex);

	//| z|
	//|  |
	//|  0 ----> x
	//+-------
	for(DWORD dwVertexIndex = 0; dwVertexIndex < m_dwNumVertex; dwVertexIndex++)
	{
		float x = (float)(dwVertexIndex % (dwWidth + 1));
		float z = (float)(dwVertexIndex / (dwWidth + 1));

		_tagVertex &tVertex = m_ptVertexList[dwVertexIndex];

		tVertex.dwVertexIndex = dwVertexIndex;
		tVertex.vPosition = D3DXVECTOR3(x, 0.0f, z);
		tVertex.vNormal = D3DXVECTOR3(0.0,1.0f,0.0f);
		tVertex.cColor = 0xffffffff;
		tVertex.cAddColor = 0xffffffff;
		tVertex.byTexID = m_nDefaultBackTextureID;
	}

	m_dwNumGrid = dwWidth * dwDepth;

	SAFEDELETE(m_ptGrid);
	m_ptGrid = new _tagGrid[m_dwNumGrid];
	ZeroMemory(m_ptGrid, sizeof(_tagGrid) * m_dwNumGrid);


	DWORD v0,v1,v2,v3;
	DWORD dwGridIndex, dwIndex;
	for(DWORD z = 0 ; z < dwDepth; z++)
	{
		for(DWORD x = 0 ; x < dwWidth ; x++)
		{
			dwIndex = x + z * dwWidth;

			v0 = z *  (dwWidth + 1)+ x;
			v1 = v0 + 1;
			v2 = v1 + (dwWidth + 1);
			v3 = v2 - 1;

			_tagGrid &tGrid = m_ptGrid[dwIndex];
			tGrid.dwGridIndex = dwIndex;
			tGrid.dwVertexIndex[0] = v0;
			tGrid.dwVertexIndex[1] = v1;
			tGrid.dwVertexIndex[2] = v2;
			tGrid.dwVertexIndex[3] = v3;

			dwGridIndex = m_ptVertexList[v0].dwBelongToNum;
			m_ptVertexList[v0].dwBelongToGrid[dwGridIndex] = dwIndex;
			m_ptVertexList[v0].dwBelongToNum++;

			dwGridIndex = m_ptVertexList[v1].dwBelongToNum;
			m_ptVertexList[v1].dwBelongToGrid[dwGridIndex] = dwIndex;
			m_ptVertexList[v1].dwBelongToNum++;

			dwGridIndex = m_ptVertexList[v2].dwBelongToNum;
			m_ptVertexList[v2].dwBelongToGrid[dwGridIndex] = dwIndex;
			m_ptVertexList[v2].dwBelongToNum++;

			dwGridIndex = m_ptVertexList[v3].dwBelongToNum;
			m_ptVertexList[v3].dwBelongToGrid[dwGridIndex] = dwIndex;
			m_ptVertexList[v3].dwBelongToNum++;

			Code2UV(&tGrid);
		}
	}

	return TRUE;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::Destroy
// Description     : 销毁数据
// Return type     : VOID 
//---------------------------------------------------------------------------------
VOID TerrainMesh::Destroy()
{
	SAFEDELETE(QuadVertex);
	SAFEDELETE(GridVertex);

	SAFEDELETE(m_ptVertexList);
	SAFEDELETE(m_ptGrid);

	if(m_dwShaderGroupHandle >= 0)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
		pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
	}
}

//---------------------------------------------------------------------------------
VOID TerrainMesh::Render(BOOL bDrawNormal,BOOL bShowTexture, render::Camera * pCamera)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();

	pDevice->SetRenderState(D3DRS_FILLMODE,(pEditWnd->GetWireframeMode())? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF ,0x00);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (bShowTexture)
	{
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	}
	else
	{
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
	}

	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	VisibleSet *pSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();

	Grid *pMapGrid(NULL);

	static D3DXVECTOR3 vertex[20000],normal[20000];
	static D3DXVECTOR2 uv0[20000],uv1[20000];
	static DWORD color[20000];
	static _tagVertex pVertex[4];
	static D3DXCOLOR color1, color2;
	static BYTE bUVIndex;

	DWORD *dwIndex;
	int num = 0;
	DWORD lastindex = 0;
	DWORD dwSetCount = pSet->GetVisibleIndexCount();


	// vertex order   uv order 
	// 3----2         0----1 
	// |    |         |    | 
	// |    |         |    | 
	// 0----1         3----2 
	//-----------------------------------------------------------------------------
	for(DWORD i = 0; i < 4; i++)
	{
		for(DWORD n = 0; n< dwSetCount; n++)
		{	
			DWORD &dwNumGrid = pVIArray[n];
			_tagGrid &tGrid = m_ptGrid[dwNumGrid];
			if(tGrid.byTexCode[i] == 0)
			{
				continue;
			}

			pMapGrid = pMap->GetGrid(dwNumGrid);

			if (!pMapGrid->GetVisible())
				continue;
			dwIndex = tGrid.dwVertexIndex;
			for(DWORD j = 0; j < 4; j++)
			{
				pVertex[j] = m_ptVertexList[dwIndex[j]];
			}
			

			BYTE& byTexID = tGrid.byTexIndex[i];
			if (byTexID == 0xF)
			{
				continue;
			}
			render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(m_dwShaderGroupHandle);

			if (byTexID >= pShaderGroup->GetShaderCount())
			{
				byTexID = 0;
			}
			if (lastindex != byTexID)
			{
				pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle, lastindex, 2 * num, vertex, uv0, color, *pCamera->GetViewMatrix(),*pCamera->GetProjectionMatrix());
				num = 0;
			}

			// the first triangle
			vertex[num * 6 + 0] = pVertex[0].vPosition;


			vertex[num * 6 + 1] = pVertex[3].vPosition;


			vertex[num * 6 + 2] = pVertex[2].vPosition;
			

			/*	normal[0].x = pVertex[0].vNormal.x;
			normal[0].y = pVertex[0].vNormal.y;
			normal[0].z = pVertex[0].vNormal.z;

			normal[1].x = pVertex[3].vNormal.x;
			normal[1].y = pVertex[3].vNormal.y;
			normal[1].z = pVertex[3].vNormal.z;

			normal[2].x = pVertex[2].vNormal.x;
			normal[2].y = pVertex[2].vNormal.y;
			normal[2].z = pVertex[2].vNormal.z;*/

			color1 = D3DXCOLOR(pVertex[0].cColor);
			color2 = D3DXCOLOR(pVertex[0].cAddColor);

			color[num * 6 + 0] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[3].cColor);
			color2 = D3DXCOLOR(pVertex[3].cAddColor);
			color[num * 6 + 1] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[2].cColor);
			color2 = D3DXCOLOR(pVertex[2].cAddColor);
			color[num * 6 + 2] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			bUVIndex = tGrid.uv[i];

			uv0[num * 6 + 0].x = CodeUVTable[bUVIndex][6];
			uv0[num * 6 + 0].y = CodeUVTable[bUVIndex][7];

			uv0[num * 6 + 1].x = CodeUVTable[bUVIndex][0];
			uv0[num * 6 + 1].y = CodeUVTable[bUVIndex][1];

			uv0[num * 6 + 2].x = CodeUVTable[bUVIndex][2];
			uv0[num * 6 + 2].y = CodeUVTable[bUVIndex][3];

			// the second triangle
			vertex[num * 6 + 3] = pVertex[0].vPosition;

			vertex[num * 6 + 4] = pVertex[2].vPosition;


			vertex[num * 6 + 5] = pVertex[1].vPosition;


			/*	normal[3].x = pVertex[0].vNormal.x;
			normal[3].y = pVertex[0].vNormal.y;
			normal[3].z = pVertex[0].vNormal.z;

			normal[4].x = pVertex[2].vNormal.x;
			normal[4].y = pVertex[2].vNormal.y;
			normal[4].z = pVertex[2].vNormal.z;

			normal[5].x = pVertex[1].vNormal.x;
			normal[5].y = pVertex[1].vNormal.y;
			normal[5].z = pVertex[1].vNormal.z;*/

			color1 = D3DXCOLOR(pVertex[0].cColor);
			color2 = D3DXCOLOR(pVertex[0].cAddColor);
			color[num * 6 + 3] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[2].cColor);
			color2 = D3DXCOLOR(pVertex[2].cAddColor);
			color[num * 6 + 4] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[1].cColor);
			color2 = D3DXCOLOR(pVertex[1].cAddColor);
			color[num * 6 + 5] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			uv0[num * 6 + 3].x = CodeUVTable[bUVIndex][6];
			uv0[num * 6 + 3].y = CodeUVTable[bUVIndex][7];

			uv0[num * 6 + 4].x = CodeUVTable[bUVIndex][2];
			uv0[num * 6 + 4].y = CodeUVTable[bUVIndex][3];

			uv0[num * 6 + 5].x = CodeUVTable[bUVIndex][4];
			uv0[num * 6 + 5].y = CodeUVTable[bUVIndex][5];
			if (((num * 6 + 5)  > 19990))
			{

				pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle, lastindex, 2 * num, vertex, uv0, color, *pCamera->GetViewMatrix(),*pCamera->GetProjectionMatrix());
				num = -1;
			}
			num++;
			lastindex = byTexID;


		}
		// draw normal
		if (bDrawNormal)
		{
			for(DWORD i = 0; i < 4; i++)
			{

				pLayer3D->_DEBUG_DrawLine(&pVertex[i].vPosition, &(pVertex[i].vPosition+pVertex[i].vNormal), 0xffffff00);
			}
			if (bShowTexture)
			{
				pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			}
			else
			{
				pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
			}

			pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		}
	}
	pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle, lastindex, 2 * num, vertex, uv0, color, *pCamera->GetViewMatrix(),*pCamera->GetProjectionMatrix());
}

void TerrainMesh::RenderAll()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	static D3DXVECTOR3 vertex[6],normal[6];
	static D3DXVECTOR2 uv0[6],uv1[6];
	static DWORD color[6];
	static _tagVertex pVertex[6];
	static D3DXCOLOR color1, color2;
	static BYTE bUVIndex;

	DWORD *dwIndex;
	for(DWORD n = 0; n< m_dwNumGrid; n++)
	{	
		_tagGrid &tGrid = m_ptGrid[n];

		dwIndex = tGrid.dwVertexIndex;

		for(DWORD i = 0; i < 4; i++)
			pVertex[i] = m_ptVertexList[dwIndex[i]];

		// vertex order   uv order 
		// 3----2         0----1 
		// |    |         |    | 
		// |    |         |    | 
		// 0----1         3----2 
		//-----------------------------------------------------------------------------
		for(DWORD i = 0; i < 4; i++)
		{
			if(tGrid.byTexCode[i] == 0)
			{
				break;
			}

			BYTE& byTexID = tGrid.byTexIndex[i];
			render::Interface *pInterface = render::Interface::GetInstance();
			render::Interface::Layer3D* pLayer3D = pInterface->GetLayer3D();
			LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

			render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(m_dwShaderGroupHandle);

			if (byTexID >= pShaderGroup->GetShaderCount())
			{
				byTexID = 0;
			}

			// the first triangle
			vertex[0].x = pVertex[0].vPosition.x;
			vertex[0].y = pVertex[0].vPosition.y;
			vertex[0].z = pVertex[0].vPosition.z;

			vertex[1].x = pVertex[3].vPosition.x;
			vertex[1].y = pVertex[3].vPosition.y;
			vertex[1].z = pVertex[3].vPosition.z;

			vertex[2].x = pVertex[2].vPosition.x;
			vertex[2].y = pVertex[2].vPosition.y;
			vertex[2].z = pVertex[2].vPosition.z;

			normal[0].x = pVertex[0].vNormal.x;
			normal[0].y = pVertex[0].vNormal.y;
			normal[0].z = pVertex[0].vNormal.z;

			normal[1].x = pVertex[3].vNormal.x;
			normal[1].y = pVertex[3].vNormal.y;
			normal[1].z = pVertex[3].vNormal.z;

			normal[2].x = pVertex[2].vNormal.x;
			normal[2].y = pVertex[2].vNormal.y;
			normal[2].z = pVertex[2].vNormal.z;

			color1 = D3DXCOLOR(pVertex[0].cColor);
			color2 = D3DXCOLOR(pVertex[0].cAddColor);
			color[0] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[3].cColor);
			color2 = D3DXCOLOR(pVertex[3].cAddColor);
			color[1] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[2].cColor);
			color2 = D3DXCOLOR(pVertex[2].cAddColor);
			color[2] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			bUVIndex = tGrid.uv[i];

			uv0[0].x = CodeUVTable[bUVIndex][6];
			uv0[0].y = CodeUVTable[bUVIndex][7];

			uv0[1].x = CodeUVTable[bUVIndex][0];
			uv0[1].y = CodeUVTable[bUVIndex][1];

			uv0[2].x = CodeUVTable[bUVIndex][2];
			uv0[2].y = CodeUVTable[bUVIndex][3];

			// the second triangle
			vertex[3].x = pVertex[0].vPosition.x;
			vertex[3].y = pVertex[0].vPosition.y;
			vertex[3].z = pVertex[0].vPosition.z;

			vertex[4].x = pVertex[2].vPosition.x;
			vertex[4].y = pVertex[2].vPosition.y;
			vertex[4].z = pVertex[2].vPosition.z;

			vertex[5].x = pVertex[1].vPosition.x;
			vertex[5].y = pVertex[1].vPosition.y;
			vertex[5].z = pVertex[1].vPosition.z;

			normal[3].x = pVertex[0].vNormal.x;
			normal[3].y = pVertex[0].vNormal.y;
			normal[3].z = pVertex[0].vNormal.z;

			normal[4].x = pVertex[2].vNormal.x;
			normal[4].y = pVertex[2].vNormal.y;
			normal[4].z = pVertex[2].vNormal.z;

			normal[5].x = pVertex[1].vNormal.x;
			normal[5].y = pVertex[1].vNormal.y;
			normal[5].z = pVertex[1].vNormal.z;

			color1 = D3DXCOLOR(pVertex[0].cColor);
			color2 = D3DXCOLOR(pVertex[0].cAddColor);
			color[3] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[2].cColor);
			color2 = D3DXCOLOR(pVertex[2].cAddColor);
			color[4] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			color1 = D3DXCOLOR(pVertex[1].cColor);
			color2 = D3DXCOLOR(pVertex[1].cAddColor);
			color[5] = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));

			uv0[3].x = CodeUVTable[bUVIndex][6];
			uv0[3].y = CodeUVTable[bUVIndex][7];

			uv0[4].x = CodeUVTable[bUVIndex][2];
			uv0[4].y = CodeUVTable[bUVIndex][3];

			uv0[5].x = CodeUVTable[bUVIndex][4];
			uv0[5].y = CodeUVTable[bUVIndex][5];

			pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle, byTexID, 2, vertex, uv0, color);
		}
	}
}

void WriteTGA(UINT uBits, UINT uWidth, UINT uHeight, BYTE *pixels, FILE *pf)
{
	static UINT count = 0;
	BYTE btTGAHeader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	BYTE btHeader[6];
	UINT uBytesPerPixel = 0;
	UINT uImageSize = 0;
	UINT uTemp;

	btHeader[0] = (BYTE)(uWidth % 256);
	btHeader[1] = (BYTE)(uWidth / 256);
	btHeader[2] = (BYTE)(uHeight % 256);
	btHeader[3] = (BYTE)(uHeight / 256);
	btHeader[4] = (BYTE)uBits;
	btHeader[5] = 8;

	uBytesPerPixel = uBits / 8;
	uImageSize = uWidth * uHeight * uBytesPerPixel;

	fwrite(btTGAHeader, 12, sizeof(BYTE), pf);
	fwrite(btHeader, 6, sizeof(BYTE), pf);

	for(UINT i = 0; i < uImageSize; i += uBytesPerPixel)
	{
		uTemp=pixels[i];
		pixels[i] = pixels[i + 2];
		pixels[i + 2] = uTemp;
	}

	fwrite(pixels, uImageSize, sizeof(BYTE), pf);
}

BOOL TerrainMesh::RenderMiniMap(TCHAR *filename)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp)
	{
		MessageBox(NULL,"TerrainMesh::RenderMiniMap: 文件打开失败!", "错误",MB_OK);
		fclose(fp);
		return FALSE;
	}

	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->GetDevice()->EndScene();

	IDirect3DSurface9 *pRenderTarget, *pDepthStencilSurface;
	HRESULT hResult;

	DWORD dwWidth, dwHeight;
	BOOL bUseMapSize = TRUE;

	if(bUseMapSize)
	{
		dwWidth = m_dwWidth;
		dwHeight = m_dwDepth;
	}
	else
	{
		dwWidth = pInterface->GetWndWidth();
		dwHeight = pInterface->GetWndHeight();
	}

	//save old render target
	IDirect3DSurface9* 	pOldRenderTarget;
	IDirect3DSurface9* 	pOldZStencil;

	hResult = pInterface->GetDevice()->GetRenderTarget(0, &pOldRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}
	hResult = pInterface->GetDevice()->GetDepthStencilSurface(&pOldZStencil);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->CreateRenderTarget( dwWidth, 
		dwHeight, 
		D3DFMT_X8R8G8B8, 
		D3DMULTISAMPLE_NONE, 
		0, 
		TRUE, 
		&pRenderTarget, 
		NULL);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->CreateDepthStencilSurface( dwWidth,
		dwHeight, 
		D3DFMT_D16, 
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&pDepthStencilSurface,
		NULL);

	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetRenderTarget(0, pRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetDepthStencilSurface(pDepthStencilSurface);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	pInterface->GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	D3DXVECTOR3 vEyePt( m_dwWidth * 0.5f, 400.0f, -5.0f + m_dwDepth * 0.5f );
	D3DXVECTOR3 vLookatPt( m_dwWidth * 0.5f, 0.0f, m_dwDepth * 0.5f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMATRIXA16 matProj;
	//D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, dwWidth / (FLOAT)dwHeight, 0.1f, 10000.0f );
	D3DXMatrixOrthoLH( &matProj, (FLOAT)dwWidth, (FLOAT)dwHeight, 0.1f, 10000.0f );

	pInterface->GetDevice()->SetTransform( D3DTS_WORLD, &matWorld);
	pInterface->GetDevice()->SetTransform( D3DTS_VIEW, &matView );
	pInterface->GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );

	RenderAll();

	//restore render
	hResult = pInterface->GetDevice()->SetRenderTarget(0, pOldRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetDepthStencilSurface(pOldZStencil);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	D3DSURFACE_DESC dsds;
	hResult = pRenderTarget->GetDesc(&dsds);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	HDC dc;
	hResult = pRenderTarget->GetDC(&dc);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	UINT uBits = 24;
	UINT uWidth = dsds.Width;
	UINT uHeight = dsds.Height;

	UINT uBytesPerPixel = uBits / 8;
	UINT uImageSize = uWidth * uHeight * uBytesPerPixel;

	BYTE *pixels = new BYTE[uImageSize];
	//static BYTE pixels[786432];		// 地图上限为 512 x 512，小地图用24位。
	ZeroMemory(pixels, sizeof(BYTE)*uImageSize);

	for(UINT height = 0; height < uHeight; height++)
	{
		for(UINT width = 0; width < uWidth; width++)
		{
			DWORD color = GetPixel(dc, width, height);
			DWORD count = (height * uWidth + width) * 3;
			pixels[count + 0] =  GetRValue(color); 
			pixels[count + 1] =  GetGValue(color);
			pixels[count + 2] =  GetBValue(color);
		}
	}

	WriteTGA(uBits, uWidth, uHeight, pixels, fp);

	SAFEDELETE(pixels);

	pRenderTarget->ReleaseDC(dc);

	fclose(fp);

	SAFERELEASE(pOldRenderTarget);
	SAFERELEASE(pOldZStencil);

	SAFERELEASE(pRenderTarget);
	SAFERELEASE(pDepthStencilSurface);

	pInterface->GetDevice()->BeginScene();

	return TRUE;
}

//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::ProcessLBDown
// Description     : 判断点击时是否进行纹理处理
// Return type     : VOID
// Argument        : render::Camera *pCamera
//---------------------------------------------------------------------------------
BOOL TerrainMesh::ProcessLBDown(render::Camera *pCamera, BOOL bChangeTex)
{
	if(m_bIntersectFlag)
	{
		if(bChangeTex)
		{
			if(m_tProcessData.nCurrentTextureID != -1)
				return TRUE;
			else
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

////---------------------------------------------------------------------------------
//// Function name   : TerrainMesh::AddColor
//// Description     : 涂色，并可实现颜色明暗随机变化。
//// Return type     : void 
//// Argument        : DWORD dwGridIndex							: 选中的网格索引
//// Argument        : DWORD dwVertexLocation						: 选中的顶点位置
//// Argument        : std::vector<_tagVertex> &vOldVertexArray	: 保存被修改过的数据
//// Argument        : D3DXCOLOR color							: 设置的颜色
//// Argument        : PAINTCOLORBRUSHTYPE eType					: 画刷类型
////---------------------------------------------------------------------------------
//void TerrainMesh::AddColor(DWORD dwGridIndex, DWORD dwVertexLocation, std::vector<_tagVertex> &vOldVertexArray, 
//						   D3DXCOLOR &ForeColor, D3DXCOLOR &BackColor, DWORD dwFlag, PAINTCOLORBRUSHTYPE eType)
//{
//	DWORD &dwVertexIndex = m_ptGrid[dwGridIndex].dwVertexIndex[dwVertexLocation];
//	D3DXVECTOR3 &vCenterVertex = m_ptVertexList[dwVertexIndex].vPosition;
//	INT nCenterRow = dwVertexIndex / (m_dwWidth + 1);
//	INT nCenterColumn = dwVertexIndex % (m_dwWidth + 1);
//
//	BYTE &bBrushSize = m_tProcessData.bBrushSize;
//	for(INT nCountRow = -bBrushSize; nCountRow <= bBrushSize; nCountRow++)
//	{
//		INT nStepRadius, nRadius;
//		switch(m_tProcessData.bBrushType)
//		{
//		case BT_CIRCLE:
//			{
//				nStepRadius = bBrushSize - abs(nCountRow);
//				nRadius = bBrushSize + 1;
//			}
//			break;
//		case BT_QUAD:
//			{
//				nStepRadius = bBrushSize;
//				double temp = (bBrushSize + 1) * (bBrushSize + 1);
//				nRadius = (INT)floor(sqrt(temp * 2));
//			}
//			break;
//		default:
//			break;
//		}
//
//		_tagVertex *pVertex;
//		for(INT nCountColumn = -nStepRadius; nCountColumn <= nStepRadius; nCountColumn++)
//		{
//			INT nCurrentRow = nCenterRow + nCountRow;
//			INT nCurrentColumn = nCenterColumn + nCountColumn;
//			INT nCurrentVertex = nCurrentRow * (m_dwWidth + 1) + nCurrentColumn;
//
//            if(ValidLocation(nCurrentRow, nCurrentColumn, FALSE))
//			{
//				pVertex = &m_ptVertexList[nCurrentVertex];
//				vOldVertexArray.push_back(*pVertex);    // 记录被处理的顶点数据。
//
//				D3DXVECTOR3 vector, vProcessedVertex;
//				D3DXCOLOR color;
//
//				// 判断使用的颜色
//				switch(dwFlag)
//				{
//				case 0:
//					color = ForeColor;
//					break;
//				case 1:
//					color = BackColor;
//					break;
//				case 2:
//					{
//						FLOAT s = (FLOAT)rand() / (FLOAT)RAND_MAX;
//						D3DXColorLerp(&color, &ForeColor, &BackColor, s);
//					}
//					break;
//				default:
//					color.a = color.r = color.g = color.b;
//				}
//
//				if(eType == PCBT_BRUSH)
//				{
//					// 画笔模式要考虑颜色渐淡
//					vProcessedVertex = pVertex->vPosition;
//					vector = vProcessedVertex - vCenterVertex;
//					vector.y = 0.0f;
//					
//					FLOAT fVectorLength = D3DXVec3Length(&vector);
//					FLOAT fScale;
//					
//					if(fVectorLength == 0.0f)
//						fScale = 0.0f;
//					else
//						fScale = fVectorLength / (nRadius - 1.0f);
//
//					D3DXColorLerp(&color, &color, &D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), fScale);
//
//					D3DXCOLOR oldcolor(pVertex->cAddColor);
//					color.a *= oldcolor.a;
//					color.r *= oldcolor.r;
//					color.g *= oldcolor.g;
//					color.b *= oldcolor.b;
//				}
//
//				pVertex->cAddColor = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
//			}
//		}
//	}
//}


//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::RecalculateGridTexture
// Description     : 重新计算网格的纹理
// Return type     : VOID 
// Argument        : DWORD dwGridIndex
// Argument        : DWORD dwVertexLocation
// Argument        : std::vector<_tagVertex> &vOldVertexArray    : 记录被处理的顶点数据
//---------------------------------------------------------------------------------
VOID TerrainMesh::RecalculateGridTexture(DWORD dwGridIndex, DWORD dwVertexLocation, std::vector<_tagVertex> &vOldVertexArray)
{
	DWORD &dwVertexIndex = m_ptGrid[dwGridIndex].dwVertexIndex[dwVertexLocation];
	INT nCenterRow = dwVertexIndex / (m_dwWidth + 1);
	INT nCenterColumn = dwVertexIndex % (m_dwWidth + 1);

	BYTE &bBrushSize = m_tProcessData.bBrushSize;
	for(INT nCountRow = -bBrushSize; nCountRow <= bBrushSize; nCountRow++)
	{
		INT nStepRadius;
		switch(m_tProcessData.bBrushType)
		{
		case BT_CIRCLE:
			nStepRadius = bBrushSize - abs(nCountRow);
			break;
		case BT_QUAD:
			nStepRadius = bBrushSize;
			break;
		default:
			break;
		}

		_tagVertex *pVertex;
		for(INT nCountColumn = -nStepRadius; nCountColumn <= nStepRadius; nCountColumn++)
		{
			INT nCurrentRow = nCenterRow + nCountRow;
			INT nCurrentColumn = nCenterColumn + nCountColumn;
			INT nCurrentVertex = nCurrentRow * (m_dwWidth + 1) + nCurrentColumn;

			if(ValidLocation(nCurrentRow, nCurrentColumn, FALSE))
			{
				pVertex = &m_ptVertexList[nCurrentVertex];
				vOldVertexArray.push_back(*pVertex);    // 记录被处理的顶点数据。

				pVertex->byTexID = m_tProcessData.nCurrentTextureID;
				DWORD dwCount = pVertex->dwBelongToNum;
				for(DWORD i = 0; i < dwCount; i++)
				{
					DWORD dwGridID = pVertex->dwBelongToGrid[i];
					Code2UV(&m_ptGrid[dwGridID]);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::Code2UV
// Description     : 将网格的纹理块编码转换为UV。
// Return type     : VOID 
// Argument        : _tagGrid* pGrid  : 需要处理的网格
//---------------------------------------------------------------------------------
VOID TerrainMesh::Code2UV(_tagGrid* pGrid)
{
	INT nFirstTex = 0;
	BYTE byCount = 0;

	// 逐纹理判断被使用的纹理
	int i;
	for(i = 0; i < 16; i++)
	{
		if(byCount > 3)
			break;

		BOOL bUsed = FALSE;
		// 逐顶点判断。如果使用了纹理，则获取其纹理编码。
		pGrid->byTexIndex[byCount] = 0;
		pGrid->byTexCode[byCount] = 0;
		for(int j = 0; j < 4; j++)
		{
			DWORD dwVertexIndex = pGrid->dwVertexIndex[j];
			if(i == m_ptVertexList[dwVertexIndex].byTexID)
			{
				bUsed = TRUE;
				pGrid->byTexIndex[byCount] = i;
				pGrid->byTexCode[byCount] |= TexCode[j];
			}
		}

		if(bUsed)
			byCount++;
	}

	// 最底层纹理不能使用有缺角的纹理块
	pGrid->byTexCode[0] = 15;

	// 计算该纹理的UV。如果未使用，则不处理。
	for(i = 0; i < 4; i++)
	{
		BYTE byTexCode = pGrid->byTexCode[i];

		if(byTexCode == 0)
			continue;

		if(byTexCode == 15)
			pGrid->uv[i] = GetRandomUV();
		else
			pGrid->uv[i] = byTexCode;
	}
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::GetRandomUV
// Description     : 查表取得随机UV值
// Return type     : BYTE  : 返回UV表的索引
//---------------------------------------------------------------------------------
BYTE TerrainMesh::GetRandomUV()
{
	INT nRand = (INT)(62.0f * rand() / RAND_MAX);

	if(nRand > 46)
		nRand -= 46;
	else
	{
		INT n = nRand % 3;
		switch(n)
		{
		case 0:
			nRand = -15;
			break;
		case 1:
			nRand = 0;
			break;
		case 2:
			nRand = 1;
			break;
		};
	}
	return nRand + 15;
}




//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::SetSelectedTextureID
// Description     : 设定当前选定的纹理
// Return type     : VOID 
// Argument        : INT nTextureID
//---------------------------------------------------------------------------------
VOID TerrainMesh::SetSelectedTextureID(INT nTextureID)
{
	m_tProcessData.nCurrentTextureID = nTextureID;
}




//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::ValidIndex
// Description     : 判断索引是否有效
// Return type     : BOOL 
// Argument        : INT nIndex
//---------------------------------------------------------------------------------
BOOL TerrainMesh::ValidIndex(INT nIndex, BOOL bGridFlag)
{
	if(bGridFlag)
	{
		if(nIndex < 0 || nIndex >= (INT)m_dwNumGrid)
			return FALSE;
	}
	else
	{
		if(nIndex < 0 || nIndex >= (INT)m_dwNumVertex)
			return FALSE;
	}

	return TRUE;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::DrawBrush
// Description     : 画笔刷
// Return type     : VOID 
//---------------------------------------------------------------------------------
VOID TerrainMesh::DrawBrush()
{
	if(!m_bIntersectFlag)
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	render::Interface::Layer3D *pLayer3D   = pInterface->GetLayer3D();

	BYTE &bBrushSize = m_tProcessData.bBrushSize;
	DWORD &dwSelectedGridIndex = m_tProcessData.dwSelectedGridIndex;
	DWORD &dwSelectedVertexIndex = m_tProcessData.dwSelectedVertexIndex;
	DWORD &dwVertexIndex = m_ptGrid[dwSelectedGridIndex].dwVertexIndex[dwSelectedVertexIndex];
	D3DXVECTOR3 &vCenterVertex = m_ptVertexList[dwVertexIndex].vPosition;

	DWORD CenterRow, CenterColumn;
	CenterRow = dwVertexIndex / (m_dwWidth + 1);
	CenterColumn = dwVertexIndex % (m_dwWidth + 1);

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	for(INT nIndexDepth = -bBrushSize; nIndexDepth <= bBrushSize; nIndexDepth ++)
	{
		INT nStepRadius;
		switch(m_tProcessData.bBrushType)
		{
		case BT_CIRCLE:
			nStepRadius = bBrushSize - abs(nIndexDepth);
			break;
		case BT_QUAD:
			nStepRadius = bBrushSize;
			break;
		default:
			break;
		}

		for(INT nIndexWidth = -nStepRadius; nIndexWidth <= nStepRadius; nIndexWidth++)
		{			
			if(!ValidLocation(CenterRow + nIndexDepth, CenterColumn + nIndexWidth, FALSE))
				continue;

			DWORD currentVertexIndex = (CenterRow + nIndexDepth) * (m_dwWidth + 1) + (CenterColumn + nIndexWidth);
			//D3DXVECTOR3 QuadVertex[4], GridVertex[4];
			_tagVertex *pVertex = &m_ptVertexList[currentVertexIndex];
			_tagGrid *pGrid;
			DWORD &dwGridCount = pVertex->dwBelongToNum;
			for(DWORD i = 0; i < dwGridCount; i++)
			{
				DWORD &gridIndex = pVertex->dwBelongToGrid[i];
				pGrid = &m_ptGrid[gridIndex];
				DWORD location;
				for(DWORD j = 0; j < 4; j++)
				{
					DWORD &VertexIndex = pGrid->dwVertexIndex[j];
					GridVertex[j] = m_ptVertexList[VertexIndex].vPosition;
					GridVertex[j].y += 0.01f;

					if(VertexIndex == currentVertexIndex)
						location = j;
				}

				switch(location)
				{
				case BOTTOM_LEFT:
					{
						QuadVertex[0] = GridVertex[0];
						QuadVertex[1] = (GridVertex[1] + GridVertex[0]) * 0.5f;
						QuadVertex[2] = (GridVertex[2] + GridVertex[0]) * 0.5f;
						QuadVertex[3] = (GridVertex[3] + GridVertex[0]) * 0.5f;
					}
					break;
				case BOTTOM_RIGHT:
					{
						QuadVertex[0] = (GridVertex[0] + GridVertex[1]) * 0.5f;
						QuadVertex[1] = GridVertex[1];
						QuadVertex[2] = (GridVertex[2] + GridVertex[1]) * 0.5f;
						QuadVertex[3] = (GridVertex[0] + GridVertex[2]) * 0.5f;
					}
					break;
				case TOP_RIGHT:
					{
						QuadVertex[0] = (GridVertex[0] + GridVertex[2]) * 0.5f;
						QuadVertex[1] = (GridVertex[1] + GridVertex[2]) * 0.5f;
						QuadVertex[2] = GridVertex[2];
						QuadVertex[3] = (GridVertex[3] + GridVertex[2]) * 0.5f;
					}
					break;
				case TOP_LEFT:
					{
						QuadVertex[0] = (GridVertex[0] + GridVertex[3]) * 0.5f;
						QuadVertex[1] = (GridVertex[0] + GridVertex[2]) * 0.5f;
						QuadVertex[2] = (GridVertex[2] + GridVertex[3]) * 0.5f;
						QuadVertex[3] = GridVertex[3];
					}
					break;
				default:
					break;
				}

				pLayer3D->_DEBUG_DrawTriangle(&QuadVertex[0], &QuadVertex[3], &QuadVertex[2], 0x8080ff80);
				pLayer3D->_DEBUG_DrawTriangle(&QuadVertex[0], &QuadVertex[2], &QuadVertex[1], 0x8080ff80);
			}
		}
	}
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::SetBrushSize
// Description     : 设置笔刷类型
// Return type     : VOID 
// Argument        : BYTE bBrushSize
//---------------------------------------------------------------------------------
VOID TerrainMesh::SetBrushSize(BYTE bBrushSize)
{
	m_tProcessData.bBrushSize = bBrushSize;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::SetBrushType
// Description     : 设置笔刷类型
// Return type     : VOID 
// Argument        : BYTE bBrushType
//---------------------------------------------------------------------------------
VOID TerrainMesh::SetBrushType(BYTE bBrushType)
{
	m_tProcessData.bBrushType = bBrushType;
}

//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::ValidLocation
// Description     : 判断行列是否有效
// Return type     : BOOL 
// Argument        : INT nRow
// Argument        : INT nColumn
// Argument        : BOOL bGridFlag    : 标记是否为网格
//---------------------------------------------------------------------------------
BOOL TerrainMesh::ValidLocation(INT nRow, INT nColumn, BOOL bGridFlag)
{
	DWORD dwDepth, dwWidth;
	if(bGridFlag)
	{
		dwDepth = m_dwDepth;
		dwWidth = m_dwWidth;
	}
	else
	{
		dwDepth = m_dwDepth + 1;
		dwWidth = m_dwWidth + 1;
	}

	if(nRow < 0 || nRow >= (INT)dwDepth)
		return FALSE;

	if(nColumn < 0 || nColumn >= (INT)dwWidth)
		return FALSE;

	return TRUE;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::SetDefaultBackTextureID
// Description     : 设置默认的底层纹理ID
// Return type     : INT 
// Argument        : INT nDefaultBackTextureID
//---------------------------------------------------------------------------------
INT TerrainMesh::SetDefaultBackTextureID(INT nDefaultBackTextureID)
{
	m_nDefaultBackTextureID = nDefaultBackTextureID;
	return nDefaultBackTextureID;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::FillVertex
// Description     : 填充指定顶点的数据
// Return type     : VOID 
// Argument        : std::vector<_tagVertex> vOldVertexArray
// Argument        : BOOL bRecalculateTex
//---------------------------------------------------------------------------------
VOID TerrainMesh::FillVertex(std::vector<_tagVertex> vOldVertexArray, BOOL bRecalculateTex)
{
	DWORD dwNumOldVertex = (DWORD)vOldVertexArray.size();
	_tagVertex *pVertex, *pOldVertex;
	for(DWORD i = 0; i < dwNumOldVertex; i++)
	{
		pOldVertex = &vOldVertexArray[i];

		DWORD &dwIndex = pOldVertex->dwVertexIndex;
		pVertex = &m_ptVertexList[dwIndex];
		*pVertex = *pOldVertex;

		DWORD &dwCountNum = pVertex->dwBelongToNum;
		for(DWORD j = 0; j < dwCountNum; j++)
		{
			DWORD &dwGridID = pVertex->dwBelongToGrid[j];
			if(bRecalculateTex)
				Code2UV(&m_ptGrid[dwGridID]);
		}
	}
}




//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::GetProcessData
// Description     : 
// Return type     : VOID 
// Argument        : _tagProcessData* pData
//---------------------------------------------------------------------------------
VOID TerrainMesh::GetProcessData(_tagProcessData* pData)
{
	ASSERT(pData);
	memcpy(pData, &m_tProcessData, sizeof(_tagProcessData));
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::SetProcessData
// Description     : 
// Return type     : VOID 
// Argument        : _tagProcessData* pData
//---------------------------------------------------------------------------------
VOID TerrainMesh::SetProcessData(_tagProcessData* pData)
{
	ASSERT(pData);
	memcpy(&m_tProcessData, pData, sizeof(_tagProcessData));
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::SetDrawBrushFlag
// Description     : 设置是否绘制画刷
// Return type     : BOOL 
// Argument        : BOOL bFlag
//---------------------------------------------------------------------------------
BOOL TerrainMesh::SetDrawBrushFlag(BOOL bFlag)
{
	m_bDrawBrushFlag = bFlag;
	return bFlag;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::ModifyTerrainHeight
// Description     : 
// Return type     : VOID 
// Argument        : DWORD dwGridIndex
// Argument        : DWORD dwVertexIndex
// Argument        : FLOAT fOffsetY    : Y轴偏移量
// Argument        : EDITTYPE et       : 编辑状态
//---------------------------------------------------------------------------------
VOID TerrainMesh::ModifyTerrainHeight(std::vector<_tagVertex> &vOldVertexArray, DWORD dwGridIndex, DWORD dwVertexIndex, FLOAT fOffsetY, EDITTYPE et)
{
	// 取得中心点索引
	DWORD &dwCenterVertexIndex = m_ptGrid[dwGridIndex].dwVertexIndex[dwVertexIndex];
	D3DXVECTOR3 &vCenterVertex = m_ptVertexList[dwCenterVertexIndex].vPosition;
	// 中心顶点的位置
	INT nCenterRow = dwCenterVertexIndex / (m_dwWidth + 1);
	INT nCenterColumn = dwCenterVertexIndex % (m_dwWidth + 1);

	// 逐顶点处理
	BYTE &bBrushSize = m_tProcessData.bBrushSize;
	for(INT nCountRow = -bBrushSize; nCountRow <= bBrushSize; nCountRow++)
	{
		INT nStepRadius, nRadius;
		switch(m_tProcessData.bBrushType)
		{
		case BT_CIRCLE:
			{
				nStepRadius = bBrushSize - abs(nCountRow);
				nRadius = bBrushSize + 1;
			}
			break;
		case BT_QUAD:
			{
				nStepRadius = bBrushSize;
				double temp = (bBrushSize + 1) * (bBrushSize + 1);
				nRadius = (INT)floor(sqrt(temp * 2));
			}
			break;
		default:
			break;
		}
		FLOAT avheight = 0.0f;
		int num = 0;
		if (et == ET_SMOOTH)
		{
			for(INT nCountColumn = -nStepRadius; nCountColumn <= nStepRadius; nCountColumn++)
			{
				INT nCurrentRow = nCenterRow + nCountRow;
				INT nCurrentColumn = nCenterColumn + nCountColumn;

				INT nCurrentVertex = nCurrentRow * (m_dwWidth + 1) + nCurrentColumn;
				//FLOAT fScale, fVectorLength, fTemple;
				D3DXVECTOR3 vector, vProcessedVertex;
				_tagVertex *pVertex;
				if(ValidLocation(nCurrentRow, nCurrentColumn, FALSE))
				{
					pVertex = &m_ptVertexList[nCurrentVertex];
					avheight += pVertex->vPosition.y;
					num++;
				}
			}
			avheight /= (float)num;

		}

		for(INT nCountColumn = -nStepRadius; nCountColumn <= nStepRadius; nCountColumn++)
		{
			INT nCurrentRow = nCenterRow + nCountRow;
			INT nCurrentColumn = nCenterColumn + nCountColumn;

			INT nCurrentVertex = nCurrentRow * (m_dwWidth + 1) + nCurrentColumn;
			FLOAT fScale, fVectorLength, fTemple;
			D3DXVECTOR3 vector, vProcessedVertex;
			_tagVertex *pVertex;
			if(ValidLocation(nCurrentRow, nCurrentColumn, FALSE))
			{
				pVertex = &m_ptVertexList[nCurrentVertex];
				vOldVertexArray.push_back(*pVertex);            // 记录被处理的顶点数据

				fTemple = pVertex->vPosition.y;
				switch(et)
				{
				case ET_ADD:                         // 相加
					{
						vProcessedVertex = pVertex->vPosition;
						vector = vProcessedVertex - vCenterVertex;
						vector.y = 0.0f;
						fVectorLength = D3DXVec3Length(&vector);
						fScale = fOffsetY * (1.0f - fVectorLength / (FLOAT)nRadius);

						fTemple += fScale;
						Clamp(fTemple, Setting::GetInstance()->GetHeightMapRatio());
					}
					break;
				case ET_REPLACE:                     // 替换
					{
						fTemple = m_ptVertexList[dwCenterVertexIndex].vPosition.y;
					}
					break;
				case ET_NOISE:                       // 噪音随机
					{
						fTemple = m_ptVertexList[dwCenterVertexIndex].vPosition.y;
						fScale = (FLOAT)rand() / RAND_MAX - 0.5f;
						fTemple += fScale * 0.5f;
						Clamp(fTemple, Setting::GetInstance()->GetHeightMapRatio());
					}
					break;
				case ET_SMOOTH:                      // 平滑
					{
						fTemple = avheight;
						if(fTemple > 0.0f)
							fTemple -= fTemple * 0.01f;
						else if(fTemple < 0.0f)
							fTemple += abs(fTemple) * 0.01f;
					}
					break;
				default:
					break;
				}

				pVertex->vPosition.y = fTemple;

				// recalculate normal
				CalculateVertexNormal(nCurrentRow, nCurrentColumn);
				INT nVertexIndexTable[6][2] = 
				{
					{nCurrentRow - 1, nCurrentColumn - 1},
					{nCurrentRow - 1, nCurrentColumn    },
					{nCurrentRow    , nCurrentColumn + 1},
					{nCurrentRow + 1, nCurrentColumn + 1},
					{nCurrentRow + 1, nCurrentColumn    },
					{nCurrentRow    , nCurrentColumn - 1},
				};

				for(DWORD i = 0; i < 6; i++)
				{
					INT &nRow = nVertexIndexTable[i][0];
					INT &nColumn = nVertexIndexTable[i][1];

					if(!ValidLocation(nRow, nColumn, FALSE))
						continue;

					CalculateVertexNormal(nRow, nColumn);
				}
			}
		}
	}
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::CalculateGridNormal
// Description     : 计算选定网格的法向
// Return type     : VOID 
// Argument        : DWORD dwGridIndex
//---------------------------------------------------------------------------------
VOID TerrainMesh::CalculateGridNormal(DWORD dwGridIndex)
{
	if(!ValidIndex(dwGridIndex))
		return;

	D3DXVECTOR3 vNormal(0.0f, 0.0f, 0.0f);
	DWORD dwIndexV0, dwIndexV1, dwIndexV2;
	D3DXVECTOR3 vector1, vector2;

	// 第一个三角形
	dwIndexV0 = m_ptGrid[dwGridIndex].dwVertexIndex[2];
	dwIndexV1 = m_ptGrid[dwGridIndex].dwVertexIndex[3];
	dwIndexV2 = m_ptGrid[dwGridIndex].dwVertexIndex[0];

	vector1 = m_ptVertexList[dwIndexV2].vPosition - m_ptVertexList[dwIndexV1].vPosition;
	vector2 = m_ptVertexList[dwIndexV0].vPosition - m_ptVertexList[dwIndexV1].vPosition;
	D3DXVec3Normalize(&vector1, &vector1);
	D3DXVec3Normalize(&vector2, &vector2);
	D3DXVec3Cross(&vNormal, &vector1, &vector2);

	m_ptVertexList[dwIndexV0].vNormal += vNormal;	
	m_ptVertexList[dwIndexV0].vNormal *= 0.5f;
	D3DXVec3Normalize(&m_ptVertexList[dwIndexV0].vNormal, &m_ptVertexList[dwIndexV0].vNormal);

	m_ptVertexList[dwIndexV1].vNormal += vNormal;	
	m_ptVertexList[dwIndexV1].vNormal *= 0.5f;
	D3DXVec3Normalize(&m_ptVertexList[dwIndexV1].vNormal, &m_ptVertexList[dwIndexV1].vNormal);

	m_ptVertexList[dwIndexV2].vNormal += vNormal;	
	m_ptVertexList[dwIndexV2].vNormal *= 0.5f;
	D3DXVec3Normalize(&m_ptVertexList[dwIndexV2].vNormal, &m_ptVertexList[dwIndexV2].vNormal);

	// 第二个三角形
	dwIndexV0 = m_ptGrid[dwGridIndex].dwVertexIndex[0];
	dwIndexV1 = m_ptGrid[dwGridIndex].dwVertexIndex[1];
	dwIndexV2 = m_ptGrid[dwGridIndex].dwVertexIndex[2];

	vector1 = m_ptVertexList[dwIndexV2].vPosition - m_ptVertexList[dwIndexV1].vPosition;
	vector2 = m_ptVertexList[dwIndexV0].vPosition - m_ptVertexList[dwIndexV1].vPosition;
	D3DXVec3Normalize(&vector1, &vector1);
	D3DXVec3Normalize(&vector2, &vector2);
	D3DXVec3Cross(&vNormal, &vector1, &vector2);

	m_ptVertexList[dwIndexV0].vNormal += vNormal;	
	m_ptVertexList[dwIndexV0].vNormal *= 0.5f;
	D3DXVec3Normalize(&m_ptVertexList[dwIndexV0].vNormal, &m_ptVertexList[dwIndexV0].vNormal);

	m_ptVertexList[dwIndexV1].vNormal += vNormal;	
	m_ptVertexList[dwIndexV1].vNormal *= 0.5f;
	D3DXVec3Normalize(&m_ptVertexList[dwIndexV1].vNormal, &m_ptVertexList[dwIndexV1].vNormal);

	m_ptVertexList[dwIndexV2].vNormal += vNormal;	
	m_ptVertexList[dwIndexV2].vNormal *= 0.5f;
	D3DXVec3Normalize(&m_ptVertexList[dwIndexV2].vNormal, &m_ptVertexList[dwIndexV2].vNormal);
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::CalculateVertexNormal
// Description     : 计算指定位置的顶点的法向
// Return type     : VOID 
// Argument        : DWORD row
// Argument        : DWORD column
//---------------------------------------------------------------------------------
VOID TerrainMesh::CalculateVertexNormal(DWORD row, DWORD column)
{
	if(!ValidLocation(row, column, FALSE))
		return;

	D3DXVECTOR3 vNormal(0.0f, 0.0f, 0.0f);
	DWORD dwIndexV0, dwIndexV1, dwIndexV2;
	D3DXVECTOR3 vector1, vector2;
	D3DXVECTOR3 *v0, *v1, *v2;

	INT nVertexIndexTable[6][2] = 
	{
		{row - 1, column - 1},
		{row - 1, column    },
		{row    , column + 1},
		{row + 1, column + 1},
		{row + 1, column    },
		{row    , column - 1},
	};

	dwIndexV1 = row * (m_dwWidth + 1) + column;
	v1 = &m_ptVertexList[dwIndexV1].vPosition;

	FLOAT count = 0;
	for(DWORD i = 0; i < 6; i++)
	{
		if(!ValidLocation(nVertexIndexTable[i][0], nVertexIndexTable[i][1], FALSE))
			continue;

		dwIndexV0 = nVertexIndexTable[i][0] * (m_dwWidth + 1) + nVertexIndexTable[i][1];

		DWORD dwRow, dwColumn;
		if(i + 1 > 5)
		{
			dwRow = nVertexIndexTable[0][0];
			dwColumn = nVertexIndexTable[0][1];
		}
		else
		{
			dwRow = nVertexIndexTable[i+1][0];
			dwColumn = nVertexIndexTable[i+1][1];
		}

		if(!ValidLocation(dwRow, dwColumn, FALSE))
			continue;

		dwIndexV2 = dwRow * (m_dwWidth + 1) + dwColumn;

		v0 = &m_ptVertexList[dwIndexV0].vPosition;
		v2 = &m_ptVertexList[dwIndexV2].vPosition;

		vector1 = *v0 - *v1;
		vector2 = *v2 - *v1;

		D3DXVec3Normalize(&vector1, &vector1);
		D3DXVec3Normalize(&vector2, &vector2);

		D3DXVECTOR3 tempNormal;
		D3DXVec3Cross(&tempNormal, &vector2, &vector1);
		D3DXVec3Normalize(&tempNormal, &tempNormal);

		vNormal += tempNormal;	
		//count++;
		D3DXVec3Normalize(&vNormal, &vNormal);
	}

	//vNormal /= count;
	//D3DXVec3Normalize(&vNormal, &vNormal);
	m_ptVertexList[dwIndexV1].vNormal = vNormal;

	//	_tagVertex *vertex = &m_ptVertexList[dwIndexV1];
	//	vertex->cColor = CalculateDirectionalLighting(&vertex->vNormal, &m_pDirectionalLightDirection);
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::Clamp
// Description     : 将输入的数调整到指定范围内
// Return type     : FLOAT 
// Argument        : FLOAT &num
// Argument        : FLOAT field
//---------------------------------------------------------------------------------
FLOAT TerrainMesh::Clamp(FLOAT &num, FLOAT field)
{
	if(num > field)
		num = field;
	else if(num < -field)
		num = -field;

	return num;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::SaveTerrainMesh
// Description     : 
// Return type     : BOOL 
// Argument        : FILE* pFile
//---------------------------------------------------------------------------------
BOOL TerrainMesh::SaveTerrainMesh(FILE* pFile)
{
	if(!pFile)
		return FALSE;

	fwrite(&m_bDrawBrushFlag, sizeof(BOOL), 1, pFile);
	fwrite(&m_bIntersectFlag, sizeof(BOOL), 1, pFile);
	fwrite(&m_dwTexSetIndex, sizeof(DWORD), 1, pFile);
	fwrite(&m_nDefaultBackTextureID, sizeof(INT), 1, pFile);

	fwrite(&m_dwDepth, sizeof(DWORD), 1, pFile);
	fwrite(&m_dwWidth, sizeof(DWORD), 1, pFile);

	fwrite(&m_dwNumGrid, sizeof(DWORD), 1, pFile);
	if(m_dwNumGrid > 0)
	{
		fwrite(m_ptGrid, sizeof(_tagGrid) * m_dwNumGrid, 1, pFile);
	}
	else
	{
		MessageBox(NULL, "Failed: m_dwNumGrid < 0!", "Error:", MB_OK);
		return FALSE;
	}

	fwrite(&m_dwNumVertex, sizeof(DWORD), 1, pFile);
	if(m_dwNumVertex >= 4)
		fwrite(m_ptVertexList, sizeof(_tagVertex) * m_dwNumVertex, 1, pFile);
	else
	{
		MessageBox(NULL, "Failed: m_dwNumVertex < 4!", "Error:", MB_OK);
		return FALSE;
	}
	fwrite(&m_tProcessData, sizeof(_tagProcessData), 1, pFile);

	return TRUE;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::LoadTerrainMesh
// Description     : 
// Return type     : BOOL 
// Argument        : FILE* pFile
//---------------------------------------------------------------------------------
BOOL TerrainMesh::LoadTerrainMesh(FILE* pFile,bool bnew)
{
	if(!pFile)
		return FALSE;
	if (bnew)
	{
		DWORD Color;
		D3DLIGHT9 LIGHT9;
		D3DMATERIAL9 MATERIAL9;
		// 光照材质信息
		fread(&Color, sizeof(DWORD), 1, pFile);
		fread(&LIGHT9, sizeof(D3DLIGHT9), 1, pFile);
		fread(&MATERIAL9, sizeof(D3DMATERIAL9), 1, pFile);
	}

	fread(&m_bDrawBrushFlag, sizeof(BOOL), 1, pFile);
	fread(&m_bIntersectFlag, sizeof(BOOL), 1, pFile);
	fread(&m_dwTexSetIndex, sizeof(DWORD), 1, pFile);
	fread(&m_nDefaultBackTextureID, sizeof(INT), 1, pFile);

	fread(&m_dwDepth, sizeof(DWORD), 1, pFile);
	fread(&m_dwWidth, sizeof(DWORD), 1, pFile);

	fread(&m_dwNumGrid, sizeof(DWORD), 1, pFile);
	if(m_dwNumGrid < 1)
	{
		MessageBox(NULL, "Failed: m_dwNumGrid < 1!", "Error:", MB_OK);
		return FALSE;
	}

	SAFEDELETE(m_ptGrid);
	m_ptGrid = new _tagGrid[m_dwNumGrid];
	fread(m_ptGrid, sizeof(_tagGrid) * m_dwNumGrid, 1, pFile);

	//_tagGrid *pGrid;
	//D3DXVECTOR2 uv[4][4];
	//for(DWORD i = 0; i < m_dwNumGrid; i++)
	//{
	//	pGrid = &m_ptGrid[i];
	//	fread(&pGrid->dwGridIndex, sizeof(DWORD), 1, pFile);
	//	fread(pGrid->dwVertexIndex, sizeof(DWORD), 4, pFile);
	//	fread(pGrid->byTexCode, sizeof(BYTE), 4, pFile);
	//	fread(pGrid->byTexIndex, sizeof(BYTE), 4, pFile); 
	//	fread(uv, sizeof(D3DXVECTOR2), 4 * 4, pFile);

	//	for(DWORD j = 0; j < 4; j++)
	//	{
	//		for(DWORD k = 0; k < 32; k++)
	//		{
	//			if(uv[j][0].x >= CodeUVTable[k][0] - 0.002 && uv[j][0].x <= CodeUVTable[k][0] + 0.002 &&
	//				uv[j][0].y >= CodeUVTable[k][1] - 0.002 && uv[j][0].y <= CodeUVTable[k][1] + 0.002)
	//				break;
	//		}

	//		if(k > 31)
	//			pGrid->uv[j] = 0;
	//		else
	//			pGrid->uv[j] = (BYTE)k;
	//	}
	//}

	fread(&m_dwNumVertex, sizeof(DWORD), 1, pFile);
	if(m_dwNumVertex < 4)
	{
		MessageBox(NULL, "Failed: m_dwNumVertex < 4!", "Error:", MB_OK);
		return FALSE;
	}

	SAFEDELETE(m_ptVertexList);
	m_ptVertexList = new _tagVertex[m_dwNumVertex];
	fread(m_ptVertexList, sizeof(_tagVertex) * m_dwNumVertex, 1, pFile);


	fread(&m_tProcessData, sizeof(_tagProcessData), 1, pFile);

	TCHAR szShaderPath[_MAX_PATH];
	_snprintf(szShaderPath,_MAX_PATH,"terrain\\set%d\\shader",m_dwTexSetIndex);
	CreateShader(szShaderPath);

	return TRUE;
}

//MODIFY BY JIATAO
BOOL TerrainMesh::ImportTerrainMesh(FILE* pFile)
{
	if(!pFile)
		return FALSE;

	fread(&m_dwTexSetIndex, sizeof(DWORD), 1, pFile);

	fread(&m_dwDepth, sizeof(DWORD), 1, pFile);
	fread(&m_dwWidth, sizeof(DWORD), 1, pFile);

	fread(&m_dwNumGrid, sizeof(DWORD), 1, pFile);
	if(m_dwNumGrid < 1)
	{
		MessageBox(NULL, "Failed: m_dwNumGrid < 1!", "Error:", MB_OK);
		return FALSE;
	}

	SAFEDELETE(m_ptGrid);
	m_ptGrid = new _tagGrid[m_dwNumGrid];
	fread(m_ptGrid, sizeof(_tagGrid) * m_dwNumGrid, 1, pFile);

	fread(&m_dwNumVertex, sizeof(DWORD), 1, pFile);
	if(m_dwNumVertex < 4)
	{
		MessageBox(NULL, "Failed: m_dwNumVertex < 4!", "Error:", MB_OK);
		return FALSE;
	}

	SAFEDELETE(m_ptVertexList);
	m_ptVertexList = new _tagVertex[m_dwNumVertex];
	fread(m_ptVertexList, sizeof(_tagVertex) * m_dwNumVertex, 1, pFile);

	TCHAR szShaderPath[_MAX_PATH];
	_snprintf(szShaderPath,_MAX_PATH,"terrain\\set%d\\shader",m_dwTexSetIndex);
	CreateShader(szShaderPath);

	return TRUE;
}


BOOL TerrainMesh::CalculateIntersect()
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	//计算选中的格子
	static GameSelectRay cSelRay;
	RECT rc;
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	pEditWnd->GetWndRect(&rc);
	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	VisibleSet *pSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();

	m_tProcessData.dwSelectedGridIndex = -1;
	m_tProcessData.dwSelectedVertexIndex = -1;

	DWORD *dwIndex;
	m_bIntersectFlag = FALSE;

	//unsigned __int64 t0 = RDTSC();
	//unsigned __int64 t1 = RDTSC();
	//unsigned __int64 tb = t1 - t0;
	FLOAT fMinDis = 1000;
	FLOAT fDis;
	DWORD dwSetCount = pSet->GetVisibleIndexCount();
	for(DWORD n = 0; n< dwSetCount; n++)
	{	
		DWORD &dwNumGrid = pVIArray[n];
		dwIndex = m_ptGrid[dwNumGrid].dwVertexIndex;

		// 分别从网格的四个顶点得到四个正方形，再判断鼠标移近哪个顶点。		
		for(DWORD i = 0; i < 4; i++)
			GridVertex[i] = m_ptVertexList[dwIndex[i]].vPosition;

		for(DWORD i = 0; i < 4; i++)
		{
			switch(/*location*/i)
			{
			case BOTTOM_LEFT:
				{
					QuadVertex[0].x = GridVertex[0].x;
					QuadVertex[0].y = GridVertex[0].y;
					QuadVertex[0].z = GridVertex[0].z;

					QuadVertex[1].x = GridVertex[1].x;
					QuadVertex[1].y = GridVertex[1].y;
					QuadVertex[1].z = GridVertex[1].z;
					QuadVertex[1].x += GridVertex[0].x;
					QuadVertex[1].y += GridVertex[0].y;
					QuadVertex[1].z += GridVertex[0].z;
					QuadVertex[1] *= 0.5f;

					QuadVertex[3].x = GridVertex[2].x;
					QuadVertex[3].y = GridVertex[2].y;
					QuadVertex[3].z = GridVertex[2].z;
					QuadVertex[3].x += GridVertex[0].x;
					QuadVertex[3].y += GridVertex[0].y;
					QuadVertex[3].z += GridVertex[0].z;
					QuadVertex[3] *= 0.5f;

					QuadVertex[2].x = GridVertex[3].x;
					QuadVertex[2].y = GridVertex[3].y;
					QuadVertex[2].z = GridVertex[3].z;
					QuadVertex[2].x += GridVertex[0].x;
					QuadVertex[2].y += GridVertex[0].y;
					QuadVertex[2].z += GridVertex[0].z;
					QuadVertex[2] *= 0.5f;

					//QuadVertex[1] = (GridVertex[1] + GridVertex[0]) * 0.5f;
					//QuadVertex[3] = (GridVertex[2] + GridVertex[0]) * 0.5f;
					//QuadVertex[2] = (GridVertex[3] + GridVertex[0]) * 0.5f;
				}
				break;
			case BOTTOM_RIGHT:
				{
					QuadVertex[0].x = GridVertex[1].x;
					QuadVertex[0].y = GridVertex[1].y;
					QuadVertex[0].z = GridVertex[1].z;
					QuadVertex[0].x += GridVertex[0].x;
					QuadVertex[0].y += GridVertex[0].y;
					QuadVertex[0].z += GridVertex[0].z;
					QuadVertex[0] *= 0.5f;

					QuadVertex[1].x = GridVertex[1].x;
					QuadVertex[1].y = GridVertex[1].y;
					QuadVertex[1].z = GridVertex[1].z;

					QuadVertex[3].x = GridVertex[2].x;
					QuadVertex[3].y = GridVertex[2].y;
					QuadVertex[3].z = GridVertex[2].z;
					QuadVertex[3].x += GridVertex[1].x;
					QuadVertex[3].y += GridVertex[1].y;
					QuadVertex[3].z += GridVertex[1].z;
					QuadVertex[3] *= 0.5f;

					QuadVertex[2].x = GridVertex[2].x;
					QuadVertex[2].y = GridVertex[2].y;
					QuadVertex[2].z = GridVertex[2].z;
					QuadVertex[2].x += GridVertex[0].x;
					QuadVertex[2].y += GridVertex[0].y;
					QuadVertex[2].z += GridVertex[0].z;
					QuadVertex[2] *= 0.5f;

					//QuadVertex[0] = (GridVertex[0] + GridVertex[1]) * 0.5f;
					//QuadVertex[1] = GridVertex[1];
					//QuadVertex[3] = (GridVertex[2] + GridVertex[1]) * 0.5f;
					//QuadVertex[2] = (GridVertex[0] + GridVertex[2]) * 0.5f;
				}
				break;
			case TOP_RIGHT:
				{
					QuadVertex[0].x = GridVertex[2].x;
					QuadVertex[0].y = GridVertex[2].y;
					QuadVertex[0].z = GridVertex[2].z;
					QuadVertex[0].x += GridVertex[0].x;
					QuadVertex[0].y += GridVertex[0].y;
					QuadVertex[0].z += GridVertex[0].z;
					QuadVertex[0] *= 0.5f;

					QuadVertex[1].x = GridVertex[2].x;
					QuadVertex[1].y = GridVertex[2].y;
					QuadVertex[1].z = GridVertex[2].z;
					QuadVertex[1].x += GridVertex[1].x;
					QuadVertex[1].y += GridVertex[1].y;
					QuadVertex[1].z += GridVertex[1].z;
					QuadVertex[1] *= 0.5f;

					QuadVertex[3].x = GridVertex[2].x;
					QuadVertex[3].y = GridVertex[2].y;
					QuadVertex[3].z = GridVertex[2].z;

					QuadVertex[2].x = GridVertex[3].x;
					QuadVertex[2].y = GridVertex[3].y;
					QuadVertex[2].z = GridVertex[3].z;
					QuadVertex[2].x += GridVertex[2].x;
					QuadVertex[2].y += GridVertex[2].y;
					QuadVertex[2].z += GridVertex[2].z;
					QuadVertex[2] *= 0.5f;

					//QuadVertex[0] = (GridVertex[0] + GridVertex[2]) * 0.5f;
					//QuadVertex[1] = (GridVertex[1] + GridVertex[2]) * 0.5f;
					//QuadVertex[3] = GridVertex[2];
					//QuadVertex[2] = (GridVertex[3] + GridVertex[2]) * 0.5f;
				}
				break;
			case TOP_LEFT:
				{
					QuadVertex[0].x = GridVertex[3].x;
					QuadVertex[0].y = GridVertex[3].y;
					QuadVertex[0].z = GridVertex[3].z;
					QuadVertex[0].x += GridVertex[0].x;
					QuadVertex[0].y += GridVertex[0].y;
					QuadVertex[0].z += GridVertex[0].z;
					QuadVertex[0] *= 0.5f;

					QuadVertex[1].x = GridVertex[2].x;
					QuadVertex[1].y = GridVertex[2].y;
					QuadVertex[1].z = GridVertex[2].z;
					QuadVertex[1].x += GridVertex[0].x;
					QuadVertex[1].y += GridVertex[0].y;
					QuadVertex[1].z += GridVertex[0].z;
					QuadVertex[1] *= 0.5f;

					QuadVertex[3].x = GridVertex[3].x;
					QuadVertex[3].y = GridVertex[3].y;
					QuadVertex[3].z = GridVertex[3].z;
					QuadVertex[3].x += GridVertex[2].x;
					QuadVertex[3].y += GridVertex[2].y;
					QuadVertex[3].z += GridVertex[2].z;
					QuadVertex[3] *= 0.5f;

					QuadVertex[2].x = GridVertex[3].x;
					QuadVertex[2].y = GridVertex[3].y;
					QuadVertex[2].z = GridVertex[3].z;

					//QuadVertex[0] = (GridVertex[0] + GridVertex[3]) * 0.5f;
					//QuadVertex[1] = (GridVertex[0] + GridVertex[2]) * 0.5f;
					//QuadVertex[3] = (GridVertex[2] + GridVertex[3]) * 0.5f;
					//QuadVertex[2] = GridVertex[3];
				}
				break;
			default:
				break;
			}

			if(cSelRay.IntersectTri(&QuadVertex[0],NULL,&fDis) || cSelRay.IntersectTri(&QuadVertex[1],NULL,&fDis))
			{
				m_bIntersectFlag = TRUE;
				//vCenterVertex = vertex;

				// 只选择最前面的顶点
				if(fDis < fMinDis)
				{
					fMinDis = fDis;
					m_tProcessData.dwSelectedGridIndex = /*gridIndex*/dwNumGrid;
					m_tProcessData.dwSelectedVertexIndex = /*location*/i;
				}
			}
		}

		//if(m_bIntersectFlag)
		//	return m_bIntersectFlag;
	}

	return m_bIntersectFlag;
}



//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::GetGrid
// Description     : 返回指定的
// Return type     : TerrainMesh::_tagGrid * 
// Argument        : DWORD dwGridIndex
//---------------------------------------------------------------------------------
TerrainMesh::_tagGrid * TerrainMesh::GetGrid(DWORD dwGridIndex) 
{ 
	if(ValidIndex(dwGridIndex))
		return &m_ptGrid[dwGridIndex];
	else
		return NULL;
}
//---------------------------------------------------------------------------------
// Function name   : TerrainMesh::GetGridCenter
// Description     : 返回指定的
// Return type     : void
// Argument        : DWORD dwGridIndex
//---------------------------------------------------------------------------------
void TerrainMesh::GetGridCenter(DWORD dwGridIndex,D3DXVECTOR3 *pCenter)
{
	_tagGrid &grid = m_ptGrid[dwGridIndex];
	static D3DXVECTOR3 pos[4];
	pos[0] = m_ptVertexList[grid.dwVertexIndex[0]].vPosition;
	pos[1] = m_ptVertexList[grid.dwVertexIndex[1]].vPosition;
	pos[2] = m_ptVertexList[grid.dwVertexIndex[2]].vPosition;
	pos[3] = m_ptVertexList[grid.dwVertexIndex[3]].vPosition;

	*pCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;
}

DWORD TerrainMesh::GetGridColor(DWORD dwGridIndex)
{
	_tagGrid &grid = m_ptGrid[dwGridIndex];
	static DWORD color[4],colorAdd[4];
	static D3DXCOLOR color1,color2;
	color[0] = m_ptVertexList[grid.dwVertexIndex[0]].cColor;
	color[1] = m_ptVertexList[grid.dwVertexIndex[1]].cColor;
	color[2] = m_ptVertexList[grid.dwVertexIndex[2]].cColor;
	color[3] = m_ptVertexList[grid.dwVertexIndex[3]].cColor;

	colorAdd[0] = m_ptVertexList[grid.dwVertexIndex[0]].cAddColor;
	colorAdd[1] = m_ptVertexList[grid.dwVertexIndex[1]].cAddColor;
	colorAdd[2] = m_ptVertexList[grid.dwVertexIndex[2]].cAddColor;
	colorAdd[3] = m_ptVertexList[grid.dwVertexIndex[3]].cAddColor;

	color1 = D3DXCOLOR(color[0]);
	color2 = D3DXCOLOR(colorAdd[0]);
	return D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
		(DWORD)(255 * color1.r * color2.r), 
		(DWORD)(255 * color1.g * color2.g), 
		(DWORD)(255 * color1.b * color2.b));
}


TerrainMesh::_tagVertex* TerrainMesh::GetVertex(DWORD dwVertexIndex)
{
	return &m_ptVertexList[dwVertexIndex];
}


// 从高度图得到地形数据
void TerrainMesh::ImportHeightMap(TCHAR *filename)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Texture *pTexture;
	DWORD dwTexHandle = 0xffffffff;
	DWORD texWidth, texDepth;

	dwTexHandle = pInterface->CreateTexture(filename);
	ASSERT(dwTexHandle != 0xffffffff);

	pTexture = pInterface->GetTexture(dwTexHandle);
	texWidth = pTexture->GetWidth();
	texDepth = pTexture->GetHeight();
	for(DWORD width = 0; width <= m_dwWidth; width++)
	{
		for(DWORD depth = 0; depth <= m_dwDepth; depth++)
		{
			DWORD color;
			DWORD column, row;
			FLOAT factor;

			if(width >= texWidth - 1)
				column = texWidth - 1;
			else
				column = width;

			if(depth >= texDepth - 1)
				row = texDepth - 1;
			else
				row = depth;


			color = pTexture->GetPixel(column, texDepth - 1 - row);
			factor = (FLOAT)(color & 0xff);
			factor = (factor - 128.0f) / 128.0f;
			m_ptVertexList[width + depth * (m_dwWidth + 1)].vPosition.y = factor * Setting::GetInstance()->GetHeightMapRatio();
		}
	}

	// recalculate normal
	for(DWORD width = 0; width < m_dwWidth; width++)
	{
		for(DWORD depth = 0; depth < m_dwDepth; depth++)
		{
			CalculateVertexNormal(depth, width);
		}
	}

	pInterface->DestroyTexture(dwTexHandle);
}

// 将地形数据导出为高度图
void TerrainMesh::ExportHeightMap(TCHAR *filename)
{
	FILE *fp = fopen(filename, "wb");

	// format : RGB
	BYTE *pixels = new BYTE[m_dwNumVertex * 3];
	for(DWORD i = 0; i < m_dwNumVertex; i++)
	{
		BYTE value = (BYTE)((m_ptVertexList[i].vPosition.y / Setting::GetInstance()->GetHeightMapRatio()) * 128 + 128);
		pixels[i*3] = pixels[i*3+1] = pixels[i*3+2] = value;
	}

	WriteTGA(24, m_dwWidth + 1, m_dwDepth + 1, pixels, fp);

	delete pixels;

	fclose(fp);
}

// 从贴图中取色，并应用到地形上。
void TerrainMesh::ApplyColorMap(TCHAR *filename)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Texture *pTexture;
	DWORD dwTexHandle = 0xffffffff;
	DWORD texWidth, texDepth;

	dwTexHandle = pInterface->CreateTexture(filename);
	ASSERT(dwTexHandle != 0xffffffff);

	pTexture = pInterface->GetTexture(dwTexHandle);
	texWidth = pTexture->GetWidth();
	texDepth = pTexture->GetHeight();
	for(DWORD width = 0; width <= m_dwWidth; width++)
	{
		for(DWORD depth = 0; depth <= m_dwDepth; depth++)
		{
			DWORD color;
			DWORD column, row;

			if(width >= texWidth - 1)
				column = texWidth - 1;
			else
				column = width;

			if(depth >= texDepth - 1)
				row = texDepth - 1;
			else
				row = depth;


			color = pTexture->GetPixel(column, texDepth - 1 - row);
			m_ptVertexList[width + depth * (m_dwWidth + 1)].cAddColor = color;
		}
	}

	pInterface->DestroyTexture(dwTexHandle);
}

void TerrainMesh::ExportColorMap(const char * pszFileName)
{
	FILE *fp = fopen(pszFileName,"wb");
	if (!fp)
	{
		MessageBox(NULL,"打开文件失败!","错误",MB_OK);
		return;
	}

	DWORD texWidth = m_dwWidth + 1;
	DWORD texDepth = m_dwDepth + 1;

	BYTE *pixels = new BYTE[m_dwNumVertex * 3];

	for(DWORD width = 0; width <= m_dwWidth; width++)
	{
		for(DWORD depth = 0; depth <= m_dwDepth; depth++)
		{
			DWORD color;
			DWORD column, row;

			if(width >= texWidth - 1)
				column = texWidth - 1;
			else
				column = width;

			if(depth >= texDepth - 1)
				row = texDepth - 1;
			else
				row = depth;

			int i = column + row * texWidth;
			color = m_ptVertexList[width + depth * (m_dwWidth + 1)].cAddColor;
			pixels[i*3]   = (BYTE)((color & 0x00FF0000) >> 16);
			pixels[i*3+1] = (BYTE)((color & 0x0000FF00) >> 8);
			pixels[i*3+2] = (BYTE)(color & 0x000000FF);			
		}
	}

	WriteTGA(24, m_dwWidth + 1, m_dwDepth + 1, pixels, fp);

	delete pixels;
	fclose(fp);
}



