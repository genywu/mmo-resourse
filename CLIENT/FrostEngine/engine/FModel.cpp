
#include "stdafx.h"
#include "futility.h"
#include "fconsole.h"
#include "frender.h"
#include "fmodel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////
//功能:更新数据
//CLASS___(model::Buffer)______
model::Buffer::Buffer():
m_pAttachMesh(NULL),
m_pPosBuffer(NULL),
m_pNmlBuffer(NULL),
m_pUV0Buffer(NULL), 
m_pColBufferDiffuse(NULL),
m_pColBufferSpecular(NULL)
{

}
model::Buffer::~Buffer()
{
	ASSERT(!m_pPosBuffer);
	ASSERT(!m_pNmlBuffer);
	ASSERT(!m_pUV0Buffer);
	ASSERT(!m_pColBufferDiffuse);		
	ASSERT(!m_pColBufferSpecular);		
}

void model::Buffer::operator =( const model::Buffer &buffer)
{
	memcpy(m_pPosBuffer,buffer.GetPosBuffer(),sizeof(D3DXVECTOR3) * m_dwPNCCount); 
	memcpy(m_pNmlBuffer,buffer.GetNmlBuffer(),sizeof(D3DXVECTOR3) * m_dwPNCCount); 
	memcpy(m_pColBufferDiffuse,buffer.GetColBufferDiffuse(),sizeof(D3DCOLOR) * m_dwPNCCount);
	memcpy(m_pColBufferSpecular,buffer.GetColBufferSpecular(),sizeof(D3DCOLOR) * m_dwPNCCount);
	memcpy(m_pUV0Buffer,buffer.GetUV0Buffer(),sizeof(D3DXVECTOR2) * m_dwUV0Count);
}

void model::Buffer::Create(const model::Mesh *pMesh)
{
	m_dwPNCCount = pMesh->GetPNCCount();
	m_dwUV0Count = pMesh->GetUV0Count();
	//m_dwUV1Count = pMesh->GetUV1Count();	

	//运算缓冲
	m_pPosBuffer = new D3DXVECTOR3[m_dwPNCCount]; 
	m_pNmlBuffer = new D3DXVECTOR3[m_dwPNCCount]; 
	m_pColBufferDiffuse = new D3DCOLOR[m_dwPNCCount];
	m_pColBufferSpecular= new D3DCOLOR[m_dwPNCCount];
	m_pUV0Buffer = new D3DXVECTOR2[m_dwUV0Count];

	m_pAttachMesh = pMesh;
}

void model::Buffer::Destroy()
{
	SAFEDELETEARRAY(m_pPosBuffer);
	SAFEDELETEARRAY(m_pNmlBuffer);
	SAFEDELETEARRAY(m_pColBufferDiffuse);
	SAFEDELETEARRAY(m_pColBufferSpecular);
	SAFEDELETEARRAY(m_pUV0Buffer);
	//	SAFEDELETEARRAY(m_pUV1Buffer);  	
}

