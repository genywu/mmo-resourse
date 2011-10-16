
#include "stdafx.h"
#include <algorithm>
#include "utility.h"
#include "console.h"
#include "render.h"
#include "particle.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_SINGLETON(particle::Manager);
particle::Emitter::Emitter()
{
	m_pParticleUsed = NULL;
	m_pParticleFree = NULL;

	//渲染头
	m_pParticleRenderHead = NULL;
	m_bReadyToDie = false;
	m_bDrawToScreen = false;
	m_bInDraw = false;
	m_bRenderToTarger = false;
	m_bDied = false;

}
particle::Emitter::~Emitter()
{
	tagParticle *pParticle,*p;

	pParticle = m_pParticleUsed;	
	while(pParticle)
	{
		p = pParticle;
		pParticle = pParticle->pNext;
		SAFEDELETE(p);
	}

	pParticle = m_pParticleFree;	
	while(pParticle)
	{
		p = pParticle;
		pParticle = pParticle->pNext;
		SAFEDELETE(p);
	}

}
void particle::Emitter::CloneEmitter(Emitter* pEmiiter)
{
	m_tParameter = pEmiiter->m_tParameter;
	strncpy(m_pszShaderFileName,pEmiiter->m_pszShaderFileName,MAX_PATH);
	m_dwShaderGroupHandle = pEmiiter->m_dwShaderGroupHandle;
	m_eLoad = pEmiiter->m_eLoad;
}
//CLASS__(particle::Emitter)_____
void particle::Emitter::CreateFromFile(const char *pszFileName)
{
	static char pszResourcePath[_MAX_PATH];

	utility::File::GetFilePath(pszFileName,pszResourcePath);

	utility::File file;
	if (!file.Open(pszFileName))
	{
		OutputConsole("error: CreateFromFile() \"%s\" failed!\n",pszFileName);
		ASSERT(0);
	}

	char *szParameter = new char[file.GetLength() + 1];
	memcpy(szParameter,file.GetData(),file.GetLength());
	szParameter[file.GetLength()] = 0;;
	file.Close();

	Create(szParameter,pszResourcePath);

	SAFEDELETEARRAY(szParameter);

}

void particle::Emitter::Create(CRFile *pFile)
{
	if(NULL == pFile)	return;
	char *szParameter = new char[pFile->GetDatalen() + 1];
	memcpy(szParameter,pFile->GetData(),pFile->GetDatalen());
	szParameter[pFile->GetDatalen()] = 0;

	CreateParam(szParameter);
	m_bDrawToScreen = false;
	m_bRenderToTarger = false;
	m_bInDraw = false;


	SAFEDELETEARRAY(szParameter);
}

void particle::Emitter::Create(const char *pszParameter,const char *pszResourcePath)
{
	ZeroMemory(&m_tParameter,sizeof(m_tParameter));
	//建立Shader
	utility::Text cScript;
	cScript.Create(pszParameter);

	//读取SHADER
	utility::Text::Node *pShaderNode = cScript.GetRootNode()->GetChildNode("shader");

	char szShaderFileName[_MAX_PATH];
	_snprintf(szShaderFileName,_MAX_PATH,"%s\\%s",pszResourcePath,pShaderNode->GetVar("file")->GetStringValue());

	utility::File fileShader;	
	if (!fileShader.Open(szShaderFileName))
	{
		OutputConsole("error: open \"%s\" failed.\n",szShaderFileName);
		ASSERT(0);
		return;
	}
	char *pszShaderText = new char [fileShader.GetLength() + 1];
	memcpy(pszShaderText,fileShader.GetData(),fileShader.GetLength());
	pszShaderText[fileShader.GetLength()] = 0;	
	fileShader.Close();
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pszShaderText,pszResourcePath);
	SAFEDELETE(pszShaderText);

	//读取参数
	utility::Text::Node *pParameterNode = cScript.GetRootNode()->GetChildNode("parameter");
	strncpy(m_tParameter.szName,pParameterNode->GetVar("name")->GetStringValue(),MAX_EMITTER_NAME - 1);
	m_tParameter.szName[MAX_EMITTER_NAME - 1] = 0;	

	utility::Text::Variable *pTempVar(NULL);
	//类型
	static const char * szPTypeString[] = {"quad","belt"};
	m_tParameter.eParticleType     = (PARTICLE_TYPE)pParameterNode->GetVar("type")->GetEnumValue(szPTypeString,2);
	//BillBoard 类型标志
	m_tParameter.bBillboard		   = pParameterNode->GetVar("billboard")->GetBOOLValue();
	pTempVar					   = pParameterNode->GetVar("billboard_y");
	m_tParameter.bBillboardY	   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("followedowner");
	m_tParameter.bfollowedowner	   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("followedemitter");
	m_tParameter.bfollowedEmitter  = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("transinownzone");
	m_tParameter.btransinownzone   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("staytofirstframe");
	m_tParameter.bstaytofirstframe = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	m_tParameter.fAngleX		   = pParameterNode->GetVar("angleX")? D3DXToRadian(pParameterNode->GetVar("angleX")->GetFloatValue()) : 0.0f;
	m_tParameter.fAngleY		   = pParameterNode->GetVar("angleY")? D3DXToRadian(pParameterNode->GetVar("angleY")->GetFloatValue()) : 0.0f;
	m_tParameter.fAngleZ		   = pParameterNode->GetVar("angleZ")? D3DXToRadian(pParameterNode->GetVar("angleZ")->GetFloatValue()) : 0.0f;

	m_tParameter.fSelfRoteSpeedX   = pParameterNode->GetVar("selfroatSpeedX")? pParameterNode->GetVar("selfroatSpeedX")->GetIntValue() : 0.0f;
	m_tParameter.fSelfRoteSpeedY   = pParameterNode->GetVar("selfroatSpeedY")? pParameterNode->GetVar("selfroatSpeedY")->GetIntValue() : 0.0f;
	m_tParameter.fSelfRoteSpeedZ   = pParameterNode->GetVar("selfroatSpeedZ")? pParameterNode->GetVar("selfroatSpeedZ")->GetIntValue() : 0.0f;

	m_tParameter.fInitVelocity     = pParameterNode->GetVar("velocity")->GetFloatValue();
	m_tParameter.dwLifeTimeLength  = (DWORD)pParameterNode->GetVar("lifetime")->GetIntValue();
	m_tParameter.iInitT            = pParameterNode->GetVar("t")->GetIntValue();
	m_tParameter.fInitWidth        = pParameterNode->GetVar("width")->GetFloatValue();
	m_tParameter.fInitHeight       = pParameterNode->GetVar("height")->GetFloatValue();
	m_tParameter.fInitWidthRange   = (pParameterNode->GetVar("width_range")) ? pParameterNode->GetVar("width_range")->GetFloatValue() : 0.0f;
	m_tParameter.fInitHeightRange   = (pParameterNode->GetVar("height_range"))? pParameterNode->GetVar("height_range")->GetFloatValue(): 0.0f;
	m_tParameter.m_iroatdir         = (pParameterNode->GetVar("roatdir"))? pParameterNode->GetVar("roatdir")->GetIntValue(): 0.0f;
	m_tParameter.m_iroatspeed       = (pParameterNode->GetVar("roatspeed"))? pParameterNode->GetVar("roatspeed")->GetIntValue(): 0.0f;
	//读取发射参数
	utility::Text::Node *pEmitNode = pParameterNode->GetChildNode("emit");
	m_tParameter.dwEmitInterval = (DWORD)pEmitNode->GetVar("interval")->GetIntValue();
	m_tParameter.dwEmitNumber   = (DWORD)pEmitNode->GetVar("number")->GetIntValue();

	//读取可见列表
	utility::Text::Node *pVisibilityNode = pParameterNode->GetChildNode("visibility");
	m_tParameter.dwVisibilityFrameCount = pVisibilityNode->GetVarCount();
	if (m_tParameter.dwVisibilityFrameCount)
	{
		m_tParameter.pfVisibilityFrameArray = new float [m_tParameter.dwVisibilityFrameCount];
		for(DWORD n = 0; n < pVisibilityNode->GetVarCount();n++)
		{
			m_tParameter.pfVisibilityFrameArray[n] = pVisibilityNode->GetVar(n)->GetFloatValue();
		}
	}
	//颜色列表
	utility::Text::Node *pColorNode = pParameterNode->GetChildNode("color");
	m_tParameter.dwColorFrameCount = pColorNode->GetVarCount();
	if (m_tParameter.dwColorFrameCount)
	{
		m_tParameter.pdwColorFrameArray = new DWORD [m_tParameter.dwColorFrameCount];
		for(DWORD n = 0; n < pColorNode->GetVarCount();n++)
		{
			m_tParameter.pdwColorFrameArray[n] = pColorNode->GetVar(n)->GetHexValue();
		}
	}

	//加速 
	utility::Text::Node *pAccNode = pParameterNode->GetChildNode("acceleration");
	m_tParameter.vAcceleration.x = pAccNode->GetVar("x")->GetFloatValue();
	m_tParameter.vAcceleration.y = pAccNode->GetVar("y")->GetFloatValue();
	m_tParameter.vAcceleration.z = pAccNode->GetVar("z")->GetFloatValue();
	//发射角度
	utility::Text::Node *pPitchNode = pParameterNode->GetChildNode("pitch");
	m_tParameter.fPitchAngleStartx = pPitchNode->GetVar("start")->GetFloatValue();
	m_tParameter.fPitchAngleStarty = (pPitchNode->GetVar("startXY"))? pPitchNode->GetVar("startXY")->GetFloatValue(): 0.0f;
	m_tParameter.fPitchAngleRange = pPitchNode->GetVar("range")->GetFloatValue();


	//读取纹理动态参数                                                 	
	utility::Text::Node *pUVAnimNode = pParameterNode->GetChildNode("uvanim");
	m_tParameter.dwAnimTextureRows    = (DWORD)pUVAnimNode->GetVar("rows")->GetIntValue();
	m_tParameter.dwAnimTextureColumns = (DWORD)pUVAnimNode->GetVar("columns")->GetIntValue(); 	
	m_tParameter.dwAnimTextureTimeLength = (DWORD)pUVAnimNode->GetVar("timelength")->GetIntValue(); 	

	cScript.Destroy();
	//添加到
	/*particle::Manager *pPM = particle::Manager::GetInstance();
	pPM->Add(this);*/
	//初始化变量
	//m_dwUpdateEmitterLastTime = timeGetTime();
	m_pParticleFree = NULL;
	m_pParticleUsed = NULL;

	m_pParticleRenderHead = NULL;
}

