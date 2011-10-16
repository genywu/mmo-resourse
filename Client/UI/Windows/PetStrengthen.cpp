#include "StdAfx.h"
#include "PetStrengthen.h"
#include "../UIDef.h"

#include "../../../Public/Common/BasePetDef.h"
#include "../../../Public/Common/PetSetup.h"

#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/PetSystem/Pet.h"

CEGUI::Window* InitPetStrengthen();								///< 初始化宠物强化界面

bool OnSelectPetUp(const CEGUI::EventArgs& e);					///< 选择宠物界面上翻
bool OnSelectPetDown(const CEGUI::EventArgs& e);				///< 选择宠物界面下翻
bool OnStrengthenPetAffirm(const CEGUI::EventArgs& e);			///< 强化操作确认按钮
bool OnPetIconDropped(const CEGUI::EventArgs& e);				///< 选择宠物拖拽操作完成
bool OnPetIconDragStart(const CEGUI::EventArgs& e);				///< 选择宠物拖拽操作开始
bool OnPetIconMouseRBtnDown(const CEGUI::EventArgs& e);			///< 选择宠物右键操作

void InitPetSelectWnd(CEGUI::Window* mainPage);					///< 初始化选择宠物界面
void InitPetStrenthenWnd(CEGUI::Window* mainPage, long type);	///< 初始化强化操作界面
void SubscribePetStrenthenEvent(CEGUI::Window* mainPage);		///< 注册事件
void UpdatePetStrenthenWnd(CEGUI::Window* mainPage, long type);	

bool PetStrenthen::OnOpen(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
	if( !wnd ) return false;

	long type = GetType();

	InitPetStrenthenWnd(wnd, type);

	UpdatePetStrenthenWnd(wnd, type);
	return true;
}

bool PetStrenthen::OnClose(const CEGUI::EventArgs& e)
{
	CEGUI::Window* wnd = static_cast<const CEGUI::WindowEventArgs&>(e).window;
	if( !wnd ) return false;

	wnd->setVisible(false);
	return true;
}

CEGUI::Window* InitPetStrengthen()
{
#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("PetStrenthenLayout.log");
#endif

	CEGUI::Window* wnd = LoadUI("PetStrengthen");

#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("CEGUI.log",true);
#endif

	InitPetSelectWnd(wnd);
	SubscribePetStrenthenEvent(wnd);

	wnd->setVisible(false);
	return wnd;
}

