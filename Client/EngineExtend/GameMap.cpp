#include "stdafx.h"

#include "../Engine/console.h"
#include "../Engine/utility.h"
#include "../Engine/math.h"

#include "DisplayModel.h"
#include "gamemap.h"
#include "gamebuildlist.h"
#include "gameselectray.h"
#include "gamemodelmanager.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 纹理编码与UV的对应表
static const float CodeUVTable[32][8] = 
{
	// v0            v1              v2              v3
	// 右边16块纹理块
	{0.002f, 0.002f, 0.123f, 0.002f, 0.123f, 0.248f, 0.002f, 0.248f},
	{0.127f, 0.002f, 0.248f, 0.002f, 0.248f, 0.248f, 0.127f, 0.248f},
	{0.252f, 0.002f, 0.373f, 0.002f, 0.373f, 0.248f, 0.252f, 0.248f},
	{0.377f, 0.002f, 0.498f, 0.002f, 0.498f, 0.248f, 0.377f, 0.248f},

	{0.002f, 0.252f, 0.123f, 0.252f, 0.123f, 0.498f, 0.002f, 0.498f},
	{0.127f, 0.252f, 0.248f, 0.252f, 0.248f, 0.498f, 0.127f, 0.498f},
	{0.252f, 0.252f, 0.373f, 0.252f, 0.373f, 0.498f, 0.252f, 0.498f},
	{0.377f, 0.252f, 0.498f, 0.252f, 0.498f, 0.498f, 0.377f, 0.498f},

	{0.002f, 0.502f, 0.123f, 0.502f, 0.123f, 0.748f, 0.002f, 0.748f},
	{0.127f, 0.502f, 0.248f, 0.502f, 0.248f, 0.748f, 0.127f, 0.748f},
	{0.252f, 0.502f, 0.373f, 0.502f, 0.373f, 0.748f, 0.252f, 0.748f},
	{0.377f, 0.502f, 0.498f, 0.502f, 0.498f, 0.748f, 0.377f, 0.748f},

	{0.002f, 0.752f, 0.123f, 0.752f, 0.123f, 0.998f, 0.002f, 0.998f},
	{0.127f, 0.752f, 0.248f, 0.752f, 0.248f, 0.998f, 0.127f, 0.998f},
	{0.252f, 0.752f, 0.373f, 0.752f, 0.373f, 0.998f, 0.252f, 0.998f},
	{0.377f, 0.752f, 0.498f, 0.752f, 0.498f, 0.998f, 0.377f, 0.998f},

	// 用于替换的纹理块UV
	//// 左边2块代码为1111的纹理块
	//{0.002f, 0.002f, 0.123f, 0.002f, 0.123f, 0.248f, 0.002f, 0.248f},
	//{0.377f, 0.752f, 0.498f, 0.752f, 0.498f, 0.998f, 0.377f, 0.998f},

	// 右边16块纹理块
	{0.502f, 0.002f, 0.623f, 0.002f, 0.623f, 0.248f, 0.502f, 0.248f},
	{0.627f, 0.002f, 0.748f, 0.002f, 0.748f, 0.248f, 0.627f, 0.248f},
	{0.752f, 0.002f, 0.873f, 0.002f, 0.873f, 0.248f, 0.752f, 0.248f},
	{0.877f, 0.002f, 0.998f, 0.002f, 0.998f, 0.248f, 0.877f, 0.248f},

	{0.502f, 0.252f, 0.623f, 0.252f, 0.623f, 0.498f, 0.502f, 0.498f},
	{0.627f, 0.252f, 0.748f, 0.252f, 0.748f, 0.498f, 0.627f, 0.498f},
	{0.752f, 0.252f, 0.873f, 0.252f, 0.873f, 0.498f, 0.752f, 0.498f},
	{0.877f, 0.252f, 0.998f, 0.252f, 0.998f, 0.498f, 0.877f, 0.498f},

	{0.502f, 0.502f, 0.623f, 0.502f, 0.623f, 0.748f, 0.502f, 0.748f},
	{0.627f, 0.502f, 0.748f, 0.502f, 0.748f, 0.748f, 0.627f, 0.748f},
	{0.752f, 0.502f, 0.873f, 0.502f, 0.873f, 0.748f, 0.752f, 0.748f},
	{0.877f, 0.502f, 0.998f, 0.502f, 0.998f, 0.748f, 0.877f, 0.748f},

	{0.502f, 0.752f, 0.623f, 0.752f, 0.623f, 0.998f, 0.502f, 0.998f},
	{0.627f, 0.752f, 0.748f, 0.752f, 0.748f, 0.998f, 0.627f, 0.998f},
	{0.752f, 0.752f, 0.873f, 0.752f, 0.873f, 0.998f, 0.752f, 0.998f},
	{0.877f, 0.752f, 0.998f, 0.752f, 0.998f, 0.998f, 0.877f, 0.998f},
};


//CLASS___(GameMap)______
GameMap::GameMap(void)
{
	m_bAsyncLoadBuild = false;

	m_dwCurFlagValue = 0;
	m_dwGridVisibleIndexCount = 0;
	m_dwBuildGridVisibleIndexCount = 0;

	m_pGridArray = NULL;
	m_pBuildList = NULL;
	m_pGridVisibleFlagArray = NULL;
	m_pBuildGridVisibleFlagArray = NULL;
	m_pLightSourceArray = NULL;

	m_pVertexPosArray = NULL;
	m_pVertexNormalArray = NULL;
	m_pVertexColArray = NULL;
	m_pGridNormalArray = NULL;
	m_dwVertexCount = 0;
	m_dwGridCount = 0;
	m_fVersion = 0;
	m_dwWidth = 0;
	m_dwDepth = 0;

	m_dwTargetHideGridIndex = 0xFFFFFFFF;

	m_pFrustum = NULL;

	//m_pGameFootprint = NULL;
	m_bFogEnable = FALSE;
	m_dwFogColor = 0xFF44BBF3;
	m_fFogStart = 22.5f;
	m_fFogEnd = 45.0f;
	ZeroMemory( &m_material, sizeof(D3DMATERIAL9) );
	m_material.Diffuse.r =  1.0f;
	m_material.Diffuse.g =  1.0f;
	m_material.Diffuse.b =  1.0f;
	m_material.Diffuse.a =  1.0f;
	m_material.Ambient.r =  1.0f;
	m_material.Ambient.g =  1.0f;
	m_material.Ambient.b =  1.0f;
	m_material.Ambient.a =  1.0f;
	m_material.Specular.r = 1.0f;
	m_material.Specular.g = 1.0f;
	m_material.Specular.b = 1.0f;
	m_material.Specular.a = 1.0f;
	m_material.Power = 10.0f;
	m_material.Emissive.r = 0.0f;
	m_material.Emissive.g = 0.0f;
	m_material.Emissive.b = 0.0f;
	m_material.Emissive.a = 1.0f;

	ZeroMemory(m_lFootEffectIdArray, sizeof(long) * 6);	
}

GameMap::~GameMap(void)
{
}



