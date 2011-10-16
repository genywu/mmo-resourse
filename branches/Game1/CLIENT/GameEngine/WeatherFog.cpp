#include "stdafx.h"
#include ".\weatherfog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


WeatherFog::WeatherFog(void)
{
	m_ptFog = NULL;
}

WeatherFog::~WeatherFog(void)
{
	ASSERT(!m_ptFog);
}

BOOL WeatherFog::CreateParticles()
{
	m_ptFog = new _tagFog[m_dwNum];

	_tagFog *pFog;
	for(DWORD i = 0; i < m_dwNum; i++)
	{
		pFog = &m_ptFog[i];

		// 在指定范围内随机位置
		pFog->position.x = FieldRandom(m_vCenterPoint.x - m_fRadius, m_vCenterPoint.x + m_fRadius);
		pFog->position.y = FieldRandom(m_fHeight, m_fHeight * 1.5f);
		pFog->position.z = FieldRandom(m_vCenterPoint.z - m_fRadius, m_vCenterPoint.z + m_fRadius);
		
		// 根据基准速度随机
		pFog->speed.x = FieldRandom(m_vSpeed.x * 0.8f, m_vSpeed.x * 1.2f);
		pFog->speed.y = FieldRandom(m_vSpeed.y * 0.8f, m_vSpeed.y * 1.2f);
		pFog->speed.z = FieldRandom(m_vSpeed.z * 0.8f, m_vSpeed.z * 1.2f);
		
		// 从基准尺寸随机
		pFog->size = FieldRandom(m_fSize * 0.8f, m_fSize * 1.2f);

		pFog->living = FALSE;
	}

	m_pVertices = new VERTEX[m_dwNum * 4];

	// 顺时间旋转45度
	D3DXMatrixRotationX(&m_matRotate, D3DX_PI/3.0);

	return TRUE;
}

void WeatherFog::ModifyParameters(TCHAR *config)
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

	// 声音
	pVar = pRoot->GetChildNode(4)->GetVar("file");
	const char *buf = pVar->GetStringValue();
	if(strcmp("null", buf) != 0)
	{
		m_szSoundFile = new TCHAR[128];
		memcpy(m_szSoundFile, buf, sizeof(TCHAR) * strlen(buf));
		m_szSoundFile[strlen(buf)]='\0';
	}

	m_dwColor = D3DCOLOR_ARGB(a, r, g, b);

	script.Destroy();


	CreateParticles();
}

//---------------------------------------------------------------------------------
// Function name   : WeatherFog::Destroy
// Description     : 销毁数据
// Return type     : VOID 
//---------------------------------------------------------------------------------
VOID WeatherFog::Destroy()
{
	SAFEDELETEARRAY(m_ptFog);

	Weather::Destroy();
}



