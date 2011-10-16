#include "StdAfx.h"

#include "..\frostengine\utility.h"
#include "..\frostengine\console.h"
#include "..\frostengine\render.h"

#include "gamefootprint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GameFootprint::GameFootprint(void)
{
	m_pFootprintFree = NULL;
	m_pFootprintUsed = NULL;

	decalEpsilon = 0.25F;
}

GameFootprint::~GameFootprint()
{
}

void GameFootprint::Destroy(void)
{
	//TODo
	tagFootprint *pFootprint,*p;

	pFootprint = m_pFootprintUsed;	
	while(pFootprint)
	{
		p = pFootprint;
		pFootprint = pFootprint->pNext;
		SAFEDELETE(p);
	}

	pFootprint = m_pFootprintFree;	
	while(pFootprint)
	{
		p = pFootprint;
		pFootprint = pFootprint->pNext;
		SAFEDELETE(p);
	}
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle); 
}

GameFootprint::tagFootprint* GameFootprint::NewFootprint()
{
	tagFootprint *pNewFootprint = NULL;

	//检查是否有空余的
	if (m_pFootprintFree)
	{
		pNewFootprint = m_pFootprintFree;
		m_pFootprintFree = m_pFootprintFree->pNext;
	}
	else
	{
        //没有
		pNewFootprint = new tagFootprint;
	}

	//加入活动列表
	pNewFootprint->pNext = m_pFootprintUsed;
	m_pFootprintUsed = pNewFootprint;    
	return pNewFootprint;
}

BOOL GameFootprint::Create()
{
	//建立Shader
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroupFromFile("model/footprint/shader");
	return TRUE;
}

void GameFootprint::Render(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	//static D3DXVECTOR2 uv0[6] = { D3DXVECTOR2(0,0),D3DXVECTOR2(1,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,1)};
	
	//static D3DXVECTOR2 uv[36];
	static DWORD color[1024];   	
	
	static D3DXMATRIX  matWorld,matTrans,matScale;

	//setup stream format
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);	
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);	
	pInterface->Cull(FALSE);
	//设置Alpha blend mode
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//int j = 0; 
	tagFootprint *pFootprint = m_pFootprintRenderHead;
	while(pFootprint)
	{
		DWORD dwColor = pFootprint->dwColor;

		for(int i = 0; i < decalTriangleCount*3; i++)
		{
			color[i] = dwColor;
		}

		//pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,0,pFootprint->dwCount,pFootprint->pos,pFootprint->m_pUV0,color);
		//*****************************************************************
		//D3DXVECTOR3 *vertexArray2 = new D3DXVECTOR3[decalTriangleCount*3];
		//D3DXVECTOR2 *texcoordArray2 = new D3DXVECTOR2[decalTriangleCount*3];
		//int va2index = 0;
		//for (int i=0; i<decalTriangleCount; i++)
		//{
		//	vertexArray2[va2index] = vertexArray[triangleArray[i].index[0]];	texcoordArray2[va2index] = texcoordArray[triangleArray[i].index[0]];
		//	vertexArray2[va2index].y+=0.04; va2index++;
		//	vertexArray2[va2index] = vertexArray[triangleArray[i].index[1]];	texcoordArray2[va2index] = texcoordArray[triangleArray[i].index[1]];
		//	vertexArray2[va2index].y+=0.04; va2index++;
		//	vertexArray2[va2index] = vertexArray[triangleArray[i].index[2]];	texcoordArray2[va2index] = texcoordArray[triangleArray[i].index[2]];
		//	vertexArray2[va2index].y+=0.04; va2index++;
		//}
		//pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,0,decalTriangleCount,vertexArray2,texcoordArray2,color);
		//delete [] vertexArray2;
		//delete [] texcoordArray2;
		//*****************************************************************
		
		pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle, 0, pFootprint->dwCount, pFootprint->pos, pFootprint->m_pUV0, color);

		// 原代码
		//pLayer3D->DrawTriangleArray(m_dwShaderGroupHandle,0,2,pFootprint->pos,uv0,color);
		

		pFootprint = pFootprint->pNextRender;
	//	j++;
	}
	//OutputError("%d\n",j);

}

