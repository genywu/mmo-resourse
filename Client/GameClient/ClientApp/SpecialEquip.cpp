
#include <stdafx.h>

#include "../../Engine\utility.h"
#include "../../Engine\console.h"
#include "../../Engine\math.h"

#include "../../EngineExtend\DisplayModel.h"
#include "../../EngineExtend\TextureProjective.h"
#include "../../EngineExtend\gamemap.h"
#include "../../EngineExtend\GameModelManager.h"
#include "../../EngineExtend\gameselectray.h"

#include "specialequip.h"
#include "clientregion.h"
#include "../GameClient/Game.h"
#include "ModelTools.h"
#include "Shape.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpecialEquip::CSpecialEquip()
:m_pDisplayModel(NULL)
{
	m_dwModelID = 0;
	m_dwLevel  = 0;
	m_dwType   = 0000;
	m_bVisiable = FALSE;
	m_eStatus = -1;
	m_pAnimInfo = new AnimInfo();
	m_bsimplemesh = false;
	ZeroMemory(m_pAnimInfo->GetActionInfo(),sizeof(AnimInfo::tagActionInfo));
}

CSpecialEquip::~CSpecialEquip()
{
	Destroy();
	SAFE_DELETE(m_pAnimInfo);
}



/*
* 功能: 强制设定装备显示模型颜色
* 摘要: -
* 参数: 需要设定的颜色值
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.15
*/
void CSpecialEquip::SetModelColorCompel(DWORD dwColor)
{
	// 光照向量
	static D3DXVECTOR3 vLightVector(-0.613793f, -0.789465f, 0.001831f);

	if (!m_bVisiable || m_pDisplayModel == NULL)
		return;

	if (m_pDisplayModel->GetLoadState() != Load_Did)
		return;

	// 得到相机
	CClientRegion * pRegion = GetGame()->GetRegion();
	if(pRegion == NULL)
		return;

	render::Camera * pCamera = pRegion->GetCamera();

	m_pDisplayModel->SetModelColorCompel(dwColor);
	//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(), &vLightVector, dwColor);
}

void CSpecialEquip::Destroy()
{
	CGameModelManager* pModelManager = CGameModelManager::GetInstance();
	//释放
	pModelManager->ReleaseDisplayModel(m_pDisplayModel);
	m_pDisplayModel = NULL;
}
void CSpecialEquip::Copy(CSpecialEquip* psequip)
{
	m_dwModelID = psequip->m_dwModelID;
	m_dwLevel = psequip->m_dwLevel;
	m_dwType = psequip->m_dwType;
	m_eStatus = psequip->m_eStatus;
	m_bVisiable = psequip->m_bVisiable;
	m_dwLocator = psequip->m_dwLocator;
};




/*
* 功能: 设置显示模型
* 摘要: -
* 参数: 需要设定的颜色值
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.12.30 - lpf
*		在"创建"处,做了显示模型是否为空的判断,避免由于创建失败返回的空模型指针造成的当机
*/
void CSpecialEquip::SetDisplayModel()
{
	if(!m_bVisiable)
	{
		Destroy();
		return;
	}
	// 从文件读取图像数据
	char pszFileName[_MAX_PATH];
	GetModelFormatString(m_dwType,m_dwModelID,pszFileName);
	if(!GetGame()->IsFileExist(pszFileName))
	{
		Log4c::Warn(ROOT_MODULE,"model文件%s不存在。",pszFileName);
		OutputError("Model ID : %d %d 不存在\n",m_dwType,m_dwModelID);
		return;
	}

	CGameModelManager* pModelManager = CGameModelManager::GetInstance();
	if(m_pDisplayModel)
	{
		//如果相同则退出
		if(m_pDisplayModel->GetGameModelID() ==
			pModelManager->GetGameModelID(pszFileName))
			return;
	}

	//释放
	pModelManager->ReleaseDisplayModel(m_pDisplayModel);

	//创建
	m_pDisplayModel = pModelManager->CreateDisplayModel(pszFileName,false);
	if (m_pDisplayModel != NULL)
		m_pDisplayModel->ShowAllGroup(false);
}



//设置等级
void CSpecialEquip::SetLevel(DWORD level)
{
	if (!m_pDisplayModel)
	{
		return;
	}
	m_dwLevel = level;
	m_pDisplayModel->ShowAllGroupbyLevel(level,false);
	/*DWORD groupname;
	DWORD partname;
	CDisplayModel::CDisplayGroup* pGroup;
	CDisplayModel::itDisplayGroup it = m_pDisplayModel->GetGroups().begin();
	for (;it != m_pDisplayModel->GetGroups().end();it++ )
	{
	groupname = (*it).first;
	pGroup = (*it).second;
	partname = pGroup->GetPartName();
	m_pDisplayModel->ShowGroup(groupname,partname,level,false);
	}*/

}
//隐藏
void CSpecialEquip::Hide()
{
	Destroy();
}