void  particle::Emitter::CreateParam(const char *pszParameter)
{
	ZeroMemory(&m_tParameter,sizeof(m_tParameter));
	//建立Shader
	utility::Text cScript;
	cScript.Create(pszParameter);

	//读取SHADER
	utility::Text::Node *pShaderNode = cScript.GetRootNode()->GetChildNode("shader");

	_snprintf(m_pszShaderFileName,_MAX_PATH,"%s",pShaderNode->GetVar("file")->GetStringValue());

	//读取参数
	utility::Text::Node *pParameterNode = cScript.GetRootNode()->GetChildNode("parameter");
	strncpy(m_tParameter.szName,pParameterNode->GetVar("name")->GetStringValue(),MAX_EMITTER_NAME - 1);
	m_tParameter.szName[MAX_EMITTER_NAME - 1] = 0;	

	utility::Text::Variable *pTempVar(NULL);
	//类型
	static const char * szPTypeString[] = {"quad","belt"};
	m_tParameter.eParticleType     = (PARTICLE_TYPE)pParameterNode->GetVar("type")->GetEnumValue(szPTypeString,2);
	//BillBoard 类型标志
	m_tParameter.bBillboard		   = pParameterNode->GetVar("billboard")->GetBOOLValue();
	pTempVar					   = pParameterNode->GetVar("billboard_y");
	m_tParameter.bBillboardY	   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("followedowner");
	m_tParameter.bfollowedowner	   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("followedemitter");
	m_tParameter.bfollowedEmitter  = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("transinownzone");
	m_tParameter.btransinownzone   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	pTempVar					   = pParameterNode->GetVar("staytofirstframe");
	m_tParameter.bstaytofirstframe = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
	m_tParameter.fAngleX		   = pParameterNode->GetVar("angleX")? D3DXToRadian(pParameterNode->GetVar("angleX")->GetFloatValue()) : 0.0f;
	m_tParameter.fAngleY		   = pParameterNode->GetVar("angleY")? D3DXToRadian(pParameterNode->GetVar("angleY")->GetFloatValue()) : 0.0f;
	m_tParameter.fAngleZ		   = pParameterNode->GetVar("angleZ")? D3DXToRadian(pParameterNode->GetVar("angleZ")->GetFloatValue()) : 0.0f;

	m_tParameter.fSelfRoteSpeedX   = pParameterNode->GetVar("selfroatSpeedX")? pParameterNode->GetVar("selfroatSpeedX")->GetIntValue() : 0.0f;
	m_tParameter.fSelfRoteSpeedY   = pParameterNode->GetVar("selfroatSpeedY")? pParameterNode->GetVar("selfroatSpeedY")->GetIntValue() : 0.0f;
	m_tParameter.fSelfRoteSpeedZ   = pParameterNode->GetVar("selfroatSpeedZ")? pParameterNode->GetVar("selfroatSpeedZ")->GetIntValue() : 0.0f;

	m_tParameter.fInitVelocity     = pParameterNode->GetVar("velocity")->GetFloatValue();
	m_tParameter.dwLifeTimeLength  = (DWORD)pParameterNode->GetVar("lifetime")->GetIntValue();
	m_tParameter.iInitT            = pParameterNode->GetVar("t")->GetIntValue();
	m_tParameter.fInitWidth        = pParameterNode->GetVar("width")->GetFloatValue();
	m_tParameter.fInitHeight       = pParameterNode->GetVar("height")->GetFloatValue();
	m_tParameter.fInitWidthRange   = (pParameterNode->GetVar("width_range")) ? pParameterNode->GetVar("width_range")->GetFloatValue() : 0.0f;
	m_tParameter.fInitHeightRange   = (pParameterNode->GetVar("height_range"))? pParameterNode->GetVar("height_range")->GetFloatValue(): 0.0f;
	m_tParameter.m_iroatdir         = (pParameterNode->GetVar("roatdir"))? pParameterNode->GetVar("roatdir")->GetIntValue(): 0.0f;
	m_tParameter.m_iroatspeed       = (pParameterNode->GetVar("roatspeed"))? pParameterNode->GetVar("roatspeed")->GetIntValue(): 0.0f;
	//读取发射参数
	utility::Text::Node *pEmitNode = pParameterNode->GetChildNode("emit");
	m_tParameter.dwEmitInterval = (DWORD)pEmitNode->GetVar("interval")->GetIntValue();
	m_tParameter.dwEmitNumber   = (DWORD)pEmitNode->GetVar("number")->GetIntValue();

	//读取可见列表
	utility::Text::Node *pVisibilityNode = pParameterNode->GetChildNode("visibility");
	m_tParameter.dwVisibilityFrameCount = pVisibilityNode->GetVarCount();
	if (m_tParameter.dwVisibilityFrameCount)
	{
		m_tParameter.pfVisibilityFrameArray = new float [m_tParameter.dwVisibilityFrameCount];
		for(DWORD n = 0; n < pVisibilityNode->GetVarCount();n++)
		{
			m_tParameter.pfVisibilityFrameArray[n] = pVisibilityNode->GetVar(n)->GetFloatValue();
		}
	}
	//颜色列表
	utility::Text::Node *pColorNode = pParameterNode->GetChildNode("color");
	m_tParameter.dwColorFrameCount = pColorNode->GetVarCount();
	if (m_tParameter.dwColorFrameCount)
	{
		m_tParameter.pdwColorFrameArray = new DWORD [m_tParameter.dwColorFrameCount];
		for(DWORD n = 0; n < pColorNode->GetVarCount();n++)
		{
			m_tParameter.pdwColorFrameArray[n] = pColorNode->GetVar(n)->GetHexValue();
		}
	}

	//加速 
	utility::Text::Node *pAccNode = pParameterNode->GetChildNode("acceleration");
	m_tParameter.vAcceleration.x = pAccNode->GetVar("x")->GetFloatValue();
	m_tParameter.vAcceleration.y = pAccNode->GetVar("y")->GetFloatValue();
	m_tParameter.vAcceleration.z = pAccNode->GetVar("z")->GetFloatValue();
	//发射角度
	utility::Text::Node *pPitchNode = pParameterNode->GetChildNode("pitch");
	m_tParameter.fPitchAngleStartx = pPitchNode->GetVar("start")->GetFloatValue();
	m_tParameter.fPitchAngleStarty = (pPitchNode->GetVar("startXY"))? pPitchNode->GetVar("startXY")->GetFloatValue(): 0.0f;
	m_tParameter.fPitchAngleRange = pPitchNode->GetVar("range")->GetFloatValue();


	//读取纹理动态参数                                                 	
	utility::Text::Node *pUVAnimNode = pParameterNode->GetChildNode("uvanim");
	m_tParameter.dwAnimTextureRows    = (DWORD)pUVAnimNode->GetVar("rows")->GetIntValue();
	m_tParameter.dwAnimTextureColumns = (DWORD)pUVAnimNode->GetVar("columns")->GetIntValue(); 	
	m_tParameter.dwAnimTextureTimeLength = (DWORD)pUVAnimNode->GetVar("timelength")->GetIntValue();
	//读取大小变化参数                                                 	
	utility::Text::Node *pSizeNode = pParameterNode->GetChildNode("size");
	if (pSizeNode)
	{
		m_tParameter.vfsize.resize(pSizeNode->GetVarCount());
		for (int i = 0;i < m_tParameter.vfsize.size();i++)
		{
			m_tParameter.vfsize[i] = pSizeNode->GetVar(i)->GetFloatValue();
		}

	}



	cScript.Destroy();
	//添加到
	/*particle::Manager *pPM = particle::Manager::GetInstance();
	pPM->Add(this);*/
	//初始化变量
	//m_dwUpdateEmitterLastTime = timeGetTime();
	m_pParticleFree = NULL;
	m_pParticleUsed = NULL;

	m_pParticleRenderHead = NULL;
}

