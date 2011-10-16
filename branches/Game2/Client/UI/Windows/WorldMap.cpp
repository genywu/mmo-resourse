#include "stdafx.h"
#include "../UIDef.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/Other/PicList.h"
#include "../../GameClient/ClientApp/clientregion.h"
#include "../../GameClient/ClientApp/other/RegionList.h"
#include "../../GameClient/ClientApp/Monster.h"
void SelectedMap(long lMapID,CRegionList::MAP_TYPE lMapTyep)
{
    char strMap[64];
    // 场景地图
    if(lMapTyep == CRegionList::REGIONMAP_TYPE)
    {
        CRegionList::tagRegion *pRegion = CRegionList::GetRegion(lMapID);
        if (pRegion)
        {
            sprintf(strMap,"%d.jpg",lMapID);
            CEGUI::Window *WorldMapWnd = CEGUI::WindowManager::getSingleton().getWindow("WorldMap");
            CEGUI::GUISheet *pMapWnd = static_cast<CEGUI::GUISheet*>(WorldMapWnd->getChild("WorldMap/MapImage"));
            SetBackGroundImage(pMapWnd,"WoldMap","pictures/regions/",strMap,false);
        }
    }
}
bool OnOpenMap(const CEGUI::EventArgs &args)
{
    SelectedMap(GetGame()->GetMainPlayer()->GetRegionID(),CRegionList::REGIONMAP_TYPE);
    FireUIEvent("WorldMap","MAP_EVENT_ONPLAYERMOVE");
    return true;
}

bool UpdateToolTip(const CEGUI::EventArgs &args)
{
    CEGUI::MouseCursor& cursor = CEGUI::MouseCursor::getSingleton();
    CEGUI::Window *pMapWnd = GetWndMgr().getWindow("WorldMap/MapImage");
    CEGUI::Window *pTipWnd = GetWndMgr().getWindow("WorldMap/MapImage/MousePos"); 
    pTipWnd->setVisible(true);
    float fMouseX = cursor.getPosition().d_x;
    float fMouseY = cursor.getPosition().d_y;

    float fParentPosX = CEGUI::CoordConverter::windowToScreenX(*pTipWnd->getParent(),0.0f);
    float fParentPosY = CEGUI::CoordConverter::windowToScreenY(*pTipWnd->getParent(),0.0f);
    float fMapPosX = CEGUI::CoordConverter::windowToScreenX(*pMapWnd,0.0f);
    float fMapPosY = CEGUI::CoordConverter::windowToScreenY(*pMapWnd,0.0f);
    char tip[256] = "";
    sprintf_s<256>(tip,"[COLOR FFFFFF00]x[COLOR FFFFFFFF]:%0.0f [COLOR FFFFFF00]y[COLOR FFFFFFFF]:%0.0f",fMouseX-fMapPosX,fMouseY-fMapPosY);
    SetText(pTipWnd,tip);
    float ffinalposX = fMouseX - fMapPosX;
    float ffinalposY = fMouseY - fMapPosY;

    //坐标提示框位置调整
    //static const float fRight = fMapPosX + pMapWnd->getPixelSize().d_width;
    //static const float fbottom = fMapPosY + pMapWnd->getPixelSize().d_height;
    //static const float fTipWidth  = pTipWnd->getWidth().asAbsolute(pTipWnd->getParentPixelWidth());
    //static const float fTipHeight = pTipWnd->getHeight().asAbsolute(pTipWnd->getParentPixelHeight());
    //float fCurRight  = fTipWidth + CEGUI::CoordConverter::windowToScreenX(*pTipWnd,0.0f);
    //float fCurBottom = fTipHeight + CEGUI::CoordConverter::windowToScreenY(*pTipWnd,0.0f);

    //if (fCurRight > fRight)
    //{
    //    ffinalposX = ffinalposX - pTipWnd->getPixelSize().d_width;
    //}
    //if (fCurBottom > fbottom)
    //{
    //    ffinalposY = ffinalposY - pTipWnd->getPixelSize().d_height;
    //}
    pTipWnd->setPosition(CEGUI::UVector2(cegui_absdim(ffinalposX+25),
        cegui_absdim(ffinalposY)));
    return true;
}

bool OnClosePosToolTip(const CEGUI::EventArgs &args)
{
    CEGUI::Window *pTipWnd = GetWndMgr().getWindow("WorldMap/MapImage/MousePos");
    pTipWnd->setVisible(false);
    return true;
}