void model::Buffer::Render(DWORD dwShaderGroupHandle,DWORD dwMode)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	D3DXVECTOR3 temp[3];
	D3DXVECTOR2 uv0[3];//,uv1[3];
	DWORD dwColorDiffuse[3],dwColorSpecular[3];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();

	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		temp[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		dwColorDiffuse[0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
		dwColorDiffuse[1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
		dwColorDiffuse[2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

		dwColorSpecular[0] = m_pColBufferSpecular[pTriangle->dwPNCIndex[0]];
		dwColorSpecular[1] = m_pColBufferSpecular[pTriangle->dwPNCIndex[1]];
		dwColorSpecular[2] = m_pColBufferSpecular[pTriangle->dwPNCIndex[2]];

		uv0[0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		uv0[1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		uv0[2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];

		pLayer3D->SendTriangleToCache(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,dwColorDiffuse,dwColorSpecular,dwMode);
	}    
}
void model::Buffer::RenderTransparent(DWORD dwShaderGroupHandle,DWORD dwMode)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	D3DXVECTOR3 temp[3];
	D3DXVECTOR2 uv0[3];//,uv1[3];
	DWORD dwColorDiffuse[3];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();

	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		temp[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		dwColorDiffuse[0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
		dwColorDiffuse[1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
		dwColorDiffuse[2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

		uv0[0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		uv0[1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		uv0[2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];

		pLayer3D->SendTriangleToCache(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,dwColorDiffuse,dwMode);
	}
}
void model::Buffer::Render(DWORD dwShaderGroupHandle,DWORD dwShaderIndex,DWORD dwMode)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	D3DXVECTOR3 temp[3];
	D3DXVECTOR2 uv0[3];//,uv1[3];
	DWORD color[6];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();

	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		temp[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		color[0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
		color[1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
		color[2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

		color[3] = m_pColBufferSpecular[pTriangle->dwPNCIndex[0]];
		color[4] = m_pColBufferSpecular[pTriangle->dwPNCIndex[1]];
		color[5] = m_pColBufferSpecular[pTriangle->dwPNCIndex[2]];

		uv0[0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		uv0[1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		uv0[2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];

		pLayer3D->SendTriangleToCache(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,color,&color[3],dwMode);
	}

}

void model::Buffer::_DEBUG_RenderNormal()
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	D3DXVECTOR3 pos[3], nml[3];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();

	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		pos[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		pos[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		pos[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		nml[0] = m_pNmlBuffer[pTriangle->dwPNCIndex[0]];
		nml[1] = m_pNmlBuffer[pTriangle->dwPNCIndex[1]];
		nml[2] = m_pNmlBuffer[pTriangle->dwPNCIndex[2]];

		pLayer3D->_DEBUG_DrawLine(&pos[0],&D3DXVECTOR3(pos[0] + nml[0] * 0.1f),0xffffffff);
		pLayer3D->_DEBUG_DrawLine(&pos[1],&D3DXVECTOR3(pos[1] + nml[1] * 0.1f ),0xffffffff);
		pLayer3D->_DEBUG_DrawLine(&pos[2],&D3DXVECTOR3(pos[2] + nml[2] * 0.1f),0xffffffff);
	}
}

void model::Buffer::Draw(DWORD dwShaderGroupHandle,DWORD dwMode)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	D3DXVECTOR3 temp[3];
	D3DXVECTOR2 uv0[3];//,uv1[3];
	DWORD color[3];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
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

	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		temp[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		color[0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
		color[1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
		color[2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

		uv0[0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		uv0[1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		uv0[2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];

		/*	uv1[0] = m_pUV1Buffer[pTriangle->dwUV1Index[0]];
		uv1[1] = m_pUV1Buffer[pTriangle->dwUV1Index[1]];
		uv1[2] = m_pUV1Buffer[pTriangle->dwUV1Index[2]];		*/

		//		pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,uv1,color);
		pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,color);
	}
}
void model::Buffer::DrawShadow(DWORD dwShaderGroupHandle)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	D3DXVECTOR3 temp[3];
	D3DXVECTOR2 uv0[3];//,uv1[3];
	DWORD color[3];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);

	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		temp[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		color[0] = 0xff800000;
		color[1] = 0xff800000;
		color[2] = 0xff800000;

		uv0[0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		uv0[1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		uv0[2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];

		/*	uv1[0] = m_pUV1Buffer[pTriangle->dwUV1Index[0]];
		uv1[1] = m_pUV1Buffer[pTriangle->dwUV1Index[1]];
		uv1[2] = m_pUV1Buffer[pTriangle->dwUV1Index[2]];		*/

		pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,color);
		//		pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,1,temp,color);
	}
}
void model::Buffer::DrawTransparent(DWORD dwShaderGroupHandle)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	static D3DXVECTOR3 temp[3];
	static D3DXVECTOR2 uv0[3];//,uv1[3];
	DWORD color[3];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//render opaque mode : mask and none
	pDevice->SetRenderState( D3DRS_ZENABLE, TRUE);	
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);  // 修改将原来的z缓冲写入打开

	pInterface->Cull(!pShader->bTwoSide);
	//0 test

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

	pShaderGroup->_SetTextureOp(pShader);

	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		temp[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		color[0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
		color[1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
		color[2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

		uv0[0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		uv0[1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		uv0[2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];

		pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,color);
	}
}


//__CLASS__(model::Mesh)____
model::Mesh::Mesh():
m_pPosBuffer(NULL),
m_pNmlBuffer(NULL),
m_pColBufferDiffuse(NULL),
m_dwPNCCount(0),   
m_pUV0Buffer(NULL),
m_dwUV0Count(0),
m_pBoundingBox(NULL),
m_dwGroupCount(0),
m_pGroupArray(NULL),
m_pTriangleBuffer(NULL)
{

}

model::Mesh::~Mesh()
{
	ASSERT(!m_pGroupArray);
	ASSERT(!m_pBoundingBox);
	ASSERT(!m_pPosBuffer);
	ASSERT(!m_pNmlBuffer);
	ASSERT(!m_pColBufferDiffuse);
	ASSERT(!m_pUV0Buffer);
	//ASSERT(!m_pUV1Buffer);
	ASSERT(!m_pTriangleBuffer);
}


BOOL model::Mesh::Create(const char *pszFileName)
{
	utility::File file;
	if (!file.Open(pszFileName))
	{
		OutputError("can't open %s file\n",pszFileName); 
		MessageBox(NULL,pszFileName,"打开失败",MB_OK);
		return FALSE;
	}

	float fTemp3[3];

	DWORD dwFlag,dwVersion;
	file.Read(&dwFlag,sizeof(DWORD));
	file.Read(&dwVersion,sizeof(DWORD));

	if (dwFlag != MAKEFOURCC('M','E','S','H'))
	{
		OutputError("type doesn't match! \"%s\"\n",file.GetPathName());
		MessageBox(NULL,pszFileName,"打开失败",MB_OK);
		return FALSE;
	}

	if (dwVersion != MAKEFOURCC('2','.','0','0'))
	{
		OutputError("version doesn't match! \"%s\"\n",file.GetPathName());
		MessageBox(NULL,pszFileName,"版本错误",MB_OK);
		return FALSE;
	}

	//read shader index
	file.Read(&m_dwShaderIndex,sizeof(DWORD));

	//read position
	file.Read(&m_dwPNCCount,sizeof(DWORD));
	m_pPosBuffer = new D3DXVECTOR3[m_dwPNCCount];
	m_pNmlBuffer = new D3DXVECTOR3[m_dwPNCCount];
	m_pColBufferDiffuse = new D3DCOLOR[m_dwPNCCount];
	for(DWORD dwPos = 0; dwPos < m_dwPNCCount; dwPos++)
	{
		file.Read(fTemp3,sizeof(float) * 3 );
		m_pPosBuffer[dwPos].x = fTemp3[0];		
		m_pPosBuffer[dwPos].y = fTemp3[1];
		m_pPosBuffer[dwPos].z = fTemp3[2];
	}
	//read normal
	for(DWORD dwNml = 0; dwNml < m_dwPNCCount; dwNml++)
	{
		file.Read(fTemp3,sizeof(float) * 3 );
		m_pNmlBuffer[dwNml].x = fTemp3[0];		
		m_pNmlBuffer[dwNml].y = fTemp3[1];
		m_pNmlBuffer[dwNml].z = fTemp3[2];
	}
	//color 

	for(DWORD dwCol = 0; dwCol < m_dwPNCCount; dwCol++)
	{
		DWORD color;
		file.Read(&color,sizeof(DWORD));
		m_pColBufferDiffuse[dwCol]  = color;
	}

	//texture 0 coord
	file.Read(&m_dwUV0Count,sizeof(DWORD));
	m_pUV0Buffer = new D3DXVECTOR2[m_dwUV0Count];
	for(DWORD dwUV0 = 0; dwUV0 < m_dwUV0Count; dwUV0++)
	{
		file.Read(fTemp3,sizeof(float)*2);
		m_pUV0Buffer[dwUV0].x = fTemp3[0];
		m_pUV0Buffer[dwUV0].y = fTemp3[1];
	}

	//texture 1 coord
	DWORD dwUV1Count;
	file.Read(&dwUV1Count,sizeof(DWORD));
	//m_pUV1Buffer = new D3DXVECTOR2[m_dwUV1Count];
	for(DWORD dwUV1 = 0; dwUV1 < dwUV1Count; dwUV1++)
	{
		file.Read(fTemp3,sizeof(float)*2);
		//m_pUV1Buffer[dwUV1].x = fTemp3[0];
		//m_pUV1Buffer[dwUV1].y = fTemp3[1];
	}

	//triangle
	DWORD dwUV1Index[3];
	file.Read(&m_dwTriangleCount,sizeof(DWORD));
	m_pTriangleBuffer = new tagTriangle[m_dwTriangleCount];
	for(DWORD dwTriangle = 0; dwTriangle < m_dwTriangleCount; dwTriangle++)
	{
		tagTriangle * pTriangle = &m_pTriangleBuffer[dwTriangle];
		file.Read(pTriangle->dwPNCIndex,sizeof(DWORD) * 3);
		file.Read(pTriangle->dwUV0Index,sizeof(DWORD) * 3);
		file.Read(dwUV1Index,sizeof(DWORD) * 3);
		//	OutputConsole("%d %d %d \n",pTriangle->dwUV0Index[0],pTriangle->dwUV0Index[1],pTriangle->dwUV0Index[2]);
	}

	//读取影响组(为0)则没有

	file.Read(&m_dwGroupCount,sizeof(DWORD));
	if (m_dwGroupCount)
	{
		//读取顶点组别
		m_pGroupArray = new tagGroup[m_dwGroupCount];
		for(DWORD dwGroup = 0; dwGroup < m_dwGroupCount; dwGroup++)
		{
			tagGroup *pGroup = &m_pGroupArray[dwGroup];

			file.Read(&pGroup->dwJointIndex,sizeof(DWORD));		

			file.Read(&pGroup->dwPNCIndexStart,sizeof(DWORD));
			file.Read(&pGroup->dwPNCIndexCount,sizeof(DWORD));
		}
	}

	//计算boundingbox             
	m_pBoundingBox = new render::BoundingBox;

	D3DXVECTOR3 vDefault = m_pPosBuffer[0];
	D3DXVECTOR3 vMin = vDefault,vMax = vDefault;
	for(DWORD dwPos = 0; dwPos < m_dwPNCCount; dwPos++)
	{
		D3DXVECTOR3 &vtx = m_pPosBuffer[dwPos];
		if (vtx.x < vMin.x) vMin.x = vtx.x;
		if (vtx.y < vMin.y) vMin.y = vtx.y;
		if (vtx.z < vMin.z) vMin.z = vtx.z;

		if (vtx.x > vMax.x) vMax.x = vtx.x;
		if (vtx.y > vMax.y) vMax.y = vtx.y;
		if (vtx.z > vMax.z) vMax.z = vtx.z;
	}
	m_pBoundingBox->Set(vMin,vMax);

	file.Close();

	return TRUE;
}

void model::Mesh::Destroy()
{
	SAFEDELETEARRAY(m_pGroupArray); 
	SAFEDELETE(m_pBoundingBox);
	SAFEDELETEARRAY(m_pPosBuffer);
	SAFEDELETEARRAY(m_pNmlBuffer);
	SAFEDELETEARRAY(m_pColBufferDiffuse);
	SAFEDELETEARRAY(m_pUV0Buffer);
	SAFEDELETEARRAY(m_pTriangleBuffer);
}

void model::Mesh::ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,DWORD dwTimeElapseA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,float fRatio,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel)
{
	static D3DXVECTOR3 vTrsA,vSclA,vTrsB,vSclB,vTrs,vScl,vPos,vNml;
	static D3DXQUATERNION qRotA,qRotB,qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl,mTrsTemp,mInvTrsTpsTemp,matnowtemp;

	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
	D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();

	ZeroMemory(pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));

	const D3DXMATRIX *pMatBindPoseArray = pBindPose->GetBindPoseArray();

	const model::AnimJoint::tagJoint * pJointArrayA = pAnimJointA->GetJointArray();
	const model::AnimJoint::tagJoint * pJointArrayB = pAnimJointB->GetJointArray();

	for(DWORD dwJointGroup = 0; dwJointGroup < m_dwGroupCount;dwJointGroup ++)
	{
		tagGroup &tGroup = m_pGroupArray[dwJointGroup];

		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//先求出A的插值结果
		GetAnimFrameByTime(pAnimJointA,tGroup.dwJointIndex,&vTrsA,&vSclA,&qRotA,dwTimeElapseA);
		//再求出B的插值结果
		GetAnimFrameByTime(pAnimJointB,tGroup.dwJointIndex,&vTrsB,&vSclB,&qRotB,dwTimeElapseB);

		//A结果和B结果混合

		//trans
		D3DXVec3Lerp(&vTrs,&vTrsA,&vTrsB,fRatio);
		//scale
		D3DXVec3Lerp(&vScl,&vSclA,&vSclB,fRatio);
		//rotation
		D3DXQuaternionSlerp(&qRot,&qRotA,&qRotB,fRatio);
		D3DXQuaternionNormalize(&qRot,&qRot);

		D3DXMatrixTranslation(&matTrs,vTrs.x,vTrs.y,vTrs.z);
		D3DXMatrixScaling(&matScl,vScl.x,vScl.y,vScl.z);
		D3DXMatrixRotationQuaternion(&matRot,&qRot);






		D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
		D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

		//MAKE DEFORMATION MATRIX

		D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);




		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);

		matrixmodel = mTrsTemp;



		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);

		matrixworld = mTrsTemp;		
		
	

		D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);

		for(DWORD dwPosNmlColIndex = tGroup.dwPNCIndexStart ; dwPosNmlColIndex < tGroup.dwPNCIndexStart + tGroup.dwPNCIndexCount; dwPosNmlColIndex++)
		{
			//POSITION BLEND
			D3DXVec3TransformCoord(&pPosBuffer[dwPosNmlColIndex],&m_pPosBuffer[dwPosNmlColIndex],&mTrsTemp);
			//NORMAL TRANSFORM
			D3DXVec3TransformNormal(&pNmlBuffer[dwPosNmlColIndex],&m_pNmlBuffer[dwPosNmlColIndex],&mInvTrsTpsTemp);
		}
	}
}

void model::Mesh::ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,DWORD dwTimeElapseA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,float fRatio)
{
	static D3DXVECTOR3 vTrsA,vSclA,vTrsB,vSclB,vTrs,vScl,vPos,vNml;
	static D3DXQUATERNION qRotA,qRotB,qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl,mTrsTemp,mInvTrsTpsTemp;

	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
	D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();

	ZeroMemory(pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));

	const D3DXMATRIX *pMatBindPoseArray = pBindPose->GetBindPoseArray();

	const model::AnimJoint::tagJoint * pJointArrayA = pAnimJointA->GetJointArray();
	const model::AnimJoint::tagJoint * pJointArrayB = pAnimJointB->GetJointArray();

	for(DWORD dwJointGroup = 0; dwJointGroup < m_dwGroupCount;dwJointGroup ++)
	{
		tagGroup &tGroup = m_pGroupArray[dwJointGroup];

		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//先求出A的插值结果
		GetAnimFrameByTime(pAnimJointA,tGroup.dwJointIndex,&vTrsA,&vSclA,&qRotA,dwTimeElapseA);
		//再求出B的插值结果
		GetAnimFrameByTime(pAnimJointB,tGroup.dwJointIndex,&vTrsB,&vSclB,&qRotB,dwTimeElapseB);

		//A结果和B结果混合

		//trans
		D3DXVec3Lerp(&vTrs,&vTrsA,&vTrsB,fRatio);
		//scale
		D3DXVec3Lerp(&vScl,&vSclA,&vSclB,fRatio);
		//rotation
		D3DXQuaternionSlerp(&qRot,&qRotA,&qRotB,fRatio);
		D3DXQuaternionNormalize(&qRot,&qRot);

		D3DXMatrixTranslation(&matTrs,vTrs.x,vTrs.y,vTrs.z);
		D3DXMatrixScaling(&matScl,vScl.x,vScl.y,vScl.z);
		D3DXMatrixRotationQuaternion(&matRot,&qRot);

		D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
		D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

		//MAKE DEFORMATION MATRIX

		D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);
		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);
		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);			

		D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);

		for(DWORD dwPosNmlColIndex = tGroup.dwPNCIndexStart ; dwPosNmlColIndex < tGroup.dwPNCIndexStart + tGroup.dwPNCIndexCount; dwPosNmlColIndex++)
		{
			//POSITION BLEND
			D3DXVec3TransformCoord(&pPosBuffer[dwPosNmlColIndex],&m_pPosBuffer[dwPosNmlColIndex],&mTrsTemp);
			//NORMAL TRANSFORM
			D3DXVec3TransformNormal(&pNmlBuffer[dwPosNmlColIndex],&m_pNmlBuffer[dwPosNmlColIndex],&mInvTrsTpsTemp);
		}
	}
}


void model::Mesh::ProcessAnimJoint(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,model::BindPose *pBindPose,model::AnimJoint *pAnimJoint,DWORD dwAnimTimeElapse,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel)
{
	static D3DXVECTOR3 vTrs,vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl;

	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
	D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();

	ZeroMemory(pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));


	D3DXMATRIX mTrsTemp,mInvTrsTpsTemp;

	D3DXVECTOR3 vPos,vNml;

	const model::AnimJoint::tagJoint * pJointArray = pAnimJoint->GetJointArray();
	const D3DXMATRIX *pMatBindPoseArray = pBindPose->GetBindPoseArray();


	DWORD dwAnimTime = dwAnimTimeElapse;
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength();
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();
	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimTime * (dwAnimFrameCount -1) / dwAnimTimeLength;
	if (dwFrameA >= dwAnimFrameCount - 1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1;


		tagGroup &tGroup = m_pGroupArray[0];

		const model::AnimJoint::tagJoint &tJoint = pJointArray[tGroup.dwJointIndex];

		//BINDPOSE MATRIX	
		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//a --- b
		model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	

		D3DXMatrixTranslation(&matTrs,tFrameA.vTranslation.x,tFrameA.vTranslation.y,tFrameA.vTranslation.z);
		D3DXMatrixScaling(&matScl,tFrameA.vScale.x,tFrameA.vScale.y,tFrameA.vScale.z);
		D3DXMatrixRotationQuaternion(&matRot,&tFrameA.qRot);

		D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
		D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);



		//MAKE DEFORMATION MATRIX
		D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);






		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);

		matrixmodel = mTrsTemp;



		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);

		matrixworld = mTrsTemp;
		


		D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);

		for(DWORD dwPosNmlColIndex = tGroup.dwPNCIndexStart ; dwPosNmlColIndex < tGroup.dwPNCIndexStart + tGroup.dwPNCIndexCount; dwPosNmlColIndex++)
		{
			//POSITION BLEND
			D3DXVec3TransformCoord(&pPosBuffer[dwPosNmlColIndex],&m_pPosBuffer[dwPosNmlColIndex],&mTrsTemp);
			//NORMAL TRANSFORM
			D3DXVec3TransformNormal(&pNmlBuffer[dwPosNmlColIndex],&m_pNmlBuffer[dwPosNmlColIndex],&mInvTrsTpsTemp);
		}

	}
	else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get A time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
		//计算A B 之间 ratio
		DWORD dwTimeRelateA = dwAnimTime - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount -1) ;
		float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 
		//OutputConsole("%f\n",fRatio);


		tagGroup &tGroup = m_pGroupArray[0];

		const model::AnimJoint::tagJoint &tJoint = pJointArray[tGroup.dwJointIndex];
		//BINDPOSE MATRIX	
		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//interpolation	a --- b
		model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	
		model::AnimJoint::tagFrame &tFrameB = tJoint.pFrameArray[dwFrameB];   	
		//trans
		D3DXVec3Lerp(&vTrs,&tFrameA.vTranslation,&tFrameB.vTranslation,fRatio);
		//scale
		D3DXVec3Lerp(&vScl,&tFrameA.vScale,&tFrameB.vScale,fRatio);
		//rotation
		D3DXQuaternionSlerp(&qRot,&tFrameA.qRot,&tFrameB.qRot,fRatio);
		D3DXQuaternionNormalize(&qRot,&qRot);

		D3DXMatrixTranslation(&matTrs,vTrs.x,vTrs.y,vTrs.z);
		D3DXMatrixScaling(&matScl,vScl.x,vScl.y,vScl.z);
		D3DXMatrixRotationQuaternion(&matRot,&qRot);

		D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
		D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

		//MAKE DEFORMATION MATRIX

		D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);






		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);

		matrixmodel = mTrsTemp;



		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);

		matrixworld = mTrsTemp;


		


		D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);

		for(DWORD dwPosNmlColIndex = tGroup.dwPNCIndexStart ; dwPosNmlColIndex < tGroup.dwPNCIndexStart + tGroup.dwPNCIndexCount; dwPosNmlColIndex++)
		{
			//POSITION BLEND
			D3DXVec3TransformCoord(&pPosBuffer[dwPosNmlColIndex],&m_pPosBuffer[dwPosNmlColIndex],&mTrsTemp);
			//NORMAL TRANSFORM
			D3DXVec3TransformNormal(&pNmlBuffer[dwPosNmlColIndex],&m_pNmlBuffer[dwPosNmlColIndex],&mInvTrsTpsTemp);
		}

	}
}


