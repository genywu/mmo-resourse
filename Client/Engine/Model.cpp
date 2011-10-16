
#include "stdafx.h"
#include "utility.h"
#include "console.h"
#include "render.h"
#include "model.h"



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
m_pJointMatrixBufferForVertex(NULL),
m_pJointMatrixBufferForNormals(NULL),
m_blight(false),
m_busenormal(false),
m_bsrendstatic(true),
m_bbuffcaculated(false),
m_bGpuCaculate(true),
m_bnoanim(true),
m_buseenvtex(false),
m_buseuvanim(false),
m_bbillboard(false),
m_bbillboardy(false),
m_bswprocess(false),
m_bNormalTransScal(false),
m_brendtextureprojcted(false),
m_dwBaseColor(0xffffffff)
{

}
model::Buffer::~Buffer()
{
	ASSERT(!m_pPosBuffer);
	ASSERT(!m_pNmlBuffer);
	ASSERT(!m_pUV0Buffer);
	ASSERT(!m_pColBufferDiffuse);
	ASSERT(!m_pJointMatrixBufferForVertex);
	ASSERT(!m_pJointMatrixBufferForNormals);
	ASSERT(!m_pIndexBuffer);
	//ASSERT(!m_pColBufferSpecular);		
}



void model::Buffer::Create(const model::Mesh *pMesh)
{
	m_dwPNCCount = pMesh->GetPNCCount();
	m_dwUV0Count = pMesh->GetUV0Count();
	m_bbuffcaculated = false;
	if (m_dwPNCCount > m_dwUV0Count)
	{
		MessageBox(NULL,NULL,NULL,MB_OK);
	}
	DWORD dwTriangleCount = pMesh->GetTriangleCount();
	//m_dwUV1Count = pMesh->GetUV1Count();	

	//运算缓冲
	m_pPosBuffer = new D3DXVECTOR3[m_dwPNCCount]; 
	if (!pMesh->GetGroupCount())
	{
		m_pJointMatrixBufferForVertex = new D3DXMATRIX[1];
		m_pJointMatrixBufferForNormals = new D3DXMATRIX[1];
		m_bsrendstatic = true;
	}
	else
	{
		m_pJointMatrixBufferForVertex = new D3DXMATRIX[pMesh->GetGroupCount()];
		m_pJointMatrixBufferForNormals = new D3DXMATRIX[pMesh->GetGroupCount()];
		m_bsrendstatic = false;
#pragma warning( push )
#pragma warning( disable : 4018 )
		if (pMesh->GetGroupCount() > render::Interface::GetInstance()->GetSetting()->ihvpmatrixcount)
		{
			m_bswprocess = true;
		}
#pragma warning( pop ) 
	}
	//m_pNmlBuffer = new D3DXVECTOR3[m_dwPNCCount]; 
	//m_pColBufferDiffuse = new D3DCOLOR[m_dwPNCCount];
	//m_pColBufferSpecular= new D3DCOLOR[m_dwPNCCount];
	//m_pUV0Buffer = new D3DXVECTOR2[m_dwUV0Count];
	/*m_dwAmbient = 0xffffffff;
	m_dwRegionAmbient = 0xffffffff;*/
	m_pAttachMesh = pMesh;

	m_pIndexBuffer = NULL;
	/*m_pIndexBuffer = new WORD[dwTriangleCount * 3];
	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	for(DWORD n = 0; n < dwTriangleCount; n++)
	{
	const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

	m_pIndexBuffer[3 * n + 0] = pTriangle->dwPNCIndex[0];
	m_pIndexBuffer[3 * n + 1] = pTriangle->dwPNCIndex[1];
	m_pIndexBuffer[3 * n + 2] = pTriangle->dwPNCIndex[2];
	}*/
}

void model::Buffer::Destroy()
{
	SAFEDELETEARRAY(m_pPosBuffer);
	SAFEDELETEARRAY(m_pNmlBuffer);
	SAFEDELETEARRAY(m_pColBufferDiffuse);
	//SAFEDELETEARRAY(m_pColBufferSpecular);
	SAFEDELETEARRAY(m_pUV0Buffer);
	SAFEDELETEARRAY(m_pIndexBuffer);
	SAFEDELETEARRAY(m_pJointMatrixBufferForVertex);
	SAFEDELETEARRAY(m_pJointMatrixBufferForNormals);
	//	SAFEDELETEARRAY(m_pUV1Buffer);  	
}
void model::Buffer::SetTempStreamType(DWORD dwShaderGroupHandle,DWORD type)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();
	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	pLayer3D->SetShaderTempStreamType(dwShaderGroupHandle,dwShaderIndex,type);
}
void model::Buffer::Render(DWORD dwShaderGroupHandle,DWORD dwMode)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();
	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	DWORD PNCcount = m_pAttachMesh->GetPNCCount();

	pLayer3D->SendBufferToCache(dwShaderGroupHandle,dwShaderIndex,PNCcount,dwTriangleCount * 3,m_pPosBuffer,m_pNmlBuffer,m_pColBufferDiffuse,m_pUV0Buffer,m_pIndexBuffer,m_blight,dwMode);
}
void model::Buffer::Render(DWORD dwShaderGroupHandle,DWORD dwShaderIndex,DWORD dwMode)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();
	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD PNCcount = m_pAttachMesh->GetPNCCount();


	pLayer3D->SendBufferToCache(dwShaderGroupHandle,dwShaderIndex,PNCcount,dwTriangleCount * 3,m_pPosBuffer,m_pNmlBuffer,m_pColBufferDiffuse,m_pUV0Buffer,m_pIndexBuffer,m_blight,dwMode);


}
void model::Buffer::RenderTransparent(DWORD dwShaderGroupHandle,DWORD dwMode)
{
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	DWORD PNCcount = m_pAttachMesh->GetPNCCount();
	pLayer3D->SendBufferToCache(dwShaderGroupHandle,dwShaderIndex,PNCcount,dwTriangleCount * 3,m_pPosBuffer,m_pNmlBuffer,m_pColBufferDiffuse,m_pUV0Buffer,m_pIndexBuffer,m_blight,dwMode);

}
/*
* 功能: 模型闪光
* 摘要: -
* 参数: dwShaderGroupHandle - 渲染组句柄
*		 dwMode				 - 渲染模式
* 返回值: -
* 作者: 彭谦
* 创建日期: 2008.09.2
*/
void model::Buffer::RenderModelFlash(DWORD dwShaderGroupHandle, DWORD dwMode)
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer3D * pLayer3D	= pInterface->GetLayer3D();
	DWORD dwShaderIndex					    = m_pAttachMesh->GetShaderIndex();
	DWORD dwTriangleCount					= m_pAttachMesh->GetTriangleCount();
	DWORD PNCcount							= m_pAttachMesh->GetPNCCount();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	if (!pShaderGroup)
	{
		return;
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer* pbuffer;
	pbuffer = pLayer3D->SendDynamicBufferToCache(dwShaderGroupHandle,dwShaderIndex,PNCcount,dwTriangleCount * 3,m_pPosBuffer,m_pNmlBuffer,m_pColBufferDiffuse,m_pUV0Buffer,m_pIndexBuffer,m_blight,dwMode);
	if (pbuffer)
	{
		D3DXMATRIX mat;
		pbuffer->dwTempColorOp   = D3DTOP_ADD;
		pbuffer->dwTempColorArg2 = pShader->tLayer->dwColorArg2;
		pbuffer->dwTempColorArg1 = pShader->tLayer->dwColorArg1;
		pbuffer->dwTempAlphaOp   = pShader->tLayer->dwAlphaOp;
		pbuffer->dwTempAlphaArg2 = pShader->tLayer->dwAlphaArg2;
		pbuffer->dwTempAlphaArg1 = pShader->tLayer->dwAlphaArg1;
		pbuffer->matworld = *D3DXMatrixIdentity(&mat);
	}
}
/*
* 功能: 渲染静态模型
* 摘要: -
* 参数: dwShaderGroupHandle - 渲染组句柄
*		 dwMode				 - 渲染模式
* 返回值: -
* 作者: 彭谦
* 创建日期: 2008.09.2
*/
void model::Buffer::RenderStatic(DWORD dwShaderGroupHandle, DWORD dwMode)
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer3D * pLayer3D	= pInterface->GetLayer3D();
	DWORD dwShaderIndex					    = m_pAttachMesh->GetShaderIndex();
	DWORD dwTriangleCount					= m_pAttachMesh->GetTriangleCount();
	DWORD PNCcount							= m_pAttachMesh->GetPNCCount();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	if (!pShaderGroup)
	{
		return;
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
	render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer* pbuffer;
	pbuffer = pLayer3D->SendDynamicBufferToCache(dwShaderGroupHandle,dwShaderIndex,PNCcount,dwTriangleCount * 3,m_pPosBuffer,m_pNmlBuffer,m_pColBufferDiffuse,m_pUV0Buffer,m_pIndexBuffer,m_blight,dwMode);
	if (pbuffer)
	{
		pbuffer->dwTempColorOp   = pShader->tLayer->dwColorOp;
		pbuffer->dwTempColorArg2 = pShader->tLayer->dwColorArg2;
		pbuffer->dwTempColorArg1 = pShader->tLayer->dwColorArg1;
		pbuffer->dwTempAlphaOp   = pShader->tLayer->dwAlphaOp;
		pbuffer->dwTempAlphaArg2 = pShader->tLayer->dwAlphaArg2;
		pbuffer->dwTempAlphaArg1 = pShader->tLayer->dwAlphaArg1;
		pbuffer->matworld = m_matworld;
	}
}