/*
* 功能: 创建地图
* 摘要: -
* 参数: szPath - 地图文件路径
*		dwTextureProcision - 保留
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.09.26 - lpf
*		增加了"读取footeffect文件"的代码
*	2009.01.15 - lpf
*		读取足底特效文件处,增加了新的材质读取(雪地和沼泽)
*	2009.10.20 - lpf
*		在创建全局动画对象时,增加了全局动画时间的设定
*/
BOOL GameMap::Create(const char *szPath,DWORD dwTextureProcision)
{
	////OutputError("----读取 Map :%s\n",pszFileName);
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	///////////////////////////////////////////////////////////////////////////////
	//读取config文件
	char szTempName[1024];
	_snprintf(szTempName,1023,"%s\\config",szPath);
	utility::File file;
	if (!file.Open(szTempName))
	{
		OutputError("open \"%s\" error\n",szTempName);
		MessageBox(NULL,szTempName,"open map file faild!",MB_OK);
		return FALSE;
	}
	utility::Text text;
	text.Create(&file);
	file.Close();

	utility::Text::Node *pRoot = text.GetRootNode();

	m_fVersion = pRoot->GetVar("Version")->GetFloatValue();
	m_dwWidth = (DWORD)pRoot->GetVar("Width")->GetIntValue();
	m_dwDepth = (DWORD)pRoot->GetVar("Depth")->GetIntValue();
	m_dwGridCount = m_dwWidth * m_dwDepth;
	m_dwTerrainAmbientColor = pRoot->GetVar("TerrainAmbientColor")?pRoot->GetVar("TerrainAmbientColor")->GetHexValue():0xffffffff;
	m_dwSunLightAmbientColorforBuild = pRoot->GetVar("SunLightAmbientColor")?pRoot->GetVar("SunLightAmbientColor")->GetHexValue():0xffffffff;
	m_dwSunLightAmbientColorforMoveShap = pRoot->GetVar("SunLightAmbientColorforMoveshap")?pRoot->GetVar("SunLightAmbientColorforMoveshap")->GetHexValue():0xff707070;
	m_dwGridColorcoefficientforMoveShapR  = pRoot->GetVar("GridColorcoefficientforMoveShapR")?pRoot->GetVar("GridColorcoefficientforMoveShapR")->GetIntValue():0;
	m_dwGridColorcoefficientforMoveShapG  = pRoot->GetVar("GridColorcoefficientforMoveShapG")?pRoot->GetVar("GridColorcoefficientforMoveShapG")->GetIntValue():0;
	m_dwGridColorcoefficientforMoveShapB  = pRoot->GetVar("GridColorcoefficientforMoveShapB")?pRoot->GetVar("GridColorcoefficientforMoveShapB")->GetIntValue():0;
	m_dwSunLightDiffuseColor = pRoot->GetVar("SunLightDiffuseColor")->GetHexValue();
	m_dwSunLightSpecularColor = pRoot->GetVar("SunLightSpecularColor")?pRoot->GetVar("SunLightSpecularColor")->GetHexValue():0;
	m_vSunLightDirection.x  = pRoot->GetVar("SunLightDirectionX")->GetFloatValue();
	m_vSunLightDirection.y  = pRoot->GetVar("SunLightDirectionY")->GetFloatValue();
	m_vSunLightDirection.z  = pRoot->GetVar("SunLightDirectionZ")->GetFloatValue();
	m_material.Diffuse.a = pRoot->GetVar("Material.Diffuse.a")?pRoot->GetVar("Material.Diffuse.a")->GetFloatValue():m_material.Diffuse.a;
	m_material.Diffuse.r = pRoot->GetVar("Material.Diffuse.r")?pRoot->GetVar("Material.Diffuse.r")->GetFloatValue():m_material.Diffuse.r;
	m_material.Diffuse.g = pRoot->GetVar("Material.Diffuse.g")?pRoot->GetVar("Material.Diffuse.g")->GetFloatValue():m_material.Diffuse.g;
	m_material.Diffuse.b = pRoot->GetVar("Material.Diffuse.b")?pRoot->GetVar("Material.Diffuse.b")->GetFloatValue():m_material.Diffuse.b;

	m_material.Ambient.a = pRoot->GetVar("Material.Ambient.a")?pRoot->GetVar("Material.Ambient.a")->GetFloatValue():m_material.Ambient.a;
	m_material.Ambient.r = pRoot->GetVar("Material.Ambient.r")?pRoot->GetVar("Material.Ambient.r")->GetFloatValue():m_material.Ambient.r;
	m_material.Ambient.g = pRoot->GetVar("Material.Ambient.g")?pRoot->GetVar("Material.Ambient.g")->GetFloatValue():m_material.Ambient.g;
	m_material.Ambient.b = pRoot->GetVar("Material.Ambient.b")?pRoot->GetVar("Material.Ambient.b")->GetFloatValue():m_material.Ambient.b;

	m_material.Specular.a = pRoot->GetVar("Material.Specular.a")?pRoot->GetVar("Material.Specular.a")->GetFloatValue():m_material.Specular.a;
	m_material.Specular.r = pRoot->GetVar("Material.Specular.r")?pRoot->GetVar("Material.Specular.r")->GetFloatValue():m_material.Specular.r;
	m_material.Specular.g = pRoot->GetVar("Material.Specular.g")?pRoot->GetVar("Material.Specular.g")->GetFloatValue():m_material.Specular.g;
	m_material.Specular.b = pRoot->GetVar("Material.Specular.b")?pRoot->GetVar("Material.Specular.b")->GetFloatValue():m_material.Specular.b;

	m_material.Power = pRoot->GetVar("Material.Power")?pRoot->GetVar("Material.Power")->GetFloatValue():m_material.Power;

	m_material.Emissive.a = pRoot->GetVar("Material.Emissive.a")?pRoot->GetVar("Material.Emissive.a")->GetFloatValue():m_material.Emissive.a;
	m_material.Emissive.r = pRoot->GetVar("Material.Emissive.r")?pRoot->GetVar("Material.Emissive.r")->GetFloatValue():m_material.Emissive.r;
	m_material.Emissive.g = pRoot->GetVar("Material.Emissive.g")?pRoot->GetVar("Material.Emissive.g")->GetFloatValue():m_material.Emissive.g;
	m_material.Emissive.b = pRoot->GetVar("Material.Emissive.b")?pRoot->GetVar("Material.Emissive.b")->GetFloatValue():m_material.Emissive.b;
	utility::Text::Variable *pTempVar(NULL);
	pTempVar = pRoot->GetVar("FogEnable");
	if (pTempVar) 
	{
		m_bFogEnable = pTempVar->GetIntValue();
	}
	pTempVar = pRoot->GetVar("FogColor");
	if (pTempVar) 
	{
		m_dwFogColor = pTempVar->GetHexValue();
	}
	pTempVar = pRoot->GetVar("FogStart");
	if (pTempVar) 
	{
		m_fFogStart  = pTempVar-> GetFloatValue();
	}
	pTempVar = pRoot->GetVar("FogEnd");
	if (pTempVar) 
	{
		m_fFogEnd    = pTempVar->GetFloatValue();
	}
	m_dwSkyBoxID = pRoot->GetVar("SkyBoxID")?pRoot->GetVar("SkyBoxID")->GetIntValue():0;
	m_fSkyBoxSize = pRoot->GetVar("SkyBoxSize")?pRoot->GetVar("SkyBoxSize")->GetIntValue():1.0f;
	text.Destroy();

	///////////////////////////////////////////////////////////////////////////////
	//读取footeffect文件
	_snprintf(szTempName,1023,"%s\\footeffect",szPath);
	if (file.Open(szTempName))
	{
		text.Create(&file);
		file.Close();
		pRoot = text.GetRootNode();
		m_lFootEffectIdArray[0] = (pRoot->GetVar("Grass") != NULL) ? (long)pRoot->GetVar("Grass")->GetIntValue() : 0;
		m_lFootEffectIdArray[1] = (pRoot->GetVar("Mud") != NULL) ? (long)pRoot->GetVar("Mud")->GetIntValue() : 0;
		m_lFootEffectIdArray[2] = (pRoot->GetVar("Sand") != NULL) ? (long)pRoot->GetVar("Sand")->GetIntValue() : 0;
		m_lFootEffectIdArray[3] = (pRoot->GetVar("Wood") != NULL) ? (long)pRoot->GetVar("Wood")->GetIntValue() : 0;
		m_lFootEffectIdArray[4] = (pRoot->GetVar("Stone") != NULL) ? (long)pRoot->GetVar("Stone")->GetIntValue() : 0;
		m_lFootEffectIdArray[5] = (pRoot->GetVar("Water") != NULL) ? (long)pRoot->GetVar("Water")->GetIntValue() : 0;
		m_lFootEffectIdArray[6] = (pRoot->GetVar("Snow") != NULL) ? (long)pRoot->GetVar("Snow")->GetIntValue() : 0;
		m_lFootEffectIdArray[7] = (pRoot->GetVar("Marish") != NULL) ? (long)pRoot->GetVar("Marish")->GetIntValue() : 0;
		text.Destroy();
	}

	/////////////////////////////////////////////////////////////////////////////////
	//分配数据冲区
	m_pGridArray = new tagGrid[m_dwWidth * m_dwDepth];
	ZeroMemory(m_pGridArray,sizeof(tagGrid) * m_dwWidth * m_dwDepth);
	m_dwVertexCount = (m_dwWidth + 1) * (m_dwDepth + 1);
	m_pVertexPosArray = new D3DXVECTOR3[m_dwVertexCount];
	m_pVertexNormalArray = new D3DXVECTOR3[m_dwVertexCount];
	m_pVertexColArray    = new D3DCOLOR[m_dwVertexCount];		

	/////////////////////////////////////////////////////////////////////////////////
	//读取地表文件
	_snprintf(szTempName,1023,"%s\\surface",szPath);
	if (!file.Open(szTempName))
	{
		OutputError("open \"%s\" error\n",szTempName);
		MessageBox(NULL,szTempName,"open map file faild!",MB_OK);
		return FALSE;
	}
	DWORD dwTextureSet;
	file.Read(&dwTextureSet,sizeof(DWORD));
	//READ SHADER
	_snprintf(szTempName,1023,"terrain\\set%d\\shader",dwTextureSet);

	utility::File fileShader;	
	if (!fileShader.Open(szTempName))
	{
		OutputConsole("error: open \"%s\" failed.\n",szTempName);
		return 0;
	}

	char *pszShaderText = new char [fileShader.GetLength() + 1];
	memcpy(pszShaderText,fileShader.GetData(),fileShader.GetLength());
	pszShaderText[fileShader.GetLength()] = 0;	
	fileShader.Close();

	//get shader group handle
	m_dwTerrainShaderGroupHandle = pLayer3D->CreateShaderGroup(pszShaderText,"terrain\\texture");
	DWORD n = 0;
	for(;n < m_dwWidth * m_dwDepth; n++)
	{
		tagGrid *pGrid = &m_pGridArray[n];
		//uv coord index
		file.Read(pGrid->byUVIndex,sizeof(BYTE) * 4);
		//texture index 
		//|0000 0000 0000 0000| 编码 F 为空标志 
		//|  3    2    1    0 | 层编号
		file.Read(&pGrid->wLayerTextureIndex,sizeof(WORD));
	}
	file.Close();
	SAFEDELETEARRAY(pszShaderText);
	/////////////////////////////////////////////////////////////////////////////////
	//读取高度图
	_snprintf(szTempName,1023,"%s\\heightmap",szPath);
	if (!file.Open(szTempName))
	{
		OutputError("open \"%s\" error\n",szTempName);
		MessageBox(NULL,szTempName,"open map file faild!",MB_OK);
		return FALSE;
	}
	//read heightmap
	for(n = 0; n < m_dwVertexCount; n++)
	{
		float fHeight;
		file.Read(&fHeight,sizeof(float));
		m_pVertexPosArray[n].x = (float)(n % (m_dwWidth + 1));
		m_pVertexPosArray[n].y = fHeight;
		m_pVertexPosArray[n].z = (float)(n / (m_dwWidth + 1));
	}
	file.Close();
	/////////////////////////////////////////////////////////////////////////////////
	//读取颜色图
	_snprintf(szTempName,1023,"%s\\colormap",szPath);
	if (!file.Open(szTempName))
	{
		OutputError("open \"%s\" error\n",szTempName);
		MessageBox(NULL,szTempName,"open map file faild!",MB_OK);
		return FALSE;
	}

	for(n = 0; n < m_dwVertexCount; n++)
	{
		DWORD dwColor;
		file.Read(&dwColor,sizeof(DWORD));
		m_pVertexColArray[n] = dwColor;
	}
	file.Close();
	/////////////////////////////////////////////////////////////////////////////////
	//Build Grid Info
	DWORD v0,v1,v2,v3;
	DWORD dwGridIndex, dwIndex;
	for(DWORD z = 0 ; z < m_dwDepth; z++)
	{
		for(DWORD x = 0 ; x < m_dwWidth ; x++)
		{
			dwIndex = x + z * m_dwWidth;

			v0 = z *  (m_dwWidth + 1) + x;
			v1 = v0 + 1;
			v2 = v1 + (m_dwWidth + 1);
			v3 = v2 - 1;

			tagGrid &tGrid = m_pGridArray[dwIndex];
			tGrid.dwVertexIndex[0] = v0;
			tGrid.dwVertexIndex[1] = v1;
			tGrid.dwVertexIndex[2] = v2;
			tGrid.dwVertexIndex[3] = v3;
		}
	}
	//////////////////////////////////////////////////////////////////
	//读取网格信息
	_snprintf(szTempName,1023,"%s\\grid",szPath);
	if (!file.Open(szTempName))
	{
		OutputError("open \"%s\" error\n",szTempName);
		MessageBox(NULL,szTempName,"open map file faild!",MB_OK);
		return FALSE;
	}

	for(n = 0 ;  n < m_dwWidth * m_dwDepth; n++)
	{
		//地形
		tagGrid *pGrid = &m_pGridArray[n];
		//揭房顶标记
		file.Read(&pGrid->dwTargetHideGridIndex,sizeof(DWORD));		
		//附加属性标志
		file.Read(&pGrid->byFlag,sizeof(BYTE));
	}
	file.Close();

	//先建立建筑物缓冲
	//m_pBuildList = new GameBuildList;
	//m_pBuildList->Create();
	//////////////////////////////////////////////////////////////////
	//读取地物信息
	_snprintf(szTempName,1023,"%s\\node",szPath);
	if (!file.Open(szTempName))
	{
		OutputError("open \"%s\" error\n",szTempName);
		MessageBox(NULL,szTempName,"open map file faild!",MB_OK);
		return FALSE;
	}
	DWORD dwBuildCount(0);
	file.Read(&dwBuildCount,sizeof(DWORD));
	for(n = 0; n < dwBuildCount; n++ )
	{
		file.Read(&dwGridIndex,sizeof(DWORD));
		tagGrid *pGrid = &m_pGridArray[dwGridIndex];
        if (n >= m_dwWidth * m_dwDepth)
        {
            return FALSE;
        }
		pGrid->pBuild = new tagBuild;
		pGrid->pBuild->pBulid = NULL;
		pGrid->pBuild->pAnimInfo=NULL;
		file.Read(&pGrid->pBuild->wID,sizeof(WORD));
		if (m_fVersion < 1.7)
		{
			file.Read(&pGrid->pBuild->vPosOffset.y,sizeof(float));
			file.Read(&pGrid->pBuild->vRotation.y,sizeof(float));
			pGrid->pBuild->vPosOffset.x = pGrid->pBuild->vPosOffset.z = 
			pGrid->pBuild->vRotation.x = pGrid->pBuild->vRotation.z = 0.0f;
			pGrid->pBuild->vScaling.x = pGrid->pBuild->vScaling.y = pGrid->pBuild->vScaling.z = 1.0f;
		}else
		{
			// 版本1.7
			file.Read(&pGrid->pBuild->vPosOffset,sizeof(D3DXVECTOR3));
			file.Read(&pGrid->pBuild->vRotation,sizeof(D3DXVECTOR3));
			file.Read(&pGrid->pBuild->vScaling,sizeof(D3DXVECTOR3));
		}
	}
	//分割数据
	m_pRoot = new  ONode;
	m_pRoot->Read(&file);
	file.Close();
	/////////////////////////////////////////////////////////////////////////////////////////
	//可见表
	m_pGridVisibleFlagArray = new DWORD[m_dwWidth * m_dwDepth];
	ZeroMemory(m_pGridVisibleFlagArray,sizeof(DWORD) * m_dwWidth * m_dwDepth);
	m_pGridVisibleIndexArray = new DWORD[m_dwWidth * m_dwDepth];

	m_pBuildGridVisibleFlagArray = new DWORD[m_dwWidth * m_dwDepth];
	ZeroMemory(m_pBuildGridVisibleFlagArray,sizeof(DWORD) * m_dwWidth * m_dwDepth);
	m_pBuildGridVisibleIndexArray = new DWORD[m_dwWidth * m_dwDepth];

	//光照颜色缓冲
	m_pVertexColorRenderArray = new DWORD[sizeof(D3DCOLOR) * m_dwVertexCount];

	//光源缓冲数组
	m_pLightSourceArray = new tagLightSource[MAX_MAP_LIGHT_SOURCE_ARRAY_SIZE];
	m_dwLightSourceArrayUsage = 0;

	//脚印容器
	//m_pGameFootprint = new GameFootprint;
	//m_pGameFootprint->Create();
	//triangleArray = new GameFootprint::Triangle[256];

	//全局动画
	m_pGlobalAnimInfoArray = new AnimInfo[8];
	for(DWORD n = 0; n < 8; n++)
	{
		m_pGlobalAnimInfoArray[n].GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
		m_pGlobalAnimInfoArray[n].GetActionInfo()->bCurActionLooped = TRUE;
		m_pGlobalAnimInfoArray[n].SetAnimTime(timeGetTime());
	}

	return TRUE;
}



