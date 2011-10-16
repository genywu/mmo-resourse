//pet和servant是同义词
#include "StdAfx.h"
#include "PetStrengthen.h"
#include "../UIDef.h"

#include "../../../Public/Common/MsgType.h"
#include "../../../Public/Common/BasePetDef.h"
#include "../../GameClient/Game.h"
#include "../../GameClient/ClientApp/Player.h"
#include "../../GameClient/ClientApp/PetSystem/Pet.h"

//for comment, maybe deleted later
#include "../CEGUI/cegui/include/CEGUIWindowManager.h"
#include "../CEGUI/cegui/include/CEGUIWindow.h"
#include "../../Net/Message.h"

const char* strRaceType[]=
{
	"人类",		//0
	"草本",		//1
	"虫",		//2
	"混沌",		//3
	"机关",		//4
	"龙",		//5
	"罗汉",		//6
	"亡魂",		//7
	"仙灵",		//8
	"修罗",		//9
	"妖魔",		//10
	"妖兽",		//11
	"鱼贝",		//12
	"元素",		//13
	"错误",		//14
};

extern CEGUI::Window* InitPetStrengthen(long type);

bool OnWindowEnabled(const CEGUI::EventArgs &e);
bool OnWindowDisabled(const CEGUI::EventArgs &e);

/// state button
bool OnAwaitButtonClicked(const CEGUI::EventArgs &e);   ///< 宠物待机按钮
bool OnFightButtonClicked(const CEGUI::EventArgs &e);   ///< 宠物出战按钮
bool OnActiveButtonClicked(const CEGUI::EventArgs &e);  ///< 宠物激活按钮

/// pet list
bool OnPetListSelectChanged(const CEGUI::EventArgs& e); ///< 选择宠物
bool OnPetListStateChanged(const CEGUI::EventArgs& e);  ///< 宠物改变状态（未）

void ShowPetAttribute(const CPet* pet);                 ///< 显示宠物属性（未）

CEGUI::Window* InitServant();							///< 初始化宠物界面
bool OnChangePetName(const CEGUI::EventArgs &e);		///< 宠物更改名字
bool OnDeletePet(const CEGUI::EventArgs &e);			///< 删除宠物按钮事件
bool OnPetListAdded(const CEGUI::EventArgs &e);			///< 添加宠物
bool OnPetListDeleted(const CEGUI::EventArgs &e);		///< 删除宠物

bool OnPetLevUpgrade(const CEGUI::EventArgs& e);		///< 宠物升级
bool OnPetDLevUpgrade(const CEGUI::EventArgs& e);		///< 宠物道行等级提升
bool OnPetUseGoodsBtnClicked(const CEGUI::EventArgs &e);

CPet* GetCurOperPet(void);								///< 取得当前操作宠物


