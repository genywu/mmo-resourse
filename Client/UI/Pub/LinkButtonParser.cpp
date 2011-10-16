#include "stdafx.h"
#include "LinkButtonParser.h"
#include "../UIDef.h"
#include "../../Net/Message.h"
#include "../../../Public/Common/MsgType.h"
#include "../../../Public/Module/GUID.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Other/NpcTalkBox.h"
LinkButtonParser::mapLinkInfo LinkButtonParser::LinkMap;
LinkButtonParser::LinkButtonParser()
{
    
}

LinkButtonParser::~LinkButtonParser()
{

}

CEGUI::String LinkButtonParser::Parse(const CEGUI::String &str)
{
    std::string szStr = CEGUIStringToAnsiChar( str );

    size_t parentWinPos,IDPos,TextPos,ColorPos,endPos;
    parentWinPos = IDPos = TextPos = ColorPos = endPos = CEGUI::String::npos;
    char ParentWinName[128] = "";
    char LinkID[32] = "";
    char LinkText[128] = "";
    char ColorVal[32] = "";
    parentWinPos = str.find("WIN:");
    IDPos        = str.find("ID:");
    TextPos      = str.find("TEXT:");
    ColorPos     = str.find("COLOR:");
    std::string  wndName("LinkBtn_"),temp;
    static DWORD LinkWndCounter = 0;
    wndName += CEGUI::PropertyHelper::intToString(LinkWndCounter++).c_str();

    CEGUI::Window *linkWnd = 0;
    if (CEGUI::WindowManager::getSingleton().isWindowPresent(wndName) == false)
    {
        linkWnd = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button",wndName);
        linkWnd->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&LinkButtonParser::OnLinkBtnClicked,this));
        //解析父窗口
        if (parentWinPos != CEGUI::String::npos)
        {
            temp = szStr.substr(parentWinPos+5);
            endPos = temp.find("'");
            strcpy_s<128>(ParentWinName,temp.substr(0,endPos).c_str());
            CEGUI::Window *pParentWnd = CEGUI::WindowManager::getSingleton().getWindow(ParentWinName);
            pParentWnd->addChildWindow(linkWnd);
        }
        //解析ID
        if (IDPos != CEGUI::String::npos)
        {
            temp = szStr.substr(IDPos+3);
            endPos = temp.find(" ");
            strcpy_s<32>(LinkID,temp.substr(0,endPos).c_str());
            LinkMap[linkWnd] = LinkID;
        }
        //解析链接按钮文本
        if (TextPos !=  CEGUI::String::npos)
        {
            temp = szStr.substr(TextPos+6);
            endPos = temp.find("'");
            strcpy_s<128>(LinkText,temp.substr(0,endPos).c_str());
            float fWidth  = linkWnd->getFont()->getTextExtent(LinkText);
            float fheight = linkWnd->getFont()->getFontHeight();
            linkWnd->setSize(CEGUI::UVector2(cegui_absdim(fWidth),cegui_absdim(fheight)));
            //解析链接按钮文本的颜色
            if (ColorPos != CEGUI::String::npos)
            {
                temp = szStr.substr(ColorPos+6);
                endPos = temp.find(" ");
                strcpy_s(ColorVal,temp.substr(0,endPos).c_str());
                temp = "[COLOR ";
                temp += ColorVal;
                temp += "]";
                temp += CEGUI::String(LinkText).c_str();
                linkWnd->setText(ToCEGUIString(temp.c_str()));
            }
            else
                linkWnd->setText(ToCEGUIString(LinkText));
        }
    }
    return wndName;
}

void LinkButtonParser::RemoveLinkBtnFromWindow(CEGUI::Window *Wnd)
{
    mapLinkInfo::iterator iterInfo = LinkMap.begin();
    while(iterInfo != LinkMap.end())
    {
        CEGUI::Window *pChildWin = iterInfo->first;

        if (Wnd == pChildWin->getParent())
        {
            pChildWin->destroy();
            iterInfo = LinkMap.erase(iterInfo);
            continue;
        }
        ++iterInfo;
    }
}

bool LinkButtonParser::OnLinkBtnClicked(const CEGUI::EventArgs &e)
{
    CEGUI::Window *pWnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    const char* strID = LinkMap[pWnd].c_str();
    CMessage msg(MSG_C2S_OTHER_SENDTALKBOXSELECT);
    msg.Add(GetGame()->GetNpcTalkBox()->GetScriptGUID());
    msg.Add((long)1);				// 1 表示热区链接被点击
    msg.Add((BYTE)0);
    msg.Add((char*)strID);
    msg.Send();
    GetGame()->GetNpcTalkBox()->SetScriptID(NULL_GUID);
    return true;
}

