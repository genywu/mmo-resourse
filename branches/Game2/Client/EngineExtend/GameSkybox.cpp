#include "StdAfx.h"
#include "../Engine/console.h"
#include "../Engine/utility.h"
#include "../Engine/render.h"
#include "../Engine/model.h"
#include "gamemodelmanager.h"
#include "GameSkybox.h"
GameSkybox::GameSkybox(void)
{
	m_pDisplayModel = NULL;
	m_pAnimInfo = NULL;
	m_dwModelID = 0;
	m_dwScal = 0;
	m_pAnimInfo = new AnimInfo();
	ZeroMemory(m_pAnimInfo->GetActionInfo(),sizeof(AnimInfo::tagActionInfo));
	m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
	m_pAnimInfo->GetActionInfo()->bCurActionLooped = true;
	D3DXMatrixIdentity(&m_matscal);
}

GameSkybox::~GameSkybox(void)
{
}


BOOL GameSkybox::Create(DWORD ModelID)
{
	m_dwModelID = 0;
	// 从文件读取图像数据
	char pszFileName[_MAX_PATH];
	sprintf(pszFileName,"model/skybox/%03d/model", ModelID);
	CGameModelManager* pModelManager = CGameModelManager::GetInstance();
	string strTempt(pszFileName);
	CheckRFileStr(strTempt);
	bool bFlag = CClientResource::GetInstance()->IsFileExist(strTempt.c_str());
	if(bFlag==false)
	{
		bFlag = CheckFile(pszFileName);
	}
	if (!bFlag)
	{
		Log4c::Error(ROOT_MODULE,"%-15s model file(%s) not exist。",__FUNCTION__,pszFileName);
		OutputError("Model ID : skybox %d not exist。",ModelID);
		return FALSE;
	}
	m_dwModelID = ModelID;
	if(m_pDisplayModel)
	{
		//如果相同则退出
		if(m_pDisplayModel->GetGameModelID() ==
			pModelManager->GetGameModelID(pszFileName))
			return TRUE;
	}
	//释放
	pModelManager->ReleaseDisplayModel(m_pDisplayModel);
	//创建
	m_pDisplayModel = pModelManager->CreateDisplayModel(pszFileName,false);

	m_pDisplayModel->ShowAllGroup(false);



	return TRUE;
}
void GameSkybox::SetScal(float size)
{
	m_dwScal = (DWORD)size;
	D3DXMatrixScaling(&m_matscal,size,size,size);
}
void GameSkybox::Destroy()
{
	CGameModelManager::GetInstance()->ReleaseDisplayModel(m_pDisplayModel);
	m_pDisplayModel = NULL;
	SAFE_DELETE(m_pAnimInfo);
}

void GameSkybox::Render(render::Camera *pCamera)
{
	if (m_pDisplayModel == NULL) return;
	if( m_pDisplayModel->GetLoadState() != Load_Did ) return;
	D3DXMATRIX matpro,matworld;
	m_pAnimInfo->SetupAnimInfo(pCamera->GetPosition()->z,pCamera->GetPosition()->x,pCamera->GetPosition()->y,0,pCamera);
	D3DXMatrixPerspectiveFovLH(&matpro,pCamera->GetFOVY(),pCamera->GetAspect(),pCamera->GetNearPlane(),2000);
	//render::Interface::GetInstance()->GetDevice()->SetTransform(D3DTS_VIEW,pCamera->GetViewMatrix());
	//render::Interface::GetInstance()->GetDevice()->SetTransform(D3DTS_PROJECTION,&matpro);
	render::Interface::GetInstance()->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	render::Interface::GetInstance()->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	matworld = *m_pAnimInfo->GetWorldMatrix();
	D3DXMatrixMultiply(&matworld,&m_matscal,&matworld);
	m_pAnimInfo->SetWorldMatrix(&matworld);
	m_pAnimInfo->SetViewMatrix(pCamera->GetViewMatrix());
	m_pAnimInfo->SetProjectedMatrix(&matpro);
	m_pDisplayModel->SetDirLightEnable(FALSE);
	m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
	m_pDisplayModel->ProcessVisibility(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
	m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
	m_pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
	m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
	m_pDisplayModel->ProcessLocator(m_pAnimInfo);
	m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
	m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
	m_pDisplayModel->DrawModel();

}