BOOL particle::Emitter::CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	//get shader group handle
	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pFile,dwTextureProcision);
	return TRUE;
}

void particle::Emitter::Destroy()
{	
	//TODo
	tagParticle *pParticle,*p;

	pParticle = m_pParticleUsed;	
	while(pParticle)
	{
		p = pParticle;
		pParticle = pParticle->pNext;
		SAFEDELETE(p);
	}

	pParticle = m_pParticleFree;	
	while(pParticle)
	{
		p = pParticle;
		pParticle = pParticle->pNext;
		SAFEDELETE(p);
	}

	particle::Manager *pPM = particle::Manager::GetInstance();
	pPM->Remove(this);  

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();

	pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);

	SAFEDELETE(m_tParameter.pdwColorFrameArray);
	SAFEDELETE(m_tParameter.pfVisibilityFrameArray);
}

particle::Emitter::tagParticle * particle::Emitter::NewParticle(void)
{
	tagParticle *pNewParticle = NULL;

	//检查是否有空余的
	if (m_pParticleFree)
	{
		pNewParticle = m_pParticleFree;
		m_pParticleFree = m_pParticleFree->pNext;
	}
	else
	{
		//没有
		pNewParticle = new tagParticle;
	}

	//加入活动列表
	pNewParticle->pNext = m_pParticleUsed;
	m_pParticleUsed = pNewParticle;    
	return pNewParticle;
}

