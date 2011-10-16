#include "stdafx.h"
#include "3DIconEffect.h"

#include "../../EngineExtend/TextureProjective.h"
#include "../../GameClient/Game.h"
//因为Player从Shape继承来，所以包含了Player.h就可以不用包含Shape.h
//#include "../../GameClient/ClientApp/Shape.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../EngineExtend/DisplayModel.h"
#include "../cegui/cegui/include/RendererModules/Direct3D9/CEGUIDirect3D9Texture.h"

C3DIconEffect::C3DIconEffect(CEGUI::Window * owner):m_Shape(0)
{
    m_3DIconTextureID = -1;    
	m_DrawX = 0;
	m_DrawY = 0;
    m_DrawZ = 0;
	m_RotAngle = D3DXToRadian(0);
	m_RotDir = 0;
	m_Scal[0] = 1;//内部自动计算了缩放比例( m_ScalRatio = m_Scal / 100)
    m_Scal[1] = 1;
    m_Scal[2] = 1;
    m_bLeftRotation = false;
    m_bRightRotation = false;
    m_lRttType = CTextureProjective::TYPE_BODY;
	m_ActionNum = MAKEFOURCC('E','I','0','0');
    m_AniInfo = new AnimInfo;
    m_AniInfo->GetActionInfo()->dwCurAction = m_ActionNum;
    ownerWnd = owner;
    if (ownerWnd->getRenderingSurface() == NULL)
        ownerWnd->setUsingAutoRenderingSurface(true);
    if (ownerWnd && ownerWnd->getRenderingSurface())
        static_cast<CEGUI::RenderingWindow*>(ownerWnd->getRenderingSurface())->
        setRenderEffect(this);
}

C3DIconEffect::~C3DIconEffect()
{
	m_Shape = NULL;
    SAFE_DELETE(m_AniInfo);
}

int C3DIconEffect::getPassCount() const
{
    return 1;
}
void C3DIconEffect::performPreRenderFunctions(const int pass)
{

}

void C3DIconEffect::performPostRenderFunctions()
{

}

