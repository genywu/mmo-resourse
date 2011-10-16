///
/// @file MiniMapFrame.cpp
///
///
#include "StdAfx.h"
#include "../UIDef.h"
#include "../UIManager.h"
#include "../../GameClient/ClientApp/MiniMap.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/ClientRegion.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/GameControl.h"

// copy from Luvinia
static void GetPosOnMiniMap(float wndX, float wndY, float lUIx, float lUIy, 
                            float mx, float my,
                            float &lMapx,float &lMapy)
{
	lMapx = lMapy = -1;
	float posX = lUIx,posY = lUIy;
	int iDir = 0;
	CMiniMap *pMinimap = GetGame()->GetRegion()->GetMiniMap();
	if(pMinimap && pMinimap->InTheCircinal(64,64,60,posX,posY,iDir))
	{
		int iMode = pMinimap->GetMapMode();
		float fScalefactor = pMinimap->GetScaleFactor();
		float fScale = fScalefactor*iMode;
		long lx = GetGame()->GetMainPlayer()->GetTileX();
		long ly = GetGame()->GetMainPlayer()->GetTileY();
		long width = (long)(abs(lx+ly) / sqrtf(1+1) * fScale);
		long height = (long)(abs(lx - ly - GetGame()->GetRegion()->GetHeight()) / sqrtf(1+1) * fScale);

		float lStartX = /*m_pMiniComp->GetXOnWindow()*/wndX + 64 - width;
		float lStartY = /*m_pMiniComp->GetYOnWindow()*/wndY + 64 - height;

		float lSizeX = (GetGame()->GetRegion()->GetWidth() * fScale / 1.414f);
		float lSizeY = (GetGame()->GetRegion()->GetHeight() * fScale / 1.414f);
		float lCurX = /*GetMouse()->GetMouseX()*/mx - lStartX;
		float lCurY = /*GetMouse()->GetMouseY()*/my - lStartY;
		posX = ((lCurX - lCurY + lSizeX) / sqrtf(1+1));
		posY = ((lCurX + lCurY - lSizeY) / sqrtf(1+1));

		lMapx = (posX / fScale);
		lMapy = (posY / fScale)+(iMode/fScalefactor);
	}
}

static void ClickMiniMapToMove(float wndX, float wndY, float mx, float my, float x, float y)
{
	if(!GetGame()->GetGameControl()->IsCanMoveCondition())
		return;
	// 用于返回场景坐标的临时变量
	float lx = -1,ly = -1;
	// 计算对应的场景坐标
	GetPosOnMiniMap(wndX, wndY, x, y, mx, my, lx, ly );
	if(lx >= 0 && ly >= 0)
	{
		if(GetGame()->GetGameControl()->GetMPActionMode() == CGameControl::MPAM_AutoAttack)
			GetGame()->GetGameControl()->AutoAttactStop();
		// 目的地无阻挡开始移动
		if(GetGame()->GetRegion()->GetBlock((int)lx,(int)ly) != CRegion::BLOCK_CANFLY &&
			GetGame()->GetRegion()->GetBlock((int)lx,(int)ly) != CRegion::BLOCK_UNFLY)
			GetGame()->GetGameControl()->AutoMoveTo((float)lx,(float)ly);
		//CImageAniIcon *pIcon = dynamic_cast<CImageAniIcon*>(m_pMiniComp->GetIcon());
		//if(pIcon)
		//{
		//	m_pMiniComp->SetInsets(x-64,0,0,y-64);
		//	// 图标不处于冷却动画中,更新冷却时间
		//	pIcon->SetFrameTime(50);
		//	pIcon->SetLoopCount(1);
		//}
	}
}

static bool OnLButtonDown (const CEGUI::EventArgs &args)
{
    CEGUI::Window *wnd = GetWndMgr().getWindow("MiniMapFrame");
    assert(wnd);
    const CEGUI::MouseEventArgs &mouseArg = (const CEGUI::MouseEventArgs&) args;

    const CEGUI::UVector2 &pos = wnd->getPosition();
    CEGUI::UDim dim( 0, 0 );
    CEGUI::Vector2 screenPos = CEGUI::CoordConverter::windowToScreen( *wnd, CEGUI::UVector2(dim, dim) );

    float mx =  mouseArg.position.d_x;
    float my =  mouseArg.position.d_y;
    float wndX =  screenPos.d_x;
    float wndY =  screenPos.d_y;
    float relX = mx - wndX;
    float relY = my - wndY;
    ClickMiniMapToMove( wndX, wndY, mx, my, relX, relY );
    return true;
}

CEGUI::Window* InitMiniMapFrame()
{
    CEGUI::Window* wnd = LoadUI("MiniMapFrame");
	wnd->setVisible(false);

    wnd->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(OnLButtonDown));
    return wnd;
}