void model::Mesh::ProcessAnimJoint(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,model::BindPose *pBindPose,model::AnimJoint *pAnimJoint,DWORD dwAnimTimeElapse)
{
	static D3DXVECTOR3 vTrs,vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl;

	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
	D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();

	ZeroMemory(pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));

	D3DXMATRIX mTrsTemp,mInvTrsTpsTemp;

	D3DXVECTOR3 vPos,vNml;

	const model::AnimJoint::tagJoint * pJointArray = pAnimJoint->GetJointArray();
	const D3DXMATRIX *pMatBindPoseArray = pBindPose->GetBindPoseArray();


	DWORD dwAnimTime = dwAnimTimeElapse;
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength();
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();
	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimTime * (dwAnimFrameCount -1) / dwAnimTimeLength;
	if (dwFrameA >= dwAnimFrameCount - 1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1;

		for(DWORD dwGroup = 0; dwGroup < m_dwGroupCount; dwGroup++)
		{
			tagGroup &tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint &tJoint = pJointArray[tGroup.dwJointIndex];
			//BINDPOSE MATRIX	
			const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//a --- b
			model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	

			D3DXMatrixTranslation(&matTrs,tFrameA.vTranslation.x,tFrameA.vTranslation.y,tFrameA.vTranslation.z);
			D3DXMatrixScaling(&matScl,tFrameA.vScale.x,tFrameA.vScale.y,tFrameA.vScale.z);
			D3DXMatrixRotationQuaternion(&matRot,&tFrameA.qRot);

			D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
			D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);
			D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);			

			D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);

			for(DWORD dwPosNmlColIndex = tGroup.dwPNCIndexStart ; dwPosNmlColIndex < tGroup.dwPNCIndexStart + tGroup.dwPNCIndexCount; dwPosNmlColIndex++)
			{
				//POSITION BLEND
				D3DXVec3TransformCoord(&pPosBuffer[dwPosNmlColIndex],&m_pPosBuffer[dwPosNmlColIndex],&mTrsTemp);
				//NORMAL TRANSFORM
				D3DXVec3TransformNormal(&pNmlBuffer[dwPosNmlColIndex],&m_pNmlBuffer[dwPosNmlColIndex],&mInvTrsTpsTemp);
			}
		}
	}
	else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get A time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
		//计算A B 之间 ratio
		DWORD dwTimeRelateA = dwAnimTime - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount -1) ;
		float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 
		//OutputConsole("%f\n",fRatio);

		for(DWORD dwGroup = 0; dwGroup < m_dwGroupCount; dwGroup++)
		{
			tagGroup &tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint &tJoint = pJointArray[tGroup.dwJointIndex];
			//BINDPOSE MATRIX	
			const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//interpolation	a --- b
			model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	
			model::AnimJoint::tagFrame &tFrameB = tJoint.pFrameArray[dwFrameB];   	
			//trans
			D3DXVec3Lerp(&vTrs,&tFrameA.vTranslation,&tFrameB.vTranslation,fRatio);
			//scale
			D3DXVec3Lerp(&vScl,&tFrameA.vScale,&tFrameB.vScale,fRatio);
			//rotation
			D3DXQuaternionSlerp(&qRot,&tFrameA.qRot,&tFrameB.qRot,fRatio);
			D3DXQuaternionNormalize(&qRot,&qRot);

			D3DXMatrixTranslation(&matTrs,vTrs.x,vTrs.y,vTrs.z);
			D3DXMatrixScaling(&matScl,vScl.x,vScl.y,vScl.z);
			D3DXMatrixRotationQuaternion(&matRot,&qRot);

			D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
			D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

			//MAKE DEFORMATION MATRIX

			D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);
			D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);			

			D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);

			for(DWORD dwPosNmlColIndex = tGroup.dwPNCIndexStart ; dwPosNmlColIndex < tGroup.dwPNCIndexStart + tGroup.dwPNCIndexCount; dwPosNmlColIndex++)
			{
				//POSITION BLEND
				D3DXVec3TransformCoord(&pPosBuffer[dwPosNmlColIndex],&m_pPosBuffer[dwPosNmlColIndex],&mTrsTemp);
				//NORMAL TRANSFORM
				D3DXVec3TransformNormal(&pNmlBuffer[dwPosNmlColIndex],&m_pNmlBuffer[dwPosNmlColIndex],&mInvTrsTpsTemp);
			}
		}
	}
}




