



#include "stdafx.h"
#include "SkillMgr.h"
#include "..\GameTest\GameTestDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSkillMgr::CSkillMgr()
{
	m_listSkillID.clear();
	m_listOccuSkill.clear();
}

CSkillMgr::~CSkillMgr()
{
	Release();
}

void	CSkillMgr::Release()
{
}

bool CSkillMgr::AddSkill(DWORD dwID)
{
	list<tagPlayerSkill>::iterator it = m_listSkillID.begin();
	for(; it != m_listSkillID.end(); ++it )
	{
		if( (*it).dwID == dwID )
			return false;
	}

	tagPlayerSkill stSkill;
	stSkill.dwID = dwID;

	m_listSkillID.push_back(stSkill);
	return true;
}

void CSkillMgr::AddOccuSkill(DWORD dwID)
{
	list<DWORD>::iterator it = m_listOccuSkill.begin();
	for(; it != m_listOccuSkill.end(); ++it )
	{
		if( (*it) == dwID )
			return;
	}

	m_listOccuSkill.push_back(dwID);
}

void CSkillMgr::RemoveSkill(DWORD dwID)
{
	for (list<tagPlayerSkill>::iterator it = m_listSkillID.begin(); it != m_listSkillID.end(); ++it )
	{
		if( it->dwID == dwID )
		{
			m_listSkillID.erase(it);
			return;
		}
	}
}

void CSkillMgr::RemoveOccuSkill(DWORD dwID)
{
	m_listOccuSkill.remove(dwID);
}

BOOL CSkillMgr::QueryOccuSkill(DWORD dwID)
{
	for (list<DWORD>::iterator it = m_listOccuSkill.begin(); it != m_listOccuSkill.end(); ++it )
	{
		if( (*it) == dwID )
			return TRUE;
	}
	return FALSE;
}

BOOL CSkillMgr::QuerySkill(DWORD dwID)
{
	for (list<tagPlayerSkill>::iterator it = m_listSkillID.begin(); it != m_listSkillID.end(); ++it )
	{
		if( it->dwID == dwID )
			return TRUE;
	}
	return FALSE;
}

tagPlayerSkill*	CSkillMgr::GetSkill(DWORD dwID)
{
	for (list<tagPlayerSkill>::iterator it = m_listSkillID.begin(); it != m_listSkillID.end(); ++it )
	{
		tagPlayerSkill* pSkill = &(*it);
		if( pSkill && pSkill->dwID == dwID )
			return pSkill;
	}
	return NULL;
}

void CSkillMgr::ClearSkillList()
{
	m_listSkillID.clear();
}

void CSkillMgr::ClearOccuSkill()
{
	m_listOccuSkill.clear();
}

void CSkillMgr::CodeToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, (long)m_listSkillID.size());
	list<tagPlayerSkill>::iterator SkillIter;
	for(SkillIter=m_listSkillID.begin();SkillIter!=m_listSkillID.end();SkillIter++)
	{
		_AddToByteArray(pByteArray, (BYTE*)&(*SkillIter), (long)sizeof(tagPlayerSkill));
	}
}

void CSkillMgr::DecodeFromDataBlock(DBReadSet& dbRead)
{
#pragma region
	if (CGameTestDlg::m_iProjName == CGameTestDlg::PROJ_XINGCHEN)
	{
		long lNum = dbRead.GetLongFromByteArray();
		dbRead.GetWordFromByteArray();

		tagPlayerSkill stSkill;
		for(int i=0;i<lNum;i++)
		{
			stSkill.dwID = dbRead.GetDwordFromByteArray();
			/*stSkill.wLevel = */dbRead.GetWordFromByteArray();
			/*stSkill.dwRestoreTime = */dbRead.GetDwordFromByteArray();
			/*stSkill.wAffectRangeMin = */dbRead.GetWordFromByteArray();
			/*stSkill.wAffectRangeMax = */dbRead.GetWordFromByteArray();
			/*stSkill.wCostMP = */dbRead.GetWordFromByteArray();
			/*DWORD	dwTime = */dbRead.GetDwordFromByteArray();
			/*if(stSkill.dwRestoreTime > 0)
			{
			stSkill.dwCoolDownStartTime = timeGetTime() - dwTime;
			}*/

			//只要战斗技能
			if(stSkill.dwID < SKILL_DEPUTY_OCCU)
			{
				m_listSkillID.push_back(stSkill);
				IsAddOccupList(stSkill.dwID);
			}
		}
	}
#pragma endregion

#pragma region
	else if(CGameTestDlg::m_iProjName == CGameTestDlg::PROJ_PANLONG)
	{
		long lNum = dbRead.GetLongFromByteArray();
		dbRead.GetWordFromByteArray();

		tagPlayerSkill stSkill;
		for(int i=0;i<lNum;i++)
		{
			stSkill.dwID = dbRead.GetDwordFromByteArray();
			/*stSkill.wLevel = */dbRead.GetWordFromByteArray();
			/*stSkill.dwRestoreTime = */dbRead.GetDwordFromByteArray();
			/*stSkill.wAffectRangeMin = */dbRead.GetFloatFromByteArray();
			/*stSkill.wAffectRangeMax = */dbRead.GetFloatFromByteArray();
			/*stSkill.wCostMP = */dbRead.GetWordFromByteArray();
			/*DWORD	dwTime = */dbRead.GetDwordFromByteArray();
			/*if(stSkill.dwRestoreTime > 0)
			{
			stSkill.dwCoolDownStartTime = timeGetTime() - dwTime;
			}*/

			//只要战斗技能
			if(stSkill.dwID < SKILL_DEPUTY_OCCU)
			{
				m_listSkillID.push_back(stSkill);
				IsAddOccupList(stSkill.dwID);
			}
		}
	}
#pragma endregion
}