bool C3DIconEffect::realiseGeometry(CEGUI::RenderingWindow& window, CEGUI::GeometryBuffer& geometry)
{
    if(m_3DIconTextureID != -1 && m_Shape)
    {
        LPDIRECT3DTEXTURE9 IconTex = CTextureProjective::GetRTTtexture(0);
        //D3DXSaveTextureToFile("1_b.bmp",D3DXIFF_BMP,IconTex,NULL);
        if (IconTex)
        {
            CEGUI::Direct3D9Renderer *d3d9Render = static_cast<CEGUI::Direct3D9Renderer*>(CEGUI::System::getSingleton().getRenderer());
            CEGUI::Direct3D9Texture *tex = static_cast<CEGUI::Direct3D9Texture*>(&window.getTextureTarget().getTexture());
            tex->setDirect3D9Texture(IconTex);
           
            static const CEGUI::colour c(1, 1, 1, 1);

            const float qw = window.getSize().d_width ;
            const float qh = window.getSize().d_height;
            //const float tcx = qw * tex->getTexelScaling().d_x;
            //const float tcy =
            //    (window.getTextureTarget().isRenderingInverted() ? -qh : qh) *
            //    tex->getTexelScaling().d_y;

            // vertex 0
            vb[0].position   = CEGUI::Vector3(0.0f, 0.0f, 0.0f);
            vb[0].colour_val = c;
            vb[0].tex_coords = CEGUI::Vector2(0, 0);

            // vertex 1
            vb[1].position   = CEGUI::Vector3(0.0f, qh , 0.0f);
            vb[1].colour_val = c;
            vb[1].tex_coords = CEGUI::Vector2(0, 1);

            // vertex 2
            vb[2].position   = CEGUI::Vector3(qw,  qh, 0.0f);
            vb[2].colour_val = c;
            vb[2].tex_coords = CEGUI::Vector2(1, 1);

            // vertex 3
            vb[3].position   = CEGUI::Vector3(qw, qh, 0.0f);
            vb[3].colour_val = c;
            vb[3].tex_coords = CEGUI::Vector2(1, 1);

            // vertex 4
            vb[4].position   = CEGUI::Vector3(qw, 0.0f, 0.0f);
            vb[4].colour_val = c;
            vb[4].tex_coords = CEGUI::Vector2(1, 0);

            // vertex 5
            vb[5].position   = CEGUI::Vector3(0.0f, 0.0f, 0.0f);
            vb[5].colour_val = c;
            vb[5].tex_coords = CEGUI::Vector2(0, 0);

            geometry.setActiveTexture(tex);
            geometry.appendGeometry(vb, 6);
        }
    }
    return false;
}
bool C3DIconEffect::update(const float elapsed, CEGUI::RenderingWindow& window)
{
    if(m_Shape && ownerWnd && ownerWnd->isVisible())
    {
        CTextureProjective::BeginRendToTexture(m_3DIconTextureID);
        m_AniInfo->GetActionInfo()->bStayToLastFrame = false;
        m_AniInfo->GetActionInfo()->bCurActionLooped = TRUE;
        m_AniInfo->SetCurrentTime(timeGetTime());
        if (m_AniInfo->GetActionInfo()->dwCurActionLoopCount > 10)
        {
            m_AniInfo->GetActionInfo()->dwCurAction = m_ActionNum;
            m_AniInfo->GetActionInfo()->bInterpolation = TRUE;
            m_AniInfo->GetActionInfo()->dwOldAction = MAKEFOURCC('E','I','0','0');
            m_AniInfo->GetActionInfo()->dwOldActionTimeElapse = m_AniInfo->GetActionInfo()->dwCurActionElapseTime;
            m_AniInfo->GetActionInfo()->fOldActionSpeedRate = m_AniInfo->GetActionInfo()->fCurActionSpeedRate;
            m_AniInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
            m_AniInfo->GetActionInfo()->dwInterpolationStartTime  = timeGetTime();
            m_AniInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
            m_AniInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('E','I','0','0');
            m_AniInfo->GetActionInfo()->dwCurActionLoopCount = 0;
        }
        D3DXMATRIX matWorld,matRot,matscal;
        D3DXMatrixIdentity(&matWorld);
        D3DXMatrixIdentity(&matRot);
        D3DXMatrixIdentity(&matscal);
        if(m_bLeftRotation)
        {
            m_RotAngle += 0.1f;
        }
        if(m_bRightRotation)
        {
            m_RotAngle -= 0.1f;
        }
        
        //移动
        D3DXMatrixTranslation(&matWorld,m_DrawX,m_DrawY,m_DrawZ);
        //旋转
        D3DXMatrixRotationY(&matRot,m_RotAngle);
        //缩放
        D3DXMatrixScaling(&matscal,m_Scal[0],m_Scal[1],m_Scal[2]);

        D3DXMatrixMultiply(&matRot,&matscal,&matRot);

        D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

        m_AniInfo->SetWorldMatrix(&matWorld);
        CTextureProjective::RendToTexture(m_Shape->GetDisplayModel(),m_AniInfo,m_lRttType);

        static D3DXMATRIX matJoint;
        static D3DXVECTOR3 *pPosBuf;
        if(m_Shape->GetType() == TYPE_PLAYER)
        {
            CWeapon *pRightHandWeapon = ((CPlayer*)m_Shape)->GetRightHandWeapon();
            CDisplayModel *pWeapon = pRightHandWeapon->GetDisplayModel();
            DWORD dwHandLocat   = pRightHandWeapon->GetLocator();
            if (pWeapon)
            {
                AnimInfo *pani = pRightHandWeapon->GetAnimInfo();
                GameModel::Locator *pWeaponLoca = m_Shape->GetDisplayModel()->GetLocator(dwHandLocat);
                if(pWeaponLoca)
                {
                    matJoint = pWeaponLoca->GetNowWorldMatrix();
                    pPosBuf = pWeaponLoca->GetMesh()->GetPosBuffer();
                
                    D3DXMatrixTranslation(&matWorld,pPosBuf[0].x,pPosBuf[0].y,pPosBuf[0].z);
                    matWorld = matWorld * matJoint;
                    pani->SetWorldMatrix(&matWorld);
                    //ZeroMemory(pani->GetParticleEmitterLastTimeArray(),GAMEMODEL_MAXPARTICLEEMITTER);
                    CTextureProjective::RendToTexture(pWeapon,pani,m_lRttType);
                    //ZeroMemory(pani->GetParticleEmitterLastTimeArray(),GAMEMODEL_MAXPARTICLEEMITTER);
                }
            }
        }

        CTextureProjective::EndRendToTexture();
        ownerWnd->invalidate(true);
    }
    return false;
}

void C3DIconEffect::SetIconShape(CShape *pShape)
{
    m_Shape = pShape;
}