/**************************************************
*		文件名  ：Mail.h
*		简要描述：邮件界面相关

*		当前版本：1.0
*		作者    ：石喜
*		完成日期：10/10/18
*		修订说明：
****************************************************/
#include "StdAfx.h"
#include "../UIDef.h"

#include <iostream>
using namespace std;
#include "../../GameClient/Game.h"

#include "../../GameClient/ClientApp/Mail/MailDef.h"
#include "../../Net/Message.h"
#include "../../../Public/Common/MsgType.h"
#include "../../../Public/Module/GUID.h"
#include "../../../Public/Common/BaseDef.h"
#include "../../GameClient/ClientApp/Mail/Mail.h"
#include "../../GameClient/ClientApp/Goods/Goods.h"
#include "../../GameClient/ClientApp/MsgBoxSys/MsgEventManager.h"
using namespace mail;

CEGUI::Window* InitPersonalMail();
bool OnSendMailButtonDown(const CEGUI::EventArgs& args); 
bool OnReadMail(const CEGUI::EventArgs& args);				 //点中邮件时或者点读邮件按钮时调用
bool OnRejectMailButtonDown(const CEGUI::EventArgs& args);			 //退信
bool OnRecvBoxButtonDown(const CEGUI::EventArgs& args); 
bool handle_PageClose(const CEGUI::EventArgs& args); 
bool OnDelMailButtonDown( const CEGUI::EventArgs &args);
bool OnUpdateRecvBox( const CEGUI::EventArgs &args);		//显示邮件内容
bool OnUpdateReadBox( const CEGUI::EventArgs &args);		//显示邮件内容
bool OnRBClickGetGoods0(const CEGUI::EventArgs& args);		//接收邮件时取物品
bool OnRBClickGetGoods1(const CEGUI::EventArgs& args); 
bool OnRBClickGetGoods2(const CEGUI::EventArgs& args); 
bool OnRBClickGetGoods3(const CEGUI::EventArgs& args); 
bool OnRBClickRemoveGoods0(const CEGUI::EventArgs& args);	//发送邮件时移除已添加的物品
bool OnRBClickRemoveGoods1(const CEGUI::EventArgs& args); 
bool OnRBClickRemoveGoods2(const CEGUI::EventArgs& args); 
bool OnRBClickRemoveGoods3(const CEGUI::EventArgs& args); 
bool OnAddGoods(const CEGUI::EventArgs& args);				//发送邮件时,添加物品
bool OnRemoveGoods(const CEGUI::EventArgs& args);			//获取物品时,删除物品
bool OnPayMailSelChanged(const CEGUI::EventArgs& args);		//取信方式选择
bool OnGetGoldButtonDown(const CEGUI::EventArgs& args);
bool OnAgreePay(const CEGUI::EventArgs& args);
bool OnUpdateGold(const CEGUI::EventArgs& args);
bool OnUpdateSendData(const CEGUI::EventArgs& args);
bool OnInputGold(const CEGUI::EventArgs& args);
bool OnGetGold(const CEGUI::EventArgs& args);


//////////////////////////////////////////////////////////////////////////
int  GetMailIndex();
bool ShowGoods( const unsigned int nMailIndex ); //点中列表中的邮件,显示物品
void SetStaticText( bool bPayMailSel );
const int GetListIndex(const unsigned int listIndex);
void RemoveAllRecvData();
void RemoveAllSendData();
CEGUI::Window * GetRecvGoodsWindow(const unsigned int index);
CEGUI::Window * GetSendGoodsWindow(const unsigned int index);
//////////////////////////////////////////////////////////////////////////
//弹出对话框处理函数
void  PayMailMsgBox(MsgEvent& event,eMsgResult UserId);
void  PresentMsgBox(MsgEvent& event,eMsgResult UserId);
void  DelMail(MsgEvent& event,eMsgResult UserId);
void  PageLoad();
void  ResetSelBox();
bool  IsDelSel(int sel);
bool  IsPaySel();
const string GetImageSetName();

