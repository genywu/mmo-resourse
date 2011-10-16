#pragma once

#include "..\frostengine\render.h"
#include "displaymodel.h"

class GameSelectRay
{
	D3DXVECTOR3 m_vecRay[2];//0 - origination原点和1 - direction方向

public:	

	void CalculateRay(const RECT *pRect,render::Camera *pCamera,const POINT &ptRay);
	void CalculateRay(render::Camera *pCamera);
	BOOL TestBoundingBox(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox,D3DXVECTOR3 *pIntersection = NULL,float *pDist = NULL) ;
	BOOL TestPickMesh(GameModel::PickMesh *pPickMesh,D3DXVECTOR3 *pIntersection = NULL,float *pDist = NULL) ;
	BOOL TestGameModel(const D3DXMATRIX *pMatWorld,CDisplayModel *pGameModel,float * pfDis);

	BOOL IntersectTri(const D3DXVECTOR3 pVtx[],D3DXVECTOR3 * pIntersection,float *pDist);
	BOOL IntersectTri(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,D3DXVECTOR3 * pIntersection,float *pDist);

	const D3DXVECTOR3 * GetOrigination() const { return &m_vecRay[0];}
	const D3DXVECTOR3 * GetDirection()   const { return &m_vecRay[1];}

	GameSelectRay(void);
	~GameSelectRay(void);
};
