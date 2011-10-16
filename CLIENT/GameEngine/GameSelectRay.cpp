#include "StdAfx.h"

#include "..\frostengine\console.h"
#include "..\frostengine\utility.h"


#include "gamemodel.h"
#include "GameSelectRay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GameSelectRay::GameSelectRay(void)
{
}

GameSelectRay::~GameSelectRay(void)
{
}

void GameSelectRay::CalculateRay(const RECT *pRect,render::Camera *pCamera,const POINT &ptPoint)
{
	//get pick position 
	int iX = pRect->left,iY = pRect->top,iW = pRect->right -pRect->left + 1,iH = pRect->bottom - pRect->top + 1;

	POINT pt = ptPoint;
	//clip to rect
	pt.x -= pRect->left;
	pt.y -= pRect->top;

	D3DXMATRIX matInvView;
	const D3DXMATRIX *pMatProj;

	pMatProj = pCamera->GetProjectionMatrix();
	D3DXMatrixInverse(&matInvView,NULL,pCamera->GetViewMatrix());

	// Compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * pt.x ) / iW) - 1 ) / pMatProj->_11;
	v.y = -( ( ( 2.0f * pt.y ) / iH) - 1 ) / pMatProj->_22;
	v.z =  1.0f;

	// Transform the screen space pick ray into 3D space
	D3DXVECTOR3 vecPickRayDir,vecPickRayOrig;
	m_vecRay[1].x  = v.x*matInvView._11 + v.y*matInvView._21 + v.z*matInvView._31;
	m_vecRay[1].y  = v.x*matInvView._12 + v.y*matInvView._22 + v.z*matInvView._32;
	m_vecRay[1].z  = v.x*matInvView._13 + v.y*matInvView._23 + v.z*matInvView._33;

	m_vecRay[0].x = matInvView._41;
	m_vecRay[0].y = matInvView._42;
	m_vecRay[0].z = matInvView._43;	
}

void GameSelectRay::CalculateRay(render::Camera *pCamera)
{
	m_vecRay[1] = * pCamera->GetView() - * pCamera->GetPosition();
	D3DXVec3Normalize(&m_vecRay[1],&m_vecRay[1]);
	m_vecRay[0] = * pCamera->GetPosition();
}

BOOL GameSelectRay::IntersectTri(const D3DXVECTOR3 pVtx[],D3DXVECTOR3 * pIntersection,float *pDist)
{
	float u,v,d;
	BOOL bResult = D3DXIntersectTri(&pVtx[0],&pVtx[1],&pVtx[2],&m_vecRay[0],&m_vecRay[1],&u,&v,&d);
	if (pDist)
	{
		*pDist = d;
	}

	if (pIntersection)
		*pIntersection = pVtx[0] + u * (pVtx[1] - pVtx[0]) + v * (pVtx[2] - pVtx[0]); 
	return bResult;
}

BOOL GameSelectRay::IntersectTri(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,D3DXVECTOR3 * pIntersection,float *pDist)
{
	float u,v,d;
	BOOL bResult = D3DXIntersectTri(pVtx0,pVtx1,pVtx2,&m_vecRay[0],&m_vecRay[1],&u,&v,&d);
	if (pDist)
	{
		*pDist = d;
	}

	if (pIntersection)
		*pIntersection = *pVtx0 + u * (*pVtx1 - *pVtx0) + v * (*pVtx2 - *pVtx0); 
	return bResult;
}

