#include "stdafx.h"
#include "utility.h"
#include "console.h"
#include "render.h"
#include "water.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SINGLETON(Water::CManager);
bool Water::CManager::s_bIsRenderWater = true;

/*
 * 功能: 开始水面渲染
 * 摘要: 进行相关渲染状态的设定
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 *	2009.11.24 - lpf
 *		增加当前雾效的激活状态参数,以设定当前水面渲染时的雾效
 */
void Water::CManager::BeginRender(BOOL bFogEnable)
{
	LPDIRECT3DDEVICE9 pDevice = render::Interface::GetInstance()->GetDevice();

	pDevice->SetRenderState(D3DRS_ZENABLE, true);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_LIGHTING, false);
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, false);

	pDevice->SetRenderState(D3DRS_FOGENABLE, bFogEnable);

	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BUMPENVMAPLUMINANCE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	static const float f = 1.0f;
	pDevice->SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET, *((DWORD *)&f));
}



/*
 * 功能: 结束水面渲染
 * 摘要: 恢复祥光渲染状态
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
void Water::CManager::EndRender()
{
	render::Interface * pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	pInterface->SetTexture(0, NULL);
	pInterface->SetTexture(1, NULL);

	pDevice->SetRenderState(D3DRS_LIGHTING, true);
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
	pDevice->SetRenderState(D3DRS_FOGENABLE, false);

	pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	static const float f = 0.0f;
	pDevice->SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET, *((DWORD *)&f));
}



/*
 * 功能: 销毁
 * 摘要: 进行所有水面资源的销毁
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
void Water::CManager::Destroy()
{
	m_listRenderWaters.clear();

	for (m_itWater = m_listAllWaters.begin(); m_itWater != m_listAllWaters.end(); ++m_itWater)
		SAFE_DELETE((*m_itWater));
	m_listAllWaters.clear();
}



/*
 * 功能: 添加水面
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
void Water::CManager::AddWater(CWater * pWater)
{
	m_listAllWaters.push_back(pWater);
	pWater->m_lID = (long)m_listAllWaters.size();
}



/*
 * 功能: 添加渲染水面
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
void Water::CManager::AddRenderWater(_tagRenderElement & Element)
{
	if (!s_bIsRenderWater)
		return;

	m_listRenderWaters.push_back(Element);
}



/*
 * 功能: 移除水面
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
void Water::CManager::RemoveWater(CWater * pWater)
{
	m_itWater = find(m_listAllWaters.begin(), m_listAllWaters.end(), pWater);
	if (m_itWater != m_listAllWaters.end())
		m_listAllWaters.erase(m_itWater);
}



/*
 * 功能: 渲染
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 *	2009.11.24 - lpf
 *		增加当前雾效的激活状态参数,以设定当前水面渲染时的雾效
 */
void Water::CManager::Render(BOOL bFogEnable)
{
	if (!s_bIsRenderWater)
		return;

	BeginRender(bFogEnable);

	for (m_itRenderWaters = m_listRenderWaters.begin(); m_itRenderWaters != m_listRenderWaters.end(); ++m_itRenderWaters)
		(*m_itRenderWaters).pWater->Render(*m_itRenderWaters);
	m_listRenderWaters.clear();

	EndRender();
}



