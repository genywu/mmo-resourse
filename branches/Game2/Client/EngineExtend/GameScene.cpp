#include "StdAfx.h"
#include "GameScene.h"
#include "GameMap.h"
#include "GameEnvCreature.h"
#include "GameEnvEffect.h"
#include "GameSkybox.h"
#include "GameSelectRay.h"

GameScene::GameScene()
{
    m_pFrustum = NULL;
    m_pOriginalCamera = NULL;
    m_pGameMap= NULL;
    m_pGameEnvCreature= NULL;
    m_pGameEnvEffect= NULL;
    m_pGameSkyBox= NULL;    
    m_pFrustum = new render::Frustum;
    m_pOriginalCamera = new render::Camera;
    m_pCamera = new render::Camera; 
    m_pGameMap = new GameMap;
    m_bFadeIn = true;
}

GameScene::~GameScene()
{
    Destroy();
}

void GameScene::CreateSence(const char * map, const char * camera,
                            const char * envCreature, const char * enveffect,bool isFadeIn)
{
    // 读取3d背景
    m_pGameMap->Create(map);
    ReadCamera(m_pOriginalCamera,camera);

    //读取环境生物
    
    if (utility::File::FileExist(envCreature))
    {
        utility::File file;
        utility::Text s;
        utility::Text::Node * pN = NULL;
        string str = map;

        file.Open(envCreature);
        s.Create(&file);
        file.Close();

        pN = s.GetRootNode()->GetChildNode("list");
        if (pN != NULL && pN->GetVarCount() != 0)
        {
            str += pN->GetVar(0)->GetStringValue();
            m_pGameEnvCreature = GameBaseEnvCreature::CreateEnvCreature(str.c_str());
        }

        s.Destroy();
    }

    //读取环境效果
    if (utility::File::FileExist(enveffect))
    {
        m_pGameEnvEffect = GameBaseEnvEffect::CreateEnvEffect(enveffect);
    }

    ////读取天空
    //char szSkyBox[] = "model/interface/login2/map/skybox/skybox";
    //if (utility::File::FileExist(szSkyBox))
    //{
    //	m_pGameSkyBox = new GameSkybox;
    //	m_pGameSkyBox->Create(szSkyBox);
    //}

    m_bFadeIn = isFadeIn;
    m_dwFadeInTimeStart = timeGetTime();
    m_dwFadeInTimeLength = 4000;

}

//	读取摄像机
void GameScene::ReadCamera(render::Camera *pCamera,const char *szName)
{
    utility::File file;
    if (file.Open(szName))
    {
        utility::Text script;
        script.Create(&file);
        utility::Text::Node *pCameraNode = script.GetRootNode()->GetChildNode("camera");
        utility::Text::Node *pPositionNode = pCameraNode->GetChildNode("position");
        ASSERT(pPositionNode);
        pCamera->GetPosition()->x = pPositionNode->GetVar("x")->GetFloatValue();
        pCamera->GetPosition()->y = pPositionNode->GetVar("y")->GetFloatValue();
        pCamera->GetPosition()->z = pPositionNode->GetVar("z")->GetFloatValue();
        utility::Text::Node *pViewNode = pCameraNode->GetChildNode("view");
        pCamera->GetView()->x = pViewNode->GetVar("x")->GetFloatValue();
        pCamera->GetView()->y = pViewNode->GetVar("y")->GetFloatValue();
        pCamera->GetView()->z = pViewNode->GetVar("z")->GetFloatValue(); 

        pCamera->SetFOVY(pCameraNode->GetVar("fovy")->GetFloatValue());
        pCamera->SetNearClipPlane(pCameraNode->GetVar("near")->GetFloatValue());
        pCamera->SetFarClipPlane(pCameraNode->GetVar("far")->GetFloatValue());
        pCamera->SetAspect(pCameraNode->GetVar("aspect")->GetFloatValue());
        script.Destroy();
        file.Close();

        *m_pCamera = *m_pOriginalCamera;
    }
}

//	销毁
void GameScene::Destroy()
{
    SAFEDELETE(m_pFrustum);
    SAFEDELETE(m_pOriginalCamera);
    SAFEDELETE(m_pCamera);
    SAFEDESTROY(m_pGameMap);
    SAFEDESTROY(m_pGameEnvCreature);
    SAFEDESTROY(m_pGameEnvEffect);
    SAFEDESTROY(m_pGameSkyBox);
}