string g_strAddGoods[4] = {ADDGOODS0,ADDGOODS1,ADDGOODS2,ADDGOODS3};
string g_strGetGoods[4] = {GETGOODS0,GETGOODS1,GETGOODS2,GETGOODS3};

CEGUI::Window* InitPersonalMail()
{
	CEGUI::Window* btn;
    CEGUI::Window* wnd = LoadUI(PERSONAL_MAIL);	
	wnd->setVisible(false);
	wnd->subscribeEvent( CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(handle_PageClose));
	
	btn = wnd->getChild(SEND);
	btn->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(OnSendMailButtonDown));

	btn = wnd->getChild(READ);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(OnReadMail));

	btn = wnd->getChild(REJECT);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(OnRejectMailButtonDown));

	btn = wnd->getChild(RECVBOX);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(OnRecvBoxButtonDown));

	btn = wnd->getChild(DELMAIL);
	btn->subscribeEvent( CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(OnDelMailButtonDown));

	btn = wnd->getChild(PAYMAILSEL);
	btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::SubscriberSlot(OnPayMailSelChanged));

	btn = wnd->getChild(MONEY);
	btn->subscribeEvent(CEGUI::Window::EventCharacterKey, CEGUI::SubscriberSlot(OnInputGold));


	btn = wnd->getChild(GOLD);
	btn->subscribeEvent(CEGUI::Window::EventCharacterKey, CEGUI::SubscriberSlot(OnGetGold));

	btn = wnd->getChild(MAILLIST);
	btn->subscribeEvent(EVENT_UPDATA,CEGUI::SubscriberSlot(OnUpdateRecvBox));

	btn = wnd->getChild(READMAIL);
	btn->subscribeEvent(EVENT_UPDATA,CEGUI::SubscriberSlot(OnUpdateReadBox));

	btn = wnd->getChild(GOLD);
	btn->subscribeEvent(EVENT_UPDATA,CEGUI::SubscriberSlot(OnUpdateGold));
	
	btn = wnd->getChild(ADDGOODSCONTAINER);//要更新整个界面的数据怎么改
	btn->subscribeEvent(EVENT_UPDATA,CEGUI::SubscriberSlot(OnUpdateSendData));

	//GetWindow(PERSONAL_MAIL)->subscribeEvent(EVENT_UPDATA,CEGUI::SubscriberSlot(OnUpdateSendData));
	btn = CEGUI::WindowManager::getSingleton().getWindow(GETGOODS0);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickGetGoods0));

	btn = CEGUI::WindowManager::getSingleton().getWindow(GETGOODS1);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickGetGoods1));

	btn = CEGUI::WindowManager::getSingleton().getWindow(GETGOODS2);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickGetGoods2));

	btn = CEGUI::WindowManager::getSingleton().getWindow(GETGOODS3);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickGetGoods3));

	btn = CEGUI::WindowManager::getSingleton().getWindow(ADDGOODS0);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickRemoveGoods0));

	btn = CEGUI::WindowManager::getSingleton().getWindow(ADDGOODS1);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickRemoveGoods1));

	btn = CEGUI::WindowManager::getSingleton().getWindow(ADDGOODS2);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickRemoveGoods2));

	btn = CEGUI::WindowManager::getSingleton().getWindow(ADDGOODS3);
	btn->subscribeEvent(CEGUI::Window::EventMouseButtonDown ,CEGUI::SubscriberSlot(OnRBClickRemoveGoods3));

	CEGUI::Listbox *pMailList = static_cast<CEGUI::Listbox*>(wnd->getChild(MAILLIST));
	pMailList->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,CEGUI::SubscriberSlot(OnReadMail));


	btn = wnd->getChild(READ);
	btn->subscribeEvent(EVENT_UPDATA, CEGUI::SubscriberSlot(OnReadMail));

	btn = wnd->getChild(GETGOODSCONTAINER);
	btn->subscribeEvent(EVENT_ADDGOODS,CEGUI::SubscriberSlot(OnAddGoods));

	btn = wnd->getChild(GETGOODSCONTAINER);
	btn->subscribeEvent(EVENT_DELGOODS,CEGUI::SubscriberSlot(OnRemoveGoods));

	btn = wnd->getChild(GETGOLD);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(OnGetGoldButtonDown));

	btn = wnd->getChild(AGREEPAY);
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(OnAgreePay));

	PageLoad();

    return wnd;
}