void GameMap::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	pLayer3D->DestroyShaderGroup(m_dwTerrainShaderGroupHandle);

	for(DWORD n = 0 ;  n < m_dwWidth * m_dwDepth; n++)
	{
		tagGrid *pGrid = &m_pGridArray[n];
		if (pGrid->pBuild)
		{
			//释放建筑资源
			GameBuildList::GetInstance()->ReleaseBuild(pGrid->pBuild->wID,pGrid->pBuild->pBulid);
			//删除动画信息
			SAFEDELETE(pGrid->pBuild->pAnimInfo);
		}
		SAFEDELETE(pGrid->pBuild);
	}
	//GameBuildList::GetInstance()->Release();

	SAFEDELETEARRAY(m_pGlobalAnimInfoArray);

	SAFEDELETEARRAY(m_pLightSourceArray);
	SAFEDELETEARRAY(m_pVertexColorRenderArray);
	//SAFEDESTROY(m_pGameFootprint);
	SAFEDELETE(m_pRoot);

	SAFEDELETEARRAY(m_pVertexPosArray);
	SAFEDELETEARRAY(m_pVertexNormalArray);
	SAFEDELETEARRAY(m_pVertexColArray);
	SAFEDELETEARRAY(m_pGridNormalArray);

	SAFEDELETEARRAY(m_pGridArray);
	SAFEDELETEARRAY(m_pGridVisibleFlagArray);
	SAFEDELETEARRAY(m_pGridVisibleIndexArray);
	SAFEDELETEARRAY(m_pBuildGridVisibleFlagArray);
	SAFEDELETEARRAY(m_pBuildGridVisibleIndexArray);
	//GameBuildList::GetInstance()->Release();
	//SAFEDESTROY(m_pBuildList);
}



