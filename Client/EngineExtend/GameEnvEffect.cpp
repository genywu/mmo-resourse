#include "StdAfx.h"

#include "../Engine/console.h"
#include "../Engine/utility.h"
#include "../Engine/render.h"

#include "gameenveffect.h"
#include "gamegraphics2d.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


GameBaseEnvEffect *GameBaseEnvEffect::CreateEnvEffect(const char *szFileName)
{
	//读取配置
	utility::File file;
	if (!file.Open(szFileName))
	{
		MessageBox(NULL,szFileName,"读取文件失败",MB_OK);
		return NULL;
	}
	utility::Text  text;
	text.Create(&file);
	file.Close();


	char szPath[1024];

	utility::File::GetFilePath(szFileName,szPath);

	GameBaseEnvEffect * p(NULL); 

	utility::Text::Node *pRoot = text.GetRootNode();
	//读取类型
	static const char * szEnvEffectTypeString[] = {"flow","particle"};
    switch(pRoot->GetVar("type")->GetEnumValue(szEnvEffectTypeString,2))
	{
	case 0:
		//流云
		p = new GameEnvEffect_Flow;
		if (!p->Create(pRoot->GetChildNode("parameter"),szPath))
		{
			MessageBox(NULL,szPath,"环境效果[Flow]建立失败",MB_OK);
			SAFEDELETE(p);
		}
		break;
	case 1:
		//粒子
		p = new GameEnvEffect_Particle;
		if (!p->Create(pRoot->GetChildNode("parameter"),szPath))
		{
			MessageBox(NULL,szPath,"环境效果[Particle]建立失败",MB_OK);
			SAFEDELETE(p);
		}
		break;
	}


	text.Destroy();

	return p;
}


GameEnvEffect_Flow::GameEnvEffect_Flow()
{
	m_pLayerArray = NULL;
}
GameEnvEffect_Flow::~GameEnvEffect_Flow()
{
}

BOOL GameEnvEffect_Flow::Create(utility::Text::Node *pNode,char *szResourcePath)
{
	BOOL bResult = TRUE;	
	render::Interface *pInterface = render::Interface::GetInstance();
	char szName[1024];

	m_dwLayerCount = pNode->GetChildCount();
	m_pLayerArray = new tagLayer[m_dwLayerCount];
	for(DWORD n = 0; n < m_dwLayerCount;n++)
	{
		utility::Text::Node *pLayerNode = pNode->GetChildNode(n);
		//读取参数,贴图
		_snprintf(szName,1024,"%s\\%s",szResourcePath,pLayerNode->GetVar("texture")->GetStringValue());
		m_pLayerArray[n].dwTexureHandle = pInterface->CreateTexture(szName);
		if (m_pLayerArray[n].dwTexureHandle == NULL)
		{
			MessageBox(NULL,szName,"环境效果纹理建立失败",MB_OK);
			bResult = FALSE;
		}
		//流动方向，和速度
		static const char *szUVType[] = {"normal","skew"};
		m_pLayerArray[n].dwType = pLayerNode->GetVar("type")->GetEnumValue(szUVType,2);
		m_pLayerArray[n].fU = pLayerNode->GetVar("u")->GetFloatValue();
		m_pLayerArray[n].fV = pLayerNode->GetVar("v")->GetFloatValue();
		m_pLayerArray[n].dwTimeCycle = (DWORD)pLayerNode->GetVar("cycle")->GetIntValue();
		m_pLayerArray[n].fUVScaleRatio = pLayerNode->GetVar("scale")->GetFloatValue();
	}  

	return bResult;
}

void GameEnvEffect_Flow::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	for(DWORD n = 0;n < m_dwLayerCount;n++)
	{
		pInterface->DestroyTexture(m_pLayerArray[n].dwTexureHandle);
	}
	
	SAFEDELETEARRAY(m_pLayerArray);

}

void GameEnvEffect_Flow::Update(D3DXVECTOR3 *pPostion)
{
	DWORD dwCurrentTime = timeGetTime();
	
	for(DWORD n = 0; n < m_dwLayerCount; n++)
	{
		m_pLayerArray[n].dwTimeElapse = dwCurrentTime - m_pLayerArray[n].dwStartTime;
		if (m_pLayerArray[n].dwTimeElapse >= m_pLayerArray[n].dwTimeCycle)
		{
			//循环一次
			m_pLayerArray[n].dwTimeElapse = m_pLayerArray[n].dwTimeCycle;
			m_pLayerArray[n].dwStartTime = dwCurrentTime;
		}
	}
}

