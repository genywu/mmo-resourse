#include "StdAfx.h"
#include ".\shadowvolume.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CShadowVolume::CShadowVolume(void)
{
	m_pVertices  = new D3DXVECTOR3[MAX_VERTICES];
	m_pEdges	 = new MeshEdge[MAX_EDGES];
	m_pFaceLights= new BOOL[MAX_TRIANGLES];

	m_dwNumVertices=0;
	m_dwNumEdges=0;
	m_bAvailable=false;
}

CShadowVolume::~CShadowVolume(void)
{
	SAFEDELETEARRAY(m_pVertices);
	SAFEDELETEARRAY(m_pEdges);
	SAFEDELETEARRAY(m_pFaceLights);
}

void CShadowVolume::Reset() 
{ 
	m_dwNumVertices=0; 
	m_bAvailable=false;
}

bool CShadowVolume::AddFaceEdge(int face,int v1,int v2)
{
	for(DWORD i=0;i<this->m_dwNumEdges;i++)
	{
		if(m_pEdges[i].v[0]==v1&&m_pEdges[i].v[1]==v2)
		{
			return false;
		}
		else if(m_pEdges[i].v[1]==v1&&m_pEdges[i].v[0]==v2)
		{
			if(m_pEdges[i].face[1]!=-1)return false;
			m_pEdges[i].face[1]=face; 
			return true;
		}
	}
	this->m_pEdges[m_dwNumEdges].v[0] =v1;
	this->m_pEdges[m_dwNumEdges].v[1] =v2;
	this->m_pEdges[m_dwNumEdges].face[0]=face;
	m_dwNumEdges++;
	return true;
}

bool CShadowVolume::IsOccludedMesh(const model::Buffer *pBuffer)
{
	const model::Mesh *pMesh=pBuffer->GetAttachMesh();
	model::Mesh::tagTriangle *faces=pMesh->GetTriangleBuffer();
	int faceCount=pMesh->GetTriangleCount();
	for(int i=0;i<faceCount;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(!AddFaceEdge(i,faces[i].dwPNCIndex[j],faces[i].dwPNCIndex[(j+1)%3]))return false;
		}
	}
	for(DWORD n=0;n<m_dwNumEdges;n++)
	{
		if(m_pEdges[n].face[1]==-1)return false;
	}
	return true;
}

BOOL CShadowVolume::AnalyseMesh(const model::Buffer *pBuffer)
{
	const model::Mesh *pMesh=pBuffer->GetAttachMesh();
	
	m_dwNumEdges=0;

	int count=pMesh->GetTriangleCount();
	count*=3;

	if (count >= MAX_EDGES)
		return FALSE;
	for(int i=0;i<count;i++)
	{
		m_pEdges[i].v[0]=-1;
		m_pEdges[i].v[1]=-1;
		m_pEdges[i].face[0]=-1;
		m_pEdges[i].face[1]=-1;
	}
	this->m_bOccluded=this->IsOccludedMesh(pBuffer);
	if(m_bOccluded)
	{
		if (count/3 >= MAX_TRIANGLES)
			return FALSE;
		if (count >= MAX_VERTICES )
			return FALSE;
		//m_pVertices=new D3DXVECTOR3[count];
	}
	else
	{
		//m_pVertices=new D3DXVECTOR3[count*6];
		if (count * 6 >= MAX_VERTICES )
			return FALSE;
	}

	return TRUE;
}

void CShadowVolume::BuildFromAccludedMesh(const model::Buffer *pBuffer,const D3DXVECTOR3 &vLight)
{
	const model::Mesh *pMesh=pBuffer->GetAttachMesh();
	int faceCount=pMesh->GetTriangleCount();
	model::Mesh::tagTriangle *faces=pMesh->GetTriangleBuffer();
	D3DXVECTOR3  *vertices=pBuffer->GetPosBuffer();
	D3DXVECTOR3 v1,v2,v3,v4,vCross1,vCross2,vNormal;
	for(int i=0;i<faceCount;i++)
	{
		vCross1=vertices[faces[i].dwPNCIndex[2]]-vertices[faces[i].dwPNCIndex[1]];
		vCross2=vertices[faces[i].dwPNCIndex[1]]-vertices[faces[i].dwPNCIndex[0]];
		D3DXVec3Cross(&vNormal,&vCross1,&vCross2);
		this->m_pFaceLights[i]=D3DXVec3Dot(&vNormal,&vLight)>=0.0f;
	}
	for(DWORD i=0;i<m_dwNumEdges;i++)
	{
		if(m_pFaceLights[m_pEdges[i].face[0]]&&!m_pFaceLights[m_pEdges[i].face[1]])
		{
			v1=vertices[m_pEdges[i].v[0]];
			v2=vertices[m_pEdges[i].v[1]];
			v3=v1-vLight*50;
			v4=v2-vLight*50;
			m_pVertices[m_dwNumVertices++]=v1;
			m_pVertices[m_dwNumVertices++]=v2;
			m_pVertices[m_dwNumVertices++]=v3;
			m_pVertices[m_dwNumVertices++]=v2;
			m_pVertices[m_dwNumVertices++]=v4;
			m_pVertices[m_dwNumVertices++]=v3;
		}
		else if(m_pFaceLights[m_pEdges[i].face[1]]&&!m_pFaceLights[m_pEdges[i].face[0]])
		{
			v1=vertices[m_pEdges[i].v[1]];
			v2=vertices[m_pEdges[i].v[0]];
			v3=v1-vLight*50;
			v4=v2-vLight*50;
			m_pVertices[m_dwNumVertices++]=v1;
			m_pVertices[m_dwNumVertices++]=v2;
			m_pVertices[m_dwNumVertices++]=v3;
			m_pVertices[m_dwNumVertices++]=v2;
			m_pVertices[m_dwNumVertices++]=v4;
			m_pVertices[m_dwNumVertices++]=v3;
		}
	}
	this->m_bAvailable=true;
}

