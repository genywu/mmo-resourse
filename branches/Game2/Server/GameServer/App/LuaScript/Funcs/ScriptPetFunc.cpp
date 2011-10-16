///
/// @file ScriptPetFunc.cpp
/// @brief ²Î¼û ScriptPetFunc.h
///

#include "StdAfx.h"
#include "../Script.h"
#include "../../Player.h"
#include "../../Pet.h"
#include "../../PetCtrl.h"

bool Script::CreatePet(long type, long id, long isWild)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).CreatePet(player, type, id, isWild>0 ? true:false);
}

bool Script::DelPet(const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).DelPet(player, guid);
}

bool Script::ActivePet(const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).ActivePet(player, guid);
}

bool Script::SetPetAttr(const char* attrName, double attrVal, const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if (player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).SetPetAttr(player, attrName, attrVal, guid);
}

long Script::GetPetAttr(const char* attrName, const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return 0;
	}
	long val = GetInst(CPetCtrl).GetPetAttr(player, attrName, guid);
	return val;
}

bool Script::AddPetSkill(const CGUID& guid, long id, long level, long pos)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).AddPetSkill(player, guid, id, level, pos);
}

const CGUID& Script::GetFocusPetGUID()
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return NULL_GUID;
	}
	return player->GetFocusPetGUID();
}

long Script::GetPetGrade(const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return -1;
	}
	return GetInst(CPetCtrl).GetPetGrade(player, guid);
}

long Script::GetPetDAfterCnt(const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return -1;
	}
	return GetInst(CPetCtrl).GetPetDAfterCnt(player, guid);
}

bool Script::ResetPetTalent(const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).ResetPetTalent(player, guid);
}

bool Script::ResetPetDAfterCnt(const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).ResetPetDAfterCnt(player, guid);
}

bool Script::UpgradePetDAfterVal(const CGUID& guid, long val)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).UpgradePetDAfterVal(player, guid, val);
}

bool Script::ResetPetSavvy(const CGUID& guid)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).ResetPetSavvy(player, guid);
}

long Script::GetMaxServantNum(void)
{
	return GetInst(CPetCtrl).GetMaxServantNum();
}


void Script::IncPetExp(const CGUID& guid, long val)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return;
	}
	return GetInst(CPetCtrl).IncPetExp(player, guid, val);
}

void Script::IncPetDExp(const CGUID& guid, long val)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return;
	}
	return GetInst(CPetCtrl).IncPetDExp(player, guid, val);
}

bool Script::PetLevUp(const CGUID& guid, long newLev)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if ( player == NULL)
	{
		return false;
	}
	return GetInst(CPetCtrl).PetLevUp(player, guid, newLev);
}