void model::Mesh::CopyPosition(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix)
{
	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
	D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();

	D3DXMATRIX  mInvTrsTpsTemp;
	D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,pWorldMatrix);
	D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);

	for(DWORD dwIndex = 0; dwIndex < m_dwPNCCount; dwIndex++)
	{
		D3DXVec3TransformCoord(&pPosBuffer[dwIndex],&m_pPosBuffer[dwIndex], pWorldMatrix);
		D3DXVec3TransformNormal(&pNmlBuffer[dwIndex], &m_pNmlBuffer[dwIndex], &mInvTrsTpsTemp);
	}
}


void model::Mesh::CopyUV(model::Buffer *pBuffer)
{
	memcpy(pBuffer->GetUV0Buffer(),m_pUV0Buffer,sizeof(D3DXVECTOR2) * m_dwUV0Count);
	//	memcpy(pBuffer->GetUV1Buffer(),m_pUV1Buffer,sizeof(D3DXVECTOR2) * m_dwUV1Count);
}

void model::Mesh::ProcessAnimVisibility(model::Buffer *pBuffer,const AnimVisibility *pAnimVisibility,DWORD dwTimeElapse,DWORD dwTimeCircle)
{
	//alpha动画
	//插值
	DWORD dwAnimFrameCount = pAnimVisibility->GetFrameCount();
	float *pFrameArray = pAnimVisibility->GetFrameArray();

	//插值
	//A -+- B

	// soft modified 2006-11-21
	DWORD dwFrameA = (dwAnimFrameCount - 1) * dwTimeElapse;
	if (dwTimeCircle != 0)
		dwFrameA /= dwTimeCircle;

	if (dwFrameA >= dwAnimFrameCount -1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1; 			

		//a
		float fVisibilityA = pFrameArray[dwFrameA];   		

		DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();

		for(DWORD n = 0; n < pBuffer->GetPNCCount(); n++)
		{
			DWORD &dwDestColor = pColBuffer[n];

			float fR = (float)( (dwDestColor & 0x00FF0000) >> 16);
			float fG = (float)( (dwDestColor & 0x0000FF00) >>  8);
			float fB = (float)(  dwDestColor & 0x000000FF);

			DWORD c = (DWORD)((float)fR * fVisibilityA);  	
			dwDestColor = (dwDestColor & 0xFF000000) | (c << 16);
			c = (DWORD)( (float)fG * fVisibilityA);  	
			dwDestColor |= (c << 8);
			c = (DWORD)( (float)fB * fVisibilityA);  	
			dwDestColor |= c;
		}
	}
	else
	{
		DWORD dwFrameB = (dwFrameA + 1) % dwAnimFrameCount;

		float fFrameInterval(0);
		DWORD dwTimeA(0);
		//get A time
		if (dwAnimFrameCount > 1)
		{
			dwTimeA =  dwFrameA * dwTimeCircle /  (dwAnimFrameCount -1);
			fFrameInterval = (float)dwTimeCircle / (float)(dwAnimFrameCount-1) ;
		}
		else
		{
			fFrameInterval = (float)dwTimeCircle;
		}

		//计算A B 之间 ratio
		DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
		float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 
		//interpolation	a --- b
		float fVisibilityA = pFrameArray[dwFrameA];   	
		float fVisibilityB = pFrameArray[dwFrameB];   	
		float fVisibility  = fVisibilityA + (fVisibilityB - fVisibilityA) * fRatio;

		DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();
		for(DWORD n = 0; n < pBuffer->GetPNCCount(); n++)
		{
			DWORD &dwDestColor = pColBuffer[n];

			float fR = (float)( (dwDestColor & 0x00FF0000) >> 16);
			float fG = (float)( (dwDestColor & 0x0000FF00) >>  8);
			float fB = (float)(  dwDestColor & 0x000000FF);

			DWORD c = (DWORD)((float)fR * fVisibility);  	
			dwDestColor = (dwDestColor & 0xFF000000) | (c << 16);
			c = (DWORD)( (float)fG * fVisibility);  	
			dwDestColor |= (c << 8);
			c = (DWORD)( (float)fB * fVisibility);  	
			dwDestColor |= c;
		}
	}
}