void GameEnvEffect_Flow::Render(const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX *pBillboardMatrixY)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();

	for(DWORD n = 0; n < m_dwLayerCount;n++)
	{
		tagLayer &tLayer = m_pLayerArray[n];
		//计算出当前的偏移
		float fRatio = (float)tLayer.dwTimeElapse / (float)tLayer.dwTimeCycle;
		float fU = fRatio * tLayer.fU;
		float fV = fRatio * tLayer.fV;
		if (tLayer.dwType == 0)
		{
			pGameGraphics2D->DrawImage(0,0,pInterface->GetWndWidth(),pInterface->GetWndHeight(),fU,fV,m_pLayerArray[n].fUVScaleRatio,m_pLayerArray[n].dwTexureHandle);
		}
		else
		{
			pGameGraphics2D->DrawImageSkew(0,0,pInterface->GetWndWidth(),pInterface->GetWndHeight(),fU,fV,m_pLayerArray[n].fUVScaleRatio,m_pLayerArray[n].dwTexureHandle);
		}
	}
}


GameEnvEffect_Particle::GameEnvEffect_Particle()
{
	m_dwShaderGroupHandle = 0xFFFFFFFF;
	m_dwLastTime = 0;
	m_dwBillboardType = 0;
}

GameEnvEffect_Particle::~GameEnvEffect_Particle()
{


}
BOOL GameEnvEffect_Particle::Create(utility::Text::Node *pNode,char *szResourcePath)
{
	//读取粒子效果,从纹理创建一个SHADERGROUP
	char szTemp[1024];
	_snprintf(szTemp,1024,"texture\n{\nlayer0\n{\n0 = %s;\n}\nlayer1\n{\n0 = noTexture;\n}\n}\n\
                     shader\n{\nsdr0\n{\nstream      = poscoltex0tex1;\ntwoside     = true;\n\
                     alphatest   = transparent;\nalphablend  = additive;\nlayer0\n{\nColorOp   = Modulate;\
					 \nColorArg1 = Texture;\nColorArg2 = Diffuse;\nAlphaOp   = Modulate;\nAlphaArg1 = Texture;\
                     AlphaArg2 = Diffuse;\ntex = 0;\n}\nlayer1\n{\nColorOp    = Disable;\nAlphaOp    = Disable;\
					 tex = 0;\n}\n}\n",pNode->GetVar("texture")->GetStringValue());
	
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(szTemp,szResourcePath,0);
	if (m_dwShaderGroupHandle == 0xFFFFFFFF)
	{
		OutputError("GameEnvEffect_Particle::Create() failed!\n");
		return FALSE;
	}
	m_dwColumns = (DWORD)pNode->GetVar("columns")->GetIntValue();
	m_dwRows    = (DWORD)pNode->GetVar("rows")->GetIntValue();


	const char *szBillboardTypeString[] = {"none","full","vertical"};
	m_dwBillboardType = pNode->GetVar("billboard")->GetEnumValue(szBillboardTypeString,3);
	m_fScale = pNode->GetVar("scale")->GetFloatValue();

	m_vInitSpeed.x = pNode->GetVar("speedX")->GetFloatValue();
	m_vInitSpeed.y = pNode->GetVar("speedY")->GetFloatValue();
	m_vInitSpeed.z = pNode->GetVar("speedZ")->GetFloatValue();

	m_vBaseWind.x = pNode->GetVar("BaseWindX")->GetFloatValue();
	m_vBaseWind.y = pNode->GetVar("BaseWindY")->GetFloatValue();
	m_vBaseWind.z = pNode->GetVar("BaseWindZ")->GetFloatValue();

	m_vRadomWind.x = pNode->GetVar("RadomWindX")->GetFloatValue();
	m_vRadomWind.y = pNode->GetVar("RadomWindY")->GetFloatValue();
	m_vRadomWind.z = pNode->GetVar("RadomWindZ")->GetFloatValue();

	m_dwMinLifeTime = (DWORD)pNode->GetVar("MinLifeTime")->GetIntValue();
	m_dwMaxLifeTime = (DWORD)pNode->GetVar("MaxLifeTime")->GetIntValue();

	m_bRotation = pNode->GetVar("rotation")->GetBOOLValue();
	m_dwRotationBaseInterval  = (DWORD)pNode->GetVar("rotationBaseInterval")->GetIntValue();
	m_dwRotationRadomInterval = (DWORD)pNode->GetVar("rotationRadomInterval")->GetIntValue();
	m_iRotationAngle = pNode->GetVar("rotationAngle")->GetIntValue();

	m_bLight = pNode->GetVar("light")->GetBOOLValue();
	m_dwLightMinInterval = (DWORD)pNode->GetVar("lightmininterval")->GetIntValue();
	m_dwLightMaxInterval = (DWORD)pNode->GetVar("lightmaxinterval")->GetIntValue();
	m_fLightMinRatio = pNode->GetVar("lightminInterval")->GetFloatValue();
	m_fLightMaxRatio = pNode->GetVar("lightmaxInterval")->GetFloatValue();

	m_fRadius = pNode->GetVar("radius")->GetFloatValue();
	const char *szAlphaModeTypeString[] = {"test","blend"};
	m_dwAlphaMode = pNode->GetVar("alphamode")->GetEnumValue(szAlphaModeTypeString,2);

	//读取粒子数量
	m_dwParticleCount = (DWORD)pNode->GetVar("number")->GetIntValue();
	m_pParticleArray = new tagParticle[m_dwParticleCount];
	ZeroMemory(m_pParticleArray,sizeof(tagParticle) * m_dwParticleCount);	

	return TRUE;
}
void GameEnvEffect_Particle::Destroy()
{
	//销毁
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
	SAFEDELETEARRAY(m_pParticleArray);
}
void GameEnvEffect_Particle::Update(D3DXVECTOR3 *pPostion)
{
	DWORD dwCurrentTime = timeGetTime();
	DWORD dwTimeElapse = dwCurrentTime - m_dwLastTime;
	m_dwLastTime = dwCurrentTime;
	//刷新
	for(DWORD n = 0; n < m_dwParticleCount;n++)
	{
		tagParticle &tParticle = m_pParticleArray[n];

		if (tParticle.dwLifeTime == 0)
		{
			tParticle.dwColor = 0xFFFFFFFF;
			tParticle.vPosition.x = pPostion->x + (m_fRadius * 2) * (float)rand() / (float)RAND_MAX - m_fRadius;
			tParticle.vPosition.y = pPostion->y + (m_fRadius * 2) * (float)rand() / (float)RAND_MAX - m_fRadius;
			tParticle.vPosition.z = pPostion->z + (m_fRadius * 2) * (float)rand() / (float)RAND_MAX - m_fRadius;

			tParticle.vSpeed.x = m_vInitSpeed.x * (float)rand() / (float)RAND_MAX - m_vInitSpeed.x/2;
			tParticle.vSpeed.y = m_vInitSpeed.y * (float)rand() / (float)RAND_MAX - m_vInitSpeed.y/2;
			tParticle.vSpeed.z = m_vInitSpeed.z * (float)rand() / (float)RAND_MAX - m_vInitSpeed.z/2;

			tParticle.dwLifeTime = m_dwMinLifeTime + rand() % (m_dwMaxLifeTime - m_dwMinLifeTime);
			
			tParticle.dwTextureRectIndex = rand() % (m_dwRows * m_dwColumns);

			tParticle.iAngleX = 0;
			tParticle.iAngleY = 0;
			tParticle.iAngleZ = 0;
			tParticle.dwLastRotationTime = dwCurrentTime;
			
			tParticle.vRadomWind.x = 0.0f;
			tParticle.vRadomWind.y = 0.0f;
			tParticle.vRadomWind.z = 0.0f;
		}
		else
		{
			tParticle.dwLifeTime = (tParticle.dwLifeTime < dwTimeElapse) ? 0 : tParticle.dwLifeTime - dwTimeElapse;
			tParticle.vPosition += tParticle.vSpeed;
			tParticle.vPosition += m_vBaseWind;

			float fRatio = (float)dwTimeElapse * 0.001f;

			tParticle.vRadomWind.x += (m_vRadomWind.x * (float)rand() / (float)RAND_MAX - m_vRadomWind.x / 2) * fRatio;
			tParticle.vRadomWind.y += (m_vRadomWind.y * (float)rand() / (float)RAND_MAX - m_vRadomWind.y / 2) * fRatio;
			tParticle.vRadomWind.z += (m_vRadomWind.z * (float)rand() / (float)RAND_MAX - m_vRadomWind.z / 2) * fRatio;

			tParticle.vPosition += tParticle.vRadomWind;
			
			if (m_bRotation)
			{
				if (dwCurrentTime - tParticle.dwLastRotationTime > (m_dwRotationBaseInterval + rand() % m_dwRotationRadomInterval))
				{
					tParticle.iAngleX += (rand() % m_iRotationAngle) - (m_iRotationAngle / 2);					
					tParticle.iAngleY += (rand() % m_iRotationAngle) - (m_iRotationAngle / 2);
					tParticle.iAngleZ += (rand() % m_iRotationAngle) - (m_iRotationAngle / 2);
					tParticle.dwLastRotationTime = dwCurrentTime;
				}
			}
			if (m_bLight)
			{
				DWORD dwTimeTemp = m_dwLightMinInterval;
				if (m_dwLightMaxInterval > m_dwLightMinInterval)
				{
					dwTimeTemp += rand() % (m_dwLightMaxInterval - m_dwLightMinInterval);
				}

				if (dwCurrentTime - tParticle.dwLastLightTime > dwTimeTemp)
				{
					DWORD dwColor = (DWORD)(255.0f * ((m_fLightMaxRatio - m_fLightMinRatio) * (float)rand() / (float)RAND_MAX + m_fLightMinRatio));
					tParticle.dwColor = D3DCOLOR_ARGB(0xFF,dwColor,dwColor,dwColor);
					tParticle.dwLastLightTime = dwCurrentTime;
				}
			}
		}
	}	
}