/*
 * 功能: 构造函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
Water::CWater::CWater()
{
	m_lID = 0;
	ZeroMemory(m_szWaterFileName, MAX_PATH);
	ZeroMemory(m_szShaderFileName, MAX_PATH);

	m_eType = WaterType_Rect;
	m_lNumVertex = 4;
	m_fRadius = 1.0f;
	m_fWidth = 20.0f;		//宽度
	m_fLength = 20.0f;		//高度
	m_dwpColor = NULL;

	// 法线纹理层参数
	m_fNorSpeedX = 0.0f;
	m_fNorSpeedY = 0.0001f;
	m_fNormat00 = 0.005f;
	m_fNormat01 = 0.0f;
	m_fNormat10 = 0.0f;
	m_fNormat11 = 0.005f;

	// 环境纹理层参数
	m_fEnvSpeedX = 0.0f;
	m_fEnvSpeedY = 0.0001f;

	m_dwShaderGroupHandle = 0xffffffff;
}



/*
 * 功能: 析构函数
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
Water::CWater::~CWater()
{
	SAFEDELETEARRAY(m_dwpColor);

	render::Interface * pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D * pLayer3D = pInterface->GetLayer3D();

	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
}



/*
 * 功能: 渲染水面
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
void Water::CWater::Render(_tagRenderElement & Element)
{
	render::Interface * pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	render::Interface::Layer3D * pLayer3D = pInterface->GetLayer3D();
	Water::CWater * pWater = Element.pWater;

	static D3DXVECTOR3 s_vPos[WATERVERTEX_MAX];
	static D3DCOLOR s_dwDif[WATERVERTEX_MAX];
	static D3DXVECTOR3 s_vNor[WATERVERTEX_MAX];
	static D3DXVECTOR2 s_vUV[WATERVERTEX_MAX * 2];
	static D3DXVECTOR3 s_vNor_Ori[4] = { D3DXVECTOR3(0.0f, 1.0f, 0.0f),
										 D3DXVECTOR3(0.0f, 1.0f, 0.0f),
										 D3DXVECTOR3(0.0f, 1.0f, 0.0f),
										 D3DXVECTOR3(0.0f, 1.0f, 0.0f) };
	static D3DXVECTOR2 s_vUV_Ori[8] = { D3DXVECTOR2(0.0f, 1.0f),
										D3DXVECTOR2(0.0f, 1.0f),
										D3DXVECTOR2(0.0f, 0.0f),
										D3DXVECTOR2(0.0f, 0.0f),
										D3DXVECTOR2(1.0f, 1.0f),
										D3DXVECTOR2(1.0f, 1.0f),
										D3DXVECTOR2(1.0f, 0.0f),
										D3DXVECTOR2(1.0f, 0.0f) };

	D3DXVECTOR2 vTmpNor((float)Element.dwElapseTime * 0.01f * pWater->m_fNorSpeedX, (float)Element.dwElapseTime * 0.01f * pWater->m_fNorSpeedY);
	D3DXVECTOR2 vTmpEnv((float)Element.dwElapseTime * 0.01f * pWater->m_fEnvSpeedX, (float)Element.dwElapseTime * 0.01f * pWater->m_fEnvSpeedY);

	pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, *((DWORD *)&pWater->m_fNormat00));
	pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, *((DWORD *)&pWater->m_fNormat01));
	pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, *((DWORD *)&pWater->m_fNormat10));
	pDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, *((DWORD *)&pWater->m_fNormat11));

	render::Interface::Layer3D::ShaderGroup * pShaderGroup = pLayer3D->GetShaderGroup(m_dwShaderGroupHandle);
	if (!pShaderGroup)
		return;

	render::Interface::Layer3D::ShaderGroup::_tagShader * pShader = pShaderGroup->GetShader(0);
	if (!pShader)
		return;

	pInterface->SetTexture(0, pShaderGroup->GetTextureHandle(0, pShader->tLayer[0].dwTexture));
	pInterface->SetTexture(1, pShaderGroup->GetTextureHandle(1, pShader->tLayer[1].dwTexture));

	if (m_eType == WaterType_Rect)
	{
		float fHalfW = pWater->m_fWidth * 0.5f;
		float fHalfL = pWater->m_fLength * 0.5f;

		// 1------3
		// |＼    |
		// |　＼  |
		// |　　＼|
		// 0 -----2
		s_vPos[0].x =
		s_vPos[1].x = Element.vPos.x - fHalfW;
		s_vPos[2].x =
		s_vPos[3].x = Element.vPos.x + fHalfW;

		s_vPos[0].z =
		s_vPos[2].z	= Element.vPos.z - fHalfL;
		s_vPos[1].z =
		s_vPos[3].z	= Element.vPos.z + fHalfL;

		s_vPos[0].y =
		s_vPos[1].y =
		s_vPos[2].y =
		s_vPos[3].y = Element.vPos.y;

		s_dwDif[0] = m_dwpColor[0];
		s_dwDif[1] = m_dwpColor[1];
		s_dwDif[2] = m_dwpColor[2];
		s_dwDif[3] = m_dwpColor[3];

		memcpy(s_vUV, s_vUV_Ori, sizeof(D3DXVECTOR2) * 8);
		s_vUV[0].x += vTmpNor.x;
		s_vUV[0].y += vTmpNor.y;
		s_vUV[2].x += vTmpNor.x;
		s_vUV[2].y += vTmpNor.y;
		s_vUV[4].x += vTmpNor.x;
		s_vUV[4].y += vTmpNor.y;
		s_vUV[6].x += vTmpNor.x;
		s_vUV[6].y += vTmpNor.y;

		s_vUV[1].x += vTmpEnv.x;
		s_vUV[1].y += vTmpEnv.y;
		s_vUV[3].x += vTmpEnv.x;
		s_vUV[3].y += vTmpEnv.y;
		s_vUV[5].x += vTmpEnv.x;
		s_vUV[5].y += vTmpEnv.y;
		s_vUV[7].x += vTmpEnv.x;
		s_vUV[7].y += vTmpEnv.y;

		pLayer3D->DrawTriangleStrips(2, s_vPos, s_vNor_Ori, s_dwDif, s_vUV);
	}else if (m_eType == WaterType_Round)
	{
		s_vPos[0]  = Element.vPos;
		s_dwDif[0] = m_dwpColor[0];
		s_vNor[0]  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		s_vUV[0]   = vTmpNor;
		s_vUV[1]   = vTmpEnv;

		float fAverageRadian = (D3DX_PI * 2.0f) / (float)(m_lNumVertex - 2);
		float fAngle		 = D3DXToDegree(fAverageRadian);
		float fCos;
		float fSin;

		for (long l = 1; l < m_lNumVertex; ++l)
		{
			fCos = cos(fAverageRadian * l);
			fSin = sin(fAverageRadian * l);

			s_vPos[l].x = -m_fRadius * fCos + Element.vPos.x;
			s_vPos[l].y = Element.vPos.y;
			s_vPos[l].z = m_fRadius * fSin + Element.vPos.z;
			s_dwDif[l]	= m_dwpColor[l];
			s_vNor[l]	= s_vNor[0];

			s_vUV[l * 2].x	   = fCos + vTmpNor.x;
			s_vUV[l * 2].y	   = fSin + vTmpNor.y;
			s_vUV[l * 2 + 1].x = fCos + vTmpEnv.x;
			s_vUV[l * 2 + 1].y = fSin + vTmpEnv.y;
		}

		pLayer3D->DrawTriangleFan(m_lNumVertex, s_vPos, s_vNor_Ori, s_dwDif, s_vUV);
	}
}



/*
 * 功能: 创建水面
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
bool Water::CWater::Create(CRFile * pFile)
{
	char szTemp[_MAX_PATH];

	if (pFile == NULL)
	{
		Log4c::Warn(ROOT_MODULE, "Error! Water file %s is not found!", m_szWaterFileName);
		return false;
	}

	utility::Text Script;
	utility::Text::Node * pNode = NULL;
	utility::Text::Node * pTempNode = NULL;
	utility::Text::Variable * pTempVar = NULL;

	Script.Create(pFile);

	// 读取基本属性
	pNode = Script.GetRootNode()->GetChildNode("baseproperty");
	if (pNode == NULL)
	{
		Log4c::Warn(ROOT_MODULE, "Error! Water file %s script is not have \"baseproperty\" node!", m_szWaterFileName);
		Script.Destroy();
		return false;
	}

	pTempVar = pNode->GetVar("type");
	if (pTempVar != NULL)
	{
		if (stricmp(pTempVar->GetStringValue(), "RECT") == 0)
			m_eType = WaterType_Rect;
		else if (stricmp(pTempVar->GetStringValue(), "ROUND") == 0)
			m_eType = WaterType_Round;
		else
			m_eType = WaterType_Rect;
	}

	if (m_eType == WaterType_Rect)
		m_lNumVertex = 4;
	else if (m_eType == WaterType_Round)
	{
		pTempVar = pNode->GetVar("vertexnum");
		m_lNumVertex = pTempVar ? pTempVar->GetIntValue() : 4;
		m_lNumVertex += 2;

		if (m_lNumVertex <= 4)
		{
			Log4c::Trace(ROOT_MODULE,szTemp);
			Script.Destroy();
			return false;
		}else if (m_lNumVertex > WATERVERTEX_MAX)
			m_lNumVertex = WATERVERTEX_MAX;
	}

	pTempVar = pNode->GetVar("width");
	m_fWidth = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	pTempVar = pNode->GetVar("length");
	m_fLength = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	m_fRadius = max(m_fWidth, m_fLength) * 0.5f;

	// 读取颜色配置
	D3DXCOLOR Color(0.0f, 0.0f, 0.0f, 1.0f);
	LPD3DXCOLOR d3dpColor = new D3DXCOLOR[m_lNumVertex];
	ZeroMemory(d3dpColor, sizeof(D3DXCOLOR) * m_lNumVertex);
	pTempNode = pNode->GetChildNode("color");
	if (pTempNode != NULL)
	{
		pTempVar = pTempNode->GetVar("r");
		Color.r = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
		pTempVar = pTempNode->GetVar("g");
		Color.g = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
		pTempVar = pTempNode->GetVar("b");
		Color.b = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	}

	for (long l = 0; l < m_lNumVertex; ++l)
		d3dpColor[l] = Color;

	pTempNode = pNode->GetChildNode("alpha");
	if (pTempNode != NULL)
	{
		for (DWORD dw = 0; dw < pTempNode->GetVarCount(); ++dw)
		{
			pTempVar = pTempNode->GetVar(dw);
			d3dpColor[dw].a = pTempVar ? pTempVar->GetFloatValue() : 1.0f;
		}
	}

	m_dwpColor = new D3DCOLOR[m_lNumVertex];
	for (long l = 0; l < m_lNumVertex; ++l)
		m_dwpColor[l] = d3dpColor[l];

	SAFEDELETEARRAY(d3dpColor);

	pNode = Script.GetRootNode()->GetChildNode("shader");
	_snprintf(m_szShaderFileName, _MAX_PATH, "%s", pNode->GetVar("file")->GetStringValue());

	// 读取法线纹理层
	pNode = Script.GetRootNode()->GetChildNode("normallayer");
	if (pNode == NULL)
	{
		Log4c::Error(ROOT_MODULE,"%-15s Error! Water file %s script is not have \"normallayer\" node!",__FUNCTION__, m_szWaterFileName);
		Script.Destroy();
		return false;
	}
	pTempVar = pNode->GetVar("speedx");
	m_fNorSpeedX = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	pTempVar = pNode->GetVar("speedy");
	m_fNorSpeedY = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	pTempVar = pNode->GetVar("normat00");
	m_fNormat00 = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	pTempVar = pNode->GetVar("normat01");
	m_fNormat01 = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	pTempVar = pNode->GetVar("normat10");
	m_fNormat10 = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	pTempVar = pNode->GetVar("normat11");
	m_fNormat11 = pTempVar ? pTempVar->GetFloatValue() : 0.0f;

	// 读取环境纹理层
	pNode = Script.GetRootNode()->GetChildNode("environmentlayer");
	if (pNode == NULL)
	{
		Log4c::Error(ROOT_MODULE,"%-15s Error! Water file %s script is not have \"environmentlayer\" node!",__FUNCTION__, m_szWaterFileName);
		Script.Destroy();
		return false;
	}
	pTempVar = pNode->GetVar("speedx");
	m_fEnvSpeedX = pTempVar ? pTempVar->GetFloatValue() : 0.0f;
	pTempVar = pNode->GetVar("speedy");
	m_fEnvSpeedY = pTempVar ? pTempVar->GetFloatValue() : 0.0f;

	Script.Destroy();

	return true;
}



/*
 * 功能: 创建着色变量组
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2009.09.02
 * 修改日志:
 */
bool Water::CWater::CreateShaderGroup(CRFile * pFile, DWORD dwTextureProcision)
{
	char szTemp[_MAX_PATH];

	if (pFile == NULL)
	{
		_snprintf(szTemp, _MAX_PATH, "Error! Water file %s script is not have Shader file!", m_szWaterFileName);
		return false;
	}

	m_dwShaderGroupHandle = render::Interface::GetInstance()->GetLayer3D()->CreateShaderGroup(pFile, dwTextureProcision);

	return true;
}