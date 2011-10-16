#include "stdafx.h"
#include "LoginEvent.h"
#include "LoginPage.h" 
#include "../../UIDef.h"
/////////////////////////////////////////////////
// zhaohang  2010/7/21 
// 测试商城
#include "../../Windows/ShopCityDef.h"
////////////////////////////////////////////////

#include "../../UIManager.h"
//#include "../../CEGUI/cegui/include/CEGUIFreeTypeFont.h" 
#include "../../../GameClient/Game.h"
#include "../../../GameClient/ClientApp/SelectLS/SelectLSMgr.h"
#include "../../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"

bool HandleEnterSelectLS(const CEGUI::EventArgs& e)
{
	FireUIEvent("SelectLS",EVENT_OPEN);
	/////////////////////////////////////////////////
	// zhaohang  2010/7/21 
	// 商城页面UI显示测试,使得不用登录即可看界面
	//FireUIEvent(SHOPCITY_PAGE_NAME,EVENT_OPEN);
	////////////////////////////////////////////////
	return true;
}
LoginEvent::LoginEvent(EVENTTYPE type)
{
    SetEventType(type);
}

LoginEvent::~LoginEvent()
{
	
}
void LoginEvent::OnPageLoad(GamePage *pPage)
{
    pPage->LoadPageWindow();
    
    CEGUI::Window *pLoginWindow = pPage->GetPageWindow();
	/////////////////////////////////////////////////
	// zhaohang  2010/6/4 
	// 添加选服的修改
	//首次打开显示选择登陆服务器界面
    FireUIEvent("SelectLS",EVENT_OPEN);
    //设置密码编辑框掩码
    CEGUI::Editbox *pPwdEdit = static_cast<CEGUI::Editbox*>(pLoginWindow->getChild("LoginPage/PassWord"));
    pPwdEdit->setTextMasked(true);
    pPwdEdit->subscribeEvent(CEGUI::Editbox::EventTextAccepted ,CEGUI::Event::Subscriber(&LoginEvent::HandleLogin, this));
	
    //设置相应事件处理函数
    CEGUI::PushButton *pEnterBtn = static_cast<CEGUI::PushButton*>(pLoginWindow->getChild("LoginPage/Enter"));
    pEnterBtn->setText(ToCEGUIString(AppFrame::GetText("Login_1")));
	//pEnterBtn->setText(ToCEGUIString("[ROLL Speed:1 Dir:Left Text:nihao]"));
    pEnterBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&LoginEvent::HandleLogin, this));

    CEGUI::PushButton *pExitBtn = static_cast<CEGUI::PushButton*>(pLoginWindow->getChild("LoginPage/Exit"));
    pExitBtn->setText(ToCEGUIString(AppFrame::GetText("Login_2")));
    pExitBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&LoginEvent::HandleExitBtn, this));

    CEGUI::PushButton *pRegistBtn = static_cast<CEGUI::PushButton*>(pLoginWindow->getChild("LoginPage/Register"));
    pRegistBtn->setText(ToCEGUIString(AppFrame::GetText("Login_3")));
    
    CEGUI::PushButton *pForgetPwdBtn = static_cast<CEGUI::PushButton*>(pLoginWindow->getChild("LoginPage/ForgetPwd"));
    pForgetPwdBtn->setText(ToCEGUIString(AppFrame::GetText("Login_4")));

	CEGUI::PushButton* serverInfo = static_cast<CEGUI::PushButton*>(pLoginWindow->getChild("Login/ServerInfo"));
    serverInfo->setText(ToCEGUIString(AppFrame::GetText("Login_5")));
	serverInfo->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(HandleEnterSelectLS));

}
void LoginEvent::OnPageOpen(GamePage *pPage)
{
    CEGUI::Window *pLoginWindow = pPage->GetPageWindow();
    //设置账号编辑框并得到焦点
    CEGUI::Editbox *pIDEdit = static_cast<CEGUI::Editbox*>(pLoginWindow->getChild("LoginPage/Account"));
	/////////////////////////////////////////////////
	// zhaohang  2010/3/29 
	// 
	//读cdkey
	ifstream stream2;
	stream2.open("setup/cdkey.ini"); 
	if (stream2.is_open())
	{
		bool bRememberCdkey=false;
		stream2 >> bRememberCdkey;
		if (bRememberCdkey)
		{
			string str;
			stream2 >> str;
			pIDEdit->setText(str.c_str());

			//m_pRememberCdkey->SetSelected(true);
		}
		stream2.close();
	}
	else
		pIDEdit->setText(""); 
	/////////////////////////////////////////////////

    //pIDEdit->activate();

    CEGUI::Editbox *pPwdEdit = static_cast<CEGUI::Editbox*>(pLoginWindow->getChild("LoginPage/PassWord"));
    pPwdEdit->setText("");
	pPwdEdit->activate();
}

