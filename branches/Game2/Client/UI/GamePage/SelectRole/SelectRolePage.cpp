#include "stdafx.h"
#include "SelectRolePage.h"
#include "SelectRoleEvent.h"
#include "../../../EngineExtend/GameScene.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../../EngineExtend/DisplayModel.h"
#include "../../../GameClient/Game.h"
#include "../../../Engine/Render.h"
#include "../../../EngineExtend/TextureProjective.h"
#include "../../../EngineExtend/GameMap.h"
#include "../../../ui/UIManager.h"
float SelectRolePage::s_RolePos[4] = 
{
    0.0f,0.0f,0.0f,0.0f,
};

int	SelectRolePage::m_CurrentAction;		// 当前动作
int  SelectRolePage::m_NextAction;           // 下一个动作

SelectRolePage::SelectRolePage():m_bRoleLeftRotate(false),m_bRoleRightRotate(false),m_event(NULL)
{
	m_event = new SREvent();
}

SelectRolePage::~SelectRolePage()
{
    
	SAFE_DELETE(m_event);
}

void SelectRolePage::LoadPage()
{
    if(NULL == m_SelectSence)
    {
        m_SelectSence = new GameScene;
        m_SelectSence->CreateSence("model/interface/selectchar/map",
            "model/interface/selectchar/map/camera_end",
            "model/interface/selectchar/map/envcreature",
            "model/interface/selectchar/map/enveffect");
    }
    CRFile* prfile = rfOpen("data/selectrolepos.ini");
    if (prfile)
    {
        stringstream stream;
        prfile->ReadToStream(stream);
        rfClose(prfile);
        stream >> s_RolePos[0]  >> s_RolePos[1]  >> s_RolePos[2]  >> s_RolePos[3];
    }
    SetLayOutFileName("SelectRolePage.layout");   
	if(m_event)
	{
		m_event->SetEventType(ON_LOAD);
		ProcessPageEvent(m_event);
	}
}

void SelectRolePage::ReleasePage()
{
   	m_event->SetEventType(ON_RELEASE);
	ProcessPageEvent(m_event);
    SAFE_DELETE(m_SelectSence);
}

bool SelectRolePage::Open()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if (pPageWindow == NULL)
    {
        LoadPage();
        pPageWindow = GetPageWindow();
    }
	if(m_event)
	{
		m_event->SetEventType(ON_OPEN);
		ProcessPageEvent(m_event);
	}
    pPageWindow->setVisible(true);
    pPageWindow->activate();
    return true;
}

bool SelectRolePage::Close()
{
    CEGUI::Window *pPageWindow = GetPageWindow();
    if(pPageWindow)
        pPageWindow->setVisible(false);
    else
        return false;
    return true;
}