void GameFootprint::Update()
{
	DWORD dwCurTime = timeGetTime();

	m_pFootprintRenderHead = NULL;

	tagFootprint *pFootprint,**ppFootprint = &m_pFootprintUsed;

	while(*ppFootprint)
	{
		pFootprint = *ppFootprint;

		//计算流逝的生命时间
		DWORD dwTimeElapse = dwCurTime - pFootprint->dwLifeTimeStart;

		//可见度Visibility 插值
		float fVisibility = 1.0f - (float)dwTimeElapse / (float)pFootprint->dwLifeTimeLength;
		DWORD c = (DWORD)( 255 * fVisibility);
		pFootprint->dwColor = (c << 24) | 0x00FFFFFF;

		//减少生命
		if (dwTimeElapse > pFootprint->dwLifeTimeLength)
		{
			//死亡，移动到空闲表中
			*ppFootprint = pFootprint->pNext;
			pFootprint->pNext = m_pFootprintFree;
			m_pFootprintFree = pFootprint;
		}
		else
		{
			//装入渲染链
			pFootprint->pNextRender = m_pFootprintRenderHead;
			m_pFootprintRenderHead  = pFootprint;
			ppFootprint = &pFootprint->pNext;
		}
	}
}

//**********************************************************
//decal添加函数
void GameFootprint::InitDecal(const D3DXVECTOR3& center, const D3DXVECTOR3& normal, const D3DXVECTOR3& tangent, float width, float height, float depth,
												long triangleCount, const Triangle *triangle, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal2)
{
	decalCenter = center;
	decalNormal = normal;

	D3DXVECTOR3 binormal;// = CrossProduct(normal, tangent);
	D3DXVec3Cross(&binormal, &normal, &tangent);

	// Calculate boundary planes
	float d = DotProduct(center, tangent);
	leftPlane = D3DXVECTOR4(tangent.x, tangent.y, tangent.z, width * 0.5F - d);
	rightPlane = D3DXVECTOR4(-tangent.x, -tangent.y, -tangent.z, width * 0.5F + d);

	d = DotProduct(center, binormal);
	bottomPlane = D3DXVECTOR4(binormal.x, binormal.y, binormal.z, height * 0.5F - d);
	topPlane = D3DXVECTOR4(-binormal.x, -binormal.y, -binormal.z, height * 0.5F + d);

	d = DotProduct(center, normal);
	frontPlane = D3DXVECTOR4(-normal.x, -normal.y, -normal.z, depth + d);
	backPlane = D3DXVECTOR4(normal.x, normal.y, normal.z, depth - d);

	for (long a = 0; a < maxDecalVertices; a++)
	{
		//初始化顶点法线
		vertexNormalArray[a] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	}

	// Begin with empty mesh
	decalVertexCount = 0;
	decalTriangleCount = 0;

	// Add this point, determine which surfaces may be affected by this decal
	// and call ClipMesh() for each one.
	ClipMesh(triangleCount, triangle, vertex,  normal2);

	// Assign texture mapping coordinates
	float one_over_w = 1.0F / width;
	float one_over_h = 1.0F / height;
	for (long a = 0; a < decalVertexCount; a++)
	{
		D3DXVECTOR3 v = vertexArray[a] - center;
		//D3DXVec3Normalize(&v, &v);
		float s = DotProduct(v, tangent);
		s = s * one_over_w + 0.5F;
		float t = DotProduct(v, binormal);
		t = t * one_over_h + 0.5F;
		texcoordArray[a] = D3DXVECTOR2(s, t);
		
	}
	
}

//加入多边形
bool GameFootprint::AddPolygon(long vertexCount, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal)
{
	long count = decalVertexCount;
	if (count + vertexCount >= maxDecalVertices) return (false);

	// Add polygon as a triangle fan
	Triangle *triangle = triangleArray + decalTriangleCount;
	decalTriangleCount += vertexCount - 2;
	for (long a = 2; a < vertexCount; a++)
	{
		triangle->index[0] = (unsigned short) count;
		triangle->index[1] = (unsigned short) (count + a - 1);
		triangle->index[2] = (unsigned short) (count + a);
		triangle++;
	}

	// Assign vertex colors
	float f = 1.0F / (1.0F - decalEpsilon);
	for (long b = 0; b < vertexCount; b++)
	{
		vertexArray[count] = vertex[b];
		const D3DXVECTOR3& n = normal[b];
		float alpha = (DotProduct(decalNormal, n) / D3DXVec3Length(&n) - decalEpsilon) * f;
		colorArray[count] = ColorRGBA(1.0F, 1.0F, 1.0F, (alpha > 0.0F) ? alpha : 0.0F);
		count++;
	}

	decalVertexCount = count;
	return (true);
}