void particle::Emitter::Update(float fscal)
{
	DWORD dwCurTime = timeGetTime();
	m_pParticleRenderHead = NULL;

	tagParticle *pParticle,**ppParticle = &m_pParticleUsed;


	//	int iC = 0,iD = 0,iR = 0;
	while(*ppParticle)
	{
		pParticle = *ppParticle;

		//计算流逝的生命时间
		DWORD dwTimeElapse = dwCurTime - pParticle->dwLifeTimeStart;
		float fT =  (m_tParameter.iInitT + (int)dwTimeElapse) * 0.001f;//计算时间参数,单位为毫秒

		//计算路程 s = s0 + v0 * t + a * t * t * 0.5f;
		D3DXVECTOR3 dpos  = pParticle->vVelocity0  * fT+ m_tParameter.vAcceleration * fT * fT * 0.5f;
		dpos *= fscal;
		if (m_tParameter.bfollowedowner||m_tParameter.bfollowedEmitter)
		{
			D3DXVec3TransformCoord(&dpos,&dpos,&m_matNowMatrix);
		}
		/*if (m_tParameter.bfollowedEmitter)
		{
		D3DXVec3TransformCoord(&dpos,&dpos,&m_matNowMatrix);
		pParticle->vPosition = pParticle->vPosition0 + dpos;
		}*/
		if (m_tParameter.btransinownzone)
		{
			D3DXVec3TransformCoord(&pParticle->vPosition,&pParticle->vSrcPosition,&m_matOwnZoneMatrix);
			pParticle->vPosition += dpos;
		}
		else
		{
			pParticle->vPosition = pParticle->vPosition0 + dpos;
		}
		if (m_tParameter.fSelfRoteSpeedX != 0)
		{
			pParticle->frotex = D3DX_PI * 2 * dwTimeElapse / m_tParameter.fSelfRoteSpeedX;
		}
		else
		{
			pParticle->frotex = 0.0f;
		}
		if (m_tParameter.fSelfRoteSpeedY != 0)
		{
			pParticle->frotey = D3DX_PI * 2 * dwTimeElapse / m_tParameter.fSelfRoteSpeedY;
		}
		else
		{
			pParticle->frotey = 0.0f;
		}
		if (m_tParameter.fSelfRoteSpeedZ != 0)
		{
			pParticle->frotez = D3DX_PI * 2 * dwTimeElapse / m_tParameter.fSelfRoteSpeedZ;
		}
		else
		{
			pParticle->frotez = 0.0f;
		}


		/*pParticle->vPosition = pParticle->vPosition0 + pParticle->vVelocity0  * fT+ 
		m_tParameter.vAcceleration * fT * fT * 0.5f;*/
		//可见度Visibility 插值
		float fVisibility = InterpolationVisibility(dwTimeElapse);
		//颜色插枝
		DWORD dwColor = InterpolationColor(dwTimeElapse);

		float fA = (float)( (dwColor & 0xFF000000) >> 24);
		float fR = (float)( (dwColor & 0x00FF0000) >> 16);
		float fG = (float)( (dwColor & 0x0000FF00) >>  8);
		float fB = (float)(  dwColor & 0x000000FF);

		//颜色浓度运算
		render::Interface::Layer3D::ShaderGroup* pshadergroup;
		pshadergroup = render::Interface::GetInstance()->GetLayer3D()->GetShaderGroup(m_dwShaderGroupHandle);
		render::Interface::Layer3D::ShaderGroup::_tagShader* ptagshader;
		if (pshadergroup)
		{
			ptagshader = pshadergroup->GetShader(0);
			if (ptagshader)
			{
				if (ptagshader->dwAlphaBlendMode == 0)
				{

					DWORD c = (DWORD)(fA);
					dwColor = c << 24;
					c = (DWORD)( fR * fVisibility); 
					dwColor |= c << 16;
					c = (DWORD)( fG * fVisibility);
					dwColor |= c << 8;
					c = (DWORD)( fB * fVisibility);
					dwColor |= c;
				}
				else
				{
					ptagshader->dwAlphaBlendMode = 1;
					DWORD c = (DWORD)( fA * fVisibility);
					dwColor = c << 24;
					c = (DWORD)( fR ); 
					dwColor |= c << 16;
					c = (DWORD)( fG );
					dwColor |= c << 8;
					c = (DWORD)( fB);
					dwColor |= c;


				}
			}
		}






		pParticle->dwColor = dwColor;

		//UV动画  
		//计算流逝的uv动画时间
		DWORD dwAnimTextureTimeElapse = dwCurTime - pParticle->dwAnimTextureStartTime;

		if (dwAnimTextureTimeElapse >= m_tParameter.dwAnimTextureTimeLength)                                            		
		{
			pParticle->dwAnimTextureStartTime = dwCurTime;
		}
		int total = m_tParameter.dwAnimTextureRows * m_tParameter.dwAnimTextureColumns;
		pParticle->iAnimTextureRectIndex  =  total * dwAnimTextureTimeElapse / m_tParameter.dwAnimTextureTimeLength;

		//减少生命,匀速运动			        
		if (dwTimeElapse > m_tParameter.dwLifeTimeLength)
		{
			//死亡，移动到空闲表中
			*ppParticle = pParticle->pNext;
			pParticle->pNext = m_pParticleFree;
			m_pParticleFree = pParticle;
			//			iD++;
		}
		else
		{
			//装入渲染链
			pParticle->pNextRender = m_pParticleRenderHead;
			m_pParticleRenderHead  = pParticle;
			//下一个
			ppParticle = &pParticle->pNext;
			//			iR++;
		}
		//		iC++;

	}
	if (m_bReadyToDie)
	{
		if (!m_pParticleRenderHead)
		{
			m_bDied = true;
		}
	}
	//OutputConsole("%d %d %d\n",iC,iD,iR);
}

void particle::Emitter::SetBillboard(const D3DXMATRIX *pBillboard)
{
	memcpy(&m_matBillboard,pBillboard,sizeof(D3DXMATRIX));
}
void particle::Emitter::SetBillboardY(const D3DXMATRIX *pBillboardY)
{
	memcpy(&m_matBillboardY,pBillboardY,sizeof(D3DXMATRIX));
}

void particle::Emitter::SetPosition(const D3DXVECTOR3 * pPosition)
{
	m_vPosition = *pPosition;
}
void particle::Emitter::SetSrcPosition(const D3DXVECTOR3 * pPosition)
{
	m_vSrcPosition = *pPosition;
}