void CShadowVolume::BuildFromNonAccludedMeshMesh(const model::Buffer *pBuffer,const D3DXVECTOR3 &vLight)
{
	const model::Mesh *pMesh=pBuffer->GetAttachMesh();
	DWORD faceCount=pMesh->GetTriangleCount();
	int edgeCount=0;
	model::Mesh::tagTriangle *faces = pMesh->GetTriangleBuffer();
	D3DXVECTOR3 *vertices=pBuffer->GetPosBuffer();
	D3DXVECTOR3 v1,v2,v3,v4,vNormal,vCross1,vCross2;
	for( DWORD i=0; i<faceCount; ++i )
	{
		vCross1=vertices[faces[i].dwPNCIndex[2]]-vertices[faces[i].dwPNCIndex[1]];
		vCross2=vertices[faces[i].dwPNCIndex[1]]-vertices[faces[i].dwPNCIndex[0]];
		D3DXVec3Cross(&vNormal,&vCross1,&vCross2);
		if(D3DXVec3Dot(&vNormal,&vLight)>=0.0f)
		{
			for(int j=0;j<3;j++)
			{
				m_pEdges[edgeCount].v[0]=faces[i].dwPNCIndex[j];
				m_pEdges[edgeCount].v[1]=faces[i].dwPNCIndex[(j+1)%3];
				edgeCount++;
			}
		}
	}
	this->m_dwNumVertices=0;
	for(int i=0;i<edgeCount;i++)
	{
		v1=vertices[m_pEdges[i].v[0]];
		v2=vertices[m_pEdges[i].v[1]];
		v3=v1 - vLight*50;
		v4=v2 - vLight*50;
		m_pVertices[m_dwNumVertices++]=v1;
		m_pVertices[m_dwNumVertices++]=v2;
		m_pVertices[m_dwNumVertices++]=v3;
		m_pVertices[m_dwNumVertices++]=v2;
		m_pVertices[m_dwNumVertices++]=v4;
		m_pVertices[m_dwNumVertices++]=v3;
	}
	m_bAvailable=true;
}

BOOL CShadowVolume::BuildFromMesh(model::Buffer *pBuffer, D3DXVECTOR3 &vLight)
{
	const model::Mesh *pMesh = pBuffer->GetAttachMesh();
	ASSERT(pMesh);
	if (!AnalyseMesh(pBuffer))
	{
		return FALSE;
	}
	if(m_bOccluded)
	{
		this->BuildFromAccludedMesh(pBuffer,vLight);
	}
	else
	{
		this->BuildFromNonAccludedMeshMesh(pBuffer,vLight);
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the volume geometry, assumes that stencil buffer is already
//		 configured correctly.
//-----------------------------------------------------------------------------
void CShadowVolume::Render()
{
	if(!m_bAvailable)
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDev = pInterface->GetDevice();

    pDev->SetFVF( D3DFVF_XYZ );
	pDev->SetTexture(0, NULL);
	pDev->SetTransform( D3DTS_WORLD, &m_matWorld );
	if (m_dwNumVertices == 0)
	{
		return;
	}

	pDev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwNumVertices/3,
		m_pVertices, sizeof(D3DXVECTOR3) );
}

void CShadowVolume::SetWorldMatrix(D3DXMATRIX *pm)
{
	if(pm)
	{
		memcpy(&m_matWorld, pm, sizeof(D3DXMATRIX));
	}
	else
	{
		D3DXMatrixIdentity(&m_matWorld);
	}
}