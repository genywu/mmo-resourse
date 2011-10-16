#include "stdafx.h"
#include "Pet.h"
#include "PetManager.h"
#include "../Player.h"
#include "../../Game.h"


//请求服务器改变宠物状态
bool AskforSetPetAwait(const CPet* pet)
{
    //先检查操作的合法性
    if (pet->GetPetState() != PET_STATE_FIGHT &&
        pet->GetPetState() != PET_STATE_ACTIVE)
        return false;

    //请求服务器改变宠物的当前战斗状态
    CMessage msg(MSG_C2S_PET_CHANGE_STATE_AWAIT);
    msg.Add(pet->GetExID());
    msg.Send();
    return true;
}

bool AskforSetPetFight(const CPet* pet)
{
    //先检查操作的合法性
    if (pet->GetPetState() != PET_STATE_AWAIT &&
        pet->GetPetState() != PET_STATE_ACTIVE)
        return false;

    //请求服务器改变宠物的当前战斗状态
    CMessage msg(MSG_C2S_PET_CHANGE_STATE_FIGHT);
    msg.Add(pet->GetExID());
    msg.Send();
    return true;
}

bool AskforSetPetActive(const CPet* pet)
{
    //先检查操作的合法性
    if (pet->GetPetState() != PET_STATE_FIGHT)
        return false;

    //请求服务器改变宠物的当前战斗状态
    CMessage msg(MSG_C2S_PET_CHANGE_STATE_ACTIVE);
    msg.Add(pet->GetExID());
    msg.Send();
    return true;
}

PetManager::PetManager(void)
{

}

PetManager::~PetManager(void)
{

}

/// 宠物修改名称
void PetManager::OnChangePetName(const CGUID& guid, const char* name)
{
	CPet* pet = GetPet(guid);
	if (!pet)
		return;
	
	pet->SetPetName(name);

	CMessage msg(MSG_C2S_PET_CHANG_NAME);
	msg.Add(pet->GetExID());
	msg.Add(name);
	msg.Send();
}

/// 删除宠物
void PetManager::OnDeletePet(const CGUID& guid)
{
	CPet* pet = GetPet(guid);
	if (!pet)
		return;

	CMessage msg(MSG_C2S_PET_CANCEL);
	msg.Add(guid);
	msg.Send();
}

/// 宠物升级
void PetManager::OnPetLevUpgrade(const CGUID& guid)
{
	CPet* pet = GetPet(guid);
	if (!pet)
		return;

	CMessage msg(MSG_C2S_PET_UPGRADE);
	msg.Add(guid);
	msg.Send();
}

/// 宠物升级道行等级
void PetManager::OnPetDLevUpgrade(const CGUID& guid)
{
	CPet* pet = GetPet(guid);
	if (!pet)
		return;

	CMessage msg(MSG_C2S_PET_UPGRADE_DLEV);
	msg.Add(guid);
	msg.Send();
}

/// 宠物改变状态
void PetManager::OnPetChangeState(const CGUID& guid)
{
	CPet* pet = GetPet(guid);
	if (!pet)
		return;

	long lStateType = 0;
	CMessage msg(MSG_C2S_PET_CHANGE_STATE);
	msg.Add(guid);
	msg.Add(lStateType);
	msg.Send();
}

/// 根据宠物GUID取得宠物
CPet* PetManager::GetPet(const CGUID& guid)
{
	CPet* pet = NULL;
	CPlayer* player = GetGame()->GetMainPlayer();
	if (player)
	{
		pet = player->GetPet(guid);
		return pet;
	}
	return pet;
}