void PageLoad()
{
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_SEND)->setText(ToCEGUIString(AppFrame::GetText("Mail_101")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_RECVBOX)->setText(ToCEGUIString(AppFrame::GetText("Mail_102")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_READ)->setText(ToCEGUIString(AppFrame::GetText("Mail_103")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_DEL)->setText(ToCEGUIString(AppFrame::GetText("Mail_104")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_REJECT)->setText(ToCEGUIString(AppFrame::GetText("Mail_105")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_AGREEPAY)->setText(ToCEGUIString(AppFrame::GetText("Mail_106")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_RECVR)->setText(ToCEGUIString(AppFrame::GetText("Mail_107")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_TITLE)->setText(ToCEGUIString(AppFrame::GetText("Mail_108")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_SENDCONTENT)->setText(ToCEGUIString(AppFrame::GetText("Mail_109")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_GOODSSEND)->setText(ToCEGUIString(AppFrame::GetText("Mail_110")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_GOODSRECV)->setText(ToCEGUIString(AppFrame::GetText("Mail_111")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_GOLD)->setText(ToCEGUIString(AppFrame::GetText("Mail_112")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_GETGOLD)->setText(ToCEGUIString(AppFrame::GetText("Mail_113")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_MONEY)->setText(ToCEGUIString(AppFrame::GetText("Mail_114")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_PAYMAILSEL)->setText(ToCEGUIString(AppFrame::GetText("Mail_116")));
	CEGUI::WindowManager::getSingleton().getWindow(STATIC_REVCCONTENT)->setText(ToCEGUIString(AppFrame::GetText("Mail_117")));
}

bool OnInputGold(const CEGUI::EventArgs& args)
{
	const CEGUI::KeyEventArgs &e_key =  static_cast<const CEGUI::KeyEventArgs&>(args);
 	if ( e_key.codepoint >= '0' && e_key.codepoint <= '9')
	{
		CEGUI::Editbox* eGold = WEditBox(GetWndMgr().getWindow(MONEY));
		CEGUI::String strNum = eGold->getText();
		char keycode = e_key.codepoint;
		strNum += keycode;
		eGold->setText(strNum.c_str());
		eGold->setCaratIndex(strNum.length());
	}

	return true;
}

bool OnGetGold(const CEGUI::EventArgs& args)
{
	return true;
}

bool OnUpdateGold(const CEGUI::EventArgs& args)
{

	CEGUI::Editbox* eGold = WEditBox(GetWndMgr().getWindow(GOLD));
	eGold->setText("0");
	
	return true;
}

bool OnAgreePay(const CEGUI::EventArgs& args)
{
	int mailIndex = GetMailIndex();
	GetInst(CMail).DoAgreePay(mailIndex);

	return true;
}
//发送邮件时,添加物品到物品列表
bool OnAddGoods(const CEGUI::EventArgs& args)
{
	//这个地方要修改
	list<CPlayer::tagGoods> listGoods = GetInst(CMail).GetGoodsList();
	size_t size = listGoods.size();
	list<CPlayer::tagGoods>::iterator it = listGoods.begin();
	for ( int i = 0;it != listGoods.end();it++,i++ )
	{		
		CGoods *pGoods = it->pGoods;
		if ( i == size - 1 && pGoods->GetProperty())//取listGoods中的最后一件物品
		{
			DWORD dwIconId = pGoods->GetProperty()->dwIconId;		
			char temp[32];
			sprintf( temp, "%d.jpg",dwIconId );
			string strName = string( (char*)temp, 32 );

			CEGUI::String img_property = "";
			//CEGUI::Window *pWnd = NULL;

			for (int i = 0; i < 4; i++)
			{
				const CEGUI::Window *pWnd = GetSendGoodsWindow(i);
				if (!pWnd)
				{
					return false;
				}
				img_property = pWnd->getProperty("Image");
				if (strlen(img_property.c_str()) == 0)
				{
					CEGUI::GUISheet *si = static_cast<CEGUI::GUISheet*>( CEGUI::WindowManager::getSingleton().getWindow(g_strAddGoods[i]) );
					const CEGUI::Image & backgroundImage = SetBackGroundImage(si, GetImageSetName().c_str(), "pictures/goods", strName.c_str());	

					return true;
				}
			}
		}
	}

	return true;
}

const string GetImageSetName()
{
	static int id = 0;
	char temp[32];
	sprintf( temp, "goods%d",id++ );
	string strName = string( (char*)temp, 32 );
	return strName;

}
//获取物品时,移除物品
bool OnRemoveGoods(const CEGUI::EventArgs& args)
{
	int goodsIndex = GetInst(CMail).GetGoodsIndex();

	 CEGUI::Window *pWnd = GetRecvGoodsWindow(goodsIndex);
	if (!pWnd)
	{
		return false;
	}
	CEGUI::String img_property = pWnd->getProperty("Image");
	if (strlen(img_property.c_str()) != 0 )
	{	
		pWnd->setProperty("Image",""); 
	}

	return true;
}
bool ShowGoods( const unsigned int nMailIndex )//点中列表中的邮件,显示物品
{
	RemoveAllRecvData();
	vector<stMailData> vMailData;
	vMailData = GetInst(CMail).GetMailData();
	if ( nMailIndex >= 0 && vMailData.size() >= nMailIndex + 1)
	{
		if (vMailData[nMailIndex].eMailType != PAYGOLD_MAIL && vMailData[nMailIndex].eMailType != SENDGOLD_MAIL)
		{
			DWORD GoldNum = vMailData[nMailIndex].dwGoldNum;//得到寄送的金币
			if(GoldNum > 0)
			{
				CEGUI::Editbox* eBox = WEditBox(GetWndMgr().getWindow(GOLD));
				char cGold[16];
				sprintf( cGold,"%d",GoldNum);
				CEGUI::String strCold = cGold;
				eBox->setText(strCold);	
			}
		}
		
		if ( vMailData[nMailIndex].GoodsList.size() > 0 )
		{
			list<CGoods*> pGoodsList = vMailData[nMailIndex].GoodsList;
			list<CGoods *>::iterator it = pGoodsList.begin();
			for ( int i = 0;it != pGoodsList.end();it++,i++ )
			{			
				if( (*it)->GetProperty() )
				{
					DWORD dwIconId = (*it)->GetProperty()->dwIconId;		
					char temp[32];
					string strName;
					sprintf( temp, "%d.jpg",dwIconId );
					strName = string( (char*)temp, 32 );

					CEGUI::GUISheet *si = static_cast<CEGUI::GUISheet*>( CEGUI::WindowManager::getSingleton().getWindow(g_strGetGoods[i]) );
					const CEGUI::Image & backgroundImage = SetBackGroundImage(si, GetImageSetName().c_str(), "pictures/goods", strName.c_str() );
				}	
			}
		}
	}
	
	return true;
}

bool OnUpdateReadBox(const CEGUI::EventArgs &args)//显示邮件内容
{
	stMailData mailData;
	if(GetInst(CMail).GetCurReadMailData( mailData ))
	{
		CEGUI::Editbox* ReadMail = WEditBox(GetWndMgr().getWindow(READMAIL));
		ReadMail->setText(ToCEGUIString(mailData.strMailDesc.c_str()));
	}
	return true;
}

bool OnUpdateRecvBox(const CEGUI::EventArgs &args)
{
	//清除先前的邮件数据
	RemoveAllRecvData();
	//清除邮件列表
	CEGUI::Listbox* pMailList = WListBox(GetWndMgr().getWindow(MAILLIST));
	pMailList->resetList();
		
	/*int itemCount = (int)pMailList->getItemCount();
	if ( itemCount > 0 )
	{
		for ( int i = itemCount-1; i >= 0; i-- )
		{
			CEGUI::ListboxItem *pItem  =   pMailList->getListboxItemFromIndex(i);
			pMailList ->removeItem(pItem);
		}
	}*/
	//添加现在的邮件数据
	vector<stMailData> vecMailList = GetInst(CMail).GetMailData();
	if ( !vecMailList.size())
	{
		return false;
	}
	const CEGUI::Image* cov_img = &CEGUI::ImagesetManager::getSingleton().get(IMAGES_FILE_NAME).getImage(CHECKBOXMARK);
	for ( unsigned int i = 0; i < vecMailList.size(); i++ )
	{
		CEGUI::String ItemText = ToCEGUIString(vecMailList[i].strMailName.c_str());
		//创建一个列表中的ITEM
		CEGUI::ListboxTextItem *pItem = new CEGUI::ListboxTextItem(ItemText,i);
		//设置选中时的背景条
		pItem->setSelectionBrushImage(IMAGES_FILE_NAME,MULTILISTSEL_BRUSH_NAME); 
		//添加到列表中
		pMailList->addItem(pItem);
	}
	
	return true;
}

bool OnDelMailButtonDown(const CEGUI::EventArgs &args)
{	
	CEGUI::Listbox* pMailList = WListBox(GetWndMgr().getWindow(MAILLIST));
	for ( int id = 0; id < 6; id++ )
	{
		if (IsDelSel(id))
		{
			GetInst(CMail).AddMailToDel(id);
		}
	}

	if (GetInst(CMail).GetDelMail().size() == 0)
	{
		//请选择要删除的信件！
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_36"));
		return false;
	}
	
	//是否确定删除所选信件？
	GetInst(MsgEventManager).PushEvent(Msg_YesNo,AppFrame::GetText("Mail_17"),DelMail);

	return true;
}

void  DelMail(MsgEvent& event,eMsgResult UserId)
{
	if (UserId == S_Ok)
	{
		GetInst(CMail).DoDelMail();
	}
	else
	{
		GetInst(CMail).GetDelMail().clear();
	}
}

bool OnRBClickGetGoods0(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		int mailIndex = GetMailIndex();
		if (mailIndex < 0 )
		{
			return false;
		}
		unsigned int goodsIndex = 0;
		GetInst(CMail).DoGetGoods( mailIndex, goodsIndex );
		GetInst(CMail).SetGoodsIndex( 0 );
	}
	
	return true;
}
bool OnRBClickGetGoods1(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		int mailIndex = GetMailIndex();
		if (mailIndex < 0 )
		{
			return false;
		}
		unsigned int goodsIndex = GetListIndex(1);
		GetInst(CMail).DoGetGoods( mailIndex, goodsIndex );
	}
	GetInst(CMail).SetGoodsIndex( 1 );
	
	return true;
}
bool OnRBClickGetGoods2(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		int mailIndex = GetMailIndex();
		if (mailIndex < 0 )
		{
			return false;
		}
		unsigned int goodsIndex = GetListIndex(2);
		GetInst(CMail).DoGetGoods( mailIndex, goodsIndex );
	}	
	GetInst(CMail).SetGoodsIndex( 2 );
	return true;
}
bool OnRBClickGetGoods3(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		int mailIndex = GetMailIndex();
		if (mailIndex < 0 )
		{
			return false;
		}
		unsigned int goodsIndex = GetListIndex(3);
		GetInst(CMail).DoGetGoods( mailIndex, goodsIndex );
	}
	GetInst(CMail).SetGoodsIndex( 3 );

	return true;
}

bool OnRBClickRemoveGoods0(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		int goodsIndex = 0;
		CEGUI::Window *pWnd = GetSendGoodsWindow(0);
		if (!pWnd)
		{
			return false;
		}
		CEGUI::String img_property = pWnd->getProperty("Image");
		if (img_property.c_str() != "" )
		{
			pWnd->setProperty("Image",""); 
			GetInst(CMail).DoRemoveGoods(goodsIndex);
		}
	}
		
	return true;
}
bool OnRBClickRemoveGoods1(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		CEGUI::Window *pWnd = GetSendGoodsWindow(1);
		if (!pWnd)
		{
			return false;
		}
		CEGUI::String img_property = pWnd->getProperty("Image");
		if (img_property.c_str() != "" )
		{
			pWnd->setProperty("Image",""); 
			int goodsIndex = GetListIndex(1);
			GetInst(CMail).DoRemoveGoods(goodsIndex);
		}
	}
	
	return true;
}
bool OnRBClickRemoveGoods2(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		CEGUI::Window *pWnd = GetSendGoodsWindow(2);
		if (!pWnd)
		{
			return false;
		}
		CEGUI::String img_property = pWnd->getProperty("Image");
		if (img_property.c_str() != "" )
		{
			pWnd->setProperty("Image",""); 
			int goodsIndex = GetListIndex(2);
			GetInst(CMail).DoRemoveGoods(goodsIndex);
		}
	}
	
	return true;
}
bool OnRBClickRemoveGoods3(const CEGUI::EventArgs& args)
{
	const CEGUI::MouseEventArgs &e_mouse = static_cast<const CEGUI::MouseEventArgs&>(args);
	if(e_mouse.button == CEGUI::RightButton)
	{
		CEGUI::Window *pWnd = GetSendGoodsWindow(3);
		if (!pWnd)
		{
			return false;
		}
		CEGUI::String img_property = pWnd->getProperty("Image");
		if (img_property.c_str() != "" )
		{
			pWnd->setProperty("Image",""); 
			int goodsIndex = GetListIndex(3);
			GetInst(CMail).DoRemoveGoods(goodsIndex);
		}
	}
	
	return true;
}

bool handle_PageClose(const CEGUI::EventArgs& args)
{
	CEGUI::Window* wnd = CEGUI::WindowManager::getSingleton().getWindow(PERSONAL_MAIL);
	if(wnd)
	{
		wnd->setVisible(false);
		wnd->setAlwaysOnTop(false);
	}
	else
		return false;
	return true;
}

bool OnRecvBoxButtonDown(const CEGUI::EventArgs& args)
{

	GetInst(CMail).DoRecvMail();

	return true;
}

bool OnReadMail(const CEGUI::EventArgs& args)
{
	//添加现在的邮件数据
	vector<stMailData> vecMailList = GetInst(CMail).GetMailData();
	if ( !vecMailList.size())
	{
		return false;
	}
	int mailIndex = GetMailIndex();

	if ( mailIndex < 0 )
	{
		//请选择一封邮件
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_34"));
		return false;
	}
	if (vecMailList[mailIndex].eMailType != PAYGOLD_MAIL)//付费邮件只有同意付费后才显示内容
	{
		ShowGoods( mailIndex );
		GetInst(CMail).DoReadMail(mailIndex);
	}
	else
	{
		CGUID guid = vecMailList[mailIndex].gMailID;
		GetInst(CMail).SetCurReadMailGUID(guid);
		RemoveAllRecvData();
	}

	return true;
}

bool OnSendMailButtonDown(const CEGUI::EventArgs& args)
{
	CEGUI::Editbox* eRecver = WEditBox(GetWndMgr().getWindow(RECVER));//收信人
	const char* strRecver = CEGUIStringToAnsiChar(eRecver->getText());	
	if (strRecver)
	{
		GetInst(CMail).SetRecver( string(strRecver,strlen(strRecver)) );
	}
	
	CEGUI::Editbox* eTitle = WEditBox(GetWndMgr().getWindow(TITLE));//主题
	const char* strTitle = CEGUIStringToAnsiChar(eTitle->getText());
	if (strTitle)
	{
		GetInst(CMail).SetTitle( string(strTitle,strlen(strTitle)) );
	}
	
	CEGUI::Editbox* eContent = WEditBox(GetWndMgr().getWindow(CONTENT));//内容
	const char* strContent = CEGUIStringToAnsiChar(eContent->getText());
	if (strContent)
	{
		GetInst(CMail).SetContent( string(strContent,strlen(strContent)));
	}
	
	CEGUI::Editbox* eGoldNum = WEditBox(GetWndMgr().getWindow(MONEY));//赠送金币或者要付的金币
	CEGUI::String strGoldNum = eGoldNum->getText();	
	GetInst(CMail).SetGoldNum(atol(strGoldNum.c_str()));

	if (IsPaySel())//付款取信
	{
		GetInst(CMail).SetMailType(IsPaySel());
		//是否确定让对方付费收信？
		GetInst(MsgEventManager).PushEvent(Msg_Agree,AppFrame::GetText("Mail_14"),PayMailMsgBox);
	}	
	else
	{
		if (atoi( strGoldNum.c_str() ) > 0 || GetInst(CMail).GetGoodsList().size() > 0)
		{
			//您是否确定在邮件中附赠金币或物品？
			GetInst(MsgEventManager).PushEvent(Msg_Agree,AppFrame::GetText("Mail_31"),PresentMsgBox);
		}
		else
		{
			GetInst(CMail).DoSendMail();
		}	
	}
	
	return true;
}

bool  OnRejectMailButtonDown(const CEGUI::EventArgs &args)
{
	int mailIndex = GetMailIndex();
	if ( mailIndex < 0 )
	{
		//请选择一封邮件
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Mail_34"));
		return false;
	}
	else
	{
		GetInst(CMail).DoRejectMail(mailIndex);
		return true;
	}
}

void  PayMailMsgBox(MsgEvent& event,eMsgResult UserId)
{
	if (UserId == S_Ok)
	{
		if (GetInst(CMail).GetGoodsList().size() > 0)
		{
			//您是否确定在邮件中附赠金币或物品？
			GetInst(MsgEventManager).PushEvent(Msg_Agree,AppFrame::GetText("Mail_31"),PresentMsgBox);
		}
		else
		{
			GetInst(CMail).DoSendMail();
		}		
	}	
	else
	{
		RemoveAllRecvData();
		GetInst(CMail).ClearData();
	}
}
void  PresentMsgBox(MsgEvent& event,eMsgResult UserId)
{
	if (UserId == S_Ok)
	{
		GetInst(CMail).DoSendMail();
	}	
	else
	{
		RemoveAllSendData();
		GetInst(CMail).ClearData();
	}
}


int GetMailIndex()//点两次才能得到ID
{
	
	CEGUI::Listbox* lMailList = WListBox(GetWndMgr().getWindow(MAILLIST));
	int ID = -1;

	if (lMailList->getFirstSelectedItem()!=NULL)
	{
		ID = lMailList->getFirstSelectedItem()->getID();
	}

	return ID;
}

bool OnPayMailSelChanged(const CEGUI::EventArgs& args)//取信方式选择
{
	CEGUI::Checkbox* pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(PAYMAILSEL));
	SetStaticText(IsPaySel());
	return true;
}

void SetStaticText(bool bPayMailSel)
{
	if ( bPayMailSel )
	{	
		CEGUI::WindowManager::getSingleton().getWindow(STATIC_MONEY)->setText(ToCEGUIString(AppFrame::GetText("Mail_115")));
	}
	else
	{
		CEGUI::WindowManager::getSingleton().getWindow(STATIC_MONEY)->setText(ToCEGUIString(AppFrame::GetText("Mail_114")));
	}
}

bool OnGetGoldButtonDown(const CEGUI::EventArgs& args)
{
	GetInst(CMail).DoGetGold();
	
	return true;
}

const int GetListIndex(const unsigned int listIndex)
{
	if (listIndex == 0)
	{
		return 0;
	}
	CEGUI::String img_property = "";
	CEGUI::Window *pWnd = NULL;

	unsigned int index = listIndex; 
	for (unsigned int i = 0; i < listIndex; i++)
	{
		pWnd = CEGUI::WindowManager::getSingleton().getWindow(g_strAddGoods[i]);
		img_property = pWnd->getProperty("Image");
		//if (img_property.c_str() == "")
		if(strlen(img_property.c_str()) == 0)
		{
			--index;			
		}
	}

	return index;
}

CEGUI::Window * GetSendGoodsWindow(const unsigned int index)
{
	CEGUI::Window *pWnd =NULL;
	pWnd = CEGUI::WindowManager::getSingleton().getWindow(g_strAddGoods[index]);

	return pWnd;
}


CEGUI::Window * GetRecvGoodsWindow(const unsigned int index)
{
	CEGUI::Window *pWnd = CEGUI::WindowManager::getSingleton().getWindow(g_strGetGoods[index]);;

	return pWnd;
}


void RemoveAllRecvData()
{	
	for (int i = 0; i < 4; i++)
	{
		CEGUI::Window *pWnd = GetRecvGoodsWindow(i);
		if (!pWnd)
		{
			return;
		}
		pWnd->setProperty("Image",""); 
	}
	//清除金币
	CEGUI::Editbox* eGold = WEditBox(GetWndMgr().getWindow(GOLD));
	eGold->setText("");
	//清除邮件内容
	CEGUI::Editbox* eText = WEditBox(GetWndMgr().getWindow(READMAIL));
	eText->setText("");
	//清除选择
	ResetSelBox();
}

void  ResetSelBox()
{
	CEGUI::Checkbox* pBox = NULL;
	pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(CHECKBOX0));
	if(pBox->isSelected())
	{
		pBox->setSelected(false);
	}
	pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(CHECKBOX1));
	if(pBox->isSelected())
	{
		pBox->setSelected(false);
	}
	pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(CHECKBOX2));
	if(pBox->isSelected())
	{
		pBox->setSelected(false);
	}
	pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(PAYMAILSEL));
	if(pBox->isSelected())
	{
		pBox->setSelected(false);
	}
}

void RemoveAllSendData()
{	
	//更新物品
	for (int i = 0; i < 4; i++)
	{
		CEGUI::Window *pWnd = GetSendGoodsWindow(i);
		if (!pWnd)
		{
			return;
		}
		pWnd->setProperty("Image",""); 
	}	
	//更新付款取信选择
	CEGUI::Checkbox* pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(PAYMAILSEL));
	if(pBox->isSelected())
	{
		pBox->setSelected(false);
	}
	CEGUI::Editbox* eRecver = WEditBox(GetWndMgr().getWindow(RECVER));//收信人
	eRecver->setText("");
	CEGUI::Editbox* eTitle = WEditBox(GetWndMgr().getWindow(TITLE));//主题
	eTitle->setText("");
	CEGUI::Editbox* eContent = WEditBox(GetWndMgr().getWindow(CONTENT));//内容
	eContent->setText("");
	CEGUI::Editbox* eGoldNum = WEditBox(GetWndMgr().getWindow(MONEY));//赠送金币或者要付的金币
	eGoldNum->setText("");	
}

bool OnUpdateSendData(const CEGUI::EventArgs& args)
{
	RemoveAllSendData();

	return true;
}

bool  IsDelSel(int sel)
{
	CEGUI::Checkbox* pBox = NULL;

	if ( sel == 0 )
	{
		pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(CHECKBOX0));
		if(pBox->isSelected())
		{
			return true;
		}
		return false;
	}
	if ( sel == 1 )
	{
		pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(CHECKBOX1));
		if(pBox->isSelected())
		{
			return true;
		}
		return false;
	}
	if ( sel == 2 )
	{
		pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(CHECKBOX2));
		if(pBox->isSelected())
		{
			return true;
		}
		return false;
	}
	
	return false;
}

bool IsPaySel()
{
	CEGUI::Checkbox* pBox = static_cast<CEGUI::Checkbox*>(GetWndMgr().getWindow(PAYMAILSEL));
	if(pBox->isSelected())
	{
		return true;
	}
	return false;
}