/*
* 功能: 渲染背景
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.07.01 - lpf
*		修改了环境生物处理时,更新坐标的参考点为摄象机视点
*	2008.11.14 - lpf
*		修改了关于环境生物“Update()”的调用,增加了一个平接头体的参数传入
*/
void GameScene::RenderLayerBack()
{
    DWORD dwCurrentTime = timeGetTime();

    /*if (!m_bFadeIn)
    {
    static DWORD dwLastCamTime = dwCurrentTime;
    static DWORD dwCamTimeWait = 5000 + rand() % 5000;
    static D3DXVECTOR3 vCamPos = *m_pOriginalCamera->GetPosition();
    static D3DXVECTOR3 vLastCamPos = vCamPos;
    const float fCamRatio = 0.05f;
    if (dwCurrentTime - dwLastCamTime >= dwCamTimeWait)
    {
    vLastCamPos = vCamPos;

    float fX = 0.5f - (float)rand() / (float)RAND_MAX;
    float fY = 0.5f - (float)rand() / (float)RAND_MAX;
    float fZ = 0.5f - (float)rand() / (float)RAND_MAX;

    vCamPos = *m_pOriginalCamera->GetPosition();
    vCamPos.x += fX * fCamRatio;
    vCamPos.y += fY * 0.02f;
    vCamPos.z += fZ * fCamRatio;

    dwCamTimeWait = 5000 + rand() % 5000;
    dwLastCamTime = dwCurrentTime;
    }
    *m_pCamera = *m_pOriginalCamera;
    D3DXVec3Lerp(m_pCamera->GetPosition(),&vLastCamPos,&vCamPos,(float)(dwCurrentTime - dwLastCamTime) / (float)dwCamTimeWait);
    }*/

    render::Interface *pInterface = render::Interface::GetInstance();
    render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
    render::Interface::Layer3D *pLayer3D   = pInterface->GetLayer3D();

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    pInterface->GetDevice()->SetTransform(D3DTS_WORLD,&matWorld);
    pInterface->GetDevice()->SetTransform(D3DTS_VIEW,m_pOriginalCamera->GetViewMatrix());
    pInterface->GetDevice()->SetTransform(D3DTS_PROJECTION,m_pCamera->GetProjectionMatrix());

    if (m_pGameMap->GetFogEnable())
    {
        DWORD dwFogColor = m_pGameMap->GetFogColor();
        float fFogStart  = m_pGameMap->GetFogStart();
        float fFogEnd    = m_pGameMap->GetFogEnd();
        pInterface->Clear(dwFogColor);
        pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE,TRUE);
        pInterface->GetDevice()->SetRenderState(D3DRS_FOGCOLOR, (DWORD)dwFogColor);
        pInterface->GetDevice()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
        pInterface->GetDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&fFogStart));
        pInterface->GetDevice()->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&fFogEnd));
    }
    else
    {
        pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE,FALSE);
    }


    RECT rc = {0,0,pInterface->GetWndWidth(),pInterface->GetWndHeight()};
    static GameSelectRay cCameraRay;
    cCameraRay.CalculateRay(m_pCamera);

    ////天空
    //if (m_pGameSkyBox)
    //{
    //	m_pGameSkyBox->Render(m_pCamera);
    //	//测试版本
    //	if (g_bDebug && GetKeyboard()->GetCurrentKey() == DIK_F7)
    //	{
    //		SAFEDESTROY(m_pGameSkyBox);
    //		if (utility::File::FileExist("model/interface/login2/map/skybox/skybox"))
    //		{
    //			m_pGameSkyBox = new GameSkybox;
    //			m_pGameSkyBox->Create("model/interface/login2/map/skybox/skybox");
    //		}
    //	}
    //}

    //更新摄像机
    m_pFrustum->CalculateFrustum(m_pCamera->GetViewMatrix(),m_pCamera->GetProjectionMatrix());
    m_pGameMap->Update(m_pFrustum,false);
    DWORD dwAmbient = m_pGameMap->GetSunLightAmbientColorforBuild();
    DWORD dwDiffuse = m_pGameMap->GetSunLightDiffuseColor();
    DWORD dwSpecular = m_pGameMap->GetSunLightSpecularColor();
    const D3DXVECTOR3* vlightdir = m_pGameMap->GetSunLightDirection();
    static D3DLIGHT9 RegionLight;
    ZeroMemory( &RegionLight, sizeof(D3DLIGHT9) );
    RegionLight.Ambient.a = (float)((dwAmbient&0xff000000)>>24)/255.0f;
    RegionLight.Ambient.r = (float)((dwAmbient&0x00ff0000)>>16)/255.0f;
    RegionLight.Ambient.g = (float)((dwAmbient&0x0000ff00)>>8)/255.0f;
    RegionLight.Ambient.b = (float)(dwAmbient&0x000000ff)/255.0f;
    RegionLight.Diffuse.a = (float)((dwDiffuse&0xff000000)>>24)/255.0f;
    RegionLight.Diffuse.r = (float)((dwDiffuse&0x00ff0000)>>16)/255.0f;
    RegionLight.Diffuse.g = (float)((dwDiffuse&0x0000ff00)>>8)/255.0f;
    RegionLight.Diffuse.b = (float)(dwDiffuse&0x000000ff)/255.0f;
    RegionLight.Specular.a = (float)((dwSpecular&0xff000000)>>24)/255.0f;
    RegionLight.Specular.r = (float)((dwSpecular&0x00ff0000)>>16)/255.0f;
    RegionLight.Specular.g = (float)((dwSpecular&0x0000ff00)>>8)/255.0f;
    RegionLight.Specular.b = (float)(dwSpecular&0x000000ff)/255.0f;
    D3DXVec3Normalize((D3DXVECTOR3*)&RegionLight.Direction,vlightdir);
    RegionLight.Attenuation0 = 1;
    RegionLight.Attenuation1 = 0;
    RegionLight.Attenuation2 = 0;
    RegionLight.Type       = D3DLIGHT_DIRECTIONAL;
    pLayer3D->SetLight(&RegionLight);
    pLayer3D->SetMaterial(&m_pGameMap->m_material);
    m_pGameMap->Render(&cCameraRay,m_pCamera,true);

    //环境生物
    if (m_pGameEnvCreature)
    {
        //D3DXVECTOR3 vTemp(67,8,80);
        m_pGameEnvCreature->Update(m_pCamera->GetView(), m_pFrustum);
        m_pGameEnvCreature->Render(m_pGameMap, m_pCamera);
        //测试版本
        //extern bool g_bDebug;
        //if (g_bDebug && GetKeyboard()->GetCurrentKey() == DIK_F7)
        //{
        //    SAFEDESTROY(m_pGameEnvCreature);
        //    m_pGameEnvCreature = GameBaseEnvCreature::CreateEnvCreature("model/interface/login2/map/envcreature");
        //}

    }

    particle::Manager::GetInstance()->Render();
    //pLayer2D->Update();
    pLayer3D->SetMipMap(FALSE);
    pLayer3D->Render();
    pLayer3D->Clear();

    //环境效果
    if (m_pGameEnvEffect)
    {
        D3DXVECTOR3 vTemp(0,0,0);
        m_pGameEnvEffect->Update(&vTemp);
        m_pGameEnvEffect->Render(m_pCamera->GetBillboard(),m_pCamera->GetBillboardY());
        //测试版本
        //if (g_bDebug && GetKeyboard()->GetCurrentKey() == DIK_F7)
        //{
        //    SAFEDESTROY(m_pGameEnvEffect);
        //    m_pGameEnvEffect = GameBaseEnvEffect::CreateEnvEffect("model/interface/login2/map/envEffect");
        //}

    }

    //GameGraphics2D::SetUIModelRendState();
    if (m_bFadeIn)
    {
        float fTimeElapse = (float)(dwCurrentTime - m_dwFadeInTimeStart);
        float fRatio = fTimeElapse / (float)m_dwFadeInTimeLength;
        if (fRatio > 1.0f)
        {
            fRatio = 1.0f;
            m_bFadeIn = FALSE;
        }
        render::Interface *pInterface = render::Interface::GetInstance();
        DWORD dwColor = (DWORD)(fRatio * 255);
        dwColor = (255 - dwColor) << 24;
        
    }

    render::Interface::GetInstance()->GetDevice()->SetRenderState( D3DRS_FOGENABLE, FALSE );
}