void model::Mesh::ProcessAnimColor(model::Buffer *pBuffer,const AnimColor *pAnimColor,DWORD dwElapseTime,DWORD dwTimeCircle)
{
	//color插枝
	if (pAnimColor)
	{
		//插值
		DWORD dwFrameCount = pAnimColor->GetFrameCount();
		DWORD *pColorArray = pAnimColor->GetColorArray();
		//插值
		//A -+- B

		// soft modified 2006-11-21
		DWORD dwFrameA = (dwFrameCount - 1) * dwElapseTime;
		if (dwTimeCircle != 0)
			dwFrameA /= dwTimeCircle;

		if (dwFrameA >= dwFrameCount - 1)
		{
			//不需要插值
			dwFrameA = dwFrameCount - 1; 			
			DWORD dwSrcColor = pColorArray[dwFrameA];   
			float fSR = (float)( (dwSrcColor & 0x00FF0000) >> 16) / 255.0f;
			float fSG = (float)( (dwSrcColor & 0x0000FF00) >>  8) / 255.0f;
			float fSB = (float)(  dwSrcColor & 0x000000FF)        / 255.0f;				

			DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();
			//颜色相乘
			for(DWORD n = 0; n < pBuffer->GetPNCCount(); n++)
			{
				DWORD &dwDestColor = pColBuffer[n];

				float fRD = (float) ( (dwDestColor & 0x00FF0000) >> 16) / 255.0f;
				float fGD = (float) ( (dwDestColor & 0x0000FF00) >>  8) / 255.0f;
				float fBD = (float) ( (dwDestColor & 0x000000FF)) / 255.0f;

				DWORD r   = (DWORD)(( fSR * fRD) * 255.0f);  	
				if (r > 255) r = 0x000000FF;
				DWORD g = (DWORD)(( fSG * fGD) * 255.0f);  	
				if (g > 255) g = 0x000000FF;
				DWORD b = (DWORD)(( fSB * fBD) * 255.0f);  	
				if (b > 255) b = 0x000000FF;
				dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;

			}
		}
		else
		{
			//else
			DWORD dwFrameB = (dwFrameA + 1) % dwFrameCount; 
			//get A time
			DWORD dwTimeA(0);
			//计算A B 之间 ratio
			float fFrameInterval(0);
			if (dwFrameCount > 1)
			{
				dwTimeA = dwFrameA * dwTimeCircle /  (dwFrameCount - 1);
				fFrameInterval = (float)dwTimeCircle / (float)(dwFrameCount - 1);
			}
			else
			{
				fFrameInterval = (float)dwTimeCircle;
			}
			DWORD dwTimeRelateA = dwElapseTime - dwTimeA;

			float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 
			if (fRatio > 1.0f) fRatio = 1.0f;

			//interpolation	color a --- b
			DWORD dwFrameAColor = pColorArray[dwFrameA];   	
			DWORD dwFrameBColor = pColorArray[dwFrameB];  

			float fAA = (float)((dwFrameAColor & 0xFF000000) >> 24);
			float fRA = (float)((dwFrameAColor & 0x00FF0000) >> 16);
			float fGA = (float)((dwFrameAColor & 0x0000FF00) >>  8);
			float fBA = (float)( dwFrameAColor & 0x000000FF);

			float fAB = (float)((dwFrameBColor & 0xFF000000) >> 24);
			float fRB = (float)((dwFrameBColor & 0x00FF0000) >> 16);
			float fGB = (float)((dwFrameBColor & 0x0000FF00) >>  8);
			float fBB = (float)( dwFrameBColor & 0x000000FF);

			DWORD dwA = (DWORD)(fAA + (fAB - fAA) * fRatio);
			if (dwA > 255) dwA = 255;
			DWORD dwR = (DWORD)(fRA + (fRB - fRA) * fRatio);
			if (dwR > 255) dwR = 255;
			DWORD dwG = (DWORD)(fGA + (fGB - fGA) * fRatio);
			if (dwG > 255) dwG = 255;
			DWORD dwB = (DWORD)(fBA + (fBB - fBA) * fRatio);
			if (dwB > 255) dwB = 255;
			DWORD dwSrcColor = D3DCOLOR_ARGB(dwA,dwR,dwG,dwB);
			float fSR = (float)( (dwSrcColor & 0x00FF0000) >> 16) / 255.0f;
			float fSG = (float)( (dwSrcColor & 0x0000FF00) >>  8) / 255.0f;
			float fSB = (float)(  dwSrcColor & 0x000000FF)        / 255.0f;				

			DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();
			for(DWORD n = 0; n < pBuffer->GetPNCCount(); n++)
			{
				DWORD &dwDestColor = pColBuffer[n];

				float fRD = (float) ( (dwDestColor & 0x00FF0000) >> 16) / 255.0f;
				float fGD = (float) ( (dwDestColor & 0x0000FF00) >>  8) / 255.0f;
				float fBD = (float) ( (dwDestColor & 0x000000FF)) / 255.0f;

				DWORD r   = (DWORD)(( fSR * fRD) * 255.0f);  	
				if (r > 255) r = 0x000000FF;
				DWORD g = (DWORD)(( fSG * fGD) * 255.0f);  	
				if (g > 255) g = 0x000000FF;
				DWORD b = (DWORD)(( fSB * fBD) * 255.0f);  	
				if (b > 255) b = 0x000000FF;
				dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;
			}
		}
	}
}