void particle::Emitter::Render(float fscal,bool bsendtobuffer)
{
	if(GetLoadState() != Load_Did)	return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	static D3DXVECTOR3 pos0[6] = { D3DXVECTOR3(-0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,-0.5f,0.0f),
		D3DXVECTOR3(-0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,-0.5f,0.0f),D3DXVECTOR3(-0.5f,-0.5f,0.0f) };		
	static D3DXVECTOR2 uv0[6]  = { D3DXVECTOR2(0,0),D3DXVECTOR2(1,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,1)};

	static D3DXVECTOR3 position[20000];
	static D3DXVECTOR2 uv[20000];
	static WORD index[60000];
	static DWORD color[20000];
	//colorSpecular[20000] = {0};  
	//static render::Interface::Layer3D::_tagTriangle triangle[20000];
	static D3DXMATRIX  matWorld,matTrans,matScale,matRot,matRotX,matRotY,matRotZ,matRotSpeedX,matRotSpeedY,matRotSpeedZ;
	DWORD dwColor = 0;

	tagParticle *pParticle = m_pParticleRenderHead;
	DWORD usenum = 0;
	while(pParticle)
	{
		if (!bsendtobuffer)
		{
			pParticle = pParticle->pNextRender;
			continue;
		}
		dwColor = pParticle->dwColor;

		float x = (float)(pParticle->iAnimTextureRectIndex % m_tParameter.dwAnimTextureRows);
		float y = (float)(pParticle->iAnimTextureRectIndex / m_tParameter.dwAnimTextureRows);
		float w = 1.0f / (float)m_tParameter.dwAnimTextureRows;
		float h = 1.0f / (float)m_tParameter.dwAnimTextureColumns;
		float u = x * w;
		float v = y * h;
		D3DXMATRIX rotmat,transmat,invtransmat;
		if (m_tParameter.m_iroatdir)
		{
			float fRatioRoat = timeGetTime()%m_tParameter.m_iroatspeed/(float)m_tParameter.m_iroatspeed;
			D3DXMatrixTranslation(&transmat,-0.5f,-0.5f,0.0f);
			D3DXMatrixInverse(&invtransmat,NULL,&transmat);
			D3DXMatrixRotationZ(&rotmat,D3DX_PI * fRatioRoat * 2.0f * m_tParameter.m_iroatdir);
			transmat =  transmat * rotmat* invtransmat ;
		}


		switch(m_tParameter.eParticleType)
		{
		case PARTICLE_QUAD:
			{
				//大小插值
				if (m_tParameter.vfsize.size())
				{
					DWORD dwCurTime = timeGetTime();
					DWORD dwTimeElapse = dwCurTime - pParticle->dwLifeTimeStart;
					float fw,fh;
					float size = InterpolationSize(dwTimeElapse);
					fh = pParticle->fScaleHeight * size * fscal;
					fw = pParticle->fScaleWidth  * size * fscal;
					D3DXMatrixScaling(&matScale,fw,fh,1.0f);

				}
				else
				{
					D3DXMatrixScaling(&matScale,pParticle->fScaleWidth,pParticle->fScaleHeight,1.0f);
				}
				D3DXMatrixIdentity(&matRot);
				if(m_tParameter.fAngleX)
				{
					D3DXMatrixRotationX(&matRotX,m_tParameter.fAngleX);
					matRot *= matRotX;
				}
				if (m_tParameter.fAngleY)
				{
					D3DXMatrixRotationY(&matRotY,m_tParameter.fAngleY);
					matRot *= matRotY;
				}
				if (m_tParameter.fAngleZ)
				{
					D3DXMatrixRotationY(&matRotZ,m_tParameter.fAngleZ);
					matRot *= matRotZ;
				}
				if (pParticle->frotex != 0.0f)
				{
					D3DXMatrixRotationX(&matRotSpeedX,pParticle->frotex);
					matRot *= matRotSpeedX;
				}
				if (pParticle->frotey != 0.0f)
				{
					D3DXMatrixRotationY(&matRotSpeedY,pParticle->frotey);
					matRot *= matRotSpeedY;
				}
				if (pParticle->frotez != 0.0f)
				{
					D3DXMatrixRotationZ(&matRotSpeedZ,pParticle->frotez);
					matRot *= matRotSpeedZ;
				}
				if (m_tParameter.bstaytofirstframe)
				{
					matRot *= pParticle->matstartstate;
				}
				else
				{
					matRot *= m_matNowMatrix;
				}






				if (m_tParameter.bBillboardY)
				{
					m_matBillboardY._41 = pParticle->vPosition.x;
					m_matBillboardY._42 = pParticle->vPosition.y;
					m_matBillboardY._43 = pParticle->vPosition.z;
					D3DXMatrixMultiply(&matWorld,&matScale,&m_matBillboardY);
				}
				else
				{
					if (m_tParameter.bBillboard)
					{
						m_matBillboard._41 = pParticle->vPosition.x;
						m_matBillboard._42 = pParticle->vPosition.y;
						m_matBillboard._43 = pParticle->vPosition.z;
						D3DXMatrixMultiply(&matWorld,&matScale,&m_matBillboard);
					}
					else
					{

						D3DXMatrixTranslation(&matTrans,pParticle->vPosition.x,pParticle->vPosition.y,pParticle->vPosition.z);
						D3DXMatrixMultiply(&matWorld,&matScale,&matRot);
						D3DXMatrixMultiply(&matWorld,&matWorld,&matTrans);
					}
				}
				for(int i = 0; i < 6; i++)
				{
					D3DXVec3TransformCoord(&position[usenum + i],&pos0[i],&matWorld);  
					if (m_tParameter.m_iroatdir)
					{
						D3DXVec2TransformCoord(&uv[usenum + i],&uv0[i],&transmat);
					}
					else
					{
						uv[usenum + i].x = u + w * uv0[i].x;
						uv[usenum + i].y = v + h * uv0[i].y;
					}
					index[usenum + i] = usenum + i;
					color[usenum + i] = dwColor;
				}

				/*for(int i = 0; i < 6; i++)
				{
				D3DXVec3TransformCoord(&triangle[usenum + (i > 2?1:0)].pos[i%3],&pos0[i],&matWorld);  
				if (m_tParameter.m_iroatdir)
				{
				D3DXVec2TransformCoord(&triangle[usenum + (i > 2?1:0)].uv0[i%3],&uv0[i],&transmat);
				}
				else
				{
				triangle[usenum + (i > 2?1:0)].uv0[i%3].x = u + w * uv0[i].x;
				triangle[usenum + (i > 2?1:0)].uv0[i%3].y = v + h * uv0[i].y;
				}
				triangle[usenum + (i > 2?1:0)].color[i%3] = dwColor;
				}*/
			}

			break;
		case PARTICLE_BELT:
			{
				D3DXVECTOR3 vDis = pParticle->vPosition - pParticle->vPosition0;

				if(abs(vDis.x) > abs(vDis.y))
				{
					position[usenum + 0].x = 0;
					position[usenum + 0].y = pParticle->fScaleHeight;
					position[usenum + 0].z = 0;

					position[usenum + 1].x = vDis.x;
					position[usenum + 1].y = vDis.y + pParticle->fScaleHeight;
					position[usenum + 1].z = vDis.z;

					position[usenum + 2].x = vDis.x;
					position[usenum + 2].y = vDis.y - pParticle->fScaleHeight;
					position[usenum + 2].z = vDis.z;

					position[usenum + 3].x = position[usenum + 0].x;
					position[usenum + 3].y = position[usenum + 0].y;
					position[usenum + 3].z = position[usenum + 0].z;

					position[usenum + 4].x = position[usenum + 2].x;
					position[usenum + 4].y = position[usenum + 2].y;
					position[usenum + 4].z = position[usenum + 2].z;

					position[usenum + 5].x = 0;
					position[usenum + 5].y = -pParticle->fScaleHeight;
					position[usenum + 5].z = 0;
				}
				else
				{
					position[usenum + 0].x = -pParticle->fScaleWidth;
					position[usenum + 0].y = 0;
					position[usenum + 0].z = 0;

					position[usenum + 1].x = vDis.x - pParticle->fScaleWidth;
					position[usenum + 1].y = vDis.y;
					position[usenum + 1].z = vDis.z;

					position[usenum + 2].x = vDis.x + pParticle->fScaleWidth;
					position[usenum + 2].y = vDis.y;
					position[usenum + 2].z = vDis.z;

					position[usenum + 3].x = position[usenum + 0].x;
					position[usenum + 3].y = position[usenum + 0].y;
					position[usenum + 3].z = position[usenum + 0].z;

					position[usenum + 4].x = position[usenum + 2].x;
					position[usenum + 4].y = position[usenum + 2].y;
					position[usenum + 4].z = position[usenum + 2].z;

					position[usenum + 5].x = pParticle->fScaleWidth;
					position[usenum + 5].y = 0;
					position[usenum + 5].z = 0;
				}

				m_matBillboard._41 = pParticle->vPosition0.x;
				m_matBillboard._42 = pParticle->vPosition0.y;
				m_matBillboard._43 = pParticle->vPosition0.z;

				for(int i = 0; i < 6; i++)
				{
					D3DXVec3TransformCoord(&position[usenum + i],&position[usenum + i],&m_matBillboard);  
					if (m_tParameter.m_iroatdir)
					{
						D3DXVec2TransformCoord(&uv[usenum + i],&uv0[i],&transmat);
					}
					else
					{
						uv[usenum + i].x = u + w * uv0[i].x;
						uv[usenum + i].y = v + h * uv0[i].y;
					}
					index[usenum + i] = usenum + i;
					color[usenum + i] = dwColor;
					//triangle[usenum + (i > 2?1:0)].color[i%3] = dwColor;
				}
			}
			break;
		}

		if (usenum > 19980)
		{
			pLayer3D->SendBufferToCache(m_dwShaderGroupHandle,0,usenum,usenum,position,NULL,color,uv,index,false,1);
			usenum = -6;
		}
		usenum += 6;
		pParticle = pParticle->pNextRender;
	}

	m_pParticleRenderHead = NULL;
	pLayer3D->SendBufferToCache(m_dwShaderGroupHandle,0,usenum,usenum,position,NULL,color,uv,index,false,1);

}
void particle::Emitter::Draw(float fscal,bool bdrawtoscreen,bool brendertotarget)
{
	if(GetLoadState() != Load_Did)	return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	static D3DXVECTOR3 pos0[6] = { D3DXVECTOR3(-0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,-0.5f,0.0f),
		D3DXVECTOR3(-0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,-0.5f,0.0f),D3DXVECTOR3(-0.5f,-0.5f,0.0f) };		
	static D3DXVECTOR2 uv0[6]  = { D3DXVECTOR2(0,0),D3DXVECTOR2(1,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,0),D3DXVECTOR2(1,1),D3DXVECTOR2(0,1)};

	static D3DXVECTOR3 position[20000];
	static D3DXVECTOR2 uv[20000];
	static WORD index[60000];
	static DWORD color[20000];
	//colorSpecular[20000] = {0};  
	//static render::Interface::Layer3D::_tagTriangle triangle[20000];
	static D3DXMATRIX  matWorld,matTrans,matScale,matRot,matRotX,matRotY,matRotZ,matRotSpeedX,matRotSpeedY,matRotSpeedZ;
	DWORD dwColor = 0;

	tagParticle *pParticle = m_pParticleRenderHead;
	DWORD usenum = 0;
	while(pParticle)
	{
		if (bdrawtoscreen)
		{
			if (!pParticle->m_bDrawToScteen)
			{
				pParticle = pParticle->pNextRender;
				continue;
			}
		}
		else if (brendertotarget)
		{
			if (!pParticle->m_bRenderToTarger)
			{
				pParticle = pParticle->pNextRender;
				continue;
			}
		}

		dwColor = pParticle->dwColor;

		float x = (float)(pParticle->iAnimTextureRectIndex % m_tParameter.dwAnimTextureRows);
		float y = (float)(pParticle->iAnimTextureRectIndex / m_tParameter.dwAnimTextureRows);
		float w = 1.0f / (float)m_tParameter.dwAnimTextureRows;
		float h = 1.0f / (float)m_tParameter.dwAnimTextureColumns;
		float u = x * w;
		float v = y * h;
		D3DXMATRIX rotmat,transmat,invtransmat;
		if (m_tParameter.m_iroatdir)
		{
			float fRatioRoat = timeGetTime()%m_tParameter.m_iroatspeed/(float)m_tParameter.m_iroatspeed;
			D3DXMatrixTranslation(&transmat,-0.5f,-0.5f,0.0f);
			D3DXMatrixInverse(&invtransmat,NULL,&transmat);
			D3DXMatrixRotationZ(&rotmat,D3DX_PI * fRatioRoat * 2.0f * m_tParameter.m_iroatdir);
			transmat =  transmat * rotmat* invtransmat ;
		}


		switch(m_tParameter.eParticleType)
		{
		case PARTICLE_QUAD:
			{
				//大小插值
				if (m_tParameter.vfsize.size())
				{
					DWORD dwCurTime = timeGetTime();
					DWORD dwTimeElapse = dwCurTime - pParticle->dwLifeTimeStart;
					float fw,fh;
					float size = InterpolationSize(dwTimeElapse);
					fh = pParticle->fScaleHeight * size * fscal;
					fw = pParticle->fScaleWidth  * size * fscal ;
					D3DXMatrixScaling(&matScale,fw,fh,1.0f);

				}
				else
				{
					D3DXMatrixScaling(&matScale,pParticle->fScaleWidth,pParticle->fScaleHeight,1.0f);
				}


				D3DXMatrixIdentity(&matRot);
				if(m_tParameter.fAngleX)
				{
					D3DXMatrixRotationX(&matRotX,m_tParameter.fAngleX);
					matRot *= matRotX;
				}
				if (m_tParameter.fAngleY)
				{
					D3DXMatrixRotationY(&matRotY,m_tParameter.fAngleY);
					matRot *= matRotY;
				}
				if (m_tParameter.fAngleZ)
				{
					D3DXMatrixRotationY(&matRotZ,m_tParameter.fAngleZ);
					matRot *= matRotZ;
				}
				if (pParticle->frotex != 0.0f)
				{
					D3DXMatrixRotationX(&matRotSpeedX,pParticle->frotex);
					matRot *= matRotSpeedX;
				}
				if (pParticle->frotey != 0.0f)
				{
					D3DXMatrixRotationY(&matRotSpeedY,pParticle->frotey);
					matRot *= matRotSpeedY;
				}
				if (pParticle->frotez != 0.0f)
				{
					D3DXMatrixRotationZ(&matRotSpeedZ,pParticle->frotez);
					matRot *= matRotSpeedZ;
				}
				if (m_tParameter.bstaytofirstframe)
				{
					matRot *= pParticle->matstartstate;
				}
				else
				{
					matRot *= m_matNowMatrix;
				}
				if (m_tParameter.bBillboardY)
				{
					m_matBillboardY._41 = pParticle->vPosition.x;
					m_matBillboardY._42 = pParticle->vPosition.y;
					m_matBillboardY._43 = pParticle->vPosition.z;
					D3DXMatrixMultiply(&matWorld,&matScale,&m_matBillboardY);
				}
				else
				{
					if (m_tParameter.bBillboard)
					{
						m_matBillboard._41 = pParticle->vPosition.x;
						m_matBillboard._42 = pParticle->vPosition.y;
						m_matBillboard._43 = pParticle->vPosition.z;
						D3DXMatrixMultiply(&matWorld,&matScale,&m_matBillboard);
					}
					else
					{

						D3DXMatrixTranslation(&matTrans,pParticle->vPosition.x,pParticle->vPosition.y,pParticle->vPosition.z);
						D3DXMatrixMultiply(&matWorld,&matScale,&matRot);
						D3DXMatrixMultiply(&matWorld,&matWorld,&matTrans);
					}
				}
				for(int i = 0; i < 6; i++)
				{
					D3DXVec3TransformCoord(&position[usenum + i],&pos0[i],&matWorld);  
					if (m_tParameter.m_iroatdir)
					{
						D3DXVec2TransformCoord(&uv[usenum + i],&uv0[i],&transmat);
					}
					else
					{
						uv[usenum + i].x = u + w * uv0[i].x;
						uv[usenum + i].y = v + h * uv0[i].y;
					}
					index[usenum + i] = usenum + i;
					color[usenum + i] = dwColor;
				}
			}
			break;
		case PARTICLE_BELT:
			{
				D3DXVECTOR3 vDis = pParticle->vPosition - pParticle->vPosition0;

				if(abs(vDis.x) > abs(vDis.y))
				{
					position[usenum + 0].x = 0;
					position[usenum + 0].y = pParticle->fScaleHeight;
					position[usenum + 0].z = 0;

					position[usenum + 1].x = vDis.x;
					position[usenum + 1].y = vDis.y + pParticle->fScaleHeight;
					position[usenum + 1].z = vDis.z;

					position[usenum + 2].x = vDis.x;
					position[usenum + 2].y = vDis.y - pParticle->fScaleHeight;
					position[usenum + 2].z = vDis.z;

					position[usenum + 3].x = position[usenum + 0].x;
					position[usenum + 3].y = position[usenum + 0].y;
					position[usenum + 3].z = position[usenum + 0].z;

					position[usenum + 4].x = position[usenum + 2].x;
					position[usenum + 4].y = position[usenum + 2].y;
					position[usenum + 4].z = position[usenum + 2].z;

					position[usenum + 5].x = 0;
					position[usenum + 5].y = -pParticle->fScaleHeight;
					position[usenum + 5].z = 0;
				}
				else
				{
					position[usenum + 0].x = -pParticle->fScaleWidth;
					position[usenum + 0].y = 0;
					position[usenum + 0].z = 0;

					position[usenum + 1].x = vDis.x - pParticle->fScaleWidth;
					position[usenum + 1].y = vDis.y;
					position[usenum + 1].z = vDis.z;

					position[usenum + 2].x = vDis.x + pParticle->fScaleWidth;
					position[usenum + 2].y = vDis.y;
					position[usenum + 2].z = vDis.z;

					position[usenum + 3].x = position[usenum + 0].x;
					position[usenum + 3].y = position[usenum + 0].y;
					position[usenum + 3].z = position[usenum + 0].z;

					position[usenum + 4].x = position[usenum + 2].x;
					position[usenum + 4].y = position[usenum + 2].y;
					position[usenum + 4].z = position[usenum + 2].z;

					position[usenum + 5].x = pParticle->fScaleWidth;
					position[usenum + 5].y = 0;
					position[usenum + 5].z = 0;
				}

				m_matBillboard._41 = pParticle->vPosition0.x;
				m_matBillboard._42 = pParticle->vPosition0.y;
				m_matBillboard._43 = pParticle->vPosition0.z;

				for(int i = 0; i < 6; i++)
				{
					D3DXVec3TransformCoord(&position[usenum + i],&position[usenum + i],&m_matBillboard);  
					if (m_tParameter.m_iroatdir)
					{
						D3DXVec2TransformCoord(&uv[usenum + i],&uv0[i],&transmat);
					}
					else
					{
						uv[usenum + i].x = u + w * uv0[i].x;
						uv[usenum + i].y = v + h * uv0[i].y;
					}
					index[usenum + i] = usenum + i;
					color[usenum + i] = dwColor;
					//triangle[usenum + (i > 2?1:0)].color[i%3] = dwColor;
				}
			}
			break;
		}
		pParticle = pParticle->pNextRender;
		if (usenum > 19980)
		{

			pLayer3D->DrawTriangleArrayByShader(m_dwShaderGroupHandle,0,usenum,usenum,position,NULL,uv,color,index,false,1);

			usenum = -6;
		}
		usenum += 6;
	}

	m_pParticleRenderHead = NULL;

	pLayer3D->DrawTriangleArrayByShader(m_dwShaderGroupHandle,0,usenum,usenum,position,NULL,uv,color,index,false,1);


}