//裁减mesh
void GameFootprint::ClipMesh(long triangleCount, const Triangle *triangle, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal)
{
	D3DXVECTOR3		newVertex[9];
	D3DXVECTOR3		newNormal[9];

	// Clip one triangle at a time
	for (long a = 0; a < triangleCount; a++)
	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];

		const D3DXVECTOR3& v1 = vertex[i1];
		const D3DXVECTOR3& v2 = vertex[i2];
		const D3DXVECTOR3& v3 = vertex[i3];

		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &(v2 - v1), &(v3 - v1));
		if (DotProduct(decalNormal, cross) > decalEpsilon * D3DXVec3Length(&cross))
		{
			newVertex[0] = v1;
			newVertex[1] = v2;
			newVertex[2] = v3;

			newNormal[0] = normal[i1];
			newNormal[1] = normal[i2];
			newNormal[2] = normal[i3];

			long count = ClipPolygon(3, newVertex, newNormal, newVertex, newNormal);
			if ((count != 0) && (!AddPolygon(count, newVertex, newNormal))) break;
		}

		triangle++;
	}
}

//裁减多边形
long GameFootprint::ClipPolygon(long vertexCount, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal, D3DXVECTOR3 *newVertex, 
						D3DXVECTOR3 *newNormal) const
{
	D3DXVECTOR3		tempVertex[9];
	D3DXVECTOR3		tempNormal[9];

	// Clip against all six planes
	long count = ClipPolygonAgainstPlane(leftPlane, vertexCount, vertex, normal, tempVertex, tempNormal);
	if (count != 0)
	{
		count = ClipPolygonAgainstPlane(rightPlane, count, tempVertex, tempNormal, newVertex, newNormal);
		if (count != 0)
		{
			count = ClipPolygonAgainstPlane(bottomPlane, count, newVertex, newNormal, tempVertex, tempNormal);
			if (count != 0)
			{
				count = ClipPolygonAgainstPlane(topPlane, count, tempVertex, tempNormal, newVertex, newNormal);
				if (count != 0)
				{
					count = ClipPolygonAgainstPlane(backPlane, count, newVertex, newNormal, tempVertex, tempNormal);
					if (count != 0)
					{
						count = ClipPolygonAgainstPlane(frontPlane, count, tempVertex, tempNormal, newVertex, newNormal);
					}
				}
			}
		}
	}

	return (count);
}

//裁减多边形
long GameFootprint::ClipPolygonAgainstPlane(const D3DXVECTOR4& plane, long vertexCount, const D3DXVECTOR3 *vertex, 
									const D3DXVECTOR3 *normal, D3DXVECTOR3 *newVertex, D3DXVECTOR3 *newNormal)
{
	bool	negative[10];

	// Classify vertices
	long negativeCount = 0;
	for (long a = 0; a < vertexCount; a++)
	{
		bool neg = (DotProductEx(plane, vertex[a]) < 0.0F);
		negative[a] = neg;
		negativeCount += neg;
	}

	// Discard this polygon if it's completely culled
	if (negativeCount == vertexCount) return (0);

	long count = 0;
	for (long b = 0; b < vertexCount; b++)
	{
		// c is the index of the previous vertex
		long c = (b != 0) ? b - 1 : vertexCount - 1;

		if (negative[b])
		{
			if (!negative[c])
			{
				// Current vertex is on negative side of plane,
				// but previous vertex is on positive side.
				const D3DXVECTOR3& v1 = vertex[c];
				const D3DXVECTOR3& v2 = vertex[b];
				float t = DotProductEx(plane, v1) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				const D3DXVECTOR3& n1 = normal[c];
				const D3DXVECTOR3& n2 = normal[b];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}
		}
		else
		{
			if (negative[c])
			{
				// Current vertex is on positive side of plane,
				// but previous vertex is on negative side.
				const D3DXVECTOR3& v1 = vertex[b];
				const D3DXVECTOR3& v2 = vertex[c];
				float t = DotProductEx(plane, v1) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				const D3DXVECTOR3& n1 = normal[b];
				const D3DXVECTOR3& n2 = normal[c];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}

			// Include current vertex
			newVertex[count] = vertex[b];
			newNormal[count] = normal[b];
			count++;
		}
	}

	// Return number of vertices in clipped polygon
	return (count);
}