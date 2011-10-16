#include "stdafx.h"
#include ".\weathermanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


WeatherManager::WeatherManager(void)
{
	m_nSnowSndID=0;
	m_nRainSndID=0;
	m_nFogSndID=0;
		
	m_bSnowSndPlaying=FALSE;
	m_bRainSndPlaying=FALSE;
	m_bFogSndPlaying=FALSE;
}

WeatherManager::~WeatherManager(void)
{
	ASSERT(!m_pSnow);
	ASSERT(!m_pRain);
	ASSERT(!m_pFog);
	ASSERT(!m_pFlutter);
}



/*
 * 功能: 创建天气管理器
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: -
 * 修改日志:
 *	2008.06.26 - lpf
 *		增加了飞舞颗粒天气的创建;
 */
void WeatherManager::Create()
{
	m_bEnableSnow = m_bEnableRain = m_bEnableFog = m_bEnableFlutter = FALSE;

	m_pSnow = NULL;
	m_pRain = NULL;
	m_pFog = NULL;
	m_pFlutter = NULL;
}

VOID WeatherManager::ModifyFogParameter(TCHAR *config)
{
	SAFEDESTROY(m_pFog);
	m_pFog = new WeatherFog;
	m_pFog->ModifyParameters(config);

	m_bEnableFog = FALSE;
}

VOID WeatherManager::ModifyRainParameter(TCHAR *config)
{
	SAFEDESTROY(m_pRain);
	m_pRain = new WeatherRain;
	m_pRain->ModifyParameters(config);

	m_bEnableRain = FALSE;
}

VOID WeatherManager::ModifySnowParameter(TCHAR *config)
{
	SAFEDESTROY(m_pSnow);
	m_pSnow = new WeatherSnow;
	m_pSnow->ModifyParameters(config);

	m_bEnableSnow = FALSE;
}



/*
 * 功能: 修改飞舞天气参数
 * 摘要: -
 * 参数: config - 配置文件路径
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.06.26
 */
VOID WeatherManager::ModifyFlutterParameter(TCHAR * config)
{
	SAFEDESTROY(m_pFlutter);
	m_pFlutter = new WeatherFlutter;
	m_pFlutter->ModifyParameters(config);

	m_bEnableFlutter = FALSE;
}



/*
 * 功能: 销毁管理器
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: -
 *	2008.06.26 - lpf
 *		增加了飞舞颗粒天气的销毁;
 */
VOID WeatherManager::Destroy()
{
	SAFEDESTROY(m_pSnow);
	SAFEDESTROY(m_pRain);
	SAFEDESTROY(m_pFog);
	SAFEDESTROY(m_pFlutter);

	m_bEnableSnow = m_bEnableRain = m_bEnableFog = m_bEnableFlutter = FALSE;
}



/*
 * 功能: 天气渲染
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 *	2009.02.24 - lpf
 *		取消了D3D设备指针参数的传入;
 */
VOID WeatherManager::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHAREF,0x00);
	pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

	pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	if (m_pFog)
	{
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_pFog->Render();
	}

	if (m_pRain)
	{
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_pRain->Render();
	}

	if (m_pSnow)
	{
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_pSnow->Render();
	}

	if (m_pFlutter)
	{
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		m_pFlutter->Render();
	}

	//pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	//pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}



/*
 * 功能: 天气更新
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 *	2009.02.24 - lpf
 *		取消了D3D设备指针参数的传入;
 *	2010.03.15 - lpf
 *		取消了漂浮物天气效果的强制风力;
 */
VOID WeatherManager::Update()
{
	if (m_pFog)
		m_pFog->Update(m_bEnableFog);

	if (m_pRain)
		m_pRain->Update(m_bEnableRain);

	static FLOAT x, z;
	if (m_bEnableSnow)// || m_bEnableFlutter)
	{
		x = (FLOAT)rand() / (FLOAT)RAND_MAX - 0.5f;
		z = (FLOAT)rand() / (FLOAT)RAND_MAX - 0.5f;

		//char sz[20];
		//sprintf(sz, "x: %f", x);
		//OutputDebugStr(sz);
		//sprintf(sz, "z: %f\n", z);
		//OutputDebugStr(sz);
	}

	if (m_pSnow)
		m_pSnow->Update(m_bEnableSnow, D3DXVECTOR3(x * 0.01f, 0.0f, z * 0.01f));

	if (m_pFlutter)
		m_pFlutter->Update(m_bEnableFlutter, D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*D3DXVECTOR3(x * 0.01f, 0, z * 0.01f)*/);
}


VOID WeatherManager::EnableFog(BOOL bFlag)
{
	if(m_pFog)
	{
		m_bEnableFog = bFlag;
		if(m_bEnableFog)
			m_pFog->EnableLiving(TRUE);
	}
}

VOID WeatherManager::EnableRain(BOOL bFlag)
{
	if(m_pRain)
	{
		m_bEnableRain = bFlag;
		if(m_bEnableRain)
			m_pRain->EnableLiving(TRUE);
	}
}



VOID WeatherManager::EnableSnow(BOOL bFlag)
{
	if(m_pSnow)
	{
		m_bEnableSnow = bFlag;
		if(m_bEnableSnow)
			m_pSnow->EnableLiving(TRUE);
	}
}



/*
 * 功能: 是否激活飞舞颗粒天气
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.06.26
 */
VOID WeatherManager::EnableFlutter(BOOL bFlag)
{
	if(m_pFlutter)
	{
		m_bEnableFlutter = bFlag;
		if(m_bEnableFlutter)
			m_pFlutter->EnableLiving(true);
	}
}



//VOID WeatherManager::ChangeFogColor(DWORD dwColor, DWORD dwTime)
//{
//	if(m_pEnvFog)
//		m_pEnvFog->ChangeColor(dwColor, dwTime);
//}

VOID WeatherManager::SetWorldMatrix(D3DXMATRIX *matWorld)
{
	if(m_pRain)
		m_pRain->GetWorldMatrix(matWorld);

	if(m_pFog)
		m_pFog->GetWorldMatrix(matWorld);

	if(m_pSnow)
		m_pSnow->GetWorldMatrix(matWorld);

	if (m_pFlutter)
		m_pFlutter->GetWorldMatrix(matWorld);
}



/*
 * 功能: 设定公告牌矩阵
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 *	2008.12.10 - lpf
 *		增加了Y公告牌矩阵的赋值;
 */

VOID WeatherManager::SetBillboard(D3DXMATRIX *matBillboard, D3DXMATRIX *matBillboardY)
{
	if(m_pRain)
		m_pRain->GetBillboardMatrix(matBillboard, matBillboardY);

	if(m_pFog)
		m_pFog->GetBillboardMatrix(matBillboard, matBillboardY);

	if(m_pSnow)
		m_pSnow->GetBillboardMatrix(matBillboard, matBillboardY);

	if (m_pFlutter)
		m_pFlutter->GetBillboardMatrix(matBillboard, matBillboardY);
}