void LoginEvent::OnPageClose(GamePage *pPage)
{

}

void LoginEvent::OnPageUpdate(GamePage *pPage)
{

}

void LoginEvent::OnPageRelease(GamePage *pPage)
{

}

bool LoginEvent::HandleLogin(const CEGUI::EventArgs &e)
{
    CEGUI::Window* pLoginWindow = GetInst(LoginPage).GetPageWindow();
    CEGUI::Editbox *pIDEdit = static_cast<CEGUI::Editbox*>(pLoginWindow->getChild("LoginPage/Account"));
    CEGUI::Editbox *pPwdEdit = static_cast<CEGUI::Editbox*>(pLoginWindow->getChild("LoginPage/PassWord"));
    CEGUI::String strName = pIDEdit->getText();
    CEGUI::String strPass = pPwdEdit->getText();
    
    if (strName.empty())
    {
        //请填写完整信息
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_50"),NULL,NULL,true);
        return false;
    }
    if (strPass.empty())
    {
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_51"),NULL,NULL,true);
        return false;
    }
    DWORD dwCurTime = timeGetTime();
    if(dwCurTime - GetInst(LoginPage).GetLastLoginTime() < 2000)
    {
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_51"),NULL,NULL,true);
        pPwdEdit->setText("");
        return false;
    }
    extern DWORD g_LoginTime;
    extern BOOL g_bLogin;
    g_LoginTime = dwCurTime;
    g_bLogin = true;

	/////////////////////////////////////////////////
	// zhaohang  2010/3/29 
	// 
	//记录CDKey
	ofstream stream;
	stream.open("setup/cdkey.ini");
	if (stream.is_open())
	{
		bool bRemomberCdkey = true;
		stream << bRemomberCdkey << '\t'
			<< pIDEdit->getText().c_str();
	}
	else
	{
		stream.clear();
	}
	stream.close();
	/////////////////////////////////////////////////
    OnLoginLS();
    return true;
}

void LoginEvent::OnLoginLS()
{
    CEGUI::Window *LoginWindow = GetInst(LoginPage).GetPageWindow();

    CEGUI::Editbox *pIDEdit = static_cast<CEGUI::Editbox*>(LoginWindow->getChild("LoginPage/Account"));

    CEGUI::Editbox *pPwdEdit = static_cast<CEGUI::Editbox*>(LoginWindow->getChild("LoginPage/PassWord"));
    CEGUI::String strName = pIDEdit->getText();
   
    /*连接 ls*/
    unsigned char strMD5[16]="";
    TMD5 tempMD5;
    tempMD5.MessageDigest((unsigned char *)pPwdEdit->getText().c_str(),(unsigned int)pPwdEdit->getText().length(),strMD5);
    vector<BYTE> byteMD5;
    byteMD5.clear();
    for(int i = 0; i< 16; i++)
    {
        byteMD5.push_back(strMD5[i]);
    }   
    
	//ifstream file;
 //   file.open("ls.txt");
 //   string ip("");
 //   ushort port(0);
 //   file >> ip >> port;
 //   if( GetGame()->ConnectLoginServer(ip.c_str(),
 //       port, strName.c_str(), byteMD5) )

 //   {
 //       pPwdEdit->setText("");
 //       LoginWindow->releaseInput();
 //       GetInst(LoginPage).SetCDkey(strName.c_str());
 //   }
	LSHelp& lshelp = GetInst(LSHelp);
	tagLS* ls = lshelp.GetSelLS();
	if(!ls)
	{
		//MessageBox(g_hWnd,"make sure you have selected one server","Error",MB_OK);
        GetInst(MsgEventManager).PushEvent(Msg_Ok,"make sure you have selected one server");
		return;
	}
	if( GetGame()->ConnectLoginServer(ls->ip.c_str(),
		ls->port, strName.c_str(), byteMD5) )

	{
		pPwdEdit->setText("");
		LoginWindow->releaseInput();
		GetInst(LoginPage).SetCDkey(strName.c_str());
        GetGame()->SetGameState(GS_SELECT);
	}
    else	// 失败
    {
        //MessageBox(g_hWnd,CStringReading::LoadText(eIDS_PUBLIC,eIDS_LoginError),"Error",MB_OK);
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_25"));
        pPwdEdit->captureInput();
        g_LoginTime=0;
    }
}

bool LoginEvent::HandleExitBtn(const CEGUI::EventArgs &e)
{
    PostQuitMessage(0);
    return true;
}