BOOL GameSelectRay::TestBoundingBox(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox,D3DXVECTOR3 *pIntersection,float *pDist) 
{
	if(NULL == pBoundingBox)	return FALSE;

	static const int iTrianglesIndices[ 3 * 12] = { 
		0,2,1, 0,3,2, 4,6,5, 4,7,6,
			1,5,6, 1,6,2, 0,4,7, 0,7,3,
			0,4,5, 0,5,1, 3,7,6, 3,6,3};

	//²âÊÔÏà½»
	D3DXVECTOR3 vPos[3];
	const D3DXVECTOR3 *pConner = pBoundingBox->GetCornerVertex();

	*pDist = 10000000.0f;
	float fDistTemp = 10000000.0f;
	int n = 0; 
	for(int iTriangle = 0; iTriangle < 12; iTriangle++)
	{
		D3DXVec3TransformCoord(&vPos[0],&pConner[iTrianglesIndices[n++]],pMatWorld);
		D3DXVec3TransformCoord(&vPos[1],&pConner[iTrianglesIndices[n++]],pMatWorld);
		D3DXVec3TransformCoord(&vPos[2],&pConner[iTrianglesIndices[n++]],pMatWorld);
		BOOL bFound = IntersectTri(vPos,pIntersection,&fDistTemp);
		if ( fDistTemp < *pDist )
		{
			*pDist = fDistTemp;
		}

		if (bFound)
			return TRUE;
	}

	return FALSE;
}

BOOL GameSelectRay::TestGameModel(const D3DXMATRIX *pMatWorld,CDisplayModel *pGameModel,float *pfDis)
{
	static D3DXVECTOR3 vPos[3];
	std::map<DWORD,CDisplayModel::CDisplayGroup* > MapGroup = pGameModel->GetGroups();
	if (pGameModel->GetLoadState() != Load_Did)
	{
		return FALSE;
	}
	for(std::map<DWORD,CDisplayModel::CDisplayGroup* >::iterator itGroup = MapGroup.begin(); itGroup != 
		MapGroup.end(); itGroup ++)
	{
		CDisplayModel::CDisplayGroup *pGroup = itGroup->second;

		GameModel::Group::Part::Level* pLevel= pGroup->GetLevelInfo();
		if (pLevel)
		{

			for(DWORD n = 0; n < (DWORD)pLevel->GetRMCount();n++)
			{

				model::Mesh *pMesh = pLevel->GetRMArray().at(n)->pMesh;
				const model::Mesh::tagTriangle *pTriBuffer = pMesh->GetTriangleBuffer();
				const D3DXVECTOR3 *pPosBuffer = pMesh->GetPosBuffer();

				for(DWORD dwTri = 0; dwTri < pMesh->GetTriangleCount(); dwTri++)
				{
					vPos[0] = pPosBuffer[pTriBuffer[dwTri].dwPNCIndex[0]];
					vPos[1] = pPosBuffer[pTriBuffer[dwTri].dwPNCIndex[1]];
					vPos[2] = pPosBuffer[pTriBuffer[dwTri].dwPNCIndex[2]];

					D3DXVec3TransformCoord(&vPos[0],&vPos[0],pMatWorld);
					D3DXVec3TransformCoord(&vPos[1],&vPos[1],pMatWorld);
					D3DXVec3TransformCoord(&vPos[2],&vPos[2],pMatWorld);
					BOOL bFound = IntersectTri(vPos,NULL,pfDis);
					if (bFound)
						return TRUE;
				}
			}
		}

	}

	return FALSE;
}
BOOL GameSelectRay::TestPickMesh(GameModel::PickMesh *pPickMesh,D3DXVECTOR3 *pIntersection,float *pDist)
{

	static D3DXVECTOR3 vPos[3];
	if (pPickMesh == NULL)
	{
		return FALSE;
	}
	if (pPickMesh->GetLoadState() != Load_Did)
	{
		return FALSE;
	}
	model::Mesh* pMesh = pPickMesh->GetMesh();
	model::Buffer* pBuffer = pPickMesh->GetBuffer();
	const model::Mesh::tagTriangle *pTriBuffer = pMesh->GetTriangleBuffer();
	const D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();

	for(DWORD dwTri = 0; dwTri < pMesh->GetTriangleCount(); dwTri++)
	{
		vPos[0] = pPosBuffer[pTriBuffer[dwTri].dwPNCIndex[0]];
		vPos[1] = pPosBuffer[pTriBuffer[dwTri].dwPNCIndex[1]];
		vPos[2] = pPosBuffer[pTriBuffer[dwTri].dwPNCIndex[2]];
		BOOL bFound = IntersectTri(vPos,NULL,pDist);
		if (bFound)
			return TRUE;
	}

	return FALSE;
}