/*
* 功能: 使用动态着色变量进行渲染
* 摘要: -
* 参数: dwShaderGroupHandle - 渲染组句柄
*		 dwMode				 - 渲染模式
*		 pShaderState		 - 动态着色变量
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.23
*/
void model::Buffer::RenderByShaderState(DWORD dwShaderGroupHandle, DWORD dwMode, const render::Interface::Layer3D::_tagShaderState * pShaderState)
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer3D * pLayer3D	= pInterface->GetLayer3D();


	DWORD dwShaderIndex								 = m_pAttachMesh->GetShaderIndex();
	DWORD dwTriangleCount							 = m_pAttachMesh->GetTriangleCount();
	const model::Mesh::tagTriangle * pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();

	render::Interface::Layer3D::ShaderGroup::_tagShader * pDynamicShader = pLayer3D->CreateDynamicShader(dwShaderGroupHandle, dwShaderIndex, pShaderState);
	if (pDynamicShader == NULL)
		return;
	static render::Interface::Layer3D::_tagTriangle triangle[20000];
	DWORD usenum = 0;
	for(DWORD n = 0; n < dwTriangleCount; n++,usenum++)
	{
		const model::Mesh::tagTriangle *pTriangle = &pTriangleBuffer[n];

		triangle[usenum].pos[0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		triangle[usenum].pos[1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		triangle[usenum].pos[2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		triangle[usenum].color[0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
		triangle[usenum].color[1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
		triangle[usenum].color[2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

		triangle[usenum].specular[0] = 0x00000000;
		triangle[usenum].specular[1] = 0x00000000;
		triangle[usenum].specular[2] = 0x00000000;

		triangle[usenum].uv0[0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		triangle[usenum].uv0[1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		triangle[usenum].uv0[2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];

		if (((usenum)  > 19980))
		{
			pLayer3D->SendTriangleToCacheEx(pDynamicShader,usenum,triangle,dwMode);
			usenum = -1;
		}

	}   
	pLayer3D->SendTriangleToCacheEx(pDynamicShader,usenum,triangle,dwMode);

	/*static DWORD		dwColorDiffuse[20000];
	static DWORD		dwColorSpecular[20000];
	static D3DXVECTOR3	temp[20000];
	static D3DXVECTOR2	uv0[20000];

	DWORD usenum = 0;
	for(DWORD n = 0; n < dwTriangleCount; ++n,++usenum)
	{
	const model::Mesh::tagTriangle * pTriangle = &pTriangleBuffer[n];

	temp[usenum * 3 + 0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
	temp[usenum * 3 + 1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
	temp[usenum * 3 + 2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

	dwColorDiffuse[usenum * 3 + 0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
	dwColorDiffuse[usenum * 3 + 1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
	dwColorDiffuse[usenum * 3 + 2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

	dwColorSpecular[usenum * 3 + 0] = m_pColBufferSpecular[pTriangle->dwPNCIndex[0]];
	dwColorSpecular[usenum * 3 + 1] = m_pColBufferSpecular[pTriangle->dwPNCIndex[1]];
	dwColorSpecular[usenum * 3 + 2] = m_pColBufferSpecular[pTriangle->dwPNCIndex[2]];

	uv0[usenum * 3 + 0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
	uv0[usenum * 3 + 1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
	uv0[usenum * 3 + 2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];
	if (((usenum * 3 + 2)  > 19980))
	{
	pLayer3D->SendTriangleToCacheEx(pDynamicShader, usenum, temp, uv0, dwColorDiffuse, dwColorSpecular, dwMode);
	usenum = -1;
	}
	}
	pLayer3D->SendTriangleToCacheEx(pDynamicShader, usenum, temp, uv0, dwColorDiffuse, dwColorSpecular, dwMode);*/
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
	if (m_dwPNCCount > LAYER3D_DRAWVERTEXBUFFER_SIZE)
	{
		Log4c::Warn(ROOT_MODULE,"绘制顶点数超过BUFFER长度");
		return;
	}
	if (!m_dwPNCCount||render::Interface::GetInstance()->GetDeviceLost())
	{
		return;
	}

	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();
	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	if (!pShaderGroup)
	{
		return;
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
	if (!pShader)
	{
		return;
	}
	// Setup sampler Texture Filter
	pInterface->SetTexture(0,pShaderGroup->GetTextureHandle(0,pShader->tLayer[0].dwTexture));
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
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
	D3DXVECTOR3 *pCachePos(NULL);
	D3DXVECTOR3    *pCacheNormal(NULL);
	D3DCOLOR*    pCacheDiffuse(NULL);
	D3DXVECTOR2 *pCacheUV(NULL);
	//WORD   *pCacheIB(NULL);
	pLayer3D->SetStreamType(LMST_POSCOLTEX0);
	pDevice->SetStreamSource(0,pLayer3D->m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
	//pDevice->SetStreamSource(1,pLayer3D->m_pVB_NORMAL,0,sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2,pLayer3D->m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3,pLayer3D->m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
	//pDevice->SetIndices(pLayer3D->m_pIB);

	LPDIRECT3DVERTEXBUFFER9 pVBPOS  = pLayer3D->m_pVB_POSITION;
	LPDIRECT3DVERTEXBUFFER9 pVBNORMAL  = pLayer3D->m_pVB_NORMAL;
	LPDIRECT3DVERTEXBUFFER9 pVBDiffuse = pLayer3D->m_pVB_COLOR_DIFFUSE;
	LPDIRECT3DVERTEXBUFFER9 pVBUV0  = pLayer3D->m_pVB_TEX0;
	//LPDIRECT3DINDEXBUFFER9  pIB  = pLayer3D->m_pIB;

	pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
	pVBDiffuse->Lock( 0,0, (void**)&pCacheDiffuse,D3DLOCK_DISCARD);
	pVBUV0->Lock( 0,0, (void**)&pCacheUV,D3DLOCK_DISCARD);
	memcpy(pCachePos,m_pPosBuffer,sizeof(D3DXVECTOR3) * m_dwPNCCount);
	memcpy(pCacheDiffuse,m_pColBufferDiffuse,sizeof(D3DCOLOR) * m_dwPNCCount);
	memcpy(pCacheUV,m_pUV0Buffer,sizeof(D3DXVECTOR2) * m_dwPNCCount);
	pVBPOS->Unlock();
	pVBDiffuse->Unlock();
	pVBUV0->Unlock();
	//pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);
	//memcpy(pCacheIB ,m_pIndexBuffer,sizeof(WORD) * GetAttachMesh()->GetTriangleCount() * 3);
	//pIB->Unlock();
	//pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0,m_dwPNCCount,0,GetAttachMesh()->GetTriangleCount());
	if (GetRendStatic())
	{
		pDevice->SetTransform(D3DTS_WORLD,&m_matworld);
	}
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,m_dwPNCCount/3);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	if (GetRendStatic())
	{
		D3DXMATRIX matid;
		D3DXMatrixIdentity(&matid);
		pDevice->SetTransform(D3DTS_WORLD,&matid);
	}



}
void model::Buffer::DrawWithoutState(DWORD dwShaderGroupHandle)
{
	if (m_dwPNCCount > LAYER3D_DRAWVERTEXBUFFER_SIZE)
	{
		Log4c::Warn(ROOT_MODULE,"绘制顶点数超过BUFFER长度");
		return;
	}
	if (!m_dwPNCCount||render::Interface::GetInstance()->GetDeviceLost())
	{
		return;
	}
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle *pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD dwShaderIndex = m_pAttachMesh->GetShaderIndex();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	if (!pShaderGroup)
	{
		return;
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
	if (!pShader)
	{
		return;
	}
	D3DXVECTOR3 *pCachePos(NULL);
	D3DCOLOR    *pCacheDiffuse(NULL);   
	D3DXVECTOR2 *pCacheUV(NULL);
	//WORD   *pCacheIB(NULL);
	pDevice->SetVertexDeclaration(pLayer3D->m_pVertexDeclarationArray[LMST_POSTEX0]);
	pDevice->SetStreamSource(0,pLayer3D->m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
	//	pDevice->SetStreamSource(1,pLayer3D->m_pVB_COLOR_DIFFUSE,0,sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3,pLayer3D->m_pVB_TEX0,0,sizeof(D3DXVECTOR2));
	//pDevice->SetIndices(pLayer3D->m_pIB);
	pInterface->SetTexture(0,pShaderGroup->GetTextureHandle(0,pShader->tLayer[0].dwTexture));
	LPDIRECT3DVERTEXBUFFER9 pVBPOS  = pLayer3D->m_pVB_POSITION;
	LPDIRECT3DVERTEXBUFFER9 pVBUV0  = pLayer3D->m_pVB_TEX0;
	//LPDIRECT3DINDEXBUFFER9  pIB  = pLayer3D->m_pIB;
	pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
	pVBUV0->Lock( 0,0, (void**)&pCacheUV,D3DLOCK_DISCARD);
	memcpy(pCachePos,m_pPosBuffer,sizeof(D3DXVECTOR3) * m_dwPNCCount);
	memcpy(pCacheUV,m_pUV0Buffer,sizeof(D3DXVECTOR2) * m_dwPNCCount);
	pVBPOS->Unlock();
	pVBUV0->Unlock();
	/*pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);
	memcpy(pCacheIB ,m_pIndexBuffer,sizeof(WORD) * GetAttachMesh()->GetTriangleCount() * 3);
	pIB->Unlock();
	pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0,m_dwPNCCount,0,GetAttachMesh()->GetTriangleCount());*/
	if (GetRendStatic())
	{
		pDevice->SetTransform(D3DTS_WORLD,&m_matworld);
	}
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,m_dwPNCCount/3);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	if (GetRendStatic())
	{
		D3DXMATRIX matid;
		D3DXMatrixIdentity(&matid);
		pDevice->SetTransform(D3DTS_WORLD,&matid);
	}



}
void model::Buffer::DrawWithoutStateAndTexture(DWORD dwShaderGroupHandle)
{

	if (m_dwPNCCount > LAYER3D_DRAWVERTEXBUFFER_SIZE)
	{
		Log4c::Warn(ROOT_MODULE,"绘制顶点数超过BUFFER长度");
		return;
	}
	if (!m_dwPNCCount||render::Interface::GetInstance()->GetDeviceLost())
	{
		return;
	}
	//渲染
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	pLayer3D->SetStreamType(LMST_POS);
	D3DXVECTOR3 *pCachePos(NULL);
	//WORD   *pCacheIB(NULL);

	pDevice->SetStreamSource(0,pLayer3D->m_pVB_POSITION,0,sizeof(D3DXVECTOR3));
	//pDevice->SetIndices(pLayer3D->m_pIB);

	LPDIRECT3DVERTEXBUFFER9 pVBPOS  = pLayer3D->m_pVB_POSITION;
	//LPDIRECT3DINDEXBUFFER9  pIB  = pLayer3D->m_pIB;

	//填充VB进行绘图

	pVBPOS->Lock( 0,0, (void**)&pCachePos,D3DLOCK_DISCARD);
	//pIB->Lock( 0,0, (void**)&pCacheIB,D3DLOCK_DISCARD);

	memcpy(pCachePos,m_pPosBuffer,sizeof(D3DXVECTOR3) * m_dwPNCCount);
	//memcpy(pCacheIB ,m_pIndexBuffer,sizeof(WORD) * GetAttachMesh()->GetTriangleCount() * 3);

	pVBPOS->Unlock();
	//pIB->Unlock();

	//pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,0,0,m_dwPNCCount,0,GetAttachMesh()->GetTriangleCount());
	if (GetRendStatic())
	{
		pDevice->SetTransform(D3DTS_WORLD,&m_matworld);
	}

	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,m_dwPNCCount/3);
	pDevice->SetStreamSource(0,NULL,0,0);
	if (GetRendStatic())
	{
		D3DXMATRIX matid;
		D3DXMatrixIdentity(&matid);
		pDevice->SetTransform(D3DTS_WORLD,&matid);
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
	if (!pShaderGroup)
	{
		return;
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader = pShaderGroup->GetShader(dwShaderIndex);
	if (!pShader)
	{
		return;
	}
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



/*
* 功能: 使用颜色操作直接绘制模型
* 摘要: -
* 参数: dwShaderGroupHandle - 渲染组句柄
*		 dwMode				 - 渲染模式
*		 dwColorOp			 - 颜色操作
*		 dwColorArg1		 - 颜色参数1
*		 dwColorArg2		 - 颜色参数2
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.16
*/
void model::Buffer::DrawByColorOp(DWORD dwShaderGroupHandle,DWORD dwMode, DWORD dwColorOp, DWORD dwColorArg1, DWORD dwColorArg2)
{
	//渲染
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer3D * pLayer3D	= pInterface->GetLayer3D();
	LPDIRECT3DDEVICE9			 pDevice	= pInterface->GetDevice();

	static D3DXVECTOR3 temp[20000];
	static D3DXVECTOR2 uv0[20000];
	static DWORD	   color[20000];
	DWORD dwTriangleCount = m_pAttachMesh->GetTriangleCount();

	const model::Mesh::tagTriangle * pTriangleBuffer = m_pAttachMesh->GetTriangleBuffer();
	DWORD							 dwShaderIndex	 = m_pAttachMesh->GetShaderIndex();

	render::Interface::Layer3D::ShaderGroup				* pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	if (!pShaderGroup)
	{
		return;
	}
	render::Interface::Layer3D::ShaderGroup::_tagShader * pShader	   = pShaderGroup->GetShader(dwShaderIndex);
	if (!pShader)
	{
		return;
	}
	// Setup sampler Texture Filter
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	if (dwMode == 0)
	{
		// render opaque mode : mask and none
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);	

		pInterface->Cull(!pShader->bTwoSide);

		//0 test
		if (!pShader->dwAlphaTestMode)
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
	}else//混合
	{
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);	
		pInterface->Cull(!pShader->bTwoSide);

		//设置Alpha blend mode
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		//blend
		switch(pShader->dwAlphaBlendMode)
		{
		case 0://blend + additive
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			break;
		case 1://blend + alpha
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			break;
		case 2://blend + color
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
			break;
		case 3://blend + reserved
			break;
		}
	}

	DWORD n = 0;
	DWORD usenum = 0;
	for(n = 0; n < dwTriangleCount; ++n,++usenum)
	{
		const model::Mesh::tagTriangle * pTriangle = &pTriangleBuffer[n];

		temp[n * 3 + 0] = m_pPosBuffer[pTriangle->dwPNCIndex[0]];
		temp[n * 3 + 1] = m_pPosBuffer[pTriangle->dwPNCIndex[1]];
		temp[n * 3 + 2] = m_pPosBuffer[pTriangle->dwPNCIndex[2]];

		color[n * 3 + 0] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[0]];
		color[n * 3 + 1] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[1]];
		color[n * 3 + 2] = m_pColBufferDiffuse[pTriangle->dwPNCIndex[2]];

		uv0[n * 3 + 0] = m_pUV0Buffer[pTriangle->dwUV0Index[0]];
		uv0[n * 3 + 1] = m_pUV0Buffer[pTriangle->dwUV0Index[1]];
		uv0[n * 3 + 2] = m_pUV0Buffer[pTriangle->dwUV0Index[2]];
		if (((usenum * 3 + 2)  > 19980))
		{
			pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,usenum,temp,uv0,color);
			usenum = -1;
		}
		/*	uv1[0] = m_pUV1Buffer[pTriangle->dwUV1Index[0]];
		uv1[1] = m_pUV1Buffer[pTriangle->dwUV1Index[1]];
		uv1[2] = m_pUV1Buffer[pTriangle->dwUV1Index[2]];		*/

		//pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,1,temp,uv0,uv1,color);

	}

	// 设置颜色操作
	render::Interface::Layer3D::ShaderGroup::_tagShader::_tagLayer * pTempLayer(NULL);
	pTempLayer = &pShader->tLayer[0];
	pDevice->SetTextureStageState(0,D3DTSS_COLOROP,dwColorOp);
	pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,dwColorArg1);
	pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,dwColorArg2);

	pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,pTempLayer->dwAlphaOp);
	pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,pTempLayer->dwAlphaArg1);
	pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,pTempLayer->dwAlphaArg2);

	//pTempLayer = &pShader->tLayer[1];
	//pDevice->SetTextureStageState(1,D3DTSS_COLOROP,pTempLayer->dwColorOp);
	//pDevice->SetTextureStageState(1,D3DTSS_COLORARG1,pTempLayer->dwColorArg1);
	//pDevice->SetTextureStageState(1,D3DTSS_COLORARG2,pTempLayer->dwColorArg2);
	//pDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,pTempLayer->dwAlphaOp);
	//pDevice->SetTextureStageState(1,D3DTSS_ALPHAARG1,pTempLayer->dwAlphaArg1);
	//pDevice->SetTextureStageState(1,D3DTSS_ALPHAARG2,pTempLayer->dwAlphaArg2);

	pLayer3D->DrawTriangleArray(dwShaderGroupHandle,dwShaderIndex,usenum,temp,uv0,color);
}
void model::Buffer::MultiAlphaToColor()
{
	DWORD *pColBuffer = GetColBufferDiffuse();
	if (!pColBuffer)
	{
		return;
	}
	float alpha = (float) ( (*pColBuffer & 0xFF000000) >> 24)/ 255.0f;
	if (GetGpuCaculate())
	{
		DWORD &dwDestColor = m_dwBaseColor;

		float fR = (float) ( (dwDestColor & 0x00FF0000) >> 16) / 255.0f * alpha;
		float fG = (float) ( (dwDestColor & 0x0000FF00) >>  8) / 255.0f * alpha;
		float fB = (float) ( (dwDestColor & 0x000000FF)) / 255.0f * alpha;

		DWORD r   = (DWORD)(( fR) * 255.0f);  	
		if (r > 255) r = 0x000000FF;
		DWORD g = (DWORD)(( fG) * 255.0f);  	
		if (g > 255) g = 0x000000FF;
		DWORD b = (DWORD)(( fB) * 255.0f);  	
		if (b > 255) b = 0x000000FF;
		dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;

	}
	else
	{
		//颜色相乘
		for(DWORD n = 0; n < GetPNCCount(); n++)
		{
			DWORD &dwDestColor = pColBuffer[n];

			float fR = (float) ( (dwDestColor & 0x00FF0000) >> 16) / 255.0f * alpha;
			float fG = (float) ( (dwDestColor & 0x0000FF00) >>  8) / 255.0f * alpha;
			float fB = (float) ( (dwDestColor & 0x000000FF)) / 255.0f * alpha;

			DWORD r   = (DWORD)(( fR) * 255.0f);  	
			if (r > 255) r = 0x000000FF;
			DWORD g = (DWORD)(( fG) * 255.0f);  	
			if (g > 255) g = 0x000000FF;
			DWORD b = (DWORD)(( fB) * 255.0f);  	
			if (b > 255) b = 0x000000FF;
			dwDestColor = (dwDestColor & 0xFF000000) | (r << 16) | (g << 8) | b;

		}

	}



}



//__CLASS__(model::Mesh)____
model::Mesh::Mesh():
m_pPosBuffer(NULL),
m_pNmlBuffer(NULL),
m_pColBufferDiffuse(NULL),
m_pdwJointIndexBuffer(NULL),
m_pwIndexBuffer(NULL),
m_dwPNCCount(0),   
m_dwIndexCount(0), 
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
	ASSERT(!m_pdwJointIndexBuffer);
	ASSERT(!m_pwIndexBuffer);
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

BOOL model::Mesh::Create(CRFile* pFile)
{
	float fTemp3[3];

	DWORD dwFlag,dwVersion;
	pFile->ReadData(&dwFlag,sizeof(DWORD));
	pFile->ReadData(&dwVersion,sizeof(DWORD));

	if (dwFlag != MAKEFOURCC('M','E','S','H'))
	{
		OutputError("mesh type doesn't match!\n");
		MessageBox(NULL,"","Mesh类型不匹配,打开失败",MB_OK);
		return FALSE;
	}

	if (dwVersion != MAKEFOURCC('2','.','0','0'))
	{
		OutputError("Mesh version doesn't match!\n");
		MessageBox(NULL,"","Mesh版本错误",MB_OK);
		return FALSE;
	}

	//read shader index
	pFile->ReadData(&m_dwShaderIndex,sizeof(DWORD));

	//read position
	pFile->ReadData(&m_dwPNCCount,sizeof(DWORD));
	m_pPosBuffer = new D3DXVECTOR3[m_dwPNCCount];
	m_pNmlBuffer = new D3DXVECTOR3[m_dwPNCCount];
	m_pColBufferDiffuse = new D3DCOLOR[m_dwPNCCount];
	for(DWORD dwPos = 0; dwPos < m_dwPNCCount; dwPos++)
	{
		pFile->ReadData(fTemp3,sizeof(float) * 3 );
		m_pPosBuffer[dwPos].x = fTemp3[0];		
		m_pPosBuffer[dwPos].y = fTemp3[1];
		m_pPosBuffer[dwPos].z = fTemp3[2];
	}
	//read normal
	for(DWORD dwNml = 0; dwNml < m_dwPNCCount; dwNml++)
	{
		pFile->ReadData(fTemp3,sizeof(float) * 3 );
		m_pNmlBuffer[dwNml].x = fTemp3[0];		
		m_pNmlBuffer[dwNml].y = fTemp3[1];
		m_pNmlBuffer[dwNml].z = fTemp3[2];
	}
	//color 

	for(DWORD dwCol = 0; dwCol < m_dwPNCCount; dwCol++)
	{
		DWORD color;
		pFile->ReadData(&color,sizeof(DWORD));
		m_pColBufferDiffuse[dwCol]  = color;
	}

	//texture 0 coord
	pFile->ReadData(&m_dwUV0Count,sizeof(DWORD));
	m_pUV0Buffer = new D3DXVECTOR2[m_dwUV0Count];
	for(DWORD dwUV0 = 0; dwUV0 < m_dwUV0Count; dwUV0++)
	{
		pFile->ReadData(fTemp3,sizeof(float)*2);
		m_pUV0Buffer[dwUV0].x = fTemp3[0];
		m_pUV0Buffer[dwUV0].y = fTemp3[1];
	}

	//texture 1 coord
	DWORD dwUV1Count;
	pFile->ReadData(&dwUV1Count,sizeof(DWORD));
	//m_pUV1Buffer = new D3DXVECTOR2[m_dwUV1Count];
	for(DWORD dwUV1 = 0; dwUV1 < dwUV1Count; dwUV1++)
	{
		pFile->ReadData(fTemp3,sizeof(float)*2);
		//m_pUV1Buffer[dwUV1].x = fTemp3[0];
		//m_pUV1Buffer[dwUV1].y = fTemp3[1];
	}

	//为了优化渲染从新组织数据---ADD BY 彭谦---//
	static D3DXVECTOR3 ppos[50000];
	static D3DXVECTOR3 pnml[50000];
	static D3DXVECTOR2 puv[50000];
	static D3DCOLOR	   pcol[50000];
	ZeroMemory(ppos,sizeof(D3DXVECTOR3) * 50000);
	ZeroMemory(pnml,sizeof(D3DXVECTOR3) * 50000);
	ZeroMemory(puv,sizeof(D3DXVECTOR2) * 50000);
	ZeroMemory(pcol,sizeof(D3DCOLOR) * 50000);
	DWORD num = 0;
	//triangle
	DWORD dwUV1Index[3];
	pFile->ReadData(&m_dwTriangleCount,sizeof(DWORD));
	m_pTriangleBuffer = new tagTriangle[m_dwTriangleCount];
	DWORD dwindexnum = m_dwTriangleCount * 3;
	hash_map<DWORD,std::list<DWORD>> mapsouseindex;
	hash_map<DWORD,std::list<DWORD>>::iterator itmapsouseindex;
	hash_map<DWORD,std::pair<DWORD,DWORD>> mapuvpossource;
	hash_map<DWORD,std::pair<DWORD,DWORD>>::iterator itmapuvpossource;
	for(DWORD dwTriangle = 0; dwTriangle < m_dwTriangleCount; dwTriangle++)
	{
		tagTriangle * pTriangle = &m_pTriangleBuffer[dwTriangle];
		pFile->ReadData(pTriangle->dwPNCIndex,sizeof(DWORD) * 3);
		pFile->ReadData(pTriangle->dwUV0Index,sizeof(DWORD) * 3);
		pFile->ReadData(dwUV1Index,sizeof(DWORD) * 3);
		for (int i = 0; i < 3;i++)
		{
			DWORD posindex = pTriangle->dwPNCIndex[i];
			DWORD uvindex = pTriangle->dwUV0Index[i];
			if (posindex > m_dwPNCCount - 1||uvindex > m_dwUV0Count - 1)
			{
				MessageBox(NULL,"越界","越界",MB_OK);
			}
			itmapuvpossource = mapuvpossource.find(uvindex);
			if (itmapuvpossource != mapuvpossource.end())
			{
				if (posindex == mapuvpossource[uvindex].first )
				{
					pTriangle->dwPNCIndex[i] = mapuvpossource[uvindex].second;
					pTriangle->dwUV0Index[i] = mapuvpossource[uvindex].second;
				}
				else
				{
					ASSERT(0);
				}
			}
			else
			{
				puv[num]  = m_pUV0Buffer[uvindex];
				ppos[num] = m_pPosBuffer[posindex];
				pcol[num] = m_pColBufferDiffuse[posindex];
				pnml[num] = m_pNmlBuffer[posindex];
				pTriangle->dwPNCIndex[i] = num;
				pTriangle->dwUV0Index[i] = num;
				mapsouseindex[posindex].push_back(num);
				mapuvpossource[uvindex].first = posindex;
				mapuvpossource[uvindex].second = num;
				num++;

			}

		}
		//	OutputConsole("%d %d %d \n",pTriangle->dwUV0Index[0],pTriangle->dwUV0Index[1],pTriangle->dwUV0Index[2]);
	}
	SAFEDELETEARRAY(m_pPosBuffer);
	SAFEDELETEARRAY(m_pColBufferDiffuse);
	SAFEDELETEARRAY(m_pUV0Buffer);
	SAFEDELETEARRAY(m_pNmlBuffer);
	SAFEDELETEARRAY(m_pdwJointIndexBuffer);
	SAFEDELETEARRAY(m_pwIndexBuffer);
	m_pPosBuffer = new D3DXVECTOR3[num];
	m_pColBufferDiffuse = new D3DCOLOR[num];
	m_pUV0Buffer = new D3DXVECTOR2[num];
	m_pNmlBuffer = new D3DXVECTOR3[num];
	m_pdwJointIndexBuffer = new DWORD[num];
	m_pwIndexBuffer = new WORD[dwindexnum];
	memcpy(m_pPosBuffer,ppos,sizeof(D3DXVECTOR3) * (num ));
	memcpy(m_pNmlBuffer,pnml,sizeof(D3DXVECTOR3) * (num ));
	memcpy(m_pColBufferDiffuse,pcol,sizeof(D3DCOLOR) * (num ));
	memcpy(m_pUV0Buffer,puv,sizeof(D3DXVECTOR2) * (num ));
	float cx(0),cy(0),cz(0);
	DWORD posnum = 0;
	for(; posnum < num; posnum++)
	{
		cx += m_pPosBuffer[posnum].x;
		cy += m_pPosBuffer[posnum].y;
		cz += m_pPosBuffer[posnum].z;
	}

	float reciprocal = 1.0f / (float)num;

	cx *= reciprocal;
	cy *= reciprocal;
	cz *= reciprocal;
	m_vAveragepoint = D3DXVECTOR3(cx,cy,cz);
	for(DWORD n = 0; n < m_dwTriangleCount; n++)
	{
		tagTriangle * pTriangle = &m_pTriangleBuffer[n];
#pragma warning (push)
#pragma warning (disable:4244)
		m_pwIndexBuffer[3 * n + 0] = pTriangle->dwPNCIndex[0];
		m_pwIndexBuffer[3 * n + 1] = pTriangle->dwPNCIndex[1];
		m_pwIndexBuffer[3 * n + 2] = pTriangle->dwPNCIndex[2];
#pragma warning (pop)
	}
	m_dwPNCCount = num;
	m_dwUV0Count = num;
	m_dwIndexCount = dwindexnum;

	//读取影响组(为0)则没有

	pFile->ReadData(&m_dwGroupCount,sizeof(DWORD));
	if (m_dwGroupCount)
	{
		//读取顶点组别
		m_pGroupArray = new tagGroup[m_dwGroupCount];
		for(DWORD dwGroup = 0; dwGroup < m_dwGroupCount; dwGroup++)
		{
			tagGroup *pGroup = &m_pGroupArray[dwGroup];

			pFile->ReadData(&pGroup->dwJointIndex,sizeof(DWORD));		

			pFile->ReadData(&pGroup->dwPNCIndexStart,sizeof(DWORD));
			pFile->ReadData(&pGroup->dwPNCIndexCount,sizeof(DWORD));
#pragma warning (push)
#pragma warning (disable:4018)
			for (int i = pGroup->dwPNCIndexStart;i < pGroup->dwPNCIndexStart+pGroup->dwPNCIndexCount;i++)
#pragma warning (pop)
			{
				itmapsouseindex = mapsouseindex.find((DWORD)i);
				if(itmapsouseindex != mapsouseindex.end())
				{
					std::list<DWORD>& list = (*itmapsouseindex).second;
					if (!list.empty())
					{
						std::list<DWORD>::iterator it = list.begin();
#pragma warning (push)
#pragma warning (disable:4244)
						pGroup->listeffectindex.push_back(*it);
#pragma warning (pop)
						m_pdwJointIndexBuffer[*it] = dwGroup;
#pragma warning (push)
#pragma warning (disable:4244)
						WORD wsourceindex = *it;
#pragma warning (pop)
						it++;
#pragma warning (push)
#pragma warning (disable:4244)
						for(;it != list.end();it++)
#pragma warning (pop)
						{
							m_pdwJointIndexBuffer[*it] = dwGroup;
#pragma warning (push)
#pragma warning (disable:4244)
							pGroup->mapsourceeffectindex[wsourceindex].push_back(*it);
#pragma warning (pop)
						}
					}

				}

			}
		}
	}
	mapuvpossource.clear();
	mapsouseindex.clear();
	//为了优化渲染从新组织数据---ADD BY 彭谦---//






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
	SAFEDELETEARRAY(m_pdwJointIndexBuffer);
	SAFEDELETEARRAY(m_pwIndexBuffer);
	SAFEDELETEARRAY(m_pTriangleBuffer);
}



/*
* 功能: 处理骨骼动画插值
* 摘要: -
* 参数: pBuffer			   - 模型缓冲数据
*		 pWorldMatrix		   - 世界矩阵
*		 pBindPose			   - 绑定点信息
*		 pAnimJointA		   - 骨骼动画A信息
*		 dwTimeElapseA		   - 骨骼动画A流逝时间
*		 fAnimCustomSpeedRateA - 骨骼动画A自定义速率
*		 pAnimJointB		   - 骨骼动画B信息
*		 dwTimeElapseB		   - 骨骼动画B流逝时间
*		 fAnimCustomSpeedRateB - 骨骼动画B自定义速率
*		 fRatio				   - 两个动画之间的插值比例系数
*		 matrixworld		   - 世界矩阵
*		 matrixmodel		   - 模型矩阵
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.03.27 - lpf
*		为该函数增加了两个关于骨骼动画速率的参数,用以处理用户自定义动画时间对插值动画帧产生的影响
*/
void model::Mesh::ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,DWORD dwTimeElapseA,float fAnimCustomSpeedRateA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,float fAnimCustomSpeedRateB,float fRatio,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel)
{
	static D3DXVECTOR3 vTrsA,vSclA,vTrsB,vSclB,vTrs,vScl,vPos,vNml;
	static D3DXQUATERNION qRotA,qRotB,qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl,mTrsTemp,mInvTrsTpsTemp,matnowtemp;

	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();


	/*ZeroMemory(pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));*/

	const D3DXMATRIX *pMatBindPoseArray = pBindPose->GetBindPoseArray();

	const model::AnimJoint::tagJoint * pJointArrayA = pAnimJointA->GetJointArray();
	const model::AnimJoint::tagJoint * pJointArrayB = pAnimJointB->GetJointArray();

	for(DWORD dwJointGroup = 0; dwJointGroup < m_dwGroupCount;dwJointGroup ++)
	{
		tagGroup &tGroup = m_pGroupArray[dwJointGroup];

		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//先求出A的插值结果
		GetAnimFrameByTime(pAnimJointA,tGroup.dwJointIndex,&vTrsA,&vSclA,&qRotA,dwTimeElapseA,fAnimCustomSpeedRateA);
		//再求出B的插值结果
		GetAnimFrameByTime(pAnimJointB,tGroup.dwJointIndex,&vTrsB,&vSclB,&qRotB,dwTimeElapseB,fAnimCustomSpeedRateB);

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
		if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
		{
			pBuffer->SetNormalTransScal(true);
		}
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
		if (pBuffer->GetGpuCaculate())
		{
			if (pBuffer->GetIsBillBoard())
			{
				D3DXMATRIX matinview;
				D3DXMATRIX trans,intrans;
				matinview = *pBuffer->GetMatForBB();
				D3DXVECTOR3 vnowpoint;
				D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
				D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
				D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
				if (pBuffer->GetUseNormal())
				{
					if (pBuffer->GetNormalTransScal())
					{
						D3DXMATRIX matinvtrs = trans *  matScl * matinview  * intrans;
						D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
						D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
						pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
						pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}


				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
				}
				pBuffer->SetNoAnim(true);

			}
			else
			{
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						pBuffer->GetJointMatrixBufferForNormals()[dwJointGroup] = mInvTrsTpsTemp;
						pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}
				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
				}
				pBuffer->SetNoAnim(false);
			}
			pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
			pBuffer->SetMatView(pViewMatrix);
			pBuffer->SetMatProjective(pProjectedMatrix);
		}
		else
		{

			if (pBuffer->GetUseNormal())
			{
				D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					//POSITION BLEND
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					//NORMAL TRANSFORM
					D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
						pNmlBuffer[*listit] = pNmlBuffer[(*it)];
					}
				}
			}
			else
			{
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
					}
				}
			}

		}
	}
}