/*
* 功能: 渲染地图
* 摘要: 包括地表和建物的渲染
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.10.20 - lpf
*		渲染建物时,增加了水面的处理
*/
void GameMap::Render(GameSelectRay *pCameraRay,render::Camera *pCamera,bool bdrawmodel,bool isForceEnableLightProcess)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	pDevice->SetRenderState(D3DRS_FILLMODE,pLayer3D->GetFillMode());

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF ,0x00000000);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_FOGENABLE,m_bFogEnable);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	//pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE2X);
	//pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	//pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	/*pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);*/
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	pDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	pDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
	pDevice->SetRenderState(D3DRS_SPECULARENABLE,false);
	if (isForceEnableLightProcess)
		pDevice->SetRenderState(D3DRS_LIGHTING,true);
	else
		pDevice->SetRenderState(D3DRS_LIGHTING,false);
	//D3DMATERIAL9 mat = m_material;
	//mat.Diffuse.a = 1.0f;
	//mat.Diffuse.r = 1.0f;
	//mat.Diffuse.g = 1.0f;
	//mat.Diffuse.b = 1.0f;
	//pDevice->SetMaterial( &mat );
	//pDevice->SetRenderState(D3DRS_AMBIENT,m_dwTerrainAmbientColor);

	static D3DXVECTOR3 vertex[20000],normal[20000];
	static D3DXVECTOR2 uv0[20000];
	static DWORD color[20000];
	//static render::Interface::Layer3D::_tagTriangle triangle[20000];
	static D3DXVECTOR3 vCenter;

	// vertex order   uv order 
	// 3----2         0----1 
	// |    |         |    | 
	// |    |         |    | 
	// 0----1         3----2 
	//-----------------------------------------------------------------------------

	static DWORD dwCurTime = timeGetTime();
	DWORD lastindex = 0;
	dwCurTime = timeGetTime();
	int dpnumber = 0;
	int usenum = 0;
	for(DWORD nLayer = 0; nLayer < 4; nLayer++)
	{
		for(DWORD n = 0; n< m_dwGridVisibleIndexCount; n++)
		{	
			DWORD dwGridIndex = m_pGridVisibleIndexArray[n];

			tagGrid &tGrid = m_pGridArray[dwGridIndex];

			WORD wLayerTextureIndex = tGrid.wLayerTextureIndex >> (nLayer * 4)  & 0xF;  
			//需要画当前层？
			if ((tGrid.byFlag & GRIDMASK_VISIBLE) && wLayerTextureIndex != 0xF)
			{
				////画
				//triangle[usenum].pos[0] = m_pVertexPosArray[tGrid.dwVertexIndex[0]];
				//triangle[usenum].pos[1] = m_pVertexPosArray[tGrid.dwVertexIndex[3]];
				//triangle[usenum].pos[2] = m_pVertexPosArray[tGrid.dwVertexIndex[2]];

				//triangle[usenum].color[0] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[0]];
				//triangle[usenum].color[1] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[3]];
				//triangle[usenum].color[2] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[2]];

				//BYTE byUVIndex = tGrid.byUVIndex[nLayer];

				//triangle[usenum].uv0[0].x = CodeUVTable[byUVIndex][6];
				//triangle[usenum].uv0[0].y = CodeUVTable[byUVIndex][7];

				//triangle[usenum].uv0[1].x = CodeUVTable[byUVIndex][0];
				//triangle[usenum].uv0[1].y = CodeUVTable[byUVIndex][1];

				//triangle[usenum].uv0[2].x = CodeUVTable[byUVIndex][2];
				//triangle[usenum].uv0[2].y = CodeUVTable[byUVIndex][3];

				//// the second triangle
				//triangle[usenum + 1].pos[0] = m_pVertexPosArray[tGrid.dwVertexIndex[0]];
				//triangle[usenum + 1].pos[1] = m_pVertexPosArray[tGrid.dwVertexIndex[2]];
				//triangle[usenum + 1].pos[2] = m_pVertexPosArray[tGrid.dwVertexIndex[1]];

				//triangle[usenum + 1].color[0] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[0]];
				//triangle[usenum + 1].color[1] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[2]];
				//triangle[usenum + 1].color[2] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[1]];

				//triangle[usenum + 1].uv0[0].x = CodeUVTable[byUVIndex][6];
				//triangle[usenum + 1].uv0[0].y = CodeUVTable[byUVIndex][7];

				//triangle[usenum + 1].uv0[1].x = CodeUVTable[byUVIndex][2];
				//triangle[usenum + 1].uv0[1].y = CodeUVTable[byUVIndex][3];

				//triangle[usenum + 1].uv0[2].x = CodeUVTable[byUVIndex][4];
				//triangle[usenum + 1].uv0[2].y = CodeUVTable[byUVIndex][5];

				//画

				if (lastindex != wLayerTextureIndex)
				{
					if(isForceEnableLightProcess)
						pLayer3D->DrawTriangleArray(m_dwTerrainShaderGroupHandle,lastindex, usenum * 2, vertex, normal, uv0,color);
					else
					{

						pLayer3D->DrawTriangleArray(m_dwTerrainShaderGroupHandle,lastindex, usenum * 2, vertex, uv0,color,*pCamera->GetViewMatrix(),*pCamera->GetProjectionMatrix());



					}
					usenum = 0;
				}
				vertex[usenum * 6 + 0] = m_pVertexPosArray[tGrid.dwVertexIndex[0]];
				vertex[usenum * 6 + 1] = m_pVertexPosArray[tGrid.dwVertexIndex[3]];
				vertex[usenum * 6 + 2] = m_pVertexPosArray[tGrid.dwVertexIndex[2]];

				color[usenum * 6 + 0] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[0]];
				color[usenum * 6 + 1] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[3]];
				color[usenum * 6 + 2] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[2]];

				if (isForceEnableLightProcess)
				{
					normal[usenum * 6 + 0] = m_pVertexNormalArray[tGrid.dwVertexIndex[0]];
					normal[usenum * 6 + 1] = m_pVertexNormalArray[tGrid.dwVertexIndex[3]];
					normal[usenum * 6 + 2] = m_pVertexNormalArray[tGrid.dwVertexIndex[2]];
				}

				BYTE byUVIndex = tGrid.byUVIndex[nLayer];

				uv0[usenum * 6 + 0].x = CodeUVTable[byUVIndex][6];
				uv0[usenum * 6 + 0].y = CodeUVTable[byUVIndex][7];

				uv0[usenum * 6 + 1].x = CodeUVTable[byUVIndex][0];
				uv0[usenum * 6 + 1].y = CodeUVTable[byUVIndex][1];

				uv0[usenum * 6 + 2].x = CodeUVTable[byUVIndex][2];
				uv0[usenum * 6 + 2].y = CodeUVTable[byUVIndex][3];

				// the second triangle
				vertex[usenum * 6 + 3] = m_pVertexPosArray[tGrid.dwVertexIndex[0]];
				vertex[usenum * 6 + 4] = m_pVertexPosArray[tGrid.dwVertexIndex[2]];
				vertex[usenum * 6 + 5] = m_pVertexPosArray[tGrid.dwVertexIndex[1]];

				color[usenum * 6 + 3] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[0]];
				color[usenum * 6 + 4] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[2]];
				color[usenum * 6 + 5] = m_pVertexColorRenderArray[tGrid.dwVertexIndex[1]];

				if (isForceEnableLightProcess)
				{
					normal[usenum * 6 + 3] = m_pVertexNormalArray[tGrid.dwVertexIndex[0]];
					normal[usenum * 6 + 4] = m_pVertexNormalArray[tGrid.dwVertexIndex[2]];
					normal[usenum * 6 + 5] = m_pVertexNormalArray[tGrid.dwVertexIndex[1]];
				}

				uv0[usenum * 6 + 3].x = CodeUVTable[byUVIndex][6];
				uv0[usenum * 6 + 3].y = CodeUVTable[byUVIndex][7];

				uv0[usenum * 6 + 4].x = CodeUVTable[byUVIndex][2];
				uv0[usenum * 6 + 4].y = CodeUVTable[byUVIndex][3];

				uv0[usenum * 6 + 5].x = CodeUVTable[byUVIndex][4];
				uv0[usenum * 6 + 5].y = CodeUVTable[byUVIndex][5];
				if (((usenum * 6 + 5)  > 19990))
				{
					if(isForceEnableLightProcess)
						pLayer3D->DrawTriangleArray(m_dwTerrainShaderGroupHandle,lastindex, usenum * 2, vertex, normal, uv0, color);
					else
					{

						pLayer3D->DrawTriangleArray(m_dwTerrainShaderGroupHandle,lastindex, usenum * 2, vertex, uv0,color,*pCamera->GetViewMatrix(),*pCamera->GetProjectionMatrix());

					}

					usenum = -1;
				}
				usenum++;
				lastindex = wLayerTextureIndex;
			}
		}
	}
	if(isForceEnableLightProcess)
		pLayer3D->DrawTriangleArray(m_dwTerrainShaderGroupHandle,lastindex, usenum * 2, vertex, normal, uv0, color);
	else
		pLayer3D->DrawTriangleArray(m_dwTerrainShaderGroupHandle,lastindex, usenum * 2, vertex, uv0,color,*pCamera->GetViewMatrix(),*pCamera->GetProjectionMatrix());

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);

	static D3DXVECTOR3 vIntersection;
	//DWORD dwtime = m_pGlobalAnimInfoArray[6].GetCurrentTime();
	//float fDist;
	for(DWORD n = 0; n< m_dwBuildGridVisibleIndexCount; n++)
	{	
		DWORD dwGridIndex = m_pBuildGridVisibleIndexArray[n];

		tagGrid &tGrid = m_pGridArray[dwGridIndex];

		vCenter = (m_pVertexPosArray[tGrid.dwVertexIndex[0]] + m_pVertexPosArray[tGrid.dwVertexIndex[1]] + 
			m_pVertexPosArray[tGrid.dwVertexIndex[2]] + m_pVertexPosArray[tGrid.dwVertexIndex[3]]) / 4.0f;
		//------------------------------------------------------
		//画地物

		if (m_dwTargetHideGridIndex == dwGridIndex)
		{
			continue;
		}

		if (tGrid.pBuild && tGrid.pBuild->GetLoadState() == Load_Did && tGrid.pBuild->pBulid)
		{
			WORD &wID = tGrid.pBuild->wID;
			GameBuildList::Build *pBuild = tGrid.pBuild->pBulid;

			static D3DXMATRIX matWorld,matRot,matScaling;
			D3DXMatrixTranslation(&matWorld, vCenter.x + tGrid.pBuild->vPosOffset.x, vCenter.y + tGrid.pBuild->vPosOffset.y, vCenter.z + tGrid.pBuild->vPosOffset.z);
			D3DXMatrixRotationYawPitchRoll(&matRot, tGrid.pBuild->vRotation.y, tGrid.pBuild->vRotation.x, tGrid.pBuild->vRotation.z);
			D3DXMatrixScaling(&matScaling, tGrid.pBuild->vScaling.x, tGrid.pBuild->vScaling.y, tGrid.pBuild->vScaling.z);
			matWorld = matScaling * matRot * matWorld;
			//D3DXMatrixRotationY(&matRot,tGrid.pBuild->fRotationY);
			//D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

			GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
			if (pModel && pModel->pGameModel &&
				pModel->pGameModel->GetLoadState() == Load_Did)
			{
				CDisplayModel *pGameModel = pModel->pGameModel;
				if (m_pFrustum)
				{
					if (!m_pFrustum->BoxIntersectFrustum(&matWorld,pGameModel->GetSizeBox()))
					{
						continue;
					}

				}
				AnimInfo *pAnimInfo = (pModel->dwGlobalTimer)? &m_pGlobalAnimInfoArray[pModel->dwGlobalTimer-1]:tGrid.pBuild->pAnimInfo;
				AnimInfo::tagActionInfo *pActionInfo = pAnimInfo->GetActionInfo();
				if (pActionInfo->dwCurActionLoopCount > 0)
				{
					pActionInfo->dwCurActionLoopCount = 0;
					//1次 切换到下一个动作
					if (pActionInfo->dwCurAction == MAKEFOURCC('I','D','L','E'))
						pActionInfo->dwCurAction = MAKEFOURCC('S','T','N','D');
					else
						pActionInfo->dwCurAction = MAKEFOURCC('I','D','L','E');
				}

				BOOL bHide = FALSE;
				pAnimInfo->SetBillboard(pCamera->GetBillboard());
				pAnimInfo->SetBillboardY(pCamera->GetBillboardY());
				pAnimInfo->SetWorldMatrix(&matWorld);
				pAnimInfo->SetViewMatrix(pCamera->GetViewMatrix());
				pAnimInfo->SetProjectedMatrix(pCamera->GetProjectionMatrix());
				pAnimInfo->SetCurrentTime(dwCurTime);


				float fDist = 0;
				if (pModel->bAutoHide && pCameraRay->TestBoundingBox(&matWorld,pGameModel->GetSizeBox(),&vIntersection,&fDist))
				{
					bHide = TRUE;
				}
				//pGameModel->SetRegionAmbient(m_dwSunLightAmbientColorforBuild);





				//方向光 + 地表颜色
				DWORD dwGridColor = 0xFF000000 | m_pVertexColArray[tGrid.dwVertexIndex[0]];

				//if (pModel->bLighted)
				//{
				//	pGameModel->SetDirLightEnable(false);
				//	
				//	
				//	//pGameModel->SetAmbient(dwGridColor);
				//	//pGameModel->SetModelColor(0x00000000);
				//	
				//}
				//else
				//{
				//	//pGameModel->SetLightEnable();
				//	
				//	//pGameModel->SetAmbient(dwGridColor);
				//	//
				//}	


				pGameModel->SetDirLightEnable(false);

				if (isForceEnableLightProcess)
				{
					pGameModel->ForceToEnableLight();
					pGameModel->ProcessAnimJoint(pAnimInfo);
					pGameModel->SetModelColor(m_dwSunLightAmbientColorforBuild);
					pGameModel->MultiplyModelColor(dwGridColor);
				}
				else
				{
					if (pModel->bLighted)
					{		

						pGameModel->SetDirLightEnable(true);
						pGameModel->ProcessAnimJoint(pAnimInfo);
						pGameModel->SetModelColor(m_dwSunLightAmbientColorforBuild);
						//pGameModel->SetModelColor(dwGridColor);
						//pGameModel->ProcessDirectionalLight(pCamera->GetPosition(),&m_vSunLightDirection,m_dwSunLightDiffuseColor);
						pGameModel->MultiplyModelColor(dwGridColor);

					}
					else
					{
						pGameModel->SetDirLightEnable(false);
						pGameModel->ProcessAnimJoint(pAnimInfo);
						pGameModel->SetModelColor(dwGridColor);
					}
				}

				pGameModel->ProcessAnimTexture(pAnimInfo);
				pGameModel->ProcessAnimUV(pAnimInfo);
				pGameModel->ProcessVisibility(pAnimInfo);
				pGameModel->ProcessParticleEmitter(pAnimInfo);
				pGameModel->ProcessTrailEmitter(pAnimInfo);
				pGameModel->ProcessLocator(pAnimInfo);
				pGameModel->ProcessTextureProjective(pAnimInfo);
				pGameModel->ProcessAnimColor(pAnimInfo);
				pGameModel->ProcessVisibility(pAnimInfo);
				pGameModel->ProcessWater(pAnimInfo);

				// 如果有水面,则不需要处理建物本身模型
				if (pGameModel->GetWater() == NULL)
				{
					if (bdrawmodel)
					{
						pGameModel->DrawModel();
					}
					else
					{
						if (bHide)
						{
							DWORD dwcolor = dwGridColor&0x00ffffff;
							dwcolor |= 0x7f000000;
							pGameModel->SetModelColor(dwcolor);
							pGameModel->RenderModelTransparent();
						}
						else
						{
							pGameModel->RenderModel();
						}
					}
				}
			}
		}
	}
	//m_pGameFootprint->Update();
	//m_pGameFootprint->Render();
}
void GameMap::GetTagMapBuffer(int x,int z,DWORD size,D3DXVECTOR3* posbuff,std::list<DWORD>& listrendbuild)
{
	DWORD dwGridIndex = 0;
	DWORD nowgridnum = 0;
	tagGrid *pGrid(NULL); 
	int i = 0;
	int j = 0;

	if (size < 1)
	{
		size = 1;
	}

	for (i = x - (size - 1);i <= x + ((int)size - 1);i++)
	{


		for (j = z - (size - 1);j <= z + ((int)size - 1);j++)
		{


			dwGridIndex = (DWORD)i + (DWORD)j * m_dwWidth;
			if (i < 0 || j < 0)
			{
				dwGridIndex = 0;
			}
			if (dwGridIndex > m_dwWidth * m_dwWidth - 1)
			{
				dwGridIndex = m_dwWidth * m_dwWidth - 1;
			}
			pGrid = GetGrid(dwGridIndex);
			if (!(pGrid->byFlag & GRIDMASK_VISIBLE))
			{
				posbuff[nowgridnum * 6 + 0] = D3DXVECTOR3(0.0f,0.0f,0.0f);
				posbuff[nowgridnum * 6 + 1] = D3DXVECTOR3(0.0f,0.0f,0.0f);
				posbuff[nowgridnum * 6 + 2] = D3DXVECTOR3(0.0f,0.0f,0.0f);
				posbuff[nowgridnum * 6 + 3] = D3DXVECTOR3(0.0f,0.0f,0.0f);
				posbuff[nowgridnum * 6 + 4] = D3DXVECTOR3(0.0f,0.0f,0.0f);
				posbuff[nowgridnum * 6 + 5] = D3DXVECTOR3(0.0f,0.0f,0.0f);

			}
			else
			{
				posbuff[nowgridnum * 6 + 0] = m_pVertexPosArray[pGrid->dwVertexIndex[0]];
				posbuff[nowgridnum * 6 + 1] = m_pVertexPosArray[pGrid->dwVertexIndex[3]];
				posbuff[nowgridnum * 6 + 2] = m_pVertexPosArray[pGrid->dwVertexIndex[2]];
				posbuff[nowgridnum * 6 + 3] = m_pVertexPosArray[pGrid->dwVertexIndex[0]];
				posbuff[nowgridnum * 6 + 4] = m_pVertexPosArray[pGrid->dwVertexIndex[2]];
				posbuff[nowgridnum * 6 + 5] = m_pVertexPosArray[pGrid->dwVertexIndex[1]];

			}
			for (std::vector<DWORD>::iterator it = pGrid->listgridindex.begin();it != pGrid->listgridindex.end();it++)
			{
				if (find(listrendbuild.begin(),listrendbuild.end(),*it) == listrendbuild.end())
				{
					listrendbuild.push_back(*it);
				}	
			}
			/*pcolorbuff[nowgridnum * 6 + 0] = D3DCOLOR_ARGB(255,255,255,255);
			pcolorbuff[nowgridnum * 6 + 1] = D3DCOLOR_ARGB(255,255,255,255);
			pcolorbuff[nowgridnum * 6 + 2] = D3DCOLOR_ARGB(255,255,255,255);
			pcolorbuff[nowgridnum * 6 + 3] = D3DCOLOR_ARGB(255,255,255,255);
			pcolorbuff[nowgridnum * 6 + 4] = D3DCOLOR_ARGB(255,255,255,255);
			pcolorbuff[nowgridnum * 6 + 5] = D3DCOLOR_ARGB(255,255,255,255);*/
			nowgridnum++;
		}
	}
}