CEGUI::Window* InitServant()
{
#ifdef _DEBUG
	CEGUI::Logger::getSingleton().setLogFilename("PetLayout.log");
#endif
    CEGUI::Window* wnd = LoadUI("Servant");
	CEGUI::Window* btn;

	// 事件注册

	// 玩家宠物列表
	CPlayer* player = GetGame()->GetMainPlayer();
	map<CGUID, CPet*>* petList = player->GetPetList();
	CEGUI::Listbox* lb = WListBox(wnd->getChildRecursive("Servant/Pets"));
	if (lb)
	{
		// 注册选择宠物事件
		lb->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
			CEGUI::Event::Subscriber(OnPetListSelectChanged));
		// 注册添加宠物事件
		lb->subscribeEvent("AddPet",
			CEGUI::SubscriberSlot(OnPetListAdded));
		// 注册删除宠物事件
		lb->subscribeEvent("DelPet",
			CEGUI::SubscriberSlot(OnPetListDeleted));
        // 注册改变宠物状态事件
        lb->subscribeEvent("ChangePetState",
            CEGUI::SubscriberSlot(OnPetListStateChanged));
		
		map<CGUID, CPet*>::iterator iterPet = petList->begin();
		for (; iterPet!=petList->end(); ++iterPet)
		{
			// 这里只添加宠物名称，宠物头像以后添加
			CEGUI::ListboxTextItem* item = 
				new CEGUI::ListboxTextItem(ToCEGUIString(iterPet->second->GetPetName()));
			item->setUserData(iterPet->second);
			item->setSelectionBrushImage("TaharezLook","TextSelectionBrush");
			// 默认打开界面时，选择并更新玩家第一只宠物信息
			if (iterPet == petList->begin())
			{
				item->setSelected(true);
				ShowPetAttribute(iterPet->second);
			}
			lb->addItem(item);
		}
	}

	// 注册更改名称按钮事件
	btn = wnd->getChild("Servant/ChangePetNameButton");
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,
		CEGUI::SubscriberSlot(OnChangePetName));

	// 注册删除宠物按钮事件
	btn = wnd->getChild("Servant/PetOut");
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,
		CEGUI::SubscriberSlot(OnDeletePet));

	// 注册宠物提升等级按钮事件
	btn = wnd->getChild("Servant/PetLevUpgrade");
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,
		CEGUI::SubscriberSlot(OnPetLevUpgrade));

	// 注册宠物提升道行等级按钮事件
	btn = wnd->getChild("Servant/PetDLevUpgrade");
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,
		CEGUI::SubscriberSlot(OnPetDLevUpgrade));

    //注册宠物待机按钮事件——click,enable,disable
    btn = wnd->getChildRecursive("Servant/Pet/Await");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::SubscriberSlot(OnAwaitButtonClicked));
    btn->subscribeEvent("MyEnabled",
        CEGUI::SubscriberSlot(OnWindowEnabled));
    btn->subscribeEvent("MyDisabled",
        CEGUI::SubscriberSlot(OnWindowDisabled));

    //注册宠物战斗按钮事件——click,enable,disable
    btn = wnd->getChildRecursive("Servant/Pet/Fight");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::SubscriberSlot(OnFightButtonClicked));
    btn->subscribeEvent("MyEnabled",
        CEGUI::SubscriberSlot(OnWindowEnabled));
    btn->subscribeEvent("MyDisabled",
        CEGUI::SubscriberSlot(OnWindowDisabled));

    //注册宠物激活按钮事件——click,enable,disable
    btn = wnd->getChildRecursive("Servant/Pet/Active");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked,
        CEGUI::SubscriberSlot(OnActiveButtonClicked));
    btn->subscribeEvent("MyEnabled",
        CEGUI::SubscriberSlot(OnWindowEnabled));
    btn->subscribeEvent("MyDisabled",
        CEGUI::SubscriberSlot(OnWindowDisabled));

	// 注册使用物品按钮事件
	btn = wnd->getChildRecursive("Servant/PetUseGoods");
	btn->subscribeEvent(CEGUI::PushButton::EventClicked,
		CEGUI::SubscriberSlot(OnPetUseGoodsBtnClicked));

    wnd->setVisible(false);//这样才能在第一次按快捷键就显示界面
    return wnd;
}

bool OnWindowEnabled(const CEGUI::EventArgs &e)
{
    CEGUI::Window* wnd = WEArgs(e).window;
    if (NULL == wnd)
        return false;

    wnd->enable();
    return true;
}

bool OnWindowDisabled(const CEGUI::EventArgs &e)
{
    CEGUI::Window* wnd = WEArgs(e).window;
    if (NULL == wnd)
        return false;

    wnd->disable();
    return true;
}

bool OnPetUseGoodsBtnClicked(const CEGUI::EventArgs &e)
{
	CEGUI::PushButton* pUseGoodsBtn = WPushButton(WEArgs(e).window);
	if (!pUseGoodsBtn)
		return false;

	// 打开宠物强化界面
	GetInst(PetStrenthen).SetType(1000);
	FireUIEvent("PetStrengthen",EVENT_SWITCH);

	return true;
}

