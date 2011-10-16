#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\GameEngine\gamebuildlist.h"
#include "..\..\GameEngine\gameselectray.h"
#include "..\..\GameEngine\displaymodel.h"

#include "CommUI.h"
#include "visibleset.h"
#include "map.h"
#include "mtledittool.h"
#include "editwnd.h"
#include "blockedittool.h"
#include "toolsecurityedit.h"
#include "toolwaredit.h"
#include "tooldooredit.h"
#include "toolbuildedit.h"
#include "changewnd.h"
#include "map.h"
#include "terrainmesh.h"
#include "commdb.h"
#include "terrainmesh.h"
#include "color.h"
#include "setting.h"

EditMap::EditMap(void)
{
	m_pGridArray = NULL;
	m_pQTRoot = NULL;
	m_dwSunLightAmbientColor = AMBIENTCOLOR;
	m_dwSunLightDiffuseColor = DIRECTIONALCOLOR;
	m_dwTotalPalacedBuildCount = 0;
	m_vSunLightDirection = D3DXVECTOR3(-0.70710671f,-0.70710671f,0.00000000f);	
	m_fSunLightIntensitisy = 1.0f;
	m_bEnableFog = FALSE;
	
}

EditMap::~EditMap(void)
{
	ASSERT(!m_pGridArray);
	ASSERT(!m_pQTRoot);
}

void EditMap::Create(int iWidth,int iDepth)
{
	//建立格子
	m_iWidth = iWidth;
	m_iDepth = iDepth;

	m_pGridArray = new Grid[iWidth * iDepth];
	//ZeroMemory(m_pGridArray,sizeof(Grid) * iWidth * iDepth);
	int i = sizeof(GameModel::AnimInfo);

	Grid::tagBuild tBuild;
	tBuild.wID = 0xFFFF;
	tBuild.fRotationY = 0.0f;
	tBuild.fHeight  = 0.0f;
	for(DWORD n = 0; n < (DWORD)(iWidth * iDepth); n ++)
	{
		m_pGridArray[n].SetBuild(tBuild);
		m_pGridArray[n].SetVisible(TRUE);
		m_pGridArray[n].SetTargetHideGridIndex(0xFFFFFFFF);
		ZeroMemory(m_pGridArray[n].m_cBuildModelAnimInfo.GetActionInfo(),sizeof(GameModel::AnimInfo::tagActionInfo));
		m_pGridArray[n].m_cBuildModelAnimInfo.GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
		m_pGridArray[n].m_cBuildModelAnimInfo.GetActionInfo()->bCurActionLooped = TRUE;
		m_pGridArray[n].m_cBuildModelAnimInfo.SetAnimTime(timeGetTime());
	}
	
	//可见集合
	m_pVisibleSet = new VisibleSet;
	m_pVisibleSet->Create(this);

	//全局动画信息
	m_pGlobalAnimInfo = new AnimInfo;
	m_pGlobalAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
	m_pGlobalAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
	m_pGlobalAnimInfo->SetAnimTime(timeGetTime());
}

void EditMap::Destroy()
{
	SAFEDELETE(m_pGlobalAnimInfo);
	SAFEDESTROY(m_pVisibleSet);
	SAFEDELETEARRAY(m_pGridArray);
}

void EditMap::Update(render::Frustum *pFrustum)
{
	m_pVisibleSet->Reset();
	//计算起始
	const D3DXVECTOR3 *pVertexBuffer = pFrustum->GetCornerVertex();

	//求xmin xmax zmin zmax

	float xmin = pVertexBuffer[0].x;
	float xmax = xmin;
	float zmin = pVertexBuffer[0].z;
	float zmax = zmin;
	for(DWORD n = 0; n < 8; n++)
	{
		const D3DXVECTOR3 &vtx = pVertexBuffer[n];
		if (vtx.x < xmin) xmin = vtx.x;
		if (vtx.z < zmin) zmin = vtx.z;

		if (vtx.x > xmax) xmax = vtx.x;
		if (vtx.z > zmax) zmax = vtx.z;
	}
	
	//起始
	int iIndex,iTotalGrid = m_iDepth * m_iWidth;

	int iXMin = (xmin < 0.0f)? 0 : int(xmin);
	int iZMin = (zmin < 0.0f)? 0 : int(zmin);
	int iXMax = (xmax >= (float)m_iWidth)? m_iWidth - 1 : (int)xmax;
	int iZMax = (zmax >= (float)m_iDepth)? m_iDepth - 1 : (int)zmax;
	
	for(int x = iXMin; x <= iXMax;x++)
		for(int z = iZMin;z <= iZMax; z++)
		{
			iIndex = x + z * m_iWidth;
			m_pVisibleSet->SetVisible(iIndex);
		}

	//计算环境光+平行光 
	CalculateSunLighting();
	//计算建筑点光源
	CalculateLightSourceLighting();
}

void EditMap::CalculateSunLighting(void)
{
	DWORD *pVIArray = m_pVisibleSet->GetVisibleIndexArray();
	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	
	DWORD dwAR = (m_dwSunLightAmbientColor & 0x00FF0000) >> 16;
	DWORD dwAG = (m_dwSunLightAmbientColor & 0x0000FF00) >> 8;
	DWORD dwAB = m_dwSunLightAmbientColor & 0x000000FF;
	DWORD r,g,b;

	float fR = (float)( (m_dwSunLightDiffuseColor & 0x00FF0000) >> 16) / 255.0f;
	float fG = (float)( (m_dwSunLightDiffuseColor & 0x0000FF00) >>  8) / 255.0f;
	float fB = (float)(  m_dwSunLightDiffuseColor & 0x000000FF)        / 255.0f;

	TerrainMesh::_tagVertex *pVertex,*pVertexList = pTerrainMesh->GetVertexList();
	TerrainMesh::_tagGrid   *pGrid; 

	for(DWORD n = 0; n< m_pVisibleSet->GetVisibleIndexCount(); n++)
	{
		pGrid = pTerrainMesh->GetGrid(pVIArray[n]);
		for(DWORD v = 0; v < 4; v++)
		{
			//取得顶点
			pVertex = &pVertexList[pGrid->dwVertexIndex[v]];

			//计算光照			
			r = dwAR;g = dwAG;b = dwAB;
			float fDOT = - D3DXVec3Dot(&m_vSunLightDirection,&pVertex->vNormal);// L . N
			
			if (fDOT > 0)//光线在面的正面
			{
				float fValue = 1.0f * fDOT;
				r += (DWORD)(( fR * fValue) * 255.0f);  	
				if (r > 255) r = 0x000000FF;
				g += (DWORD)(( fG * fValue) * 255.0f);  	
				if (g > 255) g = 0x000000FF;
				b += (DWORD)(( fB * fValue) * 255.0f);  	
				if (b > 255) b = 0x000000FF;				
			}
			pVertex->cColor = 0xFF000000 | (r << 16) | (g << 8) | b;
		}
	}
}