bool OnSelectPetUp(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnSelectPetDown(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnStrengthenPetAffirm(const CEGUI::EventArgs& e)
{
	CEGUI::PushButton* pAffirmBtn = WPushButton(WEArgs(e).window);
	if (!pAffirmBtn)
		return false;

	return true;
}

void InitPetSelectWnd(CEGUI::Window* mainPage)
{
	if (!mainPage)
		return;

	CEGUI::Window* wnd;
	char tempText[256];

	for (int i = 0; i < PET_SELECT_WND_CNT; ++i)
	{
		sprintf(tempText, "PetStrengthen/PetSelectWnd/Pet%d/DragContainer", i+1);
		wnd = mainPage->getChildRecursive(tempText);
		if (wnd)
		{
			wnd->setSize(CEGUI::UVector2(cegui_absdim(32 + 0),cegui_absdim(32 + 0)));
			wnd->setPosition(CEGUI::UVector2(cegui_absdim(5),cegui_absdim(5)));
		}

		sprintf(tempText, "PetStrengthen/PetSelectWnd/Pet%d", i+1);
		wnd = mainPage->getChildRecursive(tempText);
		if (wnd)
		{
			wnd->setSize(CEGUI::UVector2(cegui_absdim(32 + 10),cegui_absdim(32 + 10)));
		}
	}
}

void InitPetStrenthenWnd(CEGUI::Window* mainPage, long type)
{
	if (!mainPage || (type<PET_QUALITYUP_ID || type>PET_FFUP_ID))
		return;

	char tempText[256];

	for (int i = PET_QUALITYUP_ID; i <= PET_FFUP_ID; i+=PET_QUALITYUP_ID)
	{
		sprintf(tempText, "PetStrengthen/%d", i);
		CEGUI::Window* strenthenWnd = mainPage->getChildRecursive(tempText);
		if (strenthenWnd)
		{
			strenthenWnd->setVisible(false);
		}
	}

	sprintf(tempText, "PetStrengthen/%d", type);
	CEGUI::Window* strenthenWnd = mainPage->getChildRecursive(tempText);
	if (strenthenWnd)
	{
		strenthenWnd->setVisible(true);
	}

	CEGUI::Window* explainTextWnd = mainPage->getChildRecursive("PetStrengthen/ExplainText");
	if (explainTextWnd)
	{
		explainTextWnd->setText(GetInst(PetSetup).GetExplainTextByID(type).c_str());
	}
}

void SubscribePetStrenthenEvent(CEGUI::Window* mainPage)
{
	if (!mainPage)
		return;

	CEGUI::Window* btn;
	char tempText[256];

	// 注册选择宠物界面上翻事件
	btn = WPushButton(mainPage->getChildRecursive("PetStrengthen/PetSelectWnd/UpBtn"));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnSelectPetUp));
	}

	// 注册选择宠物界面下翻事件
	btn = WPushButton(mainPage->getChildRecursive("PetStrengthen/PetSelectWnd/DownBtn"));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnSelectPetDown));
	}

	// 注册确认事件
	btn = WPushButton(mainPage->getChild("PetStrengthen/AffirmBtn"));
	if (btn)
	{
		btn->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(OnStrengthenPetAffirm));
	}

	// 选择宠物界面 DragContainer 事件
	for (int i = 0; i < PET_SELECT_WND_CNT; ++i)
	{
		sprintf(tempText, "PetStrengthen/PetSelectWnd/Pet%d", i+1);
		CEGUI::Window* dragContainer = mainPage->getChildRecursive(tempText);
		sprintf(tempText, "PetStrengthen/PetSelectWnd/Pet%d/DragContainer", i+1);
		CEGUI::Window* dragItem = mainPage->getChildRecursive(tempText);
		if (dragContainer && dragItem)
		{
			dragContainer->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
				CEGUI::Event::Subscriber(OnPetIconDropped));

			dragItem->subscribeEvent(CEGUI::DragContainer::EventDragStarted,
				CEGUI::Event::Subscriber(OnPetIconDragStart));

			dragItem->subscribeEvent(CEGUI::Window::EventMouseButtonDown,
				CEGUI::Event::Subscriber(OnPetIconMouseRBtnDown));
		}
	}

	// 品质强化界面 DragContainer 事件
	/*dragContainer = mainPage->getChildRecursive("PetStrengthen/1000/MainPetIcon/DragContainer");
	if (dragContainer)
	{
	}*/

	// 主界面打开&关闭操作
	mainPage->subscribeEvent(EVENT_OPEN,
		CEGUI::Event::Subscriber(&PetStrenthen::OnOpen,&GetInst(PetStrenthen)));
	mainPage->subscribeEvent(EVENT_CLOSE,
		CEGUI::Event::Subscriber(&PetStrenthen::OnClose,&GetInst(PetStrenthen)));
}

void UpdatePetStrenthenWnd(CEGUI::Window* mainPage, long type)
{
	if (!mainPage)
		return;

	char tempText[256];

	CPlayer* player = GetGame()->GetMainPlayer();
	map<CGUID, CPet*>* petList = player->GetPetList();
	map<CGUID, CPet*>::iterator iterPet = petList->begin();
	for (int i=0; iterPet!=petList->end(); ++iterPet,++i)
	{
		sprintf(tempText, "PetStrengthen/PetSelectWnd/Pet%d/DragContainer", i+1);
		CEGUI::Window* dragItem = mainPage->getChildRecursive(tempText);
		if (dragItem)
		{
			CEGUI::GUISheet* childImg = WGUISheet(dragItem->getChildAtIdx(0));
			if(!childImg)
				return;
			SetBackGroundImage(childImg,"PetID","pictures\\Pet\\PetIcon","pet.jpg");
		}
		if (i>=PET_SELECT_WND_CNT-1)
			break;
	}
}

bool OnPetIconDropped(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnPetIconDragStart(const CEGUI::EventArgs& e)
{
	return true;
}

bool OnPetIconMouseRBtnDown(const CEGUI::EventArgs& e)
{
	return true;
}