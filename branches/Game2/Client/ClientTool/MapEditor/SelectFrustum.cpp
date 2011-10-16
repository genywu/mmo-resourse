
#include "stdafx.h"
#include "selectfrustum.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "..\..\engine\render.h"


SelectFrustum::SelectFrustum(void)
{
	ZeroMemory(&vTop,4*sizeof(D3DXVECTOR3));
	ZeroMemory(&vBottom,4*sizeof(D3DXVECTOR3));
	ZeroMemory(&vLeft,4*sizeof(D3DXVECTOR3));
	ZeroMemory(&vRight,4*sizeof(D3DXVECTOR3));
	ZeroMemory(&vNear,4*sizeof(D3DXVECTOR3));
	ZeroMemory(&vFar,4*sizeof(D3DXVECTOR3));
}

SelectFrustum::~SelectFrustum(void)
{
}

void SelectFrustum::SetFrustum(D3DXVECTOR3 pVertices[])
{
	vNear  [0] = pVertices[0];vNear  [1] = pVertices[1]; vNear  [2] = pVertices[2]; vNear  [3] = pVertices[3];
	vFar   [0] = pVertices[7];vFar   [1] = pVertices[6]; vFar   [2] = pVertices[5]; vFar   [3] = pVertices[4];
	vLeft  [0] = pVertices[4];vLeft  [1] = pVertices[0]; vLeft  [2] = pVertices[3]; vLeft  [3] = pVertices[7];
	vRight [0] = pVertices[1];vRight [1] = pVertices[5]; vRight [2] = pVertices[6]; vRight [3] = pVertices[2];
	vTop   [0] = pVertices[0];vTop   [1] = pVertices[4]; vTop   [2] = pVertices[5]; vTop   [3] = pVertices[1];
	vBottom[0] = pVertices[3];vBottom[1] = pVertices[2]; vBottom[2] = pVertices[6]; vBottom[3] = pVertices[7];

	D3DXPlaneFromPoints( &plane[0], &vNear[2],&vNear[1], &vNear[0] ); // Near
	D3DXPlaneFromPoints( &plane[1], &vFar[2],&vFar[1], &vFar[0] ); // Far
	D3DXPlaneFromPoints( &plane[2], &vLeft[2],&vLeft[1], &vLeft[0] ); // Left
	D3DXPlaneFromPoints( &plane[3], &vRight[2],&vRight[1], &vRight[0] ); // Right
	D3DXPlaneFromPoints( &plane[4], &vTop[2],&vTop[1], &vTop[0] ); // Top
	D3DXPlaneFromPoints( &plane[5], &vBottom[2],&vBottom[1], &vBottom[0] ); // Bottom
}

BOOL SelectFrustum::LineIntersectTriangle(D3DXVECTOR3 pLine[],D3DXVECTOR3 pPoly[],D3DXVECTOR3 *pIntersection)
{
	//1. 求平面方程
	D3DXVECTOR3 vA,vB,vNormal;
	vA = pPoly[2] - pPoly[0];
	vB = pPoly[1] - pPoly[0];
	D3DXVec3Cross(&vNormal,&vA,&vB);
	D3DXVec3Normalize(&vNormal,&vNormal);

	float D = - D3DXVec3Dot(&pPoly[0],&vNormal);

	float dis0 = D3DXVec3Dot(&pLine[0],&vNormal) + D;

	float dis1 = D3DXVec3Dot(&pLine[1],&vNormal) + D;

	if (dis0 * dis1 >= 0 ) return FALSE;

	//2.求交点
	D3DXVECTOR3 vPoint,vLineDir;
	float Numerator = 0.0f, Denominator = 0.0f, dist = 0.0f;

	vLineDir = pLine[1] - pLine[0];
	D3DXVec3Normalize(&vLineDir,&vLineDir);

	Numerator = - ( D3DXVec3Dot(&vNormal,&pLine[0]) + D);

	Denominator = D3DXVec3Dot(&vNormal,&vLineDir);

	D3DXVECTOR3 vIntersection;
	if( Denominator == 0.0f)	
	{
		vIntersection = pLine[0];
	}
	else
	{
		float	dist = Numerator / Denominator;

		vIntersection.x = (float)(pLine[0].x + (vLineDir.x * dist));
		vIntersection.y = (float)(pLine[0].y + (vLineDir.y * dist));
		vIntersection.z = (float)(pLine[0].z + (vLineDir.z * dist));
	}
	
	if (pIntersection) 
		*pIntersection = vIntersection;

	//3.判断交点是否在平面内
	float fAngle = 0.0;						

	for (int i = 0; i < 3; i++)
	{
		vA = pPoly[i] - vIntersection;	
		vB = pPoly[(i + 1) % 3] - vIntersection;

		fAngle += VectorAngle(&vA,&vB);	
	}

	if(fAngle >= 0.9999f * 2.0 * 3.1415826f )	// If the angle is greater than 2 PI, (360 degrees)
		return TRUE;							// The point is inside of the polygon

	return FALSE;							
}

