#pragma once

#include "..\..\frostengine\render.h"


class SelectFrustum
{

	// 4_______5
    // |\	   |\
	// | \ Far | \	
	//7|__\____|6 \
	// \   \0__\___1
	//	\  |	\  |
	//	 \ | Near\ |
	//	  \3______\2

	//6 polgyon
	D3DXVECTOR3 vTop[4],vBottom[4];
	D3DXVECTOR3 vLeft[4],vRight[4];
	D3DXVECTOR3 vNear[4],vFar[4];

	//6 plane
	D3DXPLANE   plane[6];//near far left right top bottom

public:
	SelectFrustum(void);
	~SelectFrustum(void);
	
	void SetFrustum(D3DXVECTOR3 pVertices[]);//8

	BOOL LineIntersectTriangle(D3DXVECTOR3 pLine[],D3DXVECTOR3 pPoly[],D3DXVECTOR3 *pIntersection = NULL);
	BOOL LineIntersectPolygon(D3DXVECTOR3 pLine[],D3DXVECTOR3 pVertices[],int iVerticesCount,D3DXVECTOR3 *pIntersection = NULL);

	float VectorAngle(D3DXVECTOR3 * pV0, D3DXVECTOR3 * pV1);

	BOOL TestPolygon(D3DXVECTOR3 pvPolygon[],int iVertexCount);
	BOOL TestPoint(D3DXVECTOR3 *pvPoint);
	BOOL TestEdge(D3DXVECTOR3 pvEdge[]);
	
	void RenderFrustum();

	void CalculateFrustum(const RECT *pRect,render::Camera *pCamera,const POINT &ptStart,const POINT &ptEnd);
	BOOL TestBoundingBox(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox);
};
