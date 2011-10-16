#include "StdAfx.h"
#include ".\backmodel.h"
#include "Other/ModelList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBackModel::CBackModel(void)
{
	m_pBackModel = NULL;
	m_pCamera = NULL;
	m_dwAmbient = 0xff808080;
	m_dwDiffuse = 0xff808080;
	m_vLightDirection = D3DXVECTOR3(0.7f,-0.7f,0.0f);
	m_bFogEnable = false;
	m_dwFogColor = 0xFFFFFFFF;
	m_fFogStart = 0.1f;
	m_fFogEnd   = 100.0f;
}

CBackModel::~CBackModel(void)
{
	SAFE_DELETE(m_pCamera);
	SAFEDESTROY(m_pBackModel);
}

// 读取背景模型
void CBackModel::Load(const char* strPath)
{
	char strFileName[MAX_PATH];

	if (m_pBackModel == NULL)
	{
		m_pBackModel = new GameModel;
		sprintf(strFileName, "%s/model", strPath);
		m_pBackModel->Create(strFileName);
		m_pBackModel->SetGroupVisible(TRUE);
		m_pBackModel->SelectDefaultPart();
		m_pBackModel->EnableParticleEmitter(TRUE);
		m_pBackModel->EnableTrailEmitter(TRUE);
	}

	if (!m_pCamera) 
	{
		m_pCamera = new render::Camera;
		/////////////////////////////////////////////////

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

		int iWidth = pInterface->GetWndWidth();
		int iHeight= pInterface->GetWndHeight();

		float aspect = (float)iWidth/(float)iHeight;
		m_pCamera->SetAspect(aspect);	

		utility::File file;

		sprintf(strFileName, "%s/camera", strPath);
		if (file.Open(strFileName))
		{
			utility::Text script;
			script.Create(&file);
			utility::Text::Node *m_pCameraNode = script.GetRootNode()->GetChildNode("camera");
			utility::Text::Node *pPositionNode = m_pCameraNode->GetChildNode("position");
			ASSERT(pPositionNode);
			m_pCamera->GetPosition()->x = pPositionNode->GetVar("x")->GetFloatValue();
			m_pCamera->GetPosition()->y = pPositionNode->GetVar("y")->GetFloatValue();
			m_pCamera->GetPosition()->z = pPositionNode->GetVar("z")->GetFloatValue();
			utility::Text::Node *pViewNode = m_pCameraNode->GetChildNode("view");
			m_pCamera->GetView()->x = pViewNode->GetVar("x")->GetFloatValue();
			m_pCamera->GetView()->y = pViewNode->GetVar("y")->GetFloatValue();
			m_pCamera->GetView()->z = pViewNode->GetVar("z")->GetFloatValue(); 

			m_pCamera->SetFOVY(m_pCameraNode->GetVar("fovy")->GetFloatValue());
			m_pCamera->SetNearClipPlane(m_pCameraNode->GetVar("near")->GetFloatValue());
			m_pCamera->SetFarClipPlane(m_pCameraNode->GetVar("far")->GetFloatValue());
			m_pCamera->SetAspect(m_pCameraNode->GetVar("aspect")->GetFloatValue());
			script.Destroy();
			file.Close();
		}
		sprintf(strFileName, "%s/lightdirection", strPath);
		if (file.Open(strFileName))
		{
			utility::Text script;
			script.Create(&file);
			utility::Text::Node *m_pLightNode = script.GetRootNode();
			
			m_vLightDirection.x = m_pLightNode->GetVar("x")->GetFloatValue();
			m_vLightDirection.y = m_pLightNode->GetVar("y")->GetFloatValue();
			m_vLightDirection.z = m_pLightNode->GetVar("z")->GetFloatValue();
			D3DXVec3Normalize(&m_vLightDirection,&m_vLightDirection);
			script.Destroy();
			file.Close();
		}
		sprintf(strFileName, "%s/lightcolor", strPath);
		if (file.Open(strFileName))
		{
			utility::Text script;
			script.Create(&file);
			utility::Text::Node *m_pLightNode = script.GetRootNode();
			
			m_dwAmbient = m_pLightNode->GetVar("ambient")->GetHexValue();
			m_dwDiffuse = m_pLightNode->GetVar("diffuse")->GetHexValue();
			script.Destroy();
			file.Close();
		}
		sprintf(strFileName, "%s/fog", strPath);
		if (file.Open(strFileName))
		{
			utility::Text script;
			script.Create(&file);
			utility::Text::Node *m_pNode = script.GetRootNode();
			
			m_bFogEnable = m_pNode->GetVar("enable")->GetBOOLValue();
			m_dwFogColor = m_pNode->GetVar("color")->GetHexValue();
			m_fFogStart = m_pNode->GetVar("start")->GetFloatValue();
			m_fFogEnd = m_pNode->GetVar("end")->GetFloatValue();

			script.Destroy();
			file.Close();
		}


		m_cBackModelAnimInfo.GetActionInfo()->dwCurActionStartTime = timeGetTime();//GetCurTickCount();有单位错误
	}
}

// 显示背景模型
void CBackModel::Display()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	render::Interface::Layer3D *pLayer3D   = pInterface->GetLayer3D();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pInterface->GetDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	pInterface->GetDevice()->SetTransform(D3DTS_VIEW,m_pCamera->GetViewMatrix());
	pInterface->GetDevice()->SetTransform(D3DTS_PROJECTION,m_pCamera->GetProjectionMatrix());

	if (m_pBackModel)
	{

		m_cBackModelAnimInfo.SetCurrentTime(timeGetTime());
		m_cBackModelAnimInfo.SetBillboard(m_pCamera->GetBillboard());
		m_cBackModelAnimInfo.SetBillboardY(m_pCamera->GetBillboardY());
		m_cBackModelAnimInfo.SetWorldMatrix(&matWorld);
		m_pBackModel->ProcessAnimJoint(&m_cBackModelAnimInfo);
		m_pBackModel->ProcessAnimTexture(&m_cBackModelAnimInfo);
		m_pBackModel->ProcessAnimUV(&m_cBackModelAnimInfo);
		m_pBackModel->ProcessParticleEmitter(&m_cBackModelAnimInfo);
		m_pBackModel->ProcessTrailEmitter(&m_cBackModelAnimInfo);
		m_pBackModel->ProcessLocator(&m_cBackModelAnimInfo);
		m_pBackModel->SetModelColor(m_dwAmbient);
		m_pBackModel->ProcessDirectionalLight(&m_vLightDirection,m_dwDiffuse);
		m_pBackModel->ProcessAnimColor(&m_cBackModelAnimInfo);
		m_pBackModel->ProcessVisibility(&m_cBackModelAnimInfo);
		m_pBackModel->RenderModel();

		pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE, m_bFogEnable);
		if(m_bFogEnable)
		{
			pInterface->GetDevice()->SetRenderState(D3DRS_FOGCOLOR, (DWORD)m_dwFogColor);
			pInterface->GetDevice()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
			pInterface->GetDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&m_fFogStart));
			pInterface->GetDevice()->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&m_fFogEnd));
		}

		particle::Manager::GetInstance()->Render();
		pLayer3D->Render();

	}
}