BOOL SelectFrustum::LineIntersectPolygon(D3DXVECTOR3 pLine[],D3DXVECTOR3 pVertices[],int iVerticesCount,D3DXVECTOR3 *pIntersection)
{
	ASSERT(iVerticesCount >= 3);
	//make polygon
	D3DXVECTOR3 v[3];
	int iTriangleCount = iVerticesCount -2;
	for(int iTri = 0; iTri < iTriangleCount; iTri++)
	{
		v[0] = pVertices[iTri];
		v[1] = pVertices[iTri+1];
		v[2] = pVertices[iVerticesCount -1];
		if (LineIntersectTriangle(pLine,v,pIntersection)) return TRUE;					
	}
	return FALSE;
}

float SelectFrustum::VectorAngle(D3DXVECTOR3 * pV0, D3DXVECTOR3 * pV1)
{
	float fDot = D3DXVec3Dot(pV0,pV1);				

	float fMagnitude = sqrtf( pV0->x*pV0->x + pV0->y*pV0->y + pV0->z*pV0->z) * sqrtf(pV1->x*pV1->x + pV1->y*pV1->y + pV1->z*pV1->z);
	double angle = acos( fDot / fMagnitude );

	if(_isnan(angle))	return 0.0f;

	return (float)angle;
}

BOOL SelectFrustum::TestEdge(D3DXVECTOR3 pvEdge[])
{
	//1.test pt
	if (TestPoint(&pvEdge[0])) return TRUE;
	if (TestPoint(&pvEdge[1])) return TRUE;
	//2.test intersect
	BOOL        bIntersect[6];
	ZeroMemory(bIntersect,sizeof(BOOL)*6);
	//1.测试线段和六个polygon是否相交
	bIntersect[0] = LineIntersectPolygon(pvEdge,vNear,4);
	bIntersect[1] = LineIntersectPolygon(pvEdge,vFar,4);
	bIntersect[2] = LineIntersectPolygon(pvEdge,vLeft,4);
	bIntersect[3] = LineIntersectPolygon(pvEdge,vRight,4);
	bIntersect[4] = LineIntersectPolygon(pvEdge,vTop,4); 
	bIntersect[5] = LineIntersectPolygon(pvEdge,vBottom,4);

	BOOL bS = FALSE;

	for(int i = 0; i< 6; i++)
	{
		bS  |= bIntersect[i];
	}

	if (bS) return TRUE;

	return FALSE;
}