void model::Mesh::ProcessAnimUV(model::Buffer *pBuffer,const AnimUV *pAnimUV,DWORD dwElapseTime,DWORD dwTimeCircle)
{
	//计算出当前的偏移
	float fRatio = (float)dwElapseTime / (float)dwTimeCircle;
	float fU = fRatio * pAnimUV->GetU();
	float fV = fRatio * pAnimUV->GetV();
	D3DXVECTOR2 *pUV0Buffer= pBuffer->GetUV0Buffer();
	for(DWORD n = 0 ; n < m_dwUV0Count; n++)
	{
		pUV0Buffer[n].x = m_pUV0Buffer[n].x + fU;		
		pUV0Buffer[n].y = m_pUV0Buffer[n].y + fV;		
	}

	//	memcpy(pBuffer->GetUV1Buffer(),m_pUV1Buffer,sizeof(D3DXVECTOR2) * m_dwUV1Count);
}

void model::Mesh::ProcessEnvironmentMaps(model::Buffer *pBuffer,const D3DXMATRIX *pViewMatrix)
{
	static D3DXVECTOR3 vNormal[3];
	D3DXVECTOR3 *pNormalBuffer = pBuffer->GetNmlBuffer();
	for(DWORD n = 0 ; n < m_dwTriangleCount; n ++)
	{
		tagTriangle *pTriangle = &m_pTriangleBuffer[n];

		//处理Normal
		D3DXVec3TransformNormal(&vNormal[0],&pNormalBuffer[pTriangle->dwPNCIndex[0]],pViewMatrix);
		D3DXVec3TransformNormal(&vNormal[1],&pNormalBuffer[pTriangle->dwPNCIndex[1]],pViewMatrix);
		D3DXVec3TransformNormal(&vNormal[2],&pNormalBuffer[pTriangle->dwPNCIndex[2]],pViewMatrix);

		//处理UV
		m_pUV0Buffer[pTriangle->dwUV0Index[0]].x = vNormal[0].x * 0.5f + 0.5f;
		m_pUV0Buffer[pTriangle->dwUV0Index[0]].y = vNormal[0].y * 0.5f + 0.5f;
		m_pUV0Buffer[pTriangle->dwUV0Index[1]].x = vNormal[1].x * 0.5f + 0.5f;
		m_pUV0Buffer[pTriangle->dwUV0Index[1]].y = vNormal[1].y * 0.5f + 0.5f;
		m_pUV0Buffer[pTriangle->dwUV0Index[2]].x = vNormal[2].x * 0.5f + 0.5f;
		m_pUV0Buffer[pTriangle->dwUV0Index[2]].y = vNormal[2].y * 0.5f + 0.5f;

	}

	//memcpy(pBuffer->GetUV1Buffer(),m_pUV1Buffer,sizeof(D3DXVECTOR2) * m_dwUV1Count);
}

void model::Mesh::ProcessAnimTexture(model::Buffer *pBuffer,const AnimTexture *pAnimTexture,DWORD dwTimeElapse,DWORD dwTimeCircle)
{
	//计算出当前的偏移
	int total = pAnimTexture->GetRows() * pAnimTexture->GetColumns();

	// soft modified 2006-11-21
	int iAnimTextureRectIndex  =  total * dwTimeElapse;
	if (dwTimeCircle != 0)
		iAnimTextureRectIndex /= dwTimeCircle;

	if (iAnimTextureRectIndex >= total)
		iAnimTextureRectIndex = 0;

	float fScaleU = 1.0f / (float)pAnimTexture->GetRows();
	float fScaleV = 1.0f / (float)pAnimTexture->GetColumns();

	float startu = fScaleU * (float)(iAnimTextureRectIndex % pAnimTexture->GetRows());
	float startv = fScaleV * (float)(iAnimTextureRectIndex / pAnimTexture->GetRows());

	D3DXVECTOR2 *pUV0Buffer= pBuffer->GetUV0Buffer();
	for(DWORD n = 0 ; n < m_dwUV0Count; n++)
	{
		pUV0Buffer[n].x = m_pUV0Buffer[n].x * fScaleU  + startu;		
		pUV0Buffer[n].y = m_pUV0Buffer[n].y * fScaleV  + startv;		
	}

	//	memcpy(pBuffer->GetUV1Buffer(),m_pUV1Buffer,sizeof(D3DXVECTOR2) * m_dwUV1Count);
}

void model::Mesh::CopyColor(model::Buffer *pBuffer)
{
	memcpy(pBuffer->GetColBufferDiffuse(),m_pColBufferDiffuse,m_dwPNCCount * sizeof(DWORD));
	ZeroMemory(pBuffer->GetColBufferSpecular(),m_dwPNCCount * sizeof(DWORD));
}


void model::Mesh::CalculateVertexNormal()
{	
	ZeroMemory(m_pNmlBuffer,sizeof(D3DXVECTOR3) * m_dwPNCCount);
	D3DXVECTOR3 *v0,*v1,*v2;
	D3DXVECTOR3 vector1,vector2;
	DWORD       *pCountBuffer = new DWORD[m_dwPNCCount];
	ZeroMemory(pCountBuffer,sizeof(DWORD) * m_dwPNCCount);

	DWORD n = 0;
	for(; n < m_dwTriangleCount; n++)
	{
		model::Mesh::tagTriangle &tri = m_pTriangleBuffer[n];

		v0 = &m_pPosBuffer[tri.dwPNCIndex[0]];
		v1 = &m_pPosBuffer[tri.dwPNCIndex[1]];
		v2 = &m_pPosBuffer[tri.dwPNCIndex[2]];

		vector1 = *v0 - *v1;
		vector2 = *v2 - *v1;

		D3DXVec3Normalize(&vector1, &vector1);
		D3DXVec3Normalize(&vector2, &vector2);

		D3DXVECTOR3 tempNormal;
		D3DXVec3Cross(&tempNormal, &vector2, &vector1);
		D3DXVec3Normalize(&tempNormal, &tempNormal);

		m_pNmlBuffer[tri.dwPNCIndex[0]] += tempNormal;	
		m_pNmlBuffer[tri.dwPNCIndex[1]] += tempNormal;	
		m_pNmlBuffer[tri.dwPNCIndex[2]] += tempNormal;	

		pCountBuffer[tri.dwPNCIndex[0]]++;
		pCountBuffer[tri.dwPNCIndex[1]]++;
		pCountBuffer[tri.dwPNCIndex[2]]++;
	}

	for(n =0; n < m_dwPNCCount; n++)
	{
		m_pNmlBuffer[n] /=(float)pCountBuffer[n];
		D3DXVec3Normalize(&m_pNmlBuffer[n], &m_pNmlBuffer[n]);
	}

	SAFEDELETEARRAY(pCountBuffer);
}

