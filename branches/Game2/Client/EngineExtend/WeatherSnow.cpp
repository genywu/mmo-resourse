#include "stdafx.h"
#include ".\weathersnow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


WeatherSnow::WeatherSnow(void)
{
	m_ptSnow = NULL;
	m_bLiving = NULL;
}

WeatherSnow::~WeatherSnow(void)
{
	ASSERT(!m_ptSnow);
	ASSERT(!m_bLiving);
}

BOOL WeatherSnow::CreateParticles()
{
	m_ptSnow = new VERTEX[m_dwNum];
	m_bLiving = new BOOL[m_dwNum];

	VERTEX *pSnow;
	for(DWORD i = 0; i < m_dwNum; i++)
	{
		pSnow = &m_ptSnow[i];

		pSnow->color = m_dwColor;

		// 生成雪的位置
		pSnow->position.x = FieldRandom(m_vCenterPoint.x - m_fRadius, m_vCenterPoint.x + m_fRadius);
		pSnow->position.y = FieldRandom(m_fHeight, m_fHeight * 2.0f);
		pSnow->position.z = FieldRandom(m_vCenterPoint.z - m_fRadius, m_vCenterPoint.z + m_fRadius);

		// 随机雪的速度
		pSnow->speed.x = FieldRandom(m_vSpeed.x * 0.5f, m_vSpeed.x * 1.5f);
		pSnow->speed.y = FieldRandom(m_vSpeed.y * 0.5f, m_vSpeed.y * 1.5f);
		pSnow->speed.z = FieldRandom(m_vSpeed.z * 0.5f, m_vSpeed.z * 1.5f);

		m_bLiving[i] = FALSE;
	}

	m_pVertices = new VERTEX[m_dwNum * 4];

	return TRUE;
}

void WeatherSnow::ModifyParameters(TCHAR *config)
{
	utility::Text script;
	script.Create(config);

	utility::Text::Node *pRoot = script.GetRootNode()->GetChildNode(0);
	utility::Text::Variable *pVar;

	TCHAR path[128];
	pVar = pRoot->GetChildNode(0)->GetVar("file");
	sprintf(path, "%s", pVar->GetStringValue());
	CreateShader(path);

	// 区域中心点
	pVar = pRoot->GetChildNode(1)->GetVar("x");
	m_vCenterPoint.x = pVar->GetFloatValue();

	pVar = pRoot->GetChildNode(1)->GetVar("y");
	m_vCenterPoint.y = pVar->GetFloatValue();

	pVar = pRoot->GetChildNode(1)->GetVar("z");
	m_vCenterPoint.z = pVar->GetFloatValue();
	
	// 区域半径
	pVar = pRoot->GetVar("Radius");
	m_fRadius = pVar->GetFloatValue();
	
	// 粒子数量
	pVar = pRoot->GetVar("Number");
	m_dwNum = (DWORD)pVar->GetIntValue();

	// 起始高度
	pVar = pRoot->GetVar("Height");
	m_fHeight = pVar->GetFloatValue();
	
	// 死亡高度
	pVar = pRoot->GetVar("DeadHeight");
	m_fDeadHeight = pVar->GetFloatValue();
	
	// 粒子大小
	pVar = pRoot->GetVar("Size");
	m_fSize = pVar->GetFloatValue();

	// 下落速度
	pVar = pRoot->GetChildNode(2)->GetVar("x");
	m_vSpeed.x = pVar->GetFloatValue();

	pVar = pRoot->GetChildNode(2)->GetVar("y");
	m_vSpeed.y = pVar->GetFloatValue();

	pVar = pRoot->GetChildNode(2)->GetVar("z");
	m_vSpeed.z = pVar->GetFloatValue();
	
	// 颜色
	DWORD a, r, g, b;
	pVar = pRoot->GetChildNode(3)->GetVar("a");
	a = (DWORD)pVar->GetIntValue();

	pVar = pRoot->GetChildNode(3)->GetVar("r");
	r = (DWORD)pVar->GetIntValue();

	pVar = pRoot->GetChildNode(3)->GetVar("g");
	g = (DWORD)pVar->GetIntValue();

	pVar = pRoot->GetChildNode(3)->GetVar("b");
	b = (DWORD)pVar->GetIntValue();

	m_dwColor = D3DCOLOR_ARGB(a, r, g, b);

	// 声音
	pVar = pRoot->GetChildNode(4)->GetVar("file");
	const char *buf = pVar->GetStringValue();
	if(strcmp("null", buf) != 0)
	{
		m_szSoundFile = new TCHAR[128];
		memcpy(m_szSoundFile, buf, sizeof(TCHAR) * strlen(buf));
		m_szSoundFile[strlen(buf)]='\0';
	}

	script.Destroy();

	CreateParticles();
}

