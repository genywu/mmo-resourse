#include "StdAfx.h"         
#include "MsgBox.h"
#include "../UIDef.h"   
#include "../CEGUI/cegui/include/CEGUIWindowManager.h"
#include "../CEGUI/cegui/include/CEGUIWindow.h"   
#include "../../GameClient/Game.h"
#include "../../ui/UIManager.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"

bool CMsgBox::OnOpen(const CEGUI::EventArgs& e)
{
     CEGUI::Window*  Lab = GetWndMgr().getWindow("MessageBox/LabText");
     CEGUI::Window*  WinBox = GetWndMgr().getWindow("MessageBox");
     const char* str = event.pContext ?event.pContext:"未知窗口内容";
     Lab->setText( ToCEGUIString(str) );
 
     CEGUI::Window*  OkBt= GetWndMgr().getWindow("MessageBox/OkButton");
     CEGUI::Window*  MkBt= GetWndMgr().getWindow("MessageBox/MkButton");
     CEGUI::Window*  CanBt= GetWndMgr().getWindow("MessageBox/CancelButton");
 
     if ( event.bMode )
           WinBox->setModalState(true);

     switch( event.pEventDef.BoxType )
     {
     case Msg_Ok:
         {
             MkBt->setText(ToCEGUIString("确定"));
             MkBt->setVisible(true);
             OkBt->setVisible(false);
             CanBt->setVisible(false);
         }
         break;
     case Msg_YesNo:
         {
             OkBt->setText(ToCEGUIString("是"));
             CanBt->setText(ToCEGUIString("否"));
             MkBt->setVisible(false);
             OkBt->setVisible(true);
             CanBt->setVisible(true);
         }
         break;
     case Msg_GoOn:
         {
             OkBt->setText(ToCEGUIString("继续"));
             CanBt->setText(ToCEGUIString("退出"));
             MkBt->setVisible(false);
             OkBt->setVisible(true);
             CanBt->setVisible(true);
         }
         break;
     case Msg_Agree:
         {
             OkBt->setText(ToCEGUIString("同意"));
             CanBt->setText(ToCEGUIString("不同意"));
             MkBt->setVisible(false);
             OkBt->setVisible(true);
             CanBt->setVisible(true);
         }
         break;

     case Msg_Ok_Yes_No:
         {
             OkBt->setText(ToCEGUIString("确定"));
             MkBt->setText(ToCEGUIString("拒绝"));
             CanBt->setText(ToCEGUIString("取消"));
             MkBt->setVisible(true);
             OkBt->setVisible(true);
             CanBt->setVisible(true);
         }
         break;
     default:
         break;
     }

    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
     wnd->setVisible(true);
     wnd->setEnabled(true);
     wnd->setAlwaysOnTop(true);

    return true;
}

bool CMsgBox::OnClose(const CEGUI::EventArgs& e)
{
    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    if( !wnd ) return false;

    wnd->setModalState(false);
    wnd->setVisible(false);

    if(event.pEventDef.FunTable)
        event.pEventDef.FunTable(event,S_Esc);

    Reset();
    GetInst(MsgEventManager).GoProcessEvent();
    return true;
}

bool CMsgBox::OnClickOk(const CEGUI::EventArgs& e)
{
    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    if( !wnd ) return false;

    wnd->getParent()->setModalState(false);
    wnd->getParent()->setVisible(false);

    if(event.pEventDef.FunTable)
        event.pEventDef.FunTable(event,S_Ok);

    Reset();
    GetInst(MsgEventManager).GoProcessEvent();

    return true;
}

bool  CMsgBox::OnClickMk(const CEGUI::EventArgs& e)
{
    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    if( !wnd ) return false;

    wnd->getParent()->setModalState(false);
    wnd->getParent()->setVisible(false);

    if(event.pEventDef.FunTable)
        event.pEventDef.FunTable(event,S_Mk);

    Reset();
    GetInst(MsgEventManager).GoProcessEvent();

    return true;
}

