
#include <stdafx.h>

#include "..\..\..\FrostEngine\utility.h"
#include "..\..\..\FrostEngine\console.h"
#include "..\..\..\FrostEngine\math.h"

#include "..\..\..\GameEngine\DisplayModel.h"
#include "..\..\..\GameEngine\TextureProjective.h"
#include "..\..\..\GameEngine\gamemap.h"
#include "..\..\..\GameEngine\GameModelManager.h"
#include "..\..\..\GameEngine\gameselectray.h"

#include "..\..\..\..\public\tools.h"

#include "specialequip.h"
//#include "clientregion.h"

#include "ModelTools.h"


CSpecialEquip::CSpecialEquip()
:m_pDisplayModel(NULL)
{
	m_wModelID = 0;
	m_dwLevel  = 0;
	m_dwType   = 0000;
	m_bVisiable = FALSE;
	m_pAnimInfo = new AnimInfo();
	ZeroMemory(m_pAnimInfo->GetActionInfo(),sizeof(AnimInfo::tagActionInfo));

	Create();
}

CSpecialEquip::~CSpecialEquip()
{
	Destroy();
	SAFE_DELETE(m_pAnimInfo);
}

void CSpecialEquip::Create()
{
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
	m_wModelID = psequip->m_wModelID;
	m_dwLevel = psequip->m_dwLevel;
	m_dwType = psequip->m_dwType;
	m_eStatus = psequip->m_eStatus;
	m_bVisiable = psequip->m_bVisiable;
	m_dwLocator = psequip->m_dwLocator;
};
void CSpecialEquip::RenderToScreen(int iX,int iY,float fScale,float fRotation,render::Camera* pcamera,D3DXMATRIX* matmodelworld,D3DXMATRIX* matworld)
{
	/*if (!m_bVisiable || m_pLocator == NULL)
	{
		return;
	}
	render::Interface *pInterface = render::Interface::GetInstance();
	static D3DXMATRIX matScreenWorld,matbone,matmodel;
	model::Mesh *pMesh =  m_pLocator->GetMesh();
	D3DXVECTOR3 *BACK_POS = pMesh->GetPosBuffer();
	matbone = m_pLocator->GetNowModelMatrix();
	D3DXMatrixTranslation(&matScreenWorld,BACK_POS[0].x,BACK_POS[0].y,BACK_POS[0].z);
	D3DXMatrixMultiply(&matScreenWorld,&matScreenWorld,&matbone);

	D3DXMatrixMultiply(&matmodel,&matScreenWorld,matmodelworld);

	GameModel *pModel = GetGame()->GetModelResource()->Get(m_dwType,m_wModelID);
	if (!pModel)
	{
		return;
	}
	m_pAnimInfo->SetWorldMatrix(&matmodel);
	m_pAnimInfo->SetCurrentTime(timeGetTime());
	m_pAnimInfo->SetBillboard(pcamera->GetBillboard());
	m_pAnimInfo->SetBillboardY(pcamera->GetBillboardY());
	
	pModel->SetGroupVisible(TRUE);
	pModel->SelectDefaultPart();
	pModel->SetGroupLevel(m_dwLevel);
	pModel->EnableParticleEmitter(TRUE);
	pModel->EnableTrailEmitter(TRUE);

	pModel->ProcessAnimJoint(m_pAnimInfo);
	pModel->ProcessAnimTexture(m_pAnimInfo);
	pModel->ProcessAnimUV(m_pAnimInfo);
	pModel->ProcessParticleEmitter(m_pAnimInfo);
	pModel->ProcessTrailEmitter(m_pAnimInfo);
	
	pModel->ProcessEnvironmentMaps(matworld);
	pModel->ProcessAnimColor(m_pAnimInfo);
	pModel->ProcessVisibility(m_pAnimInfo);
	pModel->ProcessLocator(m_pAnimInfo);
	pModel->DrawModel();*/

}
void CSpecialEquip::Render(render::Camera *pCamera,GameMap *pMap,BOOL alpha)
{
	//if (!pMap || !m_pLocator)
	//	return;
	//
	//static D3DXMATRIX matWorld,matbone;
	//static DWORD dwCurTime;
	//
	//model::Mesh *pMesh =  m_pLocator->GetMesh();
	//D3DXVECTOR3 *BACK_POS = pMesh->GetPosBuffer();
	//matbone = m_pLocator->GetNowWorldMatrix();
	//D3DXMatrixTranslation(&matWorld,BACK_POS[0].x,BACK_POS[0].y,BACK_POS[0].z);
	//D3DXMatrixMultiply(&matWorld,&matWorld,&matbone);

	//dwCurTime = timeGetTime();

	//m_pAnimInfo->SetBillboard(pCamera->GetBillboard());
	//m_pAnimInfo->SetBillboardY(pCamera->GetBillboardY());
	//m_pAnimInfo->SetWorldMatrix(&matWorld);
	//m_pAnimInfo->SetCurrentTime(dwCurTime);

	//GameModel *pModel = GetGame()->GetModelResource()->Get(m_dwType,m_wModelID);
	//if (!pModel)
	//{
	//	return;
	//}
	//pModel->SetGroupVisible(TRUE);
	//pModel->SelectDefaultPart();
	//pModel->SetGroupLevel(m_dwLevel);
	//pModel->EnableParticleEmitter(TRUE);
	//pModel->EnableTrailEmitter(TRUE);

	//pModel->ProcessAnimJoint(m_pAnimInfo);
	//pModel->ProcessAnimTexture(m_pAnimInfo);
	//pModel->ProcessAnimUV(m_pAnimInfo);
	//pModel->ProcessParticleEmitter(m_pAnimInfo);
	//pModel->ProcessTrailEmitter(m_pAnimInfo);
	//pModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());

	//pModel->ProcessLocator(m_pAnimInfo);

	////方向光 + 地表颜色
	//pModel->SetModelColor(pMap->GetSunLightAmbientColor());
	//pModel->ProcessDirectionalLight(pCamera->GetPosition(),pMap->GetSunLightDirection(),pMap->GetSunLightDiffuseColor());

	//pModel->ProcessAnimColor(m_pAnimInfo);
	//pModel->ProcessVisibility(m_pAnimInfo);
	//if (alpha)
	//{
	//	pModel->MultiplyModelAlpha(0.1f);
	//	pModel->RenderModelTransparent();
	//}
	//else
	//{
	//	pModel->RenderModel();
	//}
	
	//RenderShadow();


}
void CSpecialEquip::RenderShadow()
{
	//GameModel *pModel = GetGame()->GetModelResource()->Get(CModelResource::TYPE_SHADOW,m_wModelID);

	//pModel->DrawModelShadow();	
}