/*
 * 功能: 渲染天气
 * 摘要: 逐面片渲染
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
VOID WeatherFog::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	static _tagFog *pFog = NULL;
	static VERTEX *pVertex = NULL;
	static D3DXMATRIX matWorld;

	static D3DXVECTOR3 vertex[20000];
	static D3DXVECTOR2 uv0[20000];
	static DWORD color[20000]/*,colorSpecular[20000] = {0}*/;
	static WORD index[60000];
	DWORD usenum = 0;
	for(DWORD i = 0; i < m_dwNum; i++,usenum++)
	{
		pFog = &m_ptFog[i];

		if(!pFog->living)
		{
			--usenum;
			continue;
		}

		m_matBillboard._41 = pFog->position.x;
		m_matBillboard._42 = pFog->position.y;
		m_matBillboard._43 = pFog->position.z;

		// 使面片成为billboard，并向上旋转45度。
		D3DXMatrixMultiply(&matWorld, &m_matRotate, &m_matBillboard);

		D3DXVec3TransformCoord(&m_pVertices[i*4+0].position, &m_pVertices[i*4+0].position, &matWorld);
		D3DXVec3TransformCoord(&m_pVertices[i*4+1].position, &m_pVertices[i*4+1].position, &matWorld);
		D3DXVec3TransformCoord(&m_pVertices[i*4+2].position, &m_pVertices[i*4+2].position, &matWorld);
		D3DXVec3TransformCoord(&m_pVertices[i*4+3].position, &m_pVertices[i*4+3].position, &matWorld);

		pVertex = &m_pVertices[i * 4 + 2];
		vertex[usenum * 6 + 0].x = pVertex->position.x;
		vertex[usenum * 6 + 0].y = pVertex->position.y;
		vertex[usenum * 6 + 0].z = pVertex->position.z;
		uv0[usenum * 6 + 0].x = pVertex->uv0.x;
		uv0[usenum * 6 + 0].y = pVertex->uv0.y;
		color[usenum * 6 + 0] = pVertex->color;
		index[usenum * 6 + 0] = usenum * 6 + 0;

		pVertex = &m_pVertices[i * 4];
		vertex[usenum * 6 + 1].x = pVertex->position.x;
		vertex[usenum * 6 + 1].y = pVertex->position.y;
		vertex[usenum * 6 + 1].z = pVertex->position.z;
		uv0[usenum * 6 + 1].x = pVertex->uv0.x;
		uv0[usenum * 6 + 1].y = pVertex->uv0.y;
		color[usenum * 6 + 1] = pVertex->color;
		index[usenum * 6 + 1] = usenum * 6 + 1;

		pVertex = &m_pVertices[i * 4 + 1];
		vertex[usenum * 6 + 2].x = pVertex->position.x;
		vertex[usenum * 6 + 2].y = pVertex->position.y;
		vertex[usenum * 6 + 2].z = pVertex->position.z;
		uv0[usenum * 6 + 2].x = pVertex->uv0.x;
		uv0[usenum * 6 + 2].y = pVertex->uv0.y;
		color[usenum * 6 + 2] = pVertex->color;
		index[usenum * 6 + 2] = usenum * 6 + 2;

		pVertex = &m_pVertices[i * 4 + 2];
		vertex[usenum * 6 + 3].x = pVertex->position.x;
		vertex[usenum * 6 + 3].y = pVertex->position.y;
		vertex[usenum * 6 + 3].z = pVertex->position.z;
		uv0[usenum * 6 + 3].x = pVertex->uv0.x;
		uv0[usenum * 6 + 3].y = pVertex->uv0.y;
		color[usenum * 6 + 3] = pVertex->color;
		index[usenum * 6 + 3] = usenum * 6 + 3;

		pVertex = &m_pVertices[i * 4 + 1];
		vertex[usenum * 6 + 4].x = pVertex->position.x;
		vertex[usenum * 6 + 4].y = pVertex->position.y;
		vertex[usenum * 6 + 4].z = pVertex->position.z;
		uv0[usenum * 6 + 4].x = pVertex->uv0.x;
		uv0[usenum * 6 + 4].y = pVertex->uv0.y;
		color[usenum * 6 + 4] = pVertex->color;
		index[usenum * 6 + 4] = usenum * 6 + 4;

		pVertex = &m_pVertices[i * 4 + 3];
		vertex[usenum * 6 + 5].x = pVertex->position.x;
		vertex[usenum * 6 + 5].y = pVertex->position.y;
		vertex[usenum * 6 + 5].z = pVertex->position.z;
		uv0[usenum * 6 + 5].x = pVertex->uv0.x;
		uv0[usenum * 6 + 5].y = pVertex->uv0.y;
		color[usenum * 6 + 5] = pVertex->color;
		index[usenum * 6 + 5] = usenum * 6 + 5;

		if (((usenum * 6 + 5)  > 19980))
		{
			pLayer3D->DrawTriangleArrayByShader(m_dwShaderGroupHandle, 0, usenum * 6, usenum * 6, vertex, NULL, uv0, color,  index,false, 1);
			usenum = -1;
		}
		
	}

	pLayer3D->DrawTriangleArrayByShader(m_dwShaderGroupHandle, 0, usenum * 6, usenum * 6, vertex, NULL, uv0, color,  index,false, 1);
}



//---------------------------------------------------------------------------------
// Function name   : WeatherFog::Update
// Description     : 沿速度方面移动。高度低于死亡高度时重生。
// Return type     : BOOL 
// Argument        : -
//---------------------------------------------------------------------------------
BOOL WeatherFog::Update(BOOL bEnable)
{
	_tagFog *pFog = NULL;

	VERTEX *pVertex = NULL;
	//if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
	//	return FALSE;

	DWORD dwIndex = 0;
	for(DWORD i = 0; i < m_dwNum; i++)
	{
		pFog = &m_ptFog[i];
		if(pFog->position.y < m_fDeadHeight)
		{
			pFog->living = FALSE;
			if(bEnable)
				ReBirthParticle(pFog);
		}
		else
		{
			pFog->position.x += pFog->speed.x;
			pFog->position.y += pFog->speed.y;
			pFog->position.z += pFog->speed.z;
		}

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

	return TRUE;
}

//---------------------------------------------------------------------------------
// Function name   : WeatherFog::ReBirthParticle
// Description     : 重生粒子
// Return type     : VOID 
// Argument        : _tagFog *pFog
//---------------------------------------------------------------------------------
VOID WeatherFog::ReBirthParticle(_tagFog *pFog)
{
	pFog->position.x = FieldRandom(m_vCenterPoint.x - m_fRadius, m_vCenterPoint.x + m_fRadius);
	pFog->position.y = FieldRandom(m_fHeight, m_fHeight * 2.0f);
	pFog->position.z = FieldRandom(m_vCenterPoint.z - m_fRadius, m_vCenterPoint.z + m_fRadius);
	pFog->speed.x = FieldRandom(m_vSpeed.x * 0.8f, m_vSpeed.x * 1.2f);
	pFog->speed.y = FieldRandom(m_vSpeed.y * 0.8f, m_vSpeed.y * 1.2f);
	pFog->speed.z = FieldRandom(m_vSpeed.z * 0.8f, m_vSpeed.z * 1.2f);
	pFog->size = FieldRandom(m_fSize * 0.8f, m_fSize * 1.2f);
	pFog->living = TRUE;

	D3DXVec3TransformCoord(&pFog->position, &pFog->position, &m_matWorld);
}

void WeatherFog::EnableLiving(BOOL bEnable)
{
	for(DWORD i = 0; i < m_dwNum; i++)
	{
		m_ptFog[i].living = bEnable;
	}
}
