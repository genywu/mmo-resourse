
#include "stdafx.h"
#include "utility.h"
#include "console.h"
#include "render.h"
#include "math.h"
#include "jpeg\ijl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define STRINGFIX(X) #X

static char strInfo[1024] = "";


//__CLASS__(render::Quad)___
void render::QuadXZ::Get(D3DXVECTOR3 & vMin, D3DXVECTOR3 & vMax)
{
	vMin = m_vMin;
	vMax = m_vMax;
}
render::QuadXZ::QuadXZ(void)
{
}

render::QuadXZ::~QuadXZ(void)
{
}

void render::QuadXZ::Set(float fxMin , float fxMax , float fzMin , float fzMax)
{
	m_vMin.x = fxMin;
	m_vMin.y = 0.0f;
	m_vMin.z = fzMin;

	m_vMax.x = fxMax;
	m_vMax.y = 0.0f;
	m_vMax.z = fzMax;

	//3------2
	//|	     |  z
	//|	     |  |
	//0------1  +--x
	m_vConner[0] = D3DXVECTOR3(m_vMin.x,0.0f,m_vMin.z);
	m_vConner[1] = D3DXVECTOR3(m_vMax.x,0.0f,m_vMin.z);
	m_vConner[2] = D3DXVECTOR3(m_vMax.x,0.0f,m_vMax.z);
	m_vConner[3] = D3DXVECTOR3(m_vMin.x,0.0f,m_vMax.z);
}
//__CLASS__(render::BoundingBox)___

static const int iTrianglesIndices[ 3 * 12] = { 
	0,2,1, 0,3,2, 4,6,5, 4,7,6,
	1,5,6, 1,6,2, 0,4,7, 0,7,3,
	0,4,5, 0,5,1, 3,7,6, 3,6,3};

	static const int iEdgeIndices[2*12] = {0,1,
		1,2,
		2,3,
		3,0,
		0,4,1,5,2,6,3,7,
		4,5,5,6,6,7,7,4};

	void render::BoundingBox::operator = (const BoundingBox &cBoundingBox)
	{

		memcpy(&m_vCenter,&cBoundingBox.m_vCenter,sizeof(D3DXVECTOR3));
		m_fOutsideRadius = cBoundingBox.m_fOutsideRadius;
		m_vMin.x = cBoundingBox.m_vMin.x;
		m_vMin.y = cBoundingBox.m_vMin.y;
		m_vMin.z = cBoundingBox.m_vMin.z;
		m_vMax.x = cBoundingBox.m_vMax.x;
		m_vMax.y = cBoundingBox.m_vMax.y;
		m_vMax.z = cBoundingBox.m_vMax.z;
		memcpy(m_vConner,cBoundingBox.m_vConner,sizeof(D3DXVECTOR3) * 8);
		memcpy(m_plane,cBoundingBox.m_plane,sizeof(D3DXPLANE) * 6);
	}
	void render::BoundingBox::Set(const D3DXVECTOR3 &vCenter,float fRadius)
	{
		m_vMin.x = vCenter.x - fRadius;
		m_vMin.y = vCenter.y - fRadius;
		m_vMin.z = vCenter.z - fRadius;

		m_vMax.x = vCenter.x + fRadius;
		m_vMax.y = vCenter.y + fRadius;
		m_vMax.z = vCenter.z + fRadius;

		float xmin=m_vMin.x,ymin=m_vMin.y,zmin=m_vMin.z;
		float xmax=m_vMax.x,ymax=m_vMax.y,zmax=m_vMax.z;

		//          7__________6
		//         /		  /|
		//        /			 / |
		//       /			/  |
		//		4----------5   |  
		//		|		   |   |
		//		|	3	   | 2 /	
		//		|		   |  /	
		//		|0		  1| /	
		//		------------

		m_vConner[0]=D3DXVECTOR3(xmin,ymin,zmin);
		m_vConner[1]=D3DXVECTOR3(xmax,ymin,zmin);
		m_vConner[2]=D3DXVECTOR3(xmax,ymin,zmax);
		m_vConner[3]=D3DXVECTOR3(xmin,ymin,zmax);
		m_vConner[4]=D3DXVECTOR3(xmin,ymax,zmin);
		m_vConner[5]=D3DXVECTOR3(xmax,ymax,zmin);
		m_vConner[6]=D3DXVECTOR3(xmax,ymax,zmax);
		m_vConner[7]=D3DXVECTOR3(xmin,ymax,zmax);

		//build plane
		D3DXPlaneFromPoints( &m_plane[0], &m_vConner[0],&m_vConner[2], &m_vConner[1] ); 
		D3DXPlaneFromPoints( &m_plane[1], &m_vConner[0],&m_vConner[5], &m_vConner[4] ); 
		D3DXPlaneFromPoints( &m_plane[2], &m_vConner[1],&m_vConner[6], &m_vConner[5] ); 
		D3DXPlaneFromPoints( &m_plane[3], &m_vConner[6],&m_vConner[3], &m_vConner[7] ); 
		D3DXPlaneFromPoints( &m_plane[4], &m_vConner[7],&m_vConner[0], &m_vConner[4] ); 
		D3DXPlaneFromPoints( &m_plane[5], &m_vConner[4],&m_vConner[6], &m_vConner[7] ); 

		//外包围球半径
		D3DXVECTOR3 vDistance = (m_vMax - m_vMin)/2;
		m_fOutsideRadius = sqrtf(vDistance.x * vDistance.x + vDistance.y * vDistance.y + vDistance.z * vDistance.z);
		//中心点
		m_vCenter = (m_vMin + m_vMax)/2;
	}

	void render::BoundingBox::Set(const D3DXVECTOR3 & vMin, const D3DXVECTOR3 & vMax)
	{
		m_vMin = vMin;
		m_vMax = vMax;
		float xmin=vMin.x,ymin=vMin.y,zmin=vMin.z;
		float xmax=vMax.x,ymax=vMax.y,zmax=vMax.z;

		//          7__________6
		//         /		  /|
		//        /			 / |
		//       /			/  |
		//		4----------5   |  
		//		|		   |   |
		//		|	3	   | 2 /	
		//		|		   |  /	
		//		|0		  1| /	
		//		------------

		m_vConner[0]=D3DXVECTOR3(xmin,ymin,zmin);
		m_vConner[1]=D3DXVECTOR3(xmax,ymin,zmin);
		m_vConner[2]=D3DXVECTOR3(xmax,ymin,zmax);
		m_vConner[3]=D3DXVECTOR3(xmin,ymin,zmax);
		m_vConner[4]=D3DXVECTOR3(xmin,ymax,zmin);
		m_vConner[5]=D3DXVECTOR3(xmax,ymax,zmin);
		m_vConner[6]=D3DXVECTOR3(xmax,ymax,zmax);
		m_vConner[7]=D3DXVECTOR3(xmin,ymax,zmax);

		//build plane
		D3DXPlaneFromPoints( &m_plane[0], &m_vConner[0],&m_vConner[2], &m_vConner[1] ); 
		D3DXPlaneFromPoints( &m_plane[1], &m_vConner[0],&m_vConner[5], &m_vConner[4] ); 
		D3DXPlaneFromPoints( &m_plane[2], &m_vConner[1],&m_vConner[6], &m_vConner[5] ); 
		D3DXPlaneFromPoints( &m_plane[3], &m_vConner[6],&m_vConner[3], &m_vConner[7] ); 
		D3DXPlaneFromPoints( &m_plane[4], &m_vConner[7],&m_vConner[0], &m_vConner[4] ); 
		D3DXPlaneFromPoints( &m_plane[5], &m_vConner[4],&m_vConner[6], &m_vConner[7] ); 

		//外包围球半径
		D3DXVECTOR3 vDistance = (m_vMax - m_vMin)/2;
		m_fOutsideRadius = sqrtf(vDistance.x * vDistance.x + vDistance.y * vDistance.y + vDistance.z * vDistance.z);
		//中心点
		m_vCenter = (m_vMin + m_vMax)/2;

	}

	BOOL render::BoundingBox::TestPointInside(D3DXVECTOR3 * pvPoint)
	{
		float x = pvPoint->x,y = pvPoint->y , z = pvPoint->z;
		for(int i = 0; i < 6; i++ )
		{
			if(m_plane[i].a * x + m_plane[i].b * y + m_plane[i].c * z + m_plane[i].d < 0)
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	void render::BoundingBox::_DEBUG_Render(const D3DXMATRIX *pMatWorld,DWORD dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D   *pLayer3D  = pInterface->GetLayer3D();

		static D3DXVECTOR3 vPos[8];
		for(DWORD n = 0; n < 8; n++)
		{
			D3DXVec3TransformCoord(&vPos[n],&m_vConner[n],pMatWorld);
		}

		for(DWORD dwLine = 0;dwLine < 12;dwLine++)
		{
			pLayer3D->_DEBUG_DrawLine(&vPos[iEdgeIndices[dwLine*2]],&vPos[iEdgeIndices[dwLine*2+1]],dwColor);	
		}	
	}

	void render::BoundingBox::Get(D3DXVECTOR3 & vMin, D3DXVECTOR3 & vMax)
	{
		vMin = m_vMin;
		vMax = m_vMax;
	}

	BOOL render::BoundingBox::TestLineSegmentIntersected(D3DXVECTOR3 vLineSegment[2],D3DXVECTOR3 &vIntersection)
	{
		//测试相交
		D3DXVECTOR3 vTriangle[3];
		//先测试三角形3边是否与与12个三角形相交
		for(int iTriangle = 0; iTriangle < 12; iTriangle++)
		{
			vTriangle[0] = m_vConner[iTrianglesIndices[iTriangle * 3]];
			vTriangle[1] = m_vConner[iTrianglesIndices[iTriangle * 3 + 1]];
			vTriangle[2] = m_vConner[iTrianglesIndices[iTriangle * 3 + 2]];

			if (FM_LineSegmentIntesectTriangle(vLineSegment,vTriangle,vIntersection))
				return TRUE;
		}

		return FALSE;
	}

	BOOL render::BoundingBox::TestRadialIntersected(D3DXVECTOR3 &vOrigin,D3DXVECTOR3 &vDirection)
	{
		D3DXVECTOR3 vIntersection;
		float fDistance;
		//测试相交
		D3DXVECTOR3 vTriangle[3];
		//先测试三角形3边是否与与12个三角形相交
		for(int iTriangle = 0; iTriangle < 12; iTriangle++)
		{
			vTriangle[0] = m_vConner[iTrianglesIndices[iTriangle * 3]];
			vTriangle[1] = m_vConner[iTrianglesIndices[iTriangle * 3 + 1]];
			vTriangle[2] = m_vConner[iTrianglesIndices[iTriangle * 3 + 2]];

			if (FM_RadialIntersectTriangle(vOrigin,vDirection,vTriangle,vIntersection,fDistance))
				return TRUE;
		}

		return FALSE;
	}

	BOOL render::BoundingBox::TestRadialIntersected(const D3DXVECTOR3 *pOrigin,const D3DXVECTOR3 *pDirection,float &fDistance)
	{
		D3DXVECTOR3 vIntersection;
		//测试相交
		D3DXVECTOR3 vTriangle[3];
		//先测试三角形3边是否与与12个三角形相交
		for(int iTriangle = 0; iTriangle < 12; iTriangle++)
		{
			vTriangle[0] = m_vConner[iTrianglesIndices[iTriangle * 3]];
			vTriangle[1] = m_vConner[iTrianglesIndices[iTriangle * 3 + 1]];
			vTriangle[2] = m_vConner[iTrianglesIndices[iTriangle * 3 + 2]];

			if (FM_RadialIntersectTriangle(*pOrigin,*pDirection,vTriangle,vIntersection,fDistance))
				return TRUE;
		}

		return FALSE;
	}

	BOOL render::BoundingBox::TestRadialIntersected(const D3DXMATRIX *pWorldMatrix,const D3DXVECTOR3 *pOrigin,const D3DXVECTOR3 *pDirection,float &fDistance)
	{
		D3DXVECTOR3 vIntersection;
		//测试相交
		D3DXVECTOR3 vTriangle[3];
		//先测试三角形3边是否与与12个三角形相交
		for(int iTriangle = 0; iTriangle < 12; iTriangle++)
		{
			vTriangle[0] = m_vConner[iTrianglesIndices[iTriangle * 3]];
			vTriangle[1] = m_vConner[iTrianglesIndices[iTriangle * 3 + 1]];
			vTriangle[2] = m_vConner[iTrianglesIndices[iTriangle * 3 + 2]];
			D3DXVec3TransformCoord(&vTriangle[0],&vTriangle[0],pWorldMatrix);
			D3DXVec3TransformCoord(&vTriangle[1],&vTriangle[1],pWorldMatrix);
			D3DXVec3TransformCoord(&vTriangle[2],&vTriangle[2],pWorldMatrix);

			if (FM_RadialIntersectTriangle(*pOrigin,*pDirection,vTriangle,vIntersection,fDistance))
				return TRUE;
		}

		return FALSE;
	}


	BOOL render::BoundingBox::TestTriangleIntersected(D3DXVECTOR3 vTriangle[3])
	{
		D3DXVECTOR3 vIntersection;
		//测试box 12条边是否与三角形相交，解决面大于bounding box情况
		D3DXVECTOR3 vEdge[2];
		for(int iEdge = 0; iEdge < 12; iEdge++)
		{
			vEdge[0] = m_vConner[iEdgeIndices[iEdge * 2]];
			vEdge[1] = m_vConner[iEdgeIndices[iEdge * 2 + 1]];

			if (FM_LineSegmentIntesectTriangle(vEdge,vTriangle,vIntersection))
				return TRUE;
		}
		//更精确则需要测试三角形三边与BoundingBox相交，解决正好没有穿过boundingbox各个边的情况
		vEdge[0] = vTriangle[2];
		vEdge[1] = vTriangle[0];

		if (TestLineSegmentIntersected(&vTriangle[0],vIntersection))
			return TRUE;
		if (TestLineSegmentIntersected(&vTriangle[1],vIntersection)) 
			return TRUE;
		if (TestLineSegmentIntersected(&vEdge[0],vIntersection))
			return TRUE;

		return FALSE;
	}

	BOOL render::BoundingBox::TestSphereIntersected(D3DXVECTOR3 &vCenter,float fRadius)
	{
		//判断距离

		D3DXVECTOR3 vTemp = m_vCenter - vCenter;

		float fDistance = sqrtf(vTemp.x * vTemp.x + vTemp.y * vTemp.y + vTemp.z * vTemp.z);

		if (fDistance <= fRadius + m_fOutsideRadius)
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL render::BoundingBox::TestBoundingBoxIntersected(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox)
	{
		//判断距离         
		static D3DXVECTOR3 v[8],vt[3],vi,vl[2],vc;
		static float fd(0.0f);

		D3DXVec3TransformCoord(&vc,pBoundingBox->GetCenter(),pMatWorld); 
		////判断距离
		//D3DXVECTOR3 vdis = (m_vCenter - vc);
		//fd = sqrtf(vdis.x * vdis.x + vdis.y * vdis.y + vdis.z * vdis.z);
		//if (fd > m_fOutsideRadius + pBoundingBox->GetOutsideRadius())
		//{
		//	return FALSE;
		//}

		const D3DXVECTOR3 *pPosBuffer = pBoundingBox->GetCornerVertex();
		//PointInside
		int i = 0;
		for(; i < 8; i++)
		{
			D3DXVec3TransformCoord(&v[i],&pPosBuffer[i],pMatWorld);
			if (TestPointInside(&v[i]))
			{
				return TRUE;
			}
		}


		//相互四条边的交接
		//Self -> Input
		for(i = 0; i < 12; i++)
		{
			//先测试自己bbox3边是否与与12个三角形相交
			for(int iTriangle = 0; iTriangle < 12; iTriangle++)
			{ 
				vt[0] = v[iTrianglesIndices[iTriangle * 3]];
				vt[1] = v[iTrianglesIndices[iTriangle * 3 + 1]];
				vt[2] = v[iTrianglesIndices[iTriangle * 3 + 2]];
				vl[0] = m_vConner[iEdgeIndices[i*2]];
				vl[1] = m_vConner[iEdgeIndices[i*2 + 1]];
				if (FM_LineSegmentIntesectTriangle(vl,vt,vi))
					return TRUE;
			}
		}
		//Input -> Self
		for(i = 0; i < 12; i++)
		{
			//先测试三角形3边是否与与12个三角形相交
			for(int iTriangle = 0; iTriangle < 12; iTriangle++)
			{ 
				vt[0] = m_vConner[iTrianglesIndices[iTriangle * 3]];
				vt[1] = m_vConner[iTrianglesIndices[iTriangle * 3 + 1]];
				vt[2] = m_vConner[iTrianglesIndices[iTriangle * 3 + 2]];
				vl[0] = v[iEdgeIndices[i*2]];
				vl[1] = v[iEdgeIndices[i*2 + 1]];
				if (FM_LineSegmentIntesectTriangle(vl,vt,vi))
					return TRUE;
			}
		}

		return FALSE;
	}
	//快，但是不精确
	BOOL render::BoundingBox::TestBoundingBoxIntersectedFast(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox)
	{
		//判断距离         
		static D3DXVECTOR3 v[8],vt[3],vi,vl[2],vc;
		static float fd(0.0f);

		D3DXVec3TransformCoord(&vc,pBoundingBox->GetCenter(),pMatWorld); 

		const D3DXVECTOR3 *pPosBuffer = pBoundingBox->GetCornerVertex();
		//PointInside
		for(int i = 0; i < 8; i++)
		{
			D3DXVec3TransformCoord(&v[i],&pPosBuffer[i],pMatWorld);
			if (TestPointInside(&v[i]))
			{
				return TRUE;
			}
		}
		return FALSE;
	}


	//__CLASS__(render::Frustum)___
	void render::Frustum::CalculateFrustum(const D3DXMATRIX *pView,const D3DXMATRIX *pProjection)
	{    
		D3DXMATRIX mat;

		D3DXMatrixMultiply( &mat, pView, pProjection);
		D3DXMatrixInverse( &mat, NULL, &mat );

		m_vFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
		m_vFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
		m_vFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
		m_vFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
		m_vFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
		m_vFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
		m_vFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
		m_vFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

		for( int i = 0; i < 8; i++ )
		{
			D3DXVec3TransformCoord( &m_vFrustum[i], &m_vFrustum[i], &mat );
		}

		D3DXPlaneFromPoints( &m_planeFrustum[0], &m_vFrustum[0],&m_vFrustum[1], &m_vFrustum[2] ); // Near
		D3DXPlaneFromPoints( &m_planeFrustum[1], &m_vFrustum[6],&m_vFrustum[7], &m_vFrustum[5] ); // Far
		D3DXPlaneFromPoints( &m_planeFrustum[2], &m_vFrustum[2],&m_vFrustum[6], &m_vFrustum[4] ); // Left
		D3DXPlaneFromPoints( &m_planeFrustum[3], &m_vFrustum[7],&m_vFrustum[3], &m_vFrustum[5] ); // Right
		D3DXPlaneFromPoints( &m_planeFrustum[4], &m_vFrustum[2],&m_vFrustum[3], &m_vFrustum[6] ); // Top
		D3DXPlaneFromPoints( &m_planeFrustum[5], &m_vFrustum[1],&m_vFrustum[0], &m_vFrustum[4] ); // Bottom
	}

	BOOL render::Frustum::BoxIntersectFrustum(const D3DXMATRIX *pWorldMatrix,const render::BoundingBox * pBoundingBox)
	{
		if(NULL == pBoundingBox)	return FALSE;

		static D3DXVECTOR3 vConner[8];
		const D3DXVECTOR3 *pConnver = pBoundingBox->GetCornerVertex();
		for(int v = 0; v < 8; v++)
		{
			D3DXVec3TransformCoord(&vConner[v],&pConnver[v],pWorldMatrix);
		}

		float a,b,c,d;
		for(int i = 0; i < 6; i++ )
		{
			a = m_planeFrustum[i].a;b = m_planeFrustum[i].b;c = m_planeFrustum[i].c;d = m_planeFrustum[i].d;

			if(a * vConner[0].x + b * vConner[0].y + c * vConner[0].z + d > 0)
				continue;
			if(a * vConner[1].x + b * vConner[1].y + c * vConner[1].z + d > 0)
				continue;
			if(a * vConner[2].x + b * vConner[2].y + c * vConner[2].z + d > 0)
				continue;
			if(a * vConner[3].x + b * vConner[3].y + c * vConner[3].z + d > 0)
				continue;
			if(a * vConner[4].x + b * vConner[4].y + c * vConner[4].z + d > 0)
				continue;
			if(a * vConner[5].x + b * vConner[5].y + c * vConner[5].z + d > 0)
				continue;
			if(a * vConner[6].x + b * vConner[6].y + c * vConner[6].z + d > 0)
				continue;
			if(a * vConner[7].x + b * vConner[7].y + c * vConner[7].z + d > 0)
				continue;
			// If we get here, it isn't in the frustum
			return false;
		}

		return true;
	}

	BOOL render::Frustum::BoxIntersectFrustum(const render::BoundingBox * pBoundingBox)
	{
		const D3DXVECTOR3 *pvConner = pBoundingBox->GetCornerVertex();

		float a,b,c,d;
		for(int i = 0; i < 6; i++ )
		{
			a = m_planeFrustum[i].a;b = m_planeFrustum[i].b;c = m_planeFrustum[i].c;d = m_planeFrustum[i].d;

			if(a * pvConner[0].x + b * pvConner[0].y + c * pvConner[0].z + d > 0)
				continue;
			if(a * pvConner[1].x + b * pvConner[1].y + c * pvConner[1].z + d > 0)
				continue;
			if(a * pvConner[2].x + b * pvConner[2].y + c * pvConner[2].z + d > 0)
				continue;
			if(a * pvConner[3].x + b * pvConner[3].y + c * pvConner[3].z + d > 0)
				continue;
			if(a * pvConner[4].x + b * pvConner[4].y + c * pvConner[4].z + d > 0)
				continue;
			if(a * pvConner[5].x + b * pvConner[5].y + c * pvConner[5].z + d > 0)
				continue;
			if(a * pvConner[6].x + b * pvConner[6].y + c * pvConner[6].z + d > 0)
				continue;
			if(a * pvConner[7].x + b * pvConner[7].y + c * pvConner[7].z + d > 0)
				continue;
			// If we get here, it isn't in the frustum
			return false;
		}

		return true;
	}


	void render::Frustum::_DEBUG_Render()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D *pLayer3D =pInterface->GetLayer3D();

		static const int iIndices[2*12] = {0,1
			,1,3
			,3,2
			,2,0
			,2,6
			,6,7
			,7,3
			,7,5
			,6,4
			,0,4
			,4,5
			,5,1};

		for(int iLine = 0;iLine < 12;iLine++)
		{
			pLayer3D->_DEBUG_DrawLine(&m_vFrustum[iIndices[iLine*2]],&m_vFrustum[iIndices[iLine*2+1]],D3DCOLOR_RGBA(0,0,255,255));	
		}
	}

	BOOL render::Frustum::BoxInFrustum(const render::BoundingBox * pBoundingBox)
	{
		const D3DXVECTOR3 *pvConner = pBoundingBox->GetCornerVertex();

		float a,b,c,d;
		for(int i = 0; i < 6; i++ )
		{
			a = m_planeFrustum[i].a;b = m_planeFrustum[i].b;c = m_planeFrustum[i].c;d = m_planeFrustum[i].d;

			if(a * pvConner[0].x + b * pvConner[0].y + c * pvConner[0].z + d < 0)
				return false;
			if(a * pvConner[1].x + b * pvConner[1].y + c * pvConner[1].z + d < 0)
				return false;
			if(a * pvConner[2].x + b * pvConner[2].y + c * pvConner[2].z + d < 0)
				return false;
			if(a * pvConner[3].x + b * pvConner[3].y + c * pvConner[3].z + d < 0)
				return false;
			if(a * pvConner[4].x + b * pvConner[4].y + c * pvConner[4].z + d < 0)
				return false;
			if(a * pvConner[5].x + b * pvConner[5].y + c * pvConner[5].z + d < 0)
				return false;
			if(a * pvConner[6].x + b * pvConner[6].y + c * pvConner[6].z + d < 0)
				return false;
			if(a * pvConner[7].x + b * pvConner[7].y + c * pvConner[7].z + d < 0)
				return false;
		}
		return TRUE;
	}

	BOOL render::Frustum::QuadXZIntersectFrustum(render::QuadXZ * pQuadXZ)
	{
		const D3DXVECTOR3 *pvConner = pQuadXZ->GetConner();

		for(int i = 0; i < 6; i++ )
		{
			if(m_planeFrustum[i].a * pvConner[0].x + m_planeFrustum[i].b * pvConner[0].y + m_planeFrustum[i].c * pvConner[0].z + m_planeFrustum[i].d > 0)
				continue;
			if(m_planeFrustum[i].a * pvConner[1].x + m_planeFrustum[i].b * pvConner[1].y + m_planeFrustum[i].c * pvConner[1].z + m_planeFrustum[i].d > 0)
				continue;
			if(m_planeFrustum[i].a * pvConner[2].x + m_planeFrustum[i].b * pvConner[2].y + m_planeFrustum[i].c * pvConner[2].z + m_planeFrustum[i].d > 0)
				continue;
			if(m_planeFrustum[i].a * pvConner[3].x + m_planeFrustum[i].b * pvConner[3].y + m_planeFrustum[i].c * pvConner[3].z + m_planeFrustum[i].d > 0)
				continue;
			// If we get here, it isn't in the frustum
			return false;
		}

		return true;
	}

	BOOL render::Frustum::PointInFrustum(const D3DXVECTOR3 * pPoint)
	{
		float x = pPoint->x,y = pPoint->y , z = pPoint->z;
		for(int i = 0; i < 6; i++ )
		{
			if(m_planeFrustum[i].a * x + m_planeFrustum[i].b * y + m_planeFrustum[i].c * z + m_planeFrustum[i].d < 0)
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	BOOL render::Frustum::QuadXZInFrustum(QuadXZ * pQuadXZ)
	{
		const D3DXVECTOR3 *pos = pQuadXZ->GetConner();

		for(int i = 0; i < 6; i++ )
		{
			if(m_planeFrustum[i].a * pos[0].x +  m_planeFrustum[i].c * pos[0].z + m_planeFrustum[i].d < 0)
				return FALSE;
			if(m_planeFrustum[i].a * pos[1].x +  m_planeFrustum[i].c * pos[1].z + m_planeFrustum[i].d < 0)
				return FALSE;
			if(m_planeFrustum[i].a * pos[2].x +  m_planeFrustum[i].c * pos[2].z + m_planeFrustum[i].d < 0)
				return FALSE;
			if(m_planeFrustum[i].a * pos[3].x +  m_planeFrustum[i].c * pos[3].z + m_planeFrustum[i].d < 0)
				return FALSE;
		}
		return TRUE;
	}

	//__CLASS__(render::Camera)__摄像机__
	render::Camera::Camera()
	{

		m_vecPosition = D3DXVECTOR3(-5.0f,5.0f,-5.0f);
		m_vecView  = D3DXVECTOR3(0.0f,0.0f,0.0f);


		m_fFOVY = D3DXToRadian(90);
		m_fAspect = 1.0;
		m_fNearPlane = 1.0f;
		m_fFarPlane  = 50.0f;//0.6 * 5 = 30 m

		//calc axis
	}

	render::Camera::~Camera()
	{

	}

	const D3DXMATRIX * render::Camera::GetViewMatrix()  
	{
		D3DXMatrixLookAtLH(&m_matView,&m_vecPosition,&m_vecView,&D3DXVECTOR3(0.0f,1.0f,0.0f));
		return &m_matView;
	}

	const D3DXMATRIX * render::Camera::GetProjectionMatrix() 
	{
		D3DXMatrixPerspectiveFovLH(&m_matProjection,m_fFOVY,m_fAspect,m_fNearPlane,m_fFarPlane);
		return &m_matProjection;
	}

	void render::Camera::SetPosition(D3DXVECTOR3 *pvecPosition)
	{
		m_vecPosition = *pvecPosition;
		CalculateAxis();
	}

	void render::Camera::SetView(const D3DXVECTOR3 *pvecView)
	{
		m_vecView = *pvecView;
		CalculateAxis();
	}

	void render::Camera::SetNearClipPlane(float fNear)
	{
		m_fNearPlane = fNear;
	}
	void render::Camera::SetFarClipPlane(float fFar)
	{
		m_fFarPlane = fFar;
	}

	void render::Camera::SetAspect(float fAspect)
	{
		m_fAspect = fAspect;
	}

	void render::Camera::SetFOVY(float fFOVY)
	{
		m_fFOVY = fFOVY;
	}

	void render::Camera::RotatePosition(D3DXVECTOR3 *pVec)
	{	
		D3DXVECTOR3 vecVector = m_vecPosition - m_vecView;

		//get distance
		float d = sqrtf(vecVector.x*vecVector.x + vecVector.y*vecVector.y + vecVector.z*vecVector.z);
		float reta = acosf(vecVector.y/d);

		if ( ( reta < D3DXToRadian(5)  && pVec->x > 0 ) ||( reta > D3DXToRadian(175) && pVec->x < 0))  return;

		CalculateAxis();

		//rot by y
		D3DXMATRIX matRotY;
		D3DXMatrixRotationY(&matRotY,pVec->y);
		//D3DXMatrixRotationAxis(&matRotY,&m_vecYaxis,pVec->y);
		D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotY);

		//rot by x
		D3DXMATRIX matRotX;
		//D3DXMatrixRotationX(&matRotX,pVec->x);
		D3DXMatrixRotationAxis(&matRotX,&m_vecXaxis,pVec->x);
		D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotX);

		m_vecPosition = m_vecView + vecVector;
	}

	void render::Camera::Rotate(D3DXVECTOR3 *pVec)
	{
		D3DXVECTOR3 vecVector = m_vecView - m_vecPosition;

		CalculateAxis();

		//rot by y
		D3DXMATRIX matRotY;
		D3DXMatrixRotationAxis(&matRotY,&m_vecYaxis,pVec->y);
		D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotY);

		//rot by x
		D3DXMATRIX matRotX;
		D3DXMatrixRotationAxis(&matRotX,&m_vecXaxis,pVec->x);
		D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotX);

		m_vecView = m_vecPosition + vecVector;

	}

	void render::Camera::MovePosition(D3DXVECTOR3 *pVec)
	{
		CalculateAxis();

		m_vecPosition += m_vecXaxis * pVec->x;
		m_vecPosition += m_vecYaxis * pVec->y;
		m_vecPosition += m_vecZaxis * pVec->z;

	}

	void render::Camera::MoveView(D3DXVECTOR3 *pVec)
	{
		CalculateAxis();

		//move x
		m_vecView += m_vecXaxis * pVec->x;
		m_vecView += m_vecYaxis * pVec->y;
		m_vecView += m_vecZaxis * pVec->z;
	}


	void render::Camera::CalculateAxis()
	{
		//get right vector
		m_vecZaxis = m_vecView - m_vecPosition;
		D3DXVec3Normalize(&m_vecZaxis,&m_vecZaxis);
		D3DXVec3Cross(&m_vecXaxis,&D3DXVECTOR3(0,1,0),&m_vecZaxis);
		D3DXVec3Normalize(&m_vecXaxis,&m_vecXaxis);
		D3DXVec3Cross(&m_vecYaxis,&m_vecZaxis,&m_vecXaxis);

	}

	const D3DXMATRIX * render::Camera::GetBillboard()
	{
		D3DXMatrixLookAtLH(&m_matView,&m_vecPosition,&m_vecView,&D3DXVECTOR3(0.0f,1.0f,0.0f));
		D3DXMatrixInverse( &m_matBillboard, NULL, &m_matView );
		m_matBillboard._41 = m_matBillboard._42 = m_matBillboard._43 = 0.0f;
		return &m_matBillboard;
	}

	const D3DXMATRIX * render::Camera::GetBillboardY()
	{
		static D3DXVECTOR3 vDir;
		vDir = m_vecView - m_vecPosition;
		if( vDir.x >= 0.0f )
			D3DXMatrixRotationY( &m_matBillboardY, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
		else
			D3DXMatrixRotationY( &m_matBillboardY, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );

		return &m_matBillboardY;
	}

	void render::Camera::FlatMoveFB(float c)
	{
		CalculateAxis();

		m_vecZaxis.y = 0;

		m_vecZaxis *=c;
		//move z
		m_vecPosition += m_vecZaxis ;
		m_vecView += m_vecZaxis ;
	}

	void render::Camera::FlatMoveLR(float c)
	{
		CalculateAxis();

		m_vecXaxis.y = 0;

		m_vecXaxis *=c;
		//move x
		m_vecPosition += m_vecXaxis ;
		m_vecView += m_vecXaxis ;

	}


	//__CLASS__(render::Texture)____纹理___
	render::Texture::Texture(void)
	{
		m_pBitmap = NULL;
		m_pD3DTexture = NULL;
	}

	render::Texture::~Texture(void)
	{
	}

	BOOL render::Texture::Create(LPDIRECT3DDEVICE9 pDevice,const char * pszFileName,BOOL bTextureMode32,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight,DWORD dwTextureProcision)
	{
		ASSERT(pszFileName);
		CRFile* pFile;
		BOOL bResult = FALSE;
		BOOL bdds = FALSE;
		m_d3dFormat = (bTextureMode32) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;

		char szExt[_MAX_EXT];
		_splitpath(pszFileName,NULL,NULL,NULL,szExt);
		strupr(szExt);
		if (strstr(szExt,".TGA"))
			bResult = BitmapReadFileTGA(pszFileName);
		else 
			if (strstr(szExt,".BMP"))
				bResult = BitmapReadFileBMP(pszFileName);
			else
				if (strstr(szExt,".PSD"))
					bResult = BitmapReadFilePSD(pszFileName);
				else
					if (strstr(szExt,".PCX"))
						bResult = BitmapReadFilePCX(pszFileName);
					else
						if (strstr(szExt,".PPM"))
							bResult = BitmapReadFilePPM(pszFileName);
						else
							if (strstr(szExt,".JPG"))
								bResult = BitmapReadFileJPG(pszFileName);
							else
								if (strstr(szExt,".DDS"))
								{
									pFile = rfOpen(pszFileName);
									bResult = TextureReadFileDDS(pFile,pDevice);
									bdds = TRUE;
									rfClose(pFile);
								}
								if (!bResult)
								{
									OutputConsole("error: render::Interface::Texture: read \"%s\" failed.\n",pszFileName);
									MessageBox(NULL,pszFileName,"Texture open faild!",MB_OK);
									ASSERT(0);
									return FALSE;
								}

								//check bitmap width and height
								if (m_dwWidth > dwMaxTextureWidth || m_dwHeight > dwMaxTextureHeight)
								{
									OutputConsole("error: render::Interface::Texture: MAX: (%d,%d),\"%s\" : (%d,%d).\r\n",dwMaxTextureWidth,dwMaxTextureHeight,pszFileName,m_dwWidth,m_dwHeight);
									DestroyBitmap();
									ASSERT(0);
									return FALSE;
								}

								//缩小材质图象
								if(dwTextureProcision>0&&dwTextureProcision<=2)
								{
									int times=1<<dwTextureProcision;
									if(times==4&&m_dwWidth<128||m_dwHeight<128)times=2;
									if(m_dwWidth>=64||m_dwHeight>=64)
									{
										DWORD *tmp=ReduceBitmap(m_pBitmap,m_dwWidth,m_dwHeight,times);
										delete[] m_pBitmap;
										m_pBitmap=tmp;
										m_dwWidth=m_dwWidth/times;
										m_dwHeight=m_dwHeight/times;
										m_dwBitmapSize=m_dwWidth*m_dwHeight;
									}
								}

								if (!bdds)
								{
									if (!CreateD3DTexture(pDevice))
									{
										OutputError("error: render::Interface::Texture: Create d3d texture (\"%s\") failed.\n",pszFileName);
										DestroyBitmap();
										ASSERT(0);
										return FALSE;
									}

									if (!CopyBitmapToTexture())
									{
										OutputError("error: render::Interface::Texture : Copy bitmap to texture failed (\"%s\').\n",pszFileName);
										DestroyBitmap();
										DestroyD3DTexture();
										ASSERT(0);
										return FALSE;
									}
								}

								return TRUE;
	}

	BOOL render::Texture::Create(LPDIRECT3DDEVICE9 pDevice,CRFile* pFile,BOOL bTextureMode32,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight,DWORD dwTextureProcision)
	{
		ASSERT(pFile);

		BOOL bResult = FALSE;
		BOOL bdds = FALSE;
		m_d3dFormat = (bTextureMode32) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;

		char szExt[_MAX_EXT];
		_splitpath(pFile->GetFileName(),NULL,NULL,NULL,szExt);
		strupr(szExt);
		if (strstr(szExt,".TGA"))
			bResult = BitmapReadFileTGA(pFile);
		else 
			if (strstr(szExt,".BMP"))
				bResult = BitmapReadFileBMP(pFile);
			else
				if (strstr(szExt,".PSD"))
					bResult = BitmapReadFilePSD(pFile);
				else
					if (strstr(szExt,".PCX"))
						bResult = BitmapReadFilePCX(pFile);
					else
						if (strstr(szExt,".PPM"))
							bResult = BitmapReadFilePPM(pFile);
						else
							if (strstr(szExt,".JPG"))
								bResult = BitmapReadFileJPG(pFile);
							else
								if (strstr(szExt,".DDS"))
								{
									bResult = TextureReadFileDDS(pFile,pDevice);
									bdds = TRUE;
								}
								if (!bResult)
								{
									OutputConsole("error: render::Interface::Texture: read \"%s\" failed.\n",pFile->GetFileName());
									MessageBox(NULL,pFile->GetFileName(),"Texture open faild!",MB_OK);
									ASSERT(0);
									return FALSE;
								}

								//check bitmap width and height
								if (m_dwWidth > dwMaxTextureWidth || m_dwHeight > dwMaxTextureHeight)
								{
									OutputConsole("error: render::Interface::Texture: MAX: (%d,%d),\"%s\" : (%d,%d).\r\n",dwMaxTextureWidth,dwMaxTextureHeight,pFile->GetFileName(),m_dwWidth,m_dwHeight);
									DestroyBitmap();
									ASSERT(0);
									return FALSE;
								}

								//缩小材质图象
								if(dwTextureProcision>0&&dwTextureProcision<=2)
								{
									int times=1<<dwTextureProcision;
									if(times==4&&m_dwWidth<128||m_dwHeight<128)times=2;
									if(m_dwWidth>=64||m_dwHeight>=64)
									{
										DWORD *tmp=ReduceBitmap(m_pBitmap,m_dwWidth,m_dwHeight,times);
										delete[] m_pBitmap;
										m_pBitmap=tmp;
										m_dwWidth=m_dwWidth/times;
										m_dwHeight=m_dwHeight/times;
										m_dwBitmapSize=m_dwWidth*m_dwHeight;
									}
								}

								if(!bdds)
								{
									if (!CreateD3DTexture(pDevice))
									{
										OutputError("error: render::Interface::Texture: Create d3d texture (\"%s\") failed.\n",pFile->GetFileName());
										DestroyBitmap();
										ASSERT(0);
										return FALSE;
									}

									if (!CopyBitmapToTexture())
									{
										OutputError("error: render::Interface::Texture : Copy bitmap to texture failed (\"%s\').\n",pFile->GetFileName());
										DestroyBitmap();
										DestroyD3DTexture();
										ASSERT(0);
										return FALSE;
									}

								}
								return TRUE;
	}

	BOOL render::Texture::Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD dwColor,BOOL bTextureMode32,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight)
	{

		m_d3dFormat = (bTextureMode32) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;

		//check bitmap width and height
		if (dwWidth > dwMaxTextureWidth || dwHeight > dwMaxTextureHeight)
		{
			OutputConsole("error: render::Interface::Texture : MAX: (%d,%d),Bitmap: (%d,%d).\n",dwMaxTextureWidth,dwMaxTextureHeight,m_dwWidth,m_dwHeight);
			DestroyBitmap();
			ASSERT(0);
			return FALSE;
		}

		if (!CreateBitmap(dwWidth,dwHeight,dwColor))
		{
			OutputConsole("error: render::Interface::Texture : Create bitmap(%d,%d) failed.\n",dwWidth,dwHeight);
			ASSERT(0);
			return FALSE;
		}

		if (!CreateD3DTexture(pDevice))
		{
			OutputConsole("error: render::Interface::Texture : Create D3D texture failed.\n");
			DestroyBitmap();
			ASSERT(0);
			return FALSE;
		}

		if (!CopyBitmapToTexture())
		{
			OutputConsole("error: render::Interface::Texture : Copy bitmap To texture failed.\n");
			DestroyD3DTexture();
			DestroyBitmap();
			ASSERT(0);
			return FALSE;
		}

		return TRUE;
	}

	BOOL render::Texture::Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap,BOOL bTextureMode32,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight)
	{
		m_d3dFormat = (bTextureMode32) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;

		//check bitmap width and height
		if (dwWidth > dwMaxTextureWidth || dwHeight > dwMaxTextureHeight)
		{
			OutputError("error: render::Interface::Texture : MAX: (%d,%d),Bitmap: (%d,%d).\n",dwMaxTextureWidth,dwMaxTextureHeight,m_dwWidth,m_dwHeight);
			DestroyBitmap();
			ASSERT(0);
			return FALSE;
		}

		if (!CreateBitmap(dwWidth,dwHeight,0x00000000))
		{
			OutputError("error: render::Interface::Texture : Create bitmap(%d,%d) failed.\n",dwWidth,dwHeight);
			ASSERT(0);
			return FALSE;
		}

		//copya pBitmap to m_pBitmap
		memcpy(m_pBitmap,pBitmap,sizeof(DWORD) * dwWidth * dwHeight);

		if (!CreateD3DTexture(pDevice))
		{
			OutputError("error: render::Interface::Texture : Create D3D texture failed.\n");
			DestroyBitmap();
			ASSERT(0);
			return FALSE;
		}

		if (!CopyBitmapToTexture())
		{
			OutputError("error: render::Interface::Texture : Copy bitmap To texture failed.\n");
			DestroyD3DTexture();
			DestroyBitmap();
			ASSERT(0);
			return FALSE;
		}

		return TRUE;
	}

	void render::Texture::Destroy()
	{
		DestroyD3DTexture();
		DestroyBitmap();	
	}

	DWORD *render::Texture::ReduceBitmap(DWORD *pSrc,DWORD dwWidth,DWORD dwHeight,DWORD dwTimes)
	{
		DWORD w=dwWidth/dwTimes,h=dwHeight/dwTimes;
		DWORD *pDest=new DWORD[w*h];
		int index=0,index1=0;
		for(DWORD i=0;i<h;i++)
		{
			for(DWORD j=0;j<w;j++,++index)
			{
				index1=j*dwTimes+i*dwTimes*dwWidth;
				pDest[index]=pSrc[index1];
			}
		}
		return pDest;
	}

	BOOL render::Texture::CopyBitmapToTexture()
	{
		HRESULT hResult;
		D3DLOCKED_RECT lr;
		if (FAILED(hResult = m_pD3DTexture->LockRect(0,&lr,NULL,0)))
		{
			OutputError("error: render::Interface::Texture : Lock texture failed.\n");
			ASSERT(0);
			return FALSE;
		}

		switch(m_d3dFormat)
		{
		case D3DFMT_A8R8G8B8:
			{
				//fill image to Texture
				DWORD *pDest = (DWORD*)lr.pBits;
				int  iPitch = lr.Pitch / sizeof(DWORD);
				DWORD *pSrc  = m_pBitmap;

				DWORD dwHeight = m_dwHeight;
				while(dwHeight--)
				{
					memcpy(pDest,pSrc,m_dwWidth* sizeof(DWORD));
					pDest+=iPitch;
					pSrc+=m_dwWidth;
				}	
			}
			break;
		case D3DFMT_A4R4G4B4:
			{
				//fill image to Texture
				WORD *pDest = (WORD*)lr.pBits;
				int  iPitch = lr.Pitch / sizeof(WORD);
				DWORD *pSrc  = m_pBitmap;

				DWORD dwHeight = m_dwHeight;
				while(dwHeight--)
				{
					for(DWORD i = 0; i < m_dwWidth; i++)
					{
						DWORD c = pSrc[i];

						pDest[i] = (WORD)( ((c & 0xf0000000) >> 16) | ((c & 0x00f00000) >> 12) 
							| ((c & 0x0000f000) >> 8) | ((c & 0x000000f0) >> 4));
					}
					pDest+=iPitch;
					pSrc+=m_dwWidth;
				}	
			}
			break;
		}

		m_pD3DTexture->UnlockRect(0);

		return TRUE;
	}

	BOOL render::Texture::CreateBitmap(DWORD dwWidth , DWORD dwHeight,DWORD dwColor)
	{
		//分配空间
		m_dwWidth  = dwWidth;
		m_dwHeight = dwHeight;

		m_dwBitmapSize = dwWidth*dwHeight;//
		m_pBitmap = new DWORD[m_dwBitmapSize];
		if (!m_pBitmap)
		{
			OutputError("error: render::Interface::Texture : CreateBitmap() mem allocated failed.(%d,%d).\n",dwWidth,dwHeight);
			//ASSERT(0);
			return FALSE;
		}

		DWORD *pDest = m_pBitmap;
		DWORD dwSize = m_dwBitmapSize;
		__asm
		{
			mov edi,[pDest]
			mov eax,dwColor
				cld 
				mov ecx,dwSize
				rep stosd
		}

		return TRUE;
	}
	void render::Texture::DestroyBitmap()
	{
		SAFEDELETEARRAY(m_pBitmap);
	}
	BOOL render::Texture::CreateD3DTexture(LPDIRECT3DDEVICE9 pDevice,DWORD dwLevel)
	{
		HRESULT hResult = pDevice->CreateTexture(m_dwWidth,m_dwHeight,dwLevel,0,m_d3dFormat,D3DPOOL_MANAGED, &m_pD3DTexture,NULL);
		if (FAILED(hResult))
		{
			OutputError("error: render::Interface::Texture : CreateD3DTexture(DEVICE) failed.[CODE: %d]\n",hResult);
			ASSERT(0);
			return FALSE;
		}

		return TRUE;
	}
	void render::Texture::DestroyD3DTexture()
	{
		SAFERELEASE(m_pD3DTexture);
	}
	void render::Texture::BitmapFlipVertical(void)
	{
		//恢复图像顺序
		DWORD * pLineTemp = new DWORD [m_dwWidth];
		ASSERT(pLineTemp);

		DWORD * pUp = m_pBitmap;
		DWORD * pBottom = m_pBitmap + (m_dwHeight - 1) * m_dwWidth;

		DWORD dwCopySize = m_dwWidth * sizeof(DWORD);

		for(DWORD i=0;i<m_dwHeight/2;i++)
		{
			//save line to temp
			memcpy(pLineTemp,pUp,dwCopySize);
			//mov line
			memcpy(pUp,pBottom,dwCopySize);
			//restor line
			memcpy(pBottom,pLineTemp,dwCopySize);

			pUp		+= m_dwWidth;
			pBottom -= m_dwWidth;
		}

		SAFEDELETE(pLineTemp);
	}
	BOOL render::Texture::BitmapReadFilePCX(const char * pszFileName)
	{
		// PCX header
		struct _tagPCXFILEHEADER{
			char manufacture;
			char version;
			char encoding;
			char bit_per_pixel;
			short xmin,ymin;
			short xmax,ymax;
			short hres;
			short vres;
			char palette[48];
			char reserved;
			char color_planes;
			short bytes_per_line;
			short palette_type;
			char filler[58];
		}; 

		_tagPCXFILEHEADER stHdr;

		CRFile *pfile = rfOpen(pszFileName);

		if (!pfile) 
		{
			OutputError("render::Interface::Texture : Open \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		pfile->ReadData(&stHdr,sizeof(stHdr) );


		if (!CreateBitmap(stHdr.xmax - stHdr.xmin +1,stHdr.ymax - stHdr.ymin +1,0xff000000))
		{
			OutputError("render::Interface::Texture : CreateBitmap() \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		if (stHdr.bit_per_pixel !=8 )
		{
			OutputError("render::Interface::Texture : Unsupport \"%s\" format.\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		if (stHdr.color_planes == 3)
		{
			//24 color
			pfile->Seek(128,SEEK_SET);
			BYTE *pColorPlaneBuf = new BYTE[m_dwWidth * m_dwHeight * 3];
			ASSERT(pColorPlaneBuf);	 

			DWORD n=0,count;
			unsigned char c;
			do
			{
				VERIFY(pfile->ReadChar(c));// & 0xff;

				if( ( c & 0xc0 ) == 0xc0 )
				{
					count=c & 0x3f;
					pfile->ReadChar(c);				
				}
				else 
					count = 1;

				while(count--) pColorPlaneBuf[n++] = c;

			}while(n<m_dwBitmapSize * 3);

			//888->8888

			BYTE  *pBuf  = pColorPlaneBuf;
			DWORD *pData = m_pBitmap;

			for(DWORD y = 0; y < m_dwHeight;y++)
			{
				for(DWORD dwColorPlane = 0; dwColorPlane < 3; dwColorPlane++)
				{
					DWORD dwShift = (2 - dwColorPlane) << 3;
					for(DWORD x= 0; x < m_dwWidth; x++)
					{
						DWORD dwC = (DWORD) * pBuf++;

						pData[x] |= (dwC << dwShift);
					}
				}
				pData+=m_dwWidth;
			}

			SAFEDELETE(pColorPlaneBuf);

		}
		else
		{
			//READ PALETTE
			pfile->Seek(-768l,SEEK_END);
			BYTE pal[768];
			pfile->ReadData(pal,768);

			pfile->Seek(128,SEEK_SET);
			//decode
			for(DWORD y = 0; y < m_dwHeight; y++)
			{
				int bytes = stHdr.bytes_per_line;


				DWORD *pBuf = m_pBitmap + y * m_dwWidth;
				int n=0,count;
				unsigned char c;

				do
				{
					pfile->ReadChar(c);// & 0xff;

					if((c&0xc0)==0xc0)
					{
						count=c&0x3f;
						pfile->ReadChar(c);				
					}
					else 
						count = 1;

					DWORD dwIndex = (c << 1) + c;

					DWORD color  = pal[dwIndex++] << 16;
					color |= pal[dwIndex++] << 8;
					color |= pal[dwIndex];

					while(count--) pBuf[n++] |=color;

				}while(n<bytes);
			}

		}
		rfClose(pfile);
		return TRUE;
	}
	BOOL render::Texture::BitmapReadFileTGA(const char *pszFileName)
	{
		//位图部分
#pragma pack(1)
		struct _tagTGAFILEHEADER
		{	
			BYTE IdentSize;			
			BYTE ColorMapType;		
			BYTE ImageType;			
			WORD ColorMapStart;		
			WORD ColorMapLength;	
			BYTE ColorMapBits;		
			WORD XStart;			
			WORD YStart;			
			WORD Width;				
			WORD Height;			
			BYTE Bits;				
			BYTE Descriptor;		
		};
#pragma pack()

		struct _tagRGBA {
			BYTE Blue,Green,Red,Alpha;
		}; 

		_tagTGAFILEHEADER stHdr;

		CRFile *pfile = rfOpen(pszFileName);
		if (!pfile) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		pfile->ReadData((BYTE *)&stHdr,sizeof(_tagTGAFILEHEADER));

		if (!CreateBitmap(stHdr.Width,stHdr.Height,0xff000000))
		{
			OutputConsole("error: render::Interface::Texture : CreateBitmap() failed.(\"%s\")\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		DWORD * pBuf = m_pBitmap;

		if (stHdr.ImageType == 2) 
		{
			switch(stHdr.Bits)
			{
			case 16:
				{
					WORD c;
					for(int y=0;y<stHdr.Height;y++)
						for(int x =0;x<stHdr.Width;x++)
						{
							pfile->ReadData((BYTE*)&c,sizeof(WORD));

							DWORD dwB = (c & 0x1f) << 3;
							DWORD dwG = ((c >> 5) & 0x1f) << 3;
							DWORD dwR = ((c >> 10) & 0x1f) << 3;

							*pBuf++ |= (dwR<<16) | (dwG<<8) | dwB;
						}
				}
				break;
			case 24:
				{
					BYTE r,g,b;
					for(int y=0;y<stHdr.Height;y++)
						for(int x=0;x<stHdr.Width;x++)
						{
							pfile->ReadData(&b,sizeof(BYTE));
							pfile->ReadData(&g,sizeof(BYTE));
							pfile->ReadData(&r,sizeof(BYTE));

							*pBuf++ |= (r << 16) | (g << 8) | b;
						}
				}		
				break;
			case 32:
				{
					BYTE r,g,b,a;
					for(int y=0;y<stHdr.Height;y++)
						for(int x=0;x<stHdr.Width;x++)
						{
							pfile->ReadData(&b,sizeof(BYTE));
							pfile->ReadData(&g,sizeof(BYTE));
							pfile->ReadData(&r,sizeof(BYTE));
							pfile->ReadData(&a,sizeof(BYTE));

							*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
						}
				}
				break;
			default:
				{
					OutputConsole("error: render::Interface::Texture : not surport this TGA format yet.\n");
					ASSERT(0);
					rfClose(pfile);
					return FALSE;
				}
			} 
		}
		else
			if (stHdr.ImageType == 10) //RLE 
			{
				switch(stHdr.Bits)
				{
				case 24:
					{
						BYTE byLen,r,g,b;WORD c = 0;int iPixel=0;
						do
						{
							pfile->ReadData(&byLen,sizeof(BYTE));

							if(byLen >= 0x80)
							{
								byLen -= 0x7F;
								pfile->ReadData(&b,sizeof(BYTE));
								pfile->ReadData(&g,sizeof(BYTE));
								pfile->ReadData(&r,sizeof(BYTE));

								while(byLen --)
								{
									*pBuf++ |= (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
							else
							{
								byLen ++;
								while(byLen --)
								{
									pfile->ReadData(&b,sizeof(BYTE));
									pfile->ReadData(&g,sizeof(BYTE));
									pfile->ReadData(&r,sizeof(BYTE));
									*pBuf++ |= (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
						}  while(iPixel < stHdr.Width*stHdr.Height);
					}
					break;
				case 32:
					{	
						BYTE byLen,r,g,b,a;int iPixel=0;
						do
						{
							pfile->ReadData(&byLen,sizeof(BYTE));

							if(byLen >= 0x80)
							{
								byLen -= 0x7F;
								pfile->ReadData(&b,sizeof(BYTE));
								pfile->ReadData(&g,sizeof(BYTE));
								pfile->ReadData(&r,sizeof(BYTE));
								pfile->ReadData(&a,sizeof(BYTE));
								while(byLen --)
								{
									*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
							else
							{
								byLen ++;
								while(byLen --)
								{
									pfile->ReadData(&b,sizeof(BYTE));
									pfile->ReadData(&g,sizeof(BYTE));
									pfile->ReadData(&r,sizeof(BYTE));
									pfile->ReadData(&a,sizeof(BYTE));
									*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
						}  while(iPixel < stHdr.Width*stHdr.Height);
					}
					break;
				case 16:
				default:
					{
						OutputConsole("error: render::Interface::Texture : not surport this TGA format yet.\n");
						ASSERT(0);
						rfClose(pfile);
						return FALSE;
					}
				}
			}
			else
			{
				OutputConsole("error: render::Interface::Texture : Unknown \"%s\" TGA Format (%d).\n",pszFileName,stHdr.ImageType);
				ASSERT(0);
				rfClose(pfile);
				return FALSE;
			}

			BitmapFlipVertical();

			rfClose(pfile);
			return TRUE;
	}
	BOOL render::Texture::BitmapReadFileBMP(const char *pszFileName)
	{
#pragma pack(1)

		struct _tagBMPFILEHEADER{ 
			WORD   bfType; 
			DWORD  bfSize; 
			WORD   bfReserved1; 
			WORD   bfReserved2; 
			DWORD  bfOffBits; 
			DWORD  biSize; 
			LONG   biWidth; 
			LONG   biHeight; 
			WORD   biPlanes; 
			WORD   biBitCount;
			DWORD  biCompression; 
			DWORD  biSizeImage; 
			LONG   biXPelsPerMeter; 
			LONG   biYPelsPerMeter; 
			DWORD  biClrUsed; 
			DWORD  biClrImportant; 
		}; 
#pragma pack()

		CRFile* pfile = rfOpen(pszFileName);
		if (!pfile) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file Failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		_tagBMPFILEHEADER stHdr;
		pfile->ReadData(&stHdr,sizeof(_tagBMPFILEHEADER));

		if (stHdr.bfType != 0x4d42 || stHdr.biCompression != BI_RGB) 
		{
			OutputConsole("error: render::Interface::Texture : \"%s\" format not be support.\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		CreateBitmap(stHdr.biWidth,stHdr.biHeight,0xff000000);

		DWORD * pBuf = m_pBitmap;

		switch(stHdr.biBitCount)
		{
		case 8:
			{
				RGBQUAD *pColorTable = (RGBQUAD *)(pfile->GetData()+pfile->GetPosition());

				pfile->Seek(stHdr.bfOffBits,SEEK_SET);

				for(int y = 0;y < stHdr.biHeight; y++)
				{
					for(int x = 0; x < stHdr.biWidth; x++)
					{
						BYTE byIndex;
						pfile->ReadData(&byIndex,sizeof(BYTE));
						DWORD c  = pColorTable[byIndex].rgbRed << 16;
						c |= pColorTable[byIndex].rgbGreen << 8;
						c |= pColorTable[byIndex].rgbBlue;

						*pBuf++ |= c;
					}
				}
			}
			break;
		case 24:
			{
				pfile->Seek(stHdr.bfOffBits,SEEK_SET);
				BYTE r,g,b;
				for(int y=0;y<stHdr.biHeight;y++)
					for(int x=0;x<stHdr.biWidth;x++)
					{
						pfile->ReadData(&b,sizeof(BYTE));
						pfile->ReadData(&g,sizeof(BYTE));
						pfile->ReadData(&r,sizeof(BYTE));
						*pBuf++ |= ( r << 16) | (g << 8) | b;
					}
			}
			break;
		}

		BitmapFlipVertical();

		rfClose(pfile);

		return TRUE;
	}
	BOOL render::Texture::BitmapReadFilePSD(const char *pszFileName)
	{
#pragma pack(1)
		struct _tagPSDFILEHEADER
		{
			DWORD dwSignature; 
			WORD  wVersion;    
			BYTE  dwReserved[6];
			WORD  wChannels;   
			DWORD dwHeight;
			DWORD dwWidth; 
			WORD  wChannelBit; 
			WORD  wRgbMode;    
		};
#pragma pack()

		_tagPSDFILEHEADER stHdr;

		CRFile* pfile = rfOpen(pszFileName);
		if (!pfile) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file Failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}	

		pfile->ReadData((BYTE *)&stHdr,sizeof(stHdr));

		//change order
		BYTE *pstHdr = (BYTE*)&stHdr.wVersion;

		__asm {
			push edi
				push esi
				push eax

				mov edi,[pstHdr]
			mov esi,edi
				lodsw 
				xchg ah,al
				stosw
				add esi,6
				add edi,6
				lodsw 
				xchg ah,al
				stosw

				lodsd
				bswap eax
				stosd

				lodsd
				bswap eax
				stosd

				lodsw 
				xchg ah,al
				stosw

				lodsw 
				xchg ah,al
				stosw

				pop eax
				pop esi
				pop edi
		}


		if (stHdr.wChannels < 0 || stHdr.wChannels >4 || stHdr.wVersion !=1 || stHdr.wChannelBit !=8 || stHdr.wRgbMode !=3 )
		{
			OutputConsole("error: render::Interface::texture : \"%s\"'s format don't be support.\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		// 跳过数据（如调色板）
		DWORD dwModeDataCount;
		pfile->ReadData((BYTE*)&dwModeDataCount,sizeof(DWORD));	

		__asm 
		{
			mov eax,[dwModeDataCount]
			bswap eax
				mov [dwModeDataCount],eax
		}

		pfile->Seek(dwModeDataCount,SEEK_CUR );

		// 跳过数据（如：pen tool paths, etc）
		DWORD dwResourceDataCount;
		pfile->ReadData((BYTE*)&dwResourceDataCount,sizeof(DWORD));

		__asm
		{
			mov eax,[dwResourceDataCount]
			bswap eax
				mov [dwResourceDataCount],eax
		}
		pfile->Seek(dwResourceDataCount,SEEK_CUR );

		// 跳过保留数据
		DWORD dwReservedDataCount;
		pfile->ReadData((BYTE*)&dwReservedDataCount,sizeof(DWORD));
		_asm 
		{
			mov eax,[dwReservedDataCount]
			bswap eax
				mov [dwReservedDataCount],eax
		}
		pfile->Seek(dwReservedDataCount,SEEK_CUR );

		//数据部分

		//分配内存
		CreateBitmap(stHdr.dwWidth,stHdr.dwHeight);	

		// 0: 非压缩 1: RLE压缩
		WORD wCompressionType;
		pfile->ReadData((BYTE*)&wCompressionType,sizeof(WORD));
		__asm
		{
			mov ax,[wCompressionType]
			xchg ah,al
				mov [wCompressionType],ax
		}
		if (wCompressionType > 1)
		{
			OutputConsole("error: render::Interface::texture : \"%s\" file compression type don't be support.\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		// 解包数据
		DWORD dwPixelCount = (DWORD)(m_dwWidth * m_dwHeight);

		DWORD * pBuf = NULL;

		BOOL  bHasAlphaChannels = (stHdr.wChannels>3);

		switch(wCompressionType)
		{
		case  0 :
			{
				for (DWORD dwCurChannel =0; dwCurChannel < 3; dwCurChannel++ ) 
				{
					pBuf  = m_pBitmap;
					DWORD dwShift = (2 - dwCurChannel) << 3;

					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						BYTE  c;
						pfile->ReadData(&c,sizeof(BYTE));

						*pBuf++ |= c << dwShift;
					}
				}

				pBuf  = m_pBitmap;

				if (bHasAlphaChannels)
				{
					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						BYTE  c;
						pfile->ReadData(&c,sizeof(BYTE));
						*pBuf++ |= c << 24;
					}
				}
				else
				{
					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						*pBuf++ |= 0xff000000;
					}
				}
			}
			break;
		case 1:
			{
				pfile->Seek(m_dwHeight * stHdr.wChannels * 2, SEEK_CUR );
				for (DWORD dwCurChannel =0; dwCurChannel < stHdr.wChannels; dwCurChannel++ ) 
				{
					//read every channel
					DWORD dwPixel = 0;
					pBuf = m_pBitmap;
					DWORD dwShift = (2 - dwCurChannel) << 3;

					while(dwPixel<dwPixelCount)
					{
						BYTE byLen;
						pfile->ReadData(&byLen,sizeof(BYTE));
						if (byLen < 128 ) //not compress
						{
							byLen++;
							dwPixel += byLen;
							while(byLen)
							{
								BYTE c;
								pfile->ReadData(&c,sizeof(BYTE));
								*pBuf++ |= c << dwShift;
								byLen --;
							}

						}
						else //compress..
						{	
							if(byLen != 128)
							{
								byLen ^= 0xff;
								byLen += 2;
								dwPixel += byLen;
								BYTE c;
								pfile->ReadData(&c,sizeof(BYTE));

								DWORD color = c << dwShift;

								while(byLen)
								{
									*pBuf++ |= color;
									byLen--;
								}
							}
						}
					}
				}
				if (!bHasAlphaChannels)
				{
					pBuf = m_pBitmap;
					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						*pBuf++ |= 0xff000000;
					}
				}

			}
			break;
		default:
			rfClose(pfile);
			return FALSE;
			break;
		}
		rfClose(pfile);
		return TRUE;
	}	

	BOOL render::Texture::BitmapReadFilePPM(const char *pszFileName)
	{
		CRFile *pfile = rfOpen(pszFileName);
		if (!pfile) 
		{
			OutputConsole("error: render::Interface::Texture : Read \"%s\" failed.\n");
			ASSERT(0);
			return FALSE;
		}

		// read header

		char szLineBuffer[128];

		pfile->ReadLine(szLineBuffer,128);

		if (szLineBuffer[0] != 'P' || szLineBuffer[1] != '6')
		{
			OutputConsole("error: render::Interface::Texture : Not support \"%s\"'s Format.\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		int iSavePosition;
		do
		{
			iSavePosition = pfile->GetPosition();
			pfile->ReadLine(szLineBuffer,128);
		}	
		while(szLineBuffer[0] == '#');

		pfile->SetPosition(iSavePosition);

		pfile->ReadLine(szLineBuffer,128);

		//读取宽高
		int iWidth,iHeight;

		sscanf(szLineBuffer,"%d %d",&iWidth,&iHeight);

		pfile->ReadLine(szLineBuffer,128);

		CreateBitmap(iWidth,iHeight,0xff000000);


		DWORD * pBuf = m_pBitmap;

		for(int y= 0 ; y < iHeight; y++)
		{
			for(int x = 0; x < iWidth; x++)
			{
				BYTE r,g,b;
				pfile->ReadData(&r,sizeof(BYTE));
				pfile->ReadData(&g,sizeof(BYTE));
				pfile->ReadData(&b,sizeof(BYTE));
				pBuf[x] |= (r << 16) | (g << 8) | b;
			}
			pBuf+=iWidth;
		}

		BitmapFlipVertical();

		rfClose(pfile);

		return TRUE;
	}

	BOOL render::Texture::CreateJpgTextureFromMemory(unsigned char * buf,unsigned long lSize,LPDIRECT3DDEVICE9 pDevice)
	{
		ijlJPEG::JPEG_CORE_PROPERTIES jcprops;
		ijlJPEG::IJLERR ijlErr;
		ijlErr = ijlInit(&jcprops);

		jcprops.JPGBytes = buf;
		jcprops.JPGSizeBytes = lSize;
		/*jcprops.JPGChannels = 4;
		jcprops.JPGSubsampling = IJL_NONE;
		jcprops.JPGColor = IJL_OTHER;*/

		if (ijlRead(&jcprops,ijlJPEG::IJL_JBUFF_READPARAMS) != ijlJPEG::IJL_OK)
		{
			OutputError("ijlRead: jpg memory ERROR\n");
			ASSERT(0);
			return FALSE;
		}


		BYTE * pBuffer = new BYTE[jcprops.JPGWidth * jcprops.JPGHeight * jcprops.JPGChannels];

		jcprops.DIBWidth  = jcprops.JPGWidth;
		jcprops.DIBHeight = jcprops.JPGHeight;
		jcprops.DIBPadBytes = 0;
		jcprops.DIBBytes = pBuffer;

		if (jcprops.JPGChannels !=  3)
		{
			SAFEDELETEARRAY(pBuffer);
			return FALSE;
		}
		//只支持彩色JPG
		jcprops.JPGColor    = ijlJPEG::IJL_YCBCR;
		jcprops.DIBChannels = 3;
		jcprops.DIBColor    = ijlJPEG::IJL_BGR;

		if(ijlRead(&jcprops,ijlJPEG::IJL_JBUFF_READWHOLEIMAGE) != ijlJPEG::IJL_OK)
		{
			ASSERT(0);
			return FALSE;
		}

		if (!CreateBitmap(jcprops.JPGWidth,jcprops.JPGHeight,0xff000000))
		{
			OutputError("render::Interface::Texture : CreateBitmap() \" JPG Memory\" file failed.\n");
			ASSERT(0);
			return FALSE;
		}

		ijlFree(&jcprops);

		//CONVERT PIXEL TO BITMAP
		struct tagRGB
		{
			BYTE b,g,r;
		};

		tagRGB *p = (tagRGB*)pBuffer;
		for(DWORD y = 0; y < m_dwHeight;y++)
		{
			for(DWORD x = 0; x < m_dwWidth; x ++)
			{
				tagRGB &c = p[x + y * m_dwWidth];
				m_pBitmap[x + y * m_dwWidth] = 0xFF000000 | (DWORD)c.r << 16 | (DWORD)c.g << 8 | (DWORD)c.b;
			}	
		}

		SAFEDELETEARRAY(pBuffer);
		return true;
	}

	//add by 彭谦******增加压缩纹理接口
	BOOL render::Texture::TextureReadFileDDS(CRFile* pFile,LPDIRECT3DDEVICE9 pDevice,DWORD dwLevel)
	{
		if (!pFile) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file Failed.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		D3DSURFACE_DESC stHdr;
		IDirect3DSurface9* surface;
		/*D3DXCreateTextureFromFileInMemory(pDevice,pFile->GetData(),pFile->GetDatalen(),&m_pD3DTexture);*/
		D3DXCreateTextureFromFileInMemoryEx(pDevice,pFile->GetData(),pFile->GetDatalen(),D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_FROM_FILE,D3DPOOL_MANAGED,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,0,NULL,NULL,&m_pD3DTexture);
		if (!m_pD3DTexture)
		{
			/*char str[255];
			sprintf(str,"创建贴图%s失败",pFile->GetFileName());
			MessageBox(NULL,str,"错误",MB_OK);*/
			return FALSE;
		}
		m_pD3DTexture->GetSurfaceLevel(0,&surface);
		surface->GetDesc(&stHdr);
		m_dwWidth = stHdr.Width;
		m_dwHeight = stHdr.Height;
		m_dwBitmapSize = m_dwWidth * m_dwHeight;
		m_d3dFormat = stHdr.Format;
		surface->Release();

		return TRUE;
	}
	//add by 彭谦******增加压缩纹理接口

	//返回点颜色
	DWORD render::Texture::GetPixel(int iX, int iY)
	{
		//get pixel
		//if(iX>=m_dwWidth||iY>=m_dwHeight)return 0;
		if (m_pBitmap == NULL)
			return 0;
		else
		        return m_pBitmap[iY * m_dwWidth + iX];	
	}

	void render::Texture::SetPixel(int iX, int iY,DWORD dwColor)
	{
		//set pixel
		//if(iX>=m_dwWidth||iY>=m_dwHeight)return;
		m_pBitmap[iY * m_dwWidth + iX] = dwColor;	
	}


	//__CLASS__(render::Interface::Layer2D::Font)___字体
	render::Interface::Layer2D::Font::Font()
	{
		m_pTexture = NULL;
	}

	render::Interface::Layer2D::Font::~Font()
	{
	}

	BOOL render::Interface::Layer2D::Font::Create(const char *szFileName)
	{
		if (!CreateFontTexture())
		{
			OutputConsole("Create Font Texture Failed!");
			return FALSE;
		}

		if (!CreateVertexBuffer())
		{
			OutputConsole("Create Vertex Buffer Failed!");
			return FALSE;
		}

		CRFile *pfile = rfOpen(szFileName);
		if (!pfile)
		{
			OutputConsole("error: render::Layer2D::Font: Error in Font::Create(\"%s\"",szFileName);
			ASSERT(0);
			return FALSE;
		}

		m_strFileName = szFileName;

		pfile->ReadData(&m_dwFontSize,sizeof(DWORD));
		pfile->ReadData(&m_dwFontInTextureSize,sizeof(DWORD));

		//READ ASC 
		//int iBytes = m_dwFontInTextureSize / 8;
		int iCharDotChunkSize = m_dwFontInTextureSize * m_dwFontInTextureSize;

		int iFontASCSize = 0x100 * iCharDotChunkSize;

		BYTE *pFontDotASC = new BYTE[iFontASCSize];	//ASC

		pfile->ReadData(pFontDotASC,iFontASCSize);

		//写入ASC To CACHE

		D3DLOCKED_RECT d3dlr;
		m_pTexture->LockRect(0,&d3dlr,NULL,D3DLOCK_NOSYSLOCK);

		WORD * pDst = (WORD*)d3dlr.pBits;

		int iPerLineChars = 1024 / m_dwFontInTextureSize;
		//ASC DATA
		for(int i = 0; i < 0x100; i++)
		{
			int ix = i % iPerLineChars;
			int iy = i / iPerLineChars;
			WORD *pBit = pDst+ ix * m_dwFontInTextureSize + iy * m_dwFontInTextureSize * 1024;
			BYTE *pDot = pFontDotASC + iCharDotChunkSize * i; 

			for(DWORD y = 0; y < m_dwFontInTextureSize; y++)
			{
				for(DWORD x = 0; x < m_dwFontInTextureSize;x++)
				{
					BYTE b = pDot[x + y * m_dwFontInTextureSize];
					b = b >> 4;
					WORD c = (b << 12) | 0x0FFF;
					pBit[x + y * 1024] = c;
				}
			}
		}
		SAFEDELETE(pFontDotASC);
		//SIGN DATA
		int iFontSignSize = (0xff - 0xa1) * (0xaa - 0xa1) *  iCharDotChunkSize;
		BYTE *pFontDotSIGN = new BYTE[iFontSignSize];
		pfile->ReadData(pFontDotSIGN,iFontSignSize);

		for(WORD wW = 0xa1; wW< 0xaa; wW++)
		{
			for(WORD wQ = 0xa1 ; wQ < 0xff; wQ++)
			{
				WORD wCode = wQ << 8 | wW;

				int idx = (wQ-0xa1) + (wW-0xa1) * (0xff - 0xa1) + 0x100;
				int ix = idx % iPerLineChars;
				int iy = idx / iPerLineChars;
				WORD *pBit = pDst+ ix * m_dwFontInTextureSize + iy * m_dwFontInTextureSize * 1024;
				BYTE *pDot = pFontDotSIGN + ((wQ-0xa1) + (wW-0xa1) * (0xff - 0xa1)) * iCharDotChunkSize;

				for(DWORD y = 0; y < m_dwFontInTextureSize; y++)
				{
					for(DWORD x = 0; x < m_dwFontInTextureSize; x++)
					{
						BYTE b = pDot[x + y * m_dwFontInTextureSize];
						b = b >> 4;
						WORD c = (b << 12) | 0x0FFF;
						pBit[x + y * 1024] = c;
					}		
				}

			}
		}
		SAFEDELETEARRAY(pFontDotSIGN);

		//CHS DATA
		int iFontCHSSize = (0xff - 0xa1) * (0xf8 - 0xb0) *  iCharDotChunkSize;
		BYTE *pFontDotCHS = new BYTE[iFontCHSSize];
		pfile->ReadData(pFontDotCHS,iFontCHSSize);

		for(WORD wW = 0xb0; wW< 0xf8; wW++)
		{
			for(WORD wQ = 0xa1 ; wQ < 0xff; wQ++)
			{
				int idx = (wQ-0xa1) + (wW-0xb0) * (0xff - 0xa1) + (0xff - 0xa1) * (0xaa - 0xa1) + 0x100;
				int ix = idx % iPerLineChars;
				int iy = idx / iPerLineChars;
				WORD *pBit = pDst+ ix * m_dwFontInTextureSize + iy * m_dwFontInTextureSize * 1024;
				BYTE *pDot = pFontDotCHS + ((wQ-0xa1) + (wW-0xb0) * (0xff - 0xa1)) * iCharDotChunkSize;

				for(DWORD y = 0; y < m_dwFontInTextureSize; y++)
				{
					for(DWORD x = 0; x < m_dwFontInTextureSize; x++)
					{
						BYTE b = pDot[x + y * m_dwFontInTextureSize];
						b = b >> 4;
						WORD c = (b << 12) | 0x0FFF;
						pBit[x + y * 1024] = c;
					}		
				}
			}
		}
		SAFEDELETEARRAY(pFontDotCHS);
		m_pTexture->UnlockRect(NULL);

		rfClose(pfile);
		return TRUE;
	}

	void render::Interface::Layer2D::Font::Destroy()
	{
		DestroyFontTexture();
		DestroyVertexBuffer();
	}

	BOOL render::Interface::Layer2D::Font::CreateFontTexture()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		return SUCCEEDED(pInterface->GetDevice()->CreateTexture(1024,2048,1,0,D3DFMT_A4R4G4B4,D3DPOOL_MANAGED, &m_pTexture,NULL));
	}

	BOOL render::Interface::Layer2D::Font::CreateVertexBuffer()
	{
		render::Interface *pInterface = render::Interface::GetInstance();

		if (FAILED(pInterface->GetDevice()->CreateVertexBuffer( sizeof(tagPOSTDIFTEX0) * 6 * MAX_RENDER_LAYER2D_VB_SIZE
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC  , Layer2D::FVF,D3DPOOL_SYSTEMMEM , &m_pVB,NULL)))	
		{
			OutputConsole("error : font:: Create VextexBuffer failed.\n");
			ASSERT(0);
			return FALSE;
		} 
		return TRUE;
	}

	void render::Interface::Layer2D::Font::DestroyVertexBuffer()
	{
		SAFERELEASE(m_pVB);
	}

	void render::Interface::Layer2D::Font::DestroyFontTexture()
	{
		SAFERELEASE(m_pTexture);		
	}

	void render::Interface::Layer2D::Font::_DEBUG_RenderFontTexture(int iX,int iY)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		//// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);


		//Setup VertexShader
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

		pDevice->SetVertexShader(NULL);
		pDevice->SetPixelShader(NULL);

		struct tagVertex
		{
			D3DXVECTOR3 pos;
			float rhw;
			DWORD color;
			D3DXVECTOR2 uv;
		};


		float sx = (float)iX, sy = (float)iY;
		static tagVertex pVertices[6];
		//// 填充顶点缓冲区

		//// 0   5--4
		//// | \  \ |
		//// 1--2  \3

		pVertices[0].color = 
			pVertices[1].color = 
			pVertices[2].color = 
			pVertices[3].color = 
			pVertices[4].color = 
			pVertices[5].color = 0xFFFFFFFF;

		pVertices[0].pos.x = sx;
		pVertices[1].pos.x = sx;
		pVertices[2].pos.x = sx + 1024;

		pVertices[0].pos.y = sy;
		pVertices[1].pos.y = sy + 2048;
		pVertices[2].pos.y = sy + 2048;

		pVertices[3].pos.x = sx + 1024;
		pVertices[4].pos.x = sx + 1024; 
		pVertices[5].pos.x = sx;

		pVertices[3].pos.y = sy + 2048;
		pVertices[4].pos.y = sy;
		pVertices[5].pos.y = sy;

		pVertices[0].uv.x = 0;
		pVertices[1].uv.x = 0;
		pVertices[2].uv.x = 1;

		pVertices[0].uv.y = 0;
		pVertices[1].uv.y = 1;
		pVertices[2].uv.y = 1;

		pVertices[3].uv.x = 1;
		pVertices[4].uv.x = 1;
		pVertices[5].uv.x = 0;

		pVertices[3].uv.y = 1;
		pVertices[4].uv.y = 0;
		pVertices[5].uv.y = 0;

		pVertices[0].pos.z = 
			pVertices[1].pos.z = 
			pVertices[2].pos.z = 
			pVertices[3].pos.z = 
			pVertices[4].pos.z = 
			pVertices[5].pos.z = 0.5f;

		pVertices[0].rhw = 
			pVertices[1].rhw = 
			pVertices[2].rhw = 
			pVertices[3].rhw = 
			pVertices[4].rhw = 
			pVertices[5].rhw = 1.0f;	

		pDevice->SetTexture(0,m_pTexture);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,6,pVertices,sizeof(tagVertex));
	}

	void render::Interface::Layer2D::Font::Reset()
	{
		Destroy();
		Create(m_strFileName.c_str());
	}
	void render::Interface::Layer2D::Font::ResetRelease()
	{
		Destroy();
	}
	void render::Interface::Layer2D::Font::ResetReBuild()
	{
		Create(m_strFileName.c_str());
	}

	void render::Interface::Layer2D::Font::BeginRender()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		// SETUP SAMPLER TEXTURE FILTER
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		pDevice->SetTexture(0,GetTexture());
		pInterface->GetDevice()->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pInterface->GetDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);   
	}

	void render::Interface::Layer2D::Font::RenderText(int iX,int iY,const char *szText,int iLen,DWORD dwColor,float fScale)
	{
		//循环打入
		if (render::Interface::GetInstance()->GetDeviceLost())
		{
			return;
		}

		float sx = (float)iX, sy = (float)iY,offset=0, tx0=0, ty0=0, tx1=0, ty1=0;

		float w,h;


		tagPOSTDIFTEX0 * pVertices = NULL;
		UINT wNumTriangles = 0;
		LPDIRECT3DVERTEXBUFFER9 pVB_POSITIONTDIFFUSETEXCOORD0 = m_pVB;
		pVB_POSITIONTDIFFUSETEXCOORD0->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
		if (pVertices == NULL)
		{
			pVB_POSITIONTDIFFUSETEXCOORD0->Unlock();
			return;
		}

		int iTexLen = lstrlen( szText ) ;

		if (iLen > iTexLen) iLen = iTexLen;

		if (iLen >= MAX_RENDER_LAYER2D_VB_SIZE) iLen = MAX_RENDER_LAYER2D_VB_SIZE - 1;

		int   iFontSize = (int)GetFontSize();
		int   iFontInTextureSize = (int)GetFontInTextureSize();
		int   iLineChars = 1024 / iFontInTextureSize;
		int   iBytes = iFontInTextureSize / 8;
		int   iCharDotChunkSize = iBytes * iFontInTextureSize;
		h = w = (float)iFontInTextureSize * fScale;

		WORD  wChar(0);

		int n = 0;
		while(n<iLen)
		{
			unsigned char c = szText[n++];		

			if ( c=='\n' )
			{
				sy += iFontSize;
				sx=(float)iX;
				continue;
			}

			int idx = 0;
			if (c & 0x80 )
			{
				if (n+1 > iLen)
				{
					//当末尾中文字符只有半个的非正常情况
					break;
				}

				offset = (float)iFontSize * fScale;   

				WORD wW = c;
				WORD wQ = (BYTE)szText[n++];

				if (wQ < 0xa1 || wQ >= 0xff)
				{
					//无效的编码
					idx = '?';
				}
				else
				{
					if (wW >= 0xa1 && wW < 0xaa )
					{
						//SIGN a1 - aa
						idx = (wQ-0xa1) + (wW-0xa1) * (0xff - 0xa1) + 0x100;
					}
					else
					{
						if (wW >= 0xb0 && wW < 0xf8)
						{
							//CHS b0 - f8
							idx = (wQ-0xa1) + (wW-0xb0) * (0xff - 0xa1) + 0x100 + (0xaa - 0xa1) * (0xff - 0xa1);
						}
						else
						{
							//无效的中文
							idx = '?';
						}
					}
				}

			}
			else
			{
				idx = c;

				offset = (float)iFontSize * 0.5f * fScale;				

			}

			int a = (idx % iLineChars) * iFontInTextureSize;
			int b = (idx / iLineChars) * iFontInTextureSize;
			// 计算纹理左上角 0.0-1.0
			tx0 = (float)(a) / 1024.0f;
			ty0 = (float)(b) / 2048.0f;

			// 计算纹理右下角 0.0-1.0
			tx1 = tx0 + (float)iFontInTextureSize / 1024.0f;
			ty1 = ty0 + (float)iFontInTextureSize / 2048.0f;	

			// 填充顶点缓冲区

			pVertices[0].color = 
				pVertices[1].color = 
				pVertices[2].color = 
				pVertices[3].color = 
				pVertices[4].color = 
				pVertices[5].color = dwColor;

			pVertices[0].x = sx;
			pVertices[1].x = sx;
			pVertices[2].x = sx + w;

			pVertices[0].y = sy;
			pVertices[1].y = sy + h;
			pVertices[2].y = sy + h;

			pVertices[3].x = sx + w;
			pVertices[4].x = sx + w; 
			pVertices[5].x = sx;

			pVertices[3].y = sy + h;
			pVertices[4].y = sy;
			pVertices[5].y = sy;

			pVertices[0].u = tx0;
			pVertices[1].u = tx0;
			pVertices[2].u = tx1;

			pVertices[0].v = ty0;
			pVertices[1].v = ty1;
			pVertices[2].v = ty1;

			pVertices[3].u = tx1;
			pVertices[4].u = tx1;
			pVertices[5].u = tx0;

			pVertices[3].v = ty1;
			pVertices[4].v = ty0;
			pVertices[5].v = ty0;

			pVertices[0].z = 
				pVertices[1].z = 
				pVertices[2].z = 
				pVertices[3].z = 
				pVertices[4].z = 
				pVertices[5].z = 0.5f;
			pVertices[0].rhw = 
				pVertices[1].rhw = 
				pVertices[2].rhw = 
				pVertices[3].rhw = 
				pVertices[4].rhw = 
				pVertices[5].rhw = 1.0f;	

			pVertices += 6;

			wNumTriangles+=2;

			sx+=offset;	// 坐标x增量
		}

		pVB_POSITIONTDIFFUSETEXCOORD0->Unlock();
		if (wNumTriangles == 0)
		{
			return;
		}

		render::Interface *pInterface = render::Interface::GetInstance();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles);

	}

	void render::Interface::Layer2D::Font::RenderText(int iX,int iY,const char *szText,DWORD *pColor,int iLen,float fScale)
	{
		//循环打入
		if (render::Interface::GetInstance()->GetDeviceLost())
		{
			return;
		}

		float sx = (float)iX, sy = (float)iY,offset=0, tx0=0, ty0=0, tx1=0, ty1=0;

		float w,h;

		tagPOSTDIFTEX0 * pVertices = NULL;
		UINT wNumTriangles = 0;
		LPDIRECT3DVERTEXBUFFER9 pVB_POSITIONTDIFFUSETEXCOORD0 = m_pVB;
		pVB_POSITIONTDIFFUSETEXCOORD0->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

		int iTexLen = lstrlen( szText ) ;

		if (iLen > iTexLen) iLen = iTexLen;

		if (iLen >= MAX_RENDER_LAYER2D_VB_SIZE) iLen = MAX_RENDER_LAYER2D_VB_SIZE - 1;

		int   iFontSize = (int)GetFontSize();
		int   iFontInTextureSize = (int)GetFontInTextureSize();
		int   iLineChars = 1024 / iFontInTextureSize;
		int   iBytes = iFontInTextureSize / 8;
		int   iCharDotChunkSize = iBytes * iFontInTextureSize;
		h = w = (float)iFontInTextureSize * fScale;

		WORD  wChar(0);

		int n = 0;
		while(n<iLen)
		{
			unsigned char c = szText[n++];		

			if ( c=='\n' )
			{
				sy += iFontSize;
				sx=(float)iX;
				continue;
			}

			int idx = 0;
			if (c & 0x80 )
			{
				if (n+1 > iLen)
				{
					//当末尾中文字符只有半个的非正常情况
					break;
				}

				offset = (float)iFontSize * fScale;   

				WORD wW = c;
				WORD wQ = (BYTE)szText[n++];

				if (wQ < 0xa1 || wQ >= 0xff)
				{
					//无效的编码
					idx = '?';
				}
				else
				{
					if (wW >= 0xa1 && wW < 0xaa )
					{
						//SIGN a1 - aa
						idx = (wQ-0xa1) + (wW-0xa1) * (0xff - 0xa1) + 0x100;
					}
					else
					{
						if (wW >= 0xb0 && wW < 0xf8)
						{
							//CHS b0 - f8
							idx = (wQ-0xa1) + (wW-0xb0) * (0xff - 0xa1) + 0x100 + (0xaa - 0xa1) * (0xff - 0xa1);
						}
						else
						{
							//无效的中文
							idx = '?';
						}
					}
				}

			}
			else
			{
				idx = c;

				offset = (float)iFontSize * 0.5f * fScale;				

			}

			int a = (idx % iLineChars) * iFontInTextureSize;
			int b = (idx / iLineChars) * iFontInTextureSize;
			// 计算纹理左上角 0.0-1.0
			tx0 = (float)(a) / 1024.0f;
			ty0 = (float)(b) / 2048.0f;

			// 计算纹理右下角 0.0-1.0
			tx1 = tx0 + (float)iFontInTextureSize / 1024.0f;
			ty1 = ty0 + (float)iFontInTextureSize / 2048.0f;	

			// 填充顶点缓冲区

			pVertices[0].color = 
				pVertices[1].color = 
				pVertices[2].color = 
				pVertices[3].color = 
				pVertices[4].color = 
				pVertices[5].color = pColor[n];

			pVertices[0].x = sx;
			pVertices[1].x = sx;
			pVertices[2].x = sx + w;

			pVertices[0].y = sy;
			pVertices[1].y = sy + h;
			pVertices[2].y = sy + h;

			pVertices[3].x = sx + w;
			pVertices[4].x = sx + w; 
			pVertices[5].x = sx;

			pVertices[3].y = sy + h;
			pVertices[4].y = sy;
			pVertices[5].y = sy;

			pVertices[0].u = tx0;
			pVertices[1].u = tx0;
			pVertices[2].u = tx1;

			pVertices[0].v = ty0;
			pVertices[1].v = ty1;
			pVertices[2].v = ty1;

			pVertices[3].u = tx1;
			pVertices[4].u = tx1;
			pVertices[5].u = tx0;

			pVertices[3].v = ty1;
			pVertices[4].v = ty0;
			pVertices[5].v = ty0;

			pVertices[0].z = 
				pVertices[1].z = 
				pVertices[2].z = 
				pVertices[3].z = 
				pVertices[4].z = 
				pVertices[5].z = 0.5f;
			pVertices[0].rhw = 
				pVertices[1].rhw = 
				pVertices[2].rhw = 
				pVertices[3].rhw = 
				pVertices[4].rhw = 
				pVertices[5].rhw = 1.0f;	

			pVertices += 6;

			wNumTriangles+=2;

			sx+=offset;	// 坐标x增量
		}

		pVB_POSITIONTDIFFUSETEXCOORD0->Unlock();
		if (wNumTriangles == 0)
		{
			return;
		}

		render::Interface *pInterface = render::Interface::GetInstance();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles);

	}

	void render::Interface::Layer2D::Font::EndRender()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		pInterface->GetDevice()->SetTexture(0,NULL);
		// Setup sampler Texture Filter
	}

	//__CLASS__(render::Interface::Layer2D)_____
	render::Interface::Layer2D::Layer2D(void) :
	m_dwFillMode(D3DFILL_SOLID),
		m_eFilterType(D3DTEXF_POINT),
		m_iCurrentFontsSize(0)
	{
		// add by lpf 2007-11-27
		memset(m_pFonts, 0, sizeof(Font *) * MAX_RENDER_LAYER2D_FONT_SIZE);
	}
	render::Interface::Layer2D::~Layer2D(void)
	{
	}



	/*
	* 功能: 创建2D渲染层
	* 摘要: -
	* 参数: tSetting - 当前设备的配置信息
	* 返回值: 总是返回 TRUE
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2007.11.27 - lpf
	*		循环调用CreateFont()，来创建字体样式。
	*	2008.05.26 - lpf
	*		增加了对成员变量m_eFilterType(纹理过滤类型)判断赋值。
	*/
	BOOL render::Interface::Layer2D::Create(const render::Interface::_tagSetting & tSetting)
	{
		for (DWORD dw = 0; dw < MAX_RENDER_LAYER2D_FONT_SIZE; ++dw)
		{
			if (strlen(tSetting.szFonts[dw]) != 0)
				CreateFont(tSetting.szFonts[dw]);
		}

		//当前2D渲染层字库渲染信息
		m_tFontInfo.iStyleIndex	   = 0;
		m_tFontInfo.bBorder		   = false;
		m_tFontInfo.bShadow		   = false;
		m_tFontInfo.dwShadowOffset = 1;
		m_tFontInfo.fScale		   = 1.0f;
		m_tFontInfo.dwColorFont	   = 0xffffffff;
		m_tFontInfo.dwColorBorder  = 0xff000000;
		m_tFontInfo.dwColorShadow  = 0xff808080;

		CreateVertexBuffer();

		// 判断并为线性过滤类型赋值
		if (tSetting.b2DLinear)
			m_eFilterType = D3DTEXF_LINEAR;
		else
			m_eFilterType = D3DTEXF_POINT;

		return TRUE;
	}



	// modify by lpf 2007-11-27 - 创建字体类实例对象，并将指针保存到系统字体数组中去，以便以后调用。
	BOOL render::Interface::Layer2D::CreateFont(const char *pszFontFileName)
	{
		if (m_iCurrentFontsSize >= MAX_RENDER_LAYER2D_FONT_SIZE)
		{
			OutputConsole("error: render::Layer2D::CreateFont: Create font failed. Font styles is full! \n");
			ASSERT(0);
			return FALSE;
		}

		m_pFonts[m_iCurrentFontsSize] = new Font;
		if (!m_pFonts[m_iCurrentFontsSize]->Create(pszFontFileName))
		{
			OutputConsole("error: render::Layer2D::CreateFont: Create font failed.\n");
			ASSERT(0);
		}

		++m_iCurrentFontsSize;
		return FALSE;
	}

	BOOL render::Interface::Layer2D::CreateVertexBuffer()
	{
		render::Interface *pInterface = render::Interface::GetInstance();

		if (FAILED(pInterface->GetDevice()->CreateVertexBuffer( sizeof(tagD3DFVF_2D) * 6 * MAX_RENDER_LAYER2D_VB_SIZE
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY  , Layer2D::FVF,D3DPOOL_SYSTEMMEM , &m_pVB,NULL)))	
		{
			OutputConsole("error : render::Interface:Layer2D Create VextexBuffer failed.\n");
			ASSERT(0);
			return FALSE;
		} 
		return TRUE;
	}

	void render::Interface::Layer2D::DestroyFont()
	{
		// modify by lpf 2007-11-27
		for (int i = 0; i < MAX_RENDER_LAYER2D_FONT_SIZE; ++i)
			SAFEDESTROY(m_pFonts[i]);

		m_iCurrentFontsSize = 0;
	}

	void render::Interface::Layer2D::DestroyVertexBuffer()
	{
		SAFERELEASE(m_pVB);
	}

	void render::Interface::Layer2D::Destroy(void)
	{
		DestroyVertexBuffer();
		DestroyFont();
	}

	void render::Interface::Layer2D::Update()
	{
		DestroyVertexBuffer();
		CreateVertexBuffer();

		// modify by lpf 2007-11-27
		for (int i = 0; i < m_iCurrentFontsSize; ++i)
			m_pFonts[i]->Reset();
	}
	void render::Interface::Layer2D::UpdateRelease()
	{
		DestroyVertexBuffer();
		// modify by lpf 2007-11-27
		for (int i = 0; i < m_iCurrentFontsSize; ++i)
			m_pFonts[i]->ResetRelease();
	}
	void render::Interface::Layer2D::UpdateRebuild()
	{
		CreateVertexBuffer();

		// modify by lpf 2007-11-27
		for (int i = 0; i < m_iCurrentFontsSize; ++i)
			m_pFonts[i]->ResetReBuild();
	}

	void render::Interface::Layer2D::DrawLine(int iStartX,int iStartY,int iEndX,int iEndY,D3DCOLOR dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
		LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		pInterface->SetTexture(0,NULL);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}

		pVertices[0].pos.x = (float)iStartX;
		pVertices[0].pos.y = (float)iStartY;
		pVertices[0].pos.z = 
			pVertices[0].rhw = 1.0f;
		pVertices[0].color = dwColor;

		pVertices[1].pos.z = 
			pVertices[1].rhw = 1.0f;
		pVertices[1].pos.x = (float)iEndX;
		pVertices[1].pos.y = (float)iEndY;
		pVertices[1].color = dwColor;

		pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_LINELIST, 0, 1);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}
	void render::Interface::Layer2D::DrawWireQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		pInterface->SetTexture(0,NULL);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputWarring(" render::interface::Layer2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}

		pVertices[0].pos.x = (float)iX;
		pVertices[0].pos.y = (float)iY;
		pVertices[0].pos.z = 
			pVertices[0].rhw = 1.0f;

		pVertices[1].pos.x = (float)iX;
		pVertices[1].pos.y = (float)(iY+iHeight-1);
		pVertices[1].pos.z = 
			pVertices[1].rhw = 1.0f;

		pVertices[2].pos.x = (float)iX;
		pVertices[2].pos.y = (float)(iY+iHeight-1);
		pVertices[2].pos.z = 
			pVertices[2].rhw = 1.0f;

		pVertices[3].pos.x = (float)(iX+iWidth-1);
		pVertices[3].pos.y = (float)(iY+iHeight-1);
		pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;	

		pVertices[4].pos.x = (float)(iX+iWidth-1);
		pVertices[4].pos.y = (float)(iY+iHeight-1);
		pVertices[4].pos.z = 
			pVertices[4].rhw = 1.0f;	

		pVertices[5].pos.x = (float)(iX+iWidth-1);
		pVertices[5].pos.y = (float)iY;
		pVertices[5].pos.z = 
			pVertices[5].rhw = 1.0f;

		pVertices[6].pos.x = (float)(iX+iWidth-1);
		pVertices[6].pos.y = (float)iY;
		pVertices[6].pos.z = 
			pVertices[6].rhw = 1.0f;	

		pVertices[7].pos.x = (float)iX;
		pVertices[7].pos.y = (float)iY;
		pVertices[7].pos.z = 
			pVertices[7].rhw = 1.0f;	

		pVertices[0].color =
			pVertices[1].color = 
			pVertices[2].color = 
			pVertices[3].color = 
			pVertices[4].color = 
			pVertices[5].color = 
			pVertices[6].color = 
			pVertices[7].color = dwColor;

		m_pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_LINELIST, 0, 4);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}

	void render::Interface::Layer2D::DrawWireQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		pInterface->SetTexture(0,NULL);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
		{
			OutputWarring(" render::interface::Layer2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}

		pVertices[0].pos.x = (float)iX1;
		pVertices[0].pos.y = (float)iY1;
		pVertices[0].pos.z = 
			pVertices[0].rhw = 1.0f;

		pVertices[1].pos.x = (float)iX1;
		pVertices[1].pos.y = (float)(iY2);
		pVertices[1].pos.z = 
			pVertices[1].rhw = 1.0f;

		pVertices[2].pos.x = (float)iX1;
		pVertices[2].pos.y = (float)(iY2);
		pVertices[2].pos.z = 
			pVertices[2].rhw = 1.0f;

		pVertices[3].pos.x = (float)(iX2);
		pVertices[3].pos.y = (float)(iY2);
		pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;	

		pVertices[4].pos.x = (float)(iX2);
		pVertices[4].pos.y = (float)(iY2);
		pVertices[4].pos.z = 
			pVertices[4].rhw = 1.0f;	

		pVertices[5].pos.x = (float)(iX2);
		pVertices[5].pos.y = (float)iY1;
		pVertices[5].pos.z = 
			pVertices[5].rhw = 1.0f;

		pVertices[6].pos.x = (float)(iX2);
		pVertices[6].pos.y = (float)iY1;
		pVertices[6].pos.z = 
			pVertices[6].rhw = 1.0f;	

		pVertices[7].pos.x = (float)iX1;
		pVertices[7].pos.y = (float)iY1;
		pVertices[7].pos.z = 
			pVertices[7].rhw = 1.0f;	

		pVertices[0].color =
			pVertices[1].color = 
			pVertices[2].color = 
			pVertices[3].color = 
			pVertices[4].color = 
			pVertices[5].color = 
			pVertices[6].color = 
			pVertices[7].color = dwColor;

		m_pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_LINELIST, 0, 4);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}
	void render::Interface::Layer2D::DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		pInterface->SetTexture(0,NULL);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" Layer2D:: DrawSolidQuad Lock VB2D failed.\n");
			return ;
		}

		pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
		pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

		pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
		pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

		pVertices[0].color = 
			pVertices[1].color = 
			pVertices[2].color = 
			pVertices[3].color = color;

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		m_pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}

	void render::Interface::Layer2D::DrawSolidQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
		LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		pInterface->SetTexture(0,NULL);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}

		pVertices[0].pos.x = pVertices[2].pos.x = (float)iX1;
		pVertices[1].pos.x = pVertices[3].pos.x = (float)iX2;

		pVertices[0].pos.y = pVertices[1].pos.y = (float)iY1;
		pVertices[2].pos.y = pVertices[3].pos.y = (float)iY2;

		pVertices[0].color = 
			pVertices[1].color = 
			pVertices[2].color = 
			pVertices[3].color = color;

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}



	/*
	* 功能: 绘制纹理
	* 摘要: -
	* 参数: iX				 - 绘制X坐标
	*		 iY				 - 绘制Y坐标
	*		 dwTextureHandle - 纹理句柄
	*		 dwColor		 - 绘制纹理时的颜色叠加
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了根据当前渲染层纹理过滤状态来绘制纹理的功能
	*/
	void render::Interface::Layer2D::DrawTexture(int iX,int iY,DWORD dwTextureHandle,D3DCOLOR dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, m_eFilterType);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		render::Texture * pTexture = pInterface->GetTexture(dwTextureHandle);
		float fWidth = (float)pTexture->GetWidth();
		float fHeight= (float)pTexture->GetHeight();

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" Layer2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}

		pInterface->SetTexture(0,dwTextureHandle);

		pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
		pVertices[1].pos.x = pVertices[3].pos.x = (float)iX+fWidth;

		pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
		pVertices[2].pos.y = pVertices[3].pos.y = (float)iY+fHeight;

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		pVertices[0].color =
			pVertices[1].color =
			pVertices[2].color =
			pVertices[3].color = dwColor;

		pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
		pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
		pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
		pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

		m_pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}



	/*
	* 功能: 绘制旋转纹理
	* 摘要: -
	* 参数: iX				 - 绘制X坐标
	*		 iY				 - 绘制Y坐标
	*		 fAngle			 - 旋转弧度
	*		 dwTextureHandle - 纹理句柄
	*		 dwColor		 - 绘制纹理时的颜色叠加
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了根据当前渲染层纹理过滤状态来绘制纹理的功能
	*/
	void render::Interface::Layer2D::DrawTextureRotation(int iX,int iY,float fAngle,DWORD dwTextureHandle,D3DCOLOR color)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, m_eFilterType);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		render::Texture * pTexture = pInterface->GetTexture(dwTextureHandle);
		float fHalfWidth = (float)pTexture->GetWidth()  * 0.5f;
		float fHalfHeight= (float)pTexture->GetHeight() * 0.5f;

		static D3DXMATRIX matWorld,matTrans,matRot;
		D3DXMatrixTranslation(&matTrans,(float)iX,(float)iY,0);
		D3DXMatrixRotationZ(&matRot,fAngle);
		D3DXMatrixMultiply(&matWorld,&matRot,&matTrans);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}

		pInterface->SetTexture(0,dwTextureHandle);

		pVertices[0].pos.x = pVertices[2].pos.x = - fHalfWidth;
		pVertices[1].pos.x = pVertices[3].pos.x =   fHalfWidth;

		pVertices[0].pos.y = pVertices[1].pos.y = - fHalfHeight;
		pVertices[2].pos.y = pVertices[3].pos.y =   fHalfHeight;

		for(int i = 0; i < 4; i++)
		{
			D3DXVec3TransformCoord(&pVertices[i].pos,&pVertices[i].pos,&matWorld);
		}

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		pVertices[0].color =
			pVertices[1].color =
			pVertices[2].color =
			pVertices[3].color = color;

		pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
		pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
		pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
		pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

		m_pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}



	/*
	* 功能: 绘制纹理
	* 摘要: -
	* 参数: iX				 - 绘制X坐标
	*		 iY				 - 绘制Y坐标
	*		 iWidth			 - 绘制宽度
	*		 iHeight		 - 绘制高度
	*		 dwTextureHandle - 纹理句柄
	*		 dwColor		 - 绘制纹理时的颜色叠加
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了根据当前渲染层纹理过滤状态来绘制纹理的功能
	*/
	void render::Interface::Layer2D::DrawTexture(int iX,int iY,int iWidth,int iHeight,DWORD dwTextureHandle,D3DCOLOR dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, m_eFilterType);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		//render::Texture * pTexture = pInterface->GetTexture(dwTextureHandle);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" Layer2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}
		pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
		pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

		pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
		pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		pVertices[0].color =
			pVertices[1].color =
			pVertices[2].color =
			pVertices[3].color = dwColor;

		pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
		pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
		pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
		pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

		m_pVB->Unlock();

		//bind stream source
		pInterface->SetTexture(0,dwTextureHandle);
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}



	/*
	* 功能: 绘制纹理
	* 摘要: -
	* 参数: iX				 - 绘制X坐标
	*		 iY				 - 绘制Y坐标
	*		 iWidth			 - 绘制宽度
	*		 iHeight		 - 绘制高度
	*		 uOffset		 - 纹理U坐标偏移
	*		 vOffset		 - 纹理V坐标偏移
	*		 fScale			 - 缩放比例
	*		 dwTextureHandle - 纹理句柄
	*		 dwColor		 - 绘制纹理时的颜色叠加
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了根据当前渲染层纹理过滤状态来绘制纹理的功能
	*/
	void render::Interface::Layer2D::DrawTexture(int iX,int iY,int iWidth,int iHeight,float uOffset,float vOffset,float fScale,DWORD dwTextureHandle,D3DCOLOR dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		DWORD dwFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,&dwFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);
		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, m_eFilterType);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		render::Texture * pTexture = pInterface->GetTexture(dwTextureHandle);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}

		pInterface->SetTexture(0,dwTextureHandle);

		pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
		pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

		pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
		pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		pVertices[0].color =
			pVertices[1].color =
			pVertices[2].color =
			pVertices[3].color = dwColor;

		pVertices[0].uv = D3DXVECTOR2(0.0f + uOffset,0.0f + vOffset) * fScale;	
		pVertices[1].uv = D3DXVECTOR2(1.0f + uOffset,0.0f + vOffset) * fScale;	
		pVertices[2].uv = D3DXVECTOR2(0.0f + uOffset,1.0f + vOffset) * fScale;	
		pVertices[3].uv = D3DXVECTOR2(1.0f + uOffset,1.0f + vOffset) * fScale;

		m_pVB->Unlock();

		//bind stream source
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,dwFogEnable);
	}



	/*
	* 功能: 绘制纹理
	* 摘要: 画纹理的某一区域
	* 参数: iX				 - 绘制X坐标
	*		 iY				 - 绘制Y坐标
	*		 pRect			 - 纹理内的需要绘制区域矩形
	*		 dwTextureHandle - 纹理句柄
	*		 dwColor		 - 绘制纹理时的颜色叠加
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了根据当前渲染层纹理过滤状态来绘制纹理的功能
	*/
	void render::Interface::Layer2D::DrawTextureSubRect(int iX,int iY,const RECT *pRect,DWORD dwTextureHandle,D3DCOLOR color)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
		LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();	

		DWORD dwFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,&dwFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, m_eFilterType);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		render::Texture * pTexture = pInterface->GetTexture(dwTextureHandle);
		float fWidth = (float)pTexture->GetWidth();
		float fHeight= (float)pTexture->GetHeight();

		float fDrawWidth =(float)(pRect->right - pRect->left) / fWidth  * fWidth;
		float fDrawHeight=(float)(pRect->bottom- pRect->top)  / fHeight * fHeight;

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
			return ;
		}	

		pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
		pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+fDrawWidth);

		pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
		pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+fDrawHeight);

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		pVertices[0].color =
			pVertices[1].color =
			pVertices[2].color =
			pVertices[3].color = color;

		//计算UV

		float uLeft   = pRect->left / fWidth;
		float uRight  = pRect->right / fWidth;
		float vTop    = pRect->top / fHeight;
		float vBottom = pRect->bottom / fHeight;

		pVertices[0].uv = D3DXVECTOR2(uLeft,vTop);		
		pVertices[2].uv = D3DXVECTOR2(uLeft,vBottom);		
		pVertices[1].uv = D3DXVECTOR2(uRight,vTop);		
		pVertices[3].uv = D3DXVECTOR2(uRight,vBottom);

		pVB->Unlock();

		pInterface->SetTexture(0,dwTextureHandle);
		//bind stream source
		pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,dwFogEnable);
	}



	/*
	* 功能: 绘制纹理
	* 摘要: 画纹理的某一区域
	* 参数: iX				 - 绘制X坐标
	*		 iY				 - 绘制Y坐标
	*		 iWidth			 - 绘制宽度
	*		 iHeight		 - 绘制高度
	*		 pRect			 - 纹理内的需要绘制区域矩形
	*		 dwTextureHandle - 纹理句柄
	*		 dwColor		 - 绘制纹理时的颜色叠加
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了根据当前渲染层纹理过滤状态来绘制纹理的功能
	*/
	void render::Interface::Layer2D::DrawTextureSubRect(int iX,int iY,int iWidth,int iHeight,const RECT *pRect,DWORD dwTextureHandle,D3DCOLOR dwColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

		DWORD dwFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,&dwFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// Setup sampler Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, m_eFilterType);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//Setup Color OP
		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		//Setup Color OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//Setup Alpha OP
		pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		render::Texture * pTexture = pInterface->GetTexture(dwTextureHandle);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
		{
			OutputConsole(" render::Interface::Layer2D::DrawTextureSubRect:: DrawTexture Lock VB2D failed.\n");
			return ;
		}
		float fWidth = (float)pTexture->GetWidth();
		float fHeight= (float)pTexture->GetHeight();

		pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
		pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

		pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
		pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

		pVertices[0].pos.z = 
			pVertices[0].rhw = 
			pVertices[1].pos.z = 
			pVertices[1].rhw = 
			pVertices[2].pos.z = 
			pVertices[2].rhw = 
			pVertices[3].pos.z = 
			pVertices[3].rhw = 1.0f;

		pVertices[0].color =
			pVertices[1].color =
			pVertices[2].color =
			pVertices[3].color = dwColor;

		//计算UV

		float uLeft   = pRect->left / fWidth;
		float uRight  = pRect->right / fWidth;
		float vTop    = pRect->top / fHeight;
		float vBottom = pRect->bottom / fHeight;

		pVertices[0].uv = D3DXVECTOR2(uLeft,vTop);		
		pVertices[2].uv = D3DXVECTOR2(uLeft,vBottom);		
		pVertices[1].uv = D3DXVECTOR2(uRight,vTop);		
		pVertices[3].uv = D3DXVECTOR2(uRight,vBottom);

		m_pVB->Unlock();

		//bind stream source
		pInterface->SetTexture(0,dwTextureHandle);
		pDevice->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
		pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->SetRenderState(D3DRS_FOGENABLE,dwFogEnable);
	}



	/*
	* 功能: 根据字体信息来输出文本
	* 摘要: -
	* 参数: iX		   - 绘制文本左上角X坐标
	*		 iY		   - 绘制文本左上角Y坐标
	*		 pszString - 绘制文本内容
	*		 pFontInfo - 绘制的字体信息(如果为空则使用2D渲染层的默认字体信息进行绘制)
	* 返回值: -
	* 作者: lpf
	* 创建日期: 2007.11.28
	* 修改日志:
	*	2008.04.18 - lpf
	*		增加了一个_tagFontInfo *参数,用来区别是否使用2D渲染层默认字体信息进行绘制
	*/
	void render::Interface::Layer2D::DrawText(int iX, int iY, const char * pszString, _tagFontInfo * pFontInfo)
	{
		_tagFontInfo * pUseFontInfo = pFontInfo;

		if (pFontInfo == NULL)
			pUseFontInfo = &m_tFontInfo;

		if (pUseFontInfo->iStyleIndex >= MAX_RENDER_LAYER2D_FONT_SIZE ||
			m_pFonts[pUseFontInfo->iStyleIndex] == NULL ||
			pszString == NULL)
			return;

		m_pFonts[pUseFontInfo->iStyleIndex]->BeginRender();

		// 判断渲染样式
		if (pUseFontInfo->bBorder)
		{			
			// 绘制边框
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX - 1, iY, pszString, strlen(pszString), pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX, iY - 1, pszString, strlen(pszString), pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX + 1, iY, pszString, strlen(pszString), pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX, iY + 1, pszString, strlen(pszString), pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
		}else if (pUseFontInfo->bShadow)
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX + pUseFontInfo->dwShadowOffset, iY + pUseFontInfo->dwShadowOffset, pszString, strlen(pszString), pUseFontInfo->dwColorShadow, pUseFontInfo->fScale);		//绘制阴影

		m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX, iY, pszString, strlen(pszString), pUseFontInfo->dwColorFont, pUseFontInfo->fScale);		//绘制字体

		m_pFonts[pUseFontInfo->iStyleIndex]->EndRender();
	}



	/*
	* 功能: 根据字体信息来输出指定长度的文本
	* 摘要: -
	* 参数: iX		   - 绘制文本左上角X坐标
	*		 iY		   - 绘制文本左上角Y坐标
	*		 pszString - 绘制文本内容
	*		 iLen	   - 绘制字符长度
	*		 pFontInfo - 绘制的字体信息(如果为空则使用2D渲染层的默认字体信息进行绘制)
	* 返回值: -
	* 作者: lpf
	* 创建日期: 2008.01.03
	* 修改日志:
	*	2008.04.18 - lpf
	*		增加了一个_tagFontInfo *参数,用来区别是否使用2D渲染层默认字体信息进行绘制
	*/
	void render::Interface::Layer2D::DrawText(int iX, int iY, const char * pszString, int iLen, _tagFontInfo * pFontInfo)
	{
		_tagFontInfo * pUseFontInfo = pFontInfo;

		if (pFontInfo == NULL)
			pUseFontInfo = &m_tFontInfo;

		if (pUseFontInfo->iStyleIndex >= MAX_RENDER_LAYER2D_FONT_SIZE ||
			m_pFonts[pUseFontInfo->iStyleIndex] == NULL ||
			pszString == NULL)
			return;

		m_pFonts[pUseFontInfo->iStyleIndex]->BeginRender();

		// 判断渲染样式
		if (pUseFontInfo->bBorder)
		{
			// 绘制边框
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX - 1, iY, pszString, iLen, pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX, iY - 1, pszString, iLen, pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX + 1, iY, pszString, iLen, pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX, iY + 1, pszString, iLen, pUseFontInfo->dwColorBorder, pUseFontInfo->fScale);
		}else if (pUseFontInfo->bShadow)
			m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX + pUseFontInfo->dwShadowOffset, iY + pUseFontInfo->dwShadowOffset, pszString, iLen, pUseFontInfo->dwColorShadow, pUseFontInfo->fScale);		//绘制阴影

		m_pFonts[pUseFontInfo->iStyleIndex]->RenderText(iX, iY, pszString, iLen, pUseFontInfo->dwColorFont, pUseFontInfo->fScale);		//绘制字体

		m_pFonts[pUseFontInfo->iStyleIndex]->EndRender();
	}

	//带颜色码配置
	// modify by lpf 2007-11-27 - 修改了字体渲染的调用
	void render::Interface::Layer2D::TextOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color, int iStyleIndex, float fScale)
	{
		if (iStyleIndex >= MAX_RENDER_LAYER2D_FONT_SIZE ||
			m_pFonts[iStyleIndex] == NULL)
			return;

		m_pFonts[iStyleIndex]->BeginRender();
		m_pFonts[iStyleIndex]->RenderText(iX,iY,pszString,iLen,color,fScale);
		m_pFonts[iStyleIndex]->EndRender();
	}

	// modify by lpf 2007-11-27
	void render::Interface::Layer2D::_DEBUG_TextOutCache(int iX,int iY, int iStyleIndex)
	{
		if (iStyleIndex >= MAX_RENDER_LAYER2D_FONT_SIZE ||
			m_pFonts[iStyleIndex] == NULL)
			return;

		m_pFonts[iStyleIndex]->_DEBUG_RenderFontTexture(iX,iY);
	}


	//带颜色码配置
	void render::Interface::Layer2D::TextOutEx(int iX,int iY,const char *pszString,int iLen, int iStyleIndex,float fScale)
	{
		static char  szStringTemp[256];
		static DWORD dwColorArray[256];
		int i = 0,n = 0; 
		DWORD dwCurColor = 0xFFFFFFFF;

		// modify by lpf 2007-11-27
		if (iStyleIndex >= MAX_RENDER_LAYER2D_FONT_SIZE ||
			m_pFonts[iStyleIndex] == NULL)
			return;

		while(i < iLen && n < 239)
		{
			unsigned char c = pszString[i++];
			if ( c == '#') //颜色码标志
			{
				if (i + 8 <= iLen)			
				{
					dwCurColor = ConverTextColor(&pszString[i]);
					i+=8;
				}
			}
			else
			{
				dwColorArray[n] = dwCurColor;
				szStringTemp[n] = c;
				n++;
			}
		}
		szStringTemp[n] = 0;

		// modify by lpf 2007-11-27
		m_pFonts[iStyleIndex]->BeginRender();
		m_pFonts[iStyleIndex]->RenderText(iX,iY,szStringTemp,dwColorArray,lstrlen(szStringTemp),fScale);
		m_pFonts[iStyleIndex]->EndRender();
	}

	DWORD render::Interface::Layer2D::ConverTextColor(const char *pszColorText)
	{
		static const DWORD szValueTable[256] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8,
			9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 10, 11, 12, 13, 14, 15,0 };

			DWORD dwColor = szValueTable[pszColorText[0]]  << 28;
			dwColor |= szValueTable[pszColorText[1]]  << 24;
			dwColor |= szValueTable[pszColorText[2]]  << 20;
			dwColor |= szValueTable[pszColorText[3]]  << 16;
			dwColor |= szValueTable[pszColorText[4]]  << 12;
			dwColor |= szValueTable[pszColorText[5]]  << 8;
			dwColor |= szValueTable[pszColorText[6]]  << 4;
			dwColor |= szValueTable[pszColorText[7]]  << 0;

			return dwColor;
	}

	// modify by lpf 2007-11-27
	int  render::Interface::Layer2D::CalcTextWidth(const char *pszTextBuffer,int iLen)
	{
		ASSERT(m_pFonts[m_tFontInfo.iStyleIndex]);
		return iLen * m_pFonts[m_tFontInfo.iStyleIndex]->GetFontSize() / 2;
	}
	render::Interface::Layer3D::Effect::Effect()
	{
		m_dwPasNum = 0;
		m_vecTechName.clear();
		m_pd3deffect = NULL;
	}
	render::Interface::Layer3D::Effect::~Effect()
	{
		m_dwPasNum = 0;
		m_vecTechName.clear();
		SAFERELEASE(m_pd3deffect);
	}
	render::Interface::Layer3D::EffectManager::EffectManager()
	{
		render::Interface* pinterface = render::Interface::GetInstance();
		m_dwNowEffectID = 0xffffffff;
		m_dwNowPassID = 0xffffffff;
		m_dwNowTechID = 0xffffffff;
		for (int i = ET_USEPOS;i < ET_MACROMAX;i++)
		{
			m_dwMacro[i] = 1<<i;
		}
		static char strswmaxmat[255];
		static char strhwmaxmat[255];
		itoa(pinterface->GetSetting()->isvpmatrixcount,strswmaxmat,10);
		m_dwd3dxMacro[ET_SWMAXMAT].Name = "SWMAXMAT";
		m_dwd3dxMacro[ET_SWMAXMAT].Definition = strswmaxmat;

		itoa(pinterface->GetSetting()->ihvpmatrixcount,strhwmaxmat,10);
		m_dwd3dxMacro[ET_HWMAXMAT].Name = "HWMAXMAT";
		m_dwd3dxMacro[ET_HWMAXMAT].Definition = strhwmaxmat;

		m_dwd3dxMacro[ET_USEPOS].Name = "USEPOS";
		m_dwd3dxMacro[ET_USEPOS].Definition = NULL;

		m_dwd3dxMacro[ET_USENORMAL].Name = "USENORMAL";
		m_dwd3dxMacro[ET_USENORMAL].Definition = NULL;

		m_dwd3dxMacro[ET_USEUV].Name = "USEUV";
		m_dwd3dxMacro[ET_USEUV].Definition = NULL;

		m_dwd3dxMacro[ET_USEMATINDEX].Name = "USEMATINDEX";
		m_dwd3dxMacro[ET_USEMATINDEX].Definition = NULL;

		m_dwd3dxMacro[ET_USECOLOR].Name = "USECOLOR";
		m_dwd3dxMacro[ET_USECOLOR].Definition = NULL;

		m_dwd3dxMacro[ET_UVANIMI].Name = "UVANIMI";
		m_dwd3dxMacro[ET_UVANIMI].Definition = NULL;

		m_dwd3dxMacro[ET_TEXTUREANIMI].Name = "TEXTUREANIMI";
		m_dwd3dxMacro[ET_TEXTUREANIMI].Definition = NULL;

		m_dwd3dxMacro[ET_ENVIROMENT].Name = "ENVIROMENT";
		m_dwd3dxMacro[ET_ENVIROMENT].Definition = NULL;

		m_dwd3dxMacro[ET_USELIGHT].Name = "USELIGHT";
		m_dwd3dxMacro[ET_USELIGHT].Definition = NULL;

		m_dwd3dxMacro[ET_ANIMUSESCAL].Name = "ANIMUSESCAL";
		m_dwd3dxMacro[ET_ANIMUSESCAL].Definition = NULL;

		m_dwd3dxMacro[ET_BILLBOARD].Name = "BILLBOARD";
		m_dwd3dxMacro[ET_BILLBOARD].Definition = NULL;

		m_dwd3dxMacro[ET_BILLBOARDY].Name = "BILLBOARDY";
		m_dwd3dxMacro[ET_BILLBOARDY].Definition = NULL;

		m_dwd3dxMacro[ET_SCAL].Name = "SCAL";
		m_dwd3dxMacro[ET_SCAL].Definition = NULL;

		m_dwd3dxMacro[ET_TEXPROJECTED].Name = "TEXPROJECTED";
		m_dwd3dxMacro[ET_TEXPROJECTED].Definition = NULL;



		m_mapd3deffect.clear();
	}
	render::Interface::Layer3D::EffectManager::~EffectManager()
	{
		Destroy();
	}
	void render::Interface::Layer3D::EffectManager::ResetRelease()
	{
		for (hash_map<DWORD,Effect*>::iterator it = m_mapd3deffect.begin();it != m_mapd3deffect.end();it++)
		{
			SAFE_DELETE((it->second));
		}

		m_mapd3deffect.clear();
	}
	void render::Interface::Layer3D::EffectManager::ResetRebuild()
	{
	}
	void render::Interface::Layer3D::EffectManager::Destroy()
	{
		ResetRelease();
		m_dwNowEffectID = 0xffffffff;
		m_dwNowPassID = 0xffffffff;
		m_dwNowTechID = 0xffffffff;
		m_mapd3deffect.clear();

	}
	DWORD render::Interface::Layer3D::EffectManager::CreateEffectFile(DWORD dwmac)
	{
		render::Interface* pinterface = render::Interface::GetInstance();
		if (pinterface->GetDeviceLost())
		{
			return 0;
		}
		hash_map<DWORD,Effect*>::iterator it = m_mapd3deffect.find(dwmac);
		if (it != m_mapd3deffect.end())
		{
			return it->first;
		}
		static char effecttxt[8000];
		ZeroMemory(effecttxt, 8000);
		DWORD texlength = 0;
		CRFile* prfile = rfOpen("effects/EFFECTS.fx");
			if (prfile)
			{
				texlength = prfile->GetDatalen();
				memcpy(effecttxt,prfile->GetData(),texlength);
			}
			rfClose(prfile);
		static D3DXMACRO macro[ET_MACROMAX];
		ZeroMemory(macro,sizeof(macro));
		for (int i = ET_USEPOS,n = 0;i < ET_MACROMAX;i++)
		{
			if ((dwmac&m_dwMacro[i]) != 0)
			{
				macro[n] = m_dwd3dxMacro[i];
				n++;
			}
		}
		Effect* peffect0 = new Effect;
		DWORD flag = D3DXSHADER_NO_PRESHADER|D3DXSHADER_PACKMATRIX_COLUMNMAJOR|D3DXSHADER_SKIPVALIDATION|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESTATE;
		LPD3DXBUFFER d3deffecterror;
		if (FAILED(D3DXCreateEffect(render::Interface::GetInstance()->GetDevice(),effecttxt,sizeof(effecttxt),macro,0,flag,0,&peffect0->m_pd3deffect,&d3deffecterror)))
		{
			char errrorstring[256];
			sprintf(errrorstring,"%s---effect file create failed!",d3deffecterror->GetBufferPointer());
			MessageBox(NULL,errrorstring,"Error",MB_OK);
			d3deffecterror->Release();
			SAFE_DELETE(peffect0);
			return 0;
		}
		string str = "T0";
		peffect0->m_vecTechName.push_back(str);
		m_mapd3deffect[dwmac] = peffect0;
		return dwmac;

	}
	void render::Interface::Layer3D::EffectManager::SetEffectandPass(DWORD dwEffectID,DWORD dwTechID,DWORD dwPass)
	{
		if (dwEffectID == 0xffffffff||dwPass == 0xffffffff)
		{
			return;
		}
		if (dwEffectID == m_dwNowEffectID)
		{
			if (dwTechID != m_dwNowTechID)
			{
				if (dwTechID < m_mapd3deffect[dwEffectID]->m_vecTechName.size())
				{
					m_mapd3deffect[dwEffectID]->m_pd3deffect->EndPass();
					m_mapd3deffect[dwEffectID]->m_pd3deffect->End();
					m_mapd3deffect[dwEffectID]->m_pd3deffect->SetTechnique(m_mapd3deffect[dwEffectID]->m_vecTechName[dwTechID].c_str());
					m_dwNowTechID = dwTechID;
					UINT passnum; 
					m_mapd3deffect[dwEffectID]->m_pd3deffect->Begin(&passnum,D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESHADERSTATE);
					m_mapd3deffect[dwEffectID]->m_dwPasNum = passnum;
					if (dwPass < m_mapd3deffect[dwEffectID]->m_dwPasNum)
					{
						m_mapd3deffect[dwEffectID]->m_pd3deffect->BeginPass(dwPass);
						m_dwNowPassID = dwPass;
					}
				}

			} 
			else
			{
				if(dwPass != m_dwNowPassID)
				{
					if (dwPass < m_mapd3deffect[dwEffectID]->m_dwPasNum)
					{
						m_mapd3deffect[dwEffectID]->m_pd3deffect->EndPass();
						m_mapd3deffect[dwEffectID]->m_pd3deffect->BeginPass(dwPass);
						m_dwNowPassID = dwPass;
					}
				}
			}
		}
		else
		{
			if (m_dwNowEffectID != 0xffffffff)
			{
				m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect->EndPass();
				m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect->End();
				m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect->SetTechnique(NULL);
			}

			if (dwTechID < (DWORD)m_mapd3deffect[dwEffectID]->m_vecTechName.size())
			{
				m_mapd3deffect[dwEffectID]->m_pd3deffect->SetTechnique(m_mapd3deffect[dwEffectID]->m_vecTechName[dwTechID].c_str());
				m_dwNowTechID = dwTechID;
				UINT passnum;
				m_mapd3deffect[dwEffectID]->m_pd3deffect->Begin(&passnum,D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESHADERSTATE);
				m_mapd3deffect[dwEffectID]->m_dwPasNum = (DWORD)passnum;
				if (dwPass < m_mapd3deffect[dwEffectID]->m_dwPasNum)
				{
					m_mapd3deffect[dwEffectID]->m_pd3deffect->BeginPass(dwPass);
					m_dwNowPassID = dwPass;
				}
			}
			m_dwNowEffectID = dwEffectID;


		}
	}
	void render::Interface::Layer3D::EffectManager::UpdateChanges()
	{
		m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect->CommitChanges();
	}
	LPD3DXEFFECT render::Interface::Layer3D::EffectManager::GetNowEffects()
	{
		if (m_dwNowEffectID != 0xffffffff)
		{
			return m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect;
		}
		else
		{
			return NULL;
		}
	}
	void render::Interface::Layer3D::EffectManager::EndEffectandPass()
	{
		if (m_dwNowPassID != 0xffffffff)
		{
			m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect->EndPass();
			m_dwNowPassID = 0xffffffff;
		}
		if (m_dwNowEffectID != 0xffffffff)
		{
			m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect->End();
			m_mapd3deffect[m_dwNowEffectID]->m_pd3deffect->SetTechnique(NULL);
			m_dwNowEffectID = 0xffffffff;
			m_dwNowTechID = 0xffffffff;
		}
		render::Interface::GetInstance()->GetDevice()->SetVertexShader(NULL);
		render::Interface::GetInstance()->GetDevice()->SetPixelShader(NULL);
	}
	render::Interface::Layer3D::RendMesh::RendMesh()
	{
		Init();
	}
	render::Interface::Layer3D::RendMesh::~RendMesh()
	{
		Release();
		DestroyMatrixArray();
	}
	void render::Interface::Layer3D::RendMesh::Init()
	{
		m_dwtexhandle = 0;
		m_dwtagmesharraycount = 0;
		m_vectagmesharrayforstaticrend.clear();
		m_vectagmesharrayforTransparent.clear();
		m_pd3dposbuffer = NULL;
		m_pd3dmatrixindexbuffer = NULL;
		m_pd3dnormalbuffer = NULL;
		m_pd3duvbuffer = NULL;
		//m_pd3ddiffusebuffer = NULL;
		m_pd3dindexbuffer = NULL;
		m_pd3dposbufferforswp = NULL;
		m_pd3dmatrixindexbufferforswp = NULL;
		m_pd3dnormalbufferforswp = NULL;
		m_pd3duvbufferforswp = NULL;
		//m_pd3ddiffusebufferfowswp = NULL;
		m_pd3dindexbufferforswp = NULL;
		m_bused = false;
		m_bstatic = true;
		m_bpushedstaicrend = false;
		m_bpushedtransparent = false;
		m_bTwoside = false;
		m_busenormal = false;
		m_bswprocess = false;
		m_bhwpcreated = false;
		m_bswpcreated = false;
		m_btorecreate = false;
		m_dwvertexcount = 0;
		m_dwindexcount = 0;
		m_dwStreamType = 0;
		m_dwusedmatrixstaticrendcount = 0;
		m_dwusedmatrixTransparentcount = 0;
		m_dwtrianglecount = 0;
		m_dwAlphaTestOrBlendMode = 0;
		m_dwColorOp = 0;
		m_dwColorArg1 = 0;
		m_dwColorArg2 = 0;
		m_dwAlphaOp = 0;
		m_dwAlphaArg1 = 0;
		m_dwAlphaArg2 = 0;

	}
	void render::Interface::Layer3D::RendMesh::SendToRendList(DWORD dwMode)
	{

		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		//添加到渲染表
		switch(dwMode)
		{
		case 0://test
			if (!m_dwAlphaTestOrBlendMode)
			{
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphatestnonelist.push_back(this);
					m_bpushedstaicrend = true;
				}
			}
			else
			{
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphatesttransparentlist.push_back(this);
					m_bpushedstaicrend = true;
				}
			}

			break;
		case 1://blend 
			switch(m_dwAlphaTestOrBlendMode)
			{
			case 0:		//blend + additive
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphablendaddtivelist.push_back(this);
					m_bpushedstaicrend = true;
				}
				break;
			case 1:		//blend + alpha
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphablendalphalist.push_back(this);
					m_bpushedstaicrend = true;
				}
				break;
			case 2:		//blend + alphalow
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphablendalphalowlist.push_back(this);
					m_bpushedstaicrend = true;
				}
				break;
			case 3:		//blend + alphalow1
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphablendalphalow1list.push_back(this);
					m_bpushedstaicrend = true;
				}
				break;
			case 4:		//blend + alphalow2
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphablendalphalow2list.push_back(this);
					m_bpushedstaicrend = true;
				}
				break;
			case 5:		//blend + alphalow3
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphablendalphalow3list.push_back(this);
					m_bpushedstaicrend = true;
				}
				break;
			case 6:		//blend + alphalow4
				if (!m_bpushedstaicrend)
				{
					pLayer3D->m_RendMeshalphablendalphalow4list.push_back(this);
					m_bpushedstaicrend = true;
				}
				break;
			default:
				break;
				//case 2:		//blend + color

				//	pLayer3D->m_shaderalphablendcolorlist.push_back(pShader);

				//	pShader->rendbuffer.btestnonepushed = true;
				//	break;
			}
			break;
		case 2://transparent
			if (!m_bpushedtransparent)
			{
				pLayer3D->m_RendMeshtransparentlist.push_back(this);
				m_bpushedtransparent = true;
			}
			break;
		default:
			break;
		}
	}
	void render::Interface::Layer3D::RendMesh::Render(DWORD mode)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		DWORD& dwusedmatrixcount = mode == 0?m_dwusedmatrixstaticrendcount:m_dwusedmatrixTransparentcount;
		std::vector<TAGMESH*>& vectagmesharray = mode == 0?m_vectagmesharrayforstaticrend:m_vectagmesharrayforTransparent;
		bool& bpushed = mode == 0?m_bpushedstaicrend:m_bpushedtransparent;
		if (!pInterface->GetDeviceLost())
		{
			LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
			render::Interface::Layer3D* player3d = pInterface->GetLayer3D();
			render::Interface::Layer3D::EffectManager* peffectmanager = player3d->GetEffectManager();
			pInterface->SetTexture(0,m_dwtexhandle);
			pInterface->Cull(!m_bTwoside);
			SetStreamType(m_dwStreamType);
			if (m_bswprocess)
			{
				pDevice->SetSoftwareVertexProcessing(TRUE);
			}

			for (int i = 0; i < dwusedmatrixcount;i++)
			{
				for (std::vector<DWORD>::iterator it = vectagmesharray[i]->vecdwPassID.begin(); it != vectagmesharray[i]->vecdwPassID.end();it++)
				{
					player3d->SetVertexDecla(vectagmesharray[i]->dwDeclID);
					peffectmanager->SetEffectandPass(vectagmesharray[i]->dwEffectID,vectagmesharray[i]->dwTechID,*it);
					if ((vectagmesharray[i]->dwEffectID&EFFECTTYPE_ENVIROMENT) != 0
						||(vectagmesharray[i]->dwEffectID&EFFECTTYPE_UVANIMI) != 0
						||(vectagmesharray[i]->dwEffectID&EFFECTTYPE_TEXTUREANIMI) != 0)
					{
						peffectmanager->GetNowEffects()->SetMatrix("MatrixForUse",&vectagmesharray[i]->matrixforuse);
					}
					if (vectagmesharray[i]->bflash)
					{
						player3d->SetColorAndAlphaOp(D3DTOP_ADD,m_dwColorArg1,m_dwColorArg2,m_dwAlphaOp,m_dwAlphaArg1,m_dwColorArg2);
					}
					else
					{
						//IBMX200集成显卡软件模拟驱动BUG的适应--彭谦
						if (m_bswprocess)
						{
							player3d->SetColorAndAlphaOp(D3DTOP_SELECTARG1,m_dwColorArg1,m_dwColorArg2,m_dwAlphaOp,m_dwAlphaArg1,m_dwColorArg2);
						}
						//IBMX200集成显卡软件模拟驱动BUG的适应--彭谦
						else
						{
							player3d->SetColorAndAlphaOp(m_dwColorOp,m_dwColorArg1,m_dwColorArg2,m_dwAlphaOp,m_dwAlphaArg1,m_dwColorArg2);
						}

					}

					if ((vectagmesharray[i]->dwEffectID&EFFECTTYPE_USELIGHT) != 0)
					{
						peffectmanager->GetNowEffects()->SetVector("LightDir",&D3DXVECTOR4(player3d->m_pD3DLight->Direction.x,player3d->m_pD3DLight->Direction.y,player3d->m_pD3DLight->Direction.z,0));
						peffectmanager->GetNowEffects()->SetVector("LightColor",&D3DXVECTOR4(player3d->m_pD3DLight->Diffuse.r,player3d->m_pD3DLight->Diffuse.g,player3d->m_pD3DLight->Diffuse.b,player3d->m_pD3DLight->Diffuse.a));
						peffectmanager->GetNowEffects()->SetVector("MultiColor",&vectagmesharray[i]->v4multicolor);
					}
					if ((vectagmesharray[i]->dwEffectID&EFFECTTYPE_USENORMAL) != 0)
					{
						if (((vectagmesharray[i]->dwEffectID&EFFECTTYPE_ANIMUSESCAL) != 0))
						{
							peffectmanager->GetNowEffects()->SetMatrixArray("WVPMatrixN",vectagmesharray[i]->ptagmesharrayfornormal,m_dwtagmesharraycount);
						}
					}
					peffectmanager->GetNowEffects()->SetVector("basecolor",&vectagmesharray[i]->v4basecolor);

					peffectmanager->GetNowEffects()->SetMatrix("VPMatrix",&vectagmesharray[i]->matrixforVP);

					peffectmanager->GetNowEffects()->SetMatrixArray("WVPMatrix",vectagmesharray[i]->ptagmesharray,m_dwtagmesharraycount);
					peffectmanager->UpdateChanges();	



					pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_dwvertexcount,0,m_dwtrianglecount);


				}
				vectagmesharray[i]->dwEffectID = 0;
				vectagmesharray[i]->dwTechID = 0;
				vectagmesharray[i]->dwDeclID = 0;
				vectagmesharray[i]->vecdwPassID.clear();
			}
			if (m_bswprocess)
			{
				pDevice->SetSoftwareVertexProcessing(FALSE);
			}

		}

		dwusedmatrixcount = 0;
		bpushed = false;

	}
	void render::Interface::Layer3D::RendMesh::Draw(D3DXVECTOR3* ppos,DWORD* pmatrixindex, D3DXVECTOR3* pnormal,D3DXVECTOR2* puv,WORD* pindex,D3DXMATRIX* pmatarray,D3DXMATRIX* pmatarrayfornormal,D3DXMATRIX* pmatforuse,D3DXMATRIX* pmatview,D3DXMATRIX* pmatproj,D3DXMATRIX* pmatfortp,D3DXVECTOR4 v4basecolor,D3DXVECTOR4 v4multicolor,bool bnoanim,bool buseenv,bool buseuv,bool blight,bool btransusescal,bool busetexture,bool btextureprojected,DWORD dwMode)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		render::Interface::Layer3D::EffectManager* peffectmanager = pLayer3D->GetEffectManager();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (GetToReCreate())
		{
			ReCreate(ppos,pmatrixindex,pnormal,puv,pindex,false);
		}
		if (dwMode == 0)
		{
			//render opaque mode : mask and none
			pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
			pDevice->SetRenderState( D3DRS_ZENABLE, TRUE);	
			pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, pLayer3D->m_eFilterType);
			pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, pLayer3D->m_eFilterType);
			pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, pLayer3D->m_eFilterType);
			pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, pLayer3D->m_eFilterType);
			if (!m_dwAlphaTestOrBlendMode)
			{
				//test + none
				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE); 						
				pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
				pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
			}
			else
			{
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
				//test + transparent
				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
				//pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
				//pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
				pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
				pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
			}

			pInterface->Cull(!m_bTwoside);
			pLayer3D->SetColorAndAlphaOp(m_dwColorOp,m_dwColorArg1,m_dwColorArg2,m_dwAlphaOp,m_dwAlphaArg1,m_dwAlphaArg2,true);
		}
		else if(dwMode == 1)
		{
			pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
			pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);	
			//设置Alpha blend mode
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

			//blend
			switch(m_dwAlphaTestOrBlendMode)
			{
			case 0://blend + additive
				{
					pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
					pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
				}
				break;
			case 1://blend + alpha
				{
					pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
					pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
				}
				break;
			case 2://blend + color
				{
					pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
					pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
				}
				break;
			case 3://blend + reserved
				{
				}
				break;
			}

			pInterface->Cull(!m_bTwoside);
			pLayer3D->SetColorAndAlphaOp(m_dwColorOp,m_dwColorArg1,m_dwColorArg2,m_dwAlphaOp,m_dwAlphaArg1,m_dwAlphaArg2,true);
		}
		DWORD dwEffectType = EFFECTTYPE_USEPOS;
		DWORD dwMacoDecla  = EFFECTTYPE_USEPOS;
		if (GetTexHandle()&&puv)
		{
			dwEffectType |= EFFECTTYPE_USEUV;
			dwMacoDecla |= EFFECTTYPE_USEUV;
		}
		if(!GetStatic()&&!bnoanim)
		{
			dwEffectType |= EFFECTTYPE_USEMATINDEX;
			dwMacoDecla |= EFFECTTYPE_USEMATINDEX;
		}
		if (buseenv)
		{
			dwEffectType |= EFFECTTYPE_USENORMAL|EFFECTTYPE_ENVIROMENT;
			dwMacoDecla |= EFFECTTYPE_USENORMAL;

		}
		else if (buseuv)
		{
			dwEffectType |= EFFECTTYPE_UVANIMI;
		}
		if (blight)
		{
			dwEffectType |= EFFECTTYPE_USENORMAL|EFFECTTYPE_USELIGHT;
			dwMacoDecla  |= EFFECTTYPE_USENORMAL;
		}
		if (btextureprojected)
		{
			dwEffectType |= EFFECTTYPE_TEXPROJECTED;
		}
		if ((dwEffectType&EFFECTTYPE_USENORMAL) != 0)
		{
			if(btransusescal)
			{
				dwEffectType |= EFFECTTYPE_ANIMUSESCAL;
			}
		}
		if (render::Interface::GetInstance()->GetSetting()->bgpuprocess)
		{
			if (render::Interface::GetInstance()->GetSetting()->bHVProcess&&!GetIsSoftWareProcessing())
			{
				if ((dwEffectType&EFFECTTYPE_ANIMUSESCAL) != 0)
				{
					if (GetJointCount() * 2 <= render::Interface::GetInstance()->GetSetting()->ihvpmatrixcount)
					{
						dwEffectType |= EFFECTTYPE_HWMAXMAT;
					}
					else
					{
						dwEffectType |= EFFECTTYPE_SWMAXMAT;
						if (!GetIsSoftWareProcessing())
						{
							if (!GetIsRsSvpCreated())
							{
								ReCreate(ppos,pmatrixindex,pnormal,puv,pindex,true);
							}
							SetSoftWareVertexprocess(true);

						}
					}
				}
				else
				{
					dwEffectType |= EFFECTTYPE_HWMAXMAT;
					SetSoftWareVertexprocess(false);
				}
			}
			else
			{
				dwEffectType |= EFFECTTYPE_SWMAXMAT;
			}
		}
		if (busetexture)
		{
			pInterface->SetTexture(0,m_dwtexhandle);
		}
		SetStreamType(m_dwStreamType);

		if (m_bswprocess)
		{
			pDevice->SetSoftwareVertexProcessing(true);
		}
		pLayer3D->SetVertexDecla(dwMacoDecla,true);
		peffectmanager->CreateEffectFile(dwEffectType);
		peffectmanager->SetEffectandPass(dwEffectType,0,0);
		if ((dwEffectType&EFFECTTYPE_ENVIROMENT) != 0
			||(dwEffectType&EFFECTTYPE_UVANIMI) != 0
			||(dwEffectType&EFFECTTYPE_TEXTUREANIMI) != 0)
		{
			peffectmanager->GetNowEffects()->SetMatrix("MatrixForUse",pmatforuse);
		}
		if ((dwEffectType&EFFECTTYPE_USELIGHT) != 0)
		{
			peffectmanager->GetNowEffects()->SetVector("LightDir",&D3DXVECTOR4(pLayer3D->m_pD3DLight->Direction.x,pLayer3D->m_pD3DLight->Direction.y,pLayer3D->m_pD3DLight->Direction.z,0));
			peffectmanager->GetNowEffects()->SetVector("LightColor",&D3DXVECTOR4(pLayer3D->m_pD3DLight->Diffuse.r,pLayer3D->m_pD3DLight->Diffuse.g,pLayer3D->m_pD3DLight->Diffuse.b,pLayer3D->m_pD3DLight->Diffuse.a));
			peffectmanager->GetNowEffects()->SetVector("MultiColor",&v4multicolor);
		}
		if ((dwEffectType&EFFECTTYPE_USENORMAL) != 0)
		{
			if (((dwEffectType&EFFECTTYPE_ANIMUSESCAL) != 0))
			{
				peffectmanager->GetNowEffects()->SetMatrixArray("WVPMatrixN",pmatarrayfornormal,m_dwtagmesharraycount);
			}

		}



		if ((dwEffectType&EFFECTTYPE_TEXPROJECTED) != 0)
		{
			peffectmanager->GetNowEffects()->SetMatrix("TPVPMatrix",pmatfortp);
		}
		peffectmanager->GetNowEffects()->SetMatrix("VPMatrix",&(*pmatview * *pmatproj));
		peffectmanager->GetNowEffects()->SetVector("basecolor",&v4basecolor);
		peffectmanager->GetNowEffects()->SetMatrixArray("WVPMatrix",pmatarray,m_dwtagmesharraycount);
		peffectmanager->UpdateChanges();

		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_dwvertexcount,0,m_dwtrianglecount);

		peffectmanager->EndEffectandPass();
		if (m_bswprocess)
		{
			pDevice->SetSoftwareVertexProcessing(false);
		}
	}
	void render::Interface::Layer3D::RendMesh::CreateMatrixArray(DWORD dwMatrixMode)
	{
		if (m_dwtagmesharraycount)
		{
			std::vector<TAGMESH*>* vectagmesharray;
			switch (dwMatrixMode)
			{
			case 0:
				vectagmesharray = &m_vectagmesharrayforstaticrend;
				break;
			case 1:
				vectagmesharray = &m_vectagmesharrayforTransparent;
				break;
			}
			TAGMESH* tagmesharray = new TAGMESH;
			tagmesharray->ptagmesharray = new D3DXMATRIX[m_dwtagmesharraycount];
			tagmesharray->ptagmesharrayfornormal = new D3DXMATRIX[m_dwtagmesharraycount];
			vectagmesharray->push_back(tagmesharray);
		}
	}
	void render::Interface::Layer3D::RendMesh::DestroyMatrixArray()
	{
		for (std::vector<TAGMESH*>::iterator it = m_vectagmesharrayforstaticrend.begin();it != m_vectagmesharrayforstaticrend.end();it++)
		{
			(*it)->Release();
			SAFEDELETE(*it);
		}
		m_vectagmesharrayforstaticrend.clear();
		for (std::vector<TAGMESH*>::iterator it = m_vectagmesharrayforTransparent.begin();it != m_vectagmesharrayforTransparent.end();it++)
		{
			(*it)->Release();
			SAFEDELETE(*it);
		}
		m_vectagmesharrayforTransparent.clear();
	}
	void render::Interface::Layer3D::RendMesh::SetStreamType(DWORD dwmaco)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		/*pDevice->SetStreamSource(0, NULL, 0, 0);
		pDevice->SetStreamSource(1, NULL, 0, 0);
		pDevice->SetStreamSource(2, NULL, 0, 0);
		pDevice->SetStreamSource(3, NULL, 0, 0);*/
		if ((dwmaco&EFFECTTYPE_USEPOS) != 0)
		{
			pDevice->SetStreamSource(0,m_bswprocess?m_pd3dposbufferforswp:m_pd3dposbuffer,0,sizeof(D3DXVECTOR3));
		}
		if (dwmaco&EFFECTTYPE_USENORMAL)
		{
			pDevice->SetStreamSource(1,m_bswprocess?m_pd3dnormalbufferforswp:m_pd3dnormalbuffer,0,sizeof(D3DXVECTOR3));
		}
		if ((dwmaco&EFFECTTYPE_USEUV) != 0)
		{
			pDevice->SetStreamSource(2,m_bswprocess?m_pd3duvbufferforswp:m_pd3duvbuffer,0,sizeof(D3DXVECTOR2));
		}
		if ((dwmaco&EFFECTTYPE_USEMATINDEX) != 0)
		{
			pDevice->SetStreamSource(3,m_bswprocess?m_pd3dmatrixindexbufferforswp:m_pd3dmatrixindexbuffer,0,sizeof(DWORD));
		}
		pDevice->SetIndices(m_bswprocess?m_pd3dindexbufferforswp:m_pd3dindexbuffer);
	}
	render::Interface::Layer3D::RendMesh::TAGMESH* render::Interface::Layer3D::RendMesh::GetTagMesh(DWORD mode)
	{
		std::vector<TAGMESH*>& vectagmesharray = mode == 0?m_vectagmesharrayforstaticrend:m_vectagmesharrayforTransparent; 
		DWORD& dwusedmatrixcount = mode == 0?m_dwusedmatrixstaticrendcount:m_dwusedmatrixTransparentcount;
		dwusedmatrixcount++;
		if (dwusedmatrixcount <= vectagmesharray.size())
		{
			return vectagmesharray[dwusedmatrixcount - 1];
		}
		else
		{
			CreateMatrixArray(mode);
			return vectagmesharray[dwusedmatrixcount - 1];
		}
	}
	void render::Interface::Layer3D::RendMesh::TAGMESH::SetMatrix(D3DXMATRIX* pmatrix,DWORD count)
	{
		memcpy(ptagmesharray,pmatrix,sizeof(D3DXMATRIX) * count);
	}
	void render::Interface::Layer3D::RendMesh::TAGMESH::SetMatrixForNormal(D3DXMATRIX* pmatrix,DWORD count)
	{
		memcpy(ptagmesharrayfornormal,pmatrix,sizeof(D3DXMATRIX) * count);
	}
	void render::Interface::Layer3D::RendMesh::TAGMESH::SetBaseColor(DWORD dwcolor)
	{
		D3DXCOLOR d3dxcolor(dwcolor);
		v4basecolor = D3DXVECTOR4(d3dxcolor.r,d3dxcolor.g,d3dxcolor.b,d3dxcolor.a);
	}
	void render::Interface::Layer3D::RendMesh::TAGMESH::SetMultiColor(DWORD dwcolor)
	{
		D3DXCOLOR d3dxcolor(dwcolor);
		v4multicolor = D3DXVECTOR4(d3dxcolor.r,d3dxcolor.g,d3dxcolor.b,1);
	}
	void render::Interface::Layer3D::RendMesh::TAGMESH::SetFlash(bool b /* = false */)
	{
		bflash = b;
	}
	bool render::Interface::Layer3D::RendMesh::Create(D3DXVECTOR3* ppos,
		DWORD* pmatrixindex,
		D3DXVECTOR3* pnormal,
		D3DXVECTOR2* puv,WORD* pindex,
		DWORD dwvertexcount,
		DWORD dwindexcount,
		DWORD dwmatcount,
		DWORD dwtexhandle,
		DWORD dwalphatorbmodel,
		DWORD dwcolorop,
		DWORD dwcolorarg1,
		DWORD dwcolorarg2,
		DWORD dwalphaop,
		DWORD dwalphaarg1,
		DWORD dwalphaarg2,
		bool bstatic,
		BOOL btwoside,
		bool busenormal,
		bool bswprocess
		)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		if (pInterface->GetDeviceLost())
		{
			return false;
		}
		if (!GetIsUsed())
		{
			Init();
			m_dwtexhandle            = dwtexhandle;
			m_bTwoside               = btwoside;
			m_dwvertexcount          = dwvertexcount;
			m_dwindexcount           = dwindexcount;
			m_dwtrianglecount		 = dwindexcount/3;
			m_dwAlphaTestOrBlendMode = dwalphatorbmodel;
			m_dwColorOp              = dwcolorop;
			m_dwColorArg1            = dwcolorarg1;
			m_dwColorArg2            = dwcolorarg2;
			m_dwAlphaOp              = dwalphaop;
			m_dwAlphaArg1            = dwalphaarg1;
			m_dwAlphaArg2            = dwalphaarg2;
			m_bstatic				 = bstatic;
			m_busenormal			 = busenormal;
			if (m_bstatic)
			{
				m_dwtagmesharraycount = 1;
			}
			else
			{
				m_dwtagmesharraycount = dwmatcount;
			}
			DWORD dwFlag = D3DUSAGE_WRITEONLY;
			D3DPOOL d3dpool =  D3DPOOL_DEFAULT;
			bool bswp = false;
			if (!pInterface->GetSetting()->bHVProcess)
			{
				dwFlag |= D3DUSAGE_DYNAMIC;
			}
			else
			{

				if (bswprocess)
				{
					dwFlag |= D3DUSAGE_DYNAMIC;
					dwFlag |= D3DUSAGE_SOFTWAREPROCESSING;
					d3dpool = D3DPOOL_SYSTEMMEM;
					bswp = true;
				}
				else
				{
					if (m_dwtagmesharraycount > pInterface->GetSetting()->ihvpmatrixcount)
					{
						if (m_dwtagmesharraycount > pInterface->GetSetting()->isvpmatrixcount)
						{
							return false;
						}
						else
						{
							dwFlag |= D3DUSAGE_DYNAMIC;
							dwFlag |= D3DUSAGE_SOFTWAREPROCESSING;
							d3dpool = D3DPOOL_SYSTEMMEM;
							bswp = true;
						}

					}
					else
					{
						bswp = false;
					}

				}

			}
			LPDIRECT3DVERTEXBUFFER9&  pd3dposbuffer         = bswp?m_pd3dposbufferforswp:m_pd3dposbuffer;
			LPDIRECT3DVERTEXBUFFER9&	 pd3dmatrixindexbuffer = bswp?m_pd3dmatrixindexbufferforswp:m_pd3dmatrixindexbuffer;
			LPDIRECT3DVERTEXBUFFER9&  pd3dnormalbuffer      = bswp?m_pd3dnormalbufferforswp:m_pd3dnormalbuffer;
			LPDIRECT3DVERTEXBUFFER9&	 pd3duvbuffer          = bswp?m_pd3duvbufferforswp:m_pd3duvbuffer;
			//LPDIRECT3DVERTEXBUFFE						
			LPDIRECT3DINDEXBUFFER9&   pd3dindexbuffer       = bswp?m_pd3dindexbufferforswp:m_pd3dindexbuffer;
			if (ppos)
			{
				if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * dwvertexcount
					, dwFlag,0,d3dpool , &pd3dposbuffer,NULL)))
				{
					OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
					return false;
					ASSERT(0);

				}
				m_dwStreamType |= EFFECTTYPE_USEPOS;
				D3DXVECTOR3* PTPOS;
				pd3dposbuffer->Lock(0,0,(void**)&PTPOS,0);
				memcpy(PTPOS,ppos,sizeof(D3DXVECTOR3) * dwvertexcount);
				pd3dposbuffer->Unlock();
			}
			if (pmatrixindex&&!bstatic)
			{
				if (FAILED(pDevice->CreateVertexBuffer(sizeof(DWORD) * dwvertexcount
					,dwFlag,0,d3dpool , &pd3dmatrixindexbuffer,NULL)))
				{
					OutputConsole("error: render::Interface:CREATE 3D MATRIXINDEX ERROR.\n");
					return false;
					ASSERT(0);

				}
				m_dwStreamType |= EFFECTTYPE_USEMATINDEX;
				DWORD* PTMATRIXINDEX;
				pd3dmatrixindexbuffer->Lock(0,0,(void**)&PTMATRIXINDEX,0);
				memcpy(PTMATRIXINDEX,pmatrixindex,sizeof(DWORD) * dwvertexcount);
				pd3dmatrixindexbuffer->Unlock();

			}



			if (pindex)
			{
				if (FAILED(pDevice->CreateIndexBuffer( sizeof(WORD) * dwindexcount
					,dwFlag,D3DFMT_INDEX16,d3dpool , &pd3dindexbuffer,NULL)))	
				{
					OutputConsole("error: render::Interface:Create 3d indexbuffer failed.\n");
					return false;
					ASSERT(0);

				}
				WORD* PTINDEX;
				pd3dindexbuffer->Lock(0,0,(void**)&PTINDEX,0);
				memcpy(PTINDEX,pindex,sizeof(WORD) * dwindexcount);
				pd3dindexbuffer->Unlock();

			}
			if (busenormal&&pnormal)
			{
				if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * dwvertexcount
					,dwFlag ,0,d3dpool, &pd3dnormalbuffer,NULL)))
				{
					OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
					return false;
					ASSERT(0);
				}
				m_dwStreamType |= EFFECTTYPE_USENORMAL;
				D3DXVECTOR3* PTNORMAL;
				pd3dnormalbuffer->Lock(0,0,(void**)&PTNORMAL,0);
				memcpy(PTNORMAL,pnormal,sizeof(D3DXVECTOR3) * dwvertexcount);
				pd3dnormalbuffer->Unlock();
			}

			if (puv&&m_dwtexhandle)
			{
				if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * dwvertexcount
					, dwFlag,0,d3dpool, &pd3duvbuffer,NULL)))	
				{
					OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
					return false;
					ASSERT(0);

				}
				m_dwStreamType |= EFFECTTYPE_USEUV;
				D3DXVECTOR2* PTUV;
				pd3duvbuffer->Lock(0,0,(void**)&PTUV,0);
				memcpy(PTUV,puv,sizeof(D3DXVECTOR2) * dwvertexcount);
				pd3duvbuffer->Unlock();
			}
			SetIsUsed(true);
			if (bswp)
			{
				m_bswpcreated = true;
				SetSoftWareVertexprocess(true);
			}
			else
			{
				m_bhwpcreated = true;
				SetSoftWareVertexprocess(false);
			}
			return true;

		}
		return false;


	}
	bool render::Interface::Layer3D::RendMesh::ReCreate(D3DXVECTOR3* ppos,DWORD* pmatrixindex,D3DXVECTOR3* pnormal,D3DXVECTOR2* puv,WORD* pindex,bool bswprocess)
	{
		if (!m_bhwpcreated&&!m_bswpcreated&&!m_btorecreate)
		{
			return false;
		}
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		if (pInterface->GetDeviceLost())
		{
			return false;
		}
		DWORD dwFlag = D3DUSAGE_WRITEONLY;
		D3DPOOL d3dpool =  D3DPOOL_DEFAULT;
		bool bswp = false;
		if (!pInterface->GetSetting()->bHVProcess)
		{
			dwFlag |= D3DUSAGE_DYNAMIC;
		}
		else
		{
			if (bswprocess)
			{
				dwFlag |= D3DUSAGE_DYNAMIC;
				dwFlag |= D3DUSAGE_SOFTWAREPROCESSING;
				d3dpool = D3DPOOL_SYSTEMMEM;
				bswp = true;
			}
			else
			{
				if (m_dwtagmesharraycount > pInterface->GetSetting()->ihvpmatrixcount)
				{
					if (m_dwtagmesharraycount > pInterface->GetSetting()->isvpmatrixcount)
					{
						return false;
					}
					else
					{
						dwFlag |= D3DUSAGE_DYNAMIC;
						dwFlag |= D3DUSAGE_SOFTWAREPROCESSING;
						d3dpool = D3DPOOL_SYSTEMMEM;
						bswp = true;
					}

				}
				else
				{
					bswp = false;
				}

			}

		}
		if (bswp)
		{
			ReleaseRsSWP();
		}
		else
		{
			ReleaseRsHWP();
		}
		LPDIRECT3DVERTEXBUFFER9&  pd3dposbuffer         = bswp?m_pd3dposbufferforswp:m_pd3dposbuffer;
		LPDIRECT3DVERTEXBUFFER9&  pd3dmatrixindexbuffer = bswp?m_pd3dmatrixindexbufferforswp:m_pd3dmatrixindexbuffer;
		LPDIRECT3DVERTEXBUFFER9&  pd3dnormalbuffer      = bswp?m_pd3dnormalbufferforswp:m_pd3dnormalbuffer;
		LPDIRECT3DVERTEXBUFFER9&  pd3duvbuffer          = bswp?m_pd3duvbufferforswp:m_pd3duvbuffer;
		//LPDIRECT3DVERTEXBUFFE						
		LPDIRECT3DINDEXBUFFER9&   pd3dindexbuffer       = bswp?m_pd3dindexbufferforswp:m_pd3dindexbuffer;

		if (ppos)
		{
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * m_dwvertexcount
				, dwFlag,0,d3dpool , &pd3dposbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
				return false;
				ASSERT(0);

			}
			D3DXVECTOR3* PTPOS;
			pd3dposbuffer->Lock(0,0,(void**)&PTPOS,0);
			memcpy(PTPOS,ppos,sizeof(D3DXVECTOR3) * m_dwvertexcount);
			pd3dposbuffer->Unlock();
		}
		if (pmatrixindex&&!m_bstatic)
		{
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(DWORD) * m_dwvertexcount
				,dwFlag,0,d3dpool , &pd3dmatrixindexbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D MATRIXINDEX ERROR.\n");
				return false;
				ASSERT(0);

			}
			DWORD* PTMATRIXINDEX;
			pd3dmatrixindexbuffer->Lock(0,0,(void**)&PTMATRIXINDEX,0);
			memcpy(PTMATRIXINDEX,pmatrixindex,sizeof(DWORD) * m_dwvertexcount);
			pd3dmatrixindexbuffer->Unlock();

		}



		if (pindex)
		{
			if (FAILED(pDevice->CreateIndexBuffer( sizeof(WORD) * m_dwindexcount
				,dwFlag,D3DFMT_INDEX16,d3dpool , &pd3dindexbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d indexbuffer failed.\n");
				return false;
				ASSERT(0);

			}
			WORD* PTINDEX;
			pd3dindexbuffer->Lock(0,0,(void**)&PTINDEX,0);
			memcpy(PTINDEX,pindex,sizeof(WORD) * m_dwindexcount);
			pd3dindexbuffer->Unlock();

		}
		if (m_busenormal&&pnormal)
		{
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * m_dwvertexcount
				,dwFlag ,0,d3dpool, &pd3dnormalbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
				return false;
				ASSERT(0);
			}
			D3DXVECTOR3* PTNORMAL;
			pd3dnormalbuffer->Lock(0,0,(void**)&PTNORMAL,0);
			memcpy(PTNORMAL,pnormal,sizeof(D3DXVECTOR3) * m_dwvertexcount);
			pd3dnormalbuffer->Unlock();
		}

		if (puv&&m_dwtexhandle)
		{
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * m_dwvertexcount
				, dwFlag,0,d3dpool, &pd3duvbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
				return false;
				ASSERT(0);

			}
			D3DXVECTOR2* PTUV;
			pd3duvbuffer->Lock(0,0,(void**)&PTUV,0);
			memcpy(PTUV,puv,sizeof(D3DXVECTOR2) * m_dwvertexcount);
			pd3duvbuffer->Unlock();
		}
		if (bswp)
		{
			m_bswpcreated = true;
		}
		else
		{
			m_bhwpcreated = true;
		}
		SetToReCreate(false);
		return true;
	}
	void render::Interface::Layer3D::RendMesh::SetSoftWareVertexprocess(bool bswp)
	{
		if (bswp)
		{
			if (m_bswpcreated)
			{
				m_bswprocess = true;
			}
		}
		else
		{
			if (m_bhwpcreated)
			{
				m_bswprocess = false;
			}
		}
	}
	void render::Interface::Layer3D::RendMesh::ReleaseRsSWP()
	{
		SAFERELEASE(m_pd3dposbufferforswp);
		SAFERELEASE(m_pd3dmatrixindexbufferforswp);
		SAFERELEASE(m_pd3duvbufferforswp);
		SAFERELEASE(m_pd3dnormalbufferforswp);
		//SAFERELEASE(m_pd3ddiffusebufferforswp);
		SAFERELEASE(m_pd3dindexbufferforswp);
		m_bswpcreated = false;

	}
	void render::Interface::Layer3D::RendMesh::ReleaseRsHWP()
	{
		SAFERELEASE(m_pd3dposbuffer);
		SAFERELEASE(m_pd3dmatrixindexbuffer);
		SAFERELEASE(m_pd3duvbuffer);
		SAFERELEASE(m_pd3dnormalbuffer);
		//SAFERELEASE(m_pd3ddiffusebuffer);
		SAFERELEASE(m_pd3dindexbuffer);
		m_bhwpcreated = false;
	}
	void render::Interface::Layer3D::RendMesh::Release()
	{
		ReleaseRsHWP();
		ReleaseRsSWP();
	}
	render::Interface::Layer3D::RendMesh::TAGMESH::TAGMESH()
	{
		ptagmesharray = NULL;
		ptagmesharrayfornormal = NULL;
		bflash = false;
		dwEffectID = 0;
		dwTechID = 0;
		dwDeclID = 0;
		v4basecolor = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f);
		D3DXMatrixIdentity(&matrixforuse);
		vecdwPassID.clear();
	};
	render::Interface::Layer3D::RendMesh::TAGMESH::~TAGMESH()
	{
		vecdwPassID.clear();
	};
	void render::Interface::Layer3D::RendMesh::TAGMESH::Release()
	{
		SAFE_DELETE_ARRAY(ptagmesharray);
		SAFE_DELETE_ARRAY(ptagmesharrayfornormal);
		vecdwPassID.clear();
	};
	render::Interface::Layer3D::RendMeshManager::RendMeshManager()
	{

	}
	render::Interface::Layer3D::RendMeshManager::~RendMeshManager()
	{
		Destroy();
	}
	void render::Interface::Layer3D::RendMeshManager::ResetRelease()
	{
		for (std::vector<RendMesh*>::iterator it = m_vecrendmesh.begin();it != m_vecrendmesh.end();it++)
		{
			(*it)->Release();
		}
	}
	void render::Interface::Layer3D::RendMeshManager::ResetRebuild()
	{
		for (std::vector<RendMesh*>::iterator it = m_vecrendmesh.begin();it != m_vecrendmesh.end();it++)
		{
			(*it)->SetToReCreate(true);
		}

	}
	void render::Interface::Layer3D::RendMeshManager::Destroy()
	{
		ResetRelease();
		for (std::vector<RendMesh*>::iterator it = m_vecrendmesh.begin();it != m_vecrendmesh.end();it++)
		{
			SAFEDELETE((*it));
		}
		m_vecrendmesh.clear();
	}
	DWORD render::Interface::Layer3D::RendMeshManager::CreateRendMesh(DWORD dwShaderGroupHandle,DWORD dwShaderHandle,DWORD dwAlphaModel,D3DXVECTOR3* ppos,DWORD* pmatrixindex,D3DXVECTOR3* pnormal,D3DXVECTOR2* puv,WORD* pindex,DWORD dwvertexcount,DWORD dwindexcount,DWORD dwmatcount,bool bstatic,bool busenormal,bool bswprocess)
	{
		render::Interface::Layer3D::ShaderGroup *pShaderGroup =render::Interface::GetInstance()->GetLayer3D()->GetShaderGroup(dwShaderGroupHandle);
		DWORD dwTestOrBlendModel = 0;
		DWORD dwtexhandle = 0;
		DWORD colorop = 0;
		DWORD colorarg1 = 0;
		DWORD colorarg2 = 0;
		DWORD alphaop = 0;
		DWORD alphaarg1 = 0;
		DWORD alphaarg2 = 0;
		BOOL  btwoside = FALSE;
		if (pShaderGroup)
		{
			render::Interface::Layer3D::ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderHandle);
			if (pShader)
			{
				dwtexhandle = pShaderGroup->GetTextureHandle(0,pShader->tLayer[0].dwTexture);
				if (dwAlphaModel == 0)
				{
					dwTestOrBlendModel = pShader->dwAlphaTestMode;
				}
				else if (dwAlphaModel == 1)
				{
					dwTestOrBlendModel = pShader->dwAlphaBlendMode;
				}
				btwoside    = pShader->bTwoSide;
				colorop		= pShader->tLayer->dwColorOp;
				colorarg1	= pShader->tLayer->dwColorArg1;
				colorarg2	= pShader->tLayer->dwColorArg2;
				alphaop		= pShader->tLayer->dwAlphaOp;
				alphaarg1	= pShader->tLayer->dwAlphaArg1;
				alphaarg2	= pShader->tLayer->dwAlphaArg2;
			}else
				return 0xffffffff;
		}
		if (m_vecrendmesh.size() < 0xffffffff)
		{
			DWORD dwindex = 0;
			for (std::vector<RendMesh*>::iterator it = m_vecrendmesh.begin();it != m_vecrendmesh.end();it++,dwindex++)
			{
				if (!(*it)->GetIsUsed())
				{
					if((*it)->Create(ppos,pmatrixindex,pnormal,puv,pindex,dwvertexcount, dwindexcount,dwmatcount,dwtexhandle,dwTestOrBlendModel,colorop,colorarg1,colorarg2,alphaop,alphaarg1,alphaarg2,bstatic,btwoside,busenormal,bswprocess))
					{
						return dwindex;
					}
					else
					{
						return 0xffffffff;
					}
				}
			}
			RendMesh* prendmesh = new RendMesh;
			if (prendmesh->Create(ppos,pmatrixindex,pnormal,puv,pindex,dwvertexcount, dwindexcount,dwmatcount,dwtexhandle,dwTestOrBlendModel,colorop,colorarg1,colorarg2,alphaop,alphaarg1,alphaarg2,bstatic,btwoside,busenormal,bswprocess))
			{
				m_vecrendmesh.push_back(prendmesh);
				return dwindex;
			}
			else
			{
				prendmesh->Release();
				SAFEDELETE(prendmesh);
				return 0xffffffff;
			}
		}
		else
		{
			return 0xffffffff;
		}
	}
	void render::Interface::Layer3D::RendMeshManager::ReleaseRendMesh(DWORD index)
	{
		if (index != 0xffffffff)
		{
			m_vecrendmesh[index]->Release();
			m_vecrendmesh[index]->DestroyMatrixArray();
			m_vecrendmesh[index]->SetIsUsed(false);
		}
	}
	render::Interface::Layer3D::RendMesh* render::Interface::Layer3D::RendMeshManager::GetRendMesh(DWORD index)
	{
		if (index >= m_vecrendmesh.size())
		{
			return NULL;
		}
		return m_vecrendmesh[index];

	}
	//___CLASS__(render::Interface::Layer3D::ShaderGroup)___
	render::Interface::Layer3D::ShaderGroup::ShaderGroup(void)
	{
		m_pdwTexHandleArray[0] = m_pdwTexHandleArray[1] = NULL;
		m_dwTexCount[0] = m_dwTexCount[1] = 0;
		m_pShaderArray = NULL;
		m_dwShaderCount = 0;
		m_pTex1Array = NULL;

	}

	render::Interface::Layer3D::ShaderGroup::~ShaderGroup(void)
	{
	}

	BOOL render::Interface::Layer3D::ShaderGroup::Create(const char * pszString, const char * pszTexPath,DWORD dwTextureProcision)
	{
		render::Interface *pInterface = render::Interface::GetInstance();

		char szTemp[_MAX_PATH];

		utility::Text script;
		script.Create(pszString);

		utility::Text::Node *pGroupNode = script.GetRootNode();
		ASSERT(pGroupNode);

		//______读取纹理结点_________
		utility::Text::Node *pTextureNode = pGroupNode->GetChildNode("texture");
		ASSERT(pTextureNode);

		//读取第一层
		utility::Text::Node *pTexLayer1Node = pTextureNode->GetChildNode("layer1");
		ASSERT(pTexLayer1Node);

		m_dwTexCount[1] = pTexLayer1Node->GetVarCount();

		if (m_dwTexCount[1] == 0)//至少有一个缺省的
		{
			return FALSE;
		}

		m_pdwTexHandleArray[1] = new DWORD[m_dwTexCount[1]];
		//读取第一层
		//读取纹理
		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			//读取纹理文件
			utility::Text::Variable *pVar = pTexLayer1Node->GetVar(n);
			ASSERT(pVar);

			_snprintf(szTemp,_MAX_PATH,"%s\\%s",pszTexPath,pVar->GetStringValue());

			//是否是空纹理？
			if (stricmp(pVar->GetStringValue(),"noTexture") == 0)
			{
				m_pdwTexHandleArray[1][n] = NULL;
			}
			else
			{
				m_pdwTexHandleArray[1][n] = pInterface->CreateTexture(szTemp,dwTextureProcision);
				ASSERT(m_pdwTexHandleArray[1][n]);
			}
		}

		//读取第零层
		utility::Text::Node *pTexLayer0Node = pTextureNode->GetChildNode("layer0");
		ASSERT(pTexLayer0Node);

		m_dwTexCount[0] = pTexLayer0Node->GetVarCount();

		if (m_dwTexCount[0] == 0)//至少有一个缺省的
		{
			return FALSE;
		}

		m_pdwTexHandleArray[0] = new DWORD[m_dwTexCount[0]];
		//读取纹理
		for(DWORD n = 0; n < m_dwTexCount[0]; n++)
		{
			//读取纹理文件
			utility::Text::Variable *pVar = pTexLayer0Node->GetVar(n);
			ASSERT(pVar);

			_snprintf(szTemp,_MAX_PATH,"%s\\%s",pszTexPath,pVar->GetStringValue());

			if (stricmp(pVar->GetStringValue(),"noTexture") == 0 )
			{
				m_pdwTexHandleArray[0][n] = NULL;
			}
			else
			{
				m_pdwTexHandleArray[0][n] = pInterface->CreateTexture(szTemp,dwTextureProcision);
				ASSERT(m_pdwTexHandleArray[0][n]);
			}
		}

		//读取shader的属性
		utility::Text::Node *pShaderNode = pGroupNode->GetChildNode("shader");

		m_dwShaderCount		= pShaderNode->GetChildCount();
		m_pShaderArray		= new _tagShader[m_dwShaderCount];
		ZeroMemory(m_pShaderArray,m_dwShaderCount * sizeof(_tagShader));
		for(DWORD n = 0; n < m_dwShaderCount; n++)
		{
			utility::Text::Node *pNode = pShaderNode->GetChildNode(n);

			_tagShader *pShader = &m_pShaderArray[n];
			/*pShader->rendbuffer.testnonebuffer.Init();
			pShader->rendbuffer.testtransparentbuffer.Init();
			pShader->rendbuffer.blendadditivebuffer.Init();
			pShader->rendbuffer.blendalphabuffer.Init();
			pShader->rendbuffer.transparentbuffer.Init();*/

			//读取名称
			strncpy(pShader->szName,pNode->GetName(),255);
			pShader->szName[255] = 0;

			utility::Text::Variable *pTempVar(NULL);

			//读取属性
			//双面
			pTempVar = pNode->GetVar("twoside");
			pShader->bTwoSide = pTempVar? pTempVar->GetBOOLValue() : FALSE;

			//pTempVar = pNode->GetVar("zwrite");
			//pShader->bZWrite  = pTempVar? pTempVar->GetBOOLValue() : FALSE;

			//alpha操作模式
			static const char *szAlphaTestMode[] = {"none","transparent"};
			pTempVar = pNode->GetVar("alphatest");
			pShader->dwAlphaTestMode = (pTempVar)? pTempVar->GetEnumValue(szAlphaTestMode,2) : 0;
			if (pShader->dwAlphaTestMode == 0xffffffff)
			{
				//MessageBox(NULL,pszTexPath,"alphatest gets a wrong type",MB_OK);
				return FALSE;
			}

			static const char *szAlphaBlendMode[] = {"additive","alpha","alphalow","alphalow1","alphalow2","alphalow3","alphalow4","color","reserved"};
			pTempVar = pNode->GetVar("alphablend");
			pShader->dwAlphaBlendMode = (pTempVar)? pTempVar->GetEnumValue(szAlphaBlendMode,9) : 0;
			if (pShader->dwAlphaBlendMode == 0xffffffff)
			{
				//MessageBox(NULL,pszTexPath,"错误的ALPHABLEND参数",MB_OK);
				pShader->dwAlphaBlendMode = 0;
			}

			//读取第0层配置
			for(int iLayer = 0; iLayer < 2; iLayer++)
			{
				sprintf(szTemp,"Layer%d",iLayer);
				utility::Text::Node *pLayerNode = pNode->GetChildNode(szTemp);
				//读取纹理
				pShader->tLayer[iLayer].dwTexture = pLayerNode->GetVar("tex")->GetIntValue();
				ASSERT(pShader->tLayer[iLayer].dwTexture < m_dwTexCount[iLayer]);
				//读取颜色运算
				static const char *szTSS[] = {"Unknown","Disable","Selectarg1","Selectarg2","Modulate",
					"Modulate2x","Modulate4x","Add","Addsigned","Addsigned2x",
					"SUbtract","Addsmooth","Blenddiffusealpha","Blendtexturealpha",
					"BLendfactoralpha","Blendtexturealphapm","Blendcurrentalpha",
					"PRemodulate","Modulatealpha_addcolor","Modulatecolor_addalpha",
					"MOdulateinvalpha_addcolor","Modulateinvcolor_addalpha",
					"BUmpenvmap","Bumpenvmapluminance","Dotproduct3","Multiplyadd","Lerp"};

				static const char *szTOP[] = {"Diffuse","Current","Texture","Tfactor","Specular","Temp"};

				pTempVar = pLayerNode->GetVar("ColorOp");
				ASSERT(pTempVar);
				DWORD dwColorOp = pTempVar->GetEnumValue(szTSS,27);
				pShader->tLayer[iLayer].dwColorOp = dwColorOp;
				ASSERT(dwColorOp != 0xffffffff);

				if (dwColorOp == 2 || dwColorOp > 3) //selectarg1
				{
					pTempVar = pLayerNode->GetVar("ColorArg1");
					pShader->tLayer[iLayer].dwColorArg1 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwColorArg1 != 0xffffffff);
				}

				if (dwColorOp > 2)
				{
					pTempVar = pLayerNode->GetVar("ColorArg2");
					pShader->tLayer[iLayer].dwColorArg2 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwColorArg2 != 0Xffffffff);
				}

				pTempVar = pLayerNode->GetVar("AlphaOp");
				ASSERT(pTempVar);
				DWORD dwAlphaOp = pTempVar->GetEnumValue(szTSS,27);
				pShader->tLayer[iLayer].dwAlphaOp = dwAlphaOp;
				ASSERT(dwAlphaOp != 0xffffffff);

				if (dwAlphaOp == 2 || dwAlphaOp > 3) //selectarg1
				{
					pTempVar = pLayerNode->GetVar("AlphaArg1");
					pShader->tLayer[iLayer].dwAlphaArg1 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwAlphaArg1 != 0xffffffff);
				}

				if (dwAlphaOp > 2)
				{
					pTempVar = pLayerNode->GetVar("AlphaArg2");
					pShader->tLayer[iLayer].dwAlphaArg2 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwAlphaArg2 != 0xffffffff);
				}
			}


			//读取顶点流格式
			static const char *szTypeOldSteam[] = {"poscol","poscoltex0"};
			pShader->dwStreamType = pNode->GetVar("stream")->GetEnumValue(szTypeOldSteam,2);
			if (pShader->dwStreamType == 0XFFFFFFFF)
			{
				//pShader->dwStreamType = LMST_POSNMLTEX0;//POSCOLTEX0
				static const char *szTypeNewSteam[] = {"poscol","posdiftex0","posdiftex0"};
				pShader->dwStreamType = pNode->GetVar("stream")->GetEnumValue(szTypeNewSteam,3);
				if (pShader->dwStreamType == 0XFFFFFFFF)
				{
					pShader->dwStreamType = LMST_POSNMLCOLTEX0;//POSCOLTEX0
				}
			}


			pShader->pTriangleAlphaTestHead = NULL;
			pShader->pTriangleAlphaBlendHead= NULL;
			//		pShader->pTraingleAlphaBlendTopHead = NULL;

		}

		//生成渲染表
		//...生成并初始化TEX1表
		m_pTex1Array = new _tagTex1[m_dwTexCount[1]];
		ZeroMemory(m_pTex1Array,sizeof(_tagTex1) * m_dwTexCount[1]);

		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			_tagTex1 *pTex1 = &m_pTex1Array[n];
			pTex1->dwTex1Handle = m_pdwTexHandleArray[1][n];
			pTex1->pTex0Head = NULL;
		}
		//根据shader生成TEX1--TEX0关系表
		for(DWORD n = 0; n < m_dwShaderCount; n++)
		{
			_tagShader *pShader = &m_pShaderArray[n];
			//1 -- 0
			//find 1 
			_tagTex1 *pTex1 = &m_pTex1Array[pShader->tLayer[1].dwTexture];
			//mapping 0 to 1 

			//查找0 -  1 映射是否存在
			_tagTex0 *pTex0 = pTex1->pTex0Head;
			while(pTex0)
			{
				if (pTex0->dwTex0Handle == m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture])
				{
					//如果找到,添加Shader到tex0结构中
					pShader->pNext = pTex0->pShaderHead;
					pTex0->pShaderHead = pShader;
					break;
				}
				pTex0 = pTex0->pNext;
			}

			if (!pTex0)
			{
				//没找到，则添加
				_tagTex0 *pTex0 = new _tagTex0;
				pTex0->dwTex0Handle = m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture];

				pShader->pNext = NULL;
				pTex0->pShaderHead = pShader;				
				pTex0->pDynamicShaderHead = NULL;

				//link tex0 to tex1
				pTex0->pNext = pTex1->pTex0Head;
				pTex1->pTex0Head = pTex0;
			}
			pShader->rendbuffer.dwtexturehandle = m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture];
			if (pShader->rendbuffer.dwtexturehandle > MAX_TEXTURE_CACHE_SIZE)
			{
				MessageBox(NULL,"Textureindex overflow!","Error",MB_OK);
			}
		}	


		script.Destroy();

		return TRUE;
	}

	BOOL render::Interface::Layer3D::ShaderGroup::Create(CRFile* pFile,DWORD dwTextureProcision)
	{
		render::Interface *pInterface = render::Interface::GetInstance();

		char szTemp[_MAX_PATH];

		utility::Text script;
		script.Create(pFile);

		utility::Text::Node *pGroupNode = script.GetRootNode();
		ASSERT(pGroupNode);

		//______读取纹理结点_________
		utility::Text::Node *pTextureNode = pGroupNode->GetChildNode("texture");
		ASSERT(pTextureNode);

		//读取第一层
		utility::Text::Node *pTexLayer1Node = pTextureNode->GetChildNode("layer1");
		ASSERT(pTexLayer1Node);

		m_dwTexCount[1] = pTexLayer1Node->GetVarCount();

		if (m_dwTexCount[1] == 0)//至少有一个缺省的
		{
			return FALSE;
		}

		m_pdwTexHandleArray[1] = new DWORD[m_dwTexCount[1]];
		//读取第一层
		//读取纹理
		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			//读取纹理文件
			utility::Text::Variable *pVar = pTexLayer1Node->GetVar(n);
			ASSERT(pVar);

			_snprintf(szTemp,_MAX_PATH,"%s",pVar->GetStringValue());

			//是否是空纹理？
			if (stricmp(pVar->GetStringValue(),"noTexture") == 0)
			{
				m_pdwTexHandleArray[1][n] = NULL;
			}
			else
			{
				m_pdwTexHandleArray[1][n] = pInterface->AcquireTexture(szTemp);
				ASSERT(m_pdwTexHandleArray[1][n]);
			}
		}

		//读取第零层
		utility::Text::Node *pTexLayer0Node = pTextureNode->GetChildNode("layer0");
		ASSERT(pTexLayer0Node);

		m_dwTexCount[0] = pTexLayer0Node->GetVarCount();

		if (m_dwTexCount[0] == 0)//至少有一个缺省的
		{
			return FALSE;
		}

		m_pdwTexHandleArray[0] = new DWORD[m_dwTexCount[0]];
		//读取纹理
		for(DWORD n = 0; n < m_dwTexCount[0]; n++)
		{
			//读取纹理文件
			utility::Text::Variable *pVar = pTexLayer0Node->GetVar(n);
			ASSERT(pVar);

			_snprintf(szTemp,_MAX_PATH,"%s",pVar->GetStringValue());

			if (stricmp(pVar->GetStringValue(),"noTexture") == 0 )
			{
				m_pdwTexHandleArray[0][n] = NULL;
			}
			else
			{
				m_pdwTexHandleArray[0][n] = pInterface->AcquireTexture(szTemp);
				ASSERT(m_pdwTexHandleArray[0][n]);
			}
		}

		//读取shader的属性
		utility::Text::Node *pShaderNode = pGroupNode->GetChildNode("shader");

		m_dwShaderCount		= pShaderNode->GetChildCount();
		m_pShaderArray		= new _tagShader[m_dwShaderCount];
		ZeroMemory(m_pShaderArray,m_dwShaderCount * sizeof(_tagShader));
		for(DWORD n = 0; n < m_dwShaderCount; n++)
		{
			utility::Text::Node *pNode = pShaderNode->GetChildNode(n);

			_tagShader *pShader = &m_pShaderArray[n];
			/*pShader->rendbuffer.testnonebuffer.Init();
			pShader->rendbuffer.testtransparentbuffer.Init();
			pShader->rendbuffer.blendadditivebuffer.Init();
			pShader->rendbuffer.blendalphabuffer.Init();
			pShader->rendbuffer.transparentbuffer.Init();*/
			//读取名称
			strncpy(pShader->szName,pNode->GetName(),255);
			pShader->szName[255] = 0;

			utility::Text::Variable *pTempVar(NULL);

			//读取属性
			//双面
			pTempVar = pNode->GetVar("twoside");
			pShader->bTwoSide = pTempVar? pTempVar->GetBOOLValue() : FALSE;

			//pTempVar = pNode->GetVar("zwrite");
			//pShader->bZWrite  = pTempVar? pTempVar->GetBOOLValue() : FALSE;

			//alpha操作模式
			static const char *szAlphaTestMode[] = {"none","transparent"};
			pTempVar = pNode->GetVar("alphatest");
			pShader->dwAlphaTestMode = (pTempVar)? pTempVar->GetEnumValue(szAlphaTestMode,2) : 0;
			if (pShader->dwAlphaTestMode == 0xffffffff)
			{
				//MessageBox(NULL,"","错误的alphatest参数",MB_OK);
				pShader->dwAlphaTestMode = 0;
			}

			static const char *szAlphaBlendMode[] = {"additive","alpha","alphalow","alphalow1","alphalow2","alphalow3","alphalow4","color","reserved"};
			pTempVar = pNode->GetVar("alphablend");
			pShader->dwAlphaBlendMode = (pTempVar)? pTempVar->GetEnumValue(szAlphaBlendMode,9) : 0;
			if (pShader->dwAlphaBlendMode == 0xffffffff)
			{
				//MessageBox(NULL,"","错误的ALPHABLEND参数",MB_OK);
				pShader->dwAlphaBlendMode = 0;
			}

			//读取第0层配置
			for(int iLayer = 0; iLayer < 2; iLayer++)
			{
				sprintf(szTemp,"Layer%d",iLayer);
				utility::Text::Node *pLayerNode = pNode->GetChildNode(szTemp);
				//读取纹理
				pShader->tLayer[iLayer].dwTexture = pLayerNode->GetVar("tex")->GetIntValue();
				ASSERT(pShader->tLayer[iLayer].dwTexture < m_dwTexCount[iLayer]);
				//读取颜色运算
				static const char *szTSS[] = {"Unknown","Disable","Selectarg1","Selectarg2","Modulate",
					"Modulate2x","Modulate4x","Add","Addsigned","Addsigned2x",
					"SUbtract","Addsmooth","Blenddiffusealpha","Blendtexturealpha",
					"BLendfactoralpha","Blendtexturealphapm","Blendcurrentalpha",
					"PRemodulate","Modulatealpha_addcolor","Modulatecolor_addalpha",
					"MOdulateinvalpha_addcolor","Modulateinvcolor_addalpha",
					"BUmpenvmap","Bumpenvmapluminance","Dotproduct3","Multiplyadd","Lerp"};

				static const char *szTOP[] = {"Diffuse","Current","Texture","Tfactor","Specular","Temp"};

				pTempVar = pLayerNode->GetVar("ColorOp");
				ASSERT(pTempVar);
				DWORD dwColorOp = pTempVar->GetEnumValue(szTSS,27);
				ASSERT(dwColorOp != 0xffffffff);
				if(dwColorOp == 0xffffffff)
				{
					dwColorOp = D3DTOP_MODULATE;
				}
				pShader->tLayer[iLayer].dwColorOp = dwColorOp;


				if (dwColorOp == 2 || dwColorOp > 3) //selectarg1
				{
					pTempVar = pLayerNode->GetVar("ColorArg1");
					pShader->tLayer[iLayer].dwColorArg1 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwColorArg1 != 0xffffffff);
				}

				if (dwColorOp > 2)
				{
					pTempVar = pLayerNode->GetVar("ColorArg2");
					pShader->tLayer[iLayer].dwColorArg2 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwColorArg2 != 0Xffffffff);
				}

				pTempVar = pLayerNode->GetVar("AlphaOp");
				ASSERT(pTempVar);
				DWORD dwAlphaOp = pTempVar->GetEnumValue(szTSS,27);
				ASSERT(dwAlphaOp != 0xffffffff);
				if(dwAlphaOp == 0xffffffff)
				{
					dwAlphaOp = D3DTOP_MODULATE;
				}
				pShader->tLayer[iLayer].dwAlphaOp = dwAlphaOp;
				if (dwAlphaOp == 2 || dwAlphaOp > 3) //selectarg1
				{
					pTempVar = pLayerNode->GetVar("AlphaArg1");
					pShader->tLayer[iLayer].dwAlphaArg1 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwAlphaArg1 != 0xffffffff);
				}

				if (dwAlphaOp > 2)
				{
					pTempVar = pLayerNode->GetVar("AlphaArg2");
					pShader->tLayer[iLayer].dwAlphaArg2 = (pTempVar) ? pTempVar->GetEnumValue(szTOP,6) : 0;
					ASSERT(pShader->tLayer[iLayer].dwAlphaArg2 != 0xffffffff);
				}
			}

			//读取顶点流格式
			static const char *szTypeOldSteam[] = {"poscol","poscoltex0"};
			pShader->dwStreamType = pNode->GetVar("stream")->GetEnumValue(szTypeOldSteam,2);
			if (pShader->dwStreamType == 0XFFFFFFFF)
			{
				//pShader->dwStreamType = LMST_POSNMLTEX0;//POSCOLTEX0
				static const char *szTypeNewSteam[] = {"poscol","posdiftex0","posdiftex0"};
				pShader->dwStreamType = pNode->GetVar("stream")->GetEnumValue(szTypeNewSteam,3);
				if (pShader->dwStreamType == 0XFFFFFFFF)
				{
					pShader->dwStreamType = LMST_POSNMLCOLTEX0;//POSCOLTEX0
				}
			}


			pShader->pTriangleAlphaTestHead = NULL;
			pShader->pTriangleAlphaBlendHead= NULL;
			//		pShader->pTraingleAlphaBlendTopHead = NULL;

		}

		//生成渲染表
		//...生成并初始化TEX1表
		m_pTex1Array = new _tagTex1[m_dwTexCount[1]];
		ZeroMemory(m_pTex1Array,sizeof(_tagTex1) * m_dwTexCount[1]);

		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			_tagTex1 *pTex1 = &m_pTex1Array[n];
			pTex1->dwTex1Handle = m_pdwTexHandleArray[1][n];
			pTex1->pTex0Head = NULL;
		}
		//根据shader生成TEX1--TEX0关系表
		for(DWORD n = 0; n < m_dwShaderCount; n++)
		{
			_tagShader *pShader = &m_pShaderArray[n];
			//1 -- 0
			//find 1 
			_tagTex1 *pTex1 = &m_pTex1Array[pShader->tLayer[1].dwTexture];
			//mapping 0 to 1 

			//查找0 -  1 映射是否存在
			_tagTex0 *pTex0 = pTex1->pTex0Head;
			while(pTex0)
			{
				if (pTex0->dwTex0Handle == m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture])
				{
					//如果找到,添加Shader到tex0结构中
					pShader->pNext = pTex0->pShaderHead;
					pTex0->pShaderHead = pShader;
					break;
				}
				pTex0 = pTex0->pNext;
			}

			if (!pTex0)
			{
				//没找到，则添加
				_tagTex0 *pTex0 = new _tagTex0;
				pTex0->dwTex0Handle = m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture];

				pShader->pNext = NULL;
				pTex0->pShaderHead = pShader;
				pTex0->pDynamicShaderHead = NULL;

				//link tex0 to tex1
				pTex0->pNext = pTex1->pTex0Head;
				pTex1->pTex0Head = pTex0;
			}
			pShader->rendbuffer.dwtexturehandle = m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture];
			if (pShader->rendbuffer.dwtexturehandle > MAX_TEXTURE_CACHE_SIZE)
			{
				MessageBox(NULL,"TextureIndex overflow!","Error",MB_OK);
			}
		}
		script.Destroy();
		return TRUE;
	}

	BOOL render::Interface::Layer3D::ShaderGroup::CreateFromFile(const char * pszFileName,DWORD dwTextureProcision)
	{
		render::Interface *pInterface = render::Interface::GetInstance();

		CRFile* pfile = rfOpen(pszFileName);
		if (!pfile)
		{
			OutputConsole("error: render::Layer3D::ShaderGroup:: File not found!.");
			MessageBox(NULL,pszFileName,"Error",MB_OK);
			return FALSE;
		}

		char path_buffer[_MAX_PATH];

		CRFile::GetFilePath(pszFileName,path_buffer);

		char *szShaderBuffer = new char[pfile->GetDatalen() + 1];

		memcpy(szShaderBuffer,pfile->GetData(),pfile->GetDatalen());
		szShaderBuffer[pfile->GetDatalen()] = '\0';
		rfClose(pfile);

		Create(szShaderBuffer,path_buffer,dwTextureProcision);

		SAFEDELETE(szShaderBuffer);


		return TRUE;
	}

	void render::Interface::Layer3D::ShaderGroup::Destroy(void)
	{
		//释放自己
		render::Interface *pInterface = render::Interface::GetInstance();

		//释放纹理渲染节点
		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			_tagTex1 *pTex1 = &m_pTex1Array[n];

			_tagTex0 *pTex0 = pTex1->pTex0Head;
			while(pTex0)
			{
				_tagTex0 *p = pTex0;
				pTex0 = pTex0->pNext;
				SAFEDELETE(p);
			}
		}

		SAFEDELETEARRAY(m_pTex1Array);

		//释放纹理

		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			//释放
			pInterface->ReleaseTexture(m_pdwTexHandleArray[1][n]);		
		}
		SAFEDELETE(m_pdwTexHandleArray[1]);

		for(DWORD n = 0; n < m_dwTexCount[0]; n++)
		{
			//释放
			pInterface->ReleaseTexture(m_pdwTexHandleArray[0][n]);		
		}
		SAFEDELETE(m_pdwTexHandleArray[0]);

		//释放shader数组
		SAFEDELETEARRAY(m_pShaderArray);
	}

	void render::Interface::Layer3D::ShaderGroup::Dump(void)
	{
		//dump info
		OutputConsole("=====================================\n");
		OutputConsole("SHADER: %d LAYER1: %d LAYER0: %d\n",m_dwShaderCount,m_dwTexCount[1],m_dwTexCount[0]);
		//dump shader mapping table
		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			_tagTex1 *pTex1 = &m_pTex1Array[n];
			OutputConsole("TEX1(%d)\n",pTex1->dwTex1Handle);
			_tagTex0 *pTex0 = pTex1->pTex0Head;
			while(pTex0)
			{
				OutputConsole("    TEX0(%d)\n",pTex0->dwTex0Handle);

				//dump shader
				_tagShader *pShader = pTex0->pShaderHead;
				while(pShader)
				{
					OutputConsole("        SHADER(\"%s\")\n",pShader->szName);

					pShader = pShader->pNext;
				}

				pTex0 = pTex0->pNext;
			}
		}

		OutputConsole("=====================================\n");
	}
	void render::Interface::Layer3D::ShaderGroup::ReSet(void)
	{
		vector<_tagShader::RendBuffer::_tagBuffer>::iterator  it;

		for (int i = 0; i < GetShaderCount();i++)
		{
			_tagShader* pshader = GetShader(i);
			//alphatestnone缓冲重置
			pshader->rendbuffer.testnonebuffer.Release();

			//alphatesttransparent缓冲重置
			pshader->rendbuffer.testtransparentbuffer.Release();

			//blendadditive缓冲重置
			pshader->rendbuffer.blendadditivebuffer.Release();

			//blendalpha缓冲重置
			pshader->rendbuffer.blendalphabuffer.Release();

			pshader->rendbuffer.blendalphalowbuffer.Release();

			pshader->rendbuffer.blendalphalow1buffer.Release();

			pshader->rendbuffer.blendalphalow2buffer.Release();

			pshader->rendbuffer.blendalphalow3buffer.Release();

			pshader->rendbuffer.blendalphalow4buffer.Release();

			//transparent缓冲重置
			pshader->rendbuffer.transparentbuffer.Release();


		}
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::_tagShader()
	{

	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::~_tagShader()
	{
		vector<RendBuffer::_tagBuffer>::iterator it;
		//alphatestnone缓冲重置
		rendbuffer.testnonebuffer.Release();
		//alphatesttransparent缓冲重置
		rendbuffer.testtransparentbuffer.Release();
		//blendadditive缓冲重置
		rendbuffer.blendadditivebuffer.Release();
		//blendalpha缓冲重置
		rendbuffer.blendalphabuffer.Release();
		rendbuffer.blendalphalowbuffer.Release();
		rendbuffer.blendalphalow1buffer.Release();
		rendbuffer.blendalphalow2buffer.Release();
		rendbuffer.blendalphalow3buffer.Release();
		rendbuffer.blendalphalow4buffer.Release();
		//testtransparentbuffer缓冲重置
		rendbuffer.transparentbuffer.Release();

	}

	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::_tagBuffer()
	{

		pd3dposbuffer = NULL;
		pd3duvbuffer = NULL;
		pd3dnormalbuffer = NULL;
		pd3ddiffusebuffer = NULL;
		pd3dindexbuffer = NULL;
		ppos = NULL;
		pdiffuse = NULL;
		pnormal = NULL;
		puv = NULL;
		pindex = NULL;

		trianglecount = 0;
		indexcount = 0;
		vertexcount = 0;
		bposlocked = false;
		buvlocked = false;
		bdiffuselocked = false;
		bindexlocked = false;
		bnormallocked = false;

		dwTempColorOp = 0;
		dwTempColorArg1 = 0;
		dwTempColorArg2 = 0;
		dwTempAlphaOp = 0;
		dwTempAlphaArg1 = 0;
		dwTempAlphaArg2 = 0;

		vertexsize = 0;
		indexsize = 0;
		D3DXMatrixIdentity(&matworld);

	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::~_tagBuffer()
	{
		ppos = NULL;
		pnormal = NULL;
		puv = NULL;
		pindex = NULL;

	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::Release()
	{
		SAFERELEASE(pd3dposbuffer);
		SAFERELEASE(pd3duvbuffer);
		SAFERELEASE(pd3dnormalbuffer);
		SAFERELEASE(pd3ddiffusebuffer);
		SAFERELEASE(pd3dindexbuffer);
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::BufferList::BufferList()
	{
		Clear();
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::BufferList::~BufferList()
	{
	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::BufferList::Init()
	{
		/*_tagShader::RendBuffer::_tagBuffer buffer;
		rendbuffer.push_back(buffer);
		rendbuffer[0].CreateBuffer();
		dynamicrendbuffer.push_back(buffer);
		dynamicrendbuffer[0].CreateBuffer();
		lightrendbuffer.push_back(buffer);
		lightrendbuffer[0].CreateBuffer();
		lightdynamicrendbuffer.push_back(buffer);
		lightdynamicrendbuffer[0].CreateBuffer();
		Clear();*/


	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::BufferList::Release()
	{
		vector<_tagShader::RendBuffer::_tagBuffer>::iterator  it;
		for (it = rendbuffer.begin();it != rendbuffer.end();it++)
		{
			(*it).Release();
		}
		rendbuffer.clear();
		for (it = dynamicrendbuffer.begin();it != dynamicrendbuffer.end();it++)
		{
			(*it).Release();
		}
		dynamicrendbuffer.clear();
		for (it = lightrendbuffer.begin();it != lightrendbuffer.end();it++)
		{
			(*it).Release();
		}
		lightrendbuffer.clear();
		for (it = lightdynamicrendbuffer.begin();it != lightdynamicrendbuffer.end();it++)
		{
			(*it).Release();
		}
		lightdynamicrendbuffer.clear();
		Clear();


	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::BufferList::Clear()
	{
		bufferusenum = 0;
		dynamicbufferusenum = 0;
		lightbufferusenum = 0;
		lightdynamicbufferusenum = 0;
		bpushed = false;

	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::RendBuffer()
	{

		dwtexturehandle = 0;
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::~RendBuffer()
	{
		testnonebuffer.Release();
		testtransparentbuffer.Release();
		blendadditivebuffer.Release();
		blendalphabuffer.Release();
		blendalphalowbuffer.Release();
		blendalphalow1buffer.Release();
		blendalphalow2buffer.Release();
		blendalphalow3buffer.Release();
		blendalphalow4buffer.Release();
		transparentbuffer.Release();
	}
	/*void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::SetMaterial(D3DMATERIAL9* m)
	{
	pmtrl = m;
	}*/
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::Clear()
	{
		trianglecount = 0;
		indexcount = 0;
		vertexcount = 0;
		bposlocked = false;
		buvlocked = false;
		bdiffuselocked = false;
		bindexlocked = false;
		bnormallocked = false;

	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::Clear(DWORD rendtype)
	{
		switch(rendtype)
		{
		case RENDTYPE_TESTNONE:
			testnonebuffer.Clear();
			break;
		case RENDTYPE_TESTTRANSPARENT:
			testtransparentbuffer.Clear();
			break;
		case RENDTYPE_BLENDADDITIVE:
			blendadditivebuffer.Clear();
			break;
		case RENDTYPE_BLENDALPHA:
			blendalphabuffer.Clear();
			break;
		case RENDTYPE_BLENDALPHALOW:
			blendalphalowbuffer.Clear();
			break;
		case RENDTYPE_BLENDALPHALOW1:
			blendalphalow1buffer.Clear();
			break;
		case RENDTYPE_BLENDALPHALOW2:
			blendalphalow2buffer.Clear();
			break;
		case RENDTYPE_BLENDALPHALOW3:
			blendalphalow3buffer.Clear();
			break;
		case RENDTYPE_BLENDALPHALOW4:
			blendalphalow4buffer.Clear();
			break;
		case RENDTYPE_TRANSPARENT:
			transparentbuffer.Clear();
			break;
		default:
			break;
		}



	}
	bool render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::AddDynamicTagBuffer(DWORD dwCount,DWORD dwStreamType,DWORD rendtype)
	{	
		int* pusedynamicnum = NULL; 
		vector<_tagBuffer>* pbuffer = NULL;
		switch(rendtype)
		{
		case RENDTYPE_TESTNONE:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &testnonebuffer.lightdynamicbufferusenum;
				pbuffer = &testnonebuffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &testnonebuffer.dynamicbufferusenum;
				pbuffer = &testnonebuffer.dynamicrendbuffer;
			}

			break;
		case RENDTYPE_TESTTRANSPARENT:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &testtransparentbuffer.lightdynamicbufferusenum;
				pbuffer = &testtransparentbuffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &testtransparentbuffer.dynamicbufferusenum;
				pbuffer = &testtransparentbuffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_BLENDADDITIVE:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &blendadditivebuffer.lightdynamicbufferusenum;
				pbuffer = &blendadditivebuffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &blendadditivebuffer.dynamicbufferusenum;
				pbuffer = &blendadditivebuffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHA:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &blendalphabuffer.lightdynamicbufferusenum;
				pbuffer = &blendalphabuffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &blendalphabuffer.dynamicbufferusenum;
				pbuffer = &blendalphabuffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &blendalphalowbuffer.lightdynamicbufferusenum;
				pbuffer = &blendalphalowbuffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &blendalphalowbuffer.dynamicbufferusenum;
				pbuffer = &blendalphalowbuffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW1:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &blendalphalow1buffer.lightdynamicbufferusenum;
				pbuffer = &blendalphalow1buffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &blendalphalow1buffer.dynamicbufferusenum;
				pbuffer = &blendalphalow1buffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW2:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &blendalphalow2buffer.lightdynamicbufferusenum;
				pbuffer = &blendalphalow2buffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &blendalphalow2buffer.dynamicbufferusenum;
				pbuffer = &blendalphalow2buffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW3:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &blendalphalow3buffer.lightdynamicbufferusenum;
				pbuffer = &blendalphalow3buffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &blendalphalow3buffer.dynamicbufferusenum;
				pbuffer = &blendalphalow3buffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW4:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &blendalphalow4buffer.lightdynamicbufferusenum;
				pbuffer = &blendalphalow4buffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &blendalphalow4buffer.dynamicbufferusenum;
				pbuffer = &blendalphalow4buffer.dynamicrendbuffer;
			}
			break;
		case RENDTYPE_TRANSPARENT:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusedynamicnum = &transparentbuffer.lightdynamicbufferusenum;
				pbuffer = &transparentbuffer.lightdynamicrendbuffer;
			}
			else
			{
				pusedynamicnum = &transparentbuffer.dynamicbufferusenum;
				pbuffer = &transparentbuffer.dynamicrendbuffer;
			}
			break;
		default:
			pbuffer = NULL;
			break;
		}
		if (pbuffer == NULL)
		{
			return false;
		}
		vector<_tagBuffer>& buffer = *pbuffer;
		int& usedynamicnum = *pusedynamicnum;
		if (usedynamicnum + 1> buffer.size())
		{
			_tagBuffer tagbuffer;
			buffer.push_back(tagbuffer);
			if(!buffer[usedynamicnum].CreateBuffer())
			{
				return false;
			}
			buffer[usedynamicnum].Clear();
		}
		if (buffer[usedynamicnum].vertexcount + dwCount > buffer[usedynamicnum].vertexsize)
		{
			if(!buffer[usedynamicnum].ExtendVertexBuffer(buffer[usedynamicnum].vertexcount + dwCount,dwStreamType))
			{
				MessageBox(NULL,"Extend index buffer faild!","faild!",MB_OK);
			}
		}
		return true;
	}
	bool render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::AddTagBuffer(DWORD dwCount,DWORD dwStreamType,DWORD rendtype)
	{

		int* pusenum = NULL;
		vector<_tagBuffer>* pbuffer = NULL;
		switch(rendtype)
		{
		case RENDTYPE_TESTNONE:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &testnonebuffer.lightbufferusenum;
				pbuffer = &testnonebuffer.lightrendbuffer;
			}
			else
			{
				pusenum = &testnonebuffer.bufferusenum;
				pbuffer = &testnonebuffer.rendbuffer;
			}
			break;
		case RENDTYPE_TESTTRANSPARENT:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &testtransparentbuffer.lightbufferusenum;
				pbuffer = &testtransparentbuffer.lightrendbuffer;
			}
			else
			{
				pusenum = &testtransparentbuffer.bufferusenum;
				pbuffer = &testtransparentbuffer.rendbuffer;
			}
			break;
		case RENDTYPE_BLENDADDITIVE:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &blendadditivebuffer.lightbufferusenum;
				pbuffer = &blendadditivebuffer.lightrendbuffer;
			}
			else
			{
				pusenum = &blendadditivebuffer.bufferusenum;
				pbuffer = &blendadditivebuffer.rendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHA:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &blendalphabuffer.lightbufferusenum;
				pbuffer = &blendalphabuffer.lightrendbuffer;
			}
			else
			{
				pusenum = &blendalphabuffer.bufferusenum;
				pbuffer = &blendalphabuffer.rendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &blendalphalowbuffer.lightbufferusenum;
				pbuffer = &blendalphalowbuffer.lightrendbuffer;
			}
			else
			{
				pusenum = &blendalphalowbuffer.bufferusenum;
				pbuffer = &blendalphalowbuffer.rendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW1:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &blendalphalow1buffer.lightbufferusenum;
				pbuffer = &blendalphalow1buffer.lightrendbuffer;
			}
			else
			{
				pusenum = &blendalphalow1buffer.bufferusenum;
				pbuffer = &blendalphalow1buffer.rendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW2:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &blendalphalow2buffer.lightbufferusenum;
				pbuffer = &blendalphalow2buffer.lightrendbuffer;
			}
			else
			{
				pusenum = &blendalphalow2buffer.bufferusenum;
				pbuffer = &blendalphalow2buffer.rendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW3:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &blendalphalow3buffer.lightbufferusenum;
				pbuffer = &blendalphalow3buffer.lightrendbuffer;
			}
			else
			{
				pusenum = &blendalphalow3buffer.bufferusenum;
				pbuffer = &blendalphalow3buffer.rendbuffer;
			}
			break;
		case RENDTYPE_BLENDALPHALOW4:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &blendalphalow4buffer.lightbufferusenum;
				pbuffer = &blendalphalow4buffer.lightrendbuffer;
			}
			else
			{
				pusenum = &blendalphalow4buffer.bufferusenum;
				pbuffer = &blendalphalow4buffer.rendbuffer;
			}
			break;
		case RENDTYPE_TRANSPARENT:
			if (dwStreamType == LMST_POSNMLCOLTEX0)
			{
				pusenum = &transparentbuffer.lightbufferusenum;
				pbuffer = &transparentbuffer.lightrendbuffer;
			}
			else
			{
				pusenum = &transparentbuffer.bufferusenum;
				pbuffer = &transparentbuffer.rendbuffer;
			}
			break;
		default:
			pbuffer = NULL;
			break;
		}
		if (pbuffer == NULL)
		{
			return false;
		}
		vector<_tagBuffer>& buffer = *pbuffer;
		int& usenum = *pusenum;
		usenum++;
		if (usenum + 1 > buffer.size())
		{
			_tagBuffer tagbuffer;
			buffer.push_back(tagbuffer);
			if(!buffer[usenum].CreateBuffer())
			{
				usenum--;
				return false;
			} 
		}
		if (buffer[usenum].vertexcount + dwCount > buffer[usenum].vertexsize)
		{
			buffer[usenum].ExtendVertexBuffer(buffer[usenum].vertexcount + dwCount,dwStreamType);
		}
		return true;
	}
	bool render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::CreateBuffer()
	{
		vertexsize = LAYER3D_RENDVERTEXBUFFER_SIZE;
		indexsize =  LAYER3D_RENDINDEXBUFFER_SIZE;
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		UnLockBuffer(BUFFERTYPE_POS);
		UnLockBuffer(BUFFERTYPE_DIFFUSE);
		UnLockBuffer(BUFFERTYPE_UV);
		UnLockBuffer(BUFFERTYPE_INDEX);
		UnLockBuffer(BUFFERTYPE_NORMAL);
		SAFERELEASE(pd3dposbuffer);
		SAFERELEASE(pd3duvbuffer);
		SAFERELEASE(pd3dnormalbuffer);
		SAFERELEASE(pd3ddiffusebuffer);
		SAFERELEASE(pd3dindexbuffer);
		//流缓冲
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3dposbuffer,NULL)))
		{
			OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
			return false;
			ASSERT(0);

		}
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3dnormalbuffer,NULL)))
		{
			OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
			return false;
			ASSERT(0);

		}
		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DCOLOR) * vertexsize
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3ddiffusebuffer,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d color vextexbuffer failed.\n");
			return false;
			ASSERT(0);

		}		
		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * vertexsize
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3duvbuffer,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
			return false;
			ASSERT(0);

		}
		if (FAILED(pDevice->CreateIndexBuffer( sizeof(WORD) * indexsize
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,D3DFMT_INDEX16,D3DPOOL_SYSTEMMEM , &pd3dindexbuffer,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d indexbuffer failed.\n");
			return false;
			ASSERT(0);

		}
		return true;

	}
	bool render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::ExtendVertexBuffer(DWORD num,DWORD streamtype)
	{

		static D3DXVECTOR3 temppos[LAYER3D_VERTEXBUFFER_SIZE];
		static D3DXVECTOR3 tempnormal[LAYER3D_VERTEXBUFFER_SIZE];
		static D3DXVECTOR2 tempuv[LAYER3D_VERTEXBUFFER_SIZE];
		static D3DCOLOR temppdiffuse[LAYER3D_VERTEXBUFFER_SIZE];

		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		if (vertexcount > LAYER3D_VERTEXBUFFER_SIZE)
		{
			return false;
		}
		while (vertexsize < num)
		{
			vertexsize = vertexsize + LAYER3D_RENDVERTEXBUFFER_SIZE; 
		}
		switch(streamtype)
		{
		case LMST_POSNML:
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_NORMAL);
			if (ppos&&pnormal)
			{
				memcpy(temppos,ppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(tempnormal,pnormal,vertexcount * sizeof(D3DXVECTOR3));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			SAFERELEASE(pd3dposbuffer);
			SAFERELEASE(pd3dnormalbuffer);

			//流缓冲
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
				,  D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3dposbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
				,  D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3dnormalbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D Normal VEXTEXBUFFER ERROR.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D Normal VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}	
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_NORMAL);
			if (ppos&&pnormal)
			{
				ZeroMemory(ppos,vertexsize * sizeof(D3DXVECTOR3));
				ZeroMemory(pnormal,vertexsize * sizeof(D3DXVECTOR3));
				memcpy(ppos,temppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(pnormal,tempnormal,vertexcount * sizeof(D3DXVECTOR3));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			break;
		case LMST_POSNMLTEX0:
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			UnLockBuffer(BUFFERTYPE_UV);
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_NORMAL);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&pnormal&&puv)
			{
				memcpy(temppos,ppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(tempnormal,pnormal,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(tempuv,puv,vertexcount * sizeof(D3DXVECTOR2));

			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			UnLockBuffer(BUFFERTYPE_UV);
			SAFERELEASE(pd3dposbuffer);
			SAFERELEASE(pd3dnormalbuffer);
			SAFERELEASE(pd3duvbuffer);
			//流缓冲
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
				, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3dposbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR3) * vertexsize
				, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3dnormalbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d NORMAL vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D NORMAL VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}	
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * vertexsize
				, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3duvbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D tex0 VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_NORMAL);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&pnormal&&puv)
			{
				ZeroMemory(ppos,vertexsize * sizeof(D3DXVECTOR3));
				ZeroMemory(pnormal,vertexsize * sizeof(D3DXVECTOR3));
				ZeroMemory(puv,vertexsize * sizeof(D3DXVECTOR2));
				memcpy(ppos,temppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(pnormal,tempnormal,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(puv,tempuv,vertexcount * sizeof(D3DXVECTOR2));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			UnLockBuffer(BUFFERTYPE_UV);
			break;
		case LMST_POSCOLTEX0:
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_DIFFUSE);
			UnLockBuffer(BUFFERTYPE_UV);
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_DIFFUSE);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&pdiffuse&&puv)
			{
				memcpy(temppos,ppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(temppdiffuse,pdiffuse,vertexcount * sizeof(D3DCOLOR));
				memcpy(tempuv,puv,vertexcount * sizeof(D3DXVECTOR2));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_DIFFUSE);
			UnLockBuffer(BUFFERTYPE_UV);
			SAFERELEASE(pd3dposbuffer);
			SAFERELEASE(pd3ddiffusebuffer);
			SAFERELEASE(pd3duvbuffer);
			//流缓冲
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
				,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3dposbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DCOLOR) * vertexsize
				,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3ddiffusebuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d color vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D color VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}	
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * vertexsize
				,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3duvbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D tex0 VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_DIFFUSE);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&pdiffuse&&puv)
			{
				ZeroMemory(ppos,vertexsize * sizeof(D3DXVECTOR3));
				ZeroMemory(pdiffuse,vertexsize * sizeof(D3DCOLOR));
				ZeroMemory(puv,vertexsize * sizeof(D3DXVECTOR2));
				memcpy(ppos,temppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(pdiffuse,temppdiffuse,vertexcount * sizeof(D3DCOLOR));
				memcpy(puv,tempuv,vertexcount * sizeof(D3DXVECTOR2));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_DIFFUSE);
			UnLockBuffer(BUFFERTYPE_UV);
			break;
		case LMST_POSNMLCOLTEX0:
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			UnLockBuffer(BUFFERTYPE_DIFFUSE);
			UnLockBuffer(BUFFERTYPE_UV);
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_NORMAL);
			LockBuffer(BUFFERTYPE_DIFFUSE);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&pdiffuse&&puv&&pnormal)
			{
				memcpy(temppos,ppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(tempnormal,pnormal,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(temppdiffuse,pdiffuse,vertexcount * sizeof(D3DCOLOR));
				memcpy(tempuv,puv,vertexcount * sizeof(D3DXVECTOR2));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			UnLockBuffer(BUFFERTYPE_DIFFUSE);
			UnLockBuffer(BUFFERTYPE_UV);
			SAFERELEASE(pd3dposbuffer);
			SAFERELEASE(pd3dnormalbuffer);
			SAFERELEASE(pd3ddiffusebuffer);
			SAFERELEASE(pd3duvbuffer);
			//流缓冲
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
				, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3dposbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR3) * vertexsize
				, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3dnormalbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d pd3dnormalbuffer vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D pd3dnormalbuffer VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}	
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DCOLOR) * vertexsize
				,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3ddiffusebuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d color vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D color VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}	
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * vertexsize
				,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3duvbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D tex0 VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_NORMAL);
			LockBuffer(BUFFERTYPE_DIFFUSE);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&pdiffuse&&puv&&pnormal)
			{
				ZeroMemory(ppos,vertexsize * sizeof(D3DXVECTOR3));
				ZeroMemory(pnormal,vertexsize * sizeof(D3DXVECTOR3));
				ZeroMemory(pdiffuse,vertexsize * sizeof(D3DCOLOR));
				ZeroMemory(puv,vertexsize * sizeof(D3DXVECTOR2));
				memcpy(ppos,temppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(pnormal,tempnormal,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(pdiffuse,temppdiffuse,vertexcount * sizeof(D3DCOLOR));
				memcpy(puv,tempuv,vertexcount * sizeof(D3DXVECTOR2));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_NORMAL);
			UnLockBuffer(BUFFERTYPE_DIFFUSE);
			UnLockBuffer(BUFFERTYPE_UV);
			break;
		case LMST_POSTEX0:
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_UV);
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&puv)
			{
				memcpy(temppos,ppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(tempuv,puv,vertexcount * sizeof(D3DXVECTOR2));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_UV);
			SAFERELEASE(pd3dposbuffer);
			SAFERELEASE(pd3duvbuffer);
			//流缓冲
			if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * vertexsize
				,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM, &pd3dposbuffer,NULL)))
			{
				OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * vertexsize
				,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &pd3duvbuffer,NULL)))	
			{
				OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
				MessageBox(NULL,"error: render::Interface:CREATE 3D tex0 VEXTEXBUFFER ERROR.\n","Error",MB_OK);
				ASSERT(0);
			}
			LockBuffer(BUFFERTYPE_POS);
			LockBuffer(BUFFERTYPE_UV);
			if (ppos&&puv)
			{
				ZeroMemory(ppos,vertexsize * sizeof(D3DXVECTOR3));
				ZeroMemory(puv,vertexsize * sizeof(D3DXVECTOR2));
				memcpy(ppos,temppos,vertexcount * sizeof(D3DXVECTOR3));
				memcpy(puv,tempuv,vertexcount * sizeof(D3DXVECTOR2));
			}
			UnLockBuffer(BUFFERTYPE_POS);
			UnLockBuffer(BUFFERTYPE_UV);
			if (vertexcount > vertexsize)
			{
				MessageBox(NULL,"Vertex Buffer OverFlow!","Error",NULL);
			}
			break;
		}
		return true;
	}
	bool render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::ExtendIndexBuffer(DWORD num)
	{
		static WORD temppindex[LAYER3D_INDEXBUFFER_SIZE];
		if (indexcount > LAYER3D_INDEXBUFFER_SIZE)
		{
			MessageBox(NULL,"BUFFER too Large","ExtendIndexBuffer faild",NULL);
			return false;
		}

		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		UnLockBuffer(BUFFERTYPE_INDEX);
		LockBuffer(BUFFERTYPE_INDEX);
		if (pindex)
		{
			memcpy(temppindex,pindex,indexcount * sizeof(WORD));
		}
		UnLockBuffer(BUFFERTYPE_INDEX);
		SAFERELEASE(pd3dindexbuffer);
		while (indexsize < num)
		{
			indexsize = indexsize + LAYER3D_RENDINDEXBUFFER_SIZE; 
		}
		if (indexsize < indexcount)
		{
			MessageBox(NULL,"error: render::Interface:CREATE 3D indexbuffer  ERROR.\n","Faild",MB_OK);
		}
		if (FAILED(pDevice->CreateIndexBuffer( sizeof(WORD) * indexsize
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC ,D3DFMT_INDEX16,D3DPOOL_SYSTEMMEM , &pd3dindexbuffer,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d indexbuffer failed.\n");
			MessageBox(NULL,"error: render::Interface:CREATE 3D indexbuffer  ERROR.\n","Faild",MB_OK);
			ASSERT(0);

		}
		LockBuffer(BUFFERTYPE_INDEX);
		if (pindex)
		{
			ZeroMemory(pindex,indexsize * sizeof(WORD));
			memcpy(pindex,temppindex,indexcount * sizeof(WORD));
		}
		UnLockBuffer(BUFFERTYPE_INDEX);
		if (indexcount > indexsize)
		{
			MessageBox(NULL,"Index Buffer OverFlow!","Faild",NULL);
		}
		return true;

	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::LockBuffer(DWORD dwbuffertype)
	{
		switch(dwbuffertype)
		{
		case BUFFERTYPE_POS:
			if (!bposlocked&&pd3dposbuffer)
			{
				if (FAILED(pd3dposbuffer->Lock(0,0,(void**)&ppos,D3DLOCK_DISCARD)))
				{
					ppos = NULL;
					pd3dposbuffer->Unlock();
					MessageBox(NULL,"LockPosBuffer Faild","Faild",MB_OK);
					bposlocked = false;
				}
				bposlocked = true;
			}
			break;
		case BUFFERTYPE_UV:
			if (!buvlocked&&pd3duvbuffer)
			{
				if (FAILED(pd3duvbuffer->Lock(0,0,(void**)&puv,D3DLOCK_DISCARD)))
				{
					puv = NULL;
					pd3duvbuffer->Unlock();
					MessageBox(NULL,"LockUVBuffer Faild","Faild",MB_OK);
					buvlocked = false;
				}
				buvlocked = true;
			}
			break;
		case BUFFERTYPE_DIFFUSE:
			if (!bdiffuselocked&&pd3ddiffusebuffer)
			{
				if (FAILED(pd3ddiffusebuffer->Lock(0,0,(void**)&pdiffuse,D3DLOCK_DISCARD)))
				{
					pdiffuse = NULL;
					pd3ddiffusebuffer->Unlock();
					MessageBox(NULL,"LockDiffuseBuffer Faild","Faild",MB_OK);
					bdiffuselocked = false;
				}
				bdiffuselocked = true;
			}
			break;
		case BUFFERTYPE_NORMAL:
			if (!bnormallocked&&pd3dnormalbuffer)
			{
				if (FAILED(pd3dnormalbuffer->Lock(0,0,(void**)&pnormal,D3DLOCK_DISCARD)))
				{
					pnormal = NULL;
					pd3dnormalbuffer->Unlock();
					MessageBox(NULL,"LockNormalBuffer Faild","Faild",MB_OK);
					bnormallocked = false;
				}
				bnormallocked = true;
			}
			break;
		case BUFFERTYPE_INDEX:
			if (!bindexlocked&&pd3dindexbuffer)
			{
				if (FAILED(pd3dindexbuffer->Lock(0,0,(void**)&pindex,D3DLOCK_DISCARD)))
				{
					pindex = NULL;
					pd3dindexbuffer->Unlock();
					MessageBox(NULL,"LockIndexBuffer Faild","Faild",MB_OK);
					bindexlocked = false;
				}
				bindexlocked = true;
			}
			break;
		}

	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer::UnLockBuffer(DWORD dwbuffertype)
	{
		switch(dwbuffertype)
		{
		case BUFFERTYPE_POS:
			if (bposlocked&&pd3dposbuffer)
			{
				pd3dposbuffer->Unlock();
				ppos = NULL;
				bposlocked = false;
			}
			break;
		case BUFFERTYPE_UV:
			if (buvlocked&&pd3duvbuffer)
			{
				pd3duvbuffer->Unlock();
				puv = NULL;
				buvlocked = false;
			}
			break;
		case BUFFERTYPE_DIFFUSE:
			if (bdiffuselocked&&pd3ddiffusebuffer)
			{
				pd3ddiffusebuffer->Unlock();
				pdiffuse = NULL;
				bdiffuselocked = false;
			}
			break;
		case BUFFERTYPE_NORMAL:
			if (bnormallocked&&pd3dnormalbuffer)
			{
				pd3dnormalbuffer->Unlock();
				pnormal = NULL;
				bnormallocked = false;
			}
			break;
		case BUFFERTYPE_INDEX:
			if (bindexlocked&&pd3dindexbuffer)
			{
				pd3dindexbuffer->Unlock();
				pindex = NULL;
				bindexlocked = false;
			}
			break;
		}

	}
	void render::Interface::Layer3D::ShaderGroup::_tagShader::Rend(DWORD dwrendtype)
	{
		render::Interface * pInterface = render::Interface::GetInstance();
		Layer3D			  * pLayer3D   = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9	pDevice    = pInterface->GetDevice();

		_tagShader::_tagLayer * pTempLayer(NULL);
		pTempLayer = &tLayer[0];
		DWORD usenum = 0;
		DWORD dynamicusenum = 0;
		DWORD lightusenum = 0;
		DWORD lightdynamicusenum = 0;
		vector<RendBuffer::_tagBuffer>* pbuffer = &rendbuffer.testnonebuffer.rendbuffer;
		vector<RendBuffer::_tagBuffer>* pdynamicbuffer = &rendbuffer.testnonebuffer.dynamicrendbuffer;
		vector<RendBuffer::_tagBuffer>* plightbuffer = &rendbuffer.testnonebuffer.lightrendbuffer;
		vector<RendBuffer::_tagBuffer>* plightdynamicbuffer = &rendbuffer.testnonebuffer.lightdynamicrendbuffer;
		switch(dwrendtype)
		{
		case RENDTYPE_TESTNONE:
			usenum = rendbuffer.testnonebuffer.bufferusenum;
			dynamicusenum = rendbuffer.testnonebuffer.dynamicbufferusenum;
			lightusenum = rendbuffer.testnonebuffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.testnonebuffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.testnonebuffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.testnonebuffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.testnonebuffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.testnonebuffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_TESTTRANSPARENT:
			usenum = rendbuffer.testtransparentbuffer.bufferusenum;
			dynamicusenum = rendbuffer.testtransparentbuffer.dynamicbufferusenum;
			lightusenum = rendbuffer.testtransparentbuffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.testtransparentbuffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.testtransparentbuffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.testtransparentbuffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.testtransparentbuffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.testtransparentbuffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_BLENDADDITIVE:
			usenum = rendbuffer.blendadditivebuffer.bufferusenum;
			dynamicusenum = rendbuffer.blendadditivebuffer.dynamicbufferusenum;
			lightusenum = rendbuffer.blendadditivebuffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.blendadditivebuffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.blendadditivebuffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.blendadditivebuffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.blendadditivebuffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.blendadditivebuffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_BLENDALPHA:
			usenum = rendbuffer.blendalphabuffer.bufferusenum;
			dynamicusenum = rendbuffer.blendalphabuffer.dynamicbufferusenum;
			lightusenum = rendbuffer.blendalphabuffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.blendalphabuffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.blendalphabuffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.blendalphabuffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.blendalphabuffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.blendalphabuffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_BLENDALPHALOW:
			usenum = rendbuffer.blendalphalowbuffer.bufferusenum;
			dynamicusenum = rendbuffer.blendalphalowbuffer.dynamicbufferusenum;
			lightusenum = rendbuffer.blendalphalowbuffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.blendalphalowbuffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.blendalphalowbuffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.blendalphalowbuffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.blendalphalowbuffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.blendalphalowbuffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_BLENDALPHALOW1:
			usenum = rendbuffer.blendalphalow1buffer.bufferusenum;
			dynamicusenum = rendbuffer.blendalphalow1buffer.dynamicbufferusenum;
			lightusenum = rendbuffer.blendalphalow1buffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.blendalphalow1buffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.blendalphalow1buffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.blendalphalow1buffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.blendalphalow1buffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.blendalphalow1buffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_BLENDALPHALOW2:
			usenum = rendbuffer.blendalphalow2buffer.bufferusenum;
			dynamicusenum = rendbuffer.blendalphalow2buffer.dynamicbufferusenum;
			lightusenum = rendbuffer.blendalphalow2buffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.blendalphalow2buffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.blendalphalow2buffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.blendalphalow2buffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.blendalphalow2buffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.blendalphalow2buffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_BLENDALPHALOW3:
			usenum = rendbuffer.blendalphalow3buffer.bufferusenum;
			dynamicusenum = rendbuffer.blendalphalow3buffer.dynamicbufferusenum;
			lightusenum = rendbuffer.blendalphalow3buffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.blendalphalow3buffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.blendalphalow3buffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.blendalphalow3buffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.blendalphalow3buffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.blendalphalow3buffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_BLENDALPHALOW4:
			usenum = rendbuffer.blendalphalow4buffer.bufferusenum;
			dynamicusenum = rendbuffer.blendalphalow4buffer.dynamicbufferusenum;
			lightusenum = rendbuffer.blendalphalow4buffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.blendalphalow4buffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.blendalphalow4buffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.blendalphalow4buffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.blendalphalow4buffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.blendalphalow4buffer.lightdynamicrendbuffer;
			break;
		case RENDTYPE_TRANSPARENT:
			usenum = rendbuffer.transparentbuffer.bufferusenum;
			dynamicusenum = rendbuffer.transparentbuffer.dynamicbufferusenum;
			lightusenum = rendbuffer.transparentbuffer.lightbufferusenum;
			lightdynamicusenum = rendbuffer.transparentbuffer.lightdynamicbufferusenum;

			pbuffer = &rendbuffer.transparentbuffer.rendbuffer;
			pdynamicbuffer = &rendbuffer.transparentbuffer.dynamicrendbuffer;
			plightbuffer = &rendbuffer.transparentbuffer.lightrendbuffer;
			plightdynamicbuffer = &rendbuffer.transparentbuffer.lightdynamicrendbuffer;
			break;
		}
		vector<RendBuffer::_tagBuffer>& buffer = *pbuffer;
		vector<RendBuffer::_tagBuffer>& dynamicbuffer = *pdynamicbuffer;
		vector<RendBuffer::_tagBuffer>& lightbuffer = *plightbuffer;
		vector<RendBuffer::_tagBuffer>& lightdynamicbuffer = *plightdynamicbuffer;


		pInterface->SetTexture(0,rendbuffer.dwtexturehandle);
		pInterface->Cull(!bTwoSide);

		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,pTempLayer->dwColorOp);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,pTempLayer->dwColorArg1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,pTempLayer->dwColorArg2);

		pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,pTempLayer->dwAlphaOp);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,pTempLayer->dwAlphaArg1);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,pTempLayer->dwAlphaArg2);

		pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		pLayer3D->SetStreamType(LMST_POSCOLTEX0);
		for (int i = 0 ; i <= usenum ; i++)
		{
			if (buffer.empty())
			{
				break;
			}
			buffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_INDEX);
			buffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_POS);
			buffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
			buffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_UV);
			if (buffer[i].vertexcount == 0)
			{
				buffer[i].Clear();
				continue;
			}
			pDevice->SetStreamSource(0, buffer[i].pd3dposbuffer, 0, sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(2, buffer[i].pd3ddiffusebuffer, 0, sizeof(D3DCOLOR));
			pDevice->SetStreamSource(3, buffer[i].pd3duvbuffer, 0, sizeof(D3DXVECTOR2));
			/*pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);
			memcpy(pCacheIB ,buffer[i].pindexbuffer,sizeof(WORD) * buffer[i].indexcount);
			pIB->Unlock();
			if (FAILED(pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,buffer[i].vertexcount,0,buffer[i].trianglecount)))
			{
			MessageBox(NULL,NULL,NULL,NULL);
			}*/

			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,buffer[i].trianglecount);
			pDevice->SetIndices(NULL);
			pDevice->SetStreamSource(0, NULL, 0, 0);
			pDevice->SetStreamSource(1, NULL, 0, 0);
			pDevice->SetStreamSource(2, NULL, 0, 0);
			pDevice->SetStreamSource(3, NULL, 0, 0);
			buffer[i].Clear();
		}
		for (int i = 0 ; i < dynamicusenum ; i++)
		{
			if (dynamicbuffer.empty())
			{
				break;
			}
			dynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_INDEX);
			dynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_POS);
			dynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
			dynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_UV);
			if (dynamicbuffer[i].vertexcount == 0)
			{
				dynamicbuffer[i].Clear();
				continue;
			}
			pDevice->SetTextureStageState(0,D3DTSS_COLOROP,dynamicbuffer[i].dwTempColorOp);
			pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,dynamicbuffer[i].dwTempColorArg1);
			pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,dynamicbuffer[i].dwTempColorArg2);

			pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,dynamicbuffer[i].dwTempAlphaOp);
			pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,dynamicbuffer[i].dwTempAlphaArg1);
			pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,dynamicbuffer[i].dwTempAlphaArg2);
			D3DXMATRIX matworld;
			D3DXMatrixIdentity(&matworld);
			pDevice->SetTransform(D3DTS_WORLD,&dynamicbuffer[i].matworld);

			pDevice->SetStreamSource(0, dynamicbuffer[i].pd3dposbuffer, 0, sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(2, dynamicbuffer[i].pd3ddiffusebuffer, 0, sizeof(D3DCOLOR));
			pDevice->SetStreamSource(3, dynamicbuffer[i].pd3duvbuffer, 0, sizeof(D3DXVECTOR2));
			/*pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);
			memcpy(pCacheIB ,dynamicbuffer[i].pindexbuffer,sizeof(WORD) * dynamicbuffer[i].indexcount);
			pIB->Unlock();
			if (FAILED(pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,dynamicbuffer[i].vertexcount,0,dynamicbuffer[i].trianglecount)))
			{
			MessageBox(NULL,NULL,NULL,NULL);
			}*/

			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dynamicbuffer[i].trianglecount);
			pDevice->SetTransform(D3DTS_WORLD,&matworld);
			pDevice->SetIndices(NULL);
			pDevice->SetStreamSource(0, NULL, 0, 0);
			pDevice->SetStreamSource(1, NULL, 0, 0);
			pDevice->SetStreamSource(2, NULL, 0, 0);
			pDevice->SetStreamSource(3, NULL, 0, 0);
			dynamicbuffer[i].Clear();
		}

		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,pTempLayer->dwColorOp);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,pTempLayer->dwColorArg1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,pTempLayer->dwColorArg2);

		pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,pTempLayer->dwAlphaOp);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,pTempLayer->dwAlphaArg1);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,pTempLayer->dwAlphaArg2);

		pDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
		pLayer3D->SetStreamType(LMST_POSNMLCOLTEX0);
		for (int i = 0 ; i <= lightusenum ; i++)
		{
			if (lightbuffer.empty())
			{
				break;
			}
			lightbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_INDEX);
			lightbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_POS);
			lightbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
			lightbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
			lightbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_UV);
			if (lightbuffer[i].vertexcount == 0)
			{
				lightbuffer[i].Clear();
				continue;
			}
			pDevice->SetStreamSource(0, lightbuffer[i].pd3dposbuffer, 0, sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1, lightbuffer[i].pd3dnormalbuffer, 0, sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(2, lightbuffer[i].pd3ddiffusebuffer, 0, sizeof(D3DCOLOR));
			pDevice->SetStreamSource(3, lightbuffer[i].pd3duvbuffer, 0, sizeof(D3DXVECTOR2));
			/*pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);
			memcpy(pCacheIB ,lightbuffer[i].pindexbuffer,sizeof(WORD) * lightbuffer[i].indexcount);
			pIB->Unlock();
			if (FAILED(pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,lightbuffer[i].vertexcount,0,lightbuffer[i].trianglecount)))
			{
			MessageBox(NULL,NULL,NULL,NULL);
			}*/

			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,lightbuffer[i].trianglecount);
			pDevice->SetIndices(NULL);
			pDevice->SetStreamSource(0, NULL, 0, 0);
			pDevice->SetStreamSource(1, NULL, 0, 0);
			pDevice->SetStreamSource(2, NULL, 0, 0);
			pDevice->SetStreamSource(3, NULL, 0, 0);
			lightbuffer[i].Clear();
		}
		for (int i = 0 ; i < lightdynamicusenum ; i++)
		{
			if (lightdynamicbuffer.empty())
			{
				break;
			}
			lightdynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_INDEX);
			lightdynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_POS);
			lightdynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
			lightdynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
			lightdynamicbuffer[i].UnLockBuffer(RendBuffer::_tagBuffer::BUFFERTYPE_UV);
			if (lightdynamicbuffer[i].vertexcount == 0)
			{
				lightdynamicbuffer[i].Clear();
				continue;
			}
			pDevice->SetTextureStageState(0,D3DTSS_COLOROP,lightdynamicbuffer[i].dwTempColorOp);
			pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,lightdynamicbuffer[i].dwTempColorArg1);
			pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,lightdynamicbuffer[i].dwTempColorArg2);

			pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,lightdynamicbuffer[i].dwTempAlphaOp);
			pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,lightdynamicbuffer[i].dwTempAlphaArg1);
			pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,lightdynamicbuffer[i].dwTempAlphaArg2);

			D3DXMATRIX matworld;
			D3DXMatrixIdentity(&matworld);
			pDevice->SetTransform(D3DTS_WORLD,&lightdynamicbuffer[i].matworld);
			pDevice->SetStreamSource(0, lightdynamicbuffer[i].pd3dposbuffer, 0, sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1, lightdynamicbuffer[i].pd3dnormalbuffer, 0, sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(2, lightdynamicbuffer[i].pd3ddiffusebuffer, 0, sizeof(D3DCOLOR));
			pDevice->SetStreamSource(3, lightdynamicbuffer[i].pd3duvbuffer, 0, sizeof(D3DXVECTOR2));
			/*pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);
			memcpy(pCacheIB ,lightdynamicbuffer[i].pindexbuffer,sizeof(WORD) * lightdynamicbuffer[i].indexcount);
			pIB->Unlock();
			if (FAILED(pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,lightdynamicbuffer[i].vertexcount,0,lightdynamicbuffer[i].trianglecount)))
			{
			MessageBox(NULL,NULL,NULL,NULL);
			}*/

			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,lightdynamicbuffer[i].trianglecount);
			pDevice->SetTransform(D3DTS_WORLD,&matworld);
			pDevice->SetIndices(NULL);
			pDevice->SetStreamSource(0, NULL, 0, 0);
			pDevice->SetStreamSource(1, NULL, 0, 0);
			pDevice->SetStreamSource(2, NULL, 0, 0);
			pDevice->SetStreamSource(3, NULL, 0, 0);
			lightdynamicbuffer[i].Clear();
		}
	}

	/*
	* 功能: 渲染alpha测试模式
	* 摘要: 渲染方式包括:根据渲染组的配置好的着色信息进行渲染,然后渲染动态着色信息相关的多边形
	* 参数: -
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.01.23 - lpf
	*		增加了对动态着色变量链的渲染
	*/
	void render::Interface::Layer3D::ShaderGroup::RenderAlphaTestMode()
	{
		// Setup stream format
		render::Interface * pInterface = render::Interface::GetInstance();
		Layer3D			  * pLayer3D   = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9	pDevice    = pInterface->GetDevice();
		_tagShader		  * pShader(NULL);

		// Render opaque mode : mask and none
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);	

		// 循环处理所有纹理
		for (DWORD n = 0; n < m_dwTexCount[1]; ++n)
		{
			_tagTex1 * pTex1 = &m_pTex1Array[n];
			_tagTex0 * pTex0 = pTex1->pTex0Head;

			while (pTex0)
			{				
				pInterface->SetTexture(0, pTex0->dwTex0Handle);		//设置纹理Layer0

				// 处理静态着色变量的渲染
				pShader = pTex0->pShaderHead;
				while (pShader)
				{
					if (pShader->pTriangleAlphaTestHead)
					{
						// 根据当前Shader属性设置渲染设备
						pInterface->Cull(!pShader->bTwoSide);

						// 设置alpha test mode						
						if (!pShader->dwAlphaTestMode)		//0 test
						{
							// test + none
							pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
							pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 						
							pDevice->SetRenderState(D3DRS_ALPHAREF, 0x0000);
							pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
						}else
						{
							// test + transparent
							pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
							pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
							//pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
							//pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
							pDevice->SetRenderState(D3DRS_ALPHAREF, 0x0000);
							pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
						}

						_SetTextureOp(pShader);

						pLayer3D->SetStreamType(pShader->dwStreamType);			//设置顶点流格式
						switch (pShader->dwStreamType)
						{
						case LMST_POSNML:
							__RenderPOSCOL(pShader->pTriangleAlphaTestHead);
							break;
						case LMST_POSNMLTEX0:
							__RenderPOSDIFTEX0(pShader->pTriangleAlphaTestHead);
							break;
							/*	case LMST_POSNMLTEX0:
							__RenderPOSDIFSPCTEX0(pShader->pTriangleAlphaTestHead);
							break;*/
						}

						pShader->pTriangleAlphaTestHead = NULL;			//渲染完清空
					}

					pShader = pShader->pNext;		//处理下一个着色变量
				}

				// 处理动态着色变量的渲染
				pShader = pTex0->pDynamicShaderHead;
				while (pShader)
				{
					if (pShader->pTriangleAlphaTestHead)
					{
						// 根据当前Shader属性设置渲染设备
						pInterface->Cull(!pShader->bTwoSide);

						// 设置alpha test mode
						if (!pShader->dwAlphaTestMode)		//0 test
						{
							//test + none
							pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
							pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
							pDevice->SetRenderState(D3DRS_ALPHAREF, 0x0000);
							pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
						}else 
						{
							//test + transparent
							pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
							pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
							//pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
							//pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
							pDevice->SetRenderState(D3DRS_ALPHAREF, 0x0000);
							pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
						}

						_SetTextureOp(pShader);

						pLayer3D->SetStreamType(pShader->dwStreamType);			//设置顶点流格式
						switch (pShader->dwStreamType)
						{
						case LMST_POSNML:
							__RenderPOSCOL(pShader->pTriangleAlphaTestHead);
							break;
						case LMST_POSNMLTEX0:
							__RenderPOSDIFTEX0(pShader->pTriangleAlphaTestHead);
							break;
							/*case LMST_POSNMLTEX0:
							__RenderPOSDIFSPCTEX0(pShader->pTriangleAlphaTestHead);
							break;*/
						}

						pShader->pTriangleAlphaTestHead = NULL;		//渲染完清空
					}

					// 删除完成渲染的动态着色变量
					_tagShader * pTmpShader = pShader;
					pShader					= pShader->pNext;
					SAFEDELETE(pTmpShader);
				}

				// 准备处理下一个纹理
				pTex0->pDynamicShaderHead = NULL;
				pTex0					  = pTex0->pNext;
			}
		}
	}



	/*
	* 功能: 渲染alpha混合模式
	* 摘要: 渲染方式包括:根据渲染组的配置好的着色信息进行渲染,然后渲染动态着色信息相关的多边形
	* 参数: -
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.01.23 - lpf
	*		增加了对动态着色变量链的渲染
	*/
	void render::Interface::Layer3D::ShaderGroup::RenderAlphaBlendMode()
	{
		// Setup stream format
		render::Interface * pInterface = render::Interface::GetInstance();
		Layer3D			  * pLayer3D   = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9	pDevice	   = pInterface->GetDevice();
		_tagShader		  * pShader(NULL);

		// 渲染透明部分
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		// 循环处理所有纹理
		// not finish yet
		for (DWORD n = 0; n < m_dwTexCount[1]; ++n)
		{
			_tagTex1 * pTex1 = &m_pTex1Array[n];
			_tagTex0 * pTex0 = pTex1->pTex0Head;

			while (pTex0)
			{
				pInterface->SetTexture(0, pTex0->dwTex0Handle);			//设置纹理Layer0

				// 处理静态着色变量的渲染
				_tagShader * pShader = pTex0->pShaderHead;
				while (pShader)
				{
					if (pShader->pTriangleAlphaBlendHead)
					{
						// 根据当前Shader属性设置渲染设备
						pInterface->Cull(!pShader->bTwoSide);

						// 设置Alpha blend mode
						pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
						pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
						switch(pShader->dwAlphaBlendMode)
						{
						case 0:		//blend + additive
							pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
							pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
							break;
						case 1:		//blend + alpha
							pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
							pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
							break;
						case 2:		//blend + color
							pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
							pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
							break;
						case 3:		//blend + reserved
							break;
						}

						_SetTextureOp(pShader);			//设置纹理颜色操作

						pLayer3D->SetStreamType(pShader->dwStreamType);			//设置定点流格式
						switch (pShader->dwStreamType)
						{
						case LMST_POSNML:
							__RenderPOSCOL(pShader->pTriangleAlphaBlendHead);
							break;
						case LMST_POSNMLTEX0:
							__RenderPOSDIFTEX0(pShader->pTriangleAlphaBlendHead);
							break;
							/*case LMST_POSNMLTEX0:
							__RenderPOSDIFSPCTEX0(pShader->pTriangleAlphaBlendHead);
							break;*/
						}

						pShader->pTriangleAlphaBlendHead = NULL;		//渲染完清空
					}

					pShader = pShader->pNext;			//处理下一个着色变量
				}

				// 处理动态着色变量的渲染
				pShader = pTex0->pDynamicShaderHead;
				while(pShader)
				{
					if (pShader->pTriangleAlphaBlendHead)
					{
						// 根据当前Shader属性设置渲染设备
						pInterface->Cull(!pShader->bTwoSide);

						// 设置Alpha blend mode
						pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
						pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
						switch(pShader->dwAlphaBlendMode)
						{
						case 0:		//blend + additive
							pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
							pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
							break;
						case 1:		//blend + alpha
							pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
							pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
							break;
						case 2:		//blend + color
							pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
							pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
							break;
						case 3:		//blend + reserved
							break;
						}

						_SetTextureOp(pShader);			//设置纹理颜色操作

						pLayer3D->SetStreamType(pShader->dwStreamType);			//设置顶点流格式
						switch (pShader->dwStreamType)
						{
						case LMST_POSNML:
							__RenderPOSCOL(pShader->pTriangleAlphaBlendHead);
							break;
						case LMST_POSNMLTEX0:
							__RenderPOSDIFTEX0(pShader->pTriangleAlphaBlendHead);
							break;
							/*case LMST_POSNMLTEX0:
							__RenderPOSDIFSPCTEX0(pShader->pTriangleAlphaBlendHead);
							break;*/
						}

						pShader->pTriangleAlphaBlendHead = NULL;			//渲染完清空
					}

					// 删除完成渲染的动态着色变量
					_tagShader * pTmpShader = pShader;
					pShader					= pShader->pNext;
					SAFEDELETE(pTmpShader);
				}

				// 准备处理下一个纹理
				pTex0->pDynamicShaderHead = NULL;
				pTex0					  = pTex0->pNext;
			}
		}
	}

	void render::Interface::Layer3D::ShaderGroup::RenderTransparentMode()
	{
		//setup stream format
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		//渲染透明部分
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);	

		//not finish yet
		for(DWORD n = 0; n < m_dwTexCount[1]; n++)
		{
			_tagTex1 *pTex1 = &m_pTex1Array[n];
			//设置纹理Layer1
			//pInterface->SetTexture(1,pTex1->dwTex1Handle);

			_tagTex0 *pTex0 = pTex1->pTex0Head;
			while(pTex0)
			{
				//设置纹理Layer0
				pInterface->SetTexture(0,pTex0->dwTex0Handle);

				_tagShader *pShader = pTex0->pShaderHead;
				while(pShader)
				{
					if (pShader->pTriangleTransparentHead)
					{
						//设置Shader属性
						pInterface->Cull(!pShader->bTwoSide);
						//设置Alpha blend mode
						pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
						pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
						pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
						pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
						pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
						pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
						//设置颜色操作
						_SetTextureOp(pShader);
						//设置定点流格式

						pLayer3D->SetStreamType(LMST_POSNMLTEX0);
						__RenderPOSDIFTEX0(pShader->pTriangleTransparentHead);
						//渲染完清空
						pShader->pTriangleTransparentHead = NULL;
					}
					//渲染
					pShader = pShader->pNext;
				}

				pTex0 = pTex0->pNext;
			}
		}
	}
	//void render::Interface::Layer3D::ShaderGroup::RenderAlphaBlendTopMode()
	//{
	//	//setup stream format
	//	render::Interface *pInterface = render::Interface::GetInstance();
	//	Layer3D *pLayer3D = pInterface->GetLayer3D();
	//	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	//	//渲染透明部分
	//	pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
	//	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);	
	//
	//	//not finish yet
	//	for(DWORD n = 0; n < m_dwTexCount[1]; n++)
	//	{
	//		_tagTex1 *pTex1 = &m_pTex1Array[n];
	//		//设置纹理Layer1
	//		//pInterface->SetTexture(1,pTex1->dwTex1Handle);
	//
	//		_tagTex0 *pTex0 = pTex1->pTex0Head;
	//		while(pTex0)
	//		{
	//			//设置纹理Layer0
	//			pInterface->SetTexture(0,pTex0->dwTex0Handle);
	//
	//			_tagShader *pShader = pTex0->pShaderHead;
	//			while(pShader)
	//			{
	//				if (pShader->pTraingleAlphaBlendTopHead)
	//				{
	//					//设置Shader属性
	//					pInterface->Cull(!pShader->bTwoSide);
	//					//设置Alpha blend mode
	//					pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	//					pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	//					//blend
	//					switch(pShader->dwAlphaBlendMode)
	//					{
	//					case 0://blend + additive
	//						{
	//							pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	//							pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	//						}
	//						break;
	//					case 1://blend + alpha
	//						{
	//							pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	//							pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	//						}
	//						break;
	//					case 2://blend + color
	//						{
	//							pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
	//							pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
	//						}
	//						break;
	//					case 3://blend + reserved
	//						{
	//						}
	//						break;
	//					}
	//					//设置颜色操作
	//					_SetTextureOp(pShader);
	//					//设置定点流格式
	//					pLayer3D->SetStreamType(pShader->dwStreamType);
	//
	//					switch(pShader->dwStreamType)
	//					{
	//					case LMST_POSNML:
	//						__RenderPOSCOL(pShader->pTraingleAlphaBlendTopHead);
	//						break;
	//					case LMST_POSNMLTEX0:
	//						__RenderPOSDIFTEX0(pShader->pTraingleAlphaBlendTopHead);
	//						break;
	//					case LMST_POSNMLTEX0:
	//						__RenderPOSDIFSPCTEX0(pShader->pTraingleAlphaBlendTopHead);
	//						break;
	//					}
	//					//渲染完清空
	//					pShader->pTraingleAlphaBlendTopHead = NULL;
	//				}
	//				//渲染
	//				pShader = pShader->pNext;
	//			}
	//
	//			pTex0 = pTex0->pNext;
	//		}
	//	}
	//}

	void render::Interface::Layer3D::ShaderGroup::__RenderPOSCOL(_tagTriangle *pHead)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		D3DXVECTOR3 *pCachePos(NULL);
		D3DXVECTOR3    *pCacheNml(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBNML = pLayer3D->m_pVB_NORMAL;

		_tagTriangle *pPolygon = pHead;

		if (!pPolygon) 
			//没有,直接返回
			return ;

		//填充VB进行绘图

		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBNML->Lock( 0, 0, (void**)&pCacheNml, D3DLOCK_DISCARD);

		DWORD dwVerticesCount = 0;	
		while(pPolygon)
		{
			memcpy(pCachePos + dwVerticesCount,&pPolygon->pos[0],sizeof(D3DXVECTOR3) * 3);
			memcpy(pCacheNml + dwVerticesCount,&pPolygon->normal[0],sizeof(D3DXVECTOR3) * 3);

			dwVerticesCount +=3;

			//记算越界,清空缓冲
			if (dwVerticesCount >= LAYER3D_VERTEXBUFFER_SIZE)
			{
				pVBPOS->Unlock();
				pVBNML->Unlock();

				pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, LAYER3D_VERTEXBUFFER_SIZE / 3);
				pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
				pVBNML->Lock( 0, 0, (void**)&pCacheNml, D3DLOCK_DISCARD);

				dwVerticesCount = 0;
			}

			pPolygon = pPolygon->pNext;
		}

		if (dwVerticesCount)//some left
		{
			pVBPOS->Unlock();
			pVBNML->Unlock();
			pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwVerticesCount / 3);
		}
	}

	void render::Interface::Layer3D::ShaderGroup::__RenderPOSDIFTEX0(_tagTriangle *pHead)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		D3DXVECTOR3 *pCachePos(NULL);
		D3DXVECTOR3  *pCacheNml(NULL);
		D3DXVECTOR2 *pCacheTex0(NULL);

		LPDIRECT3DVERTEXBUFFER9 pVBPOS    = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBNML = pLayer3D->m_pVB_NORMAL;
		LPDIRECT3DVERTEXBUFFER9 pVBTEX0   = pLayer3D->m_pVB_TEX0;

		_tagTriangle *pPolygon = pHead;

		if (!pPolygon) 
			//没有,直接返回
			return ;

		//填充VB进行绘图

		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBNML->Lock( 0, 0, (void**)&pCacheNml, D3DLOCK_DISCARD);
		pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

		DWORD dwVerticesCount = 0;	
		while(pPolygon)
		{
			memcpy(pCachePos + dwVerticesCount,&pPolygon->pos[0],sizeof(D3DXVECTOR3) * 3);
			memcpy(pCacheNml + dwVerticesCount,&pPolygon->normal[0],sizeof(D3DXVECTOR3)  * 3);
			memcpy(pCacheTex0 + dwVerticesCount,&pPolygon->uv0[0],sizeof(D3DXVECTOR2)* 3);

			dwVerticesCount +=3;

			//记算越界,清空缓冲
			if (dwVerticesCount >= LAYER3D_VERTEXBUFFER_SIZE)
			{
				pVBPOS->Unlock();
				pVBNML->Unlock();
				pVBTEX0->Unlock();

				pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, LAYER3D_VERTEXBUFFER_SIZE / 3);
				pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
				pVBNML->Lock( 0, 0, (void**)&pCacheNml, D3DLOCK_DISCARD);
				pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

				dwVerticesCount = 0;
			}
			pPolygon = pPolygon->pNext;
		}

		if (dwVerticesCount)//some left
		{
			pVBPOS->Unlock();
			pVBNML->Unlock();
			pVBTEX0->Unlock();

			pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwVerticesCount / 3);
		}
	}

	void render::Interface::Layer3D::ShaderGroup::__RenderPOSDIFSPCTEX0(_tagTriangle *pHead)
	{
		return;
		//render::Interface *pInterface = render::Interface::GetInstance();
		//Layer3D *pLayer3D= pInterface->GetLayer3D();
		//D3DXVECTOR3 *pCachePos(NULL);
		//D3DCOLOR    *pCacheCol(NULL);
		//D3DCOLOR    *pCacheColSpc(NULL);
		//D3DXVECTOR2 *pCacheTex0(NULL);

		//LPDIRECT3DVERTEXBUFFER9 pVBPOS    = pLayer3D->m_pVB_POSITION;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOLDIF = pLayer3D->m_pVB_COLOR_DIFFUSE;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC = pLayer3D->m_pVB_COLOR_SPECULAR;
		//LPDIRECT3DVERTEXBUFFER9 pVBTEX0   = pLayer3D->m_pVB_TEX0;

		//_tagTriangle *pPolygon = pHead;

		//if (!pPolygon) 
		//	//没有,直接返回
		//	return ;

		////填充VB进行绘图

		//pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		//pVBCOLDIF->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
		//pVBCOLSPC->Lock(0,0, (void**)&pCacheColSpc,D3DLOCK_DISCARD);
		//pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

		//DWORD dwVerticesCount = 0;	
		//while(pPolygon)
		//{
		//	memcpy(pCachePos + dwVerticesCount,&pPolygon->pos[0],sizeof(D3DXVECTOR3) * 3);
		//	memcpy(pCacheCol + dwVerticesCount,&pPolygon->color[0],sizeof(D3DCOLOR)  * 3);
		//	memcpy(pCacheColSpc + dwVerticesCount,&pPolygon->specular[0],sizeof(D3DCOLOR)  * 3);
		//	memcpy(pCacheTex0 + dwVerticesCount,&pPolygon->uv0[0],sizeof(D3DXVECTOR2)* 3);

		//	dwVerticesCount +=3;

		//	//记算越界,清空缓冲
		//	if (dwVerticesCount >= LAYER3D_VERTEXBUFFER_SIZE)
		//	{
		//		pVBPOS->Unlock();
		//		pVBCOLDIF->Unlock();
		//		pVBCOLSPC->Unlock();
		//		pVBTEX0->Unlock();

		//		pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, LAYER3D_VERTEXBUFFER_SIZE / 3);
		//		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		//		pVBCOLDIF->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
		//		pVBCOLSPC->Lock(0,0, (void**)&pCacheColSpc,D3DLOCK_DISCARD);
		//		pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

		//		dwVerticesCount = 0;
		//	}
		//	pPolygon = pPolygon->pNext;
		//}

		//if (dwVerticesCount)//some left
		//{
		//	pVBPOS->Unlock();
		//	pVBCOLDIF->Unlock();
		//	pVBCOLSPC->Unlock();
		//	pVBTEX0->Unlock();

		//	pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwVerticesCount / 3);
		//}
	}


	//__CLASS__(render::Interface::Layer3D)__
	render::Interface::Layer3D::Layer3D(void) :
	m_dwFillMode(D3DFILL_SOLID),
		m_eFilterType(D3DTEXF_POINT)
	{
		ZeroMemory(m_pShaderGroupArray,MAX_SHADER_GROUP_ARRAY_SIZE * sizeof(ShaderGroup *));
		m_dwShaderGroupArrayUsage = 0;
	}

	render::Interface::Layer3D::~Layer3D(void)
	{
	}
	void render::Interface::Layer3D::SetAmbient(D3DCOLOR ambient)
	{
		m_Ambinet = ambient;
	}
	void render::Interface::Layer3D::SetLight(D3DLIGHT9* d3dlight)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		m_pD3DLight = d3dlight;
		pDevice->SetLight( 0, m_pD3DLight );
		pDevice->LightEnable( 0, true );
		pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	}
	void render::Interface::Layer3D::SetMaterial(D3DMATERIAL9* pmaterial)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		m_pMaterial = pmaterial;
		pDevice->SetMaterial(m_pMaterial);
	}
	BOOL render::Interface::Layer3D::CreateVertexBuffer()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		//流缓冲
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * LAYER3D_DRAWVERTEXBUFFER_SIZE
			, D3DUSAGE_WRITEONLY |D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &m_pVB_POSITION,NULL)))
		{
			OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
			ASSERT(0);
			return FALSE;
		}

		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR3) * LAYER3D_DRAWVERTEXBUFFER_SIZE
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC  ,0,D3DPOOL_SYSTEMMEM , &m_pVB_NORMAL,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d normal vextexbuffer failed.\n");
			ASSERT(0);
			return FALSE;
		}

		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DCOLOR) * LAYER3D_DRAWVERTEXBUFFER_SIZE
			, D3DUSAGE_WRITEONLY |D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &m_pVB_COLOR_DIFFUSE,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d color vextexbuffer failed.\n");
			ASSERT(0);
			return FALSE;
		}		

		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * LAYER3D_DRAWVERTEXBUFFER_SIZE
			, D3DUSAGE_WRITEONLY |D3DUSAGE_DYNAMIC ,0,D3DPOOL_SYSTEMMEM , &m_pVB_TEX0,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
			ASSERT(0);
			return FALSE;
		}
		if (FAILED(pDevice->CreateIndexBuffer( sizeof(WORD) * LAYER3D_DRAWINDEXBUFFER_SIZE
			, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC  ,D3DFMT_INDEX16,D3DPOOL_SYSTEMMEM , &m_pIB,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d indexbuffer failed.\n");
			ASSERT(0);
			return FALSE;
		}
		return TRUE;
	}

	void render::Interface::Layer3D::DestroyVertexBuffer()
	{
		SAFERELEASE(m_pVB_POSITION);
		SAFERELEASE(m_pVB_NORMAL);
		SAFERELEASE(m_pVB_COLOR_DIFFUSE);
		//SAFERELEASE(m_pVB_COLOR_SPECULAR);
		SAFERELEASE(m_pVB_TEX0);
		SAFERELEASE(m_pIB);
	}



	/*
	 * 功能: 创建顶点声明
	 * 摘要: -
	 * 参数: -
	 * 返回值: -
	 * 作者: -
	 * 创建日期: -
	 * 修改日志:
	 *	2009.10.20 - lpf
	 *		增加了vertexDecl9声明,即pos, nor, dif, uv2,主要用于水面渲染
	 */
	void render::Interface::Layer3D::CreateVertexDecl()
	{
		//Create VetexShader
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		HRESULT hr;
		//顶点
		D3DVERTEXELEMENT9 vertexDecl0[] =  //pos | normal
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};
		//顶点
		D3DVERTEXELEMENT9 vertexDecl1[] =  //pos | col
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};
		D3DVERTEXELEMENT9 vertexDecl2[] = //pos | normal | tex0
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },		
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 vertexDecl3[] = //pos | col | tex0
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },		
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 vertexDecl4[] =  //pos | normal | col
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};

		D3DVERTEXELEMENT9 vertexDecl5[] = //pos | normal | | col | tex0
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },		
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 vertexDecl6[] = //pos
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },	
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 vertexDecl7[] = //posuv
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,0 },	
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 vertexDecl8[] = //posuv
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,0 },	
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
			{ 3, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,1 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
                D3DVERTEXELEMENT9 vertexDecl9[] = //pos, nor, dif, uv2
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
			{ 3, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,1 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};



		hr = pDevice->CreateVertexDeclaration(vertexDecl0,&m_pVertexDeclarationArray[0]);
		if (FAILED(hr))
		{	
			OutputError("CREATE VERTEX DECLARE 0 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl1,&m_pVertexDeclarationArray[1]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 1 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl2,&m_pVertexDeclarationArray[2]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 2 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl3,&m_pVertexDeclarationArray[3]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 3 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl4,&m_pVertexDeclarationArray[4]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 4 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl5,&m_pVertexDeclarationArray[5]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 5 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl6,&m_pVertexDeclarationArray[6]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 6 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl7,&m_pVertexDeclarationArray[7]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 7 FAILED.\n");
		}
		hr = pDevice->CreateVertexDeclaration(vertexDecl8,&m_pVertexDeclarationArray[8]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 8 FAILED.\n");
		}
                hr = pDevice->CreateVertexDeclaration(vertexDecl9,&m_pVertexDeclarationArray[9]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 9 FAILED.\n");
		}


		//用GPU运算的顶点格式声明
		//顶点
		D3DVERTEXELEMENT9 Decl0[] =  //pos
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};
		//顶点
		D3DVERTEXELEMENT9 Decl1[] =  //pos | nml 
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};
		D3DVERTEXELEMENT9 Decl2[] = //pos | uv
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },	
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 Decl3[] = //pos | nml | uv
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },	
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 Decl4[] =  //pos |matrixindex
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 3, 0, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};
		//顶点
		D3DVERTEXELEMENT9 Decl5[] =  //pos | nml |matrixindex
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 3, 0, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};
		D3DVERTEXELEMENT9 Decl6[] = //pos | uv|matrixindex
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },	
			{ 3, 0, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },	
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 Decl7[] = //pos | nml | uv|matrixindex
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },	
			{ 3, 0, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 Decl8[] = //pos | color
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 4, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 Decl9[] = //pos | uv |color
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
			{ 4, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 Decl10[] = //pos | nml |color
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 4, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		D3DVERTEXELEMENT9 Decl11[] = //pos | nml | uv | color
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
			{ 4, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};
		//Create VetexShader decla
		hr = pDevice->CreateVertexDeclaration(Decl0,&m_pVertexDeclarationArrayForGpu[LMSTFG_POS]);
		if (FAILED(hr))
		{	
			OutputError("CREATE VERTEX DECLARE 0 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS] = m_pVertexDeclarationArrayForGpu[LMSTFG_POS];

		hr = pDevice->CreateVertexDeclaration(Decl1,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSNML]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 1 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USENORMAL] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSNML];

		hr = pDevice->CreateVertexDeclaration(Decl2,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSUV]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 2 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USEUV] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSUV];

		hr = pDevice->CreateVertexDeclaration(Decl3,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUV]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 3 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USENORMAL|EFFECTTYPE_USEUV] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUV];

		hr = pDevice->CreateVertexDeclaration(Decl4,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSMATRIX]);
		if (FAILED(hr))
		{	
			OutputError("CREATE VERTEX DECLARE 4 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USEMATINDEX] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSMATRIX];

		hr = pDevice->CreateVertexDeclaration(Decl5,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLMATRIX]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 5 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USENORMAL|EFFECTTYPE_USEMATINDEX] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLMATRIX];

		hr = pDevice->CreateVertexDeclaration(Decl6,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSUVMATRIX]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 6 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USEUV|EFFECTTYPE_USEMATINDEX] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSUVMATRIX];

		hr = pDevice->CreateVertexDeclaration(Decl7,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUVMATRIX]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 7 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USENORMAL|EFFECTTYPE_USEUV|EFFECTTYPE_USEMATINDEX] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUVMATRIX];

		hr = pDevice->CreateVertexDeclaration(Decl8,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSCOLOR]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 8 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USECOLOR] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSCOLOR];


		hr = pDevice->CreateVertexDeclaration(Decl9,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSUVCOLOR]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 9 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USEUV|EFFECTTYPE_USECOLOR] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSUVCOLOR];

		hr = pDevice->CreateVertexDeclaration(Decl10,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLCOLOR]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 10 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USENORMAL|EFFECTTYPE_USECOLOR] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLCOLOR];

		hr = pDevice->CreateVertexDeclaration(Decl11,&m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUVCOLOR]);
		if (FAILED(hr))
		{
			OutputError("CREATE VERTEX DECLARE 11 FAILED.\n");
		}
		m_mapVertexDeclarationArrayForGpu[EFFECTTYPE_USEPOS|EFFECTTYPE_USENORMAL|EFFECTTYPE_USEUV|EFFECTTYPE_USECOLOR] = m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUVCOLOR];
	}

	void render::Interface::Layer3D::DestroyVertexDecl()
	{	
		SAFERELEASE(m_pVertexDeclarationArray[0]);
		SAFERELEASE(m_pVertexDeclarationArray[1]);
		SAFERELEASE(m_pVertexDeclarationArray[2]);
		SAFERELEASE(m_pVertexDeclarationArray[3]);
		SAFERELEASE(m_pVertexDeclarationArray[4]);
		SAFERELEASE(m_pVertexDeclarationArray[5]);
		SAFERELEASE(m_pVertexDeclarationArray[6]);
		SAFERELEASE(m_pVertexDeclarationArray[7]);
		SAFERELEASE(m_pVertexDeclarationArray[8]);
                SAFERELEASE(m_pVertexDeclarationArray[9]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POS]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POSNML]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POSUV]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUV]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POSMATRIX]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLMATRIX]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POSUVMATRIX]);
		SAFERELEASE(m_pVertexDeclarationArrayForGpu[LMSTFG_POSNMLUVMATRIX]);
	}

	void render::Interface::Layer3D::SetTexture(DWORD dwShaderGroupHandle,DWORD dwShaderIndex)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroupHandle);
		if (!pShaderGroup)
		{
			return;
		}
		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}
		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		pInterface->SetTexture(1,NULL);
	}
	void render::Interface::Layer3D::SetShaderTempStreamType(DWORD dwShaderGroupHandle,DWORD dwShaderIndex,DWORD type)
	{

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroupHandle);
		if (!pShaderGroup)
		{
			return;
		}
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (type == -1)
		{
			pShader->dwTempStreamType = pShader->dwStreamType;
		}
		else
		{
			pShader->dwTempStreamType = type;
		}

	}
	void render::Interface::Layer3D::SetColorAndAlphaOp(DWORD dwcolorop,DWORD dwcolorarg1,DWORD dwcolorarg2,DWORD dwalphaop,DWORD dwalphaarg1,DWORD dwalphaarg2,bool binit)
	{
		static DWORD sdwcolorop     = -1;
		static DWORD sdwcolorarg1   = -1;
		static DWORD sdwcolorarg2   = -1;
		static DWORD sdwalphaop     = -1;
		static DWORD sdwalphaarg1   = -1;
		static DWORD sdwalphaarg2   = -1;
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		if (binit)
		{
			sdwcolorop    = -1;
			sdwcolorarg1  = -1;
			sdwcolorarg2  = -1;
			sdwalphaop    = -1;
			sdwalphaarg1  = -1;
			sdwalphaarg2  = -1;
		}

		if (dwcolorop != sdwcolorop)
		{
			if (dwcolorop < D3DTOP_DISABLE)
			{
				dwcolorop = D3DTOP_DISABLE;
			}
			pDevice->SetTextureStageState(0,D3DTSS_COLOROP,dwcolorop);
			sdwcolorop = dwcolorop;
		}
		if (dwcolorarg1 != sdwcolorarg1)
		{
			if (dwcolorarg1 < D3DTOP_DISABLE)
			{

				dwcolorarg1 = D3DTOP_DISABLE;
			}
			pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,dwcolorarg1);
			sdwcolorarg1 = dwcolorarg1;
		}
		if (dwcolorarg2 != sdwcolorarg2)
		{
			if (dwcolorarg2 < D3DTOP_DISABLE)
			{
				dwcolorarg2 = D3DTOP_DISABLE;
			}
			pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,dwcolorarg2);
			sdwcolorarg2 = dwcolorarg2;
		}
		if (dwalphaop != sdwalphaop)
		{
			if (dwalphaop < D3DTOP_DISABLE)
			{
				dwalphaop = D3DTOP_DISABLE;

			}
			pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,dwalphaop);
			sdwalphaop = dwalphaop;
		}
		if (dwalphaarg1 != sdwalphaarg1)
		{
			if (dwalphaarg1 < D3DTOP_DISABLE)
			{
				dwalphaarg1 = D3DTOP_DISABLE;

			}
			pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,dwalphaarg1);
			sdwalphaarg1 = dwalphaarg1;
		}
		if (dwalphaarg2 != sdwalphaarg2)
		{
			if (dwalphaarg2 < D3DTOP_DISABLE)
			{

				dwalphaarg2 = D3DTOP_DISABLE;
			}
			pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,dwalphaarg2);
			sdwalphaarg2 = dwalphaarg2;
		}





	}
	void render::Interface::Layer3D::SetVertexDecla(DWORD dwdecl,bool binit)
	{
		static DWORD sdwdecl = 0;
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		if (binit)
		{
			sdwdecl = -1;
		}
		if (dwdecl != sdwdecl)
		{
			pDevice->SetVertexDeclaration(m_mapVertexDeclarationArrayForGpu[dwdecl]);
			sdwdecl = dwdecl;
		}
	}
	void render::Interface::Layer3D::SetStreamType(DWORD dwStreamType)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[dwStreamType]);
	}

	void render::Interface::Layer3D::SetStreamSource(DWORD dwStreamType)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		switch(dwStreamType)
		{
		case LMST_POS:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			break;
		case LMST_POSCOL:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(2,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
			break;
		case LMST_POSNML:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1,m_pVB_NORMAL,0,sizeof(D3DXVECTOR3));
			break;
		case LMST_POSNMLTEX0:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1,m_pVB_NORMAL,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
			break;
		case LMST_POSNMLCOLTEX0:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1,m_pVB_NORMAL,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(2,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
			pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
			break;
		}
	}


	void render::Interface::Layer3D::CreatePolgyonArray(void)
	{
		m_pPolygonArray = new _tagTriangle[MAX_POLYGON_ARRAY_SIZE];
		m_dwPolygonArrayUsage = 0;
	}

	void render::Interface::Layer3D::DestroyPolygonArray(void)
	{
		SAFEDELETE(m_pPolygonArray);
	}



	/*
	* 功能: 创建3D渲染层
	* 摘要: -
	* 参数: tSetting - 当前设备的配置信息
	* 返回值: 总是返回 TRUE
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了对成员变量m_eFilterType(纹理过滤类型)判断赋值。
	*/
	BOOL render::Interface::Layer3D::Create(const render::Interface::_tagSetting & tSetting)
	{
		CreateVertexBuffer();
		CreateVertexDecl();
		CreatePolgyonArray();

		// 判断并为线性过滤类型赋值
		if (tSetting.b3DLinear)
			m_eFilterType = D3DTEXF_LINEAR;
		else
			m_eFilterType = D3DTEXF_POINT;

		return TRUE;
	}

	void render::Interface::Layer3D::Destroy(void)
	{
		DestroyVertexBuffer();
		DestroyVertexDecl();
		DestroyPolygonArray();
	}

	DWORD render::Interface::Layer3D::CreateShaderGroupFromFile(const char * pszFileName,DWORD dwTextureProcision)
	{
		//找空位，分配shader group
		DWORD dwShaderGroupIndex = 0;
		for(; dwShaderGroupIndex < MAX_SHADER_GROUP_ARRAY_SIZE; dwShaderGroupIndex++)
		{
			if (!m_pShaderGroupArray[dwShaderGroupIndex]) 
				break;
		}

		if (dwShaderGroupIndex == MAX_SHADER_GROUP_ARRAY_SIZE)  
		{
			OutputConsole("error: render::Layer3D CreateShaderGroup() failed. ShaderGroup cache overflow.");
			ASSERT(0);
			return 0xFFFFFFFF;
		}

		ShaderGroup *pShaderGroup = new ShaderGroup;
		if (!pShaderGroup->CreateFromFile(pszFileName,dwTextureProcision))
		{
			OutputConsole("error: render::Layer3D CreateShaderGroup() failed. ShaderGroup Create Failed.");
			ASSERT(0);
			return 0xFFFFFFFF;
		}
		m_pShaderGroupArray[dwShaderGroupIndex] = pShaderGroup;

		//if (dwShaderGroupIndex == 96)
		//{
		//	ASSERT(0);
		//}

		m_dwShaderGroupArrayUsage ++;
		return dwShaderGroupIndex;
	}
	// ----------------------------------------------------
	// 建立ShaderGroup
	// 返回内部渲染句柄
	// 
	// ----------------------------------------------------
	DWORD render::Interface::Layer3D::CreateShaderGroup(const char *pszString,const char *pszTexPath,DWORD dwTextureProcision)
	{
		//找空位，分配shader group
		DWORD dwShaderGroupIndex = 0;
		for(; dwShaderGroupIndex < MAX_SHADER_GROUP_ARRAY_SIZE; dwShaderGroupIndex++)
		{
			if (!m_pShaderGroupArray[dwShaderGroupIndex]) 
				break;
		}

		if (dwShaderGroupIndex == MAX_SHADER_GROUP_ARRAY_SIZE)  
		{
			OutputConsole("error: render::Layer3D CreateShaderGroup() failed. ShaderGroup cache overflow.");
			ASSERT(0);
			return 0xFFFFFFFF;
		}

		ShaderGroup *pShaderGroup = new ShaderGroup;
		if (!pShaderGroup->Create(pszString,pszTexPath,dwTextureProcision))
		{
			OutputConsole("error: render::Layer3D CreateShaderGroup() failed. Create Failed.\"%s\".\n",pszString);
			MessageBox(NULL,pszString,"Faild",MB_OK);
			ASSERT(0);
			return 0xFFFFFFFF;
		}

		m_pShaderGroupArray[dwShaderGroupIndex] = pShaderGroup;

		m_dwShaderGroupArrayUsage ++;
		return dwShaderGroupIndex;
	}

	DWORD render::Interface::Layer3D::CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision)
	{
		//找空位，分配shader group
		DWORD dwShaderGroupIndex = 0;
		for(; dwShaderGroupIndex < MAX_SHADER_GROUP_ARRAY_SIZE; dwShaderGroupIndex++)
		{
			if (!m_pShaderGroupArray[dwShaderGroupIndex]) 
				break;
		}

		if (dwShaderGroupIndex == MAX_SHADER_GROUP_ARRAY_SIZE)  
		{
			OutputConsole("error: render::Layer3D CreateShaderGroup() failed. ShaderGroup cache overflow.");
			ASSERT(0);
			return 0xFFFFFFFF;
		}

		ShaderGroup *pShaderGroup = new ShaderGroup;
		if (pFile==NULL || !pShaderGroup->Create(pFile,dwTextureProcision))
		{
			OutputConsole("error: render::Layer3D CreateShaderGroup() failed. Create Failed..\n");
			MessageBox(NULL,"","Faild",MB_OK);
			ASSERT(0);
			return 0xFFFFFFFF;
		}

		m_pShaderGroupArray[dwShaderGroupIndex] = pShaderGroup;
		m_dwShaderGroupArrayUsage ++;
		return dwShaderGroupIndex;
	}
	// ----------------------------------------------------
	// 销毁ShaderGroup
	// 调用前确保渲染句柄正确
	// 
	// ----------------------------------------------------
	void render::Interface::Layer3D::DestroyShaderGroup(DWORD dwSGHandle)
	{
		if(dwSGHandle >= 0 && dwSGHandle < MAX_SHADER_GROUP_ARRAY_SIZE)
		{
			SAFEDESTROY(m_pShaderGroupArray[dwSGHandle]);
			m_dwShaderGroupArrayUsage--;
		}
	}



	/*
	* 功能: 创建动态着色变量,并将其添加到对应的纹理映射表中
	* 摘要: -
	* 参数: dwShaderGroupHandle - 渲染组句柄
	*		 dwSrcShaderIndex	 - 渲染模式
	*		 pNewShaderState	 - 动态渲染变量
	* 返回值: 创建成功,返回创建好的动态着色变量指针,创建失败返回空指针
	* 作者: lpf
	* 创建日期: 2008.01.23
	* 修改日志:
	*	2008.01.24 - lpf
	*		在创建动态Shader时,对同一名称的动态Shader的情况,不予动态增加,仅返回既有的动态Shader指针,从而达到一定的优化效果
	*/
	render::Interface::Layer3D::ShaderGroup::_tagShader * render::Interface::Layer3D::CreateDynamicShader(DWORD dwShaderGroupHandle, DWORD dwSrcShaderIndex, const render::Interface::Layer3D::_tagShaderState * pNewShaderState)
	{
		if (dwShaderGroupHandle == 0xffffffff)
			return NULL;

		ShaderGroup * pShaderGroup = m_pShaderGroupArray[dwShaderGroupHandle];
		ASSERT(pShaderGroup);

		render::Interface::Layer3D::ShaderGroup::_tagShader * pSrcShader = pShaderGroup->GetShader(dwSrcShaderIndex);
		if (!pSrcShader)
		{
			return NULL;
		}

		// 添加动态Shader到纹理映射中
		for(DWORD dw = 0; dw < pShaderGroup->m_dwTexCount[1]; ++dw)
		{
			render::Interface::Layer3D::ShaderGroup::_tagTex1	* pTex1		 = &(pShaderGroup->m_pTex1Array[dw]);
			render::Interface::Layer3D::ShaderGroup::_tagTex0	* pTex0		 = pTex1->pTex0Head;
			render::Interface::Layer3D::ShaderGroup::_tagShader * pTmpShader = NULL;

			while(pTex0)
			{
				if (pTex0->dwTex0Handle == pShaderGroup->m_pdwTexHandleArray[0][pSrcShader->tLayer[0].dwTexture])
				{
					// 循环查找是否已创建了该Shader
					pTmpShader = pTex0->pDynamicShaderHead;
					while (pTmpShader)
					{
						if (strcmp(pNewShaderState->szShaderName, pTmpShader->szName) == 0)
							break;

						pTmpShader = pTmpShader->pNext;
					}

					// 如果找到则直接返回既有的Shader指针
					if (pTmpShader)
						return pTmpShader;

					// 如果没有找到,则添加到动态链表头上
					render::Interface::Layer3D::ShaderGroup::_tagShader * pNewShader = new render::Interface::Layer3D::ShaderGroup::_tagShader;
					memcpy(pNewShader, pSrcShader, sizeof(render::Interface::Layer3D::ShaderGroup::_tagShader));
					memcpy(pNewShader->szName, pNewShaderState->szShaderName, 255);
					pNewShader->szName[255] = 0;
					pNewShader->pTriangleAlphaTestHead   = NULL;
					pNewShader->pTriangleAlphaBlendHead  = NULL;
					pNewShader->pTriangleTransparentHead = NULL;

					if (pNewShaderState->dwStreamType != 0xffffffff)
						pNewShader->dwStreamType = pNewShaderState->dwStreamType;

					if (pNewShaderState->dwTwoSide != 0xffffffff)
						pNewShader->bTwoSide = (BOOL)pNewShaderState->dwTwoSide;

					if (pNewShaderState->dwAlphaTestMode != 0xffffffff)
						pNewShader->dwAlphaTestMode = pNewShaderState->dwAlphaTestMode;

					if (pNewShaderState->dwAlphaBlendMode != 0xffffffff)
						pNewShader->dwAlphaBlendMode = pNewShaderState->dwAlphaBlendMode;

					if (pNewShaderState->dwColorOp != 0xffffffff)
						pNewShader->tLayer[0].dwColorOp = pNewShaderState->dwColorOp;

					if (pNewShaderState->dwColorArg1 != 0xffffffff)
						pNewShader->tLayer[0].dwColorArg1 = pNewShaderState->dwColorArg1;

					if (pNewShaderState->dwColorArg2 != 0xffffffff)
						pNewShader->tLayer[0].dwColorArg2 = pNewShaderState->dwColorArg2;

					if (pNewShaderState->dwAlphaOp != 0xffffffff)
						pNewShader->tLayer[0].dwAlphaOp = pNewShaderState->dwAlphaOp;

					if (pNewShaderState->dwAlphaArg1 != 0xffffffff)
						pNewShader->tLayer[0].dwAlphaArg1 = pNewShaderState->dwAlphaArg1;

					if (pNewShaderState->dwAlphaArg2 != 0xffffffff)
						pNewShader->tLayer[0].dwAlphaArg2 = pNewShaderState->dwAlphaArg2;

					pNewShader->pNext		  = pTex0->pDynamicShaderHead;
					pTex0->pDynamicShaderHead = pNewShader;
					return pNewShader;
				}
				pTex0 = pTex0->pNext;
			}
		}

		return NULL;
	}



	void render::Interface::Layer3D::Dump(void)
	{
		for(DWORD n = 0; n < MAX_SHADER_GROUP_ARRAY_SIZE; n ++)
		{
			ShaderGroup *pShaderGroup = m_pShaderGroupArray[n];
			if (pShaderGroup)
				pShaderGroup->Dump();
		}
	}
	void render::Interface::Layer3D::RendAlphaTestlist()
	{

		render::Interface * pInterface = render::Interface::GetInstance();
		Layer3D			  * pLayer3D   = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9	pDevice    = pInterface->GetDevice();
		RendMesh* prendmesh;
		list<RendMesh*>::iterator it;
		pLayer3D->SetVertexDecla(EFFECTTYPE_USEPOS,true);
		pLayer3D->SetColorAndAlphaOp(0,0,0,0,0,0,true);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pDevice->SetRenderState(D3DRS_FOGENABLE,m_bFog);


		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 						
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);

		it = m_RendMeshalphatestnonelist.begin();
		for (;it != m_RendMeshalphatestnonelist.end();it++)
		{
			prendmesh = (*it);
			prendmesh->Render();
		}
		m_RendMeshalphatestnonelist.clear();

#ifdef MAP_EDITOR
		pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		it = m_RendMeshalphatesttransparentlist.begin();
		for (;it != m_RendMeshalphatesttransparentlist.end();it++)
		{
			prendmesh = (*it);
			prendmesh->Render();
		}
		m_RendMeshalphatesttransparentlist.clear();
	}
	void render::Interface::Layer3D::RendAlphaBlendlowlist()
	{
		render::Interface * pInterface = render::Interface::GetInstance();
		Layer3D			  * pLayer3D   = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9	pDevice    = pInterface->GetDevice();
		pLayer3D->SetVertexDecla(EFFECTTYPE_USEPOS,true);
		pLayer3D->SetColorAndAlphaOp(0,0,0,0,0,0,true);
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pDevice->SetRenderState(D3DRS_FOGENABLE,m_bFog);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		RendMesh* prendmesh;
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		list<RendMesh*>::reverse_iterator rit;
		//alpha 最底层4
		rit = m_RendMeshalphablendalphalow4list.rbegin();
		for (;rit != m_RendMeshalphablendalphalow4list.rend();rit++)
		{
			prendmesh = (*rit);
			prendmesh->Render();
		}
		m_RendMeshalphablendalphalow4list.clear();

		//alpha 最底层3
		rit = m_RendMeshalphablendalphalow3list.rbegin();
		for (;rit != m_RendMeshalphablendalphalow3list.rend();rit++)
		{
			prendmesh = (*rit);
			prendmesh->Render();
		}
		m_RendMeshalphablendalphalow3list.clear();

		//alpha 最底层2
		rit = m_RendMeshalphablendalphalow2list.rbegin();
		for (;rit != m_RendMeshalphablendalphalow2list.rend();rit++)
		{
			prendmesh = (*rit);
			prendmesh->Render();
		}
		m_RendMeshalphablendalphalow2list.clear();

		//alpha 最底层1
		rit = m_RendMeshalphablendalphalow1list.rbegin();
		for (;rit != m_RendMeshalphablendalphalow1list.rend();rit++)
		{
			prendmesh = (*rit);
			prendmesh->Render();
		}
		m_RendMeshalphablendalphalow1list.clear();
		//alpha 最底层0
		rit = m_RendMeshalphablendalphalowlist.rbegin();
		for (;rit != m_RendMeshalphablendalphalowlist.rend();rit++)
		{
			prendmesh = (*rit);
			prendmesh->Render();
		}
		m_RendMeshalphablendalphalowlist.clear();

	}
	void render::Interface::Layer3D::RendAlphaBlendlist()
	{
		render::Interface * pInterface = render::Interface::GetInstance();
		Layer3D			  * pLayer3D   = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9	pDevice    = pInterface->GetDevice();
		pLayer3D->SetVertexDecla(EFFECTTYPE_USEPOS,true);
		pLayer3D->SetColorAndAlphaOp(0,0,0,0,0,0,true);
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pDevice->SetRenderState(D3DRS_FOGENABLE,m_bFog);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		RendMesh* prendmesh;
		ShaderGroup::_tagShader* pshader;
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		list<RendMesh*>::reverse_iterator rit = m_RendMeshalphablendalphalist.rbegin();
		//alpha 层
		rit = m_RendMeshalphablendalphalist.rbegin();
		for (;rit != m_RendMeshalphablendalphalist.rend();rit++)
		{
			prendmesh = (*rit);
			prendmesh->Render();
		}
		m_RendMeshalphablendalphalist.clear();

		EndEffectsState();

		list<ShaderGroup::_tagShader*>::reverse_iterator srit = m_shaderalphablendalphalist.rbegin();
		srit = m_shaderalphablendalphalist.rbegin();
		for (;srit != m_shaderalphablendalphalist.rend();srit++)
		{
			pshader = (*srit);
			pshader->Rend(RENDTYPE_BLENDALPHA);
			pshader->rendbuffer.Clear(RENDTYPE_BLENDALPHA);
		}
		m_shaderalphablendalphalist.clear();

		pLayer3D->SetVertexDecla(EFFECTTYPE_USEPOS,true);
		pLayer3D->SetColorAndAlphaOp(0,0,0,0,0,0,true);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		//BOOL bFogEnable;
		//pDevice->GetRenderState(D3DRS_FOGENABLE, (DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);	

		list<RendMesh*>::iterator it = m_RendMeshalphablendaddtivelist.begin();
		for (;it != m_RendMeshalphablendaddtivelist.end();it++)
		{
			prendmesh = (*it);
			prendmesh->Render();
		}
		m_RendMeshalphablendaddtivelist.clear();

		EndEffectsState();

		list<ShaderGroup::_tagShader*>::iterator sit = m_shaderalphablendaddtivelist.begin();
		for (;sit != m_shaderalphablendaddtivelist.end();sit++)
		{
			pshader = (*sit);
			pshader->Rend(RENDTYPE_BLENDADDITIVE);
			pshader->rendbuffer.Clear(RENDTYPE_BLENDADDITIVE);
		}
		m_shaderalphablendaddtivelist.clear();

		pLayer3D->SetVertexDecla(EFFECTTYPE_USEPOS,true);
		pLayer3D->SetColorAndAlphaOp(0,0,0,0,0,0,true);
		pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pDevice->SetRenderState(D3DRS_FOGENABLE,m_bFog);

		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		rit = m_RendMeshtransparentlist.rbegin();
		for (;rit != m_RendMeshtransparentlist.rend();rit++)
		{
			prendmesh = (*rit);
			prendmesh->Render(1);
		}
		m_RendMeshtransparentlist.clear();
	}


	/*
	* 功能: 3D层统一渲染函数
	* 摘要: 多边形缓冲中记录的需要渲染的三角形会在此被统一渲染出来.
	*		 渲染方式包括:根据渲染组的配置好的着色信息进行渲染,然后渲染动态着色信息相关的多边形
	* 参数: -
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.01.23 - lpf
	*		对每一种渲染模式都增加了对动态着色变量链的渲染
	*	2008.05.26 - lpf
	*		增加了根据当前渲染层纹理过滤状态进行绘制的功能
	*/
	void render::Interface::Layer3D::Render(void)
	{

		render::Interface * pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9	pDevice	   = pInterface->GetDevice();

		pDevice->SetRenderState(D3DRS_FILLMODE, m_dwFillMode);

		// Setup texture filter
		pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, m_eFilterType);
		if (m_bMipMap)
		{
			pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
		}
		else
		{
			pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);
		}

		/*pDevice->SetStreamSource(0, m_pVB_POSITION, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1, m_pVB_NORMAL, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2, m_pVB_COLOR_DIFFUSE, 0, sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3, m_pVB_TEX0, 0, sizeof(D3DXVECTOR2));*/
		//pDevice->SetIndices(m_pIB);
		//pDevice->SetIndices(NULL);
		// Enable specular
		pDevice->SetRenderState(D3DRS_SPECULARENABLE,FALSE);
		//pDevice->SetRenderState(D3DRS_AMBIENT, m_Ambinet);
		RendAlphaTestlist();
		//BOOL bFogEnable;
		//pDevice->GetRenderState(D3DRS_FOGENABLE, (DWORD*)&bFogEnable);
		//pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);	
		RendAlphaBlendlist();
		pDevice->SetStreamSource(0, NULL, 0, 0);
		pDevice->SetStreamSource(1, NULL, 0, 0);
		pDevice->SetStreamSource(2, NULL, 0, 0);
		pDevice->SetStreamSource(3, NULL, 0, 0);
		pDevice->SetIndices(NULL);

		//pDevice->SetRenderState(D3DRS_FOGENABLE, bFogEnable);
		//pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);
	}
	void render::Interface::Layer3D::RenderTest(void)
	{

		render::Interface * pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9	pDevice	   = pInterface->GetDevice();

		pDevice->SetRenderState(D3DRS_FILLMODE, m_dwFillMode);

		// Setup texture filter
		pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, m_eFilterType);
		pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, m_eFilterType);
		pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, m_eFilterType);
		if (m_bMipMap)
		{
			pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
		}
		else
		{
			pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);
		}

		/*pDevice->SetStreamSource(0, m_pVB_POSITION, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1, m_pVB_NORMAL, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2, m_pVB_COLOR_DIFFUSE, 0, sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3, m_pVB_TEX0, 0, sizeof(D3DXVECTOR2));*/
		//pDevice->SetIndices(m_pIB);
		pDevice->SetIndices(NULL);
		// Enable specular
		pDevice->SetRenderState(D3DRS_SPECULARENABLE,FALSE);
		//pDevice->SetRenderState(D3DRS_AMBIENT, m_Ambinet);
		RendAlphaTestlist();
	}
	void render::Interface::Layer3D::RenderBlend(void)
	{
		render::Interface * pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9	pDevice	   = pInterface->GetDevice();
		RendAlphaBlendlist();
		pDevice->SetStreamSource(0, NULL, 0, 0);
		pDevice->SetStreamSource(1, NULL, 0, 0);
		pDevice->SetStreamSource(2, NULL, 0, 0);
		pDevice->SetStreamSource(3, NULL, 0, 0);
		pDevice->SetIndices(NULL);
	}
	void render::Interface::Layer3D::RenderBlendLow(void)
	{

		render::Interface * pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9	pDevice	   = pInterface->GetDevice();
		RendAlphaBlendlowlist();
	}

	void render::Interface::Layer3D::EndEffectsState()
	{
		m_EffectManager.EndEffectandPass();
	}
	void render::Interface::Layer3D::Clear(void)
	{
		//清空多边形渲染缓冲
		m_EffectManager.EndEffectandPass();
		m_dwPolygonArrayUsage = 0;
	}

	void render::Interface::Layer3D::Reset(void)
	{
		DestroyVertexDecl();
		DestroyVertexBuffer();
		CreateVertexDecl();
		CreateVertexBuffer();
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,bool btex01)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		D3DXVECTOR3 *pCachePos(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBUV0  = m_pVB_TEX0;
		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		pVBUV0->Lock( 0,0, (void**)&pCacheUV0,D3DLOCK_DISCARD);
		if (btex01)
		{
			pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSTEX0TEX1]);



			//
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2) * 2);





			memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
			memcpy(pCacheUV0 ,pUV0,sizeof(D3DXVECTOR2) * 6 * dwCount);
		}
		else
		{
			pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSTEX0]);



			//
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));





			memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
			memcpy(pCacheUV0 ,pUV0,sizeof(D3DXVECTOR2) * 3 * dwCount);
		}


		pVBPOS->Unlock();
		pVBUV0->Unlock();

		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);

	}
   	/*
	 * 功能: 绘制三角形Strips
	 * 摘要: 双重纹理
	 * 参数: -
	 * 返回值: -
	 * 作者: lpf
	 * 创建日期: 2009.10.20
	 * 修改日志:
	 */
	void render::Interface::Layer3D::DrawTriangleStrips(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNor, const D3DCOLOR * pCol, const D3DXVECTOR2 * pUV2)
	{
		render::Interface * pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost() || !dwCount)
			return;
		if (dwCount + 2 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		D3DXVECTOR3 * pCachePos(NULL);
		D3DXVECTOR3 * pCacheNor(NULL);
		D3DXVECTOR3 * pCacheCol(NULL);
		D3DXVECTOR2 * pCacheUV0(NULL);

		// 填充VB进行绘图
		m_pVB_POSITION->Lock(0, 0, (void **)&pCachePos, D3DLOCK_DISCARD);
		m_pVB_NORMAL->Lock(0, 0, (void **)&pCacheNor, D3DLOCK_DISCARD);
		m_pVB_COLOR_DIFFUSE->Lock(0, 0, (void **)&pCacheCol, D3DLOCK_DISCARD);
		m_pVB_TEX0->Lock(0, 0, (void **)&pCacheUV0, D3DLOCK_DISCARD);

		memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * (dwCount + 2));
		memcpy(pCacheNor, pNor, sizeof(D3DXVECTOR3) * (dwCount + 2));
		memcpy(pCacheCol, pCol, sizeof(D3DCOLOR) * (dwCount + 2));
		memcpy(pCacheUV0, pUV2, sizeof(D3DXVECTOR2) * (dwCount + 2) * 2);

		m_pVB_POSITION->Unlock();
		m_pVB_NORMAL->Unlock();
		m_pVB_COLOR_DIFFUSE->Unlock();
		m_pVB_TEX0->Unlock();

		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSNORDIFTEX0TEX1]);
		pDevice->SetStreamSource(0, m_pVB_POSITION, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1, m_pVB_NORMAL, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2, m_pVB_COLOR_DIFFUSE, 0, sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3, m_pVB_TEX0, 0, sizeof(D3DXVECTOR2) << 1);

		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, dwCount);

		pDevice->SetStreamSource(0, NULL, 0, 0);
		pDevice->SetStreamSource(1, NULL, 0, 0);
		pDevice->SetStreamSource(2, NULL, 0, 0);
		pDevice->SetStreamSource(3, NULL, 0, 0);
	}
   	/*
	 * 功能: 绘制三角形扇
	 * 摘要: 双重纹理
	 * 参数: -
	 * 返回值: -
	 * 作者: lpf
	 * 创建日期: 2010.04.13
	 * 修改日志:
	 */
	void render::Interface::Layer3D::DrawTriangleFan(DWORD dwNumVertex,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNor, const D3DCOLOR * pCol, const D3DXVECTOR2 * pUV2)
	{
		render::Interface * pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost() || !dwNumVertex)
			return;
		if (dwNumVertex - 2 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		D3DXVECTOR3 * pCachePos(NULL);
		D3DXVECTOR3 * pCacheNor(NULL);
		D3DXVECTOR3 * pCacheCol(NULL);
		D3DXVECTOR2 * pCacheUV0(NULL);

		// 填充VB进行绘图
		m_pVB_POSITION->Lock(0, 0, (void **)&pCachePos, D3DLOCK_DISCARD);
		m_pVB_NORMAL->Lock(0, 0, (void **)&pCacheNor, D3DLOCK_DISCARD);
		m_pVB_COLOR_DIFFUSE->Lock(0, 0, (void **)&pCacheCol, D3DLOCK_DISCARD);
		m_pVB_TEX0->Lock(0, 0, (void **)&pCacheUV0, D3DLOCK_DISCARD);

		memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * dwNumVertex);
		memcpy(pCacheNor, pNor, sizeof(D3DXVECTOR3) * dwNumVertex);
		memcpy(pCacheCol, pCol, sizeof(D3DCOLOR) * dwNumVertex);
		memcpy(pCacheUV0, pUV2, sizeof(D3DXVECTOR2) * dwNumVertex * 2);

		m_pVB_POSITION->Unlock();
		m_pVB_NORMAL->Unlock();
		m_pVB_COLOR_DIFFUSE->Unlock();
		m_pVB_TEX0->Unlock();

		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSNORDIFTEX0TEX1]);
		pDevice->SetStreamSource(0, m_pVB_POSITION, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1, m_pVB_NORMAL, 0, sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2, m_pVB_COLOR_DIFFUSE, 0, sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3, m_pVB_TEX0, 0, sizeof(D3DXVECTOR2) << 1);

		pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, dwNumVertex - 2);

		pDevice->SetStreamSource(0, NULL, 0, 0);
		pDevice->SetStreamSource(1, NULL, 0, 0);
		pDevice->SetStreamSource(2, NULL, 0, 0);
		pDevice->SetStreamSource(3, NULL, 0, 0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwCount,const D3DXVECTOR3 *pPos)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();


		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR   *pCacheCol(NULL);

		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		//pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		//pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);

		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		//memcpy(pCacheCol ,pColor,sizeof(DWORD) * 3 * dwCount);

		pVBPOS->Unlock();
		//pVBCOL->Unlock();



		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);

		pDevice->SetStreamSource(0,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwCount,const D3DXVECTOR3 *pPos,D3DXMATRIX matpos,D3DXMATRIX matproj)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();


		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR   *pCacheCol(NULL);

		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		//pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		//pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);

		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		//memcpy(pCacheCol ,pColor,sizeof(DWORD) * 3 * dwCount);

		pVBPOS->Unlock();
		//pVBCOL->Unlock();

		DWORD dweffect = EFFECTTYPE_USEPOS|EFFECTTYPE_HWMAXMAT|EFFECTTYPE_TEXPROJECTED;
		DWORD dwmacro  = EFFECTTYPE_USEPOS; 
		SetVertexDecla(dwmacro,true);
		GetEffectManager()->CreateEffectFile(dweffect);
		GetEffectManager()->SetEffectandPass(dweffect,0,0);
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		GetEffectManager()->GetNowEffects()->SetMatrix("VPMatrix",&(matpos));
		GetEffectManager()->GetNowEffects()->SetMatrixArray("WVPMatrix",&mat,1);
		GetEffectManager()->GetNowEffects()->SetMatrix("TPVPMatrix",&matproj);
		GetEffectManager()->UpdateChanges();

		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);

		GetEffectManager()->EndEffectandPass();
		pDevice->SetVertexShader(NULL);
		pDevice->SetStreamSource(0,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}

		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		//pInterface->SetTexture(1,NULL);
		/*D3DVERTEXELEMENT9 shaderDecl[] =
		{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};*/
		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POS]);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR   *pCacheCol(NULL);

		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		//pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		//pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);

		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		//memcpy(pCacheCol ,pColor,sizeof(DWORD) * 3 * dwCount);

		pVBPOS->Unlock();
		//pVBCOL->Unlock();

		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
		pDevice->SetStreamSource(0,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,D3DXMATRIX matpos,D3DXMATRIX matproj)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}

		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);

		//pInterface->SetTexture(1,NULL);
		/*D3DVERTEXELEMENT9 shaderDecl[] =
		{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};*/


		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR   *pCacheCol(NULL);

		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		//pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		//pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);

		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		//memcpy(pCacheCol ,pColor,sizeof(DWORD) * 3 * dwCount);

		pVBPOS->Unlock();
		//pVBCOL->Unlock();
		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POS]);
		DWORD dweffect = EFFECTTYPE_USEPOS|EFFECTTYPE_HWMAXMAT|EFFECTTYPE_TEXPROJECTED;
		DWORD dwmacro  = EFFECTTYPE_USEPOS; 
		SetVertexDecla(dwmacro,true);
		GetEffectManager()->CreateEffectFile(dweffect);
		GetEffectManager()->SetEffectandPass(dweffect,0,0);
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		GetEffectManager()->GetNowEffects()->SetMatrix("VPMatrix",&(matpos));
		GetEffectManager()->GetNowEffects()->SetMatrixArray("WVPMatrix",&mat,1);
		GetEffectManager()->GetNowEffects()->SetMatrix("TPVPMatrix",&matproj);
		GetEffectManager()->UpdateChanges();
		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
		GetEffectManager()->EndEffectandPass();
		pDevice->SetVertexShader(NULL);
		pDevice->SetStreamSource(0,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);

		if (!pShaderGroup)
		{
			return;
		}

		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		//pInterface->SetTexture(1,NULL);
		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSCOLTEX0]);


		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
		//pDevice->SetStreamSource(2,m_pVB_COLOR_SPECULAR,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC  = m_pVB_COLOR_SPECULAR;
		LPDIRECT3DVERTEXBUFFER9 pVBUV0  = m_pVB_TEX0;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);
		pVBUV0->Lock( 0,0, (void**)&pCacheUV0,D3DLOCK_DISCARD);

		memcpy(pCacheCol ,pColor,sizeof(DWORD) * 3 * dwCount);
		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		memcpy(pCacheUV0 ,pUV0,sizeof(D3DXVECTOR2) * 3 * dwCount);

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pVBUV0->Unlock();

		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor,D3DXMATRIX matpos,D3DXMATRIX matproj)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);

		if (!pShaderGroup)
		{
			return;
		}

		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		//pInterface->SetTexture(1,NULL);



		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
		pDevice->SetStreamSource(4,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC  = m_pVB_COLOR_SPECULAR;
		LPDIRECT3DVERTEXBUFFER9 pVBUV0  = m_pVB_TEX0;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);
		pVBUV0->Lock( 0,0, (void**)&pCacheUV0,D3DLOCK_DISCARD);

		memcpy(pCacheCol ,pColor,sizeof(DWORD) * 3 * dwCount);
		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		memcpy(pCacheUV0 ,pUV0,sizeof(D3DXVECTOR2) * 3 * dwCount);

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pVBUV0->Unlock();

		DWORD dweffect = EFFECTTYPE_USEPOS|EFFECTTYPE_USEUV|EFFECTTYPE_USECOLOR|EFFECTTYPE_HWMAXMAT;
		DWORD dwmacro    = EFFECTTYPE_USEPOS|EFFECTTYPE_USEUV|EFFECTTYPE_USECOLOR;
		SetVertexDecla(dwmacro,true);
		GetEffectManager()->CreateEffectFile(dweffect);
		GetEffectManager()->SetEffectandPass(dweffect,0,0);
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		GetEffectManager()->GetNowEffects()->SetMatrix("VPMatrix",&(matpos * matproj));
		GetEffectManager()->GetNowEffects()->SetMatrixArray("WVPMatrix",&mat,1);
		GetEffectManager()->UpdateChanges();
		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
		GetEffectManager()->EndEffectandPass();
		pDevice->SetVertexShader(NULL);
		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor,WORD* pindex,D3DXMATRIX matpos,D3DXMATRIX matproj)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);

		if (!pShaderGroup)
		{
			return;
		}

		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		//pInterface->SetTexture(1,NULL);



		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);
		WORD        *pwIndex;

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
		pDevice->SetStreamSource(4,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC  = m_pVB_COLOR_SPECULAR;
		LPDIRECT3DVERTEXBUFFER9 pVBUV0  = m_pVB_TEX0;
		LPDIRECT3DINDEXBUFFER9  pIB     = m_pIB;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);
		pVBUV0->Lock( 0,0, (void**)&pCacheUV0,D3DLOCK_DISCARD);
		pIB->Lock(0,0, (void**)&pwIndex,D3DLOCK_DISCARD);

		memcpy(pCacheCol ,pColor,sizeof(DWORD) * 3 * dwCount);
		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		memcpy(pCacheUV0 ,pUV0,sizeof(D3DXVECTOR2) * 3 * dwCount);
		memcpy(pwIndex ,pindex,sizeof(WORD) * 3 * dwCount);

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pVBUV0->Unlock();

		DWORD dweffect = EFFECTTYPE_USEPOS|EFFECTTYPE_USEUV|EFFECTTYPE_USECOLOR|EFFECTTYPE_HWMAXMAT;
		DWORD dwmacro    = EFFECTTYPE_USEPOS|EFFECTTYPE_USEUV|EFFECTTYPE_USECOLOR;
		SetVertexDecla(dwmacro,true);
		GetEffectManager()->CreateEffectFile(dweffect);
		GetEffectManager()->SetEffectandPass(dweffect,0,0);
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		GetEffectManager()->GetNowEffects()->SetMatrix("VPMatrix",&(matpos * matproj));
		GetEffectManager()->GetNowEffects()->SetMatrixArray("WVPMatrix",&mat,1);
		GetEffectManager()->UpdateChanges();
		pDevice->SetIndices(m_pIB);
		//pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,3 * dwCount,0,dwCount);

		GetEffectManager()->EndEffectandPass();
		pDevice->SetVertexShader(NULL);
		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArrayByShader(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,DWORD dwIndexcount,const D3DXVECTOR3 *pPos,const D3DXVECTOR3 *pNormal,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor,const WORD* pindexbuffer,BOOL blight,DWORD dwMode)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount == 0)
		{
			return;
		}
		if (dwCount > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}
		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}
		if (dwMode == 0)
		{
			//render opaque mode : mask and none
			pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
			pDevice->SetRenderState( D3DRS_ZENABLE, TRUE);	

			pInterface->Cull(!pShader->bTwoSide);
			//0 test

			if (!pShader->dwAlphaTestMode)
			{
				//test + none
				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE); 						
				pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
				pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
			}
			else
			{
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
				//test + transparent
				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
				//pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
				//pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
				pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
				pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
			}

			pShaderGroup->_SetTextureOp(pShader);
		}
		else//混合
		{
			pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
			pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);	
			pInterface->Cull(!pShader->bTwoSide);
			//设置Alpha blend mode
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

			//blend
			switch(pShader->dwAlphaBlendMode)
			{
			case 0://blend + additive
				{
					pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
					pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
				}
				break;
			case 1://blend + alpha
				{
					pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
					pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
				}
				break;
			case 2://blend + color
				{
					pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
					pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
				}
				break;
			case 3://blend + reserved
				{
				}
				break;
			}
			//设置颜色操作
			pShaderGroup->_SetTextureOp(pShader);

		}

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		//pInterface->SetTexture(1,NULL);

		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSCOLTEX0]);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);
		WORD   *pCacheIB(NULL);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
		//pDevice->SetStreamSource(2,m_pVB_COLOR_SPECULAR,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
		//pDevice->SetIndices(m_pIB);

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC  = m_pVB_COLOR_SPECULAR;
		LPDIRECT3DVERTEXBUFFER9 pVBUV0  = m_pVB_TEX0;
		LPDIRECT3DINDEXBUFFER9  pIB  = m_pIB;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		pVBCOL->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);
		pVBUV0->Lock( 0,0, (void**)&pCacheUV0,D3DLOCK_DISCARD);

		memcpy(pCacheCol ,pColor,sizeof(DWORD) * dwCount);
		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * dwCount);
		memcpy(pCacheUV0 ,pUV0,sizeof(D3DXVECTOR2) * dwCount);

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pVBUV0->Unlock();

		/*pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);
		memcpy(pCacheIB ,pindexbuffer,sizeof(WORD) * dwIndexcount);
		pIB->Unlock();
		pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0,dwCount,0,dwIndexcount / 3);*/

		pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwIndexcount / 3);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(2,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}
	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR3 *pNormal,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (dwCount  *  3 > LAYER3D_DRAWVERTEXBUFFER_SIZE)
		{
			PutDebugString("绘制顶点数超过BUFFER长度");
			return;
		}
		if (!dwCount)
		{
			return;
		}

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);

		if (!pShaderGroup)
		{
			return;
		}

		ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(dwShaderIndex);
		if (!pShader)
		{
			return;
		}

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		//pInterface->SetTexture(1,NULL);

		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSNMLCOLTEX0]);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DXVECTOR3 *pCacheNormal(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_NORMAL,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(2,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBNORMAL  = m_pVB_NORMAL;
		LPDIRECT3DVERTEXBUFFER9 pVBDIFFUSE  = m_pVB_COLOR_DIFFUSE;
		LPDIRECT3DVERTEXBUFFER9 pVBUV0  = m_pVB_TEX0;

		//填充VB进行绘图

		pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
		pVBNORMAL->Lock( 0,0, (void**)&pCacheNormal,D3DLOCK_DISCARD);
		pVBDIFFUSE->Lock( 0,0, (void**)&pCacheCol,D3DLOCK_DISCARD);
		pVBUV0->Lock( 0,0, (void**)&pCacheUV0,D3DLOCK_DISCARD);

		memcpy(pCachePos ,pPos,sizeof(D3DXVECTOR3) * 3 * dwCount);
		memcpy(pCacheNormal ,pNormal,sizeof(D3DXVECTOR3) * 3 * dwCount);
		memcpy(pCacheCol ,pColor,sizeof(D3DCOLOR) * 3 * dwCount);
		memcpy(pCacheUV0 ,pUV0,sizeof(D3DXVECTOR2) * 3 * dwCount);

		pVBPOS->Unlock();
		pVBNORMAL->Unlock();
		pVBDIFFUSE->Unlock();
		pVBUV0->Unlock();

		pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
		pDevice->SetStreamSource(2,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}
	void render::Interface::Layer3D::SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColorDiffuse,const D3DCOLOR *pColorSpecular,DWORD dwMode)
	{
		if(dwShaderGroup == 0xFFFFFFFF)	return;

		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}
		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);

		//分配N个element
		DWORD dwWantSize = m_dwPolygonArrayUsage + dwCount;
		if (dwWantSize >= MAX_POLYGON_ARRAY_SIZE)
		{
			OutputConsole("error: render::Layer3D triangle cache overflow [%d]!\n",m_dwPolygonArrayUsage);
			return;//over flow
		}

		//setup polygon
		for(DWORD dwOffset = 0; dwOffset < dwCount * 3; dwOffset += 3)
		{
			_tagTriangle &tTriangle = m_pPolygonArray[m_dwPolygonArrayUsage++];
			memcpy(tTriangle.pos,&pPos[dwOffset],sizeof(D3DXVECTOR3) * 3);
			memcpy(tTriangle.uv0,&pUV0[dwOffset],sizeof(D3DXVECTOR2) * 3);
			memcpy(tTriangle.color,&pColorDiffuse[dwOffset],sizeof(D3DCOLOR) * 3);
			memcpy(tTriangle.specular,&pColorSpecular[dwOffset],sizeof(D3DCOLOR) * 3);

			//添加到渲染表头
			switch(dwMode)
			{
			case 0://test
				tTriangle.pNext = pShader->pTriangleAlphaTestHead;
				pShader->pTriangleAlphaTestHead = &tTriangle;
				break;
			case 1://blend 
				tTriangle.pNext = pShader->pTriangleAlphaBlendHead;
				pShader->pTriangleAlphaBlendHead = &tTriangle;
				break;
				//case 2://top
				//	tTriangle.pNext = pShader->pTraingleAlphaBlendTopHead;
				//	pShader->pTraingleAlphaBlendTopHead = &tTriangle;
				//	break;			
			}
		}
	}

	void render::Interface::Layer3D::SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColorDiffuse,DWORD dwMode)
	{
		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}

		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);

		//分配N个element
		DWORD dwWantSize = m_dwPolygonArrayUsage + dwCount;
		if (dwWantSize >= MAX_POLYGON_ARRAY_SIZE)
		{
			OutputConsole("error: render::Layer3D triangle cache overflow [%d]!\n",m_dwPolygonArrayUsage);
			return;//over flow
		}

		//setup polygon
		for(DWORD dwOffset = 0; dwOffset < dwCount * 3; dwOffset += 3)
		{
			_tagTriangle &tTriangle = m_pPolygonArray[m_dwPolygonArrayUsage++];
			memcpy(tTriangle.pos,&pPos[dwOffset],sizeof(D3DXVECTOR3) * 3);
			memcpy(tTriangle.uv0,&pUV0[dwOffset],sizeof(D3DXVECTOR2) * 3);
			memcpy(tTriangle.color,&pColorDiffuse[dwOffset],sizeof(D3DCOLOR) * 3);
			ZeroMemory(tTriangle.specular,sizeof(D3DCOLOR) * 3);

			//添加到渲染表头
			switch(dwMode)
			{
			case 0://test
				tTriangle.pNext = pShader->pTriangleAlphaTestHead;
				pShader->pTriangleAlphaTestHead = &tTriangle;
				break;
			case 1://blend 
				tTriangle.pNext = pShader->pTriangleAlphaBlendHead;
				pShader->pTriangleAlphaBlendHead = &tTriangle;
				break;
			case 2://transparent
				tTriangle.pNext = pShader->pTriangleTransparentHead;
				pShader->pTriangleTransparentHead = &tTriangle;
				break;
				//case 2://top
				//	tTriangle.pNext = pShader->pTraingleAlphaBlendTopHead;
				//	pShader->pTraingleAlphaBlendTopHead = &tTriangle;
				//	break;			
			}
		}
	}

	void render::Interface::Layer3D::SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DCOLOR *pColorDiffuse,DWORD dwMode)
	{
		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}

		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);

		//分配N个element
		DWORD dwWantSize = m_dwPolygonArrayUsage + dwCount;
		if (dwWantSize >= MAX_POLYGON_ARRAY_SIZE)
		{
			OutputConsole("error: render::Layer3D triangle cache overflow [%d]!\n",m_dwPolygonArrayUsage);
			return;//over flow
		}

		//setup polygon
		for(DWORD dwOffset = 0; dwOffset < dwCount * 3; dwOffset += 3)
		{
			_tagTriangle &tTriangle = m_pPolygonArray[m_dwPolygonArrayUsage++];
			memcpy(tTriangle.pos,&pPos[dwOffset],sizeof(D3DXVECTOR3) * 3);
			memcpy(tTriangle.color,&pColorDiffuse[dwOffset],sizeof(D3DCOLOR) * 3);

			//添加到渲染表头
			switch(dwMode)
			{
			case 0://test
				tTriangle.pNext = pShader->pTriangleAlphaTestHead;
				pShader->pTriangleAlphaTestHead = &tTriangle;
				break;
			case 1://blend 
				tTriangle.pNext = pShader->pTriangleAlphaBlendHead;
				pShader->pTriangleAlphaBlendHead = &tTriangle;
				break;
				//case 2://top
				//	tTriangle.pNext = pShader->pTraingleAlphaBlendTopHead;
				//	pShader->pTraingleAlphaBlendTopHead = &tTriangle;
				//	break;			
			}
		}
	}
	void render::Interface::Layer3D::SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const _tagTriangle* ptriangle,DWORD dwMode)
	{
		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}

		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);

		//分配N个element
		DWORD dwWantSize = m_dwPolygonArrayUsage + dwCount;
		if (dwWantSize >= MAX_POLYGON_ARRAY_SIZE)
		{
			OutputConsole("error: render::Layer3D triangle cache overflow [%d]!\n",m_dwPolygonArrayUsage);
			return;//over flow
		}
		DWORD n = m_dwPolygonArrayUsage;
		memcpy(&m_pPolygonArray[m_dwPolygonArrayUsage],ptriangle,sizeof(_tagTriangle) * dwCount);
		m_dwPolygonArrayUsage += dwCount;
		//setup polygon
		for(DWORD dwOffset = 0; dwOffset < dwCount; dwOffset++,n++)
		{
			_tagTriangle &tTriangle = m_pPolygonArray[n];
			//memcpy(tTriangle.pos,&pPos[dwOffset],sizeof(D3DXVECTOR3) * 3);
			//memcpy(tTriangle.color,&pColorDiffuse[dwOffset],sizeof(D3DCOLOR) * 3);

			//添加到渲染表头
			switch(dwMode)
			{
			case 0://test
				tTriangle.pNext = pShader->pTriangleAlphaTestHead;
				pShader->pTriangleAlphaTestHead = &tTriangle;
				break;
			case 1://blend 
				tTriangle.pNext = pShader->pTriangleAlphaBlendHead;
				pShader->pTriangleAlphaBlendHead = &tTriangle;
				break;
			case 2://transparent
				tTriangle.pNext = pShader->pTriangleTransparentHead;
				pShader->pTriangleTransparentHead = &tTriangle;
				break;
				//case 2://top
				//	tTriangle.pNext = pShader->pTraingleAlphaBlendTopHead;
				//	pShader->pTraingleAlphaBlendTopHead = &tTriangle;
				//	break;			
			}
		}
	}
	void render::Interface::Layer3D::SendBufferToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount, DWORD dwIndexcount,const D3DXVECTOR3* pPos,const D3DXVECTOR3* pNormal,const D3DCOLOR* pDiffuse,const D3DXVECTOR2* pUV0,const WORD* pindexbuffer,BOOL blight,DWORD dwMode)
	{
		if (render::Interface::GetInstance()->GetDeviceLost())
		{
			return;
		}
		static WORD indexbuffer[LAYER3D_TEMPBUFFER_SIZE];
		if (dwCount == 0)
		{
			return;
		}
		/*if (dwIndexcount > LAYER3D_TEMPBUFFER_SIZE)
		{
		MessageBox(NULL,"SendBufferToCache临时BUFFER过小!","错误",MB_OK);
		}*/
		//const WORD* pidbuffer;
		m_dwPolygonArrayUsage += dwCount/3;
		ShaderGroup::_tagShader::RendBuffer::_tagBuffer* pbuffer = NULL;
		DWORD dwrendtype = RENDTYPE_TESTNONE;
		DWORD dwstreamtype = LMST_POSCOLTEX0;
		GetTagBuffer(pbuffer,dwrendtype,dwstreamtype,dwShaderGroup,dwShaderIndex,dwCount,blight,dwMode);
		if (pbuffer == NULL)
		{
			return;
		}
		ShaderGroup::_tagShader::RendBuffer::_tagBuffer& buffer = *pbuffer;
		if (blight)
		{
			buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
			if (!buffer.pnormal)
			{
				buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
				//MessageBox(NULL,"LOCK地址为空!","LOCK地址为空!",MB_OK);
				return;
			}
			else
			{
				memcpy(buffer.pnormal + buffer.vertexcount,pNormal,dwCount * sizeof(D3DXVECTOR3));
			}
		}
		buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_POS);
		buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
		buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_UV);
		if (!buffer.ppos||!buffer.pdiffuse||!buffer.puv)
		{
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_POS);
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_UV);
			//MessageBox(NULL,"LOCK地址为空!","LOCK地址为空!",MB_OK);
			return;
		}
		else
		{

			memcpy(buffer.ppos + buffer.vertexcount,pPos,dwCount * sizeof(D3DXVECTOR3));
			if (pDiffuse)
			{
				memcpy(buffer.pdiffuse + buffer.vertexcount,pDiffuse,dwCount * sizeof(D3DCOLOR));
			}
			if (pUV0)
			{
				memcpy(buffer.puv + buffer.vertexcount ,pUV0,dwCount * sizeof(D3DXVECTOR2));
			}

		}
		buffer.vertexcount += dwCount;
		buffer.indexcount += dwIndexcount;
		buffer.trianglecount += dwIndexcount/3;

	}
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer* render::Interface::Layer3D::SendDynamicBufferToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount, DWORD dwIndexcount,const D3DXVECTOR3* pPos,const D3DXVECTOR3* pNormal,const D3DCOLOR* pDiffuse,const D3DXVECTOR2* pUV0,const WORD* pindexbuffer,BOOL blight,DWORD dwMode)
	{
		//return NULL;
		if (render::Interface::GetInstance()->GetDeviceLost())
		{
			return NULL;
		}
		static WORD indexbuffer[LAYER3D_TEMPBUFFER_SIZE];
		/*if (dwIndexcount > LAYER3D_TEMPBUFFER_SIZE)
		{
		MessageBox(NULL,"SendBufferToCache临时BUFFER过小!","错误",MB_OK);
		}*/
		m_dwPolygonArrayUsage += dwCount/3;
		//const WORD* pidbuffer;
		ShaderGroup::_tagShader::RendBuffer::_tagBuffer* pbuffer = NULL;
		DWORD dwrendtype = RENDTYPE_TESTNONE;
		DWORD dwstreamtype = LMST_POSCOLTEX0;
		GetDynamicTagBuffer(pbuffer,dwrendtype,dwstreamtype,dwShaderGroup,dwShaderIndex,dwCount,blight,dwMode);
		if (pbuffer == NULL)
		{
			return NULL;
		}
		ShaderGroup::_tagShader::RendBuffer::_tagBuffer& buffer = *pbuffer;
		if (blight)
		{
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
			buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
			if (!buffer.pnormal)
			{
				//MessageBox(NULL,"LOCK地址为空!","LOCK地址为空!",MB_OK);
				buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
				return NULL;
			}
			else
			{
				memcpy(buffer.pnormal + buffer.vertexcount,pNormal,dwCount * sizeof(D3DXVECTOR3));
				buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_NORMAL);
			}
		}
		buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_POS);
		buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
		buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_UV);
		buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_POS);
		buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
		buffer.LockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_UV);
		if (!buffer.ppos||!buffer.pdiffuse||!buffer.puv)
		{
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_POS);
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_UV);
			//MessageBox(NULL,"LOCK地址为空!","LOCK地址为空!",MB_OK);
			return NULL;
		}
		else
		{
			memcpy(buffer.ppos + buffer.vertexcount,pPos,dwCount * sizeof(D3DXVECTOR3));
			memcpy(buffer.pdiffuse + buffer.vertexcount,pDiffuse,dwCount * sizeof(D3DCOLOR));
			memcpy(buffer.puv + buffer.vertexcount ,pUV0,dwCount * sizeof(D3DXVECTOR2));
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_POS);
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_DIFFUSE);
			buffer.UnLockBuffer(ShaderGroup::_tagShader::RendBuffer::_tagBuffer::BUFFERTYPE_UV);

		}
		buffer.vertexcount += dwCount;
		buffer.indexcount += dwIndexcount;
		buffer.trianglecount += dwIndexcount/3;
		return pbuffer;

	}
	void render::Interface::Layer3D::GetTagBuffer(Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer*& ptagbuffer,DWORD& dwrendtype,DWORD& dwstreamtype,DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,BOOL blight,DWORD dwMode)
	{
		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
		int* pusenum = NULL;
		ShaderGroup::_tagShader::RendBuffer::BufferList* bufferlist;
		vector<ShaderGroup::_tagShader::RendBuffer::_tagBuffer>* pbuffer = NULL;
		dwrendtype = RENDTYPE_TESTNONE;
		dwstreamtype = LMST_POSCOLTEX0;
		if (!pShader)
		{
			ptagbuffer = NULL;
			return;
		}
		//添加到渲染表
		switch(dwMode)
		{
		case 0://test
			if (!pShader->dwAlphaTestMode)
			{
				if (!pShader->rendbuffer.testnonebuffer.bpushed)
				{
					pLayer3D->m_shaderalphatestnonelist.push_back(pShader);
					pShader->rendbuffer.testnonebuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_TESTNONE;
				bufferlist = &pShader->rendbuffer.testnonebuffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}



			}
			else
			{
				if (!pShader->rendbuffer.testtransparentbuffer.bpushed)
				{
					pLayer3D->m_shaderalphatesttransparentlist.push_back(pShader);
					pShader->rendbuffer.testtransparentbuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_TESTTRANSPARENT;
				bufferlist = &pShader->rendbuffer.testtransparentbuffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}



			}

			break;
		case 1://blend 
			switch(pShader->dwAlphaBlendMode)
			{
			case 0:		//blend + additive
				if (!pShader->rendbuffer.blendadditivebuffer.bpushed)
				{
					pLayer3D->m_shaderalphablendaddtivelist.push_back(pShader);

					pShader->rendbuffer.blendadditivebuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDADDITIVE;
				bufferlist = &pShader->rendbuffer.blendadditivebuffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}
				break;
			case 1:		//blend + alpha
				if (!pShader->rendbuffer.blendalphabuffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalist.push_back(pShader);

					pShader->rendbuffer.blendalphabuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHA;
				bufferlist =  &pShader->rendbuffer.blendalphabuffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{	
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}
				break;
			case 2:		//blend + alphalow
				if (!pShader->rendbuffer.blendalphalowbuffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalowlist.push_back(pShader);

					pShader->rendbuffer.blendalphalowbuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW;
				bufferlist =  &pShader->rendbuffer.blendalphalowbuffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{	
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}
				break;
			case 3:		//blend + alphalow1
				if (!pShader->rendbuffer.blendalphalow1buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow1list.push_back(pShader);

					pShader->rendbuffer.blendalphalow1buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW1;
				bufferlist =  &pShader->rendbuffer.blendalphalow1buffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{	
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}
				break;
			case 4:		//blend + alphalow2
				if (!pShader->rendbuffer.blendalphalow2buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow2list.push_back(pShader);

					pShader->rendbuffer.blendalphalow2buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW2;
				bufferlist =  &pShader->rendbuffer.blendalphalow2buffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{	
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}
				break;
			case 5:		//blend + alphalow3
				if (!pShader->rendbuffer.blendalphalow3buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow3list.push_back(pShader);

					pShader->rendbuffer.blendalphalow3buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW3;
				bufferlist =  &pShader->rendbuffer.blendalphalow3buffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{	
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}
				break;
			case 6:		//blend + alphalow4
				if (!pShader->rendbuffer.blendalphalow4buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow4list.push_back(pShader);

					pShader->rendbuffer.blendalphalow4buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW4;
				bufferlist =  &pShader->rendbuffer.blendalphalow4buffer;
				if (!blight)
				{
					pusenum = &bufferlist->bufferusenum;
					pbuffer = &bufferlist->rendbuffer;
				}
				else
				{	
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusenum = &bufferlist->lightbufferusenum;
					pbuffer = &bufferlist->lightrendbuffer;
				}
				break;
			default:
				pbuffer = NULL;
				break;
				//case 2:		//blend + color

				//	pLayer3D->m_shaderalphablendcolorlist.push_back(pShader);

				//	pShader->rendbuffer.btestnonepushed = true;
				//	break;

			}
			break;
		case 2://transparent
			if (!pShader->rendbuffer.transparentbuffer.bpushed)
			{
				pLayer3D->m_shadertransparentlist.push_back(pShader);

				pShader->rendbuffer.transparentbuffer.bpushed = true;
			}
			dwrendtype = RENDTYPE_TRANSPARENT;
			bufferlist = &pShader->rendbuffer.transparentbuffer;
			if (!blight)
			{
				pusenum = &bufferlist->bufferusenum;
				pbuffer = &bufferlist->rendbuffer;
			}
			else
			{	
				dwstreamtype = LMST_POSNMLCOLTEX0;
				pusenum = &bufferlist->lightbufferusenum;
				pbuffer = &bufferlist->lightrendbuffer;
			}
			break;
		default:
			pbuffer = NULL;
			break;
		}
		if(!pbuffer)
		{
			ptagbuffer = NULL;
			return;
		}
		else
		{
			int& usenum = *pusenum;
			vector<ShaderGroup::_tagShader::RendBuffer::_tagBuffer>& buffer = *pbuffer;
			if (buffer.empty())
			{
				ShaderGroup::_tagShader::RendBuffer::_tagBuffer tagbuffer;
				buffer.push_back(tagbuffer);
				if(!buffer[0].CreateBuffer())
				{
					ptagbuffer = NULL;
					return;
				}
				buffer[0].Clear();
				usenum = 0;
			}

			if (buffer[usenum].vertexcount + dwCount > buffer[usenum].vertexsize)
			{
				if (!buffer[usenum].ExtendVertexBuffer(buffer[usenum].vertexcount + dwCount,dwstreamtype))
				{
					if(!pShader->rendbuffer.AddTagBuffer(dwCount,dwstreamtype,dwrendtype))
					{
						ptagbuffer = NULL;
					}
				}
			}
			ptagbuffer = &buffer[usenum];
		}
	}
	void render::Interface::Layer3D::GetDynamicTagBuffer(Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer*& ptagbuffer,DWORD& dwrendtype,DWORD& dwstreamtype,DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,BOOL blight,DWORD dwMode)
	{
		ShaderGroup *pShaderGroup = GetShaderGroup(dwShaderGroup);
		if (!pShaderGroup)
		{
			return;
		}
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
		int* pusedynamicnum = NULL;
		ShaderGroup::_tagShader::RendBuffer::BufferList* bufferlist;
		vector<ShaderGroup::_tagShader::RendBuffer::_tagBuffer>* pbuffer = NULL;
		dwrendtype = RENDTYPE_TESTNONE;
		dwstreamtype = LMST_POSCOLTEX0;
		if (!pShader)
		{
			ptagbuffer = NULL;
			return;
		}
		//添加到渲染表
		switch(dwMode)
		{
		case 0://test
			if (!pShader->dwAlphaTestMode)
			{

				if (!pShader->rendbuffer.testnonebuffer.bpushed)
				{
					pLayer3D->m_shaderalphatestnonelist.push_back(pShader);
					pShader->rendbuffer.testnonebuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_TESTNONE;
				bufferlist = &pShader->rendbuffer.testnonebuffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;
				}

			}
			else
			{
				if (!pShader->rendbuffer.testtransparentbuffer.bpushed)
				{
					pLayer3D->m_shaderalphatesttransparentlist.push_back(pShader);
					pShader->rendbuffer.testtransparentbuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_TESTTRANSPARENT;
				bufferlist = &pShader->rendbuffer.testtransparentbuffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}


			}

			break;
		case 1://blend 
			switch(pShader->dwAlphaBlendMode)
			{
			case 0:		//blend + additive
				if (!pShader->rendbuffer.blendadditivebuffer.bpushed)
				{
					pLayer3D->m_shaderalphablendaddtivelist.push_back(pShader);
					pShader->rendbuffer.blendadditivebuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDADDITIVE;
				bufferlist = &pShader->rendbuffer.blendadditivebuffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}
				break;
			case 1:		//blend + alpha
				if (!pShader->rendbuffer.blendalphabuffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalist.push_back(pShader);
					pShader->rendbuffer.blendalphabuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHA;
				bufferlist = &pShader->rendbuffer.blendalphabuffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}
				break;
			case 2:		//blend + alphalow
				if (!pShader->rendbuffer.blendalphalowbuffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalowlist.push_back(pShader);
					pShader->rendbuffer.blendalphalowbuffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW;
				bufferlist = &pShader->rendbuffer.blendalphalowbuffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}
				break;
			case 3:		//blend + alphalow1
				if (!pShader->rendbuffer.blendalphalow1buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow1list.push_back(pShader);
					pShader->rendbuffer.blendalphalow1buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW1;
				bufferlist = &pShader->rendbuffer.blendalphalow1buffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}
				break;
			case 4:		//blend + alphalow2
				if (!pShader->rendbuffer.blendalphalow2buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow2list.push_back(pShader);
					pShader->rendbuffer.blendalphalow2buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW2;
				bufferlist = &pShader->rendbuffer.blendalphalow2buffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}
				break;
			case 5:		//blend + alphalow3
				if (!pShader->rendbuffer.blendalphalow3buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow3list.push_back(pShader);
					pShader->rendbuffer.blendalphalow3buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW3;
				bufferlist = &pShader->rendbuffer.blendalphalow3buffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}
				break;
			case 6:		//blend + alphalow4
				if (!pShader->rendbuffer.blendalphalow4buffer.bpushed)
				{
					pLayer3D->m_shaderalphablendalphalow4list.push_back(pShader);
					pShader->rendbuffer.blendalphalow4buffer.bpushed = true;
				}
				dwrendtype = RENDTYPE_BLENDALPHALOW4;
				bufferlist = &pShader->rendbuffer.blendalphalow4buffer;
				if (!blight)
				{
					pusedynamicnum = &bufferlist->dynamicbufferusenum;
					pbuffer = &bufferlist->dynamicrendbuffer;
				}
				else
				{
					dwstreamtype = LMST_POSNMLCOLTEX0;
					pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
					pbuffer = &bufferlist->lightdynamicrendbuffer;

				}
				break;
			default:
				pbuffer = NULL;
				break;
			}
			break;
		case 2://transparent
			if (!pShader->rendbuffer.transparentbuffer.bpushed)
			{
				pLayer3D->m_shadertransparentlist.push_back(pShader);
				pShader->rendbuffer.transparentbuffer.bpushed = true;
			}
			dwrendtype = RENDTYPE_TRANSPARENT;
			bufferlist = &pShader->rendbuffer.transparentbuffer;
			if (!blight)
			{
				pusedynamicnum = &bufferlist->dynamicbufferusenum;
				pbuffer = &bufferlist->dynamicrendbuffer;
			}
			else
			{
				dwstreamtype = LMST_POSNMLCOLTEX0;
				pusedynamicnum = &bufferlist->lightdynamicbufferusenum;
				pbuffer = &bufferlist->lightdynamicrendbuffer;

			}

			break;		
		default:
			pbuffer = NULL;
			break;
		}
		if (!pbuffer)
		{
			ptagbuffer = NULL;
			return;
		}
		else
		{

			int& usedynamicnum = *pusedynamicnum;
			vector<ShaderGroup::_tagShader::RendBuffer::_tagBuffer>& buffer = *pbuffer;
			if (buffer.empty())
			{
				ShaderGroup::_tagShader::RendBuffer::_tagBuffer tagbuffer;
				buffer.push_back(tagbuffer);
				if(!buffer[0].CreateBuffer())
				{
					ptagbuffer = NULL;
					return;
				}
				buffer[0].Clear();
				usedynamicnum = 0;
			}
			if(!pShader->rendbuffer.AddDynamicTagBuffer(dwCount,dwstreamtype,dwrendtype))
			{
				ptagbuffer = NULL;
				return;
			}
			ptagbuffer = &buffer[usedynamicnum];
			usedynamicnum++;
		}

	}


	/*
	* 功能: 将需要渲染的三角形送入缓冲,并记录在指定的动态着色变量中
	* 摘要: -
	* 参数: pDynamicShader - 渲染组句柄
	*		 dwCount		- 三角形数量
	*		 pPos			- 坐标信息
	*		 pUV0			- 纹理信息
	*		 pColorDiffuse	- 顶点散射信息
	*		 pColorSpecular	- 顶点镜像颜色信息
	*		 dwMode			- 渲染模式
	* 返回值: -
	* 作者: lpf
	* 创建日期: 2008.01.23
	*/
	void render::Interface::Layer3D::SendTriangleToCacheEx(render::Interface::Layer3D::ShaderGroup::_tagShader * pDynamicShader, DWORD dwCount, const _tagTriangle* ptriangle,DWORD dwMode)
	{
		// 检查多边形缓冲是否溢出
		DWORD dwWantSize = m_dwPolygonArrayUsage + dwCount;
		if (dwWantSize >= MAX_POLYGON_ARRAY_SIZE)
		{
			OutputConsole("error: render::Layer3D triangle cache overflow [%d]!\n", m_dwPolygonArrayUsage);
			return;
		}
		DWORD n = m_dwPolygonArrayUsage;
		memcpy(&m_pPolygonArray[m_dwPolygonArrayUsage],ptriangle,sizeof(_tagTriangle) * dwCount);
		m_dwPolygonArrayUsage += dwCount;
		// 拷贝数据到多边形缓冲
		for (DWORD dwOffset = 0; dwOffset < dwCount * 3; dwOffset += 3,n++)
		{
			_tagTriangle & tTriangle = m_pPolygonArray[n];
			/*memcpy(tTriangle.pos, &pPos[dwOffset], sizeof(D3DXVECTOR3) * 3);
			memcpy(tTriangle.uv0, &pUV0[dwOffset], sizeof(D3DXVECTOR2) * 3);
			memcpy(tTriangle.color, &pColorDiffuse[dwOffset], sizeof(D3DCOLOR) * 3);
			memcpy(tTriangle.specular, &pColorSpecular[dwOffset], sizeof(D3DCOLOR) * 3);*/

			//添加到渲染表头
			switch(dwMode)
			{
			case 0:		//test
				tTriangle.pNext						   = pDynamicShader->pTriangleAlphaTestHead;
				pDynamicShader->pTriangleAlphaTestHead = &tTriangle;
				break;
			case 1:		//blend
				tTriangle.pNext							= pDynamicShader->pTriangleAlphaBlendHead;
				pDynamicShader->pTriangleAlphaBlendHead = &tTriangle;
				break;			
			}
		}
	}



	// ----------------------------------------------------
	// 调试和编辑器使用，画线
	// 使用渲染句柄
	// 
	// ----------------------------------------------------
	void render::Interface::Layer3D::_DEBUG_DrawLine(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,D3DCOLOR dwColor)//2
	{
		//设置定点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     

		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pLayer3D->SetStreamSource(LMST_POSCOL);

		pLayer3D->SetStreamType(LMST_POSCOL);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		pCachePos[0] = *pVtx0;
		pCachePos[1] = *pVtx1;

		pCacheCol[0] = pCacheCol[1] = dwColor;

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, 1);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
	}

	void render::Interface::Layer3D::_DEBUG_DrawLine(const D3DXVECTOR3 pVtx[],D3DCOLOR dwColor)//2
	{
		//设置定点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);		

		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pLayer3D->SetStreamSource(LMST_POSCOL);

		pLayer3D->SetStreamType(LMST_POSCOL);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		memcpy(pCachePos,pVtx,sizeof(D3DXVECTOR3) * 2);

		pCacheCol[0] = pCacheCol[1] = dwColor;

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, 1);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);

	}

	void render::Interface::Layer3D::_DEBUG_DrawWireQuad(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,const D3DXVECTOR3 *pVtx3,D3DCOLOR dwColor)//2
	{
		//设置定点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pLayer3D->SetStreamSource(LMST_POSCOL);

		pLayer3D->SetStreamType(LMST_POSCOL);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		pCachePos[0] = *pVtx0;
		pCachePos[1] = *pVtx1;
		pCachePos[2] = *pVtx1;
		pCachePos[3] = *pVtx2;
		pCachePos[4] = *pVtx2;
		pCachePos[5] = *pVtx3;
		pCachePos[6] = *pVtx3;
		pCachePos[7] = *pVtx0;

		pCacheCol[0] = 
			pCacheCol[1] = 
			pCacheCol[2] =
			pCacheCol[3] =
			pCacheCol[4] =
			pCacheCol[5] =
			pCacheCol[6] =
			pCacheCol[7] = dwColor;

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, 4);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
	}

	void render::Interface::Layer3D::_DEBUG_DrawWireQuad(const D3DXVECTOR3 *pVtx0,DWORD dwLineCount,D3DCOLOR dwColor)//2
	{
		//设置定点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		if (!dwLineCount)
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
		pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,dwColor);

		//
		pLayer3D->SetStreamSource(LMST_POS);

		pLayer3D->SetStreamType(LMST_POS);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		//pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		memcpy(pCachePos,pVtx0,(dwLineCount * 2) * sizeof(D3DXVECTOR3));


		pVBPOS->Unlock();
		//pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, dwLineCount);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
	}


	void render::Interface::Layer3D::_DEBUG_DrawTriangle(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,D3DCOLOR dwColor)//2
	{
		//设置顶点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     

		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		//
		pLayer3D->SetStreamSource(LMST_POSCOL);

		pLayer3D->SetStreamType(LMST_POSCOL);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		pCachePos[0] = *pVtx0;
		pCachePos[1] = *pVtx1;
		pCachePos[2] = *pVtx2;

		pCacheCol[0] = pCacheCol[1] = pCacheCol[2] = dwColor;

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(2,NULL,0,0);
	}

	void render::Interface::Layer3D::_DEBUG_DrawTriangle(DWORD dwTexturHandle,const D3DXVECTOR3 *pVtx,const D3DXVECTOR2 *pUV0,D3DCOLOR dwColor)
	{
		//设置顶点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		pInterface->SetTexture(0,dwTexturHandle);

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
		//pDevice->SetStreamSource(2,m_pVB_COLOR_SPECULAR,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));

		pLayer3D->SetStreamType(LMST_POSNMLTEX0);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DCOLOR    *pCacheColSpc(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;
		//LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC = pLayer3D->m_pVB_COLOR_SPECULAR;
		LPDIRECT3DVERTEXBUFFER9 pVbUV0 = pLayer3D->m_pVB_TEX0;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
		//pVBCOLSPC->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
		pVbUV0->Lock( 0, 0, (void**)&pCacheUV0, D3DLOCK_DISCARD);

		memcpy(pCachePos,pVtx,sizeof(D3DXVECTOR3) * 3);
		memcpy(pCacheUV0,pUV0,sizeof(D3DXVECTOR2) * 3);

		pCacheCol[0] = pCacheCol[1] = pCacheCol[2] = dwColor;

		pVbUV0->Unlock();
		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1);

		pInterface->SetTexture(0,NULL);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
		//pDevice->SetStreamSource(2,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}								   

	void render::Interface::Layer3D::_DEBUG_DrawWireBox(const D3DXVECTOR3 *pCenter,float fRadius,D3DCOLOR dwColor)
	{

		//设置顶点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		//
		pLayer3D->SetStreamSource(LMST_POSCOL);

		pLayer3D->SetStreamType(LMST_POSCOL);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		const int iIndices[2*12] = {0,1,1,2,2,3,3,0,0,4,1,5,2,6,3,7,4,5,5,6,6,7,7,4};

		float xmin=pCenter->x - fRadius,ymin=pCenter->y - fRadius,zmin= pCenter->z - fRadius;
		float xmax=pCenter->x + fRadius,ymax= pCenter->y + fRadius,zmax=pCenter->z + fRadius;

		//          7__________6
		//         /		  /|
		//        /			 / |
		//       /			/  |
		//		4----------5   |  
		//		|		   |   |
		//		|	3	   | 2 /	
		//		|		   |  /	
		//		|0		  1| /	
		//		------------

		D3DXVECTOR3 vConner[8];
		vConner[0]=D3DXVECTOR3(xmin,ymin,zmin);
		vConner[1]=D3DXVECTOR3(xmax,ymin,zmin);
		vConner[2]=D3DXVECTOR3(xmax,ymin,zmax);
		vConner[3]=D3DXVECTOR3(xmin,ymin,zmax);
		vConner[4]=D3DXVECTOR3(xmin,ymax,zmin);
		vConner[5]=D3DXVECTOR3(xmax,ymax,zmin);
		vConner[6]=D3DXVECTOR3(xmax,ymax,zmax);
		vConner[7]=D3DXVECTOR3(xmin,ymax,zmax);

		for(DWORD dwLine = 0;dwLine < 12;dwLine++)
		{
			pCachePos[dwLine * 2 ] = vConner[iIndices[dwLine *2]];
			pCachePos[dwLine * 2 + 1] = vConner[iIndices[dwLine *2 + 1]];
			pCacheCol[dwLine * 2] = dwColor;
			pCacheCol[dwLine * 2 + 1] = dwColor;
		}	

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, 12);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
	}


	void render::Interface::Layer3D::_DEBUG_DrawWireBox(const  D3DXVECTOR3 *pMin,const  D3DXVECTOR3 *pMax,D3DCOLOR dwColor)
	{
		//设置顶点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		if (pInterface->GetDeviceLost())
		{
			return;
		}
		Layer3D *pLayer3D= pInterface->GetLayer3D();

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		pDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		//
		pLayer3D->SetStreamSource(LMST_POSCOL);

		pLayer3D->SetStreamType(LMST_POSCOL);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		const int iIndices[2*12] = {0,1,1,2,2,3,3,0,0,4,1,5,2,6,3,7,4,5,5,6,6,7,7,4};

		float xmin = pMin->x,ymin = pMin->y,zmin= pMin->z;
		float xmax = pMax->x,ymax = pMax->y,zmax= pMax->z;

		//          7__________6
		//         /		  /|
		//        /			 / |
		//       /			/  |
		//		4----------5   |  
		//		|		   |   |
		//		|	3	   | 2 /	
		//		|		   |  /	
		//		|0		  1| /	
		//		------------

		D3DXVECTOR3 vConner[8];
		vConner[0]=D3DXVECTOR3(xmin,ymin,zmin);
		vConner[1]=D3DXVECTOR3(xmax,ymin,zmin);
		vConner[2]=D3DXVECTOR3(xmax,ymin,zmax);
		vConner[3]=D3DXVECTOR3(xmin,ymin,zmax);
		vConner[4]=D3DXVECTOR3(xmin,ymax,zmin);
		vConner[5]=D3DXVECTOR3(xmax,ymax,zmin);
		vConner[6]=D3DXVECTOR3(xmax,ymax,zmax);
		vConner[7]=D3DXVECTOR3(xmin,ymax,zmax);

		for(DWORD dwLine = 0;dwLine < 12;dwLine++)
		{
			pCachePos[dwLine * 2 ] = vConner[iIndices[dwLine *2]];
			pCachePos[dwLine * 2 + 1] = vConner[iIndices[dwLine *2 + 1]];
			pCacheCol[dwLine * 2] = dwColor;
			pCacheCol[dwLine * 2 + 1] = dwColor;
		}	

		pVBPOS->Unlock();
		pVBCOL->Unlock();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, 12);

		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
	}


	//__CLASSE__(render::Interface)___
	IMPLEMENT_SINGLETON(render::Interface);
	render::Interface::Interface(void)
	{
		//纹理
		//ZeroMemory(m_pTextureArray,sizeof(Texture*)*MAX_TEXTURE_CACHE_SIZE);
		m_dwTextureCnt = 0;
		m_dwTextureMemorySize = 0;

		ZeroMemory(&m_Textures,sizeof(tagTextureInfo)*MAX_TEXTURE_CACHE_SIZE);

		// add by lpf 2007-11-27
		ZeroMemory(&m_tSetting, sizeof(_tagSetting));
	}

	render::Interface::~Interface(void)
	{
	}

	BOOL render::Interface::Create(HWND hWnd,const char *pszConfigFile)
	{
		m_hWnd = hWnd;

		DefaultSetting(pszConfigFile);

		CreateDirect3D9();

		//初始化Device
		CreateDevice();

		//检测硬件支持
		if (!CheckHardware())
		{
			OutputConsole("error: render::Interface: Check hardware surpport failed.\n");
			DestroyDevice();
			//MessageBox(NULL,"检测硬件支持失败,显卡不支持游戏所要求的特性\n原因可能是显卡较旧或者未安装最新的驱动所致","致命错误",MB_OK);
			MessageBox(NULL,"Hardware checked faild!","Error",MB_OK);
			return FALSE;
		}

		//建立图层
		CreateLayer();

		//初始化状态
		SetDefaultRenderState();

		Dump();

		return TRUE;
	}

	void render::Interface::Destroy()
	{
		DestroyLayer();
		DestroyDevice();
		DestroyDirect3D9();
		DestroyAllTexture();
	}



	/*
	* 功能: 默认设定
	* 摘要: -
	* 参数: pszConfigString - 配置信息字符串
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		为2D和3D线性过滤状态赋予默认值
	*/
	void render::Interface::DefaultSetting(const char * pszConfigString)
	{
		_tagSetting &tSetting = m_tSetting;

		//设置确省的
		//当前字库名称
		// modify by lpf 2007-11-27
		strncpy(tSetting.szFonts[0], "font/12.font", _MAX_PATH - 1);
		tSetting.szFonts[0][_MAX_PATH -1] = 0;
		strncpy(tSetting.szFonts[1], "font/1.font", _MAX_PATH - 1);
		tSetting.szFonts[1][_MAX_PATH -1] = 0;


		tSetting.bHVProcess  = FALSE;

		tSetting.iWndPosX = 10;
		tSetting.iWndPosY = 10;
		tSetting.iWndWidth = 640;
		tSetting.iWndHeight= 480;

		tSetting.bWndAutoStyle  = TRUE; 
		tSetting.bWndAutoCenter = FALSE;

		tSetting.bIsWindow = TRUE;
		tSetting.bMipMapping = FALSE;

		tSetting.bTextureMode32   = TRUE;
		tSetting.b2DLinear		  = FALSE;
		tSetting.b3DLinear		  = FALSE;
		tSetting.d3dfmtBackBuffer = D3DFMT_X8R8G8B8;//

		tSetting.bPresentationInterval = FALSE;
		ModifySetting(pszConfigString);
	}



	BOOL render::Interface::CreateDevice()
	{
		//adjust windows to prev width height	
		if (m_tSetting.bWndAutoStyle)
		{
			if (!m_tSetting.bIsWindow)
			{
				//set fullscreen style
				DWORD dwStyle = GetWindowLong(m_hWnd,GWL_STYLE);

				dwStyle &=~WS_CAPTION;

				SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);

				MoveWindow(m_hWnd,0,0,m_tSetting.iWndWidth,m_tSetting.iWndHeight,TRUE);
			}
			else
			{
				//modify window style
				DWORD dwStyle = GetWindowLong(m_hWnd,GWL_STYLE);

				dwStyle |= WS_CAPTION;

				SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);

				// Set the window's initial width
				RECT rc;
				SetRect( &rc, 0, 0, m_tSetting.iWndWidth, m_tSetting.iWndHeight);
				AdjustWindowRect( &rc, dwStyle, FALSE );

				int iPosX = m_tSetting.iWndPosX;
				int iPosY = m_tSetting.iWndPosY;
				if (m_tSetting.bWndAutoCenter)
				{
					GetSystemMetrics(SM_CYSCREEN);

					iPosX = GetSystemMetrics(SM_CXSCREEN) / 2 - (rc.right - rc.left) /2;
					iPosY = GetSystemMetrics(SM_CYSCREEN) / 2 - (rc.bottom- rc.top) /2;
				}

				MoveWindow(m_hWnd,iPosX,iPosY,rc.right - rc.left,rc.bottom - rc.top,TRUE);
			}
		}
		//RECT rc;
		//GetClientRect(m_hWnd,&rc);

		ZeroMemory( &m_stInfo.d3dpp, sizeof(m_stInfo.d3dpp) );

		m_stInfo.d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_stInfo.d3dpp.BackBufferCount  = 2;
		m_stInfo.d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		m_stInfo.d3dpp.hDeviceWindow = m_hWnd;
		m_stInfo.d3dpp.Windowed = m_tSetting.bIsWindow;

		if (m_stInfo.d3dpp.Windowed)
		{
			D3DDISPLAYMODE dm;
			m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dm);
			m_stInfo.d3dpp.BackBufferFormat = dm.Format;
			m_stInfo.d3dpp.BackBufferWidth  = m_tSetting.iWndWidth;
			m_stInfo.d3dpp.BackBufferHeight = m_tSetting.iWndHeight;
			//m_stInfo.d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		}
		else
		{
			m_stInfo.d3dpp.BackBufferFormat = m_tSetting.d3dfmtBackBuffer;
			m_stInfo.d3dpp.BackBufferWidth  = m_tSetting.iWndWidth;
			m_stInfo.d3dpp.BackBufferHeight = m_tSetting.iWndHeight;
			m_stInfo.d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		}
		m_stInfo.d3dpp.PresentationInterval =  m_tSetting.bPresentationInterval? D3DPRESENT_INTERVAL_ONE: D3DPRESENT_INTERVAL_IMMEDIATE;
		//设置Depth Buffer Format
		m_stInfo.d3dpp.AutoDepthStencilFormat = CheckDepthStencilFormat(m_stInfo.d3dpp.BackBufferFormat);
		m_stInfo.d3dpp.EnableAutoDepthStencil = TRUE;


		/*if (!m_tSetting.bHVProcess)
		{
		dwBehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;			
		}*/		
		HRESULT hResult;
		DWORD dwBehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
		GetSetting()->bHVProcess = TRUE;
		D3DCAPS9 d3dCaps;
		m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&d3dCaps);
		if (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT&&d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1))
		{
			GetSetting()->dwshaderverison = d3dCaps.VertexShaderVersion;
			GetSetting()->ihvpmatrixcount = d3dCaps.MaxVertexShaderConst/4 - 10 < 0?0:(d3dCaps.MaxVertexShaderConst/4 - 10);
			GetSetting()->bgpuprocess = true;
			m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,&d3dCaps);
			GetSetting()->isvpmatrixcount = d3dCaps.MaxVertexShaderConst/4 - 10 > 200?200:(d3dCaps.MaxVertexShaderConst/4 - 10);
		}
		else
		{
			m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,&d3dCaps);
			dwBehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;			
			GetSetting()->bHVProcess = FALSE;
			GetSetting()->ihvpmatrixcount = 1;
			if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1))
			{
				GetSetting()->bgpuprocess = true;
				GetSetting()->isvpmatrixcount = d3dCaps.MaxVertexShaderConst/4 - 10 > 200?200:(d3dCaps.MaxVertexShaderConst/4 - 10);

			}
			else
			{

				GetSetting()->bgpuprocess = false;	

			}
			GetSetting()->dwshaderverison = d3dCaps.VertexShaderVersion;
		}		

		//在发布取消PerfHUD
		// Set default settings
		UINT AdapterToUse = D3DADAPTER_DEFAULT;
		D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
		// Look for 'NVIDIA PerfHUD' adapter
		// If it is present, override default settings
		for (UINT Adapter=0;Adapter<m_pD3D->GetAdapterCount();Adapter++)
		{
			D3DADAPTER_IDENTIFIER9 Identifier;
			HRESULT Res;
			Res = m_pD3D->GetAdapterIdentifier(Adapter,0,&Identifier);
			if (strstr(Identifier.Description,"PerfHUD") != 0)
			{
				AdapterToUse=Adapter;
				DeviceType=D3DDEVTYPE_REF;
				MessageBox(NULL,"PerfHUD","TEST",MB_OK);
				break;
			}
		}




		if (FAILED(hResult = m_pD3D->CreateDevice( AdapterToUse,DeviceType,m_hWnd,
			dwBehaviorFlags,&m_stInfo.d3dpp, &m_pD3DDevice ) ) )
		{
			ASSERT(0);
			/*OutputConsole("error: render::Interface: Create D3D Device(HWVP) Failed.\n");
			MessageBox(NULL,"创建DirectX 驱动失败，请确认显卡支持硬件加速","致命错误",MB_OK);*/
			return FALSE;
		}

		//READ DEVICE CAPS 
		m_pD3DDevice->GetDeviceCaps(&m_stInfo.d3dCaps);

		m_bDeviceLost = FALSE;
		m_bAlreadyBeginScene = FALSE;
		return TRUE;

	}

	void render::Interface::DestroyDevice()
	{
		SAFERELEASE(m_pD3DDevice);
	}

	BOOL render::Interface::CheckHardware(void)
	{
		//CHECK HAL RENDER SURPPORT
		HRESULT hResult;

		//CHEKCK DEVICE IS HAL DEVICE
		if (m_stInfo.d3dCaps.DeviceType != D3DDEVTYPE_HAL)
		{
			OutputError("render::Interface: Current device wasn't HAL TYPE.\n");
			return FALSE;
		}
		//____CHECK TEXTURE FORMAT 8888 4444_________
		//CHECK 4444
		if (FAILED(hResult = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			m_stInfo.d3dpp.BackBufferFormat,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_A4R4G4B4)))
		{
			OutputError("render::Interface: D3DFORMAT A4R4G4B4 test failed.\n");
			return FALSE;
		}
		//CHECK 8888
		if (FAILED(hResult = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			m_stInfo.d3dpp.BackBufferFormat,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_A8R8G8B8)))
		{
			OutputError("render::Interface: D3DFORMAT A8R8G8B8 test failed.\n");
			return FALSE;
		}

		//GET ADAPTER INFORMATION
		if (FAILED(hResult = m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,0,&m_stInfo.AdapterIdentifier)))
		{
			OutputError("render::Interface: GetAdapterIdentifier() failed.\n");
			return FALSE;
		}
		if (( m_stInfo.d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER) == 0)
		{
			GetSetting()->bborderaddress = false;
		}
		else
		{
			GetSetting()->bborderaddress = true;
		}
		GetSetting()->dwMaxTexStage = m_stInfo.d3dCaps.MaxTextureBlendStages;


		return TRUE;
	}


	void render::Interface::SetDefaultRenderState(void)
	{
		// FILL Solid Mode
		m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

		// Setup Texture Filter
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

		// Setup Alpha 
		//m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		///m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		//m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		// Setup Texture Filter
		//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

		//设置纹理平铺
		m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
		m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSW,D3DTADDRESS_WRAP);
		m_pD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		m_pD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
		m_pD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSW,D3DTADDRESS_WRAP);
	}


	void render::Interface::Clear(D3DCOLOR d3dColor)
	{
		m_pD3DDevice->Clear(0, NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER| ((m_stInfo.d3dpp.AutoDepthStencilFormat == D3DFMT_D16) ? 0 : D3DCLEAR_STENCIL) ,d3dColor, 1.0f, 0 );
	}

	BOOL render::Interface::BeginScene(void)
	{
		if (!m_bAlreadyBeginScene)
		{
			m_pD3DDevice->BeginScene();
			m_bAlreadyBeginScene = TRUE;
			return TRUE;
		}
		else
		{
			return TRUE;
		}

	}

	void render::Interface::EndScene()
	{
		//刷新屏幕
		if (m_bAlreadyBeginScene)
		{
			m_pD3DDevice->EndScene();
			m_bAlreadyBeginScene = FALSE;
		}
		/*if (m_pD3DDevice->Present( NULL, NULL, NULL, NULL ) == D3DERR_DEVICELOST)
		{

		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
		while(hr == D3DERR_DEVICELOST)
		{
		hr = m_pD3DDevice->TestCooperativeLevel();
		}

		if (hr == D3DERR_DEVICENOTRESET)
		{
		hr = m_pD3DDevice->Reset(&GetInfo()->d3dpp);
		if (hr != S_OK)
		{
		Reset();
		}

		}

		}*/
	}



	/*
	* 功能: 创建渲染层
	* 摘要: 包括2D和3D的渲染层
	* 参数: -
	* 返回值: 总是返回 TRUE
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2007.11.27 - lpf
	*		创建2D渲染层时要传入当前配置信息
	*	2008.05.26 - lpf
	*		创建3D渲染层时要传入当前配置信息
	*/
	BOOL render::Interface::CreateLayer(void)
	{
		//建立渲染层
		m_pLayer2D = new Layer2D;
		m_pLayer2D->Create(m_tSetting);

		m_pLayer3D = new Layer3D;
		m_pLayer3D->Create(m_tSetting);

		return TRUE;
	}



	void render::Interface::DestroyLayer(void)
	{
		SAFEDESTROY(m_pLayer2D);
		SAFEDESTROY(m_pLayer3D);
	}

	DWORD render::Interface::AcquireTexture(const char* pszFileName)
	{
		DWORD dwTextureID = 0;
		itHMsw it = m_TextureNameToID.find(pszFileName);
		if(it != m_TextureNameToID.end())
		{
			dwTextureID = (*it).second;
		}
		else
		{
			//循环判断是否被使用
			for(DWORD i=1;i<MAX_TEXTURE_CACHE_SIZE;i++)
			{
				if(FALSE==m_Textures[i].bUse)
				{
					strncpy(m_Textures[i].pszFileName,pszFileName,MAX_PATH);
					m_Textures[i].bUse = TRUE;
					m_Textures[i].SetLoadState(Load_Not);
					m_TextureNameToID[pszFileName]=i;
					dwTextureID = i;
					//重新设置纹理引用计数为0
					m_Textures[i].ResetCount();
					break;
				}
			}
		}
		if(dwTextureID != 0)
		{
			m_Textures[dwTextureID].AddRef();
			/*_snprintf(strInfo,1024,"Require Texture(ID:%d,Name:%s)。",dwTextureID,pszFileName);
			PutDebugString(strInfo);*/
		}
		return dwTextureID;
	}

	void render::Interface::ReleaseTexture(DWORD dwTextureID)
	{
		if(dwTextureID == 0 || dwTextureID >= MAX_TEXTURE_CACHE_SIZE) return;
		if(m_Textures[dwTextureID].bUse)
		{
			m_Textures[dwTextureID].RemoveRef();
			/*_snprintf(strInfo,1024,"Release Texture(ID:%d,Name:%s)。",
			dwTextureID,m_Textures[dwTextureID].pszFileName);
			PutDebugString(strInfo);*/
		}
	}

	render::Interface::tagTextureInfo* render::Interface::GetTextureInfo(DWORD dwID)
	{
		if(dwID >= MAX_TEXTURE_CACHE_SIZE) return NULL;
		if(m_Textures[dwID].bUse == FALSE) return NULL;
		return &m_Textures[dwID];
	}

	render::Texture *render::Interface::CreateTexture(CRFile* pFile,DWORD dwTextureProcision)
	{
		Texture *pTexture = new Texture;
		BOOL bResult = pTexture->Create(m_pD3DDevice,pFile,
			m_tSetting.bTextureMode32, m_stInfo.d3dCaps.MaxTextureWidth,
			m_stInfo.d3dCaps.MaxTextureHeight,dwTextureProcision);
		if (!bResult)
		{
			OutputError("render::Interface:CreateTexture(\"%s\").\n",pFile->GetFileName());
			SAFEDELETE(pTexture);
			ASSERT(0);
			return NULL;
		}
		m_dwTextureCnt++;
		m_dwTextureMemorySize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));
		return pTexture;
	}

	render::Texture * render::Interface::GetTexture(DWORD  dwTextureHandle)
	{
		if (!dwTextureHandle || dwTextureHandle>=MAX_TEXTURE_CACHE_SIZE) 
		{
			OutputConsole("error: GetTexture() texture handle must not be NULL.\n");
			ASSERT(0);
			return NULL;
		}
		return m_Textures[dwTextureHandle].pTexture;
		//return m_pTextureArray[dwTextureHandle - 1];
	}

	DWORD render::Interface::CreateTexture(const char * pszFileName,DWORD dwTextureProcision)
	{	
		if (m_dwTextureCnt > MAX_TEXTURE_CACHE_SIZE) 
		{
			OutputConsole("error: render::Interface:Texture Cache Overflow.\n");
			return NULL;
		}

		Texture *pTexture = new Texture;

		BOOL bResult = pTexture->Create(m_pD3DDevice,pszFileName,m_tSetting.bTextureMode32, m_stInfo.d3dCaps.MaxTextureWidth,m_stInfo.d3dCaps.MaxTextureHeight,dwTextureProcision);
		if (!bResult)
		{
			OutputError("render::Interface:CreateTexture(\"%s\").\n",pszFileName);
			SAFEDELETE(pTexture);
			ASSERT(0);
			return NULL;
		}

		DWORD dwTex = 1;
		for(; dwTex < MAX_TEXTURE_CACHE_SIZE; dwTex++)
		{
			if (m_Textures[dwTex].bUse == false)
			{
				strncpy(m_Textures[dwTex].pszFileName,pszFileName,MAX_PATH);
				m_Textures[dwTex].bUse = TRUE;
				m_Textures[dwTex].SetLoadState(Load_Did);
				m_TextureNameToID[pszFileName]=dwTex;
				m_Textures[dwTex].pTexture = pTexture;
				//重新设置纹理引用计数为0
				m_Textures[dwTex].ResetCount();
				m_Textures[dwTex].AddRef();

				/*_snprintf(strInfo,1024,"Create Texture(ID:%d,Name:%s)。",
				dwTex,pszFileName);
				PutDebugString(strInfo);*/
				break;
			}
		}

		m_dwTextureCnt++;
		m_dwTextureMemorySize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));

		return (int)dwTex;
	}

	DWORD render::Interface::CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap)
	{	
		if (m_dwTextureCnt > MAX_TEXTURE_CACHE_SIZE) 
		{
			OutputConsole("error: render::Interface:Texture Cache Overflow.\n");
			return NULL;
		}

		Texture *pTexture = new Texture;

		BOOL bResult = pTexture->Create(m_pD3DDevice,dwWidth,dwHeight,pBitmap,m_tSetting.bTextureMode32,m_stInfo.d3dCaps.MaxTextureWidth,m_stInfo.d3dCaps.MaxTextureHeight);
		if (!bResult)
		{
			OutputError("render::Interface:CreateTexture(\"%d x %d\").\n",dwWidth,dwHeight);
			SAFEDELETE(pTexture);
			ASSERT(0);
			return NULL;
		}

		DWORD dwTex = 1;
		for(; dwTex < MAX_TEXTURE_CACHE_SIZE; dwTex++)
		{
			if (m_Textures[dwTex].bUse == false)
			{
				//strncpy(m_Textures[dwTex].pszFileName,pszFileName,MAX_PATH);
				m_Textures[dwTex].bUse = TRUE;
				m_Textures[dwTex].SetLoadState(Load_Did);
				//m_TextureNameToID[pszFileName]=dwTex;
				m_Textures[dwTex].pTexture = pTexture;
				//重新设置纹理引用计数为0
				m_Textures[dwTex].ResetCount();
				m_Textures[dwTex].AddRef();

				/*_snprintf(strInfo,1024,"Create texture(ID:%d,Name:%s)。",
				dwTex,"");
				PutDebugString(strInfo);*/
				break;
			}
		}

		m_dwTextureCnt++;
		m_dwTextureMemorySize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));

		return (int)dwTex;
	}

	DWORD render::Interface::CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD dwColor)
	{	
		if (m_dwTextureCnt > MAX_TEXTURE_CACHE_SIZE)
		{
			OutputConsole("error: render::Interface: Texture Cache Overflow.\n");
			return FALSE;
		}

		Texture *pTexture = new Texture;
		BOOL bResult = pTexture->Create(m_pD3DDevice,dwWidth,dwHeight,dwColor,m_tSetting.bTextureMode32,m_stInfo.d3dCaps.MaxTextureWidth,m_stInfo.d3dCaps.MaxTextureHeight);
		if (!bResult)
		{
			OutputConsole("error: render::Interface: CreateTexture(%d,%d).\n",dwWidth,dwHeight);
			SAFEDELETE(pTexture);
			ASSERT(0);
			return NULL;
		}

		DWORD dwTex = 1;
		for(; dwTex < MAX_TEXTURE_CACHE_SIZE; dwTex++)
		{
			if (m_Textures[dwTex].bUse == false)
			{
				//strncpy(m_Textures[dwTex].pszFileName,pszFileName,MAX_PATH);
				m_Textures[dwTex].bUse = TRUE;
				m_Textures[dwTex].SetLoadState(Load_Did);
				//m_TextureNameToID[pszFileName]=dwTex;
				m_Textures[dwTex].pTexture = pTexture;
				//重新设置纹理引用计数为0
				m_Textures[dwTex].ResetCount();
				m_Textures[dwTex].AddRef();

				/*_snprintf(strInfo,1024,"Create texture(ID:%d,Name:%s)。",
				dwTex,"");
				PutDebugString(strInfo);*/
				break;
			}
		}

		m_dwTextureCnt++;
		m_dwTextureMemorySize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));

		return (int)dwTex;
	}

	void render::Interface::DestroyTexture(DWORD dwTextureHandle)
	{
		ASSERT(dwTextureHandle < MAX_TEXTURE_CACHE_SIZE);

		if (dwTextureHandle)
		{
			if( m_Textures[dwTextureHandle].bUse == TRUE)
			{
				Texture * pTexture = m_Textures[dwTextureHandle].pTexture;
				if(pTexture)
				{
					m_dwTextureMemorySize -= pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));
					SAFEDESTROY(pTexture);				
					m_Textures[dwTextureHandle].pTexture = NULL;
					m_Textures[dwTextureHandle].bUse = FALSE;
					m_Textures[dwTextureHandle].SetLoadState(Load_Not);
					m_TextureNameToID.erase(m_Textures[dwTextureHandle].pszFileName);
					m_dwTextureCnt --;
					/*_snprintf(strInfo,1024,"Destroy Texture(ID:%d,Name:%s)。",
					dwTextureHandle,m_Textures[dwTextureHandle].pszFileName);
					PutDebugString(strInfo);*/
					strncpy(m_Textures[dwTextureHandle].pszFileName,"",1);
				}
			}
		}   
	}

	void   render::Interface::DestroyAllTexture(void)
	{
		DWORD TextureHandle = 1;
		for(;TextureHandle < MAX_TEXTURE_CACHE_SIZE;TextureHandle++)
		{
			if( m_Textures[TextureHandle].bUse == TRUE &&
				m_Textures[TextureHandle].IsShared() == false)
			{
				Texture * pTexture = m_Textures[TextureHandle].pTexture;
				if(pTexture)
				{
					m_dwTextureMemorySize -= pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));
					SAFEDESTROY(pTexture);				
					m_Textures[TextureHandle].pTexture = NULL;
					m_Textures[TextureHandle].bUse = FALSE;
					m_Textures[TextureHandle].SetLoadState(Load_Not);
					m_Textures[TextureHandle].RemoveRef();

					m_TextureNameToID.erase(m_Textures[TextureHandle].pszFileName);
					m_dwTextureCnt --;
				}
			}
		}
	}

	void render::Interface::SetTexture(DWORD dwStage,DWORD dwTextureHandle)
	{
		ASSERT(dwTextureHandle < MAX_TEXTURE_CACHE_SIZE);
		ASSERT(dwStage < 8 );
		if (!dwTextureHandle) 
		{
			m_pD3DDevice->SetTexture(dwStage,NULL);
			return;
		}
		Texture * pTexture  = 0;
		if (dwTextureHandle < MAX_TEXTURE_CACHE_SIZE)
		{
			pTexture = m_Textures[dwTextureHandle].pTexture;
		}
		if(pTexture)
			m_pD3DDevice->SetTexture(dwStage,pTexture->GetD3DTexture());
		else
			int x = 8;
	}

	void render::Interface::Cull(BOOL bEnable)
	{
		//只允许CCW
		DWORD dwCullMode = bEnable? D3DCULL_CCW : D3DCULL_NONE;
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,dwCullMode);
	}

	void render::Interface::Dump(void)
	{
		//显示纹理使用
		OutputConsole("render::Interface:: DUMP__Begin.\n");

		OutputConsole("Adapter: %s\n",m_stInfo.AdapterIdentifier.Description);
		OutputConsole("Driver: %s\n",m_stInfo.AdapterIdentifier.Driver);
		OutputConsole("Version: %d %d %d %d\n",
			HIWORD(m_stInfo.AdapterIdentifier.DriverVersion.HighPart),
			LOWORD(m_stInfo.AdapterIdentifier.DriverVersion.HighPart),
			HIWORD(m_stInfo.AdapterIdentifier.DriverVersion.LowPart),
			LOWORD(m_stInfo.AdapterIdentifier.DriverVersion.LowPart));

		DWORD dwTexMemSize = 0;
		for(DWORD n = 1; n < MAX_TEXTURE_CACHE_SIZE; n++)
		{
			render::Texture * pTexture = m_Textures[n].pTexture;
			if (pTexture)
			{
				dwTexMemSize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD) : sizeof(WORD));
			}
		}
		OutputConsole("Texture Usage: %d Mem: %.2f MB\n",m_dwTextureCnt,(float)dwTexMemSize / 1024.0f / 1024.0f);
		OutputConsole("render::Interface:: DUMP__END.\n");
	}

	void render::Interface::Layer3D::ShaderGroup::_SetTextureOp(_tagShader *pShader)
	{
		LPDIRECT3DDEVICE9 pDevice = render::Interface::GetInstance()->GetDevice();

		_tagShader::_tagLayer * pTempLayer(NULL);
		pTempLayer = &pShader->tLayer[0];
		//pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);// pTempLayer->dwColorOp);
		//pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);//pTempLayer->dwColorArg1);
		//pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);//DIpTempLayer->dwColorArg2);

		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,pTempLayer->dwColorOp);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,pTempLayer->dwColorArg1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,pTempLayer->dwColorArg2);

		pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,pTempLayer->dwAlphaOp);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,pTempLayer->dwAlphaArg1);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,pTempLayer->dwAlphaArg2);

		/*pTempLayer = &pShader->tLayer[1];
		pDevice->SetTextureStageState(1,D3DTSS_COLOROP,pTempLayer->dwColorOp);
		pDevice->SetTextureStageState(1,D3DTSS_COLORARG1,pTempLayer->dwColorArg1);
		pDevice->SetTextureStageState(1,D3DTSS_COLORARG2,pTempLayer->dwColorArg2);
		pDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,pTempLayer->dwAlphaOp);
		pDevice->SetTextureStageState(1,D3DTSS_ALPHAARG1,pTempLayer->dwAlphaArg1);
		pDevice->SetTextureStageState(1,D3DTSS_ALPHAARG2,pTempLayer->dwAlphaArg2);*/
	}



	/*
	* 功能: 修改设定
	* 摘要: -
	* 参数: pszConfigString - 配置信息字符串
	* 返回值: -
	* 作者: -
	* 创建日期: -
	* 修改日志:
	*	2008.05.26 - lpf
	*		增加了2D和3D线性过滤状态信息的设定
	*/
	void render::Interface::ModifySetting(const char *pszConfigString)
	{
		_tagSetting &tSetting = m_tSetting;
		if (pszConfigString)
		{
			utility::Text script;
			script.Create(pszConfigString);

			utility::Text::Node *pNode = script.GetRootNode()->GetChildNode("render");
			if (pNode)
			{
				utility::Text::Variable *pTempVar(NULL);
				// add by lpf 2007-11-27
				utility::Text::Node *pTempNode(NULL);

				//窗口/全屏
				pTempVar = pNode->GetVar("fullscreen");
				tSetting.bIsWindow = (pTempVar)? (pTempVar->GetIntValue() == 1? 0 : 1) : tSetting.bIsWindow;

				pTempVar = pNode->GetVar("hvp");
				tSetting.bHVProcess  = (pTempVar)? pTempVar->GetBOOLValue() : tSetting.bHVProcess;

				pTempVar = pNode->GetVar("texturemode");
				tSetting.bTextureMode32 = (pTempVar) ? ((pTempVar->GetIntValue() == 32) ? TRUE : FALSE) : TRUE;

				pTempVar = pNode->GetVar("2dlinear");
				tSetting.b2DLinear = (pTempVar) ? pTempVar->GetBOOLValue() : FALSE;

				pTempVar = pNode->GetVar("3dlinear");
				tSetting.b3DLinear = (pTempVar) ? pTempVar->GetBOOLValue() : FALSE;

				pTempVar = pNode->GetVar("mipmapping");
				tSetting.bMipMapping = (pTempVar) ? pTempVar->GetBOOLValue() : FALSE;


				pTempVar = pNode->GetVar("bUseVSync");
				tSetting.bPresentationInterval = (pTempVar) ? pTempVar->GetBOOLValue() : FALSE;

				// modify by lpf 2007-11-27 - 修改为fonts节点数据读取
				pTempNode = pNode->GetChildNode("fonts");
				if (pTempNode != NULL)
				{
					for (DWORD dw = 0; dw < pTempNode->GetVarCount(); ++dw)
					{
						if (dw >= MAX_RENDER_LAYER2D_FONT_SIZE)
							break;

						pTempVar = pTempNode->GetVar(dw);
						strcpy(m_tSetting.szFonts[dw],pTempVar->GetStringValue());
					}
				}

				//窗口
				pTempNode = pNode->GetChildNode("window");
				if (pTempNode)
				{
					pTempVar = pTempNode->GetVar("x");
					tSetting.iWndPosX = (pTempVar)? pTempVar->GetIntValue() : tSetting.iWndPosX;

					pTempVar = pTempNode->GetVar("y");
					tSetting.iWndPosY = (pTempVar)? pTempVar->GetIntValue() : tSetting.iWndPosY;

					pTempVar = pTempNode->GetVar("width");
					tSetting.iWndWidth = (pTempVar)? pTempVar->GetIntValue() : tSetting.iWndWidth;

					pTempVar = pTempNode->GetVar("height");
					tSetting.iWndHeight = (pTempVar)? pTempVar->GetIntValue() : tSetting.iWndHeight ;

					pTempVar = pTempNode->GetVar("autostyle");
					tSetting.bWndAutoStyle = (pTempVar)? pTempVar->GetBOOLValue() : tSetting.bWndAutoStyle;//调整窗口风格

					pTempVar = pTempNode->GetVar("autocenter");
					tSetting.bWndAutoCenter = (pTempVar)? pTempVar->GetBOOLValue() : tSetting.bWndAutoCenter;
				}

				//全屏
				pTempNode = pNode->GetChildNode("fullscreen");
				if (pTempNode)
				{
					pTempVar = pTempNode->GetVar("backbufferfmt");
					tSetting.d3dfmtBackBuffer = (pTempVar)? (D3DFORMAT)pTempVar->GetIntValue() : tSetting.d3dfmtBackBuffer;
				}
			}
			script.Destroy(); 
		}
	}
	BOOL render::Interface::CreateDirect3D9(void)
	{
		//select default adapter
		if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		{
			OutputConsole("error: render::Interface: Create DX9.0 Failed.\n");
			//MessageBox(NULL,"创建DirectX 9.0 对象失败，请确认安装有DX9.0驱动程序或者以上版本","致命错误",MB_OK);
			MessageBox(NULL,"Create DX9 faild,Check the version of dx9","Error",MB_OK);
			return FALSE;
		}
		return TRUE;
	}

	void render::Interface::DestroyDirect3D9()
	{
		SAFERELEASE(m_pD3D);
	}



	BOOL render::Texture::CreateBitmap(const char * pszFileName)
	{
		ASSERT(pszFileName);

		BOOL bResult = FALSE;
		char szExt[_MAX_EXT];
		_splitpath(pszFileName,NULL,NULL,NULL,szExt);
		strupr(szExt);
		if (strstr(szExt,".TGA"))
			bResult = BitmapReadFileTGA(pszFileName);
		else 
			if (strstr(szExt,".BMP"))
				bResult = BitmapReadFileBMP(pszFileName);
			else
				if (strstr(szExt,".PSD"))
					bResult = BitmapReadFilePSD(pszFileName);
				else
					if (strstr(szExt,".PCX"))
						bResult = BitmapReadFilePCX(pszFileName);
					else
						if (strstr(szExt,".PPM"))
							bResult = BitmapReadFilePPM(pszFileName);
						else
							if (strstr(szExt,".JPG"))
								bResult = BitmapReadFileJPG(pszFileName);
		if (!bResult)
		{
			OutputError("render::Interface::Texture: Read Bitmap \"%s\" failed.\n",pszFileName);
#ifdef _DEBUG
			MessageBox(NULL,pszFileName,"CreateTexture Failed!",MB_OK);
#endif
			return FALSE;
		}

		//check bitmap width and height
		return TRUE;
	}


	BOOL render::Interface::Reset(void)
	{

		//Device丢失,需要复位
		//纹理
		for(DWORD n = 1; n < MAX_TEXTURE_CACHE_SIZE; n++)
		{
			Texture * pTexture = m_Textures[n].pTexture;
			if (pTexture)
				pTexture->DestroyD3DTexture();
		}   

		// 重新调整Windows风格

		//DestroyLayer();
		ShowWindow(m_hWnd, SW_HIDE);
		DestroyDevice();
		DestroyDirect3D9();


		CreateDirect3D9();
		ASSERT(m_pD3D);

		ShowWindow(m_hWnd, SW_SHOW);
		//初始化Device
		if (!CreateDevice())
		{
			return FALSE;
		}
		ASSERT(m_pD3DDevice);

		//建立图层
		m_pLayer2D->Update();
		m_pLayer3D->Reset();

		//重建
		//纹理

		CRFile* crfile;
		char buff[MAX_PATH];
		for(DWORD n = 0; n < MAX_TEXTURE_CACHE_SIZE; n++)
		{
			Texture * pTexture = m_Textures[n].pTexture;
			if (pTexture)
			{
				if (pTexture->GetFormat() == D3DFMT_DXT1 || pTexture->GetFormat() == D3DFMT_DXT5 ||pTexture->GetFormat() == D3DFMT_DXT3)
				{
					sprintf_s(buff,"Texture/%s",m_Textures[n].pszFileName);
					crfile = rfOpen(buff);
					if (!crfile)
					{
						crfile = rfOpen(m_Textures[n].pszFileName);
					}
					pTexture->Create(m_pD3DDevice,crfile,false,pTexture->GetWidth(),pTexture->GetHeight());
					rfClose(crfile);
					continue;
				}
				else
				{
					pTexture->CreateD3DTexture(m_pD3DDevice);
					pTexture->CopyBitmapToTexture();
				}

			}
		}   
		for(DWORD n = 0; n < MAX_SHADER_GROUP_ARRAY_SIZE; ++n)
		{
			render::Interface::Layer3D::ShaderGroup * pShaderGroup = GetLayer3D()->m_pShaderGroupArray[n];
			if (pShaderGroup)
				pShaderGroup->ReSet();
		}
		//VB
		//VB
		ASSERT(m_pD3DDevice);
		SetDefaultRenderState();


		return TRUE;
	}
	BOOL render::Interface::ResetRelease(void)
	{
		/*m_pLayer2D->UpdateRelease();
		m_pLayer3D->DestroyVertexBuffer();
		for(DWORD n = 0; n < MAX_SHADER_GROUP_ARRAY_SIZE; ++n)
		{
			render::Interface::Layer3D::ShaderGroup * pShaderGroup = GetLayer3D()->m_pShaderGroupArray[n];
			if (pShaderGroup)
				pShaderGroup->ReSet();
		}*/
		m_pLayer3D->GetRendMeshManager()->ResetRelease();
		m_pLayer3D->GetEffectManager()->ResetRelease();
		//重建
		//纹理
		SetDefaultRenderState();
		D3DDISPLAYMODE cm;
		m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&cm);
		GetInfo()->d3dpp.BackBufferFormat =cm.Format; 
		return TRUE;
	}
	BOOL render::Interface::ResetRebuild(void)
	{
		//建立图层
		/*m_pLayer2D->UpdateRebuild();
		m_pLayer3D->CreateVertexBuffer();*/
		m_pLayer3D->GetRendMeshManager()->ResetRebuild();

		SetDefaultRenderState();
		return TRUE;
	}
	//void render::Interface::Layer2D::Font::ResetGBLifeTable(void)
	//{
	//	m_pGBLifeTable = new DWORD[(0xff-0xa1)*(0xf8-0xa1)];
	//	for(DWORD n = 0; n < (0xff-0xa1)*(0xf8-0xa1); n++)
	//	{
	//        m_pGBLifeTable[n] = 0xFFFF0000;
	//	}
	//}
	//
	//void render::Interface::Layer2D::Font::CopyASCToCache(void)
	//{
	//	//写入ASC To CACHE
	//
	//	D3DLOCKED_RECT d3dlr;
	//	m_pTexture->LockRect(0,&d3dlr,NULL,D3DLOCK_NOSYSLOCK);
	//
	//	WORD * pDst = (WORD*)d3dlr.pBits + m_dwASCCacheRowStart * m_dwFontInTextureSize * FONT_TEXTURE_SIZE;
	//	int iCharWidth = m_dwFontInTextureSize / 2;
	//
	//	int iPreLineChars = FONT_TEXTURE_SIZE /iCharWidth;
	//	for(int n = 0; n < 0x100; n++)
	//	{
	//		//计算偏移
	//		BYTE * pDot = &m_pFontDataASC[n * iCharWidth * m_dwFontInTextureSize];
	//		WORD * pBit =  pDst + (n % iPreLineChars) * iCharWidth + (n / iPreLineChars) * FONT_TEXTURE_SIZE * m_dwFontInTextureSize;	
	//
	//		//画点
	//		for(int j = 0; j < m_dwFontInTextureSize;j++)
	//		{
	//			for(int i =0; i < iCharWidth; i++)
	//			{
	//				DWORD c = pDot[i+j*iCharWidth];
	//				c = (c << 8) | 0x0fff;
	//				pBit[i + j * FONT_TEXTURE_SIZE] = (WORD)c;
	//			}
	//		}
	//	}
	//	m_pTexture->UnlockRect(NULL);
	//}

	D3DFORMAT render::Interface::CheckDepthStencilFormat(D3DFORMAT dwBackBufferFormat)
	{
		D3DFORMAT dwDepthStencilFormat = D3DFMT_D16;
		if ( m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,dwBackBufferFormat,dwBackBufferFormat, D3DFMT_D24S8 ) != D3D_OK ) 
		{
			if ( m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dwBackBufferFormat, dwBackBufferFormat, D3DFMT_D24FS8 ) != D3D_OK )
			{
				if ( m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dwBackBufferFormat, dwBackBufferFormat, D3DFMT_D24X4S4 ) != D3D_OK ) 
				{
					if ( m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dwBackBufferFormat, dwBackBufferFormat, D3DFMT_D24FS8 ) != D3D_OK ) 
					{
						dwDepthStencilFormat = D3DFMT_D16;
					} 
					else 
					{
						//support for a d15s1 buffer
						dwDepthStencilFormat = D3DFMT_D15S1;
					}
				} 
				else 
				{
					//support for a d24x4s4 buffer
					dwDepthStencilFormat = D3DFMT_D24X4S4;
				}
			} 
			else 
			{
				//support for a d24s8 buffer
				dwDepthStencilFormat = D3DFMT_D24FS8;
			}

		}
		else 
		{
			//support for a d24fs8 buffer
			dwDepthStencilFormat = D3DFMT_D24S8;
		}
		return dwDepthStencilFormat;
	}

	BOOL render::Texture::BitmapReadFileJPG(const char *pszFileName)
	{
		CRFile *pfile = rfOpen(pszFileName);

		if (!pfile) 
		{
			OutputError("render::Interface::Texture : Open \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		ijlJPEG::JPEG_CORE_PROPERTIES jcprops;
		ijlJPEG::IJLERR ijlErr;
		ijlErr = ijlInit(&jcprops);

		jcprops.JPGBytes = (unsigned char *)pfile->GetData();
		jcprops.JPGSizeBytes = pfile->GetDatalen();
		/*jcprops.JPGChannels = 4;
		jcprops.JPGSubsampling = IJL_NONE;
		jcprops.JPGColor = IJL_OTHER;*/

		if (ijlRead(&jcprops,ijlJPEG::IJL_JBUFF_READPARAMS) != ijlJPEG::IJL_OK)
		{
			OutputError("ijlRead: %s ERROR\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}


		BYTE * pBuffer = new BYTE[jcprops.JPGWidth * jcprops.JPGHeight * jcprops.JPGChannels];

		jcprops.DIBWidth  = jcprops.JPGWidth;
		jcprops.DIBHeight = jcprops.JPGHeight;
		jcprops.DIBPadBytes = 0;
		jcprops.DIBBytes = pBuffer;

		if (jcprops.JPGChannels !=  3)
		{
			SAFEDELETEARRAY(pBuffer);
			rfClose(pfile);
			return FALSE;
		}
		//只支持彩色JPG
		jcprops.JPGColor    = ijlJPEG::IJL_YCBCR;
		jcprops.DIBChannels = 3;
		jcprops.DIBColor    = ijlJPEG::IJL_BGR;

		if(ijlRead(&jcprops,ijlJPEG::IJL_JBUFF_READWHOLEIMAGE) != ijlJPEG::IJL_OK)
		{
			ASSERT(0);
			rfClose(pfile);
			return FALSE;		
		}

		if (!CreateBitmap(jcprops.JPGWidth,jcprops.JPGHeight,0xff000000))
		{
			OutputError("render::Interface::Texture : CreateBitmap() \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			rfClose(pfile);
			return FALSE;
		}

		ijlFree(&jcprops);

		//CONVERT PIXEL TO BITMAP
		struct tagRGB
		{
			BYTE b,g,r;
		};

		tagRGB *p = (tagRGB*)pBuffer;
		for(DWORD y = 0; y < m_dwHeight;y++)
		{
			for(DWORD x = 0; x < m_dwWidth; x ++)
			{
				tagRGB &c = p[x + y * m_dwWidth];
				m_pBitmap[x + y * m_dwWidth] = 0xFF000000 | (DWORD)c.r << 16 | (DWORD)c.g << 8 | (DWORD)c.b;
			}	
		}

		SAFEDELETEARRAY(pBuffer);
		rfClose(pfile);

		return TRUE;
	}

	BOOL render::Texture::BitmapReadFilePCX(CRFile* pFile)
	{
		// PCX header
		struct _tagPCXFILEHEADER{
			char manufacture;
			char version;
			char encoding;
			char bit_per_pixel;
			short xmin,ymin;
			short xmax,ymax;
			short hres;
			short vres;
			char palette[48];
			char reserved;
			char color_planes;
			short bytes_per_line;
			short palette_type;
			char filler[58];
		}; 

		_tagPCXFILEHEADER stHdr;	
		pFile->ReadData(&stHdr,sizeof(stHdr));

		if (!CreateBitmap(stHdr.xmax - stHdr.xmin +1,stHdr.ymax - stHdr.ymin +1,0xff000000))
		{
			OutputError("render::Interface::Texture : CreateBitmap() \"%s\" file failed.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		if (stHdr.bit_per_pixel !=8 )
		{
			OutputError("render::Interface::Texture : Unsupport \"%s\" format.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		if (stHdr.color_planes == 3)
		{
			//24 color
			pFile->Seek(128,SEEK_SET);
			BYTE *pColorPlaneBuf = new BYTE[m_dwWidth * m_dwHeight * 3];
			ASSERT(pColorPlaneBuf);	 

			DWORD n=0,count;
			unsigned char c;
			do
			{
				VERIFY(pFile->ReadChar(c));// & 0xff;

				if( ( c & 0xc0 ) == 0xc0 )
				{
					count=c & 0x3f;
					pFile->ReadChar(c);				
				}
				else 
					count = 1;

				while(count--) pColorPlaneBuf[n++] = c;

			}while(n<m_dwBitmapSize * 3);

			//888->8888

			BYTE  *pBuf  = pColorPlaneBuf;
			DWORD *pData = m_pBitmap;

			for(DWORD y = 0; y < m_dwHeight;y++)
			{
				for(DWORD dwColorPlane = 0; dwColorPlane < 3; dwColorPlane++)
				{
					DWORD dwShift = (2 - dwColorPlane) << 3;
					for(DWORD x= 0; x < m_dwWidth; x++)
					{
						DWORD dwC = (DWORD) * pBuf++;

						pData[x] |= (dwC << dwShift);
					}
				}
				pData+=m_dwWidth;
			}

			SAFEDELETE(pColorPlaneBuf);

		}
		else
		{
			//READ PALETTE
			pFile->Seek(-768l,SEEK_END);
			BYTE pal[768];
			pFile->ReadData(pal,768);

			pFile->Seek(128,SEEK_SET);
			//decode
			for(DWORD y = 0; y < m_dwHeight; y++)
			{
				int bytes = stHdr.bytes_per_line;


				DWORD *pBuf = m_pBitmap + y * m_dwWidth;
				int n=0,count;
				unsigned char c;

				do
				{
					pFile->ReadChar(c);// & 0xff;

					if((c&0xc0)==0xc0)
					{
						count=c&0x3f;
						pFile->ReadChar(c);				
					}
					else 
						count = 1;

					DWORD dwIndex = (c << 1) + c;

					DWORD color  = pal[dwIndex++] << 16;
					color |= pal[dwIndex++] << 8;
					color |= pal[dwIndex];

					while(count--) pBuf[n++] |=color;

				}while(n<bytes);
			}

		}
		return TRUE;
	}
	BOOL render::Texture::BitmapReadFileBMP(CRFile* pFile)
	{
#pragma pack(1)

		struct _tagBMPFILEHEADER{ 
			WORD   bfType; 
			DWORD  bfSize; 
			WORD   bfReserved1; 
			WORD   bfReserved2; 
			DWORD  bfOffBits; 
			DWORD  biSize; 
			LONG   biWidth; 
			LONG   biHeight; 
			WORD   biPlanes; 
			WORD   biBitCount;
			DWORD  biCompression; 
			DWORD  biSizeImage; 
			LONG   biXPelsPerMeter; 
			LONG   biYPelsPerMeter; 
			DWORD  biClrUsed; 
			DWORD  biClrImportant; 
		}; 
#pragma pack()

		_tagBMPFILEHEADER stHdr;
		pFile->ReadData(&stHdr,sizeof(_tagBMPFILEHEADER));

		if (stHdr.bfType != 0x4d42 || stHdr.biCompression != BI_RGB) 
		{
			OutputConsole("error: render::Interface::Texture : \"%s\" format not be support.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		CreateBitmap(stHdr.biWidth,stHdr.biHeight,0xff000000);

		DWORD * pBuf = m_pBitmap;

		switch(stHdr.biBitCount)
		{
		case 8:
			{
				RGBQUAD *pColorTable = (RGBQUAD *)(pFile->GetData()+pFile->GetPosition());

				pFile->Seek(stHdr.bfOffBits,SEEK_SET);

				for(int y = 0;y < stHdr.biHeight; y++)
				{
					for(int x = 0; x < stHdr.biWidth; x++)
					{
						BYTE byIndex;
						pFile->ReadData(&byIndex,sizeof(BYTE));
						DWORD c  = pColorTable[byIndex].rgbRed << 16;
						c |= pColorTable[byIndex].rgbGreen << 8;
						c |= pColorTable[byIndex].rgbBlue;

						*pBuf++ |= c;
					}
				}
			}
			break;
		case 24:
			{
				pFile->Seek(stHdr.bfOffBits,SEEK_SET);
				BYTE r,g,b;
				for(int y=0;y<stHdr.biHeight;y++)
					for(int x=0;x<stHdr.biWidth;x++)
					{
						pFile->ReadData(&b,sizeof(BYTE));
						pFile->ReadData(&g,sizeof(BYTE));
						pFile->ReadData(&r,sizeof(BYTE));
						*pBuf++ |= ( r << 16) | (g << 8) | b;
					}
			}
			break;
		}

		BitmapFlipVertical();
		return TRUE;
	}
	BOOL render::Texture::BitmapReadFileTGA(CRFile* pFile)
	{
		//位图部分
#pragma pack(1)
		struct _tagTGAFILEHEADER
		{	
			BYTE IdentSize;			
			BYTE ColorMapType;		
			BYTE ImageType;			
			WORD ColorMapStart;		
			WORD ColorMapLength;	
			BYTE ColorMapBits;		
			WORD XStart;			
			WORD YStart;			
			WORD Width;				
			WORD Height;			
			BYTE Bits;				
			BYTE Descriptor;		
		};
#pragma pack()

		struct _tagRGBA {
			BYTE Blue,Green,Red,Alpha;
		}; 

		_tagTGAFILEHEADER stHdr;

		pFile->ReadData((BYTE *)&stHdr,sizeof(_tagTGAFILEHEADER));

		if (!CreateBitmap(stHdr.Width,stHdr.Height,0xff000000))
		{
			OutputConsole("error: render::Interface::Texture : CreateBitmap() failed.(\"%s\")\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		DWORD * pBuf = m_pBitmap;

		if (stHdr.ImageType == 2) 
		{
			switch(stHdr.Bits)
			{
			case 16:
				{
					WORD c;
					for(int y=0;y<stHdr.Height;y++)
						for(int x =0;x<stHdr.Width;x++)
						{
							pFile->ReadData((BYTE*)&c,sizeof(WORD));

							DWORD dwB = (c & 0x1f) << 3;
							DWORD dwG = ((c >> 5) & 0x1f) << 3;
							DWORD dwR = ((c >> 10) & 0x1f) << 3;

							*pBuf++ |= (dwR<<16) | (dwG<<8) | dwB;
						}
				}
				break;
			case 24:
				{
					BYTE r,g,b;
					for(int y=0;y<stHdr.Height;y++)
						for(int x=0;x<stHdr.Width;x++)
						{
							pFile->ReadData(&b,sizeof(BYTE));
							pFile->ReadData(&g,sizeof(BYTE));
							pFile->ReadData(&r,sizeof(BYTE));

							*pBuf++ |= (r << 16) | (g << 8) | b;
						}
				}		
				break;
			case 32:
				{
					BYTE r,g,b,a;
					for(int y=0;y<stHdr.Height;y++)
						for(int x=0;x<stHdr.Width;x++)
						{
							pFile->ReadData(&b,sizeof(BYTE));
							pFile->ReadData(&g,sizeof(BYTE));
							pFile->ReadData(&r,sizeof(BYTE));
							pFile->ReadData(&a,sizeof(BYTE));

							*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
						}
				}
				break;
			default:
				{
					OutputConsole("error: render::Interface::Texture : not surport this TGA format yet.\n");
					ASSERT(0);
					return FALSE;
				}
			} 
		}
		else
			if (stHdr.ImageType == 10) //RLE 
			{
				switch(stHdr.Bits)
				{
				case 24:
					{
						BYTE byLen,r,g,b;WORD c = 0;int iPixel=0;
						do
						{
							pFile->ReadData(&byLen,sizeof(BYTE));

							if(byLen >= 0x80)
							{
								byLen -= 0x7F;
								pFile->ReadData(&b,sizeof(BYTE));
								pFile->ReadData(&g,sizeof(BYTE));
								pFile->ReadData(&r,sizeof(BYTE));

								while(byLen --)
								{
									*pBuf++ |= (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
							else
							{
								byLen ++;
								while(byLen --)
								{
									pFile->ReadData(&b,sizeof(BYTE));
									pFile->ReadData(&g,sizeof(BYTE));
									pFile->ReadData(&r,sizeof(BYTE));
									*pBuf++ |= (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
						}  while(iPixel < stHdr.Width*stHdr.Height);
					}
					break;
				case 32:
					{	
						BYTE byLen,r,g,b,a;int iPixel=0;
						do
						{
							pFile->ReadData(&byLen,sizeof(BYTE));

							if(byLen >= 0x80)
							{
								byLen -= 0x7F;
								pFile->ReadData(&b,sizeof(BYTE));
								pFile->ReadData(&g,sizeof(BYTE));
								pFile->ReadData(&r,sizeof(BYTE));
								pFile->ReadData(&a,sizeof(BYTE));
								while(byLen --)
								{
									*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
							else
							{
								byLen ++;
								while(byLen --)
								{
									pFile->ReadData(&b,sizeof(BYTE));
									pFile->ReadData(&g,sizeof(BYTE));
									pFile->ReadData(&r,sizeof(BYTE));
									pFile->ReadData(&a,sizeof(BYTE));
									*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
									iPixel++;
								}
							}
						}  while(iPixel < stHdr.Width*stHdr.Height);
					}
					break;
				case 16:
				default:
					{
						OutputConsole("error: render::Interface::Texture : not surport this TGA format yet.\n");
						ASSERT(0);
						return FALSE;
					}
				}
			}
			else
			{
				OutputConsole("error: render::Interface::Texture : Unknown \"%s\" TGA Format (%d).\n",pFile->GetFileName(),stHdr.ImageType);
				ASSERT(0);
				return FALSE;
			}

			BitmapFlipVertical();
			return TRUE;
	}
	BOOL render::Texture::BitmapReadFilePSD(CRFile* pFile)
	{
#pragma pack(1)
		struct _tagPSDFILEHEADER
		{
			DWORD dwSignature; 
			WORD  wVersion;    
			BYTE  dwReserved[6];
			WORD  wChannels;   
			DWORD dwHeight;
			DWORD dwWidth; 
			WORD  wChannelBit; 
			WORD  wRgbMode;    
		};
#pragma pack()

		_tagPSDFILEHEADER stHdr;

		pFile->ReadData((BYTE *)&stHdr,sizeof(stHdr));

		//change order
		BYTE *pstHdr = (BYTE*)&stHdr.wVersion;

		__asm {
			push edi
				push esi
				push eax

				mov edi,[pstHdr]
			mov esi,edi
				lodsw 
				xchg ah,al
				stosw
				add esi,6
				add edi,6
				lodsw 
				xchg ah,al
				stosw

				lodsd
				bswap eax
				stosd

				lodsd
				bswap eax
				stosd

				lodsw 
				xchg ah,al
				stosw

				lodsw 
				xchg ah,al
				stosw

				pop eax
				pop esi
				pop edi
		}


		if (stHdr.wChannels < 0 || stHdr.wChannels >4 || stHdr.wVersion !=1 || stHdr.wChannelBit !=8 || stHdr.wRgbMode !=3 )
		{
			OutputConsole("error: render::Interface::texture : \"%s\"'s format don't be support.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		// 跳过数据（如调色板）
		DWORD dwModeDataCount;
		pFile->ReadData((BYTE*)&dwModeDataCount,sizeof(DWORD));	

		__asm 
		{
			mov eax,[dwModeDataCount]
			bswap eax
				mov [dwModeDataCount],eax
		}

		pFile->Seek(dwModeDataCount,SEEK_CUR );

		// 跳过数据（如：pen tool paths, etc）
		DWORD dwResourceDataCount;
		pFile->ReadData((BYTE*)&dwResourceDataCount,sizeof(DWORD));

		__asm
		{
			mov eax,[dwResourceDataCount]
			bswap eax
				mov [dwResourceDataCount],eax
		}
		pFile->Seek(dwResourceDataCount,SEEK_CUR );

		// 跳过保留数据
		DWORD dwReservedDataCount;
		pFile->ReadData((BYTE*)&dwReservedDataCount,sizeof(DWORD));
		_asm 
		{
			mov eax,[dwReservedDataCount]
			bswap eax
				mov [dwReservedDataCount],eax
		}
		pFile->Seek(dwReservedDataCount,SEEK_CUR );

		//数据部分

		//分配内存
		CreateBitmap(stHdr.dwWidth,stHdr.dwHeight);	

		// 0: 非压缩 1: RLE压缩
		WORD wCompressionType;
		pFile->ReadData((BYTE*)&wCompressionType,sizeof(WORD));
		__asm
		{
			mov ax,[wCompressionType]
			xchg ah,al
				mov [wCompressionType],ax
		}
		if (wCompressionType > 1)
		{
			OutputConsole("error: render::Interface::texture : \"%s\" file compression type don't be support.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		// 解包数据
		DWORD dwPixelCount = (DWORD)(m_dwWidth * m_dwHeight);

		DWORD * pBuf = NULL;

		BOOL  bHasAlphaChannels = (stHdr.wChannels>3);

		switch(wCompressionType)
		{
		case  0 :
			{
				for (DWORD dwCurChannel =0; dwCurChannel < 3; dwCurChannel++ ) 
				{
					pBuf  = m_pBitmap;
					DWORD dwShift = (2 - dwCurChannel) << 3;

					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						BYTE  c;
						pFile->ReadData(&c,sizeof(BYTE));

						*pBuf++ |= c << dwShift;
					}
				}

				pBuf  = m_pBitmap;

				if (bHasAlphaChannels)
				{
					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						BYTE  c;
						pFile->ReadData(&c,sizeof(BYTE));
						*pBuf++ |= c << 24;
					}
				}
				else
				{
					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						*pBuf++ |= 0xff000000;
					}
				}
			}
			break;
		case 1:
			{
				pFile->Seek(m_dwHeight * stHdr.wChannels * 2, SEEK_CUR );
				for (DWORD dwCurChannel =0; dwCurChannel < stHdr.wChannels; dwCurChannel++ ) 
				{
					//read every channel
					DWORD dwPixel = 0;
					pBuf = m_pBitmap;
					DWORD dwShift = (2 - dwCurChannel) << 3;

					while(dwPixel<dwPixelCount)
					{
						BYTE byLen;
						pFile->ReadData(&byLen,sizeof(BYTE));
						if (byLen < 128 ) //not compress
						{
							byLen++;
							dwPixel += byLen;
							while(byLen)
							{
								BYTE c;
								pFile->ReadData(&c,sizeof(BYTE));
								*pBuf++ |= c << dwShift;
								byLen --;
							}

						}
						else //compress..
						{	
							if(byLen != 128)
							{
								byLen ^= 0xff;
								byLen += 2;
								dwPixel += byLen;
								BYTE c;
								pFile->ReadData(&c,sizeof(BYTE));

								DWORD color = c << dwShift;

								while(byLen)
								{
									*pBuf++ |= color;
									byLen--;
								}
							}
						}
					}
				}
				if (!bHasAlphaChannels)
				{
					pBuf = m_pBitmap;
					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						*pBuf++ |= 0xff000000;
					}
				}

			}
			break;
		default:
			return FALSE;
			break;
		}
		return TRUE;
	}
	BOOL render::Texture::BitmapReadFilePPM(CRFile* pFile)
	{
		// read header

		char szLineBuffer[128];

		pFile->ReadLine(szLineBuffer,128);

		if (szLineBuffer[0] != 'P' || szLineBuffer[1] != '6')
		{
			OutputConsole("error: render::Interface::Texture : Not support \"%s\"'s Format.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		int iSavePosition;
		do
		{
			iSavePosition = pFile->GetPosition();
			pFile->ReadLine(szLineBuffer,128);
		}	
		while(szLineBuffer[0] == '#');

		pFile->SetPosition(iSavePosition);

		pFile->ReadLine(szLineBuffer,128);

		//读取宽高
		int iWidth,iHeight;

		sscanf(szLineBuffer,"%d %d",&iWidth,&iHeight);

		pFile->ReadLine(szLineBuffer,128);

		CreateBitmap(iWidth,iHeight,0xff000000);


		DWORD * pBuf = m_pBitmap;

		for(int y= 0 ; y < iHeight; y++)
		{
			for(int x = 0; x < iWidth; x++)
			{
				BYTE r,g,b;
				pFile->ReadData(&r,sizeof(BYTE));
				pFile->ReadData(&g,sizeof(BYTE));
				pFile->ReadData(&b,sizeof(BYTE));
				pBuf[x] |= (r << 16) | (g << 8) | b;
			}
			pBuf+=iWidth;
		}

		BitmapFlipVertical();
		return TRUE;
	}
	BOOL render::Texture::BitmapReadFileJPG(CRFile* pFile)
	{
		ijlJPEG::JPEG_CORE_PROPERTIES jcprops;
		ijlJPEG::IJLERR ijlErr;
		ijlErr = ijlInit(&jcprops);

		jcprops.JPGBytes = (unsigned char *)pFile->GetData();
		jcprops.JPGSizeBytes = pFile->GetDatalen();
		/*jcprops.JPGChannels = 4;
		jcprops.JPGSubsampling = IJL_NONE;
		jcprops.JPGColor = IJL_OTHER;*/

		if (ijlRead(&jcprops,ijlJPEG::IJL_JBUFF_READPARAMS) != ijlJPEG::IJL_OK)
		{
			OutputError("ijlRead: %s ERROR\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}


		BYTE * pBuffer = new BYTE[jcprops.JPGWidth * jcprops.JPGHeight * jcprops.JPGChannels];

		jcprops.DIBWidth  = jcprops.JPGWidth;
		jcprops.DIBHeight = jcprops.JPGHeight;
		jcprops.DIBPadBytes = 0;
		jcprops.DIBBytes = pBuffer;

		if (jcprops.JPGChannels !=  3)
		{
			SAFEDELETEARRAY(pBuffer);
			return FALSE;
		}
		//只支持彩色JPG
		jcprops.JPGColor    = ijlJPEG::IJL_YCBCR;
		jcprops.DIBChannels = 3;
		jcprops.DIBColor    = ijlJPEG::IJL_BGR;

		if(ijlRead(&jcprops,ijlJPEG::IJL_JBUFF_READWHOLEIMAGE) != ijlJPEG::IJL_OK)
		{
			ASSERT(0);
			return FALSE;		
		}

		if (!CreateBitmap(jcprops.JPGWidth,jcprops.JPGHeight,0xff000000))
		{
			OutputError("render::Interface::Texture : CreateBitmap() \"%s\" file failed.\n",pFile->GetFileName());
			ASSERT(0);
			return FALSE;
		}

		ijlFree(&jcprops);

		//CONVERT PIXEL TO BITMAP
		struct tagRGB
		{
			BYTE b,g,r;
		};

		tagRGB *p = (tagRGB*)pBuffer;
		for(DWORD y = 0; y < m_dwHeight;y++)
		{
			for(DWORD x = 0; x < m_dwWidth; x ++)
			{
				tagRGB &c = p[x + y * m_dwWidth];
				m_pBitmap[x + y * m_dwWidth] = 0xFF000000 | (DWORD)c.r << 16 | (DWORD)c.g << 8 | (DWORD)c.b;
			}	
		}

		SAFEDELETEARRAY(pBuffer);
		return TRUE;
	}