bool OnAwaitButtonClicked(const CEGUI::EventArgs &e)
{
    //找到待操作的宠物
    CPet* pet = GetCurOperPet();
    if (NULL == pet)
        return false;

    pet->AskforChangeState(PET_STATE_AWAIT);
    return true;
}

bool OnFightButtonClicked(const CEGUI::EventArgs &e)
{
    //找到待操作的宠物
    CPet* pet = GetCurOperPet();
    if (NULL == pet)
        return false;

    pet->AskforChangeState(PET_STATE_FIGHT);
    return true;
}

bool OnActiveButtonClicked(const CEGUI::EventArgs &e)
{
    //找到待操作的宠物
    CPet* pet = GetCurOperPet();
    if (NULL == pet)
        return false;

    pet->AskforChangeState(PET_STATE_ACTIVE);
    return true;
}

/// 宠物更改名字
bool OnChangePetName(const CEGUI::EventArgs &e)
{
	CEGUI::PushButton* pChangeNameBtn = WPushButton(WEArgs(e).window);
	if (!pChangeNameBtn)
		return false;

	CEGUI::Editbox* nameBox = WEditBox(GetWndMgr().getWindow("Servant/PetName"));
	if (!nameBox)
		return false;

	CEGUI::Listbox* lb = WListBox(GetWndMgr().getWindow("Servant/Pets"));
	if (!lb)
		return false;

	CEGUI::ListboxItem* lbi = lb->getFirstSelectedItem();
	if(!lbi)
	{
		nameBox->setText("");
		return false;
	}

	CPet* pet = static_cast<CPet*>(lbi->getUserData());
	if (!pet)
		return false;

	CEGUI::String name = nameBox->getText();
	lbi->setText(name);
	nameBox->setText(name);

    pet->AskforChangeName(name.c_str());

	return true;
}

/// 删除宠物
bool OnDeletePet(const CEGUI::EventArgs &e)
{
	CEGUI::PushButton* pDelPetBtn = WPushButton(WEArgs(e).window);
	if (!pDelPetBtn)
		return false;

	CPet* pet = GetCurOperPet();
	if (!pet)
		return false;

    pet->AskforDelete();

	return true;
}

/// 宠物升级
bool OnPetLevUpgrade(const CEGUI::EventArgs& e)
{
	CEGUI::PushButton* pLevUpBtn = WPushButton(WEArgs(e).window);
	if (!pLevUpBtn)
		return false;

	CPet* pet = GetCurOperPet();
	if (!pet)
		return false;

    pet->AskforLvUp();
	
	return true;
}

/// 宠物道行等级提升
bool OnPetDLevUpgrade(const CEGUI::EventArgs& e)
{
	CEGUI::PushButton* pDLevUpBtn = WPushButton(WEArgs(e).window);
	if (!pDLevUpBtn)
		return false;

	CPet* pet = GetCurOperPet();
	if (!pet)
		return false;

    pet->AskforDLvUp();

	return true;
}

/// 添加宠物
bool OnPetListAdded(const CEGUI::EventArgs &e)
{
	CEGUI::Listbox* lb = WListBox(WEArgs(e).window);
	if (!lb)
		return false;

	CPlayer* player = GetGame()->GetMainPlayer();
	CPet* pet = player->GetCurPet();
	if (!pet)
		return false;

	CEGUI::ListboxTextItem* item = 
		new CEGUI::ListboxTextItem(pet->GetPetName());
	item->setUserData(pet);
	item->setSelectionBrushImage("TaharezLook","TextSelectionBrush");
	lb->addItem(item);

	// 默认更新第一只宠物信息
	CEGUI::ListboxItem* firstItem = lb->getListboxItemFromIndex(0);
	if (firstItem)
	{
		firstItem->setSelected(true);
		ShowPetAttribute(static_cast<CPet*>(firstItem->getUserData()));
	}

	return true;
}