/*
* 功能: 显示武器
* 摘要: -
* 参数: pParentObj - 父对象指针
* 返回值: -
* 作者: 彭谦
* 创建日期: -
* 修改日志:
*	2008.10.15 - lpf
*		 将该函数的两个参数CDisplay *和AnimInfo *修改为CShape *,以传更多的信息;
*	2009.04.24 - lpf
*		增加了判断父对象模型未载入完成则直接返回;
*/
void CSpecialEquip::Display(CShape * pParentObj)
{
	if (!m_bVisiable || pParentObj == NULL || pParentObj->GetDisplayModel() == NULL || m_pDisplayModel == NULL)
		return;
	if (pParentObj->GetDisplayModel()->GetLoadState() != Load_Did || m_pDisplayModel->GetLoadState() != Load_Did )
		return;

	m_pDisplayModel->GetAnimInfobyLocator(m_pAnimInfo, pParentObj->GetDisplayModel(), pParentObj->GetAnimInfo(), m_dwLocator);
	//m_pDisplayModel->Display(m_pAnimInfo);
	m_pDisplayModel->SetDirLightEnable(false);
	m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
	m_pDisplayModel->ProcessVisibility(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
	m_pDisplayModel->ProcessEnvironmentMaps(pParentObj->GetAnimInfo()->GetViewMatrix());
	m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
	m_pDisplayModel->ProcessLocator(m_pAnimInfo);
	m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);

	// 处理模型渐变效果
	if (pParentObj->GetIsNeedAlphaFade() && pParentObj->GetAlphaFadeMode() != CAlphaFadeObjectList::ALPHA_FADE_NONE)
	{
		if (pParentObj->GetAlphaFadeMode() == CAlphaFadeObjectList::ALPHA_FADE_IN)
		{
			long lElapseTime = timeGetTime() - pParentObj->GetAlphaFadeInStartTime();
			m_pDisplayModel->MultiplyModelAlpha((float)lElapseTime / (float)pParentObj->GetAlphaFadeInTimeLength());
			m_pDisplayModel->RenderModelTransparent();
		}else if (pParentObj->GetAlphaFadeMode() == CAlphaFadeObjectList::ALPHA_FADE_OUT)
		{
			m_pDisplayModel->MultiplyModelAlpha(pParentObj->GetAlphaValue());
			m_pDisplayModel->RenderModelTransparent();
		}
	}else
		m_pDisplayModel->RenderModel();
}



/*
* 功能: 显示装备带颜色的特殊效果(强制接受灯光处理)
* 摘要: -
* 参数: pParentModel    - 父显示模型指针
*		 pParentAnimInfo - 父模型动画信息指针
*		 dwFlashColor	 - 闪光的亮度颜色
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.12
* 修改日志:
*	2009.04.24 - lpf
*		增加了判断父对象模型未载入完成则直接返回;
*/
void CSpecialEquip::DisplayForFlashColor(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo, DWORD dwFlashColor)
{
	if (!m_bVisiable || pParentModel == NULL || m_pDisplayModel == NULL)
		return;
	if (pParentModel->GetLoadState() != Load_Did || m_pDisplayModel->GetLoadState() != Load_Did)
		return;

	m_pDisplayModel->GetAnimInfobyLocator(m_pAnimInfo,pParentModel,pParentAnimInfo,m_dwLocator);
	//m_pDisplayModel->Display(m_pAnimInfo);
	m_pDisplayModel->SetDirLightEnable(false);
	m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
	m_pDisplayModel->ProcessVisibility(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
	m_pDisplayModel->ProcessEnvironmentMaps(pParentAnimInfo->GetViewMatrix());
	m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
	m_pDisplayModel->ProcessLocator(m_pAnimInfo);
	m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);

	m_pDisplayModel->RenderModelFlash(dwFlashColor);
}



/*
* 功能: 显示装备带异常状态颜色的特殊效果(强制接受灯光处理)
* 摘要: -
* 参数: pParentModel    - 父显示模型指针
*		 pParentAnimInfo - 父模型动画信息指针
*		 dwExColor		 - 异常的颜色
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.16
* 修改日志:
*	2008.03.11 - lpf
*		渲染异常状态的函数修改为RenderModelTransparent(),用以渲染透明武器
*	2009.04.24 - lpf
*		增加了判断父对象模型未载入完成则直接返回;
*/
void CSpecialEquip::DisplayForExStateColor(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo, DWORD dwExColor)
{
	if (!m_bVisiable || pParentModel == NULL || m_pDisplayModel == NULL)
		return;
	if (pParentModel->GetLoadState() != Load_Did || m_pDisplayModel->GetLoadState() != Load_Did)
		return;

	m_pDisplayModel->GetAnimInfobyLocator(m_pAnimInfo,pParentModel,pParentAnimInfo,m_dwLocator);
	//m_pDisplayModel->Display(m_pAnimInfo);
	m_pDisplayModel->SetDirLightEnable(false);
	m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
	m_pDisplayModel->ProcessVisibility(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
	m_pDisplayModel->ProcessEnvironmentMaps(pParentAnimInfo->GetViewMatrix());
	m_pDisplayModel->ProcessLocator(m_pAnimInfo);
	m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
	if ((dwExColor & 0xff000000) != 0xff000000)
	{
		dwExColor &= 0xff000000;
		dwExColor |= 0x00ffffff;
		m_pDisplayModel->SetModelColorCompel(dwExColor);
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->RenderModelTransparent();
	}
	else
	{
		m_pDisplayModel->SetModelColorCompel(dwExColor);
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->RenderModel();
	}

	//	//m_pDisplayModel->ProcessDirectionalLight(pCamera->GetPosition(), &vLightVector, dwColor);

	//m_pDisplayModel->RenderModel();


}