BOOL SelectFrustum::TestPoint(D3DXVECTOR3 *pvPoint)
{
	float x = pvPoint->x,y = pvPoint->y , z = pvPoint->z;
	for(int i = 0; i < 6; i++ )
	{
		if(plane[i].a * x + plane[i].b * y + plane[i].c * z + plane[i].d <= 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL SelectFrustum::TestPolygon(D3DXVECTOR3 pvPolygon[],int iVertexCount)
{
	//test polygon pt in bound
	for(int i = 0; i < iVertexCount; i++)
	{
		if (TestPoint(&pvPolygon[i])) return TRUE;
	}
	//test bound edge intersect polygon
	//6 line 
	D3DXVECTOR3 vIntersection[6];
	D3DXVECTOR3 line[2];

	line[0] = vNear[0];
	line[1] = vNear[1];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vNear[1];
	line[1] = vNear[2];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vNear[2];
	line[1] = vNear[3];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vNear[3];
	line[1] = vNear[0];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;

	line[0] = vFar[0];
	line[1] = vFar[1];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vFar[1];
	line[1] = vFar[2];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vFar[2];
	line[1] = vFar[3];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vFar[3];
	line[1] = vFar[0];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;


	line[0] = vTop[0];
	line[1] = vTop[1];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vTop[1];
	line[1] = vTop[2];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vTop[2];
	line[1] = vTop[3];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vTop[3];
	line[1] = vTop[0];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;

	line[0] = vBottom[0];
	line[1] = vBottom[1];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vBottom[1];
	line[1] = vBottom[2];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vBottom[2];
	line[1] = vBottom[3];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;
	line[0] = vBottom[3];
	line[1] = vBottom[0];
	if (LineIntersectPolygon(line,pvPolygon,iVertexCount)) return TRUE;

	return FALSE;
}

void SelectFrustum::RenderFrustum()
{

	//pD3D->AppendElementSolidQuad3D(&vNear[0],&vNear[1],&vNear[2],&vNear[3],D3DCOLOR_ARGB(200,255,0,0));
	//pD3D->AppendElementSolidQuad3D(&vFar[0],&vFar[1],&vFar[2],&vFar[3],D3DCOLOR_ARGB(200,255,0,0));
	//pD3D->AppendElementSolidQuad3D(&vLeft[0],&vLeft[1],&vLeft[2],&vLeft[3],D3DCOLOR_ARGB(200,0,0,255));
	//pD3D->AppendElementSolidQuad3D(&vRight[0],&vRight[1],&vRight[2],&vRight[3],D3DCOLOR_ARGB(200,0,0,255));
	//pD3D->AppendElementSolidQuad3D(&vTop[0],&vTop[1],&vTop[2],&vTop[3],D3DCOLOR_ARGB(200,0,255,0));
	//pD3D->AppendElementSolidQuad3D(&vBottom[0],&vBottom[1],&vBottom[2],&vBottom[3],D3DCOLOR_ARGB(200,0,255,0));

	/*
	//normal near
	D3DXVECTOR3 vNormal,vCenter;
	D3DXVECTOR3 vA,vB;

	vA = vNear[2] -vNear[0];
	vB = vNear[1] -vNear[0];
	D3DXVec3Cross(&vNormal,&vA,&vB);
	D3DXVec3Normalize(&vNormal,&vNormal);
	vCenter = vNear[0];
	//	pD3D->AppendElementLine3D(&vCenter,&D3DXVECTOR3(vCenter+vNormal),D3DCOLOR_ARGB(255,0,0,0),D3DCOLOR_ARGB(255,0,255,0));

	vA = vFar[2] -vFar[0];
	vB = vFar[1] -vFar[0];
	D3DXVec3Cross(&vNormal,&vA,&vB);
	D3DXVec3Normalize(&vNormal,&vNormal);
	vCenter = vFar[0];
	//	pD3D->AppendElementLine3D(&vCenter,&D3DXVECTOR3(vCenter+vNormal),D3DCOLOR_ARGB(255,0,0,0),D3DCOLOR_ARGB(255,0,255,0));

	vA = vLeft[2] -vLeft[0];
	vB = vLeft[1] -vLeft[0];
	D3DXVec3Cross(&vNormal,&vA,&vB);
	D3DXVec3Normalize(&vNormal,&vNormal);
	vCenter = vLeft[1];
	//	pD3D->AppendElementLine3D(&vCenter,&D3DXVECTOR3(vCenter+vNormal),D3DCOLOR_ARGB(255,0,0,0),D3DCOLOR_ARGB(255,0,255,0));

	vA = vRight[2] -vRight[0];
	vB = vRight[1] -vRight[0];
	D3DXVec3Cross(&vNormal,&vA,&vB);
	D3DXVec3Normalize(&vNormal,&vNormal);
	vCenter = vRight[2];
	//	pD3D->AppendElementLine3D(&vCenter,&D3DXVECTOR3(vCenter+vNormal),D3DCOLOR_ARGB(255,0,0,0),D3DCOLOR_ARGB(255,0,255,0));

	vA = vTop[2] -vTop[0];
	vB = vTop[1] -vTop[0];
	D3DXVec3Cross(&vNormal,&vA,&vB);
	D3DXVec3Normalize(&vNormal,&vNormal);
	vCenter = vTop[3];
	//	pD3D->AppendElementLine3D(&vCenter,&D3DXVECTOR3(vCenter+vNormal),D3DCOLOR_ARGB(255,0,0,0),D3DCOLOR_ARGB(255,0,255,0));

	vA = vBottom[2] -vBottom[0];
	vB = vBottom[1] -vBottom[0];
	D3DXVec3Cross(&vNormal,&vA,&vB);
	D3DXVec3Normalize(&vNormal,&vNormal);
	vCenter = vBottom[0];
	//	pD3D->AppendElementLine3D(&vCenter,&D3DXVECTOR3(vCenter+vNormal),D3DCOLOR_ARGB(255,0,0,0),D3DCOLOR_ARGB(255,0,255,0));
	*/
}
#define PICK_SELECT_RECT_SIZE 8

void SelectFrustum::CalculateFrustum(const RECT *pRect,render::Camera *pCamera,const POINT &ptStart,const POINT &ptEnd)
{
	//get pick position 
	int iX = pRect->left,iY = pRect->top,iW = pRect->right - pRect->left + 1 ,iH = pRect->bottom - pRect->top + 1;

	D3DXVECTOR3 v[4];

	int iSX = (ptStart.x < ptEnd.x)? ptStart.x : ptEnd.x;
	int iSY = (ptStart.y < ptEnd.y)? ptStart.y : ptEnd.y;

	int iSW = abs(ptStart.x - ptEnd.x);
	int iSH = abs(ptStart.y - ptEnd.y);

	iSX -= iX;
	iSY -= iY;

	//
	D3DXMATRIX matInvView;
	const D3DXMATRIX *pMatProj = pCamera->GetProjectionMatrix();
	D3DXMatrixInverse(&matInvView,NULL,pCamera->GetViewMatrix());

	POINT pt0 = { iSX,iSY};
	POINT pt1 = { iSX+iSW,iSY+iSH};

	if (iSW < PICK_SELECT_RECT_SIZE && iSH < PICK_SELECT_RECT_SIZE)//IT'S SIGLE PICK
	{
		pt0.x -= PICK_SELECT_RECT_SIZE/2;		
		pt0.y -= PICK_SELECT_RECT_SIZE/2;
	}

	pt1.x = pt0.x+ ((iSW < PICK_SELECT_RECT_SIZE)? PICK_SELECT_RECT_SIZE : iSW);
	pt1.y = pt0.y+ ((iSH < PICK_SELECT_RECT_SIZE)? PICK_SELECT_RECT_SIZE : iSH);

	v[0].x =  ( ( ( 2.0f * pt0.x ) / iW) - 1 ) / pMatProj->_11;
	v[0].y = -( ( ( 2.0f * pt0.y ) / iH) - 1 ) / pMatProj->_22;
	v[0].z =  1.0f; //near

	v[1].x =  ( ( ( 2.0f * pt0.x ) / iW) - 1 ) / pMatProj->_11;
	v[1].y = -( ( ( 2.0f * pt1.y ) / iH) - 1 ) / pMatProj->_22;
	v[1].z =  1.0f;

	v[2].x =  ( ( ( 2.0f * pt1.x ) / iW) - 1 ) / pMatProj->_11;
	v[2].y = -( ( ( 2.0f * pt1.y ) / iH) - 1 ) / pMatProj->_22;
	v[2].z =  1.0f;

	v[3].x =  ( ( ( 2.0f * pt1.x ) / iW) - 1 ) / pMatProj->_11;
	v[3].y = -( ( ( 2.0f * pt0.y ) / iH) - 1 ) / pMatProj->_22;
	v[3].z =  1.0f;

	// Transform the screen space pick ray into 3D space

	D3DXVECTOR3 vPickDirection[4],vPickOrigination[4];
	for(int iIndex = 0; iIndex < 4; iIndex++)
	{
		vPickDirection[iIndex].x  = v[iIndex].x*matInvView._11 + v[iIndex].y*matInvView._21 + v[iIndex].z*matInvView._31;
		vPickDirection[iIndex].y  = v[iIndex].x*matInvView._12 + v[iIndex].y*matInvView._22 + v[iIndex].z*matInvView._32;
		vPickDirection[iIndex].z  = v[iIndex].x*matInvView._13 + v[iIndex].y*matInvView._23 + v[iIndex].z*matInvView._33;

		vPickOrigination[iIndex].x = matInvView._41;
		vPickOrigination[iIndex].y = matInvView._42;
		vPickOrigination[iIndex].z = matInvView._43;
	}

	float fNear = pCamera->GetNearPlane();
	float fFar  = pCamera->GetFarPlane();

	D3DXVECTOR3 vB[8];
	vB[0] = vPickOrigination[0] + vPickDirection[0] * fNear;
	vB[1] = vPickOrigination[3] + vPickDirection[3] * fNear; 
	vB[2] = vPickOrigination[2] + vPickDirection[2] * fNear; 
	vB[3] = vPickOrigination[1] + vPickDirection[1] * fNear; 

	vB[4] = vPickOrigination[0] + vPickDirection[0] * fFar;
	vB[5] = vPickOrigination[3] + vPickDirection[3] * fFar; 
	vB[6] = vPickOrigination[2] + vPickDirection[2] * fFar; 
	vB[7] = vPickOrigination[1] + vPickDirection[1] * fFar; 

	SetFrustum(vB);
}

BOOL SelectFrustum::TestBoundingBox(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox)
{
	static const int iTrianglesIndices[ 3 * 12] = { 
		0,2,1, 0,3,2, 4,6,5, 4,7,6,
			1,5,6, 1,6,2, 0,4,7, 0,7,3,
			0,4,5, 0,5,1, 3,7,6, 3,6,3};

	//测试相交
	D3DXVECTOR3 vPos[3];
	const D3DXVECTOR3 *pConner = pBoundingBox->GetCornerVertex();

	int n = 0; 
	for(int iTriangle = 0; iTriangle < 12; iTriangle++)
	{
		D3DXVec3TransformCoord(&vPos[0],&pConner[iTrianglesIndices[n++]],pMatWorld);
		D3DXVec3TransformCoord(&vPos[1],&pConner[iTrianglesIndices[n++]],pMatWorld);
		D3DXVec3TransformCoord(&vPos[2],&pConner[iTrianglesIndices[n++]],pMatWorld);
		if (TestPolygon(vPos,3))
			return TRUE;
	}

	return FALSE;
}