/// 删除宠物
bool OnPetListDeleted(const CEGUI::EventArgs &e)
{
	CEGUI::Listbox* lb = WListBox(WEArgs(e).window);
	if (!lb)
		return false;

	CEGUI::ListboxItem* selectedItem = lb->getFirstSelectedItem();
	if(!selectedItem)
		return false;

	CPet* pet = static_cast<CPet*>(selectedItem->getUserData());
	if (!pet)
		return false;

	lb->removeItem(selectedItem);

	if (lb->getItemCount() != 0)
	{	
		// 默认更新第一只宠物信息
		CEGUI::ListboxItem* firstItem = lb->getListboxItemFromIndex(0);
		if (firstItem)
		{
			firstItem->setSelected(true);
			ShowPetAttribute(static_cast<CPet*>(firstItem->getUserData()));
		}
	}
	else
	{
		// 清空界面
		ShowPetAttribute(NULL);
	}
	return true;
}

/// 宠物列表中选择宠物操作
bool OnPetListSelectChanged(const CEGUI::EventArgs& e)
{
    CPet* pet = NULL;

    CEGUI::Listbox* petList = WListBox(WEArgs(e).window);
    if(NULL != petList)
    {
        CEGUI::ListboxItem* petItem = petList->getFirstSelectedItem();
        if(NULL != petItem)
            pet = (CPet*)petItem->getUserData();
    }

    ShowPetAttribute(pet);

	if (pet!=NULL)
	{
		GetGame()->GetMainPlayer()->SetCurOperPetGUID(pet->GetExID());
	}
    return true;
}