// 渲染前景
void GameScene::RenderLayerFront()
{
    DWORD dwCurrentTime = timeGetTime();
    render::Interface *pInterface = render::Interface::GetInstance();

    //if (!m_bFadeIn)
    //{
    //	static DWORD dwTime = dwCurrentTime;
    //	static DWORD dwTimeWait = 250;
    //	static int   iL = 255;
    //	static int   iLastL = 00;
    //	if (dwCurrentTime - dwTime > dwTimeWait)
    //	{
    //		iLastL = iL;
    //		iL = rand() % 255;
    //		dwTimeWait = 1000 + rand() % 5000;
    //		dwTime = dwCurrentTime;
    //	}
    //	DWORD dwLC = (DWORD)(iLastL + (int)(dwCurrentTime - dwTime) * (iL - iLastL) / (int)dwTimeWait);
    //}
    //计算淡入淡出
    if (m_bFadeIn)
    {
        float fTimeElapse = (float)(dwCurrentTime - m_dwFadeInTimeStart);
        float fRatio = fTimeElapse / (float)m_dwFadeInTimeLength;
        if (fRatio > 1.0f)
        {
            if (fRatio > 1.5f)
            {
                m_bFadeIn = FALSE;
            }
            fRatio = 1.0f;			
        }
        render::Interface *pInterface = render::Interface::GetInstance();
        DWORD dwColor = (DWORD)(fRatio * 255);
        dwColor = (255 - dwColor) << 24;
        //GameGraphics2D::GetInstance()->DrawSolidQuad(0,0,pInterface->GetWndWidth(),pInterface->GetWndHeight(),dwColor);

        //static D3DXVECTOR3 vCamPos = *m_pOriginalCamera->GetPosition() + D3DXVECTOR3(-0.15f,0.0f,0.0f);
        //*m_pCamera = *m_pOriginalCamera;
        //D3DXVec3Lerp(m_pCamera->GetPosition(),&vCamPos,m_pOriginalCamera->GetPosition(),fRatio);
    }

}