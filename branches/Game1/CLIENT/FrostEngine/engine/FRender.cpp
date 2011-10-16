
#include "stdafx.h"
#include "futility.h"
#include "fconsole.h"
#include "frender.h"
#include "fmath.h"
#include "jpeg\ijl.h"
#include "frender.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

		const D3DXVECTOR3 *pPosBuffer = pBoundingBox->GetConnerVertex();
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

		const D3DXVECTOR3 *pPosBuffer = pBoundingBox->GetConnerVertex();
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
		static D3DXVECTOR3 vConner[8];
		const D3DXVECTOR3 *pConnver = pBoundingBox->GetConnerVertex();
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
		const D3DXVECTOR3 *pvConner = pBoundingBox->GetConnerVertex();

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
		const D3DXVECTOR3 *pvConner = pBoundingBox->GetConnerVertex();

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


		m_fFOVY = D3DXToRadian(45);
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
		return D3DXMatrixPerspectiveFovLH(&m_matProjection,m_fFOVY,m_fAspect,m_fNearPlane,m_fFarPlane);
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
		if( vDir.x > 0.0f )
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
									bResult = TextureReadFileDDS(pszFileName,pDevice);
									bdds = TRUE;
								}

		if (!bResult)
		{
			OutputConsole("error: render::Interface::Texture: read \"%s\" failed.\n",pszFileName);
			MessageBox(NULL,pszFileName,"纹理读取错误",MB_OK);
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
		if(!bdds)
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

		utility::File file;

		if (!file.Open(pszFileName)) 
		{
			OutputError("render::Interface::Texture : Open \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		file.Read(&stHdr,sizeof(stHdr) );


		if (!CreateBitmap(stHdr.xmax - stHdr.xmin +1,stHdr.ymax - stHdr.ymin +1,0xff000000))
		{
			OutputError("render::Interface::Texture : CreateBitmap() \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		if (stHdr.bit_per_pixel !=8 )
		{
			OutputError("render::Interface::Texture : Unsupport \"%s\" format.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		if (stHdr.color_planes == 3)
		{
			//24 color
			file.Seek(128,SEEK_SET);
			BYTE *pColorPlaneBuf = new BYTE[m_dwWidth * m_dwHeight * 3];
			ASSERT(pColorPlaneBuf);	 

			DWORD n=0,count;
			unsigned char c;
			do
			{
				VERIFY(file.ReadChar(c));// & 0xff;

				if( ( c & 0xc0 ) == 0xc0 )
				{
					count=c & 0x3f;
					file.ReadChar(c);				
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
			file.Seek(-768l,SEEK_END);
			BYTE pal[768];
			file.Read(pal,768);

			file.Seek(128,SEEK_SET);
			//decode
			for(DWORD y = 0; y < m_dwHeight; y++)
			{
				int bytes = stHdr.bytes_per_line;


				DWORD *pBuf = m_pBitmap + y * m_dwWidth;
				int n=0,count;
				unsigned char c;

				do
				{
					file.ReadChar(c);// & 0xff;

					if((c&0xc0)==0xc0)
					{
						count=c&0x3f;
						file.ReadChar(c);				
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

		utility::File file;
		if (!file.Open(pszFileName)) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		file.Read((BYTE *)&stHdr,sizeof(_tagTGAFILEHEADER));

		if (!CreateBitmap(stHdr.Width,stHdr.Height,0xff000000))
		{
			OutputConsole("error: render::Interface::Texture : CreateBitmap() failed.(\"%s\")\n",pszFileName);
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
							file.Read((BYTE*)&c,sizeof(WORD));

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
							file.Read(&b,sizeof(BYTE));
							file.Read(&g,sizeof(BYTE));
							file.Read(&r,sizeof(BYTE));

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
							file.Read(&b,sizeof(BYTE));
							file.Read(&g,sizeof(BYTE));
							file.Read(&r,sizeof(BYTE));
							file.Read(&a,sizeof(BYTE));

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
							file.Read(&byLen,sizeof(BYTE));

							if(byLen >= 0x80)
							{
								byLen -= 0x7F;
								file.Read(&b,sizeof(BYTE));
								file.Read(&g,sizeof(BYTE));
								file.Read(&r,sizeof(BYTE));

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
									file.Read(&b,sizeof(BYTE));
									file.Read(&g,sizeof(BYTE));
									file.Read(&r,sizeof(BYTE));
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
							file.Read(&byLen,sizeof(BYTE));

							if(byLen >= 0x80)
							{
								byLen -= 0x7F;
								file.Read(&b,sizeof(BYTE));
								file.Read(&g,sizeof(BYTE));
								file.Read(&r,sizeof(BYTE));
								file.Read(&a,sizeof(BYTE));
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
									file.Read(&b,sizeof(BYTE));
									file.Read(&g,sizeof(BYTE));
									file.Read(&r,sizeof(BYTE));
									file.Read(&a,sizeof(BYTE));
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
				OutputConsole("error: render::Interface::Texture : Unknown \"%s\" TGA Format (%d).\n",pszFileName,stHdr.ImageType);
				ASSERT(0);
				return FALSE;
			}

			BitmapFlipVertical();

			file.Close();
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

		utility::File file;
		if (!file.Open(pszFileName)) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file Failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		_tagBMPFILEHEADER stHdr;
		file.Read(&stHdr,sizeof(_tagBMPFILEHEADER));

		if (stHdr.bfType != 0x4d42 || stHdr.biCompression != BI_RGB) 
		{
			OutputConsole("error: render::Interface::Texture : \"%s\" format not be support.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		CreateBitmap(stHdr.biWidth,stHdr.biHeight,0xff000000);

		DWORD * pBuf = m_pBitmap;

		switch(stHdr.biBitCount)
		{
		case 8:
			{
				RGBQUAD *pColorTable = (RGBQUAD *)(file.GetData()+file.GetPosition());

				file.Seek(stHdr.bfOffBits,SEEK_SET);

				for(int y = 0;y < stHdr.biHeight; y++)
				{
					for(int x = 0; x < stHdr.biWidth; x++)
					{
						BYTE byIndex;
						file.Read(&byIndex,sizeof(BYTE));
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
				file.Seek(stHdr.bfOffBits,SEEK_SET);
				BYTE r,g,b;
				for(int y=0;y<stHdr.biHeight;y++)
					for(int x=0;x<stHdr.biWidth;x++)
					{
						file.Read(&b,sizeof(BYTE));
						file.Read(&g,sizeof(BYTE));
						file.Read(&r,sizeof(BYTE));
						*pBuf++ |= ( r << 16) | (g << 8) | b;
					}
			}
			break;
		}

		BitmapFlipVertical();

		file.Close();

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

		utility::File file;
		if (!file.Open(pszFileName)) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file Failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}	

		file.Read((BYTE *)&stHdr,sizeof(stHdr));

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
			return FALSE;
		}

		// 跳过数据（如调色板）
		DWORD dwModeDataCount;
		file.Read((BYTE*)&dwModeDataCount,sizeof(DWORD));	

		__asm 
		{
			mov eax,[dwModeDataCount]
			bswap eax
				mov [dwModeDataCount],eax
		}

		file.Seek(dwModeDataCount,SEEK_CUR );

		// 跳过数据（如：pen tool paths, etc）
		DWORD dwResourceDataCount;
		file.Read((BYTE*)&dwResourceDataCount,sizeof(DWORD));

		__asm
		{
			mov eax,[dwResourceDataCount]
			bswap eax
				mov [dwResourceDataCount],eax
		}
		file.Seek(dwResourceDataCount,SEEK_CUR );

		// 跳过保留数据
		DWORD dwReservedDataCount;
		file.Read((BYTE*)&dwReservedDataCount,sizeof(DWORD));
		_asm 
		{
			mov eax,[dwReservedDataCount]
			bswap eax
				mov [dwReservedDataCount],eax
		}
		file.Seek(dwReservedDataCount,SEEK_CUR );

		//数据部分

		//分配内存
		CreateBitmap(stHdr.dwWidth,stHdr.dwHeight);	

		// 0: 非压缩 1: RLE压缩
		WORD wCompressionType;
		file.Read((BYTE*)&wCompressionType,sizeof(WORD));
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
						file.Read(&c,sizeof(BYTE));

						*pBuf++ |= c << dwShift;
					}
				}

				pBuf  = m_pBitmap;

				if (bHasAlphaChannels)
				{
					for( DWORD dwPixel = 0; dwPixel < dwPixelCount; dwPixel++ ) 
					{
						BYTE  c;
						file.Read(&c,sizeof(BYTE));
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
				file.Seek(m_dwHeight * stHdr.wChannels * 2, SEEK_CUR );
				for (DWORD dwCurChannel =0; dwCurChannel < stHdr.wChannels; dwCurChannel++ ) 
				{
					//read every channel
					DWORD dwPixel = 0;
					pBuf = m_pBitmap;
					DWORD dwShift = (2 - dwCurChannel) << 3;

					while(dwPixel<dwPixelCount)
					{
						BYTE byLen;
						file.Read(&byLen,sizeof(BYTE));
						if (byLen < 128 ) //not compress
						{
							byLen++;
							dwPixel += byLen;
							while(byLen)
							{
								BYTE c;
								file.Read(&c,sizeof(BYTE));
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
								file.Read(&c,sizeof(BYTE));

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
		file.Close();
		return TRUE;
	}	

	BOOL render::Texture::BitmapReadFilePPM(const char *pszFileName)
	{
		utility::File file;
		if (!file.Open(pszFileName)) 
		{
			OutputConsole("error: render::Interface::Texture : Read \"%s\" failed.\n");
			ASSERT(0);
			return FALSE;
		}

		// read header

		char szLineBuffer[128];

		file.ReadLine(szLineBuffer,128);

		if (szLineBuffer[0] != 'P' || szLineBuffer[1] != '6')
		{
			OutputConsole("error: render::Interface::Texture : Not support \"%s\"'s Format.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		int iSavePosition;
		do
		{
			iSavePosition = file.GetPosition();
			file.ReadLine(szLineBuffer,128);
		}	
		while(szLineBuffer[0] == '#');

		file.SetPosition(iSavePosition);

		file.ReadLine(szLineBuffer,128);

		//读取宽高
		int iWidth,iHeight;

		sscanf(szLineBuffer,"%d %d",&iWidth,&iHeight);

		file.ReadLine(szLineBuffer,128);

		CreateBitmap(iWidth,iHeight,0xff000000);


		DWORD * pBuf = m_pBitmap;

		for(int y= 0 ; y < iHeight; y++)
		{
			for(int x = 0; x < iWidth; x++)
			{
				BYTE r,g,b;
				file.Read(&r,sizeof(BYTE));
				file.Read(&g,sizeof(BYTE));
				file.Read(&b,sizeof(BYTE));
				pBuf[x] |= (r << 16) | (g << 8) | b;
			}
			pBuf+=iWidth;
		}

		BitmapFlipVertical();

		file.Close();

		return TRUE;
	}

	//add by 彭谦******增加压缩纹理接口
	BOOL render::Texture::TextureReadFileDDS(const char *pszFileName,LPDIRECT3DDEVICE9 pDevice,DWORD dwLevel)
	{

		
		utility::File file;
		if (!file.Open(pszFileName)) 
		{
			OutputConsole("error: render::Interface::Texture : Open \"%s\" file Failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		D3DSURFACE_DESC stHdr;
		IDirect3DSurface9* surface;
		D3DXCreateTextureFromFile(pDevice,pszFileName,&m_pD3DTexture);
		m_pD3DTexture->GetSurfaceLevel(0,&surface);
		surface->GetDesc(&stHdr);
		m_dwWidth = stHdr.Width;
		m_dwHeight = stHdr.Height;
		m_dwBitmapSize = m_dwWidth * m_dwHeight;
		surface->Release();
		

		file.Close();

		return TRUE;
	}
	//add by 彭谦******增加压缩纹理接口

	//返回点颜色
	DWORD render::Texture::GetPixel(int iX, int iY)
	{
		//get pixel
		//if(iX>=m_dwWidth||iY>=m_dwHeight)return 0;
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

		utility::File file;
		if (!file.Open(szFileName))
		{
			OutputConsole("error: render::Layer2D::Font: Error in Font::Create(\"%s\"",szFileName);
			ASSERT(0);
			return FALSE;
		}

		m_strFileName = szFileName;

		file.Read(&m_dwFontSize,sizeof(DWORD));
		file.Read(&m_dwFontInTextureSize,sizeof(DWORD));

		//READ ASC 
		//int iBytes = m_dwFontInTextureSize / 8;
		int iCharDotChunkSize = m_dwFontInTextureSize * m_dwFontInTextureSize;

		int iFontASCSize = 0x100 * iCharDotChunkSize;

		BYTE *pFontDotASC = new BYTE[iFontASCSize];	//ASC

		file.Read(pFontDotASC,iFontASCSize);

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
		file.Read(pFontDotSIGN,iFontSignSize);

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
		file.Read(pFontDotCHS,iFontCHSSize);

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

		file.Close();
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
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY  , Layer2D::FVF,D3DPOOL_DEFAULT , &m_pVB,NULL)))	
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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

	void render::Interface::Layer2D::Font::BeginRender()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		// FILL Solid Mode
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
		pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

		// SETUP SAMPLER TEXTURE FILTER
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		//pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
		//pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

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

		pDevice->SetTexture(0,GetTexture());

		pInterface->GetDevice()->SetStreamSource( 0, m_pVB,0, sizeof(tagD3DFVF_2D) );
		pInterface->GetDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);   
	}

	void render::Interface::Layer2D::Font::RenderText(int iX,int iY,const char *szText,int iLen,DWORD dwColor,float fScale)
	{
		//循环打入

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

		render::Interface *pInterface = render::Interface::GetInstance();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles);

	}

	void render::Interface::Layer2D::Font::RenderText(int iX,int iY,const char *szText,DWORD *pColor,int iLen,float fScale)
	{
		//循环打入

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

		render::Interface *pInterface = render::Interface::GetInstance();
		pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles);

	}

	void render::Interface::Layer2D::Font::EndRender()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		pInterface->GetDevice()->SetTexture(0,NULL);
	}

	//__CLASS__(render::Interface::Layer2D)_____
	render::Interface::Layer2D::Layer2D(void) :
	m_dwFillMode(D3DFILL_SOLID)
	{
	}
	render::Interface::Layer2D::~Layer2D(void)
	{
	}

	BOOL render::Interface::Layer2D::Create(const char *pszFontFileName)
	{
		CreateFont(pszFontFileName);

		CreateVertexBuffer();	

		return TRUE;
	}

	BOOL render::Interface::Layer2D::CreateFont(const char *pszFontFileName)
	{
		m_pFont = new Font;
		if (!m_pFont->Create(pszFontFileName))
		{
			OutputConsole("error: render::Layer2D::CreateFont: Create font failed.\n");
			ASSERT(0);
		}
		return FALSE;
	}

	BOOL render::Interface::Layer2D::CreateVertexBuffer()
	{
		render::Interface *pInterface = render::Interface::GetInstance();

		if (FAILED(pInterface->GetDevice()->CreateVertexBuffer( sizeof(tagD3DFVF_2D) * 6 * MAX_RENDER_LAYER2D_VB_SIZE
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY  , Layer2D::FVF,D3DPOOL_DEFAULT , &m_pVB,NULL)))	
		{
			OutputConsole("error : render::Interface:Layer2D Create VextexBuffer failed.\n");
			ASSERT(0);
			return FALSE;
		} 
		return TRUE;
	}

	void render::Interface::Layer2D::DestroyFont()
	{
		SAFEDESTROY(m_pFont);
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

		m_pFont->Reset();
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

		// Setup Alpha 
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		pInterface->SetTexture(0,NULL);

		tagD3DFVF_2D * pVertices;	
		HRESULT hResult = 0;
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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

	//////////////////////////////////////////////////////
	//名称: DrawTextureSubRect()
	//描述:画纹理的某一区域
	//////////////////////////////////////////////////////
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_NONE);

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
		if(FAILED( hResult = m_pVB->Lock( 0, 0, (void**)&pVertices, 0)))
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

	//带颜色码配置
	void render::Interface::Layer2D::TextOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale)
	{
		m_pFont->BeginRender();
		m_pFont->RenderText(iX,iY,pszString,iLen,color,fScale);
		m_pFont->EndRender();
	}

	void render::Interface::Layer2D::_DEBUG_TextOutCache(int iX,int iY)
	{
		m_pFont->_DEBUG_RenderFontTexture(iX,iY);
	}


	//带颜色码配置
	void render::Interface::Layer2D::TextOutEx(int iX,int iY,const char *pszString,int iLen,float fScale)
	{
		static char  szStringTemp[256];
		static DWORD dwColorArray[256];
		int i = 0,n = 0; 
		DWORD dwCurColor = 0xFFFFFFFF;
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

		m_pFont->BeginRender();
		m_pFont->RenderText(iX,iY,szStringTemp,dwColorArray,lstrlen(szStringTemp),fScale);
		m_pFont->EndRender();
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

	int  render::Interface::Layer2D::CalcTextWidth(const char *pszTextBuffer,int iLen)
	{
		ASSERT(m_pFont);
		return iLen * m_pFont->GetFontSize() / 2;
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

		m_dwShaderCount = pShaderNode->GetChildCount();
		m_pShaderArray = new _tagShader[m_dwShaderCount];
		ZeroMemory(m_pShaderArray,m_dwShaderCount * sizeof(_tagShader));
		for(DWORD n = 0; n < m_dwShaderCount; n++)
		{
			utility::Text::Node *pNode = pShaderNode->GetChildNode(n);

			_tagShader *pShader = &m_pShaderArray[n];

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
				MessageBox(NULL,pszTexPath,"错误的alphatest参数",MB_OK);
				return FALSE;
			}

			static const char *szAlphaBlendMode[] = {"additive","alpha","color","reserved"};
			pTempVar = pNode->GetVar("alphablend");
			pShader->dwAlphaBlendMode = (pTempVar)? pTempVar->GetEnumValue(szAlphaBlendMode,4) : 0;
			if (pShader->dwAlphaBlendMode == 0xffffffff)
			{
				MessageBox(NULL,pszTexPath,"错误的ALPHABLEND参数",MB_OK);
				return FALSE;
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
				//pShader->dwStreamType = LMST_POSDIFSPCTEX0;//POSCOLTEX0
				static const char *szTypeNewSteam[] = {"poscol","posdiftex0","posdifspctex0"};
				pShader->dwStreamType = pNode->GetVar("stream")->GetEnumValue(szTypeNewSteam,3);
				if (pShader->dwStreamType == 0XFFFFFFFF)
				{
					pShader->dwStreamType = LMST_POSDIFSPCTEX0;//POSCOLTEX0
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
		DWORD n = 0;
		for(; n < m_dwTexCount[1]; n++)
		{
			_tagTex1 *pTex1 = &m_pTex1Array[n];
			pTex1->dwTex1Handle = m_pdwTexHandleArray[1][n];
			pTex1->pTex0Head = NULL;
		}
		//根据shader生成TEX1--TEX0关系表
		for(n = 0; n < m_dwShaderCount; n++)
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

				//link tex0 to tex1
				pTex0->pNext = pTex1->pTex0Head;
				pTex1->pTex0Head = pTex0;
			}
		}	


		script.Destroy();

		return TRUE;
	}

	BOOL render::Interface::Layer3D::ShaderGroup::CreateFromFile(const char * pszFileName,DWORD dwTextureProcision)
	{
		render::Interface *pInterface = render::Interface::GetInstance();

		utility::File file;
		if (!file.Open(pszFileName))
		{
			OutputConsole("error: render::Layer3D::ShaderGroup:: File not found!.");
			MessageBox(NULL,pszFileName,"错误",MB_OK);
			return FALSE;
		}

		char path_buffer[_MAX_PATH];

		utility::File::GetFilePath(pszFileName,path_buffer);

		char *szShaderBuffer = new char[file.GetLength() + 1];

		memcpy(szShaderBuffer,file.GetData(),file.GetLength());
		szShaderBuffer[file.GetLength()] = '\0';
		file.Close();

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
		DWORD n = 0;
		for(; n < m_dwTexCount[1]; n++)
		{
			//读取纹理文件
			pInterface->DestroyTexture(m_pdwTexHandleArray[1][n]);		
		}
		SAFEDELETE(m_pdwTexHandleArray[1]);

		for(n = 0; n < m_dwTexCount[0]; n++)
		{
			//读取纹理文件
			pInterface->DestroyTexture(m_pdwTexHandleArray[0][n]);		
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

	void render::Interface::Layer3D::ShaderGroup::RenderAlphaTestMode()
	{
		//setup stream format
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		//render opaque mode : mask and none
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState( D3DRS_ZENABLE, TRUE);	

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
					if (pShader->pTriangleAlphaTestHead)
					{
						//设置Shader属性
						pInterface->Cull(!pShader->bTwoSide);
						//设置alpha test mode
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

						_SetTextureOp(pShader);
						//设置顶点流格式
						pLayer3D->SetStreamType(pShader->dwStreamType);

						switch(pShader->dwStreamType)
						{
						case LMST_POSCOL:
							__RenderPOSCOL(pShader->pTriangleAlphaTestHead);
							break;
						case LMST_POSDIFTEX0:
							__RenderPOSDIFTEX0(pShader->pTriangleAlphaTestHead);
							break;
						case LMST_POSDIFSPCTEX0:
							__RenderPOSDIFSPCTEX0(pShader->pTriangleAlphaTestHead);
							break;
						}
						//渲染完清空
						pShader->pTriangleAlphaTestHead = NULL;
					}
					//渲染
					pShader = pShader->pNext;
				}

				pTex0 = pTex0->pNext;
			}
		}
	}

	void render::Interface::Layer3D::ShaderGroup::RenderAlphaBlendMode()
	{
		//setup stream format
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D = pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		//渲染透明部分
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);	

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
					if (pShader->pTriangleAlphaBlendHead)
					{
						//设置Shader属性
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
						_SetTextureOp(pShader);
						//设置定点流格式
						pLayer3D->SetStreamType(pShader->dwStreamType);

						switch(pShader->dwStreamType)
						{
						case LMST_POSCOL:
							__RenderPOSCOL(pShader->pTriangleAlphaBlendHead);
							break;
						case LMST_POSDIFTEX0:
							__RenderPOSDIFTEX0(pShader->pTriangleAlphaBlendHead);
							break;
						case LMST_POSDIFSPCTEX0:
							__RenderPOSDIFSPCTEX0(pShader->pTriangleAlphaBlendHead);
							break;
						}
						//渲染完清空
						pShader->pTriangleAlphaBlendHead = NULL;
					}
					//渲染
					pShader = pShader->pNext;
				}

				pTex0 = pTex0->pNext;
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

						pLayer3D->SetStreamType(LMST_POSDIFTEX0);
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
	//					case LMST_POSCOL:
	//						__RenderPOSCOL(pShader->pTraingleAlphaBlendTopHead);
	//						break;
	//					case LMST_POSDIFTEX0:
	//						__RenderPOSDIFTEX0(pShader->pTraingleAlphaBlendTopHead);
	//						break;
	//					case LMST_POSDIFSPCTEX0:
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
		D3DCOLOR    *pCacheCol(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;

		_tagTriangle *pPolygon = pHead;

		if (!pPolygon) 
			//没有,直接返回
			return ;

		//填充VB进行绘图

		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

		DWORD dwVerticesCount = 0;	
		while(pPolygon)
		{
			memcpy(pCachePos + dwVerticesCount,&pPolygon->pos[0],sizeof(D3DXVECTOR3) * 3);
			memcpy(pCacheCol + dwVerticesCount,&pPolygon->color[0],sizeof(D3DCOLOR) * 3);

			dwVerticesCount +=3;

			//记算越界,清空缓冲
			if (dwVerticesCount >= LAYER3D_VERTEXBUFFER_SIZE)
			{
				pVBPOS->Unlock();
				pVBCOL->Unlock();

				pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, LAYER3D_VERTEXBUFFER_SIZE / 3);
				pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
				pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);

				dwVerticesCount = 0;
			}

			pPolygon = pPolygon->pNext;
		}

		if (dwVerticesCount)//some left
		{
			pVBPOS->Unlock();
			pVBCOL->Unlock();
			pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwVerticesCount / 3);
		}
	}

	void render::Interface::Layer3D::ShaderGroup::__RenderPOSDIFTEX0(_tagTriangle *pHead)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DXVECTOR2 *pCacheTex0(NULL);

		LPDIRECT3DVERTEXBUFFER9 pVBPOS    = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOLDIF = pLayer3D->m_pVB_COLOR_DIFFUSE;
		LPDIRECT3DVERTEXBUFFER9 pVBTEX0   = pLayer3D->m_pVB_TEX0;

		_tagTriangle *pPolygon = pHead;

		if (!pPolygon) 
			//没有,直接返回
			return ;

		//填充VB进行绘图

		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOLDIF->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
		pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

		DWORD dwVerticesCount = 0;	
		while(pPolygon)
		{
			memcpy(pCachePos + dwVerticesCount,&pPolygon->pos[0],sizeof(D3DXVECTOR3) * 3);
			memcpy(pCacheCol + dwVerticesCount,&pPolygon->color[0],sizeof(D3DCOLOR)  * 3);
			memcpy(pCacheTex0 + dwVerticesCount,&pPolygon->uv0[0],sizeof(D3DXVECTOR2)* 3);

			dwVerticesCount +=3;

			//记算越界,清空缓冲
			if (dwVerticesCount >= LAYER3D_VERTEXBUFFER_SIZE)
			{
				pVBPOS->Unlock();
				pVBCOLDIF->Unlock();
				pVBTEX0->Unlock();

				pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, LAYER3D_VERTEXBUFFER_SIZE / 3);
				pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
				pVBCOLDIF->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
				pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

				dwVerticesCount = 0;
			}
			pPolygon = pPolygon->pNext;
		}

		if (dwVerticesCount)//some left
		{
			pVBPOS->Unlock();
			pVBCOLDIF->Unlock();
			pVBTEX0->Unlock();

			pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwVerticesCount / 3);
		}
	}

	void render::Interface::Layer3D::ShaderGroup::__RenderPOSDIFSPCTEX0(_tagTriangle *pHead)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DCOLOR    *pCacheColSpc(NULL);
		D3DXVECTOR2 *pCacheTex0(NULL);

		LPDIRECT3DVERTEXBUFFER9 pVBPOS    = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOLDIF = pLayer3D->m_pVB_COLOR_DIFFUSE;
		LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC = pLayer3D->m_pVB_COLOR_SPECULAR;
		LPDIRECT3DVERTEXBUFFER9 pVBTEX0   = pLayer3D->m_pVB_TEX0;

		_tagTriangle *pPolygon = pHead;

		if (!pPolygon) 
			//没有,直接返回
			return ;

		//填充VB进行绘图

		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOLDIF->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
		pVBCOLSPC->Lock(0,0, (void**)&pCacheColSpc,D3DLOCK_DISCARD);
		pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

		DWORD dwVerticesCount = 0;	
		while(pPolygon)
		{
			memcpy(pCachePos + dwVerticesCount,&pPolygon->pos[0],sizeof(D3DXVECTOR3) * 3);
			memcpy(pCacheCol + dwVerticesCount,&pPolygon->color[0],sizeof(D3DCOLOR)  * 3);
			memcpy(pCacheColSpc + dwVerticesCount,&pPolygon->specular[0],sizeof(D3DCOLOR)  * 3);
			memcpy(pCacheTex0 + dwVerticesCount,&pPolygon->uv0[0],sizeof(D3DXVECTOR2)* 3);

			dwVerticesCount +=3;

			//记算越界,清空缓冲
			if (dwVerticesCount >= LAYER3D_VERTEXBUFFER_SIZE)
			{
				pVBPOS->Unlock();
				pVBCOLDIF->Unlock();
				pVBCOLSPC->Unlock();
				pVBTEX0->Unlock();

				pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, LAYER3D_VERTEXBUFFER_SIZE / 3);
				pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
				pVBCOLDIF->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
				pVBCOLSPC->Lock(0,0, (void**)&pCacheColSpc,D3DLOCK_DISCARD);
				pVBTEX0->Lock( 0,0, (void**)&pCacheTex0,D3DLOCK_DISCARD);

				dwVerticesCount = 0;
			}
			pPolygon = pPolygon->pNext;
		}

		if (dwVerticesCount)//some left
		{
			pVBPOS->Unlock();
			pVBCOLDIF->Unlock();
			pVBCOLSPC->Unlock();
			pVBTEX0->Unlock();

			pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwVerticesCount / 3);
		}
	}


	//__CLASS__(render::Interface::Layer3D)__
	render::Interface::Layer3D::Layer3D(void) :
	m_dwFillMode(D3DFILL_SOLID)
	{
		ZeroMemory(m_pShaderGroupArray,MAX_SHADER_GROUP_ARRAY_SIZE * sizeof(ShaderGroup *));
		m_dwShaderGroupArrayUsage = 0;
	}

	render::Interface::Layer3D::~Layer3D(void)
	{
	}

	BOOL render::Interface::Layer3D::CreateVertexBuffer()
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		//流缓冲
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * LAYER3D_VERTEXBUFFER_SIZE
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY ,0,D3DPOOL_DEFAULT , &m_pVB_POSITION,NULL)))
		{
			OutputConsole("error: render::Interface:CREATE 3D POSITION VEXTEXBUFFER ERROR.\n");
			ASSERT(0);
			return FALSE;
		}

		//if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR3) * LAYER3D_VERTEXBUFFER_SIZE
		//	, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY ,0,D3DPOOL_DEFAULT , &m_pVB_NORMAL,NULL)))	
		//{
		//	OutputConsole("error: render::Interface:Create 3d normal vextexbuffer failed.\n");
		//	ASSERT(0);
		//	return FALSE;
		//}

		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DCOLOR) * LAYER3D_VERTEXBUFFER_SIZE
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY ,0,D3DPOOL_DEFAULT , &m_pVB_COLOR_DIFFUSE,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d color vextexbuffer failed.\n");
			ASSERT(0);
			return FALSE;
		}		

		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DCOLOR) * LAYER3D_VERTEXBUFFER_SIZE
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY ,0,D3DPOOL_DEFAULT , &m_pVB_COLOR_SPECULAR,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d color vextexbuffer failed.\n");
			ASSERT(0);
			return FALSE;
		}

		if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * LAYER3D_VERTEXBUFFER_SIZE
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY ,0,D3DPOOL_DEFAULT , &m_pVB_TEX0,NULL)))	
		{
			OutputConsole("error: render::Interface:Create 3d tex0 vextexbuffer failed.\n");
			ASSERT(0);
			return FALSE;
		}		

		//if (FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * LAYER3D_VERTEXBUFFER_SIZE
		//	, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY ,0,D3DPOOL_DEFAULT , &m_pVB_TEX1,NULL)))	
		//{
		//	OutputConsole("error: render::Interface:Create 3d tex1 vextexbuffer failed.\n");
		//	ASSERT(0);
		//	return FALSE;
		//}		
		return TRUE;
	}

	void render::Interface::Layer3D::DestroyVertexBuffer()
	{
		SAFERELEASE(m_pVB_POSITION);
		SAFERELEASE(m_pVB_COLOR_DIFFUSE);
		SAFERELEASE(m_pVB_COLOR_SPECULAR);
		SAFERELEASE(m_pVB_TEX0);
	}

	void render::Interface::Layer3D::CreateVertexDecl()
	{
		//顶点
		D3DVERTEXELEMENT9 vertexDecl0[] =  //pos | color
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
		};

		D3DVERTEXELEMENT9 vertexDecl1[] = //pos | color diffuse | tex0
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },		
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};

		D3DVERTEXELEMENT9 vertexDecl2[] = //pos | color diffuse | color specular | tex0
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,1 },
			{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },		
			{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
		};


		//Create VetexShader
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		HRESULT hr;
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
			OutputError("CREATE VERTEX DECLARE 1 FAILED.\n");
		}
	}

	void render::Interface::Layer3D::DestroyVertexDecl()
	{	
		SAFERELEASE(m_pVertexDeclarationArray[0]);
		SAFERELEASE(m_pVertexDeclarationArray[1]);
		SAFERELEASE(m_pVertexDeclarationArray[2]);
	}

	void render::Interface::Layer3D::SetTexture(DWORD dwShaderGroupHandle,DWORD dwShaderIndex)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		ShaderGroup *pShaderGroup = m_pShaderGroupArray[dwShaderGroupHandle];
		ShaderGroup::_tagShader * pShader = & pShaderGroup->m_pShaderArray[dwShaderIndex];
		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		pInterface->SetTexture(1,NULL);
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
		case LMST_POSCOL:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
			break;
		case LMST_POSDIFTEX0:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
			pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
			break;
		case LMST_POSDIFSPCTEX0:
			pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
			pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
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


	BOOL render::Interface::Layer3D::Create(void)
	{
		CreateVertexBuffer();
		CreateVertexDecl();
		CreatePolgyonArray();
		return 0;
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
			MessageBox(NULL,pszString,"严重错误",MB_OK);
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
		SAFEDESTROY(m_pShaderGroupArray[dwSGHandle]);
		m_dwShaderGroupArrayUsage--;
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

	void render::Interface::Layer3D::Render(void)
	{
		//缓冲:渲染
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_FILLMODE, m_dwFillMode);
		//Setup Texture Filter
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);

		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(2,m_pVB_COLOR_SPECULAR,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));


		//Enable SPECULAR
		pDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE);

		for(DWORD n = 0; n < MAX_SHADER_GROUP_ARRAY_SIZE; n ++)
		{
			ShaderGroup *pShaderGroup = m_pShaderGroupArray[n];
			if (pShaderGroup)
				pShaderGroup->RenderAlphaTestMode();
		}

		BOOL bFogEnable;
		pDevice->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
		pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);	
		for(DWORD n = 0; n < MAX_SHADER_GROUP_ARRAY_SIZE; n ++)
		{
			ShaderGroup *pShaderGroup = m_pShaderGroupArray[n];
			if (pShaderGroup)
				pShaderGroup->RenderAlphaBlendMode();
		}
		for(DWORD n = 0; n < MAX_SHADER_GROUP_ARRAY_SIZE; n ++)
		{
			ShaderGroup *pShaderGroup = m_pShaderGroupArray[n];
			if (pShaderGroup)
				pShaderGroup->RenderTransparentMode();
		}
		//pDevice->Clear(0, NULL,D3DCLEAR_ZBUFFER,0, 1.0f, 0 );
		//for(DWORD n = 0; n < MAX_SHADER_GROUP_ARRAY_SIZE; n ++)
		//{
		//	ShaderGroup *pShaderGroup = m_pShaderGroupArray[n];
		//	if (pShaderGroup)
		//		pShaderGroup->RenderAlphaBlendTopMode();
		//}


		pDevice->SetStreamSource(0,NULL,0,0);
		pDevice->SetStreamSource(1,NULL,0,0);
		pDevice->SetStreamSource(2,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);

		pDevice->SetRenderState(D3DRS_FOGENABLE,bFogEnable);
	}

	void render::Interface::Layer3D::Clear(void)
	{
		//清空多边形渲染缓冲
		m_dwPolygonArrayUsage = 0;
	}

	void render::Interface::Layer3D::Reset(void)
	{
		DestroyVertexDecl();
		DestroyVertexBuffer();
		CreateVertexDecl();
		CreateVertexBuffer();
	}

	void render::Interface::Layer3D::DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		ShaderGroup *pShaderGroup = m_pShaderGroupArray[dwShaderGroup];

		//if (dwShaderIndex >= pShaderGroup->m_dwShaderCount)
		//{
		//	dwShaderIndex = 0;
		//}

		ShaderGroup::_tagShader * pShader = & pShaderGroup->m_pShaderArray[dwShaderIndex];

		pInterface->SetTexture(0,pShaderGroup->m_pdwTexHandleArray[0][pShader->tLayer[0].dwTexture]);
		//pInterface->SetTexture(1,NULL);

		pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[LMST_POSDIFTEX0]);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(2,m_pVB_COLOR_SPECULAR,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DXVECTOR2));

		LPDIRECT3DVERTEXBUFFER9 pVBPOS  = m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL  = m_pVB_COLOR_DIFFUSE;
		LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC  = m_pVB_COLOR_SPECULAR;
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
		pDevice->SetStreamSource(2,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}

	void render::Interface::Layer3D::SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColorDiffuse,const D3DCOLOR *pColorSpecular,DWORD dwMode)
	{
		ShaderGroup *pShaderGroup = m_pShaderGroupArray[dwShaderGroup];
		ASSERT(pShaderGroup);

		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = &pShaderGroup->m_pShaderArray[dwShaderIndex];

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
		ShaderGroup *pShaderGroup = m_pShaderGroupArray[dwShaderGroup];
		ASSERT(pShaderGroup);

		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = &pShaderGroup->m_pShaderArray[dwShaderIndex];

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
		ShaderGroup *pShaderGroup = m_pShaderGroupArray[dwShaderGroup];
		ASSERT(pShaderGroup);

		render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = &pShaderGroup->m_pShaderArray[dwShaderIndex];

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


	// ----------------------------------------------------
	// 调试和编辑器使用，画线
	// 使用渲染句柄
	// 
	// ----------------------------------------------------
	void render::Interface::Layer3D::_DEBUG_DrawLine(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,D3DCOLOR dwColor)//2
	{
		//设置定点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     

		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

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
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);		

		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

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
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

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

	void render::Interface::Layer3D::_DEBUG_DrawTriangle(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,D3DCOLOR dwColor)//2
	{
		//设置顶点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     

		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

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
		pDevice->SetStreamSource(1,NULL,0,0);
	}

	void render::Interface::Layer3D::_DEBUG_DrawTriangle(DWORD dwTexturHandle,const D3DXVECTOR3 *pVtx,const D3DXVECTOR2 *pUV0,D3DCOLOR dwColor)
	{
		//设置顶点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();
		pInterface->SetTexture(0,dwTexturHandle);

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

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
		pDevice->SetStreamSource(2,m_pVB_COLOR_SPECULAR,0,sizeof(D3DCOLOR));
		pDevice->SetStreamSource(3,m_pVB_TEX0,0,sizeof(D3DCOLOR));

		pLayer3D->SetStreamType(LMST_POSDIFTEX0);

		D3DXVECTOR3 *pCachePos(NULL);
		D3DCOLOR    *pCacheCol(NULL);
		D3DCOLOR    *pCacheColSpc(NULL);
		D3DXVECTOR2 *pCacheUV0(NULL);
		LPDIRECT3DVERTEXBUFFER9 pVBPOS = pLayer3D->m_pVB_POSITION;
		LPDIRECT3DVERTEXBUFFER9 pVBCOL = pLayer3D->m_pVB_COLOR_DIFFUSE;
		LPDIRECT3DVERTEXBUFFER9 pVBCOLSPC = pLayer3D->m_pVB_COLOR_SPECULAR;
		LPDIRECT3DVERTEXBUFFER9 pVbUV0 = pLayer3D->m_pVB_TEX0;

		//填充VB进行绘图
		pVBPOS->Lock( 0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
		pVBCOL->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
		pVBCOLSPC->Lock( 0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
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
		pDevice->SetStreamSource(2,NULL,0,0);
		pDevice->SetStreamSource(3,NULL,0,0);
	}								   

	void render::Interface::Layer3D::_DEBUG_DrawWireBox(const D3DXVECTOR3 *pCenter,float fRadius,D3DCOLOR dwColor)
	{
		//设置顶点流格式
		render::Interface *pInterface = render::Interface::GetInstance();
		Layer3D *pLayer3D= pInterface->GetLayer3D();

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

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
		Layer3D *pLayer3D= pInterface->GetLayer3D();

		LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
		pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);     


		pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

		//
		pDevice->SetStreamSource(0,m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
		pDevice->SetStreamSource(1,m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));

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
		ZeroMemory(m_pTextureArray,sizeof(Texture*)*MAX_TEXTURE_CACHE_SIZE);
		m_dwTextureCnt = 0;
		m_dwTextureMemorySize = 0;
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
			MessageBox(NULL,"检测硬件支持失败,显卡不支持游戏所要求的特性\n原因可能是显卡较旧或者未安装最新的驱动所致","致命错误",MB_OK);
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
	}

	void render::Interface::DefaultSetting(const char * pszConfigString)
	{
		_tagSetting &tSetting = m_tSetting;

		//设置确省的
		//当前字库名称
		strncpy(tSetting.szFont,"12.font",_MAX_PATH - 1);
		tSetting.szFont[_MAX_PATH -1] = 0;

		tSetting.bHVProcess  = FALSE;

		tSetting.iWndPosX = 10;
		tSetting.iWndPosY = 10;
		tSetting.iWndWidth = 640;
		tSetting.iWndHeight= 480;

		tSetting.bWndAutoStyle  = TRUE; 
		tSetting.bWndAutoCenter = FALSE;

		tSetting.bIsWindow = TRUE;
		tSetting.bMipMapping = FALSE;

		tSetting.bTextureMode32 = TRUE;
		tSetting.d3dfmtBackBuffer = D3DFMT_X8R8G8B8;//

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
		m_stInfo.d3dpp.BackBufferCount  = 1;
		m_stInfo.d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		m_stInfo.d3dpp.hDeviceWindow = m_hWnd;
		m_stInfo.d3dpp.Windowed = m_tSetting.bIsWindow;


		if (m_stInfo.d3dpp.Windowed)
		{
			D3DDISPLAYMODE dm;
			m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dm);
			m_stInfo.d3dpp.BackBufferFormat = dm.Format;
			m_stInfo.d3dpp.BackBufferWidth  = m_tSetting.iWndWidth;
			m_stInfo.d3dpp.BackBufferHeight = m_tSetting.iWndHeight;
			m_stInfo.d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			m_stInfo.d3dpp.PresentationInterval =  D3DPRESENT_INTERVAL_DEFAULT;
		}
		else
		{
			m_stInfo.d3dpp.BackBufferFormat = m_tSetting.d3dfmtBackBuffer;
			m_stInfo.d3dpp.BackBufferWidth  = m_tSetting.iWndWidth;
			m_stInfo.d3dpp.BackBufferHeight = m_tSetting.iWndHeight;
			m_stInfo.d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			m_stInfo.d3dpp.PresentationInterval =  D3DPRESENT_INTERVAL_DEFAULT;
		}

		//设置Depth Buffer Format
		m_stInfo.d3dpp.AutoDepthStencilFormat = CheckDepthStencilFormat(m_stInfo.d3dpp.BackBufferFormat);
		m_stInfo.d3dpp.EnableAutoDepthStencil = TRUE;

		HRESULT hResult;

		if (m_tSetting.bHVProcess)
		{
			//Want use HW VP
			if (FAILED(hResult = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING ,	&m_stInfo.d3dpp, &m_pD3DDevice ) ) )
			{
				OutputConsole("error: render::Interface: Create D3D Device(HWVP) Failed.\n");
				MessageBox(NULL,"创建DirectX 驱动失败，请确认显卡支持硬件加速","致命错误",MB_OK);
				return FALSE;
			}
		}
		else
		{
			if( FAILED(hResult = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING ,	&m_stInfo.d3dpp, &m_pD3DDevice ) ) )
			{ 
				OutputConsole("error: render::Interface: Create D3D Device(SWVP) Failed.\n");
				MessageBox(NULL,"创建DirectX 驱动失败，请确认显卡支持硬件加速","致命错误",MB_OK);
				return FALSE;
			}
		}

		//READ DEVICE CAPS 
		m_pD3DDevice->GetDeviceCaps(&m_stInfo.d3dCaps);

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
		return SUCCEEDED(m_pD3DDevice->BeginScene());
	}

	void render::Interface::EndScene()
	{
		//刷新屏幕
		m_pD3DDevice->EndScene();
		if (m_pD3DDevice->Present( NULL, NULL, NULL, NULL ) == D3DERR_DEVICELOST)
		{

			HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
			while(hr == D3DERR_DEVICELOST)
			{
				hr = m_pD3DDevice->TestCooperativeLevel();
			}

			if (hr = D3DERR_DEVICENOTRESET)
			{
				hr = m_pD3DDevice->Reset(&GetInfo()->d3dpp);
				if (hr != S_OK)
				{
					Reset();
				}

			}

		}
	}



	BOOL render::Interface::CreateLayer(void)
	{
		//建立渲染层
		m_pLayer2D = new Layer2D;
		m_pLayer2D->Create(m_tSetting.szFont);

		m_pLayer3D = new Layer3D;
		m_pLayer3D->Create();

		return TRUE;
	}

	void render::Interface::DestroyLayer(void)
	{
		SAFEDESTROY(m_pLayer2D);
		SAFEDESTROY(m_pLayer3D);
	}


	render::Texture * render::Interface::GetTexture(DWORD  dwTextureHandle)
	{
		if (!dwTextureHandle) 
		{
			OutputConsole("error: GetTexture() texture handle must not be NULL.\n");
			ASSERT(0);
			return NULL;
		}
		return m_pTextureArray[dwTextureHandle - 1];
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

		DWORD dwTex = 0;
		for(; dwTex < MAX_TEXTURE_CACHE_SIZE; dwTex++)
		{
			if (!m_pTextureArray[dwTex])
			{
				m_pTextureArray[dwTex] = pTexture;
				break;
			}
		}

		m_dwTextureCnt++;
		m_dwTextureMemorySize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));

		return (int)dwTex + 1;
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

		DWORD dwTex = 0;
		for(; dwTex < MAX_TEXTURE_CACHE_SIZE; dwTex++)
		{
			if (!m_pTextureArray[dwTex])
			{
				m_pTextureArray[dwTex] = pTexture;
				break;
			}
		}

		m_dwTextureCnt++;
		m_dwTextureMemorySize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));

		return (int)dwTex + 1;
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

		DWORD dwTex = 0;
		for(; dwTex < MAX_TEXTURE_CACHE_SIZE; dwTex++)
		{
			if (!m_pTextureArray[dwTex])
			{
				m_pTextureArray[dwTex] = pTexture;
				break;
			}
		}

		m_dwTextureCnt++;
		m_dwTextureMemorySize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));

		return (int)dwTex + 1;
	}

	void render::Interface::DestroyTexture(DWORD dwTextureHandle)
	{
		ASSERT(dwTextureHandle < MAX_TEXTURE_CACHE_SIZE);

		if (dwTextureHandle)
		{
			Texture * pTexture = m_pTextureArray[dwTextureHandle - 1];
			m_dwTextureMemorySize -= pTexture->GetWidth() * pTexture->GetHeight() * ((m_tSetting.bTextureMode32)? sizeof(DWORD): sizeof(WORD));
			SAFEDESTROY(pTexture);
			m_pTextureArray[dwTextureHandle - 1] = NULL;
			m_dwTextureCnt --;
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

		Texture * pTexture  = m_pTextureArray[dwTextureHandle - 1];
		ASSERT(pTexture);
		m_pD3DDevice->SetTexture(dwStage,pTexture->GetD3DTexture());
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
		for(DWORD n = 0; n < MAX_TEXTURE_CACHE_SIZE; n++)
		{
			render::Texture * pTexture = m_pTextureArray[n];
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

		_tagShader::_tagLayer *pTempLayer(NULL);
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

		pTempLayer = &pShader->tLayer[1];
		pDevice->SetTextureStageState(1,D3DTSS_COLOROP,pTempLayer->dwColorOp);
		pDevice->SetTextureStageState(1,D3DTSS_COLORARG1,pTempLayer->dwColorArg1);
		pDevice->SetTextureStageState(1,D3DTSS_COLORARG2,pTempLayer->dwColorArg2);
		pDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,pTempLayer->dwAlphaOp);
		pDevice->SetTextureStageState(1,D3DTSS_ALPHAARG1,pTempLayer->dwAlphaArg1);
		pDevice->SetTextureStageState(1,D3DTSS_ALPHAARG2,pTempLayer->dwAlphaArg2);
	}

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

				//窗口/全屏
				pTempVar = pNode->GetVar("window");
				tSetting.bIsWindow = (pTempVar)? pTempVar->GetBOOLValue() : tSetting.bIsWindow;

				pTempVar = pNode->GetVar("hvp");
				tSetting.bHVProcess  = (pTempVar)? pTempVar->GetBOOLValue() : tSetting.bHVProcess;

				pTempVar = pNode->GetVar("texturemode32");
				tSetting.bTextureMode32 = (pTempVar)? pTempVar->GetBOOLValue() : TRUE;

				pTempVar = pNode->GetVar("mipmapping");
				tSetting.bMipMapping = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;

				pTempVar = pNode->GetVar("font");
				if (pTempVar)
				{
					strcpy(m_tSetting.szFont,pTempVar->GetStringValue());
				}

				utility::Text::Node *pTempNode(NULL);

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
			MessageBox(NULL,"创建DirectX 9.0 对象失败，请确认安装有DX9.0驱动程序或者以上版本","致命错误",MB_OK);
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
			MessageBox(NULL,pszFileName,"CreateTexture Failed!",MB_OK);
			return FALSE;
		}

		//check bitmap width and height
		return TRUE;
	}


	BOOL render::Interface::Reset(void)
	{

		//Device丢失,需要复位
		//纹理
		for(DWORD n = 0; n < MAX_TEXTURE_CACHE_SIZE; n++)
		{
			Texture * pTexture = m_pTextureArray[n];
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
		CreateDevice();
		ASSERT(m_pD3DDevice);

		//建立图层
		m_pLayer2D->Update();
		m_pLayer3D->Reset();

		//重建
		//纹理
		for(DWORD n = 0; n < MAX_TEXTURE_CACHE_SIZE; n++)
		{
			Texture * pTexture = m_pTextureArray[n];
			if (pTexture)
			{
				pTexture->CreateD3DTexture(m_pD3DDevice);
				pTexture->CopyBitmapToTexture();
			}
		}   
		//VB
		//VB
		ASSERT(m_pD3DDevice);
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
		if ( m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,dwBackBufferFormat,dwBackBufferFormat, D3DFMT_D24FS8 ) != D3D_OK ) 
		{
			if ( m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dwBackBufferFormat, dwBackBufferFormat, D3DFMT_D24S8 ) != D3D_OK )
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
				dwDepthStencilFormat = D3DFMT_D24S8;
			}

		}
		else 
		{
			//support for a d24fs8 buffer
			dwDepthStencilFormat = D3DFMT_D24FS8;
		}
		return dwDepthStencilFormat;
	}

	BOOL render::Texture::BitmapReadFileJPG(const char *pszFileName)
	{
		utility::File file;

		if (!file.Open(pszFileName)) 
		{
			OutputError("render::Interface::Texture : Open \"%s\" file failed.\n",pszFileName);
			ASSERT(0);
			return FALSE;
		}

		ijlJPEG::JPEG_CORE_PROPERTIES jcprops;
		ijlJPEG::IJLERR ijlErr;
		ijlErr = ijlInit(&jcprops);

		jcprops.JPGBytes = (unsigned char *)file.GetData();
		jcprops.JPGSizeBytes = file.GetLength();
		/*jcprops.JPGChannels = 4;
		jcprops.JPGSubsampling = IJL_NONE;
		jcprops.JPGColor = IJL_OTHER;*/

		if (ijlRead(&jcprops,ijlJPEG::IJL_JBUFF_READPARAMS) != ijlJPEG::IJL_OK)
		{
			OutputError("ijlRead: %s ERROR\n",pszFileName);
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
			OutputError("render::Interface::Texture : CreateBitmap() \"%s\" file failed.\n",pszFileName);
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
		file.Close();

		return TRUE;
	}