void ShowPetAttribute(const CPet* pet)
{
    //pet可以为null，在这种情况下，所有属性栏的显示文本被清空
    if (NULL == pet)
    {
        GetWndMgr().getWindow("Servant/PetName")->setText("");
        GetWndMgr().getWindow("Servant/PetTake")->setText("");
        GetWndMgr().getWindow("Servant/PetLevel")->setText("");
        GetWndMgr().getWindow("Servant/PetDLevel")->setText("");
        GetWndMgr().getWindow("Servant/PetHP")->setText("");
        GetWndMgr().getWindow("Servant/PetMP")->setText("");
        GetWndMgr().getWindow("Servant/PetIP")->setText("");
        GetWndMgr().getWindow("Servant/PetRace")->setText("");
        GetWndMgr().getWindow("Servant/PetLoyalty")->setText("");
        GetWndMgr().getWindow("Servant/PetLife")->setText("");
        GetWndMgr().getWindow("Servant/PetStr")->setText("");
        GetWndMgr().getWindow("Servant/PetDex")->setText("");
        GetWndMgr().getWindow("Servant/PetCon")->setText("");
        GetWndMgr().getWindow("Servant/PetInt")->setText("");
        GetWndMgr().getWindow("Servant/PetSpi")->setText("");
        GetWndMgr().getWindow("Servant/PetPhyAtk")->setText("");
        GetWndMgr().getWindow("Servant/PetMagAtk")->setText("");
        GetWndMgr().getWindow("Servant/PetPhyDef")->setText("");
        GetWndMgr().getWindow("Servant/PetMagDef")->setText("");
        GetWndMgr().getWindow("Servant/PetHit")->setText("");
        GetWndMgr().getWindow("Servant/PetDod")->setText("");
        GetWndMgr().getWindow("Servant/PetStrenthPoint")->setText("");
        GetWndMgr().getWindow("Servant/PetDexterityPoint")->setText("");
        GetWndMgr().getWindow("Servant/PetConPoint")->setText("");
        GetWndMgr().getWindow("Servant/PetIntellectPoint")->setText("");
        GetWndMgr().getWindow("Servant/PetSpiritualismPoint")->setText("");
        GetWndMgr().getWindow("Servant/PetExp")->setText("");
        GetWndMgr().getWindow("Servant/PetDExp")->setText("");
        GetWndMgr().getWindow("Servant/PetStrenthTalent")->setText("");
        GetWndMgr().getWindow("Servant/PetDexterityTalent")->setText("");
        GetWndMgr().getWindow("Servant/PetConTalent")->setText("");
        GetWndMgr().getWindow("Servant/PetIntellectTalent")->setText("");
        GetWndMgr().getWindow("Servant/PetSpiritualismTalent")->setText("");

        GetWndMgr().getWindow("Servant/Pet/Await")->disable();
        GetWndMgr().getWindow("Servant/Pet/Fight")->disable();
        GetWndMgr().getWindow("Servant/Pet/Active")->disable();
        return;
    }

    char buf[32];
    GetWndMgr().getWindow("Servant/PetName")->setText(ToCEGUIString(pet->GetPetName()));
    GetWndMgr().getWindow("Servant/PetTake")->setText(_itoa(pet->GetTakeLev(), buf, 10));
    GetWndMgr().getWindow("Servant/PetLevel")->setText(_itoa(pet->GetPetLevel(), buf, 10));
    GetWndMgr().getWindow("Servant/PetDLevel")->setText(_itoa(pet->GetPetDLevel(), buf, 10));
    GetWndMgr().getWindow("Servant/PetHP")->setText(CEGUI::String(_itoa(pet->GetHp(), buf, 10)) + "/" + _itoa(pet->GetMaxHp(), buf, 10));
    GetWndMgr().getWindow("Servant/PetMP")->setText(CEGUI::String(_itoa(pet->GetMp(), buf, 10)) + "/" + _itoa(pet->GetMaxMp(), buf, 10));
    GetWndMgr().getWindow("Servant/PetIP")->setText(CEGUI::String(_itoa(pet->GetEnergy(), buf, 10)) + "/" + _itoa(pet->GetMaxEnergy(), buf, 10));
    GetWndMgr().getWindow("Servant/PetRace")->setText(ToCEGUIString(strRaceType[pet->GetPetRace()]));
    GetWndMgr().getWindow("Servant/PetLoyalty")->setText(_itoa(pet->GetPetLoyalty(), buf, 10));
    GetWndMgr().getWindow("Servant/PetLife")->setText(_itoa(pet->GetPetLife(), buf, 10));
    GetWndMgr().getWindow("Servant/PetStr")->setText(_itoa(pet->GetPetStrenth(), buf, 10));
    GetWndMgr().getWindow("Servant/PetDex")->setText(_itoa(pet->GetPetDexterity(), buf, 10));
    GetWndMgr().getWindow("Servant/PetCon")->setText(_itoa(pet->GetPetCon(), buf, 10));
    GetWndMgr().getWindow("Servant/PetInt")->setText(_itoa(pet->GetPetIntellect(), buf, 10));
    GetWndMgr().getWindow("Servant/PetSpi")->setText(_itoa(pet->GetPetSpiritualism(), buf, 10));
    GetWndMgr().getWindow("Servant/PetPhyAtk")->setText(_itoa(pet->GetPetPhysiAtk(), buf, 10));
    GetWndMgr().getWindow("Servant/PetMagAtk")->setText(_itoa(pet->GetPetMagicATK(), buf, 10));
    GetWndMgr().getWindow("Servant/PetPhyDef")->setText(_itoa(pet->GetPetPhysiDef(), buf, 10));
    GetWndMgr().getWindow("Servant/PetMagDef")->setText(_itoa(pet->GetPetMagicDef(), buf, 10));
    GetWndMgr().getWindow("Servant/PetHit")->setText(_itoa(pet->GetPetHitRate(), buf, 10));
    GetWndMgr().getWindow("Servant/PetDod")->setText(_itoa(pet->GetPetMiss(), buf, 10));
    GetWndMgr().getWindow("Servant/PetStrenthPoint")->setText(_itoa(pet->GetPetStrenthPoint(), buf, 10));
    GetWndMgr().getWindow("Servant/PetDexterityPoint")->setText(_itoa(pet->GetPetDexterityPoint(), buf, 10));
    GetWndMgr().getWindow("Servant/PetConPoint")->setText(_itoa(pet->GetPetConPoint(), buf, 10));
    GetWndMgr().getWindow("Servant/PetIntellectPoint")->setText(_itoa(pet->GetPetIntellectPoint(), buf, 10));
    GetWndMgr().getWindow("Servant/PetSpiritualismPoint")->setText(_itoa(pet->GetPetSpiritualismPoint(), buf, 10));
    GetWndMgr().getWindow("Servant/PetExp")->setText(_itoa(pet->GetPetExp(), buf, 10));
    GetWndMgr().getWindow("Servant/PetDExp")->setText(_itoa(pet->GetPetDExp(), buf, 10));
    GetWndMgr().getWindow("Servant/PetStrenthTalent")->setText(_itoa(pet->GetPetStrenthTalent(), buf, 10));
    GetWndMgr().getWindow("Servant/PetDexterityTalent")->setText(_itoa(pet->GetPetDexterityTalent(), buf, 10));
    GetWndMgr().getWindow("Servant/PetConTalent")->setText(_itoa(pet->GetPetConTalent(), buf, 10));
    GetWndMgr().getWindow("Servant/PetIntellectTalent")->setText(_itoa(pet->GetPetIntellectTalent(), buf, 10));
    GetWndMgr().getWindow("Servant/PetSpiritualismTalent")->setText(_itoa(pet->GetPetSpiritualismTalent(), buf, 10));

    switch (pet->GetPetState())
    {
    case PET_STATE_AWAIT:
        GetWndMgr().getWindow("Servant/Pet/Await")->disable();
        GetWndMgr().getWindow("Servant/Pet/Fight")->enable();
        GetWndMgr().getWindow("Servant/Pet/Active")->disable();
        break;

    case PET_STATE_FIGHT:
        GetWndMgr().getWindow("Servant/Pet/Await")->enable();
        GetWndMgr().getWindow("Servant/Pet/Fight")->disable();
        GetWndMgr().getWindow("Servant/Pet/Active")->enable();
        break;

    case PET_STATE_ACTIVE:
        GetWndMgr().getWindow("Servant/Pet/Await")->enable();
        GetWndMgr().getWindow("Servant/Pet/Fight")->enable();
        GetWndMgr().getWindow("Servant/Pet/Active")->disable();
        break;

    case PET_STATE_SLEEP:
    default:
        GetWndMgr().getWindow("Servant/Pet/Await")->disable();
        GetWndMgr().getWindow("Servant/Pet/Fight")->disable();
        GetWndMgr().getWindow("Servant/Pet/Active")->disable();
        break;
    }

	// 宠物头像
	// 星等
	// 品质
	// 特性偏好
	// 天赋技能
	// 学习技能
}

/// 取得当前操作宠物
CPet* GetCurOperPet(void)
{
	CPet* pet = NULL;
	CEGUI::Listbox* lb = WListBox(GetWndMgr().getWindow("Servant/Pets"));
	if (lb)
	{
		CEGUI::ListboxItem* selectedItem = lb->getFirstSelectedItem();
		if (selectedItem)
		{
			pet = static_cast<CPet*>(selectedItem->getUserData());
			if (!pet)
				return NULL;
			return pet;
		}
		return NULL;
	}
	return NULL;
}

/// 宠物改变状态
bool OnPetListStateChanged(const CEGUI::EventArgs& e)
{
    // 根据pet list的user data重新绘制表示状态的图标
	CEGUI::Listbox* listbox = WListBox(WEArgs(e).window);
	if(NULL == listbox)
		return false;

    size_t itemCnt = listbox->getItemCount();
    for(size_t i = 0; i < itemCnt; ++i)
    {
        CEGUI::ListboxItem* item = listbox->getListboxItemFromIndex(i);
        if (NULL == item)
            continue;
        CPet* pet = (CPet*)item->getUserData();
        if (NULL == pet)
            continue;

        //看图标是否与pet的pet state一致，不一致则改图标
    }

    return true;
}
