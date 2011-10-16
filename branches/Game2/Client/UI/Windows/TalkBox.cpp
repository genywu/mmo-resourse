#include "StdAfx.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Other/NpcTalkBox.h"
#include "../UIDef.h"
#include "../pub/AutoMoveParser.h"
#include "../pub/GoodsParser.h"
#include "../pub/LinkButtonParser.h"
#include "../../../Public/Common/MsgType.h"
#include "../../Net/Message.h"
#include "../../GameClient/ClientApp/Other/AudioList.h"
CEGUI::Window* InitTalkBox();
bool OnUpdateTalkBox(const CEGUI::EventArgs &args);
void handleAutoMove(CEGUI::RenderedString& rs, const CEGUI::String& value);
bool ResetTalkBox(const CEGUI::EventArgs &args);
bool OnTalkBtnClicked(const CEGUI::EventArgs &args);

void LoadTalkImageSetRes()
{
    //创建talkbox图像集
    CEGUI::String wsResGroup = "TalkBox";
    CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
        (CEGUI::System::getSingleton().getResourceProvider());
    rp->setResourceGroupDirectory(wsResGroup, "datafiles/imagesets/TalkBox");
    
    vector<NpcTalkBox::PicInfo> &picResList = GetGame()->GetNpcTalkBox()->GetPicResList();
    vector<NpcTalkBox::PicInfo>::iterator resIter = picResList.begin();
    char PicImageSetName[128] = "";
    for (;resIter != picResList.end();++resIter)
    {   
        sprintf_s<128>(PicImageSetName,"%d",resIter->lID);
        CEGUI::ImagesetManager::getSingleton().createFromImageFile(PicImageSetName, resIter->fileName,wsResGroup);
    }   
}

CEGUI::Window* InitTalkBox()
{
    CEGUI::Window* wnd = LoadUI("TalkBox");
    wnd->subscribeEvent("UpdataTalkBox",
        CEGUI::Event::Subscriber(OnUpdateTalkBox));
    wnd->subscribeEvent(CEGUI::Window::EventHidden,
        CEGUI::Event::Subscriber(ResetTalkBox));
    LoadTalkImageSetRes();
    CEGUI::DefaultWindow* textWnd = WDefaultWindow(GetWndMgr().getWindow("TalkBox/Text"));
    textWnd->setProperty("VertScrollbar","true");

    wnd->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
        CEGUI::Event::Subscriber(OnHide));
    CEGUI::Window *pCloseBtn = textWnd->getChild("TalkBox/Text/CloseBtn");
    pCloseBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    CEGUI::Window *pAceptBtn = textWnd->getChild("TalkBox/Text/AcceptBtn");
    pAceptBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    CEGUI::Window *pContinBtn = textWnd->getChild("TalkBox/Text/ContinueBtn");
    pContinBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    CEGUI::Window *pCompleBtn = textWnd->getChild("TalkBox/Text/CompelteBtn");
    pCompleBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    CEGUI::Window *pReturnBtn = textWnd->getChild("TalkBox/Text/ReturnBtn");
    pReturnBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    CEGUI::Window *pCancelBtn = textWnd->getChild("TalkBox/Text/CancelBtn");
    pCancelBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    CEGUI::Window *pByeBtn = textWnd->getChild("TalkBox/Text/ByeBtn");
    pByeBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    CEGUI::Window *pDenyBtn = textWnd->getChild("TalkBox/Text/DenyBtn");
    pDenyBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(OnTalkBtnClicked));
    return wnd;
}
bool OnUpdateTalkBox(const CEGUI::EventArgs &args)
{
    CEGUI::DefaultWindow* text = WDefaultWindow(GetWndMgr().getWindow("TalkBox/Text"));
    AutoMoveParser::RemoveMapInfoFromWindow(text);
    GoodsParser::RemoveGoodInfoFromWindow(text);
    LinkButtonParser::RemoveLinkBtnFromWindow(text);
    NpcTalkBox *pTalkBox = GetGame()->GetNpcTalkBox();
    const char * content = pTalkBox->GetTalkContext();
    text->setText(ToCEGUIString(content));
    CEGUI::String btnName = "TalkBox/Text/";
    for (int i = 0;i<2;i++)
    {
        btnName += CEGUI::String(pTalkBox->GetBtnInfo(i).BtnName.c_str());
        
        if (CEGUI::WindowManager::getSingleton().isWindowPresent(btnName))
        {
            CEGUI::Window *pBtn1 = CEGUI::WindowManager::getSingleton().getWindow(btnName);
            pBtn1->setVisible(pTalkBox->GetBtnInfo(i).isEnable);
            pBtn1->setEnabled(pTalkBox->GetBtnInfo(i).isEnable);
        }
        btnName = "TalkBox/Text/";
    }
    
    return true;
}
bool ResetTalkBox(const CEGUI::EventArgs &args)
{
    CEGUI::Window *pWnd = static_cast<const CEGUI::WindowEventArgs&>(args).window;
    CMessage msg(MSG_C2S_OTHER_SENDTALKBOXSELECT);
    msg.Add(GetGame()->GetNpcTalkBox()->GetScriptGUID());
    msg.Add((long)-1);
    msg.Add((char*)"");
    msg.Send();
    GetGame()->GetNpcTalkBox()->SetScriptID(NULL_GUID);
    CEGUI::DefaultWindow* textWnd = WDefaultWindow(GetWndMgr().getWindow("TalkBox/Text"));
    CEGUI::Window *pCloseBtn = textWnd->getChild("TalkBox/Text/CloseBtn");
    CEGUI::Window *pAceptBtn = textWnd->getChild("TalkBox/Text/AcceptBtn");
    CEGUI::Window *pContinBtn = textWnd->getChild("TalkBox/Text/ContinueBtn");
    CEGUI::Window *pCompleBtn = textWnd->getChild("TalkBox/Text/CompelteBtn");
    CEGUI::Window *pReturnBtn = textWnd->getChild("TalkBox/Text/ReturnBtn");
    CEGUI::Window *pCancelBtn = textWnd->getChild("TalkBox/Text/CancelBtn");
    CEGUI::Window *pByeBtn = textWnd->getChild("TalkBox/Text/ByeBtn");
    CEGUI::Window *pDenyBtn = textWnd->getChild("TalkBox/Text/DenyBtn");
    pCloseBtn->setVisible(false);
    pAceptBtn->setVisible(false);
    pContinBtn->setVisible(false);
    pCompleBtn->setVisible(false);
    pReturnBtn->setVisible(false);
    pCancelBtn->setVisible(false);
    pByeBtn->setVisible(false);
    pDenyBtn->setVisible(false);
    return true;
}