//CLASS__(particle::Manager)_____
BOOL particle::Manager::Create()
{
	return TRUE;
}

void particle::Manager::Destroy()
{
	//TODO:
	std::list<Emitter *>::iterator it;
	for(it = m_lEmitterList.begin(); it != m_lEmitterList.end();it++)
	{
		SAFE_DELETE((*it));
	}
	m_lEmitterList.clear();

	for(it = m_lEmitterListSource.begin(); it != m_lEmitterListSource.end();it++)
	{
		SAFEDESTROY((*it));
	}
	m_lEmitterListSource.clear();

}

void particle::Manager::Update(float fscal,bool brendtoscreen,bool brendertotarget)
{
	//刷新所有粒子
	std::list<Emitter *>::iterator it;
	for(it = m_lEmitterList.begin(); it != m_lEmitterList.end();)
	{
		Emitter *pEmitter = *it;
		if (brendtoscreen)
		{
			if (pEmitter->m_bDrawToScreen)
			{
				pEmitter->Update(fscal);
			}

		}
		else if (brendertotarget)
		{
			if (pEmitter->m_bRenderToTarger)
			{
				pEmitter->Update(fscal);
			}
		}
		else
		{
			pEmitter->Update(fscal);
		}
		if (pEmitter->GetIsDead())
		{
			it = m_lEmitterList.erase(it);
			SAFE_DELETE(pEmitter);
		}
		else
		{
			it++;
		}

	}	
	for(it = m_lEmitterListSource.begin(); it != m_lEmitterListSource.end();)
	{
		Emitter *pEmitter = *it;
		pEmitter->Update(fscal);
		if (pEmitter->GetIsDead())
		{
			it = m_lEmitterListSource.erase(it);
			SAFEDESTROY(pEmitter);
		}
		else
		{
			it++;
		}

	}	
}