void GameEnvEffect_Particle::Render(const D3DXMATRIX *pBillboardMatrix,const D3DXMATRIX *pBillboardMatrixY)
{
	D3DXMATRIX mBM = *pBillboardMatrix;
	D3DXMATRIX mBMY = *pBillboardMatrixY;
	static D3DXVECTOR3 pos0[6] = { D3DXVECTOR3(-0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,-0.5f,0.0f),
		D3DXVECTOR3(-0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,-0.5f,0.0f),D3DXVECTOR3(-0.5f,-0.5f,0.0f) };		
	static D3DXVECTOR2 uv0[6] = { D3DXVECTOR2(0,0),D3DXVECTOR2(1,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,1)};

	/*static D3DXVECTOR3 position[20000];
	static D3DXVECTOR2 uv[20000];
	static DWORD color[20000];   	*/
	static render::Interface::Layer3D::_tagTriangle triangle[20000];
	static D3DXMATRIX  matWorld,matTrans,matScale,matRotX,matRotY,matRotZ,matRot;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	//渲染
	DWORD usenum = 0;
	for(DWORD n = 0; n < m_dwParticleCount;n++)
	{
		tagParticle * pParticle = &m_pParticleArray[n];

		DWORD dwColor = pParticle->dwColor;

		float x = (float)(pParticle->dwTextureRectIndex % m_dwRows);
		float y = (float)(pParticle->dwTextureRectIndex / m_dwRows);
		float w = 1.0f / (float)m_dwRows;
		float h = 1.0f / (float)m_dwRows;
		float u = x * w;
		float v = y * h;
	

		D3DXMatrixScaling(&matScale,m_fScale,m_fScale,m_fScale);

		if (m_bRotation)
		{
			D3DXMatrixRotationX(&matRotX,D3DXToRadian(pParticle->iAngleX));
			D3DXMatrixRotationY(&matRotY,D3DXToRadian(pParticle->iAngleY));
			D3DXMatrixRotationZ(&matRotZ,D3DXToRadian(pParticle->iAngleZ));
			D3DXMatrixMultiply(&matRot,&matRotX,&matRotY);
			D3DXMatrixMultiply(&matRot,&matRot,&matRotZ);
		}
		else
		{
			D3DXMatrixIdentity(&matRot);
		}
				
		switch(m_dwBillboardType)
		{
		case 0://无
			D3DXMatrixTranslation(&matTrans,pParticle->vPosition.x,pParticle->vPosition.y,pParticle->vPosition.z);
			D3DXMatrixMultiply(&matWorld,&matScale,&matRot);
			D3DXMatrixMultiply(&matWorld,&matWorld,&matTrans);
			break;
		case 1://有
			mBM._41 = pParticle->vPosition.x;
			mBM._42 = pParticle->vPosition.y;
			mBM._43 = pParticle->vPosition.z;
			D3DXMatrixMultiply(&matWorld,&matScale,&mBM);
			break;
		case 2://垂直
			mBMY._41 = pParticle->vPosition.x;
			mBMY._42 = pParticle->vPosition.y;
			mBMY._43 = pParticle->vPosition.z;
			D3DXMatrixMultiply(&matWorld,&matScale,&mBMY);
			break;
		}
		
		for(int i = 0; i < 6; i++)
		{
			D3DXVec3TransformCoord(&triangle[usenum + (i > 2?1:0)].pos[i%3],&pos0[i],&matWorld);  
			triangle[usenum + (i > 2?1:0)].uv0[i%3].x = u + w * uv0[i].x;
			triangle[usenum + (i > 2?1:0)].uv0[i%3].y = v + h * uv0[i].y;
			triangle[usenum + (i > 2?1:0)].color[i%3] = dwColor;
		}
		if (usenum > 19980)
		{
			pLayer3D->SendTriangleToCache(m_dwShaderGroupHandle,0,usenum ,triangle,m_dwAlphaMode);
			usenum = -2;

		}
		usenum += 2;
	}
	pLayer3D->SendTriangleToCache(m_dwShaderGroupHandle,0,usenum ,triangle,m_dwAlphaMode);
}