// 指定的技能是否能触使职业链的变化
bool CSkillMgr::IsAddOccupList(DWORD dwSkillID)
{
	bool bRt = false;
	DWORD dwOcc = OCC_Max;
	switch(dwSkillID)
	{
	case SKILL_BASE_NOVICE_OCC:				// 初新者
		dwOcc = OCC_Novice;
		bRt = true;
		break;
	case SKILL_BASE_FIGHTER_OCC:				// 战士
		dwOcc = OCC_Fighter;
		bRt = true;
		break;
	case SKILL_BASE_RABBI_OCC:					// 法师
		dwOcc = OCC_Mystic;
		bRt = true;
		break;
	case SKILL_BASE_FAQUIR_OCC:					// 行者
		dwOcc = OCC_Rogue;
		bRt = true;
		break;
	case SKILL_BASE_KNIGHT_OCC:					// 骑士
		dwOcc = OCC_Knight;
		bRt = true;
		break;
	case SKILL_BASE_WARRIOR_OCC:				// 斗士
		dwOcc = OCC_Warrior;
		bRt = true;
		break;
	case SKILL_BASE_SCOUT_OCC:					// 斥候
		dwOcc = OCC_Scout;
		bRt = true;
		break;
	case SKILL_BASE_CRAFTSMAN_OCC:				// 艺人
		dwOcc = OCC_Craftsman;
		bRt = true;
		break;
	case SKILL_BASE_WARLOCK_OCC:				//术士
		dwOcc = OCC_Warlock;
		bRt = true;
		break;
	case SKILL_BASE_CLERIC_OCC:					//牧师
		dwOcc = OCC_Cleric;
		bRt = true;
		break;
	case SKILL_BASE_PALADIN_OCC:				//圣骑士
		dwOcc = OCC_Paladin;
		bRt = true;
		break;
	case SKILL_BASE_DARKKINGHT_OCC:				//暗骑士
		dwOcc = OCC_DarkKnight;
		bRt = true;
		break;
	case SKILL_BASE_GLADIATOR_OCC:				//角斗士
		dwOcc = OCC_Gladiator;
		bRt = true;
		break;
	case SKILL_BASE_BERSERKER_OCC:				//狂战士
		dwOcc = OCC_Berserker;
		bRt = true;
		break;
	case SKILL_BASE_RANGER_OCC:					//巡守
		dwOcc = OCC_Ranger;
		bRt = true;
		break;
	case SKILL_BASE_ASSASSIN_OCC:				//刺客
		dwOcc = OCC_Assassin;
		bRt = true;
		break;
	case SKILL_BASE_BLADEDANCER_OCC:			//剑舞者
		dwOcc = OCC_BladeDancer;
		bRt = true;
		break;
	case SKILL_BASE_BARD_OCC:					//吟游诗人
		dwOcc = OCC_Bard;
		bRt = true;
		break;
	case SKILL_BASE_SORCERER_OCC:				//巫师
		dwOcc = OCC_Sorcerer;
		bRt = true;
		break;
	case SKILL_BASE_NECROMANCER_OCC:			//亡灵巫师
		dwOcc = OCC_Necromancer;
		bRt = true;
		break;
	case SKILL_BASE_BISHOP_OCC:					//主教
		dwOcc = OCC_Bishop;
		bRt = true;
		break;
	case SKILL_BASE_PROPHET_OCC:				//贤者(先知)
		dwOcc = OCC_Prophet;
		bRt = true;
		break;
	default :
		//已初始化得值传入
		break;
	}
	// 职业合法
	if(dwOcc >= OCC_Novice && dwOcc < OCC_Max)
	{ m_listOccuSkill.push_back(dwOcc); }
	return bRt;
}