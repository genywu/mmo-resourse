/*

* MiniPetPage即被激活的宠物的界面，

* 目前包含头像，HP，MP，Energy的显示

*/

#include "StdAfx.h"
#include "../UIDef.h" // for LoadUI
#include "../../GameClient/ClientApp/PetSystem/Pet.h" // for CPet
#include "../../GameClient/Game.h" // for CGame::GetMainPlayer
#include "../../GameClient/ClientApp/Player.h" // for CPlayer::GetActivePet

bool OnMiniPetPageShow(const CEGUI::EventArgs& e)
{
    CPet* pet = GetGame()->GetMainPlayer()->GetActivePet();
    if (NULL == pet)
        return false;

    CEGUI::Window* wnd = WEArgs(e).window;

    char buf[32];
    CEGUI::String curValue = _itoa(pet->GetHp(), buf, 10);
    CEGUI::String maxValue = _itoa(pet->GetMaxHp(), buf, 10);
    wnd->getChild("MiniPetPage/Hp")->setText(curValue + "/" + maxValue);

    curValue = _itoa(pet->GetMp(), buf, 10);
    maxValue = _itoa(pet->GetMaxMp(), buf, 10);
    wnd->getChild("MiniPetPage/Mp")->setText(curValue + "/" + maxValue);

    curValue = _itoa(pet->GetEnergy(), buf, 10);
    maxValue = _itoa(pet->GetMaxEnergy(), buf, 10);
    wnd->getChild("MiniPetPage/Energy")->setText(curValue + "/" + maxValue);

    wnd->show();
    return true;
}

bool OnMiniPetPageHide(const CEGUI::EventArgs& e)
{
    WEArgs(e).window->hide();
    return true;
}

bool OnMiniPetHpChanged(const CEGUI::EventArgs& e)
{
    CEGUI::Window* wnd = WEArgs(e).window;
    CPet* pet = GetGame()->GetMainPlayer()->GetActivePet();
    if (NULL == pet)
        return false;

    char buf[32];
    CEGUI::String curValue = _itoa(pet->GetHp(), buf, 10);
    CEGUI::String maxValue = _itoa(pet->GetMaxHp(), buf, 10);
    wnd->setText(curValue + "/" + maxValue);
    return true;
}

bool OnMiniPetMpChanged(const CEGUI::EventArgs& e)
{
    CEGUI::Window* wnd = WEArgs(e).window;
    CPet* pet = GetGame()->GetMainPlayer()->GetActivePet();
    if (NULL == pet)
        return false;

    char buf[32];
    CEGUI::String curValue = _itoa(pet->GetMp(), buf, 10);
    CEGUI::String maxValue = _itoa(pet->GetMaxMp(), buf, 10);
    wnd->setText(curValue + "/" + maxValue);
    return true;
}

bool OnMiniPetEnergyChanged(const CEGUI::EventArgs& e)
{
    CEGUI::Window* wnd = WEArgs(e).window;
    CPet* pet = GetGame()->GetMainPlayer()->GetActivePet();
    if (NULL == pet)
        return false;

    char buf[32];
    CEGUI::String curValue = _itoa(pet->GetEnergy(), buf, 10);
    CEGUI::String maxValue = _itoa(pet->GetMaxEnergy(), buf, 10);
    wnd->setText(curValue + "/" + maxValue);
    return true;
}

CEGUI::Window* InitMiniPetPage()
{
    CEGUI::Window* wnd = LoadUI("MiniPetPage");
    wnd->subscribeEvent("MyShow", CEGUI::SubscriberSlot(OnMiniPetPageShow));
    wnd->subscribeEvent("MyHide", CEGUI::SubscriberSlot(OnMiniPetPageHide));
    wnd->getChild("MiniPetPage/Hp")->subscribeEvent("ChangeHp", CEGUI::SubscriberSlot(OnMiniPetHpChanged));
    wnd->getChild("MiniPetPage/Mp")->subscribeEvent("ChangeMp", CEGUI::SubscriberSlot(OnMiniPetMpChanged));
    wnd->getChild("MiniPetPage/Energy")->subscribeEvent("ChangeEnergy", CEGUI::SubscriberSlot(OnMiniPetEnergyChanged));
    wnd->hide();
    return wnd;
}