/*
* 功能: 处理骨骼动画插值
* 摘要: -
* 参数: pBuffer			   - 模型缓冲数据
*		 pWorldMatrix		   - 世界矩阵
*		 pBindPose			   - 绑定点信息
*		 pAnimJointA		   - 骨骼动画A信息
*		 dwTimeElapseA		   - 骨骼动画A流逝时间
*		 fAnimCustomSpeedRateA - 骨骼动画A自定义速率
*		 pAnimJointB		   - 骨骼动画B信息
*		 dwTimeElapseB		   - 骨骼动画B流逝时间
*		 fAnimCustomSpeedRateB - 骨骼动画B自定义速率
*		 fRatio				   - 两个动画之间的插值比例系数
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.03.27 - lpf
*		为该函数增加了两个关于骨骼动画速率的参数,用以处理用户自定义动画时间对插值动画帧产生的影响
*/
void model::Mesh::ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,DWORD dwTimeElapseA,float fAnimCustomSpeedRateA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,float fAnimCustomSpeedRateB,float fRatio)
{
	static D3DXVECTOR3 vTrsA,vSclA,vTrsB,vSclB,vTrs,vScl,vPos,vNml;
	static D3DXQUATERNION qRotA,qRotB,qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl,mTrsTemp,mInvTrsTpsTemp;

	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();

	/*ZeroMemory(pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));*/

	const D3DXMATRIX *pMatBindPoseArray = pBindPose->GetBindPoseArray();

	const model::AnimJoint::tagJoint * pJointArrayA = pAnimJointA->GetJointArray();
	const model::AnimJoint::tagJoint * pJointArrayB = pAnimJointB->GetJointArray();

	for(DWORD dwJointGroup = 0; dwJointGroup < m_dwGroupCount;dwJointGroup ++)
	{
		tagGroup &tGroup = m_pGroupArray[dwJointGroup];

		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//先求出A的插值结果
		GetAnimFrameByTime(pAnimJointA,tGroup.dwJointIndex,&vTrsA,&vSclA,&qRotA,dwTimeElapseA,fAnimCustomSpeedRateA);
		//再求出B的插值结果
		GetAnimFrameByTime(pAnimJointB,tGroup.dwJointIndex,&vTrsB,&vSclB,&qRotB,dwTimeElapseB,fAnimCustomSpeedRateB);

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
		if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
		{
			pBuffer->SetNormalTransScal(true);
		}
		D3DXMatrixRotationQuaternion(&matRot,&qRot);

		D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
		D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

		//MAKE DEFORMATION MATRIX

		D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);
		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);
		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);

		D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);
		if (pBuffer->GetGpuCaculate())
		{
			if (pBuffer->GetIsBillBoard())
			{
				D3DXMATRIX matinview;
				D3DXMATRIX trans,intrans;
				matinview = *pBuffer->GetMatForBB();
				D3DXVECTOR3 vnowpoint;
				D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
				D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
				D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
						D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
						D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
						pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
						pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);

					}
				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[0] =trans *  matinview  * intrans;
				}
				pBuffer->SetNoAnim(true);

			}
			else
			{
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
						pBuffer->GetJointMatrixBufferForNormals()[dwJointGroup] = mInvTrsTpsTemp;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] =mTrsTemp;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}


				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
				}
				pBuffer->SetNoAnim(false);
			}
			pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
			pBuffer->SetMatView(pViewMatrix);
			pBuffer->SetMatProjective(pProjectedMatrix);
		}
		else
		{

			if (pBuffer->GetUseNormal())
			{
				D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					//POSITION BLEND
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					//NORMAL TRANSFORM
					D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
						pNmlBuffer[*listit] = pNmlBuffer[(*it)];
					}
				}
			}
			else
			{
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
					}
				}
			}

		}


	}
}