void particle::Manager::Render(float fscal,bool bsendtobuffer)
{
	std::list<Emitter *>::iterator it;
	for(it = m_lEmitterList.begin(); it != m_lEmitterList.end(); it++)
	{
		Emitter *pEmitter = *it;
		pEmitter->Render( fscal,bsendtobuffer);
	}
}
void particle::Manager::Draw(float fscal,bool brendtoscreen,bool brendertotarget)
{
	std::list<Emitter *>::iterator it;
	for(it = m_lEmitterList.begin(); it != m_lEmitterList.end(); it++)
	{
		Emitter *pEmitter = *it;
		if (brendtoscreen)
		{
			if (pEmitter->m_bDrawToScreen&&!pEmitter->m_bInDraw)
			{
				pEmitter->Draw(fscal,brendtoscreen,brendertotarget);
				pEmitter->m_bInDraw = true;
			}
		}
		else if (brendertotarget)
		{
			if (pEmitter->m_bRenderToTarger)
			{
				pEmitter->Draw(fscal,brendtoscreen,brendertotarget);
			}

		}

	}
}

void particle::Manager::Add(particle::Emitter *pEmitter)
{
	m_lEmitterList.push_back(pEmitter);
}
void particle::Manager::AddSourcelist(particle::Emitter *pEmitter)
{
	m_lEmitterListSource.push_back(pEmitter);
}

void particle::Manager::Remove(particle::Emitter *pEmitter)
{
	std::list<Emitter *>::iterator it;
	it = find(m_lEmitterList.begin(),m_lEmitterList.end(),pEmitter);
	if(it != m_lEmitterList.end())
	{
		m_lEmitterList.erase(it);
	}
}

BOOL particle::Emitter::UpdateTime(DWORD dwLastTime,DWORD dwCurTime)
{
	m_bEmited = FALSE;
	if (dwCurTime - dwLastTime > m_tParameter.dwEmitInterval)
	{
		m_bEmited = TRUE;
	}
	return m_bEmited;
}