void EditMap::Render(DWORD dwRenderFlag, render::Camera * pCamera, render::Frustum * pFrustum)
{	
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	VisibleSet *pSet = GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();
	std::vector<int>::iterator itFind;
	Grid *pMapGrid(NULL);
    TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pTerrainMesh->GetVertexList();
	static D3DXVECTOR3 pos[4];
	static D3DXVECTOR3 WireLinePos[20000];
	static D3DXVECTOR3 vCenter;
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	const D3DXMATRIX *pBillBoardMatrix = pEditWnd->GetCamera()->GetBillboard();
	const D3DXMATRIX *pBillBoardMatrixY= pEditWnd->GetCamera()->GetBillboardY();
	DWORD dwCurrentTime = timeGetTime();
	static GameSelectRay cSelRay;
	RECT rc;
	pEditWnd->GetWndRect(&rc);
	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);      
	m_dwVisibleTrianglesCount = pSet->GetVisibleIndexCount() * 2;

	static GameBuildList *pGameBuildList;
	static GameBuildList::Build *pBuild(NULL);
	static ToolBuildEdit *pToolBuildEdit(NULL);

	pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	pGameBuildList = GameBuildList::GetInstance();  	

	static GameSelectRay rayCamera;
	rayCamera.CalculateRay(pEditWnd->GetCamera());

	D3DXVECTOR3 vIntersection;

	static std::vector<DWORD> vectorLightList;

	vectorLightList.clear();
	DWORD n;
	int gridnum = 0;
	for (n = 0; n < pSet->GetVisibleIndexCount(); n++)
	{
		//保存光列表
		DWORD dwGridIndex = pVIArray[n];
		pMapGrid = GetGrid(dwGridIndex);
		WORD wCurBuildID = pMapGrid->GetBuild()->wID;
		if (wCurBuildID != 0xFFFF)
		{
			pBuild = pGameBuildList->AcquireBuild(wCurBuildID,false);
			if (pBuild)
			{
				if (pBuild->GetLightSource())
					vectorLightList.push_back(dwGridIndex);
			}
		}
	}

	for (n = 0; n < pSet->GetVisibleIndexCount(); n++)
	{
		DWORD dwGridIndex = pVIArray[n];

		pMapGrid  = GetGrid(dwGridIndex);
		pMeshGrid = pTerrainMesh->GetGrid(dwGridIndex);

		//计算可视格子  in  选择锥
		for (DWORD v = 0; v < 4; v++)
		{
			pos[v] = pVertexArray[pMeshGrid->dwVertexIndex[v]].vPosition;
			pos[v].y += 0.01f;
		}

		vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;

		if (dwRenderFlag & MATERIALMASK)
		{
			//显示材质
			DWORD dwGridMtl = pMapGrid->GetMtl();
			if (dwGridMtl !=0)
			{
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],dwMtlColor[dwGridMtl]);
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],dwMtlColor[dwGridMtl]);
			}
		}

		if (dwRenderFlag & WEATHEREFFECTMASK)
		{
			//在此处画天气标志...
			BOOL bWE = pMapGrid->GetWeatherEffectFlag();
			if (bWE)
			{
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xE04455AA);
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xE04455AA);
			}
		}

		if (dwRenderFlag & BLOCKMASK)
		{
			//显示阻挡
			DWORD dwGridBlock = pMapGrid->GetBlock();

			D3DXVECTOR3 vDraw = vCenter;
			vDraw.y = 0.5f;

			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],dwBlockColor[dwGridBlock]);
			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],dwBlockColor[dwGridBlock]);
		}

		if (dwRenderFlag & SECURITYMASK)
		{
			//显示安全区
			DWORD dwGridSecurity = pMapGrid->GetSecurity();

			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],dwSecurityColor[dwGridSecurity]);
			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],dwSecurityColor[dwGridSecurity]);
		}

		if (dwRenderFlag & WARMASK)
		{
			//显示城战
			DWORD dwGridWar = pMapGrid->GetWar();

			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],dwWarColor[dwGridWar]);
			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],dwWarColor[dwGridWar]);
		}

		if (dwRenderFlag & SWITCHMASK)
		{
			//显示门
			DWORD dwDoorIndex = pMapGrid->GetDoorIndex();
			if (dwDoorIndex) //0为没有
			{
				dwDoorIndex--;

				WndDoorEdit *pWndDoorEdit = pEditWnd->GetWndDoorEdit();
				tagDoor *pDoor = pWndDoorEdit->GetDoor(dwDoorIndex);
			
				D3DXMATRIX matTRS;
				D3DXMatrixTranslation(&matTRS,vCenter.x,vCenter.y,vCenter.z);

				pEditWnd->GetToolDoorEdit()->RnderDoorModel(&matTRS);
			}
		}

		if (dwRenderFlag & TERRAINMASK)
		{
			//在此处画地表
		}

		if (dwRenderFlag & BUILDINGMASK)
		{
			//在此处画地物
			WORD wCurBuildID = pMapGrid->GetBuild()->wID;
			Grid::tagBuild *pGridBuild = pMapGrid->GetBuild();

			if (!pToolBuildEdit->FindSelectIndex(dwGridIndex))
			{
				if (pToolBuildEdit->GetHoverBuildGridIndex() != dwGridIndex)
				{
					if (wCurBuildID != 0xFFFF)
					{
						D3DXMATRIX matWorld,matRot;
						D3DXMatrixTranslation(&matWorld,vCenter.x,vCenter.y + pGridBuild->fHeight,vCenter.z);
						D3DXMatrixRotationY(&matRot,pMapGrid->GetBuild()->fRotationY);
						D3DXMatrixMultiply(&matWorld, &matRot, &matWorld);

						pBuild = pGameBuildList->AcquireBuild(wCurBuildID,false);
						const char* pstrpath;
						static char str[255];
						CDisplayModel *pGameModel(NULL);
						if (pBuild)
						{
							if (pBuild->GetModel())
							{
								if (pBuild->GetModel()->pGameModel)
								{
									pstrpath = CGameModelManager::GetInstance()->GetGameModelInfoFileName(pBuild->GetModel()->pGameModel->GetGameModelID());
									sprintf(str,"%s\\model",pstrpath);
									pstrpath = str;
								}
							}

							
							
							
						}
						
						if(pMapGrid->pDisplayModel == NULL)
						{
							pMapGrid->pDisplayModel = CGameModelManager::GetInstance()->CreateDisplayModel(pstrpath,false);
							if(pMapGrid->pDisplayModel)
							{
								pMapGrid->pDisplayModel->ShowAllGroup();
							}
						}
						if(pMapGrid->pDisplayModel&&pMapGrid->pDisplayModel->GetLoadState() == Load_Did)
						{
							pGameModel = pMapGrid->pDisplayModel;
							if (pFrustum)
							{
								if (!pFrustum->BoxIntersectFrustum(&matWorld,pGameModel->GetSizeBox()))
								{
									continue;
								}
							}
						}
						
						BOOL bAlphaBlend = FALSE;
						if (pBuild)
						{
							GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
							if (pModel)
							{
								/*pGameModel = pModel->pGameModel;*/
								m_dwVisibleTrianglesCount += pModel->dwTrianglCount;
								float fDist;
								if (pModel->bAutoHide && rayCamera.TestBoundingBox(&matWorld,pGameModel->GetSizeBox(),&vIntersection,&fDist))
								{
									bAlphaBlend = TRUE;
								}

								AnimInfo *pAnimInfo = pModel->dwGlobalTimer? &pMapGrid->m_cBuildModelAnimInfo : &pMapGrid->m_cBuildModelAnimInfo;
								if (pGameModel&&pGameModel->GetLoadState() == Load_Did)
								{
									DWORD dwGridColor = pTerrainMesh->GetGridColor(dwGridIndex);
									pAnimInfo->SetCurrentTime(dwCurrentTime);
									pAnimInfo->SetBillboard(pBillBoardMatrix);
									pAnimInfo->SetBillboardY(pBillBoardMatrixY);
									pAnimInfo->SetWorldMatrix(&matWorld);
									pAnimInfo->SetViewMatrix(pCamera->GetViewMatrix());
									pAnimInfo->SetProjectedMatrix(pCamera->GetProjectionMatrix());

									AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();
									if (pActionInfo->dwCurActionLoopCount > 0)
									{
										pActionInfo->dwCurActionLoopCount = 0;
										//1次 切换到下一个动作
										if (pActionInfo->dwCurAction == MAKEFOURCC('I','D','L','E'))
											pActionInfo->dwCurAction = MAKEFOURCC('S','T','N','D');
										else
											pActionInfo->dwCurAction = MAKEFOURCC('I','D','L','E');
									}
									pGameModel->SetDirLightEnable(pModel->bLighted == TRUE);
									//pGameModel->SetRenderModelStatic(true);
									pGameModel->ProcessAnimJoint(pAnimInfo);
									pGameModel->SetModelColor(GetSunLightAmbientColor());
									//pGameModel->ProcessDirectionalLight(&D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*pCamera->GetPosition()*/, CommDB::GetInstance()->GetMap()->GetLightDirection(),m_dwSunLightDiffuseColor);
									pGameModel->MultiplyModelColor(dwGridColor);

									//else
									//{
									//	pGameModel->SetUseNormal(false);
									//	pGameModel->SetRenderModelStatic(true);
									//	pGameModel->ProcessAnimJoint(pAnimInfo);
									//	pGameModel->SetModelColor(dwGridColor);
									//}

									pGameModel->ProcessAnimTexture(pAnimInfo);
									pGameModel->ProcessAnimUV(pAnimInfo);
									pGameModel->ProcessVisibility(pAnimInfo);
									pGameModel->ProcessParticleEmitter(pAnimInfo);
									pGameModel->ProcessTrailEmitter(pAnimInfo);
									pGameModel->ProcessLocator(pAnimInfo);
									pGameModel->ProcessTextureProjective(pAnimInfo);
									pGameModel->ProcessAnimColor(pAnimInfo);
									pGameModel->ProcessVisibility(pAnimInfo);
									pGameModel->ProcessWater(pAnimInfo);


									if (pGameModel->GetWater() == NULL)
									{
										if (bAlphaBlend)
										{
											DWORD dwcolor = dwGridColor&0x00ffffff;
											dwcolor |= 0x7f000000;
											pGameModel->SetModelColor(dwcolor);
											pGameModel->RenderModelTransparent();
										}else
											pGameModel->RenderModel();
									}
								}
								else
								{
									//画线框
									static D3DXVECTOR3 vCenter0(0,0.5,0);
									D3DXVECTOR3 vCenter;
									D3DXVec3TransformCoord(&vCenter,&vCenter0,&matWorld);
									pLayer3D->_DEBUG_DrawWireBox(&vCenter,0.5,0xFF44FF44);
								}
							}else
							{
								//画线框
								static D3DXVECTOR3 vCenter0(0,0.5,0);
								D3DXVECTOR3 vCenter;
								D3DXVec3TransformCoord(&vCenter,&vCenter0,&matWorld);
								pLayer3D->_DEBUG_DrawWireBox(&vCenter,0.5,0xFF44FF44);
							}
						}
					}
				}
			}
		}

		//画水面

		// 渲染网格
		if (dwRenderFlag & GRIDMASK)
		{
			pos[0].y += 0.005f;pos[1].y += 0.005f;pos[2].y += 0.005f;pos[3].y += 0.005f;
			WireLinePos[gridnum * 8 + 0] = pos[0];
			WireLinePos[gridnum * 8 + 1] = pos[1];
			WireLinePos[gridnum * 8 + 2] = pos[1];
			WireLinePos[gridnum * 8 + 3] = pos[2];
			WireLinePos[gridnum * 8 + 4] = pos[2];
			WireLinePos[gridnum * 8 + 5] = pos[3];
			WireLinePos[gridnum * 8 + 6] = pos[3];
			WireLinePos[gridnum * 8 + 7] = pos[0];
			if (gridnum * 8 + 7 > 19990)
			{
				pLayer3D->_DEBUG_DrawWireQuad(WireLinePos,gridnum * 4,0xFFFFFFFF);	
				gridnum = -1;
			}
			gridnum++;
			
			
		}
		
	}
	if (dwRenderFlag & GRIDMASK)
	{
		pLayer3D->_DEBUG_DrawWireQuad(WireLinePos,gridnum * 4,0xFFFFFFFF);	
	}
	
}