//---------------------------------------------------------------------------------
// Function name   : WeatherSnow::Destroy
// Description     : 销毁数据
// Return type     : VOID 
//---------------------------------------------------------------------------------
VOID WeatherSnow::Destroy()
{
	SAFEDELETEARRAY(m_ptSnow);
	SAFEDELETEARRAY(m_bLiving);

	Weather::Destroy();
}



/*
 * 功能: 渲染天气
 * 摘要: -
 * 参数: -
 * 返回值: VOID
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.10.08 - lpf
 *		修改了底层渲染调用;
 *	2008.10.17 - lpf
 *		增加了局部变量赋予初始值;
 *	2009.02.24 - lpf
 *		取消了D3D设备指针参数的传入;
 */
VOID WeatherSnow::Render()
{
	//pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

 //   // Set the render states for using point sprites
 //   pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
 //   pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
 //   pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(m_fSize) );
 //   pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
 //   pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
 //   pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
 //   pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

	//pd3dDevice->SetTexture( 0, m_pTexture );
	//pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX));
	//pd3dDevice->SetFVF(VERTEX::FVF);
	//pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_dwNum);

 //   pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
 //   pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	static VERTEX *pSnow = NULL;
	static VERTEX *pVertex = NULL;
	static D3DXMATRIX matWorld;

	static D3DXVECTOR3 vertex[20000];
	static D3DXVECTOR2 uv0[20000];
	static DWORD color[20000]/*,colorSpecular[20000] = {0}*/;
	static WORD index[60000];
	DWORD usenum = 0;

	for(DWORD i = 0; i < m_dwNum; i++,usenum++)
	{
		if(!m_bLiving[i])
		{
			--usenum;
			continue;
		}

		pSnow = &m_ptSnow[i];

		m_matBillboard._41 = pSnow->position.x;
		m_matBillboard._42 = pSnow->position.y;
		m_matBillboard._43 = pSnow->position.z;

		D3DXVec3TransformCoord(&m_pVertices[i*4+0].position, &m_pVertices[i*4+0].position, &m_matBillboard);
		D3DXVec3TransformCoord(&m_pVertices[i*4+1].position, &m_pVertices[i*4+1].position, &m_matBillboard);
		D3DXVec3TransformCoord(&m_pVertices[i*4+2].position, &m_pVertices[i*4+2].position, &m_matBillboard);
		D3DXVec3TransformCoord(&m_pVertices[i*4+3].position, &m_pVertices[i*4+3].position, &m_matBillboard);

		pVertex = &m_pVertices[i * 4 + 2];
		vertex[usenum * 6 + 0].x = pVertex->position.x;
		vertex[usenum * 6 + 0].y = pVertex->position.y;
		vertex[usenum * 6 + 0].z = pVertex->position.z;
		uv0[usenum * 6 + 0].x = pVertex->uv0.x;
		uv0[usenum * 6 + 0].y = pVertex->uv0.y;
		color[usenum * 6 + 0] = pVertex->color;
#pragma warning (push)
#pragma warning( disable:4244)
		index[usenum * 6 + 0] = usenum * 6 + 0;
#pragma warning(pop)
		pVertex = &m_pVertices[i * 4];
		vertex[usenum * 6 + 1].x = pVertex->position.x;
		vertex[usenum * 6 + 1].y = pVertex->position.y;
		vertex[usenum * 6 + 1].z = pVertex->position.z;
		uv0[usenum * 6 + 1].x = pVertex->uv0.x;
		uv0[usenum * 6 + 1].y = pVertex->uv0.y;
		color[usenum * 6 + 1] = pVertex->color;
#pragma warning (push)
#pragma warning( disable:4244)
		index[usenum * 6 + 1] = usenum * 6 + 1;
#pragma warning (pop)
		pVertex = &m_pVertices[i * 4 + 1];
		vertex[usenum * 6 + 2].x = pVertex->position.x;
		vertex[usenum * 6 + 2].y = pVertex->position.y;
		vertex[usenum * 6 + 2].z = pVertex->position.z;
		uv0[usenum * 6 + 2].x = pVertex->uv0.x;
		uv0[usenum * 6 + 2].y = pVertex->uv0.y;
		color[usenum * 6 + 2] = pVertex->color;
#pragma warning (push)
#pragma warning( disable:4244)
		index[usenum * 6 + 2] = usenum * 6 + 2;
#pragma warning(pop)
		pVertex = &m_pVertices[i * 4 + 2];
		vertex[usenum * 6 + 3].x = pVertex->position.x;
		vertex[usenum * 6 + 3].y = pVertex->position.y;
		vertex[usenum * 6 + 3].z = pVertex->position.z;
		uv0[usenum * 6 + 3].x = pVertex->uv0.x;
		uv0[usenum * 6 + 3].y = pVertex->uv0.y;
		color[usenum * 6 + 3] = pVertex->color;
#pragma warning (push)
#pragma warning( disable:4244)
		index[usenum * 6 + 3] = usenum * 6 + 3;
#pragma warning(pop)
		pVertex = &m_pVertices[i * 4 + 1];
		vertex[usenum * 6 + 4].x = pVertex->position.x;
		vertex[usenum * 6 + 4].y = pVertex->position.y;
		vertex[usenum * 6 + 4].z = pVertex->position.z;
		uv0[usenum * 6 + 4].x = pVertex->uv0.x;
		uv0[usenum * 6 + 4].y = pVertex->uv0.y;
		color[usenum * 6 + 4] = pVertex->color;
#pragma warning (push)
#pragma warning( disable:4244)
		index[usenum * 6 + 4] = usenum * 6 + 4;
#pragma warning(pop)
		pVertex = &m_pVertices[i * 4 + 3];
		vertex[usenum * 6 + 5].x = pVertex->position.x;
		vertex[usenum * 6 + 5].y = pVertex->position.y;
		vertex[usenum * 6 + 5].z = pVertex->position.z;
		uv0[usenum * 6 + 5].x = pVertex->uv0.x;
		uv0[usenum * 6 + 5].y = pVertex->uv0.y;
		color[usenum * 6 + 5] = pVertex->color;
#pragma warning (push)
#pragma warning( disable:4244)
		index[usenum * 6 + 5] = usenum * 6 + 5;
#pragma warning(pop)
		if (((usenum * 6 + 5)  > 19980))
		{
			pLayer3D->DrawTriangleArrayByShader(m_dwShaderGroupHandle, 0, usenum * 6, usenum * 6, vertex, NULL, uv0, color,  index,false, 1);
			usenum = -1;
		}
		
	}

	pLayer3D->DrawTriangleArrayByShader(m_dwShaderGroupHandle, 0, usenum * 6, usenum * 6, vertex, NULL, uv0, color,  index,false, 1);
}