void model::Mesh::_DEBUG_Render()
{
	//test render
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	D3DXVECTOR3 temp[3];
	DWORD color = 0xffffffff;

	for(DWORD  i = 0; i < m_dwTriangleCount; i++)
	{
		model::Mesh::tagTriangle *pTriangle = &m_pTriangleBuffer[i];

		temp[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		pLayer3D->_DEBUG_DrawTriangle(&temp[0],&temp[1],&temp[2],color);
	}
}

//__CLASS__(model::AnimJoint)___
model::AnimJoint::AnimJoint():
m_pJointArray(NULL)
{
	m_dwFrameCount  = 0;
	m_dwTimeLength  = 1000;
}

model::AnimJoint::~AnimJoint()
{
	ASSERT(!m_pJointArray);
}

BOOL model::AnimJoint::Create(const char *pszFileName)
{
	utility::File file;
	if (!file.Open(pszFileName))
	{
		OutputError("error: Create ANIM file \"%s\"\n",pszFileName);
		MessageBox(NULL,pszFileName,"文件打开失败",MB_OK); 
		ASSERT(0);
		return TRUE; 
	}

	DWORD dwFlag,dwVersion;
	file.Read(&dwFlag,sizeof(DWORD));
	file.Read(&dwVersion,sizeof(DWORD));

	if (dwFlag != MAKEFOURCC('A','N','I','M'))
	{
		OutputError("error: type doesn't match! \"%s\"\n",file.GetPathName());
		ASSERT(0);
		return FALSE;
	}

	if (dwVersion != MAKEFOURCC('3','.','0','0'))
	{
		OutputError("error: version doesn't match! \"%s\"\n",file.GetPathName());
		ASSERT(0);
		return FALSE;
	}

	//读入帧数
	file.Read(&m_dwFrameCount,sizeof(DWORD));
	if (m_dwFrameCount <= 1)
	{
		OutputError("error: keyframe must > 1 \"%s\"\n",file.GetPathName());
		ASSERT(0);
		return FALSE;
	}
	//读入比例
	file.Read(&m_dwTimeLength,sizeof(DWORD));
	//读取骨骼数
	file.Read(&m_dwJointCount,sizeof(DWORD));

	m_pJointArray = new tagJoint[m_dwJointCount];

	for(DWORD n = 0;n < m_dwJointCount; n++)
	{
		tagJoint *pJoint = &m_pJointArray[n];
		//读入bindpose
		DWORD dwNameLen;
		file.Read(&dwNameLen,sizeof(DWORD));
		ZeroMemory(pJoint->szName,128);
		ASSERT(dwNameLen < 128);
		file.Read(pJoint->szName,dwNameLen);
		//读入帧

		pJoint->pFrameArray = new tagFrame[m_dwFrameCount];

		for(DWORD nFrame = 0; nFrame < m_dwFrameCount; nFrame++)
		{
			tagFrame &tFrame = pJoint->pFrameArray[nFrame];
			file.Read(&tFrame.vScale,sizeof(D3DXVECTOR3));
			file.Read(&tFrame.qRot,sizeof(D3DXQUATERNION));
			file.Read(&tFrame.vTranslation,sizeof(D3DXVECTOR3));
		}
	}
	file.Close();

	return TRUE;
}
void model::AnimJoint::Destroy()
{
	//to do
	for(DWORD dwJoint = 0; dwJoint < m_dwJointCount; dwJoint++)
	{
		tagJoint *pJoint = &m_pJointArray[dwJoint];
		SAFEDELETEARRAY(pJoint->pFrameArray);
	}
	SAFEDELETEARRAY(m_pJointArray);
}


void model::AnimJoint::_DEBUG_Render(const D3DXMATRIX *pWorldMatrix,DWORD dwTimeElapse)
{
	//draw joint box 
	static D3DXVECTOR3 vTrs,vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl,matInvBindPose;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	static D3DXVECTOR3 v,v0(0,0,0);

	DWORD dwAnimTime = dwTimeElapse;
	DWORD dwAnimTimeLength = this->GetTimeLength();
	DWORD dwAnimFrameCount = this->GetFrameCount();
	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimFrameCount * dwAnimTime / dwAnimTimeLength;
	if (dwFrameA >= dwAnimFrameCount -1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1;

		for(DWORD dwJoint = 0; dwJoint < m_dwJointCount; dwJoint ++)
		{
			const model::AnimJoint::tagJoint &tJoint = m_pJointArray[dwJoint];

			//a --- b
			model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	

			D3DXMatrixTranslation(&matTrs,tFrameA.vTranslation.x,tFrameA.vTranslation.y,tFrameA.vTranslation.z);
			D3DXMatrixScaling(&matScl,tFrameA.vScale.x,tFrameA.vScale.y,tFrameA.vScale.z);
			D3DXMatrixRotationQuaternion(&matRot,&tFrameA.qRot);

			D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
			D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

			//MAKE world TRS MATRIX
			D3DXMatrixMultiply(&matTemp,&matTemp,pWorldMatrix);			

			D3DXVec3TransformCoord(&v,&v0,&matTemp);
			pLayer3D->_DEBUG_DrawWireBox(&v,0.01f,0xff4488aa);		
		}
	}
	else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get A time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  dwAnimFrameCount;
		//计算A B 之间 ratio
		DWORD dwTimeRelateA = dwAnimTime - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)dwAnimFrameCount ;
		float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 

		for(DWORD dwJoint = 0; dwJoint < m_dwJointCount; dwJoint ++)
		{
			const model::AnimJoint::tagJoint &tJoint = m_pJointArray[dwJoint];

			//interpolation	a --- b
			model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	
			model::AnimJoint::tagFrame &tFrameB = tJoint.pFrameArray[dwFrameB];   	
			//lerp trans
			D3DXVec3Lerp(&vTrs,&tFrameA.vTranslation,&tFrameB.vTranslation,fRatio);
			//lerp scale
			D3DXVec3Lerp(&vScl,&tFrameA.vScale,&tFrameB.vScale,fRatio);
			//lerp rotation
			D3DXQuaternionSlerp(&qRot,&tFrameA.qRot,&tFrameB.qRot,fRatio);
			D3DXQuaternionNormalize(&qRot,&qRot);

			D3DXMatrixTranslation(&matTrs,vTrs.x,vTrs.y,vTrs.z);
			D3DXMatrixScaling(&matScl,vScl.x,vScl.y,vScl.z);
			D3DXMatrixRotationQuaternion(&matRot,&qRot);

			D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
			D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixMultiply(&matTemp,&matTemp,pWorldMatrix);			

			D3DXVec3TransformCoord(&v,&v0,&matTemp);
			pLayer3D->_DEBUG_DrawWireBox(&v,0.01f,0xff4488aa);	
		}
	}
}



//__CLASS__(model::AnimVisibility)___
model::AnimVisibility::AnimVisibility():
m_pFrameArray(NULL)
{
}
model::AnimVisibility::~AnimVisibility()
{
}

