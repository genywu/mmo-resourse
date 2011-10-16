#include "stdafx.h"
#include "AutoMoveParser.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/GameControl.h"
#include "../../GameClient/ClientApp/Player.h"

AutoMoveParser::mapInfo AutoMoveParser::autoMoveMap;
AutoMoveParser::AutoMoveParser()
{
    
}

AutoMoveParser::~AutoMoveParser()
{

}

CEGUI::String AutoMoveParser::Parse(const CEGUI::String& str)
{
    char strRegion[32] = "";
    char strPosX[32] = "";
    char strPosY[32] = "";
    char btnText[128] = "";
    char color[32] = "";
    char ParentWinName[128] = "";
    size_t parentWinPos,rgnPos,XPos,YPos,TitlePos,ColorPos,endPos;
    parentWinPos = rgnPos = XPos = YPos = TitlePos = ColorPos = endPos = CEGUI::String::npos;
    MoveInfo automove;
    
    parentWinPos = str.find("WIN:");
    rgnPos = str.find("RGN:");
    XPos   = str.find("POS_X:");
    YPos   = str.find("POS_Y:");
    TitlePos = str.find("TITLE:");
    ColorPos = str.find("COLOR:");
    CEGUI::String temp;

    CEGUI::String WndName = "Move_Btn_";
    static DWORD MovBtnID = 0;
    WndName += CEGUI::PropertyHelper::intToString(MovBtnID++);
    CEGUI::Window *pBtn = NULL;
    //如果无此window，就创建按钮类型的。
    if(CEGUI::WindowManager::getSingleton().isWindowPresent(WndName) == false)
    {
        pBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button",WndName);
        //解析父窗口
        if(parentWinPos != CEGUI::String::npos)
        {
            temp = str.substr(parentWinPos+5);
            endPos = temp.find("'");
            strcpy_s<128>(ParentWinName,temp.substr(0,endPos).c_str());
            CEGUI::Window *pParentWnd = CEGUI::WindowManager::getSingleton().getWindow(ParentWinName);
            pParentWnd->addChildWindow(pBtn);
            pBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&AutoMoveParser::OnAutoMoveBtnClicked,this));
            float fWidth = 0;
            float fHeight = pBtn->getFont()->getFontHeight();
            //解析文字描述
            if (TitlePos != CEGUI::String::npos)
            {
                temp = str.substr(TitlePos+7);
                endPos = temp.find("'");
                strcpy_s<128>(btnText,temp.substr(0,endPos).c_str());
                fWidth = pBtn->getFont()->getTextExtent(btnText);
                //解析文字颜色
                if (ColorPos != CEGUI::String::npos)
                {
                    temp = str.substr(ColorPos+6);
                    endPos = temp.find(" ");
                    strcpy_s<32>(color,temp.substr(0,endPos).c_str());
                    temp = "[COLOR ";
                    temp += color;
                    temp += "]";
                    temp += CEGUI::String(btnText);
                    pBtn->setText(temp);
                }
                else
                    pBtn->setText(btnText);
            }
            pBtn->setWidth(cegui_absdim(fWidth));
            pBtn->setHeight(cegui_absdim(fHeight));
        }
        //解析场景ID
        if (rgnPos != CEGUI::String::npos)
        {
            temp = str.substr(rgnPos+4);
            endPos = temp.find(" ");
            strcpy_s<32>(strRegion,temp.substr(0,endPos).c_str());
            automove.RegionID = CEGUI::PropertyHelper::stringToInt(strRegion);
        }
        //解析X坐标
        if (XPos != CEGUI::String::npos)
        {
            temp = str.substr(XPos+6);
            endPos = temp.find(" ");
            strcpy_s<32>(strPosX,temp.substr(0,endPos).c_str());
            automove.PosX = CEGUI::PropertyHelper::stringToInt(strPosX);
        }
        //解析Y坐标
        if (YPos != CEGUI::String::npos)
        {
            temp = str.substr(YPos+6);
            endPos = temp.find(" ");
            strcpy_s<32>(strPosY,temp.substr(0,endPos).c_str());
            automove.PosY = CEGUI::PropertyHelper::stringToInt(strPosY);
        }
        //追加到map里面
        autoMoveMap[pBtn] = automove;
    }
    return WndName;
}
bool AutoMoveParser::OnAutoMoveBtnClicked(const CEGUI::EventArgs &e)
{
    CEGUI::Window *pWnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    if (NULL == GetGame()->GetRegion())
    {
        return false;
    }
    mapInfo::iterator Iter = autoMoveMap.find(pWnd);
    long RegionID, PosX,PosY;
    if(Iter == autoMoveMap.end())
    {
        return false;
    }
    else
    {
        RegionID = Iter->second.RegionID;
        PosX = Iter->second.PosX;
        PosY = Iter->second.PosY;
    }
    CPlayer * mainPlayer = GetGame()->GetMainPlayer();
    long CurRegID = mainPlayer->GetRegionID();
    long CurPosX = mainPlayer->GetTileX();
    long CurPosY = mainPlayer->GetTileY();
    CGameControl *pGameCtl = GetGame()->GetGameControl();
    if (CurRegID != RegionID)
    { 
        pGameCtl->FindPathToOtherRegion(CurRegID,CurPosX,CurPosY,RegionID,PosX,PosY);
    } 
    else
    {
        pGameCtl->AutoMoveTo((float)PosX,(float)PosY);
    }
   
    return true;
}

void AutoMoveParser::RemoveMapInfoFromWindow(CEGUI::Window *Wnd)
{
    mapInfo::iterator iterInfo = autoMoveMap.begin();
    while(iterInfo != autoMoveMap.end())
    {
        CEGUI::Window *pChildWin = iterInfo->first;

        if (Wnd == pChildWin->getParent())
        {
            pChildWin->destroy();
            iterInfo = autoMoveMap.erase(iterInfo);
            continue;
        }
        ++iterInfo;
    }
    
}