void particle::Emitter::Emission()
{
	DWORD dwCurTime = timeGetTime();
	m_bInDraw = false;
	if (m_bEmited)
	{
		for(DWORD n = 0; n < m_tParameter.dwEmitNumber; n++)
		{
			//取得空闲的粒子
			tagParticle *pNewParticle = NewParticle();


			//设置生成参数
			pNewParticle->vSrcPosition          = m_vSrcPosition;
			pNewParticle->vPosition0	        = m_vPosition;
			pNewParticle->dwLifeTimeStart	    = dwCurTime;
			pNewParticle->dwAnimTextureStartTime= dwCurTime; 
			pNewParticle->iAnimTextureRectIndex = 0;
			pNewParticle->m_bDrawToScteen = m_bDrawToScreen;
			pNewParticle->m_bRenderToTarger = m_bRenderToTarger;
			if (m_tParameter.bstaytofirstframe)
			{
				pNewParticle->matstartstate   = m_matNowMatrix;
			}
			//设置生成的点数据

			pNewParticle->pNextRender = NULL;						//渲染链
			FLOAT fRandYaw;
			FLOAT fRandPitch;
			//初始化速度
			if (m_tParameter.fPitchAngleRange != 0)
			{
				fRandYaw   = ((float)rand()/(float)RAND_MAX) * 3.1415926f * 2.00f;
				fRandPitch = (m_tParameter.fPitchAngleStartx + ((FLOAT)rand()/(FLOAT)RAND_MAX) * m_tParameter.fPitchAngleRange) * 3.1415926f / 180.0f;
			}
			else
			{
				fRandPitch = m_tParameter.fPitchAngleStartx * 3.1415926f / 180.0f;
				fRandYaw   = m_tParameter.fPitchAngleStarty * 3.1415926f / 180.0f; 
			}



			pNewParticle->vVelocity0.x  = sinf(fRandPitch) * cosf(fRandYaw);
			pNewParticle->vVelocity0.y  = cosf(fRandPitch);
			pNewParticle->vVelocity0.z  = sinf(fRandPitch) * sinf(fRandYaw);
			pNewParticle->vVelocity0   *= m_tParameter.fInitVelocity;
			float fRatio = ((FLOAT)rand()/(FLOAT)RAND_MAX);
			pNewParticle->fScaleWidth   = m_tParameter.fInitWidth +  fRatio * m_tParameter.fInitWidthRange; 
			pNewParticle->fScaleHeight  = m_tParameter.fInitHeight + fRatio * m_tParameter.fInitHeightRange; 
		}
	}
}

float particle::Emitter::InterpolationVisibility(DWORD dwTimeElapse)
{
	//插值
	//A -+- B
	DWORD dwFrameA = m_tParameter.dwVisibilityFrameCount * dwTimeElapse / m_tParameter.dwLifeTimeLength;
	if (dwFrameA >= m_tParameter.dwVisibilityFrameCount - 1)
	{
		//不需要插值
		dwFrameA = m_tParameter.dwVisibilityFrameCount - 1; 			
		return m_tParameter.pfVisibilityFrameArray[dwFrameA];   	
	}

	//else
	DWORD dwFrameB = (dwFrameA + 1) % m_tParameter.dwVisibilityFrameCount; 
	//get A time
	DWORD dwTimeA =  dwFrameA * m_tParameter.dwLifeTimeLength /  m_tParameter.dwVisibilityFrameCount - 1;
	//计算A B 之间 ratio
	DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
	float fFrameInterval = (float)m_tParameter.dwLifeTimeLength / (float)(m_tParameter.dwVisibilityFrameCount - 1);
	float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 

	//interpolation	a --- b
	float fFrameAData = m_tParameter.pfVisibilityFrameArray[dwFrameA];   	
	float fFrameBData = m_tParameter.pfVisibilityFrameArray[dwFrameB];   	
	return fFrameAData + (fFrameBData - fFrameAData) * fRatio;
}

float particle::Emitter::InterpolationSize(DWORD dwTimeElapse)
{
	//插值
	//A -+- B
	DWORD dwFrameA = m_tParameter.vfsize.size()* dwTimeElapse / m_tParameter.dwLifeTimeLength;
	if (dwFrameA >= m_tParameter.vfsize.size() - 1)
	{
		//不需要插值
		dwFrameA = m_tParameter.vfsize.size() - 1; 			
		return m_tParameter.vfsize[dwFrameA];   	
	}

	//else
	DWORD dwFrameB = (dwFrameA + 1) % m_tParameter.vfsize.size(); 
	//get A time
	DWORD dwTimeA =  dwFrameA * m_tParameter.dwLifeTimeLength /  m_tParameter.vfsize.size() - 1;
	//计算A B 之间 ratio
	DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
	float fFrameInterval = (float)m_tParameter.dwLifeTimeLength / (float)(m_tParameter.vfsize.size() - 1);
	float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 

	//interpolation	a --- b
	float fFrameAData = m_tParameter.vfsize[dwFrameA];   	
	float fFrameBData = m_tParameter.vfsize[dwFrameB];   	
	return fFrameAData + (fFrameBData - fFrameAData) * fRatio;
}

DWORD particle::Emitter::InterpolationColor(DWORD dwTimeElapse)
{
	//插值
	//A -+- B
	DWORD dwFrameA = (m_tParameter.dwColorFrameCount -1) * dwTimeElapse / m_tParameter.dwLifeTimeLength;
	if (dwFrameA >= m_tParameter.dwColorFrameCount - 1)
	{
		//不需要插值
		dwFrameA = m_tParameter.dwColorFrameCount - 1; 			
		return m_tParameter.pdwColorFrameArray[dwFrameA];   	
	}

	//else
	DWORD dwFrameB = (dwFrameA + 1) % m_tParameter.dwColorFrameCount; 
	//get A time
	DWORD dwTimeA = 0;
	//计算A B 之间 ratio
	float fFrameInterval = 0;
	if (m_tParameter.dwColorFrameCount > 1)
	{
		dwTimeA =  dwFrameA * m_tParameter.dwLifeTimeLength /  (m_tParameter.dwColorFrameCount - 1);
		fFrameInterval = (float)m_tParameter.dwLifeTimeLength / (float)(m_tParameter.dwColorFrameCount - 1);
	}
	else
	{
		fFrameInterval = (float)m_tParameter.dwLifeTimeLength;
	}
	DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
	float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 

	//interpolation	color a --- b
	DWORD dwFrameAColor = m_tParameter.pdwColorFrameArray[dwFrameA];   	
	DWORD dwFrameBColor = m_tParameter.pdwColorFrameArray[dwFrameB];  

	float fAA = (float)((dwFrameAColor & 0xFF000000) >> 24);
	float fRA = (float)((dwFrameAColor & 0x00FF0000) >> 16);
	float fGA = (float)((dwFrameAColor & 0x0000FF00) >>  8);
	float fBA = (float)( dwFrameAColor & 0x000000FF);

	float fAB = (float)((dwFrameBColor & 0xFF000000) >> 24);
	float fRB = (float)((dwFrameBColor & 0x00FF0000) >> 16);
	float fGB = (float)((dwFrameBColor & 0x0000FF00) >>  8);
	float fBB = (float)( dwFrameBColor & 0x000000FF);

	DWORD dwA = (DWORD)(fAA + (fAB - fAA) * fRatio);
	if (dwA > 255) dwA = 255;
	DWORD dwR = (DWORD)(fRA + (fRB - fRA) * fRatio);
	if (dwR > 255) dwR = 255;
	DWORD dwG = (DWORD)(fGA + (fGB - fGA) * fRatio);
	if (dwG > 255) dwG = 255;
	DWORD dwB = (DWORD)(fBA + (fBB - fBA) * fRatio);
	if (dwB > 255) dwB = 255;

	return D3DCOLOR_ARGB(dwA,dwR,dwG,dwB);
}