#include "stdafx.h"
#include "../../GamePage.h"
#include "ChatEvent.h"
#include "../../../Net/Message.h"
#include "../../../../Public/Common/MsgType.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../../GameClient/ClientApp/Other/MsgSendTimeLimitMgr.h"
#include "../../../GameClient/Game.h"
#include "../../UIDef.h"
ChatEvent::ChatEvent(EVENTTYPE type)
{
    SetEventType(type);
}

void ChatEvent::OnPageLoad(GamePage *pPage)
{
    pPage->LoadPageWindow();
    CEGUI::Editbox *pEditBox = static_cast<CEGUI::Editbox *>(pPage->GetPageWindow()->getChild("ChatInput"));
    pEditBox->subscribeEvent(CEGUI::Window::EventKeyDown,CEGUI::Event::Subscriber(&ChatLogicEventManager::RollBackSendedText,&GetInst(ChatLogicEventManager)));
    pEditBox->subscribeEvent(CEGUI::Editbox::EventTextAccepted,CEGUI::Event::Subscriber(&ChatLogicEventManager::SendScript,&GetInst(ChatLogicEventManager)));
    
    /*pPageWindow->setWidth(cegui_absdim(width));
    pPageWindow->setHeight(cegui_absdim(height));*/
}

void ChatEvent::OnPageOpen(GamePage *pPage)
{
    CEGUI::DefaultWindow *pPageWindow = static_cast<CEGUI::DefaultWindow*>(pPage->GetPageWindow());
    const CEGUI::Image &backgroundImage = SetBackGroundImage(pPageWindow, "Chat", "datafiles/imagesets/chat", "interface-txt.tga");
    float width  = backgroundImage.getSourceTextureArea().getWidth();
    float height = backgroundImage.getSourceTextureArea().getHeight();
    
    //pPageWindow->setSize(CEGUI::UVector2(cegui_absdim(width),cegui_absdim(height)));
    CEGUI::Editbox *pEditBox = static_cast<CEGUI::Editbox *>(pPageWindow->getChild("ChatInput"));
    
    //pEditBox->setSize(CEGUI::UVector2(cegui_absdim(width-6),cegui_absdim(height-4)));*/
    pEditBox->setText("");
    pEditBox->activate();
}

void ChatEvent::OnPageClose(GamePage *pPage)
{
    //////////////////////////////////////////////////////////////////////////
    //zhaohang 2010-03-18
    //加入处理脚本调用
    GetInst(ChatLogicEventManager).SetRollPos(GetInst(ChatLogicEventManager).GetMsgQueSize());
    //////////////////////////////////////////////////////////////////////////
}

void ChatEvent::OnPageUpdate(GamePage *pPage)
{

}

void ChatEvent::OnPageRelease(GamePage *pPage)
{

}

//////////////////////////////////////////////////////////////////////////
//zhaohang 2010-03-18
//聊天逻辑处理管理类
ChatLogicEventManager::ChatLogicEventManager():RollPos(0)
{
}

ChatLogicEventManager::~ChatLogicEventManager()
{

}

bool ChatLogicEventManager::RollBackSendedText(const CEGUI::EventArgs &args)
{
    const CEGUI::KeyEventArgs keyEvent = static_cast<const CEGUI::KeyEventArgs&>(args);
    if (keyEvent.scancode != CEGUI::Key::ArrowUp &&
        keyEvent.scancode != CEGUI::Key::ArrowDown)
        return false;

    if (SendedMsgQue.empty())
        return false;
    switch (keyEvent.scancode)
    {
    case DIK_UP:RollPos--;break;
    case DIK_DOWN:RollPos++;break;
    }
    if(RollPos >= (int)SendedMsgQue.size())
        RollPos = (int)SendedMsgQue.size()-1;
    if(RollPos < 0)
        RollPos = 0;
    string RollText = SendedMsgQue[RollPos];
    CEGUI::Editbox *pChatEdit = static_cast<CEGUI::Editbox*>
        (CEGUI::WindowManager::getSingleton().getWindow("ChatInput"));
    pChatEdit->setText(RollText.c_str());
    pChatEdit->setSelection(0,RollText.length());
    return true;
}

bool ChatLogicEventManager::SendScript(const CEGUI::EventArgs &args)
{
    FireUIEvent("ChatPage", EVENT_CLOSE);
    CEGUI::String content = CEGUI::WindowManager::getSingleton().getWindow("ChatInput")->getTextVisual();
    if(!content.length())
        return false;
    string AnsiTxt = CEGUIStringToAnsiChar(content);

	MsgSendTimeLimitMgr& mstlm = GetInst(MsgSendTimeLimitMgr);
	if(mstlm.IsTimeOut(MST_TYPE_CHAT_SCRIPT) == false)
		return false;
    CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
    if(!pMainPlayer)
        return false;
    m_chat.ReSetChatContent(AnsiTxt.c_str());
    eChatType lType = m_chat.GetChatType();
    CMessage msg(MSG_C2S_OTHER_TALK);
    msg.Add((BYTE)0);
    msg.Add((long)lType);
    msg.Add(pMainPlayer->GetType());		// 发送者type
    msg.Add(pMainPlayer->GetExID());		// 发送者ID
    //==========================物品信息========================================
    msg.Add((long)0);//
    switch (lType)
    {
    case RUN_SCRIPT:        //运行脚本
        {
            msg.Add(m_chat.RemoveFlag(RUN_SCRIPT));//文本
        }
    	break;
    case CHAT_NORMAL:       //同屏聊天
        {
            msg.Add(AnsiTxt.c_str());
        }
        break;
    case CHAT_REGION:
        break;
    } 
    
    msg.Send();
	mstlm.UpdateLastSendTime(MST_TYPE_CHAT_SCRIPT);
    SendedMsgQue.push_back(AnsiTxt);
    if (SendedMsgQue.size() > 20)
    {
        SendedMsgQue.erase(SendedMsgQue.begin());
    }
    RollPos = (long)SendedMsgQue.size();
    
    return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//zhaohang 2010-03-18
//聊天文本内容 工具类
ChatInfo::ChatInfo()
{

}

ChatInfo::~ChatInfo()
{

}
void ChatInfo::ReSetChatContent(const char *content)
{ 
        m_Content = content;
}

eChatType ChatInfo::GetChatType()
{
    if( m_Content.size() == 0)
        return TYPE_NUM;
    long lPos = 0;
    eChatType lType = CChat::GetChatType( m_Content.c_str(), lPos );
    if ( lType == TYPE_NUM )
    {
        lType = CHAT_NORMAL;
    }
    return lType;
}

const char* ChatInfo::RemoveFlag(eChatType e)
{
    if(e == RUN_SCRIPT)
    {
       m_Content.erase(0,2);
       return m_Content.c_str();
    }
    else
        return NULL;
}


