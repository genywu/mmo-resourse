///
/// @file ScriptQuestFunc.cpp
/// @brief ²Î¼û ScriptQuestFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../../Player.h"

bool Script::AddQuest(int questId)
{
	CPlayer *player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;

	if(!player->AddQuest(questId))
		return false;
	GetGame()->UpdateSavePlayer(player, SAVE_DETAIL_ATTRQUESTLIST);
	return true;
}

int Script::GetMainQuestId()
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if (player== NULL)
		return EXEC_ERROR;
	return player->GetCurrentMasterRoleID();
}

bool Script::CompleteQuest(int questId)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->CompleteQuest(questId);
}

void Script::AbandonQuest(int questId)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return;
	player->RemoveQuest(questId);
}

int Script::GetQuestState(int questId)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;

	return player->GetQuestState(questId);
}

int Script::GetValidQuestAmount()
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetValidQuestNum();
}

void Script::UpdateQuestStep(int questId, int stepId)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return;
	player->UpdateQuestStep(questId, stepId);
}

void Script::UpdateQuestParam(int questId, int paramIdx, int param, int regionId, int x, int y)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return;
	player->UpdataQuestParamData(questId, paramIdx-1, param, regionId, x, y);
}

void Script::UpdateQuestData(int questId, int paramIdx, int value)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return;
	player->UpdataQuestData(questId, paramIdx-1, value);
}

int Script::GetQuestStepAmount(int questId)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetRoleStepNum(questId);
}

int Script::GetParam(int questId, int stepId, int idx)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetRoleParam(questId, stepId, idx-1);
}

int Script::GetQuestAccParam(int questId, int stepId, int idx)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetRoleAccParam(questId, stepId, idx-1);
}

bool Script::SetQuestAccParam(int questId, int stepId, int idx, int value)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->SetRoleAccParam(questId, stepId, idx-1, value);
}

bool Script::AddQuestAccParam(int questId, int stepId, int idx, int value)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->AddRoleAccParam(questId, stepId, idx-1, value);
}

int Script::GetQuestRegionID(int questId, int stepId, int idx)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetRoleRegionID(questId, stepId, idx-1);
}

bool Script::SetQuestRegionID(int questId, int stepId, int idx, int regionId)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->SetRoleRegionID(questId, stepId, idx-1, regionId);
}

int Script::GetQuestRegionX(int questId, int stepId, int idx)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetRoleRegionX(questId, stepId, idx-1);
}

bool Script::SetQuestRegionX(int questId, int stepId, int idx, int x)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->SetRoleRegionX(questId, stepId, idx-1, x);
}

int Script::GetQuestRegionY(int questId, int stepId, int idx)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetRoleRegionY(questId, stepId, idx-1);
}

bool Script::SetQuestRegionY(int questId, int stepId, int idx, int y)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->SetRoleRegionY(questId, stepId, idx-1, y);
}

int Script::GetQuestAmount()
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetRoleSum();
}

int Script::GetQuestChapter()
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetChapter();
}

bool Script::SetQuestChapter(int num)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->SetChapter(num);
}

bool Script::AddFinishedQuestRecord(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->AddFinishedRoleRecord(questId);
}

bool Script::DelFinishedQuestRecord(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->DelFinishedRoleRecord(questId);
}

bool Script::GetQuestFinishedRecord(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->GetRoleFinishedRecord(questId);
}

int Script::GetQuestTextType(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetTextType(questId);
}

const char* Script::GetQuestName(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if (player==NULL)
		return NULL;
	return player->GetAttributeRoleName(questId);
}