void EditMap::GetGridPosition(DWORD dwGridIndex,D3DXVECTOR3 * pPos,D3DXVECTOR3 *pCenter)
{
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	TerrainMesh::_tagGrid *pGrid =  pMesh->GetGrid(dwGridIndex);

	for(DWORD v = 0; v < 4; v++)
	{
		pPos[v] = pVertexArray[pGrid->dwVertexIndex[v]].vPosition;
	}

	*pCenter = pPos[0];
	*pCenter += pPos[1];
	*pCenter += pPos[2];
	*pCenter += pPos[3];
	*pCenter /= 4.0f;
}

void EditMap::GetGridPosition(int iX,int iZ,D3DXVECTOR3 * pPos,D3DXVECTOR3 *pCenter)
{
	DWORD dwGridIndex = iX + iZ * m_iWidth;

	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	TerrainMesh::_tagGrid *pGrid =  pMesh->GetGrid(dwGridIndex);
	
	for(DWORD v = 0; v < 4; v++)
	{
		pPos[v] = pVertexArray[pGrid->dwVertexIndex[v]].vPosition;
	}

	*pCenter = pPos[0];
	*pCenter += pPos[1];
	*pCenter += pPos[2];
	*pCenter += pPos[3];
	*pCenter /= 4.0f;
}

void EditMap::Save(FILE * pFile)
{
	//保存数据
	//格子数据
	DWORD n;
	for(n = 0; n < (DWORD)(m_iWidth * m_iDepth); n++)
	{
		fwrite(&m_pGridArray[n].GetBuild()->wID,sizeof(WORD),1,pFile);
		fwrite(&m_pGridArray[n].GetBuild()->fRotationY,sizeof(float),1,pFile);
		fwrite(&m_pGridArray[n].GetBuild()->fHeight,sizeof(float),1,pFile);
		fwrite(&m_pGridArray[n].m_dwTargetHideGridIndex,sizeof(DWORD),1,pFile);
		Grid::tagResverd t;
		fwrite(&t,sizeof(Grid::tagResverd),1,pFile);
		fwrite(&m_pGridArray[n].m_dwWar,sizeof(DWORD),1,pFile);
		fwrite(&m_pGridArray[n].m_dwSecurity,sizeof(DWORD),1,pFile);
		fwrite(&m_pGridArray[n].m_dwBlock,sizeof(DWORD),1,pFile);
		fwrite(&m_pGridArray[n].m_dwMtl,sizeof(DWORD),1,pFile);
		fwrite(&m_pGridArray[n].m_dwDoorIndex,sizeof(DWORD),1,pFile);
		fwrite(&m_pGridArray[n].m_dwFlag,sizeof(DWORD),1,pFile);
	}

	//光照数据
	fwrite(&m_dwSunLightAmbientColor,sizeof(DWORD),1,pFile);
	fwrite(&m_dwSunLightDiffuseColor,sizeof(DWORD),1,pFile);
	fwrite(&m_vSunLightDirection,sizeof(D3DXVECTOR3),1,pFile);
	fwrite(&m_fSunLightIntensitisy,sizeof(float),1,pFile);

	//雾
	fwrite(&m_dwFogStart,sizeof(DWORD),1,pFile);
	fwrite(&m_dwFogEnd,sizeof(DWORD),1,pFile);
	fwrite(&m_dwFogColor,sizeof(DWORD),1,pFile);

	//写入切换点
	WndDoorEdit *pWndDoorEdit = CommUI::GetInstance()->GetEditWnd()->GetWndDoorEdit();
	DWORD dwDoorCount = pWndDoorEdit->GetDoorCount();
	fwrite(&dwDoorCount,sizeof(DWORD),1,pFile);
	for(n = 0; n < dwDoorCount; n++)
	{
		tagDoor * pDoor = pWndDoorEdit->GetDoor(n);
		fwrite(pDoor,sizeof(tagDoor),1,pFile);
	}
}