void GameMap::Update(render::Frustum *pFrustum,bool bAsyn)
{
	m_bAsyncLoadBuild = bAsyn;
	RestVisibleFlag();	//计算起始
	m_pRoot->TestVisible(this,pFrustum);
	CalculateLightSourceLighting();
	m_pFrustum = pFrustum;
}

void GameMap::SetGridVisibleIndex(DWORD dwGridIndex)
{
	DWORD &dwFlag = m_pGridVisibleFlagArray[dwGridIndex];
	if ( dwFlag != m_dwCurFlagValue)	
	{
		dwFlag = m_dwCurFlagValue;

		m_pGridVisibleIndexArray[m_dwGridVisibleIndexCount++] = dwGridIndex;
	}
}
//设置投影纹理范围内格子对应建筑所对应的格子的索引
void GameMap::SetRoundGridProIndex(DWORD gridindex,int size)
{
	DWORD dwGridIndex = 0;

	DWORD nowgridnum = 0;
	tagGrid *pGrid(NULL); 
	int i = 0;
	int j = 0;
	int x = gridindex%m_dwWidth;
	int z = gridindex/m_dwWidth;
	if (size < 1)
	{
		size = 1;
	}

	for (i = x - (size - 1);i <= x + ((int)size - 1);i++)
	{


		for (j = z - (size - 1);j <= z + ((int)size - 1);j++)
		{


			dwGridIndex = (DWORD)i + (DWORD)j * m_dwWidth;
			if (i < 0 || j < 0)
			{
				dwGridIndex = 0;
			}
			if (dwGridIndex > m_dwWidth * m_dwWidth - 1)
			{
				dwGridIndex = m_dwWidth * m_dwWidth - 1;
			}
			pGrid = GetGrid(dwGridIndex);
			pGrid->listgridindex.push_back(gridindex);
			nowgridnum++;
		}
	}

}

