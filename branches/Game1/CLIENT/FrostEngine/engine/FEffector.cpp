#include "stdafx.h"
#include "feffector.h"
#include "futility.h"
#include "fconsole.h"


//CLASS__(effector::Effector)___
effector::Effector::Effector(void):
m_eType(EFF_UNKNOWN),
m_dwCycleTime(1000),
m_dwShaderGroupHandle(NULL)
{
}

effector::Effector::~Effector(void)
{

}

BOOL effector::Effector::ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier)
{
	return TRUE;
}

BOOL effector::Effector::Create(const char *pszFileName)
{
	utility::MemFile fileShader;	
	if (!fileShader.Open(pszFileName))
	{
		OutputConsole("error: Effect::Create(\"%s\")\n",pszFileName);
		ASSERT(0);
		return FALSE;
	}

	char *pszShaderText = new char [fileShader.GetLength() + 1];
	memcpy(pszShaderText,fileShader.GetData(),fileShader.GetLength());
	pszShaderText[fileShader.GetLength()] = 0;	
	fileShader.Close();

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();

	//get shader group handle
	char szPath[_MAX_PATH];
	utility::GetFilePath(pszFileName,szPath);

	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pszShaderText,szPath);
	ASSERT(m_dwShaderGroupHandle != 0xffffffff);

	SAFEDELETE(pszShaderText);

	return TRUE;
}

void effector::Effector::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
}

//CLASS__(effector::ColorLerp)___
effector::ColorLerp::ColorLerp()
{
	m_eType = EFF_COLORLERP;
	m_tColorStart = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	m_tColorEnd = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
}

effector::ColorLerp::~ColorLerp()
{
}

void effector::ColorLerp::SetParameter(const D3DXCOLOR *pColorStart,const D3DXCOLOR *pColorEnd)
{
	m_tColorStart = *pColorStart;
	m_tColorEnd  = *pColorEnd;
}

//取得插值颜色
D3DXCOLOR * effector::ColorLerp::CalculateColorLerp(D3DXCOLOR *pOut,float fRatio, BOOL bLerpDirect)
{
	if(bLerpDirect)
	{
		D3DXColorLerp(pOut, &m_tColorStart, &m_tColorEnd, fRatio);
	}
	else
	{
		D3DXColorLerp(pOut, &m_tColorEnd, &m_tColorStart, fRatio);
	}

	return pOut;
}

BOOL effector::ColorLerp::ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier)
{
	BOOL bResult = FALSE;
	//根据开始，结束时间设定帧
	DWORD dwTimeElapse = dwCurrentTime - dwStartTime;
	DWORD dwTimeCycle  = (DWORD)((float)m_dwCycleTime * fTimeModifier);

	DWORD dwHalfTimeElapse = dwTimeElapse  /2;
	DWORD dwHalfTimeCycle = dwTimeCycle / 2;

	float fRatio;

	if (dwTimeElapse > dwTimeCycle)
	{
		//循环一次
		bResult = TRUE;
	}

	D3DCOLOR dwColor ;

	if (!dwHalfTimeElapse || !dwHalfTimeElapse)
	{
		dwColor = m_tColorStart;
	}
	else
	{
		fRatio = (float)dwHalfTimeElapse / (float)dwHalfTimeCycle;
		D3DXCOLOR dxColor;
		CalculateColorLerp(&dxColor,fRatio, (dwTimeElapse > dwHalfTimeCycle));
		dwColor = (DWORD)dxColor;
	}

	D3DCOLOR * pColBuffer = pBuffer->GetColBuffer();
	DWORD dwColCount = pBuffer->GetColCount();

	_asm
	{
		mov edi,[pColBuffer]
		mov eax,dwColor
		cld 
		mov ecx,dwColCount
		rep stosd
	}

	return bResult;
}


//CLASS__(UVOffset)___
effector::UVOffset::UVOffset()
{
	m_eType = EFF_UVOFFSET;
	m_vOffset.x =0;
	m_vOffset.y =0;
}

effector::UVOffset::~UVOffset()
{
}

void effector::UVOffset::SetParameter(const D3DXVECTOR2 *pOffset)
{
	m_vOffset = *pOffset;
}

BOOL effector::UVOffset::ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier)
{
	BOOL bResult = FALSE;
	//根据开始，结束时间设定帧
	float fTimeElapse = (float)(dwCurrentTime - dwStartTime);
	float fTimeCycle  = (float)(m_dwCycleTime) * fTimeModifier;

	if (fTimeElapse > fTimeCycle)
	{
		//循环一次
		bResult = TRUE;
	}

	D3DXVECTOR2 vec2UVOffset = m_vOffset * (fTimeElapse / fTimeCycle);

	D3DXVECTOR2* pUV0Buffer = pBuffer->GetUV0Buffer();
	DWORD dwUV0Count = pBuffer->GetUV0Count();
	for(DWORD dwUV0 = 0; dwUV0 < dwUV0Count; dwUV0++)
	{
		pUV0Buffer[dwUV0] += vec2UVOffset;
	}
	
	return bResult;
}

//CLASS__(ShadowEffector)____
effector::FlatShadow::FlatShadow()
{
	m_eType = EFF_FLATSHADOW;
}

effector::FlatShadow::~FlatShadow()
{
}

void effector::FlatShadow::SetParameter(const D3DXVECTOR3 *pLightVector,DWORD dwLightType,D3DCOLOR dwColor)
{
	m_dwLightType = dwLightType;
	m_vLightVector = *pLightVector;
	m_dwColor = dwColor;	
}

BOOL effector::FlatShadow::ProcessVertices(model::Buffer* pBuffer,DWORD dwStartTime,DWORD dwCurrentTime,float fTimeModifier)
{
	BOOL bResult = FALSE;

	D3DXMATRIX matShadow,matOffsetY;
	D3DXPLANE plane;

	D3DXVECTOR4 vLightParam(m_vLightVector.x,m_vLightVector.y,m_vLightVector.z,(float)m_dwLightType);
	D3DXPlaneFromPoints(&plane,&D3DXVECTOR3(0,0,0),&D3DXVECTOR3(1,0,0),&D3DXVECTOR3(0,0,1));

	D3DXMatrixShadow(&matShadow,&vLightParam,&plane);
	D3DXMatrixTranslation(&matOffsetY,0.0,0.01f,0.0f);

	D3DXMatrixMultiply(&matShadow,&matShadow,&matOffsetY);

	D3DXVECTOR3 *pPosBuffer = pBuffer->GetPosBuffer();

	D3DCOLOR * pColBuffer = pBuffer->GetColBuffer();
	DWORD dwColCount = pBuffer->GetColCount();
	DWORD dwColor = m_dwColor;

	_asm
	{
		mov edi,[pColBuffer]
		mov eax,dwColor
		cld 
		mov ecx,dwColCount
		rep stosd
	}
	
	//D3DXVec3TransformCoordArray(pPosBuffer,sizeof(D3DXVECTOR3),pPosBuffer,sizeof(D3DXVECTOR3),&matShadow);
	for(DWORD dwPos = 0; dwPos < pBuffer->GetPosCount(); dwPos++)
	{
		D3DXVec3TransformCoord(&pPosBuffer[dwPos],&pPosBuffer[dwPos],&matShadow);

	}

	return TRUE;
}