// 更新玩家小箭头位置和方向
bool updatePlayerPos(const CEGUI::EventArgs &args)
{
    if (!GetWndMgr().getWindow("WorldMap")->isVisible())
        return false;
    float posX = GetGame()->GetMainPlayer()->GetPosX();
    float posY = GetGame()->GetMainPlayer()->GetPosY();
   
    CEGUI::DefaultWindow *pPosWnd = WDefaultWindow(GetWndMgr().getWindow("WorldMap/MapImage/PlayerPosArrow"));
    pPosWnd->setPosition(CEGUI::UVector2(cegui_absdim(posX),
        cegui_absdim(posY)));

    float rot = D3DXToDegree(GetGame()->GetMainPlayer()->GetDirEx());
    pPosWnd->setRotation(CEGUI::Vector3(0,0,rot));
    return true;
}

bool updateShapePos(const CEGUI::EventArgs &args)
{
    CClientRegion *pRegion = GetGame()->GetRegion();
    if(!pRegion)
        return false;
    itBaseObject objIter = pRegion->GetObjectList()->begin();
    for (;objIter != pRegion->GetObjectList()->end(); ++objIter)
    {
        long lType = (*objIter)->GetType();
        switch (lType)
        {
        case TYPE_PLAYER:
            {
                if(*objIter != GetGame()->GetMainPlayer())
                {

                }
            }
        	break;
        case TYPE_MONSTER:
            {
                eNpcType mType = ((CMonster*)*objIter)->GeteNpcType();
                switch (mType)
                {
                case NT_Normal:
                	break;
                case NT_Monster:
                    break;
                }
            }
            break;
        case TYPE_COLLECTION:
            break;
        }
    }
    return true;
}

CEGUI::Window *InitWorldMap()
{
    CEGUI::Window* wnd = LoadUI("WorldMap");
    wnd->setVisible(false);
    wnd->setAlwaysOnTop(true);
    SetText(wnd,AppFrame::GetText("Base_116"));

    CEGUI::Window *pChechTransForm = GetWndMgr().getWindow("WorldMap/cDisTransPoint");
    SetText(pChechTransForm,AppFrame::GetText("WMap_1"));

    CEGUI::Window *pChechMonster = GetWndMgr().getWindow("WorldMap/cDisMonster");
    SetText(pChechMonster,AppFrame::GetText("WMap_2"));

    CEGUI::Window *pChechNpc = GetWndMgr().getWindow("WorldMap/cDisNpc");
    SetText(pChechNpc,AppFrame::GetText("WMap_3"));

    CEGUI::Window *pChechCollector = GetWndMgr().getWindow("WorldMap/cCollection");
    SetText(pChechCollector,AppFrame::GetText("WMap_4"));

    CEGUI::Window *pbCollector = GetWndMgr().getWindow("WorldMap/bSearch");
    SetText(pbCollector,AppFrame::GetText("WMap_5"));
    //设置关联事件
    wnd->subscribeEvent("Opened",CEGUI::Event::Subscriber(OnOpenMap));

    CEGUI::Window *pMapWnd = GetWndMgr().getWindow("WorldMap/MapImage");
    pMapWnd->subscribeEvent(CEGUI::Window::EventMouseMove,CEGUI::Event::Subscriber(UpdateToolTip));
    pMapWnd->subscribeEvent(CEGUI::Window::EventMouseEntersArea,CEGUI::Event::Subscriber(UpdateToolTip));
    pMapWnd->subscribeEvent(CEGUI::Window::EventMouseLeavesArea,CEGUI::Event::Subscriber(OnClosePosToolTip));
    
    //设置玩家位置小箭头图标
    CEGUI::DefaultWindow *pPosWnd = WDefaultWindow(GetWndMgr().getWindow("WorldMap/MapImage/PlayerPosArrow"));
    SetBackGroundImage(pPosWnd,"mapPlayerPosArrow","datafiles/imagesets/minimap","map-i.tga");
    wnd->subscribeEvent("MAP_EVENT_ONPLAYERMOVE",CEGUI::Event::Subscriber(updatePlayerPos));
    
    //wnd->subscribeEvent(CEGUI::Window::EventWindowUpdated ,CEGUI::Event::Subscriber(updatePlayerPos));
    return wnd;
}