void SelectRolePage::Display()
{
	//临时添加测试3DIcon
	if(m_event)
		m_event->SetEventType(ON_OPEN);
    m_SelectSence->RenderLayerBack();
    CPlayer *pCurPlayer = m_event->GetSelPlayer();
    render::Interface *pInterface = render::Interface::GetInstance();
    render::Interface::Layer3D *pLayer3D   = pInterface->GetLayer3D();
    DWORD dwCurrentTime = timeGetTime();
    // 显示角色
    if (pCurPlayer && pCurPlayer->GetDisplayModel()
       && pCurPlayer->GetDisplayModel()->GetLoadState() == Load_Did)
    {
        static WORD dwState[] = 
        {
            CShape::STATE_PEACE,		// 取武器
            CShape::STATE_FIGHT,		// 挥舞
            CShape::STATE_FIGHT,		// 站定
        };
        static WORD dwAct[] = 
        {
            CShape::ACT_SEL_X0,			// 取武器
            CShape::ACT_SEL_X1,			// 挥舞
            CShape::ACT_SEL_X2,			// 站定
        };

        // 判断角色模型旋转
        if (m_bRoleLeftRotate)
            s_RolePos[3] += 0.1f;

        if (m_bRoleRightRotate)
            s_RolePos[3] -= 0.1f;

        //判断播放结束
        CDisplayModel *pDisplayModel = pCurPlayer->GetDisplayModel();
        AnimInfo *pAnimInfo = pCurPlayer->GetAnimInfo();
        pAnimInfo->GetActionInfo()->bStayToLastFrame = false;

        pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
        pAnimInfo->SetCurrentTime(dwCurrentTime);
        if (pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 10 && pAnimInfo->GetActionInfo()->dwCurAction == MAKEFOURCC('E','I','0','0'))
        {
            pAnimInfo->GetActionInfo()->bInterpolation = TRUE;
            pAnimInfo->GetActionInfo()->dwOldAction = pAnimInfo->GetActionInfo()->dwCurAction;
            pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = pAnimInfo->GetActionInfo()->dwCurActionElapseTime;
            pAnimInfo->GetActionInfo()->fOldActionSpeedRate = pAnimInfo->GetActionInfo()->fCurActionSpeedRate;
            pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
            pAnimInfo->GetActionInfo()->dwInterpolationStartTime  = dwCurrentTime;
            pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
            pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('E','I','0','0');
            pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
        }
       /* else if (pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 0 && pAnimInfo->GetActionInfo()->dwCurAction == MAKEFOURCC('E','I','0','0'))
        {
            pAnimInfo->GetActionInfo()->bInterpolation = TRUE;
            pAnimInfo->GetActionInfo()->dwOldAction = pAnimInfo->GetActionInfo()->dwCurAction;
            pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = pAnimInfo->GetActionInfo()->dwCurActionElapseTime;
            pAnimInfo->GetActionInfo()->fOldActionSpeedRate = pAnimInfo->GetActionInfo()->fCurActionSpeedRate;
            pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
            pAnimInfo->GetActionInfo()->dwInterpolationStartTime  = dwCurrentTime;
            pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
            pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('E','I','0','0');
            pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
        }*/
        else
        {
            if (pAnimInfo->GetActionInfo()->dwCurAction != MAKEFOURCC('E','I','0','0')&&pAnimInfo->GetActionInfo()->dwCurAction != MAKEFOURCC('E','B','0','0'))
            {
                pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('E','I','0','0');
                pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
            }
        }
        
        //this->RoleLeftRotate();

        //bool bDisplayFinish = (pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 0);
        //if (bDisplayFinish)
        //{
        //	// 出招
        //	if (m_iCurrentAction < 2)
        //	{
        //		m_iCurrentAction++;
        //		g_bForceUpdate = true;
        //		pSelectedPlayer->SetAction(dwAct[m_iCurrentAction]);
        //	}
        //	else
        //	{
        //		pSelectedPlayer->SetAction(dwAct[m_iCurrentAction]);
        //		g_bForceUpdate = true;
        //	}
        //}

        // 显示坐标
        if (g_bDebug)
        {
            //s_RolePos[0]：y值 s_RolePos[1]h值 s_RolePos[2]X值 s_RolePos[1]
            if (GetAsyncKeyState(VK_UP))
                s_RolePos[1] += 0.02f;
            if (GetAsyncKeyState(VK_DOWN))
                s_RolePos[1] -= 0.02f;
            if (GetAsyncKeyState(VK_LEFT))
                s_RolePos[2] += 0.02f;
            if (GetAsyncKeyState(VK_RIGHT))
                s_RolePos[2] -= 0.02f;
            if (GetAsyncKeyState(VK_ADD))
                s_RolePos[0] += 0.02f;
            if (GetAsyncKeyState(VK_SUBTRACT))
                s_RolePos[0] -= 0.02f;
            if (GetAsyncKeyState(VK_NUMPAD0))
                s_RolePos[3] += 0.02f;
            if (GetAsyncKeyState(VK_DECIMAL))
                s_RolePos[3] -= 0.02f;
            char str[256];
            sprintf(str, "y:%f, h:%f, X:%f (rot:%f)", s_RolePos[0], s_RolePos[1], s_RolePos[2], s_RolePos[3]);
            GetGame()->OutPutText(0, 0, str, lstrlen(str), D3DCOLOR_ARGB(255,0,255,0));
        }
        // 渲染简单阴影
        render::Camera *pCamera = m_SelectSence->GetCamera();
        pDisplayModel->SetDirLightEnable(false);
        pAnimInfo->SetupAnimInfo(s_RolePos[2], s_RolePos[0], s_RolePos[1],s_RolePos[3],pCamera);
        //pAnimInfo->SetViewMatrix(m_pCamera->GetViewMatrix());
        //pAnimInfo->SetProjectedMatrix(m_pCamera->GetProjectionMatrix());
        pDisplayModel->ProcessAnimJoint(pAnimInfo);
        pDisplayModel->ProcessAnimTexture(pAnimInfo);
        pDisplayModel->ProcessAnimUV(pAnimInfo);
        pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
        pDisplayModel->ProcessParticleEmitter(pAnimInfo);
        pDisplayModel->ProcessTextureProjective(pAnimInfo);
        pDisplayModel->ProcessTrailEmitter(pAnimInfo);
        pDisplayModel->ProcessLocator(pAnimInfo);
        pDisplayModel->ProcessAnimColor(pAnimInfo);
        pDisplayModel->ProcessVisibility(pAnimInfo);
        pDisplayModel->RenderModel();
        pCurPlayer->GetLeftHandWeapon()->Display(dynamic_cast<CShape *>(pCurPlayer));
        pCurPlayer->GetRightHandWeapon()->Display(dynamic_cast<CShape *>(pCurPlayer));
        pCurPlayer->GetWing()->Display(dynamic_cast<CShape *>(pCurPlayer));
        pCurPlayer->GetHeadWear()->Display(dynamic_cast<CShape *>(pCurPlayer));
        //CTextureProjective::BeginRendShadow(D3DXVECTOR3(s_RolePos[0],s_RolePos[1],s_RolePos[2]), 3);
        //CTextureProjective::AddRendShadow(pDisplayModel,pAnimInfo);
        //CTextureProjective::AddRendShadow(pCurPlayer->GetLeftHandWeapon()->GetDisplayModel(),pCurPlayer->GetLeftHandWeapon()->GetAnimInfo());
        //CTextureProjective::AddRendShadow(pCurPlayer->GetRightHandWeapon()->GetDisplayModel(),pCurPlayer->GetRightHandWeapon()->GetAnimInfo());
        //CTextureProjective::AddRendShadow(pCurPlayer->GetWing()->GetDisplayModel(),pCurPlayer->GetWing()->GetAnimInfo());
        //CTextureProjective::EndRendShadow();
    }
    particle::Manager::GetInstance()->Render();
    //pLayer2D->Update();
    //pLayer3D->SetMaterial(&pScene->pGameMap->m_material);
    pLayer3D->SetFogEnable(m_SelectSence->GetGameMap()->GetFogEnable());
    pLayer3D->SetMipMap(FALSE);
    pLayer3D->RenderTest();
    pLayer3D->RenderBlendLow();
    pLayer3D->EndEffectsState();
    //DisplayTextureProjective();
    pLayer3D->RenderBlend();
    pLayer3D->Clear();

    // 界面显示
    UIManager::Render();
}

void SelectRolePage::AddPlayer(CPlayer* pPlayer)
{
	if(m_event)
	{
		m_event->AddPlayer(pPlayer);
	}
}

void SelectRolePage::ClearPlayer()
{
	if(m_event)
		m_event->ClearPlayer();
}

ulong SelectRolePage::GetPlayerCount()
{
    ulong count = 0;
    if(m_event)
        count = (ulong)m_event->GetPlayerCount();
    return count;
}

CEGUI::Window* SelectRolePage::GetWindow(const char* WindwoName)
{
	return CEGUI::WindowManager::getSingleton().getWindow(WindwoName);
}

CPlayer * SelectRolePage::GetSelPlayer()
{
    //return SREvent::GetSelPlayer();
	return m_event->GetSelPlayer();
}

CPlayer* SelectRolePage::GetPlayer(ulong index)
{
	return m_event->GetPlayer(index);
}