#include "stdafx.h"
#include "weatherenvfog.h"

#include "../Engine/utility.h"
#include "../Engine/render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


WeatherEnvFog::WeatherEnvFog(void)
{
	m_bEnableFog = TRUE;
	m_bUpdateFog = TRUE;
	m_bModify = FALSE;

	m_StartColor = D3DXCOLOR(0.5, 0.5, 0.5, 1);
	m_EndColor = D3DXCOLOR(0.5, 0.5, 0.5, 1);
	m_CurColor = D3DXCOLOR(0.5, 0.5, 0.5, 1);
	m_dwTime = 10000;
}

WeatherEnvFog::~WeatherEnvFog(void)
{
}

VOID WeatherEnvFog::ModifyEnvFogParameter(TCHAR *config)
{
	utility::Text script;
	script.Create(config);

	utility::Text::Node *pRoot = script.GetRootNode();
	utility::Text::Node *pEnvFogNode = pRoot->GetChildNode("EnvFog");
	utility::Text::Variable *pVar(NULL);

	utility::Text::Node *pColorNode = pEnvFogNode->GetChildNode("color");

	INT a, r, g, b;
	pVar = pColorNode->GetVar("a");
	a = pVar->GetIntValue();

	pVar = pColorNode->GetVar("r");
	r = pVar->GetIntValue();

	pVar = pColorNode->GetVar("g");
	g = pVar->GetIntValue();

	pVar = pColorNode->GetVar("b");
	b = pVar->GetIntValue();

	m_EndColor = D3DXCOLOR(D3DCOLOR_ARGB(a, r, g, b));
	m_StartColor = m_CurColor;
	m_bModify = TRUE;

	pVar = pEnvFogNode->GetVar("Start");
	m_fStart = pVar->GetFloatValue();

	pVar = pEnvFogNode->GetVar("End");
	m_fEnd = pVar->GetFloatValue();

	pVar = pEnvFogNode->GetVar("Density");
	m_fDensity = pVar->GetFloatValue();

	pVar = pEnvFogNode->GetVar("Mode");
	switch(pVar->GetIntValue())
	{
	case 0:
		m_dwMode = D3DFOG_EXP;
		break;
	case 1:
        m_dwMode = D3DFOG_EXP2;
		break;
	case 2:
		m_dwMode = D3DFOG_LINEAR;
		break;
	default:
		m_dwMode = D3DFOG_LINEAR;
		break;
	}
	utility::Text::Node *pSoundNode = pEnvFogNode->GetChildNode("sound");
	pVar = pSoundNode->GetVar("file");
	const char *buf = pVar->GetStringValue();
	if(strcmp("null", buf) != 0)
	{
		SAFEDELETE(m_szSoundFile);
		m_szSoundFile = new TCHAR[128];
		memcpy(m_szSoundFile, buf, sizeof(TCHAR) * strlen(buf));
		m_szSoundFile[strlen(buf)]='\0';
	}


	m_bUpdateFog = TRUE;

	script.Destroy();
}


//---------------------------------------------------------------------------------
// Function name   : WeatherEnvFog::EnableFog
// Description     : 打开/关闭雾效
// Return type     : VOID 
// Argument        : BOOL bFlag
//---------------------------------------------------------------------------------
VOID WeatherEnvFog::EnableFog(BOOL bFlag)
{
	m_bEnableFog = bFlag;

	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE, m_bEnableFog);
}

//---------------------------------------------------------------------------------
// Function name   : WeatherEnvFog::SetupPixelFog
// Description     : 设置雾效。渲染代码中调用。
// Return type     : VOID 
// Argument        : LPDIRECT3DDEVICE9 pd3dDevice
//---------------------------------------------------------------------------------
VOID WeatherEnvFog::SetupPixelFog(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if(!m_bEnableFog)
		return;

	// Set the fog color.
    pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, (DWORD)m_CurColor);
    
    // Set fog parameters.
	pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, m_dwMode);
    if( m_dwMode == D3DFOG_LINEAR )
    {
        pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&m_fStart));
        pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&m_fEnd));
    }
    else
    {
        pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&m_fDensity));
    }
}

void WeatherEnvFog::SetTime(DWORD dwTime)
{
	m_dwTime = dwTime * 1000;
}

void WeatherEnvFog::SetColor(D3DXCOLOR color)
{
	m_EndColor = color;
	m_bUpdateFog = TRUE;
}

//---------------------------------------------------------------------------------
// Function name   : WeatherEnvFog::Update
// Description     : 线性插值颜色，单位时间为毫秒。
// Return type     : VOID 
//---------------------------------------------------------------------------------
VOID WeatherEnvFog::Update()
{
	if(!m_bUpdateFog)
		return;

    static DWORD lastTime = 0;
	static DWORD dwCount = 0;

	if(m_bModify)
	{
		m_bModify = FALSE;
		lastTime = 0;
		dwCount = 0;
	}

	DWORD currentTime = timeGetTime();

	if(lastTime == 0)
		lastTime = currentTime;

	dwCount = currentTime - lastTime;

	FLOAT fRatio = (FLOAT)dwCount / m_dwTime;
	if(fRatio > 1.0) fRatio = 1.0;
	D3DXColorLerp(&m_CurColor, &m_StartColor, &m_EndColor, fRatio);

	if(dwCount >= m_dwTime)
	{
		m_bUpdateFog = FALSE;
		m_StartColor = m_CurColor;
		dwCount = 0;
		lastTime = 0;
	}
}

D3DCOLOR WeatherEnvFog::GetCurrentColor()
{
	return D3DCOLOR_COLORVALUE(m_CurColor.r, m_CurColor.g, m_CurColor.b, m_CurColor.a);
}

void WeatherEnvFog::SetEnvFogDistance(FLOAT start, FLOAT end)
{
	m_fStart = start;
	m_fEnd = end;
}