bool CMsgBox::OnClickCancel(const CEGUI::EventArgs& e)
{
    CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
    if( !wnd ) return false;

    wnd->getParent()->setModalState(false);
    wnd->getParent()->setVisible(false);

    if(event.pEventDef.FunTable)
        event.pEventDef.FunTable(event,S_Cancel);

    Reset();
    GetInst(MsgEventManager).GoProcessEvent();

    return true;
}

CEGUI::Window*  InitMsgBoxPage()
{
    CEGUI::Window*  root = GetWndMgr().getWindow("Root");

    CEGUI::Window*  WinBox = GetWndMgr().createWindow("TaharezLook/StaticImage","MessageBox");//"GUISheet", 

    WinBox->setPosition( CEGUI::UVector2(cegui_reldim(0.4f),cegui_reldim(0.15f)) );
    WinBox->setSize(CEGUI::UVector2(cegui_absdim(280),cegui_absdim(160)));

    CEGUI::Window* Lab = GetWndMgr().createWindow("TaharezLook/StaticText","MessageBox/LabText");
    Lab->setProperty("FrameEnabled","true");
    Lab->setProperty("BackgroundEnabled","true");
    Lab->setProperty("HorzFormatting","WordWrapLeftAligned");
    Lab->setPosition(CEGUI::UVector2(cegui_reldim(0.1f),cegui_reldim(0.1f)));
    Lab->setSize(CEGUI::UVector2(cegui_absdim(210),cegui_absdim(60)));

    CEGUI::PushButton* OkButton = static_cast<CEGUI::PushButton*>( GetWndMgr().createWindow("TaharezLook/Button","MessageBox/OkButton") );
    OkButton->setText(" O K ");
    OkButton->setPosition(CEGUI::UVector2(cegui_reldim(0.12f),cegui_reldim(0.62f)));
    OkButton->setSize(CEGUI::UVector2(cegui_absdim(60),cegui_absdim(40)));

    CEGUI::PushButton* MkButton = static_cast<CEGUI::PushButton*>( GetWndMgr().createWindow("TaharezLook/Button","MessageBox/MkButton") );
    MkButton->setText(" Make ");
    MkButton->setPosition(CEGUI::UVector2(cegui_reldim(0.36f),cegui_reldim(0.62f)));
    MkButton->setSize(CEGUI::UVector2(cegui_absdim(60),cegui_absdim(40)));


    CEGUI::PushButton* CancelBt = static_cast<CEGUI::PushButton*>(GetWndMgr().createWindow("TaharezLook/Button","MessageBox/CancelButton"));
    CancelBt->setText(" Cancel ");
    CancelBt->setPosition(CEGUI::UVector2(cegui_reldim(0.62f),cegui_reldim(0.62f)));
    CancelBt->setSize(CEGUI::UVector2(cegui_absdim(60),cegui_absdim(40)));

    WinBox->subscribeEvent(EVENT_OPEN,CEGUI::Event::Subscriber(&CMsgBox::OnOpen,&GetInst(CMsgBox)));
    WinBox->subscribeEvent(EVENT_CLOSE,CEGUI::Event::Subscriber(&CMsgBox::OnClose,&GetInst(CMsgBox)));
    
    OkButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&CMsgBox::OnClickOk,&GetInst(CMsgBox)));
    MkButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&CMsgBox::OnClickMk,&GetInst(CMsgBox)));
    CancelBt->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&CMsgBox::OnClickCancel,&GetInst(CMsgBox)));

    WinBox->addChildWindow(Lab);
    WinBox->addChildWindow(OkButton);
    WinBox->addChildWindow(MkButton);
    WinBox->addChildWindow(CancelBt);
    WinBox->setVisible(false);
    WinBox->setAlwaysOnTop(true);

    root->addChildWindow(WinBox);

    return WinBox;
}
