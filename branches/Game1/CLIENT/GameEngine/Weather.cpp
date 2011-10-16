#include "stdafx.h"
#include "weather.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


Weather::Weather(void)
{
	m_pVB = NULL; 
	m_pTexture = NULL;
	m_pVertices = NULL;
	m_szSoundFile = NULL;
	m_dwShaderGroupHandle = -1;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matBillboard);
	D3DXMatrixIdentity(&m_matBillboardY);
	D3DXMatrixIdentity(&m_matRotate);
}

Weather::~Weather(void)
{
	ASSERT(!m_pVB);
	ASSERT(!m_pTexture);
	ASSERT(!m_pVertices);
	ASSERT(!m_szSoundFile);
}

BOOL Weather::CreateParticles()
{
	return TRUE;
}

VOID Weather::Destroy()
{
	SAFEDELETEARRAY(m_szSoundFile);
	SAFEDELETEARRAY(m_pVertices);
	SAFERELEASE(m_pVB);
	SAFERELEASE(m_pTexture);

	if(m_dwShaderGroupHandle != -1)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
		pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
	}
}


//---------------------------------------------------------------------------------
// Function name   : Weather::CreateTexture
// Description     : 从文件创建纹理
// Return type     : BOOL 
// Argument        : LPDIRECT3DDEVICE9 pd3dDevice
// Argument        : TCHAR *pFilename
//---------------------------------------------------------------------------------
BOOL Weather::CreateTexture(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR *pFilename)
{
    // Use D3DX to create a texture from a file based image
    if( FAILED( D3DXCreateTextureFromFileEx(pd3dDevice,pFilename,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,D3DFMT_FROM_FILE,D3DPOOL_MANAGED,D3DX_FILTER_LINEAR,D3DX_FILTER_LINEAR,0,NULL,NULL,&m_pTexture)))
    {
		MessageBox(NULL, "Could not find texture!", "Error:", MB_OK);
		return FALSE;
    }

	return TRUE;
}

//---------------------------------------------------------------------------------
// Function name   : Weather::CreateShader
// Description     : 创建ShaderHandle
// Return type     : DWORD                : 返回创建的ShaderHandle
// Argument        : TCHAR* pzFilename    : Shader 文件名
//---------------------------------------------------------------------------------
DWORD Weather::CreateShader(TCHAR* pszFilename)
{
	utility::File fileShader;	
	if (!fileShader.Open(pszFilename))
	{
		OutputConsole("error: open \"%s\" failed.\n",pszFilename);
		return 0;
	}

	char *pszShaderText = new char [fileShader.GetLength() + 1];
	memcpy(pszShaderText,fileShader.GetData(),fileShader.GetLength());
	pszShaderText[fileShader.GetLength()] = 0;	
	fileShader.Close();

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();

	//get shader group handle
	char szPath[_MAX_PATH];
	utility::File::GetFilePath(pszFilename,szPath);

	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pszShaderText,szPath);

	SAFEDELETE(pszShaderText);

	return m_dwShaderGroupHandle;
}

VOID Weather::Render()
{
}

BOOL Weather::Update(D3DXVECTOR3 &vWindForce)
{
	return TRUE;
}

VOID Weather::ReBirthParticle(VERTEX *pVertex)
{
}

//---------------------------------------------------------------------------------
// Function name   : Weather::FieldRandom
// Description     : 从指定范围内随机选择一个数
// Return type     : FLOAT 
// Argument        : FLOAT fStart
// Argument        : FLOAT fEnd
//---------------------------------------------------------------------------------
FLOAT Weather::FieldRandom(FLOAT fStart, FLOAT fEnd)
{
	FLOAT fTemp = (FLOAT)rand() / (FLOAT)RAND_MAX;
	return (fStart + (fEnd - fStart) * fTemp);
}


DWORD Weather::FieldRandom(DWORD dwStart, DWORD dwEnd)
{
	float fTemp = (float)rand() / (float)RAND_MAX;
	return (dwStart + (dwEnd - dwStart) * fTemp);
}