//设置显示模型
void CSpecialEquip::SetDisplayModel()
{
	if(!m_bVisiable)
	{
		Destroy();
		return;
	}
	// 从文件读取图像数据
	char pszFileName[_MAX_PATH];
	GetModelFormatString(m_dwType,m_wModelID,pszFileName);
	//if(!GetGame()->IsFileExist(pszFileName))
	//{
	//	char strError[512]="";
	//	sprintf(strError,"model文件%s不存在。",pszFileName);
	//	PutDebugString(strError);
	//	OutputError("Model ID : %d %d 不存在\n",m_dwType,m_wModelID);
	//	return;
	//}

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
	m_pDisplayModel = pModelManager->CreateDisplayModel(pszFileName);

	m_pDisplayModel->ShowAllGroup();
}

/*
 * 功能: 设置显示模型
 * 摘要: -
 * 参数: 模型文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.12.27
 */
void CSpecialEquip::SetDisplayModel(const char * pszFileName)
{
	if(!m_bVisiable)
	{
		Destroy();
		return;
	}

	// 从文件读取图像数据
	CGameModelManager * pModelManager = CGameModelManager::GetInstance();
	if(m_pDisplayModel)
	{
		// 如果相同则退出
		if(m_pDisplayModel->GetGameModelID() ==	pModelManager->GetGameModelID(pszFileName))
			return;
	}

	// 释放
	pModelManager->ReleaseDisplayModel(m_pDisplayModel);

	// 创建
	m_pDisplayModel = pModelManager->CreateDisplayModel(pszFileName);

	m_pDisplayModel->ShowAllGroup();
}

//隐藏
void CSpecialEquip::Hide()
{
	Destroy();
}

//显示
//pParentModel:绑定的父显示模型
void CSpecialEquip::Display(CDisplayModel *pParentModel,AnimInfo *pParentAnimInfo)
{
	if( !m_bVisiable || NULL == pParentModel || NULL == m_pDisplayModel)	return;
	if( m_pDisplayModel->GetLoadState() != Load_Did ) return;

	GameModel::Locator * pLocator = pParentModel->GetLocator(m_dwLocator);
	if(NULL == pLocator)	return;

	const D3DXMATRIX*  pmatWorld = pParentAnimInfo->GetWorldMatrix();
	const D3DXMATRIX*  pmatBillboard = pParentAnimInfo->GetBillboard();
	const D3DXMATRIX*  pmatBillboardY = pParentAnimInfo->GetBillboardY();
	DWORD dwCurTime = pParentAnimInfo->GetCurrentTime();

	D3DXMATRIX matWorld,matJoint;
	model::Mesh *pMesh = pLocator->GetMesh();
	D3DXVECTOR3 *pPosBuf = pMesh->GetPosBuffer();
	matJoint = pLocator->GetNowWorldMatrix();
	D3DXMatrixTranslation(&matWorld,pPosBuf[0].x,pPosBuf[0].y,pPosBuf[0].z);
	D3DXMatrixMultiply(&matWorld,&matWorld,&matJoint);

	m_pAnimInfo->SetWorldMatrix(&matWorld);
	m_pAnimInfo->SetViewMatrix(pParentAnimInfo->GetViewMatrix());
	m_pAnimInfo->SetProjectedMatrix(pParentAnimInfo->GetProjectedMatrix());
	m_pAnimInfo->SetScalMatrix(pParentAnimInfo->GetScalMatrix());
	m_pAnimInfo->SetBillboard(pmatBillboard);
	m_pAnimInfo->SetBillboardY(pmatBillboardY);
	m_pAnimInfo->SetCurrentTime(dwCurTime);
	m_pDisplayModel->Display(m_pAnimInfo);
}