#pragma once
#include <d3d9.h>
#include <d3dx9.h>

BOOL  FM_IntersectedPlane(const D3DXVECTOR3 vPoly[], const D3DXVECTOR3 vLine[], D3DXVECTOR3 &vNormal, float &originDistance);
float FM_PlaneDistance(const D3DXVECTOR3 &Normal, const D3DXVECTOR3 &Point);
void  FM_IntersectionPoint(const D3DXVECTOR3 &vNormal, const D3DXVECTOR3 vLine[], float distance,D3DXVECTOR3 &vIntersection);
BOOL  FM_InsidePolygon(D3DXVECTOR3 &vIntersection, const D3DXVECTOR3 Poly[]);
float FM_AngleBetweenVectors(D3DXVECTOR3 &Vector1, D3DXVECTOR3 &Vector2);
float FM_Magnitude(const D3DXVECTOR3 &vNormal);

BOOL  FM_LineSegmentIntesectTriangle(const D3DXVECTOR3 vLineSegment[2],const D3DXVECTOR3 vTriangle[3],D3DXVECTOR3 &vIntersection);
BOOL  FM_RadialIntersectTriangle( const D3DXVECTOR3& vOrigin,const D3DXVECTOR3& vDirection,const D3DXVECTOR3 vTriangle[3],D3DXVECTOR3 &vIntersection,float & fDistance);
BOOL  FM_RadialIntersectedPlane(const D3DXVECTOR3 &vOrigin,const D3DXVECTOR3 &vDirection,D3DXVECTOR3 vTriangle[3],D3DXVECTOR3 &vIntersection);
BOOL  FM_RadialIntersectedPlane(const D3DXVECTOR3 &vOrigin,const D3DXVECTOR3 &vDirection,const D3DXPLANE &Plane,D3DXVECTOR3 &vIntersection);

bool  FM_SpherePolygonCollision(D3DXVECTOR3 vPolygon[],D3DXVECTOR3 &vCenter,float radius,D3DXVECTOR3 &vIntersection);
BOOL  FM_ClassifySphere(D3DXVECTOR3 &vCenter,D3DXVECTOR3 &vNormal, D3DXVECTOR3 &vPoint, float radius, float &distance);
bool  FM_EdgeSphereCollision(D3DXVECTOR3 &vCenter,D3DXVECTOR3 vPolygon[], int vertexCount, float radius);
void  FM_ClosestPointOnLine(const D3DXVECTOR3 &vA, const D3DXVECTOR3 &vB, const D3DXVECTOR3 &vPoint,D3DXVECTOR3 &vClosestPoint);
float FM_Distance(const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2);

D3DXVECTOR3 *FM_Vec3TransformCoordArray(D3DXVECTOR3 *pVectArrayOutput,const D3DXVECTOR3 *pVectArrayInput,const D3DXMATRIX *pMatrix,DWORD dwCount);