int Script::IsQuestRecorded(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if (player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeBeRecord(questId);
}

int Script::GetQuestType(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if (player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeQuestType(questId);
}

int Script::GetQuestIsShare(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeQuestIsShare(questId);
}

bool Script::GetQuestReqSkill(int questId)
{
    CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if(player==NULL)
        return false;
    return  player->GetAttributeQuestSkill(questId);
}

bool Script::GetQuestSexLimit(int questId)
{
    CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if(player==NULL)
        return false;
    return  player->GetAttributeQuestSex(questId);
}

bool Script::GetQuestTotePet(int questId)
{
    CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if(player==NULL)
        return false;
    return  player->GetAttributeTotePet(questId);
}

int Script::IsQuestDisplayed(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeShowAble(questId);
}

int Script::IsQuestRepeated(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeRepeated(questId);
}

int Script::IsQuestFailOnDead(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeDeadLost(questId);
}

int Script::IsQuestRepeatedOnFail(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeLostRepeated(questId);
}

int Script::GetQuestMinLvLimit(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeMinGrade(questId);
}

int Script::GetQuestMaxLvLimit(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeMaxGrade(questId);
}

int Script::GetQuestOccupationLimit(int questId, int occupationIdx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeOccupationRestrict(questId, occupationIdx);
}

int Script::GetQuestChapterAmount(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeChapterNum(questId);
}

int Script::GetNextQuestId(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAttributeNextRoleID(questId);
}

int Script::GetPreQuest(int questId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetLabelPrefix(questId, idx);
}

int Script::GetPreQuestAmount(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetLabelSum(questId);
}

int Script::GetAimAMTType(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAimAMTType(questId, stepId, aimId);
}

int Script::GetAimType(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAimType(questId, stepId, aimId);
}

int Script::GetAimId(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAimId(questId, stepId, aimId);
}

int Script::GetAimAmount(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAimNum(questId, stepId, aimId);
}

int Script::GetAimParameIndex(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetAimParameIndex(questId, stepId, aimId);
}

int Script::GetDropFrom(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetDropFrom(questId, stepId, aimId);
}

int Script::GetDropOdds(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetDropOdds(questId, stepId, aimId);
}

const char* Script::GetAimName(int questId, int stepId, int aimId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL;
	return player->GetAimName(questId, stepId, aimId);
}

int Script::GetPrepareParam(int questId, int stepId, int aimId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetPrepareParam(questId, stepId, aimId, idx-1);
}

int Script::GetQuestEncouragementSum(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetQuestEncouragementSum(questId);
}

const char* Script::GetStepEncouragementName(int questId, int stepId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL;
	return player->GetSetpEncouragementName(questId, stepId, idx);
}

const char* Script::GetQuestEncouragementName(int questId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL;
	return player->GetQuestEncouragementName(questId, idx);
}

int Script::GetStepEncouragementType(int questId, int stepId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetStepEncouragementType(questId, stepId, idx);
}

int Script::GetQuestEncouragementType(int questId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetQuestEncouragementType(questId, idx);
}

int Script::GetStepEncouragementID(int questId, int stepId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetStepEncouragementID(questId, stepId, idx);
}

int Script::GetQuestEncouragementID(int questId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetQuestEncouragementID(questId, idx);
}

int Script::GetStepEncouragementNum(int questId, int stepId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetStepEncouragementNum(questId, stepId, idx);
}

int Script::GetQuestEncouragementNum(int questId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetQuestEncouragementNum(questId, idx);
}

int Script::GetStepEncouragementCell(int questId, int idx)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetStepEncouragementCell(questId, idx);
}

int Script::GetQuestEncouragementCell(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetQuestEncouragementCell(questId);
}

int Script::GetStepSum(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetStepSum(questId);
}

int Script::GetStepAimNum(int questId, int stepId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->GetStepAimNum(questId, stepId);
}

int Script::FindRoleIsInSystem(int questId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->FindRoleIsInSystem(questId);
}

int Script::FindRoleStepIsInSystem(int questId, int stepId)
{
	CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return EXEC_ERROR;
	return player->FindRoleStepIsInSystem(questId, stepId);
}

int Script::GetQuestStateId(int questId)
{
    CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if(player==NULL)
        return EXEC_ERROR;
    return player->GetQuestStateId(questId);
}

const char* Script::GetQuestDisplayText(int questId,int state)
{
    CPlayer* player= dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
    if(player==NULL)
        return NULL;
    return player->GetQuestTextId(questId,state);
}