void EditMap::Load(FILE * pFile)
{	 
	m_dwTotalPalacedBuildCount = 0;

	//读取数据
	DWORD n;
	for(n = 0; n < (DWORD)(m_iWidth * m_iDepth); n++)
	{
		fread(&m_pGridArray[n].m_tBuild.wID,sizeof(WORD),1,pFile);
		fread(&m_pGridArray[n].m_tBuild.fRotationY,sizeof(float),1,pFile);
		fread(&m_pGridArray[n].m_tBuild.fHeight,sizeof(float),1,pFile);
	
		if (m_pGridArray[n].m_tBuild.wID != 0xFFFF)
		{
			m_dwTotalPalacedBuildCount++;
		}

		AddRefenceBuildCount(m_pGridArray[n].m_tBuild.wID);
		

		fread(&m_pGridArray[n].m_dwTargetHideGridIndex,sizeof(DWORD),1,pFile);
		Grid::tagResverd t;
		fread(&t,sizeof(Grid::tagResverd),1,pFile);
		fread(&m_pGridArray[n].m_dwWar,sizeof(DWORD),1,pFile);
		fread(&m_pGridArray[n].m_dwSecurity,sizeof(DWORD),1,pFile);
		fread(&m_pGridArray[n].m_dwBlock,sizeof(DWORD),1,pFile);
		fread(&m_pGridArray[n].m_dwMtl,sizeof(DWORD),1,pFile);
		fread(&m_pGridArray[n].m_dwDoorIndex,sizeof(DWORD),1,pFile);
		fread(&m_pGridArray[n].m_dwFlag,sizeof(DWORD),1,pFile);
	}

	//光照数据
	fread(&m_dwSunLightAmbientColor,sizeof(DWORD),1,pFile);
	fread(&m_dwSunLightDiffuseColor,sizeof(DWORD),1,pFile);
	fread(&m_vSunLightDirection,sizeof(D3DXVECTOR3),1,pFile);
	fread(&m_fSunLightIntensitisy,sizeof(float),1,pFile);
	
	fread(&m_dwFogStart,sizeof(DWORD),1,pFile);
	fread(&m_dwFogEnd,sizeof(DWORD),1,pFile);
	fread(&m_dwFogColor,sizeof(DWORD),1,pFile);

	//读入切换点
	WndDoorEdit *pWndDoorEdit = CommUI::GetInstance()->GetEditWnd()->GetWndDoorEdit();

	pWndDoorEdit->ClearAllDoor();

	DWORD dwDoorCount;
	fread(&dwDoorCount,sizeof(DWORD),1,pFile);
	for(n = 0; n < dwDoorCount; n++)
	{
		tagDoor *pDoor = new tagDoor;
		fread(pDoor,sizeof(tagDoor),1,pFile);
		pWndDoorEdit->InsertDoor(pDoor);
	}
}

void EditMap::CalculateLightSourceLighting(void)
{
	DWORD *pVIArray = m_pVisibleSet->GetVisibleIndexArray();
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	GameBuildList *pGameBuildList = GameBuildList::GetInstance();

	DWORD dwCurrentTime = timeGetTime();

	Grid *pMapGrid(NULL);
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	static D3DXVECTOR3 vCenter;

	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagVertex *pVertexArray = pTerrainMesh->GetVertexList();


	D3DXVECTOR3 *pLightOffset(NULL);

	for(DWORD n = 0; n< m_pVisibleSet->GetVisibleIndexCount(); n++)
	{
		DWORD dwGridIndex = pVIArray[n];

		pMapGrid = GetGrid(dwGridIndex);
		pMeshGrid = pTerrainMesh->GetGrid(dwGridIndex);

		WORD wBuildID = pMapGrid->GetBuild()->wID;
		
		if ( wBuildID !=0xFFFF)
		{
			//是否有光照
			GameBuildList::Build *pBuild = pGameBuildList->AcquireBuild(wBuildID,false);
			if (pBuild)
			{
				GameBuildList::Build::tagLightSource *pLightSource = pBuild->GetLightSource();
				if (pLightSource)
				{
					//计算光照
					//循环可见格子

					int iCX = (int)(dwGridIndex % m_iWidth);
					int iCZ = (int)(dwGridIndex / m_iWidth);

					pTerrainMesh->GetGridCenter(dwGridIndex,&vCenter);

					/*if (pLight->CalculateTime(pMapGrid->m_dwBuildLightAnimTime,dwCurrentTime,1.0f))
					{ 计算光循环时间
					pMapGrid->m_dwBuildLightAnimTime = dwCurrentTime;
					}*/

					vCenter.y += pLightSource->fHeight;

					//循环影响格子计算点光源
					int iRadius = (int)pLightSource->fRadius;

					for(int i = -iRadius; i <= iRadius;i ++)
					{
						for(int j = -iRadius; j <= iRadius;j ++)
						{
							//求出周围的格子
							int iX = i + iCX;
							int iZ = j + iCZ;
							D3DXVECTOR3 vec3((float)iX,0,(float)iZ);
							D3DXMATRIX matrot,matzeropoint;
							D3DXVECTOR3 vec31;
							D3DXMatrixTranslation(&matzeropoint,-(float)iCX,0,-(float)iCZ);
							D3DXMatrixRotationY(&matrot,pMapGrid->GetBuild()->fRotationY);
							matrot = matzeropoint * matrot;
							D3DXMatrixInverse(&matzeropoint,NULL,&matzeropoint);
							matrot = matrot * matzeropoint;
							D3DXVec3TransformCoord(&vec31,&vec3,&matrot);
							iX = vec31.x + (fabs(vec31.x - (int)vec31.x) > 0.5?1:0);
							iZ = vec31.z + (fabs(vec31.z - (int)vec31.z) > 0.5?1:0);
							if (iX < 0 || iZ < 0 || iX >= (int)m_iWidth || iZ >= (int)m_iDepth);
							else
							{
								//压入定点索引到列表
								TerrainMesh::_tagGrid *pGrid = pTerrainMesh->GetGrid(iX + iZ * m_iDepth);
								for(DWORD v = 0; v < 4; v++)
								{
									int iVertexIndex = pGrid->dwVertexIndex[v]; 
									std::vector<int>::iterator iFind = std::find(m_vLightVertexIndexArray.begin(),m_vLightVertexIndexArray.end(),iVertexIndex);
									if (iFind == m_vLightVertexIndexArray.end())
									{
										//未找到
										m_vLightVertexIndexArray.push_back(iVertexIndex);
									}
								}
							}
						}
					}

					float fR = (float)( (pLightSource->dwColor & 0x00FF0000) >> 16) / 255.0f;
					float fG = (float)( (pLightSource->dwColor & 0x0000FF00) >>  8) / 255.0f;
					float fB = (float)(  pLightSource->dwColor & 0x000000FF)        / 255.0f;

					//计算定点光照
					static D3DXVECTOR3 L;
					for(int i =0; i < (int)m_vLightVertexIndexArray.size(); i++)
					{
						TerrainMesh::_tagVertex &vertex = pVertexArray[m_vLightVertexIndexArray[i]];
						DWORD &dwDestColor = vertex.cColor;

						L = vertex.vPosition - vCenter;

						float fLightDistance = D3DXVec3Length(&L);		//光源到vertex的距离

						D3DXVec3Normalize(&L,&L);	

						float DOT = - D3DXVec3Dot(&L,&vertex.vNormal);// L . N

						if (DOT > 0)//光线在面的正面
						{	
							float fValue = pLightSource->fMaxIntensity / fLightDistance * DOT;

							DWORD r = (dwDestColor & 0x00FF0000) >> 16;
							DWORD g = (dwDestColor & 0x0000FF00) >> 8;
							DWORD b = (dwDestColor & 0x000000FF);

							r += (DWORD)(( fR * fValue) * 255.0f);  	
							if (r > 255) r = 0x000000FF;
							g += (DWORD)(( fG * fValue) * 255.0f);  	
							if (g > 255) g = 0x000000FF;
							b += (DWORD)(( fB * fValue) * 255.0f);  	
							if (b > 255) b = 0x000000FF;
							dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;
						}
					}

					m_vLightVertexIndexArray.clear();
				}
			}
		}
	}
}