BOOL model::AnimVisibility::Create(const char *pszFileName)
{
	utility::File file;
	if (!file.Open(pszFileName))
	{
		OutputError("Create AnimVisibility file \"%s\"\n",pszFileName);
		MessageBox(NULL,pszFileName,"文件打开错误",MB_OK);
		ASSERT(0);
		return TRUE;
	}

	utility::Text script;

	script.Create(&file);
	file.Close();

	//读入帧数
	utility::Text::Node *pFrameNode = script.GetRootNode()->GetChildNode("frame");
	m_dwFrameCount = pFrameNode->GetVarCount();
	m_pFrameArray = new float[m_dwFrameCount];
	for(DWORD n = 0;n < m_dwFrameCount; n++)
	{
		m_pFrameArray[n] = pFrameNode->GetVar(n)->GetFloatValue();
	}

	script.Destroy();

	return TRUE;
}

void model::AnimVisibility::Destroy()
{
	SAFEDELETEARRAY(m_pFrameArray);
}

//__CLASS__(model::AnimColor)___
model::AnimColor::AnimColor():
m_pColorArray(NULL)
{
}
model::AnimColor::~AnimColor()
{
}

BOOL model::AnimColor::Create(const char *pszFileName)
{
	utility::File file;
	if (!file.Open(pszFileName))
	{
		OutputError("error: Create AnimColor file \"%s\"\n",pszFileName);
		MessageBox(NULL,pszFileName,"文件不存在!",MB_OK);
		return FALSE;
	}

	utility::Text script;

	script.Create(&file);
	file.Close();

	//读入帧数
	utility::Text::Node *pFrameNode = script.GetRootNode()->GetChildNode("frame");
	m_dwFrameCount = pFrameNode->GetVarCount();
	//颜色列表
	if (m_dwFrameCount)
	{
		m_pColorArray = new DWORD [m_dwFrameCount];
		for(DWORD n = 0; n < pFrameNode->GetVarCount();n++)
		{
			m_pColorArray[n] = pFrameNode->GetVar(n)->GetHexValue();
		}
	}

	script.Destroy();

	return TRUE;
}

void model::AnimColor::Destroy()
{
	SAFEDELETEARRAY(m_pColorArray);
}


//__CLASS__(model::AnimUV)___
model::AnimUV::AnimUV()
{
}
model::AnimUV::~AnimUV()
{
}

BOOL model::AnimUV::Create(const char *pszFileName)
{
	utility::File file;
	if (!file.Open(pszFileName))
	{
		OutputError("Create AnimUV file \"%s\"\n",pszFileName);
		MessageBox(NULL,pszFileName,"打开失败",MB_OK);
		return TRUE;
	}

	utility::Text script;

	script.Create(&file);
	file.Close();

	m_fUOffset = script.GetRootNode()->GetVar("u")->GetFloatValue();
	m_fVOffset = script.GetRootNode()->GetVar("v")->GetFloatValue();

	script.Destroy();
	return TRUE;
}

void model::AnimUV::Destroy()
{
}

//__CLASS__(model::AnimTexture)___
model::AnimTexture::AnimTexture()
{
}
model::AnimTexture::~AnimTexture()
{
}

BOOL model::AnimTexture::Create(const char *pszFileName)
{
	utility::File file;
	if (!file.Open(pszFileName))
	{
		OutputError("error: Create AnimTexture file \"%s\"\n",pszFileName);
		ASSERT(0);
		return TRUE;
	}

	utility::Text script;

	script.Create(&file);
	file.Close();

	m_dwRows = (DWORD)script.GetRootNode()->GetVar("rows")->GetIntValue();
	m_dwColumns = (DWORD)script.GetRootNode()->GetVar("columns")->GetIntValue();

	script.Destroy();
	return TRUE;
}

void model::AnimTexture::Destroy()
{
}

//////////////////////////////////////////////////////////////////////////////////////
model::BindPose::BindPose()
:m_pBindPoseArray(NULL)
{
}

model::BindPose::~BindPose()
{
}

BOOL model::BindPose::Create(const char *pszFileName)
{
	utility::File file;
	if (file.Open(pszFileName))
	{

		DWORD dwFlag,dwVersion;
		file.Read(&dwFlag,sizeof(DWORD));
		file.Read(&dwVersion,sizeof(DWORD));

		if (dwFlag != MAKEFOURCC('B','I','N','D'))
		{
			OutputError("type doesn't match! \"%s\"\n",file.GetPathName());
			MessageBox(NULL,file.GetPathName(),"类型错误",MB_OK);
			return FALSE;
		}

		if (dwVersion != MAKEFOURCC('1','.','0','0'))
		{
			OutputError("version doesn't match! \"%s\"\n",file.GetPathName());
			MessageBox(NULL,file.GetPathName(),"版本错误",MB_OK);
			return FALSE;
		}

		file.Read(&m_dwJointCount,sizeof(int));
		m_pBindPoseArray = new D3DXMATRIX[m_dwJointCount];

		DWORD dwNameLen;
		char str[256];
		for(DWORD n = 0;n < m_dwJointCount; n++)
		{
			//读入
			file.Read(&m_pBindPoseArray[n],sizeof(D3DXMATRIX));
			file.Read(&dwNameLen,sizeof(DWORD));
			file.Read(str,dwNameLen);
			//file.SkipBytes(dwNameLen);

		}

		//OutputConsole("BindPose Joints: %d\n",m_dwJointCount);
		file.Close();
		return TRUE;
	}
	return FALSE;
}

void model::BindPose::Destroy()
{
	SAFEDELETEARRAY(m_pBindPoseArray);
}

void model::Mesh::GetAnimFrameByTime(const model::AnimJoint *pAnimJoint,DWORD dwJointIndex,D3DXVECTOR3 *pTrs,D3DXVECTOR3 *pScl,D3DXQUATERNION *pRot,DWORD dwTimeElapse)
{
	const model::AnimJoint::tagJoint *pJointArray = pAnimJoint->GetJointArray();
	const model::AnimJoint::tagJoint &tJoint = pJointArray[dwJointIndex];
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength();
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();
	if (dwTimeElapse > dwAnimTimeLength)
		dwTimeElapse = dwAnimTimeLength;
	//插值
	//KEYFRAME A ->- B
	DWORD dwFrameA = dwTimeElapse * (dwAnimFrameCount -1) / dwAnimTimeLength;
	if (dwFrameA >= dwAnimFrameCount - 1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1;

		//a --- b
		model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	

		pTrs->x = tFrameA.vTranslation.x;
		pTrs->y = tFrameA.vTranslation.y;
		pTrs->z = tFrameA.vTranslation.z;

		pScl->x = tFrameA.vScale.x;
		pScl->y = tFrameA.vScale.y;
		pScl->z = tFrameA.vScale.z;

		pRot->x = tFrameA.qRot.x;
		pRot->y = tFrameA.qRot.y;
		pRot->z = tFrameA.qRot.z;
		pRot->w = tFrameA.qRot.w;
	}
	else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get KEYFRAME time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
		//计算A B 之间 ratio
		DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount -1) ;
		float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 

		//interpolation	a --- b
		model::AnimJoint::tagFrame &tFrameA = tJoint.pFrameArray[dwFrameA];   	
		model::AnimJoint::tagFrame &tFrameB = tJoint.pFrameArray[dwFrameB];   	

		//trans
		D3DXVec3Lerp(pTrs,&tFrameA.vTranslation,&tFrameB.vTranslation,fRatio);
		//scale
		D3DXVec3Lerp(pScl,&tFrameA.vScale,&tFrameB.vScale,fRatio);
		//rotation
		D3DXQuaternionSlerp(pRot,&tFrameA.qRot,&tFrameB.qRot,fRatio);
		D3DXQuaternionNormalize(pRot,pRot);
	}
}