/*
* 功能: 处理骨骼动画插值
* 摘要: -
* 参数: pBuffer			   - 模型缓冲数据
*		 pWorldMatrix		   - 世界矩阵
*		 pBindPose			   - 绑定点信息
*		 pAnimJointA		   - 骨骼动画A信息
*		 dwTimeElapseA		   - 骨骼动画A流逝时间
*		 fAnimCustomSpeedRateA - 骨骼动画A自定义速率
*		 pAnimJointB		   - 骨骼动画B信息
*		 dwTimeElapseB		   - 骨骼动画B流逝时间
*		 fAnimCustomSpeedRateB - 骨骼动画B自定义速率
*		 fRatio				   - 两个动画之间的插值比例系数
*		 matrixscal			   - 缩放矩阵
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.03.27 - lpf
*		为该函数增加了两个关于骨骼动画速率的参数,用以处理用户自定义动画时间对插值动画帧产生的影响
*/
void model::Mesh::ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,DWORD dwTimeElapseA,float fAnimCustomSpeedRateA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,float fAnimCustomSpeedRateB,float fRatio,D3DXMATRIX& matrixscal)
{
	static D3DXVECTOR3 vTrsA,vSclA,vTrsB,vSclB,vTrs,vScl,vPos,vNml;
	static D3DXQUATERNION qRotA,qRotB,qRot;
	static D3DXMATRIX matTemp,matTrs,matRot,matScl,mTrsTemp,mInvTrsTpsTemp;
	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();


	/*ZeroMemory(pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));*/

	const D3DXMATRIX *pMatBindPoseArray = pBindPose->GetBindPoseArray();

	const model::AnimJoint::tagJoint * pJointArrayA = pAnimJointA->GetJointArray();
	const model::AnimJoint::tagJoint * pJointArrayB = pAnimJointB->GetJointArray();

	for(DWORD dwJointGroup = 0; dwJointGroup < m_dwGroupCount;dwJointGroup ++)
	{
		tagGroup &tGroup = m_pGroupArray[dwJointGroup];

		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//先求出A的插值结果
		GetAnimFrameByTime(pAnimJointA,tGroup.dwJointIndex,&vTrsA,&vSclA,&qRotA,dwTimeElapseA,fAnimCustomSpeedRateA);
		//再求出B的插值结果
		GetAnimFrameByTime(pAnimJointB,tGroup.dwJointIndex,&vTrsB,&vSclB,&qRotB,dwTimeElapseB,fAnimCustomSpeedRateB);

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
		if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
		{
			pBuffer->SetNormalTransScal(true);
		}
		D3DXMatrixRotationQuaternion(&matRot,&qRot);

		D3DXMatrixMultiply(&matTemp,&matScl,&matRot);
		D3DXMatrixMultiply(&matTemp,&matTemp,&matTrs);

		//MAKE DEFORMATION MATRIX

		D3DXMatrixInverse(&mTrsTemp,NULL,pMatBindPose);
		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,&matTemp);
		D3DXMatrixMultiply(&mTrsTemp,&mTrsTemp,pWorldMatrix);	

		D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,&mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);
		if (pBuffer->GetGpuCaculate())
		{
			if (pBuffer->GetIsBillBoard())
			{
				D3DXMATRIX matinview;
				D3DXMATRIX trans,intrans;
				matinview = *pBuffer->GetMatForBB();
				D3DXVECTOR3 vnowpoint;
				D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
				D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
				D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
						D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
						D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
						pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
						pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}

				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
				}
				pBuffer->SetNoAnim(true);

			}
			else
			{
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						pBuffer->GetJointMatrixBufferForNormals()[dwJointGroup] = mInvTrsTpsTemp;
						pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}


				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[dwJointGroup] = mTrsTemp;
				}
				pBuffer->SetNoAnim(false);
			}
			pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
			pBuffer->SetMatView(pViewMatrix);
			pBuffer->SetMatProjective(pProjectedMatrix);
		}
		else
		{

			if (pBuffer->GetUseNormal())
			{
				D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					//POSITION BLEND
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					//NORMAL TRANSFORM
					D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
						pNmlBuffer[*listit] = pNmlBuffer[(*it)];
					}
				}
			}
			else
			{
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
					}
				}
			}

		}


	}
	matrixscal = matScl;

}