//---------------------------------------------------------------------------------
// Function name   : WeatherSnow::Update
// Description     : 加入风速的影响。高度低于死亡高度时重生。
// Return type     : BOOL 
// Argument        : D3DXVECTOR3 &vWindForce         : 风速
//---------------------------------------------------------------------------------
BOOL WeatherSnow::Update(BOOL bEnable, D3DXVECTOR3 &vWindForce)
{
	//unsigned __int64 t0 = RDTSC();
	//if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
	//	return FALSE;

	VERTEX *pSnow = NULL;
	VERTEX *pVertex = NULL;
	DWORD dwIndex;
	for(DWORD i = 0; i < m_dwNum; i++)
	{
		pSnow = &m_ptSnow[i];

		// 粒子高度小于死亡高度时，重生；否则沿风向下降。
		if(pSnow->position.y < m_fDeadHeight)
		{
			m_bLiving[i] = FALSE;
			if(bEnable)
			{
				ReBirthParticle(pSnow);
				m_bLiving[i] = TRUE;
			}
		}
		else
		{
			pSnow->position.x += pSnow->speed.x;
			pSnow->position.y += pSnow->speed.y;
			pSnow->position.z += pSnow->speed.z;
			pSnow->position.x += vWindForce.x;
			pSnow->position.y += vWindForce.y;
			pSnow->position.z += vWindForce.z;
		}

		if(!m_bLiving[i])
			continue;

		dwIndex = i * 4;
		pVertex = &m_pVertices[dwIndex];
		pVertex->position.x = -m_fSize;
		pVertex->position.y = m_fSize;
		pVertex->position.z = 0;
		pVertex->uv0.x = 0;
		pVertex->uv0.y = 0;
		pVertex->color = m_dwColor;

		dwIndex++;
		pVertex = &m_pVertices[dwIndex];
		pVertex->position.x = m_fSize;
		pVertex->position.y = m_fSize;
		pVertex->position.z = 0;
		pVertex->uv0.x = 1;
		pVertex->uv0.y = 0;
		pVertex->color = m_dwColor;

		dwIndex++;
		pVertex = &m_pVertices[dwIndex];
		pVertex->position.x = -m_fSize;
		pVertex->position.y = -m_fSize;
		pVertex->position.z = 0;
		pVertex->uv0.x = 0;
		pVertex->uv0.y = 1;
		pVertex->color = m_dwColor;

		dwIndex++;
		pVertex = &m_pVertices[dwIndex];
		pVertex->position.x = m_fSize;
		pVertex->position.y = -m_fSize;
		pVertex->position.z = 0;
		pVertex->uv0.x = 1;
		pVertex->uv0.y = 1;
		pVertex->color = m_dwColor;
	}

	//m_pVB->Unlock();

	//unsigned __int64 t1 = RDTSC();
	//unsigned __int64 tb = t1 - t0;

	return TRUE;
}