//设置建筑可见得格子
void GameMap::SetBuildGridVisibleIndex(DWORD dwGridIndex)
{
	static GameBuildList *pInstance = GameBuildList::GetInstance();
	DWORD &dwFlag = m_pBuildGridVisibleFlagArray[dwGridIndex];
	if ( dwFlag != m_dwCurFlagValue)	
	{
		dwFlag = m_dwCurFlagValue; 
		//装载建筑信息
		tagGrid &tGrid = m_pGridArray[dwGridIndex];
		if( tGrid.pBuild->GetLoadState() != Load_Did )
		{
			GameBuildList::Build* pBuild = NULL;
			if (tGrid.pBuild->GetLoadState() == Load_Not)
			{
				pBuild = pInstance->AcquireBuild(tGrid.pBuild->wID,m_bAsyncLoadBuild);
				if (pBuild)
				{
					tGrid.pBuild->pBulid = pBuild;

				}

			}
			else
			{
				pBuild = tGrid.pBuild->pBulid;
			}
			if (pBuild)
			{
				tGrid.pBuild->SetLoadState(pBuild->GetLoadState());
			}
			//有自己的时钟
			if(pBuild && pBuild->GetLoadState() == Load_Did )
			{

				if(pBuild->GetModel() )
				{
					if (pBuild->GetModel()->dwGlobalTimer == 0)
					{
						tGrid.pBuild->pAnimInfo = new AnimInfo;
						tGrid.pBuild->pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
						tGrid.pBuild->pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
					}
					if (pBuild->GetModel()->bprojected == TRUE)
					{
						SetRoundGridProIndex(dwGridIndex,pBuild->GetModel()->dwprojectedsize);
					}
				}
				tGrid.pBuild->SetLoadState(Load_Did);
			}

		}
		if (tGrid.pBuild->GetLoadState() == Load_Did)
		{
			m_pBuildGridVisibleIndexArray[m_dwBuildGridVisibleIndexCount++] = dwGridIndex;
		}	
	}
}

void GameMap::RestVisibleFlag()
{
	m_dwCurFlagValue++;
	m_dwGridVisibleIndexCount = 0;
	m_dwBuildGridVisibleIndexCount = 0;
}

//CLASS___(GameMap::Node)______
float GameMap::CalculateHeight(float x , float z) const
{
	static D3DXVECTOR3 pos[4],vIntersection;
	D3DXVECTOR3 vRayPos(x,-1000.0f,z),vRayDir(0.0f,1.0f,0.0f);
	DWORD dwGridIndex = (DWORD)x + (DWORD)z * m_dwWidth;
	tagGrid *pGrid(NULL);     	

	pGrid = GetGrid(dwGridIndex);

	if (dwGridIndex >= m_dwWidth * m_dwDepth)
	{
		return 0.0f;
	}

	for(DWORD n = 0; n < 4; n++)
	{
		pos[n] = m_pVertexPosArray[pGrid->dwVertexIndex[n]];
	}

	float u,v,d;
	if (D3DXIntersectTri(&pos[0],&pos[1],&pos[2],&vRayPos,&vRayDir,&u,&v,&d))		
	{
		vIntersection = pos[0] + u * (pos[1] - pos[0]) + v * (pos[2] - pos[0]); 
		return vIntersection.y;
	}
	else
		if (D3DXIntersectTri(&pos[0],&pos[2],&pos[3],&vRayPos,&vRayDir,&u,&v,&d))
		{
			vIntersection = pos[0] + u * (pos[2] - pos[0]) + v * (pos[3] - pos[0]); 
			return vIntersection.y;
		}

		return 0.0f;

}

/* 判断线段相交 */ 
D3DXVECTOR3  GameMap::LineIntersectLine(float L1p1x, float L1p1y, 
										float L1p2x, float L1p2y,
										float L2p1x, float L2p1y, 
										float L2p2x, float L2p2y)
{ 
	float N, O, P, Q, k, U, V, tempX, tempY;

	N = L1p2x - L1p1x;
	O = L1p2y - L1p1y;
	P = L2p2x - L2p1x;
	Q = L2p2y - L2p1y;
	k = Q * N - O * P;
	U = N * L1p1y - O * L1p1x;
	V = P * L2p1y - Q * L2p1x;
	tempX = (P * U - N * V)/k;
	tempY = (Q * U - O * V)/k;
	D3DXVECTOR3 vec3;
	vec3.x = tempX;
	vec3.y = 0.0f;
	vec3.z = tempY;
	return vec3;
} 

BOOL GameMap::HitTestGrid(const D3DXVECTOR3 *pLine,D3DXVECTOR3 *pIntersection,D3DXVECTOR3 *pN)
{
	static D3DXVECTOR3 pos[4],vIntersection;
	DWORD dwGridIndex = (DWORD)pLine[0].x + (DWORD)pLine[0].z * m_dwWidth;
	tagGrid *pGrid(NULL);     	

	pGrid = GetGrid(dwGridIndex);

	if (dwGridIndex >= m_dwWidth * m_dwDepth)
	{
		return FALSE;
	}

	for(DWORD n = 0; n < 4; n++)
	{
		pos[n] = m_pVertexPosArray[pGrid->dwVertexIndex[n]];
	}

	static D3DXVECTOR3 vA,vB;
	if (FM_LineSegmentIntesectTriangle(pLine,pos,*pIntersection))		
	{
		vA = pos[2] - pos[0];
		vB = pos[1] - pos[0];

		D3DXVec3Cross(pN,&vA,&vB);		// Take the cross product of our 2 vectors to get a perpendicular vector
		D3DXVec3Normalize(pN,pN);
		return TRUE;
	}
	else
	{
		pos[1] = pos[2];pos[2] = pos[3];
		if (FM_LineSegmentIntesectTriangle(pLine,pos,*pIntersection))
		{
			vA = pos[2] - pos[0];
			vB = pos[1] - pos[0];

			D3DXVec3Cross(pN,&vA,&vB);		// Take the cross product of our 2 vectors to get a perpendicular vector
			D3DXVec3Normalize(pN,pN);
			return TRUE;
		}
	}
	return FALSE;
}

DWORD GameMap::CalculateHoverGrid(GameSelectRay * pSelRay,D3DXVECTOR3 &point)
{
	tagGrid *pGrid(NULL);
	static D3DXVECTOR3 pos[4],vIntersection;
	DWORD dwSelGrid = 0xFFFFFFFF;
	float fDistance = 100000000.0f;
	float fTempDist;

	for(DWORD n = 0; n< m_dwGridVisibleIndexCount; n++)
	{
		DWORD dwGridIndex = m_pGridVisibleIndexArray[n];

		pGrid = GetGrid(dwGridIndex);

		for(DWORD n = 0; n < 4; n++)
		{
			pos[n] = m_pVertexPosArray[pGrid->dwVertexIndex[n]];
		}

		if (pSelRay->IntersectTri(&pos[0],&pos[1],&pos[2],&vIntersection,&fTempDist))
		{
			if (fTempDist < fDistance)
			{
				fDistance = fTempDist;
				dwSelGrid = dwGridIndex;
				point = vIntersection;
			}
		}
		else
			if (pSelRay->IntersectTri(&pos[0],&pos[2],&pos[3],&vIntersection,&fTempDist))
			{	 
				if (fTempDist < fDistance)
				{
					fDistance = fTempDist;
					dwSelGrid = dwGridIndex;
					point = vIntersection;
				}
			}
	}
	return dwSelGrid;
}
void GameMap::_DEBUG_RenderGrid(DWORD dwCurSelGridIndex)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	static D3DXVECTOR3 vertex[6],normal[6];
	static D3DXVECTOR2 uv0[6];
	static DWORD color[6];
	static D3DXVECTOR3 vCenter;

	// vertex order   uv order 
	// 3----2         0----1 
	// |    |         |    | 
	// |    |         |    | 
	// 0----1         3----2 
	//-----------------------------------------------------------------------------

	static DWORD dwCurTime;
	dwCurTime = timeGetTime();

	for(DWORD n = 0; n< m_dwGridVisibleIndexCount; n++)
	{	
		DWORD dwGridIndex = m_pGridVisibleIndexArray[n];

		tagGrid &tGrid = m_pGridArray[dwGridIndex];

		if (dwGridIndex == dwCurSelGridIndex)
		{
			pLayer3D->_DEBUG_DrawWireQuad(&m_pVertexPosArray[tGrid.dwVertexIndex[0]],
				&m_pVertexPosArray[tGrid.dwVertexIndex[1]],
				&m_pVertexPosArray[tGrid.dwVertexIndex[2]],
				&m_pVertexPosArray[tGrid.dwVertexIndex[3]],
				0xFFFF0000);
		}
		else
		{
			pLayer3D->_DEBUG_DrawWireQuad(&m_pVertexPosArray[tGrid.dwVertexIndex[0]],
				&m_pVertexPosArray[tGrid.dwVertexIndex[1]],
				&m_pVertexPosArray[tGrid.dwVertexIndex[2]],
				&m_pVertexPosArray[tGrid.dwVertexIndex[3]],
				0xFFFFFFFF);
		}
	}

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
}

void GameMap::_DEBUG_RenderBoundingBox()
{
	m_pRoot->_DEBUG_RenderBoundingBox();
}
void GameMap::_DEBUG_RenderBoundingBox(render::Frustum *pFrustum)
{
	m_pRoot->_DEBUG_RenderBoundingBox(pFrustum);
}

//////////////////////////////////////////////////////////////////

int GameMap::ONode::m_iNodeCount = 0;

GameMap::ONode::ONode()
{
	m_pChildNodeArray = NULL;
	m_iGridIndexCount = 0;
	m_pGridIndexArray = NULL;
	m_pBuildGridIndexArray = NULL;
	m_iBuildGridIndexCount = 0;

	m_pBoundingBox = new render::BoundingBox;
}

GameMap::ONode::~ONode()
{
	SAFEDELETE(m_pBoundingBox);
	SAFEDELETEARRAY(m_pGridIndexArray);
	SAFEDELETEARRAY(m_pBuildGridIndexArray);
	SAFEDELETEARRAY(m_pChildNodeArray);
}