void model::Mesh::ProcessAnimJointByFrame(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,DWORD dwFrame)
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

	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();
	DWORD dwFrameA = dwFrame;
	if (dwFrameA <=dwAnimFrameCount - 1 && dwFrameA >= 0)
	{
		//******************//因为LOCATOR只有一个GROUP,所以此处用0***********
		tagGroup &tGroup = m_pGroupArray[0];
		const model::AnimJoint::tagJoint &tJoint = pJointArray[tGroup.dwJointIndex];
		//BINDPOSE MATRIX	
		const D3DXMATRIX *pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];
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

		if (pBuffer->GetUseNormal())
		{
			list<WORD>::iterator it = tGroup.listeffectindex.begin();
			for (;it != tGroup.listeffectindex.end();it++)
			{
				//POSITION BLEND
				D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
				//NORMAL TRANSFORM
				D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
				std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
				std::list<WORD>::iterator listit = list.begin();
				for (;listit != list.end();listit++)
				{
					pPosBuffer[*listit] = pPosBuffer[(*it)];
					pNmlBuffer[*listit] = pNmlBuffer[(*it)];
				}
			}
		}
		else
		{
			list<WORD>::iterator it = tGroup.listeffectindex.begin();
			for (;it != tGroup.listeffectindex.end();it++)
			{
				D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
				std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
				std::list<WORD>::iterator listit = list.begin();
				for (;listit != list.end();listit++)
				{
					pPosBuffer[*listit] = pPosBuffer[(*it)];
				}
			}
		}

	}


}



/*
* 功能: 处理骨骼动画信息
* 摘要: -
* 参数: pBuffer			  - 模型缓冲数据
*		 pWorldMatrix		  - 世界矩阵
*		 pBindPose			  - 模型绑定数据
*		 pAnimJoint			  - 动画骨骼信息
*		 dwAnimTimeElapse	  - 动画流逝时间
*		 fAnimCustomSpeedRate - 自定义动画速率
*		 matrixworld		  -
*		 matrixmodel		  -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.02.22 - lpf
*		为该函数原形增加了一个fAnimCustomSpeedRate参数,用以提供用户自定义的动画时间
*/
void model::Mesh::ProcessAnimJoint(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,model::AnimJoint *pAnimJoint,DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel)
{
	static D3DXVECTOR3	  vTrs, vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX	  matTemp, matTrs, matRot, matScl;

	D3DXVECTOR3 * pPosBuffer = pBuffer->GetPosBuffer();


	/*ZeroMemory(pPosBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));*/

	D3DXMATRIX  mTrsTemp, mInvTrsTpsTemp;
	D3DXVECTOR3 vPos, vNml;

	const model::AnimJoint::tagJoint * pJointArray		 = pAnimJoint->GetJointArray();
	const D3DXMATRIX				 * pMatBindPoseArray = pBindPose->GetBindPoseArray();

	DWORD dwAnimTime	   = dwAnimTimeElapse;
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength(fAnimCustomSpeedRate);
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();

	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimTime * (dwAnimFrameCount - 1) / dwAnimTimeLength;
	if (dwFrameA >= dwAnimFrameCount - 1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1;

		for(DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
		{
			tagGroup & tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];

			//BINDPOSE MATRIX	
			const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//a --- b
			model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	

			D3DXMatrixTranslation(&matTrs, tFrameA.vTranslation.x, tFrameA.vTranslation.y, tFrameA.vTranslation.z);
			D3DXMatrixScaling(&matScl, tFrameA.vScale.x, tFrameA.vScale.y, tFrameA.vScale.z);
			if (fabs(tFrameA.vScale.x - 1.0f) > 0.0001||fabs(tFrameA.vScale.y - 1.0f) > 0.0001||fabs(tFrameA.vScale.z - 1.0f) > 0.0001)
			{
				pBuffer->SetNormalTransScal(true);
			}
			D3DXMatrixRotationQuaternion(&matRot, &tFrameA.qRot);

			D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
			D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
			matrixmodel = mTrsTemp;
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);			
			matrixworld = mTrsTemp;

			D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);

			if (pBuffer->GetGpuCaculate())
			{
				if (pBuffer->GetIsBillBoard())
				{
					D3DXMATRIX matinview;
					D3DXMATRIX trans,intrans;
					matinview = *pBuffer->GetMatForBB();
					D3DXVECTOR3 vnowpoint;
					D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
					D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
					D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
							D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
							D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
							pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
							pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
					}
					pBuffer->SetNoAnim(true);

				}
				else
				{
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					pBuffer->SetNoAnim(false);
				}
				pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);
			}
			else
			{

				if (pBuffer->GetUseNormal())
				{
					D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						//POSITION BLEND
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						//NORMAL TRANSFORM
						D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
							pNmlBuffer[*listit] = pNmlBuffer[(*it)];
						}
					}
				}
				else
				{
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
						}
					}
				}

			}	
		}
	}else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get A time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
		//计算A B 之间 ratio
		DWORD dwTimeRelateA  = dwAnimTime - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount - 1);
		float fRatio		 = (float)dwTimeRelateA / fFrameInterval;
		//OutputConsole("%f\n",fRatio);

		for (DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
		{
			tagGroup & tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];
			//BINDPOSE MATRIX
			const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//interpolation	a --- b
			model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	
			model::AnimJoint::tagFrame & tFrameB = tJoint.pFrameArray[dwFrameB];   	
			//trans
			D3DXVec3Lerp(&vTrs, &tFrameA.vTranslation, &tFrameB.vTranslation, fRatio);
			//scale
			D3DXVec3Lerp(&vScl, &tFrameA.vScale, &tFrameB.vScale, fRatio);
			//rotation
			D3DXQuaternionSlerp(&qRot, &tFrameA.qRot, &tFrameB.qRot, fRatio);
			D3DXQuaternionNormalize(&qRot, &qRot);

			D3DXMatrixTranslation(&matTrs, vTrs.x, vTrs.y, vTrs.z);
			D3DXMatrixScaling(&matScl, vScl.x, vScl.y, vScl.z);
			if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
			{
				pBuffer->SetNormalTransScal(true);
			}
			D3DXMatrixRotationQuaternion(&matRot, &qRot);

			D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
			D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
			matrixmodel = mTrsTemp;
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);			
			matrixworld = mTrsTemp;

			D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);
			if (pBuffer->GetGpuCaculate())
			{
				if (pBuffer->GetIsBillBoard())
				{
					D3DXMATRIX matinview;
					D3DXMATRIX trans,intrans;
					matinview = *pBuffer->GetMatForBB();
					D3DXVECTOR3 vnowpoint;
					D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
					D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
					D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
							D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
							D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
							pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
							pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
					}
					pBuffer->SetNoAnim(true);

				}
				else
				{
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					pBuffer->SetNoAnim(false);
				}
				pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);
			}
			else
			{

				if (pBuffer->GetUseNormal())
				{
					D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						//POSITION BLEND
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						//NORMAL TRANSFORM
						D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
							pNmlBuffer[*listit] = pNmlBuffer[(*it)];
						}
					}
				}
				else
				{
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
						}
					}
				}

			}


		}
	}
}



/*
* 功能: 处理骨骼动画信息
* 摘要: -
* 参数: pBuffer			  - 模型缓冲数据
*		 pWorldMatrix		  - 世界矩阵
*		 pBindPose			  - 模型绑定数据
*		 pAnimJoint			  - 动画骨骼信息
*		 dwAnimTimeElapse	  - 动画流逝时间
*		 fAnimCustomSpeedRate - 自定义动画速率
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.02.15 - lpf
*		为该函数原形增加了一个dwAnimCustomTimeLen参数,用以提供用户自定义的动画时间
*	2008.02.18 - lpf
*		将dwAnimCustomTimeLen参数修改为fAnimCustomSpeedRate
*/
void model::Mesh::ProcessAnimJoint(model::Buffer * pBuffer, const D3DXMATRIX * pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix, model::BindPose * pBindPose, model::AnimJoint * pAnimJoint, DWORD dwAnimTimeElapse, float fAnimCustomSpeedRate)
{
	static D3DXVECTOR3	  vTrs, vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX	  matTemp, matTrs, matRot, matScl;

	D3DXVECTOR3 * pPosBuffer = pBuffer->GetPosBuffer();
	/*ZeroMemory(pPosBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));*/

	D3DXMATRIX  mTrsTemp, mInvTrsTpsTemp;
	D3DXVECTOR3 vPos, vNml;

	const model::AnimJoint::tagJoint * pJointArray		 = pAnimJoint->GetJointArray();
	const D3DXMATRIX				 * pMatBindPoseArray = pBindPose->GetBindPoseArray();

	DWORD dwAnimTime	   = dwAnimTimeElapse;
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength(fAnimCustomSpeedRate);
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();

	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimTime * (dwAnimFrameCount - 1) / dwAnimTimeLength;
	if (dwFrameA >= dwAnimFrameCount - 1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1;

		for(DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
		{
			tagGroup & tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];

			//BINDPOSE MATRIX	
			const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//a --- b
			model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	

			D3DXMatrixTranslation(&matTrs, tFrameA.vTranslation.x, tFrameA.vTranslation.y, tFrameA.vTranslation.z);
			D3DXMatrixScaling(&matScl, tFrameA.vScale.x, tFrameA.vScale.y, tFrameA.vScale.z);
			if (fabs(tFrameA.vScale.x - 1.0f) > 0.0001||fabs(tFrameA.vScale.y - 1.0f) > 0.0001||fabs(tFrameA.vScale.z - 1.0f) > 0.0001)
			{
				pBuffer->SetNormalTransScal(true);
			}
			D3DXMatrixRotationQuaternion(&matRot, &tFrameA.qRot);

			D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
			D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);	

			D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);
			if (pBuffer->GetGpuCaculate())
			{
				if (pBuffer->GetIsBillBoard())
				{
					D3DXMATRIX matinview;
					D3DXMATRIX trans,intrans;
					matinview = *pBuffer->GetMatForBB();
					D3DXVECTOR3 vnowpoint;
					D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
					D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
					D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
							D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
							D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
							pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
							pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);

						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
					}
					pBuffer->SetNoAnim(true);

				}
				else
				{
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					pBuffer->SetNoAnim(false);
				}
				pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);
			}
			else
			{

				if (pBuffer->GetUseNormal())
				{
					D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						//POSITION BLEND
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						//NORMAL TRANSFORM
						D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
							pNmlBuffer[*listit] = pNmlBuffer[(*it)];
						}
					}
				}
				else
				{
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
						}
					}
				}

			}


		}
	}
	else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get A time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
		//计算A B 之间 ratio
		DWORD dwTimeRelateA  = dwAnimTime - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount - 1);
		float fRatio		 = (float)dwTimeRelateA / fFrameInterval;
		//OutputConsole("%f\n",fRatio);

		for (DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
		{
			tagGroup & tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];
			//BINDPOSE MATRIX
			const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//interpolation	a --- b
			model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	
			model::AnimJoint::tagFrame & tFrameB = tJoint.pFrameArray[dwFrameB];   	
			//trans
			D3DXVec3Lerp(&vTrs, &tFrameA.vTranslation, &tFrameB.vTranslation, fRatio);
			//scale
			D3DXVec3Lerp(&vScl, &tFrameA.vScale, &tFrameB.vScale, fRatio);
			//rotation
			D3DXQuaternionSlerp(&qRot, &tFrameA.qRot, &tFrameB.qRot, fRatio);
			D3DXQuaternionNormalize(&qRot, &qRot);

			D3DXMatrixTranslation(&matTrs, vTrs.x, vTrs.y, vTrs.z);
			D3DXMatrixScaling(&matScl, vScl.x, vScl.y, vScl.z);
			if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
			{
				pBuffer->SetNormalTransScal(true);
			}
			D3DXMatrixRotationQuaternion(&matRot, &qRot);

			D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
			D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);	

			D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);
			if (pBuffer->GetGpuCaculate())
			{
				if (pBuffer->GetIsBillBoard())
				{
					D3DXMATRIX matinview;
					D3DXMATRIX trans,intrans;
					matinview = *pBuffer->GetMatForBB();
					D3DXVECTOR3 vnowpoint;
					D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
					D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
					D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
							D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
							D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
							pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
							pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
					}
					pBuffer->SetNoAnim(true);

				}
				else
				{
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					pBuffer->SetNoAnim(false);
				}
				pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);
			}
			else
			{

				if (pBuffer->GetUseNormal())
				{
					D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						//POSITION BLEND
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						//NORMAL TRANSFORM
						D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
							pNmlBuffer[*listit] = pNmlBuffer[(*it)];
						}
					}
				}
				else
				{
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
						}
					}
				}
			}

		}

	}
}