//---------------------------------------------------------------------------------
// Function name   : WeatherSnow::ReBirthParticle
// Description     : 重生粒子
// Return type     : VOID 
// Argument        : VERTEX *pSnow
//---------------------------------------------------------------------------------
VOID WeatherSnow::ReBirthParticle(VERTEX *pSnow)
{
	// 生成雪的位置
	pSnow->position.x = FieldRandom(m_vCenterPoint.x - m_fRadius, m_vCenterPoint.x + m_fRadius);
	pSnow->position.y = FieldRandom(m_fHeight, m_fHeight * 2.0f);
	pSnow->position.z = FieldRandom(m_vCenterPoint.z - m_fRadius, m_vCenterPoint.z + m_fRadius);

	// 随机雪的速度
	pSnow->speed.x = FieldRandom(m_vSpeed.x * 0.5f, m_vSpeed.x * 1.5f);
	pSnow->speed.y = FieldRandom(m_vSpeed.y * 0.5f, m_vSpeed.y * 1.5f);
	pSnow->speed.z = FieldRandom(m_vSpeed.z * 0.5f, m_vSpeed.z * 1.5f);

	D3DXVec3TransformCoord(&pSnow->position, &pSnow->position, &m_matWorld);
}

void WeatherSnow::EnableLiving(BOOL bEnable)
{
	for(DWORD i = 0; i < m_dwNum; i++)
	{
		m_bLiving[i] = bEnable;
	}
}