void GameMap::ONode::TestVisible(GameMap *pMap,render::Frustum *pFrustum)
{
	if (!pFrustum->BoxIntersectFrustum(m_pBoundingBox)) return;

	if (IsLeaf())
	{	
		for(int i =0; i < m_iGridIndexCount; i++)
		{
			pMap->SetGridVisibleIndex(m_pGridIndexArray[i]);
		}
		for(int j = 0; j < m_iBuildGridIndexCount; j++)
		{
			pMap->SetBuildGridVisibleIndex(m_pBuildGridIndexArray[j]);
		}
		return;
	}

	BOOL bNAIF = TRUE;

	if (m_pChildNodeArray)
	{
		for(int iChild=0;iChild<8;iChild++)	
		{
			if (!pFrustum->BoxInFrustum(m_pChildNodeArray[iChild].GetBoundingBox()))
			{
				bNAIF  = FALSE;
				break;
			}
		}
	}

	if (bNAIF)
	{
		for(int i =0; i < m_iGridIndexCount; i++)
		{
			pMap->SetGridVisibleIndex(m_pGridIndexArray[i]);
		}
		for(int j = 0; j < m_iBuildGridIndexCount; j++)
		{
			pMap->SetBuildGridVisibleIndex(m_pBuildGridIndexArray[j]);
		}
		return;
	}

	if (m_pChildNodeArray)
	{
		for(int iChild=0;iChild<8;iChild++)	
		{
			m_pChildNodeArray[iChild].TestVisible(pMap,pFrustum);
			//m_pChildNodeArray[iChild]._DEBUG_RenderBoundingBox(pFrustum);
		}

	}
}

void GameMap::ONode::Read(utility::File * pFile)
{
	m_iNodeCount++;

	//自己的数据
	D3DXVECTOR3 vMin,vMax;
	pFile->Read(&vMin,sizeof(D3DXVECTOR3));
	pFile->Read(&vMax,sizeof(D3DXVECTOR3));
	m_pBoundingBox->Set(vMin,vMax);      	
	//格子的列表
	pFile->Read(&m_iGridIndexCount,sizeof(int));
	if (m_iGridIndexCount)
	{
		m_pGridIndexArray= new int[m_iGridIndexCount];
		pFile->Read(m_pGridIndexArray,sizeof(int) * m_iGridIndexCount);
	}
	pFile->Read(&m_iBuildGridIndexCount,sizeof(int));
	if (m_iBuildGridIndexCount)
	{
		m_pBuildGridIndexArray = new int[m_iBuildGridIndexCount];
		pFile->Read(m_pBuildGridIndexArray,sizeof(int) * m_iBuildGridIndexCount);
	}

	//子节点的数据
	BOOL bIsLeaf;
	pFile->Read(&bIsLeaf,sizeof(BOOL));
	if (!bIsLeaf)
	{
		m_pChildNodeArray = new ONode[8];
		for(int iChild=0;iChild<8;iChild++)	
		{
			m_pChildNodeArray[iChild].Read(pFile);
		}	
	}
	//OutputConsole("Node:[%d] GI: %d BI: %d\n",m_iNodeCount,m_iGridIndexCount,m_iBuildGridIndexCount);
}

BOOL GameMap::ONode::IsLeaf()
{
	if (!m_pChildNodeArray)
		return TRUE;
	else
		return FALSE;
}

void GameMap::ONode::_DEBUG_RenderBoundingBox()
{
	static D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	if (IsLeaf()) 
	{
		m_pBoundingBox->_DEBUG_Render(&matWorld,0x80ffaa00);
	}
	else
		for(int iChild = 0 ; iChild < 8; iChild++ )	
		{
			m_pChildNodeArray[iChild]._DEBUG_RenderBoundingBox();	
		}
}

void GameMap::ONode::_DEBUG_RenderBoundingBox(render::Frustum *pFrustum)
{
	static D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	if (!pFrustum->BoxIntersectFrustum(m_pBoundingBox)) return;

	if (IsLeaf())
	{	
		m_pBoundingBox->_DEBUG_Render(&matWorld,0x40ffaaaa);
		return ;
	}

	BOOL bAllInThisNode = TRUE;

	for(int iChild=0;iChild<8;iChild++)	
	{
		if (!pFrustum->BoxInFrustum(m_pChildNodeArray[iChild].m_pBoundingBox))
		{
			bAllInThisNode = FALSE;
			break;
		}
	}

	if (bAllInThisNode)
	{
		m_pBoundingBox->_DEBUG_Render(&matWorld,0x4000aaff);
		return;
	}

	for(int iChild=0;iChild<8;iChild++)	
	{
		m_pChildNodeArray[iChild]._DEBUG_RenderBoundingBox(pFrustum);
	}
}



/*
* 功能: 得到指定材质需要播放的足底特效ID
* 摘要: -
* 参数: usType - 材质类型枚举
* 返回值: 与材质对应的特效ID
* 作者: lpf
* 创建日期: 2008.9.26
* 修改日志:
*	2009.01.15 - lpf
*		重新设定了传入类型范围判断(因为新增加了雪地和沼泽),并将其减1
*/
long GameMap::GetFootEffectId(unsigned short usType)
{
	if (usType < 1 || usType > 8)
		return 0;

	return m_lFootEffectIdArray[usType - 1];
}



GameBuildList * GameMap::GetBuildList(void)
{
	return m_pBuildList;
}

void GameMap::AddPointLightSource(const D3DXVECTOR3 * pLightPosition, DWORD dwLightType,float fLightRange,DWORD dwLightColor)
{
	if (m_dwLightSourceArrayUsage < MAX_MAP_LIGHT_SOURCE_ARRAY_SIZE)
	{
		tagLightSource *pLightSource = &m_pLightSourceArray[m_dwLightSourceArrayUsage];
		pLightSource->vPosition = *pLightPosition;
		pLightSource->dwColor = dwLightColor;
		pLightSource->fRange = fLightRange;
		pLightSource->dwType = dwLightType;
		m_dwLightSourceArrayUsage++;
	} 
}

void GameMap::AddPoint(DWORD dwVertexIndex,DWORD wR,DWORD wG,DWORD wB,tagLightSource *pLightSource,BYTE mask)
{
	static D3DXVECTOR3 L;
	static DWORD dwValue;
	static DWORD r,g,b;

	float fLightDistance;

	DWORD  dwDestColor = m_pVertexColorRenderArray[dwVertexIndex];

	L = m_pVertexPosArray[dwVertexIndex] - pLightSource->vPosition;

	fLightDistance = D3DXVec3Length(&L);//光源到vertex的距离

	dwValue = (DWORD)(1024.0f * (pLightSource->fRange + 1.0f) / (fLightDistance * fLightDistance * fLightDistance));//DOT

	r = (dwDestColor & 0x00FF0000) >> 16;
	g = (dwDestColor & 0x0000FF00) >> 8;
	b = (dwDestColor & 0x000000FF);

	r += wR * dwValue>>10;
	if (r > 255) r = 255;
	g += wG * dwValue>>10;
	if (g > 255) g = 255;
	b += wB * dwValue>>10;
	if (b > 255) b = 255;

	m_pVertexColorRenderArray[dwVertexIndex] = (mask<<24 )|(r<<16)|(g<<8)|b;
}
void GameMap::SubPoint(DWORD dwVertexIndex,DWORD wR,DWORD wG,DWORD wB,tagLightSource *pLightSource,BYTE mask)
{
	static D3DXVECTOR3 L;
	static DWORD dwValue;
	static DWORD r,g,b;

	float fLightDistance;

	DWORD  dwDestColor = m_pVertexColorRenderArray[dwVertexIndex];

	L = m_pVertexPosArray[dwVertexIndex] - pLightSource->vPosition;

	fLightDistance = D3DXVec3Length(&L);//光源到vertex的距离

	dwValue = (DWORD)(1024.0f * (pLightSource->fRange + 1.0f) / (fLightDistance * fLightDistance * fLightDistance)) ;//(fLightDistance * fLightDistance));//DOT

	r = (dwDestColor & 0x00FF0000) >> 16;
	g = (dwDestColor & 0x0000FF00) >> 8;
	b = (dwDestColor & 0x000000FF);

	DWORD sr = wR * dwValue>>10;
	if (sr >= r) 
		r = 0;
	else
		r = r - sr;
	DWORD sg = wG * dwValue>>10;
	if (sg >= g) 
		g = 0;
	else
		g = g - sg;
	DWORD sb = wB * dwValue>>10;
	if (sb >= b) 
		b = 0;
	else
		b = b - sb;

	m_pVertexColorRenderArray[dwVertexIndex] = (mask<<24 )|(r<<16)|(g<<8)|b;
}

void GameMap::MulPoint(DWORD dwVertexIndex,DWORD wR,DWORD wG,DWORD wB,tagLightSource *pLightSource,BYTE mask)
{
	static D3DXVECTOR3 L;
	float fLightDistance;
	DWORD r,g,b;

	DWORD  &dwDestColor = m_pVertexColorRenderArray[dwVertexIndex];

	L = m_pVertexPosArray[dwVertexIndex] - pLightSource->vPosition;

	fLightDistance = D3DXVec3Length(&L);//光源到vertex的距离

	DWORD a = (DWORD)(255.0f * (pLightSource->fRange + 1) / (fLightDistance * fLightDistance));
	if (a > 255)
		a = 255;

	r = (dwDestColor & 0x00FF0000) >> 16;
	g = (dwDestColor & 0x0000FF00) >> 8;
	b = (dwDestColor & 0x000000FF);

	r = ((wR * a) + r * (255 - a)) / 255;
	g = ((wG * a) + g * (255 - a)) / 255;
	b = ((wB * a) + b * (255 - a)) / 255;

	m_pVertexColorRenderArray[dwVertexIndex] = mask<<24|(r<<16)|(g<<8)|b;
}