void model::Mesh::ProcessAnimJoint(model::Buffer * pBuffer, const D3DXMATRIX * pWorldMatrix, const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix,model::BindPose * pBindPose, model::AnimJoint * pAnimJoint, DWORD dwAnimTimeElapse, float fAnimCustomSpeedRate,D3DXMATRIX& matrixscal)
{
	static D3DXVECTOR3	  vTrs, vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX	  matTemp, matTrs, matRot, matScl;
	D3DXVECTOR3 * pPosBuffer = pBuffer->GetPosBuffer();


	//ZeroMemory(pPosBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));
	//ZeroMemory(pNmlBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));

	D3DXMATRIX  mTrsTemp, mInvTrsTpsTemp;
	D3DXVECTOR3 vPos, vNml;

	const model::AnimJoint::tagJoint * pJointArray		 = pAnimJoint->GetJointArray();
	const D3DXMATRIX				 * pMatBindPoseArray = pBindPose->GetBindPoseArray();

	DWORD dwAnimTime	   = dwAnimTimeElapse;
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength(fAnimCustomSpeedRate);
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();

	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimTime * (dwAnimFrameCount - 1) / dwAnimTimeLength;
	if (dwFrameA >= dwAnimFrameCount - 1)
	{
		//不需要插值
		dwFrameA = dwAnimFrameCount - 1;

		for(DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
		{
			tagGroup & tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];

			//BINDPOSE MATRIX	
			const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//a --- b
			model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	

			D3DXMatrixTranslation(&matTrs, tFrameA.vTranslation.x, tFrameA.vTranslation.y, tFrameA.vTranslation.z);
			D3DXMatrixScaling(&matScl, tFrameA.vScale.x, tFrameA.vScale.y, tFrameA.vScale.z);
			if (fabs(tFrameA.vScale.x - 1.0f) > 0.0001||fabs(tFrameA.vScale.y - 1.0f) > 0.0001||fabs(tFrameA.vScale.z - 1.0f) > 0.0001)
			{
				pBuffer->SetNormalTransScal(true);
			}
			D3DXMatrixRotationQuaternion(&matRot, &tFrameA.qRot);

			D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
			D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);		

			D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);

			if (pBuffer->GetGpuCaculate())
			{
				if (pBuffer->GetIsBillBoard())
				{
					D3DXMATRIX matinview;
					D3DXMATRIX trans,intrans;
					matinview = *pBuffer->GetMatForBB();
					D3DXVECTOR3 vnowpoint;
					D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
					D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
					D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							D3DXMATRIX matinvtrs =  trans * matScl * matinview  * intrans;
							D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
							D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
							pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
							pBuffer->GetJointMatrixBufferForVertex()[0]  =  trans * matScl * matinview  * intrans;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[0] =  trans * matScl * matinview  * intrans;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] =  trans * matScl * matinview  * intrans;
					}
					pBuffer->SetNoAnim(true);

				}
				else
				{
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					pBuffer->SetNoAnim(false);
				}
				pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);
			}
			else
			{

				if (pBuffer->GetUseNormal())
				{
					D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						//POSITION BLEND
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						//NORMAL TRANSFORM
						D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
						//D3DXVec3Normalize(&pNmlBuffer[(*it)],&pNmlBuffer[(*it)]);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
							pNmlBuffer[*listit] = pNmlBuffer[(*it)];
						}
					}
				}
				else
				{
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
						}
					}
				}

			}


		}
	}else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get A time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
		//计算A B 之间 ratio
		DWORD dwTimeRelateA  = dwAnimTime - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount - 1);
		float fRatio		 = (float)dwTimeRelateA / fFrameInterval;
		//OutputConsole("%f\n",fRatio);

		for (DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
		{
			tagGroup & tGroup = m_pGroupArray[dwGroup];

			const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];
			//BINDPOSE MATRIX
			const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

			//interpolation	a --- b
			model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	
			model::AnimJoint::tagFrame & tFrameB = tJoint.pFrameArray[dwFrameB];   	
			//trans
			D3DXVec3Lerp(&vTrs, &tFrameA.vTranslation, &tFrameB.vTranslation, fRatio);
			//scale
			D3DXVec3Lerp(&vScl, &tFrameA.vScale, &tFrameB.vScale, fRatio);
			//rotation
			D3DXQuaternionSlerp(&qRot, &tFrameA.qRot, &tFrameB.qRot, fRatio);
			D3DXQuaternionNormalize(&qRot, &qRot);

			D3DXMatrixTranslation(&matTrs, vTrs.x, vTrs.y, vTrs.z);
			D3DXMatrixScaling(&matScl, vScl.x, vScl.y, vScl.z);
			if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
			{
				pBuffer->SetNormalTransScal(true);
			}
			D3DXMatrixRotationQuaternion(&matRot, &qRot);

			D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
			D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

			//MAKE DEFORMATION MATRIX
			D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
			D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);		

			D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
			D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);
			if (pBuffer->GetGpuCaculate())
			{
				if (pBuffer->GetIsBillBoard())
				{
					D3DXMATRIX matinview;
					D3DXMATRIX trans,intrans;
					matinview = *pBuffer->GetMatForBB();
					D3DXVECTOR3 vnowpoint;
					D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
					D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
					D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
							D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
							D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
							pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
							pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
					}
					pBuffer->SetNoAnim(true);

				}
				else
				{
					if (pBuffer->GetUseNormal())
					{

						if (pBuffer->GetNormalTransScal())
						{
							pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
						}
						else
						{
							pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
							pBuffer->SetMatView(pViewMatrix);
							pBuffer->SetMatProjective(pProjectedMatrix);
						}


					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					pBuffer->SetNoAnim(false);
				}
				pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);
			}
			else
			{

				if (pBuffer->GetUseNormal())
				{
					D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						//POSITION BLEND
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						//NORMAL TRANSFORM
						D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
							pNmlBuffer[*listit] = pNmlBuffer[(*it)];
						}
					}
				}
				else
				{
					list<WORD>::iterator it = tGroup.listeffectindex.begin();
					for (;it != tGroup.listeffectindex.end();it++)
					{
						D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
						std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
						std::list<WORD>::iterator listit = list.begin();
						for (;listit != list.end();listit++)
						{
							pPosBuffer[*listit] = pPosBuffer[(*it)];
						}
					}
				}

			}


		}
	}
	matrixscal = matScl;
}

//2009 11 30 nikai 自定义骨骼
void model::Mesh::ProcessAnimJointSelf(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
						  const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,
						  DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate,const D3DXVECTOR3 *pVec3Joint)
{
	static D3DXVECTOR3	  vTrs, vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX	  matTemp, matTrs, matRot, matScl;

	D3DXVECTOR3 * pPosBuffer = pBuffer->GetPosBuffer();
	/*ZeroMemory(pPosBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));*/

	D3DXMATRIX  mTrsTemp, mInvTrsTpsTemp;
	D3DXVECTOR3 vPos, vNml;

	const model::AnimJoint::tagJoint * pJointArray		 = pAnimJoint->GetJointArray();
	const D3DXMATRIX				 * pMatBindPoseArray = pBindPose->GetBindPoseArray();

	DWORD dwAnimTime	   = dwAnimTimeElapse;
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength(fAnimCustomSpeedRate);
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();

	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimTime * (dwAnimFrameCount - 1) / dwAnimTimeLength;
	DWORD dwFrameB = 0;
	if(dwFrameA > dwAnimFrameCount - 1)
	{
		dwFrameA = dwFrameB = dwAnimFrameCount - 1;
	}
	else
	{
		dwFrameB = dwFrameA + 1;
	}
	//get A time
	DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
	//计算A B 之间 ratio
	DWORD dwTimeRelateA  = dwAnimTime - dwTimeA;
	float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount - 1);
	float fRatio		 = (float)dwTimeRelateA / fFrameInterval;
	//OutputConsole("%f\n",fRatio);

	for (DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
	{
		tagGroup & tGroup = m_pGroupArray[dwGroup];

		const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];
		//BINDPOSE MATRIX
		const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//interpolation	a --- b
		model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	
		model::AnimJoint::tagFrame & tFrameB = tJoint.pFrameArray[dwFrameB];   	
		//trans
		//D3DXVec3Lerp(&vTrs, &tFrameA.vTranslation, &tFrameB.vTranslation, fRatio);
		vTrs.x = pVec3Joint[tGroup.dwJointIndex].y;
		vTrs.y = pVec3Joint[tGroup.dwJointIndex].z;
		vTrs.z = pVec3Joint[tGroup.dwJointIndex].x;
		//scale
		D3DXVec3Lerp(&vScl, &tFrameA.vScale, &tFrameB.vScale, fRatio);
		//rotation
		D3DXQuaternionSlerp(&qRot, &tFrameA.qRot, &tFrameB.qRot, fRatio);
		D3DXQuaternionNormalize(&qRot, &qRot);

		D3DXMatrixTranslation(&matTrs, vTrs.x, vTrs.y, vTrs.z);
		D3DXMatrixScaling(&matScl, vScl.x, vScl.y, vScl.z);
		if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
		{
			pBuffer->SetNormalTransScal(true);
		}
		D3DXMatrixRotationQuaternion(&matRot, &qRot);

		D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
		D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

		//MAKE DEFORMATION MATRIX
		D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
		D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
		D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);	

		D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);
		if (pBuffer->GetGpuCaculate())
		{
			if (pBuffer->GetIsBillBoard())
			{
				D3DXMATRIX matinview;
				D3DXMATRIX trans,intrans;
				matinview = *pBuffer->GetMatForBB();
				D3DXVECTOR3 vnowpoint;
				D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
				D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
				D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
						D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
						D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
						pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
						pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}


				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
				}
				pBuffer->SetNoAnim(true);

			}
			else
			{
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}


				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
				}
				pBuffer->SetNoAnim(false);
			}
			pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
			pBuffer->SetMatView(pViewMatrix);
			pBuffer->SetMatProjective(pProjectedMatrix);
		}
		else
		{

			if (pBuffer->GetUseNormal())
			{
				D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					//POSITION BLEND
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					//NORMAL TRANSFORM
					D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
						pNmlBuffer[*listit] = pNmlBuffer[(*it)];
					}
				}
			}
			else
			{
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
					}
				}
			}
		}

	}	
}

//2009 11 30 nikai 自定义骨骼
void model::Mesh::ProcessAnimJointSelf(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
						  const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,
						  DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel,const D3DXVECTOR3 *pVec3Joint)
{
	static D3DXVECTOR3	  vTrs, vScl;
	static D3DXQUATERNION qRot;
	static D3DXMATRIX	  matTemp, matTrs, matRot, matScl;

	D3DXVECTOR3 * pPosBuffer = pBuffer->GetPosBuffer();


	/*ZeroMemory(pPosBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));
	ZeroMemory(pNmlBuffer, m_dwPNCCount * sizeof(D3DXVECTOR3));*/

	D3DXMATRIX  mTrsTemp, mInvTrsTpsTemp;
	D3DXVECTOR3 vPos, vNml;

	const model::AnimJoint::tagJoint * pJointArray		 = pAnimJoint->GetJointArray();
	const D3DXMATRIX				 * pMatBindPoseArray = pBindPose->GetBindPoseArray();

	DWORD dwAnimTime	   = dwAnimTimeElapse;
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength(fAnimCustomSpeedRate);
	DWORD dwAnimFrameCount = pAnimJoint->GetFrameCount();

	//插值
	//A -+- B
	DWORD dwFrameA = dwAnimTime * (dwAnimFrameCount - 1) / dwAnimTimeLength;
	DWORD dwFrameB = 0;
	if(dwFrameA > dwAnimFrameCount - 1)
	{
		dwFrameA = dwFrameB = dwAnimFrameCount - 1;
	}
	else
	{
		dwFrameB = dwFrameA + 1;
	}
	
	
	//get A time
	DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);
	//计算A B 之间 ratio
	DWORD dwTimeRelateA  = dwAnimTime - dwTimeA;
	float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount - 1);
	float fRatio		 = (float)dwTimeRelateA / fFrameInterval;
	//OutputConsole("%f\n",fRatio);

	for (DWORD dwGroup = 0; dwGroup < m_dwGroupCount; ++dwGroup)
	{
		tagGroup & tGroup = m_pGroupArray[dwGroup];

		const model::AnimJoint::tagJoint & tJoint = pJointArray[tGroup.dwJointIndex];
		//BINDPOSE MATRIX
		const D3DXMATRIX * pMatBindPose = &pMatBindPoseArray[tGroup.dwJointIndex];

		//interpolation	a --- b
		model::AnimJoint::tagFrame & tFrameA = tJoint.pFrameArray[dwFrameA];   	
		model::AnimJoint::tagFrame & tFrameB = tJoint.pFrameArray[dwFrameB];   	
		//trans
		//D3DXVec3Lerp(&vTrs, &tFrameA.vTranslation, &tFrameB.vTranslation, fRatio);
		vTrs.x = pVec3Joint[tGroup.dwJointIndex].y;
		vTrs.y = pVec3Joint[tGroup.dwJointIndex].z;
		vTrs.z = pVec3Joint[tGroup.dwJointIndex].x;
		//scale
		D3DXVec3Lerp(&vScl, &tFrameA.vScale, &tFrameB.vScale, fRatio);
		//rotation
		D3DXQuaternionSlerp(&qRot, &tFrameA.qRot, &tFrameB.qRot, fRatio);
		D3DXQuaternionNormalize(&qRot, &qRot);

		D3DXMatrixTranslation(&matTrs, vTrs.x, vTrs.y, vTrs.z);
		D3DXMatrixScaling(&matScl, vScl.x, vScl.y, vScl.z);
		if (fabs(vScl.x - 1.0f) > 0.0001||fabs(vScl.y - 1.0f) > 0.0001||fabs(vScl.z - 1.0f) > 0.0001)
		{
			pBuffer->SetNormalTransScal(true);
		}
		D3DXMatrixRotationQuaternion(&matRot, &qRot);

		D3DXMatrixMultiply(&matTemp, &matScl, &matRot);
		D3DXMatrixMultiply(&matTemp, &matTemp, &matTrs);

		//MAKE DEFORMATION MATRIX
		D3DXMatrixInverse(&mTrsTemp, NULL, pMatBindPose);
		D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, &matTemp);
		matrixmodel = mTrsTemp;
		D3DXMatrixMultiply(&mTrsTemp, &mTrsTemp, pWorldMatrix);			
		matrixworld = mTrsTemp;

		D3DXMatrixInverse(&mInvTrsTpsTemp, NULL, &mTrsTemp);
		D3DXMatrixTranspose(&mInvTrsTpsTemp, &mInvTrsTpsTemp);
		if (pBuffer->GetGpuCaculate())
		{
			if (pBuffer->GetIsBillBoard())
			{
				D3DXMATRIX matinview;
				D3DXMATRIX trans,intrans;
				matinview = *pBuffer->GetMatForBB();
				D3DXVECTOR3 vnowpoint;
				D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
				D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,&mTrsTemp);
				D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						D3DXMATRIX matinvtrs = trans * matScl * matinview  * intrans;
						D3DXMatrixInverse(&matinvtrs,NULL,&matinvtrs);
						D3DXMatrixTranspose(&matinvtrs,&matinvtrs);
						pBuffer->GetJointMatrixBufferForNormals()[0] = matinvtrs;
						pBuffer->GetJointMatrixBufferForVertex()[0]  = trans * matScl * matinview  * intrans;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}


				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[0] = trans * matScl * matinview  * intrans;
				}
				pBuffer->SetNoAnim(true);

			}
			else
			{
				if (pBuffer->GetUseNormal())
				{

					if (pBuffer->GetNormalTransScal())
					{
						pBuffer->GetJointMatrixBufferForNormals()[dwGroup] = mInvTrsTpsTemp;
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
					}
					else
					{
						pBuffer->GetJointMatrixBufferForVertex()[dwGroup] =mTrsTemp;
						pBuffer->SetMatView(pViewMatrix);
						pBuffer->SetMatProjective(pProjectedMatrix);
					}


				}
				else
				{
					pBuffer->GetJointMatrixBufferForVertex()[dwGroup] = mTrsTemp;
				}
				pBuffer->SetNoAnim(false);
			}
			pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
			pBuffer->SetMatView(pViewMatrix);
			pBuffer->SetMatProjective(pProjectedMatrix);
		}
		else
		{

			if (pBuffer->GetUseNormal())
			{
				D3DXVECTOR3 * pNmlBuffer = pBuffer->GetNmlBuffer();
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					//POSITION BLEND
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					//NORMAL TRANSFORM
					D3DXVec3TransformNormal(&pNmlBuffer[(*it)],&m_pNmlBuffer[(*it)],&mInvTrsTpsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
						pNmlBuffer[*listit] = pNmlBuffer[(*it)];
					}
				}
			}
			else
			{
				list<WORD>::iterator it = tGroup.listeffectindex.begin();
				for (;it != tGroup.listeffectindex.end();it++)
				{
					D3DXVec3TransformCoord(&pPosBuffer[(*it)],&m_pPosBuffer[(*it)],&mTrsTemp);
					std::list<WORD>& list = tGroup.mapsourceeffectindex[*it];
					std::list<WORD>::iterator listit = list.begin();
					for (;listit != list.end();listit++)
					{
						pPosBuffer[*listit] = pPosBuffer[(*it)];
					}
				}
			}
		}
	}
}