bool OnTalkBtnClicked(const CEGUI::EventArgs &args)
{
    CEGUI::Window *pWnd = static_cast<const CEGUI::WindowEventArgs&>(args).window;
    CEGUI::String wndName = pWnd->getName();
    size_t npos = CEGUI::String::npos;
    if (wndName.find("AcceptBtn") != npos ||
        wndName.find("ContinueBtn") != npos ||
        wndName.find("CompelteBtn") != npos ||
        wndName.find("ReturnBtn") != npos)
    {
        GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
        CMessage msg(MSG_C2S_OTHER_SENDTALKBOXSELECT);
        msg.Add(GetGame()->GetNpcTalkBox()->GetScriptGUID());
        msg.Add((long)1);				// 1 表示热区链接被点击
        msg.Add(GetGame()->GetNpcTalkBox()->GetGoodsSelectedIndex());	// Goods链接index
        msg.Add((char*)"0");
        msg.Send();
        GetGame()->GetNpcTalkBox()->SetScriptID(NULL_GUID);
    }
    else if (wndName.find("CloseBtn") != npos ||
             wndName.find("CancelBtn") != npos ||
             wndName.find("ByeBtn") != npos ||
             wndName.find("DenyBtn") != npos)
    {
        GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
        CMessage msg(MSG_C2S_OTHER_SENDTALKBOXSELECT);
        msg.Add(GetGame()->GetNpcTalkBox()->GetScriptGUID());
        msg.Add((long)1);				// 1 表示热区链接被点击
        msg.Add(GetGame()->GetNpcTalkBox()->GetGoodsSelectedIndex());	// Goods链接index
        msg.Add((char*)"1");
        msg.Send();
        GetGame()->GetNpcTalkBox()->SetScriptID(NULL_GUID);
    }
    return true;
}