void EditMap::CalculateTerrainShadow()
{
	//先计算所有颜色
	//  循环所有的点
	//    计算点和光源之间是否有 A 地形阻挡 B 建筑阻挡
	//DWORD *pVIArray = m_pVisibleSet->GetVisibleIndexArray();
	//TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();

	//WheatherSystem * pWheatherSystem = WheatherSystem::GetInstance();
	//DWORD dwAmbientColor    = pWheatherSystem->GetAmbientLight();
	//LightDirectional * pSun = pWheatherSystem->GetSunLight();

	//TerrainMesh::_tagVertex *pVertex,*pVertexList = pTerrainMesh->GetVertexList();
	//TerrainMesh::_tagGrid   *pGrid; 

	//for(DWORD n = 0; n< m_pVisibleSet->GetVisibleIndexCount(); n++)
	//{
	//	pGrid = pTerrainMesh->GetGrid(pVIArray[n]);
	//	for(DWORD v = 0; v < 4; v++)
	//	{
	//		//取得顶点
	//		pVertex = &pVertexList[pGrid->dwVertexIndex[v]];		
	//		//计算光照
	//		pVertex->cColor = pSun->Calculate(dwAmbientColor,&pVertex->vNormal);
	//	}
	//}
}

//void EditMap::CalculateBuildBlock(void)
//{
//	Grid *pMapGrid;
//	GameBuildList::Build *pBuild(NULL); 
//	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
//	ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
//	GameBuildList *pGameBuildList = GameBuildList::GetInstance();  	
//
//	//计算障碍
//	for(int n = 0; n < m_iWidth * m_iDepth; n++)
//	{
//		pMapGrid = GetGrid(n);
//
//		WORD wCurBuildID = pMapGrid->GetBuild()->wID;
//		if (wCurBuildID != 0xFFFF)
//		{
//			pBuild = pGameBuildList->GetBuild(wCurBuildID);
//
//			if (pBuild->GetBlockType())
//			{
//				int iCX = (int)(n % m_iWidth);
//				int iCZ = (int)(n / m_iDepth);
//
//				for(int i = pBuild->GetBlockXMin(); i <= pBuild->GetBlockXMax(); i++)
//				{
//					for(int j = pBuild->GetBlockZMin(); j <=  pBuild->GetBlockZMax(); j++)
//					{
//						//求出周围的格子
//						int iX = i + iCX;
//						int iZ = j + iCZ;
//						if (!(iX < 0 || iZ < 0 || iX >= (int)m_iWidth || iZ >= (int)m_iDepth))
//						{
//							//计算
//							Grid *pGrid = GetGrid(iX,iZ);
//							DWORD dwBlockType = pBuild->GetBlockType();
//							pGrid->SetBlock(dwBlockType);
//						}
//					}
//				}
//			}
//		}
//	}
//}

//LightPoint* EditMap::CalculateLightPosition(DWORD dwGridIndex,DWORD dwCurrentTime)
//{
//	static D3DXVECTOR3 vCenter;	
//	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
//	Grid *pLightTempGrid = GetGrid(dwGridIndex);
//
//	WORD wBuildID = pLightTempGrid->GetBuild()->wID;
//	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
//	ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
//	GameBuildList *pGameBuildList = GameBuildList::GetInstance();
//	GameBuildList::Build *pBuild = pGameBuildList->GetBuild(wBuildID);
//
//	GameBuildList::Build::tagLightSource *pLightSource = pBuild->GetLightSource();
//	float fLightHeight = pLightSource->fHeight;
//
//	/*LightPoint *pLight = pBuild->GetLight();
//	if (pLight)
//	{
//		pTerrainMesh->GetGridCenter(dwGridIndex,&vCenter);
//		if (pLight->CalculateTime(pLightTempGrid->m_dwBuildLightAnimTime,dwCurrentTime,1.0f))
//		{
//			pLightTempGrid->m_dwBuildLightAnimTime = dwCurrentTime;
//		}
//
//		pLight->SetWorldPosition(vCenter.x,pLightTempGrid->GetBuildHeight() + vCenter.y + fLightHeight,vCenter.z);
//		
//		return pLight;
//	}*/	
//	return NULL;
//}

QNode* EditMap::GetQTRoot(void)
{
	return m_pQTRoot;
}

void EditMap::DrawAll()
{	
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	std::vector<int>::iterator itFind;
	Grid *pMapGrid(NULL);
    TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pTerrainMesh->GetVertexList();
	static D3DXVECTOR3 pos[4];
	static D3DXVECTOR3 vCenter;
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	const D3DXMATRIX *pBillboardMatrix = pEditWnd->GetCamera()->GetBillboard();
	const D3DXMATRIX *pBillboardMatrixY = pEditWnd->GetCamera()->GetBillboardY();
	DWORD dwCurrentTime = timeGetTime();
	static GameSelectRay cSelRay;
	RECT rc;
	pEditWnd->GetWndRect(&rc);
	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);      

	static GameBuildList *pGameBuildList;
	static GameBuildList::Build *pBuild(NULL);
	static ToolBuildEdit *pToolBuildEdit(NULL);

	pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	pGameBuildList = GameBuildList::GetInstance();  	

	static GameSelectRay rayCamera;
	rayCamera.CalculateRay(pEditWnd->GetCamera());

	D3DXVECTOR3 vIntersection;

	static std::vector<DWORD> vectorLightList;

	vectorLightList.clear();
	DWORD n;
	for(n = 0; n< (DWORD)(m_iWidth * m_iDepth); n++)
	{
		//保存光列表
		pMapGrid = GetGrid(n);
		WORD wCurBuildID = pMapGrid->GetBuild()->wID;
		if (wCurBuildID != 0xFFFF)
		{
			pBuild = pGameBuildList->AcquireBuild(wCurBuildID,false);
			if (pBuild  && pBuild->GetLightSource())
			{
				vectorLightList.push_back(n);
			}
		}
	}

	for(n = 0; n< (DWORD)(m_iWidth * m_iDepth); n++)
	{
		pMapGrid = GetGrid(n);
		pMeshGrid = pTerrainMesh->GetGrid(n);
		//计算可视格子  in  选择锥
		for(DWORD v = 0; v < 4; v++)
		{
			pos[v] = pVertexArray[pMeshGrid->dwVertexIndex[v]].vPosition;
			pos[v].y += 0.01f;
		}

		vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;

		{
			//在此处画地物
			WORD wCurBuildID = pMapGrid->GetBuild()->wID;
			Grid::tagBuild *pGridBuild = pMapGrid->GetBuild();

			if (!pToolBuildEdit->FindSelectIndex(n))
			{
				if (wCurBuildID != 0xFFFF)
				{
					pBuild = pGameBuildList->AcquireBuild(wCurBuildID,false);
					GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
                    CDisplayModel *pGameModel(NULL);
					if (pModel)
					{
						pGameModel = pModel->pGameModel;
						m_dwVisibleTrianglesCount += pModel->dwTrianglCount;
					}
					
					if (pGameModel)
					{

						D3DXMATRIX matWorld,matView,matPro,matRot;
						D3DXMatrixTranslation(&matWorld,vCenter.x,vCenter.y + pGridBuild->fHeight,vCenter.z);
						D3DXMatrixRotationY(&matRot,pGridBuild->fRotationY);
						D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

						pMapGrid->m_cBuildModelAnimInfo.SetBillboard(pBillboardMatrix);
						pMapGrid->m_cBuildModelAnimInfo.SetBillboardY(pBillboardMatrixY);	
						pMapGrid->m_cBuildModelAnimInfo.SetWorldMatrix(&matWorld);
						pInterface->GetDevice()->GetTransform(D3DTS_VIEW,&matView);
						pMapGrid->m_cBuildModelAnimInfo.SetViewMatrix(&matView);
						pInterface->GetDevice()->GetTransform(D3DTS_PROJECTION,&matPro);
						pMapGrid->m_cBuildModelAnimInfo.SetProjectedMatrix(&matPro);
						pMapGrid->m_cBuildModelAnimInfo.SetCurrentTime(dwCurrentTime);
						pGameModel->ProcessAnimJoint(&pMapGrid->m_cBuildModelAnimInfo);
						pGameModel->ProcessAnimTexture(&pMapGrid->m_cBuildModelAnimInfo);
						pGameModel->ProcessAnimUV(&pMapGrid->m_cBuildModelAnimInfo);
						pGameModel->ProcessTrailEmitter(&pMapGrid->m_cBuildModelAnimInfo);
						pGameModel->ProcessLocator(&pMapGrid->m_cBuildModelAnimInfo);
						

						if (pModel->bLighted)
						{
							static D3DXVECTOR3 vLightCenter;

							//方向光
							pGameModel->SetModelColor(m_dwSunLightAmbientColor);
							//pGameModel->ProcessDirectionalLight(pEditWnd->GetCamera()->GetPosition(),&m_vSunLightDirection,m_dwSunLightDiffuseColor);
							//点光
							for(DWORD nLight= 0; nLight < (DWORD)vectorLightList.size(); nLight++)
							{
								pTerrainMesh->GetGridCenter(vectorLightList[nLight]	,&vLightCenter);

								Grid *pLightGrid = GetGrid(vectorLightList[nLight]);
								GameBuildList::Build *pLightBuild = pGameBuildList->AcquireBuild(pLightGrid->GetBuild()->wID,false);
								GameBuildList::Build::tagLightSource *pLightSource = pLightBuild->GetLightSource();
								//位置
								vLightCenter.y += pLightSource->fHeight;

								//计算距离
								D3DXVECTOR3 vOffset = vLightCenter - vCenter;
								vOffset.y = vOffset.y - pGridBuild->fHeight;
								float fDist = D3DXVec3Length(&vOffset);
								if (fDist <= pLightSource->fRadius)
								{
									pGameModel->ProcessPointLight(&vLightCenter,pLightSource->fMaxIntensity ,pLightSource->dwColor);						
								}
							}
						}
						else
						{
							pGameModel->SetModelColor(0xFFFFFFFF);
						}
						
						pGameModel->ProcessAnimColor(&pMapGrid->m_cBuildModelAnimInfo);
						pGameModel->ProcessVisibility(&pMapGrid->m_cBuildModelAnimInfo);

						pGameModel->DrawModel();
					}
				}
			}

		}
	}
}
void EditMap::CalculateAllBaseLighting(void)
{   
	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	
	DWORD dwAR = (m_dwSunLightAmbientColor & 0x00FF0000) >> 16;
	DWORD dwAG = (m_dwSunLightAmbientColor & 0x0000FF00) >> 8;
	DWORD dwAB = m_dwSunLightAmbientColor & 0x000000FF;

	float fR = (float)( (m_dwSunLightDiffuseColor & 0x00FF0000) >> 16) / 255.0f;
	float fG = (float)( (m_dwSunLightDiffuseColor & 0x0000FF00) >>  8) / 255.0f;
	float fB = (float)(  m_dwSunLightDiffuseColor & 0x000000FF)        / 255.0f;

	TerrainMesh::_tagVertex *pVertex,*pVertexList = pTerrainMesh->GetVertexList();
	TerrainMesh::_tagGrid   *pGrid; 

	for(DWORD n = 0; n< (DWORD)(m_iWidth * m_iDepth); n++)
	{
		pGrid = pTerrainMesh->GetGrid(n);
		for(DWORD v = 0; v < 4; v++)
		{
			//取得顶点
			if(pGrid->dwVertexIndex[v] > pTerrainMesh->GetVertexNum() - 1)
			{
				DWORD x = n%m_iWidth;
				DWORD z = n/m_iWidth;
				//dwIndex = x + z * m_dwWidth;
				DWORD tempindex[4];
				tempindex[0] = z *  (m_iWidth + 1) + x;
				tempindex[1] = tempindex[0] + 1;
				tempindex[2] = tempindex[1] + (m_iWidth + 1);
				tempindex[3] = tempindex[2] - 1;
				pGrid->dwVertexIndex[v] = tempindex[v];
				
			}
			pVertex = &pVertexList[pGrid->dwVertexIndex[v]];		
			//计算光照
			DWORD &dwDestColor = pVertex->cColor;

			float DOT = - D3DXVec3Dot(&m_vSunLightDirection,&pVertex->vNormal);// L . N

			DWORD r = dwAR;
			DWORD g = dwAG;
			DWORD b = dwAB;
			if (DOT > 0)//光线在面的正面
			{
				float fValue = 1.0f * DOT;

				r += (DWORD)(( fR * fValue) * 255.0f);  	
				if (r > 255) r = 0x000000FF;
				g += (DWORD)(( fG * fValue) * 255.0f);  	
				if (g > 255) g = 0x000000FF;
				b += (DWORD)(( fB * fValue) * 255.0f);  	
				if (b > 255) b = 0x000000FF;
			}
			dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;
		}
	}
}

void EditMap::CalculateAllLightSourceLighting(void)
{
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	GameBuildList *pGameBuildList = GameBuildList::GetInstance();

	Grid *pMapGrid(NULL);
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	static D3DXVECTOR3 vCenter;

	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagVertex *pVertexArray = pTerrainMesh->GetVertexList();


	D3DXVECTOR3 *pLightOffset(NULL);

	for(DWORD n = 0; n<(DWORD)(m_iWidth * m_iDepth); n++)
	{
		DWORD dwGridIndex = n;

		pMapGrid = GetGrid(dwGridIndex);
		pMeshGrid = pTerrainMesh->GetGrid(dwGridIndex);

		WORD wBuildID = pMapGrid->GetBuild()->wID;
		
		if ( wBuildID !=0xFFFF)
		{
			//是否有光照
			GameBuildList::Build *pBuild = pGameBuildList->AcquireBuild(wBuildID,false);
			if (pBuild)
			{
				GameBuildList::Build::tagLightSource *pLightSource = pBuild->GetLightSource();
				if (pLightSource)
				{
					//计算光照
					//循环可见格子

					int iCX = (int)(dwGridIndex % m_iWidth);
					int iCZ = (int)(dwGridIndex / m_iDepth);
					
					pTerrainMesh->GetGridCenter(dwGridIndex,&vCenter);

					/*if (pLight->CalculateTime(pMapGrid->m_dwBuildLightAnimTime,dwCurrentTime,1.0f))
					{ 计算光循环时间
					pMapGrid->m_dwBuildLightAnimTime = dwCurrentTime;
					}*/

					vCenter.y += pLightSource->fHeight;
					//循环影响格子计算点光源
					int iRadius = (int)pLightSource->fRadius;

					for(int i = -iRadius; i <= iRadius;i ++)
					{
						for(int j = -iRadius; j <= iRadius;j ++)
						{
							//求出周围的格子
							int iX = i + iCX;
							int iZ = j + iCZ;
							D3DXVECTOR3 vec3((float)iX,0,(float)iZ);
							D3DXMATRIX matrot,matzeropoint;
							D3DXVECTOR3 vec31;
							D3DXMatrixTranslation(&matzeropoint,-(float)iCX,0,-(float)iCZ);
							D3DXMatrixRotationY(&matrot,pMapGrid->GetBuild()->fRotationY);
							matrot = matzeropoint * matrot;
							D3DXMatrixInverse(&matzeropoint,NULL,&matzeropoint);
							matrot = matrot * matzeropoint;
							D3DXVec3TransformCoord(&vec31,&vec3,&matrot);
							iX = vec31.x + (fabs(vec31.x - (int)vec31.x) > 0.5?1:0);
							iZ = vec31.z + (fabs(vec31.z - (int)vec31.z) > 0.5?1:0);
							if (iX < 0 || iZ < 0 || iX >= (int)m_iWidth || iZ >= (int)m_iDepth);
							else
							{
								//压入定点索引到列表
								TerrainMesh::_tagGrid *pGrid = pTerrainMesh->GetGrid(iX + iZ * m_iDepth);
								for(DWORD v = 0; v < 4; v++)
								{
									int iVertexIndex = pGrid->dwVertexIndex[v]; 
									std::vector<int>::iterator iFind = std::find(m_vLightVertexIndexArray.begin(),m_vLightVertexIndexArray.end(),iVertexIndex);
									if (iFind == m_vLightVertexIndexArray.end())
									{
										//未找到
										m_vLightVertexIndexArray.push_back(iVertexIndex);
									}
								}

							}
						}
					}
					float fR = (float)( (pLightSource->dwColor & 0x00FF0000) >> 16) / 255.0f;
					float fG = (float)( (pLightSource->dwColor & 0x0000FF00) >>  8) / 255.0f;
					float fB = (float)(  pLightSource->dwColor & 0x000000FF)        / 255.0f;
					//计算定点光照
					static D3DXVECTOR3 L;
					for(int i =0; i < (int)m_vLightVertexIndexArray.size(); i++)
					{
						TerrainMesh::_tagVertex &vertex = pVertexArray[m_vLightVertexIndexArray[i]];
						DWORD &dwDestColor = vertex.cColor;

						L = vertex.vPosition - vCenter;

						float fLightDistance = D3DXVec3Length(&L);		//光源到vertex的距离

						D3DXVec3Normalize(&L,&L);	

						float DOT = - D3DXVec3Dot(&L,&vertex.vNormal);// L . N

						if (DOT > 0)//光线在面的正面
						{	
							float fValue = pLightSource->fMaxIntensity / fLightDistance * DOT;

							DWORD r = (dwDestColor & 0x00FF0000) >> 16;
							DWORD g = (dwDestColor & 0x0000FF00) >> 8;
							DWORD b = (dwDestColor & 0x000000FF);

							r += (DWORD)(( fR * fValue) * 255.0f);  	
							if (r > 255) r = 0x000000FF;
							g += (DWORD)(( fG * fValue) * 255.0f);  	
							if (g > 255) g = 0x000000FF;
							b += (DWORD)(( fB * fValue) * 255.0f);  	
							if (b > 255) b = 0x000000FF;
							dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;
						}
					}
					m_vLightVertexIndexArray.clear();
				}
			}
		}
	}
}

void EditMap::ImportVisibleMap(const char * pszFileName)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	DWORD dwTexHandle = 0xffffffff;
	DWORD texWidth, texHeight;

	dwTexHandle = pInterface->CreateTexture(pszFileName);
	if (dwTexHandle == 0XFFFFFFFF)
	{
		MessageBox(pInterface->GetWndHandle(),"错误：读取文件失败","错误",MB_OK);
		return ;
	}

	render::Texture *pTexture = pInterface->GetTexture(dwTexHandle);
	texWidth = pTexture->GetWidth();
	texHeight = pTexture->GetHeight();
	for(int  x = 0; x < (int)texWidth; x++)
	{
		for(int z = 0; z < (int)texHeight; z++)
		{
			if (x < m_iWidth && z < m_iDepth)
			{
				Grid *pGrid = GetGrid(x,z);
				DWORD color = pTexture->GetPixel(x,texHeight - 1 - z); 
				color = color >> 24;
				if (color == 0)
                    pGrid->SetVisible(FALSE);
				else
					if (color = 255)
						pGrid->SetVisible(TRUE);
			}
		}
	}

	pInterface->DestroyTexture(dwTexHandle);
}

extern void WriteTGA(UINT uBits,UINT uWidth,UINT uHeight,BYTE *pixels,FILE *pf);
void EditMap::ExportVisibleMap(const char * pszFileName)
{
	FILE *fp = fopen(pszFileName, "wb");

	DWORD *pixels = new DWORD[m_iWidth * m_iDepth];
	for(int  x = 0; x < m_iWidth; x++)
	{
		for(int z = 0; z < m_iDepth; z++)
		{
			Grid *pGrid = GetGrid(x,z);
			DWORD dwColor;
			dwColor = pGrid->GetVisible() * 0xFF000000;
			pixels[x + z * m_iWidth] = dwColor;
				
		}
	}

	WriteTGA(32, m_iWidth, m_iDepth, (BYTE*)pixels, fp);
	
	delete pixels;

	fclose(fp);
}

void EditMap::ImportBlockMap(const char * pszFileName)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	DWORD dwTexHandle = 0xffffffff;
	DWORD texWidth, texHeight;

	dwTexHandle = pInterface->CreateTexture(pszFileName);
	if (dwTexHandle == 0XFFFFFFFF)
	{
		MessageBox(pInterface->GetWndHandle(),"错误：读取文件失败","错误",MB_OK);
		return ;
	}

	render::Texture *pTexture = pInterface->GetTexture(dwTexHandle);
	texWidth = pTexture->GetWidth();
	texHeight = pTexture->GetHeight();
	for(int  x = 0; x < (int)texWidth; x++)
	{
		for(int z = 0; z < (int)texHeight; z++)
		{
			if (x < m_iWidth && z < m_iDepth)
			{
				Grid *pGrid = GetGrid(x,z);
				DWORD a = pTexture->GetPixel(x,texHeight - 1 - z);
				a = (a >> 24);
				if (a == 0)
				{
					pGrid->SetBlock(0);
				}
				else
                    if (a == 128)
					{
						pGrid->SetBlock(1);
					}
					else
						if ( a == 255)
						{
                            pGrid->SetBlock(2);
						}
			}
		}
	}

	pInterface->DestroyTexture(dwTexHandle);
}

void EditMap::ExportBlockMap(const char * pszFileName)
{
	FILE *fp = fopen(pszFileName, "wb");

	// format : RGB
	DWORD *pixels = new DWORD[m_iWidth * m_iDepth];
	DWORD dwColor[3] = { 0x00000000,0x80000000,0xff000000};
	for(int  x = 0; x < m_iWidth; x++)
	{
		for(int z = 0; z < m_iDepth; z++)
		{
			Grid *pGrid = GetGrid(x,z);
            
			pixels[x + z * m_iWidth] = dwColor[pGrid->GetBlock() % 3];
		}
	}

	WriteTGA(32, m_iWidth, m_iDepth, (BYTE*)pixels, fp);
	
	delete pixels;

	fclose(fp);
}

void EditMap::RecalculateBlock(void)
{
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	GameBuildList *pGameBuildList = GameBuildList::GetInstance();
	Grid *pMapGrid(NULL);

	for(int  n = 0; n< m_iWidth * m_iDepth; n++)
	{
		pMapGrid = GetGrid(n);

		WORD wBuildID = pMapGrid->GetBuild()->wID;
		
		if ( wBuildID !=0xFFFF) //是否有建筑
		{
			GameBuildList::Build *pBuild = pGameBuildList->AcquireBuild(wBuildID,false);
			if (pBuild)
			{
				//计算阻挡

				//计算位置
				int iCX = n % m_iWidth;
				int iCZ = n / m_iDepth;
				//清除障碍                                  
				GameBuildList::Build::tagBlockData *pBlockData = pBuild->GetBlockData();
				if (pBlockData)
				{
					for(int i = pBlockData->iXMin; i <= pBlockData->iXMax; i++)
					{
						for(int j = pBlockData->iZMin; j <= pBlockData->iZMax; j++)
						{
							//求出周围的格子
							int iX = i + iCX;
							int iZ = j + iCZ;
							D3DXVECTOR3 vec3((float)iX,0,(float)iZ);
							D3DXMATRIX matrot,matzeropoint;
							D3DXVECTOR3 vec31;
							D3DXMatrixTranslation(&matzeropoint,-(float)iCX,0,-(float)iCZ);
							D3DXMatrixRotationY(&matrot,pMapGrid->GetBuild()->fRotationY);
							matrot = matzeropoint * matrot;
							D3DXMatrixInverse(&matzeropoint,NULL,&matzeropoint);
							matrot = matrot * matzeropoint;
							D3DXVec3TransformCoord(&vec31,&vec3,&matrot);
							iX = vec31.x + (fabs(vec31.x - (int)vec31.x) > 0.5?1:0);
							iZ = vec31.z + (fabs(vec31.z - (int)vec31.z) > 0.5?1:0);
							
							if (!(iX < 0 || iZ < 0 || iX >= m_iWidth || iZ >= m_iDepth))
							{
								Grid *pTempGrid = GetGrid(iX,iZ);
								pTempGrid->SetBlock(pBlockData->byType + 1);	//0 none 1 normal 2 high
							}
						}
					}
				}
			}
		}
	}
}

void EditMap::ImportWeatherEffectMap(const char * pszFileName)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	DWORD dwTexHandle = 0xffffffff;
	DWORD texWidth, texHeight;

	dwTexHandle = pInterface->CreateTexture(pszFileName);
	if (dwTexHandle == 0XFFFFFFFF)
	{
		MessageBox(pInterface->GetWndHandle(),"错误：读取文件失败","错误",MB_OK);
		return ;
	}

	render::Texture *pTexture = pInterface->GetTexture(dwTexHandle);
	texWidth = pTexture->GetWidth();
	texHeight = pTexture->GetHeight();
	for(int  x = 0; x < (int)texWidth; x++)
	{
		for(int z = 0; z < (int)texHeight; z++)
		{
			if (x < m_iWidth && z < m_iDepth)
			{
				Grid *pGrid = GetGrid(x,z);
				DWORD a = (pTexture->GetPixel(x,texHeight - 1 - z) & 0xFF000000) >> 24;
				pGrid->SetWeatherEffectFlag((BOOL)a);
			}
		}
	}

	pInterface->DestroyTexture(dwTexHandle);
}

void EditMap::ImportEnvCreateMap(const char * pszFileName)
{
}

void EditMap::ClearUnusedBuild(void)
{
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	GameBuildList *pGameBuildList = GameBuildList::GetInstance();

	Grid *pMapGrid(NULL);
	for(DWORD n = 0; n<(DWORD)(m_iWidth * m_iDepth); n++)
	{
		DWORD dwGridIndex = n;

		pMapGrid = GetGrid(dwGridIndex);

		WORD wBuildID = pMapGrid->GetBuild()->wID;
		
		if ( wBuildID !=0xFFFF)
		{						   
			if (!pGameBuildList->AcquireBuild(wBuildID),false)
			{
				static char szMsg[256];
				_snprintf(szMsg,255,"在坐标:(%d %d)处建筑\"%1.1d%3.3d\"不存在.",n % m_iWidth ,n / m_iWidth,(wBuildID & 0xf000) >> 12,wBuildID & 0x0fff);

				MessageBox(NULL,szMsg,"提示",MB_OK);
				//消除不存在建筑
				Grid::tagBuild tBuild;
				tBuild.wID = 0xFFFF;
				pMapGrid->SetBuild(tBuild);
			}
		}
	}
}
void EditMap::GetTagMapBuffer(int x,int z,DWORD size,D3DXVECTOR3* posbuff)
{
	
}
void EditMap::AddRefenceBuildCount(WORD wBuildID)
{
	if (wBuildID == 0xFFFF) return;
	std::map<WORD,DWORD>::iterator it = m_mapReferenceBuildArray.find(wBuildID);

	if (it == m_mapReferenceBuildArray.end())
	{
		m_mapReferenceBuildArray.insert(REFERENCEBUILD_PAIR(wBuildID,1));
	}
	else
		it->second++;
}

void EditMap::SubRefenceBuildCount(WORD wBuildID)
{
	if (wBuildID == 0xFFFF) return;
	std::map<WORD,DWORD>::iterator it = m_mapReferenceBuildArray.find(wBuildID);

	if (it != m_mapReferenceBuildArray.end())
	{
		it->second = it->second--;
		if (it->second == 0)
			m_mapReferenceBuildArray.erase(it);
	}
}

void EditMap::ImportMaterialMap(const char * pszFileName)
{
	const DWORD dwMaterialColor[10] = {
		 0x00000000//缺省	
		,0x0000FF00//草地
		,0x00FF0000//泥巴
		,0x00FFFF00//沙漠
		,0x00FF00FF//木板
		,0x00808080//石板
		,0x000000FF//水面
		,0x00FFFFFF//雪地
		,0x0000FFFF//沼泽
	};

	render::Interface *pInterface = render::Interface::GetInstance();
	DWORD dwTexHandle = 0xffffffff;
	DWORD texWidth, texHeight;

	dwTexHandle = pInterface->CreateTexture(pszFileName);
	if (dwTexHandle == 0XFFFFFFFF)
	{
		MessageBox(pInterface->GetWndHandle(),"错误：读取材质图文件失败","错误",MB_OK);
		return ;
	}

	render::Texture *pTexture = pInterface->GetTexture(dwTexHandle);
	texWidth = pTexture->GetWidth();
	texHeight = pTexture->GetHeight();
	for(int  x = 0; x < (int)texWidth; x++)
	{
		for(int z = 0; z < (int)texHeight; z++)
		{
			if (x < m_iWidth && z < m_iDepth)
			{
				Grid *pGrid = GetGrid(x,z);
				DWORD a = pTexture->GetPixel(x,texHeight - 1 - z) & 0x00FFFFFF;
				for(DWORD n = 0; n < 9;n++)
				{
					if (a == dwMaterialColor[n])
					{
                        pGrid->SetMtl(n);
						break;
					}
				}
			}
		}
	}

	pInterface->DestroyTexture(dwTexHandle);
}