void model::Mesh::CopyPosition(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix)
{
	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();
	D3DXVECTOR3 *pNmlBuffer = pBuffer->GetNmlBuffer();

	D3DXMATRIX  mInvTrsTpsTemp;
	D3DXMatrixInverse(&mInvTrsTpsTemp,NULL,pWorldMatrix);
	D3DXMatrixTranspose(&mInvTrsTpsTemp,&mInvTrsTpsTemp);
	pBuffer->SetNormalTransScal(false);
	if (pBuffer->GetGpuCaculate())
	{
		if (pBuffer->GetIsBillBoard())
		{
			D3DXMATRIX matinview;
			D3DXMATRIX trans,intrans;
			matinview = *pBuffer->GetMatForBB();
			D3DXVECTOR3 vnowpoint;
			D3DXMatrixTranslation(&trans,-m_vAveragepoint.x,-m_vAveragepoint.y,-m_vAveragepoint.z);
			D3DXVec3TransformCoord(&vnowpoint,&m_vAveragepoint,pWorldMatrix);
			D3DXMatrixTranslation(&intrans,vnowpoint.x,vnowpoint.y,vnowpoint.z);
			if (pBuffer->GetUseNormal())
			{
				pBuffer->GetJointMatrixBufferForVertex()[0] =trans *  matinview  * intrans;
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);

			}
			else
			{
				pBuffer->GetJointMatrixBufferForVertex()[0] =trans *  matinview  * intrans;
			}

		}
		else
		{
			if (pBuffer->GetUseNormal())
			{
				pBuffer->GetJointMatrixBufferForVertex()[0] = *pWorldMatrix;
				pBuffer->SetMatView(pViewMatrix);
				pBuffer->SetMatProjective(pProjectedMatrix);

			}
			else
			{
				pBuffer->GetJointMatrixBufferForVertex()[0] = *pWorldMatrix;
			}
		}
		pBuffer->SetMatViewProjective(&(*pViewMatrix  *  *pProjectedMatrix));
		pBuffer->SetMatView(pViewMatrix);
		pBuffer->SetMatProjective(pProjectedMatrix);
		pBuffer->SetNoAnim(true);
	}
	else
	{
		/*if (pBuffer->GetRendStatic())
		{
		if (!pBuffer->GetBufferCaculate())
		{
		memcpy(pPosBuffer,m_pPosBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
		if (pBuffer->GetUseNormal())
		{
		memcpy(pNmlBuffer,m_pNmlBuffer,m_dwPNCCount * sizeof(D3DXVECTOR3));
		}
		pBuffer->SetBufferCaculate(true);
		}

		pBuffer->SetMatWorld(pWorldMatrix);
		}
		else
		{*/
		if (pBuffer->GetUseNormal())
		{
			for(DWORD dwIndex = 0; dwIndex < m_dwPNCCount; dwIndex++)
			{
				D3DXVec3TransformCoord(&pPosBuffer[dwIndex],&m_pPosBuffer[dwIndex], pWorldMatrix);
				D3DXVec3TransformNormal(&pNmlBuffer[dwIndex], &m_pNmlBuffer[dwIndex], &mInvTrsTpsTemp);
			}
		}
		else
		{
			for(DWORD dwIndex = 0; dwIndex < m_dwPNCCount; dwIndex++)
			{
				D3DXVec3TransformCoord(&pPosBuffer[dwIndex],&m_pPosBuffer[dwIndex], pWorldMatrix);
			}
		}

		//}

	}
}


void model::Mesh::CopyUV(model::Buffer *pBuffer)
{
	if (pBuffer->GetUV0Buffer())
	{
		memcpy(pBuffer->GetUV0Buffer(),m_pUV0Buffer,sizeof(D3DXVECTOR2) * m_dwUV0Count);
	}
	//	memcpy(pBuffer->GetUV1Buffer(),m_pUV1Buffer,sizeof(D3DXVECTOR2) * m_dwUV1Count);
}