void GameMap::CalculateLightSourceLighting(void)
{
	//copy color from Array to render-Array
	DWORD dwVertexIndex(0);
	for(DWORD n = 0,dwGridIndex=0; n< m_dwGridVisibleIndexCount; n++)
	{	
		dwGridIndex = m_pGridVisibleIndexArray[n];

		tagGrid &tGrid = m_pGridArray[dwGridIndex];
		dwVertexIndex = tGrid.dwVertexIndex[0];
		m_pVertexColorRenderArray[dwVertexIndex] = m_pVertexColArray[dwVertexIndex] & 0x00ffffff;
		dwVertexIndex = tGrid.dwVertexIndex[1];
		m_pVertexColorRenderArray[dwVertexIndex] = m_pVertexColArray[dwVertexIndex] & 0x00ffffff;
		dwVertexIndex = tGrid.dwVertexIndex[2];
		m_pVertexColorRenderArray[dwVertexIndex] = m_pVertexColArray[dwVertexIndex] & 0x00ffffff;
		dwVertexIndex = tGrid.dwVertexIndex[3];
		m_pVertexColorRenderArray[dwVertexIndex] = m_pVertexColArray[dwVertexIndex] & 0x00ffffff;
	}

	static D3DXVECTOR3 L;
	//计算光照
	for(DWORD n = 0; n < m_dwLightSourceArrayUsage&&n<255; n++)
	{
		BYTE mask=(BYTE)n+1;
		tagLightSource *pLightSource = m_pLightSourceArray + n;

		DWORD wR = (DWORD)( (pLightSource->dwColor & 0x00FF0000) >> 16) ;
		DWORD wG = (DWORD)( (pLightSource->dwColor & 0x0000FF00) >>  8) ;
		DWORD wB = (DWORD)(  pLightSource->dwColor & 0x000000FF)        ;

		//求出影响格子，

		int iCX = (int)pLightSource->vPosition.x;
		int iCZ = (int)pLightSource->vPosition.z;

		//循环影响格子计算点光源
		int iRadius = (int)pLightSource->fRange;

		if (pLightSource->dwType == 0)
		{
			for(int i = -iRadius; i <= iRadius;i ++)
			{
				for(int j = -iRadius; j <= iRadius;j ++)
				{
					//求出周围的格子
					int iX = i + iCX;
					int iZ = j + iCZ;
					if (iX < 0 || iZ < 0 || iX >= (int)m_dwWidth || iZ >= (int)m_dwDepth);
					else
					{
						//压入定点索引到列表
						tagGrid *pGrid = GetGrid(iX + iZ * m_dwDepth);
						for(DWORD v = 0; v < 4; v++)
						{
							DWORD dwVertexIndex = pGrid->dwVertexIndex[v]; 

							if((m_pVertexColorRenderArray[dwVertexIndex]&0xFF000000)>>24<mask)
							{
								AddPoint(dwVertexIndex,wR,wG,wB,pLightSource,mask);
							}
						}
					}
				}
			}
		}
		else
		{
			for(int i = -iRadius; i <= iRadius;i ++)
			{
				for(int j = -iRadius; j <= iRadius;j ++)
				{
					//求出周围的格子
					int iX = i + iCX;
					int iZ = j + iCZ;
					if (iX < 0 || iZ < 0 || iX >= (int)m_dwWidth || iZ >= (int)m_dwDepth);
					else
					{
						//压入定点索引到列表
						tagGrid *pGrid = GetGrid(iX + iZ * m_dwDepth);
						for(DWORD v = 0; v < 4; v++)
						{
							DWORD dwVertexIndex = pGrid->dwVertexIndex[v]; 

							if((m_pVertexColorRenderArray[dwVertexIndex]&0xFF000000)>>24<mask)
							{
								SubPoint(dwVertexIndex,wR,wG,wB,pLightSource,mask);
							}
						}
					}
				}
			}
		}
	}
	m_dwLightSourceArrayUsage = 0;
}

void GameMap::SetTargetHideGridIndex(DWORD dwGridIndex)
{
	m_dwTargetHideGridIndex = dwGridIndex;
}

DWORD GameMap::GetGridColor(int x ,int z) const
{
	DWORD dwOffset = x + z * m_dwWidth;
	dwOffset = dwOffset % m_dwGridCount;

	tagGrid &tGrid = m_pGridArray[dwOffset];
	return 0xFF000000 | m_pVertexColorRenderArray[tGrid.dwVertexIndex[0]];
}


// 下面的函数用于计算对象的法向量 
void GameMap::ComputeNormals() 
{ 
	if (!m_dwGridCount||!m_dwVertexCount||!m_dwDepth||!m_dwWidth||m_dwGridCount > 1048576)
	{
		MessageBox(NULL,"open map file faild!","Error",MB_OK);
		return;
	}
	D3DXVECTOR3 vVector1, vVector2, vNormal, vPoly[3]; 

	// 分配需要的存储空间 
	D3DXVECTOR3 *pNormals = new D3DXVECTOR3 [m_dwGridCount*2]; 
	D3DXVECTOR3 *pTempNormals = new D3DXVECTOR3 [m_dwGridCount*2]; 
	m_pGridNormalArray = new D3DXVECTOR3 [m_dwGridCount]; 

	// 遍历对象的所有面 
	int i = 0;
	for(DWORD n = 0; n< m_dwGridCount; n++)
	{	
		tagGrid &tGrid = m_pGridArray[n];

		//第一个三角形
		vPoly[0] = m_pVertexPosArray[tGrid.dwVertexIndex[0]];
		vPoly[1] = m_pVertexPosArray[tGrid.dwVertexIndex[3]];
		vPoly[2] = m_pVertexPosArray[tGrid.dwVertexIndex[2]];
		// 计算面的法向量 
		vVector1 = Vector(vPoly[0], vPoly[2]); // 获得多边形的矢量 
		vVector2 = Vector(vPoly[2], vPoly[1]); // 获得多边形的第二个矢量 
		D3DXVec3Cross(&vNormal, &vVector2, &vVector1);// 获得两个矢量的叉积 
		pTempNormals[i] = vNormal; // 保存非规范化法向量 
		D3DXVec3Normalize(&vNormal, &vNormal);// 规范化获得的叉积
		pNormals[i] = vNormal; // 将法向量添加到法向量列表中
		i++;

		//第二个三角形
		vPoly[0] = m_pVertexPosArray[tGrid.dwVertexIndex[0]];
		vPoly[1] = m_pVertexPosArray[tGrid.dwVertexIndex[2]];
		vPoly[2] = m_pVertexPosArray[tGrid.dwVertexIndex[1]];
		// 计算面的法向量 
		vVector1 = Vector(vPoly[0], vPoly[2]); // 获得多边形的矢量 
		vVector2 = Vector(vPoly[2], vPoly[1]); // 获得多边形的第二个矢量 
		D3DXVec3Cross(&vNormal, &vVector2, &vVector1);// 获得两个矢量的叉积 
		pTempNormals[i] = vNormal; // 保存非规范化法向量 
		D3DXVec3Normalize(&vNormal, &vNormal);// 规范化获得的叉积
		pNormals[i] = vNormal; // 将法向量添加到法向量列表中
		m_pGridNormalArray[n] =(pNormals[i] + pNormals[i - 1] ) / 2;
		i++;

	}

	// 下面求顶点法向量------ ADD BY 彭谦---------
	D3DXVECTOR3 vSum(0.0f, 0.0f, 0.0f); 
	D3DXVECTOR3 vZero = vSum; 
	int shared=0; 

	// 遍历所有的顶点 
#pragma warning (push)
#pragma warning (disable:4018)
	for (i = 0; i < m_dwVertexCount; i++) 
#pragma  warning (pop)
	{ 
#pragma warning (push)
#pragma warning (disable:4244)
		int x = (float)(i%(m_dwWidth + 1));
		int z = (float)(i/(m_dwWidth + 1));
#pragma warning (pop)
		int j1 = x - 1 + (z - 1) * m_dwWidth;
#pragma warning (push)
#pragma warning (disable:4018)
		if (j1 >= 0&&j1 <= m_dwGridCount)
#pragma warning (pop)
		{
			shared++;
		}
		else
		{
			j1 = 0;
		}
		int j2 = x     + (z - 1) * m_dwWidth;
#pragma warning (push)
#pragma warning (disable:4018)
		if (j2 >= 0&&j2 <= m_dwGridCount)
#pragma warning (pop)
		{
			shared++;
		}
		else
		{
			j2 = 0;
		}
		int j3 = x - 1 + z       * m_dwWidth;
#pragma warning (push)
#pragma warning (disable:4018)
		if (j3 >= 0&&j3 <= m_dwGridCount)
#pragma warning (pop)
		{
			shared++;
		}
		else
		{
			j3 = 0;
		}
		int j4 = x     + z       * m_dwWidth;
#pragma  warning (push)
#pragma warning (disable:4018)
		if (j4 >= 0&&j4 <= m_dwGridCount)
#pragma warning (pop)
		{
			shared++;
		}
		else
		{
			j4 = 0;
		}
		if (shared == 0)
		{
			m_pVertexNormalArray[i] = m_pGridNormalArray[j4];

		}
		else
		{
#pragma warning (push)
#pragma warning (disable:4244)
			m_pVertexNormalArray[i] = (m_pGridNormalArray[j1] + m_pGridNormalArray[j2] + m_pGridNormalArray[j3] + m_pGridNormalArray[j4])/shared;
#pragma warning (pop)
		}
		D3DXVec3Normalize(&m_pVertexNormalArray[i], &m_pVertexNormalArray[i]);
		vSum = vZero; 
		shared = 0; 
	} 
	// 求顶点法向量------ ADD BY 彭谦---------


	// 释放存储空间，开始下一个对象 
	delete [] pTempNormals; 
	delete [] pNormals; 

} 

GameMap::tagGrid::tagGrid()
{
	pBuild = NULL;
	//listgridindex.resize(5);
	bDirColorCounted = FALSE;


}
GameMap::tagGrid::~tagGrid()
{
	listgridindex.clear();
	if (vecpDirColorBuffer.empty())
	{
		return;
	}
	std::vector<D3DCOLOR*>::iterator it = vecpDirColorBuffer.begin();
	for (;it != vecpDirColorBuffer.end();it++)
	{
		SAFEDELETEARRAY((*it));
	}
	vecpDirColorBuffer.clear();


}
