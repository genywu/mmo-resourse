#include "stdafx.h"
#include "Effect.h"
#include "../../EngineExtend/DisplayModel.h"
#include "../../EngineExtend/GameModelManager.h"
#include "../../EngineExtend/GameMap.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/ClientRegion.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEffect::CEffect(void)
{
	SetType(TYPE_EFFECT);
	m_bLoop = false;
	m_b3DRot = false;

	m_lEffectID = 0;
	m_lHostType = 0;
	m_HostID=CGUID::GUID_INVALID;
	m_lHostGraphicsID = 0;
	m_eHostLocator = LOCATOR_FOOTCENTER;
	m_fHeightAdjust = 0.0f;
	m_dwLightFadeStartTime = 0;
	m_dwLightFadeLength = 0;
	m_dwLightColor = 0;
	m_dwLightType  = 0;
	m_bLightAvailable = FALSE;
	m_fLightRadius = 0.0f;
	m_dwLightStartTime = timeGetTime();
	m_bTask = false;
	IsUsingSelfAnim = false;
}

CEffect::CEffect(const CEffect &srcEffect)
{
	SetType(TYPE_EFFECT);
	SetAction(ACT_STAND);

	m_dwLightType = srcEffect.m_dwLightType;
	m_bLoop = srcEffect.m_bLoop;

	m_lEffectID = srcEffect.m_lEffectID;
	m_lHostType = srcEffect.m_lHostType;
	m_lHostGraphicsID = srcEffect.m_lHostGraphicsID;
	m_eHostLocator = srcEffect.m_eHostLocator;
	m_dwLightFadeStartTime = srcEffect.m_dwLightFadeStartTime;
	m_dwLightFadeLength = srcEffect.m_dwLightFadeLength;
	m_dwLightColor = srcEffect.m_dwLightColor;
	m_dwLightType  = srcEffect.m_dwLightType;
	m_bLightAvailable = srcEffect.m_bLightAvailable;
	m_fLightRadius = srcEffect.m_fLightRadius;
	m_dwLightStartTime = srcEffect.m_dwLightStartTime;
	m_bTask = srcEffect.m_bTask;
}

CEffect::~CEffect(void)
{
}

void CEffect::AI()
{
	CShape::AI();

	if (m_bLoop==false && m_bDisplayFinish)
	{
		SetIsDeleted(true);
		return;
	}
}



/*
* 功能: 显示特效
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.10.17 - lpf
*		增加了对特效渐进，渐出的处理
*	2008.12.10 - lpf
*		显示特效时,增加了拾取盒子高度修正值;
*/
bool CEffect::Display(void)
{
	if(NULL == m_pDisplayModel)	return false;
	if(m_pDisplayModel->GetLoadState() != Load_Did)	return false;

	m_pDisplayModel->EnableTrailEmitter(TRUE);
	CClientRegion *pRegion = GetGame()->GetRegion();
	if(NULL == pRegion)	return false;
	render::Camera *pCamera = pRegion->GetCamera();

	if(IsLightAvailable())
	{
		D3DXVECTOR3 vLightPosition = D3DXVECTOR3(GetPosY(),GetHeight(),GetPosX());
		pRegion->GetGameMap()->AddPointLightSource(&vLightPosition,GetLightType(),GetLightRadius()
			,GetLightColor());
	}
	if (m_pAnimInfo->IsSelfAnimJoint() == false)
	{
		float fX = GetPosX();
		float fY = GetPosY();
		float fH = GetHeight() + m_fHeightAdjust;
		float fNowDir = GetNowDir();
		D3DXMATRIX matNowDir = Get3DRotMatrix();
		if(m_lHostType !=0 && m_HostID != CGUID::GUID_INVALID)
		{
			CShape* pShape = dynamic_cast<CShape*>(GetGame()->GetRegion()->FindChildObject(m_lHostType, m_HostID));
			if(pShape)
			{
				pShape->GetLocatorPos(m_eHostLocator,fX,fY,fH);
				fH += m_fHeightAdjust;
				matNowDir = pShape->Get3DRotMatrix();
				fNowDir = pShape->GetNowDir();
			}
		}

		if (m_b3DRot)
		{
			m_pAnimInfo->SetupAnimInfoRotMatrix(fX,fY,fH,matNowDir,pCamera);
		}else
		{
			m_pAnimInfo->SetupAnimInfo(fX,fY,fH,fNowDir,pCamera);
		}
	}	

	m_pDisplayModel->SetDirLightEnable(false);
	m_pAnimInfo->SetViewMatrix(pCamera->GetViewMatrix());
	m_pAnimInfo->SetProjectedMatrix(pCamera->GetProjectionMatrix());
	m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
	m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
	m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
	m_pDisplayModel->ProcessLocator(m_pAnimInfo);
	m_pDisplayModel->SetModelColor(0xFFFFFFFF);
	m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
	m_pDisplayModel->ProcessVisibility(m_pAnimInfo);

	if (m_bIsNeedAlphaFade && m_eAlphaFadeMode == CAlphaFadeObjectList::ALPHA_FADE_IN)
	{
		// 处理模型渐变效果
		long lElapseTime = timeGetTime() - m_lAlphaFadeInStartTime;
		if (lElapseTime > m_lAlphaFadeInTimeLength)
			m_eAlphaFadeMode = CAlphaFadeObjectList::ALPHA_FADE_NONE;

		m_pDisplayModel->MultiplyModelAlpha((float)lElapseTime / (float)m_lAlphaFadeInTimeLength);
		m_pDisplayModel->RenderModelTransparent();
	}else
		m_pDisplayModel->RenderModel();

	return true;
}



void CEffect::SetLight(DWORD dwColor,DWORD dwType,float fRadius,DWORD dwStartTime,DWORD dwLength) 
{ 
	m_bLightAvailable = TRUE;
	m_dwLightColor = dwColor;
	m_dwLightType  = dwType;
	m_dwLightFadeStartTime = dwStartTime;
	m_dwLightFadeLength    = dwLength;
	m_fLightRadius = fRadius;
}

DWORD CEffect::GetLightColor()
{
	if (m_dwLightFadeStartTime)
	{
		//计算颜色
		DWORD dwTimeElapse = timeGetTime() - m_dwLightStartTime;

		if (dwTimeElapse > m_dwLightFadeStartTime)
		{
			float fRatio = (float)(dwTimeElapse - m_dwLightFadeStartTime) / (float)m_dwLightFadeLength;
			if (fRatio > 1.0f)
				fRatio = 1.0f;
			//根据type = 0 + 
			//0+
			DWORD r = (m_dwLightColor & 0x00FF0000) >> 16;
			DWORD g = (m_dwLightColor & 0x0000FF00) >> 8;
			DWORD b = (m_dwLightColor & 0x000000FF);

			r = (DWORD)((float)r * (1 - fRatio));
			g = (DWORD)((float)g * (1 - fRatio));
			b = (DWORD)((float)b * (1 - fRatio));
			return ((r << 16) | (g << 8) | b) ;
		}
	}
	return m_dwLightColor;
}

float CEffect::GetLightRadius()
{
	return m_fLightRadius;
}