void model::Mesh::ProcessAnimVisibility(model::Buffer *pBuffer,const AnimVisibility *pAnimVisibility,DWORD dwTimeElapse,DWORD dwTimeCircle,bool blendalpha)
{
	//alpha动画
	//插值
	DWORD dwAnimFrameCount = pAnimVisibility->GetFrameCount();
	float *pFrameArray = pAnimVisibility->GetFrameArray();
	if (!pFrameArray)
	{
		return;
	}

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
		if (pBuffer->GetGpuCaculate())
		{
			DWORD& dwbasecolor = pBuffer->GetBaseColor();
			if (!blendalpha)
			{
				float fR = (float)( (dwbasecolor & 0x00FF0000) >> 16);
				float fG = (float)( (dwbasecolor & 0x0000FF00) >>  8);
				float fB = (float)(  dwbasecolor & 0x000000FF);

				DWORD c = (DWORD)((float)fR * fVisibilityA);  	
				dwbasecolor = (dwbasecolor & 0xFF000000) | (c << 16);
				c = (DWORD)( (float)fG * fVisibilityA);  	
				dwbasecolor |= (c << 8);
				c = (DWORD)( (float)fB * fVisibilityA);  	
				dwbasecolor |= c;
			}
			else
			{
				float fA = (float)( (dwbasecolor & 0xFF000000) >> 24);
				DWORD c = (DWORD)((float)fA * fVisibilityA); 
				dwbasecolor = (dwbasecolor & 0x00FFFFFF) | (c << 24);

			}
		}
		else
		{
			DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();
			if (!pColBuffer)
			{
				return;
			}

			for(DWORD n = 0; n < pBuffer->GetPNCCount(); n++)
			{
				DWORD &dwDestColor = pColBuffer[n];
				if (!blendalpha)
				{
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
				else
				{
					float fA = (float)( (dwDestColor & 0xFF000000) >> 24);
					DWORD c = (DWORD)((float)fA * fVisibilityA); 
					dwDestColor = (dwDestColor & 0x00FFFFFF) | (c << 24);

				}


			}

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

		if (pBuffer->GetGpuCaculate())
		{
			DWORD& dwbasecolor = pBuffer->GetBaseColor();
			if (!blendalpha)
			{
				float fR = (float)( (dwbasecolor & 0x00FF0000) >> 16);
				float fG = (float)( (dwbasecolor & 0x0000FF00) >>  8);
				float fB = (float)(  dwbasecolor & 0x000000FF);

				DWORD c = (DWORD)((float)fR * fVisibility);  	
				dwbasecolor = (dwbasecolor & 0xFF000000) | (c << 16);
				c = (DWORD)( (float)fG * fVisibility);  	
				dwbasecolor |= (c << 8);
				c = (DWORD)( (float)fB * fVisibility);  	
				dwbasecolor |= c;
			}
			else
			{
				float fA = (float)( (dwbasecolor & 0xFF000000) >> 24);
				DWORD c = (DWORD)((float)fA * fVisibility); 
				dwbasecolor = (dwbasecolor & 0x00FFFFFF) | (c << 24);

			}
		}
		else
		{
			DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();
			if (!pColBuffer)
			{
				return;
			}
			for(DWORD n = 0; n < pBuffer->GetPNCCount(); n++)
			{
				DWORD &dwDestColor = pColBuffer[n];

				if (!blendalpha)
				{
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
				else
				{
					float fA = (float)( (dwDestColor & 0xFF000000) >> 24);
					DWORD c = (DWORD)((float)fA * fVisibility); 
					dwDestColor = (dwDestColor & 0x00FFFFFF) | (c << 24);

				}
			}

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
			if (pBuffer->GetGpuCaculate())
			{
				DWORD& dwDestColor = pBuffer->GetBaseColor();

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
			else
			{
				DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();
				if (!pColBuffer)
				{
					return;
				}
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

			float fRA = (float)((dwFrameAColor & 0x00FF0000) >> 16);
			float fGA = (float)((dwFrameAColor & 0x0000FF00) >>  8);
			float fBA = (float)( dwFrameAColor & 0x000000FF);

			float fRB = (float)((dwFrameBColor & 0x00FF0000) >> 16);
			float fGB = (float)((dwFrameBColor & 0x0000FF00) >>  8);
			float fBB = (float)( dwFrameBColor & 0x000000FF);

			DWORD dwR = (DWORD)(fRA + (fRB - fRA) * fRatio);
			if (dwR > 255) dwR = 255;
			DWORD dwG = (DWORD)(fGA + (fGB - fGA) * fRatio);
			if (dwG > 255) dwG = 255;
			DWORD dwB = (DWORD)(fBA + (fBB - fBA) * fRatio);
			if (dwB > 255) dwB = 255;
			DWORD dwSrcColor = D3DCOLOR_ARGB(255,dwR,dwG,dwB);
			float fSR = (float)( (dwSrcColor & 0x00FF0000) >> 16) / 255.0f;
			float fSG = (float)( (dwSrcColor & 0x0000FF00) >>  8) / 255.0f;
			float fSB = (float)(  dwSrcColor & 0x000000FF)        / 255.0f;		
			if (pBuffer->GetGpuCaculate())
			{
				DWORD &dwDestColor = pBuffer->GetBaseColor();

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
			else
			{
				DWORD *pColBuffer = pBuffer->GetColBufferDiffuse();
				if (!pColBuffer)
				{
					return;
				}
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
}

void model::Mesh::ProcessAnimUV(model::Buffer *pBuffer,const AnimUV *pAnimUV,DWORD dwElapseTime,DWORD dwTimeCircle)
{
	//计算出当前的偏移
	float fRatio = (float)dwElapseTime / (float)dwTimeCircle;
	float fU = fRatio * pAnimUV->GetU();
	float fV = fRatio * pAnimUV->GetV();
	D3DXMATRIX trans,trans1,rot;
	int dir = pAnimUV->Getroatdir();
	if (pBuffer->GetGpuCaculate())
	{
		if (dir)
		{
			float fRatioRoat = timeGetTime()%pAnimUV->Getroatspeed()/(float)pAnimUV->Getroatspeed();
			D3DXMatrixTranslation(&trans,-0.5f,-0.5f,0.0f);
			D3DXMatrixInverse(&trans1,NULL,&trans);
			D3DXMatrixRotationZ(&rot,D3DX_PI * fRatioRoat * 2.0f * dir);
			trans =  trans * rot* trans1 ;
		}
		else
		{
			D3DXMatrixTranslation(&trans,fU,fV,0.0f);
		}
		pBuffer->SetMatForUse(&trans);
		pBuffer->SetUseUVAnim(true);

	}
	else
	{
		D3DXVECTOR2 *pUV0Buffer= pBuffer->GetUV0Buffer();
		if (!pUV0Buffer)
		{
			return;
		}
		if (dir)
		{
			float fRatioRoat = timeGetTime()%pAnimUV->Getroatspeed()/(float)pAnimUV->Getroatspeed();
			D3DXMatrixTranslation(&trans,-0.5f,-0.5f,0.0f);
			D3DXMatrixInverse(&trans1,NULL,&trans);
			D3DXMatrixRotationZ(&rot,D3DX_PI * fRatioRoat * 2.0f * dir);
			trans =  trans * rot* trans1 ;
		}
		for(DWORD n = 0 ; n < m_dwUV0Count; n++)
		{
			pUV0Buffer[n].x = m_pUV0Buffer[n].x + fU;		
			pUV0Buffer[n].y = m_pUV0Buffer[n].y + fV;
			if (dir)
			{
				D3DXVec2TransformCoord(&pUV0Buffer[n],&pUV0Buffer[n],&trans);
			}
		}

	}



	//	memcpy(pBuffer->GetUV1Buffer(),m_pUV1Buffer,sizeof(D3DXVECTOR2) * m_dwUV1Count);
}

void model::Mesh::ProcessEnvironmentMaps(model::Buffer *pBuffer,const D3DXMATRIX *pViewMatrix,bool bnormalize)
{
	static D3DXVECTOR3 vNormal;
	D3DXVECTOR3 *pNormalBuffer = pBuffer->GetNmlBuffer();
	D3DXVECTOR2 *pUVBuffer	   = pBuffer->GetUV0Buffer();
	if (!pNormalBuffer||!pUVBuffer)
	{
		return;
	}
	for(DWORD n = 0 ; n < m_dwPNCCount; n ++)
	{
		//tagTriangle *pTriangle = &m_pTriangleBuffer[n];

		//处理Normal
		//D3DXVec3TransformNormal(&vNormal[0],&pNormalBuffer[pTriangle->dwPNCIndex[0]],pViewMatrix);
		//D3DXVec3TransformNormal(&vNormal[1],&pNormalBuffer[pTriangle->dwPNCIndex[1]],pViewMatrix);
		//D3DXVec3TransformNormal(&vNormal[2],&pNormalBuffer[pTriangle->dwPNCIndex[2]],pViewMatrix);

		////处理UV
		//m_pUV0Buffer[pTriangle->dwUV0Index[0]].x = vNormal[0].x * 0.5f + 0.5f;
		//m_pUV0Buffer[pTriangle->dwUV0Index[0]].y = vNormal[0].y * 0.5f + 0.5f;
		//m_pUV0Buffer[pTriangle->dwUV0Index[1]].x = vNormal[1].x * 0.5f + 0.5f;
		//m_pUV0Buffer[pTriangle->dwUV0Index[1]].y = vNormal[1].y * 0.5f + 0.5f;
		//m_pUV0Buffer[pTriangle->dwUV0Index[2]].x = vNormal[2].x * 0.5f + 0.5f;
		//m_pUV0Buffer[pTriangle->dwUV0Index[2]].y = vNormal[2].y * 0.5f + 0.5f;
		D3DXVec3TransformNormal(&vNormal,&pNormalBuffer[n],pViewMatrix);
		if (bnormalize)
		{
			D3DXVec3Normalize(&vNormal,&vNormal);
		}
		pUVBuffer[n].x = vNormal.x * 0.5f + 0.5f;
		pUVBuffer[n].y = vNormal.y * 0.5f + 0.5f;

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
	if (pBuffer->GetGpuCaculate())
	{
		D3DXMATRIX trans;
		D3DXMatrixTranslation(&trans,startu,startv,0.0f);
		trans._11 = fScaleU;
		trans._22 = fScaleV;
		pBuffer->SetMatForUse(&trans);
		pBuffer->SetUseUVAnim(true);
	}

	D3DXVECTOR2 *pUV0Buffer= pBuffer->GetUV0Buffer();
	if (!pUV0Buffer)
	{
		return;
	}
	for(DWORD n = 0 ; n < m_dwUV0Count; n++)
	{
		pUV0Buffer[n].x = m_pUV0Buffer[n].x * fScaleU  + startu;		
		pUV0Buffer[n].y = m_pUV0Buffer[n].y * fScaleV  + startv;		
	}

	//	memcpy(pBuffer->GetUV1Buffer(),m_pUV1Buffer,sizeof(D3DXVECTOR2) * m_dwUV1Count);
}

void model::Mesh::CopyColor(model::Buffer *pBuffer)
{
	pBuffer->SetBaseColor(0xffffffff);
	pBuffer->SetMultiColor(0xffffffff);
	if (pBuffer->GetColBufferDiffuse())
	{
		memcpy(pBuffer->GetColBufferDiffuse(),m_pColBufferDiffuse,m_dwPNCCount * sizeof(DWORD));
	}

	//ZeroMemory(pBuffer->GetColBufferSpecular(),m_dwPNCCount * sizeof(DWORD));
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

BOOL model::AnimJoint::Create(CRFile *pFile)
{
	DWORD dwFlag,dwVersion;
	pFile->ReadData(&dwFlag,sizeof(DWORD));
	pFile->ReadData(&dwVersion,sizeof(DWORD));

	if (dwFlag != MAKEFOURCC('A','N','I','M'))
	{
		OutputError("error: type doesn't match! \"%s\"\n",pFile->GetFileName());
		ASSERT(0);
		return FALSE;
	}

	if (dwVersion != MAKEFOURCC('3','.','0','0'))
	{
		OutputError("error: version doesn't match! \"%s\"\n",pFile->GetFileName());
		ASSERT(0);
		return FALSE;
	}

	//读入帧数
	pFile->ReadData(&m_dwFrameCount,sizeof(DWORD));
	if (m_dwFrameCount <= 1)
	{
		OutputError("error: keyframe must > 1 \"%s\"\n",pFile->GetFileName());
		ASSERT(0);
		return FALSE;
	}
	//读入比例
	pFile->ReadData(&m_dwTimeLength,sizeof(DWORD));
	//读取骨骼数
	pFile->ReadData(&m_dwJointCount,sizeof(DWORD));

	m_pJointArray = new tagJoint[m_dwJointCount];

	for(DWORD n = 0;n < m_dwJointCount; n++)
	{
		tagJoint *pJoint = &m_pJointArray[n];
		//读入bindpose
		DWORD dwNameLen;
		pFile->ReadData(&dwNameLen,sizeof(DWORD));
		ZeroMemory(pJoint->szName,128);
		ASSERT(dwNameLen < 128);
		pFile->ReadData(pJoint->szName,dwNameLen);
		//读入帧

		pJoint->pFrameArray = new tagFrame[m_dwFrameCount];

		for(DWORD nFrame = 0; nFrame < m_dwFrameCount; nFrame++)
		{
			tagFrame &tFrame = pJoint->pFrameArray[nFrame];
			pFile->ReadData(&tFrame.vScale,sizeof(D3DXVECTOR3));
			pFile->ReadData(&tFrame.qRot,sizeof(D3DXQUATERNION));
			pFile->ReadData(&tFrame.vTranslation,sizeof(D3DXVECTOR3));
		}
	}
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

BOOL model::AnimVisibility::Create(CRFile* pFile)
{
	if(NULL == pFile)	return FALSE;

	utility::Text script;
	script.Create(pFile);	
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

BOOL model::AnimColor::Create(CRFile* pFile)
{
	if(NULL == pFile)	return FALSE;

	utility::Text script;
	script.Create(pFile);
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
	m_iroatdir = script.GetRootNode()->GetVar("roatdir") == NULL?0:script.GetRootNode()->GetVar("roatdir")->GetIntValue();
	m_iroatspeed = script.GetRootNode()->GetVar("roatspeed")== NULL?0:script.GetRootNode()->GetVar("roatspeed")->GetIntValue();

	script.Destroy();
	return TRUE;
}

BOOL model::AnimUV::Create(CRFile* pFile)
{
	if(NULL == pFile)	return FALSE;
	utility::Text script;

	script.Create(pFile);
	m_fUOffset = script.GetRootNode()->GetVar("u")->GetFloatValue();
	m_fVOffset = script.GetRootNode()->GetVar("v")->GetFloatValue();
	m_iroatdir = script.GetRootNode()->GetVar("roatdir") == NULL?0:script.GetRootNode()->GetVar("roatdir")->GetIntValue();
	m_iroatspeed = script.GetRootNode()->GetVar("roatspeed")== NULL?0:script.GetRootNode()->GetVar("roatspeed")->GetIntValue();

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

BOOL model::AnimTexture::Create(CRFile* pFile)
{
	if(NULL == pFile)	return FALSE;

	utility::Text script;
	script.Create(pFile);

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

BOOL model::BindPose::Create(CRFile* pFile)
{
	if(NULL == pFile)	return TRUE;

	DWORD dwFlag,dwVersion;
	pFile->ReadData(&dwFlag,sizeof(DWORD));
	pFile->ReadData(&dwVersion,sizeof(DWORD));

	if (dwFlag != MAKEFOURCC('B','I','N','D'))
	{
		OutputError("type doesn't match! \"%s\"\n",pFile->GetFileName());
		MessageBox(NULL,pFile->GetFileName(),"类型错误",MB_OK);
		return FALSE;
	}

	if (dwVersion != MAKEFOURCC('1','.','0','0'))
	{
		OutputError("version doesn't match! \"%s\"\n",pFile->GetFileName());
		MessageBox(NULL,pFile->GetFileName(),"版本错误",MB_OK);
		return FALSE;
	}

	pFile->ReadData(&m_dwJointCount,sizeof(int));
	m_pBindPoseArray = new D3DXMATRIX[m_dwJointCount];

	DWORD dwNameLen;
	char str[256];
	for(DWORD n = 0;n < m_dwJointCount; n++)
	{
		//读入
		pFile->ReadData(&m_pBindPoseArray[n],sizeof(D3DXMATRIX));
		pFile->ReadData(&dwNameLen,sizeof(DWORD));
		pFile->ReadData(str,dwNameLen);
		//file.SkipBytes(dwNameLen);

	}

	//OutputConsole("BindPose Joints: %d\n",m_dwJointCount);
	return TRUE;
}

void model::BindPose::SetFileName(const char *pszFileName)
{
	strncpy(m_strFileName,pszFileName,MAX_PATH);
}
void model::BindPose::Destroy()
{
	SAFEDELETEARRAY(m_pBindPoseArray);
}



/*
* 功能: 根据时间得到动画帧
* 摘要: -
* 参数: pAnimJoint			  - 动作关节
*		 dwJointIndex		  - 关节索引
*		 pTrs				  - 
*		 pScl				  - 
*		 pRot				  - 
*		 dwTimeElapse		  - 流逝时间
*		 fAnimCustomSpeedRate - 自定义的动画速率
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.03.27 - lpf
*		为该函数增加了一个fAnimCustomSpeedRate参数,用以处理用户自定义动画时间对得到的动画帧产生的影响
*/
void model::Mesh::GetAnimFrameByTime(const model::AnimJoint *pAnimJoint,DWORD dwJointIndex,D3DXVECTOR3 *pTrs,D3DXVECTOR3 *pScl,D3DXQUATERNION *pRot,DWORD dwTimeElapse,float fAnimCustomSpeedRate)
{
	const model::AnimJoint::tagJoint *pJointArray = pAnimJoint->GetJointArray();
	const model::AnimJoint::tagJoint &tJoint	  = pJointArray[dwJointIndex];
	DWORD dwAnimTimeLength = pAnimJoint->GetTimeLength(fAnimCustomSpeedRate);
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
	}else
	{
		DWORD dwFrameB = (dwFrameA + 1);

		//get KEYFRAME time
		DWORD dwTimeA =  dwFrameA * dwAnimTimeLength /  (dwAnimFrameCount - 1);

		//计算A B 之间 ratio
		DWORD dwTimeRelateA  = dwTimeElapse - dwTimeA;
		float fFrameInterval = (float)dwAnimTimeLength / (float)(dwAnimFrameCount -1) ;
		float fRatio		 = (float)dwTimeRelateA / fFrameInterval ; 

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