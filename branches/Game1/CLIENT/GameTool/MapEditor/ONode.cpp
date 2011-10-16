#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"

#include "..\..\GameEngine\gamebuildlist.h"

#include "map.h"
#include "CommDB.h"
#include "TerrainMesh.h"
#include "VisibleSet.h"
#include "EditWnd.h"
#include "ToolBuildEdit.h"

#include "onode.h"

ONode::ONode(void)
{
	m_pChildNodeArray[0] = NULL;
	m_pBoundingBox = new render::BoundingBox;
}

ONode::~ONode(void)
{
	SAFEDELETE(m_pBoundingBox);
	//SAFEDELETEARRAY(m_pChildNodeArray);
}

//void ONode::Build(std::vector<int> &vIndexArray,Terrain *pTerrain,EditMap *pMap,DWORD &dwNodeCount)
//{
//	static D3DXVECTOR3 pos[3],center,vMin,vMax; 
//
//	EditWnd *pWndEditor = (EditWnd *)ui::Manager::GetInstance()->GetMainWnd();
//
//
//	//1.JDUGE FACE
//	m_pBoundingBox->Get(vMin,vMax);
//
//	Terrain::_tagVertex *pVertexList = pTerrain->GetVertexList();
//	for(std::vector<int>::iterator it = vIndexArray.begin();it != vIndexArray.end(); it++)
//	{
//		int iGridIndex = *it;
//		Terrain::_tagGrid * pGrid = pTerrain->GetGrid(iGridIndex);
//		//计算grid的坐标
//
//		BOOL bAlreadyPush = FALSE;;
//		pos[0] = pVertexList[pGrid->dwVertexIndex[0]].vPosition;
//		pos[1] = pVertexList[pGrid->dwVertexIndex[1]].vPosition;
//		pos[2] = pVertexList[pGrid->dwVertexIndex[2]].vPosition;
//	
//		//0 1 2
//		center = (pos[0] + pos[1] + pos[2] )/ 3.0f;
//
//		if ((pos[0].x >= vMin.x &&
//			 pos[0].y  >= vMin.y &&
//			 pos[0].z  >= vMin.z &&
//			 pos[0].x <= vMax.x &&
//			 pos[0].y <= vMax.y &&
//			 pos[0].z <= vMax.z) ||
//
//			(pos[1].x >= vMin.x &&
//			 pos[1].y  >= vMin.y &&
//			 pos[1].z  >= vMin.z &&
//  			 pos[1].x <= vMax.x &&
//			 pos[1].y <= vMax.y &&
//			 pos[1].z <= vMax.z) ||
//
//			(pos[2].x >= vMin.x &&
//			 pos[2].y  >= vMin.y &&
//			 pos[2].z  >= vMin.z &&
// 			 pos[2].x <= vMax.x &&
//			 pos[2].y <= vMax.y &&
//			 pos[2].z <= vMax.z) ||
//			 
//			(center.x >= vMin.x &&
//			 center.y  >= vMin.y &&
//			 center.z  >= vMin.z &&
// 			 center.x <= vMax.x &&
//			 center.y <= vMax.y &&
//			 center.z <= vMax.z)) 
//		{
//			m_vGridIndexArray.push_back(iGridIndex);
//			bAlreadyPush = TRUE;
//		}
//		else
//		{
//			if (m_pBoundingBox->TestTriangleIntersected(pos))
//			{
//				m_vGridIndexArray.push_back(iGridIndex);
//				bAlreadyPush = TRUE;
//			}
//		}
//
//		if (!bAlreadyPush)
//		{
//			//0 2 3
//
//			pos[0] = pVertexList[pGrid->dwVertexIndex[0]].vPosition;
//			pos[1] = pVertexList[pGrid->dwVertexIndex[2]].vPosition;
//			pos[2] = pVertexList[pGrid->dwVertexIndex[3]].vPosition;
//
//			center = (pos[0] + pos[1] + pos[2] )/ 3.0f;
//
//			if ((pos[0].x >= vMin.x &&
//				pos[0].y  >= vMin.y &&
//				pos[0].z  >= vMin.z &&
//				pos[0].x <= vMax.x &&
//				pos[0].y <= vMax.y &&
//				pos[0].z <= vMax.z) ||
//
//				(pos[1].x >= vMin.x &&
//				pos[1].y  >= vMin.y &&
//				pos[1].z  >= vMin.z &&
//				pos[1].x <= vMax.x &&
//				pos[1].y <= vMax.y &&
//				pos[1].z <= vMax.z) ||
//
//				(pos[2].x >= vMin.x &&
//				pos[2].y  >= vMin.y &&
//				pos[2].z  >= vMin.z &&
//				pos[2].x <= vMax.x &&
//				pos[2].y <= vMax.y &&
//				pos[2].z <= vMax.z) ||
//
//				(center.x >= vMin.x &&
//				center.y  >= vMin.y &&
//				center.z  >= vMin.z &&
//				center.x <= vMax.x &&
//				center.y <= vMax.y &&
//				center.z <= vMax.z)) 
//			{
//				m_vGridIndexArray.push_back(iGridIndex);
//			}
//			else
//			{
//				if (m_pBoundingBox->TestTriangleIntersected(pos))
//				{
//					m_vGridIndexArray.push_back(iGridIndex);
//				}
//			}
//		}
//	}
//
//	//judge build 
//	ToolBuildEdit *  pToolBuildEdit = pWndEditor->GetToolBuildEdit();
//	GameBuildList *  pGameBuildList = GameBuildList::GetInstance();  	
//
//	static D3DXVECTOR3 vPos[4],vCenter;
//	static D3DXMATRIX matWorld,matRot;
//	for(int n = 0; n < pMap->GetWidth() * pMap->GetDepth();n++)
//	{
//		Terrain::_tagGrid *pGrid = pTerrain->GetGrid(n);
//		Terrain::_tagGrid::tagBuildInfo *pBuildInfo = &pGrid->tBuildInfo;
//		WORD wBuildID = pBuildInfo->wID;
//		if (wBuildID != 0xFFFF)
//		{
//			//计算build点位置
//			float fBuildHeight    = pBuildInfo->fHeight;
//			float fBuildRotationY = pBuildInfo->fRotationY;
//
//			GameBuildList::Build *pBuild = pGameBuildList->GetBuild(wBuildID);
//            
//			pMap->GetGridPosition(n,vPos,&vCenter);
//			D3DXMatrixTranslation(&matWorld,vCenter.x,vCenter.y + fBuildHeight,vCenter.z);
//			D3DXMatrixRotationY(&matRot,fBuildRotationY);
//			D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);
//			//计算build的size box
//			GameBuildList::Build::tagModel *pModel= pBuild->GetModel();
//			GameModel *pGameModel = pModel->pGameModel;
//			if (pGameModel)
//			{
//				//计算与节点的是否相交或者在里面
//				render::BoundingBox *pSizeBox = pGameModel->GetSizeBox();
//				if (m_pBoundingBox->TestBoundingBoxIntersected(&matWorld,pSizeBox))
//				{
//				    //保存index 到数组	
//					m_vBuildGridIndexArray.push_back(n);
//					//OutputConsole("build:index: %d\n",n);
//				}                                     
//			}  			
//		}
//	}
//
//	if (m_vGridIndexArray.size() == 0 && m_vBuildGridIndexArray.size() == 0) return ;
//
//	if (m_vGridIndexArray.size() > MAX_GRID_IN_NODE || m_vBuildGridIndexArray.size() > MAX_BUILD_IN_NODE)
//	{
//		m_pChildNodeArray[0] = new ONode;
//		m_pChildNodeArray[1] = new ONode;
//		m_pChildNodeArray[2] = new ONode;
//		m_pChildNodeArray[3] = new ONode;
//		m_pChildNodeArray[4] = new ONode;
//		m_pChildNodeArray[5] = new ONode;
//		m_pChildNodeArray[6] = new ONode;
//		m_pChildNodeArray[7] = new ONode;
//
//		D3DXVECTOR3 vMin,vMax;
//		m_pBoundingBox->Get(vMin,vMax);
//
//		D3DXVECTOR3 vOfs=(vMax-vMin)*0.5f;
//		
//		m_pChildNodeArray[0]->SetBoundingBox(vMin,vMin+vOfs);
//		m_pChildNodeArray[1]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y,vMin.z),D3DXVECTOR3(vMax.x,vMin.y+vOfs.y,vMin.z+vOfs.z));
//		m_pChildNodeArray[2]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y,vMin.z+vOfs.z),D3DXVECTOR3(vMax.x,vMin.y+vOfs.y,vMax.z));
//		m_pChildNodeArray[3]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y,vMin.z+vOfs.z),D3DXVECTOR3(vMin.x+vOfs.x,vMin.y+vOfs.y,vMax.z));
//		m_pChildNodeArray[4]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y+vOfs.y,vMin.z),D3DXVECTOR3(vMin.x+vOfs.x,vMax.y,vMin.z+vOfs.z));
//		m_pChildNodeArray[5]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y+vOfs.y,vMin.z),D3DXVECTOR3(vMax.x,vMax.y,vMin.z+vOfs.z));
//		m_pChildNodeArray[6]->SetBoundingBox(vMin+vOfs,vMax);
//		m_pChildNodeArray[7]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y+vOfs.y,vMin.z+vOfs.z),D3DXVECTOR3(vMin.x+vOfs.x,vMax.y,vMax.z));
//
//		for(DWORD n = 0; n < 8; n++)
//		{
//			m_pChildNodeArray[n]->Build(m_vGridIndexArray, pTerrain,pMap,dwNodeCount);
//		}
//	}
//}

//void ONode::BuildFast(std::vector<int> &vIndexArray,Terrain *pTerrain,EditMap *pMap,DWORD &dwNodeCount)
//{
//	static D3DXVECTOR3 pos[3],center,vMin,vMax; 
//
//	//m_iNodeCount ++;
//	
////	OutputConsole(":%d\n",m_iNodeCount);
//
//	//1.JDUGE FACE
//	m_pBoundingBox->Get(vMin,vMax);
//
//	Terrain::_tagVertex *pVertexList = pTerrain->GetVertexList();
//	for(std::vector<int>::iterator it = vIndexArray.begin();it != vIndexArray.end(); it++)
//	{
//		int iGridIndex = *it;
//		Terrain::_tagGrid * pGrid = pTerrain->GetGrid(iGridIndex);
//		//计算grid的坐标
//
//		BOOL bAlreadyPush = FALSE;;
//		pos[0] = pVertexList[pGrid->dwVertexIndex[0]].vPosition;
//		pos[1] = pVertexList[pGrid->dwVertexIndex[1]].vPosition;
//		pos[2] = pVertexList[pGrid->dwVertexIndex[2]].vPosition;
//	
//		//0 1 2
//		center = (pos[0] + pos[1] + pos[2] )/ 3.0f;
//
//		if ((pos[0].x >= vMin.x &&
//			 pos[0].y  >= vMin.y &&
//			 pos[0].z  >= vMin.z &&
//			 pos[0].x <= vMax.x &&
//			 pos[0].y <= vMax.y &&
//			 pos[0].z <= vMax.z) ||
//
//			(pos[1].x >= vMin.x &&
//			 pos[1].y  >= vMin.y &&
//			 pos[1].z  >= vMin.z &&
//  			 pos[1].x <= vMax.x &&
//			 pos[1].y <= vMax.y &&
//			 pos[1].z <= vMax.z) ||
//
//			(pos[2].x >= vMin.x &&
//			 pos[2].y  >= vMin.y &&
//			 pos[2].z  >= vMin.z &&
// 			 pos[2].x <= vMax.x &&
//			 pos[2].y <= vMax.y &&
//			 pos[2].z <= vMax.z) ||
//			 
//			(center.x >= vMin.x &&
//			 center.y  >= vMin.y &&
//			 center.z  >= vMin.z &&
// 			 center.x <= vMax.x &&
//			 center.y <= vMax.y &&
//			 center.z <= vMax.z)) 
//		{
//			m_vGridIndexArray.push_back(iGridIndex);
//			bAlreadyPush = TRUE;
//		}
//		else
//		{
//			if (m_pBoundingBox->TestTriangleIntersected(pos))
//			{
//				m_vGridIndexArray.push_back(iGridIndex);
//				bAlreadyPush = TRUE;
//			}
//		}
//
//		if (!bAlreadyPush)
//		{
//			//0 2 3
//
//			pos[0] = pVertexList[pGrid->dwVertexIndex[0]].vPosition;
//			pos[1] = pVertexList[pGrid->dwVertexIndex[2]].vPosition;
//			pos[2] = pVertexList[pGrid->dwVertexIndex[3]].vPosition;
//
//			center = (pos[0] + pos[1] + pos[2] )/ 3.0f;
//
//			if ((pos[0].x >= vMin.x &&
//				pos[0].y  >= vMin.y &&
//				pos[0].z  >= vMin.z &&
//				pos[0].x <= vMax.x &&
//				pos[0].y <= vMax.y &&
//				pos[0].z <= vMax.z) ||
//
//				(pos[1].x >= vMin.x &&
//				pos[1].y  >= vMin.y &&
//				pos[1].z  >= vMin.z &&
//				pos[1].x <= vMax.x &&
//				pos[1].y <= vMax.y &&
//				pos[1].z <= vMax.z) ||
//
//				(pos[2].x >= vMin.x &&
//				pos[2].y  >= vMin.y &&
//				pos[2].z  >= vMin.z &&
//				pos[2].x <= vMax.x &&
//				pos[2].y <= vMax.y &&
//				pos[2].z <= vMax.z) ||
//
//				(center.x >= vMin.x &&
//				center.y  >= vMin.y &&
//				center.z  >= vMin.z &&
//				center.x <= vMax.x &&
//				center.y <= vMax.y &&
//				center.z <= vMax.z)) 
//			{
//				m_vGridIndexArray.push_back(iGridIndex);
//			}
//			else
//			{
//				if (m_pBoundingBox->TestTriangleIntersected(pos))
//				{
//					m_vGridIndexArray.push_back(iGridIndex);
//				}
//			}
//		}
//	}
//
//	//judge build 
//	WndEditor		  *  pWndEditor       = (WndEditor *)ui::Manager::GetInstance()->GetMainWnd();
//	ToolBuildEdit *  pToolBuildEdit = pWndEditor->GetToolBuildEdit();
//	GameBuildList *  pGameBuildList = GameBuildList::GetInstance();  	
//
//	static D3DXVECTOR3 vPos[4],vCenter;
//	static D3DXMATRIX matWorld,matRot;
//	for(int n = 0; n < pMap->GetWidth() * pMap->GetDepth();n++)
//	{
//		Terrain::_tagGrid *pGrid = pTerrain->GetGrid(n);
//		Terrain::_tagGrid::tagBuildInfo *pBuildInfo = &pGrid->tBuildInfo;
//		WORD wBuildID = pBuildInfo->wID;
//		if (wBuildID != 0xFFFF)
//		{
//			//计算build点位置
//			float fBuildHeight    = pBuildInfo->fHeight;
//			float fBuildRotationY = pBuildInfo->fRotationY;
//
//			GameBuildList::Build *pBuild = pGameBuildList->GetBuild(wBuildID);
//            
//			pMap->GetGridPosition(n,vPos,&vCenter);
//			D3DXMatrixTranslation(&matWorld,vCenter.x,vCenter.y + fBuildHeight,vCenter.z);
//			D3DXMatrixRotationY(&matRot,fBuildRotationY);
//			D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);
//			//计算build的size box
//			GameBuildList::Build::tagModel *pModel= pBuild->GetModel();
//			GameModel *pGameModel = pModel->pGameModel;
//			if (pGameModel)
//			{
//				//计算与节点的是否相交或者在里面
//				render::BoundingBox *pSizeBox = pGameModel->GetSizeBox();
//				if (m_pBoundingBox->TestBoundingBoxIntersectedFast(&matWorld,pSizeBox))
//				{
//				    //保存index 到数组	
//					m_vBuildGridIndexArray.push_back(n);
//					//OutputConsole("build:index: %d\n",n);
//				}                                     
//			}  			
//		}
//	}
//
//	if (m_vGridIndexArray.size() == 0 && m_vBuildGridIndexArray.size() == 0) return ;
//
//	if (m_vGridIndexArray.size() > MAX_GRID_IN_NODE || m_vBuildGridIndexArray.size() > MAX_BUILD_IN_NODE)
//	{
//		m_pChildNodeArray[0] = new ONode;
//		m_pChildNodeArray[1] = new ONode;
//		m_pChildNodeArray[2] = new ONode;
//		m_pChildNodeArray[3] = new ONode;
//		m_pChildNodeArray[4] = new ONode;
//		m_pChildNodeArray[5] = new ONode;
//		m_pChildNodeArray[6] = new ONode;
//		m_pChildNodeArray[7] = new ONode;
//
//		D3DXVECTOR3 vMin,vMax;
//		m_pBoundingBox->Get(vMin,vMax);
//
//		D3DXVECTOR3 vOfs=(vMax-vMin)*0.5f;
//		
//		m_pChildNodeArray[0]->SetBoundingBox(vMin,vMin+vOfs);
//		m_pChildNodeArray[1]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y,vMin.z),D3DXVECTOR3(vMax.x,vMin.y+vOfs.y,vMin.z+vOfs.z));
//		m_pChildNodeArray[2]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y,vMin.z+vOfs.z),D3DXVECTOR3(vMax.x,vMin.y+vOfs.y,vMax.z));
//		m_pChildNodeArray[3]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y,vMin.z+vOfs.z),D3DXVECTOR3(vMin.x+vOfs.x,vMin.y+vOfs.y,vMax.z));
//		m_pChildNodeArray[4]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y+vOfs.y,vMin.z),D3DXVECTOR3(vMin.x+vOfs.x,vMax.y,vMin.z+vOfs.z));
//		m_pChildNodeArray[5]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y+vOfs.y,vMin.z),D3DXVECTOR3(vMax.x,vMax.y,vMin.z+vOfs.z));
//		m_pChildNodeArray[6]->SetBoundingBox(vMin+vOfs,vMax);
//		m_pChildNodeArray[7]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y+vOfs.y,vMin.z+vOfs.z),D3DXVECTOR3(vMin.x+vOfs.x,vMax.y,vMax.z));
//
//		for(DWORD n = 0; n < 8; n++)
//		{
//			m_pChildNodeArray[n]->BuildFast(m_vGridIndexArray, pTerrain,pMap,dwNodeCount);
//		}
//	}
//}

void ONode::OptimizeBuild(std::vector<int> &vParentNodeGridIndexArray,std::vector<int> &vParentNodeBuildGridIndexArray,DWORD &dwNodeCount)
{
	static D3DXVECTOR3 pos[3],center,vMin,vMax; 
	EditWnd *pWndEditor = (EditWnd *)ui::Manager::GetInstance()->GetMainWnd();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh *pTerrain = CommDB::GetInstance()->GetTerrainMesh();
	//Terrain *pTerrain = pMap->GetTerrain();
	ToolBuildEdit *  pToolBuildEdit = pWndEditor->GetToolBuildEdit();
	GameBuildList *  pGameBuildList = GameBuildList::GetInstance();  	
	TerrainMesh::_tagVertex *pVertexList = pTerrain->GetVertexList();

	dwNodeCount++;
	
	//1.JDUGE FACE
	m_pBoundingBox->Get(vMin,vMax);
	std::vector<int>::iterator it;
	for(it = vParentNodeGridIndexArray.begin();it != vParentNodeGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		TerrainMesh::_tagGrid * pGrid = pTerrain->GetGrid(iGridIndex);
		//计算grid的坐标

		BOOL bAlreadyPush = FALSE;;
		pos[0] = pVertexList[pGrid->dwVertexIndex[0]].vPosition;
		pos[1] = pVertexList[pGrid->dwVertexIndex[1]].vPosition;
		pos[2] = pVertexList[pGrid->dwVertexIndex[2]].vPosition;
	
		//0 1 2
		center = (pos[0] + pos[1] + pos[2] )/ 3.0f;

		if ((pos[0].x >= vMin.x &&
			 pos[0].y  >= vMin.y &&
			 pos[0].z  >= vMin.z &&
			 pos[0].x <= vMax.x &&
			 pos[0].y <= vMax.y &&
			 pos[0].z <= vMax.z) ||

			(pos[1].x >= vMin.x &&
			 pos[1].y  >= vMin.y &&
			 pos[1].z  >= vMin.z &&
  			 pos[1].x <= vMax.x &&
			 pos[1].y <= vMax.y &&
			 pos[1].z <= vMax.z) ||

			(pos[2].x >= vMin.x &&
			 pos[2].y  >= vMin.y &&
			 pos[2].z  >= vMin.z &&
 			 pos[2].x <= vMax.x &&
			 pos[2].y <= vMax.y &&
			 pos[2].z <= vMax.z) ||
			 
			(center.x >= vMin.x &&
			 center.y  >= vMin.y &&
			 center.z  >= vMin.z &&
 			 center.x <= vMax.x &&
			 center.y <= vMax.y &&
			 center.z <= vMax.z)) 
		{
			m_vGridIndexArray.push_back(iGridIndex);
			bAlreadyPush = TRUE;
		}
		else
		{
			if (m_pBoundingBox->TestTriangleIntersected(pos))
			{
				m_vGridIndexArray.push_back(iGridIndex);
				bAlreadyPush = TRUE;
			}
		}

		if (!bAlreadyPush)
		{
			//0 2 3

			pos[0] = pVertexList[pGrid->dwVertexIndex[0]].vPosition;
			pos[1] = pVertexList[pGrid->dwVertexIndex[2]].vPosition;
			pos[2] = pVertexList[pGrid->dwVertexIndex[3]].vPosition;

			center = (pos[0] + pos[1] + pos[2] )/ 3.0f;

			if ((pos[0].x >= vMin.x &&
				pos[0].y  >= vMin.y &&
				pos[0].z  >= vMin.z &&
				pos[0].x <= vMax.x &&
				pos[0].y <= vMax.y &&
				pos[0].z <= vMax.z) ||

				(pos[1].x >= vMin.x &&
				pos[1].y  >= vMin.y &&
				pos[1].z  >= vMin.z &&
				pos[1].x <= vMax.x &&
				pos[1].y <= vMax.y &&
				pos[1].z <= vMax.z) ||

				(pos[2].x >= vMin.x &&
				pos[2].y  >= vMin.y &&
				pos[2].z  >= vMin.z &&
				pos[2].x <= vMax.x &&
				pos[2].y <= vMax.y &&
				pos[2].z <= vMax.z) ||

				(center.x >= vMin.x &&
				center.y  >= vMin.y &&
				center.z  >= vMin.z &&
				center.x <= vMax.x &&
				center.y <= vMax.y &&
				center.z <= vMax.z)) 
			{
				m_vGridIndexArray.push_back(iGridIndex);
			}
			else
			{
				if (m_pBoundingBox->TestTriangleIntersected(pos))
				{
					m_vGridIndexArray.push_back(iGridIndex);
				}
			}
		}
	}

	//judge build 
	static D3DXVECTOR3 vPos[4],vCenter;
	static D3DXMATRIX matWorld,matRot;
	GameBuildList::Build *pBuild;
	for(it = vParentNodeBuildGridIndexArray.begin();it != vParentNodeBuildGridIndexArray.end();it++)
	{
		DWORD dwGridIndex = *it;
		TerrainMesh::_tagGrid *pGrid = pTerrain->GetGrid(dwGridIndex);
		Grid *pMapGrid = pMap->GetGrid(dwGridIndex);
		
		WORD wBuildID = pMapGrid->GetBuild()->wID;
		if (wBuildID != 0xFFFF)
		{
			//计算build点位置
			float fBuildHeight    = pMapGrid->GetBuild()->fHeight;
			float fBuildRotationY =pMapGrid->GetBuild()->fRotationY;

			pBuild = pGameBuildList->AcquireBuild(wBuildID,false);
            
			pMap->GetGridPosition(dwGridIndex,vPos,&vCenter);
			D3DXMatrixTranslation(&matWorld,vCenter.x,vCenter.y + fBuildHeight,vCenter.z);
			D3DXMatrixRotationY(&matRot,fBuildRotationY);
			D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);
			//计算build的size box
			GameBuildList::Build::tagModel *pModel= pBuild->GetModel();
			if (pModel)
			{
				CDisplayModel *pGameModel = pModel->pGameModel;
				if (pGameModel)
				{
					//计算与节点的是否相交或者在里面
					render::BoundingBox *pSizeBox = pGameModel->GetSizeBox();
					if (m_pBoundingBox->GetOutsideRadius() == 0.0000f)
					{
						int x = 0;
					}
					if (m_pBoundingBox->TestBoundingBoxIntersected(&matWorld,pSizeBox))
					{
						//保存index 到数组	
						m_vBuildGridIndexArray.push_back(dwGridIndex);
						//OutputConsole("build:index: %d\n",n);
					}                                     
				}  			
			}
			else
			{
				//1x1单位
				render::BoundingBox cSizeBox;
				cSizeBox.Set(D3DXVECTOR3(0,0,0),0.5f);
				if (m_pBoundingBox->TestBoundingBoxIntersected(&matWorld,&cSizeBox))
				{
					//保存index 到数组	
					m_vBuildGridIndexArray.push_back(dwGridIndex);
					//OutputConsole("build:index: %d\n",n);
				}                                     


			}
		}
	}

	if (m_vGridIndexArray.size() == 0 && m_vBuildGridIndexArray.size() == 0) return ;

	if (m_vGridIndexArray.size() > MAX_GRID_IN_NODE || m_vBuildGridIndexArray.size() > MAX_BUILD_IN_NODE)
	{
		m_pChildNodeArray[0] = new ONode;
		m_pChildNodeArray[1] = new ONode;
		m_pChildNodeArray[2] = new ONode;
		m_pChildNodeArray[3] = new ONode;
		m_pChildNodeArray[4] = new ONode;
		m_pChildNodeArray[5] = new ONode;
		m_pChildNodeArray[6] = new ONode;
		m_pChildNodeArray[7] = new ONode;

		D3DXVECTOR3 vMin,vMax;
		m_pBoundingBox->Get(vMin,vMax);
		if (vMin == vMax)
		{
			char str[512];
			sprintf(str,"%sSIZEBOX太大,导致空间上有点共享的建物的SIZEBOX超过最大数16",CGameModelManager::GetInstance()->GetGameModelInfoFileName(pBuild->GetModel()->pGameModel->GetGameModelID()));
			//CGameModelManager::GetInstance()->GetGameModel(pBuild->GetModel()->pGameModel->GetGameModelID())->GetModelFileName();
			MessageBox(NULL,str,"错误",MB_OK);
		}

		D3DXVECTOR3 vOfs=(vMax-vMin)*0.5f;
		
		m_pChildNodeArray[0]->SetBoundingBox(vMin,vMin+vOfs);
		m_pChildNodeArray[1]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y,vMin.z),D3DXVECTOR3(vMax.x,vMin.y+vOfs.y,vMin.z+vOfs.z));
		m_pChildNodeArray[2]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y,vMin.z+vOfs.z),D3DXVECTOR3(vMax.x,vMin.y+vOfs.y,vMax.z));
		m_pChildNodeArray[3]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y,vMin.z+vOfs.z),D3DXVECTOR3(vMin.x+vOfs.x,vMin.y+vOfs.y,vMax.z));
		m_pChildNodeArray[4]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y+vOfs.y,vMin.z),D3DXVECTOR3(vMin.x+vOfs.x,vMax.y,vMin.z+vOfs.z));
		m_pChildNodeArray[5]->SetBoundingBox(D3DXVECTOR3(vMin.x+vOfs.x,vMin.y+vOfs.y,vMin.z),D3DXVECTOR3(vMax.x,vMax.y,vMin.z+vOfs.z));
		m_pChildNodeArray[6]->SetBoundingBox(vMin+vOfs,vMax);
		m_pChildNodeArray[7]->SetBoundingBox(D3DXVECTOR3(vMin.x,vMin.y+vOfs.y,vMin.z+vOfs.z),D3DXVECTOR3(vMin.x+vOfs.x,vMax.y,vMax.z));

		for(DWORD n = 0; n < 8; n++)
		{
			m_pChildNodeArray[n]->OptimizeBuild(m_vGridIndexArray, m_vBuildGridIndexArray,dwNodeCount);
		}
	}
}

BOOL ONode::IsLeaf()
{
	if (!m_pChildNodeArray[0])
		return TRUE;
	else
		return FALSE;
}

void ONode::Write(FILE * file)
{
	//自己的数据
	D3DXVECTOR3 vMin,vMax;
	m_pBoundingBox->Get(vMin,vMax);
	fwrite(&vMin,sizeof(D3DXVECTOR3),1,file);
	fwrite(&vMax,sizeof(D3DXVECTOR3),1,file);
	//格子的列表
	int iGridIndexCount = (int)m_vGridIndexArray.size();
	fwrite(&iGridIndexCount,sizeof(int),1,file);
	for(int i = 0; i < iGridIndexCount; i++)
	{
		int iIndex = m_vGridIndexArray[i];
		fwrite(&iIndex,sizeof(int),1,file);
	}
	//建筑的列表
	int iBuildGridIndexCount = (int)m_vBuildGridIndexArray.size();
	fwrite(&iBuildGridIndexCount,sizeof(int),1,file);
	for(int i = 0; i < iBuildGridIndexCount; i++)
	{
		int iIndex = m_vBuildGridIndexArray[i];
		fwrite(&iIndex,sizeof(int),1,file);
	}
	//子节点的数据
	BOOL bIsLeaf = (m_pChildNodeArray[0])? FALSE : TRUE;
	fwrite(&bIsLeaf,sizeof(BOOL),1,file);
	if (!bIsLeaf)
	{
		for(int iChild=0;iChild<8;iChild++)	
		{
			m_pChildNodeArray[iChild]->Write(file);
		}	
	}
}

void ONode::SetBoundingBox(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax)
{
	m_pBoundingBox